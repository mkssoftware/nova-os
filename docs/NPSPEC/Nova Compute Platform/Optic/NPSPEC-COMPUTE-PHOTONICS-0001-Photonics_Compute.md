# NPSPEC-COMPUTE-PHOTONICS-0001 — Photonics Compute

**Status:** Draft  
**Version:** 0.1  
**Kategorie:** Compute / Photonics / Scientific Simulation  
**Komponente:** NovaOS Photonics Compute  
**Bezeichner:** `NPSPEC-COMPUTE-PHOTONICS-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0001`, `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-RAYTRACING-0001`, `NPSPEC-COMPUTE-LENS-0001`, `NPSPEC-COMPUTE-LASER-0001`, `NPSPEC-COMPUTE-SPECTROSCOPY-0001`  
**Kurzname:** Photonics Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Compute-Infrastruktur für die Modellierung, Simulation, Analyse und Optimierung photonischer Systeme innerhalb der NovaOS-Compute-Architektur.

Photonics Compute stellt eine einheitliche Grundlage für elektromagnetische und optische Berechnungen bereit und verbindet geometrische Optik, Wellenoptik, elektromagnetische Feldsimulation, photonische Bauelemente, integrierte Photonik, Laser, Fasern, Resonatoren, Wellenleiter, Beugung, Interferenz, Polarisation und nichtlineare optische Prozesse.

Die Komponente SOLL insbesondere folgende Anwendungsgebiete unterstützen können:

- geometrische Optik,
- Wellenoptik,
- elektromagnetische Optik,
- integrierte Photonik,
- Siliziumphotonik,
- photonische integrierte Schaltungen,
- optische Wellenleiter,
- Glasfasern,
- Resonatoren,
- Kavitäten,
- Interferometer,
- Beugung,
- Interferenz,
- Polarisation,
- Dispersion,
- Streuung,
- Absorption,
- Reflexion,
- Transmission,
- photonische Kristalle,
- Metamaterialien,
- Plasmonik,
- Nanophotonik,
- Quantenphotonik als erweiterbares Modell,
- nichtlineare Optik,
- optische Kommunikation,
- Sensorik,
- Laser- und Spektroskopiesysteme.

Die grundlegende Architektur lautet:

```text
Photonics Model
      |
      v
Geometry + Materials
      |
      v
Sources + Boundary Conditions
      |
      v
Solver
      |
      +----------------------+
      |                      |
      v                      v
Field Solution        Optical Paths / Modes
      |                      |
      +----------+-----------+
                 |
                 v
          Photonics Result
                 |
                 v
        Analysis / Optimization
```

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Photonics Compute trennt physikalisches Modell, Geometrie, Materialeigenschaften, Quellen, Randbedingungen, numerisches Verfahren und Ergebnis voneinander, sodass dieselbe photonische Struktur mit unterschiedlichen physikalischen und numerischen Modellen untersucht werden kann.

---

## 3. Modellhierarchie

Photonics Compute MUSS mehrere Modellierungsstufen unterstützen können.

Mindestens:

```text
Geometrical Optics
Scalar Wave Optics
Vector Wave Optics
Full Electromagnetic Field
Reduced-Order Photonics
```

---

## 4. Model Selection

Die verwendete physikalische Modellstufe MUSS explizit dokumentiert werden.

---

## 5. No Silent Model Substitution

Die Runtime DARF nicht stillschweigend zwischen physikalisch unterschiedlichen Modellen wechseln.

---

## 6. Photonics Model

Ein photonisches Simulationsmodell wird als:

`PhotonicsModel`

repräsentiert.

---

## 7. Model Identity

Jedes Photonics Model MUSS eindeutig identifizierbar sein.

---

## 8. Model Version

Photonics Models SOLLEN versionierbar sein.

---

## 9. Model Components

Ein Photonics Model SOLL mindestens enthalten können:

```text
Geometry
Materials
Sources
Detectors
BoundaryConditions
SolverConfiguration
AnalysisConfiguration
```

---

## 10. Coordinate System

Photonische Modelle MÜSSEN ein explizites Koordinatensystem besitzen.

---

## 11. Coordinate Systems

Mindestens folgende Systeme SOLLEN unterstützt werden:

```text
Cartesian
Cylindrical
Spherical
Local
Custom
```

---

## 12. Dimensionality

Photonics Compute SOLL mindestens folgende Modellräume unterstützen:

```text
1D
2D
2.5D
3D
```

---

## 13. Units

Physikalische Größen MÜSSEN explizite Einheiten besitzen.

---

## 14. SI Units

SI-Einheiten SOLLEN intern bevorzugt unterstützt werden.

---

## 15. Unit Conversion

Kompatible Einheiten SOLLEN automatisch und kontrolliert transformiert werden können.

---

## 16. Geometry

Die Geometrie eines photonischen Systems wird als:

`PhotonicsGeometry`

repräsentiert.

---

## 17. Geometry Objects

Mindestens folgende Geometrieobjekte SOLLEN unterstützt werden können:

```text
Point
Line
Plane
Box
Sphere
Cylinder
Cone
Surface
Volume
Waveguide
Fiber
Lens
Mirror
Grating
Resonator
CustomGeometry
```

---

## 18. Geometry Composition

Komplexe photonische Strukturen SOLLEN aus mehreren Geometrieobjekten zusammengesetzt werden können.

---

## 19. Boolean Geometry

Geometrische Operationen SOLLEN unterstützen:

```text
Union
Intersection
Difference
```

---

## 20. Parametric Geometry

Geometrien SOLLEN parametrisiert werden können.

---

## 21. Geometry Parameter

Beispiele:

```text
Width
Height
Length
Radius
Thickness
Pitch
Period
Angle
Curvature
Gap
```

---

## 22. Geometry Precision

Die geometrische Präzision MUSS ausreichend sein, um Strukturen deutlich unterhalb der simulierten Wellenlänge repräsentieren zu können.

---

## 23. Multi-Scale Geometry

Photonics Compute SOLL Geometrien mit stark unterschiedlichen Größenskalen unterstützen können.

---

## 24. Material

Ein photonisches Material wird als:

`PhotonicsMaterial`

repräsentiert.

---

## 25. Material Identity

Jedes Material MUSS eindeutig identifizierbar sein.

---

## 26. Material Version

Materialmodelle SOLLEN versionierbar sein.

---

## 27. Material Properties

Ein Material SOLL mindestens enthalten können:

```text
RefractiveIndex
ExtinctionCoefficient
Permittivity
Permeability
Conductivity
Dispersion
Absorption
NonlinearProperties
TemperatureDependence
```

---

## 28. Refractive Index

Der Brechungsindex wird als:

`n`

repräsentiert.

---

## 29. Complex Refractive Index

Absorbierende Medien SOLLEN einen komplexen Brechungsindex unterstützen:

```text
n_complex = n + i*k
```

---

## 30. Extinction Coefficient

Der Extinktionskoeffizient wird als:

`k`

repräsentiert.

---

## 31. Relative Permittivity

Die relative Permittivität wird als:

```text
epsilon_r
```

repräsentiert.

---

## 32. Relative Permeability

Die relative Permeabilität wird als:

```text
mu_r
```

repräsentiert.

---

## 33. Complex Permittivity

Komplexe Permittivität MUSS unterstützt werden können.

---

## 34. Anisotropic Material

Anisotrope Materialien SOLLEN Tensorwerte für Materialeigenschaften unterstützen.

---

## 35. Birefringence

Doppelbrechende Materialien SOLLEN modellierbar sein.

---

## 36. Dispersion

Wellenlängen- beziehungsweise frequenzabhängige Materialeigenschaften MÜSSEN unterstützt werden können.

---

## 37. Dispersion Model

Ein Dispersionsmodell wird als:

`DispersionModel`

repräsentiert.

---

## 38. Dispersion Models

Mindestens folgende Modelle SOLLEN integrierbar sein:

```text
Constant
Tabulated
Sellmeier
Cauchy
Drude
Lorentz
DrudeLorentz
Polynomial
Custom
```

---

## 39. Sellmeier Model

Sellmeier-Modelle SOLLEN unterstützt werden können.

Eine typische Form lautet:

```text
n^2(lambda) =
1 +
sum(
    Bi * lambda^2 /
    (lambda^2 - Ci)
)
```

---

## 40. Material Validity Range

Materialmodelle MÜSSEN ihren gültigen Wellenlängen-, Frequenz- oder Temperaturbereich angeben können.

---

## 41. Out-of-Range Material Data

Materialeigenschaften außerhalb ihres validierten Bereichs DÜRFEN nicht stillschweigend extrapoliert werden.

---

## 42. Material Database

Photonics Compute SOLL eine:

`PhotonicsMaterialLibrary`

integrieren können.

---

## 43. Material Provenance

Materialdaten SOLLEN ihre Quelle und Version dokumentieren.

---

## 44. Electromagnetic Field

Elektromagnetische Felder werden als:

`ElectromagneticField`

repräsentiert.

---

## 45. Electric Field

Das elektrische Feld wird als:

```text
E(x,y,z,t)
```

repräsentiert.

---

## 46. Magnetic Field

Das magnetische Feld wird als:

```text
H(x,y,z,t)
```

beziehungsweise:

```text
B(x,y,z,t)
```

repräsentiert.

---

## 47. Complex Field

Frequenzbereichssimulationen MÜSSEN komplexwertige Feldgrößen unterstützen.

---

## 48. Field Components

Kartesische Felder SOLLEN Komponenten enthalten:

```text
Ex
Ey
Ez
Hx
Hy
Hz
```

---

## 49. Maxwell Equations

Full-Wave-Solver SOLLEN auf Maxwell-Gleichungen basieren können.

Grundlegend:

```text
div(D) = rho

div(B) = 0

curl(E) = -dB/dt

curl(H) = J + dD/dt
```

---

## 50. Constitutive Relations

Materialbeziehungen SOLLEN darstellen können:

```text
D = epsilon * E

B = mu * H

J = sigma * E
```

---

## 51. Harmonic Fields

Zeit-harmonische Felder SOLLEN repräsentiert werden als:

```text
E(r,t) = Re{E(r) * exp(-i*omega*t)}
```

---

## 52. Wavelength

Die Vakuumwellenlänge wird als:

```text
lambda0
```

repräsentiert.

---

## 53. Frequency

Die Frequenz wird als:

```text
f
```

repräsentiert.

---

## 54. Angular Frequency

Die Kreisfrequenz lautet:

```text
omega = 2*pi*f
```

---

## 55. Vacuum Relation

Im Vakuum gilt:

```text
c = lambda0 * f
```

---

## 56. Wavenumber

Die Vakuumwellenzahl lautet:

```text
k0 = 2*pi/lambda0
```

---

## 57. Medium Wavenumber

Für ein homogenes isotropes Medium kann gelten:

```text
k = n * k0
```

---

## 58. Phase Velocity

Die Phasengeschwindigkeit SOLL berechnet werden können.

---

## 59. Group Velocity

Die Gruppengeschwindigkeit SOLL berechnet werden können.

---

## 60. Group Index

Der Gruppenindex SOLL unterstützt werden.

---

## 61. Group Velocity Dispersion

Group Velocity Dispersion SOLL berechnet werden können.

---

## 62. Source

Eine photonische Quelle wird als:

`PhotonicsSource`

repräsentiert.

---

## 63. Source Types

Mindestens folgende Quellen SOLLEN unterstützt werden können:

```text
PlaneWave
GaussianBeam
PointSource
DipoleSource
LineSource
ModeSource
LaserSource
PulseSource
BroadbandSource
Custom
```

---

## 64. Source Frequency

Eine Quelle MUSS Frequenz beziehungsweise Wellenlänge definieren können.

---

## 65. Broadband Source

Breitbandige Quellen SOLLEN einen Frequenz- beziehungsweise Wellenlängenbereich besitzen können.

---

## 66. Source Amplitude

Die Quellamplitude MUSS explizit definiert werden.

---

## 67. Source Phase

Die Quellphase SOLL definiert werden können.

---

## 68. Source Polarization

Die Polarisation einer Quelle MUSS spezifizierbar sein.

---

## 69. Source Direction

Die Ausbreitungsrichtung MUSS definierbar sein.

---

## 70. Gaussian Beam

Gaussian Beams SOLLEN unterstützt werden.

---

## 71. Gaussian Beam Parameters

Mindestens:

```text
Waist
WaistPosition
Wavelength
Polarization
Power
Direction
Phase
```

SOLLTEN unterstützt werden.

---

## 72. Beam Waist

Der Beam Waist wird als:

```text
w0
```

repräsentiert.

---

## 73. Rayleigh Range

Für einen idealen Gaussian Beam kann gelten:

```text
zR = pi*w0^2/lambda
```

unter geeigneter Definition von `lambda`.

---

## 74. Beam Divergence

Die Divergenz SOLL berechnet werden können.

---

## 75. Laser Integration

Laserquellen SOLLEN `NPSPEC-COMPUTE-LASER-0001` integrieren können.

---

## 76. Detector

Ein photonischer Detektor wird als:

`PhotonicsDetector`

repräsentiert.

---

## 77. Detector Types

Mindestens:

```text
PointDetector
LineDetector
PlaneDetector
VolumeDetector
PowerMonitor
FieldMonitor
ModeMonitor
SpectrumMonitor
FluxMonitor
FarFieldMonitor
Custom
```

SOLLTEN unterstützt werden.

---

## 78. Detector Sampling

Sampling-Auflösung MUSS explizit konfigurierbar sein.

---

## 79. Detector Quantity

Detektoren SOLLEN Größen messen können wie:

```text
ElectricField
MagneticField
Intensity
Power
Flux
Phase
Polarization
Spectrum
ModeAmplitude
```

---

## 80. Boundary Condition

Randbedingungen werden als:

`PhotonicsBoundaryCondition`

repräsentiert.

---

## 81. Boundary Types

Mindestens folgende Randbedingungen SOLLEN unterstützt werden können:

```text
PEC
PMC
Periodic
BlochPeriodic
Symmetry
AntiSymmetry
Absorbing
PML
Open
Port
Custom
```

---

## 82. Perfectly Matched Layer

PML-Randbedingungen SOLLEN unterstützt werden.

---

## 83. PML Parameters

Mindestens folgende Parameter SOLLEN konfigurierbar sein:

```text
Thickness
Order
AbsorptionProfile
ReflectionTarget
```

---

## 84. Periodic Boundary

Periodische Randbedingungen SOLLEN unterstützt werden.

---

## 85. Bloch Boundary

Bloch-periodische Randbedingungen SOLLEN für periodische photonische Strukturen unterstützt werden.

---

## 86. Symmetry

Symmetriebedingungen SOLLEN zur Reduzierung des Simulationsraums verwendet werden können.

---

## 87. Solver

Ein photonischer Solver wird als:

`PhotonicsSolver`

repräsentiert.

---

## 88. Solver Types

Photonics Compute SOLL mindestens folgende Solverklassen integrieren können:

```text
RayTracing
BeamPropagationMethod
FDTD
FEM
FrequencyDomain
Eigenmode
TransferMatrix
RCWA
BoundaryElement
CoupledModeTheory
Paraxial
Custom
```

---

## 89. Solver Selection

Die Solverwahl MUSS explizit dokumentiert werden.

---

## 90. FDTD

Finite-Difference Time-Domain SOLL als Full-Wave-Verfahren unterstützt werden können.

---

## 91. FDTD Grid

FDTD verwendet ein räumliches Diskretisierungsgitter.

---

## 92. Yee Grid

Ein Yee-Grid SOLL unterstützt werden können.

---

## 93. Spatial Resolution

Die räumliche Auflösung MUSS in Bezug auf:

```text
Wavelength
MaterialIndex
Geometry
FieldVariation
```

ausreichend gewählt werden.

---

## 94. Courant Stability

Explizite Zeitbereichssolver MÜSSEN ihre Stabilitätsbedingungen prüfen.

---

## 95. Courant Condition

Für FDTD MUSS die Zeitschrittgröße eine geeignete Courant-Bedingung erfüllen.

---

## 96. Unstable Time Step

Ein instabiler Zeitschritt MUSS abgelehnt oder explizit als ungültig markiert werden.

---

## 97. FDTD Time Step

Der verwendete Zeitschritt MUSS dokumentiert werden.

---

## 98. FDTD Termination

Simulationen SOLLEN über Kriterien beendet werden können wie:

```text
MaximumTime
FieldDecay
EnergyDecay
Convergence
ManualCancellation
```

---

## 99. FEM

Finite-Element-Methoden SOLLEN für elektromagnetische Feldprobleme unterstützt werden können.

---

## 100. FEM Mesh

FEM-Simulationen SOLLEN unstrukturierte Netze verwenden können.

---

## 101. Mesh Integration

Photonics Compute SOLL `NPSPEC-COMPUTE-MESH-0001` integrieren können.

---

## 102. Adaptive Mesh

Adaptive Netzverfeinerung SOLL unterstützt werden können.

---

## 103. Mesh Refinement Criteria

Verfeinerungskriterien können umfassen:

```text
Geometry
MaterialBoundary
FieldGradient
ErrorEstimate
Wavelength
```

---

## 104. Frequency-Domain Solver

Frequenzbereichssolver SOLLEN unterstützt werden.

---

## 105. Frequency Sweep

Ein Solver SOLL mehrere Frequenzen beziehungsweise Wellenlängen berechnen können.

---

## 106. Sweep Configuration

Ein Sweep wird als:

`PhotonicsSweep`

repräsentiert.

---

## 107. Sweep Types

Mindestens:

```text
Linear
Logarithmic
ExplicitList
Adaptive
```

SOLLTEN unterstützt werden.

---

## 108. Eigenmode Solver

Photonics Compute SOLL Eigenmoden photonischer Strukturen berechnen können.

---

## 109. Mode

Ein optischer Modus wird als:

`PhotonicsMode`

repräsentiert.

---

## 110. Mode Properties

Ein Mode SOLL mindestens enthalten können:

```text
EffectiveIndex
PropagationConstant
FieldDistribution
Polarization
Loss
GroupIndex
Confinement
```

---

## 111. Effective Index

Der effektive Brechungsindex wird als:

```text
n_eff
```

repräsentiert.

---

## 112. Propagation Constant

Die Propagationskonstante wird als:

```text
beta
```

repräsentiert.

---

## 113. Mode Relation

Für geeignete Wellenleiter gilt:

```text
beta = n_eff * k0
```

---

## 114. Complex Effective Index

Verlustbehaftete Modi SOLLEN komplexe effektive Brechungsindizes unterstützen.

---

## 115. Mode Loss

Aus dem imaginären Anteil eines Modus SOLL die Propagationsdämpfung ableitbar sein.

---

## 116. Mode Ordering

Moden SOLLEN deterministisch sortierbar sein.

---

## 117. Mode Tracking

