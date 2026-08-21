# NPSPEC-COMPUTE-IMAGEGUIDED-0001 — Image-Guided Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Medical Imaging / Image-Guided Systems  
**Komponente:** NovaOS Image-Guided Compute  
**Bezeichner:** `NPSPEC-COMPUTE-IMAGEGUIDED-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RADIOLOGY-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0001`, `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`, `NPSPEC-COMPUTE-DICOM-0001`, `NPSPEC-COMPUTE-RADIOLOGY-0001`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-PACS-0001`, `NPSPEC-COMPUTE-ULTRASOUND-0001`, `NPSPEC-COMPUTE-MEDAI-0001`, `NPSPEC-COMPUTE-RADIOMICS-0001`  
**Kurzname:** Image-Guided Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Image-Guided-Compute-Infrastruktur der NovaOS-Compute-Architektur.

Image-Guided Compute stellt standardisierte Compute-Funktionen für bildgestützte medizinische Verfahren bereit.

Die Komponente ist insbesondere für Workflows vorgesehen, bei denen medizinische Bilddaten mit räumlichen Informationen über:

- Patienten,
- anatomische Strukturen,
- Instrumente,
- Sensoren,
- Tracking-Systeme,
- medizinische Geräte,
- Behandlungsziele,
- Referenzrahmen

kombiniert werden.

Typische Einsatzbereiche umfassen:

- bildgestützte Chirurgie,
- interventionelle Radiologie,
- bildgestützte Biopsie,
- Navigation,
- Katheterführung,
- Nadelnavigation,
- Endoskopie,
- Ultraschallnavigation,
- orthopädische Navigation,
- neurochirurgische Navigation,
- bildgestützte Ablation,
- präoperative Planung,
- intraoperative Bildgebung,
- multimodale Navigation,
- Augmented-Reality-Unterstützung.

Die grundlegende Architektur lautet:

```text
Preoperative Imaging
        +
Intraoperative Imaging
        +
Tracking Data
        +
Patient Geometry
        |
        v
Coordinate Management
        |
        v
Registration
        |
        v
Tracking
        |
        v
Navigation
        |
        v
Visualization / Guidance
```

Image-Guided Compute MUSS räumliche Berechnung und technische Navigation von der medizinischen Entscheidung und der tatsächlichen physischen Intervention trennen.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Jede bildgestützte Position, Transformation und Navigationsinformation besitzt nur dann eine eindeutige Bedeutung, wenn ihr Koordinatensystem, ihre zeitliche Gültigkeit, ihre Transformationskette, ihre Genauigkeit und ihre Datenherkunft bekannt sind.

---

## 3. Sicherheitsgrundsatz

Image-Guided Compute DARF eine räumliche Position nicht als exakt darstellen, wenn deren Genauigkeit oder Gültigkeit nicht bekannt ist.

---

## 4. Guidance Boundary

Die Architektur MUSS zwischen:

```text
Image Processing
Registration
Tracking
Navigation
Guidance
Visualization
Clinical Decision
Physical Device Control
```

unterscheiden.

---

## 5. Ziele

Image-Guided Compute MUSS architektonisch mindestens folgende Bereiche unterstützen:

1. Coordinate Systems,
2. Reference Frames,
3. Transformations,
4. Transform Chains,
5. Registration,
6. Rigid Registration,
7. Affine Registration,
8. Deformable Registration,
9. Landmark Registration,
10. Surface Registration,
11. Image Registration,
12. Tracking,
13. Optical Tracking,
14. Electromagnetic Tracking,
15. Device Tracking,
16. Instrument Tracking,
17. Patient Tracking,
18. Marker Tracking,
19. Pose Estimation,
20. Navigation,
21. Target Definition,
22. Trajectory Definition,
23. Distance Measurement,
24. Angle Measurement,
25. Collision Awareness,
26. Safety Margins,
27. Instrument Calibration,
28. Tool Calibration,
29. Hand-Eye Calibration,
30. Patient Registration,
31. Image Fusion,
32. Multi-Modal Fusion,
33. Intraoperative Imaging,
34. Real-Time Processing,
35. Temporal Synchronization,
36. Latency Measurement,
37. Accuracy Estimation,
38. Uncertainty,
39. Quality Control,
40. Visualization Data,
41. Augmented-Reality Integration,
42. DICOM Integration,
43. Radiology Integration,
44. Ultrasound Integration,
45. Medical-AI Integration,
46. Provenance,
47. Diagnostics,
48. Profiling,
49. Testing,
50. Erweiterbarkeit.

---

## 6. Nicht-Ziele

Diese Spezifikation definiert nicht:

- konkrete chirurgische Verfahren,
- konkrete Therapieentscheidungen,
- autonome Robotersteuerung,
- konkrete Implantate,
- konkrete medizinische Instrumente,
- konkrete Tracking-Hardware,
- konkrete Navigationsgeräte,
- konkrete Benutzeroberflächen,
- regulatorische Zulassung,
- konkrete medizinische Sicherheitsgrenzen.

---

## 7. Spatial Object

Ein räumliches Objekt wird als:

`ImageGuidedSpatialObject`

repräsentiert.

---

## 8. Spatial Identity

Jedes relevante räumliche Objekt MUSS eindeutig identifizierbar sein.

---

## 9. Spatial Object Types

Mindestens folgende Objekttypen SOLLEN unterstützt werden:

```text
Patient
Image
Volume
Anatomy
Target
Instrument
Tool
Sensor
Marker
Tracker
Device
Surface
Trajectory
ReferenceFrame
```

---

## 10. Coordinate System

Ein Koordinatensystem wird als:

`ImageGuidedCoordinateSystem`

repräsentiert.

---

## 11. Coordinate System Identity

Jedes Koordinatensystem MUSS eine eindeutige Identität besitzen.

---

## 12. Coordinate System Metadata

Mindestens folgende Eigenschaften SOLLEN unterstützt werden:

```text
CoordinateSystemId
Name
Dimension
Units
AxisConvention
Handedness
Origin
Parent
```

---

## 13. Coordinate Units

Räumliche Koordinaten MÜSSEN ihre Einheit eindeutig definieren.

---

## 14. Length Unit

Die bevorzugte physikalische Längeneinheit für medizinische Navigation SOLL:

```text
millimeter
```

sein.

---

## 15. Angular Unit

Winkel MÜSSEN eine explizite Einheit besitzen.

---

## 16. Axis Convention

Die Achsenkonvention MUSS eindeutig definiert sein.

---

## 17. Handedness

Links- und rechtshändige Koordinatensysteme MÜSSEN unterscheidbar sein.

---

## 18. Image Coordinate System

Bildkoordinaten MÜSSEN von physikalischen Patientenkoordinaten unterscheidbar sein.

---

## 19. Pixel Coordinates

Pixelkoordinaten SOLLEN als diskrete beziehungsweise kontinuierliche Bildkoordinaten repräsentiert werden können.

---

## 20. Voxel Coordinates

Voxelkoordinaten SOLLEN explizit repräsentierbar sein.

---

## 21. Physical Coordinates

Physikalische Positionen SOLLEN als:

`PhysicalPoint`

repräsentiert werden.

---

## 22. Reference Frame

Ein räumlicher Referenzrahmen wird als:

`ImageGuidedReferenceFrame`

repräsentiert.

---

## 23. Reference Frame Examples

Typische Referenzrahmen sind:

```text
ImageFrame
PatientFrame
TrackerFrame
ToolFrame
DeviceFrame
OperatingRoomFrame
WorldFrame
DisplayFrame
```

---

## 24. Frame Identity

Jeder Referenzrahmen MUSS eindeutig identifizierbar sein.

---

## 25. Frame Lifetime

Ein Referenzrahmen SOLL eine definierte Lebensdauer beziehungsweise Gültigkeit besitzen.

---

## 26. Frame Validity

Ein Referenzrahmen MUSS als gültig oder ungültig markiert werden können.

---

## 27. Pose

Position und Orientierung eines Objekts werden gemeinsam als:

`ImageGuidedPose`

repräsentiert.

---

## 28. Pose Components

Eine Pose MUSS mindestens enthalten können:

```text
Position
Orientation
ReferenceFrame
Timestamp
Validity
```

---

## 29. Orientation

Orientierung SOLL über standardisierte Repräsentationen dargestellt werden können.

---

## 30. Orientation Representations

Mindestens folgende Repräsentationen SOLLEN unterstützt werden:

```text
RotationMatrix
Quaternion
AxisAngle
EulerAngles
```

---

## 31. Canonical Orientation

Intern SOLL eine numerisch robuste kanonische Orientierungsrepräsentation verwendet werden.

---

## 32. Quaternion Normalization

Quaternionen MÜSSEN vor Verwendung auf Gültigkeit geprüft werden.

---

## 33. Transformation

Eine räumliche Transformation wird als:

`ImageGuidedTransform`

repräsentiert.

---

## 34. Transform Direction

Jede Transformation MUSS eindeutig definieren:

```text
SourceFrame
TargetFrame
```

---

## 35. Transform Identity

Transformationen SOLLEN eindeutig identifizierbar sein.

---

## 36. Transform Types

Mindestens folgende Transformationstypen SOLLEN unterstützt werden:

```text
Translation
Rotation
Rigid
Similarity
Affine
Deformable
Composite
```

---

## 37. Rigid Transform

Eine starre Transformation umfasst:

```text
Rotation
Translation
```

---

## 38. Similarity Transform

Eine Similarity Transformation DARF zusätzlich uniforme Skalierung enthalten.

---

## 39. Affine Transform

Affine Transformationen SOLLEN unterstützt werden.

---

## 40. Deformable Transform

Nichtlineare beziehungsweise deformierbare Transformationen SOLLEN unterstützt werden können.

---

## 41. Transform Matrix

Lineare Transformationen SOLLEN als homogene Transformationsmatrizen repräsentiert werden können.

---

## 42. Transform Inverse

Invertierbare Transformationen SOLLEN ihre inverse Transformation bereitstellen können.

---

## 43. Non-Invertible Transform

Eine nicht invertierbare Transformation MUSS als solche erkannt werden.

---

## 44. Transform Composition

Transformationen MÜSSEN komponierbar sein.

---

## 45. Transform Chain

Eine Folge räumlicher Transformationen wird als:

`ImageGuidedTransformChain`

repräsentiert.

---

## 46. Transform Chain Example

```text
Tool
 |
 v
