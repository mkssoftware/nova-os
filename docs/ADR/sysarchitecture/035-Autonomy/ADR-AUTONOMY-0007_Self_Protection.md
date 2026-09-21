# ADR-AUTONOMY-0007 – Self-Protection

## Status

Angenommen

## Kategorie

Autonomy / Security / Resilience

## Kontext

NovaOS soll Bedrohungen und gefährliche Systemzustände erkennen und innerhalb klar definierter Grenzen selbstständig Schutzmaßnahmen einleiten können.

Mögliche Situationen sind:

```text
Integrity Violation
Unauthorized Access
Compromised Component
Suspicious Execution
Trust Loss
Malicious Input
Resource Abuse
Security Policy Violation
Unexpected Data Flow
Attack on System Services
```

Self-Protection ergänzt klassische Security-Mechanismen um autonome Reaktionen, darf dabei jedoch keine eigene Sicherheitsordnung erzeugen.

## Entscheidung

NovaOS unterstützt **Self-Protection** als policy-, evidence- und constraint-gesteuerte autonome Systemfunktion.

```text
Security Signals
      ↓
Detection
      ↓
Diagnosis + Evidence
      ↓
Risk Evaluation
      ↓
Policies + Constraints
      ↓
Protection Action
      ↓
Verification
```

Schutzmaßnahmen erfolgen nach dem Fail-Safe-Prinzip und möglichst im kleinsten sicheren Scope.

## Protection Scope

NovaOS soll Bedrohungen möglichst gezielt isolieren.

Beispiele:

```text
Object
Execution
Process
Capability
Provider
Device
Service
Network Connection
Node
Subsystem
```

Eine systemweite Einschränkung soll nur erfolgen, wenn kleinere Schutzmaßnahmen nicht ausreichend sind.

## Evidence

Self-Protection basiert auf überprüfbaren Security-Signalen.

Mögliche Quellen sind:

```text
Integrity Checks
Trust Evidence
Authorization Failures
Measured Boot
Code Signatures
Provenance
Behavioral Signals
Network Events
Resource Tracing
Audit Events
Self-Diagnosis
```

Adaptive Predictions dürfen Evidence ergänzen, aber nicht ersetzen.

## Protection Actions

Zulässige Schutzmaßnahmen können sein:

```text
Deny
Restrict
Isolate
Quarantine
Revoke
Terminate
Suspend
Disconnect
Block Data Flow
Disable Capability
Switch Provider
Restore Trusted State
Enter Safe State
```

Welche Aktion zulässig ist, wird durch Security Policy und Autonomy Boundary bestimmt.

## Containment

Bei möglicher Kompromittierung besitzt Eindämmung Vorrang vor gewöhnlicher Verfügbarkeitsoptimierung.

```text
Possible Compromise
       ↓
Contain
       ↓
Preserve Evidence
       ↓
Diagnose
       ↓
Recovery
```

Ein kompromittierter Zustand darf nicht durch einfachen Neustart automatisch wieder als vertrauenswürdig gelten.

## Trust Loss

Verliert eine Komponente erforderliches Trust, muss NovaOS reagieren können.

```text
Trusted
   ↓
Trust Evidence Lost
   ↓
Restrict / Isolate / Stop
   ↓
Reevaluate
```

Dabei gilt:

```text
Unknown Trust ≠ Trusted
```

## Capability Revocation

Self-Protection darf bestehende Capabilities einschränken oder widerrufen, wenn Security Policies dies verlangen.

```text
Security Event
      ↓
Affected Authority
      ↓
Capability Revocation
      ↓
Containment
```

Bereits delegierte Rechte müssen entsprechend der bestehenden Revocation-Mechanismen berücksichtigt werden.

## Data Protection

Self-Protection muss unerlaubte Datenbewegungen verhindern können.

```text
Data Flow
   ↓
Security / Privacy / Sovereignty Check
   ↓
Allowed / Denied
```

Bei erkannter Verletzung können Transfers blockiert, Komponenten isoliert oder Berechtigungen entzogen werden.

## Resource Abuse

Ungewöhnlicher Ressourcenverbrauch kann ein Schutzsignal darstellen.

Beispiele:

```text
Memory Exhaustion
CPU Abuse
Storage Flooding
Network Flooding
Excessive Process Creation
Repeated Failed Operations
```

Resource Exhaustion allein beweist jedoch keinen Angriff.

Self-Diagnosis muss technische Fehler und mögliche Security-Ereignisse unterscheidbar halten.

## Adaptive Detection

Adaptive Modelle dürfen bei der Erkennung ungewöhnlichen Verhaltens unterstützen.

```text
Observed Behavior
      ↓
Expected Behavior
      ↓
Deviation
      ↓
Security Signal
```

Eine Abweichung ist zunächst ein Signal und nicht automatisch ein bestätigter Angriff.

## False Positives

Self-Protection muss mit Fehlalarmen umgehen können.

```text
Suspicious
   ≠
Confirmed Compromise
```

Schutzmaßnahmen sollen hinsichtlich Confidence, möglicher Auswirkungen und Reversibilität abgestuft werden.

## Fail-Safe Verhalten

Kann eine sicherheitskritische Entscheidung nicht zuverlässig getroffen werden, gilt:

```text
Unknown
   ↓
Fail-Safe Policy
```

Für sicherheitskritische Authorization Decisions gilt weiterhin:

```text
Allow only if verified

Deny / Unknown / Error
        ↓
Deny
```

## Recovery

Self-Protection und Self-Healing werden koordiniert, bleiben jedoch logisch getrennt.

```text
Protect
   ↓
Contain
   ↓
Diagnose
   ↓
Recover
   ↓
Verify Trust
```

Recovery darf eine isolierte Komponente erst wieder freigeben, wenn die erforderlichen Trust- und Security-Bedingungen erneut erfüllt sind.

## Evidence Preservation

Schutzmaßnahmen sollen relevante Diagnose- und Audit-Evidence nicht unnötig zerstören.

Beispiele:

```text
Security Events
Relevant Logs
Integrity State
Provenance
Execution Context
Decision Trace
```

Secrets und unnötige personenbezogene Daten dürfen dabei nicht unkontrolliert gespeichert werden.

## Nutzerautorität

Explizite Nutzerentscheidungen dürfen Security Hard Constraints nicht abschwächen.

```text
User Preference
      <
Security Hard Constraint
```

Für ausdrücklich vorgesehene administrative oder Break-Glass-Verfahren gelten separate autorisierte Policies.

## Distributed Protection

In verteilten Systemen darf Self-Protection kompromittierte oder nicht mehr vertrauenswürdige Nodes isolieren.

```text
Node Trust Loss
      ↓
Stop New Placement
      ↓
Restrict Communication
      ↓
Revoke Authority
      ↓
Quarantine
```

Andere Nodes dürfen einen isolierten Node nicht automatisch wieder als vertrauenswürdig behandeln.

## Explainability

NovaOS soll relevante Schutzentscheidungen erklären können.

Beispiele:

```text
Was wurde erkannt?

Welche Evidence lag vor?

Welche Komponente wurde eingeschränkt?

Warum wurde diese Maßnahme gewählt?

Welche Rechte wurden entzogen?

Was ist für eine Freigabe erforderlich?
```

Sensitive Security Details dürfen dabei nur autorisierten Empfängern angezeigt werden.

## Deterministic Mode

Security Policies und Hard Constraints bleiben auch im Deterministic Mode vollständig wirksam.

Relevante:

```text
Security Policy
Trust State
Evidence
Decision Rules
Protection State
```

müssen für reproduzierbare Entscheidungen versionierbar oder erfassbar sein.

## Observability

Self-Protection muss mit Security-, Decision- und System-Observability korrelierbar sein.

Relevante Informationen sind:

```text
Security Event ID
Decision ID
Affected Component
Evidence
Protection Action
Protection Scope
Trust State
Verification Result
Recovery State
```

## Normative Anforderungen

