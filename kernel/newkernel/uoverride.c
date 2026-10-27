/* NovaOS C-Kernel - Nova User Override Manager (Port aus
   adaptive32.inc). */
#include "uoverride.h"
#include "sconf.h"

typedef struct {
    uint32_t key; /* 0 = frei */
    uint32_t value;
    uint32_t active;
    uint32_t version;
} uoverride_rec_t;

static uoverride_rec_t uoverride_table[UOVER_CAPACITY];
static int uoverride_ready = 0;

void uoverride_initialize(void) {
    if (uoverride_ready == 1) return;

    for (uint32_t i = 0; i < UOVER_CAPACITY; ++i) uoverride_table[i] = (uoverride_rec_t){0};
    uoverride_ready = 1;
}

int uoverride_set(uint32_t key, uint32_t value) {
    if (uoverride_ready != 1 || key == 0) return -1;

    uoverride_rec_t *rec = 0;
    for (uint32_t i = 0; i < UOVER_CAPACITY; ++i) {
        if (uoverride_table[i].key == key) {
            rec = &uoverride_table[i];
            break;
        }
    }

    if (rec) {
        ++rec->version;
        rec->value = value;
        rec->active = 1;
    } else {
        for (uint32_t i = 0; i < UOVER_CAPACITY; ++i) {
            if (uoverride_table[i].key == 0) {
                uoverride_table[i].key = key;
                uoverride_table[i].value = value;
                uoverride_table[i].active = 1;
                uoverride_table[i].version = 1;
                rec = &uoverride_table[i];
                break;
            }
        }
    }
    if (!rec) return -1;

    sconf_set(key, value); /* Rueckgabe ignoriert, 1:1 wie im alten ASM */
    return 0;
}

int uoverride_get(uint32_t key, uint32_t *out_value, uint32_t *out_version) {
    if (uoverride_ready != 1) return -1;

    for (uint32_t i = 0; i < UOVER_CAPACITY; ++i) {
        uoverride_rec_t *rec = &uoverride_table[i];
        if (rec->key != key) continue;
        if (rec->active != 1) return -1;

        *out_value = rec->value;
        *out_version = rec->version;
        return 0;
    }
    return -1;
}

int uoverride_clear(uint32_t key) {
    if (uoverride_ready != 1) return -1;

    for (uint32_t i = 0; i < UOVER_CAPACITY; ++i) {
        if (uoverride_table[i].key == key) {
            uoverride_table[i].active = 0;
            return 0;
        }
    }
    return -1;
}

int uoverride_self_test(void) {
    if (uoverride_set(42, 99) != 0) return -1;

    uint32_t value, version;
    if (uoverride_get(42, &value, &version) != 0) return -1;
    if (value != 99) return -1;

    if (uoverride_clear(42) != 0) return -1;
    if (uoverride_get(42, &value, &version) == 0) return -1;

    return 0;
}
