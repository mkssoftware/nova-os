# NPSPEC-COMPUTE-HYDRAULICS-0001 — Hydraulics Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Engineering / Fluid Mechanics / Hydraulics  
**Komponente:** NovaOS Hydraulics Compute  
**Bezeichner:** `NPSPEC-COMPUTE-HYDRAULICS-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-TURBULENCE-0001`, `NPSPEC-COMPUTE-MULTIPHASE-0001`, `NPSPEC-COMPUTE-GASDYNAMICS-0001`  
**Kurzname:** Hydraulics Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Hydraulik-Compute-Schicht der NovaOS Compute-Architektur.

Hydraulics Compute stellt physikalische Modelle, numerische Primitive, Datenstrukturen und Solver-Schnittstellen für die Berechnung von Flüssigkeitsströmungen und hydraulischen Systemen bereit.

Die Komponente SOLL sowohl einfache analytische Berechnungen als auch komplexe numerische Simulationen unterstützen.

Typische Anwendungsbereiche sind:

```text
Pipe Flow
Pipe Networks
Open Channel Flow
Hydraulic Machines
Pumps
Turbines
Valves
Nozzles
Orifices
Hydraulic Cylinders
Hydraulic Actuators
Reservoirs
Tanks
Water Distribution
Wastewater Systems
Cooling Systems
Lubrication Systems
Industrial Hydraulics
Mobile Hydraulics
Process Engineering
Marine Engineering
Civil Engineering
Hydropower
CFD
Digital Twins
```

Hydraulics Compute ist keine einzelne Solver-Implementierung.

Die Komponente definiert eine gemeinsame hydraulische Modellschicht, auf der unterschiedliche Solver und Fachmodule aufbauen können.

---

## 2. Designprinzip

Das grundlegende Designprinzip lautet:

> Hydraulische Physik wird unabhängig von Solver, Diskretisierung, Hardware und Benutzeroberfläche beschrieben.

Eine Anwendung SOLL beispielsweise definieren können:

```text
Fluid = Water

Pipe:
    Length = 100 m
    Diameter = 0.1 m
    Roughness = 0.0001 m

InletPressure = 500000 Pa
OutletPressure = 100000 Pa
```

ohne selbst implementieren zu müssen:

```text
Density
Viscosity
Velocity
Reynolds Number
Friction Factor
Pressure Loss
Flow Rate
Energy Balance
```

---

## 3. Ziele

Hydraulics Compute MUSS folgende Bereiche architektonisch unterstützen:

1. Flüssigkeitseigenschaften,
2. inkompressible Strömung,
3. schwach kompressible Flüssigkeiten,
4. stationäre Strömung,
5. instationäre Strömung,
6. laminare Strömung,
7. turbulente Strömung,
8. Rohrströmung,
9. Rohrnetzwerke,
10. lokale Druckverluste,
11. offene Gerinne,
12. freie Oberflächen,
13. hydrostatische Berechnungen,
14. Pumpen,
15. Turbinen,
16. Ventile,
17. Düsen,
18. Blenden,
19. Tanks,
20. Reservoirs,
21. hydraulische Aktoren,
22. Druckspeicher,
23. Kavitation,
24. Wasserschlag,
25. Mehrphasenintegration,
26. Wärmeübertragung,
27. Turbulenzintegration,
28. CFD-Integration,
29. CPU-Ausführung,
30. GPU-Beschleunigung,
31. HPC,
32. Distributed Compute,
33. Validierung,
34. Diagnose,
35. Checkpointing,
36. reproduzierbare Berechnungen.

---

## 4. Nicht-Ziele

Diese Basisspezifikation definiert nicht vollständig:

- einen vollständigen universellen CFD-Solver,
- vollständige Gasdynamik,
- vollständige Aerodynamik,
- vollständige Verbrennungsphysik,
- vollständige Strukturmechanik,
- vollständige Fluid-Struktur-Interaktion,
- vollständige Chemie,
- konkrete CAD- oder Meshing-Systeme,
- konkrete Benutzeroberflächen.

Diese Systeme DÜRFEN Hydraulics Compute verwenden.

---

## 5. Physikalische Größen

Hydraulics Compute MUSS mindestens folgende Größen repräsentieren können:

```text
Pressure
Velocity
FlowRate
MassFlowRate
Density
DynamicViscosity
KinematicViscosity
Temperature
Elevation
HydraulicHead
Energy
Power
Force
Area
Volume
Diameter
Length
Roughness
```

---

## 6. SI-Einheiten

Die interne Referenzdarstellung SOLL SI-Einheiten verwenden.

Beispiele:

```text
Pressure            Pa
Velocity            m/s
FlowRate            m^3/s
MassFlowRate        kg/s
Density             kg/m^3
DynamicViscosity    Pa*s
KinematicViscosity  m^2/s
Length              m
Area                m^2
Volume              m^3
Power               W
Force               N
```

---

## 7. Unit Safety

Physikalisch inkompatible Einheiten DÜRFEN nicht stillschweigend miteinander verrechnet werden.

---

## 8. Fluid Descriptor

Eine Flüssigkeit SOLL mindestens folgende Eigenschaften besitzen können:

```text
FluidId
Name
DensityModel
ViscosityModel
CompressibilityModel
ThermalModel
VaporPressureModel
BulkModulusModel
```

---

## 9. Density

Die Dichte wird bezeichnet als:

```text
rho
```

Einheit:

```text
kg/m^3
```

---

## 10. Dynamic Viscosity

Die dynamische Viskosität wird bezeichnet als:

```text
mu
```

Einheit:

```text
Pa*s
```

---

## 11. Kinematic Viscosity

Die kinematische Viskosität lautet:

```text
nu = mu / rho
```

Einheit:

```text
m^2/s
```

---

## 12. Temperature Dependence

Flüssigkeitseigenschaften DÜRFEN von der Temperatur abhängen.

Beispiel:

```text
rho = rho(T)
mu  = mu(T)
```

---

## 13. Pressure Dependence

Für schwach kompressible Flüssigkeiten DÜRFEN Eigenschaften vom Druck abhängen.

---

## 14. Bulk Modulus

Die Kompressibilität einer Flüssigkeit DARF über den Kompressionsmodul beschrieben werden:

```text
K
```

---

## 15. Vapor Pressure

Der Dampfdruck:

```text
p_vapor
```

MUSS für Kavitationsmodelle verfügbar sein können.

---

## 16. Gravity

Hydraulics Compute MUSS ein Gravitationsfeld unterstützen.

Standardmäßig:

```text
g = 9.80665 m/s^2
```

Die tatsächliche Gravitation MUSS konfigurierbar sein.

---

## 17. Pressure

Druck MUSS mindestens in folgenden Formen repräsentierbar sein:

```text
AbsolutePressure
GaugePressure
DynamicPressure
StaticPressure
TotalPressure
```

---

## 18. Absolute Pressure

Absoluter Druck MUSS relativ zum physikalischen Vakuum definiert sein.

---

## 19. Gauge Pressure

Relativdruck wird gegenüber einem Referenzdruck definiert.

Konzeptionell:

```text
p_gauge =
p_absolute
-
p_reference
```

---

## 20. Hydrostatic Pressure

Für eine ruhende inkompressible Flüssigkeit gilt:

```text
dp/dz =
-rho * g
```

---

## 21. Hydrostatic Pressure Difference

Bei konstanter Dichte:

```text
Delta p =
rho * g * Delta h
```

---

## 22. Hydraulic Head

Die Druckhöhe lautet:

```text
h_p =
p / (rho * g)
```

---

## 23. Velocity Head

Die Geschwindigkeitshöhe lautet:

```text
h_v =
v^2 / (2 * g)
```

---

## 24. Elevation Head

Die geodätische Höhe lautet:

```text
z
```

---

## 25. Total Head

Die Gesamthöhe lautet konzeptionell:

```text
H =
z
+
p / (rho * g)
+
v^2 / (2 * g)
```

---

## 26. Bernoulli Equation

Für geeignete ideale Bedingungen gilt:

```text
p / (rho * g)
+
v^2 / (2 * g)
+
z
=
constant
```

---

## 27. Extended Bernoulli Equation

Reale Systeme SOLLEN zusätzliche Terme berücksichtigen können:

```text
H_1
+
H_pump
-
H_turbine
-
H_loss
=
H_2
```

---

## 28. Conservation of Mass

Hydraulische Solver MÜSSEN Massenerhaltung berücksichtigen.

---

## 29. Continuity Equation

Für inkompressible Strömung:

```text
div(U) = 0
```

---

## 30. Mass Flow Rate

