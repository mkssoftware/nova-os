# NPSPEC-COMPUTE-MEDVIS-0001 — Medical Visualization Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Medical Imaging / Visualization  
**Komponente:** NovaOS Medical Visualization Compute  
**Bezeichner:** `NPSPEC-COMPUTE-MEDVIS-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0001`, `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-DICOM-0001`, `NPSPEC-COMPUTE-RADIOLOGY-0001`, `NPSPEC-COMPUTE-PACS-0001`, `NPSPEC-COMPUTE-ULTRASOUND-0001`, `NPSPEC-COMPUTE-MEDAI-0001`, `NPSPEC-COMPUTE-RADIOMICS-0001`, `NPSPEC-COMPUTE-IMAGEGUIDED-0001`, `NPSPEC-COMPUTE-PATHOLOGY-0001`  
**Kurzname:** Medical Visualization Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Medical-Visualization-Compute-Infrastruktur der NovaOS-Compute-Architektur.

Medical Visualization Compute stellt standardisierte Compute-Funktionen zur Aufbereitung medizinischer Daten für zwei- und dreidimensionale Visualisierung bereit.

Die Komponente verarbeitet insbesondere:

- medizinische Rasterbilder,
- Volumendaten,
- Bildserien,
- Segmentierungen,
- Masken,
- Oberflächen,
- Meshes,
- Punktwolken,
- Vektorfelder,
- Tensorfelder,
- Tracking-Daten,
- Annotationen,
- Messungen,
- quantitative Karten,
- AI-Ergebnisse,
- Pathologie-Bilddaten,
- Ultraschalldaten,
- multimodale Bilddaten.

Die grundlegende Architektur lautet:

```text
Medical Data
     |
     v
Geometry Validation
     |
     v
Visualization Preparation
     |
     +-------------------+
     |                   |
     v                   v
2D Pipeline          3D Pipeline
     |                   |
     v                   v
Image Views          Volume / Surface
     |                   |
     +---------+---------+
               |
               v
         Overlay Engine
               |
               v
        Visualization State
               |
               v
          Render Backend
```

Medical Visualization Compute definiert die Compute-Schicht für medizinische Visualisierung.

Die konkrete grafische Benutzeroberfläche beziehungsweise die allgemeine NovaOS-Grafik- und Windowing-Architektur ist nicht Bestandteil dieser Spezifikation.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Eine medizinische Visualisierung ist eine räumlich und semantisch definierte Darstellung medizinischer Daten und darf deren ursprüngliche Geometrie, Werte, Einheiten, Orientierung oder Provenance nicht stillschweigend verändern.

---

## 3. Visualisierungsgrundsatz

Medical Visualization Compute MUSS zwischen:

```text
Source Data
Visualization Representation
Rendering Parameters
Rendered Output
```

unterscheiden.

---

## 4. Originaldatengrundsatz

Visualisierungsoperationen DÜRFEN medizinische Originaldaten nicht verändern.

---

## 5. Rendering Boundary

Medical Visualization Compute MUSS zwischen:

```text
Medical Visualization Compute
Rendering Backend
Display System
User Interface
```

unterscheiden.

---

## 6. Ziele

Medical Visualization Compute MUSS architektonisch mindestens folgende Bereiche unterstützen:

1. 2D Medical Image Visualization,
2. 3D Medical Visualization,
3. Image Stack Visualization,
4. Multi-Planar Reconstruction,
5. Oblique Reconstruction,
6. Curved Planar Reconstruction,
7. Maximum Intensity Projection,
8. Minimum Intensity Projection,
9. Average Intensity Projection,
10. Volume Rendering,
11. Surface Rendering,
12. Segmentation Visualization,
13. Mask Visualization,
14. Label Maps,
15. Overlay Composition,
16. Multi-Modal Fusion,
17. Window/Level,
18. Transfer Functions,
19. Lookup Tables,
20. Color Maps,
21. Opacity Mapping,
22. Clipping,
23. Cropping,
24. Slice Extraction,
25. Resampling,
26. Interpolation,
27. Zoom,
28. Pan,
29. Rotation,
30. Camera Models,
31. Projection Models,
32. Annotation Visualization,
33. Measurement Visualization,
34. Crosshair Synchronization,
35. Linked Views,
36. Reference Lines,
37. Orientation Markers,
38. Scale Indicators,
39. 3D Geometry,
40. Mesh Visualization,
41. Point Clouds,
42. Vector Fields,
43. Tensor Fields,
44. Ultrasound Visualization,
45. Pathology Visualization,
46. Image-Guided Visualization,
47. Medical-AI Overlays,
48. Radiomics Maps,
49. Temporal Visualization,
50. Cine,
51. Multi-Frame Data,
52. GPU Acceleration,
53. CPU Fallback,
54. Streaming,
55. Large Data,
56. Quality Control,
57. Provenance,
58. Diagnostics,
59. Profiling,
60. Erweiterbarkeit.

---

## 7. Nicht-Ziele

Diese Spezifikation definiert nicht:

- konkrete diagnostische Befundregeln,
- konkrete klinische Interpretation,
- konkrete medizinische Schwellenwerte,
- konkrete UI-Layouts,
- konkrete Fensterdekorationen,
- konkrete Monitorhardware,
- konkrete GPU-Treiber,
- konkrete PACS-Workstations,
- konkrete chirurgische Entscheidungen,
- regulatorische Zulassung.

---

## 8. Medical Visualization Object

Ein visualisierbares medizinisches Objekt wird als:

`MedicalVisualizationObject`

repräsentiert.

---

## 9. Visualization Object Identity

Jedes Visualisierungsobjekt MUSS eindeutig identifizierbar sein.

---

## 10. Visualization Object Types

Mindestens folgende Typen SOLLEN unterstützt werden:

```text
Image
ImageSeries
Volume
Segmentation
Mask
LabelMap
Surface
Mesh
PointCloud
VectorField
TensorField
Annotation
Measurement
Trajectory
TrackingObject
QuantitativeMap
AIResult
```

---

## 11. Source Reference

Jedes Visualisierungsobjekt MUSS seine Quelldaten referenzieren können.

---

## 12. Derived Visualization Object

Abgeleitete Visualisierungsrepräsentationen MÜSSEN als solche gekennzeichnet werden.

---

## 13. Geometry

Jedes räumliche Visualisierungsobjekt MUSS eine definierte Geometrie besitzen.

---

## 14. Image Geometry

Bildgeometrie SOLL mindestens enthalten können:

```text
Dimensions
Spacing
Origin
Orientation
CoordinateSystem
```

---

## 15. Physical Coordinates

Medical Visualization Compute MUSS physikalische Koordinaten unterstützen.

---

## 16. Pixel Coordinates

Pixelkoordinaten MÜSSEN von physikalischen Koordinaten unterscheidbar sein.

---

## 17. Voxel Coordinates

Voxelkoordinaten MÜSSEN von physikalischen Koordinaten unterscheidbar sein.

---

## 18. Coordinate Transform

Transformationen zwischen:

```text
Pixel
Voxel
Image
Patient
World
View
Display
```

SOLLTEN explizit repräsentiert werden.

---

## 19. Orientation

Bildorientierungen MÜSSEN erhalten bleiben.

---

## 20. Orientation Convention

Die verwendete Orientierungskonvention MUSS eindeutig bekannt sein.

---

## 21. Unknown Orientation

Unbekannte Orientierung MUSS explizit darstellbar sein.

---

## 22. No Orientation Guessing

Eine unbekannte Bildorientierung DARF nicht stillschweigend als bekannte Standardorientierung angenommen werden.

---

## 23. View

Eine medizinische Ansicht wird als:

`MedicalView`

repräsentiert.

---

## 24. View Types

Mindestens folgende Ansichten SOLLEN unterstützt werden:

```text
Image2D
Slice2D
MPR
ObliqueMPR
CPR
Projection
Volume3D
Surface3D
Fusion
Cine
Pathology2D
Ultrasound2D
Ultrasound3D
Navigation
```

---

## 25. View Identity

Jede Ansicht MUSS eindeutig identifizierbar sein.

---

## 26. View State

Ein View State wird als:

`MedicalViewState`

repräsentiert.

---

## 27. View State Fields

Mindestens folgende Eigenschaften SOLLEN unterstützt werden:

```text
ViewId
Source
Geometry
Camera
Slice
WindowLevel
TransferFunction
Overlays
Zoom
Pan
Rotation
Timestamp
```

---

## 28. View Version

View States SOLLEN versionierbar sein.

---

## 29. 2D Visualization

Medical Visualization Compute MUSS zweidimensionale medizinische Bilddarstellung unterstützen.

---

## 30. Image Plane

Eine Bildebene wird als:

`MedicalImagePlane`

repräsentiert.

---

## 31. Native Plane

Originale Bildebenen SOLLEN ohne unnötiges Resampling dargestellt werden können.

---

## 32. Slice Extraction

Slices aus Volumendaten SOLLEN extrahiert werden können.

---

## 33. Slice Position

Eine Slice-Position MUSS räumlich eindeutig definiert sein.

---

## 34. Slice Thickness

Die effektive Slice Thickness SOLL verfügbar sein können.

---

## 35. Slice Index

Ein Slice Index DARF nicht als Ersatz für eine physikalische Position verwendet werden.

---

## 36. Multi-Planar Reconstruction

Multi-Planar Reconstruction wird als:

`MPR`

repräsentiert.

---

## 37. Standard MPR

Mindestens folgende anatomischen Ebenen SOLLEN unterstützt werden:

```text
Axial
Coronal
Sagittal
```

---

## 38. Oblique MPR

Beliebig orientierte Rekonstruktionsebenen SOLLEN unterstützt werden.

---

## 39. MPR Plane

