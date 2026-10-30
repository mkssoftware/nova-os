/* NovaOS C-Kernel - Nova Demand Paging (NPSPEC-MEMORY-DEMANDPAGING-0001).
   Loest einen Fault auf einer per vas_reserve_region() reservierten, aber
   noch nicht per vas_map() unterlegten virtuellen Seite auf: allokiert
   einen physischen Frame (frame_alloc), nullt ihn (Anforderung #5 - keine
   Daten eines vorherigen Sicherheitskontexts duerfen sichtbar werden) und
   mapped ihn dann an exakt der fehlerhaften Adresse.

   BEWUSSTE SCOPE-REDUKTION: dieses Modul ist als Funktion bereitgestellt,
   die ein zukuenftiger #PF-ISR-Handler aufrufen koennte - `exception.c`s
   `exception_manager_dispatch` ist laut Migrations-Status noch NICHT an
   die ISR angebunden, es gibt also noch keinen echten Hardware-Page-
   Fault-Pfad, der demandpage_fault() automatisch aufruft. Der Selbsttest
   ruft die Funktion deshalb direkt auf (simulierter Fault). "Gleichzeitige
   Faults derselben Seite" (Anforderung #7) ist in diesem kooperativen,
   noch nicht praeemptiv-Fault-fertigen Single-Core-Pfad trivial erfuellt
   (keine echte Parallelitaet an dieser Stelle). NUMA-bevorzugte
   Frame-Auswahl (Anforderung #8) ist ueber einen optionalen
   `preferred_numa_node`-Parameter moeglich, wirkt aber nur, solange der
   PMM ueberhaupt NUMA-Lokalitaet kennt (aktuell immer PMM_NUMA_UNKNOWN,
   siehe pmm.h). Interaktion mit Memory Pressure/OOM (Anforderung #9) ist
   Folgearbeit (haengt an den noch nicht portierten PRESSURE/RECLAIM-
   Modulen). Hard-Realtime-Pinning (Anforderung #10) und Prefetching
   (Anforderung #11) sind nicht umgesetzt. */
#ifndef NOVA_DEMANDPAGE_H
#define NOVA_DEMANDPAGE_H

#include <stdint.h>

#define DEMANDPAGE_RESOLVED       0
#define DEMANDPAGE_ALREADY_MAPPED 1
#define DEMANDPAGE_INVALID        2 /* keine Region / Guard-Region an dieser Adresse */
#define DEMANDPAGE_OOM            3 /* Region gueltig, aber kein physischer Frame frei */

void demandpage_initialize(void);
int demandpage_self_test(void); /* 0 = ok */

/* Loest einen Demand-Fault fuer die Seite auf, die faulting_address
   enthaelt. permissions wird NUR beim allerersten Mapping dieser Seite
   verwendet (NPSPEC-MEMORY-DEMANDPAGING-0001 #4: Validierung vor
   Bereitstellung). Gibt einen der DEMANDPAGE_*-Codes zurueck. */
int demandpage_fault(uint32_t vas_id, uint32_t faulting_address, uint32_t permissions,
                      uint32_t preferred_numa_node);

uint32_t demandpage_fault_count(void);
uint32_t demandpage_resolved_count(void);
uint32_t demandpage_invalid_count(void);

#endif
