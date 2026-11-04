#include "framesched.h"
#include "damagetrack.h"
#include "interrupt.h"
#include <stddef.h>

typedef struct {
    uint32_t output_id;
    uint32_t refresh_interval_ticks;
    uint32_t last_vsync_time;
    uint32_t vsync_counter;
    uint32_t idle_state;
    uint32_t power_mode;
    uint32_t min_interval_ticks;
    uint32_t last_frame_time;
    int has_last_frame_time;
    int in_use;
} output_sched_t;

typedef struct {
    uint32_t frame_id;
    uint32_t output_id;
    uint32_t target_id;
    uint32_t state;
    uint32_t event_time;
    uint32_t render_start;
    uint32_t render_complete;
    uint32_t presentation_time;
    uint32_t render_deadline;
    int in_use;
} frame_t;

static output_sched_t outputs[FSCHED_OUTPUT_CAPACITY];
static frame_t frames[FSCHED_FRAME_CAPACITY];
static uint32_t next_frame_id = 1;
static int framesched_ready = 0;

void framesched_initialize(void) {
    for (uint32_t i = 0; i < FSCHED_OUTPUT_CAPACITY; ++i) outputs[i] = (output_sched_t){0};
    for (uint32_t i = 0; i < FSCHED_FRAME_CAPACITY; ++i) frames[i] = (frame_t){0};
    next_frame_id = 1;
    framesched_ready = 1;
}

static output_sched_t *output_find(uint32_t id) {
    if (id == 0) return NULL;
    for (uint32_t i = 0; i < FSCHED_OUTPUT_CAPACITY; ++i) {
        if (outputs[i].in_use && outputs[i].output_id == id) return &outputs[i];
    }
    return NULL;
}

static frame_t *frame_find(uint32_t id) {
    if (id == 0) return NULL;
    for (uint32_t i = 0; i < FSCHED_FRAME_CAPACITY; ++i) {
        if (frames[i].in_use && frames[i].frame_id == id) return &frames[i];
    }
    return NULL;
}

int fsched_output_register(uint32_t output_id, uint32_t refresh_interval_ticks) {
    if (output_find(output_id)) return FSCHED_ERR_INVALID_STATE;
    uint32_t slot;
    for (slot = 0; slot < FSCHED_OUTPUT_CAPACITY; ++slot) {
        if (!outputs[slot].in_use) break;
    }
    if (slot >= FSCHED_OUTPUT_CAPACITY) return FSCHED_ERR_CAPACITY;

    outputs[slot] = (output_sched_t){0};
    outputs[slot].output_id = output_id;
    outputs[slot].refresh_interval_ticks = refresh_interval_ticks;
    outputs[slot].idle_state = OUTPUT_IDLE;
    outputs[slot].power_mode = POWER_BALANCED;
    outputs[slot].in_use = 1;
    return 0;
}

int fsched_output_set_power_mode(uint32_t output_id, uint32_t power_mode) {
    output_sched_t *o = output_find(output_id);
    if (!o) return FSCHED_ERR_NOT_FOUND;
    o->power_mode = power_mode;
    return 0;
}

int fsched_output_set_throttle(uint32_t output_id, uint32_t min_interval_ticks) {
    output_sched_t *o = output_find(output_id);
    if (!o) return FSCHED_ERR_NOT_FOUND;
    o->min_interval_ticks = min_interval_ticks;
    return 0;
}

int fsched_output_info(uint32_t output_id, output_sched_info_t *out_info) {
    output_sched_t *o = output_find(output_id);
    if (!o) return FSCHED_ERR_NOT_FOUND;
    out_info->output_id = o->output_id;
    out_info->refresh_interval_ticks = o->refresh_interval_ticks;
    out_info->last_vsync_time = o->last_vsync_time;
    out_info->vsync_counter = o->vsync_counter;
    out_info->idle_state = o->idle_state;
    out_info->power_mode = o->power_mode;
    out_info->min_interval_ticks = o->min_interval_ticks;
    out_info->last_frame_time = o->last_frame_time;
    return 0;
}

