# NPSPEC-CAPABILITY-DEPENDENCY-0001: Abhängigkeiten zwischen Fähigkeiten

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0002, ADR-CAPABILITY-0005, ADR-CAPABILITY-0008, ADR-CAPABILITY-0012, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-DESCRIPTOR-0001, NPSPEC-CAPABILITY-DISCOVERY-0001, NPSPEC-CAPABILITY-REGISTRY-0001, NPSPEC-CAPABILITY-VERSIONING-0001

---

# 1. Zweck

Diese Spezifikation definiert das Modell zur Beschreibung von Abhängigkeiten zwischen Nova Capabilities.

Abhängigkeiten beschreiben, welche anderen Fähigkeiten, Dienste oder Ressourcen für den Betrieb einer Capability erforderlich oder optional nutzbar sind.

Ein standardisiertes Abhängigkeitsmodell ermöglicht:

- automatische Installation
- automatische Validierung
- reproduzierbare Ausführung
- sichere Aktualisierung
- automatische Pipeline-Erstellung
- konsistente Versionierung
- Impact-Analysen

---

# 2. Ziele

Das Abhängigkeitsmodell verfolgt folgende Ziele:

- explizite Abhängigkeiten
- deterministische Auflösung
- minimale Kopplung
- Wiederverwendbarkeit
- Parallelbetrieb mehrerer Versionen
- automatische Konflikterkennung
- reproduzierbare Systeme
- Erweiterbarkeit

Explizit deklarierte Abhängigkeiten erleichtern die automatische Auflösung, Konflikterkennung und Wartbarkeit modularer Softwaresysteme. :contentReference[oaicite:0]{index=0}

---

# 3. Grundprinzipien

Abhängigkeiten sind:

- deklarativ
- explizit
- versionsfähig
- validierbar
- maschinenlesbar
- unabhängig von Implementierungen

Keine Capability darf versteckte oder implizite Abhängigkeiten besitzen.

---

# 4. Architekturübersicht

```text
Capability

│

├── Required Dependencies

├── Optional Dependencies

├── Runtime Dependencies

├── Build Dependencies

├── Service Dependencies

├── Hardware Dependencies

├── Data Dependencies

└── Version Constraints
```

---

# 5. Arten von Abhängigkeiten

Nova unterscheidet folgende Typen:

```text
Required

Optional

Runtime

Build

Development

Hardware

Data

Service
```

Jeder Typ besitzt eine klar definierte Bedeutung.

---

# 6. Pflichtabhängigkeiten (Required)

Pflichtabhängigkeiten müssen vorhanden sein.

Fehlt eine Required Dependency,

- darf die Capability nicht aktiviert werden,
- darf keine Pipeline ausgeführt werden.

Beispiel:

```text
OCR

↓

nova.image.core
```

---

# 7. Optionale Abhängigkeiten

Optionale Abhängigkeiten erweitern den Funktionsumfang.

Fehlen sie,

- bleibt die Capability lauffähig,
- einzelne Funktionen stehen jedoch nicht zur Verfügung.

Beispiel:

```text
Speech Recognition

↓

GPU Acceleration
```

---

# 8. Runtime-Abhängigkeiten

Runtime-Abhängigkeiten werden ausschließlich während der Ausführung benötigt.

Beispiele:

```text
AI Runtime

GPU Runtime

Physics Runtime

Database Runtime
```

Sie müssen vor dem Start verfügbar sein.

---

# 9. Build-Abhängigkeiten

Build-Abhängigkeiten werden ausschließlich während der Entwicklung verwendet.

Beispiele:

```text
Compiler

SDK

Generator

Test Framework
```

Sie werden nicht auf Zielsystemen benötigt.

---

# 10. Entwicklungsabhängigkeiten

Development Dependencies unterstützen:

- Debugging
- Tests
- Benchmarks
- Dokumentation
- Simulation

Sie gehören nicht zum produktiven Betrieb.

---

# 11. Hardware-Abhängigkeiten

Capabilities können Hardware voraussetzen.

Beispiele:

```text
GPU

NPU

TPM

Camera

Microphone

Bluetooth

USB

Serial
```

Discovery berücksichtigt diese Anforderungen automatisch.

---

# 12. Datenabhängigkeiten

Eine Capability kann bestimmte Datentypen voraussetzen.

Beispiele:

```text
RasterImage

Tensor

PointCloud

Mesh

Document

Measurement
```

Diese Informationen unterstützen den Pipeline Planner.

---

# 13. Service-Abhängigkeiten

Capabilities können andere Services benötigen.

Beispiele:

```text
Speech Service

Vision Service

Model Repository

Authentication Service

Translation Service
```

Services können lokal oder verteilt bereitgestellt werden.

---

# 14. Versionsabhängigkeiten

Jede Abhängigkeit kann Versionsregeln besitzen.

Beispiele:

```text
>=2.0

2.x

1.4.*

==3.0
```

