# NPSPEC-NAMESPACE-USER-0001 – Nova User Namespace

## Status

Angenommen

## Kategorie

Namespace / User

## Zweck

NovaOS definiert für jeden Benutzer eine eigene logische Namespace-Sicht.

Der User Namespace verbindet globale Systemressourcen mit benutzerspezifischen Daten, Einstellungen, Anwendungen und Projektionen, ohne Objektidentität oder Authority an Pfade zu koppeln.

## Grundprinzipien

```text
User Namespace ≠ Global Namespace
User Identity ≠ Namespace Identity
Visibility ≠ Authority
Path ≠ ObjectID
User Ownership ≠ Unrestricted Authority
Projection ≠ Copy
```

## Modell

```text
UserNamespace
├── NamespaceID
├── UserID
├── BaseNamespace
├── UserRoot
├── Projections
├── Overlays
├── Policy
└── Version
```

Der Namespace ist dem Benutzerkontext zugeordnet, besitzt jedoch eine eigene `NamespaceID`.

## Effektive Sicht

```text
Global Namespace
      +
User Namespace
      +
User Projections
      ↓
Effective User Namespace
```

Die Sicht kann insbesondere enthalten:

```text
/System
/Benutzer
/Apps
/Solutions
/Boot
/Volumes
Daten
```

Welche Bereiche sichtbar sind, wird durch Namespace Policy bestimmt.

## Benutzerbereich

Der persönliche Bereich eines Benutzers wird innerhalb des User Namespace eingebunden.

```text
/Benutzer/<User>/
```

Er kann unter anderem enthalten:

```text
Files
Settings
Application Data
Solution Data
Workspace Data
Media
```

Die konkrete physische Speicherung bleibt vom Namespace unabhängig.

## Daten-Projektion

`Daten` kann als semantische Projektion benutzerspezifischer Daten bereitgestellt werden:

```text
Local Objects
     +
Other Volumes
     +
Authorized Remote Objects
     ↓
Daten
```

Diese Projektion benötigt keine gemeinsame physische Verzeichnisstruktur.

## Programme und Solutions

Application-, Solution- und Process-Namespaces können auf dem User Namespace aufbauen:

```text
User Namespace
      ↓
Application / Solution Context
      ↓
Process Namespace
```

Nachgelagerte Kontexte dürfen die Benutzersicht weiter einschränken oder durch autorisierte Projektionen ergänzen.

## Mehrbenutzersystem

Jeder Benutzer kann eine unterschiedliche Sicht besitzen:

```text
User A → Namespace A
User B → Namespace B
User C → Namespace C
```

Benutzerspezifische Daten und Projektionen anderer Benutzer werden dadurch nicht automatisch sichtbar oder zugänglich.

## Auflösung

```text
Path
 ↓
User Namespace
 ↓
Projection / Overlay Resolution
 ↓
ObjectID
 ↓
Capability / Permission Check
 ↓
Authorized Handle
```

Die Namespace-Auflösung erzeugt keine Authority.

## Session und Lifecycle

Der User Namespace ist nicht an einen einzelnen Prozess gebunden.

Er kann über mehrere Prozesse und Sessions desselben Benutzers verwendet werden.

Sessiongebundene Ressourcen müssen davon getrennt behandelt werden.

## Sicherheit

Der User Namespace darf keine bestehenden Sicherheitsgrenzen umgehen.

Insbesondere gilt:

```text
User Context ≠ Full User Authority
Visible ≠ Accessible
Mounted ≠ Authorized
Owned ≠ Unrestricted
```

Zugriffe bleiben an Capability-, Permission- und Policy-Prüfungen gebunden.

## Normative Anforderungen

1. NovaOS MUSS benutzerspezifische Namespace-Kontexte unterstützen.
2. `UserID` und `NamespaceID` MÜSSEN getrennte Identitäten bleiben.
3. Benutzer DÜRFEN unterschiedliche Namespace-Sichten besitzen.
4. Der User Namespace MUSS globale und benutzerspezifische Ressourcen kombinieren können.
5. Benutzerdaten MÜSSEN unabhängig von ihrem physischen Speicherort projizierbar sein.
6. `Daten` DARF als semantische benutzerspezifische Projektion bereitgestellt werden.
7. Application-, Solution- und Process-Namespaces MÜSSEN auf dem User Namespace aufbauen können.
8. Nachgelagerte Namespace-Kontexte DÜRFEN die Benutzersicht einschränken.
9. Namespace-Sichtbarkeit DARF keine Authority erzeugen.
10. Benutzeridentität DARF keine universelle Authority auf alle sichtbaren Ressourcen erzeugen.
11. Sessiongebundene und benutzergebundene Namespace-Zustände MÜSSEN unterscheidbar sein.
12. User Namespace, Projektionen und effektive Auflösung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-NAMESPACE-APPLICATION-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-POLICY-NAMESPACE-0001`
- `NPSPEC-POLICY-USERDATA-0001`
- `NPSPEC-USERSPACE-DATA-0001`
- `NPSPEC-USERSPACE-PERMISSION-0001`

## Ergebnis

NovaOS stellt jedem Benutzer eine eigene logische Namespace-Sicht bereit. Globale Ressourcen, persönliche Daten und semantische Projektionen können darin konsistent zusammengeführt werden, während physischer Speicherort, Objektidentität und tatsächliche Authority unabhängig von der sichtbaren Namespace-Struktur bleiben.