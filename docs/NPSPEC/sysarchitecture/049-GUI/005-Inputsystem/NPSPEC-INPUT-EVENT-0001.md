# NPSPEC-INPUT-EVENT-0001 – Nova Event Dispatcher

**Status:** Angenommen  
**Bereich:** Input / Event / Dispatcher  
**System:** NovaOS

## 1. Zweck

Nova Event Dispatcher definiert die zentrale Zustellung von Eingabeereignissen an berechtigte Empfänger innerhalb von NovaOS.

Ziel ist eine latenzarme, deterministische und sichere Ereignisverteilung, die unabhängig von Eingabegeräten, Anwendungen und der grafischen Oberfläche funktioniert.

Der Dispatcher verarbeitet bereits normalisierte Ereignisse aus der Nova Input Event Pipeline.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Event Dispatcher | Zentrale Ereigniszustellung |
| Event Router | Ermittlung des zuständigen Empfängers |
| Event Queue Manager | Verwaltung der Empfängerwarteschlangen |
| Event Subscription Manager | Verwaltung zulässiger Ereignisregistrierungen |
| Event Delivery Controller | Kontrolle von Reihenfolge und Zustellung |

Der Dispatcher ist Bestandteil der Nova Input Architecture.

Er arbeitet unabhängig von Nova UI, Compositor und Rendering-Frequenz.

## 3. Ereignismodell

Ein Eingabeereignis enthält mindestens:

- Ereignistyp.
- Gerätekennung.
- Input-Seat-Kennung.
- Monotonen Zeitstempel.
- Ereignissequenznummer.
- Ereignisdaten.
- Herkunftsinformationen.

Der Dispatcher DARF zusätzliche Routing-Metadaten ergänzen.

Die ursprünglichen Eingabedaten DÜRFEN durch die Zustellung nicht unkontrolliert verändert werden.

## 4. Ereignisrouting

Die Zustellung erfolgt anhand folgender Informationen:

- Aktiver Input Seat.
- Zugehörige Benutzersitzung.
- Eingabefokus.
- Pointer- beziehungsweise Touch-Capture.
- Ereignistyp.
- Berechtigungen des Empfängers.

Die Zielermittlung erfolgt über den Nova Input Focus Manager und die zuständigen Eingabekontexte.

Der Dispatcher MUSS sicherstellen, dass Ereignisse ausschließlich an zulässige Empfänger gelangen.

## 5. Ereigniswarteschlangen

Jeder Empfängerkontext besitzt eine logisch getrennte Ereigniswarteschlange.

Dabei gilt:

- Ereignisse werden asynchron zugestellt.
- Warteschlangen besitzen definierte Kapazitätsgrenzen.
- Langsame Empfänger DÜRFEN andere Empfänger nicht blockieren.
- Die Reihenfolge zustandsrelevanter Ereignisse MUSS erhalten bleiben.
- Überlastsituationen werden kontrolliert behandelt.

Bewegungsereignisse DÜRFEN zusammengefasst werden, sofern dadurch keine relevanten Zustandsänderungen verloren gehen.

Tasten-, Kontakt- und Capture-Zustandswechsel DÜRFEN nicht unkontrolliert verworfen werden.

## 6. Ereignisreihenfolge

NovaOS gewährleistet eine konsistente Ereignisreihenfolge innerhalb eines Eingabekontexts.

Festlegungen:

- Die Reihenfolge pro Eingabegerät bleibt erhalten.
- Zustandsänderungen werden vor davon abhängigen Ereignissen berücksichtigt.
- Ereignisse unterschiedlicher Geräte benötigen keine globale Totalordnung.
- Die tatsächliche Zustellreihenfolge wird für die Verarbeitung eindeutig festgelegt.
- Identische Eingabefolgen und identische Ausgangszustände MÜSSEN reproduzierbare Routing-Ergebnisse liefern.

Zeitstempel dienen der zeitlichen Einordnung, ersetzen jedoch nicht die Ereignissequenzierung.

## 7. Ereignisregistrierung

Systemkomponenten und Anwendungen können sich für zulässige Ereigniskategorien registrieren.

Unterstützt werden:

- Registrierung.
- Deregistrierung.
- Ereignistypfilter.
- Kontextbezogene Ereigniszustellung.
- Kontrollierte Beendigung von Registrierungen.

Eine Registrierung begründet keine zusätzlichen Eingabeberechtigungen.

Globale Eingabeüberwachung erfordert eine ausdrücklich autorisierte Capability.

Registrierungen MÜSSEN beim Beenden des zugehörigen Empfängers automatisch freigegeben werden.

## 8. Fokuswechsel und Capture

Der Dispatcher berücksichtigt Änderungen des Eingabefokus und aktive Capture-Zuordnungen.

Dabei gilt:

- Neue Ereignisse werden anhand des gültigen Eingabekontexts geroutet.
- Laufende Touch- und Stiftkontakte behalten ihre zulässige Kontaktzuordnung.
- Pointer Capture besitzt innerhalb seiner Berechtigung Vorrang vor regulärem Pointer-Routing.
- Bei ungültigen Empfängern werden betroffene Eingabesequenzen kontrolliert abgebrochen.
- Fokuswechsel DÜRFEN keine unbeabsichtigte Zustellung an fremde Anwendungen verursachen.

