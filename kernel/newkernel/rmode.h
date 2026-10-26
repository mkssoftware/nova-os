/* NovaOS C-Kernel - Nova Recovery Mode Manager (Port aus adaptive32.inc,
   NPSPEC-RESILIENCE-RECOVERYMODE-0001). Systemweiter Recovery-Mode:
   NORMAL/DEGRADED/EMERGENCY/MAINTENANCE. Uebergaenge sind geordnet; bei
   Eintritt in EMERGENCY wird ein QoS-Epoch (scheduler.h) und ein
   Temporal-Isolation-Epoch (tmpiso.h) erzwungen. */
#ifndef NOVA_RMODE_H
#define NOVA_RMODE_H

#include <stdint.h>

#define RMODE_NORMAL      0u
#define RMODE_DEGRADED    1u
#define RMODE_EMERGENCY   2u
#define RMODE_MAINTENANCE 3u

void rmode_initialize(void);
int rmode_self_test(void); /* 0 = ok */

/* 0 = Uebergang erlaubt (oder bereits im Zielmodus), -1 = ungueltig
   (target_mode > RMODE_MAINTENANCE, oder EMERGENCY->NORMAL ohne
   Zwischenschritt ueber DEGRADED). */
int rmode_transition(uint32_t target_mode);
uint32_t rmode_query(void);

#endif
