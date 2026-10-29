/* NovaOS C-Kernel - §011 System Call Interface (Port aus integration32.inc,
   NPSPEC-KERNEL-0011).

   WICHTIG: `sysenter_entry` (echter SYSENTER-Trampolin, baut einen zu
   isr_common identischen synthetischen Frame auf und ruft syscall_dispatch)
   und `syscall_dispatch`/`syscall_handler_query_abi` (Dispatch-Logik
   selbst, haengt an `syscall_frame`/userspace-Puffern) sind NICHT portiert:
   es gibt noch keine Ring-3-Userspace-Ausfuehrung, die SYSENTER ausloesen
   koennte, und `syscall_dispatch` ist ohnehin in io_qos32.inc definiert
   (noch nicht portiert). Deshalb werden die IA32_SYSENTER_*-MSRs auch NICHT
   programmiert (haette sonst auf eine nicht existierende Trampolin-Adresse
   gezeigt) - `syscall_sysenter_available` spiegelt nur die CPUID-Erkennung.
   `syscall_validate_user_range` kommt eigentlich aus io_qos32.inc, wird
   hier aber portiert, da `syscall_manager_self_test` sie direkt braucht. */
#ifndef NOVA_SYSCALL_H
#define NOVA_SYSCALL_H

#include <stdint.h>

/* Bootstrap-Userspace-Fenster (NPSPEC, io_qos32.inc): 0x400000-0x404000. */
#define USER_ADDRESS_MIN 0x00400000u
#define USER_ADDRESS_MAX 0x00404000u

#define SYSCALL_CALL_DEPTH_MAX 8u

#define SYSCALL_STATUS_OK        0
#define SYSCALL_STATUS_ABI       (-1)
#define SYSCALL_STATUS_SIZE      (-4)
#define SYSCALL_STATUS_SERVICE   (-8)
#define SYSCALL_STATUS_OPERATION (-9)
#define SYSCALL_STATUS_POINTER   (-15)
#define SYSCALL_STATUS_SURFACE   (-16) /* surface.c hat die Operation abgelehnt (unbekanntes Handle/ungueltiger Zustand) */

#define SYSCALL_ABI_VERSION   1u
#define SYSCALL_SERVICE_CORE  1u
#define SYSCALL_CORE_EXIT     1u
#define SYSCALL_CORE_READY    2u

/* NPSPEC-DISPLAY-ARCH-0003/0004 (Display-Protokoll/Client-Kommunikation),
   reduziert auf einen festen Satz Operationen ueber denselben int-0x80-
   Dispatch-Mechanismus wie SYSCALL_SERVICE_CORE (NICHT ueber die globale
   ipc.c-Queue - siehe surface.h fuer die Begruendung: deren Queue hat keine
   Client-Adressierung, fuer einen Request/Response-Pfad ungeeignet).
   Parameter-Uebergabe ueber zusaetzliche Trap-Frame-Register (siehe
   syscall.c): EDX = x|(y<<16), ESI = w|(h<<16) fuer CREATE_SURFACE; EDX =
   Handle fuer ATTACH_BUFFER/COMMIT/DESTROY. Rueckgabe: EAX = Status, EDI =
   neues Handle (nur bei CREATE_SURFACE). Der Pixel-Puffer liegt IMMER an
   der festen Adresse RING3_GUI_BUFFER_VADDR (ring3.h) - es existiert
   bisher nur ein Client, daher keine dynamische Pufferzuteilung. */
#define SYSCALL_SERVICE_GUI       2u
#define GUI_OP_CREATE_SURFACE     1u
#define GUI_OP_ATTACH_BUFFER      2u
#define GUI_OP_COMMIT             3u
#define GUI_OP_DESTROY            4u

/* NPSPEC-WINDOW-MANAGEMENT-0004 (Min/Maximize) + -0009 (Fullscreen), ueber
   denselben SYSCALL_SERVICE_GUI-Namespace statt eines eigenen Services
   (konsistent mit der window.c-Architekturentscheidung: ein Window ist nur
   Surface+Metadaten, kein eigenstaendiges Objekt mit eigenem Protokoll).
   EDX = Window-Handle (wie bei ATTACH/COMMIT/DESTROY), ESI = eine der
   GUI_WINDOW_STATE_*-Konstanten. */
