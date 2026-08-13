#include "navigation.h"

static nova_navigation_entry_t entries[NOVA_NAVIGATION_CAPACITY];
static nova_navigation_diagnostics_t diagnostics;
static nova_navigation_visual_t visual;
static nova_animation_t *offset_animation;
static nova_animation_t *opacity_animation;
static uint16_t visual_group;

void nova_navigation_initialize(nova_navigation_entry_t root)
{
    diagnostics = (nova_navigation_diagnostics_t){0};
    visual = (nova_navigation_visual_t){0};
    offset_animation = opacity_animation = 0;
    visual_group = 40;
    entries[0] = root;
    diagnostics.depth = diagnostics.peak_depth = 1;
}

bool nova_navigation_push(nova_navigation_entry_t entry,
                          nova_navigation_transition_t transition)
{
    if (!diagnostics.depth || diagnostics.depth >= NOVA_NAVIGATION_CAPACITY ||
        diagnostics.transition_running) {
        ++diagnostics.rejected;
        return false;
    }
    entries[diagnostics.depth++] = entry;
    if (diagnostics.depth > diagnostics.peak_depth)
        diagnostics.peak_depth = diagnostics.depth;
    ++diagnostics.pushes;
    diagnostics.last_transition = transition;
    diagnostics.transition_running = true;
    return true;
}

bool nova_navigation_replace(nova_navigation_entry_t entry)
{
    if (!diagnostics.depth || diagnostics.transition_running) {
        ++diagnostics.rejected;
        return false;
    }
    entries[diagnostics.depth - 1] = entry;
    ++diagnostics.replaces;
    diagnostics.last_transition = NOVA_NAV_REPLACE;
    diagnostics.transition_running = true;
    return true;
}

bool nova_navigation_reset(void)
{
    if (!diagnostics.depth || diagnostics.transition_running) {
        ++diagnostics.rejected;
        return false;
    }
    entries[0].selection=0;
    entries[0].focus_id=0;
    entries[0].scroll=0;
    entries[0].context=0;
    diagnostics.depth=1;
    diagnostics.last_transition=NOVA_NAV_REPLACE;
    ++diagnostics.resets;
    return true;
}

bool nova_navigation_back(nova_navigation_entry_t *restored)
{
    if (!restored || diagnostics.depth <= 1 || diagnostics.transition_running) {
        ++diagnostics.rejected;
        return false;
    }
    --diagnostics.depth;
    *restored = entries[diagnostics.depth - 1];
    ++diagnostics.pops;
    ++diagnostics.focus_restores;
    diagnostics.last_transition = NOVA_NAV_POP;
    diagnostics.transition_running = true;
    return true;
}

bool nova_navigation_can_go_back(void) { return diagnostics.depth > 1; }

const nova_navigation_entry_t *nova_navigation_current(void)
{
    return diagnostics.depth ? &entries[diagnostics.depth - 1] : 0;
}

bool nova_navigation_update(uint16_t selection, uint16_t focus_id,
                            int32_t scroll, uint32_t context)
{
    if (!diagnostics.depth) { ++diagnostics.rejected; return false; }
    nova_navigation_entry_t *entry = &entries[diagnostics.depth - 1];
    entry->selection = selection;
    entry->focus_id = focus_id;
    entry->scroll = scroll;
    entry->context = context;
    return true;
}

void nova_navigation_transition_complete(void)
{
    diagnostics.transition_running = false;
}

bool nova_navigation_visual_begin(nova_navigation_transition_t type, bool entering,
                                  uint64_t start_ms)
{
    if (visual.running || type > NOVA_NAV_RECOVERY) {
        ++diagnostics.rejected;
        return false;
    }
    bool reduced = nova_motion_is_reduced();
    int32_t distance = reduced || type == NOVA_NAV_REPLACE ||
                       type == NOVA_NAV_RECOVERY ? 0 : 32;
    if (type == NOVA_NAV_POP) distance = -distance;
    visual = (nova_navigation_visual_t){
        entering ? distance : 0, entering ? 0 : 255,
        type == NOVA_NAV_RECOVERY ? 240u : reduced ? 150u : 180u,
        type, entering, true, reduced
    };
    diagnostics.transition_running = true;
    diagnostics.focus_locked = true;
    diagnostics.last_transition = type;
    ++diagnostics.visual_started;
    ++visual_group;
    opacity_animation = nova_motion_create(&(nova_animation_t){
        &visual.opacity,visual.opacity,entering?255:0,start_ms,0,visual.duration_ms,
        visual_group,2,0,NOVA_PROPERTY_OPACITY,NOVA_EASE_OUT_CUBIC,
        NOVA_MOTION_CREATED,false,false,true,0,0,0
    });
    offset_animation = 0;
    if (distance) offset_animation = nova_motion_create(&(nova_animation_t){
        &visual.offset_dlu,visual.offset_dlu,entering?0:-distance,start_ms,0,
        visual.duration_ms,visual_group,2,0,NOVA_PROPERTY_X,NOVA_EASE_OUT_CUBIC,
        NOVA_MOTION_CREATED,false,false,true,0,0,0
    });
    if (!opacity_animation || (distance && !offset_animation)) {
        if (opacity_animation) nova_motion_cancel(opacity_animation);
        if (offset_animation) nova_motion_cancel(offset_animation);
        visual.running=false; diagnostics.transition_running=false;
        diagnostics.focus_locked=false; ++diagnostics.rejected;
        return false;
    }
    return true;
}

void nova_navigation_visual_complete(void)
{
    if (!visual.running) return;
    visual.offset_dlu=0;
    visual.opacity=visual.entering?255:0;
    visual.running=false;
    diagnostics.transition_running=false;
    diagnostics.focus_locked=false;
    ++diagnostics.visual_completed;
}

void nova_navigation_visual_cancel(void)
{
    if (!visual.running) return;
    if (offset_animation) nova_motion_cancel(offset_animation);
    if (opacity_animation) nova_motion_cancel(opacity_animation);
    visual.running=false;
    diagnostics.transition_running=false;
    diagnostics.focus_locked=false;
    ++diagnostics.visual_cancelled;
}

const nova_navigation_visual_t *nova_navigation_visual(void) { return &visual; }

const nova_navigation_diagnostics_t *nova_navigation_diagnostics(void)
{
    return &diagnostics;
}
