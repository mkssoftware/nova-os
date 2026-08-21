# NPSPEC-COMPUTE-MEDAI-0001 — Medical AI Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Medical AI  
**Komponente:** NovaOS Medical AI Compute  
**Bezeichner:** `NPSPEC-COMPUTE-MEDAI-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`, `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-DICOM-0001`, `NPSPEC-COMPUTE-RADIOLOGY-0001`, `NPSPEC-COMPUTE-PACS-0001`, `NPSPEC-COMPUTE-ULTRASOUND-0001`  
**Kurzname:** Medical AI Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Medical-AI-Compute-Infrastruktur der NovaOS-Compute-Architektur.

Medical AI Compute stellt eine standardisierte Ausführungs-, Daten-, Sicherheits-, Provenance- und Integrationsschicht für KI-gestützte Verarbeitung medizinischer Daten bereit.

Die Komponente ist für medizinische KI-Workloads vorgesehen, beispielsweise:

- medizinische Bildanalyse,
- Segmentierung,
- Detektion,
- Klassifikation,
- Landmark Detection,
- quantitative Analyse,
- Priorisierung,
- Bildrekonstruktion,
- Bildverbesserung,
- Ultraschallanalyse,
- Signalverarbeitung,
- Zeitreihenanalyse,
- multimodale Analyse,
- klinische Sprachverarbeitung,
- medizinische Embeddings,
- Anomalieerkennung,
- Entscheidungsunterstützung,
- Qualitätskontrolle.

Die grundlegende Architektur lautet:

```text
Medical Data
     |
     v
Input Validation
     |
     v
Preprocessing
     |
     v
Medical AI Runtime
     |
 +---+---+---+
 |       |   |
 v       v   v
CPU     GPU NPU
 |       |   |
 +---+---+---+
     |
     v
AI Model
     |
     v
Postprocessing
     |
     v
Validation
     |
     v
Medical AI Result
```

Medical AI Compute MUSS KI-Ausführung von medizinischer Diagnose und Therapieentscheidung trennen.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Ein medizinisches KI-Ergebnis ist ein nachvollziehbares, versioniertes und qualifiziertes Compute-Ergebnis und keine implizit als wahr angenommene medizinische Feststellung.

Jedes relevante Ergebnis MUSS auf seine:

```text
Input Data
Model
Model Version
Configuration
Preprocessing
Inference Backend
Postprocessing
Runtime
```

zurückgeführt werden können.

---

## 3. Sicherheitsgrundsatz

Medical AI Compute DARF ein KI-Ergebnis nicht allein aufgrund der Ausgabe eines Modells als medizinisch korrekt kennzeichnen.

---

## 4. Human Oversight

Für klinisch relevante Workflows MUSS die Architektur menschliche Überprüfung ermöglichen.

---

## 5. Automation Boundary

Die Grenze zwischen:

```text
AI Suggestion
AI Measurement
AI Classification
AI Decision Support
Clinical Decision
Automated Action
```

MUSS explizit modellierbar sein.

---

## 6. Ziele

Medical AI Compute MUSS architektonisch mindestens folgende Bereiche unterstützen:

1. Model Loading,
2. Model Validation,
3. Model Versioning,
4. Model Metadata,
5. Model Provenance,
6. CPU Inference,
7. GPU Inference,
8. NPU Inference,
9. Batch Inference,
10. Streaming Inference,
11. Real-Time Inference,
12. Image AI,
13. Volume AI,
14. Video/Cine AI,
15. Signal AI,
16. Time-Series AI,
17. Text AI,
18. Multimodal AI,
19. Segmentation,
20. Detection,
21. Classification,
22. Regression,
23. Landmark Detection,
24. Reconstruction,
25. Denoising,
26. Enhancement,
27. Registration Assistance,
28. Measurement Assistance,
29. Anomaly Detection,
30. Embeddings,
31. Uncertainty,
32. Confidence,
33. Calibration,
34. Explainability,
35. Provenance,
36. Auditability,
37. Privacy,
38. De-Identification,
39. Access Control,
40. Monitoring,
41. Drift Detection,
42. Quality Control,
43. Fail-Safe-Verhalten,
44. Reproduzierbarkeit,
45. PACS-Integration,
46. DICOM-Integration,
47. Radiology-Integration,
48. Ultrasound-Integration,
49. Distributed Compute,
50. Erweiterbarkeit.

---

## 7. Nicht-Ziele

Diese Spezifikation definiert nicht:

- konkrete medizinische Diagnosen,
- konkrete Therapieentscheidungen,
- konkrete zugelassene Medizinprodukte,
- regulatorische Zulassung eines konkreten Modells,
- konkrete neuronale Netzarchitekturen,
- konkrete Trainingsdatensätze,
- konkrete Foundation Models,
- konkrete LLMs,
- konkrete Benutzeroberflächen,
- autonome medizinische Behandlung.

---

## 8. Medical AI Model

Ein medizinisches KI-Modell wird als:

`MedicalAIModel`

repräsentiert.

---

## 9. Model Identity

Jedes Modell MUSS eine eindeutige Modellidentität besitzen.

---

## 10. Model ID

Die logische Identität wird als:

`ModelId`

repräsentiert.

---

## 11. Model Version

Jede freigegebene Modellversion MUSS eindeutig identifizierbar sein.

---

## 12. Immutable Model Version

Eine veröffentlichte Modellversion SOLL unveränderlich sein.

Eine Änderung erzeugt eine neue Version.

---

## 13. Model Artifact

Die ausführbaren Modelldaten werden als:

`MedicalAIModelArtifact`

repräsentiert.

---

## 14. Model Hash

Ein Modellartefakt MUSS kryptographisch fingerprintbar sein.

---

## 15. Model Integrity

Vor Verwendung SOLL die Integrität eines Modellartefakts überprüft werden.

---

## 16. Model Metadata

Ein Modell MUSS strukturierte Metadaten besitzen können.

---

## 17. Required Model Metadata

Mindestens folgende Felder SOLLEN unterstützt werden:

```text
ModelId
ModelName
ModelVersion
ModelHash
ModelFormat
TaskType
InputSpecification
OutputSpecification
RuntimeRequirements
BackendRequirements
Precision
CreatedAt
Publisher
```

---

## 18. Extended Medical Metadata

Zusätzlich SOLLEN medizinisch relevante Metadaten unterstützt werden:

```text
IntendedUse
TargetPopulation
SupportedModalities
SupportedBodyRegions
SupportedProtocols
Contraindications
KnownLimitations
ValidationScope
PerformanceCharacteristics
```

---

## 19. Intended Use

Der vorgesehene Verwendungszweck SOLL maschinenlesbar repräsentierbar sein.

---

## 20. Intended Use Enforcement

Die Runtime SOLL prüfen können, ob Eingabedaten grundsätzlich mit dem deklarierten Verwendungsbereich kompatibel sind.

---

## 21. Out-of-Scope Input

Eingaben außerhalb des deklarierten Modellbereichs SOLLEN erkannt werden können.

---

## 22. Out-of-Scope Result

Ein Out-of-Scope-Zustand MUSS von einer normalen negativen Modellantwort unterscheidbar sein.

---

## 23. Model Format

Medical AI Compute SOLL mehrere Modellformate unterstützen können.

---

## 24. Format Independence

Höhere Medical-AI-Schichten DÜRFEN nicht von einem einzelnen Modellformat abhängig sein.

---

## 25. Backend Compilation

Modelle DÜRFEN für konkrete Compute Backends optimiert oder kompiliert werden.

---

## 26. Compiled Artifact

Ein backendoptimiertes Modellartefakt MUSS auf das ursprüngliche Modell zurückführbar sein.

---

## 27. Model Registry

Medical AI Compute SOLL mit einer Modellregistrierung integrierbar sein.

---

## 28. Model Registry Responsibilities

Eine Modellregistrierung SOLL verwalten können:

```text
Model Identity
Version
Artifact
Hash
Metadata
Compatibility
Approval State
Deployment State
```

---

## 29. Model State

Ein Modell SOLL mindestens folgende Zustände besitzen können:

```text
DEVELOPMENT
VALIDATION
APPROVED
DEPLOYED
DEPRECATED
REVOKED
```

---

## 30. Revoked Model

Ein als `REVOKED` markiertes Modell DARF nicht automatisch für neue klinisch relevante Inferenz verwendet werden.

---

## 31. Model Approval Separation

Technische Installierbarkeit und medizinische Freigabe MÜSSEN getrennte Zustände sein.

---

## 32. Model Loading

Medical AI Compute MUSS Modelle kontrolliert laden können.

---

## 33. Lazy Loading

Modelle SOLLEN bei Bedarf geladen werden können.

---

## 34. Preloading

Häufig verwendete Modelle SOLLEN vorgeladen werden können.

