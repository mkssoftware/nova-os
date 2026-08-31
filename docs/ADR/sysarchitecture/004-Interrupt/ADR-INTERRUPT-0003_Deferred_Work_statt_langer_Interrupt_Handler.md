# ADR-INTERRUPT-0003 – Deferred Work statt langer Interrupt Handler

## Status

Angenommen

## Kategorie

Kernel / Interrupts / Scheduling / Deferred Work / Latenz / Treiber / Structured Concurrency

## Kontext

Hardwareinterrupts unterbrechen den normalen Kontrollfluss des Systems.

Während ein Interrupt Handler ausgeführt wird, gelten typischerweise stärkere Einschränkungen als in normalem Thread-Kontext:

- Scheduling ist eingeschränkt,
- blockierende Operationen sind ungeeignet,
- lange Ausführungszeiten erhöhen Interruptlatenzen,
- andere Interrupts können verzögert werden,
- CPU-Zeit wird einem normalen Schedulingkontext entzogen,
- komplexe Treiberlogik wird schwieriger synchronisierbar.

Ein Gerätetreiber könnte theoretisch sämtliche Verarbeitung direkt im Interrupt Handler durchführen.

Beispiel:

```text
Interrupt
   ↓
Read Device
   ↓
Parse Data
   ↓
Process Data
   ↓
Update State
   ↓
Notify Application
   ↓
Return
```

Dies führt bei größeren Arbeitsmengen zu langen Interrupt Handlern und schlechter vorhersehbaren Latenzen.

NovaOS benötigt deshalb eine klare Architekturgrenze zwischen:

```text
time-critical interrupt handling
```

und:

```text
normal deferred processing
```

---

## Entscheidung

NovaOS verwendet für Interruptverarbeitung grundsätzlich das Prinzip:

```text
Minimal Interrupt Handler
        ↓
Capture / Acknowledge
        ↓
Schedule Deferred Work
        ↓
Return from Interrupt
        ↓
Deferred Execution Context
        ↓
Full Processing
```

Interrupt Handler müssen so kurz wie sinnvoll gehalten werden.

Arbeiten, die nicht zwingend im unmittelbaren Interruptkontext ausgeführt werden müssen, werden in einen kontrollierten Deferred-Work-Kontext verschoben.

Die zentrale Regel lautet:

```text
Do only what must happen now.

Defer everything else.
```

---

## Zweistufiges Verarbeitungsmodell

Die Interruptverarbeitung wird logisch in zwei Ebenen getrennt.

### Immediate Interrupt Context

Aufgaben:

```text
identify interrupt
acknowledge hardware
capture minimal state
protect against data loss
enqueue deferred work
```

### Deferred Execution Context

Aufgaben:

```text
process data
perform protocol logic
allocate resources
wake tasks
run complex driver logic
continue asynchronous operation
```

Die konkrete Implementierung dieser Ebenen darf architektur- und subsystemabhängig sein.

---

## Immediate Handler

Der unmittelbare Interrupt Handler darf nur Arbeiten durchführen, die:

- zeitkritisch sind,
- zur Quittierung des Controllers beziehungsweise Geräts erforderlich sind,
- Datenverlust verhindern,
- den für spätere Verarbeitung notwendigen Zustand sichern.

Beispiel:

```text
NIC Interrupt
    ↓
Acknowledge IRQ
    ↓
Record completed queue entries
    ↓
Schedule network processing
    ↓
Return
```

Das vollständige Netzwerkprotokoll wird nicht im Interrupt Handler ausgeführt.

---

## Deferred Work

Deferred Work bezeichnet Arbeit, die durch ein Interrupt-Ereignis ausgelöst wurde, aber nicht im unmittelbaren Interruptkontext ausgeführt werden muss.

Konzeptionell:

```text
DeferredWork {
    source
    operation
    executionScope
    priority
    affinity
}
```

Die konkrete Struktur wird separat spezifiziert.

---

## Kein universeller einzelner Mechanismus

