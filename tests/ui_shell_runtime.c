#include "nova/ui/shell.h"

#include <stdio.h>

static int failures;

static void expect(bool condition, const char *message) {
    if (condition) printf("[OK] %s\n", message);
    else {
        fprintf(stderr, "[FAIL] %s\n", message);
        ++failures;
    }
}

int main(void) {
    nova_shell_t shell;
    nova_ui_window_id_t explorer;
    nova_ui_window_id_t sheet;
    nova_ui_window_id_t studio;
    nova_ui_rect_t restore;
    const nova_shell_window_t *window;
    nova_ui_color_t acrylic;

    nova_shell_init(&shell, 1920, 1080);
    acrylic = nova_design_color(&shell.design, NOVA_DESIGN_ACRYLIC);
    expect(nova_design_system_valid(&shell.design) && acrylic.alpha < 255u &&
               shell.design.radii[NOVA_DESIGN_RADIUS_START_MENU] == 28,
           "gemeinsame Aurora-/Acrylic-Designtokens sind gültig");
    expect(shell.layout.taskbar.y + shell.layout.taskbar.height < 1080 &&
               shell.layout.start_menu.y + shell.layout.start_menu.height <
                   shell.layout.taskbar.y &&
               shell.layout.command_bar.x > shell.layout.branding.x,
           "Desktop-Shell ordnet Branding, Befehlsleiste, Startmenü und Taskleiste");
    expect(shell.pinned_count == 7u &&
               shell.pinned[0] == NOVA_SHELL_APP_EXPLORER &&
               shell.pinned[5] == NOVA_SHELL_APP_SHEET,
           "Nova-Taskleiste besitzt die definierten angehefteten Apps");

    nova_shell_toggle_start(&shell);
    nova_shell_tick(&shell, shell.design.transition_normal_ms);
    expect(shell.start_menu_open && shell.start_menu_progress_milli == 1000u,
           "Startmenü öffnet mit begrenzter 190-ms-Transition");
    nova_shell_click_outside(&shell);
    nova_shell_tick(&shell, shell.design.transition_normal_ms);
    expect(!shell.start_menu_open && shell.start_menu_progress_milli == 0u,
           "Klick außerhalb schließt das Startmenü");
    expect(nova_shell_handle_key(&shell, 'K', true, true) == NOVA_UI_OK &&
               shell.command_palette_open && shell.command_palette_focused,
           "Ctrl+K fokussiert die globale Befehlsleiste");
    expect(nova_shell_handle_key(&shell, 27u, false, true) == NOVA_UI_OK &&
               !shell.command_palette_open,
           "Escape schließt die globale Befehlsleiste");

    expect(nova_shell_launch(&shell, NOVA_SHELL_APP_EXPLORER, &explorer) ==
                   NOVA_UI_OK &&
               nova_shell_launch(&shell, NOVA_SHELL_APP_SHEET, &sheet) ==
                   NOVA_UI_OK &&
               nova_shell_launch(&shell, NOVA_SHELL_APP_CAPABILITIES_STUDIO,
                                 &studio) == NOVA_UI_OK &&
               shell.window_count == 3u && shell.active_window.value == studio.value,
           "Explorer, Nova Sheet und Fähigkeiten Studio öffnen als echte Fensterzustände");
    window = nova_shell_find_window(&shell, explorer);
    restore = window->bounds;
    expect(nova_shell_window_command(&shell, explorer,
                                     NOVA_SHELL_WINDOW_MAXIMIZE) == NOVA_UI_OK &&
               nova_shell_find_window(&shell, explorer)->state ==
                   NOVA_UI_WINDOW_MAXIMIZED &&
               nova_shell_find_window(&shell, explorer)->bounds.width ==
                   shell.layout.work_area.width,
           "Fenster maximiert in die reservierte Arbeitsfläche");
    expect(nova_shell_window_command(&shell, explorer,
                                     NOVA_SHELL_WINDOW_RESTORE) == NOVA_UI_OK &&
               nova_shell_find_window(&shell, explorer)->bounds.x == restore.x,
           "Fenster stellt seine vorherige Geometrie wieder her");
    expect(nova_shell_window_move(&shell, explorer, -500, -500) == NOVA_UI_OK &&
               nova_shell_find_window(&shell, explorer)->bounds.x ==
                   shell.layout.work_area.x &&
               nova_shell_find_window(&shell, explorer)->bounds.y ==
                   shell.layout.work_area.y,
           "verschobene Fenster bleiben vollständig im Arbeitsbereich");
    expect(nova_shell_window_command(&shell, sheet,
                                     NOVA_SHELL_WINDOW_MINIMIZE) == NOVA_UI_OK &&
               !nova_shell_find_window(&shell, sheet)->visible &&
               nova_shell_launch(&shell, NOVA_SHELL_APP_SHEET, &sheet) ==
                   NOVA_UI_OK &&
               nova_shell_find_window(&shell, sheet)->visible,
           "Taskleistenaktivierung stellt minimierte Apps wieder her");
    expect(nova_shell_window_command(&shell, studio,
                                     NOVA_SHELL_WINDOW_CLOSE) == NOVA_UI_OK &&
               nova_shell_find_window(&shell, studio)->state == NOVA_UI_WINDOW_HIDDEN,
           "Fenster lassen sich kontrolliert schließen");
    expect(nova_shell_resize(&shell, 1280, 720) == NOVA_UI_OK &&
               shell.layout.start_menu.x >= 0 && shell.layout.start_menu.y >= 0 &&
               shell.layout.start_menu.x + shell.layout.start_menu.width <= 1280 &&
               shell.layout.start_menu.y + shell.layout.start_menu.height <= 720,
           "Startmenü bleibt bei der Zielauflösung 1280×720 vollständig sichtbar");

    if (failures != 0) {
        fprintf(stderr, "%d Shell-Tests fehlgeschlagen\n", failures);
        return 1;
    }
    puts("Nova Desktop Shell: alle Tests bestanden");
    return 0;
}
