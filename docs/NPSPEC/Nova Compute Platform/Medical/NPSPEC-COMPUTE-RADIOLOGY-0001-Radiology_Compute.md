# NPSPEC-COMPUTE-RADIOLOGY-0001 — Radiology Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Medical Imaging / Radiology  
**Komponente:** NovaOS Radiology Compute  
**Bezeichner:** `NPSPEC-COMPUTE-RADIOLOGY-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`, `NPSPEC-COMPUTE-DICOM-0001`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`  
**Kurzname:** Radiology Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Radiology-Compute-Infrastruktur der NovaOS-Compute-Architektur.

Radiology Compute stellt höherwertige numerische und bildverarbeitende Funktionen für radiologische Bilddaten bereit.

Während:

`NPSPEC-COMPUTE-DICOM-0001`

primär für DICOM-Struktur, Metadaten, Pixeldekodierung, Serien, Geometrie und Volumenrekonstruktion zuständig ist, definiert Radiology Compute die darauf aufbauende medizinische Bildverarbeitung.

Die grundlegende Architektur lautet:

```text
DICOM Data
    |
    v
DICOM Compute
    |
    v
Radiology Compute
    |
    +--> Reconstruction
    +--> Enhancement
    +--> Registration
    +--> Segmentation
    +--> Measurement
    +--> Quantification
    +--> Visualization Preparation
    +--> AI Processing
```

Radiology Compute MUSS technische Bildverarbeitung und medizinische Interpretation logisch voneinander trennen.

Die Komponente ist keine eigenständige medizinische Diagnoseinstanz.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Radiologische Quelldaten, räumliche Geometrie, quantitative Werte, Verarbeitungsschritte, Segmentierungen, Messungen, Registrierungen, Rekonstruktionen, KI-Ergebnisse und Darstellungsinformationen werden als getrennte, nachvollziehbare Compute-Objekte behandelt.

Originaldaten DÜRFEN durch Analyseoperationen nicht stillschweigend verändert werden.

Abgeleitete Daten MÜSSEN als solche identifizierbar sein.

---

## 3. Ziele

Radiology Compute MUSS architektonisch mindestens folgende Bereiche unterstützen:

1. radiologische Bilddaten,
2. 2D-Bilder,
3. 3D-Volumen,
4. 4D-Daten,
5. CT,
6. MR,
7. Röntgen,
8. Mammographie,
9. PET,
10. SPECT,
11. Ultraschall,
12. Angiographie,
13. Bildrekonstruktion,
14. Resampling,
15. Reorientation,
16. Multiplanar Reconstruction,
17. Projektionen,
18. Filterung,
19. Bildverbesserung,
20. Rauschreduktion,
21. Registrierung,
22. Segmentierung,
23. quantitative Messungen,
24. Regionen,
25. Masken,
26. Histogramme,
27. Intensitätsstatistik,
28. geometrische Messungen,
29. Volumenmessungen,
30. Dichte- beziehungsweise Signalwertanalyse,
31. Zeitreihen,
32. Fusion verschiedener Modalitäten,
33. CPU-Ausführung,
34. GPU-Ausführung,
35. NPU-Ausführung,
36. HPC,
37. Distributed Compute,
38. reproduzierbare Verarbeitung,
39. Provenance,
40. Erweiterbarkeit.

---

## 4. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- DICOM-Dateiparsing,
- PACS,
- RIS,
- HIS,
- Befundverwaltung,
- Patientenverwaltung,
- Therapieentscheidungen,
- automatische klinische Diagnosen,
- medizinische Haftungsentscheidungen,
- regulatorische Zulassung,
- konkrete Viewer-Oberflächen,
- konkrete KI-Modelle.

Diese Bereiche MÜSSEN separat spezifiziert werden.

---

## 5. Sicherheitsgrundsatz

Radiologische Daten können sensible medizinische Informationen enthalten.

Radiology Compute MUSS deshalb mit den Sicherheits-, Berechtigungs-, Datenschutz- und Auditmechanismen von NovaOS integrierbar sein.

Die Compute-Komponente DARF nicht voraussetzen, dass medizinische Daten frei kopiert, exportiert oder dauerhaft gespeichert werden dürfen.

---

## 6. Klinischer Sicherheitsgrundsatz

Radiology Compute DARF technische Compute-Ergebnisse erzeugen wie:

```text
Measurement
Segmentation
Registration
Probability Map
Feature Map
Quantitative Value
```

Diese Ergebnisse DÜRFEN jedoch nicht automatisch als bestätigte medizinische Diagnose interpretiert werden.

---

## 7. Radiology Object Model

Die grundlegende Struktur lautet:

```text
RadiologyStudy
 |
 +-- RadiologySeries
      |
      +-- RadiologyImage
      +-- RadiologyVolume
      +-- RadiologyTimeSeries
      +-- RadiologySegmentation
      +-- RadiologyMeasurement
      +-- RadiologyRegistration
