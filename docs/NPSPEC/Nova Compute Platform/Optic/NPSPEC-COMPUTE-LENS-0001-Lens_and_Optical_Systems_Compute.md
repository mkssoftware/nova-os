# NPSPEC-COMPUTE-LENS-0001 — Lens and Optical Systems Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Optics / Lens Systems  
**Komponente:** NovaOS Lens Compute  
**Bezeichner:** `NPSPEC-COMPUTE-LENS-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0001`, `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-RAYTRACING-0001`  
**Kurzname:** Lens Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Compute-Infrastruktur zur Modellierung, Analyse und Simulation von Linsen und optischen Systemen innerhalb der NovaOS-Compute-Architektur.

Lens Compute stellt standardisierte Funktionen für geometrische und physikalisch erweiterbare Optik bereit.

Die Komponente SOLL insbesondere folgende Anwendungsbereiche unterstützen:

- Einzellinsen,
- Mehrlinsensysteme,
- Kameraobjektive,
- Mikroskopoptiken,
- Teleskopoptiken,
- Projektionsoptiken,
- Endoskopoptiken,
- Sensoroptiken,
- industrielle Bildverarbeitung,
- Machine Vision,
- VR- und AR-Optiken,
- Head-Mounted Displays,
- LiDAR-Optiken,
- medizinische Bildgebung,
- optische Messsysteme,
- wissenschaftliche Instrumente,
- optisches Engineering,
- Lens Design,
- Ray Tracing durch optische Systeme,
- Aberrationsanalyse,
- Fokusberechnung,
- Depth of Field,
- Field of View,
- Distortion,
- Vignetting,
- MTF-Analyse,
- PSF-Analyse,
- chromatische Analyse,
- Toleranzanalyse.

Die grundlegende Architektur lautet:

```text
Optical System
      |
      v
Optical Elements
      |
      v
Surface Geometry
      |
      v
Material / Medium
      |
      v
Ray Generation
      |
      v
Surface Intersection
      |
      v
Refraction / Reflection
      |
      v
Ray Propagation
      |
      v
Sensor / Image Plane
      |
      v
Optical Analysis
```

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Ein optisches System wird als explizite Folge geometrischer Oberflächen, Medien, Aperturen, Transformationen und physikalischer Materialeigenschaften modelliert und nicht auf ein vereinfachtes Kamera-FOV-Modell reduziert.

---

## 3. Abstraktionsmodell

Lens Compute MUSS mindestens zwischen folgenden Ebenen unterscheiden:

```text
Optical System
Optical Element
Optical Surface
Optical Medium
Aperture
Ray
Interaction
Propagation
Image Plane
Analysis
```

---

## 4. Optical System

Ein vollständiges optisches System wird als:

`OpticalSystem`

repräsentiert.

---

## 5. System Identity

Jedes Optical System MUSS eindeutig identifizierbar sein.

---

## 6. System Version

Optical Systems SOLLEN versionierbar sein.

---

## 7. System Contents

Ein Optical System SOLL enthalten können:

```text
Elements
Surfaces
Media
Apertures
Stops
Sensor
CoordinateSystem
Environment
Configuration
```

---

## 8. Optical Element

Ein optisches Element wird als:

`OpticalElement`

repräsentiert.

---

## 9. Element Types

Mindestens folgende Elementtypen SOLLEN unterstützt werden können:

```text
Lens
Mirror
Window
Filter
Prism
BeamSplitter
Aperture
Stop
Sensor
Detector
CustomElement
```

---

## 10. Lens

Eine Linse wird als:

`OpticalLens`

repräsentiert.

---

## 11. Lens Composition

Eine Linse SOLL aus einer oder mehreren optischen Oberflächen bestehen können.

---

## 12. Lens Types

Mindestens folgende grundlegenden Linsentypen SOLLEN modellierbar sein:

```text
PlanoConvex
PlanoConcave
Biconvex
Biconcave
PositiveMeniscus
NegativeMeniscus
Aspheric
Cylindrical
Freeform
Custom
```

---

## 13. Compound Lens

Mehrere Linsenelemente SOLLEN zu einem Compound Lens System kombiniert werden können.

---

## 14. Cemented Elements

Verkittete Linsenelemente SOLLEN modellierbar sein.

---

## 15. Air-Spaced Elements

Luftgetrennte Linsenelemente SOLLEN modellierbar sein.

---

## 16. Optical Surface

Eine optische Oberfläche wird als:

`OpticalSurface`

repräsentiert.

---

## 17. Surface Identity

Jede optische Oberfläche MUSS eindeutig identifizierbar sein.

---

## 18. Surface Order

Die Reihenfolge optischer Oberflächen MUSS explizit definiert sein.

---

## 19. Surface Types

Mindestens folgende Oberflächenformen SOLLEN unterstützt werden:

```text
Plane
Sphere
Asphere
Cylinder
Conic
Toroid
Freeform
Mesh
Implicit
Custom
```

---

## 20. Spherical Surface

Eine sphärische Oberfläche MUSS mindestens definieren:

```text
Radius
Position
Orientation
Aperture
```

---

## 21. Radius Convention

Die Vorzeichenkonvention für Krümmungsradien MUSS eindeutig definiert sein.

---

## 22. Plane Surface

Eine plane Oberfläche MUSS explizit repräsentierbar sein.

---

## 23. Infinite Radius

Implementierungen DÜRFEN eine plane Oberfläche intern als Grenzfall unendlichen Radius behandeln.

Die öffentliche Semantik MUSS jedoch eindeutig bleiben.

---

## 24. Aspheric Surface

Asphärische Oberflächen SOLLEN unterstützt werden.

---

## 25. Asphere Equation

Eine rotationssymmetrische Asphäre SOLL durch eine Gleichung der Form:

```text
z(r) =
(c * r^2) /
(1 + sqrt(1 - (1 + k) * c^2 * r^2))
+
A4 * r^4
+
A6 * r^6
+
A8 * r^8
+
...
```

repräsentierbar sein.

Dabei gilt:

```text
c = Curvature
k = Conic Constant
An = Aspheric Coefficients
```

---

## 26. Conic Surfaces

Mindestens folgende Kegelschnittformen SOLLEN darstellbar sein:

```text
Sphere
Ellipse
Parabola
Hyperbola
```

---

## 27. Freeform Surface

Freiformflächen SOLLEN architektonisch unterstützt werden.

---

## 28. Freeform Representation

Freiformflächen DÜRFEN beispielsweise beschrieben werden durch:

```text
Polynomial
Zernike
Spline
NURBS
Mesh
HeightField
ImplicitFunction
CustomFunction
```

---

## 29. Cylindrical Surface

Zylindrische optische Flächen SOLLEN unterstützt werden.

---

## 30. Toroidal Surface

Toroidale optische Flächen SOLLEN unterstützt werden können.

---

## 31. Surface Transform

Jede Oberfläche SOLL eine eigene Transformation besitzen können.

---

## 32. Surface Coordinate System

Oberflächen SOLLEN in lokalen Koordinatensystemen definiert werden können.

---

## 33. Global Coordinates

Die Runtime MUSS Transformationen zwischen lokalen und globalen Koordinaten unterstützen.

---

## 34. Optical Medium

Ein optisches Medium wird als:

`OpticalMedium`

repräsentiert.

---

## 35. Medium Properties

Ein Medium SOLL mindestens enthalten können:

```text
RefractiveIndex
Dispersion
Absorption
Transmission
WavelengthRange
Temperature
Pressure
```

---

## 36. Refractive Index

Der Brechungsindex MUSS wellenlängenabhängig modellierbar sein.

---

## 37. Constant Index

Ein konstanter Brechungsindex SOLL für vereinfachte Simulationen unterstützt werden.

---

## 38. Dispersion

Chromatische Dispersion MUSS architektonisch unterstützt werden.

---

## 39. Dispersion Models

Mindestens folgende Modelle SOLLEN unterstützt werden können:

```text
Constant
Abbe
Cauchy
Sellmeier
Tabulated
Custom
```

