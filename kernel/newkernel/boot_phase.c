/* NovaOS C-Kernel - Boot-Phasen-Tracking (Port aus integration32.inc). */
#include "boot_phase.h"
#include "serial.h"

#define BOOT_LOG_CAPACITY      12
#define BOOT_COMPONENT_KERNEL  0x4B45524Eu

uint32_t boot_phase_current = BOOT_PHASE_NONE;
uint32_t boot_phase_last_success = BOOT_PHASE_NONE;

typedef struct {
    uint32_t timer_ticks; /* TODO: sobald Time-Subsystem portiert ist */
    uint32_t reserved0;
    uint32_t phase;
    uint32_t component;
    uint32_t reserved1;
    uint32_t reserved2;
} boot_event_t;

static boot_event_t boot_events[BOOT_LOG_CAPACITY];
static uint32_t boot_event_count = 0;

/* Schreibt genau ein Bootereignis in den statischen Ring; ein voller Ring
   verwirft weitere Ereignisse deterministisch (identisch zum alten ASM). */
static void boot_event_record_current(void) {
    if (boot_event_count >= BOOT_LOG_CAPACITY) {
        return;
    }
    boot_event_t *event = &boot_events[boot_event_count];
    event->timer_ticks = 0;
    event->reserved0 = 0;
    event->phase = boot_phase_current;
    event->component = BOOT_COMPONENT_KERNEL;
    event->reserved1 = 0;
    event->reserved2 = 0;
    ++boot_event_count;
}

void boot_phase_log(void) {
    boot_event_record_current();
    serial_puts("NOVA: BOOT phase=0x");
    serial_put_hex32(boot_phase_current);
    serial_puts(" last_phase=0x");
    serial_put_hex32(boot_phase_last_success);
    serial_puts("\r\n");
}
