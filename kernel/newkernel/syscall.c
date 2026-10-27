/* NovaOS C-Kernel - §011 System Call Interface (Port aus integration32.inc
   und io_qos32.inc's syscall_validate_user_range). Siehe syscall.h fuer
   den Hinweis zum nicht portierten SYSENTER-Trampolin/Dispatch. */
#include "syscall.h"
#include "serial.h"
#include "scheduler.h"
#include "surface.h"
#include "ring3.h"

static int syscall_mgr_initialized = 0;
static int syscall_sysenter_available = 0;
static uint32_t syscall_stats_successful = 0;
static uint32_t syscall_stats_failed = 0;
static uint32_t syscall_call_depth = 0;
static volatile int syscall_ready_heartbeat_logged = 0;

/* Byte-Offsets in den von interrupt_isr.asm erzeugten Trap-Frame (siehe
   syscall.h fuer die Herleitung aus pushad+Segmentregister-Reihenfolge). */
#define FRAME_OFF_EDI    16u
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
   iretd den von syscall_dispatch umgeschriebenen Frame konsumiert hat.
   Der Ring-3-Rundweg ist damit abgeschlossen - ERST JETZT wird der echte
   Taskwechsel scharf geschaltet (scheduler_enable, siehe scheduler.h):
   das garantiert, dass kein Timer-Tick den Ring-3-Heartbeat/Exit-Rundweg
   vorzeitig unterbrechen konnte (scheduler_on_tick war bis hierhin ein
   No-Op), und ist selbst wieder eine EINWEG-Uebergabe - der folgende
   hlt-Block laeuft nur, bis der naechste Tick zugunsten von
   scheduler_thread1/2 verdraengt. */
static void syscall_userspace_exit(void) {
    serial_puts("NOVA: Ring 3 -> Ring 0 per SYSCALL_CORE_EXIT zurueckgekehrt\r\n");
    scheduler_enable();
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

/* Einziger bisher existierender Client (ring3.c) - echte Mehr-Client-PID-
   Aufloesung (z.B. ueber den aktuell laufenden Scheduler-Slot) ist bewusst
   nicht in dieser Sitzung geloest, siehe syscall.h. */
#define GUI_SINGLE_CLIENT_PID 1u

static uint32_t syscall_dispatch_gui(uint32_t frame_ptr, uint32_t operation) {
    uint32_t edx = frame_read(frame_ptr, FRAME_OFF_EDX);

    if (operation == GUI_OP_CREATE_SURFACE) {
        uint32_t x = edx & 0xFFFFu;
        uint32_t y = (edx >> 16) & 0xFFFFu;
        uint32_t esi = frame_read(frame_ptr, FRAME_OFF_ESI);
        uint32_t w = esi & 0xFFFFu;
        uint32_t h = (esi >> 16) & 0xFFFFu;

        uint32_t handle = 0;
        if (surface_create(GUI_SINGLE_CLIENT_PID, x, y, w, h, &handle) != 0 ||
            surface_configure(handle, x, y, w, h) != 0) {
            ++syscall_stats_failed;
            frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_SURFACE);
            return frame_ptr;
        }

        ++syscall_stats_successful;
        frame_write(frame_ptr, FRAME_OFF_EDI, handle);
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OK);
        return frame_ptr;
    }

    if (operation == GUI_OP_ATTACH_BUFFER) {
        int ok = (surface_attach_buffer(edx, RING3_GUI_BUFFER_VADDR) == 0);
        if (ok) ++syscall_stats_successful; else ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX,
                    (uint32_t)(ok ? SYSCALL_STATUS_OK : SYSCALL_STATUS_SURFACE));
        return frame_ptr;
    }

    if (operation == GUI_OP_COMMIT) {
        int ok = (surface_commit(edx) == 0);
        if (ok) ++syscall_stats_successful; else ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX,
                    (uint32_t)(ok ? SYSCALL_STATUS_OK : SYSCALL_STATUS_SURFACE));
        return frame_ptr;
    }

    if (operation == GUI_OP_DESTROY) {
        int ok = (surface_destroy(edx) == 0);
        if (ok) ++syscall_stats_successful; else ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX,
                    (uint32_t)(ok ? SYSCALL_STATUS_OK : SYSCALL_STATUS_SURFACE));
        return frame_ptr;
    }

    ++syscall_stats_failed;
    frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OPERATION);
    return frame_ptr;
}

