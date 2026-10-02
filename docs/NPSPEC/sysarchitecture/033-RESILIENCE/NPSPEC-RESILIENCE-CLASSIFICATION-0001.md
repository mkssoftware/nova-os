# NPSPEC-RESILIENCE-CLASSIFICATION-0001 – Nova Resilience Failure Classification

## Status

Angenommen

## Kategorie

Resilience / Failure Classification / Reliability

## Zweck

NovaOS definiert ein einheitliches Modell zur Klassifikation erkannter Fehler und degradierter Systemzustände.

```text
Detection
   ↓
Evidence
   ↓
Classification
   ↓
Severity + Scope + Persistence
   ↓
Resilience Policy
```

Die Klassifikation beschreibt Art und Auswirkungen eines Fehlers. Sie bestimmt nicht automatisch dessen Ursache oder Recovery-Methode.

## Grundprinzipien

```text
Detection ≠ Classification
Classification ≠ Diagnosis
Classification ≠ Root Cause
Severity ≠ Priority
Transient ≠ Harmless
Critical ≠ Irrecoverable
Unknown ≠ Healthy
```

## Classification Model

```text
FailureClassification
├── ClassificationID
├── DetectionID
├── FailureClass
├── Severity
├── Scope
├── Persistence
├── Confidence
└── State
```

Optional:

```text
DomainID
TargetID
Impact
Criticality
Recoverability
PropagationRisk
SecurityRelevance
Evidence
ProvenanceID
```

## Failure Classes

NovaOS unterscheidet mindestens:

```text
Transient
Recoverable
Persistent
Degraded
Critical
Inconsistent
Unknown
```

### Transient

Kurzzeitiger Fehler, der ohne dauerhafte Zustandsänderung verschwinden kann.

Beispiele:

```text
Temporary Resource Contention
Short Network Interruption
Temporary Device Busy
```

### Recoverable

Fehler, für den eine bekannte Recovery-Strategie verfügbar ist.

```text
Service Crash
Driver Failure
Lost Connection
Invalid Runtime State
```

### Persistent

Fehler bleibt trotz normaler Wiederholungs- oder Recovery-Versuche bestehen.

```text
Persistent Device Failure
Corrupted Configuration
Unavailable Required Resource
```

### Degraded

Die Komponente funktioniert weiterhin, jedoch nicht mit ihrer vollständigen zugesicherten Fähigkeit.

```text
Reduced Performance
Missing Optional Provider
Reduced Redundancy
Read-only Operation
```

### Critical

Der Fehler gefährdet wesentliche Systemgarantien.

Beispiele:

```text
Kernel Integrity Failure
Loss of Critical Storage
Unrecoverable Realtime Violation
Critical Hardware Failure
```

### Inconsistent

Beobachtete Zustände widersprechen einander oder verletzen definierte Invarianten.

```text
Metadata ≠ Payload State
Replica States Conflict
Transaction State Invalid
```

### Unknown

Die vorhandene Evidenz erlaubt keine belastbare Klassifikation.

```text
Unknown ≠ Healthy
Unknown ≠ Failed
```

## Severity

Unabhängig von der Failure Class wird die Auswirkung bewertet:

```text
Informational
Minor
Moderate
Major
Critical
```

Severity beschreibt die Auswirkung, nicht die technische Ursache.

## Scope

Ein Fehler kann unterschiedliche Reichweite besitzen:

```text
Operation
Task
Process
Service
Driver
Device
Subsystem
Node
Distributed Service
System
```

Recovery soll möglichst auf die kleinste betroffene Domain begrenzt werden.

## Persistence

NovaOS unterscheidet:

```text
Transient
Intermittent
Persistent
Unknown
```

Wiederholt auftretende transiente Fehler können zu einer höheren Klassifikation führen.

## Recoverability

Zusätzlich kann bewertet werden:

```text
AutomaticallyRecoverable
RecoverableWithDegradation
RequiresUserAction
RequiresRecoveryEnvironment
Unrecoverable
Unknown
```

```text
Recoverability ≠ Permission to Recover
```

Autonomy Policy und Security-Regeln gelten weiterhin.

## Impact

Eine Klassifikation kann Auswirkungen auf Systemgarantien beschreiben:

```text
Availability
Integrity
Confidentiality
Performance
Latency
Deadline
Durability
Consistency
Safety
```

Mehrere Auswirkungen können gleichzeitig gelten.

## Propagation Risk

NovaOS soll bewerten können, ob sich ein Fehler über Abhängigkeiten ausbreiten kann.