Eine MPR-Ebene MUSS mindestens definieren:

```text
Origin
Normal
UpVector
PixelSpacing
Dimensions
```

---

## 40. MPR Synchronization

Mehrere MPR-Ansichten SOLLEN räumlich synchronisiert werden können.

---

## 41. Crosshair

Eine gemeinsame räumliche Position wird als:

`MedicalCrosshair`

repräsentiert.

---

## 42. Crosshair Coordinates

Crosshair-Positionen MÜSSEN in einem definierten physikalischen Koordinatensystem gespeichert werden.

---

## 43. Crosshair Synchronization

Änderungen einer Ansicht SOLLEN verbundene Ansichten aktualisieren können.

---

## 44. Reference Lines

Schnittlinien anderer Ebenen SOLLEN visualisiert werden können.

---

## 45. Curved Planar Reconstruction

Curved Planar Reconstruction wird als:

`CPR`

repräsentiert.

---

## 46. CPR Path

Eine CPR MUSS einen räumlichen Pfad referenzieren.

---

## 47. CPR Sampling

Sampling-Abstand und Interpolationsmethode MÜSSEN dokumentierbar sein.

---

## 48. Projection Visualization

Medical Visualization Compute SOLL projektionsbasierte Darstellungen unterstützen.

---

## 49. Maximum Intensity Projection

Maximum Intensity Projection wird als:

`MIP`

repräsentiert.

---

## 50. Minimum Intensity Projection

Minimum Intensity Projection wird als:

`MinIP`

repräsentiert.

---

## 51. Average Intensity Projection

Average Intensity Projection wird als:

`AIP`

repräsentiert.

---

## 52. Projection Slab

Projektionsverfahren SOLLEN auf definierte Slab-Dicken begrenzt werden können.

---

## 53. Projection Direction

Die Projektionsrichtung MUSS eindeutig definiert sein.

---

## 54. Projection Parameters

Projektionsparameter MÜSSEN Bestandteil des View State sein.

---

## 55. Window/Level

Medical Visualization Compute MUSS Window/Level-Verarbeitung unterstützen.

---

## 56. Window

`WindowWidth` definiert die dargestellte Wertebreite.

---

## 57. Level

`WindowCenter` definiert das Zentrum des dargestellten Wertebereichs.

---

## 58. Window/Level State

Window/Level-Einstellungen MÜSSEN als Visualisierungsparameter behandelt werden.

---

## 59. Source Values

Window/Level DARF die zugrunde liegenden Originalwerte nicht verändern.

---

## 60. Presets

Window/Level-Presets SOLLEN unterstützt werden.

---

## 61. Preset Identity

Presets SOLLEN eindeutig identifizierbar sein.

---

## 62. Modality LUT

Modalitätsspezifische Lookup-Transformationen SOLLEN unterstützt werden.

---

## 63. VOI LUT

VOI-LUT-Verarbeitung SOLL unterstützt werden.

---

## 64. Presentation LUT

Presentation-LUT-Verarbeitung SOLL architektonisch unterstützt werden.

---

## 65. Lookup Table

Eine Lookup Table wird als:

`MedicalLookupTable`

repräsentiert.

---

## 66. Color Map

Eine Farbabbildung wird als:

`MedicalColorMap`

repräsentiert.

---

## 67. Color Map Domain

Der Eingabewertebereich einer Color Map MUSS eindeutig definiert sein.

---

## 68. Color Map Range

Der Ausgabefarbraum MUSS definiert sein.

---

## 69. Transfer Function

Eine Transfer Function wird als:

`MedicalTransferFunction`

repräsentiert.

---

## 70. Transfer Function Components

Eine Transfer Function SOLL enthalten können:

```text
ScalarMapping
ColorMapping
OpacityMapping
GradientMapping
```

---

## 71. Transfer Function Versioning

Transfer Functions SOLLEN versionierbar sein.

---

## 72. Transfer Function Presets

Wiederverwendbare Presets SOLLEN unterstützt werden.

---

## 73. Volume Rendering

Medical Visualization Compute SOLL direktes Volume Rendering unterstützen.

---

## 74. Volume Rendering Input

Ein Volume-Rendering-Datensatz MUSS mindestens enthalten:

```text
VolumeData
Geometry
ScalarType
ValueRange
```

---

## 75. Ray Casting

GPU- beziehungsweise CPU-basiertes Ray Casting SOLL unterstützt werden können.

---

## 76. Sampling Rate

Die Sampling Rate MUSS konfigurierbar sein.

---

## 77. Interpolation

Mindestens folgende Interpolationsverfahren SOLLEN unterstützt werden:

```text
Nearest
Linear
Cubic
```

---

## 78. Label Interpolation

Diskrete Label Maps SOLLTEN standardmäßig keine kontinuierliche Interpolation verwenden.

---

## 79. Gradient

Gradienten für Beleuchtung beziehungsweise Transfer Functions SOLLEN berechnet werden können.

---

## 80. Shading

Optionale Shading-Verfahren SOLLEN unterstützt werden.

---

## 81. Early Ray Termination

Volume Rendering DARF Early Ray Termination verwenden.

---

## 82. Empty Space Skipping

Leere beziehungsweise transparente Volumenbereiche SOLLEN übersprungen werden können.

---

## 83. Adaptive Sampling

Adaptive Sampling SOLL unterstützt werden können.

---

## 84. Large Volume Rendering

Volumen größer als der verfügbare GPU-Speicher MÜSSEN architektonisch visualisierbar sein.

---

## 85. Bricked Volume

Große Volumen SOLLEN in räumliche Bricks unterteilt werden können.

---

## 86. Brick

Ein Volume Brick wird als:

`MedicalVolumeBrick`

repräsentiert.

---

## 87. Brick Cache

Bricks SOLLEN zwischengespeichert werden können.

---

## 88. Brick Streaming

Bricks SOLLEN bei Bedarf gestreamt werden können.

---

## 89. Level of Detail

Volume Rendering SOLL unterschiedliche Detailstufen unterstützen.

---

## 90. Progressive Rendering

Große Datensätze SOLLEN progressiv dargestellt werden können.

---

## 91. Surface Rendering

Medical Visualization Compute SOLL Oberflächenvisualisierung unterstützen.

---

## 92. Surface

Eine Oberfläche wird als:

`MedicalSurface`

repräsentiert.

---

## 93. Mesh

Ein Mesh wird als:

`MedicalMesh`

repräsentiert.

---

## 94. Mesh Types

Mindestens folgende Mesh-Typen SOLLEN unterstützt werden:

```text
TriangleMesh
LineMesh
PointMesh
MixedMesh
```

---

## 95. Mesh Coordinates

Mesh-Koordinaten MÜSSEN einem definierten räumlichen Koordinatensystem zugeordnet sein.

---

## 96. Mesh Attributes

Meshes SOLLEN enthalten können:

```text
Normals
Colors
Labels
Scalars
Vectors
TextureCoordinates
```

---

## 97. Surface Extraction

Oberflächen SOLLEN aus Segmentierungen beziehungsweise Volumen extrahiert werden können.

---

## 98. Iso-Surface

Iso-Surface-Extraktion SOLL unterstützt werden.

---

## 99. Surface Smoothing

Oberflächenglättung SOLL unterstützt werden können.

---

## 100. Geometry Preservation

Glättungsoperationen MÜSSEN als geometrieverändernde abgeleitete Verarbeitung dokumentiert werden.

---

## 101. Mesh Decimation

Mesh Decimation SOLL unterstützt werden.

---

## 102. Decimation Error

Decimation SOLL eine geometrische Fehlermetrik bereitstellen können.

---

## 103. Surface Normals

Oberflächennormalen SOLLEN berechnet werden können.

---

## 104. Surface Coloring

Oberflächen SOLLEN anhand von:

```text
Label
Scalar
Measurement
AIResult
Distance
Quality
```

eingefärbt werden können.

---

## 105. Segmentation Visualization

Segmentierungen MÜSSEN visualisiert werden können.

---

## 106. Segmentation Object

Eine Visualisierungsrepräsentation einer Segmentierung wird als:

`MedicalSegmentationVisualization`

repräsentiert.

---

## 107. Segmentation Modes

Mindestens folgende Darstellungsarten SOLLEN unterstützt werden:

```text
Filled
Outline
Contour
Surface
Overlay
```

---

## 108. Label Map

Eine Label Map wird als:

`MedicalLabelMap`

repräsentiert.

---

## 109. Label Identity

Jedes Label MUSS eindeutig identifizierbar sein.

---

## 110. Label Color

Labels SOLLEN definierte Visualisierungsfarben besitzen können.

---

## 111. Label Opacity

Labels SOLLEN eine definierte Opazität besitzen können.

---

## 112. Label Visibility

Labels MÜSSEN einzeln ein- und ausgeblendet werden können.

---

## 113. Segmentation Source

Die Herkunft einer Segmentierung MUSS erhalten bleiben.

---

## 114. Overlay

Ein zusätzliches Visualisierungsobjekt wird als:

`MedicalOverlay`

repräsentiert.

---

## 115. Overlay Types

Mindestens folgende Overlays SOLLEN unterstützt werden:

```text
Segmentation
Annotation
Measurement
AIResult
QuantitativeMap
Tracking
Trajectory
Crosshair
ReferenceLine
Scale
OrientationMarker
```

---

## 116. Overlay Geometry

Ein räumliches Overlay MUSS geometrisch korrekt zum Basisbild ausgerichtet werden.

---

## 117. Overlay Transform

Notwendige Transformationen MÜSSEN explizit bekannt sein.

---

## 118. Overlay Composition

Mehrere Overlays SOLLEN gleichzeitig komponiert werden können.

---

## 119. Overlay Order

Die Zeichenreihenfolge SOLL explizit steuerbar sein.

