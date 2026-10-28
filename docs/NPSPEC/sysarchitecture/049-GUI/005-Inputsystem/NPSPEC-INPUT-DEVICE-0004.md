# NPSPEC-INPUT-DEVICE-0004 – Nova Pen and Stylus Input

**Status:** Angenommen  
**Bereich:** Input / Device / Pen and Stylus  
**System:** NovaOS

## 1. Zweck

Nova Pen and Stylus Input definiert die hardwareunabhängige Erfassung und Verarbeitung digitaler Stift- und Grafiktabletteingaben innerhalb von NovaOS.

Ziel ist eine präzise, latenzarme und zuverlässige Unterstützung von Eingabestiften für Zeichnen, Schreiben, CAD, Handschrift und allgemeine Benutzerinteraktionen.

Die Architektur trennt physische Stiftdaten, Kontaktzustände, Koordinatentransformationen und semantische Eingabeaktionen.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Pen Driver Adapter | Anbindung hardwareabhängiger Stifttreiber |
| Pen Device Manager | Verwaltung digitaler Stifte und Grafiktabletts |
| Pen Event Processor | Normalisierung von Stiftereignissen |
| Pen State Manager | Verwaltung von Position, Kontakt und Tasten |
| Pen Coordinate Mapper | Transformation der Stiftkoordinaten |
| Pen Calibration Manager | Verwaltung von Kalibrierungsdaten |

Die Komponenten integrieren sich in die Nova Input Device Abstraction und Nova Input Event Pipeline.

Hardwareabhängige Funktionen verbleiben in den jeweiligen Treibern.

## 3. Stiftmodell

NovaOS unterscheidet drei Eingabeebenen.

### Physical Pen Input

Hardwarebezogene Informationen:

- Absolute Stiftposition.
- Kontaktdruck.
- Neigungswinkel.
- Rotation.
- Abstand zur Eingabefläche.
- Stifttasten.
- Radiererfunktion.
- Geräte- und Werkzeugkennung.

### Logical Pen Input

Normalisierte Eingaben:

- Stiftannäherung.
- Stiftbewegung.
- Kontaktbeginn.
- Kontaktbewegung.
- Kontaktende.
- Stiftentfernung.
- Tastenbetätigung.

### Semantic Pen Input

Kontextabhängige Aktionen:

- Zeichnen.
- Schreiben.
- Radieren.
- Auswählen.
- Navigieren.
- Kontextmenü öffnen.
- Objekte bearbeiten.

Die Interpretation semantischer Aktionen erfolgt durch Nova UI beziehungsweise die zuständige Anwendung.

## 4. Stiftereignisse

NovaOS unterstützt mindestens:

- `PenProximityEnter`
- `PenProximityLeave`
- `PenHover`
- `PenDown`
- `PenMove`
- `PenUp`
- `PenButtonDown`
- `PenButtonUp`
- `PenCancel`

Jedes Ereignis enthält mindestens:

- Gerätekennung.
- Werkzeugkennung, sofern verfügbar.
- Ereignistyp.
- Monotonen Zeitstempel.
- Stiftposition.
- Kontaktstatus.
- Relevante Zustandsinformationen.

Optionale Eigenschaften werden über Device Capabilities bereitgestellt.

Die Ereignisreihenfolge pro Stift MUSS erhalten bleiben.

## 5. Druck, Neigung und Rotation

NovaOS unterstützt folgende Stifteigenschaften, sofern die Hardware diese bereitstellt:

| Eigenschaft | Beschreibung |
|---|---|
| Pressure | Kontaktdruck |
| TiltX | Neigung entlang der X-Achse |
| TiltY | Neigung entlang der Y-Achse |
| Rotation | Drehung um die Stiftachse |
| Distance | Abstand zur Eingabefläche |
| ContactSize | Größe der Kontaktfläche |

Wertebereiche und Auflösungen werden über Device Capabilities beschrieben.

Normalisierte Werte MÜSSEN eindeutig definiert sein.

Die ursprünglichen Hardwarewerte SOLLEN für berechtigte Anwendungen verfügbar bleiben.

