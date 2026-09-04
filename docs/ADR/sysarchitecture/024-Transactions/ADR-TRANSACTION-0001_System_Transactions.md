# ADR-TRANSACTION-0001 – System Transactions

## Status

Angenommen

## Kategorie

Transaction Architecture / System State / Atomic Operations

## Kontext

NovaOS führt viele Operationen aus, die mehrere Systemkomponenten oder Zustände betreffen.

Beispiele sind:

- Installation oder Aktualisierung von Software,
- Änderungen an Systemkonfiguration,
- Capability Registration,
- Identity- und Credential-Änderungen,
- Storage-Operationen,
- Driver Replacement,
- Service Migration,
- Recovery-Aktionen,
- Änderungen an deklarativen Systemzuständen.

Werden solche Änderungen nur schrittweise und ohne gemeinsames Transaktionsmodell durchgeführt, kann ein Fehler zu inkonsistentem Zwischenzustand führen.

NovaOS benötigt deshalb ein systemweites Modell für kontrollierte, atomare und nachvollziehbare Zustandsänderungen.

## Entscheidung

NovaOS führt **System Transactions** als gemeinsame Abstraktion für zusammengehörige Systemänderungen ein.

```text
Current State
     ↓
System Transaction
     ↓
Planned Changes
     ↓
Validation
     ↓
Commit
     ↓
New Valid State
```

Schlägt die Transaktion vor erfolgreichem Commit fehl, muss sie je nach Operationssemantik zurückgerollt, kompensiert oder anderweitig in einen definierten gültigen Zustand überführt werden.

## Grundprinzip

```text
Related system changes
should succeed or fail coherently.
```

Eine System Transaction ist dabei mehr als eine Storage-Transaktion.

Sie kann mehrere Subsysteme unter einem gemeinsamen semantischen Commit-Modell koordinieren.

## Transaction Scope

Eine System Transaction besitzt einen expliziten Scope.

Beispiele:

```text
Configuration
Application Installation
Service Update
Capability Registration
Driver Replacement
Identity Change
Recovery Operation
Subsystem Reconfiguration
```

Der Scope muss so klein wie möglich, aber groß genug gewählt werden, um die relevanten Invarianten gemeinsam zu erhalten.

## Transaction Phasen

Das grundlegende Modell lautet:

```text
Begin
  ↓
Prepare
  ↓
Validate
  ↓
Commit
  ↓
Verify
```

Bei Fehlern vor Commit:

```text
Abort / Rollback
```

Bei Fehlern nach nicht vollständig reversiblen Änderungen:

```text
Compensation / Reconciliation
```

Die konkrete technische Umsetzung kann je nach Subsystem variieren.

## Prepare

Während `Prepare` werden notwendige Änderungen geplant und Ressourcen reserviert.

Noch nicht sichtbare Änderungen sollen nach Möglichkeit außerhalb des aktiven Systemzustands vorbereitet werden.

Beispiele:

```text
Stage Files
Prepare New Configuration
Load Replacement Provider
Validate Dependencies
Reserve Resources
```

## Validate

Vor Commit müssen relevante Bedingungen geprüft werden.

Dazu können gehören:

```text
Schema Validity
Dependency Constraints
Capabilities
Security Policies
Trust Requirements
Resource Budgets
Data Sovereignty
State Invariants
```

Eine Transaktion darf nicht committed werden, wenn notwendige Hard Constraints verletzt sind.

## Commit

`Commit` bildet den semantischen Punkt, an dem der neue Zustand als gültiger Systemzustand übernommen wird.

```text
Prepared State
     ↓
Commit
     ↓
Visible System State
```

Der Commit muss für den jeweiligen Transaction Scope eindeutig definiert sein.

## Verification

Nach Commit kann eine zusätzliche Verification erforderlich sein.

```text
Commit
  ↓
Verify
  ↓
Valid?
 ├── Yes → Complete
 └── No  → Recovery
```

Ein technisch erfolgreicher Commit bedeutet nicht automatisch, dass der neue Zustand vollständig funktionsfähig ist.

## Rollback

Vor Commit sollen Änderungen, soweit möglich, vollständig zurückgesetzt werden können.

```text
Prepare
   ↓
Failure
   ↓
Rollback
   ↓
Previous Valid State
```

Rollback ist jedoch nicht für jede Operation technisch möglich.

