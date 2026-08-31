# ADR-INTERRUPT-0004 – Hybrides Interrupt- und Polling-Modell

## Status

Angenommen

## Kategorie

Kernel / Interrupts / Polling / Scheduling / I/O / Performance / Latenz / Energieeffizienz

## Kontext

Reine Interruptverarbeitung und reines Polling besitzen unterschiedliche Stärken und Schwächen.

Interrupts sind besonders geeignet, wenn Ereignisse:

- unregelmäßig auftreten,
- geringe durchschnittliche Frequenz besitzen,
- schnelle Reaktion ohne permanente CPU-Aktivität erfordern.

Bei sehr hohen Ereignisraten können Interrupts jedoch selbst erhebliche Kosten verursachen.

Beispiel:

```text
Packet
   ↓
Interrupt
   ↓
Handler
   ↓
Deferred Work
   ↓
Packet

Packet
   ↓
Interrupt
   ↓
Handler
   ↓
Deferred Work
```

Bei Millionen Ereignissen pro Sekunde entstehen dadurch:

- hohe Interruptfrequenz,
- häufige Kontextwechsel,
- Cache-Störungen,
- Scheduling-Overhead,
- Interrupt Storms.

Polling besitzt das gegenteilige Verhalten.

Ein aktiver Polling-Loop kann große Ereignismengen effizient verarbeiten, verbraucht aber CPU-Zeit, wenn keine Arbeit vorhanden ist.

NovaOS benötigt deshalb kein dogmatisches:

```text
Interrupts only
```

oder:

```text
Polling only
```

sondern ein kontrolliertes hybrides Modell.

---

## Entscheidung

NovaOS unterstützt ein **hybrides Interrupt- und Polling-Modell**.

Interrupts dienen grundsätzlich als effiziente Ereignisbenachrichtigung.

Bei hoher oder kontinuierlicher Ereignislast darf ein Subsystem kontrolliert in einen Polling- beziehungsweise Batch-Verarbeitungsmodus wechseln.

Das Grundmodell lautet:

```text
Idle / Low Load
      ↓
Interrupt Mode
      ↓
Event Burst detected
      ↓
Mask / Reduce Interrupts
      ↓
Polling / Batch Processing
      ↓
Queue drained
      ↓
Return to Interrupt Mode
```

Die Auswahl des Modus erfolgt oberhalb des Interrupt Controllers als Policyentscheidung.

---

## Grundprinzip

```text
Interrupt when waiting is cheaper.

Poll when work is already abundant.
```

Dabei gilt:

```text
Polling is an optimization,
not a correctness requirement.
```

---

## Baseline

Der normale Baseline-Betrieb verwendet Interrupts.

Dadurch bleiben Geräte effizient nutzbar, wenn:

- keine Arbeit vorhanden ist,
- Ereignisse selten auftreten,
- das System Energie sparen soll.

Polling wird nur eingesetzt, wenn ein geeigneter Grund vorliegt.

---

## Hybrid Mode

Ein Interrupt kann den Übergang in einen temporären Pollingmodus auslösen.

Beispiel:

```text
Device generates interrupt
        ↓
Immediate Handler
        ↓
Mask queue interrupt
        ↓
Schedule deferred polling
        ↓
Process queue in batches
        ↓
Queue becomes quiet
        ↓
Unmask interrupt
```

Dieses Muster verbindet die Energieeffizienz von Interrupts mit der Verarbeitungseffizienz von Polling.

---

## Polling Scope

Polling muss einem klar definierten Scope zugeordnet sein.

Beispiele:

```text
Device
Queue
Channel
CPU-local queue
Driver execution scope
```

Ein globaler unkontrollierter Pollingloop ist nicht vorgesehen.

---

## Queue-basiertes Polling

Polling eignet sich besonders für queue-basierte Geräte.

Beispiele:

- Netzwerkadapter,
- NVMe,
- schnelle Storagecontroller,
- Hochgeschwindigkeits-I/O,
- virtuelle Geräte.

