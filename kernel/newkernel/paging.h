/* NovaOS C-Kernel - Virtueller Bootstrap-Speichermanager (Port aus
   memory32.inc, ADR-2002 / NPSPEC-KERNEL-0007). Aktiviert Paging mit
   Identity-Mapping fuer Kernel/Stack/BIB/Heap (0..16 MiB), APIC-MMIO und
   den linearen Framebuffer. */
#ifndef NOVA_PAGING_H
#define NOVA_PAGING_H

#include <stdint.h>

#define PAGING_PAGE_PRESENT 0x001u
#define PAGING_PAGE_WRITE   0x002u
#define PAGING_PAGE_USER    0x004u

/* Allokiert das Page-Directory, baut die Identity-Mappings auf und aktiviert
   Paging (CR3/CR0.PG). 0 = Erfolg. ACHTUNG: schlaegt ein Mapping fehl,
   bleibt Paging deaktiviert (kein Absturzrisiko durch halb aktiviertes
   Paging). */
int paging_initialize(void);
int paging_self_test(void); /* 0 = ok */

/* EAX/EDX/EBX-Aequivalent aus dem alten ASM: virtuelle Seite, physische
   Seite, Flags. 0 = Erfolg. */
int paging_map_page(uint32_t virtual_page, uint32_t physical_page, uint32_t flags);

/* Entfernt eine Abbildung (PTE auf 0, inkl. invlpg). Gibt den zugehoerigen
   physischen Frame NICHT automatisch frei (NPSPEC-MEMORY-PAGETABLE-0001 #8).
   0 = Erfolg (auch wenn die Seite bereits nicht gemappt war), -1 = keine
   Seitentabelle fuer diesen Bereich vorhanden. */
int paging_unmap_page(uint32_t virtual_page);

/* Liest die aktuelle Abbildung einer virtuellen Seite aus, ohne sie zu
   aendern. 0 = Erfolg (out_physical/out_flags gesetzt), -1 = nicht gemappt
   oder keine Seitentabelle vorhanden. */
int paging_query_page(uint32_t virtual_page, uint32_t *out_physical, uint32_t *out_flags);

/* Aendert die Flags einer bestehenden Abbildung, physischer Frame bleibt
   unveraendert (inkl. invlpg). 0 = Erfolg, -1 = nicht gemappt/keine
   Seitentabelle vorhanden. */
int paging_protect_page(uint32_t virtual_page, uint32_t new_flags);

/* Physische Adresse des (einzigen, kernelweit geteilten) Page-Directory -
   dient als Page-Table-Root fuer NPSPEC-MEMORY-PAGETABLE-0001. Bewusste
   Scope-Reduktion: dieser 32-Bit-Kernel nutzt noch EIN gemeinsames
   Page-Directory fuer alle Address Spaces (kein eigenes CR3 pro Prozess) -
   siehe vas.h fuer die Begruendung. */
uint32_t paging_root_physical(void);

#endif
