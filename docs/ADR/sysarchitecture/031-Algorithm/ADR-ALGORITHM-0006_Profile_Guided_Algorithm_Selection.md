# ADR-ALGORITHM-0006 – Profile-Guided Algorithm Selection

## Status

Angenommen

## Kategorie

Algorithm Architecture / Adaptive Selection

## Kontext

Statische Cost- und Performance-Modelle können die Eignung eines Algorithmus nur begrenzt vorhersagen.

Die tatsächliche Ausführung liefert zusätzliche Informationen über:

```text
Runtime
Memory Usage
Energy Consumption
Cache Behavior
Transfer Cost
Provider Utilization
Input Characteristics
```

NovaOS soll diese realen Messwerte verwenden können, um zukünftige Algorithmusentscheidungen zu verbessern.

## Entscheidung

NovaOS unterstützt **Profile-Guided Algorithm Selection**.

Reale Ausführungsprofile dürfen als Eingabe für zukünftige Algorithmus- und Providerentscheidungen verwendet werden.

```text
Algorithm Selection
       ↓
Execution
       ↓
Runtime Profile
       ↓
Profile Store
       ↓
Selection Model
       ↓
Future Selection
```

Profilinformationen beeinflussen ausschließlich die Optimierung innerhalb der bereits zulässigen Kandidaten.

## Runtime Profiles

Eine Ausführung kann relevante Messwerte erzeugen.

Beispiele:

```text
Execution Time
Memory Consumption
Energy Consumption
Data Transfer
Queueing Time
Throughput
Provider Load
Input Characteristics
```

Es müssen nur für die jeweilige Algorithm Family relevante Daten erfasst werden.

## Profil-Kontext

Messwerte sind nur zusammen mit ihrem Ausführungskontext aussagekräftig.

```text
Algorithm
+
Algorithm Version
+
Input Characteristics
+
Compute Provider
+
Hardware Properties
+
Execution Conditions
        ↓
Runtime Profile
```

Profile unterschiedlicher Bedingungen dürfen nicht ohne geeignete Normalisierung als direkt vergleichbar behandelt werden.

## Profile-Guided Selection

Bei zukünftigen Requests können passende Profile zur Bewertung der Kandidaten verwendet werden.

```text
Current Workload
      ↓
Matching Profiles
      ↓
Expected Behavior
      ↓
Valid Algorithms
      ↓
Algorithm + Provider Selection
```

Profile ergänzen die vorhandenen Cost-, Performance-, Hardware- und Energy-Models.

## Prediction Error

NovaOS vergleicht erwartetes und tatsächliches Verhalten.

```text
Prediction
    ↓
Execution
    ↓
Measured Result
    ↓
Prediction Error
    ↓
Model Correction
```

Wiederholte Abweichungen können die zukünftige Bewertung einer Algorithmus-Provider-Kombination verändern.

## Profilgültigkeit

Profile können durch Änderungen ungültig oder weniger relevant werden.

Beispiele:

```text
Algorithm Update
Driver Update
Hardware Change
Firmware Change
Provider Change
System Configuration Change
```

Profile müssen deshalb versioniert oder mit ihrem relevanten Kontext verknüpft werden.

## Cold Start

Existieren noch keine geeigneten Profile, verwendet NovaOS die statischen Modelle.

```text
Matching Profile?
   ├── Yes → Profile-Guided Evaluation
   └── No  → Static Models
```

Profiling darf keine Voraussetzung für die Ausführbarkeit einer Capability sein.

## Exploration

NovaOS darf alternative gültige Algorithmen gelegentlich messen, um Modelle zu verbessern.

```text
Valid Candidates
      ↓
Controlled Exploration
      ↓
Measurement
      ↓
Profile Improvement
```

Exploration darf nur stattfinden, wenn Execution Contract, Ressourcenbudget und Operationssemantik dies erlauben.

Zeitkritische oder sicherheitskritische Operationen dürfen nicht allein zu Lernzwecken verschlechtert werden.

## Datenschutz

Profile sollen bevorzugt technische Eigenschaften speichern und keine unnötigen Nutz- oder Inhaltsdaten enthalten.

```text
Input Data
    ↓
Relevant Characteristics
    ↓
Runtime Profile
```