Moden SOLLEN über Parameter- oder Frequenzsweeps hinweg verfolgt werden können.

---

## 118. Mode Crossing

Mode Crossings und Anti-Crossings SOLLEN erkennbar sein.

---

## 119. Mode Overlap

Der Überlapp zwischen zwei Moden SOLL berechnet werden können.

---

## 120. Mode Coupling

Kopplung zwischen Moden SOLL modellierbar sein.

---

## 121. Coupled Mode Theory

Coupled Mode Theory SOLL als Reduced-Order-Modell unterstützt werden können.

---

## 122. Waveguide

Ein optischer Wellenleiter wird als:

`PhotonicsWaveguide`

repräsentiert.

---

## 123. Waveguide Types

Mindestens:

```text
Slab
Strip
Rib
Slot
Fiber
PhotonicCrystal
Plasmonic
Custom
```

SOLLTEN unterstützt werden.

---

## 124. Waveguide Core

Core-Material und Geometrie MÜSSEN definierbar sein.

---

## 125. Waveguide Cladding

Cladding-Materialien MÜSSEN definierbar sein.

---

## 126. Waveguide Loss

Verluste SOLLEN modelliert werden können.

---

## 127. Bend Loss

Biegeverluste SOLLEN berechnet werden können.

---

## 128. Coupling Loss

Kopplungsverluste SOLLEN berechnet werden können.

---

## 129. Propagation Loss

Propagationsverluste SOLLEN berechnet werden können.

---

## 130. Optical Fiber

Optische Fasern werden als:

`PhotonicsFiber`

repräsentiert.

---

## 131. Fiber Types

Mindestens:

```text
SingleMode
MultiMode
StepIndex
GradedIndex
PhotonicCrystalFiber
Custom
```

SOLLTEN unterstützt werden.

---

## 132. Numerical Aperture

Numerische Apertur SOLL berechnet werden können.

---

## 133. V Number

Für geeignete Fasermodelle SOLL die normierte Frequenz berechnet werden können.

---

## 134. Fiber Modes

Fasermoden SOLLEN über den Eigenmode Solver bestimmt werden können.

---

## 135. Fiber Dispersion

Chromatische und Wellenleiterdispersion SOLLEN modellierbar sein.

---

## 136. Polarization Mode Dispersion

Polarization Mode Dispersion SOLL architektonisch unterstützt werden.

---

## 137. Resonator

Ein photonischer Resonator wird als:

`PhotonicsResonator`

repräsentiert.

---

## 138. Resonator Types

Mindestens:

```text
FabryPerot
Ring
Disk
Sphere
PhotonicCrystalCavity
Microcavity
Custom
```

SOLLTEN unterstützt werden.

---

## 139. Resonance Frequency

Resonanzfrequenzen SOLLEN bestimmt werden können.

---

## 140. Resonance Wavelength

Resonanzwellenlängen SOLLEN bestimmt werden können.

---

## 141. Q Factor

Der Qualitätsfaktor wird als:

`Q`

repräsentiert.

---

## 142. Q Definition

Für geeignete Resonanzen kann gelten:

```text
Q = f0 / delta_f
```

beziehungsweise:

```text
Q = lambda0 / delta_lambda
```

unter geeigneten Voraussetzungen.

---

## 143. Mode Volume

Das effektive Mode Volume SOLL berechnet werden können.

---

## 144. Free Spectral Range

Die Free Spectral Range SOLL berechnet werden können.

---

## 145. Cavity Lifetime

Photonen- beziehungsweise Feldlebensdauer einer Kavität SOLL ableitbar sein.

---

## 146. Interference

Photonics Compute MUSS Interferenz modellieren können.

---

## 147. Coherent Superposition

Kohärente Felder MÜSSEN komplexwertig überlagert werden können.

---

## 148. Incoherent Combination

Inkohärente Quellen MÜSSEN von kohärenter Feldüberlagerung unterschieden werden.

---

## 149. Phase

Phaseninformation MUSS bei kohärenten Simulationen erhalten bleiben.

---

## 150. Optical Path Length

Die optische Weglänge wird als:

```text
OPL = integral(n ds)
```

repräsentiert.

---

## 151. Phase Difference

Phasendifferenzen SOLLEN aus optischen Weglängen berechnet werden können.

---

## 152. Interferometer

Interferometer SOLLEN als photonische Systeme modellierbar sein.

---

## 153. Interferometer Types

Beispiele:

```text
Michelson
MachZehnder
FabryPerot
Sagnac
Young
Custom
```

---

## 154. Diffraction

Photonics Compute SOLL Beugung unterstützen.

---

## 155. Diffraction Models

Mindestens:

```text
Fraunhofer
Fresnel
AngularSpectrum
FullWave
```

SOLLTEN unterstützt werden können.

---

## 156. Fraunhofer Diffraction

Fernfeldbeugung SOLL unterstützt werden.

---

## 157. Fresnel Diffraction

Nahfeldbeugung SOLL unterstützt werden.

---

## 158. Angular Spectrum

Angular-Spectrum-Propagation SOLL unterstützt werden können.

---

## 159. Grating

Ein optisches Gitter wird als:

`PhotonicsGrating`

repräsentiert.

---

## 160. Grating Parameters

Mindestens:

```text
Period
DutyCycle
Depth
Material
Orientation
```

SOLLTEN unterstützt werden.

---

## 161. Diffraction Orders

Beugungsordnungen SOLLEN bestimmt werden können.

---

## 162. Grating Efficiency

Effizienz einzelner Beugungsordnungen SOLL berechnet werden können.

---

## 163. RCWA

Rigorous Coupled-Wave Analysis SOLL für periodische Strukturen integrierbar sein.

---

## 164. Polarization

Photonics Compute MUSS Polarisation unterstützen.

---

## 165. Polarization Representations

Mindestens:

```text
JonesVector
JonesMatrix
StokesVector
MuellerMatrix
FieldComponents
```

SOLLTEN unterstützt werden können.

---

## 166. Linear Polarization

Lineare Polarisation MUSS repräsentierbar sein.

---

## 167. Circular Polarization

Zirkulare Polarisation MUSS repräsentierbar sein.

---

## 168. Elliptical Polarization

Elliptische Polarisation MUSS repräsentierbar sein.

---

## 169. Unpolarized Light

Unpolarisiertes Licht MUSS von kohärenten Jones-Feldern unterscheidbar sein.

---

## 170. Jones Calculus

Jones-Matrix-Verarbeitung SOLL unterstützt werden.

---

## 171. Mueller Calculus

Mueller-Matrix-Verarbeitung SOLL unterstützt werden.

---

## 172. Stokes Parameters

Stokes-Parameter:

```text
S0
S1
S2
S3
```

SOLLTEN unterstützt werden.

---

## 173. Degree of Polarization

Der Polarisationsgrad SOLL berechnet werden können.

---

## 174. Reflection

Reflexion an Grenzflächen SOLL modelliert werden.

---

## 175. Transmission

Transmission an Grenzflächen SOLL modelliert werden.

---

## 176. Fresnel Equations

Fresnel-Koeffizienten SOLLEN für geeignete Grenzflächen berechnet werden können.

---

## 177. TE Polarization

TE beziehungsweise s-polarisierte Komponenten SOLLEN unterstützt werden.

---

## 178. TM Polarization

TM beziehungsweise p-polarisierte Komponenten SOLLEN unterstützt werden.

---

## 179. Brewster Angle

Der Brewster-Winkel SOLL für geeignete Materialien berechnet werden können.

---

## 180. Total Internal Reflection

Totalreflexion MUSS modellierbar sein.

---

## 181. Evanescent Field

Evaneszente Felder MÜSSEN repräsentierbar sein.

---

## 182. Multilayer Optics

Mehrschichtige optische Systeme SOLLEN unterstützt werden.

---

## 183. Layer

Eine Schicht wird als:

`PhotonicsLayer`

repräsentiert.

---

## 184. Layer Properties

Mindestens:

```text
Thickness
Material
Roughness
Orientation
```

SOLLTEN unterstützt werden.

---

## 185. Transfer Matrix Method

Transfer-Matrix-Verfahren SOLLEN für geschichtete Medien unterstützt werden.

---

## 186. Scattering Matrix

Numerisch stabilere Scattering-Matrix-Verfahren SOLLEN integrierbar sein.

---

## 187. Thin Films

Dünnschichtsysteme SOLLEN analysiert werden können.

---

## 188. Coatings

Optische Beschichtungen SOLLEN modelliert und optimiert werden können.

---

## 189. Anti-Reflection Coating

Antireflexbeschichtungen SOLLEN analysiert werden können.

---

## 190. High-Reflection Coating

Hochreflektierende Beschichtungen SOLLEN analysiert werden können.

---

## 191. Photonic Crystal

Photonische Kristalle werden als:

`PhotonicCrystal`

repräsentiert.

---

## 192. Lattice

Ein periodisches Gitter wird als:

`PhotonicsLattice`

repräsentiert.

---

## 193. Lattice Types

Mindestens:

```text
1D
Square
Rectangular
Triangular
Hexagonal
Custom
```

SOLLTEN unterstützt werden.

---

## 194. Unit Cell

Eine periodische Struktur MUSS eine Unit Cell definieren können.

---

## 195. Band Structure

Photonische Bandstrukturen SOLLEN berechnet werden können.

---

## 196. Band Gap

Photonische Bandlücken SOLLEN bestimmt werden können.

---

## 197. Bloch Mode

Bloch-Moden SOLLEN repräsentiert werden können.

---

## 198. Brillouin Zone

