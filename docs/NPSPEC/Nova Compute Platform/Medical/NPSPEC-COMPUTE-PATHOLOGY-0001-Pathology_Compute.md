# NPSPEC-COMPUTE-PATHOLOGY-0001 — Pathology Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Medical Imaging / Digital Pathology  
**Komponente:** NovaOS Pathology Compute  
**Bezeichner:** `NPSPEC-COMPUTE-PATHOLOGY-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0001`, `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-DICOM-0001`, `NPSPEC-COMPUTE-RADIOLOGY-0001`, `NPSPEC-COMPUTE-PACS-0001`, `NPSPEC-COMPUTE-MEDAI-0001`, `NPSPEC-COMPUTE-RADIOMICS-0001`, `NPSPEC-COMPUTE-IMAGEGUIDED-0001`  
**Kurzname:** Pathology Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Pathology-Compute-Infrastruktur der NovaOS-Compute-Architektur.

Pathology Compute stellt standardisierte Compute-Funktionen für digitale Pathologie und quantitative Verarbeitung pathologischer Bilddaten bereit.

Die Komponente verarbeitet insbesondere:

- Whole Slide Images,
- digitale Histologieschnitte,
- Zytologiebilder,
- Mikroskopbilder,
- Fluoreszenzmikroskopie,
- Immunhistochemie,
- Multiplex-Imaging,
- Tissue Microarrays,
- makroskopische Präparatbilder,
- segmentierte Geweberegionen,
- Zell- und Kernobjekte,
- Annotationen,
- quantitative pathologische Merkmale.

Die grundlegende Architektur lautet:

```text
Pathology Image
      |
      v
Image Validation
      |
      v
Slide / Tile Model
      |
      v
Preprocessing
      |
      v
Tissue Detection
      |
      v
Segmentation
      |
      +----------------+
      |                |
      v                v
Cell Analysis      Tissue Analysis
      |                |
      +--------+-------+
               |
               v
        Feature Extraction
               |
               v
       Quantitative Result
```

Pathology Compute MUSS technische Bildanalyse von pathologischer beziehungsweise medizinischer Interpretation trennen.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Ein digitales Pathologieergebnis ist nur zusammen mit seinem Präparat, seiner Bildquelle, seiner räumlichen Position, seiner Auflösung, seiner Färbung, seiner Vorverarbeitung, seinem Analyseverfahren und seiner Provenance eindeutig interpretierbar.

---

## 3. Originaldatengrundsatz

Pathology Compute DARF originale Pathologie- und Mikroskopiedaten nicht verändern.

Alle Verarbeitungsschritte MÜSSEN abgeleitete Daten beziehungsweise Arbeitsrepräsentationen erzeugen.

---

## 4. Skalierungsgrundsatz

Whole Slide Images können mehrere Milliarden Pixel enthalten.

Pathology Compute MUSS deshalb so ausgelegt sein, dass eine vollständige Dekodierung des gesamten Bildes in den Arbeitsspeicher nicht vorausgesetzt wird.

---

## 5. Räumlicher Grundsatz

Pathologische Bildanalyse MUSS zwischen:

```text
Pixel Coordinates
Tile Coordinates
Pyramid Coordinates
Slide Coordinates
Physical Coordinates
Specimen Coordinates
```

unterscheiden können.

---

## 6. Ziele

Pathology Compute MUSS architektonisch mindestens folgende Bereiche unterstützen:

1. Whole Slide Imaging,
2. Multi-Resolution Images,
3. Image Pyramids,
4. Tile Processing,
5. Region Processing,
6. Streaming,
7. Out-of-Core Processing,
8. Histology Imaging,
9. Cytology Imaging,
10. Brightfield Imaging,
11. Fluorescence Imaging,
12. Immunohistochemistry,
13. Multiplex Imaging,
14. Tissue Microarrays,
15. Tissue Detection,
16. Background Detection,
17. Artifact Detection,
18. Color Processing,
19. Color Normalization,
20. Stain Separation,
21. Stain Deconvolution,
22. Registration,
23. Multi-Stain Registration,
24. Tissue Segmentation,
25. Cell Segmentation,
26. Nucleus Segmentation,
27. Cytoplasm Segmentation,
28. Gland Segmentation,
29. Vessel Segmentation,
30. Region Annotation,
31. Cell Detection,
32. Cell Classification,
33. Cell Counting,
34. Morphometry,
35. Spatial Analysis,
36. Density Analysis,
37. Neighborhood Analysis,
38. Feature Extraction,
39. Texture Analysis,
40. Quantitative Biomarker Measurement,
41. TMA Processing,
42. Z-Stack Processing,
43. Focus Assessment,
44. Quality Control,
45. Medical-AI Integration,
46. DICOM Integration,
47. Batch Processing,
48. GPU Processing,
49. Distributed Compute,
50. HPC,
51. Provenance,
52. Diagnostics,
53. Profiling,
54. Erweiterbarkeit.

---

## 7. Nicht-Ziele

Diese Spezifikation definiert nicht:

- konkrete pathologische Diagnosen,
- konkrete Tumorklassifikationen,
- konkrete Grading-Systeme,
- konkrete klinische Schwellenwerte,
- konkrete Therapieentscheidungen,
- konkrete Biomarker-Zulassungen,
- konkrete KI-Modelle,
- konkrete Scannerhardware,
- konkrete Mikroskophardware,
- konkrete Benutzeroberflächen,
- regulatorische Zulassung.

---

## 8. Pathology Study

Ein zusammengehöriger Pathologie-Verarbeitungskontext wird als:

`PathologyStudy`

repräsentiert.

---

## 9. Case

Ein pathologischer Fall SOLL als:

`PathologyCase`

repräsentiert werden können.

---

## 10. Case Identity

Ein Fall MUSS eine eindeutige technische Identität besitzen.

---

## 11. Specimen

Ein biologisches Präparat wird als:

`PathologySpecimen`

repräsentiert.

---

## 12. Specimen Identity

Jedes Präparat MUSS eindeutig identifizierbar sein.

---

## 13. Specimen Hierarchy

Pathology Compute SOLL hierarchische Präparatbeziehungen darstellen können.

Beispiel:

```text
Case
 |
 +-- Specimen
      |
      +-- Block
           |
           +-- Section
                |
                +-- Slide
```

---

## 14. Block

Ein Gewebeblock SOLL als:

`PathologyBlock`

repräsentiert werden können.

---

## 15. Section

Ein Gewebeschnitt SOLL als:

`PathologySection`

repräsentiert werden können.

---

## 16. Slide

Ein physischer beziehungsweise digital repräsentierter Objektträger wird als:

`PathologySlide`

repräsentiert.

---

## 17. Slide Identity

Jeder Slide MUSS eindeutig identifizierbar sein.

---

## 18. Slide Image

Ein digitales Bild eines Slides wird als:

`PathologySlideImage`

repräsentiert.

---

## 19. Multiple Images

Ein Slide DARF mehrere digitale Bildrepräsentationen besitzen.

---

## 20. Acquisition

Eine Bildaufnahme wird als:

`PathologyAcquisition`

repräsentiert.

---

## 21. Acquisition Metadata

Mindestens folgende Metadaten SOLLEN unterstützt werden können:

```text
Scanner
AcquisitionDate
Resolution
Objective
PixelSpacing
FocalPlane
Illumination
ColorProfile
Compression
```

---

## 22. Whole Slide Image

Ein Whole Slide Image wird als:

`WholeSlideImage`

beziehungsweise:

`WSI`

repräsentiert.

---

## 23. WSI Dimensions

Ein WSI MUSS Bilddimensionen größer als klassische 32-Bit-Bildgrößen sicher repräsentieren können.

---

## 24. 64-Bit Coordinates

Slide-Dimensionen und Pixelpositionen SOLLEN mindestens mit 64-Bit-Adressierung repräsentierbar sein.

---

## 25. Multi-Resolution Pyramid

Ein WSI SOLL mehrere Auflösungsstufen enthalten können.

---

## 26. Pyramid

Die Auflösungshierarchie wird als:

`PathologyImagePyramid`

repräsentiert.

---

## 27. Pyramid Level

Eine einzelne Auflösungsstufe wird als:

`PathologyPyramidLevel`

repräsentiert.

---

## 28. Level Metadata

Eine Pyramid Level MUSS mindestens enthalten können:

```text
LevelIndex
Width
Height
DownsampleFactor
PixelSpacing
TileSize
```

---

## 29. Level Zero

Die höchste verfügbare räumliche Auflösung SOLL als:

```text
Level 0
```

repräsentiert werden.

---

## 30. Downsample Factor

Das Verhältnis einer Pyramid Level zur höchsten Auflösung MUSS explizit verfügbar sein.

---

## 31. Physical Resolution

Die physikalische Pixelgröße SOLL unabhängig von der Pyramid Level bestimmbar sein.

---

## 32. Magnification

Nominale Vergrößerungsangaben SOLLEN als Metadaten unterstützt werden.

---

## 33. Magnification vs Resolution

Nominale Vergrößerung DARF nicht als Ersatz für eine bekannte physikalische Pixelgröße verwendet werden.

---

## 34. Tile

Ein rechteckiger Bildausschnitt wird als:

`PathologyTile`