Der Massenstrom lautet:

```text
m_dot =
rho * Q
```

---

## 31. Volumetric Flow Rate

Der Volumenstrom lautet:

```text
Q =
A * v
```

für eine geeignete mittlere Geschwindigkeit.

---

## 32. Cross-Sectional Area

Für ein kreisförmiges Rohr:

```text
A =
pi * D^2 / 4
```

---

## 33. Mean Velocity

Die mittlere Rohrgeschwindigkeit lautet:

```text
v =
Q / A
```

---

## 34. Reynolds Number

Die Reynolds-Zahl lautet:

```text
Re =
rho * v * D_h / mu
```

oder:

```text
Re =
v * D_h / nu
```

---

## 35. Hydraulic Diameter

Für nichtkreisförmige Querschnitte:

```text
D_h =
4 * A / P_w
```

wobei:

```text
P_w = benetzter Umfang
```

---

## 36. Flow Regime

Hydraulics Compute SOLL Strömungsregime klassifizieren können.

Mindestens:

```text
LAMINAR
TRANSITIONAL
TURBULENT
```

---

## 37. Pipe Flow Regime

Für klassische Rohrströmung DARF näherungsweise verwendet werden:

```text
Re < 2300
    -> LAMINAR

2300 <= Re <= 4000
    -> TRANSITIONAL

Re > 4000
    -> TURBULENT
```

Diese Grenzen sind modell- und problemabhängig und DÜRFEN konfigurierbar sein.

---

## 38. Darcy-Weisbach Equation

Rohrreibungsverluste SOLLEN über Darcy-Weisbach berechnet werden können:

```text
Delta p_f =
f
*
(L / D)
*
(rho * v^2 / 2)
```

---

## 39. Head Loss

Entsprechend:

```text
h_f =
f
*
(L / D)
*
(v^2 / (2 * g))
```

---

## 40. Darcy Friction Factor

Der Darcy-Reibungsbeiwert wird bezeichnet als:

```text
f
```

---

## 41. Laminar Friction Factor

Für voll entwickelte laminare Rohrströmung:

```text
f =
64 / Re
```

---

## 42. Relative Roughness

Die relative Rauheit lautet:

```text
epsilon / D
```

---

## 43. Colebrook-White Equation

Für turbulente Rohrströmung SOLL die Colebrook-White-Gleichung unterstützt werden:

```text
1 / sqrt(f)
=
-2
*
log10(
    epsilon / (3.7 * D)
    +
    2.51 / (Re * sqrt(f))
)
```

---

## 44. Explicit Friction Approximations

Alternative explizite Näherungen SOLLEN integrierbar sein.

Beispiele:

```text
Haaland
Swamee-Jain
Churchill
Custom
```

---

## 45. Friction Model Interface

Reibungsmodelle MÜSSEN austauschbar sein.

---

## 46. Local Losses

Lokale Verluste werden über Verlustbeiwerte beschrieben.

---

## 47. Minor Loss Equation

Konzeptionell:

```text
Delta p_local =
K_loss
*
rho * v^2 / 2
```

---

## 48. Minor Head Loss

```text
h_local =
K_loss
*
v^2 / (2 * g)
```

---

## 49. Loss Elements

Mindestens folgende Elemente SOLLEN modellierbar sein:

```text
Elbow
Bend
Tee
Reducer
Expansion
Contraction
Valve
Entrance
Exit
Orifice
Filter
Custom
```

---

## 50. Hydraulic Network

Hydraulics Compute SOLL hydraulische Netzwerke unterstützen.

Ein Netzwerk besteht konzeptionell aus:

```text
Nodes
Edges
Components
BoundaryConditions
```

---

## 51. Hydraulic Node

Ein Node DARF repräsentieren:

```text
Junction
Reservoir
Tank
PressureBoundary
FlowBoundary
Consumer
Source
```

---

## 52. Hydraulic Edge

Ein Edge DARF repräsentieren:

```text
Pipe
Valve
Pump
Turbine
Orifice
Restriction
HeatExchanger
CustomComponent
```

---

## 53. Node Conservation

An einem Knoten MUSS innerhalb numerischer Toleranz gelten:

```text
sum(Q_in)
-
sum(Q_out)
=
Q_source
```

---

## 54. Pressure Continuity

Geeignete Netzwerkmodelle MÜSSEN konsistente Knotendrücke bestimmen.

---

## 55. Network Solver

Netzwerksolver SOLLEN nichtlineare Gleichungssysteme lösen können.

---

## 56. Network Unknowns

Typische Unbekannte sind:

```text
NodePressure
EdgeFlowRate
TankLevel
ComponentState
```

---

## 57. Network Residual

Der Solver MUSS Residuen für Massenerhaltung und Komponentengleichungen berechnen können.

---

## 58. Hardy Cross

Das Hardy-Cross-Verfahren DARF für geeignete Netzwerke unterstützt werden.

---

## 59. Newton-Raphson

Nichtlineare hydraulische Netzwerke SOLLEN Newton-Raphson-artige Verfahren verwenden können.

---

## 60. Sparse Linear Algebra

Große Netzwerke SOLLEN Sparse-Matrix-Verfahren verwenden können.

---

## 61. Pipe Object

Ein Rohr SOLL mindestens besitzen:

```text
Length
Diameter
Roughness
Fluid
ElevationStart
ElevationEnd
LossModel
```

---

## 62. Non-Circular Pipe

Nichtkreisförmige Leitungen SOLLEN über hydraulischen Durchmesser oder spezialisierte Modelle unterstützt werden können.

---

## 63. Variable Diameter

Leitungen mit veränderlichem Querschnitt SOLLEN segmentierbar sein.

---

## 64. Pipe Segmentation

Ein Rohr DARF in mehrere numerische Segmente zerlegt werden.

---

## 65. Distributed Losses

Reibungsverluste SOLLEN entlang der Leitung verteilt berechnet werden können.

---

## 66. Elevation Change

Höhendifferenzen MÜSSEN in der hydraulischen Energiebilanz berücksichtigt werden.

---

## 67. Pipe Junction

Verzweigungen MÜSSEN Massenerhaltung gewährleisten.

---

## 68. Flow Direction

Der Solver MUSS positive und negative Strömungsrichtungen behandeln können.

---

## 69. Reverse Flow

Komponenten SOLLEN Rückströmung explizit behandeln können.

---

## 70. Zero Flow

Numerische Modelle MÜSSEN robust bei:

```text
Q -> 0
```

sein.

---

## 71. Pump

Hydraulics Compute MUSS Pumpenmodelle integrieren können.

---

## 72. Pump Head

Eine Pumpe erzeugt eine Förderhöhe:

```text
H_pump
```

---

## 73. Pump Curve

Eine Pumpenkennlinie DARF beschrieben werden als:

```text
H =
f(Q)
```

---

## 74. Pump Efficiency

Der Wirkungsgrad:

```text
eta_pump =
f(Q)
```

SOLL unterstützt werden.

---

## 75. Hydraulic Pump Power

Die hydraulische Leistung lautet:

```text
P_hyd =
rho * g * Q * H
```

---

## 76. Shaft Power

Konzeptionell:

```text
P_shaft =
P_hyd / eta_pump
```

---

## 77. Variable Speed Pump

Drehzahlgeregelte Pumpen SOLLEN unterstützt werden.

---

## 78. Affinity Laws

Für geometrisch gleiche Betriebszustände DÜRFEN Pumpen-Affinitätsgesetze verwendet werden:

```text
Q proportional n

H proportional n^2

P proportional n^3
```

---

## 79. Pump Operating Point

Der Betriebspunkt ergibt sich aus dem Schnittpunkt von:

```text
PumpCurve
SystemCurve
```

---

## 80. System Curve

Eine Systemkennlinie SOLL aus statischer Höhe und Verlusten bestimmbar sein.

---

## 81. Pump Cavitation

Pumpenmodelle SOLLEN Kavitation berücksichtigen können.

---

## 82. NPSH

Hydraulics Compute SOLL folgende Größen unterstützen:

```text
NPSH_A
NPSH_R
```

---

## 83. NPSH Safety

Ein System SOLL prüfen können:

```text
NPSH_A >= NPSH_R + SafetyMargin
```

---

## 84. Turbine

Hydraulische Turbinen SOLLEN integrierbar sein.

---

## 85. Turbine Head

Eine Turbine entnimmt hydraulische Förderhöhe beziehungsweise Energie.

---

## 86. Turbine Power

Konzeptionell:

```text
P =
rho
*
g
*
Q
*
H
*
eta
```

---

