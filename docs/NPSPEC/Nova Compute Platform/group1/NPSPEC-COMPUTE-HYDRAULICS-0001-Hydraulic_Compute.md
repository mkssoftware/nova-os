# NPSPEC-COMPUTE-HYDRAULICS-0001 — Hydraulic Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Specialized Compute  
**Komponente:** NovaOS Hydraulic Compute  
**Bezeichner:** `NPSPEC-COMPUTE-HYDRAULICS-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Kurzname:** Hydraulic Compute  

---

## 1. Zweck

Diese Spezifikation definiert **Hydraulic Compute** innerhalb der NovaOS Compute-Architektur.

Hydraulic Compute stellt standardisierte Rechenprimitive, Datenmodelle und Solver-Schnittstellen für hydraulische und fluidtechnische Berechnungen bereit.

Die Komponente ist für Anwendungen vorgesehen, die physikalische Systeme mit Flüssigkeiten, Drücken, Volumenströmen, hydraulischen Widerständen, Pumpen, Ventilen, Leitungen, Speichern, Zylindern oder hydraulischen Netzwerken modellieren oder berechnen.

Typische Einsatzgebiete sind:

```text
Hydraulic Engineering
Fluid Power
Pipe Networks
Water Distribution
Industrial Hydraulics
Mobile Hydraulics
Building Services
Process Engineering
Simulation
Digital Twins
Control Engineering
Diagnostics
Predictive Maintenance
```

Hydraulic Compute SOLL keine einzelne Fachanwendung darstellen.

Die Komponente stellt vielmehr gemeinsame hydraulische Rechenfähigkeiten bereit, die von unterschiedlichen NovaOS-Fähigkeiten und Anwendungen verwendet werden können.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Hydraulische Berechnungen werden als wiederverwendbare Compute-Fähigkeiten des Systems bereitgestellt und nicht in jeder Anwendung separat implementiert.

Eine Anwendung SOLL beispielsweise keine eigene Implementierung für:

```text
Reynolds Number
Pressure Loss
Pipe Resistance
Pump Curves
Valve Flow
Network Balancing
```

benötigen, wenn diese Berechnung bereits durch Hydraulic Compute bereitgestellt wird.

---

## 3. Ziele

Hydraulic Compute MUSS folgende Ziele verfolgen:

1. einheitliches hydraulisches Datenmodell,
2. standardisierte physikalische Größen,
3. SI-basierte interne Berechnung,
4. sichere Einheitenkonvertierung,
5. hydraulische Netzwerkberechnung,
6. stationäre Solver,
7. Erweiterbarkeit für transiente Solver,
8. Pumpenmodelle,
9. Ventilmodelle,
10. Rohrleitungsmodelle,
11. Speicher- und Tankmodelle,
12. hydraulische Aktormodelle,
13. Fluidmodelle,
14. Verlustmodelle,
15. Kavitationserkennung,
16. Diagnosefunktionen,
17. CPU/GPU-Beschleunigung,
18. deterministische Berechnungsmodi,
19. reproduzierbare Ergebnisse,
20. Erweiterbarkeit für zukünftige physikalische Modelle.

---

## 4. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- Computational Fluid Dynamics,
- vollständige 3D-Strömungssimulation,
- Turbulenzmodelle für allgemeine CFD,
- Finite-Elemente-Mechanik,
- thermodynamische Mehrphasensimulation,
- konkrete Maschinensteuerungen,
- konkrete SPS-Programme,
- eine hydraulische CAD-Anwendung.

Diese Funktionen DÜRFEN auf Hydraulic Compute aufbauen.

---

## 5. Architektur

Die grundlegende Architektur lautet:

```text
Application / Capability
        |
        v
Nova Hydraulic API
        |
        v
Hydraulic Compute
        |
        +-- Unit System
        |
        +-- Fluid Model
        |
        +-- Component Models
        |
        +-- Network Model
        |
        +-- Equation Builder
        |
        +-- Hydraulic Solver
        |
        +-- Diagnostics
        |
        v
Nova Compute Runtime
        |
        +-- CPU
        |
        +-- GPU
        |
        +-- Distributed Compute
```

---

## 6. Physikalische Größen

Hydraulic Compute MUSS mindestens folgende physikalische Größen repräsentieren können:

```text
Pressure
AbsolutePressure
GaugePressure
DifferentialPressure

Volume
VolumetricFlowRate
MassFlowRate

Density
DynamicViscosity
KinematicViscosity

Velocity
Area
Diameter
Length
Height

Force
Torque
Power
Energy

Temperature

Time
Frequency
```

---

## 7. Einheitensystem

Intern SOLL bevorzugt das SI-System verwendet werden.

Beispiele:

```text
Pressure           -> Pa
Volume             -> m³
VolumetricFlowRate -> m³/s
MassFlowRate       -> kg/s
Length             -> m
Area               -> m²
Velocity           -> m/s
Density            -> kg/m³
DynamicViscosity   -> Pa·s
KinematicViscosity -> m²/s
Force              -> N
Power              -> W
Temperature        -> K
```

---

## 8. Darstellungseinheiten

Anwendungen DÜRFEN andere Darstellungseinheiten verwenden.

Beispiele:

```text
bar
mbar
MPa
psi

L
mL

L/min
L/s
m³/h

mm
cm

°C
°F
```

Die interne physikalische Bedeutung MUSS dabei erhalten bleiben.

---

## 9. Unit Conversion

Einheitenkonvertierungen MÜSSEN eindeutig definiert sein.

Beispiel:

```text
1 bar = 100000 Pa
```

und:

```text
1 L/min =
0.001 / 60 m³/s
```

---

## 10. Dimensionsprüfung

Hydraulic Compute SOLL dimensionsbehaftete Werte unterscheiden können.

Eine Operation wie:

```text
Pressure + Length
```

MUSS als dimensionsinkompatibel erkannt werden, sofern keine explizite physikalische Transformation definiert ist.

---

## 11. Numeric Representation

Standardmäßig SOLL mindestens:

```text
IEEE 754 Binary64
```

für numerisch anspruchsvolle hydraulische Berechnungen unterstützt werden.

Optional DÜRFEN verwendet werden:

```text
Binary32
Extended Precision
Arbitrary Precision
Fixed Point
```

---

## 12. Precision Policy

Eine Berechnung DARF eine Precision Policy besitzen.

Beispiele:

```text
FAST
BALANCED
ACCURATE
DETERMINISTIC
```

---

## 13. Fluid Object

Ein Fluid MUSS als explizites Objekt modelliert werden können.

Ein Fluid Descriptor SOLL mindestens enthalten:

```text
Name
Density
DynamicViscosity
KinematicViscosity
Temperature
Compressibility
VaporPressure
```

---

## 14. Fluid Properties

Fluideigenschaften DÜRFEN abhängig sein von:

```text
Temperature
Pressure
Composition
```

---

## 15. Constant Fluid Model

Das einfachste Fluidmodell verwendet konstante Eigenschaften.

Beispiel:

```text
Density = constant
Viscosity = constant
```

---

## 16. Variable Fluid Model

Eine erweiterte Implementierung SOLL temperatur- und druckabhängige Fluideigenschaften unterstützen können.

---

## 17. Incompressible Fluid

Hydraulic Compute MUSS inkompressible Fluidmodelle unterstützen.

Für viele hydraulische Berechnungen DARF näherungsweise gelten:

```text
Density = constant
```

---

## 18. Compressible Fluid

Eine erweiterte Implementierung DARF begrenzt kompressible Fluide unterstützen.

Dies ist insbesondere für:

```text
High Pressure Hydraulics
Accumulators
Pressure Transients
```

relevant.

---

## 19. Bulk Modulus

Die Kompressibilität einer Flüssigkeit SOLL über den Bulk Modulus beschrieben werden können.

```text
K = -V * dp/dV
```

mit:

```text
K = Bulk Modulus
V = Volume
p = Pressure
```

---

## 20. Hydraulic Network

Ein hydraulisches System SOLL als Graph modelliert werden.

```text
Nodes
+
Branches
+
Components
```

---

## 21. Network Node

Ein hydraulischer Node repräsentiert einen Verbindungspunkt.

Ein Node kann beispielsweise besitzen:

```text
Pressure
Elevation
Demand
Supply
Fluid
Temperature
```

---

## 22. Branch

Ein Branch verbindet zwei hydraulische Nodes.

Beispiel:

```text
Node A
  |
  v
