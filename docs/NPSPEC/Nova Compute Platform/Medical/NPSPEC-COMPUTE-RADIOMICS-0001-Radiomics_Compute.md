# NPSPEC-COMPUTE-RADIOMICS-0001 — Radiomics Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Medical Imaging / Radiomics  
**Komponente:** NovaOS Radiomics Compute  
**Bezeichner:** `NPSPEC-COMPUTE-RADIOMICS-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RADIOLOGY-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0001`, `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`, `NPSPEC-COMPUTE-DICOM-0001`, `NPSPEC-COMPUTE-RADIOLOGY-0001`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-PACS-0001`, `NPSPEC-COMPUTE-ULTRASOUND-0001`, `NPSPEC-COMPUTE-MEDAI-0001`  
**Kurzname:** Radiomics Compute  

---

## 1. Zweck

Diese Spezifikation definiert die Radiomics-Compute-Infrastruktur der NovaOS-Compute-Architektur.

Radiomics Compute stellt standardisierte Funktionen zur reproduzierbaren Extraktion quantitativer Merkmale aus medizinischen Bild-, Volumen- und Regionsdaten bereit.

Die Komponente verarbeitet insbesondere:

- CT,
- MR,
- PET,
- SPECT,
- Ultraschall,
- Röntgen,
- Mammographie,
- multimodale Bilddaten,
- segmentierte Regionen,
- Volumes of Interest,
- Regions of Interest,
- zeitabhängige medizinische Bilddaten.

Radiomics Compute transformiert medizinische Bilddaten in strukturierte quantitative Merkmalsvektoren.

Die grundlegende Architektur lautet:

```text
Medical Image / Volume
          +
   ROI / Segmentation
          |
          v
Input Validation
          |
          v
Image Preprocessing
          |
          v
Discretization
          |
          v
Feature Extraction
          |
          +--> Shape
          +--> First Order
          +--> GLCM
          +--> GLRLM
          +--> GLSZM
          +--> GLDM
          +--> NGTDM
          +--> Filtered Features
          |
          v
Feature Validation
          |
          v
Radiomics Feature Vector
```

Radiomics Compute MUSS die Extraktion quantitativer Bildmerkmale von deren medizinischer Interpretation trennen.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Ein Radiomics-Merkmal ist nur zusammen mit seinen Quelldaten, seiner Region, seiner Bildgeometrie, seiner Vorverarbeitung, seiner Diskretisierung und seiner exakten Feature-Definition eindeutig interpretierbar.

Radiomics Compute MUSS deshalb nicht nur numerische Feature-Werte, sondern auch deren vollständigen Berechnungskontext verwalten können.

---

## 3. Reproduzierbarkeitsgrundsatz

Radiomics-Ergebnisse MÜSSEN reproduzierbar spezifizierbar sein.

Mindestens folgende Faktoren MÜSSEN nachvollziehbar sein:

```text
Source Image
Source Series
ROI / VOI
Image Geometry
Resampling
Interpolation
Intensity Processing
Discretization
Feature Definition
Feature Parameters
Filter
Runtime Version
Backend
Precision
```

---

## 4. Standardisierungsgrundsatz

Feature-Namen allein reichen nicht aus, um mathematische Gleichheit zu garantieren.

Die Architektur MUSS unterschiedliche Feature-Definitionen und Implementierungsvarianten explizit identifizieren können.

---

## 5. Ziele

Radiomics Compute MUSS architektonisch mindestens folgende Bereiche unterstützen:

1. Image Radiomics,
2. Volume Radiomics,
3. 2D Radiomics,
4. 2.5D Radiomics,
5. 3D Radiomics,
6. ROI-Verarbeitung,
7. VOI-Verarbeitung,
8. Maskenverarbeitung,
9. Segmentierungsintegration,
10. Resampling,
11. Interpolation,
12. Intensity Normalization,
13. Intensity Re-Segmentation,
14. Intensity Discretization,
15. Fixed Bin Width,
16. Fixed Bin Count,
17. Shape Features,
18. First-Order Features,
19. Histogram Features,
20. GLCM,
21. GLRLM,
22. GLSZM,
23. GLDM,
24. NGTDM,
25. Filtered Features,
26. Wavelet Features,
27. LoG Features,
28. Multi-Scale Features,
29. Feature Maps,
30. Batch Extraction,
31. Longitudinal Radiomics,
32. Delta Radiomics,
33. Multimodal Radiomics,
34. Feature Validation,
35. Feature Provenance,
36. Feature Schema,
37. Feature Export,
38. CPU-Ausführung,
39. GPU-Ausführung,
40. Distributed Compute,
41. HPC,
42. Medical-AI-Integration,
43. Diagnostics,
44. Profiling,
45. Erweiterbarkeit.

---

## 6. Nicht-Ziele

Diese Spezifikation definiert nicht:

- konkrete medizinische Diagnosen,
- konkrete Prognosemodelle,
- konkrete Therapieentscheidungen,
- konkrete Biomarker-Freigaben,
- konkrete KI-Modelle,
- konkrete Segmentierungsmodelle,
- regulatorische Zulassung,
- konkrete Benutzeroberflächen,
- konkrete klinische Schwellenwerte.

---

## 7. Radiomics Study

Ein zusammengehöriger Radiomics-Verarbeitungskontext SOLL als:

`RadiomicsStudy`

repräsentiert werden.

---

## 8. Radiomics Input

Eine Eingabe wird als:

`RadiomicsInput`

repräsentiert.

---

## 9. Source Image

Die zugrunde liegenden medizinischen Bilddaten MÜSSEN eindeutig referenzierbar sein.

---

## 10. Source Identity

Quelldaten SOLLEN über stabile Objektidentitäten referenziert werden.

---

## 11. Source Hash

Für reproduzierbare Offline-Verarbeitung SOLL ein kryptographischer Fingerprint der Quelldaten gespeichert werden können.

---

## 12. ROI

Eine zweidimensionale Region of Interest wird als:

`RadiomicsROI`

repräsentiert.

---

## 13. VOI

Eine dreidimensionale Volume of Interest wird als:

`RadiomicsVOI`

repräsentiert.

---

## 14. Mask

Eine binäre oder gelabelte Maske wird als:

`RadiomicsMask`

repräsentiert.

---

## 15. Region Identity

Jede verarbeitete Region SOLL eine eindeutige Identität besitzen.

---

## 16. Region Label

Regionen SOLLEN semantische Labels besitzen können.

---

## 17. Region Geometry

Die räumliche Geometrie einer Region MUSS eindeutig definiert sein.

---

## 18. Region Source

Es MUSS nachvollziehbar sein, wie eine Region erzeugt wurde.

Beispiele:

```text
Manual
SemiAutomatic
Automatic
Imported
AI Segmentation
Threshold
Derived
```

---

## 19. Segmentation Integration

Radiomics Compute SOLL Segmentierungen aus `NPSPEC-COMPUTE-RADIOLOGY-0001` verwenden können.

---

## 20. AI Segmentation

KI-generierte Segmentierungen SOLLEN über `NPSPEC-COMPUTE-MEDAI-0001` verwendet werden können.

---

## 21. Segmentation Provenance

Wenn eine KI-Segmentierung verwendet wird, SOLL deren Modell- und Provenance-Information referenzierbar sein.

---

## 22. Multiple Regions

Ein Bild oder Volumen DARF mehrere getrennte Radiomics-Regionen enthalten.

---

## 23. Connected Components

Getrennte Komponenten einer Maske SOLLEN wahlweise:

```text
MERGE
SEPARATE
LARGEST_ONLY
```

behandelt werden können.

---

## 24. Region Validation

Vor Feature-Extraktion MUSS die Region validiert werden.

---

## 25. Empty Region

Eine leere Region MUSS als ungültige Eingabe erkannt werden.

---

## 26. Minimum Region Size

Eine Mindestgröße für Regionen SOLL konfigurierbar sein.

---

## 27. Minimum Voxel Count

Eine minimale Voxelanzahl SOLL definierbar sein.

---

## 28. Mask Alignment

Bild und Maske MÜSSEN räumlich miteinander kompatibel sein oder kontrolliert registriert beziehungsweise resampled werden.

---

## 29. Geometry Validation

Mindestens folgende Eigenschaften SOLLEN geprüft werden:

```text
Dimensions
Spacing
Origin
Orientation
Coordinate System
Extent
```

---

## 30. Image Geometry

Die vollständige Bildgeometrie MUSS für geometrieabhängige Features erhalten bleiben.

---

## 31. Physical Units

Geometrische Radiomics-Features MÜSSEN physikalische Einheiten verwenden.

---

## 32. Pixel vs Physical Space

Pixel- beziehungsweise Voxelkoordinaten DÜRFEN nicht ohne definierte Transformation als physikalische Koordinaten behandelt werden.

---

## 33. Dimensionality

Radiomics Compute MUSS zwischen:

```text
2D
2.5D
3D
```

unterscheiden können.

---

## 34. 2D Radiomics

2D Radiomics berechnet Features innerhalb einzelner Bildschichten.

---

## 35. 2.5D Radiomics

2.5D Radiomics SOLL schichtweise Berechnung mit definierter Aggregation unterstützen können.

---

## 36. 3D Radiomics

3D Radiomics verarbeitet die vollständige räumliche VOI.

---

## 37. Dimensionality Provenance

Der verwendete Berechnungsmodus MUSS im Ergebnis gespeichert werden.

---

## 38. Input Validation

Vor der Feature-Extraktion MÜSSEN Eingabedaten validiert werden.

---

## 39. Supported Numeric Types

Mindestens folgende Eingangstypen SOLLEN unterstützt werden können:

```text
Int16
UInt16
Int32
UInt32
Float32
Float64
```

---

## 40. NaN Input

NaN-Werte MÜSSEN kontrolliert behandelt werden.

---

## 41. Infinity Input

Infinity-Werte MÜSSEN kontrolliert behandelt werden.

---

## 42. Invalid Pixel Policy

Ungültige Bildwerte SOLLEN über eine explizite Policy behandelt werden.

---

## 43. Preprocessing

Radiomics Compute MUSS eine explizite Preprocessing-Pipeline unterstützen.

---

## 44. Preprocessing Pipeline

Eine typische Pipeline lautet:

```text
Source Image
     |
     v
