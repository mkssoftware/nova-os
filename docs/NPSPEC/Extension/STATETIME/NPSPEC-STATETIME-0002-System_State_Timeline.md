# NPSPEC-STATETIME-0002 – System State Timeline

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die systemweite Timeline von `Nova.StateTime`.

Ziel ist, Zustandsänderungen verschiedener Objekte und Subsysteme zeitlich zusammenzuführen und als konsistente Entwicklung des Systems darzustellen.

## Grundprinzip

```text
System State t0
    ↓
Events / Changes
    ↓
System State t1
    ↓
Events / Changes
    ↓
System State t2
```

## Timeline

Die Timeline besteht aus geordneten Zustandsänderungen und Referenzen auf betroffene Temporal States.

Logisch:

```text
TimelineEntry {
    id
    order
    time
    affected_objects
    causal_reference
}
```

## Globale und lokale Zeit

NovaOS muss unterscheiden können zwischen:

```text
global timeline
object timeline
task timeline
```

Eine Objekt-Timeline beschreibt nur ein einzelnes Objekt.

Die System-Timeline stellt Zusammenhänge zwischen mehreren Zuständen her.

## Reihenfolge

Die Timeline muss eine eindeutige logische Reihenfolge unterstützen.

```text
Entry 100
    ↓
Entry 101
    ↓
Entry 102
```

Wall-Clock-Zeit darf zusätzlich gespeichert werden, ist aber nicht die alleinige Ordnungsgrundlage.

## Gleichzeitige Änderungen

Unabhängige Änderungen dürfen als parallel oder ohne feste Reihenfolge zueinander dargestellt werden.

```text
        ┌─ Object A change
Entry ──┤
        └─ Object B change
```

Nur tatsächlich notwendige Reihenfolgebeziehungen müssen erzwungen werden.

## Transaktionen

Mehrere Änderungen dürfen als gemeinsame atomare Zustandsänderung erscheinen.

```text
Transaction
    ├── Object A
    ├── Object B
    └── Object C
```

Die Timeline darf keinen Zwischenzustand als vollständig bestätigt darstellen, wenn die Transaktion noch nicht abgeschlossen ist.

## Causality

Timeline-Reihenfolge und Kausalität sind getrennte Konzepte.

```text
StateTime:
    Wann / in welcher Reihenfolge?

Causality:
    Warum?
```

Timeline Entries müssen auf relevante Causality-Daten verweisen können.

## Beispiel

```text
Timeline {
    100:
        document:42 → state:7

    101:
        image:18 → state:3

    102:
        document:42 → state:8
        project:5 → state:12
}
```

Damit kann NovaOS den Systemzustand zu verschiedenen Punkten rekonstruieren.

## Normative Anforderungen

1. NovaOS MUSS systemweite Zustandsänderungen in einer logisch geordneten Timeline darstellen können.
2. Timeline Entries MÜSSEN eindeutig referenzierbar sein.
3. Objekt-, Task- und System-Timelines MÜSSEN miteinander verknüpfbar sein.
4. Logische Reihenfolge MUSS unabhängig von Wall-Clock-Zeit darstellbar sein.
5. Atomare Transaktionen MÜSSEN als zusammengehörige Zustandsänderung erkennbar sein.
6. Unabhängige parallele Änderungen DÜRFEN ohne künstliche Reihenfolge dargestellt werden.
7. Timeline Entries MÜSSEN mit Causality-Informationen verknüpfbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- System State Timeline
- globale Zustandsreihenfolge
- parallele Änderungen
- Transaktionsbezug

Nicht Bestandteil sind:

- Temporal Object Identity
- Snapshot-Koordination
- historische Abfragen
- Restore und Branching
- Retention

## Zugehörige NPSPECs

- `NPSPEC-STATETIME-0001 – Temporal State Model`
- `NPSPEC-STATETIME-0003 – Temporal Object Identity`
- `NPSPEC-STATETIME-0004 – State Snapshot Coordination`
- `NPSPEC-STATETIME-0005 – Historical State Query`
- `NPSPEC-STATETIME-0006 – Temporal Restore & Branching`
- `NPSPEC-STATETIME-0007 – State Retention & Garbage Collection`
- `NPSPEC-CAUSAL-0001 – Causality Graph Model`