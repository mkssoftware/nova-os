# ADR-EXECUTION-0007 – Determinism Requirements im ExecutionContract

## Status

Angenommen

## Kategorie

Execution Architecture / Determinism

## Kontext

NovaOS kann für dieselbe Capability unterschiedliche Algorithmen, Compute Provider und Ausführungsorte verwenden.

Für viele Workloads ist diese Flexibilität erwünscht. Andere benötigen jedoch reproduzierbares Verhalten.

Beispiele:

```text
Tests
Simulationen
Wissenschaftliche Berechnungen
Debugging
Build-Prozesse
Validierung
Safety-kritische Berechnungen
```

Determinismus muss deshalb als explizite Anforderung im `Nova.ExecutionContract` beschrieben werden können.

## Entscheidung

`Nova.ExecutionContract` unterstützt explizite **Determinism Requirements**.

```text
Execution Request
       ↓
Determinism Requirement
       ↓
Execution Planning
       ↓
Algorithm Selection
       ↓
Provider Selection
       ↓
Execution
       ↓
Verification
```

Ist Determinismus verbindlich gefordert, wird er als Hard Constraint behandelt.

## Determinism Requirement

Ein Contract kann festlegen, ob deterministische Ausführung erforderlich ist.

Beispiel:

```text
Determinism = Required
```

Ohne explizite Anforderung darf NovaOS adaptive und dynamische Optimierungen verwenden.

## Umfang

Determinismus betrifft nicht nur den Algorithmus selbst.

Für eine reproduzierbare Ausführung können relevant sein:

```text
Algorithm Selection
Algorithm Version
Provider Selection
Provider Version
Input State
Execution Order
Parallel Scheduling
Randomness
Profile State
Selection Policy
Hardware Behavior
```

Der erforderliche Umfang hängt von der jeweiligen Operation ab.

## Deterministische Algorithmusauswahl

Bei `Determinism = Required` muss auch die Algorithmusauswahl reproduzierbar sein.

```text
Same Contract
+
Same Relevant Inputs
+
Same Defined State
       ↓
Same Algorithm
```

Dynamische Profile oder volatile Messwerte dürfen die Auswahl nicht unkontrolliert verändern.

## Deterministische Providerwahl

Auch die Providerwahl muss reproduzierbar sein, wenn sie das Ergebnis oder die geforderte Reproduzierbarkeit beeinflusst.

```text
Selected Algorithm
       ↓
Deterministic Provider Filtering
       ↓
Stable Provider Selection
```

Ein Provider darf nur verwendet werden, wenn er die geforderten Determinismus-Eigenschaften unterstützt.

## Parallelität

Parallelität kann zu nicht deterministischen Ausführungsreihenfolgen führen.

```text
Task A ─┐
Task B ─┼→ Scheduling Order → Result
Task C ─┘
```

Wenn die Reihenfolge das Ergebnis beeinflussen kann, muss NovaOS eine deterministische Scheduling- oder Reduktionsstrategie verwenden.

## Zufall

Zufallsbasierte Algorithmen können deterministisch ausgeführt werden, wenn ihre Zufallsquelle kontrolliert wird.

```text
Algorithm
   +
Defined Seed
   +
Defined RNG
      ↓
Reproducible Sequence
```

Ein Seed allein genügt nicht, wenn die verwendete RNG-Implementierung oder deren Version nicht definiert ist.

## Floating Point und Hardware

Unterschiedliche Hardware oder Parallelisierungsstrategien können bei Floating-Point-Berechnungen unterschiedliche Ergebnisse erzeugen.

```text
Same Input
   ↓
CPU / GPU / NPU
   ↓
Different Evaluation Order
   ↓
Potentially Different Result
```

Wenn bitgenaue Reproduzierbarkeit verlangt wird, muss der Execution Contract nur Provider und Algorithmen zulassen, die diese Eigenschaft gewährleisten können.

## Profile-Guided Selection

Runtime Profiles dürfen bei deterministischen Ausführungen nur verwendet werden, wenn ihr Zustand festgelegt ist.

```text
Profile State:
    Fixed
    Versioned
    or Ignored
```

Online-Learning darf eine laufende deterministische Ausführung nicht unkontrolliert beeinflussen.

## Verteilte Ausführung

Remote oder Distributed Execution ist nur zulässig, wenn die geforderten Determinismus-Eigenschaften erhalten bleiben.