Geometry Validation
     |
     v
ROI Validation
     |
     v
Resampling
     |
     v
Intensity Processing
     |
     v
Re-Segmentation
     |
     v
Discretization
     |
     v
Feature Extraction
```

---

## 45. Preprocessing Identity

Eine Preprocessing-Konfiguration MUSS eindeutig identifizierbar und versionierbar sein.

---

## 46. Preprocessing Immutability

Eine veröffentlichte Preprocessing-Konfiguration SOLL unveränderlich sein.

---

## 47. Resampling

Radiomics Compute MUSS räumliches Resampling unterstützen können.

---

## 48. Target Spacing

Das Ziel-Spacing MUSS explizit definierbar sein.

Beispiel:

```text
1.0 mm x 1.0 mm x 1.0 mm
```

---

## 49. Isotropic Resampling

Isotropes Resampling SOLL unterstützt werden.

---

## 50. Anisotropic Resampling

Anisotropes Resampling SOLL unterstützt werden.

---

## 51. Image Interpolation

Für Bilddaten SOLLEN mindestens folgende Interpolationsverfahren unterstützt werden können:

```text
NearestNeighbor
Linear
Cubic
BSpline
```

---

## 52. Mask Interpolation

Masken SOLLEN standardmäßig mit einer labelerhaltenden Interpolationsmethode verarbeitet werden.

---

## 53. Interpolation Provenance

Die verwendete Interpolation MUSS dokumentiert werden.

---

## 54. Original Geometry Preservation

Die Originalgeometrie MUSS unabhängig von resampleten Arbeitsdaten erhalten bleiben.

---

## 55. Resampled Image

Ein resampletes Arbeitsbild MUSS als abgeleitetes Objekt identifizierbar sein.

---

## 56. Intensity Processing

Radiomics Compute SOLL explizite Intensitätsverarbeitung unterstützen.

---

## 57. Intensity Normalization

Intensitätsnormalisierung SOLL optional unterstützt werden.

---

## 58. Normalization Methods

Mindestens folgende Methoden SOLLEN integrierbar sein:

```text
None
ZScore
MinMax
MeanCentering
ReferenceBased
Custom
```

---

## 59. Normalization Parameters

Alle Normalisierungsparameter MÜSSEN nachvollziehbar sein.

---

## 60. Modality Awareness

Intensitätsverarbeitung SOLL modalitätsspezifische Eigenschaften berücksichtigen können.

---

## 61. CT Intensity

CT-basierte Radiomics-Verarbeitung SOLL kalibrierte Intensitätswerte erhalten können.

---

## 62. PET Quantification

Quantitative PET-Werte SOLLEN einschließlich ihrer Einheit erhalten bleiben können.

---

## 63. MR Intensity

MR-Intensitäten DÜRFEN nicht automatisch als zwischen Untersuchungen absolut vergleichbare physikalische Werte behandelt werden.

---

## 64. Ultrasound Intensity

Displaytransformierte Ultraschallpixel DÜRFEN nicht automatisch mit quantitativen RF- oder IQ-Signalen gleichgesetzt werden.

---

## 65. Intensity Re-Segmentation

Radiomics Compute SOLL Intensity Re-Segmentation unterstützen.

---

## 66. Re-Segmentation Range

Ein gültiger Intensitätsbereich SOLL definierbar sein.

Beispiel:

```text
MinIntensity
MaxIntensity
```

---

## 67. Re-Segmentation Provenance

Re-Segmentation-Parameter MÜSSEN gespeichert werden.

---

## 68. Outlier Filtering

Optional SOLL eine kontrollierte Ausreißerbehandlung unterstützt werden.

---

## 69. Outlier Policy

Eine Outlier Policy MUSS explizit konfiguriert werden.

---

## 70. Discretization

Radiomics Compute MUSS Intensitätsdiskretisierung unterstützen.

---

## 71. Discretization Object

Eine Diskretisierungskonfiguration wird als:

`RadiomicsDiscretization`

repräsentiert.

---

## 72. Fixed Bin Width

Fixed Bin Width wird als:

`FBW`

bezeichnet.

---

## 73. Fixed Bin Count

Fixed Bin Count wird als:

`FBC`

bezeichnet.

---

## 74. Discretization Mode

Mindestens folgende Modi MÜSSEN unterstützt werden können:

```text
FIXED_BIN_WIDTH
FIXED_BIN_COUNT
```

---

## 75. Bin Width

Bei `FIXED_BIN_WIDTH` MUSS die Bin-Breite explizit definiert sein.

---

## 76. Bin Count

Bei `FIXED_BIN_COUNT` MUSS die Anzahl der Bins explizit definiert sein.

---

## 77. Discretization Range

Der für die Diskretisierung verwendete Intensitätsbereich MUSS nachvollziehbar sein.

---

## 78. Discretized Image

Die diskretisierte Darstellung SOLL als internes oder explizites abgeleitetes Objekt verfügbar sein können.

---

## 79. No Source Mutation

Diskretisierung DARF das ursprüngliche Bild nicht verändern.

---

## 80. Feature

Ein einzelnes Radiomics-Merkmal wird als:

`RadiomicsFeature`

repräsentiert.

---

## 81. Feature Value

Der numerische Wert wird als:

`FeatureValue`

repräsentiert.

---

## 82. Feature ID

Jedes Feature MUSS eine eindeutige maschinenlesbare Feature-ID besitzen.

---

## 83. Feature Name

Features SOLLEN zusätzlich einen menschenlesbaren Namen besitzen.

---

## 84. Feature Definition Version

Die mathematische Feature-Definition MUSS versionierbar sein.

---

## 85. Feature Family

Ein Feature MUSS einer Feature-Familie zugeordnet werden können.

---

## 86. Feature Families

Mindestens folgende Familien SOLLEN unterstützt werden:

```text
SHAPE
FIRST_ORDER
HISTOGRAM
GLCM
GLRLM
GLSZM
GLDM
NGTDM
FILTERED
CUSTOM
```

---

## 87. Feature Vector

Eine Menge zusammengehöriger Features wird als:

`RadiomicsFeatureVector`

repräsentiert.

---

## 88. Feature Set

Eine definierte Menge zu berechnender Features wird als:

`RadiomicsFeatureSet`

repräsentiert.

---

## 89. Feature Set Version

Feature Sets MÜSSEN versionierbar sein.

---

## 90. Feature Selection

Aufrufer SOLLEN einzelne Features oder vollständige Feature-Familien auswählen können.

---

## 91. Shape Features

Radiomics Compute MUSS geometrische Shape Features unterstützen.

---

## 92. Shape Independence

Shape Features SOLLEN grundsätzlich aus der Regionengeometrie und nicht aus Intensitätswerten berechnet werden.

---

## 93. 2D Shape Features

Mögliche 2D-Features umfassen:

```text
Area
Perimeter
MajorAxisLength
MinorAxisLength
Elongation
Compactness
MaximumDiameter
```

---

## 94. 3D Shape Features

Mögliche 3D-Features umfassen:

```text
Volume
SurfaceArea
SurfaceToVolumeRatio
Sphericity
Compactness
Maximum3DDiameter
MajorAxisLength
MinorAxisLength
LeastAxisLength
Elongation
Flatness
```

---

## 95. Volume

Das physikalische Volumen SOLL aus Voxelgeometrie und Region berechnet werden können.

---

## 96. Surface Area

Oberflächenberechnung MUSS die verwendete geometrische Methode dokumentieren können.

---

## 97. Surface Algorithm

Unterschiedliche Surface-Reconstruction-Verfahren MÜSSEN unterscheidbar sein.

---

## 98. Maximum Diameter

Maximale Durchmesser SOLLEN in physikalischen Einheiten ausgegeben werden.

---

## 99. Shape Precision

Die numerische Präzision geometrischer Features MUSS definierbar sein.

---

## 100. First-Order Features

Radiomics Compute MUSS First-Order-Statistik unterstützen.

---

## 101. First-Order Input

First-Order Features werden aus den Intensitätswerten innerhalb der gültigen Region berechnet.

---

## 102. First-Order Features

Mindestens folgende Features SOLLEN unterstützt werden:

```text
Minimum
Maximum
Mean
Median
Range
Variance
StandardDeviation
Skewness
Kurtosis
Energy
RootMeanSquare
Entropy
Percentile
InterquartileRange
MeanAbsoluteDeviation
RobustMeanAbsoluteDeviation
```

---

## 103. Percentiles

Perzentile SOLLEN parametrisierbar sein.

---

## 104. Entropy Definition

Die verwendete Entropiedefinition MUSS eindeutig spezifiziert sein.

---

## 105. Histogram

Radiomics Compute SOLL Histogramm-basierte Features unterstützen.

---

## 106. Histogram Object

Ein Histogramm wird als:

`RadiomicsHistogram`

repräsentiert.

---

## 107. Histogram Bins

Histogramm-Bin-Grenzen MÜSSEN nachvollziehbar sein.

---

## 108. Histogram Range

Der Histogrammbereich MUSS dokumentierbar sein.

---

## 109. Texture Features

Radiomics Compute MUSS Texturmerkmale unterstützen.

---

## 110. Texture Matrix

Texturmatrizen SOLLEN als explizite interne Compute-Objekte repräsentierbar sein.

---

## 111. GLCM

Gray Level Co-occurrence Matrix wird abgekürzt:

`GLCM`

---

## 112. GLCM Object

Eine GLCM wird als:

`RadiomicsGLCM`

repräsentiert.

---

## 113. GLCM Direction

Die räumliche Richtung beziehungsweise Offset-Definition MUSS explizit sein.

---

## 114. GLCM Distance

Die Nachbarschaftsdistanz MUSS konfigurierbar sein.

---

## 115. GLCM Symmetry

Symmetrische und nicht symmetrische GLCM-Berechnung MÜSSEN unterscheidbar sein.

---

## 116. GLCM Normalization

Die Normalisierung der Matrix MUSS definiert sein.

---

## 117. GLCM Aggregation

Mehrere Richtungen SOLLEN über definierte Aggregationsregeln kombiniert werden können.

---

## 118. GLCM Features

Mögliche Features umfassen:

```text
Contrast
Correlation
JointEnergy
JointEntropy
Homogeneity
Dissimilarity
Autocorrelation
ClusterShade
ClusterProminence
MaximumProbability
DifferenceEntropy
DifferenceVariance
SumEntropy
```

---

## 119. GLRLM

Gray Level Run Length Matrix wird abgekürzt:

`GLRLM`

---

## 120. GLRLM Object

Eine GLRLM wird als:

`RadiomicsGLRLM`

repräsentiert.

---

## 121. GLRLM Direction

Die Run-Richtung MUSS explizit definiert sein.

---

## 122. GLRLM Aggregation

Mehrere Richtungen SOLLEN über definierte Regeln aggregiert werden können.

---

## 123. GLRLM Features

Mögliche Features umfassen:

```text
ShortRunEmphasis
LongRunEmphasis
GrayLevelNonUniformity
RunLengthNonUniformity
RunPercentage
LowGrayLevelRunEmphasis
HighGrayLevelRunEmphasis
ShortRunLowGrayLevelEmphasis
ShortRunHighGrayLevelEmphasis
LongRunLowGrayLevelEmphasis
LongRunHighGrayLevelEmphasis
```

---

## 124. GLSZM

Gray Level Size Zone Matrix wird abgekürzt:

`GLSZM`

---

## 125. GLSZM Object

Eine GLSZM wird als:

`RadiomicsGLSZM`

repräsentiert.

---

## 126. Zone Definition

Die Definition räumlich zusammenhängender Zonen MUSS eindeutig spezifiziert sein.

---

## 127. Connectivity

Die verwendete Nachbarschaft beziehungsweise Connectivity MUSS definiert werden.

---

## 128. GLSZM Features

Mögliche Features umfassen:

```text
SmallAreaEmphasis
LargeAreaEmphasis
GrayLevelNonUniformity
SizeZoneNonUniformity
ZonePercentage
LowGrayLevelZoneEmphasis
HighGrayLevelZoneEmphasis
SmallAreaLowGrayLevelEmphasis
SmallAreaHighGrayLevelEmphasis
LargeAreaLowGrayLevelEmphasis
LargeAreaHighGrayLevelEmphasis
```

---

## 129. GLDM

Gray Level Dependence Matrix wird abgekürzt:

`GLDM`

---

## 130. GLDM Object

Eine GLDM wird als:

`RadiomicsGLDM`

repräsentiert.

---

## 131. Dependence Definition

Die Definition einer Abhängigkeit MUSS parametrisierbar und dokumentiert sein.

---

## 132. Dependence Distance

Die verwendete Nachbarschaftsdistanz MUSS gespeichert werden.

---

## 133. Dependence Threshold

Ein verwendeter Intensitätstoleranzwert MUSS dokumentiert werden.

---

## 134. GLDM Features

Mögliche Features umfassen:

```text
SmallDependenceEmphasis
LargeDependenceEmphasis
GrayLevelNonUniformity
DependenceNonUniformity
DependenceEntropy
LowGrayLevelEmphasis
HighGrayLevelEmphasis
```

---

## 135. NGTDM

Neighbouring Gray Tone Difference Matrix wird abgekürzt:

`NGTDM`

---

## 136. NGTDM Object

Eine NGTDM wird als:

`RadiomicsNGTDM`

repräsentiert.

---

## 137. NGTDM Neighborhood

Die verwendete räumliche Nachbarschaft MUSS definiert sein.

---

## 138. NGTDM Features

Mögliche Features umfassen:

```text
Coarseness
Contrast
Busyness
Complexity
Strength
```

---

## 139. Directional Texture

Texturfeatures SOLLEN richtungsabhängig berechnet werden können.

---

## 140. Direction Aggregation

Radiomics Compute MUSS die Aggregationsstrategie explizit repräsentieren können.

---

## 141. Aggregation Modes

Mögliche Modi:

```text
NONE
AVERAGE_FEATURES
MERGE_MATRICES
SLICE_AVERAGE
VOLUME_MERGE
CUSTOM
```

---

## 142. Aggregation Provenance

Die Aggregationsmethode MUSS im Feature-Ergebnis gespeichert werden.

---

## 143. Filtered Radiomics

Radiomics Compute SOLL Features aus gefilterten Bildrepräsentationen berechnen können.

---

## 144. Filtered Image

Ein gefiltertes Bild wird als:

`RadiomicsFilteredImage`

repräsentiert.

---

## 145. Filter Identity

Jeder Filter MUSS eindeutig identifizierbar sein.

---

## 146. Filter Parameters

Alle Filterparameter MÜSSEN gespeichert werden können.

---

## 147. Wavelet

Wavelet-basierte Radiomics-Features SOLLEN unterstützt werden können.

---

## 148. Wavelet Decomposition

Mehrdimensionale Wavelet-Zerlegung SOLL unterstützt werden können.

---

## 149. Wavelet Family

Die verwendete Wavelet-Familie MUSS dokumentiert werden.

---

## 150. Wavelet Level

Die Zerlegungsebene MUSS konfigurierbar sein.

---

## 151. Wavelet Subband

Features aus einzelnen Subbands MÜSSEN eindeutig benannt werden.

---

## 152. LoG

Laplacian-of-Gaussian wird abgekürzt:

`LoG`

---

## 153. LoG Filtering

LoG-Filterung SOLL unterstützt werden können.

---

## 154. LoG Sigma

Der verwendete Sigma-Wert MUSS gespeichert werden.

---

## 155. Multi-Scale LoG

Mehrere Sigma-Werte SOLLEN in einem Feature Set verwendet werden können.

---

## 156. Gradient Features

Gradientenbasierte Filter SOLLEN integrierbar sein.

---

## 157. Laplacian Features

Laplacian-basierte Filter SOLLEN integrierbar sein.

---

## 158. Local Binary Pattern

Local-Binary-Pattern-basierte Verarbeitung SOLL integrierbar sein.

---

## 159. Custom Filter

Benutzerdefinierte Filter SOLLEN über eine standardisierte Erweiterungsschnittstelle integrierbar sein.

---

## 160. Original Image Features

Features des ungefilterten Original- beziehungsweise Preprocessing-Arbeitsbildes MÜSSEN von Filterfeatures unterscheidbar sein.

---

## 161. Feature Naming

Feature-IDs SOLLEN strukturiert aufgebaut werden können.

Beispiel:

```text
original.firstorder.mean
original.glcm.contrast
wavelet.HHH.glcm.entropy
log.sigma_2_0.firstorder.mean
```

---

## 162. Feature Namespace

Feature-IDs SOLLEN Namespaces verwenden, um Kollisionen zu vermeiden.

---

## 163. Feature Schema

Radiomics Compute MUSS ein maschinenlesbares Feature Schema unterstützen.

---

## 164. Feature Schema Fields

Ein Schemaeintrag SOLL mindestens enthalten können:

```text
FeatureId
FeatureName
FeatureFamily
DefinitionVersion
DataType
Unit
Parameters
```

---

## 165. Unit

Dimensionsbehaftete Features MÜSSEN ihre physikalische Einheit deklarieren.

---

## 166. Dimensionless Features

Dimensionslose Features SOLLEN explizit als dimensionslos gekennzeichnet werden können.

---

## 167. Missing Feature

Nicht berechenbare Features MÜSSEN explizit repräsentierbar sein.

---

## 168. Missing Feature State

Mindestens folgende Zustände SOLLEN unterstützt werden:

```text
VALID
NOT_APPLICABLE
INSUFFICIENT_DATA
NUMERICAL_FAILURE
INVALID_INPUT
DISABLED
```

---

## 169. No Silent Zero

Ein nicht berechenbares Feature DARF nicht stillschweigend als numerischer Wert `0` ausgegeben werden.

---

## 170. Feature Precision

Feature-Werte SOLLEN mindestens in `Float64` berechnet beziehungsweise ausgegeben werden können.

---

## 171. Internal Precision

Interne Berechnungspräzision MUSS dokumentierbar sein.

---

## 172. Numerical Stability

Radiomics-Algorithmen MÜSSEN auf numerische Stabilität ausgelegt sein.

---

## 173. Overflow

Numerischer Overflow MUSS erkannt werden.

---

## 174. Underflow

Numerischer Underflow SOLL diagnostizierbar sein.

---

## 175. Division by Zero

Division durch Null MUSS kontrolliert behandelt werden.

---

## 176. Constant Region

Regionen mit konstantem Intensitätswert MÜSSEN korrekt behandelt werden.

---

## 177. Single Voxel Region

Regionen mit nur einem Voxel MÜSSEN entweder definiert verarbeitet oder explizit abgelehnt werden.

---

## 178. Sparse Region

Sehr dünn oder spärlich belegte Regionen SOLLEN diagnostizierbar sein.

---

## 179. Feature Map

Radiomics Compute SOLL lokale Feature Maps unterstützen können.

---

## 180. Feature Map Object

Eine lokale Feature Map wird als:

`RadiomicsFeatureMap`

repräsentiert.

---

## 181. Sliding Window

Lokale Features SOLLEN über Sliding-Window-Verfahren berechnet werden können.

---

## 182. Window Size

Die Fenstergröße MUSS definierbar sein.

---

## 183. Window Shape

Fensterformen SOLLEN definierbar sein.

---

## 184. Feature Map Geometry

Feature Maps MÜSSEN eine definierte räumliche Geometrie besitzen.

---

## 185. Feature Map Validity

Ungültige Rand- oder Maskenbereiche MÜSSEN gekennzeichnet werden können.

---

## 186. Multi-Region Extraction

Mehrere Regionen SOLLEN innerhalb eines Jobs verarbeitet werden können.

---

## 187. Batch Extraction

Radiomics Compute SOLL Batch-Extraktion unterstützen.

---

## 188. Batch Job

Ein Batch-Job wird als:

`RadiomicsBatchJob`

repräsentiert.

---

## 189. Batch Inputs

Ein Batch DARF enthalten:

```text
Multiple Patients
Multiple Studies
Multiple Series
Multiple Regions
Multiple Feature Sets
```

---

## 190. Batch Isolation

Ergebnisse unterschiedlicher Eingaben MÜSSEN eindeutig getrennt bleiben.

---

## 191. Parallel Extraction

Unabhängige Regionen, Bilder oder Feature-Familien SOLLEN parallel verarbeitet werden können.

---

## 192. Scheduler Integration

Radiomics Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 193. Task Graph

Eine Radiomics-Pipeline SOLL als Task Graph repräsentiert werden können.

Beispiel:

```text
Load
 |
 v
