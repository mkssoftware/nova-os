# NPSPEC-UI-COMPONENT-0001 – Nova UI Components

## Status

Angenommen

## Kategorie

UI / Components / Controls / Declarative UI

## Zweck

NovaOS definiert ein einheitliches System wiederverwendbarer UI-Komponenten als Grundlage der nativen Benutzeroberfläche.

```text
Semantic Function
      ↓
UI Component
      ↓
Declarative UI
      ↓
Scene Graph
      ↓
Rendering
```

Komponenten kapseln Verhalten, Semantik und Zustand, ohne Anwendungen an eine konkrete Rendering-Implementierung zu binden.

## Grundprinzip

Eine UI-Komponente beschreibt mehr als ihre visuelle Darstellung.

```text
Component
 ├── Semantic Role
 ├── State
 ├── Properties
 ├── Actions
 ├── Layout
 ├── Accessibility
 └── Visual Representation
```

Beispiel:

```text
Button
```

ist nicht lediglich ein gezeichnetes Rechteck, sondern ein semantisches Bedienelement mit definierter Aktion und definierten Zuständen.

## Architektur

```text
Application
    ↓
Declarative UI
    ↓
Component System
    ↓
Retained UI Tree
    ↓
Scene Graph
    ↓
Compositor
```

## Basiskomponenten

NovaOS stellt einen gemeinsamen Satz nativer Komponenten bereit.

Beispiele:

```text
Window
Panel
Button
Toggle
Checkbox
RadioButton
TextField
SearchField
Slider
Progress
List
Tree
Table
Menu
Toolbar
Tabs
Dialog
Card
Tooltip
ScrollView
Image
Text
Icon
```

Der Komponentensatz muss erweiterbar bleiben.

## Komponentenidentität

Jede instanziierte Komponente kann eine stabile Identität besitzen.

```text
ComponentID
```

Diese bleibt während des Lebenszyklus unabhängig von:

```text
Position
Size
Theme
Visual State
Scene Node
```

bestehen.

## Eigenschaften

Komponenten besitzen deklarative Properties.

Beispiel:

```text
Button
 ├── Text
 ├── Icon
 ├── Enabled
 ├── Visible
 ├── Action
 ├── Priority
 └── Style Role
```

Änderungen an Properties werden inkrementell verarbeitet.

```text
Property Change
      ↓
Component State
      ↓
Layout / Scene / Damage
```

## Zustände

Komponenten können definierte Zustände besitzen.

```text
Normal
Hovered
Focused
Pressed
Selected
Disabled
Busy
Error
```

Zustand und visuelle Darstellung bleiben getrennt.

```text
State
  ↓
Theme
  ↓
Visual Representation
```

## Semantik

Jede interaktive Standardkomponente besitzt eine definierte semantische Bedeutung.

```text
Button
→ Action

Checkbox
→ Boolean Selection

Slider
→ Bounded Value

TextField
→ Editable Text
```

Diese Semantik kann von Accessibility, Automation und Capability-Systemen verwendet werden.

## Actions

Interaktionen werden bevorzugt als semantische Actions ausgelöst.

```text
Input
  ↓
Component
  ↓
Semantic Action
  ↓
Application / Capability
```

Komponenten sollen nicht unnötig direkt Geschäftslogik enthalten.

## Composition

Komplexe Komponenten können aus kleineren Komponenten zusammengesetzt werden.

```text
SearchField
 ├── Icon
 ├── TextInput
 └── ClearAction
```

Komposition wird gegenüber großen monolithischen Spezialkomponenten bevorzugt.

## Custom Components

Anwendungen dürfen eigene Komponenten definieren.

```text
Base Components
      ↓
Composition
      ↓
Custom Component
```

Eigene Komponenten müssen dieselben grundlegenden Regeln für:

```text
Semantics
Accessibility
Input
Security
Layout
State
```

einhalten.

## Styling und Themes

Komponenten definieren keine fest eingebrannten visuellen Designs.

```text
Component
    +
Semantic Style Role
    +
Nova Theme
    ↓
Visual Representation
```

Dadurch können systemweit konsistente Designs verwendet werden.

Anwendungen dürfen definierte Style-Varianten verwenden, sollen jedoch grundlegende Systemsemantik nicht verändern.

## Responsive Verhalten

Komponenten können alternative Darstellungen besitzen.

