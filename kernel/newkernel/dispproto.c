#include "dispproto.h"
#include "gfxstate.h"
#include "damagetrack.h"
#include "interrupt.h"
#include <stddef.h>

typedef struct {
    uint32_t output_id;
    int in_use;
} output_t;

typedef struct {
    uint32_t buffer_id;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t state;
    uint32_t ref_count;
    int in_use;
} buffer_t;

typedef struct {
    uint32_t fence_id;
    int signaled;
    int in_use;
} fence_t;

typedef struct {
    uint32_t surface_id;
    uint32_t owner_pid;
    uint32_t lifecycle_state;
    uint32_t committed_buffer_id;
    uint32_t committed_fence_id;
    uint32_t content_version;

    int has_pending;
    uint32_t pending_buffer_id;
    uint32_t pending_fence_id;

    int in_use;
} surface_t;

typedef struct {
    uint32_t presentation_id;
    uint32_t output_id;
    uint32_t surface_id;
    uint32_t status;
    uint32_t submit_time;
    uint32_t presentation_time;
    int in_use;
} presentation_t;

/* Jede Surface-Transaktion ist 1:1 eine Graphics State Transaction (Phase
   1) - diese kleine Tabelle merkt sich nur, welche surface_id zu welcher
   gfx_tx_id gehoert (gleiches Muster wie scene_tx_t in scenegraph.c). */
typedef struct {
    uint32_t gfx_tx_id;
    uint32_t surface_id;
    int in_use;
} commit_tx_t;

static output_t outputs[DISP_OUTPUT_CAPACITY];
static buffer_t buffers[DISP_BUFFER_CAPACITY];
static fence_t fences[DISP_FENCE_CAPACITY];
static surface_t surfaces[DISP_SURFACE_CAPACITY];
static presentation_t presentations[DISP_PRESENT_CAPACITY];
static commit_tx_t commit_txs[DISP_SURFACE_CAPACITY];

static uint32_t next_output_id = 1;
static uint32_t next_buffer_id = 1;
static uint32_t next_fence_id = 1;
static uint32_t next_surface_id = 1;
static uint32_t next_presentation_id = 1;
static int dispproto_ready = 0;

void dispproto_initialize(void) {
    for (uint32_t i = 0; i < DISP_OUTPUT_CAPACITY; ++i) outputs[i] = (output_t){0};
    for (uint32_t i = 0; i < DISP_BUFFER_CAPACITY; ++i) buffers[i] = (buffer_t){0};
    for (uint32_t i = 0; i < DISP_FENCE_CAPACITY; ++i) fences[i] = (fence_t){0};
    for (uint32_t i = 0; i < DISP_SURFACE_CAPACITY; ++i) surfaces[i] = (surface_t){0};
    for (uint32_t i = 0; i < DISP_PRESENT_CAPACITY; ++i) presentations[i] = (presentation_t){0};
    for (uint32_t i = 0; i < DISP_SURFACE_CAPACITY; ++i) commit_txs[i] = (commit_tx_t){0};
    next_output_id = 1;
    next_buffer_id = 1;
    next_fence_id = 1;
    next_surface_id = 1;
    next_presentation_id = 1;
    dispproto_ready = 1;
}

static output_t *output_find(uint32_t id) {
    if (id == 0) return NULL;
    for (uint32_t i = 0; i < DISP_OUTPUT_CAPACITY; ++i) {
        if (outputs[i].in_use && outputs[i].output_id == id) return &outputs[i];
    }
    return NULL;
}

static buffer_t *buffer_find(uint32_t id) {
    if (id == 0) return NULL;
    for (uint32_t i = 0; i < DISP_BUFFER_CAPACITY; ++i) {
        if (buffers[i].in_use && buffers[i].buffer_id == id) return &buffers[i];
    }
    return NULL;
}

static fence_t *fence_find(uint32_t id) {
    if (id == 0) return NULL;
    for (uint32_t i = 0; i < DISP_FENCE_CAPACITY; ++i) {
        if (fences[i].in_use && fences[i].fence_id == id) return &fences[i];
    }
    return NULL;
}

static surface_t *surface_find(uint32_t id) {
    if (id == 0) return NULL;
    for (uint32_t i = 0; i < DISP_SURFACE_CAPACITY; ++i) {
        if (surfaces[i].in_use && surfaces[i].surface_id == id) return &surfaces[i];
    }
    return NULL;
}

static presentation_t *presentation_find(uint32_t id) {
    if (id == 0) return NULL;
    for (uint32_t i = 0; i < DISP_PRESENT_CAPACITY; ++i) {
        if (presentations[i].in_use && presentations[i].presentation_id == id) return &presentations[i];
    }
    return NULL;
}

