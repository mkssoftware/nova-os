
# NPSPEC-STUDIO-PROFILER-0001 – NovaLang Studio Profiler

## Status

Angenommen

## Kategorie

NovaLang Studio / Profiling / Performanceanalyse

## Zweck

Definiert den integrierten Profiler zur Analyse von Laufzeitverhalten und Ressourcenverbrauch von NovaLang-Programmen und NovaOS-Solutions.

Ziel ist die zuverlässige Erkennung von Leistungsengpässen bei möglichst geringer Beeinflussung der Programmausführung.

## Architektur

Der Profiler besteht aus:

- **Profiler Manager:** Verwaltung der Profiling-Sitzungen.
- **Runtime Collector:** Erfassung von Laufzeit- und Ressourcenmesswerten.
- **Sampling Engine:** Stichprobenbasierte Performanceanalyse.
- **Allocation Tracker:** Untersuchung von Speicherallokationen und GC-Aktivität.
- **Graph Profiler:** Analyse von Logic-Graph-Knoten und Datenflüssen.
- **Results Viewer:** Darstellung und Vergleich der Ergebnisse.

Die Implementierung verwendet die Profiling-Schnittstellen aus `NPSPEC-NOVALANG-PROFILING-0001`.

## Profiling-Funktionen

Unterstützt werden:

- CPU-Zeit und Funktionslaufzeiten
- Speicherverbrauch und Allokationen
- Garbage-Collection-Aktivität
- Task- und Thread-Auslastung
- Wartezeiten und Synchronisation
- Capability-Aufrufdauer
- Logic-Graph-Knoten und Datenflüsse
- Vergleich verschiedener Profiling-Sitzungen

Sampling wird bevorzugt. Instrumentierung ist bei Bedarf zuschaltbar.

## Integration

- **Code Editor:** Navigation zu analysierten Funktionen und Quellcodepositionen.
- **Logic Graph:** Darstellung von Knotenausführungszeiten und Engpässen.
- **Execution Manager:** Start und Beendigung von Profiling-Sitzungen.
- **Debugger:** Gemeinsame Ausführungskontexte und Debug-Informationen.
- **Build Manager:** Zuordnung der Messwerte zu Build und Artefakten.

Profiling muss für Interpreter, JIT und AOT unterstützt werden, soweit das jeweilige Backend die erforderlichen Messdaten bereitstellt.

## Normative Anforderungen

1. NovaLang Studio MUSS einen zentralen Profiler bereitstellen.
2. CPU-, Speicher- und Laufzeitprofiling MÜSSEN unterstützt werden.
3. Profiling-Sitzungen MÜSSEN eindeutig identifizierbar sein.
4. Messwerte MÜSSEN Funktionen, Tasks oder Graph-Knoten zugeordnet werden können.
5. Sampling und optionale Instrumentierung MÜSSEN unterscheidbar sein.
6. Der durch Profiling verursachte Overhead MUSS messbar oder abschätzbar und begrenzbar sein.
7. Ergebnisse MÜSSEN gespeichert und verglichen werden können.
8. Messungen MÜSSEN kontrolliert startbar, stoppbar und abbrechbar sein.
9. Profiling DARF keine Capability-, Sandbox- oder Datenschutzgrenzen umgehen.
10. Nicht verfügbare oder ungenaue Messwerte MÜSSEN entsprechend gekennzeichnet werden.
11. Der Profiler MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält einen integrierten, ressourcenschonenden Profiler zur gezielten Analyse und Optimierung von Programmen, Solutions und Logic Graphs.
