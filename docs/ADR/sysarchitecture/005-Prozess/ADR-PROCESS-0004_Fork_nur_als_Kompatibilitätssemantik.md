# ADR-PROCESS-0004 – Fork nur als Kompatibilitätssemantik

## Status

Angenommen

## Kategorie

Kernel / Prozesse / Kompatibilität / POSIX / Prozessstart / Speicher / ABI

## Kontext

`ADR-PROCESS-0003_Spawn_als_bevorzugtes_natives_Prozessmodell` definiert Spawn als natives Standardmodell für die Prozesserstellung in NovaOS.

Damit ist bereits festgelegt:

```text
Native Process Creation
    ↓
Spawn
```

und nicht:

```text
fork()
   ↓
exec()
```

Trotzdem existiert umfangreiche bestehende Software, insbesondere aus dem POSIX- und Unix-Umfeld, die eine `fork()`-Semantik erwartet.

Typische Erwartungen sind:

- ein Child-Prozess entsteht aus dem Zustand des Parent-Prozesses,
- der virtuelle Adressraum erscheint zunächst logisch kopiert,
- offene Deskriptoren beziehungsweise kompatible Handles werden übernommen,
- bestimmte Prozessattribute werden vererbt,
- Parent und Child setzen an unterschiedlichen Rückgabepunkten fort,
- anschließend wird häufig `exec()` aufgerufen.

NovaOS soll solche Software unterstützen können, ohne die interne Prozessarchitektur nach `fork()` auszurichten.

---

## Entscheidung

NovaOS behandelt `fork()` ausschließlich als **Kompatibilitätssemantik**.

`fork()` ist keine native Prozessprimitive und kein bevorzugter Mechanismus zur Prozesserstellung.

Das Architekturmodell lautet:

```text
POSIX / Compatibility API
          ↓
fork()
          ↓
Compatibility Semantics
          ↓
Native Process / Memory Mechanisms
          ↓
NovaOS
```

Die native Architektur bleibt:

```text
Spawn-first
```

---

## Grundprinzip

```text
Fork is emulated compatibility behavior.

Spawn is the native process model.
```

---

## Keine native Fork-Abhängigkeit

Der NovaOS-Kernel darf nicht so entworfen werden, dass grundlegende Funktionen wie:

- Prozessstart,
- Service Management,
- Treiberstart,
- Sandbox-Erstellung,
- Agent-Erstellung

eine Fork-Semantik voraussetzen.

Alle nativen NovaOS-Komponenten verwenden Spawn oder speziell definierte native Mechanismen.

---

## Fork als Übersetzungsschicht

Eine POSIX-Kompatibilitätsschicht darf einen Aufruf wie:

```text
fork()
```

auf interne NovaOS-Mechanismen abbilden.

Dazu können gehören:

```text
Process Clone
Copy-on-Write
Handle Duplication
Compatibility State Duplication
Runtime Coordination
```

Diese Mechanismen müssen nicht als öffentliche native Standard-API erscheinen.

---

## Semantisches Ziel

Die Implementierung muss die für die jeweilige Kompatibilitätsschicht zugesicherte Fork-Semantik reproduzieren.

Sie muss nicht intern dieselbe Architektur verwenden wie ein klassischer Unix-Kernel.

Entscheidend ist:

```text
observable compatibility
```

nicht:

```text
internal architectural imitation
```

---

## Copy-on-Write

Copy-on-Write ist der bevorzugte Mechanismus für die logische Speicherduplizierung eines Fork-Childs.

Konzeptionell:

```text
Parent Address Space
      ↓
Fork
      ↓
Shared COW Pages
   ┌───────────┴───────────┐
Parent                  Child
```

Erst bei Schreibzugriffen werden betroffene Seiten getrennt.

Damit muss der komplette Adressraum nicht unmittelbar physisch kopiert werden.

---

## COW ist eigenständiger Mechanismus

Copy-on-Write gehört nicht exklusiv zu `fork()`.

Es kann ebenfalls verwendet werden für:

- Snapshots,
- Checkpoints,
- Shared Memory,
- Clone-Operationen,
- Speicheroptimierung.

Die Memory-Architektur darf deshalb nicht speziell um Fork herum gebaut werden.

---

## Multithreaded Prozesse

Fork aus einem multithreaded Prozess ist semantisch problematisch.

