/* NovaOS C-Kernel - Nova TLB Management (NPSPEC-MEMORY-TLB-0001). Lokale
   Invalidierung (einzelne Seite, Bereich, Vollflush) + Diagnose-Zaehler
   ueber den vorhandenen x86-32-Bootstrap-Pager.

   BEWUSSTE SCOPE-REDUKTION (dokumentiert, da hier eine echte, im Code
   bereits sichtbare Grenze existiert, keine reine Bequemlichkeit): Remote-
   TLB-Shootdown (Anforderungen #4/#5) ist NICHT implementierbar, solange
   die Application Processors so arbeiten, wie `smp.h` es selbst
   dokumentiert - jeder AP idled DAUERHAFT MIT DEAKTIVIERTEN INTERRUPTS
   (IF=0) und kann deshalb grundsaetzlich KEINEN IPI entgegennehmen (auch
   keinen FIXED-IPI-Shootdown ueber den vorhandenen LAPIC). Ein Shootdown
   an einen AP wuerde also niemals zugestellt werden - `tlb_shootdown()`
   gibt deshalb ehrlich -2 ("nicht zustellbar") zurueck statt eine
   Zustellung zu simulieren. Address-Space-Identifier (Anforderung #7, z.B.
   x86 PCID) sind nicht genutzt - dieser Kernel hat ohnehin nur EIN
   gemeinsames Page-Directory (siehe vas.h), ASIDs wuerden hier keinen
   Vorteil bringen. Sobald APs echte Scheduler-Teilnahme bekommen (siehe
   smp.h-Kommentar) und damit unterbrechbar werden, kann `tlb_shootdown()`
   auf einen echten IPI-basierten Mechanismus umgestellt werden. */
#ifndef NOVA_TLB_H
#define NOVA_TLB_H

#include <stdint.h>

void tlb_initialize(void);
int tlb_self_test(void); /* 0 = ok */

/* Invalidiert genau eine virtuelle Seite lokal (invlpg). */
void tlb_invalidate_page(uint32_t virtual_page);

/* Invalidiert einen zusammenhaengenden virtuellen Bereich seitenweise
   (begrenzt auf die tatsaechlich betroffenen Seiten, Anforderung #3). */
void tlb_invalidate_range(uint32_t virtual_base, uint32_t size);

/* Vollstaendiger lokaler Flush (CR3-Reload). */
void tlb_flush_all(void);

/* Siehe Scope-Hinweis oben: -2 = Ziel-AP ist dauerhaft interruptfrei und
   kann keinen Shootdown-IPI empfangen (ehrlicher Fehlercode statt
   stiller No-Op). -1 = ungueltiger AP-Index. */
int tlb_shootdown(uint32_t ap_index, uint32_t virtual_page);

uint32_t tlb_invalidation_count(void);
uint32_t tlb_flush_count(void);

#endif
