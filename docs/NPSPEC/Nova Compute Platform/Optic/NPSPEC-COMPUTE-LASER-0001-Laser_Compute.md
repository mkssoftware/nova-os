# NPSPEC-COMPUTE-LASER-0001 — Laser Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Optics / Laser  
**Komponente:** NovaOS Laser Compute  
**Bezeichner:** `NPSPEC-COMPUTE-LASER-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0001`, `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-RAYTRACING-0001`, `NPSPEC-COMPUTE-LENS-0001`  
**Kurzname:** Laser Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Compute-Infrastruktur zur Modellierung, Simulation, Analyse und Optimierung von Lasern und kohärenten optischen Strahlungssystemen innerhalb der NovaOS-Compute-Architektur.

Laser Compute stellt standardisierte Funktionen für:

- Laserquellen,
- Laserstrahlen,
- Gaussian Beams,
- Resonatoren,
- Verstärkermedien,
- Pumpquellen,
- optische Kavitäten,
- Strahlpropagation,
- Strahlfokussierung,
- Strahlaufweitung,
- Moden,
- Polarisation,
- Kohärenz,
- spektrale Eigenschaften,
- zeitliche Pulse,
- kontinuierliche Laser,
- gepulste Laser,
- ultrakurze Pulse,
- Verstärkung,
- Verluste,
- Sättigung,
- Resonatorstabilität,
- Beam Quality,
- M²-Analyse,
- Fokusparameter,
- Intensitätsverteilungen,
- Leistungsdichte,
- thermische Effekte,
- nichtlineare Optik,
- Frequenzkonversion,
- Laser-Material-Interaktion,
- wissenschaftliche Laser-Simulation

bereit.

Die Komponente ist nicht auf eine bestimmte Laserart beschränkt.

Sie SOLL als allgemeine Compute-Infrastruktur für Anwendungen dienen wie:

```text
Scientific Optics
Laser Engineering
Optical Design
Industrial Laser Simulation
Medical Laser Simulation
Metrology
Microscopy
Spectroscopy
Interferometry
LiDAR
Optical Communication
Quantum Optics Extensions
Laser Material Processing Simulation
```

Die grundlegende Architektur lautet:

```text
Laser System
     |
     v
Source / Pump
     |
     v
Gain Medium
     |
     v
Optical Resonator
     |
     v
Laser Mode
     |
     v
Beam Generation
     |
     v
Beam Propagation
     |
     v
Optical System
     |
     v
Interaction / Detection
     |
     v
Analysis
```

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Ein Laser wird nicht lediglich als idealer Lichtstrahl beschrieben, sondern als physikalisch parametrisiertes kohärentes Strahlungssystem mit räumlichen, spektralen, zeitlichen, energetischen und polarimetrischen Eigenschaften.

---

## 3. Abstraktionsmodell

Laser Compute MUSS mindestens zwischen folgenden Ebenen unterscheiden:

```text
Laser System
Laser Source
Pump Source
Gain Medium
Resonator
Mode
Beam
Pulse
Propagation
Optical Interaction
Detector
Analysis
```

---

## 4. Laser System

Ein vollständiges Lasersystem wird als:

`LaserSystem`

repräsentiert.

---

## 5. System Identity

Jedes Laser System MUSS eindeutig identifizierbar sein.

---

## 6. System Version

Laser Systems SOLLEN versionierbar sein.

---

## 7. System Contents

Ein Laser System SOLL enthalten können:

```text
LaserSource
PumpSource
GainMedium
Resonator
OpticalElements
BeamDefinition
PulseDefinition
Environment
Detector
Configuration
```

---

## 8. Laser Source

Eine Laserquelle wird als:

`LaserSource`

repräsentiert.

---

## 9. Laser Source Properties

Eine Laser Source SOLL mindestens enthalten können:

```text
Wavelength
Frequency
Power
Energy
BeamProfile
Polarization
Coherence
SpectralWidth
TemporalMode
```

---

## 10. Laser Types

Mindestens folgende Laserklassen SOLLEN architektonisch darstellbar sein:

```text
SolidStateLaser
GasLaser
SemiconductorLaser
FiberLaser
DyeLaser
ExcimerLaser
ChemicalLaser
FreeElectronLaser
DiskLaser
QuantumCascadeLaser
CustomLaser
```

---

## 11. Operating Modes

Laser SOLLEN mindestens folgende Betriebsarten unterstützen können:

```text
ContinuousWave
Pulsed
QSwitched
ModeLocked
GainSwitched
Custom
```

---

## 12. Continuous-Wave Laser

Ein kontinuierlicher Laser wird als:

`ContinuousWaveLaser`

repräsentiert.

---

## 13. CW Properties

CW-Laser SOLLEN mindestens besitzen können:

```text
OpticalPower
Wavelength
Linewidth
BeamProfile
Polarization
```

---

## 14. Pulsed Laser

Ein gepulster Laser wird als:

`PulsedLaser`

repräsentiert.

---

## 15. Pulse Properties

Ein gepulster Laser SOLL mindestens definieren können:

```text
PulseEnergy
PulseDuration
RepetitionRate
PeakPower
PulseShape
Wavelength
SpectralWidth
```

---

## 16. Peak Power

Für geeignete vereinfachte Pulsmodelle kann gelten:

```text
PeakPower ~= PulseEnergy / PulseDuration
```

Die tatsächliche Berechnung MUSS das verwendete Pulsprofil berücksichtigen können.

---

## 17. Average Power

Für periodische Pulse kann gelten:

```text
AveragePower = PulseEnergy * RepetitionRate
```

---

## 18. Pulse Shape

Mindestens folgende zeitliche Pulsprofile SOLLEN unterstützt werden können:

```text
Gaussian
SechSquared
Lorentzian
Rectangular
Measured
Custom
```

---

## 19. Ultrashort Pulse

Ultrakurze Pulse SOLLEN architektonisch unterstützt werden.

---

## 20. Pulse Phase

Ein Puls SOLL eine spektrale beziehungsweise zeitliche Phase besitzen können.

---

## 21. Chirp

Gechirpte Pulse SOLLEN modellierbar sein.

---

## 22. Chirp Parameter

Ein Pulse Model SOLL mindestens linearen Chirp unterstützen können.

---

## 23. Spectral Phase

Erweiterte Implementierungen SOLLEN beliebige spektrale Phasenprofile unterstützen können.

---

## 24. Laser Wavelength

Die zentrale Laserwellenlänge wird als:

`LaserWavelength`

repräsentiert.

---

## 25. Frequency

Frequenz und Wellenlänge stehen im Vakuum in Beziehung:

```text
f = c / lambda
```

---

## 26. Unit Safety

Wellenlänge und Frequenz MÜSSEN mit expliziten Einheiten verarbeitet werden.

---

## 27. Spectrum

Das Laserspektrum wird als:

`LaserSpectrum`

repräsentiert.

---

## 28. Spectrum Properties

Ein Spectrum SOLL enthalten können:

```text
CenterWavelength
CenterFrequency
Bandwidth
SpectralShape
SpectralPowerDensity
Phase
```

---

## 29. Spectral Width

Spektrale Linienbreite MUSS explizit definierbar sein.

---

## 30. Spectral Representation

Spektren SOLLEN dargestellt werden können als:

```text
Analytical
Sampled
Measured
Custom
```

---

## 31. Single Frequency

Single-Frequency-Laser SOLLEN unterstützt werden.

---

## 32. Multi-Longitudinal-Mode

Mehrere longitudinale Moden SOLLEN unterstützt werden können.

---

## 33. Coherence

Kohärenzeigenschaften werden als:

`LaserCoherence`

repräsentiert.

---

## 34. Temporal Coherence

Zeitliche Kohärenz SOLL modellierbar sein.

---

## 35. Spatial Coherence

Räumliche Kohärenz SOLL modellierbar sein.

---

## 36. Coherence Length

Die Kohärenzlänge SOLL aus geeigneten Spektralmodellen berechnet werden können.

---

## 37. Coherence Time

Kohärenzzeit SOLL berechnet werden können.

---

## 38. Phase

Laserstrahlung SOLL eine explizite Phase besitzen können.

---

## 39. Phase Preservation

Kohärente Propagationsverfahren MÜSSEN Phaseninformation erhalten.

---

## 40. Laser Beam

Ein Laserstrahl wird als:

`LaserBeam`

repräsentiert.

---

## 41. Beam Properties

Ein Laser Beam SOLL mindestens enthalten können:

```text
Origin
Direction
Wavelength
Power
BeamProfile
BeamRadius
Divergence
Phase
Polarization
M2
```

---

## 42. Beam Coordinate System

