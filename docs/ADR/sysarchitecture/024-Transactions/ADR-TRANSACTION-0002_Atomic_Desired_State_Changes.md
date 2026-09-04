# ADR-TRANSACTION-0002 – Atomic Desired State Changes

## Status

Angenommen

## Kategorie

Transaction Architecture / Declarative System Model / Atomic State Change

## Kontext

NovaOS beschreibt wesentliche Teile seines Systemzustands deklarativ.

Eine Änderung am Desired State kann mehrere zusammengehörige Eigenschaften betreffen, beispielsweise:

- Services,
- Capability Provider,
- Konfiguration,
- Ressourcen,
- Policies,
- Storage,
- Abhängigkeiten,
- Routing,
- Systemkomponenten.

Werden solche Änderungen einzeln sichtbar, kann vorübergehend ein Zustand entstehen, der weder dem alten noch dem neuen Desired State entspricht.

Beispiel:

```text
Desired State A
      ↓
Change Service
      ↓
Change Provider
      ↓
Change Policy
      ↓
Desired State B
```

Andere Komponenten könnten währenddessen einen inkonsistenten Zwischenzustand beobachten.

## Entscheidung

Änderungen am deklarativen Desired State werden in NovaOS grundsätzlich als **atomare System Transactions** behandelt.

```text
Current Desired State
        ↓
Proposed State Change
        ↓
System Transaction
        ↓
Validate
        ↓
Atomic Commit
        ↓
New Desired State
```

Zusammengehörige Änderungen werden erst nach erfolgreicher Validierung gemeinsam als neuer Desired State sichtbar.

## Grundprinzip

```text
Desired State changes become visible
as one logical transition.
```

Ein Consumer soll nicht gezwungen sein, temporäre Zwischenzustände einer noch nicht vollständig angewendeten Änderung zu interpretieren.

## Change Set

Eine Desired-State-Änderung wird als zusammengehöriges Change Set beschrieben.

Beispiel:

```text
Change Set
 ├── Replace Provider A with B
 ├── Update Service Configuration
 ├── Update Resource Budget
 └── Update Routing
```

Das Change Set bildet die semantische Einheit der Transaction.

## Prepare und Validate

Vor dem Commit können notwendige Ressourcen und Komponenten vorbereitet werden.

```text
Proposed Desired State
        ↓
Resolve Dependencies
        ↓
Prepare Resources
        ↓
Validate Constraints
```

Dabei können unter anderem geprüft werden:

```text
Dependencies
Capabilities
Resource Budgets
Security Policies
Trust Requirements
Data Sovereignty
Schema Constraints
System Invariants
```

Ein ungültiger Desired State darf nicht committed werden.

## Atomic Commit

Nach erfolgreicher Vorbereitung wird der neue Desired State als eine logische Zustandsänderung aktiviert.

```text
Desired State Version N
        ↓
Atomic Commit
        ↓
Desired State Version N+1
```

Komponenten müssen eindeutig feststellen können, zu welcher Desired-State-Version ihre Beobachtung gehört.

## Observed State

Der atomare Commit des Desired State bedeutet nicht, dass sämtliche physischen Änderungen im selben CPU-Zeitpunkt abgeschlossen sein müssen.

Nach dem Commit beginnt die kontrollierte Konvergenz:

```text
Desired State N+1
       ↓
Reconciliation
       ↓
Observed State
       ↓
Converged State
```

Die System Transaction atomisiert die **deklarative Entscheidung**, nicht zwangsläufig jeden physischen Effekt.

## Reconciliation

Nach Commit sorgt das System dafür, dass der Observed State zum neuen Desired State konvergiert.

Kann eine notwendige Änderung nicht hergestellt werden, wird dies als Transaction- beziehungsweise Resilience-Fehler behandelt.

```text
Committed Desired State
        ↓
Reconciliation
        ↓
Failure
        ↓
Recover / Rollback / Compensate
```

Die konkrete Strategie hängt von Operationssemantik und Recovery Contract ab.

## Versionierung

Desired States müssen versionierbar sein.

```text
State 41
State 42
State 43
```

Eine Transaction basiert auf einer bekannten Ausgangsversion.

Dadurch können konkurrierende Änderungen erkannt werden.

## Concurrent Changes

Zwei inkompatible Änderungen dürfen nicht stillschweigend gegenseitig überschrieben werden.

