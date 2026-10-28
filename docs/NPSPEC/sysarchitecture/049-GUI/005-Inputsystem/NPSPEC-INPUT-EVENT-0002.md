# NPSPEC-INPUT-EVENT-0002 – Nova Event Routing

**Status:** Angenommen  
**Bereich:** Input / Event / Routing  
**System:** NovaOS

## 1. Zweck

Nova Event Routing definiert die Ermittlung des zuständigen Empfängers für Eingabeereignisse innerhalb von NovaOS.

Ziel ist eine sichere, deterministische und kontextabhängige Weiterleitung von Tastatur-, Maus-, Touch-, Stift- und weiteren Eingabeereignissen.

Das Routing entscheidet, **welcher Empfänger ein Ereignis erhält**. Die eigentliche Zustellung übernimmt der Nova Event Dispatcher.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Event Router | Zentrale Ermittlung des Eingabeziels |
| Routing Context Manager | Verwaltung gültiger Routing-Kontexte |
| Input Target Resolver | Ermittlung des zuständigen Eingabeelements |
| Hit Test Provider | Räumliche Zielermittlung für Zeigereingaben |
| Routing Policy Controller | Prüfung von Routing-Regeln und Berechtigungen |

Der Event Router verwendet die Informationen des Input Focus Managers, Window Managers und der Input Device Abstraction.

Er bleibt unabhängig vom Rendering-Backend und der Nova-UI-Implementierung.

## 3. Routing-Modell

Jedes Eingabeereignis wird anhand folgender Informationen ausgewertet:

- Eingabegerät und Ereignistyp.
- Input Seat.
- Benutzersitzung.
- Aktiver Eingabefokus.
- Aktive Capture-Zuordnung.
- Cursor- beziehungsweise Kontaktposition.
- Fensterhierarchie und Eingabebereiche.
- Sicherheitskontext des Empfängers.

Das Ergebnis ist ein eindeutig bestimmter Eingabeempfänger oder eine definierte Nichtzustellung.

Ein Ereignis DARF nicht allein aufgrund seiner Bildschirmposition an einen Empfänger einer fremden Sitzung weitergeleitet werden.

## 4. Routing-Verfahren

NovaOS unterstützt folgende Routing-Verfahren:

| Verfahren | Beschreibung |
|---|---|
| Focus Routing | Zustellung an den aktiven Eingabefokus |
| Hit-Test Routing | Zielermittlung anhand der Eingabeposition |
| Capture Routing | Zustellung an einen berechtigten Capture-Empfänger |
| Contact Routing | Zustellung an den gebundenen Touch- oder Stiftkontext |
| System Routing | Zustellung sicherheitskritischer Systemeingaben |
| Explicit Routing | Gezielte Zustellung innerhalb eines autorisierten Kontexts |

Die Auswahl des Verfahrens erfolgt anhand des Ereignistyps und des aktuellen Eingabezustands.

## 5. Zielermittlung

Für die Zielermittlung gelten folgende Grundregeln:

- Tastaturereignisse werden grundsätzlich an den aktiven Tastaturfokus geroutet.
- Zeigerereignisse werden über Hit Testing oder aktives Pointer Capture zugeordnet.
- Touch- und Stiftkontakte werden bei Kontaktbeginn einem gültigen Eingabekontext zugeordnet.
- Laufende Kontakte behalten ihre Zuordnung bis zum regulären Ende oder Abbruch.
- Systemreservierte Eingaben werden vor dem regulären Anwendungsrouting behandelt.

Der Input Target Resolver MUSS ungültige, zerstörte oder nicht mehr berechtigte Ziele erkennen.

## 6. Hit Testing

Hit Testing ermittelt das zuständige Eingabeziel anhand einer Position.

Dabei werden berücksichtigt:

- Fensterposition und Fenstergröße.
- Z-Reihenfolge.
- Sichtbarkeit.
- Eingabeaktivierte Bereiche.
- Transformationen.
- Clipping.
- Modale Fenster.
- Eingabedurchlässige Oberflächen.

Der Window Manager stellt die erforderlichen Fensterinformationen bereit.

Nova UI kann die Zielermittlung innerhalb eines Fensters bis auf einzelne Steuerelemente verfeinern.

**Die visuelle Darstellung allein bestimmt nicht die Eingabeberechtigung.**

## 7. Routing-Hierarchie

NovaOS unterscheidet zwei Routing-Ebenen:

### System-Routing

Ermittelt den zuständigen Prozess, das Fenster oder den Systemdienst.

Verantwortlich ist die zentrale Input-Architektur in Zusammenarbeit mit dem Window Manager.

### UI-Routing

Ermittelt innerhalb eines berechtigten Fensters das zuständige UI-Element.

Nova UI DARF hierfür hierarchische Ereignisverfahren verwenden, beispielsweise:

- Tunneling: Vom übergeordneten Element zum Ziel.
- Bubbling: Vom Ziel zu übergeordneten Elementen.
- Direct: Direkte Verarbeitung am Ziel.

Diese UI-internen Verfahren verändern nicht die systemweite Sicherheits- und Sitzungszuordnung.

## 8. Fokus und Capture

Der Event Router berücksichtigt Fokus- und Capture-Änderungen.

Dabei gilt:

- Fokuswechsel werden vor dem Routing nachfolgender Ereignisse berücksichtigt.
- Aktives Capture besitzt innerhalb seiner Berechtigung Vorrang vor regulärem Hit Testing.
- Capture DARF keine Sitzungs- oder Sicherheitsgrenzen umgehen.
- Bei Verlust des Capture-Empfängers wird die Zuordnung aufgehoben.
- Bereits gebundene Touch- und Stiftkontakte werden nicht unkontrolliert auf andere Empfänger übertragen.

Routing-Entscheidungen MÜSSEN auf einem konsistenten Eingabezustand beruhen.

## 9. Sicherheit und Isolation

Nova Event Routing unterliegt Nova Input Security und Nova.Trust.

Festlegungen:

- Eingabeereignisse dürfen nur autorisierten Empfängern zugeordnet werden.
- Geschützte Eingabekontexte besitzen Vorrang vor regulärem Anwendungsrouting.
- Anwendungen DÜRFEN keine fremden Eingabeziele erzwingen.
- Virtuelle und injizierte Ereignisse unterliegen denselben Routing-Prüfungen wie physische Eingaben.
- Systemreservierte Eingabekombinationen DÜRFEN nicht durch Anwendungen abgefangen oder umgeleitet werden.
- Globale Eingabeüberwachung erfordert eine ausdrücklich autorisierte Capability.

Routing-Entscheidungen DÜRFEN keine vertraulichen Eingabeinhalte an unberechtigte Komponenten weitergeben.

## 10. Determinismus und Fehlerbehandlung

Das Routing erfolgt anhand definierter Regeln und eines konsistenten Zustands.

Dabei gilt:

- Identische Ereignisse und identische Routing-Zustände führen zu identischen Ergebnissen.
- Gleichzeitige Zustandsänderungen werden durch eine definierte Verarbeitungsreihenfolge aufgelöst.
- Ungültige Ziele führen zu einer kontrollierten Nichtzustellung oder zum Abbruch der betroffenen Eingabesequenz.
- Fehlerhafte Hit-Test-Daten werden zurückgewiesen.
- Ein fehlgeschlagenes Routing DARF andere Eingabeereignisse nicht dauerhaft blockieren.

Der Event Router arbeitet ereignisgesteuert und benötigt keine permanente Abfrage der Fensterhierarchie.

## 11. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale, hardwareunabhängige Ereignisrouting-Architektur bereitstellen.
- **INPUT-002:** Das Routing MUSS Eingabegerät, Ereignistyp, Input Seat und Benutzersitzung berücksichtigen.
- **INPUT-003:** Tastaturereignisse MÜSSEN anhand des gültigen Tastaturfokus geroutet werden.
- **INPUT-004:** Zeigereingaben MÜSSEN Hit Testing und Pointer Capture unterstützen.
- **INPUT-005:** Touch- und Stiftkontakte MÜSSEN eine stabile Kontaktzuordnung besitzen.
- **INPUT-006:** Systemreservierte Eingaben MÜSSEN vor regulärem Anwendungsrouting verarbeitet werden.
- **INPUT-007:** Hit Testing MUSS Fensterhierarchie, Sichtbarkeit, Transformationen und Eingabebereiche berücksichtigen.
- **INPUT-008:** System-Routing und UI-internes Routing MÜSSEN voneinander getrennt bleiben.
- **INPUT-009:** Fokus- und Capture-Änderungen MÜSSEN konsistent berücksichtigt werden.
- **INPUT-010:** Ungültige oder nicht mehr verfügbare Eingabeziele MÜSSEN sicher behandelt werden.
- **INPUT-011:** Routing DARF keine Sitzungs- oder Sicherheitsgrenzen umgehen.
- **INPUT-012:** Virtuelle Eingaben MÜSSEN denselben Berechtigungsprüfungen unterliegen.
- **INPUT-013:** Routing-Entscheidungen MÜSSEN bei identischem Ausgangszustand und identischer Ereignisfolge reproduzierbar sein.
- **INPUT-014:** Die Routing-Verarbeitung MUSS unabhängig von der Rendering-Frequenz erfolgen.
- **INPUT-015:** Nova Event Routing MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 12. Integration

- **Nova Input Architecture:** Übergeordnete Eingabeverarbeitung.
- **Nova Input Event Pipeline:** Bereitstellung normalisierter Eingabeereignisse.
- **Nova Event Dispatcher:** Zustellung an ermittelte Empfänger.
- **Nova Input Focus Manager:** Fokusinformationen.
- **Nova Multi-Device Input:** Input Seats und Gerätezuordnungen.
- **Nova Cursor Management:** Logische Cursorposition.
- **Nova Window Manager:** Fensterhierarchie und Hit Testing.
- **Nova UI:** Elementbezogenes Routing und Ereignisweitergabe.
- **Nova Input Security:** Sicherheitsprüfung.
- **Nova.Trust:** Berechtigungen und Capabilities.

## 13. Ergebnis

NovaOS erhält eine einheitliche Architektur zur sicheren und deterministischen Ermittlung von Eingabeempfängern.

Durch die Trennung von System-Routing, Hit Testing, Fokusverwaltung und UI-internem Routing können unterschiedliche Eingabegeräte und Oberflächen konsistent zusammenarbeiten.

**Grundsatz:** Nicht die Anwendung entscheidet, welche fremden Eingaben sie erhält. NovaOS bestimmt anhand von Fokus, Eingabekontext und Berechtigungen den zuständigen Empfänger.