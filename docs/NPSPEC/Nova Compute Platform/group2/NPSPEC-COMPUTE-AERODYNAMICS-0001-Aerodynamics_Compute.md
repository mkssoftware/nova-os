# NPSPEC-COMPUTE-AERODYNAMICS-0001 — Aerodynamics Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Engineering / Fluid Mechanics / Aerodynamics  
**Komponente:** NovaOS Aerodynamics Compute  
**Bezeichner:** `NPSPEC-COMPUTE-AERODYNAMICS-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-TURBULENCE-0001`, `NPSPEC-COMPUTE-MULTIPHASE-0001`, `NPSPEC-COMPUTE-GASDYNAMICS-0001`  
**Kurzname:** Aerodynamics Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Aerodynamik-Compute-Schicht der NovaOS Compute-Architektur.

Aerodynamics Compute stellt physikalische Modelle, Datenstrukturen, numerische Primitive, Solver-Schnittstellen und Auswertungsverfahren für die Berechnung von Gasströmungen um und durch Körper bereit.

Typische Einsatzgebiete sind:

```text
Aircraft
Airfoils
Wings
Rotorcraft
Propellers
Fans
Turbomachinery
Automotive Aerodynamics
Trains
Buildings
Wind Engineering
Wind Turbines
Drones
Projectiles
External Aerodynamics
Internal Aerodynamics
Ventilation
Cooling
Aeroacoustics
Hypersonics
Atmospheric Flow
CFD
Digital Twins
Optimization
```

Aerodynamics Compute ist kein einzelner CFD-Solver.

Die Komponente definiert eine gemeinsame aerodynamische Modellschicht, die unterschiedliche Solver, Diskretisierungsverfahren und Hardware-Backends verwenden kann.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Aerodynamische Physik wird unabhängig von Geometrieeditor, Mesh-Generator, Solverimplementierung, Diskretisierung und Hardware-Backend beschrieben.

Eine Anwendung SOLL beispielsweise definieren können:

```text
Fluid = Air

Velocity = 70 m/s
Pressure = 101325 Pa
Temperature = 288.15 K

AngleOfAttack = 5 deg
```

und anschließend Größen wie:

```text
MachNumber
ReynoldsNumber
DynamicPressure
Lift
Drag
Moment
PressureCoefficient
LiftCoefficient
DragCoefficient
```

über gemeinsame Aerodynamics-Compute-Schnittstellen bestimmen können.

---

## 3. Ziele

Aerodynamics Compute MUSS folgende Bereiche architektonisch unterstützen:

1. aerodynamische Zustände,
2. Gas- und Atmosphäreneigenschaften,
3. inkompressible Strömung,
4. kompressible Strömung,
5. subsonische Strömung,
6. transsonische Strömung,
7. supersonische Strömung,
8. hypersonische Strömung,
9. laminare Strömung,
10. turbulente Strömung,
11. Übergangsströmung,
12. externe Aerodynamik,
13. interne Aerodynamik,
14. Tragflächen,
15. Profile,
16. Auftrieb,
17. Widerstand,
18. aerodynamische Momente,
19. Druckverteilungen,
20. Grenzschichten,
21. Ablösung,
22. Nachlauf,
23. Wirbel,
24. Stoßwellen,
25. Expansionswellen,
26. Propeller,
27. Rotoren,
28. Windturbinen,
29. instationäre Aerodynamik,
30. bewegte Geometrien,
31. Aeroakustik-Kopplung,
32. Wärmeübertragung,
33. Turbulenzintegration,
34. Mehrphasenintegration,
35. Gasdynamikintegration,
36. CFD-Integration,
37. CPU-Ausführung,
38. GPU-Beschleunigung,
39. HPC,
40. Distributed Compute,
41. Optimierung,
42. Validierung,
43. Diagnose,
44. Checkpointing,
45. reproduzierbare Berechnung.

---

## 4. Nicht-Ziele

Diese Basisspezifikation definiert nicht vollständig:

- einen universellen CFD-Solver,
- vollständige Verbrennungschemie,
- vollständige Strukturmechanik,
- vollständige Aeroelastik,
- vollständige Aeroakustik,
- vollständige Plasmaphysik,
- vollständige Atmosphärenphysik,
- vollständige Hyperschallchemie,
- konkrete CAD-Systeme,
- konkrete Mesh-Generatoren,
- konkrete Benutzeroberflächen.

Diese Systeme DÜRFEN Aerodynamics Compute verwenden.

---

## 5. Aerodynamic State

Ein aerodynamischer Zustand SOLL mindestens enthalten können:

```text
Pressure
Temperature
Density
Velocity
SpeedOfSound
MachNumber
DynamicPressure
ReynoldsNumber
Viscosity
```

---

## 6. Fluid

Aerodynamics Compute MUSS mindestens gasförmige Fluide repräsentieren können.

Typisches Fluid:

```text
Air
```

Weitere Gase DÜRFEN verwendet werden.

---

## 7. Gas Properties

Ein Gasmodell SOLL mindestens bereitstellen können:

```text
Density
DynamicViscosity
SpecificHeatCp
SpecificHeatCv
GasConstant
HeatCapacityRatio
ThermalConductivity
SpeedOfSound
```

---

## 8. Density

Die Dichte wird bezeichnet als:

```text
rho
```

Einheit:

```text
kg/m^3
```

---

## 9. Dynamic Viscosity

Die dynamische Viskosität wird bezeichnet als:

```text
mu
```

Einheit:

```text
Pa*s
```

---

## 10. Kinematic Viscosity

Die kinematische Viskosität lautet:

```text
nu =
mu / rho
```

---

## 11. Ideal Gas

Für geeignete Gaszustände DARF die ideale Gasgleichung verwendet werden:

```text
p =
rho * R * T
```

---

## 12. Heat Capacity Ratio

Das Verhältnis der Wärmekapazitäten lautet:

```text
gamma =
cp / cv
```

---

## 13. Speed of Sound

Für ein ideales Gas:

```text
a =
sqrt(gamma * R * T)
```

---

## 14. Mach Number

Die Mach-Zahl lautet:

```text
M =
|U| / a
```

---

## 15. Mach Regimes

Aerodynamics Compute SOLL Strömungsbereiche klassifizieren können.

Typische Klassifikation:

```text
INCOMPRESSIBLE_APPROXIMATION
SUBSONIC
TRANSONIC
SUPERSONIC
HYPERSONIC
```

---

## 16. Typical Mach Classification

Als allgemeine Orientierung DARF verwendet werden:

```text
M < 0.3
    -> INCOMPRESSIBLE_APPROXIMATION

0.3 <= M < 0.8
    -> SUBSONIC

0.8 <= M < 1.2
    -> TRANSONIC

1.2 <= M < 5
    -> SUPERSONIC

M >= 5
    -> HYPERSONIC
```

Diese Grenzen sind problemabhängig und nicht als universelle physikalische Grenzwerte zu interpretieren.

---

## 17. Compressibility

Kompressibilität SOLL abhängig von Mach-Zahl, Genauigkeitsanforderung und Modellwahl berücksichtigt werden können.

---

## 18. Reynolds Number

Die Reynolds-Zahl lautet:

```text
Re =
rho
*
U
*
L_ref
/
mu
```

---

## 19. Reference Length

Die Referenzlänge:

```text
L_ref
```

MUSS explizit definiert sein.

Beispiele:

```text
Chord
Diameter
VehicleLength
WingMeanAerodynamicChord
Custom
```

---

## 20. Dynamic Pressure

Der dynamische Druck lautet für geeignete Bedingungen:

```text
q =
1/2
*
rho
*
U^2
```

---

## 21. Reference Area

Aerodynamische Koeffizienten benötigen eine Referenzfläche:

```text
S_ref
```

---

## 22. Reference Point

Aerodynamische Momente benötigen einen Referenzpunkt.

---

## 23. Reference Frame

Aerodynamics Compute MUSS unterschiedliche Bezugssysteme unterstützen.

Mindestens:

```text
WORLD
BODY
WIND
STABILITY
CUSTOM
```

---

## 24. Coordinate Transform

Vektoren und Kräfte MÜSSEN zwischen unterstützten Bezugssystemen transformierbar sein.

---

## 25. Freestream

Der ungestörte Anströmzustand wird als:

```text
Freestream
```

repräsentiert.

---

## 26. Freestream State

Mindestens:

```text
Velocity
Pressure
Temperature
Density
MachNumber
TurbulenceState
```

SOLLTEN definierbar sein.

---

## 27. Angle of Attack

Der Anstellwinkel wird bezeichnet als:

```text
alpha
```

---

## 28. Sideslip Angle

Der Schiebewinkel wird bezeichnet als:

```text
beta
```

---

## 29. Aerodynamic Force

Die resultierende aerodynamische Kraft MUSS als Vektor repräsentiert werden können.

---

## 30. Lift

Auftrieb wird relativ zur definierten Anströmrichtung und Bezugsebene bestimmt.

---

## 31. Drag

Widerstand wird entgegen der relevanten Anströmrichtung bestimmt.

---

## 32. Side Force

Eine seitliche aerodynamische Kraft SOLL unterstützt werden.

---

## 33. Lift Coefficient

Der Auftriebsbeiwert lautet:

```text
C_L =
L
/
(q * S_ref)
```

---

## 34. Drag Coefficient

Der Widerstandsbeiwert lautet:

```text
C_D =
D
/
(q * S_ref)
```

---

## 35. Side Force Coefficient

Konzeptionell:

```text
C_Y =
Y
/
(q * S_ref)
```

---

## 36. Aerodynamic Moment

Aerodynamische Momente MÜSSEN um einen definierten Referenzpunkt bestimmbar sein.

---

## 37. Moment Coefficient

Ein Momentenbeiwert lautet konzeptionell:

```text
C_M =
M
/
(q * S_ref * L_ref)
```

