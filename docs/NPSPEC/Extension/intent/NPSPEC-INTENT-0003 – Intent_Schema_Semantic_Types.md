# NPSPEC-INTENT-0003 – Intent Schema & Semantic Types

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das Schema-System und die semantischen Typen von `Nova.Intent`.

Ziel ist, dass NovaOS nicht nur technische Datentypen wie:

```text
string
integer
float
byte[]
```

versteht, sondern die Bedeutung von Daten und Aufgaben erkennen kann.

Beispiele:

```text
Media.Audio
Document.Transcript
Image.Photo
Measurement.Temperature
Person.Contact
Location.Place
```

Semantische Typen bilden die gemeinsame Sprache zwischen:

- Intents
- Capabilities
- Nova.Synthesis
- Execution Contracts
- Nova Execution IR
- Datenobjekten
- Nova.Causality
- Nova.InformationFlow
- Nova.Evidence

## Grundprinzip

Ein Intent besitzt einen definierten semantischen Typ.

Beispiel:

```text
media.audio.transcribe
```

Dieser Typ definiert unter anderem:

```text
Input:
    Media.Audio

Output:
    Document.Transcript
```

Damit kann NovaOS prüfen, welche Capabilities zur Erfüllung eines Intents geeignet sind.

## Intent Schema

Jeder registrierte Intent-Typ besitzt ein Schema.

Beispiel:

```text
IntentSchema {
    type
    version
    inputs
    outputs
    properties
    constraints
    policies
}
```

Beispiel:

```text
IntentSchema {
    type: media.audio.transcribe
    version: 1

    inputs {
        source: Media.Audio
    }

    outputs {
        result: Document.Transcript
    }

    properties {
        language: Language.Code?
        timestamps: Boolean?
    }
}
```

`?` kennzeichnet ein optionales Feld.

## Schema-Aufgabe

Ein Intent-Schema beschreibt:

- erforderliche Inputs
- optionale Inputs
- erwartete Outputs
- erlaubte Properties
- semantische Typen
- Standardwerte
- gültige Constraints
- unterstützte Policies
- Schema-Version

Das Schema beschreibt nicht den konkreten Algorithmus zur Ausführung.

## Semantic Type

Ein Semantic Type beschreibt die Bedeutung eines Wertes oder Objekts.

Grundstruktur:

```text
SemanticType {
    id
    version
    parent
    representation
    properties
}
```

Beispiel:

```text
SemanticType {
    id: Measurement.Temperature
    version: 1
    parent: Measurement
    representation: Float64
}
```

Der semantische Typ ist von seiner technischen Speicherrepräsentation getrennt.

Beispiel:

```text
Semantic Type:
    Measurement.Temperature

Representation:
    Float64
```

Dadurch können zwei technisch identische Werte unterschiedliche Bedeutung besitzen.

Beispiel:

```text
20.0 °C
20.0 m
```

Beide können intern als `Float64` gespeichert sein, sind semantisch jedoch inkompatibel.

## Typnamen

Semantische Typen verwenden hierarchische Namen.

Beispiele:

```text
Media
Media.Audio
Media.Audio.Stream
Media.Video

Document
Document.Text
Document.Transcript
Document.Report

Image
Image.Photo
Image.Diagram

Measurement
Measurement.Temperature
Measurement.Length

System
System.Configuration
System.Log
```

Die Namenshierarchie dient der semantischen Organisation.

Sie stellt nicht automatisch eine vollständige Vererbungshierarchie dar.

## Typidentität

Jeder semantische Typ besitzt eine stabile Identität.

Beispiel:

```text
semantic:Measurement.Temperature@1
```

Typname und Version müssen gemeinsam eindeutig auflösbar sein.

Eine Umbenennung darf nicht stillschweigend die Identität eines bestehenden Typs verändern.

## Basistypen

NovaOS soll eine kleine Menge grundlegender semantischer Basistypen besitzen.

Beispiele:

```text
Any
Boolean
Number
Integer
Decimal
Text
Binary
Time
Duration
Identifier
Collection
Object
Stream
```

Domänenspezifische Typen bauen darauf auf.

Beispiel:

```text
Measurement.Temperature
    ↓
Number
```

