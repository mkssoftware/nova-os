# ADR-EXECUTION-0008 – Preferred und Forced Algorithm

## Status

Angenommen

## Kategorie

Execution Architecture / Algorithm Constraints

## Kontext

NovaOS wählt Algorithmen standardmäßig automatisch anhand des `Nova.ExecutionContract`.

Bestimmte Workloads benötigen jedoch Einfluss auf diese Auswahl.

Dabei bestehen zwei unterschiedliche Anforderungen:

```text
Preferred Algorithm
    → bevorzugen, wenn sinnvoll

Forced Algorithm
    → exakt diesen Algorithmus verwenden
```

Diese beiden Fälle müssen im Execution Contract eindeutig getrennt werden.

## Entscheidung

`Nova.ExecutionContract` unterstützt:

```text
Preferred Algorithm
Forced Algorithm
```

`Preferred Algorithm` ist ein **Soft Constraint**.

`Forced Algorithm` ist ein **Hard Constraint**.

## Preferred Algorithm

Ein Preferred Algorithm beeinflusst die automatische Auswahl, erzwingt sie jedoch nicht.

```text
Preferred Algorithm
       ↓
Contract Validation
       ↓
Valid Candidate?
 ├── Yes → bevorzugen
 └── No  → andere gültige Kandidaten
```

NovaOS darf einen anderen Algorithmus wählen, wenn der bevorzugte Algorithmus beispielsweise:

```text
Hard Constraints verletzt
nicht verfügbar ist
ungeeignete Semantic Types besitzt
eine Deadline nicht erfüllen kann
das Resource Budget verletzt
Trust-Anforderungen nicht erfüllt
```

## Forced Algorithm

Ein Forced Algorithm legt den zu verwendenden Algorithmus verbindlich fest.

```text
Forced Algorithm
       ↓
Contract Validation
       ↓
Valid?
 ├── Yes → verwenden
 └── No  → Contract Unsatisfied
```

NovaOS darf bei einem Forced Algorithm nicht automatisch auf einen anderen Algorithmus ausweichen.

## Hard Constraints bleiben bindend

Auch ein Forced Algorithm darf andere Hard Constraints nicht umgehen.

```text
Forced Algorithm
       ↓
Semantic Compatibility
Determinism
Deadline
Resource Budget
Data Sovereignty
Trust
Security
       ↓
Valid / Reject
```

Ein Forced Algorithm bedeutet:

```text
Use this algorithm if contract-valid
```

nicht:

```text
Ignore the contract
```

## Algorithm Family

Preferred und Forced Algorithm müssen zur benötigten Algorithm Family passen.

```text
Capability
    ↓
Algorithm Family
    ↓
Preferred / Forced Algorithm
```

Ein Algorithmus aus einer semantisch inkompatiblen Family darf nicht durch Override erzwungen werden.

## Algorithmusversion

Der Contract kann zusätzlich eine Version einschränken.

Beispiele:

```text
Algorithm = FFT.Radix2
Version = Latest Compatible
```

oder:

```text
Algorithm = FFT.Radix2
Version = 3.2.1
```

Eine exakte Version ist insbesondere für reproduzierbare Ausführungen relevant.

## Verhältnis zur automatischen Auswahl

Ohne Override gilt:

```text
Automatic Algorithm Selection
```

Mit Preferred Algorithm:

```text
Automatic Selection
       +
Preference
```

Mit Forced Algorithm:

```text
Forced Algorithm
       +
Contract Validation
```

Die automatische Auswahl wird im letzten Fall auf genau diesen Kandidaten eingeschränkt.

## Providerwahl

Algorithmus- und Providerwahl bleiben getrennt.

```text
Forced Algorithm
       ↓
Valid Providers
       ↓
Provider Selection
```

Ein Forced Algorithm erzwingt nicht automatisch einen bestimmten Compute Provider.

Dafür existieren separate Provider Constraints.

## Kombination von Overrides

Der Execution Contract kann Algorithmus- und Provideranforderungen kombinieren.

```text
Forced Algorithm
       +
Forced Provider
       ↓
Compatibility Check
       ↓
Valid Combination?
```

Ist die Kombination nicht ausführbar, gilt der Contract als nicht erfüllbar.

## Determinismus

Ein Forced Algorithm kann für reproduzierbare Ausführungen verwendet werden.

Für vollständige Reproduzierbarkeit können zusätzlich erforderlich sein:

```text
Algorithm Version
Provider
Provider Version
Selection Policy
Execution Parameters
```

Das Erzwingen eines Algorithmus allein garantiert keinen deterministischen Output.

## Typische Anwendungen

Preferred Algorithm:

```text
Performance Tuning
Energy Optimization
Known Workload Preference
Application Hint
```

