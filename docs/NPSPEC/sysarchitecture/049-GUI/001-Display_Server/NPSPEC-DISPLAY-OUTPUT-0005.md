# NPSPEC-DISPLAY-OUTPUT-0005 – Nova DPI and Scaling Management

**Status:** Angenommen  
**Bereich:** Display / DPI und Skalierung  
**System:** NovaOS

## 1. Zweck

Nova DPI and Scaling Management definiert die Verwaltung von Pixeldichte, Anzeigegrößen und Skalierungsfaktoren.

Ziel ist eine scharfe, konsistente und benutzerfreundliche Darstellung grafischer Oberflächen auf Displays unterschiedlicher Größe und Pixeldichte.

## 2. Architektur

- Integration in den Nova Display Server.
- Unabhängige DPI- und Skalierungsverwaltung pro Display.
- Trennung zwischen logischen Koordinaten und physischen Pixeln.
- Unterstützung ganzzahliger und fraktionaler Skalierung.
- Zusammenarbeit mit Compositor, Window Manager und Nova UI.
- Hardwareunabhängige Berechnung der Skalierungsparameter.
- Dynamische Anpassung bei Display- und Konfigurationsänderungen.
- Berücksichtigung benutzerspezifischer Skalierungspräferenzen.

## 3. Kernfunktionen

- Ermittlung der physischen Displaygröße und Pixeldichte.
- Automatische Berechnung geeigneter Skalierungsfaktoren.
- Manuelle Anpassung der Anzeigeskalierung.
- Unterstützung unterschiedlicher Skalierungen pro Display.
- Transformation zwischen logischen und physischen Koordinaten.
- DPI-bewusstes Rendering grafischer Oberflächen.
- Skalierung von Fenstern, Bedienelementen und Text.
- Anpassung beim Verschieben von Fenstern zwischen Displays.
- Unterstützung hochauflösender Displays (HiDPI).
- Behandlung nicht DPI-fähiger Anwendungen.
- Speicherung und Wiederherstellung von Skalierungseinstellungen.

## 4. Normative Anforderungen

- DISPLAY-001: Jedes Display MUSS einen unabhängigen Skalierungsfaktor besitzen können.
- DISPLAY-002: Logische Koordinaten und physische Pixel MÜSSEN eindeutig unterschieden werden.
- DISPLAY-003: Ganzzahlige und fraktionale Skalierungsfaktoren MÜSSEN unterstützt werden.
- DISPLAY-004: Die Pixeldichte SOLL anhand verfügbarer Displayinformationen automatisch ermittelt werden.
- DISPLAY-005: Fehlende oder ungültige DPI-Informationen MÜSSEN durch geeignete Standardwerte ersetzt werden.
- DISPLAY-006: Benutzer MÜSSEN Skalierungsfaktoren manuell festlegen können.
- DISPLAY-007: Nova UI MUSS DPI-bewusstes Rendering unterstützen.
- DISPLAY-008: Texte und Vektorgrafiken SOLLEN möglichst in der Zielauflösung gerendert werden.
- DISPLAY-009: Fenster MÜSSEN bei einem Wechsel zwischen unterschiedlich skalierten Displays korrekt angepasst werden.
- DISPLAY-010: Eingabekoordinaten MÜSSEN konsistent in das jeweilige logische Koordinatensystem transformiert werden.
- DISPLAY-011: Skalierungsänderungen MÜSSEN ohne Systemneustart möglich sein.
- DISPLAY-012: Anwendungen MÜSSEN über relevante DPI- und Skalierungsänderungen informiert werden.
- DISPLAY-013: Nicht DPI-fähige Anwendungen MÜSSEN über einen kompatiblen Skalierungspfad dargestellt werden können.
- DISPLAY-014: Skalierungsberechnungen MÜSSEN Rundungsfehler und Pixelabweichungen kontrolliert behandeln.
- DISPLAY-015: Skalierungseinstellungen MÜSSEN persistent gespeichert werden können.
- DISPLAY-016: Unterschiedliche Skalierungsfaktoren DÜRFEN die unabhängige Bildausgabe mehrerer Displays nicht beeinträchtigen.

## 5. Skalierungsmodelle

### Logical Scaling
Grafische Oberflächen verwenden displayunabhängige logische Einheiten.

### Integer Scaling
Skalierung mit ganzzahligen Faktoren zur pixelgenauen Vergrößerung.

### Fractional Scaling
Skalierung mit nicht ganzzahligen Faktoren für flexible Anzeigegrößen.

### DPI-Aware Rendering
Anwendungen rendern Inhalte entsprechend der aktuellen Zielskalierung.

### Compatibility Scaling
Nicht DPI-fähige Anwendungen werden durch den Compositor skaliert.

## 6. Fenster- und UI-Skalierung

- Der Display Server stellt die Skalierungsparameter bereit.
- Der Window Manager verwaltet Fenstergeometrien in logischen Koordinaten.
- Der Compositor transformiert und kombiniert Oberflächen für die jeweiligen Displays.
- Nova UI berücksichtigt DPI-Werte bei Layout, Text und Bedienelementen.
- Anwendungen erhalten Benachrichtigungen bei Skalierungsänderungen.
- Fenster auf mehreren Displays verwenden eine eindeutig definierte Zielskalierung.
- Änderungen SOLLEN ohne sichtbares Flackern oder unnötige Neuzeichnungen erfolgen.

## 7. Performance und Zuverlässigkeit

- Bevorzugung nativer Auflösung gegenüber nachträglicher Bitmap-Skalierung.
- GPU-beschleunigte Skalierung, sofern verfügbar.
- Minimierung unnötiger Zwischenbuffer.
- Wiederverwendung gültiger Rendering-Ressourcen.
- Vermeidung mehrfacher Skalierung desselben Inhalts.
- Konsistente Rundung und Pixelzuordnung.
- Kontrollierte Behandlung fehlerhafter DPI-Informationen.
- Automatische Wiederherstellung gültiger Skalierungseinstellungen.

## 8. Ergebnis

NovaOS erhält eine einheitliche, DPI-bewusste Skalierungsarchitektur.

Grafische Oberflächen bleiben auf unterschiedlichen Displaygrößen und Pixeldichten gut lesbar, scharf und konsistent.

Die Architektur unterstützt moderne HiDPI-Displays, fraktionale Skalierung und dynamische Multi-Monitor-Konfigurationen ohne unnötige Einschränkungen für Anwendungen.