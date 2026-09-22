# NPSPEC-UI-DECLARATIVE-0001 – Declarative UI

## Status

Angenommen

## Kategorie

UI / Declarative / State / Rendering

## Zweck

NovaOS beschreibt native Benutzeroberflächen primär **deklarativ**.

Eine UI definiert den gewünschten Zustand der Oberfläche, nicht die einzelnen Schritte, mit denen dieser Zustand erzeugt wird.

```text
Application State
      ↓
Desired UI State
      ↓
Nova UI Runtime
      ↓
UI Tree
      ↓
Scene Graph
      ↓
Compositor
```

## Grundprinzip

Anwendungen beschreiben:

```text
Was existiert?
Welche Eigenschaften besitzt es?
Wie sind Elemente miteinander verbunden?
Von welchem Zustand hängen Eigenschaften ab?
```

Die UI Runtime bestimmt daraus die notwendigen Änderungen.

```text
Desired State
      ↓
Compare
      ↓
Minimal Changes
      ↓
Actual UI State
```

## Deklarative Struktur

Eine Oberfläche kann beispielsweise logisch beschrieben werden als:

```text
Window
 ├── Toolbar
 │    ├── BackButton
 │    └── SearchField
 └── Content
      ├── Sidebar
      └── DocumentView
```

Diese Struktur wird in persistente UI-Objekte des Retained-Mode-Systems überführt.

## State Binding

UI-Eigenschaften können an Anwendungszustände gebunden werden.

```text
Application State
      ↓
Binding
      ↓
UI Property
```

Beispiele:

```text
Document.Title → Window.Title
Search.Active  → ProgressIndicator.Visible
User.Name      → ProfileLabel.Text
```

Ändert sich der zugrunde liegende Zustand, wird nur der betroffene UI-Bereich aktualisiert.

## Reconciliation

Die UI Runtime vergleicht gewünschten und aktuellen UI-Zustand.

```text
Desired UI
    +
Actual UI
    ↓
Reconciliation
    ↓
Create / Update / Remove
```

Unveränderte UI-Elemente bleiben erhalten.

Dadurch wird kein vollständiger Neuaufbau der Oberfläche für jede Zustandsänderung benötigt.

## Identität

Deklarative UI-Elemente können stabile Identitäten besitzen.

```text
ElementID
```

Die Identität bleibt unabhängig von:

```text
Position
Text
Style
Current State
Scene Node
```

Dadurch können Zustand, Focus, Animationen und Accessibility-Informationen über Aktualisierungen hinweg erhalten bleiben.

## Trennung von Zustand und Darstellung

Anwendungszustand und visuelle Darstellung werden getrennt behandelt.

```text
Application State
       ↓
UI Description
       ↓
Visual Representation
```

Geschäftslogik soll nicht von konkreten Pixelkoordinaten oder Rendering-Details abhängig sein.

## Layout

Deklarative UI beschreibt Layout über Beziehungen und Constraints.

Beispiele:

```text
Horizontal
Vertical
Grid
Overlay
Anchor
Adaptive Layout
Minimum / Maximum Size
Preferred Size
```

Die Layout Engine berechnet daraus die tatsächliche Geometrie.

## Styling

Darstellung wird getrennt von Struktur und Anwendungslogik definiert.

```text
UI Structure
     +
Style
     +
System Theme
     ↓
Visual UI
```

Systemweite Eigenschaften wie:

```text
Theme
Scaling
Contrast
Accessibility
Motion Preferences
```

können dadurch konsistent angewendet werden.

## Events und Actions

Benutzerinteraktionen werden als semantische Events oder Actions an die Anwendung übergeben.

```text
User Input
    ↓
UI Element
    ↓
Semantic Action
    ↓
Application Logic
    ↓
State Change
    ↓
UI Reconciliation
```

Die Anwendung soll nicht unnötig direkt den Rendering-Zustand manipulieren.

## Scene Graph Integration

Die deklarative UI erzeugt keinen Display Frame direkt.

```text
Declarative UI
      ↓
Retained UI Tree
      ↓
Layout
      ↓
Scene Graph
      ↓
Compositor
```

Rendering und Composition bleiben von der UI-Beschreibung getrennt.

## Accessibility

Semantik soll bereits in der UI-Beschreibung enthalten sein.

```text
Button
 ├── Role
 ├── Label
 ├── State
 └── Action
```

Diese Informationen können direkt in den Accessibility Tree übernommen werden.

Accessibility muss dadurch nicht nachträglich aus der visuellen Darstellung rekonstruiert werden.

## Native und Custom Content

Deklarative UI muss mit spezialisierten Rendering-Flächen kombinierbar sein.

```text
Declarative UI
 ├── Native Controls
 ├── Text
 ├── Images
 ├── Application Surface
 ├── Video Surface
 └── Custom Renderer
```

Damit bleibt das Modell auch für komplexe Anwendungen nutzbar.

## Normative Anforderungen

1. Native NovaOS-UI MUSS deklarativ beschreibbar sein.
2. UI-Beschreibungen MÜSSEN in persistente Retained-Mode-Objekte überführbar sein.
3. Desired und Actual UI State MÜSSEN getrennt behandelbar sein.
4. Änderungen SOLLEN durch Reconciliation inkrementell angewendet werden.
5. Unveränderte UI-Elemente SOLLEN ihre Identität behalten.
6. UI-Eigenschaften MÜSSEN an Anwendungszustände bindbar sein.
7. Anwendungslogik SOLL von konkreten Rendering-Details getrennt bleiben.
8. Layout SOLL über deklarative Beziehungen und Constraints beschrieben werden.
9. Styling MUSS von UI-Struktur und Geschäftslogik trennbar sein.
10. Benutzerinteraktionen SOLLEN als semantische Actions verarbeitet werden.
11. Accessibility-Semantik SOLL Bestandteil der UI-Beschreibung sein.
12. Deklarative UI MUSS mit eigenen Application Surfaces kombinierbar sein.
13. Änderungen MÜSSEN in Scene-Graph- und Damage-Updates überführbar sein.

## Abhängigkeiten

- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-RETAINEDMODE-0001`
- `NPSPEC-UI-DAMAGETRACKING-0001`
- `NPSPEC-UI-INPUTROUTING-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-STATE-0002_Desired_und_Actual_State`
- `ADR-STATE-0004_State_Reconciliation`
- `ADR-ARCH-0008_Semantic_Types`

## Ergebnis

Die native NovaOS-UI folgt dem Prinzip:

```text
State
  ↓
Declare Desired UI
  ↓
Reconcile
  ↓
Retained UI Tree
  ↓
Scene Graph
  ↓
Render only required changes
```

Anwendungen beschreiben damit primär den gewünschten Zustand ihrer Oberfläche. NovaOS übernimmt die effiziente Überführung dieses Zustands in Layout, Scene Graph, Rendering und Composition.