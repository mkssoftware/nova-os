#include "../uefi/uefi_min.h"
#include "compositor.h"
#include "controls.h"
#include "text.h"
#include "unicode.h"
#include "resources.h"
#include "icons.h"
#include "input.h"
#include "diagnostics.h"
#include "ui.h"
#include "branding.h"
#include "theme.h"
#include "layout.h"
#include "dialog.h"
#include "page.h"

UINTN grafik_width(void);
UINTN grafik_height(void);

static nova_surface_t *base_surface;
static nova_surface_t *interaction_surface;
static nova_surface_t *dialog_surface;
static bool initialized;
static nova_control_t *menu_list;
static nova_control_t *menu_items[6];
static nova_control_t *settings_accessories[4];
static nova_control_t *context_menu;
static nova_control_t *context_items[3];
static nova_control_t *tooltip_control;
static nova_control_t *breadcrumb_control;
static int32_t pointer_x;
static int32_t pointer_y;
static bool pointer_left;
static bool pointer_right;
static bool context_visible;
static uint8_t context_focus;
static uint16_t context_target;
static uint32_t tooltip_elapsed_ms;
static bool tooltip_visible;
static uint16_t tooltip_selection=0xffffu;
static nova_bootmenu_view_t tooltip_view=NOVA_VIEW_MAIN;
static const char *const context_text[3] = {
    "Details anzeigen", "Erweiterte Optionen", "Schließen"
};
static const char *const main_tooltips[6]={
    "NovaOS mit bevorzugtem Kernel starten.",
    "Installation ist noch nicht verfügbar.",
    "Darstellung und Bedienung ändern.",
    "Nur-lesende Prüfungen öffnen.",
    "Reparatur und Recovery öffnen.",
    "Ausschalten oder neu starten."
};
static nova_bootmenu_view_t current_view;
static nova_boot_layout_t current_layout;
static const char *status_text = "";
static int32_t transition_offset_dlu;
static uint8_t transition_opacity = 255;
static bool transition_input_locked;
static uint8_t dialog_opacity=255;
static uint16_t dialog_scale=1000;
static nova_page_t *view_pages[6];
static nova_view_t *view_content[6];
static nova_page_t *dialog_page;
static uint32_t dialog_page_id;
static const char *const main_text[6] = {
    "NovaOS starten", "NovaOS installieren", "Einstellungen",
    "Diagnose", "Recovery", "Ausschalten"
};
static const char *const settings_text[6] = {
    "Darstellung", "Barrierefreiheit", "Tastatur", "Sprache",
    "Firmware-Einstellungen", "Zurück"
};
static const char *const diagnostics_text[6] = {
    "Übersicht", "Hardware", "Boot", "Speicher", "Ereignisse", "Zurück"
};
static const char *const recovery_text[6] = {
    "Automatische Reparatur", "Integrität prüfen", "Snapshot wiederherstellen",
    "Speicherdiagnose", "Datenträgerdiagnose", "Zurück"
};
static const char *const power_text[6] = {
    "Herunterfahren", "Neustarten", "Neustart in Recovery",
    "Firmware-Setup", "Netzwerk-Boot", "Abbrechen"
};
static const char *const help_text[6] = {
    "Pfeiltasten: Auswahl ändern", "Enter: Auswahl öffnen",
    "Esc: zurück oder schließen", "F1: kontextbezogene Hilfe",
    "Maus: zeigen und auswählen", "Schließen"
};
static const nova_icon_token_t menu_icons[6] = {
    NOVA_ICON_HOME, NOVA_ICON_INSTALL, NOVA_ICON_SETTINGS,
    NOVA_ICON_DIAGNOSTICS, NOVA_ICON_RECOVERY, NOVA_ICON_POWER
};
static nova_rect_t dialog_bounds(void);
static nova_rect_t dialog_button_bounds(const nova_dialog_t *dialog, uint8_t index);
static nova_rect_t context_bounds(void);

static void rounded_panel(nova_surface_t *surface, int32_t x, int32_t y,
                          int32_t width, int32_t height, uint32_t color)
{
    if (width <= 0 || height <= 0) return;
    int32_t radius = nova_theme_tokens()->radius_medium;
    if (radius * 2 > height) radius = height / 2;
    if (radius * 2 > width) radius = width / 2;
    for (int32_t row = 0; row < height; ++row) {
        int32_t edge = row < radius ? row : height - 1 - row;
        int32_t inset = 0;
        if (edge < radius) {
            int32_t dy = radius - 1 - edge;
            int32_t extent = 0;
            while ((extent + 1) * (extent + 1) + dy * dy <= radius * radius)
                ++extent;
            inset = radius - extent;
        }
        nova_surface_rect(surface, (nova_rect_t){x + inset, y + row,
                          width - inset * 2, 1}, color);
    }
}

static bool create_page_model(nova_bootmenu_view_t page_id,const char *title)
{
    nova_page_t *page=nova_page_create((uint16_t)page_id,title,(uint32_t)page_id,false);
    uint16_t base=(uint16_t)(1000u+(uint16_t)page_id*10u);
    nova_view_t *root=nova_view_create(page,base,NOVA_VIEW_ROOT,title,1,false);
    nova_view_t *header=nova_view_create(page,(uint16_t)(base+1),NOVA_VIEW_HEADER,"Kopfbereich",2,false);
    nova_view_t *content=nova_view_create(page,(uint16_t)(base+2),NOVA_VIEW_CONTENT,"Menüinhalt",3,true);
    nova_view_t *footer=nova_view_create(page,(uint16_t)(base+3),NOVA_VIEW_FOOTER,"Statusbereich",4,false);
    if(!page||!root||!header||!content||!footer||!nova_page_set_root(page,root)||
       !nova_view_add_child(root,header)||!nova_view_add_child(root,content)||
       !nova_view_add_child(root,footer))return false;
    nova_view_t *all[4]={root,header,content,footer};
    for(uint8_t i=0;i<4;++i)if(!nova_view_initialize(all[i])||
        !nova_view_set_visibility(all[i],NOVA_VISIBILITY_VISIBLE))return false;
    if(!nova_page_load(page))return false;
    view_pages[page_id]=page;view_content[page_id]=content;return true;
}