Typischerweise setzt nur der aufrufende Thread im Child fort, während Zustände anderer Threads nicht vollständig erhalten bleiben.

Dadurch können im kopierten Prozesszustand beispielsweise verbleiben:

- gesperrte Mutexes,
- Runtime Locks,
- teilweise aktualisierte Datenstrukturen,
- Thread-Pool-Zustände.

NovaOS darf diese Probleme nicht durch eigene zusätzliche implizite Semantik verschärfen.

Die Kompatibilitätsschicht muss die zugesicherte POSIX-Semantik klar einhalten.

---

## Native Empfehlung für Multithreading

Native NovaOS-Anwendungen sollen aus multithreaded Prozessen neue Prozesse über Spawn erzeugen.

```text
Multithreaded Parent
      ↓
Spawn
      ↓
Clean Child
```

Damit wird kein komplexer Runtime-Zustand des Parents dupliziert.

---

## Handle-Semantik

POSIX-Dateideskriptoren und ähnliche Ressourcen können durch die Kompatibilitätsschicht auf native NovaOS-Handles abgebildet werden.

Bei Fork muss die Kompatibilitätsschicht die erforderliche Vererbungssemantik herstellen.

Konzeptionell:

```text
POSIX FD
   ↓
Compatibility Descriptor
   ↓
Native Handle Reference
```

Der native Handle-Mechanismus selbst muss deshalb keine globale Fork-Vererbung voraussetzen.

---

## Capability-Sicherheit

Fork darf keine Authority Amplification erzeugen.

Das Child darf durch die Fork-Kompatibilitätsoperation keine Capabilities erhalten, die im Parent nicht vorhanden beziehungsweise nicht für die entsprechende Semantik delegierbar sind.

Grundregel:

```text
Child Authority
    ⊆
Parent Authority
```

zuzüglich ausdrücklich autorisierter Systemmechanismen.

---

## Capability-Repräsentation

Eine POSIX-Kompatibilitätsschicht kann historische Konzepte wie:

```text
UID
GID
permissions
file descriptors
```

auf native NovaOS-Capabilities und Security Contexts abbilden.

Fork kopiert dabei den kompatiblen sichtbaren Zustand.

Die native Capability-Architektur bleibt jedoch autoritativ.

---

## Security Context

Der Child-Prozess erhält für Fork-Kompatibilität einen entsprechend abgeleiteten Security Context.

Dieser darf nicht stärker sein als durch Parent und Policy zulässig.

Trust und Security Context bleiben getrennt.

---

## Ressourcen

Fork bedeutet nicht, dass physische Ressourcen kostenlos dupliziert werden.

Das Child wird zu einer eigenen Ressourcendomäne entsprechend `ADR-PROCESS-0002`.

Nach erfolgreicher Erzeugung müssen Parent und Child getrennt abrechenbar sein.

---

## Speicher-Accounting

COW-Speicher benötigt eine definierte Accounting-Regel.

Konzeptionell:

```text
Shared COW Page
    ↓
Shared Accounting Policy
```

Nach einer privaten Kopie gehört die neue physische Seite eindeutig dem schreibenden Prozess beziehungsweise dessen Resource Domain.

---

## CPU-Accounting

Nach Fork besitzen Parent und Child getrennte CPU-Accounts.

Der Child darf nicht dauerhaft dem Ressourcenbudget des Parent zugerechnet werden, sofern nicht beide absichtlich in derselben übergeordneten Resource Group liegen.

---

## Resource Limits

Ein Fork darf fehlschlagen, wenn die Erstellung eines neuen Process Domains gegen Ressourcenlimits verstößt.

Beispiele:

```text
ProcessLimitExceeded
MemoryReservationUnavailable
HandleLimitExceeded
ResourcePolicyDenied
```

Kompatibilität bedeutet nicht, Ressourcenbegrenzungen zu ignorieren.

---

## Prozessidentität

Der Child erhält eine neue native Process Identity und eine eigene PID beziehungsweise kompatible Prozess-ID.

Es gilt nicht:

```text
Parent identity copied
```

sondern:

```text
process state derived
+
new process identity
```

---

## Parent-/Child-Beziehung

Fork erzeugt eine Parent-/Child-Beziehung im Kompatibilitätsmodell.

Diese Beziehung darf jedoch native Capability- und Lifecycle-Regeln nicht umgehen.

Sie ist primär:

- Prozesssemantik,
- Wait-/Exit-Beziehung,
- Kompatibilitätsmetadatum.

---

## Ausführungspunkt

Parent und Child setzen nach erfolgreichem Fork an einem logisch entsprechenden Punkt fort.

Die Kompatibilitätsschicht erzeugt die passenden Rückgabewerte.

Typisch:

```text
Parent:
    fork() → child PID

Child:
    fork() → 0
```

Diese Semantik gehört zur Kompatibilitätsschicht und nicht zur nativen NovaOS-Spawn-API.

---

## Registerzustand

Für Fork-Kompatibilität muss der relevante CPU-Ausführungszustand des aufrufenden Threads dupliziert beziehungsweise rekonstruiert werden.

Architekturspezifische Details werden durch die Process-/Thread-/HAL-Ebene gekapselt.

---

## Thread-Semantik

Bei POSIX-kompatiblem Fork aus einem multithreaded Prozess wird im Child grundsätzlich nur die kompatibel vorgeschriebene Thread-Sicht rekonstruiert.

NovaOS darf nicht blind sämtliche Threads klonen.

Ein vollständiger Multi-Thread-Clone wäre eine andere native Operation und müsste separat definiert werden.

---

## Runtime-Koordination

Sprachruntimes dürfen Fork-Hooks besitzen, wenn dies für korrekte Kompatibilität erforderlich ist.

Beispiele:

```text
BeforeFork
AfterForkParent
AfterForkChild
```

Damit können Runtimes:

- Locks,
- Allocatorzustände,
- Threadregistrierung,
- JIT-/GC-Zustände

konsistent behandeln.

Dies ist Runtime-Koordination, keine Kernelpflicht für sämtliche höheren Laufzeitzustände.

---

## `exec()` nach Fork

Ein häufiger Kompatibilitätspfad lautet:

```text
fork()
   ↓
minimal child setup
   ↓
exec()
```

NovaOS darf diesen Ablauf unterstützen.

Intern kann die Kompatibilitätsschicht bestimmte Fälle optimieren.

---

## Fork-Exec-Optimierung

Wenn erkennbar ist, dass ein Child unmittelbar ein anderes Executable starten soll, darf die Kompatibilitätsschicht den Vorgang optimieren.

Beispiel:

```text
fork + exec
```

kann intern näher an:

```text
spawn
```

umgesetzt werden, sofern das beobachtbare Verhalten erhalten bleibt.

Dies ist ausdrücklich erwünscht.

---

## `posix_spawn`

POSIX-`posix_spawn()` lässt sich besonders direkt auf das native Spawn-Modell abbilden.

Konzeptionell:

```text
posix_spawn()
      ↓
Compatibility Translation
      ↓
Nova Spawn
```

Für NovaOS-Kompatibilität ist `posix_spawn()` deshalb der bevorzugte POSIX-nahe Startpfad, wenn Anwendungen ihn verwenden können.

---

## `vfork()`

`vfork()` darf nicht als native Primitive in die NovaOS-Prozessarchitektur eingebaut werden.

Falls eine Kompatibilitätsschicht `vfork()` bereitstellt, muss sie dessen erforderliche beobachtbare Semantik sicher abbilden oder eine sichere kompatible Implementierung verwenden.

Parent- und Child-Adressraum dürfen dabei nicht unkontrolliert zu einer allgemeinen nativen Shared-Address-Space-Semantik führen.

---

## Process Clone

NovaOS darf einen internen oder expliziten nativen Process-Clone-Mechanismus besitzen.

Dieser ist semantisch von Fork getrennt.

```text
Native Clone
    = explicit NovaOS operation

POSIX fork
    = compatibility contract
```

Eine Fork-Implementierung kann Clone verwenden, muss aber nicht.

---

## Kein Fork im Kernel-Core-Design

Subsysteme dürfen nicht voraussetzen:

```text
every process can always be forked
```

Dies ist besonders wichtig für Prozesse mit:

- Accelerator Contexts,
- Device Ownership,
- Realtime Reservations,
- Remote Resources,
- nicht duplizierbaren Capabilities.

Die Kompatibilitätsschicht muss solche Fälle behandeln.

---

## Nicht duplizierbare Ressourcen

Bestimmte native Ressourcen können nicht sinnvoll dupliziert werden.

Beispiele:

```text
exclusive device ownership
hardware queue ownership
certain DMA mappings
secure hardware contexts
some accelerator contexts
```

Die Fork-Kompatibilitätsschicht benötigt dafür explizite Regeln.

Mögliche Semantiken sind:

```text
not inherited
recreated
shared through compatible wrapper
fork rejected
```

abhängig vom zugesicherten Kompatibilitätsprofil.

---

## GPU und NPU

GPU-/NPU-Kontexte werden nicht automatisch als rohe native Hardwarezustände dupliziert.

Eine Runtime oder Kompatibilitätsschicht muss definieren, wie entsprechende APIs nach Fork reagieren.

NovaOS garantiert keine universelle Hardware-Kontextkopie.

---

## Async I/O

Ausstehende native asynchrone Operationen dürfen nicht blind dupliziert werden.

Die Kompatibilitätsschicht muss unterscheiden zwischen:

- Prozesssicht,
- Handle-Sicht,
- tatsächlicher Kerneloperation.

Ein einzelner Hardware-I/O-Vorgang darf nicht unbeabsichtigt zweimal physisch ausgeführt werden, nur weil ein Process State logisch kopiert wurde.

---

## Structured Concurrency

Native Execution Scopes und Child Tasks werden nicht automatisch vollständig als neue unabhängige Ausführungshierarchie dupliziert.

Die Kompatibilitätsschicht rekonstruiert nur die für die Fork-Semantik notwendige Child-Ausführung.

Dies verhindert verwaiste oder doppelt laufende native Tasks.

---

## Deterministic Mode

Fork-Kompatibilität kann innerhalb eines Deterministic Mode erlaubt werden, sofern alle notwendigen Zustände reproduzierbar abbildbar sind.

Nicht deterministisch duplizierbare externe Ressourcen können Fork verhindern oder eine explizite alternative Semantik erfordern.

Hard Determinism darf nicht still abgeschwächt werden.

---

## Data Sovereignty

Fork darf Data-Sovereignty-Regeln nicht umgehen.

Wenn Parent-Speicher oder Handles sensitive Daten repräsentieren, gelten die entsprechenden Regeln ebenfalls für den Child.

Ein neuer Prozess stellt keine automatische neue Freigabe dar.

---

## Trust

Der Child entsteht aus einem bereits laufenden Parent-Prozesszustand.

Sein Trust-Kontext muss nachvollziehbar ableitbar sein.

Ein späteres `exec()` führt eine neue Executable- und Trust-Bewertung des geladenen Artefakts durch.

---

## Audit

Fork-Kompatibilitätsoperationen können auditierbar sein.

Beispiel:

```text
ForkRequested
ForkCreatedChild
ForkDenied
ForkExecOptimized
```

Audit darf jedoch keine unnötige Last im normalen Prozessstartpfad verursachen.

---

## Introspection

Ein durch Fork-Kompatibilität erzeugter Prozess soll als solcher introspektierbar sein können.

Beispiel:

```text
Process:
    4712

Creation Method:
    POSIX Fork Compatibility

Parent:
    4701

Memory:
    Copy-on-Write

Native Model:
    Process Domain
```

Damit bleibt sichtbar, dass Fork eine Kompatibilitätssemantik und keine native Grundprimitive ist.

---

## Fehlerbehandlung

Fork kann typisiert fehlschlagen.

Mögliche interne Gründe:

```text
ResourceUnavailable
ProcessLimitExceeded
UnsupportedResourceState
CompatibilityConstraintViolation
MemoryCloneFailed
SecurityDenied
```

Die Kompatibilitätsschicht übersetzt diese in die erwarteten API-spezifischen Fehlercodes.

---

## Graceful Degradation

Wenn eine optionale Ressource nicht exakt geklont werden kann, darf nur dann eine alternative Semantik verwendet werden, wenn diese mit dem zugesicherten Kompatibilitätsprofil vereinbar ist.

Sonst muss Fork fehlschlagen.

NovaOS darf keine scheinbar erfolgreiche Fork-Operation melden, deren Prozesszustand die versprochene Semantik verletzt.

---

## Fail-Safe Default

Ist unklar, ob eine Ressource oder Capability sicher in den Child übernommen werden kann, wird sie nicht still privilegiert dupliziert.

Wenn ihre Abwesenheit die Kompatibilitätssemantik verletzen würde, muss die Operation fehlschlagen.

---

## Performance

