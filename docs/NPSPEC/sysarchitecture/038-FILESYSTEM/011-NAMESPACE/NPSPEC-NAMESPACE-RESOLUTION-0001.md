# NPSPEC-NAMESPACE-RESOLUTION-0001 – Nova Namespace Resolution

## Status

Angenommen

## Kategorie

Namespace / Resolution

## Zweck

NovaOS definiert die deterministische Auflösung eines logischen Pfades innerhalb eines konkreten Namespace-Kontexts zu einer stabilen `ObjectID` oder `ResourceID`.

Namespace Resolution bestimmt ausschließlich, welches Objekt ein Pfad bezeichnet. Die anschließende Autorisierung bleibt davon getrennt.

## Grundprinzipien

```text
Path ≠ ObjectID
Resolution ≠ Authority
Visibility ≠ Permission
Projection ≠ Copy
Overlay ≠ Object
Resolved ≠ Authorized
Context Determines View
```

## Eingaben

Eine Auflösung berücksichtigt mindestens:

```text
Path
NamespaceID
Security Context
Namespace Version
Active Projections
Active Overlays
Mounts
Resolution Policy
```

Der gleiche Pfad darf in unterschiedlichen Namespace-Kontexten auf unterschiedliche Objekte zeigen.

## Ablauf

```text
Path
 ↓
Select Namespace Context
 ↓
Normalize Path
 ↓
Resolve Namespace Entries
 ↓
Apply Mounts
 ↓
Apply Projections
 ↓
Apply Overlays
 ↓
Resolve Virtual Entries
 ↓
ObjectID / ResourceID
 ↓
Capability / Permission Check
 ↓
Authorized Handle
```

Die Autorisierung erfolgt erst gegen das aufgelöste Ziel.

## Kontext

Resolution muss die aktive Namespace-Hierarchie berücksichtigen können:

```text
System
  ↓
User
  ↓
Application / Solution / Workspace
  ↓
Process
```

Jede Ebene darf die effektive Sicht gemäß Policy einschränken oder durch autorisierte Projektionen erweitern.

## Overlays

Bei mehreren möglichen Einträgen gelten die deterministischen Prioritäten des aktiven Overlays.

```text
Candidate A → Priority 300
Candidate B → Priority 200
Base        → Priority 100

Result → Candidate A
```

Mehrdeutige Auflösung ohne definierte Priorität muss fehlschlagen.

## Mounts und Projektionen

Mounts und Projektionen werden als Namespace-Mappings behandelt.

Sie verändern weder die zugrunde liegende Objektidentität noch erzeugen sie Authority.

## Virtuelle Ressourcen

Virtuelle Namespace-Einträge werden an ihren registrierten Provider weitergeleitet:

```text
Virtual Entry
     ↓
Provider
     ↓
ObjectID / ResourceID
```

Ein Provider-Ausfall muss als definierter Resolution-Fehler behandelt werden.

## Handles

Nach erfolgreicher Auflösung und Autorisierung wird ein Handle an die konkrete Zielidentität gebunden.

```text
Path
 ↓
ObjectID
 ↓
Authorized Handle
```

Spätere Namespace-, Overlay- oder Mount-Änderungen dürfen dieses Handle nicht auf ein anderes Objekt umleiten.

## Fehler

Resolution muss mindestens unterscheiden können:

```text
NotFound
Unavailable
Ambiguous
InvalidPath
InvalidNamespace
ProjectionFailure
ProviderUnavailable
ResolutionLoop
DepthLimitExceeded
```

Zyklen und rekursive Projektionen müssen erkannt und begrenzt werden.

## Konsistenz

Eine einzelne Resolution muss gegen eine konsistente Namespace-Version erfolgen.

Ändert sich der Namespace während der Auflösung, muss NovaOS entweder mit derselben Version fortfahren oder die Resolution kontrolliert wiederholen.

## Normative Anforderungen

1. NovaOS MUSS Pfade innerhalb eines expliziten Namespace-Kontexts auflösen.
2. Resolution MUSS zu einer stabilen `ObjectID` oder `ResourceID` führen.
3. Resolution und Autorisierung MÜSSEN getrennte Schritte bleiben.
4. Derselbe Pfad DARF in unterschiedlichen Kontexten unterschiedliche Ziele besitzen.
5. Overlay-Prioritäten MÜSSEN deterministisch ausgewertet werden.
6. Mehrdeutige Auflösungen ohne definierte Regel MÜSSEN fehlschlagen.
7. Mounts, Projektionen und virtuelle Einträge MÜSSEN in die Resolution integrierbar sein.
8. Namespace-Auflösung DARF keine Authority erzeugen.
9. Resolution-Loops MÜSSEN erkannt und begrenzt werden.
10. Eine Resolution MUSS gegen einen konsistenten Namespace-Zustand erfolgen.
11. Bereits ausgegebene Handles DÜRFEN durch spätere Namespace-Änderungen nicht umgebunden werden.
12. Resolution-Pfad, verwendete Projektionen, Overlays und Ergebnis MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-NAMESPACE-SYSTEM-0001`
- `NPSPEC-NAMESPACE-USER-0001`
- `NPSPEC-NAMESPACE-APPLICATION-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-NAMESPACE-VIRTUAL-0001`
- `NPSPEC-POLICY-NAMESPACE-0001`

## Ergebnis

NovaOS besitzt einen einheitlichen deterministischen Resolver, der Pfade innerhalb ihrer konkreten Namespace-Sicht auf stabile Objekt- oder Ressourcenidentitäten abbildet. Pfadnavigation, Overlays, Projektionen und Mounts bleiben dabei strikt von Objektidentität und tatsächlicher Authority getrennt.