Konzeptionell:

```text
Interrupt
   ↓
Queue N has work
   ↓
Poll Queue N
   ↓
Process batch
```

---

## Polling Budget

Polling darf nicht unbegrenzt laufen.

Jede Polling-Ausführung benötigt ein definiertes Budget.

Dieses kann sich beziehen auf:

```text
number of descriptors
number of packets
number of completions
time budget
resource budget
```

Beispiel:

```text
Process up to 64 entries
        ↓
budget exhausted?
        ↓
reschedule if work remains
```

Dadurch bleibt die CPU für andere Aufgaben verfügbar.

---

## Batch Processing

Polling soll Ereignisse bevorzugt in Batches verarbeiten können.

Beispiel:

```text
Interrupt mode:

1 event
1 interrupt
1 scheduling operation


Polling mode:

64 events
1 scheduling operation
1 batch
```

Batching reduziert:

- Interruptkosten,
- Schedulingkosten,
- Cachewechsel,
- Synchronisationsaufwand.

---

## Interrupt Suppression

Während aktivem Polling darf die betreffende Interruptquelle vorübergehend:

- maskiert,
- reduziert,
- coalesced

werden.

Dies verhindert redundante Interrupts für Arbeit, die bereits aktiv abgefragt wird.

Die Quelle muss anschließend zuverlässig wieder aktiviert werden.

---

## Keine verlorenen Ereignisse

Der Übergang:

```text
Interrupt → Polling
```

und:

```text
Polling → Interrupt
```

muss race-frei erfolgen.

Insbesondere darf folgendes Problem nicht entstehen:

```text
Polling finds queue empty
        ↓
new event arrives
        ↓
interrupt still masked
        ↓
event never processed
```

Die Rückkehr zum Interruptmodus benötigt daher ein definiertes Recheck-/Arm-Protokoll.

---

## Re-Arm-Protokoll

Konzeptionell kann die Rückkehr lauten:

```text
Drain queue
    ↓
Prepare interrupt re-enable
    ↓
Recheck queue
    │
    ├── work exists → continue polling
    │
    └── empty → enable interrupt
```

Die genaue Hardwaresequenz hängt vom Gerät ab.

---

## Moduswahl

Die Entscheidung zwischen Interrupt und Polling kann folgende Signale verwenden:

- Queue-Tiefe,
- Interruptfrequenz,
- Ereignisrate,
- aktuelle CPU-Last,
- Latenzanforderungen,
- Energieprofil,
- Realtime Policy,
- erwartete Burst-Länge.

Die konkrete Heuristik ist nicht Bestandteil dieser ADR.

---

## Schwellenwerte

Ein Subsystem darf definierte Schwellenwerte besitzen.

Beispiel:

```text
IRQ rate > threshold
    ↓
enter polling mode

queue depth < threshold
for stable interval
    ↓
return to interrupt mode
```

Schwellenwerte müssen konfigurierbar beziehungsweise policyfähig sein.

---

## Hysterese

Der Wechsel zwischen Interrupt und Polling darf nicht bei jeder kleinen Laständerung erfolgen.

Es muss Hysterese möglich sein.

Beispiel:

```text
Enter polling:
    high load threshold

Leave polling:
    lower load threshold
```

Dadurch wird Modus-Flapping vermieden.

---

## Adaptive Steuerung

NovaOS darf Schwellenwerte und Budgets anhand beobachteter Last adaptiv optimieren.

Beispiel:

```text
Predicted:
    polling improves throughput

Observed:
    latency became worse

Prediction Error
    ↓
policy adjustment
```

Adaptive Optimierung bleibt eine weiche Policy.

Korrektheit darf nicht davon abhängen.

---

## Explizite Moduswahl

Ein Subsystem beziehungsweise Execution Contract kann einen Modus ausdrücklich anfordern.

Beispiele:

```text
Mode = InterruptPreferred
Mode = PollingPreferred
Mode = FixedInterrupt
Mode = FixedPolling
Mode = Adaptive
```

Die endgültige Zulässigkeit hängt von Plattform, Ressourcen und Sicherheitsanforderungen ab.

---

## Fixed Interrupt Mode

Für bestimmte Szenarien kann Polling deaktiviert werden.

Dies kann sinnvoll sein für:

- niedrige Ereignisraten,
- energiesensitive Systeme,
- bestimmte Echtzeitmodelle,
- Debugging.

---

## Fixed Polling Mode

Für spezialisierte Hochlast-Workloads kann ein expliziter Pollingmodus erlaubt werden.

Beispiele:

- dedizierte Netzwerkverarbeitung,
- Storage Benchmarks,
- isolierte Realtime-Pipelines.

Fixed Polling darf jedoch nicht unbegrenzt unbudgetiert CPU-Zeit verbrauchen.

---

## CPU Affinity

Polling Worker sollen mit der Interrupt- und Queue-Affinity integriert werden.

Beispiel:

```text
Queue 3
   ↓
IRQ CPU 10
   ↓
Polling Worker CPU 10
```

Dadurch bleiben:

- Cache-Lokalität,
- NUMA-Lokalität,
- Queue Ownership

möglichst stabil.

---

## NUMA

Polling Worker sollen bevorzugt dort laufen, wo sich relevante:

- Device Queue,
- DMA Memory,
- Verarbeitungsthreads

befinden.

Die Regeln aus `ADR-INTERRUPT-0002` gelten entsprechend.

---

## Structured Concurrency

Pollingloops dürfen keine unbegrenzten unabhängigen Hintergrundaktivitäten darstellen.

Sie müssen einem Execution Scope zugeordnet sein.

Beispiel:

```text
Device Scope
    ↓
Queue Scope
    ↓
Polling Worker
```

Beim Entfernen des Geräts kann der Scope:

```text
cancel
drain
join
```

werden.

---

## Device Lifecycle

Beim Device Removal gilt mindestens:

```text
Quiesce
   ↓
Stop new polling
   ↓
Mask / disable interrupts
   ↓
Cancel or drain polling workers
   ↓
Synchronize
   ↓
Release resources
```

Polling darf nach dem Entfernen nicht weiter auf Gerätehardware zugreifen.

---

## Deferred Work Integration

Das hybride Modell baut auf `ADR-INTERRUPT-0003` auf.

Polling findet grundsätzlich nicht im langen Immediate Interrupt Handler statt.

Typisches Modell:

```text
Interrupt
   ↓
Minimal Handler
   ↓
Deferred Worker
   ↓
Polling loop with budget
```

Damit bleibt der direkte Interruptpfad kurz.

---

## Resource Economy

Polling verbraucht aktiv CPU-Ressourcen.

Daher muss es in die systemweite Ressourcenökonomie integriert werden.

Ein Polling Worker kann beispielsweise besitzen:

```text
CPU budget
latency target
energy preference
deadline
```

Das System darf Polling reduzieren, wenn Ressourcen nicht verfügbar sind, sofern keine Hard Constraint entgegensteht.

---

## Energieeffizienz

Polling ist bei geringer Last typischerweise energieineffizient.

Im Energiesparmodus kann NovaOS daher:

- längere Interruptphasen,
- größere Polling-Einstiegsschwellen,
- kürzere Polling-Budgets

bevorzugen.

Im Performance-Modus können aggressivere Pollingstrategien erlaubt sein.

---

## Latenz

Polling kann unter hoher Last Latenzen reduzieren, da Ereignisse ohne erneuten Interrupt verarbeitet werden.

Zu aggressives Polling kann jedoch andere Tasks verzögern.

Deshalb müssen:

```text
throughput
latency
CPU consumption
energy
```

gemeinsam betrachtet werden.

---

## Realtime

Realtime-Szenarien dürfen fest definierte Hybridstrategien verwenden.

Beispiele:

```text
interrupt triggers realtime poll
poll fixed maximum N items
fixed CPU
fixed priority
```

Adaptive Schwellenwerte können im Hard-Realtime-Modus deaktiviert werden.

---

## Deterministic Mode

Im Deterministic Mode müssen Moduswechsel reproduzierbar sein.

Dazu kann die Policy beispielsweise:

```text
fixed mode
```

oder:

```text
fixed thresholds
fixed polling budget
fixed affinity
```

verwenden.

Nichtdeterministische adaptive Änderungen sind innerhalb entsprechender Scopes zu deaktivieren.

---

## Interrupt Coalescing

Hardware Interrupt Coalescing kann mit dem Hybridmodell kombiniert werden.

Konzeptionell:

```text
Device coalesces events
        ↓
Interrupt
        ↓
Deferred polling
        ↓
batch processing
```

Hardware-Coalescing und Software-Polling sind unterschiedliche Mechanismen und dürfen unabhängig konfiguriert werden.

---

## Busy Polling

Extrem kurzes aktives Busy Polling kann für sehr niedrige Latenzen erlaubt sein.

Es muss jedoch:

- explizit aktiviert,
- zeitlich begrenzt,
- ressourcenbudgetiert

sein.

Generelle unkontrollierte Busy Loops sind nicht zulässig.

---

## Idle Polling

Wenn ein Polling Worker keine Arbeit mehr findet, darf er nicht unbegrenzt weiterlaufen.

Er muss abhängig von der Policy:

- den Interruptmodus wieder aktivieren,
- schlafen,
- beendet werden.

---

## Backpressure

Polling löst keine grundlegenden Überlastprobleme.

Wenn das Gerät dauerhaft mehr Arbeit erzeugt als verarbeitet werden kann, muss weiterhin Backpressure verwendet werden.

Beispiele:

- Device Throttling,
- Queue Flow Control,
- Drop Policy,
- Resource Admission Control.

---

## Shared Interrupts

Bei Legacy Shared Interrupts ist der Wechsel in Polling komplexer, da mehrere Geräte dieselbe Interruptleitung verwenden können.

Das Maskieren einer gemeinsamen Leitung darf andere Geräte nicht unbeabsichtigt blockieren.

Das Hybridmodell muss die tatsächliche Controller- und Device-Semantik berücksichtigen.

---

## Fehlerbehandlung

Mögliche Hybrid-Mode-Fehler sind:

```text
PollingUnavailable
InterruptRearmFailed
QueueStateInvalid
BudgetExceeded
DeviceRemoved
ModeTransitionFailed
```

Ein fehlgeschlagener Optimierungswechsel darf das Gerät nicht unnötig unbrauchbar machen.

Soweit sicher möglich, soll auf einen gültigen Baseline-Modus zurückgefallen werden.

---

## Fail-Safe-Verhalten

Kann das System nicht zuverlässig bestimmen, ob Polling korrekt aktiviert beziehungsweise beendet werden kann, soll es den konservativeren gültigen Modus verwenden.

Bei normaler Hardware ist dies typischerweise:

```text
Interrupt Mode
```

sofern dadurch keine bekannten Ereignisse verloren gehen.

---

## Introspection

NovaOS soll den aktuellen Modus einer Interruptquelle beziehungsweise Queue darstellen können.

Beispiel:

```text
Device:
    nvme0

Queue:
    2

Mode:
    Polling

IRQ Rate:
    185000/s

Polling Budget:
    64 completions

Affinity:
    CPU 8

Reason:
    High queue activity
```

---

## Telemetrie

Für die Policy sollen mindestens messbar sein können:

- Interruptanzahl,
- Pollingzyklen,
- verarbeitete Events pro Poll,
- Queue-Tiefe,
- Pollinglaufzeit,
- Moduswechsel,
- Re-Arm-Fehler.

Diese Daten können für Diagnose und Optimierung verwendet werden.

---

## Keine KI-Abhängigkeit

Die Wahl zwischen Interrupt und Polling muss auch mit statischen oder deterministischen Regeln vollständig funktionieren.

KI darf optional Optimierungsvorschläge liefern.

KI darf nicht dafür verantwortlich sein, dass Ereignisse überhaupt zuverlässig verarbeitet werden.

---

## Normative Anforderungen

1. NovaOS MUSS Interrupt- und Polling-Verarbeitung gemeinsam unterstützen können.
2. Interruptbetrieb MUSS als allgemeiner Baseline-Modus verfügbar sein.
3. Polling MUSS kontrolliert pro Device, Queue oder vergleichbarem Scope aktivierbar sein.
4. Polling DARF NICHT als langer Immediate Interrupt Handler implementiert werden.
5. Der Übergang zu Polling SOLL über Deferred Work erfolgen.
6. Polling MUSS ein definiertes Work- beziehungsweise Zeitbudget besitzen können.
7. Polling DARF NICHT unbegrenzt unbudgetiert CPU-Zeit verbrauchen.
8. Ereignisse SOLLEN im Pollingmodus in Batches verarbeitet werden können.
9. Redundante Interrupts DÜRFEN während aktivem Polling kontrolliert unterdrückt werden.
10. Interrupt-Suppression DARF NICHT zu verlorenen Ereignissen führen.
11. Die Rückkehr zum Interruptmodus MUSS ein race-freies Re-Arm-Verfahren verwenden.
12. Moduswechsel MÜSSEN Hysterese unterstützen können.
13. Queue-Tiefe und Ereignisrate DÜRFEN als Policy-Signale verwendet werden.
14. CPU-Last, Energieprofil und Latenzanforderungen DÜRFEN die Moduswahl beeinflussen.
15. Hard Constraints MÜSSEN gegenüber adaptiven Optimierungen Vorrang besitzen.
16. Polling Worker MÜSSEN in CPU-Affinity und Topology Awareness integrierbar sein.
17. NUMA-Lokalität SOLL bei der Polling-Platzierung berücksichtigt werden.
18. Polling MUSS einem kontrollierten Execution Scope zugeordnet werden können.
19. Device Removal MUSS Polling sicher beenden können.
20. Busy Polling MUSS explizit, begrenzt und ressourcenbudgetiert sein.
21. Idle Polling DARF NICHT unbegrenzt ohne Arbeit fortgeführt werden.
22. Polling MUSS in die systemweite Ressourcenökonomie integrierbar sein.
23. Realtime-Policies MÜSSEN feste Pollingparameter verlangen können.
24. Deterministic Mode MUSS reproduzierbare Hybridentscheidungen ermöglichen.
25. Hardware Interrupt Coalescing MUSS unabhängig vom Software-Polling modellierbar bleiben.
26. Shared Interrupts MÜSSEN ihre tatsächlichen Maskierungsgrenzen berücksichtigen.
27. Dauerhafte Überlast MUSS über Backpressure behandelt werden können.
28. Fehlgeschlagene Moduswechsel MÜSSEN typisiert behandelt werden.
29. Aktueller Hybridmodus und relevante Gründe MÜSSEN introspektierbar sein.
30. Korrekte Ereignisverarbeitung MUSS vollständig ohne KI möglich sein.

---

## Konsequenzen

### Positive Konsequenzen

- geringe CPU-Kosten bei niedriger Ereignisrate,
- hoher Durchsatz bei hoher Ereignisrate,
- weniger Interrupt Storms,
- effizientere Verarbeitung von Multi-Queue-Geräten,
- bessere Cache- und NUMA-Lokalität,
- flexible Abstimmung zwischen Performance, Latenz und Energie,
- gute Integration mit Deferred Work und Scheduler.

### Negative Konsequenzen

- Moduswechsel erhöhen die Treiber- und Policy-Komplexität,
- race-freies Re-Arming muss sorgfältig implementiert werden,
- falsche Schwellenwerte können Performance oder Energieeffizienz verschlechtern,
- Polling benötigt zusätzliche CPU-Budgets und Telemetrie.

---

## Verworfene Alternativen

### Ausschließlich Interrupts

Verworfen.

Bei sehr hohen Ereignisraten kann Interruptoverhead dominant werden.

### Ausschließlich Polling

Verworfen.

Bei geringer Last würde unnötig CPU-Zeit und Energie verbraucht.

### Polling direkt im Interrupt Handler

Verworfen.

Dies würde zu langen Interrupt Handlern führen und `ADR-INTERRUPT-0003` widersprechen.

### Unbegrenztes Busy Polling

Verworfen.

Dies verhindert faire Ressourcenverteilung und ist energieineffizient.

### Treiber entscheidet vollständig unabhängig über Hybridmodus

Verworfen.

CPU-, Realtime-, Energie- und Ressourcenpolitik benötigt systemweite Koordination.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-INTERRUPT-0001_Einheitliche_Interrupt_Controller_Abstraktion`
- `ADR-INTERRUPT-0002_Interrupt_Affinity_und_Topology_Awareness`
- `ADR-INTERRUPT-0003_Deferred_Work_statt_langer_Interrupt_Handler`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`

---

## Zugehörige NPSPECs

- `NPSPEC-INTERRUPT-HYBRID-0001`
- `NPSPEC-INTERRUPT-POLLING-0001`
- `NPSPEC-INTERRUPT-POLLING-BUDGET-0001`
- `NPSPEC-INTERRUPT-MODE-TRANSITION-0001`
- `NPSPEC-INTERRUPT-REARM-0001`
- `NPSPEC-INTERRUPT-HYBRID-POLICY-0001`
- `NPSPEC-INTERRUPT-HYBRID-AFFINITY-0001`
- `NPSPEC-INTERRUPT-BUSY-POLL-0001`
- `NPSPEC-INTERRUPT-HYBRID-REALTIME-0001`
- `NPSPEC-INTERRUPT-HYBRID-INTROSPECTION-0001`
- `NPSPEC-INTERRUPT-HYBRID-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- niedrige Ereignisrate im Interruptmodus,
- hohe Ereignisrate mit Wechsel zu Polling,
- Batch Processing,
- Polling Budget Exhaustion,
- Rescheduling bei verbleibender Arbeit,
- Rückkehr zum Interruptmodus,
- Event genau während des Re-Arming,
- kein verlorenes Event beim Moduswechsel,
- Interrupt Mask/Unmask,
- Hysterese gegen Modus-Flapping,
- Multi-Queue-Netzwerkgerät,
- Multi-Queue-NVMe,
- NUMA-aware Polling Worker,
- CPU-Affinity,
- Device Hot Removal während Polling,
- Interrupt Storm,
- Fixed Interrupt Mode,
- Fixed Polling Mode,
- Busy Polling mit Zeitlimit,
- Realtime Hybrid Mode,
- Deterministic Mode,
- Hardware Interrupt Coalescing,
- dauerhafte Überlast mit Backpressure,
- Shared Interrupt,
- Introspection und Telemetrie des aktuellen Modus.

---

## Ergebnis

NovaOS kombiniert Interrupts und Polling abhängig von Last und Ausführungsanforderungen.

Das Architekturmodell lautet:

```text
Low Activity
     ↓
Interrupt Mode
     ↓
Burst / High Load
     ↓
Deferred Polling
     ↓
Batch Processing
     ↓
Load decreases
     ↓
Re-Arm
     ↓
Interrupt Mode
```

Damit bleiben Interrupts der effiziente Weckmechanismus, während Polling bei hoher Ereignisdichte den Verarbeitungs-Overhead reduziert.

Die zentrale Architekturregel lautet:

```text
Use interrupts to discover work.

Use polling to consume abundant work.

Switch deliberately.

Budget the polling.

Never lose an event during the transition.
```