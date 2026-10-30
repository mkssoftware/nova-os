/* NovaOS C-Kernel - Nova Page Table Management (NPSPEC-MEMORY-PAGETABLE-
   0001). Architekturabhaengige Abstraktion ueber den vorhandenen x86-32-
   Bootstrap-Pager (paging.h): Map/Unmap/Query/Root hinter einer allgemeinen
   Schnittstelle, die NICHT von einer festen Seitentabellen-Ebenenzahl
   abhaengt (NPSPEC-Anforderung #2 - paging.c bleibt das einzige Modul, das
   die konkrete zweistufige x86-32-Struktur kennt).

   Bewusste Scope-Reduktion (im Modul-Header dokumentiert, siehe vas.h):
   dieser Kernel hat noch EINEN gemeinsamen Page-Table-Root fuer alle
   Address Spaces (kein per-Prozess-CR3). Anforderung #3 ("jeder
   hardwareisolierte Address Space MUSS eine eindeutig bestimmbare Root
   besitzen") ist dadurch erfuellt, aber trivial (alle Roots sind
   identisch) - echte Isolation fehlt noch, bis ein per-Prozess-CR3-
   Mechanismus existiert. Anforderung #5/#6 (SMP-Sicherheit, keine
   teilweise aktualisierten Zustaende auf anderen CPUs) ist durch die
   bestehende Interrupt-Sperre waehrend Map/Unmap abgedeckt (kein echtes
   Multi-Core-Paging-Locking noetig, da der Kernel noch kein SMP-Paging-
   Contention hat). TLB-Shootdown auf andere CPUs (#10) ist nicht
   implementiert - invlpg wirkt nur lokal; mit echtem SMP-Scheduling waere
   ein IPI-basierter Shootdown noetig (siehe NPSPEC-MEMORY-TLB-0001). */
#ifndef NOVA_PAGETABLE_H
#define NOVA_PAGETABLE_H

#include <stdint.h>

#define PAGETABLE_PRESENT 0x001u
#define PAGETABLE_WRITE   0x002u
#define PAGETABLE_USER    0x004u

void pagetable_initialize(void);
int pagetable_self_test(void); /* 0 = ok */

/* Page-Table-Root des (aktuell einzigen) Address Spaces. */
uint32_t pagetable_root(void);

/* 0 = Erfolg. */
int pagetable_map(uint32_t virtual_page, uint32_t physical_page, uint32_t flags);

/* Entfernt eine Abbildung inkl. TLB-Invalidierung; gibt den physischen
   Frame NICHT frei (Anforderung #8). 0 = Erfolg. */
int pagetable_unmap(uint32_t virtual_page);

/* 0 = Erfolg (out_physical/out_flags gesetzt), -1 = nicht gemappt. */
int pagetable_query(uint32_t virtual_page, uint32_t *out_physical, uint32_t *out_flags);

/* Aendert die Zugriffsrechte einer bestehenden Abbildung OHNE den
   physischen Frame zu aendern (NPSPEC-MEMORY-PROTECTION-0001), inkl.
   TLB-Invalidierung. 0 = Erfolg, -1 = nicht gemappt. */
int pagetable_protect(uint32_t virtual_page, uint32_t new_flags);

uint32_t pagetable_map_count(void);
uint32_t pagetable_unmap_count(void);
uint32_t pagetable_protect_count(void);

#endif
