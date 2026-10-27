# NPSPEC-WINDOW-LAYOUT-0003 – Nova Window Groups

**Status:** Angenommen  
**Bereich:** Window Manager / Fenstergruppen  
**System:** NovaOS

## 1. Zweck

Nova Window Groups definiert die logische Zusammenfassung mehrerer Fenster zu gemeinsam verwaltbaren Gruppen.

Ziel ist eine übersichtliche Organisation zusammengehöriger Fenster, ohne deren Eigenständigkeit oder die klassische Fensterverwaltung einzuschränken.

## 2. Architektur

- Zentrale Gruppenverwaltung durch den Nova Window Manager.
- Trennung zwischen Fenstergruppen, Fensterhierarchien und Tiling-Layouts.
- Unterstützung anwendungsübergreifender Fenstergruppen.
- Integration in Window Positioning, Focus Management und Z-Order Management.
- Unterstützung gemeinsamer Fensteroperationen.
- Verwaltung pro Workspace oder workspaceübergreifend.
- Capability-basierte Autorisierung.
- Optionale Persistenz und Wiederherstellung von Gruppen.

## 3. Gruppenmodell

NovaOS unterstützt folgende Gruppentypen:

| Gruppentyp | Beschreibung |
|---|---|
| Manual Group | Manuell zusammengestellte Fenster |
| Application Group | Zusammengehörige Fenster einer Anwendung |
| Task Group | Fenster unterschiedlicher Anwendungen für eine gemeinsame Aufgabe |
| Layout Group | Fenster mit gemeinsamem Layout |
| Temporary Group | Temporäre Zusammenfassung von Fenstern |

Eine Fenstergruppe ist ein logisches Verwaltungsobjekt und besitzt nicht zwangsläufig eine eigene grafische Surface.

Gruppenzugehörigkeit begründet weder Fensterbesitz noch zusätzliche Zugriffsrechte.

## 4. Kernfunktionen

- Erstellen und Auflösen von Fenstergruppen.
- Hinzufügen und Entfernen einzelner Fenster.
- Gemeinsames Verschieben und Anordnen.
- Gemeinsames Minimieren und Wiederherstellen.
- Gruppenbezogener Fokuswechsel.
- Gemeinsame Z-Order-Operationen.
- Integration in Taskbar und Fensterübersicht.
- Unterstützung gruppenbezogener Snap- und Tiling-Layouts.
- Speicherung benutzerdefinierter Gruppen.
- Wiederherstellung gespeicherter Gruppenkonfigurationen.

## 5. Normative Anforderungen

- WINDOW-001: Fenstergruppen MÜSSEN zentral durch den Window Manager verwaltet werden.
- WINDOW-002: Jede Fenstergruppe MUSS eine eindeutige Identität besitzen.
- WINDOW-003: Gruppenmitgliedschaften MÜSSEN unabhängig vom Fensterbesitz verwaltet werden.
- WINDOW-004: Anwendungen DÜRFEN fremde Fenster nur mit entsprechender Berechtigung gruppieren.
- WINDOW-005: Gruppenoperationen DÜRFEN keine Sicherheitsgrenzen umgehen.
- WINDOW-006: Fenster MÜSSEN unabhängig von ihrer Gruppenzugehörigkeit einzeln bedienbar bleiben.
- WINDOW-007: Gruppenänderungen MÜSSEN atomar erfolgen.
- WINDOW-008: Gruppenoperationen MÜSSEN die individuellen Fensterbeschränkungen berücksichtigen.
- WINDOW-009: Das Entfernen eines Fensters MUSS dessen Gruppenmitgliedschaften automatisch bereinigen.
- WINDOW-010: Das Auflösen einer Gruppe DARF die enthaltenen Fenster nicht automatisch zerstören.
- WINDOW-011: Gruppenbezogene Fokusänderungen MÜSSEN die geltenden Fokusregeln einhalten.
- WINDOW-012: Gruppenbezogene Z-Order-Änderungen MÜSSEN bestehende Fensterebenen respektieren.
- WINDOW-013: Modale Fenster MÜSSEN ihre Modalitätsregeln unabhängig von der Gruppenzugehörigkeit beibehalten.
- WINDOW-014: Gruppen MÜSSEN bei Display- und Workspace-Änderungen konsistent bleiben.
- WINDOW-015: Benutzerdefinierte Gruppen SOLLEN persistent gespeichert werden können.
- WINDOW-016: Fenstergruppen MÜSSEN ohne KI-Abhängigkeit deterministisch funktionieren.

