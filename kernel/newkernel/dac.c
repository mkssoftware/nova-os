/* NovaOS C-Kernel - Nova Discretionary Access Control (Port aus
   security32.inc). */
#include "dac.h"

typedef struct {
    uint32_t obj_id;
    uint32_t owner;
    uint32_t owner_perms;
    uint32_t others_perms;
} dac_record_t;

static dac_record_t dac_table[DAC_ENTRY_COUNT];
static int dac_ready = 0;
static uint32_t dac_denials = 0;

void dac_initialize(void) {
    for (uint32_t i = 0; i < DAC_ENTRY_COUNT; ++i) dac_table[i] = (dac_record_t){0};
    dac_ready = 1;
    dac_denials = 0;
}

int dac_register(uint32_t obj_id, uint32_t owner, uint32_t owner_perms, uint32_t others_perms) {
    for (uint32_t i = 0; i < DAC_ENTRY_COUNT; ++i) {
        if (dac_table[i].obj_id == 0) {
            dac_table[i].obj_id = obj_id;
            dac_table[i].owner = owner;
            dac_table[i].owner_perms = owner_perms;
            dac_table[i].others_perms = others_perms;
            return 0;
        }
    }
    return -1;
}

int dac_check(uint32_t obj_id, uint32_t caller_id, uint32_t requested_perms) {
    dac_record_t *rec = 0;
    for (uint32_t i = 0; i < DAC_ENTRY_COUNT; ++i) {
        if (dac_table[i].obj_id == obj_id) { rec = &dac_table[i]; break; }
    }
    if (!rec) { ++dac_denials; return -1; }

    uint32_t perms = (rec->owner == caller_id) ? rec->owner_perms : rec->others_perms;
    if ((perms & requested_perms) != requested_perms) {
        ++dac_denials;
        return -1;
    }
    return 0;
}

int dac_self_test(void) {
    if (dac_ready != 1) return -1;

    if (dac_register(100, 1, DAC_PERM_READ | DAC_PERM_WRITE | DAC_PERM_EXEC, DAC_PERM_READ) != 0) return -1;

    if (dac_check(100, 1, DAC_PERM_READ) != 0) return -1; /* Owner liest */
    if (dac_check(100, 2, DAC_PERM_WRITE) == 0) return -1; /* Anderer schreibt: verboten */

    if (dac_denials != 1) return -1;

    return 0;
}
