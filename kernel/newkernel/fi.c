/* NovaOS C-Kernel - Nova Fault Injection Framework (Port aus
   adaptive32.inc). */
#include "fi.h"

typedef struct {
    uint32_t target;
    uint32_t fault_type;
    uint32_t active;
} fi_rec_t;

static fi_rec_t fi_table[FI_CAPACITY];
static int fi_ready = 0;
static uint32_t fi_total_injected = 0;

void fi_initialize(void) {
    for (uint32_t i = 0; i < FI_CAPACITY; ++i) fi_table[i] = (fi_rec_t){0};
    fi_total_injected = 0;
    fi_ready = 1;
}

int fi_inject(uint32_t target_subsystem, uint32_t fault_type) {
    if (fi_ready != 1) return -1;

    for (uint32_t i = 0; i < FI_CAPACITY; ++i) {
        if (fi_table[i].active == 0) {
            fi_table[i].target = target_subsystem;
            fi_table[i].fault_type = fault_type;
            fi_table[i].active = 1;
            ++fi_total_injected;
            return 0;
        }
    }
    return -1;
}

int fi_check(uint32_t target_subsystem, uint32_t *out_fault_type) {
    for (uint32_t i = 0; i < FI_CAPACITY; ++i) {
        if (fi_table[i].active != 0 && fi_table[i].target == target_subsystem) {
            *out_fault_type = fi_table[i].fault_type;
            return 0;
        }
    }
    return -1;
}

void fi_clear(uint32_t target_subsystem) {
    for (uint32_t i = 0; i < FI_CAPACITY; ++i) {
        if (fi_table[i].active != 0 && fi_table[i].target == target_subsystem) {
            fi_table[i].active = 0;
        }
    }
}

int fi_self_test(void) {
    if (fi_inject(7, 3) != 0) return -1;

    uint32_t fault_type;
    if (fi_check(7, &fault_type) != 0) return -1;
    if (fault_type != 3) return -1;

    fi_clear(7);
    if (fi_check(7, &fault_type) == 0) return -1; /* kein Fehler mehr */

    return 0;
}
