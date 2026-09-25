# NPSPEC-CAPABILITY-COMPOSITION-0001 – Nova Capability Composition

## Status

Angenommen

## Kategorie

Capability / Composition / Security

## Zweck

NovaOS definiert Capability Composition als kontrollierte Kombination mehrerer einzelner Capabilities zu einer zusammengesetzten Autorität für eine konkrete Aufgabe.

```text
Capability A
     +
Capability B
     +
Capability C
     ↓
Composition
     ↓
Composite Capability
```

Composition erleichtert komplexe Operationen, darf jedoch keine Autorität erzeugen, die nicht bereits durch die beteiligten Capabilities gedeckt ist.

## Grundprinzipien

```text
Composition ≠ Authority Creation
Composition ≠ Privilege Escalation
Composite Capability ≠ Universal Capability
Missing Authority ≠ Implicitly Granted
Composition ≠ Delegation
Composition ≠ Trust
```

Grundregel:

```text
Authority(Composite) ⊆ Union(Authority(Inputs))
```

Eine Composition darf niemals Rechte außerhalb ihrer Eingabe-Capabilities erzeugen.

## Composition-Modell

Eine zusammengesetzte Capability besitzt mindestens:

```text
CompositeCapability
├── CapabilityID
├── Component Capabilities
├── Operations
├── Constraints
└── State
```

Optional:

```text
Purpose
Holder Binding
Security Domain
Expiration
ExecutionContract
Parent CapabilityIDs
Composition Policy
```

Die Composite Capability erhält eine eigene `CapabilityID`.

## Beispiel

Eine Dokumentkonvertierung benötigt:

```text
Read(Source)
      +
Write(Target)
      +
Use(Converter)
      ↓
DocumentConversion Capability
```

Die zusammengesetzte Capability darf beispielsweise keinen Zugriff auf andere Dateien erhalten.

## Rechtebildung

Composition darf Rechte kombinieren, aber nicht erweitern.

```text
Capability A
Target: File X
Rights: Read

Capability B
Target: File Y
Rights: Write

        ↓

Composite Capability
├── Read(File X)
└── Write(File Y)
```

Nicht erlaubt wäre:

```text
Read(All Files)
Write(All Files)
```

## Constraints

Constraints der Eingabe-Capabilities müssen erhalten bleiben.

```text
Capability A
Expiration: 15:00

Capability B
Expiration: 16:00
```

Eine Composition darf daraus keine bis 17:00 gültige Autorität erzeugen.

Restriktive Bedingungen müssen sicher kombiniert werden.

## Zweckbindung

Composite Capabilities können an eine konkrete Operation gebunden werden.

```text
Purpose:
"Convert Document A to PDF B"
```

Dies reduziert die Gefahr, dass eine für einen Workflow erzeugte Autorität für andere Aufgaben wiederverwendet wird.

## ExecutionContract

Capability Composition soll mit `Nova.ExecutionContract` kombinierbar sein.

```text
ExecutionContract
├── Operation
├── Required Capabilities
├── Resource Budget
├── Trust Requirements
└── Sovereignty
```

Der Capability Resolver kann daraus die für die Operation notwendige Autoritätsmenge bestimmen.

## Delegation

Eine Composite Capability kann delegiert werden, sofern alle beteiligten Capabilities dies erlauben.

```text
Composite A
     ↓
Attenuation
     ↓
Composite B
```

Dabei gilt:

```text
Authority(B) ⊆ Authority(A)
```

Nicht delegierbare Komponenten dürfen nicht durch Composition indirekt delegierbar werden.

## Revocation

Composite Capabilities hängen von ihren Komponenten ab.

```text
Capability A ─┐
Capability B ─┼→ Composite Capability
Capability C ─┘
```

Wird eine erforderliche Komponente widerrufen:

```text
Component Revoked
       ↓
Composite Re-Evaluation
       ↓
Restricted / Invalid
```

Eine Composition darf Revocation nicht umgehen.

## Dynamische Composition

NovaOS kann Capabilities für kurzlebige Operationen dynamisch zusammensetzen.

```text
Discover
   ↓
Acquire
   ↓
Compose
   ↓
Execute
   ↓
Release
```

Kurzlebige Composition ist für komplexe Workflows gegenüber dauerhaft breiten Capabilities zu bevorzugen.

## Introspection

Autorisierte Komponenten sollen mindestens abfragen können:

```text
Composite CapabilityID
Component CapabilityIDs
Effective Rights
Constraints
Purpose
Expiration
State
```

Die Darstellung darf keine verwendbaren Capability Tokens offenlegen.

## Audit

Audit kann nachvollziehen:

```text
Principal
   ↓
Composite Capability
   ↓
Component Capabilities
   ↓
Operation
```

Damit bleibt erkennbar, aus welcher Autorität eine komplexe Operation abgeleitet wurde.

## Normative Anforderungen

1. NovaOS MUSS Capability Composition ohne Erzeugung zusätzlicher Autorität ermöglichen.
2. Composite Capabilities MÜSSEN eine eigene `CapabilityID` besitzen.
3. Die effektive Autorität DARF NICHT über die Autorität der Komponenten hinausgehen.
4. Constraints der Komponenten MÜSSEN erhalten bleiben.
5. Nicht delegierbare Rechte DÜRFEN durch Composition NICHT delegierbar werden.
6. Composite Capabilities MÜSSEN zweckgebunden werden können.
7. Revocation einer erforderlichen Komponente MUSS die Composition neu bewerten.
8. Capability Composition SOLL mit `Nova.ExecutionContract` integrierbar sein.
9. Kurzlebige Composition SOLL für temporäre Workflows unterstützt werden.
10. Composition-Beziehungen SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-CAPABILITY-0005`

## Ergebnis

```text
Required Authority
       ↓
Individual Capabilities
       ↓
Controlled Composition
       ↓
Purpose-bound Composite Capability
       ↓
Operation
       ↓
Release
```

NovaOS erhält damit eine sichere Capability-Composition-Schicht, mit der komplexe Workflows aus kleinen, klar begrenzten Autoritätsbausteinen zusammengesetzt werden können, ohne neue oder implizite Privilegien zu erzeugen.