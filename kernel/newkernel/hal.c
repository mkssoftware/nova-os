/* NovaOS C-Kernel - ACPI-Tabellen (Port aus hal32.inc). Siehe hal.h fuer
   den Hinweis zu den nicht portierten Legacy-BIB-Funktionen. */
#include "hal.h"
#include "bib.h"
#include "serial.h"
#include "paging.h"

typedef struct {
    uint32_t apic_id;
    uint32_t domain;
} acpi_srat_cpu_record_t;

typedef struct {
    uint32_t domain;
    uint32_t base;
    uint32_t length;
    uint32_t flags;
} acpi_srat_memory_record_t;

static uint32_t g_acpi_address = 0;

static uint32_t g_cpu_count = 1;
static uint32_t g_apic_ids[ACPI_CPU_CAPACITY];
static int g_madt_valid = 0;
static int g_madt_bsp_seen = 0;

static int g_srat_valid = 0;
static uint32_t g_srat_cpu_count = 0;
static uint32_t g_srat_memory_count = 0;
static uint32_t g_srat_conflicts = 0;
static acpi_srat_cpu_record_t g_srat_cpu_records[ACPI_SRAT_CPU_CAPACITY];
static acpi_srat_memory_record_t g_srat_memory_records[ACPI_SRAT_MEMORY_CAPACITY];

static uint32_t g_hpet_base = 0;

static uint32_t g_stack_canary_seed = 0;

/* --------------------------------------------------------------------- */
/* On-Demand-Mapping fuer ACPI-Tabellen ausserhalb der 0..16MiB-Identity-  */
/* Mapping-Region (paging.c deckt nur Kernel/Heap/Framebuffer/APIC ab).   */
/* --------------------------------------------------------------------- */

static int ensure_mapped(uint32_t addr, uint32_t length) {
    if (length == 0) return -1;
    uint32_t end = addr + length - 1u;
    if (end < addr) return -1; /* Overflow */

    uint32_t first_page = addr & ~0xFFFu;
    uint32_t last_page = end & ~0xFFFu;
    for (uint32_t page = first_page; ; page += 4096u) {
        if (paging_map_page(page, page, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE) != 0) {
            return -1;
        }
        if (page == last_page) break;
    }
    return 0;
}

/* --------------------------------------------------------------------- */
/* Physische Tabellenvalidierung                                         */
/* --------------------------------------------------------------------- */

/* Ein kompletter Tabellenbereich muss vollstaendig in einem Firmware-RAM-
   Deskriptor (BIB-Memory-Map, Typ 1..4 = nutzbar/ACPI-reclaim/ACPI-NVS/
   Boot-Services) liegen. Keine MMIO- oder 64-Bit-Adressen. */
static int acpi_physical_range_valid(uint32_t addr, uint32_t length) {
    if (addr == 0 || length == 0) return -1;
    uint32_t end = addr + length;
    if (end < addr) return -1; /* Overflow */

    const bib_memory_info_t *mem = bib_memory();
    if (!mem->has_memory_map || mem->entry_count > 256u) return -1;

    const uint8_t *entries = (const uint8_t *)(uintptr_t)mem->map_address;
    for (uint32_t i = 0; i < mem->entry_count; ++i) {
        const uint32_t *entry = (const uint32_t *)(entries + i * 24u);
        uint32_t base_low = entry[0];
        uint32_t base_high = entry[1];
        uint32_t length_low = entry[2];
        uint32_t length_high = entry[3];
        uint32_t type = entry[4];

        if (base_high != 0) continue;
        if (type < 1u || type > 4u) continue;
        if (addr < base_low) continue;
        if (length_high != 0) return 0; /* Laenge >4G: deckt jede 32-Bit-Range ab */
        uint32_t region_end = base_low + length_low;
        if (region_end < base_low) return 0; /* Overflow: Region bis 4G-Grenze */
        if (end <= region_end) return 0;
    }
    return -1;
}

