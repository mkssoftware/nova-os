# ADR-RESILIENCE-0014 – Security Failures durch Quarantine statt blindem Restart

## Status

Angenommen

## Kategorie

Resilience Architecture / Security Recovery / Quarantine

## Kontext

Nicht jeder Fehler ist ein gewöhnlicher Betriebsfehler.

Bestimmte Fehler können auf eine Sicherheitsverletzung, Manipulation oder kompromittierte Ausführungsumgebung hinweisen.

Beispiele:

- Integrity Violations,
- ungültige oder unerwartete Code-Signaturen,
- Attestation Failures,
- verdächtige Capability-Nutzung,
- Manipulation von Systemkomponenten,
- unerwartete Privilege Escalation,
- kompromittierte Credentials,
- Policy-Verletzungen,
- verdächtige Datenflüsse.

Ein einfacher Neustart der betroffenen Komponente kann in solchen Fällen gefährlich sein.

```text
Security Failure
      ↓
Restart
      ↓
Compromised State returns
```

Zusätzlich können durch einen Restart wichtige Evidence verloren gehen.

## Entscheidung

Nova.Resilience behandelt sicherheitsrelevante Fehler grundsätzlich über **Containment und Quarantine**, bevor eine normale Recovery oder ein Restart zugelassen wird.

```text
Security Failure
      ↓
Contain
      ↓
Quarantine
      ↓
Preserve Evidence
      ↓
Security / Trust Evaluation
      ↓
Recovery Decision
```

Ein Restart gilt nicht als Sicherheitsbereinigung.

## Grundprinzip

```text
Restart
≠
Remediation
```

Bei möglicher Kompromittierung gilt:

```text
Contain first.
Preserve evidence.
Re-establish trust.
Recover afterwards.
```

## Security Failure Classification

Nova.Resilience muss sicherheitsrelevante Fehler von gewöhnlichen Betriebsfehlern unterscheiden können.

Beispiele:

```text
Operational Failure
Integrity Failure
Trust Failure
Authentication Failure
Authorization Violation
Attestation Failure
Suspicious Behavior
Potential Compromise
Confirmed Compromise
```

Die Klassifikation bestimmt den zulässigen Recovery-Pfad.

## Quarantine

Quarantine isoliert eine verdächtige Komponente von anderen Systembereichen.

Abhängig vom Scope kann dies beispielsweise bedeuten:

```text
Suspend Execution
Block IPC
Revoke Capabilities
Stop Data Flow
Remove Provider
Restrict Network Access
Detach Shared Resources
Block Secret Access
```

Die konkrete Isolation muss zum betroffenen Failure Scope passen.

## Hierarchischer Scope

Quarantine folgt den hierarchischen Failure Domains.

```text
Task
 ↓
Workload
 ↓
Service
 ↓
Subsystem
 ↓
System
```

NovaOS soll grundsätzlich den kleinsten sicheren Quarantine Scope verwenden.

Wenn eine Kompromittierung möglicherweise eine größere Failure Domain betrifft, muss die Quarantine entsprechend erweitert werden können.

## Capability Revocation

Quarantine muss mit dem Capability-System integriert sein.

```text
Compromised Workload
        ↓
Quarantine
        ↓
Capability Revocation
        ↓
No Further Authority
```

Bereits ausgegebene Handles, Leases oder delegierte Capabilities müssen entsprechend ihrer Sicherheitssemantik widerrufbar sein.

## Secret Protection

Eine quarantänisierte Workload darf keine neuen schützenswerten Secrets erhalten.

```text
Quarantined Workload
        ↓
Secret Request
        ↓
Deny
```

Bereits vorhandene Secret Handles müssen bei relevanter Kompromittierung invalidierbar sein.

Workload Attestation kann vor einer späteren erneuten Secret-Freigabe erforderlich sein.

## Evidence Preservation

Vor destruktiven Recovery-Aktionen sollen relevante Security- und Diagnoseinformationen erhalten werden.

Dazu können gehören:

```text
Integrity Evidence
Attestation Evidence
Relevant Logs
Execution State
Security Events
Causation Information
Affected Identities
Capability State
```

Preservation muss Privacy-, Secret-Protection- und Data-Sovereignty-Regeln einhalten.

## Restart

Ein Restart darf nach einem Security Failure nur erfolgen, wenn er Teil eines definierten Remediation Plans ist.

```text
Quarantine
    ↓
Evaluate
    ↓
Remediate
    ↓
Restart / Replace / Rebuild
    ↓
Verify
```

Das bloße Beenden und erneute Starten derselben möglicherweise kompromittierten Komponente ist keine ausreichende Recovery.

## Replacement

Bei kompromittierter Software kann ein vertrauenswürdiger Ersatz gegenüber einem Restart bevorzugt werden.

```text
Compromised Provider
       ↓
Quarantine
       ↓
Trusted Provider Replacement
       ↓
Verification
       ↓
Restore
```

Der Ersatz muss die notwendigen Integrity-, Trust- und Capability-Anforderungen erfüllen.

## Re-establish Trust

Vor dem Restore muss der erforderliche Trust-Zustand erneut hergestellt werden.

Dies kann abhängig vom Fall beispielsweise umfassen:

```text
Integrity Verification
Code Verification
Provenance Verification
Attestation
Credential Rotation
Capability Reissue
State Reconstruction
```

Ein vorheriger Trust-Zustand darf nach einer relevanten Kompromittierung nicht blind übernommen werden.

## Quarantine State

Quarantine und Health State bleiben getrennte Konzepte.

Eine Komponente kann beispielsweise sein:

```text
Health = Healthy
Security = Compromised
Quarantine = Active
```

Technische Funktionsfähigkeit darf eine Security Quarantine nicht automatisch aufheben.

## Restore

Eine quarantänisierte Komponente darf erst wieder in den normalen Betrieb aufgenommen werden, wenn die erforderliche Recovery Verification erfolgreich war.

```text
Quarantine
    ↓
Remediation
    ↓
Verification
    ↓
Trust Re-established?
 ├── Yes → Restore
 └── No  → Remain Quarantined / Escalate
```

## Fail Closed

Kann der Sicherheitszustand nicht ausreichend bestimmt oder wiederhergestellt werden, bleibt die betroffene Komponente isoliert.

```text
Security State Unknown
        ↓
Cannot Verify
        ↓
Remain Quarantined
```

Verfügbarkeit hat in diesem Fall keinen Vorrang vor Security und Integrity.

## Benutzer- und Administratorinteraktion

Manuelle Freigaben dürfen Quarantine nicht unkontrolliert umgehen.

Für außergewöhnliche administrative Eingriffe kann der definierte Break-Glass-Mechanismus verwendet werden.

Solche Eingriffe müssen explizit autorisiert und auditierbar sein.

## Normative Anforderungen

