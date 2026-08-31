# ADR-ARCH-0008 – Semantic Types als systemweites Typsystem

## Status

Angenommen

## Kategorie

Systemarchitektur / Typsystem / Datenmodell / Capability-System / Execution

## Kontext

Klassische Betriebssysteme behandeln Daten an vielen Systemgrenzen überwiegend als technisch primitive Werte.

Typische Beispiele sind:

```text
byte[]
void*
char*
int
float
file descriptor
handle
path
socket
buffer
```

Diese Typen beschreiben zwar die technische Repräsentation, aber häufig nicht die eigentliche Bedeutung der Daten.

Ein Betriebssystem kann beispielsweise erkennen:

```text
Buffer {
    size = 8294400 bytes
}
```

weiß daraus aber noch nicht, ob dieser Buffer enthält:

- ein Bild,
- ein Video-Frame,
- ein Tensor,
- Audiodaten,
- einen Netzwerkframe,
- medizinische Bilddaten,
- Geometrie,
- Text,
- eine Matrix,
- komprimierte Daten,
- verschlüsselte Daten.

Auch ein Datentyp wie:

```text
Float32
```

beschreibt nicht, ob der Wert beispielsweise:

```text
Temperature<Celsius>
Distance<Meter>
Probability
Latitude
AudioSample
PixelChannel
```

repräsentiert.

Dadurch gehen wichtige semantische Informationen verloren.

Die Folgen sind unter anderem:

- unnötige Konvertierungen,
- unnötige Datenkopien,
- fehleranfällige API-Verwendung,
- inkompatible Einheiten,
- falsche Providerwahl,
- schlechtere automatische Algorithmuswahl,
- zusätzliche Serialisierung,
- schwierigere Capability-Komposition,
- schwächere Data-Sovereignty-Regeln,
- schwierigere Information-Flow-Kontrolle,
- weniger präzise Architecture Introspection.

NovaOS soll deshalb neben primitiven und strukturellen Typen ein **systemweites semantisches Typsystem** besitzen.

Semantic Types beschreiben nicht nur:

```text
Wie sind Daten gespeichert?
```

sondern zusätzlich:

```text
Was bedeuten diese Daten?
```

Das grundlegende Prinzip lautet:

```text
Representation
+
Structure
+
Semantics
+
Constraints
```

Semantic Types bilden damit eine gemeinsame Sprache zwischen:

- NovaLang,
- NovaOS APIs,
- Capabilities,
- Execution Contracts,
- Datenobjekten,
- IPC,
- Storage,
- Compute,
- KI,
- Grafik,
- Audio,
- Netzwerk,
- Security,
- Data Sovereignty,
- Architecture Introspection.

## Entscheidung

NovaOS verwendet **Semantic Types als systemweites Typsystem oberhalb beziehungsweise ergänzend zu primitiven Repräsentationstypen**.

Ein Semantic Type beschreibt die Bedeutung eines Wertes oder Datenobjekts so präzise, dass Systemkomponenten diese Information für:

- Typprüfung,
- Capability-Auflösung,
- Providerwahl,
- Algorithmuswahl,
- Datenkonvertierung,
- Sicherheitsentscheidungen,
- Ressourcenplanung,
- Datenflusskontrolle,
- Serialisierung,
- Introspection

verwenden können.

Primitive Typen bleiben erhalten.

Beispiel:

```text
Float32
```

ist weiterhin ein gültiger Repräsentationstyp.

Ein Semantic Type kann darauf aufbauen:

```text
Temperature<Celsius, Float32>
```

oder:

```text
Probability<Float32>
```

oder:

```text
Distance<Meter, Float64>
```

Das bevorzugte Modell lautet:

```text
Primitive Representation
        │
        ▼
Structural Type
        │
        ▼
Semantic Type
        │
        ▼
Semantic Constraints
```

## Grundprinzip

NovaOS trennt mindestens folgende Ebenen:

```text
Physical Representation
Logical Structure
Semantic Meaning
Policy / Execution Metadata
```

Beispiel:

```text
Physical Representation:
    UInt8[]

Logical Structure:
    width  = 1920
    height = 1080
    channels = 4

Semantic Meaning:
    Image<RGBA8>

Metadata:
    ColorSpace = sRGB
```

Diese Ebenen dürfen miteinander verbunden sein, sind aber nicht identisch.

## Primitive Types

Primitive Types bilden fundamentale Speicher- und Rechenrepräsentationen.

Beispiele:

```text
Bool
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
Decimal
Char
Byte
```

Semantic Types ersetzen diese Typen nicht.

Sie ergänzen sie.

## Structural Types

Structural Types beschreiben die Form von Daten.

Beispiele:

```text
Array<T>
Vector<T>
Matrix<T>
Tensor<T>
Record
Tuple
Map<K,V>
Sequence<T>
Stream<T>
```

Ein Structural Type beschreibt noch nicht zwangsläufig die vollständige fachliche Bedeutung.

Beispiel:

```text
Matrix<Float32>
```

kann sein:

- Transformationsmatrix,
- Messdatenmatrix,
- Gewichtsmatrix eines neuronalen Netzes,
- Bild,
- wissenschaftliche Daten.

## Semantic Types

Semantic Types geben einem strukturellen oder primitiven Wert eine definierte Bedeutung.

Beispiele:

```text
Temperature<Celsius>
Distance<Meter>
Duration<Millisecond>
Probability
Image<RGBA8>
Audio<PCM16>
VideoFrame<NV12>
Tensor<Float32>
MedicalImage<CT>
GeographicCoordinate<WGS84>
Money<EUR>
```

## Semantic Type Identity

Jeder systemweit registrierte Semantic Type MUSS eine stabile Identität besitzen.

Eine Semantic-Type-Identität soll unabhängig sein von:

- Programmiersprache,
- Provider,
- Speicherlayout,
- Prozess,
- Maschine.

Beispiel:

```text
nova.semantic.image.rgba
```

oder:

```text
de.nova.semantic.medical.ct
```

Die genaue Namenskonvention wird durch separate NPSPECs definiert.

## Namespace

Semantic Types SOLLEN Namespaces verwenden.

Beispiele:

```text
nova.semantic.image.*
nova.semantic.audio.*
nova.semantic.video.*
nova.semantic.math.*
nova.semantic.physics.*
nova.semantic.medical.*
nova.semantic.geo.*
nova.semantic.security.*
```

Namespaces dienen der Organisation.

Sie bestimmen nicht automatisch Kompatibilität.

## Versionierung

Semantic Types MÜSSEN versionierbar sein.

Beispiel:

```text
nova.semantic.image.rgba@1
```

Eine neue Version kann notwendig werden, wenn sich:

- Semantik,
- Invarianten,
- Metadatenanforderungen,
- Kompatibilitätsregeln

ändern.

Reine Implementierungsänderungen erfordern keine neue Semantic-Type-Version.

## Semantic Stability

Eine einmal veröffentlichte Semantic-Type-Version darf ihre Bedeutung nicht stillschweigend verändern.

Das Prinzip lautet:

```text
Stable identifier
=
Stable meaning
```

Wenn sich die Bedeutung inkompatibel ändert, muss eine neue Version oder ein neuer Typ eingeführt werden.

## Representation Independence

Ein Semantic Type darf mehrere physische Repräsentationen besitzen.

Beispiel:

```text
Image<RGBA>
```

kann gespeichert sein als:

```text
RGBA8
RGBA16
RGBA16F
RGBA32F
```

wenn der übergeordnete Semantic Type diese Varianten zulässt.

Die konkrete Representation kann als zusätzlicher Parameter oder Subtype beschrieben werden.

## Representation Type

Ein Semantic Type kann seine Repräsentation explizit angeben.

Beispiel:

```text
Image<
    PixelFormat = RGBA8,
    ColorSpace  = sRGB
>
```

oder:

```text
Tensor<
    ElementType = Float32,
    Layout      = NCHW
>
```

## Parameterisierte Semantic Types

Semantic Types SOLLEN Parameter unterstützen.

Beispiele:

```text
Temperature<Unit=Celsius>
```

```text
Image<
    PixelFormat=RGBA8,
    ColorSpace=sRGB
>
```

```text
Audio<
    Encoding=PCM,
    SampleType=Float32,
    Channels=2,
    SampleRate=48000Hz
>
```

Parameter sind Teil der semantischen Beschreibung.

## Generic Semantic Types

Semantic Types können generisch sein.

Beispiel:

```text
Measurement<TQuantity, TUnit, TValue>
```

Instanzen:

```text
Measurement<Temperature, Celsius, Float32>
Measurement<Distance, Meter, Float64>
Measurement<Mass, Kilogram, Float32>
```

## Units

Physikalische Einheiten werden Teil des Semantic-Type-Systems.

Beispiel:

```text
Distance<Meter>
Distance<Kilometer>
Distance<Mile>
```

NovaOS darf diese Typen nicht ohne explizite Konversion als identisch behandeln.

## Unit Safety

Folgende Operation soll typensicher möglich sein:

```text
Distance<Meter> + Distance<Meter>
```

Dagegen ist:

```text
Distance<Meter> + Duration<Second>
```

semantisch ungültig.

## Unit Conversion

Kompatible Einheiten können durch definierte Converter ineinander überführt werden.

Beispiel:

```text
Distance<Kilometer>
        ↓
Convert
        ↓
Distance<Meter>
```

Die Konversion ist explizit im Typ- und Capability-System sichtbar.

## Dimensions

NovaOS SOLL physikalische Dimensionen beschreiben können.

Beispiele:

```text
Length
Time
Mass
Temperature
Current
Amount
Luminosity
```

Daraus können abgeleitete Größen entstehen:

```text
Velocity = Length / Time

Acceleration = Length / Time²

Force = Mass × Length / Time²
```

## Dimensional Analysis

NovaLang beziehungsweise entsprechende Compute-Systeme dürfen Dimensional Analysis durchführen.

Beispiel:

```text
Distance / Duration
```

ergibt semantisch:

```text
Velocity
```

Dies reduziert Fehler in wissenschaftlichen und technischen Anwendungen.

## Semantic Constraints

Ein Semantic Type kann Invarianten besitzen.

Beispiel:

```text
Probability {
    minimum = 0
    maximum = 1
}
```

oder:

```text
Latitude {
    minimum = -90°
    maximum = 90°
}
```

Diese Invarianten können:

- statisch,
- zur Laufzeit,
- an Vertrauensgrenzen

geprüft werden.

## Refinement Types

Semantic Types dürfen durch zusätzliche Constraints verfeinert werden.

Beispiel:

```text
Integer where value >= 0
```

oder:

```text
Temperature<Celsius> where value >= -273.15
```

oder:

```text
Image where width <= 8192
```

Damit können bestimmte semantische Annahmen formal ausdrückbar werden.

## Validation

Semantic-Type-Validierung kann unterschiedliche Stufen besitzen:

```text
Trusted
Validated
Unchecked
Invalid
```

Ein Wert kann beispielsweise einen Semantic Type deklarieren, ohne dass alle Invarianten bereits geprüft wurden.

Das Typsystem muss zwischen:

```text
Declared semantic type
```

und:

```text
Verified semantic validity
```

unterscheiden können.

## Trust und Type Claims

Eine externe Quelle darf nicht allein durch Behauptung vertrauenswürdige Semantic-Type-Eigenschaften erzeugen.

Beispiel:

```text
Input claims:
    MedicalImage<ValidatedCT>
```

NovaOS muss unterscheiden können, ob diese Aussage:

- selbst deklariert,
- validiert,
- signiert,
- durch vertrauenswürdigen Provider erzeugt

wurde.

## Semantic Provenance

Semantic-Type-Information kann Provenance besitzen.

Beispiel:

```text
SemanticType:
    MedicalImage<CT>

AssignedBy:
    DICOMDecoderV4

Validation:
    Verified

Provenance:
    Signed
```

## Type Composition

Semantic Types SOLLEN zusammengesetzt werden können.

Beispiel:

```text
MedicalImage<
    Modality = CT,
    Anatomy  = Thorax
>
```

oder:

```text
GeoImage<
    CoordinateSystem = WGS84
>
```

## Structural Composition

Komplexe semantische Objekte können aus anderen Semantic Types bestehen.

Beispiel:

```text
DetectionResult {
    image: Image<RGBA8>
    objects: Sequence<DetectedObject>
}
```

mit:

```text
DetectedObject {
    class: ObjectClass
    confidence: Probability
    bounds: Rectangle<PixelCoordinate>
}
```

## Semantic Records

Records können semantisch typisierte Felder besitzen.

Beispiel:

```text
PatientMeasurement {
    timestamp: Timestamp<UTC>
    temperature: Temperature<Celsius>
    pulse: HeartRate<BeatsPerMinute>
}
```

## Nominal und Structural Typing

NovaOS soll nominale und strukturelle Typinformationen kombinieren können.

Nominale Identität beantwortet:

```text
Welcher definierte Typ ist das?
```

Strukturelle Kompatibilität beantwortet:

```text
Welche Felder beziehungsweise Eigenschaften besitzt er?
```

Semantic Compatibility darf jedoch nicht ausschließlich aus struktureller Gleichheit abgeleitet werden.

Beispiel:

```text
Latitude<Float64>
Longitude<Float64>
```

haben dieselbe Repräsentation und ähnliche Struktur, sind aber semantisch nicht austauschbar.

## Semantic Subtyping

Semantic Types dürfen Subtyping unterstützen.

Beispiel:

```text
Image
    └── MedicalImage
        └── CTImage
```

Ein `CTImage` kann dort akzeptiert werden, wo ein allgemeines `Image` erlaubt ist, sofern dadurch keine semantischen oder Security-Anforderungen verletzt werden.

## Subtyping Rules

Subtyping muss explizit definiert werden.

Namensähnlichkeit oder gleiche Speicherrepräsentation reicht nicht aus.

## Type Traits

Semantic Types können Traits beziehungsweise Eigenschaften besitzen.

Beispiele:

```text
Serializable
Streamable
Comparable
Numeric
ImageLike
TensorLike
Geospatial
Sensitive
PersonallyIdentifiable
```

Traits können von Capabilities für generische Anforderungen verwendet werden.

## Sensitive Types

Security- und Privacy-relevante Eigenschaften dürfen als semantische Typattribute beziehungsweise Information-Flow-Labels modelliert werden.

Beispiele:

```text
PersonalData
MedicalData
Secret
PublicData
Credential
BiometricData
```

Semantic Type und Security Label sind jedoch logisch zu unterscheiden.

Ein `MedicalImage<CT>` beschreibt die fachliche Bedeutung.

Ein Label wie:

```text
Confidential
```

beschreibt Security beziehungsweise Information Flow.

Beide können gemeinsam auf demselben Objekt existieren.

## Semantic Type versus Policy Label

Das System unterscheidet:

```text
Semantic Type:
    What is this data?
```

von:

```text
Policy Label:
    How may this data be used?
```

Beispiel:

```text
Semantic Type:
    Image<RGBA8>

Policy:
    Public
```

oder:

```text
Semantic Type:
    MedicalImage<CT>

Policy:
    PatientConfidential
```

## Semantic Type versus Storage Format

Semantic Type und Dateiformat sind nicht identisch.

Beispiel:

```text
Semantic Type:
    Image
```

mögliche Storage Formats:

```text
PNG
JPEG
WebP
AVIF
HEIF
```

Ein Dateiformat beschreibt eine Repräsentation beziehungsweise Kodierung.

Der Semantic Type beschreibt die Bedeutung.

## Semantic Type versus MIME Type

MIME Types dürfen als Kompatibilitätsmetadaten verwendet werden.

Sie sind jedoch nicht das native systemweite Semantic-Type-System von NovaOS.

Beispiel:

```text
image/png
```

ist weniger präzise als ein Semantic Type wie:

```text
Image<
    PixelFormat = RGBA8,
    ColorSpace = sRGB
>
```

## Files

Dateien und NovaFile-Objekte SOLLEN Semantic Types besitzen können.

Beispiel:

```text
NovaFile {
    semanticType = Image
    storageFormat = PNG
}
```

oder:

```text
NovaFile {
    semanticType = Document<Text>
    storageFormat = DOCX
}
```

## NovaFile

Im nativen NovaFile-Modell kann ein Objekt unter anderem enthalten:

```text
NovaFile {
    Payload
    SemanticType
    Metadata
    Provenance
    Security
    Integrity
}
```

Der Semantic Type beschreibt, was der Payload semantisch repräsentiert.

## Legacy Files

Bei Legacy-Dateien kann der Semantic Type aus verschiedenen Quellen ermittelt werden.

Beispiele:

- eingebettete Metadaten
- Magic Bytes
- Dateiformat
- Parser
- Nutzerzuweisung
- Provenance
- Capability Output

Eine Dateiendung allein ist kein vertrauenswürdiger vollständiger Semantic-Type-Nachweis.

## Type Detection

NovaOS darf Semantic Types automatisch erkennen.

Beispiel:

```text
Unknown Payload
    ↓
Type Detection
    ↓
Image<PNGEncoded>
```

Automatische Erkennung muss zwischen:

```text
Certain
Probable
Unknown
```

unterscheiden können.

## Type Confidence

Für inferierte Typen kann Confidence gespeichert werden.

Beispiel:

```text
TypeInference {
    semanticType = Image
    confidence = 0.98
}
```

Ein inferierter Typ ist nicht automatisch gleichwertig mit einem verifizierten Typ.

## Capability Inputs

Capabilities SOLLEN Semantic Types für Eingaben deklarieren.

Beispiel:

```text
Capability:
    de.nova.image.filter.gaussian

Input:
    Image

Output:
    Image
```

Ein spezialisierter Provider kann genauer deklarieren:

```text
Input:
    Image<RGBA8>

Output:
    Image<RGBA8>
```

## Capability Outputs

Auch Outputs müssen semantisch typisierbar sein.

Beispiel:

```text
Capability:
    nova.audio.transcribe

Input:
    Audio

Output:
    Transcript<Text>
```

Dadurch können nachfolgende Capabilities automatisch kompatible Daten erhalten.

## Capability Resolution

Semantic Types sind Bestandteil der Capability Resolution.

Beispiel:

```text
Input:
    Image<RGBA8>

Provider A:
    accepts Image<RGBA8>

Provider B:
    accepts Image<NV12>
```

Provider A kann direkt verwendet werden.

Provider B benötigt möglicherweise eine Konvertierung.

## Conversion Graph

NovaOS SOLL einen Graph erlaubter Typkonvertierungen besitzen.

Beispiel:

```text
Image<RGBA8>
    ↓
Image<RGB8>
    ↓
Image<Gray8>
```

oder:

```text
Temperature<Fahrenheit>
    ↓
Temperature<Celsius>
```

## Converter Capabilities

Konvertierungen SOLLEN selbst als Capabilities modelliert werden können.

Beispiel:

```text
nova.convert.image.rgba8-to-nv12
```

oder:

```text
nova.convert.temperature.fahrenheit-to-celsius
```

Damit gelten für Konvertierungen dieselben Prinzipien wie für andere Operationen:

- Providerwahl
- Resource Economy
- Security
- Trust
- Zero-Copy
- Introspection

## Lossless Conversion

Converter sollen deklarieren können, ob eine Konvertierung:

```text
Lossless
```

ist.

Beispiel:

```text
Meter → Millimeter
```

kann unter geeigneter Repräsentation verlustfrei sein.

## Lossy Conversion

Andere Konvertierungen können:

```text
Lossy
```

sein.

Beispiel:

```text
Image<RGBA16F>
    ↓
Image<RGB8>
```

Ein Execution Contract kann verbieten:

```text
lossyConversion = Forbidden
```

## Semantic Loss

NovaOS muss zwischen:

```text
Representation Conversion
```

und:

```text
Semantic Loss
```

unterscheiden können.

Eine Konvertierung darf nicht stillschweigend semantische Information entfernen.

Beispiel:

```text
MedicalImage
    ↓
GenericImage
```

kann medizinische Metadaten verlieren.

Dies muss explizit sichtbar sein.

## Conversion Cost

Konvertierungen besitzen Kosten.

Beispiele:

- CPU
- GPU
- Memory
- Data Movement
- Latency
- Energy

Provider Selection muss diese Kosten berücksichtigen.

## Zero-Copy Compatibility

Semantic Types helfen bei der Entscheidung, ob Zero-Copy möglich ist.

Beispiel:

```text
Producer Output:
    Image<RGBA8>

Consumer Input:
    Image<RGBA8>
```

Dann kann möglicherweise derselbe Buffer direkt verwendet werden.

Bei:

```text
Producer:
    Image<RGBA8>

Consumer:
    Image<NV12>
```

ist wahrscheinlich eine Konvertierung erforderlich.

## Memory Layout

Semantic Type und Memory Layout sind getrennt.

Beispiel:

```text
Tensor<Float32>
```

kann Layout besitzen:

```text
NCHW
NHWC
Blocked
```

Provider dürfen Layoutanforderungen deklarieren.

## Type Layout Descriptor

Ein Datenobjekt kann zusätzlich einen Layout Descriptor besitzen.

Beispiel:

```text
Tensor {
    semanticType = Tensor<Float32>
    shape = [1, 3, 224, 224]
    layout = NCHW
}
```

## Shape

Tensoren, Matrizen und ähnliche Daten können Shape als Teil der strukturellen beziehungsweise semantischen Beschreibung besitzen.

Beispiel:

```text
Tensor<Float32, Shape<1,3,224,224>>
```

## Dynamic Shape

Shapes dürfen dynamisch sein.

Beispiel:

```text
Tensor<Float32, Shape<N,3,224,224>>
```

Constraints können für `N` definiert werden.

## Image Types

Das Semantic-Type-System soll Bildtypen präzise beschreiben können.

Beispiel:

```text
Image<
    Width = 1920,
    Height = 1080,
    PixelFormat = RGBA8,
    ColorSpace = sRGB,
    TransferFunction = sRGB
>
```

Nicht jede Eigenschaft muss Bestandteil der nominalen Typidentität sein.

Ein Teil kann als typisierte Metadaten repräsentiert werden.

## Color Semantics

NovaOS muss Farbraum und Pixelrepräsentation unterscheiden.

Beispiel:

```text
PixelFormat = RGBA8
ColorSpace  = sRGB
```

RGBA8 allein beschreibt nicht vollständig, wie Werte farbmetrisch interpretiert werden.

## Video Types

Video-Frames können semantisch beschrieben werden durch:

```text
VideoFrame<
    PixelFormat = NV12,
    ColorSpace = BT.709,
    Range = Limited
>
```

Zusätzliche Informationen können umfassen:

- Timestamp
- Frame Number
- Field Order
- HDR Metadata

## Audio Types

Audiodaten können beschrieben werden durch:

```text
Audio<
    Encoding = PCM,
    SampleType = Float32,
    SampleRate = 48000Hz,
    Channels = Stereo
>
```

Channel Layout ist semantisch relevant.

```text
6 Channels
```

allein beschreibt nicht, ob die Daten:

```text
5.1
```

oder eine andere Kanalzuordnung besitzen.

## Text Types

Text soll mindestens folgende semantische Aspekte unterscheiden können:

```text
Text
EncodedText
RichText
Markup
SourceCode
Transcript
```

Encoding wie UTF-8 ist eine Repräsentationseigenschaft und nicht zwingend die vollständige Textsemantik.

## Time Types

Zeitwerte dürfen nicht nur als Integer behandelt werden.

Beispiele:

```text
Duration<Nanosecond>
Timestamp<UTC>
LocalDate
LocalTime
ZonedDateTime<Europe/Berlin>
MonotonicTime
```

Ein:

```text
Timestamp<UTC>
```

ist semantisch etwas anderes als:

```text
Duration<Second>
```

obwohl beide möglicherweise intern als 64-Bit-Zahl gespeichert werden.

## Clock Domain

Zeittypen können Clock Domains besitzen.

Beispiele:

```text
MonotonicClock
RealtimeClock
DeviceClock
MediaClock
```

