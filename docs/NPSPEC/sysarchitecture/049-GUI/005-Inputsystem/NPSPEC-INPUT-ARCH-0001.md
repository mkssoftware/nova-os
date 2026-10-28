# NPSPEC-INPUT-ARCH-0001 – Nova Input Architecture

**Status:** Angenommen  
**Bereich:** Input / Architecture  
**System:** NovaOS

## 1. Zweck

Nova Input Architecture definiert die zentrale Architektur zur Erfassung, Verarbeitung und Weiterleitung von Benutzereingaben innerhalb von NovaOS.

Ziel ist ein einheitliches, sicheres und latenzarmes Eingabesystem, das unabhängig von Gerätetyp, Hardwaretreiber und Benutzeroberfläche funktioniert.

Das System unterstützt klassische und moderne Eingabegeräte sowie die Erweiterung um zukünftige Eingabeverfahren.

## 2. Architektur

Das Eingabesystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Input Driver Interface | Einheitliche Schnittstelle für Eingabetreiber |
| Input Device Manager | Erkennung und Verwaltung von Eingabegeräten |
| Input Event Processor | Normalisierung und Verarbeitung von Eingabeereignissen |
| Input State Manager | Verwaltung aktueller Eingabezustände |
| Input Mapping Engine | Zuordnung physischer Eingaben zu logischen Aktionen |
| Input Dispatcher | Weiterleitung von Eingabeereignissen |
| Input Focus Manager | Verwaltung des Eingabefokus |
| Input Security Manager | Zugriffskontrolle und Isolation |

Die Komponenten sind logisch voneinander getrennt.

Hardwareabhängige Funktionen verbleiben in den jeweiligen Gerätetreibern.

Anwendungen verwenden ausschließlich die definierten Input-Schnittstellen.

## 3. Unterstützte Eingabegeräte

NovaOS berücksichtigt folgende Geräteklassen:

- Tastaturen.
- Mäuse.
- Touchpads.
- Touchscreens.
- Stifte und Grafiktabletts.
- Gamepads und Joysticks.
- Weitere HID-kompatible Eingabegeräte.
- Virtuelle Eingabegeräte.

Die Architektur MUSS erweiterbar sein, ohne bestehende Anwendungsschnittstellen zu verändern.

Nicht unterstützte Gerätefunktionen werden über Device Capabilities erkennbar gemacht.

## 4. Eingabemodell

Das Eingabesystem unterscheidet drei Ebenen:

### Physical Input

Hardwarebezogene Informationen:

- Scancodes.
- Tasten- und Schalterzustände.
- Relative und absolute Bewegungen.
- Achsenwerte.
- Gerätekennungen.
- Hardware-Zeitstempel, sofern verfügbar.

### Logical Input

Geräteunabhängige Interpretation:

- Logische Tasten.
- Zeigerbewegungen.
- Maustasten.
- Touchkontakte.
- Gesten.
- Gamepad-Aktionen.

### Semantic Input

Anwendungsbezogene Bedeutung:

- Text eingeben.
- Element auswählen.
- Scrollen.
- Navigation.
- Aktion auslösen.
- Kontextmenü öffnen.

Die Ebenen MÜSSEN getrennt bleiben.

Eine physische Eingabe kann abhängig von Kontext und Konfiguration unterschiedliche logische oder semantische Aktionen auslösen.

## 5. Input Events

Eingabeereignisse besitzen ein einheitliches Datenmodell.

Ein Ereignis enthält mindestens:

- Ereignistyp.
- Gerätekennung.
- Zeitstempel.
- Ereignisdaten.
- Ereignisquelle.
- Relevante Modifier-Zustände.
- Sequenzkennung zur Reihenfolgesicherung.

Unterstützt werden ereignisbasierte Verarbeitung und zustandsbasierte Abfragen.

Ereignisse werden über definierte Warteschlangen verarbeitet.

Die Reihenfolge zusammengehöriger Ereignisse MUSS erhalten bleiben.

## 6. Input Device Management

Der Input Device Manager übernimmt:

- Geräteerkennung.
- Geräteinitialisierung.
- Hotplug und Hot-Unplug.
- Geräteklassifizierung.
- Abfrage unterstützter Funktionen.
- Verwaltung von Gerätezuständen.
- Fehlererkennung.
- Kontrollierte Gerätefreigabe.

Geräte MÜSSEN während des laufenden Betriebs hinzugefügt und entfernt werden können, sofern der jeweilige Bus dies unterstützt.

Der Ausfall eines Eingabegeräts DARF andere Geräte nicht beeinträchtigen.

## 7. Input Mapping

Die Input Mapping Engine ermöglicht die Zuordnung von Eingaben zu logischen Funktionen.

Unterstützt werden:

- Konfigurierbare Tastenbelegungen.
- Tastaturlayouts.
- Modifier-Kombinationen.
- Maus- und Touchpad-Einstellungen.
- Gamepad-Mapping.
- Benutzerdefinierte Eingabeprofile.
- Kontextabhängige Zuordnungen.

Systemreservierte Tastenkombinationen besitzen Vorrang vor anwendungsspezifischen Zuordnungen.

Konfigurationen MÜSSEN reproduzierbar gespeichert und wiederhergestellt werden können.

## 8. Fokus und Ereignisweiterleitung

Der Input Dispatcher leitet Eingaben an berechtigte Empfänger weiter.

Dabei werden berücksichtigt:

- Aktiver Benutzer.
- Aktive Sitzung.
- Eingabefokus.
- Fensterhierarchie.
- Modalität.
- Pointer Capture.
- Gerätezuordnung.
- Sicherheitsrichtlinien.

Der Window Manager verwaltet die Fensterstruktur.

Der Input Focus Manager bestimmt gemeinsam mit dem Window Manager den zulässigen Eingabeempfänger.

Anwendungen DÜRFEN Eingaben anderer Anwendungen nicht ohne entsprechende Berechtigung empfangen.

## 9. Text- und Zeigereingaben

Textverarbeitung und physische Tasteneingaben werden getrennt behandelt.

Eine Tasteneingabe entspricht nicht zwangsläufig einem Unicode-Zeichen.

Die Texteingabe unterstützt:

- Tastaturlayouts.
- Dead Keys.
- Compose-Sequenzen.
- Unicode-Zeichen.
- Eingabemethoden für internationale Schriftsysteme.

Zeigereingaben unterstützen relative und absolute Koordinaten.

Die Transformation von Zeigerkoordinaten berücksichtigt Display-Skalierung, Rotation und Fenstergeometrie.

## 10. Latenz und Performance

Die Eingabeverarbeitung ist auf geringe Latenz ausgelegt.

Festlegungen:

- Hardwareereignisse werden zeitnah erfasst.
- Aufwendige Verarbeitung erfolgt außerhalb zeitkritischer Interrupt-Kontexte.
- Ereigniswarteschlangen besitzen definierte Kapazitätsgrenzen.
- Bewegungsereignisse DÜRFEN bei geeigneten Bedingungen zusammengefasst werden.
- Zustandsänderungen wie Tastendruck und Tastenfreigabe DÜRFEN nicht unkontrolliert verworfen werden.
- Eingaben werden möglichst im nächsten geeigneten UI-Frame berücksichtigt.

Die Synchronisation mit der Darstellung erfolgt über Nova Frame Pacing.

## 11. Sicherheit und Isolation

Die Eingabearchitektur unterliegt Nova.Trust.

Sicherheitsanforderungen:

- Eingabegeräte und Anwendungen besitzen getrennte Berechtigungen.
- Globale Eingabeüberwachung erfordert eine ausdrückliche Berechtigung.
- Virtuelle Eingaben müssen als solche identifizierbar sein.
- Eingabeinjektion unterliegt kontrollierten Capabilities.
- Sicherheitsrelevante Eingabesequenzen dürfen nicht durch unberechtigte Anwendungen abgefangen werden.
- Gerätefehler dürfen keine unkontrollierten Speicherzugriffe verursachen.

Sicherheitskritische Eingaben werden ausschließlich an vertrauenswürdige Systemkomponenten weitergeleitet.

## 12. Integration