---

## 40. Sellmeier Equation

Die Sellmeier-Gleichung SOLL unterstützt werden können:

```text
n^2(lambda) =
1
+
B1 * lambda^2 / (lambda^2 - C1)
+
B2 * lambda^2 / (lambda^2 - C2)
+
B3 * lambda^2 / (lambda^2 - C3)
```

---

## 41. Material Database

Optische Materialien SOLLEN über eine Materialdatenbank referenzierbar sein.

---

## 42. Material Identity

Jedes Material MUSS eindeutig identifizierbar sein.

---

## 43. Material Version

Materialdaten SOLLEN versionierbar sein.

---

## 44. Environmental Dependence

Materialeigenschaften SOLLEN abhängig sein können von:

```text
Temperature
Pressure
Humidity
Wavelength
```

---

## 45. Air Model

Luft SOLL als explizites optisches Medium modellierbar sein.

---

## 46. Vacuum

Vakuum MUSS als Medium unterstützt werden.

---

## 47. Interface

Der Übergang zwischen zwei optischen Medien wird als:

`OpticalInterface`

repräsentiert.

---

## 48. Incident Medium

Jede refraktive Oberfläche MUSS das einfallende Medium bestimmen können.

---

## 49. Transmitted Medium

Jede refraktive Oberfläche MUSS das nachfolgende Medium bestimmen können.

---

## 50. Snell's Law

Brechung MUSS auf dem Snelliusschen Gesetz basieren können:

```text
n1 * sin(theta1) = n2 * sin(theta2)
```

---

## 51. Vector Refraction

Die Runtime SOLL vektorbasierte Brechungsberechnung unterstützen.

---

## 52. Reflection

Spiegelnde Reflexion MUSS unterstützt werden.

---

## 53. Reflection Law

Für ideale Spiegelung gilt:

```text
theta_incident = theta_reflected
```

---

## 54. Total Internal Reflection

Totalreflexion MUSS erkannt und korrekt behandelt werden.

---

## 55. Fresnel

Fresnel-Reflexions- und Transmissionskoeffizienten SOLLEN unterstützt werden.

---

## 56. Polarization

Polarisation SOLL architektonisch unterstützt werden.

---

## 57. Polarization State

Ein Ray SOLL optional Polarisationsinformationen tragen können.

---

## 58. Coating

Eine optische Beschichtung wird als:

`OpticalCoating`

repräsentiert.

---

## 59. Coating Types

Beschichtungen SOLLEN beispielsweise modelliert werden können als:

```text
AntiReflection
HighReflection
BandPass
LongPass
ShortPass
NeutralDensity
Custom
```

---

## 60. Multilayer Coating

Mehrschichtige Beschichtungen SOLLEN architektonisch unterstützt werden.

---

## 61. Coating Spectral Response

Reflexion und Transmission einer Beschichtung SOLLEN wellenlängenabhängig sein können.

---

## 62. Aperture

Eine Apertur wird als:

`OpticalAperture`

repräsentiert.

---

## 63. Aperture Types

Mindestens folgende Aperturformen SOLLEN unterstützt werden:

```text
Circular
Rectangular
Elliptical
Polygonal
Annular
Custom
```

---

## 64. Aperture Clipping

Rays außerhalb einer aktiven Apertur MÜSSEN korrekt blockiert beziehungsweise verworfen werden.

---

## 65. Aperture Stop

Ein Aperture Stop wird als:

`ApertureStop`

repräsentiert.

---

## 66. Stop Identification

Das Aperture Stop SOLL explizit definiert beziehungsweise automatisch bestimmbar sein.

---

## 67. Entrance Pupil

Die Eintrittspupille SOLL berechnet werden können.

---

## 68. Exit Pupil

Die Austrittspupille SOLL berechnet werden können.

---

## 69. Field Stop

Field Stops SOLLEN modellierbar sein.

---

## 70. Chief Ray

Ein Chief Ray wird als:

`ChiefRay`

repräsentiert.

---

## 71. Marginal Ray

Ein Marginal Ray wird als:

`MarginalRay`

repräsentiert.

---

## 72. Paraxial Ray

Paraxiale Rays SOLLEN unterstützt werden.

---

## 73. Real Ray

Exakte geometrische Rays SOLLEN von paraxialen Rays unterscheidbar sein.

---

## 74. Optical Ray

Ein optischer Ray wird als:

`OpticalRay`

repräsentiert.

---

## 75. Optical Ray Fields

Ein Optical Ray SOLL mindestens enthalten können:

```text
Origin
Direction
Wavelength
Intensity
Time
Polarization
Medium
PathLength
Status
```

---

## 76. Ray Wavelength

Jeder Ray SOLL eine Wellenlänge besitzen können.

---

## 77. Spectral Ray Bundle

Mehrere Wellenlängen SOLLEN gleichzeitig simuliert werden können.

---

## 78. Ray Bundle

Eine Menge optischer Rays wird als:

`OpticalRayBundle`

repräsentiert.

---

## 79. Ray Bundle Generation

Ray Bundles SOLLEN erzeugt werden können aus:

```text
ObjectPoint
FieldPoint
EntrancePupil
LightSource
SensorPixel
CustomSource
```

---

## 80. Forward Ray Tracing

Rays SOLLEN vom Objekt beziehungsweise Lichtsystem zum Sensor verfolgt werden können.

---

## 81. Reverse Ray Tracing

Rays SOLLEN vom Sensor durch das optische System zurückverfolgt werden können.

---

## 82. Sequential Ray Tracing

Lens Compute MUSS sequenzielles Ray Tracing unterstützen.

---

## 83. Sequential Traversal

Beim sequenziellen Ray Tracing werden Oberflächen in einer definierten optischen Reihenfolge durchlaufen.

---

## 84. Non-Sequential Ray Tracing

Nicht-sequenzielles Ray Tracing SOLL architektonisch unterstützt werden.

---

## 85. Non-Sequential Interaction

Ein Ray DARF bei nicht-sequenzieller Simulation beliebige kompatible optische Elemente treffen.

---

## 86. Ray Tracing Integration

Lens Compute SOLL `NPSPEC-COMPUTE-RAYTRACING-0001` integrieren.

---

## 87. Optical Ray Extension

Optische Rays SOLLEN auf der allgemeinen Ray-Repräsentation aufbauen beziehungsweise mit ihr interoperabel sein.

---

## 88. Surface Intersection

Jede optische Oberfläche MUSS einen Ray-Surface-Intersection-Test bereitstellen können.

---

## 89. Intersection Result

Ein:

`OpticalIntersection`

SOLL mindestens enthalten:

```text
Hit
Position
Normal
Distance
SurfaceId
IncidentMedium
TransmittedMedium
```

---

## 90. Closest Valid Intersection

Bei nicht-sequenzieller Simulation MUSS der nächste gültige Schnittpunkt bestimmbar sein.

---

## 91. Surface Normal

Die Oberflächennormale MUSS am tatsächlichen Schnittpunkt berechnet werden können.

---

## 92. Numerical Robustness

Ray-Surface-Intersection MUSS numerisch robust implementierbar sein.

---

## 93. Self Intersection

Selbstschnittprobleme nach Reflexion oder Brechung MÜSSEN kontrolliert verhindert werden.

---

## 94. Optical Path Length

Die optische Weglänge wird als:

`OpticalPathLength`

repräsentiert.

---

## 95. OPL

Die optische Weglänge SOLL berechnet werden als:

```text
OPL = integral(n(s) ds)
```

---

## 96. Geometric Path Length

Geometrische und optische Weglänge MÜSSEN unterscheidbar sein.

---

## 97. Optical Path Difference

Optical Path Difference wird als:

`OPD`

repräsentiert.

---

## 98. OPD Reference

OPD MUSS relativ zu einer definierten Referenz bestimmt werden.

---

## 99. Fermat Principle

Optimierungs- und Analyseverfahren DÜRFEN das Fermatsche Prinzip verwenden.

