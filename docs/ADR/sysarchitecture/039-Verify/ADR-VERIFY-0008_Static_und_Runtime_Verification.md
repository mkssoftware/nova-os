# ADR-VERIFY-0008 – Static und Runtime Verification

## Status

Angenommen

## Kategorie

Verification / Static Analysis / Runtime Verification

## Kontext

Nicht alle Eigenschaften von NovaOS können mit einer einzigen Verifikationsmethode zuverlässig geprüft werden.

Einige Fehler sind bereits vor der Ausführung erkennbar, während andere erst durch konkrete Laufzeitzustände, Hardwareverhalten oder dynamische Interaktionen entstehen.

NovaOS benötigt deshalb eine kombinierte Verifikationsstrategie.

## Entscheidung

NovaOS kombiniert **Static Verification und Runtime Verification**.

```text
Source / Model
     ↓
Static Verification
     ↓
Execution
     ↓
Runtime Verification
```

Beide Verfahren ergänzen sich und werden entsprechend Kritikalität und überprüfter Eigenschaft eingesetzt.

## Static Verification

Static Verification prüft Eigenschaften ohne Ausführung des produktiven Systems.

Dazu können gehören:

```text
Type Checking
Static Analysis
Formal Verification
Model Checking
Control/Data Flow Analysis
Contract Checking
```

Bevorzugt statisch geprüft werden Eigenschaften, die zuverlässig vor der Ausführung bestimmbar sind.

## Runtime Verification

Runtime Verification überprüft Eigenschaften anhand des tatsächlichen Systemverhaltens.

Dazu gehören beispielsweise:

```text
Runtime Assertions
Contract Validation
State Validation
Temporal Monitoring
Resource Monitoring
Information Flow Enforcement
```

Sie wird insbesondere dort eingesetzt, wo Eigenschaften von dynamischem Zustand, Hardware oder externen Komponenten abhängen.

## Hard Constraints

Harte Systemregeln dürfen nicht ausschließlich von passiver Laufzeitbeobachtung abhängen.

Wenn eine Verletzung verhindert werden kann, soll NovaOS bevorzugt:

```text
Prevent
```

statt lediglich:

```text
Detect after violation
```

Runtime Verification dient dabei zusätzlich zur Absicherung und Erkennung nicht vorhersehbarer Abweichungen.

## Verification Points

Prüfungen sollen bevorzugt an klaren Systemgrenzen stattfinden:

```text
Syscalls
IPC
Capability Calls
State Transitions
Resource Allocation
Driver Interfaces
Network Boundaries
Commit Points
```

Dadurch werden Prüfungen gezielt statt wahllos über das gesamte System verteilt.

## Reaktion auf Verletzungen

Eine Runtime-Verletzung erzeugt ein explizites Verification Result.

Abhängig von Kritikalität kann NovaOS:

```text
Reject
Isolate
Abort
Rollback
Recover
Degrade
Enter Safe State
```

Die Reaktion muss zur betroffenen Systemregel passen.

## Verification Metadata

Verifikationsergebnisse sollen ihrer Grundlage zuordenbar sein.

```text
Component Version
Specification Version
Contract Version
Verification Method
Result
```

Damit bleibt nachvollziehbar, welche Eigenschaften für welche Systemversion überprüft wurden.

## Normative Anforderungen

1. NovaOS MUSS Static und Runtime Verification kombinieren können.
2. Eigenschaften SOLLEN statisch geprüft werden, wenn dies zuverlässig möglich ist.
3. Dynamische Eigenschaften MÜSSEN bei Bedarf zur Laufzeit überprüfbar sein.
4. Hard Constraints SOLLEN bevorzugt präventiv durchgesetzt werden.
5. Runtime Verification DARF notwendige statische Verifikation nicht ersetzen.
6. Prüfungen SOLLEN bevorzugt an klar definierten Systemgrenzen stattfinden.
7. Verification Failures MÜSSEN entsprechend ihrer Kritikalität behandelt werden.
8. Verifikationsergebnisse MÜSSEN konkreten Versionen zuordenbar sein.
9. Verification Mechanisms DÜRFEN selbst keine Hard-Realtime- oder Security-Garantien unkontrolliert verletzen.

## Konsequenzen

### Positiv

- unterschiedliche Fehlerklassen werden mit geeigneten Methoden geprüft,
- Fehler können sowohl vor als auch während der Ausführung erkannt werden,
- dynamisches Hardware- und Systemverhalten bleibt überprüfbar,
- kritische Regeln erhalten mehrere Schutzebenen.

### Negativ

- mehrere Verifikationsmechanismen erhöhen den Entwicklungsaufwand,
- Runtime Verification verursacht zusätzlichen Laufzeitaufwand.

## Abhängigkeiten

- `ADR-VERIFY-0001_Formale_Spezifikation_für_kritische_Komponenten`
- `ADR-VERIFY-0002_Model_Checking_für_kritische_State_Machines`
- `ADR-VERIFY-0003_Memory_Safety`
- `ADR-VERIFY-0004_Type_Safety`
- `ADR-VERIFY-0005_Capability_Safety`
- `ADR-VERIFY-0006_Temporal_Verification`
- `ADR-VERIFY-0007_Information_Flow_Verification`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

## Ergebnis

NovaOS folgt einer mehrstufigen Verifikationsstrategie:

```text
Verify before execution
        +
Verify during execution
        =
Layered Verification
```

Was zuverlässig vor der Ausführung geprüft werden kann, wird möglichst früh geprüft. Was vom tatsächlichen Laufzeitzustand abhängt, wird zusätzlich während der Ausführung überwacht und validiert.