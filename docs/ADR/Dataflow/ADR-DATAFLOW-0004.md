# ADR-DATAFLOW-0004: Streaming und Batch-Verarbeitung nutzen dasselbe Datenflussmodell

- **Status:** Akzeptiert
- **Datum:** 2026-07-27
- **Kategorie:** Data Flow
- **Betrifft:** Nova Runtime, Pipeline Engine, Capability Runtime, Scheduler, Stream Engine, Batch Engine
- **Entscheider:** Nova Architecture Board

---

# Kontext

Viele Softwaresysteme unterscheiden grundlegend zwischen Batch-Verarbeitung und Streaming.

Typischerweise existieren dafür unterschiedliche:

- APIs
- Laufzeitmodelle
- Scheduler
- Programmierschnittstellen
- Datenmodelle
- Fehlerbehandlungen

Dadurch entstehen doppelte Implementierungen.

Beispiel:

```text
Image Processing API
Streaming API
Realtime API
Batch API
```

Obwohl alle dieselben Daten verarbeiten, unterscheiden sich deren Programmiermodelle erheblich.

Dies führt zu:

- doppeltem Entwicklungsaufwand
- komplexeren Bibliotheken
- geringerer Wiederverwendbarkeit
- schwieriger Wartbarkeit
- unterschiedlichen Capability Contracts

