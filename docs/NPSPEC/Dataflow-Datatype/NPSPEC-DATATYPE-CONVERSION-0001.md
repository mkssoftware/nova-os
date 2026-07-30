# NPSPEC-DATATYPE-CONVERSION-0001: Typkonvertierung

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Dokument-ID:** NPSPEC-DATATYPE-CONVERSION-0001
- **Titel:** Typkonvertierung
- **Version:** 1.0.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture / Datatype System
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform

## Referenzen

- NPSPEC-DATATYPE-0001
- NPSPEC-DATATYPE-PRIMITIVE-0001
- NPSPEC-DATATYPE-SEMANTIC-0001
- NPSPEC-DATATYPE-COMPOSITE-0001
- NPSPEC-DATATYPE-SCHEMA-0001
- NPSPEC-DATAFLOW-0001
- NPSPEC-CAPABILITY-CONTRACT-0001
- NPSPEC-CAPABILITY-DISCOVERY-0001
- ADR-DATAFLOW-0002
- ADR-DATAFLOW-0003
- ADR-DATAFLOW-0005
- ADR-DATAFLOW-0006

---

# 1. Zweck

Diese Spezifikation definiert das universelle Modell der Typkonvertierung innerhalb der Nova Platform.

Sie beschreibt die Regeln für die Umwandlung zwischen:

- primitiven Datentypen
- semantischen Datentypen
- zusammengesetzten Datentypen
- Dokumenttypen
- strukturierten Daten
- Streams
- Capability-Daten
- Serialisierungen

Typkonvertierungen erfolgen niemals implizit aufgrund zufälliger Implementierungsdetails, sondern ausschließlich anhand expliziter Konvertierungsregeln und registrierter Converter-Capabilities. Explizite Typdefinitionen und nachvollziehbare Konvertierungen erhöhen Vorhersagbarkeit und Wartbarkeit komplexer Typsysteme.

---

# 2. Ziele

Das Konvertierungsmodell verfolgt folgende Ziele:

- deterministische Konvertierungen
- verlustfreie Transformationen bevorzugen
- vollständige Typtransparenz
- semantische Konsistenz
- sichere automatische Konvertierungen
- explizite verlustbehaftete Konvertierungen
- reproduzierbare Ergebnisse
- Capability-basierte Erweiterbarkeit
- vollständige Reflection
- Versionierung
- Performance
- Zero-Copy-Unterstützung

---

# 3. Grundprinzipien

Die Typkonvertierung basiert auf folgenden Regeln:

- Jeder Konvertierungspfad ist eindeutig definiert.
- Implizite Konvertierungen sind stark eingeschränkt.
- Semantik besitzt Vorrang vor technischer Repräsentation.
- Verlustfreie Konvertierungen werden bevorzugt.
- Verlustbehaftete Konvertierungen müssen explizit gekennzeichnet sein.
- Jede Konvertierung besitzt eine Qualitätsklassifikation.
- Jede Konvertierung besitzt Provenance.
- Jede Konvertierung ist versionierbar.
- Jede Konvertierung ist testbar.

---

# 4. Architektur

```text
Source Object
↓
Schema Validation
↓
Converter Discovery
↓
Compatibility Check
↓
Converter Capability
↓
Validation
↓
Target Object
```

---

# 5. Konvertierungsdefinition

Eine Typkonvertierung beschreibt die Transformation eines Quelltyps in einen Zieltyp.

Formal:

```text
Converter<SourceType, TargetType>
```

Beispiele:

```text
UInt32 → UInt64

String → EmailAddress

MarkdownDocument → HTMLDocument

CSV → Table

Image → Tensor

AudioStream → Spectrogram
```

---

# 6. Typen von Konvertierungen

Nova unterscheidet:

```text
Identity Conversion

Implicit Conversion

Explicit Conversion

Semantic Conversion

Structural Conversion

Serialization Conversion

Migration Conversion

Adapter Conversion
```

---

# 7. Identity Conversion

Quelle und Ziel sind identisch.

```text
String
↓
String
```

Keine Datenänderung.

---

# 8. Implizite Konvertierung

Implizite Konvertierungen dürfen ausschließlich erfolgen, wenn:

- keine Information verloren geht,
- Semantik erhalten bleibt,
- keine Mehrdeutigkeit entsteht,
- der Capability Contract dies erlaubt.

Beispiel:

```text
UInt8
↓
UInt16
```

---

# 9. Explizite Konvertierung

Explizite Konvertierungen benötigen eine bewusste Entscheidung.

Beispiel:

```text
Float64
↓
Int32
```

Diese Konvertierung kann Nachkommastellen verlieren.

---

# 10. Semantische Konvertierung

Semantische Konvertierungen verändern nicht nur den technischen Typ.

Beispiel:

```text
String
↓
EmailAddress
```

Hier erfolgt zusätzlich eine semantische Validierung.

---

# 11. Strukturelle Konvertierung

Strukturelle Konvertierungen verändern den Aufbau eines Objekts.

Beispiele:

```text
Record
↓
Table Row
```

```text
JSON
↓
Object
```

---

# 12. Serialisierungskonvertierung

Diese Konvertierung verändert ausschließlich die Darstellung.

Beispiele:

```text
Object
↓
JSON
```

```text
Object
↓
CBOR
```

Die logische Semantik bleibt unverändert.

---

# 13. Migrationskonvertierung

Migrationen dienen der Schemaevolution.

Beispiel:

```text
Person@1
↓
Person@2
```

Migrationen besitzen eigene Versionen.

---

# 14. Adapterkonvertierung

Adapter erzeugen kompatible Ansichten.

Beispiel:

```text
Person@3
↓
Adapter
↓
Person@1 View
```

Die Quelldaten bleiben unverändert.

---

# 15. Qualitätsklassen

Nova definiert:

```text
Identity

Lossless

Semantically Lossless

Visually Lossless

Potentially Lossy

Lossy

Irreversible
```

---

# 16. Identity

Quell- und Zielwert sind identisch.

---

# 17. Lossless

Alle Informationen bleiben erhalten.

---

# 18. Semantically Lossless

Darstellung kann sich ändern.

Semantik bleibt vollständig erhalten.

---

# 19. Visually Lossless

Die visuelle Darstellung bleibt erhalten.

Interne Struktur kann sich ändern.

---

# 20. Potentially Lossy

Informationsverlust hängt von den Eingabedaten ab.

---

# 21. Lossy

Bekannter Informationsverlust.

---

# 22. Irreversible

Eine Rückkonvertierung ist grundsätzlich nicht möglich.

---

# 23. Automatische Konvertierungen

Automatisch erlaubt sind beispielsweise:

```text
UInt8
↓
UInt16
```

```text
Int16
↓
Int32
```

Nicht automatisch erlaubt:

```text
Float64
↓
Int32
```

---

# 24. Numerische Konvertierungen

Unterstützt werden:

```text
Integer

Float

Decimal

Complex

Rational
```

Geprüft werden:

- Überlauf
- Unterlauf
- Genauigkeit
- Rundung
- Vorzeichen

---

# 25. Zeichenkettenkonvertierungen

Unterstützt werden:

```text
String
↓
Number
↓
Boolean
↓
Date
↓
UUID
```

Diese Konvertierungen benötigen Validatoren.

---

# 26. Enum-Konvertierungen

Erlaubt sind:

```text
Enum
↓
String
```

und

```text
String
↓
Enum
```

Der Zielwert muss gültig sein.

---

# 27. Collection-Konvertierungen

Beispiele:

```text
Array
↓
List
```

```text
List
↓
Set
```

Bei Mengen müssen Duplikate behandelt werden.

---

# 28. Dokumentkonvertierungen

Beispiele:

```text
Markdown
↓
HTML
```

```text
Markdown
↓
PDF
```

Diese Konvertierungen erfolgen ausschließlich über registrierte Converter-Capabilities.

---

# 29. Datenbankkonvertierungen

Beispiele:

```text
CSV
↓
Table
```

```text
SQL Result
↓
DataFrame
```

---

# 30. Streamkonvertierungen

Beispiele:

```text
Video Stream
↓
Frame Stream
```

```text
Measurement Stream
↓
Window Stream
```

---

# 31. Medienkonvertierungen

Beispiele:

```text
PNG
↓
JPEG
```

```text
RAW
↓
PNG
```

---

# 32. KI-Konvertierungen

Beispiele:

```text
Image
↓
Embedding
```

```text
Text
↓
Embedding
```

```text
Audio
↓
Transcript
```

---

# 33. Robotik

Beispiele:

```text
GPS
↓
Map Position
```

```text
Lidar
↓
Point Cloud
```

---

# 34. Geodaten

Konvertierungen unterstützen:

- CRS-Wechsel
- Projektionen
- Höhenreferenzen
- Einheiten

---

# 35. Einheitenkonvertierungen

Beispiele:

```text
m
↓
cm
```

```text
°C
↓
K
```

Einheiten werden über NPSPEC-COMPUTE-UNIT validiert.

---

# 36. Dimensionale Konvertierung

Konvertierungen dürfen Dimensionsregeln nicht verletzen.

Beispiel:

```text
Length
↓
Time
```

ist ungültig.

---

# 37. Semantische Kompatibilität

Vor jeder Konvertierung wird geprüft:

- Type ID
- Semantic Type
- Schema
- Version
- Constraints

---

# 38. Converter Capability

Jede komplexe Konvertierung wird als Capability bereitgestellt.

Eigenschaften:

```text
Converter ID

Version

Input

Output

Quality

Performance
```

---

# 39. Converter Discovery

Die Runtime sucht Converter anhand von:

```text
Input Type

Output Type

Quality

Version

Security
```

---

# 40. Mehrstufige Konvertierungen

Ist keine direkte Konvertierung vorhanden, darf die Runtime mehrere Converter kombinieren.

Beispiel:

```text
Markdown
↓
HTML
↓
PDF
```

Mehrstufige Konvertierungen müssen vollständig dokumentiert werden. Mehrstufige Konvertierungspfade und deren Qualität sollten nachvollziehbar dokumentiert werden, da Zwischenschritte Informationsverluste verursachen können.

---

# 41. Konvertierungsgraph

Alle Converter bilden einen gerichteten Graphen.

```text
Type A
↓
Type B
↓
Type C
```

Die Runtime berechnet den optimalen Pfad.

---

# 42. Kostenmodell

Ein Converter besitzt Kosten.

Beispiele:

```text
CPU

Memory

Latency

Energy
```

Diese fließen in die Pfadwahl ein.

---

# 43. Qualitätsmodell

Bei mehreren Pfaden bevorzugt Nova:

1. höchste Qualität
2. geringster Informationsverlust
3. geringste Kosten

---

# 44. Konvertierungsbericht

Jede Konvertierung kann einen Bericht erzeugen.

Er enthält:

```text
Source Type

Target Type

Converter

Warnings

Quality

Duration
```

---

# 45. Fehler

Standardisierte Fehler:

```text
CONVERTER_NOT_FOUND

TYPE_INCOMPATIBLE

SEMANTIC_MISMATCH

PRECISION_LOSS

OVERFLOW

UNDERFLOW

INVALID_FORMAT

SCHEMA_MISMATCH
```

---

# 46. Provenance

Jede Konvertierung dokumentiert:

- Quelle
- Ziel
- Converter
- Version
- Zeit
- Parameter

---

# 47. Reflection

Reflection liefert:

```text
Input Type

Output Type

Quality

Version

Supported Features
```

---

# 48. Sicherheit

Converter besitzen:

- Berechtigungen
- Sandbox
- Vertrauensstufe
- Signatur

---

# 49. Datenschutz

Konvertierungen dürfen Datenschutzklassifikationen nicht entfernen.

---

# 50. YAML-Beispiel

```yaml
converter:
  id: nova.converter.markdown.html
  version: 1.0.0
  input:
    type: MarkdownDocument
  output:
    type: HTMLDocument
  quality:
    class: semantically-lossless
  security:
    trusted: true
```

---

# 51. Validierungsregeln

Eine Konvertierung ist gültig, wenn:

- ein Converter existiert,
- Typen kompatibel sind,
- Schemas kompatibel sind,
- Constraints erfüllt sind,
- Sicherheitsrichtlinien eingehalten werden,
- Qualitätsklasse definiert ist.

---

# 52. Laufzeitanforderungen

Die Runtime muss:

- Converter finden,
- Konvertierung validieren,
- Provenance erzeugen,
- Fehler protokollieren,
- Qualitätsberichte erzeugen,
- mehrstufige Pfade unterstützen.

---

# 53. Compileranforderungen

Der Compiler muss:

- implizite Konvertierungen prüfen,
- explizite Konvertierungen erzwingen,
- Typinkompatibilitäten erkennen,
- sichere Konvertierungen optimieren.

---

# 54. Testanforderungen

Jeder Converter benötigt Tests für:

- gültige Eingaben
- ungültige Eingaben
- Grenzwerte
- Informationsverlust
- Roundtrip
- Performance
- Fehlerbehandlung

---

# 55. Roundtrip

Unterstützte Konvertierungen sollen getestet werden:

```text
A
↓
B
↓
A
```

Ergebnis:

```text
Identical

Semantically Equivalent

Lossy
```

---

# 56. Qualitätsanforderungen

Das Konvertierungssystem muss:

- deterministisch sein,
- typisiert sein,
- reproduzierbar sein,
- versionierbar sein,
- erweiterbar sein,
- sicher sein,
- performant sein,
- vollständig dokumentierbar sein.

---

# 57. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation, wenn:

- alle Konvertierungen typisiert sind,
- Converter registriert werden,
- Qualitätsklassen unterstützt werden,
- Provenance erhalten bleibt,
- mehrstufige Konvertierungen möglich sind,
- Reflection verfügbar ist,
- Capability Discovery integriert ist,
- implizite Konvertierungen kontrolliert erfolgen,
- verlustbehaftete Konvertierungen explizit gekennzeichnet werden.

---

# 58. Zusammenfassung

Die **Typkonvertierung** definiert das universelle Konvertierungsmodell der Nova Platform.

Alle Konvertierungen erfolgen über klar definierte, versionierte und registrierte Converter-Capabilities. Die Trennung zwischen technischen, semantischen und strukturellen Konvertierungen verhindert unkontrollierte Datenverluste und gewährleistet reproduzierbare Ergebnisse.

Durch Qualitätsklassifikationen, Konvertierungsgraphen, Provenance, Schema-Validierung und Capability Discovery entsteht ein flexibles, sicheres und langfristig wartbares System für sämtliche Daten- und Typtransformationen innerhalb der Nova Platform.
