# NPSPEC-STATE-HISTORY-0001 – Nova State History

## Status

Angenommen

## Kategorie

State / History / Traceability

## Zweck

NovaOS definiert State History als nachvollziehbare Historie relevanter Zustandsänderungen.

```text
State v1
   ↓
State v2
   ↓
State v3
   ↓
State v4
```

Die Historie ermöglicht Analyse, Recovery, Debugging, Audit, Provenance und deterministisches Replay, ohne vorauszusetzen, dass jede historische Version vollständig gespeichert wird.

## Grundprinzipien

```text
State History ≠ Log
State History ≠ Snapshot
State History ≠ Backup
State History ≠ Audit Trail
State History ≠ Current State
Historical State ≠ Valid Current State
History Entry ≠ Full State Copy
Rollback ≠ History Rewriting
```

## History Model

```text
StateHistoryEntry
├── HistoryID
├── StateID
├── PreviousVersion
├── NewVersion
├── Transition
├── Timestamp
└── Result
```

Optional:

```text
Generation
ActorID
TransactionID
SnapshotID
ProvenanceID
ExecutionContractID
Cause
IntegrityState
```

## Historische Kette

State History bildet die Entwicklung eines logischen Zustands ab.

```text
StateID A

v1 → v2 → v3 → v4
```

State Identity bleibt dabei konstant.

```text
StateID ≠ StateVersion
```

## History Entry

Ein History Entry beschreibt primär die Änderung zwischen Zuständen.

```text
History Entry
├── Source Version
├── Target Version
├── Operation
├── Cause
└── Result
```

Er muss nicht den vollständigen State enthalten.

## Speicherung

Je nach State Type können unterschiedliche Verfahren verwendet werden:

```text
Full State
Delta
Event
Transaction Reference
Snapshot Reference
Version Metadata
```

NovaOS schreibt kein universelles Speicherformat für alle State Histories vor.

## State Reconstruction

Historischer State kann rekonstruiert werden aus:

```text
Base Snapshot
      +
State Changes
      ↓
Historical State
```

Rekonstruktion muss Integrität und Versionsfolge prüfen.

```text
Reconstructed ≠ Verified
```

## Transactions

Zusammengehörige Änderungen müssen mit ihrer Transaction verbunden werden können.

```text
Transaction T42
├── State A: v4 → v5
├── State B: v9 → v10
└── State C: v2 → v3
```

History darf vorbereitete Änderungen nicht mit committed Änderungen verwechseln.

## Rollback

Rollback löscht oder überschreibt die Historie nicht.

```text
v1 → v2 → v3 → v4
               ↓
Rollback content to v2
               ↓
              v5
```

Dadurch bleibt nachvollziehbar, dass der Rollback nach `v4` erfolgte.

## Snapshots

Snapshots können Referenzpunkte innerhalb der History bilden.

```text
v10
 ↓
Snapshot S1
 ↓
v11
 ↓
v12
```

Snapshot und State History bleiben getrennte Konzepte.

## Branching

In bestimmten Subsystemen kann State History Verzweigungen besitzen.

```text
        v4
       /  \
     v5A  v5B
```

Das jeweilige Consistency- oder Merge-Modell muss definieren, wie solche Zweige behandelt werden.

Eine universelle lineare globale Historie wird nicht vorausgesetzt.

## Distributed History

Verteilte Systeme können mehrere lokale Historien besitzen.

```text
Node A History
Node B History
Node C History
```

Zur Korrelation können beispielsweise verwendet werden:

```text
TransactionID
Causal Metadata
Logical Clock
TraceID
ProvenanceID
```

```text
Local Ordering ≠ Global Ordering
```

## Retention

State History darf nicht unbegrenzt wachsen.

Retention Policies können berücksichtigen:

```text
Age
Size
State Type
Security Relevance
Recovery Requirements
Audit Requirements
Snapshot Availability
```

Ältere Detailinformationen können verdichtet werden, sofern erforderliche Garantien erhalten bleiben.

## Security

Historische Daten können sensitive Informationen enthalten.

Zugriff muss Capability-basiert kontrolliert werden.

Mögliche Rechte:

