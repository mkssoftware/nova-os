# NPSPEC-CAPABILITY-DESCRIPTOR-0001: Deklaratives Fähigkeiten-Manifest

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:** ADR-CAPABILITY-0003, ADR-CAPABILITY-0005, ADR-CAPABILITY-0008, NPSPEC-CAPABILITY-0001, NPSPEC-CAPABILITY-MODEL-0001

---

# 1. Zweck

Diese Spezifikation definiert das **Deklarative Fähigkeiten-Manifest (Capability Descriptor)**.

Das Manifest beschreibt eine Capability vollständig, ohne deren Implementierung offenzulegen.

Es bildet die Grundlage für:

- Capability Registry
- Capability Runtime
- Intent Planner
- Pipeline Planner
- Package Manager
- SDK
- Visual Designer
- Dokumentation
- Sicherheitsprüfung
- Deployment

Das Manifest ist die einzige öffentliche Beschreibung einer Capability.

---

# 2. Ziele

Das Manifest verfolgt folgende Ziele:

- deklarative Beschreibung
- vollständige Maschinenlesbarkeit
- Implementierungsunabhängigkeit
- stabile Capability Contracts
- automatische Registrierung
- automatische Validierung
- Versionierbarkeit
- Erweiterbarkeit
- langfristige Kompatibilität

Deklarative Beschreibungen erleichtern außerdem die Wartbarkeit, Dokumentation und automatisierte Verarbeitung komplexer Softwaresysteme. :contentReference[oaicite:0]{index=0}

---

# 3. Architekturübersicht

```text
Capability Descriptor
│
├── Identity
├── Metadata
├── Contract
├── Inputs
├── Outputs
├── Parameters
├── Permissions
├── Execution
├── Resources
├── Security
├── Compatibility
├── Dependencies
├── Tags
├── Lifecycle
├── Diagnostics
├── Documentation
└── Extensions
```

---

# 4. Eigenschaften

Ein Descriptor beschreibt ausschließlich:

- was eine Capability ist,
- welche Schnittstelle sie besitzt,
- welche Eigenschaften sie besitzt,
- welche Anforderungen gelten.

Ein Descriptor beschreibt niemals:

- Quellcode,
- Binärformat,
- Programmiersprache,
- Threadmodell,
- Speicherlayout,
- Optimierungen,
- interne Algorithmen.

---

# 5. Dateiformat

Das Standardformat des Nova Capability Descriptor ist YAML.

Andere Formate (JSON, TOML, XML) dürfen intern verwendet werden, müssen jedoch verlustfrei in das Standardmodell überführbar sein.

Dateiendung:

```text
.capability.yaml
```

Beispiel:

```text
image.resize.capability.yaml
```

---

# 6. Grundstruktur

```yaml
identity:
metadata:
contract:
inputs:
outputs:
parameters:
permissions:
execution:
resources:
security:
compatibility:
dependencies:
documentation:
extensions:
```

Alle Hauptbereiche besitzen eine definierte Bedeutung.

---

# 7. Identity

Die Identity identifiziert eine Capability eindeutig.

Pflichtfelder:

```yaml
id:
version:
name:
namespace:
```

Optionale Felder:

```yaml
author:
organization:
created:
updated:
```

Beispiel

```yaml
identity:
  id: nova.image.resize
  version: 1.2.0
  name: Resize Image
  namespace: nova.image
```

---

# 8. Metadata

Metadaten dienen ausschließlich der Beschreibung.

Beispiele:

```yaml
metadata:
  title:
  description:
  category:
  subcategory:
  icon:
  homepage:
  license:
  language:
  tags:
```

Diese Informationen besitzen keinen Einfluss auf die Laufzeit.

---

# 9. Contract

Der Contract beschreibt ausschließlich die öffentliche Schnittstelle.

```yaml
contract:
  version:
  inputs:
  outputs:
  errors:
```

Er darf niemals Implementierungsdetails enthalten.

---

# 10. Inputs

Jeder Eingang besitzt mindestens:

```yaml
inputs:

- name:
  type:
  optional:
  description:
```

Beispiel

```yaml
inputs:

- name: image
  type: RasterImage
  optional: false
```

---

# 11. Outputs

Jeder Ausgang besitzt mindestens:

```yaml
outputs:

- name:
  type:
  description:
```

Beispiel

```yaml
outputs:

- name: resizedImage
  type: RasterImage
```

---

# 12. Parameters

Parameter verändern das Verhalten einer Capability.

Beispiel

```yaml
parameters:

- name: width
  type: Integer
  default: 1024

- name: algorithm
  type: Enum
  default: Lanczos
```

Parameter gehören nicht zum Capability Contract.

---

# 13. Permissions

Benötigte Berechtigungen werden explizit deklariert.

Beispiel

```yaml
permissions:

- filesystem.read
- filesystem.write
```

