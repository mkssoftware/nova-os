/* NovaOS C-Kernel - Nova Thermal Throttle Policy (Port aus adaptive32.inc,
   NPSPEC-THERMAL-THROTTLING-0001). Thermische Zonen mit simulierter
   Temperatur (Bootstrap: kein echter Sensor) und QoS-Drosselung bei HOT.
   NICHT zu verwechseln mit thermal.c (thermal32.inc: echte MSR-Sensoren,
   Sensor/Zone-Pools, Headroom-Berechnung) - ein eigenstaendiges, schon im
   alten ASM paralleles Subsystem mit eigenem, einfacherem Zustandsmodell. */
#ifndef NOVA_THERMTHROTTLE_H
#define NOVA_THERMTHROTTLE_H

#include <stdint.h>

#define THERM_ZONE_COUNT 4u

#define THERM_STATE_NORMAL 0u
#define THERM_STATE_WARM   1u
#define THERM_STATE_HOT    2u
#define THERM_STATE_CRIT   3u

void thermal_initialize(void);
int thermal_self_test(void); /* 0 = ok */

/* 0 = kein Throttle noetig, -1 bei Fehler ODER wenn HOT erreicht wurde (CF=1
   im alten ASM bedeutete "Throttle-Aktion ausgefuehrt", kein echter Fehler -
   siehe Rueckgabewert-Hinweis unten). */
int thermal_update(uint32_t zone_id, uint32_t temperature, int *out_throttled);

/* 0 = Erfolg (Zustand und Temperatur gesetzt), -1 wenn zone_id ungueltig. */
int thermal_query(uint32_t zone_id, uint32_t *out_state, uint32_t *out_temperature);

#endif