static commit_tx_t *commit_tx_find(uint32_t gfx_tx_id) {
    for (uint32_t i = 0; i < DISP_SURFACE_CAPACITY; ++i) {
        if (commit_txs[i].in_use && commit_txs[i].gfx_tx_id == gfx_tx_id) return &commit_txs[i];
    }
    return NULL;
}

int output_create(uint32_t *out_output_id) {
    uint32_t slot;
    for (slot = 0; slot < DISP_OUTPUT_CAPACITY; ++slot) {
        if (!outputs[slot].in_use) break;
    }
    if (slot >= DISP_OUTPUT_CAPACITY) return DISPPROTO_ERR_CAPACITY;
    outputs[slot].output_id = next_output_id++;
    outputs[slot].in_use = 1;
    *out_output_id = outputs[slot].output_id;
    return 0;
}

int buffer_create(uint32_t width, uint32_t height, uint32_t format, uint32_t *out_buffer_id) {
    uint32_t slot;
    for (slot = 0; slot < DISP_BUFFER_CAPACITY; ++slot) {
        if (!buffers[slot].in_use) break;
    }
    if (slot >= DISP_BUFFER_CAPACITY) return DISPPROTO_ERR_CAPACITY;

    buffers[slot].buffer_id = next_buffer_id++;
    buffers[slot].width = width;
    buffers[slot].height = height;
    buffers[slot].format = format;
    buffers[slot].state = BUFFER_AVAILABLE;
    buffers[slot].ref_count = 0;
    buffers[slot].in_use = 1;

    *out_buffer_id = buffers[slot].buffer_id;
    return 0;
}

int buffer_destroy(uint32_t buffer_id) {
    buffer_t *b = buffer_find(buffer_id);
    if (!b) return DISPPROTO_ERR_NOT_FOUND;
    if (b->ref_count > 0) return DISPPROTO_ERR_BUSY;
    *b = (buffer_t){0};
    return 0;
}

int buffer_info(uint32_t buffer_id, buffer_info_t *out_info) {
    buffer_t *b = buffer_find(buffer_id);
    if (!b) return DISPPROTO_ERR_NOT_FOUND;
    out_info->buffer_id = b->buffer_id;
    out_info->width = b->width;
    out_info->height = b->height;
    out_info->format = b->format;
    out_info->state = b->state;
    out_info->ref_count = b->ref_count;
    return 0;
}

static void buffer_add_ref(uint32_t buffer_id) {
    buffer_t *b = buffer_find(buffer_id);
    if (!b) return;
    b->ref_count++;
    b->state = BUFFER_IN_USE;
}

static void buffer_drop_ref(uint32_t buffer_id) {
    buffer_t *b = buffer_find(buffer_id);
    if (!b || b->ref_count == 0) return;
    b->ref_count--;
    if (b->ref_count == 0) b->state = BUFFER_RELEASED;
}

int fence_create(uint32_t *out_fence_id) {
    uint32_t slot;
    for (slot = 0; slot < DISP_FENCE_CAPACITY; ++slot) {
        if (!fences[slot].in_use) break;
    }
    if (slot >= DISP_FENCE_CAPACITY) return DISPPROTO_ERR_CAPACITY;
    fences[slot].fence_id = next_fence_id++;
    fences[slot].signaled = 0;
    fences[slot].in_use = 1;
    *out_fence_id = fences[slot].fence_id;
    return 0;
}

int fence_signal(uint32_t fence_id) {
    fence_t *f = fence_find(fence_id);
    if (!f) return DISPPROTO_ERR_NOT_FOUND;
    f->signaled = 1;
    return 0;
}

int fence_is_signaled(uint32_t fence_id, int *out_signaled) {
    fence_t *f = fence_find(fence_id);
    if (!f) return DISPPROTO_ERR_NOT_FOUND;
    *out_signaled = f->signaled;
    return 0;
}

int disp_surface_create(uint32_t owner_pid, uint32_t *out_surface_id) {
    uint32_t slot;
    for (slot = 0; slot < DISP_SURFACE_CAPACITY; ++slot) {
        if (!surfaces[slot].in_use) break;
    }
    if (slot >= DISP_SURFACE_CAPACITY) return DISPPROTO_ERR_CAPACITY;

    surfaces[slot] = (surface_t){0};
    surfaces[slot].surface_id = next_surface_id++;
    surfaces[slot].owner_pid = owner_pid;
    surfaces[slot].lifecycle_state = SURFACE_CREATED;
    surfaces[slot].in_use = 1;

    *out_surface_id = surfaces[slot].surface_id;
    return 0;
}