Zeitwerte unterschiedlicher Clock Domains dürfen nicht ohne definierte Synchronisation direkt verglichen werden.

## Identifiers

Semantic Types sollen unterschiedliche Identifier-Arten unterscheiden.

Beispiele:

```text
UserId
ProcessId
CapabilityId
ObjectId
TransactionId
DeviceId
```

Selbst wenn alle intern beispielsweise `UInt128` sind, dürfen sie nicht automatisch austauschbar sein.

## Paths

Pfade können semantisch typisiert werden.

Beispiele:

```text
FilePath
DirectoryPath
CapabilityPath
NamespacePath
ResourcePath
```

Dadurch werden APIs präziser.

## Network Types

Netzwerkdaten können semantisch typisiert werden.

Beispiele:

```text
IPAddress<IPv4>
IPAddress<IPv6>
Port<TCP>
Port<UDP>
NetworkEndpoint
URI
DomainName
```

Ein Integer ist nicht automatisch ein Port.

## Geographic Types

Geografische Daten benötigen Referenzsysteme.

Beispiel:

```text
Coordinate<WGS84>
```

oder:

```text
Coordinate<ETRS89>
```

Koordinaten unterschiedlicher Referenzsysteme dürfen nicht ohne Transformation als identisch behandelt werden.

## Medical Types

Medizinische Daten profitieren besonders von Semantic Types.

Beispiele:

```text
MedicalImage<CT>
MedicalImage<MRI>
MedicalImage<XRay>
MedicalImage<Ultrasound>
DICOMStudy
DICOMSeries
DICOMInstance
RadiomicsFeature
SegmentationMask
DoseMap
```

Dadurch können medizinische Compute-Capabilities präzisere Ein- und Ausgabetypen definieren.

## Scientific Types

Wissenschaftliche Typen können Einheiten, Dimensionen und Unsicherheit enthalten.

Beispiel:

```text
Measurement<
    Quantity = Temperature,
    Unit = Celsius,
    Value = Float64,
    Uncertainty = 0.1
>
```

## Uncertainty

Messwerte dürfen Unsicherheit semantisch ausdrücken.

Beispiel:

```text
Measurement {
    value = 5.0
    uncertainty = ±0.2
}
```

Algorithmen können dadurch Unsicherheit weiterpropagieren.

## Probability

Wahrscheinlichkeiten sind Semantic Types.

Beispiel:

```text
Probability<Float32>
```

mit:

```text
0 <= value <= 1
```

Dies verhindert Verwechslung mit beliebigen Float-Werten.

## Confidence

Confidence und Probability müssen nicht zwingend semantisch identisch sein.

Ein Modell kann beispielsweise einen nicht kalibrierten Confidence Score liefern.

NovaOS darf unterschiedliche Typen definieren:

```text
Probability
ConfidenceScore
Logit
```

## Money

Geldwerte müssen Währung enthalten.

Beispiel:

```text
Money<EUR>
Money<USD>
```

```text
Money<EUR> + Money<USD>
```

ist ohne definierte Umrechnung nicht direkt zulässig.

## Security Types

Security-relevante Werte sollen getrennte Semantic Types besitzen.

Beispiele:

```text
CryptographicKey
PublicKey
PrivateKey
Hash
Signature
Nonce
Token
Capability
Credential
```

Ein Bytearray darf nicht automatisch als kryptografischer Schlüssel betrachtet werden.

## Cryptographic Parameters

Kryptografische Semantic Types können Parameter besitzen.

Beispiel:

```text
Hash<SHA256>
Signature<Ed25519>
PublicKey<Ed25519>
```

Dies reduziert Algorithmusverwechslungen.

## Capability IDs

Capability IDs erhalten einen eigenen Semantic Type.

Beispiel:

```text
CapabilityId
```

und nicht:

```text
String
```

Die textuelle Darstellung kann weiterhin String-basiert sein.

## Type-Safe APIs

Native NovaOS APIs SOLLEN Semantic Types verwenden, wenn eine semantische Unterscheidung relevant ist.

Beispiel:

statt:

```text
SetTimeout(UInt64 value)
```

besser:

```text
SetTimeout(Duration<Millisecond> value)
```

oder noch allgemeiner:

```text
SetTimeout(Duration value)
```

wobei die konkrete Einheit typsicher konvertiert wird.

## Compile-Time Checking

NovaLang SOLL möglichst viele Semantic-Type-Fehler bereits zur Compile-Zeit erkennen.

Beispiel:

```text
Distance<Meter> + Duration<Second>
```

soll als Typfehler erkannt werden.

## Runtime Checking

Dynamische Grenzen benötigen weiterhin Runtime-Prüfung.

Beispiele:

- IPC
- Netzwerk
- Dateien
- dynamisch geladene Capabilities
- Legacy-Code
- Foreign Function Interfaces

## Cross-Language Types

Semantic Types sollen sprachunabhängig definiert werden.

NovaLang kann native Syntax dafür besitzen.

Andere Sprachen können sie über ABI- beziehungsweise Schema-Bindings verwenden.

## Language Binding

Ein Semantic Type kann in verschiedenen Sprachen unterschiedlich repräsentiert werden.

Beispiel:

```text
NovaLang:
    Duration

Rust:
    NovaDuration

C:
    nova_duration_t
```

Die semantische Identität bleibt dieselbe.

## Native NovaLang Integration

NovaLang soll Semantic Types als erstklassiges Sprachkonzept unterstützen können.

Beispiel:

```text
Dim Temperature As Temperature<Celsius>
```

oder in einer später finalisierten NovaLang-Syntax entsprechend äquivalent.

Semantic Types sind keine separate Sprache.

Sie sind Bestandteil des gemeinsamen NovaLang-/NovaOS-Typsystems.

## `.nlf`

`.nlf` verwendet Semantic Types für Programmlogik.

Beispiel:

```text
Function ConvertTemperature(
    value As Temperature<Celsius>
) As Temperature<Fahrenheit>
```

Die endgültige NovaLang-Syntax wird in den NovaLang-ADRs festgelegt.

## `.nui`

Da `.nui` dieselbe NovaLang-Typsemantik verwendet, können auch UI-Bindings Semantic Types kennen.

Beispiel:

```text
TemperatureField {
    Value = CurrentTemperature
    DisplayUnit = Celsius
}
```

Die UI kann dadurch passende Darstellungen oder Controls auswählen.

## Semantic UI

UI-Komponenten dürfen Semantic Types verwenden, um geeignete Darstellung automatisch auszuwählen.

Beispiele:

```text
Date
    → Calendar Control

Color
    → Color Picker

GeographicCoordinate
    → Map Control

Image
    → Image Viewer
```

Dies ist eine höhere UI-Funktion und keine zwingende Kernelfunktion.

## Capability-driven UI

Da Capabilities ihre semantischen Inputs und Outputs deklarieren, kann NovaOS daraus UI-Komposition unterstützen.

Beispiel:

```text
Capability:
    Image.Rotate

Input:
    Image

Parameter:
    Angle<Degree>
```

Eine UI kann daraus geeignete Controls ableiten.

## Execution Contract

Semantic Types sind Bestandteil von `Nova.ExecutionContract`.

Beispiel:

```text
ExecutionContract {
    input {
        type = Image<RGBA8>
    }

    output {
        type = Image<RGBA8>
    }

    latency <= 16 ms
}
```

## Type Requirements

Execution Contracts können Typanforderungen formulieren.

Beispiel:

```text
input {
    semanticType = Image
    colorSpace = sRGB
}
```

Der Planner darf einen Converter einsetzen, sofern der Contract dies erlaubt.

## Forced Representation

Ein Contract kann eine konkrete Repräsentation verlangen.

Beispiel:

```text
output {
    semanticType = Image
    pixelFormat  = RGBA16F
}
```

## Preferred Representation

Eine Representation kann auch nur bevorzugt sein.

Beispiel:

```text
pixelFormat = Prefer<RGBA8>
```

Der Planner darf dann eine alternative kompatible Representation wählen.

## Automatic Algorithm Selection

Semantic Types verbessern automatische Algorithmuswahl.

Beispiel:

```text
Sort(data)
```

Der Typ kann Informationen liefern wie:

```text
Sequence<Integer>
Sequence<String>
Sequence<Timestamp>
```

Dadurch können unterschiedliche Algorithmen oder Optimierungen gewählt werden.

## Type-specific Algorithms

Provider dürfen Algorithmen für bestimmte Semantic Types optimieren.

Beispiel:

```text
Matrix<Float32>
```

kann andere Implementierungen verwenden als:

```text
Matrix<Int8>
```

oder:

```text
SparseMatrix<Float32>
```

## Automatic Provider Selection

Providerwahl berücksichtigt Semantic-Type-Kompatibilität.

Beispiel:

```text
Provider A:
    accepts Tensor<Float32, NCHW>

Provider B:
    accepts Tensor<Float16, NHWC>
```

Der Planner kann berechnen:

- direkte Kompatibilität
- Konvertierungspfad
- Conversion Cost
- Precision Loss

## Preferred/Forced Algorithm

Das bestehende NovaOS-Prinzip bleibt erhalten:

```text
Automatic by default.
Explicit override if required.
```

Ein expliziter Algorithmus darf jedoch nur verwendet werden, wenn er mit den Semantic Types kompatibel ist.

## Precision

Numeric Semantic Types können Precision Requirements enthalten.

Beispiel:

```text
Float32
Float64
Decimal128
```

oder semantisch:

```text
Measurement<
    precision >= 1e-9
>
```

## Approximation

Execution Contracts dürfen Approximation erlauben oder verbieten.

Beispiel:

```text
accuracy {
    approximate = Allowed
}
```

oder:

```text
approximate = Forbidden
```

Ein Provider mit Approximate Computing ist im zweiten Fall ungültig.

## Loss of Precision

Konvertierungen, die Precision reduzieren, müssen sichtbar sein.

Beispiel:

```text
Float64
    ↓
Float32
```

Eine solche Konvertierung darf bei einem Contract mit:

```text
precisionLoss = Forbidden
```

nicht automatisch durchgeführt werden.

## Deterministic Semantics

Semantic Types helfen, Deterministic Mode präziser zu definieren.

Ein Typ kann relevante Eigenschaften festlegen wie:

- Rundungsverhalten
- NaN-Semantik
- Overflow
- Encoding
- Ordering
- Locale

## Numeric Semantics

Primitive numerische Typen müssen definierte Semantik besitzen.

Beispiele:

```text
Overflow:
    Trap
    Wrap
    Saturate

Rounding:
    NearestEven
    TowardZero
```

Wenn deterministische Ergebnisse erforderlich sind, dürfen solche Details nicht undefiniert bleiben.

## Ordering

Nicht jeder Semantic Type besitzt eine natürliche Ordnung.

Beispiel:

```text
Temperature
```

kann geordnet werden.

Dagegen besitzt:

```text
Image
```

keine allgemein sinnvolle natürliche Gesamtordnung.

Sortieroperationen müssen entsprechende Traits verlangen.

## Equality

Semantic Equality kann sich von Byte Equality unterscheiden.

Beispiel:

```text
Distance<1000 Meter>
```

und:

```text
Distance<1 Kilometer>
```

können semantisch gleich sein, obwohl ihre Repräsentationen unterschiedlich sind.

## Hashing

Hashing muss unterscheiden zwischen:

```text
Representation Hash
```

und möglicherweise:

```text
Semantic Canonical Hash
```

Ein Byte-Hash bleibt wichtig für Integrity und Content Addressing.

Semantic Equality darf daraus nicht automatisch abgeleitet werden.

## Canonical Representation

Bestimmte Semantic Types dürfen eine kanonische Repräsentation definieren.

Dies kann hilfreich sein für:

- Serialization
- Signatures
- Hashing
- Reproducibility

Eine kanonische Repräsentation ist jedoch nicht für jeden Typ zwingend erforderlich.

## Serialization

Semantic Types müssen serialisierbar beschreibbar sein.

Eine Serialisierung SOLL mindestens enthalten beziehungsweise ableitbar machen:

```text
Type Identity
Type Version
Representation
Payload
```

zusätzliche Metadaten abhängig vom Typ.

## Typed Serialization

NovaOS soll nicht ausschließlich untypisierte Byte-Streams zwischen Systemkomponenten austauschen.

Bei strukturierten Systeminterfaces sollen Typinformationen erhalten bleiben können.

## Schema

Komplexe Semantic Types können Schemas besitzen.

Ein Schema beschreibt beispielsweise:

- Felder
- Feldtypen
- Optionalität
- Constraints
- Version

## Schema Evolution

Schemas müssen evolvierbar sein.

Zu unterscheiden sind:

```text
Backward Compatible
Forward Compatible
Breaking
```

Provider und Consumer müssen Versionen aushandeln können.

## IPC

Typed IPC verwendet Semantic Types.

Beispiel:

```text
Request {
    image: Image<RGBA8>
    radius: Length<Pixel>
}
```

Dadurch können Interfacefehler früher erkannt werden.

## IPC Validation

Bei IPC-Grenzen muss geprüft werden:

- Type Identity
- Type Version
- Shape
- Range
- Rights
- Payload Size

abhängig von der jeweiligen Vertrauensgrenze.

## Zero-Copy IPC

Semantic Type Metadata kann unabhängig vom großen Payload übertragen werden.

Beispiel:

```text
Message {
    type = VideoFrame<NV12>
    buffer = Capability<Buffer42>
}
```

Der Payload muss nicht kopiert werden.

## Remote Execution

Semantic Types müssen über Location-Grenzen hinweg stabil bleiben.

Ein:

```text
Image<RGBA8>
```

bleibt semantisch derselbe Typ, egal ob er lokal oder remote verarbeitet wird.

## Cross-Node Type Registry

Remote NovaOS-Nodes müssen Semantic-Type-Identitäten und Versionen eindeutig interpretieren können.

Dies kann über:

- standardisierte Type IDs,
- Type Schemas,
- Registry Metadata

erfolgen.

## Unknown Types

Ein System kann einen unbekannten Semantic Type empfangen.

Es muss dann unterscheiden können:

```text
Known
Unknown
Unsupported
IncompatibleVersion
```

Ein unbekannter Typ darf nicht automatisch als untypisierter Bytebuffer mit voller Semantik behandelt werden.

## Opaque Types

Unknown oder anwendungsdefinierte Typen können als opaque transportiert werden.

Beispiel:

```text
OpaqueSemanticObject {
    typeId
    version
    payload
}
```

Dies erlaubt Weiterleitung ohne vollständiges Verständnis des Inhalts.

## Application-defined Types

Anwendungen dürfen eigene Semantic Types definieren.

Beispiele:

```text
com.example.music.Chord
```

oder entsprechend der NovaOS-Namensregeln.

Systemweite Typen müssen von anwendungsspezifischen Typen unterscheidbar bleiben.

## Type Registration

Neue Semantic Types können registriert werden.

Eine Registrierung kann enthalten:

```text
TypeId
Version
Schema
Traits
Converters
Owner
Trust
Provenance
```

## Type Registry

NovaOS SOLL eine Semantic Type Registry besitzen.

Sie ermöglicht mindestens:

- Type Lookup
- Version Lookup
- Schema Lookup
- Trait Lookup
- Conversion Lookup
- Provenance Lookup

Die Registry muss logisch einheitlich sein, muss jedoch nicht als einzelner globaler Prozess implementiert werden.

## Registry Security

Nicht jede Anwendung darf Systemtypen verändern.

System Semantic Types benötigen stärkere Autorisierung.

Anwendungen dürfen eigene Namespaces kontrollieren, sofern entsprechende Capability-Rechte vorhanden sind.

## Type Squatting

Das System muss verhindern, dass eine Anwendung fremde Type IDs registriert.

Ownership und Authority der Namespace-Identität müssen überprüfbar sein.

## Type Trust

Registry-Einträge können Trust-Information besitzen.

Beispiel:

```text
Type:
    de.nova.medical.ct

Publisher:
    Nova Medical Platform

Trust:
    SystemVerified
```

## Compatibility

Semantic-Type-Kompatibilität muss explizit bestimmbar sein.

Mögliche Beziehungen:

```text
Exact
Subtype
LosslessConvertible
LossyConvertible
StructurallyCompatible
Incompatible
Unknown
```

## Exact Compatibility

Zwei Typen sind exakt kompatibel, wenn ihre semantischen Anforderungen übereinstimmen.

## Subtype Compatibility

Ein Subtype kann verwendet werden, wenn der Consumer einen Parent Type akzeptiert.

## Convertible Compatibility

Eine Konvertierung ist notwendig, wenn unterschiedliche, aber konvertierbare Typen vorliegen.

## Structural Compatibility

Strukturelle Gleichheit allein garantiert keine semantische Gleichheit.

Sie kann jedoch für generische Tools oder Schemaadapter relevant sein.

## Type Negotiation

Producer und Consumer dürfen eine gemeinsame Representation aushandeln.

Beispiel:

```text
Producer supports:
    RGBA8
    RGBA16F

Consumer supports:
    RGBA8
    NV12

Negotiated:
    RGBA8
```

Damit kann eine Konvertierung vermieden werden.

## Capability Negotiation

Provider können mehrere Input- und Outputtypen anbieten.

Der Planner kann daraus den günstigsten kompatiblen Pfad bestimmen.

## Conversion Planning

Ein komplexer Datenpfad kann mehrere mögliche Konversionen besitzen.

Beispiel:

```text
A
├── Convert → B → Provider1
└── Convert → C → Provider2
```

Der Planner berücksichtigt:

- Conversion Loss
- Resource Cost
- Latency
- Data Movement
- Trust

## Conversion Cycles

Der Conversion Graph darf keine unkontrollierten Konvertierungsschleifen erzeugen.

Planner müssen Zyklen erkennen und begrenzen.

## Semantic Pipeline

Ein NovaOS-Datenpfad kann vollständig über Semantic Types beschrieben werden.

Beispiel:

```text
CompressedImage<JPEG>
        ↓
Decode
        ↓
Image<RGB8>
        ↓
ColorConvert
        ↓
Image<RGBA8>
        ↓
Filter
        ↓
Image<RGBA8>
        ↓
Encode
        ↓
CompressedImage<AVIF>
```

## Automatic Pipeline Construction

NovaOS DARF aus Typanforderungen automatisch kompatible Pipelines zusammensetzen.

Beispiel:

```text
Input:
    Image<RGBA8>

Target Capability expects:
    Image<Gray8>
```

NovaOS kann einen Converter einfügen:

```text
RGBA8
    ↓
Gray8
    ↓
Capability
```

Dies darf nur erfolgen, wenn:

- Konversion erlaubt ist,
- Security erfüllt ist,
- Resource Budget reicht,
- Loss Policy erfüllt ist.

## Explicit Pipeline Control

Entwickler dürfen automatische Conversion und Pipeline Construction überschreiben.

Beispiel:

```text
conversion = Forbidden
```

oder:

```text
converter = SpecificConverter
```

Harte Systemconstraints bleiben bindend.

## Data Sovereignty

Semantic Types können Data-Sovereignty-Entscheidungen unterstützen.

Beispiel:

```text
MedicalImage
```

kann durch Policy automatisch strengere Datenregeln erhalten.

Die Policy darf jedoch nicht allein auf frei manipulierbaren Typangaben vertrauen.

Type Provenance und Security Labels müssen berücksichtigt werden.

## Information Flow

Semantic Types helfen bei Information-Flow-Regeln.

Beispiel:

```text
Credential
```

darf nicht an eine Capability übergeben werden, die nur:

```text
PublicText
```

erwartet und Daten extern veröffentlicht.

Semantic Types ersetzen jedoch kein vollständiges Information-Flow-Control-System.

## Security

Semantic Types erhöhen Typsicherheit, sind jedoch keine vollständige Security Boundary.

Ein gültiger Typ bedeutet nicht automatisch:

- autorisierte Daten,
- vertrauenswürdige Daten,
- sichere Daten.

Security muss weiterhin über:

- Capabilities
- Trust
- Information Flow
- Isolation

durchgesetzt werden.

## Type Confusion

NovaOS MUSS Type-Confusion-Angriffe berücksichtigen.

Ein Objekt mit Payload:

```text
ExecutableCode
```

darf nicht allein durch manipulierte Metadaten als:

```text
Image
```

behandelt werden, wenn daraus Sicherheitsrisiken entstehen.

Validierung muss an relevanten Trust Boundaries erfolgen.

## Unsafe Cast

Native APIs SOLLEN unkontrollierte Semantic-Type-Casts vermeiden.

Ein Cast zwischen semantisch inkompatiblen Typen muss explizit unsafe beziehungsweise unchecked sein.

## Safe Cast

Subtype- oder validierte Casts können sichere Operationen sein.

Beispiel:

```text
Image
    ↓ Validate
MedicalImage<CT>
```

wenn ein vertrauenswürdiger Validator die Voraussetzungen bestätigt.

## Type Erasure

Type Erasure darf an bestimmten Low-Level-Grenzen verwendet werden.

Beispiel:

```text
Raw Buffer
```

Der Verlust semantischer Informationen muss jedoch bewusst und lokal erfolgen.

Nach Möglichkeit sollen Typinformationen über Systemgrenzen erhalten bleiben.

## FFI

Foreign Function Interfaces müssen Semantic Types auf primitive ABI-Typen abbilden können.

Beispiel:

```text
Temperature<Celsius>
```

kann auf C-Ebene als:

```text
double
```

plus Typdescriptor repräsentiert werden.

Die semantische Information darf auf höherer Ebene erhalten bleiben.

## Legacy APIs

POSIX-, Linux- oder Win32-Kompatibilitätslayer können Semantic Types teilweise verlieren.

Dies darf jedoch nicht dazu führen, dass die native NovaOS-Architektur auf untypisierte Legacy-Modelle reduziert wird.

## Compatibility Projection

Ein Semantic Type kann auf eine Legacy-Repräsentation projiziert werden.

Beispiel:

```text
NovaOS:
    Duration<Millisecond>

Legacy:
    uint32_t timeout
```

Der Compatibility Layer übernimmt die Konvertierung.

## Resource Economy

Semantic Types können Resource Profiles beeinflussen.

Beispiel:

```text
Image<8K, RGBA16F>
```

impliziert andere Speicher- und Compute-Anforderungen als:

```text
Image<720p, Gray8>
```

Der Planner darf Typinformationen für Ressourcenprognosen verwenden.

## Type-based Resource Estimation

NovaOS DARF aus Typen Ressourcenbedarf ableiten.

Beispiel:

```text
Tensor<Float32, Shape<1,3,224,224>>
```

ermöglicht eine bekannte Mindestgröße.

Solche Berechnungen verbessern Admission Control.

## Dynamic Data Size

Nicht jeder Semantic Type bestimmt die exakte Größe.

Beispiel:

```text
Text
Video
CompressedImage
```

kann variable Payload-Größe besitzen.

Der Typ kann trotzdem Schätz- oder Maximalwerte bereitstellen.

## Location Transparency

Semantic Types bleiben über Locations stabil.

Dadurch kann dieselbe Capability lokal oder remote denselben Vertrag verwenden.

Location-spezifische Repräsentationen dürfen intern variieren.

## Data Locality

Typen können auch für Data-Locality-Entscheidungen relevant sein.

Ein großer:

```text
VideoStream<4K>
```

soll beispielsweise nicht unnötig zwischen Locations übertragen werden.

Der Resource Planner kann Typ und Größe gemeinsam bewerten.

## Structured Concurrency

Typed Tasks können deklarieren, welche Semantic Types sie erzeugen.

Beispiel:

```text
Task<Image>
```

oder:

```text
Task<DetectionResult>
```

Dies erleichtert sichere Pipeline-Komposition.

## Streams

Streamingtypen sind erstklassige semantische Typen.

Beispiel:

```text
Stream<VideoFrame<NV12>>
```

oder:

```text
Stream<AudioFrame<PCM32>>
```