repräsentiert.

---

## 35. Tile Identity

Ein Tile SOLL eindeutig über mindestens:

```text
SlideId
Level
X
Y
Width
Height
```

identifizierbar sein.

---

## 36. Tile Coordinates

Tile-Koordinaten MÜSSEN eindeutig einer Pyramid Level zugeordnet sein.

---

## 37. Tile Request

Ein Tile wird über:

`PathologyTileRequest`

angefordert.

---

## 38. Tile Result

Das Ergebnis wird als:

`PathologyTileResult`

repräsentiert.

---

## 39. Tile Decoder

Pathology Compute MUSS Tile-basierte Bilddekodierung unterstützen.

---

## 40. Partial Decode

Ein WSI-Decoder SOLL nur benötigte Bildregionen dekodieren können.

---

## 41. Tile Cache

Dekodierte Tiles SOLLEN zwischengespeichert werden können.

---

## 42. Cache Budget

Der Tile Cache MUSS ein konfigurierbares Speicherbudget besitzen.

---

## 43. Cache Eviction

Nicht mehr benötigte Tiles SOLLEN automatisch aus dem Cache entfernt werden können.

---

## 44. Prefetching

Benachbarte oder wahrscheinlich benötigte Tiles SOLLEN vorgeladen werden können.

---

## 45. Predictive Prefetch

Die Runtime DARF Zugriffsmuster für Tile Prefetching verwenden.

---

## 46. Out-of-Core

Pathology Compute MUSS Out-of-Core-Verarbeitung unterstützen.

---

## 47. Streaming

Bildregionen SOLLEN als Stream verarbeitet werden können.

---

## 48. Region

Eine beliebige Slide-Region wird als:

`PathologyRegion`

repräsentiert.

---

## 49. Region Geometry

Regionen SOLLEN mindestens folgende Formen unterstützen können:

```text
Rectangle
Polygon
Ellipse
Mask
MultiPolygon
```

---

## 50. Region Coordinate Space

Jede Region MUSS ihr Koordinatensystem deklarieren.

---

## 51. Physical Region

Regionen SOLLEN in physikalischen Koordinaten definierbar sein.

---

## 52. Annotation

Eine pathologische Annotation wird als:

`PathologyAnnotation`

repräsentiert.

---

## 53. Annotation Types

Mindestens folgende Typen SOLLEN unterstützt werden:

```text
Point
Polyline
Polygon
Rectangle
Ellipse
Mask
Text
Measurement
```

---

## 54. Annotation Identity

Jede Annotation MUSS eindeutig identifizierbar sein.

---

## 55. Annotation Source

Die Herkunft einer Annotation MUSS dokumentierbar sein.

---

## 56. Annotation Sources

Mögliche Quellen:

```text
Manual
Imported
Algorithm
MedicalAI
Derived
```

---

## 57. Annotation Label

Annotationen SOLLEN semantische Labels besitzen können.

---

## 58. Annotation Versioning

Annotationen SOLLEN versionierbar sein.

---

## 59. Annotation Provenance

Änderungen an Annotationen SOLLEN nachvollziehbar sein.

---

## 60. Image Types

Pathology Compute SOLL verschiedene pathologische Bildtypen unterstützen.

---

## 61. Brightfield

Brightfield-Mikroskopie MUSS unterstützt werden können.

---

## 62. Fluorescence

Fluoreszenzmikroskopie SOLL unterstützt werden.

---

## 63. Multi-Channel Image

Mehrkanalbilder werden als:

`PathologyMultiChannelImage`

repräsentiert.

---

## 64. Channel

Ein Bildkanal wird als:

`PathologyImageChannel`

repräsentiert.

---

## 65. Channel Identity

Jeder Kanal MUSS eindeutig identifizierbar sein.

---

## 66. Channel Metadata

Kanäle SOLLEN enthalten können:

```text
Name
Marker
Wavelength
Exposure
Unit
DynamicRange
```

---

## 67. Channel Count

Die Architektur DARF nicht auf RGB mit genau drei Kanälen beschränkt sein.

---

## 68. RGB

Konventionelle RGB-Bilddaten MÜSSEN unterstützt werden.

---

## 69. RGBA

RGBA-Daten SOLLEN unterstützt werden können.

---

## 70. Grayscale

Graustufenbilder MÜSSEN unterstützt werden können.

---

## 71. High Bit Depth

Mindestens:

```text
8-bit
16-bit
Float32
```

SOLLTEN als Kanalrepräsentationen unterstützt werden können.

---

## 72. Color Space

Der verwendete Farbraum MUSS bekannt oder explizit als unbekannt markiert sein.

---

## 73. Color Profiles

ICC- beziehungsweise vergleichbare Farbprofile SOLLEN erhalten werden können.

---

## 74. Color Conversion

Farbraumtransformationen MÜSSEN dokumentierbar sein.

---

## 75. Color Normalization

Pathology Compute SOLL Farbnormalisierung unterstützen.

---

## 76. Color Normalization Object

Eine Normalisierungskonfiguration wird als:

`PathologyColorNormalization`

repräsentiert.

---

## 77. Normalization Reference

Referenzbasierte Normalisierung SOLL unterstützt werden können.

---

## 78. Normalization Provenance

Normalisierungsalgorithmus und Parameter MÜSSEN dokumentiert werden.

---

## 79. Stain

Eine histologische Färbung wird als:

`PathologyStain`

repräsentiert.

---

## 80. Stain Identity

Eine Färbung SOLL eindeutig identifizierbar sein.

---

## 81. Stain Metadata

Mindestens folgende Eigenschaften SOLLEN unterstützt werden können:

```text
StainName
Protocol
Batch
Acquisition
```

---

## 82. Multiple Stains

Ein Slide beziehungsweise eine Analyse DARF mehrere Marker oder Färbungen enthalten.

---

## 83. H&E

Hämatoxylin-Eosin-Verarbeitung SOLL unterstützt werden.

---

## 84. IHC

Immunhistochemische Bildverarbeitung SOLL unterstützt werden.

---

## 85. Multiplex

Multiplex-Imaging mit vielen Markern SOLL unterstützt werden.

---

## 86. Stain Separation

Pathology Compute SOLL Farbstoff- beziehungsweise Markertrennung unterstützen.

---

## 87. Color Deconvolution

Color Deconvolution SOLL unterstützt werden.

---

## 88. Stain Matrix

Eine für Farbdekonvolution verwendete Matrix wird als:

`PathologyStainMatrix`

repräsentiert.

---

## 89. Stain Matrix Provenance

Die verwendete Stain Matrix MUSS dokumentiert werden.

---

## 90. Optical Density

Transformationen in optische Dichte SOLLEN unterstützt werden.

---

## 91. Optical Density Definition

Die verwendete mathematische Definition MUSS dokumentiert sein.

---

## 92. Stain Channels

Aus Stain Separation erzeugte Kanäle MÜSSEN als abgeleitete Bilddaten gekennzeichnet werden.

---

## 93. Preprocessing

Pathology Compute MUSS eine explizite Preprocessing-Pipeline unterstützen.

---

## 94. Preprocessing Pipeline

Eine typische Pipeline lautet:

```text
Slide
  |
  v
Decode
  |
  v
Color Management
  |
  v
Background Detection
  |
  v
Artifact Detection
  |
  v
Normalization
  |
  v
Analysis
```

---

## 95. Preprocessing Configuration

Eine Preprocessing-Konfiguration wird als:

`PathologyPreprocessingConfiguration`

repräsentiert.

---

## 96. Configuration Versioning

Preprocessing-Konfigurationen MÜSSEN versionierbar sein.

---

## 97. Configuration Fingerprint

Eine Konfiguration SOLL fingerprintbar sein.

---

## 98. Tissue Detection

Pathology Compute MUSS Gewebe von Hintergrund unterscheiden können.

---

## 99. Tissue Mask

Das Ergebnis wird als:

`PathologyTissueMask`

repräsentiert.

---

## 100. Background

Nichtgewebliche Bereiche SOLLEN explizit erkannt werden können.

---

## 101. Tissue Fraction

Der Gewebeanteil einer Region oder eines Tiles SOLL berechnet werden können.

---

## 102. Empty Tile

Tiles ohne relevante Gewebeinformation SOLLEN übersprungen werden können.

---

## 103. Tissue Bounding Region

Die räumliche Ausdehnung des erkannten Gewebes SOLL bestimmbar sein.

---

## 104. Artifact Detection

Pathology Compute SOLL Bildartefakte erkennen können.

---

## 105. Artifact Types

Mögliche Artefakte umfassen:

```text
Blur
OutOfFocus
Fold
Tear
Bubble
Debris
PenMark
ScannerArtifact
Overexposure
Underexposure
Saturation
CompressionArtifact
```

---

## 106. Artifact Mask

Artefaktbereiche SOLLEN als:

`PathologyArtifactMask`

repräsentiert werden können.

---

## 107. Artifact Exclusion

Analysen SOLLEN definierte Artefaktbereiche ausschließen können.

---

## 108. Artifact Provenance

Automatisch erkannte Artefakte MÜSSEN ihre Herkunft dokumentieren können.

---

## 109. Focus Assessment

Pathology Compute SOLL Fokusqualität bewerten können.

---