Jeder Laser Beam SOLL ein lokales Strahlkoordinatensystem besitzen.

---

## 43. Beam Axis

Die zentrale Ausbreitungsachse MUSS explizit definiert sein.

---

## 44. Beam Profile

Ein räumliches Strahlprofil wird als:

`LaserBeamProfile`

repräsentiert.

---

## 45. Beam Profile Types

Mindestens folgende Profile SOLLEN unterstützt werden können:

```text
Gaussian
SuperGaussian
TopHat
HermiteGaussian
LaguerreGaussian
Bessel
Measured
Custom
```

---

## 46. Gaussian Beam

Gaussian Beams MÜSSEN als grundlegendes Laserstrahlmodell unterstützt werden.

---

## 47. Gaussian Intensity

Für einen idealen TEM00-Gaussian-Beam kann die Intensität beschrieben werden durch:

```text
I(r,z) =
I0(z) *
exp(
    -2 * r^2 / w(z)^2
)
```

---

## 48. Beam Radius

Der Beam Radius wird als:

`BeamRadius`

repräsentiert.

---

## 49. Radius Convention

Die verwendete Beam-Radius-Konvention MUSS explizit dokumentiert werden.

Für Gaussian Beams SOLL standardmäßig der `1/e²`-Intensitätsradius unterstützt werden.

---

## 50. Beam Waist

Die minimale Strahlbreite wird als:

`BeamWaist`

repräsentiert.

---

## 51. Waist Radius

Der Waist Radius wird als:

```text
w0
```

repräsentiert.

---

## 52. Waist Position

Die Position des Beam Waist MUSS explizit definierbar sein.

---

## 53. Rayleigh Range

Für einen idealen Gaussian Beam gilt:

```text
zR = pi * w0^2 / lambda
```

---

## 54. Beam Radius Propagation

Für einen idealen Gaussian Beam gilt:

```text
w(z) =
w0 *
sqrt(
    1 + (z / zR)^2
)
```

---

## 55. Radius of Curvature

Die Wellenfrontkrümmung SOLL berechnet werden können.

Für einen idealen Gaussian Beam gilt:

```text
R(z) =
z *
(
    1 + (zR / z)^2
)
```

für geeignete Werte von `z`.

---

## 56. Gouy Phase

Die Gouy-Phase SOLL unterstützt werden.

Für einen idealen Gaussian Beam:

```text
psi(z) = atan(z / zR)
```

---

## 57. Divergence

Die Fernfelddivergenz wird als:

`BeamDivergence`

repräsentiert.

---

## 58. Ideal Gaussian Divergence

Für einen idealen Gaussian Beam gilt näherungsweise:

```text
theta = lambda / (pi * w0)
```

---

## 59. Beam Quality

Die Strahlqualität wird über:

`BeamQuality`

repräsentiert.

---

## 60. M² Factor

Der M²-Faktor MUSS unterstützt werden.

---

## 61. Ideal Beam

Für einen idealen TEM00 Gaussian Beam gilt:

```text
M2 = 1
```

---

## 62. Real Beam

Reale Strahlen können besitzen:

```text
M2 > 1
```

---

## 63. M² Propagation

Gaussian-Beam-Berechnungen SOLLEN den M²-Faktor berücksichtigen können.

---

## 64. Beam Parameter Product

Beam Parameter Product wird als:

`BPP`

repräsentiert.

---

## 65. BPP

Für geeignete Definitionen gilt:

```text
BPP = w0 * theta
```

---

## 66. Beam Ellipticity

Elliptische Laserstrahlen SOLLEN unterstützt werden.

---

## 67. Astigmatic Beam

Astigmatische Strahlen SOLLEN unterschiedliche Parameter für zwei orthogonale Achsen besitzen können.

---

## 68. X/Y Beam Parameters

Ein astigmatischer Beam SOLL beispielsweise besitzen können:

```text
WaistX
WaistY
WaistPositionX
WaistPositionY
M2X
M2Y
```

---

## 69. Beam Matrix

Paraxiale Laserstrahlen SOLLEN über geeignete Beam-Matrix-Modelle repräsentiert werden können.

---

## 70. Complex Beam Parameter

Der komplexe Beam Parameter wird als:

`q`

repräsentiert.

---

## 71. Complex Beam Parameter Definition

Für Gaussian Beams gilt:

```text
1/q =
1/R
-
i * lambda / (pi * w^2)
```

für die jeweilige verwendete Konvention.

---

## 72. ABCD Propagation

Der komplexe Beam Parameter SOLL durch paraxiale Systeme transformiert werden können:

```text
q2 =
(A * q1 + B) /
(C * q1 + D)
```

---

## 73. Lens Integration

Laser Compute SOLL `NPSPEC-COMPUTE-LENS-0001` integrieren.

---

## 74. Optical Elements

Laserstrahlen SOLLEN durch optische Elemente propagiert werden können wie:

```text
Lens
Mirror
Window
Prism
BeamSplitter
Filter
Aperture
Fiber
CustomElement
```

---

## 75. Thin Lens Propagation

Dünne Linsen SOLLEN über ABCD-Matrizen beziehungsweise exakte optische Modelle verarbeitet werden können.

---

## 76. Thick Lens Propagation

Komplexe Linsensysteme SOLLEN über Lens Compute verarbeitet werden können.

---

## 77. Ray Tracing Integration

Laser Compute SOLL `NPSPEC-COMPUTE-RAYTRACING-0001` integrieren.

---

## 78. Geometric Laser Rays

Laserstrahlen SOLLEN für geeignete Simulationen in optische Rays beziehungsweise Ray Bundles zerlegt werden können.

---

## 79. Wave and Ray Separation

Geometrische Ray-Modelle und kohärente Feldmodelle MÜSSEN unterscheidbar bleiben.

---

## 80. Propagation Model

Die Ausbreitung eines Laserstrahls wird über:

`LaserPropagationModel`

repräsentiert.

---

## 81. Propagation Types

Mindestens folgende Propagationsmodelle SOLLEN architektonisch unterstützt werden:

```text
GeometricRay
GaussianBeam
ABCD
Fresnel
Fraunhofer
AngularSpectrum
BeamPropagationMethod
Custom
```

---

## 82. Propagation Selection

Die verwendete Propagationsmethode MUSS explizit dokumentiert werden.

---

## 83. Free-Space Propagation

Freiraumpropagation MUSS unterstützt werden.

---

## 84. Medium Propagation

Propagation durch homogene Medien SOLL unterstützt werden.

---

## 85. Inhomogeneous Medium

Inhomogene Medien SOLLEN architektonisch unterstützt werden.

---

## 86. Absorption

Absorption während der Propagation SOLL berücksichtigt werden können.

---

## 87. Scattering

Streuung SOLL architektonisch integrierbar sein.

---

## 88. Diffraction

Beugung MUSS für wellenoptische Propagationsmodelle unterstützt werden können.

---

## 89. Fresnel Propagation

Fresnel-Propagation SOLL unterstützt werden.

---

## 90. Fraunhofer Propagation

Fraunhofer-Propagation SOLL unterstützt werden.

---

## 91. Angular Spectrum

Angular-Spectrum-Propagation SOLL unterstützt werden können.

---

## 92. Beam Propagation Method

BPM-basierte Verfahren SOLLEN für geeignete Systeme unterstützt werden können.

---

## 93. Complex Field

Ein kohärentes optisches Feld wird als:

`ComplexOpticalField`

repräsentiert.

---

## 94. Complex Field Components

Ein Complex Optical Field SOLL mindestens enthalten können:

```text
Amplitude
Phase
Wavelength
Sampling
CoordinateSystem
```

---

## 95. Electric Field

Erweiterte Modelle SOLLEN elektrische Feldkomponenten repräsentieren können.

---

## 96. Vector Field

Vektorielle optische Felder SOLLEN architektonisch unterstützt werden.

---

## 97. Sampling Grid

Wellenoptische Simulationen MÜSSEN ihr Sampling Grid explizit definieren.

---

## 98. Spatial Sampling

Räumliche Sample-Abstände MÜSSEN dokumentiert werden.

---

## 99. Aliasing

Numerisches Aliasing MUSS erkannt beziehungsweise durch geeignete Sampling-Regeln begrenzt werden.

---

## 100. Propagation Window

Die physikalische Größe des Simulationsfensters MUSS explizit definiert sein.

---

## 101. Resonator

Ein Laserresonator wird als:

`LaserResonator`

repräsentiert.

---

## 102. Resonator Components

Ein Resonator SOLL enthalten können:

```text
Mirrors
GainMedium
OpticalElements
Apertures
PropagationSections
OutputCoupler
```

---

## 103. Resonator Types

