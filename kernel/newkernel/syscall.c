/* NovaOS C-Kernel - §011 System Call Interface (Port aus integration32.inc
   und io_qos32.inc's syscall_validate_user_range). Siehe syscall.h fuer
   den Hinweis zum nicht portierten SYSENTER-Trampolin/Dispatch. */
#include "syscall.h"
#include "serial.h"
#include "scheduler.h"
#include "surface.h"
#include "ring3.h"
#include "window.h"
#include "readiness.h"
#include "vfsapi.h"
#include "novafs.h"
#include "input.h"
#include "interrupt.h"
#include "paging.h"
#include "time.h"
#include "powermgr.h"
#include "display.h"
#include "cap.h"
#include "process.h"

static int syscall_mgr_initialized = 0;
/* Sichtbarkeits-Flag fuer das Startmenue (siehe SYSCALL_CORE_TOGGLE_
   START_MENU/_GET_START_MENU_VISIBLE in syscall.h) - KEIN Fenster mehr,
   nur noch dieses eine Bit, das von zwei getrennten Ring-3-Prozessen
   (Taskbar schaltet um, Startmenue-Prozess fragt ab) gemeinsam genutzt
   wird. */
static int syscall_start_menu_visible = 0;

/* NPSPEC-SECURITY-CAPABILITY-0001: erste echte Laufzeit-Nutzung des §103
   Capability Frameworks (cap.h/cap_core.c, vorher nur von dessen eigenen
   Selbsttests ausgeuebt). Genau ein Prozess kann Inhaber des Shutdown-
   Capability sein. */
#define CAP_RIGHT_SHUTDOWN 0x01u
static uint32_t syscall_shutdown_authorized_pid = 0;
static uint32_t syscall_shutdown_cap_id = 0;

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
    /* VOR scheduler_enable() aufrufen, nicht danach: Slot 0 (dieser
       Kontext) wird beim ersten Tick nach scheduler_enable() permanent
       verdraengt (siehe scheduler.h) und laeuft womoeglich nie wieder -
       ein Aufruf danach koennte durch einen Tick mitten in der Ausfuehrung
       unterbrochen werden und nie fertig laufen. Hier, noch vor
       scheduler_enable(), ist der Aufruf garantiert unterbrechungsfrei. */
    readiness_sync_fallback();
    serial_puts("NOVA: Progressive Readiness abgeschlossen - Netzwerk=");
    serial_put_hex32(readiness_network_state());
    serial_puts(" Power=");
    serial_put_hex32(readiness_power_state());
    serial_puts("\r\n");
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
    syscall_shutdown_authorized_pid = 0;
    syscall_shutdown_cap_id = 0;

    syscall_mgr_initialized = 1;
}

int syscall_grant_shutdown_capability(uint32_t pid) {
    uint32_t cap_id = 0;
    if (cap_create(pid, CAP_RIGHT_SHUTDOWN, 0u, CAP_DOMAIN_KERNEL, &cap_id) != 0) {
        return -1;
    }
    syscall_shutdown_authorized_pid = pid;
    syscall_shutdown_cap_id = cap_id;
    return 0;
}

int syscall_shutdown_authorized(uint32_t caller_cr3) {
    if (syscall_shutdown_authorized_pid == 0) {
        return 0; /* noch keine Vergabe erfolgt */
    }
    uint32_t caller_pid = process_find_by_cr3(caller_cr3);
    if (caller_pid == 0 || caller_pid != syscall_shutdown_authorized_pid) {
        return 0;
    }
    return (cap_check(syscall_shutdown_cap_id, CAP_RIGHT_SHUTDOWN) == 0) ? 1 : 0;
}

