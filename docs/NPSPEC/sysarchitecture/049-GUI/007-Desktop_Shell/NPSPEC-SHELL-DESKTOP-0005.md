# NPSPEC-SHELL-DESKTOP-0005 – Nova Desktop Workspace Management

**Status:** Angenommen  
**Bereich:** Shell / Desktop / Workspace  
**System:** NovaOS

## 1. Zweck

Nova Desktop Workspace Management definiert die Verwaltung virtueller Arbeitsbereiche innerhalb der NovaOS Desktop Shell.

Workspaces ermöglichen die logische Organisation von Fenstern, Anwendungen und Arbeitsumgebungen, ohne mehrere Benutzeranmeldungen oder unabhängige Desktopsitzungen zu benötigen.

Ziel ist eine übersichtliche, flexible und ressourcenschonende Arbeitsumgebung, die sich an unterschiedliche Aufgaben und Nutzungssituationen anpassen lässt.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Workspace Manager | Verwaltung virtueller Arbeitsbereiche |
| Workspace Registry | Registrierung und Identifikation |
| Workspace Controller | Aktivierung und Wechsel |
| Workspace Window Mapper | Zuordnung von Fenstern |
| Workspace Layout Manager | Verwaltung arbeitsbereichsspezifischer Layouts |
| Workspace State Manager | Speicherung und Wiederherstellung |

Die eigentliche Fensterverwaltung bleibt Aufgabe des Nova Window Managers.

## 3. Workspace-Modell

Ein Workspace beschreibt einen logischen Arbeitsbereich innerhalb einer Benutzersitzung.

Jeder Workspace besitzt:

- Eindeutige ID.
- Optionalen Namen.
- Reihenfolge.
- Zugeordnete Fenster.
- Layoutinformationen.
- Individuelle Einstellungen.
- Optionalen Kontext für Capabilities und Solutions.

Dabei gilt:

- Workspaces MÜSSEN unabhängig voneinander verwaltbar sein.
- Mindestens ein Workspace MUSS jederzeit verfügbar sein.
- Workspaces DÜRFEN keine eigenständigen Sicherheits- oder Benutzersitzungen darstellen.
- Die Anzahl der Workspaces SOLL nicht künstlich begrenzt werden.

## 4. Workspace-Verwaltung

NovaOS unterstützt:

- Erstellen von Workspaces.
- Entfernen von Workspaces.
- Umbenennen.
- Ändern der Reihenfolge.
- Wechseln zwischen Workspaces.
- Verschieben von Fenstern.
- Anzeigen aller Workspaces.

Dabei gilt:

- Workspace-Operationen MÜSSEN ohne Neustart der Desktop Shell möglich sein.
- Das Entfernen eines Workspace DARF zugeordnete Anwendungen nicht automatisch beenden.
- Fenster MÜSSEN bei Bedarf einem anderen Workspace zugeordnet werden.
- Änderungen SOLLEN unmittelbar wirksam werden.

## 5. Fensterzuordnung

Der Workspace Window Mapper verwaltet die logische Zuordnung von Fenstern zu Workspaces.

Dabei gilt:

- Ein Fenster MUSS einem Workspace zugeordnet werden können.
- Fenster DÜRFEN optional auf mehreren Workspaces sichtbar sein.
- Systemweite Fenster und Dialoge MÜSSEN gesondert behandelt werden.
- Fensterzustände MÜSSEN beim Workspace-Wechsel erhalten bleiben.
- Die Zuordnung MUSS über definierte Schnittstellen des Window Managers erfolgen.

Der Workspace Manager DARF keine eigene konkurrierende Fensterverwaltung implementieren.

## 6. Workspace-Wechsel

Der Wechsel zwischen Workspaces MUSS schnell und ohne Unterbrechung laufender Anwendungen erfolgen.

Unterstützt werden:

- Tastenkombinationen.
- Taskleistensteuerung.
- Workspace-Übersicht.
- Touch-Gesten.
- Alternative Eingabemethoden.

