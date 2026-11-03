/* NovaOS C-Kernel - Execution-Engine (Port aus execution32.inc, NPSPEC-EXECUTION-*). */
#ifndef NOVA_EXECUTION_H
#define NOVA_EXECUTION_H

#include <stdint.h>

#define EXEC_PROV_CPU    0u
#define EXEC_PROV_GPU    1u
#define EXEC_PROV_NPU    2u
#define EXEC_PROV_DSP    3u
#define EXEC_PROV_REMOTE 4u

#define EXEC_FLAG_DETERMINISTIC 0x01u
#define EXEC_FLAG_DEADLINE      0x02u
#define EXEC_FLAG_ENERGY_CAP    0x04u
#define EXEC_FLAG_SOVEREIGN     0x08u
#define EXEC_FLAG_TRUSTED       0x10u

#define EXEC_CTR_CAPACITY  8u
#define EXEC_PROV_CAPACITY 4u

void execution_initialize(void);
int execution_self_test(void); /* 0 = ok, registriert+prueft jetzt einen echten Vertrag */

/* 0 = Erfolg, *out_contract_id erhaelt die ID; -1 wenn Tabelle voll.
   Ruft exec_register_contract_ex() mit Standardwerten auf (siehe dort). */
int exec_register_contract(uint32_t provider, uint32_t flags, uint32_t deadline_ticks,
                            uint32_t cpu_budget, uint32_t *out_contract_id);

/* NPSPEC-EXECUTION-RESOURCEBUDGET/-ENERGY/-ALGORITHM/-SEMANTICTYPES/-TRUST:
   das Datenmodell (exec_contract_t) hatte diese Felder schon immer, aber
   nur exec_register_contract() (mit fest verdrahteten Default-Werten) war
   oeffentlich - Aufrufer konnten sie nie selbst setzen. 0 = Erfolg,
   *out_contract_id erhaelt die ID; -1 wenn Tabelle voll. */
int exec_register_contract_ex(uint32_t provider, uint32_t flags, uint32_t deadline_ticks,
                               uint32_t cpu_budget, uint32_t mem_budget,
                               uint32_t energy_budget, uint32_t algo_id,
                               uint32_t sem_type, uint32_t trust_level,
                               uint32_t *out_contract_id);

typedef struct {
    uint32_t mem_budget;
    uint32_t energy_budget;
    uint32_t algo_id;
    uint32_t sem_type;
    uint32_t trust_level;
} exec_contract_info_t;

/* Macht die bisher unerreichbaren Vertragsfelder introspizierbar. 0 =
   Erfolg (*out gefuellt), -1 wenn contract_id unbekannt. */
int exec_contract_info(uint32_t contract_id, exec_contract_info_t *out);

/* 0 = innerhalb Deadline (oder kein EXEC_FLAG_DEADLINE gesetzt),
   -1 = Deadline verpasst oder contract_id unbekannt. */
int exec_check_deadline(uint32_t contract_id);

#endif