---

## 120. Overlay Opacity

Opazität SOLL pro Overlay einstellbar sein.

---

## 121. Overlay Visibility

Sichtbarkeit SOLL pro Overlay steuerbar sein.

---

## 122. Multi-Modal Fusion

Medical Visualization Compute SOLL multimodale Bildfusion unterstützen.

---

## 123. Fusion Inputs

Typische Kombinationen umfassen:

```text
CT + MR
CT + PET
MR + PET
CT + SPECT
Ultrasound + CT
Ultrasound + MR
Preoperative + Intraoperative
```

---

## 124. Fusion Geometry

Fusion setzt räumlich kompatible beziehungsweise registrierte Daten voraus.

---

## 125. Registration Reference

Die für Fusion verwendete Registrierung MUSS referenzierbar sein.

---

## 126. Fusion Modes

Mindestens folgende Darstellungsarten SOLLEN unterstützt werden:

```text
AlphaBlend
Checkerboard
SideBySide
ColorOverlay
Difference
LinkedViews
```

---

## 127. Fusion Opacity

Alpha-Blending MUSS eine definierte Opazität besitzen.

---

## 128. Fusion Provenance

Die Herkunft und Registrierung fusionierter Daten MUSS nachvollziehbar sein.

---

## 129. Camera

Eine virtuelle Kamera wird als:

`MedicalCamera`

repräsentiert.

---

## 130. Camera State

Eine Kamera SOLL mindestens enthalten:

```text
Position
Target
UpVector
Projection
NearPlane
FarPlane
```

---

## 131. Projection Types

Mindestens folgende Projektionen SOLLEN unterstützt werden:

```text
Orthographic
Perspective
```

---

## 132. Medical Default Projection

Für geometrisch quantitative Ansichten SOLL orthografische Projektion verfügbar sein.

---

## 133. Perspective Distortion

Perspektivische Darstellung DARF nicht als geometrisch maßstabstreue 2D-Messdarstellung behandelt werden.

---

## 134. Camera Synchronization

Mehrere 3D-Ansichten SOLLEN Kamerazustände koppeln können.

---

## 135. Zoom

Zoom MUSS als Darstellungsoperation behandelt werden.

---

## 136. Pan

Pan MUSS als Darstellungsoperation behandelt werden.

---

## 137. Rotation

Rotation der Ansicht MUSS von einer Änderung der medizinischen Quelldaten unterschieden werden.

---

## 138. Fit To View

Automatische Anpassung an die View-Größe SOLL unterstützt werden.

---

## 139. Scale

Medical Visualization Compute SOLL physikalische Maßstäbe darstellen können.

---

## 140. Scale Indicator

Ein Maßstabsindikator wird als:

`MedicalScaleIndicator`

repräsentiert.

---

## 141. Scale Accuracy

Der dargestellte Maßstab MUSS aus der bekannten Bild- und View-Geometrie berechnet werden.

---

## 142. Unknown Scale

Ist keine physikalische Kalibrierung bekannt, DARF kein scheinbar physikalisch genauer Maßstab dargestellt werden.

---

## 143. Orientation Marker

Anatomische Orientierungsmarker SOLLEN unterstützt werden.

---

## 144. Orientation Source

Orientierungsmarker MÜSSEN aus der tatsächlichen Bildgeometrie bestimmt werden.

---

## 145. Hardcoded Orientation

Anatomische Richtungsmarker DÜRFEN nicht unabhängig von der tatsächlichen Bildorientierung fest codiert werden.

---

## 146. Annotation Visualization

Annotationen SOLLEN visualisiert werden.

---

## 147. Annotation Coordinates

Annotationen MÜSSEN in ihrem ursprünglichen Koordinatensystem gespeichert beziehungsweise eindeutig transformierbar sein.

---

## 148. Text Annotation

Textannotation SOLL unterstützt werden.

---

## 149. Shape Annotation

Mindestens folgende Formen SOLLEN unterstützt werden:

```text
Point
Line
Polyline
Polygon
Rectangle
Ellipse
Freehand
```

---

## 150. Measurement Visualization

Messungen SOLLEN als eigene Visualisierungsobjekte dargestellt werden.

---

## 151. Measurement Types

Mindestens:

```text
Distance
Angle
Area
Volume
Diameter
PolylineLength
```

SOLLTEN unterstützt werden.

---

## 152. Measurement Units

Physikalische Messungen MÜSSEN ihre Einheit darstellen können.

---

## 153. Measurement Geometry

Messungen MÜSSEN auf der tatsächlichen medizinischen Geometrie basieren.

---

## 154. Screen Pixel Measurement

Bildschirmpixel DÜRFEN nicht ohne räumliche Kalibrierung als physikalische Maßeinheit verwendet werden.

---

## 155. Temporal Data

Medical Visualization Compute SOLL zeitabhängige medizinische Daten unterstützen.

---

## 156. Medical Time Series

Eine zeitliche Serie wird als:

`MedicalTimeSeries`

repräsentiert.

---

## 157. Frame

Ein zeitlicher Frame wird als:

`MedicalTemporalFrame`

repräsentiert.

---

## 158. Frame Timestamp

Frames SOLLEN Zeitstempel beziehungsweise relative Zeitinformationen besitzen.

---

## 159. Cine

Zeitliche Bildfolgen SOLLEN als Cine dargestellt werden können.

---

## 160. Cine Playback

Cine-Wiedergabe SOLL unterstützen:

```text
Play
Pause
Stop
StepForward
StepBackward
Loop
PlaybackRate
```

---

## 161. Frame Rate

Die gewünschte Wiedergaberate MUSS von der tatsächlichen Datenakquisitionsrate unterscheidbar sein.

---

## 162. Dropped Frame

Ausgelassene Renderframes DÜRFEN die zugrunde liegende medizinische Zeitachse nicht verändern.

---

## 163. Temporal Synchronization

Mehrere zeitabhängige Datensätze SOLLEN synchronisiert dargestellt werden können.

---

## 164. Ultrasound Visualization

Medical Visualization Compute SOLL `NPSPEC-COMPUTE-ULTRASOUND-0001` integrieren.

---

## 165. Ultrasound 2D

B-Mode- und andere zweidimensionale Ultraschalldaten SOLLEN visualisiert werden können.

---

## 166. Ultrasound Cine

Ultraschall-Cine SOLL unterstützt werden.

---

## 167. Doppler Visualization

Farb- und Spektraldoppler-Daten SOLLEN über definierte Visualisierungsrepräsentationen integrierbar sein.

---

## 168. Ultrasound Geometry

Nichtkartesische beziehungsweise sektorförmige Ultraschallgeometrien MÜSSEN berücksichtigt werden können.

---

## 169. Tracked Ultrasound

Getrackte Ultraschalldaten SOLLEN über `NPSPEC-COMPUTE-IMAGEGUIDED-0001` räumlich visualisiert werden können.

---

## 170. Pathology Visualization

Medical Visualization Compute SOLL `NPSPEC-COMPUTE-PATHOLOGY-0001` integrieren.

---

## 171. Whole Slide Visualization

Whole Slide Images SOLLEN interaktiv visualisiert werden können.

---

## 172. Pathology Pyramid

WSI-Auflösungspyramiden SOLLEN für Level-of-Detail-Darstellung verwendet werden.

---

## 173. Pathology Tiles

Nur aktuell benötigte Tiles SOLLEN geladen werden müssen.

---

## 174. Pathology Tile Cache

Medical Visualization Compute SOLL mit dem Pathology Tile Cache zusammenarbeiten.

---

## 175. Pathology Zoom

Zoomstufen SOLLEN automatisch geeignete WSI-Pyramid Levels auswählen können.

---

## 176. Pathology Physical Scale

Mikroskopische physikalische Maßstäbe SOLLEN dargestellt werden können.

---

## 177. Pathology Annotations

Pathologie-Annotationen SOLLEN auf WSI-Daten dargestellt werden können.

---

## 178. Pathology Cell Overlays

Erkannte Zellen, Kerne und andere Pathology-Objekte SOLLEN als Overlays dargestellt werden können.

---

## 179. Large Annotation Sets

Millionen pathologischer Objekte SOLLEN über Level-of-Detail- beziehungsweise Culling-Mechanismen visualisierbar sein.

---

## 180. Medical AI Visualization

Medical Visualization Compute SOLL `NPSPEC-COMPUTE-MEDAI-0001` integrieren.

---

## 181. AI Overlay

AI-Ergebnisse SOLLEN als:

```text
BoundingBox
Contour
Mask
Heatmap
ProbabilityMap
Landmark
Vector
Label
Text
```

dargestellt werden können.

---

## 182. AI Source

Ein AI-Overlay MUSS auf das erzeugende AI-Ergebnis zurückführbar sein.

---

## 183. AI Confidence Visualization

Konfidenzwerte SOLLEN optional visualisiert werden können.

---

## 184. AI Review State

Der Review-Status eines AI-Ergebnisses SOLL visuell unterscheidbar sein können.

---

## 185. AI Is Not Ground Truth

AI-Overlays DÜRFEN nicht automatisch als Ground-Truth-Annotationen behandelt werden.

---

## 186. Radiomics Visualization

Medical Visualization Compute SOLL `NPSPEC-COMPUTE-RADIOMICS-0001` integrieren.

---

## 187. Quantitative Map

Eine quantitative Karte wird als:

`MedicalQuantitativeMap`

repräsentiert.

---

## 188. Quantitative Map Units

Quantitative Karten MÜSSEN ihre Einheiten erhalten.

---

## 189. Quantitative Color Mapping

Die Abbildung quantitativer Werte auf Farben MUSS nachvollziehbar sein.

---

## 190. Color Scale

Eine quantitative Color Scale SOLL dargestellt werden können.

