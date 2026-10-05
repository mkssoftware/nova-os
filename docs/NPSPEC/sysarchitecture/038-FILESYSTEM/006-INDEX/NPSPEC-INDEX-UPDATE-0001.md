# NPSPEC-INDEX-UPDATE-0001 – Nova Index Update

## Status

Angenommen

## Kategorie

Index / Update

## Zweck

NovaOS definiert die laufende Aktualisierung abgeleiteter Indizes nach Änderungen ihrer maßgeblichen Datenquellen.

Index Updates halten Such-, Metadaten-, Namespace-, Relations- und Workspace-Indizes möglichst aktuell, ohne die Quelldaten selbst zu verändern.

## Grundprinzipien

```text
Index Update ≠ Source Update
Index ≠ Source of Truth
Source Change → Index Update
Index Delay ≠ Source Failure
Update Failure ≠ Data Loss
```

## Update-Modell

Änderungen an maßgeblichen Daten erzeugen ein Index-Ereignis:

```text
Source Change
     ↓
Index Event
     ↓
Determine Affected Indexes
     ↓
Generate Update
     ↓
Validate
     ↓
Publish
```

## Ereignisse

Index Updates können insbesondere ausgelöst werden durch:

```text
Create
Modify
Rename
Move
Delete
Metadata Change
Relation Change
Namespace Change
Workspace Change
Projection Change
```

Ein Ereignis darf mehrere Indizes betreffen.

## Inkrementelle Aktualisierung

NovaOS soll bevorzugt nur die tatsächlich betroffenen Indexeinträge aktualisieren.

```text
Changed Object
     ↓
Affected Entries
     ↓
Incremental Update
```

Ein vollständiger Rebuild ist nur erforderlich, wenn eine gezielte Aktualisierung nicht zuverlässig möglich ist.

## Asynchrone Verarbeitung

Index Updates dürfen asynchron verarbeitet werden.

```text
Source Commit
     ↓
Index Update Queue
     ↓
Indexer
     ↓
Updated Index
```

Die Quelldaten bleiben auch dann gültig, wenn die Indexaktualisierung verzögert ist.

## Reihenfolge

Änderungen müssen in einer nachvollziehbaren Reihenfolge verarbeitet werden können.

Versionen oder Generationen verhindern, dass ältere Updates einen neueren Indexzustand überschreiben.

```text
Version N
   ↓
Version N+1
   ↓
Version N+2
```

Veraltete Update-Ereignisse müssen erkannt werden können.

## Fehlerbehandlung

Schlägt ein Update fehl:

```text
Update Failure
      ↓
Mark Entry Stale
      ↓
Retry / Repair
      ↓
Rebuild if Required
```

Ein Fehler im Index darf die bereits committed Quelldaten nicht zurückrollen.

## Transaktionen

Gehören mehrere Änderungen zu derselben Quelltransaktion, sollen die daraus entstehenden Indexänderungen gemeinsam veröffentlicht werden können.

Nicht committed Quelldaten dürfen nicht als endgültige Indexänderungen erscheinen.

## Sicherheit

Index Updates dürfen nur Informationen übernehmen, die gemäß Index- und Sicherheits-Policy indexiert werden dürfen.

Eine Aktualisierung erzeugt keine zusätzliche Authority.

## Normative Anforderungen

1. Änderungen maßgeblicher Daten MÜSSEN Index Updates auslösen können.
2. Index Updates SOLLEN inkrementell erfolgen.
3. Ein Source Change DARF mehrere Indizes aktualisieren.
4. Index Updates DÜRFEN asynchron verarbeitet werden.
5. Verzögerte Updates MÜSSEN als potenziell veralteter Indexzustand erkennbar sein.
6. Ältere Updates DÜRFEN neuere Indexzustände nicht überschreiben.
7. Fehlgeschlagene Updates DÜRFEN committed Quelldaten nicht verändern.
8. Fehlgeschlagene Updates MÜSSEN wiederholbar oder reparierbar sein.
9. Nicht zuverlässig reparierbare Zustände MÜSSEN einen Rebuild auslösen können.
10. Transaktional zusammengehörige Änderungen SOLLEN gemeinsam sichtbar werden.
11. Sicherheits- und Sichtbarkeitsregeln MÜSSEN bei Updates erhalten bleiben.
12. Index Updates DÜRFEN keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-INDEX-METADATA-0001`
- `NPSPEC-INDEX-NAMESPACE-0001`
- `NPSPEC-INDEX-RELATION-0001`
- `NPSPEC-INDEX-WORKSPACE-0001`
- `NPSPEC-INDEX-REBUILD-0001`
- `NPSPEC-INDEX-INTEGRITY-0001`
- `NPSPEC-INDEX-TRANSACTION-0001`

## Ergebnis

NovaOS hält seine abgeleiteten Indizes durch ereignisbasierte und bevorzugt inkrementelle Aktualisierungen mit den maßgeblichen Daten synchron. Verzögerungen oder Fehler beeinträchtigen nicht die Quelldaten und können durch Retry, Reparatur oder Rebuild kontrolliert behoben werden.