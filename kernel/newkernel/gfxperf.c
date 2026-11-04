#include "gfxperf.h"
#include "gfxoptimize.h"
#include "framesched.h"
#include "damagetrack.h"
#include "renderengine.h"
#include "interrupt.h"
#include <stddef.h>

typedef struct {
    uint32_t subsystem_tag;
    uint32_t frame_id;
    uint32_t start_tick;
    int in_use;
} gfxperf_span_t;

typedef struct {
    uint32_t subsystem_tag;
    uint32_t count;
    uint32_t sum_ticks;
    uint32_t min_ticks;
    uint32_t max_ticks;
    int in_use;
} gfxperf_stats_t;

static gfxperf_span_t spans[GFXPERF_MAX_SPANS];
static gfxperf_stats_t stats[GFXPERF_MAX_SUBSYSTEM_STATS];
static int gfxperf_ready = 0;
static int gfxperf_enabled_flag = 1;

static uint32_t hysteresis_count = 0;
static uint32_t last_recommended_profile = GFXOPT_QUALITY_BALANCED;

void gfxperf_initialize(void) {
    for (uint32_t i = 0; i < GFXPERF_MAX_SPANS; ++i) spans[i] = (gfxperf_span_t){0};
    for (uint32_t i = 0; i < GFXPERF_MAX_SUBSYSTEM_STATS; ++i) stats[i] = (gfxperf_stats_t){0};
    gfxperf_enabled_flag = 1;
    hysteresis_count = 0;
    last_recommended_profile = GFXOPT_QUALITY_BALANCED;
    gfxperf_ready = 1;
}

int gfxperf_set_enabled(int enabled) {
    gfxperf_enabled_flag = enabled ? 1 : 0;
    return 0;
}

int gfxperf_enabled(void) { return gfxperf_enabled_flag; }

static gfxperf_stats_t *stats_find(uint32_t subsystem_tag) {
    for (uint32_t i = 0; i < GFXPERF_MAX_SUBSYSTEM_STATS; ++i) {
        if (stats[i].in_use && stats[i].subsystem_tag == subsystem_tag) return &stats[i];
    }
    return NULL;
}

static gfxperf_stats_t *stats_alloc(uint32_t subsystem_tag) {
    gfxperf_stats_t *s = stats_find(subsystem_tag);
    if (s) return s;
    for (uint32_t i = 0; i < GFXPERF_MAX_SUBSYSTEM_STATS; ++i) {
        if (!stats[i].in_use) {
            stats[i] = (gfxperf_stats_t){0};
            stats[i].subsystem_tag = subsystem_tag;
            stats[i].in_use = 1;
            return &stats[i];
        }
    }
    return NULL;
}

int gfxperf_begin(uint32_t subsystem_tag, uint32_t frame_id, uint32_t *out_span_id) {
    if (!gfxperf_enabled_flag) {
        *out_span_id = 0;
        return 0;
    }

    uint32_t slot;
    for (slot = 0; slot < GFXPERF_MAX_SPANS; ++slot) {
        if (!spans[slot].in_use) break;
    }
    if (slot >= GFXPERF_MAX_SPANS) return GFXPERF_ERR_CAPACITY;

    spans[slot] = (gfxperf_span_t){0};
    spans[slot].subsystem_tag = subsystem_tag;
    spans[slot].frame_id = frame_id;
    spans[slot].start_tick = timer_get_ticks();
    spans[slot].in_use = 1;

    *out_span_id = slot + 1; /* 1-basiert, 0 bleibt der Deaktiviert-Sentinel */
    return 0;
}

int gfxperf_end(uint32_t span_id, uint32_t *out_duration_ticks) {
    if (span_id == 0) {
        *out_duration_ticks = 0;
        return 0;
    }
    if (span_id > GFXPERF_MAX_SPANS || !spans[span_id - 1].in_use) return GFXPERF_ERR_NOT_FOUND;

    gfxperf_span_t *s = &spans[span_id - 1];
    uint32_t duration = timer_get_ticks() - s->start_tick;
    uint32_t subsystem_tag = s->subsystem_tag;
    s->in_use = 0;

    gfxperf_stats_t *st = stats_alloc(subsystem_tag);
    if (st) {
        if (st->count == 0) {
            st->min_ticks = duration;
            st->max_ticks = duration;
        } else {
            if (duration < st->min_ticks) st->min_ticks = duration;
            if (duration > st->max_ticks) st->max_ticks = duration;
        }
        st->sum_ticks += duration;
        st->count += 1;
    }

    *out_duration_ticks = duration;
    return 0;
}

