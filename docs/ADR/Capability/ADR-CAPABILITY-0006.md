# ADR-CAPABILITY-0006: Implementierungen einer Fähigkeit sind austauschbar

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-26
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0001, ADR-CAPABILITY-0003, ADR-CAPABILITY-0005
- **Folgedokumente:** NPSPEC-CAPABILITY-IMPLEMENTATION-0001, NPSPEC-CAPABILITY-REGISTRY-0001, NPSPEC-CAPABILITY-RUNTIME-0001

---

## 1. Kontext

NovaOS trennt konsequent zwischen einer Fähigkeit (Capability) und ihrer konkreten Implementierung.

Eine Fähigkeit beschreibt **was** erledigt werden soll.

Eine Implementierung beschreibt **wie** diese Aufgabe ausgeführt wird.

Dadurch kann dieselbe Fähigkeit durch unterschiedliche Implementierungen bereitgestellt werden, ohne dass andere Komponenten angepasst werden müssen.

Dieses Prinzip ermöglicht langfristige Wartbarkeit, technologische Weiterentwicklung und automatische Laufzeitoptimierungen.

---

## 2. Problemstellung

Wäre eine Fähigkeit fest an eine konkrete Implementierung gebunden, entstünden zahlreiche Nachteile.

### 2.1 Technologische Abhängigkeit

Neue Bibliotheken oder Algorithmen könnten nur durch Änderungen aller abhängigen Komponenten eingeführt werden.

### 2.2 Eingeschränkte Hardwareunterstützung

Unterschiedliche Hardwareplattformen benötigen häufig unterschiedliche Implementierungen.

Beispiele:

- CPU
- GPU
- FPGA
- TPU
- DSP
- Mikrocontroller

Eine feste Implementierung verhindert eine optimale Nutzung der verfügbaren Hardware.

### 2.3 Fehlende Skalierbarkeit

Eine lokale Implementierung eignet sich möglicherweise nicht für einen Cluster oder eine Cloud.

### 2.4 Unterschiedliche Qualitätsziele

Nicht jede Situation stellt dieselben Anforderungen.

Beispiele:

- maximale Geschwindigkeit,
- minimale Latenz,
- geringster Energieverbrauch,
- höchste Genauigkeit,
- Offlinebetrieb,
- Datenschutz.

Eine einzige Implementierung kann diese Ziele häufig nicht gleichzeitig optimal erfüllen.

### 2.5 Erschwerte Weiterentwicklung

Neue Algorithmen müssten bestehende Implementierungen ersetzen.

Dadurch steigt das Risiko von Inkompatibilitäten.

---

## 3. Entscheidung

Jede Fähigkeit kann durch **eine oder mehrere Implementierungen** bereitgestellt werden.

Alle Implementierungen erfüllen denselben Capability Contract.

Für andere Komponenten ist ausschließlich die Fähigkeit sichtbar.

Welche Implementierung verwendet wird, entscheidet ausschließlich die Runtime.

---

## 4. Definition einer Implementierung

Eine Implementierung ist die technische Ausführung einer Fähigkeit.

Sie enthält beispielsweise:

- Programmlogik,
- Algorithmen,
- Bibliotheken,
- Hardwareoptimierungen,
- Laufzeitanpassungen.

Eine Implementierung besitzt keine eigene fachliche Bedeutung.

Sie ist lediglich eine mögliche technische Umsetzung derselben Fähigkeit.

---

## 5. Grundprinzipien

### 5.1 Fähigkeit und Implementierung sind getrennt

Beispiel:

```text
Capability

Bild skalieren
```

Implementierungen:

```text
CPUScaler

SIMDScaler

GPUScaler

VulkanScaler

AIUpscaler
```

Alle erfüllen dieselbe Aufgabe.

---

### 5.2 Alle Implementierungen erfüllen denselben Vertrag

Eine Implementierung darf ausschließlich registriert werden, wenn sie den vollständigen Capability Contract erfüllt.

Insbesondere müssen identisch bleiben:

- Ports,
- Datentypen,
- Semantik,
- Fehlerverhalten,
- Berechtigungen.

---

### 5.3 Die Runtime wählt die Implementierung

Die Auswahl erfolgt dynamisch.

Mögliche Kriterien:

- verfügbare Hardware,
- aktuelle Systemauslastung,
- Energieprofil,
- Echtzeitanforderungen,
- Datenschutzrichtlinien,
- Benutzerpräferenzen,
- Offlinefähigkeit,
- Vertrauensniveau.

Die aufrufende Pipeline bleibt unverändert.

---

### 5.4 Implementierungen dürfen unterschiedliche Optimierungen besitzen

Beispiel:

```text
OCR
├── CPU
├── GPU
├── FPGA
├── TPU
└── Cloud
```

Jede Implementierung optimiert dieselbe Aufgabe für eine andere Umgebung.

---

### 5.5 Implementierungen dürfen unterschiedliche Programmiersprachen verwenden

Eine Fähigkeit kann implementiert sein in:

- NovaLang,
- Rust,
- C,
- C++,
- Zig,
- Python,
- Java,
- C#.

Der Capability Contract bleibt unverändert.

---

### 5.6 Implementierungen dürfen ausgetauscht werden

Eine Implementierung kann jederzeit ersetzt werden.

Beispiel:

Vorher:

```text
NovaOCR v1
```

Nachher:

```text
NovaOCR v2
```

