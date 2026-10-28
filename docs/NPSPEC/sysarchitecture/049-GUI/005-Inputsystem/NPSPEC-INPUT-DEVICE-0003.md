# NPSPEC-INPUT-DEVICE-0003 – Nova Touch Input

**Status:** Angenommen  
**Bereich:** Input / Device / Touch  
**System:** NovaOS

## 1. Zweck

Nova Touch Input definiert die hardwareunabhängige Erfassung und Verarbeitung von Berührungseingaben innerhalb von NovaOS.

Ziel ist eine präzise, latenzarme und zuverlässige Unterstützung von Touchscreens und Multitouch-Geräten.

Die Architektur trennt physische Berührungserkennung, Kontaktverwaltung, Koordinatentransformation und Gesteninterpretation.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Touch Driver Adapter | Anbindung hardwareabhängiger Touch-Treiber |
| Touch Device Manager | Verwaltung angeschlossener Touch-Geräte |
| Touch Event Processor | Normalisierung von Berührungsereignissen |
| Touch Contact Manager | Verwaltung aktiver Berührungskontakte |
| Touch Coordinate Mapper | Transformation der Berührungskoordinaten |
| Touch Calibration Manager | Verwaltung gerätespezifischer Kalibrierungsdaten |

Die Komponenten integrieren sich in die Nova Input Device Abstraction und Nova Input Event Pipeline.

Die Erkennung von Gesten erfolgt durch eine übergeordnete Gestenverarbeitung.

## 3. Touch-Modell

NovaOS unterscheidet drei Ebenen:

### Physical Touch

Hardwarebezogene Informationen:

- Kontaktkennung.
- Absolute Position.
- Kontaktdruck, sofern verfügbar.
- Kontaktfläche.
- Gerätezeitstempel.
- Gerätespezifische Zusatzinformationen.

### Logical Touch

Normalisierte Berührungsinformationen:

- Kontaktbeginn.
- Kontaktbewegung.
- Kontaktende.
- Kontaktabbruch.
- Position im logischen Koordinatensystem.

### Semantic Touch

Kontextabhängige Aktionen:

- Tippen.
- Doppeltippen.
- Langes Drücken.
- Ziehen.
- Scrollen.
- Vergrößern und Verkleinern.
- Drehen.

Semantische Aktionen werden durch die Gestenverarbeitung beziehungsweise Nova UI interpretiert.

## 4. Touch-Ereignisse

NovaOS unterstützt mindestens folgende Ereignisse:

- `TouchDown`
- `TouchMove`
- `TouchUp`
- `TouchCancel`
- `TouchDeviceConnected`
- `TouchDeviceDisconnected`

Jedes Kontakt-Ereignis enthält:

- Gerätekennung.
- Kontaktkennung.
- Ereignistyp.
- Monotonen Zeitstempel.
- Kontaktposition.
- Optionale Druck- und Flächeninformationen.

Kontaktkennungen MÜSSEN während der Lebensdauer eines aktiven Kontakts eindeutig sein.

Die Ereignisreihenfolge eines Kontakts MUSS erhalten bleiben.

## 5. Multitouch

NovaOS unterstützt mehrere gleichzeitige Berührungskontakte.

Dabei gilt:

- Jeder aktive Kontakt besitzt eine eigene Kennung.
- Kontakte werden unabhängig voneinander verfolgt.
- Mehrere Kontakte können gleichzeitig beginnen, bewegt und beendet werden.
- Die maximale Kontaktanzahl wird über Device Capabilities bereitgestellt.
- Die Verarbeitung DARF keine feste Anzahl von Kontakten voraussetzen.

Mehrere Touch-Geräte MÜSSEN gleichzeitig betrieben werden können.

Kontaktkennungen werden immer im Zusammenhang mit der jeweiligen Gerätekennung interpretiert.

## 6. Koordinaten und Kalibrierung

