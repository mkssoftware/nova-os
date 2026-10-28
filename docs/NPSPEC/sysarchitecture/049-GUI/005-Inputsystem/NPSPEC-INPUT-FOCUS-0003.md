# NPSPEC-INPUT-FOCUS-0003 – Nova Input Capture

**Status:** Angenommen  
**Bereich:** Input / Focus / Capture  
**System:** NovaOS

## 1. Zweck

Nova Input Capture definiert die kontrollierte Bindung von Eingabeereignissen an einen bestimmten Empfänger.

Ziel ist die zuverlässige Verarbeitung zusammenhängender Eingabesequenzen, auch wenn sich der Zeiger außerhalb des ursprünglichen Eingabebereichs bewegt.

Input Capture ermöglicht beispielsweise Drag-and-Drop, Fensterbewegungen, Größenänderungen und komplexe Touch-Interaktionen.

Die Eingabebindung MUSS jederzeit kontrollierbar sein und darf keine Sicherheitsgrenzen umgehen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Input Capture Manager | Zentrale Verwaltung aktiver Capture-Bindungen |
| Capture State Controller | Verwaltung des Capture-Lebenszyklus |
| Capture Policy Controller | Prüfung von Berechtigungen und Einschränkungen |
| Capture Target Resolver | Ermittlung des gebundenen Eingabeempfängers |
| Capture Event Provider | Bereitstellung von Capture-Ereignissen |

Nova Input Capture arbeitet mit Nova Focus Manager und Nova Event Routing zusammen.

Die eigentliche Ereigniszustellung erfolgt über Nova Event Dispatcher.

## 3. Capture-Modell

NovaOS unterscheidet folgende Capture-Arten:

| Capture-Art | Beschreibung |
|---|---|
| Pointer Capture | Bindung eines bestimmten Pointers an einen Empfänger |
| Keyboard Capture | Kontrollierte Bindung von Tastatureingaben innerhalb eines autorisierten Fokuskontexts |
| Contact Capture | Bindung einer Touch- oder Stiftkontaktsequenz |
| Modal Capture | Einschränkung zulässiger Eingabeziele durch einen modalen Kontext |
| System Capture | Vorrangige Eingabebindung für autorisierte Systemfunktionen |

Capture ist grundsätzlich an einen Input Seat und einen gültigen Eingabekontext gebunden.

Eine Capture-Bindung besitzt mindestens:

- Eindeutige Capture-ID.
- Capture-Art.
- Eigentümer und Ziel.
- Input-Seat-Kennung.
- Betroffene Pointer- oder Gerätekennung, sofern erforderlich.
- Aktivierungszustand.
- Berechtigungsinformationen.

## 4. Capture-Lebenszyklus

Eine Capture-Bindung durchläuft folgende Zustände:

1. **Requested:** Capture wurde angefordert.
2. **Active:** Capture wurde genehmigt und aktiviert.
3. **Released:** Capture wurde regulär freigegeben.
4. **Cancelled:** Capture wurde durch das System abgebrochen.
5. **Invalid:** Capture ist aufgrund eines ungültigen Ziels nicht mehr verwendbar.

Die Aktivierung erfolgt ausschließlich nach erfolgreicher Prüfung.

Jede Capture-Bindung MUSS eindeutig freigegeben oder kontrolliert beendet werden können.

## 5. Pointer Capture

Pointer Capture bindet die Ereignisse eines bestimmten Pointers an einen festgelegten Empfänger.

Dabei gilt:

- Die Bindung bleibt bei Bewegungen außerhalb des ursprünglichen Eingabebereichs bestehen.
- Andere Pointer bleiben unabhängig.
- Pointer Capture verändert nicht automatisch den Keyboard Focus.
- Die Bindung endet bei regulärer Freigabe oder einem definierten Abbruchgrund.
- Pointer Capture ist unabhängig von Cursor Confinement.

Mehrere Pointer DÜRFEN gleichzeitig unterschiedliche Capture-Ziele besitzen.

