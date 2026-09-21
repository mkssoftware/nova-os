# ADR-AI-0002 – Model Registry

## Status

Angenommen

## Kategorie

AI / Models / Registry

## Kontext

Die systemweite `Nova.AI Runtime` benötigt eine zentrale Möglichkeit, verfügbare AI-Modelle zu entdecken, zu identifizieren und hinsichtlich ihrer Fähigkeiten und Anforderungen zu bewerten.

Ohne gemeinsame Registry müssten Anwendungen konkrete Modelle kennen und selbst verwalten.

Das würde zu enger Kopplung führen:

```text
Application
    ↓
Specific Model
    ↓
Specific Runtime
    ↓
Specific Hardware
```

NovaOS soll stattdessen AI Capabilities von konkreten Modellen trennen.

## Entscheidung

NovaOS erhält eine systemweite **AI Model Registry**.

```text
Installed Models
Remote Models
System Models
User Models
      ↓
Model Registry
      ↓
Capability Discovery
      ↓
Model Selection
      ↓
Nova.AI Runtime
```

Die Registry beschreibt verfügbare Modelle, führt sie jedoch nicht selbst aus.

## Model Identity

Jedes registrierte Modell erhält eine stabile `Model ID`.

Beispiel:

```text
Model ID
Version
Variant
```

Die Identität eines Modells ist unabhängig von:

```text
File Path
Storage Location
Loaded State
Execution Provider
Runtime Instance
```

Ein Update erzeugt eine neue Model Version, nicht stillschweigend eine neue Bedeutung derselben Version.

## Model Metadata

Für jedes Modell müssen strukturierte Metadaten verfügbar sein.

Mindestens relevant sind:

```text
Model ID
Version
Model Family
Capabilities
Input Types
Output Types
Context Limits
Resource Requirements
Supported Providers
Determinism Properties
Quantization
Provenance
Trust Information
License Metadata
Integrity Information
```

Zusätzliche modell- oder capability-spezifische Eigenschaften dürfen ergänzt werden.

## Capability Mapping

Modelle registrieren die AI Capabilities, die sie bereitstellen.

Beispiel:

```text
Model A
 ├── AI.Text.Generate
 ├── AI.Text.Summarize
 └── AI.Text.Classify

Model B
 ├── AI.Text.Embed
 └── AI.Semantic.Search
```

Eine Anwendung fragt normalerweise nach einer Capability und nicht nach einer konkreten Model ID.

## Semantic Types

Input und Output werden über semantische Typen beschrieben.

Beispiele:

```text
Text
Image
Audio
Embedding
TokenSequence
ClassificationResult
GeneratedImage
Transcription
```

Dadurch kann NovaOS feststellen, ob ein Modell mit einer Pipeline kompatibel ist.

## Model Variants

Mehrere Varianten desselben Modells dürfen parallel registriert sein.

Beispiele:

```text
FP32
FP16
BF16
INT8
INT4
CPU Optimized
GPU Optimized
NPU Optimized
```

Varianten besitzen unterschiedliche Ressourcen-, Performance- und Determinismus-Eigenschaften.

Die Auswahl erfolgt durch die AI Runtime und Execution Planning.

## Local und Remote Models

Die Registry darf lokale und entfernte Modelle beschreiben.

```text
Model Registry
   ├── Local Model
   ├── Device Model
   ├── Cluster Model
   └── Remote Model
```

Die Registrierung eines Remote Models bedeutet nicht automatisch, dass dessen Nutzung erlaubt ist.

Sovereignty, Trust und Authorization werden bei der konkreten Ausführung geprüft.

## Availability

Registry und tatsächliche Verfügbarkeit werden getrennt betrachtet.

```text
Registered
    ≠
Available
    ≠
Loaded
    ≠
Trusted
    ≠
Authorized
```

Die Runtime muss den aktuellen Zustand eines Modells berücksichtigen.

## Trust und Provenance