## 87. Turbine Characteristic

Turbinen DÜRFEN Kennfelder besitzen.

---

## 88. Valve

Ventile MÜSSEN als hydraulische Widerstandskomponenten modellierbar sein.

---

## 89. Valve Opening

Ein Ventil SOLL einen Öffnungsgrad besitzen können:

```text
0 <= opening <= 1
```

---

## 90. Valve Characteristic

Die Durchflusscharakteristik DARF sein:

```text
LINEAR
EQUAL_PERCENTAGE
QUICK_OPENING
CUSTOM
```

---

## 91. Valve Loss Coefficient

Der Verlustbeiwert DARF vom Öffnungsgrad abhängen:

```text
K =
f(opening)
```

---

## 92. Flow Coefficient

Ventile DÜRFEN über:

```text
Kv
Cv
```

beschrieben werden.

---

## 93. Check Valve

Rückschlagventile SOLLEN unterstützt werden.

---

## 94. Check Valve State

Mindestens:

```text
OPEN
CLOSED
TRANSITIONING
```

---

## 95. Pressure Relief Valve

Druckbegrenzungsventile SOLLEN unterstützt werden.

---

## 96. Pressure Control Valve

Druckregelventile SOLLEN unterstützt werden.

---

## 97. Flow Control Valve

Stromregelventile SOLLEN unterstützt werden.

---

## 98. Directional Valve

Wegeventile DÜRFEN als schaltbare Netzwerkkomponenten repräsentiert werden.

---

## 99. Orifice

Blenden und Öffnungen SOLLEN unterstützt werden.

---

## 100. Orifice Flow

Für geeignete inkompressible Bedingungen:

```text
Q =
C_d
*
A
*
sqrt(
    2 * Delta p / rho
)
```

---

## 101. Discharge Coefficient

Der Durchflussbeiwert lautet:

```text
C_d
```

---

## 102. Nozzle

Düsen SOLLEN als spezialisierte Strömungselemente unterstützt werden.

---

## 103. Jet Velocity

Für ideale Bedingungen DARF Torricelli verwendet werden:

```text
v =
sqrt(2 * Delta p / rho)
```

oder bei hydrostatischer Höhe:

```text
v =
sqrt(2 * g * h)
```

---

## 104. Tank

Hydraulics Compute MUSS Tanks und Behälter modellieren können.

---

## 105. Tank State

Mindestens:

```text
LiquidLevel
Volume
Pressure
Temperature
```

SOLLTEN repräsentierbar sein.

---

## 106. Tank Geometry

Mindestens:

```text
ConstantArea
Cylindrical
Spherical
CustomAreaFunction
```

SOLLTEN unterstützt werden.

---

## 107. Tank Volume

Allgemein:

```text
V =
V(h)
```

---

## 108. Tank Level Dynamics

Konzeptionell:

```text
dV/dt =
Q_in
-
Q_out
```

---

## 109. Reservoir

Ein Reservoir DARF als großer Tank mit näherungsweise konstantem Pegel modelliert werden.

---

## 110. Pressurized Tank

Druckbehälter SOLLEN unterstützt werden.

---

## 111. Gas Cushion

Ein Druckbehälter DARF eine Gasphase als kompressibles Polster verwenden.

---

## 112. Hydraulic Accumulator

Hydraulische Druckspeicher SOLLEN unterstützt werden.

---

## 113. Accumulator Types

Mindestens:

```text
BLADDER
PISTON
DIAPHRAGM
CUSTOM
```

SOLLTEN integrierbar sein.

---

## 114. Gas Law

Ein Speicher DARF eine geeignete Gasgleichung verwenden.

Beispiel:

```text
p * V^n =
constant
```

---

## 115. Hydraulic Cylinder

Hydraulikzylinder SOLLEN unterstützt werden.

---

## 116. Cylinder State

Mindestens:

```text
Position
Velocity
Acceleration
ChamberPressureA
ChamberPressureB
Force
```

---

## 117. Cylinder Force

Konzeptionell:

```text
F =
p_A * A_A
-
p_B * A_B
-
F_friction
-
F_external
```

---

## 118. Cylinder Flow

Kammerströmungen SOLLEN mit Kolbenbewegung gekoppelt werden.

---

## 119. Cylinder Volume

Kammervolumen MUSS von der Kolbenposition abhängen können.

---

## 120. Mechanical Coupling

Hydraulische Aktoren SOLLEN mit mechanischen Modellen koppelbar sein.

---

## 121. Hydraulic Motor

Hydraulikmotoren SOLLEN als Energieumwandlungskomponenten unterstützt werden.

---

## 122. Motor Torque

Konzeptionell:

```text
Torque =
Delta p
*
Displacement
*
eta_mechanical
/
(2 * pi)
```

für geeignete Definitionen.

---

## 123. Motor Speed

Drehzahl SOLL aus Volumenstrom, Verdrängung und volumetrischem Wirkungsgrad bestimmbar sein.

---

## 124. Positive Displacement Pump

Verdrängerpumpen SOLLEN unterstützt werden.

---

## 125. Pump Leakage

Interne Leckage DARF modelliert werden.

---

## 126. Volumetric Efficiency

Verdrängermaschinen SOLLEN einen volumetrischen Wirkungsgrad besitzen können.

---

## 127. Hydraulic Power

Allgemein:

```text
P_hyd =
Delta p * Q
```

---

## 128. Efficiency

Hydraulische Komponenten SOLLEN Wirkungsgrade besitzen können.

---

## 129. Energy Loss

Verlustleistung lautet:

```text
P_loss =
P_input
-
P_output
```

---

## 130. Heat Generation

Hydraulische Verluste DÜRFEN als Wärmequellen an ein thermisches Modell übergeben werden.

---

## 131. Thermal Coupling

Hydraulics Compute SOLL mit thermischen Compute-Modellen koppelbar sein.

---

## 132. Temperature-Dependent Viscosity

Die Änderung der Viskosität mit der Temperatur MUSS modellierbar sein.

---

## 133. Open Channel Flow

Hydraulics Compute SOLL offene Gerinneströmungen unterstützen.

---

## 134. Free Surface

Offene Gerinne besitzen eine freie Flüssigkeitsoberfläche.

---

## 135. Cross Section

Gerinnequerschnitte SOLLEN mindestens unterstützen:

```text
RECTANGULAR
TRAPEZOIDAL
TRIANGULAR
CIRCULAR_PARTIAL
CUSTOM
```

---

## 136. Wetted Area

Die benetzte Fläche wird bezeichnet als:

```text
A
```

---

## 137. Wetted Perimeter

Der benetzte Umfang lautet:

```text
P
```

---

## 138. Hydraulic Radius

Der hydraulische Radius lautet:

```text
R_h =
A / P
```

---

## 139. Manning Equation

Für geeignete offene Gerinne SOLL die Manning-Gleichung unterstützt werden:

```text
Q =
(1 / n)
*
A
*
R_h^(2/3)
*
S^(1/2)
```

---

## 140. Manning Roughness

Der Manning-Rauheitskoeffizient lautet:

```text
n
```

---

## 141. Chezy Equation

Alternativ DARF die Chézy-Gleichung unterstützt werden.

---

## 142. Froude Number

Für offene Gerinne:

```text
Fr =
v / sqrt(g * D_hyd)
```

mit geeigneter hydraulischer Tiefe.

---

## 143. Open Channel Regimes

Mindestens:

```text
SUBCRITICAL
CRITICAL
SUPERCRITICAL
```

SOLLTEN klassifizierbar sein.

---

## 144. Critical Flow

Kritische Strömung liegt konzeptionell bei:

```text
Fr = 1
```

---

## 145. Hydraulic Jump

Hydraulische Sprünge SOLLEN modellierbar oder diagnostizierbar sein.

---

## 146. Gradually Varied Flow

Langsam veränderliche Gerinneströmung SOLL unterstützt werden können.

---

## 147. Rapidly Varied Flow

Schnell veränderliche Strömung SOLL über geeignete Solver integrierbar sein.

---

## 148. Shallow Water Equations

Hydraulics Compute SOLL die Shallow-Water-Gleichungen architektonisch unterstützen.

---

## 149. 1D Shallow Water

Eine 1D-Implementierung SOLL möglich sein.

---

## 150. 2D Shallow Water

Eine 2D-Implementierung SOLL möglich sein.

---

## 151. Saint-Venant Equations

Offene Gerinneströmungen DÜRFEN über Saint-Venant-Gleichungen beschrieben werden.

---

## 152. Flood Simulation

Hydraulics Compute SOLL als Grundlage für Überflutungsmodelle dienen können.