## 9. Sicherheit und Isolation

Nova Event Dispatcher unterliegt Nova Input Security und Nova.Trust.

Festlegungen:

- Ereignisse dürfen Sitzungsgrenzen nicht unberechtigt überschreiten.
- Geschützte Eingabekontexte besitzen eigene Zustellregeln.
- Virtuelle und injizierte Ereignisse bleiben anhand ihrer Herkunft unterscheidbar.
- Anwendungen DÜRFEN keine fremden Eingabewarteschlangen auslesen.
- Sicherheitskritische Systemereignisse werden ausschließlich autorisierten Systemkomponenten zugestellt.
- Sensible Eingabeinhalte DÜRFEN nicht unnötig protokolliert werden.

## 10. Fehlerbehandlung und Performance

Der Dispatcher arbeitet ereignisgesteuert und ressourcenschonend.

Bei Fehlern gilt:

- Nicht erreichbare Empfänger werden kontrolliert abgemeldet.
- Überfüllte Warteschlangen lösen definierte Überlastmaßnahmen aus.
- Zustandskritische Ereignisverluste führen zu einer kontrollierten Zustandssynchronisierung oder zum Abbruch der betroffenen Eingabesequenz.
- Fehler einzelner Anwendungen DÜRFEN die zentrale Eingabeverarbeitung nicht blockieren.
- Ungültige Ereignisse werden zurückgewiesen.

Der Dispatcher DARF keine permanente Verarbeitungsschleife ohne anstehende Ereignisse benötigen.

## 11. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS einen zentralen Input Event Dispatcher bereitstellen.
- **INPUT-002:** Der Dispatcher MUSS normalisierte Eingabeereignisse verarbeiten.
- **INPUT-003:** Ereignisse MÜSSEN anhand von Sitzung, Fokus, Capture und Berechtigungen geroutet werden.
- **INPUT-004:** Empfängerkontexte MÜSSEN logisch getrennte Ereigniswarteschlangen besitzen.
- **INPUT-005:** Langsame Empfänger DÜRFEN andere Empfänger nicht blockieren.
- **INPUT-006:** Die Ereignisreihenfolge pro Gerät MUSS erhalten bleiben.
- **INPUT-007:** Zustandskritische Ereignisse DÜRFEN nicht unkontrolliert verworfen werden.
- **INPUT-008:** Bewegungsereignisse DÜRFEN kontrolliert zusammengefasst werden.
- **INPUT-009:** Ereignisregistrierungen MÜSSEN kontrolliert verwaltet und freigegeben werden.
- **INPUT-010:** Globale Eingabeüberwachung MUSS ausdrücklich autorisiert werden.
- **INPUT-011:** Fokus- und Capture-Änderungen MÜSSEN bei der Zustellung berücksichtigt werden.
- **INPUT-012:** Ereignisse DÜRFEN Sicherheits- und Sitzungsgrenzen nicht unberechtigt überschreiten.
- **INPUT-013:** Virtuelle und injizierte Ereignisse MÜSSEN anhand ihrer Herkunft unterscheidbar bleiben.
- **INPUT-014:** Überlastsituationen MÜSSEN kontrolliert behandelt werden.
- **INPUT-015:** Der Dispatcher MUSS unabhängig von Nova UI und der Rendering-Frequenz funktionieren.
- **INPUT-016:** Bei identischem Ausgangszustand und identischer Ereignisfolge MUSS das Routing reproduzierbar sein.
- **INPUT-017:** Nova Event Dispatcher MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 12. Integration

- **Nova Input Architecture:** Übergeordnete Eingabearchitektur.
- **Nova Input Event Pipeline:** Bereitstellung normalisierter Ereignisse.
- **Nova Input Device Abstraction:** Geräteidentität und Herkunft.
- **Nova Multi-Device Input:** Input Seats und geräteübergreifende Zustände.
- **Nova Input Focus Manager:** Ermittlung des Eingabefokus.
- **Nova Cursor Confinement:** Berücksichtigung kontrollierter Zeigereingaben.
- **Nova Window Manager:** Fensterbezogenes Routing.
- **Nova UI:** Verarbeitung zugestellter UI-Ereignisse.
- **Nova IPC:** Sichere Zustellung über Prozessgrenzen.
- **Nova Input Security / Nova.Trust:** Berechtigungen und Isolation.

## 13. Ergebnis

NovaOS erhält einen zentralen, sicheren und deterministischen Dispatcher für Eingabeereignisse.

Durch getrennte Warteschlangen, kontrolliertes Routing und definierte Überlastbehandlung können mehrere Geräte und Anwendungen gleichzeitig Eingaben verarbeiten, ohne sich gegenseitig zu blockieren.

**Grundsatz:** Jedes Eingabeereignis erreicht ausschließlich den zuständigen Empfänger – in der richtigen Reihenfolge, mit minimaler Verzögerung und unter Einhaltung aller Sicherheitsgrenzen.