int disp_surface_destroy(uint32_t surface_id) {
    surface_t *s = surface_find(surface_id);
    if (!s) return DISPPROTO_ERR_NOT_FOUND;
    if (s->committed_buffer_id != 0) {
        buffer_drop_ref(s->committed_buffer_id);
    }
    *s = (surface_t){0};
    return 0;
}

int disp_surface_info(uint32_t surface_id, surface_info_t *out_info) {
    surface_t *s = surface_find(surface_id);
    if (!s) return DISPPROTO_ERR_NOT_FOUND;
    out_info->surface_id = s->surface_id;
    out_info->owner_pid = s->owner_pid;
    out_info->lifecycle_state = s->lifecycle_state;
    out_info->committed_buffer_id = s->committed_buffer_id;
    out_info->content_version = s->content_version;
    return 0;
}

int disp_surface_commit_begin(uint32_t surface_id, uint32_t *out_tx_id) {
    surface_t *s = surface_find(surface_id);
    if (!s) return DISPPROTO_ERR_NOT_FOUND;

    uint32_t gfx_tx_id;
    int rc = gfx_transaction_begin(&gfx_tx_id);
    if (rc != 0) return rc;

    uint32_t slot;
    for (slot = 0; slot < DISP_SURFACE_CAPACITY; ++slot) {
        if (!commit_txs[slot].in_use) break;
    }
    if (slot >= DISP_SURFACE_CAPACITY) {
        gfx_transaction_abort(gfx_tx_id);
        return DISPPROTO_ERR_CAPACITY;
    }
    commit_txs[slot].gfx_tx_id = gfx_tx_id;
    commit_txs[slot].surface_id = surface_id;
    commit_txs[slot].in_use = 1;

    *out_tx_id = gfx_tx_id;
    return 0;
}

int disp_surface_attach_buffer(uint32_t tx_id, uint32_t surface_id, uint32_t buffer_id,
                           uint32_t acquire_fence_id) {
    commit_tx_t *tx = commit_tx_find(tx_id);
    if (!tx || tx->surface_id != surface_id) return DISPPROTO_ERR_NOT_FOUND;
    surface_t *s = surface_find(surface_id);
    if (!s) return DISPPROTO_ERR_NOT_FOUND;
    if (!buffer_find(buffer_id)) return DISPPROTO_ERR_NOT_FOUND;

    s->has_pending = 1;
    s->pending_buffer_id = buffer_id;
    s->pending_fence_id = acquire_fence_id;

    gfx_transaction_record_change(tx_id, surface_id, GFX_CHANGE_RESOURCE, buffer_id,
                                   s->committed_buffer_id, buffer_id);
    return 0;
}

int disp_surface_set_damage(uint32_t tx_id, uint32_t surface_id, render_rect_t rect) {
    commit_tx_t *tx = commit_tx_find(tx_id);
    if (!tx || tx->surface_id != surface_id) return DISPPROTO_ERR_NOT_FOUND;
    surface_t *s = surface_find(surface_id);
    if (!s) return DISPPROTO_ERR_NOT_FOUND;

    uint32_t region_id;
    damage_add(surface_id, rect, s->content_version + 1, &region_id);

    gfx_transaction_record_change(tx_id, surface_id, GFX_CHANGE_CONTENT, 0, 0, 0);
    return 0;
}

int disp_surface_commit_finish(uint32_t tx_id, uint32_t *out_content_version) {
    commit_tx_t *tx = commit_tx_find(tx_id);
    if (!tx) return DISPPROTO_ERR_NOT_FOUND;
    surface_t *s = surface_find(tx->surface_id);
    if (!s) return DISPPROTO_ERR_NOT_FOUND;

    uint32_t version;
    int rc = gfx_transaction_commit(tx_id, &version);
    if (rc != 0) return rc;

    if (s->has_pending) {
        if (s->committed_buffer_id != 0) {
            buffer_drop_ref(s->committed_buffer_id);
        }
        buffer_add_ref(s->pending_buffer_id);
        s->committed_buffer_id = s->pending_buffer_id;
        s->committed_fence_id = s->pending_fence_id;
        s->has_pending = 0;
    }
    s->content_version = version;
    s->lifecycle_state = (s->committed_buffer_id != 0) ? SURFACE_ACTIVE : SURFACE_CONFIGURED;

    tx->in_use = 0;
    *out_content_version = version;
    return 0;
}

