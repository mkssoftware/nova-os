# ADR-DATAFLOW-0003: Daten werden bevorzugt per Referenz statt durch vollständige Kopien übertragen

- **Status:** Akzeptiert
- **Datum:** 2026-07-27
- **Kategorie:** Data Flow
- **Betrifft:** Nova Runtime, Capability Runtime, Memory Manager, Pipeline Engine, IPC, Scheduler
- **Entscheider:** Nova Architecture Board

---

# Kontext

Moderne Softwaresysteme verarbeiten zunehmend große Datenmengen.

Beispiele:

- Bilder
- Videos
- KI-Tensoren
- Punktwolken
- 3D-Modelle
- Audiodaten
- große Dokumente
- wissenschaftliche Datensätze

In klassischen Architekturen werden diese Daten häufig zwischen Komponenten vollständig kopiert.

Beispiel:

```text
Capability A
↓
100 MB Bild
↓
Capability B
↓
100 MB Bild
↓
Capability C
```

Jeder Übergang erzeugt eine vollständige Kopie.

Dies verursacht:

- unnötigen Speicherverbrauch
- hohe Speicherbandbreite
- zusätzliche CPU-Zeit
- höhere Latenz
- schlechtere Energieeffizienz
- unnötige Garbage-Collection
- schlechtere Skalierbarkeit

