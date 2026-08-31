# ADR-CONCURRENCY-0003 – Cancellation Propagation

## Status

Angenommen

## Kategorie

Kernel / Concurrency / Structured Concurrency / Cancellation / Lifecycle

## Kontext

Mit `ADR-CONCURRENCY-0001_Structured_Concurrency` und `ADR-CONCURRENCY-0002_Task_Groups_mit_hierarchischer_Lebensdauer` besitzt NovaOS eine hierarchische Ausführungsstruktur:

```text
Execution Domain
      ↓
Execution Scope
      ↓
Task Group
      ↓
Task
```

Damit diese Struktur zuverlässig beendet werden kann, benötigt NovaOS eine einheitliche Cancellation-Semantik.

Cancellation darf nicht nur eine lokale Eigenschaft einzelner Tasks sein. Wird eine übergeordnete Operation abgebrochen, müssen davon abhängige Child-Operationen kontrolliert informiert werden.

Gleichzeitig darf Cancellation nicht mit einem sofortigen erzwungenen Thread-Abbruch gleichgesetzt werden.

---

## Entscheidung

NovaOS definiert **hierarchische Cancellation Propagation** als Bestandteil von Structured Concurrency.

Eine Cancellation wird grundsätzlich vom betroffenen Execution Scope zu seinen abhängigen Child Scopes, Task Groups und Tasks propagiert.

```text
Cancel Parent Scope
        ↓
Cancellation Requested
        ↓
┌──────────────┬──────────────┐
↓              ↓              ↓
Child A        Child B        Child Group
↓              ↓              ↓
Cancel         Cancel         Propagate
```

Cancellation ist dabei zunächst eine kontrollierte Aufforderung zur Beendigung.

---

## Grundprinzip

```text
Cancellation follows ownership.

Parents cancel children.

Children clean up before leaving.

Forced termination is escalation,
not normal cancellation.
```

---

## Cancellation Context

Jeder strukturierte Execution Scope besitzt einen Cancellation Context.

Dieser beschreibt mindestens, ob Cancellation:

```text
NotRequested
Requested
```

ist.

Child Scopes erhalten einen mit dem Parent verbundenen Cancellation Context.

---

## Abwärtsgerichtete Propagation

Cancellation wird standardmäßig von Parent zu Child propagiert.

```text
Parent
  ↓
Child
  ↓
Grandchild
```

Wird der Parent gecancelt, werden alle abhängigen Children ebenfalls zur Cancellation aufgefordert.

---

## Keine automatische Aufwärts-Cancellation

Die Cancellation eines Childs cancelt nicht automatisch seinen Parent.

```text
Cancel Child
    ≠
Cancel Parent
```

Ob ein Child-Abbruch Auswirkungen auf Parent oder Siblings besitzt, entscheidet die Failure- beziehungsweise Group Policy.

---

## Sibling Isolation

Ein gecanceltes Child cancelt nicht automatisch seine Geschwister.

Beispiel:

```text
Task Group
   ├── Task A
   ├── Task B
   └── Task C
```

Cancellation von `Task B` betrifft `Task A` und `Task C` nur dann, wenn die Group Policy dies verlangt.

---

## Cooperative Cancellation

Cooperative Cancellation ist der bevorzugte Mechanismus.

Eine Task erkennt den Cancellation Request und beendet ihre Arbeit an einem sicheren Punkt.

Typische Cancellation Points sind:

```text
await
wait
sleep
I/O
channel operation
resource acquisition
explicit cancellation check
```

---

## Cancellation ist kein Thread Kill

Ein Cancellation Request darf nicht automatisch bedeuten:

```text
destroy execution context immediately
```

Ein sofortiger Abbruch könnte:

- Locks zurücklassen,
- Ressourcen verlieren,
- Datenstrukturen inkonsistent machen,
- Transaktionen beschädigen.

Erzwungene Beendigung ist deshalb ein separater Eskalationsmechanismus.

---

## Cancellation Safety

Operationen müssen definieren können, wie sie auf Cancellation reagieren.

