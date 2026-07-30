# NPSPEC-DATATYPE-COMPOSITE-0001: Zusammengesetzte Datentypen

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:**
  - NPSPEC-DATATYPE-0001
  - NPSPEC-DATATYPE-PRIMITIVE-0001
  - NPSPEC-DATATYPE-SEMANTIC-0001
  - NPSPEC-DATAFLOW-0001
  - NPSPEC-CAPABILITY-CONTRACT-0001
  - NPSPEC-CAPABILITY-VERSIONING-0001
  - ADR-DATAFLOW-0002
  - ADR-DATAFLOW-0003
  - ADR-DATAFLOW-0006

---

# 1. Zweck

Diese Spezifikation definiert die zusammengesetzten Datentypen der Nova Platform.

Zusammengesetzte Datentypen kombinieren primitive, semantische oder weitere zusammengesetzte Datentypen zu strukturierten Datenmodellen.

Sie bilden die Grundlage für:

- Objekte
- Records
- Collections
- Capability Contracts
- Nachrichten
- Konfigurationen
- wissenschaftliche Datenstrukturen
- KI-Datenmodelle
- Netzwerkprotokolle
- persistente Datenformate

Ein zusammengesetzter Datentyp beschreibt nicht nur die enthaltenen Werte, sondern auch deren Struktur, Beziehungen, Reihenfolge und Validierungsregeln.

---

# 2. Ziele

Das Modell zusammengesetzter Datentypen verfolgt folgende Ziele:

- einheitliche strukturierte Datenmodelle
- starke Typisierung
- vollständige Reflection
- deterministische Serialisierung
- sichere Capability-Komposition
- generische Datenverarbeitung
- effiziente Speicherrepräsentation
- kontrollierte Versionierung
- plattformunabhängige Interoperabilität
- langfristige Erweiterbarkeit

---

# 3. Grundprinzipien

Zusammengesetzte Datentypen basieren auf folgenden Prinzipien:

- Jeder enthaltene Wert besitzt einen definierten Typ.
- Strukturen sind explizit und maschinenlesbar.
- Felder besitzen stabile Identitäten.
- Feldreihenfolge und Speicherlayout werden getrennt betrachtet.
- Typen sind standardmäßig unveränderlich.
- Änderungen erzeugen neue Werte oder Objektversionen.
- Verschachtelung ist zulässig.
- Zyklische Wertstrukturen sind nicht direkt zulässig.
- Referenzen können Zyklen explizit modellieren.
- Serialisierung und In-Memory-Repräsentation bleiben getrennt.
- Kompatibilität wird strukturell und nominal geprüft.
- Semantische Typinformationen bleiben erhalten.

---

# 4. Architekturübersicht

```text
Composite Type
├── Type Identity
├── Composite Kind
├── Generic Parameters
├── Members
│   ├── Field Name
│   ├── Field ID
│   ├── Field Type
│   ├── Constraints
│   └── Default Value
├── Invariants
├── Memory Layout
├── Serialization
├── Version
└── Reflection
```

---

# 5. Kategorien zusammengesetzter Typen

Nova definiert folgende grundlegende Kategorien:

```text
Array

List

Set

Map

Tuple

Record

Object

Optional

Result

Variant

Union

Enumeration

Range

Reference

Graph

Stream

Table
```

Weitere spezialisierte Typen können auf diesen Kategorien aufbauen.

---

# 6. Nominale und strukturelle Typisierung

Nova unterstützt zwei Kompatibilitätsmodelle:

```text
Nominal Typing

Structural Typing
```

Nominale Typisierung prüft die Identität eines Typs.

Beispiel:

```text
UserAddress
≠
DeliveryAddress
```

Auch wenn beide dieselben Felder besitzen, bleiben sie fachlich unterschiedliche Typen.

Strukturelle Typisierung prüft die vorhandenen Mitglieder und deren Typen.

Sie darf nur verwendet werden, wenn ein Contract dies ausdrücklich erlaubt.

---

# 7. Typidentität

Jeder veröffentlichte zusammengesetzte Typ besitzt eine stabile Type ID.

Beispiele:

```text
nova.core.person

nova.core.address

nova.media.image-metadata

nova.robotics.robot-pose

nova.compute.matrix-shape
```

Die Type ID darf nach Veröffentlichung nicht geändert oder wiederverwendet werden.

---

# 8. Mitglieder

Ein zusammengesetzter Typ besteht aus Mitgliedern.

Ein Mitglied besitzt mindestens:

```text
Field ID

Name

Type

Required

Visibility
```

Optional besitzt es:

```text
Default Value

Constraints

Semantic Role

Documentation

Deprecation Status
```

---

# 9. Feldidentität

Jedes Feld besitzt innerhalb seines Typs eine stabile Feldidentität.

