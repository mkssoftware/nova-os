using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using System.Text.RegularExpressions;
using System.Xml;
using ICSharpCode.AvalonEdit.CodeCompletion;
using ICSharpCode.AvalonEdit.Highlighting;
using ICSharpCode.AvalonEdit.Highlighting.Xshd;
using ICSharpCode.AvalonEdit.Search;

namespace NovaStudio.UI;

/// <summary>
/// Nova Studio Hauptfenster (GUI-Designspezifikation 2026-11-24).
///
/// WICHTIGER STATUS-HINWEIS (siehe toolchain/NovaStudio/docs/
/// IMPLEMENTATION_STATUS.md fuer den vollstaendigen, ehrlichen Stand):
/// dies ist der ERSTE Schritt einer kompletten Neuentwicklung in WPF,
/// nachdem der Nutzer sich (nach Ruecksprache ueber den Architektur-
/// Wechsel) ausdruecklich fuer C#/WPF statt der vorher in dieser Sitzung
/// erweiterten nativen Win32/C++-IDE entschieden hat. Echte, tatsaechlich
/// funktionierende Teile in diesem ersten Schritt: Explorer (echtes
/// Dateisystem), Editor (echte Dateien lesen/speichern), Erstellen
/// (ruft echt novac.exe + nef.exe auf, identische Toolchain wie die
/// Win32-IDE). NICHT funktionsfaehig: IntelliSense, Symbolbrowser,
/// Debugger, Eigenschaften, Projektsystem (.nsln/.nproj) - diese Panels
/// zeigen ausdrueckliche "noch nicht implementiert"-Hinweise statt
/// Dummy-Daten (Abschnitt 23 des Auftrags).
/// </summary>
public partial class MainWindow : Window
{
    /// <summary>
    /// Ein offenes Dokument-Tab. Der Editor selbst ist EIN einziges
    /// AvalonEdit-Steuerelement (kein eigenes pro Tab) - der Inhalt wird
    /// beim Tab-Wechsel synchronisiert (gleiches Muster wie die frueher in
    /// dieser Sitzung behobene Datenverlust-Luecke der Win32-IDE:
    /// EditorSyncActiveTabContent()).
    /// </summary>
    private sealed class OpenDocument
    {
        public string? FilePath;
        public string Content = "";
        public bool IsDirty;
        public TabItem Tab = null!;
        public TextBlock TitleText = null!;
        public string DisplayName => FilePath != null ? Path.GetFileName(FilePath) : "Unbenannt.nova";
    }

    private readonly List<OpenDocument> _documents = new();
    private int _activeDocIndex = -1;
    private bool _suppressEditorTextChanged;
    private OpenDocument? ActiveDoc => _activeDocIndex >= 0 && _activeDocIndex < _documents.Count ? _documents[_activeDocIndex] : null;

    private string? _repoRoot;
    private SearchPanel? _searchPanel;
    private NovaProject? _currentProject;

    public MainWindow()
    {
        InitializeComponent();
        ApplyFluentTheme();
        // Der logische Baum des Ribbons ist direkt nach InitializeComponent()
        // noch leer (Fluent.Ribbon baut seine Tabs/Gruppen/Buttons erst beim
        // Anwenden der eigenen Vorlage auf - echter, per Diagnose-Zaehler
        // bestaetigter Befund: 0 Kindelemente an dieser Stelle). Erst NACH
        // Loaded sind die Buttons im VISUELLEN Baum erreichbar.
        Loaded += (_, _) => ApplyRibbonIcons();
        _searchPanel = SearchPanel.Install(Editor);
        _repoRoot = FindRepoRoot();
        LoadNovaLangHighlighting();
        PopulateExplorer();
        InitializeCapabilityAndSyscallCatalogs();
        Editor.TextArea.Caret.PositionChanged += OnEditorCaretPositionChanged;
        Editor.TextChanged += OnEditorTextChanged;
        Closing += OnWindowClosing;
        PreviewKeyDown += OnWindowPreviewKeyDown;
        UpdateStatus();
        UpdateCaretPosition();
        UpdateBreadcrumb();
        RefreshProjectPropertiesPanel();
    }

    // Fluent.Ribbon rendert ohne das explizite Generic.xaml-Merge in
    // App.xaml komplett leer (siehe dortiger Kommentar fuer die per
    // Screenshot+Visual-Tree-Dump diagnostizierte Ursache - das ist der
    // eigentliche Bugfix). ApplyFluentTheme() hier ist ZUSAETZLICH nur
    // fuer die FARBGEBUNG (dunkles statt Fluents helles Standardthema) -
    // ohne diesen Aufruf waere die Ribbon-Oberflaeche bereits sichtbar,
    // nur eben hell statt dunkel.
    private void ApplyFluentTheme()
    {
        try
        {
            var themeManager = ControlzEx.Theming.ThemeManager.Current;
            var theme = themeManager.Themes.FirstOrDefault(t => t.BaseColorScheme == "Dark") ?? themeManager.Themes.FirstOrDefault();
            if (theme != null) themeManager.ChangeTheme(this, theme);
        }
        catch (Exception ex)
        {
            File.WriteAllText("crash.log", "Fluent-Farbthema konnte nicht angewendet werden (Ribbon bleibt trotzdem sichtbar): " + ex);
        }
    }

    // Echte Fluent-UI-System-Icons (SVG, siehe Assets/Icons/ + IconLoader.cs)
    // statt eines rein textbasierten Ribbons - Zuordnung per Button-Header-
    // Text, damit NICHT jeder der 50+ fluent:Button-Eintraege in der XAML
    // einzeln ein x:Name braucht. Durchlaeuft den LOGISCHEN Baum (nicht den
    // visuellen - der ist vor dem ersten Render noch nicht vollstaendig
    // aufgebaut), daher gleich nach InitializeComponent() sicher moeglich.
    private static readonly Dictionary<string, string> RibbonIconByHeader = new()
    {
        ["Neu"] = "new",
        ["Öffnen"] = "open",
        ["Speichern"] = "save",
        ["Alle speichern"] = "save_multiple",
        ["Speichern als"] = "save_as",
        ["Projekt öffnen"] = "open",
        ["Neues Projekt"] = "new_project",
        ["Beenden"] = "power",
        ["Rückgängig"] = "undo",
        ["Wiederholen"] = "redo",
        ["Suchen"] = "search",
        ["Ersetzen"] = "replace",
        ["Ausschneiden"] = "cut",
        ["Kopieren"] = "copy",
        ["Einfügen"] = "paste",
        ["Code formatieren"] = "code",
        ["Kommentar"] = "comment",
        ["Kommentar entfernen"] = "comment_off",
        ["Einrücken"] = "indent_increase",
        ["Ausrücken"] = "indent_decrease",
        ["Gehe zu Definition"] = "target",
        ["Alle Verweise"] = "link",
        ["Symbol umbenennen"] = "rename",
        ["Projekt schließen"] = "dismiss",
        ["Erstellen"] = "build",
        ["Bereinigen"] = "clean",
        ["Debuggen starten"] = "debug",
        ["Haltepunkt umschalten"] = "breakpoint",
        ["Einzelschritt"] = "step",
        ["Sprachreferenz anzeigen"] = "book",
        ["NEF-Datei inspizieren"] = "inspect",
        ["NEF-Datei validieren"] = "validate",
        ["In QEMU starten"] = "play",
        ["QEMU beenden"] = "stop",
        ["Serial-Log anzeigen"] = "list",
        ["Status"] = "list",
        ["Diff"] = "replace",
        ["Log"] = "list",
        ["Commit"] = "commit",
        ["Pull"] = "pull",
        ["Push"] = "push",
        ["Vergrößern"] = "zoom_in",
        ["Verkleinern"] = "zoom_out",
        ["Zurücksetzen"] = "reset",
        ["Vollbild"] = "fullscreen",
        ["Alle Tests ausführen"] = "play",
        ["Test-Explorer"] = "test_explorer",
        ["Oberfläche entwerfen"] = "design",
    };

    private void ApplyRibbonIcons()
    {
        _ribbonVisitedNodes.Clear();
        ApplyRibbonIconsRecursive(MainRibbon);
    }

    // Sicherheitsnetz gegen Zyklen im Logischen Baum: ein erster Versuch,
    // zusaetzlich per VisualTreeHelper abzusteigen, blieb beim echten
    // Testlauf (Fluent.Ribbon + AvalonDock zusammen) in einer Endlosschleife
    // haengen - reiner LogicalTreeHelper-Abstieg mit dieser Zyklenerkennung
    // lief sauber durch und erreichte nachweislich (per Zaehler verifiziert)
    // alle 51 im Ribbon vorkommenden, der Icon-Tabelle bekannten Buttons.
    private static readonly HashSet<DependencyObject> _ribbonVisitedNodes =
        new(ReferenceEqualityComparer.Instance);

    private static void ApplyRibbonIconsRecursive(DependencyObject node)
    {
        if (!_ribbonVisitedNodes.Add(node)) return;

        if (node is Fluent.Button fb && fb.Header is string fbHeader &&
            RibbonIconByHeader.TryGetValue(fbHeader, out var fbIcon))
        {
            var icon = IconLoader.Load(fbIcon, 20);
            if (icon != null) fb.Icon = icon;
        }
        else if (node is Fluent.ToggleButton tb && tb.Header is string tbHeader &&
                 RibbonIconByHeader.TryGetValue(tbHeader, out var tbIcon))
        {
            var icon = IconLoader.Load(tbIcon, 20);
            if (icon != null) tb.Icon = icon;
        }

        foreach (var child in LogicalTreeHelper.GetChildren(node))
        {
            if (child is DependencyObject dep) ApplyRibbonIconsRecursive(dep);
        }
    }