---

## 100. Paraxial Optics

Lens Compute MUSS grundlegende paraxiale Optik unterstützen.

---

## 101. ABCD Matrix

Paraxiale Systeme SOLLEN über ABCD-Matrizen modellierbar sein.

---

## 102. Ray Transfer Matrix

Eine paraxiale Transformation SOLL beschrieben werden können als:

```text
[y2]   [A B] [y1]
[u2] = [C D] [u1]
```

---

## 103. Thin Lens

Das Thin-Lens-Modell MUSS unterstützt werden.

---

## 104. Thin Lens Equation

Die dünne Linsengleichung lautet:

```text
1/f = 1/g + 1/b
```

mit:

```text
f = Focal Length
g = Object Distance
b = Image Distance
```

---

## 105. Thick Lens

Thick-Lens-Modelle SOLLEN unterstützt werden.

---

## 106. Principal Planes

Hauptebenen eines optischen Systems SOLLEN berechnet werden können.

---

## 107. Nodal Points

Knotenpunkte SOLLEN berechnet werden können.

---

## 108. Focal Points

Vorderer und hinterer Brennpunkt SOLLEN bestimmt werden können.

---

## 109. Effective Focal Length

Die effektive Brennweite wird als:

`EffectiveFocalLength`

repräsentiert.

---

## 110. Front Focal Length

Front Focal Length SOLL berechnet werden können.

---

## 111. Back Focal Length

Back Focal Length SOLL berechnet werden können.

---

## 112. Magnification

Abbildungsmaßstab SOLL berechnet werden können.

---

## 113. Angular Magnification

Winkelvergrößerung SOLL für geeignete Systeme unterstützt werden.

---

## 114. Numerical Aperture

Numerical Aperture wird als:

`NumericalAperture`

repräsentiert.

---

## 115. NA Equation

Für geeignete Systeme gilt:

```text
NA = n * sin(theta)
```

---

## 116. F-Number

Die Blendenzahl wird als:

`FNumber`

repräsentiert.

---

## 117. F-Number Approximation

Für einfache Systeme gilt näherungsweise:

```text
N = f / D
```

---

## 118. Working F-Number

Working F-Number SOLL für endliche Abbildungsmaßstäbe berechnet werden können.

---

## 119. Field of View

Field of View wird als:

`OpticalFieldOfView`

repräsentiert.

---

## 120. FOV Types

Mindestens:

```text
Horizontal
Vertical
Diagonal
Angular
ObjectSpace
ImageSpace
```

SOLLTEN unterstützt werden.

---

## 121. Sensor-Based FOV

Für vereinfachte rectilineare Systeme kann gelten:

```text
FOV = 2 * atan(sensor_size / (2 * focal_length))
```

---

## 122. Exact FOV

Für komplexe Systeme SOLL FOV durch Ray Tracing bestimmt werden können.

---

## 123. Focus

Lens Compute MUSS Fokusberechnung unterstützen.

---

## 124. Focus Plane

Eine Fokusfläche wird als:

`OpticalFocusSurface`

repräsentiert.

---

## 125. Best Focus

Best Focus SOLL anhand definierter Optimierungskriterien bestimmt werden können.

---

## 126. Focus Metrics

Mögliche Kriterien umfassen:

```text
RMSSpotRadius
GeometricSpotRadius
WavefrontError
MTF
StrehlRatio
Custom
```

---

## 127. Autofocus Simulation

Automatische Fokusoptimierung SOLL unterstützt werden können.

---

## 128. Depth of Field

Depth of Field SOLL berechnet beziehungsweise simuliert werden können.

---

## 129. Depth of Focus

Depth of Focus SOLL von Depth of Field unterschieden werden.

---

## 130. Circle of Confusion

Ein:

`CircleOfConfusion`

SOLL als definierbarer Parameter unterstützt werden.

---

## 131. Hyperfocal Distance

Hyperfokaldistanz SOLL für geeignete Modelle berechnet werden können.

---

## 132. Image Plane

Die Bildebene wird als:

`OpticalImagePlane`

repräsentiert.

---

## 133. Sensor

Ein Sensor wird als:

`OpticalSensor`

repräsentiert.

---

## 134. Sensor Geometry

Ein Sensor SOLL enthalten können:

```text
Width
Height
PixelPitch
Resolution
Position
Orientation
Curvature
```

---

## 135. Flat Sensor

Plane Sensoren MÜSSEN unterstützt werden.

---

## 136. Curved Sensor

Gekrümmte Sensoren SOLLEN architektonisch unterstützt werden.

---

## 137. Sensor Pixel

Ein Sensorpixel SOLL eine definierte physikalische Fläche besitzen können.

---

## 138. Microlens Array

Sensor-Mikrolinsen SOLLEN optional modellierbar sein.

---

## 139. Sensor Spectral Response

Spektrale Sensorempfindlichkeit SOLL modellierbar sein.

---

## 140. Color Filter Array

Color Filter Arrays SOLLEN optional berücksichtigt werden.

---

## 141. Sensor Acceptance

Die Zuordnung eines Rays zu einem Sensorpixel MUSS geometrisch eindeutig bestimmbar sein.

---

## 142. Spot Diagram

Lens Compute SOLL Spot-Diagramme berechnen können.

---

## 143. Spot

Ein Ray-Treffer auf einer Analyseebene wird als:

`OpticalSpot`

repräsentiert.

---

## 144. Spot Diagram Inputs

Spot-Diagramme SOLLEN abhängig sein können von:

```text
FieldPoint
Wavelength
Aperture
FocusPosition
RaySampling
```

---

## 145. Spot Centroid

Der Schwerpunkt eines Spot-Diagramms SOLL berechnet werden können.

---

## 146. RMS Spot Radius

RMS Spot Radius SOLL berechnet werden können.

---

## 147. Geometric Spot Radius

Geometrischer Spot Radius SOLL berechnet werden können.

---

## 148. Encircled Energy

Encircled Energy SOLL berechnet werden können.

---

## 149. Aberration

Optische Aberrationen werden als:

`OpticalAberration`

repräsentiert.

---

## 150. Aberration Types

Mindestens folgende Aberrationen SOLLEN analysierbar sein:

```text
SphericalAberration
Coma
Astigmatism
FieldCurvature
Distortion
LongitudinalChromaticAberration
LateralChromaticAberration
```

---

## 151. Spherical Aberration

Sphärische Aberration SOLL quantifizierbar sein.

---

## 152. Coma

Koma SOLL feldabhängig analysiert werden können.

---

## 153. Astigmatism

Tangentiale und sagittale Fokuseigenschaften SOLLEN unterscheidbar sein.

---

## 154. Field Curvature

Bildfeldwölbung SOLL berechnet werden können.

---

## 155. Petzval Surface

Eine Petzval-Fläche SOLL für geeignete Systeme bestimmbar sein.

---

## 156. Distortion

Geometrische Verzeichnung MUSS analysierbar sein.

---

## 157. Distortion Types

Mindestens:

```text
Barrel
Pincushion
Mustache
Custom
```

SOLLTEN darstellbar sein.

---

## 158. Distortion Map

Eine Verzeichnungskarte wird als:

`OpticalDistortionMap`

repräsentiert.

---

## 159. Distortion Grid

Verzeichnung SOLL über Grid- beziehungsweise Field-Mapping visualisierbar sein.

---

## 160. Chromatic Aberration

Chromatische Aberration MUSS über mehrere Wellenlängen analysierbar sein.

---

## 161. Longitudinal Chromatic Aberration

Longitudinale chromatische Aberration SOLL berechnet werden können.

---

## 162. Lateral Chromatic Aberration

Laterale chromatische Aberration SOLL berechnet werden können.

---

## 163. Wavefront

Eine optische Wellenfront wird als:

`OpticalWavefront`

repräsentiert.

---

## 164. Wavefront Error

Wellenfrontfehler SOLL relativ zu einer Referenzwellenfront berechnet werden können.

---

## 165. RMS Wavefront Error

