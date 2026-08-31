# ADR-SYNC-0006 – Deadlock Detection und Diagnostics

## Status

Angenommen

## Kategorie

Kernel / Synchronisation / Concurrency / Deadlock / Diagnostics

## Kontext

NovaOS unterstützt unterschiedliche Synchronisationsmechanismen:

- Mutexes,
- Priority-Inheritance-Locks,
- Futex-artige Waits,
- Semaphores,
- Wait Queues,
- RCU,
- atomare und lockfreie Strukturen,
- Ressourcen- und I/O-Waits.

Sobald Tasks mehrere exklusive Ressourcen halten und weitere Ressourcen anfordern können, sind zyklische Abhängigkeiten möglich.

Beispiel:

```text
Task A
  owns Lock 1
  waits for Lock 2

Task B
  owns Lock 2
  waits for Lock 1
```

Es entsteht:

```text
Task A → Lock 2 → Task B → Lock 1 → Task A
```

Keiner der beteiligten Tasks kann selbstständig Fortschritt erzielen.

Priority Inheritance aus `ADR-SYNC-0005` kann Priority Inversion reduzieren, löst solche zyklischen Abhängigkeiten jedoch nicht.

NovaOS benötigt deshalb eine gemeinsame Architektur für Deadlock-Erkennung und Deadlock-Diagnose.

---

## Entscheidung

NovaOS führt einen systemweit nutzbaren **Wait-Dependency-Graph** als Grundlage für Deadlock Detection und Diagnostics ein.

```text
Task
 ↓ waits for
Resource
 ↓ owned by
Task
 ↓ waits for
Resource
```

Zyklische Abhängigkeiten in diesem Graphen können als Deadlock-Kandidaten erkannt werden.

Die Architektur unterscheidet dabei zwischen:

```text
Deadlock Prevention
Deadlock Detection
Deadlock Diagnostics
Deadlock Recovery
```

Diese Aufgaben bleiben getrennte Policies.

Nicht jeder Wait wird automatisch als Deadlock behandelt und nicht jeder erkannte Deadlock darf automatisch durch den Kernel aufgelöst werden.

---

## Grundprinzip

```text
Track blocking dependencies.

Detect cycles where practical.

Make deadlocks explainable.

Do not hide synchronization failures.
```

---

## Wait-Dependency-Graph

Der Dependency Graph beschreibt relevante blockierende Beziehungen.

Knoten können beispielsweise repräsentieren:

```text
Task
Execution Scope
Mutex
Resource
Kernel Object
```

Kanten beschreiben insbesondere:

```text
Task → waits for → Resource

Resource → owned by → Task
```

Damit kann eine Abhängigkeitskette rekonstruiert werden.

---

## Owner-basierte Ressourcen

Für owner-basierte Synchronisationsprimitive ist die Deadlock-Erkennung besonders eindeutig.

Beispiel:

```text
Task A → Mutex B → Task C → Mutex D → Task A
```

Ein Zyklus aus ausschließlich blockierenden, nicht selbstauflösenden Abhängigkeiten stellt einen Deadlock dar.

---

## Nicht-owner-basierte Waits

Nicht jede Wartebedingung besitzt einen eindeutigen Owner.

Beispiele:

- Condition Variables,
- Events,
- I/O Completion,
- externe Geräteereignisse,
- Netzwerkantworten.

Solche Waits dürfen nicht künstlich einem beliebigen Owner zugeordnet werden.

NovaOS muss deshalb zwischen:

```text
Owned Dependency

und

External / Unowned Dependency
```

unterscheiden.

Dadurch werden falsche Deadlock-Diagnosen vermieden.

---

## Deadlock Detection

Deadlock Detection kann bei geeigneten Ereignissen durchgeführt werden, beispielsweise:

- beim Aufbau einer neuen blockierenden Abhängigkeit,
- bei ungewöhnlich langen Waits,
- durch Debug-/Diagnostic-Tools,
- durch periodische Systemanalyse.

Eine permanente vollständige globale Graphanalyse für jede Synchronisationsoperation wird nicht vorgeschrieben.

Die konkrete Strategie darf abhängig von:

- Build-Konfiguration,
- Scheduling Class,
- Realtime-Anforderungen,
- Diagnosemodus,
- Systemlast

variieren.

---

## Lock Dependency Tracking

Für Locks soll NovaOS zusätzlich Lock-Abhängigkeiten beobachten können.

Beispiel:

```text
Task holds A
Task acquires B

Dependency:
A → B
```

Wird an anderer Stelle die inverse Reihenfolge beobachtet:

```text
B → A
```

kann bereits eine potenzielle Deadlock-Gefahr erkannt werden, bevor ein tatsächlicher Deadlock auftritt.

Diese Funktion ist insbesondere für Debug- und Development-Builds vorgesehen.

---

## Tatsächlicher und potenzieller Deadlock

NovaOS unterscheidet:

### Actual Deadlock

Es existiert aktuell eine zyklische blockierende Abhängigkeit ohne möglichen Fortschritt.

### Potential Deadlock

Beobachtete Lock-Reihenfolgen oder Abhängigkeiten zeigen, dass ein Deadlock bei einer zukünftigen Ausführungsreihenfolge möglich wäre.

Diese Zustände müssen diagnostisch getrennt dargestellt werden.

---

## Timeouts sind keine Deadlock Detection

Ein langer Wait oder Timeout kann auf einen Deadlock hinweisen, beweist ihn jedoch nicht.

```text
Long Wait != Deadlock
Timeout != Deadlock
```

Timeouts dürfen als Trigger für eine Analyse verwendet werden.

Sie ersetzen keine Dependency-Analyse.

---

## Priority Inheritance

Der Dependency Graph soll mit der Priority-Inheritance-Infrastruktur gemeinsam nutzbare Owner-/Waiter-Beziehungen verwenden.

```text
Waiter
  ↓
Lock
  ↓
Owner
```

kann sowohl für:

- Priority Inheritance,
- Deadlock Detection,
- Diagnostics

relevant sein.

Die Mechanismen bleiben semantisch getrennt.

---

## Structured Concurrency

Structured Concurrency reduziert unkontrollierte Task-Lebensdauer, verhindert aber keine Deadlocks zwischen gemeinsam verwendeten Ressourcen.

Deadlock-Diagnostics sollen deshalb Execution-Scope-Informationen berücksichtigen können.

Beispiel:

```text
Execution Domain
  ↓
Scope
  ↓
Task A
  ↓
Mutex X
  ↓
Task B
```

Dadurch wird sichtbar, welche logischen Systemkomponenten an einer Blockierung beteiligt sind.

---

## Cross-Domain Deadlocks

Deadlocks können über mehrere Execution Domains hinweg entstehen, beispielsweise durch:

- Shared Memory,
- IPC,
- System Services,
- gemeinsam verwendete Kernelressourcen.

Der Dependency Graph muss solche Beziehungen grundsätzlich darstellen können, sofern die beteiligten Subsysteme entsprechende Dependency-Informationen bereitstellen.

Security- und Data-Sovereignty-Grenzen bleiben bei der Darstellung erhalten.

---

## Distributed und Remote Dependencies

Location Transparency kann Abhängigkeiten zu entfernten Ressourcen erzeugen.

NovaOS darf Remote Dependencies in Diagnostics darstellen:

```text
Local Task
    ↓
Remote Service
    ↓
Remote Resource
```

Eine vollständige verteilte Deadlock-Erkennung wird durch diese ADR jedoch nicht zwingend vorgeschrieben.

Unvollständige Remote-Informationen müssen als solche gekennzeichnet werden.

---

## Realtime

Hard-Realtime-Pfade dürfen nicht von unbeschränkt teurer Deadlock-Analyse im kritischen Scheduling-Pfad abhängig sein.

Realtime-Systeme sollen Deadlocks primär durch:

- definierte Lock-Reihenfolgen,
- begrenzte kritische Abschnitte,
- analysierbare Ressourcenabhängigkeiten,
- geeignete Synchronisationsprotokolle

vermeiden.

Deadlock Detection bleibt eine zusätzliche Schutz- und Diagnoseebene.

---

## Recovery

Deadlock Detection und Deadlock Recovery werden getrennt.

Ein erkannter Deadlock bedeutet nicht automatisch, dass der Kernel einen beliebigen beteiligten Task terminieren darf.

Mögliche Recovery-Policies können abhängig vom Contract sein:

```text
Diagnostic only
Cancel operation
Cancel scope
Restart service
Rollback transaction
Terminate execution domain
Escalate to supervisor
```

Die konkrete Recovery-Policy wird separat spezifiziert.

---

## Transaktionale Systemoperationen

Wenn ein Deadlock innerhalb einer transaktionalen Systemoperation auftritt, kann ein kontrollierter Abbruch und Rollback eine geeignete Recovery-Strategie darstellen.

