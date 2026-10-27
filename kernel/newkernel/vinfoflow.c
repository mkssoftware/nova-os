/* NovaOS C-Kernel - Nova Information Flow Verifier (Port aus
   adaptive32.inc). */
#include "vinfoflow.h"
#include "audit.h"

typedef struct {
    uint32_t chan_id; /* 0 = frei */
    uint32_t label;
    uint32_t owner;
    uint32_t violations;
} vinfoflow_rec_t;

static vinfoflow_rec_t vinfoflow_table[VINFO_CAPACITY];
static int vinfoflow_ready = 0;

void vinfoflow_initialize(void) {
    if (vinfoflow_ready == 1) return;

    for (uint32_t i = 0; i < VINFO_CAPACITY; ++i) vinfoflow_table[i] = (vinfoflow_rec_t){0};
    vinfoflow_ready = 1;
}

int vinfoflow_register(uint32_t chan_id, uint32_t label, uint32_t owner) {
    if (vinfoflow_ready != 1 || chan_id == 0) return -1;

    for (uint32_t i = 0; i < VINFO_CAPACITY; ++i) {
        if (vinfoflow_table[i].chan_id == 0) {
            vinfoflow_table[i].chan_id = chan_id;
            vinfoflow_table[i].label = label;
            vinfoflow_table[i].owner = owner;
            vinfoflow_table[i].violations = 0;
            return 0;
        }
    }
    return -1;
}

int vinfoflow_check(uint32_t src_chan_id, uint32_t dst_chan_id) {
    if (vinfoflow_ready != 1) return -1;

    uint32_t src_label = 0, dst_label = 0;
    int dst_index = -1;

    for (uint32_t i = 0; i < VINFO_CAPACITY; ++i) {
        if (vinfoflow_table[i].chan_id == src_chan_id) src_label = vinfoflow_table[i].label;
        if (vinfoflow_table[i].chan_id == dst_chan_id) {
            dst_label = vinfoflow_table[i].label;
            dst_index = (int)i;
        }
    }

    if (src_label <= dst_label) return 0;

    if (dst_index >= 0) ++vinfoflow_table[dst_index].violations;
    security_audit_log(AUDIT_TYPE_VIOLATION, src_chan_id, dst_chan_id, 0);
    return -1;
}

int vinfoflow_self_test(void) {
    if (vinfoflow_register(1, 5, 1) != 0) return -1;
    if (vinfoflow_register(2, 1, 1) != 0) return -1;

    if (vinfoflow_check(2, 1) != 0) return -1;
    if (vinfoflow_check(1, 2) == 0) return -1;

    return 0;
}
