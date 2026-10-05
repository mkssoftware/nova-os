# NPSPEC-STORAGE-MOUNT-0001 – Nova Storage Mount

## Status

Angenommen

## Kategorie

Storage / Mount / Namespace

## Zweck

NovaOS definiert Mounts als kontrollierte Zuordnung eines verfügbaren Volumes zu einem Ort im gemeinsamen Filesystem-Namespace.

```text
VolumeID
   ↓
Mount
   ↓
Namespace Location
```

Ein Mount verändert weder die Identität des Volumes noch die Identität seiner Objekte.

## Grundprinzipien

```text
Mountpoint ≠ VolumeID
Mountpoint ≠ ObjectID
Mount ≠ Volume
Unmount ≠ Delete
Visibility ≠ Authority
```

NovaOS verwendet keine Laufwerksbuchstaben und keine getrennten Dateisystemwurzeln.

## Mount-Modell

```text
Mount
├── MountID
├── VolumeID
├── NamespaceID
├── Mountpoint
├── Scope
├── Mode
└── State
```

Ein Volume kann abhängig von Policy und Filesystem mehrfach in unterschiedlichen Namespace-Kontexten sichtbar gemacht werden.

## Mountpoint

Beispiel:

```text
VolumeID: 8f31...
Name: Daten
Mountpoint: /Volumes/Daten
```

Der Mountpoint ist eine Namespace-Zuordnung und keine dauerhafte Identität.

Eine Änderung von:

```text
/Volumes/Daten
```

zu:

```text
/Volumes/Archiv
```

verändert weder `VolumeID` noch vorhandene `ObjectID`s.

## Mount Scopes

Mounts können unterschiedliche Gültigkeitsbereiche besitzen:

```text
Global
User
Process
Program
Solution
Workspace
Recovery
```

Dadurch können unterschiedliche Kontexte unterschiedliche Views auf dieselben Storage-Ressourcen erhalten.

## Mount Modes

Mindestens folgende Modi müssen möglich sein:

```text
ReadOnly
ReadWrite
```

Weitere Einschränkungen können durch Filesystem-, Capability- und Security-Policies bestimmt werden.

## Mount-Ablauf

```text
Volume Available
      ↓
Validate
      ↓
Authorize
      ↓
Create Mount
      ↓
Publish in Namespace
```

Die Namespace-Sichtbarkeit soll erst nach erfolgreicher Validierung veröffentlicht werden.

## Unmount

```text
Mounted
   ↓
Block New Operations
   ↓
Drain / Flush
   ↓
Remove Namespace Mapping
   ↓
Unmounted
```

Ein Unmount löscht weder das Volume noch dessen Daten.

Bei noch aktiven Handles kann die Policy einen normalen Unmount verweigern oder einen kontrollierten Deferred Unmount durchführen.

## Transaktionen

Mount- und Unmount-Operationen sollen mit Namespace-Transaktionen integrierbar sein.

Ein fehlgeschlagener Mount darf keinen unvollständigen Namespace-Zustand hinterlassen.

## Capability-Sicherheit

Mount-Operationen benötigen explizite Authority.

```text
Mount Capability
      ↓
VolumeID
      ↓
Namespace Scope
```

Der Besitz von Dateizugriffsrechten auf einem Volume gewährt nicht automatisch das Recht, dieses Volume zu mounten oder globale Mountpoints zu verändern.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
MountID
VolumeID
Mountpoint
Scope
Mode
State
```

## Normative Anforderungen

1. Jeder aktive Mount MUSS eine eindeutige `MountID` besitzen.
2. Mountpoint und `VolumeID` MÜSSEN getrennte Konzepte bleiben.
3. Mounten oder Unmounten DARF `ObjectID`s nicht verändern.
4. NovaOS MUSS Mounts in den gemeinsamen Namespace integrieren.
5. Mounts MÜSSEN unterschiedliche Scopes unterstützen können.
6. ReadOnly- und ReadWrite-Mounts MÜSSEN unterscheidbar sein.
7. Mount-Operationen MÜSSEN capability-basiert autorisiert werden.
8. Sichtbarkeit durch einen Mount DARF keine zusätzliche Authority erzeugen.
9. Unmount DARF keine Daten löschen.
10. Aktive I/O-Operationen MÜSSEN beim Unmount kontrolliert behandelt werden.
11. Mount-Änderungen SOLLEN transaktional in den Namespace integriert werden.
12. Mount-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-DEVICE-0001`
- `NPSPEC-STORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-LOCATION-0001`
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`

## Ergebnis

NovaOS bindet Volumes über explizite Mount-Objekte in den gemeinsamen Namespace ein. Volume-Identität, Mountpoint und Objektidentität bleiben dabei unabhängig, sodass Volumes flexibel eingebunden werden können, ohne stabile Referenzen oder Sicherheitsgrenzen zu verändern.