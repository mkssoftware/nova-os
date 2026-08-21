# NPSPEC-COMPUTE-RAYTRACING-0001 — Ray Tracing Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Graphics / Ray Tracing  
**Komponente:** NovaOS Ray Tracing Compute  
**Bezeichner:** `NPSPEC-COMPUTE-RAYTRACING-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0001`, `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-HPC-0001`, `NPSPEC-COMPUTE-MEDVIS-0001`  
**Kurzname:** Ray Tracing Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame Ray-Tracing-Compute-Infrastruktur der NovaOS-Compute-Architektur.

Ray Tracing Compute stellt hardwareunabhängige Funktionen zur Erzeugung, Traversierung und Auswertung von Strahlen innerhalb räumlicher Szenen bereit.

Die Komponente ist nicht ausschließlich als Grafik-Renderer definiert.

Sie SOLL als allgemeine Compute-Infrastruktur für Anwendungen dienen wie:

- Ray Tracing,
- Path Tracing,
- Ray Casting,
- Volume Ray Casting,
- Sichtbarkeitsberechnung,
- Schattenberechnung,
- Reflexionen,
- Brechungen,
- Global Illumination,
- Ambient Occlusion,
- Lichttransport,
- akustische Strahlverfolgung,
- elektromagnetische Simulation,
- Sensor-Simulation,
- LiDAR-Simulation,
- medizinische Visualisierung,
- wissenschaftliche Visualisierung,
- Kollisionsabfragen,
- geometrische Queries,
- Picking,
- Distanzabfragen.

Die grundlegende Architektur lautet:

```text
Scene
  |
  v
Geometry
  |
  v
Acceleration Structure
  |
  v
Ray Generation
  |
  v
Traversal
  |
  v
Intersection
  |
  v
Hit Processing
  |
  +--------------------+
  |                    |
  v                    v
Shading            Compute Query
  |                    |
  v                    v
Secondary Rays       Result
  |
  v
Accumulation
  |
  v
Output
```

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Ray Tracing ist eine allgemeine räumliche Compute-Operation und darf nicht fest an eine bestimmte GPU-API, Rendering-API, Shader-Sprache oder Hardwarearchitektur gebunden sein.

---

## 3. Abstraktionsmodell

Ray Tracing Compute MUSS mindestens zwischen folgenden Ebenen unterscheiden:

```text
Application
Ray Tracing API
Scene Model
Acceleration Structure
Traversal
Intersection
Hit Processing
Execution Backend
Hardware
```

---

## 4. Hardwareunabhängigkeit

Die öffentliche Ray-Tracing-Schnittstelle DARF nicht voraussetzen, dass dedizierte Ray-Tracing-Hardware vorhanden ist.

---

## 5. Backend-Modell

Mindestens folgende Backend-Klassen SOLLEN architektonisch unterstützt werden:

```text
CPU
SIMD CPU
GPU Compute
GPU Hardware Ray Tracing
NPU
Distributed
Software Reference
```

---

## 6. Ray Tracing Runtime

Die zentrale Runtime wird als:

`NovaRayTracingRuntime`

repräsentiert.

---

## 7. Runtime-Aufgaben

Die Runtime SOLL verantwortlich sein für:

```text
Device Selection
Backend Selection
Scene Management
Acceleration Structures
Ray Dispatch
Synchronization
Memory Management
Scheduling
Diagnostics
Profiling
Fallback
```

---

## 8. Ray Tracing Device

Ein ausführbares Ray-Tracing-Gerät wird als:

`RayTracingDevice`

repräsentiert.

---

## 9. Device Capabilities

Ein Device MUSS seine Fähigkeiten melden können.

Mindestens:

```text
RayTracing
RayQuery
HardwareTraversal
SoftwareTraversal
MotionBlur
Instancing
ProceduralGeometry
VolumeTraversal
CallablePrograms
MaximumRecursionDepth
MaximumRayPayloadSize
MaximumAttributeSize
```

SOLLTEN abfragbar sein.

---

## 10. Capability Discovery

Anwendungen MÜSSEN Ray-Tracing-Fähigkeiten zur Laufzeit abfragen können.

---

## 11. Unsupported Features

Nicht unterstützte Fähigkeiten MÜSSEN explizit gemeldet werden.

---

## 12. Ray

Ein Strahl wird als:

`Ray`

repräsentiert.

---

## 13. Ray Definition

Ein Ray SOLL mindestens enthalten:

```text
Origin
Direction
TMin
TMax
Time
Mask
Flags
```

---

## 14. Ray Equation

Ein Strahl folgt der parametrischen Gleichung:

```text
P(t) = O + tD
```

mit:

```text
O = Origin
D = Direction
t = Ray Parameter
```

---

## 15. Ray Interval

Nur Schnittpunkte innerhalb:

```text
TMin <= t <= TMax
```

DÜRFEN als gültige Treffer behandelt werden.

---

## 16. Ray Direction

Die Ray Direction MUSS nicht zwingend normalisiert sein.

Die verwendete Konvention MUSS jedoch eindeutig definiert sein.

---

## 17. Ray Time

`Time` SOLL zeitabhängige Geometrie und Motion Blur unterstützen.

---

## 18. Ray Mask

Ray Masks SOLLEN verwendet werden können, um Geometrieklassen während der Traversierung ein- oder auszuschließen.

---

## 19. Ray Flags

Ray Flags SOLLEN Traversierungs- und Hit-Verhalten beeinflussen können.

---

## 20. Ray Types

Die Runtime SOLL unterschiedliche Ray Types unterstützen können.

Beispiele:

```text
Primary
Shadow
Reflection
Refraction
Diffuse
Specular
Visibility
Occlusion
Volume
Sensor
Collision
Custom
```

---

## 21. Ray Payload

Anwendungsspezifische Daten eines Rays werden als:

`RayPayload`

repräsentiert.

---

## 22. Payload Requirements

Ein Payload SOLL mindestens folgende Eigenschaften besitzen können:

```text
Type
Size
Alignment
Lifetime
```

---

## 23. Payload Limits

Backend-spezifische Payload-Limits MÜSSEN über Capability Discovery ermittelbar sein.

---

## 24. Ray Attributes

Schnittpunktspezifische Attribute werden als:

`RayHitAttributes`

repräsentiert.

---

## 25. Hit Attributes

Hit Attributes SOLLEN beispielsweise enthalten können:

```text
Barycentrics
Normal
UV
PrimitiveCoordinates
CustomAttributes
```

---

## 26. Scene

Eine Ray-Tracing-Szene wird als:

`RayTracingScene`

repräsentiert.

---

## 27. Scene Identity

Jede Scene MUSS eindeutig identifizierbar sein.

---

## 28. Scene Version

Scenes SOLLEN versionierbar sein.

---

## 29. Scene Contents

Eine Scene SOLL enthalten können:

```text
Geometry
Instances
Materials
Lights
Volumes
ProceduralObjects
AccelerationStructures
```

---

## 30. Scene Mutability

Die Runtime MUSS zwischen:

```text
StaticScene
DynamicScene
StreamingScene
```

unterscheiden können.

---

## 31. Geometry

Ray-Tracing-Geometrie wird als:

`RayTracingGeometry`

repräsentiert.

---

## 32. Geometry Types

Mindestens folgende Geometrietypen SOLLEN unterstützt werden:

```text
Triangles
AABB
Procedural
Curves
Spheres
Volumes
Custom
```

---

## 33. Triangle Geometry

Dreiecksgeometrie MUSS mindestens definieren:

```text
VertexBuffer
VertexFormat
VertexStride
IndexBuffer
IndexFormat
Transform
```

---

## 34. Indexed Geometry

Indexed Triangle Geometry SOLL unterstützt werden.

---

## 35. Non-Indexed Geometry

Nicht indizierte Dreiecksgeometrie SOLL unterstützt werden.

---

## 36. Vertex Formats

Die Runtime SOLL mehrere Vertex-Formate unterstützen können.

---

## 37. Geometry Attributes

Geometrien SOLLEN zusätzliche Attribute besitzen können:

```text
Normals
Tangents
UV
Colors
MaterialIds
CustomData
```

---

## 38. Procedural Geometry

Nicht dreieckbasierte Geometrien SOLLEN über benutzerdefinierte Intersection-Verfahren unterstützt werden.

---

## 39. AABB

