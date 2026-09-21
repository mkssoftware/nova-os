# ADR-EXECUTION-0010 – Automatische Ausführungsplanung

## Status

Angenommen

## Kategorie

Execution Architecture / Planning

## Kontext

NovaOS beschreibt Ausführungen über `Nova.ExecutionContract`, Capabilities und semantisch typisierte Daten.

Die konkrete Ausführung kann jedoch unterschiedliche Kombinationen verwenden:

```text
Algorithm
Compute Provider
Hardware
Execution Location
Resources
Data Movement
```

Anwendungen sollen diese Entscheidungen nicht selbst treffen müssen.

NovaOS benötigt daher eine gemeinsame automatische Ausführungsplanung.

## Entscheidung

NovaOS verwendet einen **Execution Planner**, der aus Request, Systemzustand und `Nova.ExecutionContract` einen gültigen Ausführungsplan erzeugt.

```text
Execution Request
       ↓
Nova.ExecutionContract
       ↓
Execution Planner
       ↓
Valid Execution Plan
       ↓
Execution
       ↓
Verification
```

Der Planner bestimmt den Ausführungsweg, ohne die fachliche Semantik des Requests zu verändern.

## Planungsgrundlage

Der Execution Planner berücksichtigt insbesondere:

```text
Semantic Input / Output Types
Algorithm Families
Available Algorithms
Compute Providers
Hardware
Resource Availability
Latency / Deadlines
Resource Budgets
Data Sovereignty
Trust
Determinism
Locality
Current System State
```

## Planungsablauf

Die Planung erfolgt logisch in mehreren Schritten:

```text
Capability Request
       ↓
Semantic Compatibility
       ↓
Candidate Algorithms
       ↓
Candidate Providers
       ↓
Candidate Locations
       ↓
Hard Requirement Filtering
       ↓
Valid Solution Space
       ↓
Soft Requirement Optimization
       ↓
Execution Plan
```

Die konkrete Implementierung darf einzelne Schritte gemeinsam optimieren, solange die Semantik erhalten bleibt.

## Hard Constraints

Zuerst werden alle Kombinationen entfernt, die Hard Requirements verletzen.

Beispiele:

```text
Safety
Security
Data Sovereignty
Hard Deadline
Required Determinism
Correctness
Required Trust
Resource Limits
Forced Algorithm
Forced Provider
```

Existiert danach keine gültige Lösung:

```text
No Valid Plan
     ↓
Unsatisfied Contract
```

NovaOS darf den Contract nicht automatisch abschwächen.

## Soft Optimization

Innerhalb des gültigen Lösungsraums optimiert der Planner anhand von Soft Requirements.

Beispiele:

```text
Performance
Preferred Latency
Energy
Data Locality
Preferred Algorithm
Preferred Provider
Adaptive Policy
User Preference
```

Soft Optimization darf niemals einen Hard Constraint verletzen.

## Algorithmus und Provider

Algorithmus- und Providerwahl bleiben logisch getrennte Entscheidungen, werden jedoch gemeinsam geplant.

```text
Algorithm A + CPU
Algorithm A + GPU
Algorithm B + CPU
Algorithm B + NPU
```

Der Planner bewertet gültige Kombinationen statt Algorithmus und Hardware grundsätzlich isoliert zu betrachten.

## Data Locality

Die Position der Daten ist Bestandteil der Planung.

```text
Data Location
     +
Compute Location
     +
Transfer Cost
     ↓
Execution Plan
```

Wenn möglich, soll Compute zu den Daten gebracht werden, statt große Datenmengen unnötig zu bewegen.

Data Sovereignty besitzt dabei Vorrang vor Locality-Optimierung.

## Ressourcen

Benötigt ein Plan garantierte Ressourcen, müssen diese reservierbar sein.

```text
Execution Plan
      ↓
Resource Reservation
      ↓
Plan Valid
```

Eine Ressource darf nicht als verfügbar geplant werden, wenn ihre notwendige Verfügbarkeit nicht gesichert werden kann.

## Dynamischer Systemzustand

Der Planner darf aktuelle Zustände berücksichtigen:

```text
Provider Load
Available Memory
Network State
Thermal State
Energy State
Resource Reservations
Provider Availability
```

Diese Informationen dürfen Hard Requirements nicht verändern.

## Adaptive Planung

Historische Messwerte und Runtime Profiles dürfen die Planung verbessern.

```text
Prediction
    ↓
Execution Plan
    ↓
Measured Result
    ↓
Prediction Error
    ↓
Model Improvement
```

Adaptive Modelle beeinflussen ausschließlich die Auswahl innerhalb gültiger Lösungen.

## Deterministische Planung

Wenn Determinismus gefordert ist, muss auch die Planung reproduzierbar sein.

```text
Same Request
+
Same Contract
+
Same Defined Planning State
       ↓
Same Execution Plan
```

Volatile Zustände oder Profile dürfen dann nur verwendet werden, wenn ihr Einfluss reproduzierbar kontrolliert wird.