```text
Distributed Execution
       ↓
Ordering
Provider State
Algorithm Version
Synchronization
       ↓
Determinism Check
```

Location Transparency darf keine Unterschiede verbergen, die den Contract verletzen würden.

## Determinism und Performance

Determinismus besitzt bei expliziter Anforderung Vorrang vor Performance-Optimierungen.

```text
Determinism Required
       ↓
Valid Deterministic Candidates
       ↓
Performance Optimization
```

Ein schnellerer nichtdeterministischer Algorithmus oder Provider ist dann kein gültiger Kandidat.

## Verification

Soweit technisch überprüfbar, müssen Determinismus-Zusicherungen verifizierbar sein.

NovaOS soll relevante Informationen erfassen können:

```text
Algorithm ID + Version
Provider ID + Version
Selection Policy Version
Profile Version
RNG + Seed
Relevant Execution Parameters
```

Dadurch können Ausführungen später reproduziert oder verglichen werden.

## Normative Anforderungen

1. `Nova.ExecutionContract` MUSS Determinism Requirements ausdrücken können.
2. `Determinism = Required` MUSS als Hard Constraint behandelt werden.
3. Algorithmusauswahl MUSS bei gefordertem Determinismus reproduzierbar sein.
4. Providerwahl MUSS bei gefordertem Determinismus reproduzierbar sein, sofern sie die Reproduzierbarkeit beeinflusst.
5. Nichtdeterministische Kandidaten DÜRFEN einen erforderlichen Determinism Contract nicht erfüllen.
6. Volatile Profile und Runtime-Messwerte DÜRFEN deterministische Entscheidungen nicht unkontrolliert beeinflussen.
7. Zufallsbasierte Ausführungen MÜSSEN kontrollierbare Zufallsquellen verwenden, wenn Reproduzierbarkeit gefordert ist.
8. Parallelität MUSS deterministische Semantik erhalten, wenn der Contract dies verlangt.
9. Hardwareabhängige numerische Unterschiede MÜSSEN bei entsprechenden Determinismusanforderungen berücksichtigt werden.
10. Remote und Distributed Execution DÜRFEN Determinism Requirements nicht abschwächen.
11. Performance- und Energy-Optimierungen DÜRFEN erforderlichen Determinismus nicht überstimmen.
12. Relevante Versionen und Ausführungsparameter SOLLEN für reproduzierbare Ausführungen nachvollziehbar sein.
13. Kann geforderter Determinismus nicht gewährleistet werden, MUSS der Execution Contract als nicht erfüllbar gelten.

## Konsequenzen

### Positive Konsequenzen

- reproduzierbare Ausführungen werden systemweit planbar,
- Tests, Simulationen und wissenschaftliche Berechnungen werden nachvollziehbarer,
- Algorithmus- und Providerwahl werden Teil der Reproduzierbarkeit,
- adaptive Optimierung bleibt für nicht deterministisch gebundene Workloads erhalten.

### Negative Konsequenzen

- Determinismus kann Performance und Parallelität einschränken,
- bestimmte Hardware oder Provider können ausgeschlossen werden,
- zusätzliche Versions- und Zustandsinformationen müssen kontrolliert werden.

## Abhängigkeiten

- `ADR-EXECUTION-0001_Nova_ExecutionContract`
- `ADR-EXECUTION-0002_Semantic_Input_und_Output_Types_im_ExecutionContract`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ALGORITHM-0007_Deterministic_Algorithm_Selection`
- `ADR-ALGORITHM-0006_Profile_Guided_Algorithm_Selection`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`

## Zugehörige NPSPECs

- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `NPSPEC-EXECUTION-DETERMINISTIC-STATE-0001`
- `NPSPEC-EXECUTION-DETERMINISTIC-RANDOMNESS-0001`
- `NPSPEC-EXECUTION-DETERMINISM-VERIFICATION-0001`

## Ergebnis

Determinismus wird zu einer expliziten Eigenschaft des Execution Contracts:

```text
Execution Request
       ↓
Determinism Requirement
       ↓
Deterministic Planning
       ↓
Algorithm + Version
       ↓
Provider + Version
       ↓
Controlled Execution
       ↓
Reproducible Result
```

Der zentrale Grundsatz lautet:

```text
Reproduzierbarkeit betrifft nicht nur
den verwendeten Algorithmus.

Wenn der Execution Contract
Determinismus verlangt,
muss der gesamte relevante
Ausführungsweg deterministisch sein.
```