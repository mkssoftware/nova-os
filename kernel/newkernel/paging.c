/* NovaOS C-Kernel - Virtueller Bootstrap-Speichermanager (Port aus memory32.inc). */
#include "paging.h"
#include "pmm.h"
#include "heap.h"
#include "bib.h"

/* Identisch mit pmm.h's PMM_BOOTSTRAP_IDENTITY_LIMIT (dort uebernommen,
   nicht hier neu definiert) - pmm_initialize() garantiert seitdem, dass
   JEDER von pmm_alloc_page() gelieferte Rahmen innerhalb dieser Grenze
   liegt. Siehe pmm.h fuer die volle Begruendung und
   feedback_bss_paging_limit.md fuer den Vorfall, der die fruehere,
   unabhaengige Kopie dieser Konstante als Bootstrap-Falle entlarvte. */
#define PAGING_LOW_LIMIT PMM_BOOTSTRAP_IDENTITY_LIMIT

static uint32_t *paging_directory = 0;
static uint32_t paging_directory_phys = 0;
static int paging_enabled = 0;

/* Der urspruengliche, von paging_initialize() angelegte Kernel-Basis-
   Adressraum - bleibt bis zum Shutdown unveraendert, auch nachdem
   paging_switch_address_space() paging_directory/_directory_phys auf
   einen ANDEREN (z.B. Prozess-eigenen) Adressraum umgestellt hat. Dient
   als Klon-Quelle (paging_create_address_space) und als Erkennungs-
   Referenz fuer noch nicht privatisierte, geerbte Seitentabellen
   (paging_map_page_into). */
static uint32_t *paging_kernel_directory = 0;
static uint32_t paging_kernel_directory_phys = 0;

/* Kern von paging_map_page()/paging_map_page_into(): setzt eine einzelne
   PTE in EINEM explizit uebergebenen Directory, allokiert bei Bedarf eine
   neue (leere) Seitentabelle. Enthaelt KEINE Privatisierungs-Logik fuer
   geerbte Tabellen - das uebernimmt paging_map_page_into() selbst VOR
   diesem Aufruf, siehe dort. */
static int paging_map_page_raw(uint32_t *directory, uint32_t virtual_page,
                                uint32_t physical_page, uint32_t flags) {
    uint32_t pd_index = virtual_page >> 22;
    uint32_t pt_index = (virtual_page >> 12) & 0x3FFu;

    uint32_t *pde = &directory[pd_index];
    uint32_t *page_table;

    if (!(*pde & PAGING_PAGE_PRESENT)) {
        uint32_t numa_node;
        uint32_t table_phys = pmm_alloc_page(&numa_node);
        if (table_phys == 0) {
            return -1;
        }
        page_table = (uint32_t *)(uintptr_t)table_phys;
        for (uint32_t i = 0; i < PMM_PAGE_SIZE / 4; ++i) {
            page_table[i] = 0;
        }
        *pde = table_phys | PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE;
    } else {
        page_table = (uint32_t *)(uintptr_t)(*pde & 0xFFFFF000u);
    }

    /* Ein User-PTE benoetigt das User-Bit auf beiden Paging-Ebenen. Bereits
       vorhandene Supervisor-PTEs bleiben dadurch weiterhin geschuetzt. */
    if (flags & PAGING_PAGE_USER) {
        *pde |= PAGING_PAGE_USER;
    }

    page_table[pt_index] = (physical_page & 0xFFFFF000u) | flags;
    return 0;
}

int paging_map_page(uint32_t virtual_page, uint32_t physical_page, uint32_t flags) {
    return paging_map_page_raw(paging_directory, virtual_page, physical_page, flags);
}

int paging_create_address_space(uint32_t *out_directory_phys) {
    if (!paging_enabled) {
        return -1;
    }
    uint32_t numa_node;
    uint32_t dir_phys = pmm_alloc_page(&numa_node);
    if (dir_phys == 0) {
        return -1;
    }
    uint32_t *new_directory = (uint32_t *)(uintptr_t)dir_phys;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE / 4; ++i) {
        new_directory[i] = paging_kernel_directory[i];
    }
    *out_directory_phys = dir_phys;
    return 0;
}