RMS Wavefront Error SOLL unterstützt werden.

---

## 166. Peak-to-Valley

Peak-to-Valley Wavefront Error SOLL unterstützt werden.

---

## 167. Zernike

Wellenfrontfehler SOLLEN über Zernike-Polynome zerlegt werden können.

---

## 168. Zernike Coefficients

Zernike-Koeffizienten SOLLEN als strukturierte Ergebnisse verfügbar sein.

---

## 169. Point Spread Function

Point Spread Function wird als:

`OpticalPSF`

repräsentiert.

---

## 170. Geometric PSF

Eine geometrisch approximierte PSF SOLL aus Ray Distributionen erzeugt werden können.

---

## 171. Diffraction PSF

Eine beugungsbasierte PSF SOLL architektonisch unterstützt werden.

---

## 172. PSF Sampling

Sampling-Parameter der PSF MÜSSEN dokumentierbar sein.

---

## 173. Modulation Transfer Function

MTF wird als:

`OpticalMTF`

repräsentiert.

---

## 174. MTF Calculation

MTF SOLL aus geeigneten PSF- beziehungsweise OTF-Daten berechnet werden können.

---

## 175. Sagittal MTF

Sagittale MTF SOLL unterstützt werden.

---

## 176. Tangential MTF

Tangentiale MTF SOLL unterstützt werden.

---

## 177. Spatial Frequency

MTF-Ergebnisse MÜSSEN ihre räumliche Frequenzeinheit dokumentieren.

---

## 178. OTF

Optical Transfer Function SOLL unterstützt werden.

---

## 179. Phase Transfer

Die Phasenkomponente der OTF SOLL verfügbar sein können.

---

## 180. Diffraction

Beugung SOLL als erweiterte Compute-Fähigkeit unterstützt werden.

---

## 181. Geometric vs Wave Optics

Geometrische und wellenoptische Ergebnisse MÜSSEN unterscheidbar sein.

---

## 182. Diffraction Limit

Das theoretische Beugungslimit SOLL bestimmbar sein.

---

## 183. Airy Pattern

Airy-Disk- beziehungsweise Airy-Pattern-Analyse SOLL für geeignete Aperturen unterstützt werden.

---

## 184. Strehl Ratio

Strehl Ratio SOLL berechnet werden können.

---

## 185. Physical Optics Extension

Lens Compute SOLL zukünftige Physical-Optics-Module integrieren können.

---

## 186. Vignetting

Vignettierung MUSS analysierbar sein.

---

## 187. Mechanical Vignetting

Mechanische Abschattung SOLL durch tatsächliches Ray Clipping bestimmt werden können.

---

## 188. Natural Vignetting

Natürliche Vignettierung SOLL analysierbar sein.

---

## 189. Relative Illumination

Relative Illumination über das Bildfeld SOLL berechnet werden können.

---

## 190. Ghosting

Geisterbilder durch Mehrfachreflexionen SOLLEN über nicht-sequenzielles Ray Tracing analysierbar sein.

---

## 191. Flare

Streulicht- und Flare-Analysen SOLLEN architektonisch unterstützt werden.

---

## 192. Stray Light

Stray-Light-Analyse SOLL auf nicht-sequenzieller optischer Ray-Verfolgung aufbauen können.

---

## 193. Throughput

Optischer Durchsatz SOLL berechnet werden können.

---

## 194. Transmission

Gesamttransmission eines Systems SOLL wellenlängen- und feldabhängig berechnet werden können.

---

## 195. Etendue

Etendue SOLL für geeignete optische Systeme berechnet werden können.

---

## 196. Radiometric Extension

Radiometrische Größen SOLLEN architektonisch integrierbar sein.

---

## 197. Photometric Extension

Photometrische Größen SOLLEN architektonisch integrierbar sein.

---

## 198. Lens Prescription

Eine vollständige Linsendefinition wird als:

`LensPrescription`

repräsentiert.

---

## 199. Prescription Fields

Eine Prescription SOLL pro Oberfläche mindestens enthalten können:

```text
SurfaceId
SurfaceType
Radius
Thickness
Material
Aperture
ConicConstant
AsphericCoefficients
Transform
Coating
```

---

## 200. Prescription Order

Die Reihenfolge der Prescription-Oberflächen MUSS eindeutig sein.

---

## 201. Thickness

`Thickness` SOLL den axialen Abstand zur folgenden Referenzoberfläche beschreiben.

---

## 202. Negative Thickness

Negative Thickness MUSS für gefaltete beziehungsweise spezielle optische Systeme unterstützt werden können.

---

## 203. Lens Units

Eine Prescription MUSS ihre Längeneinheit explizit definieren.

---

## 204. Unit Safety

Unterschiedliche Einheiten DÜRFEN nicht stillschweigend vermischt werden.

---

## 205. Wavelength Units

Wellenlängeneinheiten MÜSSEN explizit definiert sein.

---

## 206. Angular Units

Winkeleinheiten MÜSSEN explizit definiert sein.

---

## 207. Configuration

Optische Systeme SOLLEN mehrere Konfigurationen unterstützen.

---

## 208. Multi-Configuration System

Ein:

`OpticalSystemConfiguration`

SOLL unterschiedliche Zustände desselben Systems beschreiben können.

---

## 209. Zoom Lens

Zoomobjektive SOLLEN über mehrere bewegliche Elementgruppen modellierbar sein.

---

## 210. Focus Group

Fokusgruppen SOLLEN verschiebbar sein.

---

## 211. Floating Elements

Floating-Element-Designs SOLLEN unterstützt werden können.

---

## 212. Internal Focus

Internal-Focus-Systeme SOLLEN modellierbar sein.

---

## 213. Variable Aperture

Variable Aperturen SOLLEN unterstützt werden.

---

## 214. Configuration Parameters

Konfigurierbare Parameter können umfassen:

```text
Zoom
Focus
Aperture
Temperature
ObjectDistance
Wavelength
SensorPosition
```

---

## 215. Tolerance Analysis

Lens Compute SOLL Toleranzanalysen unterstützen.

---

## 216. Tolerance Types

Mindestens folgende Toleranzen SOLLEN unterstützt werden können:

```text
Radius
Thickness
Decenter
Tilt
RefractiveIndex
AbbeNumber
SurfaceIrregularity
AsphereCoefficient
ElementPosition
SensorPosition
```

---

## 217. Tolerance Distribution

Toleranzen SOLLEN statistische Verteilungen besitzen können.

---

## 218. Monte Carlo Tolerance

Monte-Carlo-Toleranzanalyse SOLL unterstützt werden.

---

## 219. Deterministic Tolerance

Worst-Case- beziehungsweise Sensitivitätsanalysen SOLLEN unterstützt werden können.

---

## 220. Sensitivity

Die Sensitivität einer Qualitätsmetrik gegenüber einem Parameter SOLL berechnet werden können.

---

## 221. Compensators

Toleranzanalysen SOLLEN Kompensatoren unterstützen.

---

## 222. Optimization

Lens Compute SOLL Lens-Design-Optimierung architektonisch unterstützen.

---

## 223. Optimization Variables

Optimierbare Parameter können umfassen:

```text
Radius
Thickness
Material
AsphericCoefficient
ElementPosition
Aperture
StopPosition
SensorPosition
```

---

## 224. Constraints

Optimierungsparameter SOLLEN Grenzen und Constraints besitzen können.

---

## 225. Merit Function

Eine Bewertungsfunktion wird als:

`OpticalMeritFunction`

repräsentiert.

---

## 226. Merit Terms

Eine Merit Function SOLL kombinieren können:

```text
SpotSize
WavefrontError
Distortion
FocalLength
BackFocalLength
MTF
Telecentricity
ChiefRayAngle
Vignetting
MechanicalConstraints
Custom
```

---

## 227. Weighted Merit Terms

Einzelne Merit Terms SOLLEN gewichtet werden können.

---

## 228. Optimization Algorithms

Die Architektur SOLL verschiedene Optimierungsalgorithmen unterstützen können.

