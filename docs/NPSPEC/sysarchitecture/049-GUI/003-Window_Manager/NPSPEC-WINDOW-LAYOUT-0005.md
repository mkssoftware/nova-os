# NPSPEC-WINDOW-LAYOUT-0005 – Nova Multi-Monitor Window Placement

**Status:** Angenommen  
**Bereich:** Window Manager / Multi-Monitor-Fensterplatzierung  
**System:** NovaOS

## 1. Zweck

Nova Multi-Monitor Window Placement definiert die Platzierung, Verschiebung und Wiederherstellung von Fenstern innerhalb einer Desktopumgebung mit mehreren Displays.

Ziel ist eine zuverlässige und intuitive Fensterverwaltung bei unterschiedlichen Auflösungen, Skalierungsfaktoren, Displayanordnungen und dynamischen Änderungen der Monitorkonfiguration.

## 2. Architektur

- Zentrale Fensterplatzierung durch den Nova Window Manager.
- Integration in Nova Display Output Management.
- Verwendung eines gemeinsamen logischen Desktop-Koordinatensystems.
- Trennung zwischen Fensterposition, Displayzuordnung und physischer Ausgabe.
- Unterstützung unterschiedlicher DPI-Skalierungen und Displayausrichtungen.
- Integration in Window Positioning, Resizing und Fullscreen Management.
- Berücksichtigung virtueller Desktops und Workspaces.
- Automatische Anpassung bei Display-Hotplug.
- Optionale Persistenz von Fensterpositionen pro Displaykonfiguration.

## 3. Displaymodell

Jedes Display stellt folgende Informationen bereit:

| Eigenschaft | Beschreibung |
|---|---|
| Display ID | Eindeutige Displayidentität |
| Logical Bounds | Position und Größe im logischen Desktop |
| Work Area | Verfügbarer Arbeitsbereich |
| Scale Factor | Logischer Skalierungsfaktor |
| Orientation | Displayausrichtung |
| Primary | Kennzeichnung des primären Displays |
| Connection State | Verbindungszustand |

Der Window Manager verwendet diese Informationen zur Berechnung gültiger Fensterpositionen.

Physische Auflösung und logische Desktopgröße MÜSSEN getrennt behandelt werden.

## 4. Kernfunktionen

- Automatische Platzierung neuer Fenster.
- Verschieben von Fenstern zwischen Displays.
- Unterstützung unterschiedlicher DPI-Skalierungen.
- Platzierung über Displaygrenzen hinweg.
- Zentrierung auf einem bestimmten Display.
- Maximierung innerhalb des jeweiligen Arbeitsbereichs.
- Vollbilddarstellung auf einem ausgewählten Display.
- Wiederherstellung gespeicherter Displayzuordnungen.
- Automatische Neupositionierung bei Displayverlust.
- Berücksichtigung virtueller Desktops.
- Unterstützung von Snap-, Tiling- und Fenstergruppen.

## 5. Normative Anforderungen

- WINDOW-001: Der Window Manager MUSS Fensterpositionen über mehrere Displays hinweg verwalten.
- WINDOW-002: Fensterpositionen MÜSSEN in einem gemeinsamen logischen Koordinatensystem definiert werden.
- WINDOW-003: Displays DÜRFEN unterschiedliche Auflösungen, Skalierungen und Ausrichtungen besitzen.
- WINDOW-004: Fenster MÜSSEN zwischen Displays verschoben werden können.
- WINDOW-005: Fenster DÜRFEN mehrere Displays gleichzeitig überlappen.
- WINDOW-006: Jedes Fenster MUSS ein eindeutig bestimmbares maßgebliches Display besitzen.
- WINDOW-007: Die Bestimmung des maßgeblichen Displays MUSS nach definierten Regeln erfolgen.
- WINDOW-008: Fenstergrößen und Positionen MÜSSEN beim Wechsel zwischen unterschiedlichen DPI-Skalierungen korrekt behandelt werden.
- WINDOW-009: Maximierte Fenster MÜSSEN den Arbeitsbereich ihres zugeordneten Displays berücksichtigen.
- WINDOW-010: Vollbildfenster MÜSSEN die Geometrie ihres Ziel-Displays verwenden.
- WINDOW-011: Neue Fenster SOLLEN auf einem für den Benutzer geeigneten Display erscheinen.
- WINDOW-012: Bei Displayverlust MÜSSEN betroffene Fenster erreichbar bleiben.
- WINDOW-013: Änderungen der Displaykonfiguration MÜSSEN atomare Neuberechnungen betroffener Fenstergeometrien ermöglichen.
- WINDOW-014: Benutzerdefinierte Fensterpositionen SOLLEN bei unveränderter Displaykonfiguration erhalten bleiben.
- WINDOW-015: Gespeicherte Displayzuordnungen SOLLEN bei erneuter Verbindung wiederhergestellt werden können.
- WINDOW-016: Fensterplatzierungen DÜRFEN bestehende Sicherheits- und Modalitätsregeln nicht umgehen.
- WINDOW-017: Nicht reagierende Anwendungen DÜRFEN die Neupositionierung anderer Fenster nicht blockieren.
- WINDOW-018: Die Multi-Monitor-Platzierung MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 6. Bestimmung des maßgeblichen Displays

