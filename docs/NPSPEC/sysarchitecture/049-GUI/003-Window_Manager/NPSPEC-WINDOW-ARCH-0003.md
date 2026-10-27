# NPSPEC-WINDOW-ARCH-0003 – Nova Window State Model

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterzustandsmodell  
**System:** NovaOS

## 1. Zweck

Nova Window State Model definiert die Zustände und Zustandsübergänge von Fenstern innerhalb des Nova Window Managers.

Ziel ist eine einheitliche, deterministische und zuverlässige Verwaltung von Fenstergeometrie, Sichtbarkeit, Aktivierung und Darstellungsmodus.

## 2. Architektur

- Zentrale Zustandsverwaltung durch den Nova Window Manager.
- Trennung zwischen Lebenszyklus, Darstellungsmodus, Sichtbarkeit und Fokus.
- Transaktionale Verarbeitung von Zustandsänderungen.
- Ereignisgesteuerte Benachrichtigung betroffener Komponenten.
- Synchronisation mit Nova Compositor und Display Server.
- Unterstützung mehrerer Displays und virtueller Desktops.
- Persistenz wiederherstellbarer Fensterzustände.
- Unabhängigkeit von visuellen Fensteranimationen.

## 3. Zustandsmodell

Ein Fenster besitzt mehrere voneinander unabhängige Zustandsdimensionen.

### 3.1 Darstellungsmodus

| Zustand | Beschreibung |
|---|---|
| Normal | Frei positionierbares Fenster |
| Minimized | Minimiertes Fenster |
| Maximized | Fenster füllt den verfügbaren Arbeitsbereich |
| Fullscreen | Fenster nutzt den vorgesehenen Displaybereich |

### 3.2 Sichtbarkeit

- **Visible:** Fenster ist grundsätzlich zur Darstellung freigegeben.
- **Hidden:** Fenster wurde ausdrücklich ausgeblendet.

Die tatsächliche Sichtbarkeit hängt zusätzlich von Workspace, Display, Überlagerungen und Darstellungsmodus ab.

### 3.3 Aktivierung und Fokus

- **Active:** Fenster ist das aktive Fenster seines Eingabekontexts.
- **Inactive:** Fenster ist nicht aktiv.
- **Focused:** Fenster oder eines seiner Bedienelemente besitzt den Tastaturfokus.

Aktivierung und Tastaturfokus MÜSSEN getrennt verwaltet werden können.

### 3.4 Weitere Zustandsmerkmale

- Always on Top
- Modal
- Resizable
- Movable
- Attention Requested
- Workspace Assignment
- Display Assignment

Diese Merkmale sind Eigenschaften und keine zusätzlichen Darstellungsmodi.

## 4. Normative Anforderungen

- WINDOW-001: Jedes Fenster MUSS einen eindeutig definierten Darstellungsmodus besitzen.
- WINDOW-002: Darstellungsmodus, Sichtbarkeit und Fokus MÜSSEN unabhängig verwaltet werden.
- WINDOW-003: Zustandsänderungen MÜSSEN über den Window Manager erfolgen.
- WINDOW-004: Zusammengehörige Zustandsänderungen MÜSSEN atomar übernommen werden.
- WINDOW-005: Ungültige Zustandskombinationen MÜSSEN verhindert werden.
- WINDOW-006: Der Window Manager MUSS den verbindlichen Fensterzustand verwalten.
- WINDOW-007: Anwendungen DÜRFEN Zustandsänderungen anfordern, aber nicht eigenmächtig erzwingen.
- WINDOW-008: Beim Maximieren MUSS die zulässige Arbeitsfläche berücksichtigt werden.
- WINDOW-009: Beim Wechsel in den Vollbildmodus MUSS die Displaygeometrie berücksichtigt werden.
- WINDOW-010: Die Wiederherstellungsgeometrie MUSS bei Maximierung und Minimierung erhalten bleiben.
- WINDOW-011: Minimierte Fenster DÜRFEN keinen regulären Tastaturfokus behalten.
- WINDOW-012: Ausgeblendete Fenster DÜRFEN keine regulären Eingabeereignisse empfangen.
- WINDOW-013: Fokusänderungen MÜSSEN den geltenden Sicherheits- und Aktivierungsregeln entsprechen.
- WINDOW-014: Zustandsänderungen MÜSSEN dem Compositor konsistent übermittelt werden.
- WINDOW-015: Visuelle Animationen DÜRFEN den verbindlichen Fensterzustand nicht verändern.
- WINDOW-016: Änderungen der Displaykonfiguration MÜSSEN eine Neubewertung betroffener Fensterzustände auslösen.
- WINDOW-017: Wiederherstellbare Fensterzustände SOLLEN persistent gespeichert werden können.
- WINDOW-018: Zustandsänderungen MÜSSEN ohne KI-Abhängigkeit deterministisch verarbeitet werden.

