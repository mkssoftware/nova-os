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
#define SYSCALL_STATUS_SIZE      (-4)
#define SYSCALL_STATUS_OPERATION (-9)
#define SYSCALL_STATUS_POINTER   (-15)

void syscall_manager_initialize(void);
int syscall_manager_self_test(void); /* 0 = ok */

/* 0 = gueltig (liegt vollstaendig in [USER_ADDRESS_MIN, USER_ADDRESS_MAX],
   kein Overflow, length!=0), -1 = ungueltig. */
int syscall_validate_user_range(uint32_t addr, uint32_t length);

#endif
