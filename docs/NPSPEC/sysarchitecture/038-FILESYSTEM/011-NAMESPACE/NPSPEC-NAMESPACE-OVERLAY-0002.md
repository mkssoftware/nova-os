# NPSPEC-NAMESPACE-OVERLAY-0002 – Nova Namespace Overlay

## Status

Angenommen

## Kategorie

Namespace / Overlay

## Zweck

NovaOS definiert Overlays als kontextabhängige Zusammenführung mehrerer Namespace-Quellen zu einer effektiven Sicht.

Version `0002` präzisiert insbesondere Overlay-Auflösung, Prioritäten, Schreibziele und die Trennung zwischen logischer Sicht, Objektidentität und Authority.

## Grundprinzipien

```text
Overlay ≠ Copy
Overlay ≠ Mount
Overlay ≠ Permission
Overlay ≠ Physical Merge
Path ≠ ObjectID
Visibility ≠ Authority
Effective View ≠ Global State
```

## Overlay-Modell

```text
NamespaceOverlay
├── OverlayID
├── TargetNamespace
├── Scope
├── Sources[]
│   ├── SourceNamespace
│   ├── Priority
│   └── WritePolicy
├── ResolutionPolicy
├── State
└── Version
```

Ein Overlay erzeugt ausschließlich eine logische Sicht:

```text
Source A
   +
Source B
   +
Base Namespace
   ↓
Resolution
   ↓
Effective Namespace
```

## Auflösung

Namespace-Auflösung erfolgt deterministisch:

```text
Path
 ↓
Context
 ↓
Active Overlays
 ↓
Priority Resolution
 ↓
Namespace Entry
 ↓
ObjectID
 ↓
Authorized Handle
```

Erst nach der Auflösung wird gegen die tatsächliche `ObjectID` autorisiert.

## Prioritäten

Bei identischen Namen gewinnt die Quelle mit der höheren definierten Priorität.

```text
Priority 300 → Private Override
Priority 200 → Context Overlay
Priority 100 → Base Namespace
```

Prioritäten müssen innerhalb eines Overlays eindeutig auflösbar sein.

Ein verdecktes Objekt wird weder gelöscht noch verändert.

## Scopes

Overlays können begrenzt werden auf:

```text
User
Process
Program
Solution
Workspace
Recovery
```

Ein Overlay darf ausschließlich innerhalb seines Scopes wirksam sein.

## Program SYS Overlay

Ein Programm kann private Abhängigkeiten unter:

```text
/Apps/<Program>/SYS/
```

bereitstellen.

Diese werden im Programmkontext in den System-Namespace projiziert:

```text
Global /System
       +
/Apps/<Program>/SYS
       ↓
Effective /System
```

Physisch verbleiben die privaten Komponenten im Programmordner.

Der globale `/System`-Namespace wird nicht verändert.

## Schreibmodell

Jede beschreibbare Overlay-Quelle benötigt eine explizite Schreibregel:

```text
ReadOnly
WriteToSource
WriteToBase
CopyOnWrite
ExplicitTarget
```

Ist kein eindeutiges Schreibziel definiert, muss die Schreiboperation abgelehnt werden.

```text
Ambiguous Write Target
        ↓
       Deny
```

## Copy-on-Write

Bei `CopyOnWrite` darf eine private Kopie erzeugt werden.

Die Kopie erhält eine eigene `ObjectID`.

Das ursprüngliche Objekt bleibt unverändert.

```text
Original ObjectID A
       ↓
Copy-on-Write
       ↓
New ObjectID B
```

## Dynamische Änderungen

Overlays können zur Laufzeit:

```text
Activate
Deactivate
Replace
Reorder
Update
```

Änderungen müssen als neue konsistente Overlay-Version veröffentlicht werden.

Bereits ausgegebene Handles bleiben an die zuvor aufgelöste `ObjectID` gebunden.

## Sicherheit

Overlay-Auflösung darf keine Sicherheitsgrenze umgehen.

Insbesondere bleiben wirksam:

```text
Capability Authority
Filesystem Permissions
Namespace Policy
System Protection
System Write Policy
Trust Policy
```

Ein Overlay kann Sichtbarkeit verändern, aber niemals Authority erzeugen.

## Normative Anforderungen

1. NovaOS MUSS mehrere Namespace-Quellen deterministisch überlagern können.
2. Jedes Overlay MUSS eine stabile `OverlayID` besitzen.
3. Overlay-Sources MÜSSEN explizite Prioritäten besitzen.
4. Overlay-Auflösung MUSS vor der Objekt-Autorisierung erfolgen.
5. Das Ergebnis der Auflösung MUSS auf eine stabile `ObjectID` führen.
6. Sichtbarkeit durch ein Overlay DARF keine Authority erzeugen.
7. Overlays MÜSSEN auf definierte Scopes begrenzbar sein.
8. Private `SYS`-Overlays MÜSSEN physisch vom globalen `/System` getrennt bleiben.
9. Beschreibbare Overlays MÜSSEN ein eindeutiges Schreibziel definieren.
10. Mehrdeutige Schreibziele MÜSSEN abgelehnt werden.
11. Copy-on-Write-Kopien MÜSSEN eine eigene `ObjectID` erhalten.
12. Overlay-Änderungen MÜSSEN konsistent und versioniert veröffentlicht werden.
13. Bestehende Handles DÜRFEN durch spätere Overlay-Änderungen nicht umgebunden werden.
14. Overlays DÜRFEN System Protection oder Capability-Prüfungen nicht umgehen.
15. Overlay-Konfiguration, Auflösung, Priorität und effektive Quelle MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-PERMISSION-0001`
- `NPSPEC-POLICY-NAMESPACE-0001`
- `NPSPEC-PROGRAM-SYSOVERLAY-0001`
- `NPSPEC-PROGRAM-NAMESPACE-0001`
- `NPSPEC-POLICY-SYSTEMWRITE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein deterministisches, versioniertes Overlay-Modell, das mehrere Namespace-Quellen zu einer kontextabhängigen Sicht zusammenführt. Objektidentität, physischer Speicherort und Authority bleiben dabei vollständig von der Overlay-Darstellung getrennt.