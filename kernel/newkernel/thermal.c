/* NovaOS C-Kernel - Thermal Manager (Port aus thermal32.inc). */
#include "thermal.h"
#include "serial.h"
#include "powermgr.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t zone;
    int32_t value_mc;
    uint32_t timestamp;
    uint32_t state;
    uint32_t virtual_parent;
} thermal_sensor_t;

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t state;
    int32_t trend; /* +1 steigend, 0 stabil, -1 fallend */
    int32_t temp_mc;
    int32_t limit_elevated;
    int32_t limit_hot;
    int32_t limit_critical;
    int32_t limit_emergency;
    uint32_t cooling_actions;
    uint32_t sensor_mask;
} thermal_zone_t;

typedef struct {
    uint32_t zone;
    uint32_t budget_pct;
    uint32_t throttle_pct;
} thermal_headroom_t;

static thermal_sensor_t therm_sensors[THERM_SENSOR_CAPACITY];
static thermal_zone_t therm_zones[THERM_ZONE_CAPACITY];
static thermal_headroom_t therm_headrooms[THERM_ZONE_CAPACITY];
static uint32_t therm_sensor_count = 0;
static uint32_t therm_zone_count = 0;
static int therm_mgr_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

static void therm_create_cpu_zone(void) {
    thermal_zone_t *zone = &therm_zones[0];
    zone->id = 0;
    zone->type = THERM_ZONE_CPU;
    zone->state = THERM_STATE_UNKNOWN;
    zone->trend = 0;
    zone->temp_mc = 0;
    zone->limit_elevated = THERM_DEFAULT_ELEVATED;
    zone->limit_hot = THERM_DEFAULT_HOT;
    zone->limit_critical = THERM_DEFAULT_CRITICAL;
    zone->limit_emergency = THERM_DEFAULT_EMERGENCY;
    zone->cooling_actions = COOL_ACTION_NONE;
    zone->sensor_mask = 0x01u; /* Sensor 0 */
    ++therm_zone_count;

    thermal_headroom_t *headroom = &therm_headrooms[0];
    headroom->zone = 0;
    headroom->budget_pct = 100;
    headroom->throttle_pct = 0;
}

/* Liest IA32_THERM_STATUS (MSR 0x19C) fuer die Package-Temperatur, Ergebnis
   in Sensor 0. */
static void therm_read_msr_sensors(void) {
    thermal_sensor_t *sensor = &therm_sensors[0];
    sensor->id = 0;
    sensor->type = THERM_SRC_CPU;
    sensor->zone = 0;
    sensor->state = THERM_SENSOR_UNAVAILABLE;
    sensor->virtual_parent = 0;

    uint32_t eax, edx;
    __asm__ volatile ("rdmsr" : "=a"(eax), "=d"(edx) : "c"(0x19Cu));

    if ((eax & 0x80000000u) == 0) {
        return; /* Reading nicht gueltig */
    }
    uint32_t digital_readout = (eax >> 16) & 0x7Fu; /* Abstand von Tjmax */
    int32_t temp_mc = 100000 - (int32_t)(digital_readout * 1000u); /* Tjmax ~100C */
    if (temp_mc < 0) {
        return; /* ungueltiger Wert */
    }
    sensor->value_mc = temp_mc;
    sensor->timestamp = current_timer_ticks();
    sensor->state = THERM_SENSOR_AVAILABLE;
    ++therm_sensor_count;
}

void thermal_manager_initialize(void) {
    for (uint32_t i = 0; i < THERM_SENSOR_CAPACITY; ++i) {
        therm_sensors[i] = (thermal_sensor_t){0};
    }
    for (uint32_t i = 0; i < THERM_ZONE_CAPACITY; ++i) {
        therm_zones[i] = (thermal_zone_t){0};
        therm_headrooms[i] = (thermal_headroom_t){0};
    }
    therm_sensor_count = 0;
    therm_zone_count = 0;

    therm_create_cpu_zone();
    therm_read_msr_sensors();

    therm_mgr_ready = 1;
}

static void therm_check_sensor_staleness(void) {
    for (uint32_t i = 0; i < THERM_SENSOR_CAPACITY; ++i) {
        thermal_sensor_t *sensor = &therm_sensors[i];
        if (sensor->state != THERM_SENSOR_AVAILABLE) {
            continue;
        }
        if (current_timer_ticks() - sensor->timestamp < THERM_STALE_TICKS) {
            continue;
        }
        sensor->state = THERM_SENSOR_STALE;
    }
}

