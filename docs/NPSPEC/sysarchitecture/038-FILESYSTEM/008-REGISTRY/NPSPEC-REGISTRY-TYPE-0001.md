# NPSPEC-REGISTRY-TYPE-0001 – Nova Type Registry

## Status

Angenommen

## Kategorie

Registry / Type

## Zweck

NovaOS definiert die Type Registry als systemweites Verzeichnis semantischer Typen.

Sie ermöglicht die eindeutige Identifikation, Discovery und Auflösung von Datentypen unabhängig von Dateiendungen, Pfaden, Programmen oder konkreten Speicherformaten.

## Grundprinzipien

```text
TypeID ≠ File Extension
TypeID ≠ MIME Type
TypeID ≠ Path
TypeID ≠ Application
Type Discovery ≠ Authority
Semantic Type ≠ Physical Format
```

## Typmodell

Ein registrierter Typ besitzt mindestens:

```text
TypeRegistryEntry
├── TypeID
├── Version
├── Name
├── ParentTypeIDs
├── Properties
└── State
```

Optional:

```text
MetadataSchema
CompatibleFormats
CapabilityRequirements
ProviderReferences
Localization
```

Die `TypeID` ist die stabile Identität des semantischen Typs.

## Typhierarchie

Typen dürfen hierarchisch aufgebaut sein:

```text
Media
├── Image
│   ├── Photo
│   └── Illustration
├── Audio
└── Video
```

Ein Objekt darf mehreren kompatiblen semantischen Typen zugeordnet sein.

## Dateiformate

Dateiendungen und physische Formate können Hinweise auf einen Typ liefern:

```text
.jpg
.png
.webp
   ↓
Image
```

Die Dateiendung ist jedoch nicht die maßgebliche Typidentität.

Inhalt, Metadaten oder explizite Typinformationen dürfen zur Bestimmung verwendet werden.

## NovaFile

NovaFiles können ihre semantischen Typinformationen direkt als Metadaten tragen:

```text
NovaFile
├── Payload
├── Metadata
│   └── SemanticTypeID
└── Relations
```

Dadurch bleibt die semantische Bedeutung unabhängig vom Payload-Format erhalten.

## Capability-Integration

Typen können mit passenden Capabilities verbunden werden:

```text
SemanticTypeID
      ↓
Type Registry
      ↓
Compatible Capabilities
      ↓
Capability Registry
      ↓
Provider Selection
```

Beispielsweise kann für einen Bildtyp ermittelt werden, welche Capabilities diesen Typ anzeigen, konvertieren oder bearbeiten können.

Die Zuordnung erzeugt keine Authority.

## Discovery

Die Registry unterstützt mindestens:

```text
TypeID → Type Definition
Object → Semantic Types
Type → Parent Types
Type → Child Types
Type → Compatible Formats
Type → Compatible Capabilities
```

## Versionierung

Typdefinitionen müssen versionierbar sein.

Bestehende `TypeID`s dürfen nicht stillschweigend mit inkompatibler Bedeutung wiederverwendet werden.

## Sicherheit

Die Type Registry enthält Beschreibungen und Zuordnungen, jedoch keine Capability-Tokens oder Zugriffsrechte.

Das Erkennen des Typs eines Objekts gewährt keinen Zugriff auf dessen Inhalt.

## Normative Anforderungen

1. NovaOS MUSS eine systemweite Type Registry bereitstellen.
2. Semantische Typen MÜSSEN stabile `TypeID`s besitzen.
3. `TypeID` DARF nicht von Dateiname, Pfad oder Dateiendung abhängen.
4. Typen MÜSSEN hierarchische Beziehungen unterstützen können.
5. Ein Objekt DARF mehreren kompatiblen Typen zugeordnet sein.
6. Dateiendungen DÜRFEN nur als Hinweise auf semantische Typen dienen.
7. NovaFiles MÜSSEN semantische Typinformationen referenzieren können.
8. Typen MÜSSEN mit kompatiblen Capabilities verknüpfbar sein.
9. Type Discovery DARF keine Authority erzeugen.
10. Typdefinitionen MÜSSEN versionierbar sein.
11. Inkompatible Bedeutungsänderungen DÜRFEN nicht stillschweigend unter derselben Typversion erfolgen.
12. Typen, Hierarchien und unterstützte Capabilities MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-INDEX-METADATA-0001`
- `NPSPEC-INDEX-QUERY-0001`

## Ergebnis

NovaOS besitzt eine zentrale Registry für stabile semantische Typen. Daten können unabhängig von Dateiendung, Speicherformat und Anwendung klassifiziert werden, während Typinformationen direkt für Suche, Projektionen und die Auswahl geeigneter Capabilities verwendet werden können.