# NPSPEC-OBJECT-ZEROCOPY-0001 – Nova Object Zero-Copy

## Status

Angenommen

## Kategorie

Object / Zero-Copy / Data Movement

## Zweck

NovaOS definiert ein objektbezogenes Zero-Copy-Modell, bei dem Payload-Daten zwischen Komponenten verarbeitet werden können, ohne unnötige Speicher-Kopien zu erzeugen.

```text
Object
  ↓
Shared / Mapped Payload
  ↓
Consumer
```

Objektidentität, Speicherzugriff und Autorität bleiben dabei strikt getrennt.

## Grundprinzipien

```text
Zero-Copy ≠ No Data Movement
Object Access ≠ Memory Access
Memory Mapping ≠ Object Authority
Shared Buffer ≠ Shared Capability
ObjectID ≠ Memory Address
Buffer Lifetime ≠ Object Lifetime
Zero-Copy ≠ Mandatory
Performance ≠ Security Override
```

## Zero-Copy-Modell

Ein Zero-Copy-Zugriff beschreibt mindestens:

```text
ObjectZeroCopyView
├── ObjectID
├── VersionID
├── Buffer Reference
├── Access Mode
└── Lifetime
```

Optional:

```text
Offset
Length
SemanticTypeID
Representation
Memory Domain
Device Domain
Coherency Policy
Synchronization
Capability Reference
```

## Object View

Zero-Copy stellt eine kontrollierte Sicht auf den Payload eines Objekts bereit.

```text
Object
  ↓
Payload
  ↓
Zero-Copy View
  ↓
Consumer
```

Der View ist nicht selbst die Objektidentität.

```text
View ≠ Object
```

Mehrere Views können dasselbe Objekt oder dieselbe Version referenzieren.

## Zugriff

Zugriffsmodi können mindestens sein:

```text
ReadOnly
ReadWrite
ExclusiveWrite
```

Ein schreibbarer View darf nur erzeugt werden, wenn die entsprechende Capability dies erlaubt.

```text
Read Capability
      ↓
ReadOnly View
```

Eine Read Capability darf nicht durch Memory Mapping zu Write Authority erweitert werden.

## Versionierung

Zero-Copy muss mit Object Versioning kompatibel sein.

Ein View kann an eine konkrete Version gebunden werden:

```text
ObjectID + VersionID
        ↓
Zero-Copy View
```

Bei Copy-on-Write kann eine Änderung eine neue Version erzeugen.

```text
Version A
   ↓ Write
Copy-on-Write
   ↓
Version B
```

Bestehende Views auf Version A bleiben dadurch eindeutig.

## Pipeline Integration

Object Pipelines können Payloads direkt zwischen Stages weitergeben.

```text
Stage A
   ↓
Shared Object Buffer
   ↓
Stage B
   ↓
Stage C
```

Dadurch können unnötige Kopien zwischen Pipeline-Stages vermieden werden.

## Shared Memory

Zero-Copy kann Shared Memory verwenden.

```text
Producer Address Space
        ↕
Shared Pages
        ↕
Consumer Address Space
```

Dabei gilt:

```text
Shared Memory ≠ Shared Security Context
```

Jeder Teilnehmer benötigt eigene gültige Autorität.

## Scatter/Gather

Nicht zusammenhängende Datenbereiche können als logischer Object View dargestellt werden.

```text
Object View
├── Segment A
├── Segment B
└── Segment C
```

Dadurch kann eine physische Zusammenführung der Daten vermieden werden.

## DMA und Geräte

Zero-Copy kann direkt mit Geräten und DMA integriert werden.

```text
Object Buffer
     ↓
DMA Mapping
     ↓
Device
```

DMA benötigt separate:

```text
DMA Capability
IOMMU Mapping
Memory Permission
Device Capability
```

Eine Object Capability allein darf keinen DMA-Zugriff erzeugen.

## Synchronisation

Gemeinsam verwendete Buffer benötigen explizite Synchronisation.

Mögliche Mechanismen:

```text
Ownership Transfer
Read Lease
Write Lease
Fence
Completion Event
Atomic State
```

Daten dürfen erst weiterverwendet werden, wenn der notwendige Synchronisationszustand erreicht ist.

## Lebensdauer

Buffer Lifetime und Object Lifetime müssen getrennt behandelt werden.

```text
Acquire View
    ↓
Use
    ↓
Release View
```

Ein Buffer darf nicht freigegeben oder wiederverwendet werden, solange gültige Views darauf existieren.

## Remote Execution

Über Rechnergrenzen hinweg ist echtes Shared-Memory-Zero-Copy nicht immer möglich.

NovaOS darf dennoch dasselbe Objektmodell verwenden:

```text
Object Reference
      ↓
Transport
      ↓
Remote Representation
```

Die Transportebene entscheidet über die technisch mögliche Datenbewegung.

```text
Semantic Zero-Copy Intent ≠ Guaranteed Physical Zero-Copy
```

## Copy Fallback

Wenn Zero-Copy nicht sicher oder technisch möglich ist:

```text
Zero-Copy Attempt
      ↓
Unavailable / Unsafe
      ↓
Controlled Copy
```

Der Copy-Fallback muss dieselbe semantische Objektidentität beziehungsweise definierte Versionierungsregeln respektieren.

## Sicherheit

Zero-Copy darf keine Sicherheitsgrenzen umgehen.

Insbesondere dürfen nicht unbeabsichtigt sichtbar werden:

```text
Adjacent Memory
Kernel Memory
Uninitialized Data
Other Objects
Previous Buffer Contents
Protected Metadata
```

Buffer müssen vor Wiederverwendung gegebenenfalls bereinigt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
VersionID
View Type
Access Mode
Buffer Size
Memory Domain
Lifetime
Mapping State
Synchronization State
Zero-Copy / Copy Fallback
```

Physische Adressen dürfen nur für entsprechend autorisierte Systemkomponenten sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS Objektidentität und Speicherabbildung getrennt behandeln.
2. Zero-Copy DARF keine zusätzliche Objekt- oder Speicherautorität erzeugen.
3. Zero-Copy Views MÜSSEN an definierte Zugriffsrechte gebunden sein.
4. Schreibzugriffe MÜSSEN mit Object Versioning kompatibel sein.
5. Shared Memory DARF NICHT automatisch Capability Sharing bedeuten.
6. Buffer Lifetime MUSS unabhängig vom Object Lifetime verwaltet werden können.
7. DMA-Zugriffe MÜSSEN separat autorisiert und isoliert werden.
8. Zero-Copy SOLL Scatter/Gather und Object Pipelines unterstützen.
9. Ein sicherer Copy-Fallback MUSS verfügbar sein, wenn Zero-Copy nicht möglich ist.
10. Zero-Copy-Zustand MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-PERMISSION-0001`
- `NPSPEC-OBJECT-PIPELINE-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `ADR-ARCH-0029`

## Ergebnis

```text
Object Payload
      ↓
Capability-controlled View
      ↓
Shared / Mapped Buffer
      ↓
Pipeline / IPC / Device
      ↓
Minimal Data Copies
```

NovaOS erhält damit ein objektbezogenes Zero-Copy-Modell, das hohe Datenübertragungsleistung ermöglicht, ohne Objektidentität, Speicherzugriff und Capability-basierte Autorität miteinander zu vermischen.