int paging_map_page_into(uint32_t directory_phys, uint32_t virtual_page,
                          uint32_t physical_page, uint32_t flags) {
    uint32_t *directory = (uint32_t *)(uintptr_t)directory_phys;
    uint32_t pd_index = virtual_page >> 22;

    /* Erbt dieser Eintrag noch die beim Klonen (paging_create_address_
       space) uebernommene, mit dem Kernel-Basis-Adressraum GEMEINSAM
       GENUTZTE Seitentabelle, muss er VOR der eigentlichen Abbildung
       genau einmal privat kopiert werden - sonst waere die neue, diesem
       Adressraum eigene Abbildung auch im Kernel-Basis-Adressraum (und
       jedem anderen Adressraum, der dieselbe Tabelle noch teilt)
       sichtbar, was die Isolation unterlaufen wuerde. Erkennung: der
       Tabellen-Zeiger ist noch IDENTISCH mit dem des Kernel-Basis-
       Adressraums an derselben Stelle (eine bereits privatisierte Tabelle
       hat zwangslaeufig eine ANDERE physische Adresse). Fuer den Kernel-
       Basis-Adressraum selbst (directory == paging_kernel_directory) greift
       diese Privatisierung bewusst NICHT - dafuer ist paging_map_page()
       da. */
    if (directory != paging_kernel_directory &&
        (directory[pd_index] & PAGING_PAGE_PRESENT) &&
        (directory[pd_index] & 0xFFFFF000u) == (paging_kernel_directory[pd_index] & 0xFFFFF000u)) {
        uint32_t numa_node;
        uint32_t copy_phys = pmm_alloc_page(&numa_node);
        if (copy_phys == 0) {
            return -1;
        }
        uint32_t *copy = (uint32_t *)(uintptr_t)copy_phys;
        uint32_t *original = (uint32_t *)(uintptr_t)(directory[pd_index] & 0xFFFFF000u);
        for (uint32_t i = 0; i < PMM_PAGE_SIZE / 4; ++i) {
            copy[i] = original[i];
        }
        directory[pd_index] = copy_phys | (directory[pd_index] & 0xFFFu);
    }

    return paging_map_page_raw(directory, virtual_page, physical_page, flags);
}

void paging_switch_address_space(uint32_t directory_phys) {
    __asm__ volatile ("mov %0, %%cr3" : : "r"(directory_phys) : "memory");
    paging_directory = (uint32_t *)(uintptr_t)directory_phys;
    paging_directory_phys = directory_phys;
}

uint32_t paging_kernel_directory_physical(void) {
    return paging_kernel_directory_phys;
}

int paging_unmap_page(uint32_t virtual_page) {
    if (!paging_enabled) {
        return -1;
    }
    uint32_t pd_index = virtual_page >> 22;
    uint32_t pt_index = (virtual_page >> 12) & 0x3FFu;

    uint32_t *pde = &paging_directory[pd_index];
    if (!(*pde & PAGING_PAGE_PRESENT)) {
        return -1;
    }
    uint32_t *page_table = (uint32_t *)(uintptr_t)(*pde & 0xFFFFF000u);
    page_table[pt_index] = 0;
    __asm__ volatile ("invlpg (%0)" : : "r"(virtual_page) : "memory");
    return 0;
}

int paging_query_page(uint32_t virtual_page, uint32_t *out_physical, uint32_t *out_flags) {
    if (!paging_enabled) {
        return -1;
    }
    uint32_t pd_index = virtual_page >> 22;
    uint32_t pt_index = (virtual_page >> 12) & 0x3FFu;

    uint32_t *pde = &paging_directory[pd_index];
    if (!(*pde & PAGING_PAGE_PRESENT)) {
        return -1;
    }
    uint32_t *page_table = (uint32_t *)(uintptr_t)(*pde & 0xFFFFF000u);
    uint32_t pte = page_table[pt_index];
    if (!(pte & PAGING_PAGE_PRESENT)) {
        return -1;
    }
    if (out_physical) *out_physical = pte & 0xFFFFF000u;
    if (out_flags) *out_flags = pte & 0x00000FFFu;
    return 0;
}

int paging_protect_page(uint32_t virtual_page, uint32_t new_flags) {
    if (!paging_enabled) {
        return -1;
    }
    uint32_t pd_index = virtual_page >> 22;
    uint32_t pt_index = (virtual_page >> 12) & 0x3FFu;

    uint32_t *pde = &paging_directory[pd_index];
    if (!(*pde & PAGING_PAGE_PRESENT)) {
        return -1;
    }
    uint32_t *page_table = (uint32_t *)(uintptr_t)(*pde & 0xFFFFF000u);
    uint32_t pte = page_table[pt_index];
    if (!(pte & PAGING_PAGE_PRESENT)) {
        return -1;
    }
    uint32_t physical = pte & 0xFFFFF000u;
    page_table[pt_index] = physical | new_flags;
    if (new_flags & PAGING_PAGE_USER) {
        *pde |= PAGING_PAGE_USER;
    }
    __asm__ volatile ("invlpg (%0)" : : "r"(virtual_page) : "memory");
    return 0;
}

uint32_t paging_root_physical(void) {
    return paging_directory_phys;
}

