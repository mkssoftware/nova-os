#include "uefi_min.h"
#include "../bootmenu/motion.h"
#include "../bootmenu/input.h"
#include "../bootmenu/diagnostics.h"
#include "../bootmenu/ui.h"
#include "../bootmenu/theme.h"
#include "../bootmenu/navigation.h"
#include "../bootmenu/dialog.h"
#include "firmware.h"

EFI_STATUS grafik_init(EFI_SYSTEM_TABLE *system_table);
bool uefi_pointer_initialize(EFI_SYSTEM_TABLE *system_table);
bool uefi_pointer_poll(int32_t *dx, int32_t *dy, int32_t *wheel,
                       bool *left, bool *right);
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
    if(entering)dialog_visual=(nova_dialog_motion_t){0};
    bool scheduled=entering?nova_dialog_enter(&dialog_visual):
                            nova_dialog_exit(&dialog_visual);
    uint32_t duration=nova_motion_is_reduced()?150u:180u;
    uint64_t start=nova_motion_diagnostics()->current_ms;
    nova_debug_string(entering?"UEFI:DIALOG-ENTER-START\n":"UEFI:DIALOG-EXIT-START\n");
    if(scheduled){
        uint32_t step=duration/2u;
        for(uint32_t elapsed=0;elapsed<=duration;elapsed+=step){
            nova_motion_update(start+elapsed);
            bootmenu_set_dialog_motion((uint8_t)dialog_visual.opacity,
                                       (uint16_t)dialog_visual.scale);
            bootmenu_draw(selection,255);
            runtime_system_table->BootServices->Stall(30000);
        }
        nova_motion_update(start+duration+1u);
    }
    if(entering){dialog_visual.opacity=255;dialog_visual.scale=1000;
        dialog_visual.focused=true;}
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
    uint32_t step=duration/2u;
    if (!step) step=1;
    for (uint32_t elapsed=0;elapsed<=duration;elapsed+=step) {
        nova_motion_update(start+elapsed);
        visual=nova_navigation_visual();
        bootmenu_set_transition(visual->offset_dlu,(uint8_t)visual->opacity,true);
        bootmenu_draw(entering ? (UINTN)-1 : selection,255);
        runtime_system_table->BootServices->Stall(30000);
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
    nova_dialog_t *dialog = nova_dialog_open(type, title, message, true, true,
                                             current ? current->focus_id : 0);
    if (dialog) {
        nova_dialog_add_button(dialog, "OK", NOVA_DIALOG_RESULT_OK, false);
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
        runtime_system_table->BootServices->Stall(150000);
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
    if (nova_input_next(&event)) {
        key.ScanCode = (uint16_t)event.key.key;
        key.UnicodeChar = (CHAR16)event.key.unicode;
    }
    return key;
}

static void boot_selected(UINTN selection)
{
    if (selection == 0) nova_debug_string("UEFI:START\n");
    else if (selection == 1) nova_debug_string("UEFI:INSTALL-UNAVAILABLE\n");
    else if (selection == 2) nova_debug_string("UEFI:SETTINGS\n");
    else if (selection == 3) nova_debug_string("UEFI:DIAGNOSTICS\n");
    else if (selection == 4) nova_debug_string("UEFI:RECOVERY\n");
    else nova_debug_string("UEFI:POWEROFF\n");
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
            nova_theme_id_t next = (nova_theme_id_t)((nova_theme_active() + 1) % NOVA_THEME_COUNT);
            if (nova_theme_activate(next)) {
                if (next == NOVA_THEME_LIGHT) {
                    nova_debug_string("UEFI:THEME-LIGHT\n");
                    bootmenu_set_status("Darstellung: Light Theme aktiv.");
                } else if (next == NOVA_THEME_HIGH_CONTRAST) {
                    nova_debug_string("UEFI:THEME-HIGH-CONTRAST\n");
                    bootmenu_set_status("Darstellung: Hoher Kontrast und reduzierte Effekte aktiv.");
                } else {
                    nova_debug_string("UEFI:THEME-DARK\n");
                    bootmenu_set_status("Darstellung: Dark Theme aktiv.");
                }
            }
        } else if (selection == 1) {
            nova_theme_set_reduced_motion(!nova_theme_reduced_motion());
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
            nova_dialog_t *dialog=nova_dialog_open(NOVA_DIALOG_CONFIRMATION,
                "Firmware-Setup öffnen",
                "Der Computer wird neu gestartet und öffnet danach die Firmware-Einstellungen.",
                true,true,(uint16_t)selection);
            if(dialog){
                nova_dialog_add_button(dialog,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false);
                nova_dialog_add_button(dialog,"Neu starten",NOVA_DIALOG_RESULT_YES,true);
                animate_dialog_motion(true,selection);
            }
        }else bootmenu_set_status("Firmwareinformationen werden ausschließlich lesend angezeigt.");
    } else if (view == NOVA_VIEW_DIAGNOSTICS) {
        if (selection == 5) navigate_back(selection_pointer);
        else if (selection == 0) bootmenu_set_status("Firmware, Grafik, Eingabe und Ressourcen: bereit.");
        else if (selection == 3) run_diagnostic_progress(selection);
        else bootmenu_set_status("Diagnosedaten werden ausschließlich lesend angezeigt.");
    } else if (view == NOVA_VIEW_RECOVERY) {
        if (selection == 5) navigate_back(selection_pointer);
        else show_notice(NOVA_DIALOG_WARNING, "Recovery nicht verfügbar",
                         "Für diese Aktion fehlt ein geprüftes, sicheres Schreib-Backend.");
    } else if (view == NOVA_VIEW_POWER) {
        if (selection == 5) {
            pending_power = (UINTN)-1;
            navigate_back(selection_pointer);
        } else if (selection == 0 || selection == 1) {
            pending_power = selection;
            nova_debug_string("UEFI:POWER-CONFIRM\n");
            nova_dialog_t *dialog = nova_dialog_open(NOVA_DIALOG_CONFIRMATION,
                selection == 0 ? "NovaOS herunterfahren" : "NovaOS neu starten",
                selection == 0 ? "Der Computer wird sicher ausgeschaltet." :
                                 "Der Computer wird sicher neu gestartet.",
                true, true, (uint16_t)selection);
            if (dialog) {
                nova_dialog_add_button(dialog,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false);
                nova_dialog_add_button(dialog,selection == 0 ? "Ausschalten" : "Neustarten",
                                       NOVA_DIALOG_RESULT_YES,true);
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
    if(action==0){
        nova_debug_string("UEFI:CONTEXT-DETAILS\n");
        show_notice(NOVA_DIALOG_INFORMATION,"Eintragsdetails",
            selection==0?"NovaOS wird mit der bevorzugten NKI-Datei gestartet.":
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
    runtime_system_table = system_table;
    nova_debug_string("UEFI:NOVA-ENTRY\n");
    if (!system_table || !system_table->BootServices ||
        EFI_ERROR(grafik_init(system_table))) {
        nova_debug_string("UEFI:GOP-UNAVAILABLE\n");
        return 1;
    }

    UINTN selection = 0;
    if (!bootmenu_initialize()) {
        nova_debug_string("UEFI:COMPOSITOR-UNAVAILABLE\n");
        return 1;
    }
    nova_navigation_initialize((nova_navigation_entry_t){NOVA_VIEW_MAIN,0,0,0,0});
    nova_dialog_initialize();
    nova_debug_string("UEFI:NAVIGATION-READY\n");
    nova_debug_string("UEFI:DIALOG-READY\n");
    nova_motion_initialize();
    uefi_power_initialize(system_table);
    uefi_firmware_initialize(system_table);
    bool pointer_available = uefi_pointer_initialize(system_table);
    int32_t entrance_opacity = 0;
    nova_animation_t entrance = {
        &entrance_opacity, 0, 255, 0, 0, 180, 1, 3, 0,
        NOVA_PROPERTY_OPACITY, NOVA_EASE_OUT_CUBIC, NOVA_MOTION_CREATED,
        false, false, true
    };
    if (!nova_motion_create(&entrance)) return 1;
    for (uint64_t elapsed = 0; elapsed <= 180; elapsed += 20) {
        nova_motion_update(elapsed);
        bootmenu_draw(selection, (uint8_t)entrance_opacity);
        system_table->BootServices->Stall(20000);
    }
    nova_debug_string("UEFI:MOTION-READY\n");
    nova_diag_frame(20000, 500, 500, 8000, 4000);
    nova_diag_snapshot();
    nova_debug_string("UEFI:DIAGNOSTICS-READY\n");

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
            int32_t dx, dy, wheel;
            bool left, right, activate = false;
            if (uefi_pointer_poll(&dx, &dy, &wheel, &left, &right)) {
                (void)wheel;
                pointer_activity = bootmenu_pointer_event(dx, dy, left,
                                                          right,
                                                          &selection, &activate);
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

    for (;;) {
        if (bootmenu_context_active()) {
            if(key.ScanCode==1)bootmenu_context_move(-1);
            else if(key.ScanCode==2)bootmenu_context_move(1);
            else if(key.UnicodeChar==13)handle_context_action(selection);
            else if(key.ScanCode==23)bootmenu_context_close();
            bootmenu_draw(selection,255);
        } else if (nova_dialog_active()) {
            nova_dialog_result_t result = NOVA_DIALOG_RESULT_NONE;
            if (key.ScanCode == 1) nova_dialog_focus_move(-1);
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
        } else if (key.ScanCode == 1) {
            selection = selection ? selection - 1 : (UINTN)bootmenu_item_count() - 1u;
            bootmenu_draw(selection, 255);
        } else if (key.ScanCode == 2) {
            selection = (selection + 1) % bootmenu_item_count();
            bootmenu_draw(selection, 255);
        } else if (key.UnicodeChar == 13 ||
                   (key.UnicodeChar==32&&bootmenu_view()==NOVA_VIEW_SETTINGS&&
                    (selection==2||selection==3))) {
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
            system_table->BootServices->Stall(20000);
            if(bootmenu_tick(20))bootmenu_draw(selection,255);
            key = read_key(system_table);
            if(key.ScanCode||key.UnicodeChar)bootmenu_tooltip_hide();
            if (pointer_available) {
                int32_t dx, dy, wheel;
                bool left, right, activate = false;
                if (uefi_pointer_poll(&dx, &dy, &wheel, &left, &right)) {
                    (void)wheel;
                    bootmenu_pointer_event(dx, dy, left, right, &selection, &activate);
                    bootmenu_draw(selection, 255);
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
