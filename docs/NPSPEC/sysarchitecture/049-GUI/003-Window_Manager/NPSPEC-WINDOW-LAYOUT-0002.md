# NPSPEC-WINDOW-LAYOUT-0002 – Nova Window Tiling

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterlayout und Tiling  
**System:** NovaOS

## 1. Zweck

Nova Window Tiling definiert die automatische, platzsparende Anordnung mehrerer Fenster innerhalb eines Arbeitsbereichs.

Ziel ist eine effiziente Nutzung verfügbarer Bildschirmflächen, ohne die klassische freie Fensterverwaltung einzuschränken.

## 2. Architektur

- Integration in den Nova Window Manager.
- Trennung zwischen Layoutberechnung und grafischer Darstellung.
- Unterstützung von Floating-, Tiling- und Hybrid-Modus.
- Layoutverwaltung pro Workspace und Display.
- Integration in Window Positioning, Resizing und Snapping.
- Berücksichtigung von DPI-Skalierung und Fenstergrößenbeschränkungen.
- Transaktionale Anwendung von Layoutänderungen.
- Persistenz benutzerdefinierter Layouts.

## 3. Tiling-Modell

NovaOS unterstützt folgende Betriebsarten:

| Modus | Beschreibung |
|---|---|
| Floating | Fenster werden frei positioniert |
| Tiling | Fenster werden automatisch ohne gegenseitige Überlagerung angeordnet |
| Hybrid | Frei positionierte und gekachelte Fenster existieren gemeinsam |

### Layoutverfahren

| Layout | Beschreibung |
|---|---|
| Horizontal | Fenster werden nebeneinander angeordnet |
| Vertical | Fenster werden untereinander angeordnet |
| Grid | Fenster werden rasterförmig verteilt |
| Master-Stack | Ein Hauptfenster und mehrere Nebenfenster |
| Split | Arbeitsbereich wird rekursiv unterteilt |
| Custom | Benutzerdefiniertes Layout |

Die Layoutverfahren MÜSSEN unabhängig vom verwendeten Compositor funktionieren.

## 4. Kernfunktionen

- Automatische Fensteranordnung.
- Dynamische Anpassung bei neuen oder entfernten Fenstern.
- Wechsel zwischen Floating und Tiling.
- Änderung der Fensterreihenfolge.
- Anpassung von Teilungsverhältnissen.
- Unterstützung benutzerdefinierter Layouts.
- Verschieben von Fenstern zwischen Layoutgruppen.
- Verwaltung mehrerer Displays und Workspaces.
- Speicherung und Wiederherstellung von Layouts.
- Tastatur-, Maus- und Touchbedienung.

## 5. Normative Anforderungen

- WINDOW-001: Tiling MUSS optional und vollständig deaktivierbar sein.
- WINDOW-002: Die klassische freie Fensterverwaltung MUSS jederzeit verfügbar bleiben.
- WINDOW-003: Layoutberechnungen MÜSSEN in logischen Desktop-Koordinaten erfolgen.
- WINDOW-004: Tiling MUSS den verfügbaren Arbeitsbereich einschließlich reservierter Systemflächen berücksichtigen.
- WINDOW-005: Fenstergrößenbeschränkungen MÜSSEN eingehalten werden.
- WINDOW-006: Layoutänderungen MÜSSEN atomar übernommen werden.
- WINDOW-007: Nicht kompatible Fenster DÜRFEN nicht zwangsweise in ein Tiling-Layout integriert werden.
- WINDOW-008: Modale Dialoge und temporäre Popups SOLLEN unabhängig vom regulären Tiling angeordnet werden.
- WINDOW-009: Der Benutzer MUSS Fenster aus einem Tiling-Layout lösen können.
- WINDOW-010: Änderungen der Displaykonfiguration MÜSSEN eine Neuberechnung betroffener Layouts auslösen.
- WINDOW-011: Fenster DÜRFEN durch Tiling nicht dauerhaft unerreichbar werden.
- WINDOW-012: Tiling MUSS mit der bestehenden Fokus- und Z-Order-Verwaltung zusammenarbeiten.
- WINDOW-013: Benutzerdefinierte Layouts SOLLEN gespeichert und wiederhergestellt werden können.
- WINDOW-014: Layoutberechnungen DÜRFEN nicht durch blockierende Anwendungsaufrufe verzögert werden.
- WINDOW-015: Der Compositor DARF die verbindliche Layoutgeometrie nicht eigenständig verändern.
- WINDOW-016: Tiling MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 6. Layoutverwaltung