Beispiel:

```text
fieldId: 1
name: firstName
```
Feldnamen dürfen im Rahmen kontrollierter Migrationen geändert werden.

Die Feldidentität bleibt dabei unverändert.

Dadurch können serialisierte Daten weiterhin korrekt zugeordnet werden.

---

# 10. Array

Ein Array enthält eine feste oder deklarierte Anzahl gleichartiger Elemente.

Beispiel:

```text
Array<Float32, 3>
```

Eigenschaften:

- homogener Elementtyp
- definierte Reihenfolge
- direkter Indexzugriff
- feste oder dynamisch deklarierte Länge
- zusammenhängendes Speicherlayout möglich

Beispiele:

```text
Array<UInt8, 16>

Array<Vector3, 100>

Array<Float64, Dynamic>
```

---

# 11. Mehrdimensionale Arrays

Arrays können mehrere Dimensionen besitzen.

Beispiel:

```text
Array<Float32, 1920, 1080, 4>
```

oder:

```text
Array<Float64, Dynamic, Dynamic>
```

Jede Dimension besitzt:

```text
Größe

Indexbereich

Layout

Stride
```

Unterstützte Layouts:

```text
Row Major

Column Major

Strided

Tiled
```

---

# 12. List

Eine List enthält eine variable Anzahl gleichartiger Elemente.

Beispiel:

```text
List<String>
```

Eigenschaften:

- geordnete Elemente
- variable Länge
- Duplikate erlaubt
- dynamisches Wachstum
- sequenzieller Zugriff

Listen sind logisch unveränderlich.

Änderungen erzeugen eine neue Liste oder eine neue Objektversion.

---

# 13. Set

Ein Set enthält eindeutige Werte eines gemeinsamen Typs.

Beispiel:

```text
Set<CapabilityID>
```

Eigenschaften:

- keine Duplikate
- keine fachlich relevante Reihenfolge
- definierte Gleichheitsregel
- effizienter Mitgliedschaftstest

Die Gleichheit der Elemente muss eindeutig definiert sein.

---

# 14. Map

Eine Map ordnet eindeutige Schlüssel Werten zu.

Beispiel:

```text
Map<String, String>
```

oder:

```text
Map<ObjectID, DataObject>
```

Eine Map besitzt:

```text
Key Type

Value Type

Equality Rule

Ordering Rule
```

Schlüssel müssen:

- deterministisch vergleichbar
- stabil hashbar
- unveränderlich

sein.

---

# 15. Tuple

Ein Tuple enthält eine feste Anzahl geordneter Werte unterschiedlicher Typen.

Beispiel:

```text
Tuple<String, Int32, Bool>
```

Eigenschaften:

- feste Länge
- positionsbasierter Zugriff
- unterschiedliche Elementtypen
- keine Feldnamen erforderlich

Tuples eignen sich für kleine, lokale Datenstrukturen.

Für öffentliche Capability Contracts sollen bevorzugt benannte Records verwendet werden.

---

# 16. Record

Ein Record ist eine benannte, unveränderliche Struktur aus typisierten Feldern.

Beispiel:

```text
Record Person

firstName: String

lastName: String

age: UInt16
```

Eigenschaften:

- wertbasiert
- immutable
- benannte Felder
- strukturell vergleichbar
- vollständig serialisierbar
- keine eigene Identität

Records sind der Standardtyp für strukturierte Capability-Daten.

---

# 17. Object

Ein Object ist eine strukturierte Einheit mit stabiler Identität.

Es besitzt:

```text
Object ID

Type ID

Version

Fields

Lifecycle
```

Im Gegensatz zu einem Record ist ein Object nicht ausschließlich wertbasiert.

Zwei Objekte mit denselben Feldern können unterschiedliche Identitäten besitzen.

---

# 18. Unterschied zwischen Record und Object

```text
Record

Wertidentität
```

```text
Object

Objektidentität
```

Beispiel:

Zwei identische Koordinaten-Records gelten als gleich.

Zwei Benutzerobjekte mit identischen Feldern können trotzdem verschiedene Benutzer darstellen.

Records eignen sich für:

- Messwerte
- Konfigurationen
- Nachrichten
- mathematische Strukturen

Objects eignen sich für:

- Benutzer
- Geräte
- Dokumente
- Capabilities
- persistente Ressourcen

---

# 19. Optional

Optional beschreibt das mögliche Vorhandensein eines Wertes.

Beispiel:

```text
Optional<String>
```

Mögliche Zustände:

```text
Some<Value>

None
```

Optional ist nicht identisch mit:

- leerem String
- Null
- Standardwert
- Fehler

Öffentliche Contracts müssen optionale Werte explizit deklarieren.

---

# 20. Result

Result beschreibt entweder einen erfolgreichen Wert oder einen Fehler.

