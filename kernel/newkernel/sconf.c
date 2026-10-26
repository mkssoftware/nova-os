/* NovaOS C-Kernel - Nova Self-Configuration (Port aus adaptive32.inc). */
#include "sconf.h"

typedef struct {
    uint32_t key; /* 0 = frei */
    uint32_t value;
    uint32_t version;
} sconf_rec_t;

static sconf_rec_t sconf_table[SCONF_CAPACITY];
static int sconf_ready = 0;

void sconf_initialize(void) {
    if (sconf_ready == 1) return;

    for (uint32_t i = 0; i < SCONF_CAPACITY; ++i) sconf_table[i] = (sconf_rec_t){0};
    sconf_ready = 1;
}

int sconf_set(uint32_t key, uint32_t value) {
    if (sconf_ready != 1 || key == 0) return -1;

    for (uint32_t i = 0; i < SCONF_CAPACITY; ++i) {
        if (sconf_table[i].key == key) {
            sconf_table[i].value = value;
            ++sconf_table[i].version;
            return 0;
        }
    }

    for (uint32_t i = 0; i < SCONF_CAPACITY; ++i) {
        if (sconf_table[i].key == 0) {
            sconf_table[i].key = key;
            sconf_table[i].value = value;
            sconf_table[i].version = 1;
            return 0;
        }
    }
    return -1;
}

int sconf_get(uint32_t key, uint32_t *out_value, uint32_t *out_version) {
    if (sconf_ready != 1) return -1;

    for (uint32_t i = 0; i < SCONF_CAPACITY; ++i) {
        if (sconf_table[i].key == key) {
            *out_value = sconf_table[i].value;
            *out_version = sconf_table[i].version;
            return 0;
        }
    }
    return -1;
}

int sconf_self_test(void) {
    if (sconf_set(1, 42) != 0) return -1;

    uint32_t value, version;
    if (sconf_get(1, &value, &version) != 0) return -1;
    if (value != 42 || version != 1) return -1;

    if (sconf_set(1, 100) != 0) return -1;
    if (sconf_get(1, &value, &version) != 0) return -1;
    if (value != 100 || version != 2) return -1;

    if (sconf_get(99, &value, &version) == 0) return -1;

    return 0;
}
