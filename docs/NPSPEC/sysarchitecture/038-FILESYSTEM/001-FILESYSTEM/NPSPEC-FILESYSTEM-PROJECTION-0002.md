# NPSPEC-FILESYSTEM-PROJECTION-0002 – Nova Filesystem Projection

## Status

Angenommen

## Kategorie

Filesystem / Namespace / Projection

## Zweck

NovaOS verwendet Filesystem Projections, um vorhandene Ressourcen logisch an einer oder mehreren Stellen des Namespace darzustellen, ohne deren physische Speicherung oder Identität an die sichtbare Pfadstruktur zu koppeln.

```text
Resource
   ↓
Stable ID
   ↓
Projection
   ↓
Namespace View
```

Eine Projection ist damit eine **Sicht auf eine Ressource**, nicht die Ressource selbst.

## Grundprinzipien

```text
Projection ≠ Copy
Projection ≠ Export
Projection ≠ Object Identity
Projection Path ≠ Physical Location
Projection ≠ Capability
Multiple Projections ≠ Multiple Objects
Visible ≠ Authorized
```

## Projection Model

```text
Projection
├── ProjectionID
├── TargetID
├── NamespaceID
├── Path
├── Type
├── Scope
├── Priority
└── State
```

Optional:

```text
OwnerID
SourceID
Query
Policy
SemanticTypeID
RelationshipID
TransactionID
ProvenanceID
```

## Projection Types

NovaOS unterstützt mindestens:

```text
Direct
Overlay
Semantic
Relationship
Contextual
Virtual
```

### Direct

Projiziert eine konkrete Ressource an eine Namespace-Position.

```text
ObjectID
   ↓
/Benutzer/Dokumente/Datei.nf
```

### Overlay

Führt mehrere Quellen zu einer gemeinsamen effektiven Sicht zusammen.

```text
Global /System
      +
Private SYS
      ↓
Effective /System
```

### Semantic

Erzeugt eine Sicht anhand semantischer Eigenschaften.

```text
SemanticType = image
        ↓
/Benutzer/Bilder/
```

### Relationship

Projiziert Ressourcen anhand ihrer Beziehungen.

```text
Solution
├── Document
├── Dataset
└── Image
```

### Contextual

Ist nur innerhalb eines bestimmten Kontextes sichtbar.

```text
User
Process
Program
Solution
Workspace
Recovery
```

### Virtual

Erzeugt eine logische Namespace-Struktur ohne entsprechende physische Verzeichnisstruktur.

## Stabile Identität

Jede Projection verweist auf eine stabile Zielidentität.

```text
Projection A ─┐
Projection B ─┼──→ ObjectID
Projection C ─┘
```

Das Verschieben, Umbenennen, Hinzufügen oder Entfernen einer Projection verändert die ObjectID nicht.

## Mehrere Sichten

Ein Objekt darf gleichzeitig in mehreren logischen Bereichen erscheinen.

```text
ObjectID: 42

├── /Benutzer/Bilder/Urlaub.nf
├── /Solutions/Reise/Bilder/Urlaub.nf
└── /Sammlungen/2026/Urlaub.nf
```

Es existiert weiterhin nur ein zugrunde liegendes Objekt.

```text
Three Paths
≠
Three Copies
```

## Daten-Projection

Der sichtbare Bereich `Daten` kann als logische Projection aufgebaut werden.

```text
Objects
+
Metadata
+
Semantic Types
+
Relationships
        ↓
Projection Engine
        ↓
Daten/
```

Damit muss `Daten` weder einer Partition noch einem einzelnen Volume entsprechen.

## Application SYS Projection

Programme dürfen private Systemabhängigkeiten besitzen.

```text
/Apps/Example/SYS/
├── Libraries/
├── Runtime/
└── Dependencies/
```

Beim Start kann NovaOS diese Ressourcen in die effektive Systemansicht des Programms projizieren.

```text
Global /System
      +
/Apps/Example/SYS
      ↓
Effective Program /System
```

Andere Programme sehen dieses Overlay nicht automatisch.

```text
Private SYS Projection
≠
Global System Modification
```

## Resolution

Die Projection Engine muss Konflikte deterministisch auflösen.

Beispiel:

```text
1. Explicit Context Projection
2. Application SYS Overlay
3. Scoped Projection
4. Global Namespace
```

Die konkrete Reihenfolge wird durch die jeweilige Namespace- und Projection-Policy festgelegt.

Es darf keine zufällige oder vom Speicherort abhängige Auflösung geben.

## Projection und Capability

Eine Projection erzeugt keine Zugriffsberechtigung.

```text
Projection Path
      ↓
Resolve TargetID
      ↓
Capability Check
      ↓
Authorized Handle
```

Es gilt:

```text
Can See ≠ Can Read
Can Resolve ≠ Can Access
Can Project ≠ Can Modify
```

Berechtigungen beziehen sich auf die Ressource und die angeforderte Operation, nicht auf den sichtbaren Pfad.

## Projection und Handle

Nach erfolgreicher Auflösung soll ein stabiler Handle verwendet werden.

```text
Path
 ↓
Projection
 ↓
ObjectID
 ↓
Capability
 ↓
Handle
```

Wird die Projection anschließend entfernt, bleibt der Handle gültig, solange:

```text
Object Exists
AND
Handle Valid
AND
Capability Valid
```

## Dynamische Projections

Projections dürfen dynamisch entstehen.

Beispiele:

```text
Program Start
Solution Open
Workspace Open
Volume Mount
Device Connect
User Login
Recovery Start
```

Sie dürfen beim Ende des jeweiligen Scopes wieder entfernt werden.

## Projection Lifecycle

```text
Declared
   ↓
Validated
   ↓
Active
   ↓
Updated
   ↓
Removed
```

Fehlerzustände:

```text
Blocked
Invalid
Unavailable
Unknown
```

`Unknown` gilt nicht als aktive oder gültige Projection.

## Transaktionale Änderung

Projection-Änderungen sollen atomar sichtbar werden.

```text
Begin
 ↓
Create / Modify Projection
 ↓
Validate
 ↓
Commit
 ↓
Visible
```

Ein Prozess darf keinen teilweise aufgebauten Projection-Zustand beobachten.

## NovaFile Integration

Ein NovaFile kann mehrere Sichten bereitstellen.

```text
NovaFile Object
├── Payload
├── Metadata
└── Relationships
```

Mögliche Projections:

```text
Native
Payload
Metadata
Semantic
Relationship
```

Alle bleiben auf dieselbe ObjectID zurückführbar.

Ein Export unterscheidet sich davon ausdrücklich:

```text
Projection
→ Same ObjectID

Export
→ New ObjectID
```

## Cache

Aufgelöste Projections dürfen gecacht werden.

Der Cache muss invalidiert werden können bei Änderungen an:

```text
Target
Metadata
Relationships
Semantic Types
Namespace
Projection Policy
Capabilities
Volume State
```

```text
Cached Projection ≠ Authoritative State
```

## Rekursion und Zyklen

Projection-Ketten müssen kontrolliert werden.

```text
Projection A → B
Projection B → C
Projection C → A
```

Zyklische oder unbegrenzt rekursive Auflösungen müssen erkannt und abgebrochen werden.

Eine Projection darf nicht zu unkontrollierter Namespace-Rekursion führen.

## Entfernte Ziele

Existiert das Target einer Projection nicht mehr:

```text
Projection
    ↓
Missing Target
    ↓
Unavailable
```

Die Projection darf nicht stillschweigend auf eine andere Ressource umgebogen werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ProjectionID
TargetID
Type
Scope
Path
Owner
Source
Priority
State
Resolution Chain
Policy
```

Introspection überträgt keine zusätzliche Authority.

## Normative Anforderungen

1. NovaOS MUSS Filesystem Projections als eigenständiges Namespace-Konzept unterstützen.
2. Jede Projection MUSS eine stabile ProjectionID besitzen.
3. Jede Projection MUSS auf eine stabile Zielidentität zurückführbar sein.
4. Projection Path und TargetID MÜSSEN getrennte Konzepte sein.
5. Eine Projection DARF standardmäßig keine Kopie erzeugen.
6. Mehrere Projections DÜRFEN dieselbe Ressource referenzieren.
7. Mehrere Projections DÜRFEN NICHT als mehrere zugrunde liegende Objekte interpretiert werden.
8. Direct-, Overlay-, Semantic-, Relationship-, Contextual- und Virtual-Projections MÜSSEN unterstützt werden können.
9. `Daten` MUSS als logische Projection realisierbar sein.
10. Projections DÜRFEN mehrere physische Volumes überspannen.
11. Programmspezifische `SYS`-Projections MÜSSEN unterstützt werden.
12. Private `SYS`-Projections DÜRFEN das globale `/System` NICHT physisch verändern.
13. Projection-Auflösung MUSS deterministisch sein.
14. Projection-Sichtbarkeit DARF KEINE Capability verleihen.
15. Zugriffe MÜSSEN gegen die zugrunde liegende Ressource autorisiert werden.
16. Nach erfolgreicher Auflösung SOLLEN stabile Handles verwendet werden.
17. Das Entfernen einer Projection DARF gültige Handles NICHT automatisch widerrufen.
18. Projections MÜSSEN dynamisch erzeugt und entfernt werden können.
19. Projection-Änderungen SOLLEN transaktional sichtbar werden.
20. Projection und Export MÜSSEN unterschiedliche Operationen sein.
21. NovaFile-Projections MÜSSEN dieselbe ObjectID beibehalten.
22. Projection-Caches MÜSSEN invalidierbar sein.
23. Rekursive Projection-Ketten MÜSSEN begrenzt werden.
24. Projection-Zyklen MÜSSEN erkannt werden.
25. Fehlende Targets MÜSSEN als `Unavailable` oder gleichwertig behandelt werden.
26. Eine Projection DARF bei fehlendem Target NICHT stillschweigend auf ein anderes Objekt zeigen.
27. `Unknown` DARF NICHT als gültiger Projection-State interpretiert werden.
28. Projection-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-STORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-OVERLAY-0001`
- `NPSPEC-STORAGE-APP-SYS-0001`
- `NPSPEC-STORAGE-NOVAFILE-PROJECTION-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-OBJECT-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-CAPABILITY-APPLICATION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`

## Ergebnis

```text
Resources
   ↓
Stable IDs
   ↓
Projection Engine
   ├── Direct
   ├── Overlay
   ├── Semantic
   ├── Relationship
   ├── Contextual
   └── Virtual
          ↓
     Namespace View
          ↓
       Resolve
          ↓
    Capability Check
          ↓
   Authorized Handle
```

NovaOS erhält damit eine von physischer Speicherung, Objektidentität und Berechtigung getrennte Projection-Schicht. Dieselbe Ressource kann in unterschiedlichen Kontexten und logischen Strukturen erscheinen, ohne dupliziert zu werden, während stabile IDs, deterministische Auflösung und Capability-Prüfungen die Identität und Sicherheit des zugrunde liegenden Systems erhalten.