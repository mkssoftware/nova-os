/* NovaOS C-Kernel - Nova State Snapshot (Port aus adaptive32.inc). */
#include "ssnap.h"

typedef struct {
    uint32_t id; /* 0 = frei */
    uint32_t *address;
    uint32_t value;
    uint32_t version;
} ssnap_rec_t;

static ssnap_rec_t ssnap_table[SSNAP_CAPACITY];
static int ssnap_ready = 0;
static uint32_t ssnap_next_id = 1;

void ssnap_initialize(void) {
    if (ssnap_ready == 1) return;

    for (uint32_t i = 0; i < SSNAP_CAPACITY; ++i) ssnap_table[i] = (ssnap_rec_t){0};
    ssnap_next_id = 1;
    ssnap_ready = 1;
}

int ssnap_capture(uint32_t *address, uint32_t *out_snap_id) {
    if (ssnap_ready != 1 || address == 0) return -1;

    for (uint32_t i = 0; i < SSNAP_CAPACITY; ++i) {
        if (ssnap_table[i].id == 0) {
            ssnap_table[i].id = ssnap_next_id++;
            ssnap_table[i].address = address;
            ssnap_table[i].value = *address;
            ssnap_table[i].version = 1;
            *out_snap_id = ssnap_table[i].id;
            return 0;
        }
    }
    return -1;
}

int ssnap_restore(uint32_t snap_id) {
    if (ssnap_ready != 1 || snap_id == 0) return -1;

    for (uint32_t i = 0; i < SSNAP_CAPACITY; ++i) {
        if (ssnap_table[i].id == snap_id) {
            *ssnap_table[i].address = ssnap_table[i].value;
            ssnap_table[i].id = 0;
            return 0;
        }
    }
    return -1;
}

int ssnap_self_test(void) {
    static uint32_t ssnap_test_var = 0x1234;

    uint32_t snap_id;
    if (ssnap_capture(&ssnap_test_var, &snap_id) != 0) return -1;

    ssnap_test_var = 0xDEAD;
    if (ssnap_restore(snap_id) != 0) return -1;
    if (ssnap_test_var != 0x1234) return -1;

    return 0;
}
