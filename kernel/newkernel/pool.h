/* NovaOS C-Kernel - Nova Memory Pools (NPSPEC-MEMORY-POOL-0001). Explizit
   begrenzte, fixed-size physische Frame-Pools mit definiertem Fallback-
   Verhalten und optionalem Emergency-Reserve ueber dem bestehenden
   framealloc.h/pagetable.h.

   BEWUSSTE SCOPE-REDUKTION: Bloecke sind in diesem Port immer genau
   PMM_PAGE_SIZE (4096 Bytes) gross - echte variable Block-Groessen
   innerhalb eines Pools (NPSPEC erlaubt das implizit ueber "Fixed-Size
   Pools") sind Folgearbeit; fuer die meisten Kernel-internen Pool-
   Anwendungsfaelle (Seiten, Deskriptoren) reicht Seitengranularitaet.
   NUMA-/CPU-Lokalitaet (Anforderung #9) ist nicht umgesetzt (PMM liefert
   aktuell immer PMM_NUMA_UNKNOWN). Concurrency-Synchronisation
   (Anforderung #10) ist in diesem noch-Single-Core-kooperativen Kernel
   trivial erfuellt (kein echtes Locking noetig) - sobald mehrere Kerne
   aktiv Pools benutzen, wird echtes Locking Folgearbeit. */
#ifndef NOVA_POOL_H
#define NOVA_POOL_H

#include <stdint.h>

#define POOL_CAPACITY 8u
#define POOL_MAX_BLOCKS 32u

#define POOL_FALLBACK_NONE 0u
#define POOL_FALLBACK_HEAP 1u

#define POOL_FLAG_CRITICAL  0x01u /* MUSS POOL_FALLBACK_NONE nutzen */
#define POOL_FLAG_EMERGENCY 0x02u /* Reserve wird bei pool_create() sofort befuellt,
                                     nur ueber pool_alloc_emergency() entnehmbar */

void pool_initialize(void);
int pool_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_id gesetzt), -1 = ungueltige Kombination
   (POOL_FLAG_CRITICAL + POOL_FALLBACK_HEAP) oder POOL_CAPACITY/Emergency-
   Vorbefuellung fehlgeschlagen. */
int pool_create(uint32_t capacity, uint32_t fallback, uint32_t flags, uint32_t *out_id);

/* Gibt alle Reserve-Frames eines Emergency-Pools zurueck und den Slot
   frei. 0 = Erfolg, -1 = ungueltige ID oder noch ausstehende
   Allokationen. */
int pool_destroy(uint32_t id);

/* Normale Allokation. Verweigert auf Emergency-Pools (Anforderung #8).
   0 = Erfolg (*out_page gesetzt, genullt), -1 = Kapazitaet erschoepft
   (und Fallback war POOL_FALLBACK_NONE oder schlug ebenfalls fehl) oder
   Pool ist ein Emergency-Pool. */
int pool_alloc(uint32_t id, uint32_t *out_page);

/* Entnimmt EXKLUSIV aus dem Emergency-Reserve eines POOL_FLAG_EMERGENCY-
   Pools. 0 = Erfolg, -1 = kein Emergency-Pool oder Reserve erschoepft. */
int pool_alloc_emergency(uint32_t id, uint32_t *out_page);

/* 0 = Erfolg, -1 = ungueltige ID/Seite nicht aus diesem Pool. */
int pool_free(uint32_t id, uint32_t page);

int pool_usage(uint32_t id, uint32_t *out_used, uint32_t *out_peak, uint32_t *out_failures);

#endif
