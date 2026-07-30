# NPSPEC-DATAFLOW-BUFFER-0001: Pufferung und Flusskontrolle (Teil 1)

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Dokument-ID:** NPSPEC-DATAFLOW-BUFFER-0001
- **Titel:** Pufferung und Flusskontrolle
- **Version:** 1.0.0
- **Status:** Verbindlich
- **Kategorie:** Dataflow Architecture
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform

## Referenzen

- NPSPEC-DATAFLOW-0001
- NPSPEC-DATAFLOW-PORT-0001
- NPSPEC-DATAFLOW-CHANNEL-0001
- NPSPEC-DATATYPE-0001
- NPSPEC-CAPABILITY-CONTRACT-0001
- ADR-DATAFLOW-0001
- ADR-DATAFLOW-0002

---

# 1. Zweck

Diese Spezifikation definiert das standardisierte Buffer- und Flow-Control-System der Nova Platform.

Buffer bilden die temporäre Zwischenspeicherung zwischen Produzenten und Konsumenten.

Sie entkoppeln Verarbeitungsgeschwindigkeiten und ermöglichen einen stabilen Datenfluss auch bei unterschiedlichen Ausführungsgeschwindigkeiten der beteiligten Komponenten. Backpressure und Buffering sind dabei zentrale Bestandteile moderner Dataflow-Architekturen.

---

# 2. Ziele

Das Buffer-System verfolgt folgende Ziele:

- deterministischer Datenfluss
- kontrollierte Speicherverwaltung
- Entkopplung von Produzent und Konsument
- hohe Parallelität
- minimale Latenz
- maximale Auslastung
- Zero-Copy-Unterstützung
- automatische Flusskontrolle
- Vermeidung von Datenverlust
- Echtzeitfähigkeit

---

# 3. Grundprinzipien

Ein Buffer besitzt folgende Eigenschaften:

- eindeutig identifizierbar
- typisiert
- versioniert
- reflektierbar
- unabhängig vom Transport
- unabhängig vom Scheduler
- vollständig überwachbar
- deklarativ konfigurierbar

---

# 4. Architektur

```text
Producer

↓

Output Port

↓

Channel

↓

Buffer

↓

Input Port

↓

Consumer
```

Der Buffer bildet die einzige standardisierte Zwischenspeicherung zwischen zwei Verarbeitungsschritten.

---

# 5. Definition

Ein Buffer speichert Daten zeitlich begrenzt zwischen zwei Verarbeitungsschritten.

Ein Buffer definiert:

- Kapazität
- Reihenfolge
- Speicherstrategie
- Flusskontrolle
- Synchronisation
- QoS
- Priorität

---

# 6. Buffer-ID

Jeder Buffer besitzt:

```text
Buffer ID

Version

Channel ID

Node ID
```

Die Buffer-ID ist global eindeutig.

---

# 7. Bufferzustände

```text
Created

Initialized

Empty

Filling

Ready

Full

Draining

Closed

Error
```

---

# 8. Lebenszyklus

```text
Create

↓

Initialize

↓

Allocate

↓

Fill

↓

Consume

↓

Drain

↓

Release
```

---

# 9. Buffertypen

Nova definiert:

```text
FIFO

LIFO

Ring Buffer

Circular Buffer

Priority Queue

Double Buffer

Triple Buffer

Shared Buffer
```

---

# 10. FIFO

Standardpuffer.

```text
First In

↓

First Out
```

---

# 11. LIFO

Stack-Verhalten.

```text
Last In

↓

First Out
```

---

# 12. Ring Buffer

Kontinuierlicher Ringspeicher.

Eigenschaften:

- konstante Speichergröße
- geringe Latenz
- Echtzeitfähig

---

# 13. Circular Buffer

Ähnlich Ring Buffer.

Zusätzlich unterstützt:

- automatische Überschreibung
- konfigurierbare Strategien

---

# 14. Priority Queue

Elemente werden entsprechend ihrer Priorität entnommen.

---

# 15. Double Buffer

Verwendet zwei Speicherbereiche.

```text
Producer

↓

Buffer A

Consumer

↓

Buffer B
```

Nach Abschluss erfolgt Buffer-Swap.

---

# 16. Triple Buffer

Drei Speicherbereiche.

Besonders geeignet für:

- Rendering
- GPU
- Video
- Echtzeitdarstellung

---

# 17. Shared Buffer

Mehrere Komponenten greifen gemeinsam auf denselben Speicherbereich zu.

Synchronisation erfolgt vollständig durch die Runtime.

---

# 18. Buffergröße

Jeder Buffer definiert:

```text
Minimum

Standard

Maximum
```

Die Runtime darf innerhalb dieser Grenzen optimieren.

---

# 19. Dynamische Buffer

Dynamische Buffer dürfen ihre Größe verändern.

Grenzen werden durch Policies definiert.

---

# 20. Feste Buffer

Feste Buffer besitzen eine unveränderliche Größe.

Sie werden bevorzugt für:

- Echtzeit
- Robotik
- Embedded
- Kernel

---

# 21. Speichermodelle

Unterstützt werden:

```text
Heap

Stack

Shared Memory

GPU Memory

Persistent Memory
```

---

# 22. Speicherzuweisung

Die Runtime entscheidet:

- wann Speicher reserviert wird
- wann Speicher freigegeben wird
- wann Buffer wiederverwendet werden

---

# 23. Buffer Ownership

Ein Buffer besitzt genau einen Besitzer.

Unterstützt:

```text
Runtime

Channel

Node

Application
```

---

# 24. Referenzzählung

Buffer können Referenzzählung verwenden.

Speicher wird automatisch freigegeben sobald keine Referenzen mehr existieren.

---

# 25. Zero Copy

Ein Buffer kann deklarieren:

```text
Zero Copy
```

Dann dürfen unnötige Kopiervorgänge nicht erfolgen.

---

# 26. Copy-on-Write

Mehrere Leser dürfen denselben Buffer verwenden.

Erst bei einer Änderung erfolgt eine Kopie.

---

# 27. Immutable Buffer

Immutable Buffer dürfen nach der Erstellung nicht verändert werden.

---

# 28. Mutable Buffer

Mutable Buffer können verändert werden.

Synchronisation ist verpflichtend.

---

# 29. Thread Safety

Ein Buffer kann deklarieren:

```text
Thread Safe

Single Thread

Actor Bound
```

---

# 30. Flusskontrolle

Flow Control verhindert eine Überlastung langsamer Konsumenten.

Der Datenstrom wird anhand der verfügbaren Bufferkapazität geregelt. Moderne Dataflow-Systeme nutzen hierfür Bufferzustände und Rückmeldemechanismen (Backpressure).

---

# 31. Backpressure

Backpressure signalisiert dem Produzenten:

```text
Langsamer senden.
```

Dadurch werden Bufferüberläufe vermieden.

---

# 32. Backpressure-Strategien

Nova unterstützt:

```text
Block

Drop

Latest

Throttle

Adaptive

Credit Based
```

---

# 33. Block

Der Produzent wartet bis wieder Speicher verfügbar ist.

---

# 34. Drop

Neue Nachrichten werden verworfen.

---

# 35. Latest

Nur der aktuellste Wert bleibt erhalten.

Ideal für:

- Kamerabilder
- Sensordaten
- Live-Anzeigen

---

# 36. Throttle

Der Produzent reduziert automatisch seine Sendegeschwindigkeit.

---

# 37. Adaptive

Die Runtime entscheidet dynamisch anhand von:

- CPU-Auslastung
- Speicher
- Latenz
- QoS
- Priorität

---

# 38. Credit Based

Der Empfänger vergibt Sendeguthaben (Credits).

Nur solange Credits vorhanden sind, darf der Sender weitere Daten übertragen.

Diese Strategie eignet sich besonders für verteilte Systeme mit hoher Latenz.

---

# 39. Buffergrenzen

Ein Buffer besitzt:

```text
Low Watermark

High Watermark

Critical Watermark
```

Diese Schwellwerte steuern die Flusskontrolle.

---

# 40. Low Watermark

Unterhalb dieses Wertes kann der Produzent wieder mit voller Geschwindigkeit senden.

---

# 41. High Watermark

Ab diesem Wert beginnt die Runtime mit Flow-Control-Maßnahmen.

---

# 42. Critical Watermark

Bei Erreichen dieses Grenzwertes müssen sofortige Gegenmaßnahmen erfolgen.

Beispiele:

- Blockieren
- Umleiten
- Priorisieren
- Notfallstrategie

---

# 43. Buffermetriken

Ein Buffer stellt mindestens bereit:

```text
Current Size

Capacity

Utilization

Average Fill Level

Peak Fill Level

Overflow Count
```

---

# 44. Monitoring

Alle Buffer können überwacht werden.

Erfasst werden:

- Auslastung
- Wartezeiten
- Blockierungen
- Überläufe
- Unterläufe

---

# 45. Telemetrie

Buffer dürfen Telemetriedaten veröffentlichen.

Diese sind von den Nutzdaten getrennt.

---

# 46. Logging

Folgende Ereignisse können protokolliert werden:

- Initialisierung
- Resize
- Overflow
- Underflow
- Backpressure
- Freigabe

---

# 47. Zusammenfassung Teil 1

Dieser Teil definiert das Buffer-Modell der Nova Platform. Er beschreibt Buffertypen, Lebenszyklus, Speicherstrategien sowie die Grundlagen der Flusskontrolle einschließlich Backpressure, Watermarks und Zero-Copy-Unterstützung.




# NPSPEC-DATAFLOW-BUFFER-0001: Pufferung und Flusskontrolle (Teil 2)

---

# 48. Scheduler-Integration

Das Buffer-System ist vollständig in den Nova Scheduler integriert.

Der Scheduler berücksichtigt:

- Buffer-Auslastung
- Priorität
- QoS
- Echtzeitanforderungen
- CPU-Auslastung
- Speicherauslastung
- Energieprofil

Der Scheduler darf Bufferinformationen verwenden, um Last gleichmäßig über das System zu verteilen.

---

# 49. Prioritätssteuerung

Buffer können Prioritätsinformationen enthalten.

Unterstützt werden:

```text
Critical

High

Normal

Low

Background
```

Bei Ressourcenknappheit werden höher priorisierte Buffer bevorzugt behandelt.

---

# 50. Quality of Service

Buffer unterstützen QoS-Klassen.

Mindestens:

```text
Realtime

Interactive

Normal

Background

Bulk
```

QoS beeinflusst:

- Scheduler
- Speicherstrategie
- Flusskontrolle
- Wiederherstellung
- Monitoring

---

# 51. Persistente Buffer

Persistente Buffer speichern Daten dauerhaft.

Unterstützte Speicherorte:

```text
RAM

SSD

HDD

Persistent Memory

Distributed Storage
```

Persistente Buffer dürfen nach einem Neustart wiederhergestellt werden.

---

# 52. Flüchtige Buffer

Flüchtige Buffer existieren ausschließlich im Arbeitsspeicher.

Nach einem Neustart gehen sämtliche Inhalte verloren.

Diese Strategie bietet die geringste Latenz.

---

# 53. Wiederherstellung

Ein Buffer kann nach einem Fehler automatisch wiederhergestellt werden.

Unterstützte Verfahren:

- Checkpoint
- Journal
- Snapshot
- Replay

---

# 54. Checkpoints

Die Runtime kann regelmäßig Checkpoints erzeugen.

Ein Checkpoint enthält mindestens:

- Bufferzustand
- Schreibposition
- Leseposition
- Metadaten
- Sequenznummern

---

# 55. Snapshots

Snapshots erzeugen eine konsistente Momentaufnahme des Buffers.

Sie dürfen parallel zum normalen Betrieb erstellt werden.

---

# 56. Replay

Ein Buffer kann aufgezeichnete Daten erneut bereitstellen.

Replay muss erhalten:

- Reihenfolge
- Zeitinformationen
- Sequenznummern
- Integritätsinformationen

---

# 57. Shared Memory Buffer

Mehrere Prozesse dürfen denselben Buffer verwenden.

Die Runtime garantiert:

- Synchronisation
- Speicherkonsistenz
- Thread-Sicherheit
- korrekte Referenzzählung

---

# 58. GPU Buffer

Buffer dürfen vollständig im GPU-Speicher existieren.

Beispiele:

```text
Texture

Tensor

Vertex Buffer

Compute Buffer
```

Ein unnötiger Transfer in den Hauptspeicher soll vermieden werden.

---

# 59. DMA Buffer

Geeignete Hardware darf DMA verwenden.

Vorteile:

- geringere CPU-Last
- höhere Datenrate
- geringere Latenz

---

# 60. NUMA-Unterstützung

Auf NUMA-Systemen soll die Runtime Speicher möglichst lokal zum ausführenden Prozessor reservieren.

Dadurch werden Speicherzugriffe beschleunigt.

---

# 61. Cacheoptimierung

Buffer sollen cachefreundlich aufgebaut werden.

Beispiele:

- zusammenhängender Speicher
- Alignment
- Prefetching
- False-Sharing vermeiden

---

# 62. Speicherkomprimierung

Buffer können Daten komprimiert speichern.

Die Runtime entscheidet anhand von:

- CPU-Auslastung
- Speicherbedarf
- Latenz
- QoS

---

# 63. Speicherfragmentierung

Die Runtime soll Speicherfragmentierung minimieren.

Strategien:

- Pool Allocator
- Slab Allocator
- Arena Allocator
- Recycling

---

# 64. Speicherpools

Mehrere Buffer dürfen gemeinsame Speicherpools verwenden.

Dadurch werden:

- Allokationen reduziert
- Fragmentierung reduziert
- Performance erhöht

---

# 65. Buffer Sharing

Mehrere Channels dürfen denselben Buffer verwenden.

Voraussetzungen:

- kompatible Datentypen
- identische Lebensdauer
- gültige Synchronisation

---

# 66. Reader

Ein Buffer kann mehrere Leser besitzen.

Die Runtime garantiert konsistente Leseoperationen.

---

# 67. Writer

Standardmäßig besitzt ein Buffer genau einen Schreiber.

Mehrere Schreiber müssen ausdrücklich aktiviert werden.

---

# 68. Multi Producer

Mehrere Produzenten dürfen gleichzeitig schreiben.

Die Runtime übernimmt:

- Synchronisation
- Konfliktauflösung
- Reihenfolge

---

# 69. Multi Consumer

Mehrere Konsumenten dürfen denselben Buffer lesen.

Mögliche Strategien:

```text
Broadcast

Round Robin

Work Queue

Load Balanced
```

---

# 70. Discovery

Alle Buffer werden über Discovery auffindbar.

Suchkriterien:

```text
Name

ID

Typ

QoS

Tags

Node

Channel
```

---

# 71. Reflection

Reflection liefert sämtliche Buffereigenschaften.

Mindestens:

```text
Type

Capacity

Current Size

QoS

Strategy

State

Metrics
```

Reflection verändert niemals den Bufferzustand.

---

# 72. Monitoring

Buffer liefern kontinuierliche Laufzeitinformationen.

Beispiele:

- aktuelle Größe
- durchschnittliche Auslastung
- maximale Auslastung
- Blockierungen
- Wartezeiten
- Überläufe

---

# 73. Telemetrie

Telemetriedaten werden getrennt von den eigentlichen Nutzdaten übertragen.

Sie dienen ausschließlich Diagnose und Optimierung.

---

# 74. Logging

Folgende Ereignisse können protokolliert werden:

- Erstellung
- Resize
- Overflow
- Underflow
- Backpressure
- Fehler
- Freigabe

---

# 75. Fehlertypen

Ein Buffer kann melden:

```text
Overflow

Underflow

AllocationFailed

Timeout

Corruption

PolicyViolation
```

---

# 76. Fehlerbehandlung

Die Runtime unterstützt:

- Retry
- Resize
- Umleitung
- Priorisierung
- Notfallstrategie
- kontrolliertes Verwerfen

---

# 77. Sicherheitsmodell

Ein Buffer kann Sicherheitsattribute besitzen.

Beispiele:

```text
Encrypted

Authenticated

Signed

Trusted

Readonly
```

---

# 78. Verschlüsselung

Persistente Buffer können verschlüsselt gespeichert werden.

Die Schlüsselverwaltung erfolgt über das Nova-Sicherheitssystem.

---

# 79. Integrität

Buffer können Integritätsinformationen speichern.

Beispiele:

- Hash
- Prüfsumme
- Signatur

---

# 80. Datenschutz

Ein Buffer kann deklarieren:

```text
Contains Personal Data

Contains Sensitive Data

Temporary Only

Do Not Persist
```

Diese Informationen werden von Runtime und Scheduler berücksichtigt.

---