## 110. Focus Score

Eine Fokusmetrik wird als:

`PathologyFocusScore`

repräsentiert.

---

## 111. Focus Map

Lokale Fokusqualität SOLL als:

`PathologyFocusMap`

repräsentiert werden können.

---

## 112. Z-Stack

Mehrere Fokusebenen SOLLEN als:

`PathologyZStack`

repräsentiert werden.

---

## 113. Z Position

Jede Ebene MUSS ihre Z-Position beziehungsweise Fokusebene dokumentieren.

---

## 114. Best Focus

Die Auswahl einer optimalen Fokusebene SOLL unterstützt werden.

---

## 115. Focus Fusion

Mehrere Fokusebenen SOLLEN zu einer abgeleiteten Fokusfusion kombiniert werden können.

---

## 116. Registration

Pathology Compute SOLL Bildregistrierung unterstützen.

---

## 117. Slide Registration

Mehrere digitale Repräsentationen eines Slides SOLLEN registrierbar sein.

---

## 118. Serial Section Registration

Aufeinanderfolgende Gewebeschnitte SOLLEN räumlich registrierbar sein.

---

## 119. Multi-Stain Registration

Unterschiedlich gefärbte Schnitte SOLLEN registrierbar sein.

---

## 120. Registration Types

Mindestens folgende Verfahren SOLLEN integrierbar sein:

```text
Rigid
Affine
Deformable
Landmark
FeatureBased
IntensityBased
Hybrid
```

---

## 121. Registration Transform

Registrierungsergebnisse MÜSSEN als explizite Transformationen gespeichert werden.

---

## 122. Registration Quality

Registrierungen MÜSSEN Qualitätsinformationen besitzen können.

---

## 123. Registration Provenance

Registrierungsalgorithmus, Parameter und Eingabedaten MÜSSEN nachvollziehbar sein.

---

## 124. Tissue Segmentation

Pathology Compute MUSS Gewebesegmentierung unterstützen können.

---

## 125. Tissue Classes

Gewebeklassen SOLLEN frei definierbar sein.

---

## 126. Segmentation

Eine Segmentierung wird als:

`PathologySegmentation`

repräsentiert.

---

## 127. Segmentation Mask

Segmentierungen SOLLEN als Masken repräsentiert werden können.

---

## 128. Multi-Class Segmentation

Mehrklassige Segmentierungen SOLLEN unterstützt werden.

---

## 129. Segmentation Probability

Probabilistische Segmentierungen SOLLEN unterstützt werden können.

---

## 130. Segmentation Source

Die Herkunft einer Segmentierung MUSS dokumentiert werden.

---

## 131. Cell Detection

Pathology Compute SOLL Zellen erkennen können.

---

## 132. Cell Object

Eine erkannte Zelle wird als:

`PathologyCell`

repräsentiert.

---

## 133. Cell Identity

Jede erkannte Zelle MUSS innerhalb eines Analyseergebnisses eindeutig identifizierbar sein.

---

## 134. Cell Position

Eine Zelle MUSS eine räumliche Position besitzen.

---

## 135. Cell Geometry

Zellobjekte SOLLEN geometrische Eigenschaften besitzen können.

---

## 136. Nucleus

Ein Zellkern wird als:

`PathologyNucleus`

repräsentiert.

---

## 137. Nucleus Segmentation

Kernsegmentierung SOLL unterstützt werden.

---

## 138. Cytoplasm

Zytoplasma wird als:

`PathologyCytoplasm`

repräsentiert.

---

## 139. Cytoplasm Segmentation

Zytoplasma-Segmentierung SOLL unterstützt werden können.

---

## 140. Cell-Nucleus Relationship

Zellen und ihre Kerne MÜSSEN miteinander verknüpft werden können.

---

## 141. Multiple Nuclei

Die Datenstruktur DARF nicht voraussetzen, dass jede Zelle exakt einen Kern besitzt.

---

## 142. Gland Segmentation

Drüsenstrukturen SOLLEN segmentiert werden können.

---

## 143. Vessel Segmentation

Gefäßstrukturen SOLLEN segmentiert werden können.

---

## 144. Object Hierarchy

Pathologische Objekte SOLLEN hierarchisch repräsentierbar sein.

Beispiel:

```text
Tissue Region
 |
 +-- Gland
 |
 +-- Vessel
 |
 +-- Cell
      |
      +-- Nucleus
      |
      +-- Cytoplasm
```

---

## 145. Cell Classification

Zellen SOLLEN klassifiziert werden können.

---

## 146. Classification Result

Ein Klassifikationsergebnis wird als:

`PathologyCellClassification`

repräsentiert.

---

## 147. Classification Label

Klassifikationslabels SOLLEN frei definierbar sein.

---

## 148. Classification Confidence

Probabilistische Klassifikationen SOLLEN Konfidenzwerte besitzen.

---

## 149. Classification Source

Die Quelle einer Klassifikation MUSS dokumentiert werden.

---

## 150. Cell Counting

Pathology Compute SOLL Zellzählungen unterstützen.

---

## 151. Count Region

Jede Zellzählung MUSS ihre analysierte Region referenzieren.

---

## 152. Cell Density

Zelldichte SOLL in physikalischen Flächeneinheiten berechnet werden können.

---

## 153. Count by Class

Zellen SOLLEN nach Klassifikationslabel gezählt werden können.

---

## 154. Positive Cell Count

Markerpositive Zellen SOLLEN technisch gezählt werden können.

---

## 155. Positive Definition

Die Definition von `positive` MUSS explizit durch die Analysekonfiguration vorgegeben werden.

---

## 156. No Universal Threshold

Pathology Compute DARF keinen universellen medizinischen Positivitätsschwellenwert voraussetzen.

---

## 157. Morphometry

Pathology Compute SOLL morphometrische Messungen unterstützen.

---

## 158. Cell Morphometry

Mögliche Zellmerkmale umfassen:

```text
Area
Perimeter
Diameter
MajorAxis
MinorAxis
Eccentricity
Circularity
Solidity
Orientation
```

---

## 159. Nuclear Morphometry

Entsprechende Kernmerkmale SOLLEN unterstützt werden.

---

## 160. Physical Units

Geometrische Merkmale MÜSSEN, sofern Kalibrierung vorhanden ist, in physikalischen Einheiten ausgegeben werden können.

---

## 161. Intensity Features

Objektbasierte Intensitätsmerkmale SOLLEN unterstützt werden.

---

## 162. Intensity Statistics

Mindestens folgende Statistiken SOLLEN verfügbar sein können:

```text
Minimum
Maximum
Mean
Median
Variance
StandardDeviation
Percentile
IntegratedIntensity
```

---

## 163. Channel-Specific Features

Intensitätsmerkmale MÜSSEN einem Bild- oder Stain-Kanal zugeordnet werden können.

---

## 164. Texture Analysis

Pathology Compute SOLL Texturanalyse unterstützen.

---

## 165. Texture Features

Mögliche Texturverfahren umfassen:

```text
GLCM
GLRLM
GLSZM
LocalBinaryPattern
Wavelet
FrequencyFeatures
Custom
```

---

## 166. Texture Scale

Die räumliche Skala einer Texturanalyse MUSS dokumentiert werden.

---

## 167. Radiomics Integration

Für standardisierte Bildmerkmale SOLL `NPSPEC-COMPUTE-RADIOMICS-0001` integriert werden können.

---

## 168. Pathomics

Pathologie-spezifische quantitative Feature-Extraktion DARF als:

`Pathomics`

bezeichnet werden.

---

## 169. Pathomics Feature

Ein Pathomics-Merkmal wird als:

`PathologyFeature`

repräsentiert.

---

## 170. Feature Identity

Jedes Feature MUSS eindeutig identifizierbar sein.

---

## 171. Feature Families

Pathology Compute SOLL mindestens folgende Feature-Familien unterstützen können:

```text
Morphology
Intensity
Color
Texture
Spatial
Density
Neighborhood
Graph
Stain
Quality
```

---

## 172. Feature Vector

Eine Menge zusammengehöriger Features wird als:

`PathologyFeatureVector`

repräsentiert.

---

## 173. Feature Set

Eine definierte Menge von Features wird als:

`PathologyFeatureSet`

repräsentiert.

---

## 174. Feature Versioning

Feature-Definitionen MÜSSEN versionierbar sein.

---

## 175. Spatial Analysis

Pathology Compute SOLL räumliche Zell- und Gewebestrukturen analysieren können.

---

## 176. Spatial Point Set

Zellzentren beziehungsweise Objektpositionen SOLLEN als räumliche Punktmengen repräsentiert werden.

---

## 177. Nearest Neighbor

Nearest-Neighbor-Distanzen SOLLEN berechnet werden können.

---

## 178. K-Nearest Neighbor

K-Nearest-Neighbor-Analysen SOLLEN unterstützt werden.

---

## 179. Radius Neighborhood

Objekte innerhalb eines physikalischen Radius SOLLEN bestimmt werden können.

---

## 180. Cell Neighborhood

Eine Zellnachbarschaft wird als:

`PathologyCellNeighborhood`

repräsentiert.

---

## 181. Neighborhood Composition

Die Zusammensetzung benachbarter Zellklassen SOLL analysiert werden können.

---

