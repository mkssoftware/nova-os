# ADR-STATE-0005 – State Snapshots

## Status

Angenommen

## Kategorie

State Management / Snapshots / Recovery / Observability

## Kontext

Der globale System State Graph verändert sich kontinuierlich.

Für Analyse, Planung, Recovery und Record/Replay wird jedoch häufig eine stabile Sicht auf einen bestimmten Systemzustand benötigt.

Eine Live-Abfrage kann sonst Zustände aus unterschiedlichen Zeitpunkten vermischen.

## Entscheidung

NovaOS unterstützt **versionierte State Snapshots** des Global System State Graph.

```text
Live State
    ↓
Snapshot
    ↓
Stable State View
```

Ein Snapshot repräsentiert eine definierte Sicht auf den Systemzustand zu einer bestimmten State Generation.

## Snapshot-Inhalt

Ein Snapshot enthält primär:

```text
Node IDs
Node States
Typed Relations
State Versions
Relevant Metadata
Freshness Information
```

Große Nutzdaten werden nicht automatisch kopiert.

Stattdessen werden bevorzugt stabile Referenzen wie:

```text
Object ID + Version ID
```

verwendet.

## Konsistenz

Snapshots können unterschiedliche Konsistenzanforderungen besitzen.

```text
Local Snapshot
Subsystem Snapshot
Coordinated Snapshot
```

Nicht jeder Snapshot muss einen vollständig atomaren Zustand des gesamten Systems darstellen.

Die erreichte Konsistenz muss jedoch explizit bekannt sein.

## Immutable Snapshot

Ein erzeugter Snapshot ist unveränderlich.

```text
Snapshot Generation 42
```

bleibt Generation 42, auch wenn sich der Live State weiterentwickelt.

Neue Zustände erzeugen neue Snapshots bzw. Generationen.

## Verwendung

State Snapshots können unter anderem verwendet werden für:

```text
Diagnostics
Execution Planning
Failure Analysis
Self-Healing
Record/Replay
Security Analysis
State Comparison
```

## Snapshot und Recovery

Ein State Snapshot ist nicht automatisch ein vollständiger Recovery Snapshot.

```text
State Snapshot
≠
Memory Snapshot
≠
Storage Snapshot
```

Er beschreibt Systemzustand und Beziehungen.

Für vollständige Wiederherstellung können zusätzliche Storage-, Memory- oder Configuration-Snapshots erforderlich sein.

## Vergleich

Snapshots können miteinander verglichen werden.

```text
Snapshot A
    ↓
State Diff
    ↑
Snapshot B
```

Dadurch können Änderungen an Nodes, Zuständen und Beziehungen nachvollzogen werden.

## Security

Ein Snapshot übernimmt die Schutzanforderungen der enthaltenen Informationen.

Die Existenz eines Snapshots erzeugt keine zusätzlichen Zugriffsrechte.

```text
Snapshot Access
      ↓
Authorization
      ↓
Filtered State View
```

## Lebensdauer

Snapshots besitzen eine definierte Retention Policy.

Nicht mehr benötigte Snapshots können verworfen werden.

Persistente Snapshots müssen explizit angefordert oder durch eine Systemfunktion benötigt werden.

## Normative Anforderungen

1. NovaOS MUSS versionierte State Snapshots unterstützen.
2. Snapshots MÜSSEN einer definierten State Generation zugeordnet sein.
3. Erzeugte Snapshots MÜSSEN unveränderlich sein.
4. Die Konsistenzklasse eines Snapshots MUSS erkennbar sein.
5. Große Nutzdaten SOLLEN über stabile Objekt- und Versionsreferenzen eingebunden werden.
6. Snapshots DÜRFEN keine zusätzlichen Berechtigungen erzeugen.
7. Snapshots MÜSSEN vergleichbar sein können.
8. State Snapshots DÜRFEN nicht automatisch mit vollständigen Recovery Snapshots gleichgesetzt werden.
9. Persistente Snapshots MÜSSEN einer Retention Policy unterliegen.

## Konsequenzen

### Positiv

- stabile Sicht auf dynamischen Systemzustand,
- bessere Fehler- und Zustandsanalyse,
- Grundlage für State Diff und Record/Replay,
- keine vollständige Kopie aller Systemdaten erforderlich.

### Negativ

- Snapshots benötigen zusätzlichen Speicher,
- verteilte konsistente Snapshots können aufwendig sein,
- Retention und Zugriffsschutz müssen verwaltet werden.

## Abhängigkeiten

- `ADR-STATE-0001_Globaler_System_State_Graph`
- `ADR-STATE-0002_Desired_und_Actual_State`
- `ADR-STATE-0003_State_Machines`
- `ADR-STATE-0004_State_Reconciliation`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-OBJECT-0002`
- `ADR-REALTIME-0007_Record_Replay`
- `ADR-OBSERVABILITY-0005_System_State_Graph`

## Ergebnis

NovaOS kann den dynamischen Systemzustand als stabile, versionierte Sicht festhalten:

```text
Live State
   ↓
Snapshot
   ↓
Analyze / Compare / Replay / Recover
```

State Snapshots frieren dabei die **Sicht auf den Zustand** ein, ohne automatisch sämtliche zugrunde liegenden Daten zu duplizieren.