# ADR-OBJECT-0001 – Unified System Object Model

## Status

Angenommen

## Kategorie

Object Architecture / System Model

## Kontext

NovaOS verwaltet unterschiedliche Arten von Systemobjekten:

```text
Files
Devices
Processes
Services
Users
Capabilities
Network Resources
Data Objects
```

Werden diese vollständig getrennt modelliert, entstehen unterschiedliche Identitäts-, Metadaten-, Berechtigungs- und Discovery-Mechanismen.

NovaOS benötigt deshalb ein gemeinsames Grundmodell für adressierbare Systemobjekte.

## Entscheidung

NovaOS verwendet ein **Unified System Object Model**.

Alle adressierbaren Systemressourcen können auf einem gemeinsamen abstrakten Objektmodell basieren.

```text
Nova Object
 ├── Object ID
 ├── Object Type
 ├── Metadata
 ├── Relationships
 ├── Capabilities
 └── Permissions
```

Spezialisierte Objekttypen erweitern dieses Grundmodell.

## Objektidentität

Jedes persistente oder systemweit adressierbare Objekt erhält eine stabile Objektidentität.

```text
Object ID
    ≠
Path
    ≠
Name
    ≠
Handle
```

Pfad, Name und Handle sind lediglich Zugriffs- oder Darstellungsformen.

## Objekttypen

Objekte besitzen einen definierten Typ.

Beispiele:

```text
File
Directory
Device
Process
Service
User
CapabilityProvider
NetworkEndpoint
```

Objekttypen können durch Semantic Types ergänzt werden.

## Eigenschaften

Gemeinsame Eigenschaften werden über das einheitliche Objektmodell bereitgestellt.

Dazu gehören insbesondere:

```text
Identity
Type
Metadata
Relationships
Lifecycle
Permissions
Capabilities
```

Nicht jeder Objekttyp muss jede Eigenschaft verwenden.

## Spezialisierung

Das Unified Object Model vereinheitlicht gemeinsame Mechanismen, ohne alle Ressourcen technisch gleichzusetzen.

```text
Nova Object
   ├── File Object
   ├── Device Object
   ├── Process Object
   └── Service Object
```

Objektspezifische Eigenschaften und Operationen bleiben erhalten.

## Relationships

Objekte können über Semantic Relationships miteinander verbunden werden.

```text
Object A
    ↓
Relationship
    ↓
Object B
```

Beziehungen referenzieren stabile Object IDs statt ausschließlich Pfade oder Namen.

## Capabilities

Operationen auf Objekten werden bevorzugt über Capabilities beschrieben.

```text
Object
   ↓
Available Capability
   ↓
Authorized Operation
```

Der Objekttyp selbst muss dadurch nicht sämtliche möglichen Operationen implementieren.

## Location Transparency

Die Objektidentität ist grundsätzlich unabhängig vom physischen Speicherort.

```text
Object ID
   ↓
Resolver
   ↓
Local / Remote Resource
```

Damit bleibt das Objektmodell mit Location Transparency kompatibel.

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames abstraktes Systemobjektmodell besitzen.
2. Systemweit adressierbare Objekte MÜSSEN eindeutig identifizierbar sein.
3. Object ID, Name, Pfad und Handle MÜSSEN getrennte Konzepte bleiben.
4. Objekte MÜSSEN typisierbar sein.
5. Das Objektmodell MUSS Metadaten und Relationships unterstützen können.
6. Berechtigungen und Capabilities MÜSSEN auf Objekte anwendbar sein.
7. Spezialisierte Objekttypen MÜSSEN eigene Eigenschaften und Semantik behalten können.
8. Objektidentität DARF nicht vom physischen Speicherort abhängig sein.

## Konsequenzen

### Positive Konsequenzen

- einheitliches Modell für Systemressourcen,
- gemeinsame Identity-, Permission- und Metadata-Mechanismen,
- stabile Beziehungen zwischen Objekten,
- bessere Integration von Semantic Types und Capabilities,
- weniger getrennte Sondermodelle im System.

### Negative Konsequenzen

- das gemeinsame Basismodell muss klein und stabil gehalten werden,
- objektspezifische Eigenschaften dürfen nicht unnötig in die Basisschicht wandern.

## Abhängigkeiten

- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0017_Composability`
- `ADR-FILESYSTEM-0003_Pfad_unabhaengige_Objektidentitaet`
- `ADR-SEMANTIC-0003_Typed_Resources`
- `ADR-SEMANTIC-0008_Semantic_Relationships`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-TYPE-0001`

## Ergebnis

NovaOS behandelt adressierbare Systemressourcen über ein gemeinsames Objektmodell:

```text
Unified Nova Object
        ↓
Identity + Type
        ↓
Metadata + Relationships
        ↓
Permissions + Capabilities
        ↓
Specialized Resource
```

Der zentrale Grundsatz lautet:

```text
Unterschiedliche Systemressourcen
behalten ihre Spezialisierung,

teilen aber ein gemeinsames
Objektmodell.
```