Beispiel:

```text
Result<PDFDocument, ConversionError>
```

Mögliche Zustände:

```text
Success<Value>

Failure<Error>
```

Result darf nicht gleichzeitig Erfolg und Fehler enthalten.

Dieser Typ bildet das bevorzugte Rückgabemodell für erwartbare Fehler.

---

# 21. Variant

Ein Variant kann genau einen Wert aus mehreren benannten Alternativen enthalten.

Beispiel:

```text
Variant Shape

Circle:
    radius: Float64

Rectangle:
    width: Float64
    height: Float64
```

Jede Alternative besitzt eine stabile Variant ID.

Variants werden bevorzugt für geschlossene Zustandsmodelle verwendet.

---

# 22. Union

Eine Union beschreibt mehrere mögliche Typen ohne zwingend benannte fachliche Fälle.

Beispiel:

```text
Union<Int32, Float64>
```

Unions sollen nur verwendet werden, wenn eine präzisere Modellierung durch Variant oder gemeinsame Obertypen nicht möglich ist.

Offene, unkontrollierte Unions sind in öffentlichen Capability Contracts nicht zulässig.

---

# 23. Enumeration

Eine Enumeration definiert eine geschlossene Menge benannter Werte.

Beispiel:

```text
enum LogLevel

Trace

Debug

Info

Warning

Error

Fatal
```

Jeder Eintrag besitzt:

```text
Name

Stable Value ID

Optional Numeric Representation
```

Die numerische Repräsentation darf nicht als fachliche Identität verwendet werden.

---

# 24. Flag Enumeration

Flag Enumerations erlauben die Kombination mehrerer Werte.

Beispiel:

```text
flags AccessRights

Read

Write

Execute

Delete
```

Jeder Wert muss eine eindeutige Bitposition besitzen.

Unbekannte Bits müssen bei der Deserialisierung nach definierter Richtlinie behandelt werden.

---

# 25. Range

Ein Range beschreibt einen Wertebereich.

Beispiel:

```text
Range<Int32>
```

Eigenschaften:

```text
Start

End

Inclusive Start

Inclusive End

Step
```

Beispiele:

```text
0..100

0..<100

1..100 step 2
```

Der Elementtyp muss geordnet und schrittweise veränderbar sein.

---

# 26. Reference

Eine Reference verweist auf ein anderes Datenobjekt.

Beispiel:

```text
Reference<Document>
```

Eine Referenz enthält mindestens:

```text
Object ID

Expected Type

Version Constraint

Access Rights
```

Referenzen ermöglichen:

- gemeinsame Daten
- zyklische Graphen
- große Objektstrukturen
- Lazy Loading
- Remote Objects
- Zero-Copy

---

# 27. Schwache Referenzen

Nova unterstützt optionale Weak References.

Beispiel:

```text
WeakReference<CacheEntry>
```

Eine schwache Referenz verhindert nicht die Freigabe des Zielobjekts.

Beim Zugriff muss geprüft werden, ob das Ziel weiterhin existiert.

Weak References dürfen nicht zur Modellierung zwingender fachlicher Beziehungen verwendet werden.

---

# 28. Graph

Ein Graph besteht aus typisierten Knoten und Kanten.

Beispiel:

```text
Graph<CapabilityNode, DataflowEdge>
```

Ein Graph definiert:

```text
Node Type

Edge Type

Direction

Multiplicity

Cycle Policy
```

Graphen können gerichtet, ungerichtet, azyklisch oder zyklisch sein.

---

# 29. Tree

Ein Tree ist ein spezialisierter azyklischer Graph.

Eigenschaften:

- genau ein Wurzelknoten
- jeder Kindknoten besitzt höchstens einen Elternknoten
- keine Zyklen

Beispiele:

```text
Syntax Tree

UI Tree

Directory Tree

Type Hierarchy
```

---

# 30. Stream

Ein Stream beschreibt eine zeitlich oder sequenziell eintreffende Folge gleichartiger Werte.

Beispiel:

```text
Stream<SensorMeasurement>
```

Ein Stream kann sein:

```text
Finite

Infinite

Ordered

Unordered

Replayable

Non-Replayable
```

Der Stream-Typ beschreibt den Datenfluss.

Die konkrete Ausführungsstrategie wird von der Runtime bestimmt.

---

# 31. Table

Eine Table beschreibt strukturierte tabellarische Daten.

Sie besteht aus:

```text
Schema

Columns

Rows

Constraints

Indexes
```

Beispiel:

```text
Table<Employee>
```

Spalten besitzen:

- Name
- Field ID
- Datentyp
- Nullability
- Constraints

Tabellen können zeilen- oder spaltenorientiert gespeichert werden.

---

# 32. DataFrame

Ein DataFrame ist eine dynamischere tabellarische Struktur.