Tracker
 |
 v
Patient
 |
 v
Image
```

---

## 47. Transform Chain Validation

Vor Verwendung MUSS eine Transformationskette auf Konsistenz geprüft werden.

---

## 48. Broken Chain

Eine unvollständige Transformationskette MUSS als ungültig erkannt werden.

---

## 49. Transform Graph

Mehrere Referenzrahmen und Transformationen SOLLEN als:

`ImageGuidedTransformGraph`

repräsentiert werden.

---

## 50. Graph Query

Die Runtime SOLL Transformationen zwischen zwei verbundenen Frames automatisch bestimmen können.

---

## 51. Multiple Paths

Existieren mehrere Transformationspfade, MUSS eine definierte Auswahlstrategie verwendet werden.

---

## 52. Transform Priority

Transformationsquellen SOLLEN Prioritäten besitzen können.

---

## 53. Transform Validity

Jede Transformation MUSS einen Gültigkeitszustand besitzen.

---

## 54. Transform Timestamp

Dynamische Transformationen MÜSSEN einen Zeitstempel besitzen.

---

## 55. Static Transform

Statische Transformationen SOLLEN explizit als statisch gekennzeichnet werden.

---

## 56. Dynamic Transform

Zeitabhängige Transformationen SOLLEN als:

`DynamicTransform`

repräsentiert werden.

---

## 57. Registration

Die Bestimmung einer Transformation zwischen zwei räumlichen Datensätzen wird als:

`ImageGuidedRegistration`

bezeichnet.

---

## 58. Registration Input

Eine Registrierung MUSS ihre Eingabedaten eindeutig referenzieren.

---

## 59. Registration Output

Das Ergebnis einer Registrierung MUSS mindestens enthalten:

```text
Transform
RegistrationMethod
Quality
Timestamp
Provenance
```

---

## 60. Registration Types

Mindestens folgende Verfahren SOLLEN unterstützt werden:

```text
Landmark
Surface
Image
Rigid
Affine
Deformable
Hybrid
```

---

## 61. Landmark Registration

Landmark-basierte Registrierung MUSS unterstützt werden können.

---

## 62. Landmark Pair

Korrespondierende Landmarken werden als:

`RegistrationLandmarkPair`

repräsentiert.

---

## 63. Landmark Minimum

Eine Registrierung MUSS prüfen, ob genügend unabhängige Landmarken vorhanden sind.

---

## 64. Degenerate Landmark Geometry

Geometrisch degenerierte Landmark-Konfigurationen MÜSSEN erkannt werden.

---

## 65. Fiducial Registration Error

Der Fiducial Registration Error wird als:

`FRE`

repräsentiert.

---

## 66. Target Registration Error

Der Target Registration Error wird als:

`TRE`

repräsentiert.

---

## 67. FRE vs TRE

`FRE` DARF nicht automatisch mit `TRE` gleichgesetzt werden.

---

## 68. Surface Registration

Oberflächenbasierte Registrierung SOLL unterstützt werden.

---

## 69. Surface Input

Oberflächen können beispielsweise stammen aus:

```text
Image Segmentation
Point Cloud
Optical Scanner
Tracked Probe
Depth Sensor
```

---

## 70. Surface Matching

Iterative und direkte Surface-Matching-Verfahren SOLLEN integrierbar sein.

---

## 71. Image Registration

Bildbasierte Registrierung SOLL unterstützt werden.

---

## 72. Image Registration Modalities

Registrierung SOLL sowohl:

```text
Monomodal
Multimodal
```

unterstützen können.

---

## 73. Registration Metric

Das verwendete Ähnlichkeitsmaß MUSS dokumentiert werden.

---

## 74. Registration Optimizer

Der verwendete Optimierer MUSS dokumentierbar sein.

---

## 75. Registration Pyramid

Multi-Resolution-Registrierung SOLL unterstützt werden.

---

## 76. Registration Mask

Registrierungen SOLLEN auf definierte Regionen begrenzt werden können.

---

## 77. Deformable Registration

Deformierbare Registrierung SOLL unterstützt werden können.

---

## 78. Deformation Field

Ein Deformationsfeld wird als:

`ImageGuidedDeformationField`

repräsentiert.

---

## 79. Deformation Geometry

Ein Deformationsfeld MUSS seine räumliche Geometrie definieren.

---

## 80. Deformation Validity

Deformationsfelder SOLLEN auf physikalische Plausibilität geprüft werden können.

---

## 81. Folding Detection

Nichtphysikalische lokale Faltungen SOLLEN erkannt werden können.

---

## 82. Registration Quality

Registrierungsergebnisse MÜSSEN Qualitätsinformationen transportieren können.

---

## 83. Registration Quality Metrics

Mögliche Metriken umfassen:

```text
FRE
EstimatedTRE
SurfaceDistance
MeanDistance
MaximumDistance
SimilarityMetric
OverlapMetric
JacobianStatistics
```

---

## 84. Registration Acceptance

Eine Registrierung SOLL über eine explizite Policy akzeptiert oder verworfen werden können.

---

## 85. No Silent Acceptance

Eine Registrierung DARF nicht allein aufgrund der technischen Konvergenz automatisch als medizinisch ausreichend genau gelten.

---

## 86. Patient Registration

Die Zuordnung des realen Patienten zu Bilddaten wird als:

`PatientRegistration`

repräsentiert.

---

## 87. Patient Registration Methods

Mögliche Methoden umfassen:

```text
Fiducial
Surface
AnatomicalLandmark
ImageBased
Hybrid
```

---

## 88. Registration State

Mindestens folgende Zustände SOLLEN unterstützt werden:

```text
UNREGISTERED
REGISTERING
REGISTERED
DEGRADED
INVALID
```

---

## 89. Registration Invalidity

Bei Verlust einer für die Navigation erforderlichen Registrierung MUSS der Navigationszustand entsprechend ungültig werden.

---

## 90. Tracking

Die kontinuierliche Bestimmung räumlicher Objektposen wird als:

`ImageGuidedTracking`

bezeichnet.

---

## 91. Tracker

Ein Tracking-System wird als:

`ImageGuidedTracker`

repräsentiert.

---

## 92. Tracker Types

Mindestens folgende Tracker-Typen SOLLEN unterstützt werden können:

```text
Optical
Electromagnetic
Mechanical
Inertial
ImageBased
Hybrid
Custom
```

---

## 93. Tracker Coordinate Frame

Jeder Tracker MUSS seinen eigenen Referenzrahmen definieren.

---

## 94. Tracking Sample

Eine einzelne Tracking-Messung wird als:

`TrackingSample`

repräsentiert.

---

## 95. Tracking Sample Fields

Mindestens folgende Felder SOLLEN unterstützt werden:

```text
ObjectId
Pose
Timestamp
SequenceNumber
Quality
Validity
```

---

## 96. Tracking Frequency

Tracking-Systeme SOLLEN ihre aktuelle Messfrequenz bereitstellen können.

---

## 97. Tracking Stream

Kontinuierliche Tracking-Daten werden als:

`TrackingStream`

repräsentiert.

---

## 98. Tracking Loss

Der Verlust eines getrackten Objekts MUSS explizit signalisiert werden.

---

## 99. Stale Tracking

Veraltete Tracking-Daten MÜSSEN erkannt werden.

---

## 100. Tracking Timeout

Ein maximales Alter gültiger Tracking-Daten SOLL konfigurierbar sein.

---

## 101. Optical Tracking

Optische Tracking-Systeme SOLLEN unterstützt werden.

---

## 102. Optical Visibility

Die Sichtbarkeit optischer Marker MUSS berücksichtigt werden können.

---

## 103. Occlusion

Marker-Occlusion MUSS als Tracking-Qualitätsproblem erkannt werden können.

---

## 104. Electromagnetic Tracking

Elektromagnetische Tracking-Systeme SOLLEN unterstützt werden.

---

## 105. EM Distortion

Elektromagnetische Verzerrungen SOLLEN als potenzielle Qualitätsquelle modellierbar sein.

---

## 106. Instrument

Ein medizinisches Instrument wird als:

`ImageGuidedInstrument`

repräsentiert.

---

## 107. Instrument Identity

Jedes Instrument MUSS eindeutig identifizierbar sein.

---

## 108. Instrument Geometry

Die relevante Instrumentengeometrie SOLL gespeichert werden können.

---

## 109. Instrument Axis

Instrumente SOLLEN eine definierte Hauptachse besitzen können.

---

## 110. Tool Tip

Die aktive Instrumentenspitze wird als:

`ToolTip`

repräsentiert.

---

## 111. Tool Tip Position

Die Position der Instrumentenspitze MUSS aus der Instrumentenpose berechenbar sein.

---

## 112. Tool Calibration

Die Transformation zwischen Tracking-Sensor und funktionaler Instrumentengeometrie MUSS kalibrierbar sein.

---

## 113. Pivot Calibration

Pivot Calibration SOLL unterstützt werden.

---

## 114. Axis Calibration

Instrumentenachsen SOLLEN kalibriert werden können.

---

## 115. Calibration Result

Ein Kalibrierungsergebnis MUSS mindestens enthalten:

```text
Transform
Method
Error
Timestamp
DeviceIdentity
```

---

## 116. Calibration Validity

Kalibrierungen MÜSSEN einen Gültigkeitsstatus besitzen.

---

## 117. Calibration Expiration

Eine Kalibrierung SOLL zeitlich oder ereignisbasiert ungültig werden können.

---

## 118. Device Change

Ein Instrumenten- oder Sensorwechsel MUSS eine davon abhängige Kalibrierung invalidieren können.

---

## 119. Hand-Eye Calibration

Transformationen zwischen Bildsensoren, Tracking-Systemen und Geräten SOLLEN kalibrierbar sein.

---

## 120. Navigation

Die räumliche Zuordnung zwischen Patient, Bilddaten, Instrumenten und Zielen wird als:

`ImageGuidedNavigation`

bezeichnet.

---

## 121. Navigation Session

Ein zusammengehöriger Navigationsvorgang wird als:

`ImageGuidedNavigationSession`

repräsentiert.

---

## 122. Session State

Mindestens folgende Zustände SOLLEN unterstützt werden:

```text
INITIALIZING
READY
ACTIVE
DEGRADED
PAUSED
INVALID
COMPLETED
```

---

## 123. Navigation Preconditions

Vor Aktivierung SOLLEN mindestens geprüft werden:

```text
ImageAvailable
PatientRegistered
TrackerAvailable
InstrumentCalibrated
TransformChainValid
TimingValid
```

---

## 124. Navigation Validity

Die Navigation MUSS ungültig werden können, wenn eine notwendige Voraussetzung nicht mehr erfüllt ist.

---

## 125. Target

Ein Navigationsziel wird als:

`ImageGuidedTarget`

repräsentiert.

---

## 126. Target Types

Ziele SOLLEN repräsentiert werden können als:

```text
Point
Region
Surface
Volume
TrajectoryEndpoint
```

---

## 127. Target Coordinate System

Jedes Ziel MUSS eindeutig einem Koordinatensystem zugeordnet sein.

---

## 128. Target Source

Die Herkunft eines Ziels MUSS nachvollziehbar sein.

---

## 129. Target Provenance

Ein Ziel SOLL referenzieren können:

```text
ManualDefinition
Segmentation
RadiologyMeasurement
MedicalAIResult
ImportedPlan
```

---

## 130. Trajectory

Eine geplante räumliche Bahn wird als:

`ImageGuidedTrajectory`

repräsentiert.

---

## 131. Linear Trajectory

Eine lineare Trajektorie SOLL mindestens enthalten:

```text
EntryPoint
TargetPoint
Direction
Length
```

---

## 132. Curved Trajectory

Nichtlineare Trajektorien SOLLEN architektonisch unterstützt werden können.

---

## 133. Trajectory Coordinate System

Eine Trajektorie MUSS eindeutig einem Referenzrahmen zugeordnet sein.

---

## 134. Trajectory Validation

Trajektorien SOLLEN geometrisch validiert werden.

---

## 135. Distance to Target

Die Distanz zwischen Instrument und Ziel MUSS berechnet werden können.

---

## 136. Lateral Deviation

Die seitliche Abweichung von einer geplanten Trajektorie SOLL berechnet werden können.

---

## 137. Angular Deviation

Die Winkelabweichung von einer geplanten Trajektorie SOLL berechnet werden können.

---

## 138. Depth

Die Position entlang einer geplanten Trajektorie SOLL berechnet werden können.

---

## 139. Entry Point Distance

Die Distanz zum geplanten Eintrittspunkt SOLL berechnet werden können.

---

## 140. Guidance Metrics

Ein Navigationszustand SOLL beispielsweise enthalten können:

```text
DistanceToTarget
DistanceToEntry
LateralDeviation
AngularDeviation
Depth
TrackingQuality
RegistrationQuality
```

---

## 141. Safety Margin

Räumliche Sicherheitsabstände SOLLEN als:

`ImageGuidedSafetyMargin`

repräsentiert werden können.

---

## 142. Safety Region

Eine Sicherheitsregion SOLL um anatomische oder technische Strukturen definiert werden können.

---

## 143. Safety Margin Units

Sicherheitsabstände MÜSSEN physikalische Einheiten besitzen.

---

## 144. Safety Margin Source

Es MUSS nachvollziehbar sein, wer oder welche Komponente einen Sicherheitsabstand definiert hat.

---

## 145. Proximity Calculation

Die Distanz eines Instruments zu definierten Strukturen SOLL in Echtzeit berechnet werden können.

---

## 146. Proximity State

Mögliche Zustände:

```text
SAFE
NEAR
INSIDE_MARGIN
UNKNOWN
```

---

## 147. Unknown Is Not Safe

Ein unbekannter Abstand DARF nicht als sicher interpretiert werden.

---

## 148. Collision Awareness

Image-Guided Compute SOLL geometrische Kollisionsprüfungen unterstützen.

---

## 149. Collision Objects

Kollisionsprüfungen SOLLEN zwischen folgenden Objekten möglich sein:

```text
Instrument
PatientGeometry
AnatomicalStructure
Device
PlannedTrajectory
SafetyRegion
```

---

## 150. Collision Prediction

Eine zeitlich begrenzte Kollisionsvorhersage SOLL architektonisch unterstützt werden können.

---

## 151. Collision Awareness Boundary

Eine berechnete Kollisionswarnung DARF nicht mit einer garantierten physischen Kollisionsvermeidung gleichgesetzt werden.

---

## 152. Image Fusion

Mehrere Bilddatensätze SOLLEN räumlich fusioniert werden können.

---

## 153. Fusion Types

Mindestens folgende Fusionen SOLLEN unterstützt werden:

```text
CT + MR
CT + PET
MR + PET
Preoperative + Intraoperative
Ultrasound + CT
Ultrasound + MR
```

---

## 154. Fusion Registration

Bildfusion MUSS auf einer expliziten Registrierung basieren.

---

## 155. Fusion Provenance

Die verwendete Registrierung MUSS referenzierbar sein.

---

## 156. Original Data Preservation

Fusion DARF Originaldaten nicht verändern.

---

## 157. Intraoperative Imaging

Image-Guided Compute SOLL intraoperative Bilddaten integrieren können.

---

## 158. Intraoperative Update

Neue Bilddaten SOLLEN während einer aktiven Navigationssitzung eingespielt werden können.

---

## 159. Registration Update

Neue intraoperative Bilddaten DÜRFEN eine Aktualisierung der Registrierung auslösen.

---

## 160. Transform Versioning

Wenn eine Registrierung aktualisiert wird, MUSS eine neue Transformationsversion entstehen.

---

## 161. No Silent Transform Replacement

Eine aktive Transformation DARF nicht stillschweigend durch eine neue Registrierung ersetzt werden.

---

## 162. Navigation Epoch

Eine Menge zusammengehöriger Transformationsversionen SOLL als:

`NavigationEpoch`

repräsentiert werden können.

---

## 163. Epoch Change

Eine wesentliche Änderung der räumlichen Beziehung SOLL eine neue Navigation Epoch erzeugen.

---

## 164. Temporal Model

Image-Guided Compute MUSS Zeit als expliziten Bestandteil räumlicher Daten behandeln.

---

## 165. Timestamp

Dynamische Daten MÜSSEN Zeitstempel besitzen.

---

## 166. Monotonic Time

Für Laufzeit- und Latenzmessungen SOLL eine monotone Zeitquelle verwendet werden.

---

## 167. Wall Clock

Kalenderzeit DARF zusätzlich für Audit und Provenance verwendet werden.

---

## 168. Clock Domain

Bei mehreren Geräten MÜSSEN unterschiedliche Clock Domains berücksichtigt werden können.

---

## 169. Clock Synchronization

Zeitquellen SOLLEN synchronisiert werden können.

---

## 170. Clock Offset

Zeitversätze zwischen Geräten SOLLEN messbar sein.

---

## 171. Temporal Alignment

Bild-, Tracking- und Sensordaten SOLLEN zeitlich aufeinander ausgerichtet werden können.

---

## 172. Interpolation

Tracking-Posen SOLLEN zeitlich interpoliert werden können.

---

## 173. Extrapolation

Kurzzeitige Pose-Extrapolation DARF unterstützt werden.

---

## 174. Extrapolation Limit

Extrapolation MUSS ein definiertes maximales Zeitfenster besitzen.

---

## 175. Stale Data

Daten außerhalb ihres zulässigen Alters MÜSSEN als stale markiert werden.

---

## 176. Latency

Image-Guided Compute MUSS relevante Latenzen messen können.

---

## 177. Latency Components

Mindestens folgende Latenzen SOLLEN unterscheidbar sein:

```text
SensorLatency
TrackingLatency
TransferLatency
ProcessingLatency
RegistrationLatency
RenderingLatency
EndToEndLatency
```

---

## 178. Latency Budget

Echtzeitpipelines SOLLEN ein Latenzbudget definieren können.

---

## 179. Deadline

Latenzkritische Tasks SOLLEN Deadlines besitzen können.

---

## 180. Deadline Miss

Verpasste Deadlines MÜSSEN diagnostizierbar sein.

---

## 181. Stale Visualization

Eine Visualisierung, die auf veralteten Tracking-Daten basiert, MUSS als solche erkennbar sein.

---

## 182. Real-Time Compute

Image-Guided Compute SOLL Echtzeitverarbeitung unterstützen.

---

## 183. Real-Time Pipeline

Eine typische Echtzeitpipeline lautet:

```text
Tracker
   |
   v