NovaOS definiert Deferred Work als gemeinsame Semantik, nicht zwingend als genau eine Queue oder einen einzigen Worker Thread.

Je nach Anforderung können unterschiedliche Ausführungsformen verwendet werden:

```text
Immediate Deferred Queue
Kernel Worker
Driver Worker
Per-CPU Worker
Realtime Deferred Worker
Subsystem Execution Scope
```

Die Auswahl muss zur Latenz-, Ressourcen- und Concurrency-Anforderung passen.

---

## Interrupt Context Restrictions

Im unmittelbaren Interruptkontext sollen insbesondere vermieden werden:

- blockierende Locks,
- Dateisystemoperationen,
- lang laufende Schleifen,
- komplexe Speicherallokationen,
- Benutzerraumkommunikation mit Wartepflicht,
- umfangreiche Parsing- oder Transformationsarbeit,
- synchrone I/O-Folgeoperationen.

Falls eine Operation potentiell schlafen oder warten kann, gehört sie grundsätzlich nicht in den Immediate Handler.

---

## Speicherallokationen

Interrupt Handler sollen keine allgemeinen, potentiell blockierenden Speicherallokationen durchführen.

Falls kurzfristiger Speicher benötigt wird, müssen geeignete Mechanismen verwendet werden, beispielsweise:

```text
preallocated buffers
fixed-size pools
lock-free queues
reserved emergency resources
```

Größere oder komplexere Allokationen werden Deferred Work überlassen.

---

## Gerätequittierung

Ein Interrupt darf nicht zu früh als vollständig verarbeitet gelten, wenn dadurch das Gerät erneut unkontrolliert Interrupts erzeugt oder Daten verloren gehen könnten.

Der Immediate Handler muss daher die minimale controller- beziehungsweise gerätespezifische Quittierung korrekt durchführen.

Die eigentliche Datenverarbeitung bleibt davon getrennt.

---

## Interrupt Storm Prevention

Bei sehr hoher Interruptfrequenz darf der Immediate Handler Maßnahmen einleiten, um Interrupt Storms zu begrenzen.

Beispielsweise:

```text
Interrupt
   ↓
Mask source
   ↓
Schedule deferred drain
   ↓
Drain queue
   ↓
Unmask source
```

Diese Technik darf jedoch nicht dazu führen, dass Interruptquellen dauerhaft maskiert bleiben.

---

## Coalescing

Mehrere gleichartige Interrupt-Ereignisse dürfen zu einer Deferred-Work-Ausführung zusammengeführt werden.

Beispiel:

```text
IRQ
IRQ
IRQ
IRQ
 ↓
one queued deferred job
```

Dadurch wird verhindert, dass identische Arbeit unnötig mehrfach eingeplant wird.

Die Semantik darf dabei keine Ereignisse oder Daten verlieren.

---

## Queueing

Deferred Work muss ohne unkontrolliertes Wachstum eingeplant werden können.

Queues benötigen daher definierte:

- Kapazitäten,
- Backpressure-Regeln,
- Overflow-Verhalten,
- Prioritäten.

Ein Interrupt Storm darf nicht zu unbegrenzter Speicherbelegung führen.

---

## Per-CPU Deferred Work

Für hochfrequente Interruptquellen kann Deferred Work pro CPU organisiert werden.

Beispiel:

```text
IRQ on CPU 6
     ↓
Per-CPU Deferred Queue 6
     ↓
Worker CPU 6
```

Dies erhält Cache- und NUMA-Lokalität und reduziert unnötige Cross-CPU-Kommunikation.

---

## Affinity

Deferred Work soll die Affinity der Interruptquelle berücksichtigen können.

Beispiel:

```text
Device Queue
    ↓
IRQ CPU 8
    ↓
Deferred Worker CPU 8
```

Eine identische CPU ist nicht zwingend erforderlich.

Die Platzierung folgt den Affinity-, Scheduling- und Locality-Policies.

---

## NUMA

Bei NUMA-Systemen sollen:

- Interrupt,
- Device Queue,
- DMA Buffer,
- Deferred Worker

