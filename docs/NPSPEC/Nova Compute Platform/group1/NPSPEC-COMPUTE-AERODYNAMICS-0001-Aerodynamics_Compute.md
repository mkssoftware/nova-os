# NPSPEC-COMPUTE-AERODYNAMICS-0001 — Aerodynamics Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Specialized Compute  
**Komponente:** NovaOS Aerodynamics Compute  
**Bezeichner:** `NPSPEC-COMPUTE-AERODYNAMICS-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HYDRAULICS-0001`  
**Kurzname:** Aerodynamics Compute  

---

## 1. Zweck

Diese Spezifikation definiert **Aerodynamics Compute** innerhalb der NovaOS Compute-Architektur.

Aerodynamics Compute stellt standardisierte Rechenprimitive, physikalische Modelle, Datenstrukturen und Solver-Schnittstellen für aerodynamische und gasdynamische Berechnungen bereit.

Die Komponente dient als gemeinsame systemweite Grundlage für Anwendungen und Fähigkeiten, die unter anderem folgende Größen berechnen oder simulieren:

```text
Airflow
Velocity
Pressure
Density
Temperature
Mach Number
Reynolds Number
Dynamic Pressure
Lift
Drag
Side Force
Moments
Pressure Distribution
Boundary Layers
Compressibility Effects
Shock Waves
Wake Flow
```

Typische Einsatzgebiete sind:

```text
Aircraft Design
Automotive Aerodynamics
Railway Aerodynamics
Drone Design
Wind Engineering
Turbomachinery
Propeller Analysis
Rotor Analysis
Ventilation
Industrial Flow
Sports Engineering
Simulation
Digital Twins
Optimization
Research
Education
```

Aerodynamics Compute ist keine einzelne CFD-Anwendung.

Die Komponente stellt gemeinsame aerodynamische Compute-Fähigkeiten bereit, auf denen unterschiedliche NovaOS-Anwendungen und Fähigkeiten aufbauen können.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Aerodynamische Physik wird als wiederverwendbare systemweite Compute-Fähigkeit bereitgestellt und von der konkreten Anwendung, Darstellung und Compute-Hardware getrennt.

Eine Anwendung SOLL grundlegende aerodynamische Funktionen nicht selbst erneut implementieren müssen.

Beispiele:

```text
Mach Number
Reynolds Number
Dynamic Pressure
Lift Coefficient
Drag Coefficient
Atmosphere Properties
Compressibility Corrections
Pressure Coefficient
```

---

## 3. Ziele

Aerodynamics Compute MUSS folgende Ziele verfolgen:

1. einheitliches aerodynamisches Datenmodell,
2. SI-basierte interne Berechnung,
3. sichere Einheitenkonvertierung,
4. dimensionssichere Größen,
5. Gas- und Atmosphärenmodelle,
6. inkompressible Strömungsmodelle,
7. kompressible Strömungsmodelle,
8. Unterschallströmung,
9. transsonische Strömung,
10. Überschallströmung,
11. Erweiterbarkeit für Hyperschallströmung,
12. aerodynamische Kraftberechnung,
13. Momentenberechnung,
14. Profilberechnung,
15. Flügelmodelle,
16. Widerstandsmodelle,
17. Grenzschichtmodelle,
18. Druckverteilungen,
19. einfache Strömungsfeldmodelle,
20. Erweiterbarkeit für CFD,
21. stationäre Solver,
22. transiente Solver,
23. CPU-Beschleunigung,
24. GPU-Beschleunigung,
25. Distributed Compute,
26. deterministische Berechnungsmodi,
27. Diagnose und Validierung,
28. Optimierung,
29. Digital-Twin-Integration,
30. Erweiterbarkeit für zukünftige physikalische Modelle.

---

## 4. Nicht-Ziele

Diese Basisspezifikation definiert nicht vollständig:

- einen vollständigen industriellen CFD-Solver,
- konkrete Finite-Volume-Diskretisierung,
- konkrete Finite-Elemente-Diskretisierung,
- vollständige Turbulenzmodellierung,
- vollständige Verbrennungsmodelle,
- vollständige chemische Reaktionsmodelle,
- vollständige Mehrphasenströmung,
- Aeroakustik,
- vollständige Fluid-Struktur-Interaktion,
- konkrete CAD-Oberflächen,
- konkrete Meshing-Anwendungen.

Diese Systeme DÜRFEN auf Aerodynamics Compute aufbauen.

---

## 5. Grundarchitektur

Die grundlegende Architektur lautet:

```text
Application / Capability
        |
        v
Nova Aerodynamics API
        |
        v
Aerodynamics Compute
        |
        +-- Unit System
        |
        +-- Gas Models
        |
        +-- Atmosphere Models
        |
        +-- Geometry Models
        |
        +-- Aerodynamic Models
        |
        +-- Flow Models
        |
        +-- Solver Layer
        |
        +-- Diagnostics
        |
        +-- Optimization
        |
        v
Nova Compute Runtime
        |
        +-- CPU
        |
        +-- GPU
        |
        +-- NPU
        |
        +-- Distributed Compute
```

---

## 6. Physikalische Größen

Aerodynamics Compute MUSS mindestens folgende physikalische Größen repräsentieren können:

```text
Length
Area
Volume

Time

Velocity
Acceleration

AngularVelocity

Pressure
StaticPressure
DynamicPressure
TotalPressure

Temperature

Density
DynamicViscosity
KinematicViscosity

Mass
MassFlowRate

Force
Moment
Torque

Power
Energy

SpeedOfSound

MachNumber
ReynoldsNumber

LiftCoefficient
DragCoefficient
SideForceCoefficient
PressureCoefficient
MomentCoefficient

Angle
AngleOfAttack
SideslipAngle
```

---

## 7. Einheitensystem

Intern SOLL bevorzugt das SI-System verwendet werden.

Beispiele:

```text
Length             -> m
Area               -> m²
Volume             -> m³
Time               -> s
Velocity           -> m/s
Acceleration       -> m/s²
Pressure           -> Pa
Temperature        -> K
Density            -> kg/m³
DynamicViscosity   -> Pa·s
KinematicViscosity -> m²/s
Mass               -> kg
MassFlowRate       -> kg/s
Force              -> N
Moment             -> N·m
Power              -> W
```

---

## 8. Darstellungseinheiten

Anwendungen DÜRFEN alternative Darstellungseinheiten verwenden.

Beispiele:

```text
km/h
mph
knots

ft
in

psi
bar
hPa

°C
°F

lbf
```

Die interne physikalische Bedeutung MUSS erhalten bleiben.

---

## 9. Dimensionsprüfung

Physikalische Größen SOLLEN dimensionssicher behandelt werden.

Operationen wie:

```text
Pressure + Velocity
```

MÜSSEN als dimensionsinkompatibel erkannt werden, sofern keine explizite Transformation definiert wurde.

---

## 10. Numeric Representation

Standardmäßig SOLL mindestens:

```text
IEEE 754 Binary64
```

unterstützt werden.

Optional DÜRFEN verwendet werden:

```text
Binary32
Extended Precision
Arbitrary Precision
Mixed Precision
```

---

## 11. Precision Policy

Eine Berechnung DARF folgende Precision Policies besitzen:

```text
FAST
BALANCED
ACCURATE
DETERMINISTIC
```

---

## 12. Coordinate System

Aerodynamics Compute MUSS eindeutig definierte Koordinatensysteme unterstützen.

Mindestens:

```text
WORLD
BODY
WIND
LOCAL
```

---

## 13. Body Coordinate System

Ein typisches körperfestes Koordinatensystem DARF definiert werden als:

```text
X = Longitudinal Axis
Y = Lateral Axis
Z = Vertical Axis
```

Die konkrete Vorzeichenkonvention MUSS explizit definiert werden.

---

## 14. Wind Coordinate System

Das Wind-Koordinatensystem SOLL relativ zur Anströmrichtung definiert werden können.

---

## 15. Coordinate Transformations

Vektoren und Kräfte MÜSSEN zwischen unterstützten Koordinatensystemen transformiert werden können.

---

## 16. Flow Velocity Vector

Die Strömungsgeschwindigkeit SOLL als Vektor repräsentiert werden:

```text
V =
[
    Vx,
    Vy,
    Vz
]
```

---

## 17. Velocity Magnitude

Der Betrag lautet:

```text
|V| =
sqrt(
    Vx² +
    Vy² +
    Vz²
)
```

---

## 18. Angle of Attack

Der Anstellwinkel MUSS eindeutig anhand der verwendeten Koordinatenkonvention definiert werden.

Er wird als:

```text
α
```

bezeichnet.

---

## 19. Sideslip Angle

Der Schiebewinkel wird als:

```text
β
```

bezeichnet.

---

## 20. Gas Model

Aerodynamics Compute MUSS Gasmodelle unterstützen.

Das grundlegende Gasmodell SOLL mindestens enthalten:

```text
Density
Pressure
Temperature
GasConstant
SpecificHeatRatio
DynamicViscosity
SpeedOfSound
```

---

## 21. Ideal Gas

Eine minimale Implementierung MUSS das ideale Gasgesetz unterstützen.

```text
p =
ρ * R * T
```

mit:

```text
p = Absolute Pressure
ρ = Density
R = Specific Gas Constant
T = Absolute Temperature
```

---

## 22. Air Gas Constant

Für trockene Luft DARF standardmäßig näherungsweise verwendet werden:

```text
R_air =
287.05 J/(kg·K)
```