1. Nova.Resilience MUSS sicherheitsrelevante Fehler von gewöhnlichen Betriebsfehlern unterscheiden können.
2. Potenzielle Kompromittierungen MÜSSEN vor normaler Recovery containbar sein.
3. NovaOS MUSS Quarantine als expliziten Resilience-Zustand unterstützen.
4. Ein Restart DARF nicht automatisch als Remediation eines Security Failures gelten.
5. Quarantine SOLL den kleinsten sicheren Scope verwenden.
6. Der Quarantine Scope MUSS bei möglicher größerer Kompromittierung eskalierbar sein.
7. Quarantine MUSS Capability-Nutzung begrenzen oder widerrufen können.
8. Quarantänisierte Workloads DÜRFEN keine neuen Secrets erhalten, solange die erforderlichen Trust-Bedingungen nicht wiederhergestellt sind.
9. Relevante Secret Handles MÜSSEN bei Kompromittierung invalidierbar sein.
10. Relevante Security Evidence SOLL vor destruktiver Recovery erhalten werden.
11. Evidence Preservation MUSS Privacy-, Security- und Data-Sovereignty-Regeln einhalten.
12. Restart, Replacement oder Rebuild nach Security Failures MÜSSEN Teil eines definierten Remediation Plans sein.
13. Vor Restore MUSS der erforderliche Integrity- und Trust-Zustand erneut verifiziert werden.
14. Frühere Trust-Entscheidungen DÜRFEN nach relevanter Kompromittierung nicht blind wiederverwendet werden.
15. Health State und Quarantine State MÜSSEN getrennt bleiben.
16. Technische Funktionsfähigkeit DARF eine aktive Security Quarantine nicht automatisch aufheben.
17. Fehlgeschlagene oder unmögliche Security Verification MUSS zu fortgesetzter Quarantine oder Eskalation führen.
18. Quarantine DARF nur durch autorisierte und kontrollierte Mechanismen aufgehoben werden.
19. Break-Glass-Zugriffe MÜSSEN den dafür definierten Security- und Audit-Regeln folgen.
20. Quarantine-, Remediation-, Verification- und Restore-Entscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- kompromittierte Komponenten werden nicht blind erneut gestartet,
- Sicherheitsvorfälle können räumlich begrenzt werden,
- relevante Evidence bleibt für Analyse und Trust-Entscheidungen erhalten,
- Capabilities und Secrets können unmittelbar geschützt werden,
- Trust wird vor Wiederaufnahme des Betriebs neu hergestellt,
- Self-Healing verschleiert keine Security Incidents.

### Negative Konsequenzen

- Security Failures können zu längerer Nichtverfügbarkeit führen,
- Quarantine benötigt Integration mit Capability-, Trust- und Secret-Systemen,
- Security Recovery ist aufwendiger als ein einfacher Restart.

## Verworfene Alternativen

### Security Failure wie normalen Crash behandeln

Verworfen, da dadurch kompromittierter Zustand erneut aktiviert und Evidence verloren gehen könnte.

### Sofortiger Restart nach Integrity Failure

Verworfen, da ein Restart die Ursache einer Manipulation nicht beseitigt.

### Quarantine ausschließlich bei bestätigter Kompromittierung

Verworfen, da bereits ein ausreichend begründeter Verdacht Containment erforderlich machen kann.

### Verfügbarkeit vor Security priorisieren

Verworfen, da ein verfügbarer, aber kompromittierter Systemzustand keine gültige Recovery darstellt.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-SECURITY-0017_Break_Glass_Access_mit_Audit`
- `ADR-SECURITY-0018_Fail_Closed_für_Sicherheitsentscheidungen`
- `ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur`
- `ADR-TRUST-0010_Workload_Attestation_vor_Secret_Release`
- `ADR-CAPABILITY-0009_Capability_Revocation`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-SECURITY-FAILURE-0001`
- `NPSPEC-RESILIENCE-QUARANTINE-0001`
- `NPSPEC-RESILIENCE-QUARANTINE-SCOPE-0001`
- `NPSPEC-RESILIENCE-SECURITY-EVIDENCE-PRESERVATION-0001`
- `NPSPEC-RESILIENCE-SECURITY-REMEDIATION-0001`
- `NPSPEC-RESILIENCE-QUARANTINE-RESTORE-0001`
- `NPSPEC-RESILIENCE-SECURITY-RECOVERY-TEST-0001`

## Ergebnis

Nova.Resilience behandelt mögliche Sicherheitskompromittierungen nicht wie gewöhnliche Abstürze:

```text
Security Failure
      ↓
Classify
      ↓
Contain
      ↓
Quarantine
      ↓
Preserve Evidence
      ↓
Remediate
      ↓
Re-establish Trust
      ↓
Verify
   ┌──┴───┐
 Valid   Invalid
   ↓       ↓
Restore  Quarantine / Escalate
```

Damit verhindert NovaOS, dass Self-Healing einen kompromittierten Zustand lediglich neu startet und dadurch scheinbar wieder funktionsfähig macht. Bei Security Failures gilt zuerst **Isolation und Wiederherstellung von Trust**, danach erst die Rückkehr in den normalen Betrieb.