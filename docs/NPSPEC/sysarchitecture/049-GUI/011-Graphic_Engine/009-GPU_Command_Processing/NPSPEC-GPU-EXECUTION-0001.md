# NPSPEC-GPU-EXECUTION-0001 – Nova GPU Execution Scheduling

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Command Processing

## 1. Zweck

Nova GPU Execution Scheduling definiert die Planung, Priorisierung und Koordination ausführbarer GPU-Arbeitslasten.

Ziel ist eine effiziente Nutzung der verfügbaren GPU-Ausführungskapazitäten bei möglichst geringer Latenz, kontrollierter Ressourcennutzung und fairer Verteilung zwischen Anwendungen.

Das Scheduling MUSS unabhängig von der konkreten GPU-Architektur funktionieren und deren tatsächliche Hardwarefähigkeiten berücksichtigen.

## 2. Architekturprinzipien

- **Dependency-Driven:** Ausführung erfolgt unter Einhaltung aller Abhängigkeiten.
- **Priority-Aware:** Zeitkritische GPU-Arbeit wird bevorzugt.
- **Deadline-Aware:** Präsentationszeitpunkte werden berücksichtigt.
- **Load-Aware:** GPU-Auslastung beeinflusst Scheduling-Entscheidungen.
- **Fair Scheduling:** Anwendungen erhalten kontrollierten Zugang zur GPU.
- **Asynchronous:** CPU und GPU arbeiten möglichst unabhängig.
- **Power-Aware:** Unnötige GPU-Aktivität wird vermieden.
- **Deterministic:** Logische Scheduling-Entscheidungen sind reproduzierbar.

## 3. Execution Model

NovaOS verwaltet GPU-Arbeit als ausführbare Execution Jobs.

| Eigenschaft | Beschreibung |
|---|---|
| Job ID | Eindeutige Identifikation |
| Submission ID | Zugehörige GPU Submission |
| Queue ID | Vorgesehene GPU-Warteschlange |
| Priority | Ausführungspriorität |
| Deadline | Optionaler Zielzeitpunkt |
| Dependencies | Erforderliche Vorgängeroperationen |
| Estimated Cost | Geschätzter Ausführungsaufwand |
| Execution State | Aktueller Verarbeitungszustand |
| Owner | Zugehöriger Prozess oder Systemdienst |

Execution Jobs MÜSSEN eindeutig nachverfolgbar sein.

## 4. Execution Lifecycle

NovaOS definiert folgende Zustände:

`Created → Waiting → Ready → Submitted → Running → Completed`

Zusätzliche Zustände:

- **Failed:** Ausführung fehlgeschlagen.
- **Cancelled:** Noch nicht eingereichter Auftrag wurde verworfen.
- **Unknown:** Hardwareausführung kann nicht zuverlässig bestimmt werden.

`Running` DARF nur verwendet werden, wenn das Backend den tatsächlichen Ausführungsbeginn feststellen kann.

Andernfalls bleibt der Auftrag bis zur Abschlussbestätigung im Zustand `Submitted`.

## 5. Scheduling Priorities

NovaOS SOLL folgende Prioritätsklassen unterstützen:

| Priorität | Verwendung |
|---|---|
| Critical | Systemkritische GPU-Operationen |
| Interactive | Direkte Benutzerinteraktionen |
| Presentation | Zeitkritische Frame-Verarbeitung |
| Normal | Reguläre GPU-Arbeit |
| Background | Nicht zeitkritische Berechnungen |
| Low Power | Energieoptimierte Hintergrundverarbeitung |

Prioritäten MÜSSEN durch Systemrichtlinien kontrolliert werden.

Anwendungen DÜRFEN sich nicht eigenständig unbeschränkte Systemprioritäten zuweisen.

## 6. Dependency-Aware Scheduling

GPU-Arbeit DARF nur unter Einhaltung ihrer Ausführungsabhängigkeiten geplant werden.

Dabei gilt:

1. Abhängigkeiten werden vor der Submission geprüft.
2. Unabhängige Aufträge können parallel geplant werden.
3. Ressourcenbarrieren werden berücksichtigt.
4. Queueübergreifende Synchronisation wird eingehalten.
5. Zirkuläre Warteabhängigkeiten MÜSSEN verhindert oder erkannt werden.

Die Scheduling-Reihenfolge DARF erforderliche GPU-Synchronisation nicht ersetzen.

## 7. Deadline-Aware Scheduling

Nova GPU Execution Scheduling MUSS mit Nova Frame Scheduling zusammenarbeiten.

Dabei gilt:

- Frame-Deadlines werden berücksichtigt.
- Zeitkritische Rendering-Aufträge SOLLEN bevorzugt werden.
- Geschätzte GPU-Ausführungszeiten fließen in die Planung ein.
- Hintergrundaufträge DÜRFEN zurückgestellt werden.
- Bereits überfällige Aufträge werden anhand ihres aktuellen Nutzens bewertet.

Eine Deadline ist ein Scheduling-Ziel und keine Garantie für einen bestimmten Hardware-Ausführungszeitpunkt.

## 8. Queue und Hardware Integration

Nova GPU Execution Scheduling MUSS mit Nova GPU Queue Management zusammenarbeiten.

Dabei gilt:

- Aufträge werden kompatiblen Queues zugeordnet.
- Verfügbare Queue-Fähigkeiten werden berücksichtigt.
- Unabhängige Graphics-, Compute- und Transferaufträge können parallel geplant werden.
- Gemeinsame Hardware-Ressourcen werden berücksichtigt.
- Hardwareparallelität DARF nicht vorausgesetzt werden.

Die tatsächliche Ausführungsreihenfolge innerhalb der GPU wird durch das Backend und die Hardware bestimmt.

## 9. Fairness und Prozessisolation

NovaOS MUSS eine faire Verteilung der GPU-Ausführungskapazitäten ermöglichen.

Dabei gilt:

- GPU-Aufträge werden ihren Besitzern zugeordnet.
- Dauerhafte Verdrängung niedriger priorisierter Prozesse SOLL verhindert werden.
- Prozessbezogene Ressourcenbudgets SOLLEN unterstützt werden.
- Hintergrundberechnungen DÜRFEN begrenzt werden.
- Systemkritische Operationen behalten notwendige Prioritäten.

Die Fairness-Steuerung MUSS berücksichtigen, dass nicht jede GPU laufende Aufträge unterbrechen kann.

## 10. Adaptive Execution Scheduling

NovaOS SOLL Scheduling-Entscheidungen anhand aktueller Betriebsbedingungen anpassen.

Relevante Faktoren:

- GPU-Auslastung
- Queue-Auslastung
- Geschätzte Ausführungsdauer
- Frame-Latenz
- Ausstehende Submissions
- Speicherbandbreite
- Energiezustand
- Hardwarefähigkeiten

Häufige unnötige Strategiewechsel SOLLEN durch Hysteresis vermieden werden.

## 11. GPU Preemption

NovaOS SOLL Hardware-Preemption nutzen können, sofern diese unterstützt wird.

Dabei gilt:

- Unterstützte Preemption-Granularität wird vom Backend gemeldet.
- Laufende GPU-Aufträge dürfen nur entsprechend den Hardwarefähigkeiten unterbrochen werden.
- Nicht unterbrechbare Aufträge werden bei der Planung berücksichtigt.
- Zeitkritische Arbeit SOLL nicht unnötig hinter lang laufenden Aufträgen blockiert werden.

Softwareseitiges Scheduling DARF keine beliebige Unterbrechbarkeit von GPU-Befehlen voraussetzen.

## 12. Power-Aware Execution

Nova GPU Execution Scheduling SOLL energieeffiziente Ausführungsstrategien unterstützen.

Dazu gehören:

- Zusammenfassung geeigneter GPU-Aufträge.
- Vermeidung unnötiger GPU-Aktivierungen.
- Reduzierung leerer Submissions.
- Zurückstellung nicht dringender Arbeit.
- Unterstützung längerer GPU-Idle-Phasen.

Energieoptimierung DARF notwendige Frame-Deadlines und Systemfunktionen nicht unzulässig beeinträchtigen.

## 13. Monitoring und Feedback

NovaOS SOLL GPU-Ausführungsdaten erfassen können.

Dazu gehören:

- Submission-Zeitpunkt
- Abschlusszeitpunkt
- Queue-Wartezeit, soweit messbar
- GPU-Ausführungsdauer, soweit messbar
- Deadline-Verletzungen
- Queue-Auslastung
- Fehlgeschlagene Aufträge

Gemessene und geschätzte Werte MÜSSEN unterscheidbar bleiben.

Die Daten SOLLEN zur Verbesserung zukünftiger Scheduling-Entscheidungen verwendet werden.

## 14. Fehlerbehandlung

NovaOS MUSS GPU-Ausführungsfehler kontrolliert behandeln.

Dazu gehören:

- Fehlgeschlagene Submissions
- Ungültige Abhängigkeiten
- GPU-Timeouts
- Queue-Fehler
- Geräteverlust
- Unbekannte Ausführungszustände

Dabei gilt:

- Fehlgeschlagene Aufträge DÜRFEN nicht als erfolgreich abgeschlossen gelten.
- Ressourcen MÜSSEN bis zur sicheren Freigabe geschützt bleiben.
- Abhängige Aufträge MÜSSEN kontrolliert behandelt werden.
- Ein konservativer Scheduling-Fallback MUSS verfügbar sein.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS GPU Execution Scheduling bereitstellen.
2. GPU-Aufträge MÜSSEN eindeutig identifizierbar sein.
3. Ausführungsabhängigkeiten MÜSSEN eingehalten werden.
4. Scheduling-Prioritäten MÜSSEN unterstützt werden.
5. Frame-Deadlines MÜSSEN berücksichtigt werden.
6. Queue-Fähigkeiten MÜSSEN vor der Zuordnung geprüft werden.
7. Unabhängige GPU-Aufträge SOLLEN parallel geplant werden können.
8. Hardwareparallelität DARF nicht vorausgesetzt werden.
9. Prozessübergreifende Fairness MUSS berücksichtigt werden.
10. Dauerhafte Verdrängung einzelner Anwendungen SOLL verhindert werden.
11. Hardware-Preemption SOLL genutzt werden, sofern verfügbar.
12. GPU-Ausführungszeiten SOLLEN erfasst und ausgewertet werden.
13. Scheduling-Entscheidungen SOLLEN adaptiv erfolgen.
14. Unnötige GPU-Aktivität SOLL vermieden werden.
15. GPU Synchronization MUSS integriert sein.
16. Fehlerhafte GPU-Aufträge MÜSSEN kontrolliert behandelt werden.
17. Ein sicherer Scheduling-Fallback MUSS verfügbar sein.
18. Im deterministischen Modus MÜSSEN logische Scheduling-Entscheidungen bei identischen Eingaben reproduzierbar sein; die physische Ausführungsreihenfolge und Laufzeit der GPU werden dadurch nicht garantiert.

## 16. Ergebnis

Nova GPU Execution Scheduling ermöglicht die kontrollierte Planung und Priorisierung sämtlicher GPU-Arbeitslasten innerhalb von NovaOS.

Durch Dependency-Aware Scheduling, Frame-Deadlines, faire Ressourcenverteilung und adaptive Hardwareauslastung werden GPU-Aufträge effizient koordiniert.

Damit erhält NovaOS eine skalierbare GPU-Ausführungssteuerung, die interaktive Grafikoperationen bevorzugt, parallele Hardwarefähigkeiten nutzt und gleichzeitig Energieverbrauch sowie Ressourcenbelastung berücksichtigt.