Berechnungen im reziproken Raum SOLLEN Brillouin-Zonen unterstützen.

---

## 199. Nanophotonics

Strukturen im Nano- und Subwellenlängenbereich SOLLEN unterstützt werden.

---

## 200. Plasmonics

Photonics Compute SOLL plasmonische Systeme modellieren können.

---

## 201. Metal Dispersion

Metallische Materialien SOLLEN dispersive komplexe Permittivitäten unterstützen.

---

## 202. Surface Plasmon Polariton

Surface Plasmon Polaritons SOLLEN modellierbar sein.

---

## 203. Localized Plasmon Resonance

Lokalisierte plasmonische Resonanzen SOLLEN berechnet werden können.

---

## 204. Metamaterials

Photonische Metamaterialien SOLLEN unterstützt werden können.

---

## 205. Effective Material Parameters

Effektive:

```text
Permittivity
Permeability
RefractiveIndex
Impedance
```

SOLLTEN repräsentierbar sein.

---

## 206. Nonlinear Optics

Photonics Compute SOLL nichtlineare optische Effekte architektonisch unterstützen.

---

## 207. Nonlinear Susceptibility

Nichtlineare Suszeptibilitäten SOLLEN repräsentiert werden können:

```text
chi2
chi3
```

---

## 208. Kerr Effect

Der optische Kerr-Effekt SOLL modellierbar sein.

---

## 209. Nonlinear Refractive Index

Ein intensitätsabhängiger Brechungsindex kann dargestellt werden als:

```text
n = n0 + n2*I
```

---

## 210. Self-Phase Modulation

Self-Phase Modulation SOLL unterstützt werden können.

---

## 211. Cross-Phase Modulation

Cross-Phase Modulation SOLL unterstützt werden können.

---

## 212. Four-Wave Mixing

Four-Wave Mixing SOLL architektonisch unterstützt werden.

---

## 213. Second Harmonic Generation

Second Harmonic Generation SOLL unterstützt werden können.

---

## 214. Sum Frequency Generation

Sum Frequency Generation SOLL unterstützt werden können.

---

## 215. Difference Frequency Generation

Difference Frequency Generation SOLL unterstützt werden können.

---

## 216. Parametric Processes

Optisch parametrische Prozesse SOLLEN modellierbar sein.

---

## 217. Phase Matching

Phase-Matching-Bedingungen SOLLEN berechnet werden können.

---

## 218. Quasi-Phase Matching

Quasi-Phase Matching SOLL unterstützt werden können.

---

## 219. Pulse

Ein optischer Puls wird als:

`PhotonicsPulse`

repräsentiert.

---

## 220. Pulse Properties

Mindestens:

```text
CentralFrequency
CentralWavelength
PulseDuration
Energy
PeakPower
Envelope
Phase
Polarization
Chirp
```

SOLLTEN unterstützt werden.

---

## 221. Pulse Envelope

Mindestens folgende Pulsformen SOLLEN unterstützt werden können:

```text
Gaussian
SechSquared
Lorentzian
Custom
```

---

## 222. Chirp

Zeitliche beziehungsweise spektrale Chirp-Parameter SOLLEN repräsentiert werden können.

---

## 223. Pulse Propagation

Pulspropagation in dispersiven Medien SOLL unterstützt werden können.

---

## 224. Group Delay

Group Delay SOLL berechnet werden können.

---

## 225. Group Delay Dispersion

Group Delay Dispersion SOLL berechnet werden können.

---

## 226. Higher-Order Dispersion

Höhere Dispersionsordnungen SOLLEN architektonisch unterstützt werden.

---

## 227. Beam Propagation Method

BPM SOLL für geeignete Wellenleiter- und Propagationsprobleme unterstützt werden.

---

## 228. Paraxial Approximation

Paraxiale Näherungen MÜSSEN explizit als solche gekennzeichnet werden.

---

## 229. Approximation Validity

Solver mit physikalischen Näherungen SOLLEN ihre Gültigkeitsbedingungen dokumentieren können.

---

## 230. Ray Tracing Integration

Geometrisch-optische Berechnungen SOLLEN `NPSPEC-COMPUTE-RAYTRACING-0001` integrieren können.

---

## 231. Lens Integration

Linsen- und optische Systemmodelle SOLLEN `NPSPEC-COMPUTE-LENS-0001` integrieren können.

---

## 232. Spectroscopy Integration

Spektrale Quellen, Detektoren und Resultate SOLLEN `NPSPEC-COMPUTE-SPECTROSCOPY-0001` integrieren können.

---

## 233. Photonic Circuit

Ein photonisch integrierter Schaltkreis wird als:

`PhotonicCircuit`

repräsentiert.

---

## 234. Circuit Components

Mindestens folgende Komponenten SOLLEN repräsentierbar sein:

```text
Waveguide
Coupler
Splitter
Combiner
PhaseShifter
Modulator
RingResonator
Filter
GratingCoupler
Detector
Laser
Port
Custom
```

---

## 235. Optical Port

Ein optischer Anschluss wird als:

`PhotonicsPort`

repräsentiert.

---

## 236. Port Properties

Ein Port SOLL enthalten können:

```text
Position
Direction
Mode
FrequencyRange
Polarization
Impedance
```

---

## 237. Scattering Parameters

Photonische Netzwerke SOLLEN S-Parameter unterstützen.

---

## 238. S Matrix

Eine Scattering Matrix wird als:

`PhotonicsScatteringMatrix`

repräsentiert.

---

## 239. S Parameter Frequency Dependence

S-Parameter SOLLEN frequenzabhängig sein können.

---

## 240. Insertion Loss

Insertion Loss SOLL berechnet werden können.

---

## 241. Return Loss

Return Loss SOLL berechnet werden können.

---

## 242. Crosstalk

Optischer Crosstalk SOLL analysiert werden können.

---

## 243. Extinction Ratio

Extinction Ratio SOLL berechnet werden können.

---

## 244. Coupler

Optische Koppler SOLLEN modelliert werden können.

---

## 245. Directional Coupler

Directional Coupler SOLLEN unterstützt werden.

---

## 246. Coupling Coefficient

Kopplungskoeffizienten SOLLEN bestimmt werden können.

---

## 247. Power Splitter

Power Splitter SOLLEN modellierbar sein.

---

## 248. Modulator

Optische Modulatoren SOLLEN architektonisch unterstützt werden.

---

## 249. Modulator Types

Beispiele:

```text
ElectroOptic
ThermoOptic
CarrierInjection
CarrierDepletion
AcoustoOptic
Custom
```

---

## 250. Electro-Optic Effect

Elektrooptische Materialmodelle SOLLEN unterstützt werden können.

---

## 251. Thermo-Optic Effect

Temperaturabhängige Brechungsindexänderungen SOLLEN modellierbar sein.

---

## 252. Multi-Physics Integration

Photonics Compute SOLL mit anderen physikalischen Compute-Modulen gekoppelt werden können.

---

## 253. Thermal Coupling

Temperaturfelder SOLLEN Materialparameter beeinflussen können.

---

## 254. Mechanical Coupling

Mechanische Verformungen SOLLEN photonische Geometrie beziehungsweise Materialeigenschaften beeinflussen können.

---

## 255. Electrical Coupling

Elektrische Felder beziehungsweise Ladungsträger SOLLEN elektrooptische Komponenten beeinflussen können.

---

## 256. Coupled Simulation

Eine gekoppelte Simulation wird als:

`PhotonicsCoupledSimulation`

repräsentiert.

---

## 257. Coupling Direction

Kopplungen SOLLEN:

```text
OneWay
TwoWay
Iterative
```

unterstützen können.

---

## 258. Convergence

Iterativ gekoppelte Simulationen MÜSSEN Konvergenzkriterien besitzen.

---

## 259. Optical Power

Optische Leistung wird als:

`OpticalPower`

repräsentiert.

---

## 260. Intensity

Optische Intensität wird als:

`OpticalIntensity`

repräsentiert.

---

## 261. Poynting Vector

Der Poynting-Vektor SOLL berechnet werden können:

```text
S = E x H
```

mit korrekter zeitlicher beziehungsweise komplexer Interpretation.

---

## 262. Power Flux

Leistungsfluss durch Flächen SOLL integrierbar sein.

---

## 263. Energy Conservation

Photonics Compute SOLL Energieerhaltung beziehungsweise Leistungsbilanz prüfen können.

---

## 264. Power Balance

Eine typische Bilanz lautet:

```text
InputPower
=
ReflectedPower
+
TransmittedPower
+
AbsorbedPower
+
OtherLosses
```

innerhalb numerischer Toleranzen.

---

## 265. Conservation Diagnostic

Signifikante Verletzungen der Leistungsbilanz SOLLEN als Qualitätsdiagnose ausgegeben werden.

---

## 266. Absorption

Lokale und globale Absorption SOLL berechnet werden können.

---

## 267. Absorbed Power Density

Absorbierte Leistungsdichte SOLL als Feldgröße verfügbar sein können.

---

## 268. Scattering

Elektromagnetische Streuung SOLL unterstützt werden.

---

## 269. Scattering Cross Section

Streuquerschnitte SOLLEN berechnet werden können.

---

## 270. Absorption Cross Section

Absorptionsquerschnitte SOLLEN berechnet werden können.

---

## 271. Extinction Cross Section

Extinktionsquerschnitte SOLLEN berechnet werden können.

---

## 272. Near Field

Nahfelder SOLLEN direkt analysierbar sein.

---

## 273. Far Field

Fernfelder SOLLEN berechnet werden können.

