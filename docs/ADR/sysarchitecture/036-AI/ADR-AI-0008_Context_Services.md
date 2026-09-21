# ADR-AI-0008 – Context Services

## Status

Angenommen

## Kategorie

AI / Context / System Services

## Kontext

AI-Funktionen benötigen häufig Kontext, um sinnvolle Ergebnisse zu erzeugen.

Mögliche Kontextquellen sind:

```text
Current Task
Active Workspace
Open Objects
User Selection
Application State
Recent Actions
Device State
System State
Time
Available Capabilities
Execution History
```

Ohne gemeinsame Context Services würden Anwendungen Kontext selbst sammeln, aufbereiten und an Modelle übergeben.

Dies würde zu:

```text
Duplicate Context Logic
Uncontrolled Data Access
Oversharing
Inconsistent Context
Tight Application Coupling
```

führen.

AI darf außerdem nicht automatisch Zugriff auf alle verfügbaren System- oder Nutzerdaten erhalten.

## Entscheidung

NovaOS erhält systemweite **Context Services**, die autorisierten AI- und Systemfunktionen strukturierten, semantischen und minimal notwendigen Kontext bereitstellen.

```text
System State
User Activity
Workspace
Objects
Applications
Capabilities
      ↓
Context Services
      ↓
Authorization
Privacy Filtering
Relevance Filtering
      ↓
Context View
      ↓
AI Capability
```

Context Services sind die kontrollierte Vermittlungsschicht zwischen Kontextquellen und AI-Ausführung.

## Context Model

Kontext wird strukturiert statt als unkontrollierter Datenblock bereitgestellt.

Beispiele:

```text
Context
 ├── Execution
 ├── Workspace
 ├── Application
 ├── Object
 ├── Device
 ├── Resource
 ├── Temporal
 └── User Interaction
```

Kontextdaten sollen soweit möglich Semantic Types verwenden.

## Context View

Eine AI-Ausführung erhält keinen vollständigen globalen Kontext, sondern eine für die konkrete Aufgabe erzeugte `Context View`.

```text
Available Context
       ↓
Authorization
       ↓
Privacy Constraints
       ↓
Relevance Selection
       ↓
Context View
```

Die Context View enthält nur den für die jeweilige Ausführung zulässigen Kontext.

## Context Scope

Kontext besitzt einen expliziten Scope.

Beispiele:

```text
Execution
Session
Application
Workspace
Device
User
System
```

Context Services dürfen Informationen verschiedener Scopes nicht unkontrolliert vermischen.

## Context Lifetime

Kontext besitzt eine definierte Lebensdauer.

```text
Immediate
Execution
Session
Workspace
Persistent
```

Kurzfristiger Kontext darf nicht automatisch zu persistentem AI Memory werden.

```text
Context ≠ Memory
```

## Context Sources

Context Services dürfen Informationen aus unterschiedlichen Systemquellen aggregieren.

Beispiele:

```text
System State Graph
Object Metadata
Active Workspace
Execution State
Resource State
Capability Registry
Application State
Explicit User Input
```

Jede Quelle bleibt weiterhin Eigentümer ihres tatsächlichen Zustands.

Context Services erzeugen lediglich eine Sicht darauf.

## Context Discovery

AI Capabilities dürfen beschreiben, welchen Kontext sie benötigen.

Beispiel:

```text
AI Capability
    ↓
Context Requirements
    ↓
Context Discovery
    ↓
Authorized Context View
```

Dadurch muss ein Modell nicht selbstständig das System durchsuchen.

## Context Requirements

Eine Capability kann Kontext semantisch anfordern.

Beispiele:

```text
SelectedText
ActiveDocument
CurrentWorkspace
CurrentLanguage
AvailableTools
DeviceCapabilities
RelevantObjects
```

Die Anforderung beschreibt, **welche Information benötigt wird**, nicht wo diese physisch gespeichert ist.

## Relevance Filtering

Nicht jeder verfügbare Kontext ist für jede AI-Aufgabe relevant.

```text
Available Context
       ↓
Relevance Evaluation
       ↓
Relevant Context
```

Dadurch werden:

```text
Token Usage
Data Exposure
Processing Cost
Noise
```

reduziert.

## Data Minimization

Context Services folgen dem Prinzip:

```text
Minimum Necessary Context
```

Ein AI-Modell erhält nicht automatisch:

```text
All Files
All Messages
Complete History
All Open Applications
Entire User Profile
```

