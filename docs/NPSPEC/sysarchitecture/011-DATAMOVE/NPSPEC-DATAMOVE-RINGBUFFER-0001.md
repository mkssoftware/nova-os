# NPSPEC-DATAMOVE-RINGBUFFER-0001 – Nova Ring Buffer

## Status

Angenommen

## Kategorie

DataMove / Ring Buffer / Streaming / IPC / I/O

## Zweck

NovaOS definiert Ring Buffers als begrenzte, wiederverwendbare Datenstrukturen für kontinuierliche Datenströme zwischen Producer und Consumer.

```text
Producer
   ↓
┌───────────────────────┐
│ A │ B │ C │ D │   │   │
└─────────────┬─────────┘
              ↓
           Consumer

End → Beginning
```

Ring Buffers sollen insbesondere für Streaming, IPC, Logging, Audio, Netzwerk, Treiber und Hardware-Kommunikation effiziente Datenpfade ermöglichen.

## Grundprinzip

Ein Ring Buffer verwendet einen festen oder kontrolliert begrenzten Speicherbereich zyklisch wieder.

```text
Write Position
      ↓
[A][B][C][ ][ ][ ]
 ↑               ↓
 └──── Wrap ─────┘
```

Dabei gilt:

```text
Ring Buffer ≠ Queue Semantics in all cases
Ring Buffer ≠ Synchronization
Ring Buffer ≠ Unlimited Storage
```

Die Kapazität ist explizit begrenzt.

## Buffer-Modell

Ein Ring Buffer besitzt mindestens:

```text
RingBufferID
Capacity
ReadPosition
WritePosition
State
Backing
```

Optional:

```text
ElementSize
ProducerCount
ConsumerCount
Permissions
OverflowPolicy
NUMAPolicy
```

Read- und Write-Positionen müssen unabhängig vom physischen Speicherlayout logisch verwaltet werden.

## Zyklisches Layout

Nach Erreichen des Bufferendes wird am Anfang fortgesetzt.

```text
0                         N
↓                         ↓
[ ][ ][ ][ ][ ][ ][ ][ ][ ]
             ↑
          Write

             ↓

[ ][ ][ ][ ][ ][ ][ ][ ][ ]
 ↑
Write after wrap
```

Der Ring muss zwischen:

```text
Empty
Partially Filled
Full
```

eindeutig unterscheiden können.

## Producer / Consumer

Das grundlegende Modell lautet:

```text
Producer
   ↓
Write
   ↓
Ring Buffer
   ↓
Read
   ↓
Consumer
```

NovaOS darf verschiedene Varianten unterstützen:

```text
SPSC
Single Producer / Single Consumer

MPSC
Multiple Producer / Single Consumer

SPMC
Single Producer / Multiple Consumer

MPMC
Multiple Producer / Multiple Consumer
```

Die gewählte Variante muss explizit definiert sein.

## SPSC

Single-Producer/Single-Consumer-Ringe sollen einen besonders günstigen Datenpfad ermöglichen.

```text
Producer → WritePosition

Consumer → ReadPosition
```

Geeignete Implementierungen dürfen ohne klassische Locks arbeiten, sofern Memory Ordering korrekt eingehalten wird.

## Mehrere Teilnehmer

Bei mehreren Producern oder Consumern muss NovaOS konkurrierende Positionsänderungen sicher koordinieren.

Mögliche Mechanismen:

```text
Atomics
Compare-and-Swap
Ticketing
Reservation
Locks
```

Die konkrete Implementierung darf entsprechend dem Einsatzzweck gewählt werden.

## Reserve / Commit

Für größere oder variable Dateneinträge soll ein zweistufiges Modell unterstützt werden können.

```text
Reserve
   ↓
Write Payload
   ↓
Commit
   ↓
Visible to Consumer
```

Ein Consumer darf unvollständig geschriebene Einträge nicht als gültige Daten behandeln.

## Read / Release

Analog kann der Consumer Daten zunächst erhalten und anschließend freigeben.

```text
Acquire
   ↓
Process
   ↓
Release
```

Dadurch können Daten direkt innerhalb des Ring Buffers verarbeitet werden.

## Zero-Copy

Ring Buffers sollen Zero-Copy-Verarbeitung ermöglichen.

```text
Producer
   ↓
Reserve Region
   ↓
Write directly
   ↓
Commit
   ↓
Consumer reads directly
```

Eine zusätzliche Payload-Kopie soll vermieden werden, wenn Lifetime und Synchronisation dies erlauben.

## Wrap-Around

Ein Datensatz kann das physische Ende des Rings erreichen.

NovaOS darf dies behandeln durch:

```text
Split Entry
Padding
Wrap Marker
Scatter/Gather View
```

Higher-Level-Code soll nicht unnötig von der internen Wrap-Around-Darstellung abhängig sein.

## Overflow

Wenn ein Producer schneller schreibt als der Consumer liest, entsteht:

```text
Ring Full
```

Die Overflow-Policy muss explizit definiert sein.

Mögliche Policies:

```text
Block Producer
Reject Write
Drop Newest
Drop Oldest
Overwrite
Signal Pressure
```

Kritische Daten dürfen nicht stillschweigend überschrieben werden.

## Underflow

Wenn keine Daten verfügbar sind:

```text
Ring Empty
```

kann ein Consumer abhängig vom Vertrag:

```text
Wait
Return Empty
Poll
Yield
Receive Notification
```

verwenden.

Busy-Waiting darf nur bewusst eingesetzt werden.

