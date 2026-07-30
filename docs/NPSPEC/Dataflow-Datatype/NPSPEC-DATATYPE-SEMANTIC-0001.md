# NPSPEC-DATATYPE-SEMANTIC-0001: Semantische Datentypen

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:**
  - ADR-DATAFLOW-0002
  - ADR-DATAFLOW-0005
  - ADR-DATAFLOW-0006
  - NPSPEC-DATATYPE-0001
  - NPSPEC-DATATYPE-PRIMITIVE-0001
  - NPSPEC-DATAFLOW-0001
  - NPSPEC-CAPABILITY-CONTRACT-0001
  - NPSPEC-CAPABILITY-DISCOVERY-0001
  - NPSPEC-CAPABILITY-REGISTRY-0001
  - NPSPEC-UNITS-0001
  - NPSPEC-UNITS-0003

---

# 1. Zweck

Diese Spezifikation definiert das Modell semantischer Datentypen der Nova Platform.

Semantische Datentypen ergänzen technische Datentypen um ihre fachliche Bedeutung.

Während ein technischer Datentyp beschreibt, wie ein Wert gespeichert wird, beschreibt ein semantischer Datentyp, was dieser Wert fachlich darstellt.

Beispiel:

```text
Technischer Typ:

String

Semantischer Typ:

EmailAddress
```

Diese Trennung bildet die Grundlage für:

- semantische Capability Discovery
- automatische Pipelineplanung
- sichere Capability-Komposition
- fachliche Datenvalidierung
- nachvollziehbare Konvertierungen
- intelligente Benutzerinteraktionen
- KI-gestützte Verarbeitung

---

# 2. Ziele

Das semantische Typsystem verfolgt folgende Ziele:

- eindeutige fachliche Bedeutung von Daten
- Vermeidung fachlich ungültiger Datenflüsse
- automatische Erkennung kompatibler Capabilities
- maschinenlesbare Domänenmodelle
- sichere Konvertierung zwischen Bedeutungen
- bessere Compilerdiagnosen
- domänenübergreifende Interoperabilität
- langfristige Erweiterbarkeit

---

# 3. Grundprinzipien

Semantische Datentypen basieren auf folgenden Prinzipien:

- Semantik ergänzt technische Repräsentation.
- Technischer Typ und semantischer Typ bleiben getrennt.
- Jeder semantische Typ besitzt eine stabile Identität.
- Semantische Typen sind versionierbar.
- Semantische Typen können hierarchisch organisiert werden.
- Semantische Kompatibilität wird explizit beschrieben.
- Konvertierungen erfolgen ausschließlich über Konverter-Capabilities.
- Semantik darf nicht allein aus Dateninhalten erraten werden.
- Mehrere semantische Eigenschaften können kombiniert werden.
- Die Runtime validiert semantische Datenflüsse.

---

# 4. Architekturübersicht

```text
Nova Data Value
├── Technical Type
│   └── String
├── Primary Semantic Type
│   └── EmailAddress
├── Semantic Traits
│   ├── PersonalData
│   └── ContactInformation
├── Constraints
├── Metadata
├── Unit
└── Version
```

Der technische Typ definiert die Repräsentation.

Der semantische Typ definiert die fachliche Bedeutung.

Semantic Traits ergänzen weitere Eigenschaften.

---

# 5. Definition eines semantischen Typs

Ein semantischer Datentyp besteht mindestens aus:

```text
Type ID

Name

Namespace

Version

Technical Base Type

Description

Constraints

Compatibility Rules
```

Optional kann er enthalten:

```text
Parent Type

Semantic Traits

Units

Validation Rules

Serialization Hints

Privacy Classification

Security Classification
```

---

# 6. Typidentität

Jeder semantische Typ besitzt eine global eindeutige Typidentität.

Beispiel:

```text
nova.core.contact.email-address
```

Weitere Beispiele:

```text
nova.media.image

nova.document.markdown

nova.compute.tensor

nova.robotics.pose

nova.physics.temperature

nova.geo.coordinate
```

Die Typidentität darf nach Veröffentlichung nicht verändert werden.

---

# 7. Namensräume

Semantische Typen werden in Namensräumen organisiert.

Beispiele:

```text
nova.core

nova.document

nova.media

nova.compute

nova.physics

nova.chemistry

nova.robotics

nova.geo

nova.medical
```

Hersteller und Drittanbieter verwenden eigene Namensräume.

Beispiel:

```text
org.example.industry.machine-state
```

Namensräume verhindern Namenskollisionen.

---

# 8. Technischer Basistyp

Jeder semantische Typ besitzt mindestens einen technischen Basistyp.

Beispiele:

```text
EmailAddress
↓
String
```

```text
Temperature
↓
Float64
```

```text
PNGImage
↓
Bytes
```

```text
RobotPose
↓
Record
```

Ein semantischer Typ darf nur auf technisch kompatiblen Basistypen verwendet werden.

---

# 9. Primärer semantischer Typ

Jeder Datenwert darf genau einen primären semantischen Typ besitzen.

Beispiel:

```text
Technical Type:

String

Primary Semantic Type:

MarkdownDocument
```

Der primäre Typ definiert die zentrale fachliche Bedeutung des Wertes.

Zusätzliche Eigenschaften werden über Semantic Traits beschrieben.

---

# 10. Semantic Traits

Semantic Traits beschreiben zusätzliche fachliche Eigenschaften eines Wertes.

Beispiel:

```text
Primary Type:

Image

Traits:

PNG

RGB

MedicalImage

PersonalData
```

Weitere Beispiele:

```text
Encrypted

Compressed

Signed

Localized

Confidential

Georeferenced

TimeSeries

Lossless
```

Traits ersetzen keine primären semantischen Typen.

---

# 11. Typhierarchie

Semantische Typen können hierarchisch organisiert werden.

Beispiel:

```text
Document

├── TextDocument
│   ├── PlainText
│   ├── Markdown
│   └── HTML
├── PortableDocument
│   └── PDF
└── Spreadsheet
```

Ein spezialisierter Typ übernimmt die allgemeinen Eigenschaften seines übergeordneten Typs.

---

# 12. Vererbung

Semantische Vererbung beschreibt eine fachliche Ist-ein-Beziehung.

Beispiel:

```text
PNGImage
ist ein
RasterImage
```

```text
RasterImage
ist ein
Image
```

Eine Capability mit dem Input-Typ `Image` darf daher grundsätzlich ein `PNGImage` akzeptieren.

Die tatsächliche Kompatibilität kann zusätzlich durch Constraints eingeschränkt werden.

---

# 13. Kompatibilitätsmodell

Nova unterscheidet folgende Kompatibilitätsstufen:

```text
Exact

Subtype

Trait Compatible

Convertible

Incompatible

Unknown
```

Bedeutung:

```text
Exact

Identischer semantischer Typ
```

```text
Subtype

Der gelieferte Typ ist spezialisierter
```

```text
Trait Compatible

Benötigte Traits sind vorhanden
```

```text
Convertible

Ein expliziter Konvertierungspfad existiert
```

```text
Incompatible

Die Typen dürfen nicht verbunden werden
```

```text
Unknown

Kompatibilität ist nicht ausreichend beschrieben
```

---

# 14. Exakte Kompatibilität

Exakte Kompatibilität besteht, wenn:

```text
Input Type ID

=

Output Type ID
```

und die Versionen kompatibel sind.

Beispiel:

```text
nova.document.markdown@1
↓
nova.document.markdown@1
```

Hierfür ist keine Konvertierung erforderlich.

---

# 15. Subtypkompatibilität

Ein Subtyp darf dort verwendet werden, wo ein allgemeinerer Typ erwartet wird.

Beispiel:

```text
Input erwartet:

Image

Geliefert:

PNGImage
```