nur weil diese Daten technisch erreichbar wären.

## Authorization

Jeder Context Access unterliegt bestehenden Berechtigungen.

```text
AI Execution
     +
Requested Context
     +
Authorization
     ↓
Allowed Context
```

Context Services erzeugen keine zusätzlichen Rechte.

## Privacy

Privacy Policies werden vor der Bereitstellung des Context Views angewendet.

```text
Context Source
      ↓
Authorization
      ↓
Privacy Policy
      ↓
Minimization
      ↓
AI Context
```

Sensitive Kontextinformationen können:

```text
Excluded
Reduced
Redacted
Referenced
```

werden.

## Object References

Wo möglich, sollen Context Services stabile Object References statt unnötiger Datenkopien verwenden.

```text
Object ID
Version ID
Semantic Type
Relevant Metadata
```

Die eigentlichen Inhalte werden nur bereitgestellt, wenn sie für die AI-Ausführung erforderlich und autorisiert sind.

## Context Freshness

Kontext kann veralten.

Context Services müssen deshalb Freshness ausdrücken können.

```text
Current
Stale
Unknown
```

Ein AI-Modell darf veralteten Kontext nicht automatisch als aktuellen Systemzustand interpretieren.

## Context Confidence

Abgeleiteter Kontext kann eine Confidence besitzen.

```text
Observed
Declared
Derived
Predicted
Unknown
```

Insbesondere muss zwischen beobachtetem und vorhergesagtem Kontext unterschieden werden.

## Context Updates

Bei relevanten Zustandsänderungen darf eine Context View aktualisiert oder invalidiert werden.

```text
System State Change
       ↓
Context Change
       ↓
Update / Invalidate View
```

Laufende AI-Ausführungen bestimmen über ihren Execution Contract, ob dynamische Context Updates zulässig sind.

## AI Context und User Intent

Explizite Nutzeraktionen besitzen besondere Bedeutung.

Beispiele:

```text
Selected Object
Selected Text
Current Command
Explicit Prompt
Active Workspace
```

Expliziter Kontext besitzt gegenüber lediglich vorhergesagtem Kontext höhere Autorität.

```text
Explicit Context
      >
Inferred Context
      >
Predicted Context
```

## Context Isolation

Context Views verschiedener Executions bleiben logisch getrennt.

```text
Execution A → Context View A

Execution B → Context View B
```

Shared Models dürfen diese Trennung nicht aufheben.

## Context und AI Memory

Context Services und persistente AI Memory bleiben getrennte Konzepte.

```text
Context
    = Current relevant information

Memory
    = Explicitly retained information
```

Kontext darf nur über eine explizite Memory Policy dauerhaft gespeichert werden.

## Remote AI

Vor Remote Execution muss die Context View erneut gegen Data Sovereignty und Privacy geprüft werden.

```text
Context View
     ↓
Remote Transfer Check
     ↓
Allowed Subset
     ↓
Remote AI
```

Lokaler Zugriff bedeutet nicht automatisch, dass derselbe Kontext das Gerät verlassen darf.

## Context Caching

Context Views dürfen kurzzeitig gecacht werden, wenn:

```text
Authorization
Privacy
Lifetime
Freshness
Version
```

dies erlauben.

Ein Cache darf keine abgelaufenen Berechtigungen oder veralteten Daten konservieren.

## Deterministic Mode

Für deterministische AI-Ausführungen muss der verwendete Kontext reproduzierbar sein können.

Dazu können fixiert werden:

```text
Context View ID
Object Versions
Context Version
Relevant System State
Context Selection Rules
```

Dynamische Context Updates müssen für solche Ausführungen kontrollierbar sein.

## Observability

Context-Nutzung muss nachvollziehbar sein, ohne sensible Inhalte unnötig zu protokollieren.

Relevante Informationen sind:

```text
Execution ID
Context View ID
Context Types
Source References
Authorization Result
Freshness
Context Version
Redaction
Decision ID
```

Vollständige private Inhalte werden nicht standardmäßig geloggt.

## Normative Anforderungen