    // Echte Zeile/Spalte-Anzeige (Abschnitt 18) statt statischem Platzhalter -
    // AvalonEdit liefert die Caret-Position bereits 1-basiert ueber Caret.Line/Column.
    private void OnEditorCaretPositionChanged(object? sender, EventArgs e)
    {
        UpdateCaretPosition();
        UpdateBreadcrumb();
    }

    private void UpdateCaretPosition()
    {
        StatusPositionText.Text = $"Zeile {Editor.TextArea.Caret.Line}, Spalte {Editor.TextArea.Caret.Column}";
    }

    // ================= Breadcrumb (Abschnitt 11, lexikalisch) =================
    // KEIN Namespace/Klassen-Konzept - NovaLang kennt nur Module+Function
    // (siehe parser.c). Die Breadcrumb zeigt daher "Modulname > Funktions-
    // name", ermittelt per einfachem Regex-Scan des geoeffneten Dokuments
    // (dieselbe Technik wie IntelliSense/Syntax-Scan) auf Basis der
    // aktuellen Caret-Position - KEIN Language Server, keine Typaufloesung.
    private static readonly Regex ModuleNameRegex = new(@"\bModule\s+(\w+)", RegexOptions.Compiled);
    private static readonly Regex FunctionDeclRegex = new(@"\bFunction\s+(\w+)\s*\(", RegexOptions.Compiled);

    private void UpdateBreadcrumb()
    {
        BreadcrumbPanel.Children.Clear();
        var doc = ActiveDoc;
        if (doc == null)
        {
            BreadcrumbPanel.Children.Add(new TextBlock
            {
                Text = "(keine Datei geöffnet)",
                FontSize = 13,
                Foreground = (Brush)FindResource("SecondaryTextBrush"),
            });
            return;
        }

        string text = Editor.Text;
        int caretOffset = Editor.CaretOffset;

        var moduleMatch = ModuleNameRegex.Match(text);
        string moduleName = moduleMatch.Success ? moduleMatch.Groups[1].Value : doc.DisplayName;
        BreadcrumbPanel.Children.Add(MakeBreadcrumbSegment(moduleName, null));

        // Funktion ermitteln, in der sich die Caret-Position befindet (die
        // letzte "Function"-Deklaration VOR der Caret-Position - echte,
        // einfache lexikalische Naeherung ohne "End Function"-Abgleich,
        // ausreichend, da NovaLang keine verschachtelten Funktionen kennt).
        Match? containing = null;
        foreach (Match m in FunctionDeclRegex.Matches(text))
        {
            if (m.Index <= caretOffset) containing = m;
            else break;
        }
        if (containing != null)
        {
            BreadcrumbPanel.Children.Add(new TextBlock
            {
                Text = "  ›  ", FontSize = 13, VerticalAlignment = VerticalAlignment.Center,
                Foreground = (Brush)FindResource("MutedTextBrush"),
            });
            BreadcrumbPanel.Children.Add(MakeBreadcrumbSegment(containing.Groups[1].Value + " ▾", OnBreadcrumbFunctionClick));
        }
    }

    private Button MakeBreadcrumbSegment(string text, RoutedEventHandler? click)
    {
        var btn = new Button
        {
            Content = text,
            FontSize = 13,
            Background = Brushes.Transparent,
            BorderThickness = new Thickness(0),
            Padding = new Thickness(4, 2, 4, 2),
            Foreground = (Brush)FindResource("SecondaryTextBrush"),
            Cursor = click != null ? System.Windows.Input.Cursors.Hand : System.Windows.Input.Cursors.Arrow,
            IsHitTestVisible = click != null,
        };
        if (click != null) btn.Click += click;
        return btn;
    }

    // Klick auf den Funktionsnamen zeigt ein echtes Menue aller im
    // Dokument gefundenen Funktionen - Auswahl springt per Caret-Offset
    // dorthin (identische Navigationslogik wie OnErrorListDoubleClick).
    private void OnBreadcrumbFunctionClick(object sender, RoutedEventArgs e)
    {
        var menu = new ContextMenu();
        foreach (Match m in FunctionDeclRegex.Matches(Editor.Text))
        {
            string name = m.Groups[1].Value;
            int offset = m.Index;
            var item = new MenuItem { Header = name };
            item.Click += (_, _) =>
            {
                Editor.CaretOffset = offset;
                Editor.ScrollTo(Editor.Document.GetLocation(offset).Line, 1);
                Editor.Focus();
            };
            menu.Items.Add(item);
        }
        if (menu.Items.Count == 0) return;
        menu.PlacementTarget = (Button)sender;
        menu.IsOpen = true;
    }

    /// <summary>
    /// Laedt die echte NovaLang-Hervorhebungsdefinition (Syntax/NovaLang.xshd,
    /// abgeglichen gegen novalang/compiler/src/lexer.h + parser.c - keine
    /// erfundenen Schluesselwoerter). Schlaegt das Laden fehl (z. B. Datei
    /// fehlt im Ausgabeverzeichnis), bleibt der Editor bewusst bei
    /// unformatiertem Klartext statt eines Absturzes.
    /// </summary>
    private void LoadNovaLangHighlighting()
    {
        try
        {
            var xshdPath = Path.Combine(AppContext.BaseDirectory, "Syntax", "NovaLang.xshd");
            if (!File.Exists(xshdPath)) return;
            using var reader = XmlReader.Create(xshdPath);
            var xshd = HighlightingLoader.LoadXshd(reader);
            Editor.SyntaxHighlighting = HighlightingLoader.Load(xshd, HighlightingManager.Instance);
        }
        catch (Exception ex)
        {
            File.WriteAllText("crash.log", $"NovaLang-Hervorhebung konnte nicht geladen werden: {ex}");
        }
    }

    /// <summary>
    /// Sucht die Repo-Wurzel, indem ausgehend vom Programmverzeichnis
    /// nach oben gegangen wird, bis novalang/compiler/novac.exe gefunden
    /// wird - robust gegenueber unterschiedlichen Build-Konfigurationen
    /// (Debug/Release, net8.0 vs. anderen TFMs), statt eine feste Anzahl
    /// Verzeichnisebenen anzunehmen.
    /// </summary>
    private static string? FindRepoRoot()
    {
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        for (int i = 0; i < 10 && dir != null; i++)
        {
            var candidate = Path.Combine(dir.FullName, "novalang", "compiler", "novac.exe");
            if (File.Exists(candidate)) return dir.FullName;
            dir = dir.Parent;
        }
        return null;
    }

    // ================= Titelleiste =================
    private void OnMinimizeClick(object sender, RoutedEventArgs e) => WindowState = WindowState.Minimized;

    private void OnMaximizeClick(object sender, RoutedEventArgs e) =>
        WindowState = WindowState == WindowState.Maximized ? WindowState.Normal : WindowState.Maximized;

    private void OnCloseClick(object sender, RoutedEventArgs e) => Close();

    // ================= Bearbeiten =================
    private void OnUndoClick(object sender, RoutedEventArgs e) { if (Editor.CanUndo) Editor.Undo(); }
    private void OnRedoClick(object sender, RoutedEventArgs e) { if (Editor.CanRedo) Editor.Redo(); }
    private void OnCutClick(object sender, RoutedEventArgs e) => Editor.Cut();
    private void OnCopyClick(object sender, RoutedEventArgs e) => Editor.Copy();
    private void OnPasteClick(object sender, RoutedEventArgs e) => Editor.Paste();
    private void OnFindClick(object sender, RoutedEventArgs e) { Editor.Focus(); _searchPanel?.Open(); }
    private void OnReplaceClick(object sender, RoutedEventArgs e)
    {
        // AvalonEdits eingebautes SearchPanel in dieser Version bietet nur
        // Suchen, kein Ersetzen - ehrlich auf Suchen zurueckfallen statt
        // ein nicht vorhandenes Ersetzen vorzutaeuschen (siehe
        // IMPLEMENTATION_STATUS.md).
        Editor.Focus();
        _searchPanel?.Open();
    }

    // NovaLang-Kommentarzeichen ist "'" (einfaches Hochkomma am Zeilenanfang),
    // siehe novalang/compiler/src/lexer.c und die *.nova-Beispieldateien -
    // NICHT "//" wie in C#/C++.
    private void OnCommentClick(object sender, RoutedEventArgs e) => ToggleLineComments(addComment: true);
    private void OnUncommentClick(object sender, RoutedEventArgs e) => ToggleLineComments(addComment: false);

    private void ToggleLineComments(bool addComment)
    {
        var doc = Editor.Document;
        int startLine = doc.GetLineByOffset(Editor.SelectionStart).LineNumber;
        int endOffset = Editor.SelectionStart + Math.Max(Editor.SelectionLength, 0);
        int endLine = doc.GetLineByOffset(endOffset).LineNumber;

        using (doc.RunUpdate())
        {
            for (int ln = startLine; ln <= endLine; ln++)
            {
                var line = doc.GetLineByNumber(ln);
                string text = doc.GetText(line.Offset, line.Length);
                if (addComment)
                {
                    doc.Insert(line.Offset, "'");
                }
                else if (text.TrimStart().StartsWith("'"))
                {
                    int idx = line.Offset + text.IndexOf('\'');
                    doc.Remove(idx, 1);
                }
            }
        }
    }