Validate
 |
 v
Resample
 |
 v
Normalize
 |
 v
Discretize
 |
 +----------+----------+
 |          |          |
 v          v          v
Shape    FirstOrder  Texture
 |          |          |
 +----------+----------+
            |
            v
       Feature Vector
```

---

## 194. Dependency Reuse

Gemeinsame Zwischenprodukte SOLLEN zwischen Feature-Familien wiederverwendet werden.

---

## 195. Matrix Reuse

Eine bereits berechnete Texturmatrix SOLL für mehrere daraus abgeleitete Features wiederverwendet werden können.

---

## 196. Lazy Feature Evaluation

Nur angeforderte Features SOLLEN berechnet werden müssen.

---

## 197. Memory Model

Radiomics Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 198. Memory Objects

Mindestens folgende Speicherobjekte SOLLEN unterstützt werden:

```text
ImageBuffer
MaskBuffer
ResampledBuffer
DiscretizedBuffer
FilteredBuffer
TextureMatrixBuffer
FeatureBuffer
FeatureMapBuffer
TemporaryWorkspace
```

---

## 199. Buffer Reuse

Temporäre Speicherbereiche SOLLEN wiederverwendet werden.

---

## 200. Out-of-Core

Sehr große Volumen SOLLEN über Out-of-Core-Verarbeitung unterstützt werden können.

---

## 201. Memory Budget

Ein Radiomics-Job SOLL ein Speicherbudget deklarieren können.

---

## 202. CPU Backend

Radiomics Compute MUSS CPU-Ausführung unterstützen.

---

## 203. SIMD

Geeignete Feature-Berechnungen SOLLEN SIMD verwenden können.

---

## 204. Multi-Core

Unabhängige Feature-Familien und Regionen SOLLEN parallel auf mehreren CPU-Kernen verarbeitet werden können.

---

## 205. GPU Backend

Radiomics Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` integrieren.