## 5. Zustandsübergänge

### Normal → Maximized

- Aktuelle Fenstergeometrie speichern.
- Verfügbaren Arbeitsbereich bestimmen.
- Maximierte Geometrie übernehmen.
- Compositor benachrichtigen.

### Maximized → Normal

- Gespeicherte Wiederherstellungsgeometrie prüfen.
- Geometrie an aktuelle Displaygrenzen anpassen.
- Normalzustand wiederherstellen.

### Normal / Maximized / Fullscreen → Minimized

- Vorherigen wiederherstellbaren Darstellungsmodus erhalten.
- Fenster aus der regulären Darstellung entfernen.
- Fokus gegebenenfalls neu zuweisen.

### Minimized → Previous State

- Vorherigen Darstellungsmodus wiederherstellen.
- Fenster erneut zur Darstellung freigeben.
- Fokus gemäß Aktivierungsregeln vergeben.

### Normal / Maximized → Fullscreen

- Vorherigen Zustand und Wiederherstellungsgeometrie sichern.
- Ziel-Display bestimmen.
- Vollbildgeometrie übernehmen.

### Fullscreen → Previous State

- Vorherigen Darstellungsmodus wiederherstellen.
- Geometrie anhand der aktuellen Displaykonfiguration validieren.

## 6. Zustandsdaten

Jedes Fenster verwaltet mindestens folgende Informationen:

| Eigenschaft | Beschreibung |
|---|---|
| Window ID | Eindeutige Fensteridentität |
| Mode | Aktueller Darstellungsmodus |
| Visibility | Sichtbarkeitszustand |
| Activation | Aktivierungszustand |
| Geometry | Aktuelle Fenstergeometrie |
| Restore Geometry | Geometrie zur Wiederherstellung |
| Previous Mode | Vorheriger wiederherstellbarer Modus |
| Display ID | Zugeordnetes Display |
| Workspace ID | Zugeordneter Arbeitsbereich |
| State Version | Version des verbindlichen Zustands |

Zustandsversionen ermöglichen die eindeutige Zuordnung asynchroner Änderungen.

## 7. Zustandsverarbeitung

1. **Request:** Zustandsänderung empfangen.
2. **Authorize:** Berechtigungen prüfen.
3. **Validate:** Übergang und Zielzustand prüfen.
4. **Resolve:** Geometrie, Display und Workspace bestimmen.
5. **Commit:** Zustandsänderung atomar übernehmen.
6. **Notify:** Betroffene Komponenten informieren.
7. **Compose:** Darstellung durch den Compositor aktualisieren.
8. **Persist:** Wiederherstellbare Zustände bei Bedarf speichern.

Bei fehlgeschlagener Validierung bleibt der bisherige verbindliche Zustand erhalten.

## 8. Fehlerbehandlung und Wiederherstellung

- Ungültige Zustandsanforderungen werden kontrolliert zurückgewiesen.
- Veraltete asynchrone Änderungen dürfen neuere Zustände nicht überschreiben.
- Bei Displayverlust werden Fenster auf verfügbare Displays umgeordnet.
- Ungültige Wiederherstellungsgeometrien werden an den verfügbaren Arbeitsbereich angepasst.
- Nach Compositor-Neustart wird der verbindliche Zustand erneut übertragen.
- Fehlerhafte Anwendungen dürfen keine Zustandsänderungen anderer Fenster erzwingen.
- Zustandsänderungen und Fehler bleiben diagnostizierbar.

## 9. Ergebnis

NovaOS erhält ein einheitliches und deterministisches Fensterzustandsmodell.

Darstellungsmodus, Sichtbarkeit, Fokus und Fenstergeometrie werden unabhängig, aber konsistent verwaltet.

Durch transaktionale Zustandsänderungen und klare Wiederherstellungsregeln bleiben Fenster auch bei Displaywechseln, Anwendungsfehlern und Compositor-Neustarts zuverlässig bedienbar.