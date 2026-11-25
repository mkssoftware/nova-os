# Nova Studio – Implementierungsstand

Stand: 2026-11-24. Dieses Dokument verfolgt ehrlich (keine erfundenen
Prozentwerte, keine simulierten Ergebnisse), was in Nova Studio
tatsächlich funktioniert und was nicht.

## DRITTE Entscheidung (2026-11-24, Stack-Wechsel zurück zu C#/WPF)

Eine weitere, sehr detaillierte GUI-Designspezifikation forderte erneut
ausdrücklich C#/WPF/XAML/Fluent.Ribbon/AvalonEdit - genau den Stack, der
in der zweiten Entscheidung (siehe unten) explizit abgelehnt worden war.
Der Nutzer wurde auf diesen direkten Widerspruch zur eigenen vorherigen
Entscheidung hingewiesen und hat sich diesmal **ausdrücklich für den
Wechsel zu C#/WPF entschieden** - die native Win32/C++-IDE
(`src/*.cpp`, siehe unten) wird dadurch NICHT weiterentwickelt, bleibt
aber unverändert im Repository erhalten (keine Löschung, kein
Datenverlust der bisherigen Arbeit).

**Ab hier bezieht sich der aktuelle Stand auf die neue WPF-Anwendung**
unter `src/NovaStudio.UI/` (`NovaStudio.sln`). Die nachfolgenden
Abschnitte (Win32/C++-Historie) bleiben als Dokumentation der
TATSÄCHLICH geleisteten früheren Arbeit erhalten, beschreiben aber NICHT
mehr den aktuell weiterentwickelten Code-Pfad.

### WPF-Umsetzung, erster Schritt (2026-11-24)

Die GUI-Designspezifikation (Abschnitt 1-25) verlangt ein vollständiges,
pixel-genaues Visual-Studio-artiges Redesign mit eigenem Theme-System,
IntelliSense-Popups, Symbolbrowser, Debugger-UI usw. - das ist in einer
Sitzung nicht vollständig umsetzbar (vergleichbar mit dem ursprünglichen
27-Abschnitte-Auftrag, der denselben Umfang hatte). Umgesetzt wurde ein
erster, echter, lauffähiger Schritt:

| Bereich | Status | Details |
|---|---|---|
| Projekt-/Solution-Grundgerüst | ✅ | `NovaStudio.sln` + `src/NovaStudio.UI/NovaStudio.UI.csproj` (net8.0-windows), NuGet-Pakete `Fluent.Ribbon` 11.0.2 + `AvalonEdit` 6.3.1.120 erfolgreich aufgelöst |
| Zentrales Theme-System (Abschnitt 3/21/22) | ✅ | `Themes/Colors.xaml` (jede im Auftrag genannte Hex-Farbe exakt übernommen), `Brushes.xaml`, `Dimensions.xaml` (alle DIP-Werte aus Abschnitt 4), `Typography.xaml`, `NovaDark.xaml` (Basis-Styles für Window/Button/TextBox/ScrollBar/TreeView) |
| Eigene Titelleiste (Abschnitt 5) | ✅ | `WindowChrome`, Farbverlauf, Nova-Stern (als `Path`-Vektorgrafik mit radialem Verlauf + Glow-Effekt), Minimieren/Maximieren/Schließen mit Hover-Verhalten gemäß Spezifikation |
| Ribbon mit allen 12 Registerkarten (Abschnitt 6/7) | ✅ (alle 12 befüllt) | 9 Tabs echt funktionsfähig (Start/Erstellen/Datei/Ansicht/Projekt/Git/NovaOS/Bibliothek/Tools). Die restlichen 3 (Debuggen/Testen/Designer) sind ausdrücklich deaktiviert mit erklärendem Tooltip statt leer ODER vorgetäuscht (Abschnitt 23: keine Dummy-Funktionen) - Grund jeweils: kein Debug-Protokoll im Kernel, kein Testrunner-/Assertion-Framework für NovaLang, kein GUI-Markup-Format/Canvas-Editor für NovaOS |
| Bibliothek-Tab: echte Sprachreferenz (neu) | ✅ **echt, keine Demo** | `LanguageReferenceDialog` listet Schlüsselwörter/Typen/Builtins - identisch zu `Syntax/NovaLang.xshd`, direkt aus `novalang/compiler/src/lexer.h`+`parser.c` abgeglichen, keine erfundenen Funktionen. Kein Standardbibliotheks-Paketmanager (es gibt keine NovaLang-Pakete) |
| Tools-Tab: echter NEF-Inspektor (neu) | ✅ **echt, keine Demo** | Echter Wrapper um `tools/nef/nef.exe inspect`/`validate` auf einer per Dateidialog gewählten `.nef`-Datei - keine eigene NEF-Parser-Implementierung in NovaStudio. Gegen echte, frisch erzeugte `.nef`-Datei verifiziert (`inspect` zeigt korrekten Header+Sections, `validate` meldet `GUELTIG`) |
| NovaOS-Tab: echter QEMU-Start (neu) | ✅ **echt, keine Demo** | Repliziert exakt `qemu-test/start-qemu.cmd` (gleiches Arbeitsverzeichnis, gleiche Parameter: `-machine q35 -m 256M -smp 2 -drive if=pflash,... -drive format=raw,file=nova-uefi.img,if=ide -serial file:serial.log`), entpackt `nova-uefi.zip` falls `nova-uefi.img` fehlt, kopiert `edk2-x86_64.fd`→`edk2-vars.fd`. "QEMU beenden" killt den echten Prozess, "Serial-Log anzeigen" liest `serial.log` mit `FileShare.ReadWrite` (damit auch bei laufendem QEMU lesbar). **Echter Bug gefunden+behoben:** `qemu-system-x86_64` steht auf diesem System NICHT im PATH (verifiziert: `where qemu-system-x86_64` schlägt sowohl in `cmd.exe` als auch PowerShell fehl) - Code sucht jetzt zuerst den bekannten Installationspfad `C:\Program Files\qemu\qemu-system-x86_64.exe`, bevor er auf PATH-Auflösung zurückfällt. Voller Durchlauf echt getestet: Prozess startet, `serial.log` zeigt echten UEFI-Boot (`BdsDxe: starting Boot0001 ...`), Prozess lässt sich sauber beenden |
| Git-Tab (neu) | ✅ **echt, keine Demo** | Echter Wrapper um den im System installierten `git`-Client (`RunProcess("git", ...)`, Arbeitsverzeichnis = Repo-Wurzel) - keine eigene Git-Implementierung. Status/Diff (aktuelle Datei, Fallback auf Gesamt-Diff)/Log read-only; Commit über eigenen `CommitDialog` (WPF hat kein eingebautes InputBox) mit `git add -A` + `git commit -m`; Pull direkt; Push nur nach ausdrücklicher Ja/Nein-Rückfrage (ändert geteilten Remote-Zustand). Gegen das echte Repository verifiziert (`git status --short --branch`/`git log --oneline` liefern identische Ausgabe wie direkt in PowerShell) |
| Datei-Operationen (Abschnitt 6, neu) | ✅ **echt, keine Demo** | Neu/Öffnen/Speichern/Alle speichern/Speichern als über echte `Microsoft.Win32.OpenFileDialog`/`SaveFileDialog`, echtes Dateisystem-I/O (UTF-8 ohne BOM, identisch zum Compiler-Build-Pfad). Dirty-Tracking pro Tab über `Editor.TextChanged`, Rückfrage bei ungespeicherten Änderungen (pro Tab beim Schließen, für alle Tabs beim Fenster-Schließen) über echten `MessageBox`-Dialog mit Ja/Nein/Abbrechen. Tastenkürzel Strg+N/O/S/Umschalt+S verdrahtet |
| Ansicht-Tab: Fensterbereiche + Zoom (Abschnitt 7, neu) | ✅ **echt, keine Demo** | `fluent:ToggleButton` für Projektmappen-Explorer/Eigenschaften-Symbolbrowser/Ausgabebereich blendet die jeweilige Spalte/Zeile echt aus (Breite/Höhe auf 0, nicht nur Sichtbarkeit - der Platz geht wirklich an den Editor), Editor-Zoom (Schriftgröße ±1, Zurücksetzen auf 14), Vollbild-Umschalter (`WindowState.Maximized`) |
| Projektmappen-Explorer (Abschnitt 9) | ✅ **echtes Dateisystem + Kontextmenü** | `TreeView` durchsucht rekursiv (reale Dateien, keine Demodaten), Doppelklick öffnet echt im Editor. Rechtsklick-Kontextmenü mit echten Dateisystem-Operationen: Neue Datei/Neuer Ordner (`TextInputDialog`), Umbenennen (aktualisiert den Pfad eines offenen Tabs mit, statt es auf eine nicht mehr existierende Datei zeigen zu lassen), Löschen (verschiebt in den Papierkorb über `Microsoft.VisualBasic.FileIO`, NICHT endgültig - geringste destruktive Option), Aktualisieren. **Nebenbei behobene Lücke:** leere Ordner wurden vorher aus dem Baum gefiltert - ein frisch per "Neuer Ordner" angelegter Ordner wäre dadurch unsichtbar gewesen. Alle vier Dateisystem-Operationen isoliert gegen ein echtes Scratch-Verzeichnis verifiziert (Erstellen/Umbenennen/Papierkorb-Löschen für Datei UND Ordner) |
| Codeeditor (Abschnitt 12) | ✅ (Syntaxhervorhebung) / ⚠️ teilweise (Rest) | `AvalonEdit`-Steuerelement zeigt echten Dateiinhalt, lädt/speichert echte Dateien; echte NovaLang-Syntaxhervorhebung (`src/NovaStudio.UI/Syntax/NovaLang.xshd`) implementiert und boot-verifiziert - Schlüsselwörter/Typen/Builtins direkt aus `novalang/compiler/src/lexer.h`+`parser.c` abgeglichen (NICHT erfunden), Farben exakt aus `Themes/Colors.xaml`. Fehlende Teile: kein Fehlertoleranter Parser für semantische Hervorhebung (nur lexikalisch), keine Bracket-Matching-Anzeige |
| Erstellen (Abschnitt 23: "mit echtem Compiler") | ✅ **echt, keine Demo, End-to-End verifiziert** | Ruft `novac.exe` + `tools/nef/nef.exe` auf. Fehler werden aus der echten Compiler-Ausgabe geparst (Datei/Zeile). **Zwei echte, bis zur jetzigen End-to-End-Prüfung unentdeckte Bugs gefunden+behoben** (siehe eigenen Abschnitt unten: "Build-Pipeline, echte Bugs"): (1) `nef.exe` konnte keine absoluten Windows-Pfade mit Laufwerksbuchstaben verarbeiten, (2) `OnBuildClick` gab den Programmnamen "nef" versehentlich als zusätzliches Argument an `nef.exe` weiter. Kompletter Durchlauf `novac → nef link → nef validate` jetzt mit echten absoluten Pfaden erfolgreich nachgestellt (`GUELTIG`) |
| Statusleiste (Abschnitt 18) | ✅ | Echte Zeile/Spalte aus `Editor.TextArea.Caret` (aktualisiert live über `PositionChanged`), echter Build-Status (links: "Baue..." / "Erstellen erfolgreich: ..." / "Erstellen fehlgeschlagen (...)" mit Fehler-/Erfolgsfarbe je nach tatsächlichem `novac`/`nef`-Ergebnis) statt dauerhaft "Bereit" |
| IntelliSense (Abschnitt 13/14, neu) | ✅ (lexikalisch) / ❌ (semantisch) | **Ehrlich eingeordnet:** KEIN NovaLang Language Server (gibt es nicht, siehe `INTELLISENSE.md` der Win32-Historie) - daher NUR lexikalische Vervollständigung über `ICSharpCode.AvalonEdit.CodeCompletion.CompletionWindow` (Strg+Leertaste): echte Compiler-Schlüsselwörter/Typen/Builtins (dieselbe Liste wie `Syntax/NovaLang.xshd`) PLUS Namen, die per einfachem Regex-Scan (`\bDim\s+(\w+)`, `\bFunction\s+(\w+)\s*\(`) im geöffneten Dokument tatsächlich gefunden wurden - KEINE Typprüfung, KEIN Scope-Verständnis, KEINE projektweite Auflösung. Scan-Logik isoliert gegen eine echte Beispieldatei (`novalang/examples/hello_variables.nova`) verifiziert: findet korrekt `greeting`/`name`/`message` (Dim) und `Main` (Function) |
| Mehrfach-Dokument-Tabs (Abschnitt 10, neu) | ✅ **echt, keine Demo** | `DocumentTabs` zeigt jetzt tatsächlich mehrere gleichzeitig geöffnete Dateien statt nur einer einzelnen - "Neu"/"Öffnen"/Explorer-Doppelklick öffnen jeweils ein eigenes Tab (oder wechseln zum bereits offenen Tab derselben Datei, kein Duplikat). JEDES Tab hat ein eigenes ×-Schließen-Symbol mit eigener Ungespeichert-Rückfrage (Ja/Nein/Abbrechen), unabhängig von den anderen Tabs. Inhalt wird beim Tab-Wechsel synchronisiert (EIN AvalonEdit-Steuerelement, kein eigenes pro Tab - gleiches Architekturmuster wie die frühere Datenverlust-Bugfix-Lösung der Win32-IDE, `EditorSyncActiveTabContent()`). Fenster-Schließen fragt jetzt für JEDES ungespeicherte Tab einzeln nach (`ConfirmCloseAllDocuments()`), nicht nur für das zuletzt aktive |
| Minimal-Projektsystem (.nproj, neu) | ✅ **echt, keine Demo** | `NovaProject.cs`: Neues Projekt/Projekt öffnen/Projekt schließen (Ribbon-Tab "Projekt" UND "Datei"-Tab), JSON-Format mit Name+Hauptdatei. "Neues Projekt" erzeugt eine echt compilierbare Vorlage (gegen echten `novac.exe`-Lauf verifiziert: `novac: 49 Bytes Code -> ...` + erfolgreicher `nef link`-Vorschlag). Bewusste Einschränkung: EIN Projekt = EINE Hauptdatei, kein Mehrdatei-Linken (novac.exe übersetzt nur eine `.nova`-Datei pro Aufruf) - ein `.nsln`-Mehrprojektformat wäre damit reine Attrappe und wurde absichtlich NICHT gebaut. Projektmappen-Explorer zeigt bei offenem Projekt den Projektordner statt `toolchain/NovaStudio` |
| Eigenschaftenfenster (Abschnitt 16) | ✅ (für Projekteigenschaften) | Zeigt bei offenem Projekt echte editierbare Felder (Name, Hauptdatei) mit "Speichern", die zurück in die `.nproj`-Datei schreiben; ohne offenes Projekt weiterhin ehrlicher Hinweis statt Dummy-Feldern |
| Capabilities & Syscalls – Drag-and-Drop-Werkzeuge (neu, ersetzt Symbolbrowser) | ✅ **echt, keine Demo** | Statt eines (einen Language Server voraussetzenden) Symbolbrowsers: zwei echte, produktive Werkzeuge. **Capabilities**-Liste (`CapabilityCatalog.cs`, 1:1 aus `kernel/newkernel/security.h` SECURITY_CAP_* abgeschrieben) - Drag auf den Editor trägt die Capability echt ins `.nproj`-Manifest ein (`NovaProject.RequestedCapabilities`) UND fügt einen erklärenden Kommentar ein; beim nächsten "Erstellen" wird daraus eine echte NEF-`CAPABILITIES`-Section gebaut (4-Byte-Bitmaske, Format exakt wie `nef_loader.c` es erwartet). **Syscalls**-Liste (`SyscallCatalog.cs`, 1:1 aus `kernel/newkernel/syscall.h` SYSCALL_*/GUI_OP_*/VFS_OP_*/INPUT_OP_* abgeschrieben, nach Service gruppiert) - Drag auf den Editor fügt bei "einfachen" Operationen (per echter Code-Analyse von `syscall.c` als argumentlos identifiziert: CORE_READY/_YIELD/_SHUTDOWN/_TOGGLE_*/_GET_*_VISIBLE) ECHTEN, compilierbaren `SysCall(...)`-Code samt Statusprüfung ein (identisches Muster wie `hello_syscall.nova`); bei komplexeren Operationen (GUI/VFS/Input + einige CORE-Ops), die eine Argument-Struktur im Syscall-Pointer-Fenster brauchen, NUR einen ehrlichen Erklärungs-Kommentar statt erfundenem, ungeprüftem Poke-Code. **Vollständig End-to-End verifiziert:** generierter Syscall-Schnipsel echt mit `novac.exe` kompiliert (131 Bytes Code), generierte CAPABILITIES-Section echt mit `nef.exe link`+`validate`+`inspect` geprüft (`GUELTIG`, Section korrekt mit `flags=R vaddr=0x0 vsize=4`) |
| Fehlerliste mit 6 Spalten + Navigation (Abschnitt 17) | ✅ | `ListView`+`GridView` mit allen 6 Spalten (Status/Code/Beschreibung/Projekt/Datei/Zeile), alternierende Zeilenfarben (`ErrorRowBrush`/`ErrorRowAltBrush`, vorher ungenutzte Ressourcen), Doppelklick springt echt zur gemeldeten Zeile im Editor. "Code" zeigt ehrlich "-" (novac.exe liefert keine Fehlercodes), "Projekt" zeigt ehrlich "(kein Projektsystem)" (kein .nsln/.nproj vorhanden) - keine erfundenen Werte. Ausgabeformat gegen echten `novac.exe`-Lauf mit Syntaxfehler verifiziert ("novac: Syntaxfehler: Zeile N: ...") |
| Debugger, Terminal | ❌ nicht implementiert | Zeigen ausdrücklich "noch nicht implementiert" |
| Breadcrumb-Navigation (Abschnitt 11) | ✅ **echt, keine Demo** | Klickbare Breadcrumb "Modulname › Funktionsname ▾" statt reinem Dateipfad. KEIN Namespace/Klassen-Konzept (gibt es in NovaLang nicht, nur Module+Function) und KEIN Language Server - die "aktive Funktion" wird per einfachem Regex-Scan (`\bFunction\s+(\w+)\s*\(`) ermittelt: die letzte Function-Deklaration VOR der Caret-Position, aktualisiert live bei Cursorbewegung/Tab-Wechsel/Texteingabe. Klick auf den Funktionsnamen öffnet ein echtes Menü ALLER im Dokument gefundenen Funktionen - Auswahl springt per `Editor.CaretOffset`/`ScrollTo` real dorthin. Logik isoliert gegen eine echte Mehrfunktions-Datei (`novalang/examples/NovaShell.nova`, 8+ Funktionen) verifiziert: 3 Testzeilen lieferten exakt die erwartete umschließende Funktion (`Main`/`ProcessOneVkeyLine`/`CommandIsHelp`) |
| Screenshots (Abschnitt 24) | ❌ nicht erstellt | Siehe Einschränkung unten |

