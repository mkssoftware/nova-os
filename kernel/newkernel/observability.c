/* NovaOS C-Kernel - Nova Observability Framework (siehe observability.h
   fuer Scope). */
#include "observability.h"
#include "evbus.h"
#include "interrupt.h"
#include "boot_phase.h"
#include "pmm.h"

/* ===================================================================== */
/* §104 Diagnostics Framework                                            */
/* ===================================================================== */

#define DIAG_MAX_SOURCES     16u
#define DIAG_LOG_MAX_RECORDS 64u
#define DIAG_MAX_HEALTH      16u

/* Nach CAP-Events 9-11 (security32.inc) - lokal definiert, da evbus.h nur
   die eingebauten Event-IDs 1-8 kennt. */
#define NP_EVENT_DIAG_HEALTH_CHANGED 12u

typedef struct {
    uint32_t src_id;
    uint32_t flags;
    uint32_t default_level;
    int present;
} diag_source_t;

typedef struct {
    uint32_t level;
    uint32_t src_id;
    uint32_t event_id;
    uint32_t seq;
    uint32_t value;
} diag_log_record_t;

typedef struct {
    uint32_t src_id;
    uint32_t state;
    int present;
} diag_health_t;

static diag_source_t diag_sources[DIAG_MAX_SOURCES];
static diag_log_record_t diag_log_buf[DIAG_LOG_MAX_RECORDS];
static diag_health_t diag_health_tab[DIAG_MAX_HEALTH];
static uint32_t diag_log_write = 0;
static uint32_t diag_log_total = 0;
static uint32_t diag_log_lost = 0;
static uint32_t diag_log_seq = 0;
static uint32_t diag_stat_logged = 0;
static uint32_t diag_stat_filtered = 0;

static int diag_find_source(uint32_t src_id) {
    for (uint32_t i = 0; i < DIAG_MAX_SOURCES; ++i) {
        if (diag_sources[i].present && diag_sources[i].src_id == src_id) return (int)i;
    }
    return -1;
}

int diag_register_source(uint32_t src_id, uint32_t flags, uint32_t default_level) {
    if (diag_find_source(src_id) >= 0) return -1; /* doppelt */

    for (uint32_t i = 0; i < DIAG_MAX_SOURCES; ++i) {
        if (!diag_sources[i].present) {
            diag_sources[i].src_id = src_id;
            diag_sources[i].flags = flags;
            diag_sources[i].default_level = default_level;
            diag_sources[i].present = 1;
            return 0;
        }
    }
    return -1; /* voll */
}

int diag_log(uint32_t src_id, uint32_t log_level, uint32_t event_id, uint32_t value) {
    int i = diag_find_source(src_id);
    if (i < 0) return -1;

    if (log_level < diag_sources[i].default_level) {
        ++diag_stat_filtered;
        return -1;
    }

    if (diag_log_total >= DIAG_LOG_MAX_RECORDS) ++diag_log_lost;

    diag_log_record_t *rec = &diag_log_buf[diag_log_write];
    rec->level = log_level;
    rec->src_id = src_id;
    rec->event_id = event_id;
    rec->seq = diag_log_seq++;
    rec->value = value;

    diag_log_write = (diag_log_write + 1u) % DIAG_LOG_MAX_RECORDS;
    ++diag_log_total;
    ++diag_stat_logged;
    return 0;
}

int diag_health_report(uint32_t src_id, uint32_t health_state) {
    if (diag_find_source(src_id) < 0) return -1;

    int slot = -1;
    for (uint32_t i = 0; i < DIAG_MAX_HEALTH; ++i) {
        if (!diag_health_tab[i].present) {
            if (slot < 0) slot = (int)i;
            continue;
        }
        if (diag_health_tab[i].src_id == src_id) {
            slot = (int)i;
            break;
        }
    }
    if (slot < 0) return -1; /* voll */

    diag_health_tab[slot].src_id = src_id;
    diag_health_tab[slot].state = health_state;
    diag_health_tab[slot].present = 1;

    evbus_publish(NP_EVENT_DIAG_HEALTH_CHANGED, src_id, 0u, 0, 0u);
    return 0;
}

int diag_health_get(uint32_t src_id, uint32_t *out_state) {
    for (uint32_t i = 0; i < DIAG_MAX_HEALTH; ++i) {
        if (diag_health_tab[i].present && diag_health_tab[i].src_id == src_id) {
            *out_state = diag_health_tab[i].state;
            return 0;
        }
    }
    return -1;
}

