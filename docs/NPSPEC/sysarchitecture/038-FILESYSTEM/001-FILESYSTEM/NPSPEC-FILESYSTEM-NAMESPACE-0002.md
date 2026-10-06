# NPSPEC-FILESYSTEM-NAMESPACE-0002 – Nova Filesystem Namespace

## Status

Angenommen

## Kategorie

Filesystem / Namespace / Unified Namespace

## Zweck

NovaOS verwendet einen einheitlichen globalen Filesystem-Namespace mit `/` als Root.

Volumes, Systembereiche, Benutzerbereiche, Programme, Solutions und weitere Ressourcen werden innerhalb dieses gemeinsamen Namensraums eingebunden.

```text
/
├── System/
├── Benutzer/
├── Apps/
├── Solutions/
├── Boot/
└── Volumes/
```

Der Namespace beschreibt die logische Sicht des Systems. Physischer Speicherort, Datenträger, Partition, Dateisystem und Objektidentität bleiben davon getrennt.

## Grundprinzipien

```text
Namespace ≠ Physical Storage
Path ≠ Object Identity
Mountpoint ≠ Volume Identity
Visible ≠ Authorized
Projection ≠ Copy
Namespace ≠ Capability
```

NovaOS verwendet keine Laufwerksbuchstaben und keine voneinander getrennten Dateisystembäume nach dem Muster `C:`, `D:` oder vergleichbarer Konzepte.

## Globaler Root-Namespace

`/` bildet den gemeinsamen Einstiegspunkt des NovaOS-Dateisystems.

```text
/
```

Alle eingebundenen Filesystem-Ressourcen erscheinen logisch unterhalb dieses Namespace.

Ein Volume erzeugt daher keinen eigenständigen Root-Namespace.

## Volumes

Volumes besitzen stabile IDs und können zusätzlich einen menschenlesbaren Namen besitzen.

```text
Volume
├── VolumeID
├── Name
├── Filesystem
└── Mountpoint
```

Beispiel:

```text
VolumeID: 7f31...
Name: Daten
Mountpoint: /Volumes/Daten
```

Es gilt:

```text
VolumeID ≠ Volume Name
VolumeID ≠ Mountpoint
VolumeID ≠ Physical Device
```

Ein Volume kann auf einen anderen Datenträger verschoben oder an einer anderen Stelle eingebunden werden, ohne seine Identität zu verlieren.

## Mounts

Zusätzliche Volumes werden in den bestehenden Namespace eingebunden.

```text
Physical Volume
      ↓
VolumeID
      ↓
Mount
      ↓
/Volumes/Daten
```

Mount und Unmount verändern die Namespace-Sicht, nicht die Identität des Volumes.

## Stabile Objektidentität

Dateien und andere persistente Objekte dürfen nicht ausschließlich über ihren Pfad identifiziert werden.

```text
ObjectID
   ↓
Namespace Entry
   ↓
/Benutzer/Matthias/Dokumente/datei.nf
```

Eine Datei kann verschoben oder umbenannt werden:

```text
Old Path
   ↓
ObjectID
   ↓
New Path
```

Die ObjectID bleibt erhalten.

## Namespace Entries

Ein Namespace Entry verbindet einen Namen innerhalb eines Containers mit einer Ressource.

```text
NamespaceEntry
├── Name
├── Parent
├── TargetID
├── Type
└── Attributes
```

Das Target kann beispielsweise sein:

```text
File
Directory
NovaFile
Volume
Device Projection
Virtual Resource
Semantic Projection
```

## Projektionen

NovaOS darf virtuelle Namespace-Bereiche erzeugen, die keine direkte physische Verzeichnisstruktur besitzen.

```text
Objects
+
Metadata
+
Relationships
+
Semantic Types
      ↓
Projection
      ↓
Filesystem View
```

Eine Projektion erzeugt keine Kopie des zugrunde liegenden Objekts.

```text
Projection ≠ Duplicate
```

## Kontextabhängige Sicht

Der globale Namespace kann abhängig vom Ausführungskontext kontrolliert projiziert werden.

```text
Global Namespace
       ↓
Context Projection
       ↓
Effective Namespace
```

Kontexte können sein:

```text
User
Process
Program
Solution
Workspace
Recovery Environment
```

Dadurch kann NovaOS Ressourcen ausblenden, ergänzen oder kontrolliert überlagern, ohne separate inkompatible Dateisystemmodelle einzuführen.

## Application SYS Overlay

Programme dürfen private Systemabhängigkeiten in ihrem eigenen Programmkontext speichern.

Beispiel:

```text
/Apps/Example/
├── Program/
└── SYS/
    ├── Libraries/
    ├── Runtime/
    └── Dependencies/
```

Für das gestartete Programm kann dieser Bereich logisch in dessen Systemansicht eingeblendet werden.

```text
Global /System
       +
Application SYS
       ↓
Effective System View
```

Die Dateien bleiben physisch im Programmkontext.

```text
Private SYS
≠
Global /System Modification
```

## Overlay Resolution

Bei einem Application Overlay muss die Auflösungsreihenfolge eindeutig definiert sein.

```text
Application SYS
      ↓
Global System
      ↓
Resolved Resource
```

Ein privates Overlay darf ausschließlich für den dafür vorgesehenen Kontext gelten.

Andere Programme sehen weiterhin ihre eigene beziehungsweise die globale Systemansicht.

## Globale Systemänderungen

Benötigt ein Programm tatsächlich Änderungen an `/System`, reicht sein privates `SYS`-Overlay nicht aus.

```text
Application
    ↓
Request Global System Modification
    ↓
Capability / Policy Check
    ↓
Allow / Deny
```

Globale Systemänderungen benötigen ausdrücklich die dafür erforderliche Authority.

## Solutions

Solutions können eigene logische Arbeitsbereiche besitzen.

```text
/Solutions/<Solution>/
```

Eine Solution darf Ressourcen und Fähigkeiten zusammenführen, ohne deren physische Speicherorte vereinheitlichen zu müssen.

Solution-Namespace und Capability-System bleiben getrennt.

## Capability Integration

Namespace-Auflösung gewährt keine Authority.

```text
Path Resolution
      ↓
Object / Resource
      ↓
Capability Check
      ↓
Operation
```

Es gilt:

```text
Can Resolve
≠
Can Read

Can See
≠
Can Modify

Path Knowledge
≠
Authority
```

Berechtigungen werden nicht aus Pfaden abgeleitet.

## Handles

Nach erfolgreicher Auflösung soll NovaOS bevorzugt mit stabilen Handles arbeiten.

```text
Path
 ↓
Resolve
 ↓
ObjectID
 ↓
Authorized Handle
```

Nachfolgende Operationen müssen dadurch nicht wiederholt von einem veränderlichen Pfad abhängig sein.

## Namespace-Isolation

Ein Prozess kann eine eingeschränkte Sicht erhalten.

```text
Global Namespace
├── System
├── Benutzer
├── Apps
├── Solutions
└── Volumes
        ↓
Namespace Policy
        ↓
Process View
├── System
├── Allowed User Data
└── Application Resources
```

Nicht sichtbare Ressourcen werden dadurch nicht automatisch gelöscht oder verändert.

## Devices und Volumes

Hardwaregeräte und Filesystem-Volumes bleiben unterschiedliche Konzepte.

```text
Physical Device
      ↓
Partition / Storage Region
      ↓
Filesystem
      ↓
Volume
      ↓
Namespace Mount
```

Daher gilt:

```text
Device ≠ Filesystem
Filesystem ≠ Volume
Volume ≠ Mountpoint
Mountpoint ≠ Identity
```

## Boot und Recovery

`/Boot` stellt den für NovaOS vorgesehenen Boot-/Recovery-Bereich logisch dar.

Recovery-Umgebungen dürfen eine eigene kontrollierte Namespace-Sicht auf dasselbe System erzeugen.

```text
Normal Namespace
      ↕
Stable IDs
      ↕
Recovery Namespace
```

Dadurch bleiben Ressourcen auch außerhalb des normalen Systemstarts eindeutig identifizierbar.

## Transaktionale Namespace-Änderungen

Änderungen an der Namespace-Struktur sollen transaktional erfolgen.

Dazu gehören insbesondere:

```text
Mount
Unmount
Rename
Move
Projection Add
Projection Remove
Overlay Change
Volume Replacement
```

Andere Komponenten dürfen keinen undefinierten Zwischenzustand beobachten.

## Live Evolution

Namespace-Projektionen dürfen zur Laufzeit verändert werden.

Bereits geöffnete Handles bleiben gültig, solange:

```text
Object Exists
AND
Handle Valid
AND
Authority Valid
```

Eine Pfadänderung allein darf einen bestehenden gültigen Handle nicht ungültig machen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Namespace Root
Mountpoints
VolumeIDs
Volume Names
ObjectIDs
Active Projections
Application Overlays
Namespace Scope
Resolution Rules
Isolation Policy
```

Introspection darf keine zusätzliche Authority übertragen.

## Normative Anforderungen

1. NovaOS MUSS einen gemeinsamen globalen Root-Namespace `/` verwenden.
2. NovaOS DARF keine Laufwerksbuchstaben als primäres Volume-Modell verwenden.
3. Volumes MÜSSEN stabile VolumeIDs besitzen können.
4. VolumeID, Volume-Name und Mountpoint MÜSSEN getrennte Konzepte sein.
5. Zusätzliche Volumes MÜSSEN in den gemeinsamen Namespace mountbar sein.
6. Pfade DÜRFEN NICHT als dauerhafte Objektidentität verwendet werden.
7. Persistente Objekte MÜSSEN über stabile ObjectIDs referenzierbar sein können.
8. Rename und Move DÜRFEN die ObjectID NICHT allein aufgrund der Pfadänderung verändern.
9. Namespace-Projektionen DÜRFEN virtuelle Ressourcen darstellen.
10. Projektionen DÜRFEN keine unnötigen Kopien des zugrunde liegenden Objekts erzeugen.
11. Kontextabhängige Namespace-Sichten MÜSSEN unterstützt werden können.
12. Programme MÜSSEN private `SYS`-Overlays verwenden können.
13. Private `SYS`-Abhängigkeiten MÜSSEN physisch im Programmkontext verbleiben können.
14. Private `SYS`-Overlays DÜRFEN das globale `/System` NICHT physisch verändern.
15. Änderungen an `/System` MÜSSEN separate Authority erfordern.
16. Overlay-Auflösungsregeln MÜSSEN deterministisch sein.
17. Namespace-Sichtbarkeit DARF KEINE Capability erzeugen.
18. Berechtigungen DÜRFEN NICHT allein aus Pfaden abgeleitet werden.
19. Nach erfolgreicher Auflösung SOLLEN stabile autorisierte Handles verwendet werden.
20. Namespace-Isolation MUSS pro Ausführungskontext möglich sein.
21. Device, Filesystem, Volume und Mountpoint MÜSSEN getrennte Konzepte bleiben.
22. Normal- und Recovery-Umgebung SOLLEN Ressourcen über dieselben stabilen Identitäten referenzieren können.
23. Namespace-Änderungen SOLLEN transaktional sichtbar werden.
24. Bestehende gültige Handles SOLLEN Rename-, Move- und Projection-Änderungen überleben.
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
- `NPSPEC-CAPABILITY-APPLICATION-0001`
- `NPSPEC-CAPABILITY-ISOLATION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`

## Ergebnis

```text
Physical Devices
       ↓
Filesystems
       ↓
Volumes + Stable IDs
       ↓
      /
 ┌─────┼────────┬──────────┐
 ↓     ↓        ↓          ↓
System Benutzer Apps    Volumes
                 ↓
            Private SYS
              Overlay

Path
 ↓
Namespace Resolution
 ↓
ObjectID
 ↓
Capability Check
 ↓
Authorized Handle
```

NovaOS erhält damit einen einheitlichen Filesystem-Namespace, in dem alle Speicherressourcen unter einem gemeinsamen Root erscheinen, während Pfad, Mountpoint, Volume, physischer Speicherort und Objektidentität strikt getrennt bleiben. Kontextabhängige Projektionen und private `SYS`-Overlays ermöglichen Isolation und anwendungsspezifische Abhängigkeiten, ohne den globalen Systembestand oder das Capability-Sicherheitsmodell aufzuweichen.