---

## 38. Rolling Moment

Rollmoment SOLL repräsentiert werden können.

---

## 39. Pitching Moment

Nickmoment SOLL repräsentiert werden können.

---

## 40. Yawing Moment

Giermoment SOLL repräsentiert werden können.

---

## 41. Force Integration

Aerodynamische Kräfte SOLLEN aus Druck- und Schubspannungen über Oberflächen integrierbar sein.

---

## 42. Pressure Force

Konzeptionell:

```text
F_pressure =
integral(
    -p * n
    dA
)
```

---

## 43. Viscous Force

Konzeptionell:

```text
F_viscous =
integral(
    tau_wall
    dA
)
```

unter Berücksichtigung der lokalen Richtungen.

---

## 44. Total Aerodynamic Force

```text
F_total =
F_pressure
+
F_viscous
```

---

## 45. Pressure Coefficient

Der Druckbeiwert lautet für geeignete Referenzbedingungen:

```text
C_p =
(p - p_inf)
/
q_inf
```

---

## 46. Skin Friction Coefficient

Ein lokaler Hautreibungsbeiwert DARF definiert werden als:

```text
C_f =
tau_w
/
q_inf
```

---

## 47. Drag Decomposition

Widerstand SOLL in Komponenten zerlegbar sein.

Beispiele:

```text
PressureDrag
SkinFrictionDrag
InducedDrag
WaveDrag
InterferenceDrag
BaseDrag
CoolingDrag
Custom
```

---

## 48. Lift-to-Drag Ratio

Die Gleitzahl beziehungsweise aerodynamische Effizienz DARF bestimmt werden als:

```text
L_D =
L / D
```

oder:

```text
C_L / C_D
```

---

## 49. Airfoil

Aerodynamics Compute SOLL zweidimensionale Profilmodelle unterstützen.

---

## 50. Airfoil Geometry

Ein Profil SOLL mindestens repräsentieren können:

```text
Chord
LeadingEdge
TrailingEdge
UpperSurface
LowerSurface
Camber
Thickness
```

---

## 51. Chord

Die Profiltiefe wird bezeichnet als:

```text
c
```

---

## 52. Camber

Profilwölbung SOLL geometrisch bestimmbar sein.

---

## 53. Thickness

Profildicke SOLL relativ oder absolut repräsentiert werden können.

---

## 54. Airfoil Coordinates

Profile SOLLEN über Punktkoordinaten importierbar sein.

---

## 55. Parametric Airfoil

Parametrische Profilfamilien DÜRFEN unterstützt werden.

---

## 56. NACA Airfoils

NACA-Profilfamilien SOLLEN optional generierbar sein.

---

## 57. Airfoil Polar

Eine Profilpolare SOLL Größen enthalten können wie:

```text
Alpha
CL
CD
CM
TransitionLocation
SeparationState
```

---

## 58. Polar Interpolation

Zwischen bekannten Polarpunkten SOLL interpoliert werden können.

---

## 59. Polar Extrapolation

Extrapolation MUSS explizit gekennzeichnet werden.

---

## 60. Stall

Aerodynamics Compute SOLL Strömungsabriss repräsentieren und diagnostizieren können.

---

## 61. Stall Angle

Ein Modell DARF einen kritischen Anstellwinkel definieren.

---

## 62. Post-Stall

Erweiterte Modelle SOLLEN Post-Stall-Verhalten unterstützen können.

---

## 63. Wing

Dreidimensionale Tragflächen SOLLEN unterstützt werden.

---

## 64. Wing Geometry

Mindestens:

```text
Span
Area
RootChord
TipChord
Sweep
Dihedral
Twist
AirfoilDistribution
```

SOLLTEN repräsentierbar sein.

---

## 65. Aspect Ratio

Die Streckung lautet:

```text
AR =
b^2 / S
```

---

## 66. Taper Ratio

Die Zuspitzung lautet:

```text
lambda =
c_tip / c_root
```

---

## 67. Sweep Angle

Pfeilung SOLL an einer definierten Referenzlinie angegeben werden.

---

## 68. Dihedral

V-Stellung SOLL repräsentierbar sein.

---

## 69. Twist

Geometrische und aerodynamische Schränkung SOLLEN repräsentierbar sein.

---

## 70. Finite Wing Effects

Endliche Tragflächen MÜSSEN dreidimensionale Effekte berücksichtigen können.

---

## 71. Induced Drag

Ein einfaches Modell DARF verwenden:

```text
C_Di =
C_L^2
/
(pi * e * AR)
```

---

## 72. Oswald Efficiency

Der Effizienzfaktor wird bezeichnet als:

```text
e
```

---

## 73. Lifting Line

Prandtls Traglinientheorie SOLL als Low-Fidelity-Modell integrierbar sein.

---

## 74. Vortex Lattice Method

Die Vortex-Lattice-Methode SOLL als potenzielles mittleres Fidelity-Modell unterstützt werden können.

---

## 75. Panel Methods

Potentialströmungsbasierte Panelmethoden SOLLEN integrierbar sein.

---

## 76. CFD Model

High-Fidelity-Aerodynamik SOLL über CFD integrierbar sein.

---

## 77. Fidelity Levels

Aerodynamics Compute SOLL unterschiedliche Modelltreuen unterstützen.

Beispiel:

```text
ANALYTICAL
EMPIRICAL
PANEL
VORTEX_LATTICE
RANS
URANS
LES
DNS
CUSTOM
```

---

## 78. Model Independence

Eine aerodynamische Anwendung SOLL soweit möglich dieselben Ergebnisobjekte unabhängig vom verwendeten Fidelity-Level erhalten.

---

## 79. Boundary Layer

Aerodynamics Compute SOLL Grenzschichten repräsentieren können.

---

## 80. Laminar Boundary Layer

Laminare Grenzschichten SOLLEN unterstützt werden.

---

## 81. Turbulent Boundary Layer

Turbulente Grenzschichten SOLLEN unterstützt werden.

---

## 82. Boundary Layer Thickness

Grenzschichtdicke SOLL bestimmbar sein.

---

## 83. Displacement Thickness

Verdrängungsdicke SOLL repräsentierbar sein.

---

## 84. Momentum Thickness

Impulsverlustdicke SOLL repräsentierbar sein.

---

## 85. Shape Factor

Ein Grenzschicht-Formfaktor DARF bestimmt werden.

---

## 86. Wall Shear Stress

Wandschubspannung:

```text
tau_w
```

MUSS bei viskosen Modellen verfügbar sein können.

---

## 87. Transition

Der Übergang von laminarer zu turbulenter Grenzschicht SOLL modellierbar sein.

---

## 88. Transition Location

Transitionspositionen SOLLEN diagnostizierbar sein.

---

## 89. Transition Model

Aerodynamics Compute SOLL mit Transitionsmodellen integrierbar sein.

---

## 90. Separation

Strömungsablösung SOLL erkannt werden können.

---

## 91. Separation Point

Ablösepunkte oder Ablösebereiche SOLLEN identifizierbar sein.

---

## 92. Reattachment

Wiederanlegung SOLL diagnostizierbar sein.

---

## 93. Wake

Nachlaufströmungen SOLLEN repräsentierbar sein.

---

## 94. Wake Deficit

Geschwindigkeitsdefizite im Nachlauf SOLLEN bestimmbar sein.

---

## 95. Vorticity

Wirbelstärke wird beschrieben durch:

```text
omega =
curl(U)
```

---

## 96. Vortex Detection

Wirbelstrukturen SOLLEN über geeignete Kriterien identifizierbar sein.

Beispiele:

```text
VorticityMagnitude
QCriterion
Lambda2
SwirlingStrength
Custom
```

---

## 97. Turbulence Coupling

Aerodynamics Compute MUSS mit `NPSPEC-COMPUTE-TURBULENCE-0001` koppelbar sein.

---

## 98. RANS

Reynolds-Averaged-Navier-Stokes-Modelle SOLLEN unterstützt werden können.

---

## 99. URANS

Unsteady RANS SOLL unterstützt werden können.

---

## 100. LES

Large-Eddy-Simulation SOLL für geeignete Workloads integrierbar sein.

---

## 101. DNS

Direct Numerical Simulation DARF für Forschungs- und Referenzfälle integrierbar sein.

---

## 102. Turbulence Model Selection

Die Auswahl des Turbulenzmodells MUSS explizit sein.

---

## 103. Wall Treatment

Turbulenzmodelle SOLLEN unterschiedliche Wandbehandlungen verwenden können.

---

## 104. yPlus

Die dimensionslose Wanddistanz:

```text
y+
```

SOLL berechnet und diagnostiziert werden können.

---

## 105. yPlus Quality

Der Solver SOLL prüfen können, ob die lokale Netzauflösung zum gewählten Wandmodell passt.

---

## 106. Compressible Aerodynamics

Aerodynamics Compute SOLL kompressible Strömungen über `NPSPEC-COMPUTE-GASDYNAMICS-0001` integrieren können.

---

## 107. Conservation Equations

Kompressible High-Fidelity-Modelle SOLLEN Massenerhaltung, Impulserhaltung und Energieerhaltung lösen können.

---

## 108. Conservative Variables

Typische konservative Variablen sind:

```text
rho
rhoU
rhoV
rhoW
rhoE
```

---

## 109. Shock Wave

Stoßwellen MÜSSEN bei supersonischen und transsonischen Modellen repräsentierbar sein.

---

## 110. Shock Detection

Stoßwellen SOLLEN automatisch identifizierbar sein.

---

## 111. Shock Capturing

Kompressible Solver SOLLEN geeignete Shock-Capturing-Verfahren verwenden können.

---

## 112. Shock Fitting

Alternative Shock-Fitting-Verfahren DÜRFEN integrierbar sein.

---

## 113. Normal Shock