void diag_manager_initialize(void) {
    for (uint32_t i = 0; i < DIAG_MAX_SOURCES; ++i) diag_sources[i] = (diag_source_t){0};
    for (uint32_t i = 0; i < DIAG_LOG_MAX_RECORDS; ++i) diag_log_buf[i] = (diag_log_record_t){0};
    for (uint32_t i = 0; i < DIAG_MAX_HEALTH; ++i) diag_health_tab[i] = (diag_health_t){0};
    diag_log_write = 0;
    diag_log_total = 0;
    diag_log_lost = 0;
    diag_log_seq = 0;
    diag_stat_logged = 0;
    diag_stat_filtered = 0;

    evbus_register_schema(NP_EVENT_DIAG_HEALTH_CHANGED, NP_EVENT_CLASS_STATE, 0u,
                           NP_EVENT_SYNCHRONOUS | NP_EVENT_KERNEL_ONLY);

    diag_register_source(DIAG_SRC_KERNEL, 0u, DIAG_LEVEL_INFO);
    diag_register_source(DIAG_SRC_KOG, 0u, DIAG_LEVEL_INFO);
    diag_register_source(DIAG_SRC_EVBUS, 0u, DIAG_LEVEL_INFO);
    diag_register_source(DIAG_SRC_UOBJ, 0u, DIAG_LEVEL_INFO);
    diag_register_source(DIAG_SRC_CAP, 0u, DIAG_LEVEL_INFO);
    diag_register_source(DIAG_SRC_DIAG, 0u, DIAG_LEVEL_INFO);

    diag_health_report(DIAG_SRC_KERNEL, DIAG_HEALTH_HEALTHY);
    diag_health_report(DIAG_SRC_KOG, DIAG_HEALTH_HEALTHY);
    diag_health_report(DIAG_SRC_EVBUS, DIAG_HEALTH_HEALTHY);
    diag_health_report(DIAG_SRC_UOBJ, DIAG_HEALTH_HEALTHY);
    diag_health_report(DIAG_SRC_CAP, DIAG_HEALTH_HEALTHY);
    diag_health_report(DIAG_SRC_DIAG, DIAG_HEALTH_HEALTHY);

    diag_log(DIAG_SRC_DIAG, DIAG_LEVEL_INFO, 1u, 0u);
}

int diag_self_test(void) {
    if (diag_register_source(99u, 0u, DIAG_LEVEL_WARNING) != 0) return -1;
    if (diag_find_source(99u) < 0) return -1;

    if (diag_log(99u, DIAG_LEVEL_WARNING, 100u, 0u) != 0) return -1;
    if (diag_log(99u, DIAG_LEVEL_DEBUG, 101u, 0u) == 0) return -1; /* gefiltert */

    if (diag_health_report(99u, DIAG_HEALTH_HEALTHY) != 0) return -1;
    uint32_t state;
    if (diag_health_get(99u, &state) != 0) return -1;
    if (state != DIAG_HEALTH_HEALTHY) return -1;
    if (diag_health_report(99u, DIAG_HEALTH_DEGRADED) != 0) return -1;

    if (diag_find_source(0xBEEFu) >= 0) return -1;

    return 0;
}

/* ===================================================================== */
/* Trace Ring                                                            */
/* ===================================================================== */

#define TRACE_CAPACITY 64u

typedef struct {
    uint32_t seq;
    uint32_t timestamp;
    uint32_t subsystem;
    uint32_t event_id;
    uint32_t arg_a;
    uint32_t arg_b;
} trace_event_t;

static trace_event_t trace_ring[TRACE_CAPACITY];
static uint32_t trace_write_pos = 0;
static uint32_t trace_seq = 0;
static uint32_t trace_total_emitted = 0;
static uint32_t trace_wrapped = 0;
static int trace_ready = 0;

void trace_initialize(void) {
    for (uint32_t i = 0; i < TRACE_CAPACITY; ++i) trace_ring[i] = (trace_event_t){0};
    trace_write_pos = 0;
    trace_seq = 0;
    trace_total_emitted = 0;
    trace_wrapped = 0;
    trace_ready = 1;
}

void trace_emit(uint32_t subsystem, uint32_t event_id, uint32_t arg_a, uint32_t arg_b) {
    trace_event_t *e = &trace_ring[trace_write_pos];
    e->seq = trace_seq++;
    e->timestamp = timer_get_ticks();
    e->subsystem = subsystem;
    e->event_id = event_id;
    e->arg_a = arg_a;
    e->arg_b = arg_b;

    ++trace_write_pos;
    if (trace_write_pos >= TRACE_CAPACITY) {
        trace_write_pos = 0;
        ++trace_wrapped;
    }
    ++trace_total_emitted;
}

