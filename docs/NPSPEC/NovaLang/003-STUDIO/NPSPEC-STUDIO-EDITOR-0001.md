
# NPSPEC-STUDIO-EDITOR-0001 – NovaLang Studio Code Editor

## Status

Angenommen

## Kategorie

NovaLang Studio / Code Editor

## Zweck

Definiert den integrierten Quellcode-Editor von NovaLang Studio.

Ziel ist eine schnelle, übersichtliche und leistungsfähige Bearbeitung von NovaLang-Code mit direkter Integration in Compiler, Language Service, Debugger und Logic Graph.

Der Editor soll häufig benötigte Funktionen unmittelbar zugänglich machen und auch bei großen Dateien ressourcenschonend arbeiten.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Editor Core | Textbearbeitung und Dokumentzustand |
| Text Buffer | Effiziente Verwaltung des Textinhalts |
| Syntax Engine | Syntaxhervorhebung und Strukturinformationen |
| Language Service Client | IntelliSense und semantische Analyse |
| Navigation Engine | Symbole, Definitionen und Referenzen |
| Diagnostic Renderer | Darstellung von Fehlern und Warnungen |
| Debug Integration | Breakpoints und Ausführungsanzeige |
| Editor View | Darstellung, Eingabe und Interaktion |

Der Editor verwendet das gemeinsame Dokumentmodell des Workspace Managers.

## Unterstützte Dateitypen

| Dateityp | Verwendung |
|---|---|
| `.nova` | Allgemeiner NovaLang-Quellcode |
| `.nlf` | NovaLang-Code für Logic-Graph-Komponenten |
| `.nui` | Deklarative NovaLang-Benutzeroberflächen |
| `.xml` | Projekt- und Solution-Konfiguration |
| `.md` | Dokumentation |

Weitere Dateitypen können über Erweiterungen unterstützt werden.

`.nova`, `.nlf` und `.nui` verwenden dieselbe zugrunde liegende NovaLang-Sprachdefinition.

## Grundfunktionen

Der Editor unterstützt:

- Syntaxhervorhebung
- Automatische Einrückung
- Zeilennummern
- Code Folding
- Suchen und Ersetzen
- Mehrfachcursor und Mehrfachauswahl
- Undo und Redo
- Automatische Klammerergänzung
- Kommentieren und Auskommentieren
- Formatierung von Code
- Tastenkombinationen
- Geteilte Editoransichten

Alle grundlegenden Funktionen müssen ohne KI verfügbar sein.

## IntelliSense

Der Language Service stellt bereit:

- Kontextabhängige Autovervollständigung
- Anzeige von Funktionssignaturen
- Parameterinformationen
- Typinformationen
- Dokumentationshinweise
- Importvorschläge
- Erkennung verfügbarer Symbole
- Vorschläge für sichere Codekorrekturen

Vorschläge müssen die tatsächlichen NovaLang-Typregeln berücksichtigen.

## Navigation

Der Editor unterstützt:

- Go to Definition
- Find All References
- Go to Implementation
- Dokument- und Workspace-Symbolsuche
- Breadcrumb-Navigation
- Navigation zwischen Fehlern
- Wechsel zwischen Code und zugehörigen Logic-Graph-Knoten

Die Navigation muss projektübergreifend funktionieren, sofern die betreffenden Ressourcen zugänglich sind.

## Diagnostik

Compiler- und Language-Service-Diagnosen werden direkt im Quellcode dargestellt.

- Fehler durch eindeutige Markierungen
- Warnungen und Hinweise
- Beschreibung und Fehlercode
- Korrekturvorschläge
- Navigation zur Fehlerposition
- Inkrementelle Aktualisierung

Diagnosen dürfen die Texteingabe nicht blockieren.

## Debugging

Der Editor integriert:

- Setzen und Entfernen von Breakpoints
- Bedingte Breakpoints
- Hervorhebung der aktuellen Ausführungsposition
- Step Into, Step Over und Step Out
- Anzeige von Variablenwerten
- Watch-Ausdrücke
- Call Stack und Async Stack

Die Debugging-Funktionen verwenden das gemeinsame NovaLang-Debug-Protokoll.

## Refactoring

Der Editor unterstützt mindestens:

- Rename Symbol
- Extract Method
- Extract Variable
- Organize Imports
- Format Document
- Find and Replace Symbol References

Semantische Refactorings müssen auf geprüften Symbol- und Typinformationen beruhen.

Projektübergreifende Änderungen müssen vor ihrer Übernahme überprüfbar sein.

## Logic-Graph-Integration