---

## 153. Dam Break

Dammbruchsimulationen SOLLEN unterstützt werden können.

---

## 154. Wet-Dry Front

Shallow-Water-Solver MÜSSEN trockene und benetzte Bereiche robust behandeln können.

---

## 155. Wetting

Ein trockener Bereich DARF während der Simulation benetzt werden.

---

## 156. Drying

Ein benetzter Bereich DARF austrocknen.

---

## 157. Minimum Water Depth

Ein numerischer Mindestwasserstand DARF zur Stabilisierung verwendet werden.

---

## 158. Unsteady Pipe Flow

Hydraulics Compute SOLL instationäre Rohrströmungen unterstützen.

---

## 159. Water Hammer

Wasserschlag MUSS architektonisch unterstützt werden.

---

## 160. Pressure Wave

Schnelle Ventil- oder Pumpenänderungen können Druckwellen erzeugen.

---

## 161. Joukowsky Equation

Für einen idealisierten schnellen Geschwindigkeitswechsel:

```text
Delta p =
rho
*
a
*
Delta v
```

wobei:

```text
a = wave speed
```

---

## 162. Wave Speed

Die Druckwellengeschwindigkeit MUSS Fluid- und gegebenenfalls Rohrwandelastizität berücksichtigen können.

---

## 163. Method of Characteristics

Die Method of Characteristics SOLL als Verfahren für transiente Rohrnetzwerke integrierbar sein.

---

## 164. Valve Closure

Zeitabhängige Ventilschließung MUSS modellierbar sein.

---

## 165. Pump Trip

Pumpenausfall SOLL als transientes Ereignis modellierbar sein.

---

## 166. Surge Tank

Wasserschloss- oder Surge-Tank-Modelle SOLLEN unterstützt werden.

---

## 167. Air Vessel

Luftkessel SOLLEN als Schutzkomponenten modellierbar sein.

---

## 168. Pressure Limits

Der Solver SOLL minimale und maximale Druckwerte überwachen können.

---

## 169. Cavitation

Hydraulics Compute SOLL Kavitation unterstützen.

---

## 170. Cavitation Criterion

Ein grundlegender Indikator lautet:

```text
p_local <= p_vapor
```

---

## 171. Cavitation Number

Geeignete Modelle DÜRFEN eine Kavitationszahl verwenden.

---

## 172. Cavitation Models

Für detaillierte Kavitation SOLL `NPSPEC-COMPUTE-MULTIPHASE-0001` verwendet werden können.

---

## 173. Vapor Formation

Bei detaillierter Modellierung MUSS die Bildung einer Dampfphase möglich sein.

---

## 174. Vapor Collapse

Der Kollaps von Dampfblasen SOLL modellierbar sein.

---

## 175. Cavitation Damage Indicator

Eine erweiterte Implementierung DARF Indikatoren für potenzielle Kavitationsschädigung bereitstellen.

---

## 176. Air Entrainment

Lufteintrag in hydraulische Systeme SOLL über Multiphase Compute integrierbar sein.

---

## 177. Dissolved Gas

Gelöste Gase DÜRFEN als erweiterte Materialeigenschaft modelliert werden.

---

## 178. Free Gas

Freie Gasblasen SOLLEN über Multiphase Compute modellierbar sein.

---

## 179. Multiphase Hydraulic Flow

Hydraulics Compute MUSS mit `NPSPEC-COMPUTE-MULTIPHASE-0001` koppelbar sein.

---

## 180. Typical Multiphase Hydraulic Problems

Beispiele:

```text
Air-Water Pipe Flow
Oil-Water Flow
Cavitation
Bubble Transport
Slurry Flow
Sediment Transport
Free Surface
Hydraulic Oil with Entrained Air
```

---

## 181. Turbulence

Hydraulics Compute MUSS mit `NPSPEC-COMPUTE-TURBULENCE-0001` integrierbar sein.

---

## 182. Turbulent Pipe Flow

Turbulente Rohrströmungen DÜRFEN entweder über empirische Reibungsmodelle oder detaillierte Turbulenzmodelle berechnet werden.

---

## 183. CFD Integration

Hydraulics Compute SOLL mit einem allgemeinen CFD-Solver integrierbar sein.

---

## 184. Navier-Stokes

Für detaillierte Strömungssimulationen SOLL die inkompressible Navier-Stokes-Gleichung unterstützt werden können.

Konzeptionell:

```text
rho
*
(
    partial(U)/partial(t)
    +
    U dot grad(U)
)
=
-grad(p)
+
mu * laplacian(U)
+
rho * g
+
S
```

---

## 185. Continuity for Incompressible CFD

```text
div(U) = 0
```

---

## 186. Pressure-Velocity Coupling

Geeignete Verfahren SOLLEN integrierbar sein.

Beispiele:

```text
SIMPLE
SIMPLEC
PISO
Projection
Coupled
```

---

## 187. Spatial Discretization

Hydraulics Compute DARF mit unterschiedlichen Diskretisierungen verwendet werden:

```text
Finite Volume
Finite Difference
Finite Element
Spectral
Meshless
Custom
```

---

## 188. Solver Independence

Die hydraulische Modellschicht DARF keine bestimmte Diskretisierung voraussetzen, sofern ein Modell dies nicht zwingend erfordert.

---

## 189. Boundary Conditions

Hydraulics Compute MUSS hydraulische Randbedingungen unterstützen.

---

## 190. Pressure Boundary

Mindestens:

```text
FIXED_PRESSURE
TOTAL_PRESSURE
HYDROSTATIC_PRESSURE
```

SOLLTEN möglich sein.

---

## 191. Flow Boundary

Mindestens:

```text
FIXED_FLOW_RATE
FIXED_MASS_FLOW
FIXED_VELOCITY
```

SOLLTEN möglich sein.

---

## 192. Wall Boundary

Mindestens:

```text
NO_SLIP
SLIP
MOVING_WALL
```

SOLLTEN unterstützt werden.

---

## 193. Symmetry Boundary

Symmetrierandbedingungen SOLLEN unterstützt werden.

---

## 194. Periodic Boundary

Periodische Randbedingungen SOLLEN unterstützt werden.

---

## 195. Outlet Backflow

Rückströmung an Auslässen MUSS definiert behandelt werden.

---

## 196. Initial Conditions

Transiente hydraulische Berechnungen MÜSSEN Anfangsbedingungen unterstützen.

---

## 197. Initial Pressure

Ein initiales Druckfeld MUSS definierbar sein.

---

## 198. Initial Velocity

Ein initiales Geschwindigkeitsfeld MUSS definierbar sein.

---

## 199. Initial Flow

Netzwerke SOLLEN initiale Volumenströme besitzen können.

---

## 200. Hydrostatic Initialization

Ein Druckfeld SOLL hydrostatisch initialisiert werden können.

---

## 201. Steady State

Stationäre hydraulische Berechnungen MÜSSEN unterstützt werden.

---

## 202. Transient State

Instationäre hydraulische Berechnungen SOLLEN unterstützt werden.

---

## 203. Time Integration

Mindestens folgende Integrationsklassen SOLLEN möglich sein:

```text
Explicit
Implicit
SemiImplicit
```

---

## 204. Adaptive Time Step

Adaptive Zeitschritte SOLLEN unterstützt werden.

---

## 205. Stability Limits

Zeitabhängige Solver MÜSSEN relevante Stabilitätsbedingungen überwachen können.

---

## 206. CFL Condition

CFD- und Wellensolver SOLLEN Courant-Zahlen überwachen können.

---

## 207. Hydraulic Event

Transiente Netzwerke SOLLEN zeitabhängige Ereignisse unterstützen.

Beispiele:

```text
ValveOpen
ValveClose
PumpStart
PumpStop
PumpTrip
PressureChange
DemandChange
PipeFailure
LeakStart
```

---

## 208. Event Time

Jedes transiente Ereignis MUSS einen definierten Zeitpunkt oder Trigger besitzen.

---

## 209. Event Ordering

Gleichzeitige Ereignisse MÜSSEN deterministisch geordnet werden können.

---

## 210. Leak Model

Leckagen SOLLEN modellierbar sein.

---

## 211. Leak Flow

Ein einfaches Leckmodell DARF verwenden:

```text
Q_leak =
C
*
Delta p^n
```

---

## 212. Pipe Burst

Rohrbruch SOLL als transientes Netzwerkereignis modellierbar sein.

---

## 213. Consumer Demand

Versorgungsnetze SOLLEN zeitabhängige Verbraucherlasten besitzen können.

---

## 214. Demand Pattern

