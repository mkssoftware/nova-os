# ADR-DISTCOMM-0007 – Bounded Queues und Backpressure

## Status

Angenommen

## Kategorie

Distributed Communication / Flow Control / Backpressure / Resource Economy / Nova.Call

## Kontext

Verteilte Kommunikation entkoppelt Producer und Consumer zeitlich und räumlich.

Ein Producer kann Daten, Nachrichten oder Calls schneller erzeugen, als ein Consumer oder der zugrunde liegende Kommunikationspfad sie verarbeiten kann.

Ohne explizite Begrenzung entstehen dadurch potentiell unbegrenzte Queues.

Dies kann zu:

- unkontrolliertem Speicherverbrauch,
- steigender Latenz,
- Deadline-Verletzungen,
- unfairer Ressourcennutzung,
- kaskadierenden Überlastungen,
- Out-of-Memory-Situationen

führen.

NovaOS benötigt deshalb eine systemweite Architektur, bei der Queue-Kapazität begrenzt ist und Überlastung über Backpressure bis zum verursachenden Producer zurückpropagiert werden kann.

---

## Entscheidung

NovaOS verwendet für Distributed Communication grundsätzlich **bounded Queues mit expliziter Backpressure-Semantik**.

```text
Producer
   ↓
Bounded Queue
   ↓
Consumer
```

Erreicht eine Queue ihre zulässige Kapazität, darf das System nicht unbegrenzt weitere Elemente puffern.

Stattdessen wird Überlastung gemäß dem jeweiligen Contract behandelt.

```text
Producer
   ↓
Queue Full
   ↓
Backpressure
   ↓
Wait / Suspend / Reject / Drop / Degrade
```

Die konkrete Reaktion wird durch Queue-, Flow- und Execution Contracts bestimmt.

---

## Grundprinzip

```text
Every queue has a bound.
Every overload has a policy.
```

und:

```text
Backpressure follows causation.
```

---

## Bounded Queues

Jede relevante Kommunikationsqueue besitzt eine definierte oder systemseitig begrenzte Kapazität.

Grenzen können beispielsweise auf folgenden Größen basieren:

- Anzahl von Messages,
- Anzahl ausstehender Calls,
- Bytes,
- Shared Buffers,
- Resource Budget,
- Deadline Window.

Eine Queue darf nicht allein deshalb unbegrenzt wachsen, weil physischer Speicher verfügbar ist.

---

## Backpressure

Backpressure signalisiert einem vorgelagerten Producer, dass nachgelagerte Komponenten aktuell keine weitere Last innerhalb des vorgesehenen Contracts aufnehmen können.

```text
Producer
   ↓
Service A
   ↓
Service B
   ↓
Service C
```

Bei Überlastung von C soll Backpressure kontrolliert über die verursachende Kommunikationskette zurückpropagiert werden können:

```text
Producer
   ↑
Service A
   ↑
Service B
   ↑
Service C overloaded
```

Dadurch wird verhindert, dass Überlastung ausschließlich in immer größeren Zwischenqueues verborgen wird.

---

## Integration mit Nova.Call

`Nova.Call` muss Backpressure als normalen Bestandteil seiner Ausführungssemantik behandeln können.

Ein Call kann beispielsweise:

```text
Accepted
Queued
Backpressured
Rejected
Cancelled
DeadlineExceeded
```

werden.

Backpressure ist kein Transportfehler, sondern ein expliziter Zustand der Ressourcen- beziehungsweise Verarbeitungskapazität.

Die konkrete Result- und Wait-Semantik wird in NPSPECs definiert.

---

## Asynchrone Calls

Asynchronität darf nicht mit unbegrenzter Parallelität gleichgesetzt werden.

```text
Async
  ≠
Unlimited Outstanding Work
```

Auch asynchrone Calls unterliegen:

- Queue Limits,
- Concurrency Limits,
- Resource Budgets,
- Deadlines.

Structured Concurrency begrenzt Lebensdauer und Zugehörigkeit der Arbeit; bounded Queues begrenzen deren gepufferte Menge.

---

## Backpressure über Netzwerkgrenzen

Backpressure muss über lokale und entfernte Kommunikationsgrenzen hinweg propagierbar sein.

```text
Local Producer
      ↓
Nova.Call
      ↓
NetworkFlow
      ↓
Remote Service
```

Überlastung eines Remote Services soll nicht ausschließlich durch lokale Sendepuffer verborgen werden.

Transport-, Flow-Control- und Application-Level-Backpressure müssen deshalb miteinander integrierbar sein.

---

## Hierarchische Backpressure

Backpressure kann auf mehreren Ebenen auftreten:

```text
System
  ↓
Execution Domain
  ↓
Service
  ↓
Task Group
  ↓
Nova.Call
  ↓
NetworkFlow
```

Eine lokale Queue darf nicht unabhängig von übergeordneten Resource Budgets unbegrenzt Ressourcen reservieren.

---

## Queue Policy