```

---

## 8. Radiology Study

Eine radiologische Studie SOLL als:

`RadiologyStudy`

repräsentiert werden.

---

## 9. Radiology Series

Eine radiologische Serie SOLL als:

`RadiologySeries`

repräsentiert werden.

---

## 10. Radiology Image

Ein einzelnes radiologisches Bild wird als:

`RadiologyImage`

bezeichnet.

---

## 11. Radiology Volume

Ein dreidimensionaler Datensatz wird als:

`RadiologyVolume`

bezeichnet.

---

## 12. Radiology Time Series

Zeitabhängige Daten werden als:

`RadiologyTimeSeries`

repräsentiert.

---

## 13. Source Reference

Jedes abgeleitete Radiology-Objekt SOLL seine Quelldaten referenzieren können.

---

## 14. Source Integrity

Quellreferenzen MÜSSEN stabil bleiben, solange das abgeleitete Objekt gültig ist.

---

## 15. DICOM Integration

Radiology Compute MUSS `NPSPEC-COMPUTE-DICOM-0001` integrieren.

---

## 16. DICOM Separation

Radiology Compute DARF DICOM-Parsing nicht duplizieren.

---

## 17. DICOM Input

DICOM Compute SOLL mindestens folgende Objekte bereitstellen können:

```text
DICOMImage
DICOMFrame
DICOMSeries
DICOMVolume
DICOMSegmentation
SpatialMetadata
```

---

## 18. Non-DICOM Input

Radiology Compute DARF auch andere medizinische beziehungsweise wissenschaftliche Bildformate verarbeiten, sofern räumliche und quantitative Metadaten eindeutig verfügbar sind.

---

## 19. Input Normalization

Unterschiedliche Eingabeformate SOLLEN auf ein gemeinsames internes Radiology-Compute-Modell normalisiert werden.

---

## 20. Original Values

Originale quantitative Pixel- oder Voxelwerte MÜSSEN erhalten werden können.

---

## 21. Display Values

Darstellungswerte MÜSSEN von quantitativen Werten getrennt bleiben.

---

## 22. Spatial Metadata

Jedes räumliche Bildobjekt SOLL mindestens besitzen:

```text
Dimensions
Spacing
Origin
Orientation
CoordinateSystem
```

---

## 23. Voxel Coordinate

Ein Voxel kann durch:

```text
(i, j, k)
```

adressiert werden.

---

## 24. Physical Coordinate

Physikalische Positionen SOLLEN unabhängig von Voxelindizes repräsentiert werden.

---

## 25. Coordinate Transformation

Transformationen zwischen:

```text
Voxel Space
Image Space
Patient Space
World Space
```

MÜSSEN darstellbar sein.

---

## 26. Transform Matrix

Affine räumliche Transformationen SOLLEN als Matrix repräsentiert werden können.

---

## 27. Transform Composition

Mehrere Transformationen SOLLEN zusammengesetzt werden können.

---

## 28. Inverse Transform

Invertierbare Transformationen SOLLEN invertiert werden können.

---

## 29. Transform Validation

Ungültige oder singuläre Transformationen MÜSSEN diagnostiziert werden.

---

## 30. Orientation

Bildorientierung MUSS explizit erhalten bleiben.

---

## 31. Reorientation

Radiology Compute SOLL Volumen in definierte Orientierungen transformieren können.

---

## 32. Reorientation Safety

Eine Reorientation MUSS Origin, Spacing und Direction entsprechend aktualisieren.

---

## 33. Resampling

Radiology Compute MUSS räumliches Resampling unterstützen.

---

## 34. Nearest Neighbor

Nearest-Neighbor-Interpolation MUSS unterstützt werden.

---

## 35. Linear Interpolation

Lineare beziehungsweise trilineare Interpolation MUSS unterstützt werden.

---

## 36. Cubic Interpolation

Kubische Interpolation SOLL unterstützt werden können.

---

## 37. Higher-Order Interpolation

Weitere Interpolationsverfahren DÜRFEN integrierbar sein.

---

## 38. Label Interpolation

Segmentierungslabels SOLLEN nicht standardmäßig mit kontinuierlichen Intensitätsinterpolatoren verarbeitet werden.

---

## 39. Isotropic Resampling

Anisotrope Volumendaten SOLLEN auf isotrope Voxelauflösung resampled werden können.

---

## 40. Target Spacing

Eine Zielauflösung MUSS explizit definierbar sein.

---

## 41. Target Geometry

Eine Zielgeometrie SOLL durch:

```text
Dimensions
Spacing
Origin
Orientation
```

definiert werden können.

---

## 42. Cropping

Radiologische Daten SOLLEN zugeschnitten werden können.

---

## 43. Region of Interest

Eine Region of Interest wird abgekürzt:

`ROI`

---

## 44. ROI Object

Eine ROI SOLL als eigenständiges Objekt repräsentierbar sein.

---

## 45. ROI Types

Mindestens folgende ROI-Arten SOLLEN unterstützt werden können:

```text
Point
Line
Rectangle
Ellipse
Polygon
Freehand
Mask
Volume
```

---

## 46. ROI Coordinate System

Eine ROI MUSS ihr Koordinatensystem deklarieren.

---

## 47. ROI Transformation

ROIs SOLLEN zwischen kompatiblen registrierten Bildräumen transformiert werden können.

---

## 48. Padding

Bild- und Volumendaten SOLLEN erweitert werden können.

---

## 49. Padding Value

Der Padding-Wert MUSS definierbar sein.

---

## 50. Multiplanar Reconstruction

Radiology Compute MUSS Multiplanar Reconstruction unterstützen.

Abkürzung:

`MPR`

---

## 51. Axial MPR

Axiale Rekonstruktion SOLL unterstützt werden.

---

## 52. Coronal MPR

Koronale Rekonstruktion SOLL unterstützt werden.

---

## 53. Sagittal MPR

Sagittale Rekonstruktion SOLL unterstützt werden.

---

## 54. Oblique MPR

Schräge beziehungsweise freie Schnitte SOLLEN unterstützt werden.

---

## 55. MPR Plane

Eine MPR-Ebene SOLL mindestens definieren:

```text
Origin
Normal
UpVector
PixelSpacing
Dimensions
```

---

## 56. Curved Planar Reformation

Curved Planar Reformation SOLL als Erweiterung integrierbar sein.

Abkürzung:

`CPR`

---

## 57. CPR Path

Eine CPR-Rekonstruktion SOLL entlang eines räumlichen Pfades definiert werden können.

---

## 58. Maximum Intensity Projection

Maximum Intensity Projection SOLL unterstützt werden.

Abkürzung:

`MIP`

---

## 59. Minimum Intensity Projection

Minimum Intensity Projection SOLL unterstützt werden.

Abkürzung:

`MinIP`

---

## 60. Average Intensity Projection

Average Intensity Projection SOLL unterstützt werden.

---

## 61. Projection Direction

Projektionsrichtung MUSS frei definierbar sein.

---

## 62. Slab Projection

Projektionen SOLLEN auf begrenzte Slab-Dicken beschränkt werden können.

---

## 63. Slab Thickness

Die Slab-Dicke MUSS in physikalischen Einheiten definierbar sein.

---

## 64. Volume Rendering Preparation

Radiology Compute SOLL Volumen für Volume Rendering vorbereiten.

---

## 65. Rendering Separation

Die eigentliche grafische Darstellung SOLL durch eine Graphics- beziehungsweise Rendering-Komponente erfolgen.

---

## 66. Transfer Function

Radiology Compute DARF Transfer Functions für Volumendarstellung erzeugen oder verwalten.

---

## 67. Intensity Window

Radiology Compute SOLL Intensitätsfenster definieren können.

---

## 68. Window Center

Window Center SOLL unterstützt werden.

---

## 69. Window Width

Window Width SOLL unterstützt werden.

---

## 70. Window Preset

Modalitätsspezifische Window Presets DÜRFEN bereitgestellt werden.

---

## 71. Window Preset Safety

Presets DÜRFEN quantitative Originalwerte nicht verändern.

---

## 72. Histogram

Radiology Compute MUSS Histogramme berechnen können.

---

## 73. Histogram Range

Der Histogrammbereich SOLL konfigurierbar sein.

---

## 74. Histogram Bins

Die Anzahl der Histogramm-Bins SOLL konfigurierbar sein.

---

## 75. ROI Histogram

Histogramme SOLLEN auf ROIs beziehungsweise Masken beschränkt werden können.

---

## 76. Intensity Statistics

Mindestens folgende Statistiken SOLLEN verfügbar sein:

```text
Minimum
Maximum
Mean
Median
StandardDeviation
Variance
Percentile
```

---

## 77. Quantile

Beliebige Quantile SOLLEN berechnet werden können.

---

## 78. ROI Statistics

Statistiken SOLLEN innerhalb definierter ROIs berechnet werden.

---

## 79. Voxel Count

Die Anzahl selektierter Voxel MUSS bestimmbar sein.

---

## 80. Physical Volume

Das physikalische Volumen einer 3D-Maske MUSS aus Voxelanzahl und Voxelgeometrie bestimmbar sein.

---

## 81. Area Measurement

Flächen SOLLEN in physikalischen Einheiten bestimmt werden können.

---

## 82. Distance Measurement

Abstände SOLLEN in physikalischen Einheiten bestimmt werden können.

---

## 83. Angle Measurement

Winkel SOLLEN bestimmt werden können.

---

## 84. Volume Measurement

Volumen SOLLEN in physikalischen Einheiten bestimmt werden können.

---

## 85. Measurement Object

Eine Messung wird als:

`RadiologyMeasurement`

repräsentiert.

---

## 86. Measurement Fields

Eine Messung SOLL mindestens enthalten können:

```text
MeasurementId
Type
Value
Unit
Geometry
Source
Algorithm
Timestamp
```

---

## 87. Measurement Units

Messwerte MÜSSEN ihre Einheit deklarieren.

---

## 88. Unit Conversion

Kompatible Einheiten SOLLEN kontrolliert konvertiert werden können.

---

## 89. Unit Safety

Inkompatible Einheiten DÜRFEN nicht stillschweigend miteinander verrechnet werden.

---

## 90. Image Filtering

Radiology Compute MUSS eine Filterarchitektur bereitstellen.

---

## 91. Filter Object

Ein Filter SOLL als:

`RadiologyFilter`

repräsentiert werden.

---

## 92. Gaussian Filter

Gaussian Filtering SOLL unterstützt werden.

---

## 93. Median Filter

Median Filtering SOLL unterstützt werden.

---

## 94. Bilateral Filter

Bilateral Filtering SOLL unterstützt werden können.

---

## 95. Box Filter

Box Filtering SOLL unterstützt werden können.

---

## 96. Anisotropic Diffusion

Anisotropic Diffusion SOLL als Erweiterung integrierbar sein.

---

## 97. Edge Detection

Kantenfilter SOLLEN unterstützt werden können.

---

## 98. Sobel

Sobel-artige Gradientenoperatoren SOLLEN unterstützt werden.

---

## 99. Laplacian

Laplacian-Filter SOLLEN unterstützt werden können.

---

## 100. Gradient Magnitude

Gradient Magnitude SOLL berechnet werden können.

---

## 101. Sharpening

Bildschärfung SOLL unterstützt werden können.

---

## 102. Noise Reduction

Rauschreduktionsverfahren SOLLEN integrierbar sein.

---

## 103. Denoising Separation

Rauschreduktion MUSS als abgeleitete Verarbeitung behandelt werden.

---

## 104. Original Preservation

Ein Filter DARF Originaldaten standardmäßig nicht überschreiben.

---

## 105. Filter Parameters

Alle relevanten Filterparameter MÜSSEN für Reproduzierbarkeit speicherbar sein.

---

## 106. Filter Pipeline

Mehrere Filter SOLLEN verkettet werden können.

Beispiel:

```text
Input
 |
 v
