# ADR-AI-0001 – Systemweite AI Runtime

## Status

Angenommen

## Kategorie

AI / Runtime / System Architecture

## Kontext

NovaOS soll KI-Funktionen nicht als isolierte Bestandteile einzelner Anwendungen behandeln.

Stattdessen sollen Anwendungen, Systemdienste und Capabilities gemeinsame AI-Funktionen nutzen können.

Beispiele:

```text
Language Models
Vision Models
Speech Models
Embedding Models
Classification
Prediction
Anomaly Detection
Recommendation
Agentic Workflows
```

Ohne gemeinsame Runtime würden Modelle, Hardwarezugriff, Speicherverwaltung, Berechtigungen und Inferenzlogik mehrfach implementiert.

Gleichzeitig gilt das Architekturprinzip:

```text
AI ≠ Voraussetzung für Systemkorrektheit
```

NovaOS muss vollständig grundlegend funktionsfähig bleiben, wenn keine AI Runtime oder kein AI-Modell verfügbar ist.

## Entscheidung

NovaOS erhält eine systemweite **Nova.AI Runtime**.

```text
Applications
System Services
Capabilities
Adaptive Systems
      ↓
Nova.AI API
      ↓
Nova.AI Runtime
      ↓
Model / Provider Selection
      ↓
CPU / GPU / NPU / Remote Provider
```

Die Runtime stellt eine gemeinsame Abstraktion für AI-Ausführung bereit.

## AI Capabilities

Aufrufer verwenden bevorzugt semantische AI Capabilities statt konkrete Modelle.

Beispiele:

```text
AI.Text.Generate
AI.Text.Embed
AI.Text.Classify

AI.Image.Analyze
AI.Image.Generate

AI.Audio.Transcribe
AI.Audio.Synthesize

AI.Predict
AI.Detect.Anomaly
```

Dadurch bleibt die gewünschte Funktion von einem konkreten Modell getrennt.

## Model Abstraction

Modelle werden als austauschbare Implementierungen behandelt.

```text
AI Capability
      ↓
Compatible Models
      ↓
Model Selection
      ↓
Execution Provider
```

Anwendungen sollen normalerweise nicht von einem bestimmten Modellformat oder Framework abhängig sein.

## AI Execution Contract

AI-Aufrufe verwenden die bestehenden Execution Contracts.

Ein Contract kann beispielsweise enthalten:

```text
Capability
Semantic Input Type
Semantic Output Type
Latency
Resource Budget
Energy Budget
Data Sovereignty
Trust Requirement
Determinism
Preferred Model
Forced Model
Preferred Provider
Forced Provider
```

Damit wird AI-Ausführung in die allgemeine NovaOS-Ausführungsarchitektur integriert.

## Model Selection

Die Runtime darf automatisch ein geeignetes Modell auswählen.

```text
AI Request
    ↓
Compatible Models
    ↓
Hard Constraint Filtering
    ↓
Cost / Quality / Performance Evaluation
    ↓
Selected Model
```

Explizit erzwungene Modelle werden als Hard Constraint behandelt.

Bevorzugte Modelle bleiben Soft Constraints.

## Provider Selection

Model und Execution Provider werden getrennt betrachtet.

Ein Modell kann abhängig von seiner Implementierung beispielsweise ausgeführt werden auf:

```text
CPU
SIMD
GPU
NPU
Dedicated Accelerator
Remote Compute
```

Die bestehende Compute Runtime übernimmt die hardwarebezogene Ausführungsplanung.

## Lokale AI

Lokale Ausführung wird als vollwertiger Standardpfad unterstützt.

```text
Application
    ↓
Nova.AI
    ↓
Local Model
    ↓
CPU / GPU / NPU
```

AI-Funktionen dürfen keine Cloud-Abhängigkeit voraussetzen.

## Remote AI

Remote Provider dürfen verwendet werden, wenn der Execution Contract dies erlaubt.

```text
AI Request
    ↓
Sovereignty Check
    ↓
Trust Check
    ↓
Authorization
    ↓
Remote Provider
```

Remote AI ist lediglich ein weiterer Provider und keine Sonderarchitektur.

## Data Sovereignty

AI-Daten unterliegen denselben Sovereignty-Regeln wie andere Daten.

Beispiele:

```text
LocalOnly
DeviceOnly
TrustedCluster
OrganizationOnly
RegionRestricted
SpecificLocation
NoRemote
```

Ein AI-Modell darf diese Grenzen nicht aufgrund höherer Qualität oder Performance umgehen.

## Permissions

AI-Funktionen erhalten keine impliziten Rechte auf Nutzerdaten.

```text
AI Capability
     +
Authorized Object Access
     ↓
Allowed Input
```

Ein Modell darf nur auf Daten zugreifen, die für die konkrete Ausführung autorisiert wurden.

## Model Lifecycle

Die Runtime verwaltet den technischen Lebenszyklus von Modellen.

```text
Discover
Validate
Load
Initialize
Execute
Unload
Update
Rollback
```

Modelle können bei Bedarf geladen und wieder aus Speicher oder Accelerator Memory entfernt werden.

## Shared Model Instances

Mehrere Anwendungen dürfen kompatible Modellinstanzen gemeinsam verwenden.

```text
Application A ─┐
Application B ─┼→ Shared AI Runtime → Model
Application C ─┘
```

Dadurch können Speicherverbrauch, Ladezeiten und Accelerator-Ressourcen reduziert werden.

Isolation und Berechtigungen bleiben trotzdem pro Execution erhalten.

## Resource Management

AI Workloads werden in die normale Ressourcenverwaltung integriert.

Berücksichtigt werden insbesondere:

```text
CPU
RAM
GPU Memory
NPU Memory
Storage
Network
Energy
Execution Time
```

AI Workloads dürfen nicht außerhalb der systemweiten Resource Economy arbeiten.

## Model Metadata

Modelle müssen maschinenlesbare Eigenschaften bereitstellen können.

Beispiele:

```text
Model ID
Version
Capabilities
Input Types
Output Types
Resource Requirements
Supported Providers
Determinism Properties
Trust Information
Provenance
License Metadata
```

Diese Informationen bilden die Grundlage für automatische Auswahl und Planung.

## Model Provenance und Trust

Ein verfügbares Modell ist nicht automatisch vertrauenswürdig.

```text
Available
   ≠
Trusted
   ≠
Authorized
```

Signaturen, Herkunft, Version und Supply-Chain-Provenance können Bestandteil der Trust-Bewertung sein.

## Adaptive Auswahl

NovaOS darf aus realen Ausführungen lernen:

```text
Prediction
    ↓
Model + Provider Selection
    ↓
Execution
    ↓
Measured Result
    ↓
Prediction Error
    ↓
Model Correction
```

Dadurch können zukünftige Auswahlentscheidungen bezüglich Performance, Energie und Ressourcen verbessert werden.

Adaptive Auswahl darf keine Hard Constraints verändern.

## Determinismus

AI-Ausführung kann grundsätzlich nichtdeterministische Eigenschaften besitzen.

Der Execution Contract muss daher festlegen können, ob:

```text
Determinism Not Required
Determinism Preferred
Determinism Required
```

gilt.

Bei erforderlichem Determinismus dürfen ausschließlich geeignete Modelle, Provider und Ausführungsmodi verwendet werden.

## Failure und Fallback

Ist ein AI-Modell oder Provider nicht verfügbar, darf NovaOS:

```text
Select Alternative Model
Select Alternative Provider
Use Non-AI Capability
Degrade Functionality
Return Unavailable
```

Der jeweilige Execution Contract bestimmt, welche Alternativen zulässig sind.

## AI ist keine Kernel-Voraussetzung

Die AI Runtime liegt außerhalb der fundamentalen Kernel-Korrektheit.

```text
Kernel
System Services
Core Capabilities
      ↓
System funktioniert

Nova.AI
      ↓
Optional systemweite Fähigkeit
```

Boot, Speicherverwaltung, Storage, Security, Recovery und grundlegende Systemverwaltung dürfen nicht von einem funktionierenden AI-Modell abhängen.

## Observability

AI-Ausführungen müssen mit der bestehenden Observability korrelierbar sein.

Relevante Informationen sind:

```text
Execution ID
AI Capability
Model ID
Model Version
Provider
Resource Usage
Latency
Decision ID
Fallback
Result Status
```

Sensitive Inputs und Outputs dürfen nicht automatisch in Logs oder Traces geschrieben werden.

## Normative Anforderungen

