# ADR-TRANSACTION-0008 – Irreversible Operationen mit Rollback Barrier

## Status

Angenommen

## Kategorie

Transaction Architecture / Recovery / Consistency

## Kontext

Nicht jede Operation kann technisch zurückgesetzt oder semantisch kompensiert werden.

Beispiele sind:

- endgültiges Löschen,
- externe Seiteneffekte,
- irreversible Hardwareoperationen,
- unwiderrufliche Datenfreigaben,
- Aktionen gegenüber Systemen außerhalb der Transaktionskontrolle.

Solche Operationen dürfen nicht so behandelt werden, als wären sie vollständig rollbackfähig.

NovaOS benötigt daher eine explizite **Rollback Barrier**, die den Punkt kennzeichnet, ab dem ein vollständiger Rollback nicht mehr garantiert werden kann.

## Entscheidung

NovaOS unterstützt **Rollback Barriers** für irreversible oder nicht vollständig reversible Operationen.

```text
Transaction
    │
    ├── Rollbackfähig
    │
    ├── Rollback Barrier
    │
    └── Irreversibler Seiteneffekt
```

Vor der Rollback Barrier muss ein vollständiger Rollback grundsätzlich möglich sein.

Nach der Rollback Barrier gilt:

```text
Rollback ≠ garantiert möglich
```

Stattdessen gelten die explizit definierten Compensation-, Recovery- oder Reconciliation-Regeln.

## Grundprinzip

```text
Before Barrier
    → Rollback

Barrier
    → Irreversibility begins

After Barrier
    → Compensation / Recovery / Reconciliation
```

Eine Rollback Barrier ist kein Fehlerzustand. Sie ist eine explizite Transaktionsgrenze.

## Rollback Barrier

Eine Rollback Barrier bezeichnet den Übergang von einem reversiblen in einen nicht vollständig reversiblen Transaktionszustand.

```text
State A
   ↓
Operation 1
   ↓
Operation 2
   ↓
ROLLBACK BARRIER
   ↓
Irreversible Operation
   ↓
State B
```

Vor der Barrier kann die Transaktion auf den vorherigen konsistenten Zustand zurückgesetzt werden.

Nach der Barrier darf NovaOS keinen vollständigen technischen Rollback versprechen.

## Barrier Contract

Eine Rollback Barrier muss Bestandteil des Operations- beziehungsweise Transaction Contracts sein.

Der Contract beschreibt mindestens:

- Position der Barrier,
- betroffene Operation,
- betroffene Daten oder Ressourcen,
- Irreversibilität,
- mögliche Compensation,
- erforderliche Authority,
- Recovery-Verhalten,
- Verification,
- Verhalten bei `Unknown`.

```text
Operation
   │
   └── Barrier Contract
          ├── Irreversible Effect
          ├── Compensation
          ├── Recovery
          └── Verification
```

## Vorbereitung vor der Barrier

Vor einer Rollback Barrier soll NovaOS alle reversiblen Vorbereitungen soweit möglich abgeschlossen haben.

```text
Validate
   ↓
Reserve
   ↓
Prepare
   ↓
Verify
   ↓
ROLLBACK BARRIER
   ↓
Irreversible Effect
```

Fehler, die vor der Barrier auftreten, sollen über normalen Rollback behandelt werden.

## Commit und Rollback Barrier

Eine Rollback Barrier ersetzt keinen Transaction Commit.

```text
Rollback Barrier
      ≠
Transaction Commit
```

Die Barrier beschreibt die Reversibilität einer Operation.

Der Commit beschreibt den erfolgreichen Abschluss der gesamten Transaktion.

Eine Transaktion kann daher nach einer Barrier weiterhin fehlschlagen und einen Recovery- oder Compensation-Zustand erreichen.

## Fehler nach der Barrier

Tritt nach der Barrier ein Fehler auf, darf NovaOS nicht versuchen, einen nicht möglichen technischen Rollback zu simulieren.

```text
Barrier
   ↓
Irreversible Effect
   ↓
Failure
   ↓
Compensation?
 ┌─┴──────────┐
Yes           No
 ↓             ↓
Compensate   Recovery
 ↓             ↓
Verify      Reconcile
```

## Compensation

Wenn eine definierte Compensation existiert, muss diese nach der Barrier verwendet werden können.

Die Compensation muss nicht den ursprünglichen internen Zustand bitgenau herstellen.

Sie muss stattdessen die definierte semantische Zielbedingung wiederherstellen.

```text
Irreversible Effect
        ↓
Compensation
        ↓
Desired Semantic State
```

## Keine Compensation

