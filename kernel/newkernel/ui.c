/* NovaOS C-Kernel - UI-Subsystem (Port aus ui32.inc). */
#include "ui.h"

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t z_order;
    uint32_t damage_x;
    uint32_t damage_y;
    uint32_t damage_w;
    uint32_t damage_h;
    uint32_t framebuffer;
    uint32_t input_handler;
    uint32_t theme_id;
    uint32_t capability_mask;
} ui_window_t;

typedef struct {
    uint32_t type;
    uint32_t key_code;
    uint32_t x;
    uint32_t y;
    uint32_t modifiers;
} ui_event_t;

typedef struct {
    uint32_t id;
    uint32_t parent_id;
    uint32_t type;
    uint32_t flags;
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
} ui_scenegraph_node_t;

static ui_window_t ui_win_table[UI_WIN_CAPACITY];
static ui_event_t ui_evt_queue[UI_EVT_CAPACITY];
static ui_scenegraph_node_t ui_sgn_table[UI_SGN_CAPACITY];

static uint32_t ui_win_count = 0;
static uint32_t ui_evt_head = 0;
static uint32_t ui_sgn_count = 0;
static uint32_t ui_next_win_id = 1;
static uint32_t ui_next_sgn_id = 1;
static uint32_t ui_target_fps = UI_FPS_60;
static uint32_t ui_compositor_mode = UI_COMP_CPU;
static uint32_t ui_active_theme = UI_THEME_DARK;
static uint32_t ui_frame_count = 0;
static int ui_ready = 0;

void ui_initialize(void) {
    for (uint32_t i = 0; i < UI_WIN_CAPACITY; ++i) {
        ui_win_table[i] = (ui_window_t){0};
    }
    for (uint32_t i = 0; i < UI_EVT_CAPACITY; ++i) {
        ui_evt_queue[i] = (ui_event_t){0};
    }
    for (uint32_t i = 0; i < UI_SGN_CAPACITY; ++i) {
        ui_sgn_table[i] = (ui_scenegraph_node_t){0};
    }

    ui_win_count = 0;
    ui_evt_head = 0;
    ui_sgn_count = 0;
    ui_next_win_id = 1;
    ui_next_sgn_id = 1;
    ui_target_fps = UI_FPS_60;
    ui_compositor_mode = UI_COMP_CPU;
    ui_active_theme = UI_THEME_DARK;
    ui_frame_count = 0;

    ui_ready = 1;
}

int ui_window_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t *out_win_id) {
    if (ui_win_count >= UI_WIN_CAPACITY) {
        return -1;
    }
    ui_window_t *win = &ui_win_table[ui_win_count];
    win->id = ui_next_win_id;
    win->flags = UI_WIN_VISIBLE | UI_WIN_RETAINED;
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
    win->z_order = ui_win_count; /* neue Fenster oben */
    win->damage_x = 0;
    win->damage_y = 0;
    win->damage_w = w;
    win->damage_h = h;
    win->framebuffer = 0;
    win->input_handler = 0;
    win->theme_id = ui_active_theme;
    win->capability_mask = 0;

    if (out_win_id) {
        *out_win_id = ui_next_win_id;
    }
    ++ui_next_win_id;
    ++ui_win_count;
    return 0;
}

int ui_window_destroy(uint32_t win_id) {
    for (uint32_t i = 0; i < ui_win_count; ++i) {
        if (ui_win_table[i].id == win_id) {
            ui_win_table[i].flags &= ~UI_WIN_VISIBLE;
            return 0;
        }
    }
    return -1;
}

int ui_compositor_frame(void) {
    for (uint32_t i = 0; i < ui_win_count; ++i) {
        ui_window_t *win = &ui_win_table[i];
        if (win->flags & UI_WIN_VISIBLE) {
            win->flags &= ~UI_WIN_DAMAGED;
        }
    }
    ++ui_frame_count;
    return 0;
}

int ui_input_dispatch(uint32_t type, uint32_t key_code, uint32_t x, uint32_t y) {
    ui_event_t *evt = &ui_evt_queue[ui_evt_head];
    evt->type = type;
    evt->key_code = key_code;
    evt->x = x;
    evt->y = y;
    evt->modifiers = 0;

    ++ui_evt_head;
    if (ui_evt_head >= UI_EVT_CAPACITY) {
        ui_evt_head = 0;
    }
    return 0;
}

void ui_set_theme(uint32_t theme_id) {
    ui_active_theme = theme_id;
    for (uint32_t i = 0; i < ui_win_count; ++i) {
        ui_win_table[i].flags |= UI_WIN_DAMAGED;
        ui_win_table[i].theme_id = theme_id;
    }
}

int ui_scenegraph_add(uint32_t parent_id, uint32_t type, uint32_t x, uint32_t y,
                       uint32_t w, uint32_t h, uint32_t *out_node_id) {
    if (ui_sgn_count >= UI_SGN_CAPACITY) {
        return -1;
    }
    ui_scenegraph_node_t *node = &ui_sgn_table[ui_sgn_count];
    node->id = ui_next_sgn_id;
    node->parent_id = parent_id;
    node->type = type;
    node->flags = 0;
    node->x = x;
    node->y = y;
    node->w = w;
    node->h = h;

    if (out_node_id) {
        *out_node_id = ui_next_sgn_id;
    }
    ++ui_next_sgn_id;
    ++ui_sgn_count;
    return 0;
}

int ui_self_test(void) {
    return (ui_ready == 1) ? 0 : -1;
}
