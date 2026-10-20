/* NovaOS C-Kernel - Execution-Engine (Port aus execution32.inc). */
#include "execution.h"

typedef struct {
    uint32_t id;
    uint32_t provider;
    uint32_t flags;
    uint32_t deadline_ticks;
    uint32_t latency_ns;
    uint32_t cpu_budget;
    uint32_t mem_budget;
    uint32_t energy_budget;
    uint32_t algo_id;
    uint32_t sem_type;
    uint32_t trust_level;
} exec_contract_t;

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t capacity;
    uint32_t used;
    uint32_t flags;
} exec_provider_t;

static exec_contract_t exec_ctr_table[EXEC_CTR_CAPACITY];
static exec_provider_t exec_prov_table[EXEC_PROV_CAPACITY];
static uint32_t exec_ctr_count = 0;
static uint32_t exec_prov_count = 0;
static uint32_t exec_next_id = 1;
static int exec_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

void execution_initialize(void) {
    for (uint32_t i = 0; i < EXEC_CTR_CAPACITY; ++i) {
        exec_ctr_table[i] = (exec_contract_t){0};
    }
    for (uint32_t i = 0; i < EXEC_PROV_CAPACITY; ++i) {
        exec_prov_table[i] = (exec_provider_t){0};
    }
    exec_ctr_count = 0;
    exec_next_id = 1;

    /* Standard-Provider: CPU */
    exec_prov_table[0].id = 1;
    exec_prov_table[0].type = EXEC_PROV_CPU;
    exec_prov_table[0].capacity = 100;
    exec_prov_table[0].used = 0;
    exec_prov_table[0].flags = EXEC_FLAG_DETERMINISTIC;
    exec_prov_count = 1;

    exec_ready = 1;
}

int exec_register_contract(uint32_t provider, uint32_t flags, uint32_t deadline_ticks,
                            uint32_t cpu_budget, uint32_t *out_contract_id) {
    if (exec_ctr_count >= EXEC_CTR_CAPACITY) {
        return -1;
    }
    exec_contract_t *rec = &exec_ctr_table[exec_ctr_count];
    rec->id = exec_next_id;
    rec->provider = provider;
    rec->flags = flags;
    rec->deadline_ticks = deadline_ticks;
    rec->cpu_budget = cpu_budget;
    rec->latency_ns = 1000000; /* Default 1ms */
    rec->mem_budget = 16;
    rec->energy_budget = 0;
    rec->algo_id = 0;
    rec->sem_type = 0;
    rec->trust_level = 1;

    if (out_contract_id) {
        *out_contract_id = exec_next_id;
    }
    ++exec_next_id;
    ++exec_ctr_count;
    return 0;
}

int exec_check_deadline(uint32_t contract_id) {
    for (uint32_t i = 0; i < exec_ctr_count; ++i) {
        exec_contract_t *rec = &exec_ctr_table[i];
        if (rec->id != contract_id) {
            continue;
        }
        if (!(rec->flags & EXEC_FLAG_DEADLINE)) {
            return 0;
        }
        if (rec->deadline_ticks == 0) {
            return 0;
        }
        return (current_timer_ticks() >= rec->deadline_ticks) ? -1 : 0;
    }
    return -1; /* nicht gefunden */
}

int execution_self_test(void) {
    return (exec_ready == 1) ? 0 : -1;
}