Tracking Sample
   |
   v
Temporal Alignment
   |
   v
Transform Graph
   |
   v
Navigation Compute
   |
   v
Guidance State
   |
   v
Visualization
```

---

## 184. Scheduler Integration

Image-Guided Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 185. Priority Classes

Image-Guided Tasks SOLLEN mindestens folgende Prioritäten verwenden können:

```text
BACKGROUND
NORMAL
INTERACTIVE
URGENT
REALTIME
```

---

## 186. Tracking Priority

Tracking- und Navigationsupdates SOLLEN gegenüber nicht zeitkritischen Hintergrundberechnungen priorisiert werden können.

---

## 187. Background Processing

Nicht zeitkritische Aufgaben wie:

```text
Preoperative Segmentation
Radiomics
Large Registration
Planning
Export
```

SOLLTEN Navigation nicht unnötig blockieren.

---

## 188. Task Graph

Eine Navigation SOLL als Task Graph repräsentiert werden können.

```text
Acquire Tracking
       |
       v
Validate
       |
       v
Synchronize
       |
       v
Transform
       |
       v
Navigation Metrics
       |
       v
Safety Checks
       |
       v
Guidance Output
```

---

## 189. Memory Model

Image-Guided Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 190. Memory Objects

Mindestens folgende Objekte SOLLEN unterstützt werden:

```text
ImageBuffer
VolumeBuffer
TrackingBuffer
TransformBuffer
SurfaceBuffer
PointCloudBuffer
TrajectoryBuffer
NavigationStateBuffer
DeformationFieldBuffer
```

---

## 191. Tracking Ring Buffer

Tracking-Daten SOLLEN in einem zeitlich geordneten Ring Buffer gespeichert werden können.

---

## 192. Buffer Timestamp

Jeder Tracking-Eintrag MUSS seinen Zeitstempel behalten.

---

## 193. Snapshot

Ein konsistenter räumlich-zeitlicher Zustand SOLL als:

`ImageGuidedSnapshot`

repräsentiert werden können.

---

## 194. Snapshot Consistency

Ein Snapshot MUSS zusammengehörige Transformations- und Tracking-Versionen referenzieren.

---

## 195. CPU Backend

Image-Guided Compute MUSS CPU-Ausführung unterstützen.

---

## 196. SIMD

Geeignete mathematische Operationen SOLLEN SIMD verwenden können.

---

## 197. GPU Backend

Image-Guided Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` integrieren.