nach Möglichkeit in derselben Locality Domain verarbeitet werden.

Hard Constraints haben weiterhin Vorrang.

---

## Priorität

Deferred Work muss mit einer definierten Schedulingpriorität ausgeführt werden.

Nicht jede Interruptquelle darf automatisch höchste Thread-Priorität erhalten.

Die Priorität wird aus der Semantik der Arbeit abgeleitet.

Beispiele:

```text
Realtime Device
    → realtime deferred priority

Interactive Input
    → latency-sensitive priority

Background Storage Cleanup
    → normal/background priority
```

---

## Scheduler-Integration

Deferred Work wird in die reguläre Schedulingarchitektur von NovaOS integriert.

Damit unterliegt es:

- Prioritäten,
- CPU-Budgets,
- Affinity,
- Deadlines,
- Resource Economy,
- Structured Concurrency.

Der Interrupt selbst bleibt Hardwareereignis.

Die weiterführende Arbeit wird zu kontrollierter systemweiter Ausführung.

---

## Structured Concurrency

Deferred Work darf nicht als unverwaltete Hintergrundarbeit entstehen.

Jede längerlebige Operation muss einem definierten Execution Scope beziehungsweise Lifecycle zugeordnet werden.

Konzeptionell:

```text
Device
   ↓
Driver Execution Scope
   ↓
Deferred Work
```

Wird ein Gerät entfernt oder der Treiber beendet, kann zugehörige Arbeit:

```text
cancel
drain
join
```

werden.

---

## Device Lifecycle

Deferred Work muss mit dem Device Lifecycle kompatibel sein.

Beim Entfernen eines Geräts gilt:

```text
Quiesce
   ↓
Mask Interrupt
   ↓
Stop new deferred work
   ↓
Drain / Cancel pending work
   ↓
Synchronize
   ↓
Release resources
```

Kein Deferred Worker darf anschließend auf bereits freigegebene Device-Ressourcen zugreifen.

---

## Surprise Removal

Bei unerwartetem Device Removal können bereits eingeplante Arbeiten existieren.

Diese müssen erkennen können, dass das Device nicht mehr verfügbar ist.

Ein Deferred Worker darf Hardwarezugriffe nicht blind fortsetzen.

---

## Synchronisation

Die Übergabe zwischen Interrupt Handler und Deferred Work benötigt klar definierte Synchronisationsmechanismen.

Beispiele:

```text
atomic state
lock-free queue
bounded ring buffer
ownership transfer
```

Schwere Locks im Interruptpfad sollen vermieden werden.

---

## Ownership Transfer

Daten, die vom Interrupt Handler an Deferred Work übergeben werden, sollen eine klare Ownership besitzen.

Beispiel:

```text
DMA completion
      ↓
IRQ captures descriptor
      ↓
ownership transferred
      ↓
Deferred worker
```

Dies verhindert konkurrierende Verarbeitung derselben Daten.

---

## Zero-Copy

Deferred Work soll mit dem NovaOS-Zero-Copy-Prinzip kompatibel sein.

Der Immediate Handler soll Daten nicht unnötig kopieren, nur um sie an den Worker weiterzugeben.

Bevorzugt werden:

```text
descriptor reference
buffer ownership transfer
shared ring
view
```

sofern Sicherheit und Lebensdauer dies erlauben.

---

## Backpressure

Wenn Deferred Work langsamer verarbeitet wird als Interrupts eintreffen, muss das System einen definierten Überlastpfad besitzen.

Mögliche Maßnahmen:

- Queue-Coalescing,
- Interrupt Masking,
- Device Throttling,
- Queue Flow Control,
- Work Budgeting.

Unbegrenztes Queuewachstum ist nicht zulässig.

---

## Work Budget

Eine einzelne Deferred-Work-Ausführung darf ebenfalls nicht unbegrenzt CPU-Zeit konsumieren.

Für große Queues können Budgets verwendet werden.

Beispiel:

```text
Process up to N descriptors
        ↓
more work?
        ↓
reschedule
```

