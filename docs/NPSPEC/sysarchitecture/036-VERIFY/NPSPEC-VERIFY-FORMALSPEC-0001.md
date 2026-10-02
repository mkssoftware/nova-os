# NPSPEC-VERIFY-FORMALSPEC-0001 – Nova Formal Specification

## Status

Angenommen

## Kategorie

Verification / Formal Specification / Verified Core

## Zweck

NovaOS definiert formale Spezifikationen für sicherheitskritische Kernkomponenten, damit zentrale Systemeigenschaften mathematisch präzise beschrieben und anschließend formal überprüft werden können.

```text
Architecture Requirement
        ↓
Formal Specification
        ↓
Formal Model
        ↓
Verification
        ↓
Verified Property
```

Formale Spezifikation ergänzt Tests, Runtime Contracts, Secure/Measured Boot und Self-Healing, ersetzt diese jedoch nicht.

## Grundprinzipien

```text
Formal Specification ≠ Implementation
Formal Specification ≠ Proof
Proof ≠ Testing
Verified Property ≠ Verified Entire System
Model Correctness ≠ Implementation Correctness
Verified Build ≠ Trusted Runtime
```

NovaOS verfolgt keine unrealistische vollständige mathematische Verifikation des gesamten Betriebssystems.

Verifiziert werden gezielt kritische Eigenschaften.

## Verified Core

Priorität besitzen insbesondere:

```text
Memory Isolation
Capability Safety
Permission Enforcement
IPC Isolation
Kernel State Invariants
Critical State Transitions
Security Boundaries
```

Nicht kritische oder schwer vollständig verifizierbare Komponenten dürfen außerhalb des Verified Core liegen.

## Formal Specification Model

```text
FormalSpecification
├── SpecificationID
├── Version
├── Target
├── Assumptions
├── StateModel
├── Invariants
├── Preconditions
├── Postconditions
└── Properties
```

Optional:

```text
ThreatModel
ConcurrencyModel
TemporalProperties
CapabilityModel
InformationFlowRules
ProofArtifacts
ImplementationMapping
```

## State Model

Kritische Komponenten sollen ihren relevanten Zustand formal beschreiben.

```text
System State S
     ↓
Operation O
     ↓
Transition
     ↓
System State S'
```

Eine Transition ist nur gültig, wenn definierte Bedingungen erfüllt sind.

## Invarianten

Invarianten beschreiben Eigenschaften, die in allen zulässigen Zuständen gelten müssen.

Beispiele:

```text
No unauthorized memory access

No capability amplification

No invalid kernel state

No unauthorized IPC access
```

Formal:

```text
Invariant(S) = true
```

Für gültige Transitionen:

```text
Invariant(S)
AND
ValidTransition(S, O, S')
→
Invariant(S')
```

## Preconditions

Operationen können Voraussetzungen besitzen.

```text
Operation:
MapMemory

Preconditions:
- Capability valid
- Address range valid
- Mapping allowed
- Resource available
```

Sind Preconditions nicht erfüllt, darf die spezifizierte Transition nicht stattfinden.

## Postconditions

Nach erfolgreicher Operation müssen definierte Eigenschaften gelten.

```text
Operation
    ↓
Postcondition
```

Beispiel:

```text
MapMemory success
→
Mapping exists
AND
Mapping belongs to authorized address space
```

## Capability Safety

Das Capability-Modell muss formal ausdrücken können:

```text
Authority(Output)
⊆
AuthorizedAuthority(Input)
```

Eine Operation darf keine zusätzliche Authority erzeugen, sofern dies nicht durch eine explizit autorisierte Capability-Operation spezifiziert ist.

## Memory Isolation

Formale Spezifikationen sollen zentrale Isolationseigenschaften beschreiben.

```text
Process A Memory
      ≠
Process B Memory
```

Zugriff ist nur zulässig, wenn eine entsprechende autorisierte Beziehung existiert.

## IPC

Für sicherheitskritische IPC-Pfade sollen Eigenschaften wie:

```text
Sender Identity
Receiver Identity
Capability Transfer
Object Ownership
Buffer Access
Message Integrity
```

formal spezifizierbar sein.

```text
IPC Transfer ≠ Authority Creation
```

## State Machines

Kritische State Machines sollen zulässige Transitionen formal definieren.

```text
State A
   ↓ Allowed Transition
State B
```

Nicht spezifizierte Transitionen gelten als unzulässig.

## Safety Properties

Safety Properties beschreiben Zustände, die niemals eintreten dürfen.

```text
Unauthorized Access = Never
Invalid Capability Escalation = Never
Invalid Kernel State = Never
```

## Temporal Properties

Wo erforderlich können zeitliche Eigenschaften spezifiziert werden.

Beispiele:

```text
Request
   ↓
Eventually Response
```

oder:

```text
Revoked Capability
   ↓
Never usable afterwards
```

Zeitliche Garantien müssen mit dem jeweiligen Realtime- und Failure-Modell abgestimmt sein.

## Assumptions

Jede formale Spezifikation muss ihre Annahmen explizit machen.

Beispiele:

```text
CPU behaves according to architecture
Memory hardware behaves correctly
Boot trust anchor is valid
Cryptographic primitive satisfies assumed properties
```

```text
Hidden Assumption ≠ Verified Property
```

## Implementation Mapping

Die Verbindung zwischen Spezifikation und Implementierung muss nachvollziehbar sein.

```text
Formal Property
      ↓
Component
      ↓
Source / Interface / State
      ↓
Verification Artifact
```

Dadurch wird verhindert, dass ein korrektes Modell ohne nachvollziehbaren Bezug zur realen Implementierung als Implementierungsbeweis interpretiert wird.

## Nicht verifizierte Komponenten

Treiber und andere nicht vollständig verifizierte Komponenten müssen durch Architekturmechanismen begrenzt werden.

```text
Unverified Driver
      ↓
Isolation
      ↓
Capabilities
      ↓
IOMMU / Memory Protection
      ↓
Verified Boundary
```

```text
Unverified Component ≠ Trusted Component
```

## Runtime Contracts

Nicht vollständig formal beweisbare Eigenschaften können durch Runtime Contracts ergänzt werden.

```text
Formal Verification
        +
Runtime Validation
        +
Isolation
        +
Monitoring
```

Runtime Contracts ersetzen jedoch keinen vorhandenen formalen Beweis.

## Verification Artifacts

Verifikationsergebnisse sollen versioniert referenzierbar sein.

```text
Specification
Implementation
Proof
Tool Version
Build ID
Verification Result
```

Ändert sich eine relevante Komponente, muss bestimmt werden, ob bestehende Verifikation weiterhin gültig ist.

## Live Evolution

Bei Live Replacement gilt:

```text
Old Component Verified
        ↓
New Component
        ↓
Reverification Required
```

Eine ABI-kompatible Änderung ist nicht automatisch formal äquivalent.

```text
ABI Compatible ≠ Formally Equivalent
```

## Trusted Computing Base

NovaOS soll den für formale Garantien notwendigen Trusted Computing Base möglichst klein halten.

Zum TCB können abhängig vom Beweis gehören:

```text
Hardware
Boot Chain
Kernel Core
Verification Toolchain
Compiler
Critical Runtime Components
```

Die tatsächlichen Annahmen müssen pro Verifikation dokumentiert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
SpecificationID
Version
Target
Verified Properties
Assumptions
Verification Status
Implementation Version
Build ID
Proof Artifact
Verification Tool
Last Verification
```

## Normative Anforderungen

1. NovaOS MUSS formale Spezifikationen für sicherheitskritische Kernkomponenten unterstützen.
2. Der Verified Core MUSS explizit definiert sein.
3. Formale Spezifikation und Implementierung MÜSSEN getrennte Artefakte bleiben.
4. Verifizierte Eigenschaften MÜSSEN explizit benannt werden.
5. Nicht verifizierte Eigenschaften DÜRFEN NICHT als formal garantiert dargestellt werden.
6. Annahmen eines Beweises MÜSSEN dokumentiert werden.
7. Kritische State Invariants MÜSSEN formal beschreibbar sein.
8. Kritische State Transitions MÜSSEN formal spezifizierbar sein.
9. Memory Isolation SOLL Bestandteil des Verified Core sein.
10. Capability Safety SOLL Bestandteil des Verified Core sein.
11. Kritische IPC-Eigenschaften SOLLEN formal spezifiziert werden.
12. Authority Amplification MUSS formal ausschließbar sein können.
13. Preconditions und Postconditions MÜSSEN spezifizierbar sein.
14. Safety Properties MÜSSEN explizit modellierbar sein.
15. Temporal Properties MÜSSEN bei relevanten Komponenten spezifizierbar sein.
16. Die Verbindung zwischen Spezifikation und Implementierung MUSS nachvollziehbar sein.
17. Nicht verifizierte Komponenten MÜSSEN durch Isolation und Capabilities begrenzt werden können.
18. Runtime Contracts MÜSSEN formale Verifikation ergänzen können.
19. Verification Artifacts MÜSSEN versionierbar sein.
20. Änderungen an verifizierten Komponenten MÜSSEN eine Prüfung der Beweisgültigkeit auslösen.
21. ABI Compatibility DARF NICHT als formale Äquivalenz interpretiert werden.
22. Der Trusted Computing Base SOLL möglichst klein gehalten werden.
23. Verifikationsstatus MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-TRANSACTIONAL-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-SECURITY-ISOLATION-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-BOOT-VERIFIED-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-VERIFY-0001`

## Ergebnis

```text
Critical Requirement
        ↓
Formal Specification
        ↓
State + Invariants + Contracts
        ↓
Formal Verification
        ↓
Verified Property
        ↓
Implementation Mapping
        ↓
Build + Runtime Protection
        ↓
Continuous Verification
```

NovaOS erhält damit die Grundlage für einen gezielt formal verifizierten Kern, bei dem kritische Sicherheits- und Zustands­eigenschaften mathematisch präzise spezifiziert werden, während nicht verifizierte Komponenten durch Isolation, Capabilities, Runtime Contracts und weitere Schutzmechanismen begrenzt bleiben.