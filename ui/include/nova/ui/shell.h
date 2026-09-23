#ifndef NOVA_UI_SHELL_H
#define NOVA_UI_SHELL_H

#include "nova/ui/design.h"

#define NOVA_SHELL_MAX_WINDOWS 8u
#define NOVA_SHELL_MAX_PINS 8u

typedef enum {
    NOVA_SHELL_APP_NONE = 0,
    NOVA_SHELL_APP_EXPLORER,
    NOVA_SHELL_APP_SHEET,
    NOVA_SHELL_APP_CAPABILITIES_STUDIO,
    NOVA_SHELL_APP_BROWSER,
    NOVA_SHELL_APP_MAIL,
    NOVA_SHELL_APP_CODE,
    NOVA_SHELL_APP_TERMINAL
} nova_shell_app_t;

typedef enum {
    NOVA_SHELL_WINDOW_ACTIVATE = 0,
    NOVA_SHELL_WINDOW_MINIMIZE,
    NOVA_SHELL_WINDOW_MAXIMIZE,
    NOVA_SHELL_WINDOW_RESTORE,
    NOVA_SHELL_WINDOW_CLOSE
} nova_shell_window_command_t;

typedef struct {
    nova_ui_rect_t desktop;
    nova_ui_rect_t branding;
    nova_ui_rect_t command_bar;
    nova_ui_rect_t system_status;
    nova_ui_rect_t taskbar;
    nova_ui_rect_t start_menu;
    nova_ui_rect_t work_area;
    bool compact;
} nova_shell_layout_t;

typedef struct {
    nova_ui_window_id_t id;
    nova_shell_app_t app;
    nova_ui_rect_t bounds;
    nova_ui_rect_t restore_bounds;
    nova_ui_window_state_t state;
    uint32_t z_order;
    bool active;
    bool visible;
} nova_shell_window_t;

typedef struct {
    nova_design_system_t design;
    nova_shell_layout_t layout;
    nova_shell_window_t windows[NOVA_SHELL_MAX_WINDOWS];
    size_t window_count;
    nova_shell_app_t pinned[NOVA_SHELL_MAX_PINS];
    size_t pinned_count;
    nova_ui_window_id_t active_window;
    uint64_t next_window_id;
    uint32_t next_z_order;
    uint16_t start_menu_progress_milli;
    bool start_menu_open;
    bool command_palette_open;
    bool command_palette_focused;
    bool reduce_motion;
} nova_shell_t;

void nova_shell_init(nova_shell_t *shell, nova_ui_dlu_t width_dlu,
                     nova_ui_dlu_t height_dlu);
nova_ui_result_t nova_shell_resize(nova_shell_t *shell,
                                   nova_ui_dlu_t width_dlu,
                                   nova_ui_dlu_t height_dlu);
void nova_shell_toggle_start(nova_shell_t *shell);
void nova_shell_close_start(nova_shell_t *shell);
void nova_shell_click_outside(nova_shell_t *shell);
void nova_shell_tick(nova_shell_t *shell, uint16_t elapsed_ms);
nova_ui_result_t nova_shell_handle_key(nova_shell_t *shell, uint32_t key,
                                       bool control, bool pressed);
nova_ui_result_t nova_shell_launch(nova_shell_t *shell, nova_shell_app_t app,
                                   nova_ui_window_id_t *window);
nova_ui_result_t nova_shell_window_command(nova_shell_t *shell,
                                           nova_ui_window_id_t window,
                                           nova_shell_window_command_t command);
nova_ui_result_t nova_shell_window_move(nova_shell_t *shell,
                                        nova_ui_window_id_t window,
                                        nova_ui_dlu_t x_dlu,
                                        nova_ui_dlu_t y_dlu);
const nova_shell_window_t *nova_shell_find_window(const nova_shell_t *shell,
                                                  nova_ui_window_id_t id);

#endif
