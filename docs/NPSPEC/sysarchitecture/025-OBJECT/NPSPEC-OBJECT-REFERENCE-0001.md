# NPSPEC-OBJECT-REFERENCE-0001 – Nova Object Reference

## Status

Angenommen

## Kategorie

Object / Reference / Architecture

## Zweck

NovaOS definiert ein einheitliches Referenzmodell, mit dem Systemkomponenten auf Objekte verweisen können, ohne deren physischen Speicherort, Prozess, Provider oder konkrete Repräsentation kennen zu müssen.

```text
Object Reference
      ↓
Object Resolution
      ↓
ObjectID
      ↓
Current Object Instance
```

Eine Object Reference identifiziert ein Ziel, stellt jedoch keine Zugriffsberechtigung dar.

## Grundprinzipien

```text
Object Reference ≠ Object
Object Reference ≠ Capability
Object Reference ≠ Handle
Object Reference ≠ Pointer
Object Reference ≠ Path
Object Reference ≠ Authority
Reference Resolution ≠ Authorization
Known Reference ≠ Access
```

## Referenzmodell

Eine Object Reference besitzt mindestens:

```text
ObjectReference
└── ObjectID
```

Optional:

```text
VersionID
ObjectTypeID
SemanticTypeID
Resolution Constraints
Location Hint
Consistency Requirement
```

Die `ObjectID` bleibt die maßgebliche logische Identität.

## Referenzarten

NovaOS unterstützt unterschiedliche Referenzformen:

```text
Logical Reference
Versioned Reference
Relative Reference
Relationship Reference
Capability-bound Reference
```

### Logical Reference

```text
ObjectID
```

Verweist auf das logische Objekt und kann auf dessen aktuellen gültigen Zustand aufgelöst werden.

### Versioned Reference

```text
ObjectID + VersionID
```

Verweist auf eine konkrete Version.

## Referenz und Handle

Ein Prozess kann eine Object Reference in einen lokalen Handle auflösen.

```text
ObjectReference
      ↓
Resolution
      ↓
Capability Validation
      ↓
Local Handle
```

Dabei gilt:

```text
Persistent Reference ≠ Runtime Handle
```

Handles bleiben lokal und kontextgebunden.

## Referenz und Capability

Eine Object Reference gewährt keine Autorität.

```text
ObjectReference
       +
Capability
       ↓
Authorized Resolution / Access
```

Beispiel:

```text
ObjectID bekannt
Capability fehlt
      ↓
Access Denied
```

## Location Transparency

Object References dürfen unabhängig vom aktuellen Standort des Objekts bleiben.

```text
ObjectReference
      ↓
ObjectID
      ↓
Resolution
      ├── Local
      ├── Remote
      └── Migrated
```

Location Hints dürfen zur Optimierung verwendet werden, sind jedoch nicht Teil der Objektidentität.

```text
Location Hint ≠ Identity
```

## Pfade

Dateipfade können als benutzerfreundliche oder namespacebezogene Referenzen dienen.

```text
Benutzer/Dokumente/Bericht.md
            ↓
Namespace Resolution
            ↓
ObjectID
```

Der Pfad ist nicht die stabile Referenz des Objekts.

Umbenennen oder Verschieben darf bestehende `ObjectID`-basierte Referenzen nicht ungültig machen.

## Beziehungen

Semantic Relationships verwenden Object References zur Verbindung von Objekten.

```text
Object A
   ↓ References
ObjectReference(Object B)
```

Dadurch bleiben Beziehungen unabhängig von Pfadänderungen.

## Referenzauflösung

Die Auflösung erfolgt kontrolliert.

```text
Reference
   ↓
Validate
   ↓
Resolve ObjectID
   ↓
Locate Object
   ↓
Capability Check
   ↓
Return Authorized Handle / View
```

Auflösung und Autorisierung bleiben logisch getrennte Schritte.

## Versionierung

Versionierte Referenzen dürfen nicht still auf eine andere Version umgebogen werden.

```text
ObjectID + VersionID A
```

muss Version A bezeichnen oder fehlschlagen.

Eine logische Referenz ohne `VersionID` kann dagegen gemäß definierter Policy auf die aktuelle Version aufgelöst werden.

## Ungültige Referenzen

Referenzen können Zustände besitzen wie:

```text
Resolvable
Unavailable
Retired
Unknown
Invalid
```

Dabei gilt:

```text
Unknown ≠ Resolvable
```

Eine nicht auflösbare Referenz darf nicht still auf ein anderes Objekt zeigen.

## Remote References

Object References können über IPC oder Netzwerk transportiert werden.

```text
Node A
  ↓
ObjectReference
  ↓
Node B
```

Die Übertragung einer Referenz darf keine Capability übertragen, sofern dies nicht ausdrücklich als Capability Transfer erfolgt.

```text
Reference Transfer ≠ Authority Transfer
```

## Caching

Resolution-Ergebnisse können gecacht werden.

Der Cache muss mindestens berücksichtigen:

```text
ObjectID
VersionID
Location State
Object State
Resolution Generation
```

Migration, Retirement oder relevante Zustandsänderungen müssen eine Revalidierung ermöglichen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
VersionID
Reference Type
Resolution State
Object Type
Semantic Type
Location Class
Consistency Requirement
```

Die Introspection einer Referenz erzeugt keine Autorität über das Zielobjekt.

## Normative Anforderungen

1. NovaOS MUSS stabile `ObjectID`-basierte Referenzen unterstützen.
2. Object References MÜSSEN von Handles, Pointern, Pfaden und Capabilities getrennt bleiben.
3. Eine Object Reference DARF keine Autorität darstellen.
4. Logische und versionierte Referenzen MÜSSEN unterscheidbar sein.
5. Versionierte Referenzen DÜRFEN NICHT still auf andere Versionen aufgelöst werden.
6. Referenzen MÜSSEN Location Transparency unterstützen können.
7. Location Hints DÜRFEN die Objektidentität NICHT verändern.
8. Reference Transfer DARF NICHT automatisch Authority Transfer bedeuten.
9. Nicht auflösbare Referenzen DÜRFEN NICHT still auf andere Objekte umgeleitet werden.
10. Reference Resolution MUSS introspektierbar und revalidierbar sein.

## Abhängigkeiten

- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0023`

## Ergebnis

```text
Stable Object Reference
        ↓
ObjectID Resolution
        ↓
Location-independent Object
        ↓
Capability Validation
        ↓
Authorized Handle / View
```

NovaOS erhält damit ein stabiles Referenzmodell, das Objektidentität und Objektzugriff von Pfaden, Speicheradressen, Handles und physischen Standorten trennt und dadurch dauerhafte Beziehungen sowie lokale und verteilte Objektzugriffe ermöglicht.