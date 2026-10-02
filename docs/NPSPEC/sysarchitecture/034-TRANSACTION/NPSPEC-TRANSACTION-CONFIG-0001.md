# NPSPEC-TRANSACTION-CONFIG-0001 – Nova Transactional Configuration

## Status

Angenommen

## Kategorie

Transaction / Configuration / State Management

## Zweck

NovaOS definiert Konfigurationsänderungen als transaktionale Zustandsänderungen. Änderungen an System-, Service-, Treiber-, Benutzer- oder Capability-Konfigurationen werden zunächst vorbereitet und validiert, bevor sie atomar als neuer gültiger Konfigurationszustand aktiviert werden.

```text
Current Config
      ↓
Stage Changes
      ↓
Validate
      ↓
Prepare
      ↓
Commit
      ↓
Apply
      ↓
Verify
```

Fehlerhafte oder nur teilweise angewendete Konfigurationen sollen dadurch vermieden werden.

## Grundprinzipien

```text
Configuration Change ≠ Direct Mutation
Saved ≠ Applied
Applied ≠ Valid
Commit ≠ Verification
Configuration ≠ Authority
Rollback ≠ Blind Restore
```

## Configuration Transaction

```text
ConfigTransaction
├── TransactionID
├── TargetID
├── BaseVersion
├── ProposedVersion
├── Changes
├── Dependencies
└── State
```

Optional:

```text
OwnerID
ExecutionContractID
RequiredCapabilities
ValidationPolicy
RollbackPolicy
ActivationPolicy
Deadline
ProvenanceID
```

## Zustände

```text
Created
Staging
Validating
Prepared
Committing
Applying
Verifying
Completed
Aborting
RolledBack
Failed
Unknown
```

## Ablauf

```text
Current Configuration
        ↓
Begin Transaction
        ↓
Stage Changes
        ↓
Validate
        ↓
Prepare
        ↓
Commit
        ↓
Activate
        ↓
Verify
```

Bei Fehler:

```text
Failure
   ↓
Abort / Rollback / Recovery
```

## Staging

Änderungen werden zunächst in einem nicht aktiven Zustand vorbereitet.

```text
Active Config v12
        │
        └── Staged Config v13
```

Andere Komponenten dürfen `v13` nicht als aktive Konfiguration behandeln, bevor der Commit erfolgt.

## Validation

Vor Commit müssen relevante Bedingungen geprüft werden:

```text
Schema
Semantic Validity
Value Ranges
Dependencies
Capabilities
Security Policy
Trust Policy
Sovereignty
Resource Requirements
Compatibility
```

```text
Syntactically Valid ≠ Operationally Valid
```

## Versioning

Jeder committed Konfigurationszustand soll eindeutig versionierbar sein.

```text
Config v10
   ↓
Config v11
   ↓
Config v12
```

Die `BaseVersion` verhindert unbemerkte konkurrierende Änderungen.

## Conflict Detection

Beispiel:

```text
Transaction A → Base v12
Transaction B → Base v12

A commits → v13

B attempts commit
      ↓
Version Conflict
```

Transaction B muss anschließend:

```text
Revalidate
Merge
Restart
Abort
```

können.

## Atomic Activation

Mehrere zusammengehörige Werte sollen als logische Einheit aktiviert werden.

```text
Network Configuration
├── Address
├── Route
├── DNS
└── Firewall Policy
```

Abhängige Komponenten dürfen keinen ungültigen Zwischenzustand beobachten.

## Multi-Component Configuration

Eine Konfigurationsänderung kann mehrere Komponenten betreffen.

```text
Configuration Transaction
├── Service A
├── Service B
└── Driver C
```

NovaOS soll dafür koordinierte Prepare- und Commit-Phasen unterstützen.

Eine universelle globale Atomicity wird nicht vorausgesetzt.

## Runtime Activation

Konfigurationen können unterschiedliche Aktivierungsarten besitzen:

```text
Immediate
Deferred
On Restart
On Next Boot
Maintenance Window
Manual Activation
```

```text
Committed ≠ Currently Active
```

Commit- und Activation-State müssen getrennt darstellbar sein.

## Capability Security

Eine Konfigurationsänderung benötigt explizite Authority.

```text
Config Change
     +
Capability
     ↓
Authorized Mutation
```

Eine Transaktion darf keine zusätzlichen Konfigurationsrechte erzeugen.

Kritische Authority muss vor Commit erneut validierbar sein.

## Security-Sensitive Configuration

Besonders geschützt werden müssen beispielsweise:

```text
Security Policies
Capability Policies
Trust Anchors
Identity Configuration
Firewall Rules
Boot Policy
Encryption Configuration
Recovery Policy
```

Für kritische Änderungen können zusätzliche Anforderungen gelten:

```text
Step-up Authentication
Explicit Confirmation
Multiple Authorization
Signed Policy
Physical Presence
```

## Dependency Handling

Konfigurationsabhängigkeiten müssen explizit modelliert werden.

```text
Config A
   ↓
Service B
   ↓
Provider C
```

