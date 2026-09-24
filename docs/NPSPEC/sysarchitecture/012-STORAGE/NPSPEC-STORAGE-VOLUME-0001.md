# NPSPEC-STORAGE-VOLUME-0001 – Nova Storage Volume

## Status

Entwurf

## Kategorie

Storage / Volume / Namespace / VFS

## Zweck

NovaOS definiert Volumes als logisch adressierbare Storage-Einheiten zwischen physischem Speicher, Dateisystem und Namespace.

```text
Storage Device
     ↓
Partition / Region
     ↓
Volume
     ↓
Filesystem
     ↓
VFS / Namespace
```

Volumes besitzen eine stabile Identität und sind nicht an klassische Laufwerksbuchstaben gebunden.

## Grundprinzip

```text
VolumeID ≠ Volume Name
Volume ≠ Physical Device
Volume ≠ Filesystem
```

Ein physisches Gerät kann mehrere Volumes enthalten. Ein Volume kann unabhängig von seinem sichtbaren Namen identifiziert werden.

## Volume-Modell

Ein Volume besitzt mindestens:

```text
VolumeID
Name
State
Capacity
Provider
```

Optional:

```text
Filesystem
Encryption State
Mount State
Health State
Metadata
```

Die `VolumeID` muss stabil und eindeutig sein.

## Benennung

NovaOS verwendet benannte Volumes.

```text
System:/
Benutzer:/
Boot:/
Apps:/
Daten:/
```

Der Name dient der Navigation und Darstellung.

```text
Rename Volume
     ↓
Same VolumeID
```

Eine Umbenennung darf die Identität des Volumes nicht verändern.

## Storage-Zuordnung

Ein Volume darf auf unterschiedlichen Storage-Strukturen basieren:

```text
Partition
Whole Device
Logical Region
Virtual Device
Encrypted Container
Network-backed Storage
```

Die konkrete physische Implementierung soll gegenüber höheren Schichten abstrahiert werden.

## Zustände

Volumes müssen definierte Zustände besitzen können.

```text
Offline
Available
Mounted
ReadOnly
Degraded
Failed
```

Weitere providerspezifische Zustände dürfen ergänzt werden.

`Unknown` darf nicht automatisch als `Healthy` oder `Available` behandelt werden.

## Mount

Ein verfügbares Volume darf über das VFS in einen Namespace eingebunden werden.

```text
VolumeID
   ↓
Filesystem
   ↓
VFS Mount
   ↓
Namespace
```

Mount-Position und Volume-Identität bleiben getrennt.

Ein Volume darf abhängig von Policy auch mehrere kontrollierte Namespace-Sichten besitzen.

## System-Volumes

NovaOS darf spezielle Rollen definieren:

```text
System
Benutzer
Boot
Recovery
Apps
Data
```

Eine Rolle ist nicht zwingend mit dem sichtbaren Volume-Namen identisch.

Dadurch kann beispielsweise ein Ersatz- oder Recovery-Volume dieselbe logische Rolle übernehmen.

## Verschlüsselung

Volumes dürfen vollständig verschlüsselt sein.

```text
Volume
   ↓
Encryption Layer
   ↓
Physical Storage
```

Ein verschlüsseltes Volume darf erst nach erfolgreicher Autorisierung und Schlüsselbereitstellung regulär eingebunden werden.

## Hotplug

Volumes auf entfernbaren oder dynamischen Geräten müssen kontrolliert erscheinen und verschwinden können.

```text
Detected
   ↓
Validate
   ↓
Available
   ↓
Mount
```

Ein unerwartetes Entfernen muss als Storage-Fehler behandelt werden und darf keine falsche erfolgreiche Persistenz signalisieren.

## Health

NovaOS soll den Zustand eines Volumes beobachten können.

Beispiele:

```text
Available Capacity
I/O Errors
Integrity Errors
ReadOnly State
Degraded State
Device Availability
```

Health-Informationen des zugrunde liegenden Geräts dürfen in den Volume-Zustand einfließen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
VolumeID
Name
Role
Capacity
Free Space
Filesystem
Mount State
Encryption State
Health State
Backing Storage
```

Sicherheitsrelevante Informationen müssen entsprechend dem Security Context gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS Volumes mit stabiler `VolumeID` unterstützen.
2. `VolumeID` und sichtbarer Volume-Name MÜSSEN getrennt sein.
3. NovaOS MUSS benannte Volumes ohne Laufwerksbuchstaben unterstützen.
4. Eine Umbenennung DARF die `VolumeID` NICHT verändern.
5. Volume, physisches Gerät und Dateisystem MÜSSEN logisch getrennte Konzepte bleiben.
6. Ein physisches Gerät DARF mehrere Volumes enthalten.
7. Volumes MÜSSEN definierte Lifecycle- und Health-Zustände besitzen können.
8. `Unknown` DARF NICHT automatisch als `Healthy` oder `Available` behandelt werden.
9. Mount-Position und Volume-Identität MÜSSEN getrennt sein.
10. Verschlüsselte Volumes MÜSSEN kontrolliert entsperrt und eingebunden werden.
11. Hotplug und unerwartetes Entfernen MÜSSEN kontrolliert behandelt werden.
12. Identität, Zustand, Kapazität und Mount-Status SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-ENCRYPTION-0001`
- `NPSPEC-STORAGE-CHECKSUM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-STORAGE-0014`

## Ergebnis

```text
Physical / Virtual Storage
          ↓
        Volume
          ↓
       VolumeID
          ↓
   Filesystem / VFS
          ↓
      Namespace
```

NovaOS erhält damit ein einheitliches Volume-Modell mit stabilen Identitäten, benannten Volumes und klarer Trennung zwischen physischem Speicher, Dateisystem und sichtbarem Namespace.