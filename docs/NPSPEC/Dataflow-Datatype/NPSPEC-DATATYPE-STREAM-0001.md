# NPSPEC-DATATYPE-STREAM-0001: Streaming-Datentypen

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:**
  - NPSPEC-DATATYPE-0001
  - NPSPEC-DATATYPE-PRIMITIVE-0001
  - NPSPEC-DATATYPE-SEMANTIC-0001
  - NPSPEC-DATATYPE-COMPOSITE-0001
  - NPSPEC-DATAFLOW-0001
  - NPSPEC-CAPABILITY-CONTRACT-0001
  - NPSPEC-CAPABILITY-COMPOSITION-0001
  - NPSPEC-CAPABILITY-STATE-0001
  - ADR-DATAFLOW-0002
  - ADR-DATAFLOW-0003
  - ADR-DATAFLOW-0005
  - ADR-DATAFLOW-0006

---

# 1. Zweck

Diese Spezifikation definiert die Streaming-Datentypen der Nova Platform.

Streaming-Datentypen repräsentieren kontinuierlich eintreffende oder erzeugte Daten, deren vollständige Menge zum Zeitpunkt der Verarbeitung nicht bekannt sein muss.

Sie bilden die Grundlage für:

- Echtzeitdaten
- Sensordaten
- Telemetrie
- Audio
- Video
- Netzwerkkommunikation
- Ereignisverarbeitung
- Robotik
- Maschinensteuerung
- KI-Datenpipelines
- kontinuierliche Analyse
- verteilte Datenverarbeitung

Streaming-Daten unterscheiden sich grundlegend von klassischen Collections dadurch, dass sie kontinuierlich erzeugt und verarbeitet werden können, ohne dass die gesamte Datenmenge vorher vollständig vorliegt.

---

# 2. Ziele

Das Streaming-Modell verfolgt folgende Ziele:

- deterministische Streamverarbeitung
- starke Typisierung
- geringe Latenz
- hoher Durchsatz
- Zero-Copy-Unterstützung
- Backpressure-Unterstützung
- vollständige Typinformationen
- sichere Parallelisierung
- reproduzierbare Verarbeitung
- verteilte Ausführung

---

# 3. Grundprinzipien

Streaming-Daten basieren auf folgenden Prinzipien:

- Streams besitzen einen definierten Elementtyp.
- Elemente sind unveränderlich.
- Reihenfolge ist explizit definiert.
- Streams besitzen einen Lebenszyklus.
- Verarbeitung erfolgt inkrementell.
- Streams können unbegrenzt sein.
- Backpressure ist Bestandteil des Modells.
- Zeitinformationen sind typisiert.
- Streamoperationen sind deklarativ.
- Fehlerzustände sind Bestandteil des Typsystems.

---

# 4. Architekturübersicht

```text
Stream<T>
├── Stream ID
├── Element Type
├── Ordering
├── Time Model
├── Window Model
├── Metadata
├── State
├── Provenance
├── Quality
└── Lifecycle
```

---

# 5. Definition

Ein Stream beschreibt eine geordnete Folge typisierter Elemente.

Formal:

```text
Stream<T>
```

Beispiele:

```text
Stream<Temperature>

Stream<AudioFrame>

Stream<VideoFrame>

Stream<RobotPose>

Stream<Event>

Stream<Document>
```

Ein Stream besitzt keinen vollständig bekannten Endzustand.

---

# 6. Elementtyp

Jeder Stream besitzt genau einen logischen Elementtyp.

Beispiele:

```text
Stream<Float64>

Stream<Person>

Stream<Image>

Stream<Measurement>

Stream<Frame>
```

Der Elementtyp bleibt während der gesamten Lebensdauer konstant.

---

# 7. Streamidentität

Persistente Streams besitzen:

```text
Stream ID

Version

Namespace

Semantic Type
```

Die Identität ist unabhängig von:

- Quelle
- Verbindung
- Transportprotokoll
- Puffergröße
- Verarbeitungsknoten

---

# 8. Streamzustände

Ein Stream besitzt folgende Zustände:

```text
Created

Opening

Open

Paused

Draining

Completed

Cancelled

Failed

Disposed
```

Zustandswechsel müssen nachvollziehbar sein.

---

# 9. Streamelement

Jedes Element besitzt mindestens:

```text
Sequence Number

Timestamp

Payload

Metadata

Quality
```

Optional:

```text
Partition

Source

Correlation ID

Priority

Trace ID
```

---

# 10. Reihenfolge

Nova unterstützt:

```text
Strict Ordered

Weak Ordered

Unordered
```

Standard:

```text
Strict Ordered
```

Die gewählte Ordnung beeinflusst Parallelisierung und Wiederholbarkeit.

---

# 11. Sequenznummern

Streams können monotone Sequenznummern besitzen.

Beispiel:

```text
1

2

3

4

5
```

Sequenznummern dienen:

- Wiederaufnahme
- Debugging
- Replay
- Konsistenzprüfung

