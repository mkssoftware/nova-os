/* NovaOS C-Kernel - Nova Adaptive Network Manager (Port aus
   adaptive32.inc). */
#include "anetwork.h"

typedef struct {
    uint32_t iface_id; /* 0 = frei */
    uint32_t bw_used;
    uint32_t bw_cap;
    uint32_t qos;
} anetwork_rec_t;

static anetwork_rec_t anetwork_table[ANET_CAPACITY];
static int anetwork_ready = 0;

void anetwork_initialize(void) {
    if (anetwork_ready == 1) return;

    for (uint32_t i = 0; i < ANET_CAPACITY; ++i) anetwork_table[i] = (anetwork_rec_t){0};
    anetwork_ready = 1;
}

int anetwork_register(uint32_t iface_id, uint32_t bw_cap) {
    if (anetwork_ready != 1 || iface_id == 0) return -1;

    for (uint32_t i = 0; i < ANET_CAPACITY; ++i) {
        if (anetwork_table[i].iface_id == 0) {
            anetwork_table[i].iface_id = iface_id;
            anetwork_table[i].bw_used = 0;
            anetwork_table[i].bw_cap = bw_cap;
            anetwork_table[i].qos = ANET_BEST_EFFORT;
            return 0;
        }
    }
    return -1;
}

int anetwork_update_bw(uint32_t iface_id, uint32_t bw_sample) {
    if (anetwork_ready != 1) return -1;

    for (uint32_t i = 0; i < ANET_CAPACITY; ++i) {
        anetwork_rec_t *rec = &anetwork_table[i];
        if (rec->iface_id != iface_id) continue;

        rec->bw_used = (ANET_ALPHA * bw_sample + (256u - ANET_ALPHA) * rec->bw_used) >> 8;

        uint32_t utilization_pct = (rec->bw_cap == 0) ? 0 : (rec->bw_used * 100u) / rec->bw_cap;

        if (utilization_pct >= 80u) {
            rec->qos = ANET_REALTIME;
        } else if (utilization_pct >= 50u) {
            rec->qos = ANET_INTERACTIVE;
        } else if (utilization_pct >= 20u) {
            rec->qos = ANET_BULK;
        } else {
            rec->qos = ANET_BEST_EFFORT;
        }
        return 0;
    }
    return -1;
}

int anetwork_query(uint32_t iface_id, uint32_t *out_qos, uint32_t *out_bw_used,
                    uint32_t *out_bw_cap) {
    if (anetwork_ready != 1) return -1;

    for (uint32_t i = 0; i < ANET_CAPACITY; ++i) {
        if (anetwork_table[i].iface_id == iface_id) {
            *out_qos = anetwork_table[i].qos;
            *out_bw_used = anetwork_table[i].bw_used;
            *out_bw_cap = anetwork_table[i].bw_cap;
            return 0;
        }
    }
    return -1;
}

int anetwork_self_test(void) {
    if (anetwork_register(1, 100) != 0) return -1;

    if (anetwork_update_bw(1, 512) != 0) return -1; /* EWMA(48/256) von 0 -> ~96, 96% von 100 -> REALTIME */

    uint32_t qos, bw_used, bw_cap;
    if (anetwork_query(1, &qos, &bw_used, &bw_cap) != 0) return -1;
    if (qos != ANET_REALTIME) return -1;

    return 0;
}
