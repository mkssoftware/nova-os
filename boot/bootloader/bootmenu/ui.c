#include "../uefi/uefi_min.h"
#include "compositor.h"
#include "graphics.h"
#include "controls.h"
#include "text.h"
#include "font_resources.h"
#include "animation_resources.h"
#include "unicode.h"
#include "resources.h"
#include "resource_version.h"
#include "icons.h"
#include "input.h"
#include "diagnostics.h"
#include "recovery.h"
#include "memory.h"
#include "configuration.h"
#include "runtime.h"
#include "state_model.h"
#include "motion.h"
#include "ui.h"
#include "branding.h"
#include "rounded_geometry.h"
#include "effects.h"
#include "background_blur.h"
#include "image_renderer.h"
#include "render_quality.h"
#include "software_renderer.h"
#include "design.h"
#include "architecture.h"
#include "scene_graph.h"
#include "render_queue.h"
#include "surface_manager.h"
#include "layer_manager.h"
#include "present_scheduler.h"
#include "clip_mask.h"
#include "theme.h"
#include "layout.h"
#include "dialog.h"
#include "page.h"

static nova_surface_t *base_surface;
static nova_surface_t *interaction_surface;
static nova_surface_handle_t base_surface_handle,interaction_surface_handle;
static nova_layer_handle_t managed_layers[NOVA_LAYER_CUSTOM];
static bool initialized;
static bool base_cache_valid,base_dirty_requested=true,previous_complex_frame;
static bool first_present_pending=true,overlay_reset_requested=true;
static UINTN previous_selection=(UINTN)-1;
static uint8_t previous_frame_opacity=255;
static nova_bootmenu_view_t cached_base_view;
static nova_theme_id_t cached_base_theme=NOVA_THEME_COUNT;
static uint32_t cached_base_width,cached_base_height,cached_base_scale;
static uint32_t base_cache_generation;
static int32_t previous_pointer_x,previous_pointer_y;
static nova_control_t *menu_list;
static nova_control_t *menu_items[6];
static nova_control_t *recovery_tiles[6];
static nova_control_t *recovery_badges[6];
static nova_control_t *settings_accessories[4];
static nova_control_t *context_menu;
static nova_control_t *context_items[6];
static nova_control_t *tooltip_control;
static nova_control_t *breadcrumb_control;
static nova_control_t *breadcrumb_home_button;
static nova_rect_t breadcrumb_root_bounds;
static bool breadcrumb_root_focused;
static bool breadcrumb_root_hovered;
static bool breadcrumb_root_requested;
static nova_control_t *help_search_field;
static nova_control_t *dialog_spinner;
static nova_control_t *dialog_password_field;
static nova_control_t *dialog_buttons[NOVA_DIALOG_BUTTON_CAPACITY];
static nova_control_t *diagnostic_scroll_view;
static nova_control_t *diagnostic_scrollbar;
static nova_control_t *page_card;
static nova_control_t *brand_image;
static nova_control_t *brand_label;
static nova_control_t *status_label;
static nova_control_t *section_separator;
static nova_control_t *item_icons[6];
static bool diagnostic_scroll_dragging;
static uint32_t routed_menu_event_count;
static bool bootmenu_list_event(nova_control_t *control,nova_control_event_t *event,void *context)
{(void)control;(void)context;
 if(event->type==NOVA_CONTROL_EVENT_KEY_DOWN){
    nova_debug_string("UEFI:CONTROL-KEY-EVENT-BUBBLED\n");return false;}
 if(event->type>=NOVA_CONTROL_EVENT_MOUSE_ENTER&&event->type<=NOVA_CONTROL_EVENT_MOUSE_UP){
    nova_debug_string("UEFI:CONTROL-POINTER-EVENT-BUBBLED\n");return false;}
 if(event->type!=NOVA_CONTROL_EVENT_CLICK)return false;
 ++routed_menu_event_count;nova_debug_string("UEFI:CONTROL-EVENT-BUBBLED\n");return true;}
static int32_t pointer_x;
static int32_t pointer_y;
static bool pointer_left;
static bool pointer_right;
static nova_control_t *pointer_event_target;
static uint32_t pointer_clock_ms;
static bool context_visible;
static uint8_t context_focus;
static uint16_t context_target;
typedef enum { NOVA_CONTEXT_ACTIONS, NOVA_CONTEXT_THEMES,
    NOVA_CONTEXT_QUALITY } nova_context_kind_t;