Damit wird verhindert, dass Deferred Work lediglich zu einem neuen nicht-präemptierbaren Langläufer wird.

---

## Realtime

Für Hard-Realtime-Anforderungen kann speziell priorisiertes Deferred Work erforderlich sein.

Dabei müssen:

- maximale Interrupt Handler-Zeit,
- maximale Deferred-Work-Latenz,
- CPU Affinity,
- Schedulingpriorität

explizit kontrollierbar sein.

Nicht zeitkritische Deferred Work darf diese Pfade nicht unkontrolliert blockieren.

---

## Deterministic Mode

Im Deterministic Mode müssen relevante Deferred-Work-Eigenschaften stabilisiert werden können.

Dazu können gehören:

```text
fixed target CPU
fixed priority
fixed queue policy
fixed work budget
disabled adaptive coalescing
```

Die Verarbeitung bleibt damit reproduzierbarer.

---

## Fehlerbehandlung

Fehler in Deferred Work dürfen den Interruptpfad nicht in einen undefinierten Zustand bringen.

Mögliche Ergebnisse sind:

```text
Completed
RetryableFailure
PermanentFailure
Cancelled
DeviceRemoved
ResourceUnavailable
```

Retry-Verhalten muss ausdrücklich definiert sein.

---

## Kein Blind Retry

Nicht-idempotente Operationen dürfen bei Deferred-Work-Fehlern nicht blind erneut ausgeführt werden.

Retry benötigt eine explizite Semantik.

---

## Watchdog und Diagnose

NovaOS soll erkennen können, wenn:

- Interrupt Handler ungewöhnlich lange laufen,
- Deferred Queues dauerhaft anwachsen,
- Work nicht abgearbeitet wird,
- ein Gerät Interrupt Storms erzeugt.

Solche Zustände müssen diagnostizierbar sein.

---

## Laufzeitmessung

Interrupt Handler und Deferred Work sollen getrennt messbar sein.

Beispiel:

```text
IRQ:
    count: 120000/s
    avg immediate time: 1.8 µs
    max immediate time: 12 µs

Deferred:
    avg queue latency: 35 µs
    avg processing time: 8 µs
```

Damit können Latenz- und Überlastprobleme gezielt untersucht werden.

---

## Architecture Introspection

NovaOS soll relevante Beziehungen darstellen können.

Beispiel:

```text
Interrupt Source:
    NVMe Queue 2

Immediate Handler:
    active

Deferred Worker:
    storage.worker.2

Affinity:
    CPU 8

Queue Depth:
    3

Execution Scope:
    nvme.device.0
```

---

## Sicherheitsgrenze

Deferred Work darf keine Berechtigungen erhalten, die der auslösende Treiber beziehungsweise Execution Scope nicht besitzt.

Das Verschieben von Arbeit aus Interruptkontext in Worker-Kontext darf keine Privilegieneskalation darstellen.

---

## User-Mode-Treiber

Bei isolierten oder User-Mode-Treibern kann der Kernel einen minimalen Kernel-Interruptpfad besitzen und anschließend kontrolliert Arbeit an den Treiber weiterreichen.

Konzeptionell:

```text
Hardware IRQ
    ↓
Kernel Interrupt Core
    ↓
Validate / Acknowledge
    ↓
Driver Event
    ↓
User-Mode Driver Worker
```

Die Interrupt-Controller-Hardware bleibt weiterhin geschützt.

---

## KI-Unabhängigkeit

Deferred Work ist grundlegende Kernelinfrastruktur und muss vollständig ohne KI funktionieren.

Adaptive Systeme dürfen optional Parameter wie Coalescing oder Budgets optimieren.

Solche Optimierungen dürfen:

- Hard Realtime,
- Determinismus,
- Queue-Grenzen,
- Sicherheitsregeln

nicht verletzen.

---

## Normative Anforderungen