Existiert keine Compensation, muss die Transaktion einen entsprechenden Recovery-Zustand erhalten.

```text
Irreversible Effect
        ↓
Failure
        ↓
No Compensation
        ↓
RecoveryRequired
```

Ein solcher Zustand darf nicht als erfolgreicher Rollback gemeldet werden.

## Unknown nach der Barrier

Wenn nach einem Crash oder Kommunikationsfehler nicht eindeutig festgestellt werden kann, ob der irreversible Seiteneffekt ausgeführt wurde, gilt der Zustand als `Unknown`.

```text
Barrier
   ↓
External / Irreversible Effect
   ↓
Crash
   ↓
Unknown
```

NovaOS darf den Seiteneffekt nicht blind erneut ausführen.

Stattdessen muss Verification oder Reconciliation erfolgen.

## Externe Seiteneffekte

Bei externen Systemen kann NovaOS die tatsächliche Ausführung unter Umständen nicht selbst kontrollieren.

```text
NovaOS Transaction
       ↓
External System
       ↓
Irreversible Effect
```

Die Rollback Barrier markiert daher insbesondere die Grenze zwischen lokaler Transaktionskontrolle und externem Seiteneffekt.

Das externe Ergebnis darf nur als sicher gelten, wenn es verifiziert werden kann.

## Ressourcen

Bei Ressourcen soll die Barrier möglichst spät liegen.

```text
Acquire
  ↓
Reserve
  ↓
Prepare
  ↓
Verify
  ↓
Barrier
  ↓
Irreversible Allocation / Effect
```

Nicht benötigte Ressourcen müssen vor der Barrier wieder freigegeben werden können.

Nach der Barrier gelten die für die Ressource definierten Compensation- und Recovery-Regeln.

## Dateien und Storage

Irreversible Storage-Operationen müssen eine Rollback Barrier explizit ausweisen.

Beispielsweise:

```text
Prepare Delete
      ↓
Snapshot / Version Check
      ↓
ROLLBACK BARRIER
      ↓
Permanent Delete
```

Wenn NovaOS durch Versionierung, Snapshot oder Trash-Semantik einen technischen Rollback ermöglichen kann, soll die Operation möglichst vor der tatsächlichen Irreversibilität geschützt werden.

## Security

Eine Rollback Barrier darf keine Security-Grenze umgehen.

Die irreversible Operation benötigt eine explizite Autorisierung.

```text
Authorization
     ↓
Policy Check
     ↓
Barrier
     ↓
Irreversible Effect
```

Nachträgliche Compensation darf ebenfalls nur innerhalb der gültigen Authority erfolgen.

## Data Sovereignty

Vor einer Rollback Barrier müssen Data-Sovereignty-Regeln geprüft werden, sofern der irreversible Seiteneffekt Daten betrifft.

Eine nachträgliche Compensation darf keine unzulässige Datenkopie oder Datenübertragung erzeugen.

```text
Data Policy
    ↓
Validation
    ↓
Barrier
    ↓
Irreversible Effect
```

## User Confirmation

Eine explizite Benutzerbestätigung kann für besonders folgenreiche irreversible Operationen erforderlich sein.

Sie ersetzt jedoch keine technische Transaktionssemantik.

```text
User Confirmation
       ≠
Rollback Guarantee
```

NovaOS muss insbesondere verhindern, dass eine Benutzerbestätigung als Begründung für fehlende Recovery-Semantik verwendet wird.

## Persistenz und Recovery

Die Rollback Barrier muss bei persistenten Transaktionen recoveryfähig protokolliert werden.

Nach einem Neustart muss erkennbar sein, ob eine Transaktion:

```text
BeforeBarrier
AtBarrier
AfterBarrier
Compensating
RecoveryRequired
Completed
```

war.

Die genaue Persistenz erfolgt über den Transaction Log.

## Verification

Nach einer Operation hinter der Rollback Barrier muss der tatsächliche Zustand verifiziert werden, soweit dies möglich ist.

```text
Irreversible Effect
        ↓
Verification
        ↓
   ┌────┴────┐
   ↓         ↓
Verified   Unknown
   ↓         ↓
Continue   Recovery
```

Ein nicht verifizierter Seiteneffekt darf nicht als sicher erfolgreich betrachtet werden.

## Normative Anforderungen

1. NovaOS MUSS irreversible Operationen explizit erkennen und kennzeichnen können.
2. NovaOS MUSS Rollback Barriers als explizite Transaktionsgrenzen unterstützen.
3. Vor einer Rollback Barrier MUSS ein definierter Rollback-Pfad bestehen, sofern die vorherigen Operationen als rollbackfähig klassifiziert sind.
4. Eine Rollback Barrier DARF nicht mit einem Transaction Commit gleichgesetzt werden.
5. Nach einer Rollback Barrier DARF NovaOS keinen vollständigen Rollback garantieren, sofern dieser technisch nicht möglich ist.
6. Irreversible Operationen MÜSSEN eine definierte Compensation-, Recovery- oder Reconciliation-Semantik besitzen.
7. Fehlt eine solche Semantik, MUSS der entsprechende Zustand als `RecoveryRequired` behandelt werden.
8. Ein nach der Barrier auftretender `Unknown`-Zustand MUSS von `Failed` und `Succeeded` unterschieden werden.
9. NovaOS DARF einen unbekannten irreversiblen Seiteneffekt nicht blind erneut ausführen.
10. Rollback Barriers MÜSSEN bei persistenten Transaktionen recoveryfähig protokolliert werden.
11. Irreversible Operationen MÜSSEN vor ihrer Ausführung die erforderlichen Security- und Data-Sovereignty-Prüfungen durchlaufen.
12. Compensation MUSS selbst den geltenden Security- und Data-Sovereignty-Regeln unterliegen.
13. Der Zustand nach einer Rollback Barrier MUSS soweit möglich verifiziert werden.
14. Eine nicht verifizierte Wiederherstellung DARF nicht als erfolgreich abgeschlossen gelten.
15. Irreversible Operationen SOLLEN so spät wie möglich innerhalb einer Transaktion ausgeführt werden.
16. Eine Benutzerbestätigung DARF keine fehlende technische Recovery-Semantik ersetzen.

## Konsequenzen

### Positive Konsequenzen

- irreversible Seiteneffekte werden explizit sichtbar,
- Rollback-Garantien werden nicht überdehnt,
- Recovery nach externen oder irreversiblen Effekten wird kontrollierbar,
- `Unknown`-Zustände können korrekt behandelt werden,
- Transaktionen werden für Crash Recovery besser beschreibbar,
- irreversible Operationen können sicher in größere Transaktionsabläufe integriert werden.

### Negative Konsequenzen

- Operation Contracts werden komplexer,
- Compensation und Reconciliation müssen definiert werden,
- bestimmte Fehlerzustände können nicht vollständig automatisch behoben werden,
- Recovery kann menschliche oder externe Unterstützung benötigen.

## Verworfene Alternativen

### Irreversible Operationen ohne Barrier

Verworfen, da dadurch keine eindeutige Grenze zwischen rollbackfähigem und nicht rollbackfähigem Zustand existiert.

### Alles als Rollback behandeln

Verworfen, da irreversible Seiteneffekte technisch nicht zuverlässig zurückgesetzt werden können.

### Barrier = Commit

Verworfen, da eine irreversible Operation nicht automatisch den erfolgreichen Abschluss der gesamten Transaktion bedeutet.

### Blindes Wiederholen nach Unknown

Verworfen, da ein bereits ausgeführter irreversibler Seiteneffekt dadurch mehrfach ausgelöst werden könnte.

### Nur Benutzerbestätigung

Verworfen, da Benutzerbestätigung keine technische Recovery- oder Konsistenzgarantie darstellt.

## Abhängigkeiten

- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0003_Configuration_Transactions`
- `ADR-TRANSACTION-0004_Resource_Transactions`
- `ADR-TRANSACTION-0005_Multi-Service_Transactions`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`
- `ADR-TRANSACTION-0007_Rollback_und_Compensation`
- `ADR-RESILIENCE-0005_Retry_nur_nach_expliziter_Operationssemantik`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`

## Zugehörige NPSPECs

- `NPSPEC-TRANSACTION-ROLLBACK-BARRIER-0001`
- `NPSPEC-TRANSACTION-IRREVERSIBLE-OPERATION-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-TRANSACTION-RECOVERY-0001`
- `NPSPEC-TRANSACTION-UNKNOWN-STATE-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-BARRIER-TEST-0001`

## Ergebnis

NovaOS führt mit der **Rollback Barrier** eine explizite Grenze zwischen reversiblen und irreversiblen Transaktionszuständen ein.

```text
Rollbackfähig
     ↓
Rollback Barrier
     ↓
Irreversibler Effekt
     ↓
Compensation / Recovery / Reconciliation
     ↓
Verification
```

Damit kann NovaOS klar zwischen dem unterscheiden, was technisch zurückgesetzt werden kann, und dem, was nur noch semantisch kompensiert oder verifiziert werden kann.

Die Rollback Barrier verhindert insbesondere, dass NovaOS nach einem irreversiblen Seiteneffekt fälschlicherweise einen vollständigen Rollback verspricht.