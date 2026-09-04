# ADR-RESILIENCE-0011 – Progress-Based Watchdogs

## Status

Angenommen

## Kategorie

Resilience Architecture / Watchdogs / Failure Detection

## Kontext

Klassische Watchdogs prüfen häufig lediglich, ob eine Komponente regelmäßig ein Lebenszeichen sendet.

```text
Component
   ↓
Heartbeat
   ↓
Watchdog
```

Ein Heartbeat beweist jedoch nur, dass eine Komponente noch ausgeführt wird. Eine Workload kann weiterhin Heartbeats senden und trotzdem keine sinnvolle Arbeit mehr erledigen.

Beispiele sind:

- Deadlocks,
- Livelocks,
- endlose Retry-Schleifen,
- festhängende State Machines,
- blockierte Pipelines,
- Scheduler Starvation,
- Operationen ohne messbaren Fortschritt.

Nova.Resilience benötigt deshalb Watchdogs, die nicht ausschließlich **Lebendigkeit**, sondern den tatsächlichen **semantischen Fortschritt** einer Komponente überwachen können.

## Entscheidung

NovaOS verwendet **Progress-Based Watchdogs** als bevorzugtes Watchdog-Modell.

```text
Component
    ↓
Progress Evidence
    ↓
Watchdog
    ↓
Expected Progress?
 ├── Yes → Continue
 └── No  → Failure Detection
```

Heartbeats dürfen weiterhin als ergänzende Evidence verwendet werden, reichen für progress-kritische Komponenten jedoch nicht allein aus.

## Grundprinzip

```text
Alive
≠
Making Progress
```

Ein Watchdog soll deshalb möglichst prüfen:

```text
Is the component advancing
toward its expected state?
```

statt lediglich:

```text
Is the component still running?
```

## Progress Evidence

Komponenten können explizite Progress Evidence veröffentlichen.

Beispiele:

```text
Completed Work Units
Transaction Advancement
Queue Advancement
Pipeline Stage Completion
State Transition
Processed Sequence Number
Checkpoint Advancement
Committed Operation
```

Die konkrete Progress-Semantik hängt von der jeweiligen Komponente ab.

## Progress Marker

Komponenten können monotone oder anderweitig eindeutig interpretierbare Progress Marker bereitstellen.

Beispiel:

```text
Progress = 1042
Progress = 1043
Progress = 1044
```

Ein unveränderter Marker kann auf fehlenden Fortschritt hinweisen.

Der Marker selbst muss jedoch semantisch definiert sein.

Eine bloße Erhöhung eines Zählers darf nicht verwendet werden, um künstlich Fortschritt vorzutäuschen.

## Expected Progress

Nicht jede Komponente muss ständig Fortschritt erzeugen.

Ein wartender Dienst kann vollständig gesund sein.

Deshalb muss der Watchdog berücksichtigen, ob aktuell überhaupt Fortschritt erwartet wird.

```text
Work Pending?
   ├── No  → No progress required
   └── Yes
         ↓
   Progress expected
```

Damit werden Idle-Zustände nicht fälschlich als Fehler erkannt.

## Deadlines

Operationen mit bekannten Deadlines können direkt in die Progress-Bewertung einfließen.

```text
Operation
   ↓
Deadline
   +
Observed Progress
   ↓
Watchdog Evaluation
```

Ein Watchdog darf jedoch nicht ausschließlich auf einer globalen festen Timeout-Dauer basieren.

## Liveness und Progress

NovaOS unterscheidet mindestens:

```text
Liveness
Progress
Health
```

Eine Komponente kann beispielsweise:

```text
Liveness = Alive
Progress = Stalled
Health   = Unhealthy
```

sein.

Diese Zustände dürfen nicht miteinander gleichgesetzt werden.

## Deadlock Detection

Fehlender Fortschritt kann auf einen Deadlock hinweisen.

```text
Task A waits for B
Task B waits for A
        ↓
No Progress
```