1. NovaOS MUSS unmittelbare Interruptverarbeitung von längerer Folgearbeit trennen können.
2. Immediate Interrupt Handler MÜSSEN so kurz wie technisch sinnvoll gehalten werden.
3. Arbeiten, die warten oder blockieren können, DÜRFEN NICHT im Immediate Interrupt Context ausgeführt werden.
4. Interrupt Handler SOLLEN nur Quittierung, minimale Zustandserfassung und zeitkritische Sicherung durchführen.
5. Nicht unmittelbar erforderliche Verarbeitung MUSS als Deferred Work ausführbar sein.
6. Deferred Work MUSS über kontrollierte Schedulingkontexte ausgeführt werden.
7. Deferred Work MUSS Priorität und Affinity besitzen können.
8. Deferred Work MUSS mit NUMA- und Topology-Awareness integrierbar sein.
9. Interrupt Handler DÜRFEN keine potentiell blockierenden allgemeinen Speicherallokationen voraussetzen.
10. Queueing zwischen Interrupt Handler und Deferred Work MUSS begrenzt beziehungsweise kontrollierbar sein.
11. Überlast MUSS einen definierten Backpressure-Mechanismus besitzen können.
12. Gleichartige Deferred Events DÜRFEN zusammengeführt werden, sofern keine relevante Semantik verloren geht.
13. Deferred Work SOLL CPU-Budgets beziehungsweise Work Budgets unterstützen können.
14. Deferred Work DARF NICHT unbegrenzt CPU-Zeit ohne Schedulingkontrolle beanspruchen.
15. Interrupt- und Deferred-Work-Ausführung MÜSSEN getrennt messbar sein.
16. Deferred Work MUSS einem kontrollierten Lifecycle beziehungsweise Execution Scope zugeordnet werden können.
17. Device Removal MUSS neue Deferred Work verhindern und bestehende Arbeit drainen oder abbrechen können.
18. Surprise Removal MUSS von bereits eingeplanter Arbeit sicher behandelt werden können.
19. Datenübergabe zwischen Interrupt Handler und Deferred Work MUSS eine definierte Synchronisations- und Ownership-Semantik besitzen.
20. Zero-Copy-Übergaben SOLLEN bevorzugt werden, sofern Korrektheit und Sicherheit erhalten bleiben.
21. Realtime-Workloads MÜSSEN kontrollierte Deferred-Work-Latenz und -Priorität anfordern können.
22. Deterministic Mode MUSS Deferred-Work-Platzierung und Scheduling stabilisieren können.
23. Retry-Verhalten MUSS explizit definiert sein.
24. Nicht-idempotente Deferred Operationen DÜRFEN NICHT blind wiederholt werden.
25. Lange Interrupt Handler und dauerhaft überfüllte Deferred Queues MÜSSEN diagnostizierbar sein.
26. Sicherheits- und Capability-Grenzen MÜSSEN beim Übergang zu Deferred Work erhalten bleiben.
27. User-Mode-Treiber MÜSSEN Deferred Interrupt Processing verwenden können, ohne direkten Zugriff auf Interrupt Controller zu erhalten.
28. Interrupt Storms MÜSSEN kontrolliert begrenzbar sein.
29. Grundlegende Deferred-Work-Verarbeitung MUSS vollständig ohne KI funktionieren.
30. Adaptive Optimierungen DÜRFEN harte Latenz-, Sicherheits- oder Determinismusanforderungen NICHT überschreiben.

---

## Konsequenzen

### Positive Konsequenzen

- kürzere Interrupt Handler,
- geringere Interruptlatenz,
- besseres SMP- und NUMA-Verhalten,
- komplexe Treiberlogik läuft in kontrollierbaren Kontexten,
- bessere Scheduler- und Resource-Economy-Integration,
- sauberer Device-Lifecycle,
- einfachere Isolation von User-Mode-Treibern,
- bessere Diagnose von Interrupt- und Queueproblemen.

### Negative Konsequenzen

- zusätzliche Queue- und Worker-Infrastruktur,
- Übergabe zwischen Interrupt und Worker benötigt Synchronisation,
- Deferred Processing erhöht bei manchen Operationen geringfügig die Verarbeitungslatenz,
- Überlast- und Backpressure-Regeln müssen sorgfältig definiert werden.

