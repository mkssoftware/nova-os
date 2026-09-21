# ADR-REALTIME-0007 – Record/Replay

## Status

Angenommen

## Kategorie

Realtime / Determinism / Debugging / Observability

## Kontext

Fehler in Realtime-Systemen sind häufig schwer reproduzierbar.

Das Verhalten kann von Faktoren abhängen wie:

```text
Interrupt Timing
Task Scheduling
External Inputs
I/O Completion
IPC Ordering
Device Events
Concurrent Operations
Clock Values
Randomness
Network Events
```

Ein Fehler kann deshalb nur unter einer bestimmten Ereignisreihenfolge auftreten und bei einem erneuten Start verschwinden.

Klassisches Logging reicht für solche Fälle häufig nicht aus.

NovaOS benötigt deshalb einen Mechanismus, mit dem relevante Execution-Ereignisse aufgezeichnet und später unter kontrollierten Bedingungen reproduziert werden können.

## Entscheidung

NovaOS führt **Record/Replay** als systemweite optionale Execution-Fähigkeit ein.

```text
Execution
    ↓
Record
    ↓
Execution Record
    ↓
Replay
    ↓
Reproduced Execution
```

Record/Replay wird in das bestehende Execution-, Realtime-, Object-, I/O- und Observability-Modell integriert.

Es entsteht kein separates zweites Ausführungsmodell.

## Grundprinzip

Während des Record-Modus werden ausschließlich die für eine spätere Reproduktion notwendigen nichtdeterministischen Einflüsse erfasst.

```text
Deterministic State
        +
Recorded Non-Determinism
        ↓
Replay
```

Ziel ist nicht, jeden CPU-Zustand permanent vollständig aufzuzeichnen.

Stattdessen wird die minimale Menge relevanter Ereignisse erfasst, die benötigt wird, um die definierte Execution reproduzierbar zu machen.

## Record Scope

Record/Replay gilt immer für einen expliziten Scope.

Beispiele:

```text
Capability Invocation
Process
Agent Execution
Realtime Domain
Service
Execution Pipeline
Simulation
```

Ein Record muss eindeutig beschreiben, welcher Scope aufgezeichnet wurde.

## Execution Record

Ein Execution Record enthält mindestens Referenzen auf:

```text
Execution ID
Record ID
Execution Contract
Software Versions
Capability Versions
Configuration Version
Policy Version
Input Versions
Relevant External Events
Timing Information
Determinism Profile
```

Abhängig vom Scope können weitere Informationen erforderlich sein.

## Externe Inputs

Nichtdeterministische externe Inputs müssen für Replay erfassbar sein.

Beispiele:

```text
Keyboard Input
Mouse Input
Sensor Data
Network Packets
Device Events
External Service Responses
```

Während Replay werden diese Inputs nicht automatisch erneut von der realen Außenwelt bezogen.

Stattdessen können die aufgezeichneten Ereignisse eingespeist werden.

```text
Recorded Input
      ↓
Replay Environment
      ↓
Execution
```

## Scheduling Events

Wenn Scheduling-Reihenfolge das Ergebnis beeinflussen kann, müssen relevante Scheduling-Ereignisse erfasst werden können.

Beispiele:

```text
Task Wakeup
Task Dispatch
Preemption
Synchronization Event
IPC Wakeup
```

Nicht jede normale Execution benötigt vollständiges Scheduler Recording.

Der erforderliche Umfang hängt vom Determinism Profile ab.

## Interrupts

Für entsprechende Realtime-Replays können relevante Interrupt-Ereignisse aufgezeichnet werden.

```text
Interrupt Source
Sequence
Relevant Timestamp
Delivered Event
```

Beim Replay wird die relevante Ereignisreihenfolge kontrolliert reproduziert.

## I/O

Record/Replay wird mit Deterministic I/O integriert.

Aufgezeichnet werden können beispielsweise:

```text
I/O Request
Completion
Result
Error
Timeout
Device Event
```

Replay darf reale Hardware nicht unbeabsichtigt erneut verändern.

## Side Effects

Extern sichtbare Aktionen müssen besonders behandelt werden.

Beispiele:

```text
Send Message
Write Device
Delete Object
Control Machine
Perform Payment
Modify Remote Service
```

Ein Replay darf solche Aktionen standardmäßig nicht erneut real ausführen.

```text
Replay
   ↓
External Side Effect
   ↓
Simulate / Suppress / Explicitly Authorize
```

## Replay Isolation

Replay läuft standardmäßig in einer isolierten Umgebung.

