# ADR-EXECUTION-0009 – Hard und Soft Requirements

## Status

Angenommen

## Kategorie

Execution Architecture / Constraint Model

## Kontext

`Nova.ExecutionContract` kann unterschiedliche Anforderungen an eine Ausführung enthalten.

Nicht alle Anforderungen besitzen dieselbe Bedeutung.

Einige Bedingungen dürfen unter keinen Umständen verletzt werden:

```text
Security
Data Sovereignty
Hard Deadline
Required Determinism
Required Trust
Forced Algorithm
```

Andere beschreiben dagegen Optimierungsziele:

```text
Preferred Latency
Performance
Energy Efficiency
Preferred Algorithm
Preferred Provider
```

NovaOS benötigt deshalb eine eindeutige Trennung zwischen **Hard Requirements** und **Soft Requirements**.

## Entscheidung

Jede relevante Anforderung im `Nova.ExecutionContract` wird als:

```text
Hard Requirement
oder
Soft Requirement
```

klassifiziert.

Die Ausführungsplanung erfolgt grundsätzlich in zwei Phasen:

```text
Candidate Solutions
        ↓
Hard Requirement Filtering
        ↓
Valid Solution Space
        ↓
Soft Requirement Optimization
        ↓
Selected Execution Plan
```

Soft Requirements dürfen niemals Hard Requirements überschreiben.

## Hard Requirements

Hard Requirements definieren Bedingungen, die erfüllt sein müssen.

Beispiele:

```text
Safety
Security
Data Sovereignty
Hard Realtime
Required Determinism
Correctness
Required Precision
Required Trust
Required Hardware
Resource Limit
Forced Algorithm
Forced Provider
```

Kann ein Hard Requirement nicht erfüllt werden, existiert kein gültiger Ausführungsplan.

```text
Hard Requirement Unsatisfied
        ↓
Contract Unsatisfied
```

NovaOS darf den Requirement nicht automatisch abschwächen.

## Soft Requirements

Soft Requirements beschreiben Präferenzen und Optimierungsziele.

Beispiele:

```text
Preferred Latency
Performance
Energy Efficiency
Preferred Locality
Preferred Algorithm
Preferred Provider
Adaptive Policy
User Preference
```

Sie werden ausschließlich innerhalb des durch Hard Requirements erlaubten Lösungsraums ausgewertet.

```text
Valid Solutions
      ↓
Soft Requirements
      ↓
Best Suitable Plan
```

Das Nichterreichen eines Soft Requirements macht eine Ausführung nicht automatisch ungültig.

## Prioritätsordnung

Für konkurrierende Anforderungen gilt als systemweite Grundordnung:

```text
Safety
  ↓
Security
  ↓
Data Sovereignty
  ↓
Hard Realtime
  ↓
Determinism
  ↓
Correctness
  ↓
Explicit User Constraints
  ↓
Resource Budget
  ↓
Performance
  ↓
Energy
  ↓
Adaptive Policy
  ↓
Preference
```

Die Klassifizierung als Hard oder Soft bleibt dabei entscheidend.

Eine niedriger priorisierte Optimierung darf niemals einen höheren Hard Constraint verletzen.

## Constraint Solving

Die Ausführungsplanung verwendet Hard Requirements zuerst zur Bestimmung des gültigen Lösungsraums.

```text
Algorithms
Providers
Locations
Resources
       ↓
Hard Constraints
       ↓
Valid Combinations
       ↓
Soft Optimization
```

Dadurch bleiben Zulässigkeit und Optimierung getrennt.

## Konflikte zwischen Hard Requirements

Hard Requirements können sich gegenseitig ausschließen.

Beispiel:

```text
Forced Provider = RemoteGPU
Data Sovereignty = LocalOnly
```

NovaOS darf einen solchen Konflikt nicht eigenständig auflösen, indem eine der Anforderungen ignoriert wird.

```text
Conflicting Hard Requirements
        ↓
Unsatisfied Contract
```

## Konflikte zwischen Soft Requirements

Soft Requirements dürfen gegeneinander abgewogen werden.

Beispiel:

```text
Lowest Latency
vs.
Lowest Energy
```

Die Selection Policy kann innerhalb des gültigen Lösungsraums entscheiden, welche Kombination den Contract am besten erfüllt.

## Hard und Soft derselben Eigenschaft

Eine Eigenschaft kann je nach Contract unterschiedlich klassifiziert sein.

Beispiel:

```text
Preferred Latency = 20 ms
```

ist ein Soft Requirement.

```text
Maximum Latency = 50 ms
```

ist ein Hard Requirement.

Ebenso:

```text
Preferred Algorithm
vs.
Forced Algorithm
```

oder:

```text
Preferred Provider
vs.
Forced Provider
```

## Unknown State

Für Hard Requirements gilt grundsätzlich:

```text
Unknown ≠ Satisfied
```

Kann NovaOS nicht feststellen, ob ein Hard Requirement erfüllt ist, darf die Ausführung nicht als vertragskonform angenommen werden.