## Typbeziehungen

Semantic Types können Beziehungen besitzen.

Mindestens folgende Beziehungen müssen darstellbar sein:

```text
IS_A
COMPATIBLE_WITH
CONVERTIBLE_TO
CONTAINS
DERIVED_FROM
```

Beispiel:

```text
Media.Audio.Stream
    IS_A
Media.Audio
```

oder:

```text
Image.Raw
    CONVERTIBLE_TO
Image.Bitmap
```

Diese Beziehungen können später durch `Nova.CompatibilityGraph` verwendet werden.

## Typkompatibilität

NovaOS muss zwischen mehreren Formen der Kompatibilität unterscheiden.

```text
exact
compatible
convertible
incompatible
```

Beispiel:

```text
Input required:
    Media.Audio

Provided:
    Media.Audio.Stream

Result:
    compatible
```

Beispiel:

```text
Input required:
    Document.Text

Provided:
    Media.Audio

Result:
    incompatible
```

Sofern eine gültige Transformation existiert:

```text
Media.Audio
    → SpeechRecognition
    → Document.Transcript
```

kann `Nova.Synthesis` daraus einen Ausführungspfad erzeugen.

## Eigenschaften

Semantic Types dürfen typisierte Eigenschaften besitzen.

Beispiel:

```text
Media.Audio {
    sample_rate: Frequency
    channels: Integer
    duration: Duration
    encoding: Media.Audio.Encoding
}
```

Eigenschaften können:

- erforderlich
- optional
- readonly
- berechnet
- vererbt

sein.

## Constraints auf Typen

Semantic Types dürfen zusätzliche Gültigkeitsbedingungen definieren.

Beispiel:

```text
Measurement.Probability {
    representation: Float64

    constraint:
        value >= 0.0
        value <= 1.0
}
```

Oder:

```text
Media.Audio.SampleRate {
    unit: Hz
    value > 0
}
```

Diese Constraints gehören zur Bedeutung des Typs.

## Einheiten

Physikalische Größen müssen ihre Einheit semantisch ausdrücken können.

Beispiel:

```text
Measurement.Length {
    value: 10
    unit: meter
}
```

oder:

```text
Measurement.Temperature {
    value: 23.7
    unit: celsius
}
```

Einheiten dürfen nicht ausschließlich als freier Text gespeichert werden.

NovaOS muss Einheiten prüfen und gegebenenfalls sicher konvertieren können.

## Dimensionen

Physikalische Typen sollen eine Dimensionsbeschreibung besitzen können.

Beispiel:

```text
Length:
    L

Velocity:
    L / T

Acceleration:
    L / T²
```

Dadurch können mathematisch ungültige Operationen erkannt werden.

Beispiel:

```text
Length + Temperature
```

muss ohne explizit definierte Semantik abgelehnt werden.

## Nullable und Optional

`null` und „nicht angegeben“ müssen unterschieden werden.

Beispiel:

```text
language:
    optional
```

bedeutet:

```text
kein Wert angegeben
```

während:

```text
language: null
```

einen explizit unbekannten oder nicht vorhandenen Wert darstellen kann.

Die genaue Bedeutung wird durch das jeweilige Schema definiert.

## Collections

Semantic Types müssen Mengen und Sequenzen ausdrücken können.

Beispiele:

```text
List<Image.Photo>

Set<Person.Contact>

Stream<Media.Audio.Frame>
```

Collections behalten die Semantik ihres Elementtyps.

## Union Types

Ein Feld darf mehrere semantisch zulässige Typen akzeptieren.

Beispiel:

```text
source:
    Media.Audio
    | Media.Video
```

Dadurch kann beispielsweise eine Transkriptions-Capability Audio direkt oder Audio aus Video verarbeiten.

## Result Types

Intent-Schemas müssen mindestens einen möglichen Result Type definieren können.

Beispiel:

```text
outputs {
    transcript: Document.Transcript
}
```

Mehrere Ergebnisse sind zulässig.

Beispiel:

```text
outputs {
    transcript: Document.Transcript
    subtitles: Media.Subtitles
    metadata: Media.Metadata
}
```

## Intent Properties

