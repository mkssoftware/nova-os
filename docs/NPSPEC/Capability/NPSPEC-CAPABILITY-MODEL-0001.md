# NPSPEC-CAPABILITY-MODEL-0001: Fähigkeiten-Objektmodell

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0001 bis ADR-CAPABILITY-0015, NPSPEC-CAPABILITY-0001

---

# 1. Zweck

Diese Spezifikation definiert das objektorientierte Modell einer Nova Capability.

Sie beschreibt sämtliche Bestandteile einer Fähigkeit sowie deren Beziehungen.

Das Objektmodell dient als Grundlage für:

- Capability Registry
- Capability Runtime
- Pipeline Runtime
- Package Manager
- SDK
- Compiler
- Visual Designer
- Intent Planner
- Dokumentation

Alle Capabilities müssen diesem Modell entsprechen.

---

# 2. Architekturübersicht

```text
Capability
│
├── Identity
├── Metadata
├── Contract
├── Descriptor
├── Permissions
├── Lifecycle
├── Execution
├── Resources
├── Parameters
├── Inputs
├── Outputs
├── Events
├── Diagnostics
├── Metrics
├── Provenance
├── Security
├── Compatibility
└── Implementations
```

---

# 3. Capability

Eine Capability repräsentiert genau eine fachliche Funktion.

Sie besitzt:

- eine eindeutige Identität
- einen Capability Contract
- mindestens einen Eingang oder Ausgang
- deklarative Metadaten
- mindestens eine Implementierung

Eine Capability beschreibt ausschließlich ihre fachliche Aufgabe.

---

# 4. Identity

Jede Capability besitzt eine unveränderliche Identität.

## Eigenschaften

```text
CapabilityId

Name

Namespace

Version

Author

Organization

Created

Updated
```

Beispiel

```text
nova.image.resize
```

---

# 5. Metadata

Metadaten beschreiben die Capability.

## Eigenschaften

```text
Titel

Beschreibung

Kurzbeschreibung

Kategorie

Unterkategorie

Tags

Sprache

Lizenz

Dokumentation

Icon

Homepage
```

Metadaten besitzen keinen Einfluss auf die Ausführung.

---

# 6. Descriptor

Der Descriptor beschreibt die Capability vollständig.

Er enthält:

- Identity
- Metadata
- Contract
- Permissions
- Execution
- Version
- Ressourcen
- Eigenschaften

Der Descriptor ist deklarativ.

---

# 7. Capability Contract

Der Contract beschreibt ausschließlich die öffentliche Schnittstelle.

Er besteht aus:

```text
Inputs

Outputs

Datentypen

Fehler

Garantien

Version

Eigenschaften
```

Implementierungsdetails gehören nicht zum Contract.

---

# 8. Inputs

Ein Input beschreibt einen Eingang.

Eigenschaften:

```text
Name

Datentyp

Optional

Mehrfachwert

Standardwert

Beschreibung

Validierungsregeln
```

Beispiel

```text
image : RasterImage
```

---

# 9. Outputs

Ein Output beschreibt ein Ergebnis.

Eigenschaften:

```text
Name

Datentyp

Beschreibung

Garantien
```

Beispiel

```text
scaledImage : RasterImage
```

---

# 10. Parameter

Parameter beeinflussen das Verhalten einer Capability.

Eigenschaften

```text
Name

Datentyp

Default

Min

Max

Enumeration

Beschreibung
```

Beispiel

```text
width = 1024

height = 768

algorithm = Lanczos
```

---

# 11. Datentypen

Alle Ports verwenden typisierte Daten.

Beispiele

```text
Image

Audio

Video

Document

Text

Tensor

Matrix

Point

Mesh

File

Directory

URL

Model

Measurement
```

Eigene Datentypen können registriert werden.

---

# 12. Permissions

Capabilities deklarieren ihre benötigten Berechtigungen.

Beispiele

```text
filesystem.read

filesystem.write

camera

microphone

network

bluetooth

usb

serial

location
```

Die Runtime überprüft sämtliche Berechtigungen vor der Ausführung.

---

# 13. Execution

Execution beschreibt die technischen Eigenschaften einer Implementierung.

Eigenschaften

```text
CPU

GPU

SIMD

NPU

DSP

FPGA

Remote

Cluster

Container

VM
```

Mehrere Ausführungsarten sind zulässig.

---

# 14. Lifecycle

Eine Capability besitzt folgenden Lebenszyklus.

```text
Registriert

↓

Validiert

↓

Geladen

↓

Initialisiert

↓

Bereit

↓

Ausgeführt

↓

Pausiert

↓

Fortgesetzt

↓

Beendet

↓

Freigegeben
```

Alle Zustandswechsel werden durch die Runtime verwaltet.

---

# 15. Resources

Capabilities deklarieren ihren Ressourcenbedarf.

Beispiele

```text
RAM

VRAM

CPU

GPU

Threads

Netzwerk

Dateien

Geräte
```

Diese Angaben unterstützen die Laufzeitplanung.

---

# 16. Events

Capabilities können Ereignisse veröffentlichen.

Beispiele

