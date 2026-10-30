/* NovaOS C-Kernel - Nova Physical Frame Allocator (siehe framealloc.h). */
#include "framealloc.h"
#include "pmm.h"

typedef struct {
    uint32_t page; /* 0 = freier Slot */
    uint32_t refcount;
} frame_track_t;

static frame_track_t g_tracked[FRAMEALLOC_TRACKED_CAPACITY];

void framealloc_initialize(void) {
    for (uint32_t i = 0; i < FRAMEALLOC_TRACKED_CAPACITY; ++i) {
        g_tracked[i].page = 0;
        g_tracked[i].refcount = 0;
    }
}

static int32_t find_tracked(uint32_t page) {
    for (uint32_t i = 0; i < FRAMEALLOC_TRACKED_CAPACITY; ++i) {
        if (g_tracked[i].page == page) return (int32_t)i;
    }
    return -1;
}

uint32_t frame_alloc(uint32_t *out_numa_node) {
    return pmm_alloc_page(out_numa_node);
}

int frame_alloc_contiguous(uint32_t count, uint32_t *out_base) {
    if (count == 0 || count > FRAMEALLOC_MAX_CONTIGUOUS) {
        return -1;
    }
    uint32_t pages[FRAMEALLOC_MAX_CONTIGUOUS];
    uint32_t got = 0;
    for (; got < count; ++got) {
        uint32_t numa;
        uint32_t page = pmm_alloc_page(&numa);
        if (page == 0) {
            break;
        }
        pages[got] = page;
    }
    if (got != count) {
        for (uint32_t i = 0; i < got; ++i) {
            pmm_free_page(pages[i]);
        }
        return -1;
    }

    /* PMM liefert vom (unfragmentierten) Stack absteigende Adressen - pages[0]
       ist damit die hoechste, pages[count-1] die niedrigste Seite. Pruefen,
       ob sie tatsaechlich lueckenlos zusammenhaengen. */
    int contiguous = 1;
    for (uint32_t i = 1; i < count; ++i) {
        if (pages[i - 1] - pages[i] != PMM_PAGE_SIZE) {
            contiguous = 0;
            break;
        }
    }
    if (!contiguous) {
        for (uint32_t i = 0; i < count; ++i) {
            pmm_free_page(pages[i]);
        }
        return -1;
    }

    *out_base = pages[count - 1];
    return 0;
}

int frame_ref(uint32_t page) {
    if (page == 0 || (page & (PMM_PAGE_SIZE - 1)) != 0) {
        return -1;
    }
    int32_t slot = find_tracked(page);
    if (slot >= 0) {
        ++g_tracked[slot].refcount;
        return 0;
    }
    for (uint32_t i = 0; i < FRAMEALLOC_TRACKED_CAPACITY; ++i) {
        if (g_tracked[i].page == 0) {
            g_tracked[i].page = page;
            g_tracked[i].refcount = 2; /* 1 (impliziter Erstbesitzer) + dieser neue Verweis */
            return 0;
        }
    }
    return -1;
}

int frame_unref(uint32_t page) {
    if (page == 0 || (page & (PMM_PAGE_SIZE - 1)) != 0) {
        return -1;
    }
    int32_t slot = find_tracked(page);
    if (slot < 0) {
        return pmm_free_page(page);
    }
    if (g_tracked[slot].refcount > 1) {
        --g_tracked[slot].refcount;
        return 0;
    }
    g_tracked[slot].page = 0;
    g_tracked[slot].refcount = 0;
    return pmm_free_page(page);
}

uint32_t frame_refcount(uint32_t page) {
    int32_t slot = find_tracked(page);
    if (slot < 0) return 1;
    return g_tracked[slot].refcount;
}

uint32_t frame_tracked_count(void) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < FRAMEALLOC_TRACKED_CAPACITY; ++i) {
        if (g_tracked[i].page != 0) ++count;
    }
    return count;
}

int framealloc_self_test(void) {
    framealloc_initialize();

    uint32_t numa;
    uint32_t a = frame_alloc(&numa);
    if (a == 0) return -1;
    if (frame_refcount(a) != 1) return -1;

    if (frame_ref(a) != 0) return -1;
    if (frame_refcount(a) != 2) return -1;
    if (frame_tracked_count() != 1) return -1;

    if (frame_unref(a) != 0) return -1;
    if (frame_refcount(a) != 1) return -1;

    if (frame_unref(a) != 0) return -1;
    if (frame_tracked_count() != 0) return -1;

    uint32_t base;
    if (frame_alloc_contiguous(4u, &base) != 0) return -1;
    if ((base & (PMM_PAGE_SIZE - 1)) != 0) return -1;
    for (uint32_t i = 0; i < 4u; ++i) {
        if (frame_unref(base + i * PMM_PAGE_SIZE) != 0) return -1;
    }

    if (frame_alloc_contiguous(0u, &base) == 0) return -1;
    if (frame_alloc_contiguous(FRAMEALLOC_MAX_CONTIGUOUS + 1u, &base) == 0) return -1;

    return 0;
}