Dabei gilt:

- Anwendungen DÜRFEN beim Wechsel nicht automatisch beendet oder neu gestartet werden.
- Der zuletzt fokussierte gültige Fensterzustand SOLL wiederhergestellt werden.
- Workspace-Wechsel SOLLEN ohne unnötige Animationen möglich sein.
- Reduced Motion MUSS berücksichtigt werden.

## 7. Workspace-Layouts

Jeder Workspace kann eine eigene Fensteranordnung besitzen.

Unterstützt werden:

- Freie Fensteranordnung.
- Maximierte Fenster.
- Fenstergruppen.
- Snap-Layouts.
- Benutzerdefinierte Anordnungen.
- Monitorabhängige Layouts.

Dabei gilt:

- Layouts MÜSSEN über den Window Manager umgesetzt werden.
- Fensterpositionen und Größen SOLLEN gespeichert werden.
- Änderungen der Bildschirmkonfiguration MÜSSEN berücksichtigt werden.
- Fenster DÜRFEN nach einer Layoutänderung nicht dauerhaft unerreichbar sein.

## 8. Mehrmonitorbetrieb

Nova Desktop Workspace Management unterstützt mehrere Monitore.

Dabei gilt:

- Workspaces MÜSSEN monitorübergreifend verwendet werden können.
- NovaOS SOLL zwischen gemeinsamem und unabhängigem Workspace-Wechsel pro Monitor unterscheiden können.
- Fenster MÜSSEN zwischen Monitoren und Workspaces verschoben werden können.
- Änderungen der Monitorkonfiguration MÜSSEN kontrolliert verarbeitet werden.
- Gespeicherte Workspace-Layouts SOLLEN nach Wiederanschluss eines Monitors wiederhergestellt werden.

## 9. Kontextabhängige Arbeitsbereiche

Workspaces können optional mit bestimmten Aufgaben oder Nutzungskontexten verbunden werden.

Beispiele:

- Entwicklung.
- Büroarbeit.
- Medienbearbeitung.
- Kommunikation.
- Unterhaltung.

Dabei gilt:

- Workspaces DÜRFEN passende Solutions und Capabilities anbieten.
- Kontextabhängige Vorschläge MÜSSEN abschaltbar sein.
- Automatische Anpassungen MÜSSEN nachvollziehbar bleiben.
- Der Benutzer MUSS die Kontrolle über Workspace-Inhalte und Anordnung behalten.
- Ein Workspace DARF keine zusätzlichen Berechtigungen verleihen.

## 10. Zustand und Wiederherstellung

Der Workspace State Manager speichert wiederherstellbare Workspace-Informationen.

Dazu gehören:

- Workspace-IDs und Namen.
- Reihenfolge.
- Fensterzuordnungen.
- Layouts.
- Monitorzuordnungen.
- Aktiver Workspace.
- Benutzerdefinierte Einstellungen.

Dabei gilt:

- Zustandsänderungen SOLLEN inkrementell gespeichert werden.
- Ein Shell-Neustart DARF gespeicherte Workspace-Konfigurationen nicht verlieren.
- Fensterzuordnungen MÜSSEN nach einem Neustart anhand gültiger Fensteridentitäten wiederhergestellt werden.
- Nicht mehr vorhandene Fenster MÜSSEN kontrolliert behandelt werden.
- Beschädigte Zustandsdaten DÜRFEN den Desktopstart nicht verhindern.

## 11. Performance

Workspace Management MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Inaktive Workspaces DÜRFEN keine unnötige Shell-Aktivität verursachen.
- Verdeckte Fenster SOLLEN über den Compositor von unnötigen Präsentationen entlastet werden.
- Anwendungen DÜRFEN nicht allein aufgrund eines inaktiven Workspace angehalten werden.
- Workspace-Wechsel MÜSSEN ohne vollständige Neuerstellung der Fenster erfolgen.
- Änderungen MÜSSEN Damage Tracking berücksichtigen.
- Animationen DÜRFEN keine permanente Neuberechnung verursachen.