**Verifiziert:** `dotnet build NovaStudio.sln` - 0 Warnungen, 0 Fehler.
App gestartet und per `Get-Process` als lebendig/reagierend bestätigt
(`Responding=True`, korrekter Fenstertitel "Nova Studio").

**Echter, beim ersten Start gefundener und behobener Bug:** die App
stürzte beim allerersten Start mit einer `InvalidOperationException`
ab ("'7' ist kein gültiger Wert für die Eigenschaft 'CornerRadius'").
Ursache: `CornerRadius`-Setter, die ihren Wert aus einer als `sys:Double`
deklarierten Ressource bezogen - WPFs String→CornerRadius-Typkonvertierung
greift nur bei literalen XAML-Attributen, NICHT wenn der Wert zur
Laufzeit aus einer `{StaticResource}`-Auflösung als bereits boxtes
`double`-Objekt kommt. Behoben durch eigene, korrekt typisierte
`CornerRadius`-Ressourcen (`Dimensions.xaml`). Ein globaler
`DispatcherUnhandledException`-Handler (`App.xaml.cs`) wurde ergänzt,
der künftige Abstürze in eine `crash.log`-Datei schreibt, statt dass die
IDE kommentarlos verschwindet.

**Einschränkung zu Abschnitt 24 (Screenshots):** In dieser Sitzung ist
kein Werkzeug verfügbar, das einen Screenshot einer tatsächlich
laufenden Windows-GUI-Anwendung aufnehmen und als Datei sichern kann
(nur Browser-/Blender-Bildschirmfotos). Die visuelle Abnahme (Abschnitt
24, Punkte 1-20) wurde daher NICHT durchgeführt und `NovaStudio-Dark.png`/
`NovaStudio-IntelliSense.png` wurden NICHT erzeugt. Stattdessen wurde die
Lauffähigkeit über `Get-Process`/`Responding` sowie eine
`DispatcherUnhandledException`-Protokollierung nachgewiesen. Ein
reproduzierbares Skript für einen künftigen Screenshot (sofern ein
Screenshot-Werkzeug verfügbar wird):

```powershell
cd toolchain\NovaStudio
dotnet build NovaStudio.sln
Start-Process ".\src\NovaStudio.UI\bin\Debug\net8.0-windows\NovaStudio.UI.exe"
Start-Sleep -Seconds 2
# Hier: Screenshot-Werkzeug der Wahl aufrufen, z.B.
#   Add-Type -AssemblyName System.Windows.Forms
#   [System.Windows.Forms.SendKeys]::SendWait('{PRTSC}')
#   dann aus der Zwischenablage als PNG speichern.
```

## Echte Ribbon-Icons (Fluent UI System Icons, 2026-11-25)

