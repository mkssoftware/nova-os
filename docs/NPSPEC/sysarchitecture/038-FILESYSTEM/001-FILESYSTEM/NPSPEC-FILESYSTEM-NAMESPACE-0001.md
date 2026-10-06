# NPSPEC-FILESYSTEM-NAMESPACE-0001 – Nova Filesystem Namespace

## Status

Ersetzt

## Ersetzt durch

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`

## Hinweis

Diese Fassung bleibt als historische Grundlage erhalten. Maßgeblich für Implementierung und spätere Spezifikationen ist `NPSPEC-FILESYSTEM-NAMESPACE-0002`, weil dort der globale Root-Namespace `/`, die Standardbereiche und die Trennung von Namespace, Volume, ObjectID und Projection präzisiert sind.

## Kategorie

Filesystem / Namespace / Projection

## Zweck

NovaOS trennt die physische Speicherung konsequent von der logischen Sicht auf Dateien, Objekte, Volumes und Systemressourcen.

Der Filesystem Namespace definiert, **wie Ressourcen sichtbar und adressierbar erscheinen**, nicht wo sie physisch gespeichert sind.

```text
Physical Storage
      ↓
Filesystem / Objects
      ↓
Stable Object Identity
      ↓
Namespace Projection
      ↓
Logical Path
```

## Grundprinzipien

```text
Namespace ≠ Physical Storage
Path ≠ Object Identity
Volume Name ≠ VolumeID
Visible ≠ Authorized
Mount Location ≠ Resource Identity
Namespace ≠ Capability
Projection ≠ Copy
```

Pfade sind Ansichten auf Ressourcen und dürfen nicht deren dauerhafte Identität darstellen.

## Stable Identity

Filesystem-Ressourcen besitzen eine stabile Identität unabhängig von ihrem sichtbaren Pfad.

```text
ObjectID
   ↓
Namespace Projection
   ↓
System:/...
```

Eine Ressource kann umbenannt, verschoben oder in einem anderen Namespace dargestellt werden, ohne ihre Identität zu verlieren.

```text
Path A ─┐
Path B ─┼──→ ObjectID
Path C ─┘
```

## Namespace-Struktur

Ein Namespace bildet eine logische Hierarchie:

```text
Namespace
   ↓
Volume / Projection
   ↓
Directory
   ↓
Entry
   ↓
ObjectID
```

Der Namespace kann reale Filesystem-Einträge sowie virtuelle oder projizierte Einträge enthalten.

## Benannte Volumes

NovaOS verwendet keine klassischen Laufwerksbuchstaben.

Logische Volumes können beispielsweise erscheinen als:

```text
System:/
Benutzer:/
Boot:/
Apps:/
Daten:/
```

Der sichtbare Name ist nicht die Identität des Volumes.

```text
System:
   ↓
VolumeID
```

Ein anderes Volume kann später dieselbe Systemrolle übernehmen.

## Volume und Namespace

Volumes sind logische Speichereinheiten und können innerhalb verschiedener Namespaces unterschiedlich dargestellt werden.

```text
VolumeID
   ├──→ Namespace A → System:/
   └──→ Namespace B → Recovery/System:/
```

Ein Volume ist weder an einen bestimmten Pfad noch an ein bestimmtes physisches Gerät gebunden.

## Namespace Projection

NovaOS unterstützt deklarative Namespace-Projektionen.

Eine Projektion kann aus folgenden Informationen erzeugt werden:

```text
Objects
Metadata
Semantic Types
Relationships
Context
Policies
```

Beispiel:

```text
Object Storage
      ↓
Metadata + Semantic Type
      ↓
Projection
      ↓
Daten:/Bilder/...
```

`Daten` dient insbesondere als logische Namespace-Projektion und muss nicht einer einzelnen physischen Partition entsprechen.

## Projection Scope

Projektionen können unterschiedliche Gültigkeitsbereiche besitzen:

```text
System
User
Process
Program
Solution
Workspace
```

Dadurch können unterschiedliche Ausführungskontexte unterschiedliche Sichten auf dieselben zugrunde liegenden Ressourcen besitzen.

## Application Namespace

Programme können einen privaten Namespace erhalten.

Standardmäßig sollen anwendungsspezifische Erweiterungen nicht automatisch den globalen Namespace verändern.

```text
Global Namespace
      +
Application Projection
      ↓
Application Namespace
```

## Private SYS Overlay

Programme dürfen private Systemabhängigkeiten über einen per-Application `SYS`-Namespace bereitstellen.

```text
Application
├── Program Data
└── SYS
    ├── Framework
    ├── Libraries
    └── Dependencies
```

Für das Programm erscheinen diese Ressourcen logisch im erwarteten System-Namespace.

Physisch bleiben sie jedoch Bestandteil des Programmkontexts.

```text
Application Physical Storage
        ↓
Private SYS Overlay
        ↓
Application View of System
```

Dadurch können private Abhängigkeiten bereitgestellt werden, ohne den globalen Systembestand zu verändern.

## Global System Namespace

Das globale System bleibt getrennt vom privaten `SYS`-Overlay.

```text
Global System
      +
Application SYS Overlay
      ↓
Effective Application View
```

Das Overlay darf globale Systemressourcen überlagern oder ergänzen, soweit dies durch die jeweilige Namespace-Policy erlaubt ist.

Eine tatsächliche Änderung des globalen `System`-Bereichs benötigt separate Berechtigungen.

```text
Private SYS Write
≠
Global System Write
```

## Resolution Order

Die effektive Sicht kann beispielsweise nach folgendem Prinzip aufgelöst werden:

```text
Application Private Projection
        ↓
Scoped Namespace
        ↓
Global Namespace
        ↓
Underlying Volume / Object
```

Die konkrete Priorität muss durch Namespace-Policy eindeutig bestimmt sein.

## Namespace Isolation

Namespaces können voneinander isoliert werden.

Ein Prozess oder Programm sieht nur die für seinen Kontext freigegebenen Projektionen.

```text
Process A Namespace
├── System
├── Benutzer
└── App SYS

Process B Namespace
├── System
└── Restricted Data
```

Namespace-Isolation ersetzt jedoch keine Capability-Prüfung.

## Capability Integration

Ein sichtbarer Pfad gewährt keine Berechtigung.

```text
Visible Resource
      ↓
Capability Check
      ↓
Authorized?
```

Es gilt:

```text
Knowledge of Path
≠
Authority

Namespace Visibility
≠
Capability Possession
```

Auch eine über ein Overlay sichtbare Ressource muss den normalen Capability-, Security-, Trust- und Policy-Regeln unterliegen.

## Path Resolution

Die Pfadauflösung arbeitet innerhalb des aktuellen Namespace-Kontexts.

```text
Logical Path
     ↓
Namespace Resolver
     ↓
Projection / Volume
     ↓
Entry
     ↓
ObjectID
```

Nach erfolgreicher Auflösung soll intern bevorzugt mit stabilen Handles beziehungsweise ObjectIDs weitergearbeitet werden.

Dadurch bleiben offene Ressourcen unabhängig von späteren Pfadänderungen stabil referenzierbar.

## Virtual Views

Der Namespace darf virtuelle Ansichten bereitstellen.

Beispiele:

```text
Laufwerke
Geräte
Volumes
Locations
Semantic Collections
Workspace Views
```

Diese Ansichten müssen keine direkte physische Verzeichnisstruktur besitzen.

Insbesondere gilt:

```text
Device ≠ Volume
Volume ≠ Location
Location ≠ Path
```

## Transactionale Änderungen

Namespace-Änderungen sollen transaktional erfolgen können.

Dazu gehören:

```text
Mount
Unmount
Projection Add
Projection Remove
Rename
Overlay Change
Volume Role Change
```

Andere Komponenten dürfen keinen undefinierten Zwischenzustand beobachten.

## Live Evolution

Namespace-Projektionen dürfen zur Laufzeit verändert werden.

Bestehende Handles auf bereits aufgelöste Objekte sollen dadurch nicht ungültig werden, solange das zugrunde liegende Objekt weiterhin existiert und die Berechtigung gültig bleibt.

```text
Namespace Changed
≠
Object Identity Changed
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
NamespaceID
Namespace Scope
Visible Volumes
VolumeIDs
Active Projections
Overlay Sources
Resolution Rules
ObjectID Mapping
Namespace Owner
Isolation Policy
```

Dabei dürfen keine Capabilities oder geschützten Ressourcen allein durch Introspection übertragen werden.

## Normative Anforderungen

1. NovaOS MUSS physische Speicherung und logischen Namespace trennen.
2. Pfade DÜRFEN NICHT als dauerhafte Objektidentität verwendet werden.
3. Filesystem-Ressourcen MÜSSEN über stabile Identitäten referenzierbar sein.
4. Volume-Namen DÜRFEN NICHT mit VolumeIDs gleichgesetzt werden.
5. NovaOS MUSS benannte Volumes ohne Laufwerksbuchstaben unterstützen.
6. Namespace-Projektionen MÜSSEN virtuelle und physische Ressourcen darstellen können.
7. Projektionen MÜSSEN kontextabhängig definierbar sein.
8. User-, Process-, Program-, Solution- und Workspace-spezifische Namespaces MÜSSEN unterstützt werden können.
9. Programme MÜSSEN private Namespace-Overlays verwenden können.
10. Private Systemabhängigkeiten MÜSSEN über einen per-Application `SYS`-Namespace darstellbar sein.
11. Private `SYS`-Inhalte MÜSSEN physisch vom globalen Systembestand getrennt bleiben können.
12. Änderungen am globalen `System` MÜSSEN separate Berechtigungen erfordern.
13. Namespace-Sichtbarkeit DARF KEINE Capability verleihen.
14. Pfadkenntnis DARF NICHT als Authority interpretiert werden.
15. Namespace-Isolation MUSS mit Capability-Security kombinierbar sein.
16. Pfadauflösung MUSS innerhalb des aktuellen Namespace-Kontexts erfolgen.
17. Nach Pfadauflösung SOLLEN stabile ObjectIDs oder Handles verwendet werden.
18. Namespace-Projektionen DÜRFEN Metadaten, Semantic Types und Relationships verwenden.
19. `Daten` MUSS als logische Namespace-Projektion nutzbar sein.
20. Devices, Volumes und Locations MÜSSEN als unterschiedliche Konzepte behandelt werden.
21. Virtuelle Namespace-Einträge DÜRFEN ohne entsprechende physische Verzeichnisstruktur existieren.
22. Namespace-Änderungen SOLLEN transaktional sichtbar werden.
23. Namespace-Änderungen DÜRFEN stabile Objektidentitäten NICHT verändern.
24. Bestehende Handles SOLLEN Namespace- und Pfadänderungen überleben können.
25. Namespace-Struktur MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-FSSTORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-OVERLAY-0001`
- `NPSPEC-STORAGE-APP-SYS-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-CAPABILITY-APPLICATION-0001`

## Ergebnis

```text
Physical Storage
        ↓
Volumes + Objects
        ↓
Stable Identity
        ↓
Namespace Engine
   ┌────┼───────────┐
   ↓    ↓           ↓
System User     Application
View   View        View
                  +
             Private SYS
               Overlay
```

NovaOS erhält damit einen vollständig von der physischen Speicherung entkoppelten Filesystem Namespace. Pfade, Volume-Namen und Projektionen dienen der Navigation und Darstellung, während stabile IDs die eigentliche Identität bilden. Programme können eigene isolierte Sichten und private `SYS`-Overlays besitzen, ohne den globalen Systembestand zu verändern oder allein durch Sichtbarkeit zusätzliche Berechtigungen zu erhalten.
