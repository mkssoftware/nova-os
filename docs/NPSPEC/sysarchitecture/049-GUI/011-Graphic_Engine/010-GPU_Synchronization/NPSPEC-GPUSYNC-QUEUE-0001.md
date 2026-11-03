# NPSPEC-GPUSYNC-QUEUE-0001 – Nova Cross-Queue Synchronization

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Synchronization

## 1. Zweck

Nova Cross-Queue Synchronization definiert die Synchronisation zwischen unterschiedlichen GPU-Warteschlangen.

Ziel ist die sichere parallele Verarbeitung von Graphics-, Compute- und Transferoperationen, ohne unnötige GPU-Stalls oder CPU-Blockierungen zu verursachen.

Die Architektur MUSS sowohl GPUs mit mehreren unabhängigen Hardware-Queues als auch GPUs mit nur einer physischen Queue unterstützen.

## 2. Architekturprinzipien

- **Dependency-Driven:** Synchronisation erfolgt anhand tatsächlicher Abhängigkeiten.
- **Asynchronous:** Queueübergreifende Operationen benötigen möglichst keine CPU-Blockierung.
- **Parallel:** Unabhängige GPU-Arbeit bleibt parallel ausführbar.
- **Resource-Safe:** Gemeinsame Ressourcen werden korrekt synchronisiert.
- **Ownership-Aware:** Erforderliche Ressourcenübergaben werden berücksichtigt.
- **Minimal Synchronization:** Unnötige Warteoperationen werden vermieden.
- **Backend-Independent:** Einheitliches Modell für unterschiedliche GPU-Treiber.
- **Deterministic:** Logische Ausführungsabhängigkeiten bleiben reproduzierbar.

## 3. Cross-Queue Model

NovaOS unterscheidet folgende logische Queue-Typen:

| Queue | Funktion |
|---|---|
| Graphics Queue | Rendering und Grafikoperationen |
| Compute Queue | GPU-Berechnungen |
| Transfer Queue | Speicher- und Datentransfers |
| Presentation Queue | Übergabe präsentationsfähiger Buffer |

Eine Cross-Queue-Abhängigkeit beschreibt mindestens:

- Source Queue
- Destination Queue
- Source Submission
- Destination Submission
- Betroffene Ressourcen
- Signal- und Wait-Bedingungen
- Erforderliche Resource Barriers
- Optionalen Ownership Transfer

Mehrere logische Queues DÜRFEN auf dieselbe physische Hardware-Queue abgebildet werden.

## 4. Execution Dependencies

NovaOS MUSS queueübergreifende Ausführungsabhängigkeiten korrekt berücksichtigen.

Dabei gilt:

1. Eine Source Queue erzeugt einen definierten Synchronisationspunkt.
2. Die Destination Queue erhält eine entsprechende Wait-Bedingung.
3. Abhängige Operationen dürfen erst nach Erfüllung der erforderlichen Bedingungen ausgeführt werden.
4. Unabhängige Operationen bleiben möglichst parallel ausführbar.
5. Zirkuläre Warteabhängigkeiten MÜSSEN verhindert oder erkannt werden.

Die erfolgreiche Einreichung einer Source Submission bedeutet nicht deren abgeschlossene Ausführung.

## 5. Synchronization Mechanisms

Nova Cross-Queue Synchronization MUSS folgende Mechanismen integrieren können:

| Mechanismus | Verwendung |
|---|---|
| Binary Semaphore | Einzelne GPU-seitige Abhängigkeiten |
| Timeline Semaphore | Fortschrittsbasierte Synchronisation |
| Resource Barrier | Ausführungs- und Speichersynchronisation |
| Queue Ownership Transfer | Kontrollierter Wechsel der Ressourcenzuordnung |
| GPU Fence | CPU-seitige Abschlussüberwachung |

GPU-seitige Synchronisation SOLL bevorzugt werden.

CPU-seitiges Warten DARF nicht als regulärer Ersatz für verfügbare GPU-seitige Queue-Synchronisation verwendet werden.

## 6. Resource Ownership Transfer

NovaOS MUSS erforderliche Queue Ownership Transfers unterstützen.

Dabei gilt:

- Ressourcenbesitz wird entsprechend dem GPU-Backend verwaltet.
- Erforderliche Release- und Acquire-Operationen werden korrekt ausgeführt.
- Ressourcenlayouts und Speicherzugriffe werden berücksichtigt.
- Ein Ownership Transfer erfolgt nur, wenn die Hardware- oder API-Semantik ihn erfordert.
- Ressourcen mit zulässiger gemeinsamer Queue-Nutzung benötigen keinen unnötigen Ownership Transfer.

Ein Ownership Transfer ersetzt keine zusätzlich erforderliche Ausführungssynchronisation.

## 7. Resource Hazard Tracking

Nova Cross-Queue Synchronization MUSS mit Nova GPU Resource Hazard Tracking zusammenarbeiten.

Berücksichtigt werden:

- Read-after-Write
- Write-after-Read
- Write-after-Write
- Ressourcenlayouts
- Speicher- und Cache-Sichtbarkeit
- Überlappende Ressourcenbereiche

Unabhängige Lesezugriffe SOLLEN ohne unnötige Synchronisation möglich sein.

Konkurrierende inkompatible Zugriffe MÜSSEN korrekt geordnet werden.

## 8. Render Graph Integration

Nova Cross-Queue Synchronization MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Render Pass Dependencies bestimmen die erforderliche Ausführungsordnung.
- GPU Execution Scheduling ordnet geeignete Queues zu.
- Cross-Queue-Abhängigkeiten werden automatisch abgeleitet.
- Erforderliche Semaphores und Barriers werden eingeplant.
- Unabhängige Render Passes können parallel ausgeführt werden.

