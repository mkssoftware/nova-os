
# NPSPEC-NOVALANG-PROFILING-0001 – NovaLang Profiling

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Performanceanalyse

## Zweck

Definiert ein einheitliches Profiling-System zur Analyse von Ausführungszeit, Speicherverbrauch und Ressourcenverhalten.

Ziel ist die Erkennung von Performanceproblemen bei minimalem Messaufwand und ohne Veränderung der definierten NovaLang-Semantik.

## Architektur

| Komponente | Aufgabe |
|---|---|
| CPU Profiler | Analyse von CPU-Zeit und Funktionsaufrufen |
| Memory Profiler | Analyse von Allokationen und Speicherverbrauch |
| Task Profiler | Untersuchung von Tasks und Wartezeiten |
| GC Profiler | Analyse von Garbage-Collection-Aktivitäten |
| JIT Profiler | Messung von Kompilierung und Optimierung |
| Resource Profiler | Erfassung von Ressourcenverbrauch |
| Trace Collector | Sammlung zeitbezogener Ereignisse |

Alle Komponenten verwenden ein gemeinsames, versioniertes Profiling-Datenmodell.

## Profiling-Modi

| Modus | Beschreibung |
|---|---|
| Sampling | Periodische Messung mit geringem Overhead |
| Instrumentation | Gezielte Messpunkte für detaillierte Analysen |
| Tracing | Zeitliche Aufzeichnung von Ereignissen |
| Allocation Tracking | Erfassung von Speicherallokationen |

Profiling muss zur Laufzeit aktivierbar und deaktivierbar sein, soweit der jeweilige Ausführungsmodus dies unterstützt.

## Messdaten

Das Profiling-System erfasst bei Bedarf:

- Ausführungszeit und CPU-Auslastung
- Funktionsaufrufe und Aufrufhäufigkeit
- Speicherallokationen und Heap-Nutzung
- GC-Dauer und Unterbrechungen
- Task-Laufzeiten und Wartezustände
- JIT-Kompilierungszeit und Code-Cache-Nutzung
- Ressourcenverbrauch einzelner Execution Contexts

Messwerte müssen ihrer Quelle und ihrem Ausführungskontext zugeordnet werden können.

## Ausführungsmodi

- **Interpreter:** Profiling virtueller Instruktionen und Funktionen.
- **JIT:** Profiling interpretierter und kompilierter Codebereiche.
- **AOT:** Profiling über native Symbole und Messpunkte.

Optimierter Code muss anhand verfügbarer Debug- und Symbolinformationen zugeordnet werden.

## NovaLang Studio

NovaLang Studio visualisiert Profiling-Daten durch:

- Flame Graphs
- Call Trees
- Zeitachsen
- Speicher- und Allokationsanalysen
- Task- und Async-Analysen
- Performancevergleiche zwischen Messläufen

Bei Solutions können Messwerte einzelnen Logic-Graph-Knoten zugeordnet werden.

## Performance und Sicherheit

- Profiling muss ohne aktive Messung möglichst keinen zusätzlichen Overhead verursachen.
- Messintervalle, Puffergrößen und Datenvolumen müssen begrenzbar sein.
- Sampling darf keine Echtzeitgarantien vortäuschen.
- Profiling fremder Schutzdomänen benötigt ausdrückliche Autorisierung.
- Geschützte Daten dürfen nicht unautorisiert aufgezeichnet werden.
- Profiling darf keine Capability-Berechtigungen erzeugen.

## Normative Anforderungen

1. NovaLang MUSS ein einheitliches Profiling-System für AOT, JIT und Interpreter unterstützen.
2. CPU-, Speicher-, Task- und GC-Profiling MÜSSEN verfügbar sein.
3. Sampling und Instrumentation MÜSSEN unterstützt werden.
4. Messdaten MÜSSEN strukturiert und versioniert bereitgestellt werden.
5. Profiling MUSS Ressourcenlimits und Schutzdomänen berücksichtigen.
6. Messungen DÜRFEN die definierte Programmsemantik nicht verändern.
7. NovaLang Studio MUSS Profiling-Daten auswerten können.
8. Logic-Graph-Komponenten MÜSSEN einzeln analysierbar sein.
9. Die Profiling-Infrastruktur DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält ein einheitliches, ressourcenschonendes Profiling-System zur Performanceanalyse von Programmen, Tasks und Solutions mit direkter Integration in NovaLang Studio.
