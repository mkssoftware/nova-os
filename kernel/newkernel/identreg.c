/* NovaOS C-Kernel - Nova Identity Registry (Port aus adaptive32.inc). */
#include "identreg.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t state;
    uint32_t owner;
} identity_rec_t;

static identity_rec_t identity_table[IDENT_CAPACITY];
static int identity_ready = 0;
static uint32_t identity_count = 0;
static uint32_t identity_next_id = 1;

int identity_register(uint32_t type, uint32_t owner, uint32_t *out_id) {
    if (identity_count >= IDENT_CAPACITY) return -1;

    for (uint32_t i = 0; i < IDENT_CAPACITY; ++i) {
        if (identity_table[i].id == 0) {
            uint32_t id = identity_next_id++;
            identity_table[i].id = id;
            identity_table[i].type = type;
            identity_table[i].state = IDENT_STATE_ACTIVE;
            identity_table[i].owner = owner;
            ++identity_count;
            *out_id = id;
            return 0;
        }
    }
    return -1;
}

int identity_query(uint32_t identity_id, uint32_t *out_type, uint32_t *out_state) {
    for (uint32_t i = 0; i < IDENT_CAPACITY; ++i) {
        if (identity_table[i].id == identity_id) {
            *out_type = identity_table[i].type;
            *out_state = identity_table[i].state;
            return 0;
        }
    }
    return -1;
}

int identity_set_state(uint32_t identity_id, uint32_t new_state) {
    if (new_state > IDENT_STATE_REVOKED) return -1;

    for (uint32_t i = 0; i < IDENT_CAPACITY; ++i) {
        if (identity_table[i].id == identity_id) {
            identity_table[i].state = new_state;
            return 0;
        }
    }
    return -1;
}

void identity_initialize(void) {
    for (uint32_t i = 0; i < IDENT_CAPACITY; ++i) identity_table[i] = (identity_rec_t){0};
    identity_count = 0;
    identity_next_id = 1;

    uint32_t kernel_id;
    if (identity_register(IDENT_TYPE_KERNEL, 0, &kernel_id) != 0) return;

    identity_ready = 1;
}

int identity_self_test(void) {
    uint32_t type, state;
    if (identity_query(1, &type, &state) != 0) return -1;
    if (type != IDENT_TYPE_KERNEL) return -1;
    if (state != IDENT_STATE_ACTIVE) return -1;

    uint32_t proc_id;
    if (identity_register(IDENT_TYPE_PROCESS, 42, &proc_id) != 0) return -1;

    if (identity_query(proc_id, &type, &state) != 0) return -1;
    if (type != IDENT_TYPE_PROCESS) return -1;

    if (identity_set_state(proc_id, IDENT_STATE_REVOKED) != 0) return -1;

    return (identity_ready == 1) ? 0 : -1;
}
