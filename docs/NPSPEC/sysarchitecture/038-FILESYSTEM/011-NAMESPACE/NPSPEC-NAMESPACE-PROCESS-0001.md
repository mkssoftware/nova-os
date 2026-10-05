# NPSPEC-NAMESPACE-PROCESS-0001 – Nova Process Namespace

## Status

Angenommen

## Kategorie

Namespace / Process

## Zweck

NovaOS definiert für Prozesse eine eigene effektive Namespace-Sicht.

Ein Prozess arbeitet nicht zwingend direkt mit dem globalen Namespace, sondern mit einer aus globalen Einträgen, Kontextregeln, Projektionen und Overlays erzeugten Prozesssicht.

## Grundprinzipien

```text
Process Namespace ≠ Global Namespace
Visibility ≠ Authority
Path ≠ ObjectID
ProcessID ≠ NamespaceID
Projection ≠ Copy
Namespace Isolation ≠ Object Isolation
```

## Prozess-Namespace

Jeder Prozess besitzt einen zugeordneten Namespace-Kontext:

```text
ProcessNamespace
├── NamespaceID
├── ProcessID
├── BaseNamespace
├── Scope
├── Projections
├── Overlays
├── Policy
└── Version
```

Mehrere Prozesse dürfen denselben Namespace-Kontext teilen, sofern dies explizit vorgesehen ist.

## Aufbau

Die effektive Sicht kann entstehen aus:

```text
Global Namespace
      +
User Context
      +
Program Namespace
      +
Solution / Workspace Context
      +
Process Overlays
      ↓
Effective Process Namespace
```

Nicht benötigte oder nicht zulässige Bereiche können ausgeblendet werden.

## Auflösung

```text
Process
   ↓
Path
   ↓
Process Namespace
   ↓
Projection / Overlay Resolution
   ↓
ObjectID
   ↓
Capability / Permission Check
   ↓
Authorized Handle
```

Die Namespace-Auflösung selbst erzeugt keine Zugriffsberechtigung.

## Prozessisolation

Unterschiedliche Prozesse dürfen unterschiedliche Sichten auf denselben globalen Namespace besitzen.

```text
Process A → Namespace View A
Process B → Namespace View B
Process C → Namespace View C
```

Dadurch können beispielsweise temporäre Ressourcen, private Abhängigkeiten oder kontextspezifische Projektionen isoliert sichtbar gemacht werden.

## Program-Kontext

Ein Prozess eines klassischen Programms kann dessen privaten Namespace erben.

Insbesondere gilt:

```text
/Apps/<Program>/SYS/
        ↓
Program SYS Overlay
        ↓
Effective Process /System
```

Das Overlay bleibt auf den zugehörigen Programm- beziehungsweise Prozesskontext begrenzt.

## Prozessspezifische Projektionen

Ein Prozess darf zusätzliche temporäre Projektionen besitzen, beispielsweise für:

```text
Temporary Resources
IPC Resources
Shared Objects
Workspace Resources
Virtual Resources
Device Projections
```

Diese Projektionen müssen einen definierten Scope und Lifecycle besitzen.

## Vererbung

Beim Erzeugen eines neuen Prozesses kann ein Namespace-Kontext:

```text
Inherited
Restricted
Extended by Authorized Projection
Replaced
```

werden.

Eine Vererbung darf keine zusätzliche Authority erzeugen.

## Lifecycle

Prozessspezifische Namespace-Einträge können an den Prozess-Lifecycle gebunden sein:

```text
Process Start
     ↓
Namespace Context
     ↓
Process Runtime
     ↓
Process Exit
     ↓
Release Process Projections
```

Persistente Objekte werden dadurch nicht gelöscht.

## Sicherheit

Der Process Namespace darf:

```text
Capability Checks
Filesystem Permissions
System Protection
System Write Policy
Trust Policy
```

nicht umgehen.

Ein sichtbares Objekt bleibt ohne entsprechende Authority unzugänglich.

## Normative Anforderungen

1. NovaOS MUSS prozessbezogene Namespace-Kontexte unterstützen.
2. `ProcessID` und `NamespaceID` MÜSSEN getrennte Identitäten bleiben.
3. Prozesse DÜRFEN unterschiedliche Sichten auf denselben globalen Namespace besitzen.
4. Namespace-Sichtbarkeit DARF keine Authority erzeugen.
5. Prozesse MÜSSEN Program-, Solution- und Workspace-Kontexte berücksichtigen können.
6. Private Program-Overlays MÜSSEN im zugehörigen Prozesskontext wirksam sein können.
7. Prozessspezifische Projektionen MÜSSEN einen definierten Scope besitzen.
8. Namespace-Vererbung DARF Authority nicht erweitern.
9. Prozessgebundene Projektionen MÜSSEN beim Ende ihres Lifecycles freigegeben werden können.
10. Persistente Objekte DÜRFEN durch das Ende eines Process Namespace nicht automatisch gelöscht werden.
11. Bereits autorisierte Handles MÜSSEN an ihre aufgelöste `ObjectID` gebunden bleiben.
12. Process Namespace, Projektionen, Overlays und Auflösung MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-POLICY-NAMESPACE-0001`
- `NPSPEC-PROGRAM-NAMESPACE-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-TEMP-PROCESS-0001`

## Ergebnis

NovaOS stellt jedem Prozess eine kontrollierte effektive Namespace-Sicht bereit. Dadurch können globale Ressourcen, private Programmabhängigkeiten, Workspaces, Solutions und temporäre Ressourcen kontextabhängig kombiniert werden, ohne Objektidentität oder Authority an Pfade und Sichtbarkeit zu koppeln.