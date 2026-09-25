# NPSPEC-ARCH-TRANSACTIONS-0001 – Nova System Transactions

## Status

Angenommen

## Kategorie

Architecture / Transactions / State / Consistency

## Zweck

NovaOS verwendet Transaktionen für zusammengehörige Systemänderungen, die als konsistenter Zustandsübergang behandelt werden müssen.

```text
Current State
     ↓
Transaction
     ↓
New Consistent State
```

## Grundprinzip

Eine Transaktion folgt grundsätzlich:

```text
Begin
  ↓
Validate
  ↓
Prepare
  ↓
Commit
  ↓
Verify
```

Bei Fehlern vor dem Commit:

```text
Abort / Rollback
```

## Transaktionsmodell

Eine Transaktion besitzt mindestens:

```text
TransactionID
Operations
State
Constraints
Participants
Result
```

Zustände:

```text
Created
Preparing
Prepared
Committed
Aborted
Failed
Unknown
```

## Desired State

Änderungen am deklarativen Systemzustand können transaktional erfolgen.

```text
Desired State A
      ↓
Transaction
      ↓
Desired State B
      ↓
Reconciliation
```

Dadurch werden zusammengehörige Konfigurationsänderungen als Einheit sichtbar.

## Ressourcen

Benötigte Ressourcen können vor dem Commit vorbereitet oder reserviert werden.

```text
Validate
   ↓
Reserve
   ↓
Prepare
   ↓
Commit
```

Fehlende Ressourcen müssen möglichst vor dem Commit erkannt werden.

## Mehrere Subsysteme

Eine Transaktion darf mehrere Systemdienste betreffen.

```text
Transaction
 ├── Storage
 ├── Network
 └── Service
```

Die beteiligten Komponenten müssen ihre Transaktionssemantik explizit angeben.

NovaOS setzt keine globale ACID-Unterstützung für jede Operation voraus.

## Irreversible Operationen

Nicht jede Operation kann zurückgerollt werden.

Solche Operationen müssen als:

```text
Irreversible
```

oder mit einer definierten:

```text
Compensation
```

gekennzeichnet werden.

Irreversible Schritte sollen möglichst spät ausgeführt werden.

## Persistenz

Für persistente kritische Transaktionen muss der Commit-Zustand rekonstruierbar sein.

```text
Transaction Log
      ↓
Crash
      ↓
Recovery
      ↓
Committed / Aborted / Unknown
```

## Idempotenz

Wiederholbare Operationen sollen eine stabile Operation-ID besitzen.

```text
TransactionID
+
OperationID
```

Dadurch können Wiederholungen nach Fehlern erkannt werden.

## Unknown State

Kann nach einem Fehler nicht sicher festgestellt werden, ob ein Commit erfolgt ist:

```text
State = Unknown
```

gilt:

```text
Unknown ≠ Failed
Unknown ≠ Committed
```

Der Zustand muss vor einer gefährlichen Wiederholung geklärt werden.

## Normative Anforderungen

1. NovaOS MUSS zusammengehörige Systemänderungen transaktional ausführen können.
2. Jede Transaktion MUSS eine stabile `TransactionID` besitzen.
3. Operationen SOLLEN vor dem Commit validiert und vorbereitet werden.
4. Ressourcen SOLLEN vor einer garantierten Transaktion reserviert werden.
5. Ein Commit MUSS einen eindeutig definierten Zustandsübergang darstellen.
6. Persistente kritische Commits MÜSSEN rekonstruierbar sein.
7. Irreversible Operationen MÜSSEN explizit gekennzeichnet werden.
8. Nicht rückrollbare Fehler SOLLEN definierte Compensation verwenden können.
9. Wiederholbare Operationen SOLLEN idempotent gestaltet werden.
10. `Unknown` DARF NICHT automatisch als `Committed` oder `Failed` behandelt werden.
11. NovaOS DARF keine universelle globale ACID-Semantik für alle Systemoperationen voraussetzen.
12. Transaktionen MÜSSEN Security-, Resource- und Execution-Constraints einhalten.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-STATE-0007_Transactional_State`
- `ADR-STATE-0008_State_Rollback`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`

## Ergebnis

```text
System Change
     ↓
Validate + Prepare
     ↓
Commit
     ↓
Verify
     ↓
Consistent State
```

NovaOS erhält damit eine gemeinsame Grundlage für kontrollierte, nachvollziehbare und wiederherstellbare Zustandsänderungen.