Bereits bestehende Capture-Bindungen MÜSSEN bei neuen Anforderungen berücksichtigt werden.

## 6. Keyboard Capture

Keyboard Capture ermöglicht eine kontrollierte Eingabebindung innerhalb eines berechtigten Kontexts.

Dabei gilt:

- Keyboard Capture ersetzt nicht die reguläre Fokusverwaltung.
- Systemreservierte Tastenkombinationen behalten Vorrang.
- Geschützte Eingabekontexte können reguläres Keyboard Capture unterbrechen.
- Anwendungen DÜRFEN keine Tastatureingaben anderer Sitzungen abfangen.
- Keyboard Capture DARF nicht als unberechtigter globaler Tastatur-Hook verwendet werden.

Reguläre Tastatureingaben werden weiterhin durch Nova Focus Manager und Nova Event Routing koordiniert.

## 7. Capture-Prioritäten

NovaOS verwendet folgende Prioritätsreihenfolge:

1. Sicherheitskritische Systemeingaben.
2. Geschützte Systemkontexte.
3. Autorisierte System-Capture-Bindungen.
4. Modale Eingabekontexte.
5. Reguläre Capture-Bindungen.
6. Normales Event Routing.

Höher priorisierte Eingabekontexte können niedrigere Capture-Bindungen unterbrechen oder aufheben.

Die Konfliktauflösung MUSS deterministisch erfolgen.

Eine Anwendung DARF keine höher priorisierte Capture-Bindung eigenständig überschreiben.

## 8. Capture-Ereignisse

NovaOS stellt mindestens folgende Ereignisse bereit:

| Ereignis | Beschreibung |
|---|---|
| `CaptureRequested` | Capture wurde angefordert |
| `CaptureGained` | Capture wurde aktiviert |
| `CaptureReleased` | Capture wurde regulär freigegeben |
| `CaptureLost` | Capture wurde durch einen externen Zustandswechsel beendet |
| `CaptureCancelled` | Capture wurde kontrolliert abgebrochen |

Capture-Ereignisse enthalten:

- Capture-ID.
- Capture-Art.
- Betroffenen Input Seat.
- Capture-Eigentümer.
- Capture-Ziel.
- Monotonen Zeitstempel.
- Grund der Zustandsänderung.

Die Ereignisse werden über Nova Event Dispatcher zugestellt.

## 9. Automatische Freigabe

Aktive Capture-Bindungen MÜSSEN automatisch bereinigt werden bei:

- Beendigung oder Absturz des Eigentümers.
- Zerstörung des Capture-Ziels.
- Geräteverlust.
- Beendigung der Benutzersitzung.
- Entzug erforderlicher Berechtigungen.
- Aktivierung eines vorrangigen geschützten Eingabekontexts.
- Anderen sicherheitsrelevanten Zustandsänderungen.

Bei einer erzwungenen Freigabe MÜSSEN betroffene Eingabesequenzen kontrolliert beendet werden.

Verwaiste Capture-Bindungen sind unzulässig.

## 10. Zusammenspiel mit Fokus und Routing

Nova Input Capture beeinflusst die Zielermittlung, nicht die physische Erfassung von Eingaben.

Dabei gilt:

- Nova Focus Manager verwaltet den regulären Eingabefokus.
- Nova Input Capture verwaltet temporäre Eingabebindungen.
- Nova Event Routing berücksichtigt gültige Capture-Bindungen bei der Zielermittlung.
- Nova Event Dispatcher übernimmt die Ereigniszustellung.

Ein Capture-Wechsel DARF laufende Eingabesequenzen nicht unkontrolliert zwischen Empfängern aufteilen.

Die Aktivierung und Freigabe einer Capture-Bindung MUSS aus Sicht des Event Routings atomar erfolgen.

## 11. Sicherheit und Fehlerbehandlung

Nova Input Capture unterliegt Nova Input Security und Nova.Trust.

