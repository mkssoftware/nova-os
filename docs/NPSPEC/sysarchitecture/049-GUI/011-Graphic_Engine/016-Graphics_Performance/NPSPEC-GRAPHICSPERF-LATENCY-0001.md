# NPSPEC-GRAPHICSPERF-LATENCY-0001 – Nova Graphics Latency Analysis

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics / Nova Graphics Performance

## 1. Zweck

Nova Graphics Latency Analysis definiert die Erfassung und Analyse zeitlicher Verzögerungen innerhalb der NovaOS-Grafikarchitektur.

Ziel ist die Identifikation unnötiger Wartezeiten, verzögerter Frame-Verarbeitung und ineffizienter Synchronisation zwischen CPU, GPU, Compositor und Display.

Die Analyse MUSS zwischen Verarbeitungsdauer, Wartezeit, Präsentationslatenz und vollständiger Eingabe-zu-Anzeige-Latenz unterscheiden.

## 2. Architekturprinzipien

- **End-to-End:** Analyse der gesamten Grafikverarbeitung.
- **Event-Driven:** Zeitmessungen basieren auf definierten Ereignissen.
- **Frame-Aware:** Verzögerungen werden einzelnen Frames zugeordnet.
- **Pipeline-Aware:** Einzelne Verarbeitungsphasen werden getrennt betrachtet.
- **Asynchronous:** Parallele CPU- und GPU-Ausführung wird berücksichtigt.
- **Presentation-Aware:** Tatsächliche Bildpräsentation wird berücksichtigt.
- **Low-Overhead:** Messungen verursachen möglichst geringen Zusatzaufwand.
- **Measurement-Driven:** Bewertungen basieren auf nachvollziehbaren Messwerten.

## 3. Architektur

Nova Graphics Latency Analysis verwendet Nova Graphics Performance Architecture, Nova Graphics Performance Metrics und Nova Graphics Profiling.

| Komponente | Verantwortung |
|---|---|
| Latency Metrics Collector | Erfassung zeitbezogener Kennzahlen |
| Frame Timeline Analyzer | Analyse der Frame-Verarbeitung |
| Pipeline Latency Analyzer | Bewertung einzelner Pipeline-Phasen |
| Synchronization Latency Analyzer | Analyse von Synchronisationswartezeiten |
| Presentation Latency Analyzer | Untersuchung der Bildpräsentation |
| Input-to-Display Analyzer | Analyse der Eingabe-zu-Anzeige-Latenz |
| Latency Feedback Provider | Bereitstellung der Analyseergebnisse |

Eine zusätzliche Frame-Scheduling- oder Präsentationsverwaltung DARF nicht entstehen.

## 4. Latency Data Model

Jede analysierte Latenzmessung MUSS folgende Informationen besitzen können:

| Eigenschaft | Beschreibung |
|---|---|
| Measurement ID | Eindeutige Messung |
| Frame ID | Zugehöriger Frame |
| Event ID | Auslösendes Ereignis |
| Subsystem ID | Verantwortliches Subsystem |
| Start Timestamp | Beginn des Messintervalls |
| End Timestamp | Ende des Messintervalls |
| Duration | Ermittelte Zeitdauer |
| Latency Category | Art der Verzögerung |
| Clock Domain | Verwendete Zeitbasis |
| Measurement Quality | Genauigkeit und Herkunft |
| Presentation ID | Zugehörige Präsentation, sofern vorhanden |

Start- und Endereignisse MÜSSEN für jede Latenzkennzahl eindeutig definiert sein.

## 5. Latency Categories

NovaOS MUSS folgende Latenzarten unterscheiden können:

| Kategorie | Beschreibung |
|---|---|
| Event Latency | Verzögerung bis zur Ereignisverarbeitung |
| Scene Update Latency | Dauer der Scene-Aktualisierung |
| Damage Analysis Latency | Dauer der Damage-Ermittlung |
| Render Preparation Latency | Vorbereitung der Rendering-Aufgaben |
| GPU Queue Latency | Wartezeit bis zur GPU-Ausführung |
| GPU Execution Latency | Dauer der GPU-Verarbeitung |
| Composition Latency | Dauer der Oberflächenkomposition |
| Surface Commit Latency | Verzögerung bei Surface-Commits |
| Presentation Latency | Verzögerung bis zur Bildpräsentation |
| Input-to-Display Latency | Zeit zwischen Eingabeereignis und sichtbarer Reaktion |

