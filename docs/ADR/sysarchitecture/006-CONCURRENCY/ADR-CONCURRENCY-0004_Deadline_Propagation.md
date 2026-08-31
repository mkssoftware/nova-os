# ADR-CONCURRENCY-0004 – Deadline Propagation

## Status

Angenommen

## Kategorie

Kernel / Concurrency / Structured Concurrency / Deadlines / Scheduling / Ressourcen

## Kontext

Structured Concurrency ordnet Tasks und Task Groups hierarchischen Execution Scopes zu.

Neben Cancellation benötigt diese Hierarchie eine einheitliche Semantik für zeitliche Grenzen.

Ohne systemweite Deadline Propagation können Child-Operationen länger laufen als ihre übergeordnete Operation. Verschachtelte Timeouts können außerdem dazu führen, dass ein ursprünglich begrenztes Zeitbudget unbeabsichtigt verlängert wird.

NovaOS benötigt deshalb eine Deadline-Semantik, die über die gesamte Ausführungshierarchie erhalten bleibt.

---

## Entscheidung

NovaOS behandelt **Deadlines als hierarchisch propagierte Execution Constraints**.

Eine Deadline beschreibt den spätesten zulässigen Zeitpunkt für die betreffende Operation.

```text
Parent Scope
Deadline = T1
      ↓
Task Group
Deadline = T2
      ↓
Task
Deadline = T3
```

Für ein Child gilt grundsätzlich:

```text
EffectiveDeadline =
    min(ParentDeadline, RequestedChildDeadline)
```

Ein Child darf die Deadline seines Parents verschärfen, aber nicht verlängern.

---

## Grundprinzip

```text
Deadlines propagate downward.

Children may finish earlier,
but may not extend their parent's time budget.
```

---

## Deadline vs Timeout

NovaOS unterscheidet explizit:

```text
Timeout
    = relative duration

Deadline
    = absolute temporal boundary
```

Relative Timeouts dürfen an API-Grenzen verwendet werden, sollen intern jedoch möglichst früh in eine Deadline überführt werden.

Dadurch wird verhindert:

```text
Operation A: 5 s timeout
    ↓ after 4 s
Operation B: another 5 s timeout
```

obwohl die Gesamtoperation ursprünglich nur fünf Sekunden Zeit besitzen sollte.

---

## Deadline Context

Execution Scopes können einen Deadline Context besitzen.

Dieser wird an:

- Child Scopes,
- Task Groups,
- Tasks,
- geeignete I/O-Operationen,
- RPC-/IPC-Aufrufe

propagiert.

---

## Effektive Deadline

Besitzt nur der Parent eine Deadline:

```text
Child Effective Deadline =
    Parent Deadline
```

Besitzt nur das Child eine Deadline:

```text
Child Effective Deadline =
    Child Deadline
```

Besitzen beide eine Deadline:

```text
Child Effective Deadline =
    earlier(Parent, Child)
```

---

## Keine Deadline-Erweiterung

Ein Child darf keine Parent Deadline verlängern.

Beispiel:

```text
Parent:
    100 ms remaining

Child requests:
    500 ms

Effective:
    100 ms
```

Eine längere Child-Angabe kann als gewünschte Laufzeit dokumentiert werden, besitzt jedoch keine Authority zur Erweiterung der Parent-Grenze.

---

## Deadline Propagation

Die Propagation folgt der Structured-Concurrency-Hierarchie.

```text
Root Scope
    ↓
Task Group
    ↓
Child Group
    ↓
Task
    ↓
I/O / RPC
```

Dadurch bleibt das ursprüngliche Zeitbudget über verschachtelte Operationen erhalten.

---

## Deadline Expiry

Wird eine Deadline erreicht, gilt die betroffene Operation als zeitlich nicht mehr innerhalb ihres Contracts ausführbar.

Typischerweise:

```text
Deadline Reached
      ↓
Cancellation Requested
      ↓
Cleanup / Drain
      ↓
DeadlineExceeded
```

Deadline Expiry und Cancellation bleiben getrennte Konzepte.

Die Deadline ist die Ursache beziehungsweise Constraint-Verletzung; Cancellation ist ein möglicher Lifecycle-Mechanismus zur Beendigung.

---

## Hard und Soft Deadlines

NovaOS unterscheidet mindestens konzeptionell:

```text
Hard Deadline
Soft Deadline
```

Eine Hard Deadline ist Bestandteil eines verbindlichen Execution Contracts.

Eine Soft Deadline dient der Optimierung beziehungsweise Servicequalität.

Das Überschreiten einer Soft Deadline darf beispielsweise zu:

- Degradation,
- Prioritätsänderung,
- Cancellation optionaler Arbeit

