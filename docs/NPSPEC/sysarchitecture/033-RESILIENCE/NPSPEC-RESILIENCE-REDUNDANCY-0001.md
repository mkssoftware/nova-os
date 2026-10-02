# NPSPEC-RESILIENCE-REDUNDANCY-0001 – Nova Resilience Redundancy

## Status

Angenommen

## Kategorie

Resilience / Redundancy / Availability

## Zweck

NovaOS definiert ein systemweites Redundanzmodell, bei dem kritische Funktionen, Daten, Provider oder Ressourcen mehrfach verfügbar sein können, sodass der Ausfall einer Instanz nicht zwangsläufig zum Verlust der jeweiligen Systemfähigkeit führt.

```text
Capability
├── Instance A
├── Instance B
└── Instance C
```

Redundanz bildet die Grundlage für Failover, erhöht jedoch nur dann die Resilienz, wenn redundante Instanzen nicht denselben relevanten Fehlerursachen unterliegen.

## Grundprinzipien

```text
Redundancy ≠ Resilience
Redundancy ≠ Replication
Redundancy ≠ Backup
Redundancy ≠ Failover
Multiple Instances ≠ Independent Instances
Available ≠ Healthy
Different Provider ≠ Different Failure Domain
```

## Redundancy Model

```text
RedundancyGroup
├── GroupID
├── CapabilityID
├── Members
├── RequiredCapacity
├── RedundancyPolicy
├── FailureDomains
└── State
```

Optional:

```text
MinimumHealthyMembers
ActiveMembers
StandbyMembers
Criticality
ConsistencyPolicy
FailoverPolicy
PlacementPolicy
ExecutionContractID
ProvenanceID
```

## Redundancy Types

NovaOS soll mindestens unterstützen können:

```text
Active / Passive
Active / Active
N+1
N+M
Replicated
Diverse
Path Redundancy
Device Redundancy
Provider Redundancy
Node Redundancy
```

## Active / Passive

```text
Primary → Active
Secondary → Standby
```

Die Standby-Instanz übernimmt bei einem kontrollierten Failover.

## Active / Active

```text
Instance A ─┐
Instance B ─┼→ Workload
Instance C ─┘
```

Mehrere Instanzen können gleichzeitig Arbeit übernehmen.

Konsistenz und Koordination müssen explizit geregelt sein.

## N+M Redundancy

```text
Required Capacity = N
Additional Capacity = M
```

Der Verlust von bis zu `M` geeigneten Ressourcen kann kompensiert werden, sofern verbleibende Ressourcen die benötigten Guarantees erfüllen.

## Failure-Domain Diversity

Redundanz soll über unabhängige Failure Domains verteilt werden.

```text
Replica A → Failure Domain A
Replica B → Failure Domain B
Replica C → Failure Domain C
```

Nicht ausreichend wäre beispielsweise:

```text
Replica A ─┐
Replica B ─┼→ Same Device
Replica C ─┘
```

Der gemeinsame Ausfallpfad muss im Redundanzmodell sichtbar bleiben.

## Common-Mode Failures

NovaOS muss gemeinsame Fehlerursachen berücksichtigen.

Beispiele:

```text
Same Hardware
Same Driver
Same Firmware
Same Storage
Same Network Path
Same Power Domain
Same Provider
Same Software Defect
```

Redundante Instanzen können dadurch gleichzeitig ausfallen.

## Diversity

Für kritische Systeme kann funktionale oder technische Diversität eingesetzt werden.

```text
Provider A → Implementation A
Provider B → Implementation B
```

Diversität kann Common-Mode-Risiken reduzieren, erzeugt jedoch zusätzliche Komplexität und muss validiert werden.

## Capacity

Redundanz muss nicht nur Instanzanzahl, sondern verbleibende Kapazität berücksichtigen.

```text
Failure
   ↓
Remaining Capacity
   ↓
Required Capacity
```

```text
Enough Instances ≠ Enough Capacity
```

## Health

Jedes Mitglied besitzt einen eigenen Health State.

```text
Healthy
Degraded
Recovering
Failed
Unavailable
Unknown
```

Der Gruppenstatus darf unsichere oder degradierte kritische Mitglieder nicht verdecken.

## Redundancy State

Eine Gruppe kann beispielsweise sein:

```text
Healthy
Reduced
Degraded
Critical
Unavailable
Unknown
```

Der Verlust von Redundanz kann bereits `Degraded` bedeuten, obwohl die Capability weiterhin verfügbar ist.

## Failover Integration

Redundanz stellt Alternativen bereit.

Failover entscheidet über deren Aktivierung.

```text
Redundancy
    ↓
Alternative Available
    ↓
Failover
```

Vor Aktivierung müssen alternative Mitglieder validiert werden.

## Replication Integration

