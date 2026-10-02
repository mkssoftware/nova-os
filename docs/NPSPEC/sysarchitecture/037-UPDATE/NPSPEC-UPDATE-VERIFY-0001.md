# NPSPEC-UPDATE-VERIFY-0001 – Nova Update Verification

## Status

Angenommen

## Kategorie

Update / Verification / Integrity / Health

## Zweck

NovaOS definiert eine einheitliche Verifikation für Updates vor, während und nach ihrer Aktivierung.

Ein Update gilt erst dann als erfolgreich, wenn nicht nur seine Installation, sondern auch Integrität, Trust, Kompatibilität, Aktivierung und Betriebsfähigkeit überprüft wurden.

```text
Acquire
  ↓
Pre-Verify
  ↓
Stage
  ↓
Activate
  ↓
Post-Verify
  ↓
Commit
```

## Grundprinzipien

```text
Downloaded ≠ Valid
Valid ≠ Trusted
Trusted ≠ Compatible
Installed ≠ Active
Active ≠ Healthy
Healthy ≠ Fully Verified
Booted ≠ Known-Good
Verification Failure ≠ Undefined State
Unknown ≠ Verified
```

## Verification Model

```text
UpdateVerification
├── VerificationID
├── UpdateID
├── TargetID
├── ExpectedVersion
├── ExpectedContentID
├── VerificationPlan
├── VerificationState
└── Evidence
```

Optional:

```text
BuildID
PackageID
TransactionID
ExecutionContractID
HealthPolicy
SecurityPolicy
RollbackPolicy
ProvenanceID
```

## Verification States

```text
Pending
Running
Verified
Failed
Partial
Unavailable
Unknown
```

Nur `Verified` darf vollständigen Verifikationserfolg ausdrücken.

## Verification Phases

NovaOS unterscheidet mehrere Verifikationszeitpunkte:

```text
Pre-Installation
      ↓
Staging
      ↓
Pre-Activation
      ↓
Post-Activation
      ↓
Operational
```

Eine frühere erfolgreiche Prüfung ersetzt keine erforderliche spätere Prüfung.

## Pre-Installation Verification

Vor Installation werden mindestens relevante Eigenschaften geprüft:

```text
Package Integrity
ContentID
Signature
Signer Trust
Repository Metadata
Authorization
Dependencies
Compatibility
Security Policy
```

Fehler müssen die Installation blockieren können.

## Pre-Activation Verification

Unmittelbar vor Aktivierung müssen dynamische Bedingungen erneut geprüft werden.

```text
Current State Version
Trust State
Revocation State
Capabilities
Dependencies
Resource Availability
Security Policy
Sovereignty Policy
```

```text
Previously Valid ≠ Currently Valid
```

## Post-Activation Verification

Nach Aktivierung muss geprüft werden, ob tatsächlich der erwartete Zustand aktiv ist.

```text
Expected Version
Expected BuildID
Expected ContentID
Expected Configuration
Expected Provider
Expected Boot State
```

## Health Verification

Zusätzlich zur technischen Aktivierung muss die Betriebsfähigkeit geprüft werden.

Mögliche Kriterien:

```text
Component Responsive
Required Services Available
Driver Operational
Device Responsive
IPC Functional
Required Capabilities Available
Contracts Satisfied
No Critical Errors
Resource Usage within Limits
```

```text
Process Running ≠ Service Healthy
```

## Contract Verification

Updates dürfen bestehende Systemverträge nicht unkontrolliert verletzen.

Zu prüfen sind bei Bedarf:

```text
API Contract
ABI Contract
Execution Contract
Resource Contract
Temporal Contract
Security Contract
State Contract
Capability Contract
```

## Boot Verification

Bei bootkritischen Updates:

```text
Bootloader
   ↓
Kernel Entry
   ↓
Required Boot State
   ↓
Core Services
   ↓
Health Verification
   ↓
Known-Good
```

Ein erfolgreicher Kernel Entry allein reicht nicht zwingend für `Known-Good`.

## Driver Verification

Nach Driver Update können geprüft werden:

```text
Driver Loaded
Device Bound
Device Responsive
I/O Functional
DMA State Valid
Capability State Valid
```

## Firmware Verification

Nach Firmware Update können geprüft werden:

```text
Reported Firmware Version
Device Identity
Device Health
Driver Communication
Operational Test
```

```text
Firmware Written ≠ Firmware Healthy
```

## Live Update Verification

Bei Live Update oder Hotpatch:

```text
New Component Active
      ↓
State Valid
      ↓
Contracts Valid
      ↓
Requests Successful
      ↓
Old Component Retirable
```

Die alte Instanz soll nicht endgültig entfernt werden, bevor die definierte Verifikation abgeschlossen ist, soweit der Update-Modus dies zulässt.

## Distributed Verification

Bei Rolling- und Canary-Updates muss Verifikation pro Instanz beziehungsweise Batch möglich sein.

```text
Instance
   ↓
Verify
   ↓
Batch
   ↓
Verify
   ↓
Deployment
```

```text
One Healthy Instance
≠
Healthy Deployment
```

## Verification Evidence

Verifikation muss nachvollziehbare Evidence erzeugen können.

```text
VerificationID
TargetID
Version
BuildID
ContentID
Checks
Results
Timestamp
StateVersion
```

Evidence muss dem tatsächlich geprüften Artefakt beziehungsweise Zustand zuordenbar sein.

## Failure Handling

Bei fehlgeschlagener Verifikation können abhängig von Update und Policy erfolgen:

```text
Retry Verification
Pause Update
Contain Component
Rollback
A/B Fallback
Disable Device
Restore Snapshot
Recovery Mode
Manual Intervention
```

