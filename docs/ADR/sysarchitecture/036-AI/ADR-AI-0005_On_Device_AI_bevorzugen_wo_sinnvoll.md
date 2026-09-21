# ADR-AI-0005 – On-Device AI bevorzugen, wo sinnvoll

## Status

Angenommen

## Kategorie

AI / Execution / Privacy / Locality

## Kontext

NovaOS unterstützt lokale und entfernte AI Provider.

Viele AI-Aufgaben können direkt auf dem Gerät ausgeführt werden:

```text
Text Generation
Embeddings
Classification
Speech Recognition
Image Analysis
Prediction
Anomaly Detection
Personal Assistance
```

Lokale Ausführung bietet Vorteile bei Datenschutz, Latenz, Offline-Fähigkeit und Datenbewegung.

Remote AI kann dagegen sinnvoll sein, wenn lokale Ressourcen nicht ausreichen oder ein entferntes Modell deutlich geeigneter ist.

NovaOS soll daher weder grundsätzlich:

```text
Local Only
```

noch:

```text
Cloud First
```

verwenden.

## Entscheidung

NovaOS **bevorzugt On-Device AI, wenn sie die Anforderungen des Execution Contracts sinnvoll erfüllt**.

```text
AI Request
    ↓
Execution Contract
    ↓
Valid Providers
    ↓
Local Provider geeignet?
 ├── Yes → Prefer On-Device
 └── No  → Evaluate Other Providers
```

On-Device ist eine bevorzugte Ausführungsstrategie, aber kein universeller Hard Constraint.

## Locality Preference

Bei ansonsten geeigneten Alternativen erhält lokale Ausführung einen Vorteil.

```text
Local Model
Remote Model
     ↓
Both Valid
     ↓
Prefer Local
```

Dabei werden insbesondere berücksichtigt:

```text
Latency
Privacy
Data Sovereignty
Network Cost
Energy
Model Quality
Resource Usage
Availability
```

## Hard Constraints

Hard Constraints haben Vorrang vor der Locality Preference.

Beispiel:

```text
LocalOnly
```

bedeutet:

```text
Remote Execution = Invalid
```

Dagegen bedeutet:

```text
PreferLocal
```

lediglich:

```text
Local Execution = Preferred
```

Diese beiden Fälle dürfen nicht verwechselt werden.

## Gründe für On-Device AI

On-Device AI soll bevorzugt werden, wenn dadurch relevante Vorteile entstehen:

```text
Lower Latency
Offline Availability
Reduced Data Transfer
Better Privacy
Lower Network Dependency
Immediate Interaction
Local Context Processing
Predictable Availability
```

Besonders sensible oder persönliche Daten sollen möglichst lokal verarbeitet werden, sofern die Anforderungen lokal erfüllt werden können.

## Gründe für Remote AI

Remote AI darf gewählt werden, wenn sie innerhalb aller Constraints sinnvoller ist.

Beispiele:

```text
Model too large
Insufficient RAM
Insufficient VRAM
Missing Accelerator
Local Deadline impossible
Required Model unavailable locally
Remote execution significantly more efficient
Explicit User Request
```

Remote Execution bleibt vollständig an Sovereignty, Trust und Authorization gebunden.

## Provider Selection

Die Entscheidung wird in die allgemeine Execution Planning Architektur integriert.

```text
AI Capability
      ↓
Model Candidates
      ↓
Provider Candidates
      ↓
Hard Constraint Filtering
      ↓
Locality Preference
      ↓
Cost Evaluation
      ↓
Execution Plan
```

Es entsteht kein separater AI-Scheduler nur für Local-vs-Remote-Entscheidungen.

## Compute-to-Data

Bei großen oder sensiblen Datenmengen soll bevorzugt Compute zu den Daten bewegt werden.

```text
Large Local Data
      +
AI Capability
      ↓
Local Execution Preferred
```

Dadurch werden unnötige Datenübertragungen vermieden.

## Privacy

On-Device AI reduziert die Notwendigkeit, Nutzerdaten an externe Systeme zu übertragen.

Dies ersetzt jedoch keine normalen Berechtigungsregeln.

```text
Local Execution
      ≠
Automatic Data Access
```

Auch lokale Modelle benötigen die erforderliche Authorization für ihre Eingabedaten.