## 6. Frame Timeline Analysis

NovaOS MUSS den zeitlichen Ablauf einzelner Frames nachvollziehen können.

Dabei SOLLEN folgende Ereignisse erfasst werden:

1. Eingang eines relevanten Ereignisses.
2. Beginn der Zustandsverarbeitung.
3. Abschluss der Scene-Aktualisierung.
4. Abschluss der Damage-Analyse.
5. Abschluss der Render-Vorbereitung.
6. GPU Command Submission.
7. Beginn und Ende der GPU-Ausführung.
8. Abschluss der Komposition.
9. Surface Commit.
10. Bestätigte Bildpräsentation.

Nicht jeder Frame MUSS sämtliche Phasen durchlaufen.

Wiederverwendete Frames und Direct Scanout MÜSSEN entsprechend berücksichtigt werden.

## 7. CPU-GPU Latency Analysis

NovaOS MUSS CPU- und GPU-Verzögerungen getrennt analysieren können.

Dabei gilt:

- CPU-Verarbeitungszeit MUSS von GPU-Ausführungszeit unterscheidbar sein.
- GPU-Submission MUSS von tatsächlicher GPU-Ausführung getrennt werden.
- CPU-GPU-Synchronisationswartezeiten SOLLEN erfasst werden.
- Parallele Ausführung MUSS berücksichtigt werden.
- Unterschiedliche Zeitbasen MÜSSEN korrekt behandelt werden.

Überlappende CPU- und GPU-Zeiten DÜRFEN nicht unzulässig addiert werden.

## 8. Synchronization Latency Analysis

NovaOS SOLL synchronisationsbedingte Verzögerungen identifizieren.

Dazu gehören:

- CPU-Wartezeiten auf GPU-Fences.
- GPU-Wartezeiten auf Semaphores.
- Cross-Queue-Abhängigkeiten.
- Ressourcenbarrieren.
- Buffer-Verfügbarkeit.
- Surface-Synchronisation.
- Blockierende Ressourcenoperationen.

Die Analyse SOLL unnötige Serialisierung und vermeidbare Wartezeiten erkennen können.

## 9. Presentation Latency Analysis

NovaOS MUSS Verzögerungen bei der Bildpräsentation analysieren können.

Dabei gilt:

- Surface Commit und tatsächliche Präsentation MÜSSEN unterscheidbar sein.
- VSync-Wartezeiten SOLLEN berücksichtigt werden.
- Frame-Pacing-Abweichungen SOLLEN erfasst werden.
- Verworfene Frames MÜSSEN erkennbar sein.
- Wiederverwendete Bildinhalte MÜSSEN berücksichtigt werden.
- Direct Scanout und Hardware Overlays SOLLEN getrennt auswertbar sein.

Ein eingereichter Frame DARF nicht automatisch als präsentiert gelten.

## 10. Input-to-Display Latency

NovaOS SOLL die vollständige Eingabe-zu-Anzeige-Latenz analysieren können.

Diese umfasst:

- Eingang des Eingabeereignisses.
- Verarbeitung durch das Eingabesystem.
- Zustandsänderung der Anwendung.
- Scene- und Rendering-Aktualisierung.
- GPU-Verarbeitung.
- Komposition.
- Bildpräsentation.

Die Analyse MUSS zwischen bestätigten Zeitpunkten und geschätzten Anzeigezeitpunkten unterscheiden.

Die tatsächliche Sichtbarkeit auf dem Display DARF ohne geeignete Messgrundlage nicht als exakt gemessen ausgewiesen werden.

## 11. Frame Pacing Analysis

NovaOS SOLL zeitliche Unregelmäßigkeiten der Frame-Präsentation erkennen.

Relevante Kennzahlen sind:

- Frame Interval.
- Durchschnittliche Frame-Latenz.
- Minimale und maximale Frame-Latenz.
- P95- und P99-Latenz.
- Frame-Pacing-Abweichung.
- Anzahl verspäteter Frames.
- Anzahl verworfener Frames.