```text
Execution Record
      ↓
Replay Sandbox
      ↓
Virtualized Inputs
Controlled Time
Controlled I/O
Suppressed Side Effects
      ↓
Replay Result
```

Damit wird verhindert, dass die Reproduktion eines Fehlers reale Systemzustände unbeabsichtigt verändert.

## Object Versions

NovaOS verwendet stabile Object IDs und Version IDs.

Record/Replay referenziert deshalb bevorzugt:

```text
Object ID
+
Version ID
```

statt lediglich Dateipfade oder Dateinamen zu speichern.

Damit kann exakt bestimmt werden, welche Datenversion Bestandteil der ursprünglichen Execution war.

## Software Versions

Für Functional Replay müssen relevante Softwarekomponenten reproduzierbar identifiziert werden.

Beispiele:

```text
Executable Version
Capability Version
Library Version
Driver Version
Algorithm Version
AI Model Version
Runtime Version
```

Fehlt eine benötigte Version, muss NovaOS dies explizit anzeigen.

## Configuration und Policies

Auch Konfiguration und Policies können das Verhalten beeinflussen.

Deshalb können Records referenzieren:

```text
Configuration Version
Security Policy Version
Scheduling Policy Version
Execution Policy Version
```

Replay bedeutet nicht, dass historische Security Policies automatisch wieder aktiviert werden.

## Security

Replay darf aktuelle Sicherheitsregeln nicht umgehen.

```text
Recorded Authorization
      ≠
Current Authorization
```

Ein früher erlaubter Zugriff erzeugt keine aktuelle Berechtigung.

Historische Entscheidungen können reproduziert oder analysiert werden, ohne ihre damalige Authority erneut real zu vergeben.

## Secrets

Secrets sollen nicht unnötig Bestandteil eines Execution Records werden.

Beispiele:

```text
Passwords
Private Keys
Access Tokens
Session Secrets
Credentials
```

Wenn eine Execution von einer geheimen Operation abhängt, sollen bevorzugt:

```text
References
Controlled Substitutes
Recorded Results
Brokered Replay Operations
```

verwendet werden.

## Privacy

Record/Replay kann umfangreiche Nutzerdaten erfassen.

Deshalb gilt:

```text
Record only what is required
```

Records unterliegen:

```text
Authorization
Data Classification
Retention
Encryption
Deletion
Sovereignty
```

Ein Debugging-Zweck erzeugt keine unbegrenzte Datensammelberechtigung.

## Time

Replay benötigt kontrollierbare Zeitquellen.

```text
Real Clock
      ↓
Record

Recorded / Virtual Clock
      ↓
Replay
```

Eine Execution kann dadurch beim Replay dieselben relevanten Zeitwerte oder Zeitrelationen erhalten.

## Randomness

Nichtdeterministische Zufallswerte müssen entsprechend dem Execution Contract behandelt werden.

Mögliche Verfahren:

```text
Record Random Values
Record Seed
Use Deterministic PRNG
```

Kryptografische Sicherheitsanforderungen dürfen dadurch nicht abgeschwächt werden.

## AI

AI-Ausführungen können Teil eines Records sein.

Für reproduzierbare AI-Ausführung können relevant sein:

```text
Model ID
Model Version
Model Variant
Runtime Version
Provider
Inference Parameters
Seed
Context Version
Input Versions
```

Unterstützt ein AI Provider kein reproduzierbares Replay, kann stattdessen das relevante beobachtbare Ergebnis aufgezeichnet werden.

Dies muss im Record kenntlich gemacht werden.

```text
Re-executed
```

und:

```text
Recorded Result Injected
```

sind unterschiedliche Replay-Semantiken.

## Distributed Execution

Record/Replay kann mehrere Nodes umfassen.

```text
Node A ──┐
Node B ──┼──> Correlated Record
Node C ──┘
```

Dafür müssen relevante Ereignisse über gemeinsame IDs und definierte Ordnungsinformationen korreliert werden.

Eine globale perfekte Uhr ist dafür nicht grundsätzlich erforderlich.

Kausale Beziehungen müssen jedoch rekonstruierbar sein, wenn sie für das Replay relevant sind.

## Record Modes

NovaOS kann verschiedene Record-Modi anbieten.

```text
Minimal
Diagnostic
Deterministic
Full Realtime
```

Beispielsweise:

```text
Minimal
→ Errors + major external events

Diagnostic
→ Additional scheduling and I/O information

Deterministic
→ Inputs required for functional replay

Full Realtime
→ Timing-relevant events for temporal analysis
```

