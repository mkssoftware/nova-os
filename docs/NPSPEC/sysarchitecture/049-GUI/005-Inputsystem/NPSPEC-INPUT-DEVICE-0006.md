# NPSPEC-INPUT-DEVICE-0006 – Nova Input Hotplug

**Status:** Angenommen  
**Bereich:** Input / Device / Hotplug  
**System:** NovaOS

## 1. Zweck

Nova Input Hotplug definiert die dynamische Erkennung, Initialisierung, Entfernung und Wiederverbindung von Eingabegeräten während des laufenden Systembetriebs.

Ziel ist eine unterbrechungsfreie Eingabeverarbeitung, bei der Geräte ohne Neustart hinzugefügt, entfernt oder ausgetauscht werden können.

Die Architektur gewährleistet konsistente Gerätezustände, sichere Ressourcenverwaltung und die automatische Wiederherstellung vorhandener Gerätekonfigurationen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Input Hotplug Manager | Koordination von Geräteänderungen |
| Device Discovery Adapter | Übernahme von Erkennungsereignissen der Gerätebusse |
| Device Lifecycle Controller | Steuerung des Gerätelebenszyklus |
| Device Configuration Resolver | Zuordnung gespeicherter Gerätekonfigurationen |
| Input State Cleanup | Bereinigung aktiver Eingabezustände |
| Device Notification Provider | Bereitstellung von Geräteänderungsereignissen |

Die Hardwareerkennung erfolgt durch die zuständigen Bus- und Gerätetreiber.

Nova Input Hotplug übernimmt die geräteübergreifende Koordination innerhalb der Input-Architektur.

## 3. Gerätelebenszyklus

Ein Eingabegerät durchläuft folgende Zustände:

1. **Detected:** Das Gerät wurde erkannt.
2. **Initializing:** Treiber, Deskriptoren und Capabilities werden initialisiert.
3. **Ready:** Das Gerät ist für Eingaben verfügbar.
4. **Suspended:** Das Gerät ist vorübergehend inaktiv.
5. **Disconnecting:** Die Entfernung wird verarbeitet.
6. **Disconnected:** Das Gerät ist nicht mehr verfügbar.
7. **Released:** Sämtliche zugehörigen Ressourcen wurden freigegeben.

Fehlgeschlagene Initialisierungen führen in einen definierten Fehlerzustand.

Eingabeereignisse DÜRFEN erst nach erfolgreichem Übergang in den Zustand `Ready` regulär zugestellt werden.

## 4. Geräteerkennung und Initialisierung

Bei der Erkennung eines neuen Geräts erfolgt:

1. Überprüfung der Geräteinformationen.
2. Auswahl eines geeigneten Eingabetreibers.
3. Initialisierung der Gerätefunktionen.
4. Registrierung in der Input Device Registry.
5. Ermittlung der Device Capabilities.
6. Zuordnung zu Input Seat und Benutzersitzung.
7. Wiederherstellung zulässiger Geräteeinstellungen.
8. Freigabe für die Eingabeverarbeitung.

Die Initialisierung MUSS unabhängig von anderen Eingabegeräten erfolgen.

Fehler eines einzelnen Geräts DÜRFEN die Eingabeverarbeitung anderer Geräte nicht blockieren.

## 5. Geräteentfernung

Beim Entfernen eines Geräts MUSS NovaOS:

- Die Annahme neuer Geräteereignisse unterbinden.
- Bereits erfasste Ereignisse kontrolliert behandeln.
- Gedrückte Tasten und Maustasten bereinigen.
- Aktive Touch- und Stiftkontakte abbrechen.
- Laufende Tastenwiederholungen beenden.
- Gerätebezogenes Pointer Capture aufheben.
- Gerätezuordnungen aktualisieren.
- Ausstehende Geräteoperationen abschließen oder abbrechen.
- Ressourcen nach Abschluss aller Zugriffe freigeben.

Die Bereinigung MUSS auch bei unerwarteter Geräteentfernung funktionieren.

Andere Eingabegeräte bleiben weiterhin verfügbar.

## 6. Wiederverbindung und Konfiguration

NovaOS SOLL wieder angeschlossene Geräte anhand geeigneter Geräteinformationen erkennen.

Dazu können verwendet werden:

- Hersteller- und Produktkennung.
- Seriennummer, sofern vorhanden.
- Gerätespezifische Identifikatoren.
- Verbindungsinformationen.
- Gespeicherte Benutzerzuordnungen.

Bei erfolgreicher Wiedererkennung SOLLEN vorhandene Einstellungen automatisch wiederhergestellt werden.

Dazu gehören:

- Tastaturlayouts.
- Tastenbelegungen.
- Mausgeschwindigkeit.
- Kalibrierungsdaten.
- Display-Zuordnungen.
- Input-Seat-Zuordnungen.

Nicht eindeutig identifizierbare Geräte DÜRFEN nicht automatisch sicherheitskritische Berechtigungen eines früheren Geräts übernehmen.

## 7. Ereignisverarbeitung

Nova Input Hotplug stellt mindestens folgende Ereignisse bereit:

- `InputDeviceDetected`
- `InputDeviceReady`
- `InputDeviceSuspended`
- `InputDeviceResumed`
- `InputDeviceDisconnected`
- `InputDeviceError`

Jedes Ereignis enthält:

- Gerätekennung.
- Ereignistyp.
- Monotonen Zeitstempel.
- Gerätestatus.
- Optionale Fehlerinformationen.

Geräteereignisse werden über definierte Schnittstellen veröffentlicht.

Die Reihenfolge der Zustandsübergänge MUSS konsistent bleiben.

Veraltete Ereignisse einer früheren Geräteinstanz DÜRFEN nicht auf eine neue Instanz angewendet werden.

