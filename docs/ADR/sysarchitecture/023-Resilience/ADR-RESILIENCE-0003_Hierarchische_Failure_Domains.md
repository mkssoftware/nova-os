# ADR-RESILIENCE-0003 – Hierarchische Failure Domains

## Status

Angenommen

## Kategorie

Resilience Architecture / Failure Isolation / Recovery Scope

## Kontext

Fehler in NovaOS können auf sehr unterschiedlichen Ebenen auftreten.

Ein Fehler in einem einzelnen Task darf nicht automatisch einen kompletten Dienst, ein Subsystem oder das gesamte System beeinträchtigen. Gleichzeitig können bestimmte Fehler tatsächlich größere Bereiche betreffen und eine Recovery auf höherer Ebene erfordern.

Nova.Resilience benötigt deshalb eine explizite Struktur, mit der Fehler räumlich und logisch eingegrenzt und Recovery-Maßnahmen auf den kleinsten geeigneten Bereich beschränkt werden können.

## Entscheidung

NovaOS organisiert ausfallrelevante Systemkomponenten in **hierarchischen Failure Domains**.

```text
System
  ↓
Subsystem
  ↓
Service
  ↓
Workload
  ↓
Task
```

Eine Failure Domain definiert einen Bereich, innerhalb dessen Fehler zunächst erkannt, isoliert und behandelt werden sollen.

Recovery beginnt grundsätzlich auf der kleinsten betroffenen Failure Domain und wird nur bei Bedarf nach oben eskaliert.

## Grundprinzip

```text
Failure Scope
     ↓
Smallest Safe Recovery Domain
     ↓
Escalate only if necessary
```

Ein lokaler Fehler soll lokal bleiben.

## Failure Domains

Failure Domains können unterschiedliche Systemstrukturen abbilden.

Beispiele:

```text
System
Subsystem
Service
Execution Domain
Workload
Task
Driver
Device
Capability Provider
Storage Domain
Distributed Node
```

Die konkrete Hierarchie muss nicht für alle Komponenten identisch sein.

Entscheidend ist, dass über- und untergeordnete Failure Domains eindeutig bestimmt werden können.

## Isolation

Failure Domains definieren Grenzen für die Fehlerausbreitung.

```text
Failure Domain A
      │
      X
      │
Failure Domain B
```

Isolation kann unter anderem durch folgende Mechanismen unterstützt werden:

- getrennte Address Spaces,
- Sandboxing,
- Capability Boundaries,
- Resource Budgets,
- Structured Concurrency,
- transaktionale Zustände,
- IPC-Grenzen,
- Storage Isolation.

## Hierarchische Recovery

Recovery wird zunächst innerhalb der betroffenen Failure Domain versucht.

```text
Task Failure
    ↓
Task Recovery
    ↓ failed
Workload Recovery
    ↓ failed
Service Recovery
    ↓ failed
Subsystem Recovery
```

Eine Eskalation auf eine höhere Ebene erfolgt nur, wenn die niedrigere Ebene den gültigen Systemzustand nicht sicher wiederherstellen kann.

## Parent- und Child-Domains

Failure Domains können hierarchisch verschachtelt sein.

```text
Parent Failure Domain
        │
        ├── Child A
        ├── Child B
        └── Child C
```

Der Ausfall einer Child Domain bedeutet nicht automatisch den Ausfall ihrer Parent Domain oder ihrer Geschwister.

Ein Fehler der Parent Domain kann dagegen mehrere untergeordnete Domains betreffen.

## Fault Propagation

Fehlerpropagation muss explizit modellierbar sein.

Nova.Resilience muss unterscheiden können zwischen:

```text
Contained Failure
Propagated Failure
Shared Dependency Failure
Parent Domain Failure
Cascading Failure
```

Damit können scheinbar unabhängige Fehler auf eine gemeinsame Ursache zurückgeführt werden.

## Shared Dependencies

Gemeinsam genutzte Ressourcen können mehrere Failure Domains gleichzeitig beeinflussen.

Beispiele:

```text
Shared Storage
Shared Device
Shared Service
Memory Pressure
Network Path
Capability Provider
```

Solche Abhängigkeiten müssen in der Resilience-Topologie sichtbar sein, damit Nova.Resilience den tatsächlichen Failure Scope bestimmen kann.

## Recovery und Structured Concurrency

Structured Concurrency kann natürliche Failure Domains innerhalb von Task-Hierarchien bilden.

```text
Task Group
   ├── Task A
   ├── Task B
   └── Task C
```

Fehler und Cancellation können dadurch kontrolliert innerhalb ihrer strukturierten Ausführungshierarchie behandelt werden.

## Distributed Failure Domains

Failure Domains dürfen nicht auf einen einzelnen Rechner beschränkt sein.

```text
Distributed Service
      │
      ├── Node A
      ├── Node B
      └── Node C
```

Nova.Resilience muss lokale und verteilte Failure Domains unter demselben grundlegenden Modell behandeln können.

Location Transparency darf physische Failure Boundaries nicht unsichtbar machen.

## Failure Domain und Recovery Policy

