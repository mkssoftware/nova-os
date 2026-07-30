# ADR-DATAFLOW-0007: Große Datenobjekte verwenden Zero-Copy- oder Shared-Memory-Mechanismen

- **Status:** Akzeptiert
- **Datum:** 2026-07-27
- **Kategorie:** Data Flow
- **Betrifft:** Nova Runtime, Memory Manager, Pipeline Engine, IPC, Scheduler, GPU Runtime, Object Store
- **Entscheider:** Nova Architecture Board

---

# Kontext

Moderne Anwendungen verarbeiten zunehmend sehr große Datenobjekte.

Beispiele:

- hochauflösende Bilder
- Videos
- KI-Tensoren
- Sprachmodelle
- wissenschaftliche Simulationen
- Punktwolken
- CAD-Modelle
- medizinische Bilddaten
- Robotik-Sensordaten

In klassischen Architekturen werden diese Daten häufig mehrfach zwischen Prozessen oder Komponenten kopiert.

Beispiel:

```text
Kamera
↓
Bild
↓
Capability A
↓
Kopie
↓
Capability B
↓
Kopie
↓
Capability C
```

Jede Kopie verbraucht:

- Speicher
- CPU-Zeit
- Speicherbandbreite
- Energie

Bei mehreren hundert Megabyte oder Gigabyte großen Datenobjekten wird das Kopieren häufig teurer als die eigentliche Verarbeitung.