Mindestens folgende Resonatortypen SOLLEN darstellbar sein:

```text
Linear
Ring
Folded
StandingWave
TravelingWave
Custom
```

---

## 104. Resonator Round Trip

Eine vollständige Resonatorumlaufbahn wird als:

`ResonatorRoundTrip`

repräsentiert.

---

## 105. Round-Trip Matrix

Paraxiale Resonatoren SOLLEN über Round-Trip-ABCD-Matrizen analysiert werden können.

---

## 106. Resonator Stability

Resonatorstabilität MUSS analysierbar sein.

---

## 107. Two-Mirror Stability

Für einen einfachen Zwei-Spiegel-Resonator können definiert werden:

```text
g1 = 1 - L / R1
g2 = 1 - L / R2
```

---

## 108. Stability Criterion

Für einen idealisierten Zwei-Spiegel-Resonator gilt als Stabilitätsbedingung:

```text
0 <= g1 * g2 <= 1
```

Grenzfälle SOLLEN gesondert behandelbar sein.

---

## 109. Stable Resonator

Ein stabiler Resonator SOLL seine Eigenmode bestimmen können.

---

## 110. Unstable Resonator

Instabile Resonatoren MÜSSEN als solche diagnostizierbar sein.

---

## 111. Resonator Mode

Eine Resonatormode wird als:

`LaserMode`

repräsentiert.

---

## 112. Transverse Modes

Mindestens folgende transversale Moden SOLLEN unterstützt werden:

```text
TEM00
HermiteGaussian
LaguerreGaussian
Custom
```

---

## 113. Longitudinal Modes

Longitudinale Resonatormoden SOLLEN berechnet werden können.

---

## 114. Free Spectral Range

Free Spectral Range wird als:

`FSR`

repräsentiert.

---

## 115. FSR

Für einen einfachen linearen Resonator kann näherungsweise gelten:

```text
FSR = c / (2 * n * L)
```

---

## 116. Resonator Loss

Resonatorverluste SOLLEN modellierbar sein.

---

## 117. Loss Types

Verluste können umfassen:

```text
MirrorLoss
AbsorptionLoss
ScatteringLoss
DiffractionLoss
ApertureLoss
OutputCouplingLoss
CustomLoss
```

---

## 118. Output Coupler

Der Auskoppelspiegel wird als:

`LaserOutputCoupler`

repräsentiert.

---

## 119. Reflectivity

Reflektivität und Transmission MÜSSEN spektral modellierbar sein.

---

## 120. Gain Medium

Das aktive Lasermedium wird als:

`LaserGainMedium`

repräsentiert.

---

## 121. Gain Medium Properties

Ein Gain Medium SOLL enthalten können:

```text
Material
Length
GainCoefficient
EmissionCrossSection
AbsorptionCrossSection
Lifetime
SaturationIntensity
PumpAbsorption
Temperature
```

---

## 122. Gain Models

Mindestens folgende Gain-Modelle SOLLEN architektonisch unterstützt werden:

```text
ConstantGain
SmallSignalGain
SaturableGain
RateEquationGain
SpatialGain
SpectralGain
Custom
```

---

## 123. Small-Signal Gain

Small-Signal Gain SOLL modellierbar sein.

---

## 124. Gain Saturation

Verstärkungssättigung MUSS architektonisch unterstützt werden.

---

## 125. Saturation Intensity

Saturation Intensity wird als:

`SaturationIntensity`

repräsentiert.

---

## 126. Population Inversion

Populationsinversion SOLL modellierbar sein.

---

## 127. Rate Equations

Laser Rate Equations SOLLEN unterstützt werden können.

---

## 128. Population Variables

Ein Rate-Equation-Modell SOLL Populationen unterschiedlicher Energieniveaus repräsentieren können.

---

## 129. Two-Level Model

Ein Zwei-Niveau-Modell SOLL architektonisch möglich sein.

---

## 130. Three-Level Model

Drei-Niveau-Lasersysteme SOLLEN unterstützt werden können.

---

## 131. Four-Level Model

Vier-Niveau-Lasersysteme SOLLEN unterstützt werden können.

---

## 132. Pump Source

Eine Pumpquelle wird als:

`LaserPumpSource`

repräsentiert.

---

## 133. Pump Types

Pumpquellen SOLLEN beispielsweise darstellen können:

```text
OpticalPump
DiodePump
FlashLampPump
ElectricalPump
ChemicalPump
Custom
```

---

## 134. Pump Power

Pump Power MUSS explizit definierbar sein.

---

## 135. Pump Spectrum

Das Pumpspektrum SOLL modellierbar sein.

---

## 136. Pump Geometry

Die räumliche Pumpverteilung SOLL modellierbar sein.

---

## 137. Pump Absorption

Absorption der Pumpstrahlung im Gain Medium SOLL berechnet werden können.

---

## 138. Pump Efficiency

Pumpwirkungsgrad SOLL als Analysegröße unterstützt werden.

---

## 139. Laser Threshold

Die Laserschwelle SOLL berechnet werden können.

---

## 140. Threshold Condition

Die Schwellbedingung SOLL Gewinne und Rundlaufverluste berücksichtigen.

---

## 141. Above Threshold

Betrieb oberhalb der Laserschwelle SOLL modellierbar sein.

---

## 142. Below Threshold

Ein System unterhalb der Laserschwelle DARF nicht ohne Kennzeichnung als oszillierender Laser behandelt werden.

---

## 143. Laser Efficiency

Laserwirkungsgrade SOLLEN analysierbar sein.

---

## 144. Efficiency Types

Mindestens folgende Wirkungsgrade SOLLEN unterstützt werden können:

```text
OpticalEfficiency
SlopeEfficiency
PumpEfficiency
QuantumEfficiency
WallPlugEfficiency
Custom
```

---

## 145. Output Power

Laser Output Power SOLL aus geeigneten Resonator-, Gain- und Pumpmodellen berechnet werden können.

---

## 146. Intracavity Power

Intracavity Power SOLL separat vom ausgekoppelten Output dargestellt werden.

---

## 147. Intensity

Laserintensität wird als:

`LaserIntensity`

repräsentiert.

---

## 148. Intensity Distribution

Die räumliche Intensitätsverteilung MUSS aus dem gewählten Beam Model berechnet werden können.

---

## 149. Peak Intensity

Peak Intensity SOLL für CW- und Pulsmodelle berechnet werden können.

---

## 150. Fluence

Fluence wird als:

`LaserFluence`

repräsentiert.

---

## 151. Fluence Definition

Fluence beschreibt Energie pro Fläche:

```text
F = E / A
```

für geeignete vereinfachte Modelle.

---

## 152. Irradiance

Irradiance SOLL explizit von Fluence unterschieden werden.

---

## 153. Power Density

Leistungsdichte SOLL berechnet werden können.

---

## 154. Focused Beam

Fokussierte Laserstrahlen SOLLEN über Lens Compute analysiert werden können.

---

## 155. Focus Spot

Der Fokusspot wird als:

`LaserFocusSpot`

repräsentiert.

---

## 156. Focus Properties

Ein Focus Spot SOLL enthalten können:

```text
Position
Radius
Diameter
PeakIntensity
RayleighRange
M2
```

---

## 157. Diffraction-Limited Focus

Das theoretische Beugungslimit SOLL bestimmbar sein.

---

## 158. Real Beam Focus

Reale Fokusberechnungen SOLLEN M² und optische Aberrationen berücksichtigen können.

---

## 159. Beam Expander

Beam Expander SOLLEN modellierbar sein.

---

## 160. Beam Compressor

Optische Strahlkompressionssysteme SOLLEN modellierbar sein.

---

## 161. Collimation

Strahlkollimation SOLL analysierbar sein.

---

## 162. Fiber Coupling

Laser-Einkopplung in optische Fasern SOLL architektonisch unterstützt werden.

---

## 163. Coupling Efficiency

Kopplungseffizienz SOLL berechnet werden können.

---

## 164. Mode Overlap

Mode-Overlap-Integrale SOLLEN für geeignete Kopplungsprobleme unterstützt werden.

---

## 165. Polarization

Laserpolarisation wird als:

`LaserPolarization`

repräsentiert.

---

## 166. Polarization States

Mindestens folgende Zustände SOLLEN unterstützt werden:

```text
Linear
Circular
Elliptical
UnpolarizedApproximation
Custom
```

---

## 167. Jones Vector

Kohärente vollständig polarisierte Strahlung SOLL über Jones-Vektoren repräsentiert werden können.

---

## 168. Jones Matrix

Optische Elemente SOLLEN über Jones-Matrizen modellierbar sein.

---

## 169. Stokes Parameters

Stokes-Parameter SOLLEN unterstützt werden können.