---

## 229. Local Optimization

Lokale Optimierungsverfahren SOLLEN unterstützt werden können.

---

## 230. Global Optimization

Globale beziehungsweise stochastische Optimierungsverfahren SOLLEN unterstützt werden können.

---

## 231. Gradient-Based Optimization

Gradientenbasierte Verfahren SOLLEN architektonisch unterstützt werden.

---

## 232. Automatic Differentiation

Automatic Differentiation SOLL zukünftig integrierbar sein.

---

## 233. Inverse Design

Inverse Optical Design SOLL architektonisch möglich sein.

---

## 234. Scheduler Integration

Lens Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 235. Optical Tasks

Mindestens folgende Tasks SOLLEN planbar sein:

```text
RayTrace
RayBundleTrace
SpotAnalysis
AberrationAnalysis
DistortionAnalysis
MTFAnalysis
PSFAnalysis
WavefrontAnalysis
ToleranceAnalysis
Optimization
StrayLightAnalysis
```

---

## 236. Parallel Ray Tracing

Unabhängige Rays SOLLEN parallel verarbeitet werden.

---

## 237. Parallel Field Points

Unterschiedliche Field Points SOLLEN parallel verarbeitet werden können.

---

## 238. Parallel Wavelengths

Unterschiedliche Wellenlängen SOLLEN parallel verarbeitet werden können.

---

## 239. Parallel Configurations

Mehrere optische Konfigurationen SOLLEN parallel analysiert werden können.

---

## 240. Monte Carlo Parallelism

Monte-Carlo-Toleranzläufe SOLLEN massiv parallel ausführbar sein.

---

## 241. Cancellation

Lange Lens-Compute-Jobs MÜSSEN abbrechbar sein.

---

## 242. Memory Model Integration

Lens Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 243. Memory Objects

Mindestens folgende Speicherobjekte SOLLEN unterstützt werden:

```text
SurfaceBuffer
MaterialBuffer
RayBuffer
IntersectionBuffer
SpotBuffer
WavefrontBuffer
PSFBuffer
MTFBuffer
ToleranceBuffer
OptimizationBuffer
```

---

## 244. Object Model Integration

Lens Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0004` integrieren.

---

## 245. Compute Objects

Mindestens folgende Objekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen:

```text
OpticalSystem
OpticalElement
OpticalSurface
OpticalMedium
OpticalRay
OpticalRayBundle
OpticalSensor
LensPrescription
OpticalAnalysis
OpticalResult
```

---

## 246. GPU Runtime Integration

Lens Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` integrieren.

---

## 247. GPU Acceleration

Geeignete GPU-Operationen umfassen:

```text
Ray Bundle Tracing
Surface Intersection
Spot Analysis
Monte Carlo Analysis
PSF Calculation
Optimization Evaluation
Non-Sequential Ray Tracing
```

---

## 248. CPU Backend

Eine CPU-basierte Implementierung MUSS möglich sein.

---

## 249. SIMD

Ray-Bundle-Berechnungen SOLLEN SIMD verwenden können.

---

## 250. GPU Fallback

Lens Compute DARF keine dedizierte GPU voraussetzen.

---

## 251. HPC Integration

Lens Compute SOLL `NPSPEC-COMPUTE-HPC-0001` für große Simulationen integrieren können.

---

## 252. Distributed Optical Compute

Große:

```text
Monte Carlo Analyses
Tolerance Analyses
Optimization Runs
Stray Light Simulations
Spectral Sweeps
```

SOLLTEN verteilt ausführbar sein.

---

## 253. Precision

Lens Compute MUSS numerische Präzision explizit verwalten.

---

## 254. FP64

FP64 SOLL für präzise optische Berechnungen unterstützt werden.

---

## 255. FP32

FP32 DARF für geeignete hochparallele Näherungsberechnungen verwendet werden.

---

## 256. Mixed Precision

Mixed Precision SOLL unterstützt werden können.

---

## 257. Precision Selection

Der Präzisionsmodus MUSS konfigurierbar sein.

---

## 258. Numerical Stability

Optische Berechnungen MÜSSEN numerisch robuste Verfahren verwenden.

---

## 259. Grazing Incidence

Nahezu tangentiale Ray-Surface-Intersections MÜSSEN kontrolliert behandelt werden.

---

## 260. Near Critical Angle

Brechung nahe dem kritischen Winkel MUSS numerisch robust behandelt werden.

---

## 261. Invalid Surface

Ungültige Oberflächenparameter MÜSSEN erkannt werden.

---

## 262. Invalid Material

Ungültige Materialparameter MÜSSEN erkannt werden.

---

## 263. Invalid Refractive Index

Nicht physikalisch beziehungsweise mathematisch ungültige Brechungsindizes MÜSSEN diagnostiziert werden.

---

## 264. Ray Status

Ein Optical Ray SOLL mindestens folgende Zustände besitzen können:

```text
ACTIVE
SENSOR_HIT
BLOCKED
ABSORBED
REFLECTED
REFRACTED
TOTAL_INTERNAL_REFLECTION
ESCAPED
INVALID
TERMINATED
```

---

## 265. Analysis

Eine optische Analyse wird als:

`OpticalAnalysis`

repräsentiert.

---

## 266. Analysis Types

Mindestens folgende Analysen SOLLEN architektonisch unterstützt werden:

```text
Paraxial
RayFan
SpotDiagram
Aberration
Distortion
FieldCurvature
Chromatic
Wavefront
PSF
MTF
Vignetting
RelativeIllumination
Tolerance
StrayLight
Optimization
```

---

## 267. Analysis Configuration

Jede Analyse MUSS ihre Eingabeparameter dokumentieren können.

---

## 268. Optical Result

Ein Analyseergebnis wird als:

`OpticalResult`

repräsentiert.

---

## 269. Result Metadata

Ein Result SOLL mindestens enthalten können:

```text
SystemId
SystemVersion
Configuration
AnalysisType
Wavelengths
FieldPoints
Precision
Backend
Timestamp
```

---

## 270. Result Provenance

Jedes Ergebnis MUSS auf die verwendete Optical-System-Version zurückführbar sein.

---

## 271. Approximation State

Ergebnisse SOLLEN angeben können, ob sie auf:

```text
Paraxial
Geometric
Diffraction
Hybrid
```

basieren.

---

## 272. No Approximation Confusion

Paraxiale Ergebnisse DÜRFEN nicht ohne Kennzeichnung als exakte geometrische Ergebnisse ausgegeben werden.

---

## 273. Quality State

Ein Optical Result SOLL einen Qualitätszustand besitzen können.

---

## 274. Quality States

Mindestens:

```text
VALID
APPROXIMATE
PARTIAL
DEGRADED
INVALID
FAILED
CANCELLED
```

SOLLTEN unterstützt werden.

---

## 275. Provenance

Lens Compute MUSS Provenance unterstützen.

---

## 276. Provenance Record

Ein:

`OpticalComputeProvenanceRecord`

SOLL mindestens enthalten:

```text
SystemId
SystemVersion
PrescriptionVersion
MaterialVersions
Configuration
Analysis
RaySampling
Wavelengths
Precision
Backend
RuntimeVersion
Timestamp
```

---

## 277. Extended Provenance

Zusätzlich SOLLEN dokumentiert werden können:

```text
Temperature
Pressure
ToleranceSeed
OptimizationState
CoatingVersions
SolverParameters
```

---

## 278. Diagnostics

Lens Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 279. Diagnostic Classes

Mindestens folgende Diagnosen SOLLEN existieren:

```text
INVALID_OPTICAL_SYSTEM
INVALID_SURFACE
INVALID_RADIUS
INVALID_ASPHERE
INVALID_TRANSFORM
INVALID_MATERIAL
INVALID_REFRACTIVE_INDEX
INVALID_WAVELENGTH
INVALID_APERTURE
INVALID_SENSOR
INVALID_RAY
RAY_MISSED_SURFACE
RAY_BLOCKED
TOTAL_INTERNAL_REFLECTION
INTERSECTION_FAILED
NUMERICAL_FAILURE
NON_CONVERGENCE
OPTIMIZATION_FAILED
INVALID_TOLERANCE
UNSUPPORTED_SURFACE
UNSUPPORTED_MATERIAL_MODEL
UNSUPPORTED_ANALYSIS
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
SystemId
SurfaceId
ElementId
RayId
Wavelength
FieldPoint
Configuration
Backend
Timestamp
```

---

## 282. Profiling

Lens Compute MUSS Profiling unterstützen.

---

## 283. Profiling Stages

Mindestens folgende Bereiche SOLLTEN separat messbar sein:

```text
SystemPreparation
RayGeneration
SurfaceIntersection
Refraction
Reflection
Propagation
SensorIntersection
SpotAnalysis
WavefrontAnalysis
PSFCalculation
MTFCalculation
ToleranceEvaluation
OptimizationEvaluation
```

---

## 284. Ray Count

Die Anzahl verarbeiteter optischer Rays SOLL messbar sein.

---

## 285. Rays Per Second

Ray Throughput SOLL messbar sein.

---

## 286. Surface Intersections

Die Anzahl der Surface-Intersection-Tests SOLL messbar sein.

---

## 287. Analysis Time

Laufzeit einzelner Analysen SOLL messbar sein.

---

## 288. Optimization Iterations

Optimierungsiterationen SOLLEN messbar sein.

---

## 289. Monte Carlo Samples

Die Anzahl durchgeführter Monte-Carlo-Samples SOLL messbar sein.

---

## 290. Determinism

Lens Compute SOLL einen deterministischen Modus unterstützen.

---

## 291. Deterministic Inputs

Deterministische Ausführung SOLL mindestens berücksichtigen:

```text
SystemVersion
Configuration
RaySampling
Wavelengths
RandomSeed
Precision
BackendConfiguration
```

---

## 292. Random Seed

Monte-Carlo- und stochastische Verfahren MÜSSEN explizite Random Seeds unterstützen.

---

## 293. Backend Equivalence

CPU- und GPU-Ergebnisse SOLLTEN innerhalb definierter numerischer Toleranzen vergleichbar sein.

---

## 294. Testing — Basic Lens

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
PlanoConvex
PlanoConcave
Biconvex
Biconcave
Meniscus
PlaneParallelPlate
```

---

## 295. Testing — Surface

Mindestens:

```text
Plane
Sphere
Asphere
Cylinder
Conic
InvalidSurface
```

MÜSSEN getestet werden.

---

## 296. Testing — Refraction

Mindestens:

```text
NormalIncidence
ObliqueIncidence
AirToGlass
GlassToAir
CriticalAngle
TotalInternalReflection
```

MÜSSEN getestet werden.

---

## 297. Testing — Reflection

Mindestens:

```text
NormalReflection
ObliqueReflection
Mirror
CoatedSurface
```

SOLLTEN getestet werden.

---

## 298. Testing — Dispersion

Mindestens mehrere Wellenlängen durch ein dispersives Material MÜSSEN getestet werden.

---

## 299. Testing — Paraxial

Mindestens:

```text
ThinLens
ThickLens
EffectiveFocalLength
BackFocalLength
Magnification
```

MÜSSEN getestet werden.

---

## 300. Testing — Ray Trace

Mindestens:

```text
OnAxisRay
OffAxisRay
ChiefRay
MarginalRay
BlockedRay
MissedSurface
```

MÜSSEN getestet werden.

---

## 301. Testing — Aberrations

Mindestens:

```text
SphericalAberration
Coma
Astigmatism
FieldCurvature
Distortion
ChromaticAberration
```

SOLLTEN getestet werden.

---

## 302. Testing — Spot

Spot-Diagramme MÜSSEN gegen analytisch beziehungsweise numerisch bekannte Referenzsysteme getestet werden.

---

## 303. Testing — Wavefront

OPD- und Wavefront-Berechnungen SOLLTEN gegen bekannte Referenzen getestet werden.

---

## 304. Testing — MTF

MTF-Berechnungen SOLLTEN gegen analytische beziehungsweise validierte Referenzfälle getestet werden.

---

## 305. Testing — Vignetting

Mechanische Vignettierung MUSS mit definierten Aperturen getestet werden.

---

## 306. Testing — Sensor

Mindestens:

```text
FlatSensor
OffAxisHit
PixelMapping
SensorTilt
SensorShift
```

MÜSSEN getestet werden.

---

## 307. Testing — Zoom

Mehrere Zoom-Konfigurationen SOLLTEN getestet werden.

---

## 308. Testing — Focus

Mehrere Fokuspositionen SOLLTEN getestet werden.

---

## 309. Testing — Tolerance

Mindestens:

```text
RadiusTolerance
ThicknessTolerance
Decenter
Tilt
IndexTolerance
MonteCarlo
```

SOLLTEN getestet werden.

---

## 310. Testing — Optimization

Optimierung MUSS gegen ein definiertes Testsystem mit bekanntem Verbesserungsziel getestet werden.

---

## 311. Testing — CPU/GPU

CPU- und GPU-Ray-Tracing-Ergebnisse SOLLTEN innerhalb definierter Toleranzen verglichen werden.

---

## 312. Testing — Determinism

Identische deterministische Inputs MÜSSEN reproduzierbare Ergebnisse liefern.

---

## 313. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
ThousandsOfSurfaces
MillionsOfRays
ManyWavelengths
ManyFieldPoints
LargeRayBundles
ComplexAspheres
FreeformSurfaces
LargeMonteCarloTolerance
LongOptimization
NonSequentialSystem
StrayLightSimulation
ManyConfigurations
GPU Memory Pressure
CPU Memory Pressure
LongRunningSimulation
```

---

## 314. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
OpticalSystem
OpticalElement
OpticalSurface
OpticalMedium
OpticalRay
OpticalRayBundle
OpticalSensor

Plane Surface
Spherical Surface
Basic Asphere

Constant Refractive Index
Basic Dispersion Architecture

Sequential Ray Tracing
Ray-Surface Intersection
Reflection
Refraction
Total Internal Reflection

Aperture
Aperture Stop

Paraxial Optics
Thin Lens
Thick Lens Architecture

Effective Focal Length
Back Focal Length
Magnification
Field of View

Focus
Spot Diagram
Basic Distortion
Basic Chromatic Analysis

CPU Backend
GPU Backend Architecture

Ray Tracing Integration
Scheduler Integration
Memory Model Integration
Object Model Integration

FP64
Diagnostics
Profiling
Provenance
Cancellation
```

---

## 315. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Advanced Aspheres
Freeform Surfaces
Cylindrical Surfaces
Toroidal Surfaces

Sellmeier Materials
Temperature-Dependent Materials
Coatings
Multilayer Coatings
Polarization

Non-Sequential Ray Tracing
Ghost Analysis
Flare Analysis
Stray Light Analysis

Wavefront Analysis
Zernike Decomposition
PSF
OTF
MTF
Diffraction
Strehl Ratio

Vignetting
Relative Illumination

Zoom Systems
Internal Focus
Floating Elements
Multi-Configuration Systems

Tolerance Analysis
Monte Carlo
Sensitivity Analysis
Compensators

Lens Optimization
Merit Functions
Gradient-Based Optimization
Global Optimization
Inverse Design

GPU Acceleration
HPC
Distributed Optical Compute
```

---

## 316. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-LENS-0001` MUSS:

1. optische Systeme repräsentieren.
2. optische Systeme eindeutig identifizieren.
3. Systemversionen unterstützen.
4. optische Elemente repräsentieren.
5. optische Oberflächen repräsentieren.
6. Oberflächen eindeutig identifizieren.
7. Oberflächenreihenfolgen definieren.
8. plane Oberflächen unterstützen.
9. sphärische Oberflächen unterstützen.
10. grundlegende asphärische Oberflächen unterstützen.
11. lokale Oberflächenkoordinaten unterstützen.
12. Transformationen zwischen lokalen und globalen Koordinaten unterstützen.
13. optische Medien repräsentieren.
14. Brechungsindizes unterstützen.
15. Wellenlängenabhängigkeit architektonisch unterstützen.
16. Materialidentitäten unterstützen.
17. Materialversionen unterstützen.
18. Luft und Vakuum als Medien unterstützen.
19. optische Rays repräsentieren.
20. Ray Origin und Direction unterstützen.
21. Ray-Wellenlängen unterstützen.
22. Ray Bundles unterstützen.
23. sequenzielles Ray Tracing unterstützen.
24. Ray-Surface-Intersection unterstützen.
25. Oberflächennormalen berechnen.
26. Reflexion unterstützen.
27. Brechung unterstützen.
28. das Snelliussche Gesetz korrekt anwenden.
29. Totalreflexion erkennen.
30. Aperturen unterstützen.
31. Ray Clipping an Aperturen unterstützen.
32. Aperture Stops unterstützen.
33. optische Weglängen berechnen können.
34. geometrische und optische Weglänge unterscheiden.
35. paraxiale Optik unterstützen.
36. Thin-Lens-Berechnungen unterstützen.
37. Thick-Lens-Architektur unterstützen.
38. effektive Brennweite bestimmen können.
39. Back Focal Length bestimmen können.
40. Abbildungsmaßstab bestimmen können.
41. Field of View bestimmen können.
42. Fokusberechnung unterstützen.
43. Sensoren repräsentieren.
44. plane Sensoren unterstützen.
45. Ray-Sensor-Intersections bestimmen.
46. Spot-Diagramme berechnen können.
47. RMS Spot Radius bestimmen können.
48. grundlegende Verzeichnung analysieren können.
49. chromatische Analyse unterstützen.
50. mehrere Wellenlängen verarbeiten können.
51. Lens Prescriptions repräsentieren.
52. Einheiten explizit verwalten.
53. unterschiedliche Einheiten nicht stillschweigend vermischen.
54. `NPSPEC-COMPUTE-RAYTRACING-0001` integrieren können.
55. `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.
56. `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.
57. `NPSPEC-COMPUTE-RUNTIME-0004` integrieren.
58. CPU-Ausführung unterstützen.
59. GPU-Ausführung architektonisch unterstützen.
60. keine GPU voraussetzen.
61. FP64 unterstützen.
62. numerisch robuste Intersection-Verfahren verwenden.
63. ungültige Oberflächen erkennen.
64. ungültige Materialien erkennen.
65. ungültige Rays erkennen.
66. Ray-Status repräsentieren.
67. Analysen eindeutig konfigurieren.
68. Ergebnisse auf die Systemversion zurückführen.
69. Approximationen kennzeichnen.
70. paraxiale und exakte geometrische Ergebnisse unterscheiden.
71. Provenance unterstützen.
72. Materialversionen in Provenance dokumentieren können.
73. verwendete Wellenlängen dokumentieren.
74. verwendete Präzision dokumentieren.
75. verwendetes Backend dokumentieren.
76. strukturierte Diagnosen bereitstellen.
77. Profiling unterstützen.
78. Ray Count messen können.
79. Rays per Second messen können.
80. Analysezeiten messen können.
81. Cancellation unterstützen.
82. deterministische Ausführung unterstützen können.
83. explizite Random Seeds für stochastische Verfahren unterstützen.
84. Toleranzanalyse architektonisch unterstützen.
85. Optimierung architektonisch unterstützen.
86. Multi-Configuration-Systeme architektonisch unterstützen.
87. Vignettierungsanalyse architektonisch unterstützen.
88. Wavefront-Analyse architektonisch unterstützen.
89. PSF- und MTF-Analyse architektonisch unterstützen.
90. Erweiterungen ohne Änderung des grundlegenden optischen Objektmodells ermöglichen.

---

## 317. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaLensCompute
 |
 +-- OpticalRuntime
 |
 +-- OpticalSystemManager
 |
 +-- PrescriptionManager
 |
 +-- ElementManager
 |
 +-- SurfaceEngine
 |    |
 |    +-- PlaneSurface
 |    +-- SphericalSurface
 |    +-- AsphericSurface
 |    +-- CylindricalSurface
 |    +-- FreeformSurface
 |
 +-- MaterialEngine
 |
 +-- DispersionEngine
 |
 +-- CoatingEngine
 |
 +-- ApertureEngine
 |
 +-- RayGenerator
 |
 +-- SequentialRayTracer
 |
 +-- NonSequentialRayTracer
 |
 +-- IntersectionEngine
 |
 +-- RefractionEngine
 |
 +-- ReflectionEngine
 |
 +-- OpticalPathEngine
 |
 +-- ParaxialEngine
 |
 +-- FocusEngine
 |
 +-- SensorEngine
 |
 +-- SpotAnalysisEngine
 |
 +-- AberrationEngine
 |
 +-- DistortionEngine
 |
 +-- WavefrontEngine
 |
 +-- PSFEngine
 |
 +-- MTFEngine
 |
 +-- VignettingEngine
 |
 +-- ToleranceEngine
 |
 +-- OptimizationEngine
 |
 +-- BackendManager
 |
 +-- ProvenanceEngine
 |
 +-- Diagnostics
 |
 +-- Profiler
```

Diese Struktur ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 318. Beziehung zu `NPSPEC-COMPUTE-RAYTRACING-0001`

Ray Tracing Compute stellt allgemeine Ray-, Traversierungs- und Beschleunigungsmechanismen bereit.

Lens Compute erweitert diese um optische Semantik:

```text
General Ray
   |
   v
Optical Ray
   |
   +-- Wavelength
   +-- Intensity
   +-- Medium
   +-- Polarization
   +-- Optical Path
```

---

## 319. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt die gemeinsame Ausführungsinfrastruktur bereit.

```text
Lens Compute
     |
     v
Compute Runtime
     |
 +---+----+
 |        |
 v        v
CPU      GPU
```

---

## 320. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert insbesondere:

```text
Ray Bundles
Field Points
Wavelengths
Configurations
Tolerance Samples
Optimization Evaluations
```

---

## 321. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet insbesondere:

```text
Surfaces
Materials
Rays
Intersections
Spots
Wavefronts
PSFs
MTFs
Optimization State
```

---

## 322. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Optische Ressourcen SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 323. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

Die GPU Runtime kann hochparallele optische Ray- und Analyseoperationen beschleunigen.

---

## 324. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

HPC Compute kann große Toleranz-, Optimierungs-, Stray-Light- und Monte-Carlo-Simulationen verteilen.

---

## 325. Grundlegende Lens-Compute-Architektur

```text
Lens Prescription
       |
       v
Optical System
       |
       v
Surface Sequence
       |
       v
Ray Generation
       |
       v
Intersection
       |
       v
Refraction / Reflection
       |
       v
Next Surface
       |
       v
Image Plane
       |
       v
Analysis
```

---

## 326. Chromatische Architektur

```text
Ray Source
    |
    +--------+--------+
    |        |        |
    v        v        v
 lambda1   lambda2   lambda3
    |        |        |
    v        v        v
Dispersion Model
    |        |        |
    v        v        v
Ray Tracing
    |        |        |
    +--------+--------+
             |
             v
Chromatic Analysis
```

---

## 327. Lens-Optimization-Architektur

```text
Initial Prescription
        |
        v
Optimization Variables
        |
        v
Ray Trace / Analysis
        |
        v
Merit Function
        |
        v
Optimizer
        |
        v
Updated Prescription
        |
        +----------+
        |          |
        +<---------+
```

---

## 328. Toleranzanalyse-Architektur

```text
Nominal Lens
     |
     v
Tolerance Model
     |
     v
Parameter Variation
     |
 +---+---+---+
 |       |   |
 v       v   v
Run 1  Run 2 ...
 |       |   |
 +---+---+---+
     |
     v
Statistical Analysis
     |
     v
