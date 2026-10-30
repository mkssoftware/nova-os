/* NovaOS C-Kernel - Nova Virtual Address Space (NPSPEC-MEMORY-VAS-0001).
   Logische Adressraum-Abstraktion ueber pagetable.h/framealloc.h: jeder VAS
   verwaltet reservierte virtuelle Regionen (Code/Data/Heap/Shared/MMap/
   Stack/Reserved) und darin liegende Mappings (virtueller Bereich ->
   physischer Frame, mit Zugriffsrechten).

   BEWUSSTE SCOPE-REDUKTION (gilt fuer vas.c, pagetable.c, framealloc.c als
   zusammengehoerigen ersten NPSPEC-MEMORY-Batch, 2026-10-30): dieser
   32-Bit-Bootstrap-Kernel hat noch KEINEN echten Prozess-Adressraumwechsel
   (kein per-Prozess-CR3, siehe paging.c/process.c) - alle VAS-Instanzen
   teilen sich das EINE globale Page-Directory aus paging_initialize().
   Diese Portierung bildet deshalb die VOLLE NPSPEC-Buchhaltungsebene ab
   (AddressSpaceID/Owner/Regionen/Mappings/Permissions/Lifecycle/
   Introspection, Anforderungen #1,#3,#4,#7,#11,#12), nutzt dabei aber
   fuer die tatsaechliche Hardware-Abbildung das gemeinsame Page-Directory
   (Anforderung #9 bewusst nur teilweise erfuellt: die VAS-Abstraktion
   HAENGT nicht an einer Page-Table-Struktur, aber es gibt noch keine
   echte Hardware-Isolation zwischen zwei VAS - echte Isolation ist erst
   durch einen spaeteren per-Prozess-Page-Directory-Umbau erreichbar).
   W^X (Anforderung #8) wird bereits jetzt echt durchgesetzt (vas_map
   verweigert WRITE+EXECUTE gemeinsam, sofern nicht explizit per
   VAS_PERM_FORCE_WX autorisiert). Guard Regions (Anforderung #7) sind als
   reservierte, NIE gemappte Regionen abgebildet - ein echter Fault bei
   Zugriff setzt den noch fehlenden Page-Fault-Handler voraus (exception.c)
   und ist deshalb hier nur Buchhaltung, kein Hardware-Trap. */
#ifndef NOVA_VAS_H
#define NOVA_VAS_H

#include <stdint.h>

#define VAS_CAPACITY 8u
#define VAS_REGION_CAPACITY 16u
#define VAS_MAPPING_CAPACITY 32u

#define VAS_STATE_FREE   0u
#define VAS_STATE_ACTIVE 1u
#define VAS_STATE_DYING  2u

#define VAS_TYPE_KERNEL  0u
#define VAS_TYPE_PROCESS 1u
#define VAS_TYPE_COMPAT  2u
#define VAS_TYPE_SHARED  3u
#define VAS_TYPE_DEVICE  4u

#define VAS_REGION_CODE     0u
#define VAS_REGION_DATA     1u
#define VAS_REGION_HEAP     2u
#define VAS_REGION_SHARED   3u
#define VAS_REGION_MMAP     4u
#define VAS_REGION_STACK    5u
#define VAS_REGION_RESERVED 6u
#define VAS_REGION_GUARD    7u

#define VAS_PERM_READ        0x01u
#define VAS_PERM_WRITE       0x02u
#define VAS_PERM_EXEC        0x04u
#define VAS_PERM_USER        0x08u
#define VAS_PERM_FORCE_WX    0x10u /* explizite W^X-Ausnahme, siehe vas.h-Kopfkommentar */

typedef struct {
    uint32_t base;
    uint32_t size;
    uint32_t type;
    uint32_t used; /* 0 = freier Slot */
} vas_region_t;

typedef struct {
    uint32_t virtual_base;
    uint32_t size;
    uint32_t permissions;
    uint32_t physical_base; /* nur gueltig, wenn used != 0 */
    uint32_t owns_frames;   /* 0 = fixed/Device-Mapping, kein frame_unref beim Entfernen */
    uint32_t used;
} vas_mapping_t;

typedef struct {
    uint32_t id; /* 1-basiert, 0 = freier Slot */
    uint32_t owner;
    uint32_t type;
    uint32_t state;
    uint32_t generation;
    vas_region_t regions[VAS_REGION_CAPACITY];
    vas_mapping_t mappings[VAS_MAPPING_CAPACITY];
    uint32_t fault_count;
} vas_record_t;

