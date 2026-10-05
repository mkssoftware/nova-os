# NPSPEC-INDEX-TRANSACTION-0001 – Nova Index Transactions

## Status

Angenommen

## Kategorie

Index / Transaction

## Zweck

NovaOS definiert transaktionale Änderungen an abgeleiteten Indizes.

Indexänderungen sollen erst sichtbar werden, wenn die zugehörige Änderung der maßgeblichen Daten erfolgreich committed wurde.

## Grundprinzipien

```text
Index Transaction ≠ Source Transaction
Index ≠ Source of Truth
Prepared ≠ Committed
Committed Source ≠ Immediately Indexed
Index Failure ≠ Source Rollback
```

## Transaktionsmodell

Indexänderungen werden an die zugrunde liegende Quelltransaktion gekoppelt:

```text
Source Transaction
      ↓
Stage Source Changes
      ↓
Prepare Index Changes
      ↓
Commit Source
      ↓
Publish Index Changes
      ↓
Verify
```

Der Index darf keinen Zustand als endgültig sichtbar machen, der in den maßgeblichen Daten noch nicht committed wurde.

## Indexänderungen

Eine Indextransaktion kann enthalten:

```text
Insert Entry
Update Entry
Remove Entry
Invalidate Entry
Update References
Change Generation
```

Mehrere zusammengehörige Änderungen sollen gemeinsam veröffentlicht werden können.

## Commit-Reihenfolge

Die maßgeblichen Daten besitzen Vorrang.

```text
Source Commit
     ↓
Index Commit
```

Schlägt die Indexaktualisierung nach erfolgreichem Source Commit fehl, bleiben die Quelldaten gültig.

Der betroffene Index wird anschließend als veraltet oder inkonsistent markiert und repariert.

## Generationen

Indextransaktionen dürfen generationenbasiert arbeiten:

```text
Generation N
     ↓
Stage Changes
     ↓
Validate
     ↓
Generation N+1
     ↓
Atomic Publish
```

Leser sehen dadurch entweder den vorherigen oder den neuen gültigen Indexzustand, jedoch keinen teilweise veröffentlichten Zwischenzustand.

## Konflikte

Parallele Indextransaktionen müssen Konflikte anhand von Versionen, Generationen oder betroffenen Einträgen erkennen können.

Veraltete Änderungen dürfen aktuelle Indexzustände nicht unkontrolliert überschreiben.

## Fehlerbehandlung

```text
Index Commit Failed
       ↓
Mark Stale / Inconsistent
       ↓
Targeted Repair
       ↓
Rebuild if Required
```

Ein Indexfehler darf keine bereits erfolgreich committed Quelltransaktion rückgängig machen.

## Sicherheit

Indextransaktionen laufen innerhalb eines autorisierten Systemkontexts.

Sie dürfen keine geschützten Informationen in Indexbereiche übertragen, in denen diese nicht sichtbar sein dürfen.

## Normative Anforderungen

1. Indexänderungen MÜSSEN an den Commit-Zustand ihrer maßgeblichen Daten gekoppelt werden können.
2. Nicht committed Quelldaten DÜRFEN nicht als endgültige Indexeinträge veröffentlicht werden.
3. Mehrere zusammengehörige Indexänderungen SOLLEN atomar sichtbar werden.
4. Leser DÜRFEN keinen teilweise veröffentlichten Indexzustand erhalten.
5. Ein Indexfehler DARF committed Quelldaten nicht zurückrollen.
6. Fehlgeschlagene Indexupdates MÜSSEN als inkonsistent oder veraltet erkennbar sein.
7. Konflikte zwischen parallelen Indextransaktionen MÜSSEN erkannt werden können.
8. Generationenwechsel SOLLEN atomar erfolgen.
9. Fehlgeschlagene Änderungen MÜSSEN gezielt reparierbar oder rebuildbar sein.
10. Indextransaktionen DÜRFEN keine zusätzliche Authority erzeugen.
11. Sicherheitsgrenzen MÜSSEN während der Indexaktualisierung erhalten bleiben.
12. Transaktionszustand und Fehler MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-INDEX-METADATA-0001`
- `NPSPEC-INDEX-NAMESPACE-0001`
- `NPSPEC-INDEX-RELATION-0001`
- `NPSPEC-INDEX-WORKSPACE-0001`
- `NPSPEC-INDEX-REBUILD-0001`
- `NPSPEC-INDEX-INTEGRITY-0001`
- `NPSPEC-FILESYSTEM-TRANSACTION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

NovaOS kann Indexänderungen konsistent an Transaktionen der maßgeblichen Daten koppeln. Indexzustände werden kontrolliert und atomar veröffentlicht, während Fehler ausschließlich den abgeleiteten Index betreffen und durch Reparatur oder Rebuild behoben werden können.