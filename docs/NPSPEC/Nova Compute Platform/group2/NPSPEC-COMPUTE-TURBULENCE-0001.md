# NPSPEC-COMPUTE-TURBULENCE-0001 — Turbulence Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Fluid Dynamics / Turbulence  
**Komponente:** NovaOS Turbulence Compute  
**Bezeichner:** `NPSPEC-COMPUTE-TURBULENCE-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HYDRAULICS-0001`, `NPSPEC-COMPUTE-AERODYNAMICS-0001`, `NPSPEC-COMPUTE-GASDYNAMICS-0001`, `NPSPEC-COMPUTE-HPC-0001`  
**Kurzname:** Turbulence Compute  

---

## 1. Zweck

Diese Spezifikation definiert die Turbulence-Compute-Schicht der NovaOS Compute-Architektur.

Turbulence Compute stellt gemeinsame physikalische Modelle, Datenstrukturen, Gleichungen, Solver-Schnittstellen und numerische Primitive für turbulente Fluidströmungen bereit.

Die Komponente behandelt insbesondere Strömungen, bei denen chaotische, dreidimensionale und zeitabhängige Geschwindigkeitsschwankungen einen wesentlichen Einfluss auf:

```text
Momentum Transport
Energy Transport
Mass Transport
Pressure Loss
Wall Shear
Mixing
Heat Transfer
Drag
Lift
Noise
Combustion
Dispersion
```

besitzen.

Typische Einsatzgebiete sind:

```text
Computational Fluid Dynamics
Aerodynamics
Hydraulics
Gas Dynamics
Pipe Flow
Channel Flow
Boundary Layers
Jets
Wakes
Mixing
Turbomachinery
Vehicle Aerodynamics
Aircraft Aerodynamics
Building Aerodynamics
Weather Simulation
Combustion
Heat Transfer
Industrial Flow
Environmental Flow
Digital Twins
Engineering Simulation
```

Turbulence Compute ist kein vollständiger CFD-Solver.

Die Komponente stellt Turbulenzmodelle und Turbulenz-Compute-Fähigkeiten bereit, die von übergeordneten Fluid- und CFD-Systemen verwendet werden können.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Turbulenz wird als austauschbare physikalische Modellschicht behandelt und nicht fest mit einem bestimmten CFD-Solver, Diskretisierungsverfahren oder Compute-Backend verbunden.

Eine Anwendung SOLL beispielsweise angeben können:

```text
TurbulenceModel = K_OMEGA_SST
```

ohne selbst implementieren zu müssen:

```text
Transport Equations
Turbulent Viscosity
Production Terms
Dissipation Terms
Wall Treatment
Model Constants
Boundary Conditions
```

Die Architektur lautet:

```text
Flow Problem
     |
     v
Fluid Solver
     |
     v
Turbulence Model
     |
     v
Turbulence Compute
     |
     v
Compute Runtime
```

---

## 3. Ziele

Turbulence Compute MUSS folgende Ziele verfolgen:

1. einheitliche Turbulenzmodell-Schnittstelle,
2. laminare Referenzberechnung,
3. Reynolds-Averaged Navier-Stokes,
4. RANS-Turbulenzmodelle,
5. Large Eddy Simulation,
6. Direct Numerical Simulation als Solver-Modus,
7. hybride RANS/LES-Verfahren,
8. Reynolds-Spannungen,
9. turbulente Viskosität,
10. turbulente kinetische Energie,
11. Dissipation,
12. spezifische Dissipation,
13. Wandbehandlung,
14. Wall Functions,
15. Low-Reynolds-Behandlung,
16. Near-Wall-Modellierung,
17. Transition-Erweiterbarkeit,
18. inkompressible Strömungen,
19. kompressible Strömungen,
20. Wärmeübertragung,
21. Stofftransport,
22. Rotations- und Krümmungseffekte,
23. Auftriebseffekte,
24. Turbulenzproduktion,
25. Turbulenzdissipation,
26. Turbulenzdiffusion,
27. robuste Positivität,
28. numerische Stabilität,
29. CPU-Ausführung,
30. GPU-Beschleunigung,
31. HPC-Ausführung,
32. Distributed Compute,
33. Profiling,
34. Validierung,
35. Diagnose,
36. reproduzierbare Berechnung.

---

## 4. Nicht-Ziele

Diese Basisspezifikation definiert nicht vollständig:

- einen vollständigen CFD-Solver,
- Mesh-Generierung,
- CAD-Geometrieverarbeitung,
- allgemeine Navier-Stokes-Diskretisierung,
- vollständige Verbrennungschemie,
- Mehrphasenströmung,
- vollständige Aeroakustik,
- Wettermodelle,
- vollständige atmosphärische Turbulenz,
- konkrete Turbomaschinenanwendungen,
- konkrete Fahrzeugmodelle.

Diese Systeme DÜRFEN Turbulence Compute verwenden.

---

## 5. Turbulenz

Turbulente Strömung ist durch zeitlich und räumlich schwankende Strömungsgrößen charakterisiert.

Typische Eigenschaften sind:

```text
Irregularity
Three-Dimensionality
Vorticity
Diffusivity
Energy Cascade
Dissipation
Wide Range of Scales
```

---

## 6. Instantaneous Flow Quantity

Eine momentane Strömungsgröße wird konzeptionell dargestellt als:

```text
φ(x, t)
```

Beispiele:

```text
Velocity
Pressure
Temperature
Density
Species Concentration
```

---

## 7. Reynolds Decomposition

Für RANS-Verfahren wird eine Größe zerlegt:

```text
φ =
φ_bar
+
φ'
```

mit:

```text
φ_bar = Mean Component
φ'    = Fluctuating Component
```

---

## 8. Velocity Decomposition

Für Geschwindigkeit gilt:

```text
u_i =
U_i
+
u'_i
```

---

## 9. Mean Fluctuation

Für eine Reynolds-Mittelung gilt ideal:

```text
mean(u'_i) =
0
```

---

## 10. Reynolds Number

Die Reynolds-Zahl ist eine zentrale dimensionslose Größe:

```text
Re =
ρ * U * L / μ
```

oder:

```text
Re =
U * L / ν
```

mit:

```text
ρ = Density
U = Characteristic Velocity
L = Characteristic Length
μ = Dynamic Viscosity
ν = Kinematic Viscosity
```

---

## 11. Flow Regime

Die Runtime SOLL Strömungsregime klassifizieren können:

```text
LAMINAR
TRANSITIONAL
TURBULENT
```

Die Grenzwerte DÜRFEN nicht global fest codiert werden.

Sie hängen unter anderem ab von:

```text
Geometry
Surface Roughness
Pressure Gradient
Free-Stream Turbulence
Disturbances
Flow Type
```

---

## 12. Laminar Mode

Turbulence Compute MUSS einen Modus besitzen:

```text
LAMINAR
```

Bei diesem Modus wird keine zusätzliche turbulente Viskosität erzeugt.

```text
μ_t =
0
```

---

## 13. Turbulent Kinetic Energy

Die turbulente kinetische Energie lautet:

```text
k =
1/2
*
(
    mean(u'^2)
    +
    mean(v'^2)
    +
    mean(w'^2)
)
```

---

## 14. Turbulence Intensity

Eine typische Definition der Turbulenzintensität lautet:

```text
I =
u_rms / U
```

Für isotrope Näherungen kann eine Beziehung zu `k` verwendet werden.

---

## 15. Turbulence Length Scale

Eine Turbulenzlängenskala wird dargestellt als:

```text
L_t
```

Sie beschreibt eine charakteristische Größe energietragender Wirbel.

---

## 16. Turbulence Time Scale

Eine charakteristische Zeitskala DARF beispielsweise definiert werden als:

```text
τ_t =
k / ε
```

für geeignete Modelle.

---

## 17. Dissipation Rate

Die Dissipationsrate turbulenter kinetischer Energie wird bezeichnet als:

```text
ε
```

Einheit:

```text
m²/s³
```

---

## 18. Specific Dissipation Rate

Die spezifische Dissipationsrate wird bezeichnet als:

```text
ω
```

Einheit:

```text
1/s
```

---

## 19. Reynolds Stress

Reynolds-Spannungen werden repräsentiert durch:

```text
τ_ij^R =
-ρ * mean(u'_i * u'_j)
```

---

## 20. Reynolds Stress Tensor

Der Reynolds-Spannungstensor besitzt konzeptionell:

```text
R =
[
    uu  uv  uw
    vu  vv  vw
    wu  wv  ww
]
```

---

## 21. Reynolds-Averaged Navier-Stokes

RANS-Verfahren verwenden gemittelte Navier-Stokes-Gleichungen.

Durch die Mittelung entstehen zusätzliche Reynolds-Spannungsterme.

Diese erzeugen das Turbulenzabschlussproblem.

---

## 22. Closure Problem

Die zusätzlichen unbekannten turbulenten Größen MÜSSEN durch ein Turbulenzmodell geschlossen werden.

---

## 23. Eddy Viscosity

Viele RANS-Modelle verwenden eine turbulente Wirbelviskosität:

```text
μ_t
```

---

## 24. Boussinesq Hypothesis

Für Eddy-Viscosity-Modelle DARF die Boussinesq-Näherung verwendet werden.

Konzeptionell:

```text
-ρ * mean(u'_i * u'_j)
=
2 * μ_t * S_ij
-
2/3 * ρ * k * δ_ij
```

mit:

```text
S_ij = Mean Strain Rate Tensor
δ_ij = Kronecker Delta
```

---

## 25. Effective Viscosity

Für geeignete Modelle gilt:

```text
μ_eff =
μ
+
μ_t
```

---

## 26. Turbulence Model Interface

Jedes Turbulenzmodell MUSS über eine gemeinsame logische Schnittstelle integrierbar sein.

Mindestens:

```text
Initialize
Validate
ComputeProperties
ComputeSourceTerms
ApplyBoundaryConditions
Update
Finalize
```

---

## 27. Turbulence Model Descriptor

Ein Modell SOLL mindestens enthalten:

```text
ModelId
ModelName
ModelVersion
ModelFamily
RequiredFields
ModelConstants
Capabilities
ValidityDomain
WallTreatment
CompressibilitySupport
```

---

## 28. Model Families

Mindestens folgende Modellfamilien SOLLEN architektonisch unterstützt werden:

```text
LAMINAR
ALGEBRAIC
ONE_EQUATION_RANS
TWO_EQUATION_RANS
REYNOLDS_STRESS
LES
HYBRID_RANS_LES
DNS
CUSTOM
```

---

## 29. Algebraic Models

Algebraische Turbulenzmodelle DÜRFEN unterstützt werden.

Sie benötigen keine zusätzlichen Transportgleichungen.

---

## 30. One-Equation Models

Ein-Gleichungs-Modelle SOLLEN unterstützt werden können.

---

## 31. Spalart-Allmaras

Eine vollständige Implementierung SOLL das Spalart-Allmaras-Modell unterstützen können.

Bezeichner:

```text
SPALART_ALLMARAS
```

---

## 32. Spalart-Allmaras Field

Das Modell verwendet eine transportierte modifizierte turbulente Viskositätsgröße.

Konzeptionell:

```text
nu_tilde
```

---

## 33. Spalart-Allmaras Applications

Das Modell eignet sich insbesondere für bestimmte:

```text
External Aerodynamics
Attached Boundary Layers
Aerospace Flows
```

Die Modellwahl MUSS jedoch der konkreten Strömung entsprechen.

---

## 34. Two-Equation Models

Zwei-Gleichungs-Modelle SOLLEN als Kernmodellfamilie unterstützt werden.

---

## 35. k-epsilon Family

Mindestens folgende Familie SOLL vorgesehen werden:

```text
K_EPSILON
```

---

## 36. Standard k-epsilon

Eine Implementierung SOLL unterstützen können:

```text
STANDARD_K_EPSILON
```

---

## 37. k-epsilon Variables

Das Modell verwendet:

```text
k
ε
```

---

## 38. k-epsilon Eddy Viscosity

Eine typische Form lautet:

```text
μ_t =
ρ * C_mu * k² / ε
```

---

## 39. k Transport Equation

Konzeptionell:

```text
∂(ρk)/∂t
+
∇ · (ρUk)
=
P_k
-
ρ ε
+
Diffusion_k
+
Source_k
```

---

## 40. epsilon Transport Equation

Konzeptionell:

```text
∂(ρε)/∂t
+
∇ · (ρUε)
=
Production_ε
-
Destruction_ε
+
Diffusion_ε
+
Source_ε
```

---

## 41. RNG k-epsilon

Eine erweiterte Implementierung SOLL:

```text
RNG_K_EPSILON
```

unterstützen können.

---

## 42. Realizable k-epsilon

Eine erweiterte Implementierung SOLL:

```text
REALIZABLE_K_EPSILON
```

unterstützen können.

---

## 43. k-omega Family

Mindestens folgende Familie SOLL vorgesehen werden:

```text
K_OMEGA
```

---

## 44. Standard k-omega

Eine Implementierung SOLL:

```text
STANDARD_K_OMEGA
```

unterstützen können.

---

## 45. k-omega Variables

Das Modell verwendet:

```text
k
ω
```

---

## 46. k-omega Eddy Viscosity

Eine grundlegende Beziehung kann konzeptionell lauten:

```text
μ_t =
ρ * k / ω
```

wobei konkrete Modellkorrekturen zu berücksichtigen sind.

---

## 47. k-omega SST

Eine vollständige Implementierung SOLL unterstützen:

```text
K_OMEGA_SST
```

---

## 48. SST Principle

Das SST-Modell kombiniert Eigenschaften von:

```text
k-omega
```

in Wandnähe und:

```text
k-epsilon-like behavior
```

im Außenbereich.

---

## 49. SST Blending

SST verwendet Blending-Funktionen.

Konzeptionell:

```text
F1
F2
```

---

## 50. SST Eddy Viscosity Limiter

Eine SST-Implementierung MUSS den modellspezifischen Eddy-Viscosity-Limiter berücksichtigen.

---

## 51. Reynolds Stress Models

Reynolds-Stress-Modelle SOLLEN unterstützt werden können.

Bezeichner:

```text
RSM
```

---

## 52. RSM Principle

RSM löst Transportgleichungen für Komponenten des Reynolds-Spannungstensors.

---

## 53. RSM Fields

Mindestens:

```text
R_xx
R_yy
R_zz
R_xy
R_xz
R_yz
```

sowie eine geeignete Dissipationsgröße.

---

## 54. RSM Advantages

RSM kann für stark anisotrope Turbulenz geeigneter sein als lineare Eddy-Viscosity-Modelle.

Beispiele:

```text
Strong Swirl
Strong Curvature
Secondary Flow
Anisotropic Turbulence
```

---

## 55. RSM Cost

RSM benötigt mehr Transportgleichungen und ist typischerweise rechenintensiver.

---

## 56. Large Eddy Simulation

Turbulence Compute SOLL LES unterstützen können.

Bezeichner:

```text
LES
```

---

## 57. LES Principle

Bei LES werden große turbulente Strukturen direkt aufgelöst.

Kleine Strukturen werden durch ein Subgrid-Scale-Modell beschrieben.

---

## 58. LES Filtering

Eine LES-Größe wird konzeptionell gefiltert:

```text
φ_bar =
Filter(φ)
```

---

## 59. Subgrid Stress

Die nicht aufgelösten Skalen erzeugen einen Subgrid-Spannungstensor.

---

## 60. SGS Model Interface

LES MUSS austauschbare Subgrid-Scale-Modelle unterstützen können.

---

## 61. Smagorinsky Model

Eine Implementierung SOLL unterstützen können:

```text
SMAGORINSKY
```

---

## 62. Smagorinsky Eddy Viscosity

Konzeptionell:

```text
ν_sgs =
(C_s * Δ)² * |S|
```

mit:

```text
C_s = Smagorinsky Constant
Δ   = Filter Width
S   = Resolved Strain Rate
```

---

## 63. Dynamic Smagorinsky

Eine erweiterte Implementierung SOLL:

```text
DYNAMIC_SMAGORINSKY
```

unterstützen können.

---

## 64. WALE

Eine vollständige Implementierung SOLL:

```text
WALE
```

unterstützen können.

---

## 65. LES Filter Width

Die Filterbreite SOLL aus Mesh- und Diskretisierungsinformationen bestimmt werden können.

Beispiel:

```text
Δ =
(V_cell)^(1/3)
```

für geeignete Gitter.

---

## 66. LES Resolution

Eine LES MUSS diagnostizieren können, ob die räumliche Auflösung offensichtlich unzureichend für das gewählte Modell ist.

---

## 67. Direct Numerical Simulation

Die Architektur MUSS DNS als Turbulenzmodus repräsentieren können.

Bezeichner:

```text
DNS
```

---

## 68. DNS Principle

DNS löst alle dynamisch relevanten turbulenten Skalen direkt auf.

Es wird kein RANS- oder SGS-Turbulenzmodell verwendet.

---

## 69. DNS Requirement

DNS erfordert ausreichend feine:

```text
Spatial Resolution
Temporal Resolution
```

---

## 70. DNS Cost

DNS kann extrem hohe Rechen- und Speicheranforderungen besitzen.

Die Runtime SOLL deshalb HPC- und Distributed-Compute-Integration ermöglichen.

---

## 71. Kolmogorov Scales

DNS- und Turbulenzdiagnostik DÜRFEN Kolmogorov-Skalen verwenden.

Mindestens:

```text
Kolmogorov Length Scale
Kolmogorov Time Scale
Kolmogorov Velocity Scale
```

---

## 72. Kolmogorov Length Scale

Konzeptionell:

```text
η =
(
    ν³ / ε
)^(1/4)
```

---

## 73. Kolmogorov Time Scale

Konzeptionell:

```text
τ_η =
sqrt(
    ν / ε
)
```

---

## 74. Energy Cascade

Turbulence Compute SOLL das physikalische Konzept der Energiekaskade berücksichtigen können:

```text
Large Eddies
    |
    v
Intermediate Scales
    |
    v
Small Eddies
    |
    v
Viscous Dissipation
```

---

## 75. Turbulence Spectrum

Eine erweiterte Implementierung SOLL Energiespektren analysieren können.

---

## 76. Energy Spectrum

Das turbulente Energiespektrum wird dargestellt als:

```text
E(k)
```

wobei `k` in diesem Kontext die Wellenzahl bezeichnet und nicht mit turbulenter kinetischer Energie verwechselt werden DARF.

---

## 77. Inertial Subrange

Für geeignete isotrope Turbulenz gilt konzeptionell die Kolmogorov-Skalierung:

```text
E(κ)
proportional to
ε^(2/3) * κ^(-5/3)
```

mit:

```text
κ = Wave Number
```

---

## 78. Hybrid RANS/LES

Turbulence Compute SOLL hybride RANS/LES-Verfahren unterstützen können.

---

## 79. DES

Eine vollständige Implementierung SOLL architektonisch:

```text
DES
```

unterstützen können.

---

## 80. DDES

Optional:

```text
DDES
```

---

## 81. IDDES

Optional:

```text
IDDES
```

---

## 82. Hybrid Model Switching

Hybride Modelle MÜSSEN eindeutig definieren, wann RANS- und LES-artige Behandlung verwendet wird.

---

## 83. Near-Wall Flow

Wandnahe Strömungen sind ein zentraler Bestandteil der Turbulenzmodellierung.

---

## 84. Wall Distance

Turbulence Compute MUSS Wandabstände verwenden können.

Bezeichner:

```text
y
```

---

## 85. Wall Shear Stress

Die Wandschubspannung wird bezeichnet als:

```text
τ_w
```

---

## 86. Friction Velocity

Die Reibungsgeschwindigkeit lautet:

```text
u_tau =
sqrt(
    τ_w / ρ
)
```

---

## 87. y-plus

Die dimensionslose Wanddistanz lautet:

```text
y+ =
ρ * u_tau * y / μ
```

oder äquivalent:

```text
y+ =
u_tau * y / ν
```

---

## 88. y-plus Diagnostics

Turbulence Compute MUSS `y+` berechnen und diagnostizieren können.

---

## 89. Wall Treatment

Mindestens folgende Wandbehandlungen SOLLEN vorgesehen werden:

```text
WALL_FUNCTION
LOW_RE
AUTOMATIC
MODEL_SPECIFIC
```

---

## 90. Wall Functions

Wall Functions modellieren die wandnahe Region, anstatt sie vollständig aufzulösen.

---

## 91. Low-Re Treatment

Low-Reynolds-Behandlung löst die wandnahe Strömung bis näher an die Wand auf.

---

## 92. Automatic Wall Treatment

Eine erweiterte Implementierung SOLL automatisch zwischen geeigneten Wandbehandlungen überblenden können.

---

## 93. Law of the Wall

Die wandnahe Strömung DARF dimensionslos über:

```text
u+
y+
```

beschrieben werden.

---

## 94. Viscous Sublayer

Im viskosen Unterbereich gilt näherungsweise:

```text
u+ =
y+
```

für geeignete Bedingungen.

---

## 95. Logarithmic Layer

Im logarithmischen Bereich wird typischerweise eine Beziehung verwendet:

```text
u+ =
1/κ
*
ln(y+)
+
B
```

mit modellspezifischen Konstanten.

---

## 96. Surface Roughness

Wandrauheit MUSS optional berücksichtigt werden können.

---

## 97. Roughness Parameters

Mindestens:

```text
EquivalentSandRoughness
RoughnessHeight
RoughnessModel
```

SOLLTEN repräsentierbar sein.

---

## 98. Rough-Wall Function

Wall Functions SOLLEN Rauheitseffekte berücksichtigen können.

---

## 99. Turbulence Boundary Conditions

Turbulence Compute MUSS Randbedingungen für Turbulenzgrößen unterstützen.

---

## 100. Turbulence Inlet

Ein Einlass SOLL Turbulenz über verschiedene Parameter definieren können.

Mindestens:

```text
TurbulenceIntensity
LengthScale
k
epsilon
omega
ViscosityRatio
```

---

## 101. Intensity-Length-Scale Input

Ein Benutzer DARF beispielsweise definieren:

```text
TurbulenceIntensity
+
TurbulenceLengthScale
```

Die Runtime SOLL daraus modellspezifische Größen bestimmen können.

---

## 102. k from Turbulence Intensity

Für geeignete isotrope Annahmen DARF verwendet werden:

```text
k =
3/2
*
(U * I)²
```

---

## 103. epsilon from Length Scale

Für geeignete k-epsilon-Modelle DARF konzeptionell verwendet werden:

```text
ε =
C_mu^(3/4)
*
k^(3/2)
/
L_t
```

---

## 104. omega Initialization

Für geeignete k-omega-Modelle SOLL `ω` aus Turbulenzzuständen initialisierbar sein.

---

## 105. Turbulence Outlet

Auslassrandbedingungen SOLLEN möglichst geringe künstliche Reflexionen oder Einschränkungen der Turbulenzfelder erzeugen.

---

## 106. Wall Boundary

Modellspezifische Wandrandbedingungen MÜSSEN automatisch angewendet werden können.

---

## 107. Symmetry Boundary

Symmetrierandbedingungen SOLLEN unterstützt werden.

---

## 108. Periodic Boundary

Periodische Turbulenzrandbedingungen SOLLEN unterstützt werden können.

---

## 109. Far-Field Boundary

Aerodynamische Far-Field-Randbedingungen SOLLEN Turbulenzparameter enthalten können.

---

## 110. Turbulence Field

Turbulenzgrößen SOLLEN als Compute Fields repräsentiert werden.

Beispiele:

```text
k
epsilon
omega
nu_tilde
mu_t
ReynoldsStress
TurbulenceIntensity
```

---

## 111. Scalar Fields

Mindestens:

```text
k
epsilon
omega
mu_t
```

sind skalare Felder.

---

## 112. Tensor Fields

Reynolds-Spannungen SOLLEN als symmetrische Tensorfelder repräsentiert werden können.

---

## 113. Field Location

Felder DÜRFEN gespeichert werden an:

```text
Cell Centers
Nodes
Faces
Edges
```

abhängig vom numerischen Verfahren.

---

## 114. Mesh Independence

Die Turbulenzmodell-Schnittstelle SOLL möglichst unabhängig von einer bestimmten Mesh-Datenstruktur sein.

---

## 115. Structured Mesh

Strukturierte Gitter SOLLEN unterstützt werden können.

---

## 116. Unstructured Mesh

Unstrukturierte Gitter SOLLEN unterstützt werden können.

---

## 117. Hybrid Mesh

Hybride Gitter SOLLEN unterstützt werden können.

---

## 118. Adaptive Mesh

Turbulence Compute SOLL mit adaptiver Gitterverfeinerung kombinierbar sein.

---

## 119. Turbulence-Based Refinement

Mesh-Verfeinerung DARF anhand von Turbulenzgrößen ausgelöst werden.

Beispiele:

```text
Vorticity
k Gradient
Velocity Gradient
Q-Criterion
Wall Distance
LES Resolution Metric
```

---

## 120. Strain Rate Tensor

Turbulenzmodelle SOLLEN den Strain Rate Tensor verwenden können:

```text
S_ij =
1/2
*
(
    ∂U_i/∂x_j
    +
    ∂U_j/∂x_i
)
```

---

## 121. Rotation Tensor

Der Rotation Tensor SOLL verfügbar sein:

```text
Ω_ij =
1/2
*
(
    ∂U_i/∂x_j
    -
    ∂U_j/∂x_i
)
```

---

## 122. Vorticity

Die Wirbelstärke wird definiert als:

```text
ω_vec =
∇ × U
```

Sie MUSS vom skalaren k-omega-Modellparameter `ω` unterscheidbar sein.

---

## 123. Vorticity Magnitude

Die Runtime SOLL:

```text
|ω_vec|
```

berechnen können.

---

## 124. Turbulence Production

Die Produktion turbulenter kinetischer Energie wird konzeptionell bezeichnet als:

```text
P_k
```

---

## 125. Production Limiter

Turbulenzmodelle DÜRFEN modellspezifische Production Limiter verwenden.

---

## 126. Dissipation

Turbulente Energie MUSS über modellspezifische Dissipationsterme abgebaut werden können.

---

## 127. Diffusion

Transportgleichungen SOLLEN molekulare und turbulente Diffusion berücksichtigen.

---

## 128. Cross Diffusion

Modelle wie SST DÜRFEN Cross-Diffusion-Terme verwenden.

---

## 129. Source Terms

Turbulenztransportgleichungen MÜSSEN erweiterbare Source Terms unterstützen.

---

## 130. Buoyancy

Auftriebseffekte SOLLEN optional berücksichtigt werden können.

---

## 131. Buoyancy Production

Temperatur- oder Dichtegradienten DÜRFEN Turbulenzproduktion beeinflussen.

---

## 132. Compressibility Effects

Kompressible Turbulenz SOLL unterstützt werden können.

---

## 133. Dilatation Effects

Bei kompressibler Turbulenz DÜRFEN Dilatationskorrekturen verwendet werden.

---

## 134. Turbulent Mach Number

Eine turbulente Mach-Zahl DARF definiert werden als:

```text
M_t =
sqrt(2k) / a
```

für geeignete Modelle.

---

## 135. Compressibility Correction

Turbulenzmodelle DÜRFEN Kompressibilitätskorrekturen besitzen.

---

## 136. Rotation Correction

Rotationskorrekturen SOLLEN optional unterstützt werden.

---

## 137. Curvature Correction

Krümmungseffekte SOLLEN optional berücksichtigt werden können.

---

## 138. Transition

Laminar-turbulente Transition SOLL als Erweiterung integrierbar sein.

---

## 139. Transition Model Interface

Transition-Modelle SOLLEN auf Turbulenzmodelle aufgesetzt werden können.

---

## 140. Intermittency

Transition-Modelle DÜRFEN ein Intermittency-Feld verwenden:

```text
gamma_transition
```

---

## 141. Transition Reynolds Number

Transition-Modelle DÜRFEN zusätzliche Reynolds-Zahl-Felder verwenden.

---

## 142. Heat Transfer

Turbulence Compute SOLL turbulenten Wärmetransport unterstützen können.

---

## 143. Turbulent Thermal Conductivity

Eine turbulente Wärmeleitfähigkeit DARF aus turbulenter Viskosität abgeleitet werden.

---

## 144. Turbulent Prandtl Number

Die turbulente Prandtl-Zahl lautet:

