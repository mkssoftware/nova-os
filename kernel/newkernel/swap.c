/* NovaOS C-Kernel - Nova Swap (siehe swap.h fuer Scope). */
#include "swap.h"
#include "framealloc.h"
#include "pagetable.h"

#define SWAP_PAGE_SIZE 4096u
#define SWAP_SCRATCH_VIRT 0x36000000u /* eigener Scratch-Bereich, siehe pool.c/shared.c */

/* In-Memory-"Swap-Speicher" statt echtem Block-Geraet, siehe swap.h. */
static uint32_t g_swap_store[SWAP_SLOT_CAPACITY][SWAP_PAGE_SIZE / 4u];
static uint32_t g_slot_used[SWAP_SLOT_CAPACITY];

typedef struct {
    uint32_t page;
    uint32_t used;
} swap_pin_t;

static swap_pin_t g_pins[SWAP_PIN_CAPACITY];

void swap_initialize(void) {
    for (uint32_t i = 0; i < SWAP_SLOT_CAPACITY; ++i) g_slot_used[i] = 0;
    for (uint32_t i = 0; i < SWAP_PIN_CAPACITY; ++i) g_pins[i].used = 0;
}

int swap_pin(uint32_t page) {
    for (uint32_t i = 0; i < SWAP_PIN_CAPACITY; ++i) {
        if (g_pins[i].used && g_pins[i].page == page) return 0; /* bereits gepinnt */
    }
    for (uint32_t i = 0; i < SWAP_PIN_CAPACITY; ++i) {
        if (!g_pins[i].used) {
            g_pins[i].used = 1;
            g_pins[i].page = page;
            return 0;
        }
    }
    return -1;
}

int swap_unpin(uint32_t page) {
    for (uint32_t i = 0; i < SWAP_PIN_CAPACITY; ++i) {
        if (g_pins[i].used && g_pins[i].page == page) {
            g_pins[i].used = 0;
            return 0;
        }
    }
    return -1;
}

int swap_is_pinned(uint32_t page) {
    for (uint32_t i = 0; i < SWAP_PIN_CAPACITY; ++i) {
        if (g_pins[i].used && g_pins[i].page == page) return 1;
    }
    return 0;
}

int swap_out(uint32_t page, uint32_t *out_slot) {
    if (swap_is_pinned(page)) return -1;

    uint32_t slot = SWAP_SLOT_CAPACITY;
    for (uint32_t i = 0; i < SWAP_SLOT_CAPACITY; ++i) {
        if (!g_slot_used[i]) { slot = i; break; }
    }
    if (slot == SWAP_SLOT_CAPACITY) return -1;

    pagetable_map(SWAP_SCRATCH_VIRT, page, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint32_t *src = (volatile uint32_t *)(uintptr_t)SWAP_SCRATCH_VIRT;
    for (uint32_t i = 0; i < SWAP_PAGE_SIZE / 4u; ++i) {
        g_swap_store[slot][i] = src[i];
    }
    pagetable_unmap(SWAP_SCRATCH_VIRT);

    /* Erst NACH erfolgreicher Sicherung freigeben (Anforderung #5). */
    g_slot_used[slot] = 1;
    frame_unref(page);

    *out_slot = slot;
    return 0;
}

int swap_in(uint32_t slot, uint32_t *out_page) {
    if (slot >= SWAP_SLOT_CAPACITY || !g_slot_used[slot]) return -1;

    uint32_t numa;
    uint32_t page = frame_alloc(&numa);
    if (page == 0) return -1;

    pagetable_map(SWAP_SCRATCH_VIRT, page, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint32_t *dst = (volatile uint32_t *)(uintptr_t)SWAP_SCRATCH_VIRT;
    for (uint32_t i = 0; i < SWAP_PAGE_SIZE / 4u; ++i) {
        dst[i] = g_swap_store[slot][i];
    }
    pagetable_unmap(SWAP_SCRATCH_VIRT);

    g_slot_used[slot] = 0;
    *out_page = page;
    return 0;
}

uint32_t swap_used_slots(void) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < SWAP_SLOT_CAPACITY; ++i) {
        if (g_slot_used[i]) ++count;
    }
    return count;
}

uint32_t swap_total_slots(void) { return SWAP_SLOT_CAPACITY; }

int swap_self_test(void) {
    swap_initialize();

    uint32_t numa;
    uint32_t page = frame_alloc(&numa);
    if (page == 0) return -1;

    /* Inhalt setzen, der nach Out+In erhalten bleiben muss. */
    pagetable_map(SWAP_SCRATCH_VIRT, page, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint32_t *p = (volatile uint32_t *)(uintptr_t)SWAP_SCRATCH_VIRT;
    p[0] = 0x50414853u; /* "SWAP" */
    p[1] = 0xDEADBEEFu;
    pagetable_unmap(SWAP_SCRATCH_VIRT);

    /* Gepinnte Seiten duerfen nicht ausgelagert werden. */
    if (swap_pin(page) != 0) return -1;
    uint32_t bogus_slot;
    if (swap_out(page, &bogus_slot) == 0) return -1;
    if (swap_unpin(page) != 0) return -1;

    uint32_t slot;
    if (swap_out(page, &slot) != 0) return -1;
    if (swap_used_slots() != 1u) return -1;

    uint32_t restored_page;
    if (swap_in(slot, &restored_page) != 0) return -1;
    if (swap_used_slots() != 0u) return -1;

    pagetable_map(SWAP_SCRATCH_VIRT, restored_page, PAGETABLE_PRESENT | PAGETABLE_WRITE);
    volatile uint32_t *check = (volatile uint32_t *)(uintptr_t)SWAP_SCRATCH_VIRT;
    int ok = (check[0] == 0x50414853u && check[1] == 0xDEADBEEFu);
    pagetable_unmap(SWAP_SCRATCH_VIRT);
    if (!ok) { frame_unref(restored_page); return -1; }

    /* Ungueltiger Slot muss fehlschlagen. */
    uint32_t bogus_page;
    if (swap_in(slot, &bogus_page) == 0) { frame_unref(restored_page); return -1; }

    frame_unref(restored_page);
    return 0;
}
