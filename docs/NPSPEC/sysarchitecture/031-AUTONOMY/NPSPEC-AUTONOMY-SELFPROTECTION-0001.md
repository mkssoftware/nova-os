# NPSPEC-AUTONOMY-SELFPROTECTION-0001 – Nova Autonomous Self-Protection

## Status

Angenommen

## Kategorie

Autonomy / Self-Protection / Security / Resilience

## Zweck

NovaOS definiert einen autonomen Self-Protection-Mechanismus, der Bedrohungen, kompromittierte Komponenten und sicherheitskritische Zustandsänderungen erkennen, eindämmen und kontrolliert darauf reagieren kann.

```text
Observe
   ↓
Detect
   ↓
Assess
   ↓
Contain
   ↓
Protect
   ↓
Verify
```

Self-Protection ergänzt bestehende Security-Mechanismen, ersetzt sie jedoch nicht.

## Grundprinzipien

```text
Anomaly ≠ Attack
Suspicion ≠ Proof
Detection ≠ Authorization
Self-Protection ≠ Unlimited Authority
Isolation ≠ Deletion
Protection ≠ Recovery

Unknown Trust ≠ Trusted
Unknown Integrity ≠ Valid
```

## Protection Model

```text
ProtectionOperation
├── ProtectionID
├── Target
├── Trigger
├── ThreatState
├── ProtectionPlan
├── RiskClass
└── State
```

Optional:

```text
IdentityID
ObjectID
ResourceID
ExecutionID
ProviderID
DeviceID
CapabilityID
TrustState
PolicyID
ProvenanceID
```

## Protection Targets

Self-Protection kann unter anderem schützen:

```text
Kernel
Processes
Services
Drivers
Applications
Objects
Credentials
Capabilities
Storage
Network
Devices
System Configuration
Boot State
```

## Detection

Self-Protection kann Signale aus mehreren Quellen kombinieren:

```text
Integrity Checks
Code Integrity
Trust State
Security Audit
Runtime Contracts
Information Flow
Behavioral Anomalies
Resource Anomalies
Network Events
Boot Measurements
```

Ein einzelnes ungewöhnliches Ereignis darf nicht automatisch als bestätigter Angriff gelten.

## Threat States

NovaOS definiert mindestens:

```text
Normal
Suspicious
Threatened
Compromised
Contained
Unknown
```

`Unknown` darf bei sicherheitskritischen Entscheidungen nicht automatisch als `Normal` behandelt werden.

## Protection Actions

Abhängig von Policy und Authority können Maßnahmen sein:

```text
Block Operation
Revoke Capability
Restrict Capability
Terminate Execution
Suspend Execution
Isolate Process
Isolate Driver
Disable Provider
Block Network Path
Protect Object
Switch Provider
Enter Restricted Mode
Trigger Self-Healing
Trigger Recovery
```

## Containment

Containment besitzt Vorrang vor unkontrollierter Reparatur.

```text
Threat
  ↓
Contain
  ↓
Prevent Propagation
  ↓
Assess
  ↓
Recover
```

Kompromittierte Komponenten sollen möglichst isoliert werden, bevor weitere Systembereiche betroffen werden.

## Capability Protection

Bei kompromittierter Authority kann NovaOS:

```text
Revoke
Attenuate
Expire
Invalidate Handles
Block Delegation
```

Capability-Revocation muss die bestehenden Capability-Sicherheitsregeln einhalten.

## Trust Integration

Ändert sich der Trust-Zustand:

```text
Trusted
   ↓
Evidence Change
   ↓
Untrusted / Unknown
```

müssen davon abhängige Entscheidungen neu bewertet werden können.

Trust allein erzeugt oder entfernt jedoch keine Authority.

## Integrity Protection

Self-Protection kann Integritätsverletzungen erkennen und darauf reagieren.

```text
Expected State
      ↓
Integrity Check
      ↓
Mismatch
      ↓
Containment
```

Eine erkannte Abweichung darf nicht automatisch überschrieben werden, wenn dadurch forensische oder Recovery-relevante Informationen verloren gehen könnten.

## Autonomous Response

Die Autonomy Policy bestimmt den erlaubten Reaktionsumfang.

```text
Low Risk Protection
→ Automatic

High Impact Protection
→ Policy Controlled

Irreversible Action
→ Strong Authorization
```

Akute Safety- oder Security-Policies können ausdrücklich definierte sofortige Schutzmaßnahmen erlauben.

