# ADR-RESILIENCE-0010 – Rollback über Systemgrenzen

## Status

Angenommen

## Kategorie

Resilience Architecture / Rollback / Distributed Recovery

## Kontext

NovaOS-Operationen können mehrere Komponenten, Dienste, Capability Provider, Geräte oder entfernte Systeme betreffen.

Ein klassischer lokaler Rollback reicht in solchen Fällen nicht aus. Eine Operation kann beispielsweise lokalen Zustand verändert haben, während gleichzeitig ein Remote-Service, ein Storage Provider oder ein Gerät einen eigenen Side Effect ausgeführt hat.

```text
Operation
   ├── Local State
   ├── Storage
   ├── Remote Service
   └── Device
```

Ein vollständiges Zurücksetzen aller Beteiligten ist nicht immer technisch möglich.

Nova.Resilience benötigt deshalb ein definiertes Modell für Recovery und Rollback über Systemgrenzen hinweg.

## Entscheidung

NovaOS unterstützt **Rollback über Systemgrenzen als koordinierte semantische Recovery**.

Dabei wird nicht vorausgesetzt, dass jede beteiligte Operation technisch rückgängig gemacht werden kann.

```text
Distributed Operation
        ↓
Partial Failure
        ↓
Recovery Plan
        ↓
Rollback / Compensation / Reconciliation
        ↓
Verify
```

Das Ziel ist die Wiederherstellung eines definierten gültigen Gesamtzustands.

## Grundprinzip

```text
Rollback
≠
Undo every side effect
```

Stattdessen gilt:

```text
Failure
   ↓
Restore valid system semantics
```

## Rollback-Grenzen

Jede beteiligte Operation muss ihre Recovery-Eigenschaften explizit beschreiben können.

Beispiele:

```text
Reversible
Transactional
Compensatable
Reconstructable
Irreversible
Externally Reconciled
```

Nova.Resilience verwendet diese Semantik bei der Erstellung des Recovery Plans.

## Lokaler Rollback

Ist eine Änderung vollständig lokal und transaktional, kann ein klassischer Rollback verwendet werden.

```text
Begin
  ↓
Change
  ↓ failure
Rollback
  ↓
Previous State
```

## Kompensation

Kann ein bereits ausgeführter Side Effect nicht technisch zurückgesetzt werden, kann eine semantische Gegenoperation verwendet werden.

```text
Operation A
    ↓
Committed
    ↓
Later Failure
    ↓
Compensation A
```

Eine Kompensation muss nicht den ursprünglichen technischen Zustand herstellen.

Sie muss einen definierten semantisch gültigen Zustand erzeugen.

## Reconciliation

Bei unbekanntem oder nicht unmittelbar kontrollierbarem Remote-Zustand muss Nova.Resilience den tatsächlichen Zustand zunächst bestimmen können.

```text
Unknown Remote State
        ↓
Reconciliation
        ↓
Observed State
        ↓
Recovery Decision
```

Ein unbekannter Zustand darf nicht automatisch als erfolgreich zurückgerollt interpretiert werden.

## Recovery Graph

Systemübergreifende Operationen werden als Abhängigkeitsgraph behandelt.

```text
A
↓
B
├──→ C
└──→ D
```

Recovery muss Abhängigkeiten und Reihenfolgen berücksichtigen.

Eine Komponente darf nicht zurückgesetzt werden, wenn dadurch bereits wiederhergestellte Invarianten anderer Komponenten verletzt würden.

## Commit Points

Operationen können explizite Commit Points besitzen.

```text
Prepare
   ↓
Commit Point
   ↓
External Effect
```

Vor einem Commit Point kann ein technischer Rollback möglich sein.

Danach können Compensation oder Reconciliation erforderlich werden.

## Irreversible Operationen

Bestimmte Aktionen sind grundsätzlich nicht vollständig rückgängig zu machen.

Beispiele:

```text
Physical Actuation
External Notification
Published Information
Irreversible Device Action
External Third-Party Mutation
```

Solche Operationen müssen als `Irreversible` oder entsprechend eingeschränkt gekennzeichnet sein.

Nova.Resilience darf für sie keinen vollständigen Rollback versprechen.

## Semantic Checkpoints

Systemübergreifender Rollback kann Semantic Checkpoints verwenden.

```text
Semantic Checkpoint
        ↓
Distributed Operation
        ↓
Failure
        ↓
Rollback / Compensation
        ↓
Checkpoint Invariants
        ↓
Verify
```

Der Checkpoint beschreibt dabei den angestrebten semantisch gültigen Zustand.

## Authority

Rollback- und Compensation-Aktionen benötigen eigene gültige Authority.

```text
Original Authority
       ≠
Recovery Authority
```

Eine ursprünglich erlaubte Operation bedeutet nicht automatisch, dass jede mögliche Gegenoperation ebenfalls erlaubt ist.

Nova.Resilience muss notwendige Recovery Authorities explizit berücksichtigen.

## Security und Data Sovereignty

Systemübergreifende Recovery darf keine Sicherheits- oder Datenflussregeln umgehen.

Beispielsweise darf ein Rollback keine geschützten Daten an einen Provider übertragen, der diese Daten nach aktueller Policy nicht mehr erhalten darf.

Aktuelle Hard Constraints haben Vorrang vor der Rekonstruktion eines historischen Zustands.

## Provider-Ausfall

Ist ein ursprünglicher Capability Provider nicht mehr verfügbar, kann Nova.Resilience einen kompatiblen alternativen Provider für Recovery verwenden, sofern die Semantik dies erlaubt.

```text
Provider A
   ↓ unavailable
Recovery Plan
   ↓
Provider B
```

