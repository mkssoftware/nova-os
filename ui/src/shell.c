#include "nova/ui/shell.h"

#include <string.h>

static nova_ui_dlu_t minimum(nova_ui_dlu_t a, nova_ui_dlu_t b) {
    return a < b ? a : b;
}

static nova_ui_dlu_t maximum(nova_ui_dlu_t a, nova_ui_dlu_t b) {
    return a > b ? a : b;
}

static ptrdiff_t window_index(const nova_shell_t *shell,
                              nova_ui_window_id_t id) {
    size_t index;
    if (shell == NULL || id.value == 0u) return -1;
    for (index = 0u; index < shell->window_count; ++index)
        if (shell->windows[index].id.value == id.value) return (ptrdiff_t)index;
    return -1;
}

static void layout_compute(nova_shell_t *shell, nova_ui_dlu_t width,
                           nova_ui_dlu_t height) {
    nova_ui_dlu_t margin = width < 1400 ? 16 : 20;
    nova_ui_dlu_t taskbar_height = shell->design.taskbar_height;
    nova_ui_dlu_t taskbar_y = height - margin - taskbar_height;
    nova_ui_dlu_t command_width = minimum(520, maximum(360, width / 3));
    nova_ui_dlu_t start_width = minimum(1120, width - margin * 4);
    nova_ui_dlu_t start_height = minimum(680, taskbar_y - 64);
    shell->layout.desktop = (nova_ui_rect_t){0, 0, width, height};
    shell->layout.branding = (nova_ui_rect_t){24, 16, 196, 48};
    shell->layout.command_bar =
        (nova_ui_rect_t){(width - command_width) / 2, 16, command_width, 42};
    shell->layout.system_status =
        (nova_ui_rect_t){maximum(0, width - 326), 14, 302, 52};
    shell->layout.taskbar =
        (nova_ui_rect_t){margin, taskbar_y, width - margin * 2, taskbar_height};
    shell->layout.start_menu = (nova_ui_rect_t){
        (width - start_width) / 2, taskbar_y - start_height - 14,
        start_width, start_height};
    shell->layout.work_area =
        (nova_ui_rect_t){margin, 76, width - margin * 2, taskbar_y - 88};
    shell->layout.compact = width < 1280 || height < 720;
}

static nova_ui_rect_t default_window_bounds(const nova_shell_t *shell,
                                             nova_shell_app_t app) {
    nova_ui_rect_t area = shell->layout.work_area;
    nova_ui_dlu_t inset = app == NOVA_SHELL_APP_CAPABILITIES_STUDIO ? 8 : 36;
    nova_ui_dlu_t stagger = (nova_ui_dlu_t)(shell->window_count * 18u);
    return (nova_ui_rect_t){area.x + inset + stagger,
                            area.y + inset + stagger,
                            maximum(640, area.width - inset * 2 - stagger),
                            maximum(420, area.height - inset * 2 - stagger)};
}

static void activate(nova_shell_t *shell, size_t active_index) {
    size_t index;
    for (index = 0u; index < shell->window_count; ++index)
        shell->windows[index].active = false;
    shell->windows[active_index].active = true;
    shell->windows[active_index].visible = true;
    shell->windows[active_index].z_order = ++shell->next_z_order;
    shell->active_window = shell->windows[active_index].id;
}

void nova_shell_init(nova_shell_t *shell, nova_ui_dlu_t width_dlu,
                     nova_ui_dlu_t height_dlu) {
    static const nova_shell_app_t defaults[] = {
        NOVA_SHELL_APP_EXPLORER, NOVA_SHELL_APP_BROWSER, NOVA_SHELL_APP_MAIL,
        NOVA_SHELL_APP_CODE, NOVA_SHELL_APP_TERMINAL, NOVA_SHELL_APP_SHEET,
        NOVA_SHELL_APP_CAPABILITIES_STUDIO};
    size_t index;
    if (shell == NULL) return;
    memset(shell, 0, sizeof(*shell));
    nova_design_system_init(&shell->design);
    shell->next_window_id = 1u;
    shell->next_z_order = 1u;
    shell->pinned_count = sizeof(defaults) / sizeof(defaults[0]);
    for (index = 0u; index < shell->pinned_count; ++index)
        shell->pinned[index] = defaults[index];
    if (width_dlu < 800) width_dlu = 800;
    if (height_dlu < 600) height_dlu = 600;
    layout_compute(shell, width_dlu, height_dlu);
}

nova_ui_result_t nova_shell_resize(nova_shell_t *shell,
                                   nova_ui_dlu_t width_dlu,
                                   nova_ui_dlu_t height_dlu) {
    if (shell == NULL || width_dlu < 800 || height_dlu < 600)
        return NOVA_UI_INVALID;
    layout_compute(shell, width_dlu, height_dlu);
    return NOVA_UI_OK;
}

void nova_shell_toggle_start(nova_shell_t *shell) {
    if (shell == NULL) return;
    shell->start_menu_open = !shell->start_menu_open;
    shell->command_palette_open = false;
    shell->command_palette_focused = false;
    if (shell->reduce_motion)
        shell->start_menu_progress_milli = shell->start_menu_open ? 1000u : 0u;
}

void nova_shell_close_start(nova_shell_t *shell) {
    if (shell == NULL) return;
    shell->start_menu_open = false;
    if (shell->reduce_motion) shell->start_menu_progress_milli = 0u;
}

void nova_shell_click_outside(nova_shell_t *shell) {
    if (shell == NULL) return;
    nova_shell_close_start(shell);
    shell->command_palette_open = false;
    shell->command_palette_focused = false;
}

