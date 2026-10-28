# NPSPEC-INPUT-ARCH-0002 – Nova Input Device Abstraction

**Status:** Angenommen  
**Bereich:** Input / Architecture / Device Abstraction  
**System:** NovaOS

## 1. Zweck

Nova Input Device Abstraction definiert die hardwareunabhängige Darstellung und Ansteuerung von Eingabegeräten innerhalb von NovaOS.

Ziel ist eine einheitliche Schnittstelle für Tastaturen, Mäuse, Touchscreens, Gamepads und zukünftige Eingabegeräte.

Anwendungen und übergeordnete Input-Komponenten sollen Eingabegeräte verwenden können, ohne deren Busanbindung, Treiber oder Hardwareprotokoll kennen zu müssen.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Input Device Abstraction Layer | Einheitliche Geräteabstraktion |
| Device Registry | Registrierung verfügbarer Eingabegeräte |
| Device Descriptor | Beschreibung der Geräteeigenschaften |
| Device Capability Provider | Bereitstellung unterstützter Funktionen |
| Device State Provider | Verwaltung aktueller Gerätezustände |
| Device Event Adapter | Normalisierung gerätespezifischer Ereignisse |

Die Abstraktionsschicht befindet sich zwischen den Eingabetreibern und dem Nova Input Event Processor.

Hardwareabhängige Funktionen verbleiben im jeweiligen Treiber.

## 3. Gerätemodell

Jedes Eingabegerät wird durch ein `InputDevice` repräsentiert.

Ein InputDevice besitzt mindestens:

- Eindeutige Gerätekennung.
- Geräteklasse.
- Gerätename.
- Treiberzuordnung.
- Verbindungsstatus.
- Unterstützte Capabilities.
- Aktuellen Gerätezustand.
- Optionale Hersteller- und Produktinformationen.

Gerätekennungen MÜSSEN während der Registrierung eindeutig sein.

Persistente Geräteidentitäten können bereitgestellt werden, sofern die Hardware eine zuverlässige Wiedererkennung ermöglicht.

## 4. Geräteklassen

NovaOS definiert grundlegende Geräteklassen:

| Geräteklasse | Beschreibung |
|---|---|
| Keyboard | Tastaturen und tastaturähnliche Geräte |
| Pointer | Mäuse und andere Zeigegeräte |
| Touch | Touchscreens und Multitouch-Geräte |
| Pen | Digitale Stifte und Grafiktabletts |
| GameController | Gamepads und Joysticks |
| GenericInput | Geräte mit frei beschriebenen Eingabefunktionen |
| VirtualInput | Softwarebasierte Eingabegeräte |

Ein physisches Gerät DARF mehrere Geräteklassen beziehungsweise logische Eingabefunktionen bereitstellen.

Die Geräteklassifizierung MUSS erweiterbar sein.

## 5. Device Capabilities

Gerätefunktionen werden über standardisierte Capabilities beschrieben.

Dazu gehören:

- Digitale Tasten und Schalter.
- Relative Bewegungsachsen.
- Absolute Positionsachsen.
- Analoge Achsen.
- Touchkontakte.
- Druck- und Neigungswerte.
- Scrollachsen.
- Geräteabhängige Zusatzfunktionen.

Jede Capability beschreibt ihre Eigenschaften, Wertebereiche und gegebenenfalls Einheiten.

Anwendungen MÜSSEN unterstützte Funktionen abfragen können.

Nicht unterstützte Funktionen DÜRFEN nicht als verfügbar gemeldet werden.

## 6. Eingabeereignisse

Gerätespezifische Eingaben werden in ein einheitliches Ereignismodell überführt.

Grundlegende Ereignistypen:

- `ButtonPressed`
- `ButtonReleased`
- `AxisChanged`
- `PointerMoved`
- `TouchStarted`
- `TouchMoved`
- `TouchEnded`
- `DeviceConnected`
- `DeviceDisconnected`

Ereignisse enthalten eine Gerätekennung, einen Zeitstempel und typabhängige Daten.

Die Abstraktionsschicht normalisiert die Ereignisstruktur, ohne relevante Geräteinformationen zu verlieren.

Die Interpretation als Text, Geste oder semantische Aktion erfolgt in übergeordneten Input-Komponenten.

## 7. Gerätezustände

Der Device State Provider verwaltet den aktuellen Zustand eines Eingabegeräts.

Dazu gehören:

- Gedrückte Tasten.
- Aktuelle Achsenwerte.
- Zeigerpositionen.
- Aktive Touchkontakte.
- Verbindungsstatus.

Der Zustand MUSS konsistent mit der verarbeiteten Ereignisfolge sein.

Bei Geräteverlust MÜSSEN aktive Eingabezustände kontrolliert beendet beziehungsweise zurückgesetzt werden.

Ereignisbasierte und zustandsbasierte Eingabeabfragen verwenden dieselbe logische Gerätebasis.

## 8. Hotplug und Lebenszyklus

Eingabegeräte durchlaufen definierte Zustände:

1. **Detected:** Gerät wurde erkannt.
2. **Initializing:** Treiber und Gerätebeschreibung werden vorbereitet.
3. **Ready:** Gerät kann Eingaben bereitstellen.
4. **Suspended:** Gerät ist vorübergehend inaktiv.
5. **Disconnected:** Gerät wurde entfernt.
6. **Released:** Ressourcen wurden freigegeben.

