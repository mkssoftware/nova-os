# NPSPEC-UI-SCENEGRAPH-0001 – Nova Scene Graph

## Status

Angenommen

## Kategorie

UI / Scene Graph / Composition / Rendering

## Zweck

Der Nova Scene Graph beschreibt die sichtbare UI-Szene als hierarchische Struktur aus darstellbaren Objekten und deren Beziehungen.

Er bildet die deklarative Grundlage für:

```text
Position
Größe
Transformation
Clipping
Opacity
Z-Order
Visibility
Effects
Parent/Child Relations
```

Der Scene Graph beschreibt **was dargestellt werden soll**. Der Compositor entscheidet **wie daraus effizient ein Frame erzeugt wird**.

## Architektur

```text
Window Server
     ↓
Scene Graph
     ↓
Compositor
     ↓
Display Server
     ↓
Display
```

Der Scene Graph ist damit die Schnittstelle zwischen logischer Fenster-/Surface-Struktur und tatsächlicher Composition.

## Scene

Eine Scene beschreibt den darzustellenden Zustand eines Display Space.

Beispiel:

```text
Scene
 ├── Desktop
 ├── Window A
 │    ├── Surface
 │    └── Decoration
 ├── Window B
 │    └── Surface
 ├── System UI
 │    ├── Taskbar
 │    └── Notification
 └── Cursor
```

## Scene Node

Jedes Element wird durch einen Scene Node repräsentiert.

Ein Node besitzt mindestens:

```text
NodeID
ParentID
Type
Transform
Bounds
Visibility
Opacity
Clip
Z-Order
Content Reference
```

`NodeID` dient als stabile Identität innerhalb der Scene.

## Hierarchie

Transformationen und Eigenschaften können hierarchisch wirken.

```text
Window
 └── Surface
      └── Overlay
```

Wird das Window verschoben, folgen seine untergeordneten Nodes entsprechend ihrer definierten Beziehung.

## Content Reference

Ein Scene Node enthält nicht zwingend selbst Pixel.

Er kann auf darstellbare Ressourcen verweisen:

```text
Surface
Image
Text Layer
Vector Content
Video Surface
System UI Element
```

Die eigentlichen Daten bleiben in den dafür vorgesehenen Ressourcen.

## Transformationen

Nodes unterstützen mindestens:

```text
Translation
Scaling
Rotation
Opacity
Clipping
```

Transformationen werden entlang der Parent/Child-Hierarchie kombiniert.

```text
Parent Transform
      +
Child Transform
      ↓
Effective Transform
```

## Z-Order

Die Darstellungsreihenfolge wird explizit bestimmt.

```text
Desktop
   ↓
Application Windows
   ↓
System UI
   ↓
Security UI
   ↓
Cursor
```

Besondere Systemebenen dürfen nur durch entsprechende Capabilities verwendet werden.

## Änderungen

Scene-Änderungen sollen atomar veröffentlicht werden können.

```text
Current Scene
     ↓
Prepare Changes
     ↓
Commit
     ↓
New Scene Generation
```

Dadurch darf der Compositor keinen teilweise aktualisierten Scene-Zustand sehen.

## Versionierung

Jeder veröffentlichte Scene-Zustand erhält eine Generation.

```text
Scene Generation 104
        ↓
Scene Generation 105
```

Der Compositor arbeitet immer mit einer konsistenten Generation.

## Damage Propagation

Änderungen an Nodes erzeugen Damage für die betroffenen Bereiche.

```text
Node Change
    ↓
Scene Damage
    ↓
Compositor
    ↓
Partial Composition
```

Transformation, Sichtbarkeit und Überdeckung müssen bei der Berechnung berücksichtigt werden.

## Sichtbarkeit

Der Scene Graph kann bestimmen, welche Bereiche grundsätzlich sichtbar sind.

Vollständig verdeckte oder unsichtbare Nodes müssen nicht zwingend gerendert oder komponiert werden.

```text
Visible
Occluded
Hidden
Outside View
```

Diese Information kann für Rendering- und Ressourcenoptimierung verwendet werden.

## Sicherheit

Der Scene Graph ist keine Autoritätsquelle.

Eine vorhandene Beziehung bedeutet nicht automatisch, dass ein Prozess auf die zugrunde liegende Surface zugreifen darf.

```text
Scene Visibility
      ≠
Resource Authority
```

Security-relevante Ebenen und geschützte Inhalte behalten ihre eigenen Capability- und Security-Regeln.

## Multi-Display

Der Scene Graph muss mehrere Displays unterstützen können.

Dabei können:

```text
Shared Scene
Per-Display Scene
Display Subtrees
```

verwendet werden.

Nodes können einem oder mehreren Display Spaces zugeordnet werden.

## Normative Anforderungen

1. Der UI-Zustand MUSS als hierarchischer Scene Graph darstellbar sein.
2. Scene Nodes MÜSSEN eindeutige `NodeID`s besitzen.
3. Parent/Child-Beziehungen MÜSSEN explizit beschrieben werden.
4. Transformationen MÜSSEN hierarchisch kombinierbar sein.
5. Z-Order MUSS deterministisch bestimmbar sein.
6. Scene-Änderungen SOLLEN atomar veröffentlicht werden.
7. Veröffentlichte Scene-Zustände MÜSSEN versioniert oder generationiert sein.
8. Der Compositor MUSS eine konsistente Scene Generation erhalten.
9. Änderungen SOLLEN Damage Propagation auslösen.
10. Unsichtbare oder vollständig verdeckte Nodes SOLLEN bei der Composition übersprungen werden können.
11. System- und Security-Ebenen MÜSSEN capability-basiert geschützt werden.
12. Scene-Graph-Sichtbarkeit DARF keine zusätzliche Ressourcenautorität erzeugen.
13. Mehrere Displays MÜSSEN unterstützt werden.

## Abhängigkeiten

- `NPSPEC-UI-WINDOWSERVER-0001`
- `NPSPEC-UI-DISPLAYSERVER-0001`
- `NPSPEC-UI-COMPOSITOR-0001`
- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-CAPABILITY-0001`
- `ADR-STATE-0006_State_Versioning_und_History`

## Ergebnis

Der Nova Scene Graph bildet die deklarative Beschreibung der sichtbaren UI:

```text
Windows / Surfaces / System UI
              ↓
         Scene Graph
              ↓
     Consistent Generation
              ↓
          Compositor
              ↓
            Frame
```

Damit werden UI-Struktur und Rendering voneinander getrennt. Der Window Server beschreibt die Szene, während der Compositor diese Beschreibung effizient in sichtbare Frames überführt.