Verbrauchsmuster DÜRFEN zeitabhängig definiert werden.

---

## 215. Pressure-Dependent Demand

Verbrauch DARF vom verfügbaren Druck abhängen.

---

## 216. Control System Coupling

Hydraulische Komponenten SOLLEN mit Regelungsmodellen koppelbar sein.

---

## 217. Sensor

Virtuelle Sensoren SOLLEN Größen messen können wie:

```text
Pressure
FlowRate
Temperature
TankLevel
ValvePosition
PumpSpeed
CylinderPosition
```

---

## 218. Actuator

Steuerbare hydraulische Komponenten SOLLEN externe Sollwerte akzeptieren.

---

## 219. Closed Loop

Hydraulics Compute SOLL geschlossene Regelkreise unterstützen können.

---

## 220. Digital Twin

Hydraulics Compute SOLL für hydraulische Digital Twins verwendbar sein.

---

## 221. Real-Time Mode

Vereinfachte hydraulische Modelle SOLLEN in Echtzeit ausführbar sein können.

---

## 222. Faster-Than-Real-Time

Reduzierte Modelle DÜRFEN schneller als Echtzeit ausgeführt werden.

---

## 223. Reduced Order Models

Hydraulics Compute SOLL Reduced-Order-Modelle integrieren können.

---

## 224. Surrogate Models

Datenbasierte Ersatzmodelle DÜRFEN verwendet werden.

---

## 225. AI Assistance

AI DARF hydraulische Modelle unterstützen.

Beispiele:

```text
Parameter Estimation
Leak Detection
Pump Optimization
Anomaly Detection
Surrogate Modeling
Predictive Maintenance
Solver Acceleration
```

---

## 226. AI Physical Constraints

AI-basierte Modelle DÜRFEN physikalische Bilanzprüfungen nicht umgehen.

---

## 227. Optimization

Hydraulics Compute SOLL Optimierungsalgorithmen mit hydraulischen Modellen koppeln können.

---

## 228. Optimization Variables

Beispiele:

```text
PipeDiameter
PumpSpeed
ValvePosition
TankSize
PressureSetpoint
FlowSetpoint
```

---

## 229. Optimization Objectives

Beispiele:

```text
MinimizeEnergy
MinimizePressureLoss
MinimizeCost
MaximizeFlow
MaximizeEfficiency
MinimizeCavitationRisk
```

---

## 230. Constraints

Optimierungen SOLLEN physikalische und technische Grenzen berücksichtigen.

---

## 231. CPU Execution

Eine konforme Implementierung MUSS CPU-Ausführung unterstützen.

---

## 232. SIMD

Geeignete hydraulische Berechnungen SOLLEN SIMD verwenden können.

---

## 233. Multi-Core

Unabhängige Netzwerkbereiche, Zellen oder Szenarien SOLLEN parallel berechnet werden können.

---

## 234. GPU Execution

Geeignete hydraulische Operationen DÜRFEN über `NPSPEC-COMPUTE-RUNTIME-0005` ausgeführt werden.

---

## 235. GPU Candidates

Beispiele:

```text
CFD Field Operations
Shallow Water
Pressure Solver
Flux Evaluation
Large Parameter Sweeps
Monte Carlo Simulation
```

---

## 236. HPC Integration

Große hydraulische Simulationen SOLLEN `NPSPEC-COMPUTE-HPC-0001` verwenden können.

---

## 237. Distributed Compute

Sehr große CFD-, Flood- oder Netzwerkmodelle SOLLEN verteilt ausgeführt werden können.

---

## 238. Domain Decomposition

Räumliche hydraulische Modelle SOLLEN partitionierbar sein.

---

## 239. Network Partitioning

Große hydraulische Netzwerke DÜRFEN in Teilnetze partitioniert werden.

---

## 240. Halo Exchange

CFD- und Shallow-Water-Felder MÜSSEN an Domain-Grenzen synchronisierbar sein.

---

## 241. Load Balancing

Die Compute Runtime SOLL ungleichmäßige hydraulische Workloads berücksichtigen.

---

## 242. Scenario Parallelism

Unabhängige hydraulische Szenarien SOLLEN parallel ausgeführt werden können.

---

## 243. Parameter Sweep

Hydraulics Compute SOLL Parameterstudien effizient unterstützen können.

---

## 244. Monte Carlo

Unsicherheitsanalysen DÜRFEN Monte-Carlo-Simulationen verwenden.

---

## 245. Uncertainty Quantification

Hydraulische Ergebnisse SOLLEN Unsicherheiten berücksichtigen können.

---

## 246. Sensitivity Analysis

Parameterempfindlichkeiten SOLLEN bestimmbar sein.

---

## 247. Memory Model

Hydraulics Compute MUSS auf `NPSPEC-COMPUTE-RUNTIME-0003` abbildbar sein.

---

## 248. Typical Memory Objects

Beispiele:

```text
PressureField
VelocityField
FlowField
DensityField
ViscosityField
TemperatureField

NodeState
EdgeState
PipeState
PumpState
ValveState
TankState

ResidualVector
JacobianMatrix
SparseMatrix
SolverWorkspace
```

---

## 249. Memory Reuse

Temporäre Solver-Speicher SOLLEN wiederverwendet werden.

---

## 250. Zero-Copy

Unnötige Datenkopien zwischen hydraulischen Modellen und Compute Runtime SOLLEN vermieden werden.

---

## 251. Sparse Storage

Große Netzwerkgleichungssysteme SOLLEN Sparse-Speicher verwenden.

---

## 252. Checkpointing

Lang laufende hydraulische Simulationen MÜSSEN Checkpointing integrieren können.

---

## 253. Checkpoint Content

Je nach Modell:

```text
Time
Pressure
Velocity
FlowRate
TankLevels
PumpStates
ValveStates
CylinderStates
SolverState
EventState
Statistics
```

---

## 254. Restart

Ein kompatibler Restart MUSS den hydraulischen Zustand vollständig rekonstruieren können.

---

## 255. Model Compatibility

Beim Restart MUSS die Kompatibilität der Modellversionen geprüft werden.

---

## 256. Profiling

Hydraulics Compute MUSS Profiling unterstützen.

---

## 257. Profiling Categories

Mindestens:

```text
PropertyEvaluationTime
NetworkAssemblyTime
NetworkSolveTime
PipeFlowTime
PumpEvaluationTime
ValveEvaluationTime
TransientTime
CFDTime
CommunicationTime
```

---

## 258. Performance Metrics

Mindestens:

```text
NodesPerSecond
EdgesPerSecond
CellsPerSecond
TimeStepsPerSecond
IterationsPerSecond
MemoryUsage
CommunicationFraction
```

---

## 259. Diagnostics

Hydraulics Compute MUSS physikalische und numerische Diagnosen bereitstellen.

---

## 260. Diagnostic Classes

Mindestens:

```text
INVALID_FLUID_PROPERTY
NEGATIVE_ABSOLUTE_PRESSURE
INVALID_DIAMETER
INVALID_AREA
INVALID_ROUGHNESS
INVALID_FLOW
MASS_BALANCE_ERROR
ENERGY_BALANCE_ERROR
PRESSURE_LIMIT_EXCEEDED
CAVITATION_WARNING
NPSH_WARNING
FLOW_REVERSAL
SOLVER_DIVERGENCE
SOLVER_NOT_CONVERGED
UNSTABLE_TIME_STEP
```

---

## 261. Invalid Diameter

Ein Rohrdurchmesser MUSS größer als null sein.

---

## 262. Invalid Length

Eine physikalische Rohrlänge DARF nicht negativ sein.

---

## 263. Invalid Density

Die Flüssigkeitsdichte MUSS physikalisch gültig sein.

---

## 264. Invalid Viscosity

Die Viskosität MUSS innerhalb des verwendeten Modells gültig sein.

---

## 265. Negative Absolute Pressure

Unphysikalisch negativer Absolutdruck MUSS als Fehler erkannt werden.

---

## 266. Vapor Pressure Warning

Druck unter oder nahe Dampfdruck SOLL als potenzielles Kavitationsereignis markiert werden.

---

## 267. Mass Balance

Netzwerk- und CFD-Modelle MÜSSEN Massenbilanzfehler bestimmen können.

---

## 268. Energy Balance

Geeignete Modelle SOLLEN Energiebilanzen bereitstellen.

---

## 269. Network Connectivity

Hydraulische Netzwerke MÜSSEN auf ungültige Topologien geprüft werden können.

---

## 270. Disconnected Component

Unverbundene Komponenten SOLLEN diagnostiziert werden.

---

