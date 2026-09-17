# ADR-DECLARATIVE-0009 – Constraint Solving für Systemplanung

## Status

Angenommen

## Kategorie

Declarative Architecture / System Planning

## Kontext

Ein deklaratives System kann für dasselbe Ziel mehrere gültige Lösungen besitzen.

Beispielsweise kann eine Capability durch unterschiedliche Provider bereitgestellt werden:

```text
Required Capability
       ↓
 ┌─────┼─────┐
CPU   GPU   Remote
```

Gleichzeitig können Anforderungen an Security, Trust, Data Sovereignty, Determinism, Realtime, Ressourcen oder Locality bestehen.

Eine einfache Prioritätenliste reicht daher für komplexe Systemplanung nicht aus.

## Entscheidung

NovaOS verwendet **Constraint Solving** zur Planung deklarativer Systemzustände und Intents.

```text
Intent / Desired State
          +
Constraints
          +
Available Resources
          ↓
Constraint Solver
          ↓
Valid Plan
```

Der Solver sucht eine Lösung, die alle verpflichtenden Constraints erfüllt.

## Hard Constraints

Hard Constraints dürfen nicht verletzt werden.

Beispiele:

```text
Safety
Security
Data Sovereignty
Hard Realtime
Required Trust
Required Capability
Hardware Requirements
```

Existiert keine gültige Lösung, muss die Planung fehlschlagen.

```text
No Valid Solution
       ↓
Planning Failed
```

NovaOS darf Hard Constraints nicht automatisch abschwächen.

## Soft Constraints

Soft Constraints beschreiben Optimierungsziele oder Präferenzen.

Beispiele:

```text
Performance
Energy Efficiency
Latency
Resource Usage
Locality Preference
User Preference
Adaptive Prediction
```

Sie dürfen nur innerhalb der Menge gültiger Lösungen optimiert werden.

## Planungsreihenfolge

Die grundsätzliche Priorität lautet:

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

Niedrigere Ebenen dürfen höhere Constraints nicht verletzen.

## Solver Input

Der Solver kann Informationen aus mehreren Systembereichen verwenden:

```text
Desired State
Semantic Types
Capabilities
Execution Contracts
Resources
Policies
Trust
Locality
Dependencies
Actual State
```

Diese Informationen müssen über explizite Contracts bereitgestellt werden.

## Provider Selection

Constraint Solving kann zur Auswahl geeigneter Capability Provider verwendet werden.

```text
Capability Requirement
        ↓
Candidate Providers
        ↓
Constraint Filtering
        ↓
Valid Providers
        ↓
Policy Selection
```

Constraint Solving bestimmt zunächst die zulässige Lösungsmenge.

Die Auswahl innerhalb dieser Menge bleibt eine getrennte Policy-Entscheidung.

## Replanning

Ändert sich der Actual State, kann ein bestehender Plan ungültig werden.

```text
Resource Lost
Policy Changed
Provider Failed
       ↓
Replanning
```

Eine erneute Planung muss die aktuell gültigen Constraints verwenden.

## Erklärbarkeit

Planungsentscheidungen müssen nachvollziehbar sein.

NovaOS soll beispielsweise erklären können:

```text
Provider A rejected:
    Trust requirement not satisfied

Provider B rejected:
    Data sovereignty violation

Provider C selected:
    All hard constraints satisfied
```

## Determinismus

Wenn mehrere gleichwertige Lösungen existieren, muss NovaOS im Deterministic Mode eine reproduzierbare Auswahl ermöglichen.

Adaptive oder zufällige Auswahl darf den Deterministic Mode nicht verletzen.

## Normative Anforderungen

1. NovaOS MUSS Constraint Solving für komplexe deklarative Systemplanung unterstützen können.
2. Hard Constraints DÜRFEN nicht verletzt oder automatisch abgeschwächt werden.
3. Soft Constraints DÜRFEN nur innerhalb gültiger Lösungen optimiert werden.
4. Constraint Solving MUSS Desired State, verfügbare Ressourcen und relevante Contracts berücksichtigen können.
5. Provider Selection MUSS Hard Constraints vor Optimierungspräferenzen berücksichtigen.
6. Änderungen relevanter Systemzustände MÜSSEN Replanning auslösen können.
7. Nicht erfüllbare Constraint-Mengen MÜSSEN strukturiert fehlschlagen.
8. Planungsentscheidungen SOLLEN erklärbar sein.
9. Adaptive Policies DÜRFEN keine Hard Constraints überschreiben.
10. Deterministic Mode MUSS reproduzierbare Planungsentscheidungen ermöglichen.

## Konsequenzen

### Positive Konsequenzen

- komplexe Systemanforderungen können gemeinsam geplant werden,
- Security und Safety bleiben harte Grenzen,
- Provider und Ressourcen können dynamisch ausgewählt werden,
- Planung bleibt von konkreten Implementierungen entkoppelt,
- NovaOS kann unterschiedliche Hardware effizient nutzen.

### Negative Konsequenzen

- Constraint Solving erhöht die Komplexität der Systemplanung,
- widersprüchliche Constraints müssen erkannt und verständlich gemeldet werden.

## Abhängigkeiten

- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-DECLARATIVE-0002_State_Reconciliation`
- `ADR-DECLARATIVE-0008_Intent_Based_APIs`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-SEMANTIC-0011_Semantic_Discovery`
- `ADR-SEMANTIC-0012_Semantic_Execution`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-DECLARATIVE-CONSTRAINT-SOLVER-0001`
- `NPSPEC-DECLARATIVE-PLANNER-0001`
- `NPSPEC-DECLARATIVE-REPLANNING-0001`

## Ergebnis

NovaOS plant nicht anhand einzelner fest verdrahteter Entscheidungen, sondern ermittelt eine gültige Lösung aus Anforderungen und verfügbaren Möglichkeiten:

```text
Intent / Desired State
          ↓
Hard Constraints
          ↓
Valid Solution Space
          ↓
Soft Constraints
          ↓
Policy Selection
          ↓
Execution Plan
```

Der zentrale Grundsatz lautet:

```text
Zuerst wird bestimmt,
was erlaubt und möglich ist.

Erst danach wird entschieden,
welche gültige Lösung bevorzugt wird.
```