Denoise
 |
 v
Normalize
 |
 v
Edge Enhancement
 |
 v
Output
```

---

## 107. Pipeline Ordering

Die Reihenfolge der Filter MUSS explizit definiert sein.

---

## 108. Normalization

Radiology Compute SOLL Intensitätsnormalisierung unterstützen.

---

## 109. Min-Max Normalization

Min-Max-Normalisierung SOLL unterstützt werden können.

---

## 110. Z-Score Normalization

Z-Score-Normalisierung SOLL unterstützt werden können.

---

## 111. Percentile Normalization

Perzentilbasierte Normalisierung SOLL unterstützt werden können.

---

## 112. Modality-Aware Normalization

Modalitätsspezifische Normalisierungsverfahren SOLLEN integrierbar sein.

---

## 113. CT Quantitative Values

CT-Werte SOLLEN in ihrer kalibrierten quantitativen Repräsentation erhalten werden.

---

## 114. Hounsfield Unit

Für entsprechend kalibrierte CT-Daten SOLL die Einheit:

`HU`

repräsentierbar sein.

---

## 115. HU Preservation

Darstellungstransformationen DÜRFEN HU-Werte nicht zerstören.

---

## 116. MR Signal Values

MR-Signalwerte MÜSSEN von CT-HU-Semantik getrennt behandelt werden.

---

## 117. MR Scaling

Hersteller- oder sequenzspezifische MR-Skalierungen SOLLEN über explizite Metadaten beziehungsweise Transformationsmodelle verarbeitet werden.

---

## 118. PET Quantification

PET-Quantifizierung SOLL architektonisch unterstützt werden.

---

## 119. SUV

Standardized Uptake Values DÜRFEN berechnet werden, wenn alle dafür notwendigen Metadaten vorhanden und validiert sind.

---

## 120. SUV Validation

Fehlende oder inkonsistente Quantifizierungsparameter MÜSSEN vor einer SUV-Berechnung erkannt werden.

---

## 121. SPECT Quantification

Quantitative SPECT-Verarbeitung SOLL integrierbar sein.

---

## 122. Modality Model

Modalitätsspezifische Verarbeitung SOLL über austauschbare Modelle implementiert werden.

---

## 123. Modality Capability

Ein Modality Model SOLL seine unterstützten Operationen deklarieren.

---

## 124. Unknown Modality

Unbekannte Modalitäten DÜRFEN weiterhin als generische Bilddaten verarbeitet werden, sofern die Operation keine modalitätsspezifische Semantik voraussetzt.

---

## 125. Registration

Radiology Compute MUSS eine Bildregistrierungsarchitektur bereitstellen.

---

## 126. Registration Object

Eine Registrierung wird als:

`RadiologyRegistration`

repräsentiert.

---

## 127. Fixed Image

Das Referenzbild wird als:

`FixedImage`

bezeichnet.

---

## 128. Moving Image

Das zu transformierende Bild wird als:

`MovingImage`

bezeichnet.

---

## 129. Rigid Registration

Rigid Registration SOLL unterstützt werden.

---

## 130. Translation

Translation MUSS als Teil starrer Transformationen darstellbar sein.

---

## 131. Rotation

Rotation MUSS als Teil starrer Transformationen darstellbar sein.

---

## 132. Affine Registration

Affine Registration SOLL unterstützt werden können.

---

## 133. Deformable Registration

Deformable Registration SOLL architektonisch unterstützt werden.

---

## 134. Deformation Field

Eine nichtlineare Registrierung SOLL ein Deformationsfeld erzeugen können.

---

## 135. Displacement Field

Displacement Fields SOLLEN als Compute-Felder repräsentierbar sein.

---

## 136. Registration Metric

Registrierungsmetriken SOLLEN austauschbar sein.

---

## 137. Mean Squared Error

Mean Squared Error SOLL unterstützt werden können.

---

## 138. Cross Correlation

Cross Correlation SOLL unterstützt werden können.

---

## 139. Mutual Information

Mutual Information SOLL unterstützt werden können.

---

## 140. Normalized Mutual Information

Normalized Mutual Information SOLL unterstützt werden können.

---

## 141. Registration Optimizer

Optimierungsverfahren SOLLEN austauschbar sein.

---

## 142. Multi-Resolution Registration

Registrierung SOLL mehrstufig über Bildpyramiden ausgeführt werden können.

---

## 143. Image Pyramid

Eine Bildpyramide SOLL mehrere Auflösungsstufen enthalten.

---

## 144. Registration Mask

Registrierungen SOLLEN auf definierte Masken beschränkt werden können.

---

## 145. Registration Quality

Eine Registrierung SOLL Qualitätsmetriken bereitstellen können.

---

## 146. Registration Validation

Registrierungsergebnisse SOLLEN vor Verwendung validierbar sein.

---

## 147. Transform Provenance

Registrierungstransformationen MÜSSEN ihre Herkunft und Parameter speichern können.

---

## 148. Multi-Modality Registration

Registrierung zwischen unterschiedlichen Modalitäten SOLL unterstützt werden können.

Beispiele:

```text
CT <-> MR
CT <-> PET
MR <-> PET
CT <-> SPECT
```

---

## 149. Image Fusion

Registrierte Modalitäten SOLLEN für gemeinsame Analyse vorbereitet werden können.

---

## 150. Fusion Object

Eine Bildfusion SOLL als:

`RadiologyFusion`

repräsentiert werden.

---

## 151. Fusion Alignment

Eine Fusion DARF nur bei definierter räumlicher Beziehung zwischen den Datensätzen erfolgen.

---

## 152. Segmentation

Radiology Compute MUSS eine Segmentierungsarchitektur bereitstellen.

---

## 153. Segmentation Object

Eine Segmentierung wird als:

`RadiologySegmentation`

repräsentiert.

---

## 154. Binary Segmentation

Binäre Segmentierung MUSS unterstützt werden können.

---

## 155. Multi-Label Segmentation

Mehrere Label MÜSSEN unterstützt werden können.

---

## 156. Probability Map

Probabilistische Segmentierung SOLL unterstützt werden können.

---

## 157. Fractional Mask

Fractional Masks SOLLEN unterstützt werden können.

---

## 158. Segmentation Label

Jedes Label SOLL mindestens besitzen können:

```text
LabelId
Name
Description
Value
Source
```

---

## 159. Manual Segmentation

Manuell erzeugte Segmentierungen SOLLEN repräsentierbar sein.

---

## 160. Semi-Automatic Segmentation

Semi-automatische Segmentierungsverfahren SOLLEN unterstützt werden können.

---

## 161. Automatic Segmentation

Automatische Segmentierung SOLL integrierbar sein.

---

## 162. Threshold Segmentation

Threshold-basierte Segmentierung SOLL unterstützt werden.

---

## 163. Region Growing

Region-Growing-Verfahren SOLLEN unterstützt werden können.

---

## 164. Connected Components

Connected-Component-Analyse SOLL unterstützt werden.

---

## 165. Watershed

Watershed-Segmentierung SOLL integrierbar sein.

---

## 166. Graph-Based Segmentation

Graphbasierte Verfahren DÜRFEN integrierbar sein.

---

## 167. AI Segmentation

KI-basierte Segmentierungsmodelle SOLLEN über NPU-, GPU- oder CPU-Backends integrierbar sein.

---

## 168. AI Model Separation

Ein KI-Modell DARF nicht Teil des grundlegenden Radiology-Objektmodells sein.

---

## 169. Segmentation Confidence

Automatische Segmentierungen SOLLEN Konfidenzinformationen bereitstellen können.

---

## 170. Segmentation Source

Eine Segmentierung MUSS unterscheiden können zwischen:

```text
Manual
SemiAutomatic
Automatic
Imported
```

---

## 171. Segmentation Provenance

Automatische Segmentierungen SOLLEN Modellversion und Verarbeitungsparameter speichern.

---

## 172. Segmentation Geometry

Segmentierungen MÜSSEN eindeutig einem räumlichen Referenzsystem zugeordnet werden.

---

## 173. Mask Resampling

Masken SOLLEN zwischen registrierten Geometrien übertragen werden können.

---

## 174. Morphological Operations

Radiology Compute SOLL morphologische Operationen unterstützen.

---

## 175. Erosion

Erosion SOLL unterstützt werden.

---

## 176. Dilation

Dilation SOLL unterstützt werden.

---

## 177. Opening

Opening SOLL unterstützt werden.

---

## 178. Closing

Closing SOLL unterstützt werden.

---

## 179. Hole Filling

Hole Filling SOLL unterstützt werden können.

---

## 180. Connected Component Filtering

Komponenten SOLLEN anhand von Größe oder anderen Kriterien gefiltert werden können.

---

## 181. Distance Transform

Distance Transform SOLL unterstützt werden können.

---

## 182. Signed Distance Field

Signed Distance Fields SOLLEN erzeugbar sein.

---

## 183. Surface Extraction

Aus Segmentierungen SOLLEN Oberflächen extrahiert werden können.

---

## 184. Marching Cubes

Marching-Cubes-artige Verfahren SOLLEN unterstützt werden können.

---

## 185. Surface Mesh

Eine extrahierte Oberfläche SOLL als Mesh-Objekt an andere Compute- beziehungsweise Graphics-Komponenten übergeben werden können.

---

## 186. Surface Area

Die Oberfläche eines Segmentierungsobjekts SOLL bestimmt werden können.

---

## 187. Shape Analysis

Geometrische Formmerkmale SOLLEN berechnet werden können.

---

## 188. Bounding Box

Eine Axis-Aligned Bounding Box SOLL bestimmbar sein.

---

## 189. Oriented Bounding Box

Eine orientierte Bounding Box SOLL bestimmbar sein.

---

## 190. Centroid

Der geometrische Schwerpunkt einer Region SOLL bestimmbar sein.

---

## 191. Principal Axes

Hauptachsen einer Region SOLLEN bestimmbar sein.

---

## 192. Diameter

Maximale beziehungsweise definierte Durchmesser SOLLEN berechnet werden können.

---

## 193. Time Series

Radiology Compute SOLL zeitabhängige Bilddaten unterstützen.

---

## 194. Temporal Index

Frames oder Volumen SOLLEN einen zeitlichen Index besitzen.

---

## 195. Temporal Position

Physikalische oder relative Zeitinformationen SOLLEN erhalten bleiben.

---

## 196. Temporal Resampling

Zeitreihen SOLLEN zeitlich resampled werden können.

---

## 197. Motion Analysis

Bewegungen zwischen Zeitpunkten SOLLEN analysiert werden können.

---

## 198. Motion Field

Bewegung SOLL als Vektor- oder Deformationsfeld repräsentiert werden können.

---

## 199. Dynamic Imaging

Dynamische CT-, MR-, PET- oder andere Zeitserien SOLLEN verarbeitbar sein.

---

## 200. Time-Intensity Curve

Zeit-Intensitäts-Kurven SOLLEN aus ROIs extrahiert werden können.

---

## 201. Curve Object

Eine quantitative Zeitkurve SOLL als:

`RadiologyCurve`

repräsentiert werden.

---

## 202. Curve Samples

Ein Kurvenpunkt SOLL mindestens enthalten:

```text
Time
Value
Unit
```

---

## 203. Temporal Statistics

Zeitabhängige Statistiken SOLLEN berechnet werden können.

---

## 204. Subtraction Imaging

Subtraktionsverfahren SOLLEN unterstützt werden können.

---

## 205. Baseline

Ein Referenzzeitpunkt MUSS explizit definierbar sein.

---

## 206. Subtraction Registration

Vor Subtraktion SOLL eine Registrierung möglich sein.

---

## 207. Angiography

Radiology Compute SOLL angiographische Verarbeitung integrieren können.

---

## 208. Digital Subtraction Angiography

Digital Subtraction Angiography SOLL architektonisch unterstützt werden.

Abkürzung:

`DSA`

---

## 209. Vessel Enhancement

Gefäßverstärkende Filter SOLLEN integrierbar sein.

---

## 210. Centerline

Gefäß- oder Struktur-Centerlines SOLLEN berechnet werden können.

---

## 211. Centerline Object

Eine Centerline SOLL als geordnete räumliche Punktfolge repräsentiert werden.

---

## 212. Curved Reconstruction

Centerlines SOLLEN als Grundlage für CPR verwendet werden können.

---

## 213. Mammography

Radiology Compute SOLL hochauflösende Mammographie-Bilddaten unterstützen können.

---

## 214. Large 2D Images

Sehr große 2D-Bilder SOLLEN tile-basiert verarbeitet werden können.

---

## 215. Ultrasound

Radiology Compute SOLL Ultraschallbilddaten verarbeiten können.

---

## 216. Ultrasound Frames

2D- und Multiframe-Ultraschalldaten SOLLEN unterstützt werden können.

---

## 217. Ultrasound Geometry

Spezielle Ultraschallgeometrien SOLLEN durch Erweiterungsmodule repräsentiert werden können.

---

## 218. Nuclear Medicine

PET- und SPECT-Daten SOLLEN mit quantitativen Metadaten verarbeitet werden können.

---

## 219. Radiomics

Radiology Compute SOLL eine Architektur für Radiomics bereitstellen können.

---

## 220. Radiomics Feature

Ein extrahiertes Merkmal wird als:

`RadiomicsFeature`

repräsentiert.

---

## 221. First-Order Features

Intensitätsbasierte First-Order-Features SOLLEN berechnet werden können.

---

## 222. Shape Features

Formmerkmale SOLLEN berechnet werden können.

---

## 223. Texture Features

Texturmerkmale SOLLEN integrierbar sein.

---

## 224. Feature Configuration

Radiomics-Ergebnisse MÜSSEN die verwendeten Parameter dokumentieren können.

---

## 225. Discretization

Intensitätsdiskretisierung MUSS bei entsprechenden Features explizit konfiguriert werden.

---

## 226. Radiomics Reproducibility

Radiomics-Pipelines SOLLEN deterministisch und reproduzierbar konfigurierbar sein.

---

## 227. AI Integration

Radiology Compute SOLL KI-Modelle als optionale Compute-Komponenten integrieren können.

---

## 228. AI Tasks

Mögliche Aufgaben umfassen:

```text
Segmentation
Classification
Detection
Registration
Denoising
Reconstruction
SuperResolution
FeatureExtraction
```

---

## 229. AI Output

KI-Ausgaben MÜSSEN als abgeleitete Ergebnisse gekennzeichnet werden.

---

## 230. AI Confidence

Wenn ein Modell Konfidenzwerte bereitstellt, SOLLEN diese erhalten bleiben.

---

## 231. AI Model Identity

Ein KI-Ergebnis SOLL mindestens referenzieren können:

```text
ModelId
ModelVersion
Configuration
InputReference
```

---

## 232. AI Preprocessing

Vorverarbeitungsschritte MÜSSEN dokumentierbar sein.

---

## 233. AI Postprocessing

Nachverarbeitungsschritte MÜSSEN dokumentierbar sein.

---

## 234. AI Spatial Integrity

KI-Pipelines MÜSSEN räumliche Transformationen nachvollziehbar halten.

---

## 235. AI Result Mapping

KI-Ergebnisse MÜSSEN korrekt in den ursprünglichen Bildraum zurücktransformiert werden können.

---

## 236. CPU Backend

Radiology Compute MUSS CPU-Ausführung unterstützen.

---

## 237. Multi-Core CPU

Unabhängige Pixel, Voxel, Slices, Frames, Tiles oder Regionen SOLLEN parallel verarbeitet werden können.

---

## 238. SIMD

Pixel- und Voxeloperationen SOLLEN SIMD-Vektorisierung unterstützen können.

---

## 239. GPU Backend

Radiology Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` verwenden können.

