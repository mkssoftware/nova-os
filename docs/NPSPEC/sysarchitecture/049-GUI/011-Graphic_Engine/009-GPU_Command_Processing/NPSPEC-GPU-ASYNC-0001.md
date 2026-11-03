# NPSPEC-GPU-ASYNC-0001 – Nova Asynchronous Compute

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Command Processing

## 1. Zweck

Nova Asynchronous Compute definiert die asynchrone Ausführung von GPU-Compute-Operationen innerhalb der NovaOS-Grafikarchitektur.

Ziel ist die bessere Auslastung verfügbarer GPU-Ressourcen durch die zeitliche Überlappung unabhängiger Graphics-, Compute- und Transferoperationen.

Die Architektur MUSS auch auf GPUs funktionieren, die keine parallele Ausführung unterschiedlicher Queue-Typen unterstützen.

## 2. Architekturprinzipien

- **Asynchronous:** CPU und GPU arbeiten ohne unnötige gegenseitige Blockierung.
- **Concurrent:** Unabhängige GPU-Operationen können gleichzeitig ausgeführt werden.
- **Dependency-Aware:** Ressourcen- und Ausführungsabhängigkeiten bleiben erhalten.
- **Queue-Aware:** Verfügbare Hardware-Queues werden berücksichtigt.
- **Latency-Aware:** Zeitkritische Grafikoperationen werden bevorzugt.
- **Adaptive:** Asynchrone Ausführung erfolgt nur bei geeignetem Nutzen.
- **Resource-Safe:** Gemeinsame Ressourcen werden korrekt synchronisiert.
- **Deterministic:** Logische Abhängigkeiten und Ergebnisse bleiben reproduzierbar, soweit die verwendeten Compute-Operationen deterministisch sind.

## 3. Async Compute Model

NovaOS unterscheidet folgende Compute-Kategorien:

| Kategorie | Beschreibung |
|---|---|
| Graphics Compute | Compute-Operationen innerhalb der Rendering-Pipeline |
| Independent Compute | Von der Frame-Erzeugung unabhängige Berechnungen |
| Background Compute | Nicht zeitkritische GPU-Berechnungen |
| Transfer-Dependent Compute | Berechnungen mit erforderlichen Datentransfers |

Ein Compute-Auftrag besitzt mindestens:

- Compute Task ID
- Pipeline Reference
- Resource References
- Execution Dependencies
- Queue Requirements
- Priority
- Completion Synchronization

Compute-Aufträge MÜSSEN unabhängig von ihrer tatsächlichen Hardwareausführung beschrieben werden können.

## 4. Queue Integration

Nova Asynchronous Compute MUSS mit Nova GPU Queue Management zusammenarbeiten.

Dabei gilt:

1. Compute-Aufträge werden kompatiblen GPU-Queues zugeordnet.
2. Dedizierte Compute-Queues SOLLEN genutzt werden, wenn dies vorteilhaft ist.
3. Graphics-Queues DÜRFEN Compute-Aufträge ausführen, sofern unterstützt.
4. Mehrere logische Queues DÜRFEN dieselbe physische Queue verwenden.
5. Hardwareparallelität DARF nicht vorausgesetzt werden.

Eine separate Compute-Queue garantiert keine gleichzeitige Ausführung mit Graphics-Operationen.

## 5. Concurrent Execution

Unabhängige Compute- und Graphics-Aufträge SOLLEN zeitlich überlappend ausgeführt werden können.

Voraussetzungen:

- Kompatible GPU-Hardware.
- Ausreichende Ausführungsressourcen.
- Gültige Ressourcenabhängigkeiten.
- Korrekte Synchronisation.
- Keine unzulässigen Speicherzugriffskonflikte.

Die parallele Ausführung DARF nicht erzwungen werden, wenn dadurch höhere Synchronisationskosten oder eine Verschlechterung der Frame-Latenz entstehen.

## 6. Resource Dependencies

Nova Asynchronous Compute MUSS Ressourcenabhängigkeiten berücksichtigen.

Insbesondere:

- Read-after-Write
- Write-after-Read
- Write-after-Write
- Queue Ownership Transfers
- Speicher- und Cache-Sichtbarkeit

Unabhängige Ressourcen SOLLEN ohne unnötige Synchronisation verarbeitet werden.

Gemeinsam verwendete Ressourcen MÜSSEN vor konkurrierenden, nicht synchronisierten Zugriffen geschützt werden.

## 7. Synchronization

Nova Asynchronous Compute MUSS mit Nova GPU Synchronization zusammenarbeiten.

Unterstützt werden sollen:

- GPU Fences
- GPU Semaphores
- Timeline Synchronization
- Resource Barriers
- Cross-Queue Synchronization

Dabei gilt:

- GPU-seitige Synchronisation SOLL bevorzugt werden.
- Unnötiges CPU-seitiges Warten SOLL vermieden werden.
- Abhängige Aufträge dürfen erst nach Erfüllung ihrer Voraussetzungen ausgeführt werden.
- Zirkuläre Warteabhängigkeiten MÜSSEN verhindert oder erkannt werden.
- Fehlgeschlagene Aufträge DÜRFEN keine erfolgreichen Abschlusssignale erzeugen.

## 8. Render Graph Integration

Nova Asynchronous Compute MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Compute Passes werden als eigenständige Render-Graph-Operationen modelliert.
- Pass-Abhängigkeiten bestimmen die zulässige Ausführung.
- Unabhängige Compute Passes können parallel zu Graphics Passes geplant werden.
- Ressourcenübergänge werden automatisch berücksichtigt.
- Nicht benötigte Compute Passes erzeugen keine GPU-Arbeit.

Die Entscheidung über eine asynchrone Ausführung erfolgt anhand der verfügbaren Hardwarefähigkeiten und erwarteten Ausführungskosten.