Profiling unterliegt den normalen Privacy-, Security- und Data-Sovereignty-Regeln.

## Deterministic Mode

Im Deterministic Mode dürfen dynamisch veränderte Profile nicht zu unkontrolliert unterschiedlichen Auswahlentscheidungen führen.

Für reproduzierbare Ausführungen muss der relevante Profilzustand:

```text
Fixed
Versioned
oder
Ignored
```

werden können.

## Explizite Overrides

Explizite Algorithmus- und Provider-Overrides haben Vorrang vor profilbasierter Soft-Optimierung.

```text
Required Algorithm / Provider
          ↓
Contract Validation
          ↓
Profile-Guided Optimization
of remaining choices
```

Profile dürfen Required Overrides nicht überschreiben.

## Normative Anforderungen

1. NovaOS MUSS Runtime Profiles für Algorithmusentscheidungen verwenden können.
2. Profile MÜSSEN mit ihrem relevanten Ausführungskontext verknüpft werden.
3. Profile SOLLEN Algorithmus-, Provider- und Workload-Eigenschaften gemeinsam abbilden können.
4. Profile DÜRFEN Cost-, Performance-, Hardware- und Energy-Models verbessern.
5. Prediction Error SOLL zur Korrektur zukünftiger Auswahlmodelle verwendet werden können.
6. Veraltete oder inkompatible Profile DÜRFEN nicht ungeprüft weiterverwendet werden.
7. Ohne Profile MUSS eine Auswahl über statische Modelle möglich bleiben.
8. Profilbasierte Optimierung DARF nur innerhalb gültiger Kandidaten erfolgen.
9. Controlled Exploration DARF Hard Constraints nicht verletzen.
10. Profiling MUSS Privacy-, Security- und Data-Sovereignty-Regeln einhalten.
11. Unnötige Inhalts- oder Nutzerdaten DÜRFEN nicht in Compute-Profilen gespeichert werden.
12. Deterministic Mode MUSS einen reproduzierbaren Umgang mit Profilinformationen ermöglichen.
13. Explizite Overrides MÜSSEN Vorrang vor profilbasierter Soft-Optimierung haben.

## Konsequenzen

### Positive Konsequenzen

- Algorithmusauswahl verbessert sich anhand realer Hardware,
- tatsächliche Workloads fließen in zukünftige Entscheidungen ein,
- Cost- und Performance-Modelle können automatisch kalibriert werden,
- wiederkehrende Workloads können zunehmend effizient ausgeführt werden.

### Negative Konsequenzen

- Profile müssen versioniert und invalidiert werden,
- Profiling erzeugt zusätzlichen Mess- und Speicheraufwand,
- veraltete Profile können ohne geeignete Validierung zu schlechten Entscheidungen führen.

## Abhängigkeiten

- `ADR-ALGORITHM-0001_Algorithm_Families`
- `ADR-ALGORITHM-0002_Automatische_Algorithmusauswahl_als_Standard`
- `ADR-ALGORITHM-0004_Cost_Performance_Hardware_und_Energy_Models`
- `ADR-ALGORITHM-0005_Data_Dependent_Algorithm_Selection`
- `ADR-COMPUTE-0009_Providerwahl_unter_Execution_Contracts`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`

## Zugehörige NPSPECs

- `NPSPEC-ALGORITHM-PROFILE-GUIDED-SELECTION-0001`
- `NPSPEC-ALGORITHM-RUNTIME-PROFILE-0001`
- `NPSPEC-ALGORITHM-PROFILE-MATCHING-0001`
- `NPSPEC-ALGORITHM-PROFILE-INVALIDATION-0001`

## Ergebnis

NovaOS kann aus realen Ausführungen lernen und diese Erfahrung für zukünftige Algorithmusentscheidungen verwenden:

```text
Workload
   ↓
Algorithm + Provider
   ↓
Execution
   ↓
Runtime Profile
   ↓
Prediction Error
   ↓
Model Improvement
   ↓
Future Selection
```

Der zentrale Grundsatz lautet:

```text
NovaOS entscheidet nicht nur
anhand theoretischer Modelle.

Es kann aus realen Ausführungen lernen
und zukünftige Algorithmusentscheidungen
auf gemessene Erfahrungen stützen.
```