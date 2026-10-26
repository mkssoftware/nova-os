# NPSPEC-DISPLAY-OUTPUT-0003 – Nova Display Mode Management

**Status:** Angenommen  
**Bereich:** Display / Anzeigemodi  
**System:** NovaOS

## 1. Zweck

Nova Display Mode Management definiert die Erkennung, Auswahl und Verwaltung unterstützter Anzeigemodi.

Ziel ist eine zuverlässige, performante und dynamische Konfiguration von Auflösung, Bildwiederholrate, Pixelformat und weiteren Anzeigeparametern.

## 2. Architektur

- Integration in den Nova Display Server.
- Hardwareunabhängige Verwaltung über definierte Grafiktreiber-Schnittstellen.
- Unabhängige Modusverwaltung für jeden Display-Ausgang.
- Unterstützung physischer und virtueller Displays.
- Trennung zwischen Moduserkennung, Validierung und Aktivierung.
- Transaktionsbasierte Konfigurationsänderungen.
- Ereignisgesteuerte Verarbeitung von Modusänderungen.
- Integration in das NovaOS-Capability-System.

## 3. Kernfunktionen

- Ermittlung unterstützter Anzeigemodi.
- Auswertung von EDID und vergleichbaren Display-Informationen.
- Verwaltung von Auflösungen und Bildwiederholraten.
- Unterstützung unterschiedlicher Pixelformate und Farbtiefen.
- Erkennung bevorzugter und nativer Anzeigemodi.
- Auswahl geeigneter Standardmodi.
- Dynamischer Wechsel zwischen Anzeigemodi.
- Validierung von Hardware- und Bandbreitenbeschränkungen.
- Unterstützung benutzerdefinierter Modi, sofern technisch möglich.
- Speicherung und Wiederherstellung bevorzugter Einstellungen.
- Automatischer Rückfall auf funktionierende Anzeigemodi.

## 4. Normative Anforderungen

- DISPLAY-001: Unterstützte Anzeigemodi MÜSSEN über die zuständigen Grafiktreiber ermittelt werden.
- DISPLAY-002: Jeder Anzeigemodus MUSS eindeutig einem Display-Ausgang zugeordnet werden können.
- DISPLAY-003: Auflösung, Bildwiederholrate und Pixelformat MÜSSEN als getrennte Moduseigenschaften verwaltet werden.
- DISPLAY-004: Bevorzugte und native Anzeigemodi SOLLEN automatisch erkannt werden.
- DISPLAY-005: Anzeigemodi MÜSSEN vor ihrer Aktivierung auf technische Gültigkeit geprüft werden.
- DISPLAY-006: Modusänderungen MÜSSEN während des laufenden Betriebs möglich sein, sofern die Hardware dies unterstützt.
- DISPLAY-007: Fehlgeschlagene Modusänderungen DÜRFEN nicht zum dauerhaften Verlust der Bildausgabe führen.
- DISPLAY-008: Die letzte gültige Konfiguration MUSS wiederherstellbar sein.
- DISPLAY-009: Benutzerdefinierte Modi DÜRFEN nur innerhalb validierter Hardwaregrenzen aktiviert werden.
- DISPLAY-010: Unterschiedliche Bildwiederholraten MÜSSEN pro Display unterstützt werden.
- DISPLAY-011: Der Compositor MUSS über relevante Modusänderungen informiert werden.
- DISPLAY-012: Grafikressourcen MÜSSEN bei Modusänderungen kontrolliert angepasst werden.
- DISPLAY-013: Modusänderungen MÜSSEN durch das NovaOS-Capability-System autorisiert werden.
- DISPLAY-014: Nicht unterstützte Modi MÜSSEN kontrolliert abgewiesen werden.
- DISPLAY-015: Modusänderungen SOLLEN ohne unnötige Neuinitialisierung des gesamten Grafiksystems erfolgen.

## 5. Anzeigemodus-Eigenschaften

### Resolution
Horizontale und vertikale Pixelauflösung des Displays.

### Refresh Rate
Bildwiederholrate einschließlich nicht ganzzahliger Frequenzen.

### Pixel Format
Pixelformat und Farbtiefe der Bildausgabe.

### Timing
Synchronisationsparameter und Pixeltakt des Anzeigemodus.

### Preferred Mode
Vom Display oder System bevorzugter Anzeigemodus.

### Custom Mode
Benutzerdefinierter, durch den Grafiktreiber validierter Anzeigemodus.

## 6. Moduswechsel

1. **Discover:** Verfügbare Anzeigemodi ermitteln.
2. **Select:** Gewünschten Modus auswählen.
3. **Validate:** Hardwarekompatibilität und Ressourcen prüfen.
4. **Prepare:** Erforderliche Grafikressourcen vorbereiten.
5. **Apply:** Anzeigemodus aktivieren.
6. **Confirm:** Erfolgreiche Bildausgabe bestätigen.
7. **Commit:** Neue Konfiguration übernehmen oder bei Fehlern zurücksetzen.

Bei interaktiven Modusänderungen SOLL eine zeitlich begrenzte Benutzerbestätigung erfolgen, bevor die Konfiguration dauerhaft gespeichert wird.

## 7. Performance und Zuverlässigkeit

- Atomare Moduswechsel, sofern hardwareseitig unterstützt.
- Minimierung sichtbarer Bildunterbrechungen.
- Wiederverwendung kompatibler Grafikressourcen.
- Vermeidung unnötiger Display-Neuinitialisierungen.
- Automatischer Fallback bei fehlgeschlagenen Moduswechseln.
- Unterstützung unterschiedlicher Modi auf mehreren Grafikkarten.
- Validierung von Speicher- und Übertragungsbandbreite.
- Wiederherstellung sicherer Standardmodi bei ungültigen Konfigurationen.

## 8. Ergebnis

NovaOS erhält eine zentrale, hardwareunabhängige Verwaltung sämtlicher Anzeigemodi.

Auflösung, Bildwiederholrate und weitere Anzeigeparameter können dynamisch und unabhängig pro Display konfiguriert werden.

Validierte Moduswechsel, automatische Wiederherstellung und transaktionsbasierte Konfigurationsänderungen gewährleisten eine zuverlässige Bildausgabe.