uint32_t syscall_dispatch(uint32_t frame_ptr) {
    uint32_t service = frame_read(frame_ptr, FRAME_OFF_EAX);
    uint32_t operation = frame_read(frame_ptr, FRAME_OFF_EBX);
    uint32_t abi_version = frame_read(frame_ptr, FRAME_OFF_ECX);

    if (abi_version != SYSCALL_ABI_VERSION) {
        ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_ABI);
        return frame_ptr;
    }

    if (service == SYSCALL_SERVICE_GUI) {
        return syscall_dispatch_gui(frame_ptr, operation);
    }

    if (service != SYSCALL_SERVICE_CORE) {
        ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_SERVICE);
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

    /* SYSCALL_SERVICE_GUI-Dispatch mit einem synthetischen, auf dem Stack
       angelegten Trap-Frame pruefen (1:1 dasselbe Layout wie der echte,
       von interrupt_isr.asm gebaute Frame) - ohne echten int-0x80/Ring-3-
       Rundweg, der erst mit ring3_initialize() (laeuft deutlich spaeter im
       Boot) existiert. Das deckt die reine Dispatch-Logik (Parameter-
       Dekodierung, surface.c-Anbindung, Fehlerpfade) unabhaengig ab.
       WICHTIG: syscall_dispatch() schreibt EAX IMMER mit dem Status-Code
       voll (genau wie beim Ring-3-Shellcode in ring3.c) - EAX muss daher
       vor JEDEM Aufruf neu auf das gewuenschte Service gesetzt werden,
       sonst liest der naechste Dispatch den vorherigen Status-Code als
       Service und schlaegt mit SYSCALL_STATUS_SERVICE fehl. */
    surface_manager_initialize();
    uint8_t fake_frame[72] = {0};
    uint32_t frame_addr = (uint32_t)(uintptr_t)fake_frame;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_CREATE_SURFACE);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, 5u | (6u << 16)); /* x=5, y=6 */
    frame_write(frame_addr, FRAME_OFF_ESI, 7u | (8u << 16)); /* w=7, h=8 */
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    uint32_t gui_handle = frame_read(frame_addr, FRAME_OFF_EDI);
    if (gui_handle == 0) return -1;
    uint32_t gx, gy, gw, gh;
    if (surface_rect(gui_handle, &gx, &gy, &gw, &gh) != 0) return -1;
    if (gx != 5u || gy != 6u || gw != 7u || gh != 8u) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_ATTACH_BUFFER);
    frame_write(frame_addr, FRAME_OFF_EDX, gui_handle);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (surface_buffer_addr(gui_handle) != RING3_GUI_BUFFER_VADDR) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_COMMIT);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (surface_state(gui_handle) != SURFACE_STATE_ACTIVE) return -1;

    /* Unbekanntes Handle muss SYSCALL_STATUS_SURFACE liefern, unbekannte
       GUI-Operation SYSCALL_STATUS_OPERATION - beide Fehlerpfade duerfen
       die bereits aktive Surface nicht beeinflussen. */
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_COMMIT);
    frame_write(frame_addr, FRAME_OFF_EDX, 0xFFFFu);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_SURFACE) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, 0xFFu);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OPERATION) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_DESTROY);
    frame_write(frame_addr, FRAME_OFF_EDX, gui_handle);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (surface_state(gui_handle) != SURFACE_STATE_FREE) return -1;

    serial_puts("NOVA: System Call Interface Selbsttest OK (7/7)\r\n");
    return 0;
}
