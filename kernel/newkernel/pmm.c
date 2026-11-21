/* NovaOS C-Kernel - Physischer Bootstrap-Speichermanager (Port aus memory32.inc).
   Ueberlaufprüfungen (die im ASM per Carry-Flag nach add liefen) werden hier
   mit uint64_t-Zwischenwerten nachgebildet. */
#include "pmm.h"
#include "bib.h"

static uint32_t pmm_frames[PMM_MAX_FRAMES];
static uint32_t pmm_frame_numa_nodes[PMM_MAX_FRAMES];
static uint32_t pmm_frame_count_value = 0;
static uint32_t pmm_reserved_end = 0;

/* Dedizierte, NIE einzeln fragmentierte Reserve fuer pmm_alloc_contiguous()
   (2026-11-21, project_pmm_capacity_increase.md): nach dem Anheben von
   PMM_BOOTSTRAP_IDENTITY_LIMIT auf 64 MiB gab es zwar zehntausende freie
   Rahmen insgesamt, aber KEINEN einzigen 69-Seiten-Lauf mehr - die vielen
   einzelnen pmm_alloc_page()-Aufrufe aus ueber 200 Boot-Selbsttests (jeder
   fuer sich harmlos) hatten den gesamten Pool bis dahin bereits auf
   4-KiB-Granularitaet durchmischt. Reine Seitenzahl loest also KEINE
   Fragmentierung. Fix: die obersten PMM_CONTIG_ARENA_BYTES der Identity-
   Map werden in pmm_initialize() aus dem normalen, von pmm_alloc_page()
   erreichbaren Pool herausgenommen und bleiben bis zum ersten pmm_alloc_
   contiguous()-Fallback (siehe dort) UNBERUEHRT - garantiert einen
   pristinen zusammenhaengenden Bereich fuer grosse GUI-Puffer, unabhaengig
   davon, wie zerstueckelt der Rest des Boots den allgemeinen Pool
   hinterlaesst. Reiner Bump-Zeiger (kein Freigeben/Wiederverwenden
   innerhalb der Arena) - ausreichend, weil die bisherigen Verwender
   (Taskbar/Explorer/Einstellungen/Startmenue/Icons-GUI-Puffer) ihre
   Puffer fuer die gesamte Laufzeit des Desktops behalten, nie freigeben.
   pmm_free_page() auf einen Arena-Rahmen ist trotzdem sicher (siehe
   dort) - er wandert dann einfach dauerhaft in den allgemeinen Pool. */
