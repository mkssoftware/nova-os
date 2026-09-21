# ADR-VERIFY-0002 – Model Checking für kritische State Machines

## Status

Angenommen

## Kategorie

Verification / Model Checking / State Machines

## Kontext

NovaOS verwendet explizite State Machines für kritische Komponenten.

Bei komplexen Zustandsautomaten können Fehler entstehen wie:

```text
Unreachable States
Deadlocks
Invalid Transitions
Infinite Cycles
Missing Recovery Paths
Safety Violations
```

Normale Tests können nicht zuverlässig alle möglichen Zustandsfolgen abdecken.

## Entscheidung

NovaOS verwendet **Model Checking für ausgewählte kritische State Machines**.

```text
State Machine
     ↓
Formal Model
     ↓
Model Checker
     ↓
Invariant / Property Verification
```

Model Checking wird gezielt eingesetzt und ist keine Pflicht für jede State Machine.

## Geeignete Komponenten

Besonders geeignet sind:

```text
Security State Machines
Boot / Recovery
Transactional State
Update State Machines
Capability Lifecycle
Realtime Control
Failover
Self-Healing
```

## Zu prüfende Eigenschaften

Model Checking soll insbesondere Eigenschaften prüfen wie:

```text
Safety
Liveness
Reachability
Deadlock Freedom
Valid Recovery
Invariant Preservation
```

Beispiele:

```text
Failed → Recovery path exists

Unauthorized → Privileged
must never be reachable

Transaction cannot be
Committed and RolledBack
simultaneously
```

## Abstraktion

Das formale Modell soll nur die für die zu prüfende Eigenschaft relevanten Zustände und Übergänge enthalten.

```text
Implementation
     ↓
Relevant Abstraction
     ↓
Model Checking
```

Dadurch wird State-Space Explosion begrenzt.

## Counterexamples

Wird eine Eigenschaft verletzt, soll der Model Checker einen reproduzierbaren Gegenbeispielpfad liefern.

```text
State A
  ↓
State B
  ↓
State C
  ↓
Violation
```

Dieser Pfad kann anschließend für Tests und Fehleranalyse verwendet werden.

## Verhältnis zur Implementierung

Ein korrektes Modell beweist nicht automatisch eine korrekte Implementierung.

Deshalb wird Model Checking kombiniert mit:

```text
Implementation Tests
Static Analysis
Runtime Assertions
```

Modell und Implementierung müssen derselben State-Machine-Spezifikation folgen.

## Normative Anforderungen

1. Kritische State Machines SOLLEN für Model Checking geeignet modelliert werden.
2. Model Checking MUSS selektiv und risikobasiert eingesetzt werden.
3. Safety- und Security-Invarianten SOLLEN explizit prüfbar sein.
4. Deadlocks und unerreichbare kritische Zustände SOLLEN geprüft werden.
5. Kritische Recovery-Pfade SOLLEN auf Erreichbarkeit geprüft werden.
6. Modelle SOLLEN auf die relevanten Eigenschaften abstrahiert werden.
7. Gefundene Counterexamples SOLLEN als reproduzierbare Testfälle nutzbar sein.
8. Model Checking DARF Implementierungstests nicht ersetzen.
9. Modell und State-Machine-Version MÜSSEN eindeutig zuordenbar sein.

## Konsequenzen

### Positiv

- seltene Zustandsfehler können systematisch gefunden werden,
- Deadlocks und ungültige Übergangspfade werden früher sichtbar,
- kritische Recovery- und Security-Invarianten können überprüft werden,
- Counterexamples liefern konkrete Fehlerpfade.

### Negativ

- große State Spaces können hohen Prüfaufwand verursachen,
- Modelle müssen mit der Implementierung synchron gehalten werden.

## Abhängigkeiten

- `ADR-VERIFY-0001_Formale_Spezifikation_für_kritische_Komponenten`
- `ADR-STATE-0003_State_Machines`
- `ADR-STATE-0004_State_Reconciliation`
- `ADR-STATE-0007_Transactional_State`
- `ADR-STATE-0008_State_Rollback`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

## Ergebnis

NovaOS verwendet Model Checking gezielt für kritische Zustandsautomaten:

```text
Critical State Machine
        ↓
Model Checking
        ↓
Verify Invariants
        ↓
Counterexample if violated
```

Damit können kritische Zustandsfolgen systematisch geprüft werden, ohne das gesamte NovaOS formal modellieren zu müssen.