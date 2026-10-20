/* NovaOS C-Kernel - Autonomy-Subsystem (Port aus autonomy32.inc, NPSPEC-AUTONOMY-*). */
#ifndef NOVA_AUTONOMY_H
#define NOVA_AUTONOMY_H

#include <stdint.h>

#define AUTON_POLICY_MANUAL 0u
#define AUTON_POLICY_ASSIST 1u
#define AUTON_POLICY_AUTO   2u

#define AUTON_HEAL_RESTART     0u
#define AUTON_HEAL_RECONFIGURE 1u
#define AUTON_HEAL_FAILOVER    2u
#define AUTON_HEAL_ISOLATE     3u

#define AUTON_DIAG_OK       0u
#define AUTON_DIAG_WARN     1u
#define AUTON_DIAG_FAULT    2u
#define AUTON_DIAG_CRITICAL 3u

#define AUTON_GOAL_CAPACITY 8u

/* Initialisiert und registriert das Standard-Ziel (CPU-Auslastung <= 80%). */
void autonomy_initialize(void);
int autonomy_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Ziel-Tabelle voll. */
int auton_register_goal(uint32_t id, uint32_t metric_id, uint32_t target_value);

/* Aktualisiert alle Ziele mit passender metric_id und fuehrt Diagnose/ggf.
   Anpassung durch (immer 0 = Erfolg, identisch zum alten ASM). */
void auton_update_metric(uint32_t metric_id, uint32_t current_value);

/* 0 = Erfolg (heilt nur bei AUTON_DIAG_CRITICAL), -1 wenn goal_id unbekannt. */
int auton_self_heal(uint32_t goal_id);

#endif
