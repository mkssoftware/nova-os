/* NovaOS C-Kernel - Autonomy-Subsystem (Port aus autonomy32.inc). */
#include "autonomy.h"

typedef struct {
    uint32_t id;
    uint32_t metric_id;
    uint32_t target_value;
    uint32_t current_value;
    uint32_t policy;
    uint32_t heal_action;
    uint32_t diag_state;
    uint32_t adapt_count;
} auton_goal_t;

static auton_goal_t auton_goals[AUTON_GOAL_CAPACITY];
static uint32_t auton_goal_count = 0;
static uint32_t auton_policy = AUTON_POLICY_ASSIST;
static int auton_ready = 0;

int auton_register_goal(uint32_t id, uint32_t metric_id, uint32_t target_value) {
    if (auton_goal_count >= AUTON_GOAL_CAPACITY) {
        return -1;
    }
    auton_goal_t *goal = &auton_goals[auton_goal_count];
    goal->id = id;
    goal->metric_id = metric_id;
    goal->target_value = target_value;
    goal->current_value = 0;
    goal->policy = auton_policy;
    goal->heal_action = AUTON_HEAL_RECONFIGURE;
    goal->diag_state = AUTON_DIAG_OK;
    goal->adapt_count = 0;
    ++auton_goal_count;
    return 0;
}

void autonomy_initialize(void) {
    for (uint32_t i = 0; i < AUTON_GOAL_CAPACITY; ++i) {
        auton_goals[i] = (auton_goal_t){0};
    }
    auton_goal_count = 0;
    auton_policy = AUTON_POLICY_ASSIST;

    /* Standard-Ziel: CPU-Auslastung <= 80% */
    auton_register_goal(0, 0 /* metric: CPU */, 80);

    auton_ready = 1;
}

void auton_update_metric(uint32_t metric_id, uint32_t current_value) {
    for (uint32_t i = 0; i < auton_goal_count; ++i) {
        auton_goal_t *goal = &auton_goals[i];
        if (goal->metric_id != metric_id) {
            continue;
        }
        goal->current_value = current_value;

        if (current_value <= goal->target_value) {
            goal->diag_state = AUTON_DIAG_OK;
            continue;
        }

        if (current_value >= goal->target_value * 2) {
            goal->diag_state = AUTON_DIAG_CRITICAL;
            continue;
        }

        goal->diag_state = AUTON_DIAG_WARN;
        if (goal->policy == AUTON_POLICY_AUTO) {
            ++goal->adapt_count;
        }
    }
}

int auton_self_heal(uint32_t goal_id) {
    for (uint32_t i = 0; i < auton_goal_count; ++i) {
        auton_goal_t *goal = &auton_goals[i];
        if (goal->id != goal_id) {
            continue;
        }
        if (goal->diag_state == AUTON_DIAG_CRITICAL) {
            /* Heilaktion ausfuehren (Stub, NPSPEC-AUTONOMY-HEALING-0001) */
            goal->diag_state = AUTON_DIAG_WARN;
            ++goal->adapt_count;
        }
        return 0;
    }
    return -1;
}

int autonomy_self_test(void) {
    if (auton_ready != 1) return -1;

    auton_update_metric(0, 50);  /* 50% OK */
    auton_update_metric(0, 200); /* 200% kritisch */
    if (auton_self_heal(0) != 0) return -1;

    return 0;
}