1. NovaOS SOLL eine systemweite AI Runtime bereitstellen.
2. Anwendungen SOLLEN AI-Funktionen über gemeinsame AI Capabilities verwenden.
3. AI Capabilities MÜSSEN von konkreten Modellen getrennt sein.
4. Modelle MÜSSEN als austauschbare Implementierungen behandelbar sein.
5. AI-Ausführungen MÜSSEN Execution Contracts verwenden können.
6. Model Selection MUSS Hard Constraints berücksichtigen.
7. Model Selection DARF adaptive Optimierung innerhalb des gültigen Lösungsraums verwenden.
8. Model und Execution Provider MÜSSEN getrennte Konzepte bleiben.
9. Lokale AI-Ausführung MUSS ohne Cloud-Abhängigkeit möglich sein.
10. Remote AI MUSS Data-Sovereignty-, Trust- und Authorization-Regeln einhalten.
11. AI-Modelle DÜRFEN keine impliziten Zugriffsrechte auf Nutzerdaten erhalten.
12. AI-Ressourcen MÜSSEN in die systemweite Resource Economy integriert sein.
13. Modelle MÜSSEN maschinenlesbare Capability- und Runtime-Metadaten bereitstellen können.
14. Model Provenance und Trust MÜSSEN bewertbar sein.
15. Shared Model Instances DÜRFEN Isolation und Berechtigungen nicht aufheben.
16. Determinism Requirements MÜSSEN bei AI-Ausführungen berücksichtigt werden.
17. Fallbacks DÜRFEN keine Hard Constraints abschwächen.
18. AI-Ausführungen MÜSSEN mit Observability und Decision Tracing korrelierbar sein.
19. Sensitive AI-Eingaben und -Ausgaben DÜRFEN nicht standardmäßig geloggt werden.
20. NovaOS MUSS ohne verfügbare AI Runtime oder AI-Modelle grundlegend funktionsfähig bleiben.

## Konsequenzen

### Positive Konsequenzen

- Anwendungen benötigen keine eigenen AI-Runtimes,
- Modelle und Hardware können systemweit gemeinsam verwaltet werden,
- lokale und entfernte AI verwenden dieselbe Architektur,
- GPU-, NPU- und Speicherressourcen können effizient geteilt werden,
- Security, Privacy und Sovereignty gelten einheitlich für AI,
- Modelle können unabhängig von Anwendungen ausgetauscht werden.

### Negative Konsequenzen

- die Runtime benötigt Model-, Provider- und Ressourcenmanagement,
- unterschiedliche AI-Frameworks benötigen Adapter,
- Modellkompatibilität und Lifecycle erhöhen die Systemkomplexität.

## Abhängigkeiten

- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0020_KI_nicht_Kernel_Voraussetzung`
- `ADR-CAPABILITY-0001`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-COMPUTE-0002_Compute_Provider`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-EXECUTION-0005_Data_Sovereignty`
- `ADR-EXECUTION-0006_Trust_Requirements`
- `ADR-EXECUTION-0007_Determinism_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-AUTONOMY-0008_Autonomie_unter_harten_Safety_Security_und_User_Constraints`

## Zugehörige NPSPECs

- `NPSPEC-AI-RUNTIME-0001`
- `NPSPEC-AI-CAPABILITY-0001`
- `NPSPEC-AI-MODEL-0001`
- `NPSPEC-AI-MODEL-SELECTION-0001`
- `NPSPEC-AI-PROVIDER-0001`
- `NPSPEC-AI-EXECUTION-CONTRACT-0001`

## Ergebnis

NovaOS erhält eine gemeinsame AI-Ausführungsschicht:

```text
Application / System Capability
            ↓
         Nova.AI
            ↓
      AI Capability
            ↓
      Model Selection
            ↓
     Provider Selection
            ↓
CPU / GPU / NPU / Remote
```

Der zentrale Grundsatz lautet:

```text
AI ist in NovaOS
eine systemweite Capability,
keine Sammlung isolierter
Anwendungsbibliotheken.

Anwendungen beschreiben,
welche AI-Funktion sie benötigen.

NovaOS entscheidet innerhalb
der geltenden Contracts,
welches Modell und welche Hardware
diese Funktion ausführen.

AI erweitert das System –
aber das System hängt
für seine Korrektheit nicht von AI ab.
```