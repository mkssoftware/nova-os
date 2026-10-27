/* NovaOS C-Kernel - Nova State Global Store (Port aus adaptive32.inc). */
#include "sglobal.h"
#include "shist.h"

typedef struct {
    uint32_t key; /* 0 = frei */
    uint32_t value;
    uint32_t version;
    uint32_t owner;
} sglobal_rec_t;

static sglobal_rec_t sglobal_table[SGLOBAL_CAPACITY];
static int sglobal_ready = 0;

void sglobal_initialize(void) {
    if (sglobal_ready == 1) return;

    for (uint32_t i = 0; i < SGLOBAL_CAPACITY; ++i) sglobal_table[i] = (sglobal_rec_t){0};
    sglobal_ready = 1;
}

int sglobal_publish(uint32_t key, uint32_t value, uint32_t owner) {
    if (sglobal_ready != 1 || key == 0) return -1;

    for (uint32_t i = 0; i < SGLOBAL_CAPACITY; ++i) {
        sglobal_rec_t *rec = &sglobal_table[i];
        if (rec->key == key) {
            uint32_t old_value = rec->value;
            ++rec->version;
            rec->value = value;
            shist_record(key, old_value, rec->value);
            return 0;
        }
        if (rec->key == 0) {
            rec->key = key;
            rec->value = value;
            rec->owner = owner;
            rec->version = 1;
            shist_record(key, 0, rec->value);
            return 0;
        }
    }
    return -1;
}

int sglobal_get(uint32_t key, uint32_t *out_value, uint32_t *out_version, uint32_t *out_owner) {
    if (sglobal_ready != 1) return -1;

    for (uint32_t i = 0; i < SGLOBAL_CAPACITY; ++i) {
        if (sglobal_table[i].key == key) {
            *out_value = sglobal_table[i].value;
            *out_version = sglobal_table[i].version;
            *out_owner = sglobal_table[i].owner;
            return 0;
        }
    }
    return -1;
}

int sglobal_self_test(void) {
    if (sglobal_publish(7, 42, 1) != 0) return -1;

    uint32_t value, version, owner;
    if (sglobal_get(7, &value, &version, &owner) != 0) return -1;
    if (value != 42) return -1;

    if (sglobal_publish(7, 99, 1) != 0) return -1;
    if (sglobal_get(7, &value, &version, &owner) != 0) return -1;
    if (value != 99) return -1;

    return 0;
}
