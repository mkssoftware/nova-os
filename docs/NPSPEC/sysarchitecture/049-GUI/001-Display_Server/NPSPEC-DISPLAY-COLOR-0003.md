# NPSPEC-DISPLAY-COLOR-0003 – Nova Gamma and Brightness Control

**Status:** Angenommen  
**Bereich:** Display / Gamma und Helligkeit  
**System:** NovaOS

## 1. Zweck

Nova Gamma and Brightness Control definiert die Verwaltung und Anpassung von Gamma, Helligkeit und verwandten Anzeigeparametern.

Ziel ist eine konsistente, augenschonende und energieeffiziente Bilddarstellung unter Berücksichtigung der Display-Hardware und Benutzerpräferenzen.

## 2. Architektur

- Integration in Nova Color Management und den Nova Display Server.
- Unabhängige Steuerung pro Display.
- Hardwareunabhängige Schnittstelle zu Grafik- und Displaytreibern.
- Trennung zwischen physischer Display-Helligkeit und digitaler Bildkorrektur.
- Unterstützung hardwarebasierter Gamma-LUTs und Helligkeitssteuerung.
- Softwarebasierte Korrektur als Fallback.
- Ereignisgesteuerte Verarbeitung von Änderungen.
- Integration in das NovaOS-Energie- und Konfigurationsmanagement.

## 3. Kernfunktionen

- Einstellung der Display-Helligkeit.
- Verwaltung von Gamma-Korrektur und Tonwertkurven.
- Unterstützung individueller RGB-Gamma-Kurven.
- Steuerung verfügbarer Hintergrundbeleuchtung.
- Automatische Helligkeitsanpassung über Umgebungslichtsensoren.
- Anpassung an Energiezustand und Benutzerpräferenzen.
- Unterstützung zeitabhängiger Helligkeitsprofile.
- Verwaltung unterschiedlicher Einstellungen pro Display.
- Speicherung und Wiederherstellung bevorzugter Werte.
- Wiederherstellung sicherer Standardwerte.
- Bereitstellung von Steuerungsinformationen für Nova UI.

## 4. Normative Anforderungen

- COLOR-001: Helligkeit und Gamma MÜSSEN unabhängig voneinander verwaltet werden.
- COLOR-002: Jedes Display MUSS individuelle Einstellungen unterstützen können.
- COLOR-003: Hardwarebasierte Helligkeitssteuerung SOLL bevorzugt werden.
- COLOR-004: Digitale Helligkeitskorrektur DARF nicht als gleichwertiger Ersatz für physische Hintergrundbeleuchtungssteuerung betrachtet werden.
- COLOR-005: Gamma-Korrekturen MÜSSEN mit dem Nova Color Management abgestimmt werden.
- COLOR-006: Verfügbare Hardwarefunktionen MÜSSEN über die zuständigen Treiber ermittelt werden.
- COLOR-007: Nicht unterstützte Steuerungsfunktionen MÜSSEN kontrolliert behandelt werden.
- COLOR-008: Änderungen MÜSSEN während des laufenden Betriebs möglich sein.
- COLOR-009: Automatische Helligkeitsanpassungen MÜSSEN vom Benutzer deaktiviert werden können.
- COLOR-010: Manuelle Benutzereinstellungen MÜSSEN gegenüber automatischen Anpassungen priorisiert werden.
- COLOR-011: Sensorwerte MÜSSEN vor ihrer Verwendung auf Plausibilität geprüft werden.
- COLOR-012: Helligkeitsänderungen SOLLEN ohne sichtbare abrupte Übergänge erfolgen.
- COLOR-013: Einstellungen MÜSSEN persistent gespeichert werden können.
- COLOR-014: Fehlerhafte Einstellungen DÜRFEN nicht zum dauerhaften Verlust einer lesbaren Bildausgabe führen.
- COLOR-015: Änderungen an geschützten Hardwareparametern MÜSSEN durch das NovaOS-Capability-System autorisiert werden.
- COLOR-016: SDR- und HDR-Ausgabepfade MÜSSEN hinsichtlich ihrer unterschiedlichen Helligkeits- und Übertragungsfunktionen berücksichtigt werden.

## 5. Steuerungsbereiche

### Hardware Brightness
Physische Helligkeitssteuerung über Hintergrundbeleuchtung oder vergleichbare Hardwarefunktionen.

### Software Brightness
Digitale Anpassung der Bildhelligkeit, sofern keine geeignete Hardwaresteuerung verfügbar ist.

### Gamma Correction
Anpassung der Tonwertwiedergabe über geeignete Übertragungsfunktionen oder Lookup-Tabellen.

### Automatic Brightness
Sensorbasierte Anpassung an die Umgebungshelligkeit.

### Adaptive Brightness
Anpassung anhand von Energiezustand, Nutzungssituation und Benutzerpräferenzen.

### Brightness Profiles
Gespeicherte Einstellungen für unterschiedliche Nutzungssituationen.

## 6. Verarbeitung

1. **Detect:** Unterstützte Hardwarefunktionen ermitteln.
2. **Load:** Gespeicherte Einstellungen laden.
3. **Validate:** Werte und Hardwaregrenzen prüfen.
4. **Calculate:** Erforderliche Helligkeits- und Gamma-Anpassungen berechnen.
5. **Apply:** Einstellungen über Hardware oder Farbverarbeitung anwenden.
6. **Notify:** Betroffene Systemkomponenten informieren.
7. **Persist:** Bestätigte Benutzereinstellungen speichern.

Automatische Anpassungen SOLLEN mit Hysterese und zeitlicher Glättung erfolgen, um sichtbare Helligkeitsschwankungen zu vermeiden.

## 7. Performance und Zuverlässigkeit

- Bevorzugung energieeffizienter Hardwaresteuerung.
- Nutzung vorhandener Gamma-LUTs.
- Vermeidung unnötiger Neuberechnungen.
- Zusammenfassung kurz aufeinanderfolgender Änderungen.
- Asynchrone Verarbeitung langsamer Hardwarezugriffe.
- Minimierung zusätzlicher Renderoperationen.
- Schutz vor unzulässigen Helligkeitswerten.
- Wiederherstellung lesbarer Standardwerte bei Fehlern.
- Vermeidung von Konflikten zwischen Farbkalibrierung und Gamma-Korrektur.
- Keine unnötige Beeinträchtigung der HDR-Darstellung.

## 8. Ergebnis

NovaOS erhält eine zentrale, hardwareunabhängige Verwaltung von Gamma und Display-Helligkeit.

Benutzer können die Darstellung individuell anpassen, während automatische Mechanismen für angenehme Sichtverhältnisse und einen energieeffizienten Betrieb sorgen.

Die Architektur gewährleistet eine konsistente Zusammenarbeit mit dem Nova Color Management und unterstützt unterschiedliche Display-Technologien.