Ein Stream ist nicht dasselbe wie eine vollständig materialisierte Collection.

## Bounded Streams

Streams können zusätzliche Constraints besitzen.

Beispiel:

```text
Stream<
    VideoFrame<NV12>,
    Rate = 60Hz
>
```

Diese Information kann Resource Economy und Backpressure unterstützen.

## Events

Events können typisiert werden.

Beispiel:

```text
Event<DeviceConnected>
Event<MemoryPressure>
Event<CapabilityChanged>
```

Dadurch wird systemweites Event Routing sicherer.

## State

Desired State und Actual State können semantisch typisierte Werte enthalten.

Beispiel:

```text
ThermalLimit = Temperature<Celsius>
```

anstatt:

```text
ThermalLimit = 80
```

ohne Einheit.

## Configuration

Systemkonfiguration SOLL Semantic Types verwenden.

Beispiel:

```text
Timeout = Duration<Second>(30)
```

anstatt:

```text
Timeout = 30
```

Dadurch werden Unit- und Interpretation-Fehler reduziert.

## Transaktionen

Transaktionale Zustandsänderungen müssen Typinvarianten erhalten.

Beispiel:

```text
Old State:
    Timeout<Second>

New State:
    Timeout<Millisecond>
```

Eine notwendige Konvertierung muss explizit berücksichtigt werden.

## Hot Replacement

Hot Replacement muss Semantic-Type-Versionen berücksichtigen.

Eine neue Komponente darf einen alten State nur übernehmen, wenn:

- Type Identity kompatibel,
- Version kompatibel,
- Conversion vorhanden

ist.

## State Migration

State Migration kann typgesteuert erfolgen.

Beispiel:

```text
State<TypeV1>
    ↓ Converter
State<TypeV2>
```

## Live Evolution

Semantic Types sind ein wichtiges Werkzeug für langfristige Systementwicklung.

Provider können sich verändern, solange ihre Type Contracts kompatibel bleiben.

Dadurch wird Implementierungsevolution von Datensemantik entkoppelt.

## Architecture Introspection

NovaOS MUSS Semantic Types introspektierbar machen können.

Beispiel:

```text
Object:
    Buffer42

Semantic Type:
    Image<RGBA8>

Color Space:
    sRGB

Dimensions:
    3840 × 2160

Owner:
    PhotoEditor

Provider:
    GPUImageProvider

Validation:
    Verified
```

## Type Introspection

Für einen Type soll mindestens abrufbar sein können:

```text
Type ID
Version
Namespace
Parameters
Traits
Schema
Constraints
Compatibility
Converters
Owner
Provenance
```

soweit vorhanden.

## Decision Tracing

Automatische Conversion- oder Providerentscheidungen SOLLEN typbezogen erklärbar sein.

Beispiel:

```text
Provider A rejected:
    requires Tensor<Float16>

Input:
    Tensor<Float32>

Reason:
    precisionLoss forbidden
```

oder:

```text
Provider B selected:

Input:
    Image<RGBA8>

Provider accepts:
    Image<RGBA8>

Conversion:
    none
```

## Observability

Semantic-Type-bezogene Metriken können umfassen:

- Type Conversion Count
- Lossless Conversions
- Lossy Conversions
- Conversion Failures
- Type Validation Failures
- Unknown Types
- Schema Mismatches
- Provider Rejections due to Type
- Zero-Copy Compatibility Rate

## Debugging

Debugger und Introspection-Tools sollen nicht nur rohe Speicherwerte anzeigen.

Statt:

```text
0x41A00000
```

kann dargestellt werden:

```text
Temperature<Celsius> = 20.0 °C
```

oder:

```text
Timestamp<UTC> = 2026-08-31T06:00:00Z
```

## Logging

Structured Logging SOLL Semantic Types erhalten können.

Beispiel:

```text
{
    temperature: Temperature<Celsius>(75)
}
```

anstatt einer bedeutungslosen Zahl.

## Persistence

Persistente Semantic Types müssen Type Identity und Version erhalten.

Dadurch kann ein späteres System erkennen, wie alte Daten interpretiert werden müssen.

## Long-Term Compatibility

Da NovaOS langfristig evolvierbar sein soll, dürfen persistente Daten nicht ausschließlich von der internen Speicherstruktur einer bestimmten Programmversion abhängen.

Semantic Types und Schemas bilden eine stabile Zwischenschicht.

## Type Migration

Persistente Typen müssen Migrationspfade besitzen können.

Beispiel:

```text
Type V1
    ↓
Migration
Type V2
```

Migrationspfade können transaktional ausgeführt werden.

## Canonical Schema Storage

Für langfristig relevante Datentypen SOLL die benötigte Schemaidentität dauerhaft erhalten bleiben können.

Das bedeutet nicht zwingend, dass das vollständige Schema in jedem Objekt eingebettet sein muss.

## Compression

Komprimierte Daten bleiben semantisch typisiert.

Beispiel:

```text
Compressed<Image, JPEG>
```

oder eine äquivalente Typmodellierung.

Kompression verändert die Repräsentation, nicht automatisch die zugrunde liegende fachliche Bedeutung.

## Encryption

Verschlüsselte Daten benötigen ebenfalls zwei Ebenen.

Beispiel:

```text
Encrypted<
    MedicalImage<CT>
>
```

Die äußere Repräsentation ist Ciphertext.

Die innere deklarierte Semantik bleibt medizinisches Bild.

Der Zugriff auf die innere Semantik kann natürlich Berechtigungen benötigen.

## Encapsulation

Wrapper Semantic Types können Informationen kapseln.

Beispiele:

```text
Encrypted<T>
Compressed<T>
Signed<T>
Optional<T>
Stream<T>
```

## Provenance Wrapper

Provenance soll bevorzugt als Metadaten- beziehungsweise Trust-Ebene behandelt werden.

Bei bestimmten APIs kann jedoch ein Typtrait ausdrücken, dass validierte Provenance erforderlich ist.

Beispiel:

```text
RequiresTrait<VerifiedProvenance>
```

## AI

KI-Systeme profitieren von Semantic Types.

Beispiel:

```text
Model Input:
    Image<RGB8, 224x224>

Model Output:
    ClassificationResult
```

Damit kann NovaOS automatisch:

- Resize
- Color Conversion
- Normalization

als Pipeline-Schritte planen, sofern erlaubt.

## Model Input Semantics

Ein Tensor allein reicht für KI häufig nicht aus.

Beispiel:

```text
Tensor<Float32, [1,3,224,224]>
```

sagt noch nicht, ob Werte:

```text
0..1
-1..1
0..255
```

repräsentieren.

Semantic Types können Normalization Semantics ausdrücken.

## AI Semantic Tensor

Ein präziser AI-Input könnte semantisch beschrieben werden als:

```text
ImageTensor<
    ElementType = Float32,
    Shape = [1,3,224,224],
    Layout = NCHW,
    ColorSpace = RGB,
    Normalization = ZeroToOne
>
```

## AI Output Types

Outputs sollten ebenfalls semantisch sein.

Beispiel:

```text
ClassificationResult {
    label: ClassLabel
    confidence: Probability
}
```

anstatt:

```text
Float32[]
```

## Medical AI

Medizinische KI kann noch strengere Type Contracts verwenden.

Beispiel:

```text
Input:
    MedicalImage<CT>

Output:
    SegmentationMask<Anatomy=Lung>
```

Dadurch können ungeeignete Eingaben früh abgelehnt werden.

## Safety

Safety-kritische Anwendungen profitieren von Semantic Types.

Ein Wert:

```text
Speed<KilometerPerHour>
```

darf nicht mit:

```text
Speed<MeterPerSecond>
```

verwechselt werden, ohne Konversion.

## Hard Realtime

Semantic Types dürfen Hard-Realtime-Pfade nicht durch unnötige dynamische Reflection verlangsamen.

Kritische Typeinformationen sollen vorab:

- kompiliert,
- validiert,
- gecacht

werden können.

## Fast Path

Systemweite Semantic Types bedeuten nicht, dass bei jeder einzelnen Operation komplexe Registry-Lookups notwendig sind.

Das bevorzugte Modell lautet:

```text
Control Plane:
    Resolve Type
    Validate Contract
    Resolve Provider

Fast Path:
    Compact Type ID
    Prevalidated Layout
    Direct Execution
```

## Compact Type IDs

Zur Laufzeit dürfen kompakte numerische oder internierte Type IDs verwendet werden.

Beispiel:

```text
TypeHandle = 0x00003142
```

Diese sind interne Optimierungen.

Die stabile semantische Type Identity bleibt davon getrennt.

## Type Cache

Type- und Compatibility-Informationen dürfen gecacht werden.

Caches müssen invalidiert werden, wenn:

- Registry-Version,
- Type-Version,
- Provider Contract

sich relevant ändern.

## Compile-Time Specialization

Provider und NovaLang Compiler dürfen Code auf Semantic Types spezialisieren.

Beispiel:

```text
Filter<Image<RGBA8>>
```

kann einen optimierten Pfad erhalten.

## Monomorphization

Generische Semantic Types dürfen monomorphisiert werden, wenn dies Performancevorteile bringt.

Dies ist eine Compilerentscheidung und keine Änderung der öffentlichen Semantik.

## Dynamic Typing

NovaOS darf dynamisch typisierte Anwendungen unterstützen.

Dynamische Sprache bedeutet jedoch nicht, dass Systemgrenzen untypisiert sein müssen.

Ein Runtime Adapter kann Semantic Types zur Laufzeit prüfen.

## Any

Ein generischer Typ wie:

```text
Any
```

darf existieren.

Er soll jedoch nicht als Standard für native Systeminterfaces verwendet werden, wenn ein präziser Typ möglich ist.

## Unknown

`Unknown` soll von `Any` unterschieden werden.

```text
Any
```

bedeutet:

```text
Jeder Typ ist akzeptabel.
```

```text
Unknown
```

bedeutet:

```text
Der Typ ist aktuell nicht bekannt.
```

## Optional

Optionalität soll typisiert werden.

Beispiel:

```text
Optional<Temperature<Celsius>>
```

anstatt spezielle Magic Values wie:

```text
-9999
```

für fehlende Daten zu verwenden.

## Result Types

Operationsergebnisse SOLLEN Fehler typisiert darstellen können.

Beispiel:

```text
Result<Image, DecodeError>
```

Die genaue NovaLang-Syntax wird separat definiert.

## Null

NovaOS soll Nullability explizit modellieren.

Ein nicht-nullbarer Semantic Type soll nicht stillschweigend `null` enthalten dürfen.

## Error Types

Fehler selbst können Semantic Types besitzen.

Beispiele:

```text
TimeoutError
PermissionDenied
TypeMismatch
ConversionError
LocationFailure
ResourceUnavailable
```

Damit kann Structured Error Handling präziser werden.

## Type Contracts

Interfaces können Type Contracts veröffentlichen.

Beispiel:

```text
CapabilityContract {
    input  = Image
    output = Image
}
```

Provider verfeinern diesen gegebenenfalls.

## Contract Variance

Bei Input- und Outputtypen müssen definierte Varianzregeln gelten.

Ein Provider darf beispielsweise keine engeren Inputanforderungen verstecken, wenn der Capability Contract breitere Typen garantiert.

Die genaue Varianzsemantik wird separat spezifiziert.

## Semantic ABI

NovaOS SOLL eine Semantic ABI besitzen, die Typidentität und Type Metadata an Systemgrenzen repräsentieren kann.

Dies ist zusätzlich zur Low-Level Binary ABI zu verstehen.

## Binary ABI

Die Binary ABI beantwortet:

```text
Wie werden Werte technisch übergeben?
```

Die Semantic ABI beantwortet:

```text
Was bedeuten die übergebenen Werte?
```

Beide Ebenen müssen zusammenarbeiten.

## Schema ABI

Komplexe strukturierte Typen können über stabile Schema-Deskriptoren beschrieben werden.

Dadurch können Versionierung und Cross-Language-Kompatibilität unterstützt werden.

## Reflection

NovaOS und NovaLang SOLLEN Reflection über Semantic Types unterstützen können.

