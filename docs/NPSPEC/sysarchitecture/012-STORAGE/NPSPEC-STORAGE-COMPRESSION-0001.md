# NPSPEC-STORAGE-COMPRESSION-0001 – Nova Storage Compression

## Status

Entwurf

## Kategorie

Storage / Compression / Space Efficiency / Data Transformation

## Zweck

NovaOS definiert transparente Storage-Kompression zur Reduzierung des physischen Speicherbedarfs, ohne die logische Darstellung eines Storage-Objekts zu verändern.

```text
Logical Data
     ↓
Compression
     ↓
Stored Representation
     ↓
Decompression
     ↓
Logical Data
```

Kompression ist eine optionale Storage-Eigenschaft und darf nicht Bestandteil der logischen Objektidentität sein.

## Grundprinzip

```text
Logical Content ≠ Stored Representation
Compression ≠ Content Identity
Compression ≠ Encryption
```

Dasselbe Objekt darf komprimiert, unkomprimiert oder mit einem anderen Algorithmus gespeichert werden, ohne dadurch seine `ObjectID` oder `ContentID` zu verändern.

## Kompressionsmodell

Kompression darf auf unterschiedlichen Ebenen erfolgen:

```text
Block
Extent
Payload
Object
```

Die konkrete Ebene wird vom Storage-Provider bestimmt.

## Algorithmen

NovaOS muss unterschiedliche Kompressionsalgorithmen unterstützen können.

```text
Compression Algorithm
Compression Level
Format Version
```

Algorithmen müssen versioniert und austauschbar sein.

Die Auswahl darf anhand von:

```text
Data Type
Compression Ratio
CPU Cost
Latency
Energy Cost
Storage Pressure
```

erfolgen.

## Adaptive Kompression

NovaOS darf automatisch entscheiden, ob sich Kompression für bestimmte Daten lohnt.

```text
Input Data
    ↓
Estimate Compressibility
    ↓
Compress / Store Directly
```

Bereits stark komprimierte oder schlecht komprimierbare Daten sollen nicht unnötig erneut komprimiert werden.

## Content Addressing

Die `ContentID` muss sich auf den logischen Inhalt beziehen.

```text
Logical Payload
      ↓
ContentID
      ↓
Compression
      ↓
Physical Representation
```

Eine Änderung des Kompressionsalgorithmus darf daher keine neue ContentID erzwingen.

## Deduplication

Deduplication soll vor der physischen Storage-Repräsentation möglich bleiben.

```text
Logical Content
      ↓
ContentID / Dedup
      ↓
Compression
      ↓
Storage
```

Identische Inhalte dürfen unabhängig von ihrer ursprünglichen Darstellung gemeinsam gespeichert werden.

## Checksums

Checksums dürfen sowohl logische als auch physische Daten schützen.

```text
Logical Checksum
Physical Checksum
```

Dadurch können sowohl Inhaltsfehler als auch Beschädigungen der komprimierten Darstellung erkannt werden.

## Random Access

Große Daten sollen nicht zwingend vollständig dekomprimiert werden müssen.

NovaOS darf Daten deshalb in unabhängig dekomprimierbare Einheiten aufteilen.

```text
Object
├── Chunk A
├── Chunk B
├── Chunk C
└── Chunk D
```

Dies ermöglicht gezielten Zugriff auf einzelne Bereiche.

## Transaktionen

Änderungen an komprimierten Daten müssen die Storage-Transaction-Regeln einhalten.

Eine fehlgeschlagene Kompression darf keinen teilweise aktualisierten Objektzustand erzeugen.

## Performance

Kompression darf den Storage-Pfad nicht unnötig verlangsamen.

NovaOS soll Hardwarebeschleunigung verwenden können, sofern verfügbar.

Für latenzkritische oder Realtime-Daten muss Kompression deaktivierbar oder durch deterministische Verfahren begrenzbar sein.

## Recompression

NovaOS darf vorhandene Daten im Hintergrund neu komprimieren.

```text
Old Representation
       ↓
Recompress
       ↓
Verify
       ↓
Replace Representation
```

Dabei dürfen `ObjectID`, `VersionID` und `ContentID` unverändert bleiben, solange sich der logische Inhalt nicht ändert.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Compression State
Algorithm
Compression Level
Logical Size
Physical Size
Compression Ratio
Recompression State
```

## Normative Anforderungen

1. NovaOS MUSS optionale transparente Storage-Kompression unterstützen können.
2. Kompression DARF die logische `ObjectID` NICHT verändern.
3. Kompression DARF die `ContentID` NICHT verändern, solange der logische Inhalt identisch bleibt.
4. Kompressionsalgorithmen MÜSSEN versionierbar und austauschbar sein.
5. NovaOS SOLL schlecht komprimierbare Daten unkomprimiert speichern können.
6. Kompression SOLL mit Content Addressing und Deduplication kombinierbar sein.
7. Große Objekte SOLLEN unabhängig dekomprimierbare Bereiche unterstützen können.
8. Kompressionsänderungen MÜSSEN konsistent und crash-sicher erfolgen.
9. Recompression DARF keine neue Objektversion erzwingen, solange der logische Inhalt unverändert bleibt.
10. Realtime- und latenzkritische Daten MÜSSEN Kompression kontrollieren oder deaktivieren können.
11. NovaOS SOLL Hardwarebeschleunigung für Kompression verwenden können.
12. Kompressionsstatus und Speicherersparnis SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-CHECKSUM-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-STORAGE-0011`

## Ergebnis

```text
Logical Object
      ↓
Content Identity
      ↓
Adaptive Compression
      ↓
Physical Storage
```

NovaOS erhält damit eine transparente und adaptive Storage-Kompression, bei der physische Speicheroptimierung klar von Objektidentität und logischem Dateninhalt getrennt bleibt.