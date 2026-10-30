/* NovaOS C-Kernel - Nova Memory Reclamation (siehe reclaim.h fuer Scope). */
#include "reclaim.h"

#define RECLAIM_THRASHING_STREAK 3u /* siehe Anforderung #11 */

typedef struct {
    uint32_t used; /* 0 = freier Slot */
    uint32_t reclaim_class;
    reclaim_callback_t callback;
} reclaim_source_t;

static reclaim_source_t g_sources[RECLAIM_SOURCE_CAPACITY];
static uint32_t g_run_count = 0;
static uint32_t g_total_reclaimed = 0;
static uint32_t g_zero_yield_streak = 0;

void reclaim_initialize(void) {
    for (uint32_t i = 0; i < RECLAIM_SOURCE_CAPACITY; ++i) {
        g_sources[i].used = 0;
    }
    g_run_count = 0;
    g_total_reclaimed = 0;
    g_zero_yield_streak = 0;
}

int reclaim_register_source(uint32_t reclaim_class, reclaim_callback_t callback, uint32_t *out_slot) {
    if (reclaim_class == RECLAIM_CLASS_ACTIVE || !callback) return -1; /* Anforderung #3 */

    for (uint32_t i = 0; i < RECLAIM_SOURCE_CAPACITY; ++i) {
        if (!g_sources[i].used) {
            g_sources[i].used = 1;
            g_sources[i].reclaim_class = reclaim_class;
            g_sources[i].callback = callback;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int reclaim_unregister_source(uint32_t slot) {
    if (slot >= RECLAIM_SOURCE_CAPACITY || !g_sources[slot].used) return -1;
    g_sources[slot].used = 0;
    return 0;
}

static uint32_t run_class(uint32_t reclaim_class, uint32_t *remaining_budget) {
    uint32_t reclaimed_here = 0;
    for (uint32_t i = 0; i < RECLAIM_SOURCE_CAPACITY && *remaining_budget > 0; ++i) {
        if (!g_sources[i].used || g_sources[i].reclaim_class != reclaim_class) continue;
        uint32_t got = g_sources[i].callback(*remaining_budget);
        if (got > *remaining_budget) got = *remaining_budget; /* Quelle darf Budget nicht ueberziehen */
        *remaining_budget -= got;
        reclaimed_here += got;
    }
    return reclaimed_here;
}

uint32_t reclaim_run(uint32_t budget) {
    ++g_run_count;
    uint32_t remaining = budget;

    /* Anforderung #4: Discardable zuerst, dann Reconstructible. ACTIVE
       wird nie als Quelle registriert (siehe reclaim_register_source). */
    uint32_t reclaimed = run_class(RECLAIM_CLASS_DISCARDABLE, &remaining);
    reclaimed += run_class(RECLAIM_CLASS_RECONSTRUCTIBLE, &remaining);

    g_total_reclaimed += reclaimed;

    if (budget > 0 && reclaimed == 0) {
        ++g_zero_yield_streak;
    } else {
        g_zero_yield_streak = 0;
    }

    return reclaimed;
}

uint32_t reclaim_run_direct(uint32_t budget) { return reclaim_run(budget); }
uint32_t reclaim_run_background(uint32_t budget) { return reclaim_run(budget); }

int reclaim_is_thrashing(void) {
    return g_zero_yield_streak >= RECLAIM_THRASHING_STREAK;
}

uint32_t reclaim_run_count(void) { return g_run_count; }
uint32_t reclaim_total_reclaimed(void) { return g_total_reclaimed; }

/* ---- Selbsttest ---- */

static uint32_t g_test_cache_units = 10u;

static uint32_t test_discardable_cb(uint32_t budget) {
    uint32_t give = (budget < g_test_cache_units) ? budget : g_test_cache_units;
    g_test_cache_units -= give;
    return give;
}

static uint32_t test_empty_cb(uint32_t budget) {
    (void)budget;
    return 0;
}

int reclaim_self_test(void) {
    reclaim_initialize();
    g_test_cache_units = 10u;

    /* ACTIVE-Klasse darf nie registriert werden. */
    uint32_t bogus_slot;
    if (reclaim_register_source(RECLAIM_CLASS_ACTIVE, test_empty_cb, &bogus_slot) == 0) return -1;

    uint32_t discardable_slot, empty_slot;
    if (reclaim_register_source(RECLAIM_CLASS_DISCARDABLE, test_discardable_cb, &discardable_slot) != 0) return -1;
    if (reclaim_register_source(RECLAIM_CLASS_RECONSTRUCTIBLE, test_empty_cb, &empty_slot) != 0) return -1;

    uint32_t got = reclaim_run(6u);
    if (got != 6u) return -1;
    if (g_test_cache_units != 4u) return -1;
    if (reclaim_total_reclaimed() != 6u) return -1;
    if (reclaim_is_thrashing()) return -1;

    /* Zweiter Lauf erschoepft die Discardable-Quelle vollstaendig
       (nur noch 4 Einheiten uebrig, 10 angefordert). */
    got = reclaim_run(10u);
    if (got != 4u) return -1;
    if (g_test_cache_units != 0u) return -1;

    /* Ab jetzt liefert gar keine Quelle mehr etwas - RECLAIM_THRASHING_STREAK
       aufeinanderfolgende Null-Ertraege muessen Thrashing melden. */
    for (uint32_t i = 0; i < RECLAIM_THRASHING_STREAK; ++i) {
        if (reclaim_run(5u) != 0u) return -1;
    }
    if (!reclaim_is_thrashing()) return -1;

    /* Erneute Verfuegbarkeit setzt die Thrashing-Erkennung zurueck. */
    g_test_cache_units = 3u;
    if (reclaim_run(3u) != 3u) return -1;
    if (reclaim_is_thrashing()) return -1;

    if (reclaim_unregister_source(discardable_slot) != 0) return -1;
    if (reclaim_unregister_source(empty_slot) != 0) return -1;
    if (reclaim_unregister_source(discardable_slot) == 0) return -1; /* doppelt */

    /* Budget 0 darf nicht als Thrashing-Null-Ertrag gewertet werden. */
    reclaim_initialize();
    uint32_t slot;
    if (reclaim_register_source(RECLAIM_CLASS_DISCARDABLE, test_empty_cb, &slot) != 0) return -1;
    if (reclaim_run(0u) != 0u) return -1;
    if (reclaim_is_thrashing()) return -1;

    return 0;
}
