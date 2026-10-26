/* NovaOS C-Kernel - Nova Role-Based Access Control (Port aus
   security32.inc). */
#include "rbac.h"
#include "dac.h"

typedef struct {
    uint32_t id;
    uint32_t perms;
    int present;
} rbac_role_t;

typedef struct {
    uint32_t subject;
    uint32_t role_id;
    int present;
} rbac_assignment_t;

static rbac_role_t rbac_roles[RBAC_ROLE_COUNT];
static rbac_assignment_t rbac_assignments[RBAC_ASSIGN_COUNT];
static int rbac_ready = 0;
static uint32_t rbac_denials = 0;

void rbac_initialize(void) {
    for (uint32_t i = 0; i < RBAC_ROLE_COUNT; ++i) rbac_roles[i] = (rbac_role_t){0};
    for (uint32_t i = 0; i < RBAC_ASSIGN_COUNT; ++i) rbac_assignments[i] = (rbac_assignment_t){0};
    rbac_ready = 1;
    rbac_denials = 0;
}

int rbac_define_role(uint32_t role_id, uint32_t permission_mask) {
    for (uint32_t i = 0; i < RBAC_ROLE_COUNT; ++i) {
        if (!rbac_roles[i].present) {
            rbac_roles[i].id = role_id;
            rbac_roles[i].perms = permission_mask;
            rbac_roles[i].present = 1;
            return 0;
        }
    }
    return -1;
}

int rbac_assign_role(uint32_t subject_id, uint32_t role_id) {
    for (uint32_t i = 0; i < RBAC_ASSIGN_COUNT; ++i) {
        if (!rbac_assignments[i].present) {
            rbac_assignments[i].subject = subject_id;
            rbac_assignments[i].role_id = role_id;
            rbac_assignments[i].present = 1;
            return 0;
        }
    }
    return -1;
}

int rbac_check(uint32_t subject_id, uint32_t requested_perm) {
    for (uint32_t i = 0; i < RBAC_ASSIGN_COUNT; ++i) {
        if (!rbac_assignments[i].present || rbac_assignments[i].subject != subject_id) continue;

        uint32_t role_id = rbac_assignments[i].role_id;
        for (uint32_t j = 0; j < RBAC_ROLE_COUNT; ++j) {
            if (rbac_roles[j].present && rbac_roles[j].id == role_id) {
                if ((rbac_roles[j].perms & requested_perm) == requested_perm) {
                    return 0;
                }
                ++rbac_denials;
                return -1;
            }
        }
    }
    ++rbac_denials;
    return -1;
}

int rbac_self_test(void) {
    if (rbac_ready != 1) return -1;

    if (rbac_define_role(1, DAC_PERM_READ | DAC_PERM_WRITE) != 0) return -1;
    if (rbac_define_role(2, DAC_PERM_READ) != 0) return -1;

    if (rbac_assign_role(10, 1) != 0) return -1;
    if (rbac_assign_role(20, 2) != 0) return -1;

    if (rbac_check(10, DAC_PERM_WRITE) != 0) return -1; /* Subject 10 schreibt: ok */
    if (rbac_check(20, DAC_PERM_WRITE) == 0) return -1; /* Subject 20 schreibt: verboten */

    if (rbac_denials != 1) return -1;

    return 0;
}
