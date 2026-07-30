# NPSPEC-DATATYPE-0001: Universelles Nova-Datentypmodell

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-27
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:**
  - NPSPEC-DATAFLOW-0001
  - ADR-DATAFLOW-0001 bis ADR-DATAFLOW-0007
  - ADR-CAPABILITY-0005
  - NPSPEC-CAPABILITY-CONTRACT-0001
  - NPSPEC-COMPUTE-0001
  - NPSPEC-UNITS-0001
  - NPSPEC-UNITS-0003
  - NPSPEC-UNITS-0004

---

# 1. Zweck

Diese Spezifikation definiert das universelle Nova-Datentypmodell.

Es bildet die Grundlage für sämtliche Daten innerhalb der Nova Platform und vereinheitlicht:

- primitive Datentypen
- komplexe Datentypen
- semantische Typen
- physikalische Größen
- KI-Daten
- wissenschaftliche Daten
- Multimedia
- Dokumente
- Geodaten
- Robotikdaten

Jeder Datenwert besitzt eine eindeutig definierte technische und fachliche Bedeutung.

---

# 2. Ziele

Das Datentypmodell verfolgt folgende Ziele:

- universelle Wiederverwendbarkeit
- starke Typisierung
- semantische Interoperabilität
- automatische Validierung
- Compiler-Unterstützung
- automatische Konvertierung über Capabilities
- plattformunabhängige Serialisierung
- langfristige Erweiterbarkeit

---

# 3. Grundprinzipien

Das Nova-Datentypmodell basiert auf folgenden Prinzipien:

- jeder Wert besitzt einen Typ
- technische und semantische Typen sind getrennt
- Typen sind unveränderlich
- Typen sind versionierbar
- Typen besitzen Metadaten
- Typen sind serialisierbar
- Typen unterstützen Reflection
- Typen sind maschinenlesbar

---

# 4. Architekturübersicht

```text
Data Type
├── Technical Type
├── Semantic Type
├── Metadata
├── Constraints
├── Units
├── Serialization
└── Validation
```

Jeder Nova-Datentyp besteht aus diesen Bausteinen.

---

# 5. Technische Datentypen

Technische Datentypen definieren ausschließlich die Speicherrepräsentation.

Beispiele:

```text
Boolean
Int8
UInt8
Int16
UInt16
Int32
UInt32
Int64
UInt64
Float16
Float32
Float64
Float128
Decimal
String
Bytes
```

---

# 6. Zusammengesetzte Datentypen

Nova unterstützt strukturierte Typen.

```text
Array
List
Set
Map
Dictionary
Tuple
Record
Object
Variant
Union
```

Diese Typen sind generisch.

---

# 7. Semantische Datentypen

Semantische Typen beschreiben die fachliche Bedeutung.

Beispiele:

```text
Email
IPAddress
URI
UUID
Markdown
HTML
PDF
Image
Audio
Video
Tensor
PointCloud
RobotPose
ChemicalFormula
GPSCoordinate
```

Technische und semantische Typen werden gemeinsam ausgewertet. :contentReference[oaicite:0]{index=0}

---

# 8. Physikalische Datentypen

Das Datentypmodell integriert das Nova-Einheitensystem.

Beispiele:

```text
Length
Mass
Temperature
Pressure
Velocity
Acceleration
Current
Voltage
Force
Energy
Power
```

Jede Größe besitzt eine definierte Einheit.

---

# 9. Numerische Datentypen

Nova unterscheidet:

```text
Ganzzahlen
Gleitkommazahlen
Festkommazahlen
Beliebige Genauigkeit
Intervalle
Komplexe Zahlen
Quaternionen
```

Alle numerischen Typen unterstützen definierte Präzisionsmodelle.

---

# 10. Zeittypen

Zeit wird als eigener Typbereich behandelt.

Unterstützt werden:

```text
Instant
Duration
TimeSpan
Date
Time
DateTime
Timestamp
Interval
Schedule
```

Zeittypen unterstützen Zeitzonen und hochauflösende Zeitstempel.

---

# 11. Texttypen

Nova unterscheidet verschiedene Textarten.

Beispiele:

```text
PlainText
Markdown
HTML
XML
JSON
YAML
CSV
SourceCode
Prompt
```

Der technische Typ bleibt dabei in der Regel `String`.

---

# 12. Binärdaten

Binärdaten werden unabhängig von ihrer Bedeutung beschrieben.

Beispiele:

```text
ByteArray
Blob
Stream
MemoryBuffer
BinaryFile
```

Die semantische Bedeutung ergänzt den technischen Typ.

---

# 13. Dokumenttypen

Dokumente besitzen spezialisierte semantische Typen.

```text
PDF
Word
Spreadsheet
Presentation
Notebook
Archive
CAD
```

Dokumente bleiben normale Datenobjekte.

---

# 14. Multimedia

Unterstützt werden:

```text
Image
Audio
Video
Animation
Subtitle
Texture
Material
Font
```