oder

```text
FastOCR
```

oder

```text
GPUOCR
```

Bestehende Pipelines funktionieren unverändert weiter.

---

### 5.7 Implementierungen können parallel existieren

Mehrere Implementierungen derselben Fähigkeit dürfen gleichzeitig installiert sein.

Die Runtime entscheidet anhand ihrer Auswahlstrategie.

Beispiel:

```text
Image Resize
├── CPU
├── GPU
├── Vulkan
├── Remote
└── Embedded
```

---

### 5.8 Implementierungen besitzen Metadaten

Jede Implementierung beschreibt ihre Eigenschaften.

Beispiele:

- unterstützte Hardware,
- Speicherbedarf,
- durchschnittliche Laufzeit,
- Energieverbrauch,
- Genauigkeit,
- Thread-Sicherheit,
- GPU-Unterstützung,
- Echtzeitfähigkeit,
- Lizenzinformationen.

Diese Informationen werden vom Optimierer genutzt.

---

### 5.9 Implementierungen dürfen deaktiviert werden

Eine Implementierung kann:

- deaktiviert,
- ersetzt,
- aktualisiert,
- priorisiert,
- gesperrt

werden, ohne den Capability Contract zu verändern.

---

### 5.10 Implementierungen dürfen verteilt ausgeführt werden

Eine Implementierung kann lokal oder verteilt arbeiten.

Beispiele:

```text
lokal

Cluster

Remote Server

Edge Device

Cloud

Container

Virtuelle Maschine
```

Für die aufrufende Fähigkeit bleibt dies transparent.

---

## 6. Beispiele

### OCR

```text
Capability

OCR
```

Implementierungen:

```text
Tesseract

EasyOCR

NovaOCR

GPUOCR

CloudOCR
```

Alle erzeugen denselben Ausgabetyp.

---

### Bildskalierung

```text
Capability

Resize Image
```

Implementierungen:

```text
CPU

SIMD

OpenCL

CUDA

Vulkan
```

Die Runtime wählt automatisch die optimale Variante.

---

### Übersetzung

```text
Capability

Translate Text
```

Implementierungen:

```text
Offline LLM

LibreTranslate

DeepL Adapter

Cloud AI

NovaTranslate
```

Alle liefern denselben Contract.

---

## 7. Auswahlstrategie

Die Runtime kann folgende Kriterien kombinieren:

- Geschwindigkeit,
- Genauigkeit,
- Speicherbedarf,
- Energieverbrauch,
- Hardware,
- Datenschutz,
- Kosten,
- Verfügbarkeit,
- Offlinefähigkeit,
- Lizenz,
- Benutzerpräferenz.

Die Auswahlstrategie bleibt vollständig konfigurierbar.

---

## 8. Vorteile

Dieses Architekturprinzip ermöglicht:

- langfristige Austauschbarkeit,
- technologische Unabhängigkeit,
- automatische Optimierung,
- bessere Hardwareausnutzung,
- parallele Weiterentwicklung,
- einfachere Wartung,
- höhere Zukunftssicherheit,
- bessere Skalierbarkeit,
- flexible Deployment-Strategien.

---

## 9. Nachteile

Dieses Modell erfordert:

- Capability Registry,
- Implementierungsverwaltung,
- Auswahlalgorithmen,
- Kompatibilitätsprüfungen,
- zusätzliche Metadaten.

Diese Komplexität wird bewusst akzeptiert, da sie ausschließlich innerhalb der Plattform verbleibt.

---

## 10. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Capability Registry
- Capability Runtime
- Pipeline Planner
- Scheduler
- Optimizer
- Capability Store
- Remote Execution
- Hardware Manager
- GPU Runtime
- KI-Assistent
- Debugger
- Profiler

Alle diese Komponenten arbeiten auf Basis austauschbarer Implementierungen.

---

## 11. Implementierungshinweise

Jede Implementierung registriert sich beim Capability Registry.

Dabei werden mindestens folgende Informationen hinterlegt:

- Capability-ID,
- Implementierungs-ID,
- Version,
- unterstützte Plattformen,
- Hardwareanforderungen,
- Qualitätsmerkmale,
- Priorität,
- Vertrauensstatus.

Die Runtime bewertet diese Informationen bei jeder Ausführung.

---

## 12. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- sie den vollständigen Capability Contract erfüllt,
- alle Portdefinitionen unverändert bleiben,
- dieselbe fachliche Semantik eingehalten wird,
- keine zusätzlichen verpflichtenden Ein- oder Ausgänge eingeführt werden,
- sie unabhängig von anderen Implementierungen betrieben werden kann,
- sie registriert und versioniert ist,
- ihre Eigenschaften deklarativ beschrieben werden.

---

## 13. Zusammenfassung

NovaOS trennt konsequent zwischen Fähigkeiten und deren technischer Umsetzung.

Eine Fähigkeit kann durch beliebig viele unterschiedliche Implementierungen bereitgestellt werden.

Alle Implementierungen erfüllen denselben Capability Contract und bleiben dadurch vollständig austauschbar.

Die Runtime wählt anhand der aktuellen Umgebung automatisch die geeignetste Implementierung aus, ohne dass bestehende Pipelines oder Anwendungen angepasst werden müssen.

Dieses Architekturprinzip schafft die Grundlage für eine hardwareunabhängige, zukunftssichere und hochoptimierbare Capability-Plattform.