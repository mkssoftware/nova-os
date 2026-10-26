/* NovaOS C-Kernel - §011 System Call Interface (Port aus integration32.inc
   und io_qos32.inc's syscall_validate_user_range). Siehe syscall.h fuer
   den Hinweis zum nicht portierten SYSENTER-Trampolin/Dispatch. */
#include "syscall.h"
#include "serial.h"

static int syscall_mgr_initialized = 0;
static int syscall_sysenter_available = 0;
static uint32_t syscall_stats_successful = 0;
static uint32_t syscall_stats_failed = 0;
static uint32_t syscall_call_depth = 0;

int syscall_validate_user_range(uint32_t addr, uint32_t length) {
    if (length == 0) return -1;
    if (addr < USER_ADDRESS_MIN) return -1;

    uint32_t end = addr + length;
    if (end < addr) return -1; /* Overflow */
    if (end <= addr) return -1;
    if (end > USER_ADDRESS_MAX) return -1;

    return 0;
}

void syscall_manager_initialize(void) {
    uint32_t eax, edx;
    __asm__ volatile ("cpuid" : "=a"(eax), "=d"(edx) : "a"(1) : "ebx", "ecx");

    if (edx & (1u << 11)) {
        syscall_sysenter_available = 1;
        serial_puts("NOVA: SYSENTER verfuegbar und konfiguriert\r\n");
    } else {
        syscall_sysenter_available = 0;
        serial_puts("NOVA: SYSENTER nicht verfuegbar, nur INT 0x80 aktiv\r\n");
    }

    syscall_stats_successful = 0;
    syscall_stats_failed = 0;
    syscall_call_depth = 0;

    syscall_mgr_initialized = 1;
}

int syscall_manager_self_test(void) {
    if (syscall_mgr_initialized != 1) return -1;

    if (syscall_validate_user_range(0xC0001000u, 4) == 0) return -1; /* Kernel-Adresse: abgelehnt */
    if (syscall_validate_user_range(USER_ADDRESS_MIN, 0) == 0) return -1; /* Nulllaenge: abgelehnt */
    if (syscall_validate_user_range(0xFFFF0000u, 0x00020000u) == 0) return -1; /* Overflow: abgelehnt */

    if (syscall_stats_successful == 0xFFFFFFFFu) return -1;
    if (syscall_call_depth != 0) return -1;
    if (syscall_sysenter_available > 1) return -1;

    serial_puts("NOVA: System Call Interface Selbsttest OK (7/7)\r\n");
    return 0;
}
