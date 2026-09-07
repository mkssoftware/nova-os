# ADR-TRANSACTION-0004 – Resource Transactions

## Status

Angenommen

## Kategorie

Transaction Architecture / Resource Economy / Resource Management

## Kontext

Viele Systemoperationen in NovaOS benötigen mehrere Ressourcen gleichzeitig oder verändern bestehende Ressourcenbindungen.

Beispiele sind:

- Speicherreservierungen,
- CPU- und Realtime-Budgets,
- GPU- oder NPU-Ressourcen,
- Storage-Kapazität,
- Netzwerkbandbreite,
- Energie- und thermische Budgets,
- Device-Zugriffe,
- Ressourcen für Provider-Wechsel,
- Ressourcen für Recovery oder Migration.

Werden solche Ressourcen unabhängig voneinander reserviert, kann eine Operation nur einen Teil ihrer benötigten Ressourcen erhalten und anschließend in einem unbrauchbaren Zwischenzustand verbleiben.

```text
Need:
CPU + Memory + GPU

CPU acquired
Memory acquired
GPU unavailable

→ incomplete allocation
```

NovaOS benötigt deshalb transaktionale Ressourcenoperationen.

## Entscheidung

NovaOS führt **Resource Transactions** als Spezialisierung der System Transactions ein.

Zusammengehörige Ressourcenänderungen werden als gemeinsamer Resource Change Set geplant, validiert und committed.

```text
Resource Request
      ↓
Resource Transaction
      ↓
Plan
      ↓
Reserve
      ↓
Validate
      ↓
Commit
      ↓
Active Allocation
```

Kann die erforderliche Gesamtallokation nicht hergestellt werden, wird die Transaktion nicht teilweise als erfolgreicher Zustand übernommen.

## Grundprinzip

```text
Resources required together
are allocated coherently.
```

Resource Transactions koordinieren die systemweite Ressourcenökonomie mit dem Transaktionsmodell.

## Resource Change Set

Eine Resource Transaction kann mehrere Ressourcenklassen umfassen.

Beispiel:

```text
Resource Change Set
 ├── CPU Budget
 ├── Memory
 ├── GPU Memory
 ├── Storage I/O
 └── Network Bandwidth
```

Das Change Set beschreibt die für die Operation gemeinsam benötigten Ressourcen.

## Resource Contracts

Ressourcenanforderungen werden als Contracts beschrieben.

Diese können beispielsweise enthalten:

```text
Minimum
Preferred
Maximum
Priority
Deadline
Duration
Elasticity
Hard / Soft Constraint
```

Die konkrete Struktur wird in NPSPECs definiert.

## Planung

Vor einer Reservation prüft NovaOS, ob der angeforderte Zustand grundsätzlich realisierbar ist.

```text
Resource Requirements
        ↓
Current Availability
        ↓
System Policies
        ↓
Resource Plan
```

Dabei können alternative zulässige Ressourcenpläne entstehen.

## Reservation

Ressourcen können während der Prepare-Phase vorläufig reserviert werden.

```text
Available
   ↓
Reserved
   ↓
Committed
```

Eine Reservation ist noch keine aktive Nutzung.

Reservierte Ressourcen müssen bei Abbruch der Transaction wieder freigegeben werden.

## Commit

Beim Commit werden die vorbereiteten Ressourcenbindungen als gültige Allocation aktiviert.

```text
Reserved Resources
        ↓
Commit
        ↓
Active Allocation
```

Der Commit muss verhindern, dass andere konkurrierende Transactions dieselben exklusiven Ressourcen gleichzeitig erhalten.

## Abort

Kann die Resource Transaction vor Commit nicht vollständig erfüllt werden, wird sie abgebrochen.

```text
Reserve CPU
Reserve Memory
GPU unavailable
      ↓
Abort
      ↓
Release CPU
Release Memory
```

Damit bleiben keine unbeabsichtigten Teilreservierungen bestehen.

## Konkurrenz

Mehrere Resource Transactions können um dieselben Ressourcen konkurrieren.

NovaOS muss Konflikte kontrolliert auflösen können.

Dabei können berücksichtigt werden:

```text
Priority
Deadline
Resource Contract
Fairness
System Criticality
Existing Reservations
Recovery Requirements
```

Priorität erzeugt dabei keine zusätzliche Authority.

## Deadlocks

Mehrere Transactions dürfen nicht unbegrenzt auf gegenseitig reservierte Ressourcen warten.

NovaOS muss Resource-Transaction-Deadlocks verhindern oder erkennen und auflösen können.

Geeignete Strategien können beispielsweise sein:

```text
Canonical Reservation Order
Bounded Waiting
Abort and Retry
Reservation Timeout
Conflict Resolution
```

Die konkrete Strategie wird durch die Resource Runtime bestimmt.

