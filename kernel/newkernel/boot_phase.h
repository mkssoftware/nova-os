/* NovaOS C-Kernel - Boot-Phasen-Tracking (Port aus integration32.inc, NPSPEC-KERNEL-0002). */
#ifndef NOVA_BOOT_PHASE_H
#define NOVA_BOOT_PHASE_H

#include <stdint.h>

#define BOOT_PHASE_NONE                 0xFFFFFFFFu
#define BOOT_PHASE_KERNEL_ENTRY         0u
#define BOOT_PHASE_HANDOFF              1u
#define BOOT_PHASE_EARLY_ARCH           2u
#define BOOT_PHASE_EARLY_MEMORY         3u
#define BOOT_PHASE_VIRTUAL_MEMORY       4u
#define BOOT_PHASE_KERNEL_CORE          5u
#define BOOT_PHASE_INTERRUPTS_TIME      6u
#define BOOT_PHASE_SCHEDULER_SMP        7u
#define BOOT_PHASE_DEVICE_DISCOVERY     8u
#define BOOT_PHASE_ROOT_FILESYSTEM      9u
#define BOOT_PHASE_USERSPACE            10u
#define BOOT_PHASE_OPERATIONAL          11u
#define BOOT_PHASE_DESKTOP_READY        12u
#define BOOT_PHASE_SYSTEM_READY         13u
#define BOOT_PHASE_GRAPHICS_READY       14u
#define BOOT_PHASE_CORE_SERVICES_READY  15u

/* Wie im alten ASM: Aufrufer setzen die Phase direkt und rufen danach
   boot_phase_log(), statt ueber einen Setter zu gehen. */
extern uint32_t boot_phase_current;
extern uint32_t boot_phase_last_success;

/* Zeichnet das aktuelle Boot-Event im statischen Ring auf und gibt die
   Phase ueber die serielle Schnittstelle aus (heapfrei). */
void boot_phase_log(void);

#endif