static bool create_dialog_page(uint32_t id)
{
    uint16_t page_id=(uint16_t)(0x8000u+(id&0x3fffu));
    uint16_t root_id=(uint16_t)(0xc000u+((id*2u)&0x1ffeu));
    nova_page_t *page=nova_page_create(page_id,"Modaler Dialog",id,true);
    nova_view_t *root=nova_view_create(page,root_id,NOVA_VIEW_ROOT,"Modaler Dialog",5,false);
    nova_view_t *content=nova_view_create(page,(uint16_t)(root_id+1),NOVA_VIEW_DIALOG_CONTENT,
                                          "Dialoginhalt",6,true);
    if(!page||!root||!content||!nova_page_set_root(page,root)||
       !nova_view_add_child(root,content)||!nova_view_initialize(root)||
       !nova_view_initialize(content)||!nova_view_set_visibility(root,NOVA_VISIBILITY_VISIBLE)||
       !nova_view_set_visibility(content,NOVA_VISIBILITY_VISIBLE)||!nova_page_load(page)||
       !nova_page_activate(page)||!nova_page_set_focus(page,content))return false;
    dialog_page=page;dialog_page_id=id;nova_debug_string("UEFI:DIALOG-PAGE-ACTIVE\n");return true;
}

static void sync_active_page(void)
{
    nova_dialog_t *dialog=nova_dialog_active();
    if(dialog){if(!dialog_page||dialog_page_id!=dialog->id){
        if(dialog_page)nova_page_close(dialog_page);
        create_dialog_page(dialog->id);
    }}else{
        if(dialog_page){nova_page_close(dialog_page);dialog_page=0;dialog_page_id=0;}
        nova_page_t *target=view_pages[current_view];
        if(target&&nova_page_active()!=target&&nova_page_activate(target)){
            nova_page_set_focus(target,view_content[current_view]);
            nova_debug_string("UEFI:PAGE-RESTORED\n");
        }
    }
}