1. NovaOS MUSS systemweite Context Services für AI bereitstellen können.
2. AI-Funktionen SOLLEN Kontext über Context Services statt durch unkontrollierte Systemsuche beziehen.
3. Context Services MÜSSEN strukturierte Context Views erzeugen können.
4. Context Views MÜSSEN an eine konkrete Execution und deren Berechtigungen bindbar sein.
5. Context Access MUSS bestehende Authorization berücksichtigen.
6. Context Services DÜRFEN keine zusätzlichen Datenzugriffsrechte erzeugen.
7. Context Selection MUSS Privacy und Data Minimization berücksichtigen.
8. AI Capabilities SOLLEN semantische Context Requirements deklarieren können.
9. Kontext SOLL nach Relevanz gefiltert werden.
10. Context Scope und Context Lifetime MÜSSEN unterscheidbar sein.
11. Kurzfristiger Kontext DARF nicht automatisch zu persistenter AI Memory werden.
12. Context Services SOLLEN stabile Object References gegenüber unnötigen Datenkopien bevorzugen.
13. Context Freshness MUSS ausdrückbar sein.
14. Beobachteter, deklarierter, abgeleiteter und vorhergesagter Kontext MÜSSEN unterscheidbar sein.
15. Expliziter Nutzerkontext MUSS Vorrang vor lediglich vorhergesagtem Kontext besitzen.
16. Context Views verschiedener Executions MÜSSEN logisch isoliert bleiben.
17. Remote AI MUSS Context Data erneut gegen Privacy- und Sovereignty-Constraints prüfen.
18. Context Caches DÜRFEN abgelaufene Berechtigungen nicht umgehen.
19. Deterministic Mode MUSS verwendeten Kontext reproduzierbar machen können.
20. Context-Nutzung MUSS mit Decision Tracing korrelierbar sein.
21. Sensitive Context-Inhalte DÜRFEN nicht standardmäßig vollständig geloggt werden.
22. Context Services MÜSSEN auch außerhalb generativer AI für andere autorisierte Systemfunktionen wiederverwendbar sein.

## Konsequenzen

### Positive Konsequenzen

- AI erhält strukturierten statt unkontrollierten Systemzugriff,
- Anwendungen müssen Kontext nicht jeweils selbst zusammensuchen,
- Data Minimization wird zentral durchsetzbar,
- Kontext kann semantisch zwischen unterschiedlichen AI-Modellen verwendet werden,
- Remote AI erhält nur den tatsächlich zulässigen Context Subset,
- Context Services können auch von anderen NovaOS-Capabilities genutzt werden.

### Negative Konsequenzen

- Context Selection und Freshness erhöhen die Runtime-Komplexität,
- Context Sources benötigen semantische Schnittstellen,
- dynamischer Kontext erfordert Invalidierungs- und Versionsmechanismen.

## Abhängigkeiten

- `ADR-AI-0001_Systemweite_AI_Runtime`
- `ADR-AI-0004_Model_Sharing`
- `ADR-AI-0005_On_Device_AI_bevorzugen_wo_sinnvoll`
- `ADR-AI-0006_Privacy_Preserving_AI`
- `ADR-ADAPTIVE-0002_Prediction_User_Decision_Deviation_Model_Correction`
- `ADR-ADAPTIVE-0003_Explizite_Nutzerentscheidung_hat_Vorrang_vor_Adaptation`
- `ADR-ADAPTIVE-0011_Context_Awareness`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-OBJECT-0002`
- `ADR-EXECUTION-0005_Data_Sovereignty`
- `ADR-PRIVACY-0001`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`

## Zugehörige NPSPECs

- `NPSPEC-AI-CONTEXT-SERVICE-0001`
- `NPSPEC-AI-CONTEXT-VIEW-0001`
- `NPSPEC-AI-CONTEXT-REQUIREMENTS-0001`
- `NPSPEC-AI-CONTEXT-LIFETIME-0001`
- `NPSPEC-AI-CONTEXT-FRESHNESS-0001`
- `NPSPEC-AI-CONTEXT-ISOLATION-0001`

## Ergebnis

NovaOS stellt AI keinen unkontrollierten globalen Kontext bereit:

```text
System Context
      ↓
Context Services
      ↓
Authorization
      ↓
Privacy
      ↓
Relevance
      ↓
Minimum Context View
      ↓
AI Execution
```

Der zentrale Grundsatz lautet:

```text
AI darf wissen,
was sie für die aktuelle Aufgabe
wissen muss und wissen darf.

Nicht alles,
was NovaOS über den aktuellen
Systemzustand wissen könnte.

Kontext wird deshalb
semantisch ausgewählt,
minimal bereitgestellt,
zeitlich begrenzt
und an die jeweilige
Ausführung gebunden.
```