static int acpi_rsdp_validate(uint32_t addr) {
    if (addr > 0xFFFFEFFFu) return -1;
    if (ensure_mapped(addr, 4096u) != 0) return -1; /* deckt max. erlaubte Extended-Laenge ab */
    const uint8_t *p = (const uint8_t *)(uintptr_t)addr;
    if (p[0] != 'R' || p[1] != 'S' || p[2] != 'D' || p[3] != ' ') return -1;
    if (p[4] != 'P' || p[5] != 'T' || p[6] != 'R' || p[7] != ' ') return -1;

    uint8_t sum = 0;
    for (int i = 0; i < 20; ++i) sum += p[i];
    if (sum != 0) return -1;

    if (p[15] < 2) return 0; /* ACPI 1.0: nur erste 20 Bytes geprueft */

    uint32_t ext_len = *(const uint32_t *)(p + 20);
    if (ext_len < 36 || ext_len > 4096) return -1;
    uint32_t end = addr + ext_len;
    if (end < addr) return -1;

    uint8_t sum2 = 0;
    for (uint32_t i = 0; i < ext_len; ++i) sum2 += p[i];
    if (sum2 != 0) return -1;
    return 0;
}

/* signature als 4 ASCII-Bytes, little-endian als uint32_t (wie im alten ASM). */
static int acpi_table_validate(uint32_t addr, uint32_t signature, uint32_t *out_length) {
    if (acpi_physical_range_valid(addr, 36u) != 0) return -1;
    if (ensure_mapped(addr, 4096u) != 0) return -1; /* deckt max. erlaubte Tabellenlaenge ab */
    const uint8_t *p = (const uint8_t *)(uintptr_t)addr;
    uint32_t sig = *(const uint32_t *)p;
    if (sig != signature) return -1;
    uint32_t length = *(const uint32_t *)(p + 4);
    if (length < 36 || length > 4096) return -1;
    if (acpi_physical_range_valid(addr, length) != 0) return -1;

    uint8_t sum = 0;
    for (uint32_t i = 0; i < length; ++i) sum += p[i];
    if (sum != 0) return -1;

    *out_length = length;
    return 0;
}

#define SIG(a, b, c, d) ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

/* Sucht eine Tabelle mit gegebener Signatur ueber die RSDT/XSDT-Root-Tabelle.
   0 = gefunden (Adresse und Laenge gesetzt), -1 sonst. */
static int acpi_find_table(uint32_t signature, uint32_t *out_addr, uint32_t *out_length) {
    const uint8_t *rsdp = (const uint8_t *)(uintptr_t)g_acpi_address;
    uint32_t root_addr;
    uint32_t entry_size;
    uint32_t root_length;

    int use_xsdt = (rsdp[15] >= 2) && (*(const uint32_t *)(rsdp + 28) == 0);
    if (use_xsdt) {
        uint64_t xsdt_addr64 = *(const uint64_t *)(rsdp + 24);
        if (xsdt_addr64 > 0xFFFFFFFFu) use_xsdt = 0;
        else if (acpi_table_validate((uint32_t)xsdt_addr64, SIG('X', 'S', 'D', 'T'), &root_length) == 0) {
            root_addr = (uint32_t)xsdt_addr64;
            entry_size = 8;
        } else {
            use_xsdt = 0;
        }
    }
    if (!use_xsdt) {
        uint32_t rsdt_addr = *(const uint32_t *)(rsdp + 16);
        if (acpi_table_validate(rsdt_addr, SIG('R', 'S', 'D', 'T'), &root_length) != 0) {
            return -1;
        }
        root_addr = rsdt_addr;
        entry_size = 4;
    }

    if (((root_length - 36u) % entry_size) != 0) return -1;
    uint32_t entries_left = (root_length - 36u) / entry_size;
    const uint8_t *cursor = (const uint8_t *)(uintptr_t)root_addr + 36u;

    while (entries_left > 0) {
        uint32_t candidate;
        if (entry_size == 8) {
            uint64_t addr64 = *(const uint64_t *)cursor;
            if (addr64 > 0xFFFFFFFFu) { cursor += entry_size; --entries_left; continue; }
            candidate = (uint32_t)addr64;
        } else {
            candidate = *(const uint32_t *)cursor;
        }

        uint32_t length;
        if (acpi_table_validate(candidate, signature, &length) == 0) {
            *out_addr = candidate;
            *out_length = length;
            return 0;
        }
        cursor += entry_size;
        --entries_left;
    }
    return -1;
}

/* --------------------------------------------------------------------- */
/* MADT                                                                   */
/* --------------------------------------------------------------------- */