Der Window Manager bestimmt das maßgebliche Display anhand folgender Prioritäten:

1. Explizite, gültige Displayzuordnung.
2. Display mit dem größten sichtbaren Fensterflächenanteil.
3. Display mit dem Fenstermittelpunkt.
4. Display der aktuellen Benutzerinteraktion.
5. Primäres Display als Fallback.

Bei gleicher Eignung MUSS eine deterministische Auswahl erfolgen.

Die Displayzuordnung DARF nicht bei geringfügigen Positionsänderungen unnötig wechseln.

## 7. Fensterplatzierung

### Neue Fenster

Neue Fenster werden bevorzugt auf dem Display platziert, das zum aktuellen Benutzerkontext passt.

Dabei können berücksichtigt werden:

- Display des aktiven Fensters.
- Display des übergeordneten Fensters.
- Display der letzten Benutzerinteraktion.
- Gespeicherte Fensterposition.
- Benutzerdefinierte Platzierungsregeln.

### Fenster zwischen Displays verschieben

Beim Displaywechsel:

- Logische Fensterposition aktualisieren.
- Maßgebliches Display neu bestimmen.
- DPI-Skalierung berücksichtigen.
- Fenstergrößenbeschränkungen prüfen.
- Anwendung bei erforderlichen Konfigurationsänderungen benachrichtigen.
- Compositor-Darstellung aktualisieren.

### Fenster über mehrere Displays

Fenster DÜRFEN Displaygrenzen überschreiten.

Der Compositor übernimmt die korrekte Darstellung auf den betroffenen Displays unter Berücksichtigung ihrer jeweiligen Skalierung und Ausrichtung.

## 8. Display-Hotplug

### Display hinzufügen

1. Neues Display registrieren.
2. Logische Desktopgeometrie aktualisieren.
3. Arbeitsbereiche neu berechnen.
4. Gespeicherte Fensterzuordnungen prüfen.
5. Betroffene Fenster gegebenenfalls wiederherstellen.
6. Compositor und Anwendungen informieren.

### Display entfernen

1. Displayverlust erkennen.
2. Betroffene Fenster bestimmen.
3. Geeignetes Ersatzdisplay auswählen.
4. Fenstergeometrien anpassen.
5. Maximierte und Vollbildfenster neu konfigurieren.
6. Fensterzuordnungen atomar aktualisieren.
7. Darstellung und Fokusverwaltung synchronisieren.

Die ursprünglichen Displayzuordnungen SOLLEN für eine spätere Wiederherstellung erhalten bleiben.

## 9. Persistenz und Wiederherstellung

NovaOS kann Fensterpositionen abhängig von der Displaykonfiguration speichern.

Gespeichert werden können:

- Fensteridentität beziehungsweise wiederherstellbare Anwendungsreferenz.
- Displayidentität.
- Logische Fensterposition.
- Fenstergröße.
- Darstellungsmodus.
- Zugeordneter Workspace.
- Displaykonfiguration.

Bei der Wiederherstellung MÜSSEN gespeicherte Geometrien gegen die aktuelle Displaykonfiguration validiert werden.

Nicht mehr gültige Positionen werden automatisch in erreichbare Arbeitsbereiche übertragen.

## 10. Fehlerbehandlung und Zuverlässigkeit

- Ungültige Displayreferenzen werden zurückgewiesen.
- Fehlgeschlagene Platzierungsoperationen hinterlassen keine inkonsistenten Fensterzustände.
- Fenster bleiben auch bei vollständiger Änderung der Displayanordnung erreichbar.
- Nicht verfügbare Displays werden durch geeignete aktive Displays ersetzt.
- DPI-Änderungen führen nicht zu undefinierten Fenstergrößen.
- Modale Fenster bleiben ihrem gültigen Interaktionskontext zugeordnet.
- Ein Compositor-Neustart verändert die verbindlichen Fensterpositionen nicht.
- Fenstergruppen und Tiling-Layouts werden nach Displayänderungen neu validiert.
- Sicherheitskritische Systemfenster bleiben auf einem verfügbaren Display erreichbar.

## 11. Ergebnis

NovaOS erhält eine einheitliche Multi-Monitor-Fensterplatzierung für unterschiedliche Displaykonfigurationen.

Fenster können frei zwischen Monitoren verschoben, über Displaygrenzen dargestellt und bei Änderungen der Hardware automatisch angepasst werden.

Gespeicherte Fensterpositionen ermöglichen die Wiederherstellung vertrauter Arbeitsumgebungen.

**Grundsatz:** Die Fensteranordnung passt sich der verfügbaren Displayumgebung an, ohne die gewohnte Arbeitsweise des Benutzers zu beeinträchtigen.