Progress-Based Watchdogs können entsprechende Detection-Mechanismen auslösen.

Die eigentliche Deadlock-Diagnose bleibt eine separate Funktion von Nova.Resilience.

## Livelock Detection

Auch Livelocks müssen erkannt werden können.

```text
Component active
      ↓
State changes
      ↓
No semantic advancement
```

Hohe Aktivität oder CPU-Auslastung darf deshalb nicht automatisch als Fortschritt interpretiert werden.

## Retry Loops

Progress-Based Watchdogs integrieren sich mit Retry Budgets.

```text
Operation
   ↓
Retry
   ↓
Retry
   ↓
Retry
   ↓
No semantic progress
```

Eine aktive Retry-Schleife darf nicht allein aufgrund ihrer Aktivität als gesunder Fortschritt gelten.

## Structured Concurrency

Structured-Concurrency-Hierarchien können Progress-Informationen aggregieren.

```text
Task Group
   ├── Task A → Progress
   ├── Task B → Waiting
   └── Task C → Progress
```

Ein blockierter Child Task muss nicht automatisch die gesamte Parent Domain als festgefahren klassifizieren.

Die Abhängigkeits- und Completion-Semantik muss berücksichtigt werden.

## Hierarchische Watchdogs

Watchdogs können entlang der Failure-Domain-Hierarchie organisiert werden.

```text
System
  ↓
Subsystem
  ↓
Service
  ↓
Workload
  ↓
Task
```

Dadurch können lokale Stalls zunächst innerhalb des kleinsten betroffenen Recovery Scope behandelt werden.

## Adaptive Schwellenwerte

Progress-Erwartungen dürfen dynamisch an bekannte Betriebsbedingungen angepasst werden.

Beispielsweise können berücksichtigt werden:

```text
Current Load
Resource Budget
Historical Runtime
Hardware Performance
Provider Latency
Operation Type
```

Adaptive Schwellenwerte sind Optimierungen.

Sie dürfen Hard Deadlines oder explizite Contracts nicht verändern.

## Recovery

Erkennt ein Watchdog fehlenden Fortschritt, führt dies nicht automatisch zu einem Neustart.

```text
Progress Failure
      ↓
Detect
      ↓
Classify
      ↓
Contain
      ↓
Plan
      ↓
Recover
```

Damit bleibt der Watchdog Teil der Detection-Phase von Nova.Resilience.

Die konkrete Recovery wird anhand des Fehlerzustands und Recovery Scope bestimmt.

## Watchdog Failure

Ein Watchdog selbst darf nicht zum Single Point of Failure werden.

Kritische Watchdog-Funktionen müssen entsprechend ihrer Bedeutung isolierbar, überprüfbar oder hierarchisch absicherbar sein.

Eine überwachte Komponente darf ihren eigenen Watchdog nicht beliebig deaktivieren oder dessen Evidence manipulieren können.

## Deterministic Mode

Im Deterministic Mode müssen Progress-Erwartungen und Watchdog-Entscheidungen reproduzierbar beziehungsweise nachvollziehbar bleiben.

Adaptive oder zeitabhängige Entscheidungen dürfen die geforderten Determinismusgarantien nicht verletzen.

## Normative Anforderungen