Dies muss jedoch durch den jeweiligen Transaction Contract erlaubt sein.

Deadlock Detection darf nicht eigenständig beliebige Transaktionen zurückrollen.

---

## Architecture Introspection

Deadlock-Zustände müssen über Architecture Introspection sichtbar gemacht werden können.

Eine Diagnose soll mindestens darstellen können:

```text
Deadlock ID

Task A
  waits for Mutex X

Mutex X
  owned by Task B

Task B
  waits for Mutex Y

Mutex Y
  owned by Task A
```

Zusätzlich können relevant sein:

```text
Execution Domain
Execution Scope
Scheduling Class
Base Priority
Effective Priority
Deadline
Wait Duration
Lock Acquisition Site
Resource Type
```

---

## Diagnostics

Entwicklungs- und Debug-Werkzeuge sollen aus einem Deadlock möglichst eine verständliche Dependency Chain erzeugen.

Beispiel:

```text
Deadlock detected:

Task 42 "Renderer"
    owns: SurfaceLock
    waits: BufferLock

Task 81 "Compositor"
    owns: BufferLock
    waits: SurfaceLock

Cycle:
Renderer
 → BufferLock
 → Compositor
 → SurfaceLock
 → Renderer
```

Damit soll nicht nur festgestellt werden, **dass** ein Deadlock existiert, sondern auch **warum**.

---

## Production und Debug Mode

Nicht jede Diagnosefunktion muss im Produktionsbetrieb dieselben Kosten verursachen.

NovaOS darf unterschiedliche Stufen unterstützen:

```text
Minimal Runtime Tracking
        ↓
Production Diagnostics
        ↓
Development Diagnostics
        ↓
Full Dependency Validation
```

Korrektheitsrelevante Informationen dürfen jedoch nicht ausschließlich von optionalen Debug-Instrumentierungen abhängen.

---

## Deterministic Mode

Deterministic Mode soll Deadlock-Situationen reproduzierbarer machen können.

Dependency Graph und Diagnoseinformationen dürfen dabei verwendet werden, um:

- Lock-Reihenfolgen,
- Task-Abhängigkeiten,
- Wait-Sequenzen

zu rekonstruieren.

Die Deadlock-Definition selbst verändert sich durch Deterministic Mode nicht.

---

## Fail-Safe Verhalten

Unvollständige Dependency-Information darf nicht als Beweis dafür interpretiert werden, dass kein Deadlock existiert.

Es gilt:

```text
No detected cycle
    !=
Proof of no deadlock
```

Insbesondere bei:

- externen Ressourcen,
- Remote Dependencies,
- nicht instrumentierten Komponenten

muss die Diagnose ihren Kenntnisstand kenntlich machen.

---

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame Architektur zur Darstellung blockierender Abhängigkeiten bereitstellen.
2. Owner-basierte Wait-Beziehungen MÜSSEN als Dependency Graph darstellbar sein.
3. Zyklische owner-basierte Abhängigkeiten MÜSSEN als Deadlock-Kandidaten erkennbar sein.
4. Nicht-owner-basierte Waits DÜRFEN NICHT künstlich einem Owner zugeordnet werden.
5. Actual und Potential Deadlocks MÜSSEN diagnostisch unterscheidbar sein.
6. Lock-Order-Tracking SOLL für Entwicklungs- und Diagnosezwecke verfügbar sein.
7. Ein Timeout DARF NICHT automatisch als Deadlock interpretiert werden.
8. Lange Waits DÜRFEN eine Deadlock-Analyse auslösen.
9. Priority-Inheritance- und Deadlock-Infrastruktur SOLLEN gemeinsame Dependency-Informationen verwenden können.
10. Deadlock Detection DARF Priority Inheritance NICHT ersetzen.
11. Structured-Concurrency-Informationen SOLLEN in Deadlock-Diagnostics integrierbar sein.
12. Cross-Domain-Abhängigkeiten MÜSSEN grundsätzlich darstellbar sein.
13. Security- und Data-Sovereignty-Grenzen MÜSSEN bei Diagnostics erhalten bleiben.
14. Remote Dependencies MÜSSEN bei unvollständigem Wissen entsprechend gekennzeichnet werden.
15. Realtime-Pfade DÜRFEN NICHT von unbeschränkt teurer Deadlock-Analyse abhängen.
16. Hard-Realtime-Systeme SOLLEN Deadlocks primär durch analysierbare Synchronisationsregeln vermeiden.
17. Deadlock Detection und Recovery MÜSSEN getrennte Policies bleiben.
18. Ein erkannter Deadlock DARF NICHT automatisch zur willkürlichen Task-Termination führen.
19. Recovery MUSS den jeweiligen Execution-, Transaction- und Supervision-Contracts folgen.
20. Deadlock-Zyklen MÜSSEN über Architecture Introspection darstellbar sein.
21. Diagnostics SOLLEN Owner, Waiter, Ressource und Dependency Chain darstellen können.
22. Debug-Informationen SOLLEN Lock-Acquisition-Sites enthalten können.
23. Produktionsinstrumentierung MUSS kostenkontrolliert implementierbar sein.
24. Deterministic Mode DARF zur reproduzierbaren Analyse von Deadlocks verwendet werden.
25. Unvollständige Dependency-Information DARF NICHT als Beweis für Deadlock-Freiheit gelten.
26. Deadlock-Diagnostics MÜSSEN zwischen bekannten, unbekannten und externen Abhängigkeiten unterscheiden können.

