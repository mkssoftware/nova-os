# NPSPEC-DATATYPE-SCHEMA-0001: Schemas und Typvalidierung

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Dokument-ID:** NPSPEC-DATATYPE-SCHEMA-0001
- **Titel:** Schemas und Typvalidierung
- **Version:** 1.0.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture / Datatype System
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform
- **Verantwortungsbereich:** Nova Type System
- **Normative Sprache:** Deutsch
- **Referenzen:**
  - NPSPEC-DATATYPE-0001
  - NPSPEC-DATATYPE-PRIMITIVE-0001
  - NPSPEC-DATATYPE-SEMANTIC-0001
  - NPSPEC-DATATYPE-COMPOSITE-0001
  - NPSPEC-DATATYPE-DOCUMENT-0001
  - NPSPEC-DATATYPE-STRUCTURED-0001
  - NPSPEC-DATATYPE-STREAM-0001
  - NPSPEC-DATAFLOW-0001
  - NPSPEC-CAPABILITY-CONTRACT-0001
  - NPSPEC-CAPABILITY-REGISTRY-0001
  - NPSPEC-CAPABILITY-VERSIONING-0001
  - NPSPEC-CAPABILITY-ERROR-0001
  - NPSPEC-CAPABILITY-OBSERVABILITY-0001
  - ADR-DATAFLOW-0002
  - ADR-DATAFLOW-0003
  - ADR-DATAFLOW-0005
  - ADR-DATAFLOW-0006

---

# 1. Zweck

Diese Spezifikation definiert das Schema- und Typvalidierungsmodell der Nova Platform.

Ein Schema beschreibt die maschinenlesbare Struktur, Semantik, Einschränkungen, Beziehungen und Kompatibilitätsregeln eines Datentyps.

Die Typvalidierung prüft, ob:

- ein Wert einem technischen Datentyp entspricht,
- ein Wert die deklarierte semantische Bedeutung erfüllt,
- eine zusammengesetzte Struktur vollständig und korrekt ist,
- Constraints eingehalten werden,
- Beziehungen gültig sind,
- Versionen kompatibel sind,
- Daten sicher verarbeitet werden dürfen,
- Capability Contracts erfüllt werden.

Schemas und Typvalidierung bilden damit die verbindliche Kontrollschicht zwischen Datenerzeugung, Speicherung, Übertragung und Verarbeitung.

---

# 2. Ziele

Das Schema- und Validierungsmodell verfolgt folgende Ziele:

- eindeutige Beschreibung aller Nova-Datentypen
- statische und dynamische Typprüfung
- deterministische Validierung
- maschinenlesbare Constraints
- kontrollierte Schemaevolution
- sichere Capability-Komposition
- frühzeitige Fehlererkennung
- präzise Fehlerdiagnosen
- vollständige Reflection
- sprach- und plattformunabhängige Schemas
- Schutz vor manipulierten oder beschädigten Daten
- Validierung großer und gestreamter Datenmengen
- semantische Interoperabilität
- langfristige Datenkompatibilität

---

# 3. Geltungsbereich

Diese Spezifikation gilt für:

- primitive Datentypen
- semantische Datentypen
- zusammengesetzte Datentypen
- Dokumenttypen
- strukturierte Datentypen
- Datenbanktypen
- Streaming-Datentypen
- Capability-Ein- und Ausgaben
- Konfigurationsdaten
- persistente Objekte
- Netzwerkdaten
- Nachrichten
- Ereignisse
- API-Daten
- KI-Daten
- wissenschaftliche Daten
- Robotikdaten
- Dateiformate
- Serialisierungen

---

# 4. Normative Begriffe

Die Begriffe **MUSS**, **DARF NICHT**, **SOLL**, **SOLL NICHT** und **KANN** werden normativ verwendet.

```text
MUSS

Eine verbindliche Anforderung.
```

```text
DARF NICHT

Ein verbindliches Verbot.
```

```text
SOLL

Eine empfohlene Anforderung, von der nur begründet abgewichen werden darf.
```

```text
SOLL NICHT

Eine im Regelfall zu vermeidende Umsetzung.
```

```text
KANN

Eine optionale Fähigkeit.
```

---

# 5. Grundprinzipien

Das Schema- und Validierungssystem basiert auf folgenden Prinzipien:

- Jeder veröffentlichte Datentyp besitzt ein Schema.
- Jedes Schema besitzt eine stabile Identität.
- Schemas sind unveränderlich.
- Änderungen erzeugen neue Schemaversionen.
- Typidentität und Struktur werden getrennt betrachtet.
- Validierung ist deterministisch.
- Validierung darf keine versteckten Nebenwirkungen besitzen.
- Constraints sind explizit und maschinenlesbar.
- Validierungsergebnisse sind strukturiert.
- Fehler werden mit eindeutigen Pfaden ausgegeben.
- Unbekannte Daten werden nicht automatisch als gültig angenommen.
- Schemaevolution erfolgt kontrolliert.
- Sicherheitsregeln sind Teil der Validierung.
- Semantische Bedeutung wird zusätzlich zur technischen Struktur geprüft.

---

# 6. Architekturübersicht

```text
Data Object
├── Declared Type
├── Technical Representation
├── Semantic Metadata
└── Value
        │
        ▼
Schema Resolver
        │
        ├── Type Registry
        ├── Schema Cache
        └── Version Resolver
        │
        ▼
Validation Pipeline
        ├── Structural Validation
        ├── Technical Type Validation
        ├── Semantic Validation
        ├── Constraint Validation
        ├── Relationship Validation
        ├── Security Validation
        ├── Version Validation
        └── Custom Invariant Validation
        │
        ▼
Validation Result
        ├── Valid
        ├── Valid With Warnings
        ├── Invalid
        └── Indeterminate
```

---

# 7. Schema

Ein Schema ist eine unveränderliche, maschinenlesbare Beschreibung eines Datentyps.

Ein Schema enthält mindestens:

```text
Schema ID

Type ID

Version

Kind

Technical Representation

Validation Rules

Compatibility Rules
```

Optional enthält es:

```text
Fields

Constraints

Semantic Types

Relations

Indexes

Invariants

Security Rules

Serialization Rules

Migration Information

Documentation
```

---

# 8. Schemaidentität

Jedes Schema besitzt eine global eindeutige Schema-ID.

Beispiele:

```text
nova.schema.core.person

nova.schema.document.markdown

nova.schema.stream.temperature

nova.schema.robotics.pose
```

Eine Schema-ID bezeichnet eine logische Schemafamilie.

Eine konkrete Version wird durch die Kombination aus Schema-ID und Versionsnummer identifiziert.

Beispiel:

```text
nova.schema.core.person@2.1.0
```

---

# 9. Type ID und Schema ID

Type ID und Schema ID erfüllen unterschiedliche Aufgaben.

```text
Type ID

Identifiziert die fachliche oder technische Bedeutung eines Datentyps.
```

```text
Schema ID

Identifiziert dessen konkrete maschinenlesbare Strukturdefinition.
```

Ein Datentyp kann mehrere Schemas besitzen, wenn verschiedene Repräsentationen unterstützt werden.

Beispiel:

```text
Type:

nova.document.markdown
```

```text
Schemas:

nova.schema.document.markdown-text@1

nova.schema.document.markdown-tree@1
```

---

# 10. Schemaversion

Jede veröffentlichte Schemaänderung erzeugt eine neue Version.

Nova verwendet:

```text
Major.Minor.Patch
```

Beispiel:

```text
2.3.1
```

Bedeutung:

```text
Major

Inkompatible Schemaänderung
```

```text
Minor

Abwärtskompatible Erweiterung
```

```text
Patch

Kompatible Korrektur ohne strukturelle Bedeutungsänderung
```

---

# 11. Unveränderlichkeit

Ein veröffentlichtes Schema DARF NICHT nachträglich verändert werden.

Nicht zulässig:

- Änderung eines Feldtyps unter gleicher Schemaversion
- Wiederverwendung einer Field ID
- Änderung einer Constraint-Bedeutung
- Änderung einer Enumeration
- Änderung einer Default-Semantik
- nachträgliche Umdeutung eines Feldes

Korrekturen erfordern eine neue Schemaversion.

---

# 12. Schemaarten

Nova unterscheidet folgende Schemaarten:

```text
Primitive Schema

Semantic Schema

Record Schema

Object Schema

Collection Schema

Variant Schema

Document Schema

Table Schema

Stream Schema

Message Schema

Configuration Schema

Reference Schema

Graph Schema

Database Schema
```

---

# 13. Primitive Schemas

