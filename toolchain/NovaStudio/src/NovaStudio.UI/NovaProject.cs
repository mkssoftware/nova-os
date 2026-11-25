using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;

namespace NovaStudio.UI;

/// <summary>
/// Minimales, echtes Projektformat (.nproj) - KEIN Mehrprojekt-/Mehrdatei-
/// Linken, weil novac.exe nur eine .nova-Datei pro Aufruf uebersetzt (siehe
/// novalang/compiler/src/main.c). Ein ".nproj" haelt daher nur: einen Namen
/// und den Pfad zur Hauptdatei (relativ zum Projektordner), die beim
/// "Erstellen"-Befehl uebersetzt wird. Dient bewusst auch als "Projektmappe"
/// (Abschnitt 6 spricht von .nsln) - ein echtes Mehrprojekt-Format waere
/// aktuell reine Attrappe, da die Toolchain es nicht einlesen koennte.
/// </summary>
public sealed class NovaProject
{
    public string Name { get; set; } = "Unbenannt";
    public string MainFile { get; set; } = "main.nova";

    /// <summary>
    /// Namen (SECURITY_CAP_*, siehe CapabilityCatalog.cs) der Capabilities,
    /// die beim naechsten "Erstellen" echt als NEF-CAPABILITIES-Section
    /// (nef.h: NEF_SECTION_CAPABILITIES, 4-Byte-Bitmaske) in die erzeugte
    /// .nef-Datei eingetragen werden - siehe OnBuildClick in
    /// MainWindow.xaml.cs.
    /// </summary>
    public List<string> RequestedCapabilities { get; set; } = new();

    /// <summary>Pfad der .nproj-Datei selbst (nicht mit-serialisiert).</summary>
    [System.Text.Json.Serialization.JsonIgnore]
    public string ProjectFilePath { get; set; } = "";

    [System.Text.Json.Serialization.JsonIgnore]
    public string ProjectDirectory => Path.GetDirectoryName(ProjectFilePath) ?? ".";

    [System.Text.Json.Serialization.JsonIgnore]
    public string MainFileFullPath => Path.Combine(ProjectDirectory, MainFile);

    public static NovaProject Load(string path)
    {
        var json = File.ReadAllText(path);
        var project = JsonSerializer.Deserialize<NovaProject>(json) ?? new NovaProject();
        project.ProjectFilePath = path;
        return project;
    }

    public void Save()
    {
        var json = JsonSerializer.Serialize(new { Name, MainFile, RequestedCapabilities },
            new JsonSerializerOptions { WriteIndented = true });
        File.WriteAllText(ProjectFilePath, json);
    }

    /// <summary>ODER-Verknüpfung aller angeforderten Capability-Bitmasken
    /// (0, wenn keine angefordert wurden - dann wird beim Build keine
    /// CAPABILITIES-Section erzeugt).</summary>
    public uint ComputeCapabilityBitmask() =>
        CapabilityCatalog.All
            .Where(c => RequestedCapabilities.Contains(c.Name))
            .Aggregate(0u, (mask, c) => mask | c.Bitmask);

    /// <summary>
    /// Legt ein neues Projekt im angegebenen (bereits existierenden) Ordner
    /// an: schreibt die .nproj-Datei und - falls die Hauptdatei noch nicht
    /// existiert - ein echtes, minimales NovaLang-Programm (kein leerer
    /// Dummy, sondern ein tatsaechlich compilierbares "Hello"-Programm).
    /// </summary>
    public static NovaProject CreateNew(string projectFilePath, string projectName)
    {
        var project = new NovaProject { Name = projectName, MainFile = "main.nova", ProjectFilePath = projectFilePath };
        project.Save();

        string mainPath = project.MainFileFullPath;
        if (!File.Exists(mainPath))
        {
            // Syntax exakt gegen novalang/examples/hello_nova.nova verifiziert
            // (echter novac.exe-Lauf, siehe IMPLEMENTATION_STATUS.md) - vorher
            // generierte "WriteLine ..." ohne Console-Praefix/Klammern wurde
            // vom echten Compiler mit "Anweisung erwartet" abgelehnt.
            File.WriteAllText(mainPath,
                "Module " + projectName + "\n" +
                "    Function Main() As Int32\n" +
                "        Console.WriteLine(\"Hallo von " + projectName + "\")\n" +
                "        Return 0\n" +
                "    End Function\n" +
                "End Module\n");
        }
        return project;
    }
}
