# ADR-FILESYSTEM-0003 – Pfadunabhängige Objektidentität

## Status

Angenommen

## Kategorie

File System / Object Identity / Storage Architecture

## Kontext

In NovaOS darf die Identität eines gespeicherten Objekts nicht von seinem aktuellen Pfad abhängen.

Ein Objekt kann durch verschiedene Vorgänge seinen Ort ändern:

- Verschieben,
- Umbenennen,
- Migration,
- Snapshot,
- Restore,
- Reorganisation,
- Provider-Wechsel,
- Volume-Wechsel,
- Replikation.

Eine pfadbasierte Identität würde dadurch die Identität des Objekts verändern.

```text
/System/Data/file.nova
        ↓ Move
/User/Data/file.nova
```

Das physische oder logische Ziel hat sich geändert, das Objekt selbst jedoch nicht.

## Entscheidung

NovaOS verwendet eine **pfadunabhängige Objektidentität**.

```text
Object Identity
      │
      ├── Path
      ├── Volume
      ├── Storage Provider
      └── Physical Location
```

Die Objektidentität bleibt unabhängig von diesen Eigenschaften stabil.

## Grundprinzip

```text
Path identifies a location.
Object Identity identifies the object.
```

Ein Pfad ist eine Referenz auf ein Objekt, nicht dessen Identität.

## Objektidentität

Jedes persistente Objekt muss eine systemweit eindeutige logische Identität besitzen können.

Diese Identität muss von folgenden Eigenschaften unabhängig sein:

```text
Path
Filename
Directory
Volume
Storage Provider
Physical Block Location
Mount Point
Network Location
```

Die konkrete technische Repräsentation der Object Identity wird durch die Storage-NPSPEC festgelegt.

## Pfad als Referenz

Ein Pfad wird als auflösbare Referenz verstanden:

```text
Path
 ↓
Namespace Resolution
 ↓
Object Identity
 ↓
Object
```

Mehrere Pfade können unter geeigneter Semantik auf dasselbe Objekt verweisen.

```text
Path A ─┐
        ├──> Object Identity X
Path B ─┘
```

Dies darf jedoch keine unkontrollierte Aliasbildung oder Umgehung von Security- und Privacy-Regeln ermöglichen.

## Verschieben und Umbenennen

Ein Move oder Rename verändert grundsätzlich die Pfadreferenz, nicht die Objektidentität.

```text
Object X
   │
   ├── Path A
   ↓
   ├── Path B
```

Referenzen, Metadaten und Provenance müssen die Kontinuität des Objekts erhalten können.

## Volume-Wechsel

Auch bei einer Migration auf ein anderes Volume kann die Objektidentität erhalten bleiben, sofern die Storage-Semantik dies garantiert.

```text
Volume A
   ↓ Migration
Volume B

Object Identity = X
```

Ein Volume-Wechsel darf daher nicht automatisch als Erzeugung eines neuen Objekts interpretiert werden.

## Provider-Wechsel

Ein Objekt kann zwischen unterschiedlichen Storage Providern migriert werden.

```text
Provider A
    ↓
Object X
    ↓
Provider B
```

Die Objektidentität bleibt erhalten, wenn eine verifizierte Identitätskontinuität hergestellt werden kann.

## Copy und Clone

Eine Kopie ist grundsätzlich ein neues Objekt.

```text
Object X
   ↓ Copy
Object Y
```

`Y` erhält eine eigene Objektidentität.

Provenance kann jedoch die Beziehung zwischen `X` und `Y` erhalten.

Ein Clone kann abhängig von der Storage-Semantik gemeinsame Datenstrukturen verwenden, besitzt aber ebenfalls eine eigene logische Objektidentität.

## Snapshots und Restore

Snapshots müssen die Objektidentität entsprechend ihrer definierten Semantik erhalten oder bewusst eine neue Identität erzeugen können.

Ein Restore darf nicht automatisch eine neue Identität erzeugen, wenn damit die logische Kontinuität eines bestehenden Objekts wiederhergestellt werden soll.

Die konkrete Snapshot- und Restore-Semantik wird durch die entsprechenden Storage-Spezifikationen definiert.

## Links und Referenzen

Interne Objektverweise sollen nach Möglichkeit auf die Objektidentität und nicht auf einen absoluten Pfad verweisen.

```text
Reference
   ↓
Object Identity
   ↓
Current Location
```

Dadurch bleiben Referenzen bei Move und Rename stabil.

Externe oder legacy Pfade bleiben als separate Pfadsemantik bestehen.

## Pfadauflösung

Die Auflösung eines Pfades muss die aktuelle Zuordnung zur Objektidentität liefern können.

```text
Path
 ↓
Namespace
 ↓
Object Identity
 ↓
Current Storage Location
```

Diese Auflösung kann sich ändern, ohne dass sich die Identität des Objekts ändert.

## Security

Objektidentität und Authority müssen getrennt bleiben.

Das Kennen einer Object Identity erzeugt keine Zugriffsberechtigung.

```text
Object Identity
      ≠
Capability
      ≠
Authority
```

Ein Zugriff muss weiterhin über die normalen Security- und Capability-Mechanismen autorisiert werden.

## Privacy

Eine stabile Objektidentität kann korrelationsfähige Informationen darstellen.

NovaOS darf sie deshalb nicht unnötig an Anwendungen oder externe Systeme weitergeben.

Je nach Kontext können eingeschränkte oder pseudonyme Objektverweise verwendet werden.

```text
Internal Object Identity
        ↓ Policy
Restricted Object Reference
```

## Provenance

Die Objektidentität bildet eine wichtige Grundlage für Provenance.

Transformationen können dadurch nachvollziehbar bleiben:

```text
Object X
   ↓ Transform
Object Y

Y.provenance → X
```

Eine neue Objektidentität bedeutet daher nicht automatisch, dass die Herkunftsinformation verloren geht.

## Distributed Storage

Bei verteiltem Storage bleibt die logische Objektidentität unabhängig von:

```text
Node
Replica
Network Address
Storage Provider
Physical Location
```

Mehrere physische Repräsentationen können zu derselben logischen Objektidentität gehören.

```text
Object X
 ├── Replica A
 ├── Replica B
 └── Replica C
```

## Failure und Recovery

Recovery muss zwischen Objektidentität und physischer Repräsentation unterscheiden können.

Ein beschädigter oder verlorener physischer Standort darf nicht automatisch als Verlust der logischen Identität interpretiert werden.

```text
Object Identity X
      ↓
Physical Representation lost
      ↓
Recovery / Replica / Restore
      ↓
Object Identity X
```

## Legacy-Dateisysteme

Legacy-Dateisysteme verwenden häufig Pfad und Dateiname als primäre externe Identifikation.

NovaOS behandelt diese Darstellung als Kompatibilitätsprojektion.

```text
Nova Object Identity
        ↓
Legacy Path Projection
```

Eine Änderung des Legacy-Pfads verändert nicht die native NovaOS-Objektidentität.

## Normative Anforderungen

1. NovaOS MUSS eine pfadunabhängige Object Identity für persistente Objekte unterstützen.
2. Die Object Identity MUSS unabhängig von Path, Filename, Directory und Volume sein.
3. Die Object Identity MUSS unabhängig von physischer Storage Location sein.
4. Move und Rename DÜRFEN die Object Identity nicht verändern.
5. Eine Migration MUSS die Objektidentität erhalten können, wenn Identitätskontinuität verifiziert werden kann.
6. Ein Copy MUSS grundsätzlich eine neue Object Identity erzeugen.
7. Provenance MUSS Beziehungen zwischen ursprünglichem und neu erzeugtem Objekt erhalten können.
8. Pfade MÜSSEN als Referenzen beziehungsweise Namespace-Auflösungen behandelt werden.
9. Interne Objektverweise SOLLEN nach Möglichkeit auf Object Identity statt auf Pfade referenzieren.
10. Object Identity DARF keine Authority oder Capability erzeugen.
11. Objektidentitäten DÜRFEN nicht unnötig an unprivilegierte oder externe Consumer offengelegt werden.
12. Verteilte Replikate MÜSSEN einer gemeinsamen logischen Objektidentität zugeordnet werden können.
13. Recovery MUSS logische Objektidentität von physischer Repräsentation unterscheiden können.
14. Legacy-Pfadprojektionen DÜRFEN die native Objektidentität nicht ersetzen.
15. Objektidentitäten MÜSSEN über ihren vorgesehenen Lebenszyklus eindeutig bleiben.
16. Die konkrete Repräsentation der Object Identity MUSS von der Pfad- und Namespace-Semantik getrennt spezifiziert werden.

## Konsequenzen

### Positive Konsequenzen

- Move und Rename verändern keine Objektidentität,
- interne Referenzen können stabil bleiben,
- Storage-Migration wird erleichtert,
- Replikation kann mehrere physische Repräsentationen einer Identität zuordnen,
- Provenance und Recovery erhalten eine stabile Referenz,
- Legacy-Pfade bleiben reine Kompatibilitätsprojektionen.

### Negative Konsequenzen

- Object Identity muss zusätzlich zum Pfad verwaltet werden,
- Alias- und Referenzsemantik müssen klar definiert werden,
- stabile Identitäten erfordern besondere Privacy-Betrachtung.

## Verworfene Alternativen

### Pfad als Objektidentität

Verworfen, da Move, Rename und Migration dadurch die Identität verändern würden.

### Volume + Pfad als globale Identität

Verworfen, da Volume-Wechsel und Storage-Migration dadurch unnötig Identitätsbrüche erzeugen.

### Physische Blockadresse als Identität

Verworfen, da Reorganisation, Defragmentierung, Replikation und Provider-Wechsel die physische Position verändern können.

### Legacy-Dateiname als native Identität

Verworfen, da Dateinamen primär Namespace- und Präsentationsmerkmale sind.

## Abhängigkeiten

- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-STORAGE-0011_Legacy_Dateiprojektion_ohne_Duplikation`
- `ADR-STORAGE-0012_Named_Volumes_statt_Laufwerksbuchstaben`
- `ADR-STORAGE-0017_Stabile_localeunabhängige_Dateinamenidentität`
- `ADR-IDENTITY-0002_Stabile_Systemidentitäten`
- `ADR-SECURITY-0001_Capability-Based_Security`
- `ADR-PRIVACY-0003_Semantic_Data_Labels`

## Zugehörige NPSPECs

- `NPSPEC-FILESYSTEM-OBJECT-IDENTITY-0001`
- `NPSPEC-FILESYSTEM-OBJECT-IDENTITY-FORMAT-0001`
- `NPSPEC-FILESYSTEM-OBJECT-IDENTITY-LIFECYCLE-0001`
- `NPSPEC-FILESYSTEM-PATH-RESOLUTION-0001`
- `NPSPEC-FILESYSTEM-OBJECT-MOVE-RENAME-0001`
- `NPSPEC-FILESYSTEM-OBJECT-MIGRATION-0001`
- `NPSPEC-FILESYSTEM-OBJECT-REFERENCE-0001`
- `NPSPEC-FILESYSTEM-OBJECT-IDENTITY-TEST-0001`

## Ergebnis

NovaOS trennt Objektidentität konsequent von dessen aktuellem Speicherort:

```text
Object Identity
      ↓
Namespace / Path
      ↓
Current Location
      ↓
Physical Representation
```

Ein Objekt bleibt damit bei Move, Rename, Migration, Replikation und geeigneten Restore-Vorgängen logisch dasselbe Objekt, während Pfad, Volume und physische Repräsentation unabhängig verändert werden können.