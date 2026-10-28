# NPSPEC-SHELL-TASKBAR-0003 – Nova Taskbar Grouping

**Status:** Angenommen  
**Bereich:** Shell / Taskbar / Grouping  
**System:** NovaOS

## 1. Zweck

Nova Taskbar Grouping definiert die Gruppierung zusammengehöriger Anwendungen und Fenster innerhalb der NovaOS-Taskleiste.

Ziel ist eine übersichtliche, platzsparende und individuell anpassbare Darstellung laufender Anwendungen.

Die Gruppierung MUSS unabhängig von der eigentlichen Prozess- und Fensterverwaltung erfolgen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Taskbar Group Manager | Verwaltung der Fenstergruppen |
| Group Resolver | Ermittlung zusammengehöriger Fenster |
| Group Policy Manager | Anwendung der Gruppierungsregeln |
| Group View | Darstellung gruppierter Elemente |
| Group Interaction Controller | Navigation und Aktionen innerhalb einer Gruppe |
| Group State Manager | Speicherung der Gruppierungseinstellungen |

Die Fensterinformationen werden über das Nova Taskbar Application Management und den Nova Window Manager bezogen.

## 3. Gruppierungsmodell

Eine Taskbar-Gruppe besitzt mindestens:

- Eindeutige Gruppenidentität.
- Zugehörige Anwendungsidentität.
- Liste zugeordneter Fenster.
- Anzeigename und Symbol.
- Gruppierungsmodus.
- Aktuellen Gruppenstatus.
- Optionalen Workspace- und Monitorbezug.

Dabei gilt:

- Ein Fenster DARF innerhalb derselben Taskleistenansicht nur einer Gruppe zugeordnet sein.
- Gruppen MÜSSEN dynamisch erstellt und aufgelöst werden können.
- Fenster MÜSSEN unabhängig von ihrer Gruppierung erreichbar bleiben.
- Gruppen DÜRFEN die Identität oder den Lebenszyklus der zugeordneten Fenster nicht verändern.

## 4. Gruppierungsmodi

NovaOS unterstützt folgende Modi:

| Modus | Verhalten |
|---|---|
| Always | Fenster derselben Anwendung werden immer gruppiert |
| When Full | Gruppierung erfolgt bei Platzmangel |
| Never | Fenster werden einzeln dargestellt |
| Custom | Gruppierung erfolgt anhand benutzerdefinierter Regeln |

Dabei gilt:

- Der Benutzer MUSS den Gruppierungsmodus auswählen können.
- Gruppierungsregeln MÜSSEN pro Taskleiste konfigurierbar sein.
- Anwendungsspezifische Ausnahmen SOLLEN unterstützt werden.
- Änderungen MÜSSEN ohne Neustart der Desktop Shell wirksam werden.

## 5. Gruppierungsregeln

Der Group Resolver ermittelt die Gruppenzugehörigkeit anhand definierter Eigenschaften.

Berücksichtigt werden können:

- Anwendungsidentität.
- Fensterzugehörigkeit.
- Anwendungsinstanz.
- Workspace.
- Monitor.
- Benutzerdefinierte Gruppierungsregeln.

Dabei gilt:

- Die Anwendungsidentität MUSS das primäre Gruppierungskriterium sein.
- Prozessnamen und Fenstertitel DÜRFEN nicht als alleinige Identitätsgrundlage verwendet werden.
- Gruppierungsentscheidungen MÜSSEN deterministisch erfolgen.
- Benutzerdefinierte Regeln DÜRFEN Sicherheits- oder Sitzungsgrenzen nicht umgehen.

## 6. Gruppendarstellung

Eine Fenstergruppe wird durch ein gemeinsames Taskbar-Element dargestellt.

Die Darstellung unterstützt:

- Anwendungssymbol.
- Anzahl geöffneter Fenster.
- Aktiven Fensterzustand.
- Aufmerksamkeitsindikatoren.
- Fortschrittsanzeigen.
- Fensterübersicht.
- Optionale Fenstervorschauen.

