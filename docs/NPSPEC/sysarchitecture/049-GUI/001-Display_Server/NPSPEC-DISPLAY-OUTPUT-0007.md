# NPSPEC-DISPLAY-OUTPUT-0007 – Nova Hotplug Detection

**Status:** Angenommen  
**Bereich:** Display / Hardwareerkennung  
**System:** NovaOS

## 1. Zweck

Nova Hotplug Detection definiert die automatische Erkennung und Verarbeitung von Änderungen angeschlossener Bildschirme.

Ziel ist die dynamische Verwaltung von Display-Verbindungen ohne Systemneustart oder Unterbrechung der Benutzersitzung.

## 2. Architektur

- Integration in den Nova Display Server.
- Ereignisgesteuerte Erkennung über Grafiktreiber und Hardware-Schnittstellen.
- Unterstützung physischer und virtueller Display-Ausgänge.
- Asynchrone Verarbeitung von Verbindungsänderungen.
- Trennung zwischen Hardwareerkennung und Display-Konfiguration.
- Zusammenarbeit mit Compositor, Window Manager und Input System.
- Integration in Nova Display Lifecycle und Recovery.
- Keine Abhängigkeit von Nova UI.

## 3. Kernfunktionen

- Erkennung angeschlossener und entfernter Displays.
- Verarbeitung von Hardware-Hotplug-Ereignissen.
- Erkennung von Änderungen der Display-Eigenschaften.
- Auslesen und Aktualisieren von EDID-Informationen.
- Identifikation bereits bekannter Displays.
- Automatische Initialisierung neuer Display-Ausgänge.
- Wiederherstellung gespeicherter Display-Konfigurationen.
- Aktualisierung der Desktop-Geometrie.
- Benachrichtigung betroffener Systemkomponenten.
- Behandlung instabiler oder fehlerhafter Verbindungen.

## 4. Normative Anforderungen

- DISPLAY-001: Display-Verbindungsänderungen MÜSSEN während des Betriebs erkannt werden.
- DISPLAY-002: Hardware-Hotplug-Ereignisse MÜSSEN asynchron verarbeitet werden.
- DISPLAY-003: Der tatsächliche Verbindungszustand MUSS vor einer Konfigurationsänderung überprüft werden.
- DISPLAY-004: Neue Displays MÜSSEN automatisch identifiziert und registriert werden können.
- DISPLAY-005: Entfernte Displays MÜSSEN kontrolliert deaktiviert werden.
- DISPLAY-006: Bekannte Display-Konfigurationen SOLLEN automatisch wiederhergestellt werden.
- DISPLAY-007: Der Compositor MUSS über Änderungen aktiver Display-Ausgänge informiert werden.
- DISPLAY-008: Der Window Manager MUSS die Desktop-Geometrie entsprechend aktualisieren.
- DISPLAY-009: Fenster entfernter Displays MÜSSEN auf verbleibenden Displays erreichbar bleiben.
- DISPLAY-010: Mehrfach auftretende oder instabile Hotplug-Ereignisse MÜSSEN zusammengefasst beziehungsweise entprellt werden.
- DISPLAY-011: Fehlerhafte Display-Informationen DÜRFEN den Display Server nicht destabilisieren.
- DISPLAY-012: Hotplug-Änderungen DÜRFEN keine unautorisierten Display-Zugriffe ermöglichen.
- DISPLAY-013: Nicht betroffene Displays SOLLEN ohne Unterbrechung weiterarbeiten.
- DISPLAY-014: Die Erkennung MUSS mehrere Grafikkarten unterstützen.
- DISPLAY-015: Bei Verlust aller Displays MUSS eine spätere Wiederherstellung der Bildausgabe möglich bleiben.
- DISPLAY-016: Hotplug-Ereignisse MÜSSEN mit einer definierten Reihenfolge und Zustandsversion verarbeitet werden.

## 5. Hotplug-Lebenszyklus

1. **Detect:** Verbindungsänderung erkennen.
2. **Verify:** Tatsächlichen Hardwarezustand überprüfen.
3. **Identify:** Display und unterstützte Eigenschaften ermitteln.
4. **Configure:** Geeignete Display-Konfiguration vorbereiten.
5. **Apply:** Display aktivieren, aktualisieren oder entfernen.
6. **Notify:** Betroffene Systemkomponenten informieren.
7. **Persist:** Gültige Konfiguration bei Bedarf speichern.

## 6. Fehlerbehandlung und Performance

- Ereignisbasierte Erkennung statt dauerhaftem Polling.
- Entprellung kurzzeitig instabiler Verbindungen.
- Zusammenfassung mehrfacher Hardwareereignisse.
- Zeitlimits für Display-Initialisierung und Hardwareabfragen.
- Kontrollierte Behandlung ungültiger EDID-Daten.
- Wiederholungsversuche bei vorübergehenden Verbindungsfehlern.
- Vermeidung unnötiger Neuinitialisierung bestehender Displays.
- Wiederverwendung gültiger Grafikressourcen.
- Automatische Wiederherstellung nach Verbindungsproblemen.
- Protokollierung relevanter Hardwareänderungen und Fehler.

## 7. Ergebnis

NovaOS erhält eine zuverlässige und vollständig dynamische Hotplug-Erkennung für Bildschirmausgänge.

Displays können während des Betriebs angeschlossen, entfernt oder ausgetauscht werden, ohne die Desktop-Sitzung neu starten zu müssen.

Bestehende Arbeitsbereiche bleiben möglichst erhalten, während neue Displays automatisch erkannt und entsprechend den Benutzerpräferenzen konfiguriert werden.