# ADR-RESILIENCE-0019 – Recovery Autonomy Levels

## Status

Angenommen

## Kategorie

Resilience Architecture / Recovery Governance / Autonomy

## Kontext

Nova.Resilience kann viele Fehler automatisch erkennen, klassifizieren und beheben. Nicht jede Recovery-Maßnahme darf jedoch mit demselben Grad an Autonomie ausgeführt werden.

Ein lokaler Neustart eines zustandslosen Dienstes besitzt andere Auswirkungen als beispielsweise:

- Rollback persistenter Daten,
- Austausch kritischer Provider,
- Credential Rotation,
- systemweiter Failover,
- Disaster Recovery,
- Safety-relevante Eingriffe,
- irreversible Recovery-Aktionen.

Eine vollständig autonome Recovery wäre für bestimmte Operationen zu riskant. Eine grundsätzlich manuelle Freigabe würde dagegen das Self-Healing-Modell erheblich einschränken.

NovaOS benötigt deshalb explizite Recovery Autonomy Levels.

## Entscheidung

Nova.Resilience definiert fünf Recovery Autonomy Levels:

```text
L0 – Observe
L1 – Recommend
L2 – Approve
L3 – Autonomous
L4 – Emergency Autonomous
```

Der zulässige Autonomiegrad wird durch Recovery Policy, Recovery Contract, Authority und geltende Hard Constraints bestimmt.

## Grundprinzip

```text
Recovery Autonomy
≠
Recovery Authority
```

Ein höherer Autonomiegrad erlaubt selbstständige Entscheidungen innerhalb eines definierten Rahmens, erzeugt jedoch keine zusätzliche Authority.

## L0 – Observe

Nova.Resilience erkennt und analysiert den Fehler, führt aber keine Recovery aus.

```text
Detect
  ↓
Classify
  ↓
Report
```

Dieser Modus eignet sich insbesondere für Diagnose, Entwicklung oder besonders sensible Komponenten.

## L1 – Recommend

Nova.Resilience erstellt einen Recovery Plan und stellt diesen als Empfehlung bereit.

```text
Failure
   ↓
Recovery Plan
   ↓
Recommendation
```

Die eigentliche Ausführung erfolgt durch eine autorisierte externe Entscheidung.

## L2 – Approve

Nova.Resilience darf Recovery vorbereiten, benötigt vor der relevanten Aktion jedoch eine explizite Freigabe.

```text
Plan
  ↓
Prepare
  ↓
Approval
  ↓
Recover
```

Die Freigabe muss an den konkreten Recovery Plan gebunden sein.

## L3 – Autonomous

Nova.Resilience darf einen zulässigen Recovery Plan vollständig selbstständig ausführen.

```text
Detect
  ↓
Classify
  ↓
Plan
  ↓
Recover
  ↓
Verify
  ↓
Restore
```

Dies ist der reguläre Self-Healing-Modus für bekannte und ausreichend begrenzte Recovery-Pfade.

## L4 – Emergency Autonomous

Für vorab definierte Safety-, Security- oder Systemschutzmaßnahmen darf Nova.Resilience ohne vorherige Freigabe handeln.

Beispiele:

```text
Enter Safe State
Quarantine Workload
Contain Critical Failure
Revoke Dangerous Capability
Protect System Integrity
```

`L4` ist kein allgemeiner Superuser-Modus.

Er erlaubt ausschließlich explizit definierte Notfallmaßnahmen innerhalb ihrer vorgesehenen Authority.

## Policy

Der Autonomy Level kann abhängig von folgenden Eigenschaften bestimmt werden:

```text
Component
Failure Class
Recovery Action
Failure Domain
Recovery Scope
Recovery Contract
Security Context
Safety Context
```

Dieselbe Komponente kann daher für unterschiedliche Recovery-Aktionen unterschiedliche Levels besitzen.

## Recovery Scope

Eine Eskalation des Recovery Scope darf nicht automatisch den bisherigen Autonomy Level übernehmen.

Beispiel:

```text
Task Restart
    → L3 Autonomous

Service Rollback
    → L2 Approve
```

Jede relevante Scope-Eskalation muss erneut gegen Autonomy Policy und Authority geprüft werden.

## Authority

Autonome Recovery darf nur innerhalb vorhandener oder explizit delegierter Recovery Authority erfolgen.

```text
Autonomy
    +
Recovery Authority
    +
Policy
    ↓
Permitted Recovery
```