Hotplug-fähige Geräte können während des laufenden Betriebs hinzugefügt und entfernt werden.

Geräteereignisse DÜRFEN erst nach erfolgreicher Initialisierung veröffentlicht werden.

Nach einer Entfernung MÜSSEN ausstehende Zugriffe kontrolliert beendet werden.

## 9. Virtuelle Eingabegeräte

NovaOS unterstützt virtuelle Eingabegeräte über dieselbe Abstraktionsschicht.

Virtuelle Geräte können beispielsweise verwendet werden für:

- Barrierefreiheit.
- Automatisierte Tests.
- Eingabesimulation.
- Softwarebasierte Controller.
- Kontrollierte Fernbedienung.

Virtuelle Geräte MÜSSEN eindeutig als virtuelle Eingabequellen identifizierbar sein.

Ihre Erstellung und Verwendung unterliegen Nova.Trust.

Virtuelle Geräte DÜRFEN keine Berechtigungsprüfungen umgehen.

## 10. Sicherheit und Isolation

Die Geräteabstraktion gewährleistet:

- Trennung zwischen Hardwaretreibern und Anwendungen.
- Validierung gerätespezifischer Daten.
- Kontrollierten Zugriff auf Gerätefunktionen.
- Isolation fehlerhafter Geräte.
- Berechtigungsprüfung bei exklusivem Gerätezugriff.
- Schutz vor unberechtigter Eingabeinjektion.

Anwendungen erhalten grundsätzlich abstrahierte Eingaben und keinen direkten Hardwarezugriff.

Direkter Gerätezugriff erfordert eine ausdrücklich gewährte Capability.

## 11. Integration

- **Nova Input Architecture:** Übergeordnete Eingabeverarbeitung.
- **Nova Input Driver Interface:** Anbindung hardwareabhängiger Treiber.
- **Nova Input Device Manager:** Registrierung und Lebenszyklus.
- **Nova Input Event Processor:** Verarbeitung normalisierter Ereignisse.
- **Nova Input Mapping Engine:** Zuordnung logischer Eingaben.
- **Nova USB/HID:** Unterstützung entsprechender Eingabegeräte.
- **Nova.Trust:** Geräteberechtigungen und Isolation.

Die Geräteabstraktion MUSS unabhängig von Nova UI, Window Manager und Compositor funktionieren.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine hardwareunabhängige Eingabegeräteabstraktion bereitstellen.
- **INPUT-002:** Jedes registrierte Eingabegerät MUSS durch ein einheitliches InputDevice-Modell repräsentiert werden.
- **INPUT-003:** Geräte MÜSSEN eindeutige Kennungen besitzen.
- **INPUT-004:** Geräteklassen und Capabilities MÜSSEN erweiterbar sein.
- **INPUT-005:** Ein Gerät MUSS mehrere logische Eingabefunktionen bereitstellen können.
- **INPUT-006:** Unterstützte Gerätefunktionen MÜSSEN abfragbar sein.
- **INPUT-007:** Gerätespezifische Ereignisse MÜSSEN in ein einheitliches Ereignismodell überführt werden.
- **INPUT-008:** Relevante gerätespezifische Informationen DÜRFEN bei der Normalisierung nicht unkontrolliert verloren gehen.
- **INPUT-009:** Aktuelle Gerätezustände MÜSSEN abfragbar sein.
- **INPUT-010:** Geräteinitialisierung und Ressourcenfreigabe MÜSSEN kontrolliert erfolgen.
- **INPUT-011:** Hotplug MUSS bei entsprechend unterstützten Geräteanbindungen möglich sein.
- **INPUT-012:** Virtuelle Eingabegeräte MÜSSEN unterstützt und als solche identifizierbar sein.
- **INPUT-013:** Gerätezugriffe MÜSSEN den Berechtigungsregeln von Nova.Trust entsprechen.
- **INPUT-014:** Anwendungen DÜRFEN nicht ohne Berechtigung direkt auf Eingabehardware zugreifen.
- **INPUT-015:** Die Geräteabstraktion MUSS unabhängig von der grafischen Oberfläche funktionieren.
- **INPUT-016:** Die Verarbeitung MUSS deterministisch und ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

- Fehlerhafte Device Descriptors werden zurückgewiesen.
- Ungültige Capability-Werte werden erkannt und kontrolliert behandelt.
- Nicht unterstützte Gerätefunktionen werden eindeutig gemeldet.
- Fehlgeschlagene Geräteinitialisierungen DÜRFEN andere Eingabegeräte nicht beeinträchtigen.
- Entfernte Geräte MÜSSEN ihre aktiven Eingabezustände bereinigen.
- Ausstehende Ereignisse entfernter Geräte werden kontrolliert abgeschlossen oder verworfen.
- Treiberfehler DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.

## 14. Ergebnis

NovaOS erhält eine einheitliche Abstraktionsschicht für physische und virtuelle Eingabegeräte.

Hardwareunterschiede werden durch standardisierte Gerätebeschreibungen, Capabilities, Zustände und Ereignisse gekapselt.

Neue Eingabegeräte können integriert werden, ohne die übergeordneten Input-Komponenten oder Anwendungsschnittstellen grundlegend verändern zu müssen.

**Grundsatz:** NovaOS behandelt Eingabegeräte anhand ihrer Fähigkeiten und nicht anhand ihrer Hardwareanbindung.