int fsched_vsync_tick(uint32_t output_id) {
    output_sched_t *o = output_find(output_id);
    if (!o) return FSCHED_ERR_NOT_FOUND;
    o->last_vsync_time = timer_get_ticks();
    o->vsync_counter++;
    return 0;
}

static int output_has_unpresented_frame(uint32_t output_id) {
    for (uint32_t i = 0; i < FSCHED_FRAME_CAPACITY; ++i) {
        if (frames[i].in_use && frames[i].output_id == output_id && frames[i].state != FRAME_PRESENTED) {
            return 1;
        }
    }
    return 0;
}

uint32_t fsched_compute_deadline(uint32_t target_presentation_time,
                                  uint32_t estimated_processing_ticks,
                                  uint32_t safety_margin_ticks) {
    uint32_t cost = estimated_processing_ticks + safety_margin_ticks;
    if (cost >= target_presentation_time) return 0; /* nicht unterlaufen */
    return target_presentation_time - cost;
}

int fsched_frame_request(uint32_t output_id, uint32_t target_id,
                          uint32_t estimated_processing_ticks,
                          uint32_t *out_frame_id, int *out_scheduled) {
    output_sched_t *o = output_find(output_id);
    if (!o) return FSCHED_ERR_NOT_FOUND;

    *out_scheduled = 0;

    uint32_t pending;
    if (damage_pending_count(target_id, &pending) != 0) return FSCHED_ERR_NOT_FOUND;
    if (pending == 0) {
        return 0; /* #2 EVENT-0001: kein Bedarf, kein Frame */
    }

    uint32_t now = timer_get_ticks();
    if (o->has_last_frame_time && o->min_interval_ticks > 0) {
        uint32_t elapsed = now - o->last_frame_time;
        if (elapsed < o->min_interval_ticks) {
            return 0; /* #6 THROTTLE-0001: gedrosselt, Damage bleibt erhalten */
        }
    }

    uint32_t slot;
    for (slot = 0; slot < FSCHED_FRAME_CAPACITY; ++slot) {
        if (!frames[slot].in_use) break;
    }
    if (slot >= FSCHED_FRAME_CAPACITY) return FSCHED_ERR_CAPACITY;

    uint32_t next_vsync = o->last_vsync_time + o->refresh_interval_ticks;

    frames[slot] = (frame_t){0};
    frames[slot].frame_id = next_frame_id++;
    frames[slot].output_id = output_id;
    frames[slot].target_id = target_id;
    frames[slot].state = FRAME_SCHEDULED;
    frames[slot].event_time = now;
    frames[slot].render_deadline = fsched_compute_deadline(next_vsync, estimated_processing_ticks, 2u);
    frames[slot].in_use = 1;

    o->last_frame_time = now;
    o->has_last_frame_time = 1;
    o->idle_state = OUTPUT_ACTIVE;

    *out_frame_id = frames[slot].frame_id;
    *out_scheduled = 1;
    return 0;
}

int fsched_frame_render_start(uint32_t frame_id) {
    frame_t *f = frame_find(frame_id);
    if (!f) return FSCHED_ERR_NOT_FOUND;
    if (f->state != FRAME_SCHEDULED) return FSCHED_ERR_INVALID_STATE;
    f->state = FRAME_RENDERING;
    f->render_start = timer_get_ticks();
    return 0;
}

int fsched_frame_render_complete(uint32_t frame_id) {
    frame_t *f = frame_find(frame_id);
    if (!f) return FSCHED_ERR_NOT_FOUND;
    if (f->state != FRAME_RENDERING) return FSCHED_ERR_INVALID_STATE;
    f->state = FRAME_READY;
    f->render_complete = timer_get_ticks();
    return 0;
}

