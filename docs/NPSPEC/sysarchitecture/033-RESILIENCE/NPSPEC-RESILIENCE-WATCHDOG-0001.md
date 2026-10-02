# NPSPEC-RESILIENCE-WATCHDOG-0001 – Nova Resilience Watchdog

## Status

Angenommen

## Kategorie

Resilience / Watchdog / Failure Detection / Health Monitoring

## Zweck

NovaOS definiert ein systemweites Watchdog-Modell zur Erkennung von blockierten, nicht reagierenden oder nicht mehr fortschreitenden Komponenten.

```text
Component
    ↓
Progress / Heartbeat
    ↓
Watchdog
    ↓
Timeout / Violation
    ↓
Failure Detection
```

Watchdogs sind Teil der Failure Detection und dürfen nicht selbstständig einen Root Cause annehmen.

## Grundprinzipien

```text
Watchdog Timeout ≠ Proven Failure
No Response ≠ Crash
Heartbeat ≠ Correctness
Alive ≠ Healthy
Restart ≠ Recovery
Watchdog ≠ Diagnosis
Watchdog ≠ Recovery Policy
```

## Watchdog Model

```text
Watchdog
├── WatchdogID
├── TargetID
├── DomainID
├── WatchdogType
├── Interval
├── Deadline
├── State
└── FailureAction
```

Optional:

```text
Criticality
RealtimeProfile
ProgressToken
LastHeartbeat
MissCount
GracePeriod
RecoveryPolicy
ExecutionContractID
ProvenanceID
```

## Watchdog Types

NovaOS soll mindestens unterstützen können:

```text
Heartbeat Watchdog
Progress Watchdog
Execution Watchdog
Service Watchdog
Driver Watchdog
Device Watchdog
System Watchdog
Hardware Watchdog
```

## Heartbeat Watchdog

Eine Komponente meldet regelmäßig ihre Aktivität.

```text
Component
   ↓
Heartbeat
   ↓
Watchdog
```

Fehlt ein Heartbeat:

```text
Expected Heartbeat
        ↓
Missing
        ↓
SuspectedFailure
```

Ein Heartbeat beweist lediglich Aktivität, nicht korrekte Funktion.

## Progress Watchdog

Für kritische Komponenten ist tatsächlicher Fortschritt aussagekräftiger als reine Aktivität.

```text
Progress₀
   ↓
Work
   ↓
Progress₁
```

Bleibt der Progress-Zustand trotz Aktivität unverändert, kann beispielsweise ein Deadlock oder Livelock vermutet werden.

## Execution Watchdog

Executions können maximale Lauf- oder Reaktionszeiten besitzen.

```text
Execution Start
      ↓
Execution
      ↓
Expected Completion
      ↓
Watchdog Deadline
```

Das Überschreiten erzeugt ein Detection Event.

## Hierarchische Watchdogs

Watchdogs sollen hierarchisch organisiert werden können.

```text
Task Watchdog
      ↓
Process Watchdog
      ↓
Service Watchdog
      ↓
Subsystem Watchdog
      ↓
System Watchdog
```

Der Ausfall eines lokalen Watchdogs darf nicht automatisch einen vollständigen System-Reset auslösen.

## Independent Monitoring

Kritische Watchdogs sollen möglichst außerhalb der überwachten Failure Domain liegen.

```text
Failure Domain A
      ↓
Observed by
      ↓
Failure Domain B
```

Ein Watchdog innerhalb derselben vollständig ausgefallenen Domain kann selbst wirkungslos werden.

## Hardware Watchdog

Für kritische Systemzustände kann ein unabhängiger Hardware-Watchdog verwendet werden.

```text
NovaOS
   ↓ periodic signal
Hardware Watchdog
   ↓ timeout
Recovery / Reset
```

Ein Hardware-Reset ist die letzte Eskalationsstufe und kein Ersatz für lokale Recovery.

## Timing

Watchdog-Zeiten müssen zum überwachten Workload passen.

```text
Normal Worst-Case Time
        <
Watchdog Deadline
        <
Unacceptable Stall Time
```

Zu kurze Grenzen erzeugen False Positives.

Zu lange Grenzen erhöhen Detection Latency.

## Realtime Integration

Realtime-Executions können explizite Watchdog-Grenzen besitzen.

Diese müssen berücksichtigen:

```text
Execution Budget
Deadline
Scheduling Latency
Blocking Bound
IO Latency
Jitter
```

Ein gültiger Realtime-Pfad darf nicht allein wegen zulässiger Worst-Case-Latenz als fehlerhaft erkannt werden.

## Watchdog Miss

```text
Expected Signal
      ↓
Deadline Exceeded
      ↓
Watchdog Miss
      ↓
Detection Event
```

Mehrere Misses können vor einer Eskalation erforderlich sein.

## Escalation

Abhängig von Criticality und Policy:

```text
First Miss
   ↓
Observe / Retry
   ↓
Repeated Miss
   ↓
SuspectedFailure
   ↓
Contain
   ↓
Recover
   ↓
Escalate
```

Kritische Systeme können strengere Regeln verwenden.

## Recovery Integration

Watchdogs sollen keine komplexe Recovery-Logik selbst implementieren.

```text
Watchdog
   ↓
Failure Detection
   ↓
Classification
   ↓
Containment
   ↓
Diagnosis
   ↓
Recovery
```

Dadurch bleibt Detection von Recovery Policy getrennt.

## Watchdog Failure

Auch der Watchdog selbst kann ausfallen.

```text
Watchdog
   ↓
Health Monitoring
```

Kritische Watchdogs können gegenseitig oder hierarchisch überwacht werden.

Es darf jedoch keine unbegrenzte Watchdog-Kette entstehen.

## Resource Budget

Watchdogs müssen ressourcenbegrenzt sein.

Zu berücksichtigen sind:

```text
CPU
Memory
Timers
Interrupts
IPC
Logging
Energy
```

Monitoring darf den überwachten Realtime-Workload nicht unkontrolliert beeinträchtigen.

## False Positives

NovaOS muss Fehlalarme berücksichtigen.

Mögliche Ursachen:

```text
Temporary Overload
Scheduling Delay
Network Delay
Device Delay
Power Transition
Legitimate Long Operation
```

Grace Periods und konfigurierbare Miss Counts können Fehlalarme reduzieren.

## Distributed Watchdogs

Bei Remote-Komponenten gilt:

```text
Missing Heartbeat ≠ Remote Failure
```

Mögliche Ursachen sind:

```text
Network Partition
Congestion
Remote Overload
Local Network Failure
Clock Effects
Actual Remote Failure
```

Der Zustand muss bis zur ausreichenden Bestätigung als unsicher behandelbar bleiben.

## Boot und Recovery

Ein System-Watchdog kann Boot Health überwachen.

```text
Boot
 ↓
Expected Healthy State
 ↓
Timeout
 ↓
Recovery Boot / Rollback
```

Dies kann mit A/B Boot und NovaDOS Recovery kombiniert werden.

## Observability

Autorisierte Komponenten sollen beobachten können:

```text
WatchdogID
TargetID
Watchdog Type
Interval
Deadline
Last Signal
Last Progress
Miss Count
Watchdog State
Detection State
Escalation Level
Recovery State
```

## Normative Anforderungen

1. NovaOS MUSS systemweite Watchdogs unterstützen können.
2. Watchdog Detection MUSS von Diagnosis und Recovery getrennt bleiben.
3. Ein Watchdog Timeout DARF NICHT automatisch als bewiesener Root Cause gelten.
4. Heartbeat und tatsächlicher Progress MÜSSEN unterscheidbar sein.
5. Progress Watchdogs SOLLEN für kritische Komponenten unterstützt werden.
6. Watchdogs MÜSSEN definierte Zeitgrenzen besitzen.
7. Kritische Watchdogs SOLLEN außerhalb der überwachten Failure Domain ausführbar sein.
8. Hardware Watchdogs SOLLEN als letzte Eskalationsstufe unterstützt werden können.
9. Watchdog-Grenzen MÜSSEN zulässige Worst-Case-Zeiten berücksichtigen.
10. Realtime Watchdogs MÜSSEN mit Execution Budget und Deadline vereinbar sein.
11. Watchdog Misses MÜSSEN Failure Detection Events erzeugen können.
12. Wiederholte Misses MÜSSEN policy-gesteuert eskalierbar sein.
13. Watchdogs DÜRFEN komplexe Recovery Policy NICHT implizit ersetzen.
14. Watchdog-Ausfälle MÜSSEN selbst erkennbar sein können.
15. Unbegrenzte Watchdog-Abhängigkeitsketten DÜRFEN NICHT entstehen.
16. Watchdog Monitoring MUSS ressourcenbegrenzt sein.
17. False Positives MÜSSEN im Watchdog-Modell berücksichtigt werden.
18. Distributed Watchdogs DÜRFEN Kommunikationsverlust NICHT automatisch als Remote Failure interpretieren.
19. System-Watchdogs SOLLEN mit Boot Health und Recovery integrierbar sein.
20. Watchdog-Zustände und Misses MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-ISOLATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-REALTIME-LATENCY-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-PROCESS-SUPERVISION-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `ADR-ARCH-0115`

## Ergebnis

```text
Component
    ↓
Heartbeat / Progress
    ↓
Watchdog
    ↓
Miss Detected
    ↓
Failure Detection
    ↓
Classification
    ↓
Containment / Diagnosis
    ↓
Recovery
    ↓
Verification
```

NovaOS erhält damit ein hierarchisches und ressourcenbegrenztes Watchdog-System, das blockierte oder nicht mehr fortschreitende Komponenten frühzeitig erkennt, ohne Timeout, Diagnose und Recovery miteinander gleichzusetzen.