Im Gegensatz zu einer streng typisierten Table kann ein DataFrame:

- heterogene Spalten
- dynamische Spaltennamen
- unvollständige Werte
- Analysemetadaten

enthalten.

Für öffentliche Contracts muss das erwartete Mindestschema angegeben werden.

---

# 33. Verschachtelung

Zusammengesetzte Typen dürfen beliebig verschachtelt werden.

Beispiel:

```text
Record Project

name: String

members: List<Person>

metadata: Map<String, String>

status: Optional<ProjectStatus>
```

Implementierungen dürfen technische Tiefenlimits definieren.

Diese Limits müssen:

- dokumentiert
- konfigurierbar
- sicherheitsgeprüft

sein.

---

# 34. Zyklische Strukturen

Direkt eingebettete Wertzyklen sind nicht zulässig.

Ungültig:

```text
Record Node

child: Node
```

Zulässig:

```text
Object Node

child: Optional<Reference<Node>>
```

Zyklen müssen immer über explizite Referenzen modelliert werden.

---

# 35. Generische Typen

Zusammengesetzte Typen können generische Parameter besitzen.

Beispiele:

```text
List<T>

Map<K, V>

Result<T, E>

Reference<T>
```

Generische Parameter können Constraints besitzen.

Beispiel:

```text
Map<K, V>

where K: Hashable + Comparable
```

---

# 36. Generische Spezialisierung

Eine konkrete Verwendung eines generischen Typs erzeugt einen spezialisierten Typ.

Beispiel:

```text
List<String>
```

ist nicht identisch mit:

```text
List<Int32>
```

Spezialisierungen müssen durch Reflection vollständig auflösbar sein.

---

# 37. Constraints

Zusammengesetzte Typen können Typ- und Feldconstraints besitzen.

Beispiele:

```text
Minimum Length

Maximum Length

Unique Elements

Required Fields

Mutually Exclusive Fields

Sorted

Non-Empty

Key Uniqueness
```

Constraints werden vor der Ausführung eines Capability Contracts validiert.

---

# 38. Invarianten

Ein zusammengesetzter Typ darf domänenbezogene Invarianten besitzen.

Beispiel:

```text
Record TimeInterval

start: Instant

end: Instant
```

Invariante:

```text
start <= end
```

Invarianten müssen:

- deterministisch
- nebenwirkungsfrei
- maschinenlesbar
- testbar

sein.

---

# 39. Feldabhängigkeiten

Felder können voneinander abhängig sein.

Beispiel:

```text
Wenn encryptionEnabled = true,

muss encryptionAlgorithm gesetzt sein.
```

Solche Beziehungen werden deklarativ beschrieben.

Versteckte Validierungslogik ist nicht zulässig.

---

# 40. Pflichtfelder

Ein Feld kann sein:

```text
Required

Optional

Conditionally Required
```

Pflichtfelder müssen bei Konstruktion oder Deserialisierung vorhanden sein.

Fehlende Pflichtfelder führen zu einem Validierungsfehler.

---

# 41. Standardwerte

Felder dürfen Standardwerte besitzen.

Beispiel:

```text
enabled: Bool = true
```

Standardwerte müssen:

- typkompatibel
- deterministisch
- unveränderlich
- versionsstabil

sein.

Dynamisch berechnete Werte sind keine Standardwerte, sondern Initialisierungslogik.

---

# 42. Unbekannte Felder

Bei der Deserialisierung können unbekannte Felder auftreten.

Unterstützte Richtlinien:

```text
Reject

Ignore

Preserve
```

Für langfristig versionierte Datenformate soll `Preserve` bevorzugt werden.

Sicherheitskritische Strukturen dürfen `Reject` verlangen.

---

# 43. Feldreihenfolge

Die deklarative Feldreihenfolge dient:

- Dokumentation
- Benutzeroberflächen
- kanonischer Darstellung

Sie darf nicht automatisch die Speicherreihenfolge bestimmen.

Das physische Layout wird separat spezifiziert.

---

# 44. Speicherlayout

Ein zusammengesetzter Typ kann verschiedene Speicherlayouts besitzen.

Beispiele:

```text
Packed

Aligned

Native

Portable

Structure of Arrays

Array of Structures
```

Das Layout muss zum Ausführungskontext passen.

Persistente und übertragene Daten dürfen kein plattformabhängiges Native Layout voraussetzen.

---

# 45. Alignment

Felder können Alignment-Anforderungen besitzen.

Beispiel:

```text
alignment: 16 bytes
```

Alignment ist eine technische Eigenschaft und darf die semantische Typidentität nicht verändern.

Die Runtime kann Layouts für CPU, GPU oder Beschleuniger optimieren.

---

# 46. Padding

Physisches Padding darf zwischen Feldern eingefügt werden.

