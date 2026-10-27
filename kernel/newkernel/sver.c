/* NovaOS C-Kernel - Nova State Versioning (Port aus adaptive32.inc). */
#include "sver.h"

typedef struct {
    uint32_t obj_id; /* 0 = frei */
    uint32_t version;
    uint32_t generation;
    uint32_t parent_version;
} sver_rec_t;

static sver_rec_t sver_table[SVER_CAPACITY];
static int sver_ready = 0;
static uint32_t sver_gen_counter = 1;

void sver_initialize(void) {
    if (sver_ready == 1) return;

    for (uint32_t i = 0; i < SVER_CAPACITY; ++i) sver_table[i] = (sver_rec_t){0};
    sver_gen_counter = 1;
    sver_ready = 1;
}

int sver_create(uint32_t obj_id, uint32_t *out_slot) {
    if (sver_ready != 1 || obj_id == 0) return -1;

    for (uint32_t i = 0; i < SVER_CAPACITY; ++i) {
        if (sver_table[i].obj_id == 0) {
            sver_table[i].obj_id = obj_id;
            sver_table[i].version = 1;
            sver_table[i].generation = sver_gen_counter++;
            sver_table[i].parent_version = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int sver_bump(uint32_t obj_id, uint32_t *out_new_version) {
    if (sver_ready != 1) return -1;

    for (uint32_t i = 0; i < SVER_CAPACITY; ++i) {
        sver_rec_t *rec = &sver_table[i];
        if (rec->obj_id != obj_id) continue;

        rec->parent_version = rec->version;
        ++rec->version;
        rec->generation = sver_gen_counter++;
        *out_new_version = rec->version;
        return 0;
    }
    return -1;
}

int sver_query(uint32_t obj_id, uint32_t *out_version, uint32_t *out_parent_version,
               uint32_t *out_generation) {
    if (sver_ready != 1) return -1;

    for (uint32_t i = 0; i < SVER_CAPACITY; ++i) {
        if (sver_table[i].obj_id == obj_id) {
            *out_version = sver_table[i].version;
            *out_parent_version = sver_table[i].parent_version;
            *out_generation = sver_table[i].generation;
            return 0;
        }
    }
    return -1;
}

int sver_self_test(void) {
    uint32_t slot;
    if (sver_create(1, &slot) != 0) return -1;

    uint32_t new_version;
    if (sver_bump(1, &new_version) != 0) return -1;
    if (new_version != 2) return -1;

    uint32_t version, parent_version, generation;
    if (sver_query(1, &version, &parent_version, &generation) != 0) return -1;
    if (version != 2 || parent_version != 1) return -1;

    return 0;
}