Auf Nutzerwunsch ("in den Ribbons sind noch keine Icon ... übernehme ...
soviel wie möglich ... das es Professionell aussieht") bekamen 51 der
Ribbon-Buttons echte Icons statt reinem Text. Quelle: die bereits im
Repository liegenden **Fluent UI System Icons** (`Conceptual/Icons/
fluentui-system-icons-main/assets/`, Microsofts offizielles Open-Source-
Icon-Set, 2966 Icons) - ausgewählt nach inhaltlicher Passung zum jeweiligen
Button (z. B. "Save" für Speichern, "Arrow Undo" für Rückgängig, "Bug"
für Debuggen starten), NICHT erfunden oder zufällig.

**Technik:** Icons sind SVG (Vektorgrafik, bleibt bei jeder Größe scharf,
keine Rasterbilder) und werden zur Laufzeit über `SharpVectors.Wpf`
(1.8.6, etablierte WPF-SVG-Bibliothek, `SharpVectors.Converters.
FileSvgReader`) in ein `DrawingImage` geladen (`IconLoader.cs`). Die
Original-SVGs haben hartkodiert `fill="#212121"` (fast schwarz) - auf
unserem dunklen Theme unsichtbar gewesen; ein Kopierskript hat beim
Übernehmen nach `Assets/Icons/*.svg` JEDES Vorkommen auf unsere echte
`PrimaryTextColor` (`#E5E7F0`) umgefärbt. Zuordnung Button-Header-Text ->
Icon-Dateiname über ein Dictionary (`RibbonIconByHeader` in
`MainWindow.xaml.cs`) statt x:Name an über 50 Buttons zu vergeben.

**Zwei echte, beim ersten Testlauf gefundene Bugs behoben:**
1. Eine verwaiste XAML-Zeile (`Icon="/Icons/clipboard_paste_20_regular.svg"`,
   Rest einer fehlgeschlagenen frühen Idee, nicht Teil der finalen
   Lösung) zeigte auf einen nicht existierenden Pfad und ließ die App
   beim Start mit `XamlParseException` abstürzen - entfernt.
2. Der erste Versuch, Icons unmittelbar nach `InitializeComponent()`
   zuzuweisen, traf auf einen KOMPLETT LEEREN logischen Baum (Fluent.
   Ribbon baut seine Tabs/Gruppen/Buttons erst beim Anwenden der eigenen
   Vorlage auf, nicht sofort) - per Zähler bestätigt: 0 Treffer. Fix:
   Zuweisung auf das `Loaded`-Ereignis verschoben. Ein ZWEITER Versuch,
   zusätzlich per `VisualTreeHelper` abzusteigen (für Fälle, die der
   logische Baum nicht abdeckt), ließ die App beim Start dauerhaft
   einfrieren (Endlosschleife durch die Kombination aus Fluent.Ribbon +
   AvalonDock) - durch reinen `LogicalTreeHelper`-Abstieg mit einer
   Zyklenerkennung (`HashSet<DependencyObject>` mit Referenzgleichheit)
   ersetzt, das Problem verschwand vollständig.

**Verifiziert:** Build 0 Warnungen/Fehler, App startet/läuft/reagiert
ohne Absturz. Per Zähler bestätigt: alle 51 im Dictionary hinterlegten
Button-Header wurden im echten, laufenden Ribbon gefunden und bekamen
ihr Icon zugewiesen (`matched=51 loaded=51`). Per echtem Screenshot
bestätigt: Icons rendern korrekt, scharf und in der richtigen (hellen)
Farbe auf dem dunklen Ribbon-Hintergrund.

**Ehrliche Grenze:** nur Ribbon-Buttons bekamen Icons (das war der
explizite Nutzerwunsch, "in den Ribbons"). Explorer-Kontextmenü,
AvalonDock-Panel-Tabs (Projektmappen-Explorer/Eigenschaften/etc.) und
Dokumenttabs haben weiterhin keine Icons - könnten mit derselben
Infrastruktur (`IconLoader.Load()`) ergänzt werden, falls gewünscht.

## VS-artige andockbare/verschiebbare Panels (Abschnitt 8, 2026-11-25)

Auf Nutzerwunsch ("Panels so anpassen wie bei VS Studio") wurde das feste
Grid-Layout (Explorer/Editor/Eigenschaften/Ausgabe in festen Spalten/
Zeilen) durch **Dirkster.AvalonDock** (5.0.2, echte, etablierte WPF-
Docking-Bibliothek - keine selbstgebaute Attrappen-Docking-Logik)
ersetzt. Jedes Werkzeugfenster (Projektmappen-Explorer, Eigenschaften,
Capabilities & Syscalls, Fehlerliste/Build-Ausgabe/Git/Terminal/
Debugger) ist jetzt ein echtes `xcad:LayoutAnchorable` mit vollem
VS-artigem Bedien-Chrome: per Ziehen am Tab an jede Seite andockbar,
als eigenes schwebendes Fenster lösbar, per Reissnagel-Symbol
auto-einklappbar, per × schliessbar und über die "Ansicht"-Ribbon-
Knöpfe (die jetzt `Hide()`/`Show()` statt Grid-Spalten/Zeilen-Tricks
aufrufen) weiterhin global umschaltbar.

**Gleiche Lehre wie beim Fluent.Ribbon-Fund vorab angewandt:** AvalonDock
braucht ebenfalls ein explizites `Themes/Generic.xaml`-Merge in
`App.xaml` (`pack://application:,,,/AvalonDock;component/Themes/
Generic.xaml`), sonst waere dieselbe Art "komplett leeres Steuerelement"-
Bug zu erwarten gewesen - diesmal proaktiv vermieden statt erneut per
Trial-and-Error gefunden.

**Bewusste, ehrliche Architekturgrenze:** der Editor-Bereich bleibt EIN
gemeinsames `xcad:LayoutDocument` mit einem einzigen AvalonEdit-
Steuerelement (Inhalt wird beim Tab-Wechsel synchronisiert, wie zuvor) -
echtes paralleles Side-by-side-Bearbeiten zweier Dateien in getrennten
AvalonDock-Dokumenten wird NICHT unterstützt, da das die bestehende,
bewusst gewählte Ein-Editor-Architektur brechen würde (mehrere
gleichzeitig sichtbare Dokumente bräuchten jeweils ein eigenes
AvalonEdit-Steuerelement). Die Werkzeugfenster (Explorer/Eigenschaften/
Capabilities-Syscalls/Ausgabe) sind davon nicht betroffen und voll
VS-artig frei anpassbar.

**Bekannter, kleiner kosmetischer Rest (nicht funktional, ehrlich
dokumentiert statt verschwiegen):** die Dokument-Tab-Leiste (Bereich
rechts neben dem "Editor"-Tab-Kopf) bleibt stellenweise hell statt
dunkel - ein gezielter Style-Override auf `xcad:LayoutDocumentPaneControl`
wurde ergänzt, behebt es aber nicht vollständig (AvalonDocks Vorlage
zeichnet diesen Bereich offenbar über eine zweite, nicht ueber die
Control-`Background`-Eigenschaft erreichbare Flaeche). Betrifft nur
dieses eine optische Detail, keine Funktion.

Verifiziert per echtem Screenshot (PowerShell+`System.Drawing`, kein
dediziertes Screenshot-Werkzeug verfügbar): "Projektmappen-Explorer"
zeigt echtes AvalonDock-Chrome (Dropdown-Pfeil/Reissnagel/×), Build:
0 Warnungen/Fehler, App startet/läuft/reagiert ohne Absturz.

## Ribbon komplett unsichtbar - echter Bug gefunden+behoben (2026-11-25)

Der Nutzer meldete "ich sehe keine Ribbons" nach dem ersten Release-Build.
Per echtem Screenshot (PowerShell + `System.Drawing`, da kein dediziertes
Screenshot-Werkzeug in dieser Session verfügbar ist) bestätigt: die
Ribbon-Fläche war komplett leer (dunkler Hintergrund, aber keine Tabs,
kein Text, kein Backstage-Button) - **kein Absturz, keine Ausnahme**.

**Diagnose (Schritt für Schritt, kein Raten):**
1. Theme-Hypothese (fehlendes Fluent.Ribbon-Farbthema) per
   `ControlzEx.Theming.ThemeManager.ChangeTheme()` getestet - behob es
   NICHT, obwohl der Aufruf fehlerfrei durchlief.
2. Echter Visual-Tree-Dump (`VisualTreeHelper.GetChildrenCount`) direkt
   aus der laufenden App geschrieben: `MainRibbon: ActualHeight=0`,
   **null Kindelemente** - das Steuerelement hatte buchstäblich KEINE
   Vorlage angewendet bekommen.
3. Isoliert in einem komplett minimalen Testfenster reproduziert (kein
   `WindowChrome`, kein eigener Style, keine eigenen Resources) - exakt
   derselbe Befund. Damit ausgeschlossen: eigene Fensteranpassungen,
   Grid-Layout, `SelectedTabItem`-Bindung, mehrere Tabs.
4. **Ursache gefunden:** `Fluent.Ribbon` 11.0.2 verlässt sich unter
   `net8.0-windows` NICHT zuverlässig auf WPFs automatische
   `Themes/Generic.xaml`-Auflösung (trotz vorhandenem
   `ThemeInfoAttribute` in der Assembly) - die eingebetteten Steuerelement-
   Vorlagen wurden nie geladen. Fix bestätigt: explizites Mergen von
   `pack://application:,,,/Fluent;component/Themes/Generic.xaml` in
   `App.xaml` VOR dem eigenen Theme. Nach diesem Fix: `ActualHeight=128`,
   vollständiger Visual Tree (`RibbonTabControl`→`RibbonTabsContainer`→
   `RibbonTabItem`→...).
5. **Zweiter, kleinerer Bug dabei gefunden:** ein leeres
   `<fluent:Backstage/>`-Element zeigte einen zweiten, funktionslosen
   "Datei"-Button direkt neben dem bereits echten "Datei"-Tab (Fluents
   Standard-Header für Backstage lautet ebenfalls "Datei"). Da der
   Backstage-Bereich leer war (keine Inhalte, kein Projekt-/Speichern-
   Dialog dahinter) und unser "Datei"-Tab bereits alle echte
   Funktionalität abdeckt, wurde das leere Backstage-Element entfernt
   statt nur umbenannt - ein leerer Backstage-Bereich wäre selbst
   Attrappen-UI gewesen.

Per finalem Screenshot bestätigt: alle 12 Ribbon-Tabs sichtbar, dunkles
Farbthema (`ControlzEx.Theming.ThemeManager`, `ApplyFluentTheme()` in
`MainWindow.xaml.cs`) korrekt angewendet, kein doppelter "Datei"-Tab mehr.

## Build-Pipeline, echte Bugs gefunden+behoben (2026-11-24)

Bis zu diesem Zeitpunkt war "Erstellen" nur gegen den Syntaxfehler-Fall
(novac schlägt fehl) und per isoliertem Modelltest getestet - NIE als
echter kompletter Durchlauf `novac → nef link → nef validate` mit den
tatsächlichen absoluten Windows-Pfaden, die die IDE im Normalbetrieb
erzeugt. Ein bewusst nachgeholter End-to-End-Test (echte `.nova`-Datei,
echter Aufruf exakt wie `OnBuildClick`) deckte ZWEI echte, bis dahin
unentdeckte Bugs auf, die den gesamten Build-Weg gebrochen hätten:

1. **`tools/nef/nef.exe` konnte keine absoluten Windows-Pfade verarbeiten.**
   `parse_section_spec()` in `tools/nef/nef_main.c` spaltete die
   `--section TYP:FLAGS:VADDR:ALIGN:DATEI[:VSIZE]`-Angabe naiv bei JEDEM
   Doppelpunkt. Ein Pfad wie `C:\Users\...\datei.bin` enthält selbst einen
   Doppelpunkt (Laufwerksbuchstabe) - das DATEI-Feld wurde dadurch
   fälschlich zu `"C"` abgeschnitten (`nef: Fehler: kann 'C' nicht lesen`).
   **Fix:** Nur die ersten 4 Felder werden über die ersten 4 Doppelpunkte
   abgespalten; der Rest ist DATEI[:VSIZE]. Ob ein VSIZE-Suffix vorhanden
   ist, wird geprüft, indem der Teil NACH DEM LETZTEN Doppelpunkt auf
   vollständige Zahl-Lesbarkeit getestet wird (ein Pfadfragment wie
   `\Users\foo` ist keine vollständige Zahl, `4096` ist eine). Verifiziert:
   `nef.exe test` (eingebauter Selbsttest) weiterhin `OK`, zusätzlich
   manuell mit absoluten Pfaden, relativen Pfaden und explizitem
   `:VSIZE`-Suffix (kleiner UND größer als die echte Dateigröße, inkl.
   Prüfung der dokumentierten Clamp-Regel "vsize >= file_size") getestet.
   Dieser Bug betraf NICHT nur die neue WPF-IDE, sondern auch die
   bestehende Win32-IDE und jeden anderen Aufrufer von `nef link` mit
   absoluten Windows-Pfaden.
2. **`MainWindow.xaml.cs::OnBuildClick` reichte den Programmnamen "nef"
   als zusätzliches Argument an `nef.exe` weiter.** `novac.exe` gibt die
   Zeile `novac: naechster Schritt: nef link <ausgabe.nef> ...` aus
   (inklusive des Programmnamens "nef" als Teil der vorgeschlagenen
   Kommandozeile). Der Code extrahierte diese Zeile und übergab sie
   unverändert als Argument-String an den bereits als `nef.exe` gestarteten
   Prozess - der erhielt dadurch `"nef"` als ersten, unbekannten Subbefehl
   statt `"link"`. **Fix:** führendes `"nef "` wird vor der Übergabe
   abgeschnitten. Verifiziert: kompletter Durchlauf mit einer echten
   `.nova`-Testdatei (`novalang/examples/hello_nova.nova`) über absolute
   Pfade im System-Temp-Verzeichnis, `nef validate` meldet am Ende
   `GUELTIG`.

**Lehre:** "Build 0 Fehler" und "App startet" allein beweisen nicht, dass
eine Kommandozeilen-Integration zwischen zwei externen Tools tatsächlich
funktioniert - nur ein echter, unverkürzter Ende-zu-Ende-Lauf mit
realistischen (absoluten) Pfaden deckt das auf.

**Nachtrag zur Win32/C++-IDE (nicht behoben, da nicht mehr weiterentwickelt):**
`src/main.cpp::DoBuild()` baut die `nef link`-Kommandozeile nach exakt
demselben Muster (`nefCmdLine = "\"" + nef + "\" " + nefLinkCmd` mit
`nefLinkCmd`, das selbst mit `"nef link ..."` beginnt) - Bug Nr. 2 oben
besteht dort also ebenfalls. Bug Nr. 1 (`nef.exe`-Pfadparser) betrifft
JEDEN Aufrufer, also auch die Win32-IDE, und ist durch den Fix in
`tools/nef/nef_main.c` bereits für beide IDEs behoben. Bug Nr. 2 wird in
der Win32-IDE bewusst NICHT behoben, da der Nutzer diesen Code-Pfad
ausdrücklich nicht mehr weiterentwickelt (siehe "DRITTE Entscheidung"
oben) - frühere Protokolleinträge, die das "Erstellen" der Win32-IDE als
"boot-verifiziert" bezeichnen, bezogen sich demnach nicht auf einen
echten `nef link`-Durchlauf mit absoluten Pfaden.

## Vorherige Entscheidung (Win32/C++, NICHT mehr der aktuelle Code-Pfad)

Ein Entwicklungsauftrag vom 2026-11-24 forderte eine komplette
Neuentwicklung von Nova Studio in C#/WPF/Fluent.Ribbon unter
`toolchain/NovaStudio/`. Die Bestandsanalyse ergab jedoch, dass unter
genau diesem Pfad bereits eine substantielle, aktiv entwickelte
**"Nova Studio 2.0"** existiert: eine native Win32/C++-IDE (~4400 Zeilen,
`src/main.cpp`, `editor.cpp`, `explorer.cpp`, `highlighter.cpp`,
`ribbon.cpp`, `dialogs.cpp`, `pthread_win32.cpp`), mit eigener Titelleiste,
Ribbon, RichEdit-Editor, Explorer-TreeView, Theming (Dunkel/Hell) und
Build/Run/Clean-Grundgerüst, letzter Commit 2026-10-09, statisch gelinkt
(keine MSYS2-Laufzeit-DLL-Abhängigkeit).

Der Nutzer wurde auf diesen Konflikt hingewiesen und hat sich **explizit
dafür entschieden, die vorhandene Win32/C++-IDE weiterzuentwickeln statt
einen parallelen C#/WPF-Stack zu beginnen** (vermeidet die vom
NovaOS-Projekt selbst verbindlich verbotene doppelte Implementierung).
Alle Abschnitte dieses Dokuments beziehen sich daher auf die **Win32/C++-
Linie**, nicht auf die im ursprünglichen Auftrag spezifizierte
C#/WPF-Architektur. Die dort verlangten 13 Dokumentationsdateien werden
nicht mechanisch für eine nicht gewählte Architektur erzeugt; dieses
Dokument und `ARCHITECTURE.md` fassen den realen Stand zusammen.