Padding:

- besitzt keine fachliche Bedeutung
- wird nicht serialisiert
- beeinflusst keine strukturelle Gleichheit
- muss bei sicherheitskritischen Übergängen initialisiert werden

Nicht initialisierte Padding-Bytes dürfen nicht exportiert werden.

---

# 47. Unveränderlichkeit

Zusammengesetzte Werte sind standardmäßig unveränderlich.

Beispiel:

```text
Record V1

↓

Änderung eines Feldes

↓

Record V2
```

Bei Objects erzeugt eine Änderung eine neue Objektversion.

Mutable Strukturen müssen ausdrücklich als solche deklariert werden und unterliegen zusätzlichen Synchronisationsregeln.

---

# 48. Gleichheit

Nova unterscheidet:

```text
Value Equality

Identity Equality

Reference Equality
```

Value Equality vergleicht Inhalte.

Identity Equality vergleicht Object IDs.

Reference Equality vergleicht konkrete Referenzen.

Der verwendete Vergleich muss aus dem Typ eindeutig hervorgehen.

---

# 49. Hashing

Hashbare Typen müssen einen deterministischen Hash definieren.

Der Hash muss berücksichtigen:

- Typidentität
- relevante Feldwerte
- kanonische Reihenfolge

Nicht stabile Laufzeitadressen dürfen nicht in den Hash eingehen.

---

# 50. Ordnung

Ein Typ kann eine definierte Ordnung besitzen.

Beispiele:

```text
Lexicographic

Numeric

Chronological

Custom Semantic Order
```

Eine Ordnung muss:

- deterministisch
- transitiv
- konsistent mit der Gleichheit

sein.

Nicht alle zusammengesetzten Typen sind sortierbar.

---

# 51. Kopieren

Zusammengesetzte Werte werden bevorzugt per Referenz übertragen.

Kopien erfolgen nur, wenn:

- explizit angefordert
- Isolation erforderlich
- Copy-on-Write ausgelöst
- ein Transportmechanismus keine Referenz unterstützt

Große Strukturen sollen Zero-Copy- oder Shared-Memory-Mechanismen verwenden.

---

# 52. Projektionen

Eine Projection erzeugt eine Teilansicht eines strukturierten Typs.

Beispiel:

```text
Person
↓
Projection:
firstName
lastName
```

Projektionen dürfen:

- keine neue Semantik vortäuschen
- Pflichtinvarianten nicht verletzen
- Zugriffsrechte nicht umgehen

---

# 53. Sichten

Views ermöglichen einen alternativen Zugriff auf dieselben Daten.

Beispiel:

```text
Image Buffer
↓
Byte View
Pixel View
Tensor View
```

Eine View erzeugt nicht zwingend eine Kopie.

Der zugrunde liegende Datentyp und die verwendete Interpretation müssen explizit dokumentiert werden.

---

# 54. Schema

Jeder zusammengesetzte Typ besitzt ein maschinenlesbares Schema.

Das Schema beschreibt:

```text
Type ID

Version

Composite Kind

Generic Parameters

Members

Constraints

Invariants

Serialization

Compatibility
```

Das Schema ist Bestandteil der Type Registry.

---

# 55. Reflection

Zusammengesetzte Typen unterstützen vollständige Reflection.

Abfragbar sind:

- Typidentität
- Kategorie
- generische Parameter
- Felder
- Feldidentitäten
- Feldtypen
- Constraints
- Invarianten
- Standardwerte
- Speicherlayout
- Serialisierung
- Version

Reflection darf keine privaten Feldinhalte offenlegen.

---

# 56. Serialisierung

Zusammengesetzte Typen können in mehreren Formaten serialisiert werden.

Unterstützt werden mindestens:

```text
Nova Binary

JSON

CBOR
```

Optional:

```text
MessagePack

Protocol Buffers

XML

FlatBuffers

Apache Arrow
```

Die Serialisierung muss die Typsemantik erhalten oder eindeutig referenzieren.

---

# 57. Kanonische Serialisierung

Für Signaturen, Hashes und Reproduzierbarkeit existiert eine kanonische Serialisierung.

Diese definiert:

- eindeutige Feldreihenfolge
- standardisierte Zahlendarstellung
- standardisierte Zeichenkodierung
- eindeutige Behandlung optionaler Felder
- eindeutige Map-Sortierung
- keine bedeutungslosen Whitespaces

Identische Werte müssen identische kanonische Bytefolgen erzeugen.

---

# 58. Binärserialisierung

Das Nova-Binärformat verwendet:

- stabile Type IDs
- stabile Field IDs
- definierte Byte-Reihenfolge
- Längeninformationen
- Versionsinformationen
- optionale Prüfsummen