Beispiel:

```text
GetType()
GetSemanticType()
GetSchema()
GetTraits()
GetUnit()
```

Reflection-Zugriff kann aus Security- und Performancegründen eingeschränkt werden.

## Static Reflection

Wo möglich soll Static Reflection verwendet werden können.

Dadurch können viele Entscheidungen bereits zur Compile-Zeit getroffen werden.

## Dynamic Reflection

Dynamic Reflection bleibt für:

- Tools
- Plugins
- dynamische Capabilities
- Introspection
- Serialisierung

notwendig.

## Type Security

Nicht jede Komponente darf beliebige Typinformationen manipulieren.

Insbesondere:

- Type Identity
- Trust Claims
- Validation State
- Security Labels

müssen geschützt werden.

## Type Metadata Integrity

Type Metadata muss gegen unautorisierte Manipulation geschützt werden.

Ein Prozess darf beispielsweise nicht einfach:

```text
UntrustedData
```

in:

```text
ValidatedCredential
```

umetikettieren.

## Validation Capability

Bestimmte Validierungen können eigene Capabilities erfordern.

Beispiel:

```text
nova.validate.medical.dicom
```

Ein erfolgreicher Validator kann einen stärker verifizierten Typzustand erzeugen.

## Type State Transition

Validierung kann einen semantischen Typzustand ändern.

Beispiel:

```text
Unvalidated<Image>
    ↓ Validate
Validated<Image>
```

Dies ist konzeptionell eine kontrollierte Type Refinement Operation.

## Taint

Untrusted Inputs können Taint beziehungsweise Information-Flow-Metadaten besitzen.

Beispiel:

```text
Text
+
Source = Network
+
Trust = Untrusted
```

Semantic Type und Taint bleiben getrennte, aber kombinierbare Ebenen.

## Sanitization

Sanitization kann als Capability modelliert werden.

Beispiel:

```text
Untrusted<Html>
    ↓
Sanitize
    ↓
SafeHtml
```

Die genaue Security-Semantik wird durch Security-NPSPECs festgelegt.

## Data Sovereignty Typintegration

Bestimmte Semantic Types können Standard-Policy-Hints besitzen.

Beispiel:

```text
MedicalData
```

kann standardmäßig strengere Sovereignty Policies vorschlagen.

Solche Defaults dürfen durch autoritative Policy definiert werden.

Der Semantic Type selbst ist nicht automatisch die Policy.

## Declarative System Model

Semantic Types sind Bestandteil des deklarativen Systemmodells.

Beispiel:

```text
Resource {
    value = Temperature<Celsius>(80)
}
```

oder:

```text
DesiredState {
    MaximumTemperature = Temperature<Celsius>(75)
}
```

## Mechanism / Policy Separation

Das Typsystem definiert:

```text
Meaning
Compatibility
Constraints
```

Policies entscheiden:

```text
Welche kompatible Konvertierung oder welcher Provider soll verwendet werden?
```

Das Typsystem soll keine willkürlichen Policyentscheidungen hardcodieren.

## Resource Economy

Semantic Types liefern Informationen an die Resource Economy.

Die Resource Economy darf daraus jedoch keine ungesicherten Annahmen ableiten, wenn Größe oder Kosten unbekannt sind.

## Zero-Copy

Semantic Types und Zero-Copy sind eng gekoppelt.

Zwei Komponenten können denselben Buffer direkt teilen, wenn:

```text
Semantic Type compatible
AND
Representation compatible
AND
Layout compatible
AND
Security compatible
```

## Location Transparency

Semantic Types bleiben location-unabhängig.

Ein Typ soll nicht anders heißen, nur weil Daten:

- lokal,
- remote,
- auf GPU,
- auf NPU

liegen.

Location ist separate Execution- beziehungsweise Resource-Metadata.

## Deterministic Mode

Deterministic Mode muss Semantic-Type-Konvertierungen reproduzierbar machen können.

Relevant sind insbesondere:

- Rundung
- Encoding
- Floating-Point
- Sortierung
- Locale
- Zeitzone
- Unicode Normalization

## Locale

Locale darf nicht implizit die Semantik eines gespeicherten numerischen Werts verändern.

Beispiel:

```text
Decimal
```

ist intern unabhängig davon, ob eine UI:

```text
1,25
```

oder:

```text
1.25
```

anzeigt.

## Unicode

Text-Semantik muss Unicode-Normalisierung berücksichtigen können.

Beispielsweise können unterschiedliche Bytefolgen semantisch denselben sichtbaren Text darstellen.

APIs müssen definieren, ob:

- Byte Equality,
- Codepoint Equality,
- Normalized Equality

verwendet wird.

## Architecture Introspection

Semantic Types sind zentral für Architecture Introspection.

Anstatt nur:

```text
Buffer42
```

zu sehen, soll NovaOS beispielsweise anzeigen können:

```text
Buffer42
    SemanticType = Tensor<Float32>
    Shape = [1,3,224,224]
    Layout = NCHW
    Location = NPU0.Memory
```

## Architecture Graph

Systemobjekte können über Typeinformationen miteinander verbunden werden.

Beispiel:

```text
Camera
    │ VideoFrame<NV12>
    ▼
Decoder
    │ Image<RGBA8>
    ▼
AI
    │ DetectionResult
    ▼
Renderer
```

Dadurch wird der Systemdatenfluss semantisch nachvollziehbar.

## Hot Replacement

Komponenten dürfen hot replaced werden, wenn ihre Type Contracts kompatibel bleiben.

Ein neuer Provider mit inkompatiblem Outputtyp darf nicht stillschweigend an bestehende Consumer gebunden werden.

## Capability Rebinding

Rebinding muss Type Compatibility prüfen.

Beispiel:

```text
Old Provider:
    Output = Image<RGBA8>

New Provider:
    Output = Image<RGBA8>
```

direkt kompatibel.

Bei:

```text
New Provider:
    Output = Image<RGB16F>
```

muss geprüft werden, ob:

- Consumer kompatibel,
- Converter vorhanden,
- Conversion erlaubt

ist.

## Self-Healing

Self-Healing kann Type Violations erkennen.

Beispiele:

- beschädigte Metadaten
- falsche Schema-Version
- unerwarteter Provider Output
- ungültige Unit
- Invalid Range

Recovery kann einen anderen Provider wählen oder Daten neu validieren.

## Failure Handling

Type-bezogene Fehler sollen präzise klassifiziert werden.

Beispiele:

```text
TypeMismatch
UnknownType
UnsupportedType
UnsupportedTypeVersion
InvalidSemanticValue
ConversionUnavailable
LossyConversionForbidden
SchemaMismatch
```

## Fail-Safe

Wenn NovaOS die Bedeutung sicherheitskritischer Daten nicht zuverlässig bestimmen kann, soll es nicht automatisch die semantisch stärkste Interpretation annehmen.

Beispiel:

```text
Unknown credential data
```

darf nicht automatisch als:

```text
ValidatedCredential
```

behandelt werden.

## Type Negotiation Failure

Wenn Producer und Consumer keinen kompatiblen Typ finden, muss die Operation explizit fehlschlagen können.

Beispiel:

```text
NoCompatibleRepresentation
```

## Performance

Semantic Types dürfen Fast Paths nicht durch permanente komplexe Schemaauswertung belasten.

Deshalb werden bevorzugt:

- internierte Type IDs,
- gecachte Compatibility Results,
- vorvalidierte Contracts,
- Compiler Specialization

verwendet.

## Type Resolution

Typauflösung findet bevorzugt im Control Plane statt.

Beispiel:

```text
Capability Resolution
    ↓
Type Resolution
    ↓
Conversion Planning
    ↓
Provider Selection
    ↓
Prepared Execution Plan
```

Der Fast Path verwendet anschließend vorbereitete Type Handles.

## Boot

Frühe Bootphasen dürfen reduzierte primitive Typmodelle verwenden.

Das vollständige systemweite Semantic-Type-System muss nicht vor der initialen Memory- und Kernelinitialisierung verfügbar sein.

Nach Start der entsprechenden Core Services beziehungsweise Runtime-Komponenten muss die native Systemarchitektur Semantic Types verwenden können.

## Kernel

Der Kernel muss nicht sämtliche fachlichen Semantic Types kennen.

Der Kernel stellt fundamentale Mechanismen bereit für:

- Type IDs an Systemobjekten,
- sichere Übergabe,
- Capability Checks,
- Buffer Metadata,
- ABI-Unterstützung.

Höhere Type Schemas und fachliche Typen können außerhalb des Kernels verwaltet werden.

## Kernel Minimalism

Der Kernel soll nicht mit tausenden fachlichen Typdefinitionen überladen werden.

Das Modell lautet:

```text
Kernel:
    Type identity transport
    Enforcement primitives
    Fundamental types

System Type Runtime:
    Schemas
    Compatibility
    Conversion Graph
    Reflection
```

## Recovery

NovaDOS darf ein reduziertes Type-System verwenden.

Recovery-Correctness darf nicht von der vollständigen Semantic-Type-Registry des laufenden NovaOS abhängen.

Für NovaFile- oder Systemrecovery müssen jedoch ausreichend Typinformationen lesbar bleiben, um kritische Daten sicher zu interpretieren.

## Normative Festlegungen

1. NovaOS MUSS Semantic Types als systemweites Architekturprinzip unterstützen.

2. Semantic Types MÜSSEN die semantische Bedeutung eines Wertes unabhängig von seiner reinen Speicherrepräsentation beschreiben können.

3. Primitive Types MÜSSEN weiterhin unterstützt werden.

4. Semantic Types DÜRFEN primitive Typen NICHT unnötig ersetzen, wenn keine zusätzliche Semantik erforderlich ist.

5. Semantic Type, Structural Type und Physical Representation MÜSSEN logisch unterscheidbar sein.

6. Semantic Types MÜSSEN stabile Identitäten besitzen können.

7. Semantic Types MÜSSEN versionierbar sein.

8. Eine veröffentlichte Semantic-Type-Version DARF ihre Bedeutung NICHT inkompatibel verändern.

9. Inkompatible semantische Änderungen MÜSSEN eine neue Version oder Type Identity verwenden.

10. Semantic Types SOLLEN sprachunabhängig definierbar sein.

11. NovaLang SOLL Semantic Types als erstklassige Typen verwenden können.

12. `.nlf` und `.nui` MÜSSEN dasselbe grundlegende Semantic-Type-System verwenden können.

13. Semantic Types SOLLEN parameterisierbar sein.

14. Semantic Types SOLLEN generische Typen unterstützen können.

15. Physikalische Einheiten MÜSSEN semantisch typisierbar sein.

16. Werte unterschiedlicher inkompatibler Dimensionen DÜRFEN NICHT ohne explizite Operation miteinander verrechnet werden.

17. Kompatible Einheiten SOLLEN über definierte Converter konvertierbar sein.

18. Semantic Types SOLLEN Range- und Invariant-Constraints ausdrücken können.

19. NovaOS MUSS zwischen deklarierter und verifizierter Semantic-Type-Gültigkeit unterscheiden können.

20. Untrusted Type Claims DÜRFEN NICHT automatisch als verifiziert gelten.

21. Semantic-Type-Information SOLL Provenance besitzen können.

22. Semantic Type und Security Label MÜSSEN logisch getrennt werden.

23. Semantic Type und Data-Sovereignty-Policy MÜSSEN logisch getrennt werden.

24. Semantic Type und Storage Format MÜSSEN logisch getrennt werden.

25. Semantic Type und MIME Type MÜSSEN logisch getrennt werden.

26. NovaFile-Objekte SOLLEN Semantic Types speichern können.

27. Legacy-Dateien DÜRFEN durch Type Detection Semantic Types erhalten.

28. Dateiendungen DÜRFEN NICHT als alleiniger vertrauenswürdiger Type-Nachweis betrachtet werden.

29. Inferierte Typen MÜSSEN von verifizierten Typen unterscheidbar sein.

30. Capabilities SOLLEN Semantic Types für Inputs und Outputs deklarieren.