#define PMM_CONTIG_ARENA_BYTES (4u * 1024u * 1024u)
static uint32_t pmm_contig_arena_base = 0;
static uint32_t pmm_contig_arena_used = 0;

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
        /* Niemals Rahmen ausserhalb der von paging_initialize() identisch
           abgebildeten Region sammeln (siehe PMM_BOOTSTRAP_IDENTITY_LIMIT in
           pmm.h) - unabhaengig davon, in welcher Reihenfolge die UEFI-
           Memory-Map ihre Eintraege liefert. */
        if (end > PMM_BOOTSTRAP_IDENTITY_LIMIT) end = PMM_BOOTSTRAP_IDENTITY_LIMIT;

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

    /* Contiguous-Arena reservieren (siehe Kommentar bei den statischen
       Variablen oben) - NUR, wenn die obersten PMM_CONTIG_ARENA_BYTES
       tatsaechlich als echtes, nutzbares RAM im normalen Scan oben
       gefunden wurden (keine blinde Annahme ueber reservierte Bereiche -
       ein fehlender Treffer ist kein Fehler, pmm_alloc_contiguous()
       faellt dann einfach auf die reine Pool-Suche zurueck). */
    pmm_contig_arena_base = 0;
    pmm_contig_arena_used = 0;
    if (PMM_BOOTSTRAP_IDENTITY_LIMIT > PMM_CONTIG_ARENA_BYTES) {
        uint32_t candidate = PMM_BOOTSTRAP_IDENTITY_LIMIT - PMM_CONTIG_ARENA_BYTES;
        uint32_t arena_frames = PMM_CONTIG_ARENA_BYTES / PMM_PAGE_SIZE;
        uint32_t all_present = 1;
        for (uint32_t i = 0; i < arena_frames; ++i) {
            uint32_t want = candidate + i * PMM_PAGE_SIZE;
            uint32_t exists = 0;
            for (uint32_t j = 0; j < pmm_frame_count_value; ++j) {
                if (pmm_frames[j] == want) { exists = 1; break; }
            }
            if (!exists) { all_present = 0; break; }
        }
        if (all_present) {
            for (uint32_t i = 0; i < arena_frames; ++i) {
                uint32_t want = candidate + i * PMM_PAGE_SIZE;
                for (uint32_t j = 0; j < pmm_frame_count_value; ++j) {
                    if (pmm_frames[j] == want) {
                        --pmm_frame_count_value;
                        pmm_frames[j] = pmm_frames[pmm_frame_count_value];
                        pmm_frame_numa_nodes[j] = pmm_frame_numa_nodes[pmm_frame_count_value];
                        break;
                    }
                }
            }
            pmm_contig_arena_base = candidate;
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

    /* pmm_alloc_contiguous(): vier Rahmen muessen als EIN zusammenhaengender
       Lauf (aufsteigend, je PMM_PAGE_SIZE auseinander) zurueckkommen, und
       die Tabelle muss danach um genau 4 geschrumpft sein. */
    uint32_t contiguous_base;
    if (pmm_alloc_contiguous(4u, &contiguous_base) != 0) return -1;
    if (pmm_frame_count_value != initial_count - 4u) return -1;
    for (uint32_t k = 0; k < 4u; ++k) {
        if (pmm_free_page(contiguous_base + k * PMM_PAGE_SIZE) != 0) return -1;
    }
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

int pmm_alloc_contiguous(uint32_t count, uint32_t *out_first_page) {
    if (count == 0 || count > PMM_MAX_FRAMES) return -1;

    for (uint32_t i = 0; i < pmm_frame_count_value; ++i) {
        uint32_t base = pmm_frames[i];
        int all_found = 1;
        for (uint32_t k = 1; k < count; ++k) {
            uint32_t want = base + k * PMM_PAGE_SIZE;
            int found = 0;
            for (uint32_t j = 0; j < pmm_frame_count_value; ++j) {
                if (pmm_frames[j] == want) { found = 1; break; }
            }
            if (!found) { all_found = 0; break; }
        }
        if (!all_found) continue;

        /* Lauf gefunden - alle `count` Rahmen aus der Tabelle entfernen
           (Swap-mit-letztem-Element, Reihenfolge der verbleibenden
           Eintraege ist fuer einen LIFO-Stack ohnehin irrelevant). */
        for (uint32_t k = 0; k < count; ++k) {
            uint32_t want = base + k * PMM_PAGE_SIZE;
            for (uint32_t j = 0; j < pmm_frame_count_value; ++j) {
                if (pmm_frames[j] == want) {
                    --pmm_frame_count_value;
                    pmm_frames[j] = pmm_frames[pmm_frame_count_value];
                    pmm_frame_numa_nodes[j] = pmm_frame_numa_nodes[pmm_frame_count_value];
                    break;
                }
            }
        }
        *out_first_page = base;
        return 0;
    }

    /* Fallback: die dedizierte, NIE einzeln fragmentierte Arena am oberen
       Ende der Identity-Map (siehe pmm_initialize() und die statischen
       Variablen oben) - greift NUR, wenn die flexible Suche oben (wegen
       Fragmentierung durch viele einzelne pmm_alloc_page()-Aufrufe)
       nichts findet. Bewusst NICHT die bevorzugte erste Anlaufstelle:
       so bleibt pmm_self_test()s bestehende Pruefung (4 Rahmen aus dem
       normalen Pool, Zaehler faellt um genau 4) unveraendert gueltig -
       vier zusammenhaengende Rahmen findet die obige Suche in der Praxis
       ohnehin fast immer, nur die SELTENEN, GROSSEN Anfragen (GUI-Puffer)
       brauchen die Arena ueberhaupt. */
    uint64_t need_bytes = (uint64_t)count * (uint64_t)PMM_PAGE_SIZE;
    if (pmm_contig_arena_base != 0 &&
        (uint64_t)pmm_contig_arena_used + need_bytes <= (uint64_t)PMM_CONTIG_ARENA_BYTES) {
        uint32_t base = pmm_contig_arena_base + pmm_contig_arena_used;
        pmm_contig_arena_used += (uint32_t)need_bytes;
        *out_first_page = base;
        return 0;
    }

    return -1;
}
