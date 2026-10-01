# NPSPEC-OBSERVABILITY-RESOURCE-0001 – Nova Resource Observability

## Status

Angenommen

## Kategorie

Observability / Resources / Resource Analysis

## Zweck

NovaOS definiert ein einheitliches Observability-Modell für die Beobachtung von Ressourcenverbrauch, Verfügbarkeit, Auslastung und Engpässen.

```text
Resources
   ↓
Observation
   ↓
Metrics + Events
   ↓
Correlation
   ↓
Resource State
```

Resource Observability ergänzt Resource Accounting und Resource Economy, ersetzt diese jedoch nicht.

## Grundprinzipien

```text
Observation ≠ Accounting
Usage ≠ Ownership
Available ≠ Reserved
Measured ≠ Guaranteed
High Utilization ≠ Overload
Low Utilization ≠ Waste
Missing Data ≠ Zero
Unknown ≠ Healthy
```

## Resource Observation Model

Eine beobachtete Ressource wird beschrieben durch:

```text
ResourceObservation
├── ResourceID
├── ResourceType
├── Timestamp
├── State
└── Measurements
```

Optional:

```text
ExecutionID
TaskID
ProcessID
ProviderID
NodeID
Owner
Reservation
Budget
Limits
Pressure
Quality
```

## Beobachtbare Ressourcen

NovaOS soll mindestens folgende Ressourcen beobachten können:

```text
CPU
Memory
Storage
IO
Network
GPU
NPU
Energy
Thermal
```

Weitere Resource Types können über das Semantic Resource Model ergänzt werden.

## CPU

Beispiele:

```text
Utilization
Runnable Tasks
Scheduler Delay
Core Utilization
Frequency
CPU Time
Interrupt Load
```

## Memory

Beispiele:

```text
Used
Available
Reserved
Committed
Cache
Pressure
Reclaim
Swap
Compressed Memory
NUMA Distribution
```

## IO und Storage

Beispiele:

```text
Throughput
Operations
Queue Depth
Latency
Capacity
Free Space
Error Rate
Device Utilization
```

## Network

Beispiele:

```text
Bandwidth
Throughput
Latency
Packet Loss
Queue State
Connection Count
Congestion
```

## GPU und NPU

Beispiele:

```text
Utilization
Memory Usage
Queue Depth
Execution Time
Transfer Volume
Energy Usage
Thermal State
```

## Energy und Thermal

Beispiele:

```text
Power
Energy Consumption
Temperature
Thermal Pressure
Throttling
Battery State
```

Diese Werte können Resource Economy und Scheduling unterstützen.

## Resource Pressure

NovaOS muss Ressourcenknappheit explizit darstellen können.

```text
Normal
Elevated
High
Critical
Unknown
```

Pressure darf nicht ausschließlich aus einem einzelnen Messwert abgeleitet werden müssen.

## Budget Correlation

Resource Observability kann tatsächliche Nutzung mit Resource Budgets vergleichen.

```text
Execution Budget
      ↓
Actual Usage
      ↓
Comparison
```

Beispiel:

```text
CPU Budget     → 200 ms
Observed Usage → 160 ms
```

Die Messung ersetzt nicht das authoritative Resource Accounting.

## Reservation Correlation

Beobachtung muss unterscheiden können zwischen:

```text
Available
Allocated
Reserved
Used
Reclaimable
```

```text
Reserved ≠ Used
Available ≠ Unallocated
```

## Execution Correlation

Ressourcenverbrauch kann mit Ausführungen korreliert werden.

```text
ExecutionID
├── CPU
├── Memory
├── IO
├── Network
├── GPU
└── Energy
```

Dadurch können tatsächliche Ausführungskosten analysiert werden.

## Distributed Resources

Resource Observability funktioniert über mehrere Nodes hinweg.

```text
Cluster
├── Node A → Resource State
├── Node B → Resource State
└── Node C → Resource State
```

Ein zentraler Collector darf keine Voraussetzung für lokale Ressourcenverwaltung sein.

## Data Freshness

Resource Observations besitzen einen zeitlichen Gültigkeitsbereich.

```text
Current
PossiblyStale
Stale
Unavailable
Unknown
```

```text
Old Measurement ≠ Current State
```

Placement oder Scheduling dürfen veraltete Daten nicht uneingeschränkt als aktuelle Ressourcenlage behandeln.

## Measurement Quality

Messwerte können klassifiziert werden als:

```text
Exact
Estimated
Sampled
Derived
Incomplete
```

Die Qualität soll bei automatisierten Entscheidungen berücksichtigt werden können.

## Adaptive Optimization

Resource Observability kann Eingaben liefern für:

```text
Scheduling
Placement
Load Balancing
Resource Reclaim
Energy Optimization
Thermal Management
Provider Selection
Predictive Preloading
```

Dabei gilt:

```text
Observation
   ↓
Policy Decision
```

Observability selbst trifft keine Policy-Entscheidung.

## Predictive Resource Observation

NovaOS kann historische Messwerte verwenden, um zukünftigen Ressourcenbedarf abzuschätzen.

```text
History
   ↓
Prediction
   ↓
Expected Resource Demand
```

Vorhersagen müssen als solche gekennzeichnet bleiben.

```text
Prediction ≠ Guarantee
```

## Security

Ressourceninformationen können sensible Systeminformationen offenlegen.

Zugriff muss capabilitybasiert kontrollierbar sein.

Beispiele:

```text
ResourceObserveSelf
ResourceObserveProcess
ResourceObserveSystem
ResourceObserveCluster
ResourceObserveConfigure
```

## Privacy

Resource Observability soll möglichst technische Ressourceninformationen erfassen und unnötige personenbezogene Daten vermeiden.

Es gelten:

```text
Data Minimization
Security Labels
Retention
Controlled Export
```

## Resource Cost

Observability verbraucht selbst Ressourcen.

Begrenzbar müssen sein:

```text
Sampling Frequency
History
Buffer Size
Metric Cardinality
Aggregation Cost
Export Bandwidth
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ResourceID
Resource Type
Current Usage
Capacity
Availability
Reservation
Pressure
Budget
Limits
Measurement Quality
Data Freshness
Execution Correlation
Node Distribution
Observation Health
```

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Resource-Observability-Modell bereitstellen.
2. Resource Observability MUSS von Resource Accounting und Resource Economy getrennt bleiben.
3. CPU, Memory, Storage, IO, Network, GPU, NPU, Energy und Thermal SOLLEN beobachtbar sein.
4. Ressourcenmessungen MÜSSEN über stabile `ResourceID`s korrelierbar sein.
5. `Available`, `Reserved`, `Allocated` und `Used` MÜSSEN unterscheidbar sein.
6. Resource Pressure MUSS explizit darstellbar sein.
7. Fehlende Messwerte DÜRFEN NICHT als `0` interpretiert werden.
8. Veraltete Messwerte DÜRFEN NICHT als aktuelle Ressourcenlage dargestellt werden.
9. Measurement Quality SOLL explizit darstellbar sein.
10. Resource Usage SOLL mit ExecutionIDs und Resource Budgets korrelierbar sein.
11. Distributed Resource Observation DARF keine permanente zentrale Infrastruktur voraussetzen.
12. Observability-Daten DÜRFEN authoritative Resource Accounting NICHT ersetzen.
13. Adaptive Optimierung DARF Hard Constraints NICHT aufgrund beobachteter oder vorhergesagter Werte überschreiben.
14. Vorhersagen MÜSSEN von gemessenen Werten unterscheidbar sein.
15. Zugriff auf sensible Ressourceninformationen MUSS capabilitybasiert kontrolliert werden.
16. Ressourcenverbrauch der Observability-Infrastruktur MUSS begrenzbar sein.
17. Privacy- und Retention-Policies MÜSSEN berücksichtigt werden.
18. Resource Observation State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-PROFILING-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-RESOURCE-IO-0001`
- `NPSPEC-RESOURCE-NETWORK-0001`
- `NPSPEC-RESOURCE-GPU-0001`
- `NPSPEC-RESOURCE-NPU-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-RESOURCE-THERMAL-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-DISTRIBUTED-CLUSTER-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `ADR-ARCH-0075`

## Ergebnis

```text
System Resources
       ↓
Resource Observation
       ↓
Usage + Capacity + Pressure
       ↓
Execution + Budget Correlation
       ↓
Local + Distributed Analysis
       ↓
Scheduling / Placement / Optimization
```

NovaOS erhält damit eine einheitliche Resource-Observability-Schicht, die Ressourcen vom einzelnen Kernel-Subsystem bis zum gesamten Cluster sichtbar und analysierbar macht, ohne Beobachtungsdaten mit Accounting, Reservierungen oder garantierter Ressourcenverfügbarkeit gleichzusetzen.