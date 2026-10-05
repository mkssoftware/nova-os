# NPSPEC-INDEX-REBUILD-0001 – Nova Index Rebuild

## Status

Angenommen

## Kategorie

Index / Recovery / Rebuild

## Zweck

NovaOS definiert den kontrollierten Neuaufbau abgeleiteter Indizes aus ihren maßgeblichen Datenquellen.

Ein beschädigter, verlorener oder veralteter Index muss wiederhergestellt werden können, ohne die zugrunde liegenden Objekte, Metadaten, Beziehungen, Namespaces oder Workspaces zu verändern.

## Grundprinzipien

```text
Index ≠ Source of Truth
Rebuild ≠ Data Recovery
Rebuild ≠ Object Modification
Index Loss ≠ Data Loss
Incomplete Index ≠ Invalid Source Data
```

## Unterstützte Indizes

Der Rebuild-Mechanismus gilt mindestens für:

```text
Search Index
Metadata Index
Namespace Index
Relation Index
Workspace Index
```

Weitere abgeleitete Indizes können denselben Mechanismus verwenden.

## Rebuild-Ablauf

```text
Rebuild Request
      ↓
Identify Index
      ↓
Validate Sources
      ↓
Create New Index Generation
      ↓
Scan Authoritative Data
      ↓
Build Entries
      ↓
Validate Index
      ↓
Atomic Switch
      ↓
Retire Old Generation
```

Der bisherige gültige Index soll während des Neuaufbaus weiter nutzbar bleiben, sofern dies möglich ist.

## Maßgebliche Quellen

Je nach Index werden unterschiedliche Quellen verwendet:

```text
Filesystem Objects
Metadata
Namespace
Relations
Workspace Manifest
Workspace State
```

Der Index darf niemals verwendet werden, um fehlende maßgebliche Daten zu rekonstruieren.

## Konsistenz

Änderungen während eines Rebuilds müssen berücksichtigt werden.

NovaOS kann dafür:

```text
Snapshot
Change Log
Generation Tracking
Incremental Catch-up
```

verwenden.

Vor Aktivierung muss die neue Indexgeneration einen konsistenten Stand erreicht haben.

## Teilweiser Rebuild

NovaOS soll sowohl vollständige als auch begrenzte Rebuilds unterstützen:

```text
Full Rebuild
Volume Rebuild
Namespace Rebuild
Workspace Rebuild
Object Range Rebuild
Index Partition Rebuild
```

Dadurch müssen kleine Inkonsistenzen nicht zwingend einen vollständigen Neuaufbau auslösen.

## Fehlerbehandlung

Schlägt der Rebuild fehl, bleibt eine vorhandene gültige Indexgeneration aktiv.

Eine unvollständige neue Generation darf nicht als vollständig gültiger Index veröffentlicht werden.

## Sicherheit

Der Rebuild läuft innerhalb eines autorisierten Systemkontexts.

Er darf keine zusätzlichen Zugriffsrechte erzeugen oder geschützte Informationen in weniger geschützte Indexbereiche übertragen.

## Normative Anforderungen

1. Jeder abgeleitete NovaOS-Index MUSS rekonstruierbar sein.
2. Ein Rebuild DARF maßgebliche Quelldaten nicht verändern.
3. Der Rebuild MUSS eine neue Indexgeneration getrennt aufbauen können.
4. Eine neue Generation MUSS vor Aktivierung validiert werden.
5. Der Wechsel auf eine neue Generation SOLL atomar erfolgen.
6. Änderungen während des Rebuilds MÜSSEN berücksichtigt werden können.
7. Fehlgeschlagene Rebuilds DÜRFEN einen vorhandenen gültigen Index nicht zerstören.
8. Teilweise Rebuilds SOLLEN unterstützt werden.
9. Ein unvollständiger Index DARF nicht als vollständig gültig veröffentlicht werden.
10. Sicherheits- und Sichtbarkeitsgrenzen MÜSSEN beim Rebuild erhalten bleiben.
11. Rebuild DARF keine zusätzliche Authority erzeugen.
12. Fortschritt und Fehler eines Rebuilds MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-INDEX-METADATA-0001`
- `NPSPEC-INDEX-NAMESPACE-0001`
- `NPSPEC-INDEX-RELATION-0001`
- `NPSPEC-INDEX-WORKSPACE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`

## Ergebnis

NovaOS kann seine abgeleiteten Indizes jederzeit kontrolliert neu aufbauen. Beschädigte oder verlorene Indexdaten gefährden dadurch weder die maßgeblichen Daten noch die Systemstruktur, während neue Indexgenerationen konsistent und atomar aktiviert werden können.