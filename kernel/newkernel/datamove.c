/* NovaOS C-Kernel - Data-Movement (Port aus datamove32.inc). */
#include "datamove.h"

typedef struct {
    uint32_t src;
    uint32_t dst;
    uint32_t len;
    uint32_t flags;
    uint32_t mode;
    uint32_t status; /* 0=pending 1=done 2=error */
    uint32_t channel;
} dm_xfer_t;

typedef struct {
    uint32_t base;
    uint32_t capacity;
    uint32_t head;
    uint32_t tail;
    uint32_t entry_size;
    uint32_t flags;
} dm_ring_t;

static dm_xfer_t dm_xfer_table[DM_XFER_CAPACITY];
static dm_ring_t dm_ring_table[DM_RING_CAPACITY];
static uint32_t dm_xfer_count = 0;
static uint32_t dm_ring_count = 0;
static int dm_ready = 0;

void datamove_initialize(void) {
    for (uint32_t i = 0; i < DM_XFER_CAPACITY; ++i) {
        dm_xfer_table[i] = (dm_xfer_t){0};
    }
    for (uint32_t i = 0; i < DM_RING_CAPACITY; ++i) {
        dm_ring_table[i] = (dm_ring_t){0};
    }
    dm_xfer_count = 0;
    dm_ring_count = 0;
    dm_ready = 1;
}

int dm_dma_transfer(uint32_t src, uint32_t dst, uint32_t len, uint32_t flags,
                     uint32_t *out_slot) {
    if (dm_xfer_count >= DM_XFER_CAPACITY) {
        return -1;
    }
    if (len == 0) {
        return -1;
    }
    dm_xfer_t *xfer = &dm_xfer_table[dm_xfer_count];
    xfer->src = src;
    xfer->dst = dst;
    xfer->len = len;
    xfer->flags = flags;
    xfer->mode = (flags & DM_FLAG_BYPASS) ? DM_DMA_MEM_TO_DEV : DM_DMA_MEM_TO_MEM;
    xfer->channel = dm_xfer_count & 7u;
    xfer->status = 1; /* sofort "done" (kein echter DMA-HW) */

    if (out_slot) {
        *out_slot = dm_xfer_count;
    }
    ++dm_xfer_count;
    return 0;
}

int dm_ringbuf_init(uint32_t base_addr, uint32_t capacity, uint32_t entry_size,
                     uint32_t *out_ring_slot) {
    if (dm_ring_count >= DM_RING_CAPACITY) {
        return -1;
    }
    dm_ring_t *ring = &dm_ring_table[dm_ring_count];
    ring->base = base_addr;
    ring->capacity = capacity;
    ring->entry_size = entry_size;
    ring->head = 0;
    ring->tail = 0;
    ring->flags = 0;

    if (out_ring_slot) {
        *out_ring_slot = dm_ring_count;
    }
    ++dm_ring_count;
    return 0;
}

int dm_scatter_gather(const dm_sge_t *sge_array, uint32_t sge_count, uint32_t dst) {
    if (sge_count == 0) {
        return -1;
    }
    for (uint32_t i = 0; i < sge_count; ++i) {
        dm_dma_transfer(sge_array[i].addr, dst, sge_array[i].len,
                         DM_FLAG_ZEROCOPY | DM_FLAG_SCATTER, 0);
    }
    return 0;
}

int datamove_self_test(void) {
    if (dm_ready != 1) return -1;

    uint32_t slot = 0;
    if (dm_dma_transfer(0x200000u, 0x201000u, 0x1000u, DM_FLAG_ZEROCOPY, &slot) != 0) {
        return -1;
    }

    uint32_t ring_slot = 0;
    if (dm_ringbuf_init(0x210000u, 512, 64, &ring_slot) != 0) {
        return -1;
    }

    return 0;
}