---

## 240. GPU Operations

Geeignete Operationen umfassen:

```text
Filtering
Resampling
MPR
MIP
Registration
Segmentation
Morphology
Volume Processing
AI Preprocessing
```

---

## 241. GPU Memory

Große Volumen SOLLEN unter Berücksichtigung des verfügbaren GPU-Speichers verarbeitet werden.

---

## 242. Chunked GPU Processing

Volumen, die nicht vollständig in GPU-Speicher passen, SOLLEN blockweise verarbeitet werden können.

---

## 243. Multi-GPU

Sehr große radiologische Workloads SOLLEN auf mehrere GPUs verteilt werden können.

---

## 244. NPU Runtime

Radiology Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0006` für geeignete KI-Operationen verwenden können.

---

## 245. NPU Optionality

Die grundlegende radiologische Bildverarbeitung DARF keine NPU voraussetzen.

---

## 246. Distributed Compute

Radiology Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0007` verwenden können.

---

## 247. Distributed Workloads

Geeignete verteilte Workloads umfassen:

```text
Study Batch Processing
Series Processing
Large Volume Processing
Radiomics
AI Inference
Registration Batches
```

---

## 248. HPC

Radiology Compute SOLL `NPSPEC-COMPUTE-HPC-0001` integrieren können.

---

## 249. Memory Model