Die konkrete Konstante MUSS versionierbar oder konfigurierbar sein.

---

## 23. Specific Heat Ratio

Für trockene Luft bei üblichen Bedingungen DARF näherungsweise gelten:

```text
γ ≈ 1.4
```

Die Runtime DARF jedoch keinen universell konstanten Wert voraussetzen.

---

## 24. Speed of Sound

Für ein ideales Gas gilt:

```text
a =
sqrt(
    γ * R * T
)
```

mit:

```text
a = Speed of Sound
```

---

## 25. Mach Number

Aerodynamics Compute MUSS die Mach-Zahl berechnen können.

```text
M =
V / a
```

mit:

```text
M = Mach Number
V = Flow Velocity
a = Speed of Sound
```

---

## 26. Flow Regimes

Die Runtime SOLL mindestens folgende Strömungsbereiche klassifizieren können:

```text
INCOMPRESSIBLE_APPROXIMATION
SUBSONIC
TRANSONIC
SUPERSONIC
HYPERSONIC
```

Die Grenzwerte SOLLEN modellabhängig konfigurierbar sein.

---

## 27. Incompressible Approximation

Bei ausreichend niedriger Mach-Zahl DARF die Dichteänderung vernachlässigt werden.

Eine häufig verwendete Näherung ist:

```text
M < 0.3
```

Diese Grenze DARF jedoch nicht als universelle physikalische Garantie behandelt werden.

---

## 28. Reynolds Number

Aerodynamics Compute MUSS die Reynolds-Zahl berechnen können.

```text
Re =
ρ * V * L / μ
```

alternativ:

```text
Re =
V * L / ν
```

mit:

```text
ρ = Density
V = Characteristic Velocity
L = Characteristic Length
μ = Dynamic Viscosity
ν = Kinematic Viscosity
```

---

## 29. Characteristic Length

Die verwendete charakteristische Länge MUSS explizit angegeben werden.

Beispiele:

```text
Wing Chord
Vehicle Length
Cylinder Diameter
Airfoil Chord
Blade Chord
```

---

## 30. Dynamic Pressure

Aerodynamics Compute MUSS dynamischen Druck berechnen können.

```text
q =
1/2 * ρ * V²
```

---

## 31. Static Pressure

Statischer Druck MUSS explizit repräsentierbar sein.

---

## 32. Total Pressure

Gesamtdruck MUSS für geeignete Strömungsmodelle unterstützt werden.

---

## 33. Bernoulli Equation

Für geeignete stationäre inkompressible Strömungen DARF gelten:

```text
p
+
1/2 * ρ * V²
+
ρ * g * z
=
constant
```

---

## 34. Bernoulli Limitations

Die Runtime DARF die Bernoulli-Gleichung nicht automatisch auf Strömungen anwenden, für die ihre Modellannahmen nicht erfüllt sind.

---

## 35. Atmosphere Model

Aerodynamics Compute SOLL standardisierte Atmosphärenmodelle unterstützen.

---

## 36. Atmosphere State

Ein Atmosphere State SOLL mindestens enthalten:

```text
Altitude
Pressure
Temperature
Density
SpeedOfSound
DynamicViscosity
KinematicViscosity
```

---

## 37. Standard Atmosphere

Eine Implementierung SOLL ein standardisiertes Atmosphärenmodell bereitstellen können.

Das konkrete Modell und dessen Version MÜSSEN identifizierbar sein.

---

## 38. Custom Atmosphere

Anwendungen DÜRFEN eigene Atmosphärenbedingungen definieren.

Beispiele:

```text
Temperature
Pressure
Humidity
Density
Wind
```

---

## 39. Humidity

Eine erweiterte Implementierung SOLL Luftfeuchtigkeit berücksichtigen können.

---

## 40. Altitude

Atmosphärenparameter SOLLEN als Funktion der Höhe bestimmt werden können.

---

## 41. Viscosity Model

Die dynamische Viskosität von Luft SOLL temperaturabhängig modelliert werden können.

---

## 42. Sutherland Model

Eine Implementierung DARF die Sutherland-Beziehung für Luft verwenden.

Konzeptionell:

```text
μ =
μ_ref
*
(T / T_ref)^(3/2)
*
(T_ref + S)
/
(T + S)
```

Die verwendeten Konstanten MÜSSEN eindeutig definiert sein.

---

## 43. Aerodynamic Reference Geometry

Aerodynamische Koeffizienten benötigen definierte Referenzgrößen.

Mindestens:

```text
ReferenceArea
ReferenceLength
ReferenceSpan
ReferencePoint
```

---

## 44. Reference Area

Die Referenzfläche MUSS explizit angegeben werden.

Beispiele:

```text
Wing Planform Area
Frontal Area
Rotor Disk Area
```

---

## 45. Reference Length

Die Referenzlänge MUSS für Momentenkoeffizienten und Reynolds-Zahlen eindeutig definiert sein.

---

## 46. Aerodynamic Force

Die aerodynamische Gesamtkraft SOLL als Vektor repräsentiert werden:

```text
F_aero =
[
    Fx,
    Fy,
    Fz
]
```

---

## 47. Lift

Der Auftrieb lautet:

```text
L =
C_L * q * S
```

mit:

```text
L   = Lift
C_L = Lift Coefficient
q   = Dynamic Pressure
S   = Reference Area
```

---

## 48. Drag

Der Widerstand lautet:

```text
D =
C_D * q * S
```

mit:

```text
D   = Drag
C_D = Drag Coefficient
```

---

## 49. Side Force

Die Seitenkraft DARF berechnet werden als:

```text
Y =
C_Y * q * S
```

---

## 50. Aerodynamic Moment

Ein aerodynamisches Moment kann allgemein beschrieben werden durch:

```text
M =
C_M * q * S * L_ref
```

---

## 51. Force Coefficients

Mindestens folgende dimensionslose Koeffizienten SOLLEN unterstützt werden:

```text
C_L
C_D
C_Y
```

---

## 52. Moment Coefficients

Mindestens:

```text
C_l
C_m
C_n
```

für Roll-, Nick- und Giermoment.

Die verwendete Konvention MUSS explizit definiert sein.

---

## 53. Pressure Coefficient

Aerodynamics Compute MUSS den Druckbeiwert berechnen können.

Für geeignete inkompressible Referenzbedingungen:

```text
C_p =
(p - p_inf) / q_inf
```

---

## 54. Surface Pressure Distribution

Eine Oberfläche SOLL eine räumliche Druckverteilung besitzen können.

Beispiel:

```text
SurfacePoint -> Pressure
```

---

## 55. Surface Force Integration

Druck- und Schubspannungsverteilungen SOLLEN über Oberflächen integriert werden können, um Gesamtkräfte und Momente zu bestimmen.

---

## 56. Pressure Force

Die lokale Druckkraft lautet konzeptionell:

```text
dF_p =
-p * n * dA
```

mit:

```text
n = Surface Normal
```

---

## 57. Skin Friction

Viskose Oberflächenschubspannung SOLL berücksichtigt werden können.

---

## 58. Skin Friction Coefficient

Ein lokaler oder globaler Skin-Friction-Koeffizient DARF verwendet werden:

```text
C_f
```

---

## 59. Drag Components

Der Gesamtwiderstand SOLL in Komponenten zerlegbar sein.

Beispiele:

```text
Pressure Drag
Skin Friction Drag
Induced Drag
Wave Drag
Interference Drag
Cooling Drag
```

---

## 60. Drag Decomposition

Die Runtime SOLL nachvollziehbar angeben können, welche Widerstandsanteile durch das verwendete Modell berechnet werden.

---

## 61. Lift-to-Drag Ratio

Aerodynamics Compute SOLL berechnen können:

```text
L/D =
C_L / C_D
```

sofern:

```text
C_D != 0
```

---

## 62. Airfoil Model

Aerodynamics Compute SOLL zweidimensionale Profilmodelle unterstützen können.

---

## 63. Airfoil Geometry

Ein Airfoil Object SOLL mindestens beschreiben können:

```text
UpperSurface
LowerSurface
LeadingEdge
TrailingEdge
Chord
Thickness
Camber
```

---

## 64. Airfoil Coordinates

Profilgeometrien DÜRFEN über diskrete Koordinaten beschrieben werden.

---

## 65. Normalized Airfoil Coordinates

Profilkoordinaten SOLLEN auf die Sehnenlänge normiert werden können.

Beispiel:

```text
x/c
y/c
```

---

## 66. Airfoil Operating Point

Ein Profilbetriebspunkt SOLL mindestens enthalten:

```text
AngleOfAttack
MachNumber
ReynoldsNumber
TransitionState
```

---

## 67. Airfoil Result

Eine Profilberechnung SOLL mindestens liefern können:

```text
LiftCoefficient
DragCoefficient
MomentCoefficient
PressureDistribution
TransitionInformation
SeparationInformation
```

soweit das verwendete Modell diese Größen unterstützt.

---

## 68. Thin Airfoil Theory

Eine einfache Implementierung DARF Dünnprofiltheorie unterstützen.

Für ein ideales dünnes symmetrisches Profil bei kleinen Winkeln gilt näherungsweise:

```text
C_L =
2π * α
```

mit:

```text
α
```

in Radiant.

---

## 69. Thin Airfoil Limitations

Diese Beziehung DARF nicht außerhalb ihrer Modellannahmen als allgemeingültige Auftriebsberechnung verwendet werden.

---

## 70. Lift Curve