static uint32_t syscall_dispatch_gui(uint32_t frame_ptr, uint32_t operation) {
    uint32_t edx = frame_read(frame_ptr, FRAME_OFF_EDX);

    if (operation == GUI_OP_CREATE_SURFACE) {
        uint32_t x = edx & 0xFFFFu;
        uint32_t y = (edx >> 16) & 0xFFFFu;
        uint32_t esi = frame_read(frame_ptr, FRAME_OFF_ESI);
        uint32_t w = esi & 0xFFFFu;
        uint32_t h = (esi >> 16) & 0xFFFFu;

        /* window_create() ruft intern surface_create()+surface_configure()
           auf und registriert zusaetzlich die window.c-Metadaten (Z-Order/
           Parent/Modus) - jede ueber den Syscall-Pfad erzeugte Surface ist
           damit automatisch ein vollwertiges window.c-Fenster, auf dem
           GUI_OP_WINDOW_SET_STATE (Minimize/Maximize/Fullscreen/Restore)
           arbeiten kann. decorated=0, da das Wire-Protokoll (EDX/ESI schon
           voll mit x/y/w/h) kein Dekorations-Bit hat - ein Client kann
           vorerst keine Titelleiste ueber den Syscall-Pfad anfordern. */
        uint32_t handle = 0;
        if (window_create(SYSCALL_SINGLE_CLIENT_PID, x, y, w, h, 0, &handle) != 0) {
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
        /* RING3_GUI_BUFFER_VADDR ist dieselbe virtuelle Adresse in JEDEM
           Prozess-Adressraum (jeder Client mappt seinen eigenen Puffer
           privat dorthin, siehe ring3.c/ushell_process.c - kein Konflikt,
           da jeder Prozess seit paging_create_address_space() einen
           eigenen Adressraum hat). Der Compositor dereferenziert
           surface_buffer_addr() aber als rohen Zeiger, UNABHAENGIG vom
           gerade aktiven CR3 (siehe compositor.c). Deshalb wird hier,
           waehrend noch das CR3 des AUFRUFENDEN Prozesses aktiv ist (ein
           Syscall wechselt das CR3 nicht), einmalig auf die physische
           Adresse aufgeloest - die bleibt unter JEDEM CR3 gueltig, da der
           komplette Bootstrap-Speicherbereich in jedem Adressraum
           identisch (geklont) abgebildet ist. Schlaegt die Aufloesung
           fehl (z.B. im synthetischen Dispatch-Selbsttest ohne echtes
           Mapping), faellt dies bewusst auf die rohe virtuelle Adresse
           zurueck - identisch zum fruheren, immer-hartcodierten
           Verhalten. */
        uint32_t resolved_addr = RING3_GUI_BUFFER_VADDR;
        uint32_t phys, phys_flags;
        if (paging_query_page(RING3_GUI_BUFFER_VADDR, &phys, &phys_flags) == 0) {
            resolved_addr = phys;
        }
        int ok = (surface_attach_buffer(edx, resolved_addr) == 0);
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
        int ok = (window_destroy(edx) == 0); /* raeumt auch die Surface auf */
        if (ok) ++syscall_stats_successful; else ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX,
                    (uint32_t)(ok ? SYSCALL_STATUS_OK : SYSCALL_STATUS_SURFACE));
        return frame_ptr;
    }

    if (operation == GUI_OP_HIDE) {
        int ok = (surface_hide(edx) == 0);
        if (ok) ++syscall_stats_successful; else ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX,
                    (uint32_t)(ok ? SYSCALL_STATUS_OK : SYSCALL_STATUS_SURFACE));
        return frame_ptr;
    }

    if (operation == GUI_OP_WINDOW_SET_STATE) {
        uint32_t esi = frame_read(frame_ptr, FRAME_OFF_ESI);
        int ok;
        if (esi == GUI_WINDOW_STATE_MINIMIZE) {
            ok = (window_minimize(edx) == 0);
        } else if (esi == GUI_WINDOW_STATE_MAXIMIZE) {
            ok = (window_maximize(edx) == 0);
        } else if (esi == GUI_WINDOW_STATE_FULLSCREEN) {
            ok = (window_set_fullscreen(edx) == 0);
        } else if (esi == GUI_WINDOW_STATE_RESTORE) {
            ok = (window_restore(edx) == 0);
        } else {
            ok = 0;
        }
        if (ok) ++syscall_stats_successful; else ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX,
                    (uint32_t)(ok ? SYSCALL_STATUS_OK : SYSCALL_STATUS_SURFACE));
        return frame_ptr;
    }

    ++syscall_stats_failed;
    frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OPERATION);
    return frame_ptr;
}

/* Schreibt result ins EAX und zaehlt die Statistik - result==0 gilt als
   Erfolg, jeder andere Wert (ein negativer VFSAPI_STATUS_*-Code oder ein
   SYSCALL_STATUS_*-Dispatch-Fehler) als Fehlschlag. */
static uint32_t syscall_vfs_finish(uint32_t frame_ptr, int result) {
    if (result == 0) ++syscall_stats_successful; else ++syscall_stats_failed;
    frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)result);
    return frame_ptr;
}