Unbekannte Felder können abhängig von der Richtlinie übersprungen oder erhalten werden.

---

# 59. JSON-Serialisierung

Bei JSON gelten folgende Regeln:

- Feldnamen sind UTF-8.
- Numerische Genauigkeit darf nicht unbemerkt verloren gehen.
- Binärdaten werden explizit kodiert.
- Typinformationen können eingebettet oder extern gebunden werden.
- Maps mit Nicht-String-Schlüsseln benötigen eine definierte Repräsentation.
- Varianten benötigen einen eindeutigen Discriminator.

---

# 60. Variant-Serialisierung

Variants werden mit einem stabilen Discriminator serialisiert.

Beispiel:

```yaml
shape:
  type: circle
  radius: 5.0
```

Der Discriminator muss einer stabilen Variant ID entsprechen.

Alle nicht aktiven Alternativen bleiben ungesetzt.

---

# 61. Referenzserialisierung

Referenzen werden nicht als vollständige Zielobjekte serialisiert, sofern kein Snapshot verlangt wird.

Eine Referenzserialisierung enthält:

```text
Object ID

Expected Type

Version

Location Hint

Access Policy
```

Ein vollständiges Einbetten des Zielobjekts muss ausdrücklich deklariert werden.

---

# 62. Versionierung

Zusammengesetzte Typen verwenden semantische Versionierung.

Kompatible Änderungen können sein:

- Hinzufügen optionaler Felder
- Hinzufügen neuer Variant-Fälle bei offenen Varianten
- Ergänzung nicht verschärfender Metadaten
- Erweiterung unterstützter Serialisierungen

Inkompatible Änderungen sind unter anderem:

- Entfernen eines Pflichtfelds
- Ändern eines Feldtyps
- Wiederverwenden einer Field ID
- Ändern der Bedeutung eines Felds
- Verschärfen von Constraints ohne Migration
- Ändern einer geschlossenen Enumeration

---

# 63. Offene und geschlossene Typen

Nova unterscheidet:

```text
Closed Type

Open Type
```

Closed Types akzeptieren ausschließlich bekannte Felder oder Fälle.

Open Types können definierte Erweiterungen aufnehmen.

Öffnung muss ausdrücklich deklariert sein.

Sicherheitskritische Contracts sollen standardmäßig geschlossen sein.

---

# 64. Schema-Evolution

Schema-Evolution muss kontrolliert erfolgen.

Unterstützte Mechanismen:

```text
Field Addition

Field Deprecation

Field Alias

Default Value

Migration Capability

Version Adapter
```

Daten dürfen nicht stillschweigend fachlich uminterpretiert werden.

---

# 65. Migration

Inkompatible Typänderungen benötigen eine explizite Migrations-Capability.

Beispiel:

```text
Person@1
↓
Person v1 → v2 Migrator
↓
Person@2
```

Die Migration wird:

- versioniert
- getestet
- protokolliert
- in der Provenance dokumentiert

---

# 66. Capability Contracts

Capability Contracts müssen für zusammengesetzte Typen deklarieren:

```text
Exact Type

Minimum Required Fields

Accepted Version Range

Required Traits

Constraints

Mutability

Ownership
```

Ein Contract darf keine Struktur ausschließlich durch untypisierte Maps ersetzen, wenn ein stabiles Schema möglich ist.

---

# 67. Dataflow-Kompatibilität

Bei der Verbindung zusammengesetzter Typen prüft die Runtime:

```text
Type Identity

Composite Kind

Generic Parameters

Required Fields

Field Types

Constraints

Semantic Types

Version Compatibility
```

Eine rein technische Feldähnlichkeit reicht nicht aus.

---

# 68. Strukturelle Subtypen

Ein struktureller Subtyp darf zusätzliche Felder enthalten, wenn:

- der Contract strukturelle Typisierung erlaubt,
- alle benötigten Felder vorhanden sind,
- Feldtypen kompatibel sind,
- keine Sicherheitsrichtlinie verletzt wird.

Beispiel:

```text
Erwartet:

Record Point2D

x: Float64

y: Float64
```

Geliefert:

```text
Record Point3D

x: Float64

y: Float64

z: Float64
```

Diese Verbindung ist nur bei ausdrücklich erlaubter struktureller Kompatibilität zulässig.

---

# 69. Sicherheitsklassifikation

Ein zusammengesetzter Typ kann Sicherheitsklassifikationen auf Typ- und Feldebene besitzen.

Beispiel:

```text
Record Credential

username: String

password: Secret<String>
```

Feldklassifikationen beeinflussen:

- Logging
- Debugging
- Serialisierung
- Persistenz
- Netzwerkübertragung
- Zugriffskontrolle

---

# 70. Datenschutz

Personenbezogene Felder müssen semantisch klassifiziert werden.

Beispiel:

```text
Record Person

name: PersonalData<String>

email: PersonalData<EmailAddress>
```

Die Runtime muss verhindern, dass geschützte Felder unkontrolliert:

- protokolliert
- exportiert
- in Telemetrie aufgenommen
- dauerhaft gespeichert

werden.

---

# 71. Fehlerbehandlung

Standardisierte Fehlercodes umfassen:

```text
COMPOSITE_TYPE_UNKNOWN

COMPOSITE_KIND_MISMATCH

FIELD_REQUIRED

FIELD_UNKNOWN

FIELD_TYPE_MISMATCH

FIELD_CONSTRAINT_VIOLATION

INVARIANT_VIOLATION

GENERIC_ARGUMENT_INVALID

VARIANT_UNKNOWN

REFERENCE_INVALID

SCHEMA_VERSION_INCOMPATIBLE

CYCLIC_VALUE_STRUCTURE
```

Fehler müssen den betroffenen Typ und Feldpfad enthalten.

---

# 72. Feldpfade

Verschachtelte Fehler und Zugriffe verwenden standardisierte Feldpfade.

Beispiel:

```text
project.members[3].address.city
```

Maps verwenden Schlüsselnotation:

```text
settings["renderer"].quality
```

Feldpfade dürfen keine Zugriffsrechte umgehen.

---

# 73. Provenance

Die Provenance eines zusammengesetzten Datenobjekts dokumentiert:

- Typversion
- Schema-ID
- Erzeugungsquelle
- Feldtransformationen
- Migrationen
- Projektionen
- Zusammenführungen
- Aufteilungen

Bei strukturellen Änderungen wird eine neue Objektversion erzeugt.

---

# 74. Observability

Die Runtime kann folgende strukturbezogene Metriken erfassen:

```text
Object Size

Field Count

Collection Length

Serialization Duration

Validation Duration

Reference Count

Migration Count

Schema Cache Hit Rate
```

Geschützte Feldinhalte dürfen nicht Bestandteil allgemeiner Telemetrie sein.

---

# 75. Leistungsanforderungen

Implementierungen sollen:

- Schemainformationen cachen
- unnötige Kopien vermeiden
- Lazy Deserialization unterstützen
- partielle Datenzugriffe ermöglichen
- Zero-Copy-Views unterstützen
- spaltenorientierte Layouts für Analyse zulassen
- GPU-kompatible Layouts ermöglichen

Optimierungen dürfen das fachliche Verhalten nicht verändern.

---

# 76. Größen- und Tiefenlimits

Die Runtime muss Schutzgrenzen unterstützen für:

```text
Maximum Nesting Depth

Maximum Field Count

Maximum Collection Length

Maximum Serialized Size

Maximum Reference Count
```

Diese Grenzen verhindern:

- Speichererschöpfung
- Stack Overflow
- Parserangriffe
- zyklische Auflösungsschleifen

Limits müssen kontextabhängig konfigurierbar sein.

---

# 77. Type Registry

Alle veröffentlichten zusammengesetzten Typen werden in der Nova Type Registry registriert.

Gespeichert werden:

```text
Type ID

Name

Namespace

Version

Schema

Composite Kind

Dependencies

Compatibility

Migration Paths

Documentation
```

Die Registry muss Typabhängigkeiten als Graph verwalten.

---

# 78. Abhängigkeiten

Ein zusammengesetzter Typ ist von allen direkt verwendeten Typen abhängig.

Beispiel:

```text
Person
├── String
├── Date
└── Address
```

Änderungen abhängiger Typen können die Kompatibilität des zusammengesetzten Typs beeinflussen.

Die Registry muss solche Auswirkungen analysieren können.

---

# 79. YAML-Beispiel eines Records

```yaml
compositeType:
  id: nova.core.person
  name: Person
  namespace: nova.core
  version: 1.0.0
  kind: record
  immutable: true
  fields:
    - id: 1
      name: personId
      type: nova.core.identifier.person-id
      required: true
    - id: 2
      name: firstName
      type: String
      semanticType: nova.core.person-name
      required: true
    - id: 3
      name: lastName
      type: String
      semanticType: nova.core.person-name
      required: true
    - id: 4
      name: email
      type: Optional<nova.core.contact.email-address>
      required: false
  serialization:
    unknownFields: preserve
  compatibility:
    structural: false
```

---

# 80. YAML-Beispiel eines Variant-Typs

```yaml
compositeType:
  id: nova.geometry.shape
  name: Shape
  namespace: nova.geometry
  version: 1.0.0
  kind: variant
  variants:
    - id: 1
      name: Circle
      fields:
        - id: 1
          name: radius
          type: Float64
          semanticType: nova.geometry.length
    - id: 2
      name: Rectangle
      fields:
        - id: 1
          name: width
          type: Float64
          semanticType: nova.geometry.length
        - id: 2
          name: height
          type: Float64
          semanticType: nova.geometry.length
```