Profil- und Flügelmodelle SOLLEN Auftriebskennlinien repräsentieren können.

Beispiel:

```text
C_L = f(α)
```

---

## 71. Stall

Aerodynamics Compute SOLL Stall als möglichen aerodynamischen Zustand modellieren können.

---

## 72. Stall Angle

Ein Modell DARF einen kritischen Anstellwinkel definieren:

```text
α_stall
```

Dieser Wert MUSS vom konkreten Profil, Reynolds-Bereich, Mach-Bereich und Modell abhängen können.

---

## 73. Post-Stall

Post-Stall-Verhalten DARF durch separate empirische oder numerische Modelle beschrieben werden.

---

## 74. Wing Model

Aerodynamics Compute SOLL dreidimensionale Flügelmodelle unterstützen.

---

## 75. Wing Geometry

Ein Wing Object SOLL mindestens beschreiben können:

```text
Span
Area
AspectRatio
RootChord
TipChord
Sweep
Dihedral
Twist
AirfoilSections
```

---

## 76. Aspect Ratio

Das Seitenverhältnis lautet:

```text
AR =
b² / S
```

mit:

```text
b = Span
S = Wing Area
```

---

## 77. Taper Ratio

Die Zuspitzung lautet:

```text
λ =
c_tip / c_root
```

---

## 78. Wing Loading

Optional DARF berechnet werden:

```text
W/S
```

wenn Gewicht und Flügelfläche bekannt sind.

---

## 79. Finite Wing Effects

Dreidimensionale Flügelmodelle SOLLEN endliche Spannweiteneffekte berücksichtigen können.

---

## 80. Induced Drag

Ein vereinfachtes Modell lautet:

```text
C_Di =
C_L²
/
(π * AR * e)
```

mit:

```text
e = Oswald Efficiency Factor
```

---

## 81. Drag Polar

Eine einfache Widerstandspolare DARF modelliert werden als:

```text
C_D =
C_D0
+
k * C_L²
```

---

## 82. Lift Distribution

Eine Flügelberechnung SOLL die Auftriebsverteilung entlang der Spannweite bestimmen können.

---

## 83. Lifting-Line Theory

Eine erweiterte Implementierung SOLL Prandtls Traglinientheorie unterstützen können.

---

## 84. Vortex Lattice Method

Eine vollständige Implementierung DARF ein Vortex-Lattice-Verfahren bereitstellen.

---

## 85. Panel Methods

Potentialströmungsbasierte Panelverfahren SOLLEN als Erweiterung unterstützt werden können.

---

## 86. Potential Flow

Aerodynamics Compute DARF Potentialströmungsmodelle bereitstellen.

---

## 87. Elementary Potential Flows

Mögliche Primitive sind:

```text
Uniform Flow
Source
Sink
Vortex
Doublet
```

---

## 88. Superposition

Lineare Potentialströmungsprimitive DÜRFEN superponiert werden.

---

## 89. Circulation

Die Zirkulation DARF als:

```text
Γ
```

repräsentiert werden.

---

## 90. Kutta-Joukowski

Für geeignete zweidimensionale ideale Strömungen DARF verwendet werden:

```text
L' =
ρ * V * Γ
```

mit:

```text
L' = Lift per Unit Span
```

---

## 91. Kutta Condition

Profilsolver auf Potentialströmungsbasis SOLLEN eine geeignete Kutta-Bedingung unterstützen können.

---

## 92. Boundary Layer

Aerodynamics Compute SOLL Grenzschichtmodelle unterstützen können.

---

## 93. Boundary Layer State

Mindestens:

```text
LAMINAR
TRANSITIONAL
TURBULENT
SEPARATED
```

---

## 94. Boundary Layer Thickness

Die Grenzschichtdicke DARF als:

```text
δ
```

repräsentiert werden.

---

## 95. Displacement Thickness

Optional:

```text
δ*
```

---

## 96. Momentum Thickness

Optional:

```text
θ
```

---

## 97. Transition

Der Übergang von laminarer zu turbulenter Grenzschicht SOLL modellierbar sein.

---

## 98. Transition Model

Transition DARF abhängig sein von:

```text
ReynoldsNumber
PressureGradient
SurfaceRoughness
TurbulenceIntensity
MachNumber
```

---

## 99. Flow Separation

Strömungsablösung SOLL als diagnostizierbarer Zustand unterstützt werden.

---

## 100. Separation Point

Ein Modell DARF einen oder mehrere Ablösepunkte bestimmen.

---

## 101. Wake

Aerodynamics Compute SOLL Nachlaufmodelle unterstützen können.

---

## 102. Wake State

Ein Wake Object DARF enthalten:

```text
VelocityDeficit
Vorticity
Turbulence
Geometry
Age
```

---

## 103. Wake Interaction

Eine erweiterte Implementierung DARF Interaktionen zwischen Nachläufen und weiteren Körpern modellieren.

---

## 104. Compressible Flow

Aerodynamics Compute MUSS grundlegende kompressible Strömungsbeziehungen unterstützen können.

---

## 105. Stagnation Temperature

Für kalorisch perfektes Gas gilt bei isentroper Strömung:

```text
T0 / T =
1
+
((γ - 1) / 2) * M²
```

---

## 106. Stagnation Pressure

Für isentrope Strömung gilt:

```text
p0 / p =
(
    1
    +
    ((γ - 1) / 2) * M²
)
^
(
    γ / (γ - 1)
)
```

---

## 107. Stagnation Density

Für isentrope Strömung gilt:

```text
ρ0 / ρ =
(
    1
    +
    ((γ - 1) / 2) * M²
)
^
(
    1 / (γ - 1)
)
```

---

## 108. Compressibility Corrections

Für geeignete Unterschallbereiche DÜRFEN vereinfachte Kompressibilitätskorrekturen unterstützt werden.

---

## 109. Prandtl-Glauert

Für geeignete linearisierte Unterschallströmungen DARF verwendet werden:

```text
C_p =
C_p0
/
sqrt(1 - M²)
```

Die Runtime MUSS die begrenzte Gültigkeit dieser Näherung berücksichtigen.

---

## 110. Transonic Flow

Transsonische Strömungen SOLLEN als eigener Regime-Typ behandelt werden.

Einfache inkompressible oder linearisierte Modelle DÜRFEN dort nicht ungeprüft verwendet werden.

---

## 111. Shock Waves

Aerodynamics Compute SOLL grundlegende Stoßwellenmodelle unterstützen können.

---

## 112. Normal Shock

Eine vollständige Implementierung SOLL Normalstoßbeziehungen für perfekte Gase unterstützen können.

---

## 113. Oblique Shock

Schräge Stoßwellen SOLLEN als Erweiterung unterstützt werden können.

---

## 114. Expansion Fan

Prandtl-Meyer-Expansionen SOLLEN für Überschallströmungen unterstützt werden können.

---

## 115. Supersonic Flow

Überschallmodelle MÜSSEN Stoß- und Expansionsphänomene berücksichtigen können, sofern diese für die gewählte Modellklasse relevant sind.

---

## 116. Hypersonic Flow

Hyperschallströmung ist als erweiterte Modellklasse vorgesehen.

Sie kann zusätzliche Effekte benötigen:

```text
High Temperature
Real Gas Effects
Chemical Reactions
Thermal Radiation
Strong Shock Waves
```

Diese Effekte sind nicht vollständig Bestandteil dieser Basisspezifikation.

---

## 117. Aerodynamic Body

Ein dreidimensionaler Körper SOLL als aerodynamisches Objekt repräsentierbar sein.

---

## 118. Body Geometry

Geometrien DÜRFEN beschrieben werden durch:

```text
Analytic Geometry
Surface Mesh
Volume Mesh
CAD-derived Surface
Point Cloud-derived Surface
```

---

## 119. Surface Mesh

Ein Surface Mesh SOLL mindestens enthalten:

```text
Vertices
Faces
Normals
BoundaryTags
MaterialOrSurfaceTags
```

---

## 120. Volume Mesh

Für CFD-Erweiterungen DARF ein Volumennetz enthalten:

```text
Cells
Faces
Vertices
Connectivity
BoundaryConditions
```

---

## 121. Mesh Validation

Vor numerischer Verwendung MUSS ein Mesh validiert werden.

Mindestens:

```text
InvalidIndices
DegenerateFaces
DegenerateCells
NonFiniteCoordinates
InvalidConnectivity
InvalidNormals
```

---

## 122. Mesh Quality

Eine erweiterte Implementierung SOLL Qualitätsmetriken unterstützen.

Beispiele:

```text
AspectRatio
Skewness
Orthogonality
CellVolume
MinimumAngle
MaximumAngle
```

---

## 123. Boundary Conditions

Aerodynamische Solver SOLLEN mindestens folgende Randbedingungsklassen unterstützen können:

```text
FAR_FIELD
INLET
OUTLET
WALL
SLIP_WALL
NO_SLIP_WALL
SYMMETRY
PERIODIC
MOVING_WALL
```

---

## 124. Far Field

Eine Far-Field-Bedingung SOLL Freiströmungsparameter definieren können.

Beispiel:

```text
Velocity
Pressure
Temperature
Density
MachNumber
FlowDirection
```

---

## 125. Wall

Eine Wand MUSS geometrische und strömungsphysikalische Randbedingungen besitzen können.

---

## 126. Slip Wall

Eine Slip-Wall-Bedingung DARF normale Durchströmung verhindern, ohne viskose Tangentialhaftung zu erzwingen.

---