Der Contract bestimmt, wie eine volle Queue behandelt wird.

Mögliche Strategien umfassen:

- Wait,
- Suspend,
- Reject,
- Drop,
- Replace,
- Coalesce,
- Explicit Degradation.

Nicht jede Strategie ist für jede Datenart zulässig.

Beispielsweise dürfen verlustfreie Operationen nicht stillschweigend Nachrichten verwerfen.

---

## Semantic Types

Queue- und Backpressure-Policies können die semantische Bedeutung der Daten berücksichtigen.

Beispiele:

```text
FinancialTransaction
    → No silent drop

RealtimeVideoFrame
    → Old frame may be obsolete

TelemetrySample
    → Coalescing may be permitted

ControlCommand
    → Explicit delivery semantics required
```

Dadurch können unterschiedliche Datenklassen sinnvoll auf Überlastung reagieren.

Die konkrete Policy bleibt Bestandteil der jeweiligen NPSPEC beziehungsweise des Contracts.

---

## Deadlines

Bounded Queues müssen mit Deadlines integriert werden.

Ein Element, dessen Deadline bereits nicht mehr erreichbar ist, soll nicht unbegrenzt weitere Queue-Ressourcen belegen.

```text
Queued Work
    ↓
Deadline no longer achievable
    ↓
Cancel / Reject / Degrade
```

Die genaue Entscheidung hängt vom Contract ab.

---

## Cancellation

Wird eine Operation abgebrochen, sollen die von ihr belegten Queue- und Backpressure-Ressourcen möglichst früh freigegeben werden.

Cancellation propagiert entlang der Structured-Concurrency- und Call-Kette.

Damit wird verhindert, dass nicht mehr benötigte Arbeit weiterhin Kapazität blockiert.

---

## Resource Economy

Queue Capacity ist eine budgetierte Systemressource.

```text
Queue Capacity
     ↓
Resource Budget
     ↓
Execution Domain / Service / Flow
```

NovaOS muss Queue-Ressourcen in die systemweite Resource Economy integrieren.

Dies umfasst insbesondere:

- Memory,
- Buffer Capacity,
- Outstanding Operations,
- Network Capacity,
- Processing Capacity.

---

## QoS und Prioritäten

Backpressure muss mit QoS und Prioritäten integrierbar sein.

Höhere Priorität darf jedoch nicht automatisch unbegrenzte Queue-Kapazität bedeuten.

```text
Priority
   ≠
Unlimited Resources
```

Realtime- oder Latency-Critical-Flows benötigen explizite Ressourcenreservierungen beziehungsweise geeignete Budgets.

---

## Vermeidung kaskadierender Überlastung

Backpressure dient als Schutz gegen Overload Cascades.

Ohne Backpressure:

```text
Slow Consumer
     ↓
Growing Queue
     ↓
Memory Pressure
     ↓
Scheduler Pressure
     ↓
System Degradation
```

Mit Backpressure:

```text
Slow Consumer
     ↓
Bound Reached
     ↓
Backpressure
     ↓
Producer Rate Reduced
```

Damit wird Überlastung möglichst nahe an ihrer Ursache kontrolliert.

---

## Distributed Failure

Netzwerkpartitionen oder ausgefallene Remote Consumer dürfen nicht zu unbegrenztem lokalen Queue-Wachstum führen.

Ist ein Consumer nicht erreichbar, gelten weiterhin:

- Queue Bounds,
- Deadlines,
- Resource Budgets,
- Retry Policies.

Retries dürfen nicht selbst eine unbeschränkte zweite Queue erzeugen.

---

## Adaptive Steuerung

NovaOS darf Queue Limits und Soft Backpressure Policies innerhalb erlaubter Grenzen adaptiv optimieren.

Beispielsweise können beobachtete:

- Verarbeitungsgeschwindigkeit,
- Latenz,
- Queue Occupancy,
- Network Throughput,
- Prediction Error

zur Optimierung verwendet werden.

Adaptive Systeme dürfen jedoch keine Hard Resource Limits überschreiben.

---

## Deterministic Mode

Im Deterministic Mode müssen Queue Limits und Überlastungsentscheidungen reproduzierbar beziehungsweise explizit kontrolliert sein.

Adaptive Änderungen von Queue-Größen oder Drop-Policies können dabei eingeschränkt oder deaktiviert werden.

---

## Introspection

Bounded Queues und Backpressure müssen kontrolliert introspektierbar sein.

NovaOS soll beispielsweise darstellen können:

```text
Queue
 ├── Capacity
 ├── Current Occupancy
 ├── Resource Owner
 ├── Waiting Producers
 ├── Backpressure State
 └── Overload Policy
```

Über Causation Tracking soll nachvollziehbar sein, welche nachgelagerte Komponente Backpressure ausgelöst hat.

---

## Normative Anforderungen

