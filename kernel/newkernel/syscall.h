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
