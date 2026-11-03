# NPSPEC-DISPLAY-FEEDBACK-0001 – Nova Presentation Feedback

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Presentation Feedback definiert die Rückmeldung über den tatsächlichen Verlauf und das Ergebnis von Frame-Präsentationen.

Ziel ist die präzise Erfassung von Präsentationszeitpunkten, Latenzen und Frame-Status zur Optimierung von Frame Scheduling, Frame Pacing und Synchronisation.

Presentation Feedback unterscheidet ausdrücklich zwischen angenommenen, eingereichten und tatsächlich präsentierten Frames.

## 2. Architekturprinzipien

- **Event-Driven:** Rückmeldungen erfolgen ereignisgesteuert.
- **Asynchronous:** Präsentationen werden nicht durch Feedback-Abfragen blockiert.
- **Timestamp-Based:** Zeitpunkte werden über definierte Zeitquellen erfasst.
- **Frame-Accurate:** Rückmeldungen sind eindeutig einem Frame zugeordnet.
- **Output-Aware:** Jede Bildschirmausgabe besitzt eigene Präsentationsinformationen.
- **Hardware-Aware:** Hardwarebestätigungen werden bevorzugt.
- **Adaptive:** Feedback unterstützt die Optimierung zukünftiger Frames.

## 3. Presentation Feedback Model

Ein Feedback-Eintrag enthält mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Presentation ID | Eindeutige Präsentationsidentifikation |
| Frame ID | Zugehöriger Frame |
| Output ID | Betroffene Bildschirmausgabe |
| Surface ID | Zugehörige Surface, sofern vorhanden |
| Presentation Status | Ergebnis der Präsentation |
| Target Timestamp | Geplanter Präsentationszeitpunkt |
| Actual Timestamp | Bestätigter oder geschätzter Präsentationszeitpunkt |
| Timestamp Source | Herkunft des Zeitstempels |
| Refresh Interval | Zugehöriges Aktualisierungsintervall |
| Presentation Mode | Verwendetes Präsentationsverfahren |

Zeitstempel MÜSSEN einer definierten Zeitbasis zugeordnet sein.

Nicht verfügbare Messwerte MÜSSEN eindeutig gekennzeichnet werden.

## 4. Presentation Status

NovaOS MUSS folgende Präsentationsergebnisse unterscheiden können:

| Status | Bedeutung |
|---|---|
| Queued | Präsentation wartet auf Verarbeitung |
| Submitted | An das Display-Backend übergeben |
| Presented | Präsentation durch das Backend bestätigt |
| Superseded | Vor Präsentation durch neueren Frame ersetzt |
| Rejected | Präsentationsanforderung abgewiesen |
| Failed | Präsentation fehlgeschlagen |
| Unknown | Tatsächlicher Präsentationsstatus nicht ermittelbar |

Ein eingereichter Frame DARF nicht automatisch als präsentiert gelten.

Der Status `Presented` MUSS auf einer geeigneten Bestätigung des Display-Backends beruhen.

## 5. Timestamp Management

NovaOS SOLL folgende Zeitpunkte erfassen können:

- **Submit Time:** Übergabe an das Display-Backend.
- **Target Time:** Geplanter Präsentationszeitpunkt.
- **Presentation Time:** Bestätigter Zeitpunkt der Präsentation.
- **Completion Time:** Abschluss der zugehörigen Präsentationsoperation.

Dabei gilt:

1. Eine monotone Zeitbasis MUSS verwendet werden.
2. Unterschiedliche Hardware-Zeitbasen MÜSSEN korrekt zugeordnet werden.
3. Hardwarebestätigte und geschätzte Zeitpunkte MÜSSEN unterscheidbar sein.
4. Fehlende Zeitstempel DÜRFEN nicht durch erfundene Messwerte ersetzt werden.

Der Präsentationszeitpunkt beschreibt den vom Backend bestätigten Anzeigevorgang und nicht zwingend den Zeitpunkt, an dem ein Pixel physisch sichtbar wird.

## 6. Feedback Events

Nova Display Server MUSS asynchrone Feedback-Ereignisse bereitstellen.

Mindestens erforderlich sind:

- **PresentationSubmitted**
- **PresentationCompleted**
- **PresentationFailed**
- **PresentationSuperseded**
- **OutputTimingChanged**

Feedback-Ereignisse MÜSSEN eindeutig einer Präsentationsanforderung zugeordnet werden können.

Für jede angenommene Feedback-Anforderung MUSS ein definierter Abschlusszustand erreichbar sein, auch bei Geräteverlust oder Client-Abbruch.

