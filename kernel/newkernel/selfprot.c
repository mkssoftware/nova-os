/* NovaOS C-Kernel - Nova Self-Protection (Port aus adaptive32.inc). */
#include "selfprot.h"

typedef struct {
    uint32_t source_id; /* 0 = frei */
    uint32_t threat_type;
    uint32_t count;
    uint32_t quarantined;
} selfprot_rec_t;

static selfprot_rec_t selfprot_table[SELFPROT_CAPACITY];
static int selfprot_ready = 0;

void selfprot_initialize(void) {
    if (selfprot_ready == 1) return;

    for (uint32_t i = 0; i < SELFPROT_CAPACITY; ++i) selfprot_table[i] = (selfprot_rec_t){0};
    selfprot_ready = 1;
}

int selfprot_threat(uint32_t source_id, uint32_t threat_type, uint32_t threshold) {
    if (selfprot_ready != 1 || source_id == 0) return -1;

    selfprot_rec_t *rec = 0;
    for (uint32_t i = 0; i < SELFPROT_CAPACITY; ++i) {
        if (selfprot_table[i].source_id == source_id) {
            rec = &selfprot_table[i];
            break;
        }
    }
    if (!rec) {
        for (uint32_t i = 0; i < SELFPROT_CAPACITY; ++i) {
            if (selfprot_table[i].source_id == 0) {
                selfprot_table[i].source_id = source_id;
                selfprot_table[i].threat_type = threat_type;
                selfprot_table[i].count = 0;
                selfprot_table[i].quarantined = 0;
                rec = &selfprot_table[i];
                break;
            }
        }
    }
    if (!rec) return -1;

    if (rec->quarantined == 1) return -1;

    ++rec->count;
    if (rec->count < threshold) return 0;

    rec->quarantined = 1;
    return -1;
}

int selfprot_query(uint32_t source_id, uint32_t *out_count, uint32_t *out_quarantined) {
    if (selfprot_ready != 1) return -1;

    for (uint32_t i = 0; i < SELFPROT_CAPACITY; ++i) {
        if (selfprot_table[i].source_id == source_id) {
            *out_count = selfprot_table[i].count;
            *out_quarantined = selfprot_table[i].quarantined;
            return 0;
        }
    }
    return -1;
}

int selfprot_self_test(void) {
    if (selfprot_threat(1, SELFPROT_OVERFLOW, 2) != 0) return -1; /* 1. Treffer: gezaehlt */
    if (selfprot_threat(1, SELFPROT_OVERFLOW, 2) == 0) return -1; /* 2. Treffer: quarantiert */

    uint32_t count, quarantined;
    if (selfprot_query(1, &count, &quarantined) != 0) return -1;
    if (count != 2 || quarantined != 1) return -1;

    return 0;
}