## 9. Frame Scheduling Integration

Nova Asynchronous Compute MUSS die Anforderungen des Nova Frame Schedulers berücksichtigen.

Dabei gilt:

- Zeitkritische Rendering-Aufträge SOLLEN bevorzugt werden.
- Compute-Aufträge DÜRFEN erforderliche Frame-Deadlines nicht unnötig gefährden.
- Hintergrundberechnungen können zurückgestellt werden.
- Die GPU-Auslastung SOLL bei Scheduling-Entscheidungen berücksichtigt werden.
- Unnötige GPU-Aktivierungen SOLLEN vermieden werden.

Eine hohe GPU-Auslastung ist kein eigenständiges Optimierungsziel, wenn dadurch Latenz oder Energieeffizienz verschlechtert werden.

## 10. Adaptive Async Compute

NovaOS SOLL die Nutzung asynchroner Compute-Ausführung dynamisch bewerten.

Relevante Faktoren:

- GPU-Auslastung
- Verfügbare Hardware-Queues
- Compute-Ausführungsdauer
- Synchronisationskosten
- Speicherbandbreite
- Ressourcenabhängigkeiten
- Frame-Latenz
- Energieverbrauch

Bei ungünstigen Bedingungen DARF NovaOS Compute-Aufträge seriell ausführen.

Häufige unnötige Strategiewechsel SOLLEN vermieden werden.

## 11. Power Management

Nova Asynchronous Compute SOLL energieeffiziente Ausführungsstrategien unterstützen.

Dazu gehören:

- Zusammenfassung geeigneter Compute-Aufträge.
- Vermeidung unnötiger Queue-Aktivierungen.
- Reduzierung überflüssiger Speichertransfers.
- Nutzung vorhandener GPU-Aktivitätsphasen.
- Unterstützung geeigneter Idle-Zustände.

Asynchrone Ausführung DARF nicht grundsätzlich als energieeffizienter betrachtet werden als serielle Verarbeitung.

## 12. Sicherheit und Isolation

Compute-Aufträge MÜSSEN die NovaOS-Sicherheitsarchitektur einhalten.

Dabei gilt:

- GPU-Ressourcen MÜSSEN capability-basiert autorisiert werden.
- Anwendungen DÜRFEN keine unzulässigen GPU-Speicherbereiche verwenden.
- Prozessübergreifende Ressourcenfreigaben MÜSSEN explizit erfolgen.
- Compute-Aufträge MÜSSEN über kontrollierte Treiberschnittstellen eingereicht werden.
- Ressourcen unterschiedlicher Prozesse MÜSSEN entsprechend ihren Berechtigungen isoliert bleiben.

Die GPU-Ausführung DARF keine Umgehung der NovaOS-Prozessisolation ermöglichen.

## 13. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Nicht unterstützte Compute-Funktionen.
- Fehlende Hardware-Queues.
- Ungültige Ressourcenabhängigkeiten.
- Synchronisationsfehler.
- Fehlgeschlagene Compute-Submissions.
- GPU-Timeouts.
- Geräteverlust.

Dabei gilt:

- Fehlgeschlagene Compute-Aufträge DÜRFEN nicht als erfolgreich abgeschlossen gelten.
- Betroffene Ressourcen MÜSSEN kontrolliert behandelt werden.
- Ein serieller GPU-Fallback SOLL verfügbar sein.
- Ein Software-Fallback DARF für unterstützte Compute-Operationen bereitgestellt werden.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS asynchrone GPU-Compute-Aufträge unterstützen können.
2. Compute-Aufträge MÜSSEN unabhängig von ihrer Hardwareausführung beschrieben werden.
3. GPU Queue Management MUSS integriert sein.
4. Dedizierte Compute-Queues SOLLEN genutzt werden, sofern vorteilhaft.
5. Hardwareparallelität DARF nicht vorausgesetzt werden.
6. Unabhängige Graphics- und Compute-Aufträge SOLLEN überlappend ausführbar sein.
7. Ressourcenabhängigkeiten MÜSSEN korrekt berücksichtigt werden.
8. Cross-Queue Synchronization MUSS unterstützt werden.
9. CPU-seitiges Busy Waiting SOLL vermieden werden.
10. Render Graph MUSS Compute Passes integrieren können.
11. Frame-Deadlines MÜSSEN berücksichtigt werden.
12. Hintergrundberechnungen DÜRFEN niedriger priorisiert werden.
13. Asynchrone Ausführung SOLL adaptiv aktiviert werden.
14. Unnötige Synchronisations- und Speichertransfers SOLLEN vermieden werden.
15. Compute-Ressourcen MÜSSEN capability-basiert geschützt werden.
16. Fehlerhafte Compute-Aufträge MÜSSEN kontrolliert behandelt werden.
17. Ein serieller GPU-Ausführungspfad MUSS verfügbar sein.
18. Im deterministischen Modus MÜSSEN logische Abhängigkeiten und Scheduling-Entscheidungen reproduzierbar sein; nichtdeterministische GPU-Berechnungen benötigen zusätzliche Einschränkungen oder einen deterministischen Ausführungspfad.

## 15. Ergebnis

Nova Asynchronous Compute ermöglicht die effiziente Nutzung unabhängiger GPU-Ausführungseinheiten für Graphics-, Compute- und Transferoperationen.

Durch explizite Ressourcenabhängigkeiten, Queue-Integration, GPU-seitige Synchronisation und adaptive Ausführungsplanung können Berechnungen zeitlich überlappend verarbeitet werden.

Damit erhält NovaOS eine skalierbare Compute-Architektur, die moderne GPU-Fähigkeiten nutzt, ohne auf parallele Hardwareausführung angewiesen zu sein.