Die umgekehrte Richtung ist nicht automatisch zulässig.

Eine Capability, die ausdrücklich `PNGImage` erwartet, darf kein beliebiges `Image` erhalten.

---

# 16. Trait-Kompatibilität

Capability Contracts können benötigte Traits deklarieren.

Beispiel:

```text
Input Type:

Image

Required Traits:

RGB

Uncompressed
```

Ein Datenobjekt ist nur kompatibel, wenn alle verpflichtenden Traits vorhanden sind.

Optionale Traits dürfen für Optimierungen verwendet werden.

---

# 17. Semantische Constraints

Semantische Typen können fachliche Einschränkungen besitzen.

Beispiele:

```text
EmailAddress:

gültiges Adressformat
```

```text
Temperature:

nicht unter absolutem Nullpunkt
```

```text
Image:

Breite größer als 0

Höhe größer als 0
```

```text
Percentage:

Wert zwischen 0 und 100
```

Constraints werden unabhängig von der technischen Repräsentation geprüft.

---

# 18. Wertebereiche

Semantische Wertebereiche können technischer oder fachlicher Natur sein.

Beispiel:

```text
Technical Type:

Float64

Semantic Type:

Probability

Allowed Range:

0.0 bis 1.0
```

Ein technisch gültiger `Float64` kann semantisch ungültig sein.

Beispiel:

```text
1.5
```

ist als `Float64` gültig, aber als `Probability` ungültig.

---

# 19. Einheiten

Physikalische semantische Typen können Einheiten besitzen.

Beispiel:

```text
Technical Type:

Float64

Semantic Type:

Temperature

Unit:

Celsius
```

Weitere Beispiele:

```text
Length<Meter>

Mass<Kilogram>

Velocity<MeterPerSecond>

Pressure<Pascal>
```

Einheiten sind keine frei interpretierbaren Strings, sondern registrierte Typreferenzen.

---

# 20. Dimensionen

Physikalische Typen besitzen zusätzlich eine Dimension.

Beispiel:

```text
Temperature

Dimension:

Θ
```

```text
Velocity

Dimension:

L · T⁻¹
```

Capabilities dürfen ausschließlich dimensionskompatible Werte verbinden.

Einheitenkonvertierungen erfolgen über explizite Konverter-Capabilities.

---

# 21. Dokumenttypen

Nova definiert semantische Dokumenttypen.

Beispiele:

```text
PlainText

MarkdownDocument

HTMLDocument

PDFDocument

SpreadsheetDocument

PresentationDocument

SourceCodeDocument

ConfigurationDocument
```

Der technische Basistyp kann abhängig vom Dokumentformat `String`, `Bytes` oder `Record` sein.

---

# 22. Medientypen

Nova definiert semantische Medientypen.

Beispiele:

```text
Image

RasterImage

VectorImage

Audio

Video

Animation

Subtitle

Texture

Font
```

Spezialisierungen können Formate und Eigenschaften beschreiben.

Beispiel:

```text
PNGImage

JPEGImage

SVGImage

WAVAudio

FLACAudio
```

---

# 23. Compute-Typen

Für mathematische und wissenschaftliche Daten existieren unter anderem:

```text
Vector

Matrix

Tensor

ComplexNumber

Quaternion

Polynomial

Graph

DataFrame

TimeSeries
```

Diese Typen ergänzen technische Speicherstrukturen um mathematische Bedeutung.

---

# 24. KI-Typen

Nova definiert semantische KI-Datentypen.

Beispiele:

```text
Prompt

Token

TokenSequence

Embedding

FeatureVector

Model

Dataset

TrainingSample

InferenceRequest

InferenceResult
```

KI-Typen dürfen zusätzliche Informationen enthalten:

```text
Model Family

Tokenizer

Vector Dimension

Modality

Precision

Context Length
```

---

# 25. Robotiktypen

Nova definiert semantische Robotiktypen.

Beispiele:

```text
RobotPose

JointState

Trajectory

NavigationGoal

OccupancyGrid

SensorFrame

PointCloud

Transform

ControlCommand
```

Robotiktypen können Koordinatensysteme, Zeitstempel und Referenzrahmen voraussetzen.

---

# 26. Geodatentypen

Nova definiert semantische Geodatentypen.

Beispiele:

```text
GeoCoordinate

GeoPoint

GeoLine

GeoPolygon

GeoRaster

GeoFeature

CoordinateReferenceSystem
```

Jeder räumliche Wert muss sein verwendetes Koordinatenreferenzsystem eindeutig angeben.

---

# 27. Identitäts- und Referenztypen

Semantische Identitätstypen ergänzen primitive Identifier.

Beispiele:

```text
UserID

DeviceID

ObjectID

CapabilityID

DocumentID

SessionID

TransactionID
```

Obwohl sie technisch auf demselben Basistyp beruhen können, sind sie fachlich nicht austauschbar.

Beispiel:

```text
UserID

≠

DeviceID
```

---

# 28. Sicherheitssemantik

Semantische Typen können sicherheitsrelevante Klassifizierungen besitzen.

Beispiele:

```text
PublicData

InternalData

ConfidentialData

SecretData

Credential

CryptographicKey

AuthenticationToken
```

Diese Klassifizierungen beeinflussen:

- Zugriffsrechte
- Logging
- Persistenz
- Übertragung
- Verschlüsselung
- Löschung

---

# 29. Datenschutzsemantik

Datenschutzrelevante Eigenschaften werden als standardisierte Traits modelliert.

Beispiele:

```text
PersonalData

SensitivePersonalData

PseudonymizedData

AnonymizedData

BiometricData

LocationData
```

Die Kennzeichnung beeinflusst:

- Datenminimierung
- Aufbewahrungsfristen
- Zugriffskontrolle
- Provenance
- Telemetrie
- Export

---

# 30. Semantische Rollen

Ein Datenwert kann innerhalb eines Contracts eine semantische Rolle besitzen.

Beispiele:

```text
Source

Destination

Query

Result

Configuration

Credential

Identifier

Timestamp

Measurement
```

Rollen beschreiben den Verwendungskontext, ersetzen jedoch keinen semantischen Typ.

---

# 31. Typregistrierung

Alle semantischen Typen werden in einer Type Registry registriert.

Die Registry speichert:

```text
Type ID

Namespace

Version

Technical Base Type

Parent Types

Traits

Constraints

Documentation

Compatibility
```

Nicht registrierte Typen dürfen nur innerhalb eines privaten Ausführungskontexts verwendet werden.

---

# 32. Type Discovery

Die Registry unterstützt semantische Suche.

Beispiele:

```text
Finde alle Bildtypen.
```

```text
Finde alle Typen für Positionsdaten.
```

```text
Finde einen Typ für Temperaturmessungen.
```

Discovery verwendet:

- Namen
- Beschreibungen
- Hierarchien
- Traits
- Kategorien
- Beziehungen

---

# 33. Capability Discovery

Capability Discovery verwendet semantische Typen als primäres Auswahlkriterium.

Beispiel:

```text
Vorhandenes Objekt:

MarkdownDocument
```

Gesucht werden Capabilities mit Eingaben wie:

```text
MarkdownDocument

TextDocument

Document
```

Capabilities, die `Audio` erwarten, werden ausgeschlossen, selbst wenn beide technisch als `Bytes` gespeichert sind.

---

# 34. Pipelinevalidierung

Jede Verbindung in einem Capability Graph wird semantisch validiert.

Prüfreihenfolge:

```text
1. Technische Kompatibilität

2. Semantische Kompatibilität

3. Trait-Kompatibilität

4. Constraint-Kompatibilität

5. Einheitenkompatibilität

6. Sicherheitskompatibilität

7. Versionskompatibilität
```

Eine Verbindung ist nur gültig, wenn alle verpflichtenden Prüfungen erfolgreich sind.