---

## 170. Mueller Matrix

Mueller-Matrix-Verfahren SOLLEN architektonisch unterstützt werden.

---

## 171. Interference

Kohärente Interferenz MUSS architektonisch unterstützt werden.

---

## 172. Field Superposition

Kohärente Felder MÜSSEN als komplexe Amplituden überlagert werden können.

---

## 173. Interferometer

Interferometrische Systeme SOLLEN modellierbar sein.

---

## 174. Interferometer Types

Beispiele:

```text
Michelson
MachZehnder
FabryPerot
Sagnac
Custom
```

---

## 175. Optical Path Difference

Interferenzberechnungen SOLLEN Optical Path Difference berücksichtigen.

---

## 176. Fringe Pattern

Interferenzmuster SOLLEN berechnet werden können.

---

## 177. Nonlinear Optics

Nichtlineare Optik SOLL als erweiterte Laser-Compute-Fähigkeit unterstützt werden.

---

## 178. Nonlinear Medium

Ein nichtlineares Medium wird als:

`NonlinearOpticalMedium`

repräsentiert.

---

## 179. Nonlinear Processes

Mindestens folgende Prozesse SOLLEN architektonisch darstellbar sein:

```text
SecondHarmonicGeneration
ThirdHarmonicGeneration
SumFrequencyGeneration
DifferenceFrequencyGeneration
OpticalParametricGeneration
SelfPhaseModulation
CrossPhaseModulation
FourWaveMixing
KerrEffect
Custom
```

---

## 180. Second Harmonic Generation

SHG SOLL als Frequenzverdopplungsprozess modellierbar sein.

---

## 181. Frequency Conversion

Frequenzkonversionswirkungsgrad SOLL berechnet werden können.

---

## 182. Phase Matching

Phase Matching SOLL berücksichtigt werden können.

---

## 183. Quasi-Phase Matching

Quasi-Phase Matching SOLL architektonisch unterstützt werden.

---

## 184. Nonlinear Coefficients

Nichtlineare Materialkoeffizienten MÜSSEN mit Einheiten und Materialversion dokumentiert werden.

---

## 185. Kerr Effect

Intensitätsabhängige Brechungsindizes SOLLEN unterstützt werden können.

---

## 186. Self-Focusing

Selbstfokussierung SOLL architektonisch modellierbar sein.

---

## 187. Self-Phase Modulation

Self-Phase Modulation SOLL für Pulse unterstützt werden können.

---

## 188. Dispersion

Zeitliche Pulse SOLLEN Materialdispersion berücksichtigen können.

---

## 189. Group Velocity

Gruppengeschwindigkeit SOLL berechnet werden können.

---

## 190. Group Delay

Group Delay SOLL unterstützt werden.

---

## 191. Group Velocity Dispersion

GVD SOLL modellierbar sein.

---

## 192. Pulse Broadening

Dispersionbedingte Pulsverbreiterung SOLL simuliert werden können.

---

## 193. Pulse Compression

Dispersionskompensation und Pulse Compression SOLLEN modellierbar sein.

---

## 194. Thermal Effects

Thermische Effekte SOLLEN architektonisch unterstützt werden.

---

## 195. Thermal Lens

Thermal Lensing wird als:

`ThermalLens`

repräsentiert.

---

## 196. Temperature Field

Ein räumliches Temperaturfeld SOLL mit dem Laser-Modell gekoppelt werden können.

---

## 197. Thermo-Optic Coefficient

Temperaturabhängigkeit des Brechungsindex SOLL unterstützt werden.

---

## 198. Thermal Expansion

Thermische Expansion optischer Komponenten SOLL architektonisch berücksichtigt werden können.

---

## 199. Coupled Simulation

Laser Compute SOLL gekoppelte Simulationen mit anderen Compute-Modulen ermöglichen.

---

## 200. Coupling Domains

Geeignete Kopplungen umfassen:

```text
Optics
Heat Transfer
Fluid Dynamics
Structural Mechanics
Electromagnetics
Material Models
Medical Simulation
```

---

## 201. Heat Transfer Integration

Laserbedingte Wärmequellen SOLLEN an Heat-Transfer-Compute übergeben werden können.

---

## 202. Absorbed Power

Absorbierte optische Leistung SOLL als räumliche Wärmequelle ausgegeben werden können.

---

## 203. Material Interaction

Laser-Material-Interaktion wird als:

`LaserMaterialInteraction`

repräsentiert.

---

## 204. Interaction Inputs

Eine Interaktion SOLL berücksichtigen können:

```text
Wavelength
Power
Intensity
Fluence
PulseDuration
RepetitionRate
Material
IncidenceAngle
SpotSize
```

---

## 205. Interaction Models

Architektonisch SOLLEN Modelle unterstützt werden können für:

```text
Absorption
Reflection
Heating
Melting
Vaporization
Ablation
PhotochemicalInteraction
PlasmaExtension
Custom
```

---

## 206. Model Scope

Laser Compute MUSS klar kennzeichnen, welches physikalische Interaktionsmodell verwendet wird.

---

## 207. No Implicit Material Damage

Eine optische Intensitätsberechnung DARF nicht automatisch als vollständige Materialschädigungsprognose interpretiert werden.

---

## 208. Damage Threshold Data

Materialschwellenwerte SOLLEN als versionierte Materialdaten referenzierbar sein.

---

## 209. Detector

Ein Laser-Detektor wird als:

`LaserDetector`

repräsentiert.

---

## 210. Detector Types

Detektoren SOLLEN beispielsweise darstellen können:

```text
PowerMeter
Photodiode
Camera
PositionSensor
Spectrometer
InterferometricDetector
Custom
```

---

## 211. Detector Geometry

Detektorgeometrie MUSS explizit definierbar sein.

---

## 212. Spectral Response

Spektrale Empfindlichkeit SOLL modellierbar sein.

---

## 213. Detector Saturation

Detektorsättigung SOLL berücksichtigt werden können.

---

## 214. Detector Noise

Rauschmodelle SOLLEN optional unterstützt werden.

---

## 215. Beam Analysis

Eine Laserstrahlanalyse wird als:

`LaserBeamAnalysis`

repräsentiert.

---

## 216. Analysis Types

Mindestens folgende Analysen SOLLEN unterstützt werden können:

```text
BeamRadius
BeamWaist
Divergence
M2
BeamProfile
Power
Energy
Intensity
Fluence
Focus
Spectrum
Pulse
Polarization
Coherence
Resonator
Stability
Mode
Interference
Thermal
Custom
```

---

## 217. Beam Radius Measurement

Beam Radius SOLL aus simulierten beziehungsweise gemessenen Intensitätsdaten bestimmt werden können.

---

## 218. Beam Centroid

Der Strahlschwerpunkt SOLL berechnet werden können.

---

## 219. Second-Moment Beam Width

Second-Moment-Beam-Width-Methoden SOLLEN unterstützt werden können.

---

## 220. M² Analysis

M² SOLL über geeignete Mehrpositionsmessungen beziehungsweise Simulationen bestimmt werden können.

---

## 221. Beam Profile Result

Ein Beam Profile Result SOLL enthalten können:

```text
IntensityMap
PhaseMap
Centroid
RadiusX
RadiusY
Ellipticity
Orientation
```

---

## 222. Caustic

Die Strahlkaustik wird als:

`LaserBeamCaustic`

repräsentiert.

---

## 223. Caustic Sampling

Beam Radius SOLL entlang der Propagationsachse gesampelt werden können.

---

## 224. Focus Search

Die Position minimaler Beam Width SOLL numerisch bestimmbar sein.

---

## 225. Resonator Analysis

Resonatoranalysen SOLLEN mindestens liefern können:

```text
Stability
ModeSize
WaistPosition
WaistRadius
RoundTripLoss
FSR
```

---

## 226. Pulse Analysis

Pulse Analysis SOLL mindestens liefern können:

```text
PulseDuration
PulseEnergy
PeakPower
AveragePower
RepetitionRate
Spectrum
TimeBandwidthProduct
```

---

## 227. Time-Bandwidth Product

Das Time-Bandwidth Product SOLL für geeignete Pulsmodelle berechnet werden können.

---

## 228. Transform-Limited Pulse

Transform-limitierte Pulse SOLLEN von gechirpten Pulsen unterscheidbar sein.

---

## 229. Optimization

Laser Compute SOLL Optimierungsaufgaben unterstützen können.

---

## 230. Optimization Variables

Optimierbare Parameter können umfassen:

```text
BeamWaist
LensPosition
LensFocalLength
MirrorRadius
ResonatorLength
OutputCoupling
PumpPower
GainLength
Aperture
FocusPosition
PulseParameters
```

