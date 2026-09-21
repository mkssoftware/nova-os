# ADR-STATE-0002 – Desired und Actual State

## Status

Angenommen

## Kategorie

State Management / Declarative Architecture

## Kontext

NovaOS arbeitet deklarativ. Dafür muss zwischen dem gewünschten Systemzustand und dem tatsächlich beobachteten Zustand unterschieden werden.

```text
Desired State
≠
Actual State
```

Beispiel:

```text
Desired: Service A = Running
Actual:  Service A = Stopped
```

Ohne diese Trennung wären automatische Konfiguration, Recovery und Self-Healing nur schwer zuverlässig umsetzbar.

## Entscheidung

NovaOS modelliert **Desired State** und **Actual State** als getrennte Zustände innerhalb des System State Graph.

```text
Desired State
      ↓
   Compare
      ↑
Actual State
      ↓
 Difference
      ↓
Reconciliation
```

### Desired State

Beschreibt, welchen Zustand das System erreichen oder erhalten soll.

Beispiele:

```text
Service = Running
Volume = Mounted
Network = Connected
Replica Count = 3
Capability = Available
```

Desired State wird durch autorisierte Konfiguration, Policies oder Benutzerentscheidungen festgelegt.

### Actual State

Beschreibt den aktuell beobachteten Zustand.

Beispiele:

```text
Service = Stopped
Volume = Mounted
Network = Degraded
Replica Count = 2
Capability = Unavailable
```

Actual State wird von den zuständigen Subsystemen bzw. State Providern geliefert.

## Reconciliation

Weichen beide Zustände voneinander ab, kann ein Reconciliation-Prozess versuchen, den Actual State an den Desired State anzunähern.

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

Dabei bleibt der Desired State stabil, bis er autorisiert geändert wird.

## Authority

Der State Graph ist nicht selbst die Authority für Zustandsänderungen.

```text
Desired State
      ↓
Reconciler
      ↓
Authorized Capability
      ↓
Subsystem
      ↓
Actual State
```

Änderungen erfolgen über die zuständigen Capabilities und Subsysteme.

## Unknown State

Actual State kann unbekannt sein.

```text
Actual State:
Unknown
```

Dabei gilt:

```text
Unknown ≠ Desired State violated
Unknown ≠ Resource absent
```

NovaOS muss unbekannten Zustand explizit behandeln.

## Failure

Kann der Desired State nicht erreicht werden, bleibt die Abweichung sichtbar.

```text
Desired: Running
Actual:  Failed
```

NovaOS darf den Desired State nicht automatisch verändern, nur damit Desired und Actual wieder übereinstimmen.

Stattdessen können Recovery, Degradation oder Eskalation ausgelöst werden.

## Hard Constraints

Reconciliation darf keine Hard Constraints verletzen.

```text
Desired State
      ↓
Safety
Security
Sovereignty
Resource Constraints
Realtime Constraints
      ↓
Valid Reconciliation
```

Ein gewünschter Zustand ist keine Berechtigung, Sicherheits- oder Systemregeln zu umgehen.

## Normative Anforderungen

1. NovaOS MUSS Desired und Actual State getrennt modellieren.
2. Desired State MUSS durch autorisierte Quellen festgelegt werden.
3. Actual State MUSS aus beobachtetem Systemzustand entstehen.
4. Abweichungen zwischen Desired und Actual State MÜSSEN erkennbar sein.
5. Reconciliation MUSS über autorisierte Capabilities erfolgen.
6. `Unknown` MUSS als eigener Actual-State-Zustand behandelt werden.
7. Ein Fehler DARF den Desired State nicht automatisch umdefinieren.
8. Reconciliation DARF keine Hard Constraints verletzen.
9. Änderungen von Desired und Actual State MÜSSEN versionierbar sein.

## Konsequenzen

### Positiv

- klare Grundlage für deklarative Systemverwaltung,
- Self-Healing kann zielgerichtet arbeiten,
- Abweichungen bleiben sichtbar,
- Systemzustände werden nachvollziehbarer,
- Recovery und Konfiguration verwenden dasselbe Modell.

### Negativ

- zusätzlicher Zustand muss verwaltet werden,
- Reconciliation benötigt Konflikt- und Fehlerbehandlung,
- Actual State kann zeitweise veraltet oder unbekannt sein.

## Abhängigkeiten

- `ADR-STATE-0001_Globaler_System_State_Graph`
- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-DECLARATIVE-0001`
- `ADR-AUTONOMY-0003_Self_Configuration`
- `ADR-AUTONOMY-0005_Self_Healing`

## Ergebnis

NovaOS trennt konsequent:

```text
Desired State = Wie das System sein soll
Actual State  = Wie das System tatsächlich ist
```

Die Differenz zwischen beiden ist die Grundlage für kontrollierte Reconciliation, Self-Configuration und Self-Healing.