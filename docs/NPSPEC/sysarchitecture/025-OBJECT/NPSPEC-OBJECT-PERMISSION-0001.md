# NPSPEC-OBJECT-PERMISSION-0001 – Nova Object Permission

## Status

Angenommen

## Kategorie

Object / Permission / Security

## Zweck

NovaOS definiert ein einheitliches Berechtigungsmodell für Operationen auf Objekten.

Object Permissions beschreiben, **welche Operationen grundsätzlich auf einem Objekt erlaubt werden können**, während die tatsächliche Autorität über Capabilities und den aktuellen Security Context bestimmt wird.

```text
Principal
   ↓
Capability
   ↓
Object Permission Check
   ↓
Object
   ↓
Authorized Operation
```

## Grundprinzipien

```text
Permission ≠ Capability
Permission ≠ Authority
ObjectID ≠ Permission
Ownership ≠ Unlimited Authority
Visibility ≠ Access
Read ≠ Write
Write ≠ Delete
Delete ≠ Destroy History
Permission Policy ≠ Capability Grant
```

## Permission Model

Ein Objekt kann eine definierte Menge unterstützter Berechtigungen besitzen.

```text
ObjectPermission
├── ObjectID
├── PermissionType
├── Scope
└── Constraints
```

Optional:

```text
Version Scope
Semantic Scope
Principal Constraint
Purpose
Security Domain
Expiration
Delegation Policy
Audit Requirement
```

## Standardoperationen

Grundlegende Object Permissions können sein:

```text
Discover
Inspect
Read
Write
ModifyMetadata
ModifyRelationships
CreateVersion
ReadHistory
Delete
Export
Share
Execute
Delegate
Administer
```

Objekttypen dürfen zusätzliche spezialisierte Operationen definieren.

## Feingranulare Rechte

Berechtigungen sollen möglichst präzise formuliert werden.

Beispiel:

```text
ReadPayload
ReadMetadata
WritePayload
WriteMetadata
ReadRelationships
ModifyRelationships
```

Damit muss eine Komponente nicht automatisch Zugriff auf das gesamte Objekt erhalten.

## Capability Integration

Die tatsächliche Autorität wird über Capabilities vermittelt.

```text
Capability
├── Target ObjectID
├── Rights
└── Constraints
```

Die effektive Autorität ergibt sich aus:

```text
Capability Rights
      ∩
Object Permissions
      ∩
Security Policy
      ∩
Current Constraints
```

Ein Objekt darf keine implizite Ambient Authority erzeugen.

## Versionen

Permissions können auf bestimmte Versionen begrenzt werden.

```text
ObjectID
├── Current Version → Read + Write
└── Historical Versions → Read
```

Dabei gilt:

```text
Write Current Version ≠ Rewrite Historical Version
```

Historische Versionen sollen grundsätzlich unveränderbar behandelt werden können.

## Metadata und Relationships

Payload, Metadata und Relationships müssen getrennt kontrollierbar sein.

```text
Object
├── Payload
├── Metadata
├── Relationships
└── Provenance
```

Beispielsweise darf:

```text
Read Payload
```

nicht automatisch bedeuten:

```text
Read Protected Metadata
Read Provenance
Modify Relationships
```

## Provenance

Provenance kann eigene Rechte besitzen.

```text
ReadProvenance
AppendProvenance
AdministerProvenance
```

Normale Objektänderungen dürfen historische Provenance nicht still überschreiben.

## Delegation

Eine bestehende Object Capability kann nur gemäß ihrer Delegation Policy weitergegeben werden.

```text
Authority(B) ⊆ Authority(A)
```

Delegierte Rechte dürfen die ursprünglichen Rechte nicht erweitern.

## Permission Inheritance

Objektbeziehungen erzeugen standardmäßig keine automatische Berechtigungsvererbung.

```text
Container Access ≠ Child Object Access
Parent Object Access ≠ Derived Object Access
Relationship ≠ Authority
```

Falls Vererbung für bestimmte Objektklassen vorgesehen ist, muss sie explizit durch Policy definiert werden.

## Löschen

`Delete` beschreibt die logische Entfernung eines Objekts.

```text
Delete ≠ Secure Erase
Delete ≠ History Destruction
Delete ≠ Provenance Destruction
```

Physische Vernichtung benötigt separate Regeln und gegebenenfalls eine eigene Capability.

## Export und Share

Export und Weitergabe werden getrennt behandelt.

```text
Read ≠ Export
Read ≠ Share
Export ≠ Delegate Authority
```

Damit können Daten lokal nutzbar sein, ohne automatisch externe Weitergabe zu erlauben.

## Dynamische Änderungen

Permissions und zugehörige Capabilities können sich während der Laufzeit ändern.

```text
Permission Change
Capability Revocation
Trust Change
Security Policy Change
Object State Change
```

Laufende Operationen müssen entsprechend ihrer Policy neu bewertet, eingeschränkt oder abgebrochen werden können.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
Supported Permissions
Effective Permissions
Capability Constraints
Version Scope
Security Constraints
Delegation Policy
Expiration
```

Introspection erzeugt keine zusätzliche Autorität.

## Normative Anforderungen

1. NovaOS MUSS Object Permissions von Capabilities und Autorität unterscheiden.
2. Objektzugriff MUSS über explizite Autorität kontrollierbar sein.
3. Permissions SOLLEN feingranular nach Operation und Objektbereich definierbar sein.
4. Payload, Metadata, Relationships und Provenance MÜSSEN getrennt kontrollierbar sein können.
5. Versionsbezogene Rechte MÜSSEN unterstützt werden können.
6. Object Relationships DÜRFEN NICHT automatisch Berechtigungen übertragen.
7. Delegierte Object Authority DARF die ursprüngliche Autorität NICHT erweitern.
8. `Read`, `Export`, `Share`, `Delete` und `Secure Erase` MÜSSEN getrennte Operationen bleiben.
9. Permission- und Capability-Änderungen MÜSSEN laufende Zugriffe revalidieren können.
10. Effective Object Permissions MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DAC-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `ADR-ARCH-0026`

## Ergebnis

```text
Object
   ↓
Defined Permissions
   +
Capability
   +
Security Policy
   ↓
Effective Authority
   ↓
Authorized Object Operation
```

NovaOS erhält damit ein feingranulares Objektberechtigungsmodell, das klar zwischen den auf einem Objekt möglichen Operationen und der tatsächlich erteilten Autorität unterscheidet und sich vollständig in die Capability-basierte Sicherheitsarchitektur integriert.