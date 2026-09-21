# ADR-ADAPTIVE-0001 – Prediction Error als zentrales Lernsignal

## Status

Angenommen

## Kategorie

Adaptive Systems / Learning Architecture

## Kontext

NovaOS soll sich an reale Nutzung, Workloads und Systembedingungen anpassen können.

Adaptive Entscheidungen können beispielsweise betreffen:

```text
Algorithm Selection
Provider Selection
Scheduling
Resource Allocation
Preloading
Caching
Energy Optimization
Latency Prediction
```

Dabei benötigt NovaOS ein gemeinsames Prinzip, um aus falschen oder ungenauen Vorhersagen zu lernen.

## Entscheidung

NovaOS verwendet **Prediction Error** als zentrales Lernsignal für adaptive Systemfunktionen.

Das Grundmodell lautet:

```text
Prediction
    ↓
Decision
    ↓
Actual Result
    ↓
Prediction Error
    ↓
Model Correction
```

Das System lernt damit primär aus der Differenz zwischen erwartetem und tatsächlich beobachtetem Verhalten.

## Prediction Error

Der Prediction Error beschreibt die Abweichung zwischen:

```text
Predicted Value
      und
Observed Value
```

Konzeptionell:

```text
Prediction Error = Observed Result - Predicted Result
```

Die konkrete Fehlerfunktion hängt vom jeweiligen Modell und Datentyp ab.

## Anwendungsbeispiele

### Performance

```text
Predicted Runtime: 10 ms
Actual Runtime:    17 ms
        ↓
Prediction Error
        ↓
Performance Model Update
```

### Ressourcen

```text
Predicted Memory: 200 MiB
Actual Memory:    340 MiB
        ↓
Resource Model Update
```

### Nutzerentscheidung

```text
Predicted Action
      ↓
Actual User Decision
      ↓
Deviation
      ↓
Preference Model Update
```

### Energie

```text
Predicted Energy
      ↓
Measured Energy
      ↓
Prediction Error
      ↓
Energy Model Update
```

## Gemeinsames Lernmodell

Subsysteme dürfen unterschiedliche Modelle verwenden.

Das grundlegende Lernprinzip bleibt jedoch gleich:

```text
Observe
   ↓
Predict
   ↓
Act
   ↓
Measure
   ↓
Compare
   ↓
Adapt
```

NovaOS schreibt keinen einzelnen Machine-Learning-Algorithmus vor.

## Decision Tracing

Prediction und tatsächliches Ergebnis müssen mit der verursachenden Entscheidung korrelierbar sein.

```text
Decision ID
   ├── Prediction
   ├── Decision
   ├── Actual Result
   └── Prediction Error
```

Dadurch bleibt nachvollziehbar, warum ein adaptives Modell verändert wurde.

## Resource und Performance Models

Prediction Error kann bestehende Modelle kontinuierlich verbessern.

Beispiele:

```text
Algorithm Cost Model
Provider Performance Model
Resource Usage Model
Latency Model
Energy Model
Locality Model
```

Neue Beobachtungen korrigieren dabei zukünftige Schätzungen.

## Wiederholte Abweichungen

Ein einzelner Prediction Error muss nicht automatisch eine starke Modelländerung verursachen.

Wiederholte systematische Abweichungen besitzen höhere Bedeutung.

```text
Prediction
   ↓
Repeated Deviation
   ↓
Detected Pattern
   ↓
Model Adjustment
```

Dadurch sollen kurzfristige Ausreißer von tatsächlichen Veränderungen unterschieden werden können.

## Unsicherheit

Adaptive Modelle müssen Unsicherheit berücksichtigen können.

Vorhersagen können beispielsweise klassifiziert werden als:

```text
High Confidence
Medium Confidence
Low Confidence
Unknown
```

Geringe Datenbasis oder stark schwankende Ergebnisse dürfen nicht als sichere Vorhersage behandelt werden.

## Hard Constraints

Adaptive Modelle dürfen ausschließlich innerhalb des durch Hard Requirements erlaubten Lösungsraums arbeiten.

```text
Hard Constraints
      ↓
Valid Solution Space
      ↓
Adaptive Prediction
      ↓
Optimization
```

Prediction Error darf niemals verwendet werden, um:

```text
Security
Safety
Data Sovereignty
Hard Realtime
Required Determinism
Correctness
Trust Requirements
```

automatisch abzuschwächen.

## Deterministic Mode

Im Deterministic Mode darf Online-Learning die Reproduzierbarkeit nicht unkontrolliert verändern.

Adaptive Modelle müssen deshalb:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

Der verwendete Modellzustand muss reproduzierbar referenzierbar sein.

## Cold Start

Existieren noch keine ausreichenden Erfahrungswerte, verwendet NovaOS:

```text
Static Models
Declared Properties
Safe Defaults
Hardware Models
```

Adaptive Optimierung darf keine Voraussetzung für die korrekte Grundfunktion des Systems sein.

## Modellalterung