## 182. Spatial Density

Lokale Zelldichten SOLLEN berechnet werden können.

---

## 183. Distance to Region

Distanzen zwischen Zellen und segmentierten Geweberegionen SOLLEN berechnet werden können.

---

## 184. Boundary Distance

Die Distanz eines Objekts zu einer Regionsgrenze SOLL bestimmbar sein.

---

## 185. Spatial Graph

Objektbeziehungen SOLLEN als:

`PathologySpatialGraph`

repräsentiert werden können.

---

## 186. Graph Nodes

Graphknoten können beispielsweise sein:

```text
Cells
Nuclei
Glands
Vessels
Regions
```

---

## 187. Graph Edges

Graphkanten SOLLEN räumliche Beziehungen repräsentieren.

---

## 188. Graph Construction

Mindestens folgende Konstruktionsverfahren SOLLEN integrierbar sein:

```text
KNearestNeighbor
Radius
Delaunay
Voronoi
RegionAdjacency
Custom
```

---

## 189. Graph Features

Graphbasierte quantitative Merkmale SOLLEN berechnet werden können.

---

## 190. Tissue Microarray

Pathology Compute SOLL Tissue Microarrays unterstützen.

---

## 191. TMA

Tissue Microarray wird abgekürzt:

`TMA`

---

## 192. TMA Object

Ein TMA wird als:

`PathologyTMA`

repräsentiert.

---

## 193. TMA Core

Ein einzelner Core wird als:

`PathologyTMACore`

repräsentiert.

---

## 194. Core Identity

Jeder TMA Core MUSS eindeutig identifizierbar sein.

---

## 195. Core Grid

Eine TMA-Anordnung SOLL als Rastermodell repräsentiert werden können.

---

## 196. Missing Core

Fehlende beziehungsweise beschädigte Cores MÜSSEN explizit markiert werden können.

---

## 197. Core Detection

Automatische Core-Erkennung SOLL unterstützt werden.

---

## 198. Core Mapping

TMA Cores SOLLEN auf externe Probenidentitäten abgebildet werden können.

---

## 199. Quantitative Biomarker Measurement

Pathology Compute SOLL quantitative Marker-Messungen unterstützen.

---

## 200. Marker Measurement

Eine Marker-Messung wird als:

`PathologyMarkerMeasurement`

repräsentiert.

---

## 201. Marker Identity

Der gemessene Marker MUSS eindeutig identifizierbar sein.

---

## 202. Measurement Region

Jede Messung MUSS ihre analysierte Region referenzieren.

---

## 203. Measurement Method

Die verwendete Messmethode MUSS dokumentiert werden.

---

## 204. Measurement Units

Messwerte MÜSSEN, sofern vorhanden, ihre Einheit deklarieren.

---

## 205. Threshold-Based Measurement

Schwellenwertbasierte Messungen SOLLEN unterstützt werden können.

---

## 206. Threshold Provenance

Verwendete Schwellenwerte MÜSSEN im Analysekontext gespeichert werden.

---

## 207. Continuous Measurement

Kontinuierliche Markerintensitäten SOLLEN unabhängig von Klassifikationsschwellen ausgegeben werden können.

---

## 208. Medical AI Integration

Pathology Compute SOLL `NPSPEC-COMPUTE-MEDAI-0001` integrieren.

---

## 209. AI Tasks

Medical AI SOLL für Pathology Compute unter anderem folgende Aufgaben bereitstellen können:

```text
Tissue Detection
Artifact Detection
Cell Detection
Nucleus Segmentation
Cell Segmentation
Tissue Classification
Cell Classification
Feature Extraction
Quality Assessment
```

---

## 210. AI Result

AI-generierte Ergebnisse MÜSSEN als abgeleitete Daten gekennzeichnet werden.

---

## 211. Model Identity

Die Identität des verwendeten Modells MUSS nachvollziehbar sein.

---

## 212. Model Version

Die Modellversion MUSS dokumentierbar sein.

---

## 213. Model Provenance

AI-Ergebnisse SOLLEN auf Modell, Konfiguration und Eingangsdaten zurückführbar sein.

---

## 214. AI Confidence

Konfidenz- beziehungsweise Wahrscheinlichkeitswerte SOLLEN erhalten bleiben.

---

## 215. AI Result State

Mindestens folgende Zustände SOLLEN unterstützt werden:

```text
VALID
LOW_CONFIDENCE
INVALID
FAILED
UNKNOWN
```

---

## 216. AI Is Not Ground Truth

Ein AI-generiertes Ergebnis DARF nicht automatisch als Ground Truth behandelt werden.

---

## 217. Human Annotation

Manuelle Annotationen MÜSSEN von AI-generierten Annotationen unterscheidbar sein.

---

## 218. Hybrid Workflow

Manuelle und automatische Ergebnisse SOLLEN gemeinsam verwendet werden können.

---

## 219. Review State

AI-Ergebnisse SOLLEN einen Review-Status besitzen können.

---

## 220. Review States

Beispielsweise:

```text
UNREVIEWED
ACCEPTED
MODIFIED
REJECTED
```

---

## 221. DICOM Integration

Pathology Compute SOLL `NPSPEC-COMPUTE-DICOM-0001` integrieren.

---

## 222. DICOM WSI

DICOM-basierte Whole Slide Images SOLLEN verarbeitet werden können.

---

## 223. DICOM Metadata

Relevante DICOM-Metadaten MÜSSEN erhalten bleiben.

---

## 224. DICOM Specimen

Präparatbezogene DICOM-Metadaten SOLLEN in das Pathology-Datenmodell überführt werden können.

---

## 225. DICOM Coordinates

DICOM-basierte räumliche Informationen MÜSSEN korrekt interpretiert werden.

---

## 226. Derived Objects

Abgeleitete Pathology-Ergebnisse SOLLEN ihre DICOM-Quellen referenzieren können.

---

## 227. PACS Integration

Pathology Compute SOLL `NPSPEC-COMPUTE-PACS-0001` integrieren.

---

## 228. Study Retrieval

Pathologische Bildstudien SOLLEN aus kompatiblen medizinischen Bildarchiven geladen werden können.

---

## 229. Source Preservation

Originale Archivobjekte DÜRFEN durch Pathology Compute nicht verändert werden.

---

## 230. External Formats

Die Architektur SOLL herstellerspezifische und offene WSI-Formate über Decoder-Plugins integrieren können.

---

## 231. Decoder Interface

Ein WSI-Decoder MUSS eine standardisierte Schnittstelle implementieren.

---

## 232. Decoder Capabilities

Ein Decoder SOLL mindestens melden können:

```text
Dimensions
PyramidLevels
TileSize
PixelFormat
Channels
Compression
PhysicalResolution
Metadata
```

---

## 233. Decoder Isolation

Komplexe beziehungsweise nicht vertrauenswürdige Decoder SOLLEN isoliert ausgeführt werden können.

---

## 234. Malformed Input

Fehlerhafte Bilddateien MÜSSEN kontrolliert abgelehnt werden.

---

## 235. Decompression Limits

Decoder SOLLEN Schutz gegen unkontrollierte Speicher- und Ressourcenbelegung besitzen.

---

## 236. Runtime Integration

Pathology Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0001` integrieren.

---

## 237. Scheduler Integration

Pathology Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 238. Task Graph

Eine Pathology-Analyse SOLL als Task Graph repräsentiert werden können.

Beispiel:

```text
Load Slide Metadata
        |
        v
Tissue Detection
        |
        v
Tile Selection
        |
        +--------------------+
        |                    |
        v                    v
   Tile Analysis        Tile Analysis
        |                    |
        +---------+----------+
                  |
                  v
             Aggregation
                  |
                  v
                Result
```

---

## 239. Tile Parallelism

Unabhängige Tiles SOLLEN parallel verarbeitet werden können.

---

## 240. Region Parallelism

Unabhängige Regionen SOLLEN parallel verarbeitet werden können.

---

## 241. Pipeline Parallelism

Dekodierung, Vorverarbeitung und Analyse SOLLEN als Pipeline parallelisiert werden können.

---

## 242. Work Stealing

Der Scheduler DARF Work-Stealing für große Tile-Mengen verwenden.

---

## 243. Priority

Interaktive Tile-Anfragen SOLLEN gegenüber Batch-Hintergrundanalysen priorisiert werden können.

---

## 244. Cancellation

Pathology-Jobs MÜSSEN kontrolliert abbrechbar sein.

---

## 245. Partial Results

Bereits vollständig berechnete Teilresultate MÜSSEN von unvollständigen Ergebnissen unterscheidbar sein.

---

## 246. Memory Model

Pathology Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 247. Memory Objects

Mindestens folgende Speicherobjekte SOLLEN unterstützt werden:

```text
TileBuffer
RegionBuffer
MaskBuffer
ChannelBuffer
FeatureBuffer
CellObjectBuffer
SpatialGraphBuffer
TemporaryWorkspace
```

---

## 248. Memory Budget

Jeder große Analysejob SOLL ein Speicherbudget deklarieren können.

---

## 249. Bounded Memory

Whole-Slide-Verarbeitung MUSS mit begrenztem Arbeitsspeicher möglich sein.

---

## 250. Buffer Reuse

Temporäre Tile- und Analysebuffer SOLLEN wiederverwendet werden.

---

## 251. Zero-Copy

Geeignete Datenpfade SOLLEN Zero-Copy beziehungsweise reduzierte Kopieroperationen unterstützen.

---

## 252. CPU Backend

Pathology Compute MUSS CPU-Ausführung unterstützen.

---

## 253. SIMD

Pixel-, Farb- und Featureoperationen SOLLEN SIMD verwenden können.

---

## 254. Multi-Core

Tile- und Objektanalyse SOLL Multi-Core-Verarbeitung unterstützen.

---

## 255. GPU Backend

Pathology Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` integrieren.

