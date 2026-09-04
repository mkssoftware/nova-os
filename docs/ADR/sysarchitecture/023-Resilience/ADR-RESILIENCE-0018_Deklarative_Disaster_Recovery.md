# ADR-RESILIENCE-0018 – Deklarative Disaster Recovery

## Status

Angenommen

## Kategorie

Resilience Architecture / Disaster Recovery / Declarative System Model

## Kontext

Lokale Recovery reicht nicht aus, wenn ganze Systeme, Standorte, Storage-Domains oder zentrale Abhängigkeiten ausfallen.

Disaster Recovery wird in klassischen Architekturen häufig als Sammlung separater Runbooks, Skripte und manueller Wiederanlaufpläne umgesetzt.

Für NovaOS passt dieses Modell nicht zum deklarativen Systemansatz.

Nova.Resilience benötigt deshalb eine Disaster-Recovery-Architektur, die aus dem gewünschten Systemzustand, den Abhängigkeiten und den Recovery Contracts ableitbar ist.

## Entscheidung

NovaOS definiert **Disaster Recovery deklarativ**.

Ein System oder Subsystem beschreibt seinen gewünschten wiederherstellbaren Zustand einschließlich:

```text
Components
Dependencies
Required Data
Recovery Order
RTO
RPO
Required Capabilities
Trust Requirements
Data Sovereignty
Redundancy Requirements
```

Nova.Resilience erzeugt daraus einen Recovery Plan.

```text
Declarative Recovery Model
          ↓
Observed Disaster State
          ↓
Recovery Planning
          ↓
Ordered Recovery Actions
          ↓
Verification
          ↓
Restored Desired State
```

## Grundprinzip

```text
Describe the state to restore,
not a fixed sequence of shell commands.
```

Der Recovery Plan wird aus dem aktuellen Zustand und dem deklarativen Zielzustand abgeleitet.

## Disaster Scope

Ein Disaster kann größere Failure Domains betreffen, beispielsweise:

```text
Subsystem
Machine
Storage Domain
Cluster
Site
Region
Distributed Service
```

Nova.Resilience bestimmt den tatsächlich betroffenen Scope und die noch verfügbaren Ressourcen.

## Declarative Recovery Model

Das Recovery Model beschreibt, welche Bedingungen nach einer erfolgreichen Wiederherstellung gelten müssen.

Beispiel:

```text
Service A
    requires Storage B
    requires Capability C
    RTO = 30 s
    RPO = 0

Storage B
    requires Replica available
    sovereignty = EU
```

Die konkrete Syntax wird in NPSPECs definiert.

## Dependency Graph

Recovery-Reihenfolgen werden aus Abhängigkeiten abgeleitet.

```text
Storage
   ↓
Identity
   ↓
Service
   ↓
Application
```

NovaOS darf Komponenten nicht allein anhand einer statischen Startreihenfolge wiederherstellen.

Tatsächliche Dependencies und aktuell verfügbare Provider sind maßgeblich.

## Alternative Ressourcen

Der deklarative Ansatz ermöglicht alternative Recovery-Wege.

```text
Required Capability
      ↓
Provider A unavailable
      ↓
Provider B available
```

Alternative Provider, Storage-Replikate oder Compute-Ressourcen dürfen verwendet werden, wenn sie alle Hard Constraints erfüllen.

## Recovery Contracts

RTO und RPO sind Bestandteil der Disaster-Recovery-Planung.

```text
Recovery Model
     +
RTO / RPO
     ↓
Recovery Strategy
```

Ein Plan, der die erforderlichen Recovery Contracts nicht erfüllen kann, muss dies explizit anzeigen und gegebenenfalls eskalieren oder einen zulässigen Degraded Mode wählen.

## Data Recovery

Disaster Recovery muss zwischen:

```text
Configuration
Runtime State
Persistent Data
Secrets
Identity State
Trust State
```

unterscheiden.

Nicht jeder Zustand wird auf dieselbe Weise wiederhergestellt.

Semantic Checkpoints, Replikation, Backups und rekonstruierbarer Zustand können gemeinsam verwendet werden.

## Security und Trust

Ein Disaster darf nicht dazu führen, dass Sicherheitsanforderungen für schnellere Recovery abgeschwächt werden.

Wiederhergestellte Komponenten müssen erneut relevante Anforderungen erfüllen:

```text
Identity
Integrity
Trust
Capabilities
Attestation
Secret Release
```

Ein historisch vertrauenswürdiger Zustand darf nicht ungeprüft übernommen werden.

## Data Sovereignty

Disaster Recovery darf Daten nicht auf beliebige verfügbare Infrastruktur verschieben.

```text
Recovery Candidate
      ↓
Sovereignty Check
      ↓
Eligible / Rejected
```

Storage-, Processing- und Transit-Constraints bleiben auch im Disaster-Fall Hard Constraints.

## Secrets

Secrets sollen nach Möglichkeit nicht Bestandteil gewöhnlicher Recovery-Backups sein.

Nach Wiederherstellung einer Workload können Secrets über den Credential Broker oder geschützte Secret-Systeme erneut bereitgestellt werden.

Workload Attestation kann Voraussetzung für Secret Release sein.

## Progressive Restore

Bei großen Ausfällen darf NovaOS schrittweise wiederherstellen.

```text
Critical Core
     ↓ Verify
Required Services
     ↓ Verify
Applications
     ↓ Verify
Optional Services
```

Damit kann zunächst ein minimal funktionsfähiger und verifizierter Systemzustand hergestellt werden.

## Degraded Recovery

