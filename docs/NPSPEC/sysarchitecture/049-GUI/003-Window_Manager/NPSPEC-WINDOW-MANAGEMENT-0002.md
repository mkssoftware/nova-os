# NPSPEC-WINDOW-MANAGEMENT-0002 – Nova Window Positioning

**Status:** Angenommen  
**Bereich:** Window Manager / Fensterpositionierung  
**System:** NovaOS

## 1. Zweck

Nova Window Positioning definiert die Positionierung und Verschiebung von Fenstern innerhalb der NovaOS-Desktopumgebung.

Ziel ist eine präzise, intuitive und zuverlässige Fensterpositionierung unter Berücksichtigung mehrerer Displays, unterschiedlicher Skalierungsfaktoren und individueller Benutzerlayouts.

## 2. Architektur

- Zentrale Positionsverwaltung durch den Nova Window Manager.
- Verwendung eines einheitlichen logischen Desktop-Koordinatensystems.
- Unterstützung mehrerer Displays mit unterschiedlichen Positionen und Skalierungen.
- Trennung zwischen logischer Fensterposition und physischer Darstellung.
- Integration in Nova Display Output Management.
- Unterstützung manueller und automatischer Positionierung.
- Transaktionale Übernahme von Positionsänderungen.
- Synchronisation mit Nova Compositor und Input System.

## 3. Kernfunktionen

- Freie Positionierung von Fenstern.
- Verschieben mittels Maus, Touch und Tastatur.
- Programmatische Positionsanforderungen.
- Automatische Erstpositionierung.
- Zentrierung von Fenstern und Dialogen.
- Positionierung relativ zu übergeordneten Fenstern.
- Unterstützung negativer Desktop-Koordinaten.
- Verschieben zwischen mehreren Displays.
- Anpassung an unterschiedliche DPI-Skalierungen.
- Begrenzung auf zulässige Arbeitsbereiche.
- Wiederherstellung gespeicherter Fensterpositionen.
- Unterstützung von Snap- und Andockfunktionen.

## 4. Normative Anforderungen

- WINDOW-001: Der Window Manager MUSS Fensterpositionen zentral verwalten.
- WINDOW-002: Fensterpositionen MÜSSEN in einem definierten logischen Koordinatensystem gespeichert werden.
- WINDOW-003: Physische Display-Koordinaten MÜSSEN unter Berücksichtigung von Skalierung und Ausrichtung berechnet werden.
- WINDOW-004: Negative Fensterkoordinaten MÜSSEN unterstützt werden.
- WINDOW-005: Positionsänderungen MÜSSEN atomar übernommen werden.
- WINDOW-006: Anwendungen DÜRFEN Positionen anfordern, jedoch keine systemseitigen Platzierungsregeln umgehen.
- WINDOW-007: Benutzerinitiierte Verschiebungen MÜSSEN Vorrang vor konkurrierenden automatischen Positionsanforderungen besitzen.
- WINDOW-008: Fenster MÜSSEN zwischen Displays verschoben werden können.
- WINDOW-009: Unterschiedliche DPI-Skalierungen MÜSSEN beim Displaywechsel berücksichtigt werden.
- WINDOW-010: Fenster MÜSSEN nach Änderungen der Displaykonfiguration erreichbar bleiben.
- WINDOW-011: Systemdialoge SOLLEN relativ zu ihrem übergeordneten Fenster positioniert werden.
- WINDOW-012: Neue Fenster SOLLEN innerhalb eines geeigneten sichtbaren Arbeitsbereichs erscheinen.
- WINDOW-013: Die Positionierung MUSS reservierte Desktopbereiche berücksichtigen.
- WINDOW-014: Fensterbewegungen MÜSSEN dem Compositor konsistent übermittelt werden.
- WINDOW-015: Positionsänderungen MÜSSEN im Surface Damage Tracking berücksichtigt werden.
- WINDOW-016: Gespeicherte Fensterpositionen SOLLEN bei geeigneten Anwendungen wiederhergestellt werden.
- WINDOW-017: Ungültige oder nicht mehr erreichbare Positionen MÜSSEN automatisch korrigiert werden können.
- WINDOW-018: Fensterpositionierung MUSS unabhängig von visuellen Animationen funktionieren.

## 5. Koordinatensystem

NovaOS verwendet ein gemeinsames logisches Desktop-Koordinatensystem.