---

## Verworfene Alternativen

### Vollständige Verarbeitung im Interrupt Handler

Verworfen.

Lange Handler verschlechtern Interruptlatenzen und erschweren Scheduling, SMP und Echtzeitverhalten.

### Ein globaler Deferred Worker für das gesamte System

Verworfen.

Dies erzeugt unnötige Serialisierung und schlechte NUMA-/CPU-Lokalität.

### Unbegrenzte Deferred Queues

Verworfen.

Interrupt Storms könnten zu unbegrenztem Speicherverbrauch führen.

### Beliebige Hintergrundthreads ohne Lifecycle

Verworfen.

Dies widerspricht Structured Concurrency und erschwert Device Removal.

### Immer alles sofort deferieren

Verworfen.

Bestimmte Quittierungs- und Zustandssicherungsoperationen müssen unmittelbar im Interruptkontext erfolgen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-INTERRUPT-0001_Einheitliche_Interrupt_Controller_Abstraktion`
- `ADR-INTERRUPT-0002_Interrupt_Affinity_und_Topology_Awareness`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`

---

## Zugehörige NPSPECs

- `NPSPEC-INTERRUPT-DEFERRED-0001`
- `NPSPEC-INTERRUPT-IMMEDIATE-HANDLER-0001`
- `NPSPEC-INTERRUPT-DEFERRED-QUEUE-0001`
- `NPSPEC-INTERRUPT-DEFERRED-WORKER-0001`
- `NPSPEC-INTERRUPT-WORK-BUDGET-0001`
- `NPSPEC-INTERRUPT-BACKPRESSURE-0001`
- `NPSPEC-INTERRUPT-COALESCING-0001`
- `NPSPEC-INTERRUPT-DEFERRED-AFFINITY-0001`
- `NPSPEC-INTERRUPT-DEFERRED-REALTIME-0001`
- `NPSPEC-INTERRUPT-DEFERRED-LIFECYCLE-0001`
- `NPSPEC-INTERRUPT-DEFERRED-INTROSPECTION-0001`
- `NPSPEC-INTERRUPT-DEFERRED-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- kurzer Immediate Handler,
- Deferred Processing nach Interrupt,
- blockierende Operation wird aus Interruptkontext verhindert,
- Per-CPU Deferred Queue,
- NUMA-aware Worker-Platzierung,
- MSI-X Multi-Queue-Gerät,
- Interrupt Coalescing,
- begrenzte Deferred Queue,
- Queue Overflow,
- Backpressure,
- Interrupt Mask/Drain/Unmask,
- Work Budget mit Rescheduling,
- Device Hot Removal bei leerer Queue,
- Device Hot Removal bei laufendem Deferred Work,
- Surprise Removal,
- Cancellation über Execution Scope,
- Zero-Copy Buffer Handoff,
- Realtime Deferred Work,
- Deterministic Mode,
- nicht-idempotenter Fehlerfall ohne Blind Retry,
- User-Mode-Treiber,
- Interrupt Storm,
- Laufzeit- und Queue-Latenz-Messung.

---

## Ergebnis

NovaOS trennt zeitkritische Interruptbehandlung konsequent von längerer Folgearbeit.

Das Architekturmodell lautet:

```text
Hardware Interrupt
       ↓
Minimal Immediate Handler
       ↓
Capture / Acknowledge
       ↓
Queue Deferred Work
       ↓
Return from Interrupt
       ↓
Scheduled Execution Context
       ↓
Full Processing
```

Damit bleibt der unmittelbare Interruptpfad klein und vorhersehbar, während komplexe Verarbeitung in reguläre Scheduling-, Resource-, Locality- und Lifecycle-Mechanismen integriert wird.

Die zentrale Architekturregel lautet:

```text
Interrupt context is for urgency,
not for general work.

Acknowledge quickly.

Capture what matters.

Defer the rest.
```