```text
Pr_t
```

---

## 145. Turbulent Heat Flux

Für Gradient-Diffusion-Modelle DARF turbulenter Wärmetransport über `Pr_t` modelliert werden.

---

## 146. Species Transport

Turbulenter Stofftransport SOLL unterstützt werden können.

---

## 147. Turbulent Schmidt Number

Die turbulente Schmidt-Zahl lautet:

```text
Sc_t
```

---

## 148. Turbulent Mass Diffusion

Gradient-Diffusion-Modelle DÜRFEN `Sc_t` zur Beschreibung turbulenter Stoffdiffusion verwenden.

---

## 149. Combustion Integration

Turbulence Compute SOLL Schnittstellen für Turbulenz-Verbrennungs-Kopplung bereitstellen können.

---

## 150. Turbulence-Chemistry Interaction

Eine zukünftige Erweiterung DARF Modelle integrieren wie:

```text
Eddy Dissipation
Flamelet
PDF
EDC
Custom
```

---

## 151. Multiphase Integration

Mehrphasenmodelle DÜRFEN Turbulenzinformationen verwenden.

---

## 152. Turbulent Dispersion

Turbulente Dispersion von Partikeln oder Phasen SOLL als Erweiterung integrierbar sein.

---

## 153. Turbulence Initialization

Turbulenzfelder MÜSSEN initialisierbar sein.

---

## 154. Uniform Initialization

Ein Feld DARF gleichmäßig initialisiert werden.

---

## 155. Field Initialization

Turbulenzgrößen DÜRFEN aus vorhandenen Feldern geladen werden.

---

## 156. Restart Initialization

Ein Turbulenzzustand SOLL aus einem Checkpoint wiederhergestellt werden können.

---

## 157. Automatic Initialization

Die Runtime SOLL aus Strömungs- und Randbedingungen geeignete Startwerte abschätzen können.

---

## 158. Positivity

Physikalisch positive Turbulenzgrößen MÜSSEN kontrolliert behandelt werden.

Insbesondere:

```text
k >= 0
epsilon > 0
omega > 0
```

innerhalb der Semantik des verwendeten Modells.

---

## 159. Positivity Preservation

Numerische Verfahren SOLLEN verhindern, dass unphysikalisch negative Turbulenzgrößen entstehen.

---

## 160. Clipping

Numerisches Clipping DARF als Schutzmechanismus verwendet werden.

Es MUSS diagnostizierbar sein.

---

## 161. Clipping Diagnostics

Mindestens:

```text
Field
CellCount
MinimumValue
AppliedLimit
```

SOLLTEN protokollierbar sein.

---

## 162. Under-Relaxation

Stationäre RANS-Verfahren SOLLEN Under-Relaxation unterstützen können.

---

## 163. Relaxation Factor

Ein Relaxation Factor SOLL pro Turbulenzfeld konfigurierbar sein.

---

## 164. Residual

Jede gelöste Turbulenztransportgleichung SOLL einen Residualwert bereitstellen.

---

## 165. Convergence Criteria

Mindestens:

```text
AbsoluteResidual
RelativeResidual
MaximumIterations
FieldChange
IntegralMonitor
```

SOLLTEN unterstützt werden.

---

## 166. Coupled Convergence

Turbulenzkonvergenz MUSS gemeinsam mit dem Fluid-Solver bewertet werden können.

---

## 167. False Convergence

Die Runtime SOLL erkennen können, wenn Turbulenzresiduen klein erscheinen, aber physikalische Monitorgrößen noch nicht konvergiert sind.

---

## 168. Steady RANS

Stationäre RANS-Berechnungen SOLLEN unterstützt werden.

---

## 169. Unsteady RANS

Zeitabhängige RANS-Berechnungen SOLLEN unterstützt werden.

Bezeichner:

```text
URANS
```

---

## 170. LES Time Integration

LES MUSS zeitabhängig ausgeführt werden.

---

## 171. DNS Time Integration

DNS MUSS zeitabhängig ausgeführt werden.

---

## 172. Time Step

Zeitabhängige Turbulenzberechnungen MÜSSEN einen definierten Zeitschritt besitzen.

---

## 173. Adaptive Time Step

Adaptive Zeitschritte SOLLEN unterstützt werden können.

---

## 174. CFL

LES- und DNS-Berechnungen SOLLEN geeignete CFL-Kriterien berücksichtigen.

---

## 175. Turbulence Time Resolution

Die Runtime SOLL diagnostizieren können, ob der Zeitschritt im Verhältnis zu relevanten turbulenten Zeitskalen zu groß ist.

---

## 176. Spatial Resolution

LES und DNS benötigen geeignete räumliche Auflösung.

---

## 177. Resolution Metric

Eine Implementierung SOLL Resolution Metrics bereitstellen können.

Beispiele:

```text
y+
Delta
Delta / IntegralLengthScale
Delta / KolmogorovScale
ResolvedTKEFraction
```

---

## 178. Resolved Turbulent Kinetic Energy

LES-Diagnostik SOLL aufgelöste turbulente kinetische Energie bestimmen können.

---

## 179. Modeled Turbulent Kinetic Energy

Die modellierte turbulente Energie SOLL, soweit das SGS-Modell dies ermöglicht, bestimmt werden können.

---

## 180. Resolved Fraction

Eine LES-Qualitätsmetrik DARF das Verhältnis zwischen aufgelöster und gesamter turbulenter Energie verwenden.

---

## 181. Statistical Sampling

LES, DNS und URANS SOLLEN statistische Auswertung unterstützen.

---

## 182. Mean Field

Zeitgemittelte Felder SOLLEN erzeugt werden können.

---

## 183. RMS Field

Root-Mean-Square-Fluktuationen SOLLEN bestimmt werden können.

---

## 184. Variance

Varianzfelder SOLLEN berechnet werden können.

---

## 185. Covariance

Kovarianzen zwischen Strömungsgrößen SOLLEN berechnet werden können.

---

## 186. Reynolds Stress from Sampling

Reynolds-Spannungen SOLLEN aus zeitaufgelösten Daten berechnet werden können.

---

## 187. Sampling Window

Statistische Auswertung MUSS einen definierten Sampling-Zeitraum besitzen.

---

## 188. Warm-Up Period

Eine anfängliche transiente Phase SOLL von der statistischen Auswertung ausgeschlossen werden können.

---

## 189. Online Statistics

Statistiken SOLLEN während der Simulation ohne Speicherung aller Zeitschritte berechnet werden können.

---

## 190. Higher-Order Statistics

Optional:

```text
Skewness
Kurtosis
Higher Moments
```

---

## 191. Correlation

Turbulence Compute SOLL Korrelationsanalysen unterstützen können.

---

## 192. Autocorrelation

Zeitliche Autokorrelation SOLL berechenbar sein.

---

## 193. Spatial Correlation

Räumliche Korrelation SOLL optional berechenbar sein.

---

## 194. Integral Time Scale

Eine turbulente integrale Zeitskala DARF aus Autokorrelationen bestimmt werden.

---

## 195. Integral Length Scale

Eine turbulente integrale Längenskala DARF aus räumlichen Korrelationen bestimmt werden.

---

## 196. Spectral Analysis

Turbulence Compute SOLL mit FFT-basierten Spektralanalysen integrierbar sein.

---

## 197. Power Spectral Density

Zeitreihen SOLLEN als Leistungsdichtespektrum analysiert werden können.

---

## 198. Frequency Analysis

Typische Anwendungen:

```text
Vortex Shedding
Pressure Fluctuations
Velocity Fluctuations
Acoustic Sources
Rotating Machinery
```

---

## 199. Strouhal Number

Die Strouhal-Zahl lautet:

```text
St =
f * L / U
```

---

## 200. Vortex Detection

Turbulence Compute SOLL Wirbeldiagnostik unterstützen können.

---

## 201. Q-Criterion

Eine Implementierung SOLL das Q-Kriterium berechnen können.

Konzeptionell:

```text
Q =
1/2
*
(
    ||Omega||²
    -
    ||S||²
)
```

---

## 202. Lambda-2 Criterion

Optional SOLL:

```text
LAMBDA_2
```

unterstützt werden.

---

## 203. Vorticity Iso-Surfaces

Turbulenzdiagnostik SOLL Felder bereitstellen können, aus denen Visualisierungen wie Vorticity-Isoflächen erzeugt werden.

---

## 204. Wall Diagnostics

Mindestens:

```text
WallShearStress
FrictionVelocity
yPlus
SkinFrictionCoefficient
```

SOLLTEN berechenbar sein.

---

## 205. Skin Friction Coefficient

Konzeptionell:

```text
C_f =
τ_w
/
(
    0.5 * ρ * U_ref²
)
```

---

## 206. Turbulent Viscosity Ratio

Eine wichtige Diagnosegröße lautet:

```text
μ_t / μ
```

---

## 207. Turbulence Production Field

`P_k` SOLL als Diagnosefeld verfügbar gemacht werden können.

---

## 208. Dissipation Field

`ε` beziehungsweise entsprechende modellspezifische Dissipationsgrößen SOLLEN visualisierbar sein.

---

## 209. Model Selection

Die Runtime DARF bei der Modellwahl unterstützen.

Sie DARF jedoch nicht ohne Kennzeichnung behaupten, ein bestimmtes Modell sei für jeden Fall physikalisch korrekt.

---

## 210. Model Selection Inputs

Mögliche Kriterien:

```text
Flow Type
Reynolds Number
Wall Resolution
Separation
Swirl
Curvature
Compressibility
Required Accuracy
Available Compute
Required Runtime
```

---