Nicht deklarierte Berechtigungen dürfen nicht verwendet werden.

---

# 14. Execution

Execution beschreibt unterstützte Ausführungsarten.

Beispiel

```yaml
execution:

supported:

- cpu
- gpu
- remote

preferred:

- gpu
```

Die Runtime entscheidet über die tatsächliche Ausführung.

---

# 15. Resources

Optional können Ressourcen beschrieben werden.

Beispiel

```yaml
resources:

memory: 256MB

threads: 4

gpuMemory: 2GB
```

Diese Angaben dienen ausschließlich der Planung.

---

# 16. Security

Sicherheitsinformationen werden deklarativ beschrieben.

Beispiel

```yaml
security:

sandbox: true

signatureRequired: true

trustLevel: verified
```

Die Runtime setzt diese Regeln durch.

---

# 17. Compatibility

Kompatibilitätsinformationen beschreiben unterstützte Bridges.

Beispiel

```yaml
compatibility:

windowsBridge: true

linuxBridge: true

legacyAdapter: false
```

---

# 18. Dependencies

Eine Capability kann andere Komponenten voraussetzen.

Beispiel

```yaml
dependencies:

- nova.image.core

- nova.math
```

Abhängigkeiten beziehen sich ausschließlich auf Capability Contracts.

---

# 19. Documentation

Der Descriptor kann Dokumentationsinformationen enthalten.

Beispiel

```yaml
documentation:

summary:

examples:

reference:

tutorial:
```

Diese Informationen werden von SDKs und Entwicklungswerkzeugen verwendet.

---

# 20. Extensions

Eigene Erweiterungen sind zulässig.

Alle Erweiterungen müssen unter einem eigenen Namespace definiert werden.

Beispiel

```yaml
extensions:

vendor:

example:

property:
```

Dadurch bleibt das Standardmodell kompatibel.

---

# 21. Vollständiges Beispiel

```yaml
identity:
  id: nova.image.resize
  version: 1.2.0
  name: Resize Image
  namespace: nova.image

metadata:
  category: image
  description: Skaliert Rasterbilder.

contract:
  version: 1.0

inputs:
  - name: image
    type: RasterImage

outputs:
  - name: image
    type: RasterImage

parameters:
  - name: width
    type: Integer
    default: 1024

permissions:
  - filesystem.read
  - filesystem.write

execution:
  supported:
    - cpu
    - gpu

security:
  sandbox: true

compatibility:
  windowsBridge: false
```

---

# 22. Validierungsregeln

Ein Descriptor ist gültig, wenn:

- alle Pflichtfelder vorhanden sind,
- die ID eindeutig ist,
- Versionsnummern gültig sind,
- Ports typisiert sind,
- Berechtigungen bekannt sind,
- Datentypen registriert sind,
- Capability Contracts vollständig beschrieben sind.

Ungültige Descriptoren dürfen nicht registriert werden.

---

# 23. Versionierung

Jeder Descriptor besitzt eine eigene Version.

Änderungen am Contract führen zu einer neuen Contract-Version.

Änderungen an Metadaten können ohne Änderung des Contracts erfolgen.

Versionierung erfolgt nach Semantic Versioning.

---

# 24. Qualitätsanforderungen

Ein Descriptor muss:

- vollständig deklarativ sein,
- menschenlesbar sein,
- maschinenlesbar sein,
- validierbar sein,
- reproduzierbar sein,
- versionsfähig sein,
- unabhängig von Implementierungen sein,
- langfristig kompatibel bleiben.

Ein klar definiertes Manifest verbessert die Konsistenz zwischen Architektur, Dokumentation und Implementierung und unterstützt automatisierte Werkzeuge entlang des gesamten Software-Lebenszyklus. :contentReference[oaicite:1]{index=1}

---

# 25. Konformitätskriterien

Ein Descriptor gilt als konform, wenn:

- alle Pflichtbereiche vorhanden sind,
- ausschließlich deklarative Informationen enthalten sind,
- keine Implementierungsdetails beschrieben werden,
- der Capability Contract vollständig definiert ist,
- alle Ports typisiert sind,
- Berechtigungen vollständig deklariert werden,
- Validierungsregeln erfüllt sind,
- das Manifest durch die Capability Registry verarbeitet werden kann.

---

# 26. Zusammenfassung

Das Deklarative Fähigkeiten-Manifest bildet die standardisierte Beschreibung jeder Nova Capability.

Es definiert Identität, Metadaten, Capability Contract, Ein- und Ausgänge, Parameter, Berechtigungen, Sicherheits- und Ausführungseigenschaften in einer vollständig deklarativen Form.

Durch die strikte Trennung zwischen Manifest und Implementierung entsteht eine stabile, versionsfähige und maschinenlesbare Grundlage für Registrierung, Planung, Validierung und Ausführung aller Capabilities innerhalb der Nova Platform.