## 127. No-Slip Wall

Eine viskose Wand SOLL eine No-Slip-Bedingung unterstützen können.

---

## 128. Moving Wall

Bewegte Oberflächen SOLLEN unterstützt werden können.

Beispiele:

```text
Rotating Wheel
Moving Belt
Rotor Blade
Fan Blade
```

---

## 129. Rotating Reference Frame

Eine erweiterte Implementierung SOLL rotierende Bezugssysteme unterstützen können.

---

## 130. Rotor Model

Aerodynamics Compute SOLL Rotor- und Propellermodelle ermöglichen.

---

## 131. Propeller Geometry

Ein Propeller SOLL mindestens beschreiben können:

```text
BladeCount
Diameter
Pitch
RPM
BladeSections
Twist
ChordDistribution
AirfoilDistribution
```

---

## 132. Rotor Operating Point

Mindestens:

```text
RPM
AdvanceVelocity
Density
Temperature
CollectivePitch
```

---

## 133. Propeller Thrust

Propeller- und Rotorberechnungen SOLLEN Schub bestimmen können.

```text
T
```

---

## 134. Propeller Torque

Zusätzlich SOLL Drehmoment bestimmt werden können.

```text
Q
```

---

## 135. Propeller Power

Mechanische Leistung:

```text
P =
Q * ω
```

---

## 136. Blade Element Model

Eine erweiterte Implementierung SOLL Blade-Element-Verfahren unterstützen können.

---

## 137. Blade Element Momentum

Blade Element Momentum Theory SOLL als optionale Rotor-/Propellermethode unterstützt werden können.

---

## 138. Turbomachinery

Die Architektur SOLL zukünftige aerodynamische Modelle für:

```text
Fans
Compressors
Turbines
Blowers
```

ermöglichen.

---

## 139. Ground Effect

Flügel- und Fahrzeugmodelle DÜRFEN Bodeneffekt berücksichtigen.

---

## 140. Vehicle Aerodynamics

Aerodynamics Compute SOLL Straßen- und Schienenfahrzeuge modellieren können.

Typische Größen:

```text
Drag
Lift
Downforce
Side Force
Yaw Moment
Pressure Distribution
Wake
Crosswind Response
```

---

## 141. Drag Area

Für Fahrzeuge DARF verwendet werden:

```text
C_D * A
```

als kombinierte aerodynamische Widerstandsgröße.

---

## 142. Downforce

Negativer Auftrieb DARF als Downforce dargestellt werden.

---

## 143. Crosswind

Seitenwind SOLL über einen zusätzlichen Geschwindigkeitsvektor modelliert werden können.

---

## 144. Relative Air Velocity

Die relevante Anströmung ergibt sich aus der relativen Geschwindigkeit zwischen Körper und Luft.

Konzeptionell:

```text
V_relative =
V_air
-
V_body
```

abhängig von der verwendeten Koordinatenkonvention.

---

## 145. Gust

Zeitabhängige Böen SOLLEN als Erweiterung modellierbar sein.

---

## 146. Turbulence Intensity

Freiströmung DARF eine Turbulenzintensität besitzen:

```text
I
```

---

## 147. Turbulence Models

Eine vollständige CFD-Erweiterung DARF Turbulenzmodelle unterstützen.

Beispiele:

```text
Spalart-Allmaras
k-epsilon
k-omega
k-omega SST
LES
DES
```

Diese Basisspezifikation schreibt kein bestimmtes Modell vor.

---

## 148. Laminar Solver

Ein Solver DARF rein laminare Strömung berechnen.

---

## 149. RANS

Reynolds-Averaged Navier-Stokes SOLL als mögliche CFD-Erweiterung unterstützt werden können.

---

## 150. LES

Large Eddy Simulation DARF als fortgeschrittenes Backend unterstützt werden.

---

## 151. DNS

Direct Numerical Simulation ist architektonisch zulässig, aber keine Mindestanforderung.

---

## 152. Governing Equations

Aerodynamics Compute SOLL unterschiedliche Gleichungssysteme unterstützen können.

Beispiele:

```text
Potential Flow
Euler Equations
Navier-Stokes Equations
Reynolds-Averaged Navier-Stokes
```

---

## 153. Conservation of Mass

Die allgemeine Massenerhaltung lautet:

```text
∂ρ/∂t
+
∇ · (ρu)
=
0
```

---

## 154. Incompressible Continuity

Für inkompressible Strömung gilt:

```text
∇ · u =
0
```

---

## 155. Conservation of Momentum

Die Impulsgleichung MUSS durch geeignete Solver-Backends repräsentierbar sein.

Für Newtonsche Fluide bildet die Navier-Stokes-Gleichung die grundlegende Basis.

---

## 156. Energy Equation

Kompressible und thermisch gekoppelte Solver SOLLEN eine Energiegleichung unterstützen können.

---

## 157. Equation Model Selection

Ein Solver MUSS eindeutig angeben, welches physikalische Gleichungsmodell verwendet wird.

---

## 158. Model Assumptions

Jeder Solver SOLL seine wesentlichen Modellannahmen deklarieren.

Beispiele:

```text
Steady
Unsteady
Incompressible
Compressible
Inviscid
Viscous
Laminar
Turbulent
TwoDimensional
ThreeDimensional
```

---

## 159. Solver Interface

Aerodynamics Compute MUSS eine gemeinsame Solver-Abstraktion besitzen.

Konzeptionell:

```text
create_solver()

set_geometry()
set_fluid()
set_boundary_conditions()
set_operating_point()
set_options()

initialize()
solve()

query_status()
query_result()
query_diagnostics()

cancel()
destroy()
```

---

## 160. Solver Types

Mindestens folgende logische Solver-Klassen SOLLEN vorgesehen sein:

```text
ANALYTICAL
EMPIRICAL
PANEL
VORTEX_LATTICE
EULER
NAVIER_STOKES
RANS
CUSTOM
```

Nicht jede Implementierung MUSS alle Solver-Klassen bereitstellen.

---

## 161. Solver Selection

Die Runtime SOLL einen Solver anhand von Anforderungen auswählen können.

Kriterien können sein:

```text
Geometry
MachNumber
ReynoldsNumber
RequiredAccuracy
AvailableTime
AvailableHardware
RequiredOutputs
```

---

## 162. Explicit Solver Selection

Eine Anwendung MUSS einen bestimmten Solver explizit verlangen können.

---

## 163. Solver Accuracy

Solver SOLLEN ihre erwartete Modellgenauigkeit oder Modellklasse beschreiben können.

---

## 164. Solver Convergence

Iterative Solver MÜSSEN definierte Konvergenzkriterien besitzen.

Beispiele:

```text
ResidualTolerance
ForceConvergence
MomentConvergence
MaximumIterations
MaximumTime
```

---

## 165. Residual

Ein Solver SOLL Residuen bereitstellen können.

---

## 166. Convergence History

Eine vollständige Implementierung SOLL den Konvergenzverlauf bereitstellen können.

Beispiel:

```text
Iteration
Residual
Lift
Drag
Moment
```

---

## 167. Non-Convergence

Ein nicht konvergierter Solver Run DARF nicht als gültig konvergiertes Ergebnis gekennzeichnet werden.

---

## 168. Divergence

Numerische Divergenz MUSS erkannt werden können.

---

## 169. Solver Result Status

Mindestens:

```text
SUCCESS
CONVERGED_WITH_WARNINGS
NOT_CONVERGED
DIVERGED
CANCELLED
FAILED
```

---

## 170. Stationary Simulation

Aerodynamics Compute MUSS stationäre Betriebspunkte unterstützen können.

---

## 171. Transient Simulation

Eine erweiterte Implementierung SOLL zeitabhängige Strömungen unterstützen.

---

## 172. Time Step

Transiente Solver MÜSSEN einen definierten Zeitschritt besitzen.

```text
Δt
```

---

## 173. Fixed Time Step

Ein fester Zeitschritt DARF verwendet werden.

---

## 174. Adaptive Time Step

Adaptive Zeitschritte SOLLEN unterstützt werden können.

---

## 175. CFL Condition

Explizite Strömungssolver SOLLEN eine geeignete CFL-Bedingung berücksichtigen können.

---

## 176. Initial Conditions

Transiente Solver MÜSSEN Anfangsbedingungen besitzen.

---

## 177. Previous Solution Initialization

Eine vorherige Lösung SOLL als Initialisierung eines ähnlichen Betriebspunkts verwendet werden können.

---

## 178. Warm Start

Parameterstudien SOLLEN Warm Starts verwenden können.

---

## 179. Parameter Sweep

Aerodynamics Compute SOLL Parameterstudien unterstützen.

Beispiele:

```text
AngleOfAttack Sweep
Mach Sweep
Reynolds Sweep
Sideslip Sweep
RideHeight Sweep
RPM Sweep
```

---

## 180. Angle-of-Attack Sweep

Beispiel:

```text
α =
-10°
-8°
-6°
...
20°
```

---

## 181. Polar Generation

Eine Serie von Betriebspunkten SOLL zu einer aerodynamischen Polare zusammengefasst werden können.

---

## 182. Polar Data

Eine Polare DARF enthalten:

```text
α
C_L
C_D
C_m
L/D
Transition
Separation
```

---

## 183. Multidimensional Lookup

Aerodynamische Daten SOLLEN als mehrdimensionale Kennfelder repräsentierbar sein.

Beispiel:

```text
C_L =
f(
    α,
    β,
    Mach,
    Reynolds
)
```

