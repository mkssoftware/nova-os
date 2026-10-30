/* NovaOS C-Kernel - Nova Shared Memory Objects (NPSPEC-MEMORY-SHARED-
   0001). Ein Shared Memory Object (SMO) besitzt eine stabile ID,
   unabhaengig von jeder virtuellen Adresse (Anforderung #2), und haelt
   einen physischen Frame PRO Seite einzeln (ueber frame_alloc(),
   framealloc.h - ABSICHTLICH NICHT frame_alloc_contiguous(), siehe
   Scope-Hinweis unten). Teilnehmer binden das SMO per `shared_map()` in
   ihren eigenen VAS ein - jeder Teilnehmer kann eine eigene virtuelle
   Adresse UND eigene Permissions waehlen (Anforderung #4), ueber
   `vas_map_shared()` (vas.h, pro Seite aufgerufen), die den
   Referenzzaehler jeder Seite erhoeht (framealloc.h) statt neue Frames
   zu allokieren.

   BEWUSSTE SCOPE-REDUKTION: die SMO-Groesse ist auf SHARED_MAX_PAGES
   Seiten begrenzt. Die Backing-Frames muessen NICHT physisch
   zusammenhaengend sein (anders als ein frueherer Entwurf dieses Moduls,
   der frame_alloc_contiguous() nutzte - das schlug im echten, bereits
   fragmentierten Boot-Zustand regelmaessig fehl, siehe
   project_c_kernel_migration_status.md "Echter Boot-Bug" zu diesem
   Modul). "Gleichzeitige Demand Faults derselben Shared Page"
   (Anforderung #7) sind in diesem noch-kooperativen Single-Core-Kernel
   trivial konsistent (keine echte Parallelitaet). Realtime-Schutz vor
   Fault-/Reclaim-Latenzen (Anforderung #11) ist nicht umgesetzt (haengt
   an einem noch nicht vorhandenen Realtime-Pinning-Mechanismus). */
#ifndef NOVA_SHARED_H
#define NOVA_SHARED_H

#include <stdint.h>

#define SHARED_CAPACITY 8u
#define SHARED_MAX_ATTACH 8u
#define SHARED_MAX_PAGES 16u

void shared_initialize(void);
int shared_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_id gesetzt), -1 = ungueltige Groesse (0 oder >
   SHARED_MAX_PAGES Seiten) oder SHARED_CAPACITY/Frame-Pool erschoepft.
   Der Backing-Speicher wird vor Rueckgabe genullt. */
int shared_create(uint32_t size_bytes, uint32_t *out_id);

/* 0 = Erfolg, -1 = ungueltige ID oder noch aktive Teilnehmer (Anforderung
   #8 - ein SMO mit verbleibenden Mappings wird NICHT zerstoert). */
int shared_destroy(uint32_t id);

/* Bindet das SMO in vas_id an virtual_base ein (muss zuvor per
   vas_reserve_region() reserviert worden sein, exakt wie bei vas_map()).
   0 = Erfolg, -1 = ungueltige ID/W^X-Verstoss/keine passende Region/
   SHARED_MAX_ATTACH erschoepft. */
int shared_map(uint32_t id, uint32_t vas_id, uint32_t virtual_base, uint32_t permissions);

/* 0 = Erfolg, -1 = kein passendes aktives Mapping gefunden. */
int shared_unmap(uint32_t id, uint32_t vas_id, uint32_t virtual_base);

int shared_attach_count(uint32_t id, uint32_t *out_count);
int shared_page_count(uint32_t id, uint32_t *out_count);

#endif