---

# 12. Zeitmodell

Nova unterstützt:

```text
Event Time

Processing Time

Ingestion Time

Logical Time
```

Das Zeitmodell ist Bestandteil des Streamtyps.

---

# 13. Event Time

Die Event Time beschreibt den Zeitpunkt des tatsächlichen Ereignisses.

Beispiel:

```text
Temperaturmessung

↓

12:00:03.123
```

Nicht:

```text
Empfang:

12:00:07.552
```

---

# 14. Processing Time

Processing Time beschreibt den Zeitpunkt der Verarbeitung.

Sie hängt vom jeweiligen Verarbeitungsknoten ab.

---

# 15. Ingestion Time

Ingestion Time beschreibt den Zeitpunkt des Eingangs in die Nova Runtime.

Sie bleibt nach der Aufnahme unverändert.

---

# 16. Logische Zeit

Logische Zeit wird verwendet für:

- Simulationen
- Replay
- Testumgebungen
- deterministische Berechnungen

Sie ist unabhängig von physikalischer Zeit.

---

# 17. Bounded Streams

Ein Bounded Stream besitzt ein definiertes Ende.

Beispiele:

```text
Video File

CSV Import

Database Export
```

Der Umfang ist vollständig bekannt.

---

# 18. Unbounded Streams

Ein Unbounded Stream besitzt kein bekanntes Ende.

Beispiele:

```text
Sensor

Chat

Robot

WebSocket

Telemetry
```

Die Laufzeit entscheidet über Beendigung oder Unterbrechung. Unbegrenzte Datenströme sind ein zentrales Merkmal moderner Streamverarbeitungssysteme.

---

# 19. End-of-Stream

Ein Streamende wird explizit signalisiert.

```text
EndOfStream
```

Nicht zulässig:

```text
Null

leeres Paket

Timeout
```

Das Ende besitzt einen eigenen Streamzustand.

---

# 20. Chunk

Streams können in Chunks übertragen werden.

```text
Chunk
├── Header
├── Payload
└── Metadata
```

Chunks besitzen keine eigene fachliche Bedeutung.

---

# 21. Frame

Zeitbasierte Streams verwenden Frames.

Beispiele:

```text
AudioFrame

VideoFrame

LidarFrame

RadarFrame
```

Frames besitzen:

- Zeitstempel
- Reihenfolge
- Nutzdaten

---

# 22. Packet

Transportorientierte Streams verwenden Packets.

Ein Packet enthält:

```text
Header

Payload

Checksum

Sequence
```

Transportpakete dürfen nicht mit logischen Streamelementen verwechselt werden.

---

# 23. Window

Nova definiert:

```text
Fixed Window

Sliding Window

Session Window

Tumbling Window

Custom Window
```

Windows gruppieren Streamelemente logisch.

---

# 24. Window Metadata

Ein Window besitzt:

```text
Start

End

Element Count

Watermark

Completeness
```

---

# 25. Watermarks

Watermarks markieren den Fortschritt eines Streams.

Sie dienen:

- Window Closing
- Out-of-Order Detection
- Synchronisation

Watermarks sind typisierte Steuerinformationen.

---

# 26. Out-of-Order

Streams können verspätete Elemente enthalten.

Die Runtime unterstützt:

```text
Accept

Buffer

Reorder

Reject
```

Das Verhalten wird pro Stream deklariert.

---

# 27. Latenz

Jeder Stream kann Latenzinformationen besitzen.

Beispiele:

```text
Source Latency

Transport Latency

Processing Latency

End-to-End Latency
```

---

# 28. Durchsatz

Der Stream kann Metriken liefern.

Beispiele:

```text
Elements/s

Bytes/s

Frames/s

Messages/s
```

---

# 29. Backpressure

Backpressure ist Bestandteil jedes Streams.

Unterstützte Strategien:

```text
Pause

Slow Producer

Buffer

Drop

Priority Scheduling
```

Die Runtime muss Backpressure propagieren.

---

# 30. Buffer

Ein Stream besitzt optionale Puffer.

Eigenschaften:

```text
Capacity

Current Usage

Overflow Policy
```

---

# 31. Overflow

Strategien:

```text
Reject

Drop Oldest

Drop Newest

Compress

Persist
```

Die Strategie ist Bestandteil des Streamvertrags.

---

# 32. Replay

Persistente Streams unterstützen Replay.

Beispiele:

```text
Replay All

Replay From Timestamp

Replay From Sequence

Replay Window
```

---

# 33. Snapshot

Ein Snapshot beschreibt einen Zustand eines Streams.

Er enthält:

```text
Offset

Window

State

Metadata
```

Snapshots ermöglichen Wiederaufnahme.

---

# 34. Checkpoints

Checkpoints dienen:

- Recovery
- Migration
- Skalierung
- Fehlertoleranz

Jeder Checkpoint besitzt eine eindeutige ID.

---

# 35. Streampartitionen

Streams können partitioniert werden.

Beispiele:

```text
Hash

Round Robin

Key

Region

Topic
```

Jede Partition besitzt dieselben Typinformationen.

---

# 36. Streamfusion

Mehrere Streams können vereinigt werden.

```text
Merge

Union

Interleave

Join
```

Alle beteiligten Streams müssen kompatibel sein.

---

# 37. Streamsplit

Ein Stream kann aufgeteilt werden.

Beispiele:

```text
Filter

Branch

Partition

Multicast
```

---

# 38. Broadcast

Ein Stream kann mehreren Empfängern gleichzeitig bereitgestellt werden.

Jeder Empfänger besitzt einen eigenen Lesestatus.

---

# 39. Streamtransformation

Unterstützte Transformationen:

```text
Map

Filter

Reduce

Aggregate

Window

Join

Sort

Sample
```

Transformationen erzeugen neue Streams.

---

# 40. Streamjoin

Streams können verbunden werden.

Beispiele:

```text
Inner Join

Outer Join

Temporal Join

Window Join
```

Die Join-Bedingungen müssen typisiert sein.

---

# 41. Streamaggregation

Unterstützt:

```text
Count

Sum

Average

Median

Maximum

Minimum

Custom Aggregate
```

Aggregation erzeugt neue Streamelemente.

---

# 42. Quality of Service

Ein Stream kann QoS deklarieren.

Beispiele:

```text
Realtime

Near Realtime

Best Effort

Guaranteed Delivery
```

---

# 43. Zuverlässigkeit

Liefergarantien:

```text
At Most Once

At Least Once

Exactly Once
```

Die Garantie ist Bestandteil des Streamvertrags.

---

# 44. Fehler

Fehlerzustände:

```text
Source Error

Decode Error

Timeout

Overflow

Transport Error

Permission Error
```

Fehler dürfen den Stream kontrolliert beenden oder fortsetzen.

---

# 45. Sicherheit

Streams unterstützen:

```text
Authentication

Authorization

Encryption

Integrity

Audit
```

---

# 46. Datenschutz

Streams können personenbezogene Daten enthalten.

Klassifikationen:

```text
PersonalData

SensitiveData

Public

Secret
```

Datenschutzinformationen begleiten jedes Element.

---

# 47. Provenance

Jeder Stream besitzt Provenance.

Beispiele:

```text
Source

Transformations

Capabilities

Operator Graph

Runtime

Version
```

---

# 48. Reflection

Reflection liefert:

```text
Element Type

Time Model

Window Model

Ordering

QoS

Metadata

State
```

---

# 49. Serialisierung

Unterstützt:

```text
Nova Binary

JSON

CBOR

Apache Arrow

Protocol Buffers
```

Streaming-Serialisierung muss inkrementell erfolgen.

---

# 50. YAML-Beispiel

```yaml
stream:
  id: sensor.temperature
  type: Stream<Temperature>
  ordering: strict
  timeModel: eventTime
  bounded: false
  qos: realtime
  replay: true
  watermark:
    enabled: true
  window:
    type: sliding
    size: 10s
```

---

# 51. Validierungsregeln

Ein Stream ist gültig, wenn:

- ein Elementtyp definiert ist,
- das Zeitmodell angegeben ist,
- die Reihenfolge bekannt ist,
- QoS definiert ist,
- der Lebenszyklus gültig ist,
- Reflection verfügbar ist,
- Provenance vorhanden ist.

---

# 52. Qualitätsanforderungen

Streaming-Datentypen müssen:

- deterministisch sein,
- typisiert sein,
- fehlertolerant sein,
- Replay unterstützen,
- Backpressure unterstützen,
- Zero-Copy ermöglichen,
- Streaming-Serialisierung unterstützen,
- verteilbar sein,
- reproduzierbar bleiben.

---

# 53. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation, wenn:

- typisierte Streams unterstützt werden,
- Zeitmodelle implementiert sind,
- Windowing unterstützt wird,
- Watermarks verarbeitet werden,
- Backpressure vorhanden ist,
- Checkpoints unterstützt werden,
- Replay möglich ist,
- Reflection verfügbar ist,
- Provenance erhalten bleibt,
- Streaming-Datentypen mit Capability Contracts kompatibel sind.

---

# 54. Zusammenfassung

Die **Streaming-Datentypen** definieren das universelle Modell für kontinuierliche Datenverarbeitung innerhalb der Nova Platform.

Sie beschreiben typisierte, zeitbezogene und versionierte Datenströme mit expliziten Lebenszyklen, Zeitmodellen, Fensterkonzepten, Watermarks, Backpressure und Wiederaufnahmemechanismen.

Durch die Kombination aus starker Typisierung, deterministischer Verarbeitung, deklarativen Streamoperationen, QoS, Checkpoints und vollständiger Provenance entsteht eine einheitliche Grundlage für Echtzeitverarbeitung, Robotik, IoT, Multimedia, KI-Pipelines und verteilte Datenflüsse der Nova Platform.
```
````
