# ADR-PROCESS-0005 – Systemweite Process Supervision

## Status

Angenommen

## Kategorie

Kernel / Prozesse / Supervision / Lifecycle / Dienste / Recovery / Zuverlässigkeit

## Kontext

NovaOS behandelt Prozesse gemäß:

- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`,
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`,
- `ADR-PROCESS-0003_Spawn_als_bevorzugtes_natives_Prozessmodell`.

Damit existiert bereits eine klare Grundlage für:

- Prozesserstellung,
- Isolation,
- Ressourcen,
- Capabilities,
- Lifecycle,
- Structured Concurrency.

Was zusätzlich benötigt wird, ist ein systemweites Modell zur Überwachung und Steuerung langlebiger Prozesse.

Typische Beispiele sind:

- Systemdienste,
- Hintergrunddienste,
- Treiberprozesse,
- Session-Dienste,
- Anwendungsdienste,
- Agenten,
- Worker-Prozesse,
- kritische Infrastrukturprozesse.

In klassischen Systemen wird Supervision häufig auf einzelne Service Manager oder Anwendungslösungen verteilt.

NovaOS soll stattdessen eine gemeinsame Architektur besitzen, in der Prozesszustand, Abhängigkeiten, Fehler, Restart-Policy und Health strukturiert modelliert werden.

---

## Entscheidung

NovaOS führt **systemweite Process Supervision** als gemeinsame Lifecycle- und Recovery-Infrastruktur ein.

Ein Prozess kann einem Supervisor zugeordnet werden.

Der Supervisor besitzt keine unbegrenzte Autorität, sondern nur explizite Supervision-Capabilities über die von ihm verwalteten Prozesse.

Das Grundmodell lautet:

```text
Desired Process State
        ↓
Supervisor
        ↓
Spawn / Observe / Control
        ↓
Process
        ↓
Health + Exit State
        ↓
Supervisor Policy
        ↓
Restart / Recover / Escalate / Stop
```

Supervision ist hierarchisch und policygesteuert.

---

## Grundprinzip

```text
Processes may fail.

Failure handling must be explicit.

Supervision belongs to the system model,
not to ad-hoc restart loops.
```

---

## Supervision ist kein Prozessstartmodell

Supervision ersetzt Spawn nicht.

Es gilt:

```text
Spawn
    = Process Creation Mechanism

Supervision
    = Lifecycle and Recovery Policy
```

Ein Supervisor verwendet Spawn, wenn ein Prozess gestartet oder neu gestartet werden soll.

---

## Supervisor

Ein Supervisor ist eine kontrollierte Systemkomponente, die den Lifecycle anderer Execution Domains beziehungsweise Prozesse überwachen kann.

Konzeptionell:

```text
ProcessSupervisor {
    id
    supervisedSet
    desiredState
    restartPolicy
    healthPolicy
    failurePolicy
    resourcePolicy
}
```

Die konkrete Implementierung wird separat spezifiziert.

---

## Supervision Relationship

Die Beziehung zwischen Supervisor und Prozess muss explizit sein.

```text
Supervisor
    ↓ supervises
Process
```

Ein Prozess darf nicht allein aufgrund einer Parent-/Child-Beziehung automatisch vollständig beaufsichtigt werden.

---

## Supervision Hierarchy

Supervisoren dürfen hierarchisch organisiert werden.

Beispiel:

```text
System Supervisor
      │
      ├── Core Services Supervisor
      │      ├── Storage Service
      │      └── Network Service
      │
      ├── Driver Supervisor
      │      ├── Audio Driver
      │      └── GPU Driver
      │
      └── User Session Supervisor
             ├── Desktop Shell
             └── Session Services
```

Damit kann Fehlerbehandlung lokal erfolgen, bevor sie auf höhere Ebenen eskaliert.

---

## One-for-One Recovery

Ein Supervisor kann nur den betroffenen Prozess neu starten.

```text
Process A fails
      ↓
Restart A
```

Dies ist geeignet, wenn der Fehler lokal begrenzt ist.

---

## One-for-All Recovery

Wenn Prozesse eng gekoppelt sind, kann der Fehler eines Mitglieds den Neustart der gesamten Gruppe auslösen.

```text
A fails
 ↓
Stop A B C
 ↓
Restart A B C
```

Dies darf nur explizit konfiguriert werden.

---

## Rest-for-One Recovery

Eine geordnete abhängige Prozessgruppe kann eine Rest-for-One-Strategie besitzen.