---

## 191. Fixed Range

Quantitative Visualisierungen SOLLEN feste Wertebereiche verwenden können.

---

## 192. Dynamic Range

Automatische Wertebereiche SOLLEN explizit als dynamisch gekennzeichnet werden können.

---

## 193. Misleading Auto Scaling

Automatische Farbskalierung DARF nicht stillschweigend quantitative Vergleichbarkeit suggerieren.

---

## 194. Image-Guided Visualization

Medical Visualization Compute SOLL `NPSPEC-COMPUTE-IMAGEGUIDED-0001` integrieren.

---

## 195. Tracking Overlay

Getrackte Instrumente SOLLEN visualisiert werden können.

---

## 196. Tool Geometry

Die tatsächliche beziehungsweise kalibrierte Instrumentengeometrie SOLL dargestellt werden können.

---

## 197. Tool Tip

Die Tool-Tip-Position SOLL visualisiert werden können.

---

## 198. Target Visualization

Navigationsziele SOLLEN dargestellt werden können.

---

## 199. Trajectory Visualization

Geplante Trajektorien SOLLEN dargestellt werden können.

---

## 200. Safety Region Visualization

Definierte Safety Regions SOLLEN dargestellt werden können.

---

## 201. Navigation Validity

Die Visualisierung MUSS den Gültigkeitsstatus der zugrunde liegenden Guidance berücksichtigen können.

---

## 202. Stale Tracking

Veraltete Tracking-Daten DÜRFEN nicht ohne Kennzeichnung als aktuelle Instrumentenposition dargestellt werden.

---

## 203. Navigation Epoch

Visualisierungszustände SOLLEN die verwendete Navigation Epoch referenzieren.

---

## 204. Tracking Timestamp

Ein dynamisches Navigationsoverlay SOLL den verwendeten Tracking-Zeitstempel referenzieren.

---

## 205. Point Cloud

Medical Visualization Compute SOLL Punktwolken unterstützen.

---

## 206. Point Cloud Attributes

Punkte SOLLEN besitzen können:

```text
Position
Color
Normal
Scalar
Label
Confidence
```

---

## 207. Large Point Clouds

Große Punktwolken SOLLEN über Level of Detail dargestellt werden können.

---

## 208. Vector Field

Ein Vektorfeld wird als:

`MedicalVectorField`

repräsentiert.

---

## 209. Vector Visualization

Vektorfelder SOLLEN dargestellt werden können über:

```text
Glyphs
Arrows
Streamlines
ColorEncoding
```

---

## 210. Vector Units

Vektorfelder MÜSSEN ihre physikalischen Einheiten erhalten.

---

## 211. Tensor Field

Tensorfelder SOLLEN architektonisch unterstützt werden.

---

## 212. Tensor Visualization

Tensorfelder SOLLEN beispielsweise dargestellt werden können als:

```text
Glyphs
Ellipsoids
PrincipalDirections
DerivedScalarMaps
```

---

## 213. Clipping

Medical Visualization Compute SOLL Clipping unterstützen.

---

## 214. Clipping Plane

Eine Clipping Plane wird als:

`MedicalClippingPlane`

repräsentiert.

---

## 215. Multiple Clipping Planes

Mehrere Clipping Planes SOLLEN gleichzeitig unterstützt werden.

---

## 216. Cropping

Volumen und Geometrien SOLLEN auf definierte Regionen begrenzt werden können.

---

## 217. Crop Region

Eine Crop Region wird als:

`MedicalCropRegion`

repräsentiert.

---

## 218. Non-Destructive Cropping

Cropping MUSS standardmäßig als nicht destruktive Visualisierungsoperation behandelt werden.

---

## 219. Resampling

Medical Visualization Compute MUSS Resampling unterstützen.

---

## 220. Resampling Geometry

Ausgabegeometrie MUSS explizit definiert werden.

---

## 221. Interpolation Selection

Die Interpolationsmethode MUSS zum Datentyp passen.

---

## 222. Scalar Interpolation

Kontinuierliche Skalarbilder SOLLEN mindestens:

```text
Nearest
Linear
Cubic
```

unterstützen.

---

## 223. Label Interpolation

Label Maps SOLLTEN standardmäßig:

```text
Nearest
```

verwenden.

---

## 224. Vector Resampling

Vektorfelder MÜSSEN unter Berücksichtigung ihrer Vektorgeometrie resampelt werden können.

---

## 225. Resampling Provenance

Abgeleitete resampelte Daten SOLLEN ihre ursprüngliche Geometrie und Resampling-Konfiguration referenzieren.

---

## 226. Linked Views

Mehrere medizinische Ansichten SOLLEN miteinander verknüpft werden können.

---

## 227. Link Types

Mindestens folgende Kopplungen SOLLEN unterstützt werden:

```text
Position
Crosshair
Zoom
Pan
WindowLevel
Camera
Time
Slice
```

---

## 228. Link Group

Eine Gruppe gekoppelter Ansichten wird als:

`MedicalViewLinkGroup`

repräsentiert.

---

## 229. Partial Linking

Einzelne Eigenschaften SOLLEN unabhängig gekoppelt beziehungsweise entkoppelt werden können.

---

## 230. Synchronization Loop Prevention

View-Synchronisierung MUSS zyklische Update-Schleifen verhindern.

---

## 231. Visualization Scene

Eine Menge zusammengehöriger Visualisierungsobjekte wird als:

`MedicalVisualizationScene`

repräsentiert.

---

## 232. Scene Contents

Eine Scene SOLL enthalten können:

```text
Images
Volumes
Surfaces
Segmentations
Annotations
Measurements
Overlays
Camera
Lights
Clipping
ViewState
```

---

## 233. Scene Identity

Jede Scene MUSS eindeutig identifizierbar sein.

---

## 234. Scene Version

Scenes SOLLEN versionierbar sein.

---

## 235. Scene Serialization

Visualisierungszustände SOLLEN serialisierbar sein.

---

## 236. Source Separation

Serialisierte View States SOLLTEN Quelldaten referenzieren und nicht unnötig duplizieren.

---

## 237. Rendering Backend

Ein Rendering Backend wird als:

`MedicalRenderBackend`

repräsentiert.

---

## 238. Backend Types

Mindestens folgende Backend-Klassen SOLLEN unterstützt werden können:

```text
CPU
GPU
Software
Remote
```

---

## 239. Backend Capability

Ein Backend MUSS seine Fähigkeiten melden können.

---

## 240. Capability Examples

Beispielsweise:

```text
VolumeRendering
SurfaceRendering
ComputeShaders
Texture3D
LargeTextures
RayCasting
HDR
MultiSampling
```

---

## 241. Backend Selection

Die Runtime SOLL ein geeignetes Backend automatisch auswählen können.

---

## 242. Explicit Backend

Aufrufer SOLLEN ein Backend explizit anfordern können.

---

## 243. Backend Fallback

Bei Ausfall eines optionalen GPU-Backends SOLL, soweit technisch möglich, ein CPU- beziehungsweise Software-Fallback existieren.

---

## 244. Backend Failure

Backend-Fehler MÜSSEN diagnostizierbar sein.

---

## 245. GPU Runtime