Die kleinstmögliche ausreichende Recovery-Ebene soll bevorzugt werden.

## Unknown State

Kann ein kritischer Zustand nicht verifiziert werden:

```text
Unknown
≠
Healthy
≠
Verified
```

Bei sicherheitskritischen Komponenten muss `Unknown` einen Commit blockieren können.

## Transaction Integration

```text
Begin
 ↓
Prepare
 ↓
Apply
 ↓
Activate
 ↓
Verify
 ↓
Commit
```

Abhängig vom Update-Modell kann ein vorläufiger Commit technisch notwendig sein. Die Transaktion bleibt dann logisch `Verification Pending`, bis die erforderliche Post-Activation-Verifikation abgeschlossen wurde.

## Rollback Integration

```text
Verification Failed
       ↓
Rollback
       ↓
Verify Rollback State
```

Auch ein erfolgreicher Rollback gilt erst nach Verifikation des wiederhergestellten Zustands als abgeschlossen.

## Security

Update Verification darf Security Policy nicht umgehen.

Besonders zu berücksichtigen sind:

```text
Revocation
Minimum Secure Version
Code Integrity
Capability Authority
Trust State
Measured Boot
Secure Boot
```

Rollback darf keine inzwischen widerrufene oder verbotene Version als erfolgreich verifiziert markieren.

## Resource Economy

Verifikation muss ihren Ressourcenverbrauch deklarieren und begrenzen können.

Aufwendige Prüfungen können abhängig von Kritikalität gestaffelt werden.

```text
Critical Component
→ Strong Verification

Low-Risk Component
→ Proportional Verification
```

Harte Sicherheitsanforderungen dürfen aus Performancegründen nicht übersprungen werden.

## Provenance

NovaOS soll nachvollziehen können:

```text
VerificationID
UpdateID
TargetID
Expected State
Observed State
Checks Performed
Evidence
Result
Failure Reason
Recovery Action
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Verification State
Verification Phase
Expected Version
Observed Version
Integrity State
Trust State
Health State
Contract State
Evidence
Failure Reason
Recovery State
```

## Normative Anforderungen

1. NovaOS MUSS Updates vor und nach kritischer Aktivierung verifizieren können.
2. Verifikation MUSS eindeutig einem Update und Ziel zugeordnet werden.
3. Integrität MUSS vor Installation prüfbar sein.
4. Signatur, Trust und Authorization MÜSSEN bei sicherheitsrelevanten Updates geprüft werden.
5. Dynamische Bedingungen MÜSSEN vor Aktivierung revalidierbar sein.
6. Der tatsächlich aktive Zustand MUSS nach Aktivierung überprüfbar sein.
7. `Installed` DARF NICHT als `Verified` interpretiert werden.
8. `Active` DARF NICHT als `Healthy` interpretiert werden.
9. `Unknown` DARF NICHT als `Verified` interpretiert werden.
10. Kritische Contract-Verletzungen MÜSSEN Verifikation fehlschlagen lassen können.
11. Bootkritische Updates MÜSSEN Boot-Health-Verifikation unterstützen.
12. Driver Updates MÜSSEN Device-Health-Verifikation unterstützen können.
13. Firmware Updates MÜSSEN den aktivierten Firmwarezustand prüfen können.
14. Live Updates MÜSSEN den neuen Laufzeitzustand verifizieren können.
15. Rolling- und Canary-Updates MÜSSEN instanz- beziehungsweise batchweise verifizierbar sein.
16. Verifikation MUSS nachvollziehbare Evidence erzeugen können.
17. Evidence MUSS an den tatsächlich geprüften Zustand gebunden sein.
18. Fehlgeschlagene Verifikation MUSS Update-Fortschritt stoppen können.
19. Kritisches `Unknown` MUSS Commit oder Promotion blockieren können.
20. Rollback-Zustände MÜSSEN erneut verifiziert werden.
21. Security- und Anti-Rollback-Regeln MÜSSEN auch während Verifikation gelten.
22. Verifikation MUSS in Update-Transaktionen integrierbar sein.
23. Verifikationsaufwand SOLL proportional zur Kritikalität skalierbar sein.
24. Harte Sicherheitsprüfungen DÜRFEN NICHT aus Performancegründen entfallen.
25. Verification-State MUSS autorisiert introspektierbar sein.
26. Verifikationsergebnisse MÜSSEN nachvollziehbare Provenance besitzen.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-AB-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-LIVE-0001`
- `NPSPEC-UPDATE-HOTPATCH-0001`
- `NPSPEC-UPDATE-ROLLING-0001`
- `NPSPEC-UPDATE-CANARY-0001`
- `NPSPEC-UPDATE-STAGED-0001`
- `NPSPEC-UPDATE-DRIVER-0001`
- `NPSPEC-UPDATE-FIRMWARE-0001`
- `NPSPEC-UPDATE-BOOTLOADER-0001`
- `NPSPEC-VERIFY-RUNTIME-0001`
- `NPSPEC-VERIFY-CONTRACT-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0184`

## Ergebnis

```text
Update
  ↓
Pre-Verify
  ↓
Stage
  ↓
Revalidate
  ↓
Activate
  ↓
Post-Verify
  ↓
Health + Contract Verification
     ↙                 ↘
 Verified             Failed / Unknown
    ↓                       ↓
 Commit               Stop / Rollback /
                      Recovery
```

NovaOS erhält damit eine gemeinsame Verifikationsschicht für den gesamten Update-Lebenszyklus. Ein Update gilt nicht allein deshalb als erfolgreich, weil neue Daten geschrieben oder Komponenten gestartet wurden, sondern erst nach überprüfbarer Bestätigung des erwarteten, sicheren und funktionsfähigen Systemzustands.