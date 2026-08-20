#include "uefi_min.h"
#include "pointer.h"
#include "../bootmenu/motion.h"
#include "../bootmenu/input.h"
#include "../bootmenu/diagnostics.h"
#include "../bootmenu/ui.h"
#include "../bootmenu/theme.h"
#include "../bootmenu/navigation.h"
#include "../bootmenu/dialog.h"
#include "../bootmenu/runtime.h"
#include "../bootmenu/memory.h"
#include "../bootmenu/recovery.h"
#include "../bootmenu/configuration.h"
#include "../bootmenu/graphics.h"
#include "../bootmenu/framebuffer_backend.h"
#include "../bootmenu/present_scheduler.h"
#include "../bootmenu/render_quality.h"
#include "../bootmenu/test_architecture.h"
#include "../bootmenu/layout.h"

static uint64_t startup_tsc_hz=1000000000ull;
static uint64_t startup_tsc_origin;
static uint16_t uefi_control_event_order[2],uefi_control_event_count;
static uint32_t uefi_shortcut_command,uefi_shortcut_calls;
static bool uefi_shortcut_handler(uint32_t command,void *context)
{if(context!=(void *)&uefi_shortcut_calls)return false;
 uefi_shortcut_command=command;++uefi_shortcut_calls;return true;}
static bool uefi_control_event_handler(nova_control_t *control,
    nova_control_event_t *event,void *context)
{bool handled=*(bool *)context;
 if(uefi_control_event_count<2){
    uefi_control_event_order[uefi_control_event_count++]=control->id;
 }
 return handled&&event->type==NOVA_CONTROL_EVENT_CLICK;
}
typedef struct {uint32_t actual;} uefi_test_probe_t;
static nova_test_status_t uefi_test_execute(void *context,uint32_t *actual,uint32_t *error)
{uefi_test_probe_t *probe=(uefi_test_probe_t *)context;*actual=probe->actual;*error=0;
 return NOVA_TEST_PASSED;}
static uint64_t uefi_test_state(void){return 0x4e4f564154455354ull;}

static void startup_cpuid(uint32_t leaf,uint32_t *a,uint32_t *b,uint32_t *c,uint32_t *d)
{__asm__ volatile("cpuid":"=a"(*a),"=b"(*b),"=c"(*c),"=d"(*d):"a"(leaf),"c"(0));}
static uint64_t startup_rdtsc(void)
{uint32_t low,high;__asm__ volatile("rdtsc":"=a"(low),"=d"(high));
 return ((uint64_t)high<<32)|low;}
static void startup_clock_initialize(void)
{
    uint32_t a,b,c,d;startup_cpuid(0,&a,&b,&c,&d);uint32_t maximum=a;
    if(maximum>=0x15){startup_cpuid(0x15,&a,&b,&c,&d);
        if(a&&b&&c)startup_tsc_hz=(uint64_t)c*b/a;}
    if(startup_tsc_hz==1000000000ull&&maximum>=0x16){startup_cpuid(0x16,&a,&b,&c,&d);
        if(a)startup_tsc_hz=(uint64_t)a*1000000ull;}
    startup_tsc_origin=startup_rdtsc();
}
static uint64_t startup_time_us(void)
{return (startup_rdtsc()-startup_tsc_origin)/(startup_tsc_hz/1000000ull);}
#include "../bootmenu/gop_backend.h"
#include "../bootmenu/resolution.h"
#include "../bootmenu/software_renderer.h"
#include "../bootmenu/resources.h"
#include "firmware.h"

EFI_STATUS grafik_init(EFI_SYSTEM_TABLE *system_table);
bool uefi_power_initialize(EFI_SYSTEM_TABLE *system_table);
bool uefi_power_execute(bool shutdown);
EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table);

/*
 * Keep one absolute image reference so the PE/COFF linker emits a .reloc
 * directory.  UEFI firmware is free to load an application away from its
 * preferred image base and rejects non-relocatable images in that case.
 */
void *uefi_relocation_anchor = (void *)&efi_main;
static UINTN pending_power = (UINTN)-1;
static EFI_SYSTEM_TABLE *runtime_system_table;
static nova_dialog_motion_t dialog_visual;

static void animate_dialog_motion(bool entering,UINTN selection)
{
    if(!runtime_system_table)return;
    bootmenu_set_dialog_effects(false);
    if(entering)dialog_visual=(nova_dialog_motion_t){0};
    bool scheduled=entering?nova_dialog_enter(&dialog_visual):
                            nova_dialog_exit(&dialog_visual);
    uint32_t duration=nova_motion_is_reduced()?150u:180u;
    uint64_t start=nova_motion_diagnostics()->current_ms;
    nova_debug_string(entering?"UEFI:DIALOG-ENTER-START\n":"UEFI:DIALOG-EXIT-START\n");
    if(scheduled){
        uint32_t step=nova_low_end_profile_status()->enabled?duration:duration/2u;
        if(!step)step=1;
        for(uint32_t elapsed=0;elapsed<=duration;elapsed+=step){
            nova_motion_update(start+elapsed);
            bootmenu_set_dialog_motion((uint8_t)dialog_visual.opacity,
                                       (uint16_t)dialog_visual.scale);
            bootmenu_draw(selection,255);
            runtime_system_table->BootServices->Stall(
                nova_low_end_profile_status()->enabled?1000:30000);
        }
        nova_motion_update(start+duration+1u);
    }
    if(entering){dialog_visual.opacity=255;dialog_visual.scale=1000;
        dialog_visual.focused=true;bootmenu_set_dialog_effects(true);}
    else{dialog_visual.opacity=0;dialog_visual.scale=nova_motion_is_reduced()?1000:950;}
    bootmenu_set_dialog_motion((uint8_t)dialog_visual.opacity,(uint16_t)dialog_visual.scale);
    bootmenu_draw(selection,255);
    nova_debug_string(entering?"UEFI:DIALOG-ENTER-COMPLETE\n":
                               "UEFI:DIALOG-EXIT-COMPLETE\n");
}

static void animate_navigation(nova_navigation_transition_t type, bool entering,
                               UINTN selection)
{
    uint64_t start=nova_motion_diagnostics()->current_ms;
    if (!runtime_system_table || !nova_navigation_visual_begin(type,entering,start)) {
        bootmenu_set_transition(0,255,false);
        return;
    }
    const nova_navigation_visual_t *visual=nova_navigation_visual();
    if (visual->reduced_motion) nova_debug_string("UEFI:NAV-REDUCED-FADE\n");
    else if (type == NOVA_NAV_RECOVERY) nova_debug_string("UEFI:NAV-RECOVERY-FADE\n");
    else if (type == NOVA_NAV_REPLACE) nova_debug_string("UEFI:NAV-CROSS-FADE\n");
    else nova_debug_string(type == NOVA_NAV_POP ? "UEFI:NAV-SLIDE-RIGHT\n" :
                                                       "UEFI:NAV-SLIDE-LEFT\n");
    uint32_t duration=visual->duration_ms;
    /* Software compositing uses deterministic keyframes to stay inside the
       boot-time budget even at Full HD. */
    uint32_t step=nova_low_end_profile_status()->enabled?duration:duration/2u;
    if (!step) step=1;
    for (uint32_t elapsed=0;elapsed<=duration;elapsed+=step) {
        nova_motion_update(start+elapsed);
        visual=nova_navigation_visual();
        bootmenu_set_transition(visual->offset_dlu,(uint8_t)visual->opacity,true);
        bootmenu_draw(entering ? (UINTN)-1 : selection,255);
        runtime_system_table->BootServices->Stall(
            nova_low_end_profile_status()->enabled?1000:30000);
    }
    nova_motion_update(start+duration+1);
    nova_navigation_visual_complete();
    bootmenu_set_transition(0,255,false);
    nova_debug_string(entering ? "UEFI:NAV-ENTER-COMPLETE\n" :
                                 "UEFI:NAV-EXIT-COMPLETE\n");
}

static void show_notice(nova_dialog_type_t type, const char *title, const char *message)
{
    const nova_navigation_entry_t *current = nova_navigation_current();
    nova_dialog_t *dialog = type==NOVA_DIALOG_WARNING?
        nova_warning_dialog_show(NOVA_WARNING_LOW,NOVA_WARNING_GENERAL,title,message,
            "Bitte prüfen Sie die verfügbare sichere Alternative.",0,true,
            current?current->focus_id:0):
        nova_dialog_open(type,title,message,true,true,current?current->focus_id:0);
    if (dialog) {
        if(type!=NOVA_DIALOG_WARNING)
            nova_dialog_add_button(dialog, "OK", NOVA_DIALOG_RESULT_OK, false);
        else nova_debug_string("UEFI:WARNING-DIALOG-OPEN\n");
        nova_debug_string("UEFI:DIALOG-OPEN\n");
        animate_dialog_motion(true,current?current->selection:0);
    }
}

static bool close_dialog_animated(bool cancel,UINTN selection,
                                  nova_dialog_result_t *result)
{
    nova_dialog_t *dialog=nova_dialog_active();
    if(!dialog||!result)return false;
    if(cancel){
        if(!dialog->cancelable)return false;
    }else if(dialog->focus==dialog->destructive_button&&!dialog->destructive_armed){
        return nova_dialog_activate(result);
    }
    animate_dialog_motion(false,selection);
    bool closed=cancel?nova_dialog_cancel(result):nova_dialog_activate(result);
    bootmenu_set_dialog_motion(255,1000);
    return closed;
}

static bool navigate_to(nova_bootmenu_view_t view, UINTN *selection,
                        nova_navigation_transition_t transition)
{
    if(!nova_modal_dialog_navigation_allowed())return false;
    if (!selection || !nova_navigation_update((uint16_t)*selection,
                                               (uint16_t)*selection, 0, 0)) return false;
    animate_navigation(transition,false,*selection);
    if (!nova_navigation_push((nova_navigation_entry_t){(uint16_t)view,0,0,0,0},
                              transition)) return false;
    nova_navigation_transition_complete();
    bootmenu_set_view(view);
    *selection = 0;
    animate_navigation(transition,true,*selection);
    bootmenu_draw(*selection,255);
    return true;
}

static bool navigate_back(UINTN *selection)
{
    if(!nova_modal_dialog_navigation_allowed())return false;
    nova_navigation_entry_t restored;
    if (!selection) return false;
    animate_navigation(NOVA_NAV_POP,false,*selection);
    if (!nova_navigation_back(&restored)) return false;
    nova_navigation_transition_complete();
    bootmenu_set_view((nova_bootmenu_view_t)restored.page);
    *selection = restored.selection;
    animate_navigation(NOVA_NAV_POP,true,*selection);
    bootmenu_draw(*selection,255);
    nova_debug_string("UEFI:NAV-RESTORED\n");
    return true;
}

static bool navigate_root(UINTN *selection)
{
    if(!nova_modal_dialog_navigation_allowed())return false;
    if(!selection||bootmenu_view()==NOVA_VIEW_MAIN)return false;
    animate_navigation(NOVA_NAV_REPLACE,false,*selection);
    if(!nova_navigation_reset())return false;
    nova_navigation_transition_complete();
    bootmenu_set_view(NOVA_VIEW_MAIN);
    *selection=0;
    animate_navigation(NOVA_NAV_REPLACE,true,*selection);
    bootmenu_set_status("");
    bootmenu_draw(*selection,255);
    nova_debug_string("UEFI:BREADCRUMB-ROOT\n");
    return true;
}

