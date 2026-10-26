/* NovaOS C-Kernel - Nova Generic Finite State Machine (Port aus
   adaptive32.inc, NPSPEC-STATE-MACHINE-0001). Registriert FSMs mit
   erlaubten Uebergaengen (Transition Table). Max. 4 FSMs, je 8 erlaubte
   Transitionen. */
#ifndef NOVA_FSM_H
#define NOVA_FSM_H

#include <stdint.h>

#define FSM_CAPACITY    4u
#define FSM_TRANS_COUNT 8u

void fsm_initialize(void);
int fsm_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int fsm_register(uint32_t fsm_id, uint32_t initial_state);

/* 0 = Erfolg, -1 wenn fsm_id unbekannt oder Transition-Tabelle voll. */
int fsm_add_transition(uint32_t fsm_id, uint32_t from_state, uint32_t to_state);

/* 0 = Uebergang erlaubt und durchgefuehrt, -1 = fsm_id unbekannt oder
   Uebergang (current -> to_state) nicht in der Tabelle. */
int fsm_transition2(uint32_t fsm_id, uint32_t to_state);

#endif
