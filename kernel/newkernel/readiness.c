/* NovaOS C-Kernel - Nova Progressive Readiness (siehe readiness.h fuer
   Scope). */
#include "readiness.h"
#include "threadmgr.h"
#include "network.h"
#include "powermgr.h"
#include "interrupt.h"

static volatile uint32_t comp_network_state = COMP_STATE_NOT_STARTED;
static volatile uint32_t comp_power_state = COMP_STATE_NOT_STARTED;
static volatile uint32_t bg_init_trigger = 0;
static volatile uint32_t bg_init_complete = 0;
static volatile uint32_t bg_init_net_ok = 0;
static volatile uint32_t bg_init_power_ok = 0;
static uint32_t boot_core_services_ticks = 0;

void readiness_initialize(void) {
    comp_network_state = COMP_STATE_NOT_STARTED;
    comp_power_state = COMP_STATE_NOT_STARTED;
    bg_init_trigger = 0;
    bg_init_complete = 0;
    bg_init_net_ok = 0;
    bg_init_power_ok = 0;
    boot_core_services_ticks = 0;
}

/* Scheduler-Thread-Koerper (parameterlos, nicht zurueckkehrend - siehe
   thread_create_dynamic). Kein serial_write_string/Ausgabe hier - laeuft
   parallel zum Ring-3-Rundweg, der Hauptthread meldet den Abschluss nach
   seiner Rueckkehr (siehe kmain.c). */
static void readiness_bg_thread_body(void) {
    for (;;) {
        if (bg_init_trigger != 1u) {
            __asm__ volatile ("pause");
            continue;
        }
        break;
    }

    comp_network_state = COMP_STATE_INITIALIZING;
    network_stack_initialize();
    if (network_stack_self_test() == 0) {
        comp_network_state = COMP_STATE_READY;
        bg_init_net_ok = 1;
    } else {
        comp_network_state = COMP_STATE_FAILED;
    }

    comp_power_state = COMP_STATE_INITIALIZING;
    power_manager_initialize();
    if (power_manager_self_test() == 0) {
        comp_power_state = COMP_STATE_READY;
        bg_init_power_ok = 1;
    } else {
        comp_power_state = COMP_STATE_FAILED;
    }

    boot_core_services_ticks = timer_get_ticks();
    bg_init_complete = 1;

    for (;;) {
        __asm__ volatile ("pause");
    }
}

int readiness_start_background_init(void) {
    uint32_t tid;
    return thread_create_dynamic((uint32_t)(uintptr_t)readiness_bg_thread_body, 1u, -1,
                                  &tid);
}

void readiness_trigger(void) {
    bg_init_trigger = 1;
}

void readiness_sync_fallback(void) {
    if (bg_init_complete == 1u) return;

    if (comp_network_state != COMP_STATE_READY) {
        comp_network_state = COMP_STATE_INITIALIZING;
        network_stack_initialize();
        if (network_stack_self_test() == 0) {
            comp_network_state = COMP_STATE_READY;
            bg_init_net_ok = 1;
        } else {
            comp_network_state = COMP_STATE_FAILED;
        }
    } else {
        bg_init_net_ok = 1;
    }

    if (comp_power_state != COMP_STATE_READY) {
        comp_power_state = COMP_STATE_INITIALIZING;
        power_manager_initialize();
        if (power_manager_self_test() == 0) {
            comp_power_state = COMP_STATE_READY;
            bg_init_power_ok = 1;
        } else {
            comp_power_state = COMP_STATE_FAILED;
        }
    } else {
        bg_init_power_ok = 1;
    }

    boot_core_services_ticks = timer_get_ticks();
    bg_init_complete = 1;
}

int readiness_is_complete(void) {
    return bg_init_complete == 1u;
}

uint32_t readiness_network_state(void) {
    return comp_network_state;
}

uint32_t readiness_power_state(void) {
    return comp_power_state;
}

uint32_t readiness_core_services_ticks(void) {
    return boot_core_services_ticks;
}

int readiness_self_test(void) {
    readiness_initialize();
    if (readiness_is_complete()) return -1;
    if (readiness_network_state() != COMP_STATE_NOT_STARTED) return -1;
    if (readiness_power_state() != COMP_STATE_NOT_STARTED) return -1;

    /* Synchroner Fallback-Pfad direkt pruefen (der Hintergrund-Thread
       selbst ist in diesem frühen Selbsttest-Kontext noch nicht
       scheduler_enable()-faehig, siehe readiness.h). */
    readiness_sync_fallback();
    if (!readiness_is_complete()) return -1;
    if (readiness_network_state() != COMP_STATE_READY &&
        readiness_network_state() != COMP_STATE_FAILED) {
        return -1;
    }
    if (readiness_power_state() != COMP_STATE_READY &&
        readiness_power_state() != COMP_STATE_FAILED) {
        return -1;
    }
    if (readiness_core_services_ticks() == 0u && timer_get_ticks() != 0u) {
        /* nur informativ - kein harter Fehlschlag, da Tick-Werte frueh im
           Boot durchaus noch 0 sein koennen */
    }

    /* Erneuter Aufruf nach Abschluss muss ein No-Op bleiben (idempotent). */
    uint32_t ticks_before = readiness_core_services_ticks();
    readiness_sync_fallback();
    if (readiness_core_services_ticks() != ticks_before) return -1;

    readiness_initialize();
    return 0;
}