## Realtime Overhead

Recording erzeugt zusätzlichen Aufwand.

```text
Recording
→ CPU
→ Memory
→ Storage
→ I/O
```

Record/Replay darf deshalb eine bestehende Hard-Realtime-Garantie nicht unkontrolliert verändern.

Für Hard Realtime muss Recording entweder:

```text
Pre-budgeted
Bounded
Off Critical Path
Hardware Assisted
```

sein oder deaktiviert werden.

## Record Buffers

Realtime-Aufzeichnung soll bevorzugt vorreservierte Buffer verwenden.

```text
Realtime Execution
      ↓
Preallocated Record Buffer
      ↓
Deferred Persistence
```

Unbounded Allocation oder synchrones langsames Storage-I/O ist auf kritischen Realtime-Pfaden zu vermeiden.

## Buffer Overflow

Kann ein Record Buffer nicht alle Ereignisse aufnehmen, darf NovaOS nicht behaupten, der Record sei vollständig.

```text
Record Buffer Overflow
        ↓
Record = Incomplete
```

Der Zustand muss explizit markiert werden.

## Replay Fidelity

NovaOS unterscheidet die Qualität eines Replays.

Beispielsweise:

```text
Behavioral Replay
Functional Replay
Temporal Replay
```

### Behavioral Replay

Reproduziert relevante beobachtbare Ereignisse.

### Functional Replay

Zielt auf dasselbe definierte funktionale Ergebnis.

### Temporal Replay

Reproduziert zusätzlich relevante zeitliche Eigenschaften oder Ereignisrelationen.

Ein Functional Replay ist nicht automatisch ein exaktes Temporal Replay.

## Hardware Differences

Replay darf auf anderer Hardware möglich sein, sofern der Record dies erlaubt.

```text
Original Hardware
      ↓
Execution Record
      ↓
Compatible Replay Environment
```

Hardwareabhängige Eigenschaften müssen jedoch sichtbar bleiben.

Ein Replay auf anderer Hardware darf nicht fälschlich als identische Hardware-Reproduktion bezeichnet werden.

## Failure Analysis

Record/Replay soll insbesondere folgende Analyse ermöglichen:

```text
Race Conditions
Deadline Misses
Unexpected Scheduling
I/O Failures
Agent Failures
Driver Failures
Distributed Ordering Problems
Rare Intermittent Errors
```

Damit ergänzt Record/Replay bestehendes Logging und Tracing.

## Verhältnis zu Auditing

Auditing und Record/Replay erfüllen unterschiedliche Aufgaben.

```text
Audit
→ Was ist passiert?

Trace
→ Wie lief es durch das System?

Record/Replay
→ Kann die relevante Ausführung reproduziert werden?
```

Audit Records sind daher nicht automatisch vollständige Replay Records.

## Deterministic Execution

Record/Replay baut auf Deterministic Execution auf, ersetzt sie aber nicht.

```text
Deterministic Execution
+
Recorded Non-Determinism
=
Replayable Execution
```

Je deterministischer der Execution Path bereits ist, desto weniger zusätzliche Informationen müssen aufgezeichnet werden.

## Observability

Record/Replay integriert sich in NovaOS Observability.

Relevante Metadaten:

```text
Record ID
Execution ID
Replay ID
Record Scope
Record Mode
Completeness
Replay Fidelity
Software Versions
Input Versions
Start / End
Dropped Events
Replay Result
Divergence
```

## Replay Divergence

Weicht eine Replay-Ausführung vom erwarteten Verhalten ab:

```text
Expected Event
      ≠
Observed Replay Event
```

entsteht eine:

```text
Replay Divergence
```

NovaOS muss den frühestmöglichen bekannten Divergenzpunkt identifizierbar machen.

## Normative Anforderungen

