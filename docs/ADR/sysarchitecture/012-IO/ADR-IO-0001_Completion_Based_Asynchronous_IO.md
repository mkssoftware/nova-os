# ADR-IO-0001 – Completion-Based Asynchronous I/O

## Status

Angenommen

## Kategorie

I/O / Asynchronous I/O / Concurrency / Kernel Architecture

## Kontext

I/O-Operationen besitzen häufig deutlich höhere und variablere Latenzen als reine CPU-Operationen. Ein Thread sollte deshalb nicht unnötig blockiert werden, während ein Storage-, Netzwerk-, Geräte- oder anderer I/O-Provider eine Operation ausführt.

Klassische Readiness-Modelle melden primär, dass eine Ressource möglicherweise bereit ist. Für NovaOS passt ein Completion-basiertes Modell besser zur systemweiten Structured Concurrency und zu nativen `async`/`await`-Abstraktionen.

NovaOS benötigt daher ein einheitliches asynchrones I/O-Modell, bei dem eine Operation eingereicht und später deren tatsächlicher Abschluss gemeldet wird.

---

## Entscheidung

NovaOS verwendet **Completion-Based Asynchronous I/O als natives allgemeines I/O-Modell**.

```text
I/O Request
    ↓
Submit
    ↓
Operation in Flight
    ↓
Completion
    ↓
Result / Error
```

Die Completion repräsentiert den Abschluss der eingereichten Operation und nicht lediglich die Bereitschaft einer Ressource.

---

## Grundprinzip

```text
Submit work.

Receive completion.
```

---

## I/O Request

Eine asynchrone I/O-Operation wird als expliziter Request an einen geeigneten I/O-Provider übergeben.

Ein Request kann abhängig von der Operation unter anderem mit folgenden Contracts verbunden sein:

- Buffer,
- Resource Identity,
- Operation,
- Deadline,
- Cancellation Scope,
- Resource Budget.

Die konkrete Request-Struktur wird in den NPSPECs definiert.

---

## Completion

Nach Abschluss erzeugt die I/O-Infrastruktur eine Completion.

Diese beschreibt mindestens den relevanten Abschlusszustand:

```text
Completed
Failed
Cancelled
```

und das zur Operation gehörende Ergebnis.

Eine Completion muss eindeutig der ursprünglichen Operation zugeordnet werden können.

---

## Verhältnis zu async/await

Completion-Based I/O bildet die native Grundlage für höhere asynchrone Sprach- und Runtime-Abstraktionen.

```text
async / await
      ↓
Future / Task
      ↓
I/O Request
      ↓
Completion
```

NovaLang und andere Runtimes müssen dadurch kein eigenes grundlegendes Kernel-I/O-Modell erfinden.

---

## Structured Concurrency

Asynchrone I/O-Operationen gehören zu einem definierten Execution Scope beziehungsweise Task-Kontext.

Dadurch können:

- Lebensdauer,
- Cancellation,
- Deadlines,
- Fehler

mit der systemweiten Structured-Concurrency-Architektur verbunden werden.

Eine Operation darf nicht unbeabsichtigt zu dauerhaft verwaistem Hintergrund-I/O werden.

---

## Cancellation und Deadlines

I/O Requests müssen in die systemweiten Cancellation- und Deadline-Mechanismen integrierbar sein.

Eine Cancellation bedeutet jedoch nicht automatisch, dass eine bereits an Hardware übergebene Operation physisch sofort beendet werden kann.

Das I/O-Modell muss deshalb zwischen dem Cancellation Request und dem tatsächlichen Abschluss der Operation unterscheiden.

---

## Buffer und Data Movement

Asynchrones I/O verwendet die gemeinsamen Data-Movement-Abstraktionen von NovaOS.

Insbesondere sollen:

- Shared Buffers,
- Scatter/Gather I/O,
- DMA,
- Zero-Copy beziehungsweise Copy Avoidance

ohne separates I/O-Speichermodell nutzbar sein.

Ein Buffer muss während der erforderlichen I/O-Lebensdauer gültig und autorisiert bleiben.

---

## Provider-Modell

Unterschiedliche I/O-Systeme dürfen Completion-Based I/O über unterschiedliche Mechanismen implementieren.

Beispiele sind:

```text
Storage
Network
USB
Audio
Graphics
Accelerators
Device Drivers
```

Das gemeinsame Completion-Modell bleibt von der konkreten Hardware- oder Provider-Implementierung getrennt.