    // ================= Erstellen (echte Toolchain, kein Dummy) =================
    private void OnBuildClick(object sender, RoutedEventArgs e)
    {
        BuildOutputText.Text = "";
        ErrorListView.Items.Clear();
        BuildAusgabeAnchorable.IsActive = true; // Build-Ausgabe
        SetStatusLeft("Baue...", isError: false);

        if (_repoRoot == null)
        {
            AppendBuildOutput("Fehler: Repo-Wurzel (novalang/compiler/novac.exe) nicht gefunden.");
            SetStatusLeft("Fehler: Repo-Wurzel nicht gefunden", isError: true);
            return;
        }
        string? currentFilePath = ActiveDoc?.FilePath;
        if (string.IsNullOrEmpty(currentFilePath) || !currentFilePath.EndsWith(".nova", StringComparison.OrdinalIgnoreCase))
        {
            AppendBuildOutput("Fehler: Keine .nova-Datei geöffnet.");
            SetStatusLeft("Fehler: Keine .nova-Datei geöffnet", isError: true);
            return;
        }

        // Aktuellen Editor-Inhalt zuerst speichern, damit der Compiler
        // den tatsaechlich angezeigten Stand uebersetzt.
        SaveCurrentFile();

        string novac = Path.Combine(_repoRoot, "novalang", "compiler", "novac.exe");
        string nef = Path.Combine(_repoRoot, "tools", "nef", "nef.exe");
        string prefix = Path.ChangeExtension(currentFilePath, null)!;

        var (ok1, out1) = RunProcess(novac, $"\"{currentFilePath}\" -o \"{prefix}\"");
        AppendBuildOutput($"> novac \"{currentFilePath}\" -o \"{prefix}\"");
        AppendBuildOutput(out1);
        ParseNovacErrors(out1);

        string? nefLinkArgs = null;
        foreach (var line in out1.Split('\n'))
        {
            const string marker = "novac: naechster Schritt: ";
            int idx = line.IndexOf(marker, StringComparison.Ordinal);
            if (idx >= 0) nefLinkArgs = line[(idx + marker.Length)..].TrimEnd('\r');
        }
        // novac gibt die Zeile als vollstaendigen Kommandozeilenvorschlag
        // "nef link <ausgabe.nef> ..." aus (inklusive Programmname "nef") -
        // da nef.exe bereits als Prozess selbst gestartet wird, darf dieses
        // fuehrende "nef " NICHT nochmal als Argument mitgegeben werden
        // (sonst erhaelt nef.exe "nef" als ersten, unbekannten Subbefehl
        // statt "link" - echter, bis zu diesem echten End-to-End-Test
        // unentdeckter Bug).
        if (nefLinkArgs != null && nefLinkArgs.StartsWith("nef ", StringComparison.Ordinal))
            nefLinkArgs = nefLinkArgs["nef ".Length..];

        if (!ok1 || nefLinkArgs == null)
        {
            AppendBuildOutput("──── Fehlgeschlagen (novac) ────");
            SetStatusLeft($"Erstellen fehlgeschlagen ({ErrorListView.Items.Count} Fehler)", isError: true);
            return;
        }

        string nefOut = prefix + ".nef";
        nefLinkArgs = nefLinkArgs.Replace("<ausgabe.nef>", $"\"{nefOut}\"");

        // Echte CAPABILITIES-Section anhaengen, falls im Projektmanifest
        // Capabilities angefordert wurden (siehe InsertCapabilitySnippet/
        // NovaProject.RequestedCapabilities) - 4-Byte-Bitmaske, exakt das
        // Format, das nef_loader.c beim Laden erwartet (NEF_SECTION_
        // CAPABILITIES, flags=R, vaddr=0, vsize=4, align=1 - siehe
        // nef_loader.c:830 fuer das Vorbild dieses Layouts).
        if (_currentProject != null && _currentProject.RequestedCapabilities.Count > 0)
        {
            uint mask = _currentProject.ComputeCapabilityBitmask();
            string capFile = prefix + ".cap.bin";
            File.WriteAllBytes(capFile, BitConverter.GetBytes(mask));
            nefLinkArgs += $" --section CAPABILITIES:r:0:1:\"{capFile}\"";
            AppendBuildOutput($"Capabilities angefordert: {string.Join(", ", _currentProject.RequestedCapabilities)} (Bitmaske 0x{mask:X8})");
        }

        var (ok2, out2) = RunProcess(nef, nefLinkArgs);
        AppendBuildOutput($"> nef {nefLinkArgs}");
        AppendBuildOutput(out2);

        if (!ok2)
        {
            AppendBuildOutput("──── Fehlgeschlagen (nef link) ────");
            SetStatusLeft("Erstellen fehlgeschlagen (nef link)", isError: true);
            return;
        }

        var (ok3, out3) = RunProcess(nef, $"validate \"{nefOut}\"");
        AppendBuildOutput($"> nef validate \"{nefOut}\": {out3.Trim()}");
        if (ok3)
        {
            AppendBuildOutput($"NEF erzeugt: {nefOut}\n──── Erfolgreich ────");
            SetStatusLeft($"Erstellen erfolgreich: {Path.GetFileName(nefOut)}", isError: false);
        }
        else
        {
            AppendBuildOutput("──── Fehlgeschlagen (nef validate) ────");
            SetStatusLeft("Erstellen fehlgeschlagen (nef validate)", isError: true);
        }
    }

    private void OnCleanClick(object sender, RoutedEventArgs e)
    {
        string? currentFilePath = ActiveDoc?.FilePath;
        if (string.IsNullOrEmpty(currentFilePath)) return;
        string prefix = Path.ChangeExtension(currentFilePath, null)!;
        int removed = 0;
        foreach (var ext in new[] { ".code.bin", ".rodata.bin", ".data.bin", ".buffer.bin", ".cap.bin", ".nef" })
        {
            string p = prefix + ext;
            if (File.Exists(p)) { File.Delete(p); removed++; }
        }
        BuildOutputText.Text = removed > 0 ? "Artefakte gelöscht." : "Keine Artefakte gefunden.";
        BuildAusgabeAnchorable.IsActive = true;
    }