---

## Konsequenzen

### Positive Konsequenzen

- Deadlocks werden systemweit nachvollziehbar,
- Priority-Inheritance-Abhängigkeiten können wiederverwendet werden,
- potenziell gefährliche Lock-Reihenfolgen können früh erkannt werden,
- Cross-Domain-Deadlocks werden diagnostizierbar,
- Debugging komplexer Synchronisationsprobleme wird erheblich verbessert.

### Negative Konsequenzen

- Dependency Tracking verursacht zusätzlichen Zustand,
- vollständige Deadlock-Erkennung kann teuer sein,
- nicht-owner-basierte und externe Abhängigkeiten bleiben teilweise schwer analysierbar,
- umfangreiche Diagnoseinstrumentierung muss kostenkontrolliert werden.

---

## Verworfene Alternativen

### Deadlocks ausschließlich durch Timeouts erkennen

Verworfen.

Timeouts unterscheiden Deadlocks nicht zuverlässig von langsamer Ausführung.

### Automatische Task-Termination bei jedem erkannten Deadlock

Verworfen.

Die korrekte Recovery hängt von Ownership, Transaktionen, Execution Domains und Systemkritikalität ab.

### Vollständige globale Graphanalyse bei jedem Lock-Vorgang

Verworfen.

Die Kosten wären insbesondere für performance- und realtimekritische Pfade unangemessen.

### Keine Kernel-Unterstützung für Deadlock-Diagnostics

Verworfen.

Systemweite Abhängigkeiten zwischen Scheduler, Locks, Execution Domains und Kernelressourcen können von reinem Anwendungscode nicht vollständig rekonstruiert werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-SYNC-0001_Atomics_als_Fundament_der_Low_Level_Synchronisation`
- `ADR-SYNC-0004_Futex_artige_User_Kernel_Synchronisation`
- `ADR-SYNC-0005_Priority_Inheritance_für_Realtime`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-PROCESS-0005_Systemweite_Process_Supervision`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-SCHED-0001_Präemptiver_Mehrklassen_Scheduler`

---

## Zugehörige NPSPECs

- `NPSPEC-SYNC-DEADLOCK-0001`
- `NPSPEC-SYNC-DEPENDENCY-GRAPH-0001`
- `NPSPEC-SYNC-LOCK-ORDER-0001`
- `NPSPEC-SYNC-DEADLOCK-DETECTION-0001`
- `NPSPEC-SYNC-DEADLOCK-DIAGNOSTICS-0001`
- `NPSPEC-SYNC-DEADLOCK-RECOVERY-0001`
- `NPSPEC-SYNC-DEADLOCK-INTROSPECTION-0001`
- `NPSPEC-SYNC-DEADLOCK-TEST-0001`

---

## Ergebnis

NovaOS behandelt Deadlocks als explizit modellierbare Abhängigkeitsprobleme:

```text
Wait / Ownership Information
          ↓
Dependency Graph
          ↓
Cycle Detection
          ↓
Deadlock Diagnostics
          ↓
Contract-defined Recovery
```

Priority Inheritance reduziert Priority Inversion, während Deadlock Detection zyklische Ressourcenabhängigkeiten sichtbar macht.

Die zentrale Architekturregel lautet:

```text
Track dependencies.

Detect cycles.

Explain the cause.

Recover only according to policy.
```