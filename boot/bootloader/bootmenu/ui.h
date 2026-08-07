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
    NOVA_VIEW_HELP
} nova_bootmenu_view_t;

bool bootmenu_initialize(void);
void bootmenu_set_view(nova_bootmenu_view_t view);
nova_bootmenu_view_t bootmenu_view(void);
uint16_t bootmenu_item_count(void);
void bootmenu_set_status(const char *text);
void bootmenu_draw(uint64_t selection, uint8_t opacity);
void bootmenu_set_transition(int32_t offset_dlu, uint8_t opacity, bool input_locked);
void bootmenu_set_dialog_motion(uint8_t opacity, uint16_t scale_per_mille);
bool bootmenu_transition_input_locked(void);
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