static void acpi_madt_add_id(uint32_t apic_id) {
    if (apic_id == g_apic_ids[0]) {
        g_madt_bsp_seen = 1;
        return;
    }
    for (uint32_t i = 0; i < g_cpu_count; ++i) {
        if (g_apic_ids[i] == apic_id) return; /* bereits bekannt */
    }
    if (g_cpu_count < ACPI_CPU_CAPACITY) {
        g_apic_ids[g_cpu_count] = apic_id;
        ++g_cpu_count;
    }
}

static int acpi_madt_parse(uint32_t addr, uint32_t length) {
    if (length < 44) return -1;
    const uint8_t *base = (const uint8_t *)(uintptr_t)addr;
    const uint8_t *cursor = base + 44;
    const uint8_t *end = base + length;

    g_cpu_count = 1; /* Slot 0 bleibt der BSP (CPUID-APIC-ID), siehe acpi_rsdp_initialize */
    g_madt_bsp_seen = 0;

    while (cursor != end) {
        if (cursor + 2 > end) return -1;
        uint8_t entry_type = cursor[0];
        uint8_t entry_len = cursor[1];
        if (entry_len < 2) return -1;
        if (cursor + entry_len > end) return -1;

        if (entry_type == 0 && entry_len >= 8) { /* Local APIC */
            uint32_t flags = *(const uint32_t *)(cursor + 4);
            if (flags & 1u) {
                acpi_madt_add_id(cursor[3]);
            }
        } else if (entry_type == 9 && entry_len >= 16) { /* x2APIC */
            uint32_t flags = *(const uint32_t *)(cursor + 8);
            if (flags & 1u) {
                acpi_madt_add_id(*(const uint32_t *)(cursor + 4));
            }
        }
        cursor += entry_len;
    }

    return g_madt_bsp_seen ? 0 : -1;
}

static void acpi_madt_discover(void) {
    g_cpu_count = 1;
    g_madt_valid = 0;

    uint32_t addr, length;
    if (acpi_find_table(SIG('A', 'P', 'I', 'C'), &addr, &length) != 0) {
        g_cpu_count = 1;
        return;
    }
    if (acpi_madt_parse(addr, length) == 0) {
        g_madt_valid = 1;
    } else {
        g_cpu_count = 1;
    }
}

/* --------------------------------------------------------------------- */
/* SRAT                                                                   */
/* --------------------------------------------------------------------- */

static int acpi_srat_add_cpu(uint32_t apic_id, uint32_t domain) {
    int known = 0;
    for (uint32_t i = 0; i < g_cpu_count; ++i) {
        if (g_apic_ids[i] == apic_id) { known = 1; break; }
    }
    if (!known) {
        ++g_srat_conflicts;
        return -1;
    }

    for (uint32_t i = 0; i < g_srat_cpu_count; ++i) {
        if (g_srat_cpu_records[i].apic_id == apic_id) {
            if (g_srat_cpu_records[i].domain != domain) {
                ++g_srat_conflicts;
                return -1;
            }
            return 0;
        }
    }

    if (g_srat_cpu_count >= ACPI_SRAT_CPU_CAPACITY) {
        ++g_srat_conflicts;
        return -1;
    }
    g_srat_cpu_records[g_srat_cpu_count].apic_id = apic_id;
    g_srat_cpu_records[g_srat_cpu_count].domain = domain;
    ++g_srat_cpu_count;
    return 0;
}

static int acpi_srat_add_memory(uint32_t domain, uint32_t base, uint32_t length, uint32_t flags) {
    uint32_t end = base + length;
    if (end < base) { ++g_srat_conflicts; return -1; }

    for (uint32_t i = 0; i < g_srat_memory_count; ++i) {
        uint32_t other_end = g_srat_memory_records[i].base + g_srat_memory_records[i].length;
        if (base < other_end && end > g_srat_memory_records[i].base) {
            ++g_srat_conflicts;
            return -1;
        }
    }

    if (g_srat_memory_count >= ACPI_SRAT_MEMORY_CAPACITY) {
        ++g_srat_conflicts;
        return -1;
    }
    g_srat_memory_records[g_srat_memory_count].domain = domain;
    g_srat_memory_records[g_srat_memory_count].base = base;
    g_srat_memory_records[g_srat_memory_count].length = length;
    g_srat_memory_records[g_srat_memory_count].flags = flags;
    ++g_srat_memory_count;
    return 0;
}

