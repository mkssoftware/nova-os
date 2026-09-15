# ADR-DECLARATIVE-0002 – State Reconciliation

## Status

Angenommen

## Kategorie

Declarative Architecture / Reconciliation

## Kontext

NovaOS trennt Desired State und Actual State.

Damit diese Trennung praktisch nutzbar wird, benötigt das System einen einheitlichen Mechanismus, der Abweichungen erkennt und den Actual State kontrolliert in Richtung Desired State überführt.

## Entscheidung

NovaOS verwendet **State Reconciliation** als systemweiten Mechanismus zur Zustandskonvergenz.

```text
Desired State
      ↓
Compare
      ↑
Actual State
      ↓
Difference
      ↓
Reconciliation Plan
      ↓
Execute
      ↓
Verify
```

## Reconciliation Loop

Ein Reconciler arbeitet grundsätzlich nach folgendem Modell:

```text
Observe
   ↓
Compare
   ↓
Plan
   ↓
Execute
   ↓
Verify
```

Der Zyklus wird wiederholt, bis der Desired State erreicht wurde oder eine definierte Grenze erreicht ist.

## Observe

Der Actual State muss aus dem realen Systemzustand ermittelt werden.

```text
System
   ↓
Observation
   ↓
Actual State
```

Veraltete Annahmen dürfen nicht als aktueller Zustand behandelt werden.

## Compare

Desired und Actual State werden semantisch verglichen.

```text
Desired:
    Service = Running

Actual:
    Service = Stopped

Difference:
    Start Required
```

Nur relevante Abweichungen erzeugen Reconciliation-Arbeit.

## Plan

Aus der Differenz wird ein Ausführungsplan erzeugt.

```text
Difference
    ↓
Dependencies
    ↓
Policies
    ↓
Contracts
    ↓
Plan
```

Der Plan beschreibt die notwendigen Zustandsänderungen.

## Execute

Zustandsänderungen werden über die vorgesehenen transaktionalen und Capability-basierten Mechanismen ausgeführt.

```text
Plan
  ↓
System Transaction
  ↓
Execution
```

## Verify

Nach der Ausführung wird der tatsächliche Zustand erneut beobachtet.

```text
Execute
   ↓
Observe
   ↓
Actual == Desired?
```

Ein erfolgreicher Operationsaufruf allein gilt nicht als erfolgreiche Konvergenz.

## Idempotenz

Reconciliation-Schritte sollen soweit möglich idempotent sein.

```text
Desired == Actual
       ↓
No Operation
```

Wiederholte Reconciliation darf keine unnötigen Zustandsänderungen erzeugen.

## Konflikte

Mehrere widersprüchliche Desired States dürfen nicht unkontrolliert gegeneinander arbeiten.

```text
Desired A
    ↘
   Conflict
    ↗
Desired B
```

Konflikte müssen über definierte Ownership-, Priority- oder Policy-Regeln aufgelöst werden.

## Fehler

Kann ein Zustand nicht erreicht werden, muss der Reconciler dies explizit melden.

```text
Desired State
      ↓
Reconciliation Failed
      ↓
Reason
```

Unbegrenzte Wiederholungen sind nicht zulässig.

## Normative Anforderungen

1. NovaOS MUSS einen systemweiten State-Reconciliation-Mechanismus unterstützen.
2. Reconciliation MUSS auf Desired State und beobachtetem Actual State basieren.
3. Der Actual State MUSS vor relevanten Entscheidungen beobachtet werden.
4. Zustandsabweichungen MÜSSEN explizit bestimmbar sein.
5. Reconciliation MUSS einen kontrollierten Ausführungsplan erzeugen können.
6. Zustandsänderungen SOLLEN transaktional ausgeführt werden.
7. Nach der Ausführung MUSS der Actual State verifiziert werden.
8. Reconciliation SOLL idempotent sein.
9. Konflikte zwischen Desired States MÜSSEN kontrolliert behandelt werden.
10. Fehlgeschlagene Reconciliation DARF nicht unbegrenzt wiederholt werden.

## Konsequenzen

### Positive Konsequenzen

- einheitliche Zustandskonvergenz im gesamten System,
- Grundlage für Self-Healing und deklarative Konfiguration,
- Abweichungen können automatisch korrigiert werden,
- Operationserfolg und tatsächlicher Zustand werden getrennt betrachtet.

### Negative Konsequenzen

- zuverlässige State Observation wird benötigt,
- Konflikt- und Retry-Regeln müssen definiert werden.

## Abhängigkeiten

- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0006_Retry_Budgets_Backoff_und_Jitter`

## Zugehörige NPSPECs

- `NPSPEC-DECLARATIVE-RECONCILIATION-0001`
- `NPSPEC-DECLARATIVE-RECONCILIATION-PLAN-0001`
- `NPSPEC-DECLARATIVE-RECONCILIATION-CONFLICT-0001`

## Ergebnis

NovaOS gleicht den tatsächlichen Systemzustand kontrolliert mit dem gewünschten Zustand ab:

```text
Observe
   ↓
Compare
   ↓
Plan
   ↓
Execute
   ↓
Verify
   ↓
Converged
```

Der zentrale Grundsatz lautet:

```text
Nicht Befehle werden wiederholt,
bis sie erfolgreich erscheinen.

Der tatsächliche Zustand wird geprüft,
bis er dem gewünschten Zustand entspricht.
```