Die Queue-Zuordnung DARF die logischen Render-Graph-Abhängigkeiten nicht verändern.

## 9. Asynchronous Compute Integration

Nova Cross-Queue Synchronization MUSS Nova Asynchronous Compute unterstützen.

Beispiel:

1. Graphics Queue erzeugt einen Zwischenbuffer.
2. Graphics Queue signalisiert einen Synchronisationspunkt.
3. Compute Queue wartet auf diesen Synchronisationspunkt.
4. Compute Queue verarbeitet den Zwischenbuffer.
5. Compute Queue signalisiert den Abschluss.
6. Graphics Queue verwendet das Ergebnis.

Erforderliche Ressourcenbarrieren und Ownership Transfers MÜSSEN zusätzlich berücksichtigt werden.

## 10. Scheduling und Performance

NovaOS SOLL queueübergreifende Synchronisation anhand ihrer tatsächlichen Kosten optimieren.

Dabei gilt:

- Unnötige Queue-Wechsel SOLLEN vermieden werden.
- Unabhängige Operationen SOLLEN parallel geplant werden.
- Synchronisationskosten MÜSSEN bei Scheduling-Entscheidungen berücksichtigt werden.
- Zeitkritische Frame-Operationen SOLLEN bevorzugt werden.
- Eine serielle Ausführung DARF gewählt werden, wenn sie effizienter ist.

Mehrere Hardware-Queues garantieren keine höhere Leistung.

## 11. Single-Queue Fallback

NovaOS MUSS GPUs ohne unabhängige Hardware-Queues unterstützen.

Dabei gilt:

- Logische Queue-Abhängigkeiten bleiben erhalten.
- Aufträge werden auf kompatible Hardware-Queues abgebildet.
- Erforderliche Ausführungs- und Speicherabhängigkeiten bleiben gültig.
- Unnötige Cross-Queue-Synchronisationsobjekte können entfallen.
- Die korrekte Ausführungsreihenfolge MUSS gewährleistet bleiben.

Der Fallback DARF keine zusätzliche Hardwareparallelität voraussetzen.

## 12. Sicherheit und Isolation

Cross-Queue Synchronization MUSS die NovaOS-Sicherheitsarchitektur einhalten.

Dabei gilt:

- Queue- und Ressourcenhandles MÜSSEN validiert werden.
- Prozessübergreifende Ressourcenfreigaben erfolgen capability-basiert.
- Anwendungen DÜRFEN fremde Queues nicht unautorisiert steuern.
- Synchronisationsobjekte MÜSSEN kontrollierte Eigentumsverhältnisse besitzen.
- Fehlerhafte Anwendungen DÜRFEN fremde GPU-Ressourcen nicht unzulässig verändern.

## 13. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Queue-Abhängigkeiten.
- Nicht erfüllbare Wait-Bedingungen.
- Zirkuläre Synchronisationsabhängigkeiten.
- Fehlgeschlagene GPU-Submissions.
- Ungültige Ownership Transfers.
- GPU-Timeouts.
- Geräteverlust.

Dabei gilt:

- Fehlgeschlagene Operationen DÜRFEN keine erfolgreichen Abschlusssignale erzeugen.
- Abhängige Submissions MÜSSEN kontrolliert behandelt werden.
- Ressourcen MÜSSEN bis zur sicheren Freigabe geschützt bleiben.
- Bei Geräteverlust MUSS eine definierte Recovery- oder Invalidierungsstrategie greifen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Cross-Queue Synchronization unterstützen.
2. Graphics-, Compute- und Transfer-Queues MÜSSEN integrierbar sein.
3. Queueübergreifende Abhängigkeiten MÜSSEN explizit beschrieben werden.
4. GPU Semaphores MÜSSEN unterstützt werden.
5. Timeline Synchronization SOLL bevorzugt werden, sofern geeignet.
6. Resource Barriers MÜSSEN korrekt integriert werden.
7. Erforderliche Queue Ownership Transfers MÜSSEN unterstützt werden.
8. Resource Hazards MÜSSEN berücksichtigt werden.
9. Unabhängige GPU-Aufträge SOLLEN parallel ausführbar bleiben.
10. CPU-seitiges Busy Waiting SOLL vermieden werden.
11. Render Graph MUSS Cross-Queue-Abhängigkeiten bereitstellen können.
12. GPU Execution Scheduling MUSS integriert sein.
13. Unnötige Queue-Wechsel und Synchronisationsoperationen SOLLEN vermieden werden.
14. Bekannte Synchronisationszyklen MÜSSEN verhindert oder erkannt werden.
15. Ein Single-Queue-Fallback MUSS verfügbar sein.
16. Prozessübergreifende Synchronisation MUSS capability-basiert geschützt werden.
17. Fehlerhafte Synchronisationszustände MÜSSEN kontrolliert behandelt werden.
18. Im deterministischen Modus MÜSSEN logische Queue-Abhängigkeiten und Scheduling-Entscheidungen bei identischen Eingaben reproduzierbar sein.

## 15. Ergebnis

Nova Cross-Queue Synchronization ermöglicht die sichere Koordination unterschiedlicher GPU-Warteschlangen.

Durch explizite Abhängigkeiten, GPU-seitige Synchronisation, kontrollierte Ressourcenübergaben und adaptive Queue-Zuordnung können Graphics-, Compute- und Transferoperationen effizient zusammenarbeiten.

Damit erhält NovaOS eine skalierbare Synchronisationsarchitektur, die moderne GPU-Parallelität nutzt und gleichzeitig auf einfacher Hardware zuverlässig funktioniert.