Touch-Geräte liefern absolute Positionsinformationen.

Der Touch Coordinate Mapper berücksichtigt:

- Physische Geräteauflösung.
- Display-Auflösung.
- Display-Skalierung.
- Display-Rotation.
- Kalibrierungsdaten.
- Zuordnung zu einem Display.
- Transformation in Fensterkoordinaten.

Die ursprünglichen Gerätekoordinaten SOLLEN für berechtigte Komponenten verfügbar bleiben.

Die Transformation MUSS reproduzierbar erfolgen.

Änderungen der Display-Konfiguration MÜSSEN bei der Koordinatenzuordnung berücksichtigt werden.

## 7. Kontaktverwaltung

Der Touch Contact Manager verwaltet den Lebenszyklus jedes Kontakts.

Ein Kontakt durchläuft folgende Zustände:

1. **Inactive:** Kein aktiver Kontakt.
2. **Pressed:** Kontakt wurde erkannt.
3. **Moving:** Kontaktposition verändert sich.
4. **Released:** Kontakt wurde regulär beendet.
5. **Cancelled:** Kontakt wurde abgebrochen.

Nach `TouchUp` oder `TouchCancel` gilt der Kontakt als beendet.

Bei Geräteverlust MÜSSEN alle aktiven Kontakte kontrolliert abgebrochen werden.

Ungültige Ereignisfolgen DÜRFEN keine dauerhaft aktiven Kontakte verursachen.

## 8. Gestenverarbeitung

Nova Touch Input stellt die erforderlichen Kontaktinformationen für die Gestenerkennung bereit.

Unterstützbare Gesten sind:

- Tap.
- Double Tap.
- Long Press.
- Pan.
- Swipe.
- Pinch.
- Rotate.

Die Gestenerkennung erfolgt unabhängig vom Hardwaretreiber.

Anwendungen DÜRFEN rohe Touch-Ereignisse oder interpretierte Gesten verwenden, sofern ihr Eingabekontext dies erlaubt.

Gestenerkennung und direkte Kontaktverarbeitung MÜSSEN koordiniert werden, damit dieselbe Eingabe nicht unbeabsichtigt mehrfach als Aktion ausgeführt wird.

## 9. Touch Capture und Fokus

Touch-Ereignisse werden anhand von Sitzung, Oberfläche und Eingabeberechtigungen zugestellt.

Dabei gilt:

- Der Kontaktbeginn bestimmt den ursprünglichen Eingabeempfänger.
- Ein aktiver Kontakt bleibt grundsätzlich seinem Eingabekontext zugeordnet.
- Ein Fokuswechsel DARF aktive Kontakte nicht unkontrolliert an andere Anwendungen übertragen.
- Touch Capture MUSS kontrolliert und widerrufbar sein.
- Bei Verlust des Eingabekontexts wird der Kontakt kontrolliert abgebrochen.

Die Zustellung erfolgt über die Nova Input Event Pipeline.

## 10. Performance und Ressourcenökonomie

Die Touch-Verarbeitung erfolgt ereignisgesteuert.

Festlegungen:

- Berührungskontakte werden mit möglichst geringer Latenz verarbeitet.
- Bewegungsereignisse DÜRFEN kontrolliert zusammengefasst werden.
- Kontaktbeginn und Kontaktende DÜRFEN nicht unkontrolliert verworfen werden.
- Die Verarbeitung bleibt unabhängig von der Rendering-Frequenz.
- Unveränderte Kontakte verursachen keine unnötigen Aktualisierungen.
- Eingaben SOLLEN im nächsten geeigneten UI-Frame berücksichtigt werden.

Touch-Ereignisse können über Nova Frame Pacing mit der grafischen Darstellung koordiniert werden.

## 11. Sicherheit und Fehlerbehandlung

Nova Touch Input unterliegt Nova Input Security und Nova.Trust.

Dabei gilt:

