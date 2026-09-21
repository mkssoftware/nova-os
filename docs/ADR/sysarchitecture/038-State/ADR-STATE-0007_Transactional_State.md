# ADR-STATE-0007 – Transactional State

## Status

Angenommen

## Kategorie

State Management / Transactions / Consistency

## Kontext

Viele Zustandsänderungen in NovaOS betreffen mehrere voneinander abhängige Objekte.

Beispiel:

```text
Service stoppen
Konfiguration ändern
Ressource wechseln
Service starten
```

Werden nur einzelne Schritte ausgeführt, kann ein inkonsistenter Zwischenzustand entstehen.

## Entscheidung

NovaOS unterstützt **Transactional State Changes** für Zustandsänderungen, die gemeinsam konsistent ausgeführt werden müssen.

```text
Current State
     ↓
Prepare
     ↓
Validate
     ↓
Commit
     ↓
New State
```

Entweder wird der definierte Zustandswechsel vollständig übernommen oder der vorherige konsistente Zustand bleibt erhalten bzw. wird kontrolliert wiederhergestellt.

## Transaction Scope

Eine State Transaction definiert explizit:

```text
Affected Nodes
Expected Versions
Desired Changes
Constraints
Commit Conditions
```

Der Scope soll so klein wie möglich gehalten werden.

## Prepare

Vor dem Commit werden die geplanten Änderungen geprüft.

Dazu gehören:

```text
Authorization
State Versions
State Machine Transitions
Resource Availability
Hard Constraints
```

Erst wenn die notwendigen Voraussetzungen erfüllt sind, darf der Commit beginnen.

## Commit

Der Commit erzeugt einen neuen konsistenten Systemzustand.

```text
State Generation N
        ↓
Transaction
        ↓
State Generation N+1
```

Teilweise sichtbare Zwischenzustände sollen vermieden werden.

## Konflikte

Ändert sich ein beteiligter Zustand zwischen Planung und Commit, muss die Transaktion den Konflikt erkennen.

```text
Expected Version: 12
Current Version:  13
        ↓
Conflict
```

Die Transaktion wird dann neu geplant oder abgebrochen.

## Failure

Schlägt eine Transaktion fehl, muss NovaOS zwischen folgenden Zuständen unterscheiden können:

```text
Not Committed
Committed
Rolled Back
Unknown
```

`Unknown` darf nicht automatisch als erfolgreich oder fehlgeschlagen interpretiert werden.

## Irreversible Operations

Nicht jede Operation ist vollständig rückgängig machbar.

Solche Operationen müssen explizit gekennzeichnet und möglichst an eine Commit-Grenze verschoben werden.

Falls Rollback nicht möglich ist, können definierte Compensation- oder Recovery-Mechanismen verwendet werden.

## Reconciliation

State Reconciliation kann Transaktionen verwenden, wenn mehrere Änderungen gemeinsam erforderlich sind.

```text
Desired State
      ↓
Reconciliation Plan
      ↓
State Transaction
      ↓
Verified Actual State
```

Nach dem Commit wird der Actual State erneut verifiziert.

## Normative Anforderungen

1. NovaOS MUSS transaktionale State Changes unterstützen.
2. Eine State Transaction MUSS ihren Scope explizit definieren.
3. Betroffene State-Versionen MÜSSEN vor dem Commit validierbar sein.
4. Ungültige State-Machine-Transitions DÜRFEN nicht committed werden.
5. Hard Constraints MÜSSEN vor dem Commit geprüft werden.
6. Konflikte durch zwischenzeitliche State Changes MÜSSEN erkannt werden.
7. Teilweise sichtbare Zustandsänderungen SOLLEN vermieden werden.
8. `Unknown` MUSS als eigener Transaction State behandelt werden.
9. Irreversible Operationen MÜSSEN explizit behandelt werden.
10. Nach einem Commit MUSS der resultierende Actual State verifizierbar sein.

## Konsequenzen

### Positiv

- konsistente Mehrfachänderungen,
- weniger fehlerhafte Zwischenzustände,
- sichere Grundlage für Reconciliation und Self-Healing,
- State Versioning kann Konflikte zuverlässig erkennen.

### Negativ

- Transaktionen erhöhen die Komplexität,
- irreversible und verteilte Operationen benötigen zusätzliche Behandlung.

## Abhängigkeiten

- `ADR-STATE-0002_Desired_und_Actual_State`
- `ADR-STATE-0003_State_Machines`
- `ADR-STATE-0004_State_Reconciliation`
- `ADR-STATE-0006_State_Versioning_und_History`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-ARCH-0014_Explizite_Contracts`

## Ergebnis

NovaOS behandelt zusammengehörige Zustandsänderungen als konsistente Einheit:

```text
Validate
   ↓
Prepare
   ↓
Commit
   ↓
Verify
```

Damit wird verhindert, dass komplexe Systemänderungen das System in einem unbeabsichtigten Teilzustand zurücklassen.