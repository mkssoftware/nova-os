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

/* Physische Adresse des AKTUELL per CR3 aktiven Page-Directory (siehe
   paging_switch_address_space weiter unten - vor dem ersten Wechsel
   identisch mit paging_kernel_directory_physical()). Dient als Page-
   Table-Root fuer NPSPEC-MEMORY-PAGETABLE-0001. */
uint32_t paging_root_physical(void);

/* ===================================================================== */
/* Mehrere, echte Hardware-Adressraeume (Page-Directory pro Prozess) -    */
/* siehe ring3.c fuer die erste tatsaechliche Nutzung.                    */
/* ===================================================================== */

/* Legt einen NEUEN, eigenstaendigen Adressraum an: ein frisches Page-
   Directory, das als Ausgangspunkt die PDE-Werte des urspruenglichen
   Kernel-Basis-Adressraums uebernimmt (siehe paging_kernel_directory_
   physical) - dadurch sind Kernel-/Identity-/APIC-/Framebuffer-Mappings
   von Anfang an vorhanden, zunaechst ueber DIESELBEN, gemeinsam genutzten
   Seitentabellen wie der Kernel-Basis-Adressraum (billig: kein Kopieren
   ganzer Tabellen). paging_map_page_into() privatisiert eine geerbte
   Tabelle automatisch beim ersten Schreibzugriff genau einmal (Copy-on-
   Write auf Seitentabellen-Granularitaet) - Aenderungen in diesem neuen
   Adressraum beeinflussen daher NIE den Kernel-Basis-Adressraum oder
   andere, bereits existierende Adressraeume. 0 = Erfolg
   (*out_directory_phys gesetzt - zugleich die physische UND, da innerhalb
   der PMM_BOOTSTRAP_IDENTITY_LIMIT-Region liegend, die virtuelle Adresse
   des neuen Directory), -1 wenn kein freier PMM-Rahmen mehr verfuegbar
   ist oder Paging noch nicht aktiv ist. */
int paging_create_address_space(uint32_t *out_directory_phys);

/* Wie paging_map_page(), aber fuer einen EXPLIZIT angegebenen, nicht
   notwendigerweise gerade per CR3 aktiven Adressraum (directory_phys, von
   paging_create_address_space()) - funktioniert OHNE CR3-Wechsel, da
   directory_phys selbst und alle davon referenzierten/neu allokierten
   Seitentabellen innerhalb der Identity-Map liegen und daher unabhaengig
   vom aktiven CR3 direkt adressierbar sind. 0 = Erfolg, -1 wenn kein
   freier PMM-Rahmen mehr verfuegbar ist. NICHT fuer den Kernel-Basis-
   Adressraum selbst aufrufen - dafuer weiterhin paging_map_page(). */
int paging_map_page_into(uint32_t directory_phys, uint32_t virtual_page,
                          uint32_t physical_page, uint32_t flags);

/* Echter Hardware-Adressraumwechsel: laedt CR3 mit directory_phys UND
   aktualisiert den internen "aktiver Adressraum"-Zustand, den
   paging_map_page()/_unmap_page()/_query_page()/_protect_page()/
   paging_root_physical() ab sofort verwenden - diese wirken also immer
   auf den GERADE aktiven Adressraum, unabhaengig davon, welcher das ist. */
void paging_switch_address_space(uint32_t directory_phys);

/* Physische Adresse des urspruenglichen, von paging_initialize() beim
   Boot angelegten Kernel-Basis-Adressraums - unabhaengig davon, welcher
   Adressraum gerade per CR3 aktiv ist (anders als paging_root_physical()).
   Fuer Code, das gezielt in den Kernel-Basis-Adressraum zurueckwechseln
   will, und intern fuer paging_create_address_space()/
   paging_map_page_into()'s Privatisierungs-Logik. */
uint32_t paging_kernel_directory_physical(void);

#endif
