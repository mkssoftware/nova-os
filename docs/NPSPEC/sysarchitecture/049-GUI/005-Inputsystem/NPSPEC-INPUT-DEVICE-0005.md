# NPSPEC-INPUT-DEVICE-0005 – Nova Multi-Device Input

**Status:** Angenommen  
**Bereich:** Input / Device / Multi-Device  
**System:** NovaOS

## 1. Zweck

Nova Multi-Device Input definiert die gleichzeitige Verwendung und Koordination mehrerer Eingabegeräte innerhalb von NovaOS.

Ziel ist es, unterschiedliche Eingabegeräte unabhängig voneinander oder gemeinsam nutzen zu können, ohne Eingabekonflikte, Zustandsverluste oder unnötige Einschränkungen zu verursachen.

Das System unterstützt mehrere Geräte derselben Klasse sowie die kombinierte Nutzung unterschiedlicher Eingabeverfahren.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Multi-Device Coordinator | Koordination mehrerer Eingabegeräte |
| Device Group Manager | Verwaltung logischer Gerätegruppen |
| Input Seat Manager | Zuordnung von Geräten zu Benutzersitzungen und Eingabeplätzen |
| Multi-Device State Manager | Verwaltung geräteübergreifender Eingabezustände |
| Input Arbitration Controller | Auflösung konkurrierender Eingabeansprüche |
| Device Routing Manager | Weiterleitung entsprechend der Gerätezuordnung |

Die Komponenten verwenden die Nova Input Device Abstraction und Nova Input Event Pipeline.

Die einzelnen Gerätetreiber bleiben voneinander unabhängig.

## 3. Gerätemodell

NovaOS unterscheidet:

- **Physical Device:** Tatsächlich angeschlossenes Eingabegerät.
- **Logical Device:** Abstrahierte Eingabefunktion eines Geräts.
- **Device Group:** Zusammenfassung mehrerer logischer Geräte.
- **Input Seat:** Logischer Eingabeplatz mit zugeordneten Geräten.
- **Input Session:** Benutzer- oder Systemkontext, dem Eingaben zugestellt werden.

Ein physisches Gerät DARF mehrere logische Geräte bereitstellen.

Mehrere physische Geräte DÜRFEN zu einer gemeinsamen logischen Eingabegruppe zusammengefasst werden.

Geräteidentität und Ereignisherkunft MÜSSEN dabei erhalten bleiben.

## 4. Gleichzeitige Gerätenutzung

NovaOS unterstützt insbesondere:

- Mehrere Tastaturen.
- Mehrere Mäuse.
- Mehrere Touchscreens.
- Mehrere digitale Stifte.
- Kombinationen aus Maus, Tastatur, Touch und Stift.
- Physische und virtuelle Eingabegeräte.

Eingaben verschiedener Geräte DÜRFEN parallel erfasst werden.

Die Verarbeitung MUSS die Ereignisreihenfolge innerhalb jedes einzelnen Geräts erhalten.

Eine globale Reihenfolge sämtlicher Geräteereignisse ist nicht erforderlich.

## 5. Input Seats

Ein Input Seat beschreibt einen eigenständigen logischen Eingabeplatz.

Er kann enthalten:

- Eine oder mehrere Tastaturen.
- Ein oder mehrere Zeigegeräte.
- Touch- und Stiftgeräte.
- Einen zugeordneten Eingabefokus.
- Eine Benutzer- oder Systemsitzung.

NovaOS unterstützt standardmäßig einen gemeinsamen Eingabeplatz.

Die Architektur MUSS mehrere unabhängige Input Seats ermöglichen.

Ein Gerät wird zu einem Zeitpunkt höchstens einem exklusiven Input Seat zugeordnet.

Gerätewechsel zwischen Seats MÜSSEN kontrolliert erfolgen.

## 6. Gemeinsame Eingabezustände

Der Multi-Device State Manager verwaltet gerätebezogene und aggregierte Zustände.

Dabei gilt:

- Physische Tastenzustände bleiben gerätebezogen.
- Maustastenzustände bleiben gerätebezogen.
- Touchkontakte bleiben eindeutig einem Gerät zugeordnet.
- Stiftzustände bleiben unabhängig.
- Aggregierte Zustände werden aus den zugehörigen Geräteinformationen abgeleitet.

Wird beispielsweise dieselbe logische Taste auf zwei Tastaturen gedrückt, DARF das Loslassen einer Tastatur den weiterhin aktiven Zustand der anderen Tastatur nicht aufheben.

Die Aggregation MUSS solche Mehrfachzustände korrekt berücksichtigen.

## 7. Gemeinsame Zeigersteuerung

Mehrere Zeigegeräte können einen gemeinsamen Systemzeiger steuern.

Dabei gilt:

- Relative Bewegungen werden entsprechend ihrer Ereignisfolge verarbeitet.
- Absolute Eingaben werden anhand ihrer Display-Zuordnung transformiert.
- Jedes Gerät behält seinen eigenen Tastenstatus.
- Pointer Capture wird dem jeweiligen Eingabekontext zugeordnet.
- Gerätewechsel dürfen keine unbeabsichtigten Tastenzustände erzeugen.

Ein unabhängiger Mehrzeigerbetrieb DARF optional unterstützt werden.

Die Zuordnung zwischen Zeigegeräten und Systemzeigern MUSS eindeutig sein.

## 8. Gerätekonflikte und Prioritäten

Der Input Arbitration Controller behandelt konkurrierende Eingaben.

Beispiele:

- Gleichzeitige Maus- und Touchbedienung.
- Stifteingabe während aktiver Touchkontakte.
- Mehrere Geräte mit Pointer Capture.
- Wechsel zwischen virtueller und physischer Eingabe.
- Konkurrierende Fokusanforderungen.

Die Konfliktbehandlung erfolgt anhand definierter Regeln.

Sicherheitskritische Systemeingaben besitzen Vorrang vor regulären Anwendungseingaben.

Geräte DÜRFEN andere Eingabequellen nicht ohne entsprechende Berechtigung blockieren.

## 9. Hotplug und Gerätewechsel

Geräte können während des Betriebs hinzugefügt, entfernt oder neu zugeordnet werden.

Dabei gilt:

- Neue Geräte werden über den Input Device Manager registriert.
- Die Zuordnung zu Device Groups und Input Seats erfolgt kontrolliert.
- Entfernte Geräte werden aus ihren Gruppen entfernt.
- Aktive Eingabezustände werden bei Entfernung bereinigt.
- Andere Geräte bleiben weiterhin funktionsfähig.

Bei Gerätewechseln MÜSSEN bestehende Capture- und Fokuszuordnungen überprüft werden.

Die Entfernung eines Geräts DARF nicht zum Verlust der Eingabefähigkeit anderer Geräte führen.

## 10. Konfiguration

NovaOS unterstützt konfigurierbare Gerätezuordnungen.

Dazu gehören:

- Zuordnung zu Input Seats.
- Zuordnung zu Benutzersitzungen.
- Gemeinsame oder getrennte Zeigersteuerung.
- Gerätespezifische Eingabeprofile.
- Display-Zuordnung absoluter Eingabegeräte.
- Prioritäten bei konkurrierenden Eingaben.

Konfigurationen SOLLEN dauerhaft gespeichert und bei erneuter Geräteerkennung wiederhergestellt werden können.

Nicht eindeutig identifizierbare Geräte MÜSSEN sicher behandelt werden.

## 11. Sicherheit und Isolation

Nova Multi-Device Input unterliegt Nova Input Security und Nova.Trust.

Festlegungen:

- Gerätezuordnungen MÜSSEN die Sitzungsisolation berücksichtigen.
- Anwendungen DÜRFEN keine fremden Geräte ohne Berechtigung übernehmen.
- Virtuelle Geräte MÜSSEN identifizierbar bleiben.
- Eingabeinjektionen unterliegen den jeweiligen Capabilities.
- Gerätewechsel DÜRFEN keine Sicherheitsgrenzen umgehen.
- Geschützte Eingabekontexte besitzen Vorrang vor regulären Eingabeansprüchen.