## 211. Model Cost Classes

Modelle DÜRFEN grob klassifiziert werden:

```text
LOW
MEDIUM
HIGH
VERY_HIGH
EXTREME
```

---

## 212. Typical Cost Hierarchy

Konzeptionell:

```text
Laminar
   |
   v
RANS
   |
   v
RSM
   |
   v
URANS
   |
   v
Hybrid RANS/LES
   |
   v
LES
   |
   v
DNS
```

Dies ist keine universell exakte Laufzeitreihenfolge, sondern eine allgemeine Komplexitätstendenz.

---

## 213. Accuracy

Ein höherer Rechenaufwand garantiert nicht automatisch ein korrektes Ergebnis.

Ergebnisqualität hängt zusätzlich ab von:

```text
Mesh
Boundary Conditions
Numerics
Time Step
Physical Model
Convergence
Geometry
Input Data
```

---

## 214. Model Validity

Jedes Turbulenzmodell SOLL seinen vorgesehenen Anwendungsbereich beschreiben.

---

## 215. Model Metadata

Mindestens:

```text
ModelName
Version
Family
Reference
RequiredFields
RecommendedWallTreatment
CompressibilitySupport
TransitionSupport
ValidationCases
```

---

## 216. Model Constants

Modellkonstanten MÜSSEN explizit versioniert werden.

---

## 217. Default Constants

Standardwerte SOLLEN aus einer definierten Modellversion stammen.

---

## 218. Custom Constants

Fortgeschrittene Anwendungen DÜRFEN Modellkonstanten verändern.

---

## 219. Modified Model Diagnostics

Wenn Standardkonstanten verändert wurden, MUSS das Ergebnis entsprechend gekennzeichnet werden können.

---

## 220. Custom Turbulence Models

Die Architektur SOLL benutzerdefinierte Turbulenzmodelle unterstützen können.

---

## 221. Custom Model Requirements

Ein Custom Model MUSS deklarieren:

```text
RequiredFields
TransportEquations
BoundaryConditions
ModelConstants
SourceTerms
Capabilities
```

---

## 222. Model Plugin Isolation

Fehler eines optionalen Turbulenzmodells DÜRFEN die Stabilität der Compute Runtime nicht gefährden.

---

## 223. Solver Coupling

Turbulence Compute MUSS mit dem übergeordneten Fluid-Solver gekoppelt werden können.

---

## 224. Segregated Coupling

Turbulenzgleichungen DÜRFEN getrennt von Impuls- und Druckgleichungen gelöst werden.

---

## 225. Coupled Solver

Eine erweiterte Implementierung DARF Turbulenzgrößen in einem gekoppelten Gleichungssystem lösen.

---

## 226. Explicit Coupling

Explizite Kopplung DARF verwendet werden.

---

## 227. Implicit Coupling

Implizite Kopplung SOLL unterstützt werden können.

---

## 228. Jacobian

Implizite Solver DÜRFEN Jacobian-Informationen von Turbulenzmodellen anfordern.

---

## 229. Automatic Differentiation

Turbulenzmodelle DÜRFEN automatische Differentiation verwenden.

---

## 230. Linearization

Nichtlineare Source Terms SOLLEN kontrolliert linearisiert werden können.

---

## 231. Discretization Independence

Die Turbulenzmodell-API SOLL nicht zwingend an:

```text
Finite Volume
Finite Element
Finite Difference
Spectral
Lattice-Based
```

gebunden sein.

---

## 232. Finite Volume

Finite-Volume-Solver SOLLEN vollständig unterstützt werden können.

---

## 233. Finite Element

Finite-Element-Fluid-Solver SOLLEN Turbulence Compute verwenden können.

---

## 234. High-Order Methods

High-Order-Verfahren SOLLEN integrierbar sein.

---

## 235. Spectral Methods

Spektralverfahren SOLLEN insbesondere für DNS und Forschungsanwendungen integrierbar sein.

---

## 236. Conservation

Die Kopplung eines Turbulenzmodells DARF die konservativen Grundgleichungen nicht unkontrolliert verletzen.

---

## 237. Numerical Boundedness

Turbulenzfelder SOLLEN numerisch begrenzt werden können.

---

## 238. Gradient Calculation

Turbulenzmodelle benötigen häufig Gradienten.

Die Runtime SOLL gemeinsame Gradient-Operatoren verwenden können.

---

## 239. Divergence

Divergenzoperatoren SOLLEN wiederverwendbar sein.

---

## 240. Tensor Operations

Turbulence Compute SOLL effiziente Tensoroperationen bereitstellen können.

---

## 241. CPU Execution

Eine konforme Implementierung MUSS CPU-Ausführung unterstützen.

---

## 242. SIMD

Lokale Turbulenzmodelloperationen SOLLEN SIMD-vektorisierbar sein.

---

## 243. Multi-Core

Mesh-Zellen, Elemente oder Kontrollvolumen SOLLEN parallel über CPU-Kerne verarbeitet werden können.

---

## 244. GPU Execution

Geeignete Turbulenzoperationen DÜRFEN über `NPSPEC-COMPUTE-RUNTIME-0005` ausgeführt werden.

---

## 245. GPU Candidates

Beispiele:

```text
Field Operations
Gradient Evaluation
Source Terms
Transport Equation Assembly
Residual Evaluation
LES Filtering
Statistics
Spectral Analysis
```

---

## 246. HPC Integration

Große Turbulenzsimulationen SOLLEN `NPSPEC-COMPUTE-HPC-0001` verwenden können.

---

## 247. Domain Decomposition

Große Meshes SOLLEN über mehrere Compute-Ressourcen zerlegt werden können.

---

## 248. Halo Exchange

Turbulenzfelder MÜSSEN bei Domain Decomposition zwischen benachbarten Domains ausgetauscht werden können.

---

## 249. Distributed Compute

LES und DNS SOLLEN über `NPSPEC-COMPUTE-RUNTIME-0007` verteilt ausgeführt werden können.

---

## 250. Multi-GPU

Große Turbulenzsimulationen SOLLEN mehrere GPUs verwenden können.

---

## 251. Communication Overlap

Halo Exchange SOLL mit lokaler Berechnung überlappt werden können.

---

## 252. Load Balancing

Domain Decomposition SOLL Lastverteilung berücksichtigen.

---

## 253. Adaptive Load Balancing

Bei adaptiven Meshes SOLL dynamisches Repartitioning möglich sein.

---

## 254. Memory Model

Turbulence Compute MUSS auf das allgemeine Compute Memory Model abbildbar sein.

---

## 255. Typical Memory Objects

Beispiele:

```text
VelocityField
PressureField
DensityField
ViscosityField

TurbulentKineticEnergyField
DissipationField
SpecificDissipationField
TurbulentViscosityField

ReynoldsStressField
WallDistanceField
YPlusField

GradientFields
TemporarySolverFields
StatisticsFields
```

---

## 256. Memory Layout

Die Runtime DARF zwischen:

```text
Array of Structures
Structure of Arrays
Hybrid Layout
```

wählen.

---

## 257. Structure of Arrays

Für SIMD- und GPU-Verarbeitung SOLL Structure-of-Arrays unterstützt werden.

---

## 258. Field Fusion

Mehrere lokale Feldoperationen DÜRFEN zusammengeführt werden, um Speicherbandbreite zu reduzieren.

---

## 259. Temporary Memory

Temporäre Solverfelder SOLLEN über Memory Pools wiederverwendet werden.

---

## 260. Zero-Copy

Unnötige Kopien zwischen Fluid-Solver und Turbulenzmodell SOLLEN vermieden werden.

---

## 261. Checkpointing

Turbulenzfelder MÜSSEN in vollständige Simulationscheckpoints integriert werden können.

---

## 262. Checkpoint Fields

Mindestens modellabhängig:

```text
k
epsilon
omega
nu_tilde
ReynoldsStress
SGSState
StatisticsState
```

---

## 263. Restart Compatibility

Ein Restart MUSS prüfen, ob das gespeicherte Turbulenzmodell mit dem aktuellen Modell kompatibel ist.

---

## 264. Model Change on Restart

Ein Wechsel des Turbulenzmodells beim Restart DARF nur über eine explizite Konvertierungs- oder Initialisierungsstrategie erfolgen.

---

## 265. Profiling

Turbulence Compute MUSS Profiling unterstützen.

---

## 266. Profiling Categories

Mindestens:

```text
ModelEvaluationTime
GradientTime
SourceTermTime
EquationAssemblyTime
LinearSolveTime
BoundaryConditionTime
StatisticsTime
CommunicationTime
```

---

## 267. Performance Metrics

Mindestens:

```text
CellsPerSecond
IterationsPerSecond
TimePerIteration
MemoryUsage
CommunicationFraction
```

---

## 268. Model Cost Profiling

Die Runtime SOLL den Anteil der Turbulenzmodellierung an der gesamten CFD-Laufzeit bestimmen können.

---

## 269. Diagnostics

Turbulence Compute MUSS physikalische und numerische Diagnosen bereitstellen.

---

## 270. Diagnostic Classes

Mindestens:

```text
MODEL_WARNING
BOUNDARY_WARNING
WALL_RESOLUTION_WARNING
NEGATIVE_TURBULENCE_VALUE
EXCESSIVE_CLIPPING
NON_CONVERGENCE
LES_UNDER_RESOLVED
DNS_UNDER_RESOLVED
INVALID_MODEL_INPUT
OUTSIDE_VALIDITY_RANGE
```

---

## 271. Wall Resolution Warning

Die Runtime SOLL warnen können, wenn `y+` nicht zur gewählten Wandbehandlung passt.

---

