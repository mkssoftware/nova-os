# ADR-DISTRIBUTED-0005 – Cluster Placement

## Status

Angenommen

## Kategorie

Distributed Architecture / Cluster

## Kontext

NovaOS kann Workloads, Services, Capabilities und Daten über mehrere Systeme verteilen.

Damit benötigt das System ein einheitliches Modell zur Entscheidung, **auf welchen Cluster-Nodes Ressourcen platziert werden**.

Die Platzierung muss technische Anforderungen sowie Security, Trust, Data Sovereignty, Verfügbarkeit und Ressourcen berücksichtigen.

## Entscheidung

NovaOS verwendet **Constraint-basiertes Cluster Placement**.

```text
Placement Request
       ↓
Requirements
       ↓
Candidate Nodes
       ↓
Constraint Filtering
       ↓
Placement Policy
       ↓
Selected Node(s)
```

Placement bestimmt den zulässigen Standort einer Ressource innerhalb eines Clusters.

## Placement Objects

Placement kann unter anderem gelten für:

```text
Service
Capability Provider
Workload
Replica
Data Object
Storage Resource
```

Die logische Identität des Objekts bleibt von seiner Platzierung unabhängig.

## Hard Constraints

Vor jeder Platzierung werden verpflichtende Constraints geprüft.

Beispiele:

```text
Security
Trust
Data Sovereignty
Required Hardware
Required Capability
Realtime
Resource Requirements
Locality
```

Ein Node, der einen Hard Constraint nicht erfüllt, darf nicht ausgewählt werden.

## Soft Constraints

Innerhalb der gültigen Nodes können Optimierungsziele verwendet werden.

Beispiele:

```text
Load
Latency
Energy
Data Locality
Network Cost
Preferred Hardware
User Preference
```

Soft Constraints dürfen Hard Constraints nicht überschreiben.

## Ressourcen

Placement muss die tatsächliche Ressourcenverfügbarkeit berücksichtigen.

```text
Node
 ├── CPU
 ├── Memory
 ├── GPU / NPU
 ├── Storage
 └── Network
```

Reservierte Ressourcen dürfen nicht gleichzeitig anderen Placements zugesichert werden.

## Affinity und Anti-Affinity

Placement kann Beziehungen zwischen Ressourcen berücksichtigen.

```text
Affinity:
    A near B

Anti-Affinity:
    A separate from B
```

Damit können beispielsweise zusammengehörige Komponenten nahe beieinander oder redundante Replikate auf getrennten Failure Domains platziert werden.

## Failure Domains

Redundante Ressourcen sollen nicht unnötig innerhalb derselben Failure Domain platziert werden.

```text
Replica A → Node 1 / Failure Domain A
Replica B → Node 2 / Failure Domain B
```

Dadurch soll ein einzelner Fehler nicht alle Replikate gleichzeitig betreffen.

## Rebalancing

Ändert sich der Clusterzustand, kann Placement neu bewertet werden.

```text
Node Failure
Node Added
Resource Pressure
Policy Change
      ↓
Replanning
```

Eine Migration erfolgt nur, wenn ihr Nutzen die geltenden Kosten, Risiken und Contracts rechtfertigt.

## Placement und Scheduling

Placement und Scheduling sind getrennte Entscheidungen.

```text
Placement:
Where may it exist?

Scheduling:
Where and when should work execute?
```

Scheduling darf nur innerhalb der durch Placement und Contracts zulässigen Möglichkeiten arbeiten.

## Normative Anforderungen

1. NovaOS MUSS Cluster Placement Constraint-basiert durchführen können.
2. Placement MUSS Hard Constraints vor Optimierungszielen berücksichtigen.
3. Security-, Trust- und Data-Sovereignty-Regeln DÜRFEN nicht verletzt werden.
4. Ressourcenverfügbarkeit MUSS bei Placement berücksichtigt werden.
5. Affinity und Anti-Affinity MÜSSEN ausdrückbar sein.
6. Failure Domains MÜSSEN bei redundanten Placements berücksichtigt werden können.
7. Placement MUSS von der logischen Object Identity getrennt bleiben.
8. Änderungen des Clusterzustands MÜSSEN Replanning ermöglichen können.
9. Rebalancing DARF keine geltenden Contracts verletzen.
10. Placement und Scheduling MÜSSEN als getrennte, koordinierte Mechanismen behandelt werden.

## Konsequenzen

### Positive Konsequenzen

- Ressourcen können systematisch über Cluster verteilt werden,
- Security und Data Sovereignty bleiben harte Grenzen,
- Redundanz kann Failure Domains berücksichtigen,
- Last, Datenlokalität und Hardware können optimiert werden.

### Negative Konsequenzen

- Placement benötigt aktuelle Informationen über Nodes und Ressourcen,
- Rebalancing und Migration erhöhen die Komplexität.

## Abhängigkeiten

- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0002_Distributed_Storage`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`
- `ADR-DISTRIBUTED-0004_Remote_Capabilities`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-RESILIENCE-0003_Hierarchical_Failure_Domains`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-CLUSTER-PLACEMENT-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-CONSTRAINTS-0001`
- `NPSPEC-DISTRIBUTED-AFFINITY-0001`
- `NPSPEC-DISTRIBUTED-REBALANCING-0001`

## Ergebnis

NovaOS platziert Ressourcen anhand ihrer Anforderungen und des tatsächlichen Clusterzustands:

```text
Resource
   ↓
Placement Requirements
   ↓
Hard Constraints
   ↓
Valid Nodes
   ↓
Soft Constraints
   ↓
Placement
   ↓
Verification
```

Der zentrale Grundsatz lautet:

```text
Eine Ressource wird nicht dort platziert,
wo gerade Platz vorhanden ist.

Sie wird dort platziert,
wo ihre Contracts sicher erfüllt werden können.
```