# NPSPEC-STORAGE-OVERLAY-0001 – Nova Storage Overlay

## Status

Angenommen

## Kategorie

Storage / Overlay / VFS / Namespace

## Zweck

NovaOS definiert Storage Overlays als Mechanismus, mit dem mehrere Storage-Schichten zu einer gemeinsamen logischen Sicht kombiniert werden können.

```text
Writable Layer
      ↓
Base Layer
      ↓
Unified View
```

Overlays ermöglichen insbesondere temporäre Änderungen, System-Experimente, Recovery, Updates und schreibbare Ansichten auf unveränderlichen Datenbeständen.

## Grundprinzip

```text
Logical View ≠ Physical Layer
Overlay ≠ Full Copy
Base Layer ≠ Modified Layer
```

Änderungen werden bevorzugt in einer separaten Overlay-Schicht gespeichert, während die zugrunde liegende Basisschicht unverändert bleiben kann.

## Overlay-Modell

Ein Overlay besteht mindestens aus:

```text
OverlayID
Upper Layer
Lower Layer(s)
State
Policy
```

Die obere Schicht enthält Änderungen, während untere Schichten bestehende Daten bereitstellen.

## Layer

Mehrere Layer dürfen kombiniert werden.

```text
Upper Layer      ← writable
Middle Layer     ← optional
Base Layer       ← read-only
```

Die Anzahl der Layer soll begrenzt werden können, um unnötige Lookup- und Verwaltungsaufwände zu vermeiden.

## Auflösung

Beim Zugriff wird das sichtbarste gültige Objekt verwendet.

```text
Lookup
  ↓
Upper Layer?
  ├─ Yes → Use
  └─ No
      ↓
   Lower Layer
```

Die Layer-Reihenfolge muss eindeutig definiert sein.

## Schreiben

Änderungen sollen ausschließlich in einer schreibbaren Layer erfolgen.

```text
Base Object
    ↓
Modify
    ↓
Copy / Reference
    ↓
Upper Layer
```

Die Basisschicht bleibt dabei unverändert.

Copy-on-Write darf verwendet werden, um unnötige vollständige Kopien zu vermeiden.

## Löschen

Das Löschen eines Objekts aus der Overlay-Sicht darf durch einen Tombstone beziehungsweise Whiteout dargestellt werden.

```text
Base Object
    +
Whiteout
    ↓
Not Visible
```

Das Objekt der Basisschicht wird dadurch nicht physisch gelöscht.

## Objektidentität

Overlay-Verarbeitung darf stabile NovaOS-Objektidentitäten nicht unnötig verändern.

```text
ObjectID
   ↓
Different Physical Representation
   ↓
Same Logical Object
```

Erzeugt die Overlay-Schicht tatsächlich ein neues unabhängiges Objekt, muss eine neue `ObjectID` vergeben werden.

## Commit

Eine Overlay-Schicht darf dauerhaft in eine Zielschicht übernommen werden.

```text
Overlay
   ↓
Validate
   ↓
Transaction
   ↓
Commit
   ↓
Updated Base
```

Der Commit muss kontrolliert und möglichst transaktional erfolgen.

## Discard

Temporäre Overlays dürfen vollständig verworfen werden.

```text
Base + Overlay
      ↓
Discard
      ↓
Original Base
```

Dadurch können Änderungen rückstandsfrei entfernt werden, sofern keine externen persistenten Referenzen erzeugt wurden.

## Einsatzbereiche

Storage Overlays dürfen insbesondere verwendet werden für:

```text
System Updates
Safe Mode
Recovery
Application Sandboxes
Temporary Sessions
Testing
Immutable System Areas
```

Sie dürfen jedoch nicht für jede normale Storage-Operation verpflichtend sein.

## Snapshots und Versionierung

Overlay, Snapshot und Versionierung bleiben getrennte Konzepte.

```text
Snapshot   → fixed state
Version    → object history
Overlay    → layered view
```

Sie dürfen miteinander kombiniert werden.

## Sicherheit

Eine Overlay-Schicht darf Security- oder Capability-Regeln der Basisschicht nicht umgehen.

Eine weniger vertrauenswürdige Upper Layer darf geschützte Systemobjekte nicht ohne entsprechende Autorisierung überschreiben oder maskieren.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
OverlayID
Layer Order
Writable Layer
Base Layers
State
Changed Objects
Whiteouts
Commit State
```

## Normative Anforderungen

1. NovaOS MUSS Storage Overlays als optionale VFS-Funktion unterstützen können.
2. Overlay und zugrunde liegende Storage-Schichten MÜSSEN logisch getrennt bleiben.
3. Die Layer-Reihenfolge MUSS eindeutig definiert sein.
4. Schreiboperationen MÜSSEN in eine dafür autorisierte schreibbare Layer erfolgen.
5. Read-only-Basisschichten DÜRFEN durch Overlay-Schreiboperationen NICHT verändert werden.
6. NovaOS SOLL Copy-on-Write für effiziente Overlays unterstützen.
7. Löschungen DÜRFEN über Whiteouts oder vergleichbare Mechanismen dargestellt werden.
8. Overlays DÜRFEN stabile Objektidentitäten nicht unnötig verändern.
9. Overlay-Commit SOLL transaktional und verifizierbar erfolgen.
10. Temporäre Overlays MÜSSEN kontrolliert verworfen werden können.
11. Overlays DÜRFEN Security- und Capability-Regeln NICHT umgehen.
12. Layer, Änderungen und Commit-Zustand SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `ADR-STORAGE-0015`

## Ergebnis

```text
Immutable Base
      +
Writable Overlay
      ↓
Unified Storage View
      ↓
Commit / Discard
```

NovaOS erhält damit eine flexible Overlay-Schicht für temporäre, isolierte oder kontrolliert persistierbare Storage-Änderungen, ohne zugrunde liegende Daten unnötig zu kopieren oder zu verändern.