# NPSPEC-OBJECT-MODEL-0001 – Nova Object Model

## Status

Angenommen

## Kategorie

Architecture / Object Model / Identity

## Zweck

NovaOS definiert ein systemweites Objektmodell für eindeutig identifizierbare Systementitäten.

```text
Object
├── Identity
├── Type
├── State
├── Metadata
├── Relationships
└── Capabilities
```

Das Objektmodell bildet die gemeinsame Grundlage für Dateien, Ressourcen, Geräte, Services, Prozesse und weitere NovaOS-Entitäten.

## Grundprinzipien

```text
Object Identity ≠ Object Location
Object Identity ≠ Object Name
Object Identity ≠ Memory Address
Object Identity ≠ Handle
Object Identity ≠ Capability
Object Reference ≠ Authority
Object State ≠ Object Identity
```

## ObjectID

Jedes persistente oder systemweit referenzierbare Objekt erhält eine stabile `ObjectID`.

```text
NovaObject
├── ObjectID
├── ObjectTypeID
├── State
└── Version
```

Die `ObjectID` identifiziert das logische Objekt unabhängig von:

```text
Path
Name
Process
Memory Address
Storage Volume
Machine
Provider
Physical Location
```

## Object Type

`ObjectTypeID` beschreibt die grundlegende Systemklasse eines Objekts.

Beispiele:

```text
File
Volume
Process
Service
Device
Resource
CapabilityProvider
SemanticObject
```

Semantische Bedeutung kann zusätzlich über `SemanticTypeID` beschrieben werden.

```text
ObjectTypeID ≠ SemanticTypeID
```

## Objektzustand

Objekte besitzen einen expliziten Zustand.

```text
Created
   ↓
Active
   ↓
Modified
   ↓
Suspended / Unavailable
   ↓
Retired
```

Objekttypen können zusätzliche eigene Zustände definieren.

Eine Zustandsänderung verändert nicht automatisch die `ObjectID`.

## Versionierung

Objekte können mehrere Versionen besitzen.

```text
ObjectID
├── Version 1
├── Version 2
└── Version 3
```

Dabei gilt:

```text
Same ObjectID
Different VersionID
```

Eine neue Version repräsentiert einen neuen Zustand desselben logischen Objekts.

## Referenzen

Objekte sollen über stabile Referenzen adressiert werden.

```text
ObjectReference
├── ObjectID
└── optional VersionID
```

Speicheradressen, Dateipfade oder lokale Handles dürfen nicht als dauerhafte Objektidentität verwendet werden.

## Location Transparency

Ein Objekt kann seinen physischen Standort ändern.

```text
ObjectID
   ↓
Local Storage
   ↓
Migration
   ↓
Remote Storage
```

Die logische Identität bleibt erhalten.

```text
Identity ≠ Location
```

Location Transparency erzeugt jedoch keine zusätzliche Autorität.

## Metadaten

Objekte können typisierte Metadaten besitzen.

```text
NovaObject
└── Semantic Metadata
    ├── Properties
    ├── Labels
    ├── Provenance
    └── Context
```

Metadaten gehören zum Objektmodell, bleiben aber getrennt von dessen Identität und Payload.

## Beziehungen

Objekte können über stabile Beziehungen miteinander verbunden werden.

```text
Object A
   ↓ Relationship
Object B
```

Beispiele:

```text
Contains
References
DerivedFrom
DependsOn
VersionOf
CreatedBy
```

Beziehungen erzeugen keine Zugriffsrechte.

## Capability Security

Eine `ObjectID` stellt keine Autorität dar.

```text
ObjectID
   +
Capability
   ↓
Authorized Operation
```

Der Zugriff auf ein Objekt benötigt eine passende Capability.

```text
Known ObjectID ≠ Access
```

Capabilities können Rechte auf einzelne Objekte oder definierte Objektmengen beschränken.

## Handles

Lokale Prozesse können Objekte über Handles referenzieren.

```text
Process
   ↓
Handle
   ↓
Capability
   ↓
ObjectID
```

Ein Handle ist nur eine lokale Referenz und nicht die globale Objektidentität.

## Lebenszyklus

Der Objektlebenszyklus muss kontrollierbar sein.

```text
Create
 ↓
Register
 ↓
Use
 ↓
Modify / Version
 ↓
Retire
 ↓
Reclaim
```

`Retire` und physische Speicherfreigabe bleiben getrennte Vorgänge.

Historische Referenzen können entsprechend Retention- und Provenance-Regeln erhalten bleiben.

## Verteilte Objekte

Objekte können über Systemgrenzen hinweg referenziert werden.

```text
ObjectID
├── Local Instance
├── Remote Instance
└── Migrated Instance
```

NovaOS muss verhindern, dass mehrere aktive Instanzen unkontrolliert widersprüchliche Autorität über dieselbe Objektidentität beanspruchen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
ObjectTypeID
SemanticTypeID
VersionID
State
Location Class
Metadata
Relationships
Provenance
Available Operations
```

Introspection erzeugt keine zusätzliche Autorität.

## Normative Anforderungen

1. NovaOS MUSS systemweit referenzierbare Objekte eindeutig identifizieren können.
2. `ObjectID` MUSS von Name, Pfad, Handle und physischem Speicherort getrennt sein.
3. `ObjectTypeID` und `SemanticTypeID` MÜSSEN getrennt modellierbar sein.
4. Objektversionen MÜSSEN von der logischen Objektidentität unterscheidbar sein.
5. Objektbeziehungen SOLLEN stabile Identitäten verwenden.
6. Eine `ObjectID` DARF keine Zugriffsautorität darstellen.
7. Objektzugriff MUSS über geeignete Capabilities kontrollierbar sein.
8. Objekte MÜSSEN unabhängig von ihrem physischen Standort referenzierbar sein können.
9. Objektlebenszyklus und physische Ressourcenfreigabe MÜSSEN getrennt behandelbar sein.
10. Objektzustand, Versionen und Beziehungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-ARCH-0021`

## Ergebnis

```text
Nova Object
    ↓
Stable ObjectID
    ↓
Type + State + Version
    ↓
Metadata + Relationships
    ↓
Capability-controlled Access
    ↓
Location-independent Operation
```

NovaOS erhält damit ein einheitliches Objektmodell, in dem Systementitäten über stabile Identitäten beschrieben, versioniert, miteinander verknüpft und unabhängig von ihrem physischen Standort verwendet werden können, ohne Identität, Referenz und Autorität miteinander zu vermischen.