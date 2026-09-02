# ADR-DISTCOMM-0010 – End-to-End Deadline Propagation

## Status

Angenommen

## Kategorie

Distributed Communication / Deadlines / Structured Concurrency / Scheduling / Nova.Call

## Kontext

Eine verteilte Operation besteht häufig aus mehreren abhängigen Teiloperationen.

```text
Caller
  ↓
Service A
  ↓
Service B
  ↓
Service C
```

Besitzt der ursprüngliche Caller eine Deadline, reicht es nicht aus, jedem einzelnen Teilaufruf unabhängig ein eigenes Timeout zu geben.

Andernfalls können lokale Timeouts die tatsächliche End-to-End-Deadline überschreiten:

```text
Deadline: 500 ms

A → B: Timeout 500 ms
B → C: Timeout 500 ms
C → D: Timeout 500 ms
```

Obwohl jeder einzelne Call formal innerhalb seines eigenen Timeouts arbeitet, kann die Gesamtoperation ihre zulässige Laufzeit deutlich überschreiten.

NovaOS benötigt deshalb eine systemweite Deadline-Semantik, bei der das verbleibende Zeitbudget entlang der gesamten Causation- und Call-Kette propagiert wird.

---

## Entscheidung

NovaOS verwendet **End-to-End Deadline Propagation** für `Nova.Call` und abhängige Distributed Operations.

Eine Deadline beschreibt den spätesten zulässigen Zeitpunkt beziehungsweise das verbleibende Zeitbudget der gesamten logischen Operation.

```text
Original Deadline
       ↓
Nova.Call
       ↓
Service A
       ↓
Remaining Deadline
       ↓
Service B
       ↓
Remaining Deadline
       ↓
Service C
```

Nachgelagerte Operationen dürfen die übergeordnete Deadline nicht implizit verlängern.

---

## Grundprinzip

```text
Deadlines propagate.
Timeouts do not reset.
```

und:

```text
Child Deadline
    ≤
Parent Deadline
```

---

## Deadline statt unabhängiger Timeouts

NovaOS unterscheidet zwischen einer Deadline und einem lokalen Timeout.

```text
Deadline
    = absolute end-to-end constraint

Timeout
    = local duration constraint
```

Ein lokaler Timeout kann zusätzlich existieren, darf aber die übergeordnete Deadline nicht überschreiten.

Die effektive Grenze ergibt sich konzeptionell aus:

```text
Effective Deadline
    =
min(
    Parent Deadline,
    Local Deadline
)
```

---

## Propagation über Nova.Call

`Nova.Call` propagiert Deadline Context automatisch an abhängige Child Calls, sofern der Contract nichts Strengeres definiert.

```text
Parent Call
 └── Deadline T

Child Call
 └── Deadline ≤ T
```

Eine Komponente muss die ursprüngliche Deadline deshalb nicht manuell bei jedem nachgelagerten Call rekonstruieren.

---

## Integration mit Structured Concurrency

Deadline Propagation folgt der hierarchischen Lebensdauer von Structured Concurrency.

```text
Task Group
 ├── Task A
 │    └── Nova.Call
 └── Task B
      └── Nova.Call
```

Child Tasks und Child Calls erben die Deadline ihres Parent Contexts.

Sie dürfen selbst eine frühere, aber keine spätere effektive Deadline definieren.

---

## Remaining Budget

Jede nachgelagerte Komponente muss das verbleibende Zeitbudget berücksichtigen können.

```text
Original Budget
     ↓
Queue Delay
     ↓
Network Delay
     ↓
Remote Processing
     ↓
Remaining Budget
```

Bereits verbrauchte Zeit wird nicht durch einen neuen Call zurückgesetzt.

---

## Queueing

Deadlines müssen bereits vor der eigentlichen Ausführung berücksichtigt werden.

```text
Call
  ↓
Queue
  ↓
Remaining Deadline
```

Ist eine Deadline aufgrund der aktuellen Queue-Situation nicht mehr erreichbar, soll die Operation nicht unnötig weitere Ressourcen belegen.

Je nach Contract kann NovaOS:

- frühzeitig ablehnen,
- abbrechen,
- degradieren,
- neu planen.

---

## Scheduling

Deadline Context wird dem Scheduler und relevanten Runtime-Komponenten zugänglich gemacht.

Dies erlaubt NovaOS zu erkennen:

```text
Can this operation still meet its deadline?
```

