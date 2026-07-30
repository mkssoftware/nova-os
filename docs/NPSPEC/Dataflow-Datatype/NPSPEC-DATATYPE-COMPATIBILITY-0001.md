# NPSPEC-DATATYPE-COMPATIBILITY-0001: Typkompatibilität und Subtyping (Teil 1)

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Dokument-ID:** NPSPEC-DATATYPE-COMPATIBILITY-0001
- **Titel:** Typkompatibilität und Subtyping
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
- NPSPEC-DATATYPE-CONVERSION-0001
- NPSPEC-CAPABILITY-CONTRACT-0001
- NPSPEC-CAPABILITY-DISCOVERY-0001
- ADR-DATAFLOW-0002
- ADR-DATAFLOW-0003

---

# 1. Zweck

Diese Spezifikation definiert das universelle Modell der Typkompatibilität innerhalb der Nova Platform.

Sie beschreibt, wann zwei Typen als miteinander kompatibel gelten, wann Werte zugewiesen werden dürfen und unter welchen Bedingungen Subtyping zulässig ist.

Das Modell bildet die Grundlage für:

- Compiler
- Runtime
- Reflection
- Capability Contracts
- Serialisierung
- Netzwerkkommunikation
- Plugin-Systeme
- Datenmigration
- Schemaevolution
- automatische Adapter
- Converter Discovery

---

# 2. Ziele

Das Kompatibilitätsmodell verfolgt folgende Ziele:

- starke Typsicherheit
- deterministische Entscheidungen
- reproduzierbare Kompatibilitätsprüfung
- minimale implizite Konvertierungen
- langfristige Binärkompatibilität
- Schemaevolution
- sichere API-Versionierung
- automatische Adapter
- vollständige Reflection
- Compiler-Optimierungen

---

# 3. Grundprinzipien

Nova verwendet folgende Grundregeln:

- Jeder Typ besitzt eine eindeutige Identität.
- Kompatibilität ist explizit definiert.
- Gleichheit bedeutet nicht automatisch Kompatibilität.
- Kompatibilität bedeutet nicht automatisch Konvertierbarkeit.
- Subtyping bedeutet nicht automatische Gleichheit.
- Semantik besitzt Vorrang vor technischer Darstellung.
- Versionen beeinflussen die Kompatibilität.
- Capability Contracts dürfen Kompatibilität weiter einschränken.
- Alle Entscheidungen müssen deterministisch sein.

---

# 4. Architektur

```text
Source Type

↓

Compatibility Engine

├── Identity Check

├── Technical Check

├── Semantic Check

├── Structural Check

├── Version Check

├── Capability Rules

└── Policy Rules

↓

Compatibility Result
```

---

# 5. Definition

Typkompatibilität beschreibt, ob ein Wert eines Typs A überall dort verwendet werden darf, wo Typ B erwartet wird.

Formal:

```text
A → B
```

Dabei gilt:

```text
Compatible

Assignable

Convertible

Equivalent
```

sind unterschiedliche Eigenschaften.

---

# 6. Kompatibilitätsarten

Nova unterscheidet:

```text
Identity Compatibility

Nominal Compatibility

Structural Compatibility

Semantic Compatibility

Technical Compatibility

Behavioral Compatibility

Version Compatibility

Capability Compatibility
```

---

# 7. Identitätskompatibilität

Zwei Typen besitzen Identitätskompatibilität, wenn:

- Type ID identisch ist
- Version kompatibel ist
- Schema kompatibel ist

Beispiel:

```text
nova.core.Person
```

↓

```text
nova.core.Person
```

---

# 8. Nominale Kompatibilität

Nominale Kompatibilität basiert ausschließlich auf deklarierter Typidentität.

Beispiel:

```text
Vehicle

↑

Car
```

Car ist kompatibel zu Vehicle.

Nicht kompatibel:

```text
Car

Truck
```

obwohl beide dieselben Felder besitzen können.

---

# 9. Strukturelle Kompatibilität

Strukturelle Kompatibilität basiert ausschließlich auf der Struktur.

Beispiel:

```text
Type A

name : String

age : Int
```

```text
Type B

name : String

age : Int
```