## 7. Frame Scheduling Integration

Nova Presentation Feedback MUSS Informationen an Nova Frame Scheduling bereitstellen können.

Diese dienen insbesondere zur:

- Bestimmung tatsächlicher Präsentationsintervalle.
- Erkennung verpasster Präsentationsfristen.
- Optimierung des Frame Pacing.
- Abschätzung der Präsentationslatenz.
- Anpassung zukünftiger Scheduling-Entscheidungen.

Die Rückmeldungen DÜRFEN keinen permanenten Rendering-Takt erzwingen.

Ohne erforderliche Bildänderungen SOLLEN keine zusätzlichen Frames ausschließlich zur Erzeugung von Feedback gerendert werden.

## 8. Multi-Display und Presentation Modes

Feedback MUSS ausgabespezifisch verarbeitet werden.

Dabei gilt:

- Unterschiedliche Bildwiederholraten werden berücksichtigt.
- Variable Refresh Rate (VRR) MUSS bei entsprechender Hardware korrekt abgebildet werden können.
- Direct Scanout und Hardware Overlays MÜSSEN integrierbar sein.
- Ersetzte Frames MÜSSEN von tatsächlich präsentierten Frames unterscheidbar bleiben.

Bei mehreren Ausgaben DARF eine gemeinsame Frame ID nicht als Nachweis einer gleichzeitigen Präsentation gelten.

## 9. Performance und Diagnose

NovaOS SOLL aus Presentation Feedback folgende Kennzahlen ableiten können:

- Tatsächliche Präsentationsintervalle
- Abweichung vom geplanten Präsentationszeitpunkt
- Anzahl verworfener oder ersetzter Frames
- Verpasste Präsentationsfristen
- Präsentationslatenz, soweit messbar
- Häufigkeit fehlgeschlagener Präsentationen

Messwerte SOLLEN für Nova Graphics Performance und die Systemdiagnose verfügbar sein.

Die Erfassung MUSS mit geringem Ressourcenaufwand erfolgen.

## 10. Sicherheit und Fehlerbehandlung

Feedback-Zugriffe MÜSSEN capability-basiert kontrolliert werden.

Anwendungen DÜRFEN ausschließlich autorisierte Präsentationsinformationen erhalten.

Bei fehlenden Hardwarebestätigungen MUSS NovaOS den tatsächlichen Status als unbekannt kennzeichnen oder eine ausdrücklich als Schätzung ausgewiesene Rückmeldung bereitstellen.

Fehlerhafte oder verspätete Feedback-Ereignisse DÜRFEN keine bereits gültigen Präsentationszustände beschädigen.

## 11. Verbindliche Anforderungen

1. NovaOS MUSS asynchrones Presentation Feedback unterstützen.
2. Feedback MUSS eindeutig Frames und Ausgaben zugeordnet sein.
3. Eingereichte und tatsächlich präsentierte Frames MÜSSEN unterschieden werden.
4. Präsentationszeitstempel MÜSSEN eine definierte monotone Zeitbasis verwenden.
5. Hardwarebestätigte und geschätzte Zeitpunkte MÜSSEN unterscheidbar sein.
6. Fehlende Messwerte DÜRFEN nicht als gültige Messungen ausgegeben werden.
7. Ersetzte und fehlgeschlagene Präsentationen MÜSSEN erkennbar sein.
8. Feedback MUSS mit Nova Frame Scheduling zusammenarbeiten.
9. Mehrere Bildschirmausgaben MÜSSEN unabhängig unterstützt werden.
10. Direct Scanout und Hardware Overlays MÜSSEN berücksichtigt werden.
11. Feedback-Erfassung SOLL ressourcenschonend erfolgen.
12. Zugriffe MÜSSEN capability-basiert geschützt sein.
13. Feedback DARF keine unnötigen Rendering-Operationen erzwingen.
14. Im deterministischen Modus MÜSSEN aufgezeichnete Feedback-Ereignisse reproduzierbar verarbeitet werden können.

## 12. Ergebnis

Nova Presentation Feedback stellt zuverlässige Rückmeldungen über den tatsächlichen Ablauf der Frame-Präsentation bereit.

Durch eindeutige Frame-Zuordnung, präzise Zeitstempel und asynchrone Ereignisse können Frame Scheduling und Frame Pacing gezielt optimiert werden.

Damit erhält NovaOS die notwendige Rückkopplung für eine latenzarme, gleichmäßige und bedarfsgesteuerte Bildschirmausgabe.