/* NovaOS C-Kernel - Nova Memory Pools (siehe pool.h fuer Scope). */
#include "pool.h"
#include "framealloc.h"
#include "pagetable.h"

#define POOL_PAGE_SIZE 4096u
#define POOL_ZERO_SCRATCH_VIRT 0x39000000u /* eigener Scratch-Bereich, siehe pagetable.c/vas.c */

typedef struct {
    uint32_t id; /* 1-basiert, 0 = freier Slot */
    uint32_t capacity;
    uint32_t fallback;
    uint32_t flags;
    uint32_t used_count;
    uint32_t peak_usage;
    uint32_t alloc_failures;
    uint32_t out_pages[POOL_MAX_BLOCKS];
    uint32_t reserve_pages[POOL_MAX_BLOCKS];
    uint32_t reserve_count;
} pool_record_t;

static pool_record_t g_pools[POOL_CAPACITY];

void pool_initialize(void) {
    for (uint32_t i = 0; i < POOL_CAPACITY; ++i) {
        g_pools[i].id = 0;
    }
}

static pool_record_t *find_pool(uint32_t id) {
    if (id == 0 || id > POOL_CAPACITY) return 0;
    pool_record_t *rec = &g_pools[id - 1];
    if (rec->id != id) return 0;
    return rec;
}

/* Nullt einen physischen Frame ueber eine temporaere Scratch-Abbildung -
   Anforderung #11 "Speicher vorheriger Sicherheitskontexte DARF nicht
   unkontrolliert weitergegeben werden". */