1. NovaOS MUSS relevante Distributed-Communication-Queues begrenzen.
2. Kommunikationsqueues DÜRFEN NICHT implizit unbegrenzt wachsen.
3. Queue Capacity MUSS als budgetierte Ressource behandelbar sein.
4. Volle Queues MÜSSEN eine explizite Overload Policy besitzen.
5. Backpressure MUSS bis zu verursachenden Producern propagierbar sein.
6. `Nova.Call` MUSS Backpressure als expliziten Kommunikationszustand darstellen können.
7. Asynchrone Kommunikation DARF NICHT unbegrenzte ausstehende Arbeit implizieren.
8. Backpressure MUSS über IPC- und Netzwerkgrenzen integrierbar sein.
9. Queue Limits MÜSSEN mit der systemweiten Resource Economy integrierbar sein.
10. Queue Policies MÜSSEN Semantic Types und Delivery Requirements berücksichtigen können.
11. Verlustfreie Contracts DÜRFEN Daten NICHT stillschweigend verwerfen.
12. Deadlines und Cancellation MÜSSEN mit Queue Management integrierbar sein.
13. QoS und Priorität DÜRFEN keine unbegrenzten Ressourcen erzeugen.
14. Netzwerkfehler und Retries DÜRFEN NICHT zu unbegrenztem Queue-Wachstum führen.
15. Adaptive Queue-Steuerung DARF Hard Resource Limits NICHT überschreiben.
16. Deterministic Mode MUSS dynamische Queue- und Backpressure-Entscheidungen kontrollieren können.
17. Queue-Auslastung, Backpressure und Overload-Entscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- verhindert unkontrolliertes Queue- und Speicherwachstum,
- Überlastung wird bis zum verursachenden Producer sichtbar,
- schützt vor kaskadierenden Systemüberlastungen,
- integriert Distributed Communication in die Resource Economy,
- ermöglicht semantisch passende Overload Policies,
- verbessert Latenzverhalten unter Last,
- verbindet `Nova.Call`, NetworkFlow und Structured Concurrency mit einem gemeinsamen Flow-Control-Modell.

### Negative Konsequenzen

- Producer müssen mit Backpressure umgehen können,
- Queue Limits benötigen sinnvolle Contracts und Policies,
- verteilte Backpressure-Propagation erhöht die Kommunikationskomplexität,
- zu kleine Grenzen können Durchsatz reduzieren, zu große Grenzen erhöhen Latenz und Ressourcenverbrauch.

---

## Verworfene Alternativen

### Unbounded Queues

Verworfen.

Unbounded Queues verschieben Überlastung lediglich in Speicherverbrauch und steigende Latenz.

### Überlastung ausschließlich durch Out-of-Memory behandeln

Verworfen.

OOM ist kein geeignetes Flow-Control-Verfahren und tritt zu spät sowie auf der falschen Abstraktionsebene auf.

### Backpressure ausschließlich auf Transportebene

Verworfen.

Transport Flow Control kennt nicht automatisch die Verarbeitungskapazität des eigentlichen Services oder Consumers.

### Automatisches Dropping bei voller Queue

Verworfen.

Ob Daten verworfen werden dürfen, hängt von ihrer semantischen Bedeutung und dem jeweiligen Contract ab.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-IPC-0006_Backpressure_als_systemweite_IPC_Eigenschaft`
- `ADR-IPC-0007_Causation_und_Tracing_über_IPC_Grenzen`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0002_Location_Transparency_ohne_Kosten_und_Fehler_zu_verbergen`
- `ADR-DISTCOMM-0005_Typed_Schemas_und_versionierte_Serialisierung`
- `ADR-DISTCOMM-0006_Zero_Serialization_Local_Fast_Path`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0015_QoS_und_Traffic_Shaping`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-QUEUE-0001`
- `NPSPEC-DISTCOMM-QUEUE-BOUNDS-0001`
- `NPSPEC-DISTCOMM-BACKPRESSURE-0001`
- `NPSPEC-DISTCOMM-BACKPRESSURE-PROPAGATION-0001`
- `NPSPEC-DISTCOMM-OVERLOAD-POLICY-0001`
- `NPSPEC-DISTCOMM-QUEUE-RESOURCE-0001`
- `NPSPEC-DISTCOMM-BACKPRESSURE-INTROSPECTION-0001`
- `NPSPEC-DISTCOMM-BACKPRESSURE-TEST-0001`

---

## Ergebnis

NovaOS behandelt Queue Capacity und Backpressure als fundamentale Eigenschaften verteilter Kommunikation:

```text
Producer
   ↓
Nova.Call
   ↓
Bounded Queue
   ↓
Consumer
   ↓
Processing Capacity

      ↑
 Backpressure
      ↑
```

Damit wird Überlastung nicht durch unbegrenzte Puffer verborgen, sondern kontrolliert entlang der verursachenden Kommunikationskette zurückpropagiert.

Distributed Communication bleibt dadurch auch unter Last ressourcenbegrenzt, vorhersehbar und mit der systemweiten Resource Economy von NovaOS vereinbar.