## 272. Turbulence Level Warning

Unrealistische Turbulenzintensitäten SOLLEN diagnostiziert werden können.

---

## 273. Excessive Eddy Viscosity

Extrem hohe Werte von:

```text
μ_t / μ
```

SOLLTEN diagnostizierbar sein.

---

## 274. Negative k

Unphysikalische negative Werte von:

```text
k
```

MÜSSEN erkannt werden.

---

## 275. Invalid epsilon

Nichtpositive oder numerisch ungültige Werte von:

```text
epsilon
```

MÜSSEN erkannt werden.

---

## 276. Invalid omega

Nichtpositive oder numerisch ungültige Werte von:

```text
omega
```

MÜSSEN erkannt werden.

---

## 277. NaN Handling

NaN-Werte MÜSSEN erkannt werden.

---

## 278. Infinity Handling

Unendliche Werte MÜSSEN erkannt werden.

---

## 279. Divergence Detection

Solver-Divergenz MUSS erkannt werden.

---

## 280. Error Model

Mindestens:

```text
INVALID_MODEL
INVALID_PARAMETER
INVALID_FIELD
INVALID_BOUNDARY_CONDITION
INVALID_WALL_DISTANCE
INVALID_TURBULENCE_STATE
UNSUPPORTED_FLOW_REGIME
UNSUPPORTED_MODEL_FEATURE
MODEL_INITIALIZATION_FAILED
MODEL_EVALUATION_FAILED
SOLVER_DIVERGED
SOLVER_NOT_CONVERGED
NUMERIC_FAILURE
OUT_OF_MEMORY
DEVICE_FAILURE
COMMUNICATION_FAILURE
CANCELLED
```

---

## 281. Deterministic Mode

Turbulence Compute SOLL einen deterministischen Modus unterstützen.

---

## 282. Deterministic Reductions

Statistische und globale Reduktionen SOLLEN im deterministischen Modus eine definierte Reihenfolge verwenden können.

---

## 283. Reproducibility Metadata

Ein Turbulenzergebnis SOLL mindestens referenzieren können:

```text
TurbulenceModel
ModelVersion
ModelConstants
FluidSolverVersion
MeshVersion
RuntimeVersion
PrecisionPolicy
HardwareBackend
BoundaryConditions
```

---

## 284. Validation

Turbulenzmodelle MÜSSEN gegen geeignete Referenzfälle validierbar sein.

---

## 285. Validation Classes

Mindestens:

```text
Analytical
Experimental
DNS Reference
Benchmark
Cross-Solver
Regression
```

---

## 286. Canonical Validation Cases

Eine vollständige Testbibliothek SOLL kanonische Turbulenzfälle enthalten.

---

## 287. Channel Flow

Turbulente Kanalströmung SOLL als Referenzfall verwendet werden können.

---

## 288. Flat Plate Boundary Layer

Eine turbulente Grenzschicht über einer ebenen Platte SOLL als Referenzfall verwendet werden.

---

## 289. Pipe Flow

Voll entwickelte turbulente Rohrströmung SOLL getestet werden.

---

## 290. Backward-Facing Step

Ein Rücksprung SOLL zur Validierung von Ablösung und Wiederanlagerung verwendet werden können.

---

## 291. Mixing Layer

Eine turbulente Mischschicht SOLL als Validierungsfall verfügbar sein.

---

## 292. Free Jet

Ein turbulenter Freistrahl SOLL getestet werden können.

---

## 293. Wake Flow

Nachlaufströmungen SOLLEN als Validierungsfälle verwendet werden.

---

## 294. Cylinder Flow

Umströmung eines Zylinders SOLL als instationärer Referenzfall verwendet werden können.

---

## 295. Airfoil

Aerodynamische Turbulenzmodelle SOLLEN an geeigneten Profilströmungen validiert werden können.

---

## 296. Separation

Validierungsfälle SOLLEN Strömungsablösung enthalten.

---

## 297. Adverse Pressure Gradient

Turbulenzmodelle SOLLEN gegen Strömungen mit ungünstigem Druckgradienten geprüft werden.

---

## 298. Swirling Flow

RSM und geeignete Modelle SOLLEN gegen Wirbelströmungen validiert werden können.

---

## 299. Compressible Turbulence

Kompressible Modelle SOLLEN gegen geeignete Referenzfälle getestet werden.

---

## 300. Heat Transfer Validation

Turbulenter Wärmetransport SOLL gegen bekannte Wärmeübergangsfälle validiert werden.

---

## 301. Testing — Laminar Mode

Mindestens:

```text
mu_t = 0
No Turbulence Transport Equations
Laminar Reference Flow
```

---

## 302. Testing — Reynolds Number

Mindestens:

```text
Low Reynolds Number
Transition Range
High Reynolds Number
```

---

## 303. Testing — k-epsilon

Mindestens:

```text
Initialization
Production
Dissipation
Diffusion
Boundary Conditions
Positive Fields
Convergence
```

---

## 304. Testing — k-omega

Mindestens:

```text
Initialization
Near-Wall Behavior
Free-Stream Behavior
Boundary Conditions
Convergence
```

---

## 305. Testing — SST

Mindestens:

```text
Blending Functions
Eddy Viscosity Limiter
Attached Boundary Layer
Adverse Pressure Gradient
Separated Flow
```

---

## 306. Testing — Spalart-Allmaras

Mindestens:

```text
Initialization
Wall Boundary
Attached Boundary Layer
Aerodynamic Reference Case
```

---

## 307. Testing — RSM

Mindestens:

```text
Tensor Symmetry
Normal Stresses
Shear Stresses
Anisotropic Flow
Swirl
```

---

## 308. Testing — LES

Mindestens:

```text
Filter Width
SGS Viscosity
Resolved Fluctuations
Time Integration
Statistics
```

---

## 309. Testing — DNS

Mindestens:

```text
No Turbulence Model
Spatial Resolution Checks
Temporal Resolution Checks
Energy Spectrum
```

---

## 310. Testing — Wall Treatment

Mindestens:

```text
Wall Function
Low-Re
Automatic Wall Treatment
Smooth Wall
Rough Wall
```

---

## 311. Testing — y-plus

Mindestens:

```text
Very Low y+
Wall-Resolved Range
Wall-Function Range
Excessive y+
```

---

## 312. Testing — Inlet Turbulence

Mindestens:

```text
Intensity + Length Scale
k + epsilon
k + omega
Viscosity Ratio
```

---

## 313. Testing — Positivity

Mindestens:

```text
Negative k
Zero epsilon
Negative epsilon
Zero omega
Negative omega
```

---

## 314. Testing — Clipping

Zu testen:

```text
Single Cell Clipping
Large Region Clipping
Clipping Diagnostics
Clipping Threshold
```

---

## 315. Testing — Convergence

Mindestens:

```text
Normal Convergence
Slow Convergence
Oscillation
Divergence
Maximum Iterations
```

---

## 316. Testing — Heat Transfer

Mindestens:

```text
Turbulent Prandtl Number
Wall Heat Transfer
Temperature Field Coupling
```

---

## 317. Testing — Species Transport

Mindestens:

```text
Turbulent Schmidt Number
Passive Scalar
Mixing
```

---

## 318. Testing — Compressibility

Mindestens:

```text
Low Mach
Moderate Mach
High Mach
Compressibility Correction
```

soweit vom Modell unterstützt.

---

## 319. Testing — Rotation

Rotationskorrekturen SOLLEN gegen geeignete Referenzfälle getestet werden.

---

## 320. Testing — Curvature

Krümmungskorrekturen SOLLEN gegen geeignete Referenzfälle getestet werden.

---

## 321. Testing — Statistics

Mindestens:

```text
Mean
RMS
Variance
Covariance
Reynolds Stress
```

---

## 322. Testing — Spectral Analysis

Mindestens:

```text
Known Frequency
Broadband Signal
Energy Spectrum
```

---

## 323. Testing — Vortex Diagnostics

Mindestens:

```text
Vorticity
Q-Criterion
Lambda-2
```

soweit implementiert.

---

## 324. Testing — CPU

Alle Mindestfunktionen MÜSSEN auf CPU getestet werden.

---

## 325. Testing — GPU

GPU-Ergebnisse SOLLEN innerhalb definierter numerischer Toleranzen gegen CPU-Referenzen geprüft werden.

---

## 326. Testing — Multi-Core

Ergebnisse MÜSSEN bei unterschiedlicher Threadanzahl numerisch konsistent sein.

---

## 327. Testing — Distributed

Verteilte Simulationen SOLLEN gegen äquivalente nicht verteilte Fälle geprüft werden.

---

## 328. Testing — Domain Decomposition

Mindestens:

```text
Single Domain
Two Domains
Multiple Domains
Unequal Domains
```

---

## 329. Testing — Halo Exchange

Turbulenzfelder an Domain-Grenzen MÜSSEN korrekt synchronisiert werden.

---

## 330. Testing — Checkpoint

Mindestens:

```text
Save
Restart
Statistics Restart
Model State Restore
Corrupt Checkpoint
```

---

## 331. Testing — Determinism

Deterministische Modi MÜSSEN reproduzierbare Ergebnisse liefern.

---

## 332. Testing — Performance

Mindestens:

```text
CellsPerSecond
IterationsPerSecond
MemoryBandwidth
Scaling
CommunicationFraction
```

---

## 333. Testing — Strong Scaling

Ein festes Turbulenzproblem SOLL mit steigender Ressourcenanzahl getestet werden.

---

## 334. Testing — Weak Scaling

Problemgröße und Ressourcenanzahl SOLLEN gemeinsam erhöht werden.