int trace_self_test(void) {
    if (trace_ready != 1) return -1;

    trace_emit(TRACE_SYS_KERNEL, 0x100u, 42u, 99u);
    if (trace_total_emitted != 1u) return -1;
    if (trace_ring[0].subsystem != TRACE_SYS_KERNEL) return -1;
    if (trace_write_pos != 1u) return -1;

    for (uint32_t i = 0; i < 63u; ++i) trace_emit(TRACE_SYS_IRQ, 0x200u, 0u, 0u);
    if (trace_total_emitted != 64u) return -1;

    trace_emit(TRACE_SYS_SCHED, 0x300u, 0u, 0u);
    if (trace_wrapped != 1u) return -1;

    return 0;
}

/* ===================================================================== */
/* Performance Counters                                                  */
/* ===================================================================== */

#define METRICS_CAPACITY 32u

typedef struct {
    uint32_t value;
    uint32_t peak;
    uint32_t total;
} metrics_record_t;

static metrics_record_t metrics_table[METRICS_CAPACITY];
static int metrics_ready = 0;

void metrics_initialize(void) {
    for (uint32_t i = 0; i < METRICS_CAPACITY; ++i) metrics_table[i] = (metrics_record_t){0};
    metrics_ready = 1;
}

void metrics_increment(uint32_t slot) {
    if (slot >= METRICS_CAPACITY) return;
    metrics_table[slot].value++;
    metrics_table[slot].total++;
    if (metrics_table[slot].value > metrics_table[slot].peak) {
        metrics_table[slot].peak = metrics_table[slot].value;
    }
}

void metrics_add(uint32_t slot, uint32_t delta) {
    if (slot >= METRICS_CAPACITY) return;
    metrics_table[slot].value += delta;
    metrics_table[slot].total += delta;
    if (metrics_table[slot].value > metrics_table[slot].peak) {
        metrics_table[slot].peak = metrics_table[slot].value;
    }
}

int metrics_read(uint32_t slot, uint32_t *out_value, uint32_t *out_peak, uint32_t *out_total) {
    if (slot >= METRICS_CAPACITY) return -1;
    *out_value = metrics_table[slot].value;
    *out_peak = metrics_table[slot].peak;
    *out_total = metrics_table[slot].total;
    return 0;
}

void metrics_reset(uint32_t slot) {
    if (slot >= METRICS_CAPACITY) return;
    metrics_table[slot].value = 0;
}

int metrics_self_test(void) {
    if (metrics_ready != 1) return -1;

    metrics_increment(MET_SLOT_IRQ_COUNT);
    uint32_t value, peak, total;
    if (metrics_read(MET_SLOT_IRQ_COUNT, &value, &peak, &total) != 0) return -1;
    if (value != 1u) return -1;

    for (uint32_t i = 0; i < 4u; ++i) metrics_increment(MET_SLOT_IRQ_COUNT);
    if (metrics_read(MET_SLOT_IRQ_COUNT, &value, &peak, &total) != 0) return -1;
    if (value != 5u || peak != 5u || total != 5u) return -1;

    metrics_add(MET_SLOT_CTX_SWITCH, 100u);
    if (metrics_read(MET_SLOT_CTX_SWITCH, &value, &peak, &total) != 0) return -1;
    if (value != 100u) return -1;

    metrics_reset(MET_SLOT_IRQ_COUNT);
    if (metrics_read(MET_SLOT_IRQ_COUNT, &value, &peak, &total) != 0) return -1;
    if (value != 0u) return -1;
    if (total != 5u) return -1; /* total bleibt nach reset erhalten */

    return 0;
}

/* ===================================================================== */
/* Structured Log Ring                                                   */
/* ===================================================================== */

#define OBSLOG_CAPACITY 32u

typedef struct {
    uint32_t seq;
    uint32_t tick;
    uint32_t severity;
    uint32_t component;
    uint32_t event_type;
    uint32_t msg_id;
} obslog_record_t;

static obslog_record_t obslog_ring[OBSLOG_CAPACITY];
static int obslog_ready = 0;
static uint32_t obslog_write_pos = 0;
static uint32_t obslog_seq = 0;
static uint32_t obslog_total = 0;
static uint32_t obslog_errors = 0;

void obslog_initialize(void) {
    for (uint32_t i = 0; i < OBSLOG_CAPACITY; ++i) obslog_ring[i] = (obslog_record_t){0};
    obslog_write_pos = 0;
    obslog_seq = 0;
    obslog_total = 0;
    obslog_errors = 0;
    obslog_ready = 1;
}