Speicherkopien großer Datenobjekte sind häufig einer der größten Leistungs- und Energieverbrauchsfaktoren moderner Anwendungen. Verfahren wie Zero-Copy oder Shared Memory reduzieren diesen Aufwand erheblich. ([developer.nvidia.com](https://developer.nvidia.com/blog/efficient-data-transfer-with-zero-copy/?utm_source=chatgpt.com))

---

# Problem

Capabilities arbeiten häufig mit unveränderlichen Daten.

Beispiel:

```text
Bild
↓
OCR
↓
Objekterkennung
↓
Farbanalyse
↓
Thumbnail
```

Alle vier Capabilities benötigen dasselbe Bild.

Vier vollständige Kopien wären ineffizient.

Ebenso entstehen Probleme bei:

- GPU-Speicher
- KI-Tensoren
- Shared Memory
- Netzwerkausführung
- Pipelines mit vielen Verarbeitungsschritten

---

# Entscheidung

Nova verwendet standardmäßig ein **Reference-First-Modell**.

Daten werden bevorzugt nicht kopiert.

Stattdessen werden Referenzen auf dieselben Datenobjekte übertragen.

Nur wenn dies fachlich oder technisch erforderlich ist, werden Kopien erzeugt.

---

# Architektur

```text
Capability A
↓
Reference
↓
Capability B
↓
Reference
↓
Capability C
↓
Reference
↓
Capability D
```

Das eigentliche Datenobjekt existiert nur einmal.

---

# Datenobjekte

Jedes Datenobjekt besitzt eine eindeutige Identität.

Beispiel:

```text
Object ID
Version
Typ
Eigentümer
Größe
Speicherort
```

Capabilities arbeiten mit dieser Objektidentität.

---

# Referenzen

Eine Referenz beschreibt:

```text
Object ID
Version
Read/Write
Lifetime
Location
```

Die Referenz enthält niemals die eigentlichen Nutzdaten.

---

# Immutable by Default

Datenobjekte sind standardmäßig unveränderlich.

Mehrere Capabilities können dieselbe Referenz gleichzeitig verwenden.

Beispiel:

```text
Image
↓
OCR
↓
Object Detection
↓
Histogram
↓
Thumbnail
```

Alle lesen dasselbe Objekt.

---

# Schreibzugriffe

Soll eine Capability Daten verändern,

erzeugt die Runtime automatisch ein neues Objekt.

```text
Image
↓
Resize
↓
New Image
```

Das ursprüngliche Objekt bleibt unverändert.

Dieses Verhalten entspricht dem Copy-on-Write-Prinzip.

---

# Copy-on-Write

Nova verwendet Copy-on-Write für mutable Daten.

```text
Reference
↓
Read
↓
keine Kopie
```

```text
Reference
↓
Write
↓
Neue Objektversion
```

Dadurch entstehen Kopien nur bei tatsächlichen Änderungen.

---

# Objektversionierung

Jede Änderung erzeugt eine neue Version.

```text
Image
↓
Version 1
↓
Version 2
↓
Version 3
```

Frühere Versionen bleiben verfügbar, solange Referenzen darauf existieren.

---

# Speicherverwaltung

Die Runtime verwaltet:

- Referenzzähler
- Lebensdauer
- Objektversionen
- Speicherorte
- Garbage Collection

Capabilities verwalten Speicher nicht selbst.

---

# Zero-Copy

Wenn technisch möglich,

werden Daten vollständig ohne Kopieren übertragen.

Beispiele:

- Shared Memory
- GPU Buffer
- DMA
- Memory Mapping
- RDMA

Die Runtime entscheidet automatisch über die optimale Strategie.

---

# Shared Memory

Mehrere Capabilities dürfen dasselbe Objekt gemeinsam lesen.

```text
Shared Image
↓
Capability A
↓
Capability B
↓
Capability C
```

Dadurch entstehen keine zusätzlichen Kopien.

---

# GPU-Speicher

GPU-Daten verbleiben möglichst auf der GPU.

Beispiel:

```text
GPU Tensor
↓
AI
↓
Filter
↓
Renderer
```

Ein Rücktransfer in den Hauptspeicher erfolgt nur bei Bedarf.

---

# Verteilte Systeme

Auch in verteilten Systemen wird das Referenzmodell verwendet.

Eine Referenz kann auf:

```text
Lokal
Remote
Cluster
Cloud
Edge
```

zeigen.

Die Runtime entscheidet über die tatsächliche Datenübertragung.

---

# Netzwerkoptimierung

Ist das Zielsystem bereits im Besitz des Datenobjekts,

wird ausschließlich die Referenz übertragen.

Andernfalls erfolgt eine einmalige Übertragung des Objekts.

---

# Pipeline Engine

Pipelines arbeiten grundsätzlich mit Referenzen.

```text
Input
↓
Reference
↓
Capability
↓
Reference
↓
Capability
↓
Reference
```

Dadurch bleibt der Speicherverbrauch nahezu konstant.

---

# Lebensdauer

Jede Referenz besitzt eine definierte Lebensdauer.

```text
Erzeugt
↓
Aktiv
↓
Freigegeben
↓
Gelöscht
```

Erst wenn keine gültigen Referenzen mehr existieren,

kann das Datenobjekt entfernt werden.

---

# Sicherheit

Referenzen besitzen Zugriffsrechte.

Beispiele:

```text
Read
Write
Execute
Temporary
Persistent
```

Unberechtigte Zugriffe werden von der Runtime verhindert.

---

# Observability

Die Runtime protokolliert:

- Referenzerzeugung
- Freigaben
- Copy-on-Write
- Objektmigration
- Speicherverbrauch
- Cache-Hits
- Zero-Copy-Nutzung

Diese Informationen unterstützen Diagnose und Optimierung.

---

# KI-Unterstützung

KI-Workloads profitieren besonders.

Beispiele:

- Tensoren
- Embeddings
- Modelle
- Feature Maps
- Trainingsdaten

Mehrere KI-Capabilities können dieselben Daten ohne Kopien verwenden.

---

# Ausnahmefälle

Eine vollständige Kopie wird erzeugt, wenn:

- Isolation erforderlich ist
- Sicherheitsrichtlinien dies verlangen
- Hardware dies erzwingt
- Copy-on-Write ausgelöst wird
- eine persistente Momentaufnahme erstellt wird

Die Runtime entscheidet anhand deklarierter Richtlinien.

---

# Vorteile

- deutlich geringerer Speicherverbrauch
- geringere Speicherbandbreite
- höhere Performance
- geringere Latenz
- bessere Skalierbarkeit
- bessere Energieeffizienz
- optimale GPU-Nutzung
- ideal für KI-Workloads
- weniger Garbage Collection
- effizientere Pipelines

---

# Nachteile

- komplexere Runtime
- aufwendigere Lebensdauerverwaltung
- Referenzzählung verursacht Verwaltungsaufwand
- Debugging kann anspruchsvoller werden
- Copy-on-Write muss konsequent umgesetzt werden

Diese Nachteile werden als akzeptabel bewertet.

---

# Konsequenzen

Nova behandelt Datenobjekte als eigenständige Ressourcen mit stabiler Identität.

Capabilities arbeiten standardmäßig auf Referenzen statt auf vollständigen Kopien.

Nur bei tatsächlichen Änderungen oder expliziten Anforderungen werden neue Objektversionen erzeugt.

Dieses Reference-First-Modell bildet die Grundlage für eine hochperformante Dataflow-Architektur mit Zero-Copy-Unterstützung, effizienter Speicherverwaltung und optimaler Nutzung moderner CPU-, GPU- und Cluster-Hardware.