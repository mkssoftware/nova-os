#ifndef NOVA_BOOTMENU_UI_H
#define NOVA_BOOTMENU_UI_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    NOVA_VIEW_MAIN,
    NOVA_VIEW_SETTINGS,
    NOVA_VIEW_DIAGNOSTICS,
    NOVA_VIEW_RECOVERY,
    NOVA_VIEW_POWER,
    NOVA_VIEW_HELP,
    NOVA_VIEW_FIRMWARE
} nova_bootmenu_view_t;

bool bootmenu_initialize(void);
void bootmenu_set_view(nova_bootmenu_view_t view);
nova_bootmenu_view_t bootmenu_view(void);
uint16_t bootmenu_item_count(void);
bool bootmenu_item_available(uint16_t index);
void bootmenu_set_status(const char *text);
void bootmenu_draw(uint64_t selection, uint8_t opacity);
void bootmenu_set_transition(int32_t offset_dlu, uint8_t opacity, bool input_locked);
void bootmenu_set_dialog_motion(uint8_t opacity, uint16_t scale_per_mille);
bool bootmenu_transition_input_locked(void);
bool bootmenu_settings_toggle_tooltips(void);
bool bootmenu_settings_adjust_tooltip_delay(int32_t steps);
bool bootmenu_settings_set_tooltip_delay_edge(bool maximum);
bool bootmenu_help_search_begin(void);
bool bootmenu_help_search_active(void);
bool bootmenu_help_search_input(uint16_t scan_code,uint32_t unicode);
bool bootmenu_breadcrumb_focus(bool focused);
bool bootmenu_breadcrumb_focused(void);
bool bootmenu_breadcrumb_take_root_request(void);
bool bootmenu_scroll_selection_into_view(uint64_t selection);
bool bootmenu_scroll_key(uint16_t scan_code,uint64_t *selection);
bool bootmenu_scroll_wheel(int32_t wheel,uint64_t *selection);
const char *bootmenu_help_result_title(uint16_t selection);
const char *bootmenu_help_result_details(uint16_t selection);
void bootmenu_set_firmware_info(const char *vendor,uint32_t revision,
    bool secure_known,bool secure_boot,bool setup_known,bool setup_mode,
    bool setup_supported);
bool bootmenu_pointer_event(int32_t dx, int32_t dy, bool left, bool right,
                            uint64_t *selection, bool *activate);
bool bootmenu_context_open(uint64_t selection);
bool bootmenu_context_active(void);
bool bootmenu_context_move(int8_t direction);
bool bootmenu_context_activate(uint8_t *action);
void bootmenu_context_close(void);
bool bootmenu_tick(uint32_t elapsed_ms);
void bootmenu_tooltip_hide(void);

#endif
