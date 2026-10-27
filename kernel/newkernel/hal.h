/* NovaOS C-Kernel - ACPI-Tabellen (RSDP/MADT/SRAT/HPET) und frueher
   Security-Entropy-Seed (Port aus hal32.inc, NPSPEC-HAL-0001).

   `validate_bib`/`crc32_bib`/`create_kernel_context` aus hal32.inc sind
   NICHT portiert - sie uebersetzten den BIB in die alte "kernel_context"-
   Struktur, die es im neuen Kernel nicht gibt; bib.c (bib_parse/bib_graphics/
   bib_memory/bib_kernel/bib_acpi/bib_entropy) deckt diese Rolle bereits ab.
   Nur UEFI unterstuetzt (Architekturentscheidung) - der alte BIOS-EBDA-Scan-
   Fallback in acpi_rsdp_initialize ist deshalb entfallen: ohne BIB-ACPI-
   Adresse gilt ACPI als nicht verfuegbar. */
#ifndef NOVA_HAL_H
#define NOVA_HAL_H

#include <stdint.h>

#define ACPI_SRAT_CPU_CAPACITY    8u
#define ACPI_SRAT_MEMORY_CAPACITY 8u
#define ACPI_CPU_CAPACITY         8u

/* Liest RSDP/MADT/SRAT/HPET aus der von bib_acpi() gelieferten Adresse.
   Fehlende/fehlerhafte ACPI-Tabellen sind kein Fehler (NPSPEC-konform: der
   BSP-Betrieb bleibt ohne Firmwaretabellen funktionsfaehig). Immer 0. */
int acpi_rsdp_initialize(void);
int acpi_hal_self_test(void); /* 0 = ok */

int acpi_madt_valid(void);
uint32_t acpi_cpu_count(void);
/* APIC-ID der entdeckten CPU an index (0 = BSP, 1..acpi_cpu_count()-1 =
   APs); 0 wenn index ausserhalb des gueltigen Bereichs liegt. */
uint32_t acpi_cpu_apic_id(uint32_t index);

int acpi_srat_valid(void);
uint32_t acpi_srat_cpu_count(void);
uint32_t acpi_srat_memory_count(void);

/* 0 = Erfolg (*out_domain gesetzt), -1 wenn SRAT nicht verfuegbar oder
   apic_id unbekannt. */
int acpi_srat_cpu_domain_lookup(uint32_t apic_id, uint32_t *out_domain);

/* 0 = Erfolg (*out_domain gesetzt), -1 wenn SRAT nicht verfuegbar oder keine
   passende Memory-Affinity-Region gefunden (page = physische 4-KiB-Seite). */
int acpi_srat_memory_domain_lookup(uint32_t page, uint32_t *out_domain);

uint32_t acpi_hpet_base(void); /* 0 = keine HPET-Tabelle gefunden */

/* Leitet einen Stack-Canary-Seed aus dem BIB-Entropy-Seed ab (XOR+ROL ueber
   die ersten 16 Bytes). 0 = Erfolg (seed != 0), -1 wenn Entropy fehlt oder
   der abgeleitete Wert 0 ergibt. */
int early_security_entropy_initialize(void);
int early_security_entropy_self_test(void); /* 0 = ok */
uint32_t stack_canary_seed(void);

#endif