31. Capability Resolution MUSS Semantic-Type-Kompatibilität berücksichtigen können.

32. Provider DÜRFEN spezifischere Representation Requirements deklarieren.

33. NovaOS SOLL einen Conversion Graph für kompatible Semantic Types unterstützen.

34. Type Conversions SOLLEN als Capabilities modellierbar sein.

35. Converter MÜSSEN angeben können, ob eine Konvertierung verlustfrei oder verlustbehaftet ist.

36. Verlustbehaftete Konvertierungen DÜRFEN NICHT automatisch verwendet werden, wenn ein Hard Contract dies verbietet.

37. Semantischer Informationsverlust MUSS von reinem Representation Change unterscheidbar sein.

38. Conversion Cost MUSS in die Resource Economy integrierbar sein.

39. Zero-Copy-Kompatibilität MUSS Semantic Type, Representation und Layout berücksichtigen können.

40. Semantic Types DÜRFEN mehrere physische Repräsentationen besitzen.

41. Memory Layout MUSS von Semantic Meaning unterscheidbar sein.

42. Tensor Shapes und Layouts MÜSSEN typisiert beziehungsweise durch Type Metadata beschreibbar sein.

43. Image Pixel Format und Color Space MÜSSEN getrennt beschreibbar sein.

44. Audio Sample Format und Channel Layout MÜSSEN getrennt beschreibbar sein.

45. Zeittypen MÜSSEN Einheit und Clock Domain unterscheiden können.

46. Identifier unterschiedlicher semantischer Domänen DÜRFEN NICHT allein aufgrund gleicher Binärrepräsentation austauschbar sein.

47. Native NovaOS APIs SOLLEN Semantic Types verwenden, wenn dadurch Mehrdeutigkeiten reduziert werden.

48. NovaLang SOLL Semantic-Type-Fehler soweit möglich zur Compile-Zeit erkennen.

49. Dynamische Systemgrenzen MÜSSEN Runtime-Type-Validierung unterstützen.

50. Semantic Types MÜSSEN über IPC übertragbar sein.

51. Typed IPC SOLL Semantic Types erhalten.

52. Zero-Copy IPC SOLL Type Metadata getrennt vom Payload übertragen können.

53. Semantic Types MÜSSEN über Node- und Location-Grenzen stabil interpretierbar sein.

54. Remote Nodes MÜSSEN Type Identity und Version eindeutig austauschen können.

55. Unknown Semantic Types MÜSSEN explizit darstellbar sein.

56. Unknown Types DÜRFEN als opaque weitergeleitet werden, sofern Security und Contract dies zulassen.

57. Anwendungen DÜRFEN eigene Semantic Types definieren.

58. Systemtypen MÜSSEN von anwendungsdefinierten Types unterscheidbar sein.

59. Type Namespaces MÜSSEN Ownership beziehungsweise Authority besitzen können.

60. Unautorisierte Komponenten DÜRFEN fremde Type Namespaces NICHT übernehmen.

61. NovaOS SOLL eine Semantic Type Registry besitzen.

62. Die Registry MUSS Type Identity, Version und grundlegende Type Metadata auflösen können.

63. Die Registry SOLL Schemas, Traits und Converter auffindbar machen können.

64. Die Semantic Type Registry MUSS NICHT als einzelner zentraler Prozess implementiert sein.

65. Type Registry Mutationen MÜSSEN Capability-basiert autorisiert werden.

66. Semantic Compatibility MUSS explizite Beziehungen wie Exact, Subtype, Convertible und Incompatible ausdrücken können.

67. Strukturelle Gleichheit DARF NICHT automatisch semantische Gleichheit bedeuten.

68. Provider und Consumer SOLLEN kompatible Representations aushandeln können.

69. Type Negotiation SOLL unnötige Conversion vermeiden.

70. Automatische Conversion DARF nur erfolgen, wenn Contract und Policies dies erlauben.

71. Entwickler MÜSSEN automatische Conversion deaktivieren können.

72. Automatische Pipeline Construction DARF aus Semantic-Type-Beziehungen erfolgen.

73. Automatisch erzeugte Pipelines MÜSSEN Resource-, Security-, Trust- und Data-Sovereignty-Constraints einhalten.

74. Konvertierungszyklen MÜSSEN erkannt und begrenzt werden.

75. Semantic Types MÜSSEN Bestandteil von `Nova.ExecutionContract` sein können.

76. Execution Contracts MÜSSEN Input- und Output-Type-Anforderungen ausdrücken können.

77. Execution Contracts SOLLEN Representation Preferences ausdrücken können.

78. Execution Contracts MÜSSEN Hard Representation Requirements ausdrücken können.

79. Automatische Algorithmuswahl SOLL Semantic Types berücksichtigen.

80. Automatische Providerwahl MUSS Semantic-Type-Kompatibilität berücksichtigen können.

81. Ein expliziter Algorithmus-Override DARF inkompatible Type Contracts NICHT umgehen.

82. Precision Requirements MÜSSEN ausdrückbar sein können.

83. Precision Loss MUSS bei Konvertierungen erkennbar sein.

84. Approximate Computing MUSS durch Semantic beziehungsweise Execution Constraints begrenzbar sein.

85. Deterministic Mode MUSS relevante Type Semantics wie Rundung und Encoding fixieren können.

86. Semantic Equality MUSS von Byte Equality unterscheidbar sein können.

87. Representation Hashing DARF NICHT automatisch Semantic Equality definieren.

88. Typen DÜRFEN kanonische Repräsentationen definieren.

89. Persistente Semantic Types MÜSSEN ihre Type Identity und Version erhalten können.

90. Schema Evolution MUSS unterstützt werden.

91. Breaking Schema Changes MÜSSEN erkennbar sein.

92. Persistente Type Migration SOLL transaktional durchgeführt werden können.

93. Hot Replacement MUSS Type-Contract-Kompatibilität prüfen können.

94. Capability Rebinding MUSS Input- und Output-Type-Kompatibilität berücksichtigen.

95. State Migration SOLL Semantic-Type-Versionen verwenden können.

96. Type Confusion MUSS als Security-Risiko behandelt werden.

97. Type Metadata MUSS gegen unautorisierte Manipulation geschützt werden.

98. Unsafe Semantic Casts MÜSSEN explizit als unsicher erkennbar sein.

99. Safe Type Refinement MUSS über Validierung möglich sein.

100. Semantic Types DÜRFEN Security Labels ergänzen, aber NICHT ersetzen.

101. Semantic Types DÜRFEN Capability Security NICHT ersetzen.

102. Semantic Types DÜRFEN Trust Validation NICHT ersetzen.

103. Semantic Types SOLLEN Resource Estimation unterstützen können.

104. Type-basierte Resource Estimation DARF für Admission Control verwendet werden.

105. Semantic Types MÜSSEN mit der Resource Economy integrierbar sein.

106. Semantic Types MÜSSEN mit Zero-Copy-Planning integrierbar sein.

107. Semantic Types MÜSSEN mit Location Transparency kompatibel sein.

108. Location DARF NICHT Teil einer Semantic-Type-Identität sein, sofern Location nicht selbst fachliche Semantik ist.

109. Streamingtypen MÜSSEN als eigenständige semantische Formen modellierbar sein.

110. Stream Rate und ähnliche Constraints SOLLEN beschreibbar sein.

111. Typed Events SOLLEN unterstützt werden.

112. Desired State und Configuration SOLLEN Semantic Types verwenden können.

113. Fehlende Werte SOLLEN über Optionalität statt Magic Values modellierbar sein.

114. Nullability SOLL explizit typisiert werden.

115. Fehler SOLLEN als Semantic Error Types modellierbar sein.

116. Kernel Fast Paths DÜRFEN NICHT von komplexer dynamischer Type Reflection abhängig sein.

117. Type Resolution SOLL möglichst im Control Plane erfolgen.

118. Fast Paths SOLLEN kompakte internierte Type Handles verwenden können.

119. Type Compatibility Results DÜRFEN gecacht werden.

120. Type Caches MÜSSEN bei relevanten Registry- oder Contract-Änderungen invalidierbar sein.

121. Compiler DÜRFEN Semantic Types zur Spezialisierung verwenden.

122. Dynamisch typisierte Sprachen DÜRFEN unterstützt werden, müssen jedoch native Semantic-Type-Grenzen respektieren können.

123. `Any` und `Unknown` MÜSSEN semantisch unterscheidbar sein.

124. Architecture Introspection MUSS Semantic Types von Systemobjekten anzeigen können.

125. Type Introspection SOLL Identity, Version, Schema, Traits und Constraints sichtbar machen können.

126. Decision Tracing SOLL Type-basierte Provider- und Conversion-Entscheidungen erklären können.

127. Observability SOLL Type Conversion und Type Validation messen können.

128. Debugger SOLLEN Semantic-Type-Werte menschenverständlich darstellen können.

129. Structured Logging SOLL Semantic-Type-Information erhalten können.

130. Semantic Types SOLLEN langfristige Datenkompatibilität unterstützen.

131. Der Kernel MUSS NICHT sämtliche fachlichen Semantic Types selbst kennen.

132. Der Kernel MUSS jedoch Type Identity sicher über Systemgrenzen transportieren können.

133. Höhere Type Schemas SOLLEN außerhalb des minimalen Kernelkerns verwaltet werden.

134. Ein Ausfall der vollständigen Type Registry DARF fundamentale Kernelmechanismen NICHT funktionsunfähig machen.

135. Kritische Systemtypen SOLLEN über lokale beziehungsweise gecachte Definitionen weiterhin verwendbar sein.

136. NovaDOS DARF ein reduziertes Type-System verwenden.

137. Recovery MUSS ausreichend Type Metadata interpretieren können, um kritische persistente NovaOS-Daten sicher zu behandeln.

138. KI-basierte Type Inference DARF Typen vorschlagen oder erkennen.

139. KI-basierte Type Inference DARF NICHT ohne Validation automatisch privilegierte Trust- oder Security-Semantik erzeugen.

140. Das Semantic-Type-System MUSS langfristig erweiterbar sein, ohne Anwendungen an konkrete interne Speicherrepräsentationen zu koppeln.

## Konsequenzen

### Positive Konsequenzen

- deutlich stärkere Typsicherheit
- weniger Unit-Fehler
- weniger Type Confusion
- präzisere Capability Contracts
- bessere automatische Providerwahl
- bessere automatische Algorithmuswahl
- bessere Zero-Copy-Planung
- weniger unnötige Datenkonvertierungen
- präzisere Datenpipelines
- bessere Cross-Language-Kompatibilität
- bessere langfristige Datenevolution
- bessere Architecture Introspection
- präzisere Debugging-Informationen
- bessere wissenschaftliche und technische APIs
- bessere Unterstützung medizinischer Daten
- bessere Information-Flow-Entscheidungen
- bessere Resource Estimation
- klarere Trennung von Bedeutung und Repräsentation
- stabilere APIs gegenüber Hardware- und Implementierungsänderungen
- bessere Grundlage für Capability-driven UI
- besseres automatisches Pipelining

### Negative Konsequenzen

- erheblich komplexeres Typsystem
- Registry- und Schema-Infrastruktur erforderlich
- Versionierung muss langfristig gepflegt werden
- Conversion Graph kann sehr groß werden
- Type Negotiation erhöht Planungsaufwand
- Cross-Language-Bindings werden komplexer
- falsche oder manipulierte Type Metadata erzeugt neue Security-Risiken
- Runtime-Validierung kann zusätzliche Kosten verursachen
- sehr präzise Typen können APIs unnötig kompliziert machen, wenn sie übertrieben eingesetzt werden
- Type Migration und Schema Evolution benötigen klare Regeln
- an Legacy-Grenzen gehen teilweise semantische Informationen verloren

## Verworfene Alternativen

### Ausschließlich primitive Typen

Nicht übernommen.

Ein System, das überwiegend mit:

```text
int
float
byte[]
void*
```

arbeitet, kann wichtige semantische Unterschiede nicht ausdrücken.

### Ausschließlich strukturelles Typing

Nicht übernommen.