Die Runtime berücksichtigt ausschließlich kompatible Versionen.

---

# 15. Abhängigkeitsgraph

Alle Abhängigkeiten bilden einen gerichteten Graphen.

```text
OCR

│

├── Image Core

├── AI Runtime

└── Language Model
```

Der Graph darf keine unzulässigen Zyklen enthalten.

---

# 16. Zyklische Abhängigkeiten

Direkte zyklische Abhängigkeiten sind verboten.

Nicht zulässig:

```text
A

↓

B

↓

A
```

Indirekte Zyklen sind ebenfalls unzulässig.

Die Registry muss solche Konfigurationen bereits während der Validierung erkennen.

---

# 17. Version Resolution

Vor der Aktivierung erfolgt:

```text
Capability

↓

Dependencies

↓

Version Resolver

↓

Registry

↓

Validierung
```

Erst nach erfolgreicher Auflösung darf die Capability aktiviert werden.

---

# 18. Konflikterkennung

Folgende Konflikte müssen erkannt werden:

- inkompatible Versionen
- fehlende Abhängigkeiten
- zyklische Abhängigkeiten
- gesperrte Capabilities
- ungültige Berechtigungen

Konflikte verhindern die Aktivierung.

---

# 19. Automatische Installation

Der Package Manager darf fehlende Required Dependencies automatisch installieren.

Ablauf:

```text
Capability

↓

Dependency Analyse

↓

Registry

↓

Package Repository

↓

Installation

↓

Validierung
```

Optionale Abhängigkeiten werden nur auf Wunsch installiert.

---

# 20. Impact-Analyse

Vor Aktualisierungen kann eine Analyse durchgeführt werden.

Ermittelt werden:

- abhängige Capabilities
- betroffene Pipelines
- betroffene Solutions
- mögliche Versionskonflikte

Dadurch können Risiken vor Änderungen bewertet werden.

---

# 21. Discovery-Unterstützung

Discovery verwendet Abhängigkeiten für:

- automatische Pipeline-Erstellung
- automatische Installation
- Ranking
- Kompatibilitätsprüfung

Abhängigkeiten sind Teil der semantischen Suche.

---

# 22. Runtime-Verhalten

Vor jeder Ausführung überprüft die Runtime:

- Pflichtabhängigkeiten
- Versionen
- Hardware
- Berechtigungen
- Services

Fehlt eine notwendige Abhängigkeit, wird die Ausführung abgebrochen.

---

# 23. YAML-Beispiel

```yaml
dependencies:

  required:

    - id: nova.image.core
      version: ">=2.0"

    - id: nova.ai.runtime
      version: "2.x"

  optional:

    - id: nova.gpu.vulkan
      version: ">=1.5"

  hardware:

    - gpu

  services:

    - model.repository
```

---

# 24. Validierungsregeln

Abhängigkeiten gelten als gültig, wenn:

- alle referenzierten Capability IDs existieren,
- Versionsregeln gültig sind,
- keine Zyklen entstehen,
- alle Required Dependencies auflösbar sind,
- keine widersprüchlichen Anforderungen bestehen.

Ungültige Abhängigkeitsgraphen dürfen nicht registriert werden.

---

# 25. Qualitätsanforderungen

Das Abhängigkeitsmodell muss:

- vollständig deklarativ sein,
- deterministisch auflösbar sein,
- mehrere Versionen unterstützen,
- automatische Konflikterkennung ermöglichen,
- Impact-Analysen unterstützen,
- zyklische Abhängigkeiten verhindern,
- reproduzierbare Installationen gewährleisten,
- langfristig kompatibel bleiben.

Ein expliziter Abhängigkeitsgraph verbessert die Wartbarkeit, erlaubt automatische Auflösung und reduziert Integrationsfehler in modularen Architekturen. :contentReference[oaicite:1]{index=1}

---

# 26. Konformitätskriterien

Eine Capability erfüllt diese Spezifikation, wenn:

- sämtliche Abhängigkeiten explizit deklariert sind,
- Pflicht- und optionale Abhängigkeiten getrennt beschrieben werden,
- Versionsregeln eindeutig definiert sind,
- keine zyklischen Abhängigkeiten bestehen,
- alle Required Dependencies aufgelöst werden können,
- Discovery und Runtime das Abhängigkeitsmodell vollständig unterstützen.

---

# 27. Zusammenfassung

Das Nova-Abhängigkeitsmodell definiert eine standardisierte Beschreibung aller Beziehungen zwischen Capabilities, Services, Hardware und Daten.

Durch explizite, versionierte und validierbare Abhängigkeiten können Installationen automatisiert, Konflikte frühzeitig erkannt und reproduzierbare Capability-Pipelines aufgebaut werden.

Die klare Trennung zwischen Pflicht-, optionalen, Laufzeit- und Hardware-Abhängigkeiten schafft eine robuste Grundlage für eine skalierbare, modulare und langfristig wartbare Nova Platform.