/* NovaOS C-Kernel - Nova Page Table Management (siehe pagetable.h). */
#include "pagetable.h"
#include "paging.h"
#include "framealloc.h"

static uint32_t g_map_count = 0;
static uint32_t g_unmap_count = 0;
static uint32_t g_protect_count = 0;

void pagetable_initialize(void) {
    g_map_count = 0;
    g_unmap_count = 0;
    g_protect_count = 0;
}

uint32_t pagetable_root(void) {
    return paging_root_physical();
}

int pagetable_map(uint32_t virtual_page, uint32_t physical_page, uint32_t flags) {
    if (paging_map_page(virtual_page, physical_page, flags) != 0) {
        return -1;
    }
    ++g_map_count;
    return 0;
}

int pagetable_unmap(uint32_t virtual_page) {
    if (paging_unmap_page(virtual_page) != 0) {
        return -1;
    }
    ++g_unmap_count;
    return 0;
}

int pagetable_query(uint32_t virtual_page, uint32_t *out_physical, uint32_t *out_flags) {
    return paging_query_page(virtual_page, out_physical, out_flags);
}

int pagetable_protect(uint32_t virtual_page, uint32_t new_flags) {
    if (paging_protect_page(virtual_page, new_flags) != 0) {
        return -1;
    }
    ++g_protect_count;
    return 0;
}

uint32_t pagetable_map_count(void) { return g_map_count; }
uint32_t pagetable_unmap_count(void) { return g_unmap_count; }
uint32_t pagetable_protect_count(void) { return g_protect_count; }

/* Scratch-Bereich fuer den Selbsttest - ausserhalb der Identity-Map
   (0..16MiB) und ausserhalb APIC/Framebuffer, damit kein bestehendes
   Mapping ueberschrieben wird. */
#define PAGETABLE_TEST_VIRT 0x02000000u

int pagetable_self_test(void) {
    pagetable_initialize();

    if (pagetable_root() == 0) return -1;

    uint32_t numa;
    uint32_t phys = frame_alloc(&numa);
    if (phys == 0) return -1;

    if (pagetable_map(PAGETABLE_TEST_VIRT, phys, PAGETABLE_PRESENT | PAGETABLE_WRITE) != 0) {
        frame_unref(phys);
        return -1;
    }
    if (pagetable_map_count() != 1) {
        pagetable_unmap(PAGETABLE_TEST_VIRT);
        frame_unref(phys);
        return -1;
    }

    volatile uint32_t *scratch = (volatile uint32_t *)(uintptr_t)PAGETABLE_TEST_VIRT;
    *scratch = 0x50544142u; /* "PTAB" */
    if (*scratch != 0x50544142u) {
        pagetable_unmap(PAGETABLE_TEST_VIRT);
        frame_unref(phys);
        return -1;
    }

    uint32_t q_phys, q_flags;
    if (pagetable_query(PAGETABLE_TEST_VIRT, &q_phys, &q_flags) != 0) {
        pagetable_unmap(PAGETABLE_TEST_VIRT);
        frame_unref(phys);
        return -1;
    }
    if (q_phys != (phys & 0xFFFFF000u)) {
        pagetable_unmap(PAGETABLE_TEST_VIRT);
        frame_unref(phys);
        return -1;
    }
    if (!(q_flags & PAGETABLE_PRESENT)) {
        pagetable_unmap(PAGETABLE_TEST_VIRT);
        frame_unref(phys);
        return -1;
    }

    /* Rechteaenderung: WRITE entfernen, physischer Frame bleibt gleich. */
    if (pagetable_protect(PAGETABLE_TEST_VIRT, PAGETABLE_PRESENT) != 0) {
        frame_unref(phys);
        return -1;
    }
    if (pagetable_protect_count() != 1) {
        frame_unref(phys);
        return -1;
    }
    if (pagetable_query(PAGETABLE_TEST_VIRT, &q_phys, &q_flags) != 0) {
        frame_unref(phys);
        return -1;
    }
    if (q_phys != (phys & 0xFFFFF000u) || (q_flags & PAGETABLE_WRITE)) {
        frame_unref(phys);
        return -1;
    }
    if (*scratch != 0x50544142u) { /* Inhalt bleibt nach Protection-Change erhalten */
        frame_unref(phys);
        return -1;
    }

    if (pagetable_unmap(PAGETABLE_TEST_VIRT) != 0) {
        frame_unref(phys);
        return -1;
    }
    if (pagetable_unmap_count() != 1) {
        frame_unref(phys);
        return -1;
    }

    /* Nach Unmap muss query fehlschlagen (nicht mehr gemappt). */
    if (pagetable_query(PAGETABLE_TEST_VIRT, &q_phys, &q_flags) == 0) {
        frame_unref(phys);
        return -1;
    }

    /* Unmap einer nie gemappten Seite in einer eigenen Page-Directory-
       Region (keine Seitentabelle vorhanden) muss fehlschlagen. */
    if (pagetable_unmap(0x3FC00000u) == 0) return -1;

    /* Protect einer nicht (mehr) gemappten Seite muss fehlschlagen. */
    if (pagetable_protect(PAGETABLE_TEST_VIRT, PAGETABLE_PRESENT) == 0) return -1;

    frame_unref(phys);
    return 0;
}
