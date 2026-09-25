# NPSPEC-SCHEDULER-PREDICTIVE-0001 – Nova Predictive Scheduling

## Status

Angenommen

## Kategorie

Scheduler / Prediction / Adaptive Scheduling / Resource Economy

## Zweck

NovaOS definiert Predictive Scheduling zur vorausschauenden Vorbereitung von Scheduling-Entscheidungen anhand beobachteter System- und Workload-Muster.

```text
Past Behaviour
      ↓
Prediction Model
      ↓
Expected Workload
      ↓
Scheduler Preparation
      ↓
Actual Demand
```

Ziel ist, Ressourcen bereits vor einem erwarteten Bedarf sinnvoll vorzubereiten, ohne vorhergesagte Zustände mit tatsächlichen Anforderungen gleichzusetzen.

## Grundprinzip

Predictive Scheduling ergänzt reaktives Scheduling.

```text
Reactive:
Demand → Decision → Execution

Predictive:
Observation → Prediction → Preparation → Demand → Execution
```

Dabei gilt:

```text
Prediction ≠ Fact
```

Vorhersagen sind Optimierungshinweise und grundsätzlich keine Hard Constraints.

## Vorhersageobjekt

Eine Scheduling-Vorhersage kann enthalten:

```text
PredictionID
Target
ExpectedEvent
Probability
ExpectedTime
ExpectedDuration
ResourceDemand
Confidence
ModelVersion
```

Zusätzlich kann die Herkunft der verwendeten Beobachtungen dokumentiert werden.

## Vorhersagbare Eigenschaften

NovaOS darf unter anderem prognostizieren:

```text
Task Activation
CPU Demand
Interactive Activity
Working Set
Resource Demand
Execution Duration
Preferred CPU Class
Accelerator Demand
I/O Activity
```

Nicht jede Workload muss vorhersagbar sein.

## Scheduling-Nutzung

Vorhersagen dürfen Scheduling-Entscheidungen vorbereiten.

Beispiele:

```text
Expected Task
     ↓
Prepare CPU Capacity

Expected Interactive Load
     ↓
Preserve Low-Latency Capacity

Expected Accelerator Work
     ↓
Prepare Provider

Expected Idle Period
     ↓
Consolidate Background Work
```

Eine Vorhersage allein darf keine unnötig dauerhafte Ressourcenreservierung erzeugen.

## Prediction Confidence

Jede relevante Vorhersage soll eine Vertrauensbewertung besitzen.

```text
High Confidence
Medium Confidence
Low Confidence
```

Je geringer die Confidence, desto geringer soll der Einfluss auf Scheduling-Entscheidungen sein.

## Prediction Error

NovaOS verwendet die Abweichung zwischen Vorhersage und tatsächlichem Verhalten als Lernsignal.

```text
Prediction
    ↓
Actual Behaviour
    ↓
Deviation
    ↓
Prediction Error
    ↓
Model Correction
```

Wiederholte Fehlvorhersagen müssen den zukünftigen Einfluss des entsprechenden Modells reduzieren können.

## Nutzerentscheidung

Explizite Nutzerentscheidungen haben Vorrang vor adaptiven Vorhersagen.

```text
Prediction
    ↓
User Decision
    ↓
Deviation
    ↓
Model Correction
```

Das System darf nicht versuchen, den Nutzer an seine eigene Vorhersage anzupassen.

## Ressourcen

Predictive Scheduling arbeitet mit der Resource Economy zusammen.

Vorbereitend dürfen beispielsweise angepasst werden:

```text
CPU Availability
Performance State
Task Placement
Cache Strategy
Accelerator Readiness
Background Work
```

Spekulative Ressourcennutzung muss begrenzt bleiben.

## Topologie und Lokalität

Vorhersagen dürfen zukünftige Task-Platzierung vorbereiten.

```text
Expected Task
      ↓
Expected Working Set
      ↓
NUMA / Cache Locality
      ↓
Preferred CPU
```

Tatsächliche Laufzeitbedingungen müssen vor der Ausführung erneut geprüft werden.

## Energie und Thermal

Predictive Scheduling darf erwartete Last zur Energie- und Thermalplanung verwenden.

```text
Expected High Load
      ↓
Prepare Sustainable Capacity
```

oder:

```text
Expected Idle Period
      ↓
Prefer Energy Saving
```

Vorhersagen dürfen thermische Sicherheitsgrenzen nicht überschreiben.

## Realtime

Hard-Realtime-Garantien dürfen nicht von probabilistischen Vorhersagen abhängig sein.

```text
Prediction
   ↓
Optimization only

Hard Realtime
   ↓
Deterministic Guarantees
```

Prediction darf Realtime-Ausführung unterstützen, aber keine erforderliche Reservation oder Admission Control ersetzen.

## Fallback

Bei fehlender oder unzuverlässiger Vorhersage muss normales Scheduling vollständig funktionsfähig bleiben.

```text
Prediction Available?
   ├── Yes → Predictive Optimization
   └── No  → Normal Scheduler
```

Predictive Scheduling ist keine Voraussetzung für die korrekte Funktion des Systems.

## Introspection

NovaOS soll erklären können:

```text
What was predicted?
Why?
With what confidence?
What action was taken?
What actually happened?
How large was the error?
```

Dadurch bleiben adaptive Scheduling-Entscheidungen nachvollziehbar.

## Normative Anforderungen

1. NovaOS DARF Vorhersagen zur Optimierung zukünftiger Scheduling-Entscheidungen verwenden.
2. Vorhersagen DÜRFEN NICHT als tatsächlicher Systemzustand behandelt werden.
3. Hard Constraints MÜSSEN Vorrang vor jeder Vorhersage haben.
4. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor adaptiven Scheduling-Vorhersagen haben.
5. Relevante Vorhersagen SOLLEN eine Confidence besitzen.
6. Der Einfluss einer Vorhersage SOLL von ihrer Zuverlässigkeit abhängen.
7. Prediction Error MUSS als Feedback für adaptive Modelle nutzbar sein.
8. Wiederholte Fehlvorhersagen MÜSSEN den zukünftigen Einfluss eines Modells reduzieren können.
9. Spekulative Ressourcenreservierungen MÜSSEN begrenzt bleiben.
10. Hard-Realtime-Garantien DÜRFEN NICHT von probabilistischen Vorhersagen abhängig sein.
11. Der Scheduler MUSS ohne Predictive Scheduling vollständig funktionsfähig bleiben.
12. Vorhersagen, daraus resultierende Entscheidungen und Prediction Errors SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-TOPOLOGY-0001`
- `NPSPEC-SCHEDULER-NUMA-0001`
- `NPSPEC-SCHEDULER-CACHE-0001`
- `NPSPEC-SCHEDULER-ENERGY-0001`
- `NPSPEC-SCHEDULER-THERMAL-0001`
- `NPSPEC-SCHEDULER-QOS-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-SCHED-0012`
- `ADR-ADAPTIVE-0001`
- `ADR-ADAPTIVE-0002`
- `ADR-ADAPTIVE-0003`
- `ADR-ADAPTIVE-0004`

## Ergebnis

```text
Observe
   ↓
Predict
   ↓
Prepare
   ↓
Execute
   ↓
Compare Prediction ↔ Reality
   ↓
Prediction Error
   ↓
Adapt
```

NovaOS erhält damit ein Predictive-Scheduling-Modell, das erwartete Workloads zur frühzeitigen Optimierung nutzt, sich anhand realer Abweichungen selbst korrigiert und jederzeit hinter Hard Constraints sowie expliziten Nutzerentscheidungen zurücktritt.