---

## 231. Optimization Objectives

Optimierungsziele können umfassen:

```text
MinimumSpotSize
MaximumOutputPower
MaximumCouplingEfficiency
MinimumDivergence
DesiredBeamRadius
ResonatorStability
MaximumConversionEfficiency
MinimumAberration
Custom
```

---

## 232. Constraints

Optimierungen SOLLEN physikalische und technische Constraints unterstützen.

---

## 233. Scheduler Integration

Laser Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 234. Laser Tasks

Mindestens folgende Tasks SOLLEN planbar sein:

```text
BeamPropagation
RayTrace
FieldPropagation
ResonatorAnalysis
ModeAnalysis
PulsePropagation
SpectrumAnalysis
ThermalCoupling
ParameterSweep
Optimization
MonteCarloAnalysis
```

---

## 235. Parallel Beam Propagation

Unabhängige Beam-Samples SOLLEN parallel verarbeitet werden können.

---

## 236. Parallel Wavelengths

Mehrere Wellenlängen SOLLEN parallel verarbeitet werden können.

---

## 237. Parallel Parameter Sweeps

Parameter-Sweeps SOLLEN parallel ausführbar sein.

---

## 238. Parallel Monte Carlo

Monte-Carlo-Simulationen SOLLEN massiv parallel ausführbar sein.

---

## 239. Cancellation

Lange Laser-Compute-Jobs MÜSSEN abbrechbar sein.

---

## 240. Memory Model Integration

Laser Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 241. Memory Objects

Mindestens folgende Speicherobjekte SOLLEN unterstützt werden:

```text
BeamBuffer
RayBuffer
FieldBuffer
PhaseBuffer
SpectrumBuffer
PulseBuffer
MaterialBuffer
ResonatorBuffer
DetectorBuffer
AnalysisBuffer
```

---

## 242. Large Field Data

Große 2D- und 3D-Felddaten SOLLEN speichereffizient verarbeitet werden.

---

## 243. Streaming

Große Parameter-Sweeps beziehungsweise Felddaten SOLLEN streambar sein.

---

## 244. Object Model Integration

Laser Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0004` integrieren.

---

## 245. Compute Objects

Mindestens folgende Objekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen:

```text
LaserSystem
LaserSource
LaserBeam
LaserPulse
LaserSpectrum
LaserResonator
LaserMode
LaserGainMedium
LaserPumpSource
LaserDetector
LaserBeamAnalysis
LaserResult
```

---

## 246. GPU Runtime Integration

Laser Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` integrieren.

---

## 247. GPU Acceleration

Geeignete GPU-Operationen umfassen:

```text
Ray Propagation
Field Propagation
FFT
Angular Spectrum
Fresnel Propagation
Beam Profile Evaluation
Parameter Sweeps
Monte Carlo
Optimization Evaluation
```

---

## 248. CPU Backend

Eine CPU-basierte Implementierung MUSS möglich sein.

---

## 249. SIMD

Geeignete Ray-, Field- und Sample-Berechnungen SOLLEN SIMD verwenden können.

---

## 250. GPU Fallback

Laser Compute DARF keine dedizierte GPU voraussetzen.

---

## 251. HPC Integration

Laser Compute SOLL `NPSPEC-COMPUTE-HPC-0001` integrieren können.

---

## 252. Distributed Compute

Große:

```text
Field Propagations
Parameter Sweeps
Monte Carlo Runs
Optimization Runs
Thermal Couplings
Nonlinear Simulations
```

SOLLTEN verteilt ausführbar sein.

---

## 253. Precision

Laser Compute MUSS numerische Präzision explizit verwalten.

---

## 254. FP64

FP64 SOLL für hochpräzise optische Berechnungen unterstützt werden.

---

## 255. FP32

FP32 DARF für geeignete hochparallele Simulationen verwendet werden.

---

## 256. Mixed Precision

Mixed Precision SOLL unterstützt werden können.

---

## 257. Complex Precision

Komplexwertige Berechnungen SOLLEN mindestens äquivalente:

```text
Complex32
Complex64
```

beziehungsweise geeignete Backend-Repräsentationen unterstützen können.

---

## 258. Precision Selection

Der Präzisionsmodus MUSS konfigurierbar sein.

---

## 259. Numerical Stability

Laserberechnungen MÜSSEN numerisch robuste Verfahren verwenden.

---

## 260. Extreme Dynamic Range

Sehr große Intensitäts- beziehungsweise Leistungsunterschiede MÜSSEN kontrolliert verarbeitet werden können.

---

## 261. Phase Wrapping

Phasenwerte SOLLEN konsistent behandelt werden.

---

## 262. Phase Unwrapping

Phase-Unwrapping SOLL für geeignete Analysen unterstützt werden können.

---

## 263. Invalid Beam

Ungültige Beam-Parameter MÜSSEN erkannt werden.

---

## 264. Invalid Wavelength

Ungültige Wellenlängen MÜSSEN erkannt werden.

---

## 265. Invalid Power

Nicht endliche beziehungsweise unzulässige Leistungswerte MÜSSEN erkannt werden.

---

## 266. Invalid Pulse

Ungültige Pulsparameter MÜSSEN diagnostiziert werden.

---

## 267. Invalid Resonator

Physikalisch beziehungsweise geometrisch ungültige Resonatorkonfigurationen MÜSSEN erkannt werden.

---

## 268. Energy Conservation

Compute-Modelle SOLLEN Energieerhaltung prüfen können, sofern das verwendete physikalische Modell dies verlangt.

---

## 269. Power Balance

Resonator- und Propagationsmodelle SOLLEN Power-Balance-Diagnosen bereitstellen können.

---

## 270. Laser Result

Ein Berechnungsergebnis wird als:

`LaserResult`

repräsentiert.

---

## 271. Result Metadata

Ein Result SOLL mindestens enthalten können:

```text
SystemId
SystemVersion
AnalysisType
Wavelength
BeamModel
PropagationModel
Precision
Backend
Timestamp
```

---

## 272. Extended Result Metadata

Zusätzlich SOLLEN dokumentiert werden können:

```text
Power
PulseParameters
Polarization
Environment
OpticalConfiguration
MaterialVersions
Sampling
```

---

## 273. Result Provenance

Jedes Ergebnis MUSS auf die verwendete Laser-System-Version zurückführbar sein.

---

## 274. Approximation State

Ergebnisse SOLLEN angeben können, ob sie basieren auf:

```text
Analytical
Paraxial
Geometric
ScalarWave
VectorWave
Numerical
Hybrid
```

---

## 275. No Approximation Confusion

Ein vereinfachtes Gaussian-Beam-Ergebnis DARF nicht ohne Kennzeichnung als vollständige Maxwell-Feldlösung dargestellt werden.

---

## 276. Quality State

Ein Laser Result SOLL einen Qualitätszustand besitzen können.

---

## 277. Quality States

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

## 278. Provenance

Laser Compute MUSS Provenance unterstützen.

---

## 279. Provenance Record

Ein:

`LaserComputeProvenanceRecord`

SOLL mindestens enthalten:

```text
SystemId
SystemVersion
SourceVersion
MaterialVersions
OpticalSystemVersion
Analysis
BeamModel
PropagationModel
Wavelength
Precision
Backend
RuntimeVersion
Timestamp
```

---

## 280. Extended Provenance

Zusätzlich SOLLEN dokumentiert werden können:

```text
Temperature
Pressure
PumpConfiguration
ResonatorConfiguration
PulseConfiguration
Sampling
RandomSeed
SolverParameters
OptimizationState
```

---

## 281. Diagnostics

Laser Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 282. Diagnostic Classes

Mindestens folgende Diagnosen SOLLEN existieren:

```text
INVALID_LASER_SYSTEM
INVALID_SOURCE
INVALID_BEAM
INVALID_BEAM_RADIUS
INVALID_WAVELENGTH
INVALID_FREQUENCY
INVALID_POWER
INVALID_ENERGY
INVALID_PULSE
INVALID_SPECTRUM
INVALID_POLARIZATION
INVALID_GAIN_MEDIUM
INVALID_PUMP
INVALID_RESONATOR
UNSTABLE_RESONATOR
INVALID_OPTICAL_ELEMENT
INVALID_DETECTOR
INVALID_SAMPLING
SAMPLING_ALIASING
NUMERICAL_FAILURE
NON_CONVERGENCE
ENERGY_BALANCE_ERROR
UNSUPPORTED_BEAM_MODEL
UNSUPPORTED_PROPAGATION_MODEL
UNSUPPORTED_ANALYSIS
OUT_OF_MEMORY
BACKEND_FAILURE
CANCELLED
```

---

## 283. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 284. Diagnostic Context

