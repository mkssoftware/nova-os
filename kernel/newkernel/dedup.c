/* NovaOS C-Kernel - Nova Memory Deduplication (siehe dedup.h fuer Scope). */
#include "dedup.h"
#include "framealloc.h"
#include "pagetable.h"

#define DEDUP_PAGE_SIZE 4096u
#define DEDUP_SCRATCH_VIRT_A 0x34000000u /* eigene Scratch-Bereiche, siehe compress.c/swap.c */
#define DEDUP_SCRATCH_VIRT_B 0x34100000u

static uint32_t g_scan_count = 0;
static uint32_t g_merge_count = 0;
static uint32_t g_false_positive_count = 0;

void dedup_initialize(void) {
    g_scan_count = 0;
    g_merge_count = 0;
    g_false_positive_count = 0;
}

uint32_t dedup_hash_page(uint32_t page) {
    pagetable_map(DEDUP_SCRATCH_VIRT_A, page, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *bytes = (volatile uint8_t *)(uintptr_t)DEDUP_SCRATCH_VIRT_A;

    /* FNV-1a, 32-Bit - nur ein schneller Vorfilter (Anforderung #3). */
    uint32_t hash = 0x811C9DC5u;
    for (uint32_t i = 0; i < DEDUP_PAGE_SIZE; ++i) {
        hash ^= bytes[i];
        hash *= 0x01000193u;
    }
    pagetable_unmap(DEDUP_SCRATCH_VIRT_A);
    return hash;
}

int dedup_check_pages(uint32_t page_a, uint32_t page_b) {
    ++g_scan_count;

    if (page_a == page_b) return 1;

    uint32_t hash_a = dedup_hash_page(page_a);
    uint32_t hash_b = dedup_hash_page(page_b);
    if (hash_a != hash_b) return 0;

    /* Anforderung #4: Hash-Treffer ist nur ein Vorfilter - vollstaendigen
       Inhalt verifizieren, bevor ein Merge erlaubt wird. */
    pagetable_map(DEDUP_SCRATCH_VIRT_A, page_a, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    pagetable_map(DEDUP_SCRATCH_VIRT_B, page_b, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *bytes_a = (volatile uint8_t *)(uintptr_t)DEDUP_SCRATCH_VIRT_A;
    volatile uint8_t *bytes_b = (volatile uint8_t *)(uintptr_t)DEDUP_SCRATCH_VIRT_B;

    int identical = 1;
    for (uint32_t i = 0; i < DEDUP_PAGE_SIZE; ++i) {
        if (bytes_a[i] != bytes_b[i]) { identical = 0; break; }
    }
    pagetable_unmap(DEDUP_SCRATCH_VIRT_A);
    pagetable_unmap(DEDUP_SCRATCH_VIRT_B);

    if (!identical) {
        ++g_false_positive_count; /* Hash-Kollision ohne echten Inhaltstreffer */
        return 0;
    }
    return 1;
}

int dedup_merge(uint32_t keep_page, uint32_t redundant_page) {
    if (frame_ref(keep_page) != 0) return -1;
    frame_unref(redundant_page);
    ++g_merge_count;
    return 0;
}

uint32_t dedup_scan_count(void) { return g_scan_count; }
uint32_t dedup_merge_count(void) { return g_merge_count; }
uint32_t dedup_false_positive_count(void) { return g_false_positive_count; }

int dedup_self_test(void) {
    dedup_initialize();

    uint32_t numa;
    uint32_t page_a = frame_alloc(&numa);
    uint32_t page_b = frame_alloc(&numa);
    uint32_t page_c = frame_alloc(&numa);
    if (page_a == 0 || page_b == 0 || page_c == 0) return -1;

    /* a und b: identischer Inhalt. c: unterschiedlich. */
    pagetable_map(DEDUP_SCRATCH_VIRT_A, page_a, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *bytes = (volatile uint8_t *)(uintptr_t)DEDUP_SCRATCH_VIRT_A;
    for (uint32_t i = 0; i < DEDUP_PAGE_SIZE; ++i) bytes[i] = (uint8_t)(i & 0xFFu);
    pagetable_unmap(DEDUP_SCRATCH_VIRT_A);

    pagetable_map(DEDUP_SCRATCH_VIRT_A, page_b, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *bytes_b = (volatile uint8_t *)(uintptr_t)DEDUP_SCRATCH_VIRT_A;
    for (uint32_t i = 0; i < DEDUP_PAGE_SIZE; ++i) bytes_b[i] = (uint8_t)(i & 0xFFu);
    pagetable_unmap(DEDUP_SCRATCH_VIRT_A);

    pagetable_map(DEDUP_SCRATCH_VIRT_A, page_c, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *bytes_c = (volatile uint8_t *)(uintptr_t)DEDUP_SCRATCH_VIRT_A;
    for (uint32_t i = 0; i < DEDUP_PAGE_SIZE; ++i) bytes_c[i] = (uint8_t)((i + 1u) & 0xFFu);
    pagetable_unmap(DEDUP_SCRATCH_VIRT_A);

    if (dedup_hash_page(page_a) != dedup_hash_page(page_b)) return -1;

    if (!dedup_check_pages(page_a, page_b)) return -1;
    if (dedup_check_pages(page_a, page_c)) return -1;
    if (dedup_false_positive_count() != 0u) return -1; /* echte Hash-Kollision hier unwahrscheinlich */

    if (frame_refcount(page_a) != 1u) return -1;
    if (dedup_merge(page_a, page_b) != 0) return -1;
    if (frame_refcount(page_a) != 2u) return -1;
    if (dedup_merge_count() != 1u) return -1;

    if (frame_unref(page_a) != 0) return -1;
    if (frame_unref(page_a) != 0) return -1;
    if (frame_unref(page_c) != 0) return -1;

    return 0;
}