static void zero_physical_frame(uint32_t phys) {
    pagetable_map(POOL_ZERO_SCRATCH_VIRT, phys, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint32_t *scratch = (volatile uint32_t *)(uintptr_t)POOL_ZERO_SCRATCH_VIRT;
    for (uint32_t i = 0; i < POOL_PAGE_SIZE / 4u; ++i) {
        scratch[i] = 0;
    }
    pagetable_unmap(POOL_ZERO_SCRATCH_VIRT);
}

int pool_create(uint32_t capacity, uint32_t fallback, uint32_t flags, uint32_t *out_id) {
    if (capacity == 0 || capacity > POOL_MAX_BLOCKS) return -1;
    if ((flags & POOL_FLAG_CRITICAL) && fallback == POOL_FALLBACK_HEAP) return -1;

    uint32_t slot = POOL_CAPACITY;
    for (uint32_t i = 0; i < POOL_CAPACITY; ++i) {
        if (g_pools[i].id == 0) { slot = i; break; }
    }
    if (slot == POOL_CAPACITY) return -1;

    pool_record_t *rec = &g_pools[slot];
    rec->capacity = capacity;
    rec->fallback = fallback;
    rec->flags = flags;
    rec->used_count = 0;
    rec->peak_usage = 0;
    rec->alloc_failures = 0;
    rec->reserve_count = 0;
    for (uint32_t i = 0; i < POOL_MAX_BLOCKS; ++i) rec->out_pages[i] = 0;

    if (flags & POOL_FLAG_EMERGENCY) {
        for (uint32_t i = 0; i < capacity; ++i) {
            uint32_t numa;
            uint32_t page = frame_alloc(&numa);
            if (page == 0) {
                for (uint32_t u = 0; u < i; ++u) {
                    frame_unref(rec->reserve_pages[u]);
                }
                return -1;
            }
            rec->reserve_pages[i] = page;
        }
        rec->reserve_count = capacity;
    }

    rec->id = slot + 1u;
    *out_id = rec->id;
    return 0;
}

int pool_destroy(uint32_t id) {
    pool_record_t *rec = find_pool(id);
    if (!rec || rec->used_count != 0) return -1;

    for (uint32_t i = 0; i < rec->reserve_count; ++i) {
        frame_unref(rec->reserve_pages[i]);
    }
    rec->reserve_count = 0;
    rec->id = 0;
    return 0;
}

static int track_out_page(pool_record_t *rec, uint32_t page) {
    for (uint32_t i = 0; i < POOL_MAX_BLOCKS; ++i) {
        if (rec->out_pages[i] == 0) {
            rec->out_pages[i] = page;
            ++rec->used_count;
            if (rec->used_count > rec->peak_usage) rec->peak_usage = rec->used_count;
            return 0;
        }
    }
    return -1;
}

int pool_alloc(uint32_t id, uint32_t *out_page) {
    pool_record_t *rec = find_pool(id);
    if (!rec) return -1;
    if (rec->flags & POOL_FLAG_EMERGENCY) return -1; /* Anforderung #8 */

    if (rec->used_count >= rec->capacity) {
        ++rec->alloc_failures;
        if (rec->fallback != POOL_FALLBACK_HEAP) {
            return -1;
        }
        /* Fallback: direkt vom allgemeinen Frame-Pool, NICHT gegen die
           harte Pool-Kapazitaet angerechnet (siehe pool.h-Scope-Hinweis -
           "Heap" bedeutet hier bewusst "allgemeiner physischer Pool
           ausserhalb dieses begrenzten Pools", nicht der Kernel-Heap).
           Solche Seiten werden NICHT in out_pages[] getrackt - sie muessen
           direkt ueber frame_unref() freigegeben werden, nicht via
           pool_free(). */
        uint32_t numa;
        uint32_t page = frame_alloc(&numa);
        if (page == 0) return -1;
        zero_physical_frame(page);
        *out_page = page;
        return 0;
    }

    uint32_t numa;
    uint32_t page = frame_alloc(&numa);
    if (page == 0) {
        ++rec->alloc_failures;
        return -1;
    }
    if (track_out_page(rec, page) != 0) {
        frame_unref(page);
        return -1;
    }
    zero_physical_frame(page);
    *out_page = page;
    return 0;
}

int pool_alloc_emergency(uint32_t id, uint32_t *out_page) {
    pool_record_t *rec = find_pool(id);
    if (!rec || !(rec->flags & POOL_FLAG_EMERGENCY)) return -1;
    if (rec->reserve_count == 0) {
        ++rec->alloc_failures;
        return -1;
    }
    uint32_t page = rec->reserve_pages[--rec->reserve_count];
    if (track_out_page(rec, page) != 0) {
        rec->reserve_pages[rec->reserve_count++] = page;
        return -1;
    }
    zero_physical_frame(page);
    *out_page = page;
    return 0;
}

int pool_free(uint32_t id, uint32_t page) {
    pool_record_t *rec = find_pool(id);
    if (!rec) return -1;

    for (uint32_t i = 0; i < POOL_MAX_BLOCKS; ++i) {
        if (rec->out_pages[i] == page) {
            rec->out_pages[i] = 0;
            --rec->used_count;
            if (rec->flags & POOL_FLAG_EMERGENCY) {
                rec->reserve_pages[rec->reserve_count++] = page;
            } else {
                frame_unref(page);
            }
            return 0;
        }
    }
    return -1;
}

int pool_usage(uint32_t id, uint32_t *out_used, uint32_t *out_peak, uint32_t *out_failures) {
    pool_record_t *rec = find_pool(id);
    if (!rec) return -1;
    *out_used = rec->used_count;
    *out_peak = rec->peak_usage;
    *out_failures = rec->alloc_failures;
    return 0;
}

int pool_self_test(void) {
    pool_initialize();

    uint32_t id;
    if (pool_create(2u, POOL_FALLBACK_NONE, POOL_FLAG_CRITICAL, &id) != 0) return -1;

    /* CRITICAL + HEAP-Fallback muss verweigert werden. */
    uint32_t bogus_id;
    if (pool_create(2u, POOL_FALLBACK_HEAP, POOL_FLAG_CRITICAL, &bogus_id) == 0) return -1;

    uint32_t page_a, page_b;
    if (pool_alloc(id, &page_a) != 0) return -1;
    if (pool_alloc(id, &page_b) != 0) return -1;
    if (page_a == page_b) return -1;

    uint32_t used, peak, failures;
    if (pool_usage(id, &used, &peak, &failures) != 0) return -1;
    if (used != 2u || peak != 2u || failures != 0u) return -1;

    /* Kapazitaet erschoepft, CRITICAL + NONE -> harte Ablehnung. */
    uint32_t page_c;
    if (pool_alloc(id, &page_c) == 0) return -1;
    if (pool_usage(id, &used, &peak, &failures) != 0) return -1;
    if (failures != 1u) return -1;

    if (pool_free(id, page_a) != 0) return -1;
    if (pool_free(id, page_b) != 0) return -1;
    /* Unbekannte Seite darf nicht als freigegeben gelten. */
    if (pool_free(id, 0xDEADB000u) == 0) return -1;

    if (pool_destroy(id) != 0) return -1;

    /* HEAP-Fallback-Pool: ueber Kapazitaet hinaus muss trotzdem ein Frame
       geliefert werden (uncounted). */
    uint32_t heap_id;
    if (pool_create(1u, POOL_FALLBACK_HEAP, 0u, &heap_id) != 0) return -1;
    uint32_t hp1, hp2;
    if (pool_alloc(heap_id, &hp1) != 0) return -1;
    if (pool_alloc(heap_id, &hp2) != 0) return -1; /* Fallback-Pfad */
    if (hp1 == hp2) return -1;
    if (pool_free(heap_id, hp1) != 0) return -1;
    /* hp2 kam aus dem Fallback-Pfad und ist nicht getrackt. */
    if (pool_free(heap_id, hp2) == 0) return -1;
    if (frame_unref(hp2) != 0) return -1;
    if (pool_destroy(heap_id) != 0) return -1;

    /* Emergency-Pool: normale pool_alloc() muss verweigert werden, nur
       pool_alloc_emergency() funktioniert. */
    uint32_t em_id;
    if (pool_create(2u, POOL_FALLBACK_NONE, POOL_FLAG_EMERGENCY, &em_id) != 0) return -1;
    uint32_t dummy;
    if (pool_alloc(em_id, &dummy) == 0) return -1;
    uint32_t em_a, em_b;
    if (pool_alloc_emergency(em_id, &em_a) != 0) return -1;
    if (pool_alloc_emergency(em_id, &em_b) != 0) return -1;
    if (pool_alloc_emergency(em_id, &dummy) == 0) return -1; /* Reserve erschoepft */
    if (pool_free(em_id, em_a) != 0) return -1;
    if (pool_free(em_id, em_b) != 0) return -1;
    if (pool_destroy(em_id) != 0) return -1;

    /* Kapazitaetsgrenze des Pool-Slot-Pools selbst. */
    uint32_t ids[POOL_CAPACITY];
    for (uint32_t i = 0; i < POOL_CAPACITY; ++i) {
        if (pool_create(1u, POOL_FALLBACK_NONE, 0u, &ids[i]) != 0) return -1;
    }
    uint32_t overflow_id;
    if (pool_create(1u, POOL_FALLBACK_NONE, 0u, &overflow_id) == 0) return -1;
    for (uint32_t i = 0; i < POOL_CAPACITY; ++i) {
        if (pool_destroy(ids[i]) != 0) return -1;
    }

    return 0;
}