---

## 184. Interpolation

Kennfelder MÜSSEN eine definierte Interpolationsmethode besitzen.

---

## 185. Extrapolation

Extrapolation außerhalb eines gültigen Kennfeldes MUSS:

```text
FORBIDDEN
WARN
ALLOWED
```

konfigurierbar sein.

---

## 186. Validity Domain

Jedes empirische oder tabellarische Modell SOLL seinen gültigen Bereich angeben können.

Beispiel:

```text
Mach:
0.0 .. 0.8

Reynolds:
1e5 .. 5e6

Alpha:
-10° .. 15°
```

---

## 187. Out-of-Domain Detection

Die Runtime MUSS erkennen können, wenn ein Modell außerhalb seines definierten Gültigkeitsbereichs verwendet wird.

---

## 188. Optimization

Aerodynamics Compute SOLL mit Optimierungsalgorithmen kombinierbar sein.

---

## 189. Optimization Variables

Mögliche Optimierungsvariablen:

```text
Airfoil Shape
Wing Span
Wing Twist
Sweep
Vehicle Shape
Ride Height
Diffuser Angle
Spoiler Angle
Propeller Pitch
Rotor RPM
```

---

## 190. Objective Functions

Beispiele:

```text
Minimize Drag
Maximize Lift
Maximize L/D
Maximize Downforce
Minimize Power
Minimize Noise Proxy
Minimize Mass subject to Aerodynamic Constraints
```

---

## 191. Constraints

Beispiele:

```text
MinimumLift
MaximumDrag
MaximumMoment
MaximumPressure
GeometryConstraints
StabilityConstraints
```

---

## 192. Multi-Objective Optimization

Mehrere Ziele DÜRFEN gleichzeitig optimiert werden.

Beispiel:

```text
Minimize Drag
+
Maximize Downforce
```

---

## 193. Sensitivity Analysis

Aerodynamics Compute SOLL Sensitivitätsanalysen unterstützen können.

Beispiel:

```text
∂C_D / ∂α
```

oder:

```text
∂C_L / ∂ShapeParameter
```

---

## 194. Gradient

Solver DÜRFEN Gradienten analytisch, numerisch oder über automatische Differentiation bereitstellen.

---

## 195. Uncertainty Analysis

Unsichere Eingabeparameter DÜRFEN modelliert werden.

Beispiele:

```text
WindSpeed
WindDirection
SurfaceRoughness
Temperature
ManufacturingTolerance
```

---

## 196. Monte Carlo

Monte-Carlo-Studien DÜRFEN über Aerodynamics Compute ausgeführt werden.

---

## 197. Reduced-Order Models

Aerodynamics Compute SOLL Reduced-Order Models integrieren können.

---

## 198. Surrogate Models

Ersatzmodelle DÜRFEN für schnelle Näherungsberechnungen verwendet werden.

---

## 199. AI Surrogate

AI-basierte Surrogate Models DÜRFEN über die NPU Runtime ausgeführt werden.

---

## 200. Physical Model Authority

Ein AI-Modell DARF nicht automatisch als physikalisch exakt behandelt werden.

Die Herkunft und Genauigkeit eines Surrogate Models MUSS nachvollziehbar sein.

---

## 201. Digital Twin

Aerodynamics Compute SOLL als Rechenkern aerodynamischer Digital Twins verwendet werden können.

---

## 202. Sensor Integration

Messwerte DÜRFEN eingebunden werden.

Beispiele:

```text
AirSpeed
Pressure
Temperature
AngleOfAttack
Yaw
Force
Moment
RPM
```

---

## 203. Model Calibration

Aerodynamische Modelle SOLLEN anhand realer Messdaten kalibrierbar sein.

---

## 204. Residual Analysis

Die Differenz zwischen Messung und Modell lautet:

```text
Residual =
Measured
-
Predicted
```

---

## 205. State Estimation

Eine erweiterte Implementierung DARF nicht direkt gemessene Strömungszustände schätzen.

---

## 206. Wind Tunnel Data

Windkanaldaten SOLLEN als Referenz- oder Kalibrierungsdaten importierbar sein.

---

## 207. Flight Test Data

Flug- oder Fahrversuchsdaten DÜRFEN zur Modellvalidierung verwendet werden.

---

## 208. Experimental Metadata

Messdaten SOLLEN Metadaten enthalten können:

```text
TestID
Date
Facility
SensorCalibration
Atmosphere
GeometryVersion
Uncertainty
```

---

## 209. Validation

Aerodynamische Modelle SOLLEN gegen Referenzdaten validiert werden können.

---

## 210. Validation State

Ein Modell DARF folgende Zustände besitzen:

```text
UNVALIDATED
PARTIALLY_VALIDATED
VALIDATED
EXPERIMENTAL
```

---

## 211. Model Provenance

Die Herkunft eines Modells SOLL nachvollziehbar sein.

Beispiele:

```text
Analytical
Empirical
Experimental
CFD-derived
AI-derived
Hybrid
```

---

## 212. Hybrid Models

Aerodynamics Compute DARF mehrere Modellarten kombinieren.

Beispiel:

```text
Analytical Model
+
Empirical Correction
+
CFD Lookup Table
```

---

## 213. Model Metadata

Ein Modell SOLL mindestens folgende Metadaten besitzen können:

```text
ModelName
ModelVersion
ModelType
Author
Created
Modified
ValidityDomain
RequiredInputs
ProvidedOutputs
SolverRequirements
ValidationState
```

---

## 214. Model Versioning

Aerodynamische Modelle MÜSSEN versionierbar sein.

---

## 215. Serialization

Aerodynamische Modelle und Ergebnisse SOLLEN serialisierbar sein.

---

## 216. Portable Model

Serialisierte Modelle SOLLEN möglichst hardwareunabhängig sein.

---

## 217. Result Serialization

Ergebnisse SOLLEN inklusive relevanter Metadaten gespeichert werden können.

---

## 218. Reproducibility Metadata

Ein gespeichertes Ergebnis SOLL mindestens referenzieren können:

```text
ModelVersion
GeometryVersion
Solver
SolverVersion
RuntimeVersion
OperatingPoint
SolverOptions
PrecisionPolicy
```

---

## 219. Deterministic Mode

Aerodynamics Compute SOLL einen deterministischen Modus unterstützen.

Gleiche:

```text
Inputs
Geometry
Model
Solver
SolverOptions
RuntimeVersion
HardwareSemantics
```

SOLLEN innerhalb der definierten numerischen Regeln reproduzierbare Ergebnisse liefern.

---

## 220. CPU Execution

Eine konforme Implementierung MUSS CPU-Ausführung unterstützen.

---

## 221. SIMD

Geeignete numerische Operationen SOLLEN SIMD verwenden können.

---

## 222. Multi-Core

Aerodynamische Solver SOLLEN mehrere CPU-Kerne nutzen können.

---

## 223. GPU Acceleration

Geeignete Workloads SOLLEN über `NPSPEC-COMPUTE-RUNTIME-0005` auf GPUs ausgeführt werden können.

---

## 224. GPU Candidates

Geeignete GPU-Workloads sind insbesondere:

```text
Mesh Operations
Field Operations
Sparse Linear Algebra
Finite Volume Operations
Particle Operations
Large Parameter Sweeps
Batch Airfoil Solves
CFD
```

---

## 225. NPU Acceleration

NPUs DÜRFEN für AI-basierte aerodynamische Modelle verwendet werden.

Beispiele:

```text
Surrogate CFD
Flow Field Prediction
Drag Prediction
Lift Prediction
Anomaly Detection
Reduced-Order AI Models
```

---

## 226. Distributed Compute

Sehr große aerodynamische Workloads DÜRFEN über `NPSPEC-COMPUTE-RUNTIME-0007` verteilt werden.

---

## 227. Distributed Candidates

Beispiele:

```text
Large CFD
Parameter Sweeps
Optimization
Monte Carlo
Multiple Operating Points
Design Space Exploration
```

---

## 228. Domain Decomposition

Große CFD-Domänen DÜRFEN über mehrere Compute Devices oder Nodes partitioniert werden.

---

## 229. Halo Exchange

Bei räumlicher Domain Decomposition SOLLEN benachbarte Partitionen erforderliche Grenzdaten austauschen können.

---

## 230. Communication Cost

Distributed Solver MÜSSEN Kommunikationskosten berücksichtigen.

---

## 231. Load Balancing

Partitionen SOLLEN möglichst gleichmäßig auf verfügbare Compute-Ressourcen verteilt werden.

---

## 232. Heterogeneous Compute

Aerodynamics Compute SOLL heterogene Ausführung ermöglichen.

Beispiel:

```text
Geometry Processing -> CPU

Flow Solver -> GPU

AI Surrogate -> NPU

Optimization Cases -> Distributed Compute
```

---

## 233. Compute Graph

Eine komplexe aerodynamische Analyse DARF als Compute Graph dargestellt werden.

Beispiel:

```text
Geometry
   |
   v
Mesh
   |
   v
Flow Solver
   |
   +--> Pressure Field
   |
   +--> Velocity Field
   |
   v
Force Integration
   |
   v
Aerodynamic Coefficients
   |
   v
Optimization
```

---

## 234. Memory Model

Große aerodynamische Datenstrukturen MÜSSEN in das allgemeine Compute Memory Model integrierbar sein.

Beispiele:

```text
Meshes
Pressure Fields
Velocity Fields
Density Fields
Temperature Fields
Residual Fields
Sparse Matrices
Particle Sets
```