Medical Visualization Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` integrieren.

---

## 246. GPU Operations

Geeignete GPU-Operationen umfassen:

```text
Resampling
MPR
Projection
Volume Rendering
Surface Rendering
Overlay Composition
Color Mapping
Transfer Functions
Segmentation Rendering
Point Cloud Rendering
Vector Visualization
```

---

## 247. GPU Memory Budget

Medical Visualization Compute MUSS GPU-Speicherbudgets berücksichtigen.

---

## 248. Texture Cache

Bild- und Volumentexturen SOLLEN zwischengespeichert werden können.

---

## 249. GPU Resource Eviction

Nicht benötigte GPU-Ressourcen SOLLEN kontrolliert freigegeben werden.

---

## 250. GPU Upload

Datenübertragungen zur GPU SOLLEN minimiert werden.

---

## 251. Incremental Upload

Große Datensätze SOLLEN inkrementell übertragen werden können.

---

## 252. CPU Backend

Grundlegende medizinische 2D-Visualisierung MUSS ohne GPU möglich sein.

---

## 253. SIMD

CPU-basierte Pixel- und Resamplingoperationen SOLLEN SIMD nutzen können.

---

## 254. Multi-Core

Geeignete Visualisierungsoperationen SOLLEN Multi-Core-Verarbeitung unterstützen.

---

## 255. Scheduler Integration

Medical Visualization Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 256. Interactive Priority

Interaktive Visualisierungsaufgaben SOLLEN gegenüber nicht sichtbaren Hintergrundaufgaben priorisiert werden.

---

## 257. Task Classes

Mindestens:

```text
VISIBLE_INTERACTIVE
PREFETCH
BACKGROUND
EXPORT
```

SOLLTEN unterscheidbar sein.

---

## 258. Cancellation

Nicht mehr benötigte Render- und Prepare-Tasks MÜSSEN abbrechbar sein.

---

## 259. Obsolete Task

Veraltete Tasks SOLLEN verworfen werden können, wenn ein neuer View State vorliegt.

---

## 260. Frame Deadline

Interaktive Renderaufgaben SOLLEN Frame Deadlines verwenden können.

---

## 261. Progressive Quality

Bei begrenztem Compute Budget SOLL zunächst eine schnellere niedrigere Qualität und anschließend eine höhere Qualität erzeugt werden können.

---

## 262. Memory Model

Medical Visualization Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 263. Memory Objects

Mindestens folgende Objekte SOLLEN unterstützt werden:

```text
ImageBuffer
VolumeBuffer
SliceBuffer
TextureBuffer
OverlayBuffer
MeshBuffer
PointBuffer
VectorBuffer
TransferFunctionBuffer
RenderTarget
```

---

## 264. Memory Budget

Views SOLLEN Speicherbudgets besitzen können.

---

## 265. Shared Resources

Mehrere Ansichten SOLLEN unveränderliche Quelldaten gemeinsam verwenden können.

---

## 266. Copy Avoidance

Unnötige Kopien großer medizinischer Datensätze SOLLEN vermieden werden.

---

## 267. Zero-Copy

Geeignete CPU/GPU-Pfade SOLLEN Zero-Copy beziehungsweise reduzierte Kopierpfade unterstützen können.

---

## 268. Large Data

Medical Visualization Compute MUSS große medizinische Datensätze unterstützen.

---

## 269. Out-of-Core Visualization

Datensätze größer als RAM oder GPU-Speicher SOLLEN über Out-of-Core-Verfahren visualisierbar sein.

---

## 270. Streaming

Daten SOLLEN bedarfsgerecht gestreamt werden können.

---

## 271. Region of Interest Loading

Nur aktuell benötigte Datenregionen SOLLEN geladen werden können.

---

## 272. Level of Detail

Die Visualisierung SOLL abhängig von Zoom, Entfernung und View-Auflösung geeignete Detailstufen verwenden können.

---

## 273. Quality Adaptation

Renderingqualität SOLL dynamisch an verfügbare Ressourcen angepasst werden können.

---

## 274. Interaction Quality

Während Interaktion DARF temporär reduzierte Darstellungsqualität verwendet werden.

---

## 275. Final Quality

Nach Ende einer Interaktion SOLL eine höherwertige Darstellung erzeugt werden können.

---

## 276. DICOM Integration

Medical Visualization Compute MUSS `NPSPEC-COMPUTE-DICOM-0001` integrieren können.

---

## 277. DICOM Pixel Data

DICOM-Pixeldaten SOLLEN visualisiert werden können.

---

## 278. DICOM Geometry

DICOM-Bildgeometrie MUSS korrekt interpretiert werden.

---

## 279. DICOM Modality Transform

Modalitätstransformationen SOLLEN korrekt angewendet werden.

---

## 280. DICOM VOI

DICOM VOI-Informationen SOLLEN unterstützt werden.

---

## 281. DICOM Presentation State

Standardisierte Presentation States SOLLEN architektonisch unterstützt werden.

---

## 282. DICOM Segmentation

DICOM-Segmentierungen SOLLEN als Overlays beziehungsweise Oberflächen dargestellt werden können.

---

## 283. DICOM Structured Measurements

Geeignete strukturierte Messinformationen SOLLEN visualisierbar sein.

---

## 284. DICOM Provenance

Visualisierungsobjekte SOLLEN ihre DICOM-Quellen referenzieren.

---

## 285. PACS Integration

Medical Visualization Compute SOLL `NPSPEC-COMPUTE-PACS-0001` integrieren.

---

## 286. Progressive Study Loading

Studien SOLLEN progressiv geladen und dargestellt werden können.

---

## 287. Background Prefetch

Nach aktuell sichtbaren Daten SOLLEN angrenzende beziehungsweise erwartete Daten vorgeladen werden können.

---

## 288. Radiology Integration

Medical Visualization Compute SOLL `NPSPEC-COMPUTE-RADIOLOGY-0001` integrieren.

---

## 289. Radiology Services

Insbesondere SOLLEN wiederverwendet werden:

```text
Image Geometry
Volume Geometry
Resampling
Registration
Segmentation
Measurements
Surface Generation
```

---

## 290. Visualization Result

Ein erzeugter Visualisierungszustand wird als:

`MedicalVisualizationResult`

repräsentiert.

---

## 291. Result Types

Ergebnisse können sein:

```text
PreparedImage
PreparedSlice
PreparedVolume
PreparedSurface
OverlayComposition
RenderFrame
ExportImage
```

---

## 292. Result Source

Jedes Ergebnis MUSS seine Quelldaten referenzieren können.

---

## 293. Result State

Mindestens folgende Zustände SOLLEN unterstützt werden:

```text
VALID
PARTIAL
DEGRADED
INVALID
FAILED
CANCELLED
```

---

## 294. Partial Rendering

Progressive beziehungsweise unvollständige Frames MÜSSEN als solche gekennzeichnet werden können.

---

## 295. Render Frame

Ein gerenderter Frame wird als:

`MedicalRenderFrame`

repräsentiert.

---

## 296. Render Frame Metadata

Ein Frame SOLL enthalten können:

```text
ViewId
SceneVersion
SourceVersions
Dimensions
Timestamp
Backend
Quality
Latency
```

---

## 297. Stale Frame

Ein Frame, der nicht mehr zum aktuellen View State gehört, MUSS erkennbar sein.

---

## 298. No Stale Guidance

Veraltete Image-Guided-Frames DÜRFEN nicht ohne Kennzeichnung als aktuelle Guidance ausgegeben werden.

---

## 299. Color Management

Medical Visualization Compute SOLL kontrolliertes Farbmanagement unterstützen.

---

## 300. Source Color Space

Der Farbraum farbiger Quelldaten MUSS bekannt oder als unbekannt markiert sein.

---

## 301. Display Color Space

Das Ziel-Farbprofil SOLL berücksichtigt werden können.

---

## 302. Medical Grayscale

Graustufenverarbeitung DARF nicht unnötig durch farbbasierte Transformationen verändert werden.

---

## 303. Quantitative Colors

Quantitative Color Maps MÜSSEN unabhängig vom dekorativen UI-Farbschema definiert sein.

---

## 304. Transparency

Alpha-Compositing SOLL unterstützt werden.

---

## 305. Alpha Model

Straight- und Premultiplied-Alpha MÜSSEN eindeutig unterscheidbar sein.

---

## 306. Overlay Alpha

Overlay-Compositing MUSS ein definiertes Alpha-Modell verwenden.

---

## 307. Numerical Precision

Medical Visualization Compute MUSS numerische Präzision explizit verwalten können.

---

## 308. Coordinate Precision

Räumliche Transformationsberechnungen SOLLTEN ausreichende Präzision für medizinische Bildgeometrien verwenden.

---

## 309. Shader Precision

Reduzierte GPU-Präzision DARF nicht ohne Bewertung für geometrisch kritische Berechnungen verwendet werden.

---

## 310. Deterministic Mode

Medical Visualization Compute SOLL einen deterministischen Modus unterstützen können.

---

## 311. Deterministic Output

Identische Eingaben, View States und Backends SOLLTEN innerhalb definierter Toleranzen reproduzierbare Ergebnisse liefern.

---

## 312. Quality Model

Eine Visualisierung SOLL einen technischen Qualitätszustand besitzen können.

---

## 313. Quality Inputs

Qualität kann abhängen von:

```text
SourceQuality
GeometryValidity
RegistrationQuality
TrackingQuality
Resolution
LevelOfDetail
Backend
RenderCompleteness
```

---

## 314. Quality States

Mindestens:

```text
FULL
REDUCED
PROGRESSIVE
DEGRADED
INVALID
UNKNOWN
```

SOLLTEN unterstützt werden.

---

## 315. Unknown Is Not Full

`UNKNOWN` DARF nicht automatisch als vollständige Qualität behandelt werden.

---

## 316. Provenance

Medical Visualization Compute MUSS Provenance unterstützen.

---

## 317. Provenance Record

Ein:

`MedicalVisualizationProvenanceRecord`

SOLL mindestens enthalten können:

```text
SourceData
SourceVersion
Geometry
Registration
ViewState
WindowLevel
TransferFunction
Overlays
Resampling
Backend
RuntimeVersion
Timestamp
```

---

## 318. Extended Provenance

Zusätzlich SOLLEN dokumentiert werden können:

```text
Interpolation
Projection
Clipping
Cropping
ColorMap
Opacity
Quality
AIResult
NavigationEpoch
TrackingTimestamp
```

---

## 319. Export

Medical Visualization Compute SOLL Visualisierungsergebnisse exportieren können.

---

## 320. Export Types

Mögliche Exporttypen umfassen:

```text
RasterImage
VectorOverlay
RenderedFrame
Video
SceneDescription
PresentationState
```

---

## 321. Export Source Preservation

Ein Export MUSS von den medizinischen Quelldaten unterscheidbar bleiben.

---

## 322. Burned-In Overlays

In Rasterexporte eingebrannte Overlays SOLLEN als solche dokumentiert werden.

---

## 323. Export Resolution

Exportauflösung MUSS explizit definiert werden.

---

## 324. Export Color Space

Der Exportfarbraum MUSS definiert werden können.

---

## 325. Export Provenance

Exporte SOLLEN ihre Visualisierungsparameter dokumentieren können.

---

## 326. Security

Medical Visualization Compute MUSS in die NovaOS-Sicherheitsarchitektur integriert werden.

---

## 327. Authentication

Aufrufende Benutzer und Dienste MÜSSEN authentifizierbar sein.

---

## 328. Authorization

Zugriff auf medizinische Daten MUSS autorisierbar sein.

---

## 329. GPU Isolation

GPU-Ressourcen medizinischer Workloads SOLLEN gegenüber nicht vertrauenswürdigen Prozessen isoliert werden können.

---

## 330. Shader Validation

Dynamisch geladene Renderprogramme SOLLEN validierbar beziehungsweise isolierbar sein.

---

## 331. Resource Limits

Visualisierungsjobs MÜSSEN Ressourcenlimits unterstützen können.

---

## 332. Privacy

Medizinische Visualisierungsdaten MÜSSEN als sensible Daten behandelt werden.

---

## 333. Render Target Privacy

Temporäre Render Targets können medizinische Inhalte enthalten und MÜSSEN entsprechend behandelt werden.

---

## 334. Cache Privacy

Bild-, Tile-, Texture- und Volume-Caches MÜSSEN in Datenschutz- und Lebenszyklusregeln einbezogen werden.

---

## 335. Logging Privacy

Logs DÜRFEN keine unnötigen medizinischen Bildinhalte enthalten.

---

## 336. Audit

Relevante Visualisierungsoperationen SOLLEN auditierbar sein können.

---

## 337. Audit Events

Mindestens folgende Ereignisse SOLLEN unterstützt werden:

```text
MEDICAL_DATA_OPEN
VIEW_CREATE
VIEW_EXPORT
SCENE_EXPORT
AI_OVERLAY_ENABLE
NAVIGATION_VIEW_START
NAVIGATION_VIEW_END
```

---

## 338. Diagnostics

Medical Visualization Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 339. Diagnostic Classes

Mindestens folgende Klassen SOLLEN existieren:

```text
INVALID_IMAGE
INVALID_VOLUME
INVALID_GEOMETRY
UNKNOWN_ORIENTATION
INVALID_TRANSFORM
REGISTRATION_MISSING
REGISTRATION_INVALID
INVALID_VIEW
INVALID_SLICE
INVALID_TRANSFER_FUNCTION
INVALID_COLOR_MAP
INVALID_OVERLAY
INVALID_SEGMENTATION
INVALID_MESH
INVALID_POINT_CLOUD
INVALID_VECTOR_FIELD
UNSUPPORTED_PIXEL_FORMAT
UNSUPPORTED_RENDER_MODE
GPU_UNAVAILABLE
GPU_OUT_OF_MEMORY
CPU_OUT_OF_MEMORY
BACKEND_FAILURE
SHADER_FAILURE
TEXTURE_UPLOAD_FAILED
STREAMING_FAILED
STALE_TRACKING
INVALID_NAVIGATION_STATE
OUT_OF_MEMORY
CANCELLED
```

---

## 340. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 341. Diagnostic Context

Diagnosen SOLLEN enthalten können:

```text
ViewId
SceneId
SourceId
Backend
Frame
Timestamp
```

---

## 342. Profiling

Medical Visualization Compute MUSS Profiling unterstützen.

---

## 343. Profiling Stages

Mindestens folgende Bereiche SOLLTEN separat messbar sein:

```text
DataLoad
Decode
Resampling
SliceExtraction
MPR
Projection
VolumePreparation
VolumeRendering
SurfacePreparation
SurfaceRendering
OverlayComposition
GPUUpload
FrameRendering
Export
```

---

## 344. Frame Time

Renderzeit pro Frame SOLL messbar sein.

---

## 345. Frame Rate

Effektive Framerate SOLL messbar sein.

---

## 346. Interaction Latency

Zeit zwischen View-State-Änderung und sichtbarem Ergebnis SOLL messbar sein.

---

## 347. Time To First Frame

Zeit bis zum ersten darstellbaren Frame SOLL messbar sein.

---

## 348. Time To Full Quality

Zeit bis zur vollständigen Darstellungsqualität SOLL messbar sein.

---

## 349. GPU Memory

GPU-Speicherverbrauch SOLL messbar sein.

---

## 350. CPU Memory

CPU-Speicherverbrauch SOLL messbar sein.

---

## 351. Cache Metrics

Mindestens:

```text
Hits
Misses
Evictions
BytesUsed
```

SOLLTEN für relevante Caches verfügbar sein.

---

## 352. Dropped Frames

Verworfene Frames SOLLEN gezählt werden können.

---

## 353. Testing — Geometry

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
IdentityGeometry
AnisotropicSpacing
RotatedImage
ObliqueImage
NegativeDirection
UnknownOrientation
LargeCoordinates
```