Eine Deadline bedeutet jedoch nicht automatisch Realtime-Garantie.

```text
Deadline
    ≠
Reserved Realtime Capacity
```

Harte Realtime-Garantien benötigen weiterhin explizite Realtime Contracts und Ressourcenreservierungen.

---

## Netzwerk und Transport

Deadline Information muss entlang des NetworkFlow und geeigneter Transportprovider berücksichtigt werden können.

Die Transportauswahl darf beispielsweise berücksichtigen:

- verbleibende Deadline,
- erwartete Latenz,
- Path Health,
- Queueing Delay,
- Migration Cost.

Ein Transport darf die logische End-to-End-Deadline nicht durch einen eigenen längeren Timeout ersetzen.

---

## Retry

Retries verbrauchen das verbleibende Deadline Budget.

```text
Deadline
   ↓
Attempt 1
   ↓
Failure
   ↓
Remaining Budget
   ↓
Retry possible?
```

Ein Retry darf die ursprüngliche Deadline nicht neu starten.

Ist die verbleibende Zeit für einen sinnvollen Retry unzureichend, soll kein weiterer Versuch gestartet werden.

---

## Circuit Breaking

Circuit Breaker können Deadline Information verwenden, um offensichtlich aussichtslose Calls frühzeitig abzulehnen.

Ein bereits als nicht verfügbar klassifiziertes Ziel soll nicht unnötig Deadline Budget durch vorhersehbar erfolglose Verbindungsversuche verbrauchen.

---

## Service Discovery und Provider Selection

Bei mehreren möglichen Service Instances oder Providern kann die verbleibende Deadline Teil der Auswahlentscheidung sein.

```text
Candidate A → expected 20 ms
Candidate B → expected 80 ms
Candidate C → expected 200 ms

Remaining Budget → 60 ms
```

Nur Kandidaten, die alle Hard Constraints erfüllen, kommen grundsätzlich infrage.

Die Deadline ist dabei selbst ein Hard Constraint, wenn der Contract dies verlangt.

---

## Multipath und Connection Migration

Multipath und Connection Migration dürfen zur Einhaltung einer Deadline eingesetzt werden.

Ein Pfadwechsel ist jedoch nur sinnvoll, wenn dessen erwartete Kosten innerhalb des verbleibenden Budgets liegen.

```text
Current Path Failure
       ↓
Remaining Deadline
       ↓
Migration useful?
 ├── Yes → Migrate
 └── No  → Deadline Failure
```

---

## Cancellation

Wird eine Deadline überschritten, muss die zugehörige Operation entsprechend ihrem Contract abgebrochen beziehungsweise als `DeadlineExceeded` behandelt werden.

Cancellation soll entlang der Causation Chain propagiert werden:

```text
DeadlineExceeded
       ↓
Parent Operation
       ↓
Cancellation
       ↓
Child Calls
       ↓
Child Tasks
```

Bereits irreversible Remote Effects können dadurch nicht automatisch rückgängig gemacht werden.

Deadline und Transaction Semantics bleiben getrennte Konzepte.

---

## Unknown Outcome

Eine Deadline kann ablaufen, während eine Remote Operation möglicherweise bereits ausgeführt wurde.

```text
Request
   ↓
Remote Execution
   ↓
Deadline expires
   ↓
Response unavailable
```

In diesem Fall kann das Ergebnis:

```text
UnknownOutcome
```

sein.

`DeadlineExceeded` darf nicht automatisch bedeuten, dass die Remote Operation keinen Effekt erzeugt hat.

Idempotency und Deduplication müssen bei möglichen Retries weiterhin berücksichtigt werden.

---

## Deadline Budget und Suboperationen

Ein Service kann sein verbleibendes Budget bewusst auf mehrere Child Operations verteilen.

```text
Remaining: 300 ms

├── Operation A: ≤ 100 ms
├── Operation B: ≤ 150 ms
└── Reserve:       50 ms
```

Eine solche Budgetierung darf die Parent Deadline nicht überschreiten.

Die konkrete Budgetverteilung ist Policy und wird in NPSPECs spezifiziert.

---

## Clock Semantik

Distributed Deadlines dürfen nicht voraussetzen, dass die Wall Clocks verschiedener Systeme perfekt synchronisiert sind.

NovaOS muss eine Deadline-Repräsentation verwenden, die sichere End-to-End-Semantik trotz verteilter Clock-Unterschiede ermöglicht.