Dabei gilt:

- Der Zustand einer Gruppe MUSS aus den Zuständen ihrer Fenster abgeleitet werden.
- Aktive Fenster MÜSSEN eindeutig erkennbar sein.
- Gruppen MÜSSEN auch ohne Animationen vollständig bedienbar sein.
- Die Darstellung MUSS das Nova UI Framework verwenden.

## 7. Gruppeninteraktion

NovaOS unterstützt folgende Aktionen:

- Gruppe aktivieren.
- Fenster innerhalb einer Gruppe auswählen.
- Zwischen Gruppenfenstern wechseln.
- Fensterübersicht öffnen.
- Einzelne Fenster schließen.
- Gesamte Anwendung schließen, sofern unterstützt.
- Gruppierung ändern.
- Kontextmenü öffnen.

Dabei gilt:

- Die Aktivierung eines Gruppenfensters MUSS über den Nova Window Manager erfolgen.
- Einzelne Fenster MÜSSEN direkt erreichbar bleiben.
- Das Schließen einer Gruppe DARF nicht automatisch alle Fenster beenden.
- Aktionen auf mehrere Fenster MÜSSEN eindeutig als solche erkennbar sein.

## 8. Fensterübersicht und Vorschauen

Gruppierte Fenster SOLLEN über eine gemeinsame Fensterübersicht erreichbar sein.

Dabei gilt:

- Die Übersicht MUSS alle zur Gruppe gehörenden, für den Benutzer zugänglichen Fenster darstellen können.
- Fenster MÜSSEN anhand ihres Titels oder einer geeigneten Bezeichnung unterscheidbar sein.
- Vorschauen SOLLEN über den Compositor bereitgestellt werden.
- Geschützte Fensterinhalte DÜRFEN nicht unautorisiert dargestellt werden.
- Vorschauen MÜSSEN bedarfsgesteuert aktualisiert werden.
- Die Übersicht MUSS vollständig per Tastatur bedienbar sein.

## 9. Workspace- und Monitorintegration

Taskbar Grouping MUSS mit Nova Desktop Workspace Management zusammenarbeiten.

Dabei gilt:

- Gruppen MÜSSEN die aktuell konfigurierte Workspace-Filterung berücksichtigen.
- Fenster anderer Workspaces DÜRFEN optional in Gruppen enthalten sein.
- Die Gruppendarstellung MUSS auf unterschiedlichen Monitoren unabhängig konfigurierbar sein.
- Fensterwechsel zwischen Workspaces und Monitoren MÜSSEN dynamisch berücksichtigt werden.
- Die Gruppierung DARF keine eigene Fenster- oder Workspace-Verwaltung implementieren.

## 10. Angeheftete Anwendungen

Taskbar Grouping MUSS mit dem Pinned Application Manager zusammenarbeiten.

Dabei gilt:

- Laufende Fenster einer angehefteten Anwendung SOLLEN unter deren bestehendem Taskbar-Eintrag gruppiert werden.
- Angeheftete Elemente MÜSSEN auch ohne laufende Fenster erhalten bleiben.
- Das Beenden aller Gruppenfenster DARF die Anheftung nicht entfernen.
- Benutzerdefinierte Gruppierungsregeln MÜSSEN mit angehefteten Elementen vereinbar sein.

## 11. Performance und Zustandsverwaltung

Die Gruppierung MUSS ereignisgesteuert und ressourcenschonend arbeiten.

Dabei gilt:

- Änderungen an Fenstergruppen MÜSSEN durch Fensterereignisse ausgelöst werden.
- Vollständige Neuberechnungen SOLLEN vermieden werden, wenn nur einzelne Fenster betroffen sind.
- Unveränderte Gruppen DÜRFEN nicht permanent neu gerendert werden.
- Die Darstellung MUSS Damage Tracking unterstützen.
- Gruppierungseinstellungen MÜSSEN benutzerbezogen gespeichert werden.
- Nach einem Shell-Neustart MÜSSEN Gruppen aus den vorhandenen Fenstern rekonstruiert werden können.
- Fehlerhafte Gruppierungsdaten DÜRFEN die Taskleiste nicht beeinträchtigen.