Nova.Resilience darf fehlende Capabilities nicht durch einen höheren Autonomy Level ersetzen.

## Approval

Eine Approval gilt nur für den dafür freigegebenen Recovery Plan und Scope.

Eine Freigabe für:

```text
Restart Service A
```

darf nicht automatisch als Freigabe für:

```text
Rollback Subsystem A
```

interpretiert werden.

Ändert sich der Plan wesentlich, muss eine erneute Freigabe möglich sein.

## Approval Timeout

Das Ausbleiben einer Approval darf nicht als Zustimmung interpretiert werden.

```text
Approval Timeout
      ↓
No implicit approval
```

Ist gleichzeitig eine vorab definierte Safety- oder Security-Notfallmaßnahme erforderlich, kann diese über `L4` ausgeführt werden.

## Irreversible Recovery

Irreversible oder schwer rückgängig zu machende Recovery-Aktionen sollen strengere Autonomy Policies verwenden.

Beispiele:

```text
Permanent Data Deletion
Destructive Storage Repair
Credential Destruction
External Side Effects
Physical Actuation
```

Automatische Ausführung ist nur zulässig, wenn dies ausdrücklich durch den Recovery Contract erlaubt wurde.

## Security Recovery

Security Failures können für reine Containment-Aktionen einen höheren Autonomiegrad besitzen als für die anschließende Remediation.

Beispiel:

```text
Potential Compromise
       ↓
Quarantine          → L4
       ↓
Credential Rotation → L2
       ↓
Restore             → L2 / L3
```

Damit kann NovaOS unmittelbar schützen, ohne weitreichende Folgeaktionen automatisch durchzuführen.

## Safety

Ein notwendiger Safe-State-Eintritt muss autonom möglich sein, wenn eine menschliche Freigabe nicht innerhalb der zulässigen Safety Deadline erreichbar wäre.

```text
Safety Violation
      ↓
L4 Emergency Autonomous
      ↓
Safe State
```

Die Rückkehr aus dem Safe State kann einen niedrigeren Autonomy Level beziehungsweise eine explizite Approval verlangen.

## Disaster Recovery

Deklarative Disaster-Recovery-Pläne können unterschiedliche Autonomy Levels für einzelne Phasen definieren.

Beispiel:

```text
Containment      → L4
Core Recovery    → L3
Data Rollback    → L2
Optional Restore → L3
```

Damit wird Autonomie nicht pauschal auf den gesamten Disaster-Recovery-Prozess angewendet.

## Adaptive Systeme

Adaptive Systeme dürfen:

- Recovery-Strategien bewerten,
- Empfehlungen verbessern,
- Erfolgswahrscheinlichkeiten schätzen,
- Recovery-Pläne optimieren.

Sie dürfen den zulässigen Autonomy Level jedoch nicht selbstständig erhöhen.

```text
Prediction Confidence
        ≠
Additional Autonomy
```

Änderungen der Autonomy Policy benötigen einen autorisierten Policy-Mechanismus.

## Verification

Der Autonomy Level verändert nicht die Anforderungen an Recovery Verification.

```text
Recover
   ↓
Verify
   ↓
Restore
```

Auch `L3`- oder `L4`-Recovery darf erst nach ausreichender Verification als erfolgreich gelten.

## Normative Anforderungen