---

## 198. GPU Suitable Operations

Geeignete GPU-Aufgaben umfassen:

```text
Image Registration
Deformable Registration
Volume Resampling
Volume Rendering Preparation
Image Fusion
Segmentation
Surface Processing
Distance Fields
Collision Fields
```

---

## 199. GPU Optionality

Grundlegende Navigation DARF nicht zwingend von einer GPU abhängen.

---

## 200. NPU Integration

`NPSPEC-COMPUTE-RUNTIME-0006` SOLL für KI-basierte Funktionen integrierbar sein.

---

## 201. AI Tracking

KI-basierte Marker-, Instrumenten- oder Anatomieerkennung SOLL über Medical AI Compute integrierbar sein.

---

## 202. Medical AI Integration

Image-Guided Compute SOLL `NPSPEC-COMPUTE-MEDAI-0001` integrieren.

---

## 203. AI Segmentation

KI-generierte Segmentierungen SOLLEN als Navigationsstrukturen verwendet werden können.

---

## 204. AI Landmark Detection

KI-generierte Landmarken SOLLEN als Registrierungsinput verwendet werden können.

---

## 205. AI Pose Estimation

KI-basierte Pose-Schätzungen SOLLEN als räumliche Messungen eingebunden werden können.

---

## 206. AI Result Status

Ein AI-generiertes räumliches Ergebnis MUSS seinen Medical-AI-Result-Status behalten.

---

## 207. AI Confidence

Konfidenzwerte SOLLEN in die Qualitätsbewertung einfließen können.

---

## 208. AI Is Not Ground Truth

Ein AI-generierter Landmark oder eine AI-generierte Segmentierung DARF nicht automatisch als Ground Truth behandelt werden.

---

## 209. Ultrasound Integration

Image-Guided Compute SOLL `NPSPEC-COMPUTE-ULTRASOUND-0001` integrieren.

---

## 210. Tracked Ultrasound

Getrackter Ultraschall SOLL unterstützt werden.

---

## 211. Probe Frame

Eine Ultraschallsonde SOLL einen eigenen:

`ProbeFrame`

besitzen.

---

## 212. Ultrasound Image Frame

Das Ultraschallbild MUSS einem definierten Bildreferenzrahmen zugeordnet sein.

---

## 213. Probe Calibration

Die Transformation zwischen Ultraschallbild und Tracking-Sensor MUSS kalibrierbar sein.

---

## 214. Freehand 3D Ultrasound

Getrackte 2D-Ultraschallbilder SOLLEN zu einem 3D-Volumen rekonstruiert werden können.

---

## 215. Ultrasound Reconstruction

Die Rekonstruktionsparameter MÜSSEN dokumentiert werden.

---

## 216. Ultrasound Temporal Alignment

Ultraschallbilder und Tracking-Daten MÜSSEN zeitlich synchronisierbar sein.

---

## 217. DICOM Integration

Image-Guided Compute MUSS `NPSPEC-COMPUTE-DICOM-0001` integrieren können.

---

## 218. DICOM Geometry

DICOM-basierte Bildgeometrie MUSS korrekt in den Transformationsgraphen übernommen werden können.

---

## 219. DICOM Coordinate Convention

Die verwendete DICOM-Koordinatenkonvention MUSS berücksichtigt werden.

---

## 220. DICOM Derived Objects

Navigationsbezogene abgeleitete Daten SOLLEN, soweit geeignet, in standardisierte medizinische Objekte überführt werden können.

---

## 221. PACS Integration

Image-Guided Compute SOLL `NPSPEC-COMPUTE-PACS-0001` integrieren.

---

## 222. Preoperative Study

Präoperative Studien SOLLEN aus PACS geladen werden können.

