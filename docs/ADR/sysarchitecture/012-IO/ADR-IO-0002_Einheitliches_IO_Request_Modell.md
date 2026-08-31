# ADR-IO-0002 – Einheitliches I/O Request Modell

## Status

Angenommen

## Kategorie

I/O / Request Model / Kernel Architecture / Device Abstraction

## Kontext

`ADR-IO-0001` definiert Completion-Based Asynchronous I/O als natives I/O-Modell von NovaOS.

Ohne eine gemeinsame Request-Abstraktion würden Storage, Netzwerk, USB, Audio, Grafik, Beschleuniger und andere I/O-Subsysteme jeweils eigene Modelle für Operationen, Buffer, Deadlines, Cancellation und Ressourcenanforderungen entwickeln.

Das würde die Integration mit Structured Concurrency, Data Movement, Resource Economy und Execution Contracts erschweren.

NovaOS benötigt deshalb eine gemeinsame logische Beschreibung für I/O-Anforderungen.

---

## Entscheidung

NovaOS definiert ein **einheitliches I/O Request Modell** für alle geeigneten I/O-Subsysteme.

```text
I/O Request
 ├── Operation
 ├── Target
 ├── Data / Buffers
 ├── Execution Context
 └── Constraints
```

Der Request beschreibt **was** ausgeführt werden soll und unter welchen Bedingungen.

Die konkrete Umsetzung bleibt Aufgabe des zuständigen I/O-Providers.

---

## Grundprinzip

```text
One request model.

Many I/O providers.
```

---

## Gemeinsame Request-Semantik

Ein I/O Request besitzt eine systemweit gemeinsame Grundsemantik.

Je nach Operation können dazu gehören:

- Operation Identity,
- Target Resource,
- Input- und Output-Buffer,
- Execution Scope,
- Deadline,
- Cancellation,
- Resource Budget,
- Security Context,
- provider- oder operationsspezifische Parameter.

Nicht jeder Request muss alle Eigenschaften verwenden.

Die konkrete Datenstruktur wird in den NPSPECs definiert.

---

## Operation und Target

Ein Request trennt die gewünschte Operation von der Zielressource.

```text
Operation
   +
Target
   ↓
I/O Request
```

Beispiele für Operationen können sein:

```text
Read
Write
Send
Receive
Flush
Control
```

Subsysteme dürfen zusätzliche typisierte Operationen definieren, ohne das gemeinsame Request-Modell zu verlassen.

---

## Buffer-Modell

I/O Requests verwenden die gemeinsamen Data-Movement-Abstraktionen von NovaOS.

```text
I/O Request
     ↓
Shared Buffer / Buffer Set
     ↓
Provider
```

Scatter/Gather, DMA und Zero-Copy beziehungsweise Copy Avoidance sollen dadurch ohne separate subsystemabhängige Buffer-Architekturen nutzbar sein.

---

## Execution Context

Ein I/O Request gehört zu einem definierten Execution Context.

Dieser verbindet die Operation insbesondere mit:

- Structured Concurrency,
- Cancellation,
- Deadlines,
- Resource Accounting,
- Security Authority.

Damit bleibt eine I/O-Operation ihrem verursachenden Scope beziehungsweise Execution Domain zuordenbar.

---

## Constraints

I/O Requests können Anforderungen aus einem `Nova.ExecutionContract` übernehmen.

Dabei gilt die allgemeine NovaOS-Priorität:

```text
Hard Constraints
      ↓
Eligible Execution
      ↓
Soft Preferences
      ↓
Optimization
```

Security, Data Sovereignty, Realtime, Determinismus und andere Hard Constraints dürfen durch eine Provider-Auswahl nicht verletzt werden.

---

## Provider-spezifische Erweiterungen

Das gemeinsame Request-Modell darf typisierte Erweiterungen für spezielle Geräteklassen unterstützen.

```text
Generic I/O Request
        +
Typed Extension
        ↓
Specific Provider
```

Solche Erweiterungen dürfen die gemeinsame Lebenszyklus-, Completion-, Security- und Resource-Semantik nicht umgehen.

---

## Request-Lebenszyklus

Das gemeinsame Request-Modell muss mit dem Completion-Modell aus `ADR-IO-0001` zusammenarbeiten.

Architektonisch gilt:

```text
Created
   ↓
Submitted
   ↓
In Flight
   ↓
Completion
```

Die detaillierten Zustände, Übergänge und Race-Conditions werden in den NPSPECs spezifiziert.

---

## Synchrones und asynchrones I/O

Das Request-Modell ist nicht an eine bestimmte Programmierschnittstelle gebunden.

Asynchrone APIs können einen Request einreichen und dessen Completion abwarten.

