/* NovaOS C-Kernel - Data-Movement (Port aus datamove32.inc, NPSPEC-DATAMOVE-*). */
#ifndef NOVA_DATAMOVE_H
#define NOVA_DATAMOVE_H

#include <stdint.h>

#define DM_DMA_MEM_TO_MEM 0u
#define DM_DMA_MEM_TO_DEV 1u
#define DM_DMA_DEV_TO_MEM 2u
#define DM_DMA_DEV_TO_DEV 3u

#define DM_FLAG_ZEROCOPY 0x01u
#define DM_FLAG_BYPASS   0x02u
#define DM_FLAG_SCATTER  0x04u
#define DM_FLAG_LOCAL    0x08u
#define DM_FLAG_SHARED   0x10u

#define DM_XFER_CAPACITY 8u
#define DM_RING_CAPACITY 4u

typedef struct {
    uint32_t addr;
    uint32_t len;
} dm_sge_t;

void datamove_initialize(void);
int datamove_self_test(void); /* 0 = ok */

/* 0 = Erfolg, *out_slot erhaelt den Transfer-Slot; -1 wenn Tabelle voll
   oder len==0. */
int dm_dma_transfer(uint32_t src, uint32_t dst, uint32_t len, uint32_t flags,
                     uint32_t *out_slot);

/* 0 = Erfolg, *out_ring_slot erhaelt den Slot; -1 wenn Tabelle voll. */
int dm_ringbuf_init(uint32_t base_addr, uint32_t capacity, uint32_t entry_size,
                     uint32_t *out_ring_slot);

/* Queued jeden SGE als einzelnen Zero-Copy-Scatter-Transfer. 0 = Erfolg,
   -1 wenn sge_count==0. */
int dm_scatter_gather(const dm_sge_t *sge_array, uint32_t sge_count, uint32_t dst);

#endif
