/* NovaOS C-Kernel - strukturiertes Early-/Kernel-Logging
   (Port aus memory32.inc, NPSPEC-KERNEL-0023). Semantik (Ringgroesse,
   Drop-/Overwrite-Zaehlung, Critical-Spiegelung, Rekursionsschutz) 1:1
   aus dem alten ASM uebernommen. */
#include "logging.h"

#define LOG_RING_CAPACITY  8u
#define LOG_COMMIT_MARKER  0x43474F4Cu /* "LOGC" */
#define LOG_FLAG_EARLY_TIME 1u

typedef struct {
    uint32_t sequence;
    uint32_t level;
    uint32_t category;
    uint32_t component;
    uint32_t event;
    uint32_t timestamp;
    uint32_t flags;
    uint32_t commit_marker;
} log_record_t; /* 32 Bytes = LOG_RECORD_SIZE */

static log_record_t logging_ring[LOG_RING_CAPACITY];
static log_record_t logging_critical_record;

static uint32_t logging_write_index = 0;
static uint32_t logging_record_count = 0;
static uint32_t logging_written_records = 0;
static uint32_t logging_dropped_records = 0;
static uint32_t logging_overwritten_records = 0;
static uint32_t logging_critical_records = 0;
static uint32_t logging_recursion_events = 0;
static int logging_recursion_guard = 0;

/* TODO: durch echten Wert ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

void logging_initialize(void) {
    for (uint32_t i = 0; i < LOG_RING_CAPACITY; ++i) {
        logging_ring[i] = (log_record_t){0};
    }
    logging_critical_record = (log_record_t){0};
    logging_write_index = 0;
    logging_record_count = 0;
    logging_written_records = 0;
    logging_dropped_records = 0;
    logging_overwritten_records = 0;
    logging_critical_records = 0;
    logging_recursion_events = 0;
    logging_recursion_guard = 0;
}

int logging_write(uint32_t level, uint32_t category, uint32_t component, uint32_t event) {
    if (level > 7u || category > 13u) {
        logging_recursion_guard = 0;
        return -1;
    }
    if (logging_recursion_guard != 0) {
        ++logging_recursion_events;
        logging_recursion_guard = 0;
        return -1;
    }
    logging_recursion_guard = 1;

    log_record_t *slot = &logging_ring[logging_write_index];
    slot->commit_marker = 0;
    ++logging_written_records;
    slot->sequence = logging_written_records;
    slot->level = level;
    slot->category = category;
    slot->component = component;
    slot->event = event;
    uint32_t ticks = current_timer_ticks();
    slot->timestamp = ticks;
    slot->flags = (ticks == 0) ? LOG_FLAG_EARLY_TIME : 0u;
    slot->commit_marker = LOG_COMMIT_MARKER;

    if (logging_record_count < LOG_RING_CAPACITY) {
        ++logging_record_count;
    } else {
        ++logging_dropped_records;
        ++logging_overwritten_records;
    }
    logging_write_index = (logging_write_index + 1u) & (LOG_RING_CAPACITY - 1u);

    if (level >= LOG_LEVEL_CRITICAL) {
        ++logging_critical_records;
        logging_critical_record = *slot;
    }

    logging_recursion_guard = 0;
    return 0;
}

int logging_self_test(void) {
    for (uint32_t iteration = 0; iteration < 10; ++iteration) {
        uint32_t level = iteration & 7u;
        uint32_t category = 1; /* Kernel-Kategorie */
        uint32_t component = 1; /* Kernel-Core-Komponente */
        uint32_t event = 0x100u + iteration;
        if (logging_write(level, category, component, event) != 0) {
            return -1;
        }
    }

    if (logging_written_records != 10) return -1;
    if (logging_record_count != LOG_RING_CAPACITY) return -1;
    if (logging_dropped_records != 2) return -1;
    if (logging_overwritten_records != 2) return -1;

    uint32_t last_index = (logging_write_index - 1u) & (LOG_RING_CAPACITY - 1u);
    if (logging_ring[last_index].sequence != 10) return -1;
    if (logging_ring[last_index].commit_marker != LOG_COMMIT_MARKER) return -1;
    if (logging_critical_records != 2) return -1;
    if (logging_critical_record.commit_marker != LOG_COMMIT_MARKER) return -1;

    return 0;
}