1. NovaOS MUSS Progress-Based Watchdogs unterstützen.
2. Progress-kritische Komponenten SOLLEN nicht ausschließlich über Heartbeats überwacht werden.
3. Liveness und Progress MÜSSEN getrennte Zustände bleiben.
4. Health State und Progress State MÜSSEN getrennt modellierbar sein.
5. Komponenten MÜSSEN semantische Progress Evidence bereitstellen können.
6. Progress Marker MÜSSEN eine definierte Bedeutung besitzen.
7. Künstliche Aktivität DARF nicht automatisch als semantischer Fortschritt gelten.
8. Watchdogs MÜSSEN berücksichtigen können, ob aktuell Fortschritt erwartet wird.
9. Idle-Zustände DÜRFEN nicht allein wegen fehlender Progress Events als Fehler klassifiziert werden.
10. Deadlines MÜSSEN bei der Progress-Bewertung berücksichtigt werden können.
11. Fehlender Fortschritt MUSS Deadlock-, Livelock- oder weitere Diagnoseverfahren auslösen können.
12. Retry-Aktivität DARF nicht automatisch als Fortschritt gelten.
13. Progress Monitoring MUSS mit Structured Concurrency integrierbar sein.
14. Watchdogs MÜSSEN hierarchischen Failure Domains zugeordnet werden können.
15. Ein Watchdog-Fehler DARF nicht automatisch einen unkontrollierten systemweiten Recovery auslösen.
16. Überwachte Komponenten DÜRFEN kritische Watchdog-Policies nicht eigenmächtig umgehen.
17. Adaptive Progress-Schwellenwerte DÜRFEN Hard Deadlines und Contracts nicht verändern.
18. Watchdog-Ergebnisse MÜSSEN in die normale Nova.Resilience-Pipeline eingehen.
19. Progress- und Watchdog-Zustände MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Deadlocks und Livelocks können besser erkannt werden,
- scheinbar lebende, aber festgefahrene Komponenten werden sichtbar,
- Retry-Schleifen können als fehlender Fortschritt erkannt werden,
- Idle-Komponenten werden nicht unnötig als fehlerhaft klassifiziert,
- Watchdogs integrieren sich direkt in Failure Domains und Recovery,
- Recovery basiert auf semantischer Evidence statt ausschließlich auf Zeitouts.

### Negative Konsequenzen

- Komponenten müssen ihre Progress-Semantik definieren,
- Progress Detection ist komplexer als einfache Heartbeats,
- falsche Progress-Erwartungen können False Positives erzeugen.

## Verworfene Alternativen

### Ausschließlich Heartbeat-basierte Watchdogs

Verworfen, da eine festgefahrene Komponente weiterhin Heartbeats erzeugen kann.

### Ausschließlich Timeout-basierte Watchdogs

Verworfen, da unterschiedliche Operationen und Lastzustände unterschiedliche Fortschrittscharakteristiken besitzen.

### CPU-Aktivität als Fortschrittsindikator

Verworfen, da Livelocks und Retry-Schleifen hohe Aktivität ohne semantischen Fortschritt erzeugen können.

### Watchdog löst unmittelbar Neustart aus

Verworfen, da fehlender Fortschritt zunächst klassifiziert und der kleinste sichere Recovery Scope bestimmt werden soll.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0006_Retry_Budgets_Backoff_und_Jitter`
- `ADR-RESILIENCE-0008_Health_State_Model`
- `ADR-ARCH-0009_Structured_Concurrency_systemweit`
- `ADR-ARCH-0010_Deterministic_Mode`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-WATCHDOG-0001`
- `NPSPEC-RESILIENCE-PROGRESS-EVIDENCE-0001`
- `NPSPEC-RESILIENCE-PROGRESS-MARKER-0001`
- `NPSPEC-RESILIENCE-PROGRESS-EXPECTATION-0001`
- `NPSPEC-RESILIENCE-DEADLOCK-DETECTION-0001`
- `NPSPEC-RESILIENCE-LIVELOCK-DETECTION-0001`
- `NPSPEC-RESILIENCE-WATCHDOG-TEST-0001`

## Ergebnis

Nova.Resilience überwacht nicht nur, ob Komponenten noch ausgeführt werden, sondern ob sie den erwarteten semantischen Fortschritt erzielen:

```text
Work Pending
     ↓
Expected Progress
     ↓
Observed Progress Evidence
     ↓
Progress?
 ├── Yes → Continue
 └── No
      ↓
   Detect
      ↓
   Classify
      ↓
   Recovery Pipeline
```

Damit kann NovaOS zwischen **lebendig**, **gesund** und **tatsächlich fortschreitend** unterscheiden und festgefahrene Komponenten erkennen, bevor sie größere Failure Domains beeinträchtigen.