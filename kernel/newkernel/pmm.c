/* NovaOS C-Kernel - Physischer Bootstrap-Speichermanager (Port aus memory32.inc).
   Ueberlaufprüfungen (die im ASM per Carry-Flag nach add liefen) werden hier
   mit uint64_t-Zwischenwerten nachgebildet. */
#include "pmm.h"
#include "bib.h"

static uint32_t pmm_frames[PMM_MAX_FRAMES];
static uint32_t pmm_frame_numa_nodes[PMM_MAX_FRAMES];
static uint32_t pmm_frame_count_value = 0;
static uint32_t pmm_reserved_end = 0;

/* TODO: durch echtes ACPI/SRAT-Lookup ersetzen, sobald das ACPI-Subsystem
   portiert ist (ASM: acpi_srat_memory_domain_lookup). */
static uint32_t acpi_srat_memory_domain_lookup(uint32_t physical_address) {
    (void)physical_address;
    return PMM_NUMA_UNKNOWN;
}

uint32_t pmm_frame_count(void) {
    return pmm_frame_count_value;
}

int pmm_initialize(void) {
    pmm_frame_count_value = 0;

    const bib_kernel_info_t *kernel = bib_kernel();
    if (!kernel->has_kernel) {
        return -1;
    }
    uint64_t reserved_end64 = (uint64_t)kernel->load_address + kernel->image_size;
    if (reserved_end64 > 0xFFFFFFFFu) {
        return -1;
    }
    reserved_end64 = (reserved_end64 + (PMM_PAGE_SIZE - 1)) & ~(uint64_t)(PMM_PAGE_SIZE - 1);
    pmm_reserved_end = (uint32_t)reserved_end64;

    const bib_memory_info_t *memory = bib_memory();
    if (!memory->has_memory_map) {
        return -1;
    }

    const bib_memory_map_entry_t *entries =
        (const bib_memory_map_entry_t *)(uintptr_t)memory->map_address;

    for (uint32_t i = 0; i < memory->entry_count; ++i) {
        const bib_memory_map_entry_t *entry = &entries[i];
        if (entry->type != 1u) continue;        /* nur nutzbares RAM */
        if (entry->base_high != 0) continue;     /* Bootstrap-PMM: nur < 4 GiB */

        uint64_t aligned_base = ((uint64_t)entry->base_low + (PMM_PAGE_SIZE - 1))
            & ~(uint64_t)(PMM_PAGE_SIZE - 1);
        if (aligned_base > 0xFFFFFFFFu) continue;
        uint32_t start = (aligned_base >= pmm_reserved_end) ? (uint32_t)aligned_base : pmm_reserved_end;

        uint64_t end64 = (uint64_t)entry->base_low + entry->length_low;
        uint32_t end;
        if (end64 > 0xFFFFFFFFu || entry->length_high != 0) {
            end = 0xFFFFF000u; /* Deckel wie im ASM (.end_maximum) */
        } else {
            end = (uint32_t)end64 & 0xFFFFF000u;
        }

        for (uint32_t page = start; page < end; ) {
            if (pmm_frame_count_value >= PMM_MAX_FRAMES) {
                return (pmm_frame_count_value >= 2) ? 0 : -1;
            }
            pmm_frames[pmm_frame_count_value] = page;
            pmm_frame_numa_nodes[pmm_frame_count_value] = acpi_srat_memory_domain_lookup(page);
            ++pmm_frame_count_value;

            uint64_t next_page = (uint64_t)page + PMM_PAGE_SIZE;
            if (next_page > 0xFFFFFFFFu) {
                break; /* Ueberlauf: wie ASM .next_entry */
            }
            page = (uint32_t)next_page;
        }
    }

    return (pmm_frame_count_value >= 2) ? 0 : -1;
}