---

# 35. Semantische Konvertierungen

Semantische Typen dürfen nicht implizit geändert werden.

Beispiel:

```text
MarkdownDocument
↓
HTMLDocument
```

erfordert eine explizite Konverter-Capability.

Weitere Beispiele:

```text
Celsius
↓
Fahrenheit
```

```text
SpeechAudio
↓
PlainText
```

```text
GeoCoordinate<WGS84>
↓
GeoCoordinate<ETRS89>
```

---

# 36. Konvertierungspfade

Die Runtime kann mehrstufige Konvertierungspfade planen.

Beispiel:

```text
DOCXDocument
↓
HTMLDocument
↓
MarkdownDocument
↓
PlainText
```

Jeder Schritt muss:

- registriert
- versioniert
- beobachtbar
- testbar
- sicherheitskonform

sein.

---

# 37. Verlustbehaftete Konvertierungen

Konverter müssen deklarieren, ob eine Konvertierung verlustbehaftet ist.

Kategorien:

```text
Lossless

Potentially Lossy

Lossy

Irreversible
```

Beispiele:

```text
PNG → JPEG

Lossy
```

```text
Markdown → PlainText

Potentially Lossy
```

```text
Celsius → Kelvin

Lossless
```

Die Runtime darf verlustbehaftete Konvertierungen nicht ohne passende Richtlinie automatisch auswählen.

---

# 38. Semantische Mehrdeutigkeit

Ist die Bedeutung eines Datenwerts nicht eindeutig bekannt, darf die Runtime keinen spezifischen semantischen Typ vortäuschen.

Stattdessen wird verwendet:

```text
UnknownSemanticType
```

oder ein ausreichend allgemeiner Typ.

Beispiel:

```text
Technical Type:

Bytes

Semantic Type:

BinaryData
```

Eine Spezialisierung auf `PNGImage` erfolgt erst nach zuverlässiger Validierung.

---

# 39. Semantische Typinferenz

Semantische Typen können aus vertrauenswürdigen Quellen abgeleitet werden.

Beispiele:

- Capability Contract
- Dateiformat-Parser
- signierte Metadaten
- registrierte Datenquelle
- validierter Konverter

KI-basierte Typinferenz darf lediglich einen Vorschlag erzeugen.

Sie darf ohne Validierung keinen verbindlichen Typ zuweisen.

---

# 40. Versionierung

Semantische Typen besitzen semantische Versionen.

Beispiel:

```text
nova.media.image@1.2
```

Versionsänderungen werden klassifiziert als:

```text
Patch

Minor

Major
```

Patch:

- redaktionelle Änderungen
- zusätzliche Dokumentation
- unveränderte Semantik

Minor:

- kompatible Traits
- optionale Metadaten
- kompatible Constraints

Major:

- geänderte Bedeutung
- inkompatible Constraints
- geänderter Basistyp
- geänderte Hierarchie

---

# 41. Typaliasse

Ein Typ darf alternative Namen besitzen.

Beispiel:

```text
Primary Name:

EmailAddress

Aliases:

Email

ElectronicMailAddress
```

Aliasse dienen ausschließlich:

- Suche
- Dokumentation
- Migration

Die stabile Type ID bleibt unverändert.

---

# 42. Deprecation

Veraltete semantische Typen werden als `deprecated` markiert.

Ein Deprecation-Eintrag enthält:

```text
Reason

Deprecated Since

Replacement Type

Migration Path

Removal Policy
```

Veröffentlichte Typen dürfen nicht kommentarlos entfernt werden.

---

# 43. Reflection

Semantische Typen unterstützen vollständige Reflection.

Abfragbar sind:

- Type ID
- Name
- Namespace
- Version
- Beschreibung
- technischer Basistyp
- übergeordnete Typen
- Traits
- Constraints
- Einheiten
- Sicherheitsklassifikation
- Konvertierungsmöglichkeiten