1. NovaOS MUSS explizite Recovery Autonomy Levels unterstützen.
2. Nova.Resilience MUSS mindestens `L0 Observe`, `L1 Recommend`, `L2 Approve`, `L3 Autonomous` und `L4 Emergency Autonomous` unterscheiden.
3. Autonomy Levels MÜSSEN an Recovery Policies und Recovery Contracts bindbar sein.
4. Unterschiedliche Recovery-Aktionen derselben Komponente DÜRFEN unterschiedliche Autonomy Levels besitzen.
5. Recovery Autonomy DARF keine fehlende Authority ersetzen.
6. Autonome Recovery MUSS innerhalb vorhandener Capabilities und Hard Constraints bleiben.
7. Approval MUSS an einen konkreten Recovery Plan und Scope bindbar sein.
8. Wesentliche Planänderungen MÜSSEN eine erneute Approval-Prüfung auslösen können.
9. Ausbleibende Approval DARF nicht als Zustimmung interpretiert werden.
10. Recovery-Scope-Eskalation MUSS eine erneute Autonomy- und Authority-Prüfung ermöglichen.
11. Irreversible Recovery-Aktionen SOLLEN strengere Autonomy Policies verwenden.
12. `L4` DARF ausschließlich für explizit definierte Notfallmaßnahmen verwendet werden.
13. Security Containment MUSS als `L4`-Aktion definierbar sein.
14. Safety-kritischer Safe-State-Eintritt MUSS autonom möglich sein können, wenn eine Approval die Safety Deadline verletzen würde.
15. Die Rückkehr aus einem Safe State DARF einen anderen Autonomy Level als dessen Eintritt besitzen.
16. Disaster Recovery MUSS unterschiedliche Autonomy Levels pro Recovery-Phase unterstützen können.
17. Adaptive Systeme DÜRFEN den Autonomy Level nicht selbstständig erhöhen.
18. Recovery Verification MUSS unabhängig vom Autonomy Level erhalten bleiben.
19. Autonomy Policies DÜRFEN Security-, Trust-, Integrity-, Capability-, Safety- oder Data-Sovereignty-Hard-Constraints nicht umgehen.
20. Autonomy-Entscheidungen, Approvals und autonome Recovery-Aktionen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- sichere Alltags-Recovery kann vollständig autonom erfolgen,
- riskante Recovery-Aktionen bleiben kontrolliert,
- Security- und Safety-Notfallmaßnahmen können unmittelbar reagieren,
- Autonomie kann je Recovery-Aktion und Scope angepasst werden,
- menschliche Freigaben werden nur dort benötigt, wo sie architektonisch erforderlich sind.

### Negative Konsequenzen

- Recovery Policies müssen zulässige Autonomy Levels definieren,
- Scope-Eskalationen können zusätzliche Approval-Schritte erzeugen,
- falsch konfigurierte Autonomy Policies können Recovery unnötig verzögern.

## Verworfene Alternativen

### Vollständig autonome Recovery für alle Fehler

Verworfen, da bestimmte Recovery-Aktionen irreversible oder sehr weitreichende Auswirkungen besitzen.

### Jede Recovery benötigt manuelle Freigabe

Verworfen, da dadurch Self-Healing, schnelle Fehlerbehandlung und Realtime-Recovery stark eingeschränkt würden.

### Autonomie allein anhand eines Risiko-Scores

Verworfen, da Authority, Safety, Security und explizite Policies nicht auf einen einzelnen numerischen Wert reduziert werden sollen.

### Adaptive Systeme dürfen Autonomie selbst erhöhen

Verworfen, da Prediction Confidence keine zusätzliche Authority oder Governance-Freigabe erzeugt.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0014_Security_Failures_durch_Quarantine_statt_blindem_Restart`
- `ADR-RESILIENCE-0016_Safe_State_für_Safety_und_Realtime`
- `ADR-RESILIENCE-0017_RTO_und_RPO_als_Recovery_Contracts`
- `ADR-RESILIENCE-0018_Deklarative_Disaster_Recovery`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-CAPABILITY-0008_Capability_Attenuation`
- `ADR-SECURITY-0017_Break_Glass_Access_mit_Audit`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-AUTONOMY-0001`
- `NPSPEC-RESILIENCE-AUTONOMY-POLICY-0001`
- `NPSPEC-RESILIENCE-RECOVERY-APPROVAL-0001`
- `NPSPEC-RESILIENCE-EMERGENCY-AUTONOMY-0001`
- `NPSPEC-RESILIENCE-AUTONOMY-ESCALATION-0001`
- `NPSPEC-RESILIENCE-AUTONOMY-AUDIT-0001`
- `NPSPEC-RESILIENCE-AUTONOMY-TEST-0001`

## Ergebnis

Nova.Resilience verwendet abgestufte Autonomie statt einer binären Entscheidung zwischen vollständig manueller und vollständig automatischer Recovery:

```text
Failure
   ↓
Recovery Plan
   ↓
Policy + Authority + Scope
   ↓
Autonomy Level
   ├── L0 Observe
   ├── L1 Recommend
   ├── L2 Approve
   ├── L3 Autonomous
   └── L4 Emergency Autonomous
              ↓
           Recover
              ↓
           Verify
              ↓
           Restore
```

Damit kann NovaOS sichere und bekannte Recovery-Pfade selbstständig ausführen, während riskante, irreversible oder weitreichende Maßnahmen kontrolliert bleiben und notwendige Safety- oder Security-Notfallmaßnahmen trotzdem unmittelbar möglich sind.