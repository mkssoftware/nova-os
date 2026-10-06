# NPSPEC-APP-INTENTCENTRIC-0001 – Nova Intent-Centric App

## Status

Angenommen

## Kategorie

App / Intent-Centric

## Zweck

NovaOS definiert ein intentzentriertes App-Modell, bei dem die Absicht des Nutzers vor der Auswahl einer konkreten App, Capability oder Implementierung steht.

Ein Intent beschreibt, **was** erreicht werden soll. NovaOS bestimmt anschließend, **wie** diese Absicht unter den aktuellen Bedingungen sicher und effizient umgesetzt werden kann.

## Grundprinzipien

```text
Intent ≠ Task
Intent ≠ Command
Intent ≠ App
Intent ≠ Capability
Intent ≠ Execution Plan
Intent ≠ Authority
Intent ≠ AI Prompt
```

## Modell

```text
Intent
├── IntentID
├── Type
├── Goal
├── Inputs[]
├── DesiredOutputs[]
├── Constraints
├── Context
└── Parameters
```

`IntentID` beschreibt die semantische Art einer Absicht.

Beispiele:

```text
image.resize
document.translate
media.play
file.share
document.print
message.send
data.visualize
```

## Intent-Auflösung

```text
Intent
  +
Context
  +
Objects
  +
Constraints
    ↓
Intent Resolution
    ↓
Candidate Capabilities
    ↓
Task / Execution Plan
```

Die Auflösung darf weder eine bestimmte App noch einen bestimmten Provider voraussetzen.

## Intent und Task

Intent und Task bleiben getrennte Konzepte.

```text
Intent
   ↓
Resolution
   ↓
Task
   ↓
Execution Plan
   ↓
Execution
```

Ein Intent beschreibt das gewünschte Ergebnis.

Ein Task beschreibt die konkrete laufende Aufgabe, die zur Erfüllung dieses Intents erzeugt wurde.

Ein Intent kann mehrere Tasks erzeugen.

## Intent-Quellen

Intents können entstehen durch:

```text
UI Action
Commandbar
Natural Language
Object Action
Solution
Automation
Agent
System Event
API
```

Unterschiedliche Eingabeformen dürfen denselben semantischen Intent erzeugen.

## Kontext

Die Intent-Auflösung darf berücksichtigen:

```text
Selected Objects
Workspace
User Preferences
Available Capabilities
Device Resources
Network State
Trust
Policy
Current Activity
```

Kontext verändert die mögliche Umsetzung, nicht die grundlegende Bedeutung des Intents.

## Capability-Auflösung

```text
Intent
   ↓
Required Semantics
   ↓
Capability Discovery
   ↓
Compatible Providers
   ↓
Execution Contract
```

CapabilityID, Provider und Ausführungsort bleiben vom Intent getrennt.

## Benutzerentscheidung

Existieren mehrere sinnvolle Umsetzungen, darf NovaOS:

```text
Automatically Select
Recommend
Ask User
Use Preferred Provider
Use Explicit User Choice
```

verwenden.

Explizite Benutzerentscheidungen haben Vorrang vor adaptiver Optimierung, sofern keine höheren Sicherheits- oder Systemgrenzen verletzt werden.

## KI-Unterstützung

KI darf natürliche Sprache oder komplexe Eingaben in strukturierte Intents übersetzen.

```text
Natural Language
      ↓
Optional AI Interpretation
      ↓
Structured Intent
      ↓
Deterministic Resolution
```

NovaOS muss jedoch auch ohne KI strukturierte Intents verarbeiten können.

KI darf weder Authority erzeugen noch Sicherheitsentscheidungen umgehen.

## Authority

Ein Intent ist ausschließlich eine Beschreibung einer gewünschten Handlung.

```text
Intent
   ≠
Permission
```

Erst bei der konkreten Ausführung werden erforderliche Capabilities gegen vorhandene Authority und Policy geprüft.

## Normative Anforderungen

1. NovaOS MUSS Intents unabhängig von Apps und Providern darstellen können.
2. Intent, Task und Execution Plan MÜSSEN getrennte Konzepte bleiben.
3. Intents MÜSSEN Inputs, gewünschte Outputs und Constraints beschreiben können.
4. Unterschiedliche Eingabeformen DÜRFEN denselben Intent erzeugen.
5. Intent-Auflösung MUSS verfügbare Capabilities dynamisch berücksichtigen können.
6. Ein Intent DARF keinen bestimmten Provider voraussetzen müssen.
7. Kontext DARF die Umsetzung eines Intents beeinflussen.
8. Explizite Benutzerentscheidungen MÜSSEN gegenüber adaptiver Optimierung berücksichtigt werden.
9. KI DARF zur Intent-Erkennung verwendet werden, MUSS aber optional bleiben.
10. Intents DÜRFEN keine Authority erzeugen.
11. Die konkrete Ausführung MUSS separat autorisiert werden.
12. Intent, Auflösung, erzeugte Tasks und ausgewählte Provider MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-APP-CAPABILITYCOMPOSED-0001`
- `NPSPEC-APP-OBJECTCENTRIC-0001`
- `NPSPEC-APP-TASKCENTRIC-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Benutzerabsichten als eigenständige semantische Intents verstehen und anschließend passende Tasks, Capabilities und Provider bestimmen. Dadurch bleibt die gewünschte Handlung von Apps und Implementierungen entkoppelt, während deterministische Ausführung, Benutzerkontrolle und Capability-basierte Sicherheit erhalten bleiben.