## 12. Sicherheit und Barrierefreiheit

Taskbar Grouping unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Fenster anderer Benutzersitzungen DÜRFEN nicht unautorisiert gruppiert werden.
- Gruppierung DARF keine zusätzlichen Berechtigungen verleihen.
- Geschützte Fensterinformationen MÜSSEN berücksichtigt werden.
- Gruppen und enthaltene Fenster MÜSSEN für Screenreader unterscheidbar sein.
- Tastaturnavigation und High Contrast MÜSSEN unterstützt werden.
- Reduced Motion MUSS berücksichtigt werden.

## 13. Normative Anforderungen

- **TASKBAR-GROUP-001:** NovaOS MUSS mehrere Fenster einer Anwendung gruppieren können.
- **TASKBAR-GROUP-002:** Die Gruppierung MUSS anhand stabiler Anwendungsidentitäten erfolgen.
- **TASKBAR-GROUP-003:** Die Modi `Always`, `When Full`, `Never` und `Custom` MÜSSEN unterstützt werden.
- **TASKBAR-GROUP-004:** Gruppierungsregeln MÜSSEN deterministisch ausgewertet werden.
- **TASKBAR-GROUP-005:** Einzelne Fenster MÜSSEN innerhalb einer Gruppe erreichbar bleiben.
- **TASKBAR-GROUP-006:** Gruppen MÜSSEN Fensterzustände korrekt darstellen.
- **TASKBAR-GROUP-007:** Angeheftete Anwendungen MÜSSEN berücksichtigt werden.
- **TASKBAR-GROUP-008:** Workspace- und Monitorzuordnungen MÜSSEN unterstützt werden.
- **TASKBAR-GROUP-009:** Gruppierungsänderungen MÜSSEN ereignisgesteuert verarbeitet werden.
- **TASKBAR-GROUP-010:** Gruppierungseinstellungen MÜSSEN gespeichert werden können.
- **TASKBAR-GROUP-011:** Gruppen MÜSSEN nach einem Shell-Neustart rekonstruierbar sein.
- **TASKBAR-GROUP-012:** Die Darstellung MUSS Damage Tracking berücksichtigen.
- **TASKBAR-GROUP-013:** Gruppierung DARF keine Sicherheitsgrenzen verändern.
- **TASKBAR-GROUP-014:** Gruppen MÜSSEN barrierefrei bedienbar sein.
- **TASKBAR-GROUP-015:** Nova Taskbar Grouping MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Taskbar Architecture:** Übergeordnete Taskleistenarchitektur.
- **Nova Taskbar Application Management:** Anwendungsidentitäten und Fensterzuordnung.
- **Nova Window Manager:** Fensterzustände und Fensteraktionen.
- **Nova Desktop Workspace Management:** Workspace-Zuordnung.
- **Nova Compositor:** Fenstervorschauen und Damage Tracking.
- **Nova UI Framework:** Gruppendarstellung und Interaktion.
- **Nova Shell State Manager:** Speicherung und Wiederherstellung.
- **Nova.Trust:** Sicherheit und Berechtigungen.

## 15. Ergebnis

NovaOS erhält eine flexible und deterministische Gruppierungsverwaltung für Taskbar-Elemente.

Mehrere Fenster können platzsparend zusammengefasst werden, bleiben jedoch einzeln erreichbar. Benutzerdefinierte Regeln, Workspaces und Mehrmonitorbetrieb werden berücksichtigt.

**Grundsatz:** Die Gruppierung schafft Übersicht, ohne die Kontrolle über einzelne Fenster einzuschränken.