Pipe
  |
  v
Node B
```

---

## 23. Component

Ein Branch DARF ein oder mehrere hydraulische Komponenten enthalten.

Beispiele:

```text
Pipe
Valve
Pump
Filter
Orifice
CheckValve
FlowController
PressureController
Cylinder
Motor
Accumulator
HeatExchanger
```

---

## 24. Network Topology

Die Runtime MUSS mindestens unterstützen:

```text
Series Networks
Parallel Networks
Branched Networks
Looped Networks
```

---

## 25. Conservation of Mass

An einem hydraulischen Node MUSS für stationäre inkompressible Systeme gelten:

```text
Σ Q_in = Σ Q_out
```

oder:

```text
Σ Q = 0
```

unter Verwendung einer definierten Vorzeichenkonvention.

---

## 26. Continuity Equation

Die Kontinuitätsgleichung bildet eine zentrale Grundlage hydraulischer Netzwerkberechnungen.

Für einen konstanten Rohrquerschnitt gilt:

```text
Q = A * v
```

mit:

```text
Q = Volumetric Flow Rate
A = Cross-sectional Area
v = Mean Flow Velocity
```

---

## 27. Circular Pipe Area

Für ein kreisförmiges Rohr gilt:

```text
A = π * D² / 4
```

mit:

```text
D = Internal Diameter
```

---

## 28. Flow Velocity

Die mittlere Strömungsgeschwindigkeit ergibt sich aus:

```text
v = Q / A
```

---

## 29. Reynolds Number

Hydraulic Compute MUSS die Reynolds-Zahl berechnen können.

```text
Re = ρ * v * D / μ
```

alternativ:

```text
Re = v * D / ν
```

mit:

```text
Re = Reynolds Number
ρ  = Density
v  = Velocity
D  = Characteristic Diameter
μ  = Dynamic Viscosity
ν  = Kinematic Viscosity
```

---

## 30. Flow Regime

Die Runtime SOLL Strömungszustände klassifizieren können.

Für typische Rohrströmungen:

```text
LAMINAR
TRANSITIONAL
TURBULENT
```

Die exakten Grenzwerte MÜSSEN vom verwendeten Modell abhängen können.

---

## 31. Darcy-Weisbach

Hydraulic Compute MUSS Druckverluste nach Darcy-Weisbach berechnen können.

```text
Δp =
f * (L / D) * (ρ * v² / 2)
```

mit:

```text
Δp = Pressure Loss
f  = Darcy Friction Factor
L  = Pipe Length
D  = Internal Diameter
ρ  = Fluid Density
v  = Mean Velocity
```

---

## 32. Hydraulic Head Loss

Alternativ DARF der Verlust als Förderhöhe dargestellt werden:

```text
h_f =
f * (L / D) * (v² / (2g))
```

---

## 33. Laminar Friction Factor

Für voll entwickelte laminare Rohrströmung gilt:

```text
f = 64 / Re
```

---

## 34. Turbulent Friction Factor

Für turbulente Strömung SOLL Hydraulic Compute geeignete Reibungsmodelle unterstützen.

Mindestens SOLL ein Modell auf Basis der Colebrook-White-Beziehung verfügbar sein können.

---

## 35. Colebrook-White

Die implizite Beziehung lautet:

```text
1 / sqrt(f) =
-2 * log10(
    ε / (3.7D)
    +
    2.51 / (Re * sqrt(f))
)
```

mit:

```text
ε = Absolute Roughness
```

---

## 36. Relative Roughness

Die relative Rauheit lautet:

```text
ε_r = ε / D
```

---

## 37. Explicit Friction Approximations

Die Runtime DARF explizite Näherungen unterstützen.

Beispiele:

```text
Haaland
Swamee-Jain
Churchill
```

---

## 38. Friction Model Selection

Eine Berechnung SOLL ein Reibungsmodell explizit auswählen können.

Beispiel:

```text
FrictionModel = COLEBROOK_WHITE
```

---

## 39. Automatic Friction Model

Optional DARF die Runtime abhängig vom Strömungszustand automatisch ein geeignetes Modell auswählen.

---

## 40. Minor Losses

Lokale Verluste MÜSSEN über Verlustbeiwerte modelliert werden können.

```text
Δp =
K * ρ * v² / 2
```

mit:

```text
K = Loss Coefficient
```

---

## 41. Local Loss Components

Typische lokale Verluste entstehen durch:

```text
Elbows
Tees
Reducers
Expanders
Entries
Exits
Valves
Fittings
Filters
```

---

## 42. Equivalent Length

Lokale Verluste DÜRFEN alternativ über äquivalente Rohrlängen modelliert werden.

---

## 43. Bernoulli Equation

Hydraulic Compute SOLL die Bernoulli-Gleichung für geeignete Systeme unterstützen.

```text
p / (ρg)
+
v² / (2g)
+
z
=
constant
```

für ideale stationäre Strömung entlang einer Stromlinie.

---

## 44. Extended Bernoulli Equation

Für reale Systeme SOLL eine erweiterte Form unterstützt werden:

```text
p1/(ρg)
+
v1²/(2g)
+
z1
+
H_pump
=
p2/(ρg)
+
v2²/(2g)
+
z2
+
H_loss
+
H_turbine
```

---

## 45. Static Pressure

Hydraulic Compute MUSS statischen Druck repräsentieren können.

---

## 46. Dynamic Pressure

Der dynamische Druck lautet:

```text
q =
ρ * v² / 2
```

---

## 47. Total Pressure

Für geeignete inkompressible Strömungsmodelle DARF Gesamtdruck als Kombination aus statischem und dynamischem Anteil behandelt werden.

---

## 48. Absolute Pressure

Absolute Drücke MÜSSEN eindeutig von Relativdrücken unterschieden werden.

---

## 49. Gauge Pressure

Gauge Pressure ist relativ zu einem Referenzdruck.

Typischerweise:

```text
p_gauge =
p_absolute
-
p_atmospheric
```

---

## 50. Pressure Reference

Jede Berechnung, die Relativdruck verwendet, MUSS eine eindeutige Druckreferenz besitzen.

---

## 51. Pipe Model

Ein Pipe Object SOLL mindestens besitzen:

```text
Length
InternalDiameter
Roughness
ElevationDifference
Fluid
LossModel
```

---

## 52. Pipe Result

Eine Rohrberechnung SOLL mindestens liefern können:

```text
FlowRate
Velocity
ReynoldsNumber
FrictionFactor
PressureLoss
HeadLoss
FlowRegime
```

---

## 53. Pipe Resistance

Hydraulic Compute SOLL hydraulischen Widerstand aus Geometrie, Fluid und Strömungszustand bestimmen können.

---

## 54. Nonlinear Resistance

Die Runtime MUSS berücksichtigen, dass hydraulischer Widerstand im Allgemeinen nicht konstant ist.

Insbesondere bei turbulenter Strömung hängt der Druckverlust nicht linear vom Volumenstrom ab.

---

## 55. Orifice

Eine Blende oder Drossel SOLL als eigener Component Type unterstützt werden.

---

## 56. Orifice Flow

Für geeignete inkompressible Modelle kann gelten:

```text
Q =
C_d * A * sqrt(2 * Δp / ρ)
```

mit:

```text
C_d = Discharge Coefficient
A   = Effective Area
```

---

## 57. Bidirectional Orifice

Eine Orifice-Komponente SOLL optional bidirektionale Strömung unterstützen.

Das Vorzeichen des Volumenstroms MUSS der definierten Flussrichtung folgen.

---

## 58. Valve Model

Hydraulic Compute MUSS Ventile als hydraulische Komponenten modellieren können.

---

## 59. Valve Types

Mindestens folgende Klassen SOLLEN abbildbar sein:

```text
Shutoff Valve
Control Valve
Check Valve
Pressure Relief Valve
Pressure Reducing Valve
Flow Control Valve
Directional Valve
```

---

## 60. Valve Position

Ein steuerbares Ventil SOLL eine Position besitzen.

Beispiel:

```text
0.0 = Closed
1.0 = Fully Open
```

---

## 61. Valve Characteristic

Ventilkennlinien DÜRFEN sein:

```text
LINEAR
EQUAL_PERCENTAGE
QUICK_OPENING
CUSTOM
```

---

## 62. Valve Flow Coefficient

Hydraulic Compute SOLL standardisierte Durchflusskennwerte unterstützen können.

Beispiele:

```text
Kv
Cv
```

Die verwendete Definition und Einheit MUSS explizit angegeben werden.

---

## 63. Check Valve

Ein Rückschlagventil MUSS eine bevorzugte Flussrichtung besitzen.

---

## 64. Check Valve Cracking Pressure

Ein Rückschlagventil DARF einen Öffnungsdruck besitzen:

```text
CrackingPressure
```

---

## 65. Pump Model

Pumpen MÜSSEN als aktive hydraulische Komponenten modellierbar sein.

---

## 66. Pump Operating Point

Der Betriebspunkt ergibt sich aus dem Schnittpunkt zwischen:

```text
Pump Characteristic
```

und:

```text
System Characteristic
```

---

## 67. Pump Curve

Eine Pumpenkennlinie SOLL mindestens beschreiben können:

```text
FlowRate -> Head
```

Optional:

```text
FlowRate -> Efficiency
FlowRate -> Power
FlowRate -> NPSHRequired
```

---

## 68. Pump Interpolation

Zwischen Kennlinienpunkten MUSS eine definierte Interpolation verwendet werden.

---

## 69. Pump Extrapolation

Extrapolation außerhalb der gültigen Pumpenkennlinie MUSS:

- verboten,
- gewarnt,
- oder explizit erlaubt

werden können.

---

## 70. Pump Speed

Variable Pumpendrehzahl SOLL unterstützt werden können.

---

## 71. Affinity Laws

Für geeignete Pumpenmodelle DÜRFEN Affinitätsgesetze verwendet werden.

Näherungsweise:

```text
Q ∝ n
H ∝ n²
P ∝ n³
```

mit:

```text
n = Rotational Speed
```

---

## 72. Pump Efficiency

Pumpenwirkungsgrad SOLL modelliert werden können.

```text
η =
HydraulicPower / MechanicalPower
```

---

## 73. Hydraulic Power

Hydraulische Leistung lautet:

```text
P_h =
Δp * Q
```

für inkompressible Strömung.

---

## 74. Mechanical Input Power

Bei bekanntem Wirkungsgrad:

```text
P_mech =
P_h / η
```

---

## 75. Hydraulic Motor

Hydraulikmotoren SOLLEN modellierbar sein.

Mindestens relevant:

```text
Displacement
PressureDifference
FlowRate
Speed
Torque
VolumetricEfficiency
MechanicalEfficiency
```

---

## 76. Hydraulic Cylinder

Hydraulikzylinder SOLLEN modellierbar sein.

Mindestens:

```text
PistonDiameter
RodDiameter
Stroke
Position
Velocity
PressureA
PressureB
```

---

## 77. Cylinder Piston Area

Für die Kolbenseite gilt:

```text
A_p =
π * D_p² / 4
```

---

## 78. Cylinder Rod-Side Area

Für die Stangenseite gilt:

```text
A_r =
π * (D_p² - D_r²) / 4
```

---

## 79. Cylinder Force

Idealisiert:

```text
F =
p * A
```

Für einen doppeltwirkenden Zylinder:

```text
F =
p_A * A_A
-
p_B * A_B
```

vor Berücksichtigung mechanischer Verluste.

---

## 80. Cylinder Velocity

Idealisiert:

```text
v =
Q / A
```

---

## 81. Cylinder Leakage

Eine erweiterte Implementierung SOLL interne und externe Leckage modellieren können.

---

## 82. Accumulator

Hydraulische Speicher SOLLEN unterstützt werden.

Beispiele:

```text
Gas Accumulator
Bladder Accumulator
Piston Accumulator
Diaphragm Accumulator
```

---

## 83. Gas Accumulator Model

Ein Gasdruckspeicher DARF über eine polytrope Beziehung modelliert werden:

```text
p * V^n = constant
```

---

## 84. Accumulator Precharge

Ein Speicher SOLL einen Vorfülldruck besitzen können.

```text
PrechargePressure
```

---

## 85. Reservoir

Ein Reservoir oder Tank SOLL modellierbar sein.

Mindestens:

```text
Volume
FluidLevel
Pressure
Temperature
Inflow
Outflow
```

---

## 86. Open Reservoir

Ein offener Tank DARF mit Atmosphärendruck als Oberflächendruck modelliert werden.

---

## 87. Pressurized Reservoir

Ein geschlossener Tank DARF einen variablen Gas- oder Systemdruck besitzen.

---

## 88. Hydrostatic Pressure

Hydraulic Compute MUSS hydrostatischen Druck berechnen können.

```text
Δp =
ρ * g * Δh
```

---

## 89. Gravity

Die Erdbeschleunigung SOLL als konfigurierbare physikalische Konstante behandelt werden.

Standardwert:

```text
g = 9.80665 m/s²
```

---

## 90. Elevation

Netzwerkknoten SOLLEN Höhenkoordinaten oder relative Höhendifferenzen besitzen können.

---

## 91. Cavitation

Hydraulic Compute SOLL Kavitation erkennen oder abschätzen können.

---

## 92. Cavitation Criterion

Kavitation ist möglich, wenn der lokale absolute Druck den Dampfdruck des Fluids erreicht oder unterschreitet.

Konzeptionell:

```text
p_absolute <= p_vapor
```

---

## 93. Cavitation Warning

Die Runtime SOLL mindestens folgende Zustände melden können:

```text
NO_CAVITATION_RISK
CAVITATION_MARGIN_LOW
CAVITATION_POSSIBLE
CAVITATION_DETECTED
```

---

## 94. NPSH

Für Pumpensysteme SOLL Hydraulic Compute unterstützen können:

```text
NPSH Available
NPSH Required
```

---

## 95. NPSH Safety Check

Die Runtime SOLL prüfen können:

```text
NPSHA > NPSHR + SafetyMargin
```

---

## 96. Filter Model

Filter SOLLEN als druckverlustbehaftete Komponenten modellierbar sein.

---

## 97. Filter Contamination

Eine erweiterte Implementierung DARF den Druckverlust abhängig von einem Verschmutzungszustand modellieren.

---

## 98. Heat Effects

Eine erweiterte Hydraulic-Compute-Implementierung SOLL thermische Auswirkungen auf Fluideigenschaften berücksichtigen können.

---

## 99. Temperature-dependent Viscosity

Insbesondere die Viskosität DARF als Funktion der Temperatur modelliert werden:

```text
μ = f(T)
```

---

## 100. Leakage

Hydraulic Compute SOLL Leckagen modellieren können.

Beispiele:

```text
Internal Leakage
External Leakage
Seal Leakage
Valve Leakage
Pump Leakage
Cylinder Leakage
```

---

## 101. Leakage Model

Ein einfaches Leckagemodell DARF beispielsweise verwenden:

```text
Q_leak =
C_leak * Δp
```

Komplexere nichtlineare Modelle DÜRFEN unterstützt werden.

---

## 102. Hydraulic Network Solver

Hydraulic Compute MUSS einen Solver für stationäre hydraulische Netzwerke bereitstellen können.

---

## 103. Solver Inputs

Der Solver SOLL mindestens verwenden:

```text
NetworkTopology
BoundaryConditions
FluidProperties
ComponentModels
InitialGuess
SolverOptions
```

---

## 104. Boundary Conditions

Mindestens folgende Randbedingungen SOLLEN unterstützt werden:

```text
FixedPressure
FixedFlow
ReservoirLevel
PumpOperatingCondition
ValvePosition
Demand
```

---

## 105. Unknown Variables

Typische unbekannte Größen sind:

```text
NodePressure
BranchFlow
PumpOperatingPoint
ValvePressureDrop
```

---

## 106. Equation System

Das Netzwerk MUSS in ein Gleichungssystem überführt werden können.

Konzeptionell:

```text
F(x) = 0
```

mit:

```text
x = Unknown Hydraulic State
```

---

## 107. Nonlinear Solver

Da hydraulische Netze typischerweise nichtlinear sind, MUSS mindestens ein geeigneter nichtlinearer Solver unterstützt werden.

---

## 108. Newton-Raphson

Eine Implementierung DARF Newton-Raphson verwenden:

```text
J(x_k) * Δx =
-F(x_k)
```

und:

```text
x_(k+1) =
x_k + Δx
```

---

## 109. Jacobian

Der Solver SOLL die Jacobi-Matrix analytisch, numerisch oder automatisch bestimmen können.

---

## 110. Sparse Solver

Große hydraulische Netzwerke SOLLEN Sparse-Matrix-Verfahren verwenden können.

---

## 111. Solver Convergence

Konvergenz MUSS anhand definierter Kriterien bestimmt werden.

Beispiele:

```text
PressureResidual
FlowResidual
EquationResidual
RelativeChange
MaximumIterations
```

---

## 112. Solver Tolerance

Solver-Toleranzen MÜSSEN konfigurierbar sein.

---

## 113. Non-Convergence

Bei Nichtkonvergenz MUSS ein definierter Fehler zurückgegeben werden.

Die Runtime DARF ein nicht konvergiertes Ergebnis nicht als gültige Lösung kennzeichnen.

---

## 114. Solver Diagnostics

Bei Nichtkonvergenz SOLLEN Diagnoseinformationen verfügbar sein.

Beispiele:

```text
IterationCount
LargestResidual
AffectedNode
AffectedBranch
SingularSystem
InvalidBoundaryCondition
```

---

## 115. Initial Guess

Ein geeigneter Initial Guess SOLL die Solver-Konvergenz verbessern können.

---

## 116. Previous Solution Reuse

Bei wiederholten ähnlichen Berechnungen SOLL die vorherige Lösung als Initial Guess verwendet werden können.

---

## 117. Incremental Recalculation

Ändert sich nur ein kleiner Teil eines Netzwerks, DARF die Runtime Teile vorheriger Berechnungen wiederverwenden.

---

## 118. Network Validation

Vor Solver-Ausführung MUSS das Netzwerk validiert werden.

Mindestens:

```text
Connected Components
Missing Boundary Conditions
Invalid Dimensions
Invalid Parameters
Impossible Component States
Invalid Fluid References
```

---

## 119. Disconnected Network

Nicht verbundene hydraulische Teilnetze SOLLEN getrennt berechnet werden können, sofern jedes Teilnetz ausreichend Randbedingungen besitzt.

---

## 120. Singular Network

Ein unterbestimmtes oder singuläres hydraulisches Netzwerk MUSS erkannt werden.

---

## 121. Overconstrained Network

Widersprüchliche Randbedingungen MÜSSEN erkannt werden.

---

## 122. Reverse Flow

Komponenten SOLLEN negative Volumenströme repräsentieren können, wenn Rückströmung physikalisch erlaubt ist.

---

## 123. Directional Components

Komponenten wie Rückschlagventile DÜRFEN Rückströmung blockieren.

---

## 124. Component State

Komponenten DÜRFEN diskrete Zustände besitzen.

Beispiele:

```text
OPEN
CLOSED
ACTIVE
INACTIVE
SATURATED
BLOCKED
```

---

## 125. State-dependent Equations

Der Solver MUSS Komponenten unterstützen können, deren Gleichungen vom aktuellen Zustand abhängen.

---

## 126. Pressure Relief Valve

Ein Druckbegrenzungsventil SOLL abhängig vom Druckdifferenzzustand öffnen können.

---

## 127. Pressure Reducing Valve

Ein Druckminderventil SOLL einen nachgelagerten Zieldruck regeln können.

---

## 128. Flow Controller

Ein Durchflussregler SOLL einen Zielvolumenstrom repräsentieren können.

---

## 129. Component Saturation

Regelnde Komponenten MÜSSEN physikalische Grenzwerte besitzen können.

Beispiele:

```text
MaximumOpening
MaximumFlow
MaximumPressure
MaximumSpeed
```

---

## 130. Hydraulic Limits

Die Runtime SOLL Komponentenlimits prüfen können.

Beispiele:

```text
MaximumPressure
MinimumPressure
MaximumFlow
MaximumVelocity
MaximumTemperature
MaximumPower
```

---

## 131. Warning System

Berechnungen SOLLEN Warnungen erzeugen können.

Beispiele:

```text
HIGH_FLOW_VELOCITY
LOW_PRESSURE
HIGH_PRESSURE
CAVITATION_RISK
PUMP_OUTSIDE_CURVE
VALVE_SATURATED
FILTER_PRESSURE_DROP_HIGH
```

---

## 132. Warning Severity

Warnungen SOLLEN klassifiziert werden:

```text
INFO
WARNING
CRITICAL
```

---

## 133. Error Model

Hydraulic Compute MUSS definierte Fehlercodes verwenden können.

Beispiele:

```text
INVALID_UNIT
INVALID_PARAMETER
INVALID_NETWORK
NO_BOUNDARY_CONDITION
SINGULAR_SYSTEM
SOLVER_DIVERGED
OUT_OF_RANGE
UNSUPPORTED_MODEL
NUMERIC_OVERFLOW
```

---

## 134. NaN Handling

NaN-Werte DÜRFEN nicht unkontrolliert als gültige physikalische Ergebnisse propagiert werden.

---

## 135. Infinity Handling

Unendliche Werte MÜSSEN erkannt und entsprechend der Modellsemantik behandelt werden.

---

## 136. Negative Physical Values

Unzulässige negative physikalische Größen MÜSSEN validiert werden.

Beispiele:

```text
Negative Absolute Pressure
Negative Diameter
Negative Density
Negative Dynamic Viscosity
```

---

## 137. Zero Diameter

Ein Rohr mit:

```text
Diameter = 0
```

MUSS als ungültige Geometrie erkannt werden.

---

## 138. Zero Length

Eine Länge von null DARF für spezielle idealisierte Komponenten erlaubt sein, aber nicht automatisch für ein normales Pipe Model.

---

## 139. Deterministic Mode

Hydraulic Compute SOLL einen deterministischen Berechnungsmodus bereitstellen.

Gleiche:

```text
Inputs
Runtime Version
Solver
Solver Options
Hardware Semantics
```

SOLLEN reproduzierbare Ergebnisse liefern.

---

## 140. Parallel Solver

Große Netzwerke SOLLEN parallel berechnet werden können.

---

## 141. CPU Execution

Eine konforme Implementierung MUSS CPU-Ausführung unterstützen.

---

## 142. SIMD

Numerische Teiloperationen SOLLEN SIMD verwenden können.

---

## 143. Multi-Core

Große unabhängige Teilberechnungen SOLLEN über mehrere CPU-Kerne verteilt werden können.

---

## 144. GPU Acceleration

Geeignete hydraulische Berechnungen DÜRFEN über `NPSPEC-COMPUTE-RUNTIME-0005` auf GPUs ausgelagert werden.

---

## 145. GPU Candidates

Geeignete Kandidaten können sein:

```text
Large Parameter Sweeps
Monte Carlo Simulations
Large Sparse Operations
Optimization
Batch Network Solves
```

---

## 146. NPU Usage

NPUs sind für klassische hydraulische Solver nicht grundsätzlich das bevorzugte Ausführungsziel.

Sie DÜRFEN jedoch für AI-basierte Zusatzmodelle verwendet werden.

Beispiele:

```text
Leak Detection
Failure Prediction
Sensor Anomaly Detection
Surrogate Models
Predictive Maintenance
```

---

## 147. Distributed Compute

Sehr große Simulationen, Parameterstudien oder Optimierungsprobleme DÜRFEN über `NPSPEC-COMPUTE-RUNTIME-0007` verteilt werden.

---

## 148. Parameter Sweep

Hydraulic Compute SOLL Parameterstudien unterstützen können.

Beispiel:

```text
ValvePosition =
0.0
0.1
0.2
...
1.0
```

---

## 149. Batch Solve

Mehrere unabhängige hydraulische Fälle SOLLEN als Batch berechnet werden können.

---

## 150. Optimization Integration

Hydraulic Compute SOLL mit Optimierungsalgorithmen kombinierbar sein.

Beispiele:

```text
Pipe Diameter Optimization
Pump Selection
Pump Speed Optimization
Valve Setting Optimization
Energy Optimization
Network Balancing
```

---

## 151. Objective Function

Eine Optimierung DARF hydraulische Ergebnisse als Zielfunktion verwenden.

Beispiel:

```text
Minimize:
EnergyConsumption
```

unter Nebenbedingungen:

```text
MinimumPressure
MaximumVelocity
RequiredFlow
```

---

## 152. Sensitivity Analysis

Eine erweiterte Implementierung SOLL Sensitivitätsanalysen unterstützen können.

Beispiel:

```text
∂Pressure / ∂ValvePosition
```

---

## 153. Uncertainty Analysis

Hydraulic Compute DARF Unsicherheiten in Eingabeparametern modellieren.

Beispiele:

```text
Roughness
Demand
FluidViscosity
PumpCurve
ValveCoefficient
```

---

## 154. Monte Carlo

Monte-Carlo-Simulationen DÜRFEN zur Unsicherheitsanalyse verwendet werden.

---

## 155. Time-dependent Simulation

Eine erweiterte Implementierung SOLL zeitabhängige hydraulische Simulation unterstützen können.

---

## 156. Dynamic State

Ein dynamisches System kann Zustände besitzen wie:

```text
TankLevel
AccumulatorPressure
CylinderPosition
CylinderVelocity
ValvePosition
PumpSpeed
```

---

## 157. Time Step

Transiente Simulationen MÜSSEN eine definierte Zeitdiskretisierung verwenden.

---

## 158. Fixed Time Step

Ein Solver DARF einen festen Zeitschritt verwenden.

```text
Δt = constant
```

---

## 159. Adaptive Time Step

Eine erweiterte Implementierung SOLL adaptive Zeitschritte unterstützen können.

---

## 160. Water Hammer

Druckstoßberechnung ist eine erweiterte Hydraulic-Compute-Funktion.

Sie DARF in einer separaten Spezifikation detailliert definiert werden.

---

## 161. Wave Speed

Transiente Rohrmodelle DÜRFEN Druckwellengeschwindigkeit berücksichtigen.

---

## 162. Fluid-Structure Interaction

Vollständige Fluid-Struktur-Kopplung ist nicht Bestandteil dieser Basisspezifikation.

---

## 163. Digital Twin Integration

Hydraulic Compute SOLL als Berechnungskern für hydraulische Digital Twins verwendet werden können.

---

## 164. Sensor Inputs

Ein Digital Twin DARF reale Messwerte verwenden.

Beispiele:

```text
PressureSensor
FlowSensor
TemperatureSensor
PositionSensor
SpeedSensor
```

---

## 165. Measurement Mapping

Messwerte SOLLEN hydraulischen Nodes oder Komponenten zugeordnet werden können.

---

## 166. State Estimation

Eine erweiterte Implementierung DARF unbekannte hydraulische Zustände anhand von Messwerten schätzen.

---

## 167. Residual Analysis

Die Differenz zwischen Modell und Messung SOLL berechnet werden können.

```text
Residual =
MeasuredValue
-
PredictedValue
```

---

## 168. Leak Detection

Leckageerkennung DARF auf Residuen zwischen gemessenen und berechneten Zuständen aufbauen.

---

## 169. Model Calibration

Hydraulic Compute SOLL Modellparameter anhand von Messdaten kalibrieren können.

Beispiele:

```text
PipeRoughness
ValveCoefficient
LeakageCoefficient
PumpEfficiency
```

---

## 170. Calibration Constraints

Kalibrierte Parameter MÜSSEN innerhalb physikalisch zulässiger Grenzen bleiben.

---

## 171. Real-Time Compute

Hydraulic Compute SOLL für geeignete Modelle echtzeitnahe Berechnung ermöglichen.

---

## 172. Hard Real-Time

Diese Basisspezifikation garantiert keine Hard-Real-Time-Eigenschaften.

Hard-Real-Time-Steuerung MUSS durch eine separate Echtzeitspezifikation geregelt werden.

---

## 173. Control Integration

Hydraulic Compute DARF Sollwerte und simulierte Messwerte für Regelungsalgorithmen bereitstellen.

---

## 174. Safety Boundary

Eine Simulation DARF nicht automatisch als sicherheitszertifizierte Maschinensteuerung behandelt werden.

---

## 175. Model Metadata

Jedes hydraulische Modell SOLL Metadaten besitzen können.

Beispiele:

```text
ModelName
ModelVersion
Author
Created
Modified
UnitSystem
SolverRequirements
ValidationState
```

---

## 176. Model Versioning

Hydraulische Modelle SOLLEN versionierbar sein.

---

## 177. Component Library

NovaOS SOLL eine erweiterbare hydraulische Komponentenbibliothek ermöglichen.

---

## 178. Standard Component Descriptor

Jede Komponentenklasse SOLL mindestens definieren:

```text
Inputs
Outputs
Parameters
State
Equations
Limits
Diagnostics
```

---

## 179. Custom Component

Drittanbieter oder Anwendungen DÜRFEN eigene hydraulische Komponenten bereitstellen.

---

## 180. Custom Component Isolation

Benutzerdefinierte Komponenten DÜRFEN die Integrität des Solvers nicht gefährden.

---

## 181. Component Namespace

Komponenten SOLLEN eindeutig benannt werden können.

Beispiel:

```text
Nova.Hydraulics.Components.Pipe
Nova.Hydraulics.Components.Pump
Nova.Hydraulics.Components.Valve
```

---

## 182. API Namespace

Eine mögliche logische API-Struktur lautet:

```text
Nova.Hydraulics
Nova.Hydraulics.Fluids
Nova.Hydraulics.Components
Nova.Hydraulics.Network
Nova.Hydraulics.Solvers
Nova.Hydraulics.Diagnostics
Nova.Hydraulics.Optimization
```

Die konkrete Sprachbindung wird separat definiert.

---

## 183. Serialization

Hydraulische Modelle SOLLEN serialisierbar sein.

---

## 184. Portable Model

Ein serialisiertes Modell SOLL möglichst hardwareunabhängig sein.

---

## 185. Model Validation on Load

Geladene Modelle MÜSSEN vor Verwendung validiert werden.

---

## 186. Unknown Component

Ein unbekannter Component Type MUSS kontrolliert behandelt werden.

---

## 187. Forward Compatibility

Neuere Model Features SOLLEN durch Versionierung erkennbar sein.

---

## 188. Profiling

Hydraulic Compute SOLL Profiling unterstützen.

Mindestens:

```text
ModelValidationTime
EquationBuildTime
SolverTime
IterationCount
MemoryUsage
```

---

## 189. Solver Metrics

Mindestens folgende Solver-Metriken SOLLEN verfügbar sein:

```text
Iterations
Residual
ConvergenceRate
SolveTime
MatrixSize
NonZeroElements
```

---

## 190. Network Metrics

Optional:

```text
NodeCount
BranchCount
ComponentCount
LoopCount
```

---

## 191. Trace Events

Mindestens folgende Trace Events SOLLEN unterstützt werden:

```text
HYDRAULIC_MODEL_LOAD
HYDRAULIC_MODEL_VALIDATE