Diese Typen können strukturell kompatibel sein.

---

# 10. Semantische Kompatibilität

Semantische Kompatibilität prüft zusätzlich die fachliche Bedeutung.

Beispiel:

```text
EmailAddress
```

und

```text
URL
```

sind technisch beide String.

Semantisch sind sie inkompatibel.

---

# 11. Technische Kompatibilität

Technische Kompatibilität prüft ausschließlich die Speicherrepräsentation.

Beispiel:

```text
UInt32

↓

UInt32
```

---

# 12. Verhalten

Behavioral Compatibility beschreibt das erwartete Verhalten eines Typs.

Beispiel:

```text
Stream

↓

Closable Stream
```

Ein Typ muss alle garantierten Eigenschaften erfüllen.

---

# 13. Version Compatibility

Versionen können:

```text
Compatible

Deprecated

Unsupported

Future

Unknown
```

sein.

---

# 14. Capability Compatibility

Capability Contracts können zusätzliche Regeln definieren.

Beispiel:

```text
Input:

Vehicle
```

Capability akzeptiert möglicherweise nur:

```text
Car
```

obwohl andere Untertypen existieren.

---

# 15. Kompatibilitätsstufen

Nova definiert:

```text
Exact

Safe

Compatible

Convertible

Incompatible
```

---

# 16. Exact

Typen sind identisch.

---

# 17. Safe

Typ kann ohne Informationsverlust verwendet werden.

---

# 18. Compatible

Typ darf verwendet werden.

Kleine Unterschiede sind zulässig.

---

# 19. Convertible

Direkte Verwendung nicht möglich.

Converter erforderlich.

---

# 20. Incompatible

Keine gültige Beziehung vorhanden.

---

# 21. Typgleichheit

Nova unterscheidet:

```text
Identity

Equality

Compatibility

Assignability

Convertibility
```

Diese Begriffe dürfen niemals synonym verwendet werden.

---

# 22. Assignability

Assignability beschreibt:

"Darf ein Wert hier zugewiesen werden?"

Beispiel:

```text
Vehicle

=

Car
```

ist zulässig.

---

# 23. Convertibility

Convertibility beschreibt:

"Kann ein Wert umgewandelt werden?"

Dies ist unabhängig von Assignability.

---

# 24. Typäquivalenz

Typäquivalenz bedeutet:

Beide Typen repräsentieren dieselbe logische Bedeutung.

Nicht zwingend:

- gleiche Version
- gleiche Struktur
- gleiche Darstellung

---

# 25. Identität

Jeder Typ besitzt:

```text
Type ID

Version

Namespace

Schema
```

Die Identität bleibt stabil.

---

# 26. Supertypen

Ein Supertyp beschreibt allgemeineres Verhalten.

Beispiel:

```text
Object

↓

LivingBeing

↓

Animal

↓

Dog
```

---

# 27. Subtypen

Subtypen spezialisieren ihren Supertyp.

Sie dürfen:

- neue Eigenschaften hinzufügen
- Verhalten erweitern
- Garantien verstärken

Sie dürfen garantierte Eigenschaften des Supertyps nicht verletzen.

---

# 28. Mehrfachvererbung

Nova erlaubt Mehrfachvererbung ausschließlich für:

- Interfaces
- Traits
- Capabilities

Nicht für konkrete Datentypen.

---

# 29. Interfaces

Interfaces definieren Verhalten.

Beispiel:

```text
Printable

Serializable

Cloneable
```

Ein Typ kann mehrere Interfaces implementieren.

---

# 30. Traits

Traits beschreiben wiederverwendbare Eigenschaften.

Beispiele:

```text
Timestamped

Encrypted

Versioned

Localized
```

Traits beeinflussen die Kompatibilität.

---

# 31. Behavioral Subtyping

Ein Subtyp muss sämtliche Zusicherungen seines Supertyps erfüllen.

Ein Subtyp darf:

- stärkere Garantien geben
- zusätzliche Funktionen besitzen

Er darf jedoch keine garantierte Eigenschaft entfernen.

---

# 32. Liskov-Kompatibilität

Nova orientiert sich am Liskov-Substitutionsprinzip.

