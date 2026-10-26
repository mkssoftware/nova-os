/* NovaOS C-Kernel - Thermal Manager (Port aus thermal32.inc,
   NPSPEC-THERMAL-ARCH-0001 und verwandte NPSPEC-THERMAL-Spezifikationen). */
#ifndef NOVA_THERMAL_H
#define NOVA_THERMAL_H

#include <stdint.h>

/* THERM_STATE_NORMAL=0 und THERM_STATE_HOT=2 kommen (wie im alten ASM) aus
   adaptive32.inc - hier bewusst als Platzhalter uebernommen, bis dieses
   Subsystem portiert ist. */
#define THERM_STATE_NORMAL    0u
#define THERM_STATE_ELEVATED  1u
#define THERM_STATE_HOT       2u
#define THERM_STATE_CRITICAL  3u
#define THERM_STATE_EMERGENCY 4u
#define THERM_STATE_UNKNOWN   5u

#define THERM_ZONE_CPU      0u
#define THERM_ZONE_GPU      1u
#define THERM_ZONE_MEMORY   2u
#define THERM_ZONE_STORAGE  3u
#define THERM_ZONE_BATTERY  4u
#define THERM_ZONE_VRM      5u
#define THERM_ZONE_SOC      6u
#define THERM_ZONE_PLATFORM 7u

#define THERM_SENSOR_AVAILABLE   0u
#define THERM_SENSOR_DEGRADED    1u
#define THERM_SENSOR_STALE       2u
#define THERM_SENSOR_INVALID     3u
#define THERM_SENSOR_UNAVAILABLE 4u
#define THERM_SENSOR_FAILED      5u

#define THERM_SRC_CPU      0u
#define THERM_SRC_GPU      1u
#define THERM_SRC_MEMORY   2u
#define THERM_SRC_STORAGE  3u
#define THERM_SRC_BOARD    4u
#define THERM_SRC_FIRMWARE 5u
#define THERM_SRC_VIRTUAL  6u

#define COOL_ACTION_NONE     0x00u
#define COOL_ACTION_FAN      0x01u
#define COOL_ACTION_FREQ     0x02u
#define COOL_ACTION_POWER    0x04u
#define COOL_ACTION_MIGRATE  0x08u
#define COOL_ACTION_SHUTDOWN 0x10u

#define THERM_SENSOR_CAPACITY 8u
#define THERM_ZONE_CAPACITY   4u

#define THERM_DEFAULT_ELEVATED  70000
#define THERM_DEFAULT_HOT       85000
#define THERM_DEFAULT_CRITICAL  95000
#define THERM_DEFAULT_EMERGENCY 105000

/* Staleness-Schwelle: Sensor gilt nach 2s ohne Update als Stale (bei 100Hz IRQ). */
#define THERM_STALE_TICKS 200u

#define FAN_PWM_MIN  0u
#define FAN_PWM_MAX  255u
#define FAN_PWM_AUTO 0xFFu

/* Registriert die CPU-Zone (0) und liest, falls verfuegbar, die MSR-Temperatur. */
void thermal_manager_initialize(void);
int thermal_manager_self_test(void); /* 0 = ok */

/* Aktualisiert Sensoren/Zonen/Cooling. Sollte periodisch aus dem Timer-IRQ
   aufgerufen werden. Immer 0. */
int thermal_manager_update(void);

/* UNKNOWN wenn zone_id unbekannt. */
uint32_t thermal_get_zone_state(uint32_t zone_id);

/* 0 = Erfolg, -1 wenn zone_id unbekannt (out_budget_pct unveraendert). */
int thermal_get_headroom(uint32_t zone_id, uint32_t *out_budget_pct);

#endif