### Desktop Coordinates

Beschreiben die Position eines Fensters innerhalb des gesamten virtuellen Desktops.

### Display Coordinates

Beschreiben die Position relativ zum Ursprung eines bestimmten Displays.

### Window Coordinates

Beschreiben Positionen relativ zum Ursprung eines Fensters.

### Surface Coordinates

Beschreiben Positionen innerhalb einer grafischen Surface.

Die Umrechnung zwischen den Koordinatensystemen erfolgt über definierte Transformationen.

Fensterpositionen werden unabhängig von der physischen Pixelauflösung gespeichert.

## 6. Positionierungsverfahren

### Manual Positioning

Der Benutzer verschiebt Fenster direkt über unterstützte Eingabemethoden.

### Automatic Positioning

Der Window Manager bestimmt eine geeignete Anfangsposition.

### Centered Positioning

Fenster werden innerhalb eines Displays, Arbeitsbereichs oder übergeordneten Fensters zentriert.

### Relative Positioning

Dialoge und abhängige Fenster werden relativ zu ihrem übergeordneten Fenster positioniert.

### Restored Positioning

Eine zuvor gespeicherte Fensterposition wird wiederhergestellt und an die aktuelle Displaykonfiguration angepasst.

### Snap Positioning

Fenster werden an definierten Bildschirmbereichen oder anderen Fenstern ausgerichtet.

Die eigentliche Snap-Logik wird durch das zuständige Fensterlayoutmodul bereitgestellt.

## 7. Multi-Monitor-Verhalten

- Jedes Display besitzt eine definierte Position im logischen Desktop.
- Displays DÜRFEN unterschiedliche Auflösungen und Skalierungsfaktoren besitzen.
- Fenster können Displaygrenzen überschreiten.
- Das primäre Display bestimmt nicht zwangsläufig die Position neuer Fenster.
- Beim Entfernen eines Displays werden betroffene Fenster auf verbleibende Displays umgeordnet.
- Bei Wiederverbindung eines Displays können gespeicherte Positionen wiederhergestellt werden.
- Der Window Manager bestimmt anhand definierter Regeln das maßgebliche Display eines Fensters.

Die Positionierung MUSS auch bei asymmetrischen Displayanordnungen korrekt funktionieren.

## 8. Positionsverarbeitung

1. **Request:** Positionsänderung empfangen.
2. **Authorize:** Berechtigung und Benutzerinteraktion prüfen.
3. **Resolve:** Zielposition und Koordinatensystem bestimmen.
4. **Transform:** Erforderliche Koordinatenumrechnungen durchführen.
5. **Validate:** Displaygrenzen und Platzierungsregeln prüfen.
6. **Adjust:** Position bei Bedarf korrigieren.
7. **Commit:** Neue Fensterposition atomar übernehmen.
8. **Notify:** Compositor und betroffene Komponenten informieren.
9. **Persist:** Position bei Bedarf speichern.

## 9. Performance und Zuverlässigkeit

- Direkte Verarbeitung interaktiver Fensterbewegungen.
- Keine blockierenden Positionsberechnungen.
- Minimierung unnötiger Surface-Neuberechnungen.
- Wiederverwendung bestehender Grafikbuffer bei reinen Positionsänderungen.
- Partielle Aktualisierung betroffener Bildschirmbereiche.
- Korrekte Verarbeitung schneller Maus- und Touchbewegungen.
- Konsistente Positionierung bei Display-Hotplug.
- Automatische Korrektur nicht erreichbarer Fenster.
- Wiederherstellung gültiger Positionen nach Window-Manager-Fehlern.
- Diagnose von Positionsänderungen und Koordinatentransformationen.

## 10. Ergebnis

NovaOS erhält eine einheitliche und zuverlässige Fensterpositionierung für einzelne und mehrere Displays.

Fenster können frei verschoben, automatisch angeordnet und anhand gespeicherter Positionen wiederhergestellt werden.

Durch logische Desktop-Koordinaten, DPI-unabhängige Positionierung und automatische Korrekturmechanismen bleiben Fenster auch bei wechselnden Displaykonfigurationen erreichbar und intuitiv bedienbar.

**Grundsatz:** Fenster folgen den Entscheidungen des Benutzers und passen sich automatisch an veränderte Bildschirmbedingungen an.