int fsched_frame_mark_presented(uint32_t frame_id) {
    frame_t *f = frame_find(frame_id);
    if (!f) return FSCHED_ERR_NOT_FOUND;
    if (f->state != FRAME_READY) return FSCHED_ERR_INVALID_STATE;
    f->state = FRAME_PRESENTED;
    f->presentation_time = timer_get_ticks();

    output_sched_t *o = output_find(f->output_id);
    if (o && !output_has_unpresented_frame(o->output_id)) {
        o->idle_state = OUTPUT_IDLE;
    }
    return 0;
}

int fsched_frame_info(uint32_t frame_id, frame_info_t *out_info) {
    frame_t *f = frame_find(frame_id);
    if (!f) return FSCHED_ERR_NOT_FOUND;
    out_info->frame_id = f->frame_id;
    out_info->output_id = f->output_id;
    out_info->target_id = f->target_id;
    out_info->state = f->state;
    out_info->event_time = f->event_time;
    out_info->render_start = f->render_start;
    out_info->render_complete = f->render_complete;
    out_info->presentation_time = f->presentation_time;
    out_info->render_deadline = f->render_deadline;
    return 0;
}

int fsched_output_is_idle(uint32_t output_id, uint32_t target_id, int *out_idle) {
    output_sched_t *o = output_find(output_id);
    if (!o) return FSCHED_ERR_NOT_FOUND;

    if (output_has_unpresented_frame(output_id)) {
        *out_idle = 0;
        return 0;
    }

    uint32_t pending;
    if (damage_pending_count(target_id, &pending) != 0) return FSCHED_ERR_NOT_FOUND;
    *out_idle = (pending == 0) ? 1 : 0;
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

int framesched_self_test(void) {
    if (!framesched_ready) return -1;

    uint32_t outputA;
    /* output_id wird hier direkt vom Aufrufer vergeben (kein eigener
       ID-Allokator - die Ausgabe gehoert konzeptionell dem Display Server/
       Phase 7, hier reicht eine feste Testausgabe). */
    outputA = 1u;
    if (fsched_output_register(outputA, 16u) != 0) return -1;
    if (fsched_output_register(outputA, 16u) != FSCHED_ERR_INVALID_STATE) return -1; /* doppelt */
    if (fsched_output_set_throttle(outputA, 5u) != 0) return -1;

    output_sched_info_t oinfo;
    if (fsched_output_info(outputA, &oinfo) != 0) return -1;
    if (oinfo.idle_state != OUTPUT_IDLE || oinfo.power_mode != POWER_BALANCED) return -1;

    if (fsched_output_set_power_mode(outputA, POWER_SAVING) != 0) return -1;
    if (fsched_output_info(outputA, &oinfo) != 0) return -1;
    if (oinfo.power_mode != POWER_SAVING) return -1;

    /* Ohne Damage: kein Frame (#2 EVENT-0001). */
    uint32_t targetX = 777001u; /* generische ID, reine Damage-Verwaltung ueber Phase 5 */
    uint32_t frameId;
    int scheduled;
    if (fsched_frame_request(outputA, targetX, 3u, &frameId, &scheduled) != 0) return -1;
    if (scheduled != 0) return -1;

    /* Echter Damage (Phase 5) -> jetzt wird geplant. */
    uint32_t region_id;
    if (damage_add(targetX, (render_rect_t){0, 0, 10, 10}, 1u, &region_id) != 0) return -1;

    int idle_before;
    if (fsched_output_is_idle(outputA, targetX, &idle_before) != 0) return -1;
    if (idle_before) return -1; /* Damage liegt bereits vor, obwohl noch kein Frame offen ist */

    if (fsched_frame_request(outputA, targetX, 3u, &frameId, &scheduled) != 0) return -1;
    if (scheduled != 1) return -1;

    frame_info_t finfo;
    if (fsched_frame_info(frameId, &finfo) != 0) return -1;
    if (finfo.state != FRAME_SCHEDULED || finfo.output_id != outputA || finfo.target_id != targetX) return -1;

    int idle_after_schedule;
    if (fsched_output_is_idle(outputA, targetX, &idle_after_schedule) != 0) return -1;
    if (idle_after_schedule) return -1; /* ein offener, nicht praesentierter Frame existiert */

    if (fsched_output_info(outputA, &oinfo) != 0) return -1;
    if (oinfo.idle_state != OUTPUT_ACTIVE) return -1;

    /* Sofortige erneute Anforderung -> gedrosselt (#6 THROTTLE-0001),
       Damage bleibt erhalten (wurde nie aufgeloest). */
    uint32_t region_id2;
    if (damage_add(targetX, (render_rect_t){5, 5, 10, 10}, 2u, &region_id2) != 0) return -1;
    uint32_t frameId2;
    int scheduled2;
    if (fsched_frame_request(outputA, targetX, 3u, &frameId2, &scheduled2) != 0) return -1;
    if (scheduled2 != 0) return -1;
    uint32_t pending_after_throttle;
    if (damage_pending_count(targetX, &pending_after_throttle) != 0) return -1;
    if (pending_after_throttle < 1) return -1; /* nichts verloren */

    /* Render-Pipeline fuer den ersten Frame durchlaufen. */
    /* render_start/render_complete werden per timer_get_ticks() gestempelt;
       dieser Selbsttest laeuft bewusst vor timer_initialize()/
       interrupt_enable() (siehe kmain.c), daher ist 0 hier ein gueltiger
       Zeitstempel und wird NICHT als Fehler gewertet - nur der
       Zustandsuebergang wird geprueft. */
    if (fsched_frame_render_start(frameId) != 0) return -1;
    if (fsched_frame_info(frameId, &finfo) != 0) return -1;
    if (finfo.state != FRAME_RENDERING) return -1;

    if (fsched_frame_render_complete(frameId) != 0) return -1;
    if (fsched_frame_info(frameId, &finfo) != 0) return -1;
    if (finfo.state != FRAME_READY) return -1;

    /* Simuliert: Compositor hat die zum Zeitpunkt der Planung
       ausstehenden Regionen tatsaechlich verarbeitet. */
    uint32_t resolved_count;
    if (damage_resolve_target(targetX, &resolved_count) != 0) return -1;
    if (resolved_count == 0) return -1;

    if (fsched_frame_mark_presented(frameId) != 0) return -1;
    if (fsched_frame_info(frameId, &finfo) != 0) return -1;
    if (finfo.state != FRAME_PRESENTED) return -1;

    int idle_final;
    if (fsched_output_is_idle(outputA, targetX, &idle_final) != 0) return -1;
    if (!idle_final) return -1; /* Frame praesentiert, Damage aufgeloest */

    if (fsched_output_info(outputA, &oinfo) != 0) return -1;
    if (oinfo.idle_state != OUTPUT_IDLE) return -1;

    /* VSync-Tick: reiner Fortschritt, erzwingt keinen Frame. */
    uint32_t vsync_before = oinfo.vsync_counter;
    if (fsched_vsync_tick(outputA) != 0) return -1;
    if (fsched_output_info(outputA, &oinfo) != 0) return -1;
    if (oinfo.vsync_counter != vsync_before + 1) return -1;
    if (oinfo.idle_state != OUTPUT_IDLE) return -1; /* unveraendert durch VSync allein */

    /* Deadline-Arithmetik. */
    if (fsched_compute_deadline(100u, 10u, 5u) != 85u) return -1;
    if (fsched_compute_deadline(10u, 20u, 5u) != 0u) return -1; /* unterlaeuft nicht */

    /* Ungueltige Zustandsuebergaenge. */
    if (fsched_frame_render_start(frameId) != FSCHED_ERR_INVALID_STATE) return -1; /* bereits PRESENTED */

    /* Unbekannte IDs. */
    if (fsched_output_info(0xFFFFu, &oinfo) != FSCHED_ERR_NOT_FOUND) return -1;
    if (fsched_frame_info(0xFFFFu, &finfo) != FSCHED_ERR_NOT_FOUND) return -1;
    if (fsched_frame_render_start(0xFFFFu) != FSCHED_ERR_NOT_FOUND) return -1;

    return 0;
}
