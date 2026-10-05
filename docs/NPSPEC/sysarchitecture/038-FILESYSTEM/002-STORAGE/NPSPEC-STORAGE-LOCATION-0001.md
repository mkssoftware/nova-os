# NPSPEC-STORAGE-LOCATION-0001 – Nova Storage Location

## Status

Angenommen

## Kategorie

Storage / Location

## Zweck

NovaOS trennt die logische Identität eines Speicherobjekts von seinem tatsächlichen Speicherort.

```text
Identity ≠ Location
```

Dateien und andere Storage-Objekte können verschoben, migriert oder auf andere Volumes verlagert werden, ohne ihre `ObjectID` zu verlieren.

## Grundprinzipien

- Pfade sind keine physische Speicheridentität.
- `ObjectID` bleibt unabhängig vom Speicherort stabil.
- `VolumeID` beschreibt das logische Volume, nicht die physische Position eines Objekts.
- Speicherorte dürfen sich während der Lebenszeit eines Objekts ändern.
- Anwendungen sollen den konkreten physischen Speicherort normalerweise nicht kennen müssen.
- Location Transparency darf Sicherheitsgrenzen nicht umgehen.

## Location-Modell

```text
StorageLocation
├── ObjectID
├── VolumeID
├── DeviceID
├── PhysicalLocation
└── State
```

`PhysicalLocation` kann abhängig vom Storage-System beispielsweise Blockbereiche, Extents oder andere interne Adressierungen beschreiben.

Diese Informationen gehören zur Storage-Schicht und sind nicht Bestandteil der stabilen Objektidentität.

## Auflösung

Der Zugriff erfolgt logisch:

```text
ObjectID
   ↓
Location Resolution
   ↓
Volume
   ↓
Device
   ↓
Physical Location
```

Höhere Schichten arbeiten bevorzugt mit `ObjectID` oder autorisierten Handles.

## Migration

Ein Objekt kann seinen Speicherort ändern:

```text
Device A / Volume A
        ↓
     Migration
        ↓
Device B / Volume B
```

Dabei bleibt:

```text
ObjectID A → ObjectID A
```

Die Migration darf bestehende Relations, semantische Referenzen und Projections nicht allein aufgrund der Standortänderung ungültig machen.

## Mehrere Speicherorte

Storage-Verfahren dürfen mehrere physische Repräsentationen desselben logischen Objekts verwenden, beispielsweise für:

- Redundanz
- Cache
- Replikation
- Snapshot
- Recovery

Die Existenz mehrerer Speicherorte erzeugt nicht automatisch mehrere logische Objekte.

## Location Policy

NovaOS darf den Speicherort anhand von Policies auswählen oder verändern.

Berücksichtigt werden können:

```text
Performance
Capacity
Reliability
Energy
Sovereignty
Security
Availability
```

Explizite Benutzerentscheidungen und harte Sicherheitsanforderungen haben Vorrang vor automatischer Optimierung.

## Sicherheit

Location Transparency bedeutet nicht Authority Transparency.

```text
Transparent Location
≠
Transparent Authority
```

Eine Migration auf ein anderes Device oder Volume muss weiterhin geltende Capability-, Security- und Sovereignty-Regeln erfüllen.

## Introspection

Autorisierte Komponenten sollen feststellen können:

```text
ObjectID
VolumeID
DeviceID
Location State
Migration State
```

Physische Details dürfen nur bei entsprechender Authority offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Objektidentität und Speicherort trennen.
2. Eine Standortänderung DARF die `ObjectID` nicht automatisch verändern.
3. Anwendungen SOLLEN nicht von physischen Speicherorten abhängig sein.
4. Location Resolution MUSS über stabile Identitäten möglich sein.
5. Migration MUSS Relations und Projections erhalten können.
6. Mehrere physische Repräsentationen DÜRFEN dasselbe logische Objekt repräsentieren.
7. Location Policies MÜSSEN Sicherheits- und Sovereignty-Anforderungen berücksichtigen.
8. Location Transparency DARF KEINE zusätzliche Authority erzeugen.
9. Standortänderungen SOLLEN transaktional durchgeführt werden können.
10. Location State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-DEVICE-0001`
- `NPSPEC-STORAGE-VOLUME-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`

## Ergebnis

NovaOS kann Speicherobjekte unabhängig von ihrem konkreten physischen Speicherort adressieren. Dadurch können Daten zwischen Devices und Volumes migriert oder durch mehrere physische Repräsentationen abgesichert werden, ohne ihre logische Identität oder bestehende Beziehungen zu verlieren.