Historische Beobachtungen können mit der Zeit an Aussagekraft verlieren.

NovaOS darf deshalb berücksichtigen:

```text
Hardware Changes
Software Updates
Algorithm Updates
Provider Changes
Workload Changes
Environment Changes
```

Veraltete Erfahrungswerte dürfen nicht unbegrenzt als aktuell behandelt werden.

## Privacy

Adaptive Modelle sollen bevorzugt technische und abstrahierte Signale verwenden.

Personenbezogene oder sensible Daten dürfen nur verwendet werden, wenn dies durch die geltenden Privacy- und Security-Regeln zulässig ist.

Für Nutzeradaptivität gilt insbesondere:

```text
Minimize Data
Prefer Local Learning
Respect User Decisions
No Hidden Constraint Weakening
```

## Introspection

NovaOS soll für relevante adaptive Entscheidungen darstellen können:

```text
Welche Vorhersage wurde getroffen?
Welche tatsächliche Beobachtung trat ein?
Wie groß war die Abweichung?
Welches Modell wurde beeinflusst?
Welche Modellversion wurde verwendet?
```

## Normative Anforderungen

1. NovaOS MUSS Prediction Error als gemeinsames Lernsignal für adaptive Systemfunktionen unterstützen.
2. Prediction und tatsächliches Ergebnis MÜSSEN miteinander korrelierbar sein.
3. Adaptive Modelle MÜSSEN aus wiederholten relevanten Abweichungen lernen können.
4. Unterschiedliche Subsysteme DÜRFEN unterschiedliche Fehlerfunktionen verwenden.
5. NovaOS DARF keinen einzelnen ML-Algorithmus als systemweite Voraussetzung erzwingen.
6. Prediction Error MUSS mit relevanten Decision IDs und Execution IDs korrelierbar sein können.
7. Adaptive Modelle MÜSSEN Unsicherheit darstellen können.
8. `Unknown` DARF nicht als sichere Vorhersage behandelt werden.
9. Adaptive Optimierung DARF ausschließlich innerhalb gültiger Hard Constraints erfolgen.
10. Prediction Error DARF Hard Requirements niemals automatisch abschwächen.
11. Deterministic Mode MUSS einen festen, versionierten oder deaktivierten adaptiven Modellzustand ermöglichen.
12. Adaptive Modelle DÜRFEN keine Voraussetzung für die korrekte Grundfunktion von NovaOS sein.
13. Änderungen an Hardware, Software oder Workloads MÜSSEN bei der Gültigkeit historischer Modelle berücksichtigt werden können.
14. Adaptive Systeme MÜSSEN Privacy-, Security- und Data-Sovereignty-Regeln einhalten.
15. Relevante Prediction Errors und Modellanpassungen SOLLEN introspektierbar sein.

## Konsequenzen

### Positive Konsequenzen

- adaptive Subsysteme erhalten ein gemeinsames Lernprinzip,
- Vorhersagemodelle verbessern sich anhand realer Ergebnisse,
- systematische Fehlvorhersagen können automatisch korrigiert werden,
- Performance-, Ressourcen- und Nutzeradaptivität folgen derselben Grundarchitektur,
- Lernen bleibt von konkreten ML-Technologien unabhängig.

### Negative Konsequenzen

- Vorhersagen und reale Ergebnisse müssen zuverlässig korreliert werden,
- veraltete Trainingssignale können Modelle verschlechtern,
- adaptive Zustände benötigen Versionierung und Lifecycle-Regeln.

## Abhängigkeiten

- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0020_KI_nicht_Kernel_Voraussetzung`
- `ADR-EXECUTION-0009_Hard_und_Soft_Requirements`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-ALGORITHM-0004_Cost_Performance_Hardware_und_Energy_Models`
- `ADR-ALGORITHM-0006_Profile_Guided_Algorithm_Selection`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0006_Explainable_Scheduling`
- `ADR-OBSERVABILITY-0007_Explainable_Resource_Allocation`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-PREDICTION-ERROR-0001`
- `NPSPEC-ADAPTIVE-PREDICTION-OBSERVATION-0001`
- `NPSPEC-ADAPTIVE-MODEL-CORRECTION-0001`
- `NPSPEC-ADAPTIVE-MODEL-UNCERTAINTY-0001`
- `NPSPEC-ADAPTIVE-MODEL-VERSIONING-0001`

## Ergebnis

NovaOS verwendet einen gemeinsamen Feedback-Zyklus für adaptive Systeme:

```text
Prediction
    ↓
Decision
    ↓
Reality
    ↓
Prediction Error
    ↓
Learning
    ↓
Better Prediction
```

Der zentrale Grundsatz lautet:

```text
NovaOS lernt nicht daraus,
dass eine Vorhersage getroffen wurde.

Es lernt daraus,
wie stark diese Vorhersage
von der Realität abwich.

Prediction Error ist damit
das zentrale Feedbacksignal
adaptiver Systemfunktionen.
```