Radiology Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 250. Memory Objects

Mindestens folgende Speicherobjekte SOLLEN darstellbar sein:

```text
ImageBuffer
VolumeBuffer
MaskBuffer
LabelBuffer
TransformBuffer
VectorFieldBuffer
ProbabilityBuffer
TemporaryBuffer
```

---

## 251. Data Type

Radiology Compute SOLL mindestens unterstützen können:

```text
UInt8
Int8
UInt16
Int16
UInt32
Int32
Float16
Float32
Float64
```

soweit vom jeweiligen Backend unterstützt.

---

## 252. Source Precision

Die ursprüngliche Datenpräzision SOLL nachvollziehbar bleiben.

---

## 253. Processing Precision

Die für eine Operation verwendete Präzision MUSS getrennt von der Source Precision definierbar sein.

---

## 254. Precision Conversion

Datentypkonvertierungen MÜSSEN explizit und kontrolliert erfolgen.

---

## 255. Overflow Protection

Integeroperationen MÜSSEN gegen unerwarteten Overflow geschützt werden.

---

## 256. NaN Handling

Floating-Point-Pipelines MÜSSEN eine definierte NaN-Policy besitzen.

---

## 257. Infinity Handling

Infinity-Werte MÜSSEN kontrolliert behandelt werden.

---

## 258. Memory Budget

Radiology Compute SOLL Speicherbudgets unterstützen.

---

## 259. Memory Estimation

Der erwartete Speicherbedarf einer Operation SOLL vorab abschätzbar sein.

---

## 260. Out-of-Memory

Speicherfehler MÜSSEN strukturiert behandelt werden.

---

## 261. Lazy Evaluation

Große Verarbeitungsketten SOLLEN Lazy Evaluation unterstützen können.

---

## 262. Streaming

Große Bilddaten SOLLEN streamingfähig verarbeitet werden.

---

## 263. Tile Processing

2D-Bilder SOLLEN tile-basiert verarbeitet werden.

---

## 264. Chunk Processing

3D-Volumen SOLLEN chunk-basiert verarbeitet werden.

---

## 265. Halo Region

Filteroperationen auf Chunks SOLLEN Halo- beziehungsweise Overlap-Regionen verwenden können.

---

## 266. Seam Protection

Chunk-Verarbeitung DARF keine künstlichen Grenzen erzeugen, wenn der Algorithmus eine Nachbarschaft über Chunk-Grenzen benötigt.

---

## 267. Scheduler Integration

Radiology Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 268. Task Model

Eine Pipeline SOLL in Compute Tasks zerlegt werden.

Beispiel:

```text
Load
 |
 v
Resample
 |
 v
Register
 |
 v
Segment
 |
 v
Measure
 |
 v
Export
```

---

## 269. Task Dependencies

Abhängigkeiten zwischen Tasks MÜSSEN explizit darstellbar sein.

---

## 270. Parallel Tasks

Unabhängige Operationen SOLLEN parallel ausgeführt werden können.

---

## 271. Cancellation

Radiology-Compute-Tasks MÜSSEN kontrolliert abbrechbar sein.

---

## 272. Cancellation Safety

Ein Abbruch DARF keine unvollständigen Ergebnisse als vollständig markieren.

---

## 273. Progress

Lange Operationen SOLLEN Fortschritt melden.

---

## 274. Progress Fields

Fortschrittsinformationen SOLLEN mindestens enthalten können:

```text
Operation
CompletedWork
TotalWork
Percentage
EstimatedRemainingWork
```

---

## 275. Pipeline Object

Eine Verarbeitungspipeline wird als:

`RadiologyPipeline`

repräsentiert.

---

## 276. Pipeline Stage

Ein Verarbeitungsschritt wird als:

`RadiologyPipelineStage`

repräsentiert.

---

## 277. Pipeline Configuration

Alle relevanten Parameter MÜSSEN serialisierbar sein.

---

## 278. Pipeline Hash

Eine Pipeline-Konfiguration SOLL über einen stabilen Hash identifizierbar sein.

---

## 279. Provenance

Radiology Compute MUSS Provenance unterstützen.

---

## 280. Provenance Record

Ein Provenance-Eintrag SOLL mindestens enthalten:

```text
SourceId
OperationId
AlgorithmId
AlgorithmVersion
Parameters
RuntimeVersion
Backend
Precision
Timestamp
```

---