---

## 206. GPU Suitable Operations

Geeignete GPU-Operationen umfassen:

```text
Resampling
Filtering
Histogram Generation
Texture Matrix Generation
Feature Maps
Large Batch Processing
```

---

## 207. GPU Optionality

Eine minimale Radiomics-Implementierung DARF keine GPU voraussetzen.

---

## 208. GPU Numerical Compatibility

GPU-Ergebnisse SOLLEN innerhalb definierter Toleranzen mit CPU-Referenzen übereinstimmen.

---

## 209. NPU Backend

Eine NPU ist für klassische Radiomics-Feature-Extraktion nicht erforderlich.

NPU-Verarbeitung DARF für integrierte Medical-AI-Workflows verwendet werden.

---

## 210. Distributed Compute

Radiomics Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0007` integrieren können.

---

## 211. Distributed Batch

Große Radiomics-Kohorten SOLLEN über mehrere Compute Nodes verteilt werden können.

---

## 212. Data Locality

Der Scheduler SOLL Datenlokalität berücksichtigen.

---

## 213. Feature Determinism Across Nodes

Verteilte Ausführung SOLL dieselben Feature-Definitionen und Konfigurationen auf allen Nodes verwenden.

---

## 214. Runtime Compatibility

Compute Nodes MÜSSEN für einen reproduzierbaren Job kompatible Runtime- und Feature-Versionen verwenden.

---

## 215. HPC Integration

Radiomics Compute SOLL `NPSPEC-COMPUTE-HPC-0001` für große Kohortenanalysen integrieren.

---

## 216. Cohort Processing

Eine große Patientenkohorte SOLL als paralleler Radiomics-Workload verarbeitet werden können.

---

## 217. Longitudinal Radiomics

Radiomics Compute SOLL zeitlich wiederholte Untersuchungen unterstützen.

---

## 218. Longitudinal Study

Eine zeitabhängige Radiomics-Serie wird als:

`RadiomicsLongitudinalStudy`

repräsentiert.

---

## 219. Time Point

Jede Untersuchung MUSS einem definierten Zeitpunkt zugeordnet werden können.

---

## 220. Baseline

Ein Zeitpunkt SOLL als Baseline definiert werden können.

---

## 221. Follow-Up

Weitere Zeitpunkte SOLLEN als Follow-Up repräsentiert werden.

---

## 222. Delta Radiomics

Radiomics Compute SOLL Delta Radiomics unterstützen.

---

## 223. Delta Feature

Ein Delta Feature wird als:

`RadiomicsDeltaFeature`

repräsentiert.

---

## 224. Absolute Delta

Eine absolute Änderung kann berechnet werden als:

```text
Delta = FollowUp - Baseline
```

---

## 225. Relative Delta

Eine relative Änderung kann definiert werden als:

```text
RelativeDelta = (FollowUp - Baseline) / Baseline
```

---

## 226. Delta Definition

Die verwendete Delta-Definition MUSS explizit gespeichert werden.

---

## 227. Zero Baseline

Relative Delta-Berechnungen MÜSSEN den Fall einer Null-Baseline kontrolliert behandeln.

---

## 228. Longitudinal Compatibility

Feature-Vergleiche über Zeit SOLLEN nur bei kompatiblen Feature- und Preprocessing-Konfigurationen durchgeführt werden.

---

## 229. Longitudinal Geometry

Registrierung beziehungsweise Geometrieanpassung zwischen Zeitpunkten SOLL über Radiology Compute unterstützt werden können.

---

## 230. Multimodal Radiomics

Radiomics Compute SOLL multimodale Feature-Extraktion unterstützen.

---

## 231. Modality-Specific Features

Features verschiedener Modalitäten MÜSSEN getrennt identifizierbar sein.

---

## 232. Multimodal Feature Vector

Features mehrerer Modalitäten SOLLEN zu einem strukturierten Feature Vector kombiniert werden können.

---

## 233. Registration Requirement

Wenn Features räumlich kombiniert werden, MUSS die erforderliche Registrierung explizit definiert sein.

---

## 234. Feature Fusion

Radiomics Compute SOLL Feature-Level-Fusion unterstützen können.

---

## 235. Feature Fusion Provenance

Die Herkunft jedes fusionierten Features MUSS nachvollziehbar bleiben.

---

## 236. Feature Normalization

Feature-Vektoren SOLLEN für nachgelagerte Analysen normalisiert werden können.

---

## 237. Feature Scaling

Mögliche Verfahren umfassen:

```text
ZScore
MinMax
RobustScaling
Custom
```

---

## 238. Extraction vs Analysis

Feature Scaling für statistische oder KI-Analyse MUSS von der ursprünglichen Feature-Extraktion getrennt bleiben.

---

## 239. Feature Selection

Radiomics Compute DARF Feature Selection unterstützen.

---

## 240. Feature Selection Separation

Feature Selection ist eine nachgelagerte Analyse und DARF ursprüngliche Feature-Werte nicht verändern.

---

## 241. Feature Reduction

Dimensionsreduktion SOLL integrierbar sein.

---

## 242. PCA Integration

Principal Component Analysis DARF als nachgelagerter Verarbeitungsschritt verwendet werden.

---

## 243. Statistical Analysis Boundary

Radiomics Compute stellt primär reproduzierbare Features bereit.

Statistische Hypothesentests und klinische Modellierung SOLLEN separat spezifiziert werden.

---

## 244. Medical AI Integration

Radiomics Compute SOLL `NPSPEC-COMPUTE-MEDAI-0001` integrieren.

---

## 245. Radiomics as AI Input

Feature-Vektoren SOLLEN als strukturierte Eingaben für Medical-AI-Modelle verwendet werden können.

---

## 246. AI Pipeline

Beispiel:

```text
CT Volume
   |
   v
