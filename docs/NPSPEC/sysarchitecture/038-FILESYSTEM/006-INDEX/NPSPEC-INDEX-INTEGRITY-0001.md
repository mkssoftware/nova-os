# NPSPEC-INDEX-INTEGRITY-0001 – Nova Index Integrity

## Status

Angenommen

## Kategorie

Index / Integrity

## Zweck

NovaOS definiert die Integritätsprüfung aller abgeleiteten Indizes.

Beschädigte, unvollständige oder inkonsistente Indexdaten müssen erkannt werden können, ohne die zugrunde liegenden Originaldaten als beschädigt einzustufen.

## Grundprinzipien

```text
Index Integrity ≠ Source Integrity
Index Corruption ≠ Data Corruption
Valid Index Entry ≠ Valid Source Object
Index Repair ≠ Source Modification
Source of Truth > Index
```

## Integritätsmodell

Ein Index kann mindestens folgende Zustände besitzen:

```text
Valid
Stale
Incomplete
Inconsistent
Corrupted
Rebuilding
Unknown
```

`Unknown` darf nicht automatisch als `Valid` behandelt werden.

## Prüfung

NovaOS kann Indexeinträge gegen ihre maßgeblichen Quellen prüfen:

```text
Index Entry
    ↓
Referenced ObjectID
    ↓
Source State / Version
    ↓
Integrity Validation
```

Dabei können insbesondere geprüft werden:

```text
ObjectID
Object Version
Index Version
Source Version
References
Checksums
Generation
Completeness
```

## Inkonsistenzen

Typische Fehler sind:

```text
Missing Entry
Stale Entry
Invalid ObjectID
Deleted Source
Wrong Version
Broken Relation
Invalid Namespace Mapping
Duplicate Index Entry
Corrupted Index Structure
```

Ein fehlerhafter Indexeintrag darf die maßgeblichen Quelldaten nicht verändern.

## Generationen

Indizes sollen generationenbasiert verwaltet werden können:

```text
Generation N
     ↓
Integrity Check
     ↓
Valid
     ↓
Active
```

Eine neue oder reparierte Generation darf erst nach erfolgreicher Validierung als aktiv veröffentlicht werden.

## Automatische Reparatur

Lokale Inkonsistenzen dürfen durch gezieltes Reindexing repariert werden:

```text
Detect
  ↓
Isolate
  ↓
Reindex Affected Data
  ↓
Validate
```

Ist eine zuverlässige lokale Reparatur nicht möglich, wird ein vollständiger oder teilweiser Rebuild ausgelöst.

## Rebuild

```text
Integrity Failure
      ↓
Repair Possible?
   ↙            ↘
 Yes            No
  ↓              ↓
Repair       Rebuild
  ↓              ↓
Validate      Validate
```

Der Rebuild verwendet ausschließlich die maßgeblichen Quellen.

## Laufender Betrieb

Integritätsprüfungen dürfen:

```text
On Access
Periodic
Event-driven
During Boot
After Crash
After Update
Before Generation Switch
```

ausgeführt werden.

Prüfungen sollen nach Möglichkeit inkrementell erfolgen, um unnötige Systemlast zu vermeiden.

## Sicherheit

Manipulierte Indexdaten dürfen weder Authority erzeugen noch Sicherheitsprüfungen umgehen.

Ein Indexergebnis bleibt grundsätzlich ein Kandidat, der vor sicherheitsrelevanter Nutzung gegen den aktuellen Zustand validiert werden kann.

## Normative Anforderungen

1. NovaOS MUSS die Integrität abgeleiteter Indizes prüfen können.
2. Indexintegrität MUSS von der Integrität der Quelldaten getrennt behandelt werden.
3. `Unknown` DARF nicht automatisch als `Valid` gelten.
4. Veraltete und ungültige Referenzen MÜSSEN erkennbar sein.
5. Indexfehler DÜRFEN keine Änderungen an maßgeblichen Quelldaten verursachen.
6. Lokale Inkonsistenzen SOLLEN gezielt reparierbar sein.
7. Nicht zuverlässig reparierbare Indizes MÜSSEN rebuildbar sein.
8. Neue Indexgenerationen MÜSSEN vor Aktivierung validierbar sein.
9. Integritätsprüfungen SOLLEN inkrementell ausführbar sein.
10. Manipulierte Indexdaten DÜRFEN Sicherheitsprüfungen nicht umgehen.
11. Integritätsfehler MÜSSEN introspektierbar sein.
12. Indexreparatur DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-INDEX-METADATA-0001`
- `NPSPEC-INDEX-NAMESPACE-0001`
- `NPSPEC-INDEX-RELATION-0001`
- `NPSPEC-INDEX-WORKSPACE-0001`
- `NPSPEC-INDEX-REBUILD-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`

## Ergebnis

NovaOS kann beschädigte, veraltete und inkonsistente Indexzustände zuverlässig erkennen und gezielt reparieren oder neu aufbauen. Die maßgeblichen Daten bleiben dabei unangetastet und bilden jederzeit die Grundlage zur Wiederherstellung eines vertrauenswürdigen Indexzustands.