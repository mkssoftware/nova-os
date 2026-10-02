# NPSPEC-VERIFY-CAPABILITYSAFETY-0001 – Nova Capability Safety Verification

## Status

Angenommen

## Kategorie

Verification / Capability Safety / Verified Core

## Zweck

NovaOS definiert Capability Safety als formal überprüfbare Eigenschaft, dass Authority ausschließlich durch gültige Capabilities entsteht, verwendet, übertragen, eingeschränkt und widerrufen werden kann.

```text
Capability
    ↓
Validate
    ↓
Authority
    ↓
Authorized Operation
```

Zentrales Sicherheitsziel:

```text
Keine Operation darf mehr Authority erzeugen,
als durch ihre autorisierten Eingaben zulässig ist.
```

## Grundprinzipien

```text
Capability ≠ Identity
Capability ≠ Handle
Capability ≠ ObjectID
Capability ≠ Permission Name
Possession ≠ Knowledge
Discovery ≠ Authority
Handle Access ≠ Authority
Delegation ≠ Authority Creation
Attenuation ≠ Authority Expansion
Revoked ≠ Valid
Unknown ≠ Valid
```

## Capability Safety Model

```text
Capability
├── CapabilityID
├── CapabilityType
├── Target
├── Rights
├── Constraints
├── State
└── Generation
```

Optional:

```text
Issuer
Holder
Expiration
DelegationChain
RevocationState
SecurityLabel
ProvenanceID
```

## Authority Invariant

Die zentrale formale Eigenschaft lautet:

```text
Authority(Output)
⊆
AuthorizedAuthority(Input)
```

Ausnahmen benötigen eine explizit spezifizierte Authority-Grant-Operation.

Eine gewöhnliche Operation darf Authority nicht spontan erzeugen.

## Capability Validation

Vor sicherheitskritischer Nutzung müssen mindestens geprüft werden:

```text
Capability exists
Capability type valid
Capability state valid
Target valid
Requested right allowed
Constraints satisfied
Generation current
Not expired
Not revoked
```

Erst danach darf die geschützte Operation ausgeführt werden.

## Least Authority

Komponenten sollen nur die minimal notwendige Authority erhalten.

```text
Requested Authority
        ∩
Security Policy
        ∩
Trust Policy
        ∩
System Constraints
        ↓
Granted Authority
```

Nicht benötigte Rechte dürfen nicht implizit hinzugefügt werden.

## Delegation

Capability Delegation muss Authority erhalten oder reduzieren.

```text
Capability A
Rights = Read + Write

      ↓ Delegate

Capability B
Rights = Read
```

Es muss gelten:

```text
Authority(Delegated)
⊆
Authority(Source)
```

## Attenuation

Attenuation darf Authority ausschließlich einschränken.

```text
Read + Write + Share
        ↓
     Attenuate
        ↓
       Read
```

Formal:

```text
Authority(After)
⊆
Authority(Before)
```

## Composition

Bei Capability Composition darf die resultierende Authority die zulässige Kombination der Eingaben nicht überschreiten.

```text
Authority(Result)
⊆
Union(Authorized Inputs)
```

Zusätzliche Policy- und Constraint-Regeln können das Ergebnis weiter reduzieren.

## Transfer

Capability Transfer muss explizit erfolgen.

```text
Sender
   ↓
Authorized Transfer
   ↓
Receiver
```

IPC oder Shared Memory dürfen keine implizite Authority-Übertragung erzeugen.

```text
Data Transfer ≠ Capability Transfer
```

## Revocation

Revocation muss im Sicherheitsmodell berücksichtigt werden.

```text
Valid
  ↓
Revoked
```

Nach wirksamer Revocation darf die Capability nicht erneut als gültig akzeptiert werden.

```text
Revoked → Valid
```

darf nicht allein durch alten State, Snapshot, Cache oder Rollback entstehen.

## Unknown State

Kann der Capability-Zustand nicht sicher bestimmt werden:

```text
CapabilityState = Unknown
```

gilt für sicherheitskritische Entscheidungen:

```text
Unknown ≠ Valid
```

## Handles

Handles dürfen lediglich lokale Referenzen auf Capability-geschützte Objekte darstellen.

```text
Handle
   ↓
Resolve
   ↓
Object
   ↓
Capability Validation
```

Ein gültiger Handle ersetzt keine Authority-Prüfung.

## IPC

Capability Transfer über IPC muss explizit modelliert werden.

Zu prüfen sind:

```text
Sender Authority
Transfer Right
Attenuation
Receiver Context
Lifetime
Revocation
```

Der Empfänger darf nicht mehr Authority erhalten als tatsächlich übertragen wurde.

## Transactions

Capability-relevante Transaktionen müssen Revocation und Concurrent Changes berücksichtigen.

```text
Validate Capability
      ↓
Prepare
      ↓
Capability Revoked
      ↓
Revalidate
      ↓
Commit / Reject
```

```text
Valid at Begin ≠ Valid at Commit
```

## State Rollback

Historischer State darf keine widerrufene Authority wiederherstellen.