Segmentation
   |
   v
Radiomics
   |
   v
Feature Vector
   |
   v
Medical AI
   |
   v
Prediction
```

---

## 247. Deep Radiomics Separation

Klassische handdefinierte Radiomics-Features und durch neuronale Netze erzeugte Deep Features MÜSSEN unterscheidbar sein.

---

## 248. Deep Feature

Ein neuronales Feature kann als:

`MedicalAIEmbedding`

beziehungsweise kompatibles Deep-Feature-Objekt repräsentiert werden.

---

## 249. Hybrid Radiomics

Klassische Radiomics-Features und AI Embeddings SOLLEN gemeinsam verwendet werden können.

---

## 250. Feature Provenance

Radiomics Compute MUSS vollständige Feature Provenance unterstützen.

---

## 251. Provenance Record

Ein:

`RadiomicsProvenanceRecord`

SOLL mindestens enthalten:

```text
SourceReference
SourceHash
RegionId
RegionSource
Geometry
Dimensionality
Resampling
Interpolation
IntensityProcessing
ReSegmentation
Discretization
Filter
FeatureId
FeatureDefinitionVersion
RuntimeVersion
Backend
Precision
Timestamp
```

---

## 252. Feature Parameters

Alle feature-spezifischen Parameter MÜSSEN im Provenance-Kontext verfügbar sein.

---

## 253. Texture Provenance

Texturfeatures SOLLEN zusätzlich speichern können:

```text
Direction
Distance
Connectivity
Symmetry
Aggregation
MatrixNormalization
```

---

## 254. Filter Provenance

Filterfeatures MÜSSEN Filtertyp und Parameter speichern.

---

## 255. Processing Graph

Die Verarbeitung SOLL als Graph darstellbar sein.

Beispiel:

```text
CT Volume
   |
   +------> Segmentation
   |             |
   +-------------+
         |
         v
      Resample
         |
         v
     Discretize
         |
    +----+----+
    |         |
    v         v
FirstOrder   GLCM
    |         |
    +----+----+
         |
         v
   Feature Vector
