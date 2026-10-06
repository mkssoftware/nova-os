# NPSPEC-APP-TASKCENTRIC-0001 – Nova Task-Centric App

## Status

Angenommen

## Kategorie

App / Task-Centric

## Zweck

NovaOS definiert ein aufgabenzentriertes App-Modell, bei dem die Absicht des Nutzers im Mittelpunkt steht.

Der Nutzer wählt nicht zwingend zuerst eine Anwendung, sondern beschreibt oder startet eine Aufgabe. NovaOS ermittelt daraus benötigte Objekte, Capabilities, Services, Components und Oberflächen.

## Grundprinzipien

```text
Task ≠ App
Task ≠ Process
Task ≠ Workspace
Task ≠ Capability
Intent ≠ Implementation
Task Definition ≠ Authority
Execution Plan ≠ Permission
```

## Modell

```text
Task
├── TaskID
├── Intent
├── Inputs[]
├── Outputs[]
├── Constraints
├── Context
├── State
└── ExecutionPlan
```

`TaskID` identifiziert eine konkrete Aufgabeninstanz.

## Aufgabenermittlung

Eine Aufgabe kann entstehen durch:

```text
Direct User Action
Commandbar Prompt
UI Action
Object Operation
Solution
Automation
Agent
System Event
```

Beispiel:

```text
"Verkleinere diese Bilder und sende sie an Micha."
```

NovaOS kann daraus ableiten:

```text
Input Objects
   ↓
Image Resize Capability
   ↓
Output Objects
   ↓
Communication Capability
```

## Planung

```text
Intent
  +
Context
  +
Objects
  +
Constraints
    ↓
Capability Discovery
    ↓
Execution Plan
    ↓
Authorized Execution
```

Die Planung darf deterministisch oder durch einen optionalen KI-Planner unterstützt erfolgen.

Die tatsächliche Ausführung bleibt kontrolliert und an explizite Verträge gebunden.

## Execution Plan

Ein Plan darf enthalten:

```text
Capability Nodes
Service Calls
Object Operations
Dependencies
Data Flow
Control Flow
Transactions
Fallbacks
```

Der Plan ist von den konkreten Providern getrennt.

Provider dürfen während der Auflösung anhand von Kompatibilität, Trust, Ressourcen und Policy gewählt werden.

## Benutzeroberfläche

NovaOS darf für eine Aufgabe eine temporäre oder persistente Oberfläche erzeugen:

```text
Task
 ↓
Required Interaction
 ↓
UI Schema
 ↓
Task UI
```

Die Oberfläche zeigt bevorzugt die für die aktuelle Aufgabe relevanten Funktionen.

Nicht relevante Funktionen können zurücktreten, ohne notwendige Funktionen zu entfernen.

## Workspace

Eine Aufgabe darf innerhalb eines Workspace ausgeführt werden.

```text
Workspace
├── Task A
├── Task B
└── Shared Context
```

Task und Workspace bleiben getrennte Konzepte.

Ein Workspace kann mehrere Aufgaben enthalten und eine Aufgabe kann Unteraufgaben erzeugen.

## Structured Concurrency

Komplexe Aufgaben dürfen hierarchisch zerlegt werden:

```text
Task
├── Subtask A
├── Subtask B
│   ├── Subtask B1
│   └── Subtask B2
└── Subtask C
```

Lebensdauer, Fehler und Abbruch müssen kontrolliert an die Aufgabenhierarchie gebunden sein.

## Authority

Eine erkannte Benutzerabsicht erzeugt keine Berechtigung.

```text
Task Requirements
      ∩
Existing Authority
      ∩
User Decisions
      ∩
Policy
      =
Effective Authority
```

Planner, Agenten und generierte Execution Plans dürfen keine zusätzliche Authority erzeugen.

## Änderungen

Ein Execution Plan darf während der Aufgabe angepasst werden, wenn sich Ressourcen, Provider oder Kontext ändern.

Dabei müssen:

```text
Intent
Hard Constraints
Security
User Decisions
Transaction State
```

erhalten bleiben.

## Normative Anforderungen

1. NovaOS MUSS Aufgaben unabhängig von einzelnen Apps modellieren können.
2. Jede aktive Aufgabe MUSS eine eindeutige `TaskID` besitzen.
3. Intent und konkrete Implementierung MÜSSEN getrennt bleiben.
4. Aufgaben MÜSSEN Inputs, Outputs und Constraints beschreiben können.
5. NovaOS MUSS Aufgaben aus mehreren Capabilities zusammensetzen können.
6. Execution Plans MÜSSEN von konkreten Providern getrennt bleiben können.
7. Aufgaben DÜRFEN hierarchische Unteraufgaben erzeugen.
8. Task und Workspace MÜSSEN getrennte Konzepte bleiben.
9. Task-spezifische und generierte UIs MÜSSEN unterstützt werden können.
10. Ein Intent oder Execution Plan DARF keine Authority erzeugen.
11. KI-gestützte Planung DARF die kontrollierte Ausführung nicht umgehen.
12. TaskID, Intent, Plan, Zustand, Provider und effektive Authority MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-APP-CAPABILITYCOMPOSED-0001`
- `NPSPEC-APP-OBJECTCENTRIC-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-WORKSPACE-MODEL-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`

## Ergebnis

NovaOS kann Arbeit anhand der eigentlichen Benutzeraufgabe organisieren, statt sie zwingend an einzelne Anwendungen zu binden. Benötigte Objekte, Capabilities, Provider und Oberflächen werden passend zur Aufgabe zusammengesetzt, während Sicherheit, Benutzerentscheidungen und Ausführungskontrolle erhalten bleiben.