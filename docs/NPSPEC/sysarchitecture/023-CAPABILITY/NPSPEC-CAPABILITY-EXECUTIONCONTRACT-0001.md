# NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001 – Nova Capability Execution Contract

## Status

Angenommen

## Kategorie

Capability / Execution Contract / Security

## Zweck

NovaOS verbindet Capabilities mit `Nova.ExecutionContract`, damit für eine konkrete Operation automatisch die minimal notwendige Autorität bestimmt, geprüft und bereitgestellt werden kann.

```text
ExecutionContract
       ↓
Required Capabilities
       ↓
Discovery + Negotiation
       ↓
Attenuation / Composition
       ↓
Execution Capability Set
       ↓
Operation
```

Der ExecutionContract beschreibt Anforderungen. Er selbst stellt keine Autorität dar.

## Grundprinzipien

```text
ExecutionContract ≠ Capability
Requirement ≠ Authority
Requested Capability ≠ Granted Capability
Provider Selection ≠ Authorization
Optimization ≠ Security Override
Contract Change ≠ Automatic Authority Expansion
Missing Capability ≠ Implicit Permission
```

## Capability-Anforderungen

Ein ExecutionContract kann benötigte Autorität explizit beschreiben:

```text
ExecutionContract
├── Operation
├── Input Types
├── Output Types
├── Required Capabilities
├── Hard Requirements
├── Soft Preferences
├── Resource Budget
├── Determinism
├── Trust Requirements
├── Sovereignty
└── Security Context
```

Beispiel:

```text
Operation: ConvertDocument

Required Capabilities:
├── Read(SourceDocument)
├── Write(TargetDocument)
└── Use(DocumentConverter)
```

## Auflösung

NovaOS bestimmt aus dem Contract ein Capability Set.

```text
Contract
   ↓
Capability Resolver
   ↓
Registry / Discovery
   ↓
Policy Validation
   ↓
Negotiation
   ↓
Minimal Capability Set
```

Nur bereits vorhandene, delegierbare oder ausdrücklich neu autorisierte Rechte dürfen verwendet werden.

## Least Authority

Das resultierende Capability Set soll exakt auf die Operation begrenzt werden.

```text
Broad Authority
      ↓
Attenuation
      ↓
Required Authority
```

Es gilt:

```text
Granted Authority
⊆ Available Authorized Authority
```

Der ExecutionContract darf keine fehlende Autorität erzeugen.

## Composition

Benötigt eine Operation mehrere Rechte, können diese kontrolliert zusammengesetzt werden.

```text
Read Capability
      +
Write Capability
      +
Service Capability
      ↓
Execution Capability Set
```

Die Composition gilt nur im definierten Contract-Kontext.

## Provider-Auswahl

Der ExecutionContract kann Anforderungen an Provider definieren:

```text
Trust
Location
Latency
Determinism
Resource Budget
Sovereignty
Algorithm
Hardware
```

Provider-Auswahl erfolgt nach der NovaOS-Priorität:

```text
Safety
→ Security
→ Sovereignty / Trust
→ Hard System Constraints
→ Explicit User Decisions
→ Soft Preferences
→ Adaptive Optimization
```

Keine Optimierung darf Security- oder Capability-Grenzen überschreiben.

## Temporäre Capabilities

Für einzelne Ausführungen sollen bevorzugt kurzlebige Capabilities erzeugt werden.

```text
Begin Execution
      ↓
Acquire / Compose Capabilities
      ↓
Execute
      ↓
Verify
      ↓
Release / Expire
```

Dadurch wird dauerhaft gehaltene Autorität reduziert.

## Structured Concurrency

Execution Capabilities können an einen Task oder eine Task Group gebunden werden.

```text
Task Group
   └── Task
       └── Execution Capability Set
```

Bei:

```text
Completion
Cancellation
Deadline
Failure
```

können temporäre Capabilities automatisch freigegeben oder widerrufen werden.

## Contract-Änderung

Eine Änderung des ExecutionContracts darf Autorität nicht still erweitern.

```text
Contract v1
   ↓
Contract Change
   ↓
Capability Re-Evaluation
   ↓
New Authorization if Required
```

Zusätzliche Rechte benötigen eine erneute Policy- oder Benutzerentscheidung.

## Remote Execution

Bei Remote Execution bleibt die Capability-Autorität explizit.

```text
Local Contract
      ↓
Attenuated Remote Capability
      ↓
Remote Provider
```

Der entfernte Provider erhält nur die für die Operation benötigte Autorität.

```text
Transparent Location ≠ Transparent Authority
```

## Revocation

Wird eine benötigte Capability während der Ausführung widerrufen:

```text
Capability Revoked
       ↓
ExecutionContract Re-Evaluation
       ↓
Continue / Restrict / Cancel / Rollback
```

Die Reaktion richtet sich nach Security Policy und Operationssemantik.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ExecutionContract ID
Operation
Required Capabilities
Resolved Capabilities
Provider
Effective Rights
Constraints
Temporary Grants
Revocation State
```

Capability Tokens dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Capability-Anforderungen in `Nova.ExecutionContract` ausdrücken können.
2. Ein ExecutionContract DARF selbst keine Autorität darstellen.
3. Fehlende Capabilities DÜRFEN NICHT implizit erzeugt werden.
4. Das resultierende Capability Set SOLL minimal auf die Operation begrenzt sein.
5. Capability Discovery, Negotiation, Attenuation und Composition SOLLEN integrierbar sein.
6. Provider-Auswahl DARF Security- und Trust-Anforderungen NICHT umgehen.
7. Temporäre Execution Capabilities SOLLEN an den Lebenszyklus der Operation gebunden werden.
8. Contract-Änderungen DÜRFEN Autorität NICHT still erweitern.
9. Remote Execution MUSS attenuierte und explizite Autorität verwenden.
10. Capability-Auflösung und effektive Autorität MÜSSEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-CAPABILITY-REGISTRY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-CAPABILITY-0018`

## Ergebnis

```text
Execution Intent
      ↓
ExecutionContract
      ↓
Minimal Capability Resolution
      ↓
Purpose-bound Authority
      ↓
Provider Execution
      ↓
Release / Revoke
```

NovaOS erhält damit eine direkte Verbindung zwischen deklarativer Ausführungsplanung und Capability Security, sodass jede Operation möglichst genau die Autorität erhält, die sie für ihre konkrete Ausführung benötigt – nicht mehr und nicht länger als erforderlich.