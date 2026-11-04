# NPSPEC-GRAPHICSPERF-PROFILING-0001 – Nova Graphics Profiling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics / Nova Graphics Performance

## 1. Zweck

Nova Graphics Profiling definiert die detaillierte Untersuchung der Ausführungszeiten, Ressourcenverwendung und Leistungsengpässe innerhalb der NovaOS-Grafikarchitektur.

Ziel ist die präzise Identifikation kostenintensiver Grafikoperationen sowie die Bewertung möglicher Optimierungen.

Das Profiling MUSS CPU-, GPU-, Rendering-, Compositor- und Display-Aktivitäten zeitlich korrelieren können.

## 2. Architekturprinzipien

- **On-Demand:** Detailliertes Profiling erfolgt nur bei Bedarf.
- **Low-Overhead:** Messungen beeinflussen die Ausführung möglichst wenig.
- **Timeline-Based:** Grafikoperationen werden zeitlich nachvollziehbar dargestellt.
- **Cross-Subsystem:** Zusammenhängende Operationen können subsystemübergreifend analysiert werden.
- **Frame-Aware:** Profiling-Daten können einzelnen Frames zugeordnet werden.
- **Hardware-Aware:** Verfügbare Hardware-Performance-Counter werden berücksichtigt.
- **Non-Blocking:** Messungen dürfen kritische Rendering-Pfade nicht unnötig blockieren.
- **Reproducible:** Profiling-Konfigurationen und Messbedingungen sind dokumentierbar.

## 3. Architektur

Nova Graphics Profiling erweitert Nova Graphics Performance Architecture.

| Komponente | Verantwortung |
|---|---|
| Profiling Controller | Steuerung der Profiling-Sitzungen |
| Profiling Session Manager | Verwaltung aktiver Messungen |
| CPU Profiler | Analyse CPU-seitiger Grafikoperationen |
| GPU Profiler | Analyse GPU-seitiger Operationen |
| Timeline Correlator | Zeitliche Verknüpfung von Ereignissen |
| Bottleneck Analyzer | Erkennung möglicher Leistungsengpässe |
| Profiling Report Provider | Bereitstellung der Analyseergebnisse |

Messwerte werden über Nova Graphics Performance Metrics erfasst.

Eine zweite Performance-Messinfrastruktur DARF nicht entstehen.

## 4. Profiling Session Model

Jede Profiling-Sitzung MUSS folgende Informationen besitzen:

| Eigenschaft | Beschreibung |
|---|---|
| Session ID | Eindeutige Sitzungsidentifikation |
| Target | Zu untersuchendes Subsystem oder Programm |
| Profiling Mode | Gewählter Messmodus |
| Start Time | Beginn der Messung |
| End Time | Ende der Messung |
| Metric Selection | Aktivierte Kennzahlen |
| Frame Range | Untersuchte Frames |
| Hardware Context | Relevante Hardwareinformationen |
| Session State | Aktueller Sitzungszustand |

Profiling-Sitzungen MÜSSEN kontrolliert gestartet, beendet und ausgewertet werden können.

## 5. Profiling Modes

NovaOS SOLL folgende Profiling-Modi unterstützen:

| Modus | Beschreibung |
|---|---|
| Frame Profiling | Analyse einzelner Frames |
| CPU Profiling | Untersuchung CPU-seitiger Grafikoperationen |
| GPU Profiling | Untersuchung GPU-Ausführungszeiten |
| Render Graph Profiling | Analyse einzelner Render-Passes |
| Memory Profiling | Untersuchung des Grafikressourcenverbrauchs |
| Material Profiling | Analyse von Material- und Effektkosten |
| Composition Profiling | Untersuchung der Fensterkomposition |
| Timeline Profiling | Zeitliche Gesamtdarstellung |

Mehrere Modi DÜRFEN kombiniert werden, sofern der zusätzliche Messaufwand vertretbar bleibt.

## 6. CPU Profiling

NovaOS MUSS CPU-seitige Grafikoperationen analysieren können.

Dazu gehören:

- Scene-Graph-Aktualisierung.
- Damage Tracking.
- Render-Graph-Erstellung.
- Render Command Recording.
- GPU Command Submission.
- Ressourcenverwaltung.
- Shader-Kompilierung.
- CPU-Synchronisationswartezeiten.

CPU-Profiling MUSS zwischen aktiver Ausführungszeit und Wartezeit unterscheiden können.

## 7. GPU Profiling

NovaOS SOLL GPU-Operationen mithilfe verfügbarer Hardware-Zeitstempel analysieren.

Dazu gehören:

- Render-Pass-Ausführung.
- Compute-Operationen.
- Transferoperationen.
- Shader-Ausführung.
- GPU-Queue-Wartezeiten.
- Ressourcenabhängigkeiten.
- GPU-Synchronisationspunkte.

Hardware-Performance-Counter SOLLEN verwendet werden, sofern verfügbar.

Nicht unterstützte Messfunktionen MÜSSEN als nicht verfügbar gekennzeichnet werden.

## 8. Frame Profiling

NovaOS MUSS einzelne Frames vollständig analysieren können.

Dabei SOLLEN folgende Phasen unterscheidbar sein:

1. Ereignis- und Zustandsverarbeitung.
2. Scene Update.
3. Damage Analysis.
4. Render Graph Preparation.
5. Command Recording.
6. GPU Submission.
7. GPU Execution.
8. Composition.
9. Presentation.

Die Phasen DÜRFEN zeitlich überlappen.

Profiling MUSS tatsächliche Abhängigkeiten und Ausführungszeiten berücksichtigen.

## 9. Timeline Correlation

Nova Graphics Profiling MUSS zusammenhängende Ereignisse zeitlich korrelieren können.

Dabei gilt:

- CPU- und GPU-Zeitbasen MÜSSEN unterscheidbar sein.
- Unterschiedliche Zeitbasen SOLLEN kalibriert werden.
- Frame IDs MÜSSEN unterstützt werden.
- Operation IDs SOLLEN subsystemübergreifend verwendet werden.
- Asynchrone Ausführung MUSS berücksichtigt werden.
- Submission und tatsächliche GPU-Ausführung MÜSSEN getrennt dargestellt werden.
- Präsentationszeitpunkte MÜSSEN als bestätigt oder geschätzt gekennzeichnet werden.

Nicht ausreichend synchronisierte Zeitbasen DÜRFEN keine scheinbar exakten Zeitvergleiche erzeugen.

## 10. Bottleneck Analysis

NovaOS SOLL folgende Leistungsengpässe identifizieren können:

| Engpass | Mögliche Ursache |
|---|---|
| CPU Bound | Hoher Aufwand für Scene Updates oder Command Recording |
| GPU Bound | Hohe Shader- oder Render-Pass-Kosten |
| Memory Bound | Hohe Speicherbandbreitenauslastung |
| Synchronization Bound | CPU- oder GPU-Wartezeiten |
| Shader Compilation Stall | Laufzeitkompilierung benötigter Shader |
| Resource Allocation Stall | Verzögerungen bei Ressourcenbereitstellung |
| Composition Bound | Hohe Kosten der Fensterkomposition |
| Presentation Bound | Verzögerungen bei der Bildausgabe |

Die Analyse MUSS zwischen nachgewiesenen Engpässen und vermuteten Ursachen unterscheiden.

## 11. Profiling Reports

NovaOS SOLL Profiling-Ergebnisse strukturiert bereitstellen.

Ein Bericht SOLL enthalten:

- Untersuchte Sitzung und Messbedingungen.
- CPU- und GPU-Ausführungszeiten.
- Frame-Latenzen und Frame-Pacing.
- Kostenintensive Render-Passes.
- Speicherverbrauch und Transferkosten.
- Erkannte Synchronisationswartezeiten.
- Mögliche Leistungsengpässe.
- Messunsicherheiten und Datenverluste.

Berichte SOLLEN maschinenlesbar exportierbar sein.

## 12. Performance Comparison