---

## 223. Intraoperative Study

Intraoperative Bilddaten SOLLEN in denselben Navigationskontext integriert werden können.

---

## 224. Original Preservation

Originale PACS-Daten DÜRFEN durch Navigation nicht verändert werden.

---

## 225. Radiology Integration

Image-Guided Compute MUSS `NPSPEC-COMPUTE-RADIOLOGY-0001` integrieren.

---

## 226. Shared Radiology Services

Mindestens folgende Dienste SOLLEN wiederverwendet werden:

```text
Image Geometry
Volume Geometry
Resampling
Registration
Segmentation
Measurement
Surface Generation
Volume Processing
```

---

## 227. Radiomics Integration

`NPSPEC-COMPUTE-RADIOMICS-0001` DARF für quantitative Analyse navigierter Regionen verwendet werden.

---

## 228. Augmented Reality

Image-Guided Compute SOLL Augmented-Reality-Systeme mit räumlichen Guidance-Daten versorgen können.

---

## 229. Display Frame

Ein AR- oder Display-System SOLL einen eigenen Referenzrahmen besitzen.

---

## 230. View Transform

Die Transformation zwischen Navigation und Display MUSS explizit sein.

---

## 231. AR Calibration

Display- beziehungsweise Kamera-Kalibrierung MUSS nachvollziehbar sein.

---

## 232. Visualization Geometry

Visualisierungsdaten MÜSSEN ihre räumliche Herkunft behalten.

---

## 233. Visualization State

Ein Visualisierungszustand SOLL mindestens enthalten können:

```text
NavigationEpoch
TransformVersion
TrackingTimestamp
RegistrationState
TrackingState
Latency
```

---

## 234. Rendering Boundary

Diese Spezifikation definiert räumliche Daten für Visualisierung, nicht die konkrete Rendering Engine.

---

## 235. Uncertainty

Image-Guided Compute MUSS räumliche Unsicherheit repräsentieren können.

---

## 236. Position Uncertainty

Positionsunsicherheit SOLL in physikalischen Einheiten darstellbar sein.

---

## 237. Orientation Uncertainty

Orientierungsunsicherheit SOLL darstellbar sein.

---

## 238. Registration Uncertainty

Registrierungsunsicherheit SOLL repräsentierbar sein.

---

## 239. Tracking Uncertainty

Tracking-Systeme SOLLEN Qualitäts- beziehungsweise Unsicherheitsinformationen bereitstellen können.

---

## 240. Transform Uncertainty

Unsicherheiten SOLLEN entlang einer Transformationskette propagierbar sein.

---

## 241. Composite Uncertainty

Die Gesamtunsicherheit einer navigierten Position SOLL aus mehreren Unsicherheitsquellen bestimmt werden können.

---

## 242. Uncertainty Sources

Mögliche Quellen umfassen:

```text
Image Resolution
Segmentation
Landmark Localization
Registration
Tracking
Calibration
Temporal Alignment
Device Noise
Deformation
```

---

## 243. Accuracy

Image-Guided Compute MUSS zwischen Genauigkeit, Präzision und Auflösung unterscheiden können.

---

## 244. Accuracy Estimate

Eine geschätzte räumliche Genauigkeit SOLL zusammen mit Guidance-Daten bereitgestellt werden können.

---

## 245. Unknown Accuracy

Ist keine belastbare Genauigkeit bekannt, MUSS dieser Zustand explizit repräsentierbar sein.

---

## 246. Quality Model

Ein Image-Guided-Zustand SOLL eine kombinierte Qualitätsbewertung besitzen können.

---

## 247. Quality Inputs

Eine Qualitätsbewertung kann berücksichtigen:

```text
RegistrationQuality
TrackingQuality
CalibrationQuality
TimingQuality
ImageQuality
TransformValidity
AIConfidence
```

---

## 248. Quality States

Mindestens:

```text
GOOD
ACCEPTABLE
DEGRADED
INVALID
UNKNOWN
```

SOLLTEN unterstützt werden.

---

## 249. Unknown Quality

`UNKNOWN` DARF nicht automatisch als `GOOD` behandelt werden.

---

## 250. Provenance

Image-Guided Compute MUSS vollständige Provenance unterstützen.

---

## 251. Provenance Record

Ein:

`ImageGuidedProvenanceRecord`

SOLL mindestens enthalten können:

```text
SessionId
InputImages
ImageVersions
RegistrationId
RegistrationVersion
TransformChain
TrackerId
InstrumentId
CalibrationId
NavigationEpoch
RuntimeVersion
Backend
Timestamp
```

---

## 252. Extended Provenance

Zusätzlich SOLLEN gespeichert werden können:

```text
TrackingQuality
RegistrationQuality
CalibrationError
Latency
AIModelReference
SegmentationReference
TargetReference
TrajectoryReference
```

---

## 253. Transform Provenance

Jede relevante Transformation SOLL ihre Herkunft dokumentieren.

---

## 254. Transform Source Types

Mögliche Quellen:

```text
Calibration
Registration
Tracking
Manual
ImageMetadata
DeviceConfiguration
Derived
```

---

## 255. Navigation Reproducibility

Historische Navigationszustände SOLLEN anhand gespeicherter Daten soweit technisch möglich rekonstruierbar sein.

---

## 256. Event Recording

Relevante Zustandsänderungen SOLLEN als Ereignisse gespeichert werden können.

---

## 257. Navigation Events

Mindestens folgende Ereignisse SOLLEN unterstützt werden:

```text
SESSION_STARTED
REGISTRATION_CREATED
REGISTRATION_INVALIDATED
TRACKING_STARTED
TRACKING_LOST
TRACKING_RECOVERED
CALIBRATION_CHANGED
TOOL_CHANGED
TARGET_CHANGED
TRAJECTORY_CHANGED
NAVIGATION_DEGRADED
NAVIGATION_INVALID
SESSION_COMPLETED
```

---

## 258. Security

Image-Guided Compute MUSS in die NovaOS-Sicherheitsarchitektur integriert werden.

---

## 259. Authentication

Aufrufende Benutzer und Dienste MÜSSEN authentifizierbar sein.

---

## 260. Authorization

Zugriffe auf medizinische Bild-, Navigations- und Tracking-Daten MÜSSEN autorisierbar sein.

---

## 261. Device Authorization

Medizinische Geräte SOLLEN eine technische Geräteidentität besitzen können.

---

## 262. Least Privilege

Komponenten SOLLEN nur auf benötigte Daten und Geräte zugreifen.

---

## 263. Network Isolation

Echtzeit-Navigationskomponenten SOLLEN von unnötigem Netzwerkzugriff isoliert werden können.

---

## 264. Data Integrity

Navigationsrelevante Transformations-, Tracking- und Kalibrierungsdaten MÜSSEN gegen unbeabsichtigte Manipulation geschützt werden.

---

## 265. Configuration Integrity

Sicherheitsrelevante Konfigurationen SOLLEN fingerprintbar sein.

---

## 266. Privacy

Tracking- und Navigationsdaten MÜSSEN als potenziell medizinisch sensible Daten behandelt werden.

---

## 267. Logging Privacy

Logs DÜRFEN keine unnötigen Patientendaten enthalten.

---

## 268. Audit

Relevante Image-Guided-Operationen MÜSSEN auditierbar sein können.

---

## 269. Audit Events

Mindestens folgende Ereignisse SOLLEN auditierbar sein:

```text
SESSION_START
SESSION_END
PATIENT_DATA_ACCESS
REGISTRATION_ACCEPT
REGISTRATION_REJECT
CALIBRATION_CHANGE
TRACKER_CHANGE
TOOL_CHANGE
TARGET_CHANGE
TRAJECTORY_CHANGE
NAVIGATION_INVALIDATION
DATA_EXPORT
```

---

## 270. Fail-Safe

Bei Verlust kritischer Navigationsinformationen MUSS ein definierter Fail-Safe-Zustand existieren.

---

## 271. Critical Inputs

Kritische Eingaben können umfassen:

```text
PatientRegistration
Tracking
ToolCalibration
TransformChain
TemporalSynchronization
```

---

## 272. Critical Input Loss

Der Verlust einer erforderlichen kritischen Eingabe MUSS die abhängige Guidance invalidieren.

---

## 273. No Last-Known-Pose as Current

Die letzte bekannte Pose DARF nach Tracking-Verlust nicht ohne Kennzeichnung als aktuelle Pose verwendet werden.

---

## 274. No Invalid Transform Use

Ungültige Transformationen DÜRFEN nicht für aktive Navigation verwendet werden.

---

## 275. Fail-Safe State

Ein Fail-Safe-Zustand SOLL mindestens:

```text
GuidanceValid = false
Reason
Timestamp
AffectedObjects
```

bereitstellen.