führen.

Eine Hard Deadline darf nicht still ignoriert werden.

---

## Deadline und Scheduling

Der Scheduler darf Deadlines für Scheduling-Entscheidungen berücksichtigen.

Eine Deadline ist jedoch nicht automatisch eine Scheduling-Priorität.

Die Scheduling Policy entscheidet, wie:

- Deadline,
- Priorität,
- Ressourcen,
- Realtime-Anforderungen

gemeinsam behandelt werden.

---

## Deadline und Ressourcen

Eine Deadline erzeugt keine Ressourcen.

Kann eine Operation mit den verfügbaren Ressourcen ihre Hard Deadline nicht erfüllen, muss NovaOS abhängig vom Contract:

```text
Reject
Degrade
Replan
Fail
```

statt eine unrealistische Garantie vorzutäuschen.

---

## Admission Control

Für Hard-Realtime- beziehungsweise garantierte Deadline-Operationen kann vor Aktivierung Admission Control erforderlich sein.

```text
Requested Deadline
      +
Required Resources
      ↓
Admission Control
      ↓
Accept / Reject
```

---

## Task Groups

Task Groups übernehmen die effektive Deadline ihres Parents.

Child Tasks können strengere Deadlines besitzen.

Optional Work kann vor Ablauf der Group Deadline beendet werden, um Required Work zu schützen.

---

## I/O

Geeignete I/O-Operationen sollen die verbleibende Deadline erhalten.

Dadurch kann ein Storage-, Netzwerk- oder Device-Provider erkennen, dass ein Ergebnis nach einem bestimmten Zeitpunkt keinen Nutzen mehr besitzt.

Nicht jede Hardwareoperation ist physisch sofort abbrechbar.

---

## IPC und RPC

Deadlines sollen über IPC- und RPC-Grenzen propagierbar sein.

```text
Client Request
Deadline = T
      ↓
Server Request Scope
Deadline ≤ T
```

Ein Server darf aus einem eingehenden Request keine längere verbindliche Deadline ableiten als der Client besitzt.

---

## Remote Deadlines

Bei hostübergreifender Propagation dürfen rohe lokale Clock-Werte nicht ohne definierte Zeitsemantik übertragen werden.

Die Remote-Deadline-Repräsentation muss Clock-Differenzen und Transportzeit berücksichtigen können.

Die konkrete Clock-Synchronisationssemantik wird separat spezifiziert.

---

## Graceful Degradation

Soft Deadlines können mit Graceful Degradation verbunden werden.

Beispiel:

```text
Remaining Time Low
      ↓
Cancel Optional Enhancement
      ↓
Finish Required Result
```

Qualität darf reduziert werden, solange Hard Correctness-, Safety-, Security- und Sovereignty-Anforderungen erhalten bleiben.

---

## Deterministic Mode

Im Deterministic Mode müssen deadlineabhängige Entscheidungen reproduzierbar sein, soweit der Execution Contract dies verlangt.

Adaptive Entscheidungen dürfen eine festgelegte Deadline nicht verändern.

---

## Introspection

Deadline-Zustand muss introspektierbar sein.

Beispiel:

```text
Scope:
    image.render

Deadline:
    15:42:10.250

Remaining:
    37 ms

Class:
    Soft

Source:
    Parent Request
```

Dies unterstützt Diagnose, Scheduling und Architecture Introspection.

---

## Fail-Safe Defaults

Eine Deadline darf nicht still verlängert werden, weil ein Child mehr Zeit benötigt.

Ist eine Hard Deadline nicht erfüllbar, muss dies explizit behandelt werden.

Unbekannte oder fehlende Zeitinformationen dürfen nicht als erfüllte Deadline-Garantie interpretiert werden.

---

## Keine KI-Abhängigkeit

Deadline Propagation muss vollständig deterministisch und ohne KI funktionieren können.

KI oder adaptive Modelle dürfen optional:

- Laufzeiten prognostizieren,
- Ressourcenbedarf schätzen,
- Soft-Deadline-Optimierungen unterstützen.

Sie dürfen Hard Deadlines nicht eigenmächtig verändern oder ignorieren.

---

## Normative Anforderungen

