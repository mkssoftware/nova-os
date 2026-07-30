# ADR-CAPABILITY-0011: Fähigkeiten können lokal, entfernt oder hardwarebeschleunigt ausgeführt werden

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Nova Platform
- **Ersetzt:** Keine
- **Abhängigkeiten:** ADR-CAPABILITY-0003, ADR-CAPABILITY-0005, ADR-CAPABILITY-0006, ADR-CAPABILITY-0008, ADR-CAPABILITY-0009
- **Folgedokumente:** NPSPEC-CAPABILITY-EXECUTION-0001, NPSPEC-CAPABILITY-RUNTIME-0001, NPSPEC-HARDWARE-ABSTRACTION-0001, NPSPEC-REMOTE-EXECUTION-0001

---

# 1. Kontext

NovaOS betrachtet Fähigkeiten (Capabilities) als logische Funktionseinheiten.

Der Ort ihrer Ausführung ist für den Aufrufer grundsätzlich unerheblich.

Eine Fähigkeit kann beispielsweise ausgeführt werden:

- lokal auf der CPU,
- lokal auf der GPU,
- auf einem DSP,
- auf einem FPGA,
- auf einer TPU oder NPU,
- auf einem Mikrocontroller,
- auf einem zweiten Computer,
- auf einem lokalen Cluster,
- in einer virtuellen Maschine,
- innerhalb eines Containers,
- auf einem Cloud-System.

Für Entwickler und Pipelines soll diese Vielfalt transparent bleiben.

Die Runtime entscheidet anhand verfügbarer Informationen über die optimale Ausführungsstrategie.

---

# 2. Problemstellung

Eine feste Bindung an einen bestimmten Ausführungsort bringt erhebliche Nachteile.

## 2.1 Schlechte Hardwareausnutzung

Spezialisierte Hardware bleibt ungenutzt.

Beispiele:

- GPU
- NPU
- DSP
- FPGA

---

## 2.2 Geringe Skalierbarkeit

Rechenintensive Aufgaben können nicht auf leistungsfähigere Systeme ausgelagert werden.

---

## 2.3 Fehlende Mobilität

Eine Fähigkeit müsste für jede Zielplattform separat entwickelt werden.

---

## 2.4 Eingeschränkte Optimierung

Die Runtime kann Lasten nicht dynamisch verteilen.

---

## 2.5 Fehlende Zukunftssicherheit

Neue Hardwareklassen könnten nur durch umfangreiche Änderungen unterstützt werden.

---

# 3. Entscheidung

NovaOS trennt die **logische Fähigkeit** vollständig von ihrem **Ausführungsort**.

Jede Capability beschreibt ausschließlich ihre Funktion.

Die Runtime entscheidet, wo und wie die Ausführung erfolgt.

Der Aufrufer kennt ausschließlich den Capability Contract.

---

# 4. Definition eines Ausführungsortes

Ein Ausführungsort beschreibt die technische Umgebung, in der eine Implementierung einer Fähigkeit ausgeführt wird.

Mögliche Ausführungsorte sind:

- lokale CPU
- GPU
- DSP
- FPGA
- NPU/TPU
- Remote-Rechner
- Cluster
- virtuelle Maschine
- Container
- Cloud-Service
- Embedded-System

Weitere Ausführungsorte können zukünftig ergänzt werden.

---

# 5. Grundprinzipien

## 5.1 Der Ausführungsort ist transparent

Eine Pipeline beschreibt ausschließlich Fähigkeiten.

Beispiel:

```text
Bild laden
      ↓
Bild skalieren
      ↓
OCR
      ↓
Übersetzung
```

Es wird nicht beschrieben, auf welcher Hardware die einzelnen Fähigkeiten ausgeführt werden.

---

## 5.2 Die Runtime entscheidet

Die Auswahl erfolgt ausschließlich durch die Runtime.

Mögliche Kriterien:

- verfügbare Hardware,
- Energieverbrauch,
- aktuelle Auslastung,
- Sicherheitsrichtlinien,
- Datenschutz,
- Netzwerkverfügbarkeit,
- Kosten,
- Benutzerpräferenzen,
- Echtzeitanforderungen.

---

## 5.3 Implementierungen deklarieren ihre Fähigkeiten

Jede Implementierung beschreibt ihre unterstützten Ausführungsumgebungen.

Beispiel:

```yaml
supportedExecution:

- cpu
- gpu
- npu
```

Dadurch kann die Runtime geeignete Implementierungen auswählen.

---

## 5.4 Hardwarebeschleunigung ist optional

Eine Capability darf mehrere Implementierungen besitzen.

Beispiel:

```text
Resize Image

├── CPU
├── SIMD
├── Vulkan
├── CUDA
└── OpenCL
```

Die Runtime wählt automatisch die optimale Variante.

---

## 5.5 Remote-Ausführung ist transparent

Eine Fähigkeit kann auf einem entfernten System ausgeführt werden.

Beispiel:

```text
Pipeline

↓

Capability Runtime

↓

Remote Node

↓

Capability
```

Für die Pipeline bleibt dies unsichtbar.

---

## 5.6 Lokale Ausführung besitzt Vorrang

Standardmäßig werden Fähigkeiten lokal ausgeführt.

Eine Verlagerung erfolgt nur, wenn:

- sie zulässig ist,
- Sicherheitsrichtlinien dies erlauben,
- der erwartete Nutzen größer als der Kommunikationsaufwand ist.

---

## 5.7 Datenschutz bleibt erhalten

Fähigkeiten dürfen ausschließlich dann remote ausgeführt werden, wenn:

- ihre Sicherheitsklassifizierung dies erlaubt,
- Datenschutzrichtlinien eingehalten werden,
- erforderliche Berechtigungen vorliegen.

Vertrauliche Daten dürfen nicht automatisch an entfernte Systeme übertragen werden.

---

## 5.8 Hardware ist austauschbar

Capabilities dürfen keine hardwareabhängigen Annahmen treffen.

Beispielsweise darf eine Fähigkeit nicht voraussetzen, dass:

- CUDA verfügbar ist,
- Vulkan installiert ist,
- eine GPU vorhanden ist,
- eine bestimmte CPU-Architektur verwendet wird.

Diese Entscheidungen trifft ausschließlich die Runtime.

---

## 5.9 Verteilte Ausführung ist zulässig

Eine Pipeline darf auf mehrere Systeme verteilt werden.

Beispiel:

```text
PC

↓

OCR

↓

Cluster

↓

KI-Auswertung

↓

Lokaler Rechner

↓

Anzeige
```

Die Runtime übernimmt Synchronisation und Datentransport.

---

## 5.10 Fallback muss möglich sein

Kann eine bevorzugte Implementierung nicht verwendet werden, muss die Runtime automatisch eine geeignete Alternative auswählen.

Beispiel:

```text
GPU

↓

nicht verfügbar

↓

CPU

↓

Ausführung fortsetzen
```

Der Capability Contract bleibt unverändert.

---

# 6. Unterstützte Ausführungsmodelle

NovaOS unterstützt grundsätzlich folgende Modelle:

```text
Lokale CPU

SIMD

GPU

NPU

TPU

DSP

FPGA

Remote

Cluster

Cloud

Container

Virtuelle Maschine

Embedded-System
```

Diese Liste ist erweiterbar.

---

# 7. Beispiele

## KI-Inferenz

Laptop:

```text
Capability

KI-Inferenz

↓

NPU
```

Desktop:

```text
Capability

KI-Inferenz

↓

GPU
```

Server:

```text
Capability

KI-Inferenz

↓

Cluster
```

Alle erfüllen denselben Capability Contract.

---

## Bildverarbeitung

Notebook:

```text
CPU
```

Workstation:

```text
GPU
```

Industrie-PC:

```text
FPGA
```

Die Anwendung bleibt unverändert.

---

## Sprachübersetzung

Offline:

```text
Lokales LLM
```

Online:

```text
Remote-KI
```

Der Aufrufer verwendet dieselbe Capability.

---

# 8. Vorteile

Dieses Architekturprinzip ermöglicht:

- optimale Hardwareausnutzung,
- hohe Skalierbarkeit,
- automatische Lastverteilung,
- Zukunftssicherheit,
- einfache Unterstützung neuer Hardware,
- flexible Deployment-Strategien,
- transparente Hardwarebeschleunigung,
- bessere Energieeffizienz,
- geringere Entwicklungsaufwände.

---

# 9. Nachteile

Dieses Modell erfordert:

- eine leistungsfähige Runtime,
- Hardwareerkennung,
- Auswahlalgorithmen,
- verteilte Kommunikation,
- zusätzliche Sicherheitsmechanismen,
- Netzwerkverwaltung.

Diese Komplexität wird bewusst akzeptiert, da sie vollständig innerhalb der Plattform gekapselt bleibt.

---

# 10. Auswirkungen auf andere Komponenten

Dieses ADR beeinflusst insbesondere:

- Capability Runtime
- Scheduler
- Hardware Manager
- Resource Manager
- GPU Runtime
- NPU Runtime
- Cluster Manager
- Remote Execution Framework
- Pipeline Planner
- Optimizer
- Security Manager
- Capability Registry

Alle diese Komponenten müssen den Ausführungsort dynamisch berücksichtigen.

---

# 11. Implementierungshinweise

Jede Implementierung registriert ihre unterstützten Ausführungsumgebungen und ihre Leistungsmerkmale.

Die Runtime bewertet diese Informationen gemeinsam mit der aktuellen Systemumgebung und wählt die geeignetste Implementierung aus.

Die Auswahl sollte zur Laufzeit neu bewertet werden können, um auf Änderungen der Hardwareverfügbarkeit oder Systemauslastung reagieren zu können.

---

# 12. Konformitätskriterien

Eine Implementierung gilt als konform, wenn:

- sie ihren unterstützten Ausführungsort deklarativ beschreibt,
- der Capability Contract unabhängig vom Ausführungsort identisch bleibt,
- Hardwareabhängigkeiten nicht vorausgesetzt werden,
- Fallback-Strategien unterstützt werden,
- Sicherheits- und Datenschutzrichtlinien eingehalten werden,
- lokale und entfernte Ausführung transparent behandelt werden,
- neue Ausführungsumgebungen ohne Änderungen an bestehenden Capabilities ergänzt werden können.

---

# 13. Zusammenfassung

NovaOS trennt konsequent die fachliche Beschreibung einer Fähigkeit von ihrer technischen Ausführung.

Capabilities können lokal, hardwarebeschleunigt oder auf entfernten Systemen ausgeführt werden, ohne dass sich ihr Capability Contract oder ihre Verwendung verändert.

Die Runtime wählt anhand von Hardware, Sicherheitsrichtlinien, Datenschutz, Energieverbrauch und Systemauslastung automatisch die optimale Implementierung und den geeigneten Ausführungsort.

Dieses Architekturprinzip bildet die Grundlage für eine skalierbare, hardwareunabhängige und zukunftssichere Capability-Plattform.