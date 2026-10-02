# NPSPEC-RESILIENCE-DETECTION-0001 – Nova Resilience Failure Detection

## Status

Angenommen

## Kategorie

Resilience / Detection / Health / Failure Management

## Zweck

NovaOS definiert ein systemweites Modell zur Erkennung von Fehlern, Ausfällen und degradierten Zuständen.

```text
System State
     ↓
Observation
     ↓
Detection
     ↓
Evidence
     ↓
Classification
     ↓
Resilience Response
```

Failure Detection liefert Hinweise auf einen möglichen Fehler. Sie führt selbst noch keine Reparatur durch.

## Grundprinzipien

```text
Anomaly ≠ Failure
Failure Detection ≠ Diagnosis
Detection ≠ Root Cause
Timeout ≠ Failure
Unavailable ≠ Failed
Unknown ≠ Healthy
Suspicion ≠ Proof
```

Unsicherheit muss explizit dargestellt werden.

## Detection Model

```text
FailureDetection
├── DetectionID
├── TargetID
├── DetectionType
├── Evidence
├── Confidence
├── Timestamp
└── State
```

Optional:

```text
DomainID
HealthState
FailureClass
ExecutionID
ProviderID
NodeID
TraceID
IntegrityState
ObservationQuality
ProvenanceID
```

## Detection Sources

NovaOS kann Fehler aus unterschiedlichen Quellen erkennen:

```text
Health Checks
Watchdogs
Timeouts
Heartbeats
Assertions
Runtime Contracts
Integrity Checks
Crash Reports
Resource Monitoring
Driver Status
Device Status
Storage Errors
Network State
Execution Failures
Boot Health
```

Mehrere Quellen können gemeinsam eine Detection unterstützen.

## Detection Types

Mindestens folgende Klassen sollen unterscheidbar sein:

```text
Crash
Hang
Timeout
Integrity Failure
Resource Exhaustion
Unavailable Dependency
Device Failure
Communication Failure
Contract Violation
State Inconsistency
Performance Degradation
Unknown Failure
```

## Health Monitoring

Komponenten können ihren Zustand explizit melden:

```text
Healthy
Degraded
SuspectedFailure
Failed
Unavailable
Unknown
```

Selbst gemeldeter Zustand darf mit externen Beobachtungen verglichen werden.

## Heartbeats

Für überwachte Komponenten können Heartbeats verwendet werden.

```text
Component
   ↓
Heartbeat
   ↓
Monitor
```

Fehlt ein erwarteter Heartbeat:

```text
Missing Heartbeat
      ↓
Suspected Failure
```

Ein fehlender Heartbeat allein beweist keinen Ausfall.

## Watchdogs

Watchdogs können blockierte oder nicht reagierende Komponenten erkennen.

```text
Execution
    ↓
Progress Signal
    ↓
Watchdog
```

Bleibt erwarteter Fortschritt aus, kann eine Failure Detection erzeugt werden.

Watchdogs müssen definierte Zeitgrenzen besitzen.

## Timeout Detection

```text
Request
   ↓
Expected Response
   ↓
Timeout
```

Ein Timeout bedeutet zunächst:

```text
Result Unknown
```

und nicht automatisch:

```text
Remote Component Failed
```

Dies ist insbesondere für Netzwerk- und Distributed-Systeme wichtig.

## Integrity Detection

Integritätsprüfungen können erkennen:

```text
Corrupted Memory
Modified Code
Invalid Storage Data
Checksum Failure
Signature Failure
Invalid System State
```

Security-relevante Integritätsverletzungen müssen an Self-Protection weitergegeben werden können.

## Resource Failure Detection

NovaOS kann kritische Ressourcenzustände erkennen:

```text
Memory Exhaustion
CPU Starvation
IO Saturation
Storage Exhaustion
Network Congestion
Energy Critical
Thermal Critical
```

Ressourcendruck ist nicht automatisch ein Komponentenfehler.

## Dependency Detection

Fehler einer Abhängigkeit müssen von Fehlern der konsumierenden Komponente unterscheidbar bleiben.

```text
Service A
   ↓
Service B Failed
```

Service A kann dadurch:

```text
Degraded
Unavailable
Blocked
```

sein, ohne selbst defekt zu sein.

## Failure Evidence

Eine Detection soll nachvollziehbare Evidenz enthalten.

Beispiele:

```text
Error Code
Health State
Failed Contract
Missing Heartbeat
Crash Dump
Integrity Result
Resource State
Device Status
Trace Event
```

```text
Detection
   ↓
Evidence
   ↓
Diagnosis
```

## Confidence

Nicht eindeutige Erkennungen können Confidence besitzen.

```text
Low
Medium
High
Confirmed
```

Confidence darf keine objektive Gewissheit vortäuschen.

## Detection Correlation

Mehrere Beobachtungen können korreliert werden.

```text
Timeout
   +
Missing Heartbeat
   +
Device Error
      ↓
Correlated Detection
```

```text
Correlation ≠ Root Cause
```

