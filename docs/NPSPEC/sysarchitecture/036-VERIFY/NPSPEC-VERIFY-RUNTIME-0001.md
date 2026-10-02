# NPSPEC-VERIFY-RUNTIME-0001 – Nova Runtime Verification

## Status

Angenommen

## Kategorie

Verification / Runtime Verification / Verified Core

## Zweck

NovaOS verwendet Runtime Verification für Eigenschaften, die nicht vollständig statisch oder formal nachgewiesen werden können oder deren Gültigkeit von dynamischem Systemzustand abhängt.

```text
Static / Formal Verification
           +
Runtime Observation
           ↓
Runtime Verification
```

Runtime Verification ergänzt formale Verifikation, Model Checking und Static Analysis.

## Grundprinzipien

```text
Runtime Verification ≠ Testing
Runtime Verification ≠ Formal Proof
Runtime Check Passed ≠ Future Correctness
Contract Violation ≠ Root Cause
Observed State ≠ Guaranteed Complete State
Unknown ≠ Valid
Monitoring ≠ Authority
Detection ≠ Recovery
```

## Runtime Contract

Eine überwachte Eigenschaft wird als Runtime Contract beschrieben.

```text
RuntimeContract
├── ContractID
├── Target
├── Preconditions
├── Invariants
├── Postconditions
├── FailurePolicy
└── VerificationState
```

Optional:

```text
TemporalProperties
ResourceConstraints
SecurityConstraints
TrustRequirements
ExecutionContractID
StateVersion
ProvenanceID
```

## Verification Flow

```text
Operation
    ↓
Check Preconditions
    ↓
Execute
    ↓
Monitor Invariants
    ↓
Check Postconditions
    ↓
Verified / Violated / Unknown
```

## Verification States

Runtime Verification verwendet mindestens:

```text
Verified
Violated
Unknown
Unavailable
```

Dabei gilt:

```text
Unknown ≠ Verified
Unavailable ≠ Verified
```

## Preconditions

Vor kritischen Operationen können dynamische Voraussetzungen geprüft werden.

Beispiele:

```text
Capability valid
State version current
Resource available
Required trust satisfied
Input valid
Memory region valid
```

Eine verletzte harte Precondition verhindert die Operation.

## Invariants

Während der Ausführung können kritische Invarianten überwacht werden.

Beispiele:

```text
Memory Ownership preserved
Capability Authority not amplified
Resource Budget respected
State Machine remains valid
Security Boundary preserved
```

## Postconditions

Nach einer Operation wird geprüft, ob der erwartete Zustand tatsächlich erreicht wurde.

```text
Operation Completed
      ↓
Expected State
      ↕
Observed State
      ↓
Verify
```

```text
Execution Success ≠ Verified Result
```

## Temporal Verification

Runtime Monitors können zeitliche Eigenschaften überwachen.

```text
Event A
   ↓
Within T
   ↓
Event B
```

Beispiele:

```text
Deadline
Timeout
Maximum Latency
Required Event Order
Recovery Progress
Revocation Effectiveness
```

## State Machines

Runtime Verification soll State-Machine-Transitionen kontrollieren können.

```text
Current State
     ↓
Requested Transition
     ↓
Valid?
├── No  → Violation
└── Yes → Execute → Verify
```

Ungültige Transitionen dürfen nicht still akzeptiert werden.

## Capability Safety

Capabilities können unmittelbar vor sicherheitskritischer Verwendung erneut geprüft werden.

```text
Capability
    ↓
Validate
    ↓
Use
```

Dabei können insbesondere geprüft werden:

```text
Type
Rights
Constraints
Generation
Expiration
Revocation
```

```text
Previously Valid ≠ Currently Valid
```

## Information Flow

Dynamische Informationsflüsse müssen zur Laufzeit überprüfbar sein.

```text
Source
  ↓
Runtime Flow Check
  ↓
Destination
```

Dabei können Labels, Capabilities, Trust, Sovereignty und aktuelle Policies berücksichtigt werden.

## Resource Contracts

Runtime Verification kann ExecutionContract-Grenzen überwachen.

Beispiele:

```text
CPU Budget
Memory Budget
I/O Budget
Network Budget
Latency
Deadline
Energy Budget
```

Eine Überschreitung muss gemäß Contract Policy behandelt werden.

## Transactions

Transactions benötigen Runtime Verification an kritischen Punkten.

```text
Begin
 ↓
Validate
 ↓
Prepare
 ↓
Revalidate
 ↓
Commit
 ↓
Verify
```

Insbesondere vor Commit müssen dynamisch veränderliche Voraussetzungen erneut geprüft werden können.

## Concurrency

Runtime Verification muss Race-sensitive Eigenschaften berücksichtigen.

```text
Check
 ↓
Concurrent Change
 ↓
Use
```

Eine Prüfung darf nicht automatisch unbegrenzt gültig bleiben.

Versionen, Generationen, atomare Operationen oder Transaction Boundaries müssen TOCTOU-Probleme begrenzen.

## Distributed Systems

Remote State kann während der Verifikation unbekannt oder veraltet sein.

```text
Remote Verification
├── Verified
├── Violated
├── Unknown
└── Unavailable
```

```text
No Response ≠ Verified
No Response ≠ Failed
```

## Failure Handling

Eine Runtime-Verletzung löst eine definierte Policy aus.

Mögliche Reaktionen:

```text
Reject
Cancel
Contain
Isolate
Restart
Rollback
Failover
Degrade
Recover
Safe Mode
Panic
```

Die Reaktion muss zur Failure Domain und Kritikalität passen.

## Self-Healing

Runtime Verification ist ein zentraler Bestandteil von Self-Healing.

```text
Detect
  ↓
Diagnose
  ↓
Repair
  ↓
Runtime Verify
  ↓
Recovered?
```

```text
Repair Completed ≠ Recovery Verified
```

## Runtime Overhead

Runtime Verification verbraucht Ressourcen.

Deshalb dürfen Checks abhängig von Kritikalität klassifiziert werden:

```text
Mandatory
Adaptive
Diagnostic
Debug
```

Sicherheitskritische Mandatory Checks dürfen nicht allein aus Performancegründen deaktiviert werden.

## Static Analysis Integration

Static Analysis kann Eigenschaften klassifizieren:

```text
Proven
Violated
Unknown
```

Für `Unknown` kann ein Runtime Contract erforderlich werden.

```text
Static Unknown
      ↓
Runtime Check
```

## Formal Verification Integration

Formal bewiesene Eigenschaften benötigen nicht zwangsläufig identische Runtime Checks.

Runtime Verification bleibt jedoch sinnvoll für Annahmen über:

```text
Hardware
External Input
Dynamic Configuration
Remote Systems
Runtime State
Unverified Components
```

## Unverified Components

Treiber, Legacy-Code und andere nicht formal verifizierte Komponenten müssen besonders überwacht und isoliert werden können.

```text
Unverified Component
       ↓
Capability Boundary
       +
Runtime Contracts
       +
Isolation
```

## Evidence

Runtime Verification erzeugt überprüfbare Evidence.

```text
ContractID
Target
StateVersion
Observation
Result
Timestamp
BuildID
```

Sensitive Inhalte dürfen dabei nicht unnötig gespeichert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Runtime Contracts
Verification State
Active Monitors
Detected Violations
Failure Actions
Last Verification
State Version
Build ID
```

## Normative Anforderungen

1. NovaOS MUSS Runtime Verification für dynamische kritische Eigenschaften unterstützen.
2. Runtime Verification DARF NICHT als vollständiger formaler Beweis interpretiert werden.
3. Runtime Contracts MÜSSEN versionierbar sein.
4. Preconditions MÜSSEN vor kritischen Operationen überprüfbar sein.
5. Kritische Invarianten MÜSSEN zur Laufzeit überwachbar sein können.
6. Postconditions MÜSSEN für relevante Operationen überprüfbar sein.
7. `Unknown` DARF NICHT als `Verified` interpretiert werden.
8. State-Machine-Transitionen SOLLEN zur Laufzeit validierbar sein.
9. Capability-Zustände MÜSSEN vor kritischer Nutzung erneut validierbar sein.
10. Information-Flow-Regeln MÜSSEN dynamisch durchsetzbar sein können.
11. ExecutionContract-Grenzen SOLLEN überwacht werden können.
12. Transactions MÜSSEN vor kritischen Commit-Punkten revalidierbar sein.
13. Runtime Checks MÜSSEN Concurrent State Changes berücksichtigen.
14. Distributed Verification MUSS `Unknown` und `Unavailable` darstellen können.
15. Runtime-Verletzungen MÜSSEN definierte Failure Policies besitzen.
16. Recovery DARF erst nach erfolgreicher Verifikation als abgeschlossen gelten.
17. Mandatory Security Checks DÜRFEN NICHT allein zur Performanceoptimierung deaktiviert werden.
18. Static-Analysis-`Unknown` SOLL in Runtime Verification überführt werden können.
19. Nicht verifizierte Komponenten SOLLEN durch Runtime Contracts und Isolation begrenzt werden.
20. Runtime Verification MUSS mit Self-Healing integrierbar sein.
21. Runtime Evidence MUSS mit State-, Contract- und Build-Versionen verknüpfbar sein.
22. Sensitive Informationen DÜRFEN durch Runtime Evidence NICHT unnötig offengelegt werden.
23. Runtime-Verification-Status MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-VERIFY-FORMALSPEC-0001`
- `NPSPEC-VERIFY-MODELCHECK-0001`
- `NPSPEC-VERIFY-MEMORYSAFETY-0001`
- `NPSPEC-VERIFY-TYPESAFETY-0001`
- `NPSPEC-VERIFY-CAPABILITYSAFETY-0001`
- `NPSPEC-VERIFY-TEMPORAL-0001`
- `NPSPEC-VERIFY-INFORMATIONFLOW-0001`
- `NPSPEC-VERIFY-STATICANALYSIS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-TRANSACTIONAL-0001`
- `NPSPEC-RESILIENCE-SELFHEALING-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-VERIFY-0008`

## Ergebnis

```text
Formal + Static Verification
           ↓
Remaining Dynamic Properties
           ↓
Runtime Contracts
           ↓
Observe + Validate
           ↓
Verified?
├── Yes     → Continue
├── No      → Contain / Recover
└── Unknown → Safe Policy
           ↓
Verification Evidence
```

NovaOS erhält damit eine kontinuierliche Runtime-Verifikationsschicht, die statische und formale Nachweise um die tatsächliche Laufzeitrealität ergänzt und dynamische Zustände, Capabilities, Transaktionen, Ressourcen und Recovery-Ergebnisse kontrolliert überprüft.