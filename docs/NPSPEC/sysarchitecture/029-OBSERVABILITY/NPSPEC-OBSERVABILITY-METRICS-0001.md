# NPSPEC-OBSERVABILITY-METRICS-0001 – Nova Observability Metrics

## Status

Angenommen

## Kategorie

Observability / Metrics / System Measurement

## Zweck

NovaOS definiert ein systemweites Metrics-Modell zur quantitativen Beobachtung von Kernel, Ressourcen, Diensten, Anwendungen und verteilten Komponenten.

Metrics liefern messbare Zustands- und Leistungswerte, ohne mit Logging, Tracing oder Auditing gleichgesetzt zu werden.

```text
System State
    ↓
Measurement
    ↓
Metric Sample
    ↓
Aggregation
    ↓
Observation / Analysis
```

## Grundprinzipien

```text
Metric ≠ Log
Metric ≠ Trace
Metric ≠ Audit
Metric ≠ Event

Measurement ≠ Guarantee
Metric Value ≠ Policy Decision
Missing Metric ≠ Zero
Unknown ≠ Healthy
```

## Metric Model

Eine Metrik wird beschrieben durch:

```text
Metric
├── MetricID
├── MetricType
├── ValueType
├── Unit
└── Source
```

Optional:

```text
Timestamp
Labels
ObjectID
ResourceID
ExecutionID
ProviderID
NodeID
Quality
Aggregation
Security Label
```

## Metric Types

NovaOS unterstützt mindestens:

```text
Counter
Gauge
Histogram
Distribution
Rate
Duration
State
```

Beispiele:

```text
CPU.Utilization
Memory.Available
IO.Latency
Network.BytesReceived
Scheduler.QueueDepth
Execution.Duration
Storage.Errors
Thermal.Temperature
Energy.Consumption
```

## Units

Einheiten müssen eindeutig definiert sein.

Beispiele:

```text
Bytes
BytesPerSecond
Nanoseconds
Milliseconds
Percent
Joules
Watts
Celsius
Count
```

Einheiten dürfen nicht ausschließlich aus dem Namen der Metrik abgeleitet werden.

## Sampling

Metrics können:

```text
Periodic
EventDriven
OnDemand
Continuous
```

erfasst werden.

Die Sampling-Frequenz muss an Kosten und benötigte Genauigkeit angepasst werden können.

## Aggregation

Messwerte können aggregiert werden:

```text
Minimum
Maximum
Average
Sum
Count
Percentile
Rate
Distribution
```

Aggregation darf die ursprüngliche Bedeutung einer Metrik nicht verändern.

## Scope

Metrics können unterschiedliche Ebenen beschreiben:

```text
System
Node
Process
Task
Execution
Provider
Resource
Object
Device
Cluster
```

Der Scope muss eindeutig identifizierbar sein.

## Labels

Metrics können strukturierte Labels besitzen.

Beispiel:

```text
Metric: IO.Latency

Labels:
├── DeviceID
├── OperationType
└── ExecutionClass
```

Unkontrolliert hochdimensionale Labels sollen vermieden werden.

## Cardinality

NovaOS muss Metric Cardinality begrenzen können.

Problematische Labels können beispielsweise sein:

```text
Random IDs
Full Paths
Arbitrary User Input
Unbounded Object Names
```

Hohe Cardinality darf nicht zu unkontrolliertem Speicherverbrauch führen.

## Resource Accounting

Metrics können Daten des Resource Accounting verwenden.

```text
Resource Accounting
        ↓
Metrics
        ↓
Observation
```

Dabei bleiben beide Konzepte getrennt:

```text
Accounting = authoritative usage tracking
Metrics = observation and analysis
```

Metrics dürfen nicht automatisch als abrechnungs- oder sicherheitskritische Wahrheit behandelt werden.

## Execution Metrics

Execution Contracts können relevante Messwerte erzeugen:

```text
ExecutionID
├── Duration
├── CPU Usage
├── Memory Usage
├── IO Usage
├── Network Usage
└── Energy Usage
```

Dadurch können geplante und tatsächliche Ressourcennutzung verglichen werden.

## Latency Metrics

Latenzen sollen als Verteilungen erfasst werden können.

```text
Latency
├── Minimum
├── Average
├── P50
├── P95
├── P99
└── Maximum
```

Ein einzelner Durchschnittswert reicht für latenzkritische Systeme häufig nicht aus.

## Distributed Metrics

Jeder Node kann lokale Metrics erfassen.

```text
Node A ─┐
Node B ─┼→ Aggregation
Node C ─┘
```

Lokale Funktionsfähigkeit darf nicht von einem zentralen Metrics-System abhängen.

## Time

Messwerte benötigen eine definierte Zeitbasis.

NovaOS muss berücksichtigen:

```text
Clock Drift
Clock Synchronization
Monotonic Time
Wall Clock Time
Sampling Interval
```

Für Dauer- und Latenzmessungen soll monotone Zeit verwendet werden.

## Missing Data

Fehlende Messwerte müssen explizit behandelbar sein.

```text
Missing
Unknown
Unavailable
Stale
```

Dabei gilt:

```text
Missing ≠ 0
Unknown ≠ Normal
Stale ≠ Current
```

## Metric Quality

Messwerte können Qualitätsinformationen besitzen:

```text
Exact
Estimated
Sampled
Derived
Stale
Incomplete
```

Dadurch können adaptive Komponenten die Zuverlässigkeit der Daten berücksichtigen.

## Adaptive Optimization

NovaOS kann Metrics für Optimierung verwenden:

```text
Metrics
   ↓
Analysis
   ↓
Scheduler / Placement / Resource Policy
```

Beispiele:

```text
Load Balancing
Energy Optimization
Thermal Management
Predictive Scheduling
Placement
Resource Reclaim
```

Metrics dürfen Hard Constraints nicht überschreiben.

## Security

Zugriff auf Metrics muss capabilitybasiert kontrollierbar sein.

Beispiele:

```text
MetricsRead
MetricsConfigure
MetricsExport
MetricsReset
```

Systemweite Metrics können Informationen über andere Security Domains offenlegen und müssen entsprechend geschützt werden.

## Privacy

Metrics dürfen keine unnötigen personenbezogenen oder sensiblen Daten enthalten.

Insbesondere hochdimensionale Labels können unbeabsichtigt Informationen offenlegen.

Es gelten:

```text
Data Minimization
Security Labels
Retention
Controlled Export
```

## Resource Limits

Metrics selbst verbrauchen Ressourcen.

Daher müssen begrenzbar sein:

```text
Sampling Rate
Buffer Size
Cardinality
History
Aggregation Cost
Export Rate
```

Observability darf nicht zum dominanten System-Workload werden.

## Retention

Historische Metrics unterliegen einer Retention Policy.

```text
Raw Samples
     ↓
Aggregation
     ↓
Long-Term Metrics
     ↓
Expiration
```

Ältere Daten können verdichtet werden, sofern die Policy dies erlaubt.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Available Metrics
Metric Definitions
Current Values
Sampling Rate
Units
Aggregation
Cardinality
Dropped Samples
Metric Quality
Buffer Usage
Retention State
Collector Health
```

## Normative Anforderungen

1. NovaOS MUSS ein strukturiertes systemweites Metrics-Modell bereitstellen.
2. Metrics MÜSSEN von Logging, Tracing und Auditing getrennt bleiben.
3. Jede Metrik MUSS eine eindeutige Bedeutung und Einheit besitzen.
4. Metrics MÜSSEN unterschiedliche Scopes unterstützen können.
5. Sampling-Frequenzen MÜSSEN kontrollierbar sein.
6. NovaOS MUSS Aggregation und Verteilungen unterstützen können.
7. Metric Cardinality MUSS begrenzbar sein.
8. Fehlende Werte DÜRFEN NICHT automatisch als `0` interpretiert werden.
9. Veraltete Werte DÜRFEN NICHT als aktuelle Messwerte dargestellt werden.
10. Metric Quality SOLL explizit darstellbar sein.
11. Latenzmessungen SOLLEN Verteilungen und Perzentile unterstützen.
12. Distributed Metrics DÜRFEN keine permanente zentrale Infrastruktur voraussetzen.
13. Metrics DÜRFEN NICHT automatisch als authoritative Resource Accounting behandelt werden.
14. Adaptive Optimierung DARF Hard Constraints NICHT aufgrund von Metrics überschreiben.
15. Zugriff auf sensible Metrics MUSS capabilitybasiert kontrolliert werden.
16. Metrics MÜSSEN Privacy- und Retention-Policies berücksichtigen.
17. Ressourcenverbrauch der Metrics-Infrastruktur MUSS begrenzbar sein.
18. Metrics- und Collector-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-RESOURCE-IO-0001`
- `NPSPEC-RESOURCE-NETWORK-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-RESOURCE-THERMAL-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-CLUSTER-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-RETENTION-0001`
- `ADR-ARCH-0071`

## Ergebnis

```text
System Measurements
        ↓
Structured Metrics
        ↓
Bounded Sampling
        ↓
Aggregation
        ↓
Correlation
        ↓
Analysis
        ↓
Diagnostics + Optimization
```

NovaOS erhält damit eine einheitliche und ressourcenbegrenzte Metrics-Grundlage, mit der Systemzustand, Performance, Ressourcenverbrauch, Latenz, Energie und verteilte Ausführung quantitativ beobachtet werden können, ohne Metrics mit Logging, Accounting oder verbindlichen Systemgarantien gleichzusetzen.