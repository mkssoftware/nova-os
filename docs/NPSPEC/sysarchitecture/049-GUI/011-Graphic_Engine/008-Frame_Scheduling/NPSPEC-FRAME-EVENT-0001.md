# NPSPEC-FRAME-EVENT-0001 – Nova Event-Driven Frame Scheduling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Frame Scheduler

## 1. Zweck

Nova Event-Driven Frame Scheduling definiert die ereignisgesteuerte Erzeugung grafischer Frames innerhalb von NovaOS.

Ziel ist die vollständige Vermeidung permanenter Rendering-Schleifen, wenn keine grafischen Änderungen vorliegen.

Neue Frames werden ausschließlich durch relevante Zustandsänderungen, zeitabhängige Darstellungsanforderungen oder notwendige Systemereignisse ausgelöst.

## 2. Architekturprinzipien

- **Event-Driven:** Ereignisse lösen die Prüfung des Frame-Bedarfs aus.
- **State-Aware:** Nur tatsächlich relevante Zustandsänderungen erzeugen neue Frames.
- **Damage-Driven:** Räumliche Änderungen bestimmen den erforderlichen Arbeitsumfang.
- **Coalescing:** Mehrere Ereignisse können zu einem Frame zusammengefasst werden.
- **Asynchronous:** Ereigniserfassung und Rendering sind entkoppelt.
- **Deadline-Aware:** Frames werden passend zum Präsentationszeitpunkt geplant.
- **Idle-Safe:** Ohne Aktualisierungsbedarf bleibt die Grafikpipeline inaktiv.
- **Deterministic:** Ereignisse werden nach definierten Ordnungsregeln verarbeitet.

## 3. Frame Event Model

Ein Frame Event enthält mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Event ID | Eindeutige Ereignisidentifikation |
| Event Type | Art des Ereignisses |
| Source ID | Auslösende Komponente |
| Output ID | Betroffene Bildschirmausgabe |
| State Version | Zugehörige Zustandsversion |
| Damage Reference | Betroffene Bildbereiche |
| Timestamp | Ereigniszeitpunkt |
| Priority | Verarbeitungspriorität |

Ereignisse MÜSSEN einem gültigen grafischen Zustand zugeordnet werden können.

## 4. Event Sources

NovaOS MUSS folgende Ereignisquellen unterstützen:

- **Scene Changes:** Änderungen am Scene Graph.
- **Surface Commits:** Veröffentlichung neuer Surface-Inhalte.
- **Window Changes:** Bewegung, Größe, Sichtbarkeit und Z-Order.
- **Material Changes:** Änderungen grafischer Effekte.
- **Animation Events:** Zeitabhängige Zustandsänderungen.
- **Input Events:** Eingaben mit sichtbaren Auswirkungen.
- **Display Events:** Änderungen der Ausgabekonfiguration.
- **System Events:** Erforderliche grafische Systemaktualisierungen.

Ein Ereignis MUSS nicht automatisch einen neuen Frame erzeugen.

Erst die Prüfung des tatsächlichen Aktualisierungsbedarfs entscheidet über die Frame-Erzeugung.

## 5. Event Processing

Die Ereignisverarbeitung erfolgt in folgenden Schritten:

1. Ereignis empfangen.
2. Betroffenen grafischen Zustand bestimmen.
3. Zustandsversion validieren.
4. Relevante Änderungen ermitteln.
5. Damage Regions aktualisieren.
6. Vorhandene Frame-Anforderungen berücksichtigen.
7. Erforderlichen Frame planen.

Ereignisse ohne sichtbare Auswirkungen SOLLEN keine Rendering-Arbeit auslösen.

## 6. Event Coalescing

Mehrere Ereignisse DÜRFEN innerhalb eines geeigneten Zeitfensters zusammengefasst werden.

Dabei gilt:

- Damage Regions werden vereinigt.
- Zustandsabhängigkeiten bleiben erhalten.
- Neuere Zustandsversionen können ältere ersetzen.
- Doppelte Frame-Anforderungen werden vermieden.
- Zeitkritische Ereignisse dürfen nicht unangemessen verzögert werden.

Das Zusammenfassen DARF keine erforderlichen grafischen Änderungen verlieren.

## 7. Animation Scheduling

Animationen benötigen zeitabhängige Aktualisierungen, auch wenn keine externen Ereignisse auftreten.

NovaOS MUSS dafür explizite Animation Deadlines unterstützen.

Dabei gilt:

- Aktive Animationen können zukünftige Frame-Ereignisse registrieren.
- Animationen werden nur zu erforderlichen Zeitpunkten aktualisiert.
- Nicht sichtbare Animationen SOLLEN ausgesetzt oder reduziert werden.
- Beendete Animationen DÜRFEN keine weiteren Frame-Ereignisse erzeugen.

Eine Animation DARF keine permanente globale Rendering-Schleife erzwingen.

## 8. Idle Frame Suppression

Wenn keine relevanten Änderungen oder ausstehenden zeitabhängigen Aktualisierungen vorliegen, MUSS der Frame Scheduler in einen inaktiven Zustand wechseln können.

Dabei gilt:

- Keine neuen Render Jobs.
- Keine unnötigen Composition Jobs.
- Keine künstlichen Surface Commits.
- Keine periodischen GPU-Submissions.
- Ereignisgesteuertes Aufwecken bei neuem Bedarf.

Der zuletzt gültige Frame bleibt weiterhin sichtbar.

Die physische Bildschirmausgabe kann unabhängig davon mit ihrer regulären Bildwiederholrate arbeiten.

## 9. Frame Scheduling Integration

Nova Event-Driven Frame Scheduling MUSS mit folgenden Komponenten zusammenarbeiten:

- **Nova Graphics State Change Tracking:** Erkennung relevanter Änderungen.
- **Nova Damage Tracking:** Bestimmung betroffener Bereiche.
- **Nova Scene Graph:** Versionierte grafische Zustände.
- **Nova Composition Scheduling:** Planung konkreter Kompositionsarbeiten.
- **Nova Frame Pacing:** Zeitliche Verteilung erforderlicher Frames.
- **Nova Display Server:** Präsentation fertiger Frames.
- **Nova Presentation Feedback:** Rückmeldung tatsächlicher Präsentationen.

Der Event Scheduler entscheidet über den Frame-Bedarf, nicht über die konkrete Ausführung der Rendering-Operationen.

## 10. Fehlerbehandlung

NovaOS MUSS Ereignisverluste und inkonsistente Zustandsversionen sicher behandeln.

Dabei gilt:

- Ereignisse DÜRFEN nicht unbemerkt erforderlichen Damage verlieren.
- Bei unvollständiger Ereignishistorie MUSS eine konservative Invalidierung möglich sein.
- Fehlerhafte Ereignisse MÜSSEN erkannt werden.
- Ausstehende Frame-Anforderungen MÜSSEN nach Fehlern wiederherstellbar sein.

Bei unbekanntem grafischem Zustand MUSS eine vollständige Aktualisierung der betroffenen Ausgabe möglich sein.

## 11. Verbindliche Anforderungen

1. NovaOS MUSS Frame-Anforderungen ereignisgesteuert erzeugen.
2. Ereignisse MÜSSEN auf tatsächlichen Aktualisierungsbedarf geprüft werden.
3. Grafische Zustandsänderungen MÜSSEN versioniert verarbeitet werden.
4. Damage Tracking MUSS integriert sein.
5. Mehrere Ereignisse SOLLEN zusammengefasst werden.
6. Zusammengefasste Ereignisse DÜRFEN keine erforderlichen Änderungen verlieren.
7. Animationen MÜSSEN zeitgesteuerte Frame-Anforderungen registrieren können.
8. Ohne Aktualisierungsbedarf DÜRFEN keine unnötigen Frames erzeugt werden.
9. Ereignisverarbeitung und Rendering MÜSSEN entkoppelt sein.
10. Mehrere Bildschirmausgaben MÜSSEN unabhängig berücksichtigt werden.
11. Fehlerhafte Ereignisfolgen MÜSSEN sicher behandelt werden.
12. Ein vollständiger Redraw MUSS als Fallback möglich sein.
13. Im deterministischen Modus MÜSSEN Ereignisreihenfolge und Scheduling-Entscheidungen reproduzierbar sein.

## 12. Ergebnis

Nova Event-Driven Frame Scheduling stellt sicher, dass NovaOS neue Frames ausschließlich bei tatsächlichem Aktualisierungsbedarf erzeugt.

Durch Ereignisverarbeitung, Damage Tracking, Event Coalescing und explizite Animation Deadlines werden unnötige Rendering-Zyklen vermieden.

Damit bleibt die Grafikpipeline bei unveränderten Bildschirminhalten inaktiv und reagiert gleichzeitig mit geringer Latenz auf relevante Änderungen.