Die eigentliche Ursachenanalyse gehört zur Self-Diagnosis.

## Failure Propagation

Erkannte Fehler müssen über Abhängigkeiten nachvollziehbar sein.

```text
Device Failure
      ↓
Driver Degraded
      ↓
Provider Unavailable
      ↓
Service Degraded
```

Der ursprüngliche Fehler und seine Auswirkungen müssen getrennt bleiben.

## Distributed Detection

In verteilten Systemen kann NovaOS häufig nicht sicher zwischen folgenden Zuständen unterscheiden:

```text
Remote Failure
Network Partition
High Latency
Local Network Failure
Remote Overload
```

Daher gilt:

```text
No Response ≠ Node Failed
```

Unsichere Zustände müssen als `Unknown` oder `SuspectedFailure` darstellbar sein.

## Detection Latency

Zeitkritische Komponenten können Anforderungen an maximale Detection-Zeit besitzen.

```text
Failure Occurs
     ↓
Detection Delay
     ↓
Failure Detected
```

Detection Latency kann Bestandteil eines Resilience- oder Realtime-Contracts sein.

## False Positives und False Negatives

Detection-Systeme müssen beide Fehlerarten berücksichtigen:

```text
False Positive
→ Healthy component classified as failed

False Negative
→ Failed component remains undetected
```

Kritische Entscheidungen können zusätzliche Bestätigung verlangen.

## Detection Storms

Ein einzelner Fehler kann viele Folgeereignisse erzeugen.

```text
Root Failure
    ↓
Hundreds of Symptoms
```

NovaOS soll Ereignisse korrelieren und begrenzen können, ohne relevante Evidenz zu verlieren.

## Self-Diagnosis Integration

Failure Detection liefert Eingaben für autonome Diagnose.

```text
Detection
   ↓
Evidence
   ↓
Self-Diagnosis
   ↓
Candidate Causes
```

Detection selbst darf keine unbelegte Root Cause behaupten.

## Self-Healing Integration

Bestätigte oder ausreichend kritische Detection Events können Recovery auslösen.

```text
Detection
   ↓
Policy
   ↓
Containment
   ↓
Diagnosis
   ↓
Recovery
```

Riskante Recovery-Aktionen benötigen weiterhin die vorgesehenen Policy- und Autonomy-Prüfungen.

## Observability

Autorisierte Komponenten sollen beobachten können:

```text
DetectionID
TargetID
Detection Type
Timestamp
Health State
Evidence
Confidence
Observation Quality
Failure Class
Affected Dependencies
Detection Latency
Correlation State
```

## Normative Anforderungen

1. NovaOS MUSS Fehler und degradierte Zustände systemweit erfassen können.
2. Failure Detection MUSS von Diagnosis und Recovery getrennt sein.
3. `Unknown` DARF NICHT als `Healthy` interpretiert werden.
4. Anomalien DÜRFEN NICHT automatisch als bestätigte Fehler gelten.
5. Timeouts DÜRFEN NICHT automatisch als Beweis eines entfernten Ausfalls gelten.
6. Health Checks, Watchdogs und Heartbeats MÜSSEN als Detection Sources unterstützt werden können.
7. Integritätsverletzungen MÜSSEN als eigene Detection-Klasse behandelbar sein.
8. Ressourcendruck MUSS von Komponentenfehlern unterscheidbar bleiben.
9. Dependency Failures MÜSSEN von lokalen Fehlern unterscheidbar sein.
10. Detection Events SOLLEN nachvollziehbare Evidenz enthalten.
11. Unsichere Detection MUSS Confidence oder einen vergleichbaren Unsicherheitszustand darstellen können.
12. Mehrere Detection Events SOLLEN korrelierbar sein.
13. Korrelation DARF NICHT automatisch als Root Cause interpretiert werden.
14. Failure Propagation MUSS nachvollziehbar bleiben.
15. Distributed Detection MUSS Netzwerkpartitionen und unbekannte Zustände berücksichtigen.
16. Detection Latency MUSS für zeitkritische Komponenten messbar sein.
17. False Positives und False Negatives MÜSSEN im Detection-Modell berücksichtigt werden.
18. Detection Storms SOLLEN begrenzt und korreliert werden können.
19. Security-relevante Detection Events MÜSSEN an Self-Protection übergeben werden können.
20. Detection-Zustände und Evidenz MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-AUTONOMY-SELFPROTECTION-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `ADR-ARCH-0110`

## Ergebnis

```text
Observe
   ↓
Detect
   ↓
Collect Evidence
   ↓
Classify
   ↓
Correlate
   ↓
Resilience Decision
   ├── Continue Monitoring
   ├── Diagnose
   ├── Contain
   └── Recover
```

NovaOS erhält damit eine einheitliche Failure-Detection-Schicht, die technische Fehler, degradierte Zustände, Integritätsprobleme und unsichere Ausfälle frühzeitig erkennen kann, ohne Beobachtung, Diagnose und tatsächliche Ursache miteinander gleichzusetzen.