---

## 35. Model Cache

Geladene Modelle SOLLEN in einem kontrollierten Cache gehalten werden können.

---

## 36. Model Cache Eviction

Modelle SOLLEN bei Ressourcenbedarf entladen werden können.

---

## 37. Model Pinning

Latenzkritische Modelle SOLLEN im Speicher fixiert werden können.

---

## 38. Model Warmup

Modelle SOLLEN vor produktiver Inferenz Warmup-Durchläufe durchführen können.

---

## 39. Warmup Separation

Warmup-Ergebnisse DÜRFEN nicht als medizinische Ergebnisse veröffentlicht werden.

---

## 40. Medical AI Task

Eine KI-Aufgabe wird als:

`MedicalAITask`

repräsentiert.

---

## 41. Task Types

Mindestens folgende Task-Typen SOLLEN unterstützt werden:

```text
CLASSIFICATION
REGRESSION
SEGMENTATION
DETECTION
LANDMARK_DETECTION
ANOMALY_DETECTION
RECONSTRUCTION
DENOISING
ENHANCEMENT
REGISTRATION
MEASUREMENT
EMBEDDING
TEXT_GENERATION
TEXT_CLASSIFICATION
INFORMATION_EXTRACTION
MULTIMODAL_ANALYSIS
CUSTOM
```

---

## 42. Input Object

Eine Modelleingabe wird als:

`MedicalAIInput`

repräsentiert.

---

## 43. Output Object

Eine Modellausgabe wird als:

`MedicalAIOutput`

repräsentiert.

---

## 44. Result Object

Nach Postprocessing und Validierung entsteht:

`MedicalAIResult`

---

## 45. Input Types

Medical AI Compute SOLL mindestens folgende Eingabetypen unterstützen können:

```text
Image
ImageSeries
Volume
VolumeSequence
Cine
Waveform
Signal
TimeSeries
Tensor
Text
StructuredData
Metadata
MultiModalInput
```

---

## 46. DICOM Input

DICOM-basierte Eingaben SOLLEN über `NPSPEC-COMPUTE-DICOM-0001` bereitgestellt werden.

---

## 47. Radiology Input

Radiologische Bild- und Volumendaten SOLLEN über `NPSPEC-COMPUTE-RADIOLOGY-0001` bereitgestellt werden.

---

## 48. Ultrasound Input

Ultraschalldaten SOLLEN über `NPSPEC-COMPUTE-ULTRASOUND-0001` bereitgestellt werden.

---

## 49. PACS Input

Studien und Serien SOLLEN über `NPSPEC-COMPUTE-PACS-0001` geladen werden können.

---

## 50. Input Validation

Alle Modelleingaben MÜSSEN vor Inferenz validiert werden können.

---

## 51. Shape Validation

Tensor- und Bilddimensionen MÜSSEN gegen die Model Specification geprüft werden.

---

## 52. Data Type Validation

Datentypen MÜSSEN validiert werden.

---

## 53. Modality Validation

Die Modalität SOLL gegen die unterstützten Modalitäten des Modells geprüft werden können.

---

## 54. Body Region Validation

Falls relevant, SOLL die Körperregion validiert werden können.

---

## 55. Protocol Validation

Falls ein Modell protokollspezifisch ist, SOLL die Eingabe gegen unterstützte Protokolle geprüft werden.

---

## 56. Geometry Validation

Räumliche Modelleingaben MÜSSEN eine valide Geometrie besitzen, wenn das Modell diese voraussetzt.

---

## 57. Temporal Validation

Zeitabhängige Modelleingaben MÜSSEN eine valide zeitliche Ordnung besitzen.

---

## 58. Missing Data

Fehlende Daten MÜSSEN explizit behandelbar sein.

---

## 59. Missing Data Policy

Ein Modell SOLL deklarieren können, ob fehlende Werte:

```text
REJECT
MASK
IMPUTE
ALLOW
```

behandelt werden.

---

## 60. Input Completeness

Die Runtime SOLL prüfen können, ob alle erforderlichen Eingabekomponenten vorhanden sind.

---

## 61. Input Quality

Ein Modell SOLL Mindestanforderungen an Eingabequalität deklarieren können.

---

## 62. Quality Gate

Eine optionale Quality-Gate-Stufe SOLL vor der eigentlichen Inferenz ausgeführt werden können.

---

## 63. Quality Failure

Unzureichende Eingabequalität MUSS von einem normalen Modellergebnis unterscheidbar sein.

---

## 64. Preprocessing

Medical AI Compute MUSS standardisierte Preprocessing-Pipelines unterstützen.

---

## 65. Preprocessing Pipeline

Eine typische Pipeline lautet:

```text
Input
  |
  v
Validation
  |
  v
Decode
  |
  v
Geometry Normalization
  |
  v
Intensity Normalization
  |
  v
Resampling
  |
  v
Cropping / Padding
  |
  v
Tensor Conversion
  |
  v
Model Input
```

---

## 66. Preprocessing Identity

Eine Preprocessing-Konfiguration MUSS versionierbar sein.

---

## 67. Preprocessing Provenance

Alle relevanten Preprocessing-Schritte SOLLEN im Provenance Record gespeichert werden.

---

## 68. Resampling

Räumliche Daten SOLLEN auf die vom Modell erwartete Auflösung resampled werden können.

---

## 69. Interpolation

Die verwendete Interpolationsmethode MUSS nachvollziehbar sein.

---

## 70. Cropping

Cropping SOLL unterstützt werden.

---

## 71. Padding

Padding SOLL unterstützt werden.

---

## 72. Normalization

Normalisierung SOLL explizit konfigurierbar sein.

---

## 73. Normalization Parameters

Normalisierungsparameter SOLLEN gespeichert werden können.

---

## 74. Windowing

Medizinisches Image Windowing SOLL als expliziter Preprocessing-Schritt verwendet werden können.

---

## 75. Windowing Separation

Windowing DARF quantitative Quelldaten nicht überschreiben.

---

## 76. Orientation Normalization

Bildorientierungen SOLLEN vor Inferenz standardisiert werden können.

---

## 77. Coordinate Preservation

Transformationen MÜSSEN so dokumentiert werden, dass Ergebnisse auf die ursprüngliche Geometrie zurücktransformiert werden können.

---

## 78. Tensor Conversion

Medizinische Daten SOLLEN in Runtime-Tensoren überführt werden können.

---

## 79. Tensor Layout

Tensor Layout MUSS explizit definiert sein.

Beispiele:

```text
NCHW
NHWC
NCDHW
NDHWC
Custom
```

---

## 80. Batch Dimension

Batch-Dimensionen SOLLEN explizit behandelt werden.

---

## 81. Channel Semantics

Kanäle MÜSSEN semantisch definiert sein.

---

## 82. Multi-Channel Input

Mehrkanalige Eingaben SOLLEN unterstützt werden.

---

## 83. Multi-Modal Input

Medical AI Compute SOLL multimodale Modelle unterstützen.

---

## 84. Multi-Modal Example

```text
CT
 +
MR
 +
Clinical Metadata
 +
Text
 |
 v
Multimodal Model
```

---

## 85. Input Synchronization

Zusammengehörige multimodale Daten MÜSSEN eindeutig miteinander verknüpft werden.

---

## 86. Spatial Registration Requirement

Ein Modell SOLL deklarieren können, ob mehrere räumliche Eingaben registriert sein müssen.

---

## 87. Temporal Alignment

Zeitabhängige multimodale Eingaben SOLLEN zeitlich ausgerichtet werden können.

---

## 88. Inference

Die eigentliche Modellausführung wird als:

`MedicalAIInference`

bezeichnet.

---

## 89. Inference Request

Eine Inferenzanforderung wird als:

`MedicalAIInferenceRequest`

repräsentiert.

---

## 90. Request ID

Jede Inferenz SOLL eine eindeutige Request ID besitzen.

---

## 91. Correlation ID

Zusammengehörige Verarbeitungsschritte SOLLEN über eine Correlation ID verbunden werden.

---

## 92. Inference Backend

Ein Inferenzbackend wird als:

`MedicalAIBackend`

repräsentiert.

---

## 93. CPU Backend

CPU-Inferenz MUSS unterstützt werden können.

---

## 94. GPU Backend

GPU-Inferenz SOLL über `NPSPEC-COMPUTE-RUNTIME-0005` unterstützt werden.

---

## 95. NPU Backend

NPU-Inferenz SOLL über `NPSPEC-COMPUTE-RUNTIME-0006` unterstützt werden.

---

## 96. Backend Selection

Die Runtime SOLL automatisch ein geeignetes Backend auswählen können.

---

## 97. Explicit Backend

Ein autorisierter Aufrufer SOLL ein Backend explizit anfordern können.