Die gemeinsame Nutzung mehrerer Geräte DARF die Sicherheit einzelner Eingabequellen nicht reduzieren.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS mehrere Eingabegeräte gleichzeitig unterstützen.
- **INPUT-002:** Geräte derselben Klasse MÜSSEN unabhängig voneinander verwaltet werden können.
- **INPUT-003:** Geräteidentität und Ereignisherkunft MÜSSEN erhalten bleiben.
- **INPUT-004:** Physische und logische Geräte MÜSSEN unterscheidbar sein.
- **INPUT-005:** Mehrere Geräte MÜSSEN zu logischen Gerätegruppen zusammengefasst werden können.
- **INPUT-006:** Die Architektur MUSS mehrere unabhängige Input Seats ermöglichen.
- **INPUT-007:** Geräte MÜSSEN kontrolliert Benutzersitzungen zugeordnet werden können.
- **INPUT-008:** Gerätezustände MÜSSEN unabhängig voneinander verwaltet werden.
- **INPUT-009:** Aggregierte Eingabezustände DÜRFEN keine aktiven Einzelzustände unbeabsichtigt aufheben.
- **INPUT-010:** Mehrere Zeigegeräte MÜSSEN einen gemeinsamen Systemzeiger steuern können.
- **INPUT-011:** Konkurrierende Eingaben MÜSSEN anhand definierter Regeln behandelt werden.
- **INPUT-012:** Geräte MÜSSEN während des Betriebs hinzugefügt und entfernt werden können, sofern die Hardwareanbindung dies unterstützt.
- **INPUT-013:** Bei Geräteverlust MÜSSEN zugehörige Eingabezustände bereinigt werden.
- **INPUT-014:** Gerätewechsel MÜSSEN Fokus- und Capture-Zuordnungen berücksichtigen.
- **INPUT-015:** Geräte- und Sitzungszuordnungen MÜSSEN Nova.Trust entsprechen.
- **INPUT-016:** Die Multi-Device-Verarbeitung MUSS unabhängig von Nova UI funktionieren.
- **INPUT-017:** Die Verarbeitung MUSS bei identischer Ereignisfolge und identischem Ausgangszustand reproduzierbare Ergebnisse liefern.
- **INPUT-018:** Nova Multi-Device Input MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Input Device Abstraction:** Einheitliches Gerätemodell.
- **Nova Input Event Pipeline:** Ereignisverarbeitung und Zustellung.
- **Nova Input Security:** Berechtigungen und Sitzungsisolation.
- **Nova Keyboard Input:** Mehrere Tastaturen.
- **Nova Mouse Input:** Mehrere Mäuse und gemeinsame Zeigersteuerung.
- **Nova Touch Input:** Mehrere Touchgeräte.
- **Nova Pen and Stylus Input:** Mehrere Stiftgeräte.
- **Nova Window Manager:** Fokus und Eingabekontexte.
- **Nova Display System:** Zuordnung absoluter Eingabegeräte.
- **Nova.Trust:** Geräte- und Sitzungsberechtigungen.

## 14. Ergebnis

NovaOS erhält eine zentrale Architektur zur gleichzeitigen Nutzung und Koordination beliebig kombinierbarer Eingabegeräte.

Geräte können unabhängig arbeiten, gemeinsame Eingabefunktionen bereitstellen oder unterschiedlichen Eingabeplätzen zugeordnet werden.

Die gerätebezogene Zustandsverwaltung verhindert Konflikte und ermöglicht eine sichere, flexible Erweiterung um zukünftige Eingabekonzepte.

**Grundsatz:** NovaOS passt die Zusammenarbeit der Eingabegeräte an den Benutzer an – nicht der Benutzer seine Arbeitsweise an die vorhandenen Geräte.