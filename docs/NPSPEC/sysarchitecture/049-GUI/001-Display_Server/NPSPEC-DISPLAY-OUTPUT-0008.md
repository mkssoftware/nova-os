# NPSPEC-DISPLAY-OUTPUT-0008 – Nova Display Configuration Persistence

**Status:** Angenommen  
**Bereich:** Display / Konfigurationsverwaltung  
**System:** NovaOS

## 1. Zweck

Nova Display Configuration Persistence definiert die dauerhafte Speicherung, Wiederherstellung und Verwaltung von Display-Konfigurationen.

Ziel ist die automatische Wiederherstellung bevorzugter Anzeigeeinstellungen beim Systemstart, bei Benutzerwechseln und bei Änderungen angeschlossener Displays.

## 2. Architektur

- Integration in den Nova Display Server.
- Nutzung des NovaOS-Konfigurations- und Speichersystems.
- Trennung zwischen systemweiten und benutzerspezifischen Einstellungen.
- Persistente Identifikation bekannter Displays.
- Versionierte und transaktionsbasierte Konfigurationsspeicherung.
- Ereignisgesteuerte Aktualisierung bei Konfigurationsänderungen.
- Unterstützung mehrerer Display-Profile.
- Keine Abhängigkeit von Nova UI oder einer externen Datenbank.

## 3. Kernfunktionen

- Speicherung von Display-Auflösung und Bildwiederholrate.
- Speicherung von DPI, Skalierung und Ausrichtung.
- Speicherung von Monitorpositionen und Desktop-Anordnungen.
- Verwaltung primärer und sekundärer Displays.
- Speicherung von Spiegelungs- und Erweiterungsmodi.
- Automatische Erkennung bekannter Display-Konfigurationen.
- Wiederherstellung benutzerspezifischer Einstellungen.
- Verwaltung unterschiedlicher Hardwarekonfigurationen.
- Speicherung der letzten gültigen Konfiguration.
- Rücksetzen auf sichere Standardeinstellungen.
- Migration älterer Konfigurationsversionen.

## 4. Normative Anforderungen

- DISPLAY-001: Display-Konfigurationen MÜSSEN persistent gespeichert werden können.
- DISPLAY-002: Systemweite und benutzerspezifische Einstellungen MÜSSEN getrennt verwaltet werden.
- DISPLAY-003: Bekannte Displays MÜSSEN anhand geeigneter Identifikationsmerkmale wiedererkannt werden können.
- DISPLAY-004: Gespeicherte Konfigurationen MÜSSEN vor ihrer Anwendung validiert werden.
- DISPLAY-005: Konfigurationsänderungen MÜSSEN atomar gespeichert werden.
- DISPLAY-006: Die letzte bestätigte, funktionsfähige Konfiguration MUSS wiederherstellbar bleiben.
- DISPLAY-007: Fehlgeschlagene Konfigurationsänderungen MÜSSEN zurückgesetzt werden können.
- DISPLAY-008: Bekannte Monitoranordnungen SOLLEN automatisch wiederhergestellt werden.
- DISPLAY-009: Nicht verfügbare Displays DÜRFEN die Initialisierung vorhandener Displays nicht blockieren.
- DISPLAY-010: Benutzerpräferenzen MÜSSEN gegenüber automatisch ermittelten Standardwerten priorisiert werden, sofern sie gültig sind.
- DISPLAY-011: Konfigurationsdateien MÜSSEN versioniert und auf Integrität geprüft werden.
- DISPLAY-012: Ungültige oder beschädigte Konfigurationen DÜRFEN den Display Server nicht destabilisieren.
- DISPLAY-013: Konfigurationsänderungen MÜSSEN die Berechtigungen des NovaOS-Capability-Systems berücksichtigen.
- DISPLAY-014: Die Wiederherstellung MUSS ohne vollständigen Neustart des Displaysystems möglich sein.
- DISPLAY-015: Veraltete Konfigurationsversionen MÜSSEN kontrolliert migriert oder abgewiesen werden.
- DISPLAY-016: Nicht bestätigte Anzeigeänderungen DÜRFEN eine gültige Konfiguration nicht dauerhaft überschreiben.

## 5. Konfigurationsbereiche

### Display Identity
Persistente Zuordnung physischer und virtueller Displays anhand verfügbarer Identifikationsmerkmale.

### Display Settings
Auflösung, Bildwiederholrate, Pixelformat, Skalierung und Ausrichtung.

### Display Layout
Monitorpositionen, primäres Display, Spiegelung und erweiterter Desktop.

### User Preferences
Benutzerspezifische Anzeigeeinstellungen und bevorzugte Monitoranordnungen.

### Display Profiles
Gespeicherte Konfigurationen für unterschiedliche Kombinationen angeschlossener Displays.

### Recovery Configuration
Letzte bestätigte Konfiguration und sichere Standardeinstellungen.

## 6. Speicherung und Wiederherstellung

1. **Detect:** Aktuelle Display-Hardware erkennen.
2. **Identify:** Bekannte Displays und Profile zuordnen.
3. **Load:** Passende Konfiguration laden.
4. **Validate:** Einstellungen gegen aktuelle Hardwaremöglichkeiten prüfen.
5. **Apply:** Gültige Konfiguration aktivieren.
6. **Confirm:** Funktionsfähigkeit überprüfen und gegebenenfalls Benutzerbestätigung einholen.
7. **Commit:** Bestätigte Änderungen dauerhaft speichern.

Bei fehlgeschlagener Wiederherstellung MUSS eine sichere Ersatzkonfiguration verwendet werden.

## 7. Performance und Zuverlässigkeit

- Asynchrones Laden nicht kritischer Konfigurationen.
- Frühzeitige Verwendung einer gültigen Standardkonfiguration beim Systemstart.
- Vermeidung unnötiger Schreibzugriffe.
- Zusammenfassung kurz aufeinanderfolgender Änderungen.
- Atomare Speicherung zum Schutz vor Datenverlust.
- Integritätsprüfung gespeicherter Einstellungen.
- Wiederherstellung nach beschädigten Konfigurationsdateien.
- Keine unnötige Neuinitialisierung unveränderter Displays.
- Schutz vor wiederholten fehlerhaften Konfigurationsversuchen.
- Automatische Anpassung bei veränderter Hardware.

## 8. Ergebnis

NovaOS erhält eine zuverlässige, versionierte und benutzerspezifische Speicherung seiner Display-Konfigurationen.

Bekannte Bildschirme und Monitoranordnungen werden automatisch wiederhergestellt, während ungültige Einstellungen kontrolliert behandelt werden.

Die Architektur ermöglicht einen schnellen Systemstart, flexible Hardwarewechsel und die sichere Wiederherstellung funktionierender Anzeigeeinstellungen.