Beispiel:

```text
A → B → C → D

B fails

restart:
B C D
```

Damit können abhängige Zustände konsistent rekonstruiert werden.

---

## Restart Policy

Ein Supervisor besitzt eine explizite Restart Policy.

Mögliche Modi sind:

```text
Never
OnFailure
Always
OnSpecificFailure
```

Optional können zusätzliche Bedingungen definiert werden.

---

## Kein Blind Restart

Ein automatischer Restart ist nicht immer sicher.

Beispiele:

- nicht-idempotente externe Operation,
- beschädigter persistenter Zustand,
- fehlgeschlagene Systemmigration,
- Security-Verletzung,
- wiederholter deterministischer Programmfehler.

Deshalb gilt:

```text
Crash
    ≠
Always Restart
```

Die Restart Policy muss Ursache und Kontext berücksichtigen können.

---

## Exit Classification

Prozessbeendigungen müssen typisiert sein.

Beispiel:

```text
NormalExit
RequestedTermination
UnhandledFailure
ResourceViolation
SecurityViolation
DependencyFailure
WatchdogFailure
PlatformFailure
```

Die Supervision Policy kann unterschiedlich darauf reagieren.

---

## Health

Ein laufender Prozess gilt nicht allein deshalb als gesund, weil er existiert.

NovaOS unterscheidet:

```text
Running
```

von:

```text
Healthy
```

Ein Prozess kann laufen und dennoch:

- hängen,
- keine Requests mehr verarbeiten,
- interne Deadlines verletzen,
- Ressourcen blockieren.

---

## Health Model

Ein Supervisor darf strukturierte Health-Signale verwenden.

Beispiel:

```text
HealthState {
    status
    lastUpdate
    reason
}
```

Mögliche Zustände:

```text
Starting
Healthy
Degraded
Unhealthy
Unknown
```

---

## Unknown ist nicht Healthy

Ein fehlendes Health-Signal darf nicht automatisch als positiver Zustand interpretiert werden.

Es gilt:

```text
Unknown
    ≠
Healthy
```

Die konkrete Reaktion hängt von der Policy ab.

---

## Readiness und Liveness

NovaOS unterscheidet mindestens zwei unterschiedliche Aspekte:

```text
Liveness
    → process can still make progress

Readiness
    → process can currently serve its function
```

Ein Dienst kann beispielsweise:

```text
Alive
but
Not Ready
```

sein.

---

## Startup Supervision

Nach Spawn kann ein Supervisor auf Readiness warten.

Konzeptionell:

```text
Spawn
   ↓
Starting
   ↓
Readiness Validation
   ↓
Ready
```

Ein bloß erfolgreicher Spawn bedeutet nicht automatisch, dass der Dienst betriebsbereit ist.

---

## Startup Deadline

Für Startup beziehungsweise Readiness kann eine Deadline definiert werden.

Beispiel:

```text
Startup Deadline = 5 s
```

Wird sie verletzt, kann die Policy:

- weiter warten,
- Prozess terminieren,
- neu starten,
- degradieren,
- eskalieren.

---

## Watchdog

Supervisoren können Watchdog-Mechanismen verwenden.

Ein Watchdog darf nicht auf bloßem Lebenszeichen ohne Semantik beruhen.

Mögliche Signale:

```text
heartbeat
progress counter
request completion
deadline observation
structured health report
```

Ein Prozess, der nur einen Heartbeat sendet, aber keine Arbeit mehr erledigt, darf nicht zwangsläufig als gesund gelten.

---

## Progress Awareness

Für kritische Dienste darf Supervision Fortschritt statt reiner Aktivität überwachen.

Beispiel:

```text
Last completed operation:
    12:01:04

Current time:
    12:01:20

Maximum allowed stall:
    5 s

Result:
    Unhealthy
```

---

## Restart Limits

Automatische Neustarts müssen begrenzt werden können.

Beispiel:

```text
maxRestarts = 5
window = 60 s
```

Dadurch werden endlose Crash-Loops verhindert.

---

## Backoff

Wiederholte Neustarts sollen kontrollierten Backoff unterstützen.

Beispiel:

```text
1 s
2 s
4 s
8 s
...
```

Backoff kann:

- fest,
- exponentiell,
- policybasiert

sein.

---

## Jitter

Für große Gruppen verteilter beziehungsweise ähnlicher Prozesse darf Restart-Jitter verwendet werden, um synchrone Neustartwellen zu vermeiden.