void nova_shell_tick(nova_shell_t *shell, uint16_t elapsed_ms) {
    uint32_t step;
    if (shell == NULL || shell->reduce_motion) return;
    step = (uint32_t)elapsed_ms * 1000u / shell->design.transition_normal_ms;
    if (step == 0u && elapsed_ms != 0u) step = 1u;
    if (shell->start_menu_open) {
        uint32_t next = shell->start_menu_progress_milli + step;
        shell->start_menu_progress_milli = (uint16_t)(next > 1000u ? 1000u : next);
    } else {
        shell->start_menu_progress_milli =
            (uint16_t)(step >= shell->start_menu_progress_milli ?
                           0u : shell->start_menu_progress_milli - step);
    }
}

nova_ui_result_t nova_shell_handle_key(nova_shell_t *shell, uint32_t key,
                                       bool control, bool pressed) {
    if (shell == NULL) return NOVA_UI_INVALID;
    if (!pressed) return NOVA_UI_OK;
    if (control && (key == 'k' || key == 'K')) {
        shell->command_palette_open = true;
        shell->command_palette_focused = true;
        nova_shell_close_start(shell);
        return NOVA_UI_OK;
    }
    if (key == 27u) {
        if (shell->start_menu_open) nova_shell_close_start(shell);
        else {
            shell->command_palette_open = false;
            shell->command_palette_focused = false;
        }
        return NOVA_UI_OK;
    }
    return NOVA_UI_NOT_FOUND;
}

nova_ui_result_t nova_shell_launch(nova_shell_t *shell, nova_shell_app_t app,
                                   nova_ui_window_id_t *window) {
    size_t index;
    nova_shell_window_t *created;
    if (shell == NULL || window == NULL || app == NOVA_SHELL_APP_NONE ||
        app > NOVA_SHELL_APP_TERMINAL)
        return NOVA_UI_INVALID;
    for (index = 0u; index < shell->window_count; ++index) {
        if (shell->windows[index].app == app &&
            shell->windows[index].state != NOVA_UI_WINDOW_HIDDEN) {
            if (shell->windows[index].state == NOVA_UI_WINDOW_MINIMIZED)
                shell->windows[index].state = NOVA_UI_WINDOW_NORMAL;
            activate(shell, index);
            *window = shell->windows[index].id;
            nova_shell_close_start(shell);
            return NOVA_UI_OK;
        }
    }
    if (shell->window_count >= NOVA_SHELL_MAX_WINDOWS) return NOVA_UI_CAPACITY;
    created = &shell->windows[shell->window_count++];
    memset(created, 0, sizeof(*created));
    created->id.value = shell->next_window_id++;
    created->app = app;
    created->bounds = default_window_bounds(shell, app);
    created->restore_bounds = created->bounds;
    created->state = NOVA_UI_WINDOW_NORMAL;
    created->visible = true;
    activate(shell, shell->window_count - 1u);
    *window = created->id;
    nova_shell_close_start(shell);
    return NOVA_UI_OK;
}

nova_ui_result_t nova_shell_window_command(nova_shell_t *shell,
                                           nova_ui_window_id_t window,
                                           nova_shell_window_command_t command) {
    ptrdiff_t found = window_index(shell, window);
    nova_shell_window_t *target;
    if (found < 0 || command > NOVA_SHELL_WINDOW_CLOSE) return NOVA_UI_NOT_FOUND;
    target = &shell->windows[found];
    if (command == NOVA_SHELL_WINDOW_ACTIVATE) activate(shell, (size_t)found);
    else if (command == NOVA_SHELL_WINDOW_MINIMIZE) {
        target->state = NOVA_UI_WINDOW_MINIMIZED;
        target->visible = false;
        target->active = false;
        shell->active_window.value = 0u;
    } else if (command == NOVA_SHELL_WINDOW_MAXIMIZE) {
        if (target->state != NOVA_UI_WINDOW_MAXIMIZED)
            target->restore_bounds = target->bounds;
        target->bounds = shell->layout.work_area;
        target->state = NOVA_UI_WINDOW_MAXIMIZED;
        activate(shell, (size_t)found);
    } else if (command == NOVA_SHELL_WINDOW_RESTORE) {
        target->bounds = target->restore_bounds;
        target->state = NOVA_UI_WINDOW_NORMAL;
        activate(shell, (size_t)found);
    } else {
        target->state = NOVA_UI_WINDOW_HIDDEN;
        target->visible = false;
        target->active = false;
        if (shell->active_window.value == window.value)
            shell->active_window.value = 0u;
    }
    return NOVA_UI_OK;
}

nova_ui_result_t nova_shell_window_move(nova_shell_t *shell,
                                        nova_ui_window_id_t window,
                                        nova_ui_dlu_t x_dlu,
                                        nova_ui_dlu_t y_dlu) {
    ptrdiff_t found = window_index(shell, window);
    nova_shell_window_t *target;
    nova_ui_rect_t area;
    if (found < 0) return NOVA_UI_NOT_FOUND;
    target = &shell->windows[found];
    if (target->state != NOVA_UI_WINDOW_NORMAL) return NOVA_UI_CONFLICT;
    area = shell->layout.work_area;
    target->bounds.x = maximum(area.x, minimum(x_dlu,
        area.x + area.width - target->bounds.width));
    target->bounds.y = maximum(area.y, minimum(y_dlu,
        area.y + area.height - target->bounds.height));
    target->restore_bounds = target->bounds;
    activate(shell, (size_t)found);
    return NOVA_UI_OK;
}

const nova_shell_window_t *nova_shell_find_window(const nova_shell_t *shell,
                                                  nova_ui_window_id_t id) {
    ptrdiff_t found = window_index(shell, id);
    return found < 0 ? NULL : &shell->windows[found];
}