Spezialisierungen erfolgen über semantische Typen.

---

# 15. Geometriedaten

Nova definiert standardisierte Geometrietypen.

```text
Point
Line
Polygon
Mesh
Voxel
Spline
Bezier
Surface
```

Diese bilden die Grundlage für Grafik und CAD.

---

# 16. Robotiktypen

Robotik verwendet spezialisierte Datentypen.

Beispiele:

```text
RobotPose
Trajectory
JointState
SensorFrame
OccupancyGrid
Map
NavigationGoal
```

Diese Typen sind Bestandteil der Plattform.

---

# 17. KI-Datentypen

Nova integriert KI-Datentypen als Basistypen.

```text
Tensor
Embedding
Model
Dataset
Prompt
Token
FeatureVector
InferenceResult
```

Dadurch können KI-Capabilities standardisierte Schnittstellen verwenden.

---

# 18. Constraints

Jeder Datentyp kann Einschränkungen besitzen.

Beispiele:

```text
Min
Max
Pattern
Range
Precision
Scale
Allowed Values
```

Constraints werden automatisch validiert.

---

# 19. Metadaten

Jeder Typ besitzt Metadaten.

Beispiele:

```text
Name
Namespace
Beschreibung
Version
Autor
Kategorie
Tags
```

Metadaten unterstützen Discovery und Dokumentation.

---

# 20. Reflection

Alle Datentypen unterstützen Reflection.

Abfragbar sind:

- Name
- Eigenschaften
- Constraints
- Version
- Semantik
- Einheit
- Serialisierung

Reflection ermöglicht generische Werkzeuge und Compilerfunktionen.

---

# 21. Serialisierung

Alle Datentypen besitzen mindestens eine standardisierte Serialisierung.

Unterstützt werden:

```text
Binary
JSON
CBOR
MessagePack
Protocol Buffers
XML
```

Die Wahl des Formats ist unabhängig vom Datentyp.

---

# 22. Versionierung

Datentypen sind versionierbar.

Neue Versionen dürfen:

- Felder ergänzen
- Metadaten erweitern
- Constraints verschärfen

Breaking Changes erfordern eine neue Hauptversion.

---

# 23. Erweiterbarkeit

Das Datentypmodell ist offen.

Neue Typen können registriert werden.

Beispiele:

```text
MedicalImage
Genome
QuantumState
FluidSimulation
BuildingModel
DigitalTwin
```

Die Plattform muss ohne Kerneländerungen erweiterbar bleiben.

---

# 24. YAML-Beispiel

```yaml
type:
  technical: Float64
  semantic: Temperature
  unit: Celsius
  constraints:
    minimum: -273.15
    maximum: 10000
  serialization:
    - binary
    - json
  version: 1.0
```

---

# 25. Validierungsregeln

Ein Nova-Datentyp gilt als gültig, wenn:

- ein technischer Typ definiert ist,
- optional ein semantischer Typ angegeben wird,
- Constraints konsistent sind,
- eine eindeutige Version existiert,
- Reflection unterstützt wird,
- mindestens eine Serialisierung definiert ist.

---

# 26. Qualitätsanforderungen

Das Datentypmodell muss:

- vollständig typisiert sein,
- plattformunabhängig funktionieren,
- KI-geeignet sein,
- physikalische Größen unterstützen,
- semantisch erweiterbar bleiben,
- deterministisch validierbar sein,
- effizient serialisiert werden können,
- langfristig kompatibel bleiben.

Ein universelles Typmodell mit klarer Trennung zwischen technischer Repräsentation und semantischer Bedeutung verbessert Interoperabilität, Wiederverwendbarkeit und automatische Verarbeitung heterogener Daten. :contentReference[oaicite:1]{index=1}

---

# 27. Konformitätskriterien

Ein Datentyp erfüllt diese Spezifikation, wenn:

- technische Typinformationen vollständig definiert sind,
- semantische Informationen eindeutig beschrieben werden,
- alle Constraints maschinenlesbar vorliegen,
- Reflection verfügbar ist,
- Versionierung unterstützt wird,
- standardisierte Serialisierung vorhanden ist,
- der Typ mit der Nova Typed Dataflow Architecture kompatibel ist.

---

# 28. Zusammenfassung

Das **Universelle Nova-Datentypmodell** definiert eine einheitliche Grundlage für sämtliche Daten innerhalb der Nova Platform.

Durch die Kombination aus technischen Datentypen, semantischen Typen, physikalischen Einheiten, Constraints, Reflection, Versionierung und standardisierten Serialisierungsformaten entsteht ein vollständig typisiertes und erweiterbares Datenmodell.

Dieses Modell ermöglicht automatische Validierung, intelligente Discovery, sichere Capability-Komposition und reproduzierbare Datenverarbeitung und bildet damit die zentrale Datengrundlage für klassische Anwendungen, KI, Robotik, Wissenschaft und industrielle Systeme.