uint32_t pmm_alloc_page(uint32_t *out_numa_node) {
    if (pmm_frame_count_value == 0) {
        if (out_numa_node) *out_numa_node = PMM_NUMA_UNKNOWN;
        return 0;
    }
    --pmm_frame_count_value;
    uint32_t page = pmm_frames[pmm_frame_count_value];
    if (out_numa_node) *out_numa_node = pmm_frame_numa_nodes[pmm_frame_count_value];
    return page;
}

uint32_t pmm_alloc_page_strict(uint32_t node, uint32_t *out_numa_node) {
    for (uint32_t i = pmm_frame_count_value; i > 0; ) {
        --i;
        if (pmm_frame_numa_nodes[i] != node) {
            continue;
        }
        uint32_t page = pmm_frames[i];
        uint32_t last = pmm_frame_count_value - 1;
        if (i != last) {
            pmm_frames[i] = pmm_frames[last];
            pmm_frame_numa_nodes[i] = pmm_frame_numa_nodes[last];
        }
        pmm_frame_count_value = last;
        if (out_numa_node) *out_numa_node = node;
        return page;
    }
    if (out_numa_node) *out_numa_node = PMM_NUMA_UNKNOWN;
    return 0;
}

uint32_t pmm_alloc_page_preferred(uint32_t preferred_node, uint32_t *out_numa_node) {
    uint32_t page = pmm_alloc_page_strict(preferred_node, out_numa_node);
    if (page != 0) {
        return page;
    }
    return pmm_alloc_page(out_numa_node);
}

int pmm_free_page(uint32_t page) {
    if ((page & (PMM_PAGE_SIZE - 1)) != 0) {
        return -1;
    }
    if (page < pmm_reserved_end) {
        return -1;
    }
    if (pmm_frame_count_value >= PMM_MAX_FRAMES) {
        return -1;
    }
    pmm_frames[pmm_frame_count_value] = page;
    pmm_frame_numa_nodes[pmm_frame_count_value] = acpi_srat_memory_domain_lookup(page);
    ++pmm_frame_count_value;
    return 0;
}

int pmm_self_test(void) {
    uint32_t initial_count = pmm_frame_count_value;

    uint32_t numa_a, numa_b;
    uint32_t page_a = pmm_alloc_page(&numa_a);
    if (page_a == 0) return -1;
    uint32_t page_b = pmm_alloc_page(&numa_b);
    if (page_b == 0) {
        pmm_free_page(page_a);
        return -1;
    }
    if (page_b == page_a) {
        pmm_free_page(page_b);
        pmm_free_page(page_a);
        return -1;
    }

    if (pmm_free_page(page_b) != 0) return -1;
    if (pmm_free_page(page_a) != 0) return -1;
    if (pmm_frame_count_value != initial_count) return -1;

    /* Falls eine bekannte NUMA-Lokalitaet existiert: Strict/Preferred muessen
       deterministisch funktionieren (identisch zur ASM-Logik). */
    for (uint32_t i = 0; i < pmm_frame_count_value; ++i) {
        uint32_t node = pmm_frame_numa_nodes[i];
        if (node == PMM_NUMA_UNKNOWN) continue;

        uint32_t resolved;
        uint32_t strict_page = pmm_alloc_page_strict(node, &resolved);
        if (strict_page == 0) return -1;
        if (resolved != node) {
            pmm_free_page(strict_page);
            return -1;
        }

        uint32_t bogus_node = 0xFFFFFFFEu;
        if (pmm_alloc_page_strict(bogus_node, &resolved) != 0) {
            pmm_free_page(strict_page);
            return -1;
        }
        uint32_t preferred_page = pmm_alloc_page_preferred(bogus_node, &resolved);
        if (preferred_page == 0) {
            pmm_free_page(strict_page);
            return -1;
        }
        if (pmm_free_page(preferred_page) != 0) {
            pmm_free_page(strict_page);
            return -1;
        }
        if (pmm_free_page(strict_page) != 0) return -1;
        if (pmm_frame_count_value != initial_count) return -1;
        break;
    }

    return 0;
}