---

## 98. Backend Capability

Backends MÜSSEN ihre Fähigkeiten deklarieren können.

---

## 99. Backend Compatibility

Vor Ausführung MUSS geprüft werden können, ob Modell und Backend kompatibel sind.

---

## 100. Backend Fallback

Bei Ausfall eines bevorzugten Backends SOLL ein kompatibles alternatives Backend verwendet werden können.

---

## 101. Fallback Visibility

Ein Backend-Fallback MUSS im Provenance Record sichtbar sein.

---

## 102. Precision

Die verwendete numerische Präzision MUSS dokumentierbar sein.

---

## 103. Supported Precision

Abhängig vom Modell SOLLEN unterstützt werden können:

```text
FP64
FP32
TF32
BF16
FP16
INT8
Custom
```

---

## 104. Quantized Model

Quantisierte Modelle MÜSSEN als solche identifizierbar sein.

---

## 105. Quantization Metadata

Quantisierungsparameter SOLLEN nachvollziehbar sein.

---

## 106. Precision Validation

Eine niedrigere Präzision DARF nicht stillschweigend verwendet werden, wenn sie außerhalb der freigegebenen Modellkonfiguration liegt.

---

## 107. Batch Inference

Medical AI Compute SOLL Batch Inference unterstützen.

---

## 108. Dynamic Batching

Mehrere kompatible Requests SOLLEN dynamisch zu Batches kombiniert werden können.

---

## 109. Batch Isolation

Ergebnisse verschiedener Requests MÜSSEN eindeutig getrennt bleiben.

---

## 110. Batch Timeout

Dynamic Batching SOLL ein maximales Wartezeitbudget besitzen.

---

## 111. Streaming Inference

Medical AI Compute SOLL Streaming Inference unterstützen.

---

## 112. Streaming State

Zustandsbehaftete Modelle SOLLEN ihren Inferenzzustand zwischen Datenblöcken erhalten können.

---

## 113. Stream Identity

Jeder Stream MUSS eindeutig identifizierbar sein.

---

## 114. Stream Reset

Ein Modellstream MUSS kontrolliert zurückgesetzt werden können.

---

## 115. Real-Time Inference

Latenzkritische medizinische KI-Pipelines SOLLEN Real-Time Inference unterstützen.

---

## 116. Deadline

Ein Inference Request SOLL eine Deadline deklarieren können.

---

## 117. Deadline-Aware Scheduling

Der Compute Scheduler SOLL Deadlines berücksichtigen.

---

## 118. Deadline Miss

Eine verpasste Deadline MUSS diagnostizierbar sein.

---

## 119. No Silent Stale Result

Ein verspätetes Ergebnis DARF nicht ohne Kennzeichnung als rechtzeitiges Echtzeitergebnis behandelt werden.

---

## 120. Cancellation

Inference Requests MÜSSEN kontrolliert abbrechbar sein.

---

## 121. Cancellation State

Ein abgebrochener Request MUSS eindeutig als:

`CANCELLED`

identifizierbar sein.

---

## 122. Partial Result

Unvollständige Ergebnisse DÜRFEN nicht als vollständige medizinische KI-Ergebnisse veröffentlicht werden.

---

## 123. Scheduler Integration

Medical AI Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 124. Task Graph

Eine Medical-AI-Pipeline SOLL als Task Graph repräsentiert werden können.

Beispiel:

```text
Load
 |
 v
Validate
 |
 v
Preprocess
 |
 v
Infer
 |
 v
Postprocess
 |
 v
Validate Result
 |
 v
Publish
```

---

## 125. Task Dependencies

Task-Abhängigkeiten MÜSSEN explizit darstellbar sein.

---

## 126. Priority

Inference Requests SOLLEN Prioritäten besitzen können.

---

## 127. Priority Classes

Mindestens folgende Klassen SOLLEN unterstützt werden können:

```text
BACKGROUND
NORMAL
INTERACTIVE
URGENT
REALTIME
```

---

## 128. Starvation Prevention

Niedrig priorisierte Jobs DÜRFEN nicht unbegrenzt verhungern.

---

## 129. Resource Reservation

Kritische Modelle SOLLEN Ressourcen reservieren können.

---

## 130. Resource Quota

Modelle beziehungsweise Dienste SOLLEN Ressourcenlimits besitzen können.

---

## 131. Memory Model

Medical AI Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 132. Model Memory

Modelldaten SOLLEN getrennt von Request-Daten verwaltet werden.

---

## 133. Input Memory

Eingabetensoren SOLLEN explizite Speicherobjekte verwenden.

---

## 134. Output Memory

Ausgabetensoren SOLLEN explizite Speicherobjekte verwenden.

---

## 135. Workspace Memory

Temporärer Backend-Workspace SOLL separat budgetiert werden.

---

## 136. Memory Budget

Ein Modell SOLL seinen erwarteten Speicherbedarf deklarieren können.

---

## 137. Peak Memory

Peak Memory SOLL messbar sein.

---

## 138. Memory Pressure

Bei Speicherknappheit MUSS ein definiertes Verhalten existieren.

---

## 139. Model Eviction Under Pressure

Nicht fixierte Modelle DÜRFEN bei Speicherknappheit entladen werden.

---

## 140. Request Rejection

Ein Request DARF kontrolliert abgewiesen werden, wenn sichere Ausführung aufgrund fehlender Ressourcen nicht möglich ist.

---

## 141. Zero-Copy

Unnötige Datenkopien SOLLEN vermieden werden.

---

## 142. Shared Tensor

Geeignete Compute-Komponenten SOLLEN Tensoren ohne vollständige Kopie austauschen können.

---

## 143. Device-Local Data

Daten SOLLEN möglichst auf dem Compute Device verbleiben können, wenn mehrere Verarbeitungsschritte dasselbe Device verwenden.

---

## 144. Postprocessing

Medical AI Compute MUSS standardisierte Postprocessing-Pipelines unterstützen.

---

## 145. Postprocessing Examples

Postprocessing kann umfassen:

```text
Thresholding
Softmax
Argmax
Connected Components
Non-Maximum Suppression
Mask Cleanup
Coordinate Mapping
Label Mapping
Measurement
Confidence Filtering
```

---

## 146. Postprocessing Version

Postprocessing-Konfigurationen MÜSSEN versionierbar sein.

---

## 147. Coordinate Mapping

Räumliche Modellergebnisse MÜSSEN auf die ursprüngliche medizinische Geometrie zurücktransformierbar sein.

---

## 148. Segmentation

Medical AI Compute MUSS Segmentierungsmodelle unterstützen können.

---

## 149. Segmentation Output

Eine Segmentierung SOLL mindestens repräsentieren können:

```text
LabelMap
ProbabilityMap
BinaryMask
MultiClassMask
InstanceMask
```

---

## 150. Segmentation Geometry

Segmentierungsergebnisse MÜSSEN eine definierte räumliche Geometrie besitzen.

---

## 151. Segmentation Confidence

Konfidenz- oder Wahrscheinlichkeitsinformationen SOLLEN erhalten bleiben.

---

## 152. Detection

Objektdetektion SOLL unterstützt werden.

---

## 153. Detection Output

Eine Detektion SOLL enthalten können:

```text
Class
Confidence
BoundingBox
BoundingVolume
Position
Mask
```

---

## 154. Landmark Detection

Landmark Detection SOLL unterstützt werden.

---

## 155. Landmark Output

Ein Landmark SOLL mindestens enthalten können:

```text
Label
Position
CoordinateSystem
Confidence
```

---

## 156. Classification

Klassifikationsmodelle SOLLEN unterstützt werden.

---

## 157. Classification Output

Klassifikationen SOLLEN Score- beziehungsweise Wahrscheinlichkeitsvektoren erhalten können.

---

## 158. Top-K

Top-K-Ergebnisse SOLLEN unterstützt werden.

---

## 159. Regression

Regressionsmodelle SOLLEN unterstützt werden.

---

## 160. Regression Units

Physikalische beziehungsweise medizinische Regressionswerte MÜSSEN ihre Einheit deklarieren.

---

## 161. Anomaly Detection

Anomalieerkennung SOLL unterstützt werden.

---

## 162. Anomaly Score

Ein Anomaly Score MUSS von einer bestätigten Diagnose unterscheidbar sein.

---

## 163. Reconstruction AI

KI-basierte Rekonstruktionsverfahren SOLLEN unterstützt werden.

---

## 164. Denoising AI

KI-basierte Rauschreduktion SOLL unterstützt werden.

---

## 165. Enhancement AI

KI-basierte Bildverbesserung SOLL unterstützt werden.

---

## 166. Source Preservation

KI-veränderte Bilddaten DÜRFEN Originaldaten nicht überschreiben.

---

## 167. Derived Image