Festlegungen:

- Capture-Anforderungen MÜSSEN autorisiert werden.
- Anwendungen DÜRFEN ausschließlich Eingaben innerhalb ihres berechtigten Kontexts binden.
- Sitzungsgrenzen MÜSSEN eingehalten werden.
- Systemreservierte Eingaben DÜRFEN nicht unberechtigt abgefangen werden.
- Ungültige Capture-Ziele werden zurückgewiesen.
- Capture-Bindungen DÜRFEN keine Berechtigungen erweitern.
- Fehlerhafte Capture-Anforderungen DÜRFEN die reguläre Eingabeverarbeitung nicht blockieren.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine zentrale Verwaltung von Input-Capture-Bindungen bereitstellen.
- **INPUT-002:** Pointer Capture MUSS pro Pointer unterstützt werden.
- **INPUT-003:** Mehrere unabhängige Pointer-Capture-Bindungen MÜSSEN gleichzeitig möglich sein.
- **INPUT-004:** Capture-Anforderungen MÜSSEN vor der Aktivierung autorisiert werden.
- **INPUT-005:** Capture-Bindungen MÜSSEN eindeutig einem Eigentümer und Ziel zugeordnet sein.
- **INPUT-006:** Capture-Aktivierung und -Freigabe MÜSSEN aus Sicht des Event Routings atomar erfolgen.
- **INPUT-007:** Capture DARF den regulären Keyboard Focus nicht unbeabsichtigt verändern.
- **INPUT-008:** Systemreservierte und geschützte Eingaben MÜSSEN Vorrang besitzen.
- **INPUT-009:** Bei Geräteverlust oder Zielzerstörung MÜSSEN betroffene Capture-Bindungen automatisch freigegeben werden.
- **INPUT-010:** Capture-Zustandsänderungen MÜSSEN über definierte Ereignisse verfügbar sein.
- **INPUT-011:** Capture-Konflikte MÜSSEN deterministisch behandelt werden.
- **INPUT-012:** Capture-Bindungen DÜRFEN keine Sitzungs- oder Sicherheitsgrenzen umgehen.
- **INPUT-013:** Input Capture MUSS unabhängig von Cursor Confinement funktionieren.
- **INPUT-014:** Input Capture MUSS unabhängig von Nova UI und der Rendering-Frequenz funktionieren.
- **INPUT-015:** Identische Anforderungen und Ausgangszustände MÜSSEN reproduzierbare Ergebnisse liefern.
- **INPUT-016:** Nova Input Capture MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Focus Manager:** Reguläre Fokusverwaltung.
- **Nova Keyboard Focus Navigation:** Tastaturbasierte Fokuswechsel.
- **Nova Pointer Events:** Pointer-bezogene Eingabesequenzen.
- **Nova Keyboard Events:** Tastaturereignisse.
- **Nova Event Routing:** Capture-basierte Zielermittlung.
- **Nova Event Dispatcher:** Ereigniszustellung.
- **Nova Cursor Confinement:** Unabhängige räumliche Cursorbegrenzung.
- **Nova Gesture Recognition:** Gestensequenzen und deren Abbruch.
- **Nova Window Manager:** Fensterbewegungen und Größenänderungen.
- **Nova UI:** Steuerelementbezogene Eingabebindungen.
- **Nova Input Security / Nova.Trust:** Berechtigungen und Isolation.

## 14. Ergebnis

NovaOS erhält eine zentrale, sichere und deterministische Verwaltung temporärer Eingabebindungen.

Zusammenhängende Eingabesequenzen können zuverlässig verarbeitet werden, ohne dass Änderungen der Zeigerposition oder des Eingabekontexts zu unbeabsichtigten Empfängerwechseln führen.

**Grundsatz:** Input Capture bindet eine Eingabesequenz kontrolliert an ihren Empfänger, ohne den regulären Eingabefokus oder die Sicherheitsregeln von NovaOS außer Kraft zu setzen.