```

---

## 256. Feature Reproducibility

Ein Feature SOLL anhand seines Provenance Records reproduzierbar sein.

---

## 257. Deterministic Mode

Radiomics Compute MUSS einen deterministischen Modus unterstützen können.

---

## 258. Thread Determinism

Parallelisierung DARF im deterministischen Modus keine unkontrollierten Ergebnisänderungen verursachen.

---

## 259. Reduction Order

Numerische Reduktionen SOLLEN im deterministischen Modus eine definierte Reihenfolge verwenden können.

---

## 260. Backend Determinism

Die Runtime MUSS dokumentieren können, ob das verwendete Backend deterministische Berechnung garantiert.

---

## 261. Cross-Backend Tolerance

CPU- und GPU-Ergebnisse SOLLEN über definierte Feature-spezifische Toleranzen verglichen werden.

---

## 262. Feature Validation

Radiomics Compute MUSS Feature-Validierung unterstützen.

---

## 263. Reference Dataset

Feature-Implementierungen SOLLEN gegen definierte Referenzdatensätze getestet werden.

---

## 264. Reference Feature Values

Bekannte Referenzwerte SOLLEN mit numerischen Toleranzen hinterlegt werden können.

---

## 265. Implementation Conformance

Eine Feature-Implementierung SOLL angeben können, zu welcher Feature-Definition beziehungsweise Standardisierung sie konform ist.

---

## 266. Conformance Identifier

Eine Implementierung SOLL einen maschinenlesbaren Conformance Identifier besitzen.

---

## 267. Validation State

Ein Feature SOLL einen Validierungsstatus besitzen können:

```text
UNVALIDATED
VALIDATED
EXPERIMENTAL
DEPRECATED
```

---

## 268. Experimental Feature

Experimentelle Features MÜSSEN als solche gekennzeichnet werden.

---

## 269. Deprecated Feature

Veraltete Features SOLLEN weiterhin reproduzierbar bleiben können, aber nicht für neue Feature Sets empfohlen werden.

---

## 270. Feature Export

Radiomics Compute MUSS strukturierte Feature-Ausgabe unterstützen.

---

## 271. Export Object

Ein Export wird als:

`RadiomicsFeatureTable`

repräsentiert.

---

## 272. Feature Table

Eine Feature-Tabelle SOLL mindestens folgende logische Struktur unterstützen:

```text
Subject
Study
Series
Region
FeatureId
Value
Unit
Status
```

---

## 273. Wide Format

Feature-Tabellen SOLLEN im Wide Format darstellbar sein.

Beispiel:

```text
Subject | Region | FeatureA | FeatureB | FeatureC
```

---

## 274. Long Format

Feature-Tabellen SOLLEN im Long Format darstellbar sein.

Beispiel:

```text
Subject | Region | FeatureId | Value
```

---

## 275. Machine-Readable Export

Feature-Daten SOLLEN in maschinenlesbare Datenstrukturen exportierbar sein.

---

## 276. Export Provenance

Ein Export SOLL die verwendete Radiomics-Konfiguration referenzieren.

---

## 277. Feature Configuration

Eine vollständige Extraktionskonfiguration wird als:

`RadiomicsConfiguration`

repräsentiert.

---

## 278. Configuration Fields

Eine Konfiguration SOLL mindestens enthalten:

```text
ConfigurationId
ConfigurationVersion
Dimensionality
Resampling
IntensityProcessing
ReSegmentation
Discretization
Filters
FeatureSet
Precision
```

---

## 279. Configuration Hash

Eine Radiomics-Konfiguration SOLL fingerprintbar sein.

---

## 280. Immutable Configuration

Eine freigegebene Konfiguration SOLL unveränderlich sein.

---

## 281. Configuration Comparison

Zwei Feature-Ergebnisse SOLLEN auf Konfigurationskompatibilität geprüft werden können.

---

## 282. Compatibility State

Mindestens folgende Zustände SOLLEN unterstützt werden:

```text
IDENTICAL
COMPATIBLE
PARTIALLY_COMPATIBLE
INCOMPATIBLE
UNKNOWN
```

---

## 283. No Blind Feature Comparison

Features mit inkompatiblen Definitionen oder Preprocessing-Konfigurationen DÜRFEN nicht automatisch als direkt vergleichbar behandelt werden.

---

## 284. DICOM Integration

Radiomics Compute MUSS `NPSPEC-COMPUTE-DICOM-0001` integrieren können.

---

## 285. DICOM Source

DICOM-Studien und Serien SOLLEN als Radiomics-Quellen verwendet werden können.

---

## 286. DICOM Metadata

Relevante DICOM-Metadaten SOLLEN im Verarbeitungskontext erhalten bleiben.

---

## 287. DICOM Segmentation

DICOM-basierte Segmentierungsobjekte SOLLEN als Regionsquelle verwendet werden können.

---

## 288. Derived Data

Radiomics-Ergebnisse SOLLEN als abgeleitete Daten mit Referenz auf ihre DICOM-Quellen verwaltet werden können.

---

## 289. PACS Integration

Radiomics Compute SOLL `NPSPEC-COMPUTE-PACS-0001` integrieren.

---

## 290. PACS Study Retrieval

Studien SOLLEN aus PACS geladen werden können.

---

## 291. PACS Batch

Mehrere PACS-Studien SOLLEN als Batch verarbeitet werden können.

---

## 292. PACS Source Preservation

Originale PACS-Objekte DÜRFEN durch Radiomics-Verarbeitung nicht verändert werden.

---

## 293. Radiology Integration

Radiomics Compute MUSS `NPSPEC-COMPUTE-RADIOLOGY-0001` integrieren.

---

## 294. Radiology Services

Mindestens folgende gemeinsame Funktionen SOLLEN wiederverwendet werden:

```text
Geometry
Resampling
Registration
Segmentation
Mask Processing
Measurement
Volume Processing
```

---

## 295. Ultrasound Integration

Radiomics Compute SOLL `NPSPEC-COMPUTE-ULTRASOUND-0001` integrieren können.

---

## 296. Ultrasound Radiomics

Radiomics-Features SOLLEN aus geeigneten Ultraschalldaten berechnet werden können.

---

## 297. Ultrasound Data Level

Es MUSS unterscheidbar sein, ob Ultrasound Radiomics auf:

```text
RF
IQ
Envelope
B-Mode
Doppler
Volume
```

basiert.

---

## 298. Ultrasound Display Separation

Features aus Display-Bildern MÜSSEN von Features aus quantitativen Ultraschalldaten unterscheidbar sein.

---

## 299. Security

Radiomics Compute MUSS die NovaOS-Sicherheitsarchitektur verwenden.

---

## 300. Authorization

Zugriffe auf medizinische Quelldaten MÜSSEN autorisierbar sein.

---

## 301. Data Minimization

Radiomics-Jobs SOLLEN nur die für die Extraktion notwendigen Daten erhalten.

---

## 302. Temporary Data

Resamplete, diskretisierte und gefilterte Zwischenbilder SOLLEN nach Policy gelöscht werden können.

---

## 303. Privacy

Radiomics-Feature-Vektoren MÜSSEN als potenziell sensible medizinische Daten behandelt werden.

---

## 304. Feature Re-Identification Risk

Radiomics-Daten DÜRFEN nicht automatisch als anonym betrachtet werden.

---

## 305. Logging Privacy

Logs DÜRFEN keine unnötigen medizinischen Inhalte enthalten.

---

## 306. Audit

Relevante Radiomics-Operationen SOLLEN auditierbar sein.

---

## 307. Audit Events

Mindestens folgende Ereignisse SOLLEN unterstützt werden:

```text
RADIOMICS_JOB_START
RADIOMICS_JOB_COMPLETE
RADIOMICS_JOB_FAILED
FEATURE_EXPORT
CONFIGURATION_CHANGE
DATA_ACCESS
```

---

## 308. Diagnostics

Radiomics Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 309. Diagnostic Classes

Mindestens folgende Klassen SOLLEN existieren:

```text
INVALID_IMAGE
INVALID_VOLUME
INVALID_MASK
EMPTY_REGION
REGION_TOO_SMALL
GEOMETRY_MISMATCH
INVALID_SPACING
INVALID_ORIENTATION
RESAMPLING_FAILED
NORMALIZATION_FAILED
INVALID_RESEGMENTATION
INVALID_DISCRETIZATION
INVALID_BIN_WIDTH
INVALID_BIN_COUNT
FILTER_FAILED
FEATURE_FAILED
TEXTURE_MATRIX_FAILED
NUMERICAL_FAILURE
NAN_DETECTED
INFINITY_DETECTED
OVERFLOW
OUT_OF_MEMORY
BACKEND_FAILURE
INVALID_CONFIGURATION
INCOMPATIBLE_CONFIGURATION
CANCELLED
```

---

## 310. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 311. Feature-Level Diagnostic

Fehler einzelner Features SOLLEN diagnostizierbar sein, ohne zwingend den gesamten Feature-Job abzubrechen.

---

## 312. Fail-Fast Policy

Eine Konfiguration SOLL definieren können, ob bei einem Feature-Fehler:

```text
FAIL_JOB
MARK_FEATURE_INVALID
CONTINUE
```

verwendet wird.

---

## 313. Profiling

Radiomics Compute MUSS Profiling unterstützen.

---

## 314. Profiling Stages

Mindestens:

```text
Load
Validation
Resampling
Normalization
ReSegmentation
Discretization
Filtering
Shape
FirstOrder
GLCM
GLRLM
GLSZM
GLDM
NGTDM
FeatureMaps
Export
```

SOLLTEN separat messbar sein.

---

## 315. Feature Timing

Berechnungszeiten einzelner Feature-Familien SOLLEN messbar sein.

---

## 316. Memory Profiling

Peak Memory SOLL messbar sein.

---

## 317. Throughput

Verarbeitete Regionen beziehungsweise Volumen pro Zeiteinheit SOLLEN messbar sein.

---

## 318. Batch Progress

Große Batch-Jobs SOLLEN Fortschritt melden können.

---

## 319. Cancellation

Radiomics-Jobs MÜSSEN kontrolliert abbrechbar sein.

---

## 320. Partial Batch

Bei Abbruch eines Batch-Jobs MÜSSEN bereits vollständig berechnete Ergebnisse von unvollständigen Ergebnissen unterscheidbar bleiben.

---

## 321. Testing — Geometry

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
2DImage
3DVolume
IsotropicSpacing
AnisotropicSpacing
RotatedGeometry
MaskMismatch
```

---

## 322. Testing — Region

Mindestens:

```text
ValidRegion
EmptyRegion
SingleVoxel
SmallRegion
MultipleComponents
BoundaryRegion
```

MÜSSEN getestet werden.

---

## 323. Testing — Resampling

Mindestens:

```text
NoResampling
IsotropicResampling
AnisotropicResampling
NearestNeighbor
Linear
BSpline
```

SOLLTEN getestet werden.

---

## 324. Testing — Discretization

Mindestens:

```text
FixedBinWidth
FixedBinCount
ConstantImage
NegativeValues
FloatingPointValues
SingleBin
```

MÜSSEN getestet werden.

---

## 325. Testing — First Order

First-Order-Features MÜSSEN gegen analytisch bekannte Datensätze getestet werden.

---

## 326. Testing — Shape

Shape Features MÜSSEN gegen geometrisch bekannte Objekte getestet werden.

Beispiele:

```text
Square
Circle
Cube
Sphere
Cuboid
```

---

## 327. Testing — GLCM

GLCM-Berechnung MUSS gegen kleine manuell überprüfbare Matrizen getestet werden.

---

## 328. Testing — GLRLM

GLRLM-Berechnung MUSS gegen bekannte Referenzmatrizen getestet werden.

---

## 329. Testing — GLSZM

GLSZM-Berechnung MUSS gegen bekannte Referenzmatrizen getestet werden.

---

## 330. Testing — GLDM

GLDM-Berechnung MUSS gegen bekannte Referenzmatrizen getestet werden.

---

## 331. Testing — NGTDM

NGTDM-Berechnung MUSS gegen bekannte Referenzmatrizen getestet werden.

---

## 332. Testing — Direction Aggregation

Alle unterstützten Aggregationsmodi SOLLEN separat getestet werden.

---

## 333. Testing — Filters

Mindestens:

```text
Original
Wavelet
LoG
```

SOLLTEN mit Referenzdaten getestet werden.

---

## 334. Testing — Determinism

Identische Eingaben und Konfigurationen MÜSSEN im deterministischen Modus identische beziehungsweise spezifikationskonform gleiche Ergebnisse erzeugen.

---

## 335. Testing — CPU

Alle normativen Features MÜSSEN auf CPU testbar sein.

---

## 336. Testing — GPU

GPU-Ergebnisse SOLLEN gegen CPU-Referenzergebnisse geprüft werden.

---

## 337. Testing — Batch

Mindestens:

```text
SingleRegion
MultipleRegions
MultipleStudies
LargeBatch
PartialFailure
Cancellation
```

SOLLTEN getestet werden.

---

## 338. Testing — Longitudinal

Delta-Radiomics MUSS mit bekannten Baseline- und Follow-Up-Werten getestet werden.

---

## 339. Testing — Provenance

Für jedes Feature MUSS geprüft werden können, ob die erforderlichen Berechnungsparameter nachvollziehbar sind.

---

## 340. Testing — Configuration Compatibility

Kompatible und inkompatible Radiomics-Konfigurationen MÜSSEN korrekt erkannt werden.

---

