/* NovaOS C-Kernel - Power Manager ABI 1.0 (Port aus integration32.inc,
   NPSPEC-KERNEL-0021). System-weite Power-States (Running/Idle/Shutdown/
   Restart/Off), Wake-Locks, CPU-Idle. NICHT zu verwechseln mit power.c
   (Nova Power Management aus power32.inc: CPU-Idle-C-States/P-States/DVFS/
   Geraete-Power - ein anderes, schon im alten ASM eigenstaendiges
   Subsystem). Loest das `power_manager_request_shutdown`-TODO in power.c
   und thermal.c auf. */
#ifndef NOVA_POWERMGR_H
#define NOVA_POWERMGR_H

#include <stdint.h>

#define POWER_STATE_RUNNING         0u
#define POWER_STATE_IDLE            1u
#define POWER_STATE_SUSPEND_TO_IDLE 2u
#define POWER_STATE_SUSPEND_TO_RAM  3u
#define POWER_STATE_HIBERNATE       4u
#define POWER_STATE_HYBRID_SLEEP    5u
#define POWER_STATE_SHUTDOWN        6u
#define POWER_STATE_RESTART         7u
#define POWER_STATE_OFF             8u

#define POWER_PROFILE_PERFORMANCE 0u
#define POWER_PROFILE_BALANCED    1u
#define POWER_PROFILE_EFFICIENCY  2u
#define POWER_PROFILE_POWERSAVE   3u

#define POWER_PHASE_NONE             0u
#define POWER_PHASE_REQUEST          1u
#define POWER_PHASE_FREEZE_USERSPACE 2u
#define POWER_PHASE_SYNC_STORAGE     3u
#define POWER_PHASE_STOP_DEVICES     4u
#define POWER_PHASE_PLATFORM_OFF     5u

#define POWER_WAKE_LOCK_CAPACITY 2u

#define POWER_IDLE_SHALLOW 0u
#define POWER_IDLE_DEEP    1u

void power_manager_initialize(void);
int power_manager_self_test(void); /* 0 = ok */

/* -1 wenn bereits ein globaler Energieuebergang aktiv ist, sonst 0.
   Durchlaeuft REQUEST/FREEZE/SYNC/STOP-Phasen und setzt den Zustand auf
   SHUTDOWN (nur Statusuebergang, noch kein echter Poweroff). */
int power_manager_request_shutdown(void);

/* ECHTER Poweroff via QEMU/Bochs ACPI-Ports (0x604/0xB004) - bewusst NICHT
   im Selbsttest aufgerufen. */
void power_manager_platform_off(void);

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn voll oder duration_ticks==0. */
int power_wake_lock_acquire(uint32_t duration_ticks, uint32_t *out_slot);
int power_wake_lock_release(uint32_t slot); /* 0 = Erfolg, -1 wenn ungueltig/schon frei */
void power_wake_lock_expire(void); /* raeumt abgelaufene Wake-Locks auf */

/* Fuehrt bei leeren Wake-Locks ein echtes HLT im DEEP-Idle-Zustand aus,
   sonst SHALLOW. Bewusst NICHT im Selbsttest aufgerufen. */
void power_cpu_idle(void);

/* 0 = Erfolg, -1 wenn Profil ungueltig oder Uebergang aktiv. */
int power_manager_set_profile(uint32_t profile);

#endif