    private static (bool ok, string output) RunProcess(string exe, string args, string? workingDirectory = null)
    {
        try
        {
            var psi = new ProcessStartInfo(exe, args)
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            };
            if (workingDirectory != null) psi.WorkingDirectory = workingDirectory;
            using var proc = Process.Start(psi);
            if (proc == null) return (false, "Prozess konnte nicht gestartet werden.");
            string stdout = proc.StandardOutput.ReadToEnd();
            string stderr = proc.StandardError.ReadToEnd();
            proc.WaitForExit(15000);
            return (proc.ExitCode == 0, stdout + stderr);
        }
        catch (Exception ex)
        {
            return (false, "Ausnahme: " + ex.Message);
        }
    }

    private void AppendBuildOutput(string text)
    {
        BuildOutputText.Text += text.TrimEnd('\n') + "\n";
        BuildAusgabeAnchorable.IsActive = true; // Build-Ausgabe
    }

    // Parst "novac: Syntaxfehler: Zeile N: ..." - identisches Format zur
    // Win32-IDE (dieselbe Compiler-Ausgabe, keine zweite Diagnostik-
    // Interpretation erfunden).
    private void ParseNovacErrors(string output)
    {
        foreach (var line in output.Split('\n'))
        {
            int zIdx = line.IndexOf("Zeile ", StringComparison.Ordinal);
            if (zIdx < 0) continue;
            int numStart = zIdx + 6;
            int numEnd = numStart;
            while (numEnd < line.Length && char.IsDigit(line[numEnd])) numEnd++;
            if (numEnd == numStart) continue;
            string lineNo = line[numStart..numEnd];
            int colonIdx = line.IndexOf(':', numEnd);
            string msg = colonIdx >= 0 ? line[(colonIdx + 2)..].TrimEnd('\r') : line;

            ErrorListView.Items.Add(new
            {
                Status = "Fehler",
                Code = "-",
                Description = msg,
                Project = "(kein Projektsystem)",
                File = Path.GetFileName(ActiveDoc?.FilePath),
                Line = lineNo,
            });
        }
    }

    // Doppelklick auf eine Fehlerzeile springt zur gemeldeten Zeile im
    // Editor - echte Navigation, keine reine Anzeige (Abschnitt 17 fordert
    // "Doppelklick navigiert zur Fehlerstelle").
    private void OnErrorListDoubleClick(object sender, MouseButtonEventArgs e)
    {
        if (ErrorListView.SelectedItem == null) return;
        var lineProp = ErrorListView.SelectedItem.GetType().GetProperty("Line");
        if (lineProp?.GetValue(ErrorListView.SelectedItem) is not string lineStr) return;
        if (!int.TryParse(lineStr, out int lineNo) || lineNo < 1) return;
        if (lineNo > Editor.Document.LineCount) return;

        var docLine = Editor.Document.GetLineByNumber(lineNo);
        Editor.CaretOffset = docLine.Offset;
        Editor.ScrollToLine(lineNo);
        Editor.Focus();
    }

    // ================= Explorer (echtes Dateisystem) =================
    private void PopulateExplorer()
    {
        SolutionExplorerTree.Items.Clear();
        string rootDir;
        string header;
        if (_currentProject != null)
        {
            rootDir = _currentProject.ProjectDirectory;
            header = _currentProject.Name;
        }
        else
        {
            if (_repoRoot == null) return;
            rootDir = Path.Combine(_repoRoot, "toolchain", "NovaStudio");
            header = "toolchain/NovaStudio";
        }
        var rootItem = new TreeViewItem { Header = header, Tag = rootDir, IsExpanded = true };
        PopulateDirectory(rootItem, rootDir, depth: 0);
        SolutionExplorerTree.Items.Add(rootItem);
    }

    private static readonly string[] RecognizedExtensions = { ".nova", ".nlf", ".nui", ".xml", ".md", ".txt", ".nproj" };

    private void PopulateDirectory(TreeViewItem parent, string dir, int depth)
    {
        if (depth > 6) return;
        try
        {
            foreach (var sub in Directory.GetDirectories(dir).OrderBy(d => d))
            {
                string name = Path.GetFileName(sub);
                if (name.StartsWith(".")) continue; // .git etc. ueberspringen
                if (name is "bin" or "obj") continue; // .NET-Build-Artefakte ueberspringen
                var item = new TreeViewItem { Header = name, Tag = sub };
                PopulateDirectory(item, sub, depth + 1);
                // Ordner IMMER anzeigen, auch leer - ein Ordner, der per
                // "Neuer Ordner" angelegt wurde, muss sofort sichtbar sein
                // statt stillschweigend ausgeblendet zu werden, nur weil er
                // (noch) keine Datei mit erkannter Endung enthaelt.
                parent.Items.Add(item);
            }
            foreach (var file in Directory.GetFiles(dir).OrderBy(f => f))
            {
                if (!RecognizedExtensions.Contains(Path.GetExtension(file).ToLowerInvariant())) continue;
                parent.Items.Add(new TreeViewItem { Header = Path.GetFileName(file), Tag = file });
            }
        }
        catch (UnauthorizedAccessException) { /* Ordner ohne Zugriffsrecht ueberspringen */ }
    }

    // ================= Explorer-Kontextmenü (echte Dateisystem-Operationen) =================
    // Ziel-Verzeichnis fuer "Neue Datei"/"Neuer Ordner": der ausgewaehlte
    // Ordner selbst, oder der Elternordner einer ausgewaehlten Datei, oder
    // der Explorer-Wurzelordner, wenn nichts ausgewaehlt ist.
    private string? GetExplorerTargetDirectory()
    {
        if (SolutionExplorerTree.SelectedItem is TreeViewItem item && item.Tag is string path)
            return Directory.Exists(path) ? path : Path.GetDirectoryName(path);
        if (_currentProject != null) return _currentProject.ProjectDirectory;
        return _repoRoot != null ? Path.Combine(_repoRoot, "toolchain", "NovaStudio") : null;
    }

    private void OnExplorerNewFileClick(object sender, RoutedEventArgs e)
    {
        string? dir = GetExplorerTargetDirectory();
        if (dir == null) return;
        var dlg = new TextInputDialog("Neue Datei", "Dateiname (mit Endung):", "neu.nova") { Owner = this };
        if (dlg.ShowDialog() != true || string.IsNullOrWhiteSpace(dlg.Value)) return;
        string path = Path.Combine(dir, dlg.Value);
        if (File.Exists(path))
        {
            MessageBox.Show($"\"{dlg.Value}\" existiert bereits.", "Nova Studio", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        File.WriteAllText(path, "");
        PopulateExplorer();
        OpenFile(path);
        SetStatusLeft($"Datei erstellt: {dlg.Value}", isError: false);
    }

    private void OnExplorerNewFolderClick(object sender, RoutedEventArgs e)
    {
        string? dir = GetExplorerTargetDirectory();
        if (dir == null) return;
        var dlg = new TextInputDialog("Neuer Ordner", "Ordnername:", "NeuerOrdner") { Owner = this };
        if (dlg.ShowDialog() != true || string.IsNullOrWhiteSpace(dlg.Value)) return;
        string path = Path.Combine(dir, dlg.Value);
        if (Directory.Exists(path))
        {
            MessageBox.Show($"\"{dlg.Value}\" existiert bereits.", "Nova Studio", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        Directory.CreateDirectory(path);
        PopulateExplorer();
        SetStatusLeft($"Ordner erstellt: {dlg.Value}", isError: false);
    }

    private void OnExplorerRenameClick(object sender, RoutedEventArgs e)
    {
        if (SolutionExplorerTree.SelectedItem is not TreeViewItem item || item.Tag is not string oldPath) return;
        bool isDir = Directory.Exists(oldPath);
        string oldName = Path.GetFileName(oldPath);
        var dlg = new TextInputDialog("Umbenennen", "Neuer Name:", oldName) { Owner = this };
        if (dlg.ShowDialog() != true || string.IsNullOrWhiteSpace(dlg.Value) || dlg.Value == oldName) return;
        string? parent = Path.GetDirectoryName(oldPath);
        if (parent == null) return;
        string newPath = Path.Combine(parent, dlg.Value);
        if (File.Exists(newPath) || Directory.Exists(newPath))
        {
            MessageBox.Show($"\"{dlg.Value}\" existiert bereits.", "Nova Studio", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        if (isDir) Directory.Move(oldPath, newPath);
        else File.Move(oldPath, newPath);

        // Ein eventuell offenes Tab dieser Datei muss den neuen Pfad kennen,
        // sonst zeigt "Speichern" auf eine nicht mehr existierende Datei.
        var openDoc = _documents.FirstOrDefault(d => d.FilePath != null && string.Equals(d.FilePath, oldPath, StringComparison.OrdinalIgnoreCase));
        if (openDoc != null)
        {
            openDoc.FilePath = newPath;
            UpdateDocumentTabHeader(openDoc);
            if (openDoc == ActiveDoc) { UpdateBreadcrumb(); DocumentTitleText.Text = " – " + openDoc.DisplayName + (openDoc.IsDirty ? " *" : ""); }
        }
        PopulateExplorer();
        SetStatusLeft($"Umbenannt: {oldName} → {dlg.Value}", isError: false);
    }

    // Verschiebt in den Papierkorb statt endgueltig zu loeschen (geringst-
    // destruktive Option, die den Zweck erfuellt - siehe Sicherheitsregeln).
    private void OnExplorerDeleteClick(object sender, RoutedEventArgs e)
    {
        if (SolutionExplorerTree.SelectedItem is not TreeViewItem item || item.Tag is not string path) return;
        bool isDir = Directory.Exists(path);
        string name = Path.GetFileName(path);
        var result = MessageBox.Show(
            $"\"{name}\" wirklich in den Papierkorb verschieben?" + (isDir ? " (inklusive Inhalt)" : ""),
            "Nova Studio", MessageBoxButton.YesNo, MessageBoxImage.Warning);
        if (result != MessageBoxResult.Yes) return;

        try
        {
            if (isDir)
                Microsoft.VisualBasic.FileIO.FileSystem.DeleteDirectory(path,
                    Microsoft.VisualBasic.FileIO.UIOption.OnlyErrorDialogs,
                    Microsoft.VisualBasic.FileIO.RecycleOption.SendToRecycleBin);
            else
                Microsoft.VisualBasic.FileIO.FileSystem.DeleteFile(path,
                    Microsoft.VisualBasic.FileIO.UIOption.OnlyErrorDialogs,
                    Microsoft.VisualBasic.FileIO.RecycleOption.SendToRecycleBin);
        }
        catch (Exception ex)
        {
            MessageBox.Show($"Löschen fehlgeschlagen: {ex.Message}", "Nova Studio", MessageBoxButton.OK, MessageBoxImage.Error);
            return;
        }

        // Ein offenes Tab dieser (jetzt geloeschten) Datei bleibt offen
        // (Inhalt im Speicher), wird aber als "kein Pfad mehr" markiert,
        // damit "Speichern" nicht stillschweigend eine Phantom-Datei
        // wiederherstellt, sondern ehrlich "Speichern als" verlangt.
        var openDoc = _documents.FirstOrDefault(d => d.FilePath != null && string.Equals(d.FilePath, path, StringComparison.OrdinalIgnoreCase));
        if (openDoc != null)
        {
            openDoc.FilePath = null;
            UpdateDocumentTabHeader(openDoc);
        }
        PopulateExplorer();
        SetStatusLeft($"Gelöscht (Papierkorb): {name}", isError: false);
    }

    private void OnExplorerRefreshClick(object sender, RoutedEventArgs e) => PopulateExplorer();

    private void OnExplorerDoubleClick(object sender, MouseButtonEventArgs e)
    {
        if (SolutionExplorerTree.SelectedItem is not TreeViewItem item) return;
        if (item.Tag is not string path || !File.Exists(path)) return;
        OpenFile(path);
    }

    // ================= Mehrfach-Dokument-Tabs =================
    // Oeffnet eine Datei in einem NEUEN Tab (oder wechselt zu ihrem Tab,
    // falls bereits offen) - KEIN Ersetzen/Verwerfen des aktuell aktiven
    // Dokuments mehr noetig, da jedes Dokument sein eigenes Tab hat.
    private void OpenFile(string path)
    {
        var existing = _documents.FirstOrDefault(d =>
            d.FilePath != null && string.Equals(d.FilePath, path, StringComparison.OrdinalIgnoreCase));
        if (existing != null)
        {
            DocumentTabs.SelectedItem = existing.Tab;
            return;
        }
        var doc = new OpenDocument { FilePath = path, Content = File.ReadAllText(path), IsDirty = false };
        AddDocumentTab(doc);
    }

    private void AddDocumentTab(OpenDocument doc)
    {
        doc.TitleText = new TextBlock { Text = doc.DisplayName, VerticalAlignment = VerticalAlignment.Center };
        var closeButton = new Button
        {
            Content = "✕",
            Margin = new Thickness(10, 0, 0, 0),
            Padding = new Thickness(4, 0, 4, 0),
            Background = Brushes.Transparent,
            BorderThickness = new Thickness(0),
            Foreground = (Brush)FindResource("SecondaryTextBrush"),
            ToolTip = "Schließen",
        };
        closeButton.Click += (_, _) => CloseDocumentTab(doc);
        var headerPanel = new StackPanel { Orientation = Orientation.Horizontal };
        headerPanel.Children.Add(doc.TitleText);
        headerPanel.Children.Add(closeButton);

        doc.Tab = new TabItem { Header = headerPanel, Tag = doc };
        _documents.Add(doc);
        DocumentTabs.Items.Add(doc.Tab);
        DocumentTabs.SelectedItem = doc.Tab; // löst OnDocumentTabSelectionChanged aus
    }

    private void UpdateDocumentTabHeader(OpenDocument doc)
    {
        doc.TitleText.Text = doc.DisplayName + (doc.IsDirty ? " *" : "");
    }

    private void OnDocumentTabSelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        // Inhalt des VORHER aktiven Tabs sichern, bevor der Editor auf den
        // neuen Tab umgestellt wird (Editor.Text gehoert an dieser Stelle
        // noch dem alten Dokument).
        if (e.RemovedItems.Count > 0 && e.RemovedItems[0] is TabItem oldTab && oldTab.Tag is OpenDocument oldDoc)
            oldDoc.Content = Editor.Text;

        if (e.AddedItems.Count > 0 && e.AddedItems[0] is TabItem newTab && newTab.Tag is OpenDocument newDoc)
        {
            _activeDocIndex = _documents.IndexOf(newDoc);
            _suppressEditorTextChanged = true;
            Editor.Text = newDoc.Content;
            _suppressEditorTextChanged = false;
            UpdateCaretPosition();
            DocumentTitleText.Text = " – " + newDoc.DisplayName + (newDoc.IsDirty ? " *" : "");
        }
        else
        {
            _activeDocIndex = -1;
            _suppressEditorTextChanged = true;
            Editor.Text = "";
            _suppressEditorTextChanged = false;
            DocumentTitleText.Text = "";
        }
        UpdateBreadcrumb();
        UpdateStatus();
    }

    // Schließt EIN Tab (× im Tab-Kopf) - fragt nur fuer DIESES Dokument
    // nach, nicht fuer alle offenen Tabs (anders als Fenster-Schliessen).
    private void CloseDocumentTab(OpenDocument doc)
    {
        if (doc == ActiveDoc) doc.Content = Editor.Text;
        if (doc.IsDirty)
        {
            var result = MessageBox.Show(
                $"\"{doc.DisplayName}\" wurde geändert. Änderungen speichern?",
                "Nova Studio", MessageBoxButton.YesNoCancel, MessageBoxImage.Warning);
            if (result == MessageBoxResult.Cancel) return;
            if (result == MessageBoxResult.Yes && !SaveDocument(doc)) return;
        }

        _documents.Remove(doc);
        DocumentTabs.Items.Remove(doc.Tab);
        // Entfernen loest OnDocumentTabSelectionChanged fuer den neuen
        // aktiven (oder keinen) Tab automatisch aus.
    }

    private void OnEditorTextChanged(object? sender, EventArgs e)
    {
        if (_suppressEditorTextChanged) return;
        var doc = ActiveDoc;
        if (doc == null) return;
        if (!doc.IsDirty)
        {
            doc.IsDirty = true;
            UpdateDocumentTabHeader(doc);
            DocumentTitleText.Text = " – " + doc.DisplayName + " *";
        }
        UpdateBreadcrumb();
    }

    // Tastenkuerzel Strg+N/O/S/Umschalt+S fuer die Datei-Operationen -
    // nur fuer diese vier echt belegten Befehle, keine erfundenen weiteren.
    private void OnWindowPreviewKeyDown(object sender, KeyEventArgs e)
    {
        bool ctrl = (Keyboard.Modifiers & ModifierKeys.Control) == ModifierKeys.Control;
        bool shift = (Keyboard.Modifiers & ModifierKeys.Shift) == ModifierKeys.Shift;
        if (!ctrl) return;
        switch (e.Key)
        {
            case Key.N: OnNewFileClick(sender, e); e.Handled = true; break;
            case Key.O: OnOpenFileClick(sender, e); e.Handled = true; break;
            case Key.S when shift: SaveCurrentFileAs(); e.Handled = true; break;
            case Key.S: SaveCurrentFile(); e.Handled = true; break;
            case Key.Space when Editor.IsKeyboardFocusWithin: OnEditorShowCompletion(); e.Handled = true; break;
        }
    }

    // ================= Datei (Abschnitt 6) =================
    // Echte Operationen gegen das reale Dateisystem. "Neu"/"Öffnen" öffnen
    // jetzt jeweils ein NEUES Tab statt das aktive Dokument zu verwerfen
    // (Mehrfach-Dokument-Tabs) - kein Projektsystem (.nsln) jenseits von
    // NovaProject.cs vorhanden.
    private void OnNewFileClick(object sender, RoutedEventArgs e)
    {
        AddDocumentTab(new OpenDocument { FilePath = null, Content = "", IsDirty = false });
    }

    private void OnOpenFileClick(object sender, RoutedEventArgs e)
    {
        var dlg = new Microsoft.Win32.OpenFileDialog
        {
            Filter = "NovaLang-Quelldateien (*.nova)|*.nova|Alle Dateien (*.*)|*.*",
            InitialDirectory = _repoRoot ?? AppContext.BaseDirectory,
        };
        if (dlg.ShowDialog() == true) OpenFile(dlg.FileName);
    }

    private void OnSaveClick(object sender, RoutedEventArgs e) => SaveCurrentFile();

    private void OnSaveAsClick(object sender, RoutedEventArgs e) => SaveCurrentFileAs();

    // Speichert JEDES geaenderte Tab einzeln (ueber SaveDocument(), das bei
    // Bedarf kurz dorthin umschaltet) - kein Alles-in-einem-Schritt-Trick,
    // der die pro-Tab-Logik (Speichern-als-Dialog bei fehlendem Pfad)
    // umgehen wuerde.
    private void OnSaveAllClick(object sender, RoutedEventArgs e)
    {
        if (ActiveDoc != null) ActiveDoc.Content = Editor.Text;
        int savedCount = 0;
        foreach (var doc in _documents.ToList())
        {
            if (!doc.IsDirty) continue;
            if (SaveDocument(doc)) savedCount++;
        }
        SetStatusLeft(savedCount > 0 ? $"Alle speichern: {savedCount} Datei(en) gespeichert" : "Alle speichern: keine Änderungen", isError: false);
    }

    private bool SaveCurrentFile()
    {
        var doc = ActiveDoc;
        if (doc == null) return false;
        if (doc.FilePath == null) return SaveCurrentFileAs();
        File.WriteAllText(doc.FilePath, Editor.Text, new UTF8Encoding(false));
        doc.Content = Editor.Text;
        doc.IsDirty = false;
        UpdateDocumentTabHeader(doc);
        DocumentTitleText.Text = " – " + doc.DisplayName;
        SetStatusLeft($"Gespeichert: {doc.DisplayName}", isError: false);
        return true;
    }

    private bool SaveCurrentFileAs()
    {
        var doc = ActiveDoc;
        if (doc == null) return false;
        var dlg = new Microsoft.Win32.SaveFileDialog
        {
            Filter = "NovaLang-Quelldateien (*.nova)|*.nova|Alle Dateien (*.*)|*.*",
            InitialDirectory = _repoRoot ?? AppContext.BaseDirectory,
            FileName = doc.DisplayName,
        };
        if (dlg.ShowDialog() != true) return false;
        doc.FilePath = dlg.FileName;
        File.WriteAllText(doc.FilePath, Editor.Text, new UTF8Encoding(false));
        doc.Content = Editor.Text;
        doc.IsDirty = false;
        UpdateDocumentTabHeader(doc);
        UpdateBreadcrumb();
        DocumentTitleText.Text = " – " + doc.DisplayName;
        UpdateStatus();
        PopulateExplorer();
        SetStatusLeft($"Gespeichert: {doc.DisplayName}", isError: false);
        return true;
    }

    // Speichert EIN bestimmtes Dokument, auch wenn es NICHT das aktive
    // Tab ist (fuer CloseDocumentTab/ConfirmCloseAllDocuments) - aktiviert
    // es bei Bedarf kurz, damit derselbe Speichern/Speichern-als-Pfad
    // greift, statt die Logik zu duplizieren.
    private bool SaveDocument(OpenDocument doc)
    {
        if (doc != ActiveDoc) DocumentTabs.SelectedItem = doc.Tab;
        return doc.FilePath == null ? SaveCurrentFileAs() : SaveCurrentFile();
    }

    private void OnExitClick(object sender, RoutedEventArgs e) => Close();

    // ================= Projekt (Abschnitt 6/8, echtes Minimal-Projektsystem) =================
    // Siehe NovaProject.cs fuer die bewusste Einschraenkung auf eine
    // Hauptdatei pro Projekt (novac.exe kompiliert nur eine Datei pro Lauf).
    private void OnNewProjectClick(object sender, RoutedEventArgs e)
    {
        var dlg = new Microsoft.Win32.SaveFileDialog
        {
            Filter = "Nova-Projektdatei (*.nproj)|*.nproj",
            InitialDirectory = _repoRoot ?? AppContext.BaseDirectory,
            FileName = "Unbenannt.nproj",
        };
        if (dlg.ShowDialog() != true) return;

        string name = Path.GetFileNameWithoutExtension(dlg.FileName);
        _currentProject = NovaProject.CreateNew(dlg.FileName, name);
        PopulateExplorer();
        OpenFile(_currentProject.MainFileFullPath);
        RefreshProjectPropertiesPanel();
        SetStatusLeft($"Neues Projekt erstellt: {name}", isError: false);
    }

    private void OnOpenProjectClick(object sender, RoutedEventArgs e)
    {
        var dlg = new Microsoft.Win32.OpenFileDialog
        {
            Filter = "Nova-Projektdatei (*.nproj)|*.nproj",
            InitialDirectory = _repoRoot ?? AppContext.BaseDirectory,
        };
        if (dlg.ShowDialog() != true) return;
        OpenProject(dlg.FileName);
    }

    private void OpenProject(string nprojPath)
    {
        _currentProject = NovaProject.Load(nprojPath);
        PopulateExplorer();
        RefreshProjectPropertiesPanel();
        if (File.Exists(_currentProject.MainFileFullPath))
            OpenFile(_currentProject.MainFileFullPath);
        SetStatusLeft($"Projekt geöffnet: {_currentProject.Name}", isError: false);
    }

    private void OnCloseProjectClick(object sender, RoutedEventArgs e)
    {
        if (_currentProject == null) return;
        _currentProject = null;
        PopulateExplorer();
        RefreshProjectPropertiesPanel();
        SetStatusLeft("Projekt geschlossen", isError: false);
    }

    // Ehrliches Eigenschaften-Panel: nur editierbar, wenn tatsaechlich ein
    // Projekt offen ist; sonst der bisherige "nicht implementiert"-Hinweis.
    private void RefreshProjectPropertiesPanel()
    {
        bool hasProject = _currentProject != null;
        ProjectPropertiesPanel.Visibility = hasProject ? Visibility.Visible : Visibility.Collapsed;
        NoProjectText.Visibility = hasProject ? Visibility.Collapsed : Visibility.Visible;
        if (hasProject)
        {
            ProjectNameBox.Text = _currentProject!.Name;
            ProjectMainFileBox.Text = _currentProject.MainFile;
        }
    }

    private void OnSaveProjectPropertiesClick(object sender, RoutedEventArgs e)
    {
        if (_currentProject == null) return;
        _currentProject.Name = ProjectNameBox.Text;
        _currentProject.MainFile = ProjectMainFileBox.Text;
        _currentProject.Save();
        PopulateExplorer();
        SetStatusLeft($"Projekteigenschaften gespeichert: {_currentProject.Name}", isError: false);
    }

    // ================= Git (echter Wrapper um den installierten git-Client) =================
    // Kein eigenes Git, keine simulierten Ergebnisse - ruft echt "git" per
    // RunProcess() auf, Arbeitsverzeichnis ist die Repo-Wurzel (_repoRoot).
    // Bei Commit/Push, die den Verlauf bzw. den Remote-Zustand aendern, wird
    // ausdruecklich nachgefragt statt stillschweigend auszufuehren.
    private void AppendGitOutput(string text)
    {
        GitOutputText.Text += text.TrimEnd('\n') + "\n";
        GitAnchorable.IsActive = true;
    }

    private void RunGitCommand(string args, string label)
    {
        if (_repoRoot == null)
        {
            AppendGitOutput("Fehler: Repo-Wurzel nicht gefunden.");
            return;
        }
        var (ok, output) = RunProcess("git", args, _repoRoot);
        AppendGitOutput($"> git {args}");
        AppendGitOutput(output);
        SetStatusLeft(ok ? $"Git {label}: erfolgreich" : $"Git {label}: fehlgeschlagen", isError: !ok);
    }

    private void OnGitStatusClick(object sender, RoutedEventArgs e) => RunGitCommand("status --short --branch", "Status");

    private void OnGitLogClick(object sender, RoutedEventArgs e) => RunGitCommand("log --oneline -20", "Log");

    private void OnGitDiffClick(object sender, RoutedEventArgs e)
    {
        string? currentFilePath = ActiveDoc?.FilePath;
        if (currentFilePath != null && _repoRoot != null)
        {
            string relPath = Path.GetRelativePath(_repoRoot, currentFilePath);
            RunGitCommand($"diff -- \"{relPath}\"", "Diff");
        }
        else
        {
            RunGitCommand("diff", "Diff");
        }
    }

    private void OnGitCommitClick(object sender, RoutedEventArgs e)
    {
        var dlg = new CommitDialog { Owner = this };
        if (dlg.ShowDialog() != true) return;
        if (string.IsNullOrWhiteSpace(dlg.CommitMessage))
        {
            AppendGitOutput("Commit abgebrochen: keine Nachricht eingegeben.");
            return;
        }
        if (_repoRoot == null) { AppendGitOutput("Fehler: Repo-Wurzel nicht gefunden."); return; }

        var (okAdd, outAdd) = RunProcess("git", "add -A", _repoRoot);
        AppendGitOutput("> git add -A");
        AppendGitOutput(outAdd);
        if (!okAdd) { SetStatusLeft("Git Commit: fehlgeschlagen (add)", isError: true); return; }

        string escapedMsg = dlg.CommitMessage.Replace("\"", "\\\"");
        RunGitCommand($"commit -m \"{escapedMsg}\"", "Commit");
    }

    private void OnGitPullClick(object sender, RoutedEventArgs e) => RunGitCommand("pull", "Pull");

    private void OnGitPushClick(object sender, RoutedEventArgs e)
    {
        var result = MessageBox.Show(
            "Wirklich zum Remote-Repository pushen? Dies ändert den geteilten Verlauf.",
            "Nova Studio", MessageBoxButton.YesNo, MessageBoxImage.Warning);
        if (result != MessageBoxResult.Yes) return;
        RunGitCommand("push", "Push");
    }

    // ================= IntelliSense (lexikalisch, KEIN Language Server) =================
    // Es existiert kein NovaLang Language Server (siehe IMPLEMENTATION_
    // STATUS.md, "Gehe zu Definition" bleibt deshalb ausdruecklich
    // deaktiviert). Diese Vervollstaendigung ist bewusst nur lexikalisch:
    // echte Compiler-Schluesselwoerter/Builtins (dieselbe Liste wie
    // Syntax/NovaLang.xshd und LanguageReferenceDialog) PLUS Namen, die
    // tatsaechlich per einfachem Text-Scan im offenen Dokument gefunden
    // wurden (Dim-Variablen, Function-Namen) - keine Typpruefung, kein
    // Scope-Verstaendnis, keine projektweite Aufloesung.
    private CompletionWindow? _completionWindow;

    private static readonly (string Text, string Desc)[] StaticCompletions =
    {
        ("Module", "Schlüsselwort: beginnt ein Modul"),
        ("End", "Schlüsselwort: schließt Module/Function/If/While"),
        ("Function", "Schlüsselwort: deklariert eine Funktion"),
        ("As", "Schlüsselwort: trennt Name und Typ"),
        ("Return", "Schlüsselwort: beendet eine Funktion mit Rückgabewert"),
        ("Dim", "Schlüsselwort: deklariert eine lokale Variable"),
        ("If", "Schlüsselwort: bedingte Verzweigung"),
        ("Then", "Schlüsselwort: Teil von If"),
        ("Else", "Schlüsselwort: Teil von If"),
        ("While", "Schlüsselwort: Schleife mit Vorabbedingung"),
        ("Integer", "Typ: Ganzzahl-Variable"),
        ("String", "Typ: Zeichenkette"),
        ("Buffer", "Typ: roher Speicherpuffer"),
        ("Int32", "Typ: einzig unterstützter Function-Rückgabetyp"),
        ("Console.WriteLine", "Builtin: gibt eine Zeichenkette aus"),
        ("WriteBuffer", "Builtin: schreibt einen Puffer"),
        ("AddressOf", "Builtin: liefert eine Adresse"),
        ("PeekInt", "Builtin: liest 4 Byte aus einer Adresse"),
        ("PeekByte", "Builtin: liest 1 Byte aus einer Adresse"),
        ("PokeInt", "Builtin: schreibt 4 Byte an eine Adresse"),
        ("PokeByte", "Builtin: schreibt 1 Byte an eine Adresse"),
        ("PokeString", "Builtin: schreibt eine Zeichenkette an eine Adresse"),
        ("SysCall", "Builtin: direkter Syscall-Aufruf"),
    };

    private static readonly Regex DimNameRegex = new(@"\bDim\s+(\w+)", RegexOptions.Compiled);
    private static readonly Regex FunctionNameRegex = new(@"\bFunction\s+(\w+)\s*\(", RegexOptions.Compiled);

    private void OnEditorShowCompletion()
    {
        var data = new List<NovaLangCompletionData>();
        foreach (var (text, desc) in StaticCompletions)
            data.Add(new NovaLangCompletionData(text, desc));

        var seen = new HashSet<string>(data.Select(d => d.Text));
        foreach (Match m in DimNameRegex.Matches(Editor.Text))
        {
            string name = m.Groups[1].Value;
            if (seen.Add(name)) data.Add(new NovaLangCompletionData(name, "Variable (Dim im geöffneten Dokument gefunden)"));
        }
        foreach (Match m in FunctionNameRegex.Matches(Editor.Text))
        {
            string name = m.Groups[1].Value;
            if (seen.Add(name)) data.Add(new NovaLangCompletionData(name, "Funktion (im geöffneten Dokument gefunden)"));
        }

        _completionWindow = new CompletionWindow(Editor.TextArea);
        foreach (var d in data.OrderBy(d => d.Text))
            _completionWindow.CompletionList.CompletionData.Add(d);
        _completionWindow.Closed += (_, _) => _completionWindow = null;
        _completionWindow.Show();
    }

    // ================= Capabilities & Syscalls (Drag-and-Drop) =================
    // Erleichtert das Programmieren echt: Drag einer Capability auf den
    // Editor traegt sie ins Projekt-Manifest ein (echte NEF-CAPABILITIES-
    // Section beim naechsten "Erstellen", siehe OnBuildClick) + fuegt
    // einen erklaerenden Kommentar ein. Drag eines Syscalls fuegt bei
    // "einfachen" Operationen (siehe SyscallCatalog.IsSimple) ECHTEN,
    // compilierbaren NovaLang-Code ein (SysCall(...) + Statuspruefung,
    // identisches Muster wie novalang/examples/hello_syscall.nova); bei
    // komplexeren Operationen (Argument-Struktur im Syscall-Pointer-
    // Fenster noetig) nur einen ehrlichen Erklaerungs-Kommentar statt
    // erfundenem, ungeprueftem Poke-Code.
    private void InitializeCapabilityAndSyscallCatalogs()
    {
        CapabilitiesList.ItemsSource = CapabilityCatalog.All;

        var syscallView = new ListCollectionView(SyscallCatalog.All.ToList());
        syscallView.GroupDescriptions.Add(new PropertyGroupDescription(nameof(SyscallInfo.Group)));
        SyscallsList.ItemsSource = syscallView;

        Editor.AllowDrop = true;
        Editor.PreviewDragOver += OnEditorPreviewDragOver;
        Editor.Drop += OnEditorDrop;
    }

    private Point _catalogDragStartPoint;

    private void OnCatalogItemPreviewMouseDown(object sender, MouseButtonEventArgs e) => _catalogDragStartPoint = e.GetPosition(null);

    private static T? FindAncestor<T>(DependencyObject? current) where T : DependencyObject
    {
        while (current != null)
        {
            if (current is T match) return match;
            current = VisualTreeHelper.GetParent(current);
        }
        return null;
    }

    private void OnCapabilityPreviewMouseMove(object sender, MouseEventArgs e)
    {
        if (e.LeftButton != MouseButtonState.Pressed) return;
        if (!HasDraggedFarEnough(e)) return;
        if (FindAncestor<ListBoxItem>(e.OriginalSource as DependencyObject)?.DataContext is not CapabilityInfo cap) return;
        DragDrop.DoDragDrop((DependencyObject)sender, cap, DragDropEffects.Copy);
    }

    private void OnSyscallPreviewMouseMove(object sender, MouseEventArgs e)
    {
        if (e.LeftButton != MouseButtonState.Pressed) return;
        if (!HasDraggedFarEnough(e)) return;
        if (FindAncestor<ListBoxItem>(e.OriginalSource as DependencyObject)?.DataContext is not SyscallInfo sc) return;
        DragDrop.DoDragDrop((DependencyObject)sender, sc, DragDropEffects.Copy);
    }

    private bool HasDraggedFarEnough(MouseEventArgs e)
    {
        var pos = e.GetPosition(null);
        return Math.Abs(pos.X - _catalogDragStartPoint.X) > SystemParameters.MinimumHorizontalDragDistance
            || Math.Abs(pos.Y - _catalogDragStartPoint.Y) > SystemParameters.MinimumVerticalDragDistance;
    }

    private void OnEditorPreviewDragOver(object sender, DragEventArgs e)
    {
        e.Effects = (e.Data.GetDataPresent(typeof(CapabilityInfo)) || e.Data.GetDataPresent(typeof(SyscallInfo)))
            ? DragDropEffects.Copy : DragDropEffects.None;
        e.Handled = true;
    }

    private void OnEditorDrop(object sender, DragEventArgs e)
    {
        if (ActiveDoc == null)
        {
            MessageBox.Show("Kein Dokument geöffnet - zuerst eine .nova-Datei öffnen oder anlegen.",
                "Nova Studio", MessageBoxButton.OK, MessageBoxImage.Information);
            return;
        }

        var position = e.GetPosition(Editor);
        var viewPos = Editor.GetPositionFromPoint(position);
        int offset = viewPos.HasValue ? Editor.Document.GetOffset(viewPos.Value.Location) : Editor.CaretOffset;

        if (e.Data.GetData(typeof(CapabilityInfo)) is CapabilityInfo cap)
        {
            InsertCapabilitySnippet(cap, offset);
        }
        else if (e.Data.GetData(typeof(SyscallInfo)) is SyscallInfo sc)
        {
            InsertSyscallSnippet(sc, offset);
        }
    }

    private void InsertCapabilitySnippet(CapabilityInfo cap, int offset)
    {
        bool addedToManifest = false;
        if (_currentProject != null && !_currentProject.RequestedCapabilities.Contains(cap.Name))
        {
            _currentProject.RequestedCapabilities.Add(cap.Name);
            _currentProject.Save();
            addedToManifest = true;
        }

        string comment = addedToManifest
            ? $"' {cap.Name} wurde ins Projektmanifest eingetragen (wird beim naechsten \"Erstellen\" als\n" +
              $"' echte NEF-CAPABILITIES-Section mitgebaut). Keine eigene NovaLang-Syntax noetig -\n" +
              $"' die Capability wird vom Kernel bei jedem Syscall automatisch geprueft.\n" +
              $"' Zweck: {cap.Description}\n"
            : _currentProject != null
                ? $"' {cap.Name} ist bereits im Projektmanifest eingetragen. Zweck: {cap.Description}\n"
                : $"' {cap.Name} ({cap.Description}) - kein Projekt geöffnet, daher NICHT automatisch\n" +
                  $"' ins Manifest eingetragen. Über die Registerkarte 'Projekt' ein Projekt öffnen/anlegen,\n" +
                  $"' dann erneut hierher ziehen.\n";

        Editor.Document.Insert(offset, comment);
        SetStatusLeft(addedToManifest ? $"Capability {cap.Name} ins Manifest eingetragen" : $"Capability {cap.Name} eingefügt", isError: false);
    }

    private void InsertSyscallSnippet(SyscallInfo sc, int offset)
    {
        string varName = "result" + sc.Name.Replace("_", "");
        string snippet;
        if (sc.IsSimple)
        {
            // Echtes, lauffaehiges Muster - identisch zu novalang/examples/
            // hello_syscall.nova (dort gegen echten novac.exe-Lauf verifiziert).
            snippet =
                $"' {sc.Name} (service={sc.Service}, operation={sc.Operation}): {sc.Description}\n" +
                $"Dim {varName} As Integer = 0\n" +
                $"{varName} = SysCall({sc.Service}, {sc.Operation}, 0, 0)\n" +
                $"If {varName} = 0 Then\n" +
                $"    Console.WriteLine(\"{sc.Name} erfolgreich\")\n" +
                $"Else\n" +
                $"    Console.WriteLine(\"{sc.Name} fehlgeschlagen\")\n" +
                $"End If\n";
        }
        else
        {
            // KEIN erfundener Poke-Code: diese Operation braucht eine
            // Argument-Struktur im Syscall-Pointer-Fenster [0x400000,
            // 0x404000) (siehe syscall.h fuer das exakte Byte-Layout) -
            // das hier nachzubauen ohne es gegen den echten Kernel zu
            // verifizieren, waere genau die Art Dummy-Code, die dieses
            // Projekt ausdruecklich vermeidet.
            snippet =
                $"' {sc.Name} (service={sc.Service}, operation={sc.Operation}): {sc.Description}\n" +
                $"' Diese Operation braucht eine Argument-Struktur im Syscall-Pointer-Fenster\n" +
                $"' [0x400000, 0x404000) (siehe kernel/newkernel/syscall.h fuer das genaue\n" +
                $"' Byte-Layout) - kein automatisch generierbares Minimalbeispiel wie bei den\n" +
                $"' einfachen CORE-Operationen. Aufbau typischerweise per AddressOf/PokeInt/\n" +
                $"' PokeByte auf einen Dim-Buffer, dann SysCall({sc.Service}, {sc.Operation}, AddressOf(puffer), 0).\n";
        }

        Editor.Document.Insert(offset, snippet);
        SetStatusLeft($"Syscall {sc.Name} eingefügt" + (sc.IsSimple ? "" : " (nur Kommentar - Argument-Struktur nötig)"), isError: false);
    }

    // ================= Bibliothek (echte Sprachreferenz) =================
    private void OnShowLanguageReferenceClick(object sender, RoutedEventArgs e)
    {
        new LanguageReferenceDialog { Owner = this }.ShowDialog();
    }

    // ================= Tools (echter NEF-Wrapper) =================
    private void OnNefInspectClick(object sender, RoutedEventArgs e) => RunNefToolOnPickedFile("inspect");
    private void OnNefValidateClick(object sender, RoutedEventArgs e) => RunNefToolOnPickedFile("validate");

    private void RunNefToolOnPickedFile(string subcommand)
    {
        if (_repoRoot == null)
        {
            AppendBuildOutput("Fehler: Repo-Wurzel nicht gefunden.");
            return;
        }
        var dlg = new Microsoft.Win32.OpenFileDialog
        {
            Filter = "NEF-Dateien (*.nef)|*.nef|Alle Dateien (*.*)|*.*",
            InitialDirectory = _currentProject?.ProjectDirectory ?? _repoRoot,
        };
        if (dlg.ShowDialog() != true) return;

        string nef = Path.Combine(_repoRoot, "tools", "nef", "nef.exe");
        var (ok, output) = RunProcess(nef, $"{subcommand} \"{dlg.FileName}\"");
        AppendBuildOutput($"> nef {subcommand} \"{dlg.FileName}\"");
        AppendBuildOutput(output);
        SetStatusLeft(ok ? $"NEF {subcommand}: erfolgreich" : $"NEF {subcommand}: fehlgeschlagen", isError: !ok);
    }

    // ================= NovaOS (echter QEMU-Start) =================
    // Repliziert exakt qemu-test/start-qemu.cmd (dasselbe Arbeitsverzeichnis,
    // dieselben Parameter) - kein eigener Emulator, kein simulierter Boot.
    private Process? _qemuProcess;

    private void OnNovaOsStartQemuClick(object sender, RoutedEventArgs e)
    {
        if (_repoRoot == null)
        {
            AppendBuildOutput("Fehler: Repo-Wurzel nicht gefunden.");
            SetStatusLeft("QEMU: Repo-Wurzel nicht gefunden", isError: true);
            return;
        }
        if (_qemuProcess != null && !_qemuProcess.HasExited)
        {
            SetStatusLeft("QEMU läuft bereits", isError: false);
            return;
        }

        string qemuDir = Path.Combine(_repoRoot, "qemu-test");
        string imgPath = Path.Combine(qemuDir, "nova-uefi.img");
        string zipPath = Path.Combine(qemuDir, "nova-uefi.zip");
        string fdPath = Path.Combine(qemuDir, "edk2-x86_64.fd");
        string varsPath = Path.Combine(qemuDir, "edk2-vars.fd");

        try
        {
            if (!File.Exists(imgPath) && File.Exists(zipPath))
            {
                System.IO.Compression.ZipFile.ExtractToDirectory(zipPath, qemuDir, overwriteFiles: true);
            }
            if (!File.Exists(imgPath))
            {
                AppendBuildOutput($"Fehler: {imgPath} nicht gefunden (auch kein nova-uefi.zip zum Entpacken).");
                SetStatusLeft("QEMU: Image nicht gefunden", isError: true);
                return;
            }
            File.Copy(fdPath, varsPath, overwrite: true);

            // qemu-system-x86_64 steht auf diesem System NICHT im PATH
            // (verifiziert: "where qemu-system-x86_64" schlaegt in cmd.exe
            // UND PowerShell fehl, trotz vorhandener Installation) - daher
            // zuerst den bekannten Installationspfad probieren, erst dann
            // auf PATH-Aufloesung zurueckfallen.
            string qemuExe = "qemu-system-x86_64";
            string knownPath = @"C:\Program Files\qemu\qemu-system-x86_64.exe";
            if (File.Exists(knownPath)) qemuExe = knownPath;

            var psi = new ProcessStartInfo(qemuExe)
            {
                Arguments = "-machine q35 -m 256M -smp 2 " +
                            "-drive if=pflash,format=raw,file=edk2-vars.fd " +
                            "-drive format=raw,file=nova-uefi.img,if=ide " +
                            "-serial file:serial.log",
                WorkingDirectory = qemuDir,
                UseShellExecute = false,
            };
            _qemuProcess = Process.Start(psi);
            AppendBuildOutput($"> qemu-system-x86_64 {psi.Arguments}");
            AppendBuildOutput(_qemuProcess != null
                ? $"QEMU gestartet (PID {_qemuProcess.Id}), Arbeitsverzeichnis: {qemuDir}"
                : "Fehler: Prozess konnte nicht gestartet werden.");
            SetStatusLeft(_qemuProcess != null ? "QEMU gestartet" : "QEMU: Start fehlgeschlagen", isError: _qemuProcess == null);
        }
        catch (Exception ex)
        {
            AppendBuildOutput("Fehler beim Starten von QEMU: " + ex.Message);
            SetStatusLeft("QEMU: Start fehlgeschlagen", isError: true);
        }
    }

    private void OnNovaOsStopQemuClick(object sender, RoutedEventArgs e)
    {
        if (_qemuProcess == null || _qemuProcess.HasExited)
        {
            SetStatusLeft("QEMU läuft nicht", isError: false);
            return;
        }
        _qemuProcess.Kill();
        AppendBuildOutput("QEMU beendet.");
        SetStatusLeft("QEMU beendet", isError: false);
    }

    private void OnNovaOsShowSerialLogClick(object sender, RoutedEventArgs e)
    {
        if (_repoRoot == null) return;
        string logPath = Path.Combine(_repoRoot, "qemu-test", "serial.log");
        if (!File.Exists(logPath))
        {
            AppendBuildOutput("Kein serial.log vorhanden - QEMU wurde noch nicht gestartet.");
            return;
        }
        try
        {
            // Datei wird von QEMU evtl. noch exklusiv beschrieben - mit
            // FileShare.ReadWrite oeffnen statt File.ReadAllText (das OHNE
            // Freigabe oeffnet und bei laufendem QEMU fehlschlagen kann).
            using var stream = new FileStream(logPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
            using var reader = new StreamReader(stream);
            string content = reader.ReadToEnd();
            AppendBuildOutput("──── serial.log ────");
            AppendBuildOutput(content);
        }
        catch (Exception ex)
        {
            AppendBuildOutput("Fehler beim Lesen von serial.log: " + ex.Message);
        }
    }

    // ================= Ansicht (Abschnitt 7, Fensterbereiche) =================
    // Die eigentliche VS-artige Panel-Anpassung (frei verschieben/andocken/
    // schweben lassen/Grössenziehen/Auto-Hide) liefert jetzt AvalonDock
    // (xcad:DockingManager) selbst ueber sein eigenes Andock-UI (× zum
    // Schliessen, Reissnagel zum Auto-Hide, Ziehen an den Tab zum Andocken
    // woanders) - diese drei Ribbon-Knoepfe bleiben als bequeme globale
    // Kurzbefehle erhalten (Hide()/Show() auf den jeweiligen LayoutAnchorable),
    // ersetzen die AvalonDock-eigene Bedienung aber nicht.
    private void OnToggleExplorerClick(object sender, RoutedEventArgs e)
    {
        if (ToggleExplorerButton.IsChecked == true) ExplorerAnchorable.Show(); else ExplorerAnchorable.Hide();
    }

    private void OnTogglePropertiesClick(object sender, RoutedEventArgs e)
    {
        if (TogglePropertiesButton.IsChecked == true) { PropertiesAnchorable.Show(); CapabilitiesAnchorable.Show(); }
        else { PropertiesAnchorable.Hide(); CapabilitiesAnchorable.Hide(); }
    }

    private void OnToggleOutputClick(object sender, RoutedEventArgs e)
    {
        if (ToggleOutputButton.IsChecked == true)
        {
            FehlerlisteAnchorable.Show(); BuildAusgabeAnchorable.Show();
            GitAnchorable.Show(); TerminalAnchorable.Show(); DebuggerAnchorable.Show();
        }
        else
        {
            FehlerlisteAnchorable.Hide(); BuildAusgabeAnchorable.Hide();
            GitAnchorable.Hide(); TerminalAnchorable.Hide(); DebuggerAnchorable.Hide();
        }
    }

    private void OnZoomInClick(object sender, RoutedEventArgs e) => Editor.FontSize = Math.Min(Editor.FontSize + 1, 36);
    private void OnZoomOutClick(object sender, RoutedEventArgs e) => Editor.FontSize = Math.Max(Editor.FontSize - 1, 8);
    private void OnZoomResetClick(object sender, RoutedEventArgs e) => Editor.FontSize = 14;

    private bool _isFullScreen;

    private void OnToggleFullScreenClick(object sender, RoutedEventArgs e)
    {
        _isFullScreen = !_isFullScreen;
        WindowState = _isFullScreen ? WindowState.Maximized : WindowState.Normal;
    }

    private void OnWindowClosing(object? sender, System.ComponentModel.CancelEventArgs e)
    {
        if (!ConfirmCloseAllDocuments()) e.Cancel = true;
    }

    // Beim Fenster-Schliessen MUESSEN alle offenen Tabs geprueft werden,
    // nicht nur das aktive - fragt pro ungespeichertem Dokument einzeln
    // nach (gleiche Semantik wie CloseDocumentTab, nur fuer alle auf
    // einmal). Bricht beim ersten "Abbrechen" komplett ab, keine Daten
    // gehen verloren.
    private bool ConfirmCloseAllDocuments()
    {
        if (ActiveDoc != null) ActiveDoc.Content = Editor.Text;
        foreach (var doc in _documents.ToList())
        {
            if (!doc.IsDirty) continue;
            var result = MessageBox.Show(
                $"\"{doc.DisplayName}\" wurde geändert. Änderungen speichern?",
                "Nova Studio", MessageBoxButton.YesNoCancel, MessageBoxImage.Warning);
            if (result == MessageBoxResult.Cancel) return false;
            if (result == MessageBoxResult.Yes && !SaveDocument(doc)) return false;
        }
        return true;
    }

    private void UpdateStatus()
    {
        StatusRightText.Text = $"NovaLang | NovaOS SDK | x86-32 (NovaOS) | {(ActiveDoc?.FilePath != null ? "UTF-8" : "-")}";
    }

    // Echter Build-Status statt dauerhaft "Bereit" (Abschnitt 18).
    private void SetStatusLeft(string text, bool isError)
    {
        StatusLeftText.Text = text;
        StatusLeftText.Foreground = isError
            ? (Brush)FindResource("ErrorBrush")
            : (Brush)FindResource("SuccessBrush");
    }
}