Zero-Copy- und Shared-Memory-Verfahren reduzieren diesen Overhead erheblich, indem Daten möglichst ohne zusätzliche Speicherkopien zwischen Verarbeitungsschritten weitergegeben werden. Diese Ansätze werden unter anderem in Hochleistungsrechnern, Netzwerktechnik und GPU-Pipelines eingesetzt. ([developer.nvidia.com](https://developer.nvidia.com/blog/efficient-data-transfer-with-zero-copy/))

---

# Problem

Nova ist für:

- KI
- Robotik
- Multimedia
- Wissenschaft
- Industrie
- Simulation
- Cloud
- Edge Computing

konzipiert.

Hier können einzelne Datenobjekte mehrere Gigabyte groß sein.

Mehrfache Kopien führen zu:

- schlechter Skalierbarkeit
- hoher Speicherfragmentierung
- erhöhter Latenz
- unnötigem Energieverbrauch
- reduzierter Parallelität

Besonders problematisch ist dies bei Pipelines mit vielen Verarbeitungsschritten.

---

# Entscheidung

Große Datenobjekte werden innerhalb der Nova Runtime standardmäßig **nicht kopiert**.

Stattdessen nutzt die Runtime bevorzugt:

- Zero-Copy
- Shared Memory
- Memory Mapping
- GPU Buffer Sharing
- DMA
- RDMA (sofern verfügbar)

Erst wenn technische, sicherheitsrelevante oder fachliche Gründe dies erfordern, wird eine physische Kopie erzeugt.

---

# Architektur

```text
Großes Datenobjekt
├── Object ID
├── Shared Memory
├── Reference
├── Version
└── Access Rights
```

Capabilities arbeiten mit Referenzen auf das Datenobjekt.

---

# Definition großer Datenobjekte

Die konkrete Größenklassifizierung ist implementierungsabhängig.

Typische Beispiele:

```text
Bilder
Videos
Audio
Tensoren
Neuronale Netze
3D-Modelle
Simulationen
Datenbanken
Punktwolken
```

Die Runtime entscheidet anhand konfigurierbarer Richtlinien, wann Zero-Copy oder Shared Memory verwendet wird.

---

# Zero-Copy

Zero-Copy bedeutet:

```text
Keine zusätzliche Speicherkopie
```

zwischen zwei Verarbeitungsschritten.

Beispiel:

```text
GPU Buffer
↓
Inference
↓
Renderer
```

Der Speicherbereich bleibt unverändert erhalten.

---

# Shared Memory

Mehrere Capabilities können denselben Speicherbereich gleichzeitig verwenden.

```text
Shared Object
↓
Capability A
↓
Capability B
↓
Capability C
```

Alle greifen auf dieselben Daten zu.

---

# Memory Mapping

Dateibasierte Daten können direkt in den Adressraum eingebunden werden.

```text
Datei
↓
Memory Mapping
↓
Capability
```

Ein separates Einlesen entfällt weitgehend.

---

# GPU Buffer Sharing

GPU-Daten sollen möglichst auf der GPU verbleiben.

Beispiel:

```text
Tensor
↓
Inference
↓
Postprocessing
↓
Rendering
```

Ein Transfer zum Hauptspeicher erfolgt nur bei Bedarf.

---

# DMA-Unterstützung

Hardware, die Direct Memory Access unterstützt, kann Daten direkt übertragen.

Beispiele:

- Netzwerkkarten
- SSDs
- Kameras
- Framegrabber
- Industriehardware

Dadurch wird die CPU entlastet.

---

# RDMA-Unterstützung

In verteilten Umgebungen kann Remote Direct Memory Access genutzt werden.

Dadurch können Daten zwischen Systemen übertragen werden, ohne dass die CPU den gesamten Datentransfer verarbeitet.

Die Nutzung ist optional und hardwareabhängig.

---

# Referenzmodell

Alle Zero-Copy-Mechanismen basieren auf dem Referenzmodell.

Eine Referenz enthält beispielsweise:

```text
Object ID
Version
Speicherort
Zugriffsrechte
Lifetime
```

Die eigentlichen Nutzdaten bleiben unverändert.

---

# Copy-on-Write

Soll ein gemeinsam genutztes Objekt verändert werden,

erzeugt die Runtime automatisch eine neue Objektversion.

```text
Shared Object
↓
Write
↓
Neue Version
```

Dadurch bleiben parallele Leser unbeeinflusst.

---

# Synchronisation

Gemeinsam genutzte Daten benötigen eine kontrollierte Synchronisation.

Unterstützt werden unter anderem:

- atomare Operationen
- Read-Only-Zugriffe
- Copy-on-Write
- Sperrmechanismen
- Versionierung

Die Synchronisation erfolgt durch die Runtime.

---

# Speicherverwaltung

Der Memory Manager verwaltet:

- Referenzzählung
- Lebensdauer
- Speicherorte
- Objektversionen
- Cache
- Freigabe

Capabilities verwalten diese Ressourcen nicht selbst.

---

# Scheduler

Der Scheduler berücksichtigt die Lage großer Datenobjekte.

Beispiel:

```text
GPU
↓
GPU Capability
↓
GPU Capability
```

Anstatt Daten zur CPU zu übertragen, werden geeignete Capabilities möglichst in der Nähe der Daten ausgeführt.

---

# Verteilte Systeme

Auch in Clustern bleibt das Zero-Copy-Prinzip erhalten.

Ist ein Datenobjekt bereits auf einem Zielsystem vorhanden,

wird bevorzugt lediglich die Referenz übertragen.

Nur bei Bedarf erfolgt eine physische Übertragung.

---

# Observability

Die Runtime protokolliert:

- Zero-Copy-Nutzung
- Shared-Memory-Zugriffe
- Copy-on-Write-Ereignisse
- Speicherverbrauch
- Referenzzählung
- Cache-Treffer
- Datenmigrationen

Diese Informationen unterstützen Diagnose und Performanceoptimierung.

---

# Sicherheit

Gemeinsam genutzte Speicherbereiche unterliegen der Sicherheitsarchitektur.

Unterstützt werden:

```text
Read Only
Read Write
Temporary
Persistent
Capability Scoped
```

Unberechtigte Zugriffe werden verhindert.

---

# KI-Workloads

Große KI-Modelle profitieren besonders.

Beispiele:

```text
LLM
CNN
Transformer
Embedding
Tensor
Feature Map
```

Mehrere Verarbeitungsschritte können dieselben Speicherbereiche gemeinsam nutzen.

---

# Robotik

Robotik-Pipelines erzeugen kontinuierlich große Datenmengen.

Beispiele:

```text
Kamera
LiDAR
Radar
IMU
GPS
Tiefensensor
```

Durch Zero-Copy sinkt die Latenz der Verarbeitung erheblich.

---

# Multimedia

Video- und Audiopipelines profitieren von:

- geringer Latenz
- weniger Speicherverbrauch
- direkter GPU-Nutzung
- höherem Durchsatz

Dadurch eignet sich Nova für Echtzeitanwendungen.

---

# Vorteile

- deutlich weniger Speicherkopien
- geringerer Speicherverbrauch
- höhere Performance
- geringere Latenz
- bessere Skalierbarkeit
- geringerer Energieverbrauch
- optimale GPU-Auslastung
- bessere Echtzeitfähigkeit
- ideal für KI-, Multimedia- und Robotikanwendungen

---

# Nachteile

- komplexere Speicherverwaltung
- aufwendigere Synchronisation
- höherer Implementierungsaufwand der Runtime
- Debugging kann anspruchsvoller werden
- hardwareabhängige Optimierungen sind erforderlich

Diese Nachteile werden als akzeptabel bewertet.

---

# Konsequenzen

Nova behandelt große Datenobjekte als gemeinsam nutzbare Ressourcen und vermeidet unnötige Speicherkopien konsequent.

Die Runtime verwendet standardmäßig Zero-Copy-, Shared-Memory-, Memory-Mapping-, GPU-Buffer- und weitere geeignete Mechanismen, sofern dies technisch möglich und sicher ist.

Capabilities arbeiten dabei ausschließlich mit stabilen Referenzen auf Datenobjekte. Änderungen erfolgen über Copy-on-Write und erzeugen neue Objektversionen.

Dieses Architekturprinzip reduziert Speicherverbrauch, Latenz und Energiebedarf erheblich und bildet eine zentrale Grundlage für leistungsfähige KI-, Robotik-, Multimedia-, Industrie- und Wissenschaftsanwendungen innerhalb der Nova Dataflow-Architektur.