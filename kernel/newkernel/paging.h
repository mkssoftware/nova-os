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

#endif