#define GUI_OP_WINDOW_SET_STATE    5u
#define GUI_WINDOW_STATE_MINIMIZE   1u
#define GUI_WINDOW_STATE_MAXIMIZE   2u
#define GUI_WINDOW_STATE_FULLSCREEN 3u
#define GUI_WINDOW_STATE_RESTORE    4u

/* Einziger bisher existierender Client (ring3.c) - echte Mehr-Client-PID-
   Aufloesung (z.B. ueber den aktuell laufenden Scheduler-Slot) ist bewusst
   nicht in dieser Sitzung geloest. Von SYSCALL_SERVICE_GUI UND
   SYSCALL_SERVICE_VFS gemeinsam genutzt (war vorher GUI-lokal als
   GUI_SINGLE_CLIENT_PID definiert). */
#define SYSCALL_SINGLE_CLIENT_PID 1u

/* NPSPEC-FILESYSTEM-NAMESPACE-0002/-PERMISSION-0001/NPSPEC-POLICY-
   SYSTEMWRITE-0001 (Port aus vfs32.inc), ueber vfsapi.c (siehe dort fuer
   den Scope-Hinweis zur Fehlercode-Reduktion). Wire-Protokoll: EDX =
   Zeiger auf eine der syscall_vfs_*_args_t-Strukturen im Bootstrap-
   Userspace-Fenster [USER_ADDRESS_MIN, USER_ADDRESS_MAX) - anders als
   SYSCALL_SERVICE_GUI's reinen Register-Parametern, da VFS-Operationen
   variable Pfad-/Puffer-Adressen UND -Laengen gleichzeitig brauchen, die
   nicht mehr in die verbleibenden Trap-Frame-Register passen. Es gibt
   KEIN separates Bounce-Page/copy-from-user (siehe vfsapi.h) - Zeiger
   werden nach syscall_validate_user_range() direkt dereferenziert.
   Rueckgabe: EAX = 0 oder ein negativer VFSAPI_STATUS_*-Code (direkt
   durchgereicht, siehe vfsapi.h) bzw. SYSCALL_STATUS_POINTER/_OPERATION
   fuer Dispatch-eigene Fehler (ungueltiger Args-/Puffer-Zeiger,
   unbekannte Operation). *-Felder mit Out-Parametern (z.B.
   result_handle) werden direkt in der User-Struktur aktualisiert - kein
   zusaetzlicher Rueckkopier-Schritt noetig, da keine echte Bounce-Kopie
   existiert. */
#define SYSCALL_SERVICE_VFS       3u
#define VFS_OP_LOOKUP             1u
#define VFS_OP_READ               2u
#define VFS_OP_WRITE              3u
#define VFS_OP_CREATE             4u
#define VFS_OP_READ_DIRECTORY     5u
#define VFS_OP_QUERY              6u
#define VFS_OP_DELETE             7u
#define VFS_OP_RENAME             8u
#define VFS_OP_CLOSE              9u

typedef struct {
    uint32_t dir_handle;
    uint32_t path_addr;
    uint32_t path_len;
    uint32_t flags;
    uint32_t result_handle; /* out */
} syscall_vfs_lookup_args_t;

typedef struct {
    uint32_t handle;
    uint32_t buffer_addr;
    uint32_t offset;
    uint32_t length;
    uint32_t transferred; /* out */
} syscall_vfs_io_args_t;

typedef struct {
    uint32_t dir_handle;
    uint32_t name_addr;
    uint32_t name_len;
    uint32_t object_type;
    uint32_t result_handle; /* out */
} syscall_vfs_create_args_t;

typedef struct {
    uint32_t dir_handle;
    uint32_t index;
    uint32_t entry_addr; /* vfsapi_dir_entry_t* */
} syscall_vfs_read_dir_args_t;

