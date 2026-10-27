/* NovaOS C-Kernel - Nova Adaptive Cache Manager (Port aus adaptive32.inc). */
#include "acache.h"
#include "aschedsw.h"

typedef struct {
    uint32_t line_id; /* 0 = frei */
    uint32_t hits;
    uint32_t misses;
    uint32_t hit_rate;
} acache_rec_t;

static acache_rec_t acache_table[ACACHE_CAPACITY];
static int acache_ready = 0;

void acache_initialize(void) {
    if (acache_ready == 1) return;

    for (uint32_t i = 0; i < ACACHE_CAPACITY; ++i) acache_table[i] = (acache_rec_t){0};
    acache_ready = 1;
}

int acache_register(uint32_t line_id) {
    if (acache_ready != 1 || line_id == 0) return -1;

    for (uint32_t i = 0; i < ACACHE_CAPACITY; ++i) {
        if (acache_table[i].line_id == line_id) return -1; /* bereits registriert */
    }

    for (uint32_t i = 0; i < ACACHE_CAPACITY; ++i) {
        if (acache_table[i].line_id == 0) {
            acache_table[i].line_id = line_id;
            acache_table[i].hits = 0;
            acache_table[i].misses = 0;
            acache_table[i].hit_rate = 128;
            return 0;
        }
    }
    return -1;
}

int acache_record(uint32_t line_id, uint32_t is_hit) {
    if (acache_ready != 1) return -1;

    for (uint32_t i = 0; i < ACACHE_CAPACITY; ++i) {
        acache_rec_t *rec = &acache_table[i];
        if (rec->line_id != line_id) continue;

        if (is_hit) {
            ++rec->hits;
            rec->hit_rate = (ACACHE_ALPHA * 255u + (256u - ACACHE_ALPHA) * rec->hit_rate) >> 8;
        } else {
            ++rec->misses;
            rec->hit_rate = ((256u - ACACHE_ALPHA) * rec->hit_rate) >> 8;
        }

        if (rec->hit_rate < ACACHE_THRESHOLD) {
            aschedsw_feedback(3, -48);
        }
        return 0;
    }
    return -1;
}

int acache_query(uint32_t line_id, uint32_t *out_hit_rate, uint32_t *out_hits, uint32_t *out_misses) {
    if (acache_ready != 1) return -1;

    for (uint32_t i = 0; i < ACACHE_CAPACITY; ++i) {
        if (acache_table[i].line_id == line_id) {
            *out_hit_rate = acache_table[i].hit_rate;
            *out_hits = acache_table[i].hits;
            *out_misses = acache_table[i].misses;
            return 0;
        }
    }
    return -1;
}

int acache_self_test(void) {
    if (acache_register(1) != 0) return -1;

    if (acache_record(1, 1) != 0) return -1;

    uint32_t hit_rate, hits, misses;
    if (acache_query(1, &hit_rate, &hits, &misses) != 0) return -1;
    if (hit_rate == 0) return -1;

    return 0;
}
