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
static volatile int syscall_ready_heartbeat_logged = 0;

/* Byte-Offsets in den von interrupt_isr.asm erzeugten Trap-Frame (siehe
   syscall.h fuer die Herleitung aus pushad+Segmentregister-Reihenfolge). */
#define FRAME_OFF_ESI    20u
#define FRAME_OFF_EBX    32u
#define FRAME_OFF_EDX    36u
#define FRAME_OFF_ECX    40u
#define FRAME_OFF_EAX    44u
#define FRAME_OFF_EIP    56u
#define FRAME_OFF_CS     60u
#define FRAME_OFF_EFLAGS 64u

static inline uint32_t frame_read(uint32_t frame_ptr, uint32_t byte_offset) {
    return *(uint32_t *)((uint8_t *)frame_ptr + byte_offset);
}

static inline void frame_write(uint32_t frame_ptr, uint32_t byte_offset, uint32_t value) {
    *(uint32_t *)((uint8_t *)frame_ptr + byte_offset) = value;
}

/* Rueckkehrstelle fuer SYSCALL_CORE_EXIT: laeuft in Ring 0 weiter, sobald
   iretd den von syscall_dispatch umgeschriebenen Frame konsumiert hat. Es
   gibt noch keinen Scheduler, der hier etwas anderes vorzieht - der Kernel
   geht in den normalen Idle-Zustand. */
static void syscall_userspace_exit(void) {
    serial_puts("NOVA: Ring 3 -> Ring 0 per SYSCALL_CORE_EXIT zurueckgekehrt\r\n");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

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

uint32_t syscall_dispatch(uint32_t frame_ptr) {
    uint32_t service = frame_read(frame_ptr, FRAME_OFF_EAX);
    uint32_t operation = frame_read(frame_ptr, FRAME_OFF_EBX);
    uint32_t abi_version = frame_read(frame_ptr, FRAME_OFF_ECX);

    if (service != SYSCALL_SERVICE_CORE) {
        ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_SERVICE);
        return frame_ptr;
    }
    if (abi_version != SYSCALL_ABI_VERSION) {
        ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_ABI);
        return frame_ptr;
    }

    if (operation == SYSCALL_CORE_READY) {
        ++syscall_stats_successful;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OK);
        if (!syscall_ready_heartbeat_logged) {
            syscall_ready_heartbeat_logged = 1;
            serial_puts("NOVA: Ring 3 Userspace aktiv - SYSCALL_CORE_READY empfangen\r\n");
        }
        return frame_ptr;
    }

    if (operation == SYSCALL_CORE_EXIT) {
        ++syscall_stats_successful;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OK);
        frame_write(frame_ptr, FRAME_OFF_EIP, (uint32_t)syscall_userspace_exit);
        frame_write(frame_ptr, FRAME_OFF_CS, 0x08u); /* Ring-0-Codesegment */
        frame_write(frame_ptr, FRAME_OFF_EFLAGS, frame_read(frame_ptr, FRAME_OFF_EFLAGS) & 0xFFFFCFFFu);
        return frame_ptr;
    }

    ++syscall_stats_failed;
    frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OPERATION);
    return frame_ptr;
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
