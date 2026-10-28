/* NovaOS C-Kernel - Nova Observability Framework (Port aus
   observability32.inc): §104 Kernel Diagnostics Framework, Trace Ring,
   Performance Counters, Structured Log Ring, Subsystem Introspection
   Registry, Tick-Based Sampling Profiler.

   WICHTIG: wd_global_tick (Watchdog-Subsystem, resilience32.inc, noch
   nicht portiert) wird ueberall durch das bereits real existierende
   timer_get_ticks() (interrupt.h) ersetzt - das ist keine Vereinfachung,
   sondern ein ECHTER Tick-Zaehler statt eines kuenftigen Platzhalters,
   also strikt eine Verbesserung gegenueber dem alten ASM-Aufbau. */
#ifndef NOVA_OBSERVABILITY_H
#define NOVA_OBSERVABILITY_H

#include <stdint.h>

/* --- §104 Diagnostics Framework ------------------------------------- */
#define DIAG_LEVEL_TRACE    0u
#define DIAG_LEVEL_DEBUG    1u
#define DIAG_LEVEL_INFO     2u
#define DIAG_LEVEL_NOTICE   3u
#define DIAG_LEVEL_WARNING  4u
#define DIAG_LEVEL_ERROR    5u
#define DIAG_LEVEL_CRITICAL 6u
#define DIAG_LEVEL_FATAL    7u

#define DIAG_HEALTH_UNKNOWN    0u
#define DIAG_HEALTH_HEALTHY    1u
#define DIAG_HEALTH_DEGRADED   2u
#define DIAG_HEALTH_UNHEALTHY  3u
#define DIAG_HEALTH_FAILED     4u
#define DIAG_HEALTH_RECOVERING 5u
#define DIAG_HEALTH_OFFLINE    6u

#define DIAG_SRC_KERNEL 1u
#define DIAG_SRC_KOG    2u
#define DIAG_SRC_EVBUS  3u
#define DIAG_SRC_UOBJ   4u
#define DIAG_SRC_CAP    5u
#define DIAG_SRC_DIAG   6u

void diag_manager_initialize(void);
int diag_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn src_id bereits registriert oder Tabelle voll. */
int diag_register_source(uint32_t src_id, uint32_t flags, uint32_t default_level);

/* 0 = Erfolg (Ringpuffer-Eintrag geschrieben), -1 wenn Quelle unbekannt
   oder log_level < der default_level der Quelle (gefiltert). */
int diag_log(uint32_t src_id, uint32_t log_level, uint32_t event_id, uint32_t value);

/* 0 = Erfolg, -1 wenn Quelle unbekannt oder Health-Tabelle voll. Publiziert
   NP_EVENT_DIAG_HEALTH_CHANGED ueber evbus.h. */
int diag_health_report(uint32_t src_id, uint32_t health_state);
/* 0 = Erfolg (*out_state gesetzt), -1 wenn kein Health-Eintrag vorliegt. */
int diag_health_get(uint32_t src_id, uint32_t *out_state);

/* --- Trace Ring (NPSPEC-OBSERVABILITY-TRACING-0001) ------------------ */
#define TRACE_SYS_KERNEL 1u
#define TRACE_SYS_IRQ    2u
#define TRACE_SYS_SCHED  3u
#define TRACE_SYS_MEM    4u
#define TRACE_SYS_IPC    5u

void trace_initialize(void);
int trace_self_test(void); /* 0 = ok */
/* Ueberschreibt den aeltesten Eintrag, wenn der Ring (64 Slots) voll ist. */
void trace_emit(uint32_t subsystem, uint32_t event_id, uint32_t arg_a, uint32_t arg_b);

