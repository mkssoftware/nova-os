# NPSPEC-UI-FRAMESCHEDULING-0001 – Frame Scheduling

## Status

Angenommen

## Kategorie

UI / Rendering / Scheduling / Presentation

## Zweck

NovaOS verwendet einen zentral koordinierten Frame Scheduler, um Rendering, Composition und Display-Ausgabe zeitlich aufeinander abzustimmen.

Ziele sind:

```text
Niedrige Eingabelatenz
Flüssige Darstellung
Stabile Frame Times
Wenig unnötige Frames
Effiziente Ressourcennutzung
Synchronisation mit dem Display
```

Der Frame Scheduler entscheidet, **wann ein neuer Frame tatsächlich benötigt wird**.

## Architektur

```text
Input / UI Changes
        ↓
Damage Tracking
        ↓
Frame Scheduler
        ↓
Render / Composition
        ↓
Display Server
        ↓
Presentation
```

Der Scheduler koordiniert die beteiligten UI-Komponenten, ohne deren eigentliche Rendering-Aufgaben zu übernehmen.

## Frame Trigger

Ein neuer Frame kann ausgelöst werden durch:

```text
Surface Damage
Scene Change
User Input
Animation
Video
Window Movement
Display Change
Presentation Request
```

Ohne relevanten Änderungsgrund muss kein neuer Frame erzeugt werden.

## Presentation Deadline

Der Display Server stellt Informationen über den nächsten möglichen Presentation-Zeitpunkt bereit.

```text
Next Presentation
       ↓
Frame Deadline
       ↓
Render Deadline
       ↓
Composition Deadline
```

Der Scheduler plant rückwärts vom gewünschten Presentation-Zeitpunkt.

## Frame Pipeline

Die grundlegende Pipeline lautet:

```text
Collect Changes
      ↓
Update UI State
      ↓
Layout
      ↓
Render
      ↓
Composition
      ↓
Present
```

Nicht benötigte Schritte dürfen übersprungen werden.

Beispiel:

```text
Opacity Change
     ↓
No Layout
     ↓
Composition only
```

## Adaptive Scheduling

Der Scheduler darf historische Laufzeiten verwenden, um den optimalen Startzeitpunkt eines Frames zu bestimmen.

```text
Previous Frame Times
        ↓
Estimated Work
        ↓
Scheduling Decision
```

Dabei können berücksichtigt werden:

```text
Render Time
Composition Time
GPU Load
Display Refresh
Damage Size
Animation State
```

Adaptive Entscheidungen dürfen keine harten Timing-Anforderungen verletzen.

## Input Latency

Benutzereingaben sollen möglichst im nächsten sinnvoll erreichbaren Frame sichtbar werden.

```text
Input
  ↓
State Update
  ↓
Next Frame
  ↓
Presentation
```

Der Scheduler soll unnötiges frühzeitiges Rendering vermeiden, damit aktuelle Eingaben möglichst spät vor der Deadline noch berücksichtigt werden können.

## Frame Coalescing

Mehrere Änderungen dürfen zu einem Frame zusammengeführt werden.

```text
Change A
Change B
Change C
   ↓
Single Frame
```

Dadurch werden unnötige Zwischenframes vermieden.

## Frame Skipping

Kann eine Deadline nicht sinnvoll erreicht werden, darf ein Frame übersprungen werden.

```text
Frame N
   ↓
Deadline cannot be met
   ↓
Skip
   ↓
Frame N+1
```

Ein kontrolliert ausgelassener Frame ist einem verspäteten Frame mit zusätzlicher Latenz vorzuziehen, sofern keine harte Timing-Anforderung besteht.

## Variable Refresh Rate

Displays mit Variable Refresh Rate dürfen dynamisch angesteuert werden.

```text
Frame Ready
    ↓
Presentation
    ↓
Display Refresh
```

Damit muss NovaOS nicht zwingend auf ein festes Refresh-Raster warten.

## Multi-Display

Jedes Display kann eigene Timing-Eigenschaften besitzen.

```text
Display A → 60 Hz
Display B → 144 Hz
Display C → VRR
```

Der Scheduler muss unterschiedliche Presentation-Zeitlinien unterstützen können.

Eine gemeinsame Scene darf dadurch nicht automatisch an die langsamste Display-Frequenz gebunden werden.

## Ressourcenpriorität

Interaktive UI-Arbeit erhält gegenüber rein dekorativer Arbeit höhere Priorität.

```text
Input Response
Window Movement
Essential UI
      ↓
Decorative Animation
Background Effect
```

Bei Ressourcenknappheit dürfen optionale Effekte reduziert werden.

## Backpressure

Produziert eine Anwendung Frames schneller als diese dargestellt werden können, muss Backpressure möglich sein.

```text
Application
    ↓
Frame Queue Limit
    ↓
Compositor
    ↓
Display
```

Unbegrenzte Frame Queues sind zu vermeiden, da sie Speicherverbrauch und Eingabelatenz erhöhen.

## Observability

Für Diagnose und Optimierung sollen mindestens messbar sein:

```text
Frame Start
Render Time
Composition Time
Presentation Time
Missed Deadline
Dropped Frame
Input-to-Presentation Latency
```

Diese Daten können für Performanceanalyse und adaptive Planung verwendet werden.

## Fehlerbehandlung

Kann ein Frame nicht rechtzeitig erzeugt werden, muss der letzte gültige Frame weiterhin dargestellt werden können.

```text
New Frame Failure
       ↓
Keep Last Valid Frame
       ↓
Continue Scheduling
```

Ein einzelner Frame-Fehler darf den UI-Stack nicht blockieren.

## Normative Anforderungen

1. NovaOS MUSS UI-Frames über einen definierten Frame Scheduler koordinieren.
2. Frames SOLLEN nur bei tatsächlichem Bedarf erzeugt werden.
3. Damage Tracking MUSS als Scheduling-Signal verwendet werden können.
4. Presentation Timing des Displays MUSS berücksichtigt werden.
5. Mehrere Änderungen SOLLEN zu einem Frame zusammengeführt werden können.
6. Input-to-Presentation-Latenz SOLL minimiert werden.
7. Unbegrenzte Frame Queues DÜRFEN nicht verwendet werden.
8. Backpressure MUSS unterstützt werden.
9. Verpasste Frames MÜSSEN kontrolliert behandelt werden.
10. Unterschiedliche Display-Refresh-Raten MÜSSEN unterstützt werden.
11. Variable Refresh Rate SOLL unterstützt werden.
12. Adaptive Planung DARF harte Timing-Constraints nicht verletzen.
13. Interaktive UI-Arbeit SOLL gegenüber dekorativen Effekten priorisiert werden.
14. Frame Timing und Deadline Misses MÜSSEN beobachtbar sein.

## Abhängigkeiten

- `NPSPEC-UI-DISPLAYSERVER-0001`
- `NPSPEC-UI-COMPOSITOR-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-DAMAGETRACKING-0001`
- `NPSPEC-UI-GPUCOMPOSITION-0001`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`
- `ADR-ADAPTIVE-0004_Adaptive_Scheduling`

## Ergebnis

NovaOS plant UI-Frames nach dem Prinzip:

```text
Change / Input
      ↓
Determine Required Work
      ↓
Schedule toward Presentation Deadline
      ↓
Render only what changed
      ↓
Compose
      ↓
Present
```

Damit richtet sich die UI nicht nach einer starren Render-Schleife, sondern erzeugt Frames bedarfsgerecht und zeitlich koordiniert mit Eingaben, Änderungen und den tatsächlichen Eigenschaften des Displays.