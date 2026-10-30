/* NovaOS C-Kernel - Address Space Layout Randomization (Port aus memory32.inc). */
#include "aslr.h"
#include "resilience.h"

typedef struct {
    uint32_t type;
    uint32_t base;
    uint32_t offset;
    uint32_t flags;
} aslr_record_t;

static aslr_record_t aslr_regions[ASLR_REGION_COUNT];
static uint32_t aslr_ready = 0;
static uint32_t aslr_seed = 0;
static uint32_t aslr_count = 0;

static inline uint64_t read_tsc(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

void aslr_initialize(void) {
    aslr_seed = (uint32_t)read_tsc() ^ resilience_global_tick();
    for (uint32_t i = 0; i < ASLR_REGION_COUNT; ++i) {
        aslr_regions[i] = (aslr_record_t){0};
    }
    aslr_ready = 1;
    aslr_count = 0;
}

static uint32_t aslr_next_random(void) {
    aslr_seed = aslr_seed * 1664525u + 1013904223u;
    return aslr_seed;
}

uint32_t aslr_randomize_region(uint32_t region_type, uint32_t base_hint) {
    uint32_t offset = aslr_next_random() & 0x000FF000u; /* max. 255*4KB = 1020 KB */

    if (aslr_count >= ASLR_REGION_COUNT) {
        return offset; /* kein freier Slot - Offset trotzdem liefern */
    }

    aslr_record_t *rec = &aslr_regions[aslr_count];
    rec->type = region_type;
    rec->base = base_hint;
    rec->offset = offset;
    rec->flags = 1;
    ++aslr_count;
    return offset;
}

int aslr_self_test(void) {
    if (aslr_ready != 1) return -1;

    uint32_t offset1 = aslr_randomize_region(ASLR_TYPE_STACK, 0xC0000000u);
    uint32_t offset2 = aslr_randomize_region(ASLR_TYPE_HEAP, 0xC0100000u);

    if ((offset1 & 0xFFFu) != 0) return -1;
    if ((offset2 & 0xFFFu) != 0) return -1;
    if (aslr_count != 2) return -1;

    return 0;
}