## 6. Gruppendaten

Jede Fenstergruppe besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Group ID | Eindeutige Gruppenidentität |
| Group Name | Optionaler Gruppenname |
| Group Type | Art der Fenstergruppe |
| Owner Context | Sicherheitskontext des Gruppenerstellers |
| Window Set | Zugeordnete Fenster |
| Group Policy | Zulässige gemeinsame Operationen |
| Layout Reference | Optionales Gruppenlayout |
| Workspace Scope | Zugeordnete Workspaces |
| Group Version | Version der Gruppenkonfiguration |

Fensteridentitäten MÜSSEN bei der Wiederherstellung anhand gültiger Fensterreferenzen aufgelöst werden.

## 7. Gruppenoperationen

### Group Move

Mehrere Fenster werden gemeinsam verschoben.

Die relativen Fensterpositionen SOLLEN erhalten bleiben.

### Group Minimize

Alle zulässigen Gruppenfenster werden gemeinsam minimiert.

### Group Restore

Minimierte Gruppenfenster werden entsprechend ihren vorherigen Darstellungsmodi wiederhergestellt.

### Group Arrange

Fenster werden anhand eines definierten Layouts angeordnet.

### Group Activate

Ein geeignetes Fenster der Gruppe wird gemäß den Fokusregeln aktiviert.

### Group Dissolve

Die Gruppenbeziehung wird entfernt, während die enthaltenen Fenster bestehen bleiben.

Gemeinsame Operationen MÜSSEN vor ihrer Ausführung auf Berechtigungen und individuelle Fensterbeschränkungen geprüft werden.

## 8. Gruppenverarbeitung

1. **Request:** Gruppenoperation empfangen.
2. **Authorize:** Berechtigungen prüfen.
3. **Resolve:** Betroffene Fenster bestimmen.
4. **Validate:** Fensterzustände und Gruppenregeln prüfen.
5. **Calculate:** Erforderliche Zustands- und Geometrieänderungen berechnen.
6. **Commit:** Gruppenoperation atomar übernehmen.
7. **Notify:** Betroffene Komponenten informieren.
8. **Persist:** Gruppenkonfiguration bei Bedarf speichern.

Kann eine gemeinsame Operation nicht vollständig ausgeführt werden, MUSS sie entweder zurückgewiesen oder nach einer ausdrücklich definierten Teiloperationsregel verarbeitet werden.

## 9. Fehlerbehandlung und Wiederherstellung

- Ungültige Fensterreferenzen werden aus Gruppen entfernt.
- Fehlgeschlagene Gruppenänderungen hinterlassen keine inkonsistenten Mitgliedschaften.
- Anwendungsabstürze führen zur Bereinigung betroffener Gruppenreferenzen.
- Bei Displayverlust werden Gruppenlayouts neu validiert.
- Nicht reagierende Anwendungen blockieren keine Gruppenverwaltung.
- Gespeicherte Gruppen werden nur mit gültigen und autorisierten Fensterreferenzen wiederhergestellt.
- Ein Compositor-Neustart verändert bestehende logische Fenstergruppen nicht.
- Gruppenoperationen dürfen geschützte Systemfenster nicht unberechtigt beeinflussen.

## 10. Ergebnis

NovaOS erhält eine flexible Verwaltung logisch zusammengehöriger Fenster.

Fenster unterschiedlicher Anwendungen können zu gemeinsamen Arbeitsgruppen zusammengefasst, angeordnet und verwaltet werden.

Die einzelnen Fenster behalten dabei ihre Eigenständigkeit, Sicherheitsgrenzen und individuellen Zustandsregeln.

**Grundsatz:** Fenstergruppen organisieren die Arbeit des Benutzers, ohne die enthaltenen Fenster technisch oder sicherheitsbezogen voneinander abhängig zu machen.