1. NovaOS MUSS Deadlines als First-Class Execution Constraints unterstützen.
2. Deadlines MÜSSEN über Structured-Concurrency-Hierarchien propagierbar sein.
3. Child Scopes MÜSSEN Parent Deadlines übernehmen, sofern keine frühere Child Deadline gilt.
4. Ein Child DARF eine Parent Deadline verschärfen.
5. Ein Child DARF eine Parent Deadline NICHT verlängern.
6. Die effektive Deadline MUSS aus der frühesten anwendbaren Deadline bestimmt werden.
7. Timeout und Deadline MÜSSEN semantisch unterschieden werden.
8. Relative Timeouts SOLLEN intern möglichst früh in Deadlines überführt werden.
9. Deadline Expiry und Cancellation MÜSSEN getrennte Konzepte bleiben.
10. Deadline Expiry MUSS Cancellation auslösen können.
11. NovaOS MUSS Hard und Soft Deadline Semantics unterscheiden können.
12. Hard Deadlines DÜRFEN NICHT still ignoriert oder verlängert werden.
13. Soft Deadlines DÜRFEN Graceful Degradation auslösen.
14. Task Groups MÜSSEN Deadline Propagation unterstützen.
15. Optionale Tasks MÜSSEN zugunsten deadlinekritischer Required Work beendet werden können.
16. Geeignete I/O-Operationen SOLLEN Deadline-Informationen übernehmen können.
17. Deadlines MÜSSEN über IPC-Grenzen propagierbar sein können.
18. Remote Deadline Propagation MUSS unterschiedliche Clock-Domänen berücksichtigen.
19. Eine Deadline DARF NICHT automatisch zusätzliche Ressourcen garantieren.
20. Garantierte Deadlines MÜSSEN mit Resource Reservation beziehungsweise Admission Control integrierbar sein.
21. Nicht erfüllbare Hard Deadlines MÜSSEN explizit abgelehnt oder als Contract Failure behandelt werden.
22. Scheduling Policies MÜSSEN Deadline-Informationen berücksichtigen können.
23. Deadline Propagation MUSS mit Cancellation Propagation integriert sein.
24. Deadline Propagation MUSS mit der systemweiten Resource Economy integrierbar sein.
25. Data-Sovereignty-, Security- und Safety-Constraints DÜRFEN zur Deadline-Erfüllung NICHT abgeschwächt werden.
26. Deterministic Mode MUSS deadlineabhängige Entscheidungen kontrollieren können.
27. Deadline Source und effektive Deadline MÜSSEN introspektierbar sein.
28. Fehlende Deadline-Information DARF NICHT als Deadline-Garantie interpretiert werden.
29. Adaptive Optimierung DARF Hard Deadlines NICHT verändern.
30. Die grundlegende Deadline Propagation MUSS vollständig ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- Zeitbudgets bleiben über verschachtelte Operationen erhalten,
- Child Tasks können Parent-Operationen nicht unbeabsichtigt verlängern,
- bessere Integration von Scheduling und Realtime,
- einheitliche Deadline-Semantik für Tasks, I/O und IPC,
- Grundlage für kontrollierte Graceful Degradation,
- bessere Introspection zeitkritischer Workloads.

### Negative Konsequenzen

- APIs müssen Deadline Contexts weiterreichen,
- Remote Deadlines benötigen definierte Clock-Semantik,
- garantierte Deadlines benötigen Resource Reservation und Admission Control,
- bestehende Timeout-basierte APIs benötigen Adapter.

---

## Verworfene Alternativen

### Nur relative Timeouts

Verworfen.

Verschachtelte Timeouts können das ursprüngliche Gesamtzeitbudget unbeabsichtigt verlängern.

### Child darf eigene längere Deadline wählen

Verworfen.

Dies würde die hierarchische Lifecycle-Garantie des Parents verletzen.

### Deadline automatisch als Thread-Priorität behandeln

Verworfen.

Deadline und Scheduling Priority sind unterschiedliche Konzepte.

### Hard Deadline bei Ressourcenmangel automatisch verlängern

Verworfen.

Dies würde einen verbindlichen Execution Contract still verändern.

### Deadline und Cancellation als dasselbe Konzept behandeln

Verworfen.

Deadline beschreibt eine zeitliche Grenze; Cancellation beschreibt einen Lifecycle-Vorgang.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0002_Task_Groups_mit_hierarchischer_Lebensdauer`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-PROPAGATION-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-TIMEOUT-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-SCHEDULING-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-IPC-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-REALTIME-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-TEST-0001`

---

## Ergebnis

NovaOS propagiert Deadlines entlang derselben Hierarchie wie die Lebensdauer strukturierter Arbeit:

```text
Parent Scope
      ↓
Task Group
      ↓
Child Scope
      ↓
Task
      ↓
I/O / RPC
```

Dabei gilt:

```text
Effective Child Deadline
    =
earliest applicable deadline
```

Damit kann keine untergeordnete Operation das Zeitbudget ihrer übergeordneten Operation still verlängern.

Die zentrale Architekturregel lautet:

```text
Time budgets are hierarchical.

Propagate deadlines downward.

Children may tighten them.

Children may never extend them.
```