---

# 44. Serialisierung

Semantische Typinformationen können gemeinsam mit dem Wert serialisiert werden.

Beispiel:

```yaml
value: 21.5
type:
  technical: Float64
  semantic: nova.physics.temperature
  unit: Celsius
  version: 1.0
```

Bei kompakten Formaten darf die Type ID durch einen registrierten numerischen Identifier ersetzt werden.

---

# 45. Provenance

Die Provenance eines Datenobjekts speichert die semantischen Typinformationen jeder Objektversion.

Bei einer Konvertierung werden dokumentiert:

```text
Source Semantic Type

Target Semantic Type

Converter Capability

Converter Version

Parameters

Loss Classification
```

Dadurch bleibt jede Bedeutungsänderung nachvollziehbar.

---

# 46. Fehlerbehandlung

Semantische Typfehler verwenden standardisierte Fehlercodes.

Beispiele:

```text
SEMANTIC_TYPE_UNKNOWN

SEMANTIC_TYPE_MISMATCH

SEMANTIC_TRAIT_MISSING

SEMANTIC_CONSTRAINT_VIOLATION

SEMANTIC_VERSION_INCOMPATIBLE

SEMANTIC_CONVERSION_REQUIRED

SEMANTIC_CONVERSION_NOT_FOUND

SEMANTIC_CONVERSION_LOSSY
```

Fehler müssen den erwarteten und den tatsächlich gelieferten Typ nennen.

---

# 47. YAML-Beispiel einer Typdefinition

```yaml
semanticType:
  id: nova.core.contact.email-address
  name: EmailAddress
  namespace: nova.core.contact
  version: 1.0.0
  technicalBaseType: String
  parent:
    - nova.core.contact.contact-information
  traits:
    - nova.core.trait.personal-data
  constraints:
    minimumLength: 3
    maximumLength: 320
    validator: nova.core.validator.email-address
  compatibility:
    acceptsSubtypes: true
    implicitConversion: false
  privacy:
    classification: personal-data
```

---

# 48. YAML-Beispiel eines Datenwerts

```yaml
dataObject:
  objectId: 01JZ8W6M6JY5R3TK4C8D2F1A9B
  value: user@example.org
  type:
    technical: String
    semantic: nova.core.contact.email-address
    version: 1.0.0
  traits:
    - nova.core.trait.personal-data
  validation:
    status: valid
```

---

# 49. Validierungsregeln

Ein semantischer Datentyp gilt als gültig, wenn:

- eine global eindeutige Type ID existiert,
- ein Namensraum definiert ist,
- eine Version angegeben ist,
- ein kompatibler technischer Basistyp existiert,
- alle Eltern-Typen registriert sind,
- alle Traits registriert sind,
- Constraints widerspruchsfrei sind,
- Kompatibilitätsregeln eindeutig sind,
- die Typdefinition maschinenlesbar ist.

---

# 50. Laufzeitregeln

Die Nova Runtime muss:

- semantische Typen an Data Objects erhalten,
- Typkompatibilität vor der Ausführung prüfen,
- Constraints validieren,
- ungültige Datenflüsse verhindern,
- Konverter explizit in den Graphen einfügen,
- Sicherheits- und Datenschutz-Traits berücksichtigen,
- Typinformationen in Provenance übernehmen,
- Typfehler standardisiert melden.

---

# 51. Compileranforderungen

Der Nova-Compiler muss:

- semantische Typinformationen verarbeiten,
- inkompatible Zuweisungen erkennen,
- Capability Contracts semantisch prüfen,
- verfügbare Konverter vorschlagen,
- unsichere Konvertierungen markieren,
- Einheiten- und Dimensionsfehler erkennen,
- aussagekräftige Diagnosen erzeugen.

Beispiel:

```text
Fehler:

nova.physics.temperature kann nicht an
nova.physics.pressure übergeben werden.

Technischer Typ beider Werte:

Float64

Semantische Typen sind jedoch inkompatibel.
```

