# ADR-DATAFLOW-0002: Semantische Typen ergänzen technische Datentypen

- **Status:** Akzeptiert
- **Datum:** 2026-07-27
- **Kategorie:** Data Flow
- **Betrifft:** Nova Runtime, Nova Pipelines, Capability Contracts, Compiler, Discovery Engine
- **Entscheider:** Nova Architecture Board

---

# Kontext

Klassische Softwaresysteme verwenden überwiegend technische Datentypen.

Beispiele:

```text
int
float
string
bool
byte[]
DateTime
```

Diese beschreiben ausschließlich die technische Repräsentation von Daten.

Sie beantworten jedoch nicht die eigentliche Bedeutung eines Wertes.

Beispiel:

```text
string
```

Kann bedeuten:

- Vorname
- Nachname
- Dateiname
- Dateipfad
- URI
- E-Mail-Adresse
- Sprache
- Land
- ISBN
- GPS-Koordinate

Obwohl technisch identisch, besitzen diese Werte völlig unterschiedliche fachliche Bedeutungen.

Dadurch entstehen Probleme bei:

- automatischer Pipeline-Erstellung
- Capability Discovery
- Datenvalidierung
- KI-Planung
- automatischer Konvertierung
- Wiederverwendung von Capabilities

Semantische Typen ergänzen primitive Datentypen um Bedeutung, Einschränkungen und Dokumentation, wodurch Systeme Daten intelligenter validieren und verarbeiten können. :contentReference[oaicite:0]{index=0}

---

# Problem

Ein technischer Datentyp allein reicht nicht aus, um automatisch zu entscheiden:

- welche Capability kompatibel ist,
- welche Konvertierung zulässig ist,
- welche Einheit verwendet wird,
- welche Validierung notwendig ist,
- welche Bedeutung ein Wert besitzt.

Beispiel:

```text
Input:
string
```

Die Runtime kann daraus nicht ableiten, ob es sich um

- einen Dateinamen,
- einen Benutzernamen,
- einen Dateipfad,
- einen URL,
- einen Prompt,
- einen Quellcode,
- ein JSON-Dokument

handelt.

Dadurch wäre eine semantische Discovery kaum möglich.

---

# Entscheidung

Nova führt ein zweistufiges Typsystem ein.

Jeder Datenwert besitzt:

```text
Technischer Typ
+
Semantischer Typ
```

Beispiel:

```text
String
+
EmailAddress
```

oder

```text
Float64
+
Temperature<Celsius>
```

oder

```text
ByteArray
+
PNG Image
```

Technischer Typ und semantischer Typ sind vollständig getrennt.

---

# Architektur

```text
Data Value
├── Technical Type
│      String
│      Float64
│      UInt32
│      ByteArray
└── Semantic Type
       Email
       Image
       PDF
       Audio
       Matrix
       Temperature
       GPSCoordinate
```

---

# Technische Typen

Technische Typen beschreiben ausschließlich:

- Speicherformat
- Binärdarstellung
- Größe
- Serialisierung
- Performance

Beispiele:

```text
Int32
UInt64
Float64
Boolean
String
Bytes
Array
Map
Object
```

---

# Semantische Typen

Semantische Typen beschreiben:

- fachliche Bedeutung
- erlaubte Werte
- Einheiten
- Domänenregeln
- Beziehungen
- Validierungsregeln

Beispiele:

```text
Email
IPAddress
URI
UUID
Money
Temperature
Vector3
Image
Audio
Video
Mesh
PointCloud
Tensor
Markdown
JSON
SourceCode
Prompt
RobotPose
ChemicalFormula
Measurement
```

---

# Beispiel

Nicht ausreichend:

```text
string
```

Stattdessen:

```text
String
Semantic:
Email
```

oder

```text
String
Semantic:
Markdown
```

oder

```text
String
Semantic:
SQLQuery
```

---

# Zusammenspiel

Die Runtime betrachtet immer beide Ebenen.

Beispiel:

```text
Float64
+
Temperature<Celsius>
```

Eine Capability, die

```text
Pressure
```

verarbeitet,

ist damit **nicht** kompatibel.

---

# Discovery

Discovery verwendet primär semantische Typen.

Beispiel:

Benutzer:

```text
Bilder verkleinern
```

Discovery sucht nach

```text
Image
```

nicht nach

```text
ByteArray
```

Dadurch steigt die Treffergenauigkeit erheblich.

---

# Compiler

Der Compiler prüft:

```text
Technischer Typ
+
Semantischer Typ
```

Folgende Fälle:

✔ gleicher technischer Typ

✔ kompatibler semantischer Typ

✔ erlaubte Konvertierung

✘ inkompatibler semantischer Typ

---

# Pipeline Planner

Pipelines verbinden ausschließlich kompatible semantische Typen.

Beispiel:

```text
PNG Image
↓
Resize
↓
PNG Image
↓
OCR
↓
PlainText
```

Nicht zulässig:

```text
PNG Image
↓
Capability erwartet Audio
```

---

# Konvertierungen

Nova unterscheidet:

```text
Technische Konvertierung
Semantische Konvertierung
```

Technisch:

```text
UInt8
↓
UInt16
```

Semantisch:

```text
JPEG
↓
PNG
```

oder

```text
Markdown
↓
HTML
```

---

# Mehrfachsemantik

Ein Wert darf mehrere semantische Eigenschaften besitzen.

Beispiel:

```text
String
Semantics:
Markdown
English
TechnicalDocumentation
```

oder

```text
Image
RGB
MedicalImage
DICOM
```

---

# Erweiterbarkeit

Neue semantische Typen können registriert werden.

Beispiele:

```text
Nova.Robotics.Pose
Nova.Physics.VectorField
Nova.Chemistry.Molecule
Nova.Astronomy.StarCatalog
```

Das Typsystem bleibt offen und versionsfähig.

---

# KI-Unterstützung

KI-Agenten können semantische Typen verwenden für:

- Capability Discovery
- automatische Pipeline-Erstellung
- Datenvalidierung
- Datenkonvertierung
- Intent Planning

Semantische Typen erleichtern die Interpretation fachlicher Bedeutung und verbessern die Interoperabilität zwischen Komponenten. :contentReference[oaicite:1]{index=1}

---

# Vorteile

- deutlich intelligentere Discovery
- automatische Datenvalidierung
- bessere Pipeline-Erstellung
- weniger Fehlverbindungen
- bessere Compilerdiagnosen
- KI versteht Datenbedeutung
- bessere Dokumentation
- höhere Wiederverwendbarkeit
- klare Trennung zwischen Repräsentation und Bedeutung

---

# Nachteile

- größeres Typmodell
- zusätzlicher Validierungsaufwand
- höherer Pflegeaufwand für semantische Typbibliotheken
- Compiler und Runtime werden komplexer

Diese Nachteile werden als akzeptabel bewertet.

---

# Konsequenzen

Nova behandelt Datentypen nicht ausschließlich als technische Speicherformate.

Jeder Datenwert besitzt zusätzlich eine explizite semantische Bedeutung.

Discovery, Compiler, Runtime, Capability Contracts und Pipeline Planner verwenden beide Ebenen gemeinsam, wodurch Capabilities nicht nur anhand ihrer Binärrepräsentation, sondern anhand ihrer tatsächlichen fachlichen Bedeutung verbunden werden können.

Dadurch entsteht eine wesentlich intelligentere, sicherere und KI-fähige Dataflow-Architektur, die automatische Planung, Validierung und Interoperabilität erheblich verbessert.