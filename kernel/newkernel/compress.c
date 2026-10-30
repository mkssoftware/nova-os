/* NovaOS C-Kernel - Nova Memory Compression (siehe compress.h fuer Scope).
   Echter, aber bewusst einfacher Byte-RLE-Codec (Run-Length Encoding). */
#include "compress.h"
#include "framealloc.h"
#include "pagetable.h"

#define COMPRESS_PAGE_SIZE 4096u
#define COMPRESS_SCRATCH_VIRT 0x35000000u /* eigener Scratch-Bereich, siehe swap.c/pool.c */

typedef struct {
    uint32_t used;
    uint32_t compressed_length;
    uint8_t data[COMPRESS_MAX_OUTPUT];
} compress_slot_t;

static compress_slot_t g_slots[COMPRESS_POOL_CAPACITY];
static uint32_t g_bytes_saved = 0;

void compress_initialize(void) {
    for (uint32_t i = 0; i < COMPRESS_POOL_CAPACITY; ++i) g_slots[i].used = 0;
    g_bytes_saved = 0;
}

int compress_page(uint32_t page, uint32_t *out_slot) {
    uint32_t slot = COMPRESS_POOL_CAPACITY;
    for (uint32_t i = 0; i < COMPRESS_POOL_CAPACITY; ++i) {
        if (!g_slots[i].used) { slot = i; break; }
    }
    if (slot == COMPRESS_POOL_CAPACITY) return -1;

    pagetable_map(COMPRESS_SCRATCH_VIRT, page, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *bytes = (volatile uint8_t *)(uintptr_t)COMPRESS_SCRATCH_VIRT;

    uint8_t *out = g_slots[slot].data;
    uint32_t in_pos = 0;
    uint32_t out_pos = 0;
    while (in_pos < COMPRESS_PAGE_SIZE) {
        uint8_t value = bytes[in_pos];
        uint32_t run = 1;
        while ((in_pos + run) < COMPRESS_PAGE_SIZE && bytes[in_pos + run] == value && run < 255u) {
            ++run;
        }
        out[out_pos++] = (uint8_t)run;
        out[out_pos++] = value;
        in_pos += run;
    }
    pagetable_unmap(COMPRESS_SCRATCH_VIRT);

    /* Anforderung #6: nur bei sinnvollem Gewinn tatsaechlich komprimieren. */
    uint32_t threshold = (COMPRESS_PAGE_SIZE * COMPRESS_MIN_RATIO_PERCENT) / 100u;
    if (out_pos > threshold) {
        return -1; /* Frame bleibt unveraendert, Slot war nie belegt */
    }

    g_slots[slot].compressed_length = out_pos;
    g_slots[slot].used = 1;
    g_bytes_saved += (COMPRESS_PAGE_SIZE - out_pos);

    /* Anforderung #4: Original-Frame erst JETZT freigeben. */
    frame_unref(page);

    *out_slot = slot;
    return 0;
}

int decompress_page(uint32_t slot, uint32_t *out_page) {
    if (slot >= COMPRESS_POOL_CAPACITY || !g_slots[slot].used) return -1;

    uint32_t numa;
    uint32_t page = frame_alloc(&numa);
    if (page == 0) return -1;

    pagetable_map(COMPRESS_SCRATCH_VIRT, page, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *bytes = (volatile uint8_t *)(uintptr_t)COMPRESS_SCRATCH_VIRT;

    const uint8_t *in = g_slots[slot].data;
    uint32_t in_pos = 0;
    uint32_t out_pos = 0;
    while (out_pos < COMPRESS_PAGE_SIZE) {
        uint8_t run = in[in_pos++];
        uint8_t value = in[in_pos++];
        for (uint8_t i = 0; i < run; ++i) {
            bytes[out_pos++] = value;
        }
    }
    pagetable_unmap(COMPRESS_SCRATCH_VIRT);

    g_bytes_saved -= (COMPRESS_PAGE_SIZE - g_slots[slot].compressed_length);
    g_slots[slot].used = 0;

    *out_page = page;
    return 0;
}

uint32_t compress_used_slots(void) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < COMPRESS_POOL_CAPACITY; ++i) {
        if (g_slots[i].used) ++count;
    }
    return count;
}

uint32_t compress_total_bytes_saved(void) { return g_bytes_saved; }

int compress_self_test(void) {
    compress_initialize();

    /* Fall 1: hochkomprimierbare Seite (lange gleiche Byte-Laeufe). */
    uint32_t numa;
    uint32_t page_a = frame_alloc(&numa);
    if (page_a == 0) return -1;

    pagetable_map(COMPRESS_SCRATCH_VIRT, page_a, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *bytes = (volatile uint8_t *)(uintptr_t)COMPRESS_SCRATCH_VIRT;
    for (uint32_t i = 0; i < COMPRESS_PAGE_SIZE; ++i) {
        bytes[i] = 0x00u; /* komplett genullt = maximale RLE-Kompression */
    }
    pagetable_unmap(COMPRESS_SCRATCH_VIRT);

    uint32_t slot_a;
    if (compress_page(page_a, &slot_a) != 0) return -1;
    if (compress_used_slots() != 1u) return -1;
    if (compress_total_bytes_saved() == 0u) return -1;

    uint32_t restored_a;
    if (decompress_page(slot_a, &restored_a) != 0) return -1;
    if (compress_used_slots() != 0u) return -1;

    pagetable_map(COMPRESS_SCRATCH_VIRT, restored_a, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *check = (volatile uint8_t *)(uintptr_t)COMPRESS_SCRATCH_VIRT;
    int ok = 1;
    for (uint32_t i = 0; i < COMPRESS_PAGE_SIZE; ++i) {
        if (check[i] != 0x00u) { ok = 0; break; }
    }
    pagetable_unmap(COMPRESS_SCRATCH_VIRT);
    if (!ok) { frame_unref(restored_a); return -1; }
    frame_unref(restored_a);

    /* Fall 2: inkompressible Seite (jedes Byte anders -> RLE verdoppelt
       die Groesse) - Anforderung #6, muss abgelehnt werden, Frame bleibt
       unveraendert nutzbar. */
    uint32_t page_b = frame_alloc(&numa);
    if (page_b == 0) return -1;

    pagetable_map(COMPRESS_SCRATCH_VIRT, page_b, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *bytes_b = (volatile uint8_t *)(uintptr_t)COMPRESS_SCRATCH_VIRT;
    for (uint32_t i = 0; i < COMPRESS_PAGE_SIZE; ++i) {
        bytes_b[i] = (uint8_t)(i * 97u + 13u); /* kein Lauf laenger als 1 */
    }
    pagetable_unmap(COMPRESS_SCRATCH_VIRT);

    uint32_t slot_b;
    if (compress_page(page_b, &slot_b) == 0) { frame_unref(page_b); return -1; }
    if (compress_used_slots() != 0u) { frame_unref(page_b); return -1; }

    /* Frame muss weiterhin unveraendert lesbar sein (wurde NICHT
       freigegeben). */
    pagetable_map(COMPRESS_SCRATCH_VIRT, page_b, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint8_t *recheck = (volatile uint8_t *)(uintptr_t)COMPRESS_SCRATCH_VIRT;
    int intact = (recheck[0] == (uint8_t)13u && recheck[1] == (uint8_t)(97u + 13u));
    pagetable_unmap(COMPRESS_SCRATCH_VIRT);
    frame_unref(page_b);
    if (!intact) return -1;

    return 0;
}