static void handle_dialog_result(nova_dialog_result_t result)
{
    if (result == NOVA_DIALOG_RESULT_YES && pending_power != (UINTN)-1) {
        UINTN action = pending_power;
        pending_power = (UINTN)-1;
        nova_debug_string("UEFI:DIALOG-RESULT-YES\n");
        if(action==2){
            if(!uefi_firmware_request_setup())
                show_notice(NOVA_DIALOG_ERROR,"Firmware-Setup fehlgeschlagen",
                    "Die Firmware konnte den sicheren Setup-Neustart nicht vorbereiten.");
        }else if (!uefi_power_execute(action == 0))
            show_notice(NOVA_DIALOG_ERROR, "Energieaktion fehlgeschlagen",
                        "Die Firmware konnte die angeforderte Aktion nicht ausführen.");
    } else if (result == NOVA_DIALOG_RESULT_CANCEL) {
        pending_power = (UINTN)-1;
        nova_debug_string("UEFI:DIALOG-RESULT-CANCEL\n");
    } else {
        pending_power = (UINTN)-1;
        nova_debug_string("UEFI:DIALOG-RESULT-OK\n");
    }
}

static void run_diagnostic_progress(UINTN selection)
{
    nova_dialog_t *dialog = nova_dialog_open(NOVA_DIALOG_PROGRESS,
        "Boot-Diagnose", "NovaOS prüft die vorhandenen Diagnosedaten.",
        true, false, (uint16_t)selection);
    if (!dialog || !runtime_system_table) return;
    animate_dialog_motion(true,selection);
    static const uint16_t values[5] = {0,250,500,750,1000};
    static const char *const states[5] = {
        "Initialisierung", "Firmwaredaten", "Grafikausgabe",
        "Eingabegeräte", "Prüfung abgeschlossen"
    };
    nova_debug_string("UEFI:PROGRESS-OPEN\n");
    for (uint8_t i=0;i<5;++i) {
        nova_dialog_progress_update(dialog,values[i],i==0,states[i]);
        bootmenu_draw(selection,255);
        if(i==0)nova_debug_string("UEFI:ACTIVITY-FRAME-READY\n");
        runtime_system_table->BootServices->Stall(i==0?800000:150000);
    }
    dialog->cancelable=true;
    nova_dialog_add_button(dialog,"Schließen",NOVA_DIALOG_RESULT_OK,false);
    bootmenu_draw(selection,255);
    nova_debug_string("UEFI:PROGRESS-COMPLETE\n");
}

static EFI_INPUT_KEY read_key(EFI_SYSTEM_TABLE *system_table)
{
    EFI_INPUT_KEY key = {0, 0};
    if (!system_table->ConIn ||
        EFI_ERROR(system_table->ConIn->ReadKeyStroke(system_table->ConIn, &key))) {
        key.ScanCode = 0;
        key.UnicodeChar = 0;
        return key;
    }
    nova_input_event_t posted = {0};
    posted.device_id = 1;
    posted.type = key.UnicodeChar ? NOVA_EVENT_CHARACTER : NOVA_EVENT_KEY_DOWN;
    posted.key.key = key.ScanCode;
    posted.key.unicode = key.UnicodeChar;
    nova_input_post(posted);
    nova_input_event_t event;
    while(nova_input_next(&event)){
        if(event.type==NOVA_EVENT_KEY_DOWN||event.type==NOVA_EVENT_CHARACTER){
            key.ScanCode=(uint16_t)event.key.key;
            key.UnicodeChar=(CHAR16)event.key.unicode;break;
        }
    }
    return key;
}

static void boot_selected(UINTN selection)
{
      if (selection == 0) {
          nova_runtime_shutdown();nova_debug_string("UEFI:RUNTIME-SHUTDOWN\n");
          nova_runtime_destroy();nova_debug_string("UEFI:RUNTIME-DESTROYED\n");
          nova_resource_manager_shutdown();nova_debug_string("UEFI:RESOURCE-MANAGER-SHUTDOWN\n");
          nova_present_shutdown();nova_debug_string("UEFI:PRESENT-SCHEDULER-SHUTDOWN\n");
          nova_framebuffer_shutdown();nova_debug_string("UEFI:FRAMEBUFFER-BACKEND-SHUTDOWN\n");
          nova_gop_shutdown();nova_debug_string("UEFI:GOP-SHUTDOWN\n");
          nova_graphics_shutdown();nova_debug_string("UEFI:GAL-SHUTDOWN\n");
          nova_debug_string("UEFI:START\n");
    }
    else if (selection == 1) nova_debug_string("UEFI:INSTALL-UNAVAILABLE\n");
    else if (selection == 2) nova_debug_string("UEFI:SETTINGS\n");
    else if (selection == 3) nova_debug_string("UEFI:DIAGNOSTICS\n");
    else if (selection == 4) nova_debug_string("UEFI:RECOVERY\n");
    else nova_debug_string("UEFI:POWEROFF\n");
}

static void console_write_ascii(EFI_SYSTEM_TABLE *system_table,const char *text)
{
    if(!system_table||!system_table->ConOut||!system_table->ConOut->OutputString||!text)return;
    CHAR16 buffer[160];UINTN index=0;
    while(text[index]&&index+1<sizeof(buffer)/sizeof(buffer[0])){
        buffer[index]=(CHAR16)(uint8_t)text[index];++index;
    }
    buffer[index]=0;system_table->ConOut->OutputString(system_table->ConOut,buffer);
}

static EFI_STATUS text_fallback(EFI_SYSTEM_TABLE *system_table,const char *reason)
{
    nova_debug_string("UEFI:TEXT-FALLBACK\n");
    console_write_ascii(system_table,"\r\nNovaOS Boot Manager\r\n\r\n");
    console_write_ascii(system_table,"Die grafische Oberflaeche ist nicht verfuegbar.\r\n");
    console_write_ascii(system_table,reason);
    console_write_ascii(system_table,"\r\nDer Start wird im sicheren Textmodus fortgesetzt.\r\n");
    console_write_ascii(system_table,"Enter: NovaOS jetzt starten\r\n");
    console_write_ascii(system_table,"R: Recovery-Status  M: Speicherdiagnose-Status\r\n");
    console_write_ascii(system_table,"Automatischer Start in 5 Sekunden.\r\n");
    for(uint32_t tick=0;tick<50;++tick){
        EFI_INPUT_KEY key={0};
        if(system_table->ConIn&&system_table->ConIn->ReadKeyStroke&&
           !EFI_ERROR(system_table->ConIn->ReadKeyStroke(system_table->ConIn,&key))){
            if(key.UnicodeChar==13){boot_selected(0);nova_debug_string("UEFI:TEXT-CONTINUE\n");
                return EFI_SUCCESS;}
            if(key.UnicodeChar=='r'||key.UnicodeChar=='R'){
                nova_debug_string("UEFI:TEXT-RECOVERY-STATUS\n");
                console_write_ascii(system_table,"Recovery-Backend nicht verfuegbar; Start bleibt sicher.\r\n");
            }
            if(key.UnicodeChar=='m'||key.UnicodeChar=='M'){
                nova_debug_string("UEFI:TEXT-MEMORY-STATUS\n");
                console_write_ascii(system_table,"Speicherdiagnose-Backend nicht verfuegbar; Start bleibt sicher.\r\n");
            }
        }
        if(system_table->BootServices&&system_table->BootServices->Stall)
            system_table->BootServices->Stall(100000);
    }
    boot_selected(0);nova_debug_string("UEFI:TEXT-CONTINUE\n");return EFI_SUCCESS;
}

static bool handle_action(UINTN *selection_pointer)
{
    UINTN selection = *selection_pointer;
    nova_bootmenu_view_t view = bootmenu_view();
    bootmenu_set_status("");
    if (view == NOVA_VIEW_MAIN) {
        if (selection == 0) { boot_selected(0); return true; }
        if (selection == 1) {
            nova_debug_string("UEFI:INSTALL-UNAVAILABLE\n");
            show_notice(NOVA_DIALOG_INFORMATION, "Installation nicht verfügbar",
                        "Dieses Abbild enthält noch kein geprüftes Installationsmodul.");
        } else if (selection == 2) {
            nova_debug_string("UEFI:SETTINGS\n");
            navigate_to(NOVA_VIEW_SETTINGS, selection_pointer, NOVA_NAV_PUSH);
        } else if (selection == 3) {
            nova_debug_string("UEFI:DIAGNOSTICS-VIEW\n");
            navigate_to(NOVA_VIEW_DIAGNOSTICS, selection_pointer, NOVA_NAV_PUSH);
        } else if (selection == 4) {
            nova_debug_string("UEFI:RECOVERY-VIEW\n");
            navigate_to(NOVA_VIEW_RECOVERY, selection_pointer, NOVA_NAV_RECOVERY);
        } else {
            nova_debug_string("UEFI:POWER-VIEW\n");
            pending_power = (UINTN)-1;
            navigate_to(NOVA_VIEW_POWER, selection_pointer, NOVA_NAV_PUSH);
        }
    } else if (view == NOVA_VIEW_SETTINGS) {
        if (selection == 5) navigate_back(selection_pointer);
        else if (selection == 0) {
            bootmenu_theme_menu_open();
        } else if (selection == 1) {
            bootmenu_settings_toggle_reduced_motion();
            nova_debug_string("UEFI:SETTINGS-SWITCH-UPDATED\n");
            nova_debug_string(nova_theme_reduced_motion() ?
                              "UEFI:REDUCED-MOTION-ON\n" : "UEFI:REDUCED-MOTION-OFF\n");
            bootmenu_set_status(nova_theme_reduced_motion() ?
                "Barrierefreiheit: reduzierte Bewegung aktiv." :
                "Barrierefreiheit: normale Bewegung aktiv.");
        } else if (selection == 2) {
            bootmenu_settings_toggle_tooltips();
        } else if (selection == 3) {
            bootmenu_settings_adjust_tooltip_delay(1);
        } else if (selection == 4) {
            uefi_firmware_refresh();
            const nova_uefi_firmware_status_t *firmware=uefi_firmware_status();
            bootmenu_set_firmware_info(firmware->vendor,firmware->revision,
                firmware->secure_boot_known,firmware->secure_boot,
                firmware->setup_mode_known,firmware->setup_mode,
                firmware->firmware_setup_supported);
            nova_debug_string("UEFI:FIRMWARE-VIEW\n");
            navigate_to(NOVA_VIEW_FIRMWARE,selection_pointer,NOVA_NAV_PUSH);
        }
    } else if(view==NOVA_VIEW_FIRMWARE){
        const nova_uefi_firmware_status_t *firmware=uefi_firmware_status();
        if(selection==5)navigate_back(selection_pointer);
        else if(selection==4&&!firmware->firmware_setup_supported)
            show_notice(NOVA_DIALOG_WARNING,"Firmware-Setup nicht verfügbar",
                "Diese Firmware unterstützt keinen sicheren direkten Neustart in das Setup.");
        else if(selection==4){
            pending_power=2;
            nova_debug_string("UEFI:FIRMWARE-SETUP-CONFIRM\n");
            nova_dialog_t *dialog=nova_confirmation_dialog_show(NOVA_CONFIRM_CRITICAL,
                "Firmware-Setup öffnen",
                "Der Computer wird neu gestartet und öffnet danach die Firmware-Einstellungen.",
                "Neu starten",NOVA_DIALOG_RESULT_YES,(uint16_t)selection);
            if(dialog){
                if(dialog->focus==0&&dialog->button_results[0]==NOVA_DIALOG_RESULT_CANCEL)
                    nova_debug_string("UEFI:CONFIRMATION-SAFE-DEFAULT\n");
                animate_dialog_motion(true,selection);
            }
        }else bootmenu_set_status("Firmwareinformationen werden ausschließlich lesend angezeigt.");
    } else if(view==NOVA_VIEW_HELP){
        if(selection==5)navigate_back(selection_pointer);
        else if(selection==0)bootmenu_help_search_begin();
        else{
            const char *title=bootmenu_help_result_title((uint16_t)selection);
            const char *details=bootmenu_help_result_details((uint16_t)selection);
            if(title&&details){nova_debug_string("UEFI:HELP-RESULT-OPEN\n");
                show_notice(NOVA_DIALOG_INFORMATION,title,details);}
            else bootmenu_set_status("Für diese Suche wurde kein Hilfethema gefunden.");
        }
    } else if (view == NOVA_VIEW_DIAGNOSTICS) {
        if (selection == 5) navigate_back(selection_pointer);
        else if (selection == 0) (void)bootmenu_diagnostics_refresh();
        else if (selection == 3) run_diagnostic_progress(selection);
        else if(selection==4)(void)bootmenu_diagnostics_export();
        else if(selection==1)bootmenu_set_status("Hardware: CPU, Speicher, Grafik und Eingabe wurden lesend erfasst.");
        else bootmenu_set_status("Boot: UEFI, Bootphasen und Zeitbudgets wurden lesend erfasst.");
    } else if (view == NOVA_VIEW_RECOVERY) {
        if (selection == 5) navigate_back(selection_pointer);
        else show_notice(NOVA_DIALOG_WARNING, "Recovery nicht verfügbar",
                         "Für diese Aktion fehlt ein geprüftes und sicheres Schreib-Backend.");
    } else if (view == NOVA_VIEW_POWER) {
        if (selection == 5) {
            pending_power = (UINTN)-1;
            navigate_back(selection_pointer);
        } else if (selection == 0 || selection == 1) {
            pending_power = selection;
            nova_debug_string("UEFI:POWER-CONFIRM\n");
            nova_dialog_t *dialog = nova_confirmation_dialog_show(NOVA_CONFIRM_CRITICAL,
                selection == 0 ? "NovaOS herunterfahren" : "NovaOS neu starten",
                selection == 0 ? "Der Computer wird sicher ausgeschaltet." :
                                 "Der Computer wird sicher neu gestartet.",
                selection == 0 ? "Ausschalten" : "Neustarten",
                NOVA_DIALOG_RESULT_YES,(uint16_t)selection);
            if (dialog) {
                if(dialog->focus==0&&dialog->button_results[0]==NOVA_DIALOG_RESULT_CANCEL)
                    nova_debug_string("UEFI:CONFIRMATION-SAFE-DEFAULT\n");
                animate_dialog_motion(true,selection);
            }
        } else {
            nova_debug_string("UEFI:POWER-TARGET-UNAVAILABLE\n");
            show_notice(NOVA_DIALOG_WARNING, "Neustartziel nicht verfügbar",
                        "Die Firmware stellt dieses sichere Neustartziel nicht bereit.");
        }
    } else {
        navigate_back(selection_pointer);
    }
    return false;
}