Ein einheitliches Programmiermodell für Batch- und Stream-Verarbeitung reduziert Komplexität und verbessert die Wiederverwendbarkeit von Komponenten. Dieses Prinzip wurde unter anderem durch moderne Datenverarbeitungssysteme etabliert. ([beam.apache.org](https://beam.apache.org/documentation/programming-guide/))

---

# Problem

Viele Capabilities unterscheiden fachlich nicht zwischen:

```text
Ein Bild
```

und

```text
1000 Bilder
```

oder zwischen:

```text
Eine Sensormessung
```

und

```text
Kontinuierlicher Sensordatenstrom
```

Die eigentliche Verarbeitung bleibt identisch.

Unterschiedlich ist lediglich:

- Anzahl
- Reihenfolge
- Zeitbezug
- Scheduling

Ein getrenntes API-Modell würde dieselbe Logik mehrfach implementieren.

---

# Entscheidung

Nova verwendet ein gemeinsames Datenflussmodell für:

- Einzelobjekte
- Batch-Verarbeitung
- kontinuierliche Streams
- endliche Sequenzen
- unendliche Datenströme

Streaming und Batch unterscheiden sich ausschließlich durch ihre Ausführungsstrategie.

Der Capability Contract bleibt identisch.

---

# Architektur

```text
Input
↓
Data Flow
↓
Capability
↓
Data Flow
↓
Capability
↓
Output
```

Die Runtime entscheidet,

ob der Data Flow

- einmalig,
- blockweise,
- kontinuierlich

ausgeführt wird.

---

# Einheitliches Datenmodell

Alle Daten werden als Datenfluss betrachtet.

Ein Datenfluss kann bestehen aus:

```text
0 Elemente
1 Element
N Elemente
Unendlicher Strom
```

Für die Capability existiert kein Unterschied.

---

# Batch

Ein Batch ist ein endlicher Datenstrom.

Beispiel:

```text
100 Bilder
↓
Resize
↓
100 Bilder
```

Der Datenfluss endet nach dem letzten Objekt.

---

# Streaming

Ein Stream ist ein potenziell unbegrenzter Datenstrom.

Beispiel:

```text
Kamera
↓
Frames
↓
Object Detection
↓
Tracking
```

Der Datenfluss bleibt dauerhaft aktiv.

---

# Einzelobjekt

Ein Einzelobjekt ist ein Batch mit genau einem Element.

```text
Datei
↓
Parser
↓
Dokument
```

Es existiert kein separates API.

---

# Capability Contract

Der Capability Contract beschreibt ausschließlich:

```text
Input Type
↓
Output Type
```

Nicht jedoch:

- Batch
- Stream
- Echtzeit
- Offline

Diese Eigenschaften werden separat beschrieben.

---

# Scheduling

Die Runtime entscheidet automatisch:

```text
Einmalig
↓
Batch
↓
Stream
↓
Window
↓
Micro Batch
```

Capabilities bleiben unverändert.

---

# Fenster (Windows)

Streams können in Fenster unterteilt werden.

Unterstützt werden:

```text
Fixed Window
Sliding Window
Session Window
Event Window
Count Window
```

Die Fensterlogik wird von der Runtime verwaltet.

---

# Micro Batch

Ein kontinuierlicher Stream kann intern in kleine Batches zerlegt werden.

```text
Stream
↓
100 Elemente
↓
Capability
↓
100 Ergebnisse
```

Dies verbessert den Durchsatz, ohne den Contract zu verändern.

---

# Parallelisierung

Batch und Stream unterstützen dieselben Parallelisierungsmodelle.

Beispiele:

```text
Data Parallel
Pipeline Parallel
Task Parallel
Partition Parallel
```

Die Runtime entscheidet über die optimale Strategie.

---

# Fehlerbehandlung

Das Fehlermodell bleibt identisch.

Beispiele:

```text
Retry
Fallback
Rollback
Alternative Capability
```

Streaming benötigt keine eigenen Fehlerobjekte.

---

# Zustandsmodell

Stateful und Stateless Capabilities funktionieren gleichermaßen.

Beispiele:

```text
Stateless
↓
Bildfilter
```

```text
Stateful
↓
Objekttracking
```

Die Stream-Verarbeitung verändert das Zustandsmodell nicht.

---

# Backpressure

Die Runtime unterstützt Backpressure.

Mögliche Strategien:

```text
Blockieren
Puffern
Verwerfen
Priorisieren
Drosseln
```

Capabilities müssen Backpressure nicht selbst implementieren.

---

# Reihenfolge

Die Runtime kann unterschiedliche Reihenfolgen garantieren.

Beispiele:

```text
Strict Order
Best Effort
Parallel
Partition Order
```

Die gewünschte Reihenfolge wird deklarativ beschrieben.

---

# Zeitmodell

Streams können verschiedene Zeitmodelle verwenden.

Beispiele:

```text
Processing Time
Event Time
Ingestion Time
```

Das Zeitmodell ist Teil der Pipeline-Konfiguration.

---

# Discovery

Discovery unterscheidet nicht zwischen:

```text
Batch OCR
Stream OCR
```

Es existiert lediglich:

```text
OCR Capability
```

Die Runtime entscheidet über die Ausführungsart.

---

# Observability

Streaming und Batch verwenden dieselben:

- Logs
- Metriken
- Traces
- Events
- Diagnosedaten

Es existiert kein separates Telemetriemodell.

---

# KI-Unterstützung

KI-Capabilities profitieren besonders.

Beispiele:

```text
Bilder
Videos
Audio
Sensorik
Robotik
LLM Tokens
```

Alle können über denselben Capability Contract verarbeitet werden.

---

# Vorteile

- nur ein Programmiermodell
- keine doppelten APIs
- deutlich geringerer Entwicklungsaufwand
- bessere Wiederverwendbarkeit
- identische Contracts
- einheitliche Runtime
- vereinfachte Discovery
- einfachere Dokumentation
- bessere Wartbarkeit

---

# Nachteile

- komplexere Runtime
- Scheduler muss mehrere Ausführungsarten unterstützen
- Stream-spezifische Optimierungen liegen vollständig in der Runtime
- höherer Implementierungsaufwand im Kernsystem

Diese Nachteile werden als akzeptabel bewertet.

---

# Konsequenzen

Nova trennt fachliche Verarbeitung konsequent von der Ausführungsstrategie.

Capabilities beschreiben ausschließlich ihre Eingaben, Ausgaben und fachliche Funktion.

Ob Daten als Einzelobjekt, Batch, Micro Batch oder kontinuierlicher Stream verarbeitet werden, entscheidet ausschließlich die Runtime.

Dadurch entsteht ein einheitliches Datenflussmodell für sämtliche Verarbeitungsformen, das Wiederverwendbarkeit maximiert, doppelte APIs vermeidet und eine konsistente Architektur für klassische Anwendungen, KI-Systeme, Robotik und Echtzeitdatenverarbeitung bereitstellt.