Bei Soft Requirements kann ein unbekannter Wert dagegen als Unsicherheitsfaktor in die Optimierung eingehen.

## Adaptive Systeme

Adaptive Modelle dürfen Soft Requirements und deren Bewertung optimieren.

```text
Prediction
    ↓
Execution
    ↓
Measurement
    ↓
Prediction Error
    ↓
Policy Adjustment
```

Adaptive Systeme dürfen jedoch niemals:

```text
Hard Requirements entfernen
Hard Requirements abschwächen
Hard Requirements umklassifizieren
```

## Replanning

Ändert sich der Systemzustand, kann NovaOS einen neuen Ausführungsplan bestimmen.

```text
State Change
    ↓
Replanning
    ↓
Hard Constraint Filtering
    ↓
Soft Optimization
```

Auch beim Replanning gelten dieselben Hard Requirements.

Fallback darf nicht zu einer stillen Contract-Abschwächung führen.

## Introspection

NovaOS muss nachvollziehbar machen können:

```text
Welche Requirements waren Hard?
Welche Requirements waren Soft?
Welcher Hard Constraint schloss einen Kandidaten aus?
Welche Soft Requirements beeinflussten die Auswahl?
Warum war kein gültiger Plan möglich?
```

Damit bleibt die Entscheidung des Execution Planners erklärbar.

## Normative Anforderungen

1. `Nova.ExecutionContract` MUSS Hard und Soft Requirements unterscheiden.
2. Hard Requirements MÜSSEN vor Soft Requirements ausgewertet werden.
3. Nur Kandidaten, die alle Hard Requirements erfüllen, DÜRFEN in die Optimierung gelangen.
4. Soft Requirements DÜRFEN Hard Requirements niemals überschreiben.
5. Nicht erfüllbare Hard Requirements MÜSSEN zu einem `Unsatisfied Contract` führen.
6. Hard Requirements DÜRFEN nicht automatisch abgeschwächt werden.
7. Konflikte zwischen Hard Requirements MÜSSEN explizit erkannt werden.
8. Soft Requirements DÜRFEN gegeneinander abgewogen werden.
9. Dieselbe Eigenschaft MUSS je nach Contract als Hard oder Soft modellierbar sein.
10. `Unknown` DARF einen Hard Requirement nicht erfüllen.
11. Adaptive Systeme DÜRFEN ausschließlich innerhalb des gültigen Lösungsraums optimieren.
12. Replanning und Fallback MÜSSEN alle Hard Requirements erneut berücksichtigen.
13. Hard- und Soft-Klassifikation MUSS introspektierbar sein.
14. Ausschluss- und Auswahlentscheidungen SOLLEN strukturiert erklärbar sein.

## Konsequenzen

### Positive Konsequenzen

- Zulässigkeit und Optimierung werden klar getrennt,
- Sicherheits- und Sovereignty-Anforderungen können nicht durch Performance verdrängt werden,
- automatische Planung bleibt flexibel,
- widersprüchliche Contracts können eindeutig erkannt werden,
- adaptive Optimierung erhält klare Grenzen.

### Negative Konsequenzen

- zu viele Hard Requirements können den Lösungsraum stark einschränken,
- Anwendungen müssen Requirements korrekt klassifizieren,
- Konflikte können bewusst zum Abbruch einer Ausführung führen.

## Abhängigkeiten

- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-EXECUTION-0003_Latency_und_Deadline_Requirements`
- `ADR-EXECUTION-0004_Resource_Budgets_im_ExecutionContract`
- `ADR-EXECUTION-0005_Data_Sovereignty_im_ExecutionContract`
- `ADR-EXECUTION-0006_Trust_Requirements_im_ExecutionContract`
- `ADR-EXECUTION-0007_Determinism_Requirements_im_ExecutionContract`
- `ADR-EXECUTION-0008_Preferred_und_Forced_Algorithm`
- `ADR-DECLARATIVE-0009_Constraint_Solving_für_Systemplanung`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-ARCH-0014_Explizite_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-HARD-REQUIREMENT-0001`
- `NPSPEC-EXECUTION-SOFT-REQUIREMENT-0001`
- `NPSPEC-EXECUTION-CONSTRAINT-PRIORITY-0001`
- `NPSPEC-EXECUTION-CONSTRAINT-CONFLICT-0001`

## Ergebnis

NovaOS trennt strikt zwischen Bedingungen und Optimierungszielen:

```text
Execution Contract
       ↓
Hard Requirements
       ↓
Valid Solution Space
       ↓
Soft Requirements
       ↓
Optimization
       ↓
Execution Plan
```

Der zentrale Grundsatz lautet:

```text
Hard Requirements bestimmen,
was NovaOS tun darf.

Soft Requirements bestimmen,
welche der zulässigen Möglichkeiten
NovaOS bevorzugen soll.

Optimierung findet niemals
außerhalb des gültigen Lösungsraums statt.
```