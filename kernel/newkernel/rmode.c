/* NovaOS C-Kernel - Nova Recovery Mode Manager (Port aus adaptive32.inc). */
#include "rmode.h"
#include "scheduler.h"
#include "tmpiso.h"

static uint32_t rmode_current = RMODE_NORMAL;
static uint32_t rmode_transitions = 0;
static int rmode_ready = 0;

void rmode_initialize(void) {
    rmode_current = RMODE_NORMAL;
    rmode_transitions = 0;
    rmode_ready = 1;
}

int rmode_transition(uint32_t target_mode) {
    if (target_mode > RMODE_MAINTENANCE) return -1;
    if (rmode_current == target_mode) return 0;

    if (rmode_current == RMODE_EMERGENCY && target_mode == RMODE_NORMAL) {
        return -1; /* muss zuerst ueber DEGRADED */
    }

    rmode_current = target_mode;
    ++rmode_transitions;

    if (target_mode == RMODE_EMERGENCY) {
        qos_new_epoch();
        tmpiso_new_epoch();
    }

    return 0;
}

uint32_t rmode_query(void) {
    return rmode_current;
}

int rmode_self_test(void) {
    (void)rmode_ready;

    if (rmode_transition(RMODE_DEGRADED) != 0) return -1;
    if (rmode_transition(RMODE_EMERGENCY) != 0) return -1;
    if (rmode_transition(RMODE_NORMAL) == 0) return -1; /* muss fehlschlagen */

    if (rmode_transition(RMODE_DEGRADED) != 0) return -1;
    if (rmode_transition(RMODE_NORMAL) != 0) return -1;
    if (rmode_query() != RMODE_NORMAL) return -1;

    return 0;
}
