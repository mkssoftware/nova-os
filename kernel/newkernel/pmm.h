/* NovaOS C-Kernel - Physischer Bootstrap-Speichermanager (Port aus
   memory32.inc, ADR-2001 / NPSPEC-KERNEL-0006). LIFO-Stack von 4KB-Rahmen,
   NUMA-getaggt (vorerst immer PMM_NUMA_UNKNOWN, bis ACPI/SRAT portiert ist). */
#ifndef NOVA_PMM_H
#define NOVA_PMM_H

#include <stdint.h>

#define PMM_PAGE_SIZE 4096u
#define PMM_MAX_FRAMES 1024u
#define PMM_NUMA_UNKNOWN 0xFFFFFFFFu

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

#endif
