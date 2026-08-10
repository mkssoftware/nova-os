#include <stdint.h>
#include <stdio.h>
#include "../boot/bootloader/bootmenu/motion.h"
#include "../boot/bootloader/bootmenu/compositor.h"
#include "../boot/bootloader/bootmenu/graphics.h"
#include "../boot/bootloader/bootmenu/resolution.h"
#include "../boot/bootloader/bootmenu/controls.h"
#include "../boot/bootloader/bootmenu/text.h"
#include "../boot/bootloader/bootmenu/unicode.h"
#include "../boot/bootloader/bootmenu/resources.h"
#include "../boot/bootloader/bootmenu/icons.h"
#include "../boot/bootloader/bootmenu/input.h"
#include "../boot/bootloader/bootmenu/diagnostics.h"
#include "../boot/bootloader/bootmenu/recovery.h"
#include "../boot/bootloader/bootmenu/memory.h"
#include "../boot/bootloader/bootmenu/configuration.h"
#include "../boot/bootloader/bootmenu/runtime.h"
#include "../boot/bootloader/bootmenu/state_model.h"
#include "../boot/bootloader/bootmenu/branding.h"
#include "../boot/bootloader/bootmenu/design.h"
#include "../boot/bootloader/bootmenu/architecture.h"
#include "../boot/bootloader/bootmenu/scene_graph.h"
#include "../boot/bootloader/bootmenu/theme.h"
#include "../boot/bootloader/bootmenu/layout.h"
#include "../boot/bootloader/bootmenu/navigation.h"
#include "../boot/bootloader/bootmenu/dialog.h"
#include "../boot/bootloader/bootmenu/page.h"

static uint32_t runtime_task_order[4],runtime_task_count;
static uint32_t state_callback_count;
static void runtime_task(uint32_t id,void *context)
{(void)context;if(runtime_task_count<4)runtime_task_order[runtime_task_count++]=id;}
static void state_callback(const nova_state_object_t *object,uint8_t old_state,
                           uint8_t new_state,void *context)
{(void)object;(void)old_state;(void)new_state;(void)context;++state_callback_count;}
static uint32_t graphics_frontbuffer[80];

static int check(int condition, const char *message)
{
    if (!condition) { fprintf(stderr, "FAIL: %s\n", message); return 1; }
    return 0;
}

static bool page_event_handler(nova_view_t *view,uint32_t event,void *context)
{
    uint32_t *visits=(uint32_t *)context;++*visits;
    return event==77 && view->type==NOVA_VIEW_CONTENT;
}

