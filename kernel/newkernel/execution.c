/* NovaOS C-Kernel - Execution-Engine (Port aus execution32.inc). */
#include "execution.h"
#include "interrupt.h"

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

static uint32_t current_timer_ticks(void) {
    return timer_get_ticks();
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

int exec_register_contract_ex(uint32_t provider, uint32_t flags, uint32_t deadline_ticks,
                               uint32_t cpu_budget, uint32_t mem_budget,
                               uint32_t energy_budget, uint32_t algo_id,
                               uint32_t sem_type, uint32_t trust_level,
                               uint32_t *out_contract_id) {
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
    rec->mem_budget = mem_budget;
    rec->energy_budget = energy_budget;
    rec->algo_id = algo_id;
    rec->sem_type = sem_type;
    rec->trust_level = trust_level;

    if (out_contract_id) {
        *out_contract_id = exec_next_id;
    }
    ++exec_next_id;
    ++exec_ctr_count;
    return 0;
}

int exec_register_contract(uint32_t provider, uint32_t flags, uint32_t deadline_ticks,
                            uint32_t cpu_budget, uint32_t *out_contract_id) {
    return exec_register_contract_ex(provider, flags, deadline_ticks, cpu_budget,
                                      16u /* mem_budget Default */, 0u /* energy_budget */,
                                      0u /* algo_id */, 0u /* sem_type */,
                                      1u /* trust_level */, out_contract_id);
}

int exec_contract_info(uint32_t contract_id, exec_contract_info_t *out) {
    for (uint32_t i = 0; i < exec_ctr_count; ++i) {
        exec_contract_t *rec = &exec_ctr_table[i];
        if (rec->id != contract_id) {
            continue;
        }
        if (out) {
            out->mem_budget = rec->mem_budget;
            out->energy_budget = rec->energy_budget;
            out->algo_id = rec->algo_id;
            out->sem_type = rec->sem_type;
            out->trust_level = rec->trust_level;
        }
        return 0;
    }
    return -1;
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
    if (exec_ready != 1) return -1;

    /* Vorher nur exec_ready geprueft (identisch zum alten ASM-Self-Test) -
       jetzt zusaetzlich die echten register/query/deadline-Funktionen. */
    uint32_t cid = 0;
    if (exec_register_contract_ex(EXEC_PROV_CPU, EXEC_FLAG_DEADLINE, 0u, 5u,
                                   64u, 3u, 7u, 2u, 4u, &cid) != 0) {
        return -1;
    }
    exec_contract_info_t info;
    if (exec_contract_info(cid, &info) != 0) return -1;
    if (info.mem_budget != 64u || info.energy_budget != 3u || info.algo_id != 7u ||
        info.sem_type != 2u || info.trust_level != 4u) {
        return -1;
    }

    /* deadline_ticks==0 -> EXEC_FLAG_DEADLINE greift trotzdem nicht (siehe
       exec_check_deadline: "0" bedeutet "keine Deadline gesetzt"). Hinweis:
       dieser Selbsttest laeuft VOR interrupt_enable()/timer_initialize()
       in kmain.c - current_timer_ticks() liefert hier also deterministisch
       0, ein "bereits abgelaufene Deadline"-Fall kann an dieser Boot-Stelle
       daher nicht realitaetsnah erzeugt werden (0 >= deadline_ticks>0 ist
       immer falsch) - bewusst NICHT getestet, um keine falsche Annahme
       ueber verstrichene Zeit zu machen. */
    if (exec_check_deadline(cid) != 0) return -1;

    if (exec_check_deadline(0xFFFFu) != -1) return -1; /* unbekannte ID */

    return 0;
}
