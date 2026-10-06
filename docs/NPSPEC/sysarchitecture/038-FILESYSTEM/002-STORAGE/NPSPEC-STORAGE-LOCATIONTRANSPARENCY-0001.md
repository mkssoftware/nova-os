# NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001 – Nova Storage Location Transparency

## Status

Angenommen

## Kategorie

Storage / Location Transparency

## Zweck

NovaOS ermöglicht den Zugriff auf Speicherobjekte unabhängig von deren aktuellem physischen Speicherort.

Programme arbeiten mit stabilen `ObjectID`s oder autorisierten Handles und müssen nicht wissen, auf welchem Device, Volume oder physischen Bereich ein Objekt gespeichert ist.

```text
ObjectID
   ↓
Location Resolution
   ↓
Current Storage Location
   ↓
Access
```

## Grundprinzipien

```text
Identity ≠ Location
Access ≠ Physical Address
Location Transparency ≠ Authority Transparency
Migration ≠ New Object
```

Die Storage-Schicht übernimmt die Auflösung zwischen logischer Identität und aktuellem Speicherort.

## Transparenter Zugriff

Ein Zugriff erfolgt logisch:

```text
Application
    ↓
ObjectID / Handle
    ↓
Storage Resolution
    ↓
Volume
    ↓
Device
```

Änderungen der darunterliegenden Storage-Struktur sollen für höhere Schichten transparent bleiben.

## Migration

Ein Objekt darf zwischen Speicherorten migriert werden:

```text
Volume A / Device A
        ↓
     Migration
        ↓
Volume B / Device B
```

Dabei bleiben insbesondere erhalten:

- `ObjectID`
- Relations
- semantische Identität
- gültige Projections
- autorisierte Referenzen

Die Location-Mappings werden entsprechend aktualisiert.

## Mehrere Locations

Ein logisches Objekt darf mehrere physische Repräsentationen besitzen, beispielsweise durch:

```text
Replication
Caching
Redundancy
Recovery Copies
```

Die Storage-Schicht bestimmt anhand ihrer Policy, welche geeignete Repräsentation verwendet wird.

Mehrere Locations erzeugen nicht automatisch mehrere `ObjectID`s.

## Location Resolution

Die Auflösung muss den aktuell gültigen Speicherort bestimmen können.

```text
ObjectID
   ↓
Location Map
   ↓
Candidate Locations
   ↓
Policy
   ↓
Selected Location
```

Ungültige oder nicht mehr verfügbare Locations dürfen nicht stillschweigend als gültig behandelt werden.

## Handles

Bereits autorisierte Handles sollen nach einer transparenten Migration weiterhin verwendbar bleiben, sofern:

- dasselbe logische Objekt existiert,
- die Authority weiterhin gültig ist,
- keine Sicherheitsregel dies verhindert.

Programme sollen dadurch nicht gezwungen werden, Objekte nach jeder Storage-Migration erneut über einen Pfad aufzulösen.

## Sicherheit

Location Transparency darf Sicherheitsgrenzen nicht umgehen.

```text
Transparent Location
≠
Transparent Authority
```

Ein Objekt darf nur auf Speicherorte verschoben oder von Speicherorten gelesen werden, die mit geltenden Security-, Capability- und Sovereignty-Regeln vereinbar sind.

## Fehlerverhalten

Kann keine gültige Location bestimmt werden, muss der Zugriff einen definierten Fehler liefern.

Mögliche Zustände:

```text
Available
Migrating
Degraded
Unavailable
Unknown
```

`Unknown` darf nicht als `Available` interpretiert werden.

## Introspection

Autorisierte Komponenten sollen feststellen können:

```text
ObjectID
Active Location
Available Locations
Migration State
Resolution State
```

Physische Details dürfen entsprechend der jeweiligen Authority eingeschränkt werden.

## Normative Anforderungen

1. NovaOS MUSS Storage Location Transparency unterstützen.
2. Programme SOLLEN Speicherobjekte über stabile Identitäten oder Handles adressieren.
3. Physische Speicherorte DÜRFEN NICHT Bestandteil der `ObjectID` sein.
4. Migration DARF die logische Objektidentität nicht verändern.
5. Location Resolution MUSS den aktuell gültigen Speicherort bestimmen können.
6. Mehrere physische Locations DÜRFEN dasselbe logische Objekt repräsentieren.
7. Bestehende Handles SOLLEN transparente Migration überleben können.
8. Location Transparency DARF KEINE zusätzliche Authority erzeugen.
9. Security- und Sovereignty-Regeln MÜSSEN bei der Location-Auswahl eingehalten werden.
10. Nicht auflösbare Locations MÜSSEN einen definierten Fehlerzustand erzeugen.
11. `Unknown` DARF NICHT als verfügbare Location interpretiert werden.
12. Location- und Migration-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-LOCATION-0001`
- `NPSPEC-STORAGE-DEVICE-0001`
- `NPSPEC-FSSTORAGE-VOLUME-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`

## Ergebnis

NovaOS entkoppelt den Zugriff auf Speicherobjekte von deren physischem Speicherort. Objekte können zwischen Devices und Volumes verschoben, repliziert oder neu platziert werden, während ihre stabile Identität und bestehende autorisierte Referenzen erhalten bleiben.