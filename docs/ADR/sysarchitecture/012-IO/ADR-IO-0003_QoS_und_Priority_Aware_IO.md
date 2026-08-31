# ADR-IO-0003 – QoS und Priority-Aware I/O

## Status

Angenommen

## Kategorie

I/O / QoS / Priorities / Resource Economy / Scheduling

## Kontext

Nicht alle I/O-Operationen besitzen dieselbe Bedeutung für das System.

Ein interaktiver Audio-Stream, eine Storage-Operation während des Bootvorgangs oder ein Hard-Realtime-Request kann wesentlich strengere Latenzanforderungen besitzen als Background Indexing, Prefetching oder eine große Datenkopie.

Eine reine FIFO-Verarbeitung würde diese Unterschiede ignorieren und könnte kritische I/O-Operationen hinter weniger wichtigen Workloads blockieren.

NovaOS benötigt deshalb ein I/O-Modell, das Quality-of-Service-Anforderungen und Prioritäten systemweit berücksichtigen kann.

---

## Entscheidung

NovaOS macht **QoS und Prioritäten zu nativen Eigenschaften des I/O-Modells**.

I/O Requests können über ihren Execution Contract Anforderungen und Präferenzen an die I/O-Ausführung transportieren.

```text
I/O Request
     ↓
Hard Constraints
     ↓
QoS / Priority
     ↓
Provider Scheduling
     ↓
Completion
```

QoS beschreibt dabei die benötigte Dienstqualität. Priority beeinflusst die relative Bevorzugung konkurrierender Requests.

Beide Konzepte bleiben voneinander getrennt.

---

## Grundprinzip

```text
I/O scheduling follows workload requirements,
not submission order alone.
```

---

## QoS

Ein I/O Request kann Anforderungen an die Dienstqualität besitzen.

Dazu können insbesondere gehören:

- Latency,
- Deadline,
- Throughput,
- Bandwidth,
- Jitter,
- Reliability.

Nicht jeder Provider muss jede QoS-Eigenschaft garantieren können.

Provider müssen ihre relevanten Fähigkeiten über Contracts deklarieren.

---

## Prioritäten

I/O Requests können unterschiedliche Prioritäten besitzen.

Eine höhere Priorität darf bevorzugte Behandlung bewirken, stellt aber für sich allein keine garantierte Deadline oder Realtime-Garantie dar.

```text
Priority ≠ Deadline
Priority ≠ Realtime Guarantee
```

Damit bleiben relative Wichtigkeit und harte zeitliche Anforderungen getrennte Architekturkonzepte.

---

## Prioritätsklassen

NovaOS darf gemeinsame semantische I/O-Klassen bereitstellen, beispielsweise:

```text
Realtime
Latency Critical
Interactive
Normal
Background
Maintenance
```

Die konkrete Anzahl, Kodierung und Zuordnung dieser Klassen wird in den NPSPECs definiert.

Provider dürfen diese Klassen auf ihre eigenen Hardwaremechanismen abbilden.

---

## Hard Constraints vor Priorität

QoS und Priority dürfen keine übergeordneten Hard Constraints umgehen.

Es gilt weiterhin:

```text
Safety
  ↓
Security
  ↓
Data Sovereignty
  ↓
Realtime / Determinism Requirements
  ↓
Resource Constraints
  ↓
QoS / Priority
  ↓
Optimization
```

Ein hochpriorisierter Request darf beispielsweise keine Security- oder Data-Sovereignty-Regel verletzen.

---

## Resource Economy

QoS und Priority werden mit der systemweiten Resource Economy verbunden.

Eine hohe Priorität bedeutet nicht automatisch unbegrenzten Zugriff auf:

- I/O-Bandbreite,
- Queue-Kapazität,
- DMA-Ressourcen,
- Buffer,
- Storage-Bandbreite.

Execution Domains können entsprechende Budgets und Reservierungen besitzen.

---

## Fairness und Starvation

Priority-Aware I/O darf niedrig priorisierte Workloads nicht unbegrenzt verdrängen, sofern keine expliziten Realtime- oder System-Constraints dies erfordern.

Provider Policies müssen daher Fairness und Starvation berücksichtigen können.

Background Work darf verzögert werden, soll aber unter normalen Bedingungen weiterhin Fortschritt erzielen.

---

## Priority Propagation

I/O-Priorität soll aus dem verursachenden Execution Context beziehungsweise Execution Contract ableitbar sein.

```text
Task
 ↓
Execution Contract
 ↓
I/O Request
 ↓
I/O Priority / QoS
```

Dadurch bleibt die Bedeutung eines Workloads über Scheduler-, I/O- und Providergrenzen hinweg erhalten.

Explizite I/O-spezifische Anpassungen dürfen möglich sein, sofern sie die Hard Constraints nicht verletzen.

---

## Provider-Abbildung

Hardware und I/O-Provider besitzen unterschiedliche Möglichkeiten zur Priorisierung.

NovaOS definiert deshalb semantische QoS- und Priority-Anforderungen, nicht ein bestimmtes Hardware-Queue-Modell.

```text
NovaOS QoS
     ↓
Provider Mapping
     ↓
Hardware Queues / Scheduling
```

Kann ein Provider eine geforderte harte Eigenschaft nicht erfüllen, muss dies sichtbar werden.

---

## Realtime I/O

Hard-Realtime-I/O benötigt mehr als eine hohe Priorität.

Entsprechende Requests können reservierte Ressourcen und garantierte Provider-Eigenschaften verlangen.

Ein Provider darf eine Realtime-Garantie nur deklarieren, wenn er sie unter den definierten Bedingungen tatsächlich erfüllen kann.

---

## Adaptive Optimierung

NovaOS darf I/O-Priorisierung und Provider-Policies anhand beobachteter Workloads optimieren.

Prediction Error kann dabei zur Anpassung weicher Policies verwendet werden.

Adaptive Entscheidungen dürfen jedoch niemals:

- Hard Constraints,
- explizite Prioritäten,
- Realtime-Garantien,
- Security Policies

überschreiben.

---

## Normative Anforderungen

1. NovaOS MUSS QoS und Priority als native Eigenschaften des I/O-Modells unterstützen.
2. QoS und Priority MÜSSEN getrennte Konzepte bleiben.
3. I/O Requests MÜSSEN QoS- und Priority-Informationen aus Execution Contracts übernehmen können.
4. Priority DARF NICHT automatisch als Realtime- oder Deadline-Garantie interpretiert werden.
5. Provider MÜSSEN ihre unterstützten QoS-Fähigkeiten deklarieren können.
6. Hard Constraints MÜSSEN vor QoS- und Priority-Optimierungen ausgewertet werden.
7. I/O-Prioritäten DÜRFEN Security- oder Data-Sovereignty-Regeln NICHT umgehen.
8. QoS MUSS in die systemweite Resource Economy integrierbar sein.
9. Hohe Priorität DARF NICHT automatisch unbegrenzte Ressourcen gewähren.
10. Provider Policies SOLLEN Starvation niedriger priorisierter Workloads vermeiden.
11. Priority und QoS SOLLEN über Execution Contexts propagierbar sein.
12. Provider DÜRFEN semantische Prioritäten auf hardwareeigene Queue- und Scheduling-Mechanismen abbilden.
13. Nicht erfüllbare harte QoS-Anforderungen MÜSSEN sichtbar abgelehnt oder als nicht erfüllbar gemeldet werden.
14. Adaptive I/O-Optimierung DARF nur innerhalb zulässiger Soft Policies erfolgen.

---

## Konsequenzen

### Positive Konsequenzen

- interaktive und latenzkritische Workloads erhalten gezieltere I/O-Behandlung,
- Realtime-I/O lässt sich sauber integrieren,
- Background Work kann kontrolliert zurückgestellt werden,
- Scheduler und I/O-System können dieselben Workload-Intentionen berücksichtigen,
- Hardware-QoS-Funktionen können über eine gemeinsame Semantik genutzt werden.

### Negative Konsequenzen

- I/O-Scheduling wird komplexer,
- Provider besitzen unterschiedliche QoS-Fähigkeiten,
- Fairness und Priorität müssen gegeneinander abgewogen werden,
- harte Garantien benötigen reservierbare Ressourcen.

---

## Verworfene Alternativen

### Reines FIFO-I/O

Verworfen.

Es berücksichtigt weder Workload-Wichtigkeit noch Latenz- oder Realtime-Anforderungen.

### Nur numerische Prioritäten

Verworfen.

Eine Zahl allein beschreibt keine Anforderungen an Latenz, Deadline, Bandbreite oder andere QoS-Eigenschaften.

### Jeder Provider definiert eigene QoS-Semantik

Verworfen.

Dies würde systemweite Execution Contracts und einheitliche Workload-Steuerung verhindern.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IO-0001_Completion_Based_Asynchronous_IO`
- `ADR-IO-0002_Einheitliches_IO_Request_Modell`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-SCHED-0010_QoS_und_Latency_Aware_Scheduling`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-IO-QOS-0001`
- `NPSPEC-IO-PRIORITY-0001`
- `NPSPEC-IO-QOS-PROPAGATION-0001`
- `NPSPEC-IO-QOS-PROVIDER-0001`
- `NPSPEC-IO-QOS-RESOURCE-0001`
- `NPSPEC-IO-QOS-TEST-0001`

---

## Ergebnis

NovaOS behandelt I/O nicht als homogene Folge gleichwertiger Requests:

```text
Execution Contract
        ↓
    I/O Request
        ↓
Constraints + QoS + Priority
        ↓
 Provider Scheduling
        ↓
    Completion
```

Die zentrale Architekturregel lautet:

```text
Prioritize by intent.

Guarantee only what can be guaranteed.
```