Diagnosen SOLLEN enthalten können:

```text
SystemId
BeamId
PulseId
ResonatorId
ElementId
Wavelength
Configuration
Backend
Timestamp
```

---

## 285. Profiling

Laser Compute MUSS Profiling unterstützen.

---

## 286. Profiling Stages

Mindestens folgende Bereiche SOLLTEN separat messbar sein:

```text
SystemPreparation
BeamGeneration
RayPropagation
FieldPropagation
OpticalInteraction
ResonatorAnalysis
ModeCalculation
PulsePropagation
FFT
DetectorEvaluation
ThermalCoupling
OptimizationEvaluation
```

---

## 287. Ray Count

Bei ray-basierten Simulationen SOLL die Anzahl verarbeiteter Rays messbar sein.

---

## 288. Field Sample Count

Bei feldbasierten Simulationen SOLL die Anzahl verarbeiteter Field Samples messbar sein.

---

## 289. Propagation Time

Propagationszeit SOLL messbar sein.

---

## 290. FFT Time

FFT-basierte Propagationszeiten SOLLEN separat messbar sein.

---

## 291. Memory Usage

Speicherverbrauch SOLL messbar sein.

---

## 292. GPU Usage

GPU-Auslastung SOLL bei geeigneten Backends messbar sein.

---

## 293. Determinism

Laser Compute SOLL einen deterministischen Modus unterstützen.

---

## 294. Deterministic Inputs

Deterministische Ausführung SOLL mindestens berücksichtigen:

```text
SystemVersion
BeamDefinition
PulseDefinition
PropagationModel
Sampling
RandomSeed
Precision
BackendConfiguration
```

---

## 295. Random Seed

Stochastische Verfahren MÜSSEN explizite Random Seeds unterstützen.

---

## 296. Backend Equivalence

CPU- und GPU-Ergebnisse SOLLTEN innerhalb definierter numerischer Toleranzen vergleichbar sein.

---

## 297. Security

Laser Compute MUSS in die allgemeine NovaOS-Compute-Sicherheitsarchitektur integriert werden.

---

## 298. Input Validation

Externe Laser-, Material- und Simulationsdaten MÜSSEN validiert werden.

---

## 299. Resource Limits

Simulationen MÜSSEN Ressourcenlimits unterstützen.

---

## 300. Grid Limits

Extrem große Feldgitter MÜSSEN gegen Speicher- und Compute-Limits validiert werden.

---

## 301. Iteration Limits

Iterative Solver MÜSSEN maximale Iterationszahlen unterstützen.

---

## 302. Optimization Limits

Optimierungsjobs MÜSSEN begrenzbar und abbrechbar sein.

---

## 303. Safety Metadata

Laser Compute SOLL technische Laser-Sicherheitsmetadaten transportieren können.

---

## 304. Safety Separation

Laser-Sicherheitsklassifikation und physikalische Simulation MÜSSEN logisch getrennt sein.

---

## 305. No Automatic Safety Certification

Ein Simulationsergebnis DARF nicht automatisch als sicherheitstechnische Zertifizierung interpretiert werden.

---

## 306. Testing — Gaussian Beam

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
BeamWaist
RayleighRange
NearField
FarField
Divergence
RadiusOfCurvature
GouyPhase
```

---

## 307. Testing — M²

Mindestens:

```text
IdealM2
NonIdealM2
AstigmaticM2
InvalidM2
```

MÜSSEN getestet werden.

---

## 308. Testing — ABCD

Mindestens:

```text
FreeSpace
ThinLens
TwoLensSystem
BeamExpander
FocusedBeam
```

MÜSSEN getestet werden.

---

## 309. Testing — Spectrum

Mindestens:

```text
SingleFrequency
FiniteLinewidth
GaussianSpectrum
MeasuredSpectrum
```

SOLLTEN getestet werden.

---

## 310. Testing — Pulse

Mindestens:

```text
GaussianPulse
SechSquaredPulse
PulseEnergy
PeakPower
AveragePower
RepetitionRate
ChirpedPulse
```

MÜSSEN getestet werden.

---

## 311. Testing — Resonator

Mindestens:

```text
StableResonator
UnstableResonator
Confocal
Planar
ConcentricLimit
RoundTrip
```

MÜSSEN getestet werden.

---

## 312. Testing — Gain

Mindestens:

```text
NoGain
SmallSignalGain
SaturatedGain
BelowThreshold
AboveThreshold
```

SOLLTEN getestet werden.

---

## 313. Testing — Propagation

Mindestens:

```text
FreeSpace
HomogeneousMedium
LensPropagation
ApertureClipping
LongDistance
```

MÜSSEN getestet werden.

---

## 314. Testing — Wave Propagation

Mindestens:

```text
Fresnel
Fraunhofer
AngularSpectrum
KnownDiffractionPattern
```

SOLLTEN getestet werden.

---

## 315. Testing — Polarization

Mindestens:

```text
Linear
Circular
Elliptical
JonesMatrix
```

SOLLTEN getestet werden.

---

## 316. Testing — Interference

Mindestens:

```text
TwoBeamInterference
EqualPath
KnownPhaseDifference
Michelson
```

SOLLTEN getestet werden.

---

## 317. Testing — Nonlinear

Erweiterte Implementierungen SOLLTEN mindestens testen:

```text
SecondHarmonicGeneration
PhaseMatching
SelfPhaseModulation
```

---

## 318. Testing — Detector

Mindestens:

```text
PowerDetector
SpatialDetector
SpectralResponse
DetectorSaturation
```

SOLLTEN getestet werden.

---

## 319. Testing — Numerical Sampling

Mindestens:

```text
AdequateSampling
Undersampling
Aliasing
LargeGrid
SmallGrid
```

MÜSSEN getestet werden.

---

## 320. Testing — CPU/GPU

CPU- und GPU-Ergebnisse SOLLTEN innerhalb definierter Toleranzen verglichen werden.

---

## 321. Testing — Determinism

Identische deterministische Inputs MÜSSEN reproduzierbare Ergebnisse liefern.

---

## 322. Testing — Invalid Inputs

Mindestens:

```text
ZeroWavelength
NegativePower
InvalidBeamRadius
InvalidM2
InvalidPulseDuration
InvalidRepetitionRate
InvalidResonator
InvalidGrid
NaNInput
InfinityInput
```

MÜSSEN getestet werden.

---

## 323. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
MillionsOfRays
LargeFieldGrid
ManyWavelengths
ManyPulseSamples
ManyOpticalElements
LongPropagationDistance
LargeParameterSweep
LargeMonteCarloRun
LongOptimization
ComplexResonator
NonlinearPropagation
ThermalCoupling
GPU Memory Pressure
CPU Memory Pressure
LongRunningSimulation
```

---

## 324. Performance Tests

Mindestens folgende Kennzahlen SOLLTEN erhoben werden:

```text
RaysPerSecond
FieldSamplesPerSecond
PropagationTime
FFTTime
ResonatorAnalysisTime
PulseSimulationTime
MemoryUsage
GPUUsage
CPUUsage
OptimizationIterationsPerSecond
```

---

## 325. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
LaserSystem
LaserSource
LaserBeam
LaserSpectrum
LaserPulse

ContinuousWave
Pulsed

Gaussian Beam
Beam Waist
Beam Radius
Rayleigh Range
Divergence
Radius of Curvature
Gouy Phase

M2
Beam Parameter Product

Complex Beam Parameter
ABCD Propagation

Free-Space Propagation
Basic Lens Propagation

Basic Spectrum
Basic Coherence Architecture

Basic Polarization

Laser Resonator
Basic Stability Analysis

Laser Detector
Power
Intensity
Fluence

CPU Backend
GPU Backend Architecture

Lens Compute Integration
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

## 326. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Astigmatic Beams
Hermite-Gaussian Modes
Laguerre-Gaussian Modes
Measured Beam Profiles

Fresnel Propagation
Fraunhofer Propagation
Angular Spectrum
Beam Propagation Method

Complex Optical Fields
Vector Fields

Gain Media
Pump Sources
Rate Equations
Gain Saturation
Laser Threshold
Output Power

Advanced Resonators
Ring Resonators
Mode Analysis

Ultrashort Pulses
Chirp
Spectral Phase
GVD
Pulse Compression

Jones Calculus
Stokes Parameters
Mueller Matrices

Interference
Interferometers

Nonlinear Optics
SHG
Frequency Conversion
Kerr Effect
Self-Phase Modulation

Thermal Lensing
Heat Transfer Coupling

Laser Material Interaction

Parameter Sweeps
Optimization
Monte Carlo

GPU Acceleration
HPC
Distributed Compute
```

---

## 327. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-LASER-0001` MUSS:

1. Lasersysteme repräsentieren.
2. Lasersysteme eindeutig identifizieren.
3. Systemversionen unterstützen.
4. Laserquellen repräsentieren.
5. Laserstrahlen repräsentieren.
6. Wellenlängen explizit definieren.
7. Frequenzen aus Wellenlängen beziehungsweise umgekehrt konsistent bestimmen können.
8. Einheiten explizit verwalten.
9. Laserleistung repräsentieren.
10. Laserenergie repräsentieren können.
11. CW-Betrieb unterstützen.
12. gepulsten Betrieb unterstützen.
13. Pulse Energy unterstützen.
14. Pulse Duration unterstützen.
15. Repetition Rate unterstützen.
16. Peak Power bestimmen können.
17. Average Power bestimmen können.
18. Spektren repräsentieren.
19. spektrale Bandbreite unterstützen.
20. Kohärenz architektonisch unterstützen.
21. Phaseninformation architektonisch unterstützen.
22. Gaussian Beams unterstützen.
23. Beam Waist unterstützen.
24. Beam Radius unterstützen.
25. Rayleigh Range berechnen.
26. Beam Divergence berechnen.
27. Radius of Curvature berechnen können.
28. Gouy Phase berechnen können.
29. M² unterstützen.
30. Beam Parameter Product unterstützen.
31. komplexe Beam Parameter unterstützen.
32. ABCD-Propagation unterstützen.
33. Freiraumpropagation unterstützen.
34. optische Linsen integrieren können.
35. `NPSPEC-COMPUTE-LENS-0001` integrieren.
36. `NPSPEC-COMPUTE-RAYTRACING-0001` integrieren können.
37. geometrische und wellenoptische Modelle unterscheiden.
38. das verwendete Propagationsmodell dokumentieren.
39. Laserresonatoren repräsentieren.
40. Resonatorstabilität analysieren.
41. stabile und instabile Resonatoren unterscheiden.
42. Resonatormoden architektonisch unterstützen.
43. Gain Media architektonisch unterstützen.
44. Pump Sources architektonisch unterstützen.
45. Laserschwellen architektonisch berechnen können.
46. Intensität berechnen können.
47. Fluence berechnen können.
48. Irradiance und Fluence unterscheiden.
49. fokussierte Strahlen analysieren können.
50. Polarisation architektonisch unterstützen.
51. Laser-Detektoren repräsentieren.
52. Beam Analysis unterstützen.
53. Beam Centroid berechnen können.
54. Strahlbreite bestimmen können.
55. `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.
56. `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.
57. `NPSPEC-COMPUTE-RUNTIME-0004` integrieren.
58. CPU-Ausführung unterstützen.
59. GPU-Ausführung architektonisch unterstützen.
60. keine GPU voraussetzen.
61. FP64 unterstützen.
62. komplexwertige Berechnungen unterstützen können.
63. numerisch ungültige Eingaben erkennen.
64. NaN- und Infinity-Werte kontrolliert behandeln.
65. ungültige Wellenlängen erkennen.
66. ungültige Leistungswerte erkennen.
67. ungültige Beam-Parameter erkennen.
68. ungültige Resonatoren erkennen.
69. Sampling-Parameter explizit verwalten.
70. numerisches Aliasing diagnostizieren können.
71. lange Jobs abbrechen können.
72. deterministische Ausführung unterstützen können.
73. Random Seeds für stochastische Verfahren unterstützen.
74. Result Provenance unterstützen.
75. Systemversionen in Ergebnissen dokumentieren.
76. Beam Model dokumentieren.
77. Propagation Model dokumentieren.
78. Präzision dokumentieren.
79. Backend dokumentieren.
80. Approximation States dokumentieren.
81. analytische und numerische Ergebnisse unterscheiden.
82. strukturierte Diagnosen bereitstellen.
83. Profiling unterstützen.
84. Propagationszeit messen können.
85. Speicherverbrauch messen können.
86. Ressourcenlimits unterstützen.
87. gekoppelte Simulationen architektonisch unterstützen.
88. thermische Effekte architektonisch unterstützen.
89. nichtlineare Optik architektonisch unterstützen.
90. Erweiterungen ohne Änderung des grundlegenden Laser-Objektmodells ermöglichen.

---

## 328. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaLaserCompute
 |
 +-- LaserRuntime
 |
 +-- LaserSystemManager
 |
 +-- SourceEngine
 |
 +-- BeamEngine
 |    |
 |    +-- GaussianBeam
 |    +-- SuperGaussianBeam
 |    +-- HermiteGaussianBeam
 |    +-- LaguerreGaussianBeam
 |    +-- CustomBeam
 |
 +-- SpectrumEngine
 |
 +-- PulseEngine
 |
 +-- CoherenceEngine
 |
 +-- PolarizationEngine
 |
 +-- PropagationEngine
 |    |
 |    +-- RayPropagation
 |    +-- GaussianPropagation
 |    +-- ABCDPropagation
 |    +-- FresnelPropagation
 |    +-- FraunhoferPropagation
 |    +-- AngularSpectrumPropagation
 |    +-- BPMPropagation
 |
 +-- ResonatorEngine
 |
 +-- ModeSolver
 |
 +-- GainEngine
 |
 +-- PumpEngine
 |
 +-- RateEquationSolver
 |
 +-- InterferenceEngine
 |
 +-- NonlinearOpticsEngine
 |
 +-- ThermalCouplingEngine
 |
 +-- MaterialInteractionEngine
 |
 +-- DetectorEngine
 |
 +-- BeamAnalysisEngine
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

## 329. Beziehung zu `NPSPEC-COMPUTE-LENS-0001`

Lens Compute beschreibt optische Oberflächen und Abbildungssysteme.

Laser Compute ergänzt diese um Laserstrahlsemantik:

```text
Optical System
      |
      v
Lens Compute
      |
      v
Laser Beam
      |
      +-- Wavelength
      +-- Beam Profile
      +-- Beam Waist
      +-- Divergence
      +-- M2
      +-- Phase
      +-- Polarization
      +-- Power
```

---

## 330. Beziehung zu `NPSPEC-COMPUTE-RAYTRACING-0001`

Ray Tracing Compute kann geometrische Laserstrahlen und komplexe optische Systeme räumlich verfolgen.

Laser Compute ergänzt:

```text
Coherence
Phase
Beam Profile
Power
Pulse
Spectrum
Polarization
Gain
Resonator Physics
```

---

## 331. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt die gemeinsame Ausführungsinfrastruktur bereit.

```text
Laser Compute
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

## 332. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert insbesondere:

```text
Beam Propagation
Field Propagation
Ray Bundles
Wavelengths
Pulse Samples
Parameter Sweeps
Optimization Runs
Monte Carlo Runs
```

---

## 333. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet insbesondere:

```text
Beams
Rays
Fields
Spectra
Pulses
Materials
Resonators
Detector Data
Analysis Results
```

---

## 334. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Laser-Compute-Ressourcen SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 335. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

Die GPU Runtime kann insbesondere beschleunigen:

```text
Ray Propagation
Field Propagation
FFT
Diffraction
Parameter Sweeps
Monte Carlo
Optimization
```

---

## 336. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

HPC Compute kann große Laser-, Feld-, Parameter- und Optimierungssimulationen verteilen.

---

## 337. Grundlegende Laser-Compute-Architektur

```text
Laser Definition
       |
       v
Source
       |
       v
Beam / Pulse
       |
       v
Propagation
       |
       v
Optical System
       |
       v
Interaction
       |
       v
Detector
       |
       v
Analysis
```

---

## 338. Gaussian-Beam-Architektur

```text
Beam Waist
    |
    +-- w0
    +-- Position
    +-- Wavelength
    +-- M2
    |
    v
Beam Parameter
    |
    v
Propagation
    |
    +-- Beam Radius
    +-- Curvature
    +-- Gouy Phase
    +-- Divergence
```

---

## 339. Resonator-Architektur

```text
Pump
 |
 v
Gain Medium
 |
 v
Resonator
 |
 +----------------------+
 |                      |
 v                      |
Propagation             |
 |                      |
 v                      |
Mirror / Optics         |
 |                      |
 v                      |
Gain / Loss             |
 |                      |
 +----------------------+
 |
 v
Output Coupler
 |
 v
Laser Output
```

---

## 340. Pulsed-Laser-Architektur

```text
Pulse Definition
      |
      +-- Energy
      +-- Duration
      +-- Spectrum
      +-- Phase
      +-- Chirp
      |
      v
Temporal Field
      |
      v
Propagation
      |
      v
Dispersion / Nonlinearity
      |
      v