1. NovaOS SOLL Self-Protection als autonome Sicherheitsfunktion unterstützen.
2. Self-Protection MUSS durch Security Policies, Constraints und Autonomy Boundaries begrenzt sein.
3. Schutzentscheidungen MÜSSEN auf geeigneter Evidence basieren.
4. Adaptive Predictions DÜRFEN Evidence unterstützen, aber nicht ersetzen.
5. Schutzmaßnahmen SOLLEN den kleinsten sicheren Scope verwenden.
6. Bei möglicher Kompromittierung MUSS Containment gegenüber gewöhnlicher Verfügbarkeitsoptimierung priorisierbar sein.
7. `Unknown Trust` DARF nicht als `Trusted` behandelt werden.
8. Self-Protection MUSS Capability Revocation und Isolation auslösen können.
9. Unerlaubte Datenbewegungen MÜSSEN blockierbar sein.
10. Resource Abuse DARF als Security Signal verwendet werden, aber nicht automatisch als Angriff gelten.
11. Adaptive Anomalieerkennung DARF eine Abweichung nicht automatisch als bestätigte Kompromittierung behandeln.
12. Schutzmaßnahmen SOLLEN abhängig von Evidence, Confidence und Reversibilität abgestuft werden.
13. Sicherheitskritische Authorization Decisions MÜSSEN fail-safe behandelt werden.
14. Ein Neustart DARF kompromittiertes Trust nicht automatisch wiederherstellen.
15. Recovery MUSS erforderliches Trust vor einer Wiederfreigabe erneut verifizieren.
16. Relevante Security Evidence SOLL bei Schutzmaßnahmen erhalten bleiben.
17. Explizite Nutzerentscheidungen DÜRFEN Security Hard Constraints nicht abschwächen.
18. Distributed Self-Protection MUSS Nodes und Trust Domains isolieren können.
19. Relevante Schutzentscheidungen SOLLEN erklärbar sein.
20. Self-Protection MUSS mit Observability und Decision Tracing korrelierbar sein.
21. NovaOS MUSS auch ohne adaptive Modelle grundlegende Self-Protection bereitstellen können.

## Konsequenzen

### Positive Konsequenzen

- NovaOS kann auf Sicherheitsprobleme unmittelbar reagieren,
- kompromittierte Komponenten können gezielt isoliert werden,
- Security und Self-Healing werden kontrolliert miteinander verbunden,
- Trust-Verlust kann systemweit berücksichtigt werden,
- adaptive Erkennung bleibt von verbindlichen Security Decisions getrennt.

### Negative Konsequenzen

- Fehlalarme können temporäre Einschränkungen verursachen,
- Security Evidence und Trust State erhöhen den verwalteten Systemzustand,
- verteiltes Containment und Revocation erhöhen die Komplexität.

## Abhängigkeiten

- `ADR-AUTONOMY-0001_Policy_Driven_Autonomy`
- `ADR-AUTONOMY-0002_Constraint_Driven_Autonomy`
- `ADR-AUTONOMY-0004_Self_Diagnosis`
- `ADR-AUTONOMY-0005_Self_Healing`
- `ADR-ADAPTIVE-0012_Policy_Learning_mit_harten_Grenzen`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-CAPABILITY-0008_Capability_Attenuation`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0008_Provenance_Inspection`
- `ADR-RESILIENCE-0020_Explainable_Self_Healing`

## Zugehörige NPSPECs

- `NPSPEC-AUTONOMY-SELF-PROTECTION-0001`
- `NPSPEC-AUTONOMY-SECURITY-CONTAINMENT-0001`
- `NPSPEC-AUTONOMY-TRUST-LOSS-0001`
- `NPSPEC-AUTONOMY-SECURITY-RESPONSE-0001`
- `NPSPEC-AUTONOMY-SECURITY-VERIFICATION-0001`

## Ergebnis

NovaOS behandelt Self-Protection als kontrollierten autonomen Sicherheitszyklus:

```text
Detect
   ↓
Evaluate Evidence
   ↓
Contain
   ↓
Protect
   ↓
Diagnose
   ↓
Recover
   ↓
Verify Trust
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll sich selbst schützen können,
ohne Sicherheit mit bloßer
Anomalieerkennung zu verwechseln.

Bei Gefahr wird zuerst begrenzt,
isoliert und Evidence erhalten.

Erst wenn Sicherheit und Trust
wieder nachgewiesen sind,
wird der normale Betrieb
vollständig freigegeben.
```