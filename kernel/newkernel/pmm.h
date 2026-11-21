/* NovaOS C-Kernel - Physischer Bootstrap-Speichermanager (Port aus
   memory32.inc, ADR-2001 / NPSPEC-KERNEL-0006). LIFO-Stack von 4KB-Rahmen,
   NUMA-getaggt (vorerst immer PMM_NUMA_UNKNOWN, bis ACPI/SRAT portiert ist). */
#ifndef NOVA_PMM_H
#define NOVA_PMM_H

#include <stdint.h>

#define PMM_PAGE_SIZE 4096u
/* GESCHICHTE dieser Konstante (wichtig, um den Fix unten richtig
   einzuordnen): 1024 Eintraege reichten nach allen Kernel-Selbsttests
   nicht mehr fuer die fuenf Ring-3-Desktop-Prozesse, daher auf 2048
   angehoben. WICHTIG (2026-11-20, per echtem Boot-Test widerlegt): ein
   spaeterer Versuch, NUR dieses Limit weiter auf 2304 zu erhoehen (ohne
   PMM_BOOTSTRAP_IDENTITY_LIMIT mit anzuheben), zeigte KEINE Wirkung -
   die Tabellenkapazitaet war schon vorher groesser als die Anzahl
   tatsaechlich identity-gemappter freier Rahmen unterhalb der alten
   16-MiB-Grenze. Eine groessere Tabelle allein kann nichts bringen, wenn
   der Rahmen-SCAN selbst (pmm_initialize(), siehe PMM_BOOTSTRAP_
   IDENTITY_LIMIT unten) gar nicht mehr findet.

   FIX (2026-11-21, project_pmm_capacity_increase.md/Nutzerfrage "kann
   der PMM nicht dynamisch sein?"): diesmal BEIDE Konstanten zusammen
   angehoben - architektonisch ist das sicher, weil weder pmm.c noch
   paging.c irgendwo ein fest dimensioniertes Array fuer die Identity-Map
   selbst haben (per Code-Review bestaetigt: paging_initialize()s
   Identity-Map-Schleife ruft schlicht paging_map_page() wiederholt auf,
   das bei Bedarf dynamisch neue Seitentabellen aus dem PMM-Pool holt -
   `pmm_frames[PMM_MAX_FRAMES]` in pmm.c ist das EINZIGE Array, das von
   dieser Konstante abhaengt). 16384 Eintraege = genug Kapazitaet fuer
   die vollen 64 MiB der neuen PMM_BOOTSTRAP_IDENTITY_LIMIT unten. */
#define PMM_MAX_FRAMES 16384u
#define PMM_NUMA_UNKNOWN 0xFFFFFFFFu

/* Obergrenze fuer alle von pmm_initialize() gesammelten Bootstrap-Rahmen.
   MUSS mit paging.c's Identity-Map-Grenze (dort PAGING_LOW_LIMIT, das
   diese Konstante uebernimmt statt sie erneut zu definieren) identisch
   bleiben: paging_initialize() bildet beim Boot ausschliesslich [0,
   PMM_BOOTSTRAP_IDENTITY_LIMIT) identisch ab. Ein PMM-Rahmen ausserhalb
   dieser Grenze waere beim ersten Zugriff ungemappt und wuerde (an dieser
   fruehen Bootstelle, lange vor einem installierten Seitenfehler-Handler)
   zu einem stillen Dreifachfehler/Haenger ohne Panic fuehren - genau das
   geschah real, als ein staerker wachsender statischer Kernel-Puffer
   pmm_reserved_end so weit anhob, dass die UEFI-Memory-Map-Sondierung
   (Eintraege werden NICHT nach Adresse sortiert durchlaufen, siehe
   pmm_initialize()) einzelne Rahmen aus einem spaeteren Eintrag oberhalb
   dieser Grenze zog (siehe feedback_bss_paging_limit.md). pmm_initialize()
   kappt seitdem JEDEN Eintrag hart auf diese Grenze, statt sich auf eine
   zufaellig guenstige Eintrags-Reihenfolge zu verlassen - DESHALB ist das
   blosse Anheben dieser EINEN Konstante (zusammen mit PMM_MAX_FRAMES
   oben) sicher: beide Subsysteme lesen garantiert denselben Wert, koennen
   nie mehr auseinanderdriften wie beim urspruenglichen Vorfall.

   2026-11-21 von 16 auf 64 MiB angehoben (siehe PMM_MAX_FRAMES-Kommentar
   oben) - die QEMU-Testkonfiguration dieses Projekts hat 256 MiB RAM
   (siehe die Test-Skripte, Parameter "-m 256M"), 64 MiB bleibt darin
   komfortabel. Boot-verifiziert: behebt sowohl den rohen Kapazitaetsmangel als auch (durch
   den dadurch viel groesseren freien Pool) die Fragmentierung, die trotz
   des NovaFS-Selbsttest-Leak-Fixes (feedback_novafs_selftest_pmm_leak.md)
   noch den Explorer-Spawn verhinderte. */
#define PMM_BOOTSTRAP_IDENTITY_LIMIT 0x04000000u

/* Liest die Speicherkarte aus dem (bereits via bib_parse() validierten) BIB
   und baut den Frame-Stack auf. 0 = Erfolg (mind. 2 Rahmen gefunden). */
int pmm_initialize(void);
int pmm_self_test(void); /* 0 = ok */

/* Gibt eine physische 4KB-Seite zurueck (0 = erschoepft). *out_numa_node
   erhaelt die NUMA-Lokalitaet der Seite. */
uint32_t pmm_alloc_page(uint32_t *out_numa_node);

/* Bevorzugt preferred_node, faellt kontrolliert auf eine beliebige Seite
   zurueck. *out_numa_node liefert immer die tatsaechliche Lokalitaet. */
uint32_t pmm_alloc_page_preferred(uint32_t preferred_node, uint32_t *out_numa_node);

/* Liefert ausschliesslich eine Seite mit passender NUMA-Lokalitaet, oder 0. */
uint32_t pmm_alloc_page_strict(uint32_t node, uint32_t *out_numa_node);

/* 0 = Erfolg, -1 = ungueltige/unausgerichtete Seite oder Stack voll. */
int pmm_free_page(uint32_t page);

uint32_t pmm_frame_count(void);

/* Sucht `count` physisch ZUSAMMENHAENGENDE freie Rahmen (fuer Puffer, die
   als EIN flacher Zeiger an einen Aufrufer ausserhalb des Paging-
   Mechanismus weitergegeben werden - z.B. ein Surface-Pixelpuffer, den
   der Compositor spaeter direkt per physischer Adresse dereferenziert,
   unabhaengig vom gerade aktiven CR3, siehe syscall.c GUI_OP_ATTACH_
   BUFFER). Da pmm_free_page() Rahmen in BELIEBIGER Reihenfolge wieder auf
   den Stack legt, ist `pmm_frames` NICHT mehr garantiert sortiert/
   zusammenhaengend, sobald irgendwo im Kernel schon einmal freigegeben
   wurde - diese Funktion durchsucht die Tabelle daher aktiv nach einem
   passenden Lauf (O(count * Tabellengroesse), nur fuer seltene, groessere
   Pufferallokationen gedacht, NICHT fuer den heissen 1-Seiten-Pfad). 0 =
   Erfolg (*out_first_page = niedrigste Adresse des zusammenhaengenden
   Laufs, alle `count` Rahmen aus dem freien Pool entfernt), -1 wenn kein
   passender Lauf existiert oder count ungueltig ist. */
int pmm_alloc_contiguous(uint32_t count, uint32_t *out_first_page);

#endif