typedef struct {
    uint32_t handle;
    uint32_t info_addr; /* vfsapi_info_t* */
} syscall_vfs_query_args_t;

typedef struct {
    uint32_t dir_handle;
    uint32_t name_addr;
    uint32_t name_len;
} syscall_vfs_delete_args_t;

typedef struct {
    uint32_t src_dir_handle;
    uint32_t src_name_addr;
    uint32_t src_name_len;
    uint32_t dst_dir_handle;
    uint32_t dst_name_addr;
    uint32_t dst_name_len;
} syscall_vfs_rename_args_t;

typedef struct {
    uint32_t handle;
} syscall_vfs_close_args_t;

/* NPSPEC-INPUT-EVENT-0001..0004 (Port aus input.h ueber eine duenne
   Dispatch-Schicht, analog zu SYSCALL_SERVICE_VFS -> vfsapi.c). EDX
   zeigt wie bei VFS auf eine Args-Struktur im Bootstrap-Userspace-Fenster
   [USER_ADDRESS_MIN, USER_ADDRESS_MAX). Es gibt nur eine Operation -
   Polling statt eines Push-Modells, da dieser Kernel keinen Mechanismus
   hat, um einen schlafenden Ring-3-Client aktiv aufzuwecken (kein
   Wait/Notify-Syscall-Aequivalent portiert) - ein Client fragt also in
   einer eigenen Schleife wiederholt ab, ob ein neues Event vorliegt.
   has_event==0 bedeutet "Warteschlange aktuell leer", nicht ein Fehler -
   EAX bleibt in diesem Fall trotzdem SYSCALL_STATUS_OK. cursor_x/y
   werden bei JEDEM Poll-Aufruf aktualisiert (auch ohne neues Event), da
   ein Client die aktuelle Zeigerposition unabhaengig von einzelnen
   MOUSE_MOVE-Deltas kennen muss. */
#define SYSCALL_SERVICE_INPUT 4u
#define INPUT_OP_POLL         1u

typedef struct {
    uint32_t has_event; /* out: 1 = Event vorhanden, 0 = Warteschlange leer */
    uint32_t type;      /* out: input_event_type_t, nur gueltig wenn has_event!=0 */
    uint32_t timestamp_ticks;
    uint32_t key_vkey;
    uint32_t modifiers;
    int32_t mouse_dx, mouse_dy;
    uint32_t mouse_buttons;
    uint32_t cursor_x, cursor_y; /* out: immer gueltig, siehe oben */
} syscall_input_poll_args_t;

void syscall_manager_initialize(void);
int syscall_manager_self_test(void); /* 0 = ok */

/* 0 = gueltig (liegt vollstaendig in [USER_ADDRESS_MIN, USER_ADDRESS_MAX],
   kein Overflow, length!=0), -1 = ungueltig. */
int syscall_validate_user_range(uint32_t addr, uint32_t length);

/* Wertet einen von interrupt_dispatch_c (Vektor 0x80) hereingereichten
   Trap-Frame aus (Layout exakt wie in interrupt_isr.asm erzeugt: EAX an
   Byte-Offset 44 = Service, EBX an 32 = Operation, ECX an 40 =
   ABI-Version). Fuer SYSCALL_SERVICE_CORE/SYSCALL_CORE_READY wird nur
   EAX (Status) gesetzt und derselbe Frame zurueckgegeben (iretd kehrt
   nach Ring 3 zurueck). Fuer SYSCALL_CORE_EXIT wird der Frame so
   umgeschrieben (EIP/CS/EFLAGS), dass iretd statt dessen nach Ring 0
   zu einer dedizierten Rueckkehrstelle wechselt - eine echte
   Privilegstufen-Rueckkehr per Frame-Rewrite (1:1 wie im alten ASM
   `syscall_dispatch`, NPSPEC-KERNEL-0011 / io_qos32.inc, reduziert auf
   SYSCALL_SERVICE_CORE). Gibt den (ggf. umgeschriebenen) Frame-Zeiger
   zurueck. */
uint32_t syscall_dispatch(uint32_t frame_ptr);

#endif