Der alternative Provider muss die notwendigen Contracts, Trust Requirements und Data-Sovereignty-Anforderungen erfüllen.

## Verification

Ein systemübergreifender Rollback gilt erst nach Verification als erfolgreich.

```text
Rollback
   +
Compensation
   +
Reconciliation
        ↓
Invariant Verification
        ↓
Valid State
```

Ein technisch abgeschlossener Recovery Plan ist nicht automatisch semantisch erfolgreich.

## Partial Recovery

Kann der ursprüngliche Zustand nicht vollständig wiederhergestellt werden, muss Nova.Resilience einen definierten sicheren Zustand herstellen oder eskalieren.

```text
Full Recovery impossible
        ↓
Safe Consistent State
        ↓
Degraded / Escalated
```

Ein teilweise wiederhergestellter Zustand darf nicht als vollständiger Rollback dargestellt werden.

## Normative Anforderungen

1. NovaOS MUSS systemübergreifende Rollback- und Recovery-Operationen unterstützen können.
2. Systemübergreifender Rollback MUSS als semantische Recovery und nicht ausschließlich als technische Zustandsumkehr behandelt werden.
3. Beteiligte Operationen MÜSSEN ihre relevanten Recovery-Eigenschaften deklarieren können.
4. Nova.Resilience MUSS zwischen Rollback, Compensation und Reconciliation unterscheiden können.
5. Bereits ausgeführte irreversible Side Effects DÜRFEN nicht als vollständig rückgängig gemacht dargestellt werden.
6. Unbekannte Remote-Zustände MÜSSEN vor weiteren zustandsabhängigen Recovery-Entscheidungen reconciliert werden können.
7. Abhängigkeiten zwischen beteiligten Operationen MÜSSEN bei Recovery berücksichtigt werden.
8. Commit Points MÜSSEN bei der Auswahl der Recovery-Strategie berücksichtigt werden können.
9. Semantic Checkpoints MÜSSEN als Zielzustände für systemübergreifende Recovery verwendbar sein.
10. Recovery- und Compensation-Aktionen MÜSSEN die dafür erforderliche Authority besitzen.
11. Historische Authority DARF aktuelle Revocation- oder Security-Entscheidungen nicht umgehen.
12. Aktuelle Security-, Trust-, Integrity- und Data-Sovereignty-Constraints MÜSSEN auch während Rollback gelten.
13. Alternative Provider DÜRFEN nur bei semantischer und vertraglicher Kompatibilität verwendet werden.
14. Systemübergreifende Recovery MUSS abschließend verifiziert werden.
15. Nicht vollständig wiederherstellbare Operationen MÜSSEN in einen definierten sicheren Zustand überführt oder eskaliert werden.
16. Partial Recovery DARF nicht als vollständiger Rollback dargestellt werden.
17. Recovery-Schleifen MÜSSEN begrenzt werden.
18. Rollback-, Compensation-, Reconciliation- und Verification-Ereignisse MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Recovery funktioniert auch über Prozess-, Dienst-, Geräte- und Rechnergrenzen,
- irreversible Side Effects werden explizit berücksichtigt,
- NovaOS verspricht keinen technisch unmöglichen universellen Undo,
- Semantic Checkpoints können als systemübergreifende Recovery-Ziele dienen,
- aktuelle Security- und Sovereignty-Regeln bleiben auch während Recovery gültig.

### Negative Konsequenzen

- beteiligte Operationen benötigen explizite Recovery-Semantik,
- Compensation kann komplexer als lokaler Rollback sein,
- unbekannte Remote-Zustände benötigen zusätzliche Reconciliation,
- vollständige Wiederherstellung ist bei irreversiblen Side Effects nicht immer möglich.

## Verworfene Alternativen

### Globaler atomarer Rollback für alle Systemoperationen

Verworfen, da externe Systeme, Geräte und irreversible Side Effects nicht generell atomar zurückgesetzt werden können.

### Nur lokalen Zustand zurückrollen

Verworfen, da dadurch bereits ausgeführte externe Effekte inkonsistent zum lokalen Zustand bleiben können.

### Fehlgeschlagene Remote-Operation automatisch als nicht ausgeführt behandeln

Verworfen, da bei Kommunikationsfehlern der tatsächliche Commit-Zustand unbekannt sein kann.

### Historischen Zustand vollständig erzwingen

Verworfen, da aktuelle Security-, Revocation- oder Data-Sovereignty-Regeln Vorrang besitzen müssen.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0005_Retry_nur_nach_expliziter_Operationssemantik`
- `ADR-RESILIENCE-0009_Semantic_Checkpointing`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-CAPABILITY-0009_Capability_Revocation`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-CROSS-SYSTEM-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-COMPENSATION-0001`
- `NPSPEC-RESILIENCE-RECONCILIATION-0001`
- `NPSPEC-RESILIENCE-RECOVERY-GRAPH-0001`
- `NPSPEC-RESILIENCE-COMMIT-POINT-0001`
- `NPSPEC-RESILIENCE-IRREVERSIBLE-EFFECT-0001`
- `NPSPEC-RESILIENCE-CROSS-SYSTEM-ROLLBACK-TEST-0001`

## Ergebnis

Nova.Resilience behandelt Rollback über Systemgrenzen als koordinierte semantische Wiederherstellung:

```text
Distributed Operation
        ↓
Failure
        ↓
Determine Actual State
        ↓
Recovery Graph
        ↓
Rollback
   / Compensation
   / Reconciliation
        ↓
Verify Invariants
        ↓
Restore
```

Damit kann NovaOS auch komplexe, verteilte Operationen kontrolliert wiederherstellen, ohne fälschlicherweise anzunehmen, dass jeder bereits ausgeführte Side Effect technisch rückgängig gemacht werden kann.