Jede Failure Domain kann eine passende Recovery Policy besitzen.

```text
Failure Domain
      ↓
Failure Classification
      ↓
Recovery Policy
      ↓
Recovery Action
```

Eine übergeordnete Policy kann Grenzen für untergeordnete Recovery-Strategien definieren.

## Blast Radius

Nova.Resilience soll den erwarteten **Blast Radius** einer Recovery berücksichtigen.

Zwischen mehreren möglichen Recovery-Aktionen soll grundsätzlich diejenige bevorzugt werden, die den gültigen Zustand mit möglichst geringer Beeinträchtigung nicht betroffener Domains wiederherstellt.

```text
Task Restart
<
Workload Restart
<
Service Restart
<
Subsystem Restart
<
System Restart
```

Diese Reihenfolge ist keine starre Regel; Correctness und Sicherheit haben Vorrang.

## Normative Anforderungen

1. NovaOS MUSS hierarchische Failure Domains unterstützen.
2. Failure Domains MÜSSEN eindeutig referenzierbar sein.
3. Parent- und Child-Beziehungen MÜSSEN darstellbar sein.
4. Recovery MUSS grundsätzlich auf der kleinsten sicheren betroffenen Failure Domain beginnen.
5. Recovery DARF nur bei Bedarf auf übergeordnete Failure Domains eskalieren.
6. Der Ausfall einer Child Domain DARF nicht automatisch als Ausfall ihrer Geschwister interpretiert werden.
7. Shared Dependencies MÜSSEN bei der Bestimmung des Failure Scope berücksichtigt werden können.
8. Fault Propagation MUSS über Domain-Grenzen erkennbar sein können.
9. Cascading Failures MÜSSEN als solche klassifizierbar sein.
10. Failure Domains MÜSSEN mit Recovery Policies verknüpfbar sein.
11. Structured-Concurrency-Hierarchien SOLLEN als natürliche Failure Domains nutzbar sein.
12. Lokale und verteilte Failure Domains MÜSSEN unter demselben grundlegenden Modell darstellbar sein.
13. Location Transparency DARF relevante physische Failure Boundaries nicht verbergen.
14. Recovery Planning SOLL den erwarteten Blast Radius berücksichtigen.
15. Security-, Trust-, Integrity- und Data-Sovereignty-Grenzen DÜRFEN durch Recovery-Eskalation nicht umgangen werden.
16. Failure Domains, Eskalationen und Recovery-Ergebnisse MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Fehler können gezielt lokal isoliert werden,
- unnötige System- oder Subsystem-Neustarts werden vermieden,
- Recovery erhält einen klar definierten Eskalationspfad,
- Cascading Failures und gemeinsame Fehlerursachen können besser erkannt werden,
- Structured Concurrency und Resilience greifen ineinander,
- dasselbe Modell funktioniert lokal und verteilt.

### Negative Konsequenzen

- Failure-Domain-Beziehungen müssen gepflegt werden,
- gemeinsam genutzte Ressourcen erschweren die Bestimmung des tatsächlichen Failure Scope,
- Recovery Planning muss Abhängigkeiten zwischen Domains berücksichtigen.

## Verworfene Alternativen

### Systemweite Recovery bei jedem schwereren Fehler

Verworfen, da dadurch der Blast Radius unnötig vergrößert würde.

### Vollständig unabhängige Failure Domains ohne Hierarchie

Verworfen, da dadurch Eskalation und gemeinsame Fehlerursachen schwer modellierbar wären.

### Failure Domains ausschließlich anhand von Prozessen

Verworfen, da NovaOS auch Tasks, Dienste, Geräte, Provider, Subsysteme und verteilte Komponenten berücksichtigen muss.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-ARCH-0002_Declarative_System_Model`
- `ADR-ARCH-0009_Structured_Concurrency_systemweit`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-SECURITY-0012_Sandboxing_als_standardisierte_Isolationsschicht`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-FAILURE-DOMAIN-0001`
- `NPSPEC-RESILIENCE-FAILURE-DOMAIN-HIERARCHY-0001`
- `NPSPEC-RESILIENCE-FAULT-PROPAGATION-0001`
- `NPSPEC-RESILIENCE-SHARED-DEPENDENCY-0001`
- `NPSPEC-RESILIENCE-RECOVERY-ESCALATION-0001`
- `NPSPEC-RESILIENCE-BLAST-RADIUS-0001`
- `NPSPEC-RESILIENCE-FAILURE-DOMAIN-TEST-0001`

## Ergebnis

Nova.Resilience strukturiert Fehlergrenzen hierarchisch:

```text
System
  ↓
Subsystem
  ↓
Service
  ↓
Workload
  ↓
Task
```

Recovery beginnt auf der kleinsten sicher behandelbaren Failure Domain und eskaliert nur dann auf eine höhere Ebene, wenn der gültige Zustand lokal nicht wiederhergestellt werden kann.

Dadurch wird der **Blast Radius von Fehlern und Recovery-Maßnahmen systematisch minimiert**, ohne Correctness, Security oder Datenintegrität zugunsten lokaler Verfügbarkeit aufzugeben.