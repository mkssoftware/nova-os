# ADR-DISTRIBUTED-0006 – Failure Domain Aware Placement

## Status

Angenommen

## Kategorie

Distributed Architecture / Resilience / Placement

## Kontext

NovaOS kann redundante Services, Workloads, Capabilities und Daten über mehrere Cluster-Nodes verteilen.

Mehrere Replikate bieten jedoch keinen ausreichenden Schutz, wenn sie innerhalb derselben Failure Domain liegen.

```text
Node A ─┐
Node B ─┼─ Same Power Supply
Node C ─┘
```

Ein einzelner Fehler könnte dadurch mehrere Replikate gleichzeitig betreffen.

## Entscheidung

NovaOS verwendet **Failure Domain Aware Placement**.

Redundante Ressourcen werden soweit durch ihre Contracts gefordert über voneinander unabhängige Failure Domains verteilt.

```text
Replica A → Failure Domain A
Replica B → Failure Domain B
Replica C → Failure Domain C
```

## Failure Domains

Failure Domains können hierarchisch organisiert sein.

```text
Region
  ↓
Site
  ↓
Rack
  ↓
Host
  ↓
Device
```

Weitere Failure Domains können beispielsweise sein:

```text
Power Supply
Network Path
Storage Controller
Physical Device
Availability Zone
```

## Placement

Vor der Platzierung wird geprüft, welche Failure Domains bereits durch andere Replikate belegt sind.

```text
Replica Set
    ↓
Required Separation
    ↓
Candidate Nodes
    ↓
Failure Domain Check
    ↓
Placement
```

## Redundanz

Die erforderliche Trennung wird durch den jeweiligen Contract bestimmt.

Beispiel:

```text
Replicas = 3
Minimum Failure Domains = 3
Failure Domain Level = Host
```

NovaOS darf nicht automatisch annehmen, dass jede Ressource maximale geografische Verteilung benötigt.

## Gemeinsame Fehlerursachen

Physisch unterschiedliche Nodes gelten nicht automatisch als unabhängige Failure Domains.

```text
Node A ─┐
        ├─ Shared Storage
Node B ─┘
```

Gemeinsam genutzte Infrastruktur muss bei der Bewertung berücksichtigt werden können.

## Degraded Placement

Sind nicht genügend unabhängige Failure Domains verfügbar, kann der gewünschte Zustand möglicherweise nicht vollständig erreicht werden.

```text
Desired:
    3 independent replicas

Available:
    2 failure domains

State:
    Degraded
```

NovaOS darf eine solche Platzierung nicht als vollständig redundant melden.

## Rebalancing

Wird später eine geeignete Failure Domain verfügbar, kann Reconciliation eine bessere Platzierung herstellen.

```text
New Failure Domain
       ↓
Reconciliation
       ↓
Replica Migration
       ↓
Verified Placement
```

Unnötige Migrationen sollen vermieden werden.

## Security und Data Sovereignty

Failure-Domain-Verteilung darf keine höheren Constraints verletzen.

```text
Failure Domain Diversity
          ↓
Security
Data Sovereignty
Trust
          ↓
Valid Placement
```

Geografische Verteilung ist nur zulässig, wenn die geltenden Policies dies erlauben.

## Normative Anforderungen

1. NovaOS MUSS Failure Domains bei redundanten Placements berücksichtigen können.
2. Failure Domains MÜSSEN hierarchisch modellierbar sein.
3. Redundante Ressourcen MÜSSEN gemäß ihrem Contract über unabhängige Failure Domains verteilt werden können.
4. Unterschiedliche Nodes DÜRFEN nicht automatisch als unabhängige Failure Domains gelten.
5. Gemeinsame Infrastruktur MUSS als mögliche gemeinsame Fehlerursache modellierbar sein.
6. Nicht erfüllbare Failure-Domain-Anforderungen MÜSSEN als Degraded oder unerfüllt sichtbar bleiben.
7. Reconciliation MUSS verbesserte Platzierung ermöglichen können.
8. Rebalancing SOLL unnötige Migrationen vermeiden.
9. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN Vorrang behalten.
10. Eine Platzierung DARF erst nach Verification als ausreichend redundant gelten.

## Konsequenzen

### Positive Konsequenzen

- Redundanz schützt besser vor gemeinsamen Fehlerursachen,
- Cluster-Ausfälle können gezielter toleriert werden,
- Storage und Services verwenden ein gemeinsames Failure-Domain-Modell,
- tatsächliche Redundanz wird von bloßer Replikatanzahl getrennt.

### Negative Konsequenzen

- Infrastrukturabhängigkeiten müssen bekannt sein,
- strenge Failure-Domain-Anforderungen können mögliche Placements reduzieren.

## Abhängigkeiten

- `ADR-DISTRIBUTED-0002_Distributed_Storage`
- `ADR-DISTRIBUTED-0005_Cluster_Placement`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-RESILIENCE-0003_Hierarchical_Failure_Domains`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-FAILURE-DOMAIN-0001`
- `NPSPEC-DISTRIBUTED-FAILURE-DOMAIN-PLACEMENT-0001`
- `NPSPEC-DISTRIBUTED-FAILURE-DOMAIN-TOPOLOGY-0001`

## Ergebnis

NovaOS bewertet Redundanz anhand tatsächlicher Fehlergrenzen und nicht nur anhand der Anzahl vorhandener Replikate:

```text
Replica Requirements
        ↓
Failure Domain Topology
        ↓
Constraint Solving
        ↓
Independent Placement
        ↓
Verification
```

Der zentrale Grundsatz lautet:

```text
Mehrere Replikate sind nur dann
wirklich redundant,

wenn ein einzelner Fehler
sie nicht gemeinsam ausschalten kann.
```