## Zweite wichtige Korrektur (Compiler-Anbindung)

Die vorgefundene IDE rief bisher `novalang.exe` auf
(`toolchain/novalang/`) – ein **separater, älterer NovaLang-Compiler mit
C-Transpiler-Backend**, der direkt unter Windows ausführbare `.exe`-
Dateien erzeugt (`DoRun()` rief sie per `ShellExecuteW` auf). Das ist
**nicht** die für NovaOS tatsächlich verwendete, boot-verifizierte
Toolchain – NovaOS-Programme müssen native x86-32-NEF-Dateien sein, die
unter NovaOS (nicht unter Windows) laufen.

Die reale, aktiv gepflegte und gegen den laufenden NovaOS-Kernel
boot-verifizierte Toolchain ist:

- `novalang/compiler/novac.exe` – direkter x86-32-Codegen-Compiler,
  Sprachversionen v0.1–v0.16 (siehe `novalang/specifications/`)
- `tools/nef/nef.exe` – NEF-1.0-Linker/Validator/Inspector

Nova Studio wurde daher umgewidmet: `DoBuild()`/`DoRun()`/`DoClean()`
in `src/main.cpp` rufen jetzt `novac.exe` + `nef.exe` auf (siehe
`ARCHITECTURE.md`). `toolchain/novalang/` und `novalang.exe` werden von
der IDE nicht mehr verwendet; sie bleiben als eigenständiger, separater
Code-Pfad im Repository bestehen (nicht Teil dieser Änderung, nicht
gelöscht), sollten aber nicht mit der echten NovaOS-Toolchain verwechselt
werden.

