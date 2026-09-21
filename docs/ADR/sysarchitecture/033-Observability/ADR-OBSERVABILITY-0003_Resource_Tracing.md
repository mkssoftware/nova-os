# ADR-OBSERVABILITY-0003 – Resource Tracing

## Status

Angenommen

## Kategorie

Observability Architecture / Resource Tracing

## Kontext

NovaOS plant und verwaltet Ressourcen systemweit über `Nova.ExecutionContract` und die Resource Economy.

Eine Ausführung kann Ressourcen auf mehreren Ebenen verbrauchen:

```text
CPU
Memory
Storage
Network
I/O
GPU / NPU / Accelerator
Energy
Remote Resources
```

Für Performance-Analyse, Resource Accounting, Capacity Planning und Self-Healing muss nachvollziehbar sein, **welche Ausführung welche Ressourcen verwendet hat**.

## Entscheidung

NovaOS führt **Resource Tracing** als Bestandteil der Unified Observability Architecture ein.

```text
Execution
    ↓
Resource Allocation
    ↓
Resource Usage
    ↓
Resource Trace
    ↓
Accounting / Analysis
```

Ressourcenverbrauch wird mit der verursachenden Execution, Capability oder Operation korreliert.

## Resource Context

Resource Events können insbesondere folgende Identitäten referenzieren:

```text
Execution ID
Trace ID
Span ID
Process / Workload ID
Capability ID
Algorithm ID
Provider ID
Node ID
Resource ID
```

Dadurch kann Ressourcenverbrauch über Systemgrenzen hinweg einer logischen Ausführung zugeordnet werden.

## Ressourcenarten

Resource Tracing muss unterschiedliche Ressourcenklassen unterstützen können.

Beispiele:

```text
CPU Time
Memory Allocation
Storage I/O
Network Traffic
GPU Time
NPU Time
Accelerator Time
Energy
Reserved Resources
```

Neue Ressourcentypen müssen erweiterbar bleiben.

## Allocation und Usage

NovaOS unterscheidet zwischen reservierten und tatsächlich verwendeten Ressourcen.

```text
Requested
    ↓
Reserved
    ↓
Allocated
    ↓
Used
    ↓
Released
```

Diese Zustände dürfen nicht gleichgesetzt werden.

## Execution Contract

Resource Tracing wird mit Resource Budgets des Execution Contracts korreliert.

```text
Resource Budget
      ↓
Execution
      ↓
Measured Usage
      ↓
Budget Comparison
```

Dadurch können Abweichungen zwischen geplantem und tatsächlichem Verbrauch erkannt werden.

## Hierarchische Zuordnung

Ressourcenverbrauch verschachtelter Ausführungen muss hierarchisch zuordenbar sein.

```text
Execution A
 ├── Execution B
 │    ├── CPU
 │    └── Memory
 └── Execution C
      ├── GPU
      └── Network
```

Der Gesamtverbrauch einer Parent Execution kann aus den relevanten Child Executions aggregiert werden.

## Provider Resources

Resource Tracing umfasst auch Compute Provider.

```text
Algorithm
    ↓
Provider
    ↓
CPU / GPU / NPU / DSP / FPGA
    ↓
Measured Resource Usage
```

Damit können Algorithmus-Provider-Kombinationen anhand realer Messwerte bewertet werden.

## Distributed Resources

Bei Distributed Execution werden lokale und entfernte Ressourcen gemeinsam betrachtet.

```text
Local Compute
     +
Network Transfer
     +
Remote Compute
     +
Remote Storage
     ↓
Distributed Resource Trace
```

Remote Compute darf nicht als ressourcenfrei erscheinen.

## Data Movement

Datenbewegung ist ein explizit beobachtbarer Ressourcenverbrauch.

Beispiele:

```text
Memory Copy
Device Transfer
Network Transfer
Storage Read / Write
Remote Object Transfer
```

Dadurch können unnötige Kopien oder ungünstige Compute-to-Data-Entscheidungen erkannt werden.

## Resource Lifetime

Resource Tracing soll den Lebenszyklus relevanter Ressourcen sichtbar machen können.

```text
Reserve
  ↓
Acquire
  ↓
Use
  ↓
Release
```

Nicht freigegebene Ressourcen können dadurch als mögliche Leaks erkannt werden.

## Profile-Guided Optimization

Gemessener Ressourcenverbrauch kann zukünftige Planungsmodelle verbessern.

```text
Predicted Usage
      ↓
Execution
      ↓
Measured Usage
      ↓
Prediction Error
      ↓
Model Adjustment
```

Diese Informationen dürfen Cost-, Performance- und Resource-Modelle verbessern.

Hard Constraints bleiben unverändert.

## Sampling und Aggregation

