# NPSPEC-WINDOW-LAYOUT-0001 – Nova Window Snapping

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterlayout und Snapping  
**System:** NovaOS

## 1. Zweck

Nova Window Snapping definiert das automatische Ausrichten und Einrasten von Fenstern an Bildschirmkanten, Arbeitsbereichen und anderen Fenstern.

Ziel ist eine intuitive, schnelle und präzise Fensteranordnung, die den Benutzer unterstützt, ohne seine freie Fensterpositionierung einzuschränken.

## 2. Architektur

- Zentrale Snap-Verwaltung durch den Nova Window Manager.
- Integration in Window Positioning und Window Resizing.
- Unterstützung von Maus, Touch und Tastatur.
- Trennung zwischen Snap-Erkennung, Layoutberechnung und Darstellung.
- Berücksichtigung von Displays, Workspaces und DPI-Skalierung.
- Vorschau durch den Nova Compositor.
- Transaktionale Übernahme von Snap-Layouts.
- Benutzerseitig konfigurierbare und deaktivierbare Snap-Funktionen.

## 3. Snap-Modell

NovaOS unterstützt folgende Snap-Verfahren:

| Verfahren | Beschreibung |
|---|---|
| Edge Snap | Einrasten an Bildschirmkanten |
| Corner Snap | Einrasten in Bildschirmquadranten |
| Window Snap | Ausrichten an anderen Fenstern |
| Grid Snap | Einrasten in definierte Rasterbereiche |
| Zone Snap | Platzierung in konfigurierbaren Layoutzonen |
| Keyboard Snap | Fensteranordnung über Tastaturbefehle |

Snap-Bereiche werden in logischen Desktop-Koordinaten definiert.

Die Snap-Erkennung und die tatsächliche Fensterplatzierung MÜSSEN unabhängig voneinander funktionieren.

## 4. Kernfunktionen

- Erkennung geeigneter Snap-Ziele.
- Magnetisches Einrasten an Fensterkanten.
- Halb- und Viertelbildschirm-Layouts.
- Benutzerdefinierte Snap-Zonen.
- Visuelle Vorschau der Zielposition.
- Snap-Aktivierung über Tastaturbefehle.
- Gemeinsame Größenanpassung benachbarter Fenster.
- Wiederherstellung vorheriger Fenstergeometrien.
- Unterstützung mehrerer Displays.
- Konfigurierbare Snap-Abstände und Toleranzen.

## 5. Normative Anforderungen

- WINDOW-001: Snap-Operationen MÜSSEN durch den Window Manager koordiniert werden.
- WINDOW-002: Snap-Ziele MÜSSEN anhand gültiger Arbeitsbereiche berechnet werden.
- WINDOW-003: Unterschiedliche DPI-Skalierungen MÜSSEN berücksichtigt werden.
- WINDOW-004: Snap-Funktionen MÜSSEN deaktivierbar sein.
- WINDOW-005: Benutzerinitiierte Fensterbewegungen MÜSSEN Vorrang vor automatischem Snapping besitzen.
- WINDOW-006: Snap-Vorschauen DÜRFEN die verbindliche Fenstergeometrie nicht verändern.
- WINDOW-007: Snap-Operationen MÜSSEN atomar übernommen werden.
- WINDOW-008: Minimale und maximale Fenstergrößen MÜSSEN berücksichtigt werden.
- WINDOW-009: Nicht skalierbare Fenster DÜRFEN nicht unzulässig verändert werden.
- WINDOW-010: Snap-Operationen DÜRFEN keine Sicherheits- oder Fensterberechtigungen umgehen.
- WINDOW-011: Die ursprüngliche Fenstergeometrie MUSS für eine mögliche Wiederherstellung erhalten bleiben.
- WINDOW-012: Snap-Layouts MÜSSEN bei Displayänderungen neu validiert werden.
- WINDOW-013: Fenster DÜRFEN nicht durch automatische Snap-Operationen unerreichbar werden.
- WINDOW-014: Die Snap-Vorschau SOLL ohne blockierende Anwendungsaufrufe dargestellt werden.
- WINDOW-015: Snap-Operationen MÜSSEN ohne aktivierte Animationen funktionieren.
- WINDOW-016: Benutzerdefinierte Snap-Layouts SOLLEN gespeichert und wiederhergestellt werden können.

## 6. Snap-Erkennung

Während einer Fensterbewegung prüft NovaOS:

1. Aktuelle Fensterposition.
2. Bewegungsrichtung und Eingabemethode.
3. Verfügbare Snap-Ziele.
4. Konfigurierte Snap-Toleranz.
5. Größenbeschränkungen des Fensters.
6. Aktuelle Display- und Workspace-Geometrie.

Wird ein geeignetes Ziel erkannt, kann der Compositor eine dezente Vorschau anzeigen.

Das Einrasten erfolgt erst durch eine gültige Benutzeraktion oder einen ausdrücklich ausgelösten Snap-Befehl.

## 7. Snap-Layouts

### Bildschirmhälften

Fenster können die linke, rechte, obere oder untere Hälfte eines Arbeitsbereichs belegen.

### Bildschirmquadranten

Fenster können in einem der vier Quadranten angeordnet werden.

### Benutzerdefinierte Zonen

Der Benutzer kann eigene Layoutzonen definieren und speichern.

### Benachbarte Fenster

Fenster können aneinander ausgerichtet und innerhalb definierter Layoutgruppen gemeinsam skaliert werden.

Die Layoutberechnung MUSS sicherstellen, dass Fenstergrößenbeschränkungen eingehalten werden.

## 8. Snap-Verarbeitung

1. **Detect:** Geeignetes Snap-Ziel erkennen.
2. **Preview:** Zielgeometrie optional darstellen.
3. **Request:** Snap-Operation auslösen.
4. **Validate:** Fensterzustand und Größenbeschränkungen prüfen.
5. **Resolve:** Zielposition und Zielgröße berechnen.
6. **Commit:** Fenstergeometrie atomar übernehmen.
7. **Notify:** Anwendung und Compositor informieren.
8. **Persist:** Layout bei Bedarf speichern.

Kann ein Snap-Ziel nicht gültig umgesetzt werden, MUSS die Operation abgelehnt oder ein zulässiges alternatives Layout angeboten werden.

## 9. Fehlerbehandlung und Wiederherstellung

- Ungültige Snap-Ziele werden ignoriert.
- Fehlgeschlagene Snap-Operationen verändern die bestehende Fenstergeometrie nicht.
- Bei Displayverlust werden betroffene Snap-Layouts neu berechnet.
- Nicht mehr verfügbare Layoutzonen werden kontrolliert ersetzt oder deaktiviert.
- Nicht reagierende Anwendungen dürfen die Fensterbewegung nicht blockieren.
- Gespeicherte Layouts werden vor ihrer Wiederherstellung validiert.
- Snap-Vorschauen dürfen keine Eingabeereignisse abfangen.
- Ein Compositor-Neustart verändert die verbindlichen Fensterpositionen nicht.

## 10. Ergebnis

NovaOS erhält eine flexible und benutzerfreundliche Snap-Verwaltung.

Fenster können automatisch an Bildschirmkanten, anderen Fenstern und frei definierten Layoutzonen ausgerichtet werden.

Die Snap-Funktionen bleiben optional, ressourcenschonend und vollständig unter Kontrolle des Benutzers.

**Grundsatz:** Snapping unterstützt die Fensteranordnung, ohne dem Benutzer eine bestimmte Arbeitsweise vorzuschreiben.