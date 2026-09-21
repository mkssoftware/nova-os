# ADR-STATE-0004 – State Reconciliation

## Status

Angenommen

## Kategorie

State Management / Declarative Architecture / Autonomy

## Kontext

NovaOS trennt zwischen:

```text
Desired State
```

und:

```text
Actual State
```

Weichen beide voneinander ab, benötigt das System einen kontrollierten Mechanismus, um den tatsächlichen Zustand wieder an den gewünschten Zustand anzunähern.

## Entscheidung

NovaOS verwendet **State Reconciliation** als grundlegenden Mechanismus zur Umsetzung deklarativer Zustände.

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
   ↺
```

Der Reconciler arbeitet solange, bis:

```text
Actual State = Desired State
```

oder ein definierter Fehler-, Blockierungs- oder Degradationszustand erreicht wird.

## Reconciliation Loop

### Observe

Der aktuelle Zustand wird über die zuständigen State Provider ermittelt.

### Compare

Desired und Actual State werden verglichen.

```text
Desired == Actual
→ keine Aktion

Desired != Actual
→ Reconciliation erforderlich
```

### Plan

NovaOS bestimmt einen gültigen Übergangspfad anhand der vorhandenen State Machines.

### Execute

Die notwendigen Änderungen werden über autorisierte Capabilities ausgeführt.

### Verify

Nach der Änderung wird der tatsächliche Zustand erneut geprüft.

Eine erfolgreiche Operation allein gilt nicht als Nachweis, dass der gewünschte Zustand tatsächlich erreicht wurde.

## Idempotenz

Reconciliation-Operationen sollen soweit möglich idempotent sein.

```text
Reconcile
Reconcile
Reconcile
```

darf nicht zu unerwünschten zusätzlichen Änderungen führen.

Dies ermöglicht sichere Wiederholung nach:

```text
Crash
Timeout
Restart
Unknown Completion
```

## Konflikte

Mehrere widersprüchliche Desired States dürfen nicht unkontrolliert gleichzeitig umgesetzt werden.

Konflikte müssen durch:

```text
Authority
Policy
Priority
Constraints
```

aufgelöst oder explizit als nicht lösbar markiert werden.

## Hard Constraints

Reconciliation arbeitet ausschließlich innerhalb gültiger Hard Constraints.

```text
Desired State
      ↓
Hard Constraints
      ↓
Valid Transition Plan
```

Ein gewünschter Zustand darf insbesondere keine:

```text
Security
Safety
Sovereignty
Realtime
Resource
```

Regeln umgehen.

## Failure Handling

Kann der Desired State nicht erreicht werden, darf keine Endlosschleife entstehen.

NovaOS verwendet begrenzte:

```text
Retry
Backoff
Alternative Path
Recovery
Degradation
Escalation
```

Der nicht erreichte Zustand bleibt sichtbar.

## Transaktionen

Wenn mehrere Zustandsänderungen gemeinsam konsistent sein müssen, verwendet Reconciliation die transaktionalen Mechanismen von NovaOS.

```text
Plan
  ↓
Prepare
  ↓
Commit
  ↓
Verify
```

## Autonomie

Self-Configuration und Self-Healing verwenden State Reconciliation als Ausführungsmechanismus.

```text
Policy / Desired State
        ↓
Reconciliation
        ↓
Controlled System Change
```

Autonome Systeme ändern nicht beliebig den Actual State, sondern arbeiten über denselben kontrollierten Mechanismus.

## Normative Anforderungen

1. NovaOS MUSS State Reconciliation für deklarative Zustände unterstützen.
2. Reconciliation MUSS dem Zyklus `Observe → Compare → Plan → Execute → Verify` folgen.
3. Zustandsänderungen MÜSSEN über autorisierte Capabilities erfolgen.
4. State Machines MÜSSEN bei der Planung gültiger Übergänge berücksichtigt werden.
5. Reconciliation-Operationen SOLLEN soweit möglich idempotent sein.
6. Hard Constraints DÜRFEN durch Reconciliation nicht verletzt werden.
7. Fehlgeschlagene Reconciliation DARF nicht zu unbounded Retry-Schleifen führen.
8. Konsistenzkritische Mehrfachänderungen MÜSSEN transaktionale Mechanismen verwenden.
9. Der tatsächliche Zielzustand MUSS nach Änderungen verifiziert werden.

## Konsequenzen

### Positiv

- einheitlicher Mechanismus für deklarative Systemänderungen,
- Grundlage für Self-Configuration und Self-Healing,
- robuste Wiederholung nach Fehlern,
- Desired State bleibt vom Ausführungsmechanismus getrennt.

### Negativ

- benötigt zuverlässige State Provider,
- Konflikte zwischen Desired States müssen behandelt werden,
- komplexe Reconciliation kann mehrere Iterationen benötigen.

## Abhängigkeiten

- `ADR-STATE-0001_Globaler_System_State_Graph`
- `ADR-STATE-0002_Desired_und_Actual_State`
- `ADR-STATE-0003_State_Machines`
- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-DECLARATIVE-0001`
- `ADR-AUTONOMY-0003_Self_Configuration`
- `ADR-AUTONOMY-0005_Self_Healing`

## Ergebnis

NovaOS behandelt Systemkonfiguration nicht als Folge einmaliger Befehle, sondern als kontinuierlichen Abgleich:

```text
Desired State
      ↓
Reconciliation
      ↓
Actual State
```

Das System versucht kontrolliert, den gewünschten Zustand herzustellen und dauerhaft zu erhalten.