# NPSPEC-STORAGE-NAMESPACE-0001 – Nova Storage Namespace

## Status

Angenommen

## Kategorie

Storage / Namespace / Object Model / VFS

## Zweck

NovaOS definiert einen einheitlichen Storage-Namespace zur Navigation und Organisation persistenter Objekte.

```text
Namespace
├── System:/
├── Benutzer:/
├── Apps:/
├── Boot:/
└── Daten:/
```

Der Namespace stellt eine logische Sicht auf Storage-Objekte bereit und ist von deren physischem Speicherort getrennt.

## Grundprinzip

```text
Object Identity ≠ Path
Namespace ≠ Physical Storage
Volume Name ≠ VolumeID
```

Ein Pfad beschreibt, wie ein Objekt innerhalb eines Namespace erreicht wird. Er definiert nicht dessen Identität.

## Namespace-Modell

Ein Namespace besteht aus hierarchischen Einträgen.

```text
Namespace
   ↓
Volume
   ↓
Directory
   ↓
Entry
   ↓
ObjectID
```

Ein Eintrag verbindet einen sichtbaren Namen mit einem Storage-Objekt.

## Benannte Volumes

NovaOS verwendet benannte Volumes statt Laufwerksbuchstaben.

```text
System:/
Benutzer:/
Boot:/
Apps:/
Daten:/
```

Die sichtbare Bezeichnung ist unabhängig von der stabilen `VolumeID`.

```text
VolumeID ≠ "System"
```

Volumes dürfen umbenannt werden, ohne ihre Identität zu verlieren.

## Pfade

Ein NovaOS-Pfad beschreibt eine Position innerhalb eines Namespace.

Beispiel:

```text
Benutzer:/Matthias/Dokumente/Projekt.nf
```

Ein Pfad darf sich durch Rename, Move oder Mount-Änderungen verändern, während die `ObjectID` bestehen bleibt.

## Namespace-Einträge

Ein Eintrag enthält mindestens:

```text
Name
ObjectID
Type
Parent
```

Optional:

```text
Attributes
Relationship
Mount Information
```

Mehrere Namespace-Einträge dürfen auf dasselbe Objekt verweisen, sofern die Storage-Policy dies erlaubt.

## Mounts

Dateisysteme und Volumes werden über kontrollierte Mounts in den Namespace eingebunden.

```text
Storage Provider
      ↓
VFS Mount
      ↓
Namespace
```

Die physische Gerätehierarchie muss dadurch nicht direkt im Benutzer-Namespace sichtbar sein.

## Objektauflösung

Pfadauflösung erfolgt über Namespace-Einträge bis zur stabilen Objektidentität.

```text
Path
 ↓
Namespace Resolution
 ↓
ObjectID
 ↓
VFS Object
```

Nach erfolgreicher Auflösung sollen interne Operationen bevorzugt mit stabilen Objekt-Referenzen statt erneut mit Pfaden arbeiten.

## Links

NovaOS darf Referenzen auf andere Objekte im Namespace darstellen.

```text
Entry A
   ↓
ObjectID B
```

Links sollen nach Möglichkeit auf stabile Identitäten statt auf fragile Textpfade verweisen.

Legacy-Dateisysteme dürfen weiterhin pfadbasierte Links bereitstellen.

## Isolation

Prozesse, Container oder Sicherheitsdomänen dürfen unterschiedliche Namespace-Sichten besitzen.

```text
Global Storage
   ↓
Namespace View
   ↓
Process / Service
```

Nicht sichtbare Objekte dürfen dadurch vollständig aus einer Namespace-Sicht ausgeblendet werden.

## Transaktionen

Änderungen am Namespace sollen transaktional ausführbar sein.

Dazu gehören insbesondere:

```text
Create
Delete
Rename
Move
Link
Mount
Unmount
```

Teilweise ausgeführte Namespace-Änderungen dürfen keinen inkonsistenten Zustand hinterlassen.

## Sicherheit

Die Sichtbarkeit eines Pfades erzeugt keine automatische Zugriffsberechtigung.

```text
Visible Object
      ≠
Authorized Object
```

Nach der Namespace-Auflösung müssen weiterhin Capability- und Security-Regeln gelten.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Namespace
Volumes
Mounts
Paths
ObjectIDs
Namespace Views
Resolution State
```

Die Ausgabe muss entsprechend dem Security Context gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS einen einheitlichen Storage-Namespace bereitstellen.
2. Pfade und Objektidentitäten MÜSSEN logisch getrennt sein.
3. NovaOS MUSS benannte Volumes ohne Laufwerksbuchstaben unterstützen.
4. `VolumeID` und sichtbarer Volume-Name MÜSSEN getrennt sein.
5. Rename und Move DÜRFEN die `ObjectID` eines Objekts NICHT verändern.
6. Namespace-Einträge SOLLEN stabile `ObjectID`s referenzieren.
7. Physische Storage-Topologie DARF vom logischen Namespace abstrahiert werden.
8. Prozesse und Sicherheitsdomänen DÜRFEN eingeschränkte Namespace-Sichten besitzen.
9. Namespace-Änderungen SOLLEN transaktional ausführbar sein.
10. Sichtbarkeit im Namespace DARF KEINE Zugriffsberechtigung erzeugen.
11. Interne Operationen SOLLEN nach erfolgreicher Auflösung stabile Objekt-Referenzen verwenden.
12. Volumes, Mounts und Namespace-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-STORAGE-0013`

## Ergebnis

```text
Physical Storage
       ↓
      VFS
       ↓
Logical Namespace
       ↓
Path Resolution
       ↓
Stable ObjectID
```

NovaOS erhält damit einen pfadunabhängigen Storage-Namespace mit benannten Volumes, stabilen Objektidentitäten und kontrollierten Namespace-Sichten.