## Compensation

Bei bereits ausgeführten irreversiblen oder externen Side Effects kann statt technischem Rollback eine semantische Compensation erforderlich sein.

```text
Committed Effect
      ↓
Later Failure
      ↓
Compensation
```

Compensation muss explizit durch die Operationssemantik beschrieben werden.

## Reconciliation

Bei unbekanntem oder verteiltem Zustand kann NovaOS zunächst den tatsächlichen Zustand feststellen müssen.

```text
Unknown State
     ↓
Reconciliation
     ↓
Observed State
     ↓
Recovery Decision
```

Dies gilt insbesondere über Prozess-, Netzwerk- oder Provider-Grenzen hinweg.

## Nested Transactions

System Transactions dürfen hierarchisch verschachtelt sein.

```text
Parent Transaction
    ├── Child A
    ├── Child B
    └── Child C
```

Untergeordnete Transaktionen dürfen den Commit der übergeordneten Transaktion nicht unkontrolliert umgehen.

Die konkrete Commit- und Failure-Semantik verschachtelter Transaktionen wird in NPSPECs festgelegt.

## Transaction Identity

Jede relevante System Transaction erhält eine eindeutige Transaction Identity.

Diese dient unter anderem für:

```text
Tracing
Deduplication
Recovery
Audit
Reconciliation
```

Transaction Identity ist von Process ID oder Workload Identity getrennt.

## Transactions und Declarative System Model

System Transactions sind der primäre Mechanismus, um Änderungen am deklarativen Systemzustand kontrolliert anzuwenden.

```text
Desired State Change
        ↓
System Transaction
        ↓
Validate
        ↓
Commit
        ↓
Observed State converges
```

Damit werden deklarative Änderungen nicht als unkoordinierte Folge einzelner Mutationen ausgeführt.

## Capability Integration

Capability-basierte Operationen können Teil einer System Transaction sein.

```text
Transaction
   ├── Capability A
   ├── Capability B
   └── Capability C
```

Alle beteiligten Provider müssen ihre Transaction- und Recovery-Semantik explizit deklarieren können.

## Authority

Eine System Transaction erzeugt keine zusätzliche Authority.

```text
Transaction
≠
Privilege Escalation
```

Jede beteiligte Operation benötigt weiterhin die erforderlichen Capabilities und muss relevante Policies erfüllen.

## Resource Economy

Transaktionen können Ressourcen reservieren oder temporär zusätzlichen Zustand halten.

Deshalb müssen sie bestehende Resource Budgets berücksichtigen.

Lang laufende Transaktionen dürfen Ressourcen nicht unbegrenzt blockieren.

## Failure und Recovery

Unterbrochene System Transactions müssen durch Nova.Resilience oder NovaDOS recoverbar sein.

```text
Transaction Interrupted
        ↓
Inspect Transaction State
        ↓
Commit / Rollback / Compensate / Reconcile
```

Ein unbekannter Commit-Zustand darf nicht blind als nicht ausgeführt interpretiert werden.

## Persistenz

Für kritische System Transactions muss der notwendige Transaktionszustand persistent gespeichert werden können.

Damit kann nach Crash oder Neustart festgestellt werden:

```text
Prepared
Committed
Rolled Back
Compensating
Unknown
```

Die konkrete Journal- oder Log-Struktur wird in NPSPECs definiert.

## Distributed Transactions

NovaOS darf nicht voraussetzen, dass alle systemübergreifenden Transaktionen global atomar commitbar sind.

Bei verteilten oder externen Komponenten können unterschiedliche Mechanismen erforderlich sein:

```text
Atomic Commit
Local Transaction
Compensation
Idempotency
Reconciliation
```

Die Operationssemantik bestimmt die zulässige Strategie.

## Normative Anforderungen