## Tatsächlich funktionsfähig (manuell + per CLI nachgewiesen)

| Bereich | Status | Nachweis |
|---|---|---|
| IDE startet unter Windows | ✅ | `nova-studio.exe` gestartet, blieb responsiv |
| Ribbonbar/Menüs (Datei/Bearbeiten/Ansicht/Build/Debug/Git) | ✅ (vorhanden) | `ribbon.cpp`, 1004 Zeilen |
| Editor (RichEdit, Syntax-Highlighting, Tabs, Suchen/Ersetzen, Zoom) | ✅ (vorhanden) | `editor.cpp`, `highlighter.cpp` |
| Explorer (TreeView, Datei öffnen per Doppelklick) | ✅ (vorhanden) | `explorer.cpp` |
| Dunkles/Helles Theme | ✅ (vorhanden) | `ThemeSetDark()`/`ThemeSetLight()` |
| **novac.exe läuft nativ unter Windows** | ✅ **verifiziert** | `novac hello_if.nova -o <praefix>` erzeugt `.code.bin`/`.rodata.bin`/`.data.bin` |
| **nef.exe link erzeugt gültige NEF-Datei** | ✅ **verifiziert** | `nef link` → `nef validate` meldet `GUELTIG`, `nef inspect` zeigt 3 Sections (CODE/RODATA/DATA), Entry 0x500000 |
| **NEF-Datei hat keine Windows-Laufzeitabhängigkeit** | ✅ **verifiziert** | Sections sind reiner Maschinencode/Daten, keine Imports |
| IDE-Build-Pipeline (`DoBuild`) ruft novac+nef echt auf | ✅ **neu implementiert** | siehe `DoBuild()`, nutzt exakt die von novac selbst vorgeschlagene `nef link`-Kommandozeile (keine erratenen Sektionslayouts) |
| Fehlerliste ("Probleme"-Panel) mit echter Zeilennavigation | ✅ **neu implementiert** | ListView, Doppelklick springt zu Datei+Zeile; parst reale `novac`-Fehlermeldungen (`Zeile N: ...`); speichert den vollen Pfad fuer die Navigation, zeigt nur den Dateinamen an |
| Projekt merkt sich echte Quelldateiliste (`<Sources>`) | ✅ **neu implementiert** | `OpenProject()` parst `<File path="...">`-Einträge aus `project.xml`; "Erstellen" baut bei offenem Projekt immer die Einstiegsdatei, nicht die zufällig aktive Registerkarte; Parser isoliert per Test-Programm nachgewiesen (zwei `<File>`-Einträge korrekt extrahiert) |
| `<Target>`-Angabe im Projekt-Template | ✅ **Fehler behoben** | war fälschlich `x86_64`, NovaOS/novac ist x86-32 - korrigiert (wurde vorher von keinem Code gelesen, war aber inhaltlich falsch) |
| **NEF Inspector** (minimaler echter Umfang) | ✅ **neu implementiert** | Öffnen einer `.nef`-Datei (Explorer-Doppelklick oder Datei-öffnen) ruft jetzt `nef.exe validate`+`inspect` auf und zeigt das Ergebnis im Ausgabe-Panel, statt die Binärdatei als Text zu laden (vorher: Byte-Müll im Editor, da `.nef` kein Text ist) |
| **Syscall Browser** | ✅ **neu implementiert** | Neues Panel (Ribbon → Extras → Syscall Browser) liest `kernel/newkernel/syscall.h` tatsächlich ein und zeigt jede `#define SYSCALL_*`/`GUI_OP_*`/`VFS_OP_*`/`INPUT_OP_*`-Konstante mit Name/Wert/Kommentar - keine erfundene Liste. Parser-Logik isoliert per Testprogramm gegen die echte Datei verifiziert: alle 53 Konstanten korrekt extrahiert, inkl. gleichzeiliger Kommentare (z. B. `SYSCALL_STATUS_CAP`) |
| **Explorer-Suchfeld (Live-Filter)** | ✅ **Fehler behoben** | War rein dekorativ: literaler Text "Suchen (Strg+;)" als Feldinhalt, kein `EN_CHANGE`-Handler, keine Filterwirkung überhaupt. Jetzt: echter Platzhalter (`EM_SETCUEBANNER`, verschwindet beim Tippen) + echte Live-Filterung (`ExplorerFilter()`, rekursiv, case-insensitiv, zeigt Treffer als flache Liste mit relativem Pfad). Logik isoliert per Testprogramm gegen ein reales Testverzeichnis mit Unterordner verifiziert: Treffer in Unterordnern korrekt gefunden, nicht-passende Abfrage liefert korrekt 0 Treffer |
| **Explorer-Kontextmenü (Rechtsklick)** | ✅ **neu implementiert** | Vorher kein Rechtsklick-Menü im Explorer. Jetzt: "Neue Datei..." (legt die Datei im ausgewählten Ordner an, öffnet sie, trägt sie bei offenem Projekt automatisch in `project.xml` ein), "Umbenennen..." (verschiebt per `MoveFileW`; aktualisiert automatisch jede offene Registerkarte UND die `project.xml`-Quellenliste/Einstiegsdatei auf den neuen Pfad - sonst würde ein späteres "Speichern" die Datei unter dem alten Namen neu anlegen), "Löschen" (verschiebt in den Papierkorb über `SHFileOperationW`+`FOF_ALLOWUNDO` - **kein** endgültiges Löschen), "Aktualisieren". Ordner sind bewusst NICHT umbenennbar/löschbar (kein rekursives Handling, um versehentlichen Datenverlust zu vermeiden). Alle drei schreibenden Aktionen isoliert per Testprogramm verifiziert (Umbenennen: `MoveFileW` korrekt, alter Pfad verschwindet, neuer existiert; Löschen: siehe unten). Die zuvor nur in `dialogs.cpp` vorhandene `DlgBuilder`-Infrastruktur wurde dafür nach `studio.h` verschoben, damit `explorer.cpp` sie mitnutzen kann, ohne eine zweite Kopie zu pflegen |
| **Dunkel-/Hell-Design für alle Werkzeug-Panels** | ✅ **Fehler behoben** | `EditorApplyTheme()` färbte bisher nur Editor und Ausgabe-Panel ein - die vier ListView-Panels (Probleme, Syscalls, Capabilities, Suche) blieben beim Dunkel-Design mit dem Standard-Weiß/Schwarz-Hintergrund zurück (optisch inkonsistent). Jetzt per `ListView_SetBkColor`/`SetTextColor`/`SetTextBkColor` korrekt eingefärbt, bei jedem Design-Wechsel. **Nicht visuell verifizierbar** in dieser Session (kein Desktop-GUI-Automatisierungswerkzeug) - nur per Code-Review gegen die bereits erprobte TreeView-Einfärbung (`ExplorerApplyTheme()`/Dokumentgliederung) als Muster bestätigt |
| **Projektweite Suche** | ✅ **neu implementiert** | Neues Panel (Ribbon → Bearbeiten → Im Projekt suchen, Strg+Shift+F) durchsucht rekursiv den Projektordner (.nova/.nlf/.nui/.xml/.md/.txt, versteckte Ordner wie `.git` werden übersprungen), zeigt Treffer mit Datei/Zeile/Text, Doppelklick navigiert. Bewusst NUR Suche, KEIN projektweites Ersetzen (mehrdateiübergreifend schreibend, höheres Risiko, zurückgestellt). Logik isoliert per Testprogramm gegen ein reales Testverzeichnis verifiziert: korrekte Treffer, korrekt case-insensitiv, `.git`-Ordner korrekt übersprungen |
| **Capability Manager** | ✅ **neu implementiert** | Neues Panel (Ribbon → Extras → Capability Manager) liest `kernel/newkernel/security.h` tatsächlich ein und zeigt jede `SECURITY_CAP_*`-Bitmaske mit Name, Hex-Wert und errechneter Bitposition - deckt bewusst nur das `security.c`-Flat-Bitmask-System ab, nicht die drei weiteren, separaten Capability-Konzepte (`cap_core.c`/`program.c`/`verify.c`, siehe `SDK_SPECIFICATION.md`). Parser-Logik isoliert per Testprogramm verifiziert: alle 21 Konstanten korrekt extrahiert, Bitpositionen 0-19 korrekt errechnet, die kombinierte Maske `SECURITY_KERNEL_CAPS` korrekt OHNE Einzelbit-Position angezeigt |
| **Projekt merkt sich neu angelegte Dateien** | ✅ **neu implementiert** | "Speichern als" innerhalb des Projektordners trägt die Datei automatisch in `project.xml`s `<Sources>` ein (`ProjectRegisterSource()`); Pfad-Relativierungslogik isoliert per Testprogramm verifiziert (Datei innerhalb Projekt → korrekter relativer Pfad, Datei außerhalb → korrekt leer/abgelehnt, Datei direkt im Projektwurzelordner → kein `./`-Präfix) |
| **Tab-Wechsel verwarf ungespeicherte Änderungen (3 Fundstellen)** | ✅ **schwerwiegender Fehler behoben** | Vorher wurde beim Wechseln der Registerkarte die Zieldatei IMMER neu von der Festplatte geladen - ungespeicherte Änderungen in der verlassenen Registerkarte gingen kommentarlos verloren. Betraf nicht nur den Tab-Leisten-Klick, sondern auch "Neue Datei" und das (Wieder-)Öffnen einer Datei über Explorer/Datei-öffnen/Probleme-Navigation - alle drei Stellen überschrieben den Editor, ohne vorher die bisherige Registerkarte zu sichern. Gemeinsamer Fix: `EditorTab::content`-Speicher-Spiegel + zentrale `EditorSyncActiveTabContent()`, die vor JEDEM Wechsel aufgerufen wird; kein Festplattenzugriff mehr beim bloßen Tab-Wechsel |
| **Erneutes Öffnen einer bereits offenen Datei erzeugte Duplikat-Tab** | ✅ **Fehler behoben** | Die vorhandene "schon offen?"-Prüfung in `EditorOpenFile()` wechselte zwar zur Registerkarte, lief aber danach weiter und legte trotzdem einen zweiten Tab für dieselbe Datei an (fehlendes `return`) |
| **"Alle speichern" speicherte nur die aktive Registerkarte** | ✅ **Fehler behoben** | Neue Funktion `EditorSaveAllFiles()` speichert jetzt tatsächlich jede modifizierte Registerkarte, nicht nur die gerade sichtbare |
| **"Projekt öffnen" war über keine UI erreichbar** | ✅ **Fehler behoben** | `IDM_FILE_OPEN_PROJECT` war vollständig implementiert, aber nur über eine nie angehängte klassische Menüleiste erreichbar gewesen (diese wurde komplett durch die Ribbonbar ersetzt und war toter Code). Jetzt als Ribbon-Button ("Projekt öffnen") + Strg+Umschalt+O verdrahtet; die tote Menüleiste wurde entfernt (0 Compiler-Warnungen jetzt statt vorher 2) |
| IDE startet, Build-Pipeline funktioniert end-to-end über die GUI (Klick-Test) | ⚠️ **nicht automatisiert verifiziert** | Kein Desktop-GUI-Automatisierungswerkzeug in dieser Session verfügbar; Pipeline wurde identisch per CLI nachgewiesen, die IDE ruft exakt dieselben Befehle auf |