---

## 276. Recovery

Nach Wiederherstellung kritischer Eingaben MUSS die Gültigkeit erneut geprüft werden.

---

## 277. Automatic Recovery

Ein System DARF Navigation automatisch wieder freigeben, wenn eine definierte Recovery Policy dies erlaubt und alle Voraussetzungen erneut validiert wurden.

---

## 278. Diagnostics

Image-Guided Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 279. Diagnostic Classes

Mindestens folgende Klassen SOLLEN existieren:

```text
INVALID_COORDINATE_SYSTEM
INVALID_TRANSFORM
NON_INVERTIBLE_TRANSFORM
BROKEN_TRANSFORM_CHAIN
TRANSFORM_EXPIRED
REGISTRATION_FAILED
REGISTRATION_DEGRADED
REGISTRATION_INVALID
TRACKER_UNAVAILABLE
TRACKING_LOST
TRACKING_STALE
TRACKING_LOW_QUALITY
CALIBRATION_MISSING
CALIBRATION_INVALID
CALIBRATION_EXPIRED
TOOL_UNKNOWN
TARGET_INVALID
TRAJECTORY_INVALID
TEMPORAL_SYNC_FAILED
LATENCY_EXCEEDED
DEADLINE_MISSED
COLLISION_WARNING
SAFETY_MARGIN_VIOLATION
IMAGE_UNAVAILABLE
GEOMETRY_MISMATCH
AI_RESULT_INVALID
OUT_OF_MEMORY
BACKEND_FAILURE
CANCELLED
```

---

## 280. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 281. Diagnostic Context

Diagnosen SOLLEN enthalten können:

```text
SessionId
ObjectId
FrameId
TransformId
TrackerId
ToolId
Timestamp
```

---

## 282. Real-Time Diagnostics

Kritische Echtzeitdiagnosen SOLLEN ohne blockierende I/O-Operationen erzeugt werden können.

---

## 283. Profiling

Image-Guided Compute MUSS Profiling unterstützen.

---

## 284. Profiling Stages

Mindestens:

```text
ImageLoad
Registration
TrackingInput
TemporalAlignment
TransformResolution
NavigationCompute
SafetyCheck
Fusion
VisualizationPreparation
```

SOLLTEN separat messbar sein.

---

## 285. End-to-End Latency

Die gesamte Guidance-Latenz SOLL messbar sein.

---

## 286. Transform Resolution Time

Die Zeit zur Auflösung einer Transformationskette SOLL messbar sein.

---

## 287. Tracking Jitter

Zeitliche und räumliche Schwankungen von Tracking-Daten SOLLEN messbar sein.

---

## 288. Registration Time

Registrierungsdauer SOLL messbar sein.

---

## 289. Frame Rate

Die Aktualisierungsrate der Guidance-Pipeline SOLL messbar sein.

---

## 290. Dropped Updates

Verworfene Tracking- oder Guidance-Updates SOLLEN gezählt werden.

---

## 291. Testing — Coordinate Systems

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
IdentityTransform
Translation
Rotation
RigidTransform
AffineTransform
InverseTransform
TransformComposition
```

---

## 292. Testing — Transform Graph

Mindestens:

```text
DirectPath
MultiHopPath
BrokenPath
MultiplePaths
InvalidTransform
ExpiredTransform
```

MÜSSEN getestet werden.

---

## 293. Testing — Registration

Mindestens:

```text
LandmarkRegistration
SurfaceRegistration
RigidImageRegistration
AffineRegistration
RegistrationFailure
```

SOLLTEN getestet werden.

---

## 294. Testing — Landmark Geometry

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
ValidLandmarks
InsufficientLandmarks
DuplicateLandmarks
CollinearLandmarks
NoisyLandmarks
```

---

## 295. Testing — Tracking

Mindestens:

```text
ValidTracking
TrackingLoss
TrackingRecovery
StaleTracking
LowQualityTracking
OutOfOrderSamples
```

MÜSSEN getestet werden.

---

## 296. Testing — Calibration

Mindestens:

```text
ValidCalibration
MissingCalibration
ExpiredCalibration
DeviceChanged
PivotCalibration
```

SOLLTEN getestet werden.

---

## 297. Testing — Navigation

Mindestens:

```text
ValidNavigation
DistanceToTarget
LateralDeviation
AngularDeviation
InvalidRegistration
TrackingLossDuringNavigation
```

MÜSSEN getestet werden.

---

## 298. Testing — Temporal Synchronization

Mindestens:

```text
Synchronized
KnownOffset
ClockDrift
MissingTimestamp
StaleData
```

SOLLTEN getestet werden.

---

## 299. Testing — Latency

Echtzeitpipelines MÜSSEN unter definierten Latenzbedingungen getestet werden können.

---

## 300. Testing — Ultrasound

Tracked Ultrasound SOLL mit bekannten Probe- und Bildtransformationen getestet werden.

---

## 301. Testing — AI Integration

AI-generierte Landmarken und Segmentierungen SOLLEN mit gültigen und ungültigen Result States getestet werden.

---

## 302. Testing — Safety Margin

Mindestens:

```text
OutsideMargin
NearMargin
InsideMargin
UnknownDistance
```

MÜSSEN getestet werden.

---

## 303. Testing — Collision

Geometrische Kollisionsprüfungen SOLLEN gegen analytisch bekannte Szenarien getestet werden.

---

## 304. Testing — Fail-Safe

Mindestens folgende Fehler MÜSSEN zur korrekten Guidance-Invalidierung führen:

```text
TrackingLoss
RegistrationInvalid
CalibrationInvalid
BrokenTransformChain
StaleTracking
```

---

## 305. Testing — Provenance

Jeder relevante Guidance-Zustand SOLL auf die verwendeten räumlichen Daten zurückführbar sein.

---

## 306. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
HighFrequencyTracking
ManyTrackedTools
LargeTransformGraph
RapidToolSwitching
RepeatedRegistrationUpdates
Large3DVolumes
DeformableRegistration
GPUFailure
TrackingJitter
ClockDrift
NetworkDelay
MemoryPressure
LongNavigationSession
```

---

## 307. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
ImageGuidedSpatialObject
ImageGuidedCoordinateSystem
ImageGuidedReferenceFrame
ImageGuidedPose

ImageGuidedTransform
Rigid Transform
Affine Transform
Transform Inverse
Transform Composition
Transform Chain
Transform Graph

Landmark Registration
Rigid Registration
Registration Quality

Patient Registration

ImageGuidedTracker
TrackingSample
TrackingStream
Tracking Loss Detection
Stale Tracking Detection

ImageGuidedInstrument
Tool Tip
Tool Calibration

ImageGuidedNavigationSession
ImageGuidedTarget
Linear Trajectory

Distance To Target
Lateral Deviation
Angular Deviation

Temporal Synchronization
Latency Measurement

Navigation Validity
Fail-Safe State

CPU Execution
Scheduler Integration
Memory Model Integration
Object Model Integration

DICOM Integration
Radiology Integration

Provenance
Diagnostics
Profiling
Audit
```

---

## 308. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Surface Registration
Image Registration
Multimodal Registration
Deformable Registration

Optical Tracking
Electromagnetic Tracking
Hybrid Tracking

Pivot Calibration
Axis Calibration
Hand-Eye Calibration

Curved Trajectories
Safety Margins
Collision Awareness
Collision Prediction

Multi-Modal Image Fusion
Intraoperative Image Updates
Navigation Epochs

Tracked Ultrasound
Freehand 3D Ultrasound

GPU Acceleration
NPU Integration
Medical AI Integration

AI Landmark Detection
AI Segmentation
AI Pose Estimation

Augmented Reality Integration

Spatial Uncertainty Propagation
Composite Accuracy Estimation

Distributed Preprocessing
HPC Preoperative Processing
```

---

## 309. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-IMAGEGUIDED-0001` MUSS:

1. räumliche Objekte eindeutig identifizieren.
2. Koordinatensysteme explizit repräsentieren.
3. Referenzrahmen eindeutig identifizieren.
4. Einheiten explizit verwalten.
5. Achsenkonventionen verwalten können.
6. Positionen repräsentieren.
7. Orientierungen repräsentieren.
8. Posen repräsentieren.
9. Source Frame und Target Frame einer Transformation definieren.
10. starre Transformationen unterstützen.
11. affine Transformationen unterstützen.
12. Transformationen invertieren können, sofern mathematisch möglich.
13. nicht invertierbare Transformationen erkennen.
14. Transformationen komponieren.
15. Transformationsketten unterstützen.
16. Transformationsketten validieren.
17. einen Transformationsgraphen unterstützen.
18. Transformationen zwischen verbundenen Frames bestimmen können.
19. Transformationsgültigkeit verwalten.
20. dynamische Transformationen mit Zeitstempeln versehen.
21. Registrierung repräsentieren.
22. Landmark-Registrierung unterstützen.
23. Registrierungsqualität repräsentieren.
24. unzureichende Landmark-Konfigurationen erkennen.
25. Patient Registration unterstützen.
26. Registrierungszustände verwalten.
27. Tracking-Systeme abstrahieren.
28. Tracking Samples repräsentieren.
29. Tracking-Zeitstempel erhalten.
30. Tracking-Verlust erkennen.
31. stale Tracking-Daten erkennen.
32. Instrumente repräsentieren.
33. Instrumentengeometrie verwalten.
34. Tool Tips repräsentieren.
35. Instrumentenkalibrierung unterstützen.
36. Kalibrierungsgültigkeit verwalten.
37. Navigation Sessions repräsentieren.
38. Navigationsvoraussetzungen prüfen.
39. Navigationsgültigkeit explizit verwalten.
40. Ziele repräsentieren.
41. lineare Trajektorien repräsentieren.
42. Distanz zum Ziel berechnen.
43. laterale Abweichung berechnen.
44. Winkelabweichung berechnen.
45. physikalische Einheiten für Guidance-Metriken verwenden.
46. Zeitstempel als Bestandteil dynamischer Daten behandeln.
47. zeitliche Synchronisierung unterstützen.
48. Latenz messen können.
49. veraltete Daten erkennen.
50. Echtzeitverarbeitung architektonisch unterstützen.
51. den Compute Scheduler integrieren.
52. das Compute Memory Model integrieren.
53. das Compute Object Model integrieren.
54. CPU-Ausführung unterstützen.
55. GPU-Ausführung architektonisch unterstützen.
56. DICOM Compute integrieren.
57. Radiology Compute integrieren.
58. PACS Compute architektonisch integrieren können.
59. Ultrasound Compute architektonisch integrieren können.
60. Medical AI Compute architektonisch integrieren können.
61. Originalbilddaten erhalten.
62. Registrierungen versionierbar halten.
63. Transformationen versionierbar halten.
64. aktive Transformationen nicht stillschweigend ersetzen.
65. räumliche Unsicherheit repräsentieren können.
66. unbekannte Genauigkeit explizit darstellen.
67. unbekannte Qualität nicht automatisch als gut behandeln.
68. Provenance bereitstellen.
69. Transformationsherkunft dokumentieren.
70. Registrierungsherkunft dokumentieren.
71. Kalibrierungsherkunft dokumentieren.
72. Tracking-Herkunft dokumentieren.
73. strukturierte Diagnosen bereitstellen.
74. Profiling unterstützen.
75. Audit unterstützen.
76. kritische Eingabeverluste erkennen.
77. Guidance bei kritischem Informationsverlust invalidieren.
78. letzte bekannte Posen nach Tracking-Verlust nicht als aktuelle Pose ausgeben.
79. ungültige Transformationen nicht für aktive Navigation verwenden.
80. Fail-Safe-Zustände bereitstellen.
81. Recovery erneut validieren.
82. medizinische Navigation von medizinischer Entscheidung trennen.
83. Navigation von physischer Gerätesteuerung trennen.
84. Erweiterungen ohne Änderung des grundlegenden räumlichen Objektmodells ermöglichen.

---

## 310. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaImageGuidedCompute
 |
 +-- ImageGuidedRuntime
 |
 +-- SpatialObjectModel
 |
 +-- CoordinateSystemManager
 |
 +-- ReferenceFrameManager
 |
 +-- TransformEngine
 |
 +-- TransformGraph
 |
 +-- RegistrationEngine
 |
 +-- LandmarkRegistration
 |
 +-- SurfaceRegistration
 |
 +-- ImageRegistration
 |
 +-- DeformableRegistration
 |
 +-- TrackingManager
 |
 +-- TrackingSynchronizer
 |
 +-- InstrumentManager
 |
 +-- CalibrationEngine
 |
 +-- NavigationEngine
 |
 +-- TargetManager
 |
 +-- TrajectoryEngine
 |
 +-- SafetyGeometryEngine
 |
 +-- CollisionEngine
 |
 +-- ImageFusionEngine
 |
 +-- UltrasoundNavigation
 |
 +-- TemporalEngine
 |
 +-- AccuracyEngine
 |
 +-- UncertaintyEngine
 |
 +-- ProvenanceEngine
 |
 +-- Diagnostics
 |
 +-- Profiler
 |
 +-- AuditBridge
```

Diese Struktur ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 311. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt die gemeinsame Ausführungsinfrastruktur bereit.

```text
Image-Guided Compute
        |
        v
Compute Runtime
        |
   +----+----+
   |         |
   v         v
  CPU       GPU
```

---

## 312. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert:

```text
Tracking
Temporal Alignment
Transform Resolution
Registration
Navigation
Safety Checks
Image Fusion
Background Processing
```

---

## 313. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet:

```text
Images
Volumes
Tracking Streams
Transform Graphs
Surfaces
Point Clouds
Trajectories
Navigation States
Deformation Fields
```

---

## 314. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Image-Guided-Objekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 315. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

GPU Runtime beschleunigt insbesondere:

```text
Registration
Resampling
Image Fusion
Surface Processing
Distance Fields
Deformable Transformation
```

---

## 316. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

NPU Runtime kann KI-basierte:

```text
Landmark Detection
Segmentation
Pose Estimation
Instrument Detection
```

beschleunigen.

---

## 317. Beziehung zu `NPSPEC-COMPUTE-DICOM-0001`

DICOM Compute stellt medizinische Bilddaten und deren räumliche Metadaten bereit.

---

## 318. Beziehung zu `NPSPEC-COMPUTE-RADIOLOGY-0001`

Radiology Compute stellt:

```text
Image Geometry
Volume Geometry
Registration
Segmentation
Resampling
Measurements
Surfaces
```

bereit.

Image-Guided Compute erweitert diese Funktionen um Tracking, Kalibrierung und Navigation.

---

## 319. Beziehung zu `NPSPEC-COMPUTE-PACS-0001`

PACS Compute stellt präoperative und intraoperative Studien bereit.

---

## 320. Beziehung zu `NPSPEC-COMPUTE-ULTRASOUND-0001`

Ultrasound Compute stellt Ultraschallbilder, Cine-Daten, Volumen und Sensordaten für bildgestützte Ultraschallnavigation bereit.

---

## 321. Beziehung zu `NPSPEC-COMPUTE-MEDAI-0001`

Medical AI Compute kann:

```text
Segmentations
Landmarks
Anatomical Structures
Instrument Detections
Pose Estimates
```

für Image-Guided Compute bereitstellen.

---

## 322. Beziehung zu `NPSPEC-COMPUTE-RADIOMICS-0001`

Radiomics Compute kann quantitative Merkmale aus navigierten beziehungsweise behandelten Regionen extrahieren.

---

## 323. Grundlegende Koordinatenarchitektur

```text
Tool Frame
    |
    v
Tracker Frame
    |
    v
Patient Frame
    |
    v
Image Frame
```

Jede Verbindung ist eine explizite Transformation.

---

## 324. Registrierungsarchitektur

```text
Preoperative Image
        |
        |
        +----------+
                   |
Patient --------> Registration
                   |
                   v
             Transform
                   |
                   v
       Navigation Coordinate Space
```

---

## 325. Trackingarchitektur

```text
Tracking Device
      |
      v
Tracking Samples
      |
      v
Temporal Validation
      |
      v
Pose Stream
      |
      v
Transform Graph
```

---

## 326. Navigationsarchitektur

```text
Medical Image
     |
     v
Patient Registration
     |
     +----------------+
                      |
Tracker ------------> Transform Graph
                      |
Instrument ----------+
                      |
Target --------------+
                      |
                      v
               Navigation Engine
                      |
             +--------+--------+
             |        |        |
             v        v        v
          Distance   Angle   Safety
             |        |        |
             +--------+--------+
                      |
                      v
                Guidance State
```

---

## 327. Tracked-Ultrasound-Architektur

```text
Ultrasound Probe
      |
      +--> Ultrasound Image
      |
      +--> Tracking Sensor
               |
               v
          Probe Tracking
               |
               v
         Probe Calibration
               |
               v
      Image-to-Patient Transform
```

---

## 328. Echtzeitarchitektur

```text
Sensor / Tracker
       |
       v
Timestamped Sample
       |
       v
Temporal Alignment
       |
       v
Transform Resolution
       |
       v
Navigation Compute
       |
       v
Safety Evaluation
       |
       v
Guidance State
       |
       v
Visualization
```

---

## 329. Fail-Safe-Architektur

```text
Navigation
    |
    +--> Registration Valid?
    |
    +--> Tracking Valid?
    |
    +--> Calibration Valid?
    |
    +--> Transform Chain Valid?
    |
    +--> Timing Valid?
    |
    v