/* --- Performance Counters (NPSPEC-OBSERVABILITY-METRICS-0001) ------- */
#define MET_SLOT_IRQ_COUNT  0u
#define MET_SLOT_CTX_SWITCH 1u
#define MET_SLOT_FAULTS     2u
#define MET_SLOT_ALLOCS     3u
#define MET_SLOT_FREES      4u
#define MET_SLOT_IPC_CALLS  5u
#define MET_SLOT_TRACE_EMIT 6u
#define MET_SLOT_WD_KICKS   7u

void metrics_initialize(void);
int metrics_self_test(void); /* 0 = ok */
void metrics_increment(uint32_t slot);
void metrics_add(uint32_t slot, uint32_t delta);
/* 0 = Erfolg (*out_* gesetzt), -1 wenn slot ausserhalb des Bereichs. */
int metrics_read(uint32_t slot, uint32_t *out_value, uint32_t *out_peak, uint32_t *out_total);
void metrics_reset(uint32_t slot); /* setzt nur value, peak/total bleiben */

/* --- Structured Log Ring (NPSPEC-OBSERVABILITY-LOGGING-0001) -------- */
#define OBSLOG_SEV_DEBUG 0u
#define OBSLOG_SEV_INFO  1u
#define OBSLOG_SEV_WARN  2u
#define OBSLOG_SEV_ERROR 3u
#define OBSLOG_SEV_CRIT  4u

void obslog_initialize(void);
int obslog_self_test(void); /* 0 = ok */
/* 0 = Erfolg, -1 wenn severity ungueltig oder Ring nicht bereit. */
int obslog_write(uint32_t severity, uint32_t component, uint32_t event_type, uint32_t msg_id);
/* 0 = Erfolg (*out_count gesetzt), -1 wenn Ring nicht bereit. WICHTIG
   (1:1 aus dem alten ASM uebernommene Eigenheit): ein Record mit
   Sequenznummer 0 - das ist IMMER der allererste je geschriebene Eintrag -
   wird wie ein leerer Slot behandelt und daher von der Zaehlung
   ausgeschlossen. */
int obslog_count(uint32_t severity, uint32_t *out_count);

/* --- Subsystem Introspection Registry --------------------------------
   (NPSPEC-OBSERVABILITY-INTROSPECTION-0001) */
#define INTRO_SYS_KERNEL     1u
#define INTRO_SYS_SCHED      2u
#define INTRO_SYS_MEMORY     3u
#define INTRO_SYS_SECURITY   4u
#define INTRO_SYS_RESILIENCE 5u

/* query_key=0 ist der Basisstatus-Abfrageschluessel. Rueckgabe = Anzahl
   geschriebener Bytes in out_buf (0 = query_key nicht unterstuetzt). */
typedef uint32_t (*intro_query_fn_t)(uint8_t *out_buf, uint32_t query_key);

void intro_manager_initialize(void);
int intro_self_test(void); /* 0 = ok */
/* 0 = Erfolg (*out_slot gesetzt), -1 wenn Tabelle voll. */
int intro_register(uint32_t subsystem_id, intro_query_fn_t fn, uint32_t *out_slot);
/* 0 = Erfolg (*out_slot gesetzt), -1 wenn kein Provider fuer subsystem_id. */
int intro_query(uint32_t subsystem_id, uint32_t *out_slot);
/* 0 = Erfolg (*out_bytes gesetzt), -1 wenn kein Provider oder fn==NULL. */
int intro_invoke(uint32_t subsystem_id, uint8_t *out_buf, uint32_t query_key,
                  uint32_t *out_bytes);

/* --- Tick-Based Sampling Profiler ------------------------------------
   (NPSPEC-OBSERVABILITY-PROFILING-0001) */
void prof_initialize(void);
int prof_self_test(void); /* 0 = ok */
void prof_sample(uint32_t component_id);
/* 0 = Erfolg (*out_component_id und *out_count gesetzt), -1 wenn keine
   Samples vorliegen (auch nach prof_reset()). */
int prof_hottest(uint32_t *out_component_id, uint32_t *out_count);
void prof_reset(void); /* loescht alle Samples, NICHT prof_total */

#endif
