/* NovaOS C-Kernel - Virtueller Bootstrap-Speichermanager (Port aus memory32.inc). */
#include "paging.h"
#include "pmm.h"
#include "heap.h"
#include "bib.h"

#define PAGING_LOW_LIMIT 0x01000000u

static uint32_t *paging_directory = 0;
static uint32_t paging_directory_phys = 0;
static int paging_enabled = 0;

int paging_map_page(uint32_t virtual_page, uint32_t physical_page, uint32_t flags) {
    uint32_t pd_index = virtual_page >> 22;
    uint32_t pt_index = (virtual_page >> 12) & 0x3FFu;

    uint32_t *pde = &paging_directory[pd_index];
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
    return 0;
}
