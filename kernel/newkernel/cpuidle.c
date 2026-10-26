/* NovaOS C-Kernel - Nova CPU Idle State Management (Port aus
   adaptive32.inc). */
#include "cpuidle.h"

typedef struct {
    uint32_t state;
    uint32_t enter_tick;
    uint32_t residency;
    uint32_t count;
} cpuidle_record_t;

static cpuidle_record_t cpuidle_stats[4];
static int cpuidle_ready = 0;
static uint32_t cpuidle_current = CPUIDLE_STATE_C0;
static uint32_t cpuidle_total_halts = 0;

/* TODO: durch wd_global_tick ersetzen, sobald das Watchdog-Subsystem
   portiert ist (wie in aslr.c). */
static uint32_t wd_global_tick_stub(void) {
    return 0;
}

void cpuidle_initialize(void) {
    for (uint32_t i = 0; i < 4; ++i) cpuidle_stats[i] = (cpuidle_record_t){0};
    cpuidle_current = CPUIDLE_STATE_C0;
    cpuidle_total_halts = 0;
    cpuidle_ready = 1;
}

int cpuidle_enter(uint32_t state) {
    if (cpuidle_ready != 1) return -1;
    if (state > CPUIDLE_STATE_C3) return -1;

    cpuidle_current = state;
    cpuidle_record_t *rec = &cpuidle_stats[state];
    rec->state = state;
    rec->enter_tick = wd_global_tick_stub();
    ++rec->count;
    ++cpuidle_total_halts;

    if (state != CPUIDLE_STATE_C0) {
        __asm__ volatile ("hlt"); /* CPU schlaeft bis zum naechsten Interrupt */
    }
    return 0;
}

void cpuidle_exit(void) {
    if (cpuidle_ready != 1) return;

    cpuidle_record_t *rec = &cpuidle_stats[cpuidle_current];
    uint32_t now = wd_global_tick_stub();
    rec->residency += now - rec->enter_tick;
    cpuidle_current = CPUIDLE_STATE_C0;
}

int cpuidle_query(uint32_t state, uint32_t *out_residency_ticks, uint32_t *out_enter_count) {
    if (state > CPUIDLE_STATE_C3) return -1;
    *out_residency_ticks = cpuidle_stats[state].residency;
    *out_enter_count = cpuidle_stats[state].count;
    return 0;
}

int cpuidle_self_test(void) {
    if (cpuidle_enter(CPUIDLE_STATE_C1) != 0) return -1;
    cpuidle_exit();

    uint32_t residency, count;
    if (cpuidle_query(CPUIDLE_STATE_C1, &residency, &count) != 0) return -1;
    if (count < 1) return -1;

    return 0;
}