Normale Stoßwellen SOLLEN analytisch und numerisch unterstützt werden.

---

## 114. Oblique Shock

Schräge Stoßwellen SOLLEN unterstützt werden.

---

## 115. Expansion Fan

Prandtl-Meyer-Expansionen SOLLEN unterstützt werden.

---

## 116. Transonic Flow

Transsonische Strömungen MÜSSEN lokale subsonische und supersonische Bereiche gleichzeitig behandeln können.

---

## 117. Wave Drag

Kompressible Modelle SOLLEN Wellenwiderstand bestimmen können.

---

## 118. Critical Mach Number

Ein kritischer Mach-Bereich DARF bestimmt werden.

---

## 119. Hypersonic Flow

Hyperschallmodelle SOLLEN architektonisch unterstützt werden.

---

## 120. High-Temperature Effects

Bei hohen Mach-Zahlen DÜRFEN temperaturabhängige Gasmodelle erforderlich sein.

---

## 121. Real Gas

Aerodynamics Compute SOLL mit Realgasmodellen integrierbar sein.

---

## 122. Thermochemical Nonequilibrium

Thermochemisches Nichtgleichgewicht DARF als zukünftige Erweiterung unterstützt werden.

---

## 123. Propeller

Propeller SOLLEN als aerodynamische Rotorsysteme modellierbar sein.

---

## 124. Propeller Geometry

Mindestens:

```text
Radius
HubRadius
BladeCount
ChordDistribution
TwistDistribution
AirfoilDistribution
```

SOLLTEN repräsentierbar sein.

---

## 125. Rotational Speed

Drehzahl MUSS definierbar sein als:

```text
RPM
```

oder:

```text
omega
```

---

## 126. Advance Ratio

Ein Propeller DARF über den Fortschrittsgrad beschrieben werden:

```text
J =
V
/
(n * D)
```

---

## 127. Thrust

Propellerschub MUSS bestimmbar sein.

---

## 128. Torque

Propellermoment MUSS bestimmbar sein.

---

## 129. Propeller Efficiency

Propellerwirkungsgrad SOLL bestimmbar sein.

---

## 130. Blade Element Theory

Blade-Element-Modelle SOLLEN unterstützt werden.

---

## 131. Blade Element Momentum

BEM-artige Modelle SOLLEN unterstützt werden.

---

## 132. Actuator Disk

Ein vereinfachtes Actuator-Disk-Modell SOLL verfügbar sein können.

---

## 133. Actuator Line

Actuator-Line-Modelle DÜRFEN integriert werden.

---

## 134. Resolved Blade CFD

Vollständig aufgelöste Rotorblätter SOLLEN über CFD berechnet werden können.

---

## 135. Rotor

Rotoren SOLLEN dieselbe allgemeine Rotationsinfrastruktur verwenden können.

---

## 136. Helicopter Rotor

Hubschrauberrotoren DÜRFEN zusätzliche zyklische und kollektive Blattverstellung besitzen.

---

## 137. Wind Turbine

Windturbinen SOLLEN unterstützt werden.

---

## 138. Tip Speed Ratio

Für Windturbinen:

```text
lambda =
omega * R / V
```

---

## 139. Wind Turbine Power

Konzeptionell:

```text
P =
1/2
*
rho
*
A
*
V^3
*
C_P
```

---

## 140. Power Coefficient

Der Leistungsbeiwert:

```text
C_P
```

SOLL bestimmbar sein.

---

## 141. Betz Limit

Idealisierten Windturbinenmodellen DARF die Betz-Grenze als Plausibilitätsreferenz dienen.

---

## 142. Ground Vehicle Aerodynamics

Aerodynamics Compute SOLL Fahrzeug-Aerodynamik unterstützen.

---

## 143. Vehicle Drag

Fahrzeugwiderstand DARF berechnet werden als:

```text
D =
1/2
*
rho
*
U^2
*
S_ref
*
C_D
```

---

## 144. Vehicle Lift

Auf- beziehungsweise Abtrieb SOLL bestimmbar sein.

---

## 145. Downforce

Negative Lift-Richtung DARF als Downforce ausgewiesen werden.

---

## 146. Ground Effect

Bodeneffekt SOLL modellierbar sein.

---

## 147. Moving Ground

CFD-Simulationen SOLLEN bewegte Bodenrandbedingungen unterstützen können.

---

## 148. Rotating Wheels

Rotierende Räder SOLLEN als bewegte Geometrie oder Randbedingung modellierbar sein.

---

## 149. Train Aerodynamics

Zug-Aerodynamik SOLL unterstützt werden.

---

## 150. Crosswind

Seitenwind SOLL als Anströmbedingung definierbar sein.

---

## 151. Tunnel Entry

Druckwellen bei Tunneleinfahrt DÜRFEN über Gasdynamics Compute modelliert werden.

---

## 152. Building Aerodynamics

Gebäude- und Stadtströmungen SOLLEN unterstützt werden.

---

## 153. Wind Load

Windlasten auf Strukturen SOLLEN berechnet werden können.

---

## 154. Pressure Distribution

Oberflächendruckverteilungen SOLLEN für strukturelle Kopplung exportierbar sein.

---

## 155. Atmospheric Boundary Layer

Wind-Engineering-Modelle SOLLEN atmosphärische Grenzschichten verwenden können.

---

## 156. Terrain

Gelände SOLL in Windströmungsmodellen berücksichtigt werden können.

---

## 157. Roughness

Bodenrauheit SOLL parametrisiert werden können.

---

## 158. Gust

Böen SOLLEN zeitabhängig definierbar sein.

---

## 159. Turbulent Inflow

Turbulente Einströmprofile SOLLEN unterstützt werden.

---

## 160. Internal Aerodynamics

Aerodynamics Compute SOLL interne Gasströmungen unterstützen.

Beispiele:

```text
Duct
Diffuser
Nozzle
Intake
Ventilation
CoolingChannel
Fan
Compressor
```

---

## 161. Pressure Loss

Interne aerodynamische Systeme SOLLEN Druckverluste bestimmen können.

---

## 162. Total Pressure Loss

Gesamtdruckverluste SOLLEN ausgewertet werden können.

---

## 163. Pressure Recovery

Diffusoren und Einläufe SOLLEN Druckrückgewinnung bestimmen können.

---

## 164. Mass Flow

Interne Strömungen SOLLEN Massenstrom bestimmen können.

---

## 165. Choked Flow

Kompressible interne Strömungen MÜSSEN kritische beziehungsweise abgewürgte Strömung unterstützen können.

---

## 166. Nozzle

Düsenströmungen SOLLEN über Gasdynamics Compute integrierbar sein.

---

## 167. Intake

Einläufe SOLLEN Druckverlust, Verzerrung und Massenstrom analysieren können.

---

## 168. Fan

Ventilatoren SOLLEN über Kennfelder oder aufgelöste Geometrie modellierbar sein.

---

## 169. Fan Map

Ein Kennfeld DARF Größen enthalten:

```text
FlowRate
PressureRise
Efficiency
RPM
```

---

## 170. Compressor

Verdichter DÜRFEN über Gasdynamics- oder Turbomachinery-Modelle integriert werden.

---

## 171. Moving Geometry

Aerodynamics Compute SOLL bewegte Geometrien unterstützen können.

---

## 172. Rigid Motion

Mindestens:

```text
Translation
Rotation
CombinedRigidMotion
```

SOLLTEN unterstützt werden.

---

## 173. Prescribed Motion

Bewegung DARF zeitabhängig vorgegeben werden.

---

## 174. Coupled Motion

Bewegung DARF aus Kräften und Momenten resultieren.

---

## 175. Six Degrees of Freedom

6-DOF-Kopplung SOLL unterstützt werden können.

---

## 176. 6DOF State

Mindestens:

```text
Position
Orientation
LinearVelocity
AngularVelocity
Mass
InertiaTensor
```

---

## 177. Dynamic Mesh

Bewegte CFD-Geometrien SOLLEN Dynamic-Mesh-Verfahren verwenden können.

---

## 178. Overset Mesh

Overset- beziehungsweise Chimera-Netze SOLLEN integrierbar sein.

---

## 179. Sliding Mesh

Rotierende Maschinen SOLLEN Sliding-Mesh-Verfahren verwenden können.

---

## 180. Moving Reference Frame

Stationäre Näherungen rotierender Systeme SOLLEN Moving-Reference-Frame-Modelle verwenden können.

---

## 181. Unsteady Aerodynamics

Aerodynamics Compute SOLL instationäre aerodynamische Vorgänge unterstützen.

---

## 182. Time-Dependent Forces

Kräfte und Momente SOLLEN zeitaufgelöst verfügbar sein.

---

## 183. Dynamic Stall

Dynamischer Strömungsabriss SOLL als erweitertes Modell integrierbar sein.

---

## 184. Oscillating Airfoil

Oszillierende Profile SOLLEN als Referenz- und Anwendungsfall unterstützt werden.

---

## 185. Vortex Shedding

Wirbelablösung SOLL zeitaufgelöst berechnet werden können.

---

## 186. Strouhal Number

Die Strouhal-Zahl lautet:

```text
St =
f * L / U
```

---

## 187. Frequency Analysis

Instationäre aerodynamische Signale SOLLEN spektral analysierbar sein.

---

## 188. Aeroacoustics Coupling

Aerodynamics Compute SOLL mit einer Aeroakustik-Schicht koppelbar sein.

---

## 189. Acoustic Source Data

Mindestens folgende Größen SOLLEN exportierbar sein:

```text
PressureFluctuation
VelocityFluctuation
SurfacePressure
ForceHistory
Vorticity
DensityFluctuation
```

---

## 190. Ffowcs Williams-Hawkings

FW-H-artige Aeroakustikmodelle DÜRFEN integriert werden.

---

## 191. Thermal Coupling