Jitter ist eine Optimierung und darf Deterministic Mode nicht unkontrolliert beeinflussen.

---

## Restart Storm Prevention

Der Supervisor muss Crash- und Restart-Stürme erkennen können.

Beispiel:

```text
Process crashes repeatedly
        ↓
Restart threshold exceeded
        ↓
Stop restarting
        ↓
Escalate
```

Damit darf ein fehlerhafter Dienst nicht unbegrenzt Systemressourcen verbrauchen.

---

## Escalation

Kann ein Supervisor einen Fehler nicht lokal behandeln, wird dieser an die nächsthöhere Supervision-Ebene eskaliert.

```text
Process Failure
    ↓
Local Supervisor
    ↓ recovery failed
Parent Supervisor
    ↓
System Policy
```

Mögliche höhere Maßnahmen:

- gesamte Service Group neu starten,
- auf Fallback-Dienst wechseln,
- Funktion deaktivieren,
- Safe Mode,
- Recovery Mode,
- Systemneustart.

---

## Abhängigkeiten

Supervisoren müssen Prozessabhängigkeiten modellieren können.

Beispiel:

```text
Storage Service
      ↓
Database Service
      ↓
Application Service
```

Ein ausgefallener Storage Service kann abhängige Prozesse in:

```text
Degraded
NotReady
Stopped
```

versetzen.

---

## Dependency ist nicht automatisch Restart

Eine Abhängigkeit definiert nicht zwangsläufig:

```text
dependency fails
    ↓
restart everything
```

Policy bestimmt die Reaktion.

Mögliche Reaktionen:

```text
Wait
Degrade
Rebind
Restart
Fallback
Stop
```

---

## Declarative System Model

Systemweite Supervision wird mit dem Declarative System Model integriert.

Beispiel:

```text
Desired:
    nova.storage.service = Running + Healthy

Actual:
    Failed

Reconciler:
    ↓
evaluate policy
    ↓
restart / fallback / recovery
```

Damit ist Supervision nicht nur Eventreaktion, sondern Teil der Desired-vs-Actual-Reconciliation.

---

## Desired State

Für supervisierte Prozesse kann ein Desired State angegeben werden.

Beispiele:

```text
Running
Stopped
Suspended
```

Zusätzlich können Health-Anforderungen existieren.

Beispiel:

```text
Desired:
    Running
    Ready
    Healthy
```

---

## Reconciliation

Ein Supervisor gleicht:

```text
Desired State
```

mit:

```text
Actual State
```

ab.

Beispiel:

```text
Desired = Running
Actual = Terminated
Policy = Restart

Action = Spawn
```

---

## Transaktionale Systemoperationen

Komplexe Supervision-Aktionen können transaktional geplant werden.

Beispiel:

```text
Prepare replacement
    ↓
Validate
    ↓
Quiesce old service
    ↓
Rebind dependencies
    ↓
Activate new service
    ↓
Verify
    ↓
Commit
```

Dies ist besonders relevant für Hot Replacement und Live Evolution.

---

## Resource Economy

Supervisoren müssen Ressourcenlimits beachten.

Ein Restart darf nicht automatisch alle bisherigen Ressourcenreservierungen erneut garantieren.

Der Supervisor kann vor Restart:

```text
Admission Control
```

durchführen.

Kann ein Dienst nicht mit seinen Hard Requirements gestartet werden, wird dies als expliziter Recovery-Fehler behandelt.

---

## Resource Leak Detection

Wiederholte Prozessstarts dürfen keine alten Ressourcenlecks akkumulieren.

Nach Prozessende müssen entsprechend der Ownership-Regeln unter anderem freigegeben beziehungsweise zurückgegeben werden:

- Speicher,
- Handles,
- IPC Endpoints,
- Resource Reservations,
- Device Bindings.

---

## Restart und persistenter Zustand

Der Supervisor ist nicht automatisch für die Reparatur fachlicher Daten zuständig.

Prozesszustand und persistenter Datenzustand sind getrennt.

Beispiel:

```text
Process restart succeeded
```

bedeutet nicht:

```text
persistent database state is valid
```

Eine Recovery Policy kann dafür zusätzliche Health- oder Integrity-Prüfungen verlangen.

---

## State Recovery

Ein Prozess kann deklarieren, wie sein Laufzeitzustand wiederhergestellt werden kann.