- **Nova USB/xHCI:** Anbindung von USB-Eingabegeräten.
- **Nova HID:** Interpretation gerätespezifischer Eingabedaten.
- **Nova Window Manager:** Fensterfokus und Eingabeziele.
- **Nova Display System:** Koordinatentransformationen.
- **Nova UI:** Verarbeitung von Eingaben durch Controls.
- **Nova Time:** Monotone Ereigniszeitstempel.
- **Nova.Trust:** Eingabeberechtigungen und Isolation.
- **Nova Agent Runtime:** Kontrollierte Auslösung semantischer Aktionen.

Das Input-System MUSS unabhängig von Nova UI funktionieren.

Damit bleiben Eingaben auch in Systemkonsole, Recovery und anderen Betriebsumgebungen möglich.

## 13. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale, geräteunabhängige Eingabearchitektur bereitstellen.
- **INPUT-002:** Hardwaretreiber MÜSSEN über eine definierte Input Driver Interface angebunden werden.
- **INPUT-003:** Physische, logische und semantische Eingaben MÜSSEN unterscheidbar sein.
- **INPUT-004:** Eingabeereignisse MÜSSEN ein einheitliches Datenmodell verwenden.
- **INPUT-005:** Ereigniszeitstempel MÜSSEN einer monotonen Zeitbasis zugeordnet werden können.
- **INPUT-006:** Die Reihenfolge zusammengehöriger Eingabeereignisse MUSS erhalten bleiben.
- **INPUT-007:** Tastaturen und Zeigegeräte MÜSSEN unterstützt werden.
- **INPUT-008:** Weitere Eingabegeräte MÜSSEN über erweiterbare Device Capabilities integrierbar sein.
- **INPUT-009:** Hotplug und Hot-Unplug MÜSSEN unterstützt werden, sofern die Hardwareanbindung dies ermöglicht.
- **INPUT-010:** Eingabebelegungen MÜSSEN konfigurierbar sein.
- **INPUT-011:** Text- und Tasteneingaben MÜSSEN getrennt verarbeitet werden.
- **INPUT-012:** Eingaben MÜSSEN anhand von Sitzung, Fokus und Berechtigungen weitergeleitet werden.
- **INPUT-013:** Anwendungen DÜRFEN keine unberechtigten globalen Eingaben überwachen.
- **INPUT-014:** Virtuelle Eingaben und Eingabeinjektionen MÜSSEN kontrollierbar sein.
- **INPUT-015:** Eingabeverarbeitung MUSS unabhängig von der grafischen Oberfläche funktionieren.
- **INPUT-016:** Die Architektur MUSS ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 14. Fehlerbehandlung

- Fehlerhafte Eingabedaten werden validiert und kontrolliert verworfen.
- Nicht unterstützte Gerätefunktionen werden eindeutig gemeldet.
- Entfernte Geräte lösen die Bereinigung ihrer Eingabezustände aus.
- Verlorene oder ungültige Eingabezustände MÜSSEN eine definierte Zustandswiederherstellung ermöglichen.
- Überfüllte Ereigniswarteschlangen MÜSSEN erkannt werden.
- Fehler einzelner Treiber DÜRFEN das gesamte Eingabesystem nicht blockieren.
- Bei Ausfall der grafischen Oberfläche MUSS die Eingabeverarbeitung für verfügbare Recovery- und Konsolenumgebungen erhalten bleiben.

## 15. Ergebnis

NovaOS erhält eine einheitliche, sichere und erweiterbare Eingabearchitektur.

Unterschiedliche Eingabegeräte werden über gemeinsame Schnittstellen verarbeitet, während gerätespezifische Funktionen erhalten bleiben.

Die Trennung von Hardwareerfassung, Ereignisverarbeitung, Mapping, Fokusverwaltung und semantischen Aktionen ermöglicht eine flexible Integration in Nova UI, Systemprogramme und zukünftige Eingabekonzepte.

**Grundsatz:** NovaOS verarbeitet Eingaben unabhängig davon, mit welchem Gerät sie erfolgen. Entscheidend ist die beabsichtigte Aktion des Benutzers, nicht die verwendete Hardware.