Ändert sich eine Voraussetzung während der Transaktion, muss vor Commit erneut validiert werden können.

## Resource Validation

Eine Konfiguration darf Ressourcenanforderungen besitzen.

Beispiel:

```text
Requested:
Memory Reservation = 2 GiB

Available:
Memory = 1 GiB
```

Die Konfiguration darf nicht als vollständig aktiv bestätigt werden, wenn notwendige Hard Requirements nicht erfüllt werden können.

## Dry Run

NovaOS soll Konfigurationsänderungen ohne Aktivierung prüfen können.

```text
Proposed Configuration
        ↓
Validate
        ↓
Simulate Effects
        ↓
Report
```

Dry Run erzeugt keine Authority und verändert keinen aktiven Zustand.

## Rollback

Der vorherige validierte Zustand soll erhalten bleiben, solange dies erforderlich ist.

```text
Config v12
   ↓
Apply v13
   ↓
Verification Failed
   ↓
Rollback v12
```

Vor Rollback müssen weiterhin gültige Security-, Trust- und Capability-Zustände berücksichtigt werden.

## Boot Configuration

Bootkritische Konfigurationen müssen mit Boot Health integriert werden können.

```text
Config Update
     ↓
Next Boot
     ↓
Health Verification
├── Valid → Accept
└── Invalid → Rollback
```

Dadurch können fehlerhafte Systemkonfigurationen automatisch zurückgenommen werden.

## Verification

Nach Aktivierung muss geprüft werden können:

```text
Configuration Loaded
Dependencies Available
Required Services Healthy
Resources Granted
Security Policy Active
Execution Contracts Valid
```

```text
Configuration Applied ≠ Configuration Working
```

## Recovery

Kann eine Konfiguration weder erfolgreich aktiviert noch zurückgerollt werden:

```text
Apply Failed
    ↓
Rollback Failed
    ↓
Recovery Policy
    ↓
Safe Configuration / Recovery Mode
```

NovaOS soll einen bekannten minimalen sicheren Konfigurationszustand verwenden können.

## Provenance

Jede relevante Änderung soll nachvollziehbar sein:

```text
Who Changed
What Changed
Previous Version
New Version
Authority Used
Validation Result
Activation Result
Verification Result
```

Secrets und Capability Tokens dürfen nicht in Provenance-Daten erscheinen.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
TransactionID
TargetID
BaseVersion
ProposedVersion
CommittedVersion
ActiveVersion
State
Validation Result
Conflicts
Dependencies
Activation State
Verification State
Rollback State
```

## Normative Anforderungen

1. NovaOS MUSS Konfigurationsänderungen transaktional durchführen können.
2. Aktive Konfiguration DARF NICHT direkt durch unkontrollierte Mutation ersetzt werden.
3. Änderungen MÜSSEN vor Commit validierbar sein.
4. Staged Configuration MUSS vom aktiven Zustand getrennt bleiben.
5. Konfigurationszustände MÜSSEN versionierbar sein.
6. Concurrent Modification MUSS erkennbar sein.
7. Zusammengehörige Änderungen SOLLEN atomar aktivierbar sein.
8. Multi-Component Configuration MUSS koordinierte Änderungen unterstützen können.
9. NovaOS DARF keine universelle globale Atomicity voraussetzen.
10. Commit und Activation MÜSSEN unterscheidbar sein.
11. Konfigurationsänderungen MÜSSEN explizite Capability Authority benötigen.
12. Kritische Authority MUSS vor Commit revalidierbar sein.
13. Security-kritische Konfiguration MUSS zusätzliche Policies unterstützen können.
14. Dependencies MÜSSEN vor Aktivierung revalidierbar sein.
15. Hard Resource Requirements MÜSSEN vor Aktivierung geprüft werden.
16. Dry Run SOLL ohne Zustandsänderung unterstützt werden.
17. Vorherige gültige Konfiguration SOLL für Rollback verfügbar bleiben.
18. Rollback DARF widerrufene Authority NICHT wiederherstellen.
19. Bootkritische Konfiguration SOLL mit Boot Health integriert werden.
20. Aktivierte Konfiguration MUSS verifiziert werden können.
21. Fehlgeschlagene Konfigurationsänderungen MÜSSEN Recovery ermöglichen.
22. Konfigurationsänderungen MÜSSEN autorisiert introspektierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-RECOVERYMODE-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0134`

## Ergebnis

```text
Configuration Intent
        ↓
Begin Transaction
        ↓
Stage New Version
        ↓
Validate
        ↓
Prepare
        ↓
Commit
        ↓
Activate
        ↓
Verify
├── Valid → New Active Configuration
└── Invalid
      ↓
Rollback / Recovery
```

NovaOS erhält damit ein transaktionales Konfigurationssystem, bei dem Änderungen versioniert, validiert, kontrolliert aktiviert und verifiziert werden, sodass fehlerhafte oder teilweise angewendete Konfigurationen nicht unkontrolliert den Systemzustand beschädigen.