Relevante Kategorien sind beispielsweise:

```text
Cancellable
DeferredCancellation
NonCancellableSection
```

Nicht abbrechbare Abschnitte müssen kurz und begründet sein.

---

## Cleanup

Nach Cancellation muss eine Task ihre scopegebundenen Ressourcen kontrolliert behandeln.

Dazu gehören beispielsweise:

- Handles,
- Buffer,
- Locks,
- temporäre Dateien,
- Reservations,
- IPC-Zustand.

Structured Resource Cleanup bleibt Bestandteil des normalen Lifecycles.

---

## Cancellation und I/O

Bei asynchronem I/O muss zwischen:

```text
logical cancellation
```

und:

```text
physical operation cancellation
```

unterschieden werden.

Kann eine Hardwareoperation nicht mehr physisch gestoppt werden, muss ihre spätere Completion weiterhin sicher verarbeitet werden.

Eine Completion darf nicht auf bereits ungültige Ressourcen zugreifen.

---

## Cancellation und Deadlines

Das Überschreiten einer Deadline kann einen Cancellation Request auslösen.

```text
Deadline Exceeded
      ↓
Cancellation Requested
```

Deadline und Cancellation bleiben jedoch unterschiedliche Konzepte:

```text
Deadline
    = zeitliche Grenze

Cancellation
    = Lifecycle-Signal
```

---

## Cancellation und Task Groups

Wird eine Task Group gecancelt, propagiert die Cancellation an ihre aktiven Child Tasks und Child Groups.

Die Group gilt erst dann als beendet, wenn ihre Lifecycle-Policy erfüllt ist.

Typischerweise:

```text
Cancel
  ↓
Propagate
  ↓
Cleanup / Drain
  ↓
Join
  ↓
Cancelled
```

---

## Cancellation und Process Termination

Bei kontrollierter Prozessbeendigung wird zunächst der Root Execution Scope gecancelt.

```text
Terminate Process
      ↓
Cancel Root Scope
      ↓
Propagate
      ↓
Drain / Join
      ↓
Release Resources
      ↓
Terminate Domain
```

Damit erhält strukturierte Arbeit die Möglichkeit, kontrolliert zu enden.

---

## Eskalation

Reagiert eine Task nicht innerhalb der zulässigen Grenzen auf Cancellation, kann die Lifecycle Policy eskalieren.

Beispiel:

```text
Cancellation Request
      ↓
Grace Period
      ↓
Still Running
      ↓
Escalation
```

Mögliche Eskalationen sind abhängig vom Kontext:

```text
Force operation abort
Terminate execution context
Terminate process
Supervisor escalation
```

---

## Ressourcenbudgets

Eine gecancelte Operation darf Ressourcen nicht unbegrenzt während des Cleanup weiterverwenden.

Cleanup kann deshalb eigene begrenzte Ressourcen- und Zeitbudgets besitzen.

---

## Capability Revocation

Cancellation und Capability Revocation sind getrennte Mechanismen.

Ein Scope kann gecancelt werden, ohne sofort alle Ressourcenrechte zu verlieren, damit kontrolliertes Cleanup möglich bleibt.

Nach Abschluss beziehungsweise Eskalation können scopegebundene Capabilities revokiert werden.

---

## Cross-Domain Cancellation

Bei IPC oder RPC kann ein Cancellation Request über Domain-Grenzen weitergegeben werden.

```text
Client Scope
    ↓ request
Server Scope
```

Wird der Client Request gecancelt, kann ein entsprechendes Cancellation-Signal an den Server gesendet werden.

Dies erzeugt keine uneingeschränkte Authority über das Server Domain.

---

## Remote Cancellation

Bei hostübergreifender Ausführung kann Cancellation nicht immer garantiert sofort zugestellt werden.

Remote Cancellation muss daher mit:

- Verbindungsfehlern,
- Verzögerungen,
- verlorenen Peers

umgehen können.

Lokale Ressourcen dürfen nicht unbegrenzt auf eine Remote-Bestätigung warten.

---

## Deterministic Mode

Im Deterministic Mode muss die relevante Cancellation-Semantik reproduzierbar sein.

Insbesondere dürfen adaptive oder zufällige Entscheidungen nicht verändern:

- welche Children Cancellation erhalten,
- welche Cleanup-Reihenfolge semantisch erforderlich ist,
- wann eine definierte Eskalation erfolgt.

---

## Realtime

Realtime Scopes können feste Cancellation- und Cleanup-Budgets besitzen.

Eine Cancellation darf Hard-Realtime-Anforderungen nicht durch unbegrenzt lange Cleanup-Phasen verletzen.

---

## Graceful Degradation

Cancellation kann gezielt für optionale Arbeit verwendet werden.

Beispiel:

```text
Deadline pressure
      ↓
Cancel Optional Tasks
      ↓
Preserve Required Tasks
```

Damit unterstützt Cancellation das systemweite Graceful-Degradation-Modell.

---

## Introspection

Cancellation muss introspektierbar sein.

Mindestens sollen abhängig von Berechtigungen erkennbar sein:

```text
Scope
Cancellation Requested
Cancellation Source
Current State
Elapsed Cancellation Time
```

Dies erleichtert die Diagnose blockierter Shutdown- oder Cleanup-Vorgänge.

---

## Fail-Safe Defaults

Ist unklar, ob eine Operation sicher sofort beendet werden kann, darf NovaOS nicht automatisch einen unsicheren harten Abbruch durchführen.

Gleichzeitig darf eine nicht reagierende Operation nicht unbegrenzt das Ende ihres owning Scopes verhindern.

Deshalb muss eine definierte Eskalationspolicy existieren.

---

## Keine KI-Abhängigkeit

Cancellation Propagation ist grundlegende Lifecycle-Infrastruktur.

Sie muss vollständig ohne KI funktionieren.

KI darf weder entscheiden, ob Hard Constraints ignoriert werden, noch eine unbekannte Operation eigenmächtig als sicher abbrechbar klassifizieren.

---

## Normative Anforderungen

1. NovaOS MUSS Cancellation als First-Class-Lifecycle-Konzept behandeln.
2. Jeder strukturierte Execution Scope MUSS einen Cancellation Context besitzen können.
3. Cancellation MUSS standardmäßig vom Parent zu abhängigen Children propagiert werden.
4. Child Cancellation DARF NICHT automatisch den Parent canceln.
5. Child Cancellation DARF NICHT automatisch Sibling Tasks canceln, sofern die Group Policy dies nicht verlangt.
6. Cooperative Cancellation SOLL der bevorzugte Mechanismus sein.
7. Cancellation DARF NICHT mit sofortigem Thread Kill gleichgesetzt werden.
8. System- und Runtime-APIs MÜSSEN definierte Cancellation Points unterstützen können.
9. Nicht abbrechbare Abschnitte MÜSSEN explizit und begrenzt sein.
10. Cancellation MUSS kontrolliertes Resource Cleanup ermöglichen.
11. Asynchrones I/O MUSS logische und physische Cancellation unterscheiden können.
12. Verspätete Completions DÜRFEN NICHT auf ungültige Scope-Ressourcen zugreifen.
13. Deadline Expiry MUSS Cancellation auslösen können.
14. Deadline und Cancellation MÜSSEN semantisch getrennte Konzepte bleiben.
15. Task-Group-Cancellation MUSS an aktive Child Tasks und Child Groups propagiert werden.
16. Ein gecancelter Scope DARF NICHT erfolgreich abgeschlossen werden, solange verpflichtende Lifecycle-Arbeit unkontrolliert aktiv bleibt.
17. Kontrollierte Process Termination SOLL zunächst Cancellation des Root Execution Scopes verwenden.
18. Nicht reagierende Tasks MÜSSEN über eine definierte Eskalationspolicy behandelbar sein.
19. Cleanup DARF NICHT unbegrenzt Ressourcen oder Zeit beanspruchen.
20. Cancellation und Capability Revocation MÜSSEN getrennte Mechanismen bleiben.
21. Scopegebundene Capabilities MÜSSEN nach abgeschlossenem Lifecycle revokierbar sein.
22. Cancellation MUSS über IPC-/RPC-Grenzen propagierbar sein können.
23. Remote Cancellation DARF NICHT voraussetzen, dass der Remote Peer jederzeit erreichbar ist.
24. Parent-Constraints wie Security und Data Sovereignty DÜRFEN durch Cancellation NICHT aufgehoben werden.
25. Realtime Scopes MÜSSEN begrenzte Cancellation- und Cleanup-Zeiten definieren können.
26. Deterministic Mode MUSS eine reproduzierbare Cancellation-Semantik ermöglichen.
27. Optionale Arbeit MUSS gezielt für Graceful Degradation gecancelt werden können.
28. Cancellation-Zustand und relevante Propagationsbeziehungen MÜSSEN introspektierbar sein.
29. Unsicherer Forced Abort DARF NICHT der Fail-Safe-Standard sein.
30. Die grundlegende Cancellation Propagation MUSS vollständig ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliche Cancellation-Semantik im gesamten System,
- kontrolliertes Beenden hierarchischer Arbeit,
- weniger verwaiste Tasks,
- saubere Integration mit Deadlines,
- sichereres Prozess-Shutdown,
- bessere Behandlung asynchroner I/O-Operationen,
- Grundlage für Graceful Degradation,
- gute Integration mit Realtime und Deterministic Mode.

### Negative Konsequenzen

- APIs müssen Cancellation bewusst berücksichtigen,
- nicht abbrechbare Operationen benötigen explizite Behandlung,
- Remote- und Hardwareoperationen können Cancellation verzögern,
- Cleanup- und Eskalationsregeln müssen sauber definiert werden.

---

## Verworfene Alternativen

### Cancellation nur als Thread Kill

Verworfen.

Dies kann Ressourcen und Systemzustände inkonsistent hinterlassen.

### Keine automatische Parent-zu-Child-Propagation

Verworfen.

Dies würde den wesentlichen Lifecycle-Vorteil von Structured Concurrency aufheben.

### Child Cancellation cancelt immer die gesamte Hierarchie

Verworfen.

Lokale Cancellation muss möglich bleiben.

### Unbegrenztes Warten auf cooperative Cancellation

Verworfen.

Fehlerhafte oder blockierte Tasks könnten sonst Shutdown und Recovery dauerhaft verhindern.

### Cancellation und Capability Revocation gleichsetzen

Verworfen.

Kontrolliertes Cleanup kann kurzfristig weiterhin definierte Ressourcenrechte benötigen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-CONCURRENCY-0001_Structured_Concurrency`
- `ADR-CONCURRENCY-0002_Task_Groups_mit_hierarchischer_Lebensdauer`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0005_Systemweite_Process_Supervision`
- `ADR-PROCESS-0006_Checkpointbare_Execution_Domains`
- `ADR-PROCESS-0007_Migrierbare_Workloads`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-INTERRUPT-0003_Deferred_Work_statt_langer_Interrupt_Handler`

---

## Zugehörige NPSPECs

- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-CONTEXT-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-PROPAGATION-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-IO-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-ESCALATION-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-IPC-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-TEST-0001`

---

## Ergebnis

NovaOS behandelt Cancellation als hierarchisches Lifecycle-Signal innerhalb von Structured Concurrency.

```text
Parent Scope
      ↓
Cancellation
      ↓
Child Scopes / Task Groups
      ↓
Tasks
      ↓
Cleanup
      ↓
Join
```

Damit kann nebenläufige Arbeit kontrolliert beendet werden, ohne Cancellation mit unsicherem sofortigem Thread-Abbruch gleichzusetzen.

Die zentrale Architekturregel lautet:

```text
Cancellation follows ownership.

Propagate downward.

Clean up safely.

Escalate only when necessary.

Never abandon active work silently.
```