1. NovaOS MUSS Record/Replay als optionale Execution-Fähigkeit unterstützen können.
2. Jeder Record MUSS einen expliziten Execution Scope besitzen.
3. Nichtdeterministische Inputs MÜSSEN für deterministisches Replay erfassbar sein.
4. Relevante Object- und Software-Versionen MÜSSEN referenzierbar sein.
5. Relevante Scheduling- und Interrupt-Ereignisse MÜSSEN bei Bedarf erfassbar sein.
6. I/O-Ergebnisse und Completion Events MÜSSEN bei entsprechendem Replay erfassbar sein.
7. Replay DARF externe Side Effects standardmäßig nicht erneut real ausführen.
8. Replay SOLL standardmäßig in einer isolierten Umgebung erfolgen.
9. Recorded Authorization DARF nicht automatisch aktuelle Authorization erzeugen.
10. Aktuelle Hard Security Constraints MÜSSEN auch während Replay gelten.
11. Secrets DÜRFEN nicht unnötig in Execution Records gespeichert werden.
12. Record/Replay MUSS Privacy-, Retention- und Sovereignty-Regeln berücksichtigen.
13. Zeitquellen MÜSSEN für entsprechendes Replay kontrollierbar sein.
14. Nichtdeterministische Randomness MUSS bei Bedarf reproduzierbar erfasst werden können.
15. AI-Replay MUSS zwischen erneuter Model Execution und Injection eines aufgezeichneten Ergebnisses unterscheiden.
16. Distributed Records MÜSSEN relevante Ereignisse systemübergreifend korrelieren können.
17. Recording DARF Hard-Realtime-Garantien nicht unkontrolliert verletzen.
18. Realtime Recording SOLL vorreservierte und bounded Ressourcen verwenden.
19. Ein unvollständiger Record MUSS explizit als unvollständig markiert werden.
20. NovaOS MUSS unterschiedliche Replay-Fidelity-Level unterscheiden können.
21. Functional Replay DARF nicht automatisch als Temporal Replay behandelt werden.
22. Hardwareunterschiede MÜSSEN bei hardwareabhängigen Replays sichtbar bleiben.
23. Record/Replay DARF nicht mit Security Auditing gleichgesetzt werden.
24. Replay Divergence MUSS erkannt und beobachtbar gemacht werden können.
25. Record/Replay DARF keine Hard System Rules überschreiben.
26. Die Korrektheit von NovaOS DARF nicht von aktiviertem Record/Replay abhängen.

## Konsequenzen

### Positive Konsequenzen

- seltene Realtime-Fehler werden wesentlich besser reproduzierbar,
- Race Conditions und Scheduling-Probleme können gezielter analysiert werden,
- I/O- und Interrupt-Sequenzen können kontrolliert nachgestellt werden,
- Object- und Software-Versionierung integriert sich direkt in Debugging,
- verteilte und AI-basierte Executions können nachvollziehbarer reproduziert werden,
- deterministische Komponenten reduzieren den notwendigen Record-Umfang.

### Negative Konsequenzen

- Recording benötigt zusätzliche CPU-, Speicher- und Storage-Ressourcen,
- vollständiges Temporal Replay kann erheblich komplexer als Functional Replay sein,
- Hardware und externe Systeme können exaktes Replay begrenzen,
- sensible Daten erfordern strikte Privacy- und Retention-Regeln.

## Abhängigkeiten

- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
- `ADR-REALTIME-0002_Temporal_Isolation`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`
- `ADR-REALTIME-0005_Deterministic_Execution`
- `ADR-REALTIME-0006_Deterministic_IO`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0007_Determinism_Requirements`
- `ADR-OBJECT-0002`
- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_und_Tracing`
- `ADR-OBSERVABILITY-0002_Distributed_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-AI-0012_Agent_Auditing`
- `ADR-AI-0013_KI_darf_harte_Systemregeln_nicht_überschreiben`

## Zugehörige NPSPECs

- `NPSPEC-REALTIME-RECORD-REPLAY-0001`
- `NPSPEC-REALTIME-EXECUTION-RECORD-0001`
- `NPSPEC-REALTIME-REPLAY-SANDBOX-0001`
- `NPSPEC-REALTIME-REPLAY-IO-0001`
- `NPSPEC-REALTIME-REPLAY-TIME-0001`
- `NPSPEC-REALTIME-REPLAY-DIVERGENCE-0001`

## Ergebnis

Record/Replay ergänzt deterministische Ausführung um die kontrollierte Erfassung unvermeidbarer externer Einflüsse:

```text
Deterministic Execution
          +
Recorded External Events
          +
Recorded Versions
          ↓
     Execution Record
          ↓
    Isolated Replay
          ↓
Reproducible Analysis
```

Der zentrale Grundsatz lautet:

```text
NovaOS versucht nicht,
jeden Zustand permanent
vollständig zu kopieren.

Es kontrolliert den
deterministischen Teil
der Ausführung und zeichnet
die relevanten Quellen
des Nichtdeterminismus auf.

Dadurch kann aus einem
schwer reproduzierbaren
Realtime-Fehler eine
kontrolliert wiederholbare
Execution werden.
```