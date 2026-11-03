# NPSPEC-GPU-PARALLEL-0001 – Nova Parallel Command Recording

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Command Processing

## 1. Zweck

Nova Parallel Command Recording definiert die parallele Aufzeichnung von GPU-Befehlen durch mehrere CPU-Threads.

Ziel ist die effiziente Nutzung mehrerer CPU-Kerne, die Reduzierung der Command-Recording-Latenz und die beschleunigte Vorbereitung umfangreicher GPU-Arbeitslasten.

Die parallele Befehlsaufzeichnung MUSS unabhängig von der tatsächlichen Parallelität der GPU-Ausführung funktionieren.

## 2. Architekturprinzipien

- **Multi-Threaded:** Unabhängige Befehlssequenzen können gleichzeitig aufgezeichnet werden.
- **Task-Based:** Aufzeichnung erfolgt über planbare Recording Tasks.
- **Thread-Local:** Jeder Recording-Kontext besitzt einen isolierten Zustand.
- **Dependency-Aware:** Abhängigkeiten bestimmen die zulässige Parallelität.
- **Lock-Minimized:** Gemeinsame Sperren werden möglichst vermieden.
- **Scalable:** Anpassung an verfügbare CPU-Kerne und Arbeitslast.
- **Backend-Independent:** Unterstützung unterschiedlicher GPU-APIs.
- **Deterministic:** Reproduzierbare logische Befehlsreihenfolge.

## 3. Recording Model

NovaOS verwendet folgende Komponenten:

| Komponente | Verantwortung |
|---|---|
| Recording Scheduler | Verteilung der Recording Tasks |
| Recording Task | Unabhängige Aufzeichnungseinheit |
| Command Encoder | Erstellung der GPU-Befehle |
| Command Pool | Bereitstellung von Command Buffers |
| Dependency Tracker | Verwaltung der Aufzeichnungsabhängigkeiten |
| Command Collector | Zusammenführung abgeschlossener Befehlssequenzen |

Recording Tasks MÜSSEN eindeutig identifizierbar und einem definierten Ausführungskontext zugeordnet sein.

## 4. Parallel Recording

Unabhängige Recording Tasks DÜRFEN gleichzeitig auf mehreren CPU-Kernen ausgeführt werden.

Dabei gilt:

1. Jeder Task erhält einen eigenen Recording-Kontext.
2. Veränderliche Encoder-Zustände DÜRFEN nicht unkontrolliert gemeinsam verwendet werden.
3. Gemeinsame Ressourcen werden über definierte Referenzen eingebunden.
4. Aufzeichnungsabhängigkeiten werden vor der Ausführung berücksichtigt.
5. Abgeschlossene Command Buffers werden kontrolliert veröffentlicht.

Parallele Aufzeichnung DARF keine Datenrennen oder inkonsistenten Befehlssequenzen verursachen.

## 5. Task Partitioning

NovaOS SOLL GPU-Arbeit in geeignete Recording Tasks aufteilen.

Mögliche Aufteilungskriterien:

- Render Passes
- Scene-Graph-Bereiche
- Sichtbare Render-Regionen
- Materialgruppen
- Unabhängige Compute-Aufträge
- Transferoperationen

Die Aufteilung MUSS den zusätzlichen Scheduling- und Synchronisationsaufwand berücksichtigen.

Sehr kleine Aufgaben SOLLEN zusammengefasst werden, wenn parallele Verarbeitung keinen wirtschaftlichen Vorteil bietet.

## 6. Thread-Local Command Pools

NovaOS SOLL thread-lokale Command Pools unterstützen.

Dabei gilt:

- Jeder Recording Worker verwendet einen geeigneten Command Pool.
- Command Encoder werden nicht gleichzeitig durch mehrere Threads verändert.
- Häufige globale Speicherallokationen werden vermieden.
- Command Pools werden erst nach Abschluss abhängiger GPU-Ausführungen zurückgesetzt.
- Ressourcenbudgets werden berücksichtigt.

Backend-spezifische Threading-Anforderungen MÜSSEN eingehalten werden.

## 7. Dependency Management

Nova Parallel Command Recording MUSS mit Nova Render Graph Dependencies zusammenarbeiten.

Dabei gilt:

- Unabhängige Render Passes können parallel aufgezeichnet werden.
- Abhängige Passes behalten ihre logische Ausführungsreihenfolge.
- Ressourcenabhängigkeiten werden bei der späteren GPU-Ausführung berücksichtigt.
- Recording-Abhängigkeiten und GPU-Ausführungsabhängigkeiten bleiben getrennt.

Eine GPU-Ausführungsabhängigkeit bedeutet nicht grundsätzlich, dass die Befehlsaufzeichnung seriell erfolgen muss.

## 8. Command Collection

Abgeschlossene Command Buffers werden durch den Command Collector zusammengeführt.

Dabei gilt:

1. Jeder Recording Task veröffentlicht ein abgeschlossenes Ergebnis.
2. Ergebnisse werden anhand definierter Abhängigkeiten geordnet.
3. Kompatible Befehlssequenzen können gemeinsam eingereicht werden.
4. Erforderliche Ressourcenbarrieren bleiben erhalten.
5. Ungültige oder unvollständige Ergebnisse werden zurückgewiesen.