Die Registry speichert oder referenziert Trust- und Provenance-Informationen.

Beispiele:

```text
Publisher
Signature
Package Provenance
Model Provenance
Integrity Hash
Build Information
Source
Verification State
```

Die Registry selbst entscheidet nicht allein, ob ein Modell vertrauenswürdig oder autorisiert ist.

## Resource Requirements

Modelle beschreiben ihre Ressourcenanforderungen.

Beispiele:

```text
RAM
VRAM
NPU Memory
Storage
CPU Features
GPU Features
Accelerator Requirements
Expected Energy
```

Diese Informationen werden vom Execution Planner und der Compute Runtime verwendet.

## Runtime Compatibility

Die Registry beschreibt, über welche Runtime oder Adapter ein Modell ausgeführt werden kann.

Beispiele:

```text
Nova Native
ONNX
llama.cpp
PyTorch Adapter
Vendor NPU Runtime
Remote Provider Adapter
```

Die Anwendung bleibt davon unabhängig.

## Model Discovery

Modelle können aus unterschiedlichen Quellen registriert werden:

```text
System Installation
Software Package
Model Package
User Installation
Enterprise Repository
Remote Provider
```

Discovery darf neue Modelle sichtbar machen, aber nicht automatisch autorisieren oder ausführen.

## Registry Updates

Änderungen an der Registry erfolgen kontrolliert.

```text
Discover
   ↓
Validate Metadata
   ↓
Verify Integrity
   ↓
Register
   ↓
Expose Capabilities
```

Entfernte oder ungültige Modelle müssen aus der aktiven Auswahl ausgeschlossen werden können.

## Versionierung

Mehrere Model Versions dürfen gleichzeitig vorhanden sein.

```text
Model
 ├── v1
 ├── v2
 └── v3
```

Execution Contracts dürfen:

```text
Any Compatible Version
Minimum Version
Version Range
Exact Version
```

fordern.

Deterministische Ausführungen können eine exakte Model Version verlangen.

## Model Selection

Die Registry liefert Kandidaten für die Model Selection.

```text
Requested AI Capability
        ↓
Model Registry
        ↓
Compatible Models
        ↓
Hard Constraint Filtering
        ↓
Model Selection
```

Die Registry selbst bestimmt nicht die endgültige Auswahlstrategie.

Diese gehört zur `Nova.AI Runtime` und zum Execution Planner.

## Lifecycle Integration

Die Registry beschreibt Modelle unabhängig von ihrem aktuellen Ladezustand.

```text
Registered
   ↓
Selected
   ↓
Loaded
   ↓
Executing
   ↓
Idle
   ↓
Unloaded
```

Model Lifecycle Management bleibt Aufgabe der AI Runtime.

## Security

Registry-Einträge dürfen keine Security-Grenzen umgehen.

Ein manipuliertes Registry-Metadatum darf insbesondere nicht automatisch:

```text
Trust erhöhen
Authorization erzeugen
Sovereignty verändern
Permissions erweitern
Integrity bestätigen
```

Kritische Eigenschaften müssen durch geeignete Evidence verifizierbar sein.

## Observability

Registry-Operationen müssen beobachtbar sein.

Relevante Informationen sind:

```text
Model ID
Version
Registry Event
Source
Verification State
Availability
Capability Changes
Provider Compatibility
```

Sensitive Informationen dürfen nicht unnötig protokolliert werden.

## Deterministic Mode

Für deterministische AI-Ausführungen muss die verwendete Registry-Sicht reproduzierbar sein.

Dazu können insbesondere fixiert werden:

```text
Model ID
Model Version
Variant
Registry Version
Runtime Adapter Version
```

Spätere Registry-Änderungen dürfen eine bereits deterministisch definierte Ausführung nicht stillschweigend verändern.

## Normative Anforderungen