static int acpi_srat_parse(uint32_t addr, uint32_t length) {
    if (length < 48) return -1;
    const uint8_t *base = (const uint8_t *)(uintptr_t)addr;
    const uint8_t *cursor = base + 48;
    const uint8_t *end = base + length;

    while (cursor != end) {
        if (cursor + 2 > end) return -1;
        uint8_t entry_type = cursor[0];
        uint8_t entry_len = cursor[1];
        if (entry_len < 2) return -1;
        if (cursor + entry_len > end) return -1;

        if (entry_type == 0 && entry_len >= 16) { /* Processor Local APIC Affinity */
            uint32_t flags = *(const uint32_t *)(cursor + 4);
            if (flags & 1u) {
                uint32_t apic_id = cursor[3];
                uint32_t domain = cursor[2] | ((uint32_t)cursor[9] << 8) |
                                   ((uint32_t)cursor[10] << 16) | ((uint32_t)cursor[11] << 24);
                if (acpi_srat_add_cpu(apic_id, domain) != 0) return -1;
            }
        } else if (entry_type == 1 && entry_len >= 40) { /* Memory Affinity */
            uint32_t flags = *(const uint32_t *)(cursor + 28);
            if (flags & 1u) {
                uint32_t base_high = *(const uint32_t *)(cursor + 12);
                uint32_t length_high = *(const uint32_t *)(cursor + 20);
                if (base_high != 0 || length_high != 0) {
                    /* Nur 32-Bit-adressierbare Bereiche unterstuetzt - kein Fehler. */
                } else {
                    uint32_t domain = *(const uint32_t *)(cursor + 4);
                    uint32_t base = *(const uint32_t *)(cursor + 8);
                    uint32_t mem_length = *(const uint32_t *)(cursor + 16);
                    if (mem_length != 0) {
                        if (acpi_srat_add_memory(domain, base, mem_length, flags) != 0) return -1;
                    }
                }
            }
        } else if (entry_type == 2 && entry_len >= 24) { /* x2APIC Affinity */
            uint32_t flags = *(const uint32_t *)(cursor + 12);
            if (flags & 1u) {
                uint32_t apic_id = *(const uint32_t *)(cursor + 8);
                uint32_t domain = *(const uint32_t *)(cursor + 4);
                if (acpi_srat_add_cpu(apic_id, domain) != 0) return -1;
            }
        }
        cursor += entry_len;
    }

    if (g_srat_cpu_count == 0 && g_srat_memory_count == 0) return -1;
    return 0;
}

static void acpi_srat_discover(void) {
    g_srat_valid = 0;
    g_srat_cpu_count = 0;
    g_srat_memory_count = 0;
    g_srat_conflicts = 0;
    for (uint32_t i = 0; i < ACPI_SRAT_CPU_CAPACITY; ++i) g_srat_cpu_records[i] = (acpi_srat_cpu_record_t){0};
    for (uint32_t i = 0; i < ACPI_SRAT_MEMORY_CAPACITY; ++i) g_srat_memory_records[i] = (acpi_srat_memory_record_t){0};

    uint32_t addr, length;
    if (acpi_find_table(SIG('S', 'R', 'A', 'T'), &addr, &length) != 0) {
        return;
    }
    if (acpi_srat_parse(addr, length) == 0) {
        g_srat_valid = 1;
    } else {
        g_srat_cpu_count = 0;
        g_srat_memory_count = 0;
    }
}

/* --------------------------------------------------------------------- */
/* HPET                                                                   */
/* --------------------------------------------------------------------- */

static void acpi_hpet_discover(void) {
    g_hpet_base = 0;

    uint32_t addr, length;
    if (acpi_find_table(SIG('H', 'P', 'E', 'T'), &addr, &length) != 0) {
        return;
    }
    if (length < 56) return;

    const uint8_t *p = (const uint8_t *)(uintptr_t)addr;
    if (p[40] != 0) return; /* GAS.AddressSpaceID muss 0 (System Memory) sein */
    if (*(const uint32_t *)(p + 48) != 0) return; /* nur 32-Bit-Basisadressen */

    uint32_t base = *(const uint32_t *)(p + 44);
    if (base == 0) return;
    g_hpet_base = base;
    /* Hinweis: anders als im alten ASM wird das General-Configuration-
       Register NICHT geschrieben (ENABLE_CNF-Bit) - das waere ein echter
       MMIO-Schreibzugriff auf eine noch nicht gemappte/verifizierte Region;
       HPET-Aktivierung bleibt dem spaeteren Time-Subsystem vorbehalten. */
}