HYDRAULIC_SOLVE_START
HYDRAULIC_ITERATION
HYDRAULIC_SOLVE_COMPLETE
HYDRAULIC_SOLVE_FAILED

HYDRAULIC_WARNING
HYDRAULIC_COMPONENT_STATE_CHANGE
```

---

## 192. Diagnostic Result

Ein Solver Result SOLL neben numerischen Ergebnissen auch Diagnosen enthalten können.

Beispiel:

```text
Status
Warnings
Errors
Iterations
Residual
ExecutionTime
```

---

## 193. Testing — Units

Mindestens zu testen:

```text
Pa
bar
MPa
psi

m³/s
L/min
m³/h

m
mm

K
°C
```

---

## 194. Testing — Unit Conversion

Zu testen sind insbesondere:

```text
bar -> Pa
Pa -> bar
L/min -> m³/s
m³/s -> L/min
mm -> m
°C -> K
```

---

## 195. Testing — Dimensional Safety

Ungültige Operationen zwischen inkompatiblen Dimensionen MÜSSEN erkannt werden.

---

## 196. Testing — Reynolds Number

Zu testen sind:

```text
Laminar
Transitional
Turbulent
```

---

## 197. Testing — Pipe Loss

Mindestens:

```text
Laminar Pipe
Smooth Turbulent Pipe
Rough Turbulent Pipe
Short Pipe
Long Pipe
```

---

## 198. Testing — Friction Models

Zu testen sind:

```text
64/Re
Colebrook-White
Haaland
Swamee-Jain
```

sofern implementiert.

---

## 199. Testing — Minor Loss

Zu testen:

```text
K = 0
Small K
Large K
Multiple Local Losses
```

---

## 200. Testing — Pump

Mindestens:

```text
Zero Flow
Nominal Point
Best Efficiency Point
High Flow
Variable Speed
Outside Curve
```

---

## 201. Testing — Valve

Mindestens:

```text
Closed
Partially Open
Fully Open
Reverse Flow
Control Saturation
```

---

## 202. Testing — Check Valve

Zu testen:

```text
Forward Flow
Below Cracking Pressure
Above Cracking Pressure
Reverse Pressure
```

---

## 203. Testing — Cylinder

Mindestens:

```text
Extension
Retraction
Different Chamber Areas
Zero Flow
Maximum Stroke
```

---

## 204. Testing — Accumulator

Mindestens:

```text
Precharge
Compression
Expansion
Minimum Volume
Maximum Pressure
```

---

## 205. Testing — Cavitation

Zu testen:

```text
Pressure Above Vapor Pressure
Pressure Near Vapor Pressure
Pressure Below Vapor Pressure
```

---

## 206. Testing — Network

Mindestens:

```text
Single Pipe
Series Pipes
Parallel Pipes
Branched Network
Looped Network
Multiple Pumps
Multiple Valves
```

---

## 207. Testing — Boundary Conditions

Zu testen:

```text
Fixed Pressure
Fixed Flow
Reservoir
Demand
Pump Condition
```

---

## 208. Testing — Invalid Networks

Mindestens:

```text
No Pressure Reference
Disconnected Network
Singular Network
Conflicting Boundary Conditions
Invalid Component
```

---

## 209. Testing — Solver

Zu testen:

```text
Immediate Convergence
Normal Convergence
Slow Convergence
Maximum Iterations
Divergence
Singular Jacobian
```

---

## 210. Testing — Reverse Flow

Netzwerke mit umgekehrter Flussrichtung MÜSSEN korrekt berechnet werden.

---

## 211. Testing — Extreme Values

Mindestens:

```text
Very Low Flow
Very High Flow
Very Small Diameter
Very Long Pipe
High Pressure
Low Absolute Pressure
High Viscosity
Low Viscosity
```

---

## 212. Testing — Invalid Values

Mindestens:

```text
Negative Diameter
Negative Density
Negative Viscosity
Zero Diameter
NaN
Infinity
```

---

## 213. Testing — Determinism

Deterministische Solver Runs MÜSSEN auf derselben definierten Plattform reproduzierbar sein.

---

## 214. Testing — Parallel Execution

Parallele Berechnung DARF keine unzulässigen Unterschiede gegenüber der definierten numerischen Toleranz erzeugen.

---

## 215. Testing — Batch

Zu testen sind große Mengen unabhängiger hydraulischer Berechnungen.

---

## 216. Testing — Performance

Mindestens folgende Werte SOLLEN gemessen werden:

```text
EquationBuildTime
SolverTime
IterationsPerSecond
NetworkSolvesPerSecond
MemoryUsage
```

---

## 217. Testing — Scalability

Hydraulische Netze SOLLEN mit steigender Größe getestet werden.

Beispiele:

```text
10 Nodes
100 Nodes
1,000 Nodes
10,000 Nodes
100,000 Nodes
```

soweit die Zielimplementierung diese Größenordnung unterstützt.

---

## 218. Stress Tests

Stress Tests SOLLEN mindestens umfassen:

```text
Large Networks
Repeated Solves
Rapid Parameter Changes
Batch Simulations
Parallel Solves
Memory Pressure
```

---

## 219. Fuzzing

Mindestens folgende Inputs SOLLTEN fuzz-getestet werden:

```text
Model Files
Component Descriptors
Unit Descriptors
Network Topologies
Solver Options
Fluid Properties
```

---

## 220. Numerical Regression Tests

Änderungen am Solver MÜSSEN gegen bekannte Referenzfälle geprüft werden.

---

## 221. Reference Cases

Die Testsuite SOLL analytisch oder anderweitig verifizierte Referenzfälle enthalten.

---

## 222. Conservation Tests

Für geschlossene stationäre Systeme MUSS Massenerhaltung innerhalb der definierten Solver-Toleranz erfüllt sein.

---

## 223. Energy Consistency

Geeignete Testfälle SOLLEN Energie- und Druckverlustbeziehungen auf physikalische Konsistenz prüfen.

---

## 224. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
SI Unit System
Unit Conversion
Incompressible Fluid
Constant Density
Constant Viscosity

Pipe
Orifice
Basic Valve
Pump

Pressure Nodes
Flow Branches

Continuity Equation
Reynolds Number
Darcy-Weisbach
Laminar Friction
Turbulent Friction Model
Minor Losses

Stationary Network Solver
Solver Convergence Detection
Basic Diagnostics
CPU Execution
```