## Ausdrücklich NICHT implementiert (ehrlich gekennzeichnet, nicht vorgetäuscht)

- **QEMU-Ausführung ("Ausführen")**: `DoRun()` meldet jetzt ehrlich, dass
  QEMU-Start/NovaFS-Transfer noch nicht implementiert sind, statt (wie
  vorher) eine NEF-Datei per `ShellExecuteW` als Windows-Programm zu
  "starten" (das hätte nie funktioniert, da eine NEF-Datei kein PE-Format
  hat).
- **Language Server / LSP**: nicht vorhanden. Es gibt aktuell weder in
  `novalang/compiler/` noch in `toolchain/novalang/` eine für LSP
  geeignete inkrementelle Analyse-API – `novac.exe` ist ein reiner
  Batch-Compiler (eine Quelldatei pro Aufruf, kein Daemon-Modus).
- **IntelliSense** (Autovervollständigung, Go to Definition, Hover,
  Rename, CodeLens, Inlay Hints, Refactoring): nicht vorhanden. Die
  vorhandene Syntax-Hervorhebung (`highlighter.cpp`) ist ein reiner
  Tokenizer ohne semantische Analyse oder Symboltabellen.
- **Debugger** (Breakpoints, Call Stack, Register, Speicheranzeige via
  QEMU/GDB-Stub): nicht vorhanden.
- **Projektsystem** (`.nproj`/`.nsln`, Referenzen, Build-Konfigurationen):
  `Project`-Struktur in `studio.h` enthält aktuell nur `name`/`rootPath`/
  `open` – kein persistentes Projektdateiformat, keine Referenzen, keine
  Konfigurationen.