Strukturelle Gleichheit reicht nicht aus, um fachliche Semantik zu bestimmen.

Beispiel:

```text
Latitude<Float64>
Longitude<Float64>
```

dürfen trotz gleicher Struktur nicht beliebig vertauscht werden.

### Dateiformate als Typsystem

Nicht übernommen.

Formate wie:

```text
PNG
JPEG
DICOM
JSON
```

beschreiben Kodierungen beziehungsweise Container.

Sie sind nicht ausreichend als vollständiges systemweites Semantic-Type-Modell.

### MIME als natives Typsystem

Nicht übernommen.

MIME bleibt für Web- und Kompatibilitätszwecke relevant, ist jedoch für NovaOS-Semantik zu grob.

### Typinformationen ausschließlich in Anwendungen

Nicht übernommen.

Wenn der Kernel, Capability Runtime, Resource Planner und IPC keinerlei Typsemantik kennen, können systemweite Optimierungen und Sicherheitsprüfungen nicht zuverlässig durchgeführt werden.

Der Kernel selbst bleibt dennoch typminimal.

### Vollständige Type Reflection im Kernel

Nicht übernommen.

Tausende fachliche Schemas im Kernel würden:

- Kernelgröße erhöhen,
- Angriffsfläche vergrößern,
- Evolvierbarkeit reduzieren.

Der Kernel transportiert und schützt Type Identity.

Komplexe Semantik liegt in höheren Systemschichten.

### Automatische implizite Konvertierung zwischen allen kompatiblen Typen

Nicht übernommen.

Implizite Konvertierungen können:

- Datenverlust,
- Precision Loss,
- unerwartete Kosten

verbergen.

Automatische Conversion ist nur über bekannte Conversion Paths und Policies erlaubt.

### KI als primäre Typbestimmung

Nicht übernommen.

KI kann Type Detection unterstützen.

Fundamentale Security-, Trust- oder Correctness-Entscheidungen dürfen jedoch nicht ausschließlich auf probabilistischer Type Inference beruhen.

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starken_Isolationsgrenzen
- ADR-ARCH-0002_Mechanism_Policy_Separation
- ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage
- ADR-ARCH-0004_Transaktionale_Systemoperationen
- ADR-ARCH-0005_Systemweite_Ressourcenökonomie
- ADR-ARCH-0006_Zero_Copy_als_Architekturprinzip
- ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern
- ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining
- ADR-ARCH-0010_Structured_Concurrency_als_systemweites_Ausführungsmodell
- ADR-ARCH-0011_Deterministic_Mode
- ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution
- ADR-ARCH-0013_Architecture_Introspection
- ADR-EXECUTION-0001_Nova_ExecutionContract
- ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten
- ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard
- ADR-DATA-0001_Einheitliches_Nova_Datenobjektmodell
- ADR-IPC-0001_Typed_IPC
- ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell
- ADR-TRUST-0001_Nova_Trust_als_systemweite_Vertrauensarchitektur

## Zugehörige NPSPECs

- NPSPEC-ARCH-SEMANTICTYPES-0001
- NPSPEC-TYPE-MODEL-0001
- NPSPEC-TYPE-IDENTITY-0001
- NPSPEC-TYPE-NAMESPACE-0001
- NPSPEC-TYPE-VERSIONING-0001
- NPSPEC-TYPE-PARAMETER-0001
- NPSPEC-TYPE-GENERIC-0001
- NPSPEC-TYPE-STRUCTURAL-0001
- NPSPEC-TYPE-NOMINAL-0001
- NPSPEC-TYPE-SUBTYPING-0001
- NPSPEC-TYPE-TRAIT-0001
- NPSPEC-TYPE-REFINEMENT-0001
- NPSPEC-TYPE-CONSTRAINT-0001
- NPSPEC-TYPE-VALIDATION-0001
- NPSPEC-TYPE-PROVENANCE-0001
- NPSPEC-TYPE-REGISTRY-0001
- NPSPEC-TYPE-SCHEMA-0001
- NPSPEC-TYPE-SCHEMA-EVOLUTION-0001
- NPSPEC-TYPE-COMPATIBILITY-0001
- NPSPEC-TYPE-NEGOTIATION-0001
- NPSPEC-TYPE-CONVERSION-0001
- NPSPEC-TYPE-CONVERSION-GRAPH-0001
- NPSPEC-TYPE-CONVERSION-LOSS-0001
- NPSPEC-TYPE-UNIT-0001
- NPSPEC-TYPE-DIMENSION-0001
- NPSPEC-TYPE-TIME-0001
- NPSPEC-TYPE-IDENTIFIER-0001
- NPSPEC-TYPE-IMAGE-0001
- NPSPEC-TYPE-AUDIO-0001
- NPSPEC-TYPE-VIDEO-0001
- NPSPEC-TYPE-TENSOR-0001
- NPSPEC-TYPE-MEDICAL-0001
- NPSPEC-TYPE-GEOSPATIAL-0001
- NPSPEC-TYPE-STREAM-0001
- NPSPEC-TYPE-EVENT-0001
- NPSPEC-TYPE-ERROR-0001
- NPSPEC-TYPE-SERIALIZATION-0001
- NPSPEC-TYPE-REFLECTION-0001
- NPSPEC-TYPE-SECURITY-0001
- NPSPEC-TYPE-INTROSPECTION-0001
- NPSPEC-IPC-TYPED-0001
- NPSPEC-IPC-SCHEMA-0001
- NPSPEC-DATA-SEMANTICVIEW-0001
- NPSPEC-DATA-PIPELINE-0001
- NPSPEC-EXECUTION-CONTRACT-0001
- NPSPEC-CAPABILITY-TYPECONTRACT-0001
- NPSPEC-CAPABILITY-RESOLUTION-0001
- NPSPEC-RESOURCE-TYPEESTIMATION-0001
- NPSPEC-NOVALANG-SEMANTICTYPES-0001

## Verifikation und Tests

Die Umsetzung dieser ADR MUSS durch systematische Typ-, Kompatibilitäts-, Conversion- und Security-Tests überprüft werden.

### Primitive-Type-Tests

Zu prüfen sind:

- Integergrößen
- Signedness
- Floating-Point-Repräsentationen
- Overflow-Semantik
- Alignment
- Endianness

### Unit-Tests

Zu prüfen sind:

```text
Meter + Meter
Meter → Kilometer
Celsius → Fahrenheit
Meter + Second = Error
```

### Dimensional-Analysis-Tests

Zu prüfen sind:

```text
Distance / Time = Velocity
Velocity / Time = Acceleration
Mass × Acceleration = Force
```

### Semantic-Identity-Tests

Zwei Typen mit identischer Speicherrepräsentation müssen getrennt bleiben.

Beispiel:

```text
UserId
ProcessId
```

dürfen nicht implizit kompatibel sein.

### Subtyping-Tests

Zu prüfen sind:

```text
CTImage → MedicalImage → Image
```

sowie ungültige Gegenrichtungen.

### Constraint-Tests

Zu prüfen sind beispielsweise:

```text
Probability < 0
Probability > 1
Invalid Latitude
Invalid Temperature Constraint
```

### Type-Validation-Tests

Zu unterscheiden sind:

```text
Declared
Validated
Untrusted
Invalid
```

### Registry-Tests

Zu prüfen sind:

- Type Registration
- Namespace Ownership
- Version Lookup
- Schema Lookup
- Unauthorized Registration
- Type Squatting
- Registry Cache Invalidation

### Versionierungs-Tests

Zu prüfen sind:

- gleiche Version
- backward-compatible Version
- forward-compatible Version
- breaking Version
- unbekannte Version

### Conversion-Tests

Zu prüfen sind:

- lossless Conversion
- lossy Conversion
- Conversion Forbidden
- Precision Loss
- Conversion Chain
- fehlender Converter
- Conversion Cycle

### Unit-Conversion-Tests

Mindestens zu prüfen:

```text
Meter ↔ Kilometer
Celsius ↔ Fahrenheit
Second ↔ Millisecond
```

### Image-Type-Tests

Zu prüfen sind:

- Pixel Format
- Color Space
- Dimensions
- Stride
- compatible View
- incompatible View

### Tensor-Type-Tests

Zu prüfen sind:

- Element Type
- Shape
- Layout
- Dynamic Shape
- Precision
- Conversion

### Typed-IPC-Tests

Zu prüfen sind:

- korrekter Type
- falscher Type
- falsche Version
- ungültiges Schema
- Zero-Copy Buffer mit Type Metadata
- Remote Typed IPC

### Capability-Tests

Zu prüfen sind:

```text
Input Type accepted
Input Type rejected
Subtype accepted
Converter inserted
Lossy converter rejected
```

### Provider-Selection-Tests

Der Planner muss Typkompatibilität vor Performanceoptimierung prüfen.

Beispiel:

```text
Provider A:
    1 ms
    incompatible

Provider B:
    5 ms
    compatible

Selected:
    Provider B
```

### Zero-Copy-Tests

Zu prüfen sind:

```text
same semantic type
same representation
same layout
```

führt bei geeigneter Hardware zu einem direkten Shared-Buffer-Pfad.

Bei inkompatibler Representation muss Conversion beziehungsweise Copy korrekt sichtbar sein.

### Resource-Tests

Type-basierte Größenberechnung muss beispielsweise für feste Tensor Shapes korrekt funktionieren.

### Security-Tests

Zu prüfen sind:

- Type Confusion
- forged Type ID
- forged validation state
- forged schema
- unauthorized registry modification
- unsafe cast
- malicious converter
- untrusted Type Claim

### Persistence-Tests

Zu prüfen sind:

- Type Identity bleibt nach Neustart erhalten
- Version wird korrekt gelesen
- alte Schema-Version
- Migration
- unbekannter Type

### Hot-Replacement-Tests

Zu prüfen sind:

- kompatibler Provider
- inkompatibler Provider
- Conversion erforderlich
- State-Type-Migration
- Rollback bei Type-Mismatch

### Determinism-Tests

Zu prüfen sind:

- Float-Rundung
- Unit Conversion
- Unicode Normalization
- Time Conversion
- Locale-Unabhängigkeit
- deterministische Serialization

### Introspection-Tests

Für ein typisiertes Objekt müssen mindestens sichtbar sein können:

```text
Type Identity
Version
Representation
Relevant Parameters
Validation State
```

### Fault-Injection

Fault-Injection SOLL mindestens folgende Fälle simulieren:

```text
Corrupted Type ID
Missing Schema
Registry unavailable
Unknown Type
Invalid Version
Converter crash
Validation failure
Malformed payload
```

Das System darf dadurch keine unkontrollierte Type Confusion erzeugen.

## Ergebnis

NovaOS behandelt Daten nicht mehr ausschließlich als primitive Speicherwerte.

Das grundlegende Modell lautet:

```text
Physical Representation
        │
        ▼
Structural Type
        │
        ▼
Semantic Type
        │
        ├── Units
        ├── Dimensions
        ├── Shape
        ├── Format
        ├── Constraints
        ├── Traits
        └── Version
        │
        ▼
Capability Contract
        │
        ▼
Execution Planning
```

Damit kann NovaOS beispielsweise unterscheiden zwischen:

```text
Float32
```

und:

```text
Temperature<Celsius>
```

zwischen:

```text
UInt8[]
```

und:

```text
Image<RGBA8>
```

sowie zwischen:

```text
Tensor<Float32>
```

und:

```text
ImageTensor<
    Float32,
    NCHW,
    RGB,
    ZeroToOne
>
```

Semantic Types bilden dadurch die gemeinsame semantische Sprache zwischen:

```text
NovaLang
Capabilities
Execution Contracts
Data Objects
IPC
Resource Economy
Zero-Copy
Location Transparency
Security
Trust
Storage
Compute
AI
Architecture Introspection
```

Das verbindliche Architekturprinzip lautet:

```text
Do not describe only
how data is represented.

Describe what the data means.
```

Damit wird das Semantic-Type-System zu einer zentralen Grundlage für die native NovaOS-Architektur und ermöglicht typisierte, adaptive, sichere und langfristig evolvierbare Systemkomposition.