## 12. Sicherheit und Barrierefreiheit

Nova Desktop Workspace Management unterliegt Nova.Trust und der Nova Accessibility Architecture.

Dabei gilt:

- Workspace-Zuordnungen DÜRFEN Sicherheitsgrenzen nicht verändern.
- Fenster anderer Benutzersitzungen DÜRFEN nicht unautorisiert eingebunden werden.
- Systemkritische Dialoge MÜSSEN unabhängig vom aktiven Workspace erreichbar bleiben.
- Workspaces MÜSSEN vollständig per Tastatur bedienbar sein.
- Workspace-Namen und Zustände MÜSSEN für Screenreader verfügbar sein.
- High Contrast und Reduced Motion MÜSSEN unterstützt werden.

## 13. Normative Anforderungen

- **DESKTOP-001:** NovaOS MUSS virtuelle Workspaces unterstützen.
- **DESKTOP-002:** Workspaces MÜSSEN innerhalb einer Benutzersitzung verwaltet werden.
- **DESKTOP-003:** Mindestens ein Workspace MUSS jederzeit verfügbar sein.
- **DESKTOP-004:** Workspaces MÜSSEN dynamisch erstellt, entfernt und umbenannt werden können.
- **DESKTOP-005:** Fenster MÜSSEN Workspaces zugeordnet werden können.
- **DESKTOP-006:** Fenster MÜSSEN zwischen Workspaces verschiebbar sein.
- **DESKTOP-007:** Workspace-Wechsel DÜRFEN Anwendungen nicht automatisch beenden.
- **DESKTOP-008:** Die Fensterverwaltung MUSS beim Nova Window Manager verbleiben.
- **DESKTOP-009:** Workspace-Layouts MÜSSEN speicherbar sein.
- **DESKTOP-010:** Mehrmonitorbetrieb MUSS berücksichtigt werden.
- **DESKTOP-011:** Workspace-Zustände MÜSSEN nach einem Shell-Neustart wiederherstellbar sein.
- **DESKTOP-012:** Inaktive Workspaces DÜRFEN keine unnötige Renderingaktivität verursachen.
- **DESKTOP-013:** Workspaces DÜRFEN keine zusätzlichen Berechtigungen verleihen.
- **DESKTOP-014:** Workspace-Funktionen MÜSSEN barrierefrei bedienbar sein.
- **DESKTOP-015:** Nova Desktop Workspace Management MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Desktop Environment:** Grafische Arbeitsumgebung.
- **Nova Desktop Shell Architecture:** Übergeordnete Shell-Verwaltung.
- **Nova Shell Lifecycle:** Initialisierung und Beendigung.
- **Nova Shell Recovery:** Wiederherstellung von Workspaces.
- **Nova Window Manager:** Fensterzuordnung und Fensterzustände.
- **Nova Compositor:** Sichtbarkeit und Darstellung.
- **Nova Display Server:** Mehrmonitorbetrieb.
- **Nova Taskbar:** Anzeige workspacebezogener Fenster.
- **Nova UI Framework:** Workspace-Übersicht und Bedienung.
- **Nova Input System:** Tastatur, Maus und Gesten.
- **Nova Capability System:** Kontextabhängige Funktionen.
- **Nova.Trust:** Sicherheit und Berechtigungen.

## 15. Ergebnis

NovaOS erhält eine flexible Verwaltung virtueller Arbeitsbereiche.

Fenster und Anwendungen können logisch organisiert, zwischen Workspaces verschoben und in individuellen Layouts angeordnet werden.

Workspaces bleiben unabhängig von der eigentlichen Anwendungsausführung und können nach einem Shell-Neustart wiederhergestellt werden.

**Grundsatz:** Workspaces organisieren die Arbeit des Benutzers, ohne Anwendungen künstlich voneinander zu trennen oder zusätzliche Systemressourcen zu beanspruchen.