Sehr häufige Resource Events dürfen aggregiert oder gesampelt werden.

```text
Raw Events
    ↓
Sampling / Aggregation
    ↓
Resource Metrics / Trace Data
```

Budgetverletzungen, Ressourcenfehler oder andere verbindlich relevante Ereignisse dürfen nicht durch normales Sampling verborgen werden.

## Self-Healing

Resource Tracing liefert Signale für die Erkennung von:

```text
Memory Leaks
Resource Exhaustion
Provider Saturation
Unexpected I/O
Network Bottlenecks
Resource Budget Violations
```

Die Entscheidung über Recovery-Maßnahmen bleibt Aufgabe von `Nova.Resilience`.

## Privacy und Security

Resource Tracing soll technische Nutzungsdaten erfassen, nicht unnötig die verarbeiteten Inhalte.

```text
Resource Usage
      ≠
Payload Content
```

Secrets, Credentials und unnötige Nutzdaten dürfen nicht Bestandteil des Resource Trace werden.

## Normative Anforderungen

1. NovaOS MUSS Ressourcenverbrauch einer logischen Ausführung zuordnen können.
2. Resource Tracing MUSS mit Execution IDs und Trace IDs korrelierbar sein.
3. Reservierte und tatsächlich verwendete Ressourcen MÜSSEN unterscheidbar sein.
4. CPU-, Memory-, Storage-, Network-, I/O- und Accelerator-Ressourcen MÜSSEN erfassbar sein können.
5. Energy Usage MUSS integrierbar sein.
6. Verschachtelte Ausführungen MÜSSEN hierarchisch aggregierbar sein.
7. Compute-Provider-Ressourcen MÜSSEN einer Execution zugeordnet werden können.
8. Distributed Execution MUSS lokale, Netzwerk- und entfernte Ressourcen berücksichtigen.
9. Relevante Datenbewegungen SOLLEN beobachtbar sein.
10. Resource Lifetime SOLL für relevante Ressourcen nachvollziehbar sein.
11. Resource Usage MUSS mit Resource Budgets vergleichbar sein.
12. Budgetverletzungen MÜSSEN als Contract Violation erkennbar sein.
13. Gemessene Resource-Daten DÜRFEN adaptive Planungsmodelle verbessern.
14. Adaptive Modelle DÜRFEN Hard Resource Constraints nicht verändern.
15. Sampling und Aggregation MÜSSEN unterstützt werden können.
16. Verbindlich relevante Resource Events DÜRFEN durch Sampling nicht verborgen werden.
17. Resource Tracing MUSS Privacy-, Security-, Retention- und Data-Sovereignty-Regeln einhalten.
18. Secrets und Credentials DÜRFEN nicht in Resource Traces gespeichert werden.

## Konsequenzen

### Positive Konsequenzen

- Ressourcenverbrauch wird konkreten Ausführungen zuordenbar,
- Resource Budgets können überprüft werden,
- Leaks und Ressourcenengpässe werden leichter erkennbar,
- Algorithmus- und Provider-Modelle erhalten reale Messdaten,
- verteilte Ressourcen werden systemweit sichtbar.

### Negative Konsequenzen

- feingranulares Resource Tracing erzeugt zusätzlichen Overhead,
- hierarchische und verteilte Zuordnung erhöht die Komplexität,
- Sampling und Aggregation müssen sorgfältig abgestimmt werden.

## Abhängigkeiten

- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_Tracing`
- `ADR-OBSERVABILITY-0002_Distributed_Tracing`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-EXECUTION-0004_Resource_Budgets_im_ExecutionContract`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-ALGORITHM-0004_Cost_Performance_Hardware_und_Energy_Models`
- `ADR-ALGORITHM-0006_Profile_Guided_Algorithm_Selection`
- `ADR-COMPUTE-0001_Unified_Compute_Runtime`
- `ADR-DISTRIBUTED-0001_Distributed_Execution`
- `ADR-RESILIENCE-0015_Resource_Exhaustion_Recovery`

## Zugehörige NPSPECs

- `NPSPEC-OBSERVABILITY-RESOURCE-TRACING-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-CONTEXT-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-LIFECYCLE-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-AGGREGATION-0001`

## Ergebnis

NovaOS kann Ressourcenverbrauch über den gesamten Ausführungsweg verfolgen:

```text
Execution Contract
       ↓
Resource Budget
       ↓
Execution Plan
       ↓
Reservation
       ↓
Execution
       ↓
Resource Trace
       ↓
Accounting
       ↓
Verification
       ↓
Profile / Resilience
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll nicht nur wissen,
welche Ressourcen verwendet werden.

Es soll nachvollziehen können,
wer sie verwendet,
warum sie verwendet werden
und welcher Ausführung
der Verbrauch zuzuordnen ist.
```