## 8. Suspend und Resume

Nova Input Hotplug unterstützt das vorübergehende Deaktivieren und Wiederaktivieren von Eingabegeräten.

Bei Suspend werden:

- Geräteoperationen kontrolliert angehalten.
- Aktive Eingabezustände bei Bedarf bereinigt.
- Geräteinformationen erhalten, soweit zulässig.

Bei Resume werden:

- Geräteverfügbarkeit und Identität überprüft.
- Erforderliche Treiberfunktionen reinitialisiert.
- Gerätezustände synchronisiert.
- Eingaben erneut freigegeben.

Ein Resume DARF keine veralteten Tastendrücke oder Kontakte wiederherstellen.

## 9. Sicherheit und Isolation

Die Geräteverwaltung unterliegt Nova.Trust.

Dabei gilt:

- Neue Geräte erhalten keine automatischen Sonderberechtigungen.
- Geräteinformationen MÜSSEN validiert werden.
- Gerätezuordnungen MÜSSEN geltende Sitzungsrichtlinien berücksichtigen.
- Wiederverbindungen DÜRFEN keine Sicherheitsgrenzen umgehen.
- Virtuelle Geräte unterliegen denselben Lebenszyklusregeln.
- Anwendungen DÜRFEN Geräte nicht ohne Berechtigung registrieren oder entfernen.

Die Erkennung eines physischen Geräts begründet allein keine Vertrauenswürdigkeit.

## 10. Fehlerbehandlung

- Fehlgeschlagene Initialisierungen werden kontrolliert beendet.
- Ungültige Gerätedeskriptoren werden zurückgewiesen.
- Wiederholte Verbindungswechsel DÜRFEN keine Ressourcenlecks verursachen.
- Geräteentfernungen während laufender Eingaben MÜSSEN sicher verarbeitet werden.
- Veraltete Geräteereignisse werden verworfen.
- Nicht erreichbare Treiber DÜRFEN die gesamte Hotplug-Verarbeitung nicht blockieren.
- Fehlerhafte Geräte DÜRFEN keine fremden Eingabezustände verändern.

Die Wiederherstellung eines konsistenten Eingabezustands hat Vorrang vor der Fortsetzung fehlerhafter Geräteoperationen.

## 11. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS das dynamische Hinzufügen und Entfernen von Eingabegeräten unterstützen, sofern die Hardwareanbindung Hotplug ermöglicht.
- **INPUT-002:** Eingabegeräte MÜSSEN einen definierten Lebenszyklus besitzen.
- **INPUT-003:** Geräte DÜRFEN erst nach erfolgreicher Initialisierung reguläre Eingaben bereitstellen.
- **INPUT-004:** Die Initialisierung einzelner Geräte MUSS unabhängig voneinander erfolgen können.
- **INPUT-005:** Geräteentfernungen MÜSSEN aktive Eingabezustände kontrolliert bereinigen.
- **INPUT-006:** Ausstehende Geräteoperationen MÜSSEN bei Entfernung sicher abgeschlossen oder abgebrochen werden.
- **INPUT-007:** Ressourcen DÜRFEN erst nach Beendigung aller relevanten Zugriffe freigegeben werden.
- **INPUT-008:** Geräteänderungen MÜSSEN über definierte Ereignisse bereitgestellt werden.
- **INPUT-009:** Veraltete Ereignisse DÜRFEN nicht auf neue Geräteinstanzen übertragen werden.
- **INPUT-010:** Wiederverbindungen SOLLEN gespeicherte Gerätekonfigurationen wiederherstellen.
- **INPUT-011:** Geräteidentitäten MÜSSEN vor der Wiederverwendung sicherheitsrelevanter Zuordnungen überprüft werden.
- **INPUT-012:** Suspend und Resume MÜSSEN konsistente Eingabezustände gewährleisten.
- **INPUT-013:** Fehler einzelner Geräte DÜRFEN andere Eingabegeräte nicht beeinträchtigen.
- **INPUT-014:** Hotplug-Operationen MÜSSEN den Sicherheitsrichtlinien von Nova.Trust entsprechen.
- **INPUT-015:** Die Hotplug-Verarbeitung MUSS unabhängig von Nova UI funktionieren.
- **INPUT-016:** Nova Input Hotplug MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 12. Integration

- **Nova Input Architecture:** Zentrale Eingabeverwaltung.
- **Nova Input Device Abstraction:** Einheitliches Gerätemodell.
- **Nova Input Event Pipeline:** Ereignisverarbeitung und Zustandsbereinigung.
- **Nova Input Security:** Geräteberechtigungen und Isolation.
- **Nova Multi-Device Input:** Gerätegruppen und Input-Seat-Zuordnungen.
- **Nova USB/xHCI:** Erkennung und Entfernung von USB-Geräten.
- **Nova HID:** Gerätebeschreibung und Eingabefunktionen.
- **Nova Device Manager:** Systemweite Geräteverwaltung.
- **Nova Power Management:** Suspend und Resume.
- **Nova.Trust:** Sicherheitsprüfung und Berechtigungen.

## 13. Ergebnis

NovaOS erhält eine zentrale Hotplug-Architektur für Eingabegeräte.

Geräte können während des Betriebs erkannt, initialisiert, entfernt und wiederverbunden werden, ohne die übrige Eingabeverarbeitung zu unterbrechen.

Durch kontrollierte Zustandsübergänge, sichere Ressourcenfreigabe und automatische Konfigurationswiederherstellung bleibt die Bedienung zuverlässig und konsistent.

**Grundsatz:** Eingabegeräte können jederzeit kommen und gehen. NovaOS passt sich automatisch an, ohne den Benutzer unnötig zu unterbrechen.