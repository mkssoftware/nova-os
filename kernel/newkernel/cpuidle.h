/* NovaOS C-Kernel - Nova CPU Idle State Management (Port aus
   adaptive32.inc, NPSPEC-POWER-CPUIDLE-0001). C-State-Tracking mit
   Residency-Messung. NICHT zu verwechseln mit power.c's CPUIDLE_C0..C7
   (power32.inc) - ein eigenstaendiges, schon im alten ASM paralleles
   Subsystem mit eigenem, einfacherem Zustandsmodell (nur C0/C1/C1E/C3). */
#ifndef NOVA_CPUIDLE_H
#define NOVA_CPUIDLE_H

#include <stdint.h>

#define CPUIDLE_STATE_C0  0u /* active */
#define CPUIDLE_STATE_C1  1u /* halt (HLT) */
#define CPUIDLE_STATE_C1E 2u /* enhanced halt */
#define CPUIDLE_STATE_C3  3u /* deep sleep */

void cpuidle_initialize(void);
int cpuidle_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn state > CPUIDLE_STATE_C3. Fuehrt bei state != C0 ein
   echtes HLT aus. */
int cpuidle_enter(uint32_t state);
void cpuidle_exit(void); /* misst Residency, setzt Zustand auf C0 zurueck */

/* 0 = Erfolg (Residency und Enter-Count gesetzt), -1 wenn
   state > CPUIDLE_STATE_C3. */
int cpuidle_query(uint32_t state, uint32_t *out_residency_ticks, uint32_t *out_enter_count);

#endif