KI-rekonstruierte oder verbesserte Bilder MÜSSEN als abgeleitete Daten identifizierbar sein.

---

## 168. Measurement Assistance

KI-gestützte Messungen SOLLEN unterstützt werden.

---

## 169. Measurement Provenance

Eine KI-gestützte Messung MUSS auf Modell und Quelldaten zurückführbar sein.

---

## 170. Embeddings

Medical AI Compute SOLL medizinische Embeddings unterstützen.

---

## 171. Embedding Object

Ein Embedding wird als:

`MedicalAIEmbedding`

repräsentiert.

---

## 172. Embedding Model Identity

Ein Embedding MUSS die Modellidentität referenzieren.

---

## 173. Embedding Compatibility

Embeddings verschiedener inkompatibler Modelle DÜRFEN nicht ohne explizite Transformation miteinander verglichen werden.

---

## 174. Similarity Search

Embeddings SOLLEN für autorisierte Ähnlichkeitssuche verwendet werden können.

---

## 175. Privacy of Embeddings

Embeddings MÜSSEN als potenziell sensible medizinische Daten behandelt werden.

---

## 176. Medical Text AI

Medical AI Compute SOLL medizinische Textverarbeitung unterstützen können.

---

## 177. Text Tasks

Mögliche Aufgaben umfassen:

```text
Classification
InformationExtraction
Summarization
EntityRecognition
CodingAssistance
ReportStructuring
Embedding
Generation
```

---

## 178. Generated Text

KI-generierter medizinischer Text MUSS als generierter Inhalt identifizierbar sein.

---

## 179. Source Grounding

Textmodelle SOLLEN Quellenreferenzen beziehungsweise verwendete Eingangsdaten referenzieren können.

---

## 180. Hallucination Boundary

Generierter Text DARF nicht allein aufgrund sprachlicher Plausibilität als verifizierte medizinische Tatsache behandelt werden.

---

## 181. Structured Output

Für klinisch relevante automatisierte Weiterverarbeitung SOLLEN strukturierte Ausgaben gegenüber unstrukturiertem Freitext bevorzugt werden.

---

## 182. Output Schema

Ein Modell SOLL ein maschinenlesbares Output Schema deklarieren können.

---

## 183. Schema Validation

Strukturierte Modellausgaben MÜSSEN gegen ihr Schema validierbar sein.

---

## 184. Confidence

Medical AI Compute SOLL Konfidenzwerte verwalten können.

---

## 185. Confidence Semantics

Die Bedeutung eines Confidence-Wertes MUSS modellspezifisch definiert sein.

---

## 186. Confidence Is Not Probability

Ein beliebiger Model Score DARF nicht automatisch als kalibrierte Wahrscheinlichkeit interpretiert werden.

---

## 187. Calibration

Modelle SOLLEN Kalibrierungsinformationen besitzen können.

---

## 188. Calibration Version

Die verwendete Kalibrierung MUSS versionierbar sein.

---

## 189. Calibration Drift

Veränderungen der Kalibrierungsqualität SOLLEN über Monitoring erkannt werden können.

---

## 190. Uncertainty

Medical AI Compute SOLL Unsicherheitsinformationen unterstützen.

---

## 191. Uncertainty Types

Die Architektur SOLL mindestens unterscheiden können:

```text
PredictiveUncertainty
AleatoricUncertainty
EpistemicUncertainty
Unknown
```

---

## 192. Uncertainty Output

Unsicherheit SOLL zusammen mit dem Ergebnis transportiert werden können.

---

## 193. Abstention

Ein Modell SOLL die Möglichkeit besitzen, kein verwertbares Ergebnis zu liefern.

---

## 194. Abstain State

Ein solcher Zustand wird als:

`ABSTAIN`

repräsentiert.

---

## 195. Abstention vs Negative

`ABSTAIN` MUSS von einem negativen medizinischen Klassifikationsergebnis unterscheidbar sein.

---

## 196. Explainability

Medical AI Compute SOLL Explainability-Verfahren integrieren können.

---

## 197. Explainability Result

Ein Explainability-Ergebnis wird als:

`MedicalAIExplanation`

repräsentiert.

---

## 198. Explanation Types

Mögliche Typen umfassen:

```text
SaliencyMap
AttentionMap
FeatureImportance
Counterfactual
ExampleBased
TextualExplanation
Custom
```

---

## 199. Explanation Separation

Eine Erklärung MUSS vom eigentlichen Modellergebnis getrennt repräsentiert werden.

---

## 200. Explanation Limitation

Eine Explainability-Ausgabe DARF nicht automatisch als kausale Erklärung interpretiert werden.

---

## 201. Provenance

Medical AI Compute MUSS umfassende Provenance unterstützen.

---

## 202. Provenance Record

Ein:

`MedicalAIProvenanceRecord`

SOLL mindestens enthalten können:

```text
RequestId
InputReferences
ModelId
ModelVersion
ModelHash
PreprocessingVersion
PostprocessingVersion
RuntimeVersion
Backend
Device
Precision
Configuration
Timestamp
```

---

## 203. Extended Provenance

Zusätzlich SOLLEN gespeichert werden können:

```text
CalibrationVersion
Thresholds
RandomSeed
CompilerVersion
DriverVersion
OptimizationProfile
FallbackState
```

---

## 204. Result Traceability

Jedes klinisch relevantes AI-Ergebnis SOLL auf seinen Provenance Record zurückführbar sein.

---

## 205. Processing Graph

Die vollständige Verarbeitung SOLL als Graph darstellbar sein.

Beispiel:

```text
DICOM Study
    |
    v
Decode
    |
    v
Resample
    |
    v
Normalize
    |
    v
AI Model
    |
    v
Probability Map
    |
    v
Postprocess
    |
    v
Segmentation
```

---

## 206. Reproducibility

Offline-Inferenz SOLL bei identischen Bedingungen reproduzierbar sein.

---

## 207. Deterministic Mode

Ein deterministischer Ausführungsmodus SOLL unterstützt werden.

---

## 208. Random Seed

Stochastische Verfahren SOLLEN einen expliziten Seed verwenden können.

---

## 209. Backend Determinism

Die Runtime SOLL dokumentieren können, ob ein Backend deterministische Ausführung garantiert.

---

## 210. Cross-Backend Tolerance

CPU-, GPU- und NPU-Ergebnisse SOLLEN über definierte numerische Toleranzen vergleichbar sein.

---

## 211. Model Validation

Medical AI Compute SOLL technische Modellvalidierung unterstützen.

---

## 212. Validation Dataset Reference

Ein Modell SOLL Referenzen auf seine Validierungsgrundlage besitzen können.

---

## 213. Validation Metrics

Modellspezifische Validierungsmetriken SOLLEN gespeichert werden können.

---

## 214. Validation Scope

Es MUSS darstellbar sein, für welchen Datenbereich Validierungsergebnisse gelten.

---

## 215. Validation Separation

Technische Runtime-Validierung DARF nicht mit klinischer Validierung gleichgesetzt werden.

---

## 216. Golden Test Set

Modelle SOLLEN mit definierten Golden Test Sets geprüft werden können.

---

## 217. Golden Output

Referenzausgaben SOLLEN mit Toleranzen definiert werden können.

---

## 218. Deployment Validation

Vor Deployment SOLL ein Modell auf der Zielhardware validiert werden können.

---

## 219. Backend-Specific Validation

Optimierte oder quantisierte Varianten SOLLEN separat validierbar sein.

---

## 220. Model Update

Modellupdates MÜSSEN kontrolliert erfolgen.

---

## 221. Atomic Deployment

Eine neue Modellversion SOLL atomar aktiviert werden können.

---

## 222. Rollback

Ein Modelldeployment SOLL auf eine vorherige Version zurückgerollt werden können.

---

## 223. Canary Deployment

Neue Modelle SOLLEN schrittweise ausgerollt werden können.

---

## 224. Shadow Mode

Neue Modelle SOLLEN im Shadow Mode ausgeführt werden können.

---

## 225. Shadow Result

Shadow-Ergebnisse DÜRFEN den normalen klinischen Workflow nicht automatisch beeinflussen.

---

## 226. A/B Evaluation

Mehrere Modellversionen SOLLEN kontrolliert verglichen werden können.

---

## 227. Clinical Isolation

Experimentelle Modelle MÜSSEN von produktiv freigegebenen Modellen unterscheidbar sein.

---

## 228. Monitoring

Medical AI Compute MUSS Modell- und Runtime-Monitoring unterstützen.

---

## 229. Runtime Metrics

Mindestens folgende Metriken SOLLEN verfügbar sein:

```text
RequestCount
SuccessCount
FailureCount
AbstainCount
Latency
QueueLatency
InferenceLatency
PreprocessingLatency
PostprocessingLatency
MemoryUsage
DeviceUtilization
```