---

## 235. Field Object

Ein aerodynamisches Field Object SOLL räumlich verteilte Werte repräsentieren.

Beispiele:

```text
ScalarField
VectorField
TensorField
```

---

## 236. Scalar Field

Beispiele:

```text
Pressure
Density
Temperature
MachNumber
```

---

## 237. Vector Field

Beispiele:

```text
Velocity
Momentum
Vorticity
```

---

## 238. Field Location

Ein Feld MUSS angeben, wo Werte definiert sind.

Beispiele:

```text
CELL_CENTER
FACE_CENTER
VERTEX
SURFACE
```

---

## 239. Field Units

Jedes Feld MUSS seine physikalische Dimension eindeutig definieren.

---

## 240. Field Layout

Die Runtime SOLL unterschiedliche Memory Layouts unterstützen können.

---

## 241. Zero-Copy

Geeignete Solver-Pipelines SOLLEN unnötige Kopien großer Strömungsfelder vermeiden.

---

## 242. Streaming Results

Große transiente Simulationen SOLLEN Ergebnisse streamen können, ohne sämtliche Zeitschritte gleichzeitig im Speicher halten zu müssen.

---

## 243. Checkpointing

Lange Simulationen SOLLEN Checkpoints unterstützen.

---

## 244. Checkpoint Content

Ein CFD-Checkpoint DARF enthalten:

```text
FlowFields
SolverState
Time
Iteration
MeshReference
BoundaryConditions
ModelState
```

---

## 245. Restart

Ein kompatibler Solver SOLL eine Simulation aus einem gültigen Checkpoint fortsetzen können.

---

## 246. Checkpoint Validation

Vor einem Restart MUSS geprüft werden, ob Checkpoint, Geometrie, Solver und Modell kompatibel sind.

---

## 247. Cancellation

Lange Berechnungen MÜSSEN kontrolliert abbrechbar sein.

---

## 248. Progress

Solver SOLLEN Fortschrittsinformationen bereitstellen.

Beispiele:

```text
Iteration
Residual
ElapsedTime
EstimatedProgress
CurrentTimeStep
```

---

## 249. Diagnostics

Aerodynamics Compute MUSS Diagnoseinformationen bereitstellen.

---

## 250. Warning Classes

Mindestens:

```text
OUTSIDE_MODEL_VALIDITY
HIGH_MACH_FOR_INCOMPRESSIBLE_MODEL
STALL_POSSIBLE
FLOW_SEPARATION
SHOCK_PRESENT
POOR_MESH_QUALITY
SOLVER_SLOW_CONVERGENCE
RESULT_NOT_CONVERGED
EXTRAPOLATION_USED
```

---

## 251. Warning Severity

Warnungen SOLLEN klassifiziert werden:

```text
INFO
WARNING
CRITICAL
```

---

## 252. Error Model

Mindestens folgende Fehler SOLLEN unterstützt werden:

```text
INVALID_UNIT
INVALID_PARAMETER
INVALID_GEOMETRY
INVALID_MESH
INVALID_BOUNDARY_CONDITION
INVALID_FLUID
UNSUPPORTED_FLOW_REGIME
UNSUPPORTED_MODEL
SOLVER_DIVERGED
SOLVER_NOT_CONVERGED
NUMERIC_OVERFLOW
OUT_OF_MEMORY
CANCELLED
```

---

## 253. NaN Handling

NaN-Werte DÜRFEN nicht unkontrolliert als gültige aerodynamische Ergebnisse propagiert werden.

---

## 254. Infinity Handling

Unendliche numerische Werte MÜSSEN erkannt werden.

---

## 255. Invalid Physical Values

Mindestens folgende Werte MÜSSEN validiert werden:

```text
Negative Absolute Pressure
Negative Absolute Temperature
Negative Density
Negative Dynamic Viscosity
Negative Reference Area
Invalid Mach Number Representation
```

---

## 256. Zero Reference Area

Eine Referenzfläche von:

```text
0
```

DARF nicht für Koeffizientenberechnungen verwendet werden.

---

## 257. Zero Density

Eine Dichte von null MUSS entsprechend dem verwendeten Modell behandelt werden und DARF nicht unkontrolliert zu Divisionen durch null führen.

---

## 258. Model Applicability

Vor der Berechnung SOLL geprüft werden, ob das gewählte Modell zum Betriebspunkt passt.

---

## 259. Automatic Model Warning

Beispiel:

```text
Solver:
INCOMPRESSIBLE

Mach:
0.85
```

MUSS mindestens eine Warnung oder Ablehnung gemäß Solver Policy erzeugen.

---

## 260. Safety Boundary

Aerodynamics Compute liefert technische Berechnungsergebnisse.

Die Runtime DARF daraus nicht automatisch eine sicherheitszertifizierte Freigabe für:

```text
Aircraft
Vehicles
Structures
Machinery
```

ableiten.

---

## 261. Profiling

Aerodynamics Compute SOLL Profiling unterstützen.

Mindestens:

```text
GeometryProcessingTime
MeshProcessingTime
InitializationTime
SolverTime
PostProcessingTime
MemoryUsage
```

---

## 262. Solver Metrics

Mindestens:

```text
Iterations
Residual
ConvergenceRate
CellsPerSecond
IterationsPerSecond
ExecutionTime
```

soweit für den Solver sinnvoll.

---

## 263. GPU Metrics

Bei GPU-Ausführung SOLLEN zusätzlich verfügbar sein:

```text
KernelTime
TransferTime
DeviceMemoryUsage
GPUUtilization
```

---

## 264. Distributed Metrics

Bei Distributed Compute SOLLEN zusätzlich verfügbar sein:

```text
NodeCount
CommunicationTime
TransferredBytes
LoadBalance
SynchronizationTime
```

---

## 265. Trace Events

Mindestens folgende Trace Events SOLLEN unterstützt werden:

```text
AERO_MODEL_LOAD
AERO_MODEL_VALIDATE

AERO_GEOMETRY_LOAD
AERO_GEOMETRY_VALIDATE

AERO_SOLVER_CREATE
AERO_SOLVE_START
AERO_ITERATION
AERO_TIMESTEP
AERO_SOLVE_COMPLETE
AERO_SOLVE_FAILED

AERO_WARNING
AERO_CHECKPOINT
```

---

## 266. Privacy

Diagnosedaten SOLLEN keine unnötigen vertraulichen Geometrie- oder Modelldaten enthalten.

---

## 267. Custom Models

Anwendungen und Drittanbieter DÜRFEN eigene aerodynamische Modelle bereitstellen.

---

## 268. Custom Solver

Ein benutzerdefinierter Solver MUSS über eine definierte Solver-Schnittstelle integriert werden.

---

## 269. Custom Model Isolation

Fehlerhafte benutzerdefinierte Modelle DÜRFEN die Compute Runtime nicht kompromittieren.

---

## 270. Model Namespace

Modelle SOLLEN eindeutig benannt werden.

Beispiele:

```text
Nova.Aerodynamics.Models.Incompressible
Nova.Aerodynamics.Models.Compressible
Nova.Aerodynamics.Models.Airfoil
Nova.Aerodynamics.Models.Wing
```

---

## 271. API Namespace

Eine mögliche logische API-Struktur lautet:

```text
Nova.Aerodynamics
Nova.Aerodynamics.Units
Nova.Aerodynamics.Gases
Nova.Aerodynamics.Atmosphere
Nova.Aerodynamics.Geometry
Nova.Aerodynamics.Airfoils
Nova.Aerodynamics.Wings
Nova.Aerodynamics.Rotors
Nova.Aerodynamics.Flow
Nova.Aerodynamics.Fields
Nova.Aerodynamics.Solvers
Nova.Aerodynamics.Diagnostics
Nova.Aerodynamics.Optimization
```

Die konkrete Sprachbindung wird separat definiert.

---

## 272. Component Reuse

Gemeinsame mathematische und physikalische Primitive SOLLEN aus vorhandenen NovaOS-Systembibliotheken verwendet werden, sofern geeignete Implementierungen existieren.

---

## 273. Testing — Units

Mindestens zu testen:

```text
m/s
km/h
knots
mph

Pa
hPa
bar
psi

K
°C

kg/m³
```

---

## 274. Testing — Unit Conversion

Mindestens:

```text
km/h -> m/s
knots -> m/s
psi -> Pa
hPa -> Pa
°C -> K
```

---

## 275. Testing — Ideal Gas

Zu testen sind bekannte Kombinationen aus:

```text
Pressure
Density
Temperature
GasConstant
```

---

## 276. Testing — Speed of Sound

Die Schallgeschwindigkeit MUSS für bekannte Referenzbedingungen geprüft werden.

---

## 277. Testing — Mach Number

Mindestens:

```text
M = 0
Subsonic
Transonic
Supersonic
Hypersonic
```

---

## 278. Testing — Reynolds Number

Zu testen sind unterschiedliche:

```text
Velocity
Density
Length
Viscosity
```

---

## 279. Testing — Dynamic Pressure

Mindestens:

```text
Zero Velocity
Low Velocity
High Velocity
Different Densities
```

---

## 280. Testing — Lift

Zu testen:

```text
C_L = 0
Positive C_L
Negative C_L
Different Reference Areas
Different Dynamic Pressures
```

---

## 281. Testing — Drag

Zu testen:

```text
C_D = 0
Positive C_D
Different Areas
Different Velocities
```

---