void vas_initialize(void);
int vas_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_id gesetzt), -1 = VAS_CAPACITY erschoepft. */
int vas_create(uint32_t owner, uint32_t type, uint32_t *out_id);

/* Gibt alle Mappings (physische Frames via frame_unref) und Regionen eines
   VAS frei und markiert den Slot als frei. 0 = Erfolg, -1 = ungueltige ID. */
int vas_destroy(uint32_t id);

/* Reserviert einen virtuellen Bereich OHNE physische Unterlegung.
   0 = Erfolg, -1 = ungueltige ID/Ueberlappung/Regionen-Tabelle voll. */
int vas_reserve_region(uint32_t id, uint32_t base, uint32_t size, uint32_t region_type);

/* Legt ein Mapping an: allokiert einen physischen Frame (frame_alloc) und
   bildet ihn via pagetable_map() ab. Erzwingt W^X, ausser
   VAS_PERM_FORCE_WX ist gesetzt. Erlaubt nur innerhalb einer zuvor per
   vas_reserve_region() reservierten, NICHT-GUARD-Region.
   0 = Erfolg, -1 = ungueltige ID/W^X-Verstoss/keine passende Region/voll. */
int vas_map(uint32_t id, uint32_t virtual_base, uint32_t size, uint32_t permissions);

/* Aendert die Zugriffsrechte eines bestehenden Mappings (physischer Frame
   bleibt gleich). Erzwingt W^X wie vas_map(), ausser VAS_PERM_FORCE_WX ist
   gesetzt. 0 = Erfolg, -1 = Mapping nicht gefunden/W^X-Verstoss. */
int vas_protect(uint32_t id, uint32_t virtual_base, uint32_t new_permissions);

/* Wie vas_map(), nutzt aber eine vom Aufrufer vorgegebene physische
   Adresse DIREKT statt frame_alloc() - fuer Device/MMIO-Speicher, der
   nicht vom PMM verwaltet wird (siehe mapping.h). Das resultierende
   Mapping wird bei vas_unmap()/vas_destroy() NICHT per frame_unref
   freigegeben. 0 = Erfolg, -1 = W^X-Verstoss/keine passende Region/voll. */
int vas_map_fixed(uint32_t id, uint32_t virtual_base, uint32_t physical_base,
                   uint32_t size, uint32_t permissions);

/* Mapped einen BEREITS EXISTIERENDEN, physischen Frame (z.B. aus einem
   Shared Memory Object, siehe shared.h) - erhoeht dessen Referenzzaehler
   per frame_ref() (statt frame_alloc()) fuer jede gemappte Seite. Im
   Gegensatz zu vas_map_fixed() WIRD der Frame beim Entfernen des
   Mappings per frame_unref() wieder freigegeben (balanciert den
   frame_ref() dieses Aufrufs) - fuer echtes Referenzzaehl-basiertes
   Shared Memory. 0 = Erfolg, -1 = W^X-Verstoss/keine passende Region/voll. */
int vas_map_shared(uint32_t id, uint32_t virtual_base, uint32_t physical_base,
                    uint32_t size, uint32_t permissions);

/* Entfernt ein Mapping (pagetable_unmap + frame_unref). Physischer Speicher
   wird erst beim Erreichen von Refcount 0 freigegeben (NPSPEC-MEMORY-
   VAS-0001 "Unmap != Free Physical Memory"). 0 = Erfolg, -1 = nicht
   gefunden. */
int vas_unmap(uint32_t id, uint32_t virtual_base);

/* 0 = Erfolg (*out_permissions gesetzt), -1 = Mapping nicht gefunden. */
int vas_mapping_permissions(uint32_t id, uint32_t virtual_base, uint32_t *out_permissions);

/* Liefert den Typ der Region, die virtual_address enthaelt (fuer
   NPSPEC-MEMORY-DEMANDPAGING-0001: Validierung vor der Fault-Aufloesung).
   0 = Erfolg (*out_type gesetzt), -1 = keine Region an dieser Adresse. */
int vas_region_type_at(uint32_t id, uint32_t virtual_address, uint32_t *out_type);

/* Introspection (Anforderung #12). */
int vas_region_count(uint32_t id, uint32_t *out_count);
int vas_mapping_count(uint32_t id, uint32_t *out_count);
int vas_state_of(uint32_t id, uint32_t *out_state);

#endif
