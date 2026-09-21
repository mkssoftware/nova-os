# ADR-ADAPTIVE-0011 – Context Awareness

## Status

Angenommen

## Kategorie

Adaptive Systems / Context Awareness

## Kontext

Adaptive Entscheidungen in NovaOS hängen nicht nur von historischen Daten ab, sondern auch vom aktuellen Kontext.

Dieselbe Aktion kann abhängig von Situation und Systemzustand unterschiedlich sinnvoll sein.

Beispiele:

```text
Current Task
Workspace
Active Objects
Recent Actions
Device
Time Context
Resource State
Network State
Power State
Execution State
Data Location
```

NovaOS benötigt deshalb ein gemeinsames Modell, um relevanten Kontext für adaptive Entscheidungen bereitzustellen.

## Entscheidung

NovaOS führt **Context Awareness** als gemeinsame Grundlage adaptiver Systemfunktionen ein.

```text
Current State
     +
Relevant History
     +
Environment
     ↓
Context
     ↓
Prediction
     ↓
Adaptive Decision
```

Context beschreibt die für eine konkrete Entscheidung relevante Situation.

## Context Model

Context besteht aus strukturierten, semantisch definierten Informationen.

Beispiele:

```text
User Context
Execution Context
Resource Context
Device Context
Workspace Context
Object Context
Network Context
Power Context
Location Context
Temporal Context
```

Nicht jede adaptive Funktion erhält automatisch Zugriff auf den gesamten verfügbaren Kontext.

## Relevanter Kontext

Adaptive Systeme sollen nur den Kontext verwenden, der für ihre Aufgabe erforderlich ist.

```text
Available Context
       ↓
Relevance Filtering
       ↓
Required Context
       ↓
Adaptive Model
```

Dies reduziert:

```text
Complexity
Data Collection
Privacy Risk
Model Noise
Resource Usage
```

## Kontext und Prediction

Eine Prediction wird grundsätzlich relativ zu ihrem Kontext betrachtet.

```text
Context A
    ↓
Prediction X

Context B
    ↓
Prediction Y
```

Eine Beobachtung aus einem Kontext darf nicht automatisch auf einen anderen Kontext übertragen werden.

## Kontextidentität

Relevante Context States müssen für Prediction und Feedback korrelierbar sein.

```text
Context
   ↓
Prediction
   ↓
Decision
   ↓
Observed Result
   ↓
Prediction Error
```

Dadurch kann NovaOS erkennen, unter welchen Bedingungen eine Vorhersage erfolgreich oder fehlerhaft war.

## Nutzerkontext

Nutzerbezogener Kontext kann beispielsweise umfassen:

```text
Current Workspace
Current Activity
Recent Explicit Actions
Active Objects
Selected Capability
Explicit Preferences
```

Explizite Nutzerentscheidungen besitzen weiterhin Vorrang vor kontextbasierten Vorhersagen.

## Systemkontext

Technischer Kontext kann umfassen:

```text
CPU / GPU Load
Memory Pressure
Storage State
Network Conditions
Battery State
Thermal State
Provider Availability
Current Executions
```

Adaptive Scheduling, Compute, Storage und Power Management können daraus unterschiedliche Teilmengen verwenden.

## Objekt- und Semantic Context

Semantic Types und Object Relationships dürfen Kontext liefern.

Beispiel:

```text
Current Object:
    Image

Related Capability:
    Image Editing

Recent Action:
    Crop

Possible Prediction:
    Resize / Export
```

Semantischer Kontext soll gegenüber rein syntaktischen Heuristiken bevorzugt werden, wenn entsprechende Informationen vorhanden sind.

## Context Scope

Context besitzt eine definierte Reichweite.

Beispiele:

```text
Execution
Session
Workspace
Application
Device
User
System
```

Informationen dürfen nicht ohne explizite Semantik zwischen unterschiedlichen Scopes vermischt werden.

## Context Lifetime

Context kann unterschiedlich langlebig sein.

```text
Immediate
Short-Lived
Session
Persistent
```

Kurzfristiger Kontext darf nicht automatisch zu einer dauerhaften Nutzerpräferenz werden.

## Context Confidence

Kontextinformationen können unterschiedliche Verlässlichkeit besitzen.

Beispiele:

```text
Observed
Declared
Derived
Predicted
Unknown
```

Abgeleiteter oder vorhergesagter Context darf nicht wie explizit beobachteter Zustand behandelt werden.

## Context Changes

Ändert sich der Kontext, dürfen adaptive Entscheidungen neu bewertet werden.

```text
Context A
   ↓
Prediction
   ↓
Context Change
   ↓
Reevaluation
   ↓
New Prediction
```

Bereits gültig ausgeführte Entscheidungen werden dadurch nicht rückwirkend verändert.

## Hard Constraints

Context Awareness beeinflusst ausschließlich Entscheidungen innerhalb des zulässigen Lösungsraums.

```text
Hard Constraints
       ↓
Valid Solution Space
       ↓
Context
       ↓
Adaptive Optimization
```

Context darf keine:

```text
Security Policy
Safety Requirement
Data Sovereignty
Authorization
Hard Deadline
Correctness Requirement
```

abschächen.

## Privacy

Context Awareness folgt dem Prinzip der Datenminimierung.

```text
Collect only required context
        ↓
Use for defined purpose
        ↓
Retain only as required
```