---

## 225. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Temperature-dependent Fluids
Compressibility
Advanced Pump Models
Control Valves
Pressure Valves
Hydraulic Cylinders
Hydraulic Motors
Accumulators
Reservoirs
Leakage
Cavitation
NPSH
Filters
Dynamic Simulation
Transient Flow
Parameter Sweeps
Optimization
Sensitivity Analysis
Uncertainty Analysis
Digital Twins
State Estimation
Model Calibration
GPU Acceleration
Distributed Compute
```

---

## 226. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-HYDRAULICS-0001` MUSS:

1. physikalische hydraulische Größen eindeutig repräsentieren.
2. SI-Einheiten intern unterstützen.
3. Einheiten sicher konvertieren.
4. inkompatible Dimensionen erkennen.
5. Fluide als explizite Modelle repräsentieren.
6. inkompressible Fluide unterstützen.
7. Rohrgeometrien modellieren.
8. Volumenstrom und Geschwindigkeit berechnen.
9. Reynolds-Zahlen berechnen.
10. laminare und turbulente Strömung unterscheiden können.
11. Rohrreibungsverluste berechnen.
12. lokale Verluste modellieren.
13. hydraulische Nodes modellieren.
14. hydraulische Branches modellieren.
15. Massenerhaltung an Nodes erzwingen.
16. Pumpen modellieren.
17. grundlegende Ventile modellieren.
18. stationäre hydraulische Netzwerke lösen.
19. Solver-Konvergenz prüfen.
20. Nichtkonvergenz als Fehler melden.
21. physikalisch ungültige Eingaben erkennen.
22. numerische Fehler kontrolliert behandeln.
23. Diagnoseinformationen bereitstellen.
24. CPU-Ausführung unterstützen.
25. das allgemeine NovaOS Compute Object Model verwenden.
26. das allgemeine NovaOS Compute Scheduling integrieren können.
27. zukünftige hydraulische Modelle erweiterbar halten.