## Temporäre Doppelbelegung

Bestimmte Systemoperationen benötigen während einer Transition vorübergehend alte und neue Ressourcen gleichzeitig.

Beispiel:

```text
Provider A active
      +
Provider B prepared
      ↓
Atomic Switch
      ↓
Provider A released
```

Resource Transactions müssen solche temporären Peak-Anforderungen explizit berücksichtigen.

## Resource Migration

Ressourcen können innerhalb einer Transaction von einem Provider oder Ausführungsort auf einen anderen verschoben werden.

```text
Resource A
    ↓
Prepare Resource B
    ↓
Transfer / Rebind
    ↓
Commit
    ↓
Release Resource A
```

Location Transparency darf dabei reale Ressourcenlimits nicht verbergen.

## Elastic Resources

Nicht jede Resource Transaction benötigt exakt einen festen Wert.

Ein Contract kann einen zulässigen Bereich definieren:

```text
Memory
minimum   = 512 MiB
preferred = 1 GiB
maximum   = 2 GiB
```

NovaOS kann innerhalb dieses Bereichs eine geeignete Allocation auswählen.

Hard Minimums dürfen nicht unterschritten werden.

## Realtime und Safety

Realtime- und Safety-relevante Ressourcen können strengere Reservationsgarantien benötigen.

```text
Realtime Workload
       ↓
Guaranteed Resource Reservation
       ↓
Commit
```

Ressourcen, die für Safe State oder kritische Recovery reserviert sind, dürfen nicht durch gewöhnliche Transactions verdrängt werden.

## Recovery Resources

Nova.Resilience kann eigene reservierte Ressourcenbudgets besitzen.

Diese können beispielsweise für:

```text
Recovery Worker
Checkpoint Restore
Provider Replacement
Emergency Logging
Safe-State Transition
```

vorgehalten werden.

Damit wird verhindert, dass Resource Exhaustion gleichzeitig die Recovery unmöglich macht.

## Rollback

Vor Commit reservierte Ressourcen werden bei Rollback freigegeben.

Nach Commit kann die Rückkehr zum vorherigen Zustand zusätzliche Ressourcen benötigen.

```text
New Allocation
      ↓
Rollback requested
      ↓
Previous resources available?
```

NovaOS darf deshalb nicht voraussetzen, dass jede Ressourcenänderung nach Commit kostenlos reversibel ist.

## Resource Exhaustion

Eine Resource Transaction darf bei Ressourcenmangel nicht unbegrenzt neue Reservationsversuche erzeugen.

Retry Budgets, Backoff und Recovery-Storm-Protection gelten auch für Ressourcenoperationen.

Bei kritischer Erschöpfung kann Nova.Resilience eine gezielte Resource-Exhaustion-Recovery auslösen.

## Desired State

Resource Transactions können Bestandteil atomarer Desired-State-Änderungen sein.

```text
Desired State Change
        ↓
Resource Requirements
        ↓
Resource Transaction
        ↓
Commit
        ↓
Desired State Activation
```

Ein neuer Desired State soll nicht aktiviert werden, wenn seine Hard Resource Requirements offensichtlich nicht erfüllbar sind.

## Capability Provider

Capability Provider können ihre Ressourcenanforderungen deklarieren.

Bei einem Provider-Wechsel kann NovaOS dadurch vorab prüfen, ob der neue Provider innerhalb des verfügbaren Resource Budgets betrieben werden kann.

```text
Provider Candidate
      ↓
Execution Contract
      +
Resource Requirements
      ↓
Resource Transaction
```

## Authority und Isolation

Eine Resource Transaction darf nur Ressourcen reservieren, für die der Initiator beziehungsweise ausführende Systemdienst entsprechende Authority besitzt.

Resource Transactions dürfen keine Isolation zwischen Workloads umgehen.

## Audit und Introspection

Relevante Resource Transactions müssen nachvollziehbar machen können:

```text
Requested Resources
Reserved Resources
Committed Resources
Rejected Requirements
Conflicts
Transaction Owner
Release
Rollback
```

Sensitive Informationen anderer Workloads müssen dabei geschützt bleiben.

## Normative Anforderungen