## Replanning

Ändert sich der Systemzustand vor oder während der Ausführung, kann Replanning erforderlich werden.

```text
Execution Plan
      ↓
State Change / Failure
      ↓
Replanning
      ↓
New Valid Plan
```

Ein neuer Plan muss den ursprünglichen Execution Contract weiterhin erfüllen.

Retry, Migration oder Algorithmuswechsel sind nur zulässig, wenn die Operationssemantik dies erlaubt.

## Execution Plan

Ein erzeugter Plan kann beispielsweise enthalten:

```text
Capability
Algorithm + Version
Compute Provider
Execution Location
Resource Reservation
Data Movement
Required Conversions
Execution Parameters
Verification Requirements
```

Der Plan ist die konkrete Auflösung des abstrakten Execution Contracts.

## Introspection

Planungsentscheidungen müssen nachvollziehbar sein.

NovaOS soll beantworten können:

```text
Welche Kandidaten existierten?
Welche wurden ausgeschlossen?
Welcher Constraint war verantwortlich?
Warum wurde dieser Algorithmus gewählt?
Warum wurde dieser Provider gewählt?
Welche Optimierungsziele beeinflussten den Plan?
```

## Normative Anforderungen

1. NovaOS MUSS automatische Ausführungsplanung unterstützen.
2. Der Execution Planner MUSS `Nova.ExecutionContract` als verbindliche Planungsgrundlage verwenden.
3. Semantic Compatibility MUSS vor der Ausführung geprüft werden.
4. Hard Requirements MÜSSEN vor Soft Optimization ausgewertet werden.
5. Nur vollständig contract-konforme Lösungen DÜRFEN ausgeführt werden.
6. Algorithmus, Provider und Execution Location MÜSSEN gemeinsam planbar sein.
7. Data Locality und Data Movement SOLLEN berücksichtigt werden.
8. Benötigte garantierte Ressourcen MÜSSEN reservierbar sein.
9. Dynamischer Systemzustand DARF für die Optimierung berücksichtigt werden.
10. Adaptive Modelle DÜRFEN ausschließlich innerhalb gültiger Lösungen optimieren.
11. Deterministische Ausführungen MÜSSEN reproduzierbare Planung ermöglichen.
12. Replanning DARF Hard Requirements nicht abschwächen.
13. Retry, Migration und Algorithmuswechsel MÜSSEN die Operationssemantik berücksichtigen.
14. Ist kein gültiger Plan möglich, MUSS der Contract als nicht erfüllbar gemeldet werden.
15. Planungsentscheidungen MÜSSEN introspektierbar und strukturiert erklärbar sein.

## Konsequenzen

### Positive Konsequenzen

- Anwendungen müssen keine konkrete Hardware auswählen,
- Algorithmus-, Provider- und Standortwahl können gemeinsam optimiert werden,
- Execution Contracts werden systemweit durchsetzbar,
- lokale und verteilte Ressourcen können nach demselben Modell geplant werden,
- adaptive Optimierung bleibt kontrollierbar.

### Negative Konsequenzen

- Planung erzeugt zusätzlichen Laufzeitaufwand,
- zuverlässige Provider-, Algorithmus- und Ressourcenmodelle werden benötigt,
- komplexe Contracts können einen größeren Lösungsraum erzeugen.

## Abhängigkeiten

- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-EXECUTION-0002_Semantic_Input_und_Output_Types_im_ExecutionContract`
- `ADR-EXECUTION-0003_Latency_und_Deadline_Requirements`
- `ADR-EXECUTION-0004_Resource_Budgets_im_ExecutionContract`
- `ADR-EXECUTION-0005_Data_Sovereignty_im_ExecutionContract`
- `ADR-EXECUTION-0006_Trust_Requirements_im_ExecutionContract`
- `ADR-EXECUTION-0007_Determinism_Requirements_im_ExecutionContract`
- `ADR-EXECUTION-0008_Preferred_und_Forced_Algorithm`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-PLANNER-0001`
- `NPSPEC-EXECUTION-PLAN-0001`
- `NPSPEC-EXECUTION-PLAN-OPTIMIZATION-0001`
- `NPSPEC-EXECUTION-REPLANNING-0001`

## Ergebnis

NovaOS übersetzt abstrakte Ausführungsanforderungen automatisch in einen konkreten, gültigen Ausführungsplan:

```text
Intent
  ↓
Capability
  ↓
Nova.ExecutionContract
  ↓
Execution Planner
  ↓
Hard Constraint Filtering
  ↓
Soft Optimization
  ↓
Algorithm + Provider + Location + Resources
  ↓
Execution
  ↓
Verification
```

Der zentrale Grundsatz lautet:

```text
Der Consumer beschreibt,
was benötigt wird
und welche Bedingungen gelten.

NovaOS plant automatisch,
wie, wo und womit
die Ausführung erfolgt.
```