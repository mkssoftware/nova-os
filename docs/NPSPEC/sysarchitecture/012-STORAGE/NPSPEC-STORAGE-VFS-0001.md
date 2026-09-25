# NPSPEC-STORAGE-VFS-0001 – Nova Virtual File System

## Status

Angenommen

## Kategorie

Storage / VFS / Filesystem Abstraction

## Zweck

NovaOS definiert das Virtual File System (VFS) als gemeinsame Abstraktionsschicht zwischen Systemkomponenten und unterschiedlichen Dateisystemen.

```text
Applications / Services
        ↓
       VFS
   ┌────┼────┐
   ↓    ↓    ↓
 NovaFS FAT  NTFS
```

Das VFS stellt ein einheitliches Objekt-, Datei- und Verzeichnismodell bereit, ohne höhere Schichten an ein konkretes Dateisystem zu binden.

## Grundprinzip

```text
Filesystem Interface ≠ Filesystem Implementation
Path ≠ Object Identity
Volume Name ≠ Drive Letter
```

NovaOS verwendet stabile Objektidentitäten. Pfade und Namen dienen primär der Navigation und Darstellung.

## VFS-Objekte

Das VFS muss mindestens folgende Objekttypen darstellen können:

```text
File
Directory
Volume
Stream
Device
Link
Mount
```

Dateisystemspezifische Objekte dürfen über zusätzliche Typen erweitert werden.

## Objektidentität

Ein VFS-Objekt soll eine stabile Identität besitzen.

```text
ObjectID
Type
Metadata
Filesystem
Volume
```

Umbenennen oder Verschieben darf die Objektidentität nicht verändern, sofern das zugrunde liegende Dateisystem dies unterstützt.

## Volumes

NovaOS verwendet benannte Volumes statt Laufwerksbuchstaben.

```text
System:/
Benutzer:/
Boot:/
Daten:/
```

Die sichtbare Bezeichnung ist von der internen Volume-ID getrennt.

```text
VolumeID ≠ Volume Name
```

## Mount-Modell

Dateisysteme werden in den VFS-Namespace eingebunden.

```text
Storage Device
      ↓
Filesystem Driver
      ↓
VFS Mount
      ↓
Namespace
```

Mounts müssen dynamisch erstellt und entfernt werden können.

## Operationen

Das VFS stellt mindestens folgende Operationen bereit:

```text
Open
Close
Read
Write
Create
Delete
Rename
Move
Enumerate
GetMetadata
SetMetadata
```

Asynchrone Varianten dürfen für I/O-intensive Operationen bereitgestellt werden.

## Metadaten

Das VFS muss gemeinsame Metadaten unabhängig vom konkreten Dateisystem darstellen können.

Beispiele:

```text
ObjectID
Type
Size
Timestamps
Permissions
Owner
Semantic Type
Attributes
```

Nicht unterstützte Metadaten dürfen über kontrollierte VFS-Projektionen oder Sidecar-Mechanismen bereitgestellt werden.

## Berechtigungen

Jeder Zugriff muss vor der eigentlichen Dateisystemoperation autorisiert werden.

```text
Request
   ↓
Capability / Security Check
   ↓
VFS
   ↓
Filesystem
```

Ein Dateisystemtreiber darf die übergeordnete NovaOS-Security-Policy nicht umgehen.

## Caching

Das VFS darf gemeinsame Caches für:

```text
Metadata
Directory Entries
File Data
Object Lookup
```

verwenden.

Caches müssen mit Änderungen des zugrunde liegenden Dateisystems konsistent gehalten werden.

## DataMove

Datei-I/O soll die NovaOS-DataMove-Architektur nutzen können.

```text
File
 ↓
VFS
 ↓
Shared Buffer / Scatter-Gather
 ↓
Storage Driver
```

Zero-Copy soll verwendet werden können, wenn Dateisystem, Storage Stack und Hardware dies unterstützen.

## Transaktionen

Das VFS soll transaktionale Storage-Operationen unterstützen können.

```text
Prepare
  ↓
Apply
  ↓
Commit
  ↓
Verify
```

Nicht transaktionale Dateisysteme dürfen über eingeschränkte Semantik eingebunden werden.

## Fehlerbehandlung

Das VFS muss Dateisystemfehler in ein gemeinsames Fehlermodell übersetzen.

Beispiele:

```text
NotFound
AccessDenied
AlreadyExists
ReadOnly
NoSpace
Corrupt
IOError
Unavailable
```

Dateisystemspezifische Diagnosedaten dürfen zusätzlich erhalten bleiben.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Mounted Volumes
Filesystem Type
Mount State
Open Objects
Cache Usage
I/O Statistics
Filesystem Errors
```

## Normative Anforderungen

1. NovaOS MUSS ein dateisystemunabhängiges VFS bereitstellen.
2. Anwendungen SOLLEN nicht direkt von einem konkreten Dateisystemtreiber abhängig sein.
3. Objektidentität MUSS logisch von Pfad und Anzeigename getrennt sein.
4. NovaOS MUSS benannte Volumes ohne Laufwerksbuchstaben unterstützen.
5. Das VFS MUSS unterschiedliche Dateisysteme gleichzeitig einbinden können.
6. Dateisystemzugriffe MÜSSEN die NovaOS-Security- und Capability-Regeln einhalten.
7. Gemeinsame Metadaten MÜSSEN über eine einheitliche VFS-Schnittstelle verfügbar sein.
8. Mount und Unmount MÜSSEN kontrollierte Lifecycle-Operationen sein.
9. Das VFS SOLL asynchrone und Zero-Copy-fähige I/O-Pfade unterstützen.
10. Dateisystemfehler MÜSSEN in ein gemeinsames VFS-Fehlermodell übersetzt werden.
11. Nicht unterstützte Dateisystemfunktionen DÜRFEN NICHT als erfolgreich ausgeführt dargestellt werden.
12. Mounts, Volumes und relevante I/O-Zustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-STORAGE-0001`

## Ergebnis

```text
Nova Applications
       ↓
      VFS
       ↓
Unified Object Model
       ↓
Filesystem Providers
       ↓
Storage Devices
```

NovaOS erhält damit eine kompakte, dateisystemunabhängige Storage-Abstraktion, auf der native Nova-Dateisysteme ebenso wie FAT, NTFS und weitere kompatible Dateisysteme eingebunden werden können.