## 282. Testing — Moment

Momentenberechnungen MÜSSEN mit unterschiedlichen Referenzlängen geprüft werden.

---

## 283. Testing — Pressure Coefficient

Bekannte Referenzfälle SOLLEN verwendet werden.

---

## 284. Testing — Airfoil

Mindestens:

```text
Symmetric Airfoil
Cambered Airfoil
Zero Angle of Attack
Positive Angle of Attack
Negative Angle of Attack
Near Stall
```

---

## 285. Testing — Wing

Mindestens:

```text
Rectangular Wing
Tapered Wing
Swept Wing
Twisted Wing
```

---

## 286. Testing — Induced Drag

Bekannte analytische oder validierte Referenzfälle SOLLEN verwendet werden.

---

## 287. Testing — Atmosphere

Atmosphärenmodelle SOLLEN für mehrere Höhen geprüft werden.

---

## 288. Testing — Compressibility

Mindestens:

```text
Low Mach
Moderate Subsonic
Near Transonic
Supersonic
```

---

## 289. Testing — Invalid Model Selection

Ein inkompressibles Modell bei offensichtlich ungeeignetem Mach-Bereich MUSS entsprechend seiner Policy warnen oder ablehnen.

---

## 290. Testing — Geometry

Mindestens:

```text
Valid Geometry
Empty Geometry
Degenerate Geometry
Invalid Coordinates
NaN Coordinates
```

---

## 291. Testing — Mesh

Mindestens:

```text
Valid Mesh
Degenerate Faces
Invalid Connectivity
Zero-Volume Cells
Poor Quality Cells
```

---

## 292. Testing — Boundary Conditions

Zu testen:

```text
Far Field
Inlet
Outlet
Slip Wall
No-Slip Wall
Symmetry
Moving Wall
```

soweit vom Solver unterstützt.

---

## 293. Testing — Solver

Mindestens:

```text
Immediate Convergence
Normal Convergence
Slow Convergence
Maximum Iterations
Divergence
Cancellation
```

---

## 294. Testing — Parameter Sweep

Große Serien von Betriebspunkten MÜSSEN getestet werden.

---

## 295. Testing — Polar

Polaren MÜSSEN hinsichtlich:

```text
Ordering
Interpolation
ValidityDomain
Extrapolation
```

getestet werden.

---

## 296. Testing — Determinism

Deterministische Berechnungen MÜSSEN innerhalb der definierten numerischen Regeln reproduzierbar sein.

---

## 297. Testing — CPU

Alle Mindestfunktionen MÜSSEN auf CPU getestet werden.

---

## 298. Testing — GPU

GPU-Backends MÜSSEN gegen CPU-Referenzergebnisse innerhalb definierter Toleranzen geprüft werden.

---

## 299. Testing — NPU

AI-basierte NPU-Modelle MÜSSEN gegen ihre definierten Referenzmodelle oder Validierungsdatensätze geprüft werden.

---

## 300. Testing — Distributed Compute

Distributed Solver Runs MÜSSEN gegen äquivalente nicht verteilte Referenzfälle geprüft werden, sofern technisch möglich.

---

## 301. Testing — Checkpoint

Mindestens:

```text
Create Checkpoint
Restart
Corrupted Checkpoint
Incompatible Solver
Incompatible Geometry
```

---

## 302. Testing — Extreme Values

Mindestens:

```text
Very Low Velocity
Very High Velocity
Very Low Density
High Density
Low Temperature
High Temperature
Large Reynolds Number
Large Mach Number
```

---

## 303. Testing — Invalid Values

Mindestens:

```text
NaN
Infinity
Negative Density
Negative Absolute Temperature
Negative Reference Area
Zero Reference Area
```

---

## 304. Testing — Conservation

Numerische Solver SOLLEN je nach Modell auf:

```text
Mass Conservation
Momentum Conservation
Energy Conservation
```

geprüft werden.

---

## 305. Testing — Reference Cases

Die Testsuite SOLL analytisch, experimentell oder numerisch verifizierte Referenzfälle enthalten.

---

## 306. Testing — Experimental Validation

Wo geeignete offene oder interne Referenzdaten vorhanden sind, SOLLEN Solver gegen Messdaten validiert werden.

---

## 307. Testing — Performance

Mindestens folgende Werte SOLLEN gemessen werden:

```text
SolveTime
IterationsPerSecond
CellsPerSecond
MemoryUsage
TransferTime
```

---

## 308. Testing — Scalability

CFD- und Batch-Workloads SOLLEN mit steigender Problemgröße getestet werden.

Beispiele:

```text
10,000 Cells
100,000 Cells
1,000,000 Cells
10,000,000 Cells
100,000,000+ Cells
```

soweit die Zielimplementierung diese Größenordnungen unterstützt.

---

## 309. Stress Tests

Stress Tests SOLLEN umfassen:

```text
Large Meshes
Long Simulations
Repeated Solves
Large Parameter Sweeps
Rapid Cancellation
Memory Pressure
GPU Memory Pressure
Distributed Node Failure
```

---

## 310. Fuzzing

Mindestens folgende Inputs SOLLTEN fuzz-getestet werden:

```text
Geometry Files
Mesh Files
Model Descriptors
Solver Options
Boundary Conditions
Field Metadata
Atmosphere Data
Airfoil Data
```

---

## 311. Fault Injection

Mindestens:

```text
OUT_OF_MEMORY
GPU_FAILURE
NODE_FAILURE
NUMERIC_FAILURE
CHECKPOINT_FAILURE
INVALID_FIELD
COMMUNICATION_FAILURE
```

---

## 312. Numerical Regression

Solver-Änderungen MÜSSEN gegen definierte numerische Referenzfälle geprüft werden.

---

## 313. Performance Regression

Änderungen SOLLEN hinsichtlich:

```text
Runtime
MemoryUsage
ConvergenceRate
GPUUtilization
CommunicationCost
```

verglichen werden.

---

## 314. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
SI Unit System
Unit Conversion

Ideal Gas
Air Properties
Speed of Sound

Mach Number
Reynolds Number
Dynamic Pressure

Lift
Drag
Aerodynamic Moments

Aerodynamic Coefficients
Reference Geometry

Basic Atmosphere Model

Basic Airfoil Representation
Basic Wing Representation

Basic Incompressible Models

Model Validation
Diagnostics

CPU Execution
```

---

## 315. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Temperature-dependent Air Properties
Humidity
Compressible Flow
Transonic Models
Supersonic Models
Shock Waves
Expansion Waves

Boundary Layers
Transition
Separation
Wake Models

Airfoil Solvers
Lifting-Line Theory
Vortex Lattice
Panel Methods

Propellers
Rotors
Turbomachinery

Surface Meshes
Volume Meshes

Euler Solver
Navier-Stokes Solver
RANS
LES

Transient Simulation
Moving Geometry
Rotating Reference Frames

Optimization
Sensitivity Analysis
Uncertainty Analysis

Digital Twins
Model Calibration

GPU Acceleration
NPU Surrogate Models
Distributed CFD

Checkpointing
Profiling
Advanced Diagnostics
```

---

## 316. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-AERODYNAMICS-0001` MUSS:

1. aerodynamische physikalische Größen eindeutig repräsentieren.
2. SI-Einheiten intern unterstützen.
3. Einheiten sicher konvertieren.
4. inkompatible Dimensionen erkennen.
5. Geschwindigkeitsvektoren repräsentieren.
6. Gaszustände repräsentieren.
7. das ideale Gasgesetz unterstützen.
8. die Schallgeschwindigkeit bestimmen können.
9. die Mach-Zahl berechnen.
10. die Reynolds-Zahl berechnen.
11. dynamischen Druck berechnen.
12. Referenzflächen und Referenzlängen definieren.
13. Auftrieb berechnen.
14. Widerstand berechnen.
15. aerodynamische Momente berechnen.
16. aerodynamische Koeffizienten repräsentieren.
17. Druckbeiwerte repräsentieren.
18. grundlegende Atmosphärenzustände bereitstellen.
19. grundlegende Profilgeometrien repräsentieren.
20. grundlegende Flügelgeometrien repräsentieren.
21. Modellgültigkeitsbereiche unterstützen.
22. ungeeignete Modellverwendung diagnostizieren.
23. numerisch ungültige Eingaben erkennen.
24. Solver-Ergebnisse eindeutig kennzeichnen.
25. CPU-Ausführung unterstützen.
26. das NovaOS Compute Memory Model integrieren können.
27. das NovaOS Compute Object Model integrieren können.
28. den NovaOS Compute Scheduler integrieren können.
29. zukünftige GPU-, NPU- und Distributed-Backends ermöglichen.
30. zukünftige CFD-Solver integrierbar halten.

---

## 317. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen können:

```text
NovaAerodynamicsCompute
 |
 +-- AerodynamicUnitSystem
 |
 +-- GasModelManager
 |
 +-- AtmosphereManager
 |
 +-- AerodynamicGeometryManager
 |
 +-- AirfoilManager
 |
 +-- WingManager
 |
 +-- RotorManager
 |
 +-- FlowModelManager
 |
 +-- FieldManager
 |
 +-- MeshManager
 |
 +-- AerodynamicSolverManager
 |
 +-- AerodynamicDiagnostics
 |
 +-- AerodynamicOptimization
 |
 +-- AerodynamicProfiler
```

Diese Aufteilung ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 318. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Aerodynamics Compute baut auf der allgemeinen NovaOS Compute Runtime auf.

