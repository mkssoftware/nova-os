# NPSPEC-UI-SEMANTIC-0001 – Semantic UI

## Status

Angenommen

## Kategorie

UI / Semantics / Intent / Capabilities

## Zweck

NovaOS beschreibt native Benutzeroberflächen nicht nur durch visuelle Elemente, sondern durch deren **semantische Bedeutung**.

Ein UI-Element beschreibt daher nicht primär, wie es aussieht, sondern welche Funktion es besitzt.

```text
Semantics
   ↓
UI Representation
   ↓
Rendering
```

Dadurch können Darstellung, Eingabemethode, Accessibility und Geräteform verändert werden, ohne die Bedeutung der Funktion zu verlieren.

## Grundprinzip

Statt:

```text
Rectangle
Text
Icon
Click Handler
```

soll NovaOS möglichst beschreiben:

```text
Action: Save
Object: Current Document
Role: Primary Action
State: Available
```

Die UI Runtime bestimmt daraus die geeignete Darstellung.

## Architektur

```text
Application Intent
       ↓
Semantic UI Model
       ↓
Declarative UI
       ↓
UI Tree
       ↓
Scene Graph
       ↓
Compositor
```

Semantik liegt oberhalb der konkreten Darstellung.

## Semantic UI Element

Ein semantisches UI-Element kann mindestens enthalten:

```text
ElementID
Semantic Role
Intent
Target Object
Capability
State
Importance
Relationships
Actions
```

Visuelle Eigenschaften können zusätzlich vorhanden sein, bestimmen jedoch nicht die funktionale Identität.

## Semantische Rollen

Beispiele:

```text
Primary Action
Secondary Action
Navigation
Search
Selection
Confirmation
Destructive Action
Information
Warning
Progress
Content
Tool
```

Diese Rollen können abhängig vom UI-Kontext unterschiedlich dargestellt werden.

## Intent

UI-Aktionen können die Absicht des Nutzers beschreiben.

```text
Save Document
Share Object
Edit Image
Print Document
Search Content
Open Location
Encrypt Object
```

Der Intent kann anschließend mit einer passenden Capability verbunden werden.

```text
User Intent
    ↓
Semantic Action
    ↓
Required Capability
    ↓
Provider
```

## Semantische Objekte

UI-Elemente können auf NovaOS-Objekte statt auf Pfade oder implementationsspezifische Handles verweisen.

```text
UI Action
    ↓
ObjectID
    +
Semantic Type
    ↓
Capability
```

Beispiel:

```text
Selected Object
Type: Image
Action: Edit
      ↓
Image.Edit
```

## Darstellung

Dasselbe semantische Element darf unterschiedlich dargestellt werden.

```text
Save Action
 ├── Toolbar Button
 ├── Menu Entry
 ├── Keyboard Action
 ├── Touch Action
 └── Voice Action
```

Alle Darstellungen repräsentieren dieselbe semantische Funktion.

## Kontextabhängige Darstellung

Die UI Runtime darf anhand des Kontextes eine geeignete Darstellung wählen.

```text
Semantic Function
       ↓
Context
       ↓
Representation
```

Kontext kann umfassen:

```text
Display Size
Input Method
Accessibility
Device Type
Window Size
User Preference
Current Activity
```

Die semantische Bedeutung darf dabei nicht verändert werden.

## Beziehungen

Semantische UI-Elemente können explizite Beziehungen besitzen.

Beispiele:

```text
Controls
Describes
Belongs To
Acts On
Navigates To
Depends On
Alternative To
```

Damit kann NovaOS Zusammenhänge verstehen, ohne sie aus visueller Nähe ableiten zu müssen.

## Capability Integration

Semantische Aktionen können direkt mit Capabilities verbunden werden.

```text
Semantic Action
      ↓
Capability Requirement
      ↓
Discovery
      ↓
Authorization
      ↓
Provider
```

Die UI muss dadurch keine konkrete Anwendung kennen.

## Accessibility

Der Accessibility Tree kann direkt aus der vorhandenen UI-Semantik abgeleitet werden.