Ein primitives Schema beschreibt einen atomaren Wert.

Beispiel:

```yaml
schema:
  id: nova.schema.core.uint32
  typeId: nova.core.uint32
  version: 1.0.0
  kind: primitive
  representation: UInt32
  constraints:
    minimum: 0
    maximum: 4294967295
```

Primitive Schemas besitzen keine Felder.

---

# 14. Semantische Schemas

Ein semantisches Schema ergänzt einen technischen Basistyp um fachliche Bedeutung.

Beispiel:

```yaml
schema:
  id: nova.schema.contact.email-address
  typeId: nova.core.contact.email-address
  version: 1.0.0
  kind: semantic
  baseType: String
  constraints:
    minimumLength: 3
    maximumLength: 320
    validator: nova.validator.email-address
```

---

# 15. Strukturelle Schemas

Strukturelle Schemas beschreiben zusammengesetzte Werte.

Sie definieren:

- Felder
- Feldtypen
- Feldidentitäten
- Pflichtfelder
- optionale Felder
- Standardwerte
- Feldconstraints
- Typinvarianten
- Verschachtelung
- Beziehungen

---

# 16. Nominale Validierung

Nominale Validierung prüft die deklarierte Typidentität.

Beispiel:

```text
Erwartet:

nova.core.customer-id
```

```text
Geliefert:

nova.core.device-id
```

Auch wenn beide technisch auf `UUID` basieren, ist die Zuweisung ungültig.

Nominale Validierung ist für öffentliche Capability Contracts der Standard.

---

# 17. Strukturelle Validierung

Strukturelle Validierung prüft die tatsächliche Form eines Wertes.

Geprüft werden:

- vorhandene Felder
- Feldtypen
- Verschachtelung
- Collection-Elemente
- Variant-Fälle
- Pflichtfelder
- zusätzliche Felder

Strukturelle Validierung darf nur verwendet werden, wenn sie durch das erwartete Schema erlaubt ist.

---

# 18. Validierungsstufen

Nova definiert folgende Validierungsstufen:

```text
None

Basic

Standard

Strict

Paranoid
```

## 18.1 None

Keine Validierung.

Diese Stufe ist ausschließlich für intern nachweislich vertrauenswürdige Daten zulässig.

## 18.2 Basic

Prüft:

- Typidentität
- grundlegende Repräsentation
- Pflichtfelder
- offensichtliche Formatfehler

## 18.3 Standard

Prüft zusätzlich:

- alle Constraints
- semantische Typen
- Beziehungen
- Versionen
- Referenzen

## 18.4 Strict

Prüft zusätzlich:

- unbekannte Felder
- kanonische Darstellung
- vollständige Invarianten
- Sicherheitsklassifikationen
- Provenance-Anforderungen

## 18.5 Paranoid

Prüft zusätzlich:

- kryptographische Integrität
- externe Referenzen
- Signaturen
- Ressourcenlimits
- vollständige Rekursion
- Herkunftsvertrauen
- Richtlinienkonformität

---

# 19. Validierungsphasen

Die Validierung erfolgt in einer festgelegten Reihenfolge.

```text
1. Eingabegrenzen
2. Schemaauflösung
3. Versionsprüfung
4. Technische Typprüfung
5. Strukturprüfung
6. Feldprüfung
7. Constraint-Prüfung
8. Semantische Prüfung
9. Beziehungsprüfung
10. Invariantenprüfung
11. Sicherheitsprüfung
12. Integritätsprüfung
13. Richtlinienprüfung
14. Ergebnisaggregation
```

Die Reihenfolge verhindert unnötige oder unsichere Folgeprüfungen.

---

# 20. Vorvalidierung

Die Vorvalidierung prüft Eingaben, bevor komplexe Parser oder Validatoren ausgeführt werden.

Geprüft werden:

- maximale Eingabegröße
- maximale Verschachtelungstiefe
- maximale Feldanzahl
- maximale Collection-Länge
- gültige Kodierung
- erlaubtes Format
- bekannte Schemareferenz

Vorvalidierung dient insbesondere dem Schutz vor Ressourcenerschöpfung.

---

# 21. Schemaauflösung

Die Schemaauflösung bestimmt das anzuwendende Schema.

Mögliche Quellen:

```text
Explizite Schema ID

Capability Contract

Data Object Metadata

Type Registry

Serialisierungsheader

Formatmanifest

Vertrauenswürdiger Importer
```

Eine Dateiendung oder ein unbestätigter Media Type reicht nicht als verbindliche Schemareferenz aus.

---

# 22. Auflösungsreihenfolge

Die Runtime SOLL folgende Priorität verwenden:

```text
1. Explizite, vertrauenswürdige Schema-ID
2. Capability Contract
3. Signierte Typmetadaten
4. Registrierter Serialisierungsheader
5. Validierte Formatdetektion
6. Allgemeines Fallback-Schema
```

Widersprüchliche Schemareferenzen erzeugen einen Fehler oder eine Warnung.

---

# 23. Schema-Registry

Alle veröffentlichten Schemas werden in der Nova Type Registry registriert.

Die Registry speichert:

```text
Schema ID

Type ID

Version

Schema Kind

Definition

Dependencies

Compatibility

Migration Paths

Validators

Documentation

Deprecation Status
```

Die Registry muss lokale und optional verteilte Auflösung unterstützen.

---

# 24. Schemaabhängigkeiten

Ein Schema kann andere Schemas referenzieren.

Beispiel:

```text
Person Schema
├── PersonID Schema
├── PersonName Schema
├── Address Schema
└── EmailAddress Schema
```

Abhängigkeiten müssen:

- explizit
- versioniert
- zyklusgeprüft
- auflösbar

sein.

---

# 25. Abhängigkeitsgraph

Die Registry verwaltet Schemaabhängigkeiten als gerichteten Graphen.

Dieser wird verwendet für:

- Kompatibilitätsanalyse
- Migration
- Cacheinvalidierung
- Auswirkungsanalyse
- Sicherheitsprüfung
- Build-Abhängigkeiten
- Deployment

Direkte eingebettete Schemazyklen sind nicht zulässig.

Zyklische fachliche Beziehungen müssen über Referenzschemas modelliert werden.

---

# 26. Schema-Dialekt

Nova Schemas besitzen einen definierten Schema-Dialekt.

Beispiel:

```yaml
schemaDialect: nova.schema/1.0
```

Der Dialekt bestimmt:

- verfügbare Schlüsselwörter
- Constraint-Semantik
- Ausdruckssyntax
- Versionsregeln
- Erweiterungsmechanismen

Eine Runtime muss unbekannte Dialekte ablehnen oder als nicht vollständig validierbar kennzeichnen.

---

# 27. Felder

Ein strukturelles Schema besteht aus Feldern.

Ein Feld enthält mindestens:

```text
Field ID

Name

Type

Required
```

Optional:

```text
Semantic Type

Default Value

Constraints

Documentation

Security Classification

Visibility

Deprecation

Aliases
```

---

# 28. Field ID

Jedes Feld besitzt innerhalb eines Schemas eine stabile Field ID.

Beispiel:

```yaml
id: 12
name: displayName
```

Die Field ID:

- bleibt bei Umbenennungen erhalten,
- darf nicht wiederverwendet werden,
- identifiziert das Feld in Binärformaten,
- unterstützt Schemaevolution,
- ist unabhängig von der Feldposition.

---

# 29. Feldnamen

Feldnamen müssen innerhalb ihres Gültigkeitsbereichs eindeutig sein.

Ein Feldname SOLL:

- lesbar
- stabil
- sprachneutral
- eindeutig
- frei von Darstellungsdetails

sein.

Umbenennungen werden über Aliasse und Schemaevolution unterstützt.

---

# 30. Pflichtfelder

Ein Feld kann folgende Verfügbarkeitsregeln besitzen:

```text
Required

Optional

Conditionally Required

Forbidden
```

## 30.1 Required

Das Feld muss vorhanden sein.

## 30.2 Optional

Das Feld kann fehlen.

## 30.3 Conditionally Required

Das Feld ist abhängig von einer deklarativen Bedingung verpflichtend.

## 30.4 Forbidden

Das Feld darf in einem bestimmten Kontext nicht vorhanden sein.

---

# 31. Nullability

Nullability und Optionalität sind getrennte Konzepte.

```text
Optional Field

Das Feld kann fehlen.
```

```text
Nullable Field

Das Feld ist vorhanden, kann jedoch Null enthalten.
```

Beispiel:

```text
Optional<Nullable<String>>
```

Dies erlaubt:

- Feld fehlt
- Feld enthält Null
- Feld enthält String

Schemas müssen diese Zustände eindeutig unterscheiden.

---

# 32. Standardwerte

Ein Schema kann Standardwerte definieren.

Standardwerte müssen:

- typkompatibel
- deterministisch
- unveränderlich
- frei von Laufzeitabhängigkeiten
- versionsstabil

sein.

Nicht zulässig als statischer Standardwert:

```text
Aktuelle Uhrzeit

Zufällige UUID

Aktueller Benutzer

Systemsprache

Externe Daten
```

Solche Werte müssen durch Initialisierungs-Capabilities erzeugt werden.

---

# 33. Technische Typvalidierung

Die technische Typvalidierung prüft die Speicher- oder Laufzeitrepräsentation.

Beispiele:

```text
String

UInt32

Float64

Bytes

Record

List<T>

Map<K, V>
```

Die Prüfung umfasst:

- Typkategorie
- Bitbreite
- Vorzeichen
- Endianness, sofern relevant
- Zeichenkodierung
- Collection-Typ
- generische Parameter
- Speichergrenzen

---

# 34. Semantische Typvalidierung

Die semantische Validierung prüft die fachliche Bedeutung eines Wertes.

Beispiel:

```text
Technical Type:

String
```

```text
Semantic Type:

EmailAddress
```

Ein technisch gültiger String ist nicht automatisch eine gültige E-Mail-Adresse.

Semantische Validierung kann umfassen:

- Format
- Wertebereich
- Einheit
- Dimension
- Referenzsystem
- fachliche Rolle
- Klassifikation
- zulässige Konvertierungen

---

# 35. Constraint-Modell

Constraints beschreiben Bedingungen, die ein Wert erfüllen muss.

Nova unterscheidet:

```text
Value Constraints

Structural Constraints

Relational Constraints

Semantic Constraints

Temporal Constraints

Security Constraints

Contextual Constraints
```

Constraints müssen:

- deterministisch
- maschinenlesbar
- testbar
- versionierbar
- nebenwirkungsfrei

sein.

---

# 36. Numerische Constraints

Unterstützte numerische Constraints umfassen:

```text
Minimum

Maximum

Exclusive Minimum

Exclusive Maximum

Multiple Of

Precision

Scale

Finite

Non-Zero

Positive

Negative
```

Beispiel:

```yaml
constraints:
  minimum: 0
  maximum: 100
  multipleOf: 0.1
```

---

# 37. Gleitkommawerte

Schemas müssen bei Gleitkommatypen definieren, ob folgende Werte erlaubt sind:

```text
NaN

Positive Infinity

Negative Infinity

Negative Zero

Subnormal Values
```

Beispiel:

```yaml
floatingPoint:
  allowNaN: false
  allowInfinity: false
  preserveNegativeZero: true
```

---

# 38. Dezimalwerte

Dezimaltypen können folgende Constraints besitzen:

```text
Precision

Scale

Rounding Mode

Minimum

Maximum
```

Beispiel:

```yaml
type: Decimal
constraints:
  precision: 18
  scale: 2
  roundingMode: half-even
```

Eine unbemerkte Reduzierung der Präzision ist nicht zulässig.

---

# 39. String-Constraints

Unterstützte String-Constraints:

```text
Minimum Length

Maximum Length

Pattern

Prefix

Suffix

Contains

Allowed Values

Normalization

Case Policy

Character Set

Language
```

Längenangaben müssen das verwendete Zählmodell definieren:

```text
Bytes

Code Units

Code Points

Grapheme Clusters
```

---

# 40. Pattern-Validierung

Pattern-Constraints verwenden eine definierte Ausdruckssyntax.

Beispiel:

```yaml
pattern:
  dialect: nova.regex/1
  expression: "^[A-Z]{2}-[0-9]{6}$"
```

Regex-Auswertungen müssen gegen übermäßige Laufzeit und Speicherverbrauch geschützt sein.

Nicht vertrauenswürdige reguläre Ausdrücke dürfen nicht unkontrolliert ausgeführt werden.

---

# 41. Binärdaten-Constraints

Binärwerte können folgende Constraints besitzen:

```text
Minimum Size

Maximum Size

Exact Size

Magic Bytes

Checksum

Content Hash

Media Type

Alignment

Compression
```

Beispiel:

```yaml
constraints:
  maximumSize: 16MiB
  magicBytes: "89504E470D0A1A0A"
  expectedMediaType: image/png
```

---

# 42. Collection-Constraints

Collections unterstützen:

```text
Minimum Length

Maximum Length

Exact Length

Unique Elements

Sorted

Element Constraints

Key Constraints

Value Constraints
```

Beispiel:

```yaml
type: List<PersonID>
constraints:
  minimumLength: 1
  maximumLength: 1000
  uniqueElements: true
```

---

# 43. Map-Validierung

Maps müssen prüfen:

- Schlüsseltyp
- Werttyp
- Schlüsseleindeutigkeit
- maximale Eintragszahl
- zulässige Schlüssel
- erforderliche Schlüssel
- verbotene Schlüssel
- kanonische Schlüsselreihenfolge

Nicht-String-Schlüssel müssen eine deterministische Serialisierung besitzen.

---

# 44. Record-Validierung

Bei Records werden geprüft:

- Schemaidentität
- Pflichtfelder
- Feldtypen
- Feldconstraints
- unbekannte Felder
- Feldabhängigkeiten
- Typinvarianten
- Verschachtelungstiefe

Die Validierung erfolgt rekursiv.

---

# 45. Object-Validierung

Objects benötigen zusätzlich:

- gültige Object ID
- gültige Objektversion
- erlaubten Lebenszykluszustand
- konsistente Identität
- Referenzintegrität
- Eigentumsinformationen
- Zugriffsrechte

Zwei Objekte mit gleichem Inhalt, aber unterschiedlichen Object IDs bleiben verschiedene Objekte.

---

# 46. Variant-Validierung

Ein Variant muss genau eine aktive Alternative besitzen.

Geprüft werden:

```text
Variant ID

Variant Name

Payload Schema

Required Payload

Forbidden Payloads
```

Ungültig:

- keine aktive Alternative
- mehrere aktive Alternativen
- unbekannte geschlossene Alternative
- Payload des falschen Typs

---

# 47. Enumeration-Validierung

Enumerationen prüfen:

- bekannten Eintrag
- stabile Value ID
- Aliasregeln
- offene oder geschlossene Enumeration
- unbekannte Werte

Bei geschlossenen Enumerationen sind unbekannte Werte ungültig.

Offene Enumerationen können unbekannte Werte erhalten, müssen sie jedoch als unbekannt kennzeichnen.

---

# 48. Referenzvalidierung

Referenzen werden auf mehreren Ebenen validiert:

```text
Reference Format

Target Existence

Target Type

Version Constraint

Access Rights

Integrity

Lifecycle
```

Je nach Kontext kann eine Referenzprüfung sein:

```text
Eager

Lazy

Deferred

Offline
```

Eine nicht aufgelöste Referenz ist nicht automatisch ungültig, wenn das Schema verzögerte Auflösung erlaubt.

---

# 49. Beziehungsvalidierung

Beziehungen zwischen Datenobjekten können folgende Kardinalitäten besitzen:

```text
One-to-One

One-to-Many

Many-to-One

Many-to-Many
```

Geprüft werden:

- Zieltyp
- Mindestkardinalität
- Maximalkardinalität
- Löschregeln
- Eigentum
- Zyklusregeln
- inverse Beziehungen
- Referenzintegrität

---

# 50. Invarianten

Invarianten sind Regeln, die mehrere Felder oder Strukturelemente gemeinsam betreffen.

Beispiel:

```text
startTime <= endTime
```

```text
Wenn encrypted = true,
muss encryptionAlgorithm gesetzt sein.
```

```text
subtotal + tax = total
```

Invarianten müssen deklarativ oder über registrierte reine Validatoren definiert werden.

---

# 51. Constraint-Ausdrücke

Nova kann deklarative Constraint-Ausdrücke unterstützen.

Beispiel:

```yaml
invariants:
  - id: interval-order
    expression: start <= end
    message: Das Startdatum darf nicht nach dem Enddatum liegen.
```

Constraint-Ausdrücke dürfen:

- keine Dateien öffnen,
- keine Netzwerkzugriffe ausführen,
- keine Systemzustände verändern,
- keine nicht deterministischen Werte erzeugen.

---

# 52. Kontextfreie Validierung

Kontextfreie Validierung verwendet ausschließlich:

- den zu prüfenden Wert,
- das Schema,
- eingebettete Metadaten.

Beispiele:

- Stringlänge
- Zahlenbereich
- Feldtyp
- Pflichtfeld
- Struktur
- Pattern

Kontextfreie Validierung ist reproduzierbar und cachefähig.

---

# 53. Kontextabhängige Validierung

Kontextabhängige Validierung benötigt zusätzliche Informationen.

Beispiele:

- Eindeutigkeit in einer Datenbank
- Existenz eines referenzierten Objekts
- Benutzerberechtigung
- Verfügbarkeit einer Capability
- Gültigkeit eines externen Zertifikats

Kontextabhängige Validatoren müssen ihre Abhängigkeiten deklarieren.

---

# 54. Validator-Capabilities

Komplexe Validierungen werden als registrierte Validator-Capabilities bereitgestellt.

Eine Validator-Capability deklariert:

```text
Validator ID

Version

Input Type

Supported Schema Versions

Determinism

Required Permissions

External Dependencies

Output Type
```

Validatoren dürfen einen Wert nicht verändern.

Normalisierung und Reparatur erfolgen über separate Transformations-Capabilities.

---

# 55. Reine Validatoren

Ein reiner Validator:

- besitzt keine Nebenwirkungen,
- verändert keine Daten,
- verwendet keine unkontrollierten externen Zustände,
- liefert bei identischer Eingabe dasselbe Ergebnis,
- ist parallel ausführbar,
- ist cachefähig.

Reine Validatoren SOLLEN bevorzugt werden.

---

# 56. Externe Validatoren

Externe Validatoren können benötigt werden für:

- Zertifikatsprüfung
- Datenbankeindeutigkeit
- Verzeichnisdienste
- externe Register
- Netzwerkressourcen

Sie müssen:

- Zeitlimits besitzen,
- Fehlerzustände unterscheiden,
- Netzwerkabhängigkeit deklarieren,
- Offlineverhalten definieren,
- Ergebnisse zeitlich begrenzen.

---

# 57. Validierungsergebnis

Eine Validierung liefert ein strukturiertes `ValidationResult`.

Mögliche Hauptzustände:

```text
Valid

ValidWithWarnings

Invalid

Indeterminate
```

`Indeterminate` wird verwendet, wenn eine verbindliche Prüfung nicht abgeschlossen werden konnte.

Beispiele:

- Schema nicht verfügbar
- Referenz offline
- externer Validator nicht erreichbar
- unbekannter Schema-Dialekt

---

# 58. ValidationResult-Struktur

Ein ValidationResult enthält mindestens:

```text
Status

Schema ID

Schema Version

Validated Type

Validation Level

Issues

Timestamp
```

Optional:

```text
Validated Object ID

Validated Object Version

Validator Versions

Duration

Cache Status

Security Context

Trace ID
```

---

# 59. Validierungsprobleme

Ein Validierungsproblem enthält:

```text
Issue ID

Severity

Error Code

Path

Expected

Actual

Message

Schema Reference
```

Optional:

```text
Suggestion

Related Paths

Source Location

Validator ID

Documentation Reference
```

---

# 60. Schweregrade

Nova definiert:

```text
Info

Hint

Warning

Error

Fatal
```

Bedeutung:

```text
Info

Reine Zusatzinformation.
```

```text
Hint

Nicht kritischer Verbesserungshinweis.
```

```text
Warning

Daten sind verwendbar, können aber problematisch sein.
```

```text
Error

Eine konkrete Regel ist verletzt.
```

```text
Fatal

Die Validierung kann nicht sicher fortgesetzt werden.
```

---

# 61. Standardisierte Fehlercodes

Beispiele:

```text
SCHEMA_NOT_FOUND

SCHEMA_VERSION_UNSUPPORTED

SCHEMA_DIALECT_UNKNOWN

TYPE_MISMATCH

SEMANTIC_TYPE_MISMATCH

FIELD_REQUIRED

FIELD_FORBIDDEN

FIELD_UNKNOWN

FIELD_TYPE_MISMATCH

VALUE_OUT_OF_RANGE

VALUE_PATTERN_MISMATCH

VALUE_LENGTH_INVALID

COLLECTION_LENGTH_INVALID

COLLECTION_DUPLICATE_ELEMENT

REFERENCE_INVALID

REFERENCE_TARGET_MISSING

RELATION_CARDINALITY_INVALID

INVARIANT_VIOLATION

ENUM_VALUE_UNKNOWN

VARIANT_INVALID

VERSION_INCOMPATIBLE

VALIDATOR_UNAVAILABLE

VALIDATION_LIMIT_EXCEEDED

SECURITY_CLASSIFICATION_INVALID

INTEGRITY_CHECK_FAILED
```

---

# 62. Datenpfade

Validierungsprobleme verwenden standardisierte Datenpfade.

Beispiele:

```text
person.address.city
```

```text
orders[4].items[2].quantity
```

```text
settings["renderer"].quality
```

```text
shape.Circle.radius
```

Ein Pfad muss eindeutig auf den betroffenen Datenbereich verweisen.

---

# 63. Quellpositionen

Bei textbasierten Serialisierungen kann ein Fehler zusätzlich eine Quellposition enthalten.

Beispiel:

```text
Datei: config.yaml

Zeile: 42

Spalte: 13
```

Bei Binärformaten können angegeben werden:

```text
Byte Offset

Chunk ID

Field ID

Record Offset
```

---

# 64. Fehleraggregation

Validatoren können mehrere Probleme in einem Durchlauf sammeln.

Unterstützte Modi:

```text
Fail Fast

Collect Errors

Collect All

Threshold
```

## 64.1 Fail Fast

Beendet die Prüfung beim ersten Fehler.

## 64.2 Collect Errors

Sammelt alle Fehler, überspringt jedoch nachgelagerte Prüfungen mit ungültigen Voraussetzungen.

## 64.3 Collect All

Versucht, möglichst viele Probleme zu erfassen.

## 64.4 Threshold

Beendet die Prüfung nach einer festgelegten Anzahl von Problemen.

---

# 65. Fehlerkaskaden

Die Runtime muss redundante Folgefehler vermeiden.

Beispiel:

Ist ein Pflichtfeld nicht vorhanden, sollen nicht zusätzlich zahlreiche Fehler für Unterfelder dieses fehlenden Feldes erzeugt werden.

Validierungsprobleme können eine Ursache referenzieren.

```text
Root Issue
↓
Dependent Issues
```

---

# 66. Warnungen

Warnungen können auf gültige, aber problematische Daten hinweisen.

Beispiele:

- veraltete Schemaversion
- deprecated Feld
- unübliche Kodierung
- potenzieller Präzisionsverlust
- unbekanntes optionales Feld
- nicht kanonische Darstellung
- fehlende optionale Provenance

Warnungen dürfen nicht stillschweigend in Fehler umgewandelt werden, außer ein Contract verlangt dies ausdrücklich.

---

# 67. Unknown-Field-Richtlinien

Schemas definieren den Umgang mit unbekannten Feldern.

```text
Reject

Ignore

Preserve

Capture
```

## 67.1 Reject

Unbekannte Felder führen zu einem Fehler.

## 67.2 Ignore

Unbekannte Felder werden bei der Verarbeitung ignoriert.

## 67.3 Preserve

Unbekannte Felder werden unverändert erhalten.

## 67.4 Capture

Unbekannte Felder werden in einem definierten Erweiterungsbereich gespeichert.

Für langfristig versionierte Daten SOLL `Preserve` oder `Capture` verwendet werden.

---

# 68. Offene und geschlossene Schemas

Ein Schema kann sein:

```text
Closed

Open

Extensible
```

## 68.1 Closed

Nur deklarierte Felder und Fälle sind zulässig.

## 68.2 Open

Beliebige zusätzliche Felder sind zulässig.

## 68.3 Extensible

Zusätzliche Felder sind ausschließlich in registrierten Namespaces oder Erweiterungsbereichen zulässig.

Sicherheitskritische Schemas SOLLEN geschlossen oder kontrolliert erweiterbar sein.

---

# 69. Erweiterungsfelder

Erweiterungen müssen namespaced sein.

Beispiel:

```yaml
extensions:
  org.example.machine:
    calibrationMode: automatic
```

Erweiterungen dürfen:

- Kernfelder nicht überschreiben,
- keine Field IDs des Kernschemas wiederverwenden,
- Sicherheitsregeln nicht abschwächen,
- bestehende Semantik nicht umdeuten.

---

# 70. Schema-Kompatibilität

Nova unterscheidet:

```text
Identical

Backward Compatible

Forward Compatible

Fully Compatible

Conditionally Compatible

Incompatible
```

---

# 71. Rückwärtskompatibilität

Ein neues Schema ist rückwärtskompatibel, wenn Daten des alten Schemas vom neuen Schema verarbeitet werden können.

Typische kompatible Änderungen:

- Hinzufügen optionaler Felder
- Hinzufügen von Feldern mit sicheren Standardwerten
- Ergänzen nicht verschärfender Metadaten
- Hinzufügen offener Variant-Fälle
- Erweiterung zulässiger Wertebereiche

---

# 72. Vorwärtskompatibilität

Ein altes Schema ist vorwärtskompatibel, wenn es Daten des neuen Schemas ausreichend verarbeiten kann.

Dies setzt häufig voraus:

- unbekannte Felder werden erhalten oder ignoriert,
- neue optionale Felder sind nicht zwingend erforderlich,
- Enumerationserweiterungen sind offen,
- Erweiterungsbereiche sind vorgesehen.

---

# 73. Vollständige Kompatibilität

Vollständige Kompatibilität besteht, wenn Vorwärts- und Rückwärtskompatibilität gleichzeitig erfüllt sind.

Dies ist besonders wichtig für:

- verteilte Systeme
- Rolling Updates
- langfristige Datenspeicherung
- Capability-Komposition
- Netzwerkprotokolle

---

# 74. Inkompatible Änderungen

Inkompatible Änderungen umfassen unter anderem:

- Entfernen eines Pflichtfelds
- Hinzufügen eines Pflichtfelds ohne Standardwert
- Ändern eines Feldtyps
- Ändern einer Field ID
- Wiederverwenden einer Field ID
- Verengen eines gültigen Wertebereichs
- Ändern einer semantischen Bedeutung
- Ändern einer geschlossenen Enumeration
- Ändern einer Invariante
- Änderung der Nullability
- Änderung der Variant-Bedeutung

---

# 75. Schemaevolution

Schemaevolution erfolgt kontrolliert durch:

```text
Field Addition

Field Deprecation

Field Alias

Default Value

Schema Adapter

Migration Capability

Version Bridge

Compatibility Profile
```

Jede Änderung muss eine dokumentierte Kompatibilitätsklassifikation besitzen.

---

# 76. Felddeprecation

Ein veraltetes Feld wird markiert mit:

```text
Deprecated Since

Reason

Replacement Field

Removal Version

Migration Guidance
```

Ein deprecated Feld bleibt bis zu einer definierten inkompatiblen Hauptversion gültig.

---

# 77. Feldumbenennung

Eine Feldumbenennung verändert nicht die Field ID.

Beispiel:

```yaml
id: 4
name: displayName
aliases:
  - fullName
```

Serialisierungen, die Feldnamen statt Field IDs verwenden, müssen Aliasse berücksichtigen können.

---

# 78. Schemaadapter

Ein Schemaadapter stellt eine kompatible Sicht auf Daten bereit.

Beispiel:

```text
Person@2

↓

Schema Adapter

↓

Person@1 View
```

Ein Adapter:

- verändert das Ursprungsobjekt nicht,
- dokumentiert Informationsverlust,
- ist versioniert,
- kann lazy ausgeführt werden.

---

# 79. Migration

Eine Migration erzeugt eine neue Datenrepräsentation.

Beispiel:

```text
Customer@1
↓
Migration Capability
↓
Customer@2
```

Eine Migration muss deklarieren:

```text
Source Schema

Target Schema

Transformation Rules

Loss Classification

Rollback Support

Validator
```

---

# 80. Migrationsvalidierung

Nach jeder Migration MUSS das Ergebnis gegen das Zielschema validiert werden.

Zusätzlich SOLL geprüft werden:

- Erhaltung relevanter Identitäten
- Referenzintegrität
- semantische Gleichwertigkeit
- Pflichtfelder
- Provenance
- Sicherheitsklassifikationen

---

# 81. Versionsbereiche

Capability Contracts können Schema-Versionsbereiche angeben.

Beispiel:

```text
>=1.2.0 <2.0.0
```

oder:

```text
^3.1
```

Die konkrete Syntax muss durch einen registrierten Versionsdialekt definiert sein.

---

# 82. Validierung in Capability Contracts

Ein Capability Contract muss festlegen:

```text
Input Type

Input Schema

Accepted Versions

Validation Level

Unknown Field Policy

Required Traits

Output Schema

Error Policy
```

Vor Ausführung einer Capability muss die Runtime die Eingaben entsprechend validieren.

---

# 83. Eingabevalidierung

Capability-Eingaben werden geprüft auf:

- Typidentität
- Schemaidentität
- Version
- Constraints
- Berechtigungen
- Größenlimits
- Referenzen
- Sicherheitsklassifikation

Ungültige Eingaben dürfen nicht an die Capability weitergegeben werden.

---

# 84. Ausgabevalidierung

Capability-Ausgaben müssen gegen den deklarierten Output Contract validiert werden.

Eine Capability darf keine Daten als gültigen Output veröffentlichen, wenn:

- der Typ nicht stimmt,
- Pflichtfelder fehlen,
- Constraints verletzt werden,
- Sicherheitsklassifikationen verloren gingen,
- das Schema nicht dem Contract entspricht.

Ausgabevalidierung kann in Produktionsumgebungen optimiert werden, darf aber an Vertrauensgrenzen nicht vollständig entfallen.

---

# 85. Vertrauensgrenzen

Vollständige Validierung ist insbesondere erforderlich bei:

- Netzwerkempfang
- Dateieingaben
- Benutzerinhalten
- Plugin-Ausgaben
- externen Capabilities
- importierten Datenbanken
- Geräteeingaben
- KI-generierten Strukturen
- kryptographisch nicht bestätigten Daten

Interne Daten dürfen nur dann reduziert validiert werden, wenn ihre Herkunft und Transformationskette nachweislich vertrauenswürdig ist.

---

# 86. Datenbankvalidierung

Datenbankschemas können zusätzlich definieren:

```text
Primary Keys

Foreign Keys

Unique Constraints

Check Constraints

Indexes

Generated Fields

Transactions

Referential Actions
```

Validierung kann erfolgen:

```text
Before Insert

Before Update

At Commit

Deferred

During Migration
```

---

# 87. Transaktionsvalidierung

Bestimmte Invarianten können nur auf Transaktionsebene geprüft werden.

Beispiele:

- Eindeutigkeit über mehrere Datensätze
- ausgeglichene Buchungssumme
- konsistente Lagerbestände
- vollständige Mehrfachbeziehungen

Eine Transaktion darf erst bestätigt werden, wenn alle verpflichtenden Validierungen erfolgreich sind.

---

# 88. Dokumentvalidierung

Dokumentschemas prüfen zusätzlich:

- gültige Knotentypen
- erlaubte Verschachtelung
- eindeutige Node IDs
- interne Verweise
- Ressourcenbeziehungen
- aktive Inhalte
- Barrierefreiheitsanforderungen
- Formatfeatures
- Zeichenkodierung

---

# 89. Streaming-Validierung

Streams können auf mehreren Ebenen validiert werden:

```text
Stream Contract

Stream Metadata

Element Validation

Sequence Validation

Temporal Validation

Window Validation

Aggregate Validation
```

Ein ungültiges Element muss entsprechend der Streamrichtlinie:

- den Stream beenden,
- verworfen,
- isoliert,
- in einen Fehlerstream umgeleitet,
- oder als ungültig markiert

werden.

---

# 90. Inkrementelle Validierung

Große Datenstrukturen und Streams sollen inkrementell validiert werden.

Dabei werden nur:

- neue Elemente,
- veränderte Felder,
- betroffene Teilbäume,
- geänderte Beziehungen

erneut geprüft.

Die Runtime muss sicherstellen, dass inkrementelle Validierung zum gleichen Ergebnis wie eine vollständige Validierung führt.

---

# 91. Partielle Validierung

Partielle Validierung prüft einen klar definierten Ausschnitt.

Beispiele:

```text
Ein einzelnes Feld

Ein Dokumentkapitel

Eine Tabellenpartition

Ein Streamfenster

Ein Objektteilbaum
```

Das Ergebnis muss den geprüften Bereich eindeutig angeben.

Ein partielles Ergebnis darf nicht als vollständige Objektvalidierung dargestellt werden.

---

# 92. Lazy Validation

Bei Lazy Validation werden Prüfungen erst beim Zugriff ausgeführt.

Zulässige Anwendungsfälle:

- große Dokumente
- entfernte Ressourcen
- datenbankgestützte Collections
- Memory-Mapped Data
- Streaming
- selten verwendete optionale Bereiche

Kritische Struktur- und Sicherheitsprüfungen dürfen nicht beliebig verzögert werden.

---

# 93. Cachefähige Validierung

Validierungsergebnisse können gecacht werden, wenn:

- das Objekt unveränderlich ist,
- die Objektversion eindeutig ist,
- das Schema unverändert ist,
- alle Validatorversionen bekannt sind,
- der Sicherheitskontext kompatibel ist,
- keine zeitabhängigen externen Prüfungen vorliegen.

Cache-Schlüssel enthalten mindestens:

```text
Object Hash or Version

Schema ID

Schema Version

Validation Level

Validator Versions
```

---

# 94. Validierungshash

Ein optionaler Validierungshash kann bestätigen, dass ein bestimmtes Objekt gegen ein bestimmtes Schema geprüft wurde.

Beispiel:

```text
Hash(
    ObjectContent,
    SchemaID,
    SchemaVersion,
    ValidatorVersions,
    ValidationLevel
)
```

Ein Validierungshash ersetzt keine kryptographische Signatur, sofern keine vertrauenswürdige Signatur darüber vorliegt.

---

# 95. Kanonische Validierung

Kanonische Validierung prüft zusätzlich, ob Daten in einer eindeutig definierten Normalform vorliegen.

Beispiele:

- kanonische Feldreihenfolge
- normalisierte Zahlen
- definierte Unicode-Normalisierung
- sortierte Map-Schlüssel
- keine redundanten Standardwerte
- eindeutige Zeitdarstellung

Kanonische Form ist erforderlich für:

- Hashing
- Signaturen
- Content Addressing
- deterministische Builds
- Replikation

---

# 96. Normalisierung

Normalisierung und Validierung sind getrennte Operationen.

```text
Validation

Prüft, ob ein Wert gültig ist.
```

```text
Normalization

Erzeugt eine standardisierte Repräsentation.
```

Ein Validator darf einen Wert nicht stillschweigend normalisieren.

Normalisierung erfolgt über eine explizite Transformations-Capability.

---

# 97. Reparatur

Reparatur versucht, ungültige Daten in gültige Daten zu überführen.

Beispiele:

- fehlenden Standardwert einsetzen
- ungültige Kodierung ersetzen
- beschädigte Referenz entfernen
- veraltete Struktur migrieren

Eine Reparatur:

- erzeugt ein neues Datenobjekt,
- dokumentiert jede Änderung,
- darf das Original nicht überschreiben,
- muss das Ergebnis erneut validieren,
- muss mögliche Informationsverluste kennzeichnen.

---

# 98. Sicherheitsvalidierung

Sicherheitsvalidierung prüft:

- Zugriffsrechte
- Sicherheitsklassifikation
- erlaubte Datenflüsse
- Signaturen
- Integrität
- Verschlüsselungsanforderungen
- verbotene aktive Inhalte
- Vertrauensstufe der Quelle
- Ausführungsrichtlinien

Ein strukturell gültiges Objekt kann sicherheitstechnisch ungültig sein.

---

# 99. Datenschutzvalidierung

Datenschutzvalidierung kann prüfen:

- Kennzeichnung personenbezogener Felder
- zulässigen Verarbeitungszweck
- Aufbewahrungsrichtlinien
- Exportberechtigungen
- Anonymisierungsstatus
- Pseudonymisierung
- KI-Verarbeitungsrichtlinien
- Protokollierungsverbote

Datenschutzklassifikationen dürfen durch Transformationen nicht unbemerkt entfernt werden.

---

# 100. Provenance-Validierung

Provenance-Validierung prüft:

- Herkunftsreferenz
- Transformationskette
- Capability-Versionen
- Migrationspfade
- Zeitstempel
- Signaturen
- Integrität
- Vollständigkeit verpflichtender Herkunftsinformationen

Fehlende Provenance kann abhängig vom Schema eine Warnung oder einen Fehler erzeugen.

---

# 101. Integritätsvalidierung

Integritätsprüfungen können umfassen:

```text
Checksum

Cryptographic Hash

Merkle Proof

Digital Signature

Content Address

Chunk Integrity
```

Die geprüfte Repräsentation muss eindeutig definiert sein.

Ein Hash über eine nicht kanonische Darstellung darf nicht als universeller Inhaltsnachweis verwendet werden.

---

# 102. KI-generierte Daten

KI-generierte strukturierte Daten müssen gegen das gleiche Schema validiert werden wie manuell oder deterministisch erzeugte Daten.

KI-Ausgaben dürfen nicht aufgrund ihrer syntaktischen Plausibilität als gültig angenommen werden.

Zusätzliche Prüfungen können umfassen:

- erlaubte Werte
- Referenzexistenz
- logische Konsistenz
- Sicherheitsklassifikation
- Halluzinationsindikatoren
- Quellenreferenzen
- Konfidenzwerte

---

# 103. Unsicherheitswerte

Validierungs- oder Analyseergebnisse können Unsicherheitswerte besitzen.

Beispiel:

```yaml
confidence: 0.87
```

Ein Unsicherheitswert ersetzt keinen binären Validierungsstatus.

Beispiel:

```text
Status:

Indeterminate

Confidence:

0.87
```

Dies darf nicht als `Valid` interpretiert werden.

---

# 104. Lokalisierung von Fehlermeldungen

Fehlercodes und technische Daten bleiben sprachneutral.

Menschenlesbare Fehlermeldungen können lokalisiert werden.

Beispiel:

```text
Error Code:

FIELD_REQUIRED
```

```text
Deutsch:

Das Pflichtfeld „email“ fehlt.
```

```text
Englisch:

The required field “email” is missing.
```

Die Logik darf nicht von lokalisierten Meldungstexten abhängen.

---

# 105. Entwicklerdiagnosen

Entwicklerdiagnosen SOLLEN enthalten:

- Fehlercode
- Schema-ID
- Schema-Version
- Datenpfad
- erwarteten Typ
- tatsächlichen Typ
- Constraint
- Validator
- mögliche Korrektur
- Quellposition

Beispiel:

```text
TYPE_MISMATCH at project.members[3].age

Expected:

UInt16

Actual:

String

Schema:

nova.schema.project.member@1.2.0
```

---

# 106. Benutzermeldungen

Benutzerorientierte Meldungen sollen:

- verständlich
- konkret
- handlungsorientiert
- frei von internen Speicherinformationen
- datenschutzgerecht

sein.

Beispiel:

```text
Das eingegebene Geburtsdatum liegt in der Zukunft.
Bitte prüfen Sie Ihre Eingabe.
```

Interne Stacktraces dürfen nicht ungefiltert angezeigt werden.

---

# 107. Schema-Reflection

Reflection muss folgende Informationen bereitstellen:

```text
Schema ID

Type ID

Version

Schema Kind

Fields

Field IDs

Field Types

Constraints

Invariants

Relations

Compatibility

Validators

Migrations

Documentation
```

Reflection muss möglich sein, ohne Beispielinstanzen des Typs zu erzeugen.

---

# 108. Schema-Discovery

Schema-Discovery ermöglicht Abfragen wie:

```text
Finde das aktuelle Schema für Person.
```

```text
Finde alle Schemas, die TemperatureMeasurement erweitern.
```

```text
Finde einen kompatiblen Adapter von Version 1 nach Version 3.
```

Discovery berücksichtigt:

- Type ID
- Schema-ID
- Version
- Semantik
- Kompatibilität
- Namensraum
- Deprecation
- Vertrauensstatus

---

# 109. Schemadokumentation

Jedes öffentliche Schema SOLL Dokumentation enthalten für:

- Zweck
- fachliche Bedeutung
- Felder
- Constraints
- Beispiele
- Versionierung
- Migration
- Sicherheitsanforderungen
- Fehlerfälle

Dokumentation ergänzt das maschinenlesbare Schema, ersetzt es aber nicht.

---

# 110. Schema-Publishing

Vor der Veröffentlichung muss ein Schema:

- syntaktisch gültig sein,
- alle Abhängigkeiten auflösen,
- eindeutige IDs besitzen,
- Kompatibilität deklarieren,
- Validierungstests bestehen,
- Sicherheitsprüfung bestehen,
- Dokumentation enthalten,
- signiert oder vertrauenswürdig registriert werden.

---

# 111. Schema-Signaturen

Schemas können digital signiert werden.

Eine Signatur bestätigt:

- Herausgeber
- Schema-ID
- Version
- unveränderten Inhalt
- Zeitpunkt
- Vertrauenskette