Nutzerbezogener Kontext soll bevorzugt lokal verarbeitet werden.

Sensible Informationen dürfen nicht allein deshalb gesammelt werden, weil sie möglicherweise eine Prediction verbessern könnten.

## Context und Identity

Context ist keine Identity und keine Authorization.

```text
Context
   ≠
Identity
   ≠
Permission
   ≠
Trust
```

Aus Kontextinformationen dürfen keine zusätzlichen Berechtigungen abgeleitet werden.

## Deterministic Mode

Wenn Context eine deterministische Entscheidung beeinflusst, muss der relevante Context State:

```text
Fixed
Captured
Versioned
oder
Ignored
```

werden können.

Dadurch bleiben Entscheidungen reproduzierbar.

## Observability

Relevante adaptive Entscheidungen sollen ihren verwendeten Context referenzieren können.

NovaOS soll nachvollziehen können:

```text
Welcher Context war relevant?
Welche Context-Faktoren beeinflussten die Prediction?
Welche Prediction entstand?
Was trat tatsächlich ein?
```

Sensible Context-Daten müssen dabei geschützt oder abstrahiert werden.

## Normative Anforderungen

1. NovaOS MUSS Context Awareness als gemeinsame Grundlage adaptiver Systemfunktionen unterstützen.
2. Context MUSS strukturiert und semantisch beschreibbar sein.
3. Adaptive Systeme SOLLEN nur für ihre Aufgabe relevanten Context verwenden.
4. Predictions MÜSSEN mit ihrem relevanten Context korrelierbar sein können.
5. Beobachtungen DÜRFEN nicht ohne definierte Semantik zwischen unterschiedlichen Contexts verallgemeinert werden.
6. Context MUSS unterschiedliche Scopes unterstützen.
7. Kurzfristiger Context DARF nicht automatisch als dauerhafte Präferenz interpretiert werden.
8. Observed, Declared, Derived, Predicted und Unknown Context SOLLEN unterscheidbar sein.
9. Context Changes DÜRFEN eine Neubewertung adaptiver Entscheidungen auslösen.
10. Explizite Nutzerentscheidungen MÜSSEN Vorrang vor contextbasierten Predictions besitzen.
11. Context Awareness DARF Hard Constraints nicht verändern oder abschwächen.
12. Context DARF keine Identity, Authorization oder Trust implizieren.
13. Context-Erfassung MUSS dem Prinzip der Datenminimierung folgen.
14. Nutzerbezogener Context SOLL bevorzugt lokal verarbeitet werden.
15. Deterministic Mode MUSS relevanten Context fixieren, erfassen, versionieren oder ignorieren können.
16. Relevante Context-Einflüsse SOLLEN über Decision Tracing nachvollziehbar sein.
17. Ohne adaptive Context-Auswertung MUSS NovaOS weiterhin korrekt funktionieren.

## Konsequenzen

### Positive Konsequenzen

- adaptive Entscheidungen berücksichtigen die aktuelle Situation,
- Vorhersagen können präziser und workloadabhängiger werden,
- unterschiedliche adaptive Subsysteme erhalten ein gemeinsames Context-Modell,
- kurzfristige Situationen werden von dauerhaften Präferenzen getrennt,
- Prediction Error kann contextbezogen ausgewertet werden.

### Negative Konsequenzen

- Context-Erfassung erzeugt zusätzlichen Systemzustand,
- zu viele Context-Faktoren können Modelle unnötig komplex machen,
- Nutzerkontext benötigt besonders strenge Privacy-Regeln.

## Abhängigkeiten

- `ADR-ADAPTIVE-0001_Prediction_Error_als_zentrales_Lernsignal`
- `ADR-ADAPTIVE-0002_Prediction_User_Decision_Deviation_Model_Correction`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`
- `ADR-ADAPTIVE-0005_Adaptive_Caching_und_Prefetching`
- `ADR-ADAPTIVE-0006_Predictive_Preloading`
- `ADR-ADAPTIVE-0007_Adaptive_Memory_Management`
- `ADR-ADAPTIVE-0008_Adaptive_Power_Management`
- `ADR-ADAPTIVE-0009_Adaptive_Networking`
- `ADR-ADAPTIVE-0010_Adaptive_Storage`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`
- `ADR-PRIVACY-0001_Data_Minimization`

## Zugehörige NPSPECs

- `NPSPEC-ADAPTIVE-CONTEXT-0001`
- `NPSPEC-ADAPTIVE-CONTEXT-SCOPE-0001`
- `NPSPEC-ADAPTIVE-CONTEXT-LIFECYCLE-0001`
- `NPSPEC-ADAPTIVE-CONTEXT-CONFIDENCE-0001`
- `NPSPEC-ADAPTIVE-CONTEXT-CORRELATION-0001`

## Ergebnis

NovaOS macht Context zu einem expliziten Bestandteil adaptiver Entscheidungen:

```text
Observe
   ↓
Build Relevant Context
   ↓
Predict
   ↓
Decide
   ↓
Observe Result
   ↓
Prediction Error
   ↓
Context-Aware Learning
```

Der zentrale Grundsatz lautet:

```text
Eine Vorhersage ist nur
im richtigen Kontext sinnvoll.

NovaOS soll deshalb nicht nur lernen,
was in der Vergangenheit geschah,
sondern auch,
unter welchen Bedingungen
es geschah.
```