Prozedurale Geometrie SOLL durch Axis-Aligned Bounding Boxes räumlich begrenzt werden können.

---

## 40. AABB Definition

Eine AABB MUSS mindestens enthalten:

```text
Minimum
Maximum
```

---

## 41. Invalid AABB

Ungültige AABBs MÜSSEN erkannt werden.

---

## 42. Geometry Identity

Jede Geometry MUSS eindeutig identifizierbar sein.

---

## 43. Geometry Version

Geometrien SOLLEN versionierbar sein.

---

## 44. Geometry Flags

Geometrien SOLLEN Eigenschaften deklarieren können wie:

```text
Opaque
NonOpaque
Static
Dynamic
DoubleSided
Procedural
```

---

## 45. Instances

Geometrieinstanzen werden als:

`RayTracingInstance`

repräsentiert.

---

## 46. Instance Reference

Eine Instance MUSS eine Geometry beziehungsweise Acceleration Structure referenzieren.

---

## 47. Instance Transform

Eine Instance SOLL eine Transformation besitzen können.

---

## 48. Instance Identity

Jede Instance MUSS eindeutig identifizierbar sein.

---

## 49. Instance Mask

Instances SOLLEN eigene Visibility Masks besitzen können.

---

## 50. Instance Data

Instances SOLLEN anwendungsspezifische Daten referenzieren können.

---

## 51. Instancing

Dieselbe Geometrie SOLL ohne vollständige Duplizierung mehrfach instanziiert werden können.

---

## 52. Nested Instancing

Mehrstufiges Instancing SOLL architektonisch unterstützt werden können.

---

## 53. Acceleration Structure

Eine Beschleunigungsstruktur wird als:

`RayTracingAccelerationStructure`

repräsentiert.

---

## 54. Acceleration Structure Purpose

Acceleration Structures dienen der Reduzierung der Anzahl notwendiger Ray-Primitive-Intersection-Tests.

---

## 55. Acceleration Structure Types

Mindestens folgende logischen Typen SOLLEN unterstützt werden:

```text
BLAS
TLAS
GenericBVH
CustomAccelerationStructure
```

---

## 56. BLAS

Eine Bottom-Level Acceleration Structure wird als:

`BLAS`

repräsentiert.

---

## 57. BLAS Contents

Eine BLAS SOLL eine oder mehrere Geometrien enthalten können.

---

## 58. TLAS

Eine Top-Level Acceleration Structure wird als:

`TLAS`

repräsentiert.

---

## 59. TLAS Contents

Eine TLAS SOLL Instanzen von BLAS- beziehungsweise kompatiblen Acceleration Structures enthalten können.

---

## 60. BVH

Bounding Volume Hierarchies SOLLEN als grundlegendes Beschleunigungsmodell unterstützt werden.

---

## 61. BVH Variants

Implementierungen DÜRFEN unterschiedliche BVH-Varianten verwenden:

```text
Binary BVH
BVH4
BVH8
Wide BVH
Quantized BVH
Compressed BVH
```

---

## 62. Backend Freedom

Die konkrete interne Acceleration-Structure-Repräsentation ist Backend-spezifisch.

---

## 63. Acceleration Structure Build

Acceleration Structures MÜSSEN explizit erstellt werden können.

---

## 64. Build Modes

Mindestens folgende Build-Modi SOLLEN unterstützt werden:

```text
FAST_BUILD
FAST_TRACE
BALANCED
LOW_MEMORY
DYNAMIC
```

---

## 65. Build Quality

Build Quality DARF Einfluss auf:

```text
BuildTime
TraversalPerformance
MemoryUsage
UpdatePerformance
```

haben.

---

## 66. Acceleration Structure Update

Dynamische Acceleration Structures SOLLEN aktualisierbar sein.

---

## 67. Refit

Geometrische Änderungen ohne vollständige Topologieänderung SOLLEN über Refit unterstützt werden können.

---

## 68. Rebuild

Bei ungeeigneten Änderungen SOLL ein vollständiger Rebuild möglich sein.

---

## 69. Refit Quality

Die Runtime SOLL erkennen beziehungsweise dokumentieren können, wenn wiederholte Refits die Traversierungsqualität verschlechtern.

---

## 70. Compaction

Acceleration Structures SOLLEN nach dem Build komprimiert werden können.

---

## 71. Serialization

Acceleration Structures SOLLEN optional serialisierbar sein.

---

## 72. Serialization Compatibility

Serialisierte hardwareabhängige Strukturen MÜSSEN ihre Kompatibilitätsanforderungen dokumentieren.

---

## 73. Invalid Serialized Structure

Inkompatible serialisierte Acceleration Structures DÜRFEN nicht ungeprüft verwendet werden.

---

## 74. Build Scratch Memory

Build-Prozesse SOLLEN temporären Scratch Memory verwenden können.

---

## 75. Scratch Memory Lifetime

Scratch Memory DARF nach Abschluss des Build-Prozesses wiederverwendet werden, sofern das Backend dies erlaubt.

---

## 76. Ray Generation

Die Erzeugung von Rays wird als:

`RayGeneration`

repräsentiert.

---

## 77. Ray Generation Program

Ein Ray Generation Program SOLL Rays erzeugen und Dispatch-Dimensionen auf Work Items abbilden können.

---

## 78. Dispatch

Ray-Tracing-Arbeit wird über:

`RayDispatch`

gestartet.

---

## 79. Dispatch Dimensions

Ein Dispatch SOLL mindestens unterstützen:

```text
Width
Height
Depth
```

---

## 80. One-Dimensional Dispatch

1D-Ray-Dispatches SOLLEN unterstützt werden.

---

## 81. Two-Dimensional Dispatch

2D-Ray-Dispatches SOLLEN unterstützt werden.

---

## 82. Three-Dimensional Dispatch

3D-Ray-Dispatches SOLLEN unterstützt werden.

---

## 83. Indirect Dispatch

Indirect Ray Dispatch SOLL unterstützt werden können.

---

## 84. Ray Traversal

Traversal bezeichnet die Suche potenzieller Ray-Primitive-Intersections innerhalb einer Acceleration Structure.

---

## 85. Traversal Backend

Traversal DARF ausgeführt werden über:

```text
CPU Code
SIMD Code
GPU Compute Shader
Dedicated Ray Traversal Hardware
Custom Accelerator
```

---

## 86. Traversal Correctness

Backend-Optimierungen DÜRFEN die definierte Ray-Intersection-Semantik nicht verletzen.

---

## 87. Closest Hit

Die Runtime MUSS den nächstgelegenen gültigen Treffer bestimmen können.

---

## 88. Any Hit

Die Runtime SOLL Treffer während der Traversierung benutzerdefiniert akzeptieren beziehungsweise verwerfen können.

---

## 89. Miss

Wenn kein gültiger Treffer gefunden wird, MUSS ein Miss-Zustand erzeugt werden können.

---

## 90. Intersection

Primitive-Schnittpunkttests werden als:

`RayIntersection`

repräsentiert.

---

## 91. Triangle Intersection

Triangle-Ray-Intersection MUSS unterstützt werden.

---

## 92. Procedural Intersection

Benutzerdefinierte Intersection-Verfahren SOLLEN unterstützt werden.

---

## 93. Intersection Result

Ein Intersection Result SOLL mindestens enthalten können:

```text
Hit
T
PrimitiveId
GeometryId
InstanceId
Attributes
```

---

## 94. Closest Hit Program

Ein:

`ClosestHitProgram`

SOLL den endgültig gewählten nächstgelegenen Treffer verarbeiten können.

---

## 95. Any Hit Program

Ein:

`AnyHitProgram`

SOLL Kandidatentreffer verarbeiten können.

---

## 96. Miss Program

Ein:

`MissProgram`

SOLL Rays ohne gültigen Treffer verarbeiten können.

---

## 97. Intersection Program

Ein:

`IntersectionProgram`

SOLL benutzerdefinierte Geometrien testen können.

---

## 98. Callable Program

Optional SOLLEN wiederverwendbare Callable Programs unterstützt werden.

---

## 99. Ray Tracing Pipeline

Eine ausführbare Pipeline wird als:

`RayTracingPipeline`

repräsentiert.

---

## 100. Pipeline Contents

Eine Pipeline SOLL enthalten können:

```text
RayGenerationPrograms
MissPrograms
ClosestHitPrograms
AnyHitPrograms
IntersectionPrograms
CallablePrograms
PipelineConfiguration
```