```text
Expanded Button
→ Icon + Label

Compact Button
→ Icon
```

Die semantische Funktion bleibt identisch.

## Input

Komponenten sollen unabhängig von einer einzelnen Eingabemethode funktionieren.

```text
Mouse
Keyboard
Touch
Pen
Accessibility
```

Input wird über das zentrale Input Routing bereitgestellt.

## Accessibility

Standardkomponenten stellen ihre Accessibility-Semantik automatisch bereit.

```text
Component
   ↓
Role + State + Actions
   ↓
Accessibility Tree
```

Custom Components müssen entsprechende Semantik bereitstellen.

## Capability Integration

Komponenten dürfen direkt an semantische Capabilities gebunden werden.

```text
Button
  ↓
Action: Print
  ↓
Document.Print
  ↓
Capability Provider
```

Die Komponente selbst erhält dadurch keine zusätzliche Berechtigung.

## Lebenszyklus

Eine Komponente besitzt einen definierten Lebenszyklus.

```text
Create
  ↓
Attach
  ↓
Active
  ↓
Update
  ↓
Detach
  ↓
Destroy
```

Ressourcen müssen spätestens beim Ende des Lebenszyklus kontrolliert freigegeben werden.

## Performance

Komponenten arbeiten nach dem Retained-Mode-Prinzip.

```text
State Change
    ↓
Affected Component
    ↓
Incremental Update
    ↓
Damage Region
```

Ein einzelner Komponentenwechsel darf nicht automatisch einen vollständigen Neuaufbau der Oberfläche verursachen.

## Sicherheit

UI-Komponenten stellen keine Sicherheitsgrenze allein durch ihre Darstellung dar.

Insbesondere gilt:

```text
Visible Action
      ≠
Authorized Action
```

Sicherheitskritische Aktionen müssen weiterhin durch das Capability- und Autorisierungssystem geprüft werden.

Geschützte Systemkomponenten dürfen nur durch autorisierte System-UI-Kontexte verwendet werden.

## Normative Anforderungen

1. NovaOS MUSS einen gemeinsamen Satz nativer UI-Komponenten bereitstellen.
2. Komponenten MÜSSEN deklarativ verwendbar sein.
3. Komponenten SOLLEN stabile Instanzidentitäten besitzen.
4. Komponenten MÜSSEN Zustand und visuelle Darstellung trennen.
5. Interaktive Komponenten MÜSSEN semantische Rollen und Actions besitzen.
6. Standardkomponenten MÜSSEN Accessibility-Semantik bereitstellen.
7. Custom Components MÜSSEN Accessibility und Input vollständig unterstützen können.
8. Komposition SOLL gegenüber unnötig monolithischen Komponenten bevorzugt werden.
9. Komponenten MÜSSEN mit systemweiten Themes kombinierbar sein.
10. Komponenten SOLLEN responsive Darstellungen unterstützen.
11. Komponenten SOLLEN unabhängig von einer einzelnen Eingabemethode funktionieren.
12. Property- und State-Änderungen SOLLEN inkrementell verarbeitet werden.
13. Capability-gebundene Komponenten DÜRFEN keine Autorisierung allein aus ihrer Darstellung ableiten.
14. Geschützte Systemkomponenten MÜSSEN capability-basiert abgesichert werden.
15. Der Komponenten-Lebenszyklus MUSS definierte Ressourcenfreigabe ermöglichen.

## Abhängigkeiten

- `NPSPEC-UI-RETAINEDMODE-0001`
- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-RESPONSIVE-0001`
- `NPSPEC-UI-INPUTROUTING-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `NPSPEC-UI-CAPABILITYDRIVEN-0001`
- `NPSPEC-UI-DAMAGETRACKING-0001`
- `ADR-ARCH-0017_Composability`
- `ADR-ARCH-0008_Semantic_Types`

## Ergebnis

NovaOS erhält ein einheitliches, semantisches und wiederverwendbares UI-Komponentensystem:

```text
Semantic Component
       ↓
State + Properties
       ↓
Declarative UI
       ↓
Retained UI Tree
       ↓
Incremental Rendering
```

Damit können Desktop, Startmenü, Dashboard, Ribbon, Systemdialoge und Anwendungen auf denselben grundlegenden UI-Bausteinen aufbauen, ohne ihre Darstellung, Accessibility-Logik und Interaktionsmodelle jeweils neu implementieren zu müssen.