- **Standardbibliotheks-Projekttyp, Bootstrap-Modus, Library Manager,
  SDK Manager**: nicht vorhanden.
- **Visueller `.nui`-Designer**: nicht vorhanden.
- **NEF Inspector**: minimal vorhanden (siehe Tabelle oben) - Öffnen
  einer `.nef`-Datei zeigt `validate`+`inspect`-Ausgabe im Ausgabe-Panel.
  Kein eigenes Baumdiagramm/Strukturansicht-Fenster, keine
  Signatur-/Capability-Detailansicht über das hinaus, was `nef inspect`
  textuell ausgibt.
- **Syscall Browser**: vorhanden (siehe Tabelle oben) - zeigt alle 53
  tatsächlichen `#define SYSCALL_*`/`GUI_OP_*`/`VFS_OP_*`/`INPUT_OP_*`-
  Konstanten aus `kernel/newkernel/syscall.h` mit Name, Wert und
  gleichzeiligem Kommentar.
- **Capability Manager**: vorhanden (siehe Tabelle oben), deckt aber
  nur `security.c`s Flat-Bitmask-System ab - NICHT `cap_core.c`
  (objektbasierte Capabilities mit Delegation/Revocation) oder
  `program.c`/`verify.c` (laut `security.h`-Dateikopf zwei weitere,
  separate Konzepte).
- **Ordner löschen/anlegen/umbenennen (Explorer)**: nicht vorhanden -
  "Neue Datei" legt nur Dateien an, "Löschen"/"Umbenennen" lehnen
  Ordner ausdrücklich ab (kein rekursives Handling ohne weitere
  Sicherheitsabfragen).
- **Projektweites Ersetzen**: nicht vorhanden - nur die projektweite
  Suche (siehe Tabelle oben) ist implementiert. Mehrdateiübergreifendes
  Schreiben ist riskanter (potenzieller Datenverlust bei einem Fehler
  in mehreren Dateien gleichzeitig) und wurde bewusst zurückgestellt.
- **Object Browser, IR Viewer, Assembly Viewer, ABI Validator,
  Dependency Viewer**: nicht vorhanden.
- **Windows-Installer, portable Version**: nicht vorhanden (nur das
  statisch gelinkte `nova-studio.exe` selbst, das bereits ohne
  MSYS2-Laufzeit-DLLs lauffähig ist).
- **Git-Integration**: Menüeinträge/Ribbon-Platzhalter existieren
  (`IDM_GIT_*`), sind aber nicht mit echter Git-Funktionalität verbunden.
  Der neu hinzugefügte "Git"-Tab im unteren Panel zeigt jetzt explizit
  "noch nicht implementiert" an, statt eine leere Fläche zu zeigen.
- **"Terminal"-Tab** im unteren Panel: ebenfalls jetzt explizit als
  "noch nicht implementiert" markiert (vorher ein funktionsloser
  Platzhalter-Tab ohne jeden Hinweis).

## Abnahmetests (Abschnitt 25 des Auftrags) – ehrlicher Status

| # | Test | Status |
|---|---|---|
| 1 | IDE startet unter Windows | ✅ |
| 2 | Ribbon vollständig bedienbar | ⚠️ vorhanden, nicht Klick-für-Klick durchgetestet (kein GUI-Automatisierungswerkzeug) |
| 3 | Neues Projekt erstellbar | ⚠️ Ordner-basiert mit echtem `project.xml` (Name/Build/Sources), kein separates `.nproj`-Dateiformat wie im ursprünglichen Auftrag benannt |
| 4 | Dateien bearbeiten/speichern | ✅ (vorhanden, nicht neu) |
| 5 | IntelliSense erkennt echte Symbole | ❌ nicht implementiert |
| 6 | Syntax-/Semantikfehler live angezeigt | ⚠️ nur Highlighter-Tokenfehler live; echte Compiler-Diagnostik erst nach "Erstellen" |
| 7 | Compiler läuft nativ unter Windows | ✅ **verifiziert** |
| 8 | Programm für NovaOS kompiliert (unter Windows) | ✅ **verifiziert** |
| 9 | Linker erzeugt gültige NEF-Datei | ✅ **verifiziert** |
| 10 | NEF enthält nativen x86-32-Code | ✅ **verifiziert** |
| 11 | NEF ohne Windows-Laufzeitabhängigkeiten | ✅ **verifiziert** |
| 12 | Programm läuft unter NovaOS | ❌ nicht automatisiert (QEMU-Integration fehlt) |
| 13–14 | Statische/dynamische Bibliothek erstellbar+einbindbar | ❌ nicht implementiert |
| 15 | Standardbibliothek im Bootstrap-Modus kompilierbar | ❌ nicht implementiert |
| 16 | IntelliSense erkennt neue Bibliotheksfunktionen | ❌ nicht implementiert |
| 17 | Projektübergreifende Referenzen | ❌ nicht implementiert (und durch die Compiler-Einschränkung "eine Quelldatei pro Aufruf" aktuell auch nicht sinnvoll möglich, siehe unten) |
| 18 | Compilerfehler mit korrekter Position, Navigation per Klick | ✅ **neu implementiert** (Datei+Zeile; keine Spalte/Fehlercode, da `novac.exe` diese nicht liefert) |
| 19 | Breakpoints/Debugging | ❌ nicht implementiert |
| 20 | Automatisierte Unit-/Integrationstests | ❌ nicht implementiert |
| 21–22 | Portable Version / Installer | ❌ nicht implementiert |

## Wichtige Compiler-Einschränkung (betrifft das Projektsystem direkt)

`novac.exe` kompiliert **genau eine Quelldatei pro Aufruf**
(`novac <quelle.nova> -o <praefix>` - kein zweiter Dateiparameter
möglich, per CLI-Test verifiziert). Es gibt noch KEINE Mehrdateien-/
Modul-übergreifende Verlinkung im Compiler-Frontend (kein Cross-File-
`Imports`-Auflösung, kein Multi-Object-Link). `Project::sources` (siehe
`studio.h`) kann daher zwar mehrere `.nova`-Dateien eines Projekts
nachverfolgen (für Explorer/zukünftige Verwendung), aber `DoBuild()`
übersetzt immer nur `Project::entryFile` (die erste `<Sources><File>`-
Eintragung). Ein echtes Mehrdatei-Projekt mit internen Modulreferenzen
ist erst sinnvoll, wenn der NovaLang-Compiler selbst diese Fähigkeit
bekommt - das ist eine Erweiterung des Compilers, kein IDE-Feature, und
wurde in dieser Session nicht begonnen.

## Nächste sinnvolle Schritte (nicht in dieser Session begonnen)

1. Persistentes Projektformat (`.nproj`, mehrere Quelldateien statt nur
   der aktiven Tab-Datei).
2. QEMU-Integration: NEF-Transfer in ein isoliertes NovaFS-Testimage
   (Vorsicht: siehe `feedback_novafs_host_tool_write_bug` – der
   vorhandene NovaFS-Host-Schreibwerkzeug hat einen bekannten Fehler,
   erzeugte Dateien sind für den Kernel nicht sichtbar) + Boot + serielles
   Log im Ausgabe-Panel.
3. `novac.exe` um einen "Analyze"/"Check"-Modus erweitern, der
   Datei+Zeile+Spalte+Fehlercode für mehrere Fehler in einem Durchlauf
   liefert (aktuell bricht der Compiler beim ersten Fehler ab) – Grundlage
   für echtes IntelliSense/Live-Diagnostik ohne einen zweiten,
   unabhängigen Compiler zu bauen.