static nova_context_kind_t context_kind;
static uint8_t context_count=3;
static uint32_t tooltip_elapsed_ms;
static bool tooltip_visible;
static bool tooltips_enabled=true;
static uint32_t tooltip_delay_ms=750;
static bool settings_slider_dragging;
static bool help_search_editing;
static uint16_t tooltip_selection=0xffffu;
static nova_bootmenu_view_t tooltip_view=NOVA_VIEW_MAIN;
static const char *const context_action_text[3] = {
    "Details anzeigen", "Erweiterte Optionen", "Schließen"
};
static const char *const context_theme_text[4] = {
    "Dark", "Light", "Hoher Kontrast", "GrafikqualitÃ¤t ..."
};
static const char *const context_quality_text[6] = {
    "Automatisch", "Ultra", "High", "Balanced", "Low", "Safe"
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
static bool memory_self_test_active;
static bool runtime_self_test_active;
static bool state_model_self_test_active;
static int32_t transition_offset_dlu;
static uint8_t transition_opacity = 255;
static bool transition_input_locked;
static uint8_t dialog_opacity=255;
static bool dialog_effects_enabled=true;
static uint16_t dialog_scale=1000;
static nova_page_t *view_pages[7];
static nova_view_t *view_content[7];
static nova_page_t *dialog_page;
static uint32_t dialog_page_id;
static const char *const main_text[6] = {
    "NovaOS starten", "NovaOS installieren", "Einstellungen",
    "Diagnose", "Recovery", "Ausschalten"
};
static const char *const settings_text[6] = {
    "Darstellung", "Reduzierte Bewegung", "Tooltips anzeigen", "Tooltip-Verzögerung",
    "Firmware-Einstellungen", "Zurück"
};
static const char *const diagnostics_text[6] = {
    "Übersicht", "Hardware", "Boot", "Speicher", "Ereignisse", "Zurück"
};
static const char *const recovery_text[6] = {
    "Automatische Reparatur", "Integrität prüfen", "Snapshot wiederherstellen",
    "Speicherdiagnose", "Datenträgerdiagnose", "Zurück"
};
static const char *const recovery_descriptions[6]={
    "Startdateien sicher prüfen", "Systemintegrität nur lesend prüfen",
    "Gesicherten Zustand auswählen", "Arbeitsspeicher untersuchen",
    "Laufwerke nur lesend prüfen", "Zum Hauptmenü zurückkehren"};
static const char *const recovery_status[6]={
    "Backend fehlt", "Nur Prüfung", "Keine Snapshots", "Verfügbar",
    "Nur Prüfung", "Navigation"};
static const char *const power_text[6] = {
    "Herunterfahren", "Neustarten", "Neustart in Recovery",
    "Firmware-Setup", "Netzwerk-Boot", "Abbrechen"
};
static char help_result_text[6][NOVA_CONTROL_TEXT_CAPACITY]={
    "Hilfe durchsuchen","NovaOS starten","Diagnose","Recovery","Tastatur","Schließen"};
static const char *const help_items[6]={help_result_text[0],help_result_text[1],
    help_result_text[2],help_result_text[3],help_result_text[4],help_result_text[5]};
typedef struct {const char *title;const char *keywords;const char *details;} help_entry_t;
static const help_entry_t help_entries[]={
    {"NovaOS starten","start kernel nki elf","Startet NovaOS mit dem bevorzugten geprüften NKI-Kernel; ELF bleibt ein kompatibler Fallback."},
    {"Installation","installieren medium","Das aktuelle Abbild enthält noch kein geprüftes Installationsmodul."},
    {"Einstellungen","theme kontrast tooltips bewegung","Hier lassen sich Darstellung, reduzierte Bewegung und Tooltips ändern."},
    {"Diagnose","diagnose hardware speicher boot","Diagnosen lesen Firmware-, Grafik-, Eingabe- und Speicherstatus ohne Systemdaten zu verändern."},
    {"Recovery","recovery reparatur snapshot","Recovery-Aktionen bleiben gesperrt, solange kein geprüftes NovaFS-Schreibbackend vorhanden ist."},
    {"Ausschalten","power shutdown neustart","Energieaktionen benötigen vor der Ausführung eine ausdrückliche Bestätigung."},
    {"Tastatur","keyboard tasten pfeile enter escape f1","Pfeiltasten wählen, Enter aktiviert, Escape geht zurück und F1 öffnet die Hilfe."},
    {"Maus","mouse pointer klicken","Zeigen wählt einen Eintrag; Linksklick aktiviert und Rechtsklick öffnet das Kontextmenü."}
};
static int8_t help_result_indices[4]={0,3,4,6};
static char firmware_text[6][NOVA_CONTROL_TEXT_CAPACITY]={
    "Firmwaremodus: UEFI","Hersteller: Unbekannt","Secure Boot: nicht verfügbar",
    "Setup Mode: nicht verfügbar","Firmware-Setup nicht verfügbar","Zurück"};
static const char *const firmware_items[6]={firmware_text[0],firmware_text[1],
    firmware_text[2],firmware_text[3],firmware_text[4],firmware_text[5]};
static bool firmware_setup_available;
enum { NOVA_UI_STYLE_BADGE=1,NOVA_UI_STYLE_LIST_ITEM=2,
       NOVA_UI_STYLE_LABEL=3,NOVA_UI_STYLE_MEDIA=4,NOVA_UI_STYLE_SURFACE=5 };
static nova_theme_id_t styled_theme=NOVA_THEME_COUNT;
static const nova_icon_token_t menu_icons[6] = {
    NOVA_ICON_HOME, NOVA_ICON_INSTALL, NOVA_ICON_SETTINGS,
    NOVA_ICON_DIAGNOSTICS, NOVA_ICON_RECOVERY, NOVA_ICON_POWER
};
static const nova_icon_token_t settings_icons[6]={
    NOVA_ICON_SETTINGS,NOVA_ICON_INFORMATION,NOVA_ICON_INFORMATION,
    NOVA_ICON_DIAGNOSTICS,NOVA_ICON_SETTINGS,NOVA_ICON_BACK};
static const nova_icon_token_t diagnostics_icons[6]={
    NOVA_ICON_INFORMATION,NOVA_ICON_DIAGNOSTICS,NOVA_ICON_DIAGNOSTICS,
    NOVA_ICON_DIAGNOSTICS,NOVA_ICON_INFORMATION,NOVA_ICON_BACK};
static const nova_icon_token_t recovery_icons[6]={
    NOVA_ICON_RECOVERY,NOVA_ICON_SUCCESS,NOVA_ICON_RECOVERY,
    NOVA_ICON_DIAGNOSTICS,NOVA_ICON_DIAGNOSTICS,NOVA_ICON_BACK};
static const nova_icon_token_t power_icons[6]={
    NOVA_ICON_POWER,NOVA_ICON_RESTART,NOVA_ICON_RESTART,
    NOVA_ICON_SETTINGS,NOVA_ICON_FORWARD,NOVA_ICON_BACK};
static const nova_icon_token_t help_icons[6]={
    NOVA_ICON_SEARCH,NOVA_ICON_INFORMATION,NOVA_ICON_INFORMATION,
    NOVA_ICON_INFORMATION,NOVA_ICON_INFORMATION,NOVA_ICON_BACK};
static const nova_icon_token_t firmware_icons[6]={
    NOVA_ICON_INFORMATION,NOVA_ICON_INFORMATION,NOVA_ICON_LOCK,
    NOVA_ICON_LOCK,NOVA_ICON_SETTINGS,NOVA_ICON_BACK};
static nova_rect_t dialog_bounds(void);
static nova_rect_t dialog_button_bounds(const nova_dialog_t *dialog, uint8_t index);
static nova_rect_t context_bounds(void);

static bool refresh_control_styles(const nova_theme_tokens_t *theme)
{
    if(!theme)return false;
    nova_control_style_t base={.background=theme->surface,
        .foreground=theme->text_primary,.accent=theme->accent,.disabled=theme->disabled,
        .border=theme->border,.information=theme->information,.success=theme->success,
        .warning=theme->warning,.error=theme->error,
        .corner_dlu=nova_radius_get(NOVA_RADIUS_MEDIUM),
        .border_dlu=(uint16_t)(theme->high_contrast?2:1),
        .padding_dlu=nova_spacing_get(NOVA_SPACING_S)};
    nova_control_style_t badge=base;badge.corner_dlu=nova_radius_get(NOVA_RADIUS_XLARGE);
    badge.padding_dlu=nova_spacing_get(NOVA_SPACING_XS);
    nova_control_style_t item=base;item.background=0;
    item.corner_dlu=nova_radius_get(NOVA_RADIUS_MEDIUM);
    nova_control_style_t label=base;label.background=0;
    nova_control_style_t media=label;
    if(!nova_style_define(0,NOVA_CONTROL_NONE,&base,NOVA_STYLE_ALL,NOVA_CONTROL_MATERIAL_OPAQUE)||
       !nova_style_define(NOVA_UI_STYLE_BADGE,0,&badge,NOVA_STYLE_ALL,NOVA_CONTROL_MATERIAL_GLASS)||
       !nova_style_define(NOVA_UI_STYLE_LIST_ITEM,0,&item,NOVA_STYLE_ALL,NOVA_CONTROL_MATERIAL_GLASS)||
       !nova_style_define(NOVA_UI_STYLE_LABEL,0,&label,NOVA_STYLE_ALL,NOVA_CONTROL_MATERIAL_TRANSPARENT)||
       !nova_style_define(NOVA_UI_STYLE_MEDIA,0,&media,NOVA_STYLE_ALL,NOVA_CONTROL_MATERIAL_TRANSPARENT)||
       !nova_style_define(NOVA_UI_STYLE_SURFACE,0,&base,NOVA_STYLE_ALL,NOVA_CONTROL_MATERIAL_GLASS))return false;
    styled_theme=nova_theme_active();nova_style_theme_changed();return true;
}

static void rounded_panel(nova_surface_t *surface, int32_t x, int32_t y,
                          int32_t width, int32_t height, uint32_t color)
{
    nova_round_rect_t geometry;
    if(nova_round_rect_create(&geometry,x,y,width,height,
        nova_radius_get(NOVA_RADIUS_LARGE)))
        (void)nova_round_rect_render(surface,&geometry,color,
            nova_render_quality_parameters()->anti_aliasing);
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
    (void)nova_boot_budget_begin(NOVA_BOOT_PHASE_UI);
    nova_architecture_initialize();
    for(uint8_t subsystem=0;subsystem<NOVA_ARCH_SUBSYSTEM_COUNT;++subsystem)
        if(!nova_architecture_register((nova_architecture_subsystem_t)subsystem))return false;
    if (!nova_compositor_initialize(nova_graphics_width(),
                                    nova_graphics_height())) return false;
    nova_present_configuration_t present_configuration={
        .width=nova_graphics_width(),.height=nova_graphics_height(),
        .source_stride=NOVA_SURFACE_WIDTH,.available_memory=0,
        .request_triple_buffer=false,.page_flip_available=false,
        .vsync=NOVA_PRESENT_VSYNC_DISABLED};
    if(nova_present_initialize(&present_configuration)!=NOVA_PRESENT_OK)return false;
    nova_debug_string("UEFI:PRESENT-SCHEDULER-READY\n");
    nova_debug_string("UEFI:DIRTY-MANAGER-READY\n");
    if(!nova_clip_mask_initialize(nova_graphics_width(),nova_graphics_height()))
        return false;
    nova_debug_string("UEFI:CLIP-MASK-READY\n");
    nova_debug_string("UEFI:TRANSFORM2D-READY\n");
    nova_rounded_reset_diagnostics();
    nova_debug_string("UEFI:ROUNDED-GEOMETRY-READY\n");
    nova_effect_initialize(NOVA_EFFECT_QUALITY_STANDARD);
    nova_debug_string("UEFI:SHADOW-GLOW-READY\n");
    if(!nova_blur_initialize(nova_graphics_width(),nova_graphics_height(),
        12u*1024u*1024u))return false;
    nova_debug_string("UEFI:BACKGROUND-BLUR-READY\n");
    nova_render_initialize();
    nova_surface_manager_initialize();
    nova_layer_manager_initialize();
    base_surface = nova_surface_acquire();
    interaction_surface = nova_surface_acquire();
    initialized = base_surface && interaction_surface;
    if (initialized) {
        if(nova_managed_surface_import(base_surface,NOVA_SURFACE_BACKBUFFER,
                NOVA_SURFACE_POOL_PERSISTENT,1,&base_surface_handle)!=NOVA_SURFACE_OK||
           nova_managed_surface_import(interaction_surface,NOVA_SURFACE_LAYER,
                NOVA_SURFACE_POOL_SCENE,2,&interaction_surface_handle)!=NOVA_SURFACE_OK||
           !nova_managed_surface_native(nova_surface_manager_emergency()))return false;
        static const nova_managed_layer_type_t layer_types[]={NOVA_LAYER_BACKGROUND,
            NOVA_LAYER_CONTENT,NOVA_LAYER_CONTROL,NOVA_LAYER_DIALOG,NOVA_LAYER_OVERLAY,
            NOVA_LAYER_CURSOR,NOVA_LAYER_EMERGENCY};
        for(uint8_t i=0;i<sizeof(layer_types)/sizeof(layer_types[0]);++i){
            nova_layer_descriptor_t descriptor={.type=layer_types[i],
                .parent=nova_layer_manager_root(),.z_index=0,
                .bounds={0,0,(int32_t)nova_graphics_width(),(int32_t)nova_graphics_height()},
                .clip={0,0,(int32_t)nova_graphics_width(),(int32_t)nova_graphics_height()},
                .opacity=1000,.transform={65536,0,0,65536,0,0},
                .flags=NOVA_LAYER_ENABLED|NOVA_LAYER_VISIBLE,
                .surface_policy=layer_types[i]==NOVA_LAYER_BACKGROUND||
                    layer_types[i]==NOVA_LAYER_OVERLAY||layer_types[i]==NOVA_LAYER_EMERGENCY?
                    NOVA_LAYER_SURFACE_REQUIRED:NOVA_LAYER_SURFACE_NONE};
            if(layer_types[i]==NOVA_LAYER_DIALOG||layer_types[i]==NOVA_LAYER_EMERGENCY)
                descriptor.flags&=~NOVA_LAYER_VISIBLE;
            if(!nova_managed_layer_create(&descriptor,&managed_layers[layer_types[i]]))return false;
        }
        if(!nova_managed_layer_bind_surface(managed_layers[NOVA_LAYER_BACKGROUND],base_surface_handle)||
           !nova_managed_layer_bind_surface(managed_layers[NOVA_LAYER_OVERLAY],interaction_surface_handle)||
           !nova_managed_layer_bind_surface(managed_layers[NOVA_LAYER_EMERGENCY],
                nova_surface_manager_emergency()))return false;
        nova_debug_string("UEFI:SURFACE-MANAGER-READY\n");
        nova_debug_string("UEFI:LAYER-MANAGER-READY\n");
        (void)nova_boot_budget_begin(NOVA_BOOT_PHASE_RESOURCES);
        if(!nova_resource_loader_initialize(NOVA_RESOURCE_DEFAULT_CACHE_BUDGET))return false;
        if(!nova_resource_cache_configure(512u*1024u,NOVA_RESOURCE_DEFAULT_CACHE_BUDGET,
                                          512u*1024u,7u*1024u*1024u))return false;
        nova_debug_string("UEFI:RESOURCE-LOADER-READY\n");
        static const uint8_t version_dep_bytes[]={1},version_theme_100[]={2};
        static const uint8_t version_theme_110[]={3};
        if(!nova_resource_register("boot://version/probe/font/1.0",NOVA_RESOURCE_FONT,1,
            version_dep_bytes,sizeof(version_dep_bytes),0,0)||
           !nova_resource_register("boot://version/probe/theme/1.0",NOVA_RESOURCE_THEME,1,
            version_theme_100,sizeof(version_theme_100),0,0)||
           !nova_resource_register("boot://version/probe/theme/1.1",NOVA_RESOURCE_THEME,1,
            version_theme_110,sizeof(version_theme_110),0,0)||
           !nova_resource_version_initialize())return false;
        uint64_t version_font_id=nova_resource_id("boot://version/logical/font");
        uint64_t version_theme_id=nova_resource_id("boot://version/logical/theme");
        nova_resource_version_descriptor_t version_font={.logical_id=version_font_id,
            .backing_resource_id=nova_resource_id("boot://version/probe/font/1.0"),
            .version={1,0,0,4},.minimum={1,0,0,0},.maximum={1,9,9,65535}};
        nova_resource_version_dependency_t version_dependencies[]={
            {version_font_id,{1,0,0,0}}};
        nova_resource_version_descriptor_t version_theme_old={.logical_id=version_theme_id,
            .backing_resource_id=nova_resource_id("boot://version/probe/theme/1.0"),
            .version={1,0,0,2},.minimum={1,0,0,0},.maximum={1,9,9,65535},
            .dependencies=version_dependencies,.dependency_count=1};
        nova_resource_version_descriptor_t version_theme_new=version_theme_old;
        version_theme_new.backing_resource_id=
            nova_resource_id("boot://version/probe/theme/1.1");
        version_theme_new.version=(nova_resource_version_t){1,1,0,8};
        nova_resource_version_t version_min={1,0,0,0},version_max={1,9,9,65535};
        if(!nova_resource_version_register(&version_font)||
           !nova_resource_version_register(&version_theme_old)||
           !nova_resource_version_register(&version_theme_new)||
           !nova_resource_version_select(version_font_id,&version_min,&version_max)||
           !nova_resource_version_select(version_theme_id,&version_min,&version_max)||
           nova_resource_version(version_theme_id)->version.minor!=1||
           !nova_resource_version_rollback(version_theme_id)||
           nova_resource_version(version_theme_id)->version.minor!=0||
           !nova_resource_version_select(version_theme_id,&version_min,&version_max)||
           nova_resource_version_diagnostics()->rollbacks!=1)return false;
        nova_debug_string("UEFI:RESOURCE-VERSIONING-READY\n");
        if(!nova_compression_supported(NOVA_COMPRESSION_LZ4)||
           nova_compression_supported(NOVA_COMPRESSION_ZSTD))return false;
        nova_debug_string("UEFI:RESOURCE-COMPRESSION-READY\n");
        if(nova_integrity_policy()!=NOVA_INTEGRITY_STANDARD)return false;
        nova_debug_string("UEFI:RESOURCE-INTEGRITY-READY\n");
        static const uint8_t lz4_probe_packed[]={0x35,'N','O','V',0x03,0x00};
        static const uint8_t lz4_probe_original[]={'N','O','V','N','O','V',
            'N','O','V','N','O','V'};
        static const uint8_t lz4_probe_packed_sha256[32]={
            0x05,0x47,0x4f,0xa2,0xe3,0xa2,0x42,0x58,0x9a,0xb9,0x96,0xb9,0x11,0xef,0xb2,0x14,
            0x35,0x70,0x4f,0xc1,0x28,0x8b,0x4b,0xcb,0x30,0xb3,0xb5,0x62,0x44,0x18,0x14,0x10};
        static const uint8_t lz4_probe_original_sha256[32]={
            0x27,0x96,0x74,0xe8,0xd1,0x85,0x13,0x23,0xcb,0x62,0x8c,0xaf,0xfd,0xf2,0x41,0x82,
            0xf4,0xa1,0xe8,0xd2,0xab,0xfa,0x4a,0x58,0x18,0xc6,0x9c,0x11,0x8f,0xc3,0x56,0x27};
        nova_resource_descriptor_t lz4_probe={
            .uri="boot://resources/lz4-probe",.type=NOVA_RESOURCE_CONFIGURATION,
            .version=1,.data=lz4_probe_packed,.size=sizeof(lz4_probe_packed),
            .origin=NOVA_RESOURCE_ORIGIN_EMBEDDED,.priority=NOVA_RESOURCE_PRIORITY_CRITICAL,
            .compression=NOVA_COMPRESSION_LZ4,.original_size=sizeof(lz4_probe_original),
            .packed_sha256=lz4_probe_packed_sha256,
            .original_sha256=lz4_probe_original_sha256};
        lz4_probe.original_checksum=nova_resource_checksum(lz4_probe_original,
            sizeof(lz4_probe_original));
        nova_resource_t *lz4_probe_resource=0;
        if(nova_resource_register_descriptor(&lz4_probe,&lz4_probe_resource)!=NOVA_RESOURCE_OK||
           !lz4_probe_resource||lz4_probe_resource->data)return false;
        const nova_resource_t *lz4_probe_loaded=nova_resource_load(lz4_probe_resource->id);
        if(!lz4_probe_loaded||!lz4_probe_loaded->data||
           nova_resource_checksum(lz4_probe_loaded->data,lz4_probe_loaded->size)!=
            lz4_probe.original_checksum||nova_resource_diagnostics()->decompressions!=1||
           lz4_probe_loaded->trust!=NOVA_RESOURCE_TRUST_VALID||
           nova_integrity_diagnostics()->sha256_checks<3||
           !nova_resource_release(lz4_probe_resource->id))return false;
        nova_debug_string("UEFI:RESOURCE-LZ4-DECODE-READY\n");
        nova_debug_string("UEFI:RESOURCE-SHA256-VERIFIED\n");
        if(!nova_image_initialize())return false;
        nova_debug_string("UEFI:IMAGE-RENDERER-READY\n");
        static const uint8_t png_probe[]={
            0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0,0,0,0x0d,0x49,0x48,0x44,0x52,
            0,0,0,4,0,0,0,1,1,0,0,0,0,0xd1,0x47,0x32,0x60,0,0,0,2,0x74,0x52,
            0x4e,0x53,0,1,1,0x94,0xfd,0xae,0,0,0,0x0a,0x49,0x44,0x41,0x54,0x78,
            0x9c,0x63,0x48,0,0,0,0x62,0,0x61,0xb1,0x98,0x79,0xf2,0,0,0,0,0x49,
            0x45,0x4e,0x44,0xae,0x42,0x60,0x82};
        if(!nova_png_initialize()||!nova_png_validate(png_probe,sizeof(png_probe))||
           !nova_resource_register("boot://images/png-probe",NOVA_RESOURCE_IMAGE,1,
                png_probe,sizeof(png_probe),0,0))return false;
        nova_image_t *png_probe_image=0;
        if(nova_image_load("boot://images/png-probe",&png_probe_image)!=NOVA_IMAGE_OK||
           !png_probe_image||png_probe_image->width!=4||png_probe_image->height!=1||
           png_probe_image->pixels[0]!=0xff000000u||png_probe_image->pixels[1]!=0||
           png_probe_image->pixels[2]!=0||png_probe_image->pixels[3]!=0xff000000u)
            return false;
        nova_debug_string("UEFI:PNG-DECODER-READY\n");
        (void)nova_boot_budget_begin(NOVA_BOOT_PHASE_THEME);
        if (!nova_theme_initialize()) return false;
        const nova_boot_configuration_t *initial_configuration=
            nova_configuration_effective();
        if(!nova_theme_activate(initial_configuration->theme))return false;
        if(!nova_theme_current()||nova_theme_current()->references!=1||
           nova_theme_diagnostics()->registered!=NOVA_THEME_COUNT)return false;
        nova_debug_string("UEFI:THEME-RESOURCE-READY\n");
        const nova_theme_descriptor_t *default_theme=nova_theme_default();
        if(!default_theme||!default_theme->is_default||!default_theme->is_dark||
           !nova_theme_validate(NOVA_THEME_DARK)||
           nova_theme_resource_find(default_theme->theme_id)==0)return false;
        nova_debug_string("UEFI:DARK-THEME-READY\n");
        const nova_theme_descriptor_t *light_theme=nova_theme_light();
        if(!light_theme||light_theme->is_default||light_theme->is_dark||
           !nova_theme_validate(NOVA_THEME_LIGHT)||
           nova_theme_resource_find(light_theme->theme_id)==0)return false;
        nova_debug_string("UEFI:LIGHT-THEME-READY\n");
        const nova_accessibility_theme_t *high_contrast_theme=
            nova_theme_high_contrast();
        if(!high_contrast_theme||!high_contrast_theme->accessibility_theme||
           !high_contrast_theme->high_contrast||
           !high_contrast_theme->reduced_motion||
           !high_contrast_theme->opaque_materials||
           high_contrast_theme->minimum_focus_dlu<2||
           !nova_theme_validate(NOVA_THEME_HIGH_CONTRAST))return false;
        nova_debug_string("UEFI:HIGH-CONTRAST-THEME-READY\n");
        nova_theme_set_reduced_motion(initial_configuration->reduced_motion);
        (void)nova_boot_budget_end(NOVA_BOOT_PHASE_THEME);
        tooltips_enabled=initial_configuration->tooltips;
        tooltip_delay_ms=initial_configuration->tooltip_delay_ms;
        nova_layout_initialize();
        if (!nova_layout_compute(nova_graphics_width(),nova_graphics_height(),
                                 false, &current_layout)) return false;
        nova_unicode_initialize();
        nova_text_layout_initialize();
        if (!nova_text_register_font_resource() ||
            !nova_font_resource_load(NOVA_SYSTEM_FONT_ID))
            return false;
        if(!nova_font_resource_resolve(NOVA_SYSTEM_FONT_ID,'N')||
           nova_font_theme_get(NOVA_FONT_ROLE_PRIMARY)!=NOVA_SYSTEM_FONT_ID)return false;
        nova_debug_string("UEFI:FONT-RESOURCE-REGISTRY-READY\n");
        if (!nova_icons_initialize()) return false;
        if (!nova_branding_initialize()) return false;
        if(!nova_design_initialize())return false;
        if(!nova_typography_exists(NOVA_TYPOGRAPHY_DISPLAY)||
           !nova_typography_exists(NOVA_TYPOGRAPHY_MONOSPACE)||
           nova_typography_get(NOVA_TYPOGRAPHY_BODY)->line_dlu<
            nova_typography_get(NOVA_TYPOGRAPHY_BODY)->size_dlu||
           nova_spacing_get(NOVA_SPACING_XXXL)<nova_spacing_get(NOVA_SPACING_XXS)||
           !nova_radius_exists(NOVA_RADIUS_CIRCLE))return false;
        nova_debug_string("UEFI:SEMANTIC-DESIGN-TOKENS-READY\n");
        if(!nova_shadow_exists(NOVA_ELEVATION_LEVEL5)||
           !nova_material_exists(NOVA_DESIGN_MATERIAL_DIALOG)||
           !nova_design_motion_exists(NOVA_TRANSITION_DIALOG)||
           !nova_shadow_get(NOVA_ELEVATION_LEVEL4)->layer_count||
           !nova_material_get(NOVA_DESIGN_MATERIAL_DIALOG)->blur_dlu||
           !nova_design_motion_get(NOVA_TRANSITION_DIALOG)->duration_ms)return false;
        nova_debug_string("UEFI:EFFECT-MOTION-TOKENS-READY\n");
        const nova_visual_continuity_context_t *continuity=
            nova_visual_continuity_current();
        if(!continuity||!continuity->visual_continuity_enabled||
           !continuity->accessibility_semantics_preserved||
           continuity->typography_count!=NOVA_TYPOGRAPHY_COUNT||
           continuity->icon_count!=NOVA_ICON_COUNT||
           continuity->interaction_count!=NOVA_INTERACTION_COUNT)return false;
        nova_component_descriptor_t boot_component={
            .design_language_version=continuity->design_language_version,
            .registry_signature=continuity->registry_signature,
            .typography_count=NOVA_TYPOGRAPHY_COUNT,.spacing_count=NOVA_SPACING_COUNT,
            .radius_count=NOVA_RADIUS_COUNT,.icon_count=NOVA_ICON_COUNT,
            .material_count=NOVA_DESIGN_MATERIAL_COUNT,.motion_count=NOVA_TRANSITION_COUNT,
            .interaction_count=NOVA_INTERACTION_COUNT,.semantic_tokens_only=true};
        if(!nova_visual_continuity_validate(&boot_component))return false;
        nova_debug_string("UEFI:VISUAL-CONTINUITY-READY\n");
        bool interaction_states_ready=true;
        for(uint8_t state=0;state<NOVA_INTERACTION_COUNT;++state)
            if(!nova_state_exists((nova_interaction_state_t)state)||
               !nova_state_get((nova_interaction_state_t)state))
                interaction_states_ready=false;
        if(!interaction_states_ready||
           !nova_state_get(NOVA_INTERACTION_DISABLED)->input_blocked||
           !nova_state_get(NOVA_INTERACTION_ERROR)->indicator_required||
           !nova_state_transition_allowed(NOVA_INTERACTION_HOVER,NOVA_INTERACTION_PRESSED)||
           !nova_state_transition_allowed(NOVA_INTERACTION_EXPANDED,NOVA_INTERACTION_COLLAPSED))
            return false;
        nova_debug_string("UEFI:INTERACTION-STATES-READY\n");
        if(!nova_runtime_subsystem_ready(NOVA_RUNTIME_RESOURCES)||
           !nova_runtime_building_scene())return false;
        nova_debug_string("UEFI:BRANDING-READY\n");
        nova_debug_string("UEFI:DESIGN-COMPATIBILITY-READY\n");
        if(!nova_resource_preload_priority(NOVA_RESOURCE_PRIORITY_NORMAL))return false;
        const nova_resource_t *cached_font=nova_resource_find(
            nova_resource_id("boot://fonts/segoe-ui/semibold/15"));
        const nova_resource_t *cached_icon=nova_resource_find(
            nova_resource_id("boot://icons/home"));
        const nova_resource_t *cached_logo=nova_resource_find(
            nova_resource_id("boot://branding/novaos/logo"));
        if(!cached_font||!cached_icon||!cached_logo||
           cached_font->cache_policy!=NOVA_CACHE_PERMANENT||
           cached_icon->cache_policy!=NOVA_CACHE_PERMANENT||
           cached_logo->cache_policy!=NOVA_CACHE_PERMANENT||
           nova_resource_diagnostics()->cache_minimum!=512u*1024u||
           nova_resource_diagnostics()->cache_critical!=7u*1024u*1024u)return false;
        nova_debug_string("UEFI:RESOURCE-CACHE-READY\n");
        nova_debug_string("UEFI:RESOURCE-PRELOAD-READY\n");
        nova_debug_string("UEFI:RESOURCES-READY\n");
        (void)nova_boot_budget_end(NOVA_BOOT_PHASE_RESOURCES);
        nova_page_model_initialize();
        static const char *const page_titles[7]={"NovaOS Bootmanager","Einstellungen",
            "Boot-Diagnose","Recovery","Ausschalten / Neustarten","Hilfe",
            "Firmware-Einstellungen"};
        for(uint16_t i=0;i<7;++i)if(!create_page_model((nova_bootmenu_view_t)i,page_titles[i]))return false;
        if(!nova_page_activate(view_pages[NOVA_VIEW_MAIN])||
           !nova_page_set_focus(view_pages[NOVA_VIEW_MAIN],view_content[NOVA_VIEW_MAIN]))return false;
        nova_debug_string("UEFI:PAGES-READY\n");
        const nova_theme_tokens_t *initial_theme = nova_theme_tokens();
        nova_control_style_t style = {
            .background=0x00000000u,.foreground=initial_theme->text_primary,
            .accent=initial_theme->focus,.disabled=initial_theme->disabled,
            .border=initial_theme->border,.information=initial_theme->information,
            .success=initial_theme->success,.warning=initial_theme->warning,
            .error=initial_theme->error,.corner_dlu=nova_radius_get(NOVA_RADIUS_MEDIUM),
            .border_dlu=1,.padding_dlu=nova_spacing_get(NOVA_SPACING_S)
        };
        nova_controls_initialize(&style);
        if(!refresh_control_styles(initial_theme))return false;
        menu_list = nova_control_create(NOVA_CONTROL_LIST);
        if (!menu_list) return false;
        nova_control_set_state(menu_list, NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(menu_list, NOVA_CONTROL_VISIBLE);
        if(!nova_control_set_event_handler(menu_list,bootmenu_list_event,0))return false;
        for (uint16_t i = 0; i < 6; ++i) {
            menu_items[i] = nova_control_create(NOVA_CONTROL_LIST_ITEM);
            if (!menu_items[i] || !nova_list_add_item(menu_list,menu_items[i])) return false;
            menu_items[i]->action = i;
            nova_control_set_state(menu_items[i], NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(menu_items[i], NOVA_CONTROL_VISIBLE);
            nova_control_set_text(menu_items[i], main_text[i]);
            nova_control_set_accessibility(menu_items[i], 1, main_text[i], false);
            nova_style_apply(menu_items[i],NOVA_UI_STYLE_LIST_ITEM);
        }
        for(uint16_t i=0;i<6;++i){
            recovery_tiles[i]=nova_control_create(NOVA_CONTROL_TILE);
            if(!recovery_tiles[i]||!nova_control_set_parent(recovery_tiles[i],menu_list))return false;
            recovery_tiles[i]->action=i;nova_control_set_state(recovery_tiles[i],NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(recovery_tiles[i],NOVA_CONTROL_VISIBLE);
            nova_control_set_text(recovery_tiles[i],recovery_text[i]);
            nova_tile_set_description(recovery_tiles[i],recovery_descriptions[i]);
            nova_tile_set_status(recovery_tiles[i],recovery_status[i]);
            nova_tile_set_type(recovery_tiles[i],i==2?NOVA_TILE_RECOVERY:
                i==3||i==4?NOVA_TILE_DIAGNOSTIC:i<2?NOVA_TILE_MAINTENANCE:NOVA_TILE_STANDARD);
            nova_control_set_accessibility(recovery_tiles[i],12,recovery_text[i],false);
            recovery_badges[i]=nova_control_create(NOVA_CONTROL_STATUS_BADGE);
            if(!recovery_badges[i])return false;
            nova_control_set_state(recovery_badges[i],NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(recovery_badges[i],NOVA_CONTROL_VISIBLE);
            nova_control_set_text(recovery_badges[i],recovery_status[i]);
            nova_status_badge_set_type(recovery_badges[i],i==1||i==4?NOVA_BADGE_INFORMATION:
                i==3?NOVA_BADGE_SUCCESS:i==5?NOVA_BADGE_NEUTRAL:NOVA_BADGE_WARNING);
            nova_status_badge_set_icon(recovery_badges[i],i==3?NOVA_ICON_SUCCESS:
                i==0||i==2?NOVA_ICON_WARNING:NOVA_ICON_INFORMATION);
            nova_control_set_accessibility(recovery_badges[i],13,recovery_status[i],false);
            nova_style_apply(recovery_badges[i],NOVA_UI_STYLE_BADGE);
        }
        static const nova_control_type_t settings_types[4]={NOVA_CONTROL_MENU_BUTTON,
            NOVA_CONTROL_SWITCH,NOVA_CONTROL_CHECKBOX,NOVA_CONTROL_SLIDER};
        for (uint16_t i = 0; i < 4; ++i) {
            settings_accessories[i] = nova_control_create(settings_types[i]);
            if (!settings_accessories[i]) return false;
            nova_control_set_state(settings_accessories[i], NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(settings_accessories[i], NOVA_CONTROL_VISIBLE);
            nova_control_set_flags(settings_accessories[i],NOVA_CONTROL_FLAG_VISIBLE |
                NOVA_CONTROL_FLAG_ENABLED);
        }
        nova_control_set_accessibility(settings_accessories[1],4,"Reduzierte Bewegung",false);
        nova_control_set_accessibility(settings_accessories[2],5,"Tooltips anzeigen",false);
        nova_control_set_accessibility(settings_accessories[3],6,"Tooltip-Verzögerung",false);
        if(!nova_control_set_range(settings_accessories[3],250,1500,750)||
           !nova_control_set_step(settings_accessories[3],250))return false;
        nova_control_set_checked(settings_accessories[2],true);
        context_menu=nova_control_create(NOVA_CONTROL_CONTEXT_MENU);
        if(!context_menu)return false;
        nova_control_set_state(context_menu,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(context_menu,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(context_menu,NOVA_CONTROL_FLAG_VISIBLE |
                                            NOVA_CONTROL_FLAG_MODAL);
        for(uint16_t i=0;i<6;++i){
            context_items[i]=nova_control_create(NOVA_CONTROL_LIST_ITEM);
            if(!context_items[i]||!nova_control_set_parent(context_items[i],context_menu))
                return false;
            context_items[i]->action=i;
            nova_control_set_state(context_items[i],NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(context_items[i],NOVA_CONTROL_VISIBLE);
            const char *initial=i<3?context_action_text[i]:"";
            nova_control_set_text(context_items[i],initial);
            nova_control_set_accessibility(context_items[i],2,initial,false);
        }
        if(!nova_menu_button_bind(settings_accessories[0],context_menu)||
           !nova_control_set_text(settings_accessories[0],"Dark")||
           !nova_control_set_accessibility(settings_accessories[0],5,
                                            "Darstellung auswählen",false))return false;
        tooltip_control=nova_control_create(NOVA_CONTROL_TOOLTIP);
        breadcrumb_control=nova_control_create(NOVA_CONTROL_BREADCRUMB);
        breadcrumb_home_button=nova_control_create(NOVA_CONTROL_ICON_BUTTON);
        help_search_field=nova_control_create(NOVA_CONTROL_TEXT_FIELD);
        dialog_spinner=nova_control_create(NOVA_CONTROL_SPINNER);
        dialog_password_field=nova_password_field_create();
        diagnostic_scroll_view=nova_control_create(NOVA_CONTROL_SCROLL_VIEW);
        diagnostic_scrollbar=nova_control_create(NOVA_CONTROL_SCROLLBAR);
        page_card=nova_control_create(NOVA_CONTROL_CARD);
        brand_image=nova_control_create(NOVA_CONTROL_IMAGE);
        brand_label=nova_control_create(NOVA_CONTROL_LABEL);
        status_label=nova_control_create(NOVA_CONTROL_LABEL);
        section_separator=nova_control_create(NOVA_CONTROL_SEPARATOR);
        for(uint8_t i=0;i<6;++i)item_icons[i]=nova_control_create(NOVA_CONTROL_ICON);
        if(!tooltip_control||!breadcrumb_control||!breadcrumb_home_button||!help_search_field||!dialog_spinner||
           !dialog_password_field||
           !diagnostic_scroll_view||!diagnostic_scrollbar||!page_card||!brand_image||
           !brand_label||!status_label||!section_separator)return false;
        for(uint8_t i=0;i<6;++i)if(!item_icons[i])return false;
        nova_control_t *visuals[10]={brand_image,brand_label,status_label,section_separator,
            item_icons[0],item_icons[1],item_icons[2],item_icons[3],item_icons[4],item_icons[5]};
        for(uint8_t i=0;i<10;++i){
            nova_control_set_state(visuals[i],NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(visuals[i],NOVA_CONTROL_VISIBLE);
            nova_control_set_flags(visuals[i],NOVA_CONTROL_FLAG_VISIBLE|
                NOVA_CONTROL_FLAG_READONLY|NOVA_CONTROL_FLAG_DECORATIVE);
        }
        nova_image_set_resource(brand_image,NOVA_IMAGE_BRANDING_LOGO);
        nova_image_set_scaling(brand_image,NOVA_IMAGE_FIT);
        nova_label_set_type(brand_label,NOVA_LABEL_HEADER);
        nova_label_set_alignment(brand_label,NOVA_ALIGN_LEFT_MIDDLE);
        nova_control_set_text(brand_label,"NovaOS");
        nova_control_set_accessibility(brand_label,8,"NovaOS",false);
        brand_label->flags&=~NOVA_CONTROL_FLAG_DECORATIVE;
        nova_label_set_type(status_label,NOVA_LABEL_STATUS);
        nova_label_set_alignment(status_label,NOVA_ALIGN_LEFT_TOP);
        nova_control_set_accessibility(status_label,8,"Bootmanager-Status",false);
        status_label->flags&=~NOVA_CONTROL_FLAG_DECORATIVE;
        nova_separator_set_orientation(section_separator,NOVA_SEPARATOR_HORIZONTAL);
        nova_style_apply(brand_image,NOVA_UI_STYLE_MEDIA);
        nova_style_apply(brand_label,NOVA_UI_STYLE_LABEL);
        nova_style_apply(status_label,NOVA_UI_STYLE_LABEL);
        nova_style_apply(section_separator,NOVA_UI_STYLE_LABEL);
        nova_style_apply(page_card,NOVA_UI_STYLE_SURFACE);
        for(uint8_t i=0;i<6;++i)nova_style_apply(item_icons[i],NOVA_UI_STYLE_MEDIA);
        nova_control_set_state(tooltip_control,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(tooltip_control,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(tooltip_control,NOVA_CONTROL_FLAG_VISIBLE |
                                               NOVA_CONTROL_FLAG_DECORATIVE |
                                               NOVA_CONTROL_FLAG_READONLY);
        nova_control_set_state(breadcrumb_control,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(breadcrumb_control,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(breadcrumb_control,NOVA_CONTROL_FLAG_VISIBLE |
                                                  NOVA_CONTROL_FLAG_ENABLED);
        nova_control_set_accessibility(breadcrumb_control,3,"Navigationspfad",false);
        nova_control_set_state(breadcrumb_home_button,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(breadcrumb_home_button,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(breadcrumb_home_button,NOVA_CONTROL_FLAG_VISIBLE|
                                                      NOVA_CONTROL_FLAG_ENABLED);
        nova_control_set_accessibility(breadcrumb_home_button,4,"Zum Hauptmenü",false);
        if(!nova_icon_button_set_icon(breadcrumb_home_button,NOVA_ICON_HOME)||
           !nova_icon_button_set_tooltip(breadcrumb_home_button,"Zum Hauptmenü")||
           !nova_icon_button_set_action(breadcrumb_home_button,1))return false;
        nova_control_set_state(help_search_field,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(help_search_field,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(help_search_field,NOVA_CONTROL_FLAG_VISIBLE|
                                                 NOVA_CONTROL_FLAG_ENABLED);
        nova_text_field_set_placeholder(help_search_field,"Hilfethemen durchsuchen");
        nova_text_field_set_mode(help_search_field,NOVA_TEXT_INPUT_SEARCH);
        nova_text_field_set_maximum(help_search_field,48);
        nova_control_set_accessibility(help_search_field,7,"Hilfethemen durchsuchen",false);
        nova_control_set_state(dialog_spinner,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(dialog_password_field,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(dialog_password_field,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(dialog_password_field,NOVA_CONTROL_FLAG_VISIBLE|
            NOVA_CONTROL_FLAG_ENABLED|NOVA_CONTROL_FLAG_FOCUSED);
        nova_password_field_set_placeholder(dialog_password_field,"Passwort eingeben");
        nova_password_field_enable_reveal(dialog_password_field,false);
        nova_control_set_accessibility(dialog_password_field,9,"Passwortfeld",false);
        nova_control_set_state(dialog_spinner,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(dialog_spinner,NOVA_CONTROL_FLAG_VISIBLE|NOVA_CONTROL_FLAG_ENABLED|
                                              NOVA_CONTROL_FLAG_DECORATIVE);
        nova_control_set_accessibility(dialog_spinner,8,"Operation wird ausgeführt",true);
        nova_activity_set_style(dialog_spinner,NOVA_ACTIVITY_SPINNER);
        for(uint8_t i=0;i<NOVA_DIALOG_BUTTON_CAPACITY;++i){
            dialog_buttons[i]=nova_control_create(NOVA_CONTROL_BUTTON);
            if(!dialog_buttons[i])return false;
            nova_control_set_state(dialog_buttons[i],NOVA_CONTROL_INITIALIZED);
            nova_control_set_state(dialog_buttons[i],NOVA_CONTROL_VISIBLE);
            nova_control_set_flags(dialog_buttons[i],NOVA_CONTROL_FLAG_VISIBLE|
                                                     NOVA_CONTROL_FLAG_ENABLED);
            nova_control_set_accessibility(dialog_buttons[i],5,"Dialogaktion",false);
        }
        nova_control_set_state(diagnostic_scroll_view,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(diagnostic_scroll_view,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(diagnostic_scroll_view,NOVA_CONTROL_FLAG_VISIBLE|
                                                      NOVA_CONTROL_FLAG_ENABLED);
        nova_control_set_accessibility(diagnostic_scroll_view,9,"Diagnosedaten, weiterer Inhalt verfügbar",false);
        nova_control_set_state(diagnostic_scrollbar,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(diagnostic_scrollbar,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(diagnostic_scrollbar,NOVA_CONTROL_FLAG_VISIBLE|
                                                     NOVA_CONTROL_FLAG_ENABLED);
        nova_control_set_accessibility(diagnostic_scrollbar,10,"Diagnose-Scrollposition",false);
        if(!nova_scrollbar_attach(diagnostic_scrollbar,diagnostic_scroll_view,
                                  NOVA_SCROLLBAR_VERTICAL))return false;
        nova_control_set_state(page_card,NOVA_CONTROL_INITIALIZED);
        nova_control_set_state(page_card,NOVA_CONTROL_VISIBLE);
        nova_control_set_flags(page_card,NOVA_CONTROL_FLAG_VISIBLE|NOVA_CONTROL_FLAG_ENABLED);
        nova_control_set_accessibility(page_card,11,"Bootmanager-Inhaltsbereich",false);
        nova_card_set_type(page_card,NOVA_CARD_STANDARD);
        if(!nova_runtime_subsystem_ready(NOVA_RUNTIME_SCENE)||
           !nova_runtime_layout()||
           !nova_runtime_subsystem_ready(NOVA_RUNTIME_LAYOUT_ENGINE))return false;
        nova_debug_string("UEFI:LAYOUT-READY\n");
        nova_motion_initialize();
        int32_t interrupt_probe_value=0;
        nova_animation_t interrupt_probe={.target=&interrupt_probe_value,.from=0,.to=1000,
            .start_ms=0,.duration_ms=100,.group=0xff,.priority=3,
            .property=NOVA_PROPERTY_X,.easing=NOVA_EASE_LINEAR,
            .state=NOVA_MOTION_CREATED,.interruptible=true};
        nova_animation_t *interrupt_animation=nova_motion_create(&interrupt_probe);
        nova_motion_update(50);
        if(!interrupt_animation||interrupt_probe_value!=500||
           !nova_motion_interrupt(interrupt_animation)||
           !nova_motion_resume(interrupt_animation,50)||
           !nova_motion_redirect(interrupt_animation,1500,50))return false;
        nova_motion_update(150);
        if(interrupt_probe_value!=1500||nova_motion_diagnostics()->interruptions!=1||
           nova_motion_diagnostics()->resumes!=1||
           nova_motion_diagnostics()->redirects!=1)return false;
        nova_debug_string("UEFI:INTERRUPTIBLE-MOTION-READY\n");
        int32_t reduced_position=0,reduced_opacity=0;
        nova_animation_t reduced_spatial={.target=&reduced_position,.from=0,.to=100,
            .start_ms=200,.duration_ms=200,.group=0xfe,.priority=2,
            .property=NOVA_PROPERTY_X,.easing=NOVA_EASE_SPRING,
            .state=NOVA_MOTION_CREATED,.interruptible=true};
        nova_animation_t reduced_fade=reduced_spatial;
        reduced_fade.target=&reduced_opacity;reduced_fade.to=255;
        reduced_fade.property=NOVA_PROPERTY_OPACITY;
        if(!nova_motion_create(&reduced_spatial)||!nova_motion_create(&reduced_fade))return false;
        nova_motion_update(250);
        if(!nova_motion_set_reduced(true)||reduced_position!=100||
           nova_motion_policy(NOVA_PROPERTY_X)!=NOVA_MOTION_POLICY_REPLACED||
           nova_motion_policy(NOVA_PROPERTY_OPACITY)!=NOVA_MOTION_POLICY_ALLOWED||
           nova_motion_diagnostics()->policy_replacements<1)return false;
        nova_debug_string("UEFI:REDUCED-MOTION-POLICY-READY\n");
        for(uint8_t violation=0;violation<6;++violation)
            nova_motion_budget_update(20000,1200);
        const nova_motion_budget_t *motion_budget=nova_motion_budget_get();
        if(!motion_budget||motion_budget->memory_bytes>512u*1024u||
           motion_budget->violations!=6||motion_budget->fallback_step!=6||
           !motion_budget->safe_mode||motion_budget->decorative_enabled)return false;
        nova_debug_string("UEFI:MOTION-BUDGET-READY\n");
        nova_motion_initialize();
        nova_motion_set_reduced(nova_theme_reduced_motion());
        if(!nova_animation_resource_initialize())return false;
        static const nova_animation_keyframe_t page_motion_frames[]={
            {0,NOVA_PROPERTY_OPACITY,0,NOVA_EASE_LINEAR},
            {180,NOVA_PROPERTY_OPACITY,255,NOVA_EASE_OUT_CUBIC},
            {0,NOVA_PROPERTY_X,24,NOVA_EASE_LINEAR},
            {180,NOVA_PROPERTY_X,0,NOVA_EASE_OUT_CUBIC}};
        if(!nova_resource_register("boot://animations/page-enter",NOVA_RESOURCE_ANIMATION,1,
            page_motion_frames,sizeof(page_motion_frames),0,0))return false;
        nova_animation_resource_descriptor_t page_motion={
            .animation_id=0x50414745454e5445ull,
            .resource_id=nova_resource_id("boot://animations/page-enter"),
            .name="Page Enter",.version=1,.resource_version=1,.duration_ms=180,
            .repeat=NOVA_ANIMATION_REPEAT_ONCE,.trigger=NOVA_ANIMATION_TRIGGER_PAGE_CHANGE,
            .category=NOVA_ANIMATION_CATEGORY_NAVIGATION,.priority=2,
            .keyframes=page_motion_frames,.keyframe_count=4};
        nova_animation_sample_t page_motion_sample;
        if(!nova_animation_resource_register(&page_motion)||
           !nova_animation_resource_load(page_motion.animation_id)||
           !nova_animation_theme_bind(0,page_motion.animation_id)||
           !nova_animation_resource_sample(page_motion.animation_id,90,&page_motion_sample)||
           !(page_motion_sample.property_mask&(1u<<NOVA_PROPERTY_OPACITY))||
           !(page_motion_sample.property_mask&(1u<<NOVA_PROPERTY_X)))return false;
        nova_animation_resource_set_reduced(initial_configuration->reduced_motion);
        nova_debug_string("UEFI:ANIMATION-RESOURCE-READY\n");
        if(!nova_render_quality_initialize(true,64ull*1024u*1024u))return false;
        nova_quality_telemetry_t adaptive_probe={.frame_time_us=16000,
            .width=nova_graphics_width(),.height=nova_graphics_height(),
            .available_memory=64ull*1024u*1024u,.cache_used=
                nova_resource_diagnostics()->cached_bytes,
            .cache_budget=nova_resource_diagnostics()->cache_budget,
            .cpu_score=50,.gpu_load_percent=0,.gpu_available=false,
            .software_renderer=true,.firmware_limited=false};
        if(!nova_render_quality_observe(&adaptive_probe)||
           !nova_render_quality_set_auto(adaptive_probe.available_memory,false,true)||
           !nova_boot_quality_update()||
           nova_render_quality_get()!=NOVA_RENDER_QUALITY_PERFORMANCE||
           !nova_boot_quality_status()->state_preserved||
           !nova_boot_quality_status()->security_preserved||
           nova_boot_quality_status()->parameters.animation_frame_step!=3)return false;
        for(uint8_t profile=0;profile<NOVA_RENDER_QUALITY_COUNT;++profile)
            if(!nova_boot_quality_set((nova_boot_quality_t)profile))return false;
        if(!nova_boot_quality_set(NOVA_BOOT_QUALITY_AUTO))return false;
        nova_debug_string("UEFI:ADAPTIVE-QUALITY-READY\n");
        if(!nova_low_end_profile_initialize()||
           !nova_low_end_profile_detect(1,1000,32ull*1024u*1024u,false,false)||
           !nova_low_end_profile_status()->enabled||
           nova_low_end_profile_status()->target_fps!=30||
           nova_memory_budget_status()->total_budget!=32ull*1024u*1024u||
           nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_RESOURCE_CACHE]!=
                6ull*1024u*1024u||
           nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_GLYPH_CACHE]!=
                2ull*1024u*1024u||
           nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_SVG_CACHE]!=
                2ull*1024u*1024u||
           nova_memory_budget_status()->area_budget[NOVA_MEMORY_AREA_THEME_CACHE]!=
                512ull*1024u||
           nova_render_quality_get()!=NOVA_RENDER_QUALITY_PERFORMANCE)return false;
        nova_debug_string("UEFI:LOW-END-PROFILE-READY\n");
        if(!nova_high_quality_profile_initialize()||
           nova_high_quality_profile_detect(8,16ull*1024u*1024u*1024u,
                512ull*1024u*1024u,true,true)||
           nova_high_quality_profile_status()->enabled||
           nova_high_quality_profile_status()->gpu_acceleration||
           !nova_high_quality_profile_status()->software_fallback_available)return false;
        nova_debug_string("UEFI:HIGH-QUALITY-FALLBACK-READY\n");
        nova_debug_string("UEFI:RENDER-QUALITY-READY\n");
        nova_sw_renderer_configuration_t software={nova_graphics_width(),
            nova_graphics_height(),nova_memory_budget_status()->total_budget,true,false,false,
            nova_configuration_effective()->safe_mode};
        if(nova_sw_renderer_initialize(&software)!=NOVA_SW_RENDERER_OK)return false;
        nova_debug_string("UEFI:SOFTWARE-RENDERER-READY\n");
        if(!nova_runtime_subsystem_ready(NOVA_RUNTIME_MOTION))return false;
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
        if(!nova_runtime_subsystem_ready(NOVA_RUNTIME_INPUT)||
           !nova_runtime_subsystem_ready(NOVA_RUNTIME_RENDERER))return false;
        nova_debug_string("UEFI:RECOVERY-MANAGER-READY\n");
        nova_debug_string("UEFI:MEMORY-MANAGER-READY\n");
        nova_debug_string("UEFI:CONFIGURATION-MANAGER-READY\n");
        nova_debug_string("UEFI:INPUT-READY\n");
        nova_debug_string("UEFI:CONTROLS-READY\n");
        if(!nova_scene_traverse(0,0,false)||nova_scene_diagnostics()->visited<2)return false;
        nova_debug_string("UEFI:SCENE-GRAPH-READY\n");
        nova_debug_string("UEFI:RENDER-COMMANDS-READY\n");
        static const nova_architecture_subsystem_t architecture_order[]={
            NOVA_ARCH_PLATFORM,NOVA_ARCH_RESOURCE,NOVA_ARCH_GRAPHICS,
            NOVA_ARCH_RENDERER,NOVA_ARCH_SCENE,NOVA_ARCH_LAYOUT,NOVA_ARCH_MOTION,
            NOVA_ARCH_CONTROL,NOVA_ARCH_NAVIGATION,NOVA_ARCH_DIALOG,
            NOVA_ARCH_APPLICATION,NOVA_ARCH_DIAGNOSTICS};
        for(uint8_t i=0;i<sizeof(architecture_order)/sizeof(architecture_order[0]);++i)
            if(!nova_architecture_ready(architecture_order[i]))return false;
        if(!nova_architecture_validate())return false;
        nova_debug_string("UEFI:UI-ARCHITECTURE-READY\n");
    }
    if (initialized) nova_debug_string("UEFI:COMPOSITOR-READY\n");
    (void)nova_boot_budget_end(NOVA_BOOT_PHASE_UI);
    return initialized;
}

static const char *const *view_items(void)
{
    if (current_view == NOVA_VIEW_SETTINGS) return settings_text;
    if (current_view == NOVA_VIEW_DIAGNOSTICS) return diagnostics_text;
    if (current_view == NOVA_VIEW_RECOVERY) return recovery_text;
    if (current_view == NOVA_VIEW_POWER) return power_text;
    if (current_view == NOVA_VIEW_HELP) return help_items;
    if (current_view == NOVA_VIEW_FIRMWARE) return firmware_items;
    return main_text;
}

static const nova_icon_token_t *view_icons(void)
{
    if(current_view==NOVA_VIEW_SETTINGS)return settings_icons;
    if(current_view==NOVA_VIEW_DIAGNOSTICS)return diagnostics_icons;
    if(current_view==NOVA_VIEW_RECOVERY)return recovery_icons;
    if(current_view==NOVA_VIEW_POWER)return power_icons;
    if(current_view==NOVA_VIEW_HELP)return help_icons;
    if(current_view==NOVA_VIEW_FIRMWARE)return firmware_icons;
    return menu_icons;
}

void bootmenu_set_view(nova_bootmenu_view_t view)
{
    if(view>NOVA_VIEW_FIRMWARE)return;
    nova_input_pointer_cancel_all();
    diagnostic_scroll_dragging=false;settings_slider_dragging=false;
    if(view!=NOVA_VIEW_HELP){help_search_editing=false;
        if(help_search_field)help_search_field->flags&=~NOVA_CONTROL_FLAG_FOCUSED;}
    current_view = view;base_dirty_requested=true;
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
bool bootmenu_item_available(uint16_t index)
{
    if(index>=6)return false;
    if(current_view==NOVA_VIEW_HELP&&index>=1&&index<=4)
        return help_result_indices[index-1]>=0;
    return true;
}
void bootmenu_set_status(const char *text) { status_text = text ? text : ""; }
void bootmenu_set_transition(int32_t offset_dlu, uint8_t opacity, bool input_locked)
{
    transition_offset_dlu=offset_dlu;
    transition_opacity=opacity;
    transition_input_locked=input_locked;
}
bool bootmenu_transition_input_locked(void) { return transition_input_locked; }

static void firmware_line(uint8_t line,const char *prefix,const char *value)
{
    if(line>=6)return;
    uint32_t out=0;
    while(prefix&&*prefix&&out+1<NOVA_CONTROL_TEXT_CAPACITY)firmware_text[line][out++]=*prefix++;
    while(value&&*value&&out+1<NOVA_CONTROL_TEXT_CAPACITY)firmware_text[line][out++]=*value++;
    firmware_text[line][out]=0;
}

void bootmenu_set_firmware_info(const char *vendor,uint32_t revision,
    bool secure_known,bool secure_boot,bool setup_known,bool setup_mode,
    bool setup_supported)
{
    (void)revision;
    firmware_line(0,"Firmwaremodus: ","UEFI");
    firmware_line(1,"Hersteller: ",vendor&&*vendor?vendor:"Unbekannt");
    firmware_line(2,"Secure Boot: ",secure_known?(secure_boot?"aktiv":"deaktiviert"):
                                               "nicht verfügbar");
    firmware_line(3,"Setup Mode: ",setup_known?(setup_mode?"aktiv":"inaktiv"):
                                           "nicht verfügbar");
    firmware_line(4,"",setup_supported?"Firmware-Setup öffnen":
                                      "Firmware-Setup nicht verfügbar");
    firmware_setup_available=setup_supported;
    base_dirty_requested=true;
}

static void tooltip_delay_status(void)
{
    static char text[48]="Tooltip-Verzögerung: 0000 ms.";
    uint32_t value=tooltip_delay_ms;
    uint8_t offset=22;
    text[offset++]=(char)('0'+(value/1000u)%10u);
    text[offset++]=(char)('0'+(value/100u)%10u);
    text[offset++]=(char)('0'+(value/10u)%10u);
    text[offset++]=(char)('0'+value%10u);
    bootmenu_set_status(text);
}

static bool configuration_commit(nova_configuration_key_t key,uint32_t value)
{
    if(!nova_configuration_begin()||!nova_configuration_set(key,value)||
       !nova_configuration_commit())return false;
    nova_debug_string("UEFI:CONFIGURATION-COMMIT\n");return true;
}

bool bootmenu_configuration_select_theme(uint8_t theme)
{
    if(!initialized||theme>=NOVA_THEME_COUNT||
       !configuration_commit(NOVA_CONFIG_THEME,theme))return false;
    if(!nova_theme_activate((nova_theme_id_t)theme))return false;
    if(!nova_design_tokens_reload())return false;
    nova_animation_resource_set_reduced(nova_theme_reduced_motion());
    return true;
}

bool bootmenu_configuration_select_quality(uint8_t quality)
{
    if(!initialized||quality>NOVA_QUALITY_AUTO||
       !configuration_commit(NOVA_CONFIG_QUALITY,quality))return false;
    nova_diag_set_quality((nova_quality_t)quality,quality==NOVA_QUALITY_AUTO);
    return nova_diag_quality()->quality<=NOVA_QUALITY_SAFE;
}

bool bootmenu_settings_toggle_reduced_motion(void)
{
    if(!initialized)return false;
    bool reduced=!nova_configuration_effective()->reduced_motion;
    if(!configuration_commit(NOVA_CONFIG_REDUCED_MOTION,reduced))return false;
    nova_theme_set_reduced_motion(reduced);
    if(!nova_design_tokens_reload())return false;
    nova_animation_resource_set_reduced(reduced);return true;
}

bool bootmenu_settings_toggle_tooltips(void)
{
    if(!initialized)return false;
    bool enabled=!nova_configuration_effective()->tooltips;
    if(!configuration_commit(NOVA_CONFIG_TOOLTIPS,enabled))return false;
    tooltips_enabled=enabled;
    nova_control_set_checked(settings_accessories[2],tooltips_enabled);
    bootmenu_tooltip_hide();
    bootmenu_set_status(tooltips_enabled?"Tooltips sind aktiviert.":"Tooltips sind deaktiviert.");
    nova_debug_string(tooltips_enabled?"UEFI:SETTINGS-TOOLTIPS-ON\n":
                                       "UEFI:SETTINGS-TOOLTIPS-OFF\n");
    return true;
}

bool bootmenu_settings_adjust_tooltip_delay(int32_t steps)
{
    if(!initialized||!steps)return false;
    if(!nova_control_adjust(settings_accessories[3],steps))return false;
    uint32_t delay=(uint32_t)settings_accessories[3]->value;
    if(!configuration_commit(NOVA_CONFIG_TOOLTIP_DELAY,delay))return false;
    tooltip_delay_ms=delay;
    bootmenu_tooltip_hide();tooltip_delay_status();
    nova_debug_string("UEFI:SETTINGS-SLIDER-UPDATED\n");
    return true;
}

bool bootmenu_settings_set_tooltip_delay_edge(bool maximum)
{
    if(!initialized)return false;
    if(!nova_control_set_value(settings_accessories[3],maximum?1500:250))return false;
    uint32_t delay=(uint32_t)settings_accessories[3]->value;
    if(!configuration_commit(NOVA_CONFIG_TOOLTIP_DELAY,delay))return false;
    tooltip_delay_ms=delay;
    bootmenu_tooltip_hide();tooltip_delay_status();
    nova_debug_string(maximum?"UEFI:SETTINGS-SLIDER-END\n":"UEFI:SETTINGS-SLIDER-HOME\n");
    return true;
}

static char ascii_lower(char value)
{
    return value>='A'&&value<='Z'?(char)(value-'A'+'a'):value;
}

static bool help_contains(const char *text,const char *query)
{
    if(!query||!*query)return true;
    for(uint16_t start=0;text&&text[start];++start){uint16_t i=0;
        while(query[i]&&text[start+i]&&ascii_lower(text[start+i])==ascii_lower(query[i]))++i;
        if(!query[i])return true;
    }
    return false;
}

static void help_copy(uint8_t row,const char *text)
{
    uint32_t out=0;if(row>=6)return;
    while(text&&*text&&out+1<NOVA_CONTROL_TEXT_CAPACITY)help_result_text[row][out++]=*text++;
    help_result_text[row][out]=0;
    if(menu_items[row])nova_control_set_text(menu_items[row],help_result_text[row]);
}

static void help_refresh_results(void)
{
    uint8_t found=0;
    for(uint8_t i=0;i<sizeof(help_entries)/sizeof(help_entries[0])&&found<4;++i)
        if(help_contains(help_entries[i].title,help_search_field->text)||
           help_contains(help_entries[i].keywords,help_search_field->text)){
            help_result_indices[found]=(int8_t)i;help_copy((uint8_t)(found+1),help_entries[i].title);
            ++found;
        }
    if(!found){help_result_indices[0]=-1;help_copy(1,"Keine Treffer");found=1;}
    while(found<4){help_result_indices[found]=-1;help_copy((uint8_t)(found+1),"");++found;}
    nova_debug_string("UEFI:HELP-SEARCH-RESULTS\n");
}

bool bootmenu_help_search_begin(void)
{
    if(!initialized||current_view!=NOVA_VIEW_HELP)return false;
    help_search_editing=true;help_search_field->flags|=NOVA_CONTROL_FLAG_FOCUSED;
    nova_input_focus_set(help_search_field);bootmenu_tooltip_hide();
    nova_debug_string("UEFI:HELP-SEARCH-FOCUS\n");return true;
}

bool bootmenu_help_search_active(void){return help_search_editing;}

bool bootmenu_help_search_input(uint16_t scan_code,uint32_t unicode)
{
    if(!help_search_editing)return false;
    bool changed=false;
    if(scan_code==23||unicode==13){help_search_editing=false;
        help_search_field->flags&=~NOVA_CONTROL_FLAG_FOCUSED;
        nova_input_focus_set(menu_items[help_result_indices[0]>=0?1:0]);
        nova_debug_string("UEFI:HELP-SEARCH-COMPLETE\n");return true;}
    if(unicode==8)changed=nova_text_field_backspace(help_search_field);
    else if(scan_code==8)changed=nova_text_field_delete(help_search_field);
    else if(scan_code==3)changed=nova_text_field_move(help_search_field,1,false);
    else if(scan_code==4)changed=nova_text_field_move(help_search_field,-1,false);
    else if(scan_code==5)changed=nova_text_field_move(help_search_field,-32767,false);
    else if(scan_code==6)changed=nova_text_field_move(help_search_field,32767,false);
    else if(unicode>=0x20&&unicode!=0x7f)changed=nova_text_field_insert(help_search_field,unicode);
    if(changed){help_refresh_results();nova_debug_string("UEFI:HELP-SEARCH-INPUT\n");}
    return true;
}

const char *bootmenu_help_result_title(uint16_t selection)
{
    if(selection<1||selection>4||help_result_indices[selection-1]<0)return 0;
    return help_entries[(uint8_t)help_result_indices[selection-1]].title;
}

const char *bootmenu_help_result_details(uint16_t selection)
{
    if(selection<1||selection>4||help_result_indices[selection-1]<0)return 0;
    return help_entries[(uint8_t)help_result_indices[selection-1]].details;
}
void bootmenu_set_dialog_motion(uint8_t opacity,uint16_t scale_per_mille)
{
    dialog_opacity=opacity;
    if(scale_per_mille<900)scale_per_mille=900;
    if(scale_per_mille>1000)scale_per_mille=1000;
    dialog_scale=scale_per_mille;
}
void bootmenu_set_dialog_effects(bool enabled){dialog_effects_enabled=enabled;}

bool bootmenu_context_open(uint64_t selection)
{
    if(!initialized||nova_dialog_active()||selection>=bootmenu_item_count())return false;
    bootmenu_tooltip_hide();
    nova_menu_button_close(settings_accessories[0]);
    context_kind=NOVA_CONTEXT_ACTIONS;
    context_count=3;
    for(uint8_t i=0;i<3;++i){
        nova_control_set_text(context_items[i],context_action_text[i]);
        nova_control_set_accessibility(context_items[i],2,context_action_text[i],false);
    }
    context_visible=true;context_focus=0;context_target=(uint16_t)selection;
    nova_input_focus_set(context_items[0]);
    nova_debug_string("UEFI:CONTEXT-OPEN\n");
    return true;
}

bool bootmenu_theme_menu_open(void)
{
    if(!initialized||nova_dialog_active()||current_view!=NOVA_VIEW_SETTINGS||
       !nova_menu_button_open(settings_accessories[0]))return false;
    bootmenu_tooltip_hide();context_kind=NOVA_CONTEXT_THEMES;context_count=4;
    context_visible=true;
    context_focus=(uint8_t)nova_theme_active();context_target=0;
    for(uint8_t i=0;i<context_count;++i){
        nova_control_set_text(context_items[i],context_theme_text[i]);
        nova_control_set_accessibility(context_items[i],2,context_theme_text[i],false);
    }
    nova_input_focus_set(context_items[context_focus]);
    nova_debug_string("UEFI:MENU-BUTTON-OPEN\n");return true;
}

bool bootmenu_quality_menu_open(void)
{
    if(!initialized||nova_dialog_active()||current_view!=NOVA_VIEW_SETTINGS||
       !nova_menu_button_open(settings_accessories[0]))return false;
    bootmenu_tooltip_hide();context_kind=NOVA_CONTEXT_QUALITY;context_count=6;
    context_visible=true;context_target=0;
    nova_quality_t configured=nova_configuration_effective()->quality;
    context_focus=configured==NOVA_QUALITY_AUTO?0:(uint8_t)configured+1;
    for(uint8_t i=0;i<context_count;++i){
        nova_control_set_text(context_items[i],context_quality_text[i]);
        nova_control_set_accessibility(context_items[i],2,context_quality_text[i],false);
    }
    nova_input_focus_set(context_items[context_focus]);
    nova_debug_string("UEFI:QUALITY-MENU-OPEN\n");return true;
}

bool bootmenu_context_active(void){return context_visible;}

bool bootmenu_context_move(int8_t direction)
{
    if(!context_visible||!direction)return false;
    context_focus=(uint8_t)((context_focus+(direction>0?1:context_count-1))%context_count);
    nova_input_focus_set(context_items[context_focus]);
    return true;
}

bool bootmenu_context_edge(bool end)
{
    if(!context_visible)return false;
    context_focus=end?(uint8_t)(context_count-1):0;
    nova_input_focus_set(context_items[context_focus]);
    nova_debug_string(end?"UEFI:CONTEXT-END\n":"UEFI:CONTEXT-HOME\n");return true;
}

bool bootmenu_context_activate(uint8_t *action)
{
    if(!context_visible||!action)return false;
    *action=(uint8_t)(context_focus|(context_kind==NOVA_CONTEXT_THEMES?0x80u:
        context_kind==NOVA_CONTEXT_QUALITY?0x40u:0u));
    context_visible=false;
    if(context_kind==NOVA_CONTEXT_THEMES||context_kind==NOVA_CONTEXT_QUALITY){
        nova_menu_button_close(settings_accessories[0]);
        nova_input_focus_set(settings_accessories[0]);
    }
    nova_debug_string("UEFI:CONTEXT-ACTION\n");
    return true;
}

void bootmenu_context_close(void)
{
    if(context_visible)nova_debug_string("UEFI:CONTEXT-CLOSE\n");
    context_visible=false;
    if(context_kind==NOVA_CONTEXT_THEMES||context_kind==NOVA_CONTEXT_QUALITY){
        nova_menu_button_close(settings_accessories[0]);
        nova_input_focus_set(settings_accessories[0]);
        nova_debug_string("UEFI:MENU-BUTTON-FOCUS-RESTORED\n");
    }
}

void bootmenu_tooltip_hide(void)
{
    if(tooltip_visible)overlay_reset_requested=true;
    tooltip_visible=false;tooltip_elapsed_ms=0;
}

bool bootmenu_recovery_self_test(void)
{
    if(!initialized||current_view!=NOVA_VIEW_DIAGNOSTICS)return false;
    if(!nova_recovery_report(0x80010001u,NOVA_UI_SUBSYSTEM_RENDERING,
        NOVA_UI_ERROR_CRITICAL,NOVA_RECOVERY_SAFE_MODE,0))return false;
    bootmenu_set_status("UI-Fehler behoben - sicherer Darstellungsmodus aktiv");
    nova_debug_string("UEFI:RECOVERY-SAFE-MODE\n");
    return nova_recovery_safe_mode()&&nova_recovery_continue_boot();
}

bool bootmenu_memory_self_test(void)
{
    if(!initialized||current_view!=NOVA_VIEW_DIAGNOSTICS)return false;
    nova_memory_reset_frame();
    void *normal=nova_memory_allocate(NOVA_MEMORY_FRAME,48,0x55495354u,16);
    void *render=nova_memory_allocate(NOVA_MEMORY_FRAME,96,0x55495354u,64);
    bool valid=normal&&render&&
        nova_memory_validate_pointer(normal,NOVA_MEMORY_FRAME,48)&&
        nova_memory_validate_pointer(render,NOVA_MEMORY_FRAME,96)&&
        nova_memory_reset_frame();
    if(!valid)return false;
    bootmenu_set_status("Speicherpools geprueft - Alignment und Frame-Reset bereit");
    memory_self_test_active=true;
    nova_debug_string("UEFI:MEMORY-SELF-TEST\n");return true;
}

bool bootmenu_runtime_self_test(void)
{
    if(!initialized||current_view!=NOVA_VIEW_DIAGNOSTICS||
       !nova_runtime_suspend()||nova_runtime_input_allowed()||
       !nova_runtime_resume()||!nova_runtime_input_allowed())return false;
    nova_state_object_t *scene=nova_state_create(0x7f00,NOVA_STATE_DOMAIN_SCENE,0,1,1);
    if(!scene||!nova_state_transition(scene,1)||!nova_state_checkpoint(scene)||
       !nova_state_transition(scene,2)||!nova_state_rollback(scene)||
       !nova_state_transition(scene,2)||!nova_state_transition(scene,3)||
       !nova_state_transition(scene,4))return false;
    nova_state_object_t *control=nova_state_create(0x7f01,NOVA_STATE_DOMAIN_CONTROL,0,1,scene->id);
    nova_state_snapshot_t snapshot;
    if(!control||!nova_state_transition(control,1)||!nova_state_transition(control,2)||
       !nova_state_transition(control,3)||!nova_state_transition(control,4)||
       !nova_state_serialize(control,&snapshot)||!nova_state_transition(control,7)||
       !nova_state_deserialize(control,&snapshot)||control->current!=4||
       !nova_state_transition(control,7)||!nova_state_transition(control,2)||
       !nova_state_transition(control,8)||!nova_state_destroy(control)||
       !nova_state_transition(scene,5)||!nova_state_transition(scene,6)||
       !nova_state_destroy(scene))return false;
    bootmenu_set_status("State Model geprueft - Lifecycle, Rollback und Restore bereit");
    runtime_self_test_active=true;
    state_model_self_test_active=true;
    nova_debug_string("UEFI:STATE-MODEL-SELF-TEST\n");
    nova_debug_string("UEFI:RUNTIME-LIFECYCLE-SELF-TEST\n");return true;
}

bool bootmenu_diagnostics_refresh(void)
{
    if(!initialized||current_view!=NOVA_VIEW_DIAGNOSTICS)return false;
    nova_diag_snapshot();
    const nova_diag_statistics_t *diagnostics=nova_diag_statistics();
    const nova_render_statistics_t *render=nova_render_statistics_current();
    static char summary[96];uint32_t p=0;
    const char *prefix=diagnostics->errors?"Fehler | ":diagnostics->warnings?"Warnung | ":"Bereit | ";
    while(prefix[p]&&p+1<sizeof(summary)){summary[p]=prefix[p];++p;}summary[p]=0;
    const char *fps="Rendering: ";for(uint32_t i=0;fps[i]&&p+1<sizeof(summary);++i)summary[p++]=fps[i];
    uint32_t values[3]={render->fps,(uint32_t)(render->frame_time_us/1000u),render->draw_calls};
    const char *suffix[3]={" FPS | "," ms | "," Draw Calls"};
    for(uint8_t v=0;v<3;++v){char digits[10];uint8_t n=0;
        if(!values[v])digits[n++]='0';else while(values[v]&&n<sizeof(digits)){
            digits[n++]=(char)('0'+values[v]%10u);values[v]/=10u;}
        while(n&&p+1<sizeof(summary))summary[p++]=digits[--n];
        for(uint32_t i=0;suffix[v][i]&&p+1<sizeof(summary);++i)summary[p++]=suffix[v][i];}
    summary[p]=0;bootmenu_set_status(summary);
    nova_debug_string("UEFI:DIAGNOSTICS-REFRESH\n");return true;
}

bool bootmenu_diagnostics_export(void)
{
    static uint8_t report[8192],input_report[8192],animation_report[8192],resource_report[65536];
    uint32_t written=0,input_written=0,animation_written=0,resource_written=0;
    if(!nova_input_trace_enabled()){
        if(!nova_input_trace_set_enabled(true,true))return false;
        bootmenu_set_status("Eingabe-Tracing aktiv - Ereignisse erneut wählen, um Berichte zu exportieren");
        nova_debug_string("UEFI:INPUT-TRACE-ENABLED\n");return true;
    }
    if(!initialized||current_view!=NOVA_VIEW_DIAGNOSTICS||
       !nova_diag_export(NOVA_DIAG_EXPORT_NDF,true,report,sizeof(report),&written)||
       !nova_input_trace_export(NOVA_INPUT_TRACE_NDF,true,input_report,
            sizeof(input_report),&input_written)||
       !nova_animation_diag_export(NOVA_ANIMATION_EXPORT_NDF,true,animation_report,
            sizeof(animation_report),&animation_written)||
       !nova_resource_diag_export(NOVA_RESOURCE_EXPORT_NDF,true,resource_report,
            sizeof(resource_report),&resource_written)){
        bootmenu_set_status("Diagnosebericht konnte nicht im lokalen Exportpuffer erstellt werden");
        return false;
    }
    (void)nova_input_trace_set_enabled(false,true);
    bootmenu_set_status("Diagnose-, Eingabe-, Animations- und Ressourcenbericht als NDF erstellt");
    nova_debug_string("UEFI:INPUT-TRACE-EXPORT\n");
    nova_debug_string("UEFI:DIAGNOSTICS-EXPORT\n");return written>0;
}

bool bootmenu_tick(uint32_t elapsed_ms)
{
    nova_runtime_tick(elapsed_ms);
    nova_input_advance_time(elapsed_ms);
    pointer_clock_ms+=elapsed_ms;
    if(!initialized||!tooltips_enabled||context_visible||nova_dialog_active()||transition_input_locked||
       tooltip_selection>=bootmenu_item_count())return false;
    if(tooltip_visible)return false;
    if(elapsed_ms>1000u)elapsed_ms=1000u;
    if(tooltip_elapsed_ms<tooltip_delay_ms)tooltip_elapsed_ms+=elapsed_ms;
    if(tooltip_elapsed_ms>=tooltip_delay_ms){tooltip_visible=true;
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
    if (current_view == NOVA_VIEW_FIRMWARE) return "Firmware-Einstellungen";
    return "NovaOS Bootmanager";
}

bool bootmenu_breadcrumb_focus(bool focused)
{
    if(!initialized||current_view==NOVA_VIEW_MAIN)return false;
    breadcrumb_root_focused=focused;base_dirty_requested=true;
    if(focused){breadcrumb_home_button->flags|=NOVA_CONTROL_FLAG_FOCUSED;
        nova_input_focus_set(breadcrumb_home_button);}
    else breadcrumb_home_button->flags&=~NOVA_CONTROL_FLAG_FOCUSED;
    if(focused)nova_debug_string("UEFI:BREADCRUMB-FOCUS\n");
    return true;
}

bool bootmenu_breadcrumb_focused(void){return breadcrumb_root_focused;}

bool bootmenu_breadcrumb_take_root_request(void)
{
    bool requested=breadcrumb_root_requested;
    breadcrumb_root_requested=false;
    return requested;
}

bool bootmenu_scroll_selection_into_view(UINTN selection)
{
    if(!initialized||current_view!=NOVA_VIEW_DIAGNOSTICS||selection>=6)return false;
    int32_t stride=current_layout.item_height+current_layout.item_gap;
    nova_rect_t child={0,(int32_t)selection*stride,current_layout.list.width,
                       current_layout.item_height};
    return nova_scroll_view_scroll_into_view(diagnostic_scroll_view,&child);
}

bool bootmenu_scroll_key(uint16_t scan_code,UINTN *selection)
{
    if(!selection||current_view!=NOVA_VIEW_DIAGNOSTICS)return false;
    UINTN next=*selection;
    if(scan_code==9)next=next>3?next-4:0;
    else if(scan_code==10)next=next+4<6?next+4:5;
    else if(scan_code==5)next=0;
    else if(scan_code==6)next=5;
    else return false;
    *selection=next;bootmenu_scroll_selection_into_view(next);
    nova_debug_string("UEFI:SCROLL-KEY\n");return true;
}

bool bootmenu_scroll_wheel(int32_t wheel,UINTN *selection)
{
    if(!selection||!wheel||current_view!=NOVA_VIEW_DIAGNOSTICS)return false;
    if(wheel>0&&*selection)*selection-=1;
    else if(wheel<0&&*selection<5)*selection+=1;
    else return false;
    bootmenu_scroll_selection_into_view(*selection);
    nova_debug_string("UEFI:SCROLL-WHEEL\n");return true;
}

bool bootmenu_tile_move(uint16_t scan_code,UINTN *selection)
{
    if(!selection||current_view!=NOVA_VIEW_RECOVERY||*selection>=6)return false;
    UINTN next=*selection;
    if(scan_code==4&&next%2)next--;
    else if(scan_code==3&&!(next%2))next++;
    else if(scan_code==1&&next>=2)next-=2;
    else if(scan_code==2&&next<4)next+=2;
    else return false;
    *selection=next;nova_input_focus_set(recovery_tiles[next]);
    nova_debug_string("UEFI:TILE-NAVIGATION\n");return true;
}

static bool point_in_control(const nova_control_t *control,int32_t x,int32_t y)
{
    if(!control||(control->flags&(NOVA_CONTROL_FLAG_VISIBLE|NOVA_CONTROL_FLAG_ENABLED))!=
       (NOVA_CONTROL_FLAG_VISIBLE|NOVA_CONTROL_FLAG_ENABLED))return false;
    nova_rect_t b=control->bounds;
    return x>=b.x&&y>=b.y&&x<b.x+b.width&&y<b.y+b.height;
}

static nova_control_t *bootmenu_pointer_target_at(int32_t x,int32_t y)
{
    /* Decorative icons are created after the list items and therefore win the
       generic reverse-order hit test. Map the complete visual item back to its
       semantic, invokable control. */
    nova_control_t *captured=nova_input_pointer_capture_owner(0);
    if(captured)return captured;
    if(nova_dialog_active())for(uint8_t i=0;i<NOVA_DIALOG_BUTTON_CAPACITY;++i)
        if(point_in_control(dialog_buttons[i],x,y))return dialog_buttons[i];
    if(context_visible)for(uint8_t i=0;i<context_count;++i)
        if(point_in_control(context_items[i],x,y))return context_items[i];
    if(current_view!=NOVA_VIEW_MAIN&&point_in_control(breadcrumb_home_button,x,y))
        return breadcrumb_home_button;
    if(current_view==NOVA_VIEW_HELP&&point_in_control(help_search_field,x,y))
        return help_search_field;
    if(current_view==NOVA_VIEW_SETTINGS)for(uint8_t i=0;i<4;++i)
        if(point_in_control(settings_accessories[i],x,y))return settings_accessories[i];
    for(uint8_t i=0;i<6;++i){
        nova_control_t *item=current_view==NOVA_VIEW_RECOVERY?recovery_tiles[i]:menu_items[i];
        if(point_in_control(item,x,y))return item;
    }
    return nova_control_hit_test(x,y);
}

static void route_pointer_events(nova_control_t *target,bool left,bool right)
{
    uint32_t buttons=(left?1u:0u)|(right?2u:0u);
    if(target!=pointer_event_target){
        if(pointer_event_target){nova_control_event_t leave={
            .type=NOVA_CONTROL_EVENT_MOUSE_LEAVE,.device_id=2,.x=pointer_x,.y=pointer_y,
            .value=(int32_t)buttons};
            (void)nova_control_dispatch_event(pointer_event_target,&leave);}
        if(target){nova_control_event_t enter={.type=NOVA_CONTROL_EVENT_MOUSE_ENTER,
            .device_id=2,.x=pointer_x,.y=pointer_y,.value=(int32_t)buttons};
            (void)nova_control_dispatch_event(target,&enter);}
        pointer_event_target=target;
    }
    if(target){
        nova_control_event_t move={.type=NOVA_CONTROL_EVENT_MOUSE_MOVE,.device_id=2,
            .x=pointer_x,.y=pointer_y,.value=(int32_t)buttons};
        (void)nova_control_dispatch_event(target,&move);
        bool pressed=(left&&!pointer_left)||(right&&!pointer_right);
        bool released=(!left&&pointer_left)||(!right&&pointer_right);
        if(pressed){nova_control_event_t button={.type=NOVA_CONTROL_EVENT_MOUSE_DOWN,
            .device_id=2,.x=pointer_x,.y=pointer_y,.value=(int32_t)buttons};
            (void)nova_control_dispatch_event(target,&button);
            if(left&&!pointer_left&&nova_input_pointer_click_update(2,0,target,
               pointer_x,pointer_y,pointer_clock_ms,true,
               nova_configuration_effective()->double_click_time_ms,
               (uint32_t)nova_dlu_to_pixels(4,current_layout.scale_milli))){
                nova_control_event_t double_click={
                    .type=NOVA_CONTROL_EVENT_DOUBLE_CLICK,.device_id=2,
                    .x=pointer_x,.y=pointer_y,.value=(int32_t)buttons};
                (void)nova_control_dispatch_event(target,&double_click);
                nova_debug_string("UEFI:CONTROL-DOUBLE-CLICK\n");
            }}
        if(released){nova_control_event_t button={.type=NOVA_CONTROL_EVENT_MOUSE_UP,
            .device_id=2,.x=pointer_x,.y=pointer_y,.value=(int32_t)buttons};
            (void)nova_control_dispatch_event(target,&button);
            if(!left&&pointer_left)(void)nova_input_pointer_click_update(2,0,target,
                pointer_x,pointer_y,pointer_clock_ms,false,
                nova_configuration_effective()->double_click_time_ms,
                (uint32_t)nova_dlu_to_pixels(4,current_layout.scale_milli));}
    }
}

bool bootmenu_key_event(uint16_t scan_code,uint32_t unicode)
{
    if(!initialized||(!scan_code&&!unicode))return false;
    nova_control_t *target=nova_input_focus_get();
    if(!target)return false;
    nova_control_event_t key={.type=NOVA_CONTROL_EVENT_KEY_DOWN,.device_id=1,
        .key=scan_code,.character=unicode};
    bool dispatched=nova_control_dispatch_event(target,&key);
    /* Editable controls emit CharacterInput at the point where insertion is
       accepted, avoiding duplicate text events for rejected characters. */
    if(unicode>=0x20&&target->type!=NOVA_CONTROL_TEXT_FIELD&&
       target->type!=NOVA_CONTROL_PASSWORD_FIELD){nova_control_event_t character={
        .type=NOVA_CONTROL_EVENT_CHARACTER_INPUT,.device_id=1,.key=scan_code,
        .character=unicode};
        dispatched=nova_control_dispatch_event(target,&character)&&dispatched;}
    return dispatched;
}

static bool bootmenu_pointer_apply(int32_t first,int32_t second,bool absolute,
                                   bool left,bool right,UINTN *selection,bool *activate)
{
    if (!initialized || !selection || !activate) return false;
    if (transition_input_locked) return false;
    bootmenu_tooltip_hide();
    int32_t width = (int32_t)nova_graphics_width();
    int32_t height = (int32_t)nova_graphics_height();
    uint16_t pointer_speed=nova_configuration_effective()->mouse_speed_percent;
    if(absolute){pointer_x=first;pointer_y=second;}
    else{
        pointer_x+=nova_input_pointer_scale_delta(first,pointer_speed);
        pointer_y+=nova_input_pointer_scale_delta(second,pointer_speed);
    }
    if (pointer_x < 0) pointer_x = 0;
    if (pointer_y < 0) pointer_y = 0;
    if (pointer_x >= width) pointer_x = width - 1;
    if (pointer_y >= height) pointer_y = height - 1;
    route_pointer_events(bootmenu_pointer_target_at(pointer_x,pointer_y),left,right);

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

    if(current_view==NOVA_VIEW_SETTINGS&&!context_visible){
        nova_rect_t theme_button=settings_accessories[0]->bounds;
        bool over_theme=pointer_x>=theme_button.x&&pointer_y>=theme_button.y&&
            pointer_x<theme_button.x+theme_button.width&&
            pointer_y<theme_button.y+theme_button.height;
        if(over_theme)settings_accessories[0]->flags|=NOVA_CONTROL_FLAG_HOVERED;
        else settings_accessories[0]->flags&=~NOVA_CONTROL_FLAG_HOVERED;
        if(left&&!pointer_left&&over_theme){
            *selection=0;nova_input_focus_set(settings_accessories[0]);
            bootmenu_theme_menu_open();pointer_left=left;
            nova_debug_string("UEFI:MENU-BUTTON-POINTER\n");return true;
        }
    }

    if(right&&!pointer_right&&!context_visible){
        nova_control_t *hover=nova_control_hit_test(pointer_x,pointer_y);
        if(hover&&(hover->type==NOVA_CONTROL_LIST_ITEM||hover->type==NOVA_CONTROL_TILE))
            *selection=hover->action;
        bootmenu_context_open(*selection);
    }
    pointer_right=right;
    if(context_visible){
        nova_rect_t bounds=context_bounds();
        int32_t row_h=bounds.height/context_count;
        bool inside=false;
        for(uint8_t i=0;i<context_count;++i)if(pointer_x>=bounds.x&&pointer_x<bounds.x+bounds.width&&
           pointer_y>=bounds.y+i*row_h&&pointer_y<bounds.y+(i+1)*row_h){
            inside=true;
            context_focus=i;nova_input_focus_set(context_items[i]);
            if(left&&!pointer_left)*activate=true;
        }
        if(left&&!pointer_left&&!inside)bootmenu_context_close();
        pointer_left=left;
        return true;
    }

    if(current_view!=NOVA_VIEW_MAIN){
        bool over=pointer_x>=breadcrumb_root_bounds.x&&pointer_y>=breadcrumb_root_bounds.y&&
            pointer_x<breadcrumb_root_bounds.x+breadcrumb_root_bounds.width&&
            pointer_y<breadcrumb_root_bounds.y+breadcrumb_root_bounds.height;
        if(breadcrumb_root_hovered!=over)base_dirty_requested=true;
        breadcrumb_root_hovered=over;
        if(left&&!pointer_left&&over){
            breadcrumb_root_focused=true;
            breadcrumb_root_requested=true;
            nova_input_focus_set(breadcrumb_home_button);
            nova_debug_string("UEFI:BREADCRUMB-POINTER\n");
        }
        if(over){pointer_left=left;return true;}
    }else{if(breadcrumb_root_hovered)base_dirty_requested=true;
        breadcrumb_root_hovered=false;}

    if(current_view==NOVA_VIEW_DIAGNOSTICS){
        nova_rect_t bar=diagnostic_scrollbar->bounds;
        bool over=pointer_x>=bar.x&&pointer_y>=bar.y&&pointer_x<bar.x+bar.width&&
                  pointer_y<bar.y+bar.height;
        if(left&&!pointer_left&&over){
            diagnostic_scroll_dragging=nova_input_pointer_capture(0,diagnostic_scrollbar);
            if(diagnostic_scroll_dragging)nova_debug_string("UEFI:POINTER-CAPTURE-BEGIN\n");
        }
        if(diagnostic_scroll_dragging&&left){
            int32_t relative=pointer_y-bar.y;
            int32_t target=bar.height>1?(int32_t)((int64_t)relative*
                diagnostic_scrollbar->maximum/(bar.height-1)):0;
            nova_scroll_view_scroll_to(diagnostic_scroll_view,0,target);
            int32_t stride=current_layout.item_height+current_layout.item_gap;
            UINTN nearest=(UINTN)((diagnostic_scroll_view->scroll_y+stride/2)/stride);
            if(nearest>5)nearest=5;
            *selection=nearest;
            nova_debug_string("UEFI:SCROLL-DRAG\n");
        }
        if(diagnostic_scroll_dragging&&!left){diagnostic_scroll_dragging=false;
            nova_input_pointer_release(0);
            nova_debug_string("UEFI:POINTER-CAPTURE-END\n");}
        if(over||diagnostic_scroll_dragging){pointer_left=left;return true;}
    }else if(diagnostic_scroll_dragging){diagnostic_scroll_dragging=false;
        nova_input_pointer_cancel(0);}

    if(current_view==NOVA_VIEW_SETTINGS){
        nova_rect_t checkbox=settings_accessories[2]->bounds;
        nova_rect_t slider=settings_accessories[3]->bounds;
        bool over_checkbox=pointer_x>=checkbox.x&&pointer_y>=checkbox.y&&
            pointer_x<checkbox.x+checkbox.width&&pointer_y<checkbox.y+checkbox.height;
        bool over_slider=pointer_x>=slider.x&&pointer_y>=slider.y&&
            pointer_x<slider.x+slider.width&&pointer_y<slider.y+slider.height;
        if(left&&!pointer_left&&over_checkbox){
            *selection=2;nova_input_focus_set(menu_items[2]);
            bootmenu_settings_toggle_tooltips();
            nova_debug_string("UEFI:SETTINGS-CHECKBOX-POINTER\n");
            pointer_left=left;return true;
        }
        if(left&&!pointer_left&&over_slider){
            settings_slider_dragging=nova_input_pointer_capture(0,settings_accessories[3]);
            if(settings_slider_dragging)nova_debug_string("UEFI:POINTER-CAPTURE-BEGIN\n");
        }
        if(settings_slider_dragging&&left){
            int32_t local=pointer_x-slider.x;
            if(local<0)local=0;
            if(local>slider.width)local=slider.width;
            int32_t raw=250+(local*1250+slider.width/2)/slider.width;
            int32_t snapped=250+((raw-250+125)/250)*250;
            if(snapped>1500)snapped=1500;
            if(nova_control_set_value(settings_accessories[3],snapped)&&
               configuration_commit(NOVA_CONFIG_TOOLTIP_DELAY,(uint32_t)snapped)){
                tooltip_delay_ms=(uint32_t)snapped;tooltip_delay_status();
                nova_debug_string("UEFI:SETTINGS-SLIDER-POINTER\n");
            }
            *selection=3;nova_input_focus_set(menu_items[3]);
            pointer_left=left;return true;
        }
        if(settings_slider_dragging&&!left){settings_slider_dragging=false;
            nova_input_pointer_release(0);
            nova_debug_string("UEFI:POINTER-CAPTURE-END\n");
            nova_debug_string("UEFI:SETTINGS-SLIDER-POINTER-END\n");}
    }
    if(current_view==NOVA_VIEW_HELP&&help_search_field){
        nova_rect_t field=help_search_field->bounds;
        bool over=pointer_x>=field.x&&pointer_y>=field.y&&
            pointer_x<field.x+field.width&&pointer_y<field.y+field.height;
        if(left&&!pointer_left&&over){*selection=0;bootmenu_help_search_begin();
            pointer_left=left;nova_debug_string("UEFI:HELP-SEARCH-POINTER\n");return true;}
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
    if(target&&(target->type==NOVA_CONTROL_LIST_ITEM||target->type==NOVA_CONTROL_TILE)){
        if(!bootmenu_item_available((uint16_t)target->action)){pointer_left=left;return true;}
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

bool bootmenu_pointer_event(int32_t dx,int32_t dy,bool left,bool right,
                            UINTN *selection,bool *activate)
{return bootmenu_pointer_apply(dx,dy,false,left,right,selection,activate);}

bool bootmenu_pointer_absolute_event(int32_t x,int32_t y,bool left,bool right,
                                     UINTN *selection,bool *activate)
{return bootmenu_pointer_apply(x,y,true,left,right,selection,activate);}

static bool pointer_uses_accessibility_size(void)
{
    /* High contrast is the existing user-selectable accessibility profile.
       Keep the ordinary pointer compact in the normal themes, but enlarge it
       together with the already enlarged accessible layout. */
    return current_layout.accessibility||nova_theme_is_high_contrast();
}

static nova_rect_t pointer_damage_bounds(int32_t x,int32_t y)
{
    int32_t scale=pointer_uses_accessibility_size()?2:1;
    return (nova_rect_t){x-2,y-2,24*scale,30*scale};
}

static nova_rect_t pointer_layer_bounds(void)
{
    int32_t scale=pointer_uses_accessibility_size()?2:1;
    return (nova_rect_t){pointer_x,pointer_y,18*scale,24*scale};
}

static void draw_pointer(nova_surface_t *surface,int32_t offset_x,int32_t offset_y)
{
    /* Alpha-faehige zentrale Pfeilform. Der Hochkontrastmodus erhält dieselbe
       Geometrie in zweifacher Größe, ohne den Pointer-Hotspot zu verschieben. */
    uint32_t pointer_color = nova_theme_tokens()->focus;
    int32_t scale=pointer_uses_accessibility_size()?2:1;
    for (int32_t row = 0; row < 14; ++row) {
        int32_t span = row < 9 ? row / 2 + 1 : 3;
        for(int32_t pixel_row=0;pixel_row<scale;++pixel_row)
            nova_surface_rect(surface,(nova_rect_t){pointer_x-offset_x,
                pointer_y-offset_y+row*scale+pixel_row,span*scale,1},pointer_color);
    }
    nova_surface_rect(surface,(nova_rect_t){pointer_x-offset_x+3*scale,
        pointer_y-offset_y+9*scale,2*scale,7*scale},pointer_color);
}

static nova_rect_t dialog_bounds(void)
{
    int32_t width = nova_dlu_to_pixels(460, current_layout.scale_milli);
    int32_t height = nova_dlu_to_pixels(230, current_layout.scale_milli);
    if (width > current_layout.safe.width - 32) width = current_layout.safe.width - 32;
    if (height > current_layout.safe.height - 32) height = current_layout.safe.height - 32;
    width=(int32_t)((int64_t)width*dialog_scale/1000);
    height=(int32_t)((int64_t)height*dialog_scale/1000);
    return (nova_rect_t){
        current_layout.safe.x + (current_layout.safe.width - width) / 2,
        current_layout.panel.y + (current_layout.panel.height - height) / 2,
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
    int32_t height=row_h*context_count;
    int32_t target_y=current_layout.list.y+context_target*
                     (current_layout.item_height+current_layout.item_gap);
    int32_t x=current_layout.list.x+current_layout.list.width-width-
              current_layout.item_padding;
    int32_t y=target_y+current_layout.item_height/2;
    if(context_kind==NOVA_CONTEXT_THEMES||context_kind==NOVA_CONTEXT_QUALITY){
        nova_rect_t anchor=settings_accessories[0]->bounds;
        x=anchor.x;y=anchor.y+anchor.height;
    }
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
    int32_t row_h=bounds.height/context_count;
    rounded_panel(surface,bounds.x-2,bounds.y-2,bounds.width+4,bounds.height+4,
                  theme->border);
    rounded_panel(surface,bounds.x,bounds.y,bounds.width,bounds.height,
                  theme->surface_secondary);
    nova_control_set_bounds(context_menu,bounds);
    for(uint8_t i=0;i<context_count;++i){
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
    int32_t maximum_width=nova_dlu_to_pixels(370,current_layout.scale_milli);
    nova_text_layout_request_t text_request={.utf8=tooltip_control->text,
        .available_width=maximum_width-nova_dlu_to_pixels(24,current_layout.scale_milli),
        .scale_milli=current_layout.scale_milli,.maximum_lines=1,
        .wrap=NOVA_TEXT_WRAP_NONE,.ellipsis=NOVA_TEXT_ELLIPSIS_END,
        .accessibility=current_layout.accessibility};
    nova_text_layout_metrics_t text_metrics;
    nova_rect_t desired;
    if(!nova_layout_apply_text(&text_request,(nova_layout_insets_t){12,11,12,11},
        0,0,&desired,&text_metrics))return;
    int32_t minimum_width=nova_dlu_to_pixels(180,current_layout.scale_milli);
    int32_t minimum_height=nova_dlu_to_pixels(42,current_layout.scale_milli);
    int32_t width=desired.width<minimum_width?minimum_width:desired.width;
    int32_t height=desired.height<minimum_height?minimum_height:desired.height;
    if(width>maximum_width)width=maximum_width;
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
    nova_debug_string("UEFI:TEXT-AWARE-TOOLTIP-READY\n");
    nova_debug_string("UEFI:TOOLTIP-FRAME-READY\n");
}

static uint32_t dialog_color(uint32_t color)
{
    const nova_render_quality_parameters_t *quality=nova_render_quality_parameters();
    if(quality&&!quality->transparency_enabled)return color|0xff000000u;
    uint32_t alpha=(color>>24)&0xffu;
    return (color&0x00ffffffu)|((alpha*dialog_opacity/255u)<<24);
}

static void alpha_over_rect(nova_surface_t *surface,nova_rect_t rect,uint32_t color)
{
    if(!surface||rect.width<=0||rect.height<=0)return;
    if(rect.x<0){rect.width+=rect.x;rect.x=0;}
    if(rect.y<0){rect.height+=rect.y;rect.y=0;}
    if(rect.x+rect.width>(int32_t)surface->width)rect.width=(int32_t)surface->width-rect.x;
    if(rect.y+rect.height>(int32_t)surface->height)rect.height=(int32_t)surface->height-rect.y;
    uint32_t alpha=(color>>24)&0xffu,inverse=255u-alpha;
    for(int32_t y=0;y<rect.height;++y)for(int32_t x=0;x<rect.width;++x){
        uint32_t *pixel=&surface->pixels[(rect.y+y)*surface->stride+rect.x+x];
        uint32_t old=*pixel;
        uint32_t r=(((color>>16)&0xffu)*alpha+((old>>16)&0xffu)*inverse)/255u;
        uint32_t g=(((color>>8)&0xffu)*alpha+((old>>8)&0xffu)*inverse)/255u;
        uint32_t b=((color&0xffu)*alpha+(old&0xffu)*inverse)/255u;
        *pixel=0xff000000u|(r<<16)|(g<<8)|b;
    }
    nova_damage_add(surface,rect);
}

static void draw_dialog(nova_surface_t *surface,int32_t x,int32_t y,
                        int32_t width,int32_t height)
{
    nova_dialog_t *dialog = nova_dialog_active();
    if (!dialog) return;
    if(dialog->type!=NOVA_DIALOG_CREDENTIAL&&dialog_password_field->text_length)
        nova_password_field_clear(dialog_password_field);
    nova_rect_t card = {x,y,width,height};
    const nova_theme_tokens_t *theme = nova_theme_tokens();
    const nova_shadow_style_t *shadow=nova_shadow_get(NOVA_ELEVATION_LEVEL4);
    const nova_material_style_t *material=nova_material_get(NOVA_DESIGN_MATERIAL_DIALOG);
    nova_round_rect_t shadow_geometry;
    nova_effect_object_t shadow_object;
    const nova_render_quality_parameters_t *quality=nova_render_quality_parameters();
    uint32_t semantic_border=material->border;
    nova_icon_token_t semantic_icon=NOVA_ICON_INFORMATION;
    if(dialog->type==NOVA_DIALOG_CONFIRMATION){
        if(dialog->confirmation_level==NOVA_CONFIRM_WARNING){semantic_border=theme->warning;
            semantic_icon=NOVA_ICON_WARNING;}
        else if(dialog->confirmation_level==NOVA_CONFIRM_CRITICAL){semantic_border=theme->error;
            semantic_icon=NOVA_ICON_ERROR;}
        else if(dialog->confirmation_level==NOVA_CONFIRM_SECURITY){semantic_border=theme->error;
            semantic_icon=NOVA_ICON_LOCK;}
        else semantic_border=theme->information;
    }else if(dialog->type==NOVA_DIALOG_WARNING){
        semantic_border=dialog->warning_level==NOVA_WARNING_LOW?theme->information:
            dialog->warning_level==NOVA_WARNING_MEDIUM?theme->warning:theme->error;
        static const nova_icon_token_t warning_icons[NOVA_WARNING_ICON_COUNT]={
            NOVA_ICON_WARNING,NOVA_ICON_DIAGNOSTICS,NOVA_ICON_LOCK,
            NOVA_ICON_FORWARD,NOVA_ICON_RECOVERY,NOVA_ICON_ERROR};
        semantic_icon=dialog->warning_icon<NOVA_WARNING_ICON_COUNT?
            warning_icons[dialog->warning_icon]:NOVA_ICON_WARNING;
    }
    if(dialog_effects_enabled&&!nova_motion_is_reduced()&&quality&&quality->shadows_enabled&&
       shadow&&!shadow->disabled&&
       nova_round_rect_create(&shadow_geometry,card.x,card.y,card.width,card.height,
        nova_radius_get(NOVA_RADIUS_XLARGE))&&nova_effect_object_create(&shadow_object,&shadow_geometry)){
        for(uint8_t layer=0;layer<shadow->layer_count;++layer){nova_effect_t effect={
            layer?NOVA_EFFECT_AMBIENT_SHADOW:NOVA_EFFECT_DROP_SHADOW,
            shadow->layers[layer].color,
            (float)nova_dlu_to_pixels(shadow->layers[layer].blur_dlu,current_layout.scale_milli),
            (float)shadow->layers[layer].opacity/255.0f,
            (float)nova_dlu_to_pixels((uint16_t)(shadow->layers[layer].x_dlu<0?
                -shadow->layers[layer].x_dlu:shadow->layers[layer].x_dlu),current_layout.scale_milli)*
                (shadow->layers[layer].x_dlu<0?-1.0f:1.0f),
            (float)nova_dlu_to_pixels((uint16_t)(shadow->layers[layer].y_dlu<0?
                -shadow->layers[layer].y_dlu:shadow->layers[layer].y_dlu),current_layout.scale_milli)*
                (shadow->layers[layer].y_dlu<0?-1.0f:1.0f)};
            (void)nova_shadow_set(&shadow_object,&effect);}
        (void)nova_effect_render(&shadow_object,surface,NOVA_EFFECT_PHASE_SHADOW,
             (nova_rect_t){0,0,(int32_t)surface->width,(int32_t)surface->height},0);}
    rounded_panel(surface, card.x - 2, card.y - 2, card.width + 4, card.height + 4,
                  dialog_color(dialog->type == NOVA_DIALOG_ERROR ? theme->error :
                  dialog->type == NOVA_DIALOG_WARNING ? theme->warning : semantic_border));
    rounded_panel(surface, card.x, card.y, card.width, card.height,
                  dialog_color(material->surface));
    int32_t light_inset=nova_dlu_to_pixels(16,current_layout.scale_milli);
    alpha_over_rect(surface,(nova_rect_t){card.x+light_inset,card.y+1,
        card.width-light_inset*2,1},(material->border&0x00ffffffu)|0x59000000u);
    int32_t padding = nova_dlu_to_pixels(20, current_layout.scale_milli);
    int32_t title_x=card.x+padding;
    if(dialog->type==NOVA_DIALOG_CONFIRMATION||dialog->type==NOVA_DIALOG_WARNING){
        int32_t icon_size=nova_dlu_to_pixels(22,current_layout.scale_milli);
        nova_icon_draw(surface,semantic_icon,title_x,card.y+padding,
                       (uint16_t)icon_size,dialog_color(semantic_border));
        title_x+=icon_size+nova_dlu_to_pixels(8,current_layout.scale_milli);
        nova_debug_string(dialog->type==NOVA_DIALOG_CONFIRMATION?
            "UEFI:CONFIRMATION-DIALOG-FRAME-READY\n":
            "UEFI:WARNING-DIALOG-FRAME-READY\n");
    }
    nova_text_draw_scaled(surface, title_x, card.y + padding,
                          card.x+card.width-padding-title_x, dialog->title,
                          dialog_color(theme->text_primary), NOVA_TEXT_LEFT, true,
                          current_layout.scale_milli);
    nova_text_draw_wrapped_scaled(surface, card.x + padding,
                          card.y + padding + nova_dlu_to_pixels(38,current_layout.scale_milli),
                          card.width - padding * 2, 3, dialog->message,
                          dialog_color(theme->text_secondary), current_layout.scale_milli);
    if(dialog->type==NOVA_DIALOG_WARNING){
        int32_t detail_y=card.y+padding+nova_dlu_to_pixels(92,current_layout.scale_milli);
        if(dialog->recommendation[0])
            nova_text_draw_wrapped_scaled(surface,card.x+padding,detail_y,
                card.width-padding*2,2,dialog->recommendation,
                dialog_color(theme->text_primary),current_layout.scale_milli);
        if(dialog->warning_code){
            char code[24]="Warncode: W-";uint8_t p=12;uint32_t value=dialog->warning_code;
            char reverse[10];uint8_t count=0;
            do{reverse[count++]=(char)('0'+value%10u);value/=10u;}while(value&&count<10);
            while(count){code[p++]=reverse[--count];}
            code[p]=0;
            nova_text_draw_scaled(surface,card.x+padding,detail_y+
                nova_dlu_to_pixels(22,current_layout.scale_milli),card.width-padding*2,
                code,dialog_color(theme->text_secondary),NOVA_TEXT_LEFT,true,
                current_layout.scale_milli);
        }
    }
    if (dialog->type == NOVA_DIALOG_PROGRESS) {
        int32_t bar_x = card.x + padding;
        int32_t bar_y = card.y + padding + nova_dlu_to_pixels(88,current_layout.scale_milli);
        int32_t bar_w = card.width - padding * 2;
        int32_t bar_h = nova_dlu_to_pixels(12,current_layout.scale_milli);
        dialog_spinner->style.background=theme->surface;
        dialog_spinner->style.foreground=theme->text_secondary;
        dialog_spinner->style.accent=theme->accent;
        if(dialog->progress_indeterminate){
            int32_t spinner_size=nova_dlu_to_pixels(22,current_layout.scale_milli);
            nova_control_set_bounds(dialog_spinner,(nova_rect_t){bar_x,bar_y-4,
                spinner_size,spinner_size});
            nova_activity_set_phase(dialog_spinner,dialog->progress_per_mille);
            nova_activity_start(dialog_spinner);
            nova_control_render(dialog_spinner,surface);
        }else nova_activity_stop(dialog_spinner);
        if(!dialog->progress_indeterminate){
            rounded_panel(surface,bar_x,bar_y,bar_w,bar_h,theme->surface);
            int32_t fill=(bar_w*dialog->progress_per_mille)/1000;
            if(fill>0)rounded_panel(surface,bar_x,bar_y,fill,bar_h,theme->accent);
        }
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
            nova_text_draw_scaled(surface,bar_x+(dialog->progress_indeterminate?
                nova_dlu_to_pixels(30,current_layout.scale_milli):0),bar_y+bar_h+6,
                bar_w-80-(dialog->progress_indeterminate?
                nova_dlu_to_pixels(30,current_layout.scale_milli):0),
                dialog->progress_status,theme->text_secondary,NOVA_TEXT_LEFT,true,
                current_layout.scale_milli);
    } else if (dialog->type == NOVA_DIALOG_CREDENTIAL) {
        int32_t field_x=card.x+padding;
        int32_t field_y=card.y+padding+nova_dlu_to_pixels(82,current_layout.scale_milli);
        int32_t field_w=card.width-padding*2;
        int32_t field_h=nova_dlu_to_pixels(38,current_layout.scale_milli);
        if(dialog_password_field->text_length!=dialog->credential_glyphs){
            nova_password_field_clear(dialog_password_field);
            for(uint16_t i=0;i<dialog->credential_glyphs&&
                i<dialog_password_field->maximum_length;++i)
                (void)nova_text_field_insert(dialog_password_field,'x');
        }
        nova_control_set_bounds(dialog_password_field,
            (nova_rect_t){field_x,field_y,field_w,field_h});
        dialog_password_field->style.background=theme->surface;
        dialog_password_field->style.foreground=theme->text_primary;
        dialog_password_field->style.accent=theme->focus;
        dialog_password_field->style.disabled=theme->border;
        dialog_password_field->style.error=theme->error;
        dialog_password_field->style.corner_dlu=theme->radius_small;
        dialog_password_field->style.padding_dlu=(uint16_t)nova_dlu_to_pixels(10,
            current_layout.scale_milli);
        nova_control_render(dialog_password_field,surface);
        nova_debug_string("UEFI:PASSWORD-FIELD-FRAME-READY\n");
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
        nova_control_t *control=dialog_buttons[i];
        bool destructive=i==dialog->destructive_button;
        bool secondary=dialog->button_results[i]==NOVA_DIALOG_RESULT_CANCEL||
                       dialog->button_results[i]==NOVA_DIALOG_RESULT_NO;
        nova_button_set_type(control,destructive?NOVA_BUTTON_DESTRUCTIVE:
                             secondary?NOVA_BUTTON_SECONDARY:NOVA_BUTTON_PRIMARY);
        nova_button_set_action(control,(uint32_t)dialog->button_results[i]);
        nova_control_set_text(control,dialog->buttons[i]);
        nova_control_set_accessibility(control,5,dialog->buttons[i],false);
        nova_control_set_bounds(control,button);
        control->style.background=destructive?theme->error:
            (i==dialog->focus?theme->selected:theme->surface);
        control->style.foreground=theme->text_primary;
        control->style.accent=theme->focus;
        control->style.disabled=theme->disabled;
        control->style.corner_dlu=theme->radius_small;
        control->flags&=~(NOVA_CONTROL_FLAG_FOCUSED|NOVA_CONTROL_FLAG_SELECTED);
        if(i==dialog->focus)control->flags|=NOVA_CONTROL_FLAG_FOCUSED|
                                           NOVA_CONTROL_FLAG_SELECTED;
        nova_control_render(control,surface);
    }
}

void bootmenu_draw(UINTN selection, uint8_t opacity)
{
    if (!initialized) return;
    if(nova_sw_renderer_begin_frame()!=NOVA_SW_RENDERER_OK)return;
    bool runtime_frame=nova_runtime_frame_begin();
    if(runtime_frame)for(uint8_t stage=NOVA_FRAME_INPUT;stage<=NOVA_FRAME_MOTION;++stage)
        nova_runtime_frame_step((nova_runtime_frame_stage_t)stage);
    if(selection<bootmenu_item_count()&&
       (tooltip_selection!=(uint16_t)selection||tooltip_view!=current_view)){
        tooltip_selection=(uint16_t)selection;tooltip_view=current_view;
        bootmenu_tooltip_hide();
        nova_control_set_text(tooltip_control,current_view==NOVA_VIEW_MAIN?
                              main_tooltips[selection]:
                              "Enter öffnet diese Option. Esc kehrt zurück.");
    }
    /* Selection markers and tooltips live on the transparent interaction
       surface. Painting only the new state cannot erase old opaque pixels,
       because transparent pixels intentionally preserve the framebuffer. */
    bool selection_changed=previous_selection!=selection;
    UINTN invalidated_selection=previous_selection;
    if(selection_changed){
        overlay_reset_requested=true;
    }
    sync_active_page();
    int32_t width = (int32_t)nova_graphics_width();
    const nova_theme_tokens_t *theme = nova_theme_tokens();
    nova_effect_begin_frame();
    const nova_render_quality_parameters_t *render_quality=nova_render_quality_parameters();
    nova_effect_set_quality(theme->opaque_materials?NOVA_EFFECT_QUALITY_LOW:
                            render_quality->effect_quality);
    if(styled_theme!=nova_theme_active()){
        nova_blur_clear_cache();
        if(!refresh_control_styles(theme)){
            if(runtime_frame)nova_runtime_frame_abort();
            return;
        }
    }
    if(runtime_frame)nova_runtime_frame_step(NOVA_FRAME_LAYOUT);
    if (!nova_layout_compute((uint32_t)width,nova_graphics_height(),
                             theme->high_contrast, &current_layout)){
        if(runtime_frame)nova_runtime_frame_abort();
        return;
    }
    if(runtime_frame){nova_runtime_frame_step(NOVA_FRAME_DIRTY_DETECTION);
        nova_runtime_frame_step(NOVA_FRAME_RENDER_QUEUE);
        nova_runtime_frame_step(NOVA_FRAME_RENDERING);}
    nova_memory_reset_frame();
    nova_rect_t *frame_safe=(nova_rect_t *)nova_memory_allocate(NOVA_MEMORY_FRAME,
        sizeof(nova_rect_t),0x4652414du,64);
    if(frame_safe)*frame_safe=current_layout.safe;
    nova_page_t *active_page=nova_page_active();
    if(active_page){
        nova_view_t *root=nova_page_root_view(active_page);
        if(root)nova_view_set_layout(root,current_layout.safe);
        if(!dialog_page&&view_content[current_view])
            nova_view_set_layout(view_content[current_view],current_layout.panel);
    }
    bool rebuild_base=!base_cache_valid||base_dirty_requested||
        cached_base_view!=current_view||cached_base_theme!=nova_theme_active()||
        cached_base_width!=(uint32_t)width||cached_base_height!=nova_graphics_height()||
        cached_base_scale!=current_layout.scale_milli;
    if(!nova_render_begin_frame()||
       (rebuild_base&&!nova_render_clear(base_surface,theme->background,NOVA_RENDER_BACKGROUND))||
       !nova_render_execute()){
        if(runtime_frame)nova_runtime_frame_abort();
        return;
    }
    if(rebuild_base){
    ++base_cache_generation;
    nova_control_set_bounds(page_card,current_layout.panel);
    page_card->style.background=theme->surface;
    page_card->style.foreground=theme->text_primary;
    page_card->style.accent=theme->accent;
    page_card->style.disabled=theme->border;
    page_card->style.corner_dlu=theme->radius_medium;
    page_card->style.border_dlu=theme->high_contrast?2:1;
    nova_control_render(page_card,base_surface);
    nova_compositor_set_material_tokens(theme->surface,theme->surface_secondary);
    rounded_panel(base_surface, current_layout.top_bar.x, current_layout.top_bar.y,
                  current_layout.top_bar.width, current_layout.top_bar.height, theme->accent);
    nova_control_set_bounds(brand_image,current_layout.brand);
    nova_image_set_tint(brand_image,theme->accent);
    nova_control_render(brand_image,base_surface);
    int32_t brand_text_width = current_layout.brand.width +
                               (current_layout.brand_compact ? 32 : 0);
    nova_control_set_bounds(brand_label,(nova_rect_t){current_layout.brand.x,
        current_layout.brand.y+current_layout.brand.height+4,brand_text_width,
        nova_dlu_to_pixels(24,current_layout.scale_milli)});
    brand_label->style.background=0;brand_label->style.foreground=theme->accent;
    nova_label_set_scale(brand_label,(uint16_t)current_layout.scale_milli);
    nova_control_render(brand_label,base_surface);
    nova_control_set_bounds(breadcrumb_control,current_layout.title);
    if(current_view==NOVA_VIEW_MAIN){
        breadcrumb_root_bounds=(nova_rect_t){0,0,0,0};
        breadcrumb_root_focused=false;
        breadcrumb_home_button->flags&=~NOVA_CONTROL_FLAG_VISIBLE;
        nova_control_set_text(breadcrumb_control,view_title());
        nova_text_draw_scaled(base_surface,current_layout.title.x,current_layout.title.y,
            current_layout.title.width,breadcrumb_control->text,theme->text_primary,
            NOVA_TEXT_LEFT,true,current_layout.scale_milli);
    }else{
        breadcrumb_home_button->flags|=NOVA_CONTROL_FLAG_VISIBLE;
        const char *root="Start";
        nova_control_set_text(breadcrumb_control,"Start / Unterseite");
        int32_t icon_size=nova_dlu_to_pixels(18,current_layout.scale_milli);
        int32_t icon_gap=nova_dlu_to_pixels(6,current_layout.scale_milli);
        int32_t text_width=nova_text_measure_scaled(root,current_layout.title.width,
                                                   current_layout.scale_milli).width;
        int32_t root_width=icon_size+icon_gap+text_width;
        int32_t root_pad=nova_dlu_to_pixels(4,current_layout.scale_milli);
        breadcrumb_root_bounds=(nova_rect_t){current_layout.title.x-root_pad,
            current_layout.title.y-root_pad,root_width+root_pad*2,
            nova_dlu_to_pixels(24,current_layout.scale_milli)};
        nova_control_set_bounds(breadcrumb_home_button,breadcrumb_root_bounds);
        breadcrumb_home_button->style.background=breadcrumb_root_focused?theme->selected:
            theme->surface_secondary;
        breadcrumb_home_button->style.foreground=theme->text_secondary;
        breadcrumb_home_button->style.accent=theme->focus;
        breadcrumb_home_button->style.corner_dlu=theme->radius_small;
        if(breadcrumb_root_hovered)breadcrumb_home_button->flags|=NOVA_CONTROL_FLAG_HOVERED;
        else breadcrumb_home_button->flags&=~NOVA_CONTROL_FLAG_HOVERED;
        nova_control_render(breadcrumb_home_button,base_surface);
        uint32_t root_color=breadcrumb_root_focused||breadcrumb_root_hovered?
            theme->text_primary:theme->text_secondary;
        nova_icon_draw(base_surface,(nova_icon_token_t)breadcrumb_home_button->value,
            current_layout.title.x,current_layout.title.y,(uint16_t)icon_size,root_color);
        nova_text_draw_scaled(base_surface,current_layout.title.x+icon_size+icon_gap,
            current_layout.title.y,current_layout.title.width-icon_size-icon_gap,root,
            root_color,NOVA_TEXT_LEFT,true,current_layout.scale_milli);
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
        nova_debug_string("UEFI:ICONBUTTON-FRAME-READY\n");
    }
    base_cache_valid=true;base_dirty_requested=false;cached_base_view=current_view;
    cached_base_theme=nova_theme_active();cached_base_width=(uint32_t)width;
    cached_base_height=nova_graphics_height();cached_base_scale=current_layout.scale_milli;
    nova_debug_string("UEFI:BASE-SURFACE-CACHE-MISS\n");
    }else nova_debug_string("UEFI:BASE-SURFACE-CACHE-HIT\n");

    bool complex_frame=nova_dialog_active()||tooltip_visible||context_visible||
        transition_offset_dlu!=0||transition_opacity!=255;
    bool opacity_changed=first_present_pending||previous_frame_opacity!=opacity;
    if(opacity_changed){
        /* The composed framebuffer is persistent.  A layer opacity change is
           therefore damage even when none of the cached base pixels changed.
           Without this, the entrance frame at opacity zero permanently left
           the brand, top bar and background outside the control panel black. */
        base_surface->full_damage=true;
        interaction_surface->full_damage=true;
    }
    if(first_present_pending||overlay_reset_requested||complex_frame||
       previous_complex_frame||rebuild_base){
        nova_surface_clear(interaction_surface,0x00000000u);
        /* Transparent pixels in the overlay intentionally do not overwrite the
           framebuffer.  Recompose the cached base first so pixels removed by
           a tooltip, context menu or previous selection cannot remain stale. */
        base_surface->full_damage=true;
        overlay_reset_requested=false;
    }else{
        nova_rect_t cursor_old=pointer_damage_bounds(previous_pointer_x,previous_pointer_y);
        nova_rect_t cursor_new=pointer_damage_bounds(pointer_x,pointer_y);
        nova_surface_rect(interaction_surface,current_layout.panel,0x00000000u);
        nova_surface_rect(interaction_surface,current_layout.status,0x00000000u);
        nova_surface_rect(interaction_surface,cursor_old,0x00000000u);
        nova_surface_rect(interaction_surface,cursor_new,0x00000000u);
        nova_damage_add(base_surface,current_layout.panel);
        nova_damage_add(base_surface,current_layout.status);
        nova_damage_add(base_surface,cursor_old);nova_damage_add(base_surface,cursor_new);
        nova_debug_string("UEFI:PARTIAL-INTERACTION-FRAME\n");
    }
    if(selection_changed){
        /* Keep the old and new selection areas damaged through tooltip state
           changes.  Clearing the transparent overlay alone changes pixels,
           but those pixels must also be replayed by the compositor so the old
           accent marker is restored from the base layer. */
        int32_t stride=current_layout.item_height+current_layout.item_gap;
        if(invalidated_selection<bootmenu_item_count())
            nova_damage_add(interaction_surface,(nova_rect_t){current_layout.list.x,
                current_layout.list.y+(int32_t)invalidated_selection*stride,
                current_layout.list.width,current_layout.item_height});
        if(selection<bootmenu_item_count())
            nova_damage_add(interaction_surface,(nova_rect_t){current_layout.list.x,
                current_layout.list.y+(int32_t)selection*stride,
                current_layout.list.width,current_layout.item_height});
        previous_selection=selection;
        nova_debug_string("UEFI:SELECTION-OLD-NEW-DAMAGED\n");
    }
    const nova_icon_token_t *icons=view_icons();
    int32_t scroll_offset=0;
    int32_t diagnostic_viewport_bottom=0;
    if(current_view==NOVA_VIEW_DIAGNOSTICS){
        int32_t stride=current_layout.item_height+current_layout.item_gap;
        int32_t viewport_height=stride*4-current_layout.item_gap;
        int32_t content_height=stride*6-current_layout.item_gap;
        nova_control_set_bounds(diagnostic_scroll_view,(nova_rect_t){current_layout.list.x,
            current_layout.list.y,current_layout.list.width,viewport_height});
        nova_scroll_view_configure(diagnostic_scroll_view,current_layout.list.width,
            viewport_height,current_layout.list.width,content_height);
        scroll_offset=diagnostic_scroll_view->scroll_y;
        diagnostic_viewport_bottom=current_layout.list.y+viewport_height;
    }
    for(uint16_t i=0;i<6;++i){
        if(current_view==NOVA_VIEW_RECOVERY){menu_items[i]->flags&=~NOVA_CONTROL_FLAG_VISIBLE;
            recovery_tiles[i]->flags|=NOVA_CONTROL_FLAG_VISIBLE;}
        else{recovery_tiles[i]->flags&=~NOVA_CONTROL_FLAG_VISIBLE;}
    }
    if(current_view!=NOVA_VIEW_RECOVERY){
        uint16_t first=0,count=6;
        if(current_view==NOVA_VIEW_DIAGNOSTICS){
            int32_t stride=current_layout.item_height+current_layout.item_gap;
            first=(uint16_t)((scroll_offset+stride-1)/stride);
            count=(uint16_t)(first+4<=6?4:6-first);
        }
        nova_list_set_virtual_window(menu_list,first,count);
        for(uint16_t i=0;i<6;++i){
            if(bootmenu_item_available(i)&&nova_list_item_visible(menu_list,i))
                menu_items[i]->flags|=NOVA_CONTROL_FLAG_VISIBLE;
            else menu_items[i]->flags&=~NOVA_CONTROL_FLAG_VISIBLE;
        }
        nova_list_select(menu_list,(uint16_t)selection);
    }
    for (uint16_t i = 0; i < 6; ++i) {
        nova_control_t *item=current_view==NOVA_VIEW_RECOVERY?recovery_tiles[i]:menu_items[i];
        if(!bootmenu_item_available(i)){
            item->flags&=~NOVA_CONTROL_FLAG_VISIBLE;
            continue;
        }
        if(current_view!=NOVA_VIEW_RECOVERY&&!nova_list_item_visible(menu_list,i)){
            item->flags&=~NOVA_CONTROL_FLAG_VISIBLE;
            continue;
        }
        item->flags|=NOVA_CONTROL_FLAG_VISIBLE;
        int32_t item_y=current_view==NOVA_VIEW_RECOVERY?
            current_layout.list.y+(i/2)*((current_layout.list.height-current_layout.item_gap*2)/3+
            current_layout.item_gap):current_layout.list.y+
            i*(current_layout.item_height+current_layout.item_gap)-scroll_offset;
        if(current_view==NOVA_VIEW_DIAGNOSTICS&&
           (item_y<current_layout.list.y||item_y+current_layout.item_height>
            diagnostic_viewport_bottom)){
            item->flags&=~NOVA_CONTROL_FLAG_VISIBLE;
            continue;
        }
        int32_t item_width=current_view==NOVA_VIEW_RECOVERY?
            (current_layout.list.width-current_layout.item_gap)/2:current_layout.list.width-
            (current_view==NOVA_VIEW_DIAGNOSTICS?
             nova_dlu_to_pixels(18,current_layout.scale_milli):0);
        int32_t item_x=current_view==NOVA_VIEW_RECOVERY?
            current_layout.list.x+(i%2)*(item_width+current_layout.item_gap):current_layout.list.x;
        int32_t item_height=current_view==NOVA_VIEW_RECOVERY?
            (current_layout.list.height-current_layout.item_gap*2)/3:current_layout.item_height;
        nova_control_set_bounds(item,(nova_rect_t){item_x,item_y,item_width,item_height});
        item->style.background=current_view==NOVA_VIEW_RECOVERY?theme->surface_secondary:
            (i==selection?theme->selected:0x00000000u);
        item->style.foreground = theme->text_primary;
        item->style.accent = theme->focus;
        item->style.disabled = theme->disabled;
        item->style.corner_dlu = theme->radius_medium;
        item->flags &= ~(NOVA_CONTROL_FLAG_FOCUSED | NOVA_CONTROL_FLAG_SELECTED);
        if (i == selection) {
            item->flags |= NOVA_CONTROL_FLAG_SELECTED;
            nova_input_focus_set(item);
        }
        if(current_view==NOVA_VIEW_RECOVERY)item->style.disabled=theme->selected;
        if(current_view==NOVA_VIEW_FIRMWARE&&i==4&&!firmware_setup_available){
            item->style.foreground=theme->disabled;
            item->flags&=~NOVA_CONTROL_FLAG_ENABLED;
        }else item->flags|=NOVA_CONTROL_FLAG_ENABLED;
        nova_control_render(item, interaction_surface);
        if(current_view==NOVA_VIEW_RECOVERY){
            int32_t pad=nova_dlu_to_pixels(14,current_layout.scale_milli);
            int32_t icon_size=nova_dlu_to_pixels(26,current_layout.scale_milli);
            nova_icon_control_set(item_icons[i],icons[i]);
            nova_control_set_bounds(item_icons[i],(nova_rect_t){item_x+pad,item_y+pad,
                                                               icon_size,icon_size});
            item_icons[i]->style.foreground=item->style.foreground;
            item_icons[i]->style.disabled=theme->disabled;
            nova_control_render(item_icons[i],interaction_surface);
            int32_t title_x=item_x+pad+icon_size+nova_dlu_to_pixels(10,current_layout.scale_milli);
            nova_text_draw_scaled(interaction_surface,title_x,item_y+pad,
                item_width-(title_x-item_x)-pad,item->text,item->style.foreground,
                NOVA_TEXT_LEFT,true,current_layout.scale_milli);
            nova_text_draw_wrapped_scaled(interaction_surface,item_x+pad,
                item_y+pad+nova_dlu_to_pixels(34,current_layout.scale_milli),item_width-pad*2,
                2,item->placeholder,theme->text_secondary,current_layout.scale_milli);
            int32_t badge_h=nova_dlu_to_pixels(24,current_layout.scale_milli);
            int32_t badge_icon=nova_dlu_to_pixels(16,current_layout.scale_milli);
            int32_t badge_w=nova_text_measure_scaled(recovery_badges[i]->text,item_width-pad*2,
                current_layout.scale_milli).width+badge_icon+nova_dlu_to_pixels(24,
                current_layout.scale_milli);
            if(badge_w>item_width-pad*2)badge_w=item_width-pad*2;
            nova_control_set_bounds(recovery_badges[i],(nova_rect_t){item_x+pad,
                item_y+item_height-pad-badge_h,badge_w,badge_h});
            recovery_badges[i]->style.background=theme->surface_secondary;
            recovery_badges[i]->style.foreground=theme->text_primary;
            recovery_badges[i]->style.accent=theme->accent;
            recovery_badges[i]->style.disabled=theme->disabled;
            recovery_badges[i]->style.border=theme->border;
            recovery_badges[i]->style.information=theme->information;
            recovery_badges[i]->style.success=theme->success;
            recovery_badges[i]->style.warning=theme->warning;
            recovery_badges[i]->style.error=theme->error;
            recovery_badges[i]->style.border_dlu=1;
            recovery_badges[i]->style.padding_dlu=(uint16_t)nova_dlu_to_pixels(7,
                current_layout.scale_milli);
            nova_control_render(recovery_badges[i],interaction_surface);
            continue;
        }
        int32_t content_y = item_y + (current_layout.item_height - current_layout.icon_size) / 2;
        int32_t icon_x = current_layout.list.x + current_layout.item_padding;
        if(bootmenu_item_available(i)){
            nova_icon_control_set(item_icons[i],icons[i]);
            nova_control_set_bounds(item_icons[i],(nova_rect_t){icon_x,content_y,
                current_layout.icon_size,current_layout.icon_size});
            item_icons[i]->style.foreground=item->style.foreground;
            item_icons[i]->style.disabled=theme->disabled;
            nova_control_render(item_icons[i],interaction_surface);
        }
        int32_t text_x = icon_x + current_layout.icon_size + current_layout.text_gap;
        if(current_view==NOVA_VIEW_HELP&&i==0){
            int32_t field_h=current_layout.item_height-nova_dlu_to_pixels(12,current_layout.scale_milli);
            nova_control_set_bounds(help_search_field,(nova_rect_t){text_x,item_y+(current_layout.item_height-field_h)/2,
                current_layout.list.x+current_layout.list.width-text_x-current_layout.item_padding,field_h});
            help_search_field->style.background=theme->surface_secondary;
            help_search_field->style.foreground=theme->text_primary;
            help_search_field->style.accent=theme->focus;
            help_search_field->style.disabled=theme->disabled;
            help_search_field->style.corner_dlu=theme->radius_small;
            help_search_field->style.padding_dlu=(uint16_t)nova_dlu_to_pixels(10,current_layout.scale_milli);
            if(help_search_editing)help_search_field->flags|=NOVA_CONTROL_FLAG_FOCUSED;
            else help_search_field->flags&=~NOVA_CONTROL_FLAG_FOCUSED;
            nova_control_render(help_search_field,interaction_surface);
        }else nova_text_draw_scaled(interaction_surface, text_x, content_y,
                         current_layout.list.x + item_width - text_x -
                         current_layout.item_padding,
                         item->text, item->style.foreground, NOVA_TEXT_LEFT, true,
                         current_layout.scale_milli);
        if (current_view == NOVA_VIEW_SETTINGS && i < 4) {
            nova_control_t *accessory=settings_accessories[i];
            int32_t accessory_h=nova_dlu_to_pixels(i==1?22:i==2?24:24,current_layout.scale_milli);
            int32_t total_w=nova_dlu_to_pixels(i==1?44:i==2?28:i==3?220:92,
                                               current_layout.scale_milli);
            int32_t accessory_w=i==3?nova_dlu_to_pixels(145,current_layout.scale_milli):total_w;
            if(accessory_h>current_layout.item_height-8)accessory_h=current_layout.item_height-8;
            int32_t accessory_x=current_layout.list.x+current_layout.list.width-
                                current_layout.item_padding-total_w;
            int32_t accessory_y=item_y+(current_layout.item_height-accessory_h)/2;
            nova_control_set_bounds(accessory,(nova_rect_t){accessory_x,accessory_y,
                                     accessory_w,accessory_h});
            accessory->style.background=theme->surface_secondary;
            accessory->style.foreground=theme->text_primary;
            accessory->style.accent=theme->accent;
            accessory->style.disabled=theme->disabled;
            accessory->style.corner_dlu=i==2?theme->radius_small:(uint16_t)(accessory_h/2);
            accessory->flags&=~(NOVA_CONTROL_FLAG_FOCUSED|NOVA_CONTROL_FLAG_SELECTED);
            if(i==0&&selection==0)accessory->flags|=NOVA_CONTROL_FLAG_FOCUSED;
            if(i==1)nova_control_set_checked(accessory,nova_theme_reduced_motion());
            if(i==2)nova_control_set_checked(accessory,tooltips_enabled);
            if(i==0)nova_control_set_text(accessory,
                nova_theme_active()==NOVA_THEME_LIGHT?"Light":
                nova_theme_active()==NOVA_THEME_HIGH_CONTRAST?"Kontrast":"Dark");
            nova_control_render(accessory,interaction_surface);
            if(i==3){
                char value[8];uint32_t delay=tooltip_delay_ms;uint8_t p=0;
                if(delay>=1000)value[p++]=(char)('0'+delay/1000);
                value[p++]=(char)('0'+(delay/100)%10);
                value[p++]=(char)('0'+(delay/10)%10);
                value[p++]=(char)('0'+delay%10);value[p++]=' ';value[p++]='m';value[p++]='s';value[p]=0;
                int32_t label_x=accessory_x+accessory_w+nova_dlu_to_pixels(8,current_layout.scale_milli);
                nova_text_draw_scaled(interaction_surface,label_x,
                    accessory_y+(accessory_h-nova_dlu_to_pixels(20,current_layout.scale_milli))/2,
                    total_w-accessory_w,value,theme->text_secondary,NOVA_TEXT_RIGHT,true,
                    current_layout.scale_milli);
            }
        }
    }
    if(current_view==NOVA_VIEW_DIAGNOSTICS){
        int32_t bar_w=nova_dlu_to_pixels(8,current_layout.scale_milli);
        nova_control_set_bounds(diagnostic_scrollbar,(nova_rect_t){
            current_layout.list.x+current_layout.list.width-bar_w,
            current_layout.list.y,bar_w,
            diagnostic_viewport_bottom-current_layout.list.y});
        diagnostic_scrollbar->style.background=theme->surface_secondary;
        diagnostic_scrollbar->style.accent=theme->accent;
        diagnostic_scrollbar->style.disabled=theme->disabled;
        nova_control_render(diagnostic_scrollbar,interaction_surface);
        nova_debug_string("UEFI:SCROLLVIEW-FRAME-READY\n");
    }
    if(current_view==NOVA_VIEW_SETTINGS){
        int32_t separator_y=current_layout.list.y+4*(current_layout.item_height+
                            current_layout.item_gap)-current_layout.item_gap;
        nova_control_set_bounds(section_separator,(nova_rect_t){current_layout.list.x+
            current_layout.item_padding,separator_y,current_layout.list.width-
            current_layout.item_padding*2,current_layout.item_gap});
        section_separator->style.disabled=theme->border;
        section_separator->style.foreground=theme->text_secondary;
        section_separator->style.border_dlu=1;
        nova_control_render(section_separator,interaction_surface);
    }
    if(current_view==NOVA_VIEW_RECOVERY)nova_debug_string("UEFI:TILE-FRAME-READY\n");
    if (status_text[0]) {
        status_label->flags|=NOVA_CONTROL_FLAG_VISIBLE;
        nova_control_set_text(status_label,status_text);
        nova_control_set_bounds(status_label,current_layout.status);
        status_label->style.background=0;status_label->style.foreground=theme->text_secondary;
        status_label->style.disabled=theme->disabled;
        nova_label_set_scale(status_label,(uint16_t)current_layout.scale_milli);
        nova_control_render(status_label,interaction_surface);
    }else status_label->flags&=~NOVA_CONTROL_FLAG_VISIBLE;
    draw_tooltip(interaction_surface);
    draw_context_menu(interaction_surface);
    if(nova_dialog_active()){
        nova_rect_t card=dialog_bounds();
        nova_round_rect_t blur_geometry;
        bool glass_blur_applied=false;
        if(!dialog_effects_enabled)
            nova_debug_string("UEFI:BACKGROUND-BLUR-LIFECYCLE-SKIP\n");
        else if(!render_quality->blur_enabled)
            nova_debug_string("UEFI:BACKGROUND-BLUR-QUALITY-SKIP\n");
        else if(theme->high_contrast)
            nova_debug_string("UEFI:BACKGROUND-BLUR-HIGH-CONTRAST-SKIP\n");
        else if(nova_motion_is_reduced())
            nova_debug_string("UEFI:BACKGROUND-BLUR-REDUCED-MOTION-SKIP\n");
        else {
          const nova_material_style_t *dialog_material=nova_material_get(NOVA_DESIGN_MATERIAL_DIALOG);
          if(dialog_material&&!dialog_material->opaque&&nova_round_rect_create(&blur_geometry,
            card.x,card.y,card.width,card.height,nova_radius_get(NOVA_RADIUS_XLARGE))){
            uint16_t requested=nova_dlu_to_pixels(dialog_material->blur_dlu,current_layout.scale_milli);
            float radius=(float)(render_quality->maximum_blur_radius<requested?
                render_quality->maximum_blur_radius:requested);
            nova_blur_region_t blur_region={card,radius,
                render_quality->blur_quality,NOVA_BLUR_KERNEL_BOX,
                &blur_geometry,0,base_cache_generation};
            uint32_t blur_hits=nova_blur_diagnostics()->cache_hits;
            nova_blur_result_t blur_result=nova_blur_render(&blur_region,base_surface,
                                                            interaction_surface);
            if(blur_result==NOVA_BLUR_OK){
                glass_blur_applied=true;
                nova_debug_string("UEFI:BACKGROUND-BLUR-FRAME\n");
                nova_debug_string(nova_blur_diagnostics()->cache_hits>blur_hits?
                    "UEFI:BACKGROUND-BLUR-CACHE-HIT\n":"UEFI:BACKGROUND-BLUR-CACHE-MISS\n");
            }else nova_debug_string("UEFI:BACKGROUND-BLUR-FAILED\n");
          }else nova_debug_string("UEFI:BACKGROUND-BLUR-GEOMETRY-FAILED\n");
        }
        if(nova_modal_dialog_backdrop_active()){
            const nova_material_style_t *overlay=nova_material_get(NOVA_DESIGN_MATERIAL_OVERLAY);
            uint8_t backdrop=render_quality->transparency_enabled&&!overlay->opaque?
                (uint8_t)((uint16_t)overlay->opacity*dialog_opacity/255u):255u;
            alpha_over_rect(interaction_surface,
                (nova_rect_t){0,0,width,(int32_t)nova_graphics_height()},
                (uint32_t)backdrop<<24);
            nova_debug_string("UEFI:MODAL-BACKDROP-FRAME\n");
        }
        draw_dialog(interaction_surface,card.x,card.y,card.width,card.height);
        const nova_material_style_t *glass=nova_material_get(NOVA_DESIGN_MATERIAL_DIALOG);
        if(glass_blur_applied)nova_debug_string("UEFI:GLASS-DIALOG-FRAME\n");
        else if(glass&&glass->opaque)
            nova_debug_string("UEFI:GLASS-DIALOG-OPAQUE-FALLBACK\n");
        else nova_debug_string("UEFI:GLASS-DIALOG-TRANSPARENT-FALLBACK\n");
        draw_pointer(interaction_surface,0,0);
    }else draw_pointer(interaction_surface,0,0);

    if(!nova_controls_sync_scene()||!nova_scene_traverse(0,0,true)){
        if(runtime_frame)nova_runtime_frame_abort();
        (void)nova_sw_renderer_report_failure(false);
        return;
    }
    nova_layer_manager_set_phase(NOVA_LAYER_PHASE_STATE_UPDATE);
    nova_rect_t viewport={0,0,width,(int32_t)nova_graphics_height()};
    if(!nova_managed_layer_set_bounds(managed_layers[NOVA_LAYER_BACKGROUND],viewport)||
       !nova_managed_layer_set_bounds(managed_layers[NOVA_LAYER_CONTENT],viewport)||
       !nova_managed_layer_set_bounds(managed_layers[NOVA_LAYER_CONTROL],viewport)||
       !nova_managed_layer_set_bounds(managed_layers[NOVA_LAYER_DIALOG],dialog_bounds())||
       !nova_managed_layer_set_bounds(managed_layers[NOVA_LAYER_OVERLAY],viewport)||
       !nova_managed_layer_set_bounds(managed_layers[NOVA_LAYER_CURSOR],
            pointer_layer_bounds())||
       !nova_managed_layer_set_visible(managed_layers[NOVA_LAYER_DIALOG],nova_dialog_active())||
       !nova_managed_layer_set_opacity(managed_layers[NOVA_LAYER_BACKGROUND],
            (uint16_t)((uint32_t)opacity*1000u/255u))||
       !nova_managed_layer_set_opacity(managed_layers[NOVA_LAYER_OVERLAY],
            (uint16_t)((uint32_t)opacity*transition_opacity*1000u/(255u*255u)))){
        if(runtime_frame)nova_runtime_frame_abort();
        (void)nova_sw_renderer_report_failure(false);
        return;
    }
    nova_layer_handle_t layer_order[16];
    if(nova_layer_manager_build_order(layer_order,16)<5){
        if(runtime_frame)nova_runtime_frame_abort();
        (void)nova_sw_renderer_report_failure(false);
        return;
    }

    nova_layer_t base = {
        1, base_surface, {0, 0, width, (int32_t)nova_graphics_height()},
        {0, 0, width, (int32_t)nova_graphics_height()}, 0, opacity,
        NOVA_MATERIAL_NONE, true, false, false, true
    };
    nova_layer_t interaction = {
        2, interaction_surface, {0, 0, width, (int32_t)nova_graphics_height()},
        {0, 0, width, (int32_t)nova_graphics_height()}, 10,
        (uint8_t)((uint16_t)opacity * transition_opacity / 255u),
        NOVA_MATERIAL_NONE, true, true, false, true
    };
    interaction.modal=nova_modal_dialog_active();
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
    if(!nova_render_end_frame()){
        if(runtime_frame)nova_runtime_frame_abort();
        (void)nova_sw_renderer_report_failure(false);
        return;
    }
    if(!nova_surface_manager_reset_frame()){
        if(runtime_frame)nova_runtime_frame_abort();
        (void)nova_sw_renderer_report_failure(false);
        return;
    }
    nova_layer_manager_set_phase(NOVA_LAYER_PHASE_COMPOSITING);
    nova_compositor_begin_frame();
    nova_compositor_submit_layer(&base);
    nova_compositor_submit_layer(&interaction);
    if(runtime_frame)nova_runtime_frame_step(NOVA_FRAME_COMPOSITOR);
    bool composed=nova_compositor_compose();
    if(runtime_frame)nova_runtime_frame_step(NOVA_FRAME_PRESENT);
    nova_layer_manager_set_phase(NOVA_LAYER_PHASE_PRESENT);
    nova_sw_renderer_result_t present_result=composed?
        nova_sw_renderer_present():nova_sw_renderer_report_failure(false);
    if(present_result!=NOVA_SW_RENDERER_OK){
        if(runtime_frame)nova_runtime_frame_abort();
        nova_debug_string("UEFI:GAL-PRESENT-FAILED\n");
        nova_memory_reset_frame();return;
    }
    (void)nova_sw_renderer_complete_frame(true,true);
    nova_layer_manager_set_phase(NOVA_LAYER_PHASE_STATE_UPDATE);
    nova_debug_string("UEFI:GAL-PRESENT\n");
    nova_debug_string("UEFI:MENU-DRAWN\n");
    nova_debug_string("UEFI:IMAGE-CONTROL-FRAME-READY\n");
    nova_debug_string("UEFI:ICON-CONTROL-FRAME-READY\n");
    nova_debug_string("UEFI:LIST-CONTROL-FRAME-READY\n");
    nova_debug_string("UEFI:STYLE-TEMPLATE-FRAME-READY\n");
    if(status_text[0])nova_debug_string("UEFI:LABEL-FRAME-READY\n");
    if(nova_recovery_safe_mode())nova_debug_string("UEFI:RECOVERY-FRAME-READY\n");
    if(current_view==NOVA_VIEW_SETTINGS)
        nova_debug_string("UEFI:SEPARATOR-FRAME-READY\n");
    if(current_view==NOVA_VIEW_RECOVERY)
        nova_debug_string("UEFI:STATUS-BADGE-FRAME-READY\n");
    if(context_visible&&context_kind==NOVA_CONTEXT_THEMES)
        nova_debug_string("UEFI:MENU-BUTTON-FRAME-READY\n");
    if(pointer_uses_accessibility_size())
        nova_debug_string("UEFI:ACCESSIBLE-CURSOR-FRAME\n");
    if (nova_dialog_active()) nova_debug_string("UEFI:DIALOG-FRAME-READY\n");
    if(frame_safe&&nova_memory_validate_pointer(frame_safe,NOVA_MEMORY_FRAME,
       sizeof(nova_rect_t)))nova_debug_string("UEFI:MEMORY-FRAME-READY\n");
    if(memory_self_test_active)nova_debug_string("UEFI:MEMORY-SELF-TEST-FRAME\n");
    if(runtime_self_test_active)
        nova_debug_string("UEFI:RUNTIME-LIFECYCLE-SELF-TEST-FRAME\n");
    if(state_model_self_test_active)
        nova_debug_string("UEFI:STATE-MODEL-SELF-TEST-FRAME\n");
    previous_pointer_x=pointer_x;previous_pointer_y=pointer_y;
    previous_complex_frame=complex_frame;
    previous_frame_opacity=opacity;
    first_present_pending=false;
    if(runtime_frame){nova_runtime_frame_step(NOVA_FRAME_DIAGNOSTICS);
        nova_diag_frame(16667,500,500,8000,4000);nova_diag_snapshot();
        if(nova_runtime_frame_end())nova_debug_string("UEFI:RUNTIME-FRAME-COMPLETE\n");}
    nova_memory_reset_frame();
}