Synchrone APIs können denselben Mechanismus verwenden und den aufrufenden Kontext bis zur Completion warten lassen.

Damit bleibt die zugrunde liegende I/O-Semantik identisch.

---

## Resource Economy

Jeder I/O Request muss dem verursachenden Resource Context zurechenbar sein.

Dies betrifft insbesondere:

- I/O-Bandbreite,
- Queue-Ressourcen,
- Buffer,
- DMA,
- Pinned Memory,
- Provider-Ressourcen.

Dadurch kann I/O in die systemweite Ressourcenökonomie integriert werden.

---

## Security und Data Sovereignty

Ein I/O Request trägt beziehungsweise referenziert den für seine Ausführung relevanten Security Context.

Die gemeinsame Request-Abstraktion darf keine Berechtigung erzeugen.

```text
Request
   ≠
Authority
```

Der zuständige Provider muss die erforderlichen Authority-, Security- und Data-Sovereignty-Constraints respektieren.

---

## Normative Anforderungen

1. NovaOS MUSS ein systemweit einheitliches I/O Request Modell bereitstellen.
2. Geeignete I/O-Subsysteme SOLLEN dieses gemeinsame Modell verwenden.
3. Ein Request MUSS Operation und Target eindeutig beschreiben können.
4. Requests MÜSSEN die gemeinsamen Data-Movement-Abstraktionen verwenden können.
5. Requests MÜSSEN einem Execution Context beziehungsweise Resource Context zuordenbar sein.
6. Cancellation und Deadlines MÜSSEN in das Request-Modell integrierbar sein.
7. Requests MÜSSEN Security- und Data-Sovereignty-Constraints transportieren beziehungsweise referenzieren können.
8. `Nova.ExecutionContract`-Anforderungen MÜSSEN auf I/O Requests anwendbar sein können.
9. Provider-spezifische Erweiterungen MÜSSEN typisiert und kontrolliert erfolgen.
10. Provider-Erweiterungen DÜRFEN die gemeinsame Security-, Completion- oder Resource-Semantik NICHT umgehen.
11. Das Request-Modell MUSS mit Completion-Based I/O zusammenarbeiten.
12. Synchrones und asynchrones I/O SOLLEN dasselbe grundlegende Request-Modell verwenden.
13. I/O-Ressourcen MÜSSEN der systemweiten Resource Economy zurechenbar sein.
14. Die konkrete Provider-Implementierung DARF NICHT Bestandteil der generischen Request-Semantik sein.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliche I/O-Semantik über Geräteklassen hinweg,
- gemeinsame Integration mit Structured Concurrency,
- gemeinsame Buffer- und Data-Movement-Architektur,
- zentrale Unterstützung für Deadlines und Cancellation,
- einfachere Resource- und Security-Kontrolle,
- neue I/O-Provider können in dasselbe Modell integriert werden.

### Negative Konsequenzen

- das gemeinsame Modell muss ausreichend erweiterbar bleiben,
- spezielle Hardware benötigt teilweise typisierte Erweiterungen,
- Provider müssen generische Requests auf ihre native Hardwaresemantik abbilden.

---

## Verworfene Alternativen

### Eigenes Request-Modell pro Subsystem

Verworfen.

Dies würde redundante und inkompatible I/O-Abstraktionen erzeugen.

### Hardware-Requests direkt als systemweite API verwenden

Verworfen.

Hardwaredeskriptoren und gerätespezifische Queue-Formate sind keine stabile allgemeine I/O-Abstraktion.

### Ein vollständig untypisiertes Universal-Request-Format

Verworfen.

Gerätespezifische Erweiterungen müssen kontrolliert und semantisch typisiert bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-IO-0001_Completion_Based_Asynchronous_IO`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0003_Scatter_Gather_IO`
- `ADR-DATAMOVE-0004_DMA_Based_Transfer`

---

## Zugehörige NPSPECs

- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-REQUEST-TARGET-0001`
- `NPSPEC-IO-REQUEST-BUFFER-0001`
- `NPSPEC-IO-REQUEST-CONTRACT-0001`
- `NPSPEC-IO-REQUEST-EXTENSION-0001`
- `NPSPEC-IO-REQUEST-TEST-0001`

---

## Ergebnis

Alle geeigneten NovaOS-I/O-Subsysteme erhalten eine gemeinsame Request-Semantik:

```text
Application / System Component
             ↓
       Unified I/O Request
             ↓
       I/O Infrastructure
        /      |       \
   Storage   Network   Device
        \      |       /
             ↓
         Completion
```

Die konkrete Hardware bleibt providerabhängig, während Request-, Contract- und Completion-Semantik systemweit einheitlich bleiben.

Die zentrale Architekturregel lautet:

```text
Describe I/O once.

Execute it through the appropriate provider.
```