# NPSPEC-USERSPACE-DATA-0001 – Nova Userspace Data

## Status

Angenommen

## Kategorie

Userspace / Data

## Zweck

NovaOS behandelt Benutzerdaten als logische Datenmenge unabhängig von einem einzelnen Ordner, Volume oder physischen Speicherort.

`Daten` ist daher primär eine semantische Sicht auf benutzerrelevante Inhalte und kein zwingend physisches Verzeichnis oder eine eigene Partition.

## Grundprinzipien

```text
Data ≠ Directory
Data ≠ Volume
Data ≠ Physical Location
Data View ≠ Copy
```

Benutzerdaten können über mehrere Volumes und Storage Devices verteilt sein und trotzdem als zusammengehörige Datenmenge erscheinen.

## Datenmodell

Benutzerdaten basieren auf vorhandenen Filesystem-Objekten:

```text
ObjectID
├── Semantic Type
├── Metadata
├── Relations
└── Location
```

Die Zugehörigkeit zu einer Datenansicht wird aus diesen Eigenschaften abgeleitet und nicht ausschließlich aus einem festen Pfad.

## Datenansicht

NovaOS kann eine logische Ansicht wie:

```text
Daten/
├── Dokumente/
├── Bilder/
├── Musik/
├── Videos/
└── Projekte/
```

bereitstellen.

Diese Struktur darf vollständig oder teilweise durch Projections erzeugt werden.

Ein Objekt kann dadurch beispielsweise gleichzeitig unter:

```text
Daten/Bilder/
Daten/2026/
Daten/Projekt-Nova/
```

erscheinen, ohne mehrfach gespeichert zu werden.

## Verteilung

Die zugrunde liegenden Objekte können sich auf unterschiedlichen Volumes befinden:

```text
Volume System ─┐
Volume Daten ──┼─→ Datenansicht
Remote Storage ┘
```

Storage Location Transparency sorgt dafür, dass die logische Organisation nicht vom physischen Speicherort abhängig ist.

## Benutzerorganisation

Benutzer dürfen weiterhin klassische Ordner und eigene Strukturen verwenden.

Semantische Organisation ergänzt die klassische Verzeichnisstruktur, ersetzt sie aber nicht zwingend.

```text
Manuelle Ordner
+
Semantische Projections
```

## Programme und Solutions

Programme und Solutions sollen Benutzerdaten bevorzugt über:

```text
ObjectID
Semantic Query
Authorized Handle
```

adressieren können.

Feste Annahmen über physische Datenpfade sollen vermieden werden.

## Sicherheit

Eine Datenansicht gewährt keine zusätzliche Authority.

```text
Data Projection
      ↓
ObjectID
      ↓
Permission Check
      ↓
Authorized Handle
```

Ein Objekt darf nur erscheinen beziehungsweise geöffnet werden, soweit der jeweilige Kontext dazu berechtigt ist.

## Normative Anforderungen

1. NovaOS MUSS Benutzerdaten unabhängig von einem einzelnen physischen Datenordner verwalten können.
2. `Daten` DARF als semantische Projection bereitgestellt werden.
3. Benutzerdaten MÜSSEN über mehrere Volumes verteilt sein können.
4. Mehrere Datenansichten DÜRFEN dasselbe Objekt darstellen.
5. Projections DÜRFEN dabei keine unnötigen Kopien erzeugen.
6. Klassische benutzerdefinierte Ordner MÜSSEN weiterhin unterstützt werden.
7. Programme SOLLEN nicht von festen physischen Datenpfaden abhängig sein.
8. Storage-Migration DARF die logische Datenzugehörigkeit nicht automatisch verändern.
9. Datenansichten DÜRFEN keine zusätzliche Authority erzeugen.
10. Semantische Datenorganisation MUSS mit ObjectID, Metadata und Relations integrierbar sein.

## Abhängigkeiten

- `NPSPEC-USERSPACE-LAYOUT-0001`
- `NPSPEC-USERSPACE-FILES-0001`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-RELATION-0001`
- `NPSPEC-STORAGE-LOCATIONTRANSPARENCY-0001`

## Ergebnis

NovaOS trennt die logische Organisation von Benutzerdaten von deren physischer Speicherung. Dadurch können Inhalte über verschiedene Volumes verteilt und gleichzeitig über klassische Ordner sowie semantische Datenansichten konsistent dargestellt werden.