---

## 354. Testing — 2D

Mindestens:

```text
SingleImage
ImageSeries
WindowLevel
Zoom
Pan
Rotation
Interpolation
```

MÜSSEN getestet werden.

---

## 355. Testing — MPR

Mindestens:

```text
Axial
Coronal
Sagittal
Oblique
CrosshairSynchronization
ReferenceLines
```

MÜSSEN getestet werden.

---

## 356. Testing — Projection

Mindestens:

```text
MIP
MinIP
AIP
ThinSlab
ThickSlab
```

SOLLTEN getestet werden.

---

## 357. Testing — Volume Rendering

Mindestens:

```text
SmallVolume
LargeVolume
TransferFunction
Opacity
Clipping
Cropping
ProgressiveRendering
```

SOLLTEN getestet werden.

---

## 358. Testing — Segmentation

Mindestens:

```text
BinaryMask
MultiLabel
Outline
Filled
Surface
Opacity
```

MÜSSEN getestet werden.

---

## 359. Testing — Fusion

Mindestens:

```text
RegisteredFusion
MissingRegistration
AlphaBlend
Checkerboard
ColorOverlay
```

SOLLTEN getestet werden.

---

## 360. Testing — Measurements

Mindestens:

```text
Distance
Angle
Area
Volume
AnisotropicSpacing
ObliquePlane
```

MÜSSEN getestet werden.

---

## 361. Testing — Orientation

Anatomische Orientierungsmarker MÜSSEN gegen bekannte Bildorientierungen getestet werden.

---

## 362. Testing — Scale

Maßstabsdarstellung MUSS gegen bekannte physikalische Pixelabstände getestet werden.

---

## 363. Testing — Pathology

Mindestens:

```text
LargeWSI
PyramidSelection
TileStreaming
HighZoom
LowZoom
MillionObjectOverlay
```

SOLLTEN getestet werden.

---

## 364. Testing — Ultrasound

Mindestens:

```text
BMode
Cine
SectorGeometry
TrackedUltrasound
```

SOLLTEN getestet werden.

---

## 365. Testing — Medical AI

Mindestens:

```text
BoundingBox
Mask
Heatmap
ProbabilityMap
LowConfidence
InvalidResult
```

SOLLTEN getestet werden.

---

## 366. Testing — Image Guided

Mindestens:

```text
ValidTracking
TrackingLoss
StaleTracking
Target
Trajectory
SafetyRegion
```

MÜSSEN getestet werden.

---

## 367. Testing — GPU Failure

GPU-Ausfall während einer Visualisierung MUSS kontrolliert behandelt werden.

---

## 368. Testing — CPU Fallback

Grundlegende Visualisierung SOLL nach GPU-Ausfall über einen verfügbaren CPU-Fallback fortgesetzt werden können.

---

## 369. Testing — Large Data

Datensätze größer als verfügbarer GPU-Speicher SOLLTEN getestet werden.

---

## 370. Testing — Out-of-Core

Out-of-Core-Visualisierung MUSS mit begrenztem Speicher getestet werden.

---

## 371. Testing — Determinism

Der deterministische Modus SOLL mit identischen View States getestet werden.

---

## 372. Testing — Security

Mindestens folgende Fälle SOLLTEN getestet werden:

```text
MalformedImage
MalformedMesh
OversizedVolume
InvalidShader
UnauthorizedAccess
ResourceExhaustion
```

---

## 373. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
VeryLargeVolume
GigapixelWSI
ThousandsOfSlices
ManySimultaneousViews
ManyOverlays
MillionsOfMeshTriangles
MillionsOfPoints
MillionsOfPathologyObjects
HighFrequencyTracking
RapidWindowLevelChanges
RapidSliceScrolling
ContinuousCine
GPU Memory Pressure
CPU Memory Pressure
BackendSwitch
LongRunningSession
```

---

## 374. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
MedicalVisualizationObject
MedicalView
MedicalViewState

2D Image Visualization
Image Series
Slice Extraction

Axial MPR
Coronal MPR
Sagittal MPR
Oblique MPR

Window/Level
Lookup Tables
Color Maps

Nearest Interpolation
Linear Interpolation

Zoom
Pan
Rotation
Fit To View

Crosshair
Linked Views
Reference Lines

Orientation Markers
Physical Scale

Annotations
Distance Measurement
Angle Measurement
Area Measurement

Segmentation Overlay
Label Maps
Overlay Composition

Basic Surface Visualization
Basic Volume Visualization

CPU Backend
GPU Backend Architecture
CPU Fallback

Scheduler Integration
Memory Model Integration
Object Model Integration

DICOM Integration
Radiology Integration

Provenance
Diagnostics
Profiling
Cancellation
Security
```

---

## 375. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
CPR
MIP
MinIP
AIP

Advanced Volume Rendering
Ray Casting
Adaptive Sampling
Bricked Volumes
Progressive Rendering

Advanced Surface Rendering
Mesh Decimation
Point Clouds
Vector Fields
Tensor Fields

Multi-Modal Fusion
Advanced Transfer Functions
Advanced Color Management

Cine
Temporal Synchronization

Whole Slide Pathology
Pathology Tile Streaming
Massive Annotation Rendering

Ultrasound Visualization
Tracked Ultrasound

Medical AI Overlays
Radiomics Maps

Image-Guided Navigation
Tracked Instruments
Targets
Trajectories
Safety Regions

Out-of-Core Visualization
Remote Rendering
Distributed Preparation
```

---

## 376. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-MEDVIS-0001` MUSS:

1. medizinische Visualisierungsobjekte repräsentieren.
2. Visualisierungsobjekte eindeutig identifizieren.
3. Quelldaten referenzieren.
4. abgeleitete Visualisierungsobjekte kennzeichnen.
5. Bildgeometrie erhalten.
6. physikalische Koordinaten unterstützen.
7. Pixel-, Voxel- und physikalische Koordinaten unterscheiden.
8. Bildorientierung erhalten.
9. unbekannte Orientierung explizit darstellen.
10. unbekannte Orientierung nicht stillschweigend erraten.
11. medizinische Views repräsentieren.
12. View States repräsentieren.
13. 2D-Bilddarstellung unterstützen.
14. Bildserien darstellen.
15. Slice Extraction unterstützen.
16. Slice Index von physikalischer Position unterscheiden.
17. MPR unterstützen.
18. axiale MPR unterstützen.
19. koronale MPR unterstützen.
20. sagittale MPR unterstützen.
21. oblique MPR unterstützen.
22. MPR-Ebenen geometrisch definieren.
23. Crosshair-Synchronisierung unterstützen.
24. Reference Lines unterstützen.
25. Window/Level unterstützen.
26. Window/Level als nicht destruktive Darstellung behandeln.
27. Lookup Tables unterstützen.
28. Color Maps unterstützen.
29. mindestens Nearest-Interpolation unterstützen.
30. mindestens Linear-Interpolation unterstützen.
31. Label Maps ohne ungeeignete kontinuierliche Interpolation behandeln.
32. Zoom unterstützen.
33. Pan unterstützen.
34. View Rotation unterstützen.
35. Fit To View unterstützen.
36. anatomische Orientierungsmarker unterstützen.
37. Orientierungsmarker aus tatsächlicher Geometrie ableiten.
38. physikalische Maßstabsdarstellung unterstützen.
39. keinen falschen physikalischen Maßstab bei unbekannter Kalibrierung darstellen.
40. Annotationen visualisieren.
41. Distanzmessungen darstellen.
42. Winkelmessungen darstellen.
43. Flächenmessungen darstellen.
44. Messungen auf medizinischer Geometrie berechnen.
45. Segmentierungen darstellen.
46. Label Maps darstellen.
47. mehrere Overlays komponieren.
48. Overlay-Sichtbarkeit steuern.
49. Overlay-Opazität steuern.
50. räumliche Overlays korrekt ausrichten.
51. grundlegende Oberflächenvisualisierung unterstützen.
52. grundlegende Volumenvisualisierung architektonisch unterstützen.
53. CPU-basierte Grundvisualisierung unterstützen.
54. GPU-Beschleunigung architektonisch unterstützen.
55. GPU-Speicherbudgets berücksichtigen.
56. CPU-Fallback architektonisch unterstützen.
57. den Compute Scheduler integrieren.
58. das Compute Memory Model integrieren.
59. das Compute Object Model integrieren.
60. interaktive Tasks priorisieren.
61. veraltete Tasks abbrechen können.
62. Cancellation unterstützen.
63. große medizinische Daten architektonisch unterstützen.
64. Streaming unterstützen können.
65. Level of Detail unterstützen können.
66. Originaldaten unverändert erhalten.
67. DICOM Compute integrieren.
68. DICOM-Geometrie korrekt berücksichtigen.
69. Radiology Compute integrieren.
70. PACS Compute architektonisch integrieren.
71. Ultrasound Compute architektonisch integrieren.
72. Pathology Compute architektonisch integrieren.
73. Medical AI Compute architektonisch integrieren.
74. Image-Guided Compute architektonisch integrieren.
75. AI-Ergebnisse von Ground Truth unterscheiden.
76. Tracking-Gültigkeit berücksichtigen.
77. stale Tracking-Daten erkennen können.
78. quantitative Wertebereiche nachvollziehbar darstellen.
79. quantitative Einheiten erhalten.
80. Color-Map-Skalierung dokumentierbar machen.
81. Visualisierungsqualität repräsentieren.
82. progressive Ergebnisse kennzeichnen.
83. stale Frames erkennen.
84. Provenance unterstützen.
85. Source Version dokumentieren können.
86. View State dokumentieren können.
87. Transfer Functions dokumentieren können.
88. Resampling dokumentieren können.
89. Backend dokumentieren können.
90. strukturierte Diagnosen bereitstellen.
91. Backend-Fehler diagnostizieren.
92. Profiling unterstützen.
93. Frame Time messen können.
94. Interaction Latency messen können.
95. Speicherverbrauch messen können.
96. Datenschutz für temporäre Visualisierungsressourcen berücksichtigen.
97. Audit architektonisch unterstützen.
98. medizinische Quelldaten von Darstellungsparametern trennen.
99. Rendering von medizinischer Interpretation trennen.
100. Erweiterungen ohne Änderung des grundlegenden Visualisierungsobjektmodells ermöglichen.

---

## 377. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaMedicalVisualizationCompute
 |
 +-- MedicalVisualizationRuntime
 |
 +-- VisualizationObjectModel
 |
 +-- MedicalGeometryEngine
 |
 +-- ViewManager
 |
 +-- ViewStateManager
 |
 +-- SliceEngine
 |
 +-- MPREngine
 |
 +-- CPREngine
 |
 +-- ProjectionEngine
 |
 +-- ResamplingEngine
 |
 +-- WindowLevelEngine
 |
 +-- LookupTableEngine
 |
 +-- TransferFunctionEngine
 |
 +-- VolumeRenderingEngine
 |
 +-- SurfaceRenderingEngine
 |
 +-- MeshEngine
 |
 +-- PointCloudEngine
 |
 +-- VectorFieldEngine
 |
 +-- OverlayEngine
 |
 +-- SegmentationVisualization
 |
 +-- AnnotationVisualization
 |
 +-- MeasurementVisualization
 |
 +-- FusionEngine
 |
 +-- TemporalVisualization
 |
 +-- CineEngine
 |
 +-- PathologyVisualization
 |
 +-- UltrasoundVisualization
 |
 +-- MedicalAIVisualization
 |
 +-- ImageGuidedVisualization
 |
 +-- LargeDataManager
 |
 +-- ResourceCache
 |
 +-- BackendManager
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

## 378. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt die gemeinsame Ausführungsinfrastruktur bereit.

```text
Medical Visualization
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

## 379. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert insbesondere:

```text
Slice Extraction
MPR
Resampling
Projection
Volume Preparation
Surface Preparation
Overlay Composition
Streaming
Prefetching
Rendering
```

---

## 380. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet insbesondere:

```text
Image Buffers
Volume Buffers
Slices
Textures
Meshes
Overlays
Render Targets
Caches
```

---

## 381. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Visualisierungsobjekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 382. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

GPU Runtime stellt Hardwarebeschleunigung für:

```text
Resampling
Projection
Volume Rendering
Surface Rendering
Overlay Composition
Color Mapping
```

bereit.

---

## 383. Beziehung zu `NPSPEC-COMPUTE-DICOM-0001`

DICOM Compute stellt:

```text
Pixel Data
Image Geometry
Modality Information
VOI Information
Segmentations
Presentation Information
```

bereit.

---

## 384. Beziehung zu `NPSPEC-COMPUTE-RADIOLOGY-0001`

Radiology Compute stellt medizinische Bild-, Volumen-, Segmentierungs-, Registrierungs- und Messdaten bereit.

---

## 385. Beziehung zu `NPSPEC-COMPUTE-PACS-0001`

PACS Compute stellt medizinische Studien für progressive Visualisierung bereit.

---

## 386. Beziehung zu `NPSPEC-COMPUTE-ULTRASOUND-0001`

Ultrasound Compute stellt Ultraschallbilder, Cine-Daten, Volumen und Doppler-Daten bereit.

---

## 387. Beziehung zu `NPSPEC-COMPUTE-MEDAI-0001`

Medical AI Compute stellt visualisierbare:

```text
Detections
Segmentations
Landmarks
Heatmaps
Probability Maps
Classifications
```

bereit.

---

## 388. Beziehung zu `NPSPEC-COMPUTE-RADIOMICS-0001`

Radiomics Compute stellt quantitative Karten und Merkmale bereit.

---

## 389. Beziehung zu `NPSPEC-COMPUTE-IMAGEGUIDED-0001`

Image-Guided Compute stellt:

```text
Tracking
Transforms
Targets
Trajectories
Instrument Geometry
Safety Regions
Navigation State
```

für die Visualisierung bereit.

---

## 390. Beziehung zu `NPSPEC-COMPUTE-PATHOLOGY-0001`

Pathology Compute stellt:

```text
Whole Slide Images
Image Pyramids
Tiles
Annotations
Cells
Nuclei
Segmentations
Pathomics Results
```

bereit.

---

## 391. Grundlegende 2D-Architektur

```text
Medical Image
      |
      v
Geometry
      |
      v
Slice / Image Selection
      |
      v
Window / Level
      |
      v
Color Mapping
      |
      v
Overlay Composition
      |
      v
2D Render Frame
```

---

## 392. MPR-Architektur

```text
Volume
  |
  v
Volume Geometry
  |
  +----------+----------+
  |          |          |
  v          v          v
Axial     Coronal    Sagittal
  |          |          |
  +----------+----------+
             |
             v
        Linked Views
```

---

## 393. Volume-Rendering-Architektur

```text
Volume
  |
  v
Volume Preparation
  |
  v
Transfer Function
  |
  v
Ray Casting
  |
  v
Shading / Composition
  |
  v
3D Render Frame
```

---

## 394. Overlay-Architektur

```text
Base Image
    |
    +--------------------+
    |                    |
Segmentation          AI Result
    |                    |
    +---------+----------+
              |
         Annotation
              |
              v
       Overlay Engine
              |
              v
       Composite Frame
```

---

## 395. Large-Data-Architektur

```text
Large Medical Dataset
        |
        v
Spatial Index / Pyramid
        |
        v
Visible Region
        |
        v
LOD Selection
        |
        v
Streaming / Cache
        |
        v
Rendering
```

---

## 396. Image-Guided-Architektur

```text
Medical Image
      |
      v
Image View
      |
      +----------------+
      |                |
      v                v
Tracking            Target
      |                |
      v                v
Instrument        Trajectory
      |                |
      +-------+--------+
              |
              v
        Guidance Overlay
              |
              v
       Navigation View