---

## 101. Pipeline Identity

Jede Pipeline MUSS eindeutig identifizierbar sein.

---

## 102. Pipeline Version

Pipelines SOLLEN versionierbar sein.

---

## 103. Pipeline Compilation

Backend-spezifische Programme SOLLEN kompiliert beziehungsweise übersetzt werden können.

---

## 104. Pipeline Cache

Kompilierte Pipelines SOLLEN zwischengespeichert werden können.

---

## 105. Pipeline Validation

Pipelines MÜSSEN vor Ausführung validiert werden.

---

## 106. Shader Binding

Die Zuordnung von Geometrien und Programmen MUSS explizit definiert werden.

---

## 107. Shader Binding Table

Backends mit Shader Binding Tables SOLLEN über eine gemeinsame Abstraktion unterstützt werden.

---

## 108. Binding Record

Ein Binding Record SOLL enthalten können:

```text
Program
Geometry
Material
Instance
LocalData
```

---

## 109. Backend-Neutral Binding

Anwendungen SOLLTEN nicht direkt von einer herstellerspezifischen Shader-Binding-Table-Repräsentation abhängig sein.

---

## 110. Ray Recursion

Sekundäre Rays SOLLEN unterstützt werden.

---

## 111. Recursion Depth

Die maximale Rekursionstiefe MUSS konfigurierbar sein.

---

## 112. Recursion Capability

Das Backend MUSS seine maximal unterstützte Rekursionstiefe melden.

---

## 113. Iterative Ray Processing

Implementierungen SOLLEN Ray-Rekursion intern iterativ beziehungsweise über Queues realisieren können.

---

## 114. Wavefront Ray Tracing

Wavefront Ray Tracing SOLL architektonisch unterstützt werden.

---

## 115. Ray Queue

Eine Ray Queue wird als:

`RayQueue`

repräsentiert.

---

## 116. Queue Types

Beispielsweise:

```text
PrimaryRayQueue
ShadowRayQueue
ReflectionRayQueue
RefractionRayQueue
VolumeRayQueue
CustomRayQueue
```

---

## 117. Queue Compaction

Ray Queues SOLLEN kompaktiert werden können.

---

## 118. Ray Sorting

Rays SOLLEN zur Verbesserung von Kohärenz sortiert beziehungsweise gruppiert werden können.

---

## 119. Sorting Keys

Mögliche Sortierkriterien:

```text
Direction
OriginRegion
Material
Geometry
RayType
Depth
```

---

## 120. Divergence Reduction

GPU-Backends SOLLEN Ray Sorting zur Verringerung von Divergenz einsetzen können.

---

## 121. Shading

Ray Tracing Compute SOLL Shading-Operationen unterstützen können.

---

## 122. Material

Ein Material wird als:

`RayTracingMaterial`

repräsentiert.

---

## 123. Material Parameters

Materialien SOLLEN anwendungsspezifische Parameter enthalten können.

---

## 124. Material Independence

Das Kernsystem DARF kein bestimmtes Materialmodell erzwingen.

---

## 125. Standard Material Models

Optionale Bibliotheken SOLLEN Modelle bereitstellen können wie:

```text
Lambert
Phong
BlinnPhong
CookTorrance
DisneyPrincipled
CustomBSDF
```

---

## 126. BSDF

Bidirectional Scattering Distribution Functions SOLLEN unterstützt werden können.

---

## 127. BRDF

Bidirectional Reflectance Distribution Functions SOLLEN unterstützt werden können.

---

## 128. BTDF

Bidirectional Transmittance Distribution Functions SOLLEN unterstützt werden können.

---

## 129. Emission

Emissive Materialien SOLLEN unterstützt werden.

---

## 130. Reflection

Reflexionsstrahlen SOLLEN unterstützt werden.

---

## 131. Refraction

Brechungsstrahlen SOLLEN unterstützt werden.

---

## 132. Fresnel

Fresnel-basierte Reflexions- und Transmissionsmodelle SOLLEN unterstützt werden können.

---

## 133. Total Internal Reflection

Totalreflexion MUSS mathematisch korrekt behandelt werden können.

---

## 134. Lighting

Ray Tracing Compute SOLL Lichtquellen abstrahieren können.

---

## 135. Light Types

Optionale Grafikbibliotheken SOLLEN unterstützen können:

```text
Point
Directional
Spot
Area
Environment
EmissiveGeometry
```

---

## 136. Shadow Rays

Shadow Rays SOLLEN zur Sichtbarkeitsprüfung zwischen Oberflächen und Lichtquellen verwendet werden können.

---

## 137. Early Termination

Shadow Rays SOLLEN beim ersten blockierenden Treffer beendet werden können.

---

## 138. Global Illumination

Mehrfacher Lichttransport SOLL unterstützt werden können.

---

## 139. Path Tracing

Path Tracing SOLL auf der Ray-Tracing-Infrastruktur implementierbar sein.

---

## 140. Path

Ein Lichttransportpfad wird als:

`RayPath`

repräsentiert.

---

## 141. Path Depth

Die aktuelle Path Depth MUSS nachvollziehbar sein.

---

## 142. Throughput

Path-Tracing-Implementierungen SOLLEN einen Throughput-Wert pro Pfad führen können.

---

## 143. Russian Roulette

Probabilistische Pfadterminierung SOLL unterstützt werden können.

---

## 144. Russian Roulette Bias

Russian Roulette DARF bei korrekter Gewichtung keinen systematischen Bias einführen.

---

## 145. Sampling

Stochastische Ray-Tracing-Verfahren SOLLEN Sampling-Infrastruktur verwenden können.

---

## 146. Random Number Generation

Die Runtime SOLL reproduzierbare Zufallszahlengeneratoren unterstützen können.

---

## 147. Seed

Random Seeds SOLLEN explizit gesetzt werden können.

---

## 148. Deterministic Sampling

Ein deterministischer Sampling-Modus SOLL unterstützt werden können.

---

## 149. Sampling Sequences

Optionale Sequenzen können umfassen:

```text
PseudoRandom
Sobol
Halton
BlueNoise
Stratified
Custom
```

---

## 150. Importance Sampling

Importance Sampling SOLL implementierbar sein.

---

## 151. Multiple Importance Sampling

Multiple Importance Sampling SOLL implementierbar sein.

---

## 152. Accumulation

Mehrere Samples SOLLEN akkumuliert werden können.

---

## 153. Accumulation Buffer

Ein:

`RayAccumulationBuffer`

SOLL Sample-Ergebnisse speichern können.

---

## 154. Sample Count

Die Anzahl akkumulierter Samples MUSS nachvollziehbar sein.

---

## 155. Progressive Rendering

Path-Tracing-Ergebnisse SOLLEN progressiv verbessert werden können.

---

## 156. Accumulation Reset

Änderungen relevanter Szenenparameter SOLLEN eine kontrollierte Invalidierung der Akkumulation auslösen.

---

## 157. Motion Blur

Zeitabhängige Ray Queries SOLLEN Motion Blur unterstützen können.

---

## 158. Motion Transform

Instanzen SOLLEN zeitabhängige Transformationen besitzen können.

---

## 159. Time Interval

Motion-Daten SOLLEN für ein definiertes Zeitintervall gültig sein.

---

## 160. Deforming Geometry

Deformierende Geometrie SOLL über aktualisierbare Acceleration Structures unterstützt werden können.

---

## 161. Volume Ray Tracing

Ray Tracing Compute SOLL volumetrische Ray-Verarbeitung unterstützen.

---

## 162. Volume

Ein Volumen wird als:

`RayTracingVolume`

repräsentiert.

---

## 163. Volume Types

Unterstützbare Volumentypen umfassen:

```text
DenseGrid
SparseGrid
BrickedVolume
ProceduralVolume
```

---

## 164. Volume Sampling

Volumetrische Daten SOLLEN entlang eines Rays gesampelt werden können.

---

## 165. Volume Step Size

Sampling-Schrittweite SOLL konfigurierbar sein.

---

## 166. Adaptive Volume Sampling

Adaptive Sampling-Schrittweiten SOLLEN unterstützt werden können.

---

## 167. Empty Space Skipping

Leere Volumenregionen SOLLEN übersprungen werden können.

---

## 168. Volume Transfer Function

Volumen SOLLEN Transfer Functions verwenden können.

