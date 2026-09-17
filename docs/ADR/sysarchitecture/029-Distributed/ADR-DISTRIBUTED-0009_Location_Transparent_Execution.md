# ADR-DISTRIBUTED-0009 – Location Transparent Execution

## Status

Angenommen

## Kategorie

Distributed Architecture / Execution

## Kontext

NovaOS kann Capabilities lokal oder auf entfernten Systemen ausführen.

Consumer sollen nicht unnötig wissen müssen, auf welchem physischen Gerät, Node oder Prozessor eine Operation ausgeführt wird.

Gleichzeitig dürfen reale Unterschiede wie Latenz, Security, Datenlokalität oder Fehlersemantik nicht verborgen werden.

## Entscheidung

NovaOS unterstützt **Location Transparent Execution**.

```text
Consumer
   ↓
Capability / Intent
   ↓
Execution Contract
   ↓
Resolver
   ↓
Local or Remote Provider
   ↓
Execution
```

Der physische Ausführungsort ist grundsätzlich eine Eigenschaft der Systemplanung und nicht der fachlichen Operation.

## Einheitliches Execution Model

Lokale und entfernte Ausführung verwenden dasselbe logische Modell.

```text
Typed Input
    ↓
Capability
    ↓
Execution Contract
    ↓
Provider
    ↓
Typed Output
```

Consumer benötigen keine separate Remote-API, sofern keine explizite Kontrolle des Ausführungsorts erforderlich ist.

## Location Constraints

Der Execution Contract kann den möglichen Ausführungsort einschränken.

Beispiele:

```text
LocalOnly
Specific Device
Trusted Cluster
Required Region
Data Locality
Required Hardware
```

Location Transparency darf solche Constraints nicht umgehen.

## Provider Resolution

Der Resolver kann lokale und entfernte Provider gemeinsam betrachten.

```text
Capability Requirement
        ↓
Candidate Providers
   ┌────┼────┐
 Local Remote
   └────┼────┘
        ↓
Constraint Solving
        ↓
Selected Provider
```

Nur Provider, die alle Hard Constraints erfüllen, dürfen ausgewählt werden.

## Datenlokalität

Der Standort der benötigten Daten muss bei der Planung berücksichtigt werden.

```text
Data Location
      +
Execution Candidates
      ↓
Placement Decision
```

NovaOS soll unnötige Datenbewegungen vermeiden.

## Sichtbare Kosten

Location Transparency bedeutet nicht, dass Kosten unsichtbar werden.

Relevante Eigenschaften müssen über Contracts und Telemetrie sichtbar bleiben.

Beispiele:

```text
Latency
Bandwidth
Transfer Cost
Energy
Availability
Trust
```

## Fehlersemantik

Remote Execution besitzt zusätzliche Fehlerzustände.

```text
Network Failure
Provider Unreachable
Timeout
Remote Failure
Unknown Completion State
```

Diese dürfen nicht als lokale Fehler abstrahiert werden, wenn dadurch die Operationssemantik verändert würde.

## Explizite Location

Consumer können den Ausführungsort explizit einschränken, wenn dies fachlich notwendig ist.

```text
Intent
  +
Location Constraint
  ↓
Execution Planning
```

Dies bleibt eine Contract-Anforderung und keine separate Ausführungsarchitektur.

## Migration und Replanning

Ändert sich die Verfügbarkeit eines Providers, kann NovaOS die Ausführung neu planen.

```text
Provider Lost
     ↓
Replanning
     ↓
Alternative Provider
```

Dies ist nur zulässig, wenn Operationssemantik und Execution Contract dies erlauben.

## Normative Anforderungen

1. NovaOS MUSS lokale und entfernte Ausführung über ein gemeinsames Execution Model unterstützen können.
2. Consumer SOLLEN nicht unnötig an physische Ausführungsorte gekoppelt werden.
3. Location Constraints MÜSSEN explizit ausdrückbar sein.
4. Provider Resolution MUSS lokale und entfernte Provider gemeinsam bewerten können.
5. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN eingehalten werden.
6. Datenlokalität SOLL bei der Ausführungsplanung berücksichtigt werden.
7. Location Transparency DARF relevante Kosten nicht verbergen.
8. Location Transparency DARF Remote-Fehlersemantik nicht verfälschen.
9. Explizite Location-Anforderungen MÜSSEN möglich bleiben.
10. Replanning DARF Operationssemantik und Execution Contract nicht verletzen.

## Konsequenzen

### Positive Konsequenzen

- Consumer bleiben unabhängig vom physischen Ausführungsort,
- lokale und entfernte Provider können austauschbar sein,
- spezialisierte Hardware kann transparent genutzt werden,
- Distributed Execution bleibt Teil des normalen Capability-Modells.

### Negative Konsequenzen

- Resolver und Planner benötigen Informationen über entfernte Provider,
- Latenz, Netzwerkfehler und Datenbewegung müssen weiterhin explizit modelliert werden.

## Abhängigkeiten

- `ADR-ARCH-0007_Location_Transparency`
- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0003_Distributed_Scheduling`
- `ADR-DISTRIBUTED-0004_Remote_Capabilities`
- `ADR-DISTRIBUTED-0005_Cluster_Placement`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-DISTRIBUTED-LOCATION-TRANSPARENCY-0001`
- `NPSPEC-DISTRIBUTED-EXECUTION-LOCATION-0001`
- `NPSPEC-DISTRIBUTED-LOCATION-CONSTRAINTS-0001`

## Ergebnis

NovaOS trennt die fachliche Ausführung vom physischen Ausführungsort:

```text
Intent
  ↓
Capability
  ↓
Execution Contract
  ↓
Constraint Solving
  ↓
Local / Remote Provider
  ↓
Execution
  ↓
Verified Result
```

Der zentrale Grundsatz lautet:

```text
Der Consumer beschreibt,
was ausgeführt werden soll.

Wo es ausgeführt wird,
entscheidet NovaOS innerhalb der geltenden Contracts.
```