/* --------------------------------------------------------------------- */
/* Oeffentliche API                                                       */
/* --------------------------------------------------------------------- */

int acpi_rsdp_initialize(void) {
    g_cpu_count = 1;
    g_madt_valid = 0;
    g_srat_valid = 0;
    g_srat_cpu_count = 0;
    g_srat_memory_count = 0;
    for (uint32_t i = 0; i < ACPI_CPU_CAPACITY; ++i) g_apic_ids[i] = 0;

    uint32_t eax, ebx;
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx) : "a"(1) : "ecx", "edx");
    g_apic_ids[0] = (ebx >> 24) & 0xFFu;

    const bib_acpi_info_t *acpi = bib_acpi();
    if (!acpi->has_acpi || acpi->address == 0) {
        serial_puts("NOVA: ACPI RSDP nicht verfuegbar\r\n");
        return 0;
    }
    g_acpi_address = acpi->address;

    if (acpi_rsdp_validate(g_acpi_address) != 0) {
        serial_puts("NOVA: ACPI RSDP nicht verfuegbar\r\n");
        g_acpi_address = 0;
        return 0;
    }

    acpi_madt_discover();
    acpi_srat_discover();
    acpi_hpet_discover();

    serial_puts("NOVA: ACPI RSDP gueltig\r\n");
    return 0;
}

int acpi_madt_valid(void) { return g_madt_valid; }
uint32_t acpi_cpu_count(void) { return g_cpu_count; }
int acpi_srat_valid(void) { return g_srat_valid; }
uint32_t acpi_srat_cpu_count(void) { return g_srat_cpu_count; }
uint32_t acpi_srat_memory_count(void) { return g_srat_memory_count; }
uint32_t acpi_hpet_base(void) { return g_hpet_base; }

int acpi_srat_cpu_domain_lookup(uint32_t apic_id, uint32_t *out_domain) {
    if (!g_srat_valid) return -1;
    for (uint32_t i = 0; i < g_srat_cpu_count; ++i) {
        if (g_srat_cpu_records[i].apic_id == apic_id) {
            *out_domain = g_srat_cpu_records[i].domain;
            return 0;
        }
    }
    return -1;
}

int acpi_srat_memory_domain_lookup(uint32_t page, uint32_t *out_domain) {
    if (!g_srat_valid) return -1;
    for (uint32_t i = 0; i < g_srat_memory_count; ++i) {
        uint32_t base = g_srat_memory_records[i].base;
        uint32_t end = base + g_srat_memory_records[i].length;
        if (page >= base && page + 4096u <= end) {
            *out_domain = g_srat_memory_records[i].domain;
            return 0;
        }
    }
    return -1;
}

int early_security_entropy_initialize(void) {
    const bib_entropy_info_t *entropy = bib_entropy();
    if (!entropy->has_entropy) {
        return -1;
    }
    const uint32_t *seed = (const uint32_t *)entropy->seed;
    uint32_t value = seed[0];
    value ^= seed[1];
    value = (value << 9) | (value >> 23);
    value ^= seed[2];
    value = (value << 13) | (value >> 19);
    value ^= seed[3];
    if (value == 0) {
        return -1;
    }
    g_stack_canary_seed = value;
    return 0;
}

int early_security_entropy_self_test(void) {
    return (g_stack_canary_seed != 0) ? 0 : -1;
}

uint32_t stack_canary_seed(void) {
    return g_stack_canary_seed;
}

int acpi_hal_self_test(void) {
    /* Fehlende ACPI-Tabellen sind kein Fehler - nur grobe Konsistenz
       pruefen, wenn Daten vorhanden sind. */
    if (g_madt_valid && g_cpu_count == 0) return -1;
    if (g_srat_valid && g_srat_cpu_count == 0 && g_srat_memory_count == 0) return -1;
    return 0;
}