## 271. Missing Boundary Condition

Unterbestimmte hydraulische Systeme MÜSSEN erkannt werden.

---

## 272. Over-Constrained System

Überbestimmte Systeme SOLLEN erkannt werden.

---

## 273. Solver Convergence

Nichtlineare Solver MÜSSEN Konvergenzmetriken bereitstellen.

---

## 274. Residual

Mindestens:

```text
MassResidual
PressureResidual
FlowResidual
EnergyResidual
```

SOLLTEN verfügbar sein.

---

## 275. Convergence Tolerance

Konvergenztoleranzen MÜSSEN konfigurierbar sein.

---

## 276. Maximum Iterations

Eine maximale Iterationsanzahl MUSS definierbar sein.

---

## 277. Non-Convergence

Nichtkonvergenz DARF nicht stillschweigend als gültige Lösung behandelt werden.

---

## 278. Error Model

Mindestens:

```text
INVALID_MODEL
INVALID_FLUID
INVALID_PROPERTY
INVALID_COMPONENT
INVALID_NETWORK
INVALID_BOUNDARY_CONDITION
INVALID_INITIAL_CONDITION
UNDER_CONSTRAINED
OVER_CONSTRAINED
SOLVER_DIVERGED
SOLVER_NOT_CONVERGED
NUMERIC_FAILURE
CAVITATION_FAILURE
OUT_OF_MEMORY
DEVICE_FAILURE
COMMUNICATION_FAILURE
CHECKPOINT_FAILURE
CANCELLED
```

---

## 279. Deterministic Mode

Hydraulics Compute SOLL einen deterministischen Modus unterstützen können.

---

## 280. Floating-Point Reproducibility

Deterministische Ausführung SOLL definierte Reduktionsreihenfolgen verwenden können.

---

## 281. Reproducibility Metadata

Ein Ergebnis SOLL mindestens referenzieren:

```text
HydraulicModel
ModelVersion
FluidModel
Solver
SolverVersion
BoundaryConditions
RuntimeVersion
HardwareBackend
Tolerance
```

---

## 282. Validation

Hydraulische Modelle MÜSSEN gegen geeignete Referenzfälle validierbar sein.

---

## 283. Validation Classes

Mindestens:

```text
Analytical
Experimental
Benchmark
Cross-Solver
Regression
ManufacturedSolution
```

---

## 284. Poiseuille Flow

Laminare Rohrströmung SOLL gegen Hagen-Poiseuille validierbar sein.

---

## 285. Hagen-Poiseuille

Für laminare Strömung in einem kreisförmigen Rohr gilt:

```text
Q =
pi
*
Delta p
*
R^4
/
(8 * mu * L)
```

---

## 286. Darcy-Weisbach Validation

Turbulente Rohrverluste SOLLEN gegen definierte Referenzwerte geprüft werden.

---

## 287. Bernoulli Validation

Ideale Energiegleichungen SOLLEN gegen analytische Fälle getestet werden.

---

## 288. Hydrostatic Validation

Hydrostatische Druckverteilungen MÜSSEN analytisch validiert werden.

---

## 289. Orifice Validation

Blendenströmungen SOLLEN gegen definierte Durchflusskoeffizienten geprüft werden.

---

## 290. Pump Validation

Pumpenmodelle SOLLEN Kennlinien reproduzieren.

---

## 291. Valve Validation

Ventilmodelle SOLLEN definierte Druckverlustkennlinien reproduzieren.

---

## 292. Tank Validation

Tankfüllung und -entleerung SOLLEN gegen analytische oder Referenzlösungen geprüft werden.

---

## 293. Water Hammer Validation

Wasserschlagmodelle SOLLEN gegen Joukowsky- oder geeignete Referenzfälle geprüft werden.

---

## 294. Open Channel Validation

Gerinneströmung SOLL gegen Manning-, Chézy- oder experimentelle Referenzfälle geprüft werden.

---

## 295. Dam Break Validation

Shallow-Water-Solver SOLLEN gegen etablierte Dammbruch-Benchmarks geprüft werden.

---

## 296. Testing — Fluid Properties

Mindestens:

```text
Constant Density
Constant Viscosity
Temperature-Dependent Density
Temperature-Dependent Viscosity
Invalid Density
Invalid Viscosity
```

---

## 297. Testing — Hydrostatics

Mindestens:

```text
Constant Density
Vertical Column
Different Elevations
Reference Pressure
```

---

## 298. Testing — Pipe Flow

Mindestens:

```text
Laminar Flow
Transitional Flow
Turbulent Flow
Smooth Pipe
Rough Pipe
Zero Flow
Reverse Flow
```

---

## 299. Testing — Friction

Mindestens:

```text
64/Re
Colebrook-White
Explicit Approximation
Relative Roughness
```

---

## 300. Testing — Local Losses

Mindestens:

```text
Elbow
Valve
Expansion
Contraction
Entrance
Exit
```

---

## 301. Testing — Networks

Mindestens:

```text
Single Pipe
Series Pipes
Parallel Pipes
Branch Network
Loop Network
Multiple Sources
Multiple Consumers
```

---

## 302. Testing — Pumps

Mindestens:

```text
Pump Curve
Operating Point
Efficiency
Variable Speed
Reverse Flow
Pump Off
```

---

## 303. Testing — Valves

Mindestens:

```text
Fully Open
Partially Open
Closed
Control Valve
Check Valve
```

---

## 304. Testing — Tanks

Mindestens:

```text
Fill
Drain
Steady Level
Overflow
Empty Tank
```

---

## 305. Testing — Hydraulic Cylinders

Mindestens:

```text
Static Force
Extension
Retraction
Pressure Difference
External Load
```

---

## 306. Testing — Open Channels

Mindestens:

```text
Subcritical
Critical
Supercritical
Hydraulic Jump
```

---

## 307. Testing — Shallow Water

Mindestens:

```text
Still Water
Wet-Dry Front
Dam Break
Flow over Terrain
Mass Conservation
```

---

## 308. Testing — Water Hammer

Mindestens:

```text
Instant Valve Closure
Slow Valve Closure
Pump Trip
Pressure Wave Reflection
```

---

## 309. Testing — Cavitation

Mindestens:

```text
Pressure Above Vapor Pressure
Pressure Near Vapor Pressure
Pressure Below Vapor Pressure
Recovery
```

---

## 310. Testing — Mass Conservation

Jedes Solvermodell MUSS auf Massenbilanz geprüft werden.

---

## 311. Testing — Energy Conservation

Geeignete Modelle SOLLEN auf Energiebilanz geprüft werden.

---

## 312. Testing — CPU

Alle Mindestfunktionen MÜSSEN auf CPU getestet werden.

---

## 313. Testing — GPU

GPU-Ergebnisse SOLLEN innerhalb definierter Toleranzen mit CPU-Referenzen übereinstimmen.

---

## 314. Testing — Multi-Core

Mehrkernberechnung MUSS numerisch konsistent sein.

---

## 315. Testing — Distributed

Verteilte Berechnungen SOLLEN gegen äquivalente lokale Berechnungen geprüft werden.

---

## 316. Testing — Checkpoint

Mindestens:

```text
Save
Restart
Network State Restore
Tank State Restore
Transient State Restore
```

---

## 317. Testing — Determinism

Deterministische Modi MÜSSEN reproduzierbare Ergebnisse liefern können.

---

## 318. Testing — Performance

Mindestens:

```text
Small Network
Large Network
Large CFD Mesh
Large Shallow Water Grid
Parameter Sweep
```

---

## 319. Stress Tests

Mindestens:

```text
Millions of Network Elements
Large Pipe Networks
Large Flood Domain
Long Transient Simulation
Rapid Valve Switching
Many Pumps
Many Control Valves
Multi-GPU
High Node Count
Memory Pressure
```

---

## 320. Fault Injection

Mindestens:

```text
OUT_OF_MEMORY
GPU_FAILURE
NODE_FAILURE
COMMUNICATION_FAILURE
CHECKPOINT_FAILURE
SOLVER_FAILURE
INVALID_COMPONENT_STATE
```

---

## 321. Regression Testing

Änderungen an hydraulischen Modellen MÜSSEN gegen definierte Referenzfälle geprüft werden.

---

## 322. Regression Quantities

Mindestens:

```text
Pressure
FlowRate
Velocity
HeadLoss
TankLevel
PumpOperatingPoint
ValveFlow
MassBalance
EnergyBalance
```

---

## 323. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
Fluid Descriptor

Density
Dynamic Viscosity
Kinematic Viscosity

Pressure
Flow Rate
Velocity

