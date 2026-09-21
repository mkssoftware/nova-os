# ADR-AI-0006 – Privacy-Preserving AI

## Status

Angenommen

## Kategorie

AI / Privacy / Security / Data Governance

## Kontext

AI-Systeme können besonders sensible Daten verarbeiten:

```text
Documents
Messages
Images
Audio
User Context
Personal Preferences
Embeddings
Conversation History
Behavioral Data
```

Dabei entstehen neben Eingaben und Ausgaben zusätzliche Daten wie:

```text
Prompts
Tokens
Embeddings
KV Caches
Intermediate States
Temporary Files
Model Adaptation Data
Telemetry
Logs
```

Privacy darf deshalb nicht nur am API-Eingang betrachtet werden, sondern muss den gesamten AI-Datenfluss umfassen.

## Entscheidung

NovaOS implementiert **Privacy-Preserving AI** als systemweites Prinzip der `Nova.AI Runtime`.

```text
AI Request
    ↓
Data Classification
    ↓
Authorization
    ↓
Privacy + Sovereignty Constraints
    ↓
Minimum Required Data
    ↓
AI Execution
    ↓
Controlled Output
    ↓
Cleanup / Retention
```

AI erhält grundsätzlich nur die Daten und Rechte, die für die konkrete Ausführung erforderlich sind.

## Data Minimization

NovaOS übergibt einem AI-Modell nur die tatsächlich benötigten Daten.

```text
Available Context
      ↓
Relevance + Permission Filter
      ↓
Minimum Required Context
      ↓
AI Model
```

Die bloße technische Verfügbarkeit von Daten erzeugt keinen legitimen AI-Zugriff.

## Local Processing

Sensible Daten sollen bevorzugt lokal verarbeitet werden, sofern eine geeignete On-Device-Ausführung verfügbar ist.

```text
Sensitive Data
     ↓
Local AI Available?
 ├── Yes → Prefer Local
 └── No  → Evaluate Constraints
```

Lokale Verarbeitung ersetzt jedoch keine Authorization.

## Remote AI

Vor Remote AI muss geprüft werden, welche Daten das Gerät verlassen würden.

```text
Input
  ↓
Privacy Check
  ↓
Sovereignty Check
  ↓
Trust Check
  ↓
Authorization
  ↓
Remote Transfer
```

Ist der Transfer nicht zulässig, ist der Remote Provider kein gültiger Kandidat.

## Context Isolation

AI-Kontexte werden nach Execution, Nutzer und Security Context isoliert.

```text
User A → Private AI Context A

User B → Private AI Context B
```

Shared Models dürfen diese Isolation nicht aufheben.

Insbesondere dürfen nicht implizit geteilt werden:

```text
Prompt History
Conversation State
KV Cache
Private Embeddings
Personal Context
Temporary Outputs
Secrets
```

## Embeddings

Embeddings werden als potenziell sensitive abgeleitete Daten behandelt.

```text
Private Document
      ↓
Embedding
      ↓
Derived Sensitive Data
```

Ein Embedding verliert nicht automatisch die Schutzanforderungen seiner Quelldaten.

Retention, Sharing und Remote Transfer müssen deshalb explizit geregelt werden.

## Temporary AI State

Temporärer AI-Zustand soll eine klar definierte Lebensdauer besitzen.

Beispiele:

```text
KV Cache
Activations
Temporary Tokens
Intermediate Representations
Scratch Buffers
Temporary Files
```

Nach Ende der erforderlichen Nutzung sollen diese Daten freigegeben oder gemäß ihrer Policy behandelt werden.

## Persistent AI Memory

Persistente AI Memory darf nicht implizit aus normalen AI-Interaktionen entstehen.

```text
Execution Context
      ≠
Persistent Memory
```

Persistente Speicherung benötigt eine explizite Policy und einen definierten Zweck.

## Model Adaptation

Nutzerdaten dürfen nicht automatisch für Fine-Tuning oder andere dauerhafte Modellanpassungen verwendet werden.

```text
User Data
   ↓
Normal Inference

≠

Automatic Training
```

Training, Fine-Tuning, LoRA oder andere persistente Anpassungen benötigen eine explizit zulässige Datenverwendung.

## Logging und Tracing

AI Observability darf sensitive Inhalte nicht standardmäßig vollständig erfassen.

Bevorzugt werden Metadaten wie:

```text
Execution ID
Model ID
Provider
Latency
Token Count
Resource Usage
Result Status
```

statt:

```text
Full Prompt
Full Output
Private Context
Secrets
```

Inhaltslogging muss explizit autorisiert sein.

## Provenance

NovaOS soll nachvollziehen können, welche Datenquellen eine AI-Ausführung verwendet hat.

```text
Input Objects
     ↓
AI Execution
     ↓
Output Object
```

Provenance darf dabei selbst keine unnötige Kopie sensibler Inhalte erzeugen.

Referenzen auf Object IDs und Version IDs sind gegenüber vollständigen Datenkopien zu bevorzugen.

## Output Handling

AI-Ausgaben können Informationen aus geschützten Eingaben enthalten.

Daher gilt:

```text
Protected Input
      ↓
AI Transformation
      ↓
Output may remain protected
```

Eine AI-Transformation entfernt nicht automatisch Privacy-, Security- oder Sovereignty-Anforderungen.

## Information Flow

NovaOS muss verhindern können, dass AI als Umgehung bestehender Datenflussregeln verwendet wird.

```text
Protected Data
     ↓
AI Model
     ↓
Untrusted Destination
```

darf nicht allein deshalb zulässig werden, weil zwischen Quelle und Ziel ein AI-Modell liegt.

## Remote Provider Retention

Remote Provider dürfen nur verwendet werden, wenn ihre Datenverarbeitung mit dem Execution Contract vereinbar ist.

Relevante Eigenschaften können sein:

```text
Data Retention
Training Usage
Processing Location
Logging
Storage
Deletion Policy
Trust Level
```

Nicht verifizierbare erforderliche Eigenschaften werden als `Unknown` behandelt.

## User Control

Nutzer müssen relevante AI-Datenflüsse begrenzen können.

Beispiele:

```text
Local AI Only
No Remote AI
Ask Before Remote AI
Do Not Retain Context
Do Not Use for Training
Delete AI Context
```

Diese Vorgaben werden als Policies oder Hard Constraints behandelt.

## Privacy und Model Sharing

Geteilte Modellgewichte sind grundsätzlich von privaten Execution-Daten getrennt.

```text
Shared Model Weights
        +
Private Execution State
        ↓
AI Execution
```

Model Sharing darf nicht zu Context Sharing werden.

## Adaptive Systeme

Adaptive AI-Funktionen dürfen nur die für ihren Zweck zulässigen Daten verwenden.

Prediction Error oder andere Lernsignale rechtfertigen keine unbegrenzte Speicherung von Nutzerinteraktionen.

```text
Required Learning Signal
      ↓
Minimum Necessary Data
      ↓
Adaptive Update
```

## Datenlebenszyklus

AI-Daten erhalten einen expliziten Lebenszyklus.

```text
Collect
   ↓
Use
   ↓
Retain if Required
   ↓
Expire
   ↓
Delete / Cryptographic Erasure
```

Unbegrenzte Speicherung ist kein Standardverhalten.

## Fail-Safe Verhalten

Kann NovaOS nicht feststellen, ob ein AI-Datenfluss zulässig ist:

```text
Privacy State = Unknown
        ↓
Do Not Release Data
```

Für erforderliche Privacy- und Sovereignty-Eigenschaften gilt:

```text
Unknown ≠ Allowed
```

## Observability

Privacy-relevante AI-Entscheidungen müssen nachvollziehbar sein.

Beispiele:

```text
Execution ID
Data Classification
Provider
Execution Location
Privacy Policy
Sovereignty Policy
Data Transfer Decision
Retention Policy
Decision ID
```

Die Observability selbst unterliegt wiederum Data Minimization.

## Normative Anforderungen