Mögliche Klassen:

```text
Stateless
Restartable
CheckpointRecoverable
ExternallyRecoverable
NonRestartable
```

Diese Information kann die Supervision Policy beeinflussen.

---

## Stateless Services

Stateless Services können typischerweise einfach neu gestartet werden.

```text
Crash
  ↓
Restart
```

sofern keine anderen Constraints verletzt werden.

---

## Stateful Services

Stateful Services können vor einem Restart zusätzliche Schritte benötigen.

Beispiel:

```text
Validate persistent state
        ↓
Recover journal
        ↓
Spawn
        ↓
Verify readiness
```

Der Supervisor koordiniert die Schritte, muss ihre fachliche Logik jedoch nicht selbst implementieren.

---

## Structured Concurrency

Supervision und Structured Concurrency ergänzen sich.

Structured Concurrency kontrolliert Arbeit **innerhalb** eines Execution Scopes.

Process Supervision kontrolliert langlebige Execution Domains **oberhalb** dieser Scopes.

```text
Supervisor
    ↓
Process
    ↓
Root Execution Scope
    ↓
Tasks
```

---

## Parent/Child ist nicht Supervisor/Supervisee

Eine Prozess-Parent-Beziehung und eine Supervision-Beziehung sind unterschiedliche Konzepte.

```text
Parent
    ≠
Supervisor
```

Ein Prozess kann von einer Komponente beaufsichtigt werden, die ihn nicht direkt erzeugt hat.

---

## Capability-basierte Supervision

Supervisorrechte müssen explizit sein.

Beispiele:

```text
Process.Observe
Process.Wait
Process.Start
Process.Stop
Process.Restart
Process.Health.Read
Process.Resource.Adjust
```

Die Existenz einer Supervision-Beziehung darf keine uneingeschränkte Debug- oder Speicherzugriffsberechtigung erzeugen.

---

## Least Authority

Ein Supervisor erhält nur die Rechte, die er für seine Aufgabe benötigt.

Ein Supervisor, der einen Prozess neu starten darf, muss beispielsweise nicht automatisch dessen privaten Speicher lesen dürfen.

---

## Security Failures

Security-Verletzungen können eine andere Policy als gewöhnliche Abstürze besitzen.

Beispiel:

```text
Memory Fault
    → restart allowed

Trust Violation
    → isolate + audit + no automatic restart
```

Automatische Neustarts dürfen Sicherheitsprobleme nicht verschleiern.

---

## Trust

Vor jedem erneuten Spawn gelten weiterhin die aktuellen Trust-Prüfungen.

Ein zuvor vertrauenswürdiges Executable darf nicht blind neu gestartet werden, wenn dessen Trust-Zustand inzwischen ungültig ist.

---

## Capability Reevaluation

Bei Restart müssen Capabilities neu bewertet werden können.

Der neue Prozess muss nicht automatisch exakt dieselben Rechte wie der vorherige erhalten.

Beispiel:

```text
Policy changed
    ↓
Restart
    ↓
reduced capability set
```

---

## Data Sovereignty

Supervision darf Data-Sovereignty-Regeln nicht umgehen.

Ein Recovery-Fallback auf einen Remote-Service darf beispielsweise nicht erfolgen, wenn:

```text
Data Policy = LocalOnly
```

gilt.

---

## Graceful Degradation

Wenn ein Prozess nicht wiederhergestellt werden kann, darf NovaOS kontrolliert degradieren.

Beispiele:

```text
Primary GPU service unavailable
    ↓
software renderer

Primary network service unavailable
    ↓
offline mode
```

Degradation darf keine Hard Safety-, Security- oder Sovereignty-Constraints verletzen.

---

## Fallback Provider

Supervisoren können alternative Provider kennen.

Beispiel:

```text
Primary Service
      ↓ failed
Fallback Service
      ↓
Verify
      ↓
Rebind
```

Die Auswahl eines Fallbacks bleibt Policy.

---

## Hot Replacement

Systemweite Supervision bildet eine Grundlage für Hot Replacement.

Anstatt:

```text
kill old
restart new
```

kann eine kontrollierte Ersetzung stattfinden:

```text
Prepare Candidate
      ↓
Health Check
      ↓
Quiesce Old
      ↓
Transfer / Rebind
      ↓
Activate Candidate
      ↓
Verify
      ↓
Commit
```

---

## Session Supervision

Benutzersitzungen können eigene Supervisoren besitzen.