- Touch-Ereignisse werden ausschließlich berechtigten Empfängern zugestellt.
- Virtuelle Touch-Eingaben MÜSSEN identifizierbar sein.
- Eingabeinjektionen erfordern entsprechende Berechtigungen.
- Geschützte Eingabekontexte dürfen nicht unberechtigt überwacht werden.
- Ungültige Kontaktkennungen und Koordinaten werden kontrolliert behandelt.
- Geräteausfälle DÜRFEN andere Eingabegeräte nicht beeinträchtigen.
- Bei Geräteverlust werden aktive Kontakte durch `TouchCancel` beendet.

Fehlerhafte Touch-Daten DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine hardwareunabhängige Touch-Verarbeitung bereitstellen.
- **INPUT-002:** Absolute Berührungskoordinaten MÜSSEN unterstützt werden.
- **INPUT-003:** Multitouch MUSS unterstützt werden.
- **INPUT-004:** Jeder aktive Kontakt MUSS eindeutig identifizierbar sein.
- **INPUT-005:** Kontaktbeginn, Bewegung, Ende und Abbruch MÜSSEN unterscheidbar sein.
- **INPUT-006:** Die logische Ereignisreihenfolge pro Kontakt MUSS erhalten bleiben.
- **INPUT-007:** Mehrere Touch-Geräte MÜSSEN gleichzeitig unterstützt werden.
- **INPUT-008:** Die maximale Kontaktanzahl MUSS über Device Capabilities abfragbar sein.
- **INPUT-009:** Touch-Koordinaten MÜSSEN in Display- und Fensterkoordinaten transformierbar sein.
- **INPUT-010:** Kalibrierungsdaten MÜSSEN berücksichtigt werden können.
- **INPUT-011:** Display-Rotation und Skalierung MÜSSEN bei der Koordinatentransformation berücksichtigt werden.
- **INPUT-012:** Gestenverarbeitung MUSS unabhängig vom Hardwaretreiber möglich sein.
- **INPUT-013:** Touch Capture MUSS kontrolliert und widerrufbar sein.
- **INPUT-014:** Bei Geräteverlust MÜSSEN aktive Kontakte kontrolliert abgebrochen werden.
- **INPUT-015:** Touch-Ereignisse MÜSSEN über die Nova Input Event Pipeline verarbeitet werden.
- **INPUT-016:** Die Touch-Verarbeitung DARF nicht von der Rendering-Frequenz abhängig sein.
- **INPUT-017:** Virtuelle Touch-Eingaben MÜSSEN den Sicherheitsrichtlinien von Nova.Trust entsprechen.
- **INPUT-018:** Nova Touch Input MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Input Device Abstraction:** Hardwareunabhängiges Touch-Modell.
- **Nova Input Event Pipeline:** Verarbeitung und Zustellung der Touch-Ereignisse.
- **Nova Input Security:** Berechtigungen und Eingabeisolation.
- **Nova USB/HID:** Anbindung kompatibler Touch-Geräte.
- **Nova Display System:** Display-Zuordnung und Koordinatentransformation.
- **Nova Window Manager:** Eingabeziele und Touch Capture.
- **Nova UI:** Touch-Bedienung und Gestenverarbeitung.
- **Nova Frame Pacing:** Zeitnahe Darstellung von Touch-Interaktionen.

## 14. Ergebnis

NovaOS erhält eine einheitliche Architektur zur Verarbeitung von Touch- und Multitouch-Eingaben.

Die Trennung von Hardwareerfassung, Kontaktverwaltung, Koordinatentransformation und Gestenerkennung ermöglicht die Integration unterschiedlicher Touch-Geräte ohne gerätespezifische Anpassungen in Anwendungen.

**Grundsatz:** Jede Berührung wird als eigenständiger Kontakt präzise verfolgt. Welche Aktion daraus entsteht, entscheidet der jeweilige Eingabekontext.