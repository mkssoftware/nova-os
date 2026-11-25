namespace NovaStudio.UI;

/// <summary>
/// Eine echte NovaOS-Capability (SECURITY_CAP_*, siehe kernel/newkernel/
/// security.h) - Werte 1:1 aus dem Kernel-Header uebernommen, nicht
/// erfunden.
/// </summary>
public sealed record CapabilityInfo(string Name, uint Bitmask, string Description);

public static class CapabilityCatalog
{
    // Werte exakt aus kernel/newkernel/security.h abgeschrieben.
    public static readonly CapabilityInfo[] All =
    {
        new("SECURITY_CAP_MEMORY", 0x00000001u, "Speicherverwaltung (low-level)"),
        new("SECURITY_CAP_IO", 0x00000002u, "Roher I/O-Port-Zugriff"),
        new("SECURITY_CAP_SERVICE", 0x00000004u, "Systemdienst-Betrieb"),
        new("SECURITY_CAP_ADMIN", 0x00000008u, "Administrative Rechte"),
        new("SECURITY_CAP_IPC", 0x00000010u, "Inter-Prozess-Kommunikation"),
        new("SECURITY_CAP_POWER_QUERY", 0x00000020u, "Energiestatus abfragen"),
        new("SECURITY_CAP_POWER_WAKE", 0x00000040u, "Wake-Events verwalten"),
        new("SECURITY_CAP_POWER_SHUTDOWN", 0x00000080u, "System herunterfahren (SYSCALL_CORE_SHUTDOWN)"),
        new("SECURITY_CAP_POWER_PROFILE", 0x00000100u, "Energieprofil ändern"),
        new("SECURITY_CAP_NET_QUERY", 0x00000200u, "Netzwerkstatus abfragen"),
        new("SECURITY_CAP_NET_RAW", 0x00000400u, "Roher Netzwerkzugriff"),
        new("SECURITY_CAP_NET_CONNECT", 0x00000800u, "Netzwerkverbindungen aufbauen"),
        new("SECURITY_CAP_NET_LISTEN", 0x00001000u, "Netzwerkverbindungen annehmen"),
        new("SECURITY_CAP_LOG_READ", 0x00002000u, "Systemprotokolle lesen"),
        new("SECURITY_CAP_DISPLAY_SYSTEM_UI", 0x00004000u, "Taskbar/Startmenü/Explorer/Einstellungen umschalten (SYSCALL_CORE_TOGGLE_*)"),
        new("SECURITY_CAP_BOOT_HEALTH_REPORT", 0x00008000u, "Boot-Zustand melden"),
        new("SECURITY_CAP_BOOT_HEALTH_COMMIT", 0x00010000u, "Boot-Zustand festschreiben"),
        new("SECURITY_CAP_FS_READ", 0x00020000u, "Dateien lesen (SYSCALL_SERVICE_VFS, Lesevorgänge)"),
        new("SECURITY_CAP_FS_WRITE", 0x00040000u, "Dateien schreiben (SYSCALL_SERVICE_VFS, Schreibvorgänge)"),
        new("SECURITY_CAP_FS_SYSTEM_WRITE", 0x00080000u, "An der NovaFS-Wurzel schreiben"),
    };
}