int main(void)
{
    int failed = 0;
    nova_controls_initialize(0);
    nova_diag_initialize();
    nova_recovery_initialize();
    failed |= check(!nova_recovery_report(0,NOVA_UI_SUBSYSTEM_RENDERING,
                    NOVA_UI_ERROR_WARNING,NOVA_RECOVERY_RETRY,0),
                    "Recovery weist ungueltigen Fehlercode ab");
    failed |= check(nova_recovery_report(0x80010010u,NOVA_UI_SUBSYSTEM_LAYOUT,
                    NOVA_UI_ERROR_RECOVERABLE,NOVA_RECOVERY_RETRY,1)&&
                    nova_recovery_diagnostics()->state==NOVA_RECOVERY_VALIDATING&&
                    nova_recovery_validate(false,2)&&
                    nova_recovery_diagnostics()->level==NOVA_RECOVERY_LOCAL_FALLBACK&&
                    nova_recovery_diagnostics()->state==NOVA_RECOVERY_HEALTHY,
                    "Recovery eskaliert Retry deterministisch zum lokalen Fallback");
    nova_recovery_initialize();
    failed |= check(nova_recovery_report(0x80010011u,NOVA_UI_SUBSYSTEM_RENDERING,
                    NOVA_UI_ERROR_CRITICAL,NOVA_RECOVERY_SAFE_MODE,3)&&
                    nova_recovery_safe_mode()&&nova_recovery_continue_boot()&&
                    !nova_recovery_feature_enabled(NOVA_RECOVERY_FEATURE_MOTION)&&
                    !nova_recovery_feature_enabled(NOVA_RECOVERY_FEATURE_GLASS)&&
                    nova_diag_quality()->quality==NOVA_QUALITY_SAFE,
                    "Safe Mode deaktiviert Effekte und setzt sicheres Qualitaetsprofil");
    nova_recovery_initialize();
    failed |= check(nova_recovery_watchdog_configure(NOVA_UI_SUBSYSTEM_INPUT,10,100)&&
                    nova_recovery_watchdog_check(109)&&
                    !nova_recovery_watchdog_check(110)&&
                    nova_recovery_diagnostics()->watchdog_timeouts==1&&
                    nova_recovery_diagnostics()->level==NOVA_RECOVERY_DISABLE_SUBSYSTEM,
                    "Watchdog erkennt Timeout genau einmal und isoliert Subsystem");
    failed |= check(nova_recovery_report(0x8001ffffu,NOVA_UI_SUBSYSTEM_RESOURCES,
                    NOVA_UI_ERROR_FATAL,NOVA_RECOVERY_RETRY,120)&&
                    nova_recovery_text_mode()&&nova_recovery_continue_boot()&&
                    nova_recovery_record_count()>=2&&nova_recovery_record(0)!=0&&
                    nova_recovery_record(nova_recovery_record_count())==0,
                    "Fataler UI-Fehler wechselt in Textmodus und laesst Boot weiterlaufen");
    nova_recovery_initialize();nova_memory_initialize();
    void *memory16=nova_memory_allocate(NOVA_MEMORY_RUNTIME,33,0x101u,16);
    void *memory64=nova_memory_allocate(NOVA_MEMORY_RENDER,65,0x202u,64);
    failed |= check(memory16&&memory64&&((uintptr_t)memory16&15u)==0&&
                    ((uintptr_t)memory64&63u)==0&&
                    nova_memory_validate_pointer(memory16,NOVA_MEMORY_RUNTIME,33)&&
                    nova_memory_validate_pointer(memory64,NOVA_MEMORY_RENDER,65)&&
                    nova_memory_object(memory64)->owner==0x202u,
                    "Memory Manager allokiert und validiert 16/64-Byte ausgerichtet");
    failed |= check(nova_memory_retain(memory16)&&nova_memory_release(memory16)&&
                    nova_memory_object(memory16)->references==1&&
                    nova_memory_release(memory16)&&!nova_memory_release(memory16)&&
                    nova_memory_statistics()->double_frees==1,
                    "Referenzzaehlung und Double-Free-Erkennung");
    void *frame_a=nova_memory_allocate(NOVA_MEMORY_FRAME,31,0x303u,16);
    void *frame_b=nova_memory_allocate(NOVA_MEMORY_FRAME,77,0x303u,64);
    failed |= check(frame_a&&frame_b&&nova_memory_reset_frame()&&
                    nova_memory_pool_statistics(NOVA_MEMORY_FRAME)->used==0&&
                    !nova_memory_validate_pointer(frame_a,NOVA_MEMORY_FRAME,1)&&
                    nova_memory_statistics()->frame_resets==1,
                    "Frame-Arena wird atomar und ohne Einzelfreigaben geleert");
    _Alignas(64) static uint8_t tracked_static[128];
    failed |= check(nova_memory_track_static(NOVA_MEMORY_PERMANENT,tracked_static,
                    sizeof(tracked_static),0x404u,64)&&
                    nova_memory_object(tracked_static)->state==NOVA_MEMORY_OBJECT_STATIC&&
                    !nova_memory_release(tracked_static),
                    "Statische Permanent-Ressource wird zentral bilanziert");
    failed |= check(!nova_memory_allocate(NOVA_MEMORY_CACHE,600u*1024u,0x505u,64)&&
                    nova_memory_pool_statistics(NOVA_MEMORY_CACHE)->overflows==1&&
                    nova_memory_statistics()->recovery_requests==1&&
                    nova_recovery_safe_mode()&&nova_recovery_continue_boot(),
                    "Pool-Overflow aktiviert Recovery statt Speicher zu ueberschreiben");
    nova_configuration_initialize();
    const nova_boot_configuration_t *configuration=nova_configuration_get();
    failed |= check(configuration->theme==NOVA_THEME_DARK&&configuration->tooltips&&
                    configuration->tooltip_delay_ms==750&&
                    nova_configuration_validate(configuration)&&
                    configuration->checksum==nova_configuration_checksum(configuration),
                    "Versionierte sichere Konfigurationsdefaults");
    failed |= check(nova_configuration_begin()&&
                    nova_configuration_set(NOVA_CONFIG_THEME,NOVA_THEME_LIGHT)&&
                    nova_configuration_set(NOVA_CONFIG_TOOLTIP_DELAY,1000)&&
                    !nova_configuration_begin()&&nova_configuration_commit()&&
                    nova_configuration_get()->theme==NOVA_THEME_LIGHT&&
                    nova_configuration_get()->tooltip_delay_ms==1000&&
                    nova_configuration_diagnostics()->generation==2,
                    "Atomarer Konfigurationscommit und Benachrichtigung");
    failed |= check(nova_configuration_begin()&&
                    nova_configuration_set(NOVA_CONFIG_TOOLTIP_DELAY,333)&&
                    !nova_configuration_commit()&&
                    nova_configuration_get()->tooltip_delay_ms==1000&&
                    nova_configuration_diagnostics()->validation_errors==1,
                    "Ungueltige Transaktion wird vollstaendig verworfen");
    failed |= check(nova_configuration_begin()&&
                    nova_configuration_set(NOVA_CONFIG_TOOLTIPS,0)&&
                    nova_configuration_rollback()&&nova_configuration_get()->tooltips,
                    "Expliziter Konfigurationsrollback");
    failed |= check(nova_configuration_override(NOVA_CONFIG_SAFE_MODE,1)&&
                    nova_configuration_effective()->safe_mode&&
                    !nova_configuration_get()->safe_mode&&
                    nova_configuration_clear_overrides()&&
                    !nova_configuration_effective()->safe_mode,
                    "Temporaerer Runtime-Override bleibt von gespeicherten Werten getrennt");
    failed |= check(nova_configuration_reset_defaults()&&
                    nova_configuration_get()->theme==NOVA_THEME_DARK&&
                    nova_configuration_get()->tooltip_delay_ms==750&&
                    nova_configuration_diagnostics()->resets==1,
                    "Konfiguration sicher auf Defaults zuruecksetzen");
    nova_boot_configuration_t corrupt=*nova_configuration_get();
    corrupt.theme=NOVA_THEME_LIGHT;
    failed |= check(!nova_configuration_validate(&corrupt),
                    "Beschaedigte Runtime-Pruefsumme erkennen");
    nova_runtime_create();
    failed |= check(!nova_runtime_run()&&nova_runtime_begin_initialization()&&
                    !nova_runtime_subsystem_ready(NOVA_RUNTIME_GRAPHICS)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_MEMORY)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_PLATFORM)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_GRAPHICS)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_DIAGNOSTICS)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_CONFIGURATION)&&
                    nova_runtime_loading()&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_RESOURCES)&&
                    nova_runtime_building_scene()&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_SCENE)&&
                    nova_runtime_layout()&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_LAYOUT_ENGINE)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_MOTION)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_INPUT)&&
                    nova_runtime_subsystem_ready(NOVA_RUNTIME_RENDERER)&&
                    nova_runtime_ready()&&nova_runtime_run()&&
                    nova_runtime_input_allowed(),
                    "Runtime initialisiert Subsysteme und Lifecycle strikt geordnet");
    failed |= check(nova_runtime_frame_begin()&&
                    nova_runtime_frame_step(NOVA_FRAME_INPUT)&&
                    !nova_runtime_frame_step(NOVA_FRAME_CONTROL_UPDATE)&&
                    nova_runtime_frame_abort(),
                    "Frame-Pipeline weist uebersprungene Phase ab");
    failed |= check(nova_runtime_frame_begin(),"Vollstaendigen Runtime-Frame beginnen");
    for(uint8_t stage=0;stage<NOVA_FRAME_STAGE_COUNT;++stage)
        failed|=check(nova_runtime_frame_step((nova_runtime_frame_stage_t)stage),
                      "Runtime-Framephase in fester Reihenfolge");
    failed|=check(nova_runtime_frame_end()&&
                  nova_runtime_diagnostics()->completed_frames==1,
                  "Runtime-Frame atomar abschliessen");
    runtime_task_count=0;
    nova_runtime_task_t idle={1,0,0,runtime_task,0,NOVA_TASK_IDLE,true,false,false};
    nova_runtime_task_t critical={2,0,0,runtime_task,0,NOVA_TASK_CRITICAL,true,false,false};
    failed|=check(nova_runtime_schedule(&idle)&&nova_runtime_schedule(&critical),
                  "Priorisierte Runtime-Aufgaben registrieren");
    nova_runtime_tick(1);
    failed|=check(runtime_task_count==2&&runtime_task_order[0]==2&&runtime_task_order[1]==1,
                  "Scheduler fuehrt Critical vor Idle deterministisch aus");
    state_callback_count=0;
    nova_state_object_t *scene=nova_state_create(100,NOVA_STATE_DOMAIN_SCENE,0,1,1);
    failed|=check(scene&&nova_state_set_callbacks(scene,state_callback,state_callback,
                  state_callback,0)&&nova_state_transition(scene,1)&&
                  nova_state_checkpoint(scene)&&nova_state_transition(scene,2)&&
                  nova_state_rollback(scene)&&scene->current==1&&
                  nova_state_transition(scene,2)&&nova_state_transition(scene,3)&&
                  nova_state_transition(scene,4),
                  "Hierarchischer Scene-Automat mit Events und Rollback");
    nova_state_object_t *state_control=nova_state_create(101,NOVA_STATE_DOMAIN_CONTROL,0,1,100);
    failed|=check(state_control&&nova_state_transition(state_control,1)&&
                  nova_state_transition(state_control,2)&&nova_state_transition(state_control,3)&&
                  !nova_state_transition(state_control,5)&&nova_state_transition(state_control,4)&&
                  nova_state_transition(state_control,5)&&nova_state_transition(state_control,6)&&
                  nova_state_transition(state_control,4),
                  "Control-Automat verwirft Spruenge und folgt Press/Release");
    nova_state_snapshot_t snapshot;
    failed|=check(nova_state_serialize(state_control,&snapshot),
                  "Versionierten State-Snapshot serialisieren");
    failed|=check(nova_state_transition(state_control,7),
                  "Control vor Snapshot-Restore veraendern");
    failed|=check(nova_state_deserialize(state_control,&snapshot)&&state_control->current==4,
                  "Versionierten State-Snapshot wiederherstellen");
    nova_state_snapshot_t corrupt_snapshot=snapshot;corrupt_snapshot.current=15;
    failed|=check(!nova_state_deserialize(state_control,&corrupt_snapshot)&&
                  state_callback_count>=15&&nova_state_diagnostics()->invalid_transitions>=1&&
                  nova_state_event(0)!=0,
                  "Korrupte Snapshots, Transitionsevents und Diagnostik");
    failed|=check(nova_state_transition(state_control,7)&&
                  nova_state_transition(state_control,2)&&
                  nova_state_transition(state_control,8)&&nova_state_destroy(state_control)&&
                  nova_state_transition(scene,5)&&nova_state_transition(scene,6)&&
                  nova_state_destroy(scene),
                  "Kind vor Elternteil deterministisch zerstoeren");
    failed|=check(nova_runtime_suspend()&&!nova_runtime_input_allowed()&&
                  nova_runtime_resume()&&nova_runtime_input_allowed()&&
                  nova_runtime_enter_recovery()&&!nova_runtime_input_allowed()&&
                  nova_runtime_leave_recovery(true)&&nova_runtime_input_allowed()&&
                  nova_runtime_shutdown()&&nova_runtime_destroy()&&
                  nova_runtime_state()==NOVA_RUNTIME_DESTROYED&&!nova_runtime_run(),
                  "Suspend, Recovery, Shutdown und Destroy Lifecycle");
    nova_page_model_initialize();
    nova_page_t *main_page=nova_page_create(1,"Bootmanager",11,false);
    nova_view_t *root_view=nova_view_create(main_page,100,NOVA_VIEW_ROOT,"Bootmanager",1,false);
    nova_view_t *header_view=nova_view_create(main_page,101,NOVA_VIEW_HEADER,"Kopfbereich",2,false);
    nova_view_t *content_view=nova_view_create(main_page,102,NOVA_VIEW_CONTENT,"Startauswahl",3,true);
    nova_view_t *footer_view=nova_view_create(main_page,103,NOVA_VIEW_FOOTER,"Status",4,false);
    failed |= check(main_page&&root_view&&header_view&&content_view&&footer_view&&
                    nova_page_set_root(main_page,root_view)&&
                    nova_view_add_child(root_view,header_view)&&
                    nova_view_add_child(root_view,content_view)&&
                    nova_view_add_child(root_view,footer_view),
                    "Page mit eindeutigem Root-View-Baum");
    failed |= check(!nova_view_add_child(content_view,root_view),"View-Zyklus abweisen");
    failed |= check(nova_view_initialize(root_view)&&nova_view_initialize(header_view)&&
                    nova_view_initialize(content_view)&&nova_view_initialize(footer_view)&&
                    nova_view_set_visibility(root_view,NOVA_VISIBILITY_VISIBLE)&&
                    nova_view_set_visibility(header_view,NOVA_VISIBILITY_VISIBLE)&&
                    nova_view_set_visibility(content_view,NOVA_VISIBILITY_VISIBLE)&&
                    nova_view_set_visibility(footer_view,NOVA_VISIBILITY_VISIBLE)&&
                    nova_page_load(main_page)&&nova_page_activate(main_page)&&
                    nova_page_set_focus(main_page,content_view),
                    "deterministische Page-/View-Lebenszyklen und Fokus");
    uint32_t page_visits=0;
    failed |= check(nova_page_dispatch(main_page,77,page_event_handler,&page_visits)&&page_visits>=2,
                    "iterative Eventweiterleitung durch View Tree");
    nova_page_t *dialog_page=nova_page_create(2,"Dialog",22,true);
    nova_view_t *dialog_root=nova_view_create(dialog_page,200,NOVA_VIEW_ROOT,"Dialog",5,false);
    nova_view_t *dialog_content=nova_view_create(dialog_page,201,NOVA_VIEW_DIALOG_CONTENT,"Dialoginhalt",6,true);
    failed |= check(dialog_page&&dialog_root&&dialog_content&&nova_page_set_root(dialog_page,dialog_root)&&
                    nova_view_add_child(dialog_root,dialog_content)&&nova_view_initialize(dialog_root)&&
                    nova_view_initialize(dialog_content)&&nova_view_set_visibility(dialog_root,NOVA_VISIBILITY_VISIBLE)&&
                    nova_view_set_visibility(dialog_content,NOVA_VISIBILITY_VISIBLE)&&nova_page_load(dialog_page)&&
                    nova_page_activate(dialog_page)&&main_page->state==NOVA_PAGE_SUSPENDED&&
                    nova_page_set_focus(dialog_page,dialog_content)&&nova_page_activate(main_page)&&
                    dialog_page->state==NOVA_PAGE_SUSPENDED&&nova_page_close(dialog_page),
                    "Dialog als eigene Page und Wiederaktivierung");
    nova_navigation_entry_t root = {0,2,17,48,0x1234};
    nova_navigation_initialize(root);
    failed |= check(!nova_navigation_can_go_back() &&
                    nova_navigation_current()->selection == 2,
                    "Navigation Root State");
    failed |= check(nova_navigation_push((nova_navigation_entry_t){1,0,22,0,9},
                                         NOVA_NAV_PUSH) &&
                    nova_navigation_diagnostics()->transition_running,
                    "Navigation Push mit Transition");
    nova_navigation_transition_complete();
    failed |= check(nova_navigation_update(4,26,72,10),
                    "Navigation State aktualisieren");
    nova_navigation_entry_t restored;
    failed |= check(nova_navigation_back(&restored) && restored.page == 0 &&
                    restored.selection == 2 && restored.focus_id == 17 &&
                    restored.scroll == 48 && restored.context == 0x1234,
                    "Back stellt Seite Fokus Scroll und Kontext wieder her");
    nova_navigation_transition_complete();
    failed |= check(!nova_navigation_back(&restored), "Back unter Root abweisen");
    failed |= check(nova_navigation_push((nova_navigation_entry_t){3,2,9,44,7},NOVA_NAV_PUSH),
                    "Navigation vor Reset erweitern");
    nova_navigation_transition_complete();
    failed |= check(nova_navigation_reset()&&!nova_navigation_can_go_back()&&
                    nova_navigation_current()->page==0&&nova_navigation_diagnostics()->resets==1,
                    "Navigation Reset behaelt ausschliesslich Root");

    nova_dialog_initialize();
    nova_dialog_t *confirm = nova_dialog_open(NOVA_DIALOG_CONFIRMATION,
        "Ausschalten", "NovaOS jetzt sicher herunterfahren?", true, true, 17);
    failed |= check(confirm && confirm->state == NOVA_DIALOG_ACTIVE &&
                    nova_dialog_diagnostics()->modal_active,
                    "modaler Dialog mit vollstaendigem Lifecycle");
    failed |= check(nova_dialog_add_button(confirm,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false) &&
                    nova_dialog_add_button(confirm,"Ausschalten",NOVA_DIALOG_RESULT_YES,true),
                    "Dialogaktionen anlegen");
    failed |= check(nova_dialog_focus_move(1) && confirm->focus == 1,
                    "Dialog Fokusfang");
    nova_dialog_result_t dialog_result = NOVA_DIALOG_RESULT_NONE;
    failed |= check(!nova_dialog_activate(&dialog_result) &&
                    confirm->destructive_armed == 1 &&
                    dialog_result == NOVA_DIALOG_RESULT_NONE,
                    "zerstoerende Aktion verlangt zweite Bestaetigung");
    failed |= check(nova_dialog_activate(&dialog_result) &&
                    dialog_result == NOVA_DIALOG_RESULT_YES &&
                    !nova_dialog_diagnostics()->modal_active,
                    "Dialog liefert genau ein Ergebnis");
    nova_dialog_t *warning = nova_dialog_open(NOVA_DIALOG_WARNING,
        "Nicht verfuegbar", "Backend fehlt", true, true, 22);
    nova_dialog_add_button(warning,"OK",NOVA_DIALOG_RESULT_OK,false);
    failed |= check(nova_dialog_cancel(&dialog_result) &&
                    dialog_result == NOVA_DIALOG_RESULT_CANCEL,
                    "Escape-Abbruch schliesst obersten Dialog");
    nova_dialog_t *progress_dialog = nova_dialog_open(NOVA_DIALOG_PROGRESS,
        "Diagnose", "Komponenten werden geprüft.", true, false, 22);
    failed |= check(progress_dialog &&
                    nova_dialog_progress_update(progress_dialog,-10,false,"Start") == false &&
                    progress_dialog->progress_per_mille == 0 &&
                    nova_dialog_progress_update(progress_dialog,1000,false,"Abgeschlossen") &&
                    progress_dialog->progress_per_mille == 1000,
                    "Progress wird begrenzt und aktualisiert");
    nova_control_t *activity=nova_control_create(NOVA_CONTROL_SPINNER);
    failed |= check(activity&&nova_control_set_state(activity,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(activity,NOVA_CONTROL_VISIBLE)&&
        nova_activity_start(activity)&&nova_activity_running(activity),"Activity Indicator startet");
    for(uint8_t style=0;style<4;++style)
        failed |= check(nova_activity_set_style(activity,(nova_activity_style_t)style)&&
            nova_activity_set_phase(activity,(uint16_t)(style*250)),"Activity-Stil und Phase");
    failed |= check(!nova_activity_set_style(activity,(nova_activity_style_t)4)&&
        !nova_activity_set_phase(activity,1001)&&nova_activity_stop(activity)&&
        !nova_activity_running(activity),"Activity-Validierung und Stop");
    nova_control_t *scroll_view=nova_control_create(NOVA_CONTROL_SCROLL_VIEW);
    nova_control_t *vertical_bar=nova_control_create(NOVA_CONTROL_SCROLLBAR);
    nova_control_t *horizontal_bar=nova_control_create(NOVA_CONTROL_SCROLLBAR);
    failed |= check(scroll_view&&vertical_bar&&horizontal_bar&&
        nova_scroll_view_configure(scroll_view,200,100,500,400)&&
        nova_scrollbar_attach(vertical_bar,scroll_view,NOVA_SCROLLBAR_VERTICAL)&&
        nova_scrollbar_attach(horizontal_bar,scroll_view,NOVA_SCROLLBAR_HORIZONTAL),
        "ScrollView und Scrollbars konfigurieren");
    failed |= check(nova_scroll_view_scroll_to(scroll_view,999,999)&&
        scroll_view->scroll_x==300&&scroll_view->scroll_y==300&&
        vertical_bar->value==300&&horizontal_bar->value==300,
        "Scrollposition und gekoppelte Scrollbars begrenzen");
    nova_rect_t scroll_child={40,50,20,20};
    failed |= check(nova_scroll_view_scroll_into_view(scroll_view,&scroll_child)&&
        scroll_view->scroll_x==40&&scroll_view->scroll_y==50,
        "Kind-Control automatisch sichtbar machen");
    failed |= check(!nova_scroll_view_configure(scroll_view,200,100,199,400)&&
        !nova_scrollbar_attach(vertical_bar,scroll_view,NOVA_SCROLLBAR_HORIZONTAL),
        "ungültige Scrollkonfiguration und Mehrfachbindung ablehnen");
    nova_control_t *card=nova_control_create(NOVA_CONTROL_CARD);
    nova_control_t *card_child=nova_control_create(NOVA_CONTROL_LABEL);
    failed |= check(card&&card_child&&nova_card_add_child(card,card_child),
        "Glass Card gruppiert Kind-Control");
    for(uint8_t type=0;type<=NOVA_CARD_CUSTOM;++type)
        failed|=check(nova_card_set_type(card,(nova_card_type_t)type),"Glass-Card-Typen");
    failed|=check(!nova_card_set_type(card,(nova_card_type_t)(NOVA_CARD_CUSTOM+1))&&
        !nova_card_add_child(card,card),"Glass Card validiert Typ und verhindert Rekursion");
    nova_control_t *tile=nova_control_create(NOVA_CONTROL_TILE);
    failed|=check(tile&&nova_control_set_text(tile,"Recovery")&&
        nova_tile_set_description(tile,"System sicher reparieren")&&
        nova_tile_set_status(tile,"Backend fehlt"),"Boot Option Tile Metadaten");
    for(uint8_t type=0;type<=NOVA_TILE_CUSTOM;++type)
        failed|=check(nova_tile_set_type(tile,(nova_boot_tile_type_t)type),"Boot Option Tile Typen");
    failed|=check(!nova_tile_set_type(tile,(nova_boot_tile_type_t)(NOVA_TILE_CUSTOM+1)),
        "Boot Option Tile validiert Typ");
    nova_control_t *icon_button=nova_control_create(NOVA_CONTROL_ICON_BUTTON);
    failed|=check(icon_button&&nova_icon_button_set_icon(icon_button,NOVA_ICON_HOME)&&
        nova_icon_button_set_tooltip(icon_button,"Zum Hauptmenü")&&
        nova_icon_button_set_action(icon_button,77)&&
        nova_control_set_accessibility(icon_button,4,"Zum Hauptmenü",false),
        "Icon Button Icon, Tooltip, Aktion und Accessibility");
    failed|=check(!nova_icon_button_set_icon(icon_button,NOVA_ICON_COUNT)&&
        !nova_icon_button_set_tooltip(icon_button,""),"Icon Button validiert Icon und Tooltip");
    nova_control_t *button=nova_control_create(NOVA_CONTROL_BUTTON);
    failed|=check(button&&nova_control_set_state(button,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(button,NOVA_CONTROL_VISIBLE)&&
        nova_button_set_action(button,91),"Button initialisieren und Aktion setzen");
    for(uint8_t type=0;type<=NOVA_BUTTON_TOGGLE;++type)
        failed|=check(nova_button_set_type(button,(nova_button_type_t)type),"Button-Typen");
    uint32_t button_action=0;
    failed|=check(nova_control_invoke(button,&button_action)&&button_action==91&&
        (button->flags&NOVA_CONTROL_FLAG_CHECKED)&&
        !nova_control_invoke(button,&button_action),"Toggle-Button genau einmal aktivieren");
    nova_control_release(button);button->flags|=NOVA_CONTROL_FLAG_BUSY;
    failed|=check(!nova_control_invoke(button,&button_action)&&
        !nova_button_set_type(button,(nova_button_type_t)(NOVA_BUTTON_TOGGLE+1)),
        "Busy- und Typvalidierung des Buttons");
    nova_control_t *bound_menu=nova_control_create(NOVA_CONTROL_CONTEXT_MENU);
    nova_control_t *menu_button=nova_control_create(NOVA_CONTROL_MENU_BUTTON);
    failed|=check(bound_menu&&menu_button&&
        nova_control_set_state(bound_menu,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(bound_menu,NOVA_CONTROL_VISIBLE)&&
        nova_control_set_state(menu_button,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(menu_button,NOVA_CONTROL_VISIBLE)&&
        nova_menu_button_bind(menu_button,bound_menu)&&
        nova_menu_button_open(menu_button)&&nova_menu_button_expanded(menu_button)&&
        !nova_menu_button_open(menu_button)&&nova_menu_button_close(menu_button)&&
        !nova_menu_button_expanded(menu_button),"Menu Button Bindung und Expanded-Zustand");
    failed|=check(!nova_menu_button_bind(menu_button,button)&&
        !nova_menu_button_open(0)&&!nova_menu_button_expanded(0),
        "Menu Button Fehlerfälle");
    nova_control_t *label=nova_control_create(NOVA_CONTROL_LABEL);
    failed|=check(label&&nova_control_set_state(label,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(label,NOVA_CONTROL_VISIBLE)&&nova_control_set_text(label,"Status bereit")&&
        nova_label_set_alignment(label,NOVA_ALIGN_RIGHT_BOTTOM)&&
        nova_label_set_scale(label,1500)&&nova_label_get_text(label)&&
        !nova_label_set_alignment(label,(nova_control_alignment_t)9)&&
        !nova_label_set_scale(label,499),"Label Text, Ausrichtung und DPI-Skalierung");
    for(uint8_t type=0;type<=NOVA_LABEL_INFORMATION;++type)
        failed|=check(nova_label_set_type(label,(nova_label_type_t)type),"Label-Typen");
    nova_control_t *icon_control=nova_control_create(NOVA_CONTROL_ICON);
    failed|=check(icon_control&&nova_control_set_state(icon_control,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(icon_control,NOVA_CONTROL_VISIBLE)&&
        nova_icon_control_set(icon_control,NOVA_ICON_SUCCESS)&&
        nova_icon_control_get(icon_control)==NOVA_ICON_SUCCESS&&
        !nova_icon_control_set(icon_control,NOVA_ICON_COUNT),"Icon-Control und Tokenvalidierung");
    nova_control_t *image_control=nova_control_create(NOVA_CONTROL_IMAGE);
    failed|=check(image_control&&nova_control_set_state(image_control,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(image_control,NOVA_CONTROL_VISIBLE)&&
        nova_image_set_resource(image_control,NOVA_IMAGE_BRANDING_LOGO)&&
        nova_image_set_tint(image_control,0xff218bd1u),"Image-Control und zentrale Ressource");
    for(uint8_t mode=0;mode<=NOVA_IMAGE_CENTER;++mode)
        failed|=check(nova_image_set_scaling(image_control,(nova_image_scaling_mode_t)mode),
                      "Image-Skalierungsmodi");
    failed|=check(!nova_image_set_scaling(image_control,(nova_image_scaling_mode_t)5),
                  "Image-Modusvalidierung");
    nova_control_t *separator=nova_control_create(NOVA_CONTROL_SEPARATOR);
    failed|=check(separator&&nova_control_set_state(separator,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(separator,NOVA_CONTROL_VISIBLE)&&
        nova_separator_set_orientation(separator,NOVA_SEPARATOR_HORIZONTAL)&&
        nova_separator_set_title(separator,"System")&&
        nova_separator_set_orientation(separator,NOVA_SEPARATOR_VERTICAL)&&
        !(separator->flags&NOVA_CONTROL_FLAG_ENABLED)&&
        !nova_separator_set_orientation(separator,(nova_separator_orientation_t)2),
        "Separator Ausrichtungen, Titel und Interaktionsfreiheit");
    nova_control_t *badge=nova_control_create(NOVA_CONTROL_STATUS_BADGE);
    failed|=check(badge&&nova_control_set_state(badge,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(badge,NOVA_CONTROL_VISIBLE)&&
        nova_control_set_text(badge,"Bereit")&&
        nova_status_badge_set_icon(badge,NOVA_ICON_SUCCESS)&&
        nova_status_badge_set_visible(badge,false)&&
        nova_status_badge_set_visible(badge,true)&&
        !(badge->flags&NOVA_CONTROL_FLAG_ENABLED),"StatusBadge Inhalt und Interaktionsfreiheit");
    for(uint8_t type=0;type<=NOVA_BADGE_CUSTOM;++type)
        failed|=check(nova_status_badge_set_type(badge,(nova_badge_type_t)type),"StatusBadge-Typen");
    failed|=check(!nova_status_badge_set_type(badge,(nova_badge_type_t)7)&&
        !nova_status_badge_set_icon(badge,NOVA_ICON_COUNT),"StatusBadge-Validierung");
    nova_control_style_t inherited_style=badge->style;
    inherited_style.accent=0xff123456u;inherited_style.corner_dlu=12;
    failed|=check(nova_style_define(10,0,&inherited_style,
        NOVA_STYLE_ACCENT|NOVA_STYLE_GEOMETRY,NOVA_CONTROL_MATERIAL_GLASS)&&
        nova_style_apply(badge,10)&&badge->style.accent==0xff123456u&&
        badge->style.foreground==nova_style_get(0)->resolved.foreground&&
        nova_style_get(10)->material==NOVA_CONTROL_MATERIAL_GLASS,
        "Style-Vererbung, Material und O(1)-Bindung");
    nova_style_theme_changed();
    failed|=check(badge->style_id==10&&badge->style.corner_dlu==12&&
        !nova_style_define(11,11,&inherited_style,NOVA_STYLE_ALL,NOVA_CONTROL_MATERIAL_OPAQUE),
        "Style-Neubindung und Rekursionsschutz");
    uint16_t badge_template=(uint16_t)(NOVA_CONTROL_STATUS_BADGE+1);
    failed|=check(nova_control_template_define(40,badge_template,
        NOVA_CONTROL_STATUS_BADGE,NOVA_TEMPLATE_PART_STATUS,
        NOVA_TEMPLATE_PART_BACKGROUND|NOVA_TEMPLATE_PART_TEXT,5)&&
        nova_control_template_apply(badge,40)&&
        nova_control_template_has_part(nova_control_template_get(40),"Background")&&
        nova_control_template_has_part(nova_control_template_get(40),"Status")&&
        !nova_control_template_has_part(nova_control_template_get(40),"Thumb"),
        "Template-Vererbung, Pflicht-Parts und Bindung");
    failed|=check(!nova_control_template_define(41,41,NOVA_CONTROL_STATUS_BADGE,
        NOVA_TEMPLATE_PART_TEXT,NOVA_TEMPLATE_PART_TEXT,1)&&
        !nova_control_template_apply(label,40),"Template-Rekursion und Typkompatibilitaet");
    nova_control_t *typed_list=nova_control_create(NOVA_CONTROL_LIST);
    nova_control_t *typed_items[3];
    failed|=check(typed_list&&nova_control_set_state(typed_list,NOVA_CONTROL_INITIALIZED)&&
        nova_control_set_state(typed_list,NOVA_CONTROL_VISIBLE),"List-Control initialisieren");
    for(uint8_t i=0;i<3;++i){
        typed_items[i]=nova_control_create(NOVA_CONTROL_LIST_ITEM);
        failed|=check(typed_items[i]&&nova_control_set_state(typed_items[i],NOVA_CONTROL_INITIALIZED)&&
            nova_control_set_state(typed_items[i],NOVA_CONTROL_VISIBLE)&&
            nova_list_item_set_subtitle(typed_items[i],"Beschreibung")&&
            nova_list_item_set_status(typed_items[i],"Bereit")&&
            nova_list_add_item(typed_list,typed_items[i]),"List Item einfügen");
    }
    failed|=check(nova_list_count(typed_list)==3&&nova_list_select(typed_list,1)&&
        nova_list_selected_index(typed_list)==1&&
        (typed_items[1]->flags&NOVA_CONTROL_FLAG_SELECTED),"O(1)-Einzelauswahl");
    failed|=check(nova_list_set_selection_mode(typed_list,NOVA_LIST_SELECTION_MULTIPLE)&&
        nova_list_select(typed_list,2)&&(typed_items[1]->flags&NOVA_CONTROL_FLAG_SELECTED)&&
        (typed_items[2]->flags&NOVA_CONTROL_FLAG_SELECTED),"Mehrfachauswahl");
    failed|=check(nova_list_set_virtual_window(typed_list,1,2)&&
        !nova_list_item_visible(typed_list,0)&&nova_list_item_visible(typed_list,1)&&
        nova_list_item_visible(typed_list,2),"List-Virtualisierungsfenster");
    failed|=check(nova_list_remove_item(typed_list,1)&&nova_list_count(typed_list)==2&&
        !nova_list_remove_item(typed_list,5)&&
        nova_list_item_set_state(typed_items[0],NOVA_LIST_ITEM_ERROR)&&
        (typed_items[0]->flags&NOVA_CONTROL_FLAG_ERROR),"Entfernung und List-Item-Zustände");
    nova_dialog_add_button(progress_dialog,"Schließen",NOVA_DIALOG_RESULT_OK,false);
    failed |= check(nova_dialog_activate(&dialog_result),"Progressdialog abschliessen");
    nova_dialog_t *credential = nova_dialog_open(NOVA_DIALOG_CREDENTIAL,
        "Volume entsperren", "Passphrase", true, true, 22);
    failed |= check(credential &&
                    nova_dialog_credential_configure(credential,NOVA_CREDENTIAL_PASSPHRASE) &&
                    nova_dialog_credential_input(credential,'N') &&
                    nova_dialog_credential_input(credential,0x00e4) &&
                    credential->credential_glyphs == 2 && credential->credential_bytes == 3 &&
                    nova_dialog_credential_backspace(credential) &&
                    credential->credential_glyphs == 1,
                    "maskierte UTF-8-Credential-Eingabe");
    nova_dialog_add_button(credential,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false);
    failed |= check(nova_dialog_cancel(&dialog_result) && credential->credential_bytes == 0 &&
                    credential->credential[0] == 0,
                    "Credential-Puffer beim Schliessen sicher geloescht");
    nova_layout_initialize();
    nova_resolution_initialize();
    failed|=check(nova_resolution_configure(1920,1080,96,false)&&
                  nova_resolution_viewport()->scale_milli==1000&&
                  nova_resolution_viewport()->resolution_class==NOVA_RESOLUTION_XL&&
                  nova_resolution_viewport()->aspect_ratio==NOVA_ASPECT_16_9&&
                  nova_resolution_viewport()->logical_width_dlu==1920,
                  "Full-HD-Referenz verwendet 100 Prozent und DLU-Viewport");
    const uint32_t resolutions[][2] = {
        {640,480},{800,600},{1024,768},{1280,720},{1280,800},
        {1366,768},{1600,900},{1920,1080},{2560,1440},{3840,2160}
    };
    for (uint32_t i = 0; i < sizeof(resolutions)/sizeof(resolutions[0]); ++i) {
        nova_boot_layout_t layout;
        failed |= check(nova_layout_compute(resolutions[i][0], resolutions[i][1],
                                            false, &layout),
                        "responsive Safe-Area-Berechnung");
        failed |= check(layout.panel.x >= layout.safe.x &&
                        layout.panel.y >= layout.safe.y &&
                        layout.panel.x + layout.panel.width <= layout.safe.x + layout.safe.width &&
                        layout.status.y + layout.status.height <= layout.safe.y + layout.safe.height,
                        "interaktive Flächen innerhalb der Safe Area");
    }
    failed|=check(nova_resolution_viewport()->resolution_class==NOVA_RESOLUTION_ULTRA&&
                  nova_resolution_viewport()->scale_milli==2000&&
                  nova_resolution_viewport()->logical_width_dlu==1920&&
                  nova_resolution_viewport()->safe_pixels.width<3840,
                  "UHD skaliert automatisch auf 200 Prozent mit Safe Area");
    failed|=check(nova_resolution_set_resolution(3440,1440)&&
                  nova_resolution_viewport()->aspect_ratio==NOVA_ASPECT_21_9&&
                  nova_resolution_viewport()->resolution_class==NOVA_RESOLUTION_XXL,
                  "UltraWide-Seitenverhaeltnis und Aufloesungsklasse");
    failed|=check(nova_resolution_set_scale(1750)&&
                  nova_logical_to_pixel(10)==18&&nova_pixel_to_logical(18)==10&&
                  !nova_resolution_set_scale(999)&&nova_resolution_set_automatic(),
                  "Benutzerskalierung, Pixelalignment und Rueckkehr zu Automatik");
    failed|=check(nova_resolution_set_resolution(1920,1080)&&
                  nova_resolution_set_dpi(144,true)&&
                  nova_resolution_viewport()->scale_milli==1500&&
                  nova_resolution_set_dpi(0,true)&&
                  !nova_resolution_viewport()->dpi_reliable,
                  "Verlaessliche DPI und sicherer DPI-Fallback");
    failed |= check(nova_dlu_to_pixels(10,1250)==13 &&
                    nova_dlu_to_pixels(-10,1250)==-13,
                    "deterministische DLU-Rundung");
    nova_size_constraints_t valid_constraints = {40,30,100,80,200,160};
    uint32_t constrained_width=0, constrained_height=0;
    failed |= check(nova_constraints_resolve(&valid_constraints,90,70,
                    &constrained_width,&constrained_height) &&
                    constrained_width==90 && constrained_height==70,
                    "priorisierte Größenconstraints");
    int32_t value = 0;
    nova_motion_initialize();
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_PUSH,true,0) &&
                    nova_navigation_visual()->offset_dlu == 32 &&
                    nova_navigation_diagnostics()->focus_locked,
                    "Navigation Enter sperrt Fokus und startet bei 32 DLU");
    nova_motion_update(90);
    failed |= check(nova_navigation_visual()->offset_dlu > 0 &&
                    nova_navigation_visual()->offset_dlu < 32 &&
                    nova_navigation_visual()->opacity > 0 &&
                    nova_navigation_visual()->opacity < 255,
                    "Slide und Fade laufen synchron");
    nova_motion_update(181);
    nova_navigation_visual_complete();
    failed |= check(nova_navigation_visual()->offset_dlu == 0 &&
                    nova_navigation_visual()->opacity == 255 &&
                    !nova_navigation_diagnostics()->focus_locked,
                    "Navigation gibt Fokus erst nach Enter frei");
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_POP,true,182) &&
                    nova_navigation_visual()->offset_dlu == -32,
                    "Back Transition ist spiegelbildlich");
    nova_navigation_visual_cancel();
    failed |= check(nova_navigation_diagnostics()->visual_cancelled == 1 &&
                    !nova_navigation_diagnostics()->focus_locked,
                    "Navigation Transition unterbrechen");
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_REPLACE,true,183) &&
                    nova_navigation_visual()->offset_dlu == 0,
                    "Replace verwendet Cross Fade ohne Richtung");
    nova_motion_update(364); nova_navigation_visual_complete();
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_RECOVERY,true,365) &&
                    nova_navigation_visual()->offset_dlu == 0 &&
                    nova_navigation_visual()->duration_ms == 240,
                    "Recovery verwendet ruhigen Fade");
    nova_motion_update(606); nova_navigation_visual_complete();
    nova_motion_initialize();
    nova_animation_t description = {
        &value, 0, 1000, 100, 20, 200, 7, 2, 0,
        NOVA_PROPERTY_X, NOVA_EASE_LINEAR, NOVA_MOTION_CREATED,
        false, false, true
    };
    nova_animation_t *animation = nova_motion_create(&description);
    failed |= check(animation != 0, "Animation anlegen");
    nova_motion_update(119);
    failed |= check(value == 0, "Startverzoegerung");
    nova_motion_update(220);
    failed |= check(value == 500, "zeitbasierte Interpolation");
    failed |= check(nova_motion_interrupt(animation), "Unterbrechung");
    nova_motion_update(260);
    failed |= check(value == 500, "Pause haelt Property");
    failed |= check(nova_motion_resume(animation, 260), "Fortsetzung");
    failed |= check(nova_motion_redirect(animation, 1500, 260), "Zielumleitung");
    nova_motion_update(460);
    failed |= check(value == 1500, "umgeleitetes Ziel");

    nova_motion_set_reduced(true);
    failed |= check(nova_navigation_visual_begin(NOVA_NAV_POP,true,500) &&
                    nova_navigation_visual()->offset_dlu == 0 &&
                    nova_navigation_visual()->reduced_motion,
                    "Reduced Motion entfernt seitliche Navigation");
    nova_motion_update(651);
    nova_navigation_visual_complete();
    int32_t reduced_value = 0;
    nova_animation_t reduced = description;
    reduced.target = &reduced_value;
    reduced.start_ms = 500;
    reduced.delay_ms = 0;
    reduced.easing = NOVA_EASE_SPRING;
    nova_animation_t *reduced_animation = nova_motion_create(&reduced);
    failed |= check(reduced_animation && reduced_animation->easing == NOVA_EASE_OUT_CUBIC,
                    "Reduced Motion ersetzt Spring");
    failed |= check(nova_motion_create(&reduced) == 0, "doppelte Property abweisen");
    nova_motion_update(1000);

    nova_motion_initialize();
    nova_motion_set_reduced(true);
    nova_dialog_motion_t dialog = {0};
    failed |= check(nova_dialog_enter(&dialog), "Dialog Enter Motion");
    failed |= check(dialog.scale==1000,"Reduced Motion entfernt Dialog-Scale");
    nova_motion_update(0);
    failed |= check(nova_dialog_motion_running(),"Reduced-Motion-Fade laeuft");
    nova_motion_update(1200);
    failed |= check(dialog.opacity == 255 && dialog.visible, "Dialog eingeblendet");
    failed |= check(nova_dialog_exit(&dialog), "Dialog Exit Motion");
    nova_motion_update(1400);
    failed |= check(dialog.opacity == 0, "Dialog ausgeblendet");
    nova_motion_set_reduced(false);
    dialog=(nova_dialog_motion_t){0};
    failed |= check(nova_dialog_enter(&dialog)&&dialog.scale==950,
                    "Standarddialog startet mit Fade und Scale");
    nova_dialog_motion_cancel(&dialog);
    failed |= check(!nova_dialog_motion_running(),"Dialogmotion sicher abbrechen");
    dialog=(nova_dialog_motion_t){0};
    failed |= check(nova_dialog_enter(&dialog),"Dialogmotion nach Abbruch erneut starten");
    nova_motion_update(1600);
    failed |= check(dialog.opacity==255&&dialog.scale==1000,
                    "Standarddialog beendet Enter fokussiert");
    failed |= check(nova_dialog_exit(&dialog),"Standarddialog Exit Motion");
    nova_motion_update(1800);
    failed |= check(dialog.opacity==0&&dialog.scale==950,
                    "Standarddialog beendet Exit verkleinert");

    nova_motion_set_reduced(true);
    nova_navigation_motion_t navigation = {0};
    failed |= check(nova_navigation_begin(&navigation, true) && navigation.target_x == 0,
                    "Reduced-Motion-Navigation als Cross Fade");
    nova_focus_motion_t focus = {0};
    failed |= check(nova_focus_set(&focus, true, true), "Fokus- und Auswahlmotion");
    nova_progress_motion_t progress = {0};
    failed |= check(nova_progress_set(&progress, 750), "determinate Progress Motion");
    failed |= check(!nova_progress_set(&progress, 1001) && progress.indeterminate,
                    "ungueltiger Progress faellt sicher zurueck");
    nova_motion_budget_update(20000, 1200);
    failed |= check(nova_motion_budget()->violations == 1 &&
                    nova_motion_budget()->quality == 3, "Budget-Degradation");

    nova_graphics_context_t invalid_graphics={.width=8,.height=8,.pitch=28,
        .bits_per_pixel=32,.pixel_format=NOVA_PIXEL_BGRA8888,
        .firmware=NOVA_GRAPHICS_FIRMWARE_TEST,.framebuffer=graphics_frontbuffer,
        .framebuffer_size=sizeof(graphics_frontbuffer)};
    failed|=check(!nova_graphics_initialize(&invalid_graphics),
                  "Graphics Context weist zu kleinen Pitch ab");
    nova_graphics_context_t graphics={.width=8,.height=8,.pitch=40,
        .bits_per_pixel=32,.pixel_format=NOVA_PIXEL_BGRA8888,
        .firmware=NOVA_GRAPHICS_FIRMWARE_TEST,.framebuffer=graphics_frontbuffer,
        .framebuffer_size=sizeof(graphics_frontbuffer),
        .capabilities=NOVA_GRAPHICS_LINEAR_FRAMEBUFFER};
    failed|=check(nova_graphics_initialize(&graphics)&&
                  nova_graphics_context()->scale_milli==1000&&
                  (nova_graphics_context()->capabilities&NOVA_GRAPHICS_DOUBLE_BUFFER),
                  "Validierter gemeinsamer Graphics Context");
    failed|=check(nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_RGBA8888,0,0,0,0)==0xff332211&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_BGRA8888,0,0,0,0)==0xff112233&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_RGB888,0,0,0,0)==0x00332211&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_BGR888,0,0,0,0)==0x00112233&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_RGB565,0,0,0,0)==0x1106&&
                  nova_graphics_convert_pixel(0xff112233,NOVA_PIXEL_BIT_MASK,0xf800,0x07e0,0x001f,0)==0x1106,
                  "RGBA-Konvertierung fuer RGB, BGR, RGB565 und Bitmask");
    failed |= check(nova_compositor_initialize(8, 8), "Compositor initialisieren");
    nova_surface_t *base = nova_surface_acquire();
    nova_surface_t *overlay = nova_surface_acquire();
    failed |= check(base && overlay, "feste Surface-Pools");

    nova_resource_manager_initialize();
    failed |= check(nova_theme_initialize() &&
                    nova_theme_validate(NOVA_THEME_DARK) &&
                    nova_theme_validate(NOVA_THEME_LIGHT) &&
                    nova_theme_validate(NOVA_THEME_HIGH_CONTRAST),
                    "drei vollständige Theme-Token-Tabellen");
    failed |= check(nova_theme_activate(NOVA_THEME_LIGHT) &&
                    nova_theme_tokens()->background == 0xfff2f4f7u,
                    "O(1)-Wechsel zum Light Theme");
    failed |= check(nova_theme_activate(NOVA_THEME_HIGH_CONTRAST) &&
                    nova_theme_reduced_motion() &&
                    nova_compositor_diagnostics()->fallback_level == 3,
                    "High Contrast überschreibt Motion und Materialien");
    failed |= check(nova_theme_activate(NOVA_THEME_DARK) &&
                    !nova_theme_reduced_motion() &&
                    nova_compositor_diagnostics()->fallback_level == 0,
                    "Dark Theme als validierter Standard");
    static const uint8_t resource_data[] = {1,2,3,4};
    failed |= check(nova_resource_register("boot://test/data", NOVA_RESOURCE_CONFIGURATION,
                    1, resource_data, sizeof(resource_data), 0, 0), "Ressource registrieren");
    uint64_t resource_id = nova_resource_id("boot://test/data");
    failed |= check(nova_resource_load(resource_id) && nova_resource_load(resource_id),
                    "Lazy Load und Cache Hit");
    failed |= check(nova_resource_release(resource_id) && nova_resource_release(resource_id),
                    "Referenzzaehlung und Freigabe");
    failed |= check(!nova_resource_register("boot://test/broken", NOVA_RESOURCE_IMAGE,
                    1, resource_data, sizeof(resource_data), 123, 0),
                    "ungueltige Pruefsumme abweisen");
    failed |= check(nova_text_register_font_resource() && nova_icons_initialize() &&
                    nova_branding_initialize(),
                    "Font-, Icon- und Brandingressourcen zentral registrieren");
    failed |= check(nova_resource_find(nova_resource_id("boot://branding/novaos/logo")) != 0,
                    "NovaOS-Logo im Resource Manager");
    bool all_icons=true;
    for(uint32_t icon=0;icon<NOVA_ICON_COUNT;++icon)
        if(!nova_icon_exists((nova_icon_token_t)icon))all_icons=false;
    failed |= check(all_icons,"alle semantischen Icon-Tokens besitzen Ressourcen");
    failed|=check(nova_design_initialize()&&nova_design_diagnostics()->compatible&&
                  nova_design_manifest()->design.major==NOVA_DESIGN_VERSION_MAJOR&&
                  nova_design_components()->button_height_dlu==48&&
                  nova_design_components()->button_radius_dlu==12&&
                  nova_design_typography()->heading_dlu==32&&
                  nova_design_motion()->dialog_ms==180&&
                  nova_design_effects()->glass_blur_dlu==12&&
                  nova_design_validate_resources(),
                  "Versionierte Nova Design Language und gemeinsame DLU-Tokens");
    nova_design_manifest_t incompatible=*nova_design_manifest();
    incompatible.tokens.major=2;incompatible.checksum=0;
    failed|=check(!nova_design_validate_manifest(&incompatible),
                  "Inkompatible Design-Major-Version sicher abweisen");
    nova_oem_design_t invalid_oem={.accent=0x80267cc1u};
    nova_oem_design_t valid_oem={.accent=0xff267cc1u};
    failed|=check(!nova_design_apply_oem(&invalid_oem)&&
                  nova_design_apply_oem(&valid_oem)&&
                  nova_design_accent()==0xff267cc1u,
                  "OEM-Anpassung auf validierte Marke und Akzent begrenzen");

    nova_architecture_initialize();
    for(uint8_t i=0;i<NOVA_ARCH_SUBSYSTEM_COUNT;++i)
        failed|=check(nova_architecture_register((nova_architecture_subsystem_t)i),
                      "Architekturdeskriptor registrieren");
    failed|=check(!nova_architecture_ready(NOVA_ARCH_APPLICATION),
                  "Architekturabhaengigkeit abweisen");
    static const nova_architecture_subsystem_t architecture_order[]={
        NOVA_ARCH_PLATFORM,NOVA_ARCH_RESOURCE,NOVA_ARCH_GRAPHICS,
        NOVA_ARCH_RENDERER,NOVA_ARCH_SCENE,NOVA_ARCH_LAYOUT,NOVA_ARCH_MOTION,
        NOVA_ARCH_CONTROL,NOVA_ARCH_NAVIGATION,NOVA_ARCH_DIALOG,
        NOVA_ARCH_APPLICATION,NOVA_ARCH_DIAGNOSTICS};
    for(uint8_t i=0;i<sizeof(architecture_order)/sizeof(architecture_order[0]);++i)
        failed|=check(nova_architecture_ready(architecture_order[i]),
                      "Architekturabhaengigkeiten deterministisch aufloesen");
    failed|=check(nova_architecture_validate()&&
                  nova_architecture_diagnostics()->complete&&
                  nova_architecture_descriptor(NOVA_ARCH_RENDERER)->platform_neutral,
                  "Modulares Architekturmanifest validieren");

    nova_unicode_initialize();
    const char *unicode = "AΩЖאあ中";
    uint32_t decoded = 0, cp;
    while (*unicode) { nova_unicode_next(&unicode, &cp); ++decoded; }
    failed |= check(decoded == 6, "UTF-8 ueber mehrere Unicode-Bloecke");
    const char invalid_utf8[] = {(char)0xc0, (char)0xaf, 0};
    const char *invalid_cursor = invalid_utf8;
    failed |= check(!nova_unicode_next(&invalid_cursor, &cp) &&
                    cp == NOVA_UNICODE_REPLACEMENT, "ungueltiges UTF-8 ersetzt");

    nova_controls_initialize(0);
    nova_control_t *list = nova_control_create(NOVA_CONTROL_LIST);
    nova_control_t *item = nova_control_create(NOVA_CONTROL_LIST_ITEM);
    failed |= check(list && item && nova_control_set_parent(item, list),
                    "Control-Pool und Parent/Child");
    failed |= check(!nova_control_set_parent(list, item), "Control-Zyklus abweisen");
    failed |= check(nova_control_set_state(item, NOVA_CONTROL_INITIALIZED) &&
                    nova_control_set_state(item, NOVA_CONTROL_VISIBLE),
                    "Control-Lebenszyklus");
    failed |= check(!nova_control_set_state(item, NOVA_CONTROL_CREATED),
                    "ungueltigen Zustandswechsel abweisen");
    failed |= check(nova_control_set_bounds(item, (nova_rect_t){1,1,6,3}) &&
                    nova_control_set_text(item, "NovaOS starten") &&
                    nova_control_set_accessibility(item, 1, "NovaOS starten", false),
                    "Layout, Text und Accessibility");
    failed |= check(nova_control_focus(item) && nova_control_hit_test(2,2) == item,
                    "Fokus und Hit-Test");
    item->action = 42;
    uint32_t action = 0;
    failed |= check(nova_control_invoke(item, &action) && action == 42 &&
                    !nova_control_invoke(item, &action), "Aktion exakt einmal");
    nova_control_release(item);
    failed |= check(nova_control_set_range(item, 0, 1000, 500), "Control-Wertebereich");
    nova_control_render(item, overlay);
    nova_control_t *sw = nova_control_create(NOVA_CONTROL_SWITCH);
    nova_control_t *slider = nova_control_create(NOVA_CONTROL_SLIDER);
    failed |= check(sw && slider &&
                    nova_control_set_state(sw,NOVA_CONTROL_INITIALIZED) &&
                    nova_control_set_state(sw,NOVA_CONTROL_VISIBLE) &&
                    nova_control_set_bounds(sw,(nova_rect_t){10,1,20,10}) &&
                    nova_control_toggle(sw) &&
                    (sw->flags & NOVA_CONTROL_FLAG_CHECKED),
                    "Switch-Zustand und Toggle");
    failed |= check(nova_control_set_state(slider,NOVA_CONTROL_INITIALIZED) &&
                    nova_control_set_state(slider,NOVA_CONTROL_VISIBLE) &&
                    nova_control_set_bounds(slider,(nova_rect_t){10,12,40,12}) &&
                    nova_control_set_range(slider,0,100,50) &&
                    nova_control_set_step(slider,10) &&
                    nova_control_adjust(slider,2) && slider->value==70,
                    "Slider-Schrittweite und begrenzter Wert");
    nova_control_render(sw,overlay);
    nova_control_render(slider,overlay);
    sw->flags |= NOVA_CONTROL_FLAG_LOCKED;
    failed |= check(!nova_control_toggle(sw) &&
                    nova_control_diagnostics()->toggles==1 &&
                    nova_control_diagnostics()->value_changes==1,
                    "gesperrte Controls und Diagnosedaten");
    nova_control_t *field=nova_control_create(NOVA_CONTROL_TEXT_FIELD);
    nova_control_t *password=nova_control_create(NOVA_CONTROL_PASSWORD_FIELD);
    failed |= check(field&&password&&nova_control_set_state(field,NOVA_CONTROL_INITIALIZED)&&
                    nova_control_set_state(field,NOVA_CONTROL_VISIBLE)&&
                    nova_control_set_bounds(field,(nova_rect_t){2,26,70,24})&&
                    nova_text_field_set_placeholder(field,"Suchtext")&&
                    nova_text_field_set_maximum(field,20)&&
                    nova_text_field_insert(field,'N')&&nova_text_field_insert(field,0x00e4)&&
                    field->text_length==3&&field->caret==3&&
                    nova_text_field_move(field,-1,false)&&field->caret==1&&
                    nova_text_field_delete(field)&&field->text_length==1,
                    "UTF-8-Textfeld, Caret und codepointweises Löschen");
    failed |= check(nova_text_field_set_mode(field,NOVA_TEXT_INPUT_NUMERIC)&&
                    !nova_text_field_insert(field,'x')&&nova_text_field_insert(field,'7')&&
                    nova_text_field_select_all(field)&&nova_text_field_backspace(field)&&
                    field->text_length==0,"Textfeldfilter und Auswahl");
    const char invalid_field_text[]={(char)0xc0,(char)0xaf,0};
    failed |= check(!nova_control_set_text(field,invalid_field_text),
                    "Textfeld weist ungültiges UTF-8 ab");
    failed |= check(nova_control_set_state(password,NOVA_CONTROL_INITIALIZED)&&
                    nova_control_set_state(password,NOVA_CONTROL_VISIBLE)&&
                    nova_control_set_bounds(password,(nova_rect_t){2,52,70,24})&&
                    nova_text_field_insert(password,'s')&&password->text_length==1,
                    "Passwortfeld mit geschütztem Festpuffer");
    nova_control_render(field,overlay);nova_control_render(password,overlay);
    nova_text_field_clear(password);
    failed |= check(password->text_length==0&&password->text[0]==0,
                    "Passwortpuffer sicher leeren");
    nova_text_metrics_t text_metrics = nova_text_measure("Ärger mit Öl und Größe", 300);
    failed |= check(text_metrics.width > 0 && !text_metrics.truncated,
                    "UTF-8-Textmessung mit Umlauten");
    nova_text_metrics_t scaled_metrics = nova_text_measure_scaled("NovaOS",300,1500);
    failed |= check(scaled_metrics.width > nova_text_measure("NovaOS",300).width &&
                    scaled_metrics.height == 30,
                    "DLU-skalierte Textmessung");
    nova_text_draw(overlay, 0, 0, 8, "NovaOS", 0xffffffffu,
                   NOVA_TEXT_LEFT, true);
    failed |= check(nova_text_draw_wrapped_scaled(overlay,0,0,40,3,
                    "NovaOS erklärt einen verständlichen Fehlertext",
                    0xffffffffu,1000) >= 2,
                    "mehrzeiliger UTF-8-Dialogtext");
    failed |= check(nova_text_measure("NovaOS", 8).truncated,
                    "Text-Clipping und Ellipsis");
    nova_icon_draw(overlay, NOVA_ICON_SETTINGS, 0, 0, 20, 0xffffffffu);
    failed |= check(nova_icon_diagnostics()->renders == 1 &&
                    nova_icon_exists(NOVA_ICON_POWER), "semantische Icon-Tokens und Cache");

    nova_input_initialize();
    failed |= check(nova_input_device_set(1, NOVA_DEVICE_KEYBOARD, true) &&
                    nova_input_device_set(2, NOVA_DEVICE_MOUSE, true),
                    "Input Device Hot Detection");
    nova_input_event_t key_event = {.timestamp=10,.device_id=1,.type=NOVA_EVENT_KEY_DOWN};
    key_event.key.key = 40;
    nova_input_set_secure(true);
    failed |= check(nova_input_post(key_event), "Event Queue Post");
    nova_input_event_t received;
    failed |= check(nova_input_next(&received) && received.sequence == 1 &&
                    (received.flags & 0x80000000u), "deterministische sichere Event Queue");
    failed |= check(nova_input_shortcut_register(40,0,0,77) &&
                    !nova_input_shortcut_register(40,0,0,88), "Shortcut-Konfliktpruefung");
    uint32_t command = 0;
    failed |= check(nova_input_shortcut_resolve(&received,&command) && command==77,
                    "Shortcut erzeugt Command");
    nova_control_t *item2 = nova_control_create(NOVA_CONTROL_LIST_ITEM);
    nova_control_set_state(item2,NOVA_CONTROL_INITIALIZED);
    nova_control_set_state(item2,NOVA_CONTROL_VISIBLE);
    nova_control_set_bounds(item2,(nova_rect_t){1,5,6,2});
    failed |= check(nova_input_focus_set(item) &&
                    nova_input_neighbor_set(item,NOVA_DIRECTION_DOWN,item2) &&
                    nova_input_focus_navigate(NOVA_DIRECTION_DOWN) &&
                    nova_input_focus_get()==item2, "Directional Focus Navigation");
    failed |= check(nova_input_pointer_capture(9,item2), "Pointer Capture");
    nova_input_event_t pointer={.type=NOVA_EVENT_POINTER_MOVE};
    pointer.pointer.pointer_id=9;pointer.pointer.x=2;pointer.pointer.y=2;
    failed |= check(nova_input_pointer_target(&pointer)==item2, "Capture vor Hit Test");
    nova_input_pointer_release(9);
    failed |= check(nova_input_pointer_target(&pointer)==item, "Hit Test nach Capture Release");
    failed |= check(nova_input_repeat_start(40,100), "Input Repeat starten");
    nova_input_repeat_update(500);
    failed |= check(nova_input_next(&received) && received.key.repeat,
                    "deterministisches Input Repeat");
    failed |= check(nova_input_device_set(2,NOVA_DEVICE_MOUSE,false),
                    "Hot Removal beendet Geraetezustand");

    nova_diag_initialize();
    for(uint32_t i=0;i<NOVA_DIAG_CAPACITY+3u;++i)
        nova_diag_log((nova_diag_event_t){i,NOVA_DIAG_INFO,1,i,0,0});
    failed |= check(nova_diag_statistics()->overwritten==3 &&
                    nova_diag_get(0)->event_id==3, "heapfreier Diagnose-Ringpuffer");
    nova_diag_frame(40000,1000,1200,20000,8000);
    failed |= check(nova_diag_frame_budget()->violations==1 &&
                    nova_diag_quality()->low_end, "Framebudget und adaptive Qualitaet");
    nova_diag_set_quality(NOVA_QUALITY_SAFE,false);
    failed |= check(nova_diag_quality()->quality==NOVA_QUALITY_SAFE &&
                    nova_motion_is_reduced() &&
                    nova_compositor_diagnostics()->fallback_level==3,
                    "Safe-Profil reduziert Motion und Materialien");
    failed |= check(nova_diag_memory_budget()->used_memory <
                    nova_diag_memory_budget()->total_budget,
                    "32-MiB-Low-End-Speicherbudget");
    nova_surface_clear(base, 0xff101010u);
    nova_surface_clear(overlay, 0x00000000u);
    nova_surface_rect(overlay, (nova_rect_t){2, 2, 3, 3}, 0x80267cc1u);
    nova_layer_t lower = { 1, base, {0,0,8,8}, {0,0,8,8}, 0, 255,
                           NOVA_MATERIAL_NONE, true, false, false, true };
    nova_layer_t modal = { 2, overlay, {0,0,8,8}, {0,0,8,8}, 10, 255,
                           NOVA_MATERIAL_ACRYLIC, true, true, true, true };
    failed |= check(nova_compositor_begin_frame(), "Frame beginnen");
    failed |= check(nova_compositor_submit_layer(&modal), "Modal Layer einreichen");
    failed |= check(nova_compositor_submit_layer(&lower), "stabile Z-Sortierung");
    failed |= check(!nova_compositor_input_allowed(1) && nova_compositor_input_allowed(2),
                    "modale Eingabesperre");
    failed |= check(nova_compositor_compose(), "Damage compositing im Offscreen-Buffer");
    nova_state_set_phase(NOVA_STATE_PHASE_RENDER);
    failed|=check(!nova_compositor_present(),"Present ausserhalb der Present-Phase sperren");
    nova_state_set_phase(NOVA_STATE_PHASE_PRESENT);
    failed|=check(nova_compositor_present()&&nova_graphics_diagnostics()->presents==1&&
                  graphics_frontbuffer[2+2*10]!=0,
                  "Pitch-sicheres Present am Frameende");
    nova_compositor_set_fallback(3);
    failed |= check(nova_compositor_diagnostics()->fallback_level == 3,
                    "Material-Fallback");
    failed |= check(nova_compositor_diagnostics()->frames == 1,
                    "Compositor-Diagnose");
    nova_compositor_set_fallback(0);
    nova_surface_clear(base, 0xff000000u);
    nova_surface_clear(overlay, 0x00000000u);
    nova_surface_rect(overlay, (nova_rect_t){0,0,1,1}, 0x80ff0000u);
    modal.material = NOVA_MATERIAL_NONE;
    modal.modal = false;
    failed |= check(nova_compositor_begin_frame() &&
                    nova_compositor_submit_layer(&lower) &&
                    nova_compositor_submit_layer(&modal) &&
                    nova_compositor_compose(), "Alpha-Testframe");
    nova_state_set_phase(NOVA_STATE_PHASE_PRESENT);
    failed|=check(nova_compositor_present(),"Alpha-Testframe praesentieren");
    failed |= check(graphics_frontbuffer[0] == 0xff800000u,
                    "kanalgetreues Alpha-Compositing");

    if (!failed) puts("BOOT_UI_RUNTIME_OK");
    return failed ? 1 : 0;
}
