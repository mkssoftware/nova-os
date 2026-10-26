/* NovaOS C-Kernel - Power Management (Port aus power32.inc,
   NPSPEC-POWER-ARCH-0001 und verwandte NPSPEC-POWER-Spezifikationen). */
#ifndef NOVA_POWER_H
#define NOVA_POWER_H

#include <stdint.h>

#define CPUIDLE_C0  0u
#define CPUIDLE_C1  1u
#define CPUIDLE_C1E 2u
#define CPUIDLE_C3  3u
#define CPUIDLE_C6  4u
#define CPUIDLE_C7  5u

#define PSTATE_MAX       0u
#define PSTATE_TURBO     1u
#define PSTATE_NOMINAL   2u
#define PSTATE_EFFICIENT 3u
#define PSTATE_MIN       4u

#define DVFS_LEVEL_HIGH      0u
#define DVFS_LEVEL_MEDIUM    1u
#define DVFS_LEVEL_LOW       2u
#define DVFS_LEVEL_ULTRA_LOW 3u

#define DEV_PM_D0     0u
#define DEV_PM_D1     1u
#define DEV_PM_D2     2u
#define DEV_PM_D3HOT  3u
#define DEV_PM_D3COLD 4u

#define PLAT_S0   0u
#define PLAT_S0IX 1u
#define PLAT_S3   3u
#define PLAT_S4   4u
#define PLAT_S5   5u

#define CPU_CORE_BIG    0u
#define CPU_CORE_LITTLE 1u

#define WAKE_SRC_RTC     0x01u
#define WAKE_SRC_USB     0x02u
#define WAKE_SRC_NETWORK 0x04u
#define WAKE_SRC_KBD     0x08u
#define WAKE_SRC_PCIE    0x10u
#define WAKE_SRC_DISPLAY 0x20u

#define CPU_CORE_CAPACITY 8u
#define DEV_PWR_CAPACITY  8u

/* Registriert zusaetzlich den virtuellen CPU-Core 0 (CPU_CORE_BIG). */
void power_extended_initialize(void);
int power_extended_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int power_register_cpu_core(uint32_t core_id, uint32_t type);

/* 0 = Erfolg (Frequenz wird je PSTATE_* vereinfacht gesetzt), -1 wenn
   core_id unbekannt. */
int power_set_pstate(uint32_t core_id, uint32_t pstate);

/* Setzt den Idle-Zustand; fuehrt bei CPUIDLE_C1+ ein echtes HLT aus - NICHT
   waehrend Selbsttests aufrufen, solange Interrupts nicht aktiv sind. */
void power_enter_idle(uint32_t cstate);

/* 0 = Erfolg, -1 wenn Tabelle voll. */
int power_device_register(uint32_t dev_id, uint32_t budget_mws, uint32_t wake_mask);

/* 0 = Erfolg, -1 wenn dev_id unbekannt. */
int power_device_set_state(uint32_t dev_id, uint32_t pm_state);

/* Bucht Energie; erhoeht bei Budget-Ueberschreitung die DVFS-Drosselung. Immer 0. */
int power_energy_account(int32_t mws_delta);

/* Setzt den Plattform-Zustand (S3/S4/S0ix als vereinfachte Stubs). */
void power_set_platform_state(uint32_t plat_state);

#endif