---

## 169. Volume Integration

Volumetrische Beiträge SOLLEN entlang des Rays integriert werden können.

---

## 170. Early Ray Termination

Volumen-Rays SOLLEN bei ausreichender Opazität frühzeitig beendet werden können.

---

## 171. Sparse Volumes

Sparse Volume Representations SOLLEN unterstützt werden können.

---

## 172. Large Volumes

Volumen größer als verfügbarer Gerätespeicher SOLLEN architektonisch unterstützt werden.

---

## 173. Out-of-Core Volume Traversal

Out-of-Core-Volumen SOLLEN über Streaming und räumliche Unterteilung verarbeitet werden können.

---

## 174. Ray Query

Eine direkte geometrische Ray-Abfrage wird als:

`RayQuery`

repräsentiert.

---

## 175. Ray Query Purpose

Ray Queries SOLLEN unabhängig von einer vollständigen Rendering-Pipeline verwendet werden können.

---

## 176. Query Types

Mindestens folgende Queries SOLLEN unterstützt werden:

```text
ClosestHit
AnyHit
Occlusion
Visibility
Distance
Custom
```

---

## 177. Query Result

Ein Query Result wird als:

`RayQueryResult`

repräsentiert.

---

## 178. Query Result Fields

Ein Result SOLL enthalten können:

```text
Hit
Distance
Position
Normal
PrimitiveId
GeometryId
InstanceId
Attributes
```

---

## 179. Batch Queries

Viele Ray Queries SOLLEN gebündelt ausgeführt werden können.

---

## 180. Async Queries

Ray Queries SOLLEN asynchron ausgeführt werden können.

---

## 181. Picking

Objektauswahl durch einen View Ray SOLL auf Ray Queries aufbauen können.

---

## 182. Collision Queries

Ray-basierte Kollisionsabfragen SOLLEN auf derselben Infrastruktur aufbauen können.

---

## 183. Visibility Queries

Sichtbarkeitsabfragen SOLLEN auf derselben Infrastruktur aufbauen können.

---

## 184. Sensor Simulation

Ray Tracing Compute SOLL als Basis für virtuelle Sensoren dienen können.

---

## 185. Sensor Ray

Ein Sensorstrahl SOLL zusätzliche Sensormetadaten referenzieren können.

---

## 186. LiDAR

LiDAR-Simulation SOLL auf Ray Queries implementierbar sein.

---

## 187. LiDAR Result

Ein LiDAR-Ray kann beispielsweise liefern:

```text
Distance
Position
Normal
Material
Intensity
ReturnIndex
```

---

## 188. Multiple Returns

LiDAR-Simulation SOLL mehrere Returns pro Ray unterstützen können.

---

## 189. Depth Sensor

Depth-Sensor-Simulation SOLL unterstützt werden können.

---

## 190. Camera Simulation

Kamera-Ray-Generierung SOLL unterstützt werden können.

---

## 191. Lens Models

Optionale Kameraimplementierungen SOLLEN unterschiedliche Linsenmodelle unterstützen können.

---

## 192. Acoustic Ray Tracing

Akustische Strahlverfolgung SOLL auf der allgemeinen Ray-Tracing-Infrastruktur implementierbar sein.

---

## 193. Acoustic Payload

Akustische Rays SOLLEN Payload-Daten führen können wie:

```text
Frequency
Energy
Phase
TravelTime
```

---

## 194. Acoustic Reflection

Materialabhängige akustische Reflexion SOLL implementierbar sein.

---

## 195. Acoustic Absorption

Materialabhängige Absorption SOLL implementierbar sein.

---

## 196. Electromagnetic Ray Tracing

Elektromagnetische beziehungsweise hochfrequente geometrische Optik SOLL auf der Infrastruktur implementierbar sein.

---

## 197. Domain Independence

Grafische Annahmen DÜRFEN nicht Bestandteil der fundamentalen Ray-Tracing-Semantik sein.

---

## 198. Coordinate System

Jede Scene MUSS ein definiertes Koordinatensystem besitzen.

---

## 199. Coordinate Precision

Die Runtime SOLL unterschiedliche numerische Präzision unterstützen können.

---

## 200. Precision Modes

Mindestens architektonisch:

```text
FP32
FP64
MixedPrecision
```

SOLLTEN möglich sein.

---

## 201. FP64

CPU- und geeignete Compute-Backends SOLLEN FP64-Ray-Queries unterstützen können.

---

## 202. Large World Coordinates

Große Koordinatenbereiche SOLLEN über geeignete Precision-Strategien unterstützt werden können.

---

## 203. Ray Epsilon

Implementierungen SOLLEN numerisch robuste Verfahren gegen Self-Intersection verwenden.

---

## 204. Fixed Global Epsilon

Ein universeller fest codierter Epsilon-Wert SOLLTE vermieden werden.

---

## 205. Origin Offset

Robuste Ray-Origin-Offset-Verfahren SOLLEN verwendet werden können.

---

## 206. NaN Handling

NaN-Werte in Rays, Geometrien oder Transformationen MÜSSEN kontrolliert behandelt werden.

---

## 207. Infinity Handling

Unendliche beziehungsweise nicht endliche Geometriewerte MÜSSEN validiert werden.

---

## 208. Degenerate Geometry

Degenerierte Primitive MÜSSEN kontrolliert behandelt werden.

---

## 209. Zero-Length Direction

Rays mit ungültiger Nullrichtung MÜSSEN erkannt werden.

---

## 210. Scheduler Integration

Ray Tracing Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 211. Ray Tracing Tasks

Mindestens folgende Tasks SOLLEN planbar sein:

```text
AccelerationBuild
AccelerationUpdate
RayDispatch
RayQuery
VolumeTraversal
Denoising
Compaction
Streaming
```

---

## 212. Task Priority

Ray-Tracing-Aufgaben SOLLEN Prioritäten besitzen können.

---

## 213. Interactive Priority

Interaktive Ray-Tracing-Aufgaben SOLLEN gegenüber nicht sichtbaren Hintergrundaufgaben priorisiert werden können.

---

## 214. Cancellation

Ray-Tracing-Jobs MÜSSEN abbrechbar sein, soweit Backend und Ausführungszustand dies zulassen.

---

## 215. Obsolete Work

Veraltete progressive Renderjobs SOLLEN verworfen werden können.

---

## 216. Async Execution

Ray-Tracing-Aufgaben SOLLEN asynchron ausgeführt werden können.

---

## 217. Dependencies

Task-Abhängigkeiten SOLLEN explizit definiert werden können.

---

## 218. Example Dependency

```text
Geometry Upload
      |
      v
BLAS Build
      |
      v
TLAS Build
      |
      v
Ray Dispatch
```

---

## 219. Memory Model Integration

Ray Tracing Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 220. Memory Objects

Mindestens folgende Speicherobjekte SOLLEN unterstützt werden:

```text
VertexBuffer
IndexBuffer
AABBBuffer
InstanceBuffer
AccelerationStructureBuffer
ScratchBuffer
RayBuffer
PayloadBuffer
HitBuffer
ShaderTableBuffer
AccumulationBuffer
VolumeBuffer
OutputBuffer
```

---

## 221. Memory Residency

Die Runtime SOLL die Residency großer Ray-Tracing-Ressourcen verwalten können.

---

## 222. GPU Memory Budget

GPU-Backends MÜSSEN verfügbare Speicherbudgets berücksichtigen.

---

## 223. Out-of-Core Geometry

Geometrie größer als verfügbarer Gerätespeicher SOLL architektonisch verarbeitet werden können.

---

## 224. Geometry Streaming

Geometrie SOLL bei Bedarf gestreamt werden können.

---

## 225. Acceleration Streaming

Geeignete Acceleration-Structure-Teile SOLLEN streambar beziehungsweise rekonstruierbar sein.

---

## 226. Resource Lifetime

Ressourcenlebenszeiten MÜSSEN explizit verwaltet werden.

---

## 227. Shared Geometry

Mehrere Scenes beziehungsweise Pipelines SOLLEN unveränderliche Geometrie gemeinsam nutzen können.

---

## 228. Copy Avoidance

Unnötige Kopien großer Geometrie- und Volumendaten SOLLEN vermieden werden.

---

## 229. Object Model Integration