---

## Synchrones I/O

Synchrones I/O bleibt als höhere oder kompatible Abstraktion möglich.

Es soll jedoch grundsätzlich auf dem gemeinsamen I/O-Modell aufbauen können:

```text
Synchronous Call
      ↓
Async I/O Request
      ↓
Wait for Completion
      ↓
Return Result
```

Damit benötigt NovaOS keine vollständig getrennten synchronen und asynchronen I/O-Architekturen.

---

## Determinismus und Realtime

Completion-Based I/O muss mit Deterministic Mode und Realtime-Domänen kombinierbar sein.

Deadlines, Ressourcenreservierungen und Provider-Garantien können dabei Bestandteil des jeweiligen Execution Contracts sein.

Adaptive Optimierungen dürfen harte Realtime- oder Determinismusanforderungen nicht verletzen.

---

## Normative Anforderungen

1. NovaOS MUSS Completion-Based Asynchronous I/O als natives allgemeines I/O-Modell verwenden.
2. Eine asynchrone I/O-Operation MUSS als expliziter Request eingereicht werden können.
3. Der tatsächliche Abschluss MUSS durch eine Completion repräsentiert werden.
4. Completion und ursprünglicher Request MÜSSEN eindeutig zuordenbar sein.
5. Fehler und Cancellation MÜSSEN über das Completion-Modell darstellbar sein.
6. Asynchrones I/O MUSS mit Structured Concurrency integrierbar sein.
7. Cancellation und Deadline Propagation MÜSSEN unterstützt werden können.
8. Cancellation DARF NICHT mit garantiert sofortigem physischem Abbruch gleichgesetzt werden.
9. Das I/O-Modell MUSS die gemeinsamen Shared-Buffer- und Data-Movement-Abstraktionen verwenden können.
10. Provider DÜRFEN unterschiedliche interne Completion-Mechanismen verwenden.
11. Das gemeinsame I/O-Modell DARF NICHT von einer bestimmten Geräteklasse abhängig sein.
12. `async`/`await` und vergleichbare Runtime-Abstraktionen SOLLEN auf diesem Modell aufbauen können.
13. Synchrones I/O SOLL auf dem gemeinsamen Completion-basierten Modell aufbauen können.
14. Determinismus-, Realtime-, Security- und Resource-Constraints MÜSSEN erhalten bleiben.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliches asynchrones I/O-Modell,
- weniger blockierte Threads,
- direkte Integration mit `async`/`await`,
- gute Skalierbarkeit bei vielen parallelen I/O-Operationen,
- gemeinsame Grundlage für unterschiedliche Geräteklassen,
- Integration mit Structured Concurrency und Data Movement.

### Negative Konsequenzen

- Provider benötigen Completion-Unterstützung oder entsprechende Adapter,
- Buffer-Lebensdauer muss über asynchrone Operationen hinweg kontrolliert werden,
- Cancellation besitzt bei bereits gestarteter Hardware-I/O komplexere Semantik.

---

## Verworfene Alternativen

### Readiness-Based I/O als natives Hauptmodell

Verworfen.

Readiness beschreibt primär mögliche Ausführbarkeit und nicht den tatsächlichen Abschluss einer Operation.

### Eigenes Async-Modell pro I/O-Subsystem

Verworfen.

Dies würde unterschiedliche Semantiken für Storage, Netzwerk und Geräte erzeugen.

### Synchrones I/O als Kernel-Grundmodell

Verworfen.

Asynchronität würde dadurch unnötig über blockierende Threads emuliert.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-CONCURRENCY-0003_Cancellation_Propagation`
- `ADR-CONCURRENCY-0004_Deadline_Propagation`
- `ADR-CONCURRENCY-0005_Async_Await_als_native_Abstraktion`
- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0003_Scatter_Gather_IO`
- `ADR-DATAMOVE-0004_DMA_Based_Transfer`

---

## Zugehörige NPSPECs

- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-IO-CANCELLATION-0001`
- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-IO-BUFFER-0001`
- `NPSPEC-IO-ASYNC-TEST-0001`

---

## Ergebnis

NovaOS verwendet für asynchrones I/O einen einheitlichen Submit-and-Complete-Datenfluss:

```text
Task
 ↓
I/O Request
 ↓
Provider
 ↓
Completion
 ↓
Task resumes
```

Die zentrale Architekturregel lautet:

```text
I/O is submitted as work
and observed through completion.
```