```text
Semantic UI
    ├── Role
    ├── State
    ├── Action
    └── Relationships
           ↓
Accessibility Tree
```

Accessibility ist damit Bestandteil des UI-Modells und keine nachträgliche Interpretation der Pixeloberfläche.

## Adaptive UI

Adaptive UI darf die Darstellung semantischer Funktionen verändern.

```text
Semantic UI
     ↓
Adaptive Decision
     ↓
Alternative Representation
```

Die zugrunde liegende Bedeutung und Funktion bleibt erhalten.

Beispiel:

```text
Desktop → Toolbar Button
Tablet  → Touch Action
Small UI → Menu Entry
```

## Mehrere Darstellungen

Eine semantische Funktion darf gleichzeitig mehrere Repräsentationen besitzen.

```text
Search
 ├── Search Field
 ├── Keyboard Shortcut
 └── Voice Command
```

Alle Repräsentationen können auf dieselbe Action und Capability verweisen.

## Semantische Zustände

Zustände sollen möglichst funktional beschrieben werden.

```text
Available
Unavailable
Active
Selected
Busy
Pending
Completed
Failed
```

Die visuelle Darstellung dieses Zustands bleibt Aufgabe der UI Runtime und des Themes.

## Sicherheit

Semantische Bedeutung erzeugt keine Autorität.

```text
Semantic Action
      ↓
Capability Request
      ↓
Authorization
      ↓
Execution
```

Ein Element mit:

```text
Action: Delete
```

besitzt dadurch nicht automatisch die Berechtigung, ein Objekt zu löschen.

## Automation und Agents

Autorisierte Automation und Agents dürfen semantische UI-Informationen verwenden.

```text
Semantic UI
    ↓
Authorized Automation
```

Dadurch müssen UI-Aktionen nicht über Pixelpositionen oder simulierte Mausbewegungen erkannt werden.

Semantischer Zugriff erzeugt jedoch keine zusätzlichen Capabilities.

## Normative Anforderungen

1. Native NovaOS-UI SOLL funktionale Bedeutung explizit beschreiben.
2. Semantik MUSS von konkreter visueller Darstellung trennbar sein.
3. Semantische UI-Elemente SOLLEN stabile Identitäten besitzen.
4. Aktionen SOLLEN als semantische Intents beschreibbar sein.
5. UI-Aktionen SOLLEN auf semantische NovaOS-Objekte verweisen können.
6. Dasselbe semantische Element DARF mehrere Darstellungen besitzen.
7. Kontextabhängige Darstellung DARF die Bedeutung einer Funktion nicht verändern.
8. Semantische Beziehungen SOLLEN explizit beschreibbar sein.
9. Semantische Actions SOLLEN mit Capabilities verbunden werden können.
10. Accessibility SOLL vorhandene UI-Semantik direkt wiederverwenden können.
11. Adaptive UI DARF Darstellung, aber nicht eigenmächtig die Bedeutung einer Funktion verändern.
12. Semantische UI-Informationen DÜRFEN keine zusätzlichen Berechtigungen erzeugen.
13. Autorisierte Automation DARF semantische UI-Informationen verwenden.
14. UI-Funktionalität SOLL nicht von Pixelpositionen oder konkreten visuellen Repräsentationen abhängig sein.

## Abhängigkeiten

- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-ADAPTIVE-0001`
- `NPSPEC-UI-CAPABILITYDRIVEN-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-SEMANTIC-0001`
- `ADR-SEMANTIC-0006_Typed_Relationships`
- `ADR-CAPABILITY-0001`
- `ADR-OBJECT-0001`

## Ergebnis

NovaOS behandelt Benutzeroberflächen als semantische Schnittstelle zwischen Nutzerabsicht und Systemfunktion:

```text
User Intent
    ↓
Semantic UI
    ↓
Semantic Action
    ↓
Capability
    ↓
Authorized Provider
    ↓
Result
```

Die konkrete visuelle Darstellung wird damit zu einer austauschbaren Repräsentation der zugrunde liegenden Bedeutung, statt selbst die Funktion zu definieren.