## 281. Derived Object

Jedes abgeleitete Objekt SOLL seine Provenance referenzieren.

---

## 282. Processing Graph

Die Herkunft eines Ergebnisses SOLL als gerichteter Graph darstellbar sein.

Beispiel:

```text
Original CT
    |
    v
Resample
    |
    v
Denoise
    |
    v
Segmentation
    |
    +--> Volume Measurement
    |
    +--> Surface Mesh
```

---

## 283. Reproducibility

Eine Pipeline SOLL bei identischer Konfiguration reproduzierbar ausführbar sein.

---

## 284. Deterministic Mode

Radiology Compute SOLL einen deterministischen Modus unterstützen.

---

## 285. Random Seed

Stochastische Algorithmen MÜSSEN im deterministischen Modus einen definierbaren Seed verwenden können.

---

## 286. Backend Differences

Numerische Unterschiede zwischen CPU-, GPU- und NPU-Backends MÜSSEN über definierte Toleranzen berücksichtigt werden.

---

## 287. Validation

Radiology Compute MUSS Eingaben und Ergebnisse validieren können.

---

## 288. Geometry Validation

Vor räumlichen Operationen MUSS die Bildgeometrie validiert werden.

---

## 289. Data Validation

Ungültige Dimensions-, Spacing- oder Datentypinformationen MÜSSEN erkannt werden.

---

## 290. Operation Validation

Eine Operation MUSS prüfen, ob ihre Eingabedaten kompatibel sind.

---

## 291. Registration Validation

Registrierungen SOLLEN auf Plausibilität und mathematische Gültigkeit geprüft werden.

---

## 292. Segmentation Validation

Segmentierungen SOLLEN auf gültige Label und Geometrie geprüft werden.

---

## 293. Measurement Validation

Messungen DÜRFEN nur aus gültiger Geometrie abgeleitet werden.

---

## 294. Quantification Validation

Quantitative modalitätsspezifische Berechnungen MÜSSEN erforderliche Metadaten validieren.

---

## 295. Diagnostics

Radiology Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 296. Diagnostic Classes

Mindestens folgende Klassen SOLLEN existieren:

```text
INVALID_IMAGE
INVALID_VOLUME
INVALID_GEOMETRY
INVALID_SPACING
INVALID_ORIENTATION
INVALID_TRANSFORM
INCOMPATIBLE_GEOMETRY
INVALID_MASK
INVALID_LABEL
INVALID_MEASUREMENT
INVALID_UNIT
INVALID_PIPELINE
REGISTRATION_FAILED
REGISTRATION_NOT_CONVERGED
SEGMENTATION_FAILED
RESAMPLING_FAILED
QUANTIFICATION_METADATA_MISSING
NUMERICAL_ERROR
NAN_DETECTED
INFINITY_DETECTED
OUT_OF_MEMORY
DEVICE_FAILURE
CANCELLED
```

---

## 297. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 298. Privacy-Aware Diagnostics

Diagnosen SOLLEN keine unnötigen patientenbezogenen Informationen enthalten.

---

## 299. Diagnostic Object Reference

Eine Diagnose SOLL das betroffene Compute-Objekt über technische IDs referenzieren können.

---

## 300. Profiling

Radiology Compute MUSS Profiling unterstützen.

---

## 301. Profiling Categories

Mindestens:

```text
Input
Resampling
Filtering
MPR
Projection
Registration
Segmentation
Morphology
Measurement
Radiomics
AI
Output
```

---

## 302. Throughput

Geeignete Metriken umfassen:

```text
PixelsPerSecond
VoxelsPerSecond
FramesPerSecond
VolumesPerSecond
```

---

## 303. Memory Profiling

Peak Memory und temporärer Speicherbedarf SOLLEN messbar sein.

---

## 304. GPU Profiling

GPU-Kernelzeit und Transferzeit SOLLEN getrennt messbar sein.

---

## 305. NPU Profiling

NPU-Inferenzzeit SOLL messbar sein.

---

## 306. Distributed Profiling

Kommunikations- und Compute-Zeit SOLLEN getrennt messbar sein.

---

## 307. Checkpointing

Lange Verarbeitungspipelines SOLLEN Checkpointing unterstützen.

---

## 308. Pipeline Checkpoint

Ein Checkpoint SOLL mindestens enthalten:

```text
PipelineId
ConfigurationHash
CompletedStages
PendingStages
SourceFingerprint
```

---

## 309. Intermediate Results

Zwischenergebnisse DÜRFEN Bestandteil eines Checkpoints sein.

---

## 310. Restart

Eine kompatible Pipeline SOLL aus einem Checkpoint fortgesetzt werden können.

---

## 311. Source Validation on Restart

Vor Restart MUSS geprüft werden können, ob die Quelldaten unverändert sind.

---

## 312. Cache

Radiology Compute SOLL einen Cache für aufwendige Zwischenergebnisse verwenden können.

---

## 313. Cache Key

Ein Cache Key SOLL mindestens berücksichtigen:

```text
SourceFingerprint
Operation
Parameters
AlgorithmVersion
```

---

## 314. Cache Invalidation

Änderungen relevanter Eingaben oder Parameter MÜSSEN den Cache invalidieren.

---

## 315. Testing — Geometry

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
IdentityGeometry
AnisotropicSpacing
ObliqueOrientation
CoordinateTransform
InverseTransform
Reorientation
```

---

## 316. Testing — Resampling

Mindestens:

```text
NearestNeighbor
Linear
IsotropicResampling
LabelResampling
BoundaryHandling
```

MÜSSEN getestet werden.

---

## 317. Testing — MPR

Mindestens:

```text
Axial
Coronal
Sagittal
Oblique
```

SOLLTEN getestet werden.

---

## 318. Testing — Projection

Mindestens:

```text
MIP
MinIP
AverageProjection
SlabProjection
```

soweit implementiert.

---

## 319. Testing — Filtering

Mindestens:

```text
Gaussian
Median
Gradient
```

SOLLTEN gegen Referenzdaten geprüft werden.

---

## 320. Testing — Registration

Mindestens:

```text
Identity
Translation
Rotation
RigidRegistration
KnownTransformRecovery
```

MÜSSEN für implementierte Registrierungsverfahren getestet werden.

---

## 321. Testing — Segmentation

Mindestens:

```text
BinaryMask
MultiLabel
Threshold
ConnectedComponents
Morphology
```

MÜSSEN getestet werden.

---

## 322. Testing — Measurement

Mindestens:

```text
Distance
Area
Volume
Centroid
VoxelCount
```

MÜSSEN gegen analytisch bekannte Geometrien getestet werden.

---

## 323. Testing — Units

Physikalische Einheiten MÜSSEN auf korrekte Konvertierung geprüft werden.

---

## 324. Testing — Quantitative Values

Quantitative Werte DÜRFEN durch reine Darstellungsoperationen nicht verändert werden.

---

## 325. Testing — CPU

Alle Mindestfunktionen MÜSSEN auf CPU getestet werden.

---

## 326. Testing — GPU

GPU-Ergebnisse SOLLEN innerhalb definierter Toleranzen mit CPU-Referenzen übereinstimmen.

---

## 327. Testing — NPU

KI-Backend-Integration SOLL hinsichtlich Ein- und Ausgabetransformationen getestet werden.

---

## 328. Testing — Determinism

Deterministische Pipelines MÜSSEN reproduzierbar sein.

---

## 329. Testing — Cancellation

Abgebrochene Pipelines DÜRFEN keine Ergebnisse fälschlich als vollständig markieren.

---

## 330. Testing — Large Data

Große 3D- und 4D-Datensätze SOLLEN getestet werden.

---

## 331. Stress Tests

Mindestens:

```text
LargeVolume
LargeTimeSeries
HighResolution2D
ManyROIs
ManySegments
RepeatedResampling
ParallelFiltering
RegistrationBatch
MemoryPressure
GPUFallback
Cancellation
```

SOLLTEN getestet werden.

---

## 332. Reference Phantom

Validierung SOLL geeignete mathematische oder physikalische Phantome verwenden können.

---

## 333. Analytical Geometry

Messfunktionen SOLLEN gegen analytisch bekannte Formen getestet werden.

Beispiele:

```text
Sphere
Cube
Cylinder
Plane
Line
```

---

## 334. Backend Comparison

CPU-, GPU- und andere Backend-Ergebnisse SOLLEN innerhalb definierter Toleranzen vergleichbar sein.

---

## 335. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
RadiologyImage
RadiologyVolume

2D
3D

Spatial Metadata
Voxel Coordinates
Physical Coordinates

Resampling
Nearest Neighbor
Linear Interpolation

Reorientation

Axial MPR
Coronal MPR
Sagittal MPR
Oblique MPR

MIP

ROI
Mask

Histogram
Minimum
Maximum
Mean
Standard Deviation

Distance Measurement
Area Measurement
Volume Measurement

Gaussian Filter
Median Filter

Binary Segmentation
Multi-Label Segmentation
Threshold Segmentation
Connected Components

Erosion
Dilation

Rigid Transform
Registration Architecture

CPU Execution
Multi-Core Architecture

DICOM Integration

Compute Memory Model
Compute Scheduler
Compute Object Model

Diagnostics
Profiling
Provenance
Cancellation
```