int obslog_write(uint32_t severity, uint32_t component, uint32_t event_type, uint32_t msg_id) {
    if (!obslog_ready) return -1;
    if (severity > OBSLOG_SEV_CRIT) return -1;

    obslog_record_t *rec = &obslog_ring[obslog_write_pos];
    rec->seq = obslog_seq++;
    rec->tick = timer_get_ticks();
    rec->severity = severity;
    rec->component = component;
    rec->event_type = event_type;
    rec->msg_id = msg_id;

    obslog_write_pos = (obslog_write_pos + 1u) % OBSLOG_CAPACITY;
    ++obslog_total;
    if (severity >= OBSLOG_SEV_ERROR) ++obslog_errors;
    return 0;
}

int obslog_count(uint32_t severity, uint32_t *out_count) {
    if (!obslog_ready) return -1;

    uint32_t count = 0;
    for (uint32_t i = 0; i < OBSLOG_CAPACITY; ++i) {
        if (obslog_ring[i].seq == 0u) continue; /* siehe observability.h */
        if (obslog_ring[i].severity == severity) ++count;
    }
    *out_count = count;
    return 0;
}

int obslog_self_test(void) {
    if (obslog_write(OBSLOG_SEV_INFO, 1u, 1u, 0x4C4F4701u) != 0) return -1;
    if (obslog_write(OBSLOG_SEV_ERROR, 2u, 2u, 0x4C4F4702u) != 0) return -1;

    uint32_t count;
    if (obslog_count(OBSLOG_SEV_WARN, &count) != 0) return -1;
    if (count != 0u) return -1;

    if (obslog_count(OBSLOG_SEV_ERROR, &count) != 0) return -1;
    if (count == 0u) return -1;

    if (obslog_errors < 1u) return -1;

    return 0;
}

/* ===================================================================== */
/* Subsystem Introspection Registry                                      */
/* ===================================================================== */

#define INTRO_CAPACITY 16u

typedef struct {
    int occupied;
    uint32_t subsystem_id;
    intro_query_fn_t fn;
} intro_record_t;

static intro_record_t intro_registry[INTRO_CAPACITY];
static uint32_t intro_count = 0;

static uint32_t intro_provider_kernel(uint8_t *out_buf, uint32_t query_key) {
    if (query_key != 0) return 0;
    uint32_t *p = (uint32_t *)out_buf;
    p[0] = boot_phase_current;
    p[1] = boot_phase_last_success;
    p[2] = timer_get_ticks();
    return 12u;
}

/* Thread-Anzahl/aktueller Thread sind in scheduler.c/threadmgr.c nicht
   ueber einen oeffentlichen Zaehler-Accessor exponiert - Platzhalter 0,
   analog zu anderen bereits dokumentierten Stub-Feldern in diesem Kernel
   (z.B. aslr.c's wd_global_tick). */
static uint32_t intro_provider_sched(uint8_t *out_buf, uint32_t query_key) {
    if (query_key != 0) return 0;
    uint32_t *p = (uint32_t *)out_buf;
    p[0] = 0u;
    p[1] = 0u;
    return 8u;
}

static uint32_t intro_provider_memory(uint8_t *out_buf, uint32_t query_key) {
    if (query_key != 0) return 0;
    uint32_t *p = (uint32_t *)out_buf;
    p[0] = pmm_frame_count();
    p[1] = 0u; /* heap_bytes - kein oeffentlicher Accessor in heap.h */
    return 8u;
}

/* Platzhalter - cap.c exponiert keinen oeffentlichen Capability-Zaehler. */
static uint32_t intro_provider_security(uint8_t *out_buf, uint32_t query_key) {
    if (query_key != 0) return 0;
    uint32_t *p = (uint32_t *)out_buf;
    p[0] = 0u;
    return 4u;
}

/* Platzhalter - Watchdog-Subsystem (resilience32.inc) noch nicht portiert. */
static uint32_t intro_provider_resilience(uint8_t *out_buf, uint32_t query_key) {
    if (query_key != 0) return 0;
    uint32_t *p = (uint32_t *)out_buf;
    p[0] = 0u;
    return 4u;
}

void intro_manager_initialize(void) {
    for (uint32_t i = 0; i < INTRO_CAPACITY; ++i) intro_registry[i] = (intro_record_t){0};
    intro_count = 0;

    uint32_t slot;
    intro_register(INTRO_SYS_KERNEL, intro_provider_kernel, &slot);
    intro_register(INTRO_SYS_SCHED, intro_provider_sched, &slot);
    intro_register(INTRO_SYS_MEMORY, intro_provider_memory, &slot);
    intro_register(INTRO_SYS_SECURITY, intro_provider_security, &slot);
    intro_register(INTRO_SYS_RESILIENCE, intro_provider_resilience, &slot);
}