Kann der vollständige Desired State nicht rechtzeitig oder überhaupt wiederhergestellt werden, kann ein deklarativ erlaubter Degraded State verwendet werden.

```text
Desired State
   ├── Normal
   └── Allowed Degraded State
```

Dieser muss weiterhin alle Hard Constraints erfüllen.

## Wiederholbarkeit

Ein Disaster-Recovery-Plan soll aus demselben deklarativen Modell wiederholt erzeugbar sein.

Damit kann Recovery regelmäßig getestet werden, ohne ein separates manuelles Runbook zu pflegen.

## Verification

Disaster Recovery gilt erst als abgeschlossen, wenn der wiederhergestellte Zustand überprüft wurde.

```text
Recovery Actions
      ↓
State Verification
      ↓
Dependency Verification
      ↓
Contract Verification
      ↓
Restore Complete
```

Ein erfolgreich gestarteter Satz von Komponenten ist allein noch kein erfolgreich wiederhergestelltes System.

## Normative Anforderungen

1. NovaOS MUSS Disaster Recovery deklarativ beschreibbar machen.
2. Disaster Recovery MUSS auf dem Desired State des Systems basieren.
3. Recovery-Pläne MÜSSEN aus aktuellem Zustand und deklarativem Zielzustand ableitbar sein.
4. Abhängigkeiten MÜSSEN bei der Recovery-Reihenfolge berücksichtigt werden.
5. Recovery DARF nicht ausschließlich auf statischen Startskripten oder Runbooks basieren.
6. Alternative Provider und Ressourcen MÜSSEN bei kompatiblen Contracts nutzbar sein.
7. RTO und RPO MÜSSEN in die Disaster-Recovery-Planung einfließen.
8. Persistent Data, Runtime State, Configuration, Identity und Secrets MÜSSEN getrennt wiederherstellbar sein können.
9. Security-, Trust-, Capability- und Attestation-Anforderungen MÜSSEN nach Recovery erneut gelten.
10. Disaster Recovery DARF Data-Sovereignty-Hard-Constraints nicht abschwächen.
11. Secrets SOLLEN nicht unnötig als exportierbares Material in Recovery-Artefakten gespeichert werden.
12. Progressive Restore MUSS unterstützt werden können.
13. Deklarativ definierte Degraded States MÜSSEN als Recovery-Ziel unterstützt werden können.
14. Recovery-Modelle MÜSSEN wiederholt testbar sein.
15. Disaster Recovery MUSS abschließend verifiziert werden.
16. Nicht erfüllbare Recovery Contracts MÜSSEN explizit als Violation behandelt werden.
17. Disaster-Recovery-Plan, Ausführung und Ergebnis MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- kein separates manuelles Recovery-Modell neben der Systemarchitektur,
- Recovery-Pläne können an den tatsächlichen Ausfall angepasst werden,
- alternative Provider und Ressourcen können automatisch verwendet werden,
- RTO, RPO, Trust und Sovereignty fließen direkt in die Planung ein,
- Disaster Recovery kann regelmäßig automatisiert getestet werden,
- Normal- und Degraded-Zustände sind explizit definierbar.

### Negative Konsequenzen

- das deklarative Systemmodell muss Recovery-relevante Abhängigkeiten vollständig genug beschreiben,
- großflächige Recovery benötigt zuverlässige Dependency- und State-Informationen,
- vollständige Recovery kann bei nicht verfügbaren Hard-Constraint-konformen Ressourcen unmöglich sein.

## Verworfene Alternativen

### Disaster Recovery über feste Runbooks

Verworfen, da statische Abläufe den tatsächlichen Ausfallzustand und alternative Ressourcen nur eingeschränkt berücksichtigen.

### Vollständiges System-Image als alleinige Recovery

Verworfen, da Identität, Secrets, externe Abhängigkeiten, Sovereignty und verteilte Zustände nicht allein durch ein Image korrekt wiederhergestellt werden.

### Hard Constraints im Disaster-Fall lockern

Verworfen, da ein wiederhergestelltes, aber unsicheres oder nicht souveränitätskonformes System kein gültiger Recovery-Zustand ist.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0009_Semantic_Checkpointing`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0013_Recovery_Storm_Protection`
- `ADR-RESILIENCE-0017_RTO_und_RPO_als_Recovery_Contracts`
- `ADR-ARCH-0002_Declarative_System_Model`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-TRUST-0010_Workload_Attestation_vor_Secret_Release`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-DISASTER-RECOVERY-0001`
- `NPSPEC-RESILIENCE-DR-MODEL-0001`
- `NPSPEC-RESILIENCE-DR-DEPENDENCY-GRAPH-0001`
- `NPSPEC-RESILIENCE-DR-PLANNING-0001`
- `NPSPEC-RESILIENCE-DR-PROGRESSIVE-RESTORE-0001`
- `NPSPEC-RESILIENCE-DR-DEGRADED-STATE-0001`
- `NPSPEC-RESILIENCE-DR-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-DISASTER-RECOVERY-TEST-0001`

## Ergebnis

NovaOS behandelt Disaster Recovery als Wiederherstellung eines deklarativ beschriebenen gültigen Systemzustands:

```text
Declarative Desired State
          +
Recovery Contracts
          +
Current Disaster State
          ↓
Nova.Resilience
          ↓
Dependency-aware Recovery Plan
          ↓
Progressive Recovery
          ↓
Verification
          ↓
Restored Normal / Allowed Degraded State
```

Damit wird Disaster Recovery nicht zu einem separaten Satz manueller Notfallskripte, sondern zu einer direkten Erweiterung des deklarativen NovaOS-Systemmodells.