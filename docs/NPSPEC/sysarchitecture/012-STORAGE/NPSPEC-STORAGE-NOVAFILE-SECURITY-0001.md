# NPSPEC-STORAGE-NOVAFILE-SECURITY-0001 – NovaFile Security

## Status

Angenommen

## Kategorie

Storage / NovaFile / Security / Access Control

## Zweck

NovaOS definiert Sicherheitsregeln für NovaFile-Container, deren Payload, Metadaten und Beziehungen.

```text
NovaFile
├── Payload
├── Metadata
├── Relationships
└── Security Context
```

NovaFile darf seine erweiterten Funktionen nicht nutzen, um die allgemeinen Security- und Capability-Regeln von NovaOS zu umgehen.

## Grundprinzip

```text
Container Access ≠ Unlimited Content Access
Object Identity ≠ Authorization
Metadata Visibility ≠ Payload Permission
```

Zugriff muss anhand des aktuellen Security Context und der zugehörigen Capabilities entschieden werden.

## Security Scope

Sicherheitsregeln dürfen unterschiedliche Bereiche eines NovaFile schützen:

```text
Whole Object
Payload
Metadata
Relationship
Projection
```

Dadurch können beispielsweise öffentliche Metadaten mit einem geschützten Payload kombiniert werden.

## Zugriffskontrolle

Zugriffe erfolgen über autorisierte Objekt-Referenzen.

```text
Security Context
      +
Capability
      ↓
NovaFile Access
```

Mindestens folgende Rechte müssen unterscheidbar sein:

```text
Read
Write
MetadataRead
MetadataWrite
Execute
Share
Export
```

Weitere Rechte dürfen ergänzt werden.

## Metadaten

Metadaten können sensible Informationen enthalten.

Beispiele:

```text
Author
Provenance
Relationships
Security Labels
Location Information
Application Data
```

NovaOS muss deshalb Metadaten unabhängig vom Payload schützen können.

## Integrität

Kritische NovaFile-Strukturen müssen gegen unbemerkte Veränderung geschützt werden können.

```text
Header
Metadata
Payload
Relationships
   ↓
Integrity Verification
```

Beschädigte oder ungültige sicherheitsrelevante Strukturen dürfen nicht stillschweigend akzeptiert werden.

## Verschlüsselung

NovaFile darf verschlüsselte Bereiche enthalten oder durch die Storage-Verschlüsselung vollständig geschützt werden.

```text
NovaFile
   ↓
Encryption Policy
   ↓
Encrypted Representation
```

Schlüsselverwaltung bleibt Aufgabe der NovaOS-Security- und Storage-Infrastruktur.

## Provenance und Trust

Provenance darf für Sicherheitsentscheidungen ausgewertet werden, erzeugt jedoch nicht automatisch Vertrauen.

```text
Provenance ≠ Trust
Signature ≠ Permission
```

Signaturen können Herkunft oder Integrität bestätigen, ersetzen aber keine Zugriffsberechtigung.

## Projektionen

NovaFile-Projektionen müssen die Sicherheitsregeln des zugrunde liegenden Objekts erhalten.

```text
NovaFile
   ↓
Projection
   ↓
Same or More Restricted Access
```

Eine Projektion darf keine geschützten Metadaten oder Inhalte offenlegen, die über den nativen Zugriff nicht erlaubt wären.

## Export

Beim Export eines NovaFile-Payloads muss geprüft werden, ob der Security Context die Weitergabe erlaubt.

```text
NovaFile
   ↓
Export Authorization
   ↓
Standalone Object
```

Security-Metadaten dürfen nicht unkontrolliert entfernt werden, wenn dadurch definierte Schutzregeln umgangen würden.

## Beziehungen

Objektreferenzen innerhalb eines NovaFile erzeugen keine Zugriffsrechte auf die referenzierten Objekte.

```text
Reference to Object B
        ≠
Access to Object B
```

Jede Auflösung muss separat autorisiert werden.

## Untrusted NovaFiles

Extern importierte NovaFiles müssen zunächst als nicht vertrauenswürdig behandelt werden.

NovaOS soll mindestens prüfen können:

```text
Container Structure
Format Version
Size Limits
Integrity
Metadata Schema
Relationships
Embedded Content
```

Fehlerhafte oder manipulierte Container müssen kontrolliert abgewiesen oder isoliert werden können.

## Ressourcenbegrenzung

NovaFile-Verarbeitung muss gegen missbräuchliche Container geschützt sein.

Dazu gehören Limits für:

```text
Container Size
Metadata Size
Relationship Count
Nesting
Decompression
Parsing Resources
```

Dadurch sollen Ressourcenerschöpfung und Parser-Missbrauch verhindert werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ObjectID
Security State
Permissions
Encryption State
Integrity State
Trust State
Security Labels
Export Restrictions
```

Geheime oder nicht autorisierte Informationen dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaFile MUSS die NovaOS-Security- und Capability-Regeln einhalten.
2. Payload, Metadaten und Beziehungen MÜSSEN getrennt autorisierbar sein können.
3. Die Kenntnis einer `ObjectID` DARF KEINE Zugriffsberechtigung erzeugen.
4. Objektreferenzen DÜRFEN KEINE Rechte auf referenzierte Objekte übertragen.
5. Projektionen DÜRFEN die Sicherheitsregeln des zugrunde liegenden Objekts NICHT umgehen.
6. Export MUSS separat autorisierbar sein.
7. Sicherheitsrelevante Containerstrukturen MÜSSEN auf Integrität prüfbar sein.
8. Externe NovaFiles SOLLEN standardmäßig als nicht vertrauenswürdig behandelt werden.
9. Parser und Decoder MÜSSEN definierte Ressourcenlimits einhalten.
10. Verschlüsselte NovaFile-Bereiche MÜSSEN über kontrollierte Schlüsselverwaltung zugänglich sein.
11. Provenance oder Signaturen DÜRFEN NICHT automatisch Zugriffsrechte erzeugen.
12. Security-Zustände SOLLEN sicher introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-STORAGE-ENCRYPTION-0001`
- `NPSPEC-STORAGE-NOVAFILE-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-PROJECTION-0001`
- `ADR-STORAGE-0020`

## Ergebnis

```text
NovaFile
   ↓
Security + Capability Check
   ↓
Authorized View
├── Payload
├── Metadata
├── Relationships
└── Projection
```

NovaFile erhält damit ein Sicherheitsmodell, das Payload, Metadaten, Beziehungen und Projektionen kontrolliert schützt und vollständig in die allgemeine NovaOS-Sicherheitsarchitektur integriert.