## Backpressure

Ring Buffers sollen Backpressure unterstützen können.

```text
Consumer slower
      ↓
Fill Level rises
      ↓
Threshold
      ↓
Backpressure
      ↓
Producer adapts
```

Dies kann mit Resource Economy und Scheduling gekoppelt werden.

## Shared Buffer

Ein Ring Buffer darf auf Shared-Buffer-Speicher basieren.

```text
Producer VAS ─┐
              ├→ Shared Ring Buffer
Consumer VAS ─┘
```

Dadurch können IPC-Daten ohne Kernel-Payload-Kopie übertragen werden.

Mappings dürfen unterschiedliche virtuelle Adressen besitzen.

## DMA

Hardware darf Ring Buffers direkt verwenden.

Typische Struktur:

```text
CPU
 ↓
Descriptor Ring
 ↓
Device
 ↓
Completion Ring
 ↓
CPU
```

DMA-Ringe müssen den Anforderungen des jeweiligen Devices entsprechen.

## Descriptor Rings

Ein Ring muss nicht den Payload selbst enthalten.

Er darf stattdessen Descriptoren auf externe Buffer speichern.

```text
Ring
├→ Descriptor → Buffer A
├→ Descriptor → Buffer B
└→ Descriptor → Buffer C
```

Dies ist insbesondere für:

```text
Network
Storage
GPU
Accelerators
```

geeignet.

## Memory Ordering

Producer und Consumer müssen eine definierte Sichtbarkeit der Daten gewährleisten.

Konzeptionell:

```text
Producer:
Write Payload
    ↓
Release
    ↓
Publish WritePosition

Consumer:
Observe WritePosition
    ↓
Acquire
    ↓
Read Payload
```

Die konkreten Atomics und Barriers hängen von Architektur und Ring-Variante ab.

## Cache-Verhalten

Read- und Write-Metadaten sollen so organisiert werden können, dass unnötiges Cache-Line-Sharing vermieden wird.

Insbesondere häufig veränderte:

```text
ReadPosition
WritePosition
Counters
```

sollen bei SMP-Systemen False Sharing vermeiden können.

## NUMA

Producer, Consumer und Ring Buffer können auf unterschiedlichen NUMA-Nodes liegen.

```text
Producer Node 0
      ↓
Ring Node ?
      ↓
Consumer Node 1
```

Die Platzierung soll anhand des erwarteten Datenflusses erfolgen.

Für stark asymmetrische Datenströme kann eine Platzierung nahe am dominanten Teilnehmer sinnvoll sein.

## Realtime

Ring Buffers eignen sich für deterministische Datenpfade, wenn:

```text
Capacity fixed
Memory preallocated
No dynamic allocation
Operations bounded
Overflow policy defined
```

ist.

Hard-Realtime-Ringe dürfen nicht von unbeschränkter dynamischer Speicherallokation abhängig sein.

## Fehlerbehandlung

Ein beschädigter Ringzustand darf nicht zu unkontrolliertem Speicherzugriff führen.

Zu prüfen sind insbesondere:

```text
Invalid Position
Invalid Length
Integer Overflow
Corrupt Descriptor
Invalid State Transition
```

Ungültige Einträge müssen kontrolliert verworfen oder der betroffene Ring isoliert werden können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Capacity
Current Fill Level
Peak Fill Level
Read Position
Write Position
Producer Count
Consumer Count
Dropped Entries
Overflow Events
Underflow Events
Transferred Bytes
Wait Time
```

## Normative Anforderungen

1. NovaOS MUSS begrenzte zyklische Ring Buffers für kontinuierliche Datenströme unterstützen können.
2. Ein Ring Buffer MUSS Empty und Full eindeutig unterscheiden können.
3. Producer DÜRFEN unvollständig geschriebene Daten NICHT für Consumer sichtbar machen.
4. Concurrent Producer und Consumer MÜSSEN entsprechend der gewählten Ring-Variante synchronisiert werden.
5. Memory Ordering MUSS auf SMP-Systemen korrekt eingehalten werden.
6. Die Overflow-Policy MUSS für jeden Ring eindeutig definiert sein.
7. Kritische Daten DÜRFEN NICHT ohne explizite Policy stillschweigend überschrieben werden.
8. Ring Buffers SOLLEN Zero-Copy- und Shared-Buffer-Nutzung unterstützen können.
9. DMA-basierte Ringe MÜSSEN Buffer-Lifetime, Cache-Kohärenz und Device-Ownership berücksichtigen.
10. Hard-Realtime-Ringe SOLLEN mit vorab reserviertem Speicher und begrenzten Operationen arbeiten.
11. Fehlerhafte Positions-, Längen- oder Descriptorwerte DÜRFEN NICHT zu Speicherzugriffen außerhalb des Rings führen.
12. Auslastung, Overflow, Drops und Transferverhalten SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-SYNC-LOCKFREE-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DATAMOVE-0005`

## Ergebnis

```text
              ┌──────────────────────┐
Producer ───→ │   Nova Ring Buffer   │ ───→ Consumer
              │                      │
              │ Write → → →          │
              │ ↑           ↓        │
              │ ← ← ← ← ← ←         │
              └──────────────────────┘
                       ↓
             Zero-Copy / IPC / DMA
```

NovaOS erhält damit eine einheitliche Ring-Buffer-Abstraktion für begrenzte, kontinuierliche und hochperformante Datenströme zwischen Threads, Prozessen, Systemdiensten und Hardwaregeräten.