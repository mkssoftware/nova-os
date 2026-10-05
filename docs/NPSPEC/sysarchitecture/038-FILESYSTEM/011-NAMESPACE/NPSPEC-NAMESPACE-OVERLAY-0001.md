# NPSPEC-NAMESPACE-OVERLAY-0001 – Nova Namespace Overlay

## Status

Ersetzt

## Ersetzt durch

- `NPSPEC-NAMESPACE-OVERLAY-0002`

## Hinweis

Diese Fassung bleibt als historische Grundlage erhalten. Maßgeblich für Implementierung und spätere Spezifikationen ist `NPSPEC-NAMESPACE-OVERLAY-0002`, weil dort OverlayID, Prioritäten, Schreibziele und Authority-Grenzen präzisiert sind.

## Kategorie

Namespace / Overlay

## Zweck

NovaOS definiert Overlays als kontextabhängige Überlagerung mehrerer Namespace-Quellen zu einer gemeinsamen effektiven Sicht.

Ein Overlay verändert weder die zugrunde liegenden Objekte noch deren physische Speicherorte oder Identitäten.

## Grundprinzipien

```text
Overlay ≠ Copy
Overlay ≠ Mount
Overlay ≠ Object
Overlay ≠ Permission
Overlay ≠ Physical Merge
Effective View ≠ Global State
```

## Overlay-Modell

```text
NamespaceOverlay
├── OverlayID
├── Scope
├── BaseNamespace
├── OverlaySources
├── Priority
├── Policy
└── State
```

Mehrere Quellen können zu einer effektiven Sicht kombiniert werden:

```text
Base Namespace
      +
Overlay A
      +
Overlay B
      ↓
Effective Namespace
```

## Priorität

Bei Namenskonflikten bestimmt eine definierte Prioritätsreihenfolge, welcher Eintrag sichtbar wird.

```text
Higher Priority
      ↓
Lower Priority
      ↓
Base Namespace
```

Verdeckte Objekte bleiben bestehen und behalten ihre `ObjectID`.

## Kontext

Overlays können auf folgende Scopes begrenzt werden:

```text
User
Process
Program
Solution
Workspace
Recovery
```

Ein Overlay eines Kontexts darf andere Kontexte nicht automatisch verändern.

## Program SYS Overlay

Programme können private Systemabhängigkeiten besitzen:

```text
/Apps/<Program>/SYS/
```

Diese können innerhalb des Programmkontexts über den globalen System-Namespace gelegt werden:

```text
Global /System
      +
Program SYS
      ↓
Effective /System
```

Dadurch kann das Programm private Libraries, Runtimes oder andere Abhängigkeiten logisch im System-Namespace verwenden.

Der globale `/System`-Zustand bleibt unverändert.

## Objektidentität

Overlay-Einträge referenzieren vorhandene Objekte.

```text
Effective Path
     ↓
Resolve Overlay
     ↓
ObjectID
     ↓
Authorized Handle
```

Die Position innerhalb eines Overlays darf keine neue Objektidentität erzeugen.

## Schreibzugriffe

Ein Overlay muss definieren, wohin Schreiboperationen geleitet werden.

Mögliche Regeln sind:

```text
ReadOnly
WriteToBase
WriteToOverlay
CopyOnWrite
ExplicitTarget
```

Die Schreibregel erzeugt keine zusätzliche Authority.

## Dynamik

Overlays dürfen zur Laufzeit aktiviert, deaktiviert oder ersetzt werden.

Änderungen sollen transaktional veröffentlicht werden, sodass kein teilweise aktualisierter Namespace sichtbar wird.

Bereits aufgelöste Handles bleiben an ihre `ObjectID` gebunden.

## Sicherheit

Overlay-Auflösung darf:

```text
Permissions
Capabilities
System Protection
Trust Policy
System Write Policy
```

nicht umgehen.

Ein privates Overlay auf `/System` gewährt insbesondere keine Authority zur Veränderung des globalen `/System`.

## Normative Anforderungen

1. NovaOS MUSS kontextbezogene Namespace-Overlays unterstützen.
2. Overlays DÜRFEN keine zugrunde liegenden Objekte kopieren müssen.
3. Overlay-Prioritäten MÜSSEN deterministisch auflösbar sein.
4. Verdeckte Objekte MÜSSEN ihre bestehende `ObjectID` behalten.
5. Overlays MÜSSEN auf definierte Kontexte beschränkbar sein.
6. Programme MÜSSEN private `SYS`-Overlays verwenden können.
7. Private `SYS`-Overlays DÜRFEN den globalen `/System`-Zustand nicht verändern.
8. Schreibziele MÜSSEN für beschreibbare Overlays eindeutig definiert sein.
9. Overlay-Schreibregeln DÜRFEN keine zusätzliche Authority erzeugen.
10. Overlay-Änderungen SOLLEN transaktional veröffentlicht werden.
11. Bestehende autorisierte Handles DÜRFEN durch Overlay-Änderungen nicht auf andere Objekte umgelenkt werden.
12. Overlay-Auflösung, Quellen, Prioritäten und effektive Sicht MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-POLICY-NAMESPACE-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-PROGRAM-NAMESPACE-0001`
- `NPSPEC-POLICY-SYSTEMWRITE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

NovaOS kann mehrere Namespace-Quellen zu kontextabhängigen effektiven Sichten überlagern. Dadurch können insbesondere Programme private Systemabhängigkeiten transparent im `/System`-Namespace verwenden, ohne globale Systemdateien zu verändern, Objektidentitäten zu duplizieren oder zusätzliche Authority zu erhalten.