1. NovaOS MUSS zusammengehörige Ressourcenänderungen als Resource Transactions unterstützen.
2. Resource Transactions MÜSSEN auf dem System-Transaction-Modell basieren.
3. Mehrere gemeinsam benötigte Ressourcen MÜSSEN als gemeinsamer Resource Change Set behandelbar sein.
4. Resource Requirements MÜSSEN als explizite Contracts beschreibbar sein.
5. Hard und Preferred Resource Requirements MÜSSEN unterscheidbar sein.
6. Ressourcen MÜSSEN vor Commit reservierbar sein können.
7. Eine Reservation DARF nicht automatisch als aktive Allocation gelten.
8. Nicht committed Reservations MÜSSEN bei Abort oder Rollback freigegeben werden.
9. Exklusive Ressourcen DÜRFEN nicht gleichzeitig inkompatiblen Transactions committed werden.
10. Resource-Transaction-Konflikte MÜSSEN kontrolliert auflösbar sein.
11. Resource-Transaction-Deadlocks MÜSSEN verhindert oder erkannt und aufgelöst werden können.
12. Temporäre Peak-Ressourcen während Migration oder Provider-Wechsel MÜSSEN berücksichtigt werden.
13. Elastic Resource Contracts MÜSSEN Minimum-, Preferred- und Maximum-Anforderungen unterstützen können.
14. Hard Minimums DÜRFEN nicht durch adaptive Ressourcenoptimierung unterschritten werden.
15. Safety-, Realtime- und Recovery-Reservations MÜSSEN vor Verdrängung durch gewöhnliche Workloads geschützt werden können.
16. Resource Transactions MÜSSEN mit Resource-Exhaustion-Recovery integrierbar sein.
17. Resource Transactions MÜSSEN Bestandteil atomarer Desired-State-Änderungen sein können.
18. Resource Transactions DÜRFEN keine zusätzliche Authority erzeugen oder Workload-Isolation umgehen.
19. Retry- und Reservation-Verhalten MUSS begrenzbar sein.
20. Reservation, Commit, Release, Conflict und Recovery MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- zusammengehörige Ressourcen werden konsistent vergeben,
- fehlgeschlagene Allokationen hinterlassen keine unbeabsichtigten Teilreservierungen,
- Provider-Wechsel und Migrationen können Ressourcen vorab absichern,
- Hard Resource Requirements werden Teil des Transaktionsmodells,
- Recovery- und Safety-Ressourcen können geschützt reserviert werden,
- Resource Economy und Desired State werden direkt miteinander verbunden.

### Negative Konsequenzen

- Reservations benötigen zusätzlichen Verwaltungszustand,
- konkurrierende Transactions benötigen Konflikt- und Deadlock-Behandlung,
- temporäre Doppelbelegung kann kurzfristig höheren Ressourcenbedarf erzeugen.

## Verworfene Alternativen

### Ressourcen unabhängig voneinander reservieren

Verworfen, da dadurch teilweise erfolgreiche, aber insgesamt unbrauchbare Allokationen entstehen können.

### Ressourcen erst nach Desired-State-Commit suchen

Verworfen, da dadurch ein neuer Desired State aktiviert werden könnte, dessen Hard Resource Requirements nicht erfüllbar sind.

### Globale Reservierung aller potenziell benötigten Ressourcen

Verworfen, da dies Ressourcen unnötig blockieren und die systemweite Auslastung erheblich verschlechtern würde.

### Unbegrenztes Warten auf Ressourcen

Verworfen, da dadurch Deadlocks, Starvation und nicht kontrollierbare Transaction-Laufzeiten entstehen können.

## Abhängigkeiten

- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-TRANSACTION-0003_Configuration_Transactions`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-RESILIENCE-0006_Retry_Budgets_Backoff_und_Jitter`
- `ADR-RESILIENCE-0013_Recovery_Storm_Protection`
- `ADR-RESILIENCE-0015_Resource_Exhaustion_Recovery`
- `ADR-RESILIENCE-0016_Safe_State_für_Safety_und_Realtime`

## Zugehörige NPSPECs

- `NPSPEC-TRANSACTION-RESOURCE-0001`
- `NPSPEC-TRANSACTION-RESOURCE-CONTRACT-0001`
- `NPSPEC-TRANSACTION-RESOURCE-RESERVATION-0001`
- `NPSPEC-TRANSACTION-RESOURCE-COMMIT-0001`
- `NPSPEC-TRANSACTION-RESOURCE-CONFLICT-0001`
- `NPSPEC-TRANSACTION-RESOURCE-DEADLOCK-0001`
- `NPSPEC-TRANSACTION-RESOURCE-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-RESOURCE-TEST-0001`

## Ergebnis

NovaOS behandelt zusammengehörige Ressourcenänderungen als transaktionale Allokationen:

```text
Resource Requirements
        ↓
Resource Transaction
        ↓
Plan
        ↓
Reserve
        ↓
Validate
        ↓
Commit
        ↓
Active Allocation
        ↓
Release / Reallocate
```

Kann die notwendige Gesamtallokation nicht hergestellt werden, werden vorbereitete Reservations kontrolliert freigegeben.

Damit verbindet NovaOS seine systemweite Ressourcenökonomie mit dem Transaktionsmodell und verhindert, dass komplexe Systemoperationen aufgrund teilweise erfolgreicher Ressourcenallokationen in inkonsistenten Zuständen verbleiben.