## 341. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
Large3DVolume
LargeROI
ManySmallROIs
ThousandsOfFeatures
ManyFilters
LargeFeatureMaps
LargePatientCohort
HighParallelism
MemoryPressure
GPUFallback
DistributedNodeFailure
CancellationUnderLoad
```

---

## 342. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
RadiomicsInput
RadiomicsROI
RadiomicsVOI
RadiomicsMask

RadiomicsFeature
RadiomicsFeatureVector
RadiomicsFeatureSet
RadiomicsConfiguration

2D Radiomics
3D Radiomics

Geometry Validation
Mask Validation

Resampling
Nearest Neighbor
Linear Interpolation

Intensity Re-Segmentation

Fixed Bin Width
Fixed Bin Count

Shape Features
First-Order Features

GLCM
GLRLM
GLSZM
GLDM
NGTDM

Original Image Features

CPU Execution
Multi-Core Architecture

Scheduler Integration
Memory Model Integration
Object Model Integration

DICOM Integration
Radiology Integration

Feature Provenance
Configuration Versioning
Deterministic Mode

Diagnostics
Profiling
Cancellation
```

---

## 343. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
2.5D Radiomics

Cubic Interpolation
BSpline Interpolation

Intensity Normalization
Outlier Processing

Wavelet Radiomics
LoG Radiomics
Gradient Features
Laplacian Features
Local Binary Patterns

Feature Maps
Sliding Window Radiomics

GPU Acceleration
Distributed Compute
HPC Cohort Processing

Longitudinal Radiomics
Delta Radiomics

Multimodal Radiomics
Feature Fusion

Ultrasound Radiomics

Medical AI Integration
Deep Features
Hybrid Radiomics

PACS Integration

Feature Validation Framework
Reference Dataset Framework
Conformance Profiles
```

---

## 344. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RADIOMICS-0001` MUSS:

1. medizinische Bilddaten als Radiomics-Eingabe repräsentieren.
2. medizinische Volumendaten als Radiomics-Eingabe repräsentieren.
3. ROIs repräsentieren.
4. VOIs repräsentieren.
5. Masken repräsentieren.
6. Regionen eindeutig identifizieren.
7. Regionsquellen dokumentieren können.
8. Bild- und Maskengeometrie validieren.
9. leere Regionen erkennen.
10. zu kleine Regionen erkennen können.
11. 2D Radiomics unterstützen.
12. 3D Radiomics unterstützen.
13. den Berechnungsmodus dokumentieren.
14. räumliches Resampling unterstützen.
15. Target Spacing definieren können.
16. Interpolationsmethoden dokumentieren.
17. Originalgeometrie erhalten.
18. Intensitätsverarbeitung explizit konfigurieren.
19. Intensity Re-Segmentation unterstützen.
20. Re-Segmentation-Parameter dokumentieren.
21. Intensitätsdiskretisierung unterstützen.
22. Fixed Bin Width unterstützen.
23. Fixed Bin Count unterstützen.
24. Diskretisierungsparameter dokumentieren.
25. Originaldaten durch Diskretisierung nicht verändern.
26. Features eindeutig identifizieren.
27. Feature-Definitionen versionieren können.
28. Feature-Familien unterscheiden.
29. Feature Sets definieren.
30. Feature Sets versionieren.
31. Shape Features unterstützen.
32. First-Order Features unterstützen.
33. Histogrammverarbeitung unterstützen.
34. GLCM unterstützen.
35. GLCM-Richtung und Distanz dokumentieren.
36. GLCM-Aggregation dokumentieren.
37. GLRLM unterstützen.
38. GLRLM-Aggregation dokumentieren.
39. GLSZM unterstützen.
40. Connectivity dokumentieren können.
41. GLDM unterstützen.
42. Dependence-Parameter dokumentieren.
43. NGTDM unterstützen.
44. Texture-Aggregationsmodi unterscheiden.
45. dimensionsbehaftete Features mit Einheiten versehen.
46. nicht berechenbare Features explizit kennzeichnen.
47. nicht berechenbare Features nicht stillschweigend als Null ausgeben.
48. numerische Fehler erkennen.
49. NaN erkennen.
50. Infinity erkennen.
51. Division durch Null kontrolliert behandeln.
52. konstante Regionen behandeln.
53. CPU-Ausführung unterstützen.
54. Multi-Core-Ausführung architektonisch unterstützen.
55. GPU-Ausführung architektonisch unterstützen.
56. den Compute Scheduler integrieren.
57. das Compute Memory Model integrieren.
58. das Compute Object Model integrieren.
59. DICOM Compute integrieren.
60. Radiology Compute integrieren.
61. PACS Compute architektonisch integrieren können.
62. Ultrasound Compute architektonisch integrieren können.
63. Medical AI Compute architektonisch integrieren können.
64. vollständige Feature Provenance unterstützen.
65. Quelldaten referenzieren.
66. Region referenzieren.
67. Geometrie dokumentieren.
68. Preprocessing dokumentieren.
69. Diskretisierung dokumentieren.
70. Feature-Version dokumentieren.
71. Runtime-Version dokumentieren.
72. Backend dokumentieren.
73. numerische Präzision dokumentieren.
74. Radiomics-Konfigurationen versionieren.
75. Radiomics-Konfigurationen fingerprinten können.
76. Konfigurationskompatibilität prüfen können.
77. einen deterministischen Modus bereitstellen können.
78. strukturierte Diagnosen bereitstellen.
79. Profiling unterstützen.
80. kontrollierten Abbruch unterstützen.
81. Batch-Verarbeitung architektonisch unterstützen.
82. Feature-Ergebnisse maschinenlesbar exportieren.
83. Originaldaten und abgeleitete Daten unterscheiden.
84. Feature-Werte von deren medizinischer Interpretation trennen.
85. Erweiterungen ohne Änderung des grundlegenden Radiomics-Objektmodells ermöglichen.

---

## 345. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaRadiomicsCompute
 |
 +-- RadiomicsRuntime
 |
 +-- RadiomicsDataModel
 |
 +-- RadiomicsInputValidator
 |
 +-- RadiomicsGeometryEngine
 |
 +-- RadiomicsMaskEngine
 |
 +-- RadiomicsPreprocessor
 |
 +-- RadiomicsResampler
 |
 +-- RadiomicsIntensityEngine
 |
 +-- RadiomicsDiscretizer
 |
 +-- RadiomicsShapeEngine
 |
 +-- RadiomicsFirstOrderEngine
 |
 +-- RadiomicsHistogramEngine
 |
 +-- RadiomicsGLCMEngine
 |
 +-- RadiomicsGLRLMEngine
 |
 +-- RadiomicsGLSZMEngine
 |
 +-- RadiomicsGLDMEngine
 |
 +-- RadiomicsNGTDMEngine
 |
 +-- RadiomicsFilterEngine
 |
 +-- RadiomicsFeatureMapEngine
 |
 +-- RadiomicsLongitudinalEngine
 |
 +-- RadiomicsBatchEngine
 |
 +-- RadiomicsFeatureRegistry
 |
 +-- RadiomicsValidator
 |
 +-- RadiomicsProvenance
 |
 +-- RadiomicsExporter
 |
 +-- RadiomicsDiagnostics
 |
 +-- RadiomicsProfiler
```

Diese Struktur ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 346. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt die gemeinsame Ausführungsinfrastruktur bereit.

```text
Radiomics
   |
   v
Compute Runtime
   |
 +-- CPU
 +-- GPU
 +-- Distributed
```

---

## 347. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert:

```text
Resampling
Filtering
Discretization
Shape Extraction
First-Order Extraction
Texture Matrix Generation
Feature Extraction
Batch Processing
```

---

## 348. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet:

```text
Image Buffers
Mask Buffers
Resampled Images
Discretized Images
Filtered Images
Texture Matrices
Feature Maps
Feature Vectors
```

---

## 349. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Radiomics-Objekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 350. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

GPU Runtime SOLL geeignete parallele Radiomics-Operationen beschleunigen können.

---

## 351. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Distributed Compute ermöglicht großskalige Radiomics-Kohortenverarbeitung.

---

## 352. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

HPC Compute stellt Ressourcen für große Feature-Extraktionsjobs und Kohorten bereit.

---

## 353. Beziehung zu `NPSPEC-COMPUTE-DICOM-0001`

DICOM Compute stellt medizinische Bilddaten, Serien, Metadaten und standardisierte Segmentierungsobjekte bereit.

---

## 354. Beziehung zu `NPSPEC-COMPUTE-RADIOLOGY-0001`

Radiology Compute stellt gemeinsame:

```text
Geometry
Resampling
Registration
Segmentation
Volume Processing
```

Funktionen bereit.

Radiomics Compute erweitert diese um quantitative Feature-Extraktion.

---

## 355. Beziehung zu `NPSPEC-COMPUTE-PACS-0001`

PACS Compute stellt Studien und Serien für Radiomics-Jobs bereit.

---

## 356. Beziehung zu `NPSPEC-COMPUTE-ULTRASOUND-0001`

Ultrasound Compute stellt geeignete Ultraschallbild-, Signal-, Doppler- und Volumendaten für Ultrasound Radiomics bereit.

---

## 357. Beziehung zu `NPSPEC-COMPUTE-MEDAI-0001`

Medical AI Compute kann Radiomics Feature Vectors als strukturierte Modelleingaben verwenden.

---

## 358. Grundlegende Radiomics-Pipeline

```text
Medical Image
      +