Zusätzliche Intent-Parameter müssen ebenfalls semantisch typisiert werden.

Beispiel:

```text
properties {
    language: Language.Code
    speaker_detection: Boolean
    max_speakers: Integer
}
```

Ungültige Parameter müssen bereits vor der Ausführungsplanung erkannt werden können.

## Standardwerte

Intent-Schemas dürfen Standardwerte definieren.

Beispiel:

```text
properties {
    timestamps {
        type: Boolean
        default: false
    }
}
```

Standardwerte dürfen keine Sicherheits- oder Datenschutzregeln umgehen.

## Schema-Versionierung

Jedes Intent-Schema und jeder Semantic Type muss versioniert sein.

Beispiel:

```text
media.audio.transcribe@1
media.audio.transcribe@2
```

Neue Versionen dürfen:

- zusätzliche optionale Felder hinzufügen
- neue kompatible Typen zulassen
- neue Metadaten definieren

Breaking Changes benötigen eine neue inkompatible Version.

## Schema-Kompatibilität

NovaOS muss Schema-Kompatibilität prüfen können.

Mindestens folgende Fälle sind zu unterscheiden:

```text
compatible
requires_migration
requires_adapter
incompatible
```

Eine Schema-Migration darf nicht implizit Datenbedeutung verändern.

## Schema Registry

NovaOS benötigt eine systemweite Registry für:

- Intent-Schemas
- Semantic Types
- Versionen
- Beziehungen
- Konvertierungen
- Namespaces

Logisches Beispiel:

```text
Nova.Semantics.Registry
```

Die konkrete Implementierung ist nicht Bestandteil dieser NPSPEC.

## Namespaces

Semantic Types und Intent-Schemas müssen Namespaces unterstützen.

Beispiele:

```text
nova.media.audio
nova.document
vendor.example.sensor
project.novaos.custom
```

Systemeigene Namespaces müssen von Drittanbieter-Namespaces unterscheidbar sein.

## Erweiterbarkeit

Drittanbieter und Anwendungen dürfen neue semantische Typen registrieren.

Neue Typen dürfen bestehende Systemtypen nicht überschreiben.

Beispiel:

```text
vendor.acme.LidarPointCloud
```

Ein neuer Typ kann Beziehungen zu bestehenden Typen definieren.

Beispiel:

```text
vendor.acme.LidarPointCloud
    IS_A
Spatial.PointCloud
```

## Validierung

Die Validierung eines Intents erfolgt mindestens gegen:

```text
Intent Schema
    +
Semantic Types
    +
Type Constraints
    +
Schema Version
```

Beispiel:

```text
Intent:
    media.audio.transcribe

Input:
    Document.PDF
```

Ergebnis:

```text
SchemaValidationError:
    expected: Media.Audio
    received: Document.PDF
```

Die Validierung muss vor der eigentlichen Ausführung erfolgen.

## Konvertierungen

Semantic Types dürfen bekannte Konvertierungspfade besitzen.

Beispiel:

```text
Image.Raw
    → Image.Bitmap
```

Konvertierungen müssen unterscheiden zwischen:

```text
lossless
lossy
semantic_preserving
semantic_changing
```

Beispiel:

```text
Audio.PCM
    → Audio.MP3
```

kann semantisch kompatibel, aber technisch verlustbehaftet sein.

Derartige Eigenschaften müssen bei Planung und Policy-Prüfung berücksichtigt werden können.

## Semantische Transformation

Nicht jede Transformation ist eine reine Typkonvertierung.

Beispiel:

```text
Media.Audio
    → Document.Transcript
```

ist eine semantische Transformation.

Solche Transformationen werden durch Capabilities beschrieben und von `Nova.Synthesis` bzw. `Intent Resolution` verwendet.

## Unsicherheit

Semantic Types müssen mit `Nova.Uncertainty` kombinierbar sein.

Beispiel:

```text
Measurement.Temperature {
    value: 23.7
    uncertainty: ±0.4
}
```

oder logisch:

```text
Uncertain<Measurement.Temperature>
```

Unsicherheit verändert nicht die grundlegende Bedeutung des Wertes.

## Präzision

Semantic Types müssen mit `Nova.Precision` kombinierbar sein.

