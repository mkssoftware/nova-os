/* NovaOS C-Kernel - Semantic-Layer (Port aus semantic32.inc). */
#include "semantic.h"
#include <stddef.h>

typedef struct {
    uint32_t id;
    uint32_t name_hash;
    uint32_t type;
    uint32_t flags;
    uint32_t metadata_ptr;
    semmgr_validate_fn_t validate_fn;
    uint32_t relationship;
    uint32_t related_id;
} semmgr_record_t;

static semmgr_record_t semmgr_table[SEM_CAPACITY];
static uint32_t semmgr_count = 0;
static uint32_t semmgr_next_id = 1;
static int semmgr_ready = 0;

void semmgr_initialize(void) {
    for (uint32_t i = 0; i < SEM_CAPACITY; ++i) {
        semmgr_table[i] = (semmgr_record_t){0};
    }
    semmgr_count = 0;
    semmgr_next_id = 1;
    semmgr_ready = 1;
}

int semmgr_register(uint32_t name_hash, uint32_t type, uint32_t flags, uint32_t *out_sem_id) {
    if (semmgr_count >= SEM_CAPACITY) {
        return -1;
    }
    if (type >= STYPE_COUNT) {
        return -1;
    }
    semmgr_record_t *rec = &semmgr_table[semmgr_count];
    rec->id = semmgr_next_id;
    rec->name_hash = name_hash;
    rec->type = type;
    rec->flags = flags;
    rec->metadata_ptr = 0;
    rec->validate_fn = 0;
    rec->relationship = SREL_NONE;
    rec->related_id = 0;

    if (out_sem_id) {
        *out_sem_id = semmgr_next_id;
    }
    ++semmgr_next_id;
    ++semmgr_count;
    return 0;
}

void *semmgr_query(uint32_t name_hash) {
    for (uint32_t i = 0; i < semmgr_count; ++i) {
        if (semmgr_table[i].name_hash == name_hash) {
            return &semmgr_table[i];
        }
    }
    return 0;
}

int semmgr_validate(uint32_t sem_id, uint32_t value, uint32_t *out_result) {
    for (uint32_t i = 0; i < semmgr_count; ++i) {
        if (semmgr_table[i].id == sem_id) {
            semmgr_record_t *rec = &semmgr_table[i];
            *out_result = rec->validate_fn ? rec->validate_fn(value) : SVAL_OK;
            return 0;
        }
    }
    *out_result = SVAL_TYPE_MISMATCH;
    return -1;
}

int semmgr_self_test(void) {
    return (semmgr_ready == 1) ? 0 : -1;
}