Tolerance Result
```

---

## 329. Datenmodellgrundsatz

Lens Compute MUSS zwischen:

```text
Optical System
Prescription
Element
Surface
Medium
Aperture
Ray
Intersection
Sensor
Analysis
Result
```

unterscheiden.

---

## 330. Optischer Geometriegrundsatz

Eine Linse ist nicht lediglich durch ihre Brennweite definiert.

Die physikalisch relevante Beschreibung kann umfassen:

```text
Surface Geometry
Surface Position
Surface Orientation
Material
Thickness
Aperture
Coating
Environment
Wavelength
```

---

## 331. Spektralgrundsatz

Ein einzelner Brechungsindex ist keine vollständige Beschreibung eines realen dispersiven optischen Materials.

Daher gilt:

```text
n = f(lambda, Temperature, Pressure, ...)
```

für Systeme, bei denen diese Abhängigkeiten relevant sind.

---

## 332. Ray-Tracing-Grundsatz

Paraxiale Optik und reales Ray Tracing MÜSSEN als unterschiedliche Berechnungsmodelle behandelt werden.

```text
Paraxial Model
!=
Exact Geometric Ray Trace
```

---

## 333. Analysegrundsatz

Optische Qualitätsmetriken MÜSSEN zusammen mit ihren Berechnungsbedingungen interpretierbar sein.

Beispielsweise ist:

```text
MTF
```

ohne:

```text
Spatial Frequency
Field Position
Wavelength
Focus State
Aperture
```

nicht vollständig beschrieben.

---

## 334. Toleranzgrundsatz

Ein nominal optimiertes optisches System ist nicht automatisch fertigungstauglich.

Lens Compute SOLL deshalb nominale Leistung und toleranzbedingte Leistung getrennt behandeln.

---

## 335. Provenance-Grundsatz

Ein optisches Analyseergebnis MUSS auf die exakte Version der verwendeten Prescription, Materialien, Konfiguration und Analyseparameter zurückführbar sein.

---

## 336. Erweiterbarkeit

Neue:

```text
Surface Types
Material Models
Dispersion Models
Coatings
Ray Models
Sensor Models
Analysis Methods
Optimization Algorithms
Tolerance Models
Physical Optics Methods
Compute Backends
```

SOLLTEN ohne Änderung des grundlegenden Optical-System-Objektmodells integrierbar sein.

---

## 337. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-LENS-SYSTEM-0001
Optical System Model

NPSPEC-COMPUTE-LENS-SURFACE-0001
Optical Surface Model

NPSPEC-COMPUTE-LENS-ASPHERE-0001
Aspheric Surfaces

NPSPEC-COMPUTE-LENS-FREEFORM-0001
Freeform Optical Surfaces

NPSPEC-COMPUTE-LENS-MATERIAL-0001
Optical Materials

NPSPEC-COMPUTE-LENS-DISPERSION-0001
Optical Dispersion

NPSPEC-COMPUTE-LENS-COATING-0001
Optical Coatings

NPSPEC-COMPUTE-LENS-APERTURE-0001
Optical Apertures and Stops

NPSPEC-COMPUTE-LENS-RAYTRACE-0001
Sequential Optical Ray Tracing

NPSPEC-COMPUTE-LENS-NONSEQUENTIAL-0001
Non-Sequential Optical Ray Tracing

NPSPEC-COMPUTE-LENS-PARAXIAL-0001
Paraxial Optics

NPSPEC-COMPUTE-LENS-FOCUS-0001
Focus and Depth of Field

NPSPEC-COMPUTE-LENS-SENSOR-0001
Optical Sensor Model

NPSPEC-COMPUTE-LENS-SPOT-0001
Spot Diagram Analysis

NPSPEC-COMPUTE-LENS-ABERRATION-0001
Optical Aberrations

NPSPEC-COMPUTE-LENS-DISTORTION-0001
Optical Distortion

NPSPEC-COMPUTE-LENS-WAVEFRONT-0001
Wavefront Analysis

NPSPEC-COMPUTE-LENS-PSF-0001
Point Spread Function

NPSPEC-COMPUTE-LENS-MTF-0001
Modulation Transfer Function

NPSPEC-COMPUTE-LENS-DIFFRACTION-0001
Diffraction Compute

NPSPEC-COMPUTE-LENS-VIGNETTING-0001
Optical Vignetting

NPSPEC-COMPUTE-LENS-STRAYLIGHT-0001
Stray Light Analysis

NPSPEC-COMPUTE-LENS-TOLERANCE-0001
Optical Tolerance Analysis

NPSPEC-COMPUTE-LENS-OPTIMIZE-0001
Lens Design Optimization

NPSPEC-COMPUTE-LENS-POLARIZATION-0001
Polarization Optics

NPSPEC-COMPUTE-LENS-PROVENANCE-0001
Optical Compute Provenance

NPSPEC-COMPUTE-LENS-TEST-0001
Lens Compute Conformance Tests
```

---

## 338. Zusammenfassung

`NPSPEC-COMPUTE-LENS-0001` definiert die gemeinsame Infrastruktur zur Modellierung, Simulation und Analyse optischer Linsen- und Abbildungssysteme innerhalb der NovaOS-Compute-Architektur.

Die grundlegende Verarbeitung lautet:

```text
Lens Prescription
       |
       v
Optical System
       |
       v
Ray Generation
       |
       v
Surface Intersection
       |
       v
Reflection / Refraction
       |
       v
Ray Propagation
       |
       v
Sensor
       |
       v
Optical Analysis
```

Die Architektur unterstützt:

```text
Single Lenses
Compound Lenses
Spherical Surfaces
Aspheric Surfaces
Freeform Surfaces
Optical Materials
Dispersion
Apertures
Stops
Reflection
Refraction
Sequential Ray Tracing
Non-Sequential Ray Tracing
Paraxial Optics
Focus
Field of View
Spot Diagrams
Aberrations
Distortion
Wavefront Analysis
PSF
MTF
Vignetting
Tolerance Analysis
Lens Optimization
```

Die Berechnung kann über:

```text
CPU
SIMD
Multi-Core
GPU
HPC
Distributed Compute
```

beschleunigt werden.

Die Architektur trennt konsequent:

```text
Optical Geometry
Material
Surface
Ray
Interaction
Propagation
Sensor
Analysis
Optimization
Result
```

voneinander.

Die zentrale Geometrieregel lautet:

> Ein reales optisches System wird aus seinen tatsächlichen Flächen, Abständen, Materialien, Aperturen und Transformationen aufgebaut. Brennweite und Field of View sind daraus abgeleitete Systemeigenschaften und ersetzen nicht die physikalische Linsenbeschreibung.

Die zentrale Spektralregel lautet:

> Optische Materialeigenschaften werden, soweit relevant, wellenlängen- und umgebungsabhängig behandelt. Chromatische Eigenschaften dürfen nicht auf einen einzigen konstanten Brechungsindex reduziert werden, wenn die Simulation spektrale Genauigkeit erfordert.

Die zentrale Analyse-Regel lautet:

> Spot Size, Aberration, Wavefront Error, PSF, MTF, Distortion und andere Qualitätsmetriken besitzen nur zusammen mit Feldposition, Wellenlänge, Apertur, Fokuszustand und verwendeter Berechnungsmethode eine eindeutige technische Bedeutung.

Das grundlegende Architekturprinzip lautet:

> NovaOS Lens Compute behandelt optische Systeme als explizite, versionierte Compute-Modelle aus Oberflächen, Medien, Aperturen, Beschichtungen, Rays, Sensoren und Analyseverfahren. Die allgemeine Ray-Tracing-Infrastruktur übernimmt die räumliche Strahlverfolgung, während Lens Compute die optische Physik und Systemsemantik ergänzt. Dadurch kann dieselbe Infrastruktur von einfacher Linsenberechnung über Kamera- und Mikroskopobjektive bis zu komplexer Aberrations-, MTF-, Toleranz-, Stray-Light- und Lens-Design-Optimierung skaliert werden.