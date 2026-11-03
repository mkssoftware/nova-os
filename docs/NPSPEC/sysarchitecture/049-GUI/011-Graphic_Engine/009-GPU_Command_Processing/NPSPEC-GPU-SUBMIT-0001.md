# NPSPEC-GPU-SUBMIT-0001 – Nova GPU Command Submission

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Command Processing

## 1. Zweck

Nova GPU Command Submission definiert die kontrollierte Übergabe aufgezeichneter GPU-Befehle an die GPU-Ausführungseinheiten.

Ziel ist eine effiziente, asynchrone und sichere Befehlsübermittlung mit minimalem CPU-Overhead.

Die Architektur trennt die erfolgreiche Annahme einer Submission vom tatsächlichen Abschluss ihrer GPU-Ausführung.

## 2. Architekturprinzipien

- **Asynchronous:** CPU und GPU können unabhängig arbeiten.
- **Queue-Aware:** Submissions berücksichtigen verfügbare GPU-Queues.
- **Dependency-Aware:** Ausführungsabhängigkeiten werden eingehalten.
- **Batch-Oriented:** Geeignete Command Buffers können gemeinsam eingereicht werden.
- **Non-Blocking:** Unnötige CPU-Wartezeiten werden vermieden.
- **Resource-Safe:** Ressourcen bleiben während ihrer Verwendung gültig.
- **Priority-Aware:** Zeitkritische Operationen werden bevorzugt.
- **Deterministic:** Reproduzierbare logische Submission-Planung.

## 3. Submission Model

Eine GPU Submission beschreibt eine zusammenhängende Übergabe ausführbarer GPU-Arbeit.

| Eigenschaft | Beschreibung |
|---|---|
| Submission ID | Eindeutige Identifikation |
| Queue ID | Zielwarteschlange |
| Command Buffers | Auszuführende Befehlssequenzen |
| Dependencies | Erforderliche Vorgängeroperationen |
| Wait Synchronization | Synchronisationsbedingungen vor der Ausführung |
| Signal Synchronization | Signale nach definierten Ausführungspunkten |
| Priority | Scheduling-Priorität |
| Submission State | Aktueller Verarbeitungszustand |

Jede Submission MUSS eindeutig nachverfolgbar sein.

## 4. Submission Lifecycle

NovaOS definiert folgende Zustände:

`Created → Validated → Queued → Submitted → Completed`

Zusätzliche Zustände:

- **Waiting:** Abhängigkeiten sind noch nicht erfüllt.
- **Failed:** Submission oder Ausführung fehlgeschlagen.
- **Cancelled:** Noch nicht übergebene Submission wurde verworfen.
- **Unknown:** Ausführungszustand kann nicht zuverlässig bestimmt werden.

Dabei gilt:

1. Nur validierte Submissions dürfen an das GPU-Backend übergeben werden.
2. Eine erfolgreiche Submission bedeutet nicht den Abschluss der GPU-Ausführung.
3. Der Abschluss MUSS durch geeignete Synchronisationsmechanismen festgestellt werden.
4. Fehlgeschlagene Submissions DÜRFEN nicht als erfolgreich abgeschlossen gelten.
5. Bereits an die Hardware übergebene Arbeit DARF nicht ohne entsprechende Backend-Unterstützung als abgebrochen betrachtet werden.

## 5. Submission Validation

Vor der Übergabe MUSS NovaOS die Submission validieren.

Geprüft werden:

- Gültigkeit der Command Buffers.
- Kompatibilität mit der Ziel-Queue.
- Gültigkeit referenzierter Ressourcen.
- Erforderliche Zugriffsberechtigungen.
- Ressourcenabhängigkeiten.
- Synchronisationsbedingungen.
- Zulässige Command-Buffer-Zustände.

Ungültige Submissions MÜSSEN vor der Hardwareübergabe zurückgewiesen werden.

## 6. Asynchronous Submission

NovaOS SOLL GPU-Arbeit ohne blockierendes Warten auf deren Abschluss einreichen.

Dabei gilt:

- Die CPU kann nach erfolgreicher Submission weitere Aufgaben ausführen.
- GPU-Abschlüsse werden über Fences, Semaphores oder geeignete Ereignisse verfolgt.
- CPU-seitiges Busy Waiting SOLL vermieden werden.
- Mehrere Submissions dürfen gleichzeitig ausstehen.
- Die Anzahl ausstehender Submissions MUSS begrenzbar sein.

Asynchrone Submission DARF erforderliche Ressourcen- und Speicherabhängigkeiten nicht umgehen.

## 7. Submission Batching

NovaOS SOLL geeignete Command Buffers gemeinsam einreichen können.

Ziele:

- Reduzierung der Anzahl von Treiberaufrufen.
- Verringerung des CPU-Overheads.
- Effizientere GPU-Auslastung.
- Reduzierung unnötiger Synchronisationsoperationen.

Batching MUSS die Ausführungsabhängigkeiten erhalten.

Zeitkritische Submissions DÜRFEN nicht unangemessen verzögert werden, nur um größere Batches zu erzeugen.

## 8. Queue Integration

Nova GPU Command Submission MUSS mit Nova GPU Queue Management zusammenarbeiten.

Dabei gilt:

- Jede Submission wird einer kompatiblen Queue zugeordnet.
- Queue-Prioritäten werden berücksichtigt.
- Queueübergreifende Abhängigkeiten werden synchronisiert.
- Unabhängige Submissions können parallel verarbeitet werden.
- Die tatsächliche Hardwareparallelität wird vom GPU-Backend bestimmt.

Eine Submission DARF keine nicht unterstützten Queue-Fähigkeiten voraussetzen.

## 9. Synchronization

Nova GPU Command Submission MUSS Nova GPU Synchronization verwenden.

Unterstützt werden sollen:

- GPU Fences
- GPU Semaphores
- Timeline Synchronization
- Resource Barriers
- Cross-Queue Dependencies

Dabei gilt:

1. Erforderliche Wait-Bedingungen werden vor der betroffenen GPU-Arbeit berücksichtigt.
2. Signal-Bedingungen werden erst am definierten Synchronisationspunkt erfüllt.
3. Ressourcenabhängigkeiten MÜSSEN korrekt synchronisiert werden.
4. Zirkuläre Warteabhängigkeiten MÜSSEN verhindert oder erkannt werden.
5. Fehlgeschlagene Operationen DÜRFEN keine erfolgreichen Abschlusssignale erzeugen.

## 10. Resource Lifetime

Referenzierte GPU-Ressourcen MÜSSEN während ihrer gesamten Verwendung gültig bleiben.

Dabei gilt:

- Command Buffers bleiben bis zum zulässigen Abschluss geschützt.
- GPU-Buffer und Texturen dürfen nicht vorzeitig freigegeben werden.
- Temporäre Ressourcen werden erst nach Abschluss ihrer letzten Verwendung wiederverwendet.
- Mehrere ausstehende Submissions werden berücksichtigt.
- Fehlgeschlagene oder unbekannte Ausführungszustände erfordern eine sichere Ressourcenbehandlung.

Eine erfolgreiche Submission allein berechtigt nicht zur Ressourcenfreigabe.

## 11. Frame Scheduling Integration

Nova GPU Command Submission MUSS mit Nova Frame Scheduling zusammenarbeiten.

Dabei gilt:

- Zeitkritische Rendering-Submissions werden bevorzugt.
- Frame-Deadlines werden berücksichtigt.
- Unnötige Submissions werden vermieden.
- Veraltete, noch nicht eingereichte Frame-Arbeit DARF verworfen werden.
- Bereits eingereichte GPU-Arbeit bleibt bis zu ihrem tatsächlichen Abschluss nachverfolgbar.

Das Frame Scheduling bestimmt den zeitlichen Bedarf.

GPU Command Submission übernimmt die kontrollierte Hardwareübergabe.

## 12. Render Graph Integration

Nova GPU Command Submission MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Ausführbare Render Passes werden in Command Buffers überführt.
- Pass-Abhängigkeiten bestimmen die Submission-Reihenfolge.
- Unabhängige Passes können gemeinsam oder parallel eingereicht werden.
- Ressourcenbarrieren werden berücksichtigt.
- Nicht benötigte Render Passes erzeugen keine Submissions.

Die Submission-Planung MUSS die vom Render Graph definierten Abhängigkeiten erhalten.

