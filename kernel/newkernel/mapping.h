/* NovaOS C-Kernel - Nova Memory Mapping Semantics (NPSPEC-MEMORY-MAPPING-
   0001). Ergaenzt die bereits in vas.h vorhandene Mapping-Buchhaltung
   (Virtual Range + Backing-Frame + Permissions) um die in dieser Spec
   geforderte MAPPING-TYP-Semantik (Anonymous/Private/Shared/Object-
   backed/Device) und Cache-Policy, ohne vas.c's Kernfunktionen zu
   duplizieren - mapping.c ruft fuer die eigentliche Seitenverwaltung
   weiterhin vas_map()/vas_unmap() auf und fuehrt daneben eine eigene,
   nach (vas_id, virtual_base) indizierte Metadaten-Tabelle.

   BEWUSSTE SCOPE-REDUKTION: Object-backed Mappings (Anforderung #5)
   werden nur als opaque `backing_object_id` referenziert - es gibt noch
   keine echte Backing-Object-Abstraktion (haengt an einem spaeteren
   Object-/Filesystem-Mapping-Modul). Teilweises Unmapping (Anforderung #8,
   "Teilweises Unmapping... MUESSEN konsistente Mapping-Metadaten
   erhalten") ist NICHT implementiert - `mapping_destroy` entfernt immer
   das GESAMTE Mapping; ein echter Split einer bestehenden Mapping in zwei
   verbleibende Teile ist Folgearbeit. Device-Mappings mappen eine vom
   Aufrufer vorgegebene physische Adresse DIREKT (kein frame_alloc, da
   Geraetespeicher nicht vom PMM verwaltet wird) und werden beim Entfernen
   NICHT per frame_unref freigegeben (kein Eigentum am Speicher). */
#ifndef NOVA_MAPPING_H
#define NOVA_MAPPING_H

#include <stdint.h>

#define MAPPING_TRACK_CAPACITY 32u

#define MAPPING_TYPE_ANONYMOUS 0u
#define MAPPING_TYPE_PRIVATE   1u
#define MAPPING_TYPE_SHARED    2u
#define MAPPING_TYPE_OBJECT    3u
#define MAPPING_TYPE_DEVICE    4u

#define MAPPING_CACHE_WRITEBACK    0u
#define MAPPING_CACHE_WRITETHROUGH 1u
#define MAPPING_CACHE_UNCACHED     2u

void mapping_initialize(void);
int mapping_self_test(void); /* 0 = ok */

/* Anonymous/Private/Shared/Object-backed Mapping: allokiert Speicher via
   vas_map() und legt zusaetzlich den Mapping-Typ + (fuer OBJECT) die
   Backing-Object-ID ab. Verweigert Anforderung #6: `permissions` DARF bei
   OBJECT-backed Mappings die `backing_permissions`-Maske nicht
   ueberschreiten. 0 = Erfolg, -1 = Ablehnung/voll. */
int mapping_create(uint32_t vas_id, uint32_t virtual_base, uint32_t size,
                    uint32_t permissions, uint32_t map_type,
                    uint32_t backing_object_id, uint32_t backing_permissions);

/* Device-Mapping: bildet physical_base DIREKT ab (kein frame_alloc), mit
   expliziter Cache-Policy (Anforderung #11). 0 = Erfolg, -1 = Ablehnung. */
int mapping_create_device(uint32_t vas_id, uint32_t virtual_base, uint32_t physical_base,
                           uint32_t size, uint32_t permissions, uint32_t cache_policy);

/* Entfernt ein komplettes Mapping (siehe Scope-Hinweis: kein Teil-Unmap).
   0 = Erfolg, -1 = nicht gefunden. */
int mapping_destroy(uint32_t vas_id, uint32_t virtual_base);

int mapping_type_of(uint32_t vas_id, uint32_t virtual_base, uint32_t *out_type);
int mapping_cache_policy_of(uint32_t vas_id, uint32_t virtual_base, uint32_t *out_policy);
uint32_t mapping_tracked_count(void);

#endif
