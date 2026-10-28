/* NovaOS C-Kernel - Nova Resilience Framework (Port aus
   resilience32.inc): Watchdog-Timer, Circuit-Breaker, Fault-Detection,
   Supervised Restart, Resource-Accounting, Failover-Manager, Fault-
   Containment, Checkpoint/Restore.

   WICHTIG: `wd_global_tick` ist HIER (im Gegensatz zu observability.c,
   das echte `timer_get_ticks()`-Werte verwendet) ein EIGENER, von
   `resilience_tick()` verwalteter Zaehler, NICHT durch `timer_get_ticks()`
   ersetzt - die Selbsttests setzen ihn gezielt auf feste Werte, um
   Watchdog-Ablauf/Cooldown/Backoff deterministisch zu simulieren; ein
   echter Hardware-Tick-Zaehler liesse sich dafuer nicht manipulieren.
   `resilience_tick()` wird von `interrupt.c`s Timer-ISR (Vektor 32) real
   aufgerufen - der Zaehler laeuft also auch im normalen Betrieb echt mit,
   nur eben unabhaengig vom globalen `timer_get_ticks()`. */
#ifndef NOVA_RESILIENCE_H
#define NOVA_RESILIENCE_H

#include <stdint.h>

/* Von interrupt.c's Timer-ISR aufzurufen (einmal pro Tick). */
void resilience_tick(void);
uint32_t resilience_global_tick(void); /* nur fuer Diagnose/Tests */

/* --- Watchdog Timer (NPSPEC-RESILIENCE-WATCHDOG-0001) ---------------- */
#define WD_CAPACITY 4u

#define WD_STATE_FREE     0u
#define WD_STATE_DISARMED 1u
#define WD_STATE_ARMED    2u
#define WD_STATE_EXPIRED  3u

#define WD_ACTION_PANIC    1u
#define WD_ACTION_REBOOT   2u
#define WD_ACTION_LOG      3u
#define WD_ACTION_CALLBACK 4u

void watchdog_manager_initialize(void);
int watchdog_self_test(void); /* 0 = ok */
/* 0 = Erfolg (*out_slot gesetzt, Zustand -> DISARMED), -1 wenn Tabelle voll. */
int watchdog_create(uint32_t owner_pid, uint32_t timeout_ticks, uint32_t action,
                     uint32_t *out_slot);
/* 0 = Erfolg (Zustand -> ARMED), -1 wenn slot unbekannt oder nicht DISARMED. */
int watchdog_arm(uint32_t slot);
/* 0 = Erfolg (last_kick aktualisiert), -1 wenn slot unbekannt oder nicht ARMED. */
int watchdog_kick(uint32_t slot);
int watchdog_disarm(uint32_t slot); /* 0 = Erfolg, -1 wenn slot unbekannt */
/* Prueft alle ARMED Watchdogs; abgelaufene -> EXPIRED. WD_ACTION_PANIC
   ruft panic_trigger() (panic.h) auf - siehe Risiko-Hinweis in resilience.c. */
void watchdog_check(void);
uint32_t watchdog_state(uint32_t slot); /* WD_STATE_FREE wenn unbekannt */
uint32_t watchdog_total_expires(void); /* fuer integration.c's watchdog_expire_pipeline */

/* --- Circuit Breaker (NPSPEC-RESILIENCE-CIRCUITBREAKER-0001) --------- */
#define CB_CAPACITY 8u

#define CB_STATE_FREE     0u
#define CB_STATE_CLOSED   1u
#define CB_STATE_OPEN     2u
#define CB_STATE_HALFOPEN 3u

void circuitbreaker_manager_initialize(void);
int circuitbreaker_self_test(void); /* 0 = ok */
int circuitbreaker_create(uint32_t owner, uint32_t fail_threshold, uint32_t cooldown_ticks,
                           uint32_t *out_slot);
int circuitbreaker_check(uint32_t slot); /* 0 = Aufruf erlaubt, -1 = OPEN/unbekannt */
void circuitbreaker_record_failure(uint32_t slot);
void circuitbreaker_record_success(uint32_t slot);
void circuitbreaker_poll(void); /* OPEN + Cooldown abgelaufen -> HALF-OPEN */
uint32_t circuitbreaker_state(uint32_t slot);

/* --- Fault Detection (NPSPEC-RESILIENCE-DETECTION-0001) -------------- */
#define FD_TYPE_NONE     0u
#define FD_TYPE_MEMORY   1u
#define FD_TYPE_TIMING   2u
#define FD_TYPE_RESOURCE 3u
#define FD_TYPE_SUBSYS   4u
#define FD_TYPE_HARDWARE 5u
#define FD_TYPE_SECURITY 6u

void detection_initialize(void);
int detection_self_test(void); /* 0 = ok */
void detection_record_fault(uint32_t fault_type, uint32_t subsystem, uint32_t fault_code);
uint32_t detection_get_fault_count(uint32_t fault_type);

/* --- Supervised Restart (NPSPEC-RESILIENCE-RESTART-0001) ------------- */
#define RST_CAPACITY 8u

