/* NovaOS C-Kernel - Nova Adaptive Preload Manager (Port aus
   adaptive32.inc). */
#include "apreload.h"
#include "acache.h"

typedef struct {
    uint32_t obj_id; /* 0 = frei */
    uint32_t priority;
    uint32_t state;
} apreload_rec_t;

static apreload_rec_t apreload_table[APRELOAD_CAPACITY];
static int apreload_ready = 0;

void apreload_initialize(void) {
    if (apreload_ready == 1) return;

    for (uint32_t i = 0; i < APRELOAD_CAPACITY; ++i) apreload_table[i] = (apreload_rec_t){0};
    apreload_ready = 1;
}

int apreload_enqueue(uint32_t obj_id, uint32_t priority) {
    if (apreload_ready != 1 || obj_id == 0) return -1;

    for (uint32_t i = 0; i < APRELOAD_CAPACITY; ++i) {
        if (apreload_table[i].obj_id == 0 && apreload_table[i].state == APRELOAD_PENDING) {
            apreload_table[i].obj_id = obj_id;
            apreload_table[i].priority = priority;
            apreload_table[i].state = APRELOAD_PENDING;
            return 0;
        }
    }
    return -1;
}

int apreload_dequeue_highest(uint32_t *out_obj_id) {
    if (apreload_ready != 1) return -1;

    uint32_t best_priority = 0;
    int best_slot = -1;

    for (uint32_t i = 0; i < APRELOAD_CAPACITY; ++i) {
        apreload_rec_t *rec = &apreload_table[i];
        if (rec->state != APRELOAD_PENDING || rec->obj_id == 0) continue;
        if (rec->priority <= best_priority) continue;

        best_priority = rec->priority;
        best_slot = (int)i;
    }

    if (best_slot < 0) return -1;

    apreload_table[best_slot].state = APRELOAD_LOADING;
    *out_obj_id = apreload_table[best_slot].obj_id;
    return 0;
}

int apreload_complete(uint32_t obj_id) {
    if (apreload_ready != 1) return -1;

    for (uint32_t i = 0; i < APRELOAD_CAPACITY; ++i) {
        if (apreload_table[i].obj_id == obj_id) {
            apreload_table[i].state = APRELOAD_READY;

            acache_register(obj_id); /* Rueckgabe ignoriert, falls bereits registriert */
            acache_record(obj_id, 1);
            return 0;
        }
    }
    return -1;
}

int apreload_self_test(void) {
    if (apreload_enqueue(1, 5) != 0) return -1;
    if (apreload_enqueue(2, 10) != 0) return -1;

    uint32_t obj_id;
    if (apreload_dequeue_highest(&obj_id) != 0) return -1;
    if (obj_id != 2) return -1; /* hoehere Prioritaet zuerst */

    if (apreload_complete(2) != 0) return -1;

    return 0;
}
