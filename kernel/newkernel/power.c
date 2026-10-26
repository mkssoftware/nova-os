/* NovaOS C-Kernel - Power Management (Port aus power32.inc). */
#include "power.h"
#include "powermgr.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t pstate;
    uint32_t cstate;
    uint32_t freq_khz;
    uint32_t volt_mv;
    uint32_t energy_mws;
    uint32_t idle_ticks;
} power_cpu_core_t;

typedef struct {
    uint32_t device_id;
    uint32_t pm_state;
    uint32_t budget_mws;
    uint32_t used_mws;
    uint32_t wake_mask;
    uint32_t runtime_autosuspend;
    uint32_t last_access;
} power_dev_t;

static power_cpu_core_t power_cpu_cores[CPU_CORE_CAPACITY];
static power_dev_t power_dev_table[DEV_PWR_CAPACITY];
static uint32_t power_platform_state = PLAT_S0;
static uint32_t power_dvfs_level = DVFS_LEVEL_MEDIUM;
static int32_t power_energy_total_mws = 0;
static int32_t power_budget_mws = 100000;
static uint32_t power_cpu_count = 0;
static uint32_t power_dev_count = 0;
static int power_ext_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

int power_register_cpu_core(uint32_t core_id, uint32_t type) {
    if (power_cpu_count >= CPU_CORE_CAPACITY) {
        return -1;
    }
    power_cpu_core_t *core = &power_cpu_cores[power_cpu_count];
    core->id = core_id;
    core->type = type;
    core->pstate = PSTATE_NOMINAL;
    core->cstate = CPUIDLE_C0;
    core->freq_khz = 2000000; /* 2 GHz Default */
    core->volt_mv = 1000;     /* 1V Default */
    core->energy_mws = 0;
    core->idle_ticks = 0;
    ++power_cpu_count;
    return 0;
}

void power_extended_initialize(void) {
    for (uint32_t i = 0; i < CPU_CORE_CAPACITY; ++i) {
        power_cpu_cores[i] = (power_cpu_core_t){0};
    }
    for (uint32_t i = 0; i < DEV_PWR_CAPACITY; ++i) {
        power_dev_table[i] = (power_dev_t){0};
    }

    power_platform_state = PLAT_S0;
    power_dvfs_level = DVFS_LEVEL_MEDIUM;
    power_energy_total_mws = 0;
    power_budget_mws = 100000; /* 100 Watt-Sekunden Default */
    power_cpu_count = 0;
    power_dev_count = 0;

    power_register_cpu_core(0, CPU_CORE_BIG);

    power_ext_ready = 1;
}

static power_cpu_core_t *power_find_core(uint32_t core_id) {
    for (uint32_t i = 0; i < power_cpu_count; ++i) {
        if (power_cpu_cores[i].id == core_id) {
            return &power_cpu_cores[i];
        }
    }
    return 0;
}

int power_set_pstate(uint32_t core_id, uint32_t pstate) {
    power_cpu_core_t *core = power_find_core(core_id);
    if (!core) {
        return -1;
    }
    core->pstate = pstate;

    /* Frequenz je nach P-State (vereinfacht). */
    switch (pstate) {
    case PSTATE_TURBO: core->freq_khz = 4000000; break;
    case PSTATE_MAX: core->freq_khz = 3500000; break;
    case PSTATE_EFFICIENT: core->freq_khz = 1200000; break;
    case PSTATE_MIN: core->freq_khz = 400000; break;
    default: core->freq_khz = 2000000; break; /* nominal */
    }
    return 0;
}

void power_enter_idle(uint32_t cstate) {
    if (cstate == CPUIDLE_C0) {
        return;
    }
    if (cstate >= CPUIDLE_C1) {
        __asm__ volatile ("hlt");
    }
}

int power_device_register(uint32_t dev_id, uint32_t budget_mws, uint32_t wake_mask) {
    if (power_dev_count >= DEV_PWR_CAPACITY) {
        return -1;
    }
    power_dev_t *dev = &power_dev_table[power_dev_count];
    dev->device_id = dev_id;
    dev->pm_state = DEV_PM_D0;
    dev->budget_mws = budget_mws;
    dev->used_mws = 0;
    dev->wake_mask = wake_mask;
    dev->runtime_autosuspend = 500; /* 5s bei 100Hz */
    dev->last_access = current_timer_ticks();
    ++power_dev_count;
    return 0;
}

int power_device_set_state(uint32_t dev_id, uint32_t pm_state) {
    for (uint32_t i = 0; i < power_dev_count; ++i) {
        if (power_dev_table[i].device_id == dev_id) {
            power_dev_table[i].pm_state = pm_state;
            return 0;
        }
    }
    return -1;
}

int power_energy_account(int32_t mws_delta) {
    power_energy_total_mws += mws_delta;

    if (power_energy_total_mws < power_budget_mws) {
        return 0;
    }
    /* Budget ueberschritten: Drosselung erhoehen. */
    if (power_dvfs_level != DVFS_LEVEL_ULTRA_LOW) {
        ++power_dvfs_level;
    }
    return 0;
}

void power_set_platform_state(uint32_t plat_state) {
    power_platform_state = plat_state;

    if (plat_state == PLAT_S3) {
        /* S3: Inhalt in RAM sichern, Strom halten. Stub: nur Status setzen. */
        return;
    }
    if (plat_state == PLAT_S4) {
        /* S4: Inhalt auf Storage schreiben. Stub: Shutdown anfordern. */
        power_manager_request_shutdown();
        return;
    }
    if (plat_state == PLAT_S0IX) {
        power_enter_idle(CPUIDLE_C6);
    }
}

int power_extended_self_test(void) {
    if (power_ext_ready != 1) return -1;
    if (power_cpu_count == 0) return -1;

    if (power_set_pstate(0, PSTATE_EFFICIENT) != 0) return -1;
    if (power_energy_account(100) != 0) return -1;
    if (power_device_register(0xFFu, 5000, WAKE_SRC_USB) != 0) return -1;

    return 0;
}
