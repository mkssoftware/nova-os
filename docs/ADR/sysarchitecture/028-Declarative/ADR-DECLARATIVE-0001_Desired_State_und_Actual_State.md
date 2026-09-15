# ADR-DECLARATIVE-0001 – Desired State und Actual State

## Status

Angenommen

## Kategorie

Declarative Architecture / System State

## Kontext

Traditionelle Systeme beschreiben häufig einzelne imperative Schritte:

```text
Stop Service
Change Configuration
Install Component
Start Service
```

Dadurch muss der Aufrufer wissen, wie ein gewünschter Zustand erreicht wird.

NovaOS soll stattdessen zwischen dem **gewünschten Zustand** und dem **tatsächlichen Zustand** unterscheiden.

## Entscheidung

NovaOS verwendet systemweit das Modell:

```text
Desired State
     ↓
Reconciliation
     ↓
Actual State
```

Der Desired State beschreibt, **wie das System sein soll**.

Der Actual State beschreibt, **wie das System tatsächlich ist**.

## Desired State

Der Desired State ist deklarativ.

Beispiele:

```text
Service = Running
Version = 3
Firewall = Enabled
Storage.Encryption = Required
Capability = Available
```

Er beschreibt das Ziel, nicht die dafür notwendigen Einzelschritte.

## Actual State

Der Actual State wird aus dem realen Systemzustand ermittelt.

```text
Desired:
    Service = Running

Actual:
    Service = Stopped
```

Die Differenz bildet die Grundlage für weitere Aktionen.

## Reconciliation

Ein Reconciler vergleicht Desired und Actual State.

```text
Desired State
      ↓
Compare
      ↑
Actual State
      ↓
Difference
      ↓
Plan
      ↓
Execute
      ↓
Verify
```

Nach der Ausführung wird der Actual State erneut geprüft.

## Konvergenz

Das Ziel ist:

```text
Actual State
     →
Desired State
```

NovaOS soll den tatsächlichen Zustand kontrolliert zum gewünschten Zustand konvergieren lassen.

## Kein blindes Erzwingen

Desired State bedeutet nicht, dass jede Abweichung sofort verändert werden darf.

Vor einer Änderung müssen relevante:

```text
Permissions
Policies
Contracts
Dependencies
Trust
Safety
Resource Constraints
```

berücksichtigt werden.

## Fehler

Kann der Desired State nicht erreicht werden, muss die Abweichung sichtbar bleiben.

```text
Desired ≠ Actual
      ↓
Reason
```

NovaOS darf einen fehlgeschlagenen Zustand nicht fälschlich als konvergiert melden.

## Idempotenz

Reconciliation soll soweit möglich idempotent sein.

```text
Desired == Actual
       ↓
No Change
```

Wiederholte Auswertung desselben Zustands darf keine unnötigen Änderungen verursachen.

## Normative Anforderungen

1. NovaOS MUSS Desired State und Actual State als getrennte Konzepte behandeln.
2. Desired State MUSS deklarativ beschreibbar sein.
3. Actual State MUSS aus dem tatsächlichen Systemzustand ermittelt werden.
4. Abweichungen MÜSSEN explizit erkennbar sein.
5. Reconciliation MUSS einen kontrollierten Übergang zum Desired State planen können.
6. Nach Änderungen MUSS der Actual State erneut verifiziert werden können.
7. Reconciliation SOLL idempotent sein.
8. Sicherheits-, Trust-, Policy- und Contract-Regeln DÜRFEN durch Desired State nicht umgangen werden.
9. Nicht erreichter Desired State DARF nicht als erfolgreich konvergiert gemeldet werden.

## Konsequenzen

### Positive Konsequenzen

- Systemzustände werden deklarativ beschreibbar,
- Recovery und Self-Healing können dasselbe Zustandsmodell verwenden,
- Konfiguration und Updates werden reproduzierbarer,
- unnötige imperative Ablaufsteuerung wird reduziert.

### Negative Konsequenzen

- Actual State muss zuverlässig beobachtbar sein,
- Reconciliation benötigt Konflikt-, Fehler- und Policy-Regeln.

## Abhängigkeiten

- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-UPDATE-0001_Deklarativer_Softwarezustand`

## Zugehörige NPSPECs

- `NPSPEC-DECLARATIVE-STATE-0001`
- `NPSPEC-DECLARATIVE-RECONCILIATION-0001`
- `NPSPEC-DECLARATIVE-ACTUAL-STATE-0001`

## Ergebnis

NovaOS beschreibt primär das gewünschte Ergebnis und leitet daraus die notwendigen Änderungen ab:

```text
Desired State
      ↓
Observe Actual State
      ↓
Calculate Difference
      ↓
Plan
      ↓
Execute
      ↓
Verify
      ↓
Converged State
```

Der zentrale Grundsatz lautet:

```text
Beschreibe,
wie das System sein soll.

NovaOS bestimmt,
wie es diesen Zustand sicher erreicht.
```