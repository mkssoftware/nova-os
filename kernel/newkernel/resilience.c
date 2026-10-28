/* NovaOS C-Kernel - Nova Resilience Framework (siehe resilience.h fuer
   Scope). */
#include "resilience.h"
#include "panic.h"

static uint32_t wd_global_tick = 0;

void resilience_tick(void) {
    ++wd_global_tick;
}

uint32_t resilience_global_tick(void) {
    return wd_global_tick;
}

/* ===================================================================== */
/* Watchdog Timer                                                        */
/* ===================================================================== */

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t owner;
    uint32_t timeout_ticks;
    uint32_t last_kick;
    uint32_t action;
    uint32_t expire_count;
} wd_record_t;

static wd_record_t wd_table[WD_CAPACITY];
static uint32_t wd_next_id = 0;
static uint32_t wd_total_expires = 0;

void watchdog_manager_initialize(void) {
    for (uint32_t i = 0; i < WD_CAPACITY; ++i) wd_table[i] = (wd_record_t){0};
    wd_next_id = 0;
    wd_total_expires = 0;
}

int watchdog_create(uint32_t owner_pid, uint32_t timeout_ticks, uint32_t action,
                     uint32_t *out_slot) {
    for (uint32_t i = 0; i < WD_CAPACITY; ++i) {
        if (wd_table[i].state == WD_STATE_FREE) {
            wd_table[i].id = wd_next_id++;
            wd_table[i].state = WD_STATE_DISARMED;
            wd_table[i].owner = owner_pid;
            wd_table[i].timeout_ticks = timeout_ticks;
            wd_table[i].action = action;
            wd_table[i].last_kick = 0;
            wd_table[i].expire_count = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int watchdog_arm(uint32_t slot) {
    if (slot >= WD_CAPACITY) return -1;
    if (wd_table[slot].state != WD_STATE_DISARMED) return -1;
    wd_table[slot].last_kick = wd_global_tick;
    wd_table[slot].state = WD_STATE_ARMED;
    return 0;
}

int watchdog_kick(uint32_t slot) {
    if (slot >= WD_CAPACITY) return -1;
    if (wd_table[slot].state != WD_STATE_ARMED) return -1;
    wd_table[slot].last_kick = wd_global_tick;
    return 0;
}

int watchdog_disarm(uint32_t slot) {
    if (slot >= WD_CAPACITY) return -1;
    wd_table[slot].state = WD_STATE_DISARMED;
    return 0;
}

void watchdog_check(void) {
    for (uint32_t i = 0; i < WD_CAPACITY; ++i) {
        if (wd_table[i].state != WD_STATE_ARMED) continue;
        if (wd_global_tick - wd_table[i].last_kick < wd_table[i].timeout_ticks) continue;

        ++wd_total_expires;
        ++wd_table[i].expire_count;
        wd_table[i].state = WD_STATE_EXPIRED;

        if (wd_table[i].action == WD_ACTION_PANIC) {
            kernel_panic(0x3033u, wd_table[i].owner, "Watchdog expired");
        }
    }
}

uint32_t watchdog_state(uint32_t slot) {
    if (slot >= WD_CAPACITY) return WD_STATE_FREE;
    return wd_table[slot].state;
}

uint32_t watchdog_total_expires(void) {
    return wd_total_expires;
}

int watchdog_self_test(void) {
    uint32_t slot;
    if (watchdog_create(7u, 50u, WD_ACTION_LOG, &slot) != 0) return -1;
    if (watchdog_arm(slot) != 0) return -1;
    if (watchdog_state(slot) != WD_STATE_ARMED) return -1;
    if (watchdog_kick(slot) != 0) return -1;

    wd_table[slot].last_kick = 0;
    wd_global_tick = 200;
    watchdog_check();
    if (watchdog_state(slot) != WD_STATE_EXPIRED) return -1;

    if (watchdog_disarm(slot) != 0) return -1;
    if (watchdog_state(slot) != WD_STATE_DISARMED) return -1;

    wd_global_tick = 0;
    return 0;
}

/* ===================================================================== */
/* Circuit Breaker                                                       */
/* ===================================================================== */

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t owner;
    uint32_t fail_count;
    uint32_t fail_threshold;
    uint32_t last_fail_tick;
    uint32_t cooldown_ticks;
    uint32_t success_count;
} cb_record_t;

static cb_record_t cb_table[CB_CAPACITY];
static uint32_t cb_next_id = 0;
static uint32_t cb_trips = 0;

void circuitbreaker_manager_initialize(void) {
    for (uint32_t i = 0; i < CB_CAPACITY; ++i) cb_table[i] = (cb_record_t){0};
    cb_next_id = 0;
    cb_trips = 0;
}

int circuitbreaker_create(uint32_t owner, uint32_t fail_threshold, uint32_t cooldown_ticks,
                           uint32_t *out_slot) {
    for (uint32_t i = 0; i < CB_CAPACITY; ++i) {
        if (cb_table[i].state == CB_STATE_FREE) {
            cb_table[i].id = cb_next_id++;
            cb_table[i].state = CB_STATE_CLOSED;
            cb_table[i].owner = owner;
            cb_table[i].fail_count = 0;
            cb_table[i].fail_threshold = fail_threshold;
            cb_table[i].last_fail_tick = 0;
            cb_table[i].cooldown_ticks = cooldown_ticks;
            cb_table[i].success_count = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int circuitbreaker_check(uint32_t slot) {
    if (slot >= CB_CAPACITY) return -1;
    uint32_t state = cb_table[slot].state;
    if (state == CB_STATE_OPEN || state == CB_STATE_FREE) return -1;
    return 0;
}

void circuitbreaker_record_failure(uint32_t slot) {
    if (slot >= CB_CAPACITY) return;
    cb_record_t *cb = &cb_table[slot];
    if (cb->state == CB_STATE_FREE) return;

    ++cb->fail_count;
    cb->last_fail_tick = wd_global_tick;

    if (cb->state == CB_STATE_HALFOPEN) {
        cb->state = CB_STATE_OPEN;
        cb->success_count = 0;
        ++cb_trips;
        return;
    }
    if (cb->fail_count >= cb->fail_threshold) {
        cb->state = CB_STATE_OPEN;
        ++cb_trips;
    }
}

void circuitbreaker_record_success(uint32_t slot) {
    if (slot >= CB_CAPACITY) return;
    cb_record_t *cb = &cb_table[slot];
    if (cb->state != CB_STATE_HALFOPEN) return;

    ++cb->success_count;
    cb->state = CB_STATE_CLOSED;
    cb->fail_count = 0;
}

void circuitbreaker_poll(void) {
    for (uint32_t i = 0; i < CB_CAPACITY; ++i) {
        if (cb_table[i].state != CB_STATE_OPEN) continue;
        if (wd_global_tick - cb_table[i].last_fail_tick < cb_table[i].cooldown_ticks) continue;

        cb_table[i].state = CB_STATE_HALFOPEN;
        cb_table[i].success_count = 0;
    }
}

uint32_t circuitbreaker_state(uint32_t slot) {
    if (slot >= CB_CAPACITY) return CB_STATE_FREE;
    return cb_table[slot].state;
}

int circuitbreaker_self_test(void) {
    uint32_t slot;
    if (circuitbreaker_create(1u, 3u, 10u, &slot) != 0) return -1;
    if (circuitbreaker_check(slot) != 0) return -1;

    circuitbreaker_record_failure(slot);
    circuitbreaker_record_failure(slot);
    circuitbreaker_record_failure(slot);
    if (circuitbreaker_state(slot) != CB_STATE_OPEN) return -1;
    if (circuitbreaker_check(slot) == 0) return -1;

    cb_table[slot].last_fail_tick = 0;
    wd_global_tick = 100;
    circuitbreaker_poll();
    if (circuitbreaker_state(slot) != CB_STATE_HALFOPEN) return -1;

    circuitbreaker_record_success(slot);
    if (circuitbreaker_state(slot) != CB_STATE_CLOSED) return -1;

    wd_global_tick = 0;
    return 0;
}

/* ===================================================================== */
/* Fault Detection                                                       */
/* ===================================================================== */

#define FD_CAPACITY 32u

typedef struct {
    uint32_t seq;
    uint32_t type;
    uint32_t subsystem;
    uint32_t code;
    uint32_t tick;
} fd_record_t;

static fd_record_t fd_log[FD_CAPACITY];
static uint32_t fd_write_pos = 0;
static uint32_t fd_seq = 0;
static uint32_t fd_total = 0;

void detection_initialize(void) {
    for (uint32_t i = 0; i < FD_CAPACITY; ++i) fd_log[i] = (fd_record_t){0};
    fd_write_pos = 0;
    fd_seq = 0;
    fd_total = 0;
}

void detection_record_fault(uint32_t fault_type, uint32_t subsystem, uint32_t fault_code) {
    fd_record_t *rec = &fd_log[fd_write_pos];
    rec->seq = fd_seq++;
    rec->type = fault_type;
    rec->subsystem = subsystem;
    rec->code = fault_code;
    rec->tick = wd_global_tick;

    fd_write_pos = (fd_write_pos + 1u) % FD_CAPACITY;
    ++fd_total;
}

uint32_t detection_get_fault_count(uint32_t fault_type) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < FD_CAPACITY; ++i) {
        if (fd_log[i].type == FD_TYPE_NONE) continue;
        if (fd_log[i].type == fault_type) ++count;
    }
    return count;
}

int detection_self_test(void) {
    detection_record_fault(FD_TYPE_MEMORY, 1u, 0xDEADu);
    if (fd_total != 1u) return -1;
    if (fd_log[0].type != FD_TYPE_MEMORY) return -1;
    if (detection_get_fault_count(FD_TYPE_MEMORY) < 1u) return -1;

    detection_record_fault(FD_TYPE_HARDWARE, 2u, 0xBAD0u);
    if (detection_get_fault_count(FD_TYPE_HARDWARE) != 1u) return -1;
    if (fd_total != 2u) return -1;

    return 0;
}

/* ===================================================================== */
/* Supervised Restart                                                    */
/* ===================================================================== */

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t owner;
    uint32_t restart_count;
    uint32_t max_restarts;
    uint32_t cooldown_ticks;
    uint32_t last_restart;
    uint32_t policy;
} rst_record_t;

static rst_record_t rst_table[RST_CAPACITY];
static uint32_t rst_next_id = 0;
static uint32_t rst_total_restarts = 0;

void restart_manager_initialize(void) {
    for (uint32_t i = 0; i < RST_CAPACITY; ++i) rst_table[i] = (rst_record_t){0};
    rst_next_id = 0;
    rst_total_restarts = 0;
}

int restart_register(uint32_t owner, uint32_t max_restarts, uint32_t cooldown_ticks,
                      uint32_t policy, uint32_t *out_slot) {
    for (uint32_t i = 0; i < RST_CAPACITY; ++i) {
        if (rst_table[i].state == RST_STATE_FREE) {
            rst_table[i].id = rst_next_id++;
            rst_table[i].state = RST_STATE_ACTIVE;
            rst_table[i].owner = owner;
            rst_table[i].restart_count = 0;
            rst_table[i].max_restarts = max_restarts;
            rst_table[i].cooldown_ticks = cooldown_ticks;
            rst_table[i].last_restart = 0;
            rst_table[i].policy = policy;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int restart_check_and_record(uint32_t slot) {
    if (slot >= RST_CAPACITY) return -1;
    rst_record_t *rec = &rst_table[slot];
    if (rec->state != RST_STATE_ACTIVE) return -1;

    if (rec->restart_count >= rec->max_restarts) {
        rec->state = RST_STATE_EXHAUSTED;
        return -1;
    }
    if (wd_global_tick - rec->last_restart < rec->cooldown_ticks) return -1;

    rec->last_restart = wd_global_tick;
    ++rec->restart_count;
    ++rst_total_restarts;
    return 0;
}

int restart_find_by_owner(uint32_t owner, uint32_t *out_slot) {
    for (uint32_t i = 0; i < RST_CAPACITY; ++i) {
        if (rst_table[i].state != RST_STATE_FREE && rst_table[i].owner == owner) {
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int restart_self_test(void) {
    uint32_t slot;
    if (restart_register(10u, 3u, 5u, RST_POLICY_COOLDOWN, &slot) != 0) return -1;

    wd_global_tick = 10;
    if (restart_check_and_record(slot) != 0) return -1;
    if (rst_table[slot].restart_count != 1u) return -1;

    wd_global_tick = 20;
    restart_check_and_record(slot);
    wd_global_tick = 30;
    restart_check_and_record(slot);
    if (rst_table[slot].restart_count != 3u) return -1;

    wd_global_tick = 40;
    if (restart_check_and_record(slot) == 0) return -1; /* exhausted */

    if (rst_total_restarts != 3u) return -1;

    wd_global_tick = 0;
    return 0;
}

/* ===================================================================== */
/* Resource Accounting                                                   */
/* ===================================================================== */

typedef struct {
    uint32_t id;
    int active;
    uint32_t owner;
    uint32_t cpu_used, cpu_limit;
    uint32_t mem_used, mem_limit;
    uint32_t io_used, io_limit;
} racct_record_t;

static racct_record_t racct_table[RACCT_CAPACITY];
static uint32_t racct_next_id = 0;
static uint32_t racct_violations = 0;

void resource_accounting_initialize(void) {
    for (uint32_t i = 0; i < RACCT_CAPACITY; ++i) racct_table[i] = (racct_record_t){0};
    racct_next_id = 0;
    racct_violations = 0;
}

int resource_accounting_create(uint32_t owner, uint32_t cpu_limit, uint32_t mem_limit,
                                uint32_t *out_slot) {
    for (uint32_t i = 0; i < RACCT_CAPACITY; ++i) {
        if (!racct_table[i].active) {
            racct_table[i].id = racct_next_id++;
            racct_table[i].active = 1;
            racct_table[i].owner = owner;
            racct_table[i].cpu_used = 0;
            racct_table[i].cpu_limit = cpu_limit;
            racct_table[i].mem_used = 0;
            racct_table[i].mem_limit = mem_limit;
            racct_table[i].io_used = 0;
            racct_table[i].io_limit = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int resource_accounting_charge(uint32_t slot, uint32_t cpu_delta, uint32_t mem_delta) {
    if (slot >= RACCT_CAPACITY) return -1;
    racct_record_t *rec = &racct_table[slot];
    if (!rec->active) return -1;

    if (rec->cpu_limit != 0u && rec->cpu_used + cpu_delta > rec->cpu_limit) {
        ++racct_violations;
        return -1;
    }
    if (rec->mem_limit != 0u && rec->mem_used + mem_delta > rec->mem_limit) {
        ++racct_violations;
        return -1;
    }

    rec->cpu_used += cpu_delta;
    rec->mem_used += mem_delta;
    return 0;
}

int resource_accounting_read(uint32_t slot, uint32_t *out_cpu_used, uint32_t *out_mem_used,
                              uint32_t *out_io_used) {
    if (slot >= RACCT_CAPACITY || !racct_table[slot].active) return -1;
    *out_cpu_used = racct_table[slot].cpu_used;
    *out_mem_used = racct_table[slot].mem_used;
    *out_io_used = racct_table[slot].io_used;
    return 0;
}

int resource_accounting_self_test(void) {
    uint32_t slot;
    if (resource_accounting_create(5u, 1000u, 4096u, &slot) != 0) return -1;
    if (resource_accounting_charge(slot, 500u, 1024u) != 0) return -1;

    uint32_t cpu, mem, io;
    if (resource_accounting_read(slot, &cpu, &mem, &io) != 0) return -1;
    if (cpu != 500u || mem != 1024u) return -1;

    if (resource_accounting_charge(slot, 600u, 0u) == 0) return -1; /* ueberschreitet 1000 */
    if (racct_violations != 1u) return -1;

    return 0;
}

/* ===================================================================== */
/* Failover Manager                                                      */
/* ===================================================================== */

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t primary_id, backup_id;
    uint32_t fail_tick;
    uint32_t switches;
} fo_record_t;

static fo_record_t fo_table[FO_CAPACITY];
static uint32_t fo_next_id = 0;
static uint32_t fo_total_switches = 0;

void failover_manager_initialize(void) {
    for (uint32_t i = 0; i < FO_CAPACITY; ++i) fo_table[i] = (fo_record_t){0};
    fo_next_id = 0;
    fo_total_switches = 0;
}

int failover_register(uint32_t primary_id, uint32_t backup_id, uint32_t *out_slot) {
    for (uint32_t i = 0; i < FO_CAPACITY; ++i) {
        if (fo_table[i].state == FO_STATE_FREE) {
            fo_table[i].id = fo_next_id++;
            fo_table[i].state = FO_STATE_PRIMARY_OK;
            fo_table[i].primary_id = primary_id;
            fo_table[i].backup_id = backup_id;
            fo_table[i].fail_tick = 0;
            fo_table[i].switches = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int failover_trigger(uint32_t slot) {
    if (slot >= FO_CAPACITY) return -1;
    if (fo_table[slot].state != FO_STATE_PRIMARY_OK) return -1;

    fo_table[slot].state = FO_STATE_FAILOVER;
    fo_table[slot].fail_tick = wd_global_tick;
    ++fo_table[slot].switches;
    ++fo_total_switches;
    return 0;
}

int failover_query(uint32_t slot, uint32_t *out_state, uint32_t *out_primary,
                    uint32_t *out_backup) {
    if (slot >= FO_CAPACITY || fo_table[slot].state == FO_STATE_FREE) return -1;
    *out_state = fo_table[slot].state;
    *out_primary = fo_table[slot].primary_id;
    *out_backup = fo_table[slot].backup_id;
    return 0;
}

int failover_find_by_primary(uint32_t primary_id, uint32_t *out_slot) {
    for (uint32_t i = 0; i < FO_CAPACITY; ++i) {
        if (fo_table[i].state != FO_STATE_FREE && fo_table[i].primary_id == primary_id) {
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int failover_self_test(void) {
    uint32_t slot;
    if (failover_register(10u, 20u, &slot) != 0) return -1;
    if (failover_trigger(slot) != 0) return -1;

    uint32_t state, primary, backup;
    if (failover_query(slot, &state, &primary, &backup) != 0) return -1;
    if (state != FO_STATE_FAILOVER) return -1;
    if (fo_total_switches != 1u) return -1;

    if (failover_trigger(slot) == 0) return -1; /* nicht mehr PRIMARY_OK */

    return 0;
}

/* ===================================================================== */
/* Fault Containment                                                     */
/* ===================================================================== */

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t subsystem_id;
    uint32_t fault_count;
    uint32_t threshold;
} cont_record_t;

static cont_record_t cont_table[CONT_CAPACITY];
static uint32_t cont_next_id = 0;
static uint32_t cont_isolated_count = 0;

void containment_initialize(void) {
    for (uint32_t i = 0; i < CONT_CAPACITY; ++i) cont_table[i] = (cont_record_t){0};
    cont_next_id = 0;
    cont_isolated_count = 0;
}

int containment_register(uint32_t subsystem_id, uint32_t fault_threshold, uint32_t *out_slot) {
    for (uint32_t i = 0; i < CONT_CAPACITY; ++i) {
        if (cont_table[i].state == CONT_STATE_FREE) {
            cont_table[i].id = cont_next_id++;
            cont_table[i].state = CONT_STATE_OK;
            cont_table[i].subsystem_id = subsystem_id;
            cont_table[i].fault_count = 0;
            cont_table[i].threshold = fault_threshold;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int containment_report_fault(uint32_t slot) {
    if (slot >= CONT_CAPACITY) return -1;
    cont_record_t *rec = &cont_table[slot];
    if (rec->state == CONT_STATE_FREE) return -1;
    if (rec->state == CONT_STATE_ISOLATED) return 0; /* bereits isoliert */

    ++rec->fault_count;
    if (rec->fault_count >= rec->threshold) {
        rec->state = CONT_STATE_ISOLATED;
        ++cont_isolated_count;
    } else {
        rec->state = CONT_STATE_DEGRADED;
    }
    return 0;
}

int containment_check(uint32_t slot) {
    if (slot >= CONT_CAPACITY) return -1;
    return (cont_table[slot].state == CONT_STATE_ISOLATED) ? -1 : 0;
}

int containment_find_by_subsystem(uint32_t subsystem_id, uint32_t *out_slot) {
    for (uint32_t i = 0; i < CONT_CAPACITY; ++i) {
        if (cont_table[i].state != CONT_STATE_FREE && cont_table[i].subsystem_id == subsystem_id) {
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int containment_self_test(void) {
    uint32_t slot;
    if (containment_register(1u, 2u, &slot) != 0) return -1;

    if (containment_report_fault(slot) != 0) return -1;
    if (containment_check(slot) != 0) return -1; /* noch nicht isoliert */

    if (containment_report_fault(slot) != 0) return -1;
    if (containment_check(slot) == 0) return -1; /* jetzt isoliert */

    if (cont_isolated_count != 1u) return -1;
    return 0;
}

/* ===================================================================== */
/* Checkpoint/Restore                                                    */
/* ===================================================================== */

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t owner;
    uint32_t tick;
} cp_record_t;

static cp_record_t cp_table[CP_CAPACITY];
static uint32_t cp_next_id = 0;
static uint32_t cp_total_saves = 0;
static uint32_t cp_total_restores = 0;

void checkpoint_initialize(void) {
    for (uint32_t i = 0; i < CP_CAPACITY; ++i) cp_table[i] = (cp_record_t){0};
    cp_next_id = 0;
    cp_total_saves = 0;
    cp_total_restores = 0;
}

int checkpoint_save(uint32_t owner_id, uint32_t *out_slot) {
    for (uint32_t i = 0; i < CP_CAPACITY; ++i) {
        if (cp_table[i].state == CP_STATE_FREE || cp_table[i].state == CP_STATE_RESTORED) {
            cp_table[i].id = cp_next_id++;
            cp_table[i].state = CP_STATE_VALID;
            cp_table[i].owner = owner_id;
            cp_table[i].tick = wd_global_tick;
            ++cp_total_saves;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int checkpoint_restore(uint32_t slot) {
    if (slot >= CP_CAPACITY) return -1;
    if (cp_table[slot].state != CP_STATE_VALID) return -1;

    cp_table[slot].state = CP_STATE_RESTORED;
    ++cp_total_restores;
    return 0;
}

int checkpoint_self_test(void) {
    uint32_t slot;
    if (checkpoint_save(42u, &slot) != 0) return -1;
    if (cp_total_saves != 1u) return -1;

    if (checkpoint_restore(slot) != 0) return -1;
    if (cp_total_restores != 1u) return -1;

    if (checkpoint_restore(slot) == 0) return -1; /* nicht mehr VALID */

    return 0;
}
