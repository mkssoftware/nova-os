# ADR-TRANSACTION-0007 – Rollback und Compensation

## Status

Angenommen

## Kategorie

Transaction Architecture / Recovery / Consistency

## Kontext

Nicht jede Transaktion kann nach einem Fehler durch einfaches Zurücksetzen des physischen Zustands rückgängig gemacht werden.

Insbesondere bei:

- mehreren Services,
- externen Systemen,
- bereits sichtbaren Seiteneffekten,
- irreversiblen Operationen,
- Ressourcenreservierungen,
- verteilten Transaktionen

kann ein klassischer Rollback nicht immer vollständig möglich sein.

NovaOS benötigt deshalb eine klare Trennung zwischen **Rollback** und **Compensation**.

## Entscheidung

NovaOS unterstützt zwei unterschiedliche Mechanismen:

```text
Rollback
    = Zustand technisch zurücksetzen

Compensation
    = fachlich/semantisch eine Gegenoperation ausführen
```

Rollback wird bevorzugt, wenn der ursprüngliche Zustand sicher und atomar wiederhergestellt werden kann.

Compensation wird verwendet, wenn ein technischer Rollback nicht möglich oder nicht sinnvoll ist.

## Grundprinzip

```text
Fehler
  │
  ├── Rollback möglich
  │      ↓
  │   Zustand zurücksetzen
  │
  └── Rollback nicht möglich
         ↓
      Compensation
         ↓
   Gegenoperation / Reconciliation
```

Compensation ist kein Ersatz für atomare Transaktionen, sondern ein Mechanismus für Operationen, deren Seiteneffekte nicht vollständig zurückgesetzt werden können.

## Rollback

Rollback stellt einen vorher definierten konsistenten Zustand wieder her.

```text
State A
  ↓
Transaction
  ↓
State B
  ↓ Fehler
Rollback
  ↓
State A
```

Rollback muss nur dann als erfolgreich gelten, wenn die Wiederherstellung verifiziert wurde.

```text
Rollback
   ↓
Verification
   ↓
Restored
```

Ein nicht verifizierter Rollback darf nicht als erfolgreich gemeldet werden.

## Compensation

Eine Compensation führt eine definierte Gegenoperation aus.

```text
Operation A
   ↓
Side Effect B
   ↓
Compensation
   ↓
Semantic State C
```

Dabei muss `C` nicht zwingend bitgenau dem Zustand vor der ursprünglichen Operation entsprechen.

Beispiel:

```text
Ressource reservieren
        ↓
Folgeoperation schlägt fehl
        ↓
Reservierung freigeben
```

Die Compensation stellt die gewünschte fachliche Semantik wieder her, nicht notwendigerweise den ursprünglichen internen Zustand.

## Compensation Contract

Jede Operation, die Compensation benötigt, muss deren Semantik explizit definieren können.

Ein Compensation Contract beschreibt mindestens:

- auslösbaren Fehlerzustand,
- betroffene Ressource oder Daten,
- zulässige Gegenoperation,
- Idempotenzverhalten,
- erforderliche Berechtigungen,
- Erfolgskriterium,
- Fehlerverhalten,
- Reconciliation-Verhalten.

```text
Operation
   │
   └── Compensation Contract
          ├── Trigger
          ├── Action
          ├── Authority
          ├── Idempotency
          └── Verification
```

## Reihenfolge bei mehreren Operationen

Bei einer Folge kompensierbarer Operationen erfolgt Compensation grundsätzlich in umgekehrter logischer Reihenfolge.

```text
A → B → C

Fehler

C → B → A
```

Dies gilt nur, wenn die jeweiligen Operationen tatsächlich kompensierbar sind.

Nicht kompensierbare Operationen müssen bereits vor ihrer Ausführung entsprechend gekennzeichnet und behandelt werden.

## Teilweise Compensation

Eine Compensation kann selbst fehlschlagen.

```text
Transaction
    ↓
Failure
    ↓
Compensation
    ↓
Failure
```

NovaOS darf in diesem Fall nicht behaupten, dass die Transaktion vollständig zurückgesetzt wurde.

Der Zustand muss als Recovery- beziehungsweise Reconciliation-Zustand erhalten bleiben.

```text
CompensationFailed
        ↓
RecoveryRequired
        ↓
Reconciliation
```

## Idempotenz

Rollback- und Compensation-Operationen müssen, soweit technisch möglich, idempotent ausführbar sein.

Wiederholung darf dadurch keinen zusätzlichen unerwünschten Seiteneffekt erzeugen.

```text
Compensation(X)
Compensation(X)
Compensation(X)

        ↓

gleicher gewünschter semantischer Zustand
```

Dies ist insbesondere für Crash Recovery und Retry erforderlich.

## Unknown

Wenn nach einem Fehler nicht eindeutig festgestellt werden kann, ob eine Operation bereits ausgeführt wurde, muss der Zustand als `Unknown` behandelt werden.

```text
Unknown
   ↓
nicht blind erneut ausführen
   ↓
Recovery / Verification / Reconciliation
```

`Unknown` darf weder als Erfolg noch als sicherer Fehlschlag interpretiert werden.

## Irreversible Operationen

Irreversible Operationen dürfen nicht mit einem impliziten Rollback-Versprechen versehen werden.

Wenn keine Compensation möglich ist, muss die Operation dies explizit ausweisen.

```text
Operation
   ↓
Irreversible Side Effect
   ↓
kein Rollback
   ↓
kein Compensation Contract
```

Solche Operationen benötigen eine geeignete Transaktions- und Recovery-Semantik, bevor sie Teil einer übergeordneten Transaktion werden können.

## Multi-Service-Transaktionen

Bei mehreren Services darf Compensation nicht als versteckter globaler ACID-Mechanismus behandelt werden.

Jeder Teilnehmer definiert seine eigenen:

- Commit-Semantik,
- Rollback-Möglichkeiten,
- Compensation-Semantik,
- Recovery-Zustände.

```text
Transaction
 ├── Service A → Rollback
 ├── Service B → Compensation
 └── Service C → Reconciliation
```

Die übergeordnete Transaktion muss den Gesamtzustand entsprechend darstellen können.

## Ressourcen

Bei Resource Transactions soll zunächst technischer Rollback verwendet werden, sofern die Ressource vollständig zurückgegeben werden kann.

```text
Reserve
  ↓
Prepare
  ↓
Failure
  ↓
Release / Rollback
```

Wenn eine Ressource bereits einen nicht reversiblen Seiteneffekt erzeugt hat, muss eine Compensation oder Reconciliation erfolgen.

## Konfiguration

Bei Configuration Transactions soll der vorherige validierte Zustand bevorzugt wiederhergestellt werden.

```text
Config A
   ↓
Transaction
   ↓
Config B
   ↓ Fehler
Rollback
   ↓
Config A
```

Kann der vorherige Zustand nicht vollständig wiederhergestellt werden, muss ein definierter Recovery-Zustand entstehen.

## Persistenz

Rollback- und Compensation-Informationen, die für die Wiederherstellung nach einem Neustart erforderlich sind, müssen persistent nachvollziehbar sein.

Der Transaction Log aus `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety` bildet hierfür die Grundlage.

Insbesondere muss nach einem Crash erkennbar sein:

```text
Transaction
   ↓
Operation State
   ↓
Rollback / Compensation Status
   ↓
Recovery Required
```

## Security

Rollback und Compensation benötigen dieselben oder explizit dafür autorisierte Berechtigungen wie die zugehörigen Operationen.

Eine Compensation darf keine Berechtigungsgrenze umgehen.

```text
Original Authority
        ↓
Compensation Authority
        ↓
Policy Check
```

Fehlende oder unklare Berechtigung führt zu `Denied` beziehungsweise `RecoveryRequired`, nicht zu einer stillschweigenden Umgehung.

## Data Sovereignty

Compensation darf keine Datenbewegung oder Datenfreigabe erzeugen, die nach den geltenden Data-Sovereignty-Regeln unzulässig ist.

Die ursprüngliche Datenklassifikation und relevante Policy müssen auch während Recovery und Compensation erhalten bleiben.

## Recovery Verification

Nach Rollback oder Compensation muss der erreichte Zustand verifiziert werden.

```text
Rollback / Compensation
          ↓
Verification
          ↓
      ┌───┴───┐
      ↓       ↓
   Valid    Invalid
      ↓       ↓
  Continue  Recovery
```

Dies folgt dem übergeordneten Prinzip:

```text
Recover → Verify → Restore / Continue
```

## Normative Anforderungen

1. NovaOS MUSS Rollback und Compensation als unterschiedliche Mechanismen behandeln.
2. Rollback MUSS einen definierten vorherigen konsistenten Zustand wiederherstellen.
3. Compensation MUSS eine explizit definierte Gegenoperation darstellen.
4. Eine Compensation DARF nicht als technischer Rollback bezeichnet werden.
5. Rollback und Compensation MÜSSEN nach Möglichkeit idempotent sein.
6. Ein fehlgeschlagener Rollback MUSS als nicht abgeschlossen behandelt werden.
7. Eine fehlgeschlagene Compensation MUSS einen Recovery- oder Reconciliation-Zustand erzeugen.
8. `Unknown` MUSS von `Failed` und `Succeeded` unterschieden werden.
9. Irreversible Operationen DÜRFEN keine implizite Rollback-Garantie besitzen.
10. Multi-Service-Transaktionen MÜSSEN unterschiedliche Rollback-, Compensation- und Reconciliation-Semantiken unterstützen können.
11. Für jede kompensierbare Operation MUSS die Compensation-Semantik explizit definiert sein.
12. Rollback und Compensation MÜSSEN vor Abschluss verifiziert werden.
13. Persistente Transaktionen MÜSSEN ihren Rollback- und Compensation-Fortschritt recoveryfähig protokollieren.
14. Rollback und Compensation MÜSSEN den geltenden Security- und Capability-Regeln unterliegen.
15. Compensation MUSS die Data-Sovereignty-Regeln der betroffenen Daten einhalten.
16. NovaOS DARF nach einer nicht verifizierten Compensation keinen erfolgreichen Gesamtabschluss melden.

## Konsequenzen

### Positive Konsequenzen

- klare Trennung zwischen technischem Rollback und semantischer Compensation,
- sichere Behandlung verteilter Seiteneffekte,
- bessere Crash-Recovery,
- explizite Semantik für nicht vollständig reversible Operationen,
- weniger Gefahr von falschen Erfolgszuständen,
- bessere Grundlage für Multi-Service-Transaktionen.

### Negative Konsequenzen

- Operationen benötigen zusätzliche Compensation-Semantik,
- nicht jede Operation kann vollständig rückgängig gemacht werden,
- Recovery kann länger dauern,
- teilweise kompensierte Zustände müssen explizit behandelt werden.

## Verworfene Alternativen

### Alles per Rollback zurücksetzen

Verworfen, da externe und irreversible Seiteneffekte nicht zuverlässig technisch zurückgesetzt werden können.

### Compensation als versteckter Rollback

Verworfen, da eine Gegenoperation semantisch nicht zwingend den ursprünglichen Zustand wiederherstellt.

### Blindes Wiederholen nach fehlgeschlagener Compensation

Verworfen, da dadurch zusätzliche Seiteneffekte entstehen können.

### Globales ACID über alle Services

Verworfen, da NovaOS unterschiedliche lokale Commit- und Recovery-Semantiken unterstützen muss.

## Abhängigkeiten

- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-TRANSACTION-0004_Resource_Transactions`
- `ADR-TRANSACTION-0005_Multi-Service_Transactions`
- `ADR-TRANSACTION-0006_Transaction_Log_und_Power_Failure_Safety`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0005_Retry_nur_nach_expliziter_Operationssemantik`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-CONTRACT-0001`
- `NPSPEC-TRANSACTION-RECONCILIATION-0001`
- `NPSPEC-TRANSACTION-RECOVERY-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-COMPENSATION-TEST-0001`

## Ergebnis

NovaOS verwendet Rollback und Compensation als zwei klar getrennte Transaktionsmechanismen.

```text
Rollback
   ↓
technische Wiederherstellung

Compensation
   ↓
semantische Gegenoperation

beide
   ↓
Verification
   ↓
konsistenter Recovery-Zustand
```

Rollback ist der bevorzugte Mechanismus für vollständig reversible Zustandsänderungen. Compensation ermöglicht dagegen kontrollierte Recovery für Operationen, deren Seiteneffekte technisch nicht vollständig rückgängig gemacht werden können.

Ein nicht verifizierter Rollback oder eine nicht verifizierte Compensation gilt niemals als erfolgreich abgeschlossen.