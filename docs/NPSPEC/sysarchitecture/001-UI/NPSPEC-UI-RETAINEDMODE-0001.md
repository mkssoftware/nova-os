# NPSPEC-UI-RETAINEDMODE-0001 – Retained Mode UI

## Status

Angenommen

## Kategorie

UI / Rendering Model / Scene Graph / State

## Zweck

NovaOS verwendet für seine native Benutzeroberfläche ein **Retained-Mode-Modell**.

UI-Elemente bleiben als persistente Objekte erhalten und werden nicht bei jedem Frame vollständig neu beschrieben.

```text
Application State
      ↓
UI Objects
      ↓
Scene Graph
      ↓
Compositor
      ↓
Display
```

Damit können Zustand, Beziehungen und Änderungen der Oberfläche unabhängig vom eigentlichen Rendering verwaltet werden.

## Grundmodell

Eine Anwendung erzeugt UI-Objekte und verändert anschließend deren Eigenschaften.

```text
Create Window
   ↓
Create Controls
   ↓
Modify Properties
   ↓
UI System detects changes
   ↓
Update Scene
```

Die Anwendung muss die vollständige Oberfläche nicht für jeden Frame erneut erzeugen.

## UI Tree

Die Oberfläche wird als hierarchischer UI Tree verwaltet.

```text
Window
 ├── Toolbar
 │    ├── Button
 │    └── SearchField
 └── Content
      ├── Sidebar
      └── DocumentView
```

UI-Elemente behalten ihre Identität über mehrere Frames hinweg.

## UI Object

Ein UI-Objekt besitzt mindestens:

```text
ElementID
Type
Parent
Children
Properties
State
Layout Information
Content Reference
```

Darstellungszustand und Anwendungsdaten bleiben logisch getrennt.

## Properties

Darstellung und Verhalten werden über Eigenschaften beschrieben.

Beispiele:

```text
Position
Size
Visibility
Enabled
Opacity
Text
Style
Transform
Layout Constraints
```

Eine Änderung markiert nur die davon betroffenen Teile des UI Trees als verändert.

## State

UI-Elemente können persistenten Interaktionszustand besitzen.

Beispiele:

```text
Focused
Hovered
Pressed
Selected
Expanded
Disabled
```

Dieser Zustand wird nicht bei jedem Frame neu konstruiert.

## Änderungserkennung

Änderungen werden inkrementell verarbeitet.

```text
Property Change
      ↓
Dirty State
      ↓
Layout / Scene Update
      ↓
Damage Region
      ↓
Composition
```

Nur betroffene Bereiche müssen neu berechnet oder dargestellt werden.

## Layout

Layout wird nur neu berechnet, wenn relevante Eigenschaften geändert wurden.

```text
Content Change
Size Change
Constraint Change
Style Change
```

Eine reine Änderung der Opacity muss beispielsweise nicht zwangsläufig eine vollständige Layout-Neuberechnung verursachen.

## Scene Graph Integration

Der UI Tree und der Scene Graph erfüllen unterschiedliche Aufgaben.

```text
UI Tree
   ↓
Layout + Visual State
   ↓
Scene Graph
   ↓
Compositor
```

Der UI Tree beschreibt semantische UI-Elemente.

Der Scene Graph beschreibt die daraus resultierende darstellbare Szene.

## Rendering

Retained Mode bedeutet nicht, dass Anwendungen zwingend jedes Detail ihres Renderings an NovaOS abgeben müssen.

Komplexe Inhalte können weiterhin eigene Surfaces verwenden.

```text
Native UI Elements
       +
Application Surface
       ↓
Scene Graph
```

Damit können klassische Controls, Dokumentansichten, Spiele, Video oder spezialisierte Renderer gemeinsam verwendet werden.

## Animationen

Animationen werden als zeitabhängige Änderungen bestehender Eigenschaften behandelt.

```text
Property A
   ↓
Animation
   ↓
Property B
```

Geeignete Animationen können direkt durch Scene Graph oder Compositor ausgeführt werden, ohne dass die Anwendung jeden Frame aktiv erzeugen muss.

## Ressourcen

Nicht sichtbare UI-Bereiche dürfen Ressourcen reduzieren.

Beispiele:

```text
Hidden
Occluded
Minimized
Off-Screen
```

Retained State bleibt dabei erhalten, während Rendering oder Aktualisierung pausiert werden kann.

## Immediate Mode

Immediate-Mode-Rendering bleibt für spezielle Anwendungsfälle möglich.

```text
Canvas
Game Surface
Visualization
Custom Renderer
```

Es ersetzt jedoch nicht das native Retained-Mode-Modell der NovaOS-Systemoberfläche.

## Normative Anforderungen

1. Native NovaOS-UI MUSS Retained Mode als primäres UI-Modell verwenden.
2. UI-Elemente MÜSSEN über Frames hinweg eine stabile Identität besitzen können.
3. UI-Zustand MUSS unabhängig vom aktuellen Frame erhalten bleiben können.
4. Änderungen SOLLEN inkrementell verarbeitet werden.
5. Unveränderte UI-Bereiche SOLLEN nicht vollständig neu berechnet werden.
6. Layout SOLL nur bei relevanten Änderungen neu berechnet werden.
7. UI Tree und Scene Graph MÜSSEN logisch getrennt bleiben.
8. UI-Änderungen MÜSSEN in Scene- und Damage-Updates überführbar sein.
9. Animationen SOLLEN ohne vollständigen Neuaufbau des UI Trees möglich sein.
10. Nicht sichtbare UI-Bereiche SOLLEN Rendering-Ressourcen reduzieren können.
11. Eigene Application Surfaces MÜSSEN mit Retained-Mode-UI kombinierbar sein.
12. Immediate-Mode-Rendering DARF für spezialisierte Inhalte parallel unterstützt werden.

## Abhängigkeiten

- `NPSPEC-UI-WINDOWSERVER-0001`
- `NPSPEC-UI-COMPOSITOR-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-STATE-0003_State_Machines`

## Ergebnis

Die native NovaOS-Oberfläche folgt dem Modell:

```text
Persistent UI Objects
        ↓
Incremental Changes
        ↓
UI Tree
        ↓
Scene Graph
        ↓
Compositor
```

Damit bleibt die Oberfläche als strukturierter Zustand erhalten und NovaOS verarbeitet nur die Änderungen, die für den nächsten sichtbaren Zustand tatsächlich notwendig sind.