---

# 52. Discovery-Anforderungen

Discovery-Systeme müssen:

- Typidentitäten berücksichtigen,
- Hierarchien auflösen,
- Traits durchsuchen,
- Constraints vergleichen,
- Konvertierungspfade finden,
- veraltete Typen kennzeichnen,
- Versionskompatibilität berücksichtigen.

Die technische Speicherrepräsentation darf nicht das primäre Discovery-Kriterium sein.

---

# 53. Sicherheitsanforderungen

Semantische Typinformationen müssen gegen Manipulation geschützt werden.

Die Runtime muss verhindern, dass eine Capability:

- Sicherheitsklassifikationen entfernt,
- personenbezogene Daten als öffentlich markiert,
- einen inkompatiblen Typ vortäuscht,
- Provenance-Typinformationen verändert,
- Constraints ohne neue Objektversion überschreibt.

Bei Vertrauensgrenzen müssen Typinformationen validiert oder kryptographisch abgesichert werden.

---

# 54. Datenschutzanforderungen

Semantische Metadaten dürfen keine unnötigen personenbezogenen Inhalte enthalten.

Typdefinitionen beschreiben ausschließlich die Datenklasse und nicht den konkreten Inhalt.

Beispiel:

```text
Erlaubt:

PersonalData
```

```text
Nicht als Typmetadatum erlaubt:

Person besitzt Krankheit X
```

Konkrete Inhalte verbleiben im geschützten Datenobjekt.

---

# 55. Testanforderungen

Jeder semantische Typ benötigt Tests für:

- Type-ID-Eindeutigkeit
- Basistypkompatibilität
- Constraint-Validierung
- Hierarchieauflösung
- Trait-Vererbung
- Versionskompatibilität
- Serialisierung
- Fehlerfälle
- Konverterintegration

Konformitätstests müssen unabhängig von einer konkreten Programmiersprache ausführbar sein.

---

# 56. Qualitätsanforderungen

Das semantische Typsystem muss:

- eindeutig
- deterministisch
- maschinenlesbar
- versionierbar
- erweiterbar
- dokumentierbar
- performant
- plattformunabhängig
- sicher
- interoperabel

sein.

Die Einführung neuer Typen darf bestehende Typidentitäten und Datenflüsse nicht unkontrolliert verändern.

---

# 57. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation, wenn:

- technische und semantische Typen getrennt verwaltet werden,
- jeder semantische Typ eine stabile Type ID besitzt,
- Typen hierarchisch organisiert werden können,
- Semantic Traits unterstützt werden,
- Constraints automatisch validiert werden,
- Typkompatibilität maschinenlesbar geprüft wird,
- implizite semantische Konvertierungen verhindert werden,
- Konverter-Capabilities unterstützt werden,
- Type Registry und Reflection verfügbar sind,
- semantische Typinformationen in Dataflow und Provenance erhalten bleiben.

---

# 58. Zusammenfassung

Die **semantischen Datentypen** ergänzen die technischen Datentypen der Nova Platform um eine explizite fachliche Bedeutung.

Ein `String` ist dadurch nicht länger lediglich eine Zeichenfolge, sondern kann eindeutig als `EmailAddress`, `MarkdownDocument`, `FilePath`, `Prompt` oder ein anderer fachlicher Wert beschrieben werden.

Durch stabile Typidentitäten, Namensräume, Hierarchien, Semantic Traits, Constraints, Einheiten, Versionierung und registrierte Konverter entsteht ein maschinenlesbares Bedeutungsmodell für sämtliche Nova-Daten.

Dieses Modell ermöglicht sichere Capability-Komposition, intelligente Discovery, automatische Pipelineplanung, präzise Compilerdiagnosen und nachvollziehbare Datenverarbeitung.

Semantische Datentypen bilden damit eine zentrale Grundlage für die universelle, KI-fähige und domänenübergreifende Architektur der Nova Platform.