Clock Synchronization kann die Genauigkeit verbessern, darf aber nicht unkontrolliert zur Voraussetzung korrekter Deadline-Behandlung werden.

Die konkrete Repräsentation, Clock-Konvertierung und Fehlergrenzen werden in NPSPECs definiert.

---

## Resource Economy

Deadline-verletzte oder offensichtlich nicht mehr rechtzeitig abschließbare Arbeit soll nicht unnötig weitere Ressourcen verbrauchen.

Deadline Propagation integriert sich deshalb mit:

- CPU Scheduling,
- Queue Management,
- Network Scheduling,
- I/O,
- Accelerator Scheduling,
- Resource Budgets.

Dadurch kann NovaOS Ressourcen für weiterhin sinnvolle Arbeit freigeben.

---

## Graceful Degradation

Erlaubt ein Contract eine degradierte Alternative, kann eine knappe Deadline einen Wechsel auf eine schnellere Alternative auslösen.

```text
Preferred Operation
      ↓
Deadline risk
      ↓
Allowed degradation?
 ├── Yes → Faster Alternative
 └── No  → Continue / Fail
```

Eine Degradation darf nur innerhalb explizit erlaubter Grenzen erfolgen.

Security, Trust, Data Sovereignty und Correctness dürfen nicht zugunsten einer Deadline abgeschwächt werden.

---

## Adaptive Systeme

NovaOS darf historische Laufzeitdaten und Prediction Error verwenden, um vorherzusagen, ob ein Call seine Deadline wahrscheinlich erfüllen kann.

Beispielsweise:

```text
Predicted Latency
       ↓
Actual Latency
       ↓
Prediction Error
       ↓
Model Correction
```

Diese Vorhersagen dürfen Soft Optimization verbessern.

Sie dürfen Hard Constraints nicht verändern.

---

## Deterministic Mode

Im Deterministic Mode müssen Deadline-bezogene Entscheidungen kontrollierbar und reproduzierbar sein.

Adaptive Provider-, Retry-, Migration- oder Degradation-Entscheidungen können entsprechend eingeschränkt werden.

Die Deadline selbst bleibt unabhängig davon ein expliziter Contract.

---

## Introspection und Causation

NovaOS muss Deadline-Verbrauch entlang einer Distributed Operation nachvollziehbar machen können.

Beispielsweise:

```text
Operation
 ├── Original Deadline
 ├── Queue Delay
 ├── Call A
 │    └── 35 ms
 ├── Call B
 │    └── 80 ms
 ├── Retry
 │    └── 20 ms
 └── Remaining Budget
```

Damit kann analysiert werden, welche Komponente das Deadline Budget verbraucht hat.

---

## Normative Anforderungen

1. NovaOS MUSS End-to-End Deadline Propagation für Distributed Operations unterstützen.
2. Deadlines MÜSSEN über `Nova.Call`-Grenzen propagierbar sein.
3. Child Operations DÜRFEN die effektive Parent Deadline NICHT implizit verlängern.
4. Lokale Timeouts DÜRFEN die übergeordnete End-to-End-Deadline NICHT ersetzen.
5. Bereits verbrauchte Zeit DARF bei Child Calls NICHT zurückgesetzt werden.
6. Structured-Concurrency-Child-Tasks MÜSSEN Deadline Context erben können.
7. Queueing MUSS bei der Berechnung des verbleibenden Deadline Budgets berücksichtigt werden.
8. Scheduler und relevante Runtime-Komponenten MÜSSEN Deadline Information berücksichtigen können.
9. Eine Deadline DARF NICHT automatisch als Realtime-Garantie interpretiert werden.
10. NetworkFlow und Transportprovider MÜSSEN Deadline Constraints berücksichtigen können.
11. Retries DÜRFEN die ursprüngliche End-to-End-Deadline NICHT neu starten.
12. Service-, Provider-, Path- und Migration-Entscheidungen MÜSSEN verbleibende Deadlines berücksichtigen können.
13. Deadline Expiration MUSS mit Cancellation Propagation integrierbar sein.
14. `DeadlineExceeded` DARF NICHT automatisch bedeuten, dass eine Remote Operation keinen Effekt erzeugt hat.
15. `UnknownOutcome` MUSS bei entsprechend unsicherem Remote-Zustand darstellbar bleiben.
16. Deadline Propagation DARF NICHT von perfekt synchronisierten Wall Clocks abhängen.
17. Deadline-verletzte Arbeit SOLL möglichst früh Ressourcen freigeben.
18. Graceful Degradation aufgrund einer Deadline DARF nur erfolgen, wenn der Contract sie explizit erlaubt.
19. Security-, Trust-, Correctness- und Data-Sovereignty-Constraints DÜRFEN zur Deadline-Einhaltung NICHT abgeschwächt werden.
20. Adaptive Deadline-Optimierung DARF Hard Constraints NICHT verändern.
21. Deadline Propagation und Deadline-Verbrauch MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- echte End-to-End-Latenzgrenzen statt unabhängiger lokaler Timeouts,
- aussichtslose Arbeit kann frühzeitig beendet werden,
- Retries und Failover berücksichtigen das tatsächlich verbleibende Zeitbudget,
- Scheduler, Netzwerk und Services können gemeinsam auf Deadline Constraints reagieren,
- Structured Concurrency erhält eine konsistente Distributed-Deadline-Semantik,
- Deadline-Verbrauch wird über Servicegrenzen nachvollziehbar.