---

## 230. Model Metrics

Zusätzlich SOLLEN modellbezogene Metriken unterstützt werden:

```text
InputDistribution
OutputDistribution
ConfidenceDistribution
ClassDistribution
QualityFailures
OutOfScopeInputs
```

---

## 231. Drift Detection

Medical AI Compute SOLL Drift Detection unterstützen.

---

## 232. Input Drift

Veränderungen der Eingabedatenverteilung SOLLEN erkannt werden können.

---

## 233. Output Drift

Veränderungen der Modellausgaben SOLLEN erkannt werden können.

---

## 234. Performance Drift

Wenn Ground Truth verfügbar ist, SOLL Performance Drift erkannt werden können.

---

## 235. Drift Is Not Failure

Ein Drift-Signal MUSS von einem unmittelbaren technischen Runtime-Fehler unterschieden werden.

---

## 236. Drift Action

Drift-Ereignisse SOLLEN Policies auslösen können:

```text
LOG
WARN
REVIEW
LIMIT
DISABLE_MODEL
```

---

## 237. Data Quality Monitoring

Eingabequalität SOLL kontinuierlich überwacht werden können.

---

## 238. Model Health

Ein Modell SOLL einen technischen Betriebszustand besitzen:

```text
HEALTHY
DEGRADED
UNAVAILABLE
DISABLED
```

---

## 239. Fail-Safe

Bei kritischen Modell- oder Runtime-Fehlern MUSS ein definierter Fail-Safe-Zustand existieren.

---

## 240. Fail-Safe Principle

Ein technischer Fehler DARF nicht in ein scheinbar gültiges medizinisches Ergebnis umgewandelt werden.

---

## 241. Error Result

Fehler MÜSSEN explizit als Fehlerzustand zurückgegeben werden.

---

## 242. Fallback Model

Ein alternatives Modell DARF als Fallback verwendet werden, wenn dies durch Policy erlaubt ist.

---

## 243. Fallback Model Visibility

Die Verwendung eines Fallback-Modells MUSS im Ergebnis und Provenance Record sichtbar sein.

---

## 244. Security

Medical AI Compute MUSS in die NovaOS-Sicherheitsarchitektur integriert werden.

---

## 245. Authentication

Aufrufende Benutzer und Dienste MÜSSEN authentifizierbar sein.

---

## 246. Authorization

Zugriffe auf Modelle und medizinische Daten MÜSSEN autorisierbar sein.

---

## 247. Model Permission

Die Berechtigung zur Nutzung eines Modells SOLL getrennt von der Berechtigung auf Patientendaten geprüft werden.

---

## 248. Least Privilege

Medical-AI-Dienste SOLLEN nur auf die für ihren Zweck erforderlichen Daten zugreifen.

---

## 249. Service Identity

Jeder automatisierte Medical-AI-Dienst SOLL eine eigene technische Identität besitzen.

---

## 250. Model Artifact Security

Modellartefakte MÜSSEN als potenziell sicherheitsrelevante ausführbare Daten behandelt werden.

---

## 251. Untrusted Model

Nicht vertrauenswürdige Modelle DÜRFEN nicht ungeprüft in privilegierten Runtime-Kontexten ausgeführt werden.

---

## 252. Model Sandbox

Medical AI Compute SOLL isolierte Ausführungsumgebungen unterstützen.

---

## 253. Resource Isolation

Modelle SOLLEN bezüglich:

```text
CPU
GPU
NPU
Memory
Storage
Network
```

begrenzt werden können.

---

## 254. Network Access

Ein Modell SOLL standardmäßig keinen direkten Netzwerkzugriff benötigen.

---

## 255. Explicit Network Permission

Falls Netzwerkzugriff erforderlich ist, MUSS dieser explizit autorisiert werden.

---

## 256. Privacy

Medical AI Compute MUSS medizinische Daten als sensible Daten behandeln.

---

## 257. Data Minimization

Modelle SOLLEN nur die tatsächlich erforderlichen Daten erhalten.

---

## 258. De-Identification

Medical AI Compute SOLL De-Identification-Pipelines integrieren können.

---

## 259. Research Mode

Forschungsworkflows SOLLEN de-identifizierte Datensätze verwenden können.

---

## 260. Original Separation

De-identifizierte und originale Patientendaten MÜSSEN unterscheidbar bleiben.

---

## 261. Temporary Data

Temporäre AI-Daten SOLLEN nach Abschluss entsprechend der Policy entfernt werden können.

---

## 262. Tensor Privacy

Zwischentensoren KÖNNEN sensible Informationen enthalten und MÜSSEN entsprechend behandelt werden.

---

## 263. Embedding Privacy

Embeddings MÜSSEN als potenziell sensible Daten behandelt werden.

---

## 264. Logging Privacy

Logs DÜRFEN keine unnötigen medizinischen Daten enthalten.

---

## 265. Audit

Medical AI Compute MUSS relevante Operationen auditierbar machen.

---

## 266. Audit Events

Mindestens folgende Ereignisse SOLLEN auditierbar sein:

```text
MODEL_LOAD
MODEL_DEPLOY
MODEL_DISABLE
MODEL_ROLLBACK
INFERENCE
INFERENCE_FAILURE
DATA_ACCESS
RESULT_EXPORT
POLICY_CHANGE
AUTHORIZATION_FAILURE
```

---

## 267. Audit Fields

Ein Audit Event SOLL mindestens enthalten können:

```text
EventId
Timestamp
Actor
Operation
ModelId
ModelVersion
Resource
Result
```

---

## 268. Audit Integrity

Auditinformationen SOLLEN gegen unbemerkte Manipulation geschützt werden.

---

## 269. DICOM Integration

Medical AI Compute MUSS `NPSPEC-COMPUTE-DICOM-0001` integrieren können.

---

## 270. DICOM Metadata

DICOM-Metadaten SOLLEN als Modellinput verwendet werden können, wenn das Modell dies vorsieht.

---

## 271. DICOM Derived Result

Geeignete KI-Ergebnisse SOLLEN in standardisierte DICOM-Derivate überführt werden können.

---

## 272. Source Reference

Ein DICOM-basiertes AI-Ergebnis SOLL seine Quellobjekte referenzieren.

---

## 273. PACS Integration

Medical AI Compute SOLL `NPSPEC-COMPUTE-PACS-0001` integrieren.

---

## 274. PACS Trigger

PACS-Ereignisse SOLLEN autorisierte AI-Pipelines auslösen können.

---

## 275. Study Processing

Eine vollständige Studie SOLL als Medical-AI-Job verarbeitet werden können.

---

## 276. Series Processing

Einzelne Serien SOLLEN verarbeitet werden können.

---

## 277. Result Return

Abgeleitete medizinische Ergebnisse SOLLEN an PACS zurückgegeben werden können.

---

## 278. Original Preservation

PACS-Originalobjekte DÜRFEN durch AI-Verarbeitung nicht überschrieben werden.

---

## 279. Radiology Integration

Medical AI Compute SOLL `NPSPEC-COMPUTE-RADIOLOGY-0001` integrieren.

---

## 280. Radiology Pipeline

Beispiel:

```text
CT Volume
   |
   v
Radiology Preprocessing
   |
   v
Medical AI
   |
   v
Segmentation
   |
   v
Radiology Measurement
```

---

## 281. Ultrasound Integration

Medical AI Compute SOLL `NPSPEC-COMPUTE-ULTRASOUND-0001` integrieren.

---

## 282. Ultrasound AI Inputs

Modelle DÜRFEN verwenden:

```text
RF Data
IQ Data
B-Mode
M-Mode
Doppler
Cine
3D Volume
4D Sequence
```

---

## 283. Ultrasound Real-Time AI

KI-Verarbeitung in Echtzeit-Ultraschallpipelines MUSS Deadline- und Ressourcenanforderungen berücksichtigen.

---

## 284. Distributed Compute