# 81. YAML-Beispiel

```yaml
buffer:

  id: vision.framebuffer

  type: RingBuffer

  capacity: 256

  qos: Realtime

  zero_copy: true
```

---

# 82. YAML-Watermarks

```yaml
watermarks:

  low: 20%

  high: 80%

  critical: 95%
```

---

# 83. YAML-Backpressure

```yaml
backpressure:

  strategy: Adaptive

  enabled: true

  priority: High
```

---

# 84. YAML-Persistenz

```yaml
storage:

  persistent: true

  backend: SSD

  encryption: true
```

---

# 85. Compiler-Anforderungen

Der Compiler muss prüfen:

- Bufferdefinitionen
- Datentypen
- Größenangaben
- QoS
- Richtlinien
- Referenzen
- Reflection-Metadaten

Ungültige Definitionen sind Compile-Fehler.

---

# 86. Runtime-Anforderungen

Die Runtime muss:

- Buffer erzeugen
- Speicher reservieren
- Flusskontrolle anwenden
- Backpressure berechnen
- Monitoring bereitstellen
- Wiederherstellung unterstützen
- Speicher freigeben

---

# 87. Testanforderungen

Jeder Buffer benötigt Tests für:

- Überlauf
- Unterlauf
- Resize
- Zero-Copy
- Persistenz
- Replay
- Recovery
- Reflection
- Discovery
- Performance

---

# 88. Performance-Anforderungen

Das Buffer-System muss:

- lockarm oder lockfrei implementierbar sein
- hochgradig parallelisierbar sein
- deterministisch arbeiten
- Zero-Copy unterstützen
- Shared Memory unterstützen
- Cacheoptimierungen ermöglichen

---

# 89. Konformitätsstufen

Nova definiert:

```text
Core

Standard

Complete
```

---

# 90. Core

Unterstützt:

- FIFO
- Ring Buffer
- Watermarks
- Reflection
- Monitoring

---

# 91. Standard

Zusätzlich:

- Prioritätswarteschlangen
- Shared Memory
- Persistenz
- Replay
- automatische Flusskontrolle

---

# 92. Complete

Zusätzlich:

- GPU Buffer
- DMA
- NUMA
- adaptive Backpressure
- Snapshot
- Checkpoint
- vollständige Telemetrie

---

# 93. Zusammenspiel mit anderen Spezifikationen

Das Buffer-System arbeitet unmittelbar mit folgenden Spezifikationen zusammen:

- NPSPEC-DATAFLOW-PORT-0001
- NPSPEC-DATAFLOW-CHANNEL-0001
- NPSPEC-DATAFLOW-NODE-0001
- NPSPEC-DATATYPE-COMPATIBILITY-0001
- NPSPEC-CAPABILITY-CONTRACT-0001

Ports definieren Schnittstellen, Channels transportieren Daten und Buffer gleichen unterschiedliche Verarbeitungsgeschwindigkeiten aus.

---

# 94. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation, wenn:

- Buffer eindeutig identifizierbar sind,
- Flusskontrolle deterministisch arbeitet,
- Backpressure unterstützt wird,
- Watermarks korrekt ausgewertet werden,
- Reflection sämtliche Eigenschaften bereitstellt,
- Discovery alle veröffentlichten Buffer findet,
- Sicherheitsrichtlinien eingehalten werden,
- Speicherverwaltung reproduzierbar arbeitet,
- Bufferzustände jederzeit konsistent sind.

---

# 95. Zusammenfassung

Das Buffer-System bildet die zentrale Entkopplungsschicht der Nova-Dataflow-Architektur.

Durch standardisierte Buffer, intelligente Flusskontrolle, adaptive Backpressure-Strategien, Zero-Copy-Unterstützung, Shared Memory, Persistenz sowie umfassende Monitoring-, Reflection- und Sicherheitsmechanismen entsteht ein leistungsfähiges und deterministisches Puffersystem für klassische Anwendungen, KI-Pipelines, Robotik, Echtzeitsysteme und verteilte Cluster.

Gemeinsam mit Nodes, Ports und Channels stellt das Buffer-System sicher, dass Datenströme unabhängig von Ausführungsort, Verarbeitungsgeschwindigkeit und Lastzustand zuverlässig, effizient und kontrolliert verarbeitet werden können.
