# NPSPEC-WINDOW-DECORATION-0004 – Nova Window Controls

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterbedienelemente  
**System:** NovaOS

## 1. Zweck

Nova Window Controls definiert die standardisierten Bedienelemente zur Steuerung von Fenstern innerhalb von NovaOS.

Ziel ist eine einheitliche, intuitive und anpassbare Fenstersteuerung, die unabhängig vom verwendeten Theme oder Dekorationsstil zuverlässig funktioniert.

## 2. Architektur

- Verwaltung der Fensteroperationen durch den Nova Window Manager.
- Darstellung der Bedienelemente über Nova UI und den Compositor.
- Integration in Nova Window Decorations und Nova Title Bars.
- Trennung zwischen visuellen Controls und ausführenden Fensteroperationen.
- Zustandsabhängige Aktivierung und Darstellung.
- Unterstützung von Maus, Touch und Tastatur.
- Deklarative Gestaltung über Styles und Templates.
- Capability-basierte Autorisierung von Fensteroperationen.

## 3. Standardbedienelemente

NovaOS definiert folgende Fensterbedienelemente:

| Control | Funktion |
|---|---|
| Minimize | Fenster minimieren |
| Maximize | Fenster maximieren |
| Restore | Vorherigen Fensterzustand wiederherstellen |
| Close | Fenster schließen |
| System Menu | Verfügbare Fensteroperationen anzeigen |

Die Standardanordnung lautet:

**Minimize → Maximize/Restore → Close**

Die Position und Gestaltung DÜRFEN durch Benutzerpräferenzen oder Themes angepasst werden.

## 4. Control-Modell

Jedes Fensterbedienelement besitzt folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Control ID | Eindeutige Identität innerhalb der Dekoration |
| Action | Zugeordnete Fensteroperation |
| State | Aktueller Interaktionszustand |
| Enabled | Verfügbarkeit der Operation |
| Visible | Sichtbarkeit des Controls |
| Style | Visuelle Gestaltung |
| Hit Region | Interaktiver Eingabebereich |
| Accessibility Label | Zugängliche Funktionsbezeichnung |

Die Verfügbarkeit wird aus den Eigenschaften, dem Zustand und den Berechtigungen des Fensters abgeleitet.

## 5. Normative Anforderungen

- WINDOW-001: NovaOS MUSS standardisierte Fensterbedienelemente bereitstellen.
- WINDOW-002: Reguläre Fenster MÜSSEN standardmäßig die zulässigen Bedienelemente erhalten.
- WINDOW-003: Fensteroperationen MÜSSEN durch den Window Manager ausgeführt werden.
- WINDOW-004: Controls DÜRFEN Fensterzustände nicht unmittelbar verändern.
- WINDOW-005: Die Verfügbarkeit eines Controls MUSS den aktuellen Fensterzustand berücksichtigen.
- WINDOW-006: Nicht zulässige Operationen MÜSSEN deaktiviert oder ausgeblendet werden.
- WINDOW-007: Maximize und Restore MÜSSEN entsprechend dem Fensterzustand wechseln.
- WINDOW-008: Das Close-Control MUSS eine reguläre Schließanforderung auslösen.
- WINDOW-009: Fensterbedienelemente MÜSSEN DPI-unabhängig skalierbar sein.
- WINDOW-010: Controls MÜSSEN mit Maus, Touch und Tastatur bedienbar sein.
- WINDOW-011: Interaktive Bereiche MÜSSEN eindeutig definiert sein.
- WINDOW-012: Themes DÜRFEN die Semantik der Standardoperationen nicht verändern.
- WINDOW-013: Anwendungen DÜRFEN Standardcontrols visuell ersetzen, jedoch keine Sicherheitsprüfungen umgehen.
- WINDOW-014: Geschützte Systemfenster MÜSSEN ihre vorgeschriebenen Bedienregeln beibehalten.
- WINDOW-015: Controls MÜSSEN auch ohne Animationen und visuelle Effekte funktionieren.
- WINDOW-016: Die Fenstersteuerung MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 6. Interaktionszustände

Jedes Control unterstützt folgende Zustände:

| Zustand | Beschreibung |
|---|---|
| Normal | Standarddarstellung |
| Hover | Zeiger befindet sich über dem Control |
| Pressed | Control wird betätigt |
| Disabled | Operation nicht verfügbar |
| Focused | Control besitzt Tastaturfokus |

Zustandsänderungen werden durch das Nova Input System erfasst und über Nova UI dargestellt.

Die Ausführung einer Fensteroperation erfolgt erst nach einer gültigen Aktivierung des Controls.

## 7. Fensteroperationen

### Minimize

- Löst eine Minimierungsanforderung aus.
- Übergibt die Verarbeitung an den Window Manager.
- Berücksichtigt die geltenden Fenster- und Modalitätsregeln.

### Maximize

- Maximiert das Fenster innerhalb des gültigen Arbeitsbereichs.
- Speichert die erforderliche Wiederherstellungsgeometrie.
- Wechselt anschließend zur Restore-Funktion.

### Restore

- Stellt den vorherigen zulässigen Darstellungsmodus wieder her.
- Berücksichtigt Änderungen der Displaykonfiguration.
- Verwendet eine gültige Wiederherstellungsgeometrie.

### Close

- Sendet eine reguläre Schließanforderung.
- Ermöglicht der Anwendung die vorgesehene Behandlung der Anforderung.
- Erzwingt keine unmittelbare Zerstörung des Fensters.

### System Menu

- Zeigt die aktuell verfügbaren Fensteroperationen.
- Berücksichtigt Fensterzustand, Berechtigungen und Benutzerpräferenzen.

## 8. Integration in Nova UI

Fensterbedienelemente werden als wiederverwendbare Nova UI-Controls bereitgestellt.

Unterstützt werden:

- Austauschbare Control Templates.
- Themeabhängige Styles.
- Zustandsabhängige Symbole.
- Anpassbare Größen und Abstände.
- Datenbindung an den Fensterzustand.
- Barrierefreie Beschriftungen.
- Benutzerdefinierte visuelle Effekte.

Die funktionale Bedeutung der Standardcontrols bleibt unabhängig vom verwendeten Template erhalten.

## 9. Eingabe und Bedienbarkeit

- Mausinteraktionen verwenden definierte Hit-Testing-Bereiche.
- Touch-Bedienung DARF vergrößerte Eingabebereiche verwenden.
- Tastaturbedienung MUSS über definierte Fensterbefehle möglich sein.
- Standardcontrols SOLLEN eindeutige visuelle Rückmeldungen liefern.
- Bedienflächen MÜSSEN auch bei hohen DPI-Skalierungen zuverlässig erreichbar sein.
- Benutzerdefinierte Titelleisten DÜRFEN vorgeschriebene Systemfunktionen nicht unzugänglich machen.

## 10. Fehlerbehandlung und Performance

- Ungültige Control Templates werden durch sichere Standardimplementierungen ersetzt.
- Fehlgeschlagene Fensteroperationen hinterlassen den bisherigen gültigen Fensterzustand.
- Nicht reagierende Anwendungen dürfen systemseitige Fensterbedienelemente nicht blockieren.
- Änderungen einzelner Controls SOLLEN nur betroffene Darstellungsbereiche aktualisieren.
- Animationen DÜRFEN die Ausführung von Fensteroperationen nicht verzögern.
- Ein Compositor-Neustart verändert die logischen Fensterzustände nicht.
- Nicht verfügbare visuelle Effekte werden automatisch deaktiviert.

## 11. Ergebnis

NovaOS erhält ein einheitliches und erweiterbares System zur Bedienung von Fenstern.

Standardoperationen wie Minimieren, Maximieren, Wiederherstellen und Schließen bleiben unabhängig von Themes, Titelleisten und individuellen Dekorationen konsistent.

Durch die Trennung von Darstellung und Fensteroperationen können Anwendungen ihre Bedienelemente frei gestalten, ohne die Sicherheit oder Zuverlässigkeit der Fensterverwaltung zu beeinträchtigen.

**Grundsatz:** Fensterbedienelemente dürfen unterschiedlich aussehen, müssen sich aber jederzeit vertraut, eindeutig und zuverlässig bedienen lassen.