Jede Tiling-Gruppe besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Layout ID | Eindeutige Layoutidentität |
| Workspace ID | Zugeordneter Workspace |
| Display ID | Zugeordnetes Display |
| Layout Type | Verwendetes Layoutverfahren |
| Window Set | Enthaltene Fenster |
| Split Ratios | Teilungsverhältnisse |
| Spacing | Abstände zwischen Fenstern |
| Layout Version | Version der Layoutkonfiguration |

Fenster können innerhalb einer Layoutgruppe verschoben oder zwischen Gruppen übertragen werden.

Ein Fenster DARF nicht gleichzeitig mehreren aktiven Tiling-Gruppen angehören.

## 7. Dynamische Layoutanpassung

Bei Änderungen der Fensteranzahl wird das Layout automatisch neu berechnet.

### Neues Fenster

- Fenster auf Tiling-Eignung prüfen.
- Geeignete Layoutposition bestimmen.
- Verfügbare Fläche neu verteilen.
- Neue Fenstergeometrien atomar übernehmen.

### Fenster entfernen

- Fenster aus der Layoutgruppe entfernen.
- Frei gewordene Fläche neu verteilen.
- Verbleibende Fenstergeometrien aktualisieren.

### Fenstergröße ändern

- Teilungsverhältnisse entsprechend der Benutzerinteraktion anpassen.
- Größenbeschränkungen benachbarter Fenster berücksichtigen.
- Betroffene Fenster gemeinsam aktualisieren.

## 8. Layoutwechsel

Der Wechsel zwischen Layoutverfahren erfolgt transaktional.

1. **Request:** Layoutänderung anfordern.
2. **Validate:** Fenster und Größenbeschränkungen prüfen.
3. **Calculate:** Neue Fenstergeometrien berechnen.
4. **Resolve:** Unzulässige Größenkonflikte behandeln.
5. **Commit:** Layout und Fenstergeometrien atomar übernehmen.
6. **Notify:** Anwendungen und Compositor informieren.
7. **Persist:** Layoutkonfiguration bei Bedarf speichern.

Kann ein Layout nicht vollständig umgesetzt werden, MUSS ein definierter Fallback erfolgen.

Bereits gültige Fensterzustände dürfen dabei nicht verloren gehen.

## 9. Fehlerbehandlung und Wiederherstellung

- Ungültige Layoutkonfigurationen werden zurückgewiesen.
- Nicht erfüllbare Größenbeschränkungen führen zu einem geeigneten Alternativlayout oder Floating-Fallback.
- Bei Displayverlust werden betroffene Layoutgruppen auf verfügbare Arbeitsbereiche übertragen.
- Nicht reagierende Anwendungen blockieren keine Layoutänderungen.
- Gespeicherte Layouts werden vor der Wiederherstellung validiert.
- Ein Compositor-Neustart verändert die verbindlichen Layoutdaten nicht.
- Beim Verlassen des Tiling-Modus SOLLEN vorherige Floating-Geometrien wiederhergestellt werden.

## 10. Ergebnis

NovaOS erhält eine flexible Fensterkachelverwaltung mit klassischen, automatischen und hybriden Layoutverfahren.

Fenster können effizient auf verfügbare Bildschirmflächen verteilt und dynamisch angepasst werden.

Der Benutzer entscheidet jederzeit, ob Fenster frei positioniert oder automatisch angeordnet werden.

**Grundsatz:** Tiling optimiert die Nutzung des Arbeitsbereichs, ohne die Freiheit der klassischen Fensterverwaltung einzuschränken.