# NPSPEC-FSSTORAGE-VOLUME-0001 – Nova Filesystem Storage Volume Integration

## Status

Angenommen

## Kategorie

Storage / Volume

## Zweck

NovaOS definiert Volumes als logische Speicherressourcen zwischen Storage Device und Filesystem/Namespace.

```text
Device
  ↓
Volume
  ↓
Filesystem
  ↓
Namespace
```

Dabei gilt:

```text
DeviceID ≠ VolumeID ≠ Volume Name ≠ Mountpoint
```

## Grundprinzipien

- Jedes Volume besitzt eine stabile `VolumeID`.
- Volume-Namen dienen der benutzerfreundlichen Darstellung.
- NovaOS verwendet keine Laufwerksbuchstaben.
- Ein Volume ist nicht an einen festen Namespace-Pfad gebunden.
- Ein Device kann mehrere Volumes enthalten.
- Ein Volume kann bei geeigneten Storage-Verfahren mehrere Devices umfassen.
- Mounten verändert weder `VolumeID` noch Objektidentitäten.

## Volume-Modell

```text
Volume
├── VolumeID
├── Name
├── State
├── Filesystem
├── Capacity
├── Mountpoint
└── DeviceSet
```

Optional können unter anderem Verschlüsselungs-, Health-, Performance- und Provenance-Informationen zugeordnet werden.

## Namespace-Integration

Volumes werden in den gemeinsamen NovaOS-Namespace eingebunden.

Beispiel:

```text
/
├── System/
├── Benutzer/
├── Apps/
└── Volumes/
    ├── Daten/
    └── Backup/
```

`/Volumes/Daten` ist dabei ein Mountpoint und nicht die Identität des Volumes.

Ein Volume kann umbenannt oder an anderer Stelle eingebunden werden, ohne seine `VolumeID` zu verändern.

## Volume-Namen

Volume-Namen sind menschenlesbare Bezeichnungen.

```text
VolumeID: 8f31...
Name: Daten
```

Namen müssen nicht global eindeutig sein. Für dauerhafte interne Referenzen muss die `VolumeID` verwendet werden.

## Zustände

Ein Volume kann mindestens folgende Zustände besitzen:

```text
Available
Mounted
ReadOnly
Degraded
Locked
Unavailable
Failed
Unknown
```

`Unknown` darf nicht als verfügbar oder fehlerfrei interpretiert werden.

## Verschlüsselung

Ein verschlüsseltes Volume kann vor der Freigabe den Zustand `Locked` besitzen.

```text
Locked
  ↓
Authorized Unlock
  ↓
Available
  ↓
Mounted
```

Entsperren und Mounten bleiben getrennte Operationen.

## Entfernen

Vor dem kontrollierten Entfernen eines Volumes müssen ausstehende Schreiboperationen abgeschlossen und neue Zugriffe blockiert werden.

```text
Mounted
  ↓
Drain / Flush
  ↓
Unmount
  ↓
Unavailable
```

## Capability-Sicherheit

Volume-Operationen benötigen entsprechende Authority.

Beispiele:

```text
Mount
Unmount
Read
Write
Format
Unlock
Rename
Manage
```

Filesystem-Zugriff gewährt nicht automatisch administrative Kontrolle über das Volume.

## Introspection

Autorisierte Komponenten sollen mindestens folgende Informationen abfragen können:

```text
VolumeID
Name
State
Filesystem
Capacity
Mountpoint
DeviceSet
```

## Normative Anforderungen

1. Jedes Volume MUSS eine stabile `VolumeID` besitzen.
2. `VolumeID`, Name, Device und Mountpoint MÜSSEN getrennte Konzepte sein.
3. NovaOS DARF keine Laufwerksbuchstaben als primäre Volume-Identität verwenden.
4. Volume-Namen DÜRFEN geändert werden, ohne die `VolumeID` zu verändern.
5. Mountpoint-Änderungen DÜRFEN die `VolumeID` nicht verändern.
6. Ein Device MUSS mehrere Volumes bereitstellen können.
7. Multi-Device-Volumes MÜSSEN architektonisch möglich sein.
8. Volume-Zustände MÜSSEN explizit darstellbar sein.
9. `Unknown` DARF NICHT als verfügbar interpretiert werden.
10. Verschlüsseltes Entsperren und Mounten MÜSSEN getrennte Operationen bleiben.
11. Volume-Operationen MÜSSEN capability-basiert autorisierbar sein.
12. Dauerhafte interne Referenzen SOLLEN die `VolumeID` statt Name oder Mountpoint verwenden.

## Abhängigkeiten

- `NPSPEC-STORAGE-DEVICE-0001`
- `NPSPEC-FSSTORAGE-VOLUME-0001` ersetzt bzw. konkretisiert `ADR-STORAGE-0014`
- `NPSPEC-STORAGE-ENCRYPTION-0001`
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`

## Ergebnis

NovaOS behandelt Volumes als stabile logische Speicherressourcen mit eigener `VolumeID`. Namen und Mountpoints bleiben veränderbare Darstellungen, während die Volume-Identität unabhängig von Device-Pfaden und Namespace-Strukturen erhalten bleibt.