Nicht unterstützte Eigenschaften DÜRFEN nicht als tatsächlich gemessene Werte ausgegeben werden.

## 6. Stiftzustände

Der Pen State Manager verwaltet folgende Zustände:

1. **OutOfRange:** Stift befindet sich außerhalb des Erfassungsbereichs.
2. **Hover:** Stift wird ohne Kontakt erkannt.
3. **Contact:** Stift berührt die Eingabefläche.
4. **Released:** Kontakt wurde beendet.
5. **Cancelled:** Eingabesequenz wurde abgebrochen.

Die Unterstützung von Hover und Proximity hängt von den Gerätefähigkeiten ab.

Bei Geräteverlust MÜSSEN aktive Kontakte kontrolliert beendet werden.

Ungültige Ereignisfolgen DÜRFEN keine dauerhaft aktiven Stiftzustände verursachen.

## 7. Koordinaten und Kalibrierung

Der Pen Coordinate Mapper berücksichtigt:

- Physische Geräteauflösung.
- Aktive Eingabefläche.
- Display-Zuordnung.
- Display-Skalierung.
- Display-Rotation.
- Kalibrierungsdaten.
- Transformation in Fensterkoordinaten.

Grafiktabletts ohne integriertes Display MÜSSEN einem konfigurierbaren Ausgabebereich zugeordnet werden können.

Die Koordinatentransformation MUSS reproduzierbar erfolgen.

Kalibrierungsdaten MÜSSEN unabhängig vom Hardwaretreiber verwaltet werden können.

## 8. Werkzeuge und Stifttasten

NovaOS unterstützt unterschiedliche Stiftwerkzeuge.

Dazu gehören:

- Stiftspitze.
- Radierer.
- Zusätzliche Stifttasten.
- Geräteabhängige Werkzeugtypen.

Die Werkzeugfunktionen werden über Device Capabilities beschrieben.

Die Zuordnung von Stifttasten zu logischen Aktionen MUSS konfigurierbar sein.

Ein Werkzeugwechsel DARF aktive Eingabesequenzen nicht unkontrolliert verändern.

## 9. Handschrift und Zeicheneingaben

Nova Pen and Stylus Input stellt präzise Eingabedaten für Zeichen- und Handschriftanwendungen bereit.

Dazu gehören:

- Zeitlich geordnete Stiftpositionen.
- Druckverläufe.
- Neigungsinformationen.
- Kontaktbeginn und Kontaktende.
- Werkzeugwechsel.

Die eigentliche Strichdarstellung erfolgt durch die jeweilige Anwendung beziehungsweise Render Engine.

Handschrifterkennung ist nicht Bestandteil der grundlegenden Stifteingabeverarbeitung.

Sie DARF als optionale Capability ergänzt werden.

Die Stiftverarbeitung MUSS ohne Handschrifterkennung und ohne KI funktionieren.

## 10. Zusammenspiel mit Touch

NovaOS unterstützt Geräte, die gleichzeitig Stift- und Touch-Eingaben bereitstellen.

Dabei gilt:

- Stift- und Touchkontakte bleiben unterscheidbar.
- Gleichzeitige Eingaben MÜSSEN korrekt verarbeitet werden können.
- Palm Rejection SOLL unterstützt werden.
- Die Unterdrückung unbeabsichtigter Handflächenkontakte MUSS konfigurierbar sein.
- Touch-Eingaben DÜRFEN nicht grundsätzlich deaktiviert werden, nur weil ein Stift vorhanden ist.

Die Koordination erfolgt über die zentrale Input-Architektur.

## 11. Performance und Sicherheit

Die Stiftverarbeitung erfolgt ereignisgesteuert und mit geringer Latenz.

Festlegungen:

- Stiftbewegungen werden zeitnah verarbeitet.
- Hochfrequente Bewegungsdaten SOLLEN möglichst vollständig erhalten bleiben.
- Bewegungsereignisse DÜRFEN nur kontrolliert zusammengefasst werden.
- Kontaktbeginn, Kontaktende und Werkzeugwechsel DÜRFEN nicht unkontrolliert verloren gehen.
- Die Eingabeverarbeitung bleibt unabhängig von der Rendering-Frequenz.
- Anwendungen erhalten ausschließlich berechtigte Stifteingaben.
- Virtuelle Stiftereignisse MÜSSEN als solche identifizierbar sein.

Die Sicherheitskontrolle erfolgt über Nova Input Security und Nova.Trust.

## 12. Normative Anforderungen

- **INPUT-001:** NovaOS MUSS eine hardwareunabhängige Stifteingabeverarbeitung bereitstellen.
- **INPUT-002:** Absolute Stiftkoordinaten MÜSSEN unterstützt werden.
- **INPUT-003:** Stiftbewegungen und Kontaktzustände MÜSSEN getrennt verarbeitet werden.
- **INPUT-004:** Kontaktdruck MUSS unterstützt werden, sofern die Hardware entsprechende Daten bereitstellt.
- **INPUT-005:** Neigung und Rotation SOLLEN bei geeigneter Hardware unterstützt werden.
- **INPUT-006:** Unterstützte Stifteigenschaften MÜSSEN über Device Capabilities abfragbar sein.
- **INPUT-007:** Mehrere Stiftgeräte MÜSSEN gleichzeitig unterstützt werden können.
- **INPUT-008:** Stiftwerkzeuge und Stifttasten MÜSSEN eindeutig identifizierbar sein.
- **INPUT-009:** Stiftkoordinaten MÜSSEN in Display- und Fensterkoordinaten transformierbar sein.
- **INPUT-010:** Grafiktabletts MÜSSEN konfigurierbaren Ausgabebereichen zugeordnet werden können.
- **INPUT-011:** Kalibrierungsdaten MÜSSEN berücksichtigt werden können.
- **INPUT-012:** Stift- und Touch-Eingaben MÜSSEN unterscheidbar bleiben.
- **INPUT-013:** Palm Rejection SOLL unterstützt werden.
- **INPUT-014:** Bei Geräteverlust MÜSSEN aktive Stiftkontakte kontrolliert beendet werden.
- **INPUT-015:** Stiftereignisse MÜSSEN über die Nova Input Event Pipeline verarbeitet werden.
- **INPUT-016:** Die Stiftverarbeitung DARF nicht von der Rendering-Frequenz abhängig sein.
- **INPUT-017:** Virtuelle Stifteingaben MÜSSEN den Sicherheitsrichtlinien von Nova.Trust entsprechen.
- **INPUT-018:** Nova Pen and Stylus Input MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova Input Architecture:** Zentrale Eingabeverarbeitung.
- **Nova Input Device Abstraction:** Hardwareunabhängiges Stiftmodell.
- **Nova Input Event Pipeline:** Ereignisverarbeitung und Zustellung.
- **Nova Input Security:** Berechtigungen und Eingabeisolation.
- **Nova Touch Input:** Koordination von Stift- und Touchkontakten.
- **Nova USB/HID:** Anbindung kompatibler Stiftgeräte.
- **Nova Display System:** Koordinatentransformation und Display-Zuordnung.
- **Nova Window Manager:** Eingabefokus und Capture.
- **Nova UI:** Stiftbedienung und Interaktionen.
- **Nova Graphics:** Darstellung von Zeichen- und Handschriftinhalten.

## 14. Ergebnis

NovaOS erhält eine einheitliche Architektur zur Verarbeitung digitaler Stifte und Grafiktabletts.

Die Trennung von Hardwareerfassung, Stiftzuständen, Werkzeugfunktionen und Koordinatentransformation ermöglicht die Integration unterschiedlicher Stiftgeräte.

Druckempfindlichkeit, Neigung, Rotation und präzise Bewegungsdaten bilden die Grundlage für professionelle Zeichen-, CAD- und Handschriftanwendungen.

**Grundsatz:** NovaOS erfasst Stifteingaben mit ihrer vollständigen verfügbaren Präzision. Die Anwendung entscheidet, wie diese Informationen verwendet werden.