---

## 227. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen können:

```text
NovaHydraulicCompute
 |
 +-- HydraulicUnitSystem
 |
 +-- FluidManager
 |
 +-- HydraulicComponentManager
 |
 +-- HydraulicNetworkManager
 |
 +-- HydraulicEquationBuilder
 |
 +-- HydraulicSolver
 |
 +-- HydraulicLinearSolver
 |
 +-- HydraulicNonlinearSolver
 |
 +-- HydraulicDiagnostics
 |
 +-- HydraulicOptimization
 |
 +-- HydraulicProfiler
```

Diese Aufteilung ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 228. Beziehung zur Compute Runtime

Hydraulic Compute baut auf der allgemeinen Compute Runtime auf.

```text
Hydraulic Model
      |
      v
Hydraulic Compute
      |
      v
Compute Runtime
      |
      +--> CPU
      |
      +--> GPU
      |
      +--> Distributed Compute
```

Hydraulic Compute definiert die Fachsemantik.

Die Compute Runtime übernimmt die technische Ausführung.

---

## 229. Beziehung zum Scheduler

Der Compute Scheduler DARF abhängig vom Workload entscheiden, wie eine hydraulische Berechnung ausgeführt wird.

Beispiel:

```text
Small Network
    |
    v
CPU

Large Batch
    |
    v
Multi-Core CPU

Parameter Sweep
    |
    v
GPU

Massive Study
    |
    v
Distributed Compute
```