---

## 336. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
4D Imaging

CPR
MinIP
Average Projection
Slab Projection

Advanced Denoising
Anisotropic Diffusion

Affine Registration
Deformable Registration
Multi-Modality Registration

Probability Segmentation
AI Segmentation

Surface Extraction
Marching Cubes
Shape Analysis

Temporal Analysis
Motion Fields
Subtraction Imaging
DSA

PET Quantification
SPECT Quantification

Radiomics

GPU
Multi-GPU
NPU
HPC
Distributed Compute

Streaming
Chunk Processing
Checkpointing
```

---

## 337. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RADIOLOGY-0001` MUSS:

1. radiologische 2D-Bilder repräsentieren können.
2. radiologische 3D-Volumen repräsentieren können.
3. DICOM Compute integrieren.
4. Quelldaten und abgeleitete Daten unterscheiden.
5. Originalwerte erhalten können.
6. Darstellungswerte von quantitativen Werten trennen.
7. Bilddimensionen repräsentieren.
8. Voxel Spacing repräsentieren.
9. Origin repräsentieren.
10. Orientation repräsentieren.
11. Voxelkoordinaten in physikalische Koordinaten transformieren.
12. inverse Transformationen ermöglichen.
13. Bildgeometrie validieren.
14. Resampling unterstützen.
15. Nearest-Neighbor-Interpolation unterstützen.
16. lineare beziehungsweise trilineare Interpolation unterstützen.
17. labelgeeignete Interpolation ermöglichen.
18. Reorientation unterstützen.
19. MPR unterstützen.
20. axiale MPR unterstützen.
21. koronale MPR unterstützen.
22. sagittale MPR unterstützen.
23. oblique MPR unterstützen.
24. MIP unterstützen.
25. ROIs repräsentieren.
26. Masken repräsentieren.
27. Histogramme berechnen.
28. Minimum bestimmen.
29. Maximum bestimmen.
30. Mittelwert bestimmen.
31. Standardabweichung bestimmen.
32. Voxelanzahl bestimmen.
33. physikalische Abstände messen.
34. physikalische Flächen bestimmen.
35. physikalische Volumen bestimmen.
36. Messwerte mit Einheiten versehen.
37. inkompatible Einheiten erkennen.
38. Filterarchitektur bereitstellen.
39. Gaussian Filtering unterstützen.
40. Median Filtering unterstützen.
41. Filterparameter speichern können.
42. Originaldaten bei Filterung standardmäßig erhalten.
43. Segmentierungen repräsentieren.
44. binäre Segmentierungen unterstützen.
45. Multi-Label-Segmentierungen unterstützen.
46. Threshold-Segmentierung unterstützen.
47. Connected Components unterstützen.
48. Erosion unterstützen.
49. Dilation unterstützen.
50. Segmentierungsgeometrie erhalten.
51. Registrierung architektonisch unterstützen.
52. starre Transformationen repräsentieren.
53. Translation repräsentieren.
54. Rotation repräsentieren.
55. Registrierungsergebnisse validierbar machen.
56. Transformations-Provenance unterstützen.
57. CPU-Ausführung unterstützen.
58. Multi-Core-Ausführung architektonisch unterstützen.
59. SIMD-Ausführung architektonisch unterstützen.
60. GPU-Ausführung architektonisch unterstützen.
61. NPU-Ausführung architektonisch unterstützen.
62. Distributed Compute architektonisch unterstützen.
63. das Compute Memory Model integrieren.
64. den Compute Scheduler integrieren.
65. das Compute Object Model integrieren.
66. Speicherbudgets unterstützen können.
67. große Daten streamingfähig verarbeiten können.
68. Tile Processing architektonisch unterstützen.
69. Chunk Processing architektonisch unterstützen.
70. kontrollierten Abbruch unterstützen.
71. unvollständige Ergebnisse eindeutig kennzeichnen.
72. strukturierte Diagnosen bereitstellen.
73. NaN-Werte erkennen.
74. Infinity-Werte erkennen.
75. ungültige Geometrien erkennen.
76. Provenance für abgeleitete Ergebnisse bereitstellen.
77. Pipeline-Konfigurationen serialisieren können.
78. deterministische Verarbeitung architektonisch unterstützen.
79. backendabhängige numerische Toleranzen unterstützen.
80. Erweiterungen ohne Änderung des grundlegenden Radiology-Objektmodells ermöglichen.

---

## 338. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaRadiologyCompute
 |
 +-- RadiologyRuntime
 |
 +-- RadiologyDataModel
 |
 +-- GeometryEngine
 |
 +-- ResamplingEngine
 |
 +-- ReconstructionEngine
 |
 +-- ProjectionEngine
 |
 +-- FilteringEngine
 |
 +-- RegistrationEngine
 |
 +-- SegmentationEngine
 |
 +-- MorphologyEngine
 |
 +-- MeasurementEngine
 |
 +-- QuantificationEngine
 |
 +-- TemporalEngine
 |
 +-- RadiomicsEngine
 |
 +-- AIIntegration
 |
 +-- PipelineManager
 |
 +-- ProvenanceManager
 |
 +-- RadiologyDiagnostics
 |
 +-- RadiologyProfiler
```

Diese Struktur ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 339. Beziehung zu `NPSPEC-COMPUTE-DICOM-0001`

DICOM Compute stellt die standardkonforme medizinische Datenrepräsentation bereit.

Radiology Compute baut darauf auf:

```text
DICOM File
    |
    v
DICOM Compute
    |
    +--> Metadata
    +--> Pixel Data
    +--> Geometry
    +--> Series
    +--> Volume
            |
            v
     Radiology Compute
            |
            +--> Reconstruction
            +--> Filtering
            +--> Registration
            +--> Segmentation
            +--> Measurement
            +--> Quantification
```

---

## 340. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die allgemeine Compute Runtime stellt Hardwareabstraktion und Ausführungsressourcen bereit.

```text
Radiology Pipeline
       |
       v
Radiology Compute
       |
       v
Compute Runtime
       |
 +-----+-----+-----+-----+
 |           |     |     |
