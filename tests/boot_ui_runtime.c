#include <stdint.h>
#include <stdio.h>
#include "../boot/bootloader/bootmenu/motion.h"
#include "../boot/bootloader/bootmenu/compositor.h"
#include "../boot/bootloader/bootmenu/controls.h"
#include "../boot/bootloader/bootmenu/text.h"
#include "../boot/bootloader/bootmenu/unicode.h"
#include "../boot/bootloader/bootmenu/resources.h"
#include "../boot/bootloader/bootmenu/icons.h"
#include "../boot/bootloader/bootmenu/input.h"
#include "../boot/bootloader/bootmenu/diagnostics.h"
#include "../boot/bootloader/bootmenu/branding.h"
#include "../boot/bootloader/bootmenu/theme.h"
#include "../boot/bootloader/bootmenu/layout.h"
#include "../boot/bootloader/bootmenu/navigation.h"
#include "../boot/bootloader/bootmenu/dialog.h"
#include "../boot/bootloader/bootmenu/page.h"

static uint32_t writes;
static uint32_t captured[8][8];
void pixel_set(uint64_t x, uint64_t y, uint32_t color)
{
    if (x < 8 && y < 8) captured[y][x] = color;
    ++writes;
}

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

    nova_dialog_motion_t dialog = {0};
    failed |= check(nova_dialog_enter(&dialog), "Dialog Enter Motion");
    nova_motion_update(1200);
    failed |= check(dialog.opacity == 255 && dialog.visible, "Dialog eingeblendet");
    failed |= check(nova_dialog_exit(&dialog), "Dialog Exit Motion");
    nova_motion_update(1400);
    failed |= check(dialog.opacity == 0, "Dialog ausgeblendet");

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
    failed |= check(nova_compositor_compose() && writes != 0, "Damage compositing");
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
    failed |= check(captured[0][0] == 0xff800000u,
                    "kanalgetreues Alpha-Compositing");

    if (!failed) puts("BOOT_UI_RUNTIME_OK");
    return failed ? 1 : 0;
}