---

## 230. Beziehung zum Memory Model

Hydraulische Datenstrukturen MÜSSEN auf dem allgemeinen Compute Memory Model abbildbar sein.

Beispiele:

```text
Node Arrays
Branch Arrays
Sparse Matrices
Solver Vectors
Parameter Arrays
Result Arrays
```

---

## 231. Beziehung zum Object Model

Hydraulic Compute Objects SOLLEN auf dem allgemeinen Compute Object Model aufbauen.

Beispiele:

```text
Fluid
Pipe
Valve
Pump
Cylinder
Accumulator
Node
Branch
Network
Solver
Result
```

---

## 232. Beziehung zur GPU Runtime

Geeignete massiv parallele hydraulische Workloads DÜRFEN über `NPSPEC-COMPUTE-RUNTIME-0005` ausgeführt werden.

Die fachliche Hydrauliksemantik MUSS unabhängig vom verwendeten Compute Device bleiben.

---

## 233. Beziehung zur NPU Runtime

AI-basierte hydraulische Zusatzfunktionen DÜRFEN `NPSPEC-COMPUTE-RUNTIME-0006` verwenden.

Beispiele:

```text
Leak Prediction
Failure Prediction
Anomaly Detection
Surrogate Simulation
Predictive Maintenance
```

Die klassische physikalische Solver-Semantik DARF dadurch nicht zwingend von AI abhängig werden.