Aerodynamics Compute SOLL thermische Kopplung unterstützen.

---

## 192. Convective Heat Transfer

Konvektiver Wärmeübergang SOLL berechenbar sein.

---

## 193. Wall Heat Flux

Wandwärmestrom SOLL verfügbar sein können.

---

## 194. Adiabatic Wall

Adiabatische Wände SOLLEN unterstützt werden.

---

## 195. Isothermal Wall

Wände mit fester Temperatur SOLLEN unterstützt werden.

---

## 196. Conjugate Heat Transfer

Gekoppelte Fluid-Festkörper-Wärmeübertragung SOLL architektonisch unterstützt werden.

---

## 197. Multiphase Aerodynamics

Aerodynamics Compute MUSS mit `NPSPEC-COMPUTE-MULTIPHASE-0001` koppelbar sein.

---

## 198. Typical Multiphase Applications

Beispiele:

```text
Rain
Droplets
Spray
Dust
Sand
IceParticles
WaterIngestion
ParticleLadenFlow
Condensation
```

---

## 199. Particle Tracking

Partikel in Luftströmungen SOLLEN über Euler-Lagrange-Modelle verfolgt werden können.

---

## 200. Droplet Tracking

Tropfen SOLLEN einschließlich Trägheit, Widerstand und gegebenenfalls Verdampfung verfolgt werden können.

---

## 201. Icing Integration

Vereisungsmodelle DÜRFEN auf Droplet- und Thermal-Compute aufbauen.

---

## 202. Sand and Dust

Partikelbeladene Luftströmungen SOLLEN modellierbar sein.

---

## 203. Erosion Coupling

Partikelaufprall DÜRFEN Erosionsmodelle speisen.

---

## 204. Boundary Conditions

Aerodynamics Compute MUSS aerodynamische Randbedingungen unterstützen.

---

## 205. Freestream Boundary

Eine Freestream-Randbedingung SOLL mindestens definieren:

```text
Velocity
Pressure
Temperature
Direction
Turbulence
```

---

## 206. Velocity Inlet

Geschwindigkeitseinlässe SOLLEN unterstützt werden.

---

## 207. Mass Flow Inlet

Massenstromeinlässe SOLLEN unterstützt werden.

---

## 208. Pressure Inlet

Druckeinlässe SOLLEN unterstützt werden.

---

## 209. Pressure Outlet

Druckauslässe SOLLEN unterstützt werden.

---

## 210. Far Field

Kompressible externe Aerodynamik SOLL Far-Field-Randbedingungen unterstützen.

---

## 211. Wall Boundary

Mindestens:

```text
NO_SLIP
SLIP
MOVING_WALL
ADIABATIC
ISOTHERMAL
```

SOLLTEN kombinierbar sein.

---

## 212. Symmetry

Symmetrierandbedingungen SOLLEN unterstützt werden.

---

## 213. Periodic

Periodische Randbedingungen SOLLEN unterstützt werden.

---

## 214. Rotational Periodic

Rotationsperiodische Randbedingungen SOLLEN für Turbomaschinen integrierbar sein.

---

## 215. Initial Conditions

Transiente aerodynamische Simulationen MÜSSEN Anfangszustände unterstützen.

---

## 216. Uniform Initialization

Ein gleichförmiger Freestream SOLL als Initialzustand verwendet werden können.

---

## 217. Imported Initialization

Vorherige Lösungen SOLLEN als Anfangsfeld importierbar sein.

---

## 218. Restart Initialization

Checkpoints SOLLEN als Initialzustand verwendet werden.

---

## 219. Solver Coupling

Aerodynamics Compute MUSS mit unterschiedlichen Flow-Solvern integrierbar sein.

---

## 220. Incompressible Solver

Niedrige Mach-Zahlen SOLLEN über inkompressible Solver berechenbar sein.

---

## 221. Compressible Solver

Kompressible Strömungen SOLLEN über konservative Gasdynamics-Solver berechenbar sein.

---

## 222. Pressure-Based Solver

Pressure-Based-Solver SOLLEN unterstützt werden können.

---

## 223. Density-Based Solver

Density-Based-Solver SOLLEN unterstützt werden können.

---

## 224. Spatial Discretization

Mindestens folgende Verfahren SOLLEN integrierbar sein:

```text
FiniteVolume
FiniteDifference
FiniteElement
Spectral
DiscontinuousGalerkin
Meshless
Custom
```

---

## 225. Temporal Discretization

Mindestens:

```text
Steady
Explicit
Implicit
SemiImplicit
DualTime
```

SOLLTEN integrierbar sein.

---

## 226. Numerical Flux

Kompressible Solver SOLLEN austauschbare numerische Flüsse verwenden können.

Beispiele:

```text
Roe
HLL
HLLC
AUSM
Rusanov
Custom
```

---

## 227. Reconstruction

Higher-Order-Verfahren SOLLEN unterschiedliche Rekonstruktionsmethoden verwenden können.

---

## 228. Limiter

Shock-Capturing-Solver SOLLEN Limiter integrieren können.

---

## 229. Gradient Reconstruction

Gradienten SOLLEN über unterschiedliche Verfahren bestimmt werden können.

---

## 230. Mesh

Aerodynamics Compute SOLL unterschiedliche Mesh-Typen unterstützen können.

---

## 231. Structured Mesh

Strukturierte Netze SOLLEN unterstützt werden.

---

## 232. Unstructured Mesh

Unstrukturierte Netze SOLLEN unterstützt werden.

---

## 233. Hybrid Mesh

Hybride Netze SOLLEN unterstützt werden.

---

## 234. Boundary Layer Mesh

Prismatische oder anderweitig wandangepasste Schichten SOLLEN unterstützt werden.

---

## 235. Mesh Quality

Aerodynamics Compute SOLL Mesh-Qualitätsmetriken auswerten können.

---

## 236. Mesh Quality Metrics

Beispiele:

```text
Skewness
Orthogonality
AspectRatio
CellVolume
FaceArea
Jacobian
```

---

## 237. Invalid Mesh

Ungültige Zellen MÜSSEN vor Solverstart erkannt werden können.

---

## 238. Mesh Independence

Aerodynamische Ergebnisse SOLLEN über Netzverfeinerungsstudien überprüfbar sein.

---

## 239. Adaptive Mesh Refinement

AMR SOLL architektonisch unterstützt werden.

---

## 240. Refinement Indicators

Beispiele:

```text
PressureGradient
DensityGradient
Vorticity
Shock
BoundaryLayer
Wake
ErrorEstimator
```

---

## 241. Coarsening

Adaptive Netze SOLLEN Bereiche geringer Anforderung wieder vergröbern können.

---

## 242. CPU Execution

Eine konforme Implementierung MUSS CPU-Ausführung unterstützen.

---

## 243. SIMD

Lokale Feldoperationen SOLLEN SIMD verwenden können.

---

## 244. Multi-Core

Zellen, Flächen und andere unabhängige Workloads SOLLEN parallel berechnet werden können.

---

## 245. GPU Execution

Geeignete aerodynamische Operationen DÜRFEN über `NPSPEC-COMPUTE-RUNTIME-0005` ausgeführt werden.

---

## 246. GPU Candidates

Beispiele:

```text
FluxEvaluation
GradientEvaluation
ResidualAssembly
TurbulenceKernels
ForceIntegration
ParticleTracking
LinearAlgebra
PostProcessing
```

---

## 247. Multi-GPU

Große CFD-Probleme SOLLEN auf mehrere GPUs verteilt werden können.

---

## 248. HPC Integration

Aerodynamics Compute SOLL `NPSPEC-COMPUTE-HPC-0001` verwenden können.

---

## 249. Distributed Compute

Große aerodynamische Simulationen SOLLEN über mehrere Nodes verteilt werden können.

---

## 250. Domain Decomposition

CFD-Domains MÜSSEN partitionierbar sein.

---

## 251. Halo Exchange

Feldwerte MÜSSEN an Partitionsgrenzen synchronisiert werden können.

---

## 252. Load Balancing

Die Runtime SOLL ungleichmäßige Workloads berücksichtigen.

---

## 253. Dynamic Load Balancing

AMR und bewegte Geometrien SOLLEN dynamische Lastverteilung ermöglichen.

---

## 254. Communication Overlap

Kommunikation SOLL mit lokaler Berechnung überlappt werden können.

---

## 255. Memory Model

Aerodynamics Compute MUSS auf `NPSPEC-COMPUTE-RUNTIME-0003` abbildbar sein.

---

## 256. Typical Memory Objects

Beispiele:

```text
PressureField
VelocityField
DensityField
TemperatureField
EnergyField

TurbulenceFields
GradientFields
ResidualFields

SurfacePressure
WallShearStress

ForceHistory
MomentHistory

MeshConnectivity
BoundaryData
SolverWorkspace
```

---

## 257. Structure of Arrays

Feld- und Zelloperationen SOLLEN für geeignete Backends Structure-of-Arrays verwenden können.

---

## 258. Memory Pools

Temporäre Solverfelder SOLLEN aus wiederverwendbaren Memory Pools stammen können.

---

## 259. Zero-Copy

Unnötige Kopien zwischen Aerodynamics Compute, Turbulence Compute und Gasdynamics Compute SOLLEN vermieden werden.

---

## 260. Checkpointing

Lang laufende aerodynamische Simulationen MÜSSEN Checkpointing integrieren können.

---

## 261. Checkpoint Content

Je nach Modell:

```text
Time
MeshState
Pressure
Velocity
Density
Temperature
Energy
TurbulenceState
MovingGeometryState
SolverState
Statistics
```

---

## 262. Restart

Ein kompatibler Restart MUSS den aerodynamischen Zustand vollständig wiederherstellen können.

---

## 263. Moving Geometry Restart

Bewegte Geometrien MÜSSEN Position und Bewegung wiederherstellen können.