Beispiel:

```text
User Session Supervisor
      ├── Desktop Shell
      ├── Notification Service
      ├── Clipboard Service
      └── User Agents
```

Ein Fehler innerhalb einer Session muss nicht automatisch das gesamte System beeinflussen.

---

## Application Supervision

Mehrprozess-Anwendungen dürfen eine eigene Supervision-Struktur besitzen.

Beispiel:

```text
Application Supervisor
      ├── Main UI
      ├── Renderer
      └── Worker
```

Dabei bleibt die Systempolicy autoritativ bezüglich:

- Capabilities,
- Ressourcen,
- Prozessisolation.

---

## Driver Supervision

User-Mode- und isolierte Treiber können supervisiert werden.

Beispiel:

```text
Driver crashes
    ↓
Device quiesce
    ↓
Revoke DMA / IRQ resources
    ↓
Restart driver
    ↓
Rebind device
    ↓
Verify
```

Ein Restart darf niemals auf noch aktive alte DMA- oder Hardwarezugriffe vertrauen.

---

## Kritische Prozesse

Prozesse können unterschiedliche Kritikalitätsklassen besitzen.

Beispiel:

```text
Optional
Normal
Important
Critical
BootCritical
```

Die Kritikalität beeinflusst die Eskalationspolitik.

Sie verleiht nicht automatisch zusätzliche Authority.

---

## Critical Service Failure

Ein wiederholt nicht wiederherstellbarer kritischer Dienst kann höhere Maßnahmen auslösen.

Beispiel:

```text
Critical storage service
    ↓
recovery failed
    ↓
enter Recovery Mode
```

Ein vollständiger Systemneustart ist nur eine mögliche Eskalation und nicht die einzige.

---

## Kernel Failure Boundary

Process Supervision gilt primär für isolierbare Execution Domains.

Ein fundamentaler Kernel-Corruption-Fall kann nicht zuverlässig durch normalen Prozessneustart repariert werden.

In solchen Fällen greifen höhere Recovery-, Self-Healing- oder Bootmechanismen.

---

## Deterministic Mode

Im Deterministic Mode müssen Supervision-Entscheidungen reproduzierbar sein.

Dazu können festgelegt werden:

```text
restart count
backoff
restart timing
failure classification
dependency order
fallback selection
```

Zufälliger Jitter und adaptive Restartstrategien müssen in entsprechenden Scopes deaktivierbar sein.

---

## Realtime

Realtime-Prozesse können spezielle Supervisionregeln benötigen.

Ein Restart darf beispielsweise nicht als automatische Garantie gelten, eine verlorene Deadline nachträglich zu reparieren.

Realtime-Supervision kann stattdessen:

- redundante Provider,
- Hot Standby,
- vorreservierte Ressourcen

verwenden.

---

## Observability

Supervisoren müssen relevante Zustände beobachten können.

Mindestens:

- Prozessstatus,
- Exit Reason,
- Restart Count,
- Health,
- Readiness,
- Ressourcenstatus,
- Dependency State.

Dabei gelten Capability- und Datenschutzgrenzen.

---

## Introspection

Supervision-Strukturen werden in Architecture Introspection dargestellt.

Beispiel:

```text
Supervisor:
    system.storage

Policy:
    OnFailure

Supervised:
    nova.storage.service

State:
    Healthy

Restart Count:
    1

Last Failure:
    ResourceViolation

Dependencies:
    nvme.driver
```

---

## Supervision Decision Record

Wichtige Recovery-Entscheidungen sollen strukturiert nachvollziehbar sein.

Konzeptionell:

```text
SupervisionDecision {
    process
    observedState
    failure
    selectedAction
    policy
    attempt
    result
}
```

Dies ist ein technischer Entscheidungsdatensatz und kein internes Chain-of-Thought.

---

## Audit

Sicherheits- und systemrelevante Ereignisse sollen auditierbar sein.

Beispiele:

```text
ProcessFailed
RestartRequested
RestartDenied
RestartLimitReached
SupervisorEscalated
FallbackActivated
```

---

## Adaptive Optimierung

Nichtkritische Parameter dürfen adaptiv optimiert werden.

Beispiele:

- Backoff-Dauer,
- Health-Check-Frequenz,
- bevorzugter Fallback.

Prediction Error darf als Lernsignal dienen.

Beispiel:

```text
Prediction:
    quick restart will recover

Observed:
    repeated immediate crash

Prediction Error
    ↓
increase backoff / change recovery strategy
```

Hard Constraints und Restart Limits dürfen dadurch nicht aufgehoben werden.

---

## Keine KI-Abhängigkeit

Die gesamte grundlegende Supervision muss regelbasiert und deterministisch funktionsfähig sein.

KI darf optional:

- Fehlercluster erkennen,
- Root-Cause-Hinweise liefern,
- Recovery-Strategien vorschlagen.

KI besitzt keine notwendige Autorität für:

- Restart,
- Termination,
- Capability-Erteilung,
- Security Escalation.

---

## Fail-Safe Defaults

Ist unklar, ob ein automatischer Restart sicher ist, wird nicht blind neu gestartet.

Ist unklar, ob ein Fallback Sicherheits- oder Data-Sovereignty-Regeln erfüllt, wird er nicht aktiviert.

Es gilt:

```text
Unknown safety
    ≠
permission to recover arbitrarily
```

---

## Normative Anforderungen

1. NovaOS MUSS systemweite Process Supervision unterstützen.
2. Supervision MUSS auf dem nativen Execution-Domain- und Prozessmodell aufbauen.
3. Spawn MUSS der bevorzugte Mechanismus zur Prozesserzeugung und zum Restart bleiben.
4. Supervision-Beziehungen MÜSSEN explizit modelliert werden.
5. Parent-/Child-Beziehungen DÜRFEN NICHT automatisch vollständige Supervision bedeuten.
6. Supervisoren MÜSSEN hierarchisch organisiert werden können.
7. Supervisoren MÜSSEN typisierte Prozessbeendigungen unterscheiden können.
8. Running und Healthy MÜSSEN getrennte Zustände sein.
9. Readiness und Liveness MÜSSEN getrennt modellierbar sein.
10. Unknown Health DARF NICHT automatisch als Healthy gelten.
11. Restart Policies MÜSSEN explizit konfigurierbar sein.
12. Automatische Restarts MÜSSEN begrenzbar sein.
13. Restart Backoff MUSS unterstützt werden können.
14. Crash-Loops MÜSSEN erkennbar und begrenzbar sein.
15. Recovery-Fehlschläge MÜSSEN an höhere Supervisoren eskalierbar sein.
16. Prozessabhängigkeiten MÜSSEN explizit modellierbar sein.
17. Dependency Failure DARF NICHT automatisch einen globalen Restart erzwingen.
18. Desired und Actual Process State MÜSSEN über Reconciliation abgleichbar sein.
19. Hard Resource Requirements MÜSSEN auch bei Restart erneut geprüft werden.
20. Prozess-Restart DARF persistente Datenintegrität NICHT implizit garantieren.
21. Stateful Services MÜSSEN eigene Recovery-Schritte integrieren können.
22. Supervisorrechte MÜSSEN capability-basiert sein.
23. Ein Supervisor DARF NICHT allein aufgrund seiner Rolle uneingeschränkten Prozesszugriff erhalten.
24. Security Failures MÜSSEN von gewöhnlichen Runtime Failures unterscheidbar sein.
25. Trust und Capabilities MÜSSEN bei Restart neu bewertet werden können.
26. Data-Sovereignty-Regeln DÜRFEN durch Fallback oder Restart NICHT umgangen werden.
27. Supervision MUSS Graceful Degradation und alternative Provider unterstützen können.
28. Supervision-Zustand und Recovery-Entscheidungen MÜSSEN introspektierbar sein.
29. Wiederholte sicherheits- oder systemrelevante Recovery-Ereignisse SOLLEN auditierbar sein.
30. Die vollständige grundlegende Process Supervision MUSS ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliche Fehlerbehandlung für langlebige Prozesse,
- weniger ad-hoc Restart-Logik,
- bessere Crash-Loop-Kontrolle,
- strukturierte Health- und Readiness-Semantik,
- klare Recovery- und Eskalationspfade,
- gute Integration mit Declarative System Model,
- gute Grundlage für Self-Healing,
- systemweite Sicht auf Abhängigkeiten,
- geeignet für Dienste, Treiber, Sessions und Mehrprozess-Anwendungen,
- Hot Replacement kann auf derselben Lifecycle-Infrastruktur aufbauen.

### Negative Konsequenzen

- Supervision erhöht die Komplexität des Prozess-Lifecycle-Modells,
- Health-Semantik muss pro Dienst sinnvoll definiert werden,
- falsch konfigurierte Restart Policies können unnötige Recovery-Loops erzeugen,
- Abhängigkeitsgraphen müssen vor Zyklen und unklaren Recovery-Regeln geschützt werden.

---

## Verworfene Alternativen

### Jeder Prozess implementiert seinen eigenen Restart Loop

Verworfen.

Dies führt zu inkonsistenten Recovery-Regeln und erschwert systemweite Ressourcen- und Fehlerkontrolle.

### Nur ein globaler Service Manager ohne Hierarchie

Verworfen.

Lokale Fehler sollten möglichst lokal behandelt werden können.

### Jeden Crash automatisch sofort neu starten

Verworfen.

Dies kann Crash-Loops, Datenkorruption und Sicherheitsprobleme verschärfen.

### Prozess existiert = Prozess gesund

Verworfen.

Ein Prozess kann laufen und dennoch keine korrekte Funktion mehr erfüllen.

### Parent-Prozess automatisch als Supervisor

Verworfen.

Parent-/Child- und Supervision-Semantik sind unterschiedliche Beziehungen.

### KI-basierte Recovery als Voraussetzung

Verworfen.

Grundlegende Systemzuverlässigkeit muss ohne KI funktionieren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0003_Spawn_als_bevorzugtes_natives_Prozessmodell`
- `ADR-PROCESS-0004_Fork_nur_als_Kompatibilitätssemantik`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Declarative_System_Model`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvable_System_Architecture`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-PROCESS-SUPERVISION-0001`
- `NPSPEC-PROCESS-SUPERVISOR-0001`
- `NPSPEC-PROCESS-SUPERVISION-HIERARCHY-0001`
- `NPSPEC-PROCESS-RESTART-POLICY-0001`
- `NPSPEC-PROCESS-HEALTH-0001`
- `NPSPEC-PROCESS-READINESS-0001`
- `NPSPEC-PROCESS-WATCHDOG-0001`
- `NPSPEC-PROCESS-RESTART-LIMIT-0001`
- `NPSPEC-PROCESS-BACKOFF-0001`
- `NPSPEC-PROCESS-DEPENDENCY-0001`
- `NPSPEC-PROCESS-SUPERVISION-RECOVERY-0001`
- `NPSPEC-PROCESS-SUPERVISION-FALLBACK-0001`
- `NPSPEC-PROCESS-SUPERVISION-INTROSPECTION-0001`
- `NPSPEC-PROCESS-SUPERVISION-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- supervisierter normaler Prozessstart,
- NormalExit ohne Restart,
- Failure mit OnFailure-Restart,
- Always-Restart-Policy,
- Never-Restart-Policy,
- Readiness nach Spawn,
- Startup Timeout,
- Healthy → Unhealthy,
- Liveness Failure,
- laufender, aber nicht fortschreitender Prozess,
- Watchdog Failure,
- One-for-One-Recovery,
- One-for-All-Recovery,
- Rest-for-One-Recovery,
- Restart Limit,
- Backoff,
- Restart Storm,
- Eskalation zum Parent Supervisor,
- Dependency Failure,
- abhängiger Prozess in Degraded State,
- Stateful Service Recovery,
- fehlgeschlagene Hard Resource Reservation beim Restart,
- Trust-Rejection beim Restart,
- Capability-Änderung zwischen zwei Starts,
- Security Failure ohne blindes Auto-Restart,
- Data-Sovereignty-konformer Fallback,
- abgelehnter nicht zulässiger Remote-Fallback,
- Driver Crash mit Resource Revocation,
- Session-lokaler Fehler ohne Systemausfall,
- Deterministic Supervision,
- Introspection,
- Audit.

---

## Ergebnis

NovaOS behandelt Prozessfehler nicht als Sonderfälle, die jeder Dienst selbst lösen muss.

Stattdessen entsteht eine systemweite Supervision-Hierarchie:

```text
System Supervisor
      ↓
Subsystem Supervisor
      ↓
Process
      ↓
Health / Exit / Resource State
      ↓
Policy
      ↓
Restart / Recover / Fallback / Escalate
```

Damit werden Prozess-Lifecycle, Health, Restart und Recovery als kontrollierte Bestandteile der Systemarchitektur behandelt.

Die zentrale Architekturregel lautet:

```text
Expect processes to fail.

Supervise them explicitly.

Restart only when safe.

Escalate when local recovery fails.

Keep recovery policy above process mechanics.
```