## Data Sovereignty

Data Sovereignty ist ein Hard Constraint.

Beispiele:

```text
LocalOnly
DeviceOnly
NoRemote
RegionRestricted
TrustedCluster
```

Kann ein Remote Provider diese Anforderungen nicht erfüllen, wird er vor der Optimierung aus dem gültigen Lösungsraum entfernt.

## Offline-Fähigkeit

Wesentliche AI-Funktionen sollen nach Möglichkeit lokal verfügbar sein.

```text
Network Available
      ↓
Local + Remote

Network Unavailable
      ↓
Local AI remains available
```

NovaOS darf jedoch nicht behaupten, eine AI Capability offline bereitstellen zu können, wenn kein geeignetes lokales Modell vorhanden ist.

## Resource Awareness

Lokale AI darf das Gesamtsystem nicht unangemessen beeinträchtigen.

Berücksichtigt werden:

```text
CPU Load
RAM
VRAM
NPU Memory
Battery
Thermal State
Energy
Interactive Workloads
```

Ein lokaler Provider kann deshalb trotz Locality Preference ungeeignet sein.

## Energy Awareness

On-Device ist nicht automatisch energieeffizienter.

NovaOS bewertet:

```text
Local Compute Energy

versus

Network Transfer
+
Remote Compute
```

insbesondere auf mobilen oder batteriebetriebenen Geräten.

## Model Quality

Locality darf erforderliche Qualitätsanforderungen nicht unterlaufen.

Wenn ein Execution Contract eine Eigenschaft verlangt, die kein lokales Modell erfüllt:

```text
Local Models
     ↓
Requirements not satisfied
     ↓
Remote Candidate Evaluation
```

Ein kleineres lokales Modell darf nicht allein wegen seiner Lokalität als gleichwertig behandelt werden.

## Hybrid Execution

AI-Pipelines dürfen lokale und entfernte Verarbeitung kombinieren.

Beispiel:

```text
Local Preprocessing
       ↓
Privacy Filtering
       ↓
Remote AI
       ↓
Local Postprocessing
```

Dabei gelten Constraints für jeden einzelnen Datenfluss.

## Nutzerentscheidung

Nutzer können die Locality explizit beeinflussen.

Beispiele:

```text
Always Local
Prefer Local
Allow Remote
Ask Before Remote
Specific Provider
```

Solche Entscheidungen werden über Execution Contracts und Policies abgebildet.

## Adaptive Auswahl

NovaOS darf lernen, wann lokale oder entfernte Ausführung besser geeignet ist.

```text
Prediction
    ↓
Local / Remote Selection
    ↓
Measured Result
    ↓
Prediction Error
    ↓
Model Correction
```

Gemessen werden können:

```text
Latency
Energy
Resource Usage
Transfer Cost
Availability
```

Adaptive Auswahl bleibt innerhalb aller Hard Constraints.

## Fallback

Fällt der bevorzugte lokale Provider aus, darf NovaOS einen anderen Provider wählen, sofern der Contract dies erlaubt.

```text
Local Provider Failed
        ↓
Contract Check
        ↓
Alternative Local Provider
        ↓
Remote Provider
        ↓
Unavailable
```

Remote Fallback darf niemals implizit einen `LocalOnly`- oder `NoRemote`-Constraint umgehen.

## Observability

NovaOS muss relevante Local-vs-Remote-Entscheidungen nachvollziehbar machen können.

Beispiele:

```text
Selected Model
Selected Provider
Execution Location
Local Candidates
Remote Candidates
Rejected Candidates
Transfer Cost
Resource Cost
Decision Reason
```

Decision Tracing bildet hierfür die Grundlage.

## Normative Anforderungen