Nicht vertrauenswürdige Schemas dürfen abhängig von der Systemrichtlinie nur in isolierten Namensräumen verwendet werden.

---

# 112. Schema-Namensräume

Kernschemas verwenden Namensräume wie:

```text
nova.core

nova.document

nova.media

nova.compute

nova.robotics

nova.security
```

Drittanbieter verwenden eigene Namensräume.

Beispiel:

```text
org.example.facility
```

Ein Drittanbieter darf keine Schema-ID im reservierten `nova.*`-Namensraum veröffentlichen.

---

# 113. Schemaimporte

Schemas können andere Schemas importieren.

Beispiel:

```yaml
imports:
  - schema: nova.schema.core.identifier
    version: "^1.0"
  - schema: nova.schema.core.timestamp
    version: "^2.1"
```

Importe müssen:

- eindeutig auflösbar,
- versionsbeschränkt,
- zyklusfrei,
- sicherheitsgeprüft

sein.

---

# 114. Offline-Schemaauflösung

Die Nova Runtime muss Kernschemas lokal auflösen können.

Für externe Schemas können Strategien definiert werden:

```text
Local Cache

Embedded Schema

Signed Package

Remote Registry

Reject When Offline
```

Netzwerkzugriff darf nicht zwingende Voraussetzung für die Validierung grundlegender Systemdaten sein.

---

# 115. Schema-Caching

Schemas können lokal gecacht werden.

Der Cache muss berücksichtigen:

- Schema-ID
- Version
- Signatur
- Inhalts-Hash
- Vertrauensstatus
- Ablaufdatum bei dynamischen Registern

Ein Schema darf unter gleicher ID und Version nicht durch einen anderen Inhalt ersetzt werden.

---

# 116. Ressourcenlimits

Die Validierungsruntime muss Grenzen unterstützen für:

```text
Maximum Input Size

Maximum Nesting Depth

Maximum Field Count

Maximum Collection Length

Maximum Error Count

Maximum Validation Time

Maximum Regex Complexity

Maximum Reference Depth

Maximum Schema Dependency Depth
```

Das Überschreiten einer Grenze erzeugt:

```text
VALIDATION_LIMIT_EXCEEDED
```

---

# 117. Zeitlimits

Validatoren können Zeitlimits besitzen.

Bei Überschreitung wird unterschieden zwischen:

```text
Validation Timeout

External Dependency Timeout

Schema Resolution Timeout
```

Ein Timeout führt im Regelfall zu `Indeterminate`, nicht zu `Valid`.

---

# 118. Speicherlimits

Validatoren müssen gegen Speichererschöpfung geschützt sein.

Besonders zu prüfen sind:

- stark verschachtelte Objekte
- komprimierte Daten
- große Strings
- riesige Collections
- rekursive Referenzen
- viele gleichzeitige Fehler
- komplexe reguläre Ausdrücke

---

# 119. Parallelisierung

Unabhängige Validierungsaufgaben können parallel ausgeführt werden.

Beispiele:

- unterschiedliche Felder
- Collection-Elemente
- Dokumentseiten
- Tabellenpartitionen
- Streamfenster

Die Parallelisierung darf die Reihenfolge und Reproduzierbarkeit des endgültigen ValidationResult nicht beeinflussen.

---

# 120. Determinismus

Bei identischen:

- Eingabedaten,
- Schemaversionen,
- Validatorversionen,
- Validierungsstufen,
- Kontextinformationen

muss dasselbe Validierungsergebnis entstehen.

Nicht deterministische Prüfungen müssen ausdrücklich gekennzeichnet sein.

---

# 121. Observability

Die Validierungsruntime kann folgende Metriken erfassen:

```text
Validation Count

Validation Duration

Validation Failure Rate

Schema Cache Hit Rate

Validator Cache Hit Rate

Issues per Schema

Timeout Count

Migration Validation Count

Incremental Validation Ratio
```

Geschützte Dateninhalte dürfen nicht in allgemeiner Telemetrie enthalten sein.

---

# 122. Audit

Sicherheitskritische Validierungen können auditierbar sein.

Ein Auditeintrag kann enthalten:

```text
Validation ID

Schema ID

Object ID

Object Version

Validator Versions

Result

Timestamp

Security Context

Trace ID
```

Geheime Werte und personenbezogene Inhalte dürfen nicht unnötig protokolliert werden.

---

# 123. YAML-Beispiel eines Record-Schemas

```yaml
schema:
  schemaDialect: nova.schema/1.0
  id: nova.schema.core.person
  typeId: nova.core.person
  version: 1.0.0
  kind: record
  compatibility:
    unknownFields: preserve
    structuralMatching: false
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
      constraints:
        minimumLength: 1
        maximumLength: 100
    - id: 3
      name: lastName
      type: String
      semanticType: nova.core.person-name
      required: true
      constraints:
        minimumLength: 1
        maximumLength: 100
    - id: 4
      name: email
      type: Optional<nova.core.contact.email-address>
      required: false
    - id: 5
      name: birthDate
      type: Optional<Date>
      required: false
      constraints:
        maximumExpression: currentDate
  security:
    classification: personal-data
```

---

# 124. YAML-Beispiel mit Invarianten

```yaml
schema:
  schemaDialect: nova.schema/1.0
  id: nova.schema.core.time-interval
  typeId: nova.core.time-interval
  version: 1.0.0
  kind: record
  fields:
    - id: 1
      name: start
      type: Instant
      required: true
    - id: 2
      name: end
      type: Instant
      required: true
  invariants:
    - id: start-before-end
      expression: start <= end
      severity: error
      errorCode: INVARIANT_VIOLATION
      messageKey: interval.startMustNotFollowEnd
```

---

# 125. YAML-Beispiel eines ValidationResult

```yaml
validationResult:
  status: invalid
  schema:
    id: nova.schema.core.person
    version: 1.0.0
  validationLevel: standard
  validatedType: nova.core.person
  issues:
    - issueId: issue-0001
      severity: error
      errorCode: FIELD_REQUIRED
      path: email
      expected: nova.core.contact.email-address
      actual: missing
      message: Das Pflichtfeld „email“ fehlt.
    - issueId: issue-0002
      severity: error
      errorCode: VALUE_OUT_OF_RANGE
      path: birthDate
      expected: Datum nicht in der Zukunft
      actual: 2032-04-17
      message: Das Geburtsdatum darf nicht in der Zukunft liegen.
  statistics:
    durationMilliseconds: 2.4
    checkedFields: 5
    errors: 2
    warnings: 0
```

---

# 126. YAML-Beispiel einer Schemaevolution

```yaml
schemaEvolution:
  schemaId: nova.schema.core.person
  fromVersion: 1.0.0
  toVersion: 1.1.0
  compatibility:
    backward: true
    forward: true
  changes:
    - operation: addField
      field:
        id: 6
        name: preferredName
        type: Optional<String>
        required: false
  migrationRequired: false
```

---

# 127. YAML-Beispiel einer inkompatiblen Migration

```yaml
schemaMigration:
  id: nova.migration.person-v1-to-v2
  source:
    schema: nova.schema.core.person
    version: "^1.0"
  target:
    schema: nova.schema.core.person
    version: "2.0.0"
  capability:
    id: nova.capability.migrate-person-v1-v2
    version: 1.0.0
  changes:
    - splitField:
        source: fullName
        targets:
          - firstName
          - lastName
  lossClassification: potentially-lossy
  targetValidation:
    level: strict
    required: true
```

---

# 128. YAML-Beispiel eines Capability Contracts

```yaml
contract:
  id: nova.contract.person-import
  version: 1.0.0
  input:
    type: nova.core.person
    schema:
      id: nova.schema.core.person
      versions: ">=1.0.0 <2.0.0"
    validation:
      level: strict
      unknownFields: preserve
  output:
    type: nova.core.import-result
    schema:
      id: nova.schema.core.import-result
      versions: "^1.0"
    validation:
      level: standard
  errors:
    - SCHEMA_VERSION_UNSUPPORTED
    - TYPE_MISMATCH
    - FIELD_REQUIRED
    - INVARIANT_VIOLATION
```

---

# 129. Schema-Validierungsregeln

Ein Schema selbst gilt als gültig, wenn:

- eine eindeutige Schema-ID vorhanden ist,
- eine gültige Version angegeben ist,
- ein bekannter Schema-Dialekt verwendet wird,
- eine Type ID vorhanden ist,
- die Schemaart gültig ist,
- alle Field IDs eindeutig sind,
- alle Feldnamen eindeutig sind,
- alle referenzierten Typen auflösbar sind,
- alle Schemaabhängigkeiten gültig sind,
- keine unzulässigen Zyklen existieren,
- Constraints syntaktisch und semantisch gültig sind,
- Invarianten deterministisch sind,
- Kompatibilitätsregeln angegeben sind,
- Sicherheitsanforderungen widerspruchsfrei sind,
- Erweiterungen gültige Namensräume verwenden.

---

# 130. Instanzvalidierungsregeln

Eine Dateninstanz gilt als gültig, wenn:

- das verwendete Schema auflösbar ist,
- Typ und Schema kompatibel sind,
- die Schemaversion unterstützt wird,
- die technische Repräsentation gültig ist,
- alle Pflichtfelder vorhanden sind,
- alle Felder typkompatibel sind,
- alle Constraints erfüllt sind,
- alle Invarianten erfüllt sind,
- alle verpflichtenden Referenzen gültig sind,
- Beziehungen konsistent sind,
- Sicherheitsrichtlinien erfüllt sind,
- keine Validierungsgrenze überschritten wurde.

---

# 131. Laufzeitanforderungen

Die Nova Runtime MUSS:

- Schemas dynamisch auflösen können,
- Schemaabhängigkeiten verwalten,
- Schemaversionen vergleichen,
- technische und semantische Typen validieren,
- Constraints ausführen,
- strukturierte Fehlerergebnisse erzeugen,
- Capability-Eingaben und -Ausgaben validieren,
- inkrementelle Validierung unterstützen,
- Ressourcenlimits erzwingen,
- Validatorversionen berücksichtigen,
- Sicherheitsklassifikationen erhalten,
- ValidationResult-Objekte bereitstellen.

---

# 132. Compileranforderungen

Der Nova-Compiler MUSS für statisch bekannte Schemas:

- Typfehler vor der Laufzeit erkennen,
- Pflichtfelder prüfen,
- Feldzugriffe validieren,
- Constraint-Ausdrücke typprüfen,
- Variant-Vollständigkeit analysieren,
- inkompatible Schemaübergänge erkennen,
- verfügbare Adapter oder Migrationen vorschlagen,
- Versionsbereiche prüfen,
- aussagekräftige Diagnosen erzeugen.

---

# 133. IDE-Anforderungen

Nova-Entwicklungswerkzeuge SOLLEN:

- Schema-Autovervollständigung
- Feldinformationen
- Constraint-Hinweise
- Live-Validierung
- Versionswarnungen
- Migrationsvorschläge
- Fehlernavigation
- Schema-Diagramme
- Referenzauflösung
- Dokumentation

bereitstellen.

---

# 134. Testanforderungen

Jedes veröffentlichte Schema benötigt Tests für:

- gültige Minimalinstanz
- gültige vollständige Instanz
- fehlende Pflichtfelder
- falsche Feldtypen
- unbekannte Felder
- Grenzwerte
- Nullability
- optionale Felder
- Defaultwerte
- Invarianten
- Schemaabhängigkeiten
- Versionierung
- Kompatibilität
- Migration
- Sicherheitsregeln
- Ressourcenlimits

---

# 135. Validator-Tests

Jeder Validator benötigt Tests für:

- gültige Werte
- ungültige Werte
- Grenzfälle
- leere Werte
- maximale Größen
- fehlerhafte Kodierung
- deterministisches Verhalten
- Zeitlimits
- Speicherlimits
- Parallelisierung
- Fehlermeldungen
- lokalisierte Meldungen
- Cacheverhalten

---

# 136. Kompatibilitätstests

Bei jeder Schemaversion müssen automatisierte Tests prüfen:

```text
Old Data → New Schema

New Data → Old Schema

Old Capability → New Data

New Capability → Old Data
```

Das Ergebnis muss die deklarierte Kompatibilitätsklasse bestätigen.

---

# 137. Migrationstests

Migrationen müssen getestet werden auf:

- vollständige Feldübertragung
- Identitätserhaltung
- Referenzintegrität
- semantische Korrektheit
- Wiederholbarkeit
- Fehlerbehandlung
- Zielvalidierung
- Informationsverlust
- Rollback, sofern unterstützt
- große Datenmengen

---

# 138. Sicherheitstests

Schema- und Validierungssysteme müssen getestet werden gegen:

- übermäßige Verschachtelung
- riesige Collections
- Regex-Denial-of-Service
- manipulierte Schema-IDs
- gefälschte Versionsangaben
- zirkuläre Referenzen
- Schema-Importzyklen
- beschädigte Binärdaten
- Typ-Spoofing
- unbekannte aktive Erweiterungen
- Speichererschöpfung
- Zeitüberschreitungen

---

# 139. Leistungsanforderungen

Die Implementierung SOLL:

- Schemas cachen,
- vorkompilierte Validatoren verwenden,
- inkrementelle Validierung unterstützen,
- unveränderte Teilstrukturen überspringen,
- parallele Feldvalidierung ermöglichen,
- gestreamte Validierung unterstützen,
- Zero-Copy-Eingaben verarbeiten,
- Fehleraggregation begrenzen,
- Schemaabhängigkeiten effizient auflösen.

---

# 140. Qualitätsanforderungen

Das Schema- und Validierungssystem muss:

- deterministisch
- stark typisiert
- semantisch präzise
- maschinenlesbar
- versionierbar
- erweiterbar
- sicher
- datenschutzfähig
- performant
- testbar
- reflektierbar
- sprachunabhängig
- plattformunabhängig
- migrationsfähig
- langfristig kompatibel

sein.

---

# 141. Konformitätsstufen

Nova definiert drei Konformitätsstufen.

## 141.1 Schema Core

Unterstützt:

- Schemaidentität
- Versionierung
- primitive Constraints
- Records
- Pflichtfelder
- strukturierte Fehler

## 141.2 Schema Standard

Unterstützt zusätzlich:

- semantische Validierung
- Invarianten
- Collections
- Referenzen
- Schemaevolution
- Migration
- Capability-Integration

## 141.3 Schema Complete

Unterstützt zusätzlich:

- Sicherheitsvalidierung
- Provenance
- Streaming-Validierung
- inkrementelle Validierung
- verteilte Registry
- digitale Signaturen
- vollständige Reflection
- kanonische Validierung

---

# 142. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation vollständig, wenn:

- alle veröffentlichten Datentypen Schemas besitzen,
- Schema- und Type IDs getrennt verwaltet werden,
- Schemas unveränderlich und versioniert sind,
- nominale und strukturelle Prüfung unterstützt werden,
- technische und semantische Validierung verfügbar sind,
- Constraints und Invarianten ausgeführt werden,
- strukturierte ValidationResult-Objekte erzeugt werden,
- Schemaevolution und Migration unterstützt werden,
- Capability-Ein- und -Ausgaben validiert werden,
- Ressourcenlimits durchgesetzt werden,
- Reflection und Registry verfügbar sind,
- Sicherheits- und Datenschutzvalidierung unterstützt werden,
- inkrementelle und gestreamte Validierung möglich ist.

---

# 143. Zusammenfassung

Die **Schemas und Typvalidierung** bilden die verbindliche Kontroll- und Beschreibungsschicht des Nova-Datentypmodells.

Schemas definieren die technische Struktur, fachliche Semantik, Felder, Beziehungen, Constraints, Invarianten, Sicherheitsregeln und Kompatibilität eines Datentyps. Stabile Schema- und Feldidentitäten ermöglichen eine langfristige Versionierung, ohne veröffentlichte Datenstrukturen nachträglich umzudeuten.

Die Nova-Validierung prüft Daten stufenweise auf technische, strukturelle, semantische, relationale und sicherheitsbezogene Korrektheit. Validierungsergebnisse werden als strukturierte Objekte mit eindeutigen Fehlercodes, Datenpfaden, Schweregraden und Diagnosen ausgegeben.

Durch kontrollierte Schemaevolution, Schemaadapter und explizite Migrationen können Datentypen weiterentwickelt werden, ohne bestehende Daten oder Capabilities unkontrolliert zu beschädigen. Inkrementelle, partielle, verzögerte und gestreamte Validierung ermöglichen zugleich die Verarbeitung großer Dokumente, Datenbanken und Echtzeitströme.

Damit stellt NPSPEC-DATATYPE-SCHEMA-0001 sicher, dass Daten innerhalb der Nova Platform nicht nur technisch lesbar, sondern eindeutig beschrieben, fachlich korrekt, sicher verarbeitbar, versionierbar und langfristig interoperabel bleiben.