int intro_register(uint32_t subsystem_id, intro_query_fn_t fn, uint32_t *out_slot) {
    if (intro_count >= INTRO_CAPACITY) return -1;

    for (uint32_t i = 0; i < INTRO_CAPACITY; ++i) {
        if (!intro_registry[i].occupied) {
            intro_registry[i].occupied = 1;
            intro_registry[i].subsystem_id = subsystem_id;
            intro_registry[i].fn = fn;
            ++intro_count;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int intro_query(uint32_t subsystem_id, uint32_t *out_slot) {
    for (uint32_t i = 0; i < INTRO_CAPACITY; ++i) {
        if (intro_registry[i].occupied && intro_registry[i].subsystem_id == subsystem_id) {
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int intro_invoke(uint32_t subsystem_id, uint8_t *out_buf, uint32_t query_key,
                  uint32_t *out_bytes) {
    uint32_t slot;
    if (intro_query(subsystem_id, &slot) != 0) return -1;
    if (intro_registry[slot].fn == 0) return -1;

    *out_bytes = intro_registry[slot].fn(out_buf, query_key);
    return 0;
}

int intro_self_test(void) {
    uint32_t slot;
    if (intro_query(INTRO_SYS_KERNEL, &slot) != 0) return -1;
    if (intro_query(99u, &slot) == 0) return -1;

    if (intro_register(10u, 0, &slot) != 0) return -1;
    if (intro_query(10u, &slot) != 0) return -1;

    static uint8_t buf[32];
    uint32_t bytes;
    if (intro_invoke(INTRO_SYS_KERNEL, buf, 0u, &bytes) != 0) return -1;
    if (bytes != 12u) return -1;

    if (intro_invoke(99u, buf, 0u, &bytes) == 0) return -1;

    return 0;
}

/* ===================================================================== */
/* Tick-Based Sampling Profiler                                          */
/* ===================================================================== */

#define PROF_SAMPLE_COUNT 16u

typedef struct {
    uint32_t tick;
    uint32_t component_id; /* 0 = frei */
    uint32_t count;
} prof_record_t;

static prof_record_t prof_samples[PROF_SAMPLE_COUNT];
static int prof_ready = 0;
static uint32_t prof_total = 0;

void prof_initialize(void) {
    for (uint32_t i = 0; i < PROF_SAMPLE_COUNT; ++i) prof_samples[i] = (prof_record_t){0};
    prof_total = 0;
    prof_ready = 1;
}

void prof_sample(uint32_t component_id) {
    if (!prof_ready) return;

    for (uint32_t i = 0; i < PROF_SAMPLE_COUNT; ++i) {
        if (prof_samples[i].component_id == component_id) {
            prof_samples[i].tick = timer_get_ticks();
            ++prof_samples[i].count;
            ++prof_total;
            return;
        }
    }

    uint32_t slot = 0; /* Fallback: Slot 0 ueberschreiben, wenn kein freier da ist */
    for (uint32_t i = 0; i < PROF_SAMPLE_COUNT; ++i) {
        if (prof_samples[i].component_id == 0u) {
            slot = i;
            break;
        }
    }
    prof_samples[slot].tick = timer_get_ticks();
    prof_samples[slot].component_id = component_id;
    prof_samples[slot].count = 1u;
    ++prof_total;
}

int prof_hottest(uint32_t *out_component_id, uint32_t *out_count) {
    uint32_t best_component = 0;
    uint32_t best_count = 0;

    for (uint32_t i = 0; i < PROF_SAMPLE_COUNT; ++i) {
        if (prof_samples[i].component_id == 0u) continue;
        if (prof_samples[i].count == 0u) continue;
        if (prof_samples[i].count > best_count) {
            best_count = prof_samples[i].count;
            best_component = prof_samples[i].component_id;
        }
    }

    if (best_component == 0u) return -1;
    *out_component_id = best_component;
    *out_count = best_count;
    return 0;
}

void prof_reset(void) {
    for (uint32_t i = 0; i < PROF_SAMPLE_COUNT; ++i) prof_samples[i] = (prof_record_t){0};
}

int prof_self_test(void) {
    prof_sample(1u);
    prof_sample(1u);
    prof_sample(2u);

    uint32_t component, count;
    if (prof_hottest(&component, &count) != 0) return -1;
    if (component != 1u) return -1;
    if (count < 2u) return -1;

    prof_reset();
    if (prof_hottest(&component, &count) == 0) return -1;

    return 0;
}