ROI / VOI
      |
      v
Validation
      |
      v
Resampling
      |
      v
Intensity Processing
      |
      v
Discretization
      |
      +-------------------------+
      |            |            |
      v            v            v
    Shape      First Order    Texture
                                |
                    +-----------+-----------+
                    |     |      |     |    |
                    v     v      v     v    v
                   GLCM GLRLM GLSZM GLDM NGTDM
                    |     |      |     |    |
                    +-----------+-----------+
                                |
                                v
                         Feature Vector
```

---

## 359. Filtered-Radiomics-Pipeline

```text
Preprocessed Image
       |
       +-------------------+
       |                   |
       v                   v
    Original             Filter
       |                   |
       |          +--------+--------+
       |          |        |        |
       |          v        v        v
       |       Wavelet    LoG    Custom
       |          |        |        |
       +----------+--------+--------+
                  |
                  v
           Feature Extraction
```

---

## 360. Longitudinal Pipeline

```text
Baseline Image
      |
      v
Radiomics
      |
      v
Baseline Features
      |
      +--------------------+
                           |
Follow-Up Image            |
      |                    |
      v                    |
Radiomics                  |
      |                    |
      v                    |
Follow-Up Features         |
      |                    |
      +---------+----------+
                |
                v
          Delta Radiomics
```

---

## 361. Medical-AI-Pipeline

```text
Medical Image
     |
     v
Segmentation
     |
     v
Radiomics
     |
     v
Feature Vector
     |
     +----------------+
     |                |
     v                v
Classical Model    Medical AI
     |                |
     +-------+--------+
             |
             v
       Derived Result
```

---

## 362. Datenmodellgrundsatz

Radiomics Compute MUSS unterscheiden zwischen:

```text
Original Image
Working Image
Resampled Image
Filtered Image
Discretized Image
Mask
Texture Matrix
Feature
Feature Vector
Feature Map
```

---

## 363. Vergleichbarkeitsgrundsatz

Zwei Radiomics-Werte sind nur dann unmittelbar vergleichbar, wenn ihre relevanten Berechnungsdefinitionen kompatibel sind.

Insbesondere SOLLEN geprüft werden:

```text
Feature Definition
Dimensionality
Image Geometry
Resampling
Intensity Processing
Re-Segmentation
Discretization
Filter
Texture Parameters
Aggregation
```

---

## 364. Originaldatengrundsatz

Radiomics Compute DARF medizinische Originaldaten nicht verändern.

Alle Verarbeitungsschritte erzeugen logische oder physische Arbeitsrepräsentationen.

---

## 365. Feature-Grundsatz

Ein Feature besteht logisch aus:

```text
Feature Identity
Feature Definition
Feature Value
Unit
Status
Parameters
Source
Region
Provenance
```

und nicht lediglich aus einer Gleitkommazahl.

---

## 366. Standardisierungsgrundsatz

Radiomics Compute SOLL Feature-Definitionen so implementieren, dass externe Standardisierungsprofile und Referenzdefinitionen eingebunden werden können.

Eine konkrete externe Standardisierung DARF durch ein separates Conformance Profile festgelegt werden.

---

## 367. Erweiterbarkeit

Neue:

```text
Feature Families
Texture Matrices
Filters
Discretization Methods
Aggregation Methods
Feature Maps
Modalities
Validation Profiles
Export Formats
Compute Backends
```

SOLLTEN ohne Änderung des grundlegenden Radiomics-Objektmodells integrierbar sein.

---

## 368. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-RADIOMICS-DATA-0001
Radiomics Data Model

NPSPEC-COMPUTE-RADIOMICS-PREPROCESS-0001
Radiomics Preprocessing

NPSPEC-COMPUTE-RADIOMICS-RESAMPLE-0001
Radiomics Resampling

NPSPEC-COMPUTE-RADIOMICS-DISCRETIZE-0001
Radiomics Intensity Discretization

NPSPEC-COMPUTE-RADIOMICS-SHAPE-0001
Radiomics Shape Features

NPSPEC-COMPUTE-RADIOMICS-FIRSTORDER-0001
Radiomics First-Order Features

NPSPEC-COMPUTE-RADIOMICS-GLCM-0001
Gray Level Co-occurrence Matrix

NPSPEC-COMPUTE-RADIOMICS-GLRLM-0001
Gray Level Run Length Matrix

NPSPEC-COMPUTE-RADIOMICS-GLSZM-0001
Gray Level Size Zone Matrix

NPSPEC-COMPUTE-RADIOMICS-GLDM-0001
Gray Level Dependence Matrix

NPSPEC-COMPUTE-RADIOMICS-NGTDM-0001
Neighbouring Gray Tone Difference Matrix

NPSPEC-COMPUTE-RADIOMICS-FILTER-0001
Radiomics Image Filters

NPSPEC-COMPUTE-RADIOMICS-WAVELET-0001
Wavelet Radiomics

NPSPEC-COMPUTE-RADIOMICS-LOG-0001
Laplacian-of-Gaussian Radiomics

NPSPEC-COMPUTE-RADIOMICS-FEATUREMAP-0001
Radiomics Feature Maps

NPSPEC-COMPUTE-RADIOMICS-LONGITUDINAL-0001
Longitudinal Radiomics

NPSPEC-COMPUTE-RADIOMICS-DELTA-0001
Delta Radiomics

NPSPEC-COMPUTE-RADIOMICS-MULTIMODAL-0001
Multimodal Radiomics

NPSPEC-COMPUTE-RADIOMICS-ULTRASOUND-0001
Ultrasound Radiomics

NPSPEC-COMPUTE-RADIOMICS-BATCH-0001
Radiomics Batch Processing

NPSPEC-COMPUTE-RADIOMICS-PROVENANCE-0001
Radiomics Provenance

NPSPEC-COMPUTE-RADIOMICS-CONFORMANCE-0001
Radiomics Conformance Profiles

NPSPEC-COMPUTE-RADIOMICS-EXPORT-0001
Radiomics Feature Export

NPSPEC-COMPUTE-RADIOMICS-TEST-0001
Radiomics Conformance Tests
```

---

## 369. Zusammenfassung

`NPSPEC-COMPUTE-RADIOMICS-0001` definiert die gemeinsame Radiomics-Infrastruktur der NovaOS-Compute-Architektur.

Die grundlegende Verarbeitung lautet:

```text
Medical Image / Volume
          +
       ROI / VOI
          |
          v
      Validation
          |
          v
      Resampling
          |
          v
Intensity Processing
          |
          v
    Discretization
          |
          v
   Feature Extraction
          |
          v
Radiomics Feature Vector
```

Radiomics Compute unterstützt Feature-Familien wie:

```text
Shape
First Order
Histogram
GLCM
GLRLM
GLSZM
GLDM
NGTDM
Filtered Features
```

Erweiterte Verarbeitung umfasst:

```text
Wavelet Radiomics
LoG Radiomics
Feature Maps
Longitudinal Radiomics
Delta Radiomics
Multimodal Radiomics
Ultrasound Radiomics
```

Die Verarbeitung kann über:

```text
CPU
SIMD
Multi-Core
GPU
Distributed Compute
HPC
```

skalieren.

Radiomics Feature Vectors können anschließend unter anderem von:

```text
Statistical Analysis
Medical AI
Machine Learning
Research Pipelines
Longitudinal Analysis
```

verwendet werden.

Die Architektur trennt konsequent:

```text
Original Image
Working Image
Resampled Image
Filtered Image
Discretized Image
Region
Texture Matrix
Feature
Feature Vector
Derived Analysis
```

voneinander.

Für jedes relevante Feature bleiben mindestens:

```text
Source
Region
Geometry
Preprocessing
Resampling
Discretization
Filter
Feature Definition
Feature Parameters
Runtime
Backend
Precision
```

nachvollziehbar.

Die zentrale Reproduzierbarkeitsregel lautet:

> Ein Radiomics-Wert besitzt nur dann eine eindeutige technische Bedeutung, wenn neben seinem numerischen Wert auch seine mathematische Definition und der vollständige Verarbeitungskontext bekannt sind.

Das grundlegende Architekturprinzip lautet:

> NovaOS Radiomics Compute behandelt Radiomics nicht als Sammlung isolierter Formeln, die aus einem medizinischen Bild beliebige Zahlen erzeugen. Bildgeometrie, Region, Vorverarbeitung, Intensitätsdiskretisierung, Filterung, Texturdefinition, Aggregation, Feature-Version und numerische Ausführung werden als explizite Bestandteile jeder Feature-Berechnung behandelt. Dadurch können quantitative Bildmerkmale reproduzierbar, vergleichbar, versionierbar und skalierbar erzeugt und anschließend kontrolliert in medizinische KI-, Forschungs- und Analysepipelines integriert werden.