int paging_initialize(void) {
    uint32_t numa_node;
    uint32_t dir_phys = pmm_alloc_page(&numa_node);
    if (dir_phys == 0) {
        return -1;
    }
    paging_directory = (uint32_t *)(uintptr_t)dir_phys;
    paging_directory_phys = dir_phys;
    paging_kernel_directory = paging_directory;
    paging_kernel_directory_phys = dir_phys;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE / 4; ++i) {
        paging_directory[i] = 0;
    }

    /* Frueher Kernel, Stack, BIB, Heap und Seitentabellen bleiben identisch
       abgebildet (0..16 MiB, deckt alle Bootstrap-PMM-Frames). */
    for (uint32_t addr = 0; addr < PAGING_LOW_LIMIT; addr += PMM_PAGE_SIZE) {
        if (paging_map_page(addr, addr, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE) != 0) {
            return -1;
        }
    }

    /* APIC-MMIO fuer den UEFI/Q35-Interruptpfad identisch abbilden. */
    if (paging_map_page(0xFEC00000u, 0xFEC00000u, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE) != 0) {
        return -1;
    }
    if (paging_map_page(0xFEE00000u, 0xFEE00000u, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE) != 0) {
        return -1;
    }

    /* Linearen Framebuffer in seiner bestehenden Adresse abbilden, falls vorhanden. */
    const bib_graphics_info_t *gfx = bib_graphics();
    if (gfx->has_graphics && gfx->framebuffer_address != 0) {
        uint32_t fb_start = gfx->framebuffer_address & 0xFFFFF000u;
        uint64_t fb_end64 = (uint64_t)gfx->pitch * gfx->height + gfx->framebuffer_address;
        if (fb_end64 > 0xFFFFFFFFu) {
            return -1;
        }
        fb_end64 = (fb_end64 + (PMM_PAGE_SIZE - 1)) & ~(uint64_t)(PMM_PAGE_SIZE - 1);
        uint32_t fb_end = (uint32_t)fb_end64;

        for (uint32_t addr = fb_start; addr < fb_end; addr += PMM_PAGE_SIZE) {
            if (paging_map_page(addr, addr, PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE) != 0) {
                return -1;
            }
        }
    }

    __asm__ volatile ("mov %0, %%cr3" : : "r"(dir_phys) : "memory");
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000u;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0) : "memory");

    paging_enabled = 1;
    return 0;
}

int paging_self_test(void) {
    if (!paging_enabled) {
        return -1;
    }
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    if (!(cr0 & 0x80000000u)) {
        return -1;
    }

    uint32_t *mem = (uint32_t *)heap_allocate(32);
    if (!mem) {
        return -1;
    }
    *mem = 0x50414745u;
    if (*mem != 0x50414745u) {
        return -1;
    }

    /* Beweis echter Adressraum-Isolation (NPSPEC-PROCESS-ISOLATION):
       zwei unabhaengige Adressraeume bilden dieselbe virtuelle Testadresse
       auf ZWEI VERSCHIEDENE physische Seiten mit unterscheidbarem Inhalt
       ab - inklusive des Falls, dass die Testadresse urspruenglich eine
       GEMEINSAM GENUTZTE (vom Kernel-Basis-Adressraum geerbte) Seitentabelle
       traf und daher erst privatisiert werden musste (siehe
       paging_map_page_into). Am Ende MUSS der Kernel-Basis-Adressraum
       selbst unveraendert geblieben sein (sonst waere die "Isolation" nur
       vorgetaeuscht) - der Rest des Boots laeuft nach diesem Selbsttest im
       Kernel-Basis-Adressraum weiter, daher wird am Ende explizit dorthin
       zurueckgewechselt. */
    #define PAGING_SELFTEST_VADDR 0x00700000u /* bereits identity-gemappt, PDE 1 */
    uint32_t dir_a, dir_b;
    if (paging_create_address_space(&dir_a) != 0) return -1;
    if (paging_create_address_space(&dir_b) != 0) return -1;

    uint32_t numa_node;
    uint32_t page_a = pmm_alloc_page(&numa_node);
    uint32_t page_b = pmm_alloc_page(&numa_node);
    if (page_a == 0 || page_b == 0) return -1;
    *(uint32_t *)(uintptr_t)page_a = 0xAAAAAAAAu;
    *(uint32_t *)(uintptr_t)page_b = 0xBBBBBBBBu;

    if (paging_map_page_into(dir_a, PAGING_SELFTEST_VADDR, page_a,
                              PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE) != 0) {
        return -1;
    }
    if (paging_map_page_into(dir_b, PAGING_SELFTEST_VADDR, page_b,
                              PAGING_PAGE_PRESENT | PAGING_PAGE_WRITE) != 0) {
        return -1;
    }

    uint32_t kernel_dir_phys = paging_kernel_directory_physical();

    paging_switch_address_space(dir_a);
    if (*(volatile uint32_t *)PAGING_SELFTEST_VADDR != 0xAAAAAAAAu) {
        paging_switch_address_space(kernel_dir_phys);
        return -1;
    }

    paging_switch_address_space(dir_b);
    if (*(volatile uint32_t *)PAGING_SELFTEST_VADDR != 0xBBBBBBBBu) {
        paging_switch_address_space(kernel_dir_phys);
        return -1;
    }

    paging_switch_address_space(kernel_dir_phys);
    uint32_t query_phys, query_flags;
    if (paging_query_page(PAGING_SELFTEST_VADDR, &query_phys, &query_flags) != 0) return -1;
    if (query_phys != PAGING_SELFTEST_VADDR) return -1; /* Kernel-Identity-Map unveraendert */
    #undef PAGING_SELFTEST_VADDR

    return 0;
}