Ray Tracing Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0004` integrieren.

---

## 230. Compute Objects

Mindestens folgende Objekte SOLLEN auf dem gemeinsamen Object Model aufbauen:

```text
RayTracingDevice
RayTracingScene
RayTracingGeometry
RayTracingInstance
RayTracingAccelerationStructure
RayTracingPipeline
RayDispatch
RayQuery
RayTracingVolume
RayTracingMaterial
```

---

## 231. GPU Runtime Integration

Ray Tracing Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` integrieren.

---

## 232. Hardware Ray Tracing

Dedizierte GPU-Ray-Tracing-Hardware SOLL verwendet werden können, wenn verfügbar.

---

## 233. GPU Compute Fallback

GPU-Backends ohne dedizierte Ray-Tracing-Hardware SOLLEN softwarebasierte Traversierung über Compute unterstützen können.

---

## 234. CPU Fallback

Eine CPU-basierte Referenz- beziehungsweise Fallback-Implementierung SOLL verfügbar sein.

---

## 235. Backend Equivalence

Unterschiedliche Backends SOLLTEN innerhalb definierter numerischer Toleranzen semantisch äquivalente Ergebnisse erzeugen.

---

## 236. NPU Integration

NPU-Beschleunigung SOLL für geeignete ergänzende Operationen verwendet werden können.

---

## 237. NPU Operations

Geeignete Operationen können umfassen:

```text
Denoising
Upscaling
Ray Reconstruction
Sampling Prediction
Adaptive Quality Control
```

---

## 238. Ray Traversal on NPU

Die Architektur DARF zukünftige NPU- beziehungsweise AI-basierte Traversal-Beschleunigung nicht ausschließen.

---

## 239. Distributed Compute

Ray Tracing Compute SOLL verteilte Ausführung unterstützen können.

---

## 240. Distributed Strategies

Mögliche Strategien umfassen:

```text
Frame Distribution
Tile Distribution
Sample Distribution
Ray Distribution
Scene Partitioning
```

---

## 241. Tile Distribution

Ein Bild beziehungsweise Output-Raster SOLL in unabhängige Tiles unterteilt werden können.

---

## 242. Sample Distribution

Samples desselben Outputs SOLLEN auf mehrere Nodes verteilt werden können.

---

## 243. Scene Partitioning

Sehr große Szenen SOLLEN räumlich auf mehrere Compute Nodes verteilt werden können.

---

## 244. Distributed Accumulation

Teilergebnisse SOLLEN deterministisch beziehungsweise kontrolliert zusammengeführt werden können.

---

## 245. Distributed Failure

Der Ausfall eines Compute Nodes MUSS kontrolliert behandelt werden.

---

## 246. Determinism

Ray Tracing Compute SOLL einen deterministischen Modus unterstützen.

---

## 247. Deterministic Inputs

Deterministische Ausführung SOLL mindestens berücksichtigen:

```text
SceneVersion
PipelineVersion
RandomSeed
DispatchDimensions
BackendConfiguration
PrecisionMode
```

---

## 248. Floating Point Variance

Backendabhängige Floating-Point-Abweichungen MÜSSEN über definierte Toleranzen berücksichtigt werden.

---

## 249. Reference Backend

Eine Software-Referenzimplementierung SOLL für Validierung verfügbar sein können.

---

## 250. Denoising

Ray-Tracing-Ergebnisse SOLLEN optional entrauscht werden können.

---

## 251. Denoiser Separation

Denoising MUSS als separate Verarbeitung vom eigentlichen Ray-Tracing-Ergebnis unterscheidbar sein.

---

## 252. Denoiser Inputs

Denoiser SOLLEN zusätzliche Buffers verwenden können:

```text
Color
Albedo
Normal
Depth
Motion
Variance
```

---

## 253. Denoiser Provenance

Entrauschte Ergebnisse SOLLEN den verwendeten Denoiser und seine Parameter dokumentieren können.

---

## 254. Ray Reconstruction

AI- beziehungsweise algorithmische Ray Reconstruction SOLL als optionale Post- beziehungsweise Reconstruction-Stufe unterstützt werden können.

---

## 255. Reconstruction Separation

Rekonstruierte Samples DÜRFEN nicht mit tatsächlich berechneten Ray Samples verwechselt werden.

---

## 256. Adaptive Sampling

Die Runtime SOLL adaptive Sample-Verteilung unterstützen können.

---

## 257. Variance Estimation

Adaptive Sampling SOLL auf Varianz- beziehungsweise Fehlerschätzungen basieren können.

---

## 258. Sample Budget

Ein maximales Sample Budget SOLL definierbar sein.

---

## 259. Time Budget

Ein maximales Zeitbudget SOLL definierbar sein.

---

## 260. Convergence

Path-Tracing-Implementierungen SOLLEN Konvergenzmetriken bereitstellen können.

---

## 261. Quality State

Ein Ray-Tracing-Ergebnis SOLL einen Qualitätszustand besitzen können.

---

## 262. Quality States

Mindestens:

```text
FULL
PROGRESSIVE
REDUCED
DENOISED
RECONSTRUCTED
DEGRADED
INVALID
```

SOLLTEN unterstützt werden.

---

## 263. Result

Ein Ray-Tracing-Ergebnis wird als:

`RayTracingResult`

repräsentiert.

---

## 264. Result Metadata

Ein Result SOLL enthalten können:

```text
SceneId
SceneVersion
PipelineId
PipelineVersion
Backend
Device
Dispatch
SampleCount
Quality
Timestamp
ExecutionTime
```

---

## 265. Result Provenance

Ergebnisse MÜSSEN auf die verwendete Scene und Pipeline zurückführbar sein.

---

## 266. Profiling

Ray Tracing Compute MUSS Profiling unterstützen.

---

## 267. Profiling Stages

Mindestens folgende Bereiche SOLLTEN separat messbar sein:

```text
ScenePreparation
GeometryUpload
BLASBuild
TLASBuild
AccelerationUpdate
RayGeneration
Traversal
Intersection
HitProcessing
Shading
SecondaryRayGeneration
Accumulation
Denoising
Output
```

---

## 268. Ray Count

Die Anzahl verarbeiteter Rays SOLL messbar sein.

---

## 269. Rays Per Second

Ray Throughput SOLL als:

```text
RaysPerSecond
```

messbar sein.

---

## 270. Intersection Count

Die Anzahl durchgeführter Intersection Tests SOLL messbar sein können.

---

## 271. Traversal Steps

Traversal-Schritte SOLLEN messbar sein können.

---

## 272. Hit Rate

Trefferquote SOLL messbar sein.

---

## 273. Miss Rate

Miss-Rate SOLL messbar sein.

---

## 274. Acceleration Build Time

Build-Zeit von Acceleration Structures SOLL messbar sein.

---

## 275. Acceleration Memory

Speicherverbrauch von Acceleration Structures SOLL messbar sein.

---

## 276. Ray Depth Distribution

Die Verteilung der Ray Depth SOLL messbar sein können.

---

## 277. Queue Statistics

Wavefront-Implementierungen SOLLEN Queue-Größen und Queue-Auslastung messen können.

---

## 278. GPU Occupancy

Geeignete GPU-Backends SOLLEN Occupancy-Metriken bereitstellen können.

---

## 279. Divergence

Geeignete GPU-Backends SOLLEN Divergenzmetriken bereitstellen können.

---

## 280. Cache Metrics

Cache-Hits und Cache-Misses SOLLEN messbar sein können.

---

## 281. Diagnostics

Ray Tracing Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 282. Diagnostic Classes

Mindestens folgende Diagnosen SOLLEN existieren:

```text
INVALID_RAY
INVALID_DIRECTION
INVALID_SCENE
INVALID_GEOMETRY
INVALID_TRANSFORM
INVALID_AABB
INVALID_INSTANCE
INVALID_ACCELERATION_STRUCTURE
ACCELERATION_BUILD_FAILED
ACCELERATION_UPDATE_FAILED
INVALID_PIPELINE
PIPELINE_COMPILATION_FAILED
INVALID_BINDING
INVALID_PAYLOAD
UNSUPPORTED_FEATURE
UNSUPPORTED_GEOMETRY
UNSUPPORTED_PRECISION
DEVICE_UNAVAILABLE
GPU_UNAVAILABLE
OUT_OF_MEMORY
GPU_OUT_OF_MEMORY
INVALID_VOLUME
INVALID_DISPATCH
RAY_RECURSION_LIMIT
BACKEND_FAILURE
DEVICE_LOST
STREAMING_FAILED
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

Diagnosen SOLLEN Kontext enthalten können:

```text
SceneId
GeometryId
InstanceId
PipelineId
DeviceId
DispatchId
Backend
Timestamp
```

---

## 285. Security

Ray Tracing Compute MUSS in die NovaOS-Sicherheitsarchitektur integriert werden.

---

## 286. Untrusted Geometry

Nicht vertrauenswürdige Geometriedaten MÜSSEN validiert werden.

---

## 287. Buffer Validation

Buffer-Größen, Offsets, Strides und Indizes MÜSSEN validiert werden.

---

## 288. Shader Validation

Dynamisch bereitgestellte Ray-Tracing-Programme MÜSSEN validiert beziehungsweise isoliert werden.

---

## 289. Resource Limits

Ray-Tracing-Workloads MÜSSEN Ressourcenlimits unterstützen.

---

## 290. Recursion Limits

Rekursionstiefen MÜSSEN begrenzt werden können.

---

## 291. Dispatch Limits

Dispatch-Größen MÜSSEN gegen Device Limits validiert werden.

---

## 292. Memory Exhaustion

Speichererschöpfung MUSS kontrolliert behandelt werden.

---

## 293. Denial of Service

Untrusted Workloads DÜRFEN nicht unbegrenzt Compute-, Speicher- oder Traversierungsressourcen belegen.

---

## 294. Isolation

Ray-Tracing-Ressourcen unterschiedlicher Sicherheitsdomänen SOLLEN isoliert werden können.

---

## 295. Testing — Ray

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
NormalizedDirection
NonNormalizedDirection
ZeroDirection
ShortRay
LongRay
TMin
TMax
InvalidInterval
```

---

## 296. Testing — Triangle Intersection

Mindestens:

```text
CenterHit
EdgeHit
VertexHit
Miss
ParallelRay
BackFace
DegenerateTriangle
```

MÜSSEN getestet werden.

---

## 297. Testing — AABB

Mindestens:

```text
Inside
Outside
Boundary
ParallelAxis
ZeroThickness
InvalidBounds
```

MÜSSEN getestet werden.

---

## 298. Testing — Acceleration Structure

Mindestens:

```text
EmptyScene
SinglePrimitive
ManyPrimitives
StaticBuild
DynamicBuild
Refit
Rebuild
Compaction
```

MÜSSEN getestet werden.

---

## 299. Testing — Instancing

Mindestens:

```text
SingleInstance
ManyInstances
TranslatedInstance
RotatedInstance
ScaledInstance
NestedInstance
MaskedInstance
```

SOLLTEN getestet werden.

---

## 300. Testing — Procedural Geometry

Mindestens:

```text
Sphere
CustomPrimitive
ValidIntersection
RejectedIntersection
InvalidAABB
```

SOLLTEN getestet werden.

---

## 301. Testing — Ray Types

Mindestens:

```text
Primary
Shadow
Reflection
Refraction
Custom
```

SOLLTEN getestet werden.

---

## 302. Testing — Recursion

Mindestens:

```text
Depth0
Depth1
MaximumDepth
ExceededDepth
```

MÜSSEN getestet werden.

---

## 303. Testing — Ray Query

Mindestens:

```text
ClosestHit
AnyHit
Occlusion
Miss
BatchQuery
AsyncQuery
```

MÜSSEN getestet werden.

---

## 304. Testing — Volume

Mindestens:

```text
DenseVolume
SparseVolume
EmptyVolume
AdaptiveSampling
EarlyTermination
```

SOLLTEN getestet werden.

---

## 305. Testing — Motion

Mindestens:

```text
StaticObject
MovingInstance
MovingRayTime
DeformingGeometry
```

SOLLTEN getestet werden.

---

## 306. Testing — CPU/GPU Equivalence

CPU- und GPU-Backends SOLLTEN mit identischen Szenen gegen definierte numerische Toleranzen verglichen werden.

---

## 307. Testing — Determinism

Deterministische Ausführung MUSS mit identischen Seeds und Szenenversionen getestet werden.

---

## 308. Testing — Memory Pressure

Mindestens:

```text
LowMemory
AccelerationBuildPressure
LargeGeometry
LargeVolume
GPUOutOfMemory
```

MÜSSEN getestet werden.

---

## 309. Testing — Device Loss

Device Loss MUSS kontrolliert behandelt werden.

---

## 310. Testing — Fallback

Ein Wechsel von Hardware-Ray-Tracing zu einem verfügbaren Software- beziehungsweise CPU-Backend SOLL getestet werden.

---

## 311. Testing — Security

Mindestens:

```text
MalformedGeometry
OutOfBoundsIndex
InvalidStride
OversizedPayload
InvalidShader
ExcessiveRecursion
OversizedDispatch
```

MÜSSEN getestet werden.

---

## 312. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
MillionsOfTriangles
MillionsOfInstances
DeepBVH
DynamicScene
ContinuousRefit
ContinuousRebuild
HighRayCount
DeepRayPaths
LargePayloads
LargeVolumes
OutOfCoreGeometry
RapidSceneChanges
ManyConcurrentQueries
ManyConcurrentDispatches
LongRunningPathTrace
GPU Memory Pressure
CPU Memory Pressure
BackendSwitch
```

---

## 313. Performance Tests

Mindestens folgende Kennzahlen SOLLTEN erhoben werden:

```text
BLASBuildTime
TLASBuildTime
RefitTime
RaysPerSecond
PrimaryRaysPerSecond
ShadowRaysPerSecond
TraversalTime
IntersectionTime
MemoryUsage
GPUUsage
CPUUsage
TimeToFirstSample
TimeToConvergence
```

---

## 314. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
NovaRayTracingRuntime

Ray
RayPayload
RayHitAttributes

RayTracingScene
RayTracingGeometry
RayTracingInstance

Triangle Geometry
AABB
Basic Procedural Geometry Architecture

BLAS
TLAS
Acceleration Structure Build

Ray Generation
Ray Dispatch

Traversal
Triangle Intersection

Closest Hit
Miss

Ray Query
ClosestHit Query
AnyHit Query
Occlusion Query

CPU Backend
GPU Backend Architecture
CPU Fallback

Scheduler Integration
Memory Model Integration
Object Model Integration

Capability Discovery
Diagnostics
Profiling
Cancellation
Security
```

---

## 315. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Hardware Ray Tracing
GPU Compute Traversal
SIMD CPU Traversal

Advanced BVH
Wide BVH
Compaction
Serialization

Any Hit Programs
Procedural Intersection Programs
Callable Programs

Wavefront Ray Tracing
Ray Queues
Ray Sorting

Reflection
Refraction
Shadows
Global Illumination
Path Tracing

Importance Sampling
Multiple Importance Sampling
Progressive Accumulation
Adaptive Sampling

Motion Blur
Deforming Geometry

Dense Volumes
Sparse Volumes
Bricked Volumes
Out-of-Core Volumes

Denoising
Ray Reconstruction

LiDAR Simulation
Depth Sensor Simulation
Camera Simulation
Acoustic Ray Tracing

Distributed Ray Tracing
Distributed Sampling
Scene Partitioning

Deterministic Reference Backend
```

---

## 316. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-RAYTRACING-0001` MUSS:

1. eine hardwareunabhängige Ray-Tracing-Abstraktion bereitstellen.
2. Rays mit Origin und Direction repräsentieren.
3. `TMin` und `TMax` unterstützen.
4. ungültige Rays erkennen.
5. Ray Payloads architektonisch unterstützen.
6. Hit Attributes unterstützen.
7. Ray-Tracing-Szenen repräsentieren.
8. Szenen eindeutig identifizieren.
9. Szenenversionen unterstützen.
10. Ray-Tracing-Geometrien repräsentieren.
11. Dreiecksgeometrie unterstützen.
12. AABBs unterstützen.
13. prozedurale Geometrie architektonisch unterstützen.
14. Geometrieinstanzen unterstützen.
15. Instanztransformationen unterstützen.
16. Instanzmasken unterstützen.
17. Geometrieinstancing ohne vollständige Datenkopie ermöglichen.
18. Acceleration Structures unterstützen.
19. BLAS unterstützen.
20. TLAS unterstützen.
21. Acceleration Structures erstellen können.
22. Build-Fehler diagnostizieren.
23. dynamische Acceleration Structures architektonisch unterstützen.
24. Refit architektonisch unterstützen.
25. Rebuild unterstützen.
26. Ray Generation unterstützen.
27. Ray Dispatch unterstützen.
28. 1D-Dispatch unterstützen.
29. 2D-Dispatch unterstützen.
30. Traversierung unterstützen.
31. Triangle-Ray-Intersection unterstützen.
32. den nächstgelegenen Treffer bestimmen können.
33. Miss-Zustände unterstützen.
34. Closest-Hit-Verarbeitung unterstützen.
35. Ray Queries unterstützen.
36. Closest-Hit Queries unterstützen.
37. Any-Hit beziehungsweise Occlusion Queries unterstützen.
38. Batch Queries architektonisch unterstützen.
39. asynchrone Ausführung unterstützen.
40. eine Ray-Tracing-Pipeline repräsentieren.
41. Pipelines validieren.
42. Backend Capabilities abfragen.
43. nicht unterstützte Fähigkeiten explizit melden.
44. CPU-Ausführung unterstützen.
45. GPU-Ausführung architektonisch unterstützen.
46. dedizierte Ray-Tracing-Hardware optional verwenden.
47. einen CPU-Fallback ermöglichen.
48. Scheduler Integration unterstützen.
49. Memory Model Integration unterstützen.
50. Object Model Integration unterstützen.
51. GPU Runtime Integration architektonisch unterstützen.
52. Ressourcenlebenszeiten verwalten.
53. Speicherbudgets berücksichtigen.
54. große Geometrien architektonisch unterstützen.
55. Cancellation unterstützen.
56. veraltete Jobs verwerfen können.
57. numerisch ungültige Geometrien erkennen.
58. NaN- und Infinity-Werte kontrolliert behandeln.
59. degenerierte Primitive kontrolliert behandeln.
60. Self-Intersection-Probleme numerisch robust behandeln.
61. unterschiedliche Ray Types unterstützen können.
62. Ray Masks unterstützen.
63. Ray Flags unterstützen.
64. rekursive beziehungsweise iterative Sekundärstrahlen architektonisch unterstützen.
65. maximale Ray Depth begrenzen können.
66. Volumen-Ray-Tracing architektonisch unterstützen.
67. Out-of-Core-Verarbeitung architektonisch ermöglichen.
68. deterministische Ausführung unterstützen können.
69. Random Seeds explizit setzen können.
70. Result Provenance unterstützen.
71. Scene Version im Result dokumentieren können.
72. Pipeline Version dokumentieren können.
73. Backend dokumentieren können.
74. strukturierte Diagnosen bereitstellen.
75. Profiling unterstützen.
76. Ray Count messen können.
77. Rays per Second messen können.
78. Acceleration-Structure-Build-Zeit messen können.
79. Speicherverbrauch messen können.
80. Buffer-Grenzen validieren.
81. Indexzugriffe validieren.
82. Dispatch Limits validieren.
83. Payload Limits validieren.
84. Ressourcenlimits unterstützen.
85. untrusted Geometrie validieren.
86. untrusted Programme isolieren beziehungsweise validieren.
87. Device Loss kontrolliert behandeln.
88. Backend Failure kontrolliert behandeln.
89. Out-of-Memory kontrolliert behandeln.
90. Erweiterungen ohne Änderung des grundlegenden Ray-Objektmodells ermöglichen.

---

## 317. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaRayTracingCompute
 |
 +-- RayTracingRuntime
 |
 +-- RayDeviceManager
 |
 +-- SceneManager
 |
 +-- GeometryManager
 |
 +-- InstanceManager
 |
 +-- AccelerationStructureManager
 |    |
 |    +-- BLASBuilder
 |    +-- TLASBuilder
 |    +-- BVHBuilder
 |    +-- RefitEngine
 |    +-- CompactionEngine
 |
 +-- RayGenerationEngine
 |
 +-- TraversalEngine
 |
 +-- IntersectionEngine
 |
 +-- RayQueryEngine
 |
 +-- PipelineManager
 |
 +-- BindingManager
 |
 +-- RayQueueManager
 |
 +-- VolumeTraversalEngine
 |
 +-- SamplingEngine
 |
 +-- AccumulationEngine
 |
 +-- DenoisingBridge
 |
 +-- StreamingManager
 |
 +-- BackendManager
 |    |
 |    +-- CPUBackend
 |    +-- GPUComputeBackend
 |    +-- HardwareRTBackend
 |    +-- ReferenceBackend
 |
 +-- SchedulerBridge
 |
 +-- MemoryManager
 |
 +-- Diagnostics
 |
 +-- Profiler
 |
 +-- SecurityValidator
```

Diese Struktur ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 318. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt die gemeinsame Ausführungsinfrastruktur bereit.

```text
Ray Tracing
     |
     v
Compute Runtime
     |
 +---+----+------+
 |        |      |
 v        v      v
CPU      GPU    NPU
```

---

## 319. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert insbesondere:

```text
Acceleration Builds
Acceleration Updates
Ray Dispatches
Ray Queries
Streaming
Denoising
Background Compilation
```

---

## 320. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model verwaltet insbesondere:

```text
Geometry Buffers
Instance Buffers
Acceleration Structures
Ray Buffers
Payload Buffers
Hit Buffers
Volume Buffers
Accumulation Buffers
Scratch Memory
```

---

## 321. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Ray-Tracing-Ressourcen SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 322. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

Die GPU Runtime stellt GPU-Compute- und Hardware-Ray-Tracing-Ressourcen bereit.

---

## 323. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

Die NPU Runtime kann ergänzende AI-basierte Operationen wie Denoising und Ray Reconstruction beschleunigen.

---

## 324. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

HPC Compute kann große Ray-Tracing-Workloads über mehrere CPUs, GPUs oder Compute Nodes verteilen.

---

## 325. Beziehung zu `NPSPEC-COMPUTE-MEDVIS-0001`

Medical Visualization Compute kann Ray Tracing insbesondere verwenden für:

```text
Volume Ray Casting
Volume Rendering
Surface Rendering
Visibility
Picking
Image-Guided Visualization
Scientific Visualization
```

---

## 326. Grundlegende Ray-Tracing-Architektur

```text
Scene
  |
  v
Geometry
  |
  v
BLAS
  |
  v
Instances
  |
  v
TLAS
  |
  v
Ray Generation
  |
  v
Traversal
  |
  v
Intersection
  |
  +--------+--------+
  |                 |
  v                 v
Hit                Miss
  |
  v
Result
```

---

## 327. Path-Tracing-Architektur

```text
Camera Ray
    |
    v
Intersection
    |
    v
Material
    |
    v
Sample BSDF
    |
    v
Secondary Ray
    |
    v
Intersection
    |
    v
...
    |
    v
Accumulation
```

---

## 328. Wavefront-Architektur

```text
Ray Generation
      |
      v
Primary Queue
      |
      v
Traversal
      |
      +----------------+
      |                |
      v                v
Hit Queue          Miss Queue
      |
      v
Shading
      |
 +----+------+------+
 |           |      |
 v           v      v
Shadow   Reflection Refraction
 Queue      Queue      Queue
```

---

## 329. Volume-Ray-Tracing-Architektur

```text
Ray
 |
 v
Volume Bounds
 |
 v
Entry / Exit
 |
 v
Volume Sampling
 |
 v
Transfer Function
 |
 v
Integration
 |
 v
Early Termination
 |
 v
Result
```

---

## 330. Hardware-Fallback-Architektur

```text
Ray Tracing Request
        |
        v
Capability Discovery
        |
   +----+----+
   |         |
   v         v
Hardware RT Available?
   |         |
  Yes        No
   |         |
   v         v
Hardware    GPU Compute
RT Backend      |
                v
          GPU Available?
                |
           +----+----+
           |         |
          Yes        No
           |         |
           v         v
       GPU BVH      CPU
       Traversal   Traversal
```

---

## 331. Distributed-Ray-Tracing-Architektur

```text
Scene / Workload
       |
       v
Work Partitioning
       |
 +-----+-----+-----+
 |           |     |
 v           v     v
Node A     Node B Node C
 |           |     |
 v           v     v
Partial    Partial Partial
Result     Result  Result
 |           |     |
 +-----+-----+-----+
       |
       v
Accumulation
       |
       v
