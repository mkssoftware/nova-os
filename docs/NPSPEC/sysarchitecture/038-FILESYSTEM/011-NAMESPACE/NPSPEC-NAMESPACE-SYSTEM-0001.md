# NPSPEC-NAMESPACE-SYSTEM-0001 – Nova System Namespace

## Status

Angenommen

## Kategorie

Namespace / System

## Zweck

NovaOS definiert den System Namespace als systemweite logische Basissicht auf Betriebssystemressourcen.

Er bildet die Grundlage für User-, Application-, Solution-, Workspace- und Process-Namespaces, ohne die physische Speicherstruktur oder Objektidentität festzulegen.

## Grundprinzipien

```text
System Namespace ≠ Physical Storage
System Namespace ≠ /System
Path ≠ ObjectID
Visibility ≠ Authority
Namespace ≠ Permission
Global View ≠ Unrestricted Access
```

## Modell

```text
SystemNamespace
├── NamespaceID
├── Root
├── Entries
├── Projections
├── Mounts
├── Policy
└── Version
```

Der System Namespace besitzt eine stabile `NamespaceID` und stellt die globale logische Ausgangsstruktur bereit.

## Globale Struktur

Die logische Basisstruktur von NovaOS ist:

```text
/
├── System/
├── Benutzer/
├── Apps/
├── Solutions/
├── Boot/
└── Volumes/
```

Die sichtbaren Namen können lokalisiert werden. Die internen Namespace-Identitäten bleiben sprachneutral und stabil.

## `/System`

`/System` ist ein geschützter Teil des System Namespace und enthält systemweite Betriebssystemkomponenten.

```text
/System/
├── Kernel/
├── Services/
├── Libraries/
├── Runtime/
├── Drivers/
├── Components/
├── Resources/
├── Configuration/
└── Security/
```

`/System` ist nicht mit dem gesamten System Namespace gleichzusetzen.

## Volumes

Volumes werden über definierte Mounts oder Projektionen in den System Namespace eingebunden.

```text
VolumeID
   ↓
Mount / Projection
   ↓
Namespace Entry
```

Volume-Name, Mountpoint und `VolumeID` bleiben voneinander getrennt.

## Abgeleitete Namespaces

Der System Namespace dient als Basis weiterer kontextabhängiger Sichten:

```text
System Namespace
       ↓
User Namespace
       ↓
Application / Solution / Workspace
       ↓
Process Namespace
```

Jede Ebene darf die Sicht einschränken oder autorisierte Projektionen und Overlays ergänzen.

## Auflösung

```text
Path
 ↓
System Namespace
 ↓
Mount / Projection
 ↓
ObjectID
 ↓
Capability / Permission Check
 ↓
Authorized Handle
```

Namespace-Auflösung und Autorisierung bleiben getrennt.

## Globale Änderungen

Änderungen am System Namespace können unter anderem betreffen:

```text
Mount
Unmount
Project
Remove Projection
Publish System Component
Change Namespace Mapping
```

Globale Änderungen müssen explizit autorisiert und nach Möglichkeit transaktional veröffentlicht werden.

## Schutz

Kritische Bereiche wie:

```text
/System
/Boot
Security Configuration
Recovery Resources
```

unterliegen zusätzlicher System Protection und System Write Policy.

Ein abgeleiteter Namespace oder Overlay darf diese Schutzgrenzen nicht umgehen.

## Dynamik

Der System Namespace darf zur Laufzeit erweitert oder verändert werden.

Änderungen erzeugen eine neue konsistente Namespace-Version.

Bereits ausgegebene Handles bleiben an die zuvor aufgelöste `ObjectID` gebunden.

## Normative Anforderungen

1. NovaOS MUSS einen globalen System Namespace bereitstellen.
2. Der System Namespace MUSS eine stabile `NamespaceID` besitzen.
3. Namespace-Struktur und physische Speicherstruktur MÜSSEN getrennt bleiben.
4. Pfade DÜRFEN nicht als dauerhafte Objektidentität verwendet werden.
5. Systemdefinierte Namespace-Identitäten MÜSSEN unabhängig von lokalisierten Anzeigenamen bleiben.
6. Volumes MÜSSEN über definierte Mounts oder Projektionen eingebunden werden können.
7. User-, Application-, Solution-, Workspace- und Process-Namespaces MÜSSEN auf dem System Namespace aufbauen können.
8. Namespace-Sichtbarkeit DARF keine Authority erzeugen.
9. Globale Namespace-Änderungen MÜSSEN explizit autorisiert sein.
10. Kritische Bereiche MÜSSEN System Protection und System Write Policy unterliegen.
11. Namespace-Änderungen SOLLEN transaktional und versioniert veröffentlicht werden.
12. Bestehende Handles DÜRFEN durch spätere Namespace-Änderungen nicht auf andere Objekte umgebunden werden.
13. System Namespace, Mounts, Projektionen und Auflösung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-NAMESPACE-USER-0001`
- `NPSPEC-NAMESPACE-APPLICATION-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-POLICY-NAMESPACE-0001`
- `NPSPEC-POLICY-SYSTEMWRITE-0001`
- `NPSPEC-SYSTEM-PROTECTION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

NovaOS besitzt einen stabilen globalen System Namespace als logische Basis aller weiteren Namespace-Sichten. Die sichtbare Systemstruktur bleibt dabei von physischem Speicher, Objektidentität und Authority getrennt und kann sicher, versioniert und kontextabhängig erweitert werden.