---

## 264. Model Compatibility

Beim Restart MUSS die Kompatibilität von Solver- und Modellversion geprüft werden.

---

## 265. Profiling

Aerodynamics Compute MUSS Profiling unterstützen.

---

## 266. Profiling Categories

Mindestens:

```text
FluxTime
GradientTime
ResidualTime
LinearSolveTime
TurbulenceTime
BoundaryTime
ForceIntegrationTime
MeshMotionTime
CommunicationTime
PostProcessingTime
```

---

## 267. Performance Metrics

Mindestens:

```text
CellsPerSecond
FacesPerSecond
IterationsPerSecond
TimeStepsPerSecond
MemoryUsage
CommunicationFraction
GPUUtilization
```

---

## 268. Aerodynamic Diagnostics

Mindestens folgende Größen SOLLTEN während oder nach einer Berechnung verfügbar sein:

```text
CL
CD
CY
CM
PressureRange
VelocityRange
MachRange
ReynoldsRange
MassBalanceError
Residuals
```

---

## 269. Surface Diagnostics

Mindestens:

```text
Cp
Cf
WallShearStress
WallPressure
HeatFlux
yPlus
```

SOLLTEN auswertbar sein.

---

## 270. Flow Diagnostics

Mindestens:

```text
Vorticity
QCriterion
MachNumber
TotalPressure
Entropy
TurbulenceIntensity
```

SOLLTEN verfügbar sein können.

---

## 271. Diagnostic Classes

Mindestens:

```text
INVALID_FLUID
INVALID_REFERENCE_STATE
INVALID_REFERENCE_AREA
INVALID_REFERENCE_LENGTH
INVALID_MESH
INVALID_BOUNDARY_CONDITION
MASS_BALANCE_ERROR
ENERGY_BALANCE_ERROR
HIGH_RESIDUAL
FLOW_DIVERGENCE
SHOCK_INSTABILITY
TURBULENCE_MODEL_WARNING
YPLUS_WARNING
SEPARATION_WARNING
STALL_WARNING
MODEL_OUTSIDE_VALIDITY
```

---

## 272. Invalid Reference Area

Eine Referenzfläche MUSS größer als null sein.

---

## 273. Invalid Reference Length

Eine Referenzlänge MUSS größer als null sein.

---

## 274. Invalid Density

Eine physikalisch ungültige Dichte MUSS erkannt werden.

---

## 275. Invalid Temperature

Ungültige absolute Temperaturen MÜSSEN erkannt werden.

---

## 276. Negative Absolute Pressure

Unphysikalisch negativer Absolutdruck MUSS erkannt werden.

---

## 277. Mass Conservation Diagnostics

CFD-Modelle MÜSSEN Massenbilanzfehler bestimmen können.

---

## 278. Momentum Conservation Diagnostics

Geeignete Modelle SOLLEN Impulsbilanzen bereitstellen können.

---

## 279. Energy Conservation Diagnostics

Kompressible Solver SOLLEN Energiebilanzen überwachen.

---

## 280. Solver Residuals

Mindestens:

```text
ContinuityResidual
MomentumResidual
EnergyResidual
TurbulenceResidual
```

SOLLTEN verfügbar sein.

---

## 281. Convergence

Konvergenzkriterien MÜSSEN explizit definiert sein.

---

## 282. Force Convergence

Stationäre externe Aerodynamik SOLL zusätzlich die Konvergenz von:

```text
CL
CD
CM
```

überwachen können.

---

## 283. Non-Convergence

Nichtkonvergenz DARF nicht stillschweigend als gültiges Ergebnis behandelt werden.

---

## 284. Error Model

Mindestens:

```text
INVALID_MODEL
INVALID_FLUID
INVALID_GEOMETRY
INVALID_MESH
INVALID_REFERENCE_STATE
INVALID_BOUNDARY_CONDITION
INVALID_INITIAL_CONDITION
SOLVER_DIVERGED
SOLVER_NOT_CONVERGED
NUMERIC_FAILURE
TURBULENCE_FAILURE
GASDYNAMICS_FAILURE
MULTIPHASE_FAILURE
OUT_OF_MEMORY
DEVICE_FAILURE
COMMUNICATION_FAILURE
CHECKPOINT_FAILURE
CANCELLED
```

---

## 285. Deterministic Mode

Aerodynamics Compute SOLL einen deterministischen Modus unterstützen können.

---

## 286. Deterministic Reduction

Parallele Summationen für Kräfte, Momente und Residuen SOLLEN bei Bedarf deterministisch ausgeführt werden können.

---

## 287. Random Processes

Stochastische Turbulenz- oder Partikelmodelle MÜSSEN explizite Seeds verwenden können.

---

## 288. Reproducibility Metadata

Ein Ergebnis SOLL mindestens referenzieren können:

```text
AerodynamicModel
ModelVersion
FluidModel
GeometryVersion
MeshVersion
Solver
SolverVersion
TurbulenceModel
BoundaryConditions
ReferenceState
RuntimeVersion
HardwareBackend
```

---

## 289. Validation

Aerodynamische Modelle MÜSSEN gegen geeignete Referenzfälle validierbar sein.

---

## 290. Validation Classes

Mindestens:

```text
Analytical
Experimental
WindTunnel
Benchmark
CrossSolver
Regression
ManufacturedSolution
```

---

## 291. Flat Plate

Eine ebene Platte SOLL für Grenzschicht- und Reibungsvalidierung verwendet werden können.

---

## 292. Cylinder Flow

Zylinderumströmung SOLL als Referenzfall verwendet werden können.

---

## 293. Airfoil Validation

Profile SOLLEN gegen experimentelle oder etablierte Polaren geprüft werden.

---

## 294. Wing Validation

Endliche Tragflächen SOLLEN gegen geeignete Referenzdaten geprüft werden.

---

## 295. Ahmed Body

Fahrzeug-Aerodynamik DARF gegen etablierte Bluff-Body-Benchmarks wie den Ahmed Body validiert werden.

---

## 296. Backward-Facing Step

Ablösung und Wiederanlegung SOLLEN über geeignete Benchmarkfälle geprüft werden.

---

## 297. Transonic Airfoil

Transsonische Solver SOLLEN an Profilfällen mit Stoßwellen validiert werden.

---

## 298. Supersonic Wedge

Supersonische Solver SOLLEN gegen analytische schräge Stoßwellen getestet werden.

---

## 299. Normal Shock Tube

Gasdynamische Stoßbehandlung SOLL über geeignete Shock-Tube-Fälle validierbar sein.

---

## 300. Propeller Validation

Propellermodelle SOLLEN gegen Schub-, Moment- und Wirkungsgradmessungen geprüft werden.

---

## 301. Wind Turbine Validation

Windturbinenmodelle SOLLEN gegen Leistungs- und Schubkennlinien geprüft werden.

---

## 302. Testing — Freestream

Mindestens:

```text
ZeroVelocity
LowSpeed
Subsonic
Transonic
Supersonic
```

---

## 303. Testing — Aerodynamic Coefficients

Mindestens:

```text
CL
CD
CY
CM
ReferenceArea
ReferenceLength
ReferencePoint
```

---

## 304. Testing — Coordinate Systems

Mindestens:

```text
WorldToBody
BodyToWind
WindToBody
ForceTransformation
MomentTransformation
```

---

## 305. Testing — Airfoil

Mindestens:

```text
ZeroAngle
PositiveAngle
NegativeAngle
LiftCurve
DragPolar
StallRegion
```

---

## 306. Testing — Wing

Mindestens:

```text
AspectRatio
Sweep
Twist
InducedDrag
LiftDistribution
```

---

## 307. Testing — Boundary Layer

Mindestens:

```text
Laminar
Turbulent
Transition
WallShear
Separation
```

---

## 308. Testing — Compressibility

Mindestens:

```text
M0.1
M0.5
M0.9
M1.0
M2.0
M5.0
```

soweit vom jeweiligen Solver unterstützt.

---

## 309. Testing — Shock

Mindestens:

```text
NormalShock
ObliqueShock
ShockReflection
ShockDetection
```

---

## 310. Testing — Propeller

Mindestens:

```text
StaticThrust
ForwardFlight
RPMVariation
BladePitchVariation
```

---

## 311. Testing — Moving Geometry

Mindestens:

```text
Translation
Rotation
CombinedMotion
Restart
```

---

## 312. Testing — Unsteady Flow

Mindestens:

```text
OscillatingAirfoil
VortexShedding
TimeDependentForce
```

---

## 313. Testing — Turbulence Coupling

Mindestens:

```text
RANS
URANS
WallTreatment
yPlus
```

soweit implementiert.

---

## 314. Testing — Multiphase Coupling

Mindestens:

```text
ParticleTracking
DropletTracking
OneWayCoupling
```

soweit implementiert.

---

## 315. Testing — Mass Conservation

Jeder CFD-Solver MUSS auf Massenerhaltung geprüft werden.

---

## 316. Testing — Energy Conservation

Kompressible Solver SOLLEN auf Energieerhaltung geprüft werden.

---

## 317. Testing — Force Integration

Kräfte und Momente MÜSSEN auf analytischen oder kontrollierten Oberflächenfällen geprüft werden.

---

## 318. Testing — CPU

Alle Mindestfunktionen MÜSSEN auf CPU getestet werden.

---

## 319. Testing — GPU

GPU-Ergebnisse SOLLEN innerhalb definierter Toleranzen mit CPU-Referenzen übereinstimmen.

---

## 320. Testing — Multi-Core

Mehrkernberechnung MUSS numerisch konsistent sein.

---

## 321. Testing — Distributed

Verteilte Ergebnisse SOLLEN gegen äquivalente lokale Berechnungen geprüft werden.

---

## 322. Testing — Checkpoint

Mindestens:

```text
Save
Restart
FlowFieldRestore
TurbulenceStateRestore
MovingGeometryRestore
```

---

## 323. Testing — Determinism

Deterministische Modi MÜSSEN reproduzierbare Ergebnisse liefern können.

---

## 324. Testing — Performance

Mindestens:

```text
SmallMesh
MediumMesh
LargeMesh
MultiCore
GPU
MultiGPU
MultiNode
```

---

## 325. Strong Scaling

Ein festes aerodynamisches Problem SOLL mit steigender Ressourcenanzahl getestet werden.

---

## 326. Weak Scaling

Problemgröße und Ressourcenanzahl SOLLEN gemeinsam erhöht werden.

---

## 327. Stress Tests

Mindestens:

```text
LargeMesh
VeryHighAspectRatioMesh
HighReynoldsFlow
TransonicShock
LongTransientRun
LargeLES
MovingMesh
MultiGPU
HighNodeCount
MemoryPressure
```

---

## 328. Fault Injection

Mindestens:

```text
OUT_OF_MEMORY
GPU_FAILURE
NODE_FAILURE
COMMUNICATION_FAILURE
CHECKPOINT_FAILURE
SOLVER_FAILURE
INVALID_MESH_STATE
```

---

## 329. Regression Testing

Änderungen an aerodynamischen Modellen MÜSSEN gegen definierte Referenzfälle geprüft werden.

---

## 330. Regression Quantities

Mindestens:

```text
CL
CD
CM
Cp
Pressure
Velocity
Mach
ShockPosition
SeparationPoint
MassBalance
```

---

## 331. Optimization

Aerodynamics Compute SOLL Optimierungsworkflows unterstützen.

---

## 332. Design Variables

Beispiele:

```text
AirfoilShape
WingSpan
WingSweep
WingTwist
AngleOfAttack
VehicleShape
RideHeight
PropellerPitch
RotorRPM
```

---

## 333. Objectives

Beispiele:

```text
MaximizeLift
MinimizeDrag
MaximizeLiftToDrag
MinimizeMoment
MaximizeDownforce
MaximizeEfficiency
MinimizeNoise
MinimizeHeatLoad
```

---

## 334. Constraints

Optimierung SOLL technische und physikalische Nebenbedingungen berücksichtigen können.

---

## 335. Gradient-Based Optimization

Gradientenbasierte Optimierung SOLL integrierbar sein.

---

## 336. Adjoint Methods

Adjungierte Verfahren SOLLEN für große Designräume integrierbar sein.

---

## 337. Gradient-Free Optimization

Gradientenfreie Verfahren SOLLEN unterstützt werden können.

---

## 338. Parameter Sweeps

Parameterstudien SOLLEN parallel ausführbar sein.

---

## 339. Uncertainty Quantification

Unsicherheiten in:

```text
Geometry
Freestream
MaterialProperties
Turbulence
BoundaryConditions
```

SOLLTEN analysierbar sein.

---

## 340. Sensitivity Analysis

Sensitivitäten aerodynamischer Ergebnisse SOLLEN bestimmbar sein.

---

## 341. Reduced Order Models

Aerodynamics Compute SOLL Reduced-Order-Modelle integrieren können.

---

## 342. Surrogate Models

Datenbasierte Ersatzmodelle DÜRFEN verwendet werden.

---

## 343. AI Assistance

AI DARF Aerodynamics Compute unterstützen.

Beispiele:

```text
FlowFieldPrediction
SurrogateModeling
GeometryOptimization
MeshAdaptation
TurbulenceClosure
AnomalyDetection
ConvergencePrediction
SolverAcceleration
```

---

## 344. AI Physical Constraints

AI-basierte Modelle DÜRFEN physikalische Bilanzprüfungen nicht umgehen.

---

## 345. AI Model Validity

AI-Modelle MÜSSEN ihren Trainings- und Gültigkeitsbereich deklarieren können.

---

## 346. Model Provenance

Die Herkunft eines aerodynamischen Modells MUSS nachvollziehbar sein.

---

## 347. Validation State

Ein Modell SOLL einen Status besitzen:

```text
EXPERIMENTAL
VALIDATED
PRODUCTION
DEPRECATED
```

---

## 348. Custom Models

Benutzerdefinierte aerodynamische Modelle SOLLEN unterstützt werden können.

---

## 349. Custom Model Requirements

Ein Custom Model MUSS mindestens deklarieren:

```text
RequiredInputs
RequiredFields
Outputs
ModelConstants
ValidityRange
Capabilities
Dependencies
```

---

## 350. Serialization

Aerodynamische Modelle und Szenarien SOLLEN serialisierbar sein.

---

## 351. Serialized Configuration

Mindestens:

```text
ModelId
ModelVersion
Fluid
ReferenceState
ReferenceArea
ReferenceLength
ReferencePoint
Geometry
Mesh
BoundaryConditions
SolverSettings
TurbulenceModel
NumericalOptions
```

---

## 352. Portable Configuration

Aerodynamische Konfigurationen SOLLEN unabhängig vom Hardware-Backend gespeichert werden.

---

## 353. Result Object

Ein aerodynamisches Ergebnis SOLL mindestens enthalten können:

```text
Converged
Iterations
CL
CD
CY
CM
Force
Moment
MassBalanceError
Warnings
Runtime
```

---

## 354. Field Results

High-Fidelity-Ergebnisse SOLLEN Felder bereitstellen können:

```text
Pressure
Velocity
Density
Temperature
Mach
Vorticity
Turbulence
```

---

## 355. Surface Results

Mindestens:

```text
Cp
Cf
WallShear
HeatFlux
yPlus
```

SOLLTEN verfügbar sein können.

---

## 356. Quality Metrics

Mindestens:

```text
MassBalanceError
EnergyBalanceError
MaximumResidual
ForceConvergence
MeshQuality
yPlusQuality
```

SOLLTEN verfügbar sein.

---

## 357. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
Aerodynamic State

Pressure
Temperature
Density
Velocity

Speed of Sound
Mach Number
Reynolds Number
Dynamic Pressure

Reference Area
Reference Length
Reference Point

Angle of Attack
Sideslip Angle

Lift
Drag
Side Force

Aerodynamic Moments

CL
CD
CY
CM

Pressure Coefficient

Airfoil Interface
Wing Interface

Basic Polar Representation

Freestream Boundary

CPU Execution

Diagnostics
Profiling
Validation
Checkpoint Integration
```

---

## 358. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Incompressible CFD
Compressible CFD

Subsonic
Transonic
Supersonic
Hypersonic

Boundary Layers
Transition
Separation
Wake
Vortices

RANS
URANS
LES
DNS

Shock Waves
Expansion Waves

Airfoils
Finite Wings
Panel Methods
Vortex Lattice

Propellers
Rotors
Wind Turbines

Ground Vehicles
Trains
Buildings
Wind Engineering

Moving Geometry
6DOF
Dynamic Mesh
Overset Mesh
Sliding Mesh

Unsteady Aerodynamics
Dynamic Stall

Thermal Coupling
Aeroacoustic Coupling
Multiphase Coupling

Optimization
Adjoint
Reduced Order Models
AI Assistance

GPU
Multi-GPU
HPC
Distributed Compute

Advanced Checkpointing
Advanced Diagnostics
```

---

## 359. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-AERODYNAMICS-0001` MUSS:

1. aerodynamische Zustände repräsentieren.
2. Druck repräsentieren.
3. Temperatur repräsentieren.
4. Dichte repräsentieren.
5. Geschwindigkeit repräsentieren.
6. Schallgeschwindigkeit bestimmen können.
7. Mach-Zahlen bestimmen können.
8. Reynolds-Zahlen bestimmen können.
9. dynamischen Druck bestimmen können.
10. Referenzflächen definieren.
11. Referenzlängen definieren.
12. Referenzpunkte definieren.
13. Anstellwinkel repräsentieren.
14. Schiebewinkel repräsentieren.
15. aerodynamische Kräfte repräsentieren.
16. Auftrieb bestimmen können.
17. Widerstand bestimmen können.
18. Seitenkraft bestimmen können.
19. aerodynamische Momente repräsentieren.
20. dimensionslose Kraftkoeffizienten bestimmen können.
21. Momentenkoeffizienten bestimmen können.
22. Druckkoeffizienten bestimmen können.
23. unterschiedliche Bezugssysteme unterstützen.
24. Kräfte zwischen Bezugssystemen transformieren können.
25. Profilmodelle architektonisch unterstützen.
26. Tragflächenmodelle architektonisch unterstützen.
27. Freestream-Randbedingungen unterstützen.
28. stationäre aerodynamische Berechnungen ermöglichen.
29. Fehler und Nichtkonvergenz explizit melden.
30. CPU-Ausführung unterstützen.
31. das Compute Memory Model integrieren.
32. das Compute Object Model integrieren.
33. den Compute Scheduler integrieren.
34. Gas Dynamics Compute integrierbar halten.
35. Turbulence Compute integrierbar halten.
36. Multiphase Compute integrierbar halten.
37. GPU-Ausführung architektonisch ermöglichen.
38. HPC-Ausführung architektonisch ermöglichen.
39. Distributed Compute architektonisch ermöglichen.
40. Checkpointing integrierbar halten.
41. reproduzierbare Berechnungen unterstützen können.
42. aerodynamische Modelle vom konkreten Hardware-Backend entkoppeln.

---

## 360. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen können:

```text
NovaAerodynamicsCompute
 |
 +-- AerodynamicStateManager
 |
 +-- AtmosphereManager
 |
 +-- ReferenceStateManager
 |
 +-- ForceManager
 |
 +-- MomentManager
 |
 +-- CoefficientManager
 |
 +-- AirfoilManager
 |
 +-- WingManager
 |
 +-- BoundaryLayerManager
 |
 +-- TransitionManager
 |
 +-- WakeManager
 |
 +-- VortexManager
 |
 +-- CompressibilityManager
 |
 +-- ShockManager
 |
 +-- PropellerManager
 |
 +-- RotorManager
 |
 +-- WindTurbineManager
 |
 +-- MovingGeometryManager
 |
 +-- TurbulenceInterface
 |
 +-- GasDynamicsInterface
 |
 +-- MultiphaseInterface
 |
 +-- ThermalInterface
 |
 +-- AeroacousticsInterface
 |
 +-- AerodynamicDiagnostics
 |
 +-- AerodynamicProfiler
 |
 +-- AerodynamicValidation
```

Diese Aufteilung ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 361. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Aerodynamics Compute baut auf der allgemeinen NovaOS Compute Runtime auf.

```text
Aerodynamic Model
       |
       v
Aerodynamics Compute
       |
       v
Compute Runtime
       |
       +--> CPU
       +--> SIMD
       +--> GPU
       +--> HPC
       +--> Distributed
```

Die aerodynamische Schicht definiert die physikalische Semantik.

Die Compute Runtime übernimmt die technische Ausführung.

---

## 362. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Compute Scheduler DARF aerodynamische Workloads abhängig von Modell und Problemgröße verteilen.

Beispiele:

```text
Airfoil Polar
    |
    v
CPU

Vortex Lattice Wing
    |
    v
CPU / Multi-Core

RANS CFD
    |
    v
CPU / GPU

Large LES
    |
    v
HPC / Multi-GPU

Massive External Aerodynamics
    |
    v
Distributed Compute
```

---

## 363. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Aerodynamische Felder MÜSSEN auf das Compute Memory Model abbildbar sein.

Beispiele:

```text
PressureField
VelocityField
DensityField
TemperatureField
MachField
TurbulenceFields
SurfaceFields
ForceHistory
```

---

## 364. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Aerodynamics Compute SOLL auf dem Compute Object Model aufbauen.

Beispiele:

```text
AerodynamicState
Airfoil
Wing
Rotor
Propeller
Freestream
AerodynamicScenario
AerodynamicResult
```

---

## 365. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

Massiv parallele aerodynamische Operationen DÜRFEN über die GPU Runtime ausgeführt werden.

Insbesondere:

```text
FluxEvaluation
GradientEvaluation
ResidualAssembly
Turbulence
LinearAlgebra
ParticleTracking
PostProcessing
```

---

## 366. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Große aerodynamische Simulationen DÜRFEN über mehrere Nodes verteilt werden.

---

## 367. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

Aerodynamics Compute SOLL HPC Compute verwenden können für:

```text
Large CFD
LES
DNS
Multi-GPU
Multi-Node
Domain Decomposition
Halo Exchange
Load Balancing
Checkpointing
Profiling
```

---

## 368. Beziehung zu `NPSPEC-COMPUTE-GASDYNAMICS-0001`

Kompressible Aerodynamik SOLL Gas Dynamics Compute verwenden.

Konzeptionell:

```text
Aerodynamic Problem
       |
       v
Aerodynamics Compute
       |
       +--> Gas Dynamics Compute
       |
       v
Compressible Flow Solution
```

---

## 369. Beziehung zu `NPSPEC-COMPUTE-TURBULENCE-0001`

Turbulente aerodynamische Strömungen SOLLEN Turbulence Compute verwenden.

```text
Aerodynamics Compute
       |
       +--> Turbulence Compute
       |
       v
RANS / URANS / LES / DNS
```

---

## 370. Beziehung zu `NPSPEC-COMPUTE-MULTIPHASE-0001`

Mehrphasige aerodynamische Probleme SOLLEN Multiphase Compute verwenden.

Beispiele:

```text
Air + Rain
Air + Droplets
Air + Dust
Air + Sand
Air + IceParticles
```

---

## 371. Gemeinsame Fluid-Compute-Architektur

Aerodynamics Compute SOLL Teil der gemeinsamen NovaOS Fluid-Compute-Architektur sein:

```text
                     Nova Fluid Compute
                            |
          +-----------------+-----------------+
          |                 |                 |
          v                 v                 v
     Hydraulics        Gas Dynamics      Aerodynamics
          |                 |                 |
          +-----------------+-----------------+
                            |
          +-----------------+-----------------+
          |                                   |
          v                                   v
  Turbulence Compute                  Multiphase Compute
          |                                   |
          +-----------------+-----------------+
                            |
                            v
                         CFD Solver
```

---

## 372. Aerodynamics Fidelity Architecture

Aerodynamics Compute SOLL unterschiedliche Genauigkeits- und Kostenstufen über eine gemeinsame Architektur verfügbar machen.

```text
Aerodynamic Problem
       |
       v
Model Selection
       |
       +--> Analytical
       |
       +--> Empirical
       |
       +--> Panel
       |
       +--> Vortex Lattice
       |
       +--> RANS
       |
       +--> URANS
       |
       +--> LES
       |
       +--> DNS
       |
       v
Aerodynamic Result
```

---

## 373. Automatic Model Advisor

NovaOS DARF zukünftig einen Aerodynamics Model Advisor bereitstellen.

Eingaben können sein:

```text
Geometry
MachNumber
ReynoldsNumber
AngleOfAttack
ExpectedSeparation
ShockExpected
UnsteadyEffects
RequiredAccuracy
AvailableMemory
AvailableHardware
MaximumRuntime
```

Der Advisor DARF geeignete Modellfamilien vorschlagen.

---

## 374. Model Validity

Jedes aerodynamische Modell SOLL seinen Gültigkeitsbereich deklarieren können.

Beispiele:

```text
MachRange
ReynoldsRange
AngleOfAttackRange
GeometryRestrictions
FlowRegime
SteadyAssumption
CompressibilityAssumption
```

---

## 375. Model Selection Safety

Ein Modell außerhalb seines Gültigkeitsbereichs SOLL mindestens eine Warnung erzeugen.

---

## 376. Model Provenance

Die Herkunft eines aerodynamischen Modells MUSS nachvollziehbar sein.

---

## 377. Validation State

Ein Modell SOLL einen Status besitzen:

```text
EXPERIMENTAL
VALIDATED
PRODUCTION
DEPRECATED
```

---

## 378. Model Versioning

Änderungen an Modellgleichungen, Konstanten oder numerischer Semantik MÜSSEN versionierbar sein.

---

## 379. Backward Compatibility

Gespeicherte aerodynamische Szenarien SOLLEN mit kompatiblen neueren Implementierungen geladen werden können.

---

## 380. Model Migration

Nicht kompatible Modelländerungen SOLLEN explizite Migrationsmechanismen verwenden.

---

## 381. Result Provenance

Ein Ergebnis MUSS auf die verwendeten Modelle und relevanten Einstellungen zurückführbar sein.

---

## 382. Validation Metadata

Ein Modell DARF Referenzen auf Validierungsfälle und Genauigkeitsbereiche besitzen.

---

## 383. Engineering Safety

Aerodynamics Compute DARF Simulationsergebnisse nicht automatisch als physikalisch korrekt kennzeichnen, nur weil der Solver numerisch konvergiert ist.

---

## 384. Numerical Convergence

Numerische Konvergenz und physikalische Validität MÜSSEN getrennte Zustände sein.

---

## 385. Result Confidence

Eine erweiterte Implementierung DARF eine Ergebnisvertrauensbewertung ausgeben.

Diese SOLL auf nachvollziehbaren Kriterien basieren.

Beispiele:

```text
MeshQuality
MeshIndependence
ResidualConvergence
ForceConvergence
ModelValidity
ValidationCoverage
BoundaryConditionQuality
```

---

## 386. Engineering Workflow

Ein typischer Aerodynamics-Compute-Workflow lautet:

```text
Geometry
   |
   v
Reference Definition
   |
   v
Freestream
   |
   v
Model Selection
   |
   v
Mesh / Discretization
   |
   v
Flow Solution
   |
   +--> Turbulence
   +--> Gas Dynamics
   +--> Multiphase
   +--> Thermal
   |
   v
Convergence
   |
   v
Force Integration
   |
   v
Aerodynamic Coefficients
   |
   v
Validation
   |
   v
Result
```

---

## 387. Progressive Fidelity

NovaOS SOLL ermöglichen, dass ein aerodynamisches Problem schrittweise mit höherer Modelltreue berechnet wird.

Beispiel:

```text
Analytical Estimate
        |
        v
Panel Method
        |
        v
Vortex Lattice
        |
        v
RANS
        |
        v
URANS / LES
```

---

## 388. Result Reuse

Ergebnisse niedrigerer Fidelity-Level DÜRFEN zur Initialisierung höherer Fidelity-Level verwendet werden.

---

## 389. Compute Cost Awareness

Jedes Modell SOLL seine ungefähren Compute-Anforderungen beschreiben können.

Beispiele:

```text
CPUCost
MemoryCost
GPUCapability
ParallelEfficiency
ExpectedRuntimeClass
```

---

## 390. Scheduler Integration

Der Compute Scheduler DARF Modellinformationen zur Ressourcenwahl verwenden.

---

## 391. Resource Adaptation

Die Runtime DARF abhängig von verfügbarer Hardware zwischen kompatiblen Ausführungsstrategien wählen.

Die physikalische Modellsemantik DARF dabei nicht stillschweigend verändert werden.

---

## 392. Precision

Aerodynamics Compute SOLL mindestens unterstützen können:

```text
FP32
FP64
```

---

## 393. Mixed Precision

Geeignete Solver DÜRFEN Mixed Precision verwenden.

---

## 394. Precision Safety

Physikalisch oder numerisch kritische Operationen SOLLEN bei unzureichender Präzision automatisch auf höhere Präzision wechseln können.

---

## 395. Precision Metadata

Die verwendete numerische Präzision SOLL im Resultat nachvollziehbar sein.

---

## 396. Large-Scale Data