Custom Scripts können direkt aus dem Logic Graph geöffnet werden.

Dabei gilt:

- Der zugehörige Graph-Knoten bleibt eindeutig identifizierbar.
- Ein- und Ausgänge werden anhand ihrer Typverträge angezeigt.
- Codeänderungen aktualisieren die relevanten Analyseinformationen.
- Ungültige Verbindungen werden diagnostiziert.
- Der Editor darf keine zusätzlichen Capability-Berechtigungen erzeugen.

Der Wechsel zwischen Code Editor und Logic Graph darf keine ungespeicherten Änderungen verlieren.

## UI-Designer-Integration

Bei `.nui`-Dateien unterstützt der Editor:

- Deklarative Syntaxhervorhebung
- Autovervollständigung von UI-Typen und Eigenschaften
- Validierung von Datenbindungen
- Navigation zu gebundenen Symbolen
- Synchronisation mit dem visuellen UI Designer
- Vorschau von Änderungen

Visuelle Bearbeitung und Quellcodebearbeitung müssen auf demselben Dokumentzustand basieren.

## Benutzeroberfläche

Der Editor verwendet die NovaOS-Designsprache.

- Ruhige, übersichtliche Darstellung
- Fluent-inspirierte Gestaltung
- Anpassbare Schriftgröße und Farbschemata
- Direkt erreichbare Standardfunktionen
- Kontextabhängige Werkzeugleisten
- Frei konfigurierbare Tastenkombinationen
- Unterstützung für mehrere Editorbereiche

Kontextabhängige Bedienelemente dürfen die Textdarstellung nicht unerwartet verschieben.

## Performance

- Große Dateien müssen effizient bearbeitet werden können.
- Syntaxanalyse und Darstellung erfolgen inkrementell.
- Sichtbare Textbereiche werden bevorzugt gerendert.
- Aufwendige Analysen laufen im Hintergrund.
- Eingaben dürfen nicht durch Compileroperationen blockiert werden.
- Speicherverbrauch und Analyseaufgaben müssen begrenzbar sein.
- Nicht benötigte Editorfunktionen sollen erst bei Bedarf geladen werden.

## Zuverlässigkeit

- Ungespeicherte Änderungen müssen wiederherstellbar sein.
- Externe Dateiänderungen müssen erkannt werden.
- Gleichzeitige Bearbeitungen dürfen nicht unbemerkt überschrieben werden.
- Fehlgeschlagene Formatierungen oder Refactorings müssen rückgängig gemacht werden können.
- Dokumentänderungen müssen konsistent mit dem Workspace Manager synchronisiert werden.

## Sicherheit

- Der Editor führt Quellcode nicht automatisch aus.
- Codeausführung und Debugging benötigen entsprechende Autorisierung.
- Erweiterungen unterliegen dem NovaOS-Capability-Modell.
- Externe Dateien werden als potenziell nicht vertrauenswürdig behandelt.
- IntelliSense und Codeanalyse dürfen keine zusätzlichen Systemberechtigungen anfordern.
- KI-generierte Änderungen müssen vor ihrer Übernahme überprüfbar bleiben.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten, eigenständigen Code Editor bereitstellen.
2. Der Editor MUSS `.nova`, `.nlf` und `.nui` unterstützen.
3. Alle NovaLang-Dateitypen MÜSSEN dieselbe Sprachdefinition verwenden.
4. Syntaxhervorhebung, Autovervollständigung und Fehlerdiagnostik MÜSSEN verfügbar sein.
5. Navigation und Refactoring MÜSSEN semantische Sprachinformationen verwenden.
6. Der Editor MUSS mit Workspace, Logic Graph und UI Designer integriert sein.
7. Dokumentänderungen MÜSSEN konsistent synchronisiert werden.
8. Ungespeicherte Änderungen MÜSSEN gegen Datenverlust geschützt werden.
9. Große Dateien MÜSSEN inkrementell und ressourcenschonend verarbeitet werden.
10. Compiler- und Analyseaufgaben DÜRFEN die Texteingabe nicht blockieren.
11. Der Editor DARF keine Capability- oder Sandbox-Grenzen umgehen.
12. Sämtliche grundlegenden Bearbeitungsfunktionen MÜSSEN ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält einen schnellen, modularen und vollständig integrierten Code Editor mit IntelliSense, Refactoring, Debugging und direkter Verbindung zu Logic Graph und UI Designer.

Der Editor verbindet die Leistungsfähigkeit einer modernen Entwicklungsumgebung mit der einfachen, unmittelbaren Bedienbarkeit von NovaOS.