```text
Transaction A → based on State 42
Transaction B → based on State 42

A commits → State 43

B commit
   ↓
Conflict Detection
```

NovaOS muss solche Konflikte erkennen und je nach Policy neu planen, zusammenführen oder ablehnen können.

## Rollback

Kann der neue Desired State nach Commit nicht ausreichend realisiert oder verifiziert werden, kann ein Rollback auf einen vorherigen gültigen Desired State erforderlich sein.

```text
State N
   ↓
Commit State N+1
   ↓
Reconciliation fails
   ↓
Rollback Decision
   ↓
State N
```

Rollback darf nur erfolgen, wenn die zugrunde liegenden physischen und semantischen Änderungen dies zulassen.

Andernfalls muss Compensation oder Reconciliation verwendet werden.

## Irreversible Änderungen

Nicht jede Desired-State-Änderung ist vollständig reversibel.

Beispiele:

```text
External Side Effect
Credential Destruction
Published Data
Physical Device Action
Irreversible Migration
```

Solche Änderungen müssen explizit als nicht vollständig rollbackfähig gekennzeichnet werden.

Der atomare Desired-State-Commit darf keine falsche Garantie über physische Rücksetzbarkeit erzeugen.

## Capability Provider

Desired-State-Änderungen können Provider-Wechsel enthalten.

```text
Desired State
   ↓
Provider A → Provider B
```

Der neue Provider muss vor Commit beziehungsweise Aktivierung alle relevanten Contracts erfüllen können.

Dazu gehören insbesondere:

```text
Semantic Compatibility
Trust Requirements
Resource Requirements
Data Sovereignty
Security Constraints
Execution Contract
```

## Security und Authority

Eine atomare Desired-State-Änderung erzeugt keine zusätzliche Authority.

Alle enthaltenen Änderungen müssen einzeln und als Gesamttransaktion autorisiert sein.

```text
Authorized Change Set
        +
Valid System Transaction
        ↓
Permitted Commit
```

Ein einzelner autorisierter Teil darf nicht automatisch den gesamten Change Set legitimieren.

## Resource Reservations

Eine geplante Desired-State-Änderung kann Ressourcen benötigen, bevor der alte Zustand aufgegeben wird.

Beispiel:

```text
Old Provider active
       +
New Provider prepared
```

NovaOS muss solche temporären Ressourcenanforderungen mit der systemweiten Ressourcenökonomie koordinieren.

Ein Commit darf nicht erfolgen, wenn der neue Zustand seine notwendigen Ressourcen offensichtlich nicht erhalten kann.

## Trust und Data Sovereignty

Desired-State-Änderungen müssen aktuelle Trust- und Data-Sovereignty-Regeln erfüllen.

Ein neuer Provider, Speicherort oder Ausführungsort darf nicht allein deshalb aktiviert werden, weil er funktional kompatibel ist.

```text
Candidate State
      ↓
Trust + Sovereignty Validation
      ↓
Eligible State
```

## Recovery

Unterbrochene oder unvollständig konvergierte Desired-State-Transactions müssen durch Nova.Resilience wieder aufgenommen oder wiederhergestellt werden können.

```text
Transaction Interrupted
        ↓
Determine Desired State Version
        ↓
Determine Observed State
        ↓
Reconcile / Rollback / Compensate
```

Der deklarative Desired State bleibt dabei die Referenz für den angestrebten gültigen Zustand.

## Offline Recovery

Ist eine sichere Online-Reconciliation nicht mehr möglich, kann NovaDOS den persistenten Transaction- und Desired-State-Kontext verwenden.

```text
Interrupted Desired-State Change
        ↓
NovaDOS
        ↓
Inspect State Versions
        ↓
Repair / Rollback / Reconcile
```

Der gespeicherte Zustand muss vor Verwendung validiert werden.

## Normative Anforderungen