static uint32_t syscall_dispatch_vfs(uint32_t frame_ptr, uint32_t operation) {
    uint32_t args_addr = frame_read(frame_ptr, FRAME_OFF_EDX);

    if (operation == VFS_OP_LOOKUP) {
        if (syscall_validate_user_range(args_addr, sizeof(syscall_vfs_lookup_args_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        syscall_vfs_lookup_args_t *args = (syscall_vfs_lookup_args_t *)(uintptr_t)args_addr;
        if (syscall_validate_user_range(args->path_addr, args->path_len) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        uint32_t result_handle = 0;
        int rc = vfsapi_lookup(SYSCALL_SINGLE_CLIENT_PID, args->dir_handle,
                                (const char *)(uintptr_t)args->path_addr, args->path_len,
                                args->flags, &result_handle);
        if (rc == 0) args->result_handle = result_handle;
        return syscall_vfs_finish(frame_ptr, rc);
    }

    if (operation == VFS_OP_READ || operation == VFS_OP_WRITE) {
        if (syscall_validate_user_range(args_addr, sizeof(syscall_vfs_io_args_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        syscall_vfs_io_args_t *args = (syscall_vfs_io_args_t *)(uintptr_t)args_addr;
        if (syscall_validate_user_range(args->buffer_addr, args->length) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        uint32_t transferred = 0;
        int rc;
        if (operation == VFS_OP_READ) {
            rc = vfsapi_read(SYSCALL_SINGLE_CLIENT_PID, args->handle, args->offset,
                              (uint8_t *)(uintptr_t)args->buffer_addr, args->length, &transferred);
        } else {
            rc = vfsapi_write(SYSCALL_SINGLE_CLIENT_PID, args->handle, args->offset,
                               (const uint8_t *)(uintptr_t)args->buffer_addr, args->length,
                               &transferred);
        }
        if (rc == 0) args->transferred = transferred;
        return syscall_vfs_finish(frame_ptr, rc);
    }

    if (operation == VFS_OP_CREATE) {
        if (syscall_validate_user_range(args_addr, sizeof(syscall_vfs_create_args_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        syscall_vfs_create_args_t *args = (syscall_vfs_create_args_t *)(uintptr_t)args_addr;
        if (syscall_validate_user_range(args->name_addr, args->name_len) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        uint32_t result_handle = 0;
        int rc = vfsapi_create(SYSCALL_SINGLE_CLIENT_PID, args->dir_handle,
                                (const char *)(uintptr_t)args->name_addr, args->name_len,
                                args->object_type, &result_handle);
        if (rc == 0) args->result_handle = result_handle;
        return syscall_vfs_finish(frame_ptr, rc);
    }

    if (operation == VFS_OP_READ_DIRECTORY) {
        if (syscall_validate_user_range(args_addr, sizeof(syscall_vfs_read_dir_args_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        syscall_vfs_read_dir_args_t *args = (syscall_vfs_read_dir_args_t *)(uintptr_t)args_addr;
        if (syscall_validate_user_range(args->entry_addr, sizeof(vfsapi_dir_entry_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        int rc = vfsapi_read_directory(SYSCALL_SINGLE_CLIENT_PID, args->dir_handle, args->index,
                                        (vfsapi_dir_entry_t *)(uintptr_t)args->entry_addr);
        return syscall_vfs_finish(frame_ptr, rc);
    }

    if (operation == VFS_OP_QUERY) {
        if (syscall_validate_user_range(args_addr, sizeof(syscall_vfs_query_args_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        syscall_vfs_query_args_t *args = (syscall_vfs_query_args_t *)(uintptr_t)args_addr;
        if (syscall_validate_user_range(args->info_addr, sizeof(vfsapi_info_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        int rc = vfsapi_query(SYSCALL_SINGLE_CLIENT_PID, args->handle,
                               (vfsapi_info_t *)(uintptr_t)args->info_addr);
        return syscall_vfs_finish(frame_ptr, rc);
    }

    if (operation == VFS_OP_DELETE) {
        if (syscall_validate_user_range(args_addr, sizeof(syscall_vfs_delete_args_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        syscall_vfs_delete_args_t *args = (syscall_vfs_delete_args_t *)(uintptr_t)args_addr;
        if (syscall_validate_user_range(args->name_addr, args->name_len) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        int rc = vfsapi_delete(SYSCALL_SINGLE_CLIENT_PID, args->dir_handle,
                                (const char *)(uintptr_t)args->name_addr, args->name_len);
        return syscall_vfs_finish(frame_ptr, rc);
    }

    if (operation == VFS_OP_RENAME) {
        if (syscall_validate_user_range(args_addr, sizeof(syscall_vfs_rename_args_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        syscall_vfs_rename_args_t *args = (syscall_vfs_rename_args_t *)(uintptr_t)args_addr;
        if (syscall_validate_user_range(args->src_name_addr, args->src_name_len) != 0 ||
            syscall_validate_user_range(args->dst_name_addr, args->dst_name_len) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        int rc = vfsapi_rename(SYSCALL_SINGLE_CLIENT_PID, args->src_dir_handle,
                                (const char *)(uintptr_t)args->src_name_addr, args->src_name_len,
                                args->dst_dir_handle,
                                (const char *)(uintptr_t)args->dst_name_addr, args->dst_name_len);
        return syscall_vfs_finish(frame_ptr, rc);
    }

    if (operation == VFS_OP_CLOSE) {
        if (syscall_validate_user_range(args_addr, sizeof(syscall_vfs_close_args_t)) != 0) {
            return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_POINTER);
        }
        syscall_vfs_close_args_t *args = (syscall_vfs_close_args_t *)(uintptr_t)args_addr;
        int rc = vfsapi_close(SYSCALL_SINGLE_CLIENT_PID, args->handle);
        return syscall_vfs_finish(frame_ptr, rc);
    }

    return syscall_vfs_finish(frame_ptr, SYSCALL_STATUS_OPERATION);
}

/* "Scharf geschaltet" von syscall_manager_self_test() nach dessen eigenem,
   garantiert leerem Abschluss (siehe dort) - verhindert, dass der
   Selbsttest-eigene synthetische Event-Durchlauf als Beweis durchgeht.
   Danach loggt syscall_dispatch_input() EINMALIG das erste echte, an
   einen Ring-3-Client zugestellte Event - exakt das gleiche "erstes-
   echtes-Ergebnis"-Muster wie compositor.c's Framebuffer-Pixel-Beweis
   fuer GUI_OP_COMMIT (siehe dort compositor_log_client_proof_once). */
static int syscall_input_proof_armed = 0;
static int syscall_input_proof_logged = 0;

static uint32_t syscall_dispatch_input(uint32_t frame_ptr, uint32_t operation) {
    uint32_t args_addr = frame_read(frame_ptr, FRAME_OFF_EDX);

    if (operation != INPUT_OP_POLL) {
        ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OPERATION);
        return frame_ptr;
    }

    if (syscall_validate_user_range(args_addr, sizeof(syscall_input_poll_args_t)) != 0) {
        ++syscall_stats_failed;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_POINTER);
        return frame_ptr;
    }
    syscall_input_poll_args_t *args = (syscall_input_poll_args_t *)(uintptr_t)args_addr;

    /* NPSPEC-INPUT-EVENT-0001 Abschnitt 5: jeder Ring-3-Client bekommt
       jetzt seine EIGENE, an seine PID gebundene Warteschlange statt mit
       allen anderen Clients um eine einzige globale FIFO zu konkurrieren
       (siehe input.h/input_client_poll_for() fuer die volle Begruendung -
       vorher konnte ein schneller/haeufiger Poller einem anderen Prozess
       systematisch jedes Event wegschnappen). */
    input_event_t evt;
    if (input_client_poll_for(process_find_by_cr3(paging_root_physical()), &evt) == 0) {
        args->has_event = 1u;
        args->type = (uint32_t)evt.type;
        args->timestamp_ticks = evt.timestamp_ticks;
        args->key_vkey = evt.key_vkey;
        args->modifiers = evt.modifiers;
        args->mouse_dx = evt.mouse_dx;
        args->mouse_dy = evt.mouse_dy;
        args->mouse_buttons = evt.mouse_buttons;

        if (syscall_input_proof_armed && !syscall_input_proof_logged) {
            syscall_input_proof_logged = 1;
            serial_puts("NOVA: GUI-Input-Syscall liefert echtes Event an Ring-3-Client - Typ=");
            serial_put_hex32(args->type);
            serial_puts(" Vkey=");
            serial_put_hex32(args->key_vkey);
            serial_puts("\r\n");
        }
    } else {
        args->has_event = 0u;
    }
    args->cursor_x = input_cursor_x();
    args->cursor_y = input_cursor_y();

    ++syscall_stats_successful;
    frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OK);
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

    if (service == SYSCALL_SERVICE_VFS) {
        return syscall_dispatch_vfs(frame_ptr, operation);
    }

    if (service == SYSCALL_SERVICE_INPUT) {
        return syscall_dispatch_input(frame_ptr, operation);
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

    if (operation == SYSCALL_CORE_GET_DISPLAY_INFO) {
        ++syscall_stats_successful;
        frame_write(frame_ptr, FRAME_OFF_EDI, display_width());
        frame_write(frame_ptr, FRAME_OFF_ESI, display_height());
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OK);
        return frame_ptr;
    }

    if (operation == SYSCALL_CORE_GET_TIME) {
        time_civil_t civil;
        time_read_rtc(&civil);
        ++syscall_stats_successful;
        frame_write(frame_ptr, FRAME_OFF_EDI, civil.hour);
        frame_write(frame_ptr, FRAME_OFF_ESI, civil.minute);
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OK);
        return frame_ptr;
    }

    if (operation == SYSCALL_CORE_TOGGLE_START_MENU) {
        syscall_start_menu_visible = !syscall_start_menu_visible;
        ++syscall_stats_successful;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OK);
        return frame_ptr;
    }

    if (operation == SYSCALL_CORE_GET_START_MENU_VISIBLE) {
        ++syscall_stats_successful;
        frame_write(frame_ptr, FRAME_OFF_EDI, (uint32_t)syscall_start_menu_visible);
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OK);
        return frame_ptr;
    }

    if (operation == SYSCALL_CORE_SHUTDOWN) {
        /* NPSPEC-SECURITY-CAPABILITY-0001: nur der Prozess, dem per
           syscall_grant_shutdown_capability() das Shutdown-Capability
           uebertragen wurde (siehe ushell_process_initialize()), darf die
           Maschine herunterfahren - fail-closed, jeder andere Aufrufer
           (oder ein Aufruf bevor die Vergabe ueberhaupt stattgefunden hat)
           wird abgelehnt. */
        if (!syscall_shutdown_authorized(paging_root_physical())) {
            ++syscall_stats_failed;
            frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_CAP);
            return frame_ptr;
        }
        ++syscall_stats_successful;
        frame_write(frame_ptr, FRAME_OFF_EAX, (uint32_t)SYSCALL_STATUS_OK);
        /* Sicheres Herunterfahren: erst die geordnete Uebergangssequenz
           (Userspace einfrieren, Storage synchronisieren, Geraete
           stoppen), danach der tatsaechliche ACPI-Poweroff - identisch
           zur fruheren, jetzt entfernten Shutdown-Klick-Behandlung in
           desktop_shell.c. Kehrt im Erfolgsfall nicht zurueck. */
        power_manager_request_shutdown();
        power_manager_platform_off();
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
    window_manager_initialize();
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

    /* GUI_OP_HIDE: Surface bleibt bestehen, wird aber HIDDEN statt
       ACTIVE - und per erneutem COMMIT wieder sichtbar (siehe
       ushell/startmenu_main.c's Zeig/Verbirg-Zyklus). */
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_HIDE);
    frame_write(frame_addr, FRAME_OFF_EDX, gui_handle);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (surface_state(gui_handle) != SURFACE_STATE_HIDDEN) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_COMMIT);
    frame_write(frame_addr, FRAME_OFF_EDX, gui_handle);
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

    /* GUI_OP_WINDOW_SET_STATE: Minimize->Restore->Maximize->Restore ueber
       denselben Dispatch-Pfad (EDX=Handle, ESI=GUI_WINDOW_STATE_*). */
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_WINDOW_SET_STATE);
    frame_write(frame_addr, FRAME_OFF_EDX, gui_handle);
    frame_write(frame_addr, FRAME_OFF_ESI, GUI_WINDOW_STATE_MINIMIZE);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (window_mode(gui_handle) != WINDOW_MODE_MINIMIZED) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_WINDOW_SET_STATE);
    frame_write(frame_addr, FRAME_OFF_ESI, GUI_WINDOW_STATE_MAXIMIZE);
    syscall_dispatch(frame_addr); /* MAXIMIZE waehrend minimiert -> Fehler */
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_SURFACE) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_WINDOW_SET_STATE);
    frame_write(frame_addr, FRAME_OFF_ESI, GUI_WINDOW_STATE_RESTORE);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (window_mode(gui_handle) != WINDOW_MODE_NORMAL) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_WINDOW_SET_STATE);
    frame_write(frame_addr, FRAME_OFF_ESI, GUI_WINDOW_STATE_MAXIMIZE);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (window_mode(gui_handle) != WINDOW_MODE_MAXIMIZED) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_WINDOW_SET_STATE);
    frame_write(frame_addr, FRAME_OFF_ESI, GUI_WINDOW_STATE_RESTORE);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (window_mode(gui_handle) != WINDOW_MODE_NORMAL) return -1;

    /* Unbekannte GUI_WINDOW_STATE_*-Konstante muss SYSCALL_STATUS_SURFACE
       liefern (wie ein unbekanntes Handle), nicht abstuerzen. */
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_WINDOW_SET_STATE);
    frame_write(frame_addr, FRAME_OFF_ESI, 0xFFu);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_SURFACE) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_GUI);
    frame_write(frame_addr, FRAME_OFF_EBX, GUI_OP_DESTROY);
    frame_write(frame_addr, FRAME_OFF_EDX, gui_handle);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (surface_state(gui_handle) != SURFACE_STATE_FREE) return -1;

    /* SYSCALL_SERVICE_VFS-Dispatch (syscall_dispatch_vfs): anders als der
       obige synthetische GUI-Frame-Test genuegt hier nicht nur ein
       Fake-Frame - die Args-Struktur selbst muss an einer echten,
       per syscall_validate_user_range() gueltigen Adresse liegen (siehe
       syscall.h). USER_ADDRESS_MIN (4 MiB) ist von paging.c identity-
       gemappt und damit aus Ring 0 direkt beschreibbar - genau wie bei
       jedem echten Ring-3-Client, der dort seine Args-Struktur ablegt,
       OHNE dass hierfuer ring3_initialize() (laeuft noch gar nicht)
       gebraucht wird. Baut auf dem bereits von vfsapi_self_test()
       gemounteten Testvolume auf (siehe kmain.c - "/Benutzer" existiert
       dort bereits), statt ein drittes eigenes NovaFS-Testvolume
       anzulegen. */
    uint8_t *user_base = (uint8_t *)(uintptr_t)USER_ADDRESS_MIN;
    syscall_vfs_lookup_args_t *lookup_args = (syscall_vfs_lookup_args_t *)(user_base + 0);
    char *path_buf = (char *)(user_base + 64);
    syscall_vfs_create_args_t *create_args = (syscall_vfs_create_args_t *)(user_base + 128);
    char *name_buf = (char *)(user_base + 192);
    syscall_vfs_io_args_t *io_args = (syscall_vfs_io_args_t *)(user_base + 256);
    uint8_t *data_buf = user_base + 320;
    syscall_vfs_close_args_t *close_args = (syscall_vfs_close_args_t *)(user_base + 384);

    const char benutzer_path[9] = {'/', 'B', 'e', 'n', 'u', 't', 'z', 'e', 'r'};
    for (uint32_t i = 0; i < 9u; ++i) path_buf[i] = benutzer_path[i];

    lookup_args->dir_handle = 0u;
    lookup_args->path_addr = (uint32_t)(uintptr_t)path_buf;
    lookup_args->path_len = 9u;
    lookup_args->flags = VFSAPI_LOOKUP_FLAG_WRITE;
    lookup_args->result_handle = 0u;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_VFS);
    frame_write(frame_addr, FRAME_OFF_EBX, VFS_OP_LOOKUP);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, (uint32_t)(uintptr_t)lookup_args);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != 0u) return -1;
    uint32_t benutzer_handle = lookup_args->result_handle;
    if (benutzer_handle == 0u) return -1;

    const char file_name[12] = {'d', 'i', 's', 'p', 'a', 't', 'c', 'h', '.', 't', 'x', 't'};
    for (uint32_t i = 0; i < 12u; ++i) name_buf[i] = file_name[i];

    create_args->dir_handle = benutzer_handle;
    create_args->name_addr = (uint32_t)(uintptr_t)name_buf;
    create_args->name_len = 12u;
    create_args->object_type = NOVAFS_TYPE_FILE;
    create_args->result_handle = 0u;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_VFS);
    frame_write(frame_addr, FRAME_OFF_EBX, VFS_OP_CREATE);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, (uint32_t)(uintptr_t)create_args);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != 0u) return -1;
    uint32_t file_handle = create_args->result_handle;
    if (file_handle == 0u) return -1;

    data_buf[0] = 'O'; data_buf[1] = 'K'; data_buf[2] = '!'; data_buf[3] = '!';
    io_args->handle = file_handle;
    io_args->buffer_addr = (uint32_t)(uintptr_t)data_buf;
    io_args->offset = 0u;
    io_args->length = 4u;
    io_args->transferred = 0u;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_VFS);
    frame_write(frame_addr, FRAME_OFF_EBX, VFS_OP_WRITE);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, (uint32_t)(uintptr_t)io_args);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != 0u) return -1;
    if (io_args->transferred != 4u) return -1;

    data_buf[0] = 0; data_buf[1] = 0; data_buf[2] = 0; data_buf[3] = 0;
    io_args->transferred = 0u;
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_VFS);
    frame_write(frame_addr, FRAME_OFF_EBX, VFS_OP_READ);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, (uint32_t)(uintptr_t)io_args);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != 0u) return -1;
    if (io_args->transferred != 4u) return -1;
    if (data_buf[0] != 'O' || data_buf[1] != 'K' || data_buf[2] != '!' || data_buf[3] != '!') {
        return -1;
    }

    /* Unbekannte Operation -> SYSCALL_STATUS_OPERATION, ungueltiger
       Args-Zeiger (ausserhalb [USER_ADDRESS_MIN, USER_ADDRESS_MAX)) ->
       SYSCALL_STATUS_POINTER - beide Fehlerpfade duerfen die bereits
       offenen Handles nicht beeinflussen. */
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_VFS);
    frame_write(frame_addr, FRAME_OFF_EBX, 0xFFu);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OPERATION) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_VFS);
    frame_write(frame_addr, FRAME_OFF_EBX, VFS_OP_LOOKUP);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, 0xC0001000u); /* Kernel-Adresse */
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_POINTER) return -1;

    close_args->handle = file_handle;
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_VFS);
    frame_write(frame_addr, FRAME_OFF_EBX, VFS_OP_CLOSE);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, (uint32_t)(uintptr_t)close_args);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != 0u) return -1;

    close_args->handle = benutzer_handle;
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_VFS);
    frame_write(frame_addr, FRAME_OFF_EBX, VFS_OP_CLOSE);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, (uint32_t)(uintptr_t)close_args);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != 0u) return -1;

    /* Aufraeumen: die eben erzeugte Testdatei nicht dauerhaft im
       gemeinsam genutzten Testvolume zuruecklassen. */
    uint32_t cleanup_handle;
    if (vfsapi_lookup(SYSCALL_SINGLE_CLIENT_PID, 0u, benutzer_path, 9u,
                       VFSAPI_LOOKUP_FLAG_WRITE, &cleanup_handle) == 0) {
        vfsapi_delete(SYSCALL_SINGLE_CLIENT_PID, cleanup_handle, file_name, 12u);
        vfsapi_close(SYSCALL_SINGLE_CLIENT_PID, cleanup_handle);
    }

    /* SYSCALL_SERVICE_INPUT-Dispatch (syscall_dispatch_input): ein
       synthetisches Event direkt in input.c's rohen ISR-Ringpuffer
       geben (input_push_event), per input_drain_pending() deterministisch
       in die Client-Warteschlange ueberfuehren (ohne auf den echten,
       bereits laufenden Input-Thread warten zu muessen), dann ueber den
       echten Syscall-Dispatch abholen. */
    syscall_input_poll_args_t *poll_args = (syscall_input_poll_args_t *)(user_base + 448);

    /* Echte IRQ1/IRQ12-Handler laufen seit dem Input-System tatsaechlich
       asynchron im Hintergrund (siehe ps2.c's gleichnamige Lektion zu
       ps2_mouse_cycle) und koennen jederzeit ein echtes Event in
       input.c's Warteschlangen legen (z.B. ein Rausch-Paket, falls IRQ12
       ohne tatsaechlich streamende Maus feuert). Ohne Schutz koennte ein
       solches Event VOR dem gleich gepushten synthetischen Event landen
       und von input_client_poll() zuerst ausgeliefert werden - der Test
       wuerde dann faelschlich am Event-Inhalt scheitern, nicht an der
       Dispatch-Logik selbst. Fix: Interrupts kurz sperren, beide
       Warteschlangen (roher Ring + Client-Warteschlange) leeren, DANN
       erst das synthetische Event einspeisen - exakt das gleiche Muster
       wie ps2_controller_self_test's Absicherung. */
    interrupt_disable();

    /* input_client_poll_for() bindet eine PID ERST beim ersten Poll-Aufruf
       an einen Slot - ein Broadcast, der VOR dieser Bindung passiert,
       erreicht den neuen Slot nicht. Darum hier VORAB (leerer Dummy-Poll)
       binden, BEVOR unten das synthetische Event eingespeist wird - sonst
       ginge genau dieses Event an Slot 0 (noch ungebunden), waehrend der
       gleich folgende echte syscall_dispatch()-Aufruf bereits aus dem neu
       gebundenen PID-Slot liest und faelschlich "kein Event" saehe. */
    input_event_t drain_tmp;
    (void)input_client_poll_for(process_find_by_cr3(paging_root_physical()), &drain_tmp);
    while (input_pop_event(&drain_tmp) == 0) { }
    /* Leert ALLE Client-Slots (nicht nur Slot 0). */
    input_client_drain_all();

    input_event_t synth_evt = {0};
    synth_evt.type = INPUT_EVENT_KEY_DOWN;
    synth_evt.key_vkey = 'Q';
    synth_evt.modifiers = INPUT_MOD_LSHIFT;
    input_push_event(&synth_evt);
    input_drain_pending();

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_INPUT);
    frame_write(frame_addr, FRAME_OFF_EBX, INPUT_OP_POLL);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, (uint32_t)(uintptr_t)poll_args);
    syscall_dispatch(frame_addr);
    int input_ok = 1;
    if (frame_read(frame_addr, FRAME_OFF_EAX) != 0u) input_ok = 0;
    if (input_ok && poll_args->has_event != 1u) input_ok = 0;
    if (input_ok && poll_args->type != (uint32_t)INPUT_EVENT_KEY_DOWN) input_ok = 0;
    if (input_ok && poll_args->key_vkey != (uint32_t)'Q') input_ok = 0;
    if (input_ok && poll_args->modifiers != INPUT_MOD_LSHIFT) input_ok = 0;

    /* Erneutes Poll ohne neues Event: has_event==0, aber EAX bleibt OK
       (eine leere Warteschlange ist kein Fehler, siehe syscall.h). */
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_INPUT);
    frame_write(frame_addr, FRAME_OFF_EBX, INPUT_OP_POLL);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    frame_write(frame_addr, FRAME_OFF_EDX, (uint32_t)(uintptr_t)poll_args);
    syscall_dispatch(frame_addr);
    if (input_ok && frame_read(frame_addr, FRAME_OFF_EAX) != 0u) input_ok = 0;
    if (input_ok && poll_args->has_event != 0u) input_ok = 0;

    interrupt_enable();

    if (!input_ok) return -1;

    /* Unbekannte Operation -> SYSCALL_STATUS_OPERATION. */
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_INPUT);
    frame_write(frame_addr, FRAME_OFF_EBX, 0xFFu);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OPERATION) return -1;

    /* Ab hier sind beide Input-Warteschlangen wieder garantiert leer
       (siehe oben) - ein spaeteres has_event!=0 in syscall_dispatch_input
       kann also nur noch von einem echten, nach diesem Selbsttest
       eingetroffenen Event stammen. Siehe ring3.c fuer den Ring-3-Client,
       der als erster davon profitiert. */
    syscall_input_proof_armed = 1;

    /* SYSCALL_CORE_GET_DISPLAY_INFO/_GET_TIME (ushell/shell_main.c's
       Ersatz fuer den in Ring 3 verbotenen direkten display.c-/RTC-Port-
       Zugriff): beide muessen SYSCALL_STATUS_OK liefern und plausible
       Werte in EDI/ESI schreiben - exakt dieselben Werte, die display.c/
       time_read_rtc() auch direkt lieferten (kein neuer Wert entsteht,
       nur ein neuer Zugriffsweg). */
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_CORE);
    frame_write(frame_addr, FRAME_OFF_EBX, SYSCALL_CORE_GET_DISPLAY_INFO);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (frame_read(frame_addr, FRAME_OFF_EDI) != display_width()) return -1;
    if (frame_read(frame_addr, FRAME_OFF_ESI) != display_height()) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_CORE);
    frame_write(frame_addr, FRAME_OFF_EBX, SYSCALL_CORE_GET_TIME);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (frame_read(frame_addr, FRAME_OFF_EDI) > 23u) return -1; /* Stunde */
    if (frame_read(frame_addr, FRAME_OFF_ESI) > 59u) return -1; /* Minute */

    /* SYSCALL_CORE_TOGGLE_START_MENU/_GET_START_MENU_VISIBLE: reines
       Kernel-Flag (kein Fenster mehr noetig) - zweimal umschalten muss
       exakt zum Ausgangszustand zurueckfuehren, GET muss den jeweils
       aktuellen Wert widerspiegeln. */
    uint32_t visible_before;
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_CORE);
    frame_write(frame_addr, FRAME_OFF_EBX, SYSCALL_CORE_GET_START_MENU_VISIBLE);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    visible_before = frame_read(frame_addr, FRAME_OFF_EDI);

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_CORE);
    frame_write(frame_addr, FRAME_OFF_EBX, SYSCALL_CORE_TOGGLE_START_MENU);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_CORE);
    frame_write(frame_addr, FRAME_OFF_EBX, SYSCALL_CORE_GET_START_MENU_VISIBLE);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (frame_read(frame_addr, FRAME_OFF_EDI) == visible_before) return -1; /* muss gekippt sein */

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_CORE);
    frame_write(frame_addr, FRAME_OFF_EBX, SYSCALL_CORE_TOGGLE_START_MENU);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;

    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_CORE);
    frame_write(frame_addr, FRAME_OFF_EBX, SYSCALL_CORE_GET_START_MENU_VISIBLE);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_OK) return -1;
    if (frame_read(frame_addr, FRAME_OFF_EDI) != visible_before) return -1; /* zurueck auf Ausgangszustand */

    /* NPSPEC-SECURITY-CAPABILITY-0001: der echte Dispatch-Pfad fuer
       SYSCALL_CORE_SHUTDOWN darf HIER gefahrlos durchlaufen werden, weil
       an dieser Boot-Stelle (deutlich vor ushell_process_initialize(),
       das als einziger Aufrufer syscall_grant_shutdown_capability()
       aufruft) noch KEINE Vergabe stattgefunden hat - die Autorisierungs-
       pruefung schlaegt also sicher VOR power_manager_request_shutdown()
       fehl, kein echter ACPI-Poweroff. */
    frame_write(frame_addr, FRAME_OFF_EAX, SYSCALL_SERVICE_CORE);
    frame_write(frame_addr, FRAME_OFF_EBX, SYSCALL_CORE_SHUTDOWN);
    frame_write(frame_addr, FRAME_OFF_ECX, SYSCALL_ABI_VERSION);
    syscall_dispatch(frame_addr);
    if (frame_read(frame_addr, FRAME_OFF_EAX) != (uint32_t)SYSCALL_STATUS_CAP) return -1;

    /* Direkter Test der Autorisierungslogik mit einem eigenen, dedizierten
       Test-Prozess (nicht die dauerhaften PIDs anderer Selbsttests) -
       beweist PASS (richtige PID + richtiges Capability), FAIL (falsches
       cr3) und FAIL (anderer Prozess, kein Capability-Inhaber). */
    uint32_t cap_test_pid = process_create(0u, 0xCAFEF00Du /* synthetisches Test-cr3 */);
    if (cap_test_pid == 0) return -1;
    uint32_t other_pid = process_create(0u, 0xDEADF00Du);
    if (other_pid == 0) {
        process_terminate(cap_test_pid, 0u);
        process_reap(cap_test_pid);
        return -1;
    }

    if (syscall_shutdown_authorized(0xCAFEF00Du) != 0) goto cap_test_fail; /* noch keine Vergabe */

    if (syscall_grant_shutdown_capability(cap_test_pid) != 0) goto cap_test_fail;
    if (syscall_shutdown_authorized(0xCAFEF00Du) != 1) goto cap_test_fail; /* Inhaber */
    if (syscall_shutdown_authorized(0xDEADF00Du) != 0) goto cap_test_fail; /* anderer Prozess */
    if (syscall_shutdown_authorized(0x12345678u) != 0) goto cap_test_fail; /* unbekanntes cr3 */

    process_terminate(cap_test_pid, 0u);
    process_reap(cap_test_pid);
    process_terminate(other_pid, 0u);
    process_reap(other_pid);
    syscall_shutdown_authorized_pid = 0; /* Testvergabe zuruecknehmen */
    syscall_shutdown_cap_id = 0;

    serial_puts("NOVA: System Call Interface Selbsttest OK (7/7)\r\n");
    return 0;

cap_test_fail:
    process_terminate(cap_test_pid, 0u);
    process_reap(cap_test_pid);
    process_terminate(other_pid, 0u);
    process_reap(other_pid);
    syscall_shutdown_authorized_pid = 0;
    syscall_shutdown_cap_id = 0;
    return -1;
}
