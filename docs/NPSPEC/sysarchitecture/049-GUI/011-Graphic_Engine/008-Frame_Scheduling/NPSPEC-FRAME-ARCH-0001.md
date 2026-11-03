# NPSPEC-FRAME-ARCH-0001 – Nova Frame Scheduling Architecture

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Frame Scheduling Architecture definiert die zentrale Planung, Koordination und zeitliche Steuerung grafischer Frames innerhalb von NovaOS.

Ziel ist eine ereignisgesteuerte, latenzarme und ressourcenschonende Grafikpipeline, die ausschließlich bei tatsächlichem Bedarf neue Frames erzeugt.

Unveränderte Bildschirminhalte MÜSSEN ohne permanente Neuberechnung dargestellt bleiben können.

## 2. Architekturprinzipien

- **Event-Driven:** Frame-Erzeugung erfolgt aufgrund relevanter Ereignisse.
- **Damage-Driven:** Grafische Änderungen bestimmen den Aktualisierungsbedarf.
- **Deadline-Aware:** Rendering wird anhand angestrebter Präsentationszeitpunkte geplant.
- **Adaptive:** Die Planung berücksichtigt Systemlast und Hardwarefähigkeiten.
- **Latency-Aware:** Unnötige Verzögerungen werden minimiert.
- **Power-Aware:** CPU- und GPU-Aktivität wird bei fehlendem Bedarf reduziert.
- **Multi-Display:** Jede Bildschirmausgabe besitzt eine unabhängige Frame-Planung.
- **Deterministic:** Scheduling-Entscheidungen sind anhand definierter Eingaben reproduzierbar.

## 3. Architekturmodell

Nova Frame Scheduler besteht logisch aus folgenden Komponenten:

| Komponente | Verantwortung |
|---|---|
| Event Collector | Erfassung relevanter Grafikereignisse |
| Damage Evaluator | Bestimmung des Aktualisierungsbedarfs |
| Frame Planner | Planung erforderlicher Frames |
| Deadline Manager | Verwaltung angestrebter Fertigstellungszeiten |
| Frame Pacing | Steuerung gleichmäßiger Frame-Abstände |
| VSync Coordinator | Abstimmung mit der Bildschirmausgabe |
| Latency Controller | Optimierung der Eingabe-zu-Anzeige-Latenz |
| Power Controller | Vermeidung unnötiger Grafikaktivität |
| Presentation Tracker | Verarbeitung von Presentation Feedback |

Die Komponenten MÜSSEN über definierte Schnittstellen zusammenarbeiten.

## 4. Frame Model

Ein geplanter Frame enthält mindestens:

- **Frame ID:** Eindeutige Identifikation.
- **Output ID:** Zugehörige Bildschirmausgabe.
- **Scene Version:** Verwendeter grafischer Zustands-Snapshot.
- **Damage Regions:** Zu aktualisierende Bildbereiche.
- **Target Presentation Time:** Angestrebter Präsentationszeitpunkt.
- **Render Deadline:** Spätester geplanter Rendering-Abschluss.
- **Priority:** Ausführungspriorität.
- **Frame State:** Aktueller Verarbeitungszustand.

Ein Frame MUSS auf einem konsistenten grafischen Zustand basieren.

## 5. Frame Lifecycle

Ein Frame durchläuft folgende Zustände:

`Requested → Scheduled → Rendering → Ready → Submitted → Presented`

Weitere mögliche Zustände:

- **Deferred:** Verarbeitung aufgeschoben.
- **Superseded:** Durch einen neueren Frame ersetzt.
- **Cancelled:** Vor der Ausführung abgebrochen.
- **Failed:** Verarbeitung oder Präsentation fehlgeschlagen.

Ein Frame DARF erst nach Bestätigung des Display-Backends als präsentiert gelten.

Die Zustandsverwaltung MUSS zwischen Rendering-Abschluss und tatsächlicher Präsentation unterscheiden.

## 6. Event-Driven Frame Generation

NovaOS MUSS neue Frames durch relevante Ereignisse auslösen können.

