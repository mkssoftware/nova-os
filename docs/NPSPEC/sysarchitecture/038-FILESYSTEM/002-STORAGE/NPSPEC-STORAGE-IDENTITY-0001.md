# NPSPEC-STORAGE-IDENTITY-0001 – Nova Storage Identity

## Status

Angenommen

## Kategorie

Storage / Identity

## Zweck

NovaOS definiert stabile Identitäten für Ressourcen der Storage-Architektur.

Physischer Speicherort, sichtbarer Name und Namespace-Pfad werden konsequent von der eigentlichen Identität getrennt.

```text
Identity ≠ Name ≠ Path ≠ Location
```

## Identitätsebenen

NovaOS unterscheidet mindestens:

```text
DeviceID   → Storage Device
VolumeID   → logisches Volume
ObjectID   → gespeichertes Objekt
MountID    → Mount-Instanz
```

Jede ID identifiziert ausschließlich die Ressource ihrer eigenen Ebene.

```text
DeviceID ≠ VolumeID ≠ ObjectID ≠ MountID
```

## Stabile Identität

Eine Storage-Identität bleibt stabil, solange dieselbe logische Ressource existiert.

Folgende Änderungen dürfen die Identität nicht automatisch verändern:

- Umbenennen
- Mountpoint-Änderung
- Namespace-Änderung
- Projection-Änderung
- physische Verlagerung
- Migration zwischen Devices oder Volumes

Beispiel:

```text
ObjectID 42
Device A
   ↓ Migration
Device B
   ↓
ObjectID 42
```

## Neue Identität

Eine neue Identität entsteht, wenn tatsächlich eine neue logische Ressource erzeugt wird.

```text
Copy Object
    ↓
New ObjectID
```

Eine reine Projection erzeugt dagegen keine neue Objektidentität.

```text
Projection
    ↓
Same ObjectID
```

## Identität und Inhalt

Objektidentität und Inhaltsidentität bleiben getrennt.

```text
ObjectID ≠ ContentID
```

Der Inhalt eines Objekts kann sich ändern, während dessen `ObjectID` erhalten bleibt.

Umgekehrt können zwei unterschiedliche Objekte denselben Inhalt besitzen und trotzdem unterschiedliche `ObjectID`s haben.

## Persistenz

Persistente Storage-IDs müssen so gespeichert werden, dass sie Neustarts und normale Storage-Reorganisationen überleben.

Gelöschte IDs dürfen nicht stillschweigend für eine andere, unabhängige Ressource wiederverwendet werden.

## Resolution

Storage-IDs werden auf den aktuellen Zustand der Ressource aufgelöst.

```text
Stable ID
   ↓
Resolution
   ↓
Current Location / State
```

Dadurch bleiben Referenzen unabhängig von veränderlichen Pfaden und Speicherorten.

## Sicherheit

Kenntnis einer Storage-ID gewährt keine Zugriffsberechtigung.

```text
Know ID
  ↓
Capability Check
  ↓
Authorized Handle
```

Es gilt:

```text
Identity ≠ Authority
```

## Introspection

Autorisierte Komponenten sollen Identität und aktuellen Zustand einer Storage-Ressource abfragen können, ohne deren stabile ID zu verändern.

## Normative Anforderungen

1. NovaOS MUSS stabile IDs für Storage-Ressourcen verwenden.
2. Device, Volume, Object und Mount MÜSSEN getrennte Identitäten besitzen.
3. Namen, Pfade und Speicherorte DÜRFEN NICHT als primäre Identität verwendet werden.
4. Migration DARF die Identität derselben logischen Ressource nicht verändern.
5. Kopieren MUSS für das neue logische Objekt eine neue `ObjectID` erzeugen.
6. Projections DÜRFEN keine neue `ObjectID` erzeugen.
7. `ObjectID` und `ContentID` MÜSSEN getrennt bleiben.
8. Persistente IDs MÜSSEN Neustarts überleben.
9. Gelöschte IDs DÜRFEN NICHT stillschweigend für unabhängige Ressourcen wiederverwendet werden.
10. Kenntnis einer ID DARF KEINE Authority verleihen.
11. Storage-IDs MÜSSEN auf den aktuellen Ressourcenstatus auflösbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-DEVICE-0001`
- `NPSPEC-FSSTORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-LOCATION-0001`
- `NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-STORAGE-MOUNT-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`

## Ergebnis

NovaOS erhält ein durchgängiges Storage-Identitätsmodell, bei dem Ressourcen unabhängig von Namen, Pfaden und physischen Speicherorten stabil referenziert werden können. Damit bleiben Referenzen auch bei Migration, Mount-Änderungen und Storage-Reorganisationen erhalten.