---

## 256. GPU Suitable Operations

Geeignete GPU-Aufgaben umfassen:

```text
Color Conversion
Stain Deconvolution
Filtering
Tissue Segmentation
Cell Segmentation
Nucleus Segmentation
Image Registration
Feature Extraction
Texture Analysis
Large Tile Batches
```

---

## 257. GPU Optionality

Eine grundlegende Pathology-Implementierung DARF keine GPU voraussetzen.

---

## 258. GPU Batching

Kleine Tiles SOLLEN zu GPU-Batches zusammengefasst werden können.

---

## 259. GPU Memory Budget

GPU-Verarbeitung MUSS ein Speicherbudget berücksichtigen.

---

## 260. NPU Runtime

Pathology Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0006` für AI-Inferenz integrieren können.

---

## 261. AI Accelerator Selection

Medical-AI-Workloads SOLLEN dynamisch zwischen:

```text
CPU
GPU
NPU
```

verteilt werden können.

---

## 262. Distributed Compute

Pathology Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0007` integrieren.

---

## 263. Distributed Tile Processing

Whole Slide Images SOLLEN über mehrere Compute Nodes verarbeitet werden können.

---

## 264. Distributed Case Processing

Unabhängige Fälle SOLLEN auf unterschiedliche Compute Nodes verteilt werden können.

---

## 265. Data Locality

Der Scheduler SOLL Datenlokalität berücksichtigen.

---

## 266. Deterministic Aggregation

Verteilte Teilresultate SOLLEN deterministisch aggregierbar sein.

---

## 267. Node Failure

Der Ausfall eines Compute Nodes SOLL einzelne Tasks wiederholbar machen.

---

## 268. HPC Integration

Pathology Compute SOLL `NPSPEC-COMPUTE-HPC-0001` integrieren.

---

## 269. Cohort Processing

Große Pathologie-Kohorten SOLLEN als HPC-Workloads verarbeitet werden können.

---

## 270. Batch Processing

Ein Batch wird als:

`PathologyBatchJob`

repräsentiert.

---

## 271. Batch Inputs

Ein Batch DARF enthalten:

```text
MultipleCases
MultipleSlides
MultipleRegions
MultipleStains
MultipleAnalysisConfigurations
```

---

## 272. Batch Isolation

Ergebnisse verschiedener Fälle MÜSSEN eindeutig getrennt bleiben.

---

## 273. Result

Ein allgemeines Analyseergebnis wird als:

`PathologyResult`

repräsentiert.

---

## 274. Result Types

Ergebnisse können unter anderem sein:

```text
Mask
Annotation
CellSet
FeatureVector
Measurement
SpatialGraph
QualityMap
Classification
Count
DensityMap
```

---

## 275. Result Identity

Jedes Ergebnis MUSS eindeutig identifizierbar sein.

---

## 276. Result Source

Jedes Ergebnis MUSS seine Quelldaten referenzieren.

---

## 277. Result Region

Regionale Ergebnisse MÜSSEN ihre analysierte Region referenzieren.

---

## 278. Result Version

Ergebnisse SOLLEN versionierbar sein.

---

## 279. Result Status

Mindestens folgende Zustände SOLLEN unterstützt werden:

```text
VALID
PARTIAL
INVALID
FAILED
CANCELLED
```

---

## 280. No Silent Failure

Ein fehlgeschlagenes Analyseergebnis DARF nicht als gültiges leeres Ergebnis ausgegeben werden.

---

## 281. Feature Provenance

Jedes quantitative Feature MUSS seine Berechnungsherkunft nachvollziehbar machen können.

---

## 282. Provenance Record

Ein:

`PathologyProvenanceRecord`

SOLL mindestens enthalten:

```text
CaseId
SpecimenId
SlideId
SourceImage
SourceHash
Region
PyramidLevel
PhysicalResolution
Stain
Preprocessing
Algorithm
AlgorithmVersion
Parameters
RuntimeVersion
Backend
Timestamp
```

---

## 283. AI Provenance

Bei AI-Verarbeitung SOLLEN zusätzlich gespeichert werden:

```text
ModelId
ModelVersion
ModelHash
InferenceBackend
Confidence
```

---

## 284. Tile Provenance

Tile-basierte Resultate SOLLEN ihre Slide-Koordinaten und Pyramid Level dokumentieren.

---

## 285. Aggregation Provenance

Aggregierte Whole-Slide-Ergebnisse MÜSSEN die zugrunde liegenden Teilresultate beziehungsweise deren Konfiguration referenzieren können.

---

## 286. Deterministic Mode

Pathology Compute MUSS einen deterministischen Modus unterstützen können.

---

## 287. Deterministic Tile Order

Im deterministischen Modus SOLL die Ergebnisaggregation unabhängig von paralleler Task-Reihenfolge reproduzierbar sein.

---

## 288. Random Seeds

Stochastische Algorithmen MÜSSEN im deterministischen Modus explizite Seeds verwenden.

---

## 289. Backend Compatibility

CPU-, GPU- und NPU-Ergebnisse SOLLEN innerhalb definierter Toleranzen vergleichbar sein, sofern der Algorithmus dies ermöglicht.

---

## 290. Numerical Precision

Die verwendete numerische Präzision MUSS dokumentierbar sein.

---

## 291. Quantization

Quantisierte AI-Modelle MÜSSEN als solche identifizierbar sein.

---

## 292. Quality Control

Pathology Compute MUSS technische Qualitätskontrolle unterstützen.

---

## 293. Slide Quality

Ein Slide SOLL einen strukturierten Qualitätsstatus besitzen können.

---

## 294. Quality Dimensions

Mögliche Qualitätsdimensionen umfassen:

```text
Focus
TissueCoverage
Color
Exposure
Artifacts
Compression
ScanningCompleteness
```

---

## 295. Quality State

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

## 296. Unknown Is Not Good

`UNKNOWN` DARF nicht automatisch als `GOOD` behandelt werden.

---

## 297. Quality Map

Lokale Qualitätsinformationen SOLLEN als:

`PathologyQualityMap`

repräsentiert werden.

---

## 298. Analysis Eligibility

Eine Analysekonfiguration SOLL technische Mindestqualitätsanforderungen definieren können.

---

## 299. Quality Failure

Unzureichende Bildqualität MUSS diagnostizierbar sein.

---

## 300. Security

Pathology Compute MUSS in die NovaOS-Sicherheitsarchitektur integriert werden.

---

## 301. Authentication

Aufrufende Benutzer und Dienste MÜSSEN authentifizierbar sein.

---

## 302. Authorization

Zugriff auf Pathologie-Daten MUSS autorisierbar sein.

---

## 303. Data Minimization

Analysejobs SOLLEN nur benötigte Daten erhalten.

---

## 304. Decoder Security

Bilddecoder SOLLEN nach Möglichkeit mit minimalen Berechtigungen ausgeführt werden.

---

## 305. Resource Limits

Decoder und Analysejobs MÜSSEN Ressourcenlimits unterstützen können.

---

## 306. Privacy

Pathologie-Daten und daraus abgeleitete Merkmale MÜSSEN als potenziell sensible medizinische Daten behandelt werden.

---

## 307. Feature Privacy

Auch quantitative Feature-Vektoren DÜRFEN nicht automatisch als anonym betrachtet werden.

---

## 308. Temporary Data

Temporäre Tiles, Masken und Zwischenresultate SOLLEN nach definierter Policy entfernt werden können.

---

## 309. Logging Privacy

Logs DÜRFEN keine unnötigen medizinischen Inhalte enthalten.

---

## 310. Audit

Relevante Pathology-Operationen SOLLEN auditierbar sein.

---

## 311. Audit Events

Mindestens folgende Ereignisse SOLLEN unterstützt werden:

```text
PATHOLOGY_DATA_ACCESS
SLIDE_OPEN
ANALYSIS_START
ANALYSIS_COMPLETE
ANALYSIS_FAILED
ANNOTATION_CREATE
ANNOTATION_CHANGE
AI_RESULT_REVIEW
RESULT_EXPORT
```

---

## 312. Diagnostics

Pathology Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 313. Diagnostic Classes

Mindestens folgende Klassen SOLLEN existieren:

```text
INVALID_IMAGE
INVALID_WSI
UNSUPPORTED_FORMAT
DECODER_FAILURE
MALFORMED_IMAGE
INVALID_PYRAMID
INVALID_TILE
TILE_DECODE_FAILED
INVALID_PIXEL_SPACING
UNKNOWN_RESOLUTION
INVALID_COLOR_PROFILE
COLOR_CONVERSION_FAILED
STAIN_SEPARATION_FAILED
TISSUE_DETECTION_FAILED
ARTIFACT_DETECTION_FAILED
REGISTRATION_FAILED
SEGMENTATION_FAILED
CELL_DETECTION_FAILED
FEATURE_EXTRACTION_FAILED
INVALID_ANNOTATION
INVALID_REGION
OUT_OF_MEMORY
GPU_OUT_OF_MEMORY
BACKEND_FAILURE
AI_MODEL_FAILURE
LOW_IMAGE_QUALITY
CANCELLED
```

---

## 314. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 315. Tile-Level Diagnostic

Fehler einzelner Tiles SOLLEN separat diagnostizierbar sein.

---

## 316. Region-Level Diagnostic

Fehler einzelner Regionen SOLLEN separat diagnostizierbar sein.

---

## 317. Partial Failure Policy

Eine Analysekonfiguration SOLL definieren können:

```text
FAIL_JOB
SKIP_TILE
SKIP_REGION
MARK_PARTIAL
```

---

## 318. Profiling

Pathology Compute MUSS Profiling unterstützen.

---

## 319. Profiling Stages

Mindestens folgende Bereiche SOLLTEN separat messbar sein:

```text
MetadataRead
TileDecode
ColorConversion
TissueDetection
ArtifactDetection
Normalization
Registration
Segmentation
CellDetection
FeatureExtraction
SpatialAnalysis
Aggregation
AIInference
Export
```

---

## 320. Tile Decode Throughput

Dekodierte Pixel beziehungsweise Tiles pro Sekunde SOLLEN messbar sein.

---

## 321. Analysis Throughput

Analysierte Gewebefläche pro Zeiteinheit SOLL messbar sein.

---

## 322. Cell Throughput

Analysierte Zellen pro Sekunde SOLLEN messbar sein.

---

## 323. Peak Memory

Peak Memory SOLL messbar sein.

---

## 324. GPU Utilization

GPU-Auslastung SOLL messbar sein, sofern verfügbar.

---

## 325. Cache Metrics

Mindestens folgende Tile-Cache-Metriken SOLLEN verfügbar sein:

```text
Hits
Misses
Evictions
BytesUsed
```

---

## 326. Progress

Langlaufende Whole-Slide-Analysen MÜSSEN Fortschritt melden können.

---

## 327. Progress Units

Fortschritt SOLL beispielsweise auf:

```text
Tiles
Regions
TissueArea
AnalysisTasks
```

basieren können.

---

## 328. Testing — WSI

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
SmallSlide
LargeSlide
VeryLargeSlide
SingleLevel
MultiLevelPyramid
IrregularDimensions
PartialTiles
```

---

## 329. Testing — Tile Processing

Mindestens:

```text
ValidTile
BoundaryTile
PartialBoundaryTile
InvalidTile
ParallelTiles
CacheHit
CacheMiss
```

MÜSSEN getestet werden.

---

## 330. Testing — Coordinates

Transformationen zwischen:

```text
Tile
PyramidLevel
LevelZero
PhysicalSlideCoordinates
```

MÜSSEN getestet werden.

---

## 331. Testing — Color

Mindestens:

```text
RGB
Grayscale
16Bit
ColorProfile
ColorNormalization
```

SOLLTEN getestet werden.

---

## 332. Testing — Stain Processing

Mindestens:

```text
H&E
IHC
ColorDeconvolution
MultipleStains
InvalidStainMatrix
```

SOLLTEN getestet werden.

---

## 333. Testing — Tissue Detection

Mindestens:

```text
FullTissue
NoTissue
SparseTissue
MultipleFragments
BackgroundOnly
```

MÜSSEN getestet werden.

---

## 334. Testing — Artifact Detection

Mindestens:

```text
SharpImage
Blur
Fold
Bubble
PenMark
ScannerArtifact
```

SOLLTEN getestet werden.

---

## 335. Testing — Segmentation

Mindestens:

```text
TissueSegmentation
NucleusSegmentation
CellSegmentation
MultiClassSegmentation
EmptyRegion
BoundaryObjects
```

SOLLTEN getestet werden.

---

## 336. Testing — Cell Analysis

Mindestens:

```text
SingleCell
MultipleCells
OverlappingCells
NoCells
MultipleNuclei
BoundaryCell
```

SOLLTEN getestet werden.

---

## 337. Testing — Morphometry

Geometrische Features MÜSSEN gegen synthetische Objekte mit bekannten Eigenschaften getestet werden.

---

## 338. Testing — Spatial Analysis

Nearest-Neighbor-, Radius- und Graphberechnungen SOLLEN gegen analytisch bekannte Punktmengen getestet werden.

---

## 339. Testing — TMA

Mindestens:

```text
CompleteGrid
MissingCore
DamagedCore
IrregularCore
CoreMapping
```

SOLLTEN getestet werden.

---

## 340. Testing — Registration

Mindestens:

```text
RigidRegistration
AffineRegistration
DeformableRegistration
MultiStainRegistration
RegistrationFailure
```

SOLLTEN getestet werden.

---

## 341. Testing — AI

Mindestens:

```text
ValidModel
InvalidModel
LowConfidence
InferenceFailure
CPUInference
GPUInference
NPUInference
```

SOLLTEN getestet werden.

---

## 342. Testing — Out-of-Core

Sehr große Slides MÜSSEN mit begrenztem Arbeitsspeicher verarbeitet werden können.

---

## 343. Testing — Determinism

Identische Eingaben und Konfigurationen MÜSSEN im deterministischen Modus reproduzierbare Ergebnisse erzeugen.

---

## 344. Testing — Distributed Compute

Mindestens:

```text
DistributedTiles
DistributedSlides
NodeFailure
TaskRetry
DeterministicAggregation
```

SOLLTEN getestet werden.

---

## 345. Testing — Security

Mindestens folgende Fälle SOLLTEN getestet werden:

```text
MalformedFile
OversizedMetadata
DecompressionBomb
UnauthorizedAccess
DecoderCrash
ResourceExhaustion
```

---

## 346. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
GigapixelSlide
TerabyteScaleDataset
ThousandsOfSlides
MillionsOfCells
LargeTMA
ManyChannels
LargeZStack
HighTileConcurrency
GPUMemoryPressure
CPUOversubscription
CachePressure
DistributedNodeFailure
LongRunningBatch
CancellationUnderLoad
```

---

## 347. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
PathologyCase
PathologySpecimen
PathologySlide
PathologySlideImage
WholeSlideImage

Image Pyramid
Pyramid Levels
64-Bit Slide Coordinates

Tile Requests
Tile Decoding
Tile Cache
Region Processing
Out-of-Core Processing

RGB
Grayscale
Physical Pixel Resolution

Annotations
Tissue Detection
Tissue Mask

Basic Color Processing
H&E Processing
Color Deconvolution

Tissue Segmentation
Cell Detection
Nucleus Segmentation

Cell Counting
Cell Density
Basic Morphometry
Intensity Features

PathologyFeature
PathologyFeatureVector

CPU Execution
Multi-Core Processing

Scheduler Integration
Memory Model Integration
Object Model Integration

DICOM Integration Architecture
Medical AI Integration Architecture

Provenance
Quality Control
Diagnostics
Profiling
Cancellation
Security
```

---

## 348. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Fluorescence
Multi-Channel Imaging
Multiplex Imaging
IHC

Color Normalization
Advanced Stain Separation

Artifact Detection
Focus Maps
Z-Stacks
Focus Fusion

Multi-Stain Registration
Serial Section Registration
Deformable Registration

Cell Segmentation
Cytoplasm Segmentation
Gland Segmentation
Vessel Segmentation

Cell Classification
Spatial Analysis
Neighborhood Analysis
Spatial Graphs

Tissue Microarrays

Pathomics
Texture Analysis
Radiomics Integration

GPU Acceleration
NPU Acceleration
Distributed Compute
HPC Cohort Processing

AI Tissue Classification
AI Cell Classification
AI Segmentation
AI Quality Control

DICOM WSI
PACS Integration

Advanced Provenance
Deterministic Distributed Processing
```

---

## 349. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-PATHOLOGY-0001` MUSS:

1. pathologische Fälle repräsentieren können.
2. Präparate repräsentieren können.
3. Slides repräsentieren können.
4. digitale Slide-Bilder repräsentieren.
5. Whole Slide Images unterstützen.
6. sehr große Bilddimensionen sicher repräsentieren.
7. mindestens 64-Bit-Slide-Koordinaten unterstützen.
8. Multi-Resolution-Pyramiden unterstützen.
9. Pyramid Levels eindeutig identifizieren.
10. Downsample-Faktoren verwalten.
11. physikalische Pixelauflösung verwalten.
12. Tile-basierte Verarbeitung unterstützen.
13. Tile Requests repräsentieren.
14. Teilregionen dekodieren können.
15. vollständige WSI-Dekodierung im RAM nicht voraussetzen.
16. Out-of-Core-Verarbeitung unterstützen.
17. Tile Caching unterstützen.
18. Cache-Speicher begrenzen können.
19. Regionen repräsentieren.
20. Regionskoordinatensysteme explizit verwalten.
21. Annotationen repräsentieren.
22. Annotationen eindeutig identifizieren.
23. Annotation Source dokumentieren.
24. RGB-Bilddaten unterstützen.
25. Graustufenbilder unterstützen.
26. physikalische Auflösung von nominaler Vergrößerung unterscheiden.
27. Farbräume verwalten können.
28. Färbungsinformationen verwalten.
29. H&E-Verarbeitung unterstützen.
30. Color Deconvolution architektonisch unterstützen.
31. verwendete Stain Matrices dokumentieren.
32. Preprocessing-Konfigurationen versionieren.
33. Gewebe erkennen können.
34. Hintergrundbereiche erkennen können.
35. Tissue Masks erzeugen können.
36. leere Tiles überspringen können.
37. technische Bildqualität repräsentieren.
38. Fokusqualität architektonisch bewerten können.
39. Gewebesegmentierung unterstützen.
40. Zellobjekte repräsentieren.
41. Zellkerne repräsentieren.
42. Kernsegmentierung unterstützen.
43. Zellen räumlich lokalisieren.
44. Zellzählungen unterstützen.
45. Zelldichte berechnen können.
46. morphometrische Features unterstützen.
47. Intensitätsfeatures unterstützen.
48. Features eindeutig identifizieren.
49. Feature-Definitionen versionieren.
50. Feature-Vektoren repräsentieren.
51. räumliche Analysen architektonisch unterstützen.
52. CPU-Ausführung unterstützen.
53. Multi-Core-Verarbeitung unterstützen.
54. GPU-Verarbeitung architektonisch unterstützen.
55. NPU-Verarbeitung architektonisch unterstützen.
56. den Compute Scheduler integrieren.
57. das Compute Memory Model integrieren.
58. das Compute Object Model integrieren.
59. begrenzte Speicherbudgets unterstützen.
60. kontrollierten Abbruch unterstützen.
61. Teilresultate von vollständigen Resultaten unterscheiden.
62. Medical AI Compute architektonisch integrieren.
63. AI-Ergebnisse als abgeleitete Daten kennzeichnen.
64. AI-Modellidentität dokumentieren können.
65. AI-Modellversion dokumentieren können.
66. AI-Konfidenz erhalten können.
67. AI-Ergebnisse nicht automatisch als Ground Truth behandeln.
68. DICOM Compute architektonisch integrieren.
69. DICOM WSI architektonisch unterstützen.
70. PACS Compute architektonisch integrieren.
71. Originaldaten unverändert erhalten.
72. herstellerspezifische Decoder über standardisierte Schnittstellen integrieren können.
73. fehlerhafte Bilddateien kontrolliert behandeln.
74. Decoder-Ressourcen begrenzen können.
75. vollständige Provenance unterstützen.
76. Quelldaten referenzieren.
77. Slide-Identität dokumentieren.
78. Region dokumentieren.
79. Pyramid Level dokumentieren.
80. physikalische Auflösung dokumentieren.
81. Färbung dokumentieren.
82. Preprocessing dokumentieren.
83. Algorithmusversion dokumentieren.
84. Runtime-Version dokumentieren.
85. Compute Backend dokumentieren.
86. einen deterministischen Modus unterstützen können.
87. strukturierte Diagnosen bereitstellen.
88. Tile-Level-Fehler diagnostizieren können.
89. Profiling unterstützen.
90. Fortschritt großer Analysen melden.
91. technische Qualitätskontrolle unterstützen.
92. unbekannte Qualität nicht automatisch als gute Qualität behandeln.
93. sensible medizinische Daten schützen.
94. Audit architektonisch unterstützen.
95. quantitative Bildanalyse von medizinischer Interpretation trennen.
96. Erweiterungen ohne Änderung des grundlegenden Pathology-Datenmodells ermöglichen.

---

## 350. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaPathologyCompute
 |
 +-- PathologyRuntime
 |
 +-- PathologyDataModel
 |
 +-- SpecimenModel
 |
 +-- SlideManager
 |
 +-- WSIReader
 |
 +-- DecoderRegistry
 |
 +-- PyramidManager
 |
 +-- TileEngine
 |
 +-- TileCache
 |
 +-- RegionEngine
 |
 +-- AnnotationEngine
 |
 +-- ColorEngine
 |
 +-- StainEngine
 |
 +-- TissueDetector
 |
 +-- ArtifactDetector
 |
 +-- FocusAnalyzer
 |
 +-- RegistrationEngine
 |
 +-- TissueSegmentationEngine
 |
 +-- CellDetectionEngine
 |
 +-- NucleusSegmentationEngine
 |
 +-- CellSegmentationEngine
 |
 +-- MorphometryEngine
 |
 +-- FeatureEngine
 |
 +-- SpatialAnalysisEngine
 |
 +-- SpatialGraphEngine
 |
 +-- TMAEngine
 |
 +-- PathomicsEngine
 |
 +-- QualityEngine
 |
 +-- MedicalAIBridge
 |
 +-- DICOMBridge
 |
 +-- PACSBridge
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

## 351. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt die gemeinsame Ausführungsinfrastruktur bereit.

```text
Pathology Compute
       |
       v
Compute Runtime
       |
 +-----+-----+-----+
 |     |     |     |
 v     v     v     v
CPU   GPU   NPU  Distributed
```

---

## 352. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert insbesondere:

```text
Tile Decode
Preprocessing
Tissue Detection
Segmentation
Cell Detection
Feature Extraction
AI Inference
Spatial Analysis
Aggregation
```

---

## 353. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet:

```text
Tile Buffers
Region Buffers
Masks
Channels
Cell Objects
Feature Buffers
Spatial Graphs
Temporary Workspaces
```

---

## 354. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Pathology-Objekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 355. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

GPU Runtime beschleunigt insbesondere:

```text
Image Filtering
Color Processing
Stain Separation
Segmentation
Registration
Feature Extraction
AI Inference
```

---

## 356. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

NPU Runtime kann insbesondere:

```text
Tissue Detection
Cell Detection
Cell Classification
Segmentation
Quality Assessment
```

beschleunigen.

---

## 357. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Distributed Compute ermöglicht die Verarbeitung sehr großer Slide-Mengen und Kohorten.

---

## 358. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

HPC Compute stellt Ressourcen für großskalige Pathologie-, Pathomics- und AI-Analysen bereit.

---

## 359. Beziehung zu `NPSPEC-COMPUTE-DICOM-0001`

DICOM Compute stellt standardisierte medizinische Bild-, Präparat- und Metadatenstrukturen bereit.

---

## 360. Beziehung zu `NPSPEC-COMPUTE-PACS-0001`

PACS Compute stellt pathologische Bilddaten aus kompatiblen medizinischen Archiven bereit.

---

## 361. Beziehung zu `NPSPEC-COMPUTE-MEDAI-0001`

Medical AI Compute stellt KI-basierte:

```text
Detection
Segmentation
Classification
Feature Extraction
Quality Assessment
```

bereit.

---

## 362. Beziehung zu `NPSPEC-COMPUTE-RADIOMICS-0001`

Radiomics Compute kann standardisierte quantitative Bildmerkmale für pathologische Regionen bereitstellen.

Pathologie-spezifische Erweiterungen werden innerhalb Pathology Compute als Pathomics behandelt.

---

## 363. Grundlegende Whole-Slide-Architektur

```text
Whole Slide Image
       |
       v
Metadata Reader
       |
       v
Image Pyramid
       |
       v
Tile Engine
       |
       +----------------------+
       |                      |
       v                      v
Interactive Tiles        Analysis Tiles
       |                      |
       v                      v
Visualization           Compute Pipeline
```

---

## 364. Analysearchitektur

```text
Whole Slide Image
       |
       v
Tissue Detection
       |
       v
Relevant Tiles
       |
       v
Preprocessing
       |
       v
Segmentation
       |
   +---+---+
   |       |
   v       v
Cells    Tissue
   |       |
   +---+---+
       |
       v
Feature Extraction
       |
       v
Spatial Analysis
       |
       v
Quantitative Result
```

---

## 365. Zellmodell

```text
Tissue
 |
 +-- Cell
 |    |
 |    +-- Nucleus
 |    |
 |    +-- Cytoplasm
 |
 +-- Cell
      |
      +-- Nucleus
```

Das Modell DARF keine biologisch unzulässigen Vereinfachungen als universelle Struktur erzwingen.

---

## 366. Multi-Stain-Architektur

```text
Section A -- H&E
      |
      +------+
             |
Section B -- IHC
      |      |
      +------+
             |
             v
        Registration
             |
             v
       Spatial Alignment
             |
             v
      Combined Analysis
```

---

## 367. AI-Architektur

```text
WSI
 |
 v
Tissue Detection
 |
 v
Tile Selection
 |
 v
AI Inference
 |
 +------------------+
 |        |         |
 v        v         v
Cells   Tissue   Quality
 |        |         |
 +--------+---------+
          |
          v
     Structured Result
```

---

## 368. Spatial-Analysis-Architektur

```text
Detected Cells
      |
      v
Cell Positions
      |
      +------------------+
      |        |         |
      v        v         v
Nearest     Radius     Graph
Neighbor  Neighborhood Analysis
      |        |         |
      +--------+---------+
               |
               v
        Spatial Features
```

---

## 369. Datenmodellgrundsatz

Pathology Compute MUSS zwischen:

```text
Case
Specimen
Block
Section
Slide
Slide Image
Image Pyramid
Tile
Region
Annotation
Segmentation
Cell
Nucleus
Feature
Result
```

unterscheiden.

---

## 370. Whole-Slide-Grundsatz

Ein Whole Slide Image ist nicht lediglich ein sehr großes normales Bild.

Es MUSS als:

```text
Large Spatial Dataset
+
Multi-Resolution Pyramid
+
Tile Address Space
+
Physical Geometry
+
Metadata
```

behandelt werden.

---

## 371. Tile-Grundsatz

Ein Tile ist eine temporäre beziehungsweise adressierbare Ansicht auf einen Bereich eines Slides.

Ein Tile DARF nicht ohne Slide-, Level- und Koordinateninformation als eigenständige räumliche Quelle behandelt werden.

---

## 372. Auflösungsgrundsatz

Quantitative Merkmale MÜSSEN ihre räumliche Auflösung berücksichtigen.

Features aus unterschiedlichen Auflösungen DÜRFEN nicht automatisch als direkt vergleichbar behandelt werden.

---

## 373. Färbungsgrundsatz

Bildintensitäten und Farben hängen wesentlich von Färbung, Präparation, Scanner und Bildverarbeitung ab.

Daher gilt:

```text
Color Value != Universal Biological Measurement
```

ohne einen definierten Mess- und Kalibrierungskontext.

---

## 374. Zellgrundsatz

Eine erkannte Zelle ist ein abgeleitetes Analyseobjekt.

Sie MUSS mindestens auf:

```text
Source Slide
Region
Detection Method
Segmentation
Position
Version
```

zurückführbar sein.

---

## 375. AI-Grundsatz

KI-basierte Pathologieanalyse MUSS zwischen:

```text
Image Data
AI Prediction
Human Annotation
Reviewed Result
Derived Measurement
Clinical Interpretation
```

unterscheiden.

---

## 376. Reproduzierbarkeitsgrundsatz

Ein quantitatives Pathology-Ergebnis SOLL mindestens auf folgende Informationen zurückführbar sein:

```text
Source Image
Source Hash
Slide
Region
Pyramid Level
Physical Resolution
Stain
Color Processing
Preprocessing
Segmentation
Algorithm
Algorithm Version
Parameters
Runtime
Backend
```

---

## 377. Skalierungsarchitektur

Die bevorzugte Skalierungsstrategie lautet:

```text
Small Image
    |
    v
Direct Processing


Whole Slide Image
    |
    v
Tile Processing
    |
    v
Multi-Core / GPU


Large Slide Collection
    |
    v
Distributed Compute / HPC
```

---

## 378. Erweiterbarkeit

Neue:

```text
WSI Formats
Decoders
Stains
Markers
Image Channels
Segmentation Methods
Cell Models
Feature Families
Spatial Analyses
AI Models
Scanner Integrations
Compute Backends
```

SOLLTEN ohne Änderung des grundlegenden Pathology-Objektmodells integrierbar sein.

---

## 379. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-PATHOLOGY-DATA-0001
Pathology Data Model

NPSPEC-COMPUTE-PATHOLOGY-WSI-0001
Whole Slide Imaging

NPSPEC-COMPUTE-PATHOLOGY-PYRAMID-0001
Whole Slide Image Pyramid

NPSPEC-COMPUTE-PATHOLOGY-TILE-0001
Pathology Tile Runtime

NPSPEC-COMPUTE-PATHOLOGY-DECODER-0001
Whole Slide Image Decoder Interface

NPSPEC-COMPUTE-PATHOLOGY-COLOR-0001
Pathology Color Management

NPSPEC-COMPUTE-PATHOLOGY-STAIN-0001
Histology Stain Processing

NPSPEC-COMPUTE-PATHOLOGY-DECONV-0001
Color Deconvolution

NPSPEC-COMPUTE-PATHOLOGY-NORMALIZE-0001
Stain and Color Normalization

NPSPEC-COMPUTE-PATHOLOGY-TISSUE-0001
Tissue Detection

NPSPEC-COMPUTE-PATHOLOGY-ARTIFACT-0001
Pathology Artifact Detection

NPSPEC-COMPUTE-PATHOLOGY-FOCUS-0001
Focus and Image Quality

NPSPEC-COMPUTE-PATHOLOGY-REGISTRATION-0001
Pathology Image Registration

NPSPEC-COMPUTE-PATHOLOGY-SEGMENT-0001
Pathology Segmentation

NPSPEC-COMPUTE-PATHOLOGY-CELL-0001
Cell Detection and Representation

NPSPEC-COMPUTE-PATHOLOGY-NUCLEUS-0001
Nucleus Segmentation

NPSPEC-COMPUTE-PATHOLOGY-MORPHOMETRY-0001
Cell and Tissue Morphometry

NPSPEC-COMPUTE-PATHOLOGY-PATHOMICS-0001
Pathomics Feature Extraction

NPSPEC-COMPUTE-PATHOLOGY-SPATIAL-0001
Spatial Pathology Analysis

NPSPEC-COMPUTE-PATHOLOGY-GRAPH-0001
Pathology Spatial Graphs

NPSPEC-COMPUTE-PATHOLOGY-TMA-0001
Tissue Microarray Processing

NPSPEC-COMPUTE-PATHOLOGY-MULTIPLEX-0001
Multiplex Pathology Imaging

NPSPEC-COMPUTE-PATHOLOGY-AI-0001
Pathology Medical AI Integration

NPSPEC-COMPUTE-PATHOLOGY-QUALITY-0001
Pathology Quality Control

NPSPEC-COMPUTE-PATHOLOGY-PROVENANCE-0001
Pathology Provenance

NPSPEC-COMPUTE-PATHOLOGY-TEST-0001
Pathology Conformance Tests
```

---

## 380. Zusammenfassung

`NPSPEC-COMPUTE-PATHOLOGY-0001` definiert die gemeinsame Infrastruktur für digitale Pathologie innerhalb der NovaOS-Compute-Architektur.

Die grundlegende Verarbeitung lautet:

```text
Pathology Case
      |
      v
Specimen
      |
      v
Slide
      |
      v
Whole Slide Image
      |
      v
Image Pyramid
      |
      v
Tile Processing
      |
      v
Tissue Detection
      |
      v
Segmentation
      |
      v
Cell / Tissue Analysis
      |
      v
Feature Extraction
      |
      v
Quantitative Result
```

Die Architektur unterstützt:

```text
Whole Slide Imaging
Multi-Resolution Pyramids
Tile Processing
Out-of-Core Processing
Histology
Cytology
H&E
IHC
Fluorescence
Multiplex Imaging
Tissue Detection
Artifact Detection
Color Normalization
Stain Separation
Registration
Tissue Segmentation
Cell Detection
Nucleus Segmentation
Morphometry
Pathomics
Spatial Analysis
Tissue Microarrays
Quality Control
```

Große Datenmengen können über:

```text
CPU
SIMD
Multi-Core
GPU
NPU
Distributed Compute
HPC
```

verarbeitet werden.

Medical AI kann unter anderem:

```text
Tissue Detection
Cell Detection
Nucleus Segmentation
Cell Classification
Tissue Classification
Quality Assessment
```

bereitstellen.

Die Architektur trennt konsequent:

```text
Original Slide
Image Pyramid
Tile
Region
Annotation
Segmentation
Cell Object
Quantitative Feature
AI Prediction
Reviewed Result
Clinical Interpretation
```

voneinander.

Jedes quantitative Ergebnis bleibt auf:

```text
Case
Specimen
Slide
Source Image
Region
Pyramid Level
Physical Resolution
Stain
Preprocessing
Algorithm
Algorithm Version
Runtime
Backend
```

zurückführbar.

Die zentrale Skalierungsregel lautet:

> Whole Slide Images werden nicht als gewöhnliche vollständig zu ladende Rasterbilder behandelt, sondern als sehr große, räumlich kalibrierte, mehrstufige und Tile-adressierbare Datensätze.

Die zentrale Reproduzierbarkeitsregel lautet:

> Ein pathologisches Analyseergebnis besitzt nur dann eine eindeutige technische Bedeutung, wenn neben seinem numerischen oder strukturellen Ergebnis auch Slide, Region, räumliche Auflösung, Färbung, Vorverarbeitung, Algorithmus, Version und Berechnungskontext bekannt sind.

Das grundlegende Architekturprinzip lautet:

> NovaOS Pathology Compute behandelt digitale Pathologie nicht als einfachen Bildverarbeitungsfall. Präparate, Slides, Whole Slide Images, Auflösungspyramiden, Tiles, Färbungen, Farbmodelle, Geweberegionen, Zellen, Kerne, Annotationen, Segmentierungen, räumliche Beziehungen und quantitative Merkmale werden als eigenständige, miteinander verknüpfte Compute-Objekte behandelt. Dadurch können Gigapixel- und größere Pathologiedaten speichereffizient über CPU, GPU, NPU, Distributed Compute und HPC verarbeitet und gleichzeitig reproduzierbar mit DICOM, Medical AI, Radiomics und weiteren NovaOS-Compute-Fähigkeiten verbunden werden, ohne technische Bildanalyse mit medizinischer Interpretation gleichzusetzen.