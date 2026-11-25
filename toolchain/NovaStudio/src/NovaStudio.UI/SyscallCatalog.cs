namespace NovaStudio.UI;

/// <summary>
/// Ein echter NovaOS-Syscall (siehe kernel/newkernel/syscall.h) - Service-/
/// Operation-Nummern 1:1 aus dem Kernel-Header uebernommen.
///
/// IsSimple unterscheidet, ob ein automatisch einfuegbares, ECHT
/// lauffaehiges NovaLang-Codebeispiel moeglich ist: einige Operationen
/// (SYSCALL_CORE_TOGGLE_*/_GET_*_VISIBLE/_SHUTDOWN/_READY/_YIELD) nehmen
/// laut syscall.c nachweislich KEINE Argument-Struktur entgegen (nur die
/// vier rohen SysCall(...)-Skalare, exakt wie im echten Beispielprogramm
/// novalang/examples/hello_syscall.nova). Alle anderen (GUI/VFS/INPUT und
/// die komplexeren CORE-Operationen) erwarten einen Zeiger auf eine
/// Argument-Struktur im Syscall-Pointer-Fenster [0x400000,0x404000) -
/// deren exaktes Byte-Layout pro Operation hier NICHT automatisch
/// nachgebaut wird (das waere erfundener, ungeprueft funktionierender
/// Code - verstoesst gegen die Projektregel "keine Dummy-Funktionen").
/// Fuer diese wird nur ein ehrlicher Erklaerungs-Kommentar eingefuegt.
/// </summary>
public sealed record SyscallInfo(string Group, string Name, uint Service, uint Operation, bool IsSimple, string Description);

public static class SyscallCatalog
{
    public static readonly SyscallInfo[] All =
    {
        // ---- SYSCALL_SERVICE_CORE = 1 ----
        new("Core", "CORE_READY", 1, 2, true, "Meldet dem Kernel, dass der Ring-3-Prozess bereit ist (einmalig beim Start)"),
        new("Core", "CORE_YIELD", 1, 13, true, "Gibt die restliche Zeitscheibe an den Scheduler ab"),
        new("Core", "CORE_SHUTDOWN", 1, 7, true, "Fährt das System geordnet herunter (braucht SECURITY_CAP_POWER_SHUTDOWN + Shutdown-Capability-Vergabe)"),
        new("Core", "CORE_TOGGLE_START_MENU", 1, 5, true, "Schaltet das Startmenü um (braucht SECURITY_CAP_DISPLAY_SYSTEM_UI)"),
        new("Core", "CORE_GET_START_MENU_VISIBLE", 1, 6, true, "Liefert, ob das Startmenü sichtbar ist"),
        new("Core", "CORE_TOGGLE_EXPLORER", 1, 8, true, "Schaltet den Explorer um (braucht SECURITY_CAP_DISPLAY_SYSTEM_UI)"),
        new("Core", "CORE_GET_EXPLORER_VISIBLE", 1, 9, true, "Liefert, ob der Explorer sichtbar ist"),
        new("Core", "CORE_TOGGLE_SETTINGS", 1, 10, true, "Schaltet die Einstellungen um (braucht SECURITY_CAP_DISPLAY_SYSTEM_UI)"),
        new("Core", "CORE_GET_SETTINGS_VISIBLE", 1, 11, true, "Liefert, ob die Einstellungen sichtbar sind"),
        new("Core", "CORE_TOGGLE_AUTOHIDE", 1, 15, true, "Schaltet Taskbar-Auto-Hide um (braucht SECURITY_CAP_DISPLAY_SYSTEM_UI)"),
        new("Core", "CORE_GET_AUTOHIDE_ENABLED", 1, 16, true, "Liefert, ob Taskbar-Auto-Hide aktiv ist"),
        new("Core", "CORE_EXIT", 1, 1, false, "Beendet den Prozess - NUR für den Ring-3-Bootstrap gedacht, siehe syscall.c:825, nicht für normale Programme"),
        new("Core", "CORE_GET_DISPLAY_INFO", 1, 3, false, "Liefert Anzeige-Informationen in eine Argument-Struktur"),
        new("Core", "CORE_GET_TIME", 1, 4, false, "Liefert die Uhrzeit in eine Argument-Struktur"),
        new("Core", "CORE_GET_SYSTEM_INFO", 1, 12, false, "Liefert Systeminformationen in eine Argument-Struktur"),
        new("Core", "CORE_NOTIFY", 1, 14, false, "Zeigt eine Benachrichtigung (fester Text-Enum als Argument)"),
        new("Core", "CORE_DEBUG_WRITE", 1, 17, false, "Schreibt eine Debug-Zeile (Zeiger auf Puffer als Argument)"),
        new("Core", "CORE_SPAWN", 1, 18, false, "Startet einen neuen Prozess aus einer NEF-Datei"),

        // ---- SYSCALL_SERVICE_GUI = 2 ----
        new("GUI", "GUI_OP_CREATE_SURFACE", 2, 1, false, "Erzeugt eine Fensteroberfläche"),
        new("GUI", "GUI_OP_ATTACH_BUFFER", 2, 2, false, "Hängt einen Pixelpuffer an eine Oberfläche"),
        new("GUI", "GUI_OP_COMMIT", 2, 3, false, "Übergibt eine Oberfläche zur Anzeige"),
        new("GUI", "GUI_OP_DESTROY", 2, 4, false, "Zerstört eine Oberfläche"),
        new("GUI", "GUI_OP_WINDOW_SET_STATE", 2, 5, false, "Setzt Fensterzustand (minimiert/maximiert/Vollbild/wiederherstellen)"),
        new("GUI", "GUI_OP_HIDE", 2, 6, false, "Blendet eine Oberfläche aus"),
        new("GUI", "GUI_OP_MOVE", 2, 7, false, "Verschiebt ein Fenster"),
        new("GUI", "GUI_OP_GET_RECT", 2, 8, false, "Liefert die Fenstergeometrie"),
        new("GUI", "GUI_OP_SET_APPEARANCE", 2, 9, false, "Ändert das Erscheinungsbild einer Oberfläche"),

        // ---- SYSCALL_SERVICE_VFS = 3 ----
        new("VFS", "VFS_OP_LOOKUP", 3, 1, false, "Löst einen Dateipfad auf (braucht SECURITY_CAP_FS_READ/_WRITE)"),
        new("VFS", "VFS_OP_READ", 3, 2, false, "Liest aus einer Datei (braucht SECURITY_CAP_FS_READ)"),
        new("VFS", "VFS_OP_WRITE", 3, 3, false, "Schreibt in eine Datei (braucht SECURITY_CAP_FS_WRITE)"),
        new("VFS", "VFS_OP_CREATE", 3, 4, false, "Erstellt eine Datei (braucht SECURITY_CAP_FS_WRITE)"),
        new("VFS", "VFS_OP_READ_DIRECTORY", 3, 5, false, "Listet ein Verzeichnis"),
        new("VFS", "VFS_OP_QUERY", 3, 6, false, "Fragt Dateimetadaten ab"),
        new("VFS", "VFS_OP_DELETE", 3, 7, false, "Löscht eine Datei (braucht SECURITY_CAP_FS_WRITE)"),
        new("VFS", "VFS_OP_RENAME", 3, 8, false, "Benennt eine Datei um"),
        new("VFS", "VFS_OP_CLOSE", 3, 9, false, "Schließt einen Datei-Handle"),

        // ---- SYSCALL_SERVICE_INPUT = 4 ----
        new("Input", "INPUT_OP_POLL", 4, 1, false, "Fragt ein Tastatur-/Eingabeereignis ab"),
    };
}