```text
Started

Progress

Warning

Finished

Cancelled

Error
```

Events besitzen ausschließlich informativen Charakter.

---

# 17. Diagnostics

Diagnosedaten unterstützen Fehlersuche.

Beispiele

```text
Logs

Trace

Stack

Warnings

Performance

Validation
```

Diagnosedaten sind optional.

---

# 18. Metrics

Zur Laufzeit können Metriken erzeugt werden.

Beispiele

```text
Ausführungszeit

CPU-Auslastung

GPU-Auslastung

Speicher

I/O

Netzwerk

Energieverbrauch
```

Diese Daten unterstützen Optimierungen.

---

# 19. Provenance

Eine Capability kann Informationen über ihre Ausführung bereitstellen.

Beispiele

```text
Version

Parameter

Hardware

Zeitpunkt

Pipeline

Benutzer

Implementierung

Ausführungsort
```

Dadurch werden reproduzierbare Ausführungen unterstützt.

---

# 20. Security

Sicherheitsinformationen beschreiben:

```text
Sandbox

Isolation

Signatur

Vertrauensstufe

Berechtigungen

Datenschutzklasse
```

Die Runtime setzt diese Eigenschaften durch.

---

# 21. Compatibility

Eine Capability kann Kompatibilitätsinformationen besitzen.

Beispiele

```text
Windows Bridge

Linux Bridge

Android Bridge

Legacy Adapter

Application Host
```

Dadurch können klassische Anwendungen eingebunden werden.

---

# 22. Implementations

Eine Capability besitzt mindestens eine Implementierung.

Mehrere Implementierungen sind zulässig.

Beispiel

```text
Capability

Resize Image

├── CPU

├── SIMD

├── Vulkan

├── CUDA

├── OpenCL

└── Remote
```

Alle Implementierungen erfüllen denselben Capability Contract.

---

# 23. Beziehungen

Das Objektmodell besitzt folgende Beziehungen.

```text
Capability

│

├── besitzt genau einen Descriptor

├── besitzt genau einen Contract

├── besitzt mehrere Implementierungen

├── besitzt mehrere Ports

├── besitzt mehrere Parameter

├── besitzt mehrere Berechtigungen

├── kann mehrere Events erzeugen

└── kann Bestandteil beliebig vieler Pipelines sein
```

---

# 24. Objektmodell

```text
Capability

│

├── Identity

├── Metadata

├── Descriptor

├── Contract

│   ├── Inputs

│   ├── Outputs

│   └── Errors

├── Parameters

├── Permissions

├── Lifecycle

├── Execution

├── Resources

├── Metrics

├── Diagnostics

├── Provenance

├── Security

├── Compatibility

└── Implementations
```

---

# 25. YAML-Beispiel

```yaml
id: nova.image.resize

version: 1.0.0

name: Resize Image

category: image

description: Skaliert Rasterbilder.

inputs:
  - image: RasterImage

outputs:
  - image: RasterImage

parameters:
  width: 1024
  height: 768

permissions:
  - filesystem.read
  - filesystem.write

execution:
  - cpu
  - gpu

implementations:
  - cpu
  - vulkan
  - cuda
```

---

# 26. Qualitätsanforderungen

Jede Capability muss:

- deklarativ beschreibbar sein,
- eindeutig identifizierbar sein,
- versioniert werden,
- typisierte Ports besitzen,
- reproduzierbar ausführbar sein,
- isoliert laufen,
- austauschbare Implementierungen unterstützen,
- Berechtigungen deklarieren,
- Metadaten besitzen,
- validierbar sein.

Diese Eigenschaften bilden die Grundlage einer modularen und langfristig wartbaren Capability-Plattform. Die klare Trennung zwischen Schnittstelle, Implementierung und Metadaten entspricht bewährten Architekturprinzipien für nachhaltige Systementwicklung. :contentReference[oaicite:0]{index=0}

---

# 27. Konformitätskriterien

Eine Capability gilt als konform, wenn:

- sie exakt eine fachliche Verantwortung besitzt,
- sie einen vollständigen Descriptor bereitstellt,
- sie einen gültigen Capability Contract besitzt,
- sämtliche Ein- und Ausgänge typisiert sind,
- Berechtigungen vollständig deklariert werden,
- mindestens eine Implementierung existiert,
- mehrere Implementierungen denselben Contract erfüllen,
- sie durch die Runtime validiert und ausgeführt werden kann.

---

# 28. Zusammenfassung

Das Fähigkeiten-Objektmodell definiert die vollständige Struktur einer Nova Capability.

Jede Capability besteht aus einer unveränderlichen Identität, einem deklarativen Descriptor, einem stabilen Capability Contract sowie ergänzenden Informationen zu Parametern, Berechtigungen, Ressourcen, Sicherheit und Implementierungen.

Dieses Modell bildet die gemeinsame Grundlage für Registrierung, Planung, Ausführung, Versionierung und Wiederverwendung aller Fähigkeiten innerhalb der Nova Platform und stellt sicher, dass jede Capability unabhängig von ihrer technischen Umsetzung konsistent beschrieben und verarbeitet werden kann.