/* NovaOS C-Kernel - Bootstrap-Kernel-Heap (Port aus memory32.inc).
   Bootstrap-Allokationen sind maximal eine Seite gross und werden auf
   HEAP_ALIGNMENT ausgerichtet; neue Seiten werden deterministisch genullt. */
#include "heap.h"

#define PMM_PAGE_SIZE 4096u

static uint8_t *heap_page = 0;
static uint32_t heap_offset = PMM_PAGE_SIZE;
static uint32_t heap_allocations = 0;
static uint32_t heap_bytes = 0;

/* TODO: durch den echten Physical-Memory-Manager (E820/NUMA, pmm_alloc_page)
   ersetzen, sobald dieser portiert ist. Bis dahin: statisches Seiten-Arena. */
#define BOOTSTRAP_PAGE_ARENA_PAGES 64u
static uint8_t bootstrap_page_arena[BOOTSTRAP_PAGE_ARENA_PAGES][PMM_PAGE_SIZE] __attribute__((aligned(16)));
static uint32_t bootstrap_page_arena_next = 0;

static void *bootstrap_pmm_alloc_page(void) {
    if (bootstrap_page_arena_next >= BOOTSTRAP_PAGE_ARENA_PAGES) {
        return 0;
    }
    return bootstrap_page_arena[bootstrap_page_arena_next++];
}

void heap_initialize(void) {
    heap_page = 0;
    heap_offset = PMM_PAGE_SIZE;
    heap_allocations = 0;
    heap_bytes = 0;
}

void *heap_allocate(uint32_t size) {
    if (size == 0 || size > PMM_PAGE_SIZE) {
        return 0;
    }
    uint32_t aligned_size = (size + HEAP_ALIGNMENT - 1u) & ~(HEAP_ALIGNMENT - 1u);
    if (aligned_size < size) { /* Ueberlauf wie im ASM (jc .invalid) */
        return 0;
    }

    uint32_t offset = heap_offset;
    uint32_t end = offset + aligned_size;
    if (end < offset) {
        return 0;
    }

    if (end > PMM_PAGE_SIZE) {
        uint8_t *new_page = bootstrap_pmm_alloc_page();
        if (!new_page) {
            return 0;
        }
        heap_page = new_page;
        offset = 0;
        /* Neue Heap-Seiten werden deterministisch genullt (bootstrap_page_arena
           liefert bereits genullte Seiten, daher kein expliziter memset hier -
           sobald der echte PMM nicht-garantiert-genullte Seiten liefert, muss
           hier wieder genullt werden). */
    }

    void *result = heap_page + offset;
    heap_offset = offset + aligned_size;
    ++heap_allocations;
    heap_bytes += aligned_size;
    return result;
}

int heap_self_test(void) {
    uint32_t *first = (uint32_t *)heap_allocate(64);
    if (!first) return -1;
    if (((uintptr_t)first & (HEAP_ALIGNMENT - 1u)) != 0) return -1;
    first[0] = 0x4E4F5641u;
    first[15] = 0x48454150u; /* Offset 60 */

    uint32_t *second = (uint32_t *)heap_allocate(128);
    if (!second) return -1;
    if (((uintptr_t)second & (HEAP_ALIGNMENT - 1u)) != 0) return -1;
    if ((uint8_t *)second <= (uint8_t *)first) return -1;
    if (first[0] != 0x4E4F5641u) return -1;
    if (first[15] != 0x48454150u) return -1;
    if (second[0] != 0) return -1;

    return 0;
}