#define RST_STATE_FREE      0u
#define RST_STATE_ACTIVE    1u
#define RST_STATE_EXHAUSTED 2u

#define RST_POLICY_IMMEDIATE 0u
#define RST_POLICY_BACKOFF   1u
#define RST_POLICY_COOLDOWN  2u

void restart_manager_initialize(void);
int restart_self_test(void); /* 0 = ok */
int restart_register(uint32_t owner, uint32_t max_restarts, uint32_t cooldown_ticks,
                      uint32_t policy, uint32_t *out_slot);
/* 0 = Neustart erlaubt (aufgezeichnet), -1 = exhausted oder noch in Cooldown. */
int restart_check_and_record(uint32_t slot);
/* 0 = Erfolg (*out_slot gesetzt), -1 wenn kein Eintrag mit diesem owner
   existiert - fuer integration.c's resilience_fault_pipeline. */
int restart_find_by_owner(uint32_t owner, uint32_t *out_slot);

/* --- Resource Accounting (NPSPEC-RESOURCE-ACCOUNTING-0001) ----------- */
#define RACCT_CAPACITY 8u

void resource_accounting_initialize(void);
int resource_accounting_self_test(void); /* 0 = ok */
int resource_accounting_create(uint32_t owner, uint32_t cpu_limit, uint32_t mem_limit,
                                uint32_t *out_slot);
/* 0 = Erfolg (verbraucht), -1 wenn ein Hard-Limit ueberschritten wuerde
   (dann NICHT verbucht) oder slot unbekannt. */
int resource_accounting_charge(uint32_t slot, uint32_t cpu_delta, uint32_t mem_delta);
int resource_accounting_read(uint32_t slot, uint32_t *out_cpu_used, uint32_t *out_mem_used,
                              uint32_t *out_io_used);

/* --- Failover Manager (NPSPEC-RESILIENCE-FAILOVER-0001) -------------- */
#define FO_CAPACITY 4u

#define FO_STATE_FREE       0u
#define FO_STATE_PRIMARY_OK 1u
#define FO_STATE_FAILED     2u
#define FO_STATE_FAILOVER   3u

void failover_manager_initialize(void);
int failover_self_test(void); /* 0 = ok */
int failover_register(uint32_t primary_id, uint32_t backup_id, uint32_t *out_slot);
int failover_trigger(uint32_t slot); /* 0 = Erfolg, -1 wenn nicht PRIMARY_OK */
int failover_query(uint32_t slot, uint32_t *out_state, uint32_t *out_primary,
                    uint32_t *out_backup);
/* 0 = Erfolg (*out_slot gesetzt), -1 wenn kein Eintrag mit diesem
   primary_id existiert - fuer integration.c's resilience_fault_pipeline. */
int failover_find_by_primary(uint32_t primary_id, uint32_t *out_slot);

/* --- Fault Containment (NPSPEC-RESILIENCE-CONTAINMENT-0001) ---------- */
#define CONT_CAPACITY 8u

#define CONT_STATE_FREE     0u
#define CONT_STATE_OK       1u
#define CONT_STATE_DEGRADED 2u
#define CONT_STATE_ISOLATED 3u

void containment_initialize(void);
int containment_self_test(void); /* 0 = ok */
int containment_register(uint32_t subsystem_id, uint32_t fault_threshold, uint32_t *out_slot);
/* 0 = Erfolg (Zustand aktualisiert), -1 wenn slot unbekannt. */
int containment_report_fault(uint32_t slot);
int containment_check(uint32_t slot); /* 0 = ok, -1 = ISOLATED/unbekannt */
/* 0 = Erfolg (*out_slot gesetzt), -1 wenn kein Eintrag fuer subsystem_id
   existiert - fuer integration.c's resilience_fault_pipeline. */
int containment_find_by_subsystem(uint32_t subsystem_id, uint32_t *out_slot);

/* --- Checkpoint/Restore (NPSPEC-RESILIENCE-CHECKPOINT-0001) ---------- */
#define CP_CAPACITY 4u

#define CP_STATE_FREE     0u
#define CP_STATE_VALID    1u
#define CP_STATE_RESTORED 2u

void checkpoint_initialize(void);
int checkpoint_self_test(void); /* 0 = ok */
/* Rein buchhalterisch (Slot+Tick+Owner) - ein echtes Register-/Stack-
   Snapshot braucht Kontext, der ausserhalb des Aufrufers nicht sinnvoll
   fail-safe nachgebildet werden kann (das alte ASM sicherte nur die
   zufaellig im jeweiligen Aufrufkontext noch lebenden Register, siehe
   resilience32.inc Kommentar "EAX im Checkpoint = 0, kann nicht self-
   save" - ein bekanntermassen unvollstaendiger Mechanismus schon im
   Original). 0 = Erfolg (*out_slot gesetzt), -1 wenn Tabelle voll. */
int checkpoint_save(uint32_t owner_id, uint32_t *out_slot);
int checkpoint_restore(uint32_t slot); /* 0 = Erfolg, -1 wenn nicht VALID */

#endif