```text
Local
Contained
Propagating
Systemic
Unknown
```

Hohe Propagation Risk kann frühzeitiges Containment rechtfertigen.

## Security Relevance

Fehler mit möglichem Security-Bezug müssen markierbar sein.

```text
Normal Failure
Potential Security Event
Confirmed Security Event
Unknown
```

Eine Security-relevante Klassifikation kann `Nova Autonomous Self-Protection` aktivieren.

```text
Technical Failure ≠ Security Compromise
```

## Classification Confidence

Klassifikationen können unterschiedliche Sicherheit besitzen:

```text
Low
Medium
High
Confirmed
```

Neue Evidenz darf eine bestehende Klassifikation verändern.

```text
Initial Classification
        ↓
New Evidence
        ↓
Reclassification
```

## Reclassification

Failure Classification ist nicht zwingend statisch.

Beispiel:

```text
Transient
   ↓ repeated
Intermittent
   ↓ recovery fails
Persistent
   ↓ integrity affected
Critical
```

Jede relevante Änderung soll nachvollziehbar bleiben.

## Dependency Context

Die gleiche technische Störung kann abhängig vom Kontext unterschiedlich bewertet werden.

```text
Optional Provider Failure
→ Minor

Only Critical Provider Failure
→ Major / Critical
```

Klassifikation muss daher Abhängigkeiten und Criticality berücksichtigen können.

## Policy Integration

Die Klassifikation liefert Eingaben für:

```text
Containment
Diagnosis
Recovery
Failover
Degradation
Self-Healing
Self-Protection
Escalation
```

Sie schreibt jedoch nicht automatisch eine konkrete Aktion vor.

## Distributed Systems

Bei verteilten Systemen müssen unsichere Zustände erhalten bleiben.

```text
No Response
   ↓
Network Partition?
Node Failure?
Overload?
```

Ohne ausreichende Evidenz muss die Klassifikation `Unknown` oder entsprechend unsicher bleiben.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
ClassificationID
DetectionID
Failure Class
Severity
Scope
Persistence
Recoverability
Impact
Propagation Risk
Security Relevance
Confidence
Evidence
Classification History
```

## Normative Anforderungen

1. NovaOS MUSS erkannte Fehler strukturiert klassifizieren können.
2. Classification MUSS von Detection und Diagnosis getrennt bleiben.
3. `Unknown` DARF NICHT als `Healthy` interpretiert werden.
4. Failure Class und Severity MÜSSEN getrennt modelliert werden.
5. Transient, Recoverable, Persistent, Degraded, Critical, Inconsistent und Unknown MÜSSEN darstellbar sein.
6. Der Scope eines Fehlers MUSS bestimmbar sein.
7. Persistence MUSS unabhängig von Severity beschreibbar sein.
8. Recoverability MUSS explizit modellierbar sein.
9. Recoverability DARF NICHT automatisch Recovery autorisieren.
10. Auswirkungen auf Systemgarantien SOLLEN klassifizierbar sein.
11. Propagation Risk SOLL berücksichtigt werden.
12. Security-relevante Fehler MÜSSEN kennzeichnungsfähig sein.
13. Technische Fehler DÜRFEN NICHT automatisch als Security Compromise gelten.
14. Unsichere Klassifikationen MÜSSEN Confidence ausdrücken können.
15. Neue Evidenz MUSS Reclassification ermöglichen.
16. Reclassification MUSS nachvollziehbar bleiben.
17. Abhängigkeiten und Criticality SOLLEN bei der Klassifikation berücksichtigt werden.
18. Distributed Failure Classification MUSS unbekannte Zustände erhalten können.
19. Klassifikation DARF NICHT automatisch als Root-Cause-Diagnose interpretiert werden.
20. Klassifikationen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-AUTONOMY-SELFPROTECTION-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `ADR-ARCH-0111`

## Ergebnis

```text
Failure Detection
       ↓
Evidence
       ↓
Classification
       ↓
Class + Severity + Scope
       ↓
Persistence + Recoverability
       ↓
Impact + Propagation Risk
       ↓
Resilience Policy
       ↓
Contain / Diagnose / Recover / Degrade / Escalate
```

NovaOS erhält damit ein einheitliches Failure-Classification-Modell, das erkannte Störungen nach Art, Schwere, Reichweite, Dauer, Wiederherstellbarkeit und Auswirkungen einordnet und damit eine belastbare Grundlage für Containment, Diagnosis, Recovery und Self-Healing bildet.