---

## 274. Near-to-Far Transformation

Near-to-Far-Field-Transformation SOLL unterstützt werden können.

---

## 275. Radiation Pattern

Abstrahlcharakteristiken SOLLEN erzeugt werden können.

---

## 276. Directivity

Direktivität SOLL berechnet werden können.

---

## 277. Numerical Precision

Photonics Compute MUSS numerische Präzision explizit verwalten.

---

## 278. FP64

FP64 MUSS für präzise wissenschaftliche Berechnungen unterstützt werden.

---

## 279. FP32

FP32 DARF für geeignete hochparallele Simulationen verwendet werden.

---

## 280. Mixed Precision

Mixed Precision SOLL unterstützt werden können.

---

## 281. Complex Precision

Komplexwertige FP32- und FP64-Datentypen SOLLEN unterstützt werden.

---

## 282. Numerical Stability

Solver MÜSSEN numerische Instabilitäten erkennen können.

---

## 283. Convergence Criteria

Iterative Solver MÜSSEN explizite Konvergenzkriterien besitzen.

---

## 284. Residual

Solverresiduen SOLLEN messbar sein.

---

## 285. Solver Tolerance

Numerische Toleranzen MÜSSEN dokumentiert werden.

---

## 286. Maximum Iterations

Iterative Solver SOLLEN eine maximale Iterationszahl besitzen.

---

## 287. Non-Convergence

Nicht konvergierte Lösungen DÜRFEN nicht als vollständig gültige Resultate behandelt werden.

---

## 288. Scheduler Integration

Photonics Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 289. Photonics Tasks

Mindestens folgende Aufgaben SOLLEN planbar sein:

```text
GeometryPreparation
MeshGeneration
MaterialEvaluation
ModeSolve
FieldSolve
FrequencySweep
WavelengthSweep
ParameterSweep
Optimization
PostProcessing
FarFieldTransform
CircuitAnalysis
```

---

## 290. Parallel Frequencies

Unabhängige Frequenzpunkte SOLLEN parallel berechnet werden können.

---

## 291. Parallel Parameters

Unabhängige Parameterpunkte SOLLEN parallel berechnet werden können.

---

## 292. Parallel Modes

Geeignete Modensuchen SOLLEN parallelisiert werden können.

---

## 293. Cancellation

Photonics-Jobs MÜSSEN kontrolliert abbrechbar sein.

---

## 294. Partial Results

Bei Abbruch MÜSSEN vollständige und unvollständige Ergebnisse unterscheidbar sein.

---

## 295. Memory Model Integration

Photonics Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 296. Memory Objects

Mindestens folgende Speicherobjekte SOLLEN unterstützt werden:

```text
GeometryBuffer
MaterialBuffer
MeshBuffer
FieldBuffer
ComplexFieldBuffer
ModeBuffer
SolverWorkspace
MonitorBuffer
SweepBuffer
ResultBuffer
```

---

## 297. Large Field Data

Große elektromagnetische Feldvolumen SOLLEN speichereffizient verarbeitet werden.

---

## 298. Out-of-Core Processing

Simulationen mit Ergebnissen größer als der verfügbare RAM SOLLEN Out-of-Core-Verarbeitung unterstützen können.

---

## 299. Checkpointing

Lang laufende photonische Simulationen SOLLEN Checkpoints unterstützen.

---

## 300. Restart

Simulationen SOLLEN aus gültigen Checkpoints fortgesetzt werden können.

---

## 301. Object Model Integration

Photonics Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0004` integrieren.

---

## 302. Compute Objects

Mindestens:

```text
PhotonicsModel
PhotonicsGeometry
PhotonicsMaterial
PhotonicsSource
PhotonicsDetector
PhotonicsBoundaryCondition
PhotonicsSolver
PhotonicsMode
PhotonicsResult
```

SOLLTEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 303. GPU Runtime Integration

Photonics Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` integrieren.

---

## 304. GPU Suitable Operations

Geeignete GPU-Operationen umfassen:

```text
FDTD Updates
FFT
Field Operations
Matrix Operations
Mode Calculations
Parameter Sweeps
Far Field Transforms
Post Processing
```

---

## 305. CPU Backend

Grundlegende photonische Simulationen MÜSSEN auf CPU ausführbar sein.

---

## 306. SIMD

Feldoperationen SOLLEN SIMD verwenden können.

---

## 307. Multi-Core

Solver SOLLEN Multi-Core-Verarbeitung unterstützen können.

---

## 308. Multi-GPU

Große Simulationen SOLLEN Multi-GPU-Ausführung unterstützen können.

---

## 309. Domain Decomposition

Räumliche Domänen SOLLEN über mehrere Compute Devices verteilt werden können.

---

## 310. Halo Exchange

Domain-Decomposition-Solver SOLLEN Randdaten effizient austauschen können.

---

## 311. NPU Runtime Integration

`NPSPEC-COMPUTE-RUNTIME-0006` SOLL für geeignete AI-basierte photonische Aufgaben integrierbar sein.

---

## 312. AI Photonics Tasks

Mögliche AI-Aufgaben umfassen:

```text
SurrogateModel
InverseDesign
GeometryPrediction
MaterialOptimization
ModeClassification
FieldApproximation
AnomalyDetection
```

---

## 313. Distributed Compute

Photonics Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0007` integrieren können.

---

## 314. Distributed Tasks

Geeignete Aufgaben umfassen:

```text
LargeParameterSweeps
Optimization
MonteCarloAnalysis
FrequencySweeps
DesignSpaceExploration
LargeFieldSimulation
```

---

## 315. HPC Integration

Photonics Compute SOLL `NPSPEC-COMPUTE-HPC-0001` integrieren.

---

## 316. Optimization

Photonics Compute SOLL photonische Optimierungsprobleme unterstützen.

---

## 317. Optimization Problem

Ein Optimierungsproblem wird als:

`PhotonicsOptimization`

repräsentiert.

---

## 318. Design Variables

Designvariablen können umfassen:

```text
Geometry
Material
Thickness
Gap
Radius
Period
DutyCycle
WaveguideWidth
CouplingLength
SourceParameters
```

---

## 319. Objective Function

Zielfunktionen können umfassen:

```text
Transmission
Reflection
Absorption
QFactor
ModeOverlap
CouplingEfficiency
Bandwidth
InsertionLoss
FieldEnhancement
Power
Custom
```

---

## 320. Constraints

Optimierungen SOLLEN Constraints unterstützen.

---

## 321. Optimization Methods

Mindestens folgende Verfahren SOLLEN integrierbar sein:

```text
GradientDescent
QuasiNewton
Adjoint
GeneticAlgorithm
ParticleSwarm
BayesianOptimization
GridSearch
RandomSearch
Custom
```

---

## 322. Adjoint Optimization

Adjoint-basierte Optimierung SOLL für geeignete Solver unterstützt werden können.

---

## 323. Inverse Design

Photonisches Inverse Design SOLL architektonisch unterstützt werden.

---

## 324. Optimization Provenance

Optimierungsverlauf, Parameter und Solverversion MÜSSEN dokumentiert werden.

---

## 325. Monte Carlo

Statistische Toleranzanalysen SOLLEN unterstützt werden.

---

## 326. Manufacturing Tolerance

Fertigungsabweichungen SOLLEN parametrisch modelliert werden können.

---

## 327. Uncertainty Parameters

Beispiele:

```text
WidthVariation
ThicknessVariation
IndexVariation
TemperatureVariation
AlignmentError
Roughness
```

---

## 328. Statistical Result

Statistische Simulationsergebnisse SOLLEN enthalten können:

```text
Mean
Variance
StandardDeviation
Percentiles
Yield
ConfidenceInterval
```

---

## 329. Fabrication Yield

Design Yield SOLL berechnet werden können.

---

## 330. Surface Roughness

Oberflächenrauheit SOLL architektonisch modellierbar sein.

---

## 331. Disorder

Zufällige geometrische beziehungsweise Materialstörungen SOLLEN unterstützt werden können.

---

## 332. Random Seed

Stochastische Simulationen MÜSSEN explizite Seeds unterstützen.

---

## 333. Deterministic Mode

Photonics Compute SOLL einen deterministischen Modus unterstützen.

---

## 334. Reproducibility

Identische Eingaben, Solverversionen und Einstellungen SOLLEN reproduzierbare Ergebnisse liefern.

---

## 335. Photonics Result

Ein Simulationsergebnis wird als:

`PhotonicsResult`

repräsentiert.

---

## 336. Result Types

Mindestens:

```text
FieldSolution
ModeSolution
TransmissionSpectrum
ReflectionSpectrum
AbsorptionSpectrum
ScatteringResult
FarFieldPattern
ResonanceResult
CircuitResult
OptimizationResult
```

SOLLTEN unterstützt werden.

---

## 337. Result Identity

Jedes Ergebnis MUSS eindeutig identifizierbar sein.

---

## 338. Result State

Mindestens folgende Zustände SOLLEN unterstützt werden:

```text
VALID
PARTIAL
DEGRADED
NON_CONVERGED
INVALID
FAILED
CANCELLED
```

---

## 339. No Silent Failure

Fehlgeschlagene Solverläufe DÜRFEN nicht als gültige Nullfelder oder leere Ergebnisse ausgegeben werden.

---

## 340. Quality Metrics

Ergebnisse SOLLEN Qualitätsinformationen enthalten können.

---

## 341. Quality Metrics Examples

Mindestens:

```text
Residual
Convergence
EnergyBalance
MeshQuality
TimeStepQuality
BoundaryReflection
NumericalErrorEstimate
```

SOLLTEN unterstützt werden.

---

## 342. Provenance

Photonics Compute MUSS vollständige Provenance unterstützen.

---

## 343. Provenance Record

Ein:

`PhotonicsProvenanceRecord`

SOLL mindestens enthalten:

```text
ModelId
ModelVersion
GeometryVersion
MaterialVersions
SourceConfiguration
BoundaryConditions
MeshConfiguration
Solver
SolverVersion
SolverParameters
Precision
Backend
RuntimeVersion
Timestamp
```

---

## 344. Extended Provenance

Zusätzlich SOLLEN gespeichert werden können:

```text
Device
GPU
NodeCount
RandomSeed
OptimizationIteration
Checkpoint
ExternalLibraryVersions
```

---

## 345. Processing Graph

Eine photonische Berechnung SOLL als Graph darstellbar sein.

Beispiel:

```text
Geometry
   |
   +------> Materials
   |
   +------> Sources
   |
   +------> Boundaries
   |
   v