### Negative Konsequenzen

- Deadline Context muss über mehrere Systemschichten propagiert werden,
- verteilte Clock-Semantik benötigt eine präzise Spezifikation,
- Services müssen mit knappen oder bereits abgelaufenen Deadlines umgehen können,
- Deadline-aware Scheduling und Planning erhöhen die Runtime-Komplexität.

---

## Verworfene Alternativen

### Separater Timeout pro Service

Verworfen.

Lokale Timeouts würden bei langen Call Chains die ursprüngliche End-to-End-Grenze vervielfachen.

### Deadline nur im ursprünglichen Caller

Verworfen.

Nachgelagerte Services könnten ihre Arbeit nicht anhand des tatsächlich verbleibenden Budgets planen.

### Deadline ausschließlich im Netzwerkstack

Verworfen.

Queueing, Compute, Storage, Accelerator Work und Remote Service Processing tragen ebenfalls zur End-to-End-Latenz bei.

### Perfekte Clock Synchronization als Voraussetzung

Verworfen.

Distributed Correctness darf nicht vollständig von perfekt synchronisierten Wall Clocks abhängen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-SCHED-0002_Fair_Deadline_und_Realtime_Scheduling_in_einer_Architektur`
- `ADR-SCHED-0010_QoS_und_Latency_Aware_Scheduling`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`
- `ADR-IPC-0005_Lokale_und_entfernte_Aufrufe_unter_einem_Contract`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0002_Location_Transparency_ohne_Kosten_und_Fehler_zu_verbergen`
- `ADR-DISTCOMM-0007_Bounded_Queues_und_Backpressure`
- `ADR-DISTCOMM-0008_Idempotency_und_Deduplication_für_Exactly_Once_Effects`
- `ADR-DISTCOMM-0009_Operation_Aware_Retry_und_Circuit_Breaking`
- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0019_Connection_Migration`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `NPSPEC-DISTCOMM-DEADLINE-CONTEXT-0001`
- `NPSPEC-DISTCOMM-DEADLINE-PROPAGATION-0001`
- `NPSPEC-DISTCOMM-DEADLINE-BUDGET-0001`
- `NPSPEC-DISTCOMM-DEADLINE-CLOCK-0001`
- `NPSPEC-DISTCOMM-DEADLINE-CANCELLATION-0001`
- `NPSPEC-DISTCOMM-DEADLINE-INTROSPECTION-0001`
- `NPSPEC-DISTCOMM-DEADLINE-TEST-0001`

---

## Ergebnis

NovaOS behandelt Deadlines als durchgängigen Contract einer gesamten Distributed Operation:

```text
                End-to-End Deadline
                         ↓
                      Caller
                         ↓
                     Nova.Call
                         ↓
                    Service A
                         ↓
                 Remaining Budget
                         ↓
                    Service B
                         ↓
                 Remaining Budget
                         ↓
                    Service C
                         ↓
               Result / DeadlineExceeded
```

Damit wird bei jedem nachgelagerten Call nur das tatsächlich verbleibende Zeitbudget weitergegeben.

NovaOS verhindert so, dass lokale Timeouts eine globale Deadline unbemerkt verlängern, und schafft eine gemeinsame Deadline-Semantik für Structured Concurrency, Scheduling, Netzwerk, Retries, Failover und verteilte Services.