---

## 335. Stress Tests

Mindestens:

```text
Large Mesh
Many Fields
Long LES
Long DNS
High Rank Count
Multi-GPU
Memory Pressure
Repeated Restart
```

---

## 336. Fault Injection

Mindestens:

```text
OUT_OF_MEMORY
GPU_FAILURE
NODE_FAILURE
COMMUNICATION_FAILURE
CHECKPOINT_FAILURE
NUMERIC_FAILURE
```

---

## 337. Regression Testing

Änderungen an Turbulenzmodellen MÜSSEN gegen definierte Referenzfälle geprüft werden.

---

## 338. Model Regression

Modelländerungen SOLLEN Änderungen folgender Größen überwachen:

```text
Drag
Lift
PressureDrop
WallShear
SeparationPoint
ReattachmentPoint
HeatTransfer
TurbulenceKineticEnergy
```

---

## 339. Benchmark Metadata

Benchmark-Ergebnisse SOLLEN enthalten:

```text
Model
ModelVersion
Mesh
CellCount
Solver
Precision
Hardware
RuntimeVersion
IterationCount
ExecutionTime
```

---

## 340. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
LAMINAR

Reynolds Number

Turbulent Kinetic Energy

Turbulence Intensity
Turbulence Length Scale

Eddy Viscosity

RANS Model Interface

STANDARD_K_EPSILON
STANDARD_K_OMEGA
K_OMEGA_SST

Wall Distance
Wall Shear Stress
Friction Velocity
y+

Wall Function Support

Turbulence Inlet Conditions

Turbulence Fields

Production
Dissipation
Diffusion

Positivity Handling
Clipping Diagnostics

Residuals
Convergence Monitoring

CPU Execution

Profiling
Diagnostics
Validation
```

---

## 341. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
SPALART_ALLMARAS

RNG_K_EPSILON
REALIZABLE_K_EPSILON

RSM

LES
SMAGORINSKY
DYNAMIC_SMAGORINSKY
WALE

DNS

DES
DDES
IDDES

Transition Models

Automatic Wall Treatment
Rough Walls

Compressibility Corrections
Rotation Corrections
Curvature Corrections
Buoyancy

Turbulent Heat Transfer
Turbulent Species Transport

Statistical Sampling
RMS
Variance
Covariance

Spectral Analysis
Energy Spectrum

Vorticity
Q-Criterion
Lambda-2

Adaptive Mesh Integration

GPU Acceleration
Multi-GPU
HPC
Distributed Compute

Checkpointing
Restart

Strong Scaling
Weak Scaling

Advanced Diagnostics
```

---

## 342. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-TURBULENCE-0001` MUSS:

1. laminare und turbulente Modi unterscheiden.
2. Reynolds-Zahlen berechnen können.
3. turbulente kinetische Energie repräsentieren.
4. Turbulenzintensität repräsentieren.
5. Turbulenzlängenskalen repräsentieren.
6. turbulente Viskosität repräsentieren.
7. Turbulenzmodelle über eine gemeinsame Schnittstelle abstrahieren.
8. RANS-Modelle integrieren können.
9. mindestens ein Zwei-Gleichungs-Modell unterstützen.
10. `k` als Compute Field repräsentieren.
11. `epsilon` oder `omega` als Compute Field repräsentieren.
12. Produktions- und Dissipationsterme berechnen können.
13. Wandabstände verwenden können.
14. Wandschubspannung repräsentieren.
15. Reibungsgeschwindigkeit berechnen.
16. `y+` berechnen.
17. Wall Functions unterstützen können.
18. Turbulenzeinlassbedingungen unterstützen.
19. Turbulenzfelder initialisieren können.
20. unphysikalische negative Zustände erkennen.
21. numerische Schutzmechanismen diagnostizierbar machen.
22. Residuen bereitstellen.
23. Konvergenz überwachen.
24. Turbulenzmodellversionen speichern.
25. Modellkonstanten eindeutig definieren.
26. Modellgültigkeitsbereiche dokumentieren können.
27. CPU-Ausführung unterstützen.
28. das Compute Memory Model integrieren.
29. das Compute Object Model integrieren.
30. den Compute Scheduler integrieren.
31. GPU-Ausführung architektonisch ermöglichen.
32. HPC-Ausführung architektonisch ermöglichen.
33. Distributed Compute architektonisch ermöglichen.
34. Checkpointing integrierbar halten.
35. reproduzierbare Berechnungen unterstützen können.
36. physikalische und numerische Diagnosen bereitstellen.
37. Turbulenzmodelle von Fluid-Solver und Hardware-Backend entkoppeln.

---

## 343. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen können:

```text
NovaTurbulenceCompute
 |
 +-- TurbulenceModelManager
 |
 +-- TurbulenceFieldManager
 |
 +-- TurbulenceBoundaryManager
 |
 +-- TurbulenceWallManager
 |
 +-- TurbulenceSourceManager
 |
 +-- TurbulenceStatistics
 |
 +-- TurbulenceSpectrumAnalyzer
 |
 +-- TurbulenceVortexAnalyzer
 |
 +-- TurbulenceDiagnostics
 |
 +-- TurbulenceProfiler
 |
 +-- TurbulenceValidation
 |
 +-- RansModelManager
 |
 +-- LesModelManager
 |
 +-- HybridModelManager
 |
 +-- DnsManager
```

Diese Aufteilung ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 344. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Turbulence Compute baut auf der allgemeinen NovaOS Compute Runtime auf.

```text
Turbulence Model
       |
       v
Turbulence Compute
       |
       v
Compute Runtime
       |
       +--> CPU
       +--> GPU
       +--> Distributed
```

Die Turbulenzschicht definiert die physikalische Semantik.

Die Compute Runtime übernimmt die technische Ausführung.

---

## 345. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Compute Scheduler verteilt Turbulenzoperationen auf geeignete Compute-Ressourcen.

Beispiele:

```text
Small RANS
    |
    v
CPU

Large RANS
    |
    v
Multi-Core / GPU

LES
    |
    v
GPU / HPC

DNS
    |
    v
HPC / Distributed
```

---

## 346. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Turbulenzfelder MÜSSEN auf das allgemeine Compute Memory Model abbildbar sein.

Beispiele:

```text
k
epsilon
omega
mu_t
ReynoldsStress
WallDistance
yPlus
Statistics
```

---

## 347. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Turbulence Compute SOLL auf dem allgemeinen Compute Object Model aufbauen.

Beispiele:

```text
TurbulenceModel
TurbulenceField
WallTreatment
TurbulenceBoundaryCondition
TurbulenceStatistics
TurbulenceResult
```

---

## 348. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

Massiv parallele Feldoperationen DÜRFEN über die GPU Runtime ausgeführt werden.

Die Turbulenzmodell-Semantik MUSS unabhängig vom GPU-Backend bleiben.

---

## 349. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Große LES- und DNS-Simulationen DÜRFEN über Distributed Compute ausgeführt werden.

---

## 350. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

Turbulence Compute SOLL HPC Compute für große Simulationen verwenden.

Insbesondere:

```text
Domain Decomposition
Multi-Core
NUMA
Multi-GPU
Multi-Node
Halo Exchange
Load Balancing
Checkpointing
Profiling
```

---

## 351. Beziehung zu `NPSPEC-COMPUTE-HYDRAULICS-0001`

Hydraulics Compute DARF Turbulence Compute für turbulente Flüssigkeitsströmungen verwenden.

Beispiele:

```text
Pipe Flow
Open Channel Flow
Valves
Pumps
Hydraulic Machinery
```

---

## 352. Beziehung zu `NPSPEC-COMPUTE-GASDYNAMICS-0001`

Gas Dynamics Compute DARF Turbulence Compute für kompressible turbulente Gasströmungen verwenden.

Beispiele:

```text
Compressible Boundary Layer
Nozzle Flow
Jet Flow
Shock-Boundary-Layer Interaction
Gas Pipeline
Turbomachinery
```

---

## 353. Beziehung zu `NPSPEC-COMPUTE-AERODYNAMICS-0001`

Aerodynamics Compute verwendet Turbulence Compute für:

```text
Aircraft
Vehicles
Airfoils
Wings
Rotors
Fans
External Flow
Boundary Layers
Wakes
```

---

## 354. Gemeinsame Fluid-Compute-Architektur

Die langfristige Architektur SOLL Turbulenz als gemeinsame Fluid-Fähigkeit behandeln:

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
                            v
                    Turbulence Compute
                            |
                            v
                       HPC Compute
```

Turbulenzmodelle werden dadurch nicht mehrfach separat in Hydraulik, Gasdynamik und Aerodynamik implementiert.

---

## 355. CFD-Integration

Eine zukünftige CFD-Schicht SOLL Turbulence Compute als Modellkomponente verwenden.

```text
Geometry
   |
   v
Mesh
   |
   v
Fluid Properties
   |
   v
Navier-Stokes Solver
   |
   +--> Turbulence Compute
   |
   +--> Heat Transfer
   |
   +--> Species Transport
   |
   v
Solution
```

---

## 356. Modellhierarchie

Die Turbulenzmodellhierarchie kann konzeptionell dargestellt werden als:

```text
Turbulence
 |
 +-- Laminar
 |
 +-- RANS
 |    |
 |    +-- Algebraic
 |    |
 |    +-- One Equation
 |    |    |
 |    |    +-- Spalart-Allmaras
 |    |
 |    +-- Two Equation
 |    |    |
 |    |    +-- k-epsilon
 |    |    |
 |    |    +-- k-omega
 |    |         |
 |    |         +-- SST
 |    |
 |    +-- Reynolds Stress Model
 |
 +-- Hybrid RANS/LES
 |    |
 |    +-- DES
 |    +-- DDES
 |    +-- IDDES
 |
 +-- LES
 |    |
 |    +-- Smagorinsky
 |    +-- Dynamic Smagorinsky
 |    +-- WALE
 |
 +-- DNS