Ein Untertyp muss überall dort einsetzbar sein, wo sein Obertyp erwartet wird, ohne das beobachtbare Verhalten zu verletzen.

---

# 33. Primitive Typen

Primitive Typen besitzen fest definierte Kompatibilitätsregeln.

Beispiele:

```text
UInt8

↓

UInt16
```

kompatibel.

```text
Float64

↓

UInt8
```

nicht direkt kompatibel.

---

# 34. Numerische Hierarchie

```text
Integer

↓

Signed Integer

↓

Int32
```

```text
Integer

↓

Unsigned Integer

↓

UInt32
```

---

# 35. Zeichenketten

Alle String-Typen besitzen denselben technischen Basistyp.

Semantische Untertypen bleiben jedoch eigenständig.

---

# 36. Bool

Bool besitzt keine numerische Kompatibilität.

Nicht zulässig:

```text
true

↓

1
```

ohne explizite Konvertierung.

---

# 37. Collections

Collections prüfen:

- Elementtyp
- Reihenfolge
- Constraints
- Nullability
- Mutability

---

# 38. Arrays

Arrays besitzen feste Größe.

```text
Array<10>

↓

Array<20>
```

ist inkompatibel.

---

# 39. Listen

Listen besitzen variable Größe.

Die Elementtypen bestimmen die Kompatibilität.

---

# 40. Mengen

Sets besitzen keine Reihenfolge.

List und Set sind daher nicht kompatibel.

---

# 41. Maps

Maps prüfen:

- Key Type
- Value Type

Beide müssen kompatibel sein.

---

# 42. Records

Records prüfen:

- Felder
- Typen
- Constraints
- Required Fields

---

# 43. Objekte

Objekte prüfen zusätzlich:

- Object ID
- Lebenszyklus
- Identität
- Version

---

# 44. Dokumente

Dokumenttypen besitzen:

- Struktur
- Inhalt
- Metadaten

Kompatibilität berücksichtigt alle drei Ebenen.

---

# 45. Streams

Streams prüfen:

- Elementtyp
- Reihenfolge
- QoS
- Zeitmodell
- Fenster

---

# 46. Strukturierte Daten

Tabellen prüfen:

- Schema
- Spalten
- Constraints
- Beziehungen

---

# 47. Nullability

Nova unterscheidet:

```text
Optional<T>

Nullable<T>
```

Diese Typen sind nicht identisch.

---

# 48. Optional

Optional bedeutet:

Der Wert kann fehlen.

---

# 49. Nullable

Nullable bedeutet:

Der Wert existiert.

Er kann Null enthalten.

---

# 50. Union Types

Union-Typen sind kompatibel, wenn der Zieltyp alle möglichen Varianten akzeptiert.

Beispiel:

```text
Int | Float
```

↓

```text
Number
```

ist zulässig.

---

# 51. Variant Types

Variant-Typen prüfen:

- aktive Variante
- Payload
- Version
- Schema

---

# 52. Enum-Kompatibilität

Geschlossene Enumerationen sind nur mit identischer Definition kompatibel.

Offene Enumerationen können zusätzliche Werte aufnehmen.

---

# 53. Schema-Kompatibilität

Vor jeder Typprüfung wird geprüft:

- Schema-ID
- Version
- Constraints
- Evolution

---

# 54. Reflection

Reflection liefert:

```text
Compatible Types

Supertypes

Subtypes

Interfaces

Traits

Converter
```

---

# 55. Zusammenfassung erster Teil

Dieser erste Teil definiert die grundlegenden Konzepte der Typkompatibilität innerhalb der Nova Platform. Er beschreibt Identität, nominale und strukturelle Kompatibilität, semantische Regeln sowie das Subtyping-Modell als Basis für Compiler, Runtime und Capability Contracts.

**Fortsetzung in Teil 2:** Varianz (Kovarianz, Kontravarianz, Invarianz), Generics, Versionierung, Schemaevolution, Compatibility Graph, automatische Adapter, Compiler- und Runtime-Regeln, YAML-Beispiele, Tests und Konformitätskriterien.


---

# 56. Varianzmodell

Nova definiert ein formales Varianzmodell für alle generischen Typen.

Unterstützt werden:

```text
Covariant

Contravariant

Invariant

Bivariant
```

Die Varianz ist Bestandteil der Typdefinition und darf nicht implizit angenommen werden.

---

# 57. Kovarianz

Kovarianz erlaubt die Ersetzung eines Typs durch einen spezielleren Typ.

Formal:

```text
T <: U

↓

Container<T> <: Container<U>
```

Beispiel:

```text
Animal

↑

Dog
```

```text
ReadOnlyList<Dog>

↓

ReadOnlyList<Animal>
```

Dies ist zulässig, da ausschließlich gelesen wird.

---

# 58. Kontravarianz

Kontravarianz erlaubt die Ersetzung eines Typs durch einen allgemeineren Typ.

Formal:

```text
T <: U

↓

Consumer<U> <: Consumer<T>
```

Beispiel:

```text
Handler<Animal>

↓

Handler<Dog>
```

Ein Handler für alle Tiere kann selbstverständlich auch Hunde verarbeiten.

---

# 59. Invarianz

Bei Invarianz müssen beide Typen identisch sein.

```text
List<Dog>

≠

List<Animal>
```

Standardmäßig verwendet Nova Invarianz.

---

# 60. Bivarianz

Bivarianz erlaubt beide Richtungen.

Sie darf ausschließlich in genau definierten Runtime-Schnittstellen verwendet werden.

Sie ist für normale Datentypen verboten.

---

# 61. Standardregel

Falls ein Typ nichts anderes deklariert gilt:

```text
Invariant
```

Dies verhindert unbeabsichtigte Typfehler.

---

# 62. Generische Typen

Generische Typen besitzen Typparameter.

Beispiel:

```text
List<T>

Map<K,V>

Result<T,E>

Stream<T>
```

Jeder Parameter besitzt eine eigene Varianzdefinition.

---

# 63. Mehrere Typparameter

Beispiel:

```text
Map<Key,Value>
```

Es gilt:

```text
Key

Invariant

Value

Covariant
```

Die Varianz jedes Parameters wird unabhängig geprüft.

---

# 64. Verschachtelte Generics

Kompatibilität wird rekursiv bestimmt.

Beispiel:

```text
List<List<Dog>>
```

↓

```text
List<List<Animal>>
```

Die Entscheidung erfolgt auf jeder Ebene getrennt.

---

# 65. Rekursive Typen

Nova unterstützt rekursive Typdefinitionen.

Beispiel:

```text
Directory

↓

Children

↓

Directory
```

Die Kompatibilitätsprüfung muss Zyklen erkennen.

---

# 66. Selbstreferenzen

Selbstreferenzierende Typen bleiben eindeutig identifizierbar.

```text
Node

↓

Parent

↓

Node
```

Reflection darf keine Endlosschleifen erzeugen.

---

# 67. Interface-Kompatibilität

Ein Typ ist interface-kompatibel wenn:

- alle Methoden vorhanden sind
- alle Verträge erfüllt werden
- alle Parameter kompatibel sind
- alle Rückgabewerte kompatibel sind

---

# 68. Trait-Kompatibilität

Traits erweitern einen Typ.

Beispiel:

```text
Encrypted

Localized

Timestamped
```

Ein Trait darf bestehende Garantien nicht verletzen.

---

# 69. Capability-Kompatibilität

Capabilities prüfen zusätzlich:

```text
Input Type

Output Type

Contract

Permissions

Version
```

---

# 70. Parameterkompatibilität

Funktionsparameter verwenden standardmäßig:

```text
Contravariant
```

Dadurch bleibt das Liskov-Prinzip erhalten.

---

# 71. Rückgabekompatibilität

Rückgabewerte verwenden:

```text
Covariant
```

Ein spezielleres Ergebnis ist zulässig.

---

# 72. Ausnahmekompatibilität

Eine Capability darf keine neuen ungeprüften Fehler garantieren.

Neue Fehler müssen:

- dokumentiert
- versioniert
- kompatibel

sein.

---

# 73. Null-Kompatibilität

Null ist kein universeller Typ.

Nur:

```text
Nullable<T>
```

akzeptiert Null.

---

# 74. Optional-Kompatibilität

Optional beschreibt:

```text
Vorhanden

oder

Nicht vorhanden
```

Nicht:

```text
Null
```

---

# 75. Any

Nova definiert einen obersten Typ.

```text
Any
```

Alle Typen sind zu Any kompatibel.

---

# 76. Never

Never besitzt keine Instanzen.

Beispiel:

```text
throw

panic

abort
```

Never ist Untertyp aller Typen.

---

# 77. Unknown

Unknown beschreibt einen unbekannten Typ.

Unknown ist weder automatisch kompatibel noch inkompatibel.

Vor Verwendung ist eine Typprüfung erforderlich.

---

# 78. Dynamic

Dynamic erlaubt dynamische Bindung.

Die Runtime muss vor jedem Zugriff prüfen.

---

# 79. Objektidentität

Objekte mit gleicher Struktur bleiben verschieden.

Beispiel:

```text
Person A

Person B
```

gleiche Daten

↓

nicht identisch

---

# 80. Referenztypen

Referenzen besitzen:

```text
Target Type

Ownership

Lifetime

Mutability
```

Alle Eigenschaften fließen in die Kompatibilität ein.

---

# 81. Ownership

Ownership beeinflusst:

- Kopierbarkeit
- Übergabe
- Lebensdauer

---

# 82. Lebensdauer

Lifetime muss kompatibel sein.

Ein kürzeres Lifetime darf nicht einem längeren Lifetime zugewiesen werden.

---

# 83. Mutability

Nova unterscheidet:

```text
Immutable

Mutable

ReadOnly
```

Mutable ist nicht automatisch kompatibel zu Immutable.

---

# 84. Thread-Sicherheit

Typen können deklarieren:

```text
Thread Safe

Single Thread

Actor Bound

Realtime Safe
```

Diese Eigenschaften gehören zur Kompatibilität.

---

# 85. Echtzeitfähigkeit

Realtime-Komponenten dürfen ausschließlich kompatible Echtzeittypen akzeptieren.

---

# 86. Sicherheitsklassifikation

Ein Typ besitzt optional:

```text
Public

Internal

Confidential

Secret
```

Ein Secret-Typ darf nicht implizit in Public konvertiert werden.

---

# 87. Provenance

Kompatibilität kann Provenance verlangen.

Beispiel:

```text
Trusted Source
```

↓

zulässig

```text
Unknown Source
```

↓

Warnung oder Fehler

---

# 88. Schemaevolution

Schemaänderungen beeinflussen die Typkompatibilität.

Unterstützt:

```text
Backward

Forward

Full
```

---

# 89. Rückwärtskompatibilität

Neue Version verarbeitet alte Daten.

---

# 90. Vorwärtskompatibilität

Alte Version verarbeitet neue Daten.

---

# 91. Vollständige Kompatibilität

Beide Richtungen sind möglich.

---

# 92. Adapter

Adapter können fehlende Kompatibilität herstellen.

Beispiel:

```text
Person V1

↓

Adapter

↓

Person V3
```

---

# 93. Automatische Adapter

Automatische Adapter dürfen ausschließlich verwendet werden wenn:

- eindeutig
- verlustfrei
- dokumentiert

---

# 94. Converter

Ist keine Kompatibilität vorhanden, kann ein Converter eingesetzt werden.

---

# 95. Compatibility Graph

Alle Typen bilden einen gerichteten Graphen.

```text
Type

↓

Subtype

↓

Subtype
```

---

# 96. Pfadsuche

Die Runtime sucht den besten Kompatibilitätspfad.

Bewertet werden:

- Qualität
- Kosten
- Sicherheit
- Version

---

# 97. Mehrere Pfade

Existieren mehrere gültige Pfade, wird gewählt:

1.

höchste Sicherheit

2.

geringster Informationsverlust

3.

geringste Kosten

---

# 98. Compilerregeln

Der Compiler muss:

- alle Typbeziehungen prüfen
- implizite Fehler erkennen
- unnötige Konvertierungen entfernen
- sichere Konvertierungen optimieren

---

# 99. Runtime-Regeln

Die Runtime muss:

- Reflection verwenden
- Adapter finden
- Converter verwenden
- Provenance erhalten
- Policies beachten