Nova Graphics Profiling SOLL den Vergleich mehrerer Messungen ermöglichen.

Dabei gilt:

- Messbedingungen MÜSSEN dokumentiert werden.
- Unterschiedliche Hardwarekonfigurationen MÜSSEN erkennbar sein.
- Qualitätsprofile MÜSSEN berücksichtigt werden.
- Vergleichbare Kennzahlen MÜSSEN dieselben Definitionen verwenden.
- Unterschiede SOLLEN absolut und relativ dargestellt werden können.

Ein Vergleich DARF unterschiedliche Messbedingungen nicht als identisch behandeln.

## 13. Profiling Overhead

NovaOS MUSS den zusätzlichen Aufwand des Profilings begrenzen.

Dabei gilt:

- Profiling-Puffer MÜSSEN begrenzt sein.
- GPU-Zeitstempel SOLLEN asynchron ausgewertet werden.
- Hardware-Counter-Abfragen DÜRFEN Rendering-Aufgaben nicht unnötig blockieren.
- Sampling-Raten MÜSSEN konfigurierbar sein.
- Bei Überlast DÜRFEN optionale Ereignisse verworfen werden.
- Verlorene Ereignisse MÜSSEN erkennbar bleiben.

Der Messaufwand SOLL selbst als Kennzahl erfasst werden können.

## 14. Sicherheit und Isolation

Nova Graphics Profiling MUSS die Sicherheitsarchitektur von NovaOS berücksichtigen.

Dabei gilt:

- Profiling-Sitzungen benötigen geeignete Berechtigungen.
- Anwendungen DÜRFEN nicht unautorisiert andere Prozesse analysieren.
- Geschützte Oberflächeninhalte DÜRFEN nicht offengelegt werden.
- Hardware-Counter MÜSSEN über autorisierte Schnittstellen verwendet werden.
- Profiling-Daten MÜSSEN vor unautorisiertem Zugriff geschützt werden.

Die Profiling-Verwaltung SOLL außerhalb zeitkritischer Kernelpfade ausgeführt werden.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS detailliertes Graphics Profiling unterstützen.
2. Profiling MUSS auf Nova Graphics Performance Architecture aufbauen.
3. Eine redundante Messinfrastruktur DARF nicht entstehen.
4. Profiling-Sitzungen MÜSSEN kontrolliert verwaltbar sein.
5. CPU Profiling MUSS unterstützt werden.
6. GPU Profiling SOLL unterstützt werden, sofern Hardwarefunktionen verfügbar sind.
7. Frame Profiling MUSS unterstützt werden.
8. Render Graph Profiling SOLL unterstützt werden.
9. CPU- und GPU-Ereignisse MÜSSEN zeitlich korrelierbar sein.
10. Unterschiedliche Zeitbasen MÜSSEN korrekt behandelt werden.
11. Asynchrone Operationen MÜSSEN berücksichtigt werden.
12. Leistungsengpässe SOLLEN automatisch analysiert werden können.
13. Profiling-Ergebnisse SOLLEN exportierbar sein.
14. Messungen MÜSSEN ressourcenbegrenzt sein.
15. GPU-Messungen SOLLEN asynchron ausgewertet werden.
16. Messunsicherheiten und Datenverluste MÜSSEN erkennbar sein.
17. Profiling MUSS deaktivierbar sein.
18. Sicherheits- und Prozessisolationsgrenzen MÜSSEN eingehalten werden.
19. Die funktionale Rendering-Korrektheit MUSS unabhängig vom Profiling erhalten bleiben.

## 16. Ergebnis

Nova Graphics Profiling ermöglicht die detaillierte Untersuchung der gesamten NovaOS-Grafikpipeline.

Durch CPU- und GPU-Zeitmessungen, Frame-Timelines, Render-Pass-Analysen und subsystemübergreifende Ereigniskorrelation können Leistungsengpässe gezielt identifiziert werden.

Damit erhält NovaOS eine präzise Grundlage zur Bewertung und Weiterentwicklung seiner Rendering-, Compositor-, Display- und GPU-Optimierungen.