Hydrostatic Pressure
Hydraulic Head
Bernoulli Equation

Reynolds Number

Pipe Flow
Darcy-Weisbach

Laminar Friction Factor
Turbulent Friction Model

Local Losses

Hydraulic Nodes
Hydraulic Edges
Basic Pipe Networks

Mass Conservation

Pump Interface
Valve Interface
Tank Interface

Steady-State Calculation

CPU Execution

Diagnostics
Profiling
Validation
Checkpoint Integration
```

---

## 324. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Temperature-Dependent Fluids
Weakly Compressible Liquids

Large Pipe Networks
Sparse Network Solver

Pump Maps
Variable-Speed Pumps

Hydraulic Turbines
Advanced Valves

Hydraulic Cylinders
Hydraulic Motors
Accumulators

Open Channel Flow
Manning
Chezy
Saint-Venant
Shallow Water

Flood Simulation
Dam Break
Wet-Dry Front

Transient Pipe Flow
Water Hammer
Method of Characteristics

Cavitation
Air Entrainment

Multiphase Hydraulics
Turbulence Coupling

CFD Integration

Thermal Coupling
Mechanical Coupling
Control-System Coupling

Digital Twins
Real-Time Models
Reduced-Order Models

Optimization
Uncertainty Quantification

GPU
Multi-GPU
HPC
Distributed Compute

Advanced Checkpointing
Advanced Diagnostics
```

---

## 325. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-HYDRAULICS-0001` MUSS:

1. Flüssigkeiten als eigenständige physikalische Objekte repräsentieren.
2. Dichte repräsentieren.
3. dynamische Viskosität repräsentieren.
4. kinematische Viskosität bestimmen können.
5. Druck repräsentieren.
6. Volumenstrom repräsentieren.
7. Massenstrom bestimmen können.
8. Geschwindigkeit repräsentieren.
9. hydrostatischen Druck bestimmen können.
10. hydraulische Höhe repräsentieren.
11. Massenerhaltung berücksichtigen.
12. Reynolds-Zahlen bestimmen können.
13. laminare und turbulente Rohrströmung unterscheiden können.
14. Darcy-Weisbach-Verluste berechnen können.
15. mindestens ein turbulentes Reibungsmodell bereitstellen.
16. lokale Verluste unterstützen.
17. Rohrleitungen repräsentieren.
18. hydraulische Knoten repräsentieren.
19. hydraulische Netzwerke repräsentieren.
20. Knotenmassenbilanzen prüfen.
21. Pumpen architektonisch unterstützen.
22. Ventile architektonisch unterstützen.
23. Tanks architektonisch unterstützen.
24. stationäre hydraulische Berechnungen ermöglichen.
25. Fehler und Nichtkonvergenz explizit melden.
26. physikalisch ungültige Zustände diagnostizieren.
27. CPU-Ausführung unterstützen.
28. das Compute Memory Model integrieren.
29. das Compute Object Model integrieren.
30. den Compute Scheduler integrieren.
31. GPU-Ausführung architektonisch ermöglichen.
32. HPC-Ausführung architektonisch ermöglichen.
33. Distributed Compute architektonisch ermöglichen.
34. Turbulence Compute integrierbar halten.
35. Multiphase Compute integrierbar halten.
36. Checkpointing integrierbar halten.
37. reproduzierbare Berechnungen unterstützen können.
38. hydraulische Modelle vom konkreten Hardware-Backend entkoppeln.

---

## 326. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen können:

```text
NovaHydraulicsCompute
 |
 +-- FluidPropertyManager
 |
 +-- HydrostaticsEngine
 |
 +-- PipeFlowEngine
 |
 +-- FrictionModelManager
 |
 +-- LossModelManager
 |
 +-- HydraulicNetwork
 |
 +-- NetworkSolver
 |
 +-- PumpModelManager
 |
 +-- TurbineModelManager
 |
 +-- ValveModelManager
 |
 +-- TankModelManager
 |
 +-- AccumulatorModelManager
 |
 +-- HydraulicActuatorManager
 |
 +-- OpenChannelEngine
 |
 +-- ShallowWaterEngine
 |
 +-- TransientHydraulicsEngine
 |
 +-- WaterHammerEngine
 |
 +-- CavitationInterface
 |
 +-- MultiphaseInterface
 |
 +-- TurbulenceInterface
 |
 +-- HydraulicDiagnostics
 |
 +-- HydraulicProfiler
 |
 +-- HydraulicValidation
```

Diese Aufteilung ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 327. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Hydraulics Compute baut auf der allgemeinen NovaOS Compute Runtime auf.

```text
Hydraulic Model
      |
      v
Hydraulics Compute
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

Die hydraulische Schicht definiert die physikalische Semantik.

Die Compute Runtime übernimmt die technische Ausführung.

---

## 328. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Compute Scheduler DARF hydraulische Workloads abhängig von Problemgröße und Modell verteilen.

Beispiele:

```text
Simple Pipe Calculation
    |
    v
CPU

Large Pipe Network
    |
    v
Multi-Core CPU

Shallow Water
    |
    v
GPU

Large CFD Hydraulics
    |
    v
GPU / HPC

Large Flood Simulation
    |
    v
Distributed Compute
```

---

## 329. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Hydraulische Felder und Netzwerkdaten MÜSSEN auf das Compute Memory Model abbildbar sein.

Beispiele:

```text
PressureField
VelocityField
FlowField

NodeArray
EdgeArray

PumpState
ValveState
TankState

SparseMatrix
ResidualVector
```

---

## 330. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Hydraulics Compute SOLL auf dem Compute Object Model aufbauen.

Beispiele:

```text
Fluid
Pipe
Node
Pump
Valve
Tank
Cylinder
HydraulicNetwork
HydraulicScenario
HydraulicResult
```

---

## 331. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

Massiv parallele hydraulische Berechnungen DÜRFEN über die GPU Runtime ausgeführt werden.

Insbesondere:

```text
CFD Field Operations
Shallow Water
Flood Simulation
Parameter Sweeps
```

---

## 332. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Große hydraulische Modelle DÜRFEN über mehrere Nodes verteilt werden.

---

## 333. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

Hydraulics Compute SOLL HPC Compute verwenden können für:

```text
Large CFD
Large Networks
Flood Simulation
Domain Decomposition
Multi-GPU
Multi-Node
Sparse Solvers
Checkpointing
Profiling
```

---

## 334. Beziehung zu `NPSPEC-COMPUTE-TURBULENCE-0001`

Detaillierte turbulente Flüssigkeitsströmungen SOLLEN Turbulence Compute verwenden können.

Konzeptionell:

```text
Hydraulic Fluid Model
        |
        v
Hydraulics Compute
        |
        +--> Turbulence Compute
        |
        v
Flow Solver
```

---

## 335. Beziehung zu `NPSPEC-COMPUTE-MULTIPHASE-0001`

Mehrphasige hydraulische Systeme SOLLEN Multiphase Compute verwenden.

Beispiele:

```text
Water + Air
Oil + Air
Water + Vapor
Liquid + Particles
Slurry
```

---

## 336. Gemeinsame Fluid-Compute-Architektur

Hydraulics Compute SOLL Teil einer gemeinsamen Fluid-Compute-Architektur sein:

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

## 337. Modellhierarchie

Eine mögliche hydraulische Modellhierarchie lautet:

```text
Hydraulics
 |
 +-- Hydrostatics
 |
 +-- Internal Flow
 |    |
 |    +-- Pipe Flow
 |    +-- Network Flow
 |    +-- Components
 |
 +-- Hydraulic Machines
 |    |
 |    +-- Pumps
 |    +-- Turbines
 |    +-- Motors
 |
 +-- Hydraulic Actuation
 |    |
 |    +-- Cylinders
 |    +-- Accumulators
 |
 +-- Open Channel
 |    |
 |    +-- Manning
 |    +-- Saint-Venant
 |    +-- Shallow Water
 |
 +-- Transient Hydraulics
 |    |
 |    +-- Water Hammer
 |    +-- Surge
 |
 +-- Cavitation
 |
 +-- Multiphase
 |
 +-- CFD
```

---

## 338. Modellwahl

Die Modellwahl MUSS von Problem, Skala und benötigter Genauigkeit abhängen.

Beispiele:

```text
Single Pipe
    -> Analytical / 1D Pipe Model

Large Water Network
    -> Hydraulic Network Solver

Water Hammer
    -> Transient 1D / Method of Characteristics

River
    -> Open Channel / Shallow Water

Pump Internal Flow
    -> CFD

Cavitation in Pump
    -> CFD + Multiphase