int gfxperf_subsystem_stats(uint32_t subsystem_tag, uint32_t *out_count,
                             uint32_t *out_avg_ticks, uint32_t *out_min_ticks,
                             uint32_t *out_max_ticks) {
    gfxperf_stats_t *st = stats_find(subsystem_tag);
    if (!st || st->count == 0) return GFXPERF_ERR_NOT_FOUND;

    *out_count = st->count;
    *out_avg_ticks = st->sum_ticks / st->count;
    *out_min_ticks = st->min_ticks;
    *out_max_ticks = st->max_ticks;
    return 0;
}

int gfxperf_gpu_metric_available(void) { return 0; }

int gfxperf_frame_latency(uint32_t frame_id, uint32_t *out_wait_ticks,
                           uint32_t *out_render_ticks, uint32_t *out_present_ticks) {
    frame_info_t finfo;
    if (fsched_frame_info(frame_id, &finfo) != 0) return GFXPERF_ERR_NOT_FOUND;

    *out_wait_ticks = finfo.render_start - finfo.event_time;
    *out_render_ticks = finfo.render_complete - finfo.render_start;
    *out_present_ticks = finfo.presentation_time - finfo.render_complete;
    return 0;
}

int gfxperf_memory_report(uint32_t subsystem_tag, uint32_t used, uint32_t capacity,
                           uint32_t *out_percent) {
    (void)subsystem_tag;
    *out_percent = (capacity > 0) ? (used * 100u) / capacity : 0u;
    return 0;
}

int gfxperf_recommend_profile(uint32_t avg_frame_ticks, uint32_t *out_profile) {
    uint32_t target;
    if (avg_frame_ticks == 0) {
        *out_profile = last_recommended_profile;
        return 0;
    } else if (avg_frame_ticks <= 2u) {
        target = GFXOPT_QUALITY_ULTRA;
    } else if (avg_frame_ticks <= 5u) {
        target = GFXOPT_QUALITY_HIGH;
    } else if (avg_frame_ticks <= 10u) {
        target = GFXOPT_QUALITY_BALANCED;
    } else if (avg_frame_ticks <= 20u) {
        target = GFXOPT_QUALITY_PERFORMANCE;
    } else {
        target = GFXOPT_QUALITY_POWER_SAVING;
    }

    if (target == last_recommended_profile) {
        hysteresis_count = 0;
    } else {
        hysteresis_count += 1;
        if (hysteresis_count >= GFXPERF_HYSTERESIS_COUNT) {
            last_recommended_profile = target;
            hysteresis_count = 0;
        }
    }

    *out_profile = last_recommended_profile;
    return 0;
}