Medical AI Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0007` integrieren können.

---

## 285. Distributed Batch Inference

Große Datensätze SOLLEN über mehrere Nodes verteilt verarbeitet werden können.

---

## 286. Data Locality

Der Scheduler SOLL Datenlokalität berücksichtigen.

---

## 287. Model Locality

Jobs SOLLEN bevorzugt auf Nodes geplant werden können, auf denen das benötigte Modell bereits geladen ist.

---

## 288. HPC Integration

Große Medical-AI-Batchworkloads SOLLEN `NPSPEC-COMPUTE-HPC-0001` verwenden können.

---

## 289. Training Boundary

Diese Spezifikation konzentriert sich primär auf Inferenz.

Training DARF auf derselben Compute-Infrastruktur aufbauen, MUSS jedoch separat spezifiziert werden.

---

## 290. Training Data Separation

Trainingsdaten, Validierungsdaten und produktive Inferenzdaten MÜSSEN logisch unterscheidbar sein.

---

## 291. Online Learning

Ein produktiv eingesetztes medizinisches Modell DARF sich nicht unkontrolliert selbst verändern.

---

## 292. Controlled Learning

Online- oder Continual-Learning MUSS als explizit kontrollierter, versionierter Prozess implementiert werden.

---

## 293. No Silent Weight Mutation

Gewichte eines freigegebenen Modells DÜRFEN während normaler Inferenz nicht stillschweigend verändert werden.

---

## 294. Model Version after Learning

Eine Änderung der Modellparameter MUSS zu einer neuen nachvollziehbaren Modellversion führen.

---

## 295. Federated Learning

Federated Learning SOLL architektonisch integrierbar sein.

---

## 296. Federated Learning Separation

Federated Learning MUSS von normaler Medical-AI-Inferenz getrennt spezifiziert werden.

---

## 297. Explainable Result Bundle

Ein Medical-AI-Ergebnis DARF als Bundle bereitgestellt werden:

```text
PrimaryResult
Confidence
Uncertainty
Explanation
Provenance
QualityState
```

---

## 298. Result Status

Ein Ergebnis SOLL mindestens folgende Zustände besitzen können:

```text
VALID
LOW_CONFIDENCE
OUT_OF_SCOPE
ABSTAIN
DEGRADED
INVALID
FAILED
CANCELLED
```

---

## 299. Result Quality

Result Status und medizinischer Ergebniswert MÜSSEN getrennte Felder sein.

---

## 300. Result Publication

Ein Ergebnis DARF erst nach erfolgreicher Postprocessing- und Schema-Validierung als vollständig veröffentlicht werden.

---

## 301. Result Immutability

Veröffentlichte Ergebnisse SOLLEN unveränderlich sein.

Änderungen erzeugen ein neues Ergebnis.

---

## 302. Result Identity

Jedes veröffentlichte Ergebnis SOLL eine eindeutige:

`ResultId`

besitzen.

---

## 303. Result Source

Ein Ergebnis MUSS seine Eingabedaten referenzieren können.

---

## 304. Result Model

Ein Ergebnis MUSS Modell-ID und Modellversion referenzieren können.

---

## 305. Result Timestamp

Inferenz- und Veröffentlichungszeitpunkt SOLLEN getrennt gespeichert werden können.

---

## 306. Diagnostics

Medical AI Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 307. Diagnostic Classes

Mindestens folgende Klassen SOLLEN existieren:

```text
INVALID_MODEL
MODEL_NOT_FOUND
MODEL_REVOKED
MODEL_INCOMPATIBLE
MODEL_INTEGRITY_FAILURE
INVALID_INPUT
INPUT_OUT_OF_SCOPE
INPUT_QUALITY_FAILURE
INVALID_SHAPE
INVALID_DATA_TYPE
INVALID_GEOMETRY
MISSING_INPUT
PREPROCESSING_FAILED
INFERENCE_FAILED
POSTPROCESSING_FAILED
OUTPUT_SCHEMA_FAILED
LOW_CONFIDENCE
ABSTAIN
BACKEND_UNAVAILABLE
BACKEND_FAILURE
BACKEND_FALLBACK
OUT_OF_MEMORY
DEADLINE_MISSED
DEVICE_FAILURE
MODEL_TIMEOUT
AUTHENTICATION_FAILED
AUTHORIZATION_FAILED
CANCELLED
```

---

## 308. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 309. Diagnostic Context

Diagnosen SOLLEN enthalten können:

```text
RequestId
ModelId
ModelVersion
Backend
Device
TaskType
```

---

## 310. Privacy-Aware Diagnostics

Diagnosen DÜRFEN keine unnötigen Patientendaten enthalten.

---

## 311. Profiling

Medical AI Compute MUSS Profiling unterstützen.

---

## 312. Profiling Stages

Mindestens:

```text
Load
Decode
Validate
Preprocess
TransferToDevice
Inference
TransferFromDevice
Postprocess
Publish
```

SOLLTEN separat messbar sein.

---

## 313. Latency

End-to-End-Latenz SOLL messbar sein.

---

## 314. Queue Latency

Wartezeit vor Ausführung SOLL getrennt messbar sein.

---

## 315. Inference Latency

Reine Modelllaufzeit SOLL separat messbar sein.

---

## 316. Throughput

Requests pro Zeiteinheit SOLLEN messbar sein.

---

## 317. Device Utilization

CPU-, GPU- und NPU-Auslastung SOLL beobachtbar sein.

---

## 318. Model Load Time

Modellladezeiten SOLLEN messbar sein.

---

## 319. Cache Hit Rate

Model Cache Hit Rate SOLL messbar sein.

---

## 320. Testing — Model Loading

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
ValidModel
InvalidModel
CorruptModel
UnsupportedModel
WrongVersion
RevokedModel
```

---

## 321. Testing — Input Validation

Mindestens:

```text
ValidInput
InvalidShape
InvalidType
MissingInput
InvalidGeometry
UnsupportedModality
OutOfScopeInput
```

MÜSSEN getestet werden.

---

## 322. Testing — Preprocessing

Preprocessing-Schritte MÜSSEN gegen definierte Referenzdaten testbar sein.

---

## 323. Testing — Inference

Mindestens:

```text
CPUInference
BatchInference
SingleInference
Cancellation
Timeout
```

MÜSSEN getestet werden.

---

## 324. Testing — GPU

GPU-Ergebnisse SOLLEN gegen freigegebene Referenzergebnisse geprüft werden.

---

## 325. Testing — NPU

NPU-Ergebnisse SOLLEN gegen freigegebene Referenzergebnisse geprüft werden.

---

## 326. Testing — Backend Fallback

Backend-Fallback MUSS getestet werden.

---

## 327. Testing — Precision

Alle freigegebenen Präzisionsmodi SOLLEN separat getestet werden.

---

## 328. Testing — Segmentation

Segmentierungsmodelle SOLLEN mit bekannten Referenzmasken getestet werden.

---

## 329. Testing — Detection

Detektionsmodelle SOLLEN mit bekannten Referenzobjekten getestet werden.

---

## 330. Testing — Classification

Klassifikationsausgaben SOLLEN gegen definierte Golden Outputs geprüft werden.

---

## 331. Testing — Confidence

Confidence-Verarbeitung SOLL getestet werden.

---

## 332. Testing — Abstention

`ABSTAIN` MUSS von normalen negativen Ergebnissen unterscheidbar getestet werden.

---

## 333. Testing — Out-of-Scope

Out-of-Scope-Erkennung SOLL mit ungeeigneten Eingaben getestet werden.

---

## 334. Testing — Provenance

Für jedes relevante Ergebnis MUSS geprüft werden können, ob die notwendige Provenance vorhanden ist.

---

## 335. Testing — Determinism

Deterministische Ausführung MUSS reproduzierbare Ergebnisse liefern.

---

## 336. Testing — Security

Mindestens:

```text
UnauthorizedModelAccess
UnauthorizedPatientDataAccess
InvalidModelArtifact
SandboxEscapeAttempt
UnauthorizedNetworkAccess
```

SOLLTEN getestet werden.

---

## 337. Testing — Privacy

Logs, Diagnosen und temporäre Daten SOLLEN auf unbeabsichtigte Datenoffenlegung geprüft werden.

---

## 338. Testing — Model Deployment

Mindestens:

```text
Deploy
AtomicSwitch
Rollback
Canary
ShadowMode
Revoke
```

SOLLTEN getestet werden.

---

## 339. Testing — Monitoring

Drift-, Quality- und Runtime-Monitoring SOLLEN mit kontrollierten Testdaten überprüft werden.

---

## 340. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
ManyConcurrentRequests
Large3DVolumes
LongCineSequences
LargeBatch
MultipleModels
RapidModelSwitching
GPUOutOfMemory
NPUUnavailable
CPUFallback
MemoryPressure
QueuePressure
ModelReload
DeviceFailure
CancellationUnderLoad
```

---

## 341. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
MedicalAIModel
ModelId
ModelVersion
ModelHash
ModelMetadata

MedicalAITask
MedicalAIInput
MedicalAIOutput
MedicalAIResult

Classification
Regression
Segmentation
Detection

Image Input
Volume Input
Tensor Input
Structured Metadata Input

Input Validation
Shape Validation
Type Validation
Geometry Validation

Preprocessing
Normalization
Resampling
Tensor Conversion

CPU Inference
GPU Architecture
NPU Architecture

Postprocessing
Output Validation

Confidence
Result Status

Model Loading
Model Cache
Model Versioning
Model Integrity

Scheduler Integration
Memory Model Integration
Object Model Integration

DICOM Integration
Radiology Integration
PACS Integration
Ultrasound Integration

Cancellation
Timeout
Diagnostics
Profiling
Provenance
Audit
Access Control
```