bool bootmenu_initialize(void)
{
    if (!nova_compositor_initialize((uint32_t)grafik_width(),
                                    (uint32_t)grafik_height())) return false;
    base_surface = nova_surface_acquire();
    interaction_surface = nova_surface_acquire();
    dialog_surface = nova_surface_acquire();
    initialized = base_surface && interaction_surface && dialog_surface;
    if (initialized) {
        nova_resource_manager_initialize();
        if (!nova_theme_initialize()) return false;
        nova_layout_initialize();
        if (!nova_layout_compute((uint32_t)grafik_width(), (uint32_t)grafik_height(),
                                 false, &current_layout)) return false;
        nova_page_model_initialize();
        static const char *const page_titles[6]={"NovaOS Bootmanager","Einstellungen",
            "Boot-Diagnose","Recovery","Ausschalten / Neustarten","Hilfe"};
        for(uint16_t i=0;i<6;++i)if(!create_page_model((nova_bootmenu_view_t)i,page_titles[i]))return false;
        if(!nova_page_activate(view_pages[NOVA_VIEW_MAIN])||
           !nova_page_set_focus(view_pages[NOVA_VIEW_MAIN],view_content[NOVA_VIEW_MAIN]))return false;
        nova_debug_string("UEFI:PAGES-READY\n");
        nova_debug_string("UEFI:LAYOUT-READY\n");
        const nova_theme_tokens_t *initial_theme = nova_theme_tokens();
        nova_control_style_t style = {
            0x00000000u, initial_theme->text_primary, initial_theme->focus,
            initial_theme->disabled, initial_theme->radius_medium, 1,
            initial_theme->spacing_small
        };
        nova_controls_initialize(&style);
        nova_unicode_initialize();
        if (!nova_text_register_font_resource() ||
            !nova_resource_load(nova_resource_id("boot://fonts/segoe-ui/semibold/15")))
            return false;
        if (!nova_icons_initialize()) return false;
        if (!nova_branding_initialize()) return false;
        nova_debug_string("UEFI:BRANDING-READY\n");
        nova_debug_string("UEFI:RESOURCES-READY\n");
        menu_list = nova_control_create(NOVA_CONTROL_LIST);
        if (!menu_list) return false;
        nova_control_set_state(menu_list, NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(menu_list, NOVA_CONTROL_VISIBLE);
        for (uint16_t i = 0; i < 6; ++i) {
            menu_items[i] = nova_control_create(NOVA_CONTROL_LIST_ITEM);
            if (!menu_items[i] || !nova_control_set_parent(menu_items[i], menu_list)) return false;
            menu_items[i]->action = i;
            nova_control_set_state(menu_items[i], NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(menu_items[i], NOVA_CONTROL_VISIBLE);
            nova_control_set_text(menu_items[i], main_text[i]);
            nova_control_set_accessibility(menu_items[i], 1, main_text[i], false);
        }
        for (uint16_t i = 0; i < 4; ++i) {
            settings_accessories[i] = nova_control_create(i == 1 ? NOVA_CONTROL_SWITCH :
                                                                   NOVA_CONTROL_STATUS_BADGE);
            if (!settings_accessories[i]) return false;
            nova_control_set_state(settings_accessories[i], NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(settings_accessories[i], NOVA_CONTROL_VISIBLE);
            nova_control_set_flags(settings_accessories[i], NOVA_CONTROL_FLAG_VISIBLE |
                                                           NOVA_CONTROL_FLAG_READONLY);
        }
        context_menu=nova_control_create(NOVA_CONTROL_CONTEXT_MENU);
        if(!context_menu)return false;
        nova_control_set_state(context_menu,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(context_menu,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(context_menu,NOVA_CONTROL_FLAG_VISIBLE |
                                            NOVA_CONTROL_FLAG_MODAL);
        for(uint16_t i=0;i<3;++i){
            context_items[i]=nova_control_create(NOVA_CONTROL_LIST_ITEM);
            if(!context_items[i]||!nova_control_set_parent(context_items[i],context_menu))
                return false;
            context_items[i]->action=i;
            nova_control_set_state(context_items[i],NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(context_items[i],NOVA_CONTROL_VISIBLE);
            nova_control_set_text(context_items[i],context_text[i]);
            nova_control_set_accessibility(context_items[i],2,context_text[i],false);
        }
        tooltip_control=nova_control_create(NOVA_CONTROL_TOOLTIP);
        breadcrumb_control=nova_control_create(NOVA_CONTROL_BREADCRUMB);
        if(!tooltip_control||!breadcrumb_control)return false;
        nova_control_set_state(tooltip_control,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(tooltip_control,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(tooltip_control,NOVA_CONTROL_FLAG_VISIBLE |
                                               NOVA_CONTROL_FLAG_DECORATIVE |
                                               NOVA_CONTROL_FLAG_READONLY);
        nova_control_set_state(breadcrumb_control,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(breadcrumb_control,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(breadcrumb_control,NOVA_CONTROL_FLAG_VISIBLE |
                                                  NOVA_CONTROL_FLAG_READONLY);
        nova_control_set_accessibility(breadcrumb_control,3,"Navigationspfad",false);
        nova_input_initialize();
        nova_input_device_set(1, NOVA_DEVICE_KEYBOARD, true);
        nova_input_device_set(2, NOVA_DEVICE_MOUSE, true);
        for (uint16_t i = 0; i < 6; ++i) {
            nova_input_neighbor_set(menu_items[i], NOVA_DIRECTION_UP,
                                    menu_items[i ? i - 1 : 5]);
            nova_input_neighbor_set(menu_items[i], NOVA_DIRECTION_DOWN,
                                    menu_items[(i + 1) % 6]);
        }
        nova_input_focus_set(menu_items[0]);
        pointer_x = current_layout.list.x + current_layout.list.width / 2;
        pointer_y = current_layout.list.y + current_layout.item_height / 2;
        nova_diag_initialize();
        nova_debug_string("UEFI:INPUT-READY\n");
        nova_debug_string("UEFI:CONTROLS-READY\n");
    }
    if (initialized) nova_debug_string("UEFI:COMPOSITOR-READY\n");
    return initialized;
}

static const char *const *view_items(void)
{
    if (current_view == NOVA_VIEW_SETTINGS) return settings_text;
    if (current_view == NOVA_VIEW_DIAGNOSTICS) return diagnostics_text;
    if (current_view == NOVA_VIEW_RECOVERY) return recovery_text;
    if (current_view == NOVA_VIEW_POWER) return power_text;
    if (current_view == NOVA_VIEW_HELP) return help_text;
    return main_text;
}

void bootmenu_set_view(nova_bootmenu_view_t view)
{
    if(view>NOVA_VIEW_HELP)return;
    current_view = view;
    if(!dialog_page&&nova_page_active()!=view_pages[view]){
        nova_page_activate(view_pages[view]);
        nova_page_set_focus(view_pages[view],view_content[view]);
        nova_debug_string("UEFI:PAGE-ACTIVE\n");
    }
    const char *const *items = view_items();
    for (uint16_t i = 0; i < 6; ++i) {
        nova_control_set_text(menu_items[i], items[i]);
        nova_control_set_accessibility(menu_items[i], 1, items[i], false);
        nova_control_release(menu_items[i]);
    }
    nova_input_focus_set(menu_items[0]);
    bootmenu_tooltip_hide();
}

nova_bootmenu_view_t bootmenu_view(void) { return current_view; }
uint16_t bootmenu_item_count(void) { return 6; }
void bootmenu_set_status(const char *text) { status_text = text ? text : ""; }
void bootmenu_set_transition(int32_t offset_dlu, uint8_t opacity, bool input_locked)
{
    transition_offset_dlu=offset_dlu;
    transition_opacity=opacity;
    transition_input_locked=input_locked;
}
bool bootmenu_transition_input_locked(void) { return transition_input_locked; }
void bootmenu_set_dialog_motion(uint8_t opacity,uint16_t scale_per_mille)
{
    dialog_opacity=opacity;
    if(scale_per_mille<900)scale_per_mille=900;
    if(scale_per_mille>1000)scale_per_mille=1000;
    dialog_scale=scale_per_mille;
}

bool bootmenu_context_open(uint64_t selection)
{
    if(!initialized||nova_dialog_active()||selection>=bootmenu_item_count())return false;
    bootmenu_tooltip_hide();
    context_visible=true;context_focus=0;context_target=(uint16_t)selection;
    nova_input_focus_set(context_items[0]);
    nova_debug_string("UEFI:CONTEXT-OPEN\n");
    return true;
}

bool bootmenu_context_active(void){return context_visible;}

bool bootmenu_context_move(int8_t direction)
{
    if(!context_visible||!direction)return false;
    context_focus=(uint8_t)((context_focus+(direction>0?1:2))%3);
    nova_input_focus_set(context_items[context_focus]);
    return true;
}

bool bootmenu_context_activate(uint8_t *action)
{
    if(!context_visible||!action)return false;
    *action=context_focus;context_visible=false;
    nova_debug_string("UEFI:CONTEXT-ACTION\n");
    return true;
}

void bootmenu_context_close(void)
{
    if(context_visible)nova_debug_string("UEFI:CONTEXT-CLOSE\n");
    context_visible=false;
}

void bootmenu_tooltip_hide(void)
{
    tooltip_visible=false;tooltip_elapsed_ms=0;
}

bool bootmenu_tick(uint32_t elapsed_ms)
{
    if(!initialized||context_visible||nova_dialog_active()||transition_input_locked||
       tooltip_selection>=bootmenu_item_count())return false;
    if(tooltip_visible)return false;
    if(elapsed_ms>1000u)elapsed_ms=1000u;
    if(tooltip_elapsed_ms<750u)tooltip_elapsed_ms+=elapsed_ms;
    if(tooltip_elapsed_ms>=750u){tooltip_visible=true;
        nova_debug_string("UEFI:TOOLTIP-SHOW\n");return true;}
    return false;
}

static const char *view_title(void)
{
    if (current_view == NOVA_VIEW_SETTINGS) return "Einstellungen";
    if (current_view == NOVA_VIEW_DIAGNOSTICS) return "Boot-Diagnose";
    if (current_view == NOVA_VIEW_RECOVERY) return "Recovery";
    if (current_view == NOVA_VIEW_POWER) return "Ausschalten / Neustarten";
    if (current_view == NOVA_VIEW_HELP) return "Hilfe";
    return "NovaOS Bootmanager";
}

bool bootmenu_pointer_event(int32_t dx, int32_t dy, bool left, bool right,
                            UINTN *selection, bool *activate)
{
    if (!initialized || !selection || !activate) return false;
    if (transition_input_locked) return false;
    bootmenu_tooltip_hide();
    int32_t width = (int32_t)grafik_width();
    int32_t height = (int32_t)grafik_height();
    int32_t step_x = dx / 8;
    int32_t step_y = dy / 8;
    if (dx && !step_x) step_x = dx < 0 ? -1 : 1;
    if (dy && !step_y) step_y = dy < 0 ? -1 : 1;
    pointer_x += step_x;
    pointer_y += step_y;
    if (pointer_x < 0) pointer_x = 0;
    if (pointer_y < 0) pointer_y = 0;
    if (pointer_x >= width) pointer_x = width - 1;
    if (pointer_y >= height) pointer_y = height - 1;

    nova_dialog_t *dialog = nova_dialog_active();
    if (dialog) {
        for (uint8_t i = 0; i < dialog->button_count; ++i) {
            nova_rect_t bounds = dialog_button_bounds(dialog, i);
            if (pointer_x >= bounds.x && pointer_y >= bounds.y &&
                pointer_x < bounds.x + bounds.width &&
                pointer_y < bounds.y + bounds.height) {
                while (dialog->focus != i) nova_dialog_focus_move(1);
                if (left && !pointer_left) *activate = true;
                pointer_left = left;
                nova_debug_string("UEFI:DIALOG-POINTER\n");
                return true;
            }
        }
        pointer_left = left;
        pointer_right = right;
        return true;
    }

    if(right&&!pointer_right&&!context_visible){
        nova_control_t *hover=nova_control_hit_test(pointer_x,pointer_y);
        if(hover&&hover->type==NOVA_CONTROL_LIST_ITEM)*selection=hover->action;
        bootmenu_context_open(*selection);
    }
    pointer_right=right;
    if(context_visible){
        nova_rect_t bounds=context_bounds();
        int32_t row_h=bounds.height/3;
        bool inside=false;
        for(uint8_t i=0;i<3;++i)if(pointer_x>=bounds.x&&pointer_x<bounds.x+bounds.width&&
           pointer_y>=bounds.y+i*row_h&&pointer_y<bounds.y+(i+1)*row_h){
            inside=true;
            context_focus=i;nova_input_focus_set(context_items[i]);
            if(left&&!pointer_left)*activate=true;
        }
        if(left&&!pointer_left&&!inside)bootmenu_context_close();
        pointer_left=left;
        return true;
    }

    nova_input_event_t posted = {0};
    posted.device_id = 2;
    posted.type = left && !pointer_left ? NOVA_EVENT_POINTER_DOWN :
                  (!left && pointer_left ? NOVA_EVENT_POINTER_UP : NOVA_EVENT_POINTER_MOVE);
    posted.pointer.pointer_id = 0;
    posted.pointer.buttons = left ? 1u : 0u;
    posted.pointer.x = pointer_x;
    posted.pointer.y = pointer_y;
    if (!nova_input_post(posted)) return false;
    nova_debug_string("UEFI:POINTER-EVENT\n");
    nova_input_event_t event;
    if (!nova_input_next(&event)) return false;
    nova_control_t *target = nova_input_pointer_target(&event);
    if (target && target->type == NOVA_CONTROL_LIST_ITEM) {
        *selection = target->action;
        nova_input_focus_set(target);
        if (event.type == NOVA_EVENT_POINTER_DOWN) {
            uint32_t action;
            if (nova_control_invoke(target, &action)) {
                *activate = true;
                nova_debug_string("UEFI:POINTER-ACTION\n");
            }
        } else if (event.type == NOVA_EVENT_POINTER_UP) {
            nova_control_release(target);
        }
    }
    pointer_left = left;
    return true;
}

static void draw_pointer(nova_surface_t *surface,int32_t offset_x,int32_t offset_y)
{
    /* Kleine, alpha-faehige Pfeilform; zentrale Darstellung im Interaction-Layer. */
    uint32_t pointer_color = nova_theme_tokens()->focus;
    for (int32_t row = 0; row < 14; ++row) {
        int32_t span = row < 9 ? row / 2 + 1 : 3;
        nova_surface_rect(surface, (nova_rect_t){pointer_x-offset_x, pointer_y-offset_y+row, span, 1},
                          pointer_color);
    }
    nova_surface_rect(surface, (nova_rect_t){pointer_x-offset_x+3, pointer_y-offset_y+9, 2, 7},
                      pointer_color);
}

static nova_rect_t dialog_bounds(void)
{
    int32_t width = nova_dlu_to_pixels(460, current_layout.scale_milli);
    int32_t height = nova_dlu_to_pixels(230, current_layout.scale_milli);
    if (width > current_layout.safe.width - 32) width = current_layout.safe.width - 32;
    if (height > current_layout.safe.height - 32) height = current_layout.safe.height - 32;
    width=(int32_t)((int64_t)width*dialog_scale/1000);
    height=(int32_t)((int64_t)height*dialog_scale/1000);
    if(width>(int32_t)NOVA_DIALOG_SURFACE_WIDTH)width=NOVA_DIALOG_SURFACE_WIDTH;
    if(height>(int32_t)NOVA_DIALOG_SURFACE_HEIGHT)height=NOVA_DIALOG_SURFACE_HEIGHT;
    return (nova_rect_t){
        current_layout.safe.x + (current_layout.safe.width - width) / 2,
        current_layout.safe.y + (current_layout.safe.height - height) / 2,
        width, height
    };
}

static nova_rect_t dialog_button_bounds(const nova_dialog_t *dialog, uint8_t index)
{
    nova_rect_t card = dialog_bounds();
    int32_t gap = nova_dlu_to_pixels(8, current_layout.scale_milli);
    int32_t padding = nova_dlu_to_pixels(20, current_layout.scale_milli);
    int32_t height = nova_dlu_to_pixels(42, current_layout.scale_milli);
    int32_t available = card.width - padding * 2 - gap * (dialog->button_count - 1);
    int32_t width = available / dialog->button_count;
    return (nova_rect_t){card.x + padding + index * (width + gap),
                         card.y + card.height - padding - height, width, height};
}

static nova_rect_t context_bounds(void)
{
    int32_t row_h=nova_dlu_to_pixels(40,current_layout.scale_milli);
    int32_t width=nova_dlu_to_pixels(250,current_layout.scale_milli);
    int32_t height=row_h*3;
    int32_t target_y=current_layout.list.y+context_target*
                     (current_layout.item_height+current_layout.item_gap);
    int32_t x=current_layout.list.x+current_layout.list.width-width-
              current_layout.item_padding;
    int32_t y=target_y+current_layout.item_height/2;
    if(x<current_layout.safe.x)x=current_layout.safe.x;
    if(y+height>current_layout.safe.y+current_layout.safe.height)
        y=current_layout.safe.y+current_layout.safe.height-height;
    if(y<current_layout.safe.y)y=current_layout.safe.y;
    return (nova_rect_t){x,y,width,height};
}

static void draw_context_menu(nova_surface_t *surface)
{
    if(!context_visible)return;
    const nova_theme_tokens_t *theme=nova_theme_tokens();
    nova_rect_t bounds=context_bounds();
    int32_t row_h=bounds.height/3;
    rounded_panel(surface,bounds.x-2,bounds.y-2,bounds.width+4,bounds.height+4,
                  theme->border);
    rounded_panel(surface,bounds.x,bounds.y,bounds.width,bounds.height,
                  theme->surface_secondary);
    nova_control_set_bounds(context_menu,bounds);
    for(uint8_t i=0;i<3;++i){
        nova_control_t *item=context_items[i];
        nova_control_set_bounds(item,(nova_rect_t){bounds.x,bounds.y+i*row_h,
                                                   bounds.width,row_h});
        item->style.background=i==context_focus?theme->selected:0x00000000u;
        item->style.accent=theme->focus;
        item->style.foreground=theme->text_primary;
        item->style.corner_dlu=theme->radius_small;
        item->flags&=~(NOVA_CONTROL_FLAG_FOCUSED|NOVA_CONTROL_FLAG_SELECTED);
        if(i==context_focus)item->flags|=NOVA_CONTROL_FLAG_SELECTED;
        nova_control_render(item,surface);
        int32_t pad=nova_dlu_to_pixels(14,current_layout.scale_milli);
        nova_text_draw_scaled(surface,bounds.x+pad,
            bounds.y+i*row_h+(row_h-nova_dlu_to_pixels(20,current_layout.scale_milli))/2,
            bounds.width-pad*2,item->text,theme->text_primary,NOVA_TEXT_LEFT,true,
            current_layout.scale_milli);
    }
    nova_debug_string("UEFI:CONTEXT-FRAME-READY\n");
}

static void draw_tooltip(nova_surface_t *surface)
{
    if(!tooltip_visible||context_visible||nova_dialog_active()||
       tooltip_selection>=bootmenu_item_count())return;
    const nova_theme_tokens_t *theme=nova_theme_tokens();
    int32_t width=nova_dlu_to_pixels(370,current_layout.scale_milli);
    int32_t height=nova_dlu_to_pixels(42,current_layout.scale_milli);
    int32_t item_y=current_layout.list.y+tooltip_selection*
        (current_layout.item_height+current_layout.item_gap);
    int32_t x=current_layout.list.x+current_layout.item_padding;
    int32_t y=item_y+current_layout.item_height+current_layout.item_gap/2;
    if(x+width>current_layout.safe.x+current_layout.safe.width)
        x=current_layout.safe.x+current_layout.safe.width-width;
    if(y+height>current_layout.safe.y+current_layout.safe.height)
        y=item_y-height-current_layout.item_gap/2;
    nova_control_set_bounds(tooltip_control,(nova_rect_t){x,y,width,height});
    rounded_panel(surface,x-1,y-1,width+2,height+2,theme->border);
    rounded_panel(surface,x,y,width,height,theme->surface_secondary);
    int32_t pad=nova_dlu_to_pixels(12,current_layout.scale_milli);
    nova_text_draw_scaled(surface,x+pad,
        y+(height-nova_dlu_to_pixels(20,current_layout.scale_milli))/2,
        width-pad*2,tooltip_control->text,theme->text_primary,NOVA_TEXT_LEFT,true,
        current_layout.scale_milli);
    nova_debug_string("UEFI:TOOLTIP-FRAME-READY\n");
}

static void draw_dialog(nova_surface_t *surface,nova_rect_t card)
{
    nova_dialog_t *dialog = nova_dialog_active();
    if (!dialog) return;
    nova_debug_string("UEFI:DIALOG-TITLE=");nova_debug_string(dialog->title);
    nova_debug_string("\nUEFI:DIALOG-MESSAGE=");nova_debug_string(dialog->message);
    nova_debug_string("\n");
    const nova_theme_tokens_t *theme = nova_theme_tokens();
    rounded_panel(surface, card.x - 2, card.y - 2, card.width + 4, card.height + 4,
                  dialog->type == NOVA_DIALOG_ERROR ? theme->error :
                  dialog->type == NOVA_DIALOG_WARNING ? theme->warning : theme->border);
    rounded_panel(surface, card.x, card.y, card.width, card.height,
                  theme->surface_secondary);
    int32_t padding = nova_dlu_to_pixels(20, current_layout.scale_milli);
    nova_text_draw_scaled(surface, card.x + padding, card.y + padding,
                          card.width - padding * 2, dialog->title,
                          theme->text_primary, NOVA_TEXT_LEFT, true,
                          current_layout.scale_milli);
    nova_text_draw_wrapped_scaled(surface, card.x + padding,
                          card.y + padding + nova_dlu_to_pixels(38,current_layout.scale_milli),
                          card.width - padding * 2, 3, dialog->message,
                          theme->text_secondary, current_layout.scale_milli);
    if (dialog->type == NOVA_DIALOG_PROGRESS) {
        int32_t bar_x = card.x + padding;
        int32_t bar_y = card.y + padding + nova_dlu_to_pixels(88,current_layout.scale_milli);
        int32_t bar_w = card.width - padding * 2;
        int32_t bar_h = nova_dlu_to_pixels(12,current_layout.scale_milli);
        rounded_panel(surface,bar_x,bar_y,bar_w,bar_h,theme->surface);
        int32_t fill = dialog->progress_indeterminate ? bar_w / 3 :
                       (bar_w * dialog->progress_per_mille) / 1000;
        int32_t offset = dialog->progress_indeterminate ?
                         (bar_w-fill) * dialog->progress_per_mille / 1000 : 0;
        if (fill > 0) rounded_panel(surface,bar_x+offset,bar_y,fill,bar_h,theme->accent);
        char percent[13];
        uint16_t value = dialog->progress_per_mille / 10;
        uint8_t length = 0;
        if (value >= 100) percent[length++]='1';
        if (value >= 10) percent[length++]=(char)('0'+(value/10)%10);
        percent[length++]=(char)('0'+value%10);
        static const char suffix[]=" Prozent";
        for (uint8_t i=0;suffix[i];++i) percent[length++]=suffix[i];
        percent[length]=0;
        nova_text_draw_scaled(surface,bar_x,bar_y+bar_h+6,bar_w,
            dialog->progress_indeterminate ? "Wird ausgeführt" : percent,
            theme->text_secondary,NOVA_TEXT_RIGHT,true,current_layout.scale_milli);
        if (dialog->progress_status[0])
            nova_text_draw_scaled(surface,bar_x,bar_y+bar_h+6,bar_w-80,
                dialog->progress_status,theme->text_secondary,NOVA_TEXT_LEFT,true,
                current_layout.scale_milli);
    } else if (dialog->type == NOVA_DIALOG_CREDENTIAL) {
        int32_t field_x=card.x+padding;
        int32_t field_y=card.y+padding+nova_dlu_to_pixels(82,current_layout.scale_milli);
        int32_t field_w=card.width-padding*2;
        int32_t field_h=nova_dlu_to_pixels(38,current_layout.scale_milli);
        rounded_panel(surface,field_x,field_y,field_w,field_h,theme->surface);
        int32_t dot=nova_dlu_to_pixels(5,current_layout.scale_milli);
        int32_t gap=nova_dlu_to_pixels(10,current_layout.scale_milli);
        uint16_t visible=(uint16_t)((field_w-16)/gap);
        if (visible > dialog->credential_glyphs) visible=dialog->credential_glyphs;
        for (uint16_t i=0;i<visible;++i)
            rounded_panel(surface,field_x+10+i*gap,field_y+(field_h-dot)/2,dot,dot,
                          theme->text_primary);
    }
    if (dialog->destructive_armed)
        nova_text_draw_scaled(surface, card.x + padding,
                              card.y + padding + nova_dlu_to_pixels(72,current_layout.scale_milli),
                              card.width - padding * 2,
                              "Zur Sicherheit erneut bestätigen.", theme->warning,
                              NOVA_TEXT_LEFT, true, current_layout.scale_milli);
    for (uint8_t i = 0; i < dialog->button_count; ++i) {
        int32_t gap=nova_dlu_to_pixels(8,current_layout.scale_milli);
        int32_t button_h=nova_dlu_to_pixels(42,current_layout.scale_milli);
        int32_t available=card.width-padding*2-gap*(dialog->button_count-1);
        int32_t button_w=available/dialog->button_count;
        nova_rect_t button={card.x+padding+i*(button_w+gap),
                            card.y+card.height-padding-button_h,button_w,button_h};
        rounded_panel(surface, button.x, button.y, button.width, button.height,
                      i == dialog->focus ? theme->selected : theme->surface);
        if (i == dialog->focus)
            nova_surface_rect(surface, (nova_rect_t){button.x,button.y,3,button.height},
                              theme->focus);
        nova_text_draw_scaled(surface, button.x + 8,
                              button.y + (button.height - nova_dlu_to_pixels(20,current_layout.scale_milli))/2,
                              button.width - 16, dialog->buttons[i], theme->text_primary,
                              NOVA_TEXT_CENTER, true, current_layout.scale_milli);
    }
}

void bootmenu_draw(UINTN selection, uint8_t opacity)
{
    if (!initialized) return;
    if(selection<bootmenu_item_count()&&
       (tooltip_selection!=(uint16_t)selection||tooltip_view!=current_view)){
        tooltip_selection=(uint16_t)selection;tooltip_view=current_view;
        bootmenu_tooltip_hide();
        nova_control_set_text(tooltip_control,current_view==NOVA_VIEW_MAIN?
                              main_tooltips[selection]:
                              "Enter öffnet diese Option. Esc kehrt zurück.");
    }
    sync_active_page();
    int32_t width = (int32_t)grafik_width();
    const nova_theme_tokens_t *theme = nova_theme_tokens();
    if (!nova_layout_compute((uint32_t)width, (uint32_t)grafik_height(),
                             theme->high_contrast, &current_layout)) return;
    nova_page_t *active_page=nova_page_active();
    if(active_page){
        nova_view_t *root=nova_page_root_view(active_page);
        if(root)nova_view_set_layout(root,current_layout.safe);
        if(!dialog_page&&view_content[current_view])
            nova_view_set_layout(view_content[current_view],current_layout.panel);
    }
    nova_surface_clear(base_surface, theme->background);
    if (theme->high_contrast)
        rounded_panel(base_surface, current_layout.panel.x - 2,
                      current_layout.panel.y - 2, current_layout.panel.width + 4,
                      current_layout.panel.height + 4, theme->border);
    rounded_panel(base_surface, current_layout.panel.x, current_layout.panel.y,
                  current_layout.panel.width, current_layout.panel.height, theme->surface);
    rounded_panel(base_surface, current_layout.top_bar.x, current_layout.top_bar.y,
                  current_layout.top_bar.width, current_layout.top_bar.height, theme->accent);
    nova_branding_draw(base_surface, current_layout.brand.x, current_layout.brand.y,
                       (uint16_t)current_layout.brand.width,
                       (uint16_t)current_layout.brand.height, theme->accent);
    int32_t brand_text_width = current_layout.brand.width +
                               (current_layout.brand_compact ? 32 : 0);
    nova_text_draw_scaled(base_surface, current_layout.brand.x,
                   current_layout.brand.y + current_layout.brand.height + 4,
                   brand_text_width, "NovaOS",
                   theme->accent, NOVA_TEXT_LEFT, true, current_layout.scale_milli);
    nova_control_set_bounds(breadcrumb_control,current_layout.title);
    if(current_view==NOVA_VIEW_MAIN){
        nova_control_set_text(breadcrumb_control,view_title());
        nova_text_draw_scaled(base_surface,current_layout.title.x,current_layout.title.y,
            current_layout.title.width,breadcrumb_control->text,theme->text_primary,
            NOVA_TEXT_LEFT,true,current_layout.scale_milli);
    }else{
        const char *root="Start";
        nova_control_set_text(breadcrumb_control,"Start / Unterseite");
        nova_text_draw_scaled(base_surface,current_layout.title.x,current_layout.title.y,
            current_layout.title.width,root,theme->text_secondary,NOVA_TEXT_LEFT,true,
            current_layout.scale_milli);
        int32_t root_width=nova_text_measure_scaled(root,current_layout.title.width,
                                                   current_layout.scale_milli).width;
        int32_t chevron_x=current_layout.title.x+root_width+
                          nova_dlu_to_pixels(8,current_layout.scale_milli);
        int32_t chevron_y=current_layout.title.y+
                          nova_dlu_to_pixels(6,current_layout.scale_milli);
        int32_t chevron_h=nova_dlu_to_pixels(8,current_layout.scale_milli);
        for(int32_t row=0;row<chevron_h;++row){
            int32_t half=chevron_h/2;
            int32_t offset=row<=half?row:chevron_h-1-row;
            nova_surface_rect(base_surface,(nova_rect_t){chevron_x+offset,chevron_y+row,2,1},
                              theme->text_secondary);
        }
        int32_t title_x=chevron_x+chevron_h+nova_dlu_to_pixels(8,current_layout.scale_milli);
        nova_text_draw_scaled(base_surface,title_x,
            current_layout.title.y,current_layout.title.x+current_layout.title.width-title_x,view_title(),
            theme->text_primary,NOVA_TEXT_LEFT,true,current_layout.scale_milli);
        nova_debug_string("UEFI:BREADCRUMB-FRAME-READY\n");
    }

    nova_surface_clear(interaction_surface, 0x00000000u);
    nova_surface_clear(dialog_surface,0x00000000u);
    for (uint16_t i = 0; i < 6; ++i) {
        nova_control_t *item = menu_items[i];
        int32_t item_y = current_layout.list.y +
                         i * (current_layout.item_height + current_layout.item_gap);
        nova_control_set_bounds(item, (nova_rect_t){current_layout.list.x, item_y,
                               current_layout.list.width, current_layout.item_height});
        item->style.background = i == selection ? theme->selected : 0x00000000u;
        item->style.foreground = theme->text_primary;
        item->style.accent = theme->focus;
        item->style.disabled = theme->disabled;
        item->style.corner_dlu = theme->radius_medium;
        item->flags &= ~(NOVA_CONTROL_FLAG_FOCUSED | NOVA_CONTROL_FLAG_SELECTED);
        if (i == selection) {
            item->flags |= NOVA_CONTROL_FLAG_SELECTED;
            nova_input_focus_set(item);
        }
        nova_control_render(item, interaction_surface);
        int32_t content_y = item_y + (current_layout.item_height - current_layout.icon_size) / 2;
        int32_t icon_x = current_layout.list.x + current_layout.item_padding;
        nova_icon_draw(interaction_surface, menu_icons[i], icon_x, content_y,
                       (uint16_t)current_layout.icon_size, theme->text_primary);
        int32_t text_x = icon_x + current_layout.icon_size + current_layout.text_gap;
        nova_text_draw_scaled(interaction_surface, text_x, content_y,
                       current_layout.list.x + current_layout.list.width - text_x -
                       current_layout.item_padding,
                       item->text, theme->text_primary, NOVA_TEXT_LEFT, true,
                       current_layout.scale_milli);
        if (current_view == NOVA_VIEW_SETTINGS && i < 4) {
            nova_control_t *accessory=settings_accessories[i];
            int32_t accessory_h=nova_dlu_to_pixels(i==1?22:24,current_layout.scale_milli);
            int32_t accessory_w=nova_dlu_to_pixels(i==1?44:92,current_layout.scale_milli);
            if(accessory_h>current_layout.item_height-8)accessory_h=current_layout.item_height-8;
            int32_t accessory_x=current_layout.list.x+current_layout.list.width-
                                current_layout.item_padding-accessory_w;
            int32_t accessory_y=item_y+(current_layout.item_height-accessory_h)/2;
            nova_control_set_bounds(accessory,(nova_rect_t){accessory_x,accessory_y,
                                     accessory_w,accessory_h});
            accessory->style.background=theme->surface_secondary;
            accessory->style.foreground=theme->text_primary;
            accessory->style.accent=theme->accent;
            accessory->style.disabled=theme->disabled;
            accessory->style.corner_dlu=(uint16_t)(accessory_h/2);
            if(i==1)nova_control_set_checked(accessory,nova_theme_reduced_motion());
            nova_control_render(accessory,interaction_surface);
            if(i!=1){
                const char *value=i==0?(nova_theme_active()==NOVA_THEME_LIGHT?"Light":
                    nova_theme_active()==NOVA_THEME_HIGH_CONTRAST?"Kontrast":"Dark"):
                    i==2?"Aktiv":"Deutsch";
                nova_text_draw_scaled(interaction_surface,accessory_x+6,
                    accessory_y+(accessory_h-nova_dlu_to_pixels(20,current_layout.scale_milli))/2,
                    accessory_w-12,value,theme->text_secondary,NOVA_TEXT_CENTER,true,
                    current_layout.scale_milli);
            }
        }
    }
    if (status_text[0]) {
        nova_text_draw_scaled(interaction_surface, current_layout.status.x,
                       current_layout.status.y, current_layout.status.width,
                       status_text, theme->text_secondary, NOVA_TEXT_LEFT, true,
                       current_layout.scale_milli);
    }
    draw_tooltip(interaction_surface);
    draw_context_menu(interaction_surface);
    if(nova_dialog_active()){
        nova_rect_t card=dialog_bounds();
        uint8_t backdrop=(uint8_t)((uint16_t)0x99u*dialog_opacity/255u);
        nova_surface_rect(interaction_surface,(nova_rect_t){0,0,width,(int32_t)grafik_height()},
                          (uint32_t)backdrop<<24);
        draw_dialog(dialog_surface,(nova_rect_t){0,0,card.width,card.height});
        if(pointer_x>=card.x&&pointer_y>=card.y&&pointer_x<card.x+card.width&&
           pointer_y<card.y+card.height)draw_pointer(dialog_surface,card.x,card.y);
        else draw_pointer(interaction_surface,0,0);
    }else draw_pointer(interaction_surface,0,0);

    nova_layer_t base = {
        1, base_surface, {0, 0, width, (int32_t)grafik_height()},
        {0, 0, width, (int32_t)grafik_height()}, 0, opacity,
        NOVA_MATERIAL_NONE, true, false, false, true
    };
    nova_layer_t interaction = {
        2, interaction_surface, {0, 0, width, (int32_t)grafik_height()},
        {0, 0, width, (int32_t)grafik_height()}, 10,
        (uint8_t)((uint16_t)opacity * transition_opacity / 255u),
        NOVA_MATERIAL_NONE, true, true, false, true
    };
    nova_rect_t active_dialog_bounds=dialog_bounds();
    nova_layer_t dialog_layer = {
        3,dialog_surface,{0,0,active_dialog_bounds.width,active_dialog_bounds.height},
        active_dialog_bounds,20,dialog_opacity,
        NOVA_MATERIAL_NONE,true,true,true,true
    };
    int32_t offset_px=nova_dlu_to_pixels(transition_offset_dlu,current_layout.scale_milli);
    if (offset_px) nova_debug_string("UEFI:NAV-TRANSFORMED-FRAME\n");
    if (offset_px > 0 && offset_px < width) {
        interaction.source.width=width-offset_px;
        interaction.destination.x=offset_px;
        interaction.destination.width=width-offset_px;
    } else if (offset_px < 0 && -offset_px < width) {
        interaction.source.x=-offset_px;
        interaction.source.width=width+offset_px;
        interaction.destination.width=width+offset_px;
    }
    nova_compositor_begin_frame();
    nova_compositor_submit_layer(&base);
    nova_compositor_submit_layer(&interaction);
    if(nova_dialog_active())nova_compositor_submit_layer(&dialog_layer);
    nova_compositor_compose();
    nova_debug_string("UEFI:MENU-DRAWN\n");
    if (nova_dialog_active()) nova_debug_string("UEFI:DIALOG-FRAME-READY\n");
}