## 13. Submission Backpressure

NovaOS MUSS eine Überlastung der GPU-Submission-Pipeline begrenzen können.

Dazu gehören:

- Begrenzung ausstehender Submissions.
- Überwachung der Queue-Auslastung.
- Zusammenfassung geeigneter Aufträge.
- Zurückstellung nicht dringender Arbeit.
- Vermeidung unbegrenzten Speicherwachstums.

Backpressure DARF erforderliche grafische Zustandsänderungen nicht verlieren.

Bereits akzeptierte GPU-Arbeit MUSS entsprechend ihrem tatsächlichen Ausführungszustand verwaltet werden.

## 14. Sicherheit und Isolation

GPU Submissions MÜSSEN die NovaOS-Sicherheitsarchitektur einhalten.

Dabei gilt:

- Anwendungen dürfen ausschließlich autorisierte Ressourcen verwenden.
- Privilegierte Hardwareübergaben erfolgen über den GPU-Treiber.
- Prozessbezogene Submissions müssen eindeutig zugeordnet werden können.
- Ressourcen anderer Prozesse dürfen nicht unautorisiert referenziert werden.
- Fehlerhafte Anwendungen dürfen die Submission-Verwaltung nicht unkontrolliert beeinträchtigen.

Der GPU-Treiber MUSS die maßgebliche Validierung an der Vertrauensgrenze durchführen.

## 15. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Command Buffers.
- Fehlende oder ungültige Ressourcen.
- Inkompatible GPU-Queues.
- Fehlgeschlagene Submission.
- Synchronisationsfehler.
- GPU-Timeouts.
- Geräteverlust.

Dabei gilt:

- Fehlgeschlagene Submissions MÜSSEN eindeutig gekennzeichnet werden.
- Ausführungsabschlüsse DÜRFEN nicht vorgetäuscht werden.
- Ressourcen MÜSSEN bis zur sicheren Freigabe geschützt bleiben.
- Bei Geräteverlust MÜSSEN betroffene Submissions kontrolliert invalidiert werden.
- Ein geeigneter Recovery- oder Fallback-Pfad MUSS verfügbar sein.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS GPU Command Submission unterstützen.
2. Command Recording und Submission MÜSSEN getrennt sein.
3. Jede Submission MUSS eindeutig identifizierbar sein.
4. Submissions MÜSSEN vor der Hardwareübergabe validiert werden.
5. Asynchrone Submission SOLL unterstützt werden.
6. Erfolgreiche Submission und GPU-Ausführungsabschluss MÜSSEN unterschieden werden.
7. Mehrere Command Buffers SOLLEN gemeinsam eingereicht werden können.
8. Queue-Fähigkeiten MÜSSEN berücksichtigt werden.
9. Ressourcenabhängigkeiten MÜSSEN eingehalten werden.
10. GPU Synchronization MUSS integriert sein.
11. Referenzierte Ressourcen MÜSSEN bis zum zulässigen Abschluss gültig bleiben.
12. Zeitkritische Frame-Submissions SOLLEN priorisiert werden.
13. Unnötige GPU-Submissions SOLLEN vermieden werden.
14. Die Anzahl ausstehender Submissions MUSS begrenzbar sein.
15. CPU-seitiges Busy Waiting SOLL vermieden werden.
16. Prozessbezogene GPU-Submissions MÜSSEN sicher isoliert werden.
17. Fehlerhafte Submissions MÜSSEN kontrolliert behandelt werden.
18. Im deterministischen Modus MÜSSEN Submission-Planung und logische Abhängigkeiten bei identischen Eingaben reproduzierbar sein.

## 17. Ergebnis

Nova GPU Command Submission bildet die kontrollierte Übergabeschicht zwischen aufgezeichneten GPU-Befehlen und ihrer tatsächlichen Hardwareausführung.

Durch asynchrone Submission, Batching, Queue-Integration, Synchronisation und Backpressure werden CPU-Overhead und unnötige Wartezeiten reduziert.

Damit erhält NovaOS eine effiziente, sichere und skalierbare Grundlage für die GPU-Ausführung, die sowohl interaktive Grafikoperationen als auch parallele Compute- und Transferaufträge unterstützt.