---

## 234. Beziehung zu Distributed Compute

Große Parameterstudien, Optimierungen oder Simulationen DÜRFEN über `NPSPEC-COMPUTE-RUNTIME-0007` verteilt werden.

Beispiel:

```text
10,000 Hydraulic Scenarios
          |
          v
Distributed Compute
     /     |     \
    v      v      v
 Node A  Node B  Node C
```

---

## 235. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-HYDRAULICS-UNITS-0001
Hydraulic Units

NPSPEC-COMPUTE-HYDRAULICS-FLUID-0001
Fluid Model

NPSPEC-COMPUTE-HYDRAULICS-NETWORK-0001
Hydraulic Network Model

NPSPEC-COMPUTE-HYDRAULICS-PIPE-0001
Pipe Flow

NPSPEC-COMPUTE-HYDRAULICS-LOSS-0001
Pressure Loss Models

NPSPEC-COMPUTE-HYDRAULICS-PUMP-0001
Pump Model

NPSPEC-COMPUTE-HYDRAULICS-VALVE-0001
Valve Model

NPSPEC-COMPUTE-HYDRAULICS-CYLINDER-0001
Hydraulic Cylinder

NPSPEC-COMPUTE-HYDRAULICS-MOTOR-0001
Hydraulic Motor

NPSPEC-COMPUTE-HYDRAULICS-ACCUMULATOR-0001
Hydraulic Accumulator