```text
v10: Capability Valid
v11: Capability Revoked
v12: Rollback State Content

Result:
Capability remains Revoked
```

Security State besitzt Vorrang vor historischer Authority.

## Memory und Type Safety

Capability Safety ergänzt andere Verifikationsbereiche:

```text
Memory Safety
+
Type Safety
+
Capability Safety
+
Isolation
```

Ein speichersicherer und korrekt typisierter Zugriff kann trotzdem unautorisiert sein.

## Formal Verification

Kritische Capability-Operationen sollen formal spezifiziert werden:

```text
Create
Grant
Use
Delegate
Attenuate
Compose
Transfer
Revoke
Expire
Destroy
```

Für jede Operation müssen relevante Preconditions, Postconditions und Authority-Invarianten definierbar sein.

## Model Checking

Model Checking soll insbesondere untersuchen:

```text
Concurrent Delegation
Concurrent Revocation
Transfer during Revocation
Stale Capability
Generation Reuse
Rollback after Revocation
Delegation Chains
Composition
```

Unzulässige Authority Amplification muss als Property-Verletzung erkannt werden.

## Unsafe Boundaries

Komponenten außerhalb des Verified Core dürfen Capability Safety nicht umgehen.

Treiber, Legacy-Komponenten und fremder Code müssen über:

```text
Isolation
Sandboxing
Capability Gateways
IOMMU
Validated IPC
```

begrenzt werden.

## Provenance

Sicherheitsrelevante Capability-Operationen sollen nachvollziehbar sein:

```text
CapabilityID
Operation
Source
Target
Actor
Previous Authority
Resulting Authority
Timestamp
TransactionID
```

Sensitive Capability-Tokens dürfen dabei nicht in Logs oder Provenance-Daten offengelegt werden.

## Verification Artifacts

Capability-Safety-Verifikation soll referenzieren:

```text
SpecificationID
CapabilityModelVersion
Component
SourceVersion
BuildID
VerifiedProperties
Assumptions
Tool
VerificationResult
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Capability Type
Verification Status
Authority Invariants
Delegation Rules
Attenuation Rules
Revocation Guarantees
Unsafe Boundaries
Verification Version
```

Capability-Secrets oder übertragbare Tokens dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Capability Safety als Eigenschaft des Verified Core behandeln.
2. Capability Authority MUSS formal modellierbar sein.
3. Normale Operationen DÜRFEN keine unzulässige Authority erzeugen.
4. Authority Amplification MUSS formal ausschließbar sein können.
5. Capability Type, Target, Rights und Constraints MÜSSEN validierbar sein.
6. Least Authority MUSS unterstützt werden.
7. Delegation DARF Authority NICHT über die Source Capability hinaus erweitern.
8. Attenuation DARF Authority ausschließlich erhalten oder reduzieren.
9. Capability Composition DARF keine unzulässige zusätzliche Authority erzeugen.
10. Capability Transfer MUSS explizit erfolgen.
11. Data Transfer DARF NICHT automatisch Capability Transfer bedeuten.
12. Revoked Capabilities DÜRFEN NICHT erneut als gültig akzeptiert werden.
13. Unknown Capability State DARF NICHT als Valid interpretiert werden.
14. Handles DÜRFEN Authority-Prüfungen NICHT ersetzen.
15. IPC Capability Transfer MUSS formal überprüfbar sein können.
16. Kritische Transactions MÜSSEN Capability Revocation vor Commit berücksichtigen können.
17. State Rollback DARF widerrufene Authority NICHT wiederherstellen.
18. Capability Safety MUSS getrennt von Memory und Type Safety betrachtet werden.
19. Kritische Capability-Operationen MÜSSEN formal spezifizierbar sein.
20. Concurrent Capability Operations SOLLEN modellgeprüft werden.
21. Nicht verifizierte Komponenten MÜSSEN Capability-Grenzen nicht umgehen können.
22. Kritische Capability-Operationen SOLLEN Provenance besitzen.
23. Capability-Tokens DÜRFEN NICHT durch Verification, Logging oder Introspection offengelegt werden.
24. Verification Results MÜSSEN mit konkreten Implementierungsversionen verknüpfbar sein.
25. Capability-Safety-Status MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-VERIFY-FORMALSPEC-0001`
- `NPSPEC-VERIFY-MODELCHECK-0001`
- `NPSPEC-VERIFY-MEMORYSAFETY-0001`
- `NPSPEC-VERIFY-TYPESAFETY-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`
- `ADR-VERIFY-0005`

## Ergebnis

```text
Requested Operation
        ↓
Resolve Capability
        ↓
Validate Type + Target
        ↓
Validate Rights + Constraints
        ↓
Validate Revocation + Generation
        ↓
Check Authority Invariants
        ↓
Authorized?
├── No  → Reject
└── Yes → Execute
             ↓
       Preserve Authority
             ↓
           Verify
```

NovaOS erhält damit ein formal überprüfbares Capability-Sicherheitsmodell, das Authority Amplification verhindert und sicherstellt, dass Rechte ausschließlich über explizite, validierte und kontrollierte Capability-Operationen entstehen, übertragen, eingeschränkt und widerrufen werden können.