Discretization
   |
   v
Solver
   |
   v
Field Solution
   |
   +------------+
   |            |
   v            v
Modes       Monitors
   |            |
   +------+-----+
          |
          v
       Result
```

---

## 346. Diagnostics

Photonics Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 347. Diagnostic Classes

Mindestens:

```text
INVALID_MODEL
INVALID_GEOMETRY
INVALID_MATERIAL
MATERIAL_OUT_OF_RANGE
INVALID_SOURCE
INVALID_BOUNDARY
INVALID_MESH
INSUFFICIENT_RESOLUTION
INVALID_TIME_STEP
COURANT_VIOLATION
SOLVER_DIVERGENCE
NON_CONVERGENCE
INVALID_MODE
MODE_NOT_FOUND
PML_FAILURE
BOUNDARY_REFLECTION
ENERGY_IMBALANCE
NUMERICAL_INSTABILITY
OUT_OF_MEMORY
GPU_MEMORY_EXHAUSTED
BACKEND_FAILURE
CHECKPOINT_FAILURE
CANCELLED
```

SOLLTEN existieren.

---

## 348. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 349. Diagnostic Context

Diagnosen SOLLEN enthalten können:

```text
ModelId
SolverId
GeometryObject
MaterialId
Frequency
Wavelength
Iteration
Backend
Timestamp
```

---

## 350. Profiling

Photonics Compute MUSS Profiling unterstützen.

---

## 351. Profiling Stages

Mindestens folgende Bereiche SOLLTEN separat messbar sein:

```text
GeometryPreparation
MaterialEvaluation
MeshGeneration
SolverInitialization
FieldSolve
ModeSolve
BoundaryProcessing
Communication
PostProcessing
FarFieldTransform
Optimization
```

---

## 352. Solver Time

Gesamte Solverlaufzeit MUSS messbar sein.

---

## 353. Iteration Time

Iterative Solver SOLLEN Iterationszeiten messen können.

---

## 354. Memory Usage

Speicherverbrauch SOLL messbar sein.

---

## 355. GPU Usage

GPU-Auslastung SOLL bei GPU-Backends messbar sein.

---

## 356. Communication Time

Verteilte Solver SOLLEN Kommunikationszeit messen können.

---

## 357. Testing — Material

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
Vacuum
ConstantIndex
ComplexIndex
DispersiveMaterial
AnisotropicMaterial
InvalidMaterial
OutOfRangeMaterial
```

---

## 358. Testing — Plane Wave

Eine ebene Welle im homogenen Medium MUSS gegen analytische Lösungen getestet werden.

---

## 359. Testing — Interface

Reflexion und Transmission an einer planaren Grenzfläche MÜSSEN gegen Fresnel-Gleichungen geprüft werden.

---

## 360. Testing — Total Internal Reflection

Totalreflexion MUSS getestet werden.

---

## 361. Testing — Brewster Angle

Der Brewster-Winkel SOLL gegen analytische Werte geprüft werden.

---

## 362. Testing — Interference

Konstruktive und destruktive Interferenz MÜSSEN getestet werden.

---

## 363. Testing — Diffraction

Fraunhofer- und Fresnel-Testfälle SOLLTEN gegen bekannte Lösungen geprüft werden.

---

## 364. Testing — Waveguide

Mindestens:

```text
SlabWaveguide
SingleModeWaveguide
MultiModeWaveguide
ModeCutoff
```

SOLLTEN getestet werden.

---

## 365. Testing — Fiber

Mindestens:

```text
StepIndexFiber
FundamentalMode
NumericalAperture
VNumber
```

SOLLTEN getestet werden.

---

## 366. Testing — Resonator

Mindestens:

```text
FabryPerot
RingResonator
ResonanceFrequency
QFactor
FreeSpectralRange
```

SOLLTEN getestet werden.

---

## 367. Testing — FDTD

Mindestens:

```text
StableTimeStep
UnstableTimeStep
PlaneWavePropagation
BoundaryReflection
PMLAbsorption
```

MÜSSEN getestet werden.

---

## 368. Testing — Eigenmode

Mindestens:

```text
FundamentalMode
HigherOrderMode
ModeOrdering
ModeTracking
NoMode
```

SOLLTEN getestet werden.

---

## 369. Testing — Polarization

Mindestens:

```text
Linear
Circular
Elliptical
JonesMatrix
StokesParameters
```

SOLLTEN getestet werden.

---

## 370. Testing — Multilayer

Thin-Film-Transmission und Reflexion SOLLTEN gegen analytische beziehungsweise Referenzlösungen geprüft werden.

---

## 371. Testing — Photonic Crystal

Mindestens:

```text
PeriodicCell
BlochBoundary
BandStructure
BandGap
```

SOLLTEN getestet werden.

---

## 372. Testing — Nonlinear

Mindestens geeignete Referenzfälle für:

```text
KerrEffect
SelfPhaseModulation
SecondHarmonicGeneration
```

SOLLTEN unterstützt werden.

---

## 373. Testing — Energy Conservation

Leistungsbilanzen MÜSSEN innerhalb definierter Toleranzen geprüft werden.

---

## 374. Testing — CPU/GPU

CPU- und GPU-Ergebnisse SOLLTEN innerhalb definierter numerischer Toleranzen übereinstimmen.

---

## 375. Testing — Determinism

Deterministische Solver MÜSSEN reproduzierbare Ergebnisse liefern.

---

## 376. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
Large3DField
FineFDTDGrid
LargeFEMMesh
BroadbandSweep
ThousandsOfFrequencies
LargeParameterSweep
ManyModes
ComplexDispersiveMaterials
LargePhotonicCircuit
LongPulsePropagation
LargeOptimization
MultiGPU
DistributedDomain
OutOfCoreFields
CheckpointRestart
```

---

## 377. Performance Tests

Mindestens folgende Kennzahlen SOLLTEN erhoben werden:

```text
CellsPerSecond
ElementsPerSecond
IterationsPerSecond
FrequenciesPerSecond
ModesPerSecond
MemoryBandwidth
MemoryUsage
GPUUtilization
CommunicationOverhead
SolverScaling
```

---

## 378. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
PhotonicsModel
PhotonicsGeometry
PhotonicsMaterial
PhotonicsSource
PhotonicsDetector
PhotonicsBoundaryCondition

Cartesian Coordinates
2D
3D

Vacuum
Constant Refractive Index
Complex Refractive Index

Plane Wave
Gaussian Beam

Electric Field
Magnetic Field
Complex Fields

Wavelength
Frequency
Wavenumber

Reflection
Transmission
Fresnel Equations

Linear Polarization
TE
TM

Interference
Optical Path Length

Basic Diffraction

Waveguide Model
Eigenmode Architecture

FDTD Architecture
Frequency-Domain Architecture

PML Architecture

CPU Backend
GPU Backend Architecture

FP64
Complex FP64

Scheduler Integration
Memory Model Integration
Object Model Integration

Diagnostics
Profiling
Provenance
Cancellation
```

---

## 379. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
FDTD
FEM
BPM
RCWA
Transfer Matrix
Scattering Matrix
Eigenmode Solver
Coupled Mode Theory

Adaptive Mesh
Domain Decomposition
Multi-GPU
Distributed Compute
HPC

Dispersive Materials
Anisotropic Materials
Birefringence
Nonlinear Materials
Thermo-Optic Effects
Electro-Optic Effects

Fibers
Waveguides
Resonators
Photonic Crystals
Metamaterials
Plasmonics
Nanophotonics

Photonic Integrated Circuits
S Parameters
Couplers
Modulators
Filters

Pulse Propagation
Dispersion
Nonlinear Optics

Far Field
Near-to-Far Transformation
Scattering

Optimization
Adjoint Optimization
Inverse Design
Monte Carlo
Tolerance Analysis

AI Surrogate Models
NPU Integration
```

---

## 380. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-PHOTONICS-0001` MUSS:

1. photonische Modelle als eigenständige Compute-Objekte repräsentieren.
2. Modelle eindeutig identifizieren.
3. Modelle versionieren können.
4. Geometrie explizit repräsentieren.
5. Materialien explizit repräsentieren.
6. Quellen explizit repräsentieren.
7. Detektoren beziehungsweise Monitore repräsentieren.
8. Randbedingungen explizit repräsentieren.
9. Solverkonfigurationen explizit repräsentieren.
10. physikalische Einheiten verwalten.
11. Wellenlängen unterstützen.
12. Frequenzen unterstützen.
13. Wellenzahlen unterstützen.
14. komplexwertige elektromagnetische Felder unterstützen.
15. elektrische Felder repräsentieren.
16. magnetische Felder repräsentieren.
17. komplexe Brechungsindizes unterstützen.
18. dispersive Materialmodelle architektonisch unterstützen.
19. Materialgültigkeitsbereiche verwalten.
20. Materialdaten außerhalb des Gültigkeitsbereichs erkennen.
21. ebene Wellen unterstützen.
22. Gaussian-Beam-Quellen unterstützen.
23. Quellpolarisation repräsentieren.
24. Quellphase repräsentieren.
25. Quellrichtung repräsentieren.
26. Reflexion unterstützen.
27. Transmission unterstützen.
28. Fresnel-Koeffizienten berechnen können.
29. TE-Polarisation unterstützen.
30. TM-Polarisation unterstützen.
31. Interferenz unterstützen.
32. kohärente und inkohärente Überlagerung unterscheiden.
33. optische Weglänge berechnen können.
34. Beugung architektonisch unterstützen.
35. Wellenleiter architektonisch unterstützen.
36. Eigenmoden architektonisch unterstützen.
37. effektive Brechungsindizes repräsentieren.
38. Propagationskonstanten repräsentieren.
39. FDTD architektonisch unterstützen.
40. Frequenzbereichssolver architektonisch unterstützen.
41. absorbierende Randbedingungen unterstützen.
42. PML architektonisch unterstützen.
43. numerische Stabilitätsbedingungen validieren.
44. Konvergenzstatus verwalten.
45. nicht konvergierte Ergebnisse kennzeichnen.
46. Leistungsfluss berechnen können.
47. Energie- beziehungsweise Leistungsbilanz architektonisch prüfen können.
48. CPU-Ausführung unterstützen.
49. GPU-Ausführung architektonisch unterstützen.
50. keine GPU voraussetzen.
51. FP64 unterstützen.
52. komplexes FP64 unterstützen.
53. `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.
54. `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.
55. `NPSPEC-COMPUTE-RUNTIME-0004` integrieren.
56. Batch- beziehungsweise Sweep-Verarbeitung unterstützen.
57. Cancellation unterstützen.
58. Partial Results kennzeichnen.
59. Checkpointing architektonisch unterstützen.
60. Out-of-Core-Verarbeitung architektonisch unterstützen.
61. vollständige Provenance unterstützen.
62. Geometrieversion dokumentieren.
63. Materialversionen dokumentieren.
64. Solverversion dokumentieren.
65. Solverparameter dokumentieren.
66. Backend dokumentieren.
67. Precision dokumentieren.
68. strukturierte Diagnosen bereitstellen.
69. Profiling unterstützen.
70. Solverlaufzeiten messen können.
71. Speicherverbrauch messen können.
72. deterministische Ausführung unterstützen können.
73. Random Seeds für stochastische Verfahren unterstützen.
74. Multi-Core-Verarbeitung architektonisch unterstützen.
75. Multi-GPU architektonisch unterstützen.
76. Distributed Compute architektonisch unterstützen.
77. Wellenleiterverluste architektonisch unterstützen.
78. Resonatoren architektonisch unterstützen.
79. Q-Faktoren architektonisch unterstützen.
80. Polarisation vollständig erweiterbar modellieren.
81. Jones-Darstellung architektonisch unterstützen.
82. Stokes-Darstellung architektonisch unterstützen.
83. mehrschichtige optische Systeme architektonisch unterstützen.
84. photonische Kristalle architektonisch unterstützen.
85. Bloch-periodische Randbedingungen architektonisch unterstützen.
86. nichtlineare Optik architektonisch unterstützen.
87. Pulspropagation architektonisch unterstützen.
88. photonisch integrierte Schaltungen architektonisch unterstützen.
89. S-Parameter architektonisch unterstützen.
90. Optimierung architektonisch unterstützen.
91. Inverse Design architektonisch unterstützen.
92. Multi-Physics-Kopplung architektonisch unterstützen.
93. `NPSPEC-COMPUTE-LASER-0001` integrieren können.
94. `NPSPEC-COMPUTE-LENS-0001` integrieren können.
95. `NPSPEC-COMPUTE-RAYTRACING-0001` integrieren können.
96. `NPSPEC-COMPUTE-SPECTROSCOPY-0001` integrieren können.
97. `NPSPEC-COMPUTE-MESH-0001` integrieren können.
98. AI-basierte Beschleunigungs- und Inverse-Design-Verfahren erweiterbar integrieren können.
99. neue Solver ohne Änderung des grundlegenden Photonics-Objektmodells integrieren können.
100. physikalisches Modell und numerischen Solver klar voneinander trennen.

---

## 381. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaPhotonicsCompute
 |
 +-- PhotonicsRuntime
 |
 +-- PhotonicsModelManager
 |
 +-- GeometryEngine
 |
 +-- MaterialEngine
 |    |
 |    +-- DispersionEngine
 |    +-- NonlinearMaterialEngine
 |    +-- MaterialLibrary
 |
 +-- SourceEngine
 |
 +-- DetectorEngine
 |
 +-- BoundaryEngine
 |
 +-- MeshBridge
 |
 +-- SolverManager
 |    |
 |    +-- FDTDSolver
 |    +-- FEMSolver
 |    +-- FrequencyDomainSolver
 |    +-- EigenmodeSolver
 |    +-- BPMSolver
 |    +-- RCWASolver
 |    +-- TransferMatrixSolver
 |    +-- CoupledModeSolver
 |
 +-- WaveguideEngine
 |
 +-- FiberEngine
 |
 +-- ResonatorEngine
 |
 +-- PolarizationEngine
 |
 +-- DiffractionEngine
 |
 +-- InterferenceEngine
 |
 +-- MultilayerEngine
 |
 +-- PhotonicCrystalEngine
 |
 +-- PlasmonicsEngine
 |
 +-- NonlinearOpticsEngine
 |
 +-- PulsePropagationEngine
 |
 +-- PhotonicCircuitEngine
 |
 +-- FarFieldEngine
 |
 +-- OptimizationEngine
 |
 +-- MultiPhysicsBridge
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

## 382. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt die gemeinsame Ausführungsinfrastruktur bereit.

```text
Photonics Compute
       |
       v
 Compute Runtime
       |
 +-----+------+------+
 |            |      |
 v            v      v
CPU          GPU    NPU
```

---

## 383. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert insbesondere:

```text
Field Solves
Mode Solves
Frequency Sweeps
Wavelength Sweeps
Parameter Sweeps
Optimization
Post Processing
```

---

## 384. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet insbesondere:

```text
Geometry
Materials
Meshes
Fields
Modes
Solver Workspaces
Monitor Data
Results
```

---

## 385. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Photonische Ressourcen SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 386. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

Die GPU Runtime kann insbesondere beschleunigen:

```text
FDTD
FFT
Field Operations
Mode Solvers
Matrix Operations
Parameter Sweeps
Post Processing
```

---

## 387. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

Die NPU Runtime kann insbesondere:

```text
Surrogate Models
Inverse Design
Field Prediction
Mode Classification
Optimization Assistance
```

beschleunigen.

---

## 388. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Distributed Compute ermöglicht:

```text
Large Parameter Sweeps
Large Optimization Jobs
Distributed Field Solves
Monte Carlo Analysis
Design Space Exploration
```

---

## 389. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

HPC Compute stellt skalierbare Ressourcen für große 3D-Feldsimulationen und Optimierungsprobleme bereit.

---

## 390. Beziehung zu `NPSPEC-COMPUTE-RAYTRACING-0001`

Raytracing Compute stellt geometrisch-optische Strahlenmodelle bereit.

Photonics Compute erweitert diese um Wellen- und elektromagnetische Effekte.

---

## 391. Beziehung zu `NPSPEC-COMPUTE-LENS-0001`

Lens Compute stellt spezialisierte Modelle für Linsen, Abbildungsoptik und optische Systeme bereit.

---

## 392. Beziehung zu `NPSPEC-COMPUTE-LASER-0001`

Laser Compute stellt spezialisierte Laserquellen, Kavitäten und Strahlmodelle bereit.

---

## 393. Beziehung zu `NPSPEC-COMPUTE-SPECTROSCOPY-0001`

Spectroscopy Compute kann photonische Quellen, optische Pfade, Filter, Resonatoren und Detektoren zur Simulation spektroskopischer Instrumente verwenden.

---

## 394. Grundlegende Photonics-Architektur

```text
Geometry
   |
   +--------+
   |        |
   v        v
Materials Sources
   |        |
   +---+----+
       |
       v
Boundary Conditions
       |
       v
Discretization
       |
       v
Solver
       |
       v
Electromagnetic Fields
       |
       v
Analysis
```

---

## 395. Wellenleiter-Architektur

```text
Waveguide Geometry
       |
       v
Materials
       |
       v
Cross Section
       |
       v
Eigenmode Solver
       |
       v
Modes
       |
       +----------------+
       |                |
       v                v
Propagation        Mode Coupling
```

---

## 396. Resonator-Architektur

```text
Resonator
    |
    v
Field Solver
    |
    v
Resonances
    |
    +----------+
    |          |
    v          v
Frequency     Q Factor
    |
    v
Mode Profile
```

---

## 397. Photonic-Circuit-Architektur

```text
Components
    |
    v
Ports
    |
    v
S Parameters
    |
    v
Circuit Network
    |
    v
Circuit Solver
    |
    v
Transmission / Reflection
```

---

## 398. Inverse-Design-Architektur