All Valid
    |
    v
Guidance Valid


Any Critical Failure
    |
    v
Guidance Invalid
```

---

## 330. Datenmodellgrundsatz

Image-Guided Compute MUSS zwischen:

```text
Image Geometry
Physical Geometry
Coordinate System
Reference Frame
Pose
Transform
Registration
Tracking Sample
Calibration
Target
Trajectory
Guidance State
```

unterscheiden.

---

## 331. Transformationsgrundsatz

Eine Transformation ist nicht lediglich eine Matrix.

Sie besteht logisch aus:

```text
Transform Identity
Source Frame
Target Frame
Transform Type
Parameters
Timestamp
Validity
Accuracy
Version
Provenance
```

---

## 332. Trackinggrundsatz

Eine Tracking-Pose besteht logisch aus:

```text
Object
Pose
Reference Frame
Timestamp
Quality
Validity
```

Eine Pose ohne gültige Zeit- und Frame-Zuordnung DARF nicht als vollständige Tracking-Information behandelt werden.

---

## 333. Registrierungsgrundsatz

Eine Registrierung besteht logisch aus:

```text
Source
Target
Method
Transform
Quality
Validity
Version
Provenance
```

Eine erfolgreich konvergierte mathematische Optimierung ist nicht automatisch eine medizinisch akzeptable Registrierung.

---

## 334. Navigationsgrundsatz

Navigation ist eine abgeleitete räumliche Berechnung aus:

```text
Images
Registration
Tracking
Calibration
Targets
Trajectories
Time
```

Ändert sich eine dieser Grundlagen wesentlich, MUSS die Gültigkeit der Navigation neu bewertet werden.

---

## 335. Zeitgrundsatz

Für dynamische Image-Guided-Systeme ist eine räumliche Information ohne zeitliche Zuordnung unvollständig.

Daher gilt:

```text
Spatial State = Geometry + Time + Validity
```

---

## 336. Genauigkeitsgrundsatz

Die nominelle numerische Auflösung einer Koordinate DARF nicht mit der tatsächlichen räumlichen Genauigkeit gleichgesetzt werden.

Beispielsweise bedeutet:

```text
Position = 12.345 mm
```

nicht automatisch eine Genauigkeit von:

```text
0.001 mm
```

---

## 337. Originaldatengrundsatz

Image-Guided Compute DARF medizinische Originalbilder nicht verändern.

Registrierung, Resampling, Fusion, Segmentierung und Navigation erzeugen abgeleitete Daten beziehungsweise räumliche Beziehungen.

---

## 338. Safety-by-Validity-Grundsatz

Ein Image-Guided-System MUSS Sicherheit nicht durch die Annahme korrekter Daten herstellen, sondern durch kontinuierliche Gültigkeitsprüfung.

Insbesondere:

```text
Unknown != Valid
Stale != Current
LastKnown != Current
Converged != Accurate
Tracked != Calibrated
Registered != Permanently Registered
```

---

## 339. Erweiterbarkeit

Neue:

```text
Tracking Systems
Registration Algorithms
Calibration Methods
Coordinate Conventions
Navigation Metrics
Safety Models
Image Modalities
AR Systems
Medical Devices
AI Methods
Compute Backends
```

SOLLTEN integrierbar sein, ohne das grundlegende räumliche Objekt- und Transformationsmodell zu verändern.

---

## 340. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-IMAGEGUIDED-COORD-0001
Image-Guided Coordinate Systems

NPSPEC-COMPUTE-IMAGEGUIDED-TRANSFORM-0001
Image-Guided Transform Graph

NPSPEC-COMPUTE-IMAGEGUIDED-REGISTRATION-0001
Image-Guided Registration

NPSPEC-COMPUTE-IMAGEGUIDED-DEFORM-0001
Deformable Registration

NPSPEC-COMPUTE-IMAGEGUIDED-PATIENTREG-0001
Patient Registration

NPSPEC-COMPUTE-IMAGEGUIDED-TRACKING-0001
Medical Tracking Runtime

NPSPEC-COMPUTE-IMAGEGUIDED-OPTICAL-0001
Optical Tracking

NPSPEC-COMPUTE-IMAGEGUIDED-EM-0001
Electromagnetic Tracking

NPSPEC-COMPUTE-IMAGEGUIDED-INSTRUMENT-0001
Tracked Medical Instruments

NPSPEC-COMPUTE-IMAGEGUIDED-CALIBRATION-0001
Instrument and Sensor Calibration

NPSPEC-COMPUTE-IMAGEGUIDED-NAVIGATION-0001
Medical Navigation Engine

NPSPEC-COMPUTE-IMAGEGUIDED-TARGET-0001
Navigation Targets

NPSPEC-COMPUTE-IMAGEGUIDED-TRAJECTORY-0001
Navigation Trajectories

NPSPEC-COMPUTE-IMAGEGUIDED-SAFETY-0001
Navigation Safety Geometry

NPSPEC-COMPUTE-IMAGEGUIDED-COLLISION-0001
Collision Awareness

NPSPEC-COMPUTE-IMAGEGUIDED-TIME-0001
Temporal Synchronization

NPSPEC-COMPUTE-IMAGEGUIDED-ACCURACY-0001
Navigation Accuracy and Uncertainty

NPSPEC-COMPUTE-IMAGEGUIDED-FUSION-0001
Image Fusion

NPSPEC-COMPUTE-IMAGEGUIDED-ULTRASOUND-0001
Tracked Ultrasound

NPSPEC-COMPUTE-IMAGEGUIDED-AR-0001
Augmented-Reality Guidance

NPSPEC-COMPUTE-IMAGEGUIDED-PROVENANCE-0001
Image-Guided Provenance

NPSPEC-COMPUTE-IMAGEGUIDED-TEST-0001
Image-Guided Conformance Tests
```

---

## 341. Zusammenfassung

`NPSPEC-COMPUTE-IMAGEGUIDED-0001` definiert die gemeinsame Infrastruktur für bildgestützte medizinische Compute-Workflows innerhalb der NovaOS-Compute-Architektur.

Die grundlegende Verarbeitung lautet:

```text
Medical Imaging
      +
Patient Geometry
      +
Tracking
      |
      v
Coordinate Systems
      |
      v
Registration
      |
      v
Transform Graph
      |
      v
Navigation
      |
      v
Guidance State
```

Die Architektur unterstützt:

```text
Coordinate Systems
Reference Frames
Transformations
Transform Chains
Registration
Patient Registration
Tracking
Instrument Calibration
Navigation
Targets
Trajectories
Safety Margins
Collision Awareness
Image Fusion
Tracked Ultrasound
```

Die räumliche Verarbeitung kann über:

```text
CPU
SIMD
Multi-Core
GPU
NPU-assisted AI
```

beschleunigt werden.

Medical AI kann zusätzliche:

```text
Segmentations
Landmarks
Instrument Detections
Pose Estimates
Anatomical Structures
```

bereitstellen.

Die Architektur trennt konsequent:

```text
Medical Image
Image Geometry
Patient Geometry
Coordinate System
Registration
Tracking
Calibration
Navigation
Guidance
Clinical Decision
Physical Device Control
```

voneinander.

Jede relevante räumliche Information bleibt auf:

```text
Source
Coordinate System
Reference Frame
Transform Chain
Registration
Tracking
Calibration
Timestamp
Quality
Accuracy
Version
Provenance
```

zurückführbar.

Die zentrale Gültigkeitsregel lautet:

> Eine räumliche Information darf in einem bildgestützten medizinischen System nur so lange als gültig verwendet werden, wie ihre erforderlichen Registrierungs-, Tracking-, Kalibrierungs-, Transformations- und Zeitbeziehungen gültig sind.

Die zentrale Sicherheitsregel lautet:

> Unknown ist nicht Valid, Stale ist nicht Current, Last Known ist nicht Current, Converged ist nicht automatisch Accurate und Tracked ist nicht automatisch Calibrated.

Das grundlegende Architekturprinzip lautet:

> NovaOS Image-Guided Compute behandelt bildgestützte Navigation nicht als einfache Überlagerung eines Instrumentensymbols auf einem medizinischen Bild. Koordinatensysteme, Referenzrahmen, Transformationen, Registrierungen, Tracking-Messungen, Instrumentenkalibrierungen, Zeitbeziehungen, Genauigkeiten, Unsicherheiten und Gültigkeitszustände werden als explizite Compute-Objekte behandelt. Dadurch können präoperative Bildgebung, intraoperative Bildgebung, Ultraschall, Tracking, Medical AI und Navigation zu nachvollziehbaren räumlichen Workflows verbunden werden, ohne die Grenze zwischen berechneter Guidance, medizinischer Entscheidung und physischer Gerätesteuerung aufzulösen.