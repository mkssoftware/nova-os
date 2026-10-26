/* NovaOS C-Kernel - Nova RT Temporal Isolation (Port aus adaptive32.inc). */
#include "tmpiso.h"

typedef struct {
    uint32_t id;
    uint32_t budget;
    uint32_t used;
    uint32_t epoch;
} tmpiso_rec_t;

static tmpiso_rec_t tmpiso_partitions[TMPISO_CAPACITY];
static int tmpiso_ready = 0;
static uint32_t tmpiso_epoch = 0;
static uint32_t tmpiso_violations = 0;

void tmpiso_initialize(void) {
    for (uint32_t i = 0; i < TMPISO_CAPACITY; ++i) {
        tmpiso_partitions[i].id = i;
        tmpiso_partitions[i].budget = 250;
        tmpiso_partitions[i].used = 0;
        tmpiso_partitions[i].epoch = 0;
    }
    tmpiso_epoch = 0;
    tmpiso_violations = 0;
    tmpiso_ready = 1;
}

int tmpiso_admit(uint32_t partition, uint32_t ticks_needed) {
    if (tmpiso_ready != 1) return -1;
    if (partition >= TMPISO_CAPACITY) return -1;

    tmpiso_rec_t *rec = &tmpiso_partitions[partition];
    uint32_t prospective = rec->used + ticks_needed;
    if (prospective > rec->budget) {
        ++tmpiso_violations;
        return -1;
    }

    rec->used = prospective;
    return 0;
}

void tmpiso_new_epoch(void) {
    if (tmpiso_ready != 1) return;

    ++tmpiso_epoch;
    for (uint32_t i = 0; i < TMPISO_CAPACITY; ++i) {
        tmpiso_partitions[i].used = 0;
        tmpiso_partitions[i].epoch = tmpiso_epoch;
    }
}

int tmpiso_query(uint32_t partition, uint32_t *out_budget, uint32_t *out_used) {
    if (partition >= TMPISO_CAPACITY) return -1;

    *out_budget = tmpiso_partitions[partition].budget;
    *out_used = tmpiso_partitions[partition].used;
    return 0;
}

int tmpiso_self_test(void) {
    if (tmpiso_admit(0, 100) != 0) return -1;
    if (tmpiso_admit(0, 100) != 0) return -1; /* used=200, noch <=250 */
    if (tmpiso_admit(0, 100) == 0) return -1; /* used wuerde 300>250: abgelehnt */

    tmpiso_new_epoch();
    if (tmpiso_admit(0, 100) != 0) return -1; /* nach Reset wieder frei */

    return 0;
}