```text
Aerodynamic Workload
        |
        v
Aerodynamics Compute
        |
        v
Compute Runtime
        |
        +--> CPU
        +--> GPU
        +--> NPU
        +--> Distributed Compute
```

Aerodynamics Compute definiert die physikalische und fachliche Semantik.

Die Compute Runtime übernimmt die technische Ausführung.

---

## 319. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Compute Scheduler DARF anhand des aerodynamischen Workloads das geeignete Compute Device auswählen.

Beispiel:

```text
Simple Coefficient Calculation
        |
        v
CPU

Airfoil Batch
        |
        v
Multi-Core CPU / GPU

Large CFD
        |
        v
GPU

Large Parameter Study
        |
        v
Distributed Compute
```

---

## 320. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Aerodynamische Datenstrukturen MÜSSEN auf das allgemeine Compute Memory Model abbildbar sein.

Beispiele:

```text
Geometry
Meshes
Pressure Fields
Velocity Fields
Density Fields
Temperature Fields
Solver Vectors
Sparse Matrices
```

---

## 321. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Aerodynamics Compute Objects SOLLEN auf dem allgemeinen Compute Object Model aufbauen.

Beispiele:

```text
Gas
Atmosphere
Airfoil
Wing
Rotor
Body
Mesh
FlowField
Solver
Simulation
Result
```

---

## 322. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

Massiv parallele aerodynamische Workloads DÜRFEN über die GPU Runtime ausgeführt werden.

Beispiel:

```text
Aerodynamics Compute
        |
        v
GPU Runtime
        |
        v
GPU
```

---

## 323. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

AI-basierte aerodynamische Modelle DÜRFEN die NPU Runtime verwenden.

Beispiele:

```text
Flow Prediction
Drag Prediction
Lift Prediction
Surrogate CFD
Anomaly Detection
```

Die physikalische Solver-Infrastruktur DARF dadurch nicht zwingend von AI abhängig werden.

---

## 324. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Große Simulationen und Parameterstudien DÜRFEN über Distributed Compute ausgeführt werden.

Beispiel:

```text
Design Space
     |
     v
Distributed Compute
  /    |     |    \
 v     v     v     v
NodeA NodeB NodeC NodeD
```

---

## 325. Beziehung zu `NPSPEC-COMPUTE-HYDRAULICS-0001`

Hydraulics Compute und Aerodynamics Compute teilen grundlegende Fluidmechanik-Konzepte.

Gemeinsame Konzepte umfassen:

```text
Pressure
Density
Velocity
Viscosity
Reynolds Number
Continuity
Momentum
Energy
Flow Resistance
```

Die fachlichen Modelle unterscheiden sich jedoch.

```text
Fluid Compute
     |
     +--> Hydraulics
     |
     +--> Aerodynamics
```

Langfristig DÜRFEN gemeinsame Primitive in eine allgemeine Fluid-Compute-Schicht ausgelagert werden.

---

## 326. Gemeinsame Fluid-Compute-Basis

Eine zukünftige Architektur DARF folgende Struktur verwenden:

```text
                 Nova Fluid Compute
                        |
          +-------------+-------------+
          |                           |
          v                           v
    Hydraulics Compute        Aerodynamics Compute
          |                           |
          v                           v
 Liquid-oriented Models       Gas-oriented Models
```

Eine solche gemeinsame Schicht DARF insbesondere enthalten:

```text
Units
Fluid Properties
Continuity
Momentum
Energy
Reynolds Number
Field Objects
Mesh Objects
Solver Infrastructure
```

---

## 327. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-AERODYNAMICS-UNITS-0001
Aerodynamic Units

NPSPEC-COMPUTE-AERODYNAMICS-GAS-0001
Gas Model

NPSPEC-COMPUTE-AERODYNAMICS-ATMOSPHERE-0001
Atmosphere Model

NPSPEC-COMPUTE-AERODYNAMICS-GEOMETRY-0001
Aerodynamic Geometry

NPSPEC-COMPUTE-AERODYNAMICS-AIRFOIL-0001
Airfoil Model

NPSPEC-COMPUTE-AERODYNAMICS-WING-0001
Wing Model

NPSPEC-COMPUTE-AERODYNAMICS-FORCE-0001
Aerodynamic Forces and Moments

NPSPEC-COMPUTE-AERODYNAMICS-BOUNDARY-0001
Boundary Layer

NPSPEC-COMPUTE-AERODYNAMICS-TRANSITION-0001
Boundary-Layer Transition

NPSPEC-COMPUTE-AERODYNAMICS-WAKE-0001
Wake Model

NPSPEC-COMPUTE-AERODYNAMICS-COMPRESSIBLE-0001
Compressible Flow

NPSPEC-COMPUTE-AERODYNAMICS-SHOCK-0001
Shock and Expansion Waves

NPSPEC-COMPUTE-AERODYNAMICS-POTENTIAL-0001
Potential Flow

NPSPEC-COMPUTE-AERODYNAMICS-PANEL-0001
Panel Method

NPSPEC-COMPUTE-AERODYNAMICS-VLM-0001
Vortex Lattice Method

NPSPEC-COMPUTE-AERODYNAMICS-ROTOR-0001
Rotor and Propeller Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-MESH-0001
Aerodynamic Mesh Model

NPSPEC-COMPUTE-AERODYNAMICS-FIELD-0001
Flow Field Model

NPSPEC-COMPUTE-AERODYNAMICS-CFD-0001
Computational Fluid Dynamics Runtime

NPSPEC-COMPUTE-AERODYNAMICS-EULER-0001
Euler Solver

NPSPEC-COMPUTE-AERODYNAMICS-NAVIERSTOKES-0001
Navier-Stokes Solver

NPSPEC-COMPUTE-AERODYNAMICS-TURBULENCE-0001
Turbulence Models

NPSPEC-COMPUTE-AERODYNAMICS-TRANSIENT-0001
Transient Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-OPTIMIZATION-0001
Aerodynamic Optimization

NPSPEC-COMPUTE-AERODYNAMICS-DIGITALTWIN-0001
Aerodynamic Digital Twin

NPSPEC-COMPUTE-AERODYNAMICS-DIAG-0001
Aerodynamic Diagnostics

NPSPEC-COMPUTE-AERODYNAMICS-TEST-0001
Aerodynamics Compute Conformance Tests
```

---

## 328. Zusammenfassung

Aerodynamics Compute stellt NovaOS eine gemeinsame Rechenplattform für aerodynamische und gasdynamische Aufgaben bereit.

Die grundlegende Verarbeitung lautet:

```text
Geometry
    |
    v
Operating Conditions
    |
    v
Gas / Atmosphere
    |
    v
Select Physical Model
    |
    v
Select Solver
    |
    v
Compute Flow
    |
    v
Pressure / Velocity / Density
    |
    v
Integrate Forces
    |
    v
Lift / Drag / Moments
    |
    v
Diagnostics
    |
    v
Result
```

Grundlegende aerodynamische Beziehungen umfassen:

```text
p =
ρ * R * T
```

```text
a =
sqrt(γ * R * T)
```

```text
M =
V / a
```

```text
Re =
ρ * V * L / μ
```

```text
q =
1/2 * ρ * V²
```

```text
L =
C_L * q * S
```

```text
D =
C_D * q * S
```

Aerodynamics Compute muss dabei unterschiedliche Modellkomplexitäten ermöglichen.

```text
                    Aerodynamics Compute
                            |
       +--------------------+--------------------+
       |                    |                    |
       v                    v                    v
 Analytical Models     Engineering Models      CFD
       |                    |                    |
       v                    v                    v
   Very Fast              Fast              High Detail
```

Eine Anwendung kann dadurch für eine Aufgabe eine einfache schnelle Näherung verwenden und für eine andere Aufgabe einen hochauflösenden numerischen Solver.

Die Compute-Hardware bleibt von der aerodynamischen Fachsemantik getrennt:

```text
                    Aerodynamic Model
                           |
                           v
                  Aerodynamics Compute
                           |
          +----------------+----------------+
          |                |                |
          v                v                v
         CPU              GPU          Distributed
                                             |
                                             v
                                       Multiple Nodes
```

AI-basierte Ersatzmodelle können zusätzlich NPUs verwenden:

```text
                Aerodynamics Compute
                       |
          +------------+------------+
          |                         |
          v                         v
   Physical Solver            AI Surrogate
          |                         |
          v                         v
       CPU/GPU                     NPU
          |                         |
          +------------+------------+
                       |
                       v
                Validated Result
```

Damit kann NovaOS langfristig gemeinsame Fähigkeiten für unter anderem folgende Aufgaben bereitstellen:

```text
Airfoil Analysis
Wing Analysis
Aircraft Aerodynamics
Drone Aerodynamics
Vehicle Aerodynamics
Railway Aerodynamics
Crosswind Analysis
Propeller Analysis
Rotor Analysis
Pressure Distribution
Drag Estimation
Lift Estimation
CFD
Design Optimization
Digital Twins
Aerodynamic AI Models
```

Das grundlegende Architekturprinzip lautet:

> Aerodynamics Compute behandelt aerodynamische Physik als wiederverwendbare systemweite Compute-Fähigkeit. Anwendungen beschreiben Geometrie, Gas, Atmosphäre, Randbedingungen und Betriebspunkt; Aerodynamics Compute übernimmt physikalische Modelle, Solver, Validierung, Diagnose und die effiziente Ausführung über CPU, GPU, NPU oder verteilte Compute-Ressourcen.