int disp_surface_commit_abort(uint32_t tx_id) {
    commit_tx_t *tx = commit_tx_find(tx_id);
    if (!tx) return DISPPROTO_ERR_NOT_FOUND;
    surface_t *s = surface_find(tx->surface_id);
    if (s) {
        s->has_pending = 0; /* Pending-Aenderungen wurden nie veroeffentlicht */
    }
    int rc = gfx_transaction_abort(tx_id);
    tx->in_use = 0;
    return rc;
}

int present_submit(uint32_t output_id, uint32_t surface_id, uint32_t *out_presentation_id) {
    if (!output_find(output_id)) return DISPPROTO_ERR_NOT_FOUND;
    surface_t *s = surface_find(surface_id);
    if (!s) return DISPPROTO_ERR_NOT_FOUND;
    if (s->committed_buffer_id == 0) return DISPPROTO_ERR_INVALID_STATE;

    if (s->committed_fence_id != 0) {
        int signaled = 0;
        if (fence_is_signaled(s->committed_fence_id, &signaled) != 0) return DISPPROTO_ERR_NOT_FOUND;
        if (!signaled) return DISPPROTO_ERR_NOT_READY;
    }

    uint32_t slot;
    for (slot = 0; slot < DISP_PRESENT_CAPACITY; ++slot) {
        if (!presentations[slot].in_use) break;
    }
    if (slot >= DISP_PRESENT_CAPACITY) return DISPPROTO_ERR_CAPACITY;

    presentations[slot].presentation_id = next_presentation_id++;
    presentations[slot].output_id = output_id;
    presentations[slot].surface_id = surface_id;
    presentations[slot].status = PRESENT_SUBMITTED;
    presentations[slot].submit_time = timer_get_ticks();
    presentations[slot].presentation_time = 0;
    presentations[slot].in_use = 1;

    *out_presentation_id = presentations[slot].presentation_id;
    return 0;
}

int present_mark_presented(uint32_t presentation_id) {
    presentation_t *p = presentation_find(presentation_id);
    if (!p) return DISPPROTO_ERR_NOT_FOUND;
    p->status = PRESENT_PRESENTED;
    p->presentation_time = timer_get_ticks();
    return 0;
}