Beispiel:

```text
Measurement.Position {
    required_precision: 1mm
}
```

Die technische Repräsentation kann durch NovaOS dynamisch gewählt werden, solange der semantische Vertrag erfüllt bleibt.

## Information Flow

Semantic Types dürfen mit Information-Flow-Klassifikationen kombiniert werden.

Beispiel:

```text
Person.MedicalRecord

classification:
    confidential
```

Die Sicherheitsklassifikation ist jedoch nicht Bestandteil der eigentlichen Typidentität.

Sie wird durch `Nova.InformationFlow` verwaltet.

## Beispiel: Transkriptions-Intent

Schema:

```text
IntentSchema {
    type: media.audio.transcribe
    version: 1

    inputs {
        source {
            type: Media.Audio
            required: true
        }
    }

    properties {
        language {
            type: Language.Code
            required: false
        }

        timestamps {
            type: Boolean
            default: false
        }
    }

    outputs {
        result {
            type: Document.Transcript
        }
    }
}
```

Intent:

```text
Intent {
    type: media.audio.transcribe

    inputs {
        source {
            type: Media.Audio
            ref: object:9827
        }
    }

    properties {
        language: de
        timestamps: true
    }
}
```

Validierung:

```text
Intent Type:
    valid

Input Type:
    Media.Audio
    → compatible

language:
    Language.Code
    → valid

timestamps:
    Boolean
    → valid

Result:
    VALID
```

## Beispiel: Fehlerhafte Typisierung

```text
Intent {
    type: media.audio.transcribe

    inputs {
        source {
            type: Image.Photo
            ref: object:1234
        }
    }
}
```

Schema erwartet:

```text
Media.Audio
```

Ergebnis:

```text
INVALID

reason:
    semantic_type_mismatch
```

NovaOS darf anschließend prüfen, ob ein gültiger Transformationspfad existiert.

Ohne solchen Pfad darf der Intent nicht ausgeführt werden.

## Normative Anforderungen

1. Jeder registrierte Intent-Typ MUSS ein versioniertes Schema besitzen.
2. Inputs und Outputs eines Intent-Schemas MÜSSEN semantisch typisierbar sein.
3. Semantic Types MÜSSEN von ihrer technischen Speicherrepräsentation getrennt sein.
4. Typidentitäten MÜSSEN stabil und eindeutig auflösbar sein.
5. Intent-Schemas MÜSSEN vor der Ausführungsplanung validierbar sein.
6. NovaOS MUSS zwischen exakter, kompatibler, konvertierbarer und inkompatibler Typbeziehung unterscheiden können.
7. Physikalische Größen SOLLEN Einheiten und Dimensionen semantisch ausdrücken können.
8. Breaking Changes an Schemas oder Semantic Types MÜSSEN versioniert werden.
9. Drittanbieter-Typen DÜRFEN Systemtypen nicht überschreiben.
10. Konvertierungen MÜSSEN mögliche Informations- oder Qualitätsverluste beschreiben können.
11. Sicherheitsklassifikation und semantische Typidentität MÜSSEN getrennt behandelbar sein.
12. Semantic Types MÜSSEN durch andere NovaOS-Subsysteme referenzierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Intent-Schemas
- Semantic Types
- Typbeziehungen
- Typkompatibilität
- Schema-Versionierung
- grundlegende Validierung

Nicht Bestandteil sind:

- konkrete Intent-Auflösung
- Auswahl von Capabilities
- Synthesis-Planung
- Execution IR
- konkrete Serialisierungsformate
- Information-Flow-Policies
- Unsicherheitsberechnung
- dynamische Präzisionswahl

Diese Bereiche werden separat spezifiziert.

## Zugehörige NPSPECs

- `NPSPEC-INTENT-0001 – Intent Object Model`
- `NPSPEC-INTENT-0002 – Intent Lifecycle`
- `NPSPEC-INTENT-0004 – Intent Resolution`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`
- `NPSPEC-INTENT-0006 – Intent Composition`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`
- `NPSPEC-UNCERTAINTY-0001 – Uncertainty Type System`
- `NPSPEC-PRECISION-0001 – Precision Contract`
- `NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor`
- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`