static void handle_context_action(UINTN selection)
{
    uint8_t action;
    if(!bootmenu_context_activate(&action))return;
    if(action&0x80u){
        uint8_t theme_action=(uint8_t)(action&0x7fu);
        if(theme_action==3){(void)bootmenu_quality_menu_open();return;}
        nova_theme_id_t theme=(nova_theme_id_t)theme_action;
        if(bootmenu_configuration_select_theme((uint8_t)theme)){
            if(theme==NOVA_THEME_LIGHT){nova_debug_string("UEFI:THEME-LIGHT\n");
                bootmenu_set_status("Darstellung: Light Theme aktiv.");}
            else if(theme==NOVA_THEME_HIGH_CONTRAST){
                nova_debug_string("UEFI:THEME-HIGH-CONTRAST\n");
                bootmenu_set_status("Darstellung: Hoher Kontrast und reduzierte Effekte aktiv.");}
            else{nova_debug_string("UEFI:THEME-DARK\n");
                bootmenu_set_status("Darstellung: Dark Theme aktiv.");}
            nova_debug_string("UEFI:MENU-BUTTON-SELECTION\n");
        }
        return;
    }
    if(action&0x40u){
        uint8_t index=(uint8_t)(action&0x3fu);
        uint8_t quality=index?index-1:NOVA_QUALITY_AUTO;
        if(bootmenu_configuration_select_quality(quality)){
            static const char *const labels[6]={"Automatisch","Ultra","High",
                "Balanced","Low","Safe"};
            bootmenu_set_status(labels[index]);
            nova_debug_string("UEFI:QUALITY-SELECTION\n");
        }
        return;
    }
    if(action==0){
        nova_debug_string("UEFI:CONTEXT-DETAILS\n");
        show_notice(NOVA_DIALOG_INFORMATION,"Eintragsdetails",
            selection==0?
            "NovaOS\nVersion: Entwicklungsabbild\nKernel: bevorzugt NKI, ELF-Fallback\n"
            "Architektur: x86_64 UEFI\nDatenträger: lokale EFI-Systempartition\n"
            "Bootmodus: geprüft\nStandardstart: Ja\nStatus: startbereit\n\n"
            "Der Eintrag wird ausschließlich lesend dargestellt.":
            "Für diesen Eintrag stehen derzeit keine weiteren Details bereit.");
    }else if(action==1){
        nova_debug_string("UEFI:CONTEXT-ADVANCED\n");
        show_notice(NOVA_DIALOG_INFORMATION,"Erweiterte Optionen",
            "Für diesen Eintrag sind keine zusätzlichen sicheren Startparameter verfügbar.");
    }
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
    (void)image_handle;
    startup_clock_initialize();
    uint64_t entry_time_us=startup_time_us();
    runtime_system_table = system_table;
    nova_debug_string("UEFI:NOVA-ENTRY\n");
    if (!system_table || !system_table->BootServices) return 1;
    nova_runtime_create();
    if(!nova_runtime_begin_initialization())return 1;
    nova_debug_string("UEFI:STATE-MODEL-READY\n");
    nova_memory_initialize();
    uint64_t memory_area_sum=0;
    if(!nova_memory_budget_configure(NOVA_MEMORY_PROFILE_MINIMAL)||
       nova_memory_budget_status()->total_budget!=32ull*1024u*1024u||
       !nova_memory_budget_configure(NOVA_MEMORY_PROFILE_STANDARD))return 1;
    for(uint8_t area=0;area<NOVA_MEMORY_AREA_COUNT;++area)
        memory_area_sum+=nova_memory_budget_status()->area_budget[area];
    if(memory_area_sum!=nova_memory_budget_status()->total_budget||
       !nova_memory_budget_report(NOVA_MEMORY_AREA_GLYPH_CACHE,4ull*1024u*1024u)||
       nova_memory_budget_report(NOVA_MEMORY_AREA_GLYPH_CACHE,4ull*1024u*1024u+1))return 1;
    uint8_t secret_test[4]={1,2,3,4};nova_memory_secure_zero(secret_test,sizeof(secret_test));
    if(secret_test[0]||secret_test[1]||secret_test[2]||secret_test[3]||
       !nova_memory_budget_reset())return 1;
    nova_debug_string("UEFI:MEMORY-BUDGET-READY\n");
    if(!nova_runtime_subsystem_ready(NOVA_RUNTIME_MEMORY)||
       !nova_runtime_subsystem_ready(NOVA_RUNTIME_PLATFORM))return 1;
    uint64_t graphics_start_us=startup_time_us();
    if (EFI_ERROR(grafik_init(system_table))) {
        nova_debug_string("UEFI:GOP-UNAVAILABLE\n");
        nova_runtime_enter_recovery();
        return text_fallback(system_table,"Grafikinitialisierung fehlgeschlagen.");
    }
    if(!nova_runtime_subsystem_ready(NOVA_RUNTIME_GRAPHICS))return 1;
    uint64_t graphics_end_us=startup_time_us();
    nova_diag_initialize();nova_boot_budget_set_clock(startup_time_us);
    nova_diag_event_descriptor_t diag_probe={
        .event={startup_time_us(),NOVA_DIAG_INFO,NOVA_DIAG_AREA_BOOT,0x00010001u,0,0},
        .module="BootManager",.source="UEFI",.message="Diagnostics initialized",
        .phase=NOVA_DIAG_PHASE_UI};
    static uint8_t diag_export_probe[512];uint32_t diag_export_size=0;
    if(!nova_diag_log_extended(&diag_probe)||
       nova_diag_framework_status()->registered_modules!=NOVA_DIAG_AREA_COUNT||
       nova_diag_framework_status()->live_enabled||
       nova_diag_export(NOVA_DIAG_EXPORT_JSON,false,diag_export_probe,
           sizeof(diag_export_probe),&diag_export_size)||
       !nova_diag_export(NOVA_DIAG_EXPORT_NDF,true,diag_export_probe,
           sizeof(diag_export_probe),&diag_export_size)||!diag_export_size)return 1;
    nova_debug_string("UEFI:DIAGNOSTICS-FRAMEWORK-READY\n");
    nova_diag_reset();
    if(!nova_boot_perf_record_startup(30000,10000,20000,50000,100000))return 1;
    nova_diag_frame_extended(16000,400,400,1800,1800,1400,5500,1800,1400);
    if(nova_frame_budget_exceeded()||
       nova_boot_perf_metrics()->frame_class!=NOVA_FRAME_CLASS_A)return 1;
    nova_diag_frame_extended(40000,600,600,2500,2500,1700,7000,2500,1700);
    if(!nova_frame_budget_exceeded()||
       nova_boot_perf_metrics()->frame_class!=NOVA_FRAME_CLASS_E||
       nova_boot_perf_metrics()->hard_violations!=1)return 1;
    nova_debug_string("UEFI:FRAME-BUDGET-READY\n");
    nova_diag_initialize();nova_recovery_initialize();
    (void)nova_boot_budget_begin_at(NOVA_BOOT_PHASE_STAGE2,entry_time_us);
    (void)nova_boot_budget_end_at(NOVA_BOOT_PHASE_STAGE2,graphics_start_us);
    (void)nova_boot_budget_begin_at(NOVA_BOOT_PHASE_HARDWARE,graphics_start_us);
    (void)nova_boot_budget_end_at(NOVA_BOOT_PHASE_HARDWARE,graphics_end_us);
    (void)nova_boot_budget_begin_at(NOVA_BOOT_PHASE_GRAPHICS,graphics_start_us);
    (void)nova_boot_budget_end_at(NOVA_BOOT_PHASE_GRAPHICS,graphics_end_us);
    if(!nova_runtime_subsystem_ready(NOVA_RUNTIME_DIAGNOSTICS))return 1;
    nova_configuration_initialize();
    if(nova_configuration_effective()->scale_milli){
        if(!nova_resolution_set_scale(nova_configuration_effective()->scale_milli))return 1;
    }else if(!nova_resolution_set_automatic())return 1;
    nova_debug_string("UEFI:SCALING-READY\n");
    nova_rect_t alignment_container={0,0,200,100};
    nova_rect_t alignment_measured={0,8,40,20};
    nova_rect_t alignment_result={0};
    nova_rect_t spacing_content={0,0,100,20};
    nova_alignment_spec_t alignment_spec={
        .horizontal=NOVA_ALIGN_CENTER,.vertical=NOVA_ALIGN_CENTER_VERTICAL,
        .margin={10,10,10,10},.padding={10,10,10,10},
        .spacing_dlu=0,.baseline_dlu=0,
        .distribution=NOVA_DISTRIBUTE_NONE,.accessibility=false};
    int32_t alignment_positions[3]={0};
    if(!nova_layout_apply_alignment(&alignment_container,&alignment_measured,
           &alignment_spec,1000,&alignment_result)||
       alignment_result.x!=80||alignment_result.y!=40||
       alignment_result.width!=40||alignment_result.height!=20||
       !nova_layout_apply_spacing(&spacing_content,3,10,0,
           NOVA_DISTRIBUTE_SPACE_BETWEEN,true,1000,alignment_positions,3)||
       alignment_positions[0]!=0||alignment_positions[1]!=45||
       alignment_positions[2]!=90)return 1;
    nova_debug_string("UEFI:ALIGNMENT-SPACING-READY\n");
    static nova_stack_t stack_probe;
    nova_stack_child_t stack_probe_a={.id=1,.desired={0,0,40,20},
        .horizontal=NOVA_ALIGN_LEFT,.vertical=NOVA_ALIGN_TOP,.visible=true};
    nova_stack_child_t stack_probe_b={.id=2,.desired={0,0,20,10},
        .horizontal=NOVA_ALIGN_STRETCH,.vertical=NOVA_ALIGN_TOP,
        .visible=true,.stretch=true};
    if(!nova_stack_initialize(&stack_probe,NOVA_STACK_VERTICAL,
           (nova_rect_t){0,0,100,100},(nova_layout_insets_t){5,5,5,5},4,
           NOVA_DISTRIBUTE_NONE,1000,false)||
       !nova_stack_add(&stack_probe,&stack_probe_a)||
       !nova_stack_add(&stack_probe,&stack_probe_b)||
       !nova_stack_update(&stack_probe)||stack_probe.children[0].bounds.y!=5||
       stack_probe.children[1].bounds.y!=29||
       stack_probe.children[1].bounds.width!=90||
       stack_probe.children[1].bounds.height!=66)return 1;
    nova_debug_string("UEFI:STACK-LAYOUT-READY\n");
    static nova_grid_t grid_probe;
    nova_grid_child_t grid_probe_child={.id=1,.desired={0,0,40,20},
        .row=0,.column=0,.row_span=1,.column_span=1,
        .horizontal=NOVA_ALIGN_STRETCH,.vertical=NOVA_ALIGN_STRETCH_VERTICAL,
        .visible=true};
    if(!nova_grid_initialize(&grid_probe,(nova_rect_t){0,0,120,60},1000,4,4,false)||
       !nova_grid_add_row(&grid_probe,NOVA_GRID_FIXED,20)||
       !nova_grid_add_row(&grid_probe,NOVA_GRID_STAR,1)||
       !nova_grid_add_column(&grid_probe,NOVA_GRID_AUTO,0)||
       !nova_grid_add_column(&grid_probe,NOVA_GRID_PERCENT,500)||
       !nova_grid_add(&grid_probe,&grid_probe_child)||
       !nova_grid_update(&grid_probe)||grid_probe.columns[0].pixels!=40||
       grid_probe.columns[1].pixels!=58||grid_probe.rows[0].pixels!=20||
       grid_probe.rows[1].pixels!=36||grid_probe.children[0].bounds.width!=40||
       grid_probe.children[0].bounds.height!=20)return 1;
    nova_debug_string("UEFI:GRID-LAYOUT-READY\n");
    static nova_overlay_t overlay_probe;
    nova_overlay_child_t overlay_probe_background={.id=1,.desired={0},
        .horizontal=NOVA_ALIGN_STRETCH,.vertical=NOVA_ALIGN_STRETCH_VERTICAL,
        .layer=NOVA_OVERLAY_BACKGROUND,.material=NOVA_MATERIAL_ACRYLIC,
        .visibility=NOVA_OVERLAY_VISIBLE,.opacity=255};
    nova_overlay_child_t overlay_probe_dialog={.id=2,.desired={0,0,80,40},
        .horizontal=NOVA_ALIGN_CENTER,.vertical=NOVA_ALIGN_CENTER_VERTICAL,
        .layer=NOVA_OVERLAY_DIALOG,.material=NOVA_MATERIAL_GLASS,
        .visibility=NOVA_OVERLAY_VISIBLE,.z_index=1,.opacity=230,
        .modal=true,.interactive=true};
    if(!nova_overlay_initialize(&overlay_probe,(nova_rect_t){0,0,200,100},
           (nova_layout_insets_t){10,10,10,10},1000,true,false)||
       !nova_overlay_add(&overlay_probe,&overlay_probe_dialog)||
       !nova_overlay_add(&overlay_probe,&overlay_probe_background)||
       !nova_overlay_update(&overlay_probe)||overlay_probe.order[0]!=1||
       overlay_probe.order[1]!=0||overlay_probe.children[0].bounds.x!=60||
       overlay_probe.children[0].bounds.y!=30||!overlay_probe.modal_active||
       nova_overlay_input_allowed(&overlay_probe,1)||
       !nova_overlay_input_allowed(&overlay_probe,2))return 1;
    nova_debug_string("UEFI:OVERLAY-LAYOUT-READY\n");
    nova_constraint_spec_t constraint_probe={.width_mode=NOVA_SIZE_PERCENT,
        .height_mode=NOVA_SIZE_CONTENT,.width_percent_milli=500,
        .content_height_dlu=20,.margin={10,10,10,10},.padding={5,5,5,5},
        .aspect_width=4,.aspect_height=3};
    nova_rect_t constraint_probe_result;
    if(!nova_constraints_resolve_dlu(&constraint_probe,
           (nova_rect_t){0,0,220,140},&constraint_probe_result,1000)||
       constraint_probe_result.x!=10||constraint_probe_result.y!=10||
       constraint_probe_result.width!=110||constraint_probe_result.height!=82||
       nova_constraint_diagnostics()->aspect_adjustments<1)return 1;
    nova_debug_string("UEFI:CONSTRAINT-SIZING-READY\n");
    static const uint16_t resolution_matrix[][2]={{640,480},{800,600},{1024,768},
        {1280,720},{1280,800},{1366,768},{1600,900},{1920,1080},{2560,1440},
        {3840,2160},{1280,1024},{2560,1080},{3840,1080}};
    static const uint16_t scale_matrix[]={100,125,150,175,200};
    if(!nova_resolution_test_initialize())return 1;
    for(uint8_t resolution=0;resolution<13;++resolution)
        for(uint8_t scale=0;scale<5;++scale)
            if(!nova_resolution_test_execute(resolution_matrix[resolution][0],
                resolution_matrix[resolution][1],scale_matrix[scale]))return 1;
    static uint8_t resolution_report[16384];uint32_t resolution_report_size=0;
    if(nova_resolution_test_status()->passed!=65||
       !nova_resolution_test_status()->configuration_unchanged||
       !nova_resolution_test_generate_report(true,resolution_report,sizeof(resolution_report),
           &resolution_report_size)||!resolution_report_size)return 1;
    nova_debug_string("UEFI:RESOLUTION-COMPATIBILITY-READY\n");
    nova_rect_t responsive_dialog;
    if(!nova_responsive_update(false)||!nova_responsive_state()->valid||
       !nova_responsive_fit_dialog((nova_rect_t){0,0,4096,4096},&responsive_dialog)||
       responsive_dialog.x<nova_resolution_viewport()->safe_pixels.x||
       responsive_dialog.y<nova_resolution_viewport()->safe_pixels.y||
       responsive_dialog.x+responsive_dialog.width>
          nova_resolution_viewport()->safe_pixels.x+nova_resolution_viewport()->safe_pixels.width||
       responsive_dialog.y+responsive_dialog.height>
          nova_resolution_viewport()->safe_pixels.y+nova_resolution_viewport()->safe_pixels.height)
       return 1;
    nova_debug_string("UEFI:RESPONSIVE-LAYOUT-READY\n");
    const nova_rect_t *uefi_safe=nova_safe_area_get();
    if(!uefi_safe||uefi_safe->x!=(int32_t)(nova_graphics_width()*5u/100u)||
       uefi_safe->y!=(int32_t)(nova_graphics_height()*5u/100u)||
       !nova_safe_area_contains(responsive_dialog)||
       nova_safe_area_contains((nova_rect_t){0,0,16,16}))return 1;
    nova_debug_string("UEFI:SAFE-DISPLAY-AREA-READY\n");
    uint32_t gop_global_scale=nova_resolution_viewport()->scale_milli;
    int32_t gop_edge_a=0,gop_width_a=0,gop_edge_b=0,gop_width_b=0;
    if(!nova_scaling_update(true)||nova_scaling_get()->dpi_x!=96||
       nova_scaling_get()->effective_scale_milli!=
          nova_scaling_effective(gop_global_scale,true)||
       !nova_scale_interval(1,1,&gop_edge_a,&gop_width_a)||
       !nova_scale_interval(2,1,&gop_edge_b,&gop_width_b)||
       gop_edge_a+gop_width_a!=gop_edge_b||
       !nova_scaling_update(false)||nova_scaling_get()->global_scale_milli!=gop_global_scale)
       return 1;
    nova_debug_string("UEFI:DPI-RESOLUTION-SCALING-READY\n");
    if(!nova_runtime_subsystem_ready(NOVA_RUNTIME_CONFIGURATION)||
       !nova_runtime_loading())return 1;

    UINTN selection = 0;
    (void)nova_boot_budget_begin(NOVA_BOOT_PHASE_MANAGER);
    if (!bootmenu_initialize()) {
        nova_debug_string("UEFI:COMPOSITOR-UNAVAILABLE\n");
        nova_runtime_enter_recovery();
        return text_fallback(system_table,"Bootoberflaeche konnte nicht initialisiert werden.");
    }
    nova_input_event_t startup_device_event;uint8_t startup_device_events=0;
    while(nova_input_next(&startup_device_event)){
        if(startup_device_event.type!=NOVA_EVENT_SYSTEM)return 1;
        ++startup_device_events;
    }
    if(startup_device_events!=6)return 1;
    nova_input_event_t trace_probe={.timestamp=100,.device_id=1,.type=NOVA_EVENT_KEY_DOWN};
    trace_probe.key.key=30;trace_probe.key.unicode='X';
    nova_input_event_t trace_received;static uint8_t trace_export_probe[512];
    uint32_t trace_export_size=0;nova_input_set_secure(true);
    if(nova_input_trace_enabled()||!nova_input_trace_set_enabled(true,true)||
       !nova_input_post(trace_probe)||!nova_input_next(&trace_received)||
       !nova_input_trace_complete(trace_received.sequence,110,120,0,NOVA_VIEW_MAIN,0,true)||
       !nova_input_trace_get(0)->masked||nova_input_trace_get(0)->unicode||
       nova_input_trace_export(NOVA_INPUT_TRACE_JSON,false,trace_export_probe,
           sizeof(trace_export_probe),&trace_export_size)||
       !nova_input_trace_export(NOVA_INPUT_TRACE_NDF,true,trace_export_probe,
           sizeof(trace_export_probe),&trace_export_size)||!trace_export_size||
       !nova_input_trace_set_enabled(false,true))return 1;
    nova_input_set_secure(false);nova_input_trace_reset();
    nova_debug_string("UEFI:INPUT-EVENT-TRACING-READY\n");
    static uint8_t resource_diag_export[65536];uint32_t resource_diag_export_size=0;
    const nova_resource_diag_status_t *resource_diag_status=nova_resource_diag_status();
    if(!resource_diag_status->initialized||!resource_diag_status->passive||
       !resource_diag_status->read_only||!resource_diag_status->boot_unaffected||
       !resource_diag_status->requests||!resource_diag_status->loads||
       nova_resource_diag_export(NOVA_RESOURCE_EXPORT_JSON,false,resource_diag_export,
           sizeof(resource_diag_export),&resource_diag_export_size)||
       !nova_resource_diag_export(NOVA_RESOURCE_EXPORT_NDF,true,resource_diag_export,
           sizeof(resource_diag_export),&resource_diag_export_size)||!resource_diag_export_size)return 1;
    nova_debug_string("UEFI:RESOURCE-LOADING-DIAGNOSTICS-READY\n");
    uint64_t corruption_probe_id=nova_resource_id("boot://branding/novaos/logo");
    if(!nova_resource_corruption_initialize()||
       !nova_resource_corruption_execute(corruption_probe_id,NOVA_CORRUPTION_BIT_ERROR)||
       !nova_resource_corruption_execute(corruption_probe_id,NOVA_CORRUPTION_TRUNCATED)||
       !nova_resource_corruption_execute(corruption_probe_id,NOVA_CORRUPTION_EMPTY)||
       !nova_resource_corruption_execute(corruption_probe_id,NOVA_CORRUPTION_INVALID_HEADER)||
       !nova_resource_corruption_execute(corruption_probe_id,NOVA_CORRUPTION_INVALID_SIGNATURE)||
       !nova_resource_corruption_execute(corruption_probe_id,NOVA_CORRUPTION_INVALID_CHECKSUM)||
       !nova_resource_corruption_execute(corruption_probe_id,NOVA_CORRUPTION_INVALID_VERSION)||
       !nova_resource_corruption_execute(corruption_probe_id,NOVA_CORRUPTION_INVALID_METADATA)||
       nova_resource_corruption_status()->detected!=8||
       !nova_resource_corruption_status()->productive_data_unchanged)return 1;
    static uint8_t corruption_report[4096];uint32_t corruption_report_size=0;
    if(!nova_resource_corruption_generate_report(true,corruption_report,
        sizeof(corruption_report),&corruption_report_size)||!corruption_report_size)return 1;
    nova_debug_string("UEFI:RESOURCE-CORRUPTION-TESTS-READY\n");
    if(!nova_memory_budget_set_runtime(true)||
       !nova_low_end_profile_update(30,33333,
            nova_memory_budget_status()->used_memory,
            nova_resource_diagnostics()->cached_bytes)||
       !nova_low_end_profile_status()->runtime_heap_locked)return 1;
    const nova_boot_configuration_t *boot_configuration=nova_configuration_effective();
    nova_diag_set_quality(boot_configuration->quality,
                          boot_configuration->quality==NOVA_QUALITY_AUTO);
    nova_navigation_initialize((nova_navigation_entry_t){NOVA_VIEW_MAIN,0,0,0,0});
    if(!nova_navigation_test_initialize())return 1;
    for(uint32_t route=0;route<NOVA_NAVIGATION_TEST_CAPACITY;++route)
        if(!nova_navigation_test_execute(route))return 1;
    static uint8_t navigation_test_report[2048];uint32_t navigation_test_report_size=0;
    if(nova_navigation_test_summary()->passed!=8||
       nova_navigation_test_summary()->skipped!=2||
       nova_navigation_test_summary()->failed||
       !nova_navigation_test_summary()->configuration_unchanged||
       !nova_navigation_test_generate_report(true,navigation_test_report,
           sizeof(navigation_test_report),&navigation_test_report_size)||
       navigation_test_report_size<300)return 1;
    nova_debug_string("UEFI:NAVIGATION-TESTS-READY\n");
    nova_dialog_initialize();
    if(!nova_dialog_test_initialize())return 1;
    for(uint8_t dialog_type=NOVA_DIALOG_INFORMATION;
        dialog_type<=NOVA_DIALOG_CUSTOM;++dialog_type)
        if(!nova_dialog_test_execute((nova_dialog_type_t)dialog_type))return 1;
    static uint8_t dialog_test_report[2048];uint32_t dialog_test_report_size=0;
    if(nova_dialog_test_summary()->passed!=NOVA_DIALOG_TEST_CAPACITY||
       nova_dialog_test_summary()->failed||
       !nova_dialog_test_summary()->configuration_unchanged||
       !nova_dialog_test_generate_report(true,dialog_test_report,
           sizeof(dialog_test_report),&dialog_test_report_size)||
       dialog_test_report_size<300)return 1;
    nova_debug_string("UEFI:DIALOG-TESTS-READY\n");
    nova_debug_string("UEFI:DIALOG-ARCHITECTURE-READY\n");
    nova_debug_string("UEFI:MODAL-DIALOG-READY\n");
    if(!nova_control_test_initialize())return 1;
    for(uint32_t control_case=0;control_case<NOVA_CONTROL_TEST_CAPACITY;++control_case)
        if(!nova_control_test_execute(control_case))return 1;
    static uint8_t control_test_report[2048];uint32_t control_test_report_size=0;
    if(nova_control_test_summary()->passed!=10||nova_control_test_summary()->skipped!=2||
       nova_control_test_summary()->failed||
       !nova_control_test_summary()->configuration_unchanged||
       !nova_control_test_generate_report(true,control_test_report,
           sizeof(control_test_report),&control_test_report_size)||
       control_test_report_size<350)return 1;
    nova_debug_string("UEFI:CONTROL-INTERACTION-TESTS-READY\n");
    nova_control_t *password_probe=nova_password_field_create();
    if(!password_probe||!nova_control_set_state(password_probe,NOVA_CONTROL_INITIALIZED)||
       !nova_control_set_state(password_probe,NOVA_CONTROL_VISIBLE)||
       !nova_password_field_set_placeholder(password_probe,"Passwort")||
       nova_control_set_text(password_probe,"Klartext")||
       !nova_text_field_insert(password_probe,'N')||!nova_text_field_insert(password_probe,'o')||
       !nova_text_field_insert(password_probe,'v')||!nova_text_field_insert(password_probe,'a')||
       !nova_text_field_insert(password_probe,'7')||
       !nova_password_field_validate(password_probe,5,true,true,true)||
       !nova_password_field_enable_reveal(password_probe,true)||
       !nova_password_field_set_revealed(password_probe,true,100)||
       !nova_password_field_tick(password_probe,100)||
       (password_probe->flags&NOVA_CONTROL_FLAG_REVEALED))return 1;
    nova_password_field_clear(password_probe);
    if(!nova_password_field_empty(password_probe)||
       !nova_control_destroy(password_probe))return 1;
    nova_debug_string("UEFI:PASSWORD-FIELD-READY\n");
    nova_control_t *event_parent=nova_control_create(NOVA_CONTROL_LIST);
    nova_control_t *event_child=nova_control_create(NOVA_CONTROL_LIST_ITEM);
    bool child_handles=false,parent_handles=true;uint32_t event_action=0;
    if(!event_parent||!event_child||!nova_control_set_parent(event_child,event_parent)||
       !nova_control_set_state(event_parent,NOVA_CONTROL_INITIALIZED)||
       !nova_control_set_state(event_parent,NOVA_CONTROL_VISIBLE)||
       !nova_control_set_state(event_child,NOVA_CONTROL_INITIALIZED)||
       !nova_control_set_state(event_child,NOVA_CONTROL_VISIBLE)||
       !nova_control_set_event_handler(event_child,uefi_control_event_handler,&child_handles)||
       !nova_control_set_event_handler(event_parent,uefi_control_event_handler,&parent_handles))return 1;
    uefi_control_event_count=0;
    if(!nova_control_set_bounds(event_child,(nova_rect_t){1,1,10,10})||
       uefi_control_event_count!=2)return 1;
    event_child->action=77;uefi_control_event_count=0;
    if(!nova_control_invoke(event_child,&event_action)||event_action!=77||
       uefi_control_event_count!=2||uefi_control_event_order[0]!=event_child->id||
       uefi_control_event_order[1]!=event_parent->id)return 1;
    nova_control_release(event_child);
    static const nova_control_event_type_t input_event_types[]={
        NOVA_CONTROL_EVENT_MOUSE_ENTER,NOVA_CONTROL_EVENT_MOUSE_MOVE,
        NOVA_CONTROL_EVENT_MOUSE_DOWN,NOVA_CONTROL_EVENT_MOUSE_UP,
        NOVA_CONTROL_EVENT_MOUSE_LEAVE,NOVA_CONTROL_EVENT_DOUBLE_CLICK,
        NOVA_CONTROL_EVENT_KEY_DOWN,
        NOVA_CONTROL_EVENT_CHARACTER_INPUT};
    for(uint8_t i=0;i<sizeof(input_event_types)/sizeof(input_event_types[0]);++i){
        nova_control_event_t routed={.type=input_event_types[i],
            .device_id=i<5?2u:1u,.key=2,.character='N',.x=2,.y=2};
        uefi_control_event_count=0;
        if(!nova_control_dispatch_event(event_child,&routed)||
           uefi_control_event_count!=2||uefi_control_event_order[0]!=event_child->id||
           uefi_control_event_order[1]!=event_parent->id)return 1;
    }
    nova_focus_scope_t page_scope,overlay_scope;
    if(!nova_control_set_bounds(event_parent,(nova_rect_t){1,1,10,4})||
       !nova_control_set_bounds(event_child,(nova_rect_t){1,10,10,4})||
       !nova_input_neighbor_set(event_parent,NOVA_DIRECTION_DOWN,event_child))return 1;
    if(!nova_input_focus_set(event_parent))return 1;
    if(!nova_input_focus_scope_configure(&page_scope,1,event_parent)||
       !nova_input_focus_scope_register(&page_scope,event_parent)||
       !nova_input_focus_scope_register(&page_scope,event_child)||
       !nova_input_focus_scope_build_navigation(&page_scope,NOVA_NAV_EDGE_WRAP))return 1;
    if(!nova_input_focus_enter_scope(&page_scope)||
       nova_input_focus_get()!=event_parent||
       !nova_input_focus_navigate(NOVA_DIRECTION_DOWN)||
       nova_input_focus_get()!=event_child||
       !nova_input_focus_navigate(NOVA_DIRECTION_UP)||
       nova_input_focus_get()!=event_parent||!nova_input_focus_next()||
       nova_input_focus_get()!=event_child||
       nova_input_directional_diagnostics()->explicit_hits!=1||
       nova_input_directional_diagnostics()->geometric_hits!=1)return 1;
    nova_debug_string("UEFI:DIRECTIONAL-FOCUS-READY\n");
    if(!nova_input_focus_scope_configure(&overlay_scope,2,event_parent)||
       !nova_input_focus_scope_register(&overlay_scope,event_parent)||
       !nova_input_focus_enter_scope(&overlay_scope)||
       nova_input_focus_get()!=event_parent||nova_input_focus_set(event_child))return 1;
    if(!nova_input_focus_leave_scope()||nova_input_focus_get()!=event_child||
       !nova_input_focus_leave_scope()||nova_input_focus_get()!=event_parent||
       nova_input_focus_diagnostics()->restores!=2||
       !nova_input_focus_visible())return 1;
    nova_debug_string("UEFI:FOCUS-SCOPE-MANAGER-READY\n");
    nova_shortcut_t global_shortcut={.key=59,.command=10,
        .scope=NOVA_SHORTCUT_GLOBAL,.focus_id=NOVA_CONTROL_NONE,.enabled=true};
    nova_shortcut_t context_shortcut={.key=59,.command=20,
        .scope=NOVA_SHORTCUT_CONTEXT,.context_id=5,
        .focus_id=NOVA_CONTROL_NONE,.enabled=true};
    nova_shortcut_t dialog_shortcut={.key=59,.command=30,
        .scope=NOVA_SHORTCUT_DIALOG,.context_id=7,
        .focus_id=NOVA_CONTROL_NONE,.enabled=true};
    nova_input_event_t shortcut_event={.device_id=1,.type=NOVA_EVENT_KEY_DOWN};
    shortcut_event.key.key=59;uint32_t shortcut_command=0;
    if(!nova_input_shortcut_register_ex(&global_shortcut)||
       !nova_input_shortcut_register_ex(&context_shortcut)||
       !nova_input_shortcut_register_ex(&dialog_shortcut)||
       nova_input_shortcut_register_ex(&dialog_shortcut))return 1;
    nova_input_shortcut_set_context(5,7);
    if(!nova_input_shortcut_resolve(&shortcut_event,&shortcut_command)||
       shortcut_command!=30||
       !nova_input_shortcut_set_handler(uefi_shortcut_handler,&uefi_shortcut_calls)||
       !nova_input_shortcut_execute(&shortcut_event)||uefi_shortcut_command!=30||
       uefi_shortcut_calls!=1||
       !nova_input_shortcut_unregister(59,0,NOVA_SHORTCUT_DIALOG,7,NOVA_CONTROL_NONE))return 1;
    nova_input_shortcut_set_context(5,0);
    if(!nova_input_shortcut_resolve(&shortcut_event,&shortcut_command)||shortcut_command!=20||
       nova_input_shortcut_diagnostics()->conflicts!=1)return 1;
    nova_debug_string("UEFI:KEYBOARD-SHORTCUTS-READY\n");
    nova_input_event_t repeat_event={.device_id=1,.type=NOVA_EVENT_KEY_DOWN};
    repeat_event.key.key=70;nova_input_event_t repeated_event;
    if(!nova_input_repeat_configure(400,50,true)||
       !nova_input_repeat_start_event(70,&repeat_event,100,false))return 1;
    nova_input_repeat_update(499);
    if(nova_input_next(&repeated_event))return 1;
    nova_input_repeat_update(500);
    if(!nova_input_next(&repeated_event)||!repeated_event.key.repeat||
       nova_input_repeat_context(70)->state!=NOVA_REPEAT_REPEATING||
       nova_input_repeat_context(70)->count!=1)return 1;
    nova_input_repeat_stop(70);
    if(nova_input_repeat_context(70)->duration_ms!=400||
       !nova_input_repeat_start_event(71,&repeat_event,600,false)||
       !nova_input_focus_set(event_child)||nova_input_repeat_active(71)||
       nova_input_repeat_diagnostics()->focus_stops!=1||
       nova_input_repeat_start_event(72,&repeat_event,600,true)||
       !nova_input_repeat_configure(600,100,false)||
       nova_input_repeat_start_event(73,&repeat_event,600,false)||
       !nova_input_repeat_configure(400,50,true)||
       !nova_input_focus_set(event_parent))return 1;
    nova_debug_string("UEFI:INPUT-REPEAT-READY\n");
    if(nova_input_pointer_click_update(2,0,event_child,2,2,100,false,500,4)||
       !nova_input_pointer_click_update(2,0,event_child,3,3,550,true,500,4)||
       nova_input_diagnostics()->double_clicks!=1)return 1;
    nova_debug_string("UEFI:DOUBLE-CLICK-DETECTION-READY\n");
    nova_control_t *hot_focus=nova_input_focus_get();
    if(nova_input_device_count()!=2||!nova_input_device_connected(1)||
       !nova_input_device_connected(2)||
       nova_input_device_get(2)->state!=NOVA_DEVICE_STATE_READY||
       !nova_input_device_set(2,NOVA_DEVICE_MOUSE,false)||
       nova_input_device_connected(2)||nova_input_focus_get()!=hot_focus||
       !nova_input_device_set(2,NOVA_DEVICE_MOUSE,true)||
       !nova_input_device_connected(2)||
       nova_input_device_get(2)->changes!=7||
       nova_input_device_set(18,NOVA_DEVICE_MOUSE,true))return 1;
    while(nova_input_next(&startup_device_event)){
        if(startup_device_event.type!=NOVA_EVENT_SYSTEM)return 1;
    }
    nova_debug_string("UEFI:INPUT-HOT-DETECTION-READY\n");
    if(nova_input_pointer_scale_delta(80,25)!=20||
       nova_input_pointer_scale_delta(80,100)!=80||
       nova_input_pointer_scale_delta(80,400)!=320||
       nova_input_pointer_scale_delta(1,25)!=1||
       nova_input_pointer_scale_delta(-1,25)!=-1)return 1;
    nova_debug_string("UEFI:POINTER-SPEED-READY\n");
    nova_input_event_t capture_probe={.type=NOVA_EVENT_POINTER_MOVE};
    capture_probe.pointer.pointer_id=0;capture_probe.pointer.x=200;capture_probe.pointer.y=200;
    if(!nova_input_pointer_capture(0,event_child)||
       !nova_input_pointer_is_captured(0)||
       nova_input_pointer_capture_owner(0)!=event_child||
       nova_input_pointer_target(&capture_probe)!=event_child||
       nova_input_pointer_capture_state(0)!=NOVA_CAPTURE_ACTIVE)return 1;
    nova_input_pointer_release(0);
    if(nova_input_pointer_is_captured(0)||
       nova_input_pointer_capture_state(0)!=NOVA_CAPTURE_RELEASED||
       nova_input_pointer_capture_diagnostics(0)->owner_id!=event_child->id)return 1;
    uefi_control_event_count=0;nova_input_advance_time(30);
    if(!nova_input_pointer_capture(0,event_child))return 1;
    nova_input_advance_time(45);nova_input_pointer_cancel(0);
    if(nova_input_pointer_capture_state(0)!=NOVA_CAPTURE_CANCELLED||
       nova_input_pointer_capture_diagnostics(0)->duration_ms!=45||
       nova_input_pointer_capture_diagnostics(0)->cancels!=1||
       uefi_control_event_count!=2)return 1;
    nova_debug_string("UEFI:POINTER-CAPTURE-CANCEL-EVENT-READY\n");
    uefi_control_event_count=0;
    if(!nova_input_pointer_capture(0,event_child))return 1;
    nova_input_advance_time(20);
    nova_dialog_t *capture_dialog=nova_dialog_open(NOVA_DIALOG_INFORMATION,
        "Capture","Dialogwechsel",true,true,event_child->id);
    nova_dialog_result_t capture_result=NOVA_DIALOG_RESULT_NONE;
    if(!capture_dialog||nova_input_pointer_capture_state(0)!=NOVA_CAPTURE_CANCELLED||
       nova_input_pointer_capture_diagnostics(0)->duration_ms!=20||
       uefi_control_event_count!=2||!nova_dialog_cancel(&capture_result)||
       capture_result!=NOVA_DIALOG_RESULT_CANCEL)return 1;
    nova_debug_string("UEFI:DIALOG-CAPTURE-CANCEL-READY\n");
    uefi_control_event_count=0;nova_recovery_initialize();
    if(!nova_input_pointer_capture(0,event_child))return 1;
    nova_input_advance_time(30);
    if(!nova_recovery_report(0x80010022u,NOVA_UI_SUBSYSTEM_INPUT,
        NOVA_UI_ERROR_RECOVERABLE,NOVA_RECOVERY_LOCAL_FALLBACK,100)||
       nova_input_pointer_capture_state(0)!=NOVA_CAPTURE_CANCELLED||
       nova_input_pointer_capture_diagnostics(0)->duration_ms!=30||
       uefi_control_event_count!=2)return 1;
    nova_debug_string("UEFI:RECOVERY-CAPTURE-CANCEL-READY\n");
    nova_debug_string("UEFI:POINTER-CAPTURE-READY\n");
    nova_debug_string("UEFI:CONTROL-INPUT-EVENTS-READY\n");
    if(!nova_control_destroy(event_child)||!nova_control_destroy(event_parent))return 1;
    nova_debug_string("UEFI:CONTROL-EVENT-ROUTING-READY\n");
    static const uint32_t render_reference[16]={
        0xff101112u,0xff202122u,0xff303132u,0xff404142u,
        0xff505152u,0xff606162u,0xff707172u,0xff808182u,
        0xff909192u,0xffa0a1a2u,0xffb0b1b2u,0xffc0c1c2u,
        0xffd0d1d2u,0xffe0e1e2u,0xfff0f1f2u,0xff010203u};
    static uint32_t render_actual[16],render_difference[16];
    static uint8_t render_mask[16];
    for(uint8_t i=0;i<16;++i){render_actual[i]=render_reference[i];render_mask[i]=1;}
    nova_render_reference_metadata_t render_metadata={.reference_id=2002,.version=1,
        .width=4,.height=4,.dpi=96,.theme=NOVA_REFERENCE_DARK,
        .profile=NOVA_REFERENCE_SOFTWARE,.created_epoch=1,.read_only=true,.validated=true};
    if(!nova_render_reference_initialize()||
       !nova_render_reference_execute(1,&render_metadata,render_reference,render_actual,0,4,
                                      render_difference,4))return 1;
    render_actual[6]=0xffffffffu;
    if(nova_render_reference_execute(2,&render_metadata,render_reference,render_actual,0,4,
                                     render_difference,4)||render_difference[6]!=0xffff00ffu)return 1;
    render_mask[6]=0;
    if(!nova_render_reference_execute(3,&render_metadata,render_reference,render_actual,render_mask,4,
                                      render_difference,4))return 1;
    static uint8_t render_reference_report[1024];uint32_t render_reference_report_size=0;
    if(nova_render_reference_summary()->passed!=2||nova_render_reference_summary()->failed!=1||
       !nova_render_reference_generate_report(true,render_reference_report,
           sizeof(render_reference_report),&render_reference_report_size)||
       render_reference_report_size<150)return 1;
    nova_debug_string("UEFI:RENDER-REFERENCE-TESTS-READY\n");
    static uefi_test_probe_t architecture_probes[4]={{10},{8},{10},{2}};
    static const nova_boot_test_case_t architecture_cases[4]={
        {1101,1,1,"Controls","Control Interaction Suite",10,1,5,1,1,1,
            NOVA_TEST_COMPONENT,NOVA_TEST_FUNCTIONAL,uefi_test_execute,&architecture_probes[0]},
        {1102,1,1,"Navigation","Navigation Suite",8,2,5,1,1,1,
            NOVA_TEST_INTEGRATION,NOVA_TEST_FUNCTIONAL,uefi_test_execute,&architecture_probes[1]},
        {1103,1,1,"Dialogs","Dialog Suite",10,3,5,1,1,1,
            NOVA_TEST_UI,NOVA_TEST_GRAPHICAL,uefi_test_execute,&architecture_probes[2]},
        {1104,1,1,"Rendering","Reference Comparator",2,4,5,1,1,1,
            NOVA_TEST_REGRESSION,NOVA_TEST_GRAPHICAL,uefi_test_execute,&architecture_probes[3]}};
    if(!nova_boot_test_initialize(startup_time_us,uefi_test_state))return 1;
    for(uint8_t i=0;i<4;++i)if(!nova_boot_test_register(&architecture_cases[i]))return 1;
    static uint8_t architecture_report[2048];uint32_t architecture_report_size=0;
    if(!nova_boot_test_run(1)||nova_boot_test_summary()->passed!=4||
       nova_boot_test_summary()->failed||!nova_boot_test_summary()->isolated||
       !nova_boot_test_generate_report(true,architecture_report,sizeof(architecture_report),
            &architecture_report_size)||architecture_report_size<250)return 1;
    nova_debug_string("UEFI:BOOT-TEST-ARCHITECTURE-READY\n");
    if(!nova_runtime_ready()||!nova_runtime_run())return 1;
    nova_debug_string("UEFI:RUNTIME-RUNNING\n");
    nova_debug_string("UEFI:NAVIGATION-READY\n");
    nova_debug_string("UEFI:DIALOG-READY\n");
    uefi_power_initialize(system_table);
    uefi_firmware_initialize(system_table);
    bool pointer_available = uefi_pointer_initialize(system_table);
    if(!nova_input_device_scan())return 1;
    pointer_available=uefi_pointer_available();
    nova_debug_string("UEFI:INPUT-DEVICE-MONITOR-READY\n");
    int32_t entrance_opacity = 0;
    nova_animation_t entrance = {
        &entrance_opacity, 0, 255, 0, 0, 180, 1, 3, 0,
        NOVA_PROPERTY_OPACITY, NOVA_EASE_OUT_CUBIC, NOVA_MOTION_CREATED,
        false, false, true, 0, 0, 0
    };
    if (!nova_motion_create(&entrance)) return 1;
    for (uint64_t elapsed = 0; elapsed <= 180; elapsed += 20) {
        nova_motion_update(elapsed);
        bootmenu_draw(selection, (uint8_t)entrance_opacity);
        if(elapsed==0){
            (void)nova_boot_budget_first_frame(startup_time_us());
            (void)nova_boot_budget_end(NOVA_BOOT_PHASE_MANAGER);
        }
        system_table->BootServices->Stall(20000);
    }
    if(nova_sw_renderer_text_required())
        return text_fallback(system_table,"Software-Renderer konnte nicht wiederhergestellt werden.");
    nova_debug_string("UEFI:MOTION-READY\n");
    nova_diag_frame(20000, 500, 500, 8000, 4000);
    static uint8_t animation_export[4096];uint32_t animation_export_size=0;
    const nova_animation_diag_status_t *animation_status=nova_animation_diag_status();
    if(!animation_status->initialized||!animation_status->passive||
       !animation_status->read_only||!animation_status->boot_unaffected||
       !animation_status->started||!animation_status->completed||
       nova_animation_diag_export(NOVA_ANIMATION_EXPORT_JSON,false,animation_export,
           sizeof(animation_export),&animation_export_size)||
       !nova_animation_diag_export(NOVA_ANIMATION_EXPORT_NDF,true,animation_export,
           sizeof(animation_export),&animation_export_size)||!animation_export_size)return 1;
    nova_debug_string("UEFI:ANIMATION-DIAGNOSTICS-READY\n");
    nova_diag_snapshot();
    const nova_render_statistics_t *rendering=nova_render_statistics_current();
    static uint8_t rendering_export[1024];uint32_t rendering_export_size=0;
    if(!rendering->frame_count||!rendering->frame_time_us||
       !nova_render_statistics_average(NOVA_RENDER_WINDOW_SESSION)->frame_count||
       nova_render_statistics_export(NOVA_RENDER_EXPORT_CSV,false,rendering_export,
           sizeof(rendering_export),&rendering_export_size)||
       !nova_render_statistics_export(NOVA_RENDER_EXPORT_CSV,true,rendering_export,
           sizeof(rendering_export),&rendering_export_size)||!rendering_export_size)return 1;
    nova_debug_string("UEFI:RENDERING-STATISTICS-READY\n");
    static uint8_t regression_report[2048];uint32_t regression_report_size=0;
    if(!nova_performance_regression_initialize("qemu-current","1.0",1,1)||
       !nova_performance_regression_set_baseline(NOVA_BENCH_FRAME_TIME,20000,true,500)||
       !nova_performance_regression_measure(NOVA_BENCH_FRAME_TIME,21000)||
       nova_performance_regression_results()[NOVA_BENCH_FRAME_TIME].level!=NOVA_REGRESSION_NONE||
       !nova_performance_regression_generate_report(true,regression_report,
           sizeof(regression_report),&regression_report_size)||!regression_report_size)return 1;
    nova_debug_string("UEFI:PERFORMANCE-REGRESSION-READY\n");
    if(!nova_fallback_test_initialize())return 1;
    for(uint8_t mode=0;mode<NOVA_FALLBACK_MODE_COUNT;++mode)
        if(!nova_fallback_test_execute((nova_fallback_mode_t)mode))return 1;
    static uint8_t fallback_report[4096];uint32_t fallback_report_size=0;
    if(nova_fallback_test_status()->successful!=NOVA_FALLBACK_MODE_COUNT||
       !nova_fallback_test_status()->security_preserved||
       !nova_fallback_test_generate_report(true,fallback_report,sizeof(fallback_report),
           &fallback_report_size)||!fallback_report_size)return 1;
    nova_recovery_initialize();nova_compositor_set_fallback(0);
    nova_motion_set_reduced(nova_configuration_effective()->reduced_motion);
    nova_debug_string("UEFI:FALLBACK-MODE-TESTS-READY\n");
    nova_debug_string("UEFI:DIAGNOSTICS-READY\n");
    if(!nova_boot_budget_phase(NOVA_BOOT_PHASE_GRAPHICS)->complete||
       !nova_boot_budget_phase(NOVA_BOOT_PHASE_RESOURCES)->complete||
       !nova_boot_budget_phase(NOVA_BOOT_PHASE_THEME)->complete||
       !nova_boot_budget_phase(NOVA_BOOT_PHASE_UI)->complete||
       !nova_boot_budget_phase(NOVA_BOOT_PHASE_MANAGER)->complete||
       !nova_boot_budget_diagnostics()->first_frame_valid)return 1;
    nova_debug_string("UEFI:STARTUP-BUDGET-READY\n");

    /* Fuenf Sekunden; jede Taste beendet den automatischen Start. */
    EFI_INPUT_KEY key = {0, 0};
    bool pointer_activity = false;
    static const char *const countdown[5] = {
        "Auswahl startet in 1 Sek.", "Auswahl startet in 2 Sek.",
        "Auswahl startet in 3 Sek.", "Auswahl startet in 4 Sek.",
        "Auswahl startet in 5 Sek."
    };
    static const char *const countdown_marker[5] = {
        "UEFI:COUNTDOWN-1\n", "UEFI:COUNTDOWN-2\n", "UEFI:COUNTDOWN-3\n",
        "UEFI:COUNTDOWN-4\n", "UEFI:COUNTDOWN-5\n"
    };
    for (UINTN tick = 0; tick < 50; ++tick) {
        if ((tick % 10) == 0) {
            bootmenu_set_status(countdown[4 - tick / 10]);
            nova_debug_string(countdown_marker[4 - tick / 10]);
            bootmenu_draw(selection, 255);
            if(nova_sw_renderer_text_required())
                return text_fallback(system_table,"Software-Renderer konnte nicht wiederhergestellt werden.");
            if (tick == 0) nova_debug_string("UEFI:COUNTDOWN-FRAME-READY\n");
        }
        system_table->BootServices->Stall(100000);
        key = read_key(system_table);
        if (key.ScanCode || key.UnicodeChar) {
            nova_debug_string("UEFI:COUNTDOWN-CANCELLED\n");
            bootmenu_set_status("");
            bootmenu_draw(selection, 255);
            break;
        }
        if (pointer_available) {
            uefi_pointer_event_t event;bool activate=false;
            if (uefi_pointer_poll(nova_graphics_width(),nova_graphics_height(),&event)) {
                bootmenu_scroll_wheel(event.wheel,&selection);
                pointer_activity=event.absolute?
                    bootmenu_pointer_absolute_event(event.x,event.y,event.left,event.right,
                                                    &selection,&activate):
                    bootmenu_pointer_event(event.dx,event.dy,event.left,event.right,
                                           &selection,&activate);
                bootmenu_draw(selection, 255);
                nova_debug_string("UEFI:COUNTDOWN-CANCELLED\n");
                bootmenu_set_status("");
                if (activate) {
                    if (handle_action(&selection)) return EFI_SUCCESS;
                    bootmenu_draw(selection, 255);
                }
                break;
            }
        }
    }

    if (!key.ScanCode && !key.UnicodeChar && !pointer_activity) {
        boot_selected(0);
        return EFI_SUCCESS;
    }

    /* Relative pointer packets can arrive substantially faster than the GOP
       framebuffer can be composed in software.  Keep polling at 5 ms for low
       input latency, but coalesce motion to roughly one 60 Hz visual frame.
       Button edges and wheel input are dispatched immediately so clicks never
       feel delayed or get swallowed. */
    int32_t pending_pointer_dx=0,pending_pointer_dy=0,pending_pointer_wheel=0;
    int32_t pending_pointer_x=0,pending_pointer_y=0;
    bool pending_pointer=false,pending_left=false,pending_right=false;
    bool pending_pointer_absolute=false;
    bool dispatched_left=false,dispatched_right=false;
    uint32_t pointer_frame_elapsed_ms=0,device_scan_elapsed_ms=0;
    for (;;) {
        if(nova_sw_renderer_text_required())
            return text_fallback(system_table,"Software-Renderer konnte nicht wiederhergestellt werden.");
        if(key.ScanCode||key.UnicodeChar)(void)bootmenu_key_event(key.ScanCode,key.UnicodeChar);
        if(bootmenu_help_search_active()){
            bootmenu_help_search_input(key.ScanCode,key.UnicodeChar);
            bootmenu_draw(selection,255);
            nova_debug_string("UEFI:HELP-SEARCH-STABLE\n");
        } else if (bootmenu_context_active()) {
            if(key.ScanCode==1)bootmenu_context_move(-1);
            else if(key.ScanCode==2)bootmenu_context_move(1);
            else if(key.ScanCode==5)bootmenu_context_edge(false);
            else if(key.ScanCode==6)bootmenu_context_edge(true);
            else if(key.UnicodeChar==13||key.UnicodeChar==32)handle_context_action(selection);
            else if(key.ScanCode==23||key.ScanCode==4)bootmenu_context_close();
            bootmenu_draw(selection,255);
        } else if (nova_dialog_active()) {
            nova_dialog_result_t result = NOVA_DIALOG_RESULT_NONE;
            nova_dialog_t *active_dialog=nova_dialog_active();
            if(active_dialog->type==NOVA_DIALOG_CREDENTIAL&&key.UnicodeChar==8){
                if(nova_dialog_credential_backspace(active_dialog))
                    nova_debug_string("UEFI:PASSWORD-FIELD-BACKSPACE\n");
            } else if(active_dialog->type==NOVA_DIALOG_CREDENTIAL&&
                      key.UnicodeChar>=0x20&&key.UnicodeChar!=0x7f){
                if(nova_dialog_credential_input(active_dialog,key.UnicodeChar))
                    nova_debug_string("UEFI:PASSWORD-FIELD-INPUT\n");
            } else if (key.ScanCode == 1) nova_dialog_focus_move(-1);
            else if (key.ScanCode == 2) nova_dialog_focus_move(1);
            else if (key.UnicodeChar == 13) {
                if (close_dialog_animated(false,selection,&result)) handle_dialog_result(result);
                else nova_debug_string("UEFI:DIALOG-DESTRUCTIVE-ARMED\n");
            } else if (key.ScanCode == 23) {
                if (close_dialog_animated(true,selection,&result)) handle_dialog_result(result);
            }
            bootmenu_draw(selection, 255);
        } else if (bootmenu_view()==NOVA_VIEW_SETTINGS&&selection==3&&
                   (key.ScanCode==3||key.ScanCode==4||key.ScanCode==5||key.ScanCode==6)) {
            if(key.ScanCode==3)bootmenu_settings_adjust_tooltip_delay(1);
            else if(key.ScanCode==4)bootmenu_settings_adjust_tooltip_delay(-1);
            else bootmenu_settings_set_tooltip_delay_edge(key.ScanCode==6);
            bootmenu_draw(selection,255);
            nova_debug_string("UEFI:SETTINGS-CONTROLS-STABLE\n");
        } else if(key.UnicodeChar==9&&bootmenu_view()!=NOVA_VIEW_MAIN){
            bootmenu_breadcrumb_focus(!bootmenu_breadcrumb_focused());
            bootmenu_draw(selection,255);
        } else if(bootmenu_breadcrumb_focused()&&
                  (key.UnicodeChar==13||key.UnicodeChar==32)){
            navigate_root(&selection);
        } else if(bootmenu_tile_move(key.ScanCode,&selection)){
            bootmenu_draw(selection,255);
        } else if(bootmenu_scroll_key(key.ScanCode,&selection)){
            bootmenu_draw(selection,255);
        } else if(key.ScanCode==5||key.ScanCode==6){
            selection=key.ScanCode==5?0:(UINTN)bootmenu_item_count()-1u;
            while(!bootmenu_item_available((uint16_t)selection))
                selection=key.ScanCode==5?selection+1:selection-1;
            bootmenu_draw(selection,255);nova_debug_string("UEFI:LIST-HOME-END\n");
        } else if(key.ScanCode==9||key.ScanCode==10){
            UINTN step=4;
            if(key.ScanCode==9)selection=selection>step?selection-step:0;
            else selection=selection+step<bootmenu_item_count()?selection+step:
                           bootmenu_item_count()-1u;
            while(!bootmenu_item_available((uint16_t)selection))
                selection=key.ScanCode==9?(selection?selection-1:0):
                          (selection+1<bootmenu_item_count()?selection+1:selection);
            bootmenu_draw(selection,255);nova_debug_string("UEFI:LIST-PAGE-NAVIGATION\n");
        } else if (key.ScanCode == 1) {
            bootmenu_breadcrumb_focus(false);
            do{selection=selection?selection-1:(UINTN)bootmenu_item_count()-1u;}
            while(!bootmenu_item_available((uint16_t)selection));
            bootmenu_scroll_selection_into_view(selection);
            bootmenu_draw(selection, 255);
        } else if (key.ScanCode == 2) {
            bootmenu_breadcrumb_focus(false);
            do{selection=(selection+1)%bootmenu_item_count();}
            while(!bootmenu_item_available((uint16_t)selection));
            bootmenu_scroll_selection_into_view(selection);
            bootmenu_draw(selection, 255);
        } else if (key.UnicodeChar == 13 ||
                   (key.UnicodeChar==32&&bootmenu_view()!=NOVA_VIEW_HELP)) {
            if (handle_action(&selection)) return EFI_SUCCESS;
            bootmenu_draw(selection, 255);
            if(nova_dialog_active())nova_debug_string("UEFI:DIALOG-STABLE\n");
            if(bootmenu_view()==NOVA_VIEW_SETTINGS&&(selection==2||selection==3))
                nova_debug_string("UEFI:SETTINGS-CONTROLS-STABLE\n");
            if(bootmenu_view()==NOVA_VIEW_FIRMWARE)
                nova_debug_string("UEFI:FIRMWARE-VIEW-STABLE\n");
        } else if (key.ScanCode == 23) {
            nova_debug_string("UEFI:BACK\n");
            pending_power = (UINTN)-1;
            if (!navigate_back(&selection)) return EFI_SUCCESS;
            bootmenu_set_status("");
            bootmenu_draw(selection, 255);
        } else if (key.ScanCode == 16 &&
                   bootmenu_view()==NOVA_VIEW_DIAGNOSTICS) {
            if(bootmenu_runtime_self_test())bootmenu_draw(selection,255);
        } else if (key.ScanCode == 18 &&
                   bootmenu_view()==NOVA_VIEW_DIAGNOSTICS) {
            if(bootmenu_memory_self_test())bootmenu_draw(selection,255);
        } else if (key.ScanCode == 19 &&
                   bootmenu_view()==NOVA_VIEW_DIAGNOSTICS) {
            nova_debug_string("UEFI:TEXT-FALLBACK-SELF-TEST\n");
            return text_fallback(system_table,"Diagnose hat einen fatalen UI-Fehler simuliert.");
        } else if (key.ScanCode == 20 &&
                   bootmenu_view()==NOVA_VIEW_DIAGNOSTICS) {
            if(bootmenu_recovery_self_test())bootmenu_draw(selection,255);
        } else if (key.ScanCode == 21 &&
                   bootmenu_view()==NOVA_VIEW_DIAGNOSTICS) {
            if(nova_sw_renderer_report_failure(false)==NOVA_SW_RENDERER_RECOVERED){
                nova_debug_string("UEFI:SOFTWARE-RENDERER-RESET\n");
                bootmenu_set_status("Software-Renderer wurde sicher neu initialisiert.");
                bootmenu_draw(selection,255);
                nova_debug_string("UEFI:SOFTWARE-RENDERER-RECOVERED\n");
            }
        } else if (key.ScanCode == 22 &&
                   bootmenu_view()==NOVA_VIEW_DIAGNOSTICS) {
            const nova_navigation_entry_t *current=nova_navigation_current();
            nova_dialog_t *credential=nova_dialog_open(NOVA_DIALOG_CREDENTIAL,
                "Sichere Eingabe","Diagnose des geschuetzten Passwortfeldes.",
                true,true,current?current->focus_id:0);
            if(credential&&nova_dialog_credential_configure(credential,NOVA_CREDENTIAL_PASSWORD)&&
               nova_dialog_add_button(credential,"Bestaetigen",NOVA_DIALOG_RESULT_OK,false)&&
               nova_dialog_add_button(credential,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false)){
                nova_debug_string("UEFI:PASSWORD-FIELD-DIALOG-OPEN\n");
                animate_dialog_motion(true,selection);bootmenu_draw(selection,255);
            }
        } else if (key.ScanCode == 11) {
            nova_debug_string("UEFI:HELP-VIEW\n");
            navigate_to(NOVA_VIEW_HELP, &selection, NOVA_NAV_PUSH);
            bootmenu_set_status("Kontext: aktueller Bootmanager-Eintrag");
            bootmenu_draw(selection, 255);
        } else if (key.ScanCode == 12) {
            bootmenu_context_open(selection);
            bootmenu_draw(selection,255);
        }
        do {
            system_table->BootServices->Stall(5000);
            pointer_frame_elapsed_ms+=5;
            device_scan_elapsed_ms+=5;
            if(device_scan_elapsed_ms>=250u){
                bool was_available=pointer_available;
                if(!nova_input_device_scan())return EFI_SUCCESS;
                pointer_available=uefi_pointer_available();device_scan_elapsed_ms=0;
                if(was_available&&!pointer_available){pending_pointer=false;
                    pending_pointer_dx=pending_pointer_dy=pending_pointer_wheel=0;
                    pending_pointer_absolute=false;
                    pending_left=pending_right=dispatched_left=dispatched_right=false;}
            }
            if(bootmenu_tick(5))bootmenu_draw(selection,255);
            key = read_key(system_table);
            if(key.ScanCode||key.UnicodeChar)bootmenu_tooltip_hide();
            if (pointer_available) {
                uefi_pointer_event_t event;bool activate=false;
                if(uefi_pointer_poll(nova_graphics_width(),nova_graphics_height(),&event)){
                    if(event.absolute){pending_pointer_x=event.x;pending_pointer_y=event.y;
                        pending_pointer_absolute=true;}
                    else if(!pending_pointer_absolute){pending_pointer_dx+=event.dx;
                        pending_pointer_dy+=event.dy;}
                    pending_pointer_wheel+=event.wheel;
                    pending_left=event.left;pending_right=event.right;pending_pointer=true;
                }
                bool pointer_edge=pending_pointer&&
                    (pending_left!=dispatched_left||pending_right!=dispatched_right);
                if(pending_pointer&&(pointer_frame_elapsed_ms>=15u||pointer_edge||
                                    pending_pointer_wheel)){
                    bool visual=bootmenu_scroll_wheel(pending_pointer_wheel,&selection);
                    visual=(pending_pointer_absolute?
                        bootmenu_pointer_absolute_event(pending_pointer_x,pending_pointer_y,
                            pending_left,pending_right,&selection,&activate):
                        bootmenu_pointer_event(pending_pointer_dx,pending_pointer_dy,
                            pending_left,pending_right,&selection,&activate))||visual;
                    dispatched_left=pending_left;dispatched_right=pending_right;
                    pending_pointer_dx=pending_pointer_dy=pending_pointer_wheel=0;
                    pending_pointer=false;pending_pointer_absolute=false;
                    pointer_frame_elapsed_ms=0;
                    if(visual)bootmenu_draw(selection,255);
                    nova_debug_string("UEFI:POINTER-FRAME-COALESCED\n");
                    if(bootmenu_breadcrumb_take_root_request()){
                        navigate_root(&selection);
                        continue;
                    }
                    if (activate) {
                        if (nova_dialog_active()) {
                            nova_dialog_result_t result = NOVA_DIALOG_RESULT_NONE;
                            if (close_dialog_animated(false,selection,&result))
                                handle_dialog_result(result);
                        } else if (bootmenu_context_active()) {
                            handle_context_action(selection);
                        } else if (handle_action(&selection)) return EFI_SUCCESS;
                        bootmenu_draw(selection, 255);
                        if(nova_dialog_active())nova_debug_string("UEFI:DIALOG-STABLE\n");
                    }
                }
            }
        } while (!key.ScanCode && !key.UnicodeChar);
    }
}