Fork-Kompatibilität soll effizient umgesetzt werden.

Bevorzugte Techniken umfassen:

```text
Copy-on-Write
Lazy Page Duplication
Shared immutable metadata
Deferred copying
Fork-Exec optimization
```

Performanceoptimierungen dürfen die sichtbare Semantik nicht verändern.

---

## Keine KI-Abhängigkeit

Fork-Kompatibilität ist deterministische Prozessinfrastruktur.

Sie muss vollständig ohne KI funktionieren.

KI darf weder entscheiden, welche Capabilities kopiert werden, noch unklare Fork-Zustände eigenmächtig reparieren.

---

## Normative Anforderungen

1. NovaOS MUSS Spawn als natives bevorzugtes Prozessmodell beibehalten.
2. `fork()` DARF ausschließlich als Kompatibilitätssemantik bereitgestellt werden.
3. Native NovaOS-Systemkomponenten DÜRFEN `fork()` NICHT als grundlegende Voraussetzung benötigen.
4. Die Fork-Kompatibilität MUSS auf nativen Process-, Memory- und Capability-Mechanismen aufbauen.
5. Die interne Implementierung MUSS keine klassische Unix-Kernelarchitektur imitieren.
6. Copy-on-Write SOLL für die logische Speicherduplizierung verwendet werden.
7. COW DARF NICHT ausschließlich an Fork gekoppelt sein.
8. Der Child MUSS eine neue native Prozessidentität erhalten.
9. Parent und Child MÜSSEN nach Fork getrennte Ressourcendomänen besitzen.
10. Fork DARF KEINE Authority Amplification erzeugen.
11. Capability-Vererbung MUSS den nativen Security-Regeln entsprechen.
12. Kompatible Handle-/Descriptor-Vererbung MUSS explizit auf native Handles abgebildet werden.
13. Nicht duplizierbare native Ressourcen MÜSSEN explizit behandelt werden.
14. Exklusive Hardwarezustände DÜRFEN NICHT blind kopiert werden.
15. GPU-/NPU-Kontexte DÜRFEN NICHT als universell forkfähig vorausgesetzt werden.
16. Async I/O DARF NICHT durch Fork unbeabsichtigt physisch dupliziert werden.
17. Fork aus multithreaded Prozessen MUSS die zugesicherte Kompatibilitätssemantik einhalten.
18. Native Execution Scopes und Threads DÜRFEN NICHT blind vollständig geklont werden.
19. Runtime-Fork-Hooks MÜSSEN unterstützt werden können.
20. Fork gefolgt von Exec DARF intern in Richtung Spawn optimiert werden, sofern die sichtbare Semantik erhalten bleibt.
21. `posix_spawn()` SOLL möglichst direkt auf Nova Spawn abgebildet werden.
22. `vfork()` DARF NICHT das native Prozessmodell bestimmen.
23. Ressourcenlimits und Admission Control MÜSSEN auch für Fork gelten.
24. Fork MUSS typisiert fehlschlagen können, wenn sichere Kompatibilität nicht hergestellt werden kann.
25. Data-Sovereignty-Regeln MÜSSEN über Fork hinweg erhalten bleiben.
26. Deterministic Mode DARF durch Fork NICHT still abgeschwächt werden.
27. Fork-erzeugte Prozesse SOLLEN introspektierbar als Kompatibilitätsprozesse erkennbar sein.
28. Fork-relevante Sicherheitsereignisse SOLLEN auditierbar sein.
29. Performanceoptimierungen DÜRFEN die beobachtbare Kompatibilitätssemantik NICHT verändern.
30. Die vollständige Fork-Kompatibilität MUSS ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- volle Freiheit für ein modernes natives Prozessmodell,
- POSIX-Kompatibilität bleibt möglich,
- keine systemweite Abhängigkeit von Fork-Semantik,
- Spawn bleibt für moderne multithreaded Anwendungen optimiert,
- Copy-on-Write kann unabhängig als allgemeiner Speichermechanismus entwickelt werden,
- Fork-Exec kann intern effizient auf Spawn abgebildet werden,
- Capabilities und Resource Economy bleiben autoritativ,
- Hardware- und Accelerator-Ressourcen müssen nicht künstlich vollständig forkfähig werden.

### Negative Konsequenzen