1. NovaOS SOLL On-Device AI bevorzugen, wenn lokale Ausführung sinnvoll und contract-konform ist.
2. On-Device Preference DARF nicht automatisch als Hard Constraint behandelt werden.
3. Explizite `LocalOnly`-, `DeviceOnly`- oder `NoRemote`-Constraints MÜSSEN als Hard Constraints behandelt werden.
4. Hard Constraints MÜSSEN vor der Locality Optimization ausgewertet werden.
5. Lokale AI MUSS dieselben Authorization-Regeln wie Remote AI einhalten.
6. Data Sovereignty MUSS bei AI-Ausführungen verbindlich berücksichtigt werden.
7. NovaOS SOLL unnötige Übertragung sensibler Daten vermeiden.
8. Compute-to-Data SOLL bei geeigneten AI-Workloads berücksichtigt werden.
9. Lokale Ausführung DARF erforderliche Qualitätsanforderungen nicht unterschreiten.
10. Lokale Ressourcen- und Energiebelastung MUSS bei der Provider Selection berücksichtigt werden.
11. On-Device DARF nicht grundsätzlich als energieeffizienter angenommen werden.
12. Wesentliche AI Capabilities SOLLEN soweit sinnvoll offline verfügbar sein.
13. Hybrid AI Pipelines MÜSSEN unterstützt werden können.
14. Nutzer MÜSSEN Remote AI einschränken oder vollständig verbieten können.
15. Adaptive Provider Selection DARF nur innerhalb des gültigen Constraint Space erfolgen.
16. Remote Fallback DARF Locality Hard Constraints nicht abschwächen.
17. Der Ausfall eines lokalen Providers SOLL alternatives Execution Planning auslösen können.
18. Local-vs-Remote-Entscheidungen SOLLEN über Decision Tracing erklärbar sein.
19. AI Provider Selection MUSS in die allgemeine NovaOS Execution Planning Architektur integriert sein.
20. NovaOS DARF für seine grundlegende Funktion weder lokale noch entfernte AI voraussetzen.

## Konsequenzen

### Positive Konsequenzen

- sensible Daten bleiben häufiger auf dem Gerät,
- viele AI-Funktionen funktionieren ohne Netzwerk,
- Netzwerkabhängigkeit und Datenübertragung werden reduziert,
- lokale Beschleuniger können effizient genutzt werden,
- Remote AI bleibt für anspruchsvolle Workloads verfügbar,
- Locality wird mit Ressourcen-, Energie- und Qualitätsanforderungen abgewogen.

### Negative Konsequenzen

- lokale Modelle benötigen Storage, RAM und Accelerator Memory,
- Model Scheduling wird stärker von Hardware und Systemzustand abhängig,
- Local-vs-Remote-Entscheidungen benötigen zusätzliche Kostenmodelle.

## Abhängigkeiten

- `ADR-AI-0001_Systemweite_AI_Runtime`
- `ADR-AI-0002_Model_Registry`
- `ADR-AI-0003_Model_Scheduling_und_Caching`
- `ADR-AI-0004_Model_Sharing`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-COMPUTE-0003_Automatische_Accelerator_Auswahl`
- `ADR-COMPUTE-0005_Energy_Aware_Compute`
- `ADR-COMPUTE-0007_Distributed_Compute`
- `ADR-EXECUTION-0005_Data_Sovereignty`
- `ADR-EXECUTION-0006_Trust_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-ADAPTIVE-0008_Adaptive_Power_Management`
- `ADR-ADAPTIVE-0009_Adaptive_Networking`
- `ADR-AUTONOMY-0008_Autonomie_unter_harten_Safety_Security_und_User_Constraints`

## Zugehörige NPSPECs

- `NPSPEC-AI-ON-DEVICE-0001`
- `NPSPEC-AI-LOCALITY-PREFERENCE-0001`
- `NPSPEC-AI-REMOTE-FALLBACK-0001`
- `NPSPEC-AI-HYBRID-EXECUTION-0001`
- `NPSPEC-AI-OFFLINE-EXECUTION-0001`

## Ergebnis

NovaOS behandelt lokale AI als bevorzugten, aber nicht erzwungenen Ausführungspfad:

```text
AI Request
    ↓
Hard Constraints
    ↓
Valid Models + Providers
    ↓
Prefer On-Device
    ↓
Evaluate Quality / Latency /
Resources / Energy / Locality
    ↓
Best Valid Execution Plan
```

Der zentrale Grundsatz lautet:

```text
Wenn AI sinnvoll lokal laufen kann,
soll sie lokal laufen.

Nicht weil lokal immer besser ist,
sondern weil Datenlokalität,
Offline-Fähigkeit und geringe
Netzwerkabhängigkeit wertvoll sind.

Remote AI bleibt verfügbar,
wenn sie erforderlich oder sinnvoller ist
und alle Constraints dies erlauben.
```