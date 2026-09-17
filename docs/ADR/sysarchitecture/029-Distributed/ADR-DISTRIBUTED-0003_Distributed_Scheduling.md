# ADR-DISTRIBUTED-0003 – Distributed Scheduling

## Status

Angenommen

## Kategorie

Distributed Architecture / Scheduling

## Kontext

Mit Distributed Execution kann NovaOS Aufgaben auf lokalen oder entfernten Systemen ausführen.

Damit benötigt NovaOS einen einheitlichen Mechanismus, der entscheidet, **wann und wo** eine ausführbare Arbeit eingeplant wird.

Dabei müssen Ressourcen, Datenlokalität, Trust, Security, Realtime-Anforderungen und Execution Contracts berücksichtigt werden.

## Entscheidung

NovaOS unterstützt **Distributed Scheduling** als Erweiterung des lokalen Scheduling-Modells.

```text
Work
 ↓
Execution Contract
 ↓
Candidate Nodes
 ↓
Constraint Filtering
 ↓
Scheduling
 ↓
Execution
```

Der Scheduler plant Arbeit auf geeigneten Execution Providern, ohne die Semantik der Operation zu verändern.

## Scheduling Domain

Ein Scheduling Domain beschreibt die Menge der für eine Aufgabe grundsätzlich erreichbaren Execution Provider.

```text
Scheduling Domain
 ├── Local CPU
 ├── Local GPU
 ├── Remote Node A
 └── Remote Node B
```

Erreichbarkeit allein bedeutet nicht, dass ein Provider verwendet werden darf.

## Candidate Selection

Mögliche Execution Provider werden zunächst anhand ihrer Capabilities bestimmt.

```text
Required Capability
        ↓
Available Providers
        ↓
Candidates
```

Danach werden unzulässige Kandidaten durch Constraints ausgeschlossen.

## Constraints

Distributed Scheduling muss insbesondere berücksichtigen:

```text
Safety
Security
Data Sovereignty
Trust
Realtime / Deadline
Determinism
Resource Budget
Required Hardware
Data Locality
Availability
```

Hard Constraints dürfen nicht zugunsten einer besseren Auslastung verletzt werden.

## Datenlokalität

Scheduling und Distributed Storage müssen gemeinsam planbar sein.

```text
Work
 +
Data Location
 +
Available Compute
 ↓
Scheduling Decision
```

Wenn sinnvoll, soll Arbeit zu den Daten verschoben werden, statt große Datenmengen unnötig zu übertragen.

## Ressourcen

Der Scheduler kann verfügbare Ressourcen verschiedener Systeme berücksichtigen.

Beispiele:

```text
CPU
GPU
NPU
Memory
Storage
Network
Energy
```

Reservierte oder nicht verfügbare Ressourcen dürfen nicht als frei behandelt werden.

## Realtime und Deadlines

Eine entfernte Ausführung ist nur zulässig, wenn die geforderten Timing Contracts eingehalten werden können.

```text
Deadline
   ↓
Local Provider  → Valid
Remote Provider → Invalid
```

Unsichere Netzwerkbedingungen dürfen nicht als garantierte Realtime-Eigenschaft behandelt werden.

## Failures

Ein geplanter Provider kann vor oder während der Ausführung ausfallen.

```text
Scheduled Provider
       ↓
Failure
       ↓
Execution State
       ↓
Replanning
```

Replanning oder Migration ist nur zulässig, wenn Operationssemantik und Execution Contract dies erlauben.

## Load Balancing

Load Balancing ist eine Policy innerhalb der Menge gültiger Scheduling-Entscheidungen.

```text
Valid Providers
      ↓
Load / Cost / Energy
      ↓
Selection
```

Auslastungsoptimierung darf keine Hard Constraints überschreiben.

## Determinismus

Im Deterministic Mode müssen Scheduling-Entscheidungen soweit erforderlich reproduzierbar sein.

Adaptive oder zufällige Verteilung darf deterministische Anforderungen nicht verletzen.

## Normative Anforderungen

1. NovaOS MUSS Distributed Scheduling für verteilte Execution Provider unterstützen können.
2. Scheduling MUSS auf Execution Contracts und Capabilities basieren.
3. Hard Constraints MÜSSEN vor Optimierungszielen berücksichtigt werden.
4. Security-, Trust- und Data-Sovereignty-Regeln DÜRFEN nicht verletzt werden.
5. Datenlokalität SOLL bei Scheduling-Entscheidungen berücksichtigt werden.
6. Ressourcenverfügbarkeit MUSS vor der Einplanung geprüft werden.
7. Realtime- und Deadline-Anforderungen MÜSSEN bei Remote Scheduling berücksichtigt werden.
8. Provider-Ausfälle MÜSSEN Replanning ermöglichen können.
9. Replanning DARF die Operationssemantik nicht verletzen.
10. Load Balancing MUSS als Policy unterhalb der Hard Constraints behandelt werden.
11. Deterministic Mode MUSS reproduzierbare Scheduling-Entscheidungen ermöglichen können.

## Konsequenzen

### Positive Konsequenzen

- lokale und entfernte Rechenressourcen können gemeinsam geplant werden,
- spezialisierte Hardware kann systemweit genutzt werden,
- Datenlokalität kann unnötige Transfers reduzieren,
- Last kann über mehrere Systeme verteilt werden.

### Negative Konsequenzen

- Scheduling wird durch Netzwerk- und Partial-Failure-Zustände komplexer,
- Ressourcen- und Providerzustände müssen ausreichend aktuell beobachtbar sein.

## Abhängigkeiten

- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0002_Distributed_Storage`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-SCHEDULING-0001`
- `NPSPEC-DISTRIBUTED-SCHEDULING-DOMAIN-0001`
- `NPSPEC-DISTRIBUTED-SCHEDULING-REPLAN-0001`
- `NPSPEC-DISTRIBUTED-LOAD-BALANCING-0001`

## Ergebnis

NovaOS betrachtet verfügbare Rechenressourcen als gemeinsamen Scheduling-Raum:

```text
Work
 ↓
Execution Contract
 ↓
Capability Candidates
 ↓
Constraint Solving
 ↓
Locality / Resources / Load
 ↓
Scheduling Decision
 ↓
Execution Provider
```

Der zentrale Grundsatz lautet:

```text
Nicht der Standort entscheidet,
wo eine Aufgabe ausgeführt wird.

Die Anforderungen der Aufgabe
bestimmen den geeigneten Ausführungsort.
```