1. NovaOS MUSS eine systemweite AI Model Registry bereitstellen.
2. Jedes registrierte Modell MUSS eine stabile Model ID besitzen.
3. Model Identity MUSS von Speicherort und Runtime Instance getrennt sein.
4. Modelle MÜSSEN versionierbar sein.
5. Die Registry MUSS AI Capabilities eines Modells beschreiben können.
6. Input- und Output-Typen SOLLEN semantisch beschrieben werden.
7. Mehrere Varianten und Versionen eines Modells MÜSSEN parallel registrierbar sein.
8. Lokale und Remote Models MÜSSEN durch dasselbe Registry-Modell beschreibbar sein.
9. `Registered`, `Available`, `Loaded`, `Trusted` und `Authorized` MÜSSEN getrennte Zustände bleiben.
10. Resource Requirements MÜSSEN maschinenlesbar beschreibbar sein.
11. Runtime- und Provider-Kompatibilität MUSS beschreibbar sein.
12. Model Provenance und Integrity MÜSSEN referenzierbar sein.
13. Discovery DARF keine automatische Authorization erzeugen.
14. Registry-Metadaten DÜRFEN Trust nicht ohne Evidence erzeugen.
15. Model Selection MUSS Registry-Daten verwenden können.
16. Die endgültige Model Selection DARF nicht Aufgabe der Registry selbst sein.
17. Exact Model Versions MÜSSEN für deterministische Ausführungen adressierbar sein.
18. Registry-Änderungen MÜSSEN beobachtbar sein.
19. Ungültige oder nicht verfügbare Modelle MÜSSEN aus der aktiven Auswahl ausgeschlossen werden können.
20. Die Registry MUSS auch ohne aktive AI-Ausführung funktionsfähig sein.

## Konsequenzen

### Positive Konsequenzen

- Anwendungen werden von konkreten AI-Modellen entkoppelt,
- Modelle können systemweit entdeckt und ausgetauscht werden,
- lokale und entfernte Modelle verwenden dieselbe Abstraktion,
- Versionen und Hardwarevarianten können parallel existieren,
- automatische Model Selection erhält strukturierte Entscheidungsdaten,
- Trust, Provenance und Ressourcenanforderungen werden systemweit sichtbar.

### Negative Konsequenzen

- Model-Metadaten benötigen ein stabiles Schema,
- unterschiedliche AI-Frameworks benötigen Adapter und Normalisierung,
- Registry-, Model- und Runtime-Versionen müssen konsistent verwaltet werden.

## Abhängigkeiten

- `ADR-AI-0001_Systemweite_AI_Runtime`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Introspection`
- `ADR-CAPABILITY-0001`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0002_Compute_Provider`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-EXECUTION-0006_Trust_Requirements`
- `ADR-EXECUTION-0007_Determinism_Requirements`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-OBSERVABILITY-0009_Architecture_Introspection_als_Systemfunktion`

## Zugehörige NPSPECs

- `NPSPEC-AI-MODEL-REGISTRY-0001`
- `NPSPEC-AI-MODEL-IDENTITY-0001`
- `NPSPEC-AI-MODEL-METADATA-0001`
- `NPSPEC-AI-MODEL-CAPABILITY-0001`
- `NPSPEC-AI-MODEL-VERSIONING-0001`
- `NPSPEC-AI-MODEL-DISCOVERY-0001`

## Ergebnis

Die Model Registry bildet den systemweiten Katalog der für Nova.AI verfügbaren Modelle:

```text
AI Capability Request
        ↓
Model Registry
        ↓
Compatible Models
        ↓
Model Selection
        ↓
Nova.AI Runtime
        ↓
Compute Provider
```

Der zentrale Grundsatz lautet:

```text
Anwendungen kennen Fähigkeiten.

Die Model Registry kennt Modelle.

Die AI Runtime entscheidet,
welches geeignete Modell
eine angeforderte Fähigkeit
konkret bereitstellt.

Registrierung bedeutet dabei
weder Vertrauen noch Berechtigung
noch tatsächliche Verfügbarkeit.
```