static void therm_evaluate_zones(void) {
    for (uint32_t zi = 0; zi < therm_zone_count; ++zi) {
        thermal_zone_t *zone = &therm_zones[zi];

        /* Maximale gueltige Temperatur aller Sensoren dieser Zone bestimmen. */
        int32_t max_temp = 0;
        int have_valid = 0;
        for (uint32_t si = 0; si < THERM_SENSOR_CAPACITY; ++si) {
            if ((zone->sensor_mask & (1u << si)) == 0) {
                continue;
            }
            thermal_sensor_t *sensor = &therm_sensors[si];
            if (sensor->state != THERM_SENSOR_AVAILABLE) {
                continue;
            }
            if (!have_valid || sensor->value_mc > max_temp) {
                max_temp = sensor->value_mc;
                have_valid = 1;
            }
        }

        if (!have_valid) {
            /* Kein valider Sensor -> UNKNOWN (NPSPEC: Unknown != Safe). */
            zone->state = THERM_STATE_UNKNOWN;
            continue;
        }

        int32_t prev_temp = zone->temp_mc;
        zone->temp_mc = max_temp;
        if (max_temp > prev_temp) {
            zone->trend = 1;
        } else if (max_temp < prev_temp) {
            zone->trend = -1;
        } else {
            zone->trend = 0;
        }

        if (max_temp >= zone->limit_emergency) {
            zone->state = THERM_STATE_EMERGENCY;
            zone->cooling_actions = COOL_ACTION_FAN | COOL_ACTION_FREQ | COOL_ACTION_POWER | COOL_ACTION_SHUTDOWN;
        } else if (max_temp >= zone->limit_critical) {
            zone->state = THERM_STATE_CRITICAL;
            zone->cooling_actions = COOL_ACTION_FAN | COOL_ACTION_FREQ | COOL_ACTION_POWER;
        } else if (max_temp >= zone->limit_hot) {
            zone->state = THERM_STATE_HOT;
            zone->cooling_actions = COOL_ACTION_FAN | COOL_ACTION_FREQ;
        } else if (max_temp >= zone->limit_elevated) {
            zone->state = THERM_STATE_ELEVATED;
            zone->cooling_actions = COOL_ACTION_FAN;
        } else {
            zone->state = THERM_STATE_NORMAL;
            zone->cooling_actions = COOL_ACTION_NONE;
        }

        /* Headroom: Budget = (Limit_Hot - Temp) / (Limit_Hot - Limit_Elevated) * 100. */
        thermal_headroom_t *headroom = &therm_headrooms[zi];
        int32_t span = zone->limit_hot - zone->limit_elevated;
        int32_t remaining = zone->limit_hot - max_temp;
        uint32_t budget_pct;
        if (remaining <= 0 || span <= 0) {
            budget_pct = 0;
        } else {
            int32_t pct = (remaining * 100) / span;
            budget_pct = (pct > 100) ? 100u : (uint32_t)pct;
        }
        headroom->zone = zone->id;
        headroom->budget_pct = budget_pct;
    }
}

static void therm_safety_check(void) {
    for (uint32_t i = 0; i < therm_zone_count; ++i) {
        if (therm_zones[i].state == THERM_STATE_EMERGENCY) {
            serial_puts("NOVA: THERMAL EMERGENCY - Notabschaltung\r\n");
            power_manager_request_shutdown();
        }
    }
}

int thermal_manager_update(void) {
    if (therm_mgr_ready != 1) {
        return 0;
    }
    therm_read_msr_sensors();
    therm_check_sensor_staleness();
    therm_evaluate_zones();
    therm_safety_check();
    return 0;
}

uint32_t thermal_get_zone_state(uint32_t zone_id) {
    if (zone_id >= therm_zone_count) {
        return THERM_STATE_UNKNOWN;
    }
    return therm_zones[zone_id].state;
}

int thermal_get_headroom(uint32_t zone_id, uint32_t *out_budget_pct) {
    if (zone_id >= therm_zone_count) {
        return -1;
    }
    *out_budget_pct = therm_headrooms[zone_id].budget_pct;
    return 0;
}

int thermal_manager_self_test(void) {
    if (therm_mgr_ready != 1) return -1;
    if (therm_zone_count == 0) return -1;

    uint32_t state = thermal_get_zone_state(0);
    if (state > THERM_STATE_UNKNOWN) return -1; /* UNKNOWN oder gueltiger Zustand = ok */

    return 0;
}