Output Pulse
```

---

## 341. Wave-Propagation-Architektur

```text
Complex Input Field
        |
        v
Sampling Validation
        |
        v
Propagation Operator
        |
   +----+----+----------+
   |         |          |
   v         v          v
Fresnel   Angular     BPM
           Spectrum
   |         |          |
   +----+----+----------+
        |
        v
Complex Output Field
        |
        v
Intensity / Phase
```

---

## 342. Datenmodellgrundsatz

Laser Compute MUSS zwischen:

```text
Laser System
Source
Beam
Pulse
Spectrum
Resonator
Gain Medium
Pump
Optical System
Propagation
Detector
Analysis
Result
```

unterscheiden.

---

## 343. Beam-Grundsatz

Ein Laserstrahl ist nicht vollständig durch:

```text
Origin
Direction
```

beschrieben.

Für physikalisch relevante Simulationen können zusätzlich erforderlich sein:

```text
Wavelength
Beam Profile
Beam Radius
Divergence
M2
Power
Phase
Polarization
Spectrum
Temporal Structure
```

---

## 344. Modellgrundsatz

Laser Compute MUSS zwischen unterschiedlichen physikalischen Abstraktionsebenen unterscheiden.

```text
Geometric Ray
Paraxial Beam
Gaussian Beam
Scalar Wave
Vector Wave
```

DÜRFEN nicht ohne Kennzeichnung als äquivalente Modelle behandelt werden.

---

## 345. Kohärenzgrundsatz

Bei kohärenten Simulationen ist Phase Bestandteil des physikalischen Zustands.

Daher gilt:

```text
Intensity Only
!=
Coherent Optical Field
```

---

## 346. Energiegrundsatz

Leistung, Energie, Intensität, Irradiance und Fluence MÜSSEN als unterschiedliche physikalische Größen behandelt werden.

---

## 347. Pulsgrundsatz

Bei gepulsten Lasern MÜSSEN mindestens:

```text
Pulse Energy
Pulse Duration
Repetition Rate
Peak Power
Average Power
```

voneinander unterschieden werden.

---

## 348. Resonatorgrundsatz

Ein Laserresonator darf nicht allein anhand geometrischer Strahlpfade bewertet werden.

Für eine vollständige Analyse können zusätzlich erforderlich sein:

```text
Gain
Loss
Mode
Phase
Stability
Output Coupling
Spectral Behavior
```

---

## 349. Sampling-Grundsatz

Wellenoptische Ergebnisse sind nur zusammen mit:

```text
Grid Size
Sample Spacing
Window Size
Wavelength
Propagation Distance
Propagation Method
```

numerisch interpretierbar.

---

## 350. Provenance-Grundsatz

Ein Laser-Simulationsergebnis MUSS auf die exakte Version des Lasersystems, der optischen Komponenten, Materialien, Beam-Definition, Propagationsmethode und numerischen Parameter zurückführbar sein.

---

## 351. Erweiterbarkeit

Neue:

```text
Laser Types
Beam Models
Pulse Models
Spectrum Models
Gain Models
Pump Models
Resonator Types
Propagation Algorithms
Nonlinear Processes
Polarization Models
Detector Models
Material Interaction Models
Optimization Algorithms
Compute Backends
```

SOLLTEN ohne Änderung des grundlegenden Laser-System-Objektmodells integrierbar sein.

---

## 352. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-LASER-SOURCE-0001
Laser Source Model

NPSPEC-COMPUTE-LASER-BEAM-0001
Laser Beam Model

NPSPEC-COMPUTE-LASER-GAUSSIAN-0001
Gaussian Beam Compute

NPSPEC-COMPUTE-LASER-MODE-0001
Laser Mode Model

NPSPEC-COMPUTE-LASER-RESONATOR-0001
Laser Resonator Compute

NPSPEC-COMPUTE-LASER-GAIN-0001
Laser Gain Media

NPSPEC-COMPUTE-LASER-PUMP-0001
Laser Pump Model

NPSPEC-COMPUTE-LASER-RATE-0001
Laser Rate Equations

NPSPEC-COMPUTE-LASER-PULSE-0001
Laser Pulse Compute

NPSPEC-COMPUTE-LASER-SPECTRUM-0001
Laser Spectrum Compute

NPSPEC-COMPUTE-LASER-COHERENCE-0001
Laser Coherence

NPSPEC-COMPUTE-LASER-POLARIZATION-0001
Laser Polarization

NPSPEC-COMPUTE-LASER-PROPAGATION-0001
Laser Beam Propagation

NPSPEC-COMPUTE-LASER-WAVE-0001
Wave Optical Propagation

NPSPEC-COMPUTE-LASER-DIFFRACTION-0001
Laser Diffraction

NPSPEC-COMPUTE-LASER-INTERFERENCE-0001
Laser Interference

NPSPEC-COMPUTE-LASER-NONLINEAR-0001
Nonlinear Laser Optics

NPSPEC-COMPUTE-LASER-THERMAL-0001
Laser Thermal Effects

NPSPEC-COMPUTE-LASER-MATERIAL-0001
Laser Material Interaction

NPSPEC-COMPUTE-LASER-DETECTOR-0001
Laser Detector Model

NPSPEC-COMPUTE-LASER-BEAMANALYSIS-0001
Laser Beam Analysis

NPSPEC-COMPUTE-LASER-OPTIMIZE-0001
Laser System Optimization

NPSPEC-COMPUTE-LASER-PROVENANCE-0001
Laser Compute Provenance

NPSPEC-COMPUTE-LASER-TEST-0001
Laser Compute Conformance Tests
```

---

## 353. Zusammenfassung

`NPSPEC-COMPUTE-LASER-0001` definiert die gemeinsame Infrastruktur zur Modellierung, Simulation, Analyse und Optimierung von Lasersystemen innerhalb der NovaOS-Compute-Architektur.

Die grundlegende Verarbeitung lautet:

```text
Laser System
     |
     v
Source
     |
     v
Beam / Pulse
     |
     v
Propagation
     |
     v
Optical System
     |
     v
Interaction
     |
     v
Detector
     |
     v
Analysis
```

Die Architektur unterstützt:

```text
Continuous-Wave Lasers
Pulsed Lasers
Gaussian Beams
Beam Waist
Rayleigh Range
Divergence
M2
ABCD Propagation
Laser Resonators
Laser Modes
Gain Media
Pump Sources
Spectra
Coherence
Polarization
Interference
Wave Propagation
Diffraction
Ultrashort Pulses
Dispersion
Nonlinear Optics
Thermal Effects
Laser Material Interaction
Beam Analysis
Optimization
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
Laser Source
Beam
Pulse
Spectrum
Resonator
Gain
Propagation
Optical Interaction
Detection
Analysis
Result
```

voneinander.

Die zentrale Strahlregel lautet:

> Ein Laserstrahl wird innerhalb von NovaOS nicht auf einen geometrischen Ray reduziert. Je nach gewähltem Modell umfasst sein Zustand zusätzlich räumliches Profil, Wellenlänge, Phase, Leistung, Polarisation, Divergenz, Beam Quality und zeitliche Eigenschaften.

Die zentrale Modellregel lautet:

> Geometrische Optik, Gaussian-Beam-Optik, skalare Wellenoptik und vektorielle Feldmodelle sind unterschiedliche Abstraktionsebenen. NovaOS muss die verwendete Ebene explizit kennzeichnen und darf Ergebnisse verschiedener Modelle nicht ohne definierte Transformation als physikalisch gleichwertig behandeln.

Die zentrale Energie-Regel lautet:

> Leistung, Pulsenergie, Spitzenleistung, mittlere Leistung, Intensität, Irradiance und Fluence sind eigenständige physikalische Größen und müssen mit expliziten Einheiten und klar definierter räumlicher beziehungsweise zeitlicher Bezugsgröße verarbeitet werden.

Die zentrale Kohärenzregel lautet:

> Sobald kohärente optische Effekte simuliert werden, ist die Phase Teil des physikalischen Zustands und muss durch die relevanten Compute-Stufen erhalten bleiben.

Das grundlegende Architekturprinzip lautet:

> NovaOS Laser Compute behandelt Laser als versionierte physikalische Compute-Systeme aus Quelle, Beam, Puls, Spektrum, Resonator, Gain Medium, Pumpquelle, optischen Komponenten, Propagationsmodellen und Detektoren. Dadurch kann dieselbe Infrastruktur von einfacher Gaussian-Beam-Berechnung über Resonator- und Objektivsimulation bis zu kohärenter Wellenpropagation, ultrakurzen Pulsen, nichtlinearer Optik, thermischer Kopplung und komplexer Laser-Systemoptimierung skaliert werden.