Bei Daten und zustandsbehafteten Services kann Redundanz Replication verwenden.

Zu berücksichtigen sind:

```text
Freshness
Consistency
Integrity
Durability
Version
Transaction State
```

```text
Replica Exists ≠ Replica Usable
```

## Placement

NovaOS soll redundante Instanzen anhand ihrer Failure Domains platzieren können.

```text
Placement
   ↓
Failure-Domain Diversity
   ↓
Reduced Correlated Risk
```

Adaptive Placement darf Hard Constraints nicht verletzen.

## Degraded Redundancy

Fällt ein Mitglied aus:

```text
3 Healthy
   ↓ failure
2 Healthy
   ↓
Reduced Redundancy
```

NovaOS kann anschließend:

```text
Repair
Replace
Rebuild
Rebalance
Re-replicate
```

initiieren.

## Redundancy Restoration

Nach Verlust redundanter Kapazität soll die gewünschte Redundanz kontrolliert wiederhergestellt werden.

```text
Desired Redundancy
        ↓
Actual Redundancy
        ↓
Difference
        ↓
Reconciliation
```

Dies folgt dem deklarativen NovaOS-Systemmodell.

## Security und Trust

Redundante Provider müssen unabhängig validiert werden.

```text
Trusted Primary
      ≠
Automatically Trusted Secondary
```

Jedes Mitglied muss relevante Anforderungen an:

```text
Capability
Security
Trust
Sovereignty
Integrity
```

erfüllen.

## Realtime

Redundanz für Realtime-Systeme muss zeitliche Garantien berücksichtigen.

```text
Primary Failure
      ↓
Failover Time
      +
Alternative Latency
      ↓
Deadline
```

Eine verfügbare Alternative ist nicht automatisch realtimefähig.

## Resource Economy

Redundanz verursacht zusätzliche Kosten:

```text
CPU
Memory
Storage
Network
Energy
Synchronization
```

Redundanzgrad soll deshalb entsprechend Criticality und Guarantees festgelegt werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
GroupID
Members
Member Health
Failure Domains
Required Capacity
Available Capacity
Redundancy Level
Redundancy State
Common Dependencies
Active Failover
Restoration State
```

## Normative Anforderungen

1. NovaOS MUSS redundante Ressourcen und Provider explizit modellieren können.
2. Redundancy MUSS von Replication, Backup und Failover getrennt bleiben.
3. Redundante Mitglieder MÜSSEN individuelle Health States besitzen.
4. Failure Domains der Mitglieder MÜSSEN bestimmbar sein.
5. Unterschiedliche Instanzen DÜRFEN NICHT automatisch als unabhängige Failure Domains gelten.
6. Common-Mode Failures MÜSSEN berücksichtigt werden können.
7. Redundanz SOLL möglichst über unabhängige Failure Domains verteilt werden.
8. Redundanz MUSS benötigte Kapazität berücksichtigen können.
9. Instanzanzahl DARF NICHT mit verfügbarer Kapazität gleichgesetzt werden.
10. Verlust von Redundanz MUSS als degradierter Zustand darstellbar sein.
11. Alternative Mitglieder MÜSSEN vor Failover validiert werden.
12. Replicas MÜSSEN hinsichtlich Freshness, Integrity und Consistency validierbar sein.
13. Redundanzgrad SOLL anhand von Criticality und Guarantees definierbar sein.
14. Verlorene Redundanz SOLL automatisch rekonstruierbar sein.
15. Wiederherstellung der Redundanz MUSS Resource Budgets respektieren.
16. Jedes redundante Mitglied MUSS relevante Security-, Trust- und Sovereignty-Anforderungen erfüllen.
17. Realtime-Redundanz MUSS zeitliche Garantien berücksichtigen.
18. Adaptive Placement-Entscheidungen DÜRFEN Hard Constraints NICHT verletzen.
19. Redundanz DARF NICHT als Ersatz für Backup oder Recovery betrachtet werden.
20. Redundanzzustand und Failure-Domain-Verteilung MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILOVER-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-DISTRIBUTED-REPLICATION-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `ADR-ARCH-0121`

## Ergebnis

```text
Required Capability
       ↓
Redundancy Group
├── Member A → Failure Domain A
├── Member B → Failure Domain B
└── Member C → Failure Domain C
       ↓
Member Failure
       ↓
Reduced Redundancy
       ↓
Failover
       ↓
Continue Operation
       ↓
Restore Redundancy
```

NovaOS erhält damit ein systemweites Redundanzmodell, das nicht nur mehrere Instanzen bereitstellt, sondern deren Failure Domains, Kapazität, Health, gemeinsame Abhängigkeiten und Garantien berücksichtigt und damit eine belastbare Grundlage für Failover und automatische Wiederherstellung schafft.