int gfxperf_self_test(void) {
    if (!gfxperf_ready) return -1;

    uint32_t span;
    uint32_t duration;

    /* Grundlegender Span-Zyklus. */
    if (gfxperf_begin(GFXPERF_SUBSYS_RENDER, 0, &span) != 0) return -1;
    if (span == 0) return -1;
    if (gfxperf_end(span, &duration) != 0) return -1;

    uint32_t count, avg, min_t, max_t;
    if (gfxperf_subsystem_stats(GFXPERF_SUBSYS_RENDER, &count, &avg, &min_t, &max_t) != 0) return -1;
    if (count != 1) return -1;

    if (gfxperf_begin(GFXPERF_SUBSYS_RENDER, 0, &span) != 0) return -1;
    if (gfxperf_end(span, &duration) != 0) return -1;
    if (gfxperf_subsystem_stats(GFXPERF_SUBSYS_RENDER, &count, &avg, &min_t, &max_t) != 0) return -1;
    if (count != 2) return -1;

    /* Unbekannter Subsystem-Tag -> NOT_FOUND. */
    if (gfxperf_subsystem_stats(GFXPERF_SUBSYS_MATERIAL, &count, &avg, &min_t, &max_t) != GFXPERF_ERR_NOT_FOUND) return -1;

    /* Unbekannte span_id -> NOT_FOUND. */
    if (gfxperf_end(0xFFFFu, &duration) != GFXPERF_ERR_NOT_FOUND) return -1;

    /* Deaktivierung: begin liefert Sentinel 0, end ist No-Op, Statistik
       bleibt unveraendert (PROFILING-0001 §17). */
    if (gfxperf_set_enabled(0) != 0) return -1;
    if (gfxperf_enabled() != 0) return -1;

    if (gfxperf_begin(GFXPERF_SUBSYS_RENDER, 0, &span) != 0) return -1;
    if (span != 0) return -1;
    if (gfxperf_end(span, &duration) != 0) return -1;
    if (duration != 0) return -1;

    if (gfxperf_subsystem_stats(GFXPERF_SUBSYS_RENDER, &count, &avg, &min_t, &max_t) != 0) return -1;
    if (count != 2) return -1; /* unveraendert, da Messung deaktiviert war */

    if (gfxperf_set_enabled(1) != 0) return -1;
    if (gfxperf_enabled() != 1) return -1;

    /* GPU-Metriken sind in diesem Kernel nie verfuegbar. */
    if (gfxperf_gpu_metric_available() != 0) return -1;

    /* Frame-Latenz: echter Phase-8-Frame durch den vollen Lebenszyklus. */
    uint32_t outputZ = 7001u;
    if (fsched_output_register(outputZ, 16u) != 0) return -1;
    uint32_t targetZ = 999001u;
    uint32_t region_id;
    if (damage_add(targetZ, (render_rect_t){0, 0, 4, 4}, 1u, &region_id) != 0) return -1;
    uint32_t frame_id;
    int scheduled;
    if (fsched_frame_request(outputZ, targetZ, 1u, &frame_id, &scheduled) != 0) return -1;
    if (scheduled != 1) return -1;
    if (fsched_frame_render_start(frame_id) != 0) return -1;
    if (fsched_frame_render_complete(frame_id) != 0) return -1;
    if (fsched_frame_mark_presented(frame_id) != 0) return -1;

    uint32_t wait_t, render_t, present_t;
    if (gfxperf_frame_latency(frame_id, &wait_t, &render_t, &present_t) != 0) return -1;

    /* Unbekannte frame_id -> NOT_FOUND. */
    if (gfxperf_frame_latency(0xFFFFu, &wait_t, &render_t, &present_t) != GFXPERF_ERR_NOT_FOUND) return -1;

    /* Speicher-Bericht: reine, deterministische Berechnung. */
    uint32_t percent;
    if (gfxperf_memory_report(GFXPERF_SUBSYS_SCENE, 10u, 16u, &percent) != 0) return -1;
    if (percent != 62u) return -1; /* 10*100/16 = 62 (Integer-Division) */
    if (gfxperf_memory_report(GFXPERF_SUBSYS_SCENE, 5u, 0u, &percent) != 0) return -1;
    if (percent != 0u) return -1; /* capacity==0 -> 0, keine Division durch 0 */

    /* Adaptive Empfehlung mit echter Hysterese: anfangs BALANCED,
       zwei abweichende Berichte wechseln noch nicht, der dritte
       (GFXPERF_HYSTERESIS_COUNT=3) wechselt auf POWER_SAVING. */
    uint32_t profile;
    if (gfxperf_recommend_profile(30u, &profile) != 0) return -1;
    if (profile != GFXOPT_QUALITY_BALANCED) return -1;
    if (gfxperf_recommend_profile(30u, &profile) != 0) return -1;
    if (profile != GFXOPT_QUALITY_BALANCED) return -1;
    if (gfxperf_recommend_profile(30u, &profile) != 0) return -1;
    if (profile != GFXOPT_QUALITY_POWER_SAVING) return -1;

    /* Keine Messdaten (0) aendert die Empfehlung nicht. */
    if (gfxperf_recommend_profile(0u, &profile) != 0) return -1;
    if (profile != GFXOPT_QUALITY_POWER_SAVING) return -1;

    return 0;
}