1. NovaOS MUSS Privacy als Bestandteil der systemweiten AI Runtime behandeln.
2. AI Executions MÜSSEN dem Prinzip der Data Minimization folgen.
3. AI-Modelle DÜRFEN keine impliziten Rechte auf verfügbare Nutzerdaten erhalten.
4. Sensitive AI-Daten SOLLEN bevorzugt lokal verarbeitet werden, sofern dies sinnvoll möglich ist.
5. Remote AI MUSS Privacy-, Sovereignty-, Trust- und Authorization-Constraints erfüllen.
6. Shared Models DÜRFEN private AI-Kontexte nicht implizit teilen.
7. Embeddings MÜSSEN als potenziell sensitive abgeleitete Daten behandelbar sein.
8. Temporäre AI-Zustände MÜSSEN eine definierbare Lebensdauer besitzen.
9. Persistente AI Memory DARF nicht implizit aus normaler Inferenz entstehen.
10. Nutzerdaten DÜRFEN nicht automatisch für persistentes Model Training oder Fine-Tuning verwendet werden.
11. Sensitive Prompts und Outputs DÜRFEN nicht standardmäßig vollständig geloggt werden.
12. AI Provenance SOLL ohne unnötige Inhaltsduplikation erfassbar sein.
13. AI-Ausgaben MÜSSEN Schutzanforderungen ihrer Quelldaten übernehmen können.
14. AI DARF bestehende Information-Flow-Regeln nicht umgehen.
15. Remote Provider Retention MUSS bei erforderlichen Privacy Constraints bewertbar sein.
16. Nutzer MÜSSEN Remote AI und AI-Retention einschränken können.
17. Adaptive AI DARF nur notwendige Lernsignale speichern.
18. AI-Daten MÜSSEN durch Retention- und Lifecycle-Policies kontrollierbar sein.
19. `Unknown` DARF bei erforderlichen Privacy-Eigenschaften nicht als `Allowed` behandelt werden.
20. Privacy Decisions MÜSSEN mit Decision Tracing korrelierbar sein.
21. Observability für AI MUSS selbst dem Prinzip der Data Minimization folgen.

## Konsequenzen

### Positive Konsequenzen

- AI erhält keinen pauschalen Zugriff auf Nutzerdaten,
- lokale AI kann sensible Daten auf dem Gerät halten,
- Shared Models bleiben von privaten Kontexten getrennt,
- Embeddings und temporäre AI-Zustände werden ausdrücklich berücksichtigt,
- Remote AI kann kontrolliert verwendet werden,
- AI wird in das bestehende Privacy- und Sovereignty-Modell von NovaOS integriert.

### Negative Konsequenzen

- Privacy Checks erhöhen die AI-Ausführungsplanung,
- Remote Provider benötigen zusätzliche beschreibbare Eigenschaften,
- Context Isolation und Lifecycle Management erhöhen die Runtime-Komplexität.

## Abhängigkeiten

- `ADR-AI-0001_Systemweite_AI_Runtime`
- `ADR-AI-0003_Model_Scheduling_und_Caching`
- `ADR-AI-0004_Model_Sharing`
- `ADR-AI-0005_On_Device_AI_bevorzugen_wo_sinnvoll`
- `ADR-EXECUTION-0005_Data_Sovereignty`
- `ADR-EXECUTION-0006_Trust_Requirements`
- `ADR-PRIVACY-0001`
- `ADR-PRIVACY-0002`
- `ADR-PRIVACY-0003`
- `ADR-PRIVACY-0006`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-TRUST-0001_Trust_als_Evidence_basiertes_System`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-OBSERVABILITY-0008_Provenance_Inspection`
- `ADR-AUTONOMY-0008_Autonomie_unter_harten_Safety_Security_und_User_Constraints`

## Zugehörige NPSPECs

- `NPSPEC-AI-PRIVACY-0001`
- `NPSPEC-AI-DATA-MINIMIZATION-0001`
- `NPSPEC-AI-CONTEXT-ISOLATION-0001`
- `NPSPEC-AI-DATA-LIFECYCLE-0001`
- `NPSPEC-AI-REMOTE-DATA-PROTECTION-0001`
- `NPSPEC-AI-EMBEDDING-PRIVACY-0001`

## Ergebnis

NovaOS behandelt Privacy als Teil jedes AI-Datenflusses:

```text
Data
  ↓
Minimize
  ↓
Authorize
  ↓
Apply Privacy + Sovereignty
  ↓
Execute AI
  ↓
Protect Output
  ↓
Expire / Retain / Delete
```

Der zentrale Grundsatz lautet:

```text
AI bekommt nicht alle Daten,
die technisch erreichbar sind.

AI bekommt nur die Daten,
die für die konkrete Aufgabe
notwendig und erlaubt sind.

Lokal wird bevorzugt,
Remote bleibt kontrolliert,
private Kontexte bleiben getrennt
und temporäre AI-Daten
werden nicht automatisch
zu dauerhaftem Wissen.
```