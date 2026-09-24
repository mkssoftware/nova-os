# NPSPEC-PROCESS-SUPERVISION-0001 – Nova Process Supervision

## Status

Entwurf

## Kategorie

Process / Supervision / Lifecycle / Resilience

## Zweck

NovaOS definiert ein einheitliches Supervisionsmodell zur Überwachung und kontrollierten Behandlung von Prozessen und deren Fehlerzuständen.

```text
Supervisor
    ↓
Process
    ↓
Health / Exit / Failure
    ↓
Supervision Policy
    ↓
Restart / Stop / Recover
```

## Grundprinzip

Lang laufende oder kritische Prozesse sollen einen eindeutig definierten Supervisor besitzen.

```text
Process Failure
      ↓
Supervisor
      ↓
Defined Recovery Action
```

Supervision ersetzt keine Prozessisolation, sondern ergänzt sie um Lifecycle- und Recovery-Verantwortung.

## Supervisor

Ein Supervisor kann verantwortlich sein für:

```text
Process Start
Lifecycle Monitoring
Failure Handling
Restart
Termination
Resource Cleanup
Health Evaluation
Escalation
```

Ein Supervisor kann selbst ein Prozess, Systemdienst oder Kernelmechanismus sein.

## Supervisionsbeziehung

Die Beziehung wird explizit im Systemmodell geführt.

```text
Supervisor
   ├── Process A
   ├── Process B
   └── Process C
```

Dabei gilt:

```text
Supervision ≠ Permission
```

Die Supervisionsbeziehung erzeugt keine automatischen Zugriffsrechte auf Ressourcen des überwachten Prozesses.

## Ereignisse

Ein Supervisor kann mindestens auf folgende Ereignisse reagieren:

```text
Normal Exit
Unexpected Exit
Process Failure
Health Failure
Timeout
Resource Violation
Security Termination
Repeated Restart Failure
```

## Supervision Policy

Für überwachte Prozesse kann eine Policy definieren:

```text
Restart
Stop
Ignore
Recover
Escalate
```

Die Entscheidung kann vom Fehlergrund, Prozesszustand und bisherigen Fehlversuchen abhängen.

## Restart

Ein Prozess darf nach einem Fehler automatisch neu gestartet werden.

```text
Failure
   ↓
Cleanup
   ↓
Restart Decision
   ↓
Spawn
   ↓
Health Check
```

Ein Neustart muss eine neue gültige Prozessinstanz erzeugen.

## Restart Budget

Unbegrenzte Neustartschleifen sind zu verhindern.

```text
Restart Count
+
Time Window
+
Backoff
    ↓
Restart Decision
```

Wird das definierte Budget überschritten, muss eine Eskalation möglich sein.

## Fehlereskalation

Kann ein Supervisor einen Fehler nicht behandeln:

```text
Process
   ↓
Supervisor
   ↓
Higher Supervisor
   ↓
Recovery / System Policy
```

Damit können hierarchische Supervisionsstrukturen aufgebaut werden.

## Supervisor-Ausfall

Der Ausfall eines Supervisors muss für seine abhängigen Prozesse definiert behandelt werden.

Mögliche Aktionen:

```text
Transfer Supervision
Terminate Children
Adopt by Higher Supervisor
Enter Recovery
```

Prozesse dürfen nicht unkontrolliert ohne Lifecycle-Verantwortung verbleiben.

## Health Monitoring

Supervision darf neben dem Prozesszustand zusätzliche Health-Informationen verwenden.

```text
Running ≠ Healthy
```

Ein laufender, aber nicht funktionsfähiger Prozess kann dadurch erkannt und behandelt werden.

## Normative Anforderungen

1. NovaOS MUSS explizite Prozess-Supervision unterstützen.
2. Kritische oder dauerhaft laufende Prozesse SOLLEN einen definierten Supervisor besitzen.
3. Supervisionsbeziehungen MÜSSEN explizit im Systemmodell abgebildet werden.
4. Supervision DARF keine impliziten zusätzlichen Berechtigungen erzeugen.
5. Prozessfehler MÜSSEN dem zuständigen Supervisor gemeldet werden können.
6. Supervisoren MÜSSEN definierte Recovery-Aktionen auslösen können.
7. Automatische Neustarts MÜSSEN durch Restart Budgets begrenzbar sein.
8. Wiederholte Fehler MÜSSEN eskaliert werden können.
9. Neustarts MÜSSEN über den kontrollierten Process-Spawn erfolgen.
10. Der Ausfall eines Supervisors MUSS definiert behandelt werden.
11. Prozesse DÜRFEN NICHT unkontrolliert ohne Lifecycle-Verantwortung verbleiben.
12. Supervisionsentscheidungen SOLLEN diagnostizierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-PROCESS-MODEL-0001`
- `NPSPEC-PROCESS-LIFECYCLE-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-PROCESS-SPAWN-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-PROCESS-0006`

## Ergebnis

```text
Process
   ↓
Explicit Supervisor
   ↓
Monitor Lifecycle / Health
   ↓
Failure
   ↓
Restart / Recover / Escalate
```

NovaOS erhält damit eine kontrollierte Supervisionsstruktur, die Prozessfehler erkennt, Recovery-Maßnahmen ausführt und unkontrollierte Neustart- oder Fehlerketten verhindert.