## User Override

User Overrides dürfen Self-Protection nur innerhalb der erlaubten Security Policy beeinflussen.

```text
User Override
      ↓
Security Validation
      ↓
Allowed / Rejected
```

Ein User Override darf keine zwingende Sicherheitsgrenze aufheben.

## Self-Healing Integration

Self-Protection und Self-Healing bleiben getrennte Funktionen.

```text
Self-Protection
→ Detect + Contain

Self-Healing
→ Diagnose + Repair + Recover
```

Sie können jedoch eine gemeinsame Recovery-Kette bilden.

## Escalation

Bei unklaren oder nicht sicher behandelbaren Zuständen:

```text
Unknown Threat
      ↓
Restrict
      ↓
Preserve Evidence
      ↓
Escalate
```

NovaOS darf Unsicherheit nicht durch riskante autonome Aktionen verdecken.

## Safe Mode

Bei systemweiter Gefährdung kann NovaOS in einen eingeschränkten Zustand wechseln.

```text
Normal Mode
    ↓
Critical Threat
    ↓
Restricted / Safe Mode
```

Nur notwendige und vertrauenswürdige Komponenten sollen dabei aktiv bleiben.

## Provenance und Audit

Relevante Schutzmaßnahmen sollen erfassen:

```text
Trigger
Evidence
Target
Decision
Action
Authority
Previous State
Result
Timestamp
Policy
Provenance
```

Sicherheitsrelevante Geheimnisse dürfen dabei nicht ungeschützt protokolliert werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
ProtectionID
Target
Threat State
Evidence
Confidence
Protection Action
Containment State
Capability Changes
Trust Changes
Verification
Escalation
Policy Version
Provenance
```

## Normative Anforderungen

1. NovaOS MUSS Self-Protection durch Security Policy, Autonomy Policy und Constraints begrenzen.
2. Self-Protection DARF keine unbegrenzte Authority besitzen.
3. Anomalien DÜRFEN NICHT automatisch als bestätigte Angriffe behandelt werden.
4. `Unknown` DARF bei sicherheitskritischen Entscheidungen NICHT automatisch als sicher gelten.
5. Self-Protection MUSS kompromittierte Komponenten isolieren können.
6. Containment SOLL vor riskanter autonomer Reparatur erfolgen.
7. Capability-Revocation und Attenuation MÜSSEN unterstützt werden können.
8. Änderungen des Trust-Zustands MÜSSEN abhängige Entscheidungen revalidierbar machen.
9. Integritätsverletzungen MÜSSEN kontrollierte Schutzmaßnahmen auslösen können.
10. Self-Protection und Self-Healing MÜSSEN logisch getrennt bleiben.
11. Zwingende Security Constraints DÜRFEN durch User Overrides NICHT aufgehoben werden.
12. Irreversible Schutzmaßnahmen MÜSSEN entsprechend ihrer Auswirkungen stärker autorisiert werden können.
13. Self-Protection SOLL forensisch relevante Informationen erhalten können.
14. Kritische Bedrohungen MÜSSEN einen Restricted oder Safe Mode auslösen können.
15. Fehlende sichere Gegenmaßnahmen MÜSSEN Eskalation ermöglichen.
16. Schutzmaßnahmen MÜSSEN nach Möglichkeit auf ihre Wirksamkeit verifiziert werden.
17. Relevante Schutzentscheidungen MÜSSEN Audit und Provenance unterstützen.
18. Self-Protection MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-AUTONOMY-POLICY-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-ADAPTIVE-USEROVERRIDE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-SECURITY-INFORMATIONFLOW-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-TRUST-ATTESTATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-BOOT-SECURE-0001`
- `NPSPEC-BOOT-MEASURED-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `ADR-ARCH-0098`

## Ergebnis

```text
Security Evidence
       ↓
Threat Assessment
       ↓
Authority + Constraints
       ↓
Containment
       ↓
Protection
       ↓
Verification
       ↓
┌──────────┬──────────┬────────────┐
│ Continue │ Recovery │ Escalation │
└──────────┴──────────┴────────────┘
```

NovaOS erhält damit eine autonome Schutzschicht, die Bedrohungen frühzeitig erkennen und eindämmen kann, ohne Verdacht mit Beweis oder Autonomie mit unbegrenzter Authority gleichzusetzen. Self-Protection bildet zusammen mit Isolation, Capability Security, Trust, Self-Healing und Recovery eine kontrollierte Verteidigungskette.