Final Result
```

---

## 332. Ray-Query-Architektur

```text
Application
    |
    v
Ray Query
    |
    v
Acceleration Structure
    |
    v
Traversal
    |
    v
Intersection
    |
    v
Query Result
```

---

## 333. Datenmodellgrundsatz

Ray Tracing Compute MUSS zwischen:

```text
Scene
Geometry
Instance
Acceleration Structure
Ray
Payload
Pipeline
Dispatch
Hit
Result
```

unterscheiden.

---

## 334. Traversierungsgrundsatz

Die Acceleration Structure ist eine Optimierungsstruktur.

Sie DARF nicht als semantische Quelle der eigentlichen Szenengeometrie behandelt werden.

Daher gilt:

```text
Geometry = Source
Acceleration Structure = Derived Optimization
```

---

## 335. Backend-Grundsatz

Das Ergebnis einer Ray Query darf nicht davon abhängen, ob Traversierung über:

```text
CPU
GPU Compute
Dedicated RT Hardware
```

erfolgt, abgesehen von dokumentierten numerischen Toleranzen und explizit gewählten Backend-spezifischen Funktionen.

---

## 336. Ray-Query-Grundsatz

Ray Tracing Compute ist nicht auf Bilderzeugung beschränkt.

Daher gilt:

```text
Ray Tracing
!=
Only Rendering
```

Die Infrastruktur MUSS auch allgemeine räumliche Queries ermöglichen.

---

## 337. Präzisionsgrundsatz

Numerische Robustheit ist Bestandteil der Ray-Tracing-Korrektheit.

Insbesondere:

```text
Self Intersection
Floating Point Error
Degenerate Geometry
Large Coordinates
Parallel Rays
Near-Zero Directions
```

MÜSSEN als grundlegende Implementierungsprobleme behandelt werden.

---

## 338. Beschleunigungsgrundsatz

Eine schnellere Acceleration Structure darf die definierte Intersection-Semantik nicht verändern.

---

## 339. Progressive-Grundsatz

Ein progressives Ray-Tracing-Ergebnis MUSS von einem vollständig konvergierten beziehungsweise finalen Ergebnis unterscheidbar sein.

---

## 340. Rekonstruktionsgrundsatz

AI-basierte oder algorithmisch rekonstruierte Samples MÜSSEN von tatsächlich berechneten Ray Samples unterscheidbar bleiben.

---

## 341. Erweiterbarkeit

Neue:

```text
Geometry Types
Acceleration Structures
Traversal Algorithms
Ray Types
Intersection Programs
Material Models
Sampling Methods
Volume Representations
Sensor Models
Hardware Backends
Denoisers
Reconstruction Methods
Distributed Strategies
```

SOLLTEN ohne Änderung des grundlegenden Ray-Objektmodells integrierbar sein.

---

## 342. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-RAYTRACING-RAY-0001
Ray Model

NPSPEC-COMPUTE-RAYTRACING-SCENE-0001
Ray Tracing Scene Model

NPSPEC-COMPUTE-RAYTRACING-GEOMETRY-0001
Ray Tracing Geometry

NPSPEC-COMPUTE-RAYTRACING-BVH-0001
Bounding Volume Hierarchy

NPSPEC-COMPUTE-RAYTRACING-AS-0001
Acceleration Structures

NPSPEC-COMPUTE-RAYTRACING-TRAVERSAL-0001
Ray Traversal

NPSPEC-COMPUTE-RAYTRACING-INTERSECTION-0001
Ray Intersection

NPSPEC-COMPUTE-RAYTRACING-PIPELINE-0001
Ray Tracing Pipeline

NPSPEC-COMPUTE-RAYTRACING-QUERY-0001
Ray Queries

NPSPEC-COMPUTE-RAYTRACING-WAVEFRONT-0001
Wavefront Ray Tracing

NPSPEC-COMPUTE-RAYTRACING-PATHTRACING-0001
Path Tracing

NPSPEC-COMPUTE-RAYTRACING-SAMPLING-0001
Ray Sampling

NPSPEC-COMPUTE-RAYTRACING-VOLUME-0001
Volume Ray Tracing

NPSPEC-COMPUTE-RAYTRACING-MOTION-0001
Motion Ray Tracing

NPSPEC-COMPUTE-RAYTRACING-SENSOR-0001
Sensor Ray Tracing

NPSPEC-COMPUTE-RAYTRACING-LIDAR-0001
LiDAR Simulation

NPSPEC-COMPUTE-RAYTRACING-ACOUSTIC-0001
Acoustic Ray Tracing

NPSPEC-COMPUTE-RAYTRACING-DISTRIBUTED-0001
Distributed Ray Tracing

NPSPEC-COMPUTE-RAYTRACING-DENOISE-0001
Ray Tracing Denoising

NPSPEC-COMPUTE-RAYTRACING-BACKEND-0001
Ray Tracing Backend

NPSPEC-COMPUTE-RAYTRACING-PRECISION-0001
Ray Tracing Numerical Precision

NPSPEC-COMPUTE-RAYTRACING-PROFILING-0001
Ray Tracing Profiling

NPSPEC-COMPUTE-RAYTRACING-TEST-0001
Ray Tracing Conformance Tests
```

---

## 343. Zusammenfassung

`NPSPEC-COMPUTE-RAYTRACING-0001` definiert die gemeinsame Ray-Tracing-Compute-Infrastruktur innerhalb der NovaOS-Compute-Architektur.

Die grundlegende Verarbeitung lautet:

```text
Scene
  |
  v
Geometry
  |
  v
Acceleration Structure
  |
  v
Ray Generation
  |
  v
Traversal
  |
  v
Intersection
  |
  +------------------+
  |                  |
  v                  v
Hit                 Miss
  |
  v
Hit Processing
  |
  v
Secondary Rays / Result
```

Die Architektur unterstützt:

```text
Triangle Geometry
Procedural Geometry
Instancing
BLAS
TLAS
BVH
Ray Dispatch
Ray Queries
Closest Hit
Any Hit
Miss
Secondary Rays
Wavefront Ray Tracing
Path Tracing
Volume Ray Tracing
Motion
Sensor Simulation
LiDAR
Acoustic Ray Tracing
Distributed Ray Tracing
```

Die Ausführung kann über:

```text
CPU
SIMD CPU
GPU Compute
Dedicated Ray Tracing Hardware
NPU-assisted Compute
Distributed Compute
Software Reference Backend
```

erfolgen.

Die Architektur trennt konsequent:

```text
Source Geometry
Instances
Acceleration Structures
Rays
Traversal
Intersection
Hit Processing
Sampling
Reconstruction
Output
```

voneinander.

Die zentrale Architekturregel lautet:

> Die Szenengeometrie ist die semantische Quelle. Acceleration Structures sind abgeleitete Optimierungsstrukturen und dürfen die Bedeutung oder Intersection-Semantik der ursprünglichen Geometrie nicht verändern.

Die zentrale Backend-Regel lautet:

> Anwendungen beschreiben Ray-Tracing-Arbeit unabhängig von konkreter Hardware. NovaOS entscheidet anhand von Fähigkeiten, Präzisionsanforderungen, Speicherbudget, Performance-Zielen und verfügbaren Geräten, ob Traversierung über CPU, GPU Compute, dedizierte Ray-Tracing-Hardware oder zukünftige Compute-Beschleuniger ausgeführt wird.

Die zentrale Ray-Query-Regel lautet:

> Ray Tracing wird innerhalb von NovaOS als allgemeine räumliche Compute-Fähigkeit behandelt und ist nicht auf photorealistisches Rendering beschränkt.

Das grundlegende Architekturprinzip lautet:

> NovaOS Ray Tracing Compute abstrahiert Strahlen, Geometrien, Instanzen, Beschleunigungsstrukturen, Traversierung, Schnittpunkte, Pipelines, Ray Queries und Ergebnisse als eigenständige Compute-Objekte. Dadurch kann dieselbe Infrastruktur für Echtzeitgrafik, Path Tracing, wissenschaftliche und medizinische Visualisierung, Volumen-Ray-Casting, Sensor- und LiDAR-Simulation, akustische Strahlverfolgung, Sichtbarkeitsberechnungen und allgemeine geometrische Queries verwendet werden, während CPU-, GPU-, Hardware-Ray-Tracing- und zukünftige Beschleuniger-Backends austauschbar bleiben.