Forced Algorithm:

```text
Debugging
Benchmarking
Validation
Compatibility Testing
Scientific Reproduction
Regression Testing
```

## Fehlerverhalten

Kann ein Forced Algorithm nicht verwendet werden, muss NovaOS den Grund strukturiert melden.

Beispiele:

```text
AlgorithmUnavailable
VersionUnavailable
SemanticTypeMismatch
DeterminismUnsatisfied
DeadlineUnsatisfied
ResourceBudgetUnsatisfied
TrustUnsatisfied
ProviderUnavailable
```

Es darf kein stiller Fallback erfolgen.

## Introspection

NovaOS muss nachvollziehbar machen können:

```text
Automatic Selection?
Preferred Algorithm?
Forced Algorithm?
Selected Algorithm?
Selected Version?
Preference ignored?
Why?
```

Damit bleibt sichtbar, ob eine Auswahl automatisch oder explizit beeinflusst wurde.

## Normative Anforderungen

1. `Nova.ExecutionContract` MUSS Preferred und Forced Algorithm ausdrücken können.
2. Preferred Algorithm MUSS als Soft Constraint behandelt werden.
3. Forced Algorithm MUSS als Hard Constraint behandelt werden.
4. Preferred Algorithm DARF durch einen besser geeigneten gültigen Kandidaten ersetzt werden.
5. Bei Forced Algorithm DARF kein automatischer Algorithmus-Fallback erfolgen.
6. Forced Algorithm DARF andere Hard Constraints nicht umgehen.
7. Preferred und Forced Algorithm MÜSSEN mit der benötigten Algorithm Family kompatibel sein.
8. Algorithmusversionen MÜSSEN einschränkbar sein.
9. Algorithmus- und Provider-Constraints MÜSSEN getrennt modelliert bleiben.
10. Forced Algorithm und Forced Provider MÜSSEN kombinierbar sein.
11. Eine inkompatible Forced-Kombination MUSS den Contract als nicht erfüllbar markieren.
12. Ein Forced Algorithm DARF nicht automatisch als deterministisch gelten.
13. Fehler bei Forced Algorithms MÜSSEN strukturiert gemeldet werden.
14. Algorithmus-Overrides MÜSSEN über Introspection nachvollziehbar sein.

## Konsequenzen

### Positive Konsequenzen

- automatische Auswahl bleibt der Standard,
- Anwendungen können Optimierungspräferenzen ausdrücken,
- Debugging und Benchmarking können konkrete Algorithmen erzwingen,
- reproduzierbare Tests können Algorithmusversionen fixieren,
- Overrides umgehen keine Sicherheits- oder Contract-Regeln.

### Negative Konsequenzen

- Forced Algorithms können Optimierungsmöglichkeiten reduzieren,
- nicht verfügbare Forced Algorithms führen bewusst zum Fehlschlag,
- Versionsbindung kann die Portabilität einer Ausführung einschränken.

## Abhängigkeiten

- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-EXECUTION-0002_Semantic_Input_und_Output_Types_im_ExecutionContract`
- `ADR-EXECUTION-0003_Latency_und_Deadline_Requirements`
- `ADR-EXECUTION-0004_Resource_Budgets_im_ExecutionContract`
- `ADR-EXECUTION-0006_Trust_Requirements_im_ExecutionContract`
- `ADR-EXECUTION-0007_Determinism_Requirements_im_ExecutionContract`
- `ADR-ALGORITHM-0001_Algorithm_Families`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-ALGORITHM-0003_Explizites_Algorithmus_Override`
- `ADR-ALGORITHM-0008_Algorithm_Registry`
- `ADR-COMPUTE-0008_Explizites_Provider_Override`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-ALGORITHM-PREFERENCE-0001`
- `NPSPEC-EXECUTION-ALGORITHM-FORCE-0001`
- `NPSPEC-EXECUTION-ALGORITHM-VERSION-0001`
- `NPSPEC-EXECUTION-ALGORITHM-OVERRIDE-VALIDATION-0001`

## Ergebnis

Der Execution Contract unterscheidet klar zwischen Empfehlung und Zwang:

```text
No Override
    ↓
Automatic Selection

Preferred Algorithm
    ↓
Prefer if Contract-Valid
    ↓
Fallback Allowed

Forced Algorithm
    ↓
Use if Contract-Valid
    ↓
No Algorithm Fallback
```

Der zentrale Grundsatz lautet:

```text
Preferred bedeutet:
"Nimm diesen Algorithmus,
wenn er sinnvoll und zulässig ist."

Forced bedeutet:
"Nimm genau diesen Algorithmus
oder führe die Operation nicht aus."

Keines von beiden
darf den Execution Contract umgehen.
```