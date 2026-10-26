# NPSPEC-DISPLAY-OUTPUT-0006 – Nova Display Rotation and Orientation

**Status:** Angenommen  
**Bereich:** Display / Rotation und Ausrichtung  
**System:** NovaOS

## 1. Zweck

Nova Display Rotation and Orientation definiert die Verwaltung der Bildschirmausrichtung und Rotation physischer und virtueller Displays.

Ziel ist eine flexible, performante und dynamische Anpassung der Bildausgabe an unterschiedliche Display-Ausrichtungen.

## 2. Architektur

- Integration in den Nova Display Server.
- Unabhängige Rotationsverwaltung pro Display.
- Hardwareunabhängige Schnittstelle zu den Grafiktreibern.
- Zusammenarbeit mit Compositor, Window Manager und Input System.
- Trennung zwischen physischer Ausrichtung und logischer Desktop-Geometrie.
- Unterstützung manueller und automatischer Ausrichtungsänderungen.
- Ereignisgesteuerte Verarbeitung von Orientierungsänderungen.
- Integration in das NovaOS-Capability-System.

## 3. Kernfunktionen

- Unterstützung von 0°, 90°, 180° und 270° Rotation.
- Horizontale und vertikale Spiegelung.
- Automatische Erkennung der Geräteausrichtung über verfügbare Sensoren.
- Manuelle Einstellung der Display-Ausrichtung.
- Unabhängige Rotation mehrerer Displays.
- Anpassung der logischen Display-Geometrie.
- Transformation von Eingabe- und Zeigerkoordinaten.
- Anpassung von Fensterpositionen und Arbeitsbereichen.
- Speicherung und Wiederherstellung bevorzugter Ausrichtungen.
- Dynamische Ausrichtungsänderungen während des Betriebs.

## 4. Normative Anforderungen

- DISPLAY-001: Jedes Display MUSS unabhängig rotiert werden können.
- DISPLAY-002: Rotationen um 0°, 90°, 180° und 270° MÜSSEN unterstützt werden.
- DISPLAY-003: Horizontale und vertikale Spiegelung MÜSSEN unterstützt werden.
- DISPLAY-004: Rotationsänderungen MÜSSEN ohne Systemneustart möglich sein.
- DISPLAY-005: Die logische Display-Geometrie MUSS entsprechend der Ausrichtung aktualisiert werden.
- DISPLAY-006: Eingabekoordinaten MÜSSEN korrekt an die aktuelle Transformation angepasst werden.
- DISPLAY-007: Der Compositor MUSS die Bildausgabe entsprechend der Display-Ausrichtung transformieren.
- DISPLAY-008: Hardwarebeschleunigte Rotation SOLL bevorzugt werden, sofern verfügbar.
- DISPLAY-009: Automatische Rotation DARF nur bei verfügbaren und autorisierten Orientierungssensoren aktiviert werden.
- DISPLAY-010: Benutzer MÜSSEN die automatische Rotation deaktivieren können.
- DISPLAY-011: Manuell festgelegte Ausrichtungen MÜSSEN gegenüber automatischen Änderungen priorisiert werden.
- DISPLAY-012: Der Window Manager MUSS Änderungen der verfügbaren Display-Geometrie berücksichtigen.
- DISPLAY-013: Anwendungen MÜSSEN über relevante Geometrieänderungen informiert werden.
- DISPLAY-014: Rotation und Skalierung MÜSSEN gemeinsam korrekt verarbeitet werden.
- DISPLAY-015: Ausrichtungseinstellungen MÜSSEN persistent gespeichert werden können.
- DISPLAY-016: Fehlgeschlagene Ausrichtungsänderungen MÜSSEN kontrolliert zurückgesetzt werden.

## 5. Ausrichtungsmodi

### Landscape
Horizontale Standardausrichtung mit 0° Rotation.

### Portrait
Vertikale Ausrichtung mit 90° oder 270° Rotation.

### Inverted Landscape
Umgekehrte horizontale Ausrichtung mit 180° Rotation.

### Mirrored
Horizontale oder vertikale Spiegelung der Bildausgabe.

### Automatic Orientation
Sensorbasierte Anpassung der Display-Ausrichtung.

### Orientation Lock
Sperrung der aktuellen Ausrichtung gegen automatische Änderungen.

## 6. Transformation und Eingabeverarbeitung

- Der Display Server verwaltet die aktuelle Display-Transformation.
- Der Compositor übernimmt die Rotation und Spiegelung der Bildausgabe.
- Der Window Manager aktualisiert die logische Desktop-Geometrie.
- Das Input System transformiert Maus-, Touch- und Stifteingaben.
- Rotations- und Skalierungstransformationen werden konsistent kombiniert.
- Fenster und Systemoberflächen bleiben nach Ausrichtungsänderungen erreichbar.
- Anwendungen verwenden weiterhin ein definiertes logisches Koordinatensystem.

## 7. Performance und Zuverlässigkeit

- Nutzung hardwarebeschleunigter Display-Transformationen.
- GPU-basierte Rotation, falls keine geeignete Display-Hardwarefunktion verfügbar ist.
- Software-Fallback bei fehlender Hardwarebeschleunigung.
- Vermeidung unnötiger Zwischenbuffer und Speicherkopien.
- Wiederverwendung kompatibler Grafikressourcen.
- Minimierung sichtbarer Unterbrechungen bei Ausrichtungsänderungen.
- Filterung fehlerhafter oder instabiler Sensorereignisse.
- Wiederherstellung der letzten gültigen Ausrichtung bei Fehlern.

## 8. Ergebnis

NovaOS erhält eine flexible und hardwareunabhängige Verwaltung der Display-Ausrichtung.

Physische und virtuelle Displays können unabhängig rotiert und gespiegelt werden.

Die Architektur ermöglicht dynamische Orientierungsänderungen, korrekte Eingabetransformationen und eine konsistente Darstellung auf Desktop-, Tablet- und anderen unterstützten Geräten.