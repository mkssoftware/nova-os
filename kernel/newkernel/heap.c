/* NovaOS C-Kernel - Bootstrap-Kernel-Heap (Port aus memory32.inc).
   Bootstrap-Allokationen sind maximal eine Seite gross und werden auf
   HEAP_ALIGNMENT ausgerichtet; neue Seiten werden deterministisch genullt. */
#include "heap.h"
#include "pmm.h"

static uint8_t *heap_page = 0;
static uint32_t heap_offset = PMM_PAGE_SIZE;
static uint32_t heap_allocations = 0;
static uint32_t heap_bytes = 0;

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
        uint32_t numa_node;
        uint32_t new_page_addr = pmm_alloc_page(&numa_node);
        if (new_page_addr == 0) {
            return 0;
        }
        heap_page = (uint8_t *)(uintptr_t)new_page_addr;
        offset = 0;

        /* Neue Heap-Seiten werden deterministisch genullt (wie im alten ASM). */
        uint32_t *words = (uint32_t *)heap_page;
        for (uint32_t i = 0; i < PMM_PAGE_SIZE / 4; ++i) {
            words[i] = 0;
        }
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
