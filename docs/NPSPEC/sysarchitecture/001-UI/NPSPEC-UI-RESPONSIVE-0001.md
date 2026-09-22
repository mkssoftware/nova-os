# NPSPEC-UI-RESPONSIVE-0001 – Responsive UI

## Status

Angenommen

## Kategorie

UI / Layout / Responsive / Device Independence

## Zweck

NovaOS-Oberflächen passen ihre Darstellung dynamisch an den tatsächlich verfügbaren Darstellungsraum und die aktuelle Nutzungssituation an.

```text
Semantic UI
    ↓
Available Space + Context
    ↓
Responsive Layout
    ↓
Final Representation
```

Responsive UI ist Bestandteil des nativen Layoutsystems und keine separate Gerätevariante einer Anwendung.

## Grundprinzip

Eine Anwendung soll nicht unterschiedliche Oberflächen für Desktop, Tablet oder kleine Displays implementieren müssen.

```text
One Semantic UI
       ↓
Responsive Rules
       ↓
Desktop / Tablet / Small Window / Touch
```

Die semantische Funktion bleibt erhalten, während sich ihre Darstellung ändern darf.

## Architektur

```text
Declarative UI
      ↓
Semantic UI
      ↓
Responsive Constraints
      ↓
Layout Engine
      ↓
UI Tree
      ↓
Scene Graph
```

## Verfügbarer Raum

Layoutentscheidungen basieren primär auf dem tatsächlich verfügbaren Raum.

```text
Window Size
Content Area
System UI Insets
Scaling
Safe Areas
```

Eine Anwendung darf nicht allein anhand einer Geräteklasse annehmen, wie viel Platz verfügbar ist.

## Responsive Constraints

UI-Elemente können Regeln definieren wie:

```text
Minimum Size
Preferred Size
Maximum Size
Priority
Can Collapse
Can Wrap
Can Move
Can Hide
Alternative Representation
```

Die Layout Engine löst daraus eine geeignete Darstellung.

## Layout-Anpassung

Beispiel:

```text
Large Width

[Navigation] [Content] [Properties]
```

Bei weniger Platz:

```text
Medium Width

[Navigation] [Content]
             [Properties Overlay]
```

Bei sehr wenig Platz:

```text
Small Width

[Content]

Navigation → Menu
Properties → Context Action
```

Die Funktionen bleiben semantisch erhalten.

## Breakpoints

NovaOS darf Breakpoints unterstützen.

```text
Compact
Medium
Expanded
```

Breakpoints sollen jedoch auf Layoutbedingungen basieren und nicht auf festen Gerätebezeichnungen wie:

```text
Phone
Tablet
Desktop
```

## Adaptive Komponenten

Ein UI-Element darf abhängig vom verfügbaren Raum eine alternative Darstellung verwenden.

```text
Expanded:

[Icon] Save Document

Compact:

[Icon]
```

Beide Darstellungen repräsentieren dieselbe semantische Action.

## Input-Methode

Responsive UI darf die aktive Eingabemethode berücksichtigen.

```text
Mouse
Touch
Pen
Keyboard
Accessibility Input
```

Beispielsweise dürfen Touch-Ziele größer dargestellt werden.

Die funktionale Bedeutung darf sich dadurch nicht ändern.

## Skalierung

Responsive Layout und Display Scaling bleiben getrennte Mechanismen.

```text
Display Scaling
      ↓
Logical Coordinate Space
      ↓
Responsive Layout
```

Responsive UI arbeitet bevorzugt mit logischen Größen statt physischen Pixeln.

## Fenstergrößen

Responsive Verhalten muss auch bei normalen Desktop-Fenstern funktionieren.

```text
Large Window
    ↓
Medium Window
    ↓
Small Window
```

Responsive UI ist daher nicht auf mobile Geräte beschränkt.

## Multi-Display

Wird ein Fenster zwischen Displays verschoben, kann sich ändern:

```text
Available Size
Scaling
Orientation
Input Context
```

Die UI darf darauf ohne Neustart reagieren.

## Kontinuität

Responsive Änderungen sollen den aktuellen Nutzerzustand erhalten.

Beispiele:

```text
Selection
Scroll Position
Focused Object
Opened Document
Current Task
Input State
```

Ein Layoutwechsel darf nicht unnötig den Arbeitskontext verlieren.

## Accessibility

Accessibility-Anforderungen werden als Constraints berücksichtigt.

```text
Available Space
      +
Accessibility Requirements
      ↓
Responsive Layout
```

Beispielsweise darf größere Schrift nicht einfach abgeschnitten werden, nur um ein ursprüngliches Layout beizubehalten.

## Adaptive UI

Responsive und Adaptive UI werden getrennt betrachtet.

```text
Responsive UI
→ reagiert auf konkrete Layoutbedingungen

Adaptive UI
→ reagiert auf Kontext und erlernte Nutzung
```

Beide Mechanismen dürfen zusammenarbeiten.

## Performance

Layoutänderungen sollen nur betroffene UI-Bereiche neu berechnen.

```text
Size Change
    ↓
Affected Layout Nodes
    ↓
Incremental Layout
    ↓
Damage
```

Ein Resize darf nicht automatisch einen vollständigen Neuaufbau der UI verursachen.

## Normative Anforderungen

1. Native NovaOS-UI MUSS responsive Layouts unterstützen.
2. Responsive Verhalten MUSS auf dem tatsächlich verfügbaren Darstellungsraum basieren können.
3. Anwendungen SOLLEN nicht separate UIs für einzelne Geräteklassen benötigen.
4. Layoutregeln SOLLEN mit logischen Größen statt physischen Pixeln arbeiten.
5. UI-Elemente DÜRFEN alternative Darstellungen für unterschiedliche Platzverhältnisse besitzen.
6. Alternative Darstellungen DÜRFEN die semantische Bedeutung einer Funktion nicht verändern.
7. Responsive Layout MUSS normale Fenstergrößenänderungen unterstützen.
8. Änderungen von Display, Skalierung und Orientierung MÜSSEN dynamisch verarbeitet werden können.
9. Input-Methoden DÜRFEN die geeignete Darstellung beeinflussen.
10. Accessibility-Anforderungen MÜSSEN bei Layoutentscheidungen berücksichtigt werden.
11. Responsive Änderungen SOLLEN Nutzerzustand und Arbeitskontext erhalten.
12. Layoutänderungen SOLLEN inkrementell verarbeitet werden.
13. Responsive UI MUSS mit Declarative, Semantic und Adaptive UI kombinierbar sein.

## Abhängigkeiten

- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-ADAPTIVE-0001`
- `NPSPEC-UI-RETAINEDMODE-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `NPSPEC-UI-DISPLAYSERVER-0001`

## Ergebnis

NovaOS verwendet keine starren Oberflächen für bestimmte Geräteklassen.

```text
Semantic UI
     ↓
Current Constraints
     ↓
Responsive Layout
     ↓
Appropriate Representation
```

Dieselbe UI kann sich dadurch von einem großen Desktopfenster bis zu einer kleinen oder touchoptimierten Darstellung skalieren, während Funktion, Zustand und semantische Bedeutung erhalten bleiben.