Die Reihenfolge des Abschlusses einzelner CPU-Threads DARF nicht unkontrolliert die logische GPU-Ausführungsreihenfolge bestimmen.

## 9. Adaptive Parallelism

NovaOS SOLL den Parallelisierungsgrad dynamisch anpassen.

Berücksichtigt werden:

- Anzahl verfügbarer CPU-Kerne
- Anzahl unabhängiger Recording Tasks
- Geschätzte Aufzeichnungskosten
- CPU-Auslastung
- Synchronisationsaufwand
- Frame-Deadlines

Bei geringer Arbeitslast DARF die Aufzeichnung vollständig seriell erfolgen.

Zusätzliche Threads SOLLEN nur verwendet werden, wenn ein messbarer oder begründet erwartbarer Vorteil besteht.

## 10. Frame Scheduling Integration

Nova Parallel Command Recording MUSS mit Nova Frame Scheduling zusammenarbeiten.

Dabei gilt:

- Zeitkritische Recording Tasks werden bevorzugt.
- Frame-Deadlines werden berücksichtigt.
- Veraltete, noch nicht benötigte Recording Tasks DÜRFEN kontrolliert verworfen werden.
- Bereits veröffentlichte Command Buffers bleiben bis zur sicheren Freigabe gültig.
- Ohne Rendering-Bedarf werden keine Recording Tasks erzeugt.

Die parallele Aufzeichnung SOLL die Frame-Latenz reduzieren, ohne unnötige CPU-Aktivität zu verursachen.

## 11. Synchronization und Sicherheit

NovaOS MUSS gemeinsam verwendete Recording-Daten kontrolliert synchronisieren.

Dabei gilt:

- Veränderliche Zustände werden thread-lokal gehalten oder explizit geschützt.
- Veröffentlichte Command Buffers sind unveränderlich.
- Ressourcenreferenzen MÜSSEN gültig und autorisiert sein.
- Prozessübergreifende Ressourcenfreigaben erfordern entsprechende Berechtigungen.
- CPU-Synchronisation DARF erforderliche GPU-Synchronisation nicht ersetzen.

Globale Sperren SOLLEN auf das notwendige Minimum reduziert werden.

## 12. Fehlerbehandlung

NovaOS MUSS Fehler einzelner Recording Tasks isoliert behandeln können.

Dabei gilt:

- Fehlgeschlagene Tasks werden eindeutig gekennzeichnet.
- Unvollständige Command Buffers DÜRFEN nicht eingereicht werden.
- Abhängige Tasks werden kontrolliert behandelt.
- Ressourcen werden sicher freigegeben.
- Ein serieller Recording-Fallback MUSS verfügbar sein.

Ein Fehler während der parallelen Aufzeichnung DARF keine ungültige GPU-Submission verursachen.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS parallele GPU-Befehlsaufzeichnung unterstützen können.
2. Recording Tasks MÜSSEN unabhängig planbar sein.
3. Jeder Recording-Kontext MUSS einen kontrollierten veränderlichen Zustand besitzen.
4. Gleichzeitige ungeschützte Zugriffe auf Command Encoder DÜRFEN nicht erfolgen.
5. Thread-lokale Command Pools SOLLEN unterstützt werden.
6. Unabhängige Render Passes SOLLEN parallel aufgezeichnet werden können.
7. Recording- und GPU-Ausführungsabhängigkeiten MÜSSEN unterschieden werden.
8. Abgeschlossene Command Buffers MÜSSEN kontrolliert zusammengeführt werden.
9. Die logische GPU-Ausführungsreihenfolge MUSS erhalten bleiben.
10. Der Parallelisierungsgrad SOLL adaptiv bestimmt werden.
11. Frame-Deadlines MÜSSEN berücksichtigt werden.
12. Unnötige Recording Tasks SOLLEN vermieden werden.
13. Ressourcenreferenzen und Lebensdauern MÜSSEN geschützt bleiben.
14. Fehlgeschlagene Recording Tasks DÜRFEN keine ungültigen Submissions erzeugen.
15. Ein serieller Fallback MUSS verfügbar sein.
16. Im deterministischen Modus MÜSSEN Task-Aufteilung, veröffentlichte Befehlssequenzen und logische Ausführungsreihenfolge bei identischen Eingaben reproduzierbar sein.

## 14. Ergebnis

Nova Parallel Command Recording ermöglicht die gleichzeitige Vorbereitung unabhängiger GPU-Befehlssequenzen auf mehreren CPU-Kernen.

Durch isolierte Recording-Kontexte, thread-lokale Command Pools, adaptive Task-Verteilung und kontrollierte Zusammenführung wird der CPU-seitige Aufwand der Grafikpipeline reduziert.

Damit kann NovaOS moderne Mehrkernprozessoren effizient nutzen, ohne die grafische Korrektheit, Ressourcenisolation oder deterministische Befehlsplanung zu beeinträchtigen.