1. NovaOS MUSS Desired-State-Änderungen als atomare System Transactions unterstützen.
2. Zusammengehörige Desired-State-Änderungen MÜSSEN als gemeinsames Change Set behandelbar sein.
3. Ein neuer Desired State DARF erst nach erfolgreicher Validierung sichtbar werden.
4. Desired States MÜSSEN versionierbar sein.
5. Transactions MÜSSEN auf einer bekannten Ausgangsversion basieren können.
6. Konkurrierende inkompatible Änderungen MÜSSEN erkannt werden.
7. Desired-State-Commit und physische Konvergenz MÜSSEN semantisch getrennt bleiben.
8. Der Commit MUSS eine atomare deklarative Zustandsänderung darstellen.
9. Der Observed State MUSS nach Commit zum neuen Desired State reconciliiert werden können.
10. Fehlgeschlagene Reconciliation MUSS Recovery, Rollback oder Compensation ermöglichen.
11. Irreversible Änderungen MÜSSEN explizit als solche deklarierbar sein.
12. Atomarer Desired-State-Commit DARF keine physische globale Rollback-Garantie vortäuschen.
13. Provider-Wechsel MÜSSEN vor Aktivierung auf Contract-Kompatibilität geprüft werden.
14. Alle Bestandteile eines Change Sets MÜSSEN die erforderliche Authority besitzen.
15. Eine Transaction DARF keine zusätzliche Authority erzeugen.
16. Temporäre Resource Reservations MÜSSEN durch die systemweite Ressourcenökonomie begrenzt werden können.
17. Trust-, Security-, Integrity- und Data-Sovereignty-Hard-Constraints MÜSSEN vor Commit gelten.
18. Unterbrochene Desired-State-Transactions MÜSSEN durch Nova.Resilience recoverbar sein.
19. Offline Recovery MUSS den persistenten Desired-State- und Transaction-Kontext verwenden können.
20. Change Set, Ausgangsversion, Commit, Reconciliation und Recovery MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- deklarative Systemänderungen werden konsistent sichtbar,
- Zwischenzustände werden für Consumer vermieden,
- konkurrierende Änderungen können erkannt werden,
- Desired State und physische Konvergenz bleiben sauber getrennt,
- Recovery kann auf versioniertem Systemzustand aufbauen,
- Provider-, Security- und Sovereignty-Wechsel werden vor Commit geprüft.

### Negative Konsequenzen

- Desired-State-Versionen und Change Sets müssen verwaltet werden,
- Reconciliation kann nach dem atomaren Commit weiterhin Zeit benötigen,
- irreversible Änderungen benötigen zusätzliche Compensation- oder Recovery-Semantik.

## Verworfene Alternativen

### Desired State direkt Eigenschaft für Eigenschaft verändern

Verworfen, da dadurch inkonsistente Zwischenzustände sichtbar werden können.

### Commit erst nach vollständiger physischer Konvergenz

Verworfen, da verteilte oder langlaufende Systemänderungen nicht sinnvoll als ein einzelner physischer Atomaritätsmoment behandelt werden können.

### Letzte Änderung gewinnt bei Konflikten

Verworfen, da dadurch inkompatible Desired-State-Änderungen unbemerkt überschrieben werden könnten.

### Vollständiger physischer Rollback für jede Desired-State-Änderung

Verworfen, da externe und irreversible Effekte nicht generell rückgängig gemacht werden können.

## Abhängigkeiten

- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-ARCH-0002_Declarative_System_Model`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0022_NovaDOS_für_Offline_Recovery`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-TRANSACTION-DESIRED-STATE-0001`
- `NPSPEC-TRANSACTION-DESIRED-STATE-CHANGESET-0001`
- `NPSPEC-TRANSACTION-DESIRED-STATE-VERSION-0001`
- `NPSPEC-TRANSACTION-DESIRED-STATE-CONFLICT-0001`
- `NPSPEC-TRANSACTION-DESIRED-STATE-COMMIT-0001`
- `NPSPEC-TRANSACTION-DESIRED-STATE-RECONCILIATION-0001`
- `NPSPEC-TRANSACTION-DESIRED-STATE-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-DESIRED-STATE-TEST-0001`

## Ergebnis

NovaOS behandelt Änderungen des deklarativen Systemmodells als atomare, versionierte Desired-State-Transitions:

```text
Desired State N
      ↓
Change Set
      ↓
Prepare + Validate
      ↓
Atomic Commit
      ↓
Desired State N+1
      ↓
Reconciliation
      ↓
Observed State
      ↓
Verification
```

Damit sehen Systemkomponenten entweder den alten oder den neuen deklarativen Desired State, während die tatsächliche physische Systemkonvergenz kontrolliert, recoverbar und nachvollziehbar im Hintergrund erfolgt.