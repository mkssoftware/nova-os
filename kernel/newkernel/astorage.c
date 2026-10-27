/* NovaOS C-Kernel - Nova Adaptive Storage Manager (Port aus
   adaptive32.inc). */
#include "astorage.h"

#define ASTOR_HOT_THRESH  100u
#define ASTOR_COLD_THRESH 20u

typedef struct {
    uint32_t vol_id; /* 0 = frei */
    uint32_t io_rate;
    uint32_t tier;
    uint32_t total_ops;
} astorage_rec_t;

static astorage_rec_t astorage_table[ASTOR_CAPACITY];
static int astorage_ready = 0;

void astorage_initialize(void) {
    if (astorage_ready == 1) return;

    for (uint32_t i = 0; i < ASTOR_CAPACITY; ++i) astorage_table[i] = (astorage_rec_t){0};
    astorage_ready = 1;
}

int astorage_register(uint32_t vol_id) {
    if (astorage_ready != 1 || vol_id == 0) return -1;

    for (uint32_t i = 0; i < ASTOR_CAPACITY; ++i) {
        if (astorage_table[i].vol_id == 0) {
            astorage_table[i].vol_id = vol_id;
            astorage_table[i].io_rate = 50;
            astorage_table[i].tier = ASTOR_WARM;
            astorage_table[i].total_ops = 0;
            return 0;
        }
    }
    return -1;
}

int astorage_record_io(uint32_t vol_id, uint32_t ops_this_tick) {
    if (astorage_ready != 1) return -1;

    for (uint32_t i = 0; i < ASTOR_CAPACITY; ++i) {
        astorage_rec_t *rec = &astorage_table[i];
        if (rec->vol_id != vol_id) continue;

        rec->total_ops += ops_this_tick;
        rec->io_rate = (ASTOR_ALPHA * ops_this_tick + (256u - ASTOR_ALPHA) * rec->io_rate) >> 8;

        if (rec->io_rate >= ASTOR_HOT_THRESH) {
            rec->tier = ASTOR_HOT;
        } else if (rec->io_rate <= ASTOR_COLD_THRESH) {
            rec->tier = ASTOR_COLD;
        } else {
            rec->tier = ASTOR_WARM;
        }
        return 0;
    }
    return -1;
}

int astorage_query(uint32_t vol_id, uint32_t *out_tier, uint32_t *out_io_rate,
                    uint32_t *out_total_ops) {
    if (astorage_ready != 1) return -1;

    for (uint32_t i = 0; i < ASTOR_CAPACITY; ++i) {
        if (astorage_table[i].vol_id == vol_id) {
            *out_tier = astorage_table[i].tier;
            *out_io_rate = astorage_table[i].io_rate;
            *out_total_ops = astorage_table[i].total_ops;
            return 0;
        }
    }
    return -1;
}

int astorage_self_test(void) {
    if (astorage_register(1) != 0) return -1;

    if (astorage_record_io(1, 512) != 0) return -1; /* EWMA = (32*512+224*50)/256 = 107 -> HOT */

    uint32_t tier, io_rate, total_ops;
    if (astorage_query(1, &tier, &io_rate, &total_ops) != 0) return -1;
    if (tier != ASTOR_HOT) return -1;

    return 0;
}