---

## 342. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Real-Time Inference
Streaming Inference
Dynamic Batching

GPU Inference
NPU Inference
Distributed Inference
HPC Batch Inference

Multimodal AI
Medical Text AI
Cine AI
Signal AI
Time-Series AI

Landmark Detection
Anomaly Detection
AI Reconstruction
AI Denoising
AI Enhancement
Measurement Assistance

Embeddings
Similarity Search

Confidence Calibration
Uncertainty Estimation
Abstention
Explainability

Model Registry
Atomic Deployment
Rollback
Canary Deployment
Shadow Mode

Drift Detection
Input Quality Monitoring
Performance Monitoring

De-Identification
Sandboxing
Resource Isolation

Federated-Learning Architecture
Controlled Continual-Learning Architecture
```

---

## 343. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-MEDAI-0001` MUSS:

1. medizinische KI-Modelle eindeutig identifizieren.
2. Modellversionen eindeutig identifizieren.
3. Modellartefakte fingerprinten können.
4. Modellintegrität prüfen können.
5. Modellmetadaten verwalten.
6. Intended Use repräsentieren können.
7. Modellzustände unterscheiden können.
8. widerrufene Modelle erkennen.
9. technische Installierbarkeit und Freigabestatus trennen.
10. Modelle kontrolliert laden.
11. Modelle entladen können.
12. Modell-Caching unterstützen.
13. Medical-AI-Tasks repräsentieren.
14. Eingaben repräsentieren.
15. Ausgaben repräsentieren.
16. veröffentlichte Ergebnisse repräsentieren.
17. Bilddaten als Eingabe unterstützen.
18. Volumendaten als Eingabe unterstützen.
19. Tensoren als Eingabe unterstützen.
20. strukturierte Metadaten als Eingabe unterstützen.
21. Eingaben validieren.
22. Tensor Shapes validieren.
23. Datentypen validieren.
24. räumliche Geometrie validieren können.
25. fehlende Eingaben erkennen.
26. Out-of-Scope-Zustände repräsentieren.
27. Preprocessing-Pipelines unterstützen.
28. Preprocessing versionieren.
29. Normalisierung unterstützen.
30. Resampling unterstützen.
31. Tensor Conversion unterstützen.
32. räumliche Transformationen nachvollziehbar halten.
33. CPU-Inferenz unterstützen.
34. GPU-Inferenz architektonisch unterstützen.
35. NPU-Inferenz architektonisch unterstützen.
36. Backend-Kompatibilität prüfen.
37. Backend-Fallback kontrolliert durchführen können.
38. verwendetes Backend dokumentieren.
39. verwendete Präzision dokumentieren.
40. Batch Inference unterstützen.
41. kontrollierten Abbruch unterstützen.
42. Timeouts unterstützen.
43. unvollständige Ergebnisse nicht als vollständig veröffentlichen.
44. Postprocessing unterstützen.
45. Postprocessing versionieren.
46. Output-Schema-Validierung ermöglichen.
47. Klassifikation unterstützen.
48. Regression unterstützen.
49. Segmentierung unterstützen.
50. Detektion unterstützen.
51. Konfidenzwerte repräsentieren können.
52. Konfidenz nicht automatisch mit kalibrierter Wahrscheinlichkeit gleichsetzen.
53. Result Status getrennt vom Ergebniswert verwalten.
54. Fehler explizit als Fehler behandeln.
55. Provenance für relevante Ergebnisse bereitstellen.
56. Modell-ID im Ergebnis referenzieren.
57. Modellversion im Ergebnis referenzieren.
58. Eingabedaten im Ergebnis referenzieren können.
59. Preprocessing-Version dokumentieren.
60. Postprocessing-Version dokumentieren.
61. Runtime-Version dokumentieren.
62. Backend dokumentieren.
63. Präzision dokumentieren.
64. DICOM Compute integrieren.
65. Radiology Compute integrieren.
66. PACS Compute integrieren.
67. Ultrasound Compute integrieren.
68. den Compute Scheduler integrieren.
69. das Compute Memory Model integrieren.
70. das Compute Object Model integrieren.
71. Zugriffskontrolle integrieren.
72. Modellzugriff und Patientendatenzugriff getrennt autorisieren können.
73. Least Privilege unterstützen.
74. Modellartefakte als sicherheitsrelevante Daten behandeln.
75. Logs datenschutzfreundlich gestalten.
76. Audit unterstützen.
77. strukturierte Diagnosen bereitstellen.
78. Profiling unterstützen.
79. End-to-End-Latenz messen können.
80. Inferenzlatenz messen können.
81. Ressourcenverbrauch beobachten können.
82. Modellupdates versioniert durchführen.
83. Rollback architektonisch unterstützen.
84. freigegebene Modellgewichte während Inferenz nicht stillschweigend verändern.
85. KI-Ergebnisse von medizinischen Entscheidungen trennen.
86. AI-generierte und abgeleitete Daten kennzeichnen.
87. Originaldaten erhalten.
88. Erweiterungen ohne Änderung des grundlegenden Medical-AI-Objektmodells ermöglichen.

---

## 344. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaMedicalAICompute
 |
 +-- MedicalAIRuntime
 |
 +-- MedicalAIModelManager
 |
 +-- MedicalAIModelRegistry
 |
 +-- MedicalAIModelValidator
 |
 +-- MedicalAIInputValidator
 |
 +-- MedicalAIPreprocessor
 |
 +-- MedicalAIInferenceEngine
 |
 +-- MedicalAIBackendManager
 |
 +-- MedicalAIPostprocessor
 |
 +-- MedicalAIResultValidator
 |
 +-- MedicalAIConfidenceEngine
 |
 +-- MedicalAIUncertaintyEngine
 |
 +-- MedicalAIExplainability
 |
 +-- MedicalAIProvenance
 |
 +-- MedicalAIDeploymentManager
 |
 +-- MedicalAIMonitor
 |
 +-- MedicalAIDriftDetector
 |
 +-- MedicalAISecurityBridge
 |
 +-- MedicalAIAuditBridge
 |
 +-- MedicalAIDiagnostics
 |
 +-- MedicalAIProfiler
```

Diese Struktur ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 345. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt die gemeinsame Ausführungsinfrastruktur bereit.

```text
Medical AI
    |
    v
Compute Runtime
    |
 +-- CPU
 +-- GPU
 +-- NPU
 +-- Distributed
```

---

## 346. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert:

```text
Preprocessing
Inference
Postprocessing
Batch Jobs
Streaming Jobs
Real-Time Jobs
Background Jobs
```

---

## 347. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet:

```text
Model Memory
Input Tensors
Output Tensors
Workspace
Shared Buffers
Device Memory
```

---

## 348. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Medical-AI-Objekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 349. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

GPU Runtime stellt beschleunigte Inferenz und Tensorverarbeitung bereit.

---

## 350. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

NPU Runtime stellt spezialisierte AI-Beschleunigung bereit.

---

## 351. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Distributed Compute ermöglicht verteilte Medical-AI-Workloads.

---

## 352. Beziehung zu `NPSPEC-COMPUTE-DICOM-0001`

DICOM Compute stellt standardisierte medizinische Daten und Metadaten bereit.

---

## 353. Beziehung zu `NPSPEC-COMPUTE-RADIOLOGY-0001`

Radiology Compute stellt Bild-, Volumen-, Geometrie-, Segmentierungs- und Messinfrastruktur bereit.

---

## 354. Beziehung zu `NPSPEC-COMPUTE-PACS-0001`

PACS Compute stellt medizinische Studien und Serien bereit und kann abgeleitete AI-Ergebnisse archivieren.

---

## 355. Beziehung zu `NPSPEC-COMPUTE-ULTRASOUND-0001`

Ultrasound Compute stellt ultraschallspezifische Signal-, Bild-, Doppler-, Cine- und Volumendaten bereit.

---

## 356. Grundlegende Inferenzpipeline

```text
Medical Data
     |
     v
Input Validation
     |
     v
Quality Gate
     |
     v
Preprocessing
     |
     v
Tensor
     |
     v
Model Inference
     |
     v
Raw Output
     |
     v
Postprocessing
     |
     v
Result Validation
     |
     v
Medical AI Result
```

---

## 357. Backend-Modell

```text
Medical AI Model
       |
       v
Backend Selection
       |
 +-----+-----+
 |     |     |
 v     v     v
CPU   GPU   NPU
 |     |     |
 +-----+-----+
       |
       v
Raw Model Output
```

---

## 358. PACS-Integrationsmodell

```text
PACS
 |
 v