---

# 81. YAML-Beispiel einer Collection

```yaml
compositeType:
  id: nova.core.person-list
  name: PersonList
  namespace: nova.core
  version: 1.0.0
  kind: list
  elementType: nova.core.person
  constraints:
    minimumLength: 0
    maximumLength: 100000
  ordering:
    significant: true
```

---

# 82. Validierungsregeln

Ein zusammengesetzter Typ gilt als gültig, wenn:

- eine eindeutige Type ID existiert,
- eine gültige Kategorie definiert ist,
- alle verwendeten Typen registriert oder primitive Typen sind,
- Field IDs innerhalb des Typs eindeutig sind,
- Feldnamen innerhalb ihres Gültigkeitsbereichs eindeutig sind,
- generische Parameter vollständig gebunden oder deklariert sind,
- Constraints widerspruchsfrei sind,
- Invarianten deterministisch validierbar sind,
- keine direkt eingebetteten Wertzyklen existieren,
- Version und Kompatibilitätsregeln angegeben sind,
- mindestens eine Serialisierung definiert ist.

---

# 83. Laufzeitanforderungen

Die Nova Runtime muss:

- zusammengesetzte Typen dynamisch auflösen,
- Feldtypen validieren,
- Invarianten prüfen,
- generische Typen spezialisieren,
- Referenzen sicher auflösen,
- Unknown-Field-Richtlinien beachten,
- Schema-Versionen vergleichen,
- Copy-on-Write unterstützen,
- Reflection bereitstellen,
- Provenance erhalten,
- Größen- und Tiefenlimits erzwingen.

---

# 84. Compileranforderungen

Der Nova-Compiler muss:

- Feldzugriffe statisch prüfen,
- Pflichtfelder validieren,
- generische Parameter prüfen,
- Variant-Fälle vollständig analysieren,
- ungültige Zyklen erkennen,
- Typ- und Schema-Kompatibilität prüfen,
- strukturierte Pattern Matches unterstützen,
- aussagekräftige Feldpfadfehler erzeugen,
- Speicherlayoutoptimierungen ermöglichen.

---

# 85. Testanforderungen

Jeder zusammengesetzte Typ benötigt Tests für:

- Konstruktion
- Pflichtfelder
- optionale Felder
- Constraints
- Invarianten
- Gleichheit
- Hashing
- Serialisierung
- Deserialisierung
- unbekannte Felder
- Versionierung
- Migration
- Reflection
- Grenzwerte
- ungültige Referenzen
- verschachtelte Strukturen

Öffentliche Typen benötigen zusätzliche Konformitätstests.

---

# 86. Qualitätsanforderungen

Das Modell zusammengesetzter Datentypen muss:

- stark typisiert
- deterministisch
- vollständig reflektierbar
- effizient serialisierbar
- versionierbar
- migrationsfähig
- plattformunabhängig
- sicher
- speichereffizient
- Zero-Copy-fähig
- semantisch erweiterbar
- langfristig kompatibel

sein.

---

# 87. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation, wenn:

- alle verpflichtenden zusammengesetzten Typkategorien unterstützt werden,
- stabile Type IDs und Field IDs verwendet werden,
- Records und Objects korrekt unterschieden werden,
- Optional, Result und Variant unterstützt werden,
- generische Typen verarbeitet werden können,
- zyklische Beziehungen nur über Referenzen entstehen,
- Constraints und Invarianten validiert werden,
- Reflection vollständig verfügbar ist,
- Schema-Evolution und Migration unterstützt werden,
- kanonische Serialisierung möglich ist,
- semantische Typinformationen erhalten bleiben,
- referenzbasierte Datenübertragung unterstützt wird.

---

# 88. Zusammenfassung

Die **zusammengesetzten Datentypen** bilden die strukturierte Ebene des universellen Nova-Datentypmodells.

Sie kombinieren primitive, semantische und weitere zusammengesetzte Typen zu eindeutig definierten Records, Objects, Collections, Varianten, Referenzen, Graphen, Streams und Tabellen.

Durch stabile Typ- und Feldidentitäten, generische Parameter, deklarative Constraints, Invarianten, vollständige Reflection, kontrollierte Schema-Evolution und explizite Migrationen entstehen langfristig kompatible Datenmodelle.

Die Trennung zwischen Wertidentität und Objektidentität, die unveränderliche Verarbeitung, das referenzbasierte Datenmodell sowie die Unterstützung kanonischer Serialisierung schaffen eine sichere und leistungsfähige Grundlage für Capability Contracts, Dataflows, KI-Systeme, Robotik, wissenschaftliche Anwendungen und verteilte Nova-Systeme.