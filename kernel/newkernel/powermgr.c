/* NovaOS C-Kernel - Power Manager ABI 1.0 (Port aus integration32.inc). */
#include "powermgr.h"
#include "interrupt.h"
#include "serial.h"

#define POWER_SUPPORTED_MASK ((1u << POWER_STATE_RUNNING) | (1u << POWER_STATE_IDLE) | \
                               (1u << POWER_STATE_SHUTDOWN) | (1u << POWER_STATE_RESTART) | \
                               (1u << POWER_STATE_OFF))

static uint32_t power_current_state = POWER_STATE_RUNNING;
static uint32_t power_target_state = POWER_STATE_RUNNING;
static uint32_t power_profile = POWER_PROFILE_BALANCED;
static uint32_t power_transition_phase = POWER_PHASE_NONE;
static int power_transition_active = 0;
static uint32_t power_shutdown_requests = 0;
static uint32_t power_transition_failures = 0;
static uint32_t power_wake_lock_count = 0;
static uint32_t power_wake_lock_expirations = 0;
static uint32_t power_idle_state = POWER_IDLE_SHALLOW;
static uint32_t power_idle_entries = 0;
static uint32_t power_deep_idle_entries = 0;
static uint32_t power_profile_changes = 0;
static uint32_t power_wake_lock_deadlines[POWER_WAKE_LOCK_CAPACITY];

void power_manager_initialize(void) {
    power_current_state = POWER_STATE_RUNNING;
    power_target_state = POWER_STATE_RUNNING;
    power_profile = POWER_PROFILE_BALANCED;
    power_transition_phase = POWER_PHASE_NONE;
    power_transition_active = 0;
    power_shutdown_requests = 0;
    power_transition_failures = 0;
    power_wake_lock_count = 0;
    power_wake_lock_expirations = 0;
    power_idle_entries = 0;
    power_deep_idle_entries = 0;
    power_profile_changes = 0;
    for (uint32_t i = 0; i < POWER_WAKE_LOCK_CAPACITY; ++i) power_wake_lock_deadlines[i] = 0;
}

int power_wake_lock_acquire(uint32_t duration_ticks, uint32_t *out_slot) {
    if (duration_ticks == 0) return -1;
    if (power_wake_lock_count >= POWER_WAKE_LOCK_CAPACITY) return -1;

    uint32_t deadline = timer_get_ticks() + duration_ticks;

    for (uint32_t i = 0; i < POWER_WAKE_LOCK_CAPACITY; ++i) {
        if (power_wake_lock_deadlines[i] == 0) {
            power_wake_lock_deadlines[i] = deadline;
            ++power_wake_lock_count;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int power_wake_lock_release(uint32_t slot) {
    if (slot >= POWER_WAKE_LOCK_CAPACITY) return -1;
    if (power_wake_lock_deadlines[slot] == 0) return -1;
    power_wake_lock_deadlines[slot] = 0;
    if (power_wake_lock_count == 0) return -1;
    --power_wake_lock_count;
    return 0;
}

void power_wake_lock_expire(void) {
    uint32_t now = timer_get_ticks();
    for (uint32_t i = 0; i < POWER_WAKE_LOCK_CAPACITY; ++i) {
        uint32_t deadline = power_wake_lock_deadlines[i];
        if (deadline == 0) continue;
        if ((int32_t)(now - deadline) < 0) continue; /* noch nicht abgelaufen */
        power_wake_lock_deadlines[i] = 0;
        --power_wake_lock_count;
        ++power_wake_lock_expirations;
    }
}

void power_cpu_idle(void) {
    power_wake_lock_expire();
    ++power_idle_entries;
    if (power_wake_lock_count == 0) {
        power_idle_state = POWER_IDLE_DEEP;
        ++power_deep_idle_entries;
    } else {
        power_idle_state = POWER_IDLE_SHALLOW;
    }
    __asm__ volatile ("hlt");
}

int power_manager_set_profile(uint32_t profile) {
    if (profile > POWER_PROFILE_POWERSAVE) return -1;
    if (power_transition_active) return -1;
    if (power_profile != profile) {
        power_profile = profile;
        ++power_profile_changes;
    }
    return 0;
}

int power_manager_request_shutdown(void) {
    if (power_transition_active) {
        ++power_transition_failures;
        return -1;
    }
    power_transition_active = 1;
    ++power_shutdown_requests;
    power_target_state = POWER_STATE_SHUTDOWN;

    power_transition_phase = POWER_PHASE_REQUEST;
    serial_puts("NOVA: Shutdown angefordert\r\n");
    power_transition_phase = POWER_PHASE_FREEZE_USERSPACE;
    serial_puts("NOVA: Userspace wird eingefroren\r\n");
    power_transition_phase = POWER_PHASE_SYNC_STORAGE;
    serial_puts("NOVA: Storage wird synchronisiert\r\n");
    power_transition_phase = POWER_PHASE_STOP_DEVICES;
    serial_puts("NOVA: Geraete werden gestoppt\r\n");

    power_current_state = POWER_STATE_SHUTDOWN;
    return 0;
}

void power_manager_platform_off(void) {
    power_transition_phase = POWER_PHASE_PLATFORM_OFF;
    serial_puts("NOVA: Plattform wird abgeschaltet\r\n");

    __asm__ volatile ("outw %0, %1" : : "a"((uint16_t)0x2000), "Nd"((uint16_t)0x0604)); /* QEMU/ACPI */
    __asm__ volatile ("outw %0, %1" : : "a"((uint16_t)0x2000), "Nd"((uint16_t)0xB004)); /* Bochs/QEMU Fallback */

    power_current_state = POWER_STATE_OFF;
}

int power_manager_self_test(void) {
    if (power_current_state != POWER_STATE_RUNNING) return -1;

    uint32_t slot;
    if (power_wake_lock_acquire(10, &slot) != 0) return -1;
    if (power_wake_lock_count != 1) return -1;
    if (power_wake_lock_release(slot) != 0) return -1;
    if (power_wake_lock_count != 0) return -1;

    return 0;
}