Study / Series
 |
 v
Medical AI Pipeline
 |
 v
Derived Result
 |
 v
DICOM Representation
 |
 v
PACS
```

---

## 359. Result-Modell

```text
MedicalAIResult
 |
 +-- ResultId
 +-- Status
 +-- PrimaryResult
 +-- Confidence
 +-- Uncertainty
 +-- Explanation
 +-- InputReferences
 +-- ModelReference
 +-- Provenance
```

---

## 360. Deployment-Modell

```text
Development
    |
    v
Validation
    |
    v
Approved
    |
    v
Canary / Shadow
    |
    v
Deployed
    |
    +------+
    |      |
    v      v
Rollback Deprecated
           |
           v
         Revoked
```

---

## 361. Monitoring-Modell

```text
Inference
   |
   +--> Runtime Metrics
   |
   +--> Input Statistics
   |
   +--> Output Statistics
   |
   +--> Confidence
   |
   +--> Quality
   |
   v
Monitoring
   |
   +--> Drift Detection
   +--> Health
   +--> Alert
```

---

## 362. Fail-Safe-Modell

```text
AI Request
    |
    v
Validation
    |
    +-- Invalid ------> Reject
    |
    v
Inference
    |
    +-- Failure ------> Error
    |
    v
Result Validation
    |
    +-- Invalid ------> Reject Result
    |
    +-- Low Confidence -> Flag
    |
    +-- Out of Scope --> Flag / Abstain
    |
    v
Valid AI Result
```

Keiner dieser Fehlerpfade DARF automatisch in ein scheinbar normales medizinisches Ergebnis umgewandelt werden.

---

## 363. Datenhoheitsgrundsatz

Medical AI Compute MUSS zwischen:

```text
Original Medical Data
Preprocessed Data
Model Input
Raw Model Output
Postprocessed Result
Derived Medical Object
AI Explanation
```

unterscheiden.

---

## 364. Modellhoheitsgrundsatz

Ein produktiv eingesetztes Modell ist durch:

```text
ModelId
ModelVersion
ModelHash
Configuration
```

definiert.

Eine Änderung dieser Bestandteile MUSS nachvollziehbar sein.

---

## 365. Entscheidungsgrundsatz

Medical AI Compute stellt Compute-Ergebnisse bereit.

Die Runtime DARF nicht implizit festlegen, dass:

```text
High Confidence = Correct
Low Confidence = Incorrect
Positive Classification = Confirmed Diagnosis
Negative Classification = Disease Excluded
```

gilt.

Die medizinische Bedeutung MUSS durch den jeweiligen validierten Workflow definiert werden.

---

## 366. Selbstlern-Grundsatz

Produktive medizinische Modelle DÜRFEN nicht unkontrolliert während des normalen Betriebs ihre Gewichte verändern.

Der erlaubte Ablauf lautet:

```text
New Data
   |
   v
Controlled Training
   |
   v
Validation
   |
   v
New Model Version
   |
   v
Approval
   |
   v
Deployment
```

und nicht:

```text
Inference
   |
   v
Silent Weight Change
```

---

## 367. Erweiterbarkeit

Neue:

```text
Model Formats
Inference Engines
CPU Backends
GPU Backends
NPU Backends
Medical Modalities
Task Types
Preprocessors
Postprocessors
Explainability Methods
Uncertainty Methods
Deployment Strategies
Monitoring Systems
```

SOLLTEN integrierbar sein, ohne das grundlegende Medical-AI-Objektmodell zu verändern.

---

## 368. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-MEDAI-MODEL-0001
Medical AI Model Format and Metadata

NPSPEC-COMPUTE-MEDAI-REGISTRY-0001
Medical AI Model Registry

NPSPEC-COMPUTE-MEDAI-INFERENCE-0001
Medical AI Inference Engine

NPSPEC-COMPUTE-MEDAI-PREPROCESS-0001
Medical AI Preprocessing

NPSPEC-COMPUTE-MEDAI-POSTPROCESS-0001
Medical AI Postprocessing

NPSPEC-COMPUTE-MEDAI-IMAGE-0001
Medical Image AI

NPSPEC-COMPUTE-MEDAI-VOLUME-0001
Medical Volume AI

NPSPEC-COMPUTE-MEDAI-SIGNAL-0001
Medical Signal AI

NPSPEC-COMPUTE-MEDAI-TIMESERIES-0001
Medical Time-Series AI

NPSPEC-COMPUTE-MEDAI-TEXT-0001
Medical Text AI

NPSPEC-COMPUTE-MEDAI-MULTIMODAL-0001
Multimodal Medical AI

NPSPEC-COMPUTE-MEDAI-SEGMENT-0001
Medical AI Segmentation

NPSPEC-COMPUTE-MEDAI-DETECT-0001
Medical AI Detection

NPSPEC-COMPUTE-MEDAI-RECON-0001
Medical AI Reconstruction

NPSPEC-COMPUTE-MEDAI-CONFIDENCE-0001
Medical AI Confidence and Calibration

NPSPEC-COMPUTE-MEDAI-UNCERTAINTY-0001
Medical AI Uncertainty

NPSPEC-COMPUTE-MEDAI-XAI-0001
Medical AI Explainability

NPSPEC-COMPUTE-MEDAI-PROVENANCE-0001
Medical AI Provenance

NPSPEC-COMPUTE-MEDAI-DEPLOY-0001
Medical AI Deployment

NPSPEC-COMPUTE-MEDAI-MONITOR-0001
Medical AI Monitoring and Drift

NPSPEC-COMPUTE-MEDAI-SECURITY-0001
Medical AI Security

NPSPEC-COMPUTE-MEDAI-PRIVACY-0001
Medical AI Privacy

NPSPEC-COMPUTE-MEDAI-TRAIN-0001
Medical AI Training

NPSPEC-COMPUTE-MEDAI-FEDERATED-0001
Federated Medical AI

NPSPEC-COMPUTE-MEDAI-TEST-0001
Medical AI Conformance Tests
```

---

## 369. Zusammenfassung

`NPSPEC-COMPUTE-MEDAI-0001` definiert die gemeinsame Medical-AI-Infrastruktur der NovaOS-Compute-Architektur.

Die grundlegende Verarbeitung lautet:

```text
Medical Data
     |
     v
Validation
     |
     v
Preprocessing
     |
     v
Medical AI Model
     |
 +---+---+---+
 |       |   |
CPU     GPU NPU
 |       |   |
 +---+---+---+
     |
     v
Postprocessing
     |
     v
Validated AI Result
```

Medical AI Compute unterstützt:

```text
Classification
Regression
Segmentation
Detection
Landmark Detection
Anomaly Detection
Reconstruction
Denoising
Enhancement
Measurement Assistance
Embeddings
Medical Text AI
Multimodal AI
```

Die medizinischen Daten können aus:

```text
DICOM Compute
Radiology Compute
PACS Compute
Ultrasound Compute
```

bereitgestellt werden.

Die Ausführung kann über:

```text
CPU
GPU
NPU
Distributed Compute
HPC
```

skalieren.

Die Architektur trennt konsequent:

```text
Original Data
Preprocessed Data
Model Input
Raw AI Output
Validated AI Result
Derived Medical Data
AI Explanation
```

voneinander.

Jedes relevante Ergebnis kann auf:

```text
Input
ModelId
ModelVersion
ModelHash
Preprocessing
Backend
Precision
Postprocessing
Runtime
```

zurückgeführt werden.

Produktive Modelle verändern sich während normaler Inferenz nicht unkontrolliert selbst. Änderungen erfolgen über:

```text
Training
   |
   v
Validation
   |
   v
Versioning
   |
   v
Approval
   |
   v
Deployment
```

Die zentrale Sicherheitsregel lautet:

> Ein Medical-AI-Ergebnis ist ein Compute-Ergebnis mit definierter Herkunft, Qualität, Unsicherheit und Gültigkeitsgrenze. Es ist nicht automatisch eine medizinische Wahrheit, Diagnose oder Therapieentscheidung.

Das grundlegende Architekturprinzip lautet:

> NovaOS Medical AI Compute behandelt medizinische KI nicht als undurchsichtige Funktion, die aus medizinischen Daten lediglich eine Antwort erzeugt. Modelle, Versionen, Eingaben, Vorverarbeitung, Hardware-Backend, numerische Präzision, Nachverarbeitung, Konfidenz, Unsicherheit und Provenance werden als explizite Bestandteile des Compute-Prozesses behandelt. Dadurch können medizinische KI-Systeme reproduzierbar, überprüfbar, austauschbar, skalierbar und sicher in DICOM-, Radiology-, PACS- und Ultrasound-Workflows integriert werden, ohne die Grenze zwischen maschineller Berechnung und medizinischer Entscheidung aufzulösen.