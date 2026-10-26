/* NovaOS C-Kernel - Nova Thermal Throttle Policy (Port aus adaptive32.inc). */
#include "thermthrottle.h"
#include "scheduler.h" /* qos_new_epoch */

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t temperature;
    uint32_t thresh_warm;
    uint32_t thresh_hot;
} thermal_zone_rec_t;

static thermal_zone_rec_t thermal_zones[THERM_ZONE_COUNT];
static int thermal_ready = 0;
static uint32_t thermal_throttle_events = 0;

void thermal_initialize(void) {
    for (uint32_t i = 0; i < THERM_ZONE_COUNT; ++i) thermal_zones[i] = (thermal_zone_rec_t){0};
    thermal_throttle_events = 0;

    /* Default-Zone 0: Schwellwerte warm=70, hot=90. */
    thermal_zones[0].id = 0;
    thermal_zones[0].state = THERM_STATE_NORMAL;
    thermal_zones[0].temperature = 40;
    thermal_zones[0].thresh_warm = 70;
    thermal_zones[0].thresh_hot = 90;

    thermal_ready = 1;
}

int thermal_update(uint32_t zone_id, uint32_t temperature, int *out_throttled) {
    if (thermal_ready != 1) return -1;
    if (zone_id >= THERM_ZONE_COUNT) return -1;

    thermal_zone_rec_t *zone = &thermal_zones[zone_id];
    zone->temperature = temperature;

    if (temperature >= zone->thresh_hot) {
        zone->state = THERM_STATE_HOT;
        ++thermal_throttle_events;
        qos_new_epoch(); /* alle QoS-Klassen drosseln */
        *out_throttled = 1;
    } else if (temperature >= zone->thresh_warm) {
        zone->state = THERM_STATE_WARM;
        *out_throttled = 0;
    } else {
        zone->state = THERM_STATE_NORMAL;
        *out_throttled = 0;
    }
    return 0;
}

int thermal_query(uint32_t zone_id, uint32_t *out_state, uint32_t *out_temperature) {
    if (zone_id >= THERM_ZONE_COUNT) return -1;
    *out_state = thermal_zones[zone_id].state;
    *out_temperature = thermal_zones[zone_id].temperature;
    return 0;
}

int thermal_self_test(void) {
    int throttled;
    if (thermal_update(0, 50, &throttled) != 0) return -1;
    if (throttled) return -1; /* 50 < warm(70): kein Throttle erwartet */

    uint32_t state, temp;
    if (thermal_query(0, &state, &temp) != 0) return -1;
    if (state != THERM_STATE_NORMAL) return -1;

    if (thermal_update(0, 95, &throttled) != 0) return -1;
    if (!throttled) return -1; /* 95 >= hot(90): Throttle erwartet */

    if (thermal_query(0, &state, &temp) != 0) return -1;
    if (state != THERM_STATE_HOT) return -1;

    return 0;
}