Hydraulic Cylinder
    -> Lumped Hydraulic Model
```

Diese Zuordnung ist eine Orientierung und keine universelle Modellgarantie.

---

## 339. Automatic Model Advisor

NovaOS DARF zukünftig einen Hydraulics Model Advisor bereitstellen.

Eingaben können sein:

```text
Geometry
Fluid
PressureRange
FlowRange
MachNumber
ReynoldsNumber
FreeSurface
CavitationRisk
TransientBehavior
RequiredAccuracy
AvailableHardware
```

Der Advisor DARF geeignete Modellfamilien vorschlagen.

---

## 340. Model Validity

Jedes hydraulische Modell SOLL seinen Gültigkeitsbereich deklarieren können.

Beispiele:

```text
ReynoldsRange
PressureRange
TemperatureRange
GeometryRestrictions
CompressibilityAssumptions
SteadyStateAssumption
```

---

## 341. Model Provenance

Die Herkunft eines hydraulischen Modells MUSS nachvollziehbar sein.

---

## 342. Validation State

Ein Modell SOLL einen Status besitzen:

```text
EXPERIMENTAL
VALIDATED
PRODUCTION
DEPRECATED
```

---

## 343. Custom Models

Benutzerdefinierte hydraulische Modelle SOLLEN unterstützt werden können.

---

## 344. Custom Model Requirements

Ein Custom Model MUSS mindestens deklarieren:

```text
RequiredInputs
Outputs
StateVariables
Equations
ModelConstants
ValidityRange
Capabilities
```

---

## 345. Serialization

Hydraulische Modelle und Szenarien SOLLEN serialisierbar sein.

---

## 346. Serialized Configuration

Mindestens:

```text
ModelId
ModelVersion
Fluid
Components
NetworkTopology
BoundaryConditions
InitialConditions
SolverSettings
NumericalOptions
```

---

## 347. Portable Configuration

Hydraulische Konfigurationen SOLLEN unabhängig vom Hardware-Backend gespeichert werden können.

---

## 348. Result Metadata

Ein hydraulisches Ergebnis SOLL mindestens enthalten können:

```text
Model
ModelVersion
Fluid
Solver
Converged
Iterations
MassBalanceError
EnergyBalanceError
Warnings
Runtime
```

---

## 349. Quality Metrics

Mindestens:

```text
MassBalanceError
EnergyBalanceError
MaximumResidual
PressureRange
FlowRange
ConvergenceRate
```

SOLLTEN verfügbar sein.

---

## 350. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-HYDRAULICS-FLUID-0001
Hydraulic Fluid Model

NPSPEC-COMPUTE-HYDRAULICS-HYDROSTATICS-0001
Hydrostatics

NPSPEC-COMPUTE-HYDRAULICS-PIPE-0001
Pipe Flow

NPSPEC-COMPUTE-HYDRAULICS-FRICTION-0001
Pipe Friction Models

NPSPEC-COMPUTE-HYDRAULICS-LOSS-0001
Local Loss Models

NPSPEC-COMPUTE-HYDRAULICS-NETWORK-0001
Hydraulic Network

NPSPEC-COMPUTE-HYDRAULICS-NETWORKSOLVER-0001
Hydraulic Network Solver

NPSPEC-COMPUTE-HYDRAULICS-PUMP-0001
Pump Models

NPSPEC-COMPUTE-HYDRAULICS-TURBINE-0001
Hydraulic Turbine Models

NPSPEC-COMPUTE-HYDRAULICS-VALVE-0001
Valve Models

NPSPEC-COMPUTE-HYDRAULICS-ORIFICE-0001
Orifice and Nozzle Models

NPSPEC-COMPUTE-HYDRAULICS-TANK-0001
Tank and Reservoir Models

NPSPEC-COMPUTE-HYDRAULICS-ACCUMULATOR-0001
Hydraulic Accumulator

NPSPEC-COMPUTE-HYDRAULICS-CYLINDER-0001
Hydraulic Cylinder

NPSPEC-COMPUTE-HYDRAULICS-MOTOR-0001
Hydraulic Motor

NPSPEC-COMPUTE-HYDRAULICS-OPENCHANNEL-0001
Open Channel Flow

NPSPEC-COMPUTE-HYDRAULICS-SHALLOWWATER-0001
Shallow Water Runtime

NPSPEC-COMPUTE-HYDRAULICS-FLOOD-0001
Flood Simulation

NPSPEC-COMPUTE-HYDRAULICS-TRANSIENT-0001
Transient Hydraulics

NPSPEC-COMPUTE-HYDRAULICS-WATERHAMMER-0001
Water Hammer

NPSPEC-COMPUTE-HYDRAULICS-CAVITATION-0001
Hydraulic Cavitation

NPSPEC-COMPUTE-HYDRAULICS-THERMAL-0001
Thermal Hydraulics

NPSPEC-COMPUTE-HYDRAULICS-CONTROL-0001
Hydraulic Control Systems

NPSPEC-COMPUTE-HYDRAULICS-DIGITALTWIN-0001
Hydraulic Digital Twin

NPSPEC-COMPUTE-HYDRAULICS-CFD-0001
Hydraulic CFD Integration

NPSPEC-COMPUTE-HYDRAULICS-HPC-0001
HPC Hydraulics

NPSPEC-COMPUTE-HYDRAULICS-DIAG-0001
Hydraulic Diagnostics

NPSPEC-COMPUTE-HYDRAULICS-TEST-0001
Hydraulics Conformance Tests
```

---

## 351. Zusammenfassung

Hydraulics Compute stellt NovaOS eine gemeinsame physikalische und numerische Compute-Schicht für Flüssigkeitsströmungen und hydraulische Systeme bereit.

Die grundlegende Verarbeitung lautet:

```text
Fluid Properties
      |
      v
Hydraulic Model
      |
      +--> Hydrostatics
      +--> Pipe Flow
      +--> Networks
      +--> Pumps
      +--> Valves
      +--> Tanks
      +--> Open Channels
      +--> Transients
      |
      v
Hydraulic Solver
      |
      v
Pressure / Flow / Velocity / State
```

Für Rohrströmungen bilden Größen wie:

```text
rho
mu
Q
v
Re
f
Delta p
```

die grundlegenden hydraulischen Zustands- und Berechnungsgrößen.

Der Volumenstrom lautet:

```text
Q =
A * v
```

Die Reynolds-Zahl:

```text
Re =
rho * v * D_h / mu
```

und der Rohrreibungsverlust nach Darcy-Weisbach:

```text
Delta p =
f
*
(L / D)
*
rho * v^2 / 2
```

Hydraulische Netzwerke werden konzeptionell als Graph behandelt:

```text
Reservoir
    |
    v
  Pump
    |
    v
  Pipe
    |
    v
 Junction
   /   \
  v     v
Pipe   Valve
 |       |
 v       v
Tank   Consumer
```

An jedem Knoten muss die Massenerhaltung erfüllt sein:

```text
sum(Q_in)
-
sum(Q_out)
=
Q_source
```

Für transiente hydraulische Systeme erweitert sich die Architektur um zeitabhängige Zustände:

```text
Hydraulic Network
       |
       +--> Valve Events
       +--> Pump Events
       +--> Tank Dynamics
       +--> Pressure Waves
       |
       v
Transient Hydraulics
       |
       v
Water Hammer / Surge
```

Mehrphasige oder turbulente Probleme verwenden zusätzliche Compute-Fähigkeiten:

```text
Hydraulics Compute
       |
       +--> Turbulence Compute
       |
       +--> Multiphase Compute
       |
       v
Coupled Hydraulic Solution
```

Die physikalische Modellierung bleibt von der Hardware getrennt:

```text
Hydraulic Model
       |
       v
Hydraulics Compute
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

Das grundlegende Architekturprinzip lautet:

> Hydraulics Compute behandelt Flüssigkeitseigenschaften, hydrostatische Zustände, Rohrströmungen, Druckverluste, hydraulische Netzwerke, Pumpen, Ventile, Tanks, Aktoren, offene Gerinne und transiente hydraulische Vorgänge als wiederverwendbare, solver- und hardwareunabhängige Compute-Fähigkeiten. Einfache Probleme können analytisch oder als 1D-Netzwerk gelöst werden, während komplexe Strömungen auf CFD, Turbulence Compute, Multiphase Compute, GPU, HPC und Distributed Compute erweitert werden können. Physikalische Bilanzprüfungen, Modellgültigkeit, Diagnose, Reproduzierbarkeit und Validierung bleiben dabei integrale Bestandteile jeder Berechnung.