```

---

## 397. Pathology-Visualisierungsarchitektur

```text
Whole Slide Image
       |
       v
Image Pyramid
       |
       v
Visible Region
       |
       v
Tile Selection
       |
       v
Tile Cache
       |
       v
Tile Composition
       |
       +----------------+
       |                |
       v                v
Annotations        Cell Objects
       |                |
       +-------+--------+
               |
               v
         Pathology View
```

---

## 398. Datenmodellgrundsatz

Medical Visualization Compute MUSS zwischen:

```text
Source Data
Geometry
Visualization Object
View
View State
Scene
Overlay
Render Backend
Render Frame
```

unterscheiden.

---

## 399. Geometriegrundsatz

Medizinische Bildgeometrie ist Bestandteil der Datenbedeutung.

Daher gilt:

```text
Pixel Value + Pixel Index
```

allein ist nicht ausreichend.

Eine räumlich interpretierbare Darstellung benötigt mindestens:

```text
Pixel Value
Pixel Index
Spacing
Origin
Orientation
Coordinate System
```

---

## 400. Darstellungsgrundsatz

Eine Darstellung ist keine Veränderung der Quelldaten.

Operationen wie:

```text
WindowLevel
Zoom
Pan
Rotation
ColorMap
TransferFunction
Opacity
Clipping
Cropping
```

MÜSSEN als View- beziehungsweise Visualisierungszustand behandelt werden.

---

## 401. Messgrundsatz

Eine medizinische Messung MUSS auf der räumlichen Datengeometrie basieren.

Daher gilt:

```text
Screen Distance != Physical Distance
```

sofern keine korrekte geometrische Transformation vorliegt.

---

## 402. Overlay-Grundsatz

Ein Overlay darf nur dann räumlich über einem medizinischen Bild dargestellt werden, wenn seine räumliche Beziehung zum Bild bekannt ist.

Daher gilt:

```text
Same Matrix Size != Same Geometry
```

---

## 403. Fusion-Grundsatz

Multimodale Bilder dürfen nicht allein aufgrund ähnlicher Bilddimensionen fusioniert werden.

Erforderlich ist eine bekannte räumliche Beziehung beziehungsweise Registrierung.

---

## 404. Quantitativer Visualisierungsgrundsatz

Eine Farbe besitzt in einer quantitativen medizinischen Visualisierung nur zusammen mit ihrer Mapping-Funktion eine quantitative Bedeutung.

Daher gilt:

```text
Color
+
Color Map
+
Value Range
+
Unit
=
Interpretable Quantitative Visualization
```

---

## 405. Progressive-Rendering-Grundsatz

Eine reduzierte interaktive Darstellungsqualität ist zulässig, sofern sie als Darstellungsoptimierung behandelt wird und die zugrunde liegenden medizinischen Daten nicht verändert.

---

## 406. Navigation-Visualisierungsgrundsatz

Eine räumlich korrekt gerenderte Instrumentengeometrie ist nur dann aktuelle Guidance, wenn:

```text
Tracking Valid
+
Registration Valid
+
Calibration Valid
+
Transform Chain Valid
+
Timing Valid
```

sind.

---

## 407. Erweiterbarkeit

Neue:

```text
Medical Modalities
Visualization Objects
Render Backends
Projection Methods
Transfer Functions
Overlay Types
Mesh Types
Point Representations
Vector Representations
AI Visualizations
Navigation Visualizations
Large Data Strategies
```

SOLLTEN ohne Änderung des grundlegenden Medical-Visualization-Objektmodells integrierbar sein.

---

## 408. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-MEDVIS-GEOMETRY-0001
Medical Visualization Geometry

NPSPEC-COMPUTE-MEDVIS-VIEW-0001
Medical View Model

NPSPEC-COMPUTE-MEDVIS-2D-0001
Medical 2D Visualization

NPSPEC-COMPUTE-MEDVIS-MPR-0001
Multi-Planar Reconstruction

NPSPEC-COMPUTE-MEDVIS-CPR-0001
Curved Planar Reconstruction

NPSPEC-COMPUTE-MEDVIS-PROJECTION-0001
Medical Projection Visualization

NPSPEC-COMPUTE-MEDVIS-VOLUME-0001
Medical Volume Rendering

NPSPEC-COMPUTE-MEDVIS-SURFACE-0001
Medical Surface Rendering

NPSPEC-COMPUTE-MEDVIS-MESH-0001
Medical Mesh Visualization

NPSPEC-COMPUTE-MEDVIS-OVERLAY-0001
Medical Overlay Composition

NPSPEC-COMPUTE-MEDVIS-SEGMENTATION-0001
Segmentation Visualization

NPSPEC-COMPUTE-MEDVIS-TRANSFER-0001
Medical Transfer Functions

NPSPEC-COMPUTE-MEDVIS-FUSION-0001
Multi-Modal Visualization

NPSPEC-COMPUTE-MEDVIS-MEASURE-0001
Medical Visualization Measurements

NPSPEC-COMPUTE-MEDVIS-CINE-0001
Medical Cine Visualization

NPSPEC-COMPUTE-MEDVIS-POINTCLOUD-0001
Medical Point Cloud Visualization

NPSPEC-COMPUTE-MEDVIS-VECTOR-0001
Medical Vector Field Visualization

NPSPEC-COMPUTE-MEDVIS-PATHOLOGY-0001
Digital Pathology Visualization

NPSPEC-COMPUTE-MEDVIS-ULTRASOUND-0001
Ultrasound Visualization

NPSPEC-COMPUTE-MEDVIS-AI-0001
Medical AI Visualization

NPSPEC-COMPUTE-MEDVIS-IMAGEGUIDED-0001
Image-Guided Visualization

NPSPEC-COMPUTE-MEDVIS-LARGEDATA-0001
Large Medical Data Visualization

NPSPEC-COMPUTE-MEDVIS-BACKEND-0001
Medical Rendering Backend

NPSPEC-COMPUTE-MEDVIS-COLOR-0001
Medical Visualization Color Management

NPSPEC-COMPUTE-MEDVIS-PROVENANCE-0001
Medical Visualization Provenance

NPSPEC-COMPUTE-MEDVIS-TEST-0001
Medical Visualization Conformance Tests
```

---

## 409. Zusammenfassung

`NPSPEC-COMPUTE-MEDVIS-0001` definiert die gemeinsame Compute-Infrastruktur für medizinische Visualisierung innerhalb der NovaOS-Compute-Architektur.

Die grundlegende Verarbeitung lautet:

```text
Medical Data
     |
     v
Geometry
     |
     v
Visualization Preparation
     |
     +------------------+
     |                  |
     v                  v
2D Visualization   3D Visualization
     |                  |
     +---------+--------+
               |
               v
          Overlays
               |
               v
         Render Frame
```

Die Architektur unterstützt:

```text
2D Images
Image Series
MPR
Oblique MPR
CPR
MIP
MinIP
AIP
Volume Rendering
Surface Rendering
Meshes
Point Clouds
Vector Fields
Tensor Fields
Segmentations
Annotations
Measurements
Multi-Modal Fusion
Cine
Whole Slide Imaging
Ultrasound
Medical AI
Image-Guided Navigation
```

Die Verarbeitung kann über:

```text
CPU
SIMD
Multi-Core
GPU
Streaming
Out-of-Core
```

beschleunigt beziehungsweise skaliert werden.

Die Architektur trennt konsequent:

```text
Medical Source Data
Geometry
Visualization Representation
View State
Scene
Overlay
Render Backend
Rendered Frame
Clinical Interpretation
```

voneinander.

Jede relevante Visualisierung bleibt auf:

```text
Source Data
Source Version
Geometry
Registration
View State
Window/Level
Transfer Function
Color Map
Interpolation
Resampling
Overlay Configuration
Backend
Runtime Version
```

zurückführbar.

Die zentrale Geometrieregel lautet:

> Medizinische Daten dürfen nur dann räumlich gemeinsam dargestellt werden, wenn ihre Koordinatensysteme und Transformationen bekannt beziehungsweise korrekt bestimmt sind. Gleiche Matrixdimensionen bedeuten nicht automatisch gleiche räumliche Geometrie.

Die zentrale Messregel lautet:

> Medizinische Messungen werden aus der physikalischen Datengeometrie und nicht aus der Darstellung auf dem Bildschirm bestimmt.

Die zentrale Darstellungsregel lautet:

> Window/Level, Transfer Functions, Color Maps, Zoom, Pan, Rotation, Clipping, Cropping und andere Darstellungsoperationen verändern den View State und nicht die medizinischen Originaldaten.

Die zentrale Guidance-Regel lautet:

> Ein dargestelltes Instrument oder Navigationsobjekt darf nur dann als aktuelle räumliche Guidance behandelt werden, wenn Tracking, Registrierung, Kalibrierung, Transformationskette und zeitliche Gültigkeit des zugrunde liegenden Navigationszustands gültig sind.

Das grundlegende Architekturprinzip lautet:

> NovaOS Medical Visualization Compute behandelt medizinische Visualisierung nicht als gewöhnliche Ausgabe von Pixeln auf einem Bildschirm. Bildgeometrie, physikalische Koordinaten, Orientierung, Volumen, Oberflächen, Segmentierungen, quantitative Karten, Annotationen, Messungen, multimodale Registrierungen, Tracking-Zustände, Transfer Functions und Darstellungsparameter werden als explizite Compute-Objekte behandelt. Dadurch können Radiologie, Ultraschall, digitale Pathologie, Medical AI, Radiomics und bildgestützte Navigation über eine gemeinsame, skalierbare Visualisierungsinfrastruktur dargestellt werden, während medizinische Originaldaten, quantitative Bedeutung und räumliche Provenance erhalten bleiben.