```text
Design Variables
      |
      v
Geometry
      |
      v
Photonics Solver
      |
      v
Objective Function
      |
      v
Optimizer
      |
      +----------------+
      |                |
      v                |
Updated Design --------+
```

---

## 399. Physikalischer Modellgrundsatz

Photonics Compute MUSS zwischen:

```text
Ray Model
Scalar Wave Model
Vector Wave Model
Full Maxwell Model
Reduced Order Model
```

unterscheiden.

---

## 400. Solvergrundsatz

Ein numerisches Ergebnis ist nur zusammen mit dem verwendeten Solver, dessen Diskretisierung, Toleranzen und Randbedingungen vollständig definiert.

---

## 401. Auflösungsgrundsatz

Die räumliche Diskretisierung MUSS ausreichend fein sein, um die relevanten elektromagnetischen Feldvariationen aufzulösen.

---

## 402. Dispersionsgrundsatz

Materialparameter DÜRFEN bei breitbandigen Berechnungen nicht automatisch als konstant angenommen werden, wenn ein dispersives Modell erforderlich ist.

---

## 403. Kohärenzgrundsatz

Kohärente und inkohärente optische Quellen MÜSSEN physikalisch getrennt behandelt werden.

---

## 404. Polarisationgrundsatz

Polarisation ist ein Bestandteil des elektromagnetischen Zustands und DARF nicht auf einen optionalen Darstellungsparameter reduziert werden.

---

## 405. Energiegrundsatz

Photonische Solver SOLLEN Leistungs- beziehungsweise Energieerhaltung als numerische Qualitätskontrolle verwenden.

---

## 406. Näherungsgrundsatz

Näherungsverfahren wie:

```text
Paraxial Approximation
Scalar Approximation
Effective Index Method
Coupled Mode Theory
```

MÜSSEN als solche dokumentiert werden.

---

## 407. Ergebnisgrundsatz

Ein nicht konvergiertes oder numerisch instabiles Feld DARF nicht als physikalisch gültige Lösung ausgegeben werden.

---

## 408. Provenance-Grundsatz

Ein photonisches Simulationsergebnis MUSS mindestens auf:

```text
Geometry
Materials
Sources
Boundary Conditions
Mesh
Solver
Solver Version
Solver Parameters
Precision
Backend
Runtime
```

zurückführbar sein.

---

## 409. Erweiterbarkeit

Neue:

```text
Geometry Types
Materials
Dispersion Models
Sources
Detectors
Boundary Conditions
Solvers
Mode Models
Waveguide Types
Photonic Components
Nonlinear Effects
Optimization Algorithms
AI Models
Compute Backends
```

SOLLTEN integrierbar sein, ohne das grundlegende Photonics-Objektmodell zu verändern.

---

## 410. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-PHOTONICS-DATA-0001
Photonics Data Model

NPSPEC-COMPUTE-PHOTONICS-MATERIAL-0001
Photonics Material Model

NPSPEC-COMPUTE-PHOTONICS-SOURCE-0001
Photonics Sources

NPSPEC-COMPUTE-PHOTONICS-FIELD-0001
Electromagnetic Field Model

NPSPEC-COMPUTE-PHOTONICS-BOUNDARY-0001
Photonics Boundary Conditions

NPSPEC-COMPUTE-PHOTONICS-FDTD-0001
Finite-Difference Time-Domain Solver

NPSPEC-COMPUTE-PHOTONICS-FEM-0001
Photonics Finite Element Solver

NPSPEC-COMPUTE-PHOTONICS-FREQUENCY-0001
Frequency-Domain Solver

NPSPEC-COMPUTE-PHOTONICS-MODE-0001
Photonics Eigenmode Solver

NPSPEC-COMPUTE-PHOTONICS-BPM-0001
Beam Propagation Method

NPSPEC-COMPUTE-PHOTONICS-RCWA-0001
Rigorous Coupled-Wave Analysis

NPSPEC-COMPUTE-PHOTONICS-WAVEGUIDE-0001
Optical Waveguides

NPSPEC-COMPUTE-PHOTONICS-FIBER-0001
Optical Fibers

NPSPEC-COMPUTE-PHOTONICS-RESONATOR-0001
Photonics Resonators

NPSPEC-COMPUTE-PHOTONICS-POLARIZATION-0001
Photonics Polarization

NPSPEC-COMPUTE-PHOTONICS-DIFFRACTION-0001
Photonics Diffraction

NPSPEC-COMPUTE-PHOTONICS-MULTILAYER-0001
Multilayer Optics

NPSPEC-COMPUTE-PHOTONICS-CRYSTAL-0001
Photonic Crystals

NPSPEC-COMPUTE-PHOTONICS-PLASMONICS-0001
Plasmonics

NPSPEC-COMPUTE-PHOTONICS-NONLINEAR-0001
Nonlinear Photonics

NPSPEC-COMPUTE-PHOTONICS-PULSE-0001
Optical Pulse Propagation

NPSPEC-COMPUTE-PHOTONICS-CIRCUIT-0001
Photonic Integrated Circuits

NPSPEC-COMPUTE-PHOTONICS-SCATTERING-0001
Photonics Scattering

NPSPEC-COMPUTE-PHOTONICS-FARFIELD-0001
Near-to-Far Field Processing

NPSPEC-COMPUTE-PHOTONICS-OPTIMIZATION-0001
Photonics Optimization

NPSPEC-COMPUTE-PHOTONICS-INVERSEDESIGN-0001
Photonics Inverse Design

NPSPEC-COMPUTE-PHOTONICS-MULTIPHYSICS-0001
Photonics Multi-Physics Coupling

NPSPEC-COMPUTE-PHOTONICS-UNCERTAINTY-0001
Photonics Uncertainty and Tolerance Analysis

NPSPEC-COMPUTE-PHOTONICS-PROVENANCE-0001
Photonics Provenance

NPSPEC-COMPUTE-PHOTONICS-TEST-0001
Photonics Conformance Tests
```

---

## 411. Zusammenfassung

`NPSPEC-COMPUTE-PHOTONICS-0001` definiert die gemeinsame Compute-Infrastruktur zur Modellierung, Simulation, Analyse und Optimierung photonischer Systeme innerhalb der NovaOS-Compute-Architektur.

Die grundlegende Verarbeitung lautet:

```text
Geometry
   +
Materials
   +
Sources
   +
Boundary Conditions
   |
   v
Photonics Solver
   |
   v
Electromagnetic / Optical Solution
   |
   v
Analysis
   |
   v
Photonics Result
```

Die Architektur unterstützt:

```text
Geometrical Optics
Wave Optics
Electromagnetic Optics

FDTD
FEM
Frequency Domain
Eigenmode
BPM
RCWA
Transfer Matrix
Coupled Mode Theory

Waveguides
Fibers
Resonators
Interferometers
Gratings
Thin Films
Photonic Crystals
Metamaterials
Plasmonics
Nanophotonics
Photonic Integrated Circuits

Reflection
Transmission
Interference
Diffraction
Polarization
Dispersion
Scattering
Absorption

Pulse Propagation
Nonlinear Optics
Multi-Physics
Optimization
Inverse Design
```

Die Berechnungen können über:

```text
CPU
SIMD
Multi-Core
GPU
Multi-GPU
NPU
Distributed Compute
HPC
```

beschleunigt beziehungsweise skaliert werden.

Die Architektur trennt konsequent:

```text
Physical Model
Geometry
Material Model
Source
Boundary Condition
Discretization
Solver
Numerical Configuration
Field Solution
Analysis
Interpretation
```

voneinander.

Die zentrale Modellregel lautet:

> Ein photonisches Ergebnis ist nur im Kontext des verwendeten physikalischen Modells gültig. Geometrische Optik, skalare Wellenmodelle, Vektormodelle und vollständige Maxwell-Simulationen dürfen deshalb nicht als austauschbare Berechnungsmethoden behandelt werden.

Die zentrale Materialregel lautet:

> Photonische Materialeigenschaften sind im Allgemeinen abhängig von Wellenlänge, Frequenz, Temperatur, Polarisation und weiteren physikalischen Bedingungen. Materialdaten müssen deshalb zusammen mit ihrem Gültigkeitsbereich und ihrer Provenance verwaltet werden.

Die zentrale Solverregel lautet:

> Eine elektromagnetische Feldlösung ist nur zusammen mit Geometrie, Materialien, Quellen, Randbedingungen, Diskretisierung, Solver, numerischen Toleranzen und Präzision vollständig definiert.

Die zentrale Auflösungsregel lautet:

> Eine numerische Simulation kann physikalische Strukturen und Feldvariationen nur dann zuverlässig erfassen, wenn ihre räumliche und zeitliche Diskretisierung für die relevanten Wellenlängen, Materialien und Geometrien ausreichend fein ist.

Die zentrale Qualitätsregel lautet:

> Konvergenz, Stabilität, Randreflexionen und Energieerhaltung sind Bestandteile der Ergebnisqualität und müssen von Photonics Compute als technische Eigenschaften der Lösung behandelt werden.

Das grundlegende Architekturprinzip lautet:

> NovaOS Photonics Compute stellt eine solverunabhängige photonische Compute-Schicht bereit, in der geometrische Optik, Wellenoptik, elektromagnetische Feldsimulation, Wellenleiter, Fasern, Resonatoren, photonische Schaltungen, Nanophotonik, nichtlineare Optik und Inverse Design über gemeinsame versionierte Objekte, Materialmodelle, Quellen, Randbedingungen, Ergebnisse und Provenance miteinander verbunden werden können.