```text
ReadHistory
QueryHistory
ReconstructState
ExportHistory
PruneHistory
```

```text
Read Current State ≠ Read Full History
```

## Monotone Security State

Historische Sicherheitszustände dürfen nicht als aktuelle Authority verwendet werden.

```text
Historical Capability = Valid
Current Capability = Revoked
```

Die historische Version darf die aktuelle Revocation nicht überschreiben.

## Integrity

Kritische State History soll Manipulationen erkennen können.

Mögliche Mechanismen:

```text
Checksums
Hash Chains
Authenticated Metadata
Signatures
Transaction References
Immutable Records
```

## Provenance

State History und Provenance ergänzen sich.

```text
State History
→ What changed?

Provenance
→ Why, by whom and from what?
```

History Entries können Provenance Records referenzieren, ohne sämtliche Provenance-Daten zu duplizieren.

## Replay

State History kann deterministisches Replay unterstützen.

```text
Known Snapshot
      +
Ordered State Changes
      +
Recorded External Inputs
      ↓
Replay
```

State History allein garantiert keine vollständige Reproduzierbarkeit.

## Pruning

Historie darf kontrolliert reduziert werden.

```text
Detailed History
      ↓
Checkpoint / Snapshot
      ↓
Validated Compaction
      ↓
Reduced History
```

Erforderliche Recovery-, Security-, Audit- und Provenance-Informationen dürfen dabei nicht unzulässig verloren gehen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
StateID
Current Version
Historical Versions
Transitions
Transactions
Snapshots
Provenance
Branches
Retention State
Integrity State
```

## Normative Anforderungen

1. NovaOS MUSS relevante State History erfassen können.
2. State History MUSS von Logging, Snapshots und Backups getrennt bleiben.
3. History Entries MÜSSEN StateID und Versionsbezug besitzen können.
4. State History MUSS nicht jede vollständige State-Version speichern.
5. Unterschiedliche State Types DÜRFEN unterschiedliche History-Strategien verwenden.
6. Historischer State MUSS bei ausreichenden Daten rekonstruierbar sein können.
7. Rekonstruierter State DARF NICHT automatisch als verifiziert gelten.
8. Transaction References MÜSSEN in State History darstellbar sein.
9. Prepared und Committed Changes MÜSSEN unterscheidbar bleiben.
10. Rollback DARF bestehende Historie NICHT überschreiben.
11. Rollback SOLL eine neue State-Version erzeugen.
12. Snapshots MÜSSEN als History-Referenzpunkte verwendbar sein.
13. State History MUSS Verzweigungen unterstützen können, wenn das Subsystem sie benötigt.
14. Distributed State History DARF keine universelle globale Reihenfolge voraussetzen.
15. Lokale und kausale Ordnung MÜSSEN unterscheidbar sein können.
16. State History MUSS Retention Policies unterstützen.
17. History-Zugriff MUSS Capability-basiert kontrollierbar sein.
18. Historische Authority DARF aktuelle Security States NICHT überschreiben.
19. Kritische Historien SOLLEN Integritätsschutz unterstützen.
20. State History SOLL mit Provenance verknüpfbar sein.
21. State History MUSS deterministisches Replay unterstützen können.
22. History Pruning MUSS kontrolliert und validierbar erfolgen.
23. Kritische Recovery- und Security-Informationen DÜRFEN durch Pruning NICHT unzulässig verloren gehen.
24. State History MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STATE-GLOBAL-0001`
- `NPSPEC-STATE-ACTUAL-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-RECONCILIATION-0001`
- `NPSPEC-STATE-SNAPSHOT-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-TRANSACTION-LOG-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `ADR-ARCH-0160`

## Ergebnis

```text
State Change
     ↓
Create New Version
     ↓
Record History Entry
     ↓
Link Transaction / Provenance / Snapshot
     ↓
Protect + Retain
     ↓
Query / Reconstruct / Replay / Recover
```

NovaOS erhält damit eine nachvollziehbare State History, die Zustandsentwicklung über Versionen hinweg dokumentiert, ohne History mit Logging, Snapshots oder Backups gleichzusetzen, und eine gemeinsame Grundlage für Recovery, Debugging, Replay, Provenance und Systemanalyse bildet.