NPSPEC-COMPUTE-HYDRAULICS-CAVITATION-0001
Cavitation and NPSH

NPSPEC-COMPUTE-HYDRAULICS-SOLVER-0001
Hydraulic Network Solver

NPSPEC-COMPUTE-HYDRAULICS-TRANSIENT-0001
Transient Hydraulic Simulation

NPSPEC-COMPUTE-HYDRAULICS-WATERHAMMER-0001
Pressure Surge / Water Hammer

NPSPEC-COMPUTE-HYDRAULICS-OPTIMIZATION-0001
Hydraulic Optimization

NPSPEC-COMPUTE-HYDRAULICS-DIGITALTWIN-0001
Hydraulic Digital Twin

NPSPEC-COMPUTE-HYDRAULICS-DIAG-0001
Hydraulic Diagnostics

NPSPEC-COMPUTE-HYDRAULICS-TEST-0001
Hydraulic Compute Conformance Tests
```

---

## 236. Zusammenfassung

Hydraulic Compute stellt NovaOS eine gemeinsame Rechenplattform für hydraulische und fluidtechnische Systeme bereit.

Die grundlegende Verarbeitung lautet:

```text
Hydraulic Model
      |
      v
Validate
      |
      v
Build Network
      |
      v
Build Equations
      |
      v
Select Solver
      |
      v
Compute
      |
      v
Validate Convergence
      |
      v
Physical Diagnostics
      |
      v
Results
```

Das hydraulische Netzwerk wird dabei als Graph betrachtet:

```text
Node
 |
 v
Pipe
 |
 v
Node
 |
 v
Valve
 |
 v
Node
 |
 v
Pump
 |
 v
Node
```

Aus diesem Graphen erzeugt Hydraulic Compute ein mathematisches Gleichungssystem.

Die zentralen physikalischen Beziehungen umfassen unter anderem:

```text
Q = A * v
```

```text
Re = ρ * v * D / μ
```

```text
Δp =
f * (L / D) * (ρ * v² / 2)
```

```text
Σ Q = 0
```

und für hydraulische Leistung:

```text
P = Δp * Q
```

Hydraulic Compute ist dabei nicht auf einen einzelnen Ausführungsweg beschränkt.

```text
                   Hydraulic Compute
                          |
             +------------+------------+
             |            |            |
             v            v            v
            CPU          GPU      Distributed
```

Kleine einzelne Netze können direkt auf der CPU berechnet werden.

Große Batch-Berechnungen und Parameterstudien können parallelisiert werden.

AI-basierte Diagnosefunktionen können zusätzlich NPUs verwenden:

```text
Physical Hydraulic Model
          |
          +-------------------+
          |                   |
          v                   v
   Hydraulic Solver      AI Diagnostics
          |                   |
          v                   v
        CPU/GPU               NPU
          |                   |
          +---------+---------+
                    |
                    v
             Combined Result
```

Dadurch entsteht eine Grundlage, auf der NovaOS unter anderem folgende Fähigkeiten bereitstellen kann:

```text
Pipe Sizing
Pressure Loss Calculation
Pump Selection
Valve Calculation
Hydraulic Network Simulation
Cylinder Calculation
Energy Analysis
Leak Detection
System Optimization
Digital Twins
Predictive Maintenance
```

Das grundlegende Architekturprinzip lautet:

> Hydraulische Physik wird in NovaOS als wiederverwendbare systemweite Compute-Fähigkeit behandelt. Anwendungen beschreiben Fluide, Komponenten und hydraulische Netzwerke; Hydraulic Compute übernimmt Einheiten, physikalische Gleichungen, Solver, Validierung, Diagnose und die effiziente Ausführung über die NovaOS Compute Runtime.