int present_info(uint32_t presentation_id, present_info_t *out_info) {
    presentation_t *p = presentation_find(presentation_id);
    if (!p) return DISPPROTO_ERR_NOT_FOUND;
    out_info->presentation_id = p->presentation_id;
    out_info->output_id = p->output_id;
    out_info->surface_id = p->surface_id;
    out_info->status = p->status;
    out_info->submit_time = p->submit_time;
    out_info->presentation_time = p->presentation_time;
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

int dispproto_self_test(void) {
    if (!dispproto_ready) return -1;

    uint32_t outputA;
    if (output_create(&outputA) != 0) return -1;

    uint32_t bufA;
    if (buffer_create(100, 100, 1u, &bufA) != 0) return -1;
    buffer_info_t binfo;
    if (buffer_info(bufA, &binfo) != 0) return -1;
    if (binfo.state != BUFFER_AVAILABLE || binfo.ref_count != 0) return -1;

    uint32_t surfA;
    if (disp_surface_create(42u, &surfA) != 0) return -1;

    uint32_t fenceA;
    if (fence_create(&fenceA) != 0) return -1;
    int signaled;
    if (fence_is_signaled(fenceA, &signaled) != 0) return -1;
    if (signaled) return -1;

    /* Damage-Zaehlung VOR der Aenderung erfassen statt eine absolute Zahl
       zu erwarten: damagetrack.c's Tabelle ist namensraumfrei UEBER ALLE
       Module hinweg (target_id ist nur ein generischer uint32_t) - eine
       frueher im selben Boot erzeugte, nie aufgeloeste Region eines
       VOELLIG ANDEREN Moduls KANN zufaellig dieselbe numerische ID treffen
       wie eine frisch vergebene surface_id. Die Differenz ist deshalb der
       einzige robuste Nachweis echter Integration mit Phase 5. */
    uint32_t pending_before;
    if (damage_pending_count(surfA, &pending_before) != 0) return -1;

    uint32_t tx1;
    if (disp_surface_commit_begin(surfA, &tx1) != 0) return -1;
    if (disp_surface_attach_buffer(tx1, surfA, bufA, fenceA) != 0) return -1;
    if (disp_surface_set_damage(tx1, surfA, (render_rect_t){0, 0, 100, 100}) != 0) return -1;

    uint32_t pending_after;
    if (damage_pending_count(surfA, &pending_after) != 0) return -1;
    if (pending_after != pending_before + 1) return -1;

    /* Pending/Committed-Trennung: vor dem Commit noch nicht sichtbar. */
    surface_info_t sinfo;
    if (disp_surface_info(surfA, &sinfo) != 0) return -1;
    if (sinfo.committed_buffer_id != 0 || sinfo.lifecycle_state != SURFACE_CREATED) return -1;

    uint32_t v1;
    if (disp_surface_commit_finish(tx1, &v1) != 0) return -1;
    if (disp_surface_info(surfA, &sinfo) != 0) return -1;
    if (sinfo.committed_buffer_id != bufA || sinfo.lifecycle_state != SURFACE_ACTIVE) return -1;
    if (sinfo.content_version != v1) return -1;

    if (buffer_info(bufA, &binfo) != 0) return -1;
    if (binfo.state != BUFFER_IN_USE || binfo.ref_count != 1) return -1;

    /* Commit-Annahme != Fence signalisiert (#5 SYNC-0001/COMMIT-0001). */
    uint32_t presA;
    if (present_submit(outputA, surfA, &presA) != DISPPROTO_ERR_NOT_READY) return -1;

    if (fence_signal(fenceA) != 0) return -1;
    if (fence_is_signaled(fenceA, &signaled) != 0) return -1;
    if (!signaled) return -1;

    if (present_submit(outputA, surfA, &presA) != 0) return -1;
    present_info_t pinfo;
    if (present_info(presA, &pinfo) != 0) return -1;
    if (pinfo.status != PRESENT_SUBMITTED || pinfo.presentation_time != 0) return -1;

    if (present_mark_presented(presA) != 0) return -1;
    if (present_info(presA, &pinfo) != 0) return -1;
    if (pinfo.status != PRESENT_PRESENTED) return -1;

    /* Zweiter Commit mit neuem Buffer: alter Buffer wird dereferenziert,
       neuer referenziert. */
    uint32_t bufB, fenceB;
    if (buffer_create(100, 100, 1u, &bufB) != 0) return -1;
    if (fence_create(&fenceB) != 0) return -1;
    if (fence_signal(fenceB) != 0) return -1;

    uint32_t tx2;
    if (disp_surface_commit_begin(surfA, &tx2) != 0) return -1;
    if (disp_surface_attach_buffer(tx2, surfA, bufB, fenceB) != 0) return -1;
    uint32_t v2;
    if (disp_surface_commit_finish(tx2, &v2) != 0) return -1;
    if (v2 <= v1) return -1;

    if (buffer_info(bufA, &binfo) != 0) return -1;
    if (binfo.state != BUFFER_RELEASED || binfo.ref_count != 0) return -1;
    if (buffer_info(bufB, &binfo) != 0) return -1;
    if (binfo.state != BUFFER_IN_USE || binfo.ref_count != 1) return -1;

    /* bufA ist jetzt frei - darf zerstoert werden; bufB ist noch
       referenziert - darf nicht. */
    if (buffer_destroy(bufA) != 0) return -1;
    if (buffer_destroy(bufB) != DISPPROTO_ERR_BUSY) return -1;

    /* Abort: Pending-Aenderung wird verworfen, committed Zustand bleibt
       unveraendert. */
    uint32_t bufC;
    if (buffer_create(50, 50, 1u, &bufC) != 0) return -1;
    uint32_t tx3;
    if (disp_surface_commit_begin(surfA, &tx3) != 0) return -1;
    if (disp_surface_attach_buffer(tx3, surfA, bufC, 0) != 0) return -1;
    if (disp_surface_commit_abort(tx3) != 0) return -1;
    if (disp_surface_info(surfA, &sinfo) != 0) return -1;
    if (sinfo.committed_buffer_id != bufB || sinfo.content_version != v2) return -1;
    if (buffer_info(bufC, &binfo) != 0) return -1;
    if (binfo.ref_count != 0) return -1; /* niemals committed, nie referenziert */

    /* Aufraeumen: disp_surface_destroy dereferenziert bufB. */
    if (disp_surface_destroy(surfA) != 0) return -1;
    if (buffer_info(bufB, &binfo) != 0) return -1;
    if (binfo.ref_count != 0) return -1;
    if (buffer_destroy(bufB) != 0) return -1;
    if (buffer_destroy(bufC) != 0) return -1;

    /* Unbekannte IDs. */
    if (disp_surface_info(0xFFFFu, &sinfo) != DISPPROTO_ERR_NOT_FOUND) return -1;
    if (buffer_info(0xFFFFu, &binfo) != DISPPROTO_ERR_NOT_FOUND) return -1;
    if (fence_signal(0xFFFFu) != DISPPROTO_ERR_NOT_FOUND) return -1;
    if (present_info(0xFFFFu, &pinfo) != DISPPROTO_ERR_NOT_FOUND) return -1;

    return 0;
}