Dazu gehören:

- Änderungen am Scene Graph
- Surface-Commits
- Fensterbewegungen und Größenänderungen
- Änderungen von Materialeffekten
- Animationen
- Eingabeabhängige UI-Aktualisierungen
- Änderungen der Bildschirmausgabe
- Explizite Aktualisierungsanforderungen

Mehrere Ereignisse DÜRFEN zu einem Frame zusammengefasst werden.

Ohne relevanten Aktualisierungsbedarf DARF kein permanenter Rendering-Zyklus erforderlich sein.

## 7. Damage-Driven Scheduling

Nova Frame Scheduler MUSS mit Nova Damage Tracking zusammenarbeiten.

Dabei gilt:

1. Grafische Änderungen erzeugen Damage.
2. Damage wird den betroffenen Ausgaben zugeordnet.
3. Der Scheduler bestimmt den erforderlichen Frame-Bedarf.
4. Der Compositor verarbeitet die betroffenen Bereiche.
5. Der Display Server präsentiert das Ergebnis.

Unveränderte Bereiche SOLLEN nicht erneut berechnet werden.

Damage DARF erst verworfen werden, wenn seine Verarbeitung und Übernahme in einen gültigen Ausgabezustand sichergestellt sind.

## 8. Deadline und Frame Pacing

NovaOS SOLL Rendering-Aufträge anhand der erwarteten Präsentationszeitpunkte planen.

Dabei werden berücksichtigt:

- Bildwiederholrate
- VSync-Zeitpunkte
- Variable Refresh Rate
- Erwartete Rendering-Dauer
- GPU- und CPU-Auslastung
- Präsentationslatenz
- Verfügbare Buffer

Frames SOLLEN möglichst spät, aber rechtzeitig für den vorgesehenen Präsentationszeitpunkt berechnet werden.

Die Planung DARF keine unnötigen Wartezeiten erzeugen.

## 9. Adaptive Frame Scheduling

NovaOS SOLL die Frame-Planung dynamisch anpassen können.

Mögliche Strategien:

| Strategie | Verwendung |
|---|---|
| Immediate | Zeitkritische Aktualisierungen |
| VSync-Aligned | Synchronisierte Desktop-Darstellung |
| Deferred | Zusammenfassbare Änderungen |
| Throttled | Begrenzte Aktualisierungsfrequenz |
| Idle | Keine neuen Frames erforderlich |
| Power-Saving | Reduzierte Aktivität bei geringer Priorität |

Die Auswahl MUSS die grafische Korrektheit und erforderliche Aktualisierungen erhalten.

## 10. Multi-Display Scheduling

Jede Bildschirmausgabe MUSS unabhängig geplant werden können.

Dabei gilt:

- Unterschiedliche Bildwiederholraten werden unterstützt.
- Jede Ausgabe besitzt eigene Frame-Deadlines.
- Damage wird ausgabespezifisch verwaltet.
- Gemeinsame Rendering-Ergebnisse dürfen wiederverwendet werden.
- Langsame Ausgaben SOLLEN andere Ausgaben nicht unnötig blockieren.

Eine globale Synchronisation mehrerer Ausgaben wird nur verwendet, wenn sie ausdrücklich erforderlich und technisch möglich ist.

## 11. Presentation Feedback

Nova Frame Scheduler MUSS Rückmeldungen des Nova Display Server verarbeiten können.

Dazu gehören:

- Tatsächliche Präsentationszeitpunkte
- Verpasste Deadlines
- Ersetzte Frames
- Präsentationsintervalle
- Ausgabe- und Synchronisationsfehler

Diese Informationen SOLLEN zur Verbesserung zukünftiger Scheduling-Entscheidungen verwendet werden.

Geschätzte und hardwarebestätigte Zeitpunkte MÜSSEN unterscheidbar bleiben.

## 12. Idle und Power Management

Bei unveränderten Bildschirminhalten MUSS NovaOS unnötige Rendering- und Kompositionsarbeit vermeiden.

Dabei gilt:

- Keine periodische Frame-Erzeugung ohne Bedarf.
- Keine unnötigen GPU-Submissions.
- Keine künstlichen Surface-Updates.
- Ereignisgesteuertes Aufwecken bei Änderungen.
- Unterstützung energiesparender CPU- und GPU-Zustände.

Die physische Display-Hardware DARF den zuletzt gültigen Frame weiterhin entsprechend ihrer Betriebsart ausgeben.

Idle Frame Suppression bedeutet nicht das Abschalten des Displays.

## 13. Integration

Nova Frame Scheduling Architecture arbeitet mit:

- **Nova Scene Graph:** Grafische Zustandsänderungen.
- **Nova Damage Tracking:** Räumlicher Aktualisierungsbedarf.
- **Nova Render Engine:** Ausführung grafischer Berechnungen.
- **Nova Compositor:** Zusammensetzung der Ausgabe.
- **Nova Composition Scheduling:** Planung konkreter Kompositionsaufträge.
- **Nova Display Server:** Frame-Präsentation.
- **Nova Presentation Feedback:** Rückmeldung tatsächlicher Ausgabezeitpunkte.
- **Nova GPU Synchronization:** Ressourcen- und Ausführungssynchronisation.

Der Frame Scheduler bestimmt, **wann ein Frame benötigt wird**.

Der Compositor bestimmt, **welche Kompositionsarbeiten erforderlich sind**.

Die Render Engine bestimmt, **wie grafische Inhalte berechnet werden**.

Der Display Server übernimmt die **tatsächliche Präsentation**.

## 14. Fehlerbehandlung

NovaOS MUSS fehlgeschlagene oder nicht rechtzeitig fertiggestellte Frames kontrolliert behandeln.

Dazu gehören:

- Verpasste Rendering-Deadlines
- Nicht verfügbare Surface-Buffer
- GPU-Synchronisationsfehler
- Fehlgeschlagene Präsentationen
- Geräteverlust
- Änderungen der Display-Konfiguration

Ein fehlgeschlagener Frame DARF erforderlichen Damage nicht verlieren.

Wenn möglich, SOLL der zuletzt gültige Frame sichtbar bleiben.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS einen zentralen Frame Scheduler bereitstellen.
2. Frame-Erzeugung MUSS ereignisgesteuert erfolgen.
3. Damage Tracking MUSS den Frame-Bedarf bestimmen können.
4. Unveränderte Inhalte DÜRFEN keine permanente Neuberechnung erfordern.
5. Frames MÜSSEN konsistente Zustandsversionen verwenden.
6. Frame-Deadlines SOLLEN berücksichtigt werden.
7. VSync und Frame Pacing MÜSSEN integrierbar sein.
8. Adaptive Scheduling-Strategien SOLLEN unterstützt werden.
9. Mehrere Bildschirmausgaben MÜSSEN unabhängig planbar sein.
10. Presentation Feedback MUSS verarbeitet werden können.
11. Unnötige CPU- und GPU-Aktivität MUSS vermieden werden.
12. Veraltete Frames DÜRFEN ersetzt werden, sofern keine erforderlichen Änderungen verloren gehen.
13. Fehlgeschlagene Frames DÜRFEN ausstehenden Damage nicht verwerfen.
14. Frame Scheduling MUSS von Rendering und Präsentation getrennt bleiben.
15. Im deterministischen Modus MÜSSEN Scheduling-Entscheidungen anhand definierter Eingaben und einer kontrollierten Zeitbasis reproduzierbar sein.

## 16. Ergebnis

Nova Frame Scheduling Architecture bildet die zentrale zeitliche Steuerung der NovaOS-Grafikpipeline.

Durch Event-Driven Scheduling, Damage Tracking, adaptive Deadlines und Presentation Feedback werden ausschließlich tatsächlich benötigte Frames erzeugt.

Damit reduziert NovaOS unnötige CPU- und GPU-Arbeit, verbessert die Darstellungslatenz und ermöglicht eine gleichmäßige, energieeffiziente Bildschirmausgabe.