Die Analyse MUSS variable Bildwiederholraten berücksichtigen können.

## 12. Latency Bottleneck Detection

NovaOS SOLL folgende Verzögerungsursachen identifizieren:

| Engpass | Mögliche Ursache |
|---|---|
| CPU Latency | Langsame Zustands- oder Render-Vorbereitung |
| GPU Latency | Hohe GPU-Ausführungskosten |
| Queue Latency | Verzögerte GPU-Ausführung |
| Synchronization Latency | Blockierende Abhängigkeiten |
| Composition Latency | Aufwendige Oberflächenkomposition |
| Presentation Latency | Verzögerte Bildausgabe |
| Scheduling Latency | Ungünstige Frame-Planung |
| Resource Latency | Verzögerte Ressourcenbereitstellung |

Die Analyse MUSS zwischen nachgewiesenen Verzögerungen und vermuteten Ursachen unterscheiden.

## 13. Latency Feedback

Nova Graphics Latency Analysis MUSS Ergebnisse für folgende Systeme bereitstellen können:

- Nova Adaptive Frame Scheduling.
- Nova Frame Pacing.
- Nova Frame Latency Management.
- Nova Adaptive Rendering Strategy.
- Nova Render Graph Scheduling.
- Nova GPU Execution Scheduling.
- Nova Adaptive Rendering Quality.

Die jeweiligen Subsysteme bleiben für ihre Optimierungsentscheidungen verantwortlich.

## 14. Measurement Overhead

NovaOS MUSS den Aufwand der Latenzanalyse begrenzen.

Dabei gilt:

- Zeitmessungen SOLLEN vorhandene Performance-Ereignisse verwenden.
- GPU-Zeitstempel SOLLEN asynchron ausgewertet werden.
- Messpuffer MÜSSEN begrenzt sein.
- Detaillierte Messungen MÜSSEN deaktivierbar sein.
- Kritische Rendering-Pfade DÜRFEN nicht unnötig blockiert werden.
- Messunsicherheiten und verlorene Ereignisse MÜSSEN erkennbar bleiben.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS Grafiklatenzen analysieren können.
2. Bestehende Performance-Messsysteme MÜSSEN verwendet werden.
3. Latenzkennzahlen MÜSSEN eindeutig definierte Start- und Endereignisse besitzen.
4. Frame-bezogene Latenzen MÜSSEN unterstützt werden.
5. CPU- und GPU-Latenzen MÜSSEN unterscheidbar sein.
6. Asynchrone Ausführung MUSS korrekt berücksichtigt werden.
7. Synchronisationswartezeiten SOLLEN analysierbar sein.
8. Surface Commit und Bildpräsentation MÜSSEN unterscheidbar sein.
9. Input-to-Display-Latenz SOLL analysierbar sein.
10. Frame-Pacing-Abweichungen SOLLEN erfasst werden.
11. Unterschiedliche Zeitbasen MÜSSEN korrekt behandelt werden.
12. Nicht verfügbare oder geschätzte Messwerte MÜSSEN gekennzeichnet werden.
13. Latenzengpässe SOLLEN automatisch erkannt werden können.
14. Analyseergebnisse MÜSSEN bestehenden Optimierungssystemen bereitgestellt werden können.
15. Messungen MÜSSEN ressourcenbegrenzt sein.
16. Sicherheits- und Prozessisolationsgrenzen MÜSSEN eingehalten werden.
17. Die funktionale Rendering-Korrektheit MUSS unabhängig von der Latenzanalyse erhalten bleiben.

## 16. Ergebnis

Nova Graphics Latency Analysis ermöglicht die präzise Untersuchung zeitlicher Verzögerungen innerhalb der gesamten NovaOS-Grafikpipeline.

Durch die Analyse von CPU-, GPU-, Synchronisations-, Frame- und Präsentationslatenzen können unnötige Wartezeiten und ineffiziente Verarbeitungsschritte erkannt werden.

Damit erhält NovaOS eine messwertbasierte Grundlage zur Verbesserung der Reaktionsgeschwindigkeit, Frame-Stabilität und wahrgenommenen Flüssigkeit seiner grafischen Oberfläche.