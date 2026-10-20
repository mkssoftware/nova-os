/* NovaOS C-Kernel - Panic-Reporter und Minimal-Crash-Dump
   (Port aus integration32.inc, ADR-2014). Byte-Layout ist nicht mehr an die
   alte ASM-Struktur gebunden, da nichts Externes diese Offsets mehr liest -
   Feldnamen/Semantik bleiben 1:1 erhalten. */
#include "panic.h"
#include "serial.h"
#include "boot_phase.h"
#include <stddef.h>

#define PANIC_REPORT_SIZE 48

typedef struct {
    uint32_t error_code;
    uint32_t subsystem;
    uint32_t eip;
    uint32_t esp;
    uint32_t cr2;
    uint32_t timer_ticks;
    uint32_t boot_phase;
    uint32_t boot_last_phase;
    uint32_t cpu_id;
    uint32_t security_state;
} panic_report_t;

static panic_report_t panic_report;

/* Platzhalter bis das Security-Subsystem portiert ist (ASM: CONTEXT_SECURITY_STATE). */
static uint32_t security_state_stub = 0;

void panic_manager_initialize(void) {
    panic_report = (panic_report_t){0};
}

int panic_manager_self_test(void) {
    return 0;
}

#define CRASH_DUMP_COMMIT_MARKER 0x504D5544u /* "DUMP" */
#define CRASH_STATUS_EMPTY       0u
#define CRASH_STATUS_WRITING     1u
#define CRASH_STATUS_COMPLETE    2u
#define CRASH_STATUS_PARTIAL     3u

typedef struct {
    uint32_t magic0; /* "NVCD" */
    uint32_t magic1; /* "UMP1" */
    uint32_t status;
    uint32_t hash;
    uint32_t commit_marker;
    uint32_t recursive_flag;
    panic_report_t report;
} crash_dump_t;

static crash_dump_t crash_dump;
static int crash_dump_active = 0;
static uint32_t crash_dump_recursions = 0;
static uint32_t crash_dump_completed = 0;
static uint32_t crash_dump_partial = 0;

void crash_dump_initialize(void) {
    crash_dump = (crash_dump_t){0};
    crash_dump_active = 0;
    crash_dump_recursions = 0;
    crash_dump_completed = 0;
    crash_dump_partial = 0;
}

/* Bounded additive Integritaetspruefsumme, identisch zum alten ASM
   (rol eax,5; xor eax,byte) ueber den Report. */
static uint32_t crash_dump_hash(const uint8_t *data, uint32_t size) {
    uint32_t hash = 0;
    for (uint32_t i = 0; i < size; ++i) {
        hash = (hash << 5) | (hash >> 27);
        hash ^= data[i];
    }
    return hash;
}

static int crash_dump_capture_minimal(void) {
    if (crash_dump_active) {
        ++crash_dump_recursions;
        ++crash_dump_partial;
        crash_dump.status = CRASH_STATUS_PARTIAL;
        crash_dump.recursive_flag = 1;
        crash_dump.commit_marker = CRASH_DUMP_COMMIT_MARKER;
        return -1;
    }
    crash_dump_active = 1;
    crash_dump.magic0 = 0x4443564Eu;
    crash_dump.magic1 = 0x31504D55u;
    crash_dump.status = CRASH_STATUS_WRITING;
    crash_dump.report = panic_report;
    crash_dump.hash = crash_dump_hash((const uint8_t *)&crash_dump.report, (uint32_t)sizeof(crash_dump.report));
    crash_dump.commit_marker = CRASH_DUMP_COMMIT_MARKER;
    crash_dump.status = CRASH_STATUS_COMPLETE;
    ++crash_dump_completed;
    crash_dump_active = 0;
    return 0;
}

static int crash_dump_validate(void) {
    if (crash_dump.magic0 != 0x4443564Eu) return -1;
    if (crash_dump.magic1 != 0x31504D55u) return -1;
    if (crash_dump.status != CRASH_STATUS_COMPLETE) return -1;
    if (crash_dump.commit_marker != CRASH_DUMP_COMMIT_MARKER) return -1;
    uint32_t hash = crash_dump_hash((const uint8_t *)&crash_dump.report, (uint32_t)sizeof(crash_dump.report));
    return (hash == crash_dump.hash) ? 0 : -1;
}

int crash_dump_self_test(void) {
    if (crash_dump_capture_minimal() != 0) return -1;
    if (crash_dump_validate() != 0) return -1;
    if (crash_dump_completed != 1) return -1;
    /* Der Selbsttest gilt nicht als echter Crash: Slot danach freigeben. */
    crash_dump_initialize();
    return 0;
}

static inline uint32_t read_cr2(void) {
    uint32_t value;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(value));
    return value;
}

static uint32_t read_apic_id(void) {
    uint32_t eax = 1, ebx, ecx, edx;
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(eax));
    return ebx >> 24;
}

void kernel_halt(void) {
    __asm__ volatile ("cli");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void kernel_panic(uint32_t error_code, uint32_t subsystem, const char *message) {
    __asm__ volatile ("cli");

    uint32_t current_esp;
    __asm__ volatile ("mov %%esp, %0" : "=r"(current_esp));

    panic_report.error_code = error_code;
    panic_report.subsystem = subsystem;
    panic_report.eip = (uint32_t)(uintptr_t)__builtin_return_address(0);
    panic_report.esp = current_esp;
    panic_report.cr2 = read_cr2();
    panic_report.timer_ticks = 0; /* TODO: Time-Subsystem */
    panic_report.boot_phase = boot_phase_current;
    panic_report.boot_last_phase = boot_phase_last_success;
    panic_report.cpu_id = read_apic_id();
    panic_report.security_state = security_state_stub;

    crash_dump_capture_minimal();
    /* TODO: draw_kernel_panic_screen - folgt mit dem Grafik-Subsystem */

    serial_puts("NOVA PANIC: Kernel-Exception code=0x");
    serial_put_hex32(panic_report.error_code);
    serial_puts(" subsystem=0x");
    serial_put_hex32(panic_report.subsystem);
    serial_puts(" eip=0x");
    serial_put_hex32(panic_report.eip);
    serial_puts(" cr2=0x");
    serial_put_hex32(panic_report.cr2);
    serial_puts(" phase=0x");
    serial_put_hex32(panic_report.boot_phase);
    serial_puts(" last_phase=0x");
    serial_put_hex32(panic_report.boot_last_phase);
    serial_puts(" cpu=0x");
    serial_put_hex32(panic_report.cpu_id);
    serial_puts(" security=0x");
    serial_put_hex32(panic_report.security_state);
    serial_puts("\r\n");
    serial_puts(message);
    serial_puts("\r\nNOVA_PANIC_HALTED\r\n");

    kernel_halt();
}