Große CFD-Ergebnisse SOLLEN gestreamt, partitioniert oder schrittweise gespeichert werden können.

---

## 397. Field Streaming

Felder DÜRFEN während der Berechnung an Visualisierungs- oder Analysekomponenten gestreamt werden.

---

## 398. In-Situ Analysis

Große HPC-Simulationen SOLLEN In-Situ-Auswertung unterstützen können.

---

## 399. In-Situ Quantities

Beispiele:

```text
CL
CD
CM
ShockLocation
WakeMetrics
VortexMetrics
PressureExtrema
ForceSpectrum
```

---

## 400. Output Reduction

Große Simulationen SOLLEN reduzierte Resultate speichern können, ohne sämtliche Rohfelder dauerhaft schreiben zu müssen.

---

## 401. Surface Sampling

Oberflächenwerte SOLLEN entlang definierter Linien, Flächen oder Punkte extrahiert werden können.

---

## 402. Probe

Virtuelle Messpunkte SOLLEN unterstützt werden.

---

## 403. Probe Quantities

Beispiele:

```text
Pressure
Velocity
Temperature
Mach
Density
Turbulence
```

---

## 404. Line Probe

Größen SOLLEN entlang einer Linie abgetastet werden können.

---

## 405. Surface Probe

Größen SOLLEN auf einer definierten Fläche abgetastet werden können.

---

## 406. Force Monitor

Kräfte und Momente SOLLEN während der Lösung kontinuierlich überwacht werden können.

---

## 407. Residual Monitor

Solverresiduen SOLLEN während der Berechnung verfügbar sein.

---

## 408. Event Detection

Aerodynamics Compute SOLL relevante physikalische Ereignisse erkennen können.

Beispiele:

```text
STALL
SEPARATION
SHOCK_FORMATION
SHOCK_MOVEMENT
VORTEX_SHEDDING
FLOW_REVERSAL
MASS_BALANCE_FAILURE
```

---

## 409. Event Metadata

Ein erkanntes Ereignis SOLL mindestens enthalten:

```text
Type
Time
Location
Severity
RelatedFields
```

---

## 410. Cancellation

Laufende aerodynamische Berechnungen MÜSSEN kontrolliert abbrechbar sein.

---

## 411. Graceful Cancellation

Bei kontrolliertem Abbruch SOLL optional ein Checkpoint erzeugt werden können.

---

## 412. Progress Reporting

Lange Simulationen SOLLEN Fortschritt melden.

Mindestens:

```text
CurrentIteration
CurrentTimeStep
PhysicalTime
ResidualState
EstimatedCompletion
```

soweit sinnvoll bestimmbar.

---

## 413. Result Status

Ein Resultat MUSS einen eindeutigen Status besitzen können:

```text
SUCCESS
CONVERGED
PARTIALLY_CONVERGED
NOT_CONVERGED
FAILED
CANCELLED
```

---

## 414. Partial Results

Bei Abbruch oder Fehler DÜRFEN verwertbare Teilergebnisse zurückgegeben werden.

Diese MÜSSEN eindeutig als unvollständig markiert sein.

---

## 415. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-AERODYNAMICS-STATE-0001
Aerodynamic State

NPSPEC-COMPUTE-AERODYNAMICS-ATMOSPHERE-0001
Atmosphere Model

NPSPEC-COMPUTE-AERODYNAMICS-REFERENCE-0001
Aerodynamic Reference System

NPSPEC-COMPUTE-AERODYNAMICS-FORCE-0001
Aerodynamic Forces

NPSPEC-COMPUTE-AERODYNAMICS-MOMENT-0001
Aerodynamic Moments

NPSPEC-COMPUTE-AERODYNAMICS-COEFFICIENT-0001
Aerodynamic Coefficients

NPSPEC-COMPUTE-AERODYNAMICS-AIRFOIL-0001
Airfoil Model

NPSPEC-COMPUTE-AERODYNAMICS-POLAR-0001
Airfoil Polar

NPSPEC-COMPUTE-AERODYNAMICS-WING-0001
Wing Model

NPSPEC-COMPUTE-AERODYNAMICS-LIFTINGLINE-0001
Lifting Line

NPSPEC-COMPUTE-AERODYNAMICS-VLM-0001
Vortex Lattice Method

NPSPEC-COMPUTE-AERODYNAMICS-PANEL-0001
Panel Method

NPSPEC-COMPUTE-AERODYNAMICS-BOUNDARYLAYER-0001
Boundary Layer

NPSPEC-COMPUTE-AERODYNAMICS-TRANSITION-0001
Boundary-Layer Transition

NPSPEC-COMPUTE-AERODYNAMICS-SEPARATION-0001
Flow Separation

NPSPEC-COMPUTE-AERODYNAMICS-WAKE-0001
Wake Model

NPSPEC-COMPUTE-AERODYNAMICS-VORTEX-0001
Vortex Analysis

NPSPEC-COMPUTE-AERODYNAMICS-COMPRESSIBLE-0001
Compressible Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-TRANSONIC-0001
Transonic Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-SUPERSONIC-0001
Supersonic Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-HYPERSONIC-0001
Hypersonic Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-SHOCK-0001
Shock Waves

NPSPEC-COMPUTE-AERODYNAMICS-PROPELLER-0001
Propeller Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-ROTOR-0001
Rotor Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-WINDTURBINE-0001
Wind Turbine Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-VEHICLE-0001
Vehicle Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-TRAIN-0001
Train Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-WIND-0001
Wind Engineering

NPSPEC-COMPUTE-AERODYNAMICS-INTERNAL-0001
Internal Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-MOTION-0001
Moving Geometry

NPSPEC-COMPUTE-AERODYNAMICS-6DOF-0001
Six-Degree-of-Freedom Coupling

NPSPEC-COMPUTE-AERODYNAMICS-UNSTEADY-0001
Unsteady Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-THERMAL-0001
Aerodynamic Thermal Coupling

NPSPEC-COMPUTE-AERODYNAMICS-AEROACOUSTICS-0001
Aeroacoustic Coupling

NPSPEC-COMPUTE-AERODYNAMICS-CFD-0001
Aerodynamics CFD Integration

NPSPEC-COMPUTE-AERODYNAMICS-OPTIMIZATION-0001
Aerodynamic Optimization

NPSPEC-COMPUTE-AERODYNAMICS-HPC-0001
HPC Aerodynamics

NPSPEC-COMPUTE-AERODYNAMICS-DIAG-0001
Aerodynamic Diagnostics

NPSPEC-COMPUTE-AERODYNAMICS-TEST-0001
Aerodynamics Conformance Tests
```

---

## 416. Zusammenfassung

Aerodynamics Compute stellt NovaOS eine gemeinsame physikalische und numerische Compute-Schicht für aerodynamische Berechnungen bereit.

Die grundlegende Verarbeitung lautet:

```text
Geometry
    |
    v
Freestream
    |
    v
Aerodynamic Model
    |
    +--> Analytical
    +--> Empirical
    +--> Panel
    +--> Vortex Lattice
    +--> CFD
    |
    v
Flow Solution
    |
    v
Surface Loads
    |
    v
Forces / Moments
    |
    v
Aerodynamic Coefficients
```

Zu den grundlegenden dimensionslosen Größen gehören:

```text
Mach:
M =
U / a

Reynolds:
Re =
rho * U * L / mu

Lift:
C_L =
L / (q * S_ref)

Drag:
C_D =
D / (q * S_ref)

Pressure:
C_p =
(p - p_inf) / q_inf
```

Dabei lautet der dynamische Druck:

```text
q =
1/2
*
rho
*
U^2
```

Die Modelltreue kann abhängig von Aufgabe und Ressourcen angepasst werden:

```text
Analytical
    |
    v
Panel / VLM
    |
    v
  RANS
    |
    v
  URANS
    |
    v
   LES
    |
    v
   DNS
```

Komplexe aerodynamische Probleme verwenden zusätzliche NovaOS Compute-Fähigkeiten:

```text
Aerodynamics Compute
        |
        +--> Gas Dynamics Compute
        |
        +--> Turbulence Compute
        |
        +--> Multiphase Compute
        |
        +--> Thermal Compute
        |
        v
Coupled Aerodynamic Solution
```

Die technische Ausführung bleibt von der physikalischen Modellierung getrennt:

```text
Aerodynamic Model
       |
       v
Aerodynamics Compute
       |
       v
Compute Runtime
       |
       +--> CPU
       +--> SIMD
       +--> GPU
       +--> Multi-GPU
       +--> HPC
       +--> Distributed
```

Dadurch kann dieselbe Aerodynamics-Compute-Infrastruktur für sehr unterschiedliche Anwendungen eingesetzt werden:

```text
Aircraft
Vehicles
Trains
Drones
Propellers
Rotors
Wind Turbines
Buildings
Wind Engineering
Cooling
Internal Flow
Supersonics
Hypersonics
```

Das grundlegende Architekturprinzip lautet:

> Aerodynamics Compute behandelt aerodynamische Zustände, Referenzsysteme, Profile, Tragflächen, Kräfte, Momente, Grenzschichten, Wirbel, Nachläufe, Stoßwellen, rotierende Systeme und instationäre Strömungen als wiederverwendbare, solver- und hardwareunabhängige Compute-Fähigkeiten. Ein Problem kann abhängig von Genauigkeitsanforderung und verfügbaren Ressourcen von analytischen und reduzierten Modellen bis zu RANS, LES oder DNS skaliert werden. Gas Dynamics Compute übernimmt kompressible Gasphysik, Turbulence Compute die Turbulenzmodellierung und Multiphase Compute zusätzliche disperse oder mehrphasige Strömungen. Die NovaOS Compute Runtime entscheidet über CPU-, GPU-, HPC- oder verteilte Ausführung, ohne die physikalische Semantik des gewählten Modells zu verändern.