CPU         GPU   NPU   Distributed
```

---

## 341. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler verwaltet die Ausführung radiologischer Compute Tasks.

---

## 342. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Bild-, Volumen-, Masken-, Transformations- und temporäre Puffer werden über das gemeinsame Compute Memory Model verwaltet.

---

## 343. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Radiologische Compute-Objekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 344. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

GPU Runtime SOLL massiv parallele Bild- und Volumenoperationen beschleunigen.

---

## 345. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

NPU Runtime SOLL für geeignete KI-Modelle verwendet werden können.

---

## 346. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Distributed Compute SOLL große radiologische Datenbestände und Batch-Pipelines verteilen können.

---

## 347. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

HPC Compute SOLL für sehr große Bildverarbeitungs-, Registrierungs-, Radiomics- und KI-Workloads verwendet werden können.

---

## 348. Grundlegender Verarbeitungsablauf

Der konzeptionelle Ablauf lautet:

```text
Source Data
    |
    v
DICOM Compute
    |
    v
Radiology Object
    |
    v
Geometry Validation
    |
    v
Preprocessing
    |
    +-------------------+
    |                   |
    v                   v
Registration       Reconstruction
    |                   |
    +---------+---------+
              |
              v
        Segmentation
              |
              v
        Quantification
              |
              v
         Measurement
              |
              v
       Derived Results
```

Nicht jede Pipeline MUSS alle Schritte enthalten.

---

## 349. Datenintegritätsmodell

Die grundlegende Trennung lautet:

```text
Original Data
     |
     +-------------------------+
     |                         |
     v                         v
Display Pipeline         Compute Pipeline
                               |
                               v
                         Derived Data
```

Darstellungs- und Compute-Pipelines DÜRFEN die Originaldaten nicht implizit überschreiben.

---

## 350. Registrierungsmodell

```text
Fixed Image
     |
     +----------------+
                      |
Moving Image          |
     |                |
     v                v
   Transform Optimization
             |
             v
       Registration
             |
             v
     Registered Image
```

---

## 351. Segmentierungsmodell

```text
Radiology Image / Volume
          |
          v
    Segmentation
          |
     +----+----+
     |         |
     v         v
   Mask     Probability
     |
     v
Measurement / Surface / Analysis
```

---

## 352. KI-Integrationsmodell

```text
Radiology Data
      |
      v
Preprocessing
      |
      v
Tensor
      |
      v
CPU / GPU / NPU
      |
      v
AI Model
      |
      v
Model Output
      |
      v
Spatial Mapping
      |
      v
Radiology Result
```

---

## 353. Performance-Grundsatz

Radiology Compute SOLL Operationen möglichst datenlokal ausführen.

Bevorzugt:

```text
Load Required Region
        |
        v
Process
        |
        v
Reuse Buffer
        |
        v
Publish Result
```

anstatt:

```text
Copy Full Volume
      |
      v
Copy Again
      |
      v
Process
      |
      v
Copy Again
```

---

## 354. Numerischer Grundsatz

Performanceoptimierungen DÜRFEN räumliche Geometrie, quantitative Werte oder definierte numerische Semantik nicht unkontrolliert verändern.

---

## 355. Nachvollziehbarkeitsgrundsatz

Für jedes klinisch oder wissenschaftlich relevantes abgeleitetes Ergebnis SOLL nachvollziehbar sein:

```text
Welche Quelldaten?
Welche Operation?
Welcher Algorithmus?
Welche Version?
Welche Parameter?
Welche Präzision?
Welches Backend?
```

---

## 356. Erweiterbarkeit

Neue:

```text
Modalities
Filters
Registration Methods
Segmentation Methods
Measurements
Quantification Models
Radiomics Features
AI Models
Reconstruction Methods
```

SOLLTEN ohne Änderung des grundlegenden Radiology-Compute-Modells integrierbar sein.

---

## 357. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-RADIOLOGY-GEOMETRY-0001
Radiology Geometry

NPSPEC-COMPUTE-RADIOLOGY-RESAMPLE-0001
Radiology Resampling

NPSPEC-COMPUTE-RADIOLOGY-MPR-0001
Multiplanar Reconstruction

NPSPEC-COMPUTE-RADIOLOGY-PROJECTION-0001
MIP, MinIP and Projection Compute

NPSPEC-COMPUTE-RADIOLOGY-FILTER-0001
Radiology Image Filtering

NPSPEC-COMPUTE-RADIOLOGY-REGISTRATION-0001
Medical Image Registration

NPSPEC-COMPUTE-RADIOLOGY-SEGMENTATION-0001
Medical Image Segmentation

NPSPEC-COMPUTE-RADIOLOGY-MORPHOLOGY-0001
Medical Image Morphology

NPSPEC-COMPUTE-RADIOLOGY-MEASUREMENT-0001
Radiology Measurement

NPSPEC-COMPUTE-RADIOLOGY-QUANT-0001
Radiology Quantification

NPSPEC-COMPUTE-RADIOLOGY-TEMPORAL-0001
Dynamic and 4D Imaging

NPSPEC-COMPUTE-RADIOLOGY-FUSION-0001
Multi-Modality Image Fusion

NPSPEC-COMPUTE-RADIOLOGY-RADIOMICS-0001
Radiomics Compute

NPSPEC-COMPUTE-RADIOLOGY-AI-0001
Radiology AI Integration

NPSPEC-COMPUTE-RADIOLOGY-GPU-0001
Radiology GPU Backend

NPSPEC-COMPUTE-RADIOLOGY-DIAG-0001
Radiology Diagnostics

NPSPEC-COMPUTE-RADIOLOGY-TEST-0001
Radiology Conformance Tests
```

---

## 358. Zusammenfassung

`NPSPEC-COMPUTE-RADIOLOGY-0001` definiert die gemeinsame radiologische Bildverarbeitungsinfrastruktur der NovaOS-Compute-Architektur.

Die grundlegende Schichtung lautet:

```text
Medical Imaging Data
        |
        v
   DICOM Compute
        |
        v
 Radiology Compute
        |
        +--> Geometry
        +--> Resampling
        +--> Reconstruction
        +--> Filtering
        +--> Registration
        +--> Segmentation
        +--> Measurement
        +--> Quantification
        +--> Radiomics
        +--> AI
```

Radiology Compute verarbeitet:

```text
2D Images
3D Volumes
4D Time Series
ROIs
Masks
Segmentations
Transforms
Measurements
Probability Maps
Vector Fields
```

und kann unterschiedliche radiologische Modalitäten unterstützen:

```text
CT
MR
PET
SPECT
X-Ray
Mammography
Ultrasound
Angiography
```

Die Compute-Ausführung kann skalieren über:

```text
CPU
SIMD
GPU
Multi-GPU
NPU
HPC
Distributed Compute
```

Die Architektur trennt konsequent:

```text
Original Data
Quantitative Data
Display Data
Derived Data
Geometry
Processing
AI Results
Measurements
```

voneinander.

Das grundlegende Architekturprinzip lautet:

> NovaOS Radiology Compute behandelt radiologische Daten nicht lediglich als Bilder, sondern als räumlich definierte, quantitativ interpretierbare und nachvollziehbar transformierbare Compute-Objekte. Jede Rekonstruktion, Registrierung, Segmentierung, Messung und KI-Auswertung bleibt auf ihre Quelldaten, Geometrie, Algorithmen und Parameter zurückführbar. Dadurch entsteht eine gemeinsame Infrastruktur für klassische medizinische Bildverarbeitung, quantitative Analyse, multimodale Verarbeitung und zukünftige KI-gestützte Verfahren, ohne technische Compute-Ergebnisse mit einer medizinischen Diagnose gleichzusetzen.