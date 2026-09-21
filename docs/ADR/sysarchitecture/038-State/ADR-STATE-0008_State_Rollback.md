# ADR-STATE-0008 – State Rollback

## Status

Angenommen

## Kategorie

State Management / Recovery / Transactions

## Kontext

Fehlerhafte Änderungen können einen zuvor funktionierenden Systemzustand beeinträchtigen.

Durch State Versioning, Snapshots und Transactional State besitzt NovaOS die notwendigen Informationen, um auf einen früheren bekannten Zustand zurückzugehen.

Rollback darf jedoch nicht als blindes Zurückkopieren alter Zustände verstanden werden.

## Entscheidung

NovaOS unterstützt **State Rollback** als kontrollierte Wiederherstellung eines früheren gültigen Zustands.

```text
Current State
     ↓
Rollback Target
     ↓
Validate
     ↓
Restore
     ↓
Verify
```

## Rollback Target

Ein Rollback-Ziel wird über eine bekannte State-Version oder einen Snapshot bestimmt.

```text
State v42
State v43
State v44 ← Current
    ↓
Rollback to v42
```

Das Ziel muss eindeutig identifizierbar sein.

## Validierung

Vor einem Rollback prüft NovaOS:

```text
Target State
Current Constraints
Authorization
Dependencies
Available Versions
```

Ein historisch gültiger Zustand ist nicht automatisch unter den aktuellen Bedingungen wieder gültig.

## Rollback und State Machines

Rollback umgeht keine State Machines.

Falls ein direkter Rücksprung nicht zulässig ist, muss NovaOS einen gültigen Übergangspfad oder einen speziellen Recovery-Pfad verwenden.

## Daten und Ressourcen

State Rollback bedeutet nicht automatisch Rollback aller Nutzdaten.

```text
System State Rollback
≠
Object Data Rollback
≠
Storage Rollback
```

Benötigte Daten-, Konfigurations- oder Storage-Versionen müssen explizit Bestandteil des Recovery-Plans sein.

## Irreversible Änderungen

Nicht jede Zustandsänderung ist rückgängig machbar.

Beispiele:

```text
External Message
Physical Device Action
Remote Side Effect
Secure Erase
```

Solche Änderungen benötigen gegebenenfalls Compensation statt Rollback.

## Transaktionen

Fehlgeschlagene State Transactions können Rollback verwenden, um einen vorherigen konsistenten Zustand wiederherzustellen.

```text
Transaction Failure
       ↓
Rollback
       ↓
Previous Consistent State
```

## Verifikation

Nach einem Rollback muss NovaOS den tatsächlichen Zustand erneut prüfen.

```text
Rollback
   ↓
Observe
   ↓
Verify
```

Erst danach gilt der Rollback als erfolgreich.

## Normative Anforderungen

1. NovaOS MUSS State Rollback auf bekannte State-Versionen oder Snapshots unterstützen können.
2. Rollback-Ziele MÜSSEN eindeutig identifizierbar sein.
3. Rollback MUSS aktuelle Authorization und Hard Constraints respektieren.
4. State Machines DÜRFEN durch Rollback nicht unkontrolliert umgangen werden.
5. System State Rollback DARF nicht automatisch als Daten- oder Storage-Rollback behandelt werden.
6. Irreversible Änderungen MÜSSEN explizit erkannt werden.
7. Nicht rückrollbare Änderungen SOLLEN Compensation oder Recovery verwenden.
8. Der resultierende Actual State MUSS nach dem Rollback verifiziert werden.
9. Ein fehlgeschlagener Rollback MUSS als eigener Fehlerzustand erkennbar sein.

## Konsequenzen

### Positiv

- fehlerhafte Systemänderungen können kontrolliert zurückgenommen werden,
- State History und Snapshots werden direkt für Recovery nutzbar,
- Transaction Failures können auf bekannte Zustände zurückgeführt werden,
- Self-Healing erhält einen definierten Rollback-Mechanismus.

### Negativ

- nicht alle Änderungen sind reversibel,
- historische Zustände können unter aktuellen Bedingungen ungültig sein,
- Daten- und System-State-Rollback müssen getrennt koordiniert werden.

## Abhängigkeiten

- `ADR-STATE-0003_State_Machines`
- `ADR-STATE-0004_State_Reconciliation`
- `ADR-STATE-0005_State_Snapshots`
- `ADR-STATE-0006_State_Versioning_und_History`
- `ADR-STATE-0007_Transactional_State`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-AUTONOMY-0005_Self_Healing`

## Ergebnis

NovaOS kann auf bekannte Zustände zurückkehren, behandelt Rollback jedoch als kontrollierte Systemoperation:

```text
Known Good State
      ↓
Validate
      ↓
Rollback
      ↓
Verify
```

Ein früherer Zustand wird nicht blind wiederhergestellt, sondern nur dann, wenn er unter den aktuellen Systemregeln weiterhin gültig ist.