```

---

## 357. Auflösungshierarchie

Die physikalische Auflösung steigt konzeptionell:

```text
RANS
 |
 v
URANS
 |
 v
Hybrid RANS/LES
 |
 v
LES
 |
 v
DNS
```

Gleichzeitig steigt typischerweise der Compute-Bedarf.

---

## 358. Automatische Modellunterstützung

NovaOS DARF zukünftig einen Model Advisor bereitstellen.

Dieser DARF anhand von:

```text
Flow Type
Geometry
Reynolds Number
Mach Number
Wall Resolution
Available Hardware
Required Accuracy
Maximum Runtime
```

geeignete Modelle vorschlagen.

Die endgültige Modellentscheidung MUSS nachvollziehbar bleiben.

---

## 359. AI-Unterstützung

AI DARF Turbulence Compute unterstützen.

Beispiele:

```text
Model Selection
Initial Field Estimation
Convergence Prediction
Mesh Refinement Prediction
Surrogate Turbulence Model
Anomaly Detection
```

---

## 360. AI Turbulence Models

AI-basierte Turbulenzmodelle DÜRFEN als zusätzliche Modellfamilie integriert werden.

Bezeichner:

```text
AI_TURBULENCE
```

---

## 361. Physical Constraints for AI

AI-basierte Modelle SOLLEN physikalische Constraints berücksichtigen können.

Beispiele:

```text
Positivity
Conservation
Tensor Symmetry
Galilean Invariance
Rotational Invariance
Dimensional Consistency
```

---

## 362. AI Model Metadata

Ein AI-Turbulenzmodell MUSS mindestens enthalten:

```text
ModelVersion
TrainingDomain
InputFields
OutputFields
Precision
ValidationDomain
FallbackModel
```

---

## 363. AI Fallback

Ein AI-Modell SOLL ein klassisches Fallback-Modell definieren können.

Beispiel:

```text
AI Turbulence Model
        |
        | invalid / outside domain
        v
K_OMEGA_SST
```

---

## 364. Model Provenance

Die Herkunft eines Turbulenzmodells MUSS nachvollziehbar sein.

---

## 365. Validation State

Ein Modell SOLL einen Status besitzen können:

```text
EXPERIMENTAL
VALIDATED
PRODUCTION
DEPRECATED
```

---

## 366. Experimental Models

Experimentelle Modelle MÜSSEN eindeutig gekennzeichnet werden.

---

## 367. Deprecated Models

Veraltete Modelle SOLLEN weiterhin lesbar sein können, aber bei neuer Verwendung eine Warnung erzeugen.

---

## 368. Serialization

Turbulenzmodelle und deren Konfiguration SOLLEN serialisierbar sein.

---

## 369. Serialized Configuration

Mindestens:

```text
ModelId
ModelVersion
ModelConstants
WallTreatment
BoundarySettings
Initialization
NumericalOptions
```

---

## 370. Portable Configuration

Konfigurationen SOLLEN unabhängig vom Compute-Backend speicherbar sein.

---

## 371. Result Metadata

Ein Ergebnis SOLL enthalten:

```text
TurbulenceModel
ModelVersion
WallTreatment
MeshResolution
TimeStep
Iterations
ConvergenceState
Warnings
```

---

## 372. Quality Metrics

Turbulence Compute SOLL Qualitätsmetriken bereitstellen können.

Beispiele:

```text
MaximumYPlus
AverageYPlus
ResolvedTKEFraction
ClippedCellFraction
ResidualQuality
StatisticalSampleDuration
```

---

## 373. Confidence

Die Runtime DARF eine technische Qualitätsbewertung bereitstellen.

Diese DARF nicht als absolute physikalische Gewissheit dargestellt werden.

---

## 374. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-TURBULENCE-MODEL-0001
Turbulence Model Interface

NPSPEC-COMPUTE-TURBULENCE-FIELD-0001
Turbulence Field Model

NPSPEC-COMPUTE-TURBULENCE-RANS-0001
RANS Runtime

NPSPEC-COMPUTE-TURBULENCE-KEPSILON-0001
k-epsilon Model Family

NPSPEC-COMPUTE-TURBULENCE-KOMEGA-0001
k-omega Model Family

NPSPEC-COMPUTE-TURBULENCE-SST-0001
k-omega SST Model

NPSPEC-COMPUTE-TURBULENCE-SA-0001
Spalart-Allmaras Model

NPSPEC-COMPUTE-TURBULENCE-RSM-0001
Reynolds Stress Model

NPSPEC-COMPUTE-TURBULENCE-LES-0001
Large Eddy Simulation

NPSPEC-COMPUTE-TURBULENCE-SGS-0001
Subgrid Scale Models

NPSPEC-COMPUTE-TURBULENCE-DNS-0001
Direct Numerical Simulation

NPSPEC-COMPUTE-TURBULENCE-HYBRID-0001
Hybrid RANS/LES

NPSPEC-COMPUTE-TURBULENCE-TRANSITION-0001
Transition Modeling

NPSPEC-COMPUTE-TURBULENCE-WALL-0001
Wall Treatment

NPSPEC-COMPUTE-TURBULENCE-ROUGHNESS-0001
Surface Roughness

NPSPEC-COMPUTE-TURBULENCE-BOUNDARY-0001
Turbulence Boundary Conditions

NPSPEC-COMPUTE-TURBULENCE-COMPRESSIBLE-0001
Compressible Turbulence

NPSPEC-COMPUTE-TURBULENCE-HEAT-0001
Turbulent Heat Transfer

NPSPEC-COMPUTE-TURBULENCE-SPECIES-0001
Turbulent Species Transport

NPSPEC-COMPUTE-TURBULENCE-STATISTICS-0001
Turbulence Statistics

NPSPEC-COMPUTE-TURBULENCE-SPECTRUM-0001
Turbulence Spectral Analysis

NPSPEC-COMPUTE-TURBULENCE-VORTEX-0001
Vortex Identification

NPSPEC-COMPUTE-TURBULENCE-DIAG-0001
Turbulence Diagnostics

NPSPEC-COMPUTE-TURBULENCE-AI-0001
AI Turbulence Models

NPSPEC-COMPUTE-TURBULENCE-HPC-0001
HPC Turbulence Runtime

NPSPEC-COMPUTE-TURBULENCE-TEST-0001
Turbulence Conformance Tests
```

---

## 375. Zusammenfassung

Turbulence Compute stellt NovaOS eine gemeinsame Modell- und Compute-Schicht für turbulente Fluidströmungen bereit.

Die grundlegende Verarbeitung lautet:

```text
Flow State
    |
    v
Flow Regime
    |
    v
Turbulence Model
    |
    +--> Laminar
    |
    +--> RANS
    |
    +--> RSM
    |
    +--> Hybrid RANS/LES
    |
    +--> LES
    |
    +--> DNS
    |
    v
Turbulence Fields
    |
    v
Production
Dissipation
Diffusion
Wall Interaction
    |
    v
Fluid Solver
    |
    v
Updated Flow State
```

Die zentrale RANS-Zerlegung lautet:

```text
u_i =
U_i
+
u'_i
```

Die turbulente kinetische Energie lautet:

```text
k =
1/2
*
(
    mean(u'^2)
    +
    mean(v'^2)
    +
    mean(w'^2)
)
```

Die Reynolds-Spannungen lauten:

```text
τ_ij^R =
-ρ * mean(u'_i * u'_j)
```

Viele RANS-Modelle verwenden:

```text
μ_eff =
μ
+
μ_t
```

Für die wandnahe Auflösung ist insbesondere:

```text
y+ =
u_tau * y / ν
```

relevant.

Die Modellkomplexität kann abhängig von Problem und Ressourcen gewählt werden:

```text
                         Turbulence Compute
                                |
       +------------------------+------------------------+
       |                        |                        |
       v                        v                        v
      RANS                 Hybrid / LES                DNS
       |                        |                        |
       v                        v                        v
     Lower                   Higher                  Extreme
  Compute Cost             Compute Cost            Compute Cost
```

Die Ausführung bleibt von der physikalischen Modellierung getrennt:

```text
Turbulence Model
       |
       v
Turbulence Compute
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

Dadurch können verschiedene Fluid-Bereiche dieselbe Turbulenzinfrastruktur verwenden:

```text
                 Turbulence Compute
                        |
          +-------------+-------------+
          |             |             |
          v             v             v
     Hydraulics    Gas Dynamics   Aerodynamics
          |             |             |
          +-------------+-------------+
                        |
                        v
                   CFD Solver
```

Das grundlegende Architekturprinzip lautet:

> Turbulence Compute behandelt Turbulenz als eigenständige, austauschbare und hardwareunabhängige physikalische Compute-Fähigkeit. Fluid-Solver stellen Strömungszustand, Geometrie, Mesh und Randbedingungen bereit; Turbulence Compute übernimmt Turbulenzfelder, Modellgleichungen, Produktion, Dissipation, Diffusion, Wandbehandlung, statistische Auswertung, Validierung und Diagnose. Die eigentliche Ausführung kann abhängig von Problemgröße und Modell über CPU, SIMD, GPU, Multi-GPU, HPC oder Distributed Compute erfolgen, ohne die fachliche Turbulenzdefinition an ein bestimmtes Hardware- oder Solver-Backend zu binden.