---

# 100. Reflection

Reflection liefert:

```text
Compatible Types

Supertypes

Subtypes

Variance

Interfaces

Traits

Capabilities

Converters
```

---

# 101. Discovery

Discovery ermöglicht:

```text
Finde alle kompatiblen Typen.

Finde alle Untertypen.

Finde alle Supertypen.

Finde alle Adapter.
```

---

# 102. YAML-Beispiel

```yaml
compatibility:

  source:

    type: Dog

  target:

    type: Animal

  result:

    compatible: true

    level: safe

    subtype: true
```

---

# 103. YAML-Beispiel Varianz

```yaml
generic:

  type: ReadOnlyList

  variance:

    parameter:

      T: covariant
```

---

# 104. YAML-Beispiel Interface

```yaml
type:

  name: Printer

implements:

  Printable

  Disposable
```

---

# 105. YAML-Beispiel Converter

```yaml
converter:

  source: Markdown

  target: HTML

  quality: lossless
```

---

# 106. Validierungsregeln

Eine Typbeziehung ist gültig wenn:

- Type IDs bekannt sind
- Versionen kompatibel sind
- Constraints erfüllt werden
- Policies eingehalten werden
- Reflection erfolgreich ist

---

# 107. Testanforderungen

Jeder Typ benötigt Tests für:

- Gleichheit
- Kompatibilität
- Assignability
- Converter
- Adapter
- Versionierung
- Reflection

---

# 108. Performanz

Kompatibilitätsprüfungen sollen:

- cachefähig
- deterministisch
- parallelisierbar

sein.

---

# 109. Cache

Die Runtime darf Ergebnisse cachen.

Cache-Key:

```text
Source Type

Target Type

Version

Policy
```

---

# 110. Diagnostik

Compilerdiagnosen sollen enthalten:

```text
Expected Type

Actual Type

Compatibility Level

Suggested Converter
```

---

# 111. Fehlercodes

```text
TYPE_NOT_COMPATIBLE

SUBTYPE_REQUIRED

VERSION_INCOMPATIBLE

SCHEMA_MISMATCH

CONVERTER_REQUIRED

POLICY_DENIED

UNKNOWN_TYPE
```

---

# 112. Qualitätsanforderungen

Das Kompatibilitätssystem muss:

- deterministisch
- reproduzierbar
- stark typisiert
- versionierbar
- sicher
- performant

sein.

---

# 113. Konformitätsstufen

Nova definiert:

```text
Core

Standard

Complete
```

---

# 114. Core

Unterstützt:

- nominale Typen
- primitive Typen
- Reflection
- Assignability

---

# 115. Standard

Zusätzlich:

- Generics
- Interfaces
- Traits
- Versionierung
- Adapter

---

# 116. Complete

Zusätzlich:

- automatische Discovery
- Compatibility Graph
- Converter-Auswahl
- vollständige Reflection
- Policies
- Capability Contracts

---

# 117. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation, wenn:

- nominale und strukturelle Typkompatibilität unterstützt werden,
- Subtyping formal definiert ist,
- Varianzregeln umgesetzt werden,
- Generics korrekt geprüft werden,
- Versionen berücksichtigt werden,
- Reflection alle Typbeziehungen bereitstellt,
- Capability Contracts Typkompatibilität erzwingen,
- automatische Adapter und Converter kontrolliert eingesetzt werden,
- alle Entscheidungen deterministisch und reproduzierbar sind.

---

# 118. Zusammenfassung

Die **Typkompatibilität und das Subtyping-Modell** definieren die Grundlage für sichere Typbeziehungen innerhalb der Nova Platform.

Durch die klare Trennung zwischen Identität, Kompatibilität, Zuweisbarkeit und Konvertierbarkeit entsteht ein konsistentes Typsystem, das sowohl statische Compilerprüfungen als auch dynamische Runtime-Entscheidungen unterstützt.

Das Modell integriert nominales und strukturelles Subtyping, Varianzregeln, Generics, Versionierung, Capability Contracts sowie Reflection und bildet damit die Basis für langfristig stabile APIs, sichere Datenflüsse und eine kontrollierte Weiterentwicklung aller Nova-Komponenten.