- POSIX-Kompatibilität benötigt zusätzliche Übersetzungslogik,
- vollständige Fork-Semantik ist bei komplexen nativen Ressourcen schwierig,
- einige Altanwendungen können besondere Runtime-Unterstützung benötigen,
- Tests müssen sowohl native Spawn- als auch Fork-Kompatibilität separat abdecken.

---

## Verworfene Alternativen

### `fork()` als native Kernelprimitive

Verworfen.

Dies würde NovaOS unnötig an historische Unix-Prozesssemantik binden.

### Native APIs vollständig auf Fork/Exec aufbauen

Verworfen.

Dies widerspricht dem expliziten Spawn-, Capability- und Resource-Domain-Modell.

### Fork vollständig entfernen

Verworfen.

Dadurch würde POSIX- und Unix-Kompatibilität für relevante Software deutlich erschwert.

### Alle Ressourcen automatisch duplizieren

Verworfen.

Viele moderne Ressourcen besitzen keine sinnvolle oder sichere Clone-Semantik.

### Fork nur durch vollständige physische Speicherkopie

Verworfen.

Dies wäre unnötig teuer und würde Copy-on-Write ignorieren.

### Fork-Erfolg auch bei unbekannter Ressourcensemantik erzwingen

Verworfen.

Kompatibilität darf nicht durch stillschweigend inkorrekten Zustand vorgetäuscht werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0003_Spawn_als_bevorzugtes_natives_Prozessmodell`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0019_Evolvable_System_Architecture`

---

## Zugehörige NPSPECs

- `NPSPEC-PROCESS-FORK-COMPAT-0001`
- `NPSPEC-PROCESS-FORK-COW-0001`
- `NPSPEC-PROCESS-FORK-HANDLE-0001`
- `NPSPEC-PROCESS-FORK-CAPABILITY-0001`
- `NPSPEC-PROCESS-FORK-RESOURCE-0001`
- `NPSPEC-PROCESS-FORK-MULTITHREAD-0001`
- `NPSPEC-PROCESS-FORK-RUNTIME-0001`
- `NPSPEC-PROCESS-FORK-EXEC-0001`
- `NPSPEC-PROCESS-POSIX-SPAWN-0001`
- `NPSPEC-PROCESS-VFORK-COMPAT-0001`
- `NPSPEC-PROCESS-FORK-INTROSPECTION-0001`
- `NPSPEC-PROCESS-FORK-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- einfacher POSIX-Fork,
- unterschiedliche Parent-/Child-Rückgabewerte,
- neue Child-PID,
- neue native Domain Identity,
- Copy-on-Write des Adressraums,
- Parent-Schreibzugriff nach Fork,
- Child-Schreibzugriff nach Fork,
- Descriptor-Vererbung,
- eingeschränkte native Handle-Rechte,
- keine Capability Amplification,
- getrenntes CPU-Accounting,
- getrenntes Memory-Accounting,
- Ressourcenlimit beim Fork,
- Fork aus multithreaded Prozess,
- Runtime Before-/After-Fork-Hooks,
- gesperrter Runtime-Lock-Fall,
- ausstehendes Async I/O,
- nicht duplizierbare Device-Ressource,
- GPU-/NPU-Kontext,
- Fork gefolgt von Exec,
- optimierter Fork-Exec-Pfad,
- `posix_spawn()` über Nova Spawn,
- `vfork()`-Kompatibilität,
- Deterministic Mode,
- Data-Sovereignty-Erhaltung,
- Fork-Fehler ohne halbfertigen Child,
- Introspection,
- Audit.

---

## Ergebnis

NovaOS unterstützt Fork dort, wo bestehende Software dessen Semantik erwartet, macht daraus jedoch keine native Architekturvorgabe.

Das Modell lautet:

```text
POSIX Application
      ↓
fork()
      ↓
Compatibility Layer
      ↓
COW + Process Clone Semantics
      ↓
Native NovaOS Process Domain
```

Während native Software weiterhin verwendet:

```text
SpawnRequest
      ↓
Nova Spawn
      ↓
Process Domain
```

Damit erhält NovaOS POSIX-Kompatibilität, ohne die eigenen Prozess-, Speicher-, Capability- und Ressourcenmodelle nach historischen Fork-Annahmen auszurichten.

Die zentrale Architekturregel lautet:

```text
Support fork where compatibility needs it.

Do not design the operating system around it.

Spawn is native.

Fork is translated.
```