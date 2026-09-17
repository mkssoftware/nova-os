# ADR-COMPUTE-0007 – Distributed Compute

## Status

Angenommen

## Kategorie

Compute Architecture / Distributed Execution

## Kontext

NovaOS kann Compute-Ressourcen nicht nur auf dem lokalen Gerät, sondern auch auf anderen vertrauenswürdigen Systemen nutzen.

```text
Local CPU
Local GPU
Remote CPU
Remote GPU
Cluster
Compute Node
```

Distributed Compute soll dabei kein separates Programmiermodell bilden, sondern die Unified Compute Runtime erweitern.

## Entscheidung

NovaOS integriert **Distributed Compute als regulären Bestandteil der Unified Compute Runtime**.

Remote Compute-Ressourcen werden als Compute Provider behandelt.

```text
Compute Request
      ↓
Unified Compute Runtime
      ↓
Provider Resolution
      ↓
Local / Remote Providers
      ↓
Execution
```

Der Consumer muss grundsätzlich nicht wissen, auf welchem physischen System die Berechnung ausgeführt wird.

## Distributed Compute Provider

Remote Systeme können Compute Provider anbieten.

```text
Remote Node
 ├── CPU Provider
 ├── GPU Provider
 ├── NPU Provider
 └── Accelerator Provider
```

Diese Provider verwenden dieselben grundlegenden Compute Contracts wie lokale Provider.

## Provider Discovery

Remote Compute Provider werden über die Capability- und Provider-Discovery gefunden.

```text
Compute Capability
       ↓
Provider Discovery
       ↓
Local Providers
Remote Providers
       ↓
Candidate Set
```

Discovery bedeutet nicht automatisch, dass ein Provider verwendet werden darf.

## Constraint Filtering

Remote Provider müssen alle relevanten Hard Constraints erfüllen.

Beispiele:

```text
Capability Compatibility
Security
Trust
Authorization
Data Sovereignty
Determinism
Latency / Deadline
Required Hardware
Resource Budget
```

Erst danach dürfen Optimierungskriterien angewendet werden.

## Datenlokalität

Distributed Compute und Distributed Storage müssen gemeinsam geplant werden können.

```text
Data Location
      +
Compute Provider
      ↓
Placement Decision
```

NovaOS soll bevorzugt Compute zu den Daten bringen, wenn dadurch unnötige Datenbewegungen vermieden werden.

## Netzwerk

Netzwerkressourcen sind Bestandteil der Compute-Kosten.

```text
Remote Compute Cost =
    Transfer
  + Network Latency
  + Queueing
  + Execution
  + Result Transfer
```

Eine leistungsfähige Remote GPU ist nicht automatisch sinnvoller als eine langsamere lokale CPU.

## Parallelisierung

Ein Compute Workload kann auf mehrere Provider verteilt werden, wenn seine Semantik dies erlaubt.

```text
Compute Workload
      ↓
Partitioning
   ┌──┼──┐
Node A Node B Node C
   └──┼──┘
      ↓
Result Composition
```

Die Runtime darf Workloads nicht automatisch aufteilen, wenn dadurch Semantik oder Determinismus verändert werden.

## Failure Handling

Remote Compute besitzt zusätzliche Fehlerzustände.

```text
Provider Unreachable
Network Failure
Timeout
Remote Failure
Node Failure
Unknown Completion State
```

Ein Verbindungsverlust bedeutet nicht automatisch, dass die Berechnung nicht ausgeführt wurde.

`Unknown` muss deshalb von `Failed` unterschieden werden.

## Replanning

Fällt ein Remote Provider aus, kann NovaOS einen anderen kompatiblen Provider wählen.

```text
Remote Provider A
       ↓
Failure
       ↓
Replanning
       ↓
Local Provider
        oder
Remote Provider B
```

Eine erneute Ausführung ist nur zulässig, wenn Operationssemantik und Compute Contract dies erlauben.

## Sovereignty und Trust

Remote Compute unterliegt den gleichen Sicherheitsgrenzen wie andere verteilte Ausführungen.

```text
Remote Provider
      ↓
Sovereignty
      ↓
Trust
      ↓
Authorization
      ↓
Compute Contract
      ↓
Execution
```

Performance darf diese Constraints niemals überschreiben.

## Normative Anforderungen

1. NovaOS MUSS Remote Compute Provider über die Unified Compute Runtime integrieren können.
2. Lokale und Remote Provider SOLLEN dasselbe logische Compute-Modell verwenden.
3. Remote Provider MÜSSEN ihre Compute Capabilities deklarieren.
4. Discovery DARF nicht mit Authorization gleichgesetzt werden.
5. Trust und Data Sovereignty MÜSSEN als Hard Constraints behandelt werden.
6. Netzwerk-, Transfer- und Latenzkosten MÜSSEN bei Distributed Compute berücksichtigt werden.
7. Datenlokalität SOLL bei der Provider-Auswahl berücksichtigt werden.
8. Workload Partitioning DARF nur bei geeigneter Operationssemantik erfolgen.
9. Remote Failure States MÜSSEN explizit modelliert werden.
10. `Unknown` MUSS von `Failed` unterschieden werden.
11. Replanning und Retry DÜRFEN nur erfolgen, wenn Operationssemantik und Contract dies erlauben.
12. Distributed Compute DARF lokale Compute Provider nicht als separates Ausführungsmodell behandeln.

## Konsequenzen

### Positive Konsequenzen

- lokale und entfernte Rechenressourcen bilden einen gemeinsamen Compute Pool,
- leistungsfähige Remote Hardware kann transparent genutzt werden,
- Compute kann näher an Daten platziert werden,
- Cluster und zukünftige Compute-Infrastruktur lassen sich integrieren.

### Negative Konsequenzen

- Netzwerk und Remote Failures erhöhen die Planungsunsicherheit,
- Trust, Sovereignty und Datenlokalität müssen in die Compute-Auswahl einbezogen werden.

## Abhängigkeiten

- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0002_CPU_GPU_NPU_DSP_FPGA_als_Compute_Provider`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-COMPUTE-0005_Energy_Aware_Compute`
- `ADR-COMPUTE-0006_Latency_Aware_Compute`
- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`
- `ADR-DISTRIBUTED-0010_Remote_Execution_unter_Sovereignty_und_Trust_Constraints`

## Zugehörige NPSPECs

- `NPSPEC-COMPUTE-DISTRIBUTED-0001`
- `NPSPEC-COMPUTE-REMOTE-PROVIDER-0001`
- `NPSPEC-COMPUTE-DISTRIBUTED-PARTITIONING-0001`
- `NPSPEC-COMPUTE-DISTRIBUTED-FAILURE-0001`

## Ergebnis

NovaOS erweitert das lokale Compute-Modell über Systemgrenzen hinweg:

```text
Compute Request
      ↓
Unified Compute Runtime
      ↓
Contract + Constraints
      ↓
Provider Resolution
      ↓
Local / Remote / Cluster
      ↓
Execution
      ↓
Verified Result
```

Der zentrale Grundsatz lautet:

```text
Distributed Compute ist kein
separates Rechenmodell.

Entfernte Rechenressourcen sind
weitere Provider derselben
Unified Compute Runtime.
```