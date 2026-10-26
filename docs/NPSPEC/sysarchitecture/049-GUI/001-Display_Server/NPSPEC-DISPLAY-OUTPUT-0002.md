# NPSPEC-DISPLAY-OUTPUT-0002 – Nova Multi-Monitor Management

**Status:** Angenommen  
**Bereich:** Display / Multi-Monitor-Verwaltung  
**System:** NovaOS

## 1. Zweck

Nova Multi-Monitor Management definiert die Verwaltung und Koordination mehrerer Bildschirme innerhalb einer gemeinsamen Desktop-Umgebung.

Ziel ist ein nahtloser, performanter und benutzerfreundlicher Multi-Monitor-Betrieb mit unabhängigen Display-Einstellungen.

## 2. Architektur

- Integration in den Nova Display Server.
- Zentrale Verwaltung der Multi-Monitor-Konfiguration.
- Einheitlicher logischer Desktop-Koordinatenraum.
- Unabhängige Konfiguration jedes Displays.
- Unterstützung unterschiedlicher Grafikkarten.
- Trennung zwischen physischer Display-Anordnung und logischer Desktop-Geometrie.
- Zusammenarbeit mit Compositor, Window Manager und Input System.
- Ereignisgesteuerte Verarbeitung von Konfigurationsänderungen.

## 3. Kernfunktionen

- Automatische Erkennung mehrerer Bildschirme.
- Erweiterter Desktop über mehrere Displays.
- Bildschirmspiegelung und Duplizierung.
- Individuelle Auflösung und Bildwiederholrate.
- Unabhängige Skalierung und Ausrichtung.
- Freie Positionierung der Displays.
- Festlegung und Änderung des primären Displays.
- Verschieben von Fenstern zwischen Displays.
- Verwaltung unterschiedlicher Arbeitsbereiche.
- Dynamisches Hinzufügen und Entfernen von Displays.
- Speicherung und Wiederherstellung von Monitoranordnungen.
- Unterstützung virtueller Displays.

## 4. Normative Anforderungen

- DISPLAY-001: NovaOS MUSS mehrere Displays gleichzeitig unterstützen.
- DISPLAY-002: Jedes Display MUSS unabhängig konfigurierbar sein.
- DISPLAY-003: Alle aktiven Displays MÜSSEN einem definierten logischen Koordinatenraum zugeordnet werden.
- DISPLAY-004: Unterschiedliche Auflösungen, Skalierungen und Bildwiederholraten MÜSSEN unterstützt werden.
- DISPLAY-005: Fenster MÜSSEN zwischen Displays verschoben werden können.
- DISPLAY-006: Der Window Manager MUSS Fensterpositionen bei Display-Änderungen anpassen können.
- DISPLAY-007: Beim Entfernen eines Displays MÜSSEN betroffene Fenster auf verbleibenden Displays erreichbar bleiben.
- DISPLAY-008: Display-Anordnungen MÜSSEN persistent gespeichert werden können.
- DISPLAY-009: Bekannte Display-Konfigurationen SOLLEN automatisch wiederhergestellt werden.
- DISPLAY-010: Der Compositor MUSS die Bildausgabe pro Display unabhängig synchronisieren können.
- DISPLAY-011: Unterschiedliche Bildwiederholraten DÜRFEN sich nicht gegenseitig unnötig begrenzen.
- DISPLAY-012: Bildschirmspiegelung MUSS unterschiedliche Ausgabeauflösungen berücksichtigen.
- DISPLAY-013: Änderungen der Monitoranordnung MÜSSEN ohne Neustart der Desktop-Umgebung möglich sein.
- DISPLAY-014: Eingabekoordinaten MÜSSEN korrekt zwischen logischem Desktop und physischen Displays transformiert werden.
- DISPLAY-015: Privilegierte Konfigurationsänderungen MÜSSEN durch das NovaOS-Capability-System autorisiert werden.
- DISPLAY-016: Die Multi-Monitor-Verwaltung MUSS auch mit Displays unterschiedlicher Grafikkarten funktionieren.

## 5. Betriebsmodi

### Extended Desktop
Mehrere Displays bilden eine gemeinsame Arbeitsfläche mit frei positionierbaren Fenstern.

### Mirrored Desktop
Mehrere Displays zeigen denselben Desktopinhalt mit bedarfsgerechter Skalierung.

### Independent Workspaces
Displays können unterschiedliche virtuelle Arbeitsbereiche darstellen.

### Single Display
Ein ausgewähltes Display wird verwendet, während andere Ausgänge deaktiviert bleiben.

### Dynamic Configuration
Displays können während des Betriebs hinzugefügt, entfernt oder neu angeordnet werden.

## 6. Fenster- und Eingabeverwaltung

- Der Window Manager verwaltet Fensterpositionen im logischen Koordinatenraum.
- Der Compositor übernimmt die displaybezogene Darstellung.
- Das Input System transformiert Zeigerkoordinaten entsprechend der Display-Anordnung.
- Fensterpositionen und Größen werden bei Skalierungsänderungen angepasst.
- Vollbildanwendungen können gezielt einem Display zugeordnet werden.
- Systemdialoge erscheinen auf einem geeigneten aktiven Display.
- Beim Trennen eines Displays werden nicht mehr erreichbare Fenster automatisch umpositioniert.

## 7. Performance und Zuverlässigkeit

- Unabhängiges Frame-Timing pro Display.
- Vermeidung unnötiger Renderoperationen auf unveränderten Displays.
- Unterstützung von Direct Scanout und Hardware-Overlays.
- Minimierung von Speicherübertragungen zwischen Grafikkarten.
- Wiederverwendung bestehender Grafikressourcen.
- Atomare Konfigurationsänderungen, sofern technisch möglich.
- Automatische Wiederherstellung der letzten gültigen Monitoranordnung.
- Kontrollierte Behandlung von Hotplug- und Treiberfehlern.

## 8. Ergebnis

NovaOS erhält eine flexible und leistungsfähige Multi-Monitor-Verwaltung.

Unterschiedliche Displays können unabhängig konfiguriert und zu einer gemeinsamen Desktop-Umgebung verbunden werden.

Fenster, Eingaben und Bildausgabe passen sich dynamisch an Änderungen der Monitoranordnung an, ohne den Arbeitsfluss des Benutzers unnötig zu unterbrechen.