1. NovaOS MUSS System Transactions als systemweite Abstraktion für zusammengehörige Zustandsänderungen unterstützen.
2. System Transactions MÜSSEN einen expliziten Transaction Scope besitzen.
3. Transaction Scope SOLL so klein wie möglich und so groß wie für die relevanten Invarianten notwendig sein.
4. NovaOS MUSS `Prepare`, `Validate`, `Commit` und `Verify` semantisch unterscheiden können.
5. Hard Constraints MÜSSEN vor Commit geprüft werden können.
6. Ein Commit MUSS einen eindeutig definierten semantischen Zustandsübergang darstellen.
7. Vor Commit SOLLEN reversible Änderungen vollständig rollbackfähig sein.
8. Nicht vollständig reversible Operationen MÜSSEN Compensation oder Reconciliation deklarieren können.
9. Ein unbekannter Commit-Zustand DARF nicht automatisch als nicht ausgeführt interpretiert werden.
10. System Transactions MÜSSEN eine eindeutige Transaction Identity besitzen können.
11. Verschachtelte Transaktionen MÜSSEN kontrollierte Parent-Child-Semantik besitzen.
12. System Transactions MÜSSEN mit dem Declarative System Model integrierbar sein.
13. Capability Provider MÜSSEN ihre relevante Transaction- und Recovery-Semantik deklarieren können.
14. System Transactions DÜRFEN keine zusätzliche Authority erzeugen.
15. Transaktionen MÜSSEN Resource Budgets und Deadlines berücksichtigen können.
16. Kritische Transaction States MÜSSEN über Crash oder Neustart hinweg rekonstruierbar sein können.
17. Unterbrochene Transactions MÜSSEN durch Nova.Resilience oder NovaDOS recoverbar sein.
18. Verteilte Transactions DÜRFEN nicht pauschal globale Atomarität voraussetzen.
19. Security-, Trust-, Integrity- und Data-Sovereignty-Hard-Constraints MÜSSEN auch innerhalb einer Transaction gelten.
20. Transaction Lifecycle, Commit, Rollback, Compensation und Recovery MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- zusammengehörige Systemänderungen bleiben konsistent,
- deklarative Änderungen erhalten ein einheitliches Commit-Modell,
- Crash Recovery kann auf explizitem Transaction State aufbauen,
- Rollback, Compensation und Reconciliation werden sauber unterschieden,
- Systemupdates und Recovery können dieselbe Transaktionsarchitektur verwenden.

### Negative Konsequenzen

- beteiligte Komponenten müssen Transaction-Semantik deklarieren,
- nicht reversible und verteilte Operationen benötigen zusätzliche Recovery-Logik,
- persistenter Transaction State erzeugt zusätzlichen Verwaltungsaufwand.

## Verworfene Alternativen

### Jede Systemänderung einzeln ausführen

Verworfen, da zusammengehörige Zustandsänderungen dadurch inkonsistent werden können.

### Globale ACID-Transaktion über das gesamte System

Verworfen, da externe Geräte, Remote Services und irreversible Side Effects nicht generell global atomar kontrollierbar sind.

### Rollback als einzige Fehlerbehandlung

Verworfen, da bestimmte Operationen nur kompensiert oder reconciliert werden können.

### Transaction Authority als administratives Sonderrecht

Verworfen, da Transaktionen keine bestehenden Capability- und Security-Grenzen umgehen dürfen.

## Abhängigkeiten

- `ADR-ARCH-0002_Declarative_System_Model`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0009_Semantic_Checkpointing`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`
- `ADR-CAPABILITY-0001_Capabilities_als_primäre_Systemfähigkeiten`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`

## Zugehörige NPSPECs

- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-LIFECYCLE-0001`
- `NPSPEC-TRANSACTION-IDENTITY-0001`
- `NPSPEC-TRANSACTION-COMMIT-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-TRANSACTION-RECONCILIATION-0001`
- `NPSPEC-TRANSACTION-PERSISTENCE-0001`
- `NPSPEC-TRANSACTION-NESTED-0001`
- `NPSPEC-TRANSACTION-RECOVERY-0001`
- `NPSPEC-TRANSACTION-TEST-0001`

## Ergebnis

NovaOS behandelt zusammengehörige Systemänderungen als explizite System Transactions:

```text
Desired Change
      ↓
Begin Transaction
      ↓
Prepare
      ↓
Validate
      ↓
Commit
      ↓
Verify
   ┌──┴───┐
 Valid   Invalid
   ↓       ↓
Complete  Recovery
           ├── Rollback
           ├── Compensation
           └── Reconciliation
```

Damit erhält NovaOS ein einheitliches Transaktionsmodell für Systemzustände, das lokale Atomarität ermöglicht, aber zugleich realistisch mit verteilten, externen und nicht vollständig reversiblen Operationen umgehen kann.