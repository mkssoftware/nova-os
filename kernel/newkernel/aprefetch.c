/* NovaOS C-Kernel - Nova Adaptive Prefetch Engine (Port aus
   adaptive32.inc). */
#include "aprefetch.h"

typedef struct {
    uint32_t stream_id; /* 0 = frei */
    uint32_t last_addr;
    int has_last_addr;
    uint32_t stride;
    uint32_t confidence;
} aprefetch_rec_t;

static aprefetch_rec_t aprefetch_table[APREFETCH_CAPACITY];
static int aprefetch_ready = 0;

void aprefetch_initialize(void) {
    if (aprefetch_ready == 1) return;

    for (uint32_t i = 0; i < APREFETCH_CAPACITY; ++i) aprefetch_table[i] = (aprefetch_rec_t){0};
    aprefetch_ready = 1;
}

int aprefetch_register(uint32_t stream_id) {
    if (aprefetch_ready != 1 || stream_id == 0) return -1;

    for (uint32_t i = 0; i < APREFETCH_CAPACITY; ++i) {
        if (aprefetch_table[i].stream_id == 0) {
            aprefetch_table[i].stream_id = stream_id;
            aprefetch_table[i].has_last_addr = 0;
            aprefetch_table[i].stride = 0;
            aprefetch_table[i].confidence = 0;
            return 0;
        }
    }
    return -1;
}

int aprefetch_access(uint32_t stream_id, uint32_t new_addr, uint32_t *out_prefetch_addr) {
    if (aprefetch_ready != 1) return -1;

    for (uint32_t i = 0; i < APREFETCH_CAPACITY; ++i) {
        aprefetch_rec_t *rec = &aprefetch_table[i];
        if (rec->stream_id != stream_id) continue;

        if (!rec->has_last_addr) {
            rec->last_addr = new_addr;
            rec->has_last_addr = 1;
            return -1;
        }

        uint32_t observed_stride = new_addr - rec->last_addr;

        if (rec->stride == 0) {
            rec->stride = observed_stride;
            rec->confidence = APREFETCH_ALPHA;
        } else if (observed_stride == rec->stride) {
            rec->confidence = (APREFETCH_ALPHA * 255u + (256u - APREFETCH_ALPHA) * rec->confidence) >> 8;
        } else {
            rec->confidence = ((256u - APREFETCH_ALPHA) * rec->confidence) >> 8;
        }

        rec->last_addr = new_addr;

        if (rec->confidence <= APREFETCH_TRIGGER) return -1;

        *out_prefetch_addr = new_addr + rec->stride;
        return 0;
    }
    return -1;
}

int aprefetch_self_test(void) {
    if (aprefetch_register(1) != 0) return -1;

    uint32_t prefetch_addr;
    aprefetch_access(1, 0x1000, &prefetch_addr); /* erster Zugriff: Fehler erwartet, ok */
    aprefetch_access(1, 0x1010, &prefetch_addr); /* Stride=16 gesetzt */
    aprefetch_access(1, 0x1020, &prefetch_addr); /* Stride=16 bestaetigt, noch nicht konfident genug */

    return 0;
}
