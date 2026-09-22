# NPSPEC-UI-FENSTER-0001 – Nova Fenster

## Status

Angenommen

## Kategorie

UI / Window / Desktop / Window Management

## Zweck

NovaOS verwendet Fenster als flexible Arbeitsbereiche für Anwendungen, Objekte und Capabilities.

Ein Fenster ist dabei keine einfache rechteckige Zeichenfläche, sondern ein vom Window Server verwaltetes UI-Objekt mit:

```text
Identität
Inhalt
Zustand
Position
Größe
Security Context
Capabilities
Semantik
```

## Grundprinzip

```text
Application / Capability
        ↓
Window
        ↓
Surface / UI Tree
        ↓
Window Server
        ↓
Scene Graph
        ↓
Compositor
```

Die Anwendung kontrolliert ihren Fensterinhalt, während der Window Server Position, Sichtbarkeit, Fokus und Integration in den Desktop verwaltet.

## Fensterobjekt

Ein Fenster besitzt mindestens:

```text
WindowID
Owner
Title
Semantic Role
Content
Bounds
State
Workspace
Display
Security Context
```

`WindowID` bleibt während der Lebensdauer des Fensters stabil.

## Fensterzustände

NovaOS unterstützt mindestens:

```text
Normal
Minimized
Maximized
Fullscreen
Hidden
Suspended
```

Zustandswechsel erfolgen kontrolliert über den Window Server.

## Fensterrahmen

Der Fensterrahmen wird bevorzugt durch NovaOS dargestellt.

Typische Elemente:

```text
Title
Application / Object Identity
Minimize
Maximize / Restore
Close
Window Menu
```

Das Design wird durch das Nova Theme System bestimmt.

## Client Area

Der eigentliche Anwendungsinhalt befindet sich in der Client Area.

```text
┌───────────────────────────────┐
│ Title                    ─ □ ×│
├───────────────────────────────┤
│                               │
│         Client Area           │
│                               │
└───────────────────────────────┘
```

Systemrahmen und Anwendungsinhalt bleiben logisch getrennt.

## Fensterbewegung

Fenster können verschoben werden.

```text
Input
  ↓
Window Move
  ↓
Window Server
  ↓
Scene Graph Transform
```

Eine Bewegung soll möglichst über Scene-Graph-Transformation erfolgen, ohne den Fensterinhalt vollständig neu rendern zu müssen.

## Größenänderung

Fenster können dynamisch skaliert werden.

```text
Resize
   ↓
New Bounds
   ↓
Responsive Layout
   ↓
Affected Damage
```

Die Anwendung erhält den neuen verfügbaren Inhaltsbereich und kann ihre deklarative UI entsprechend reconciliieren.

## Snap

NovaOS unterstützt das kontrollierte Andocken von Fenstern.

Beispiele:

```text
Left Half
Right Half
Top
Quarter
Custom Region
```

Zusätzlich dürfen komplexere Layouts angeboten werden.

```text
┌────────────┬────────────┐
│            │            │
│ Window A   │ Window B   │
│            ├────────────┤
│            │ Window C   │
└────────────┴────────────┘
```

## Snap Layouts

NovaOS kann vordefinierte oder dynamisch erzeugte Snap Layouts anbieten.

```text
50 / 50
70 / 30
Three Columns
Main + Side
Four Quadrants
```

Die verfügbaren Layouts dürfen sich an Displaygröße und verfügbarem Raum orientieren.

## Fenstergruppen

Zusammengehörige Fenster können logisch gruppiert werden.

```text
Window Group
 ├── Window A
 ├── Window B
 └── Window C
```

Eine Gruppe kann beispielsweise gemeinsam:

```text
verschoben
minimiert
wiederhergestellt
einem Workspace zugeordnet
```

werden.

Die einzelnen Fenster behalten trotzdem ihre eigene Identität.

## Tabs

Wo semantisch sinnvoll, können mehrere Inhalte innerhalb eines Fensters als Tabs organisiert werden.

```text
Window
 ├── Tab A
 ├── Tab B
 └── Tab C
```

Tabs sind jedoch kein Ersatz für unabhängige Fenster, wenn getrennte Fensterzustände erforderlich sind.

## Always-on-Top

Fenster dürfen eine erhöhte Z-Order anfordern.

```text
Normal Window
      ↓
Always-on-Top Request
      ↓
Window Server Policy
```

Eine Anwendung darf dadurch keine geschützte System- oder Security-UI überlagern.

## Fullscreen

Fullscreen kann für geeignete Inhalte verwendet werden.

```text
Window
   ↓
Fullscreen
   ↓
Optional Direct Scanout
   ↓
Display
```

Systemkritische Overlays müssen weiterhin dargestellt werden können.

## Multi-Display

Fenster können zwischen Displays verschoben werden.

```text
Display A
   ↓
Window
   ↓
Display B
```

Dabei können sich ändern:

```text
Scaling
Color Profile
Refresh Rate
VRR
Available Area
Orientation
```

Das Fenster muss sich ohne Neustart anpassen können.

## Workspaces

Fenster können virtuellen Workspaces zugeordnet werden.

```text
Window
 ├── Workspace 1
 └── Display A
```

Eine Änderung des Workspaces verändert nicht die Identität des Fensters.

## Fokus

Ein Fenster kann:

```text
Active
Inactive
Focused
Unfocused
```

sein.

Der Window Server kontrolliert den Fokus.

Eine Anwendung darf sich nicht unkontrolliert selbst in den Vordergrund setzen oder anderen Anwendungen den Fokus entziehen.

## Modale Fenster

NovaOS unterstützt modale Interaktionen.

```text
Parent Window
      ↓
Modal Dialog
```

Modalität soll möglichst auf den notwendigen Kontext begrenzt bleiben.

Ein Dialog einer Anwendung darf nicht den gesamten Desktop blockieren.

## Systemdialoge

Sicherheitskritische Dialoge besitzen eine geschützte Systemrolle.

Beispiele:

```text
Credential Prompt
Permission Request
Secure Confirmation
Unlock
```

Normale Anwendungen dürfen diese Rolle nicht übernehmen.

## Fenster und Capabilities

Ein Fenster kann eine Capability oder ein Objekt repräsentieren.

```text
Object
   ↓
Capability
   ↓
Window UI
```

Dadurch muss ein Fenster nicht zwingend einer klassischen monolithischen Anwendung entsprechen.

Beispiel:

```text
Image Object
    ↓
Image.Edit
    ↓
Editing Window
```

## Responsive UI

Fensterinhalt muss auf Größenänderungen reagieren können.

```text
Large Window
    ↓
Expanded Layout

Medium Window
    ↓
Compact Layout

Small Window
    ↓
Minimal Layout
```

Die semantische Funktion bleibt erhalten.

## Persistenz

Fensterzustände können gespeichert werden.

Beispiele:

```text
Position
Size
State
Workspace
Display Assignment
Window Group
```

Nach einer neuen Sitzung wird der gespeicherte Zustand mit der tatsächlich vorhandenen Umgebung reconciled.

Ein nicht mehr vorhandenes Display darf beispielsweise nicht dazu führen, dass ein Fenster unerreichbar bleibt.

## Animationen

Fensteroperationen dürfen animiert werden.

Beispiele:

```text
Open
Close
Minimize
Restore
Snap
Workspace Transition
```

Animationen sind dekorativ und dürfen reduziert oder deaktiviert werden.

Die eigentliche Fensteroperation darf nicht von der Animation abhängen.

## Ressourcenverhalten

Nicht sichtbare Fenster dürfen Ressourcen reduzieren.

```text
Visible
→ Normal Rendering

Occluded
→ Reduced Rendering

Minimized
→ Rendering largely suspended

Suspended
→ Minimal Resources
```

Der logische Fensterzustand bleibt dabei erhalten.

## Fehlerverhalten

Stürzt der Fensterbesitzer ab:

```text
Application Failure
       ↓
Window Server detects loss
       ↓
Window becomes invalid
       ↓
Controlled Removal / Recovery
```

Ein fehlerhaftes Fenster darf weder Window Server noch Desktop blockieren.

## Sicherheit

Fenstergrenzen erzeugen keine Berechtigung zwischen Anwendungen.

Eine Anwendung darf nicht ohne Capability:

```text
fremde Fensterinhalte lesen
fremde Fenster verändern
Input anderer Fenster empfangen
geschützte Overlays erzeugen
System-UI imitieren
```

## Accessibility

Fenster müssen im Accessibility Tree als eigenständige semantische Bereiche erscheinen.

Mindestens folgende Operationen müssen zugänglich sein:

```text
Activate
Move
Resize
Minimize
Maximize
Restore
Close
```

## Normative Anforderungen

1. Fenster MÜSSEN durch den Nova Window Server verwaltet werden.
2. Jedes Fenster MUSS eine stabile `WindowID` besitzen.
3. Anwendung und Fensterverwaltung MÜSSEN logisch getrennt sein.
4. Fenster MÜSSEN Normal, Minimized, Maximized und Fullscreen unterstützen können.
5. Fensterbewegungen SOLLEN ohne vollständiges Neurendering des Inhalts möglich sein.
6. Größenänderungen MÜSSEN responsive UI-Aktualisierungen auslösen können.
7. Snap und Snap Layouts SOLLEN unterstützt werden.
8. Fenstergruppen SOLLEN unterstützt werden.
9. Fenster MÜSSEN zwischen Displays und Workspaces verschiebbar sein.
10. Anwendungen DÜRFEN den globalen Fokus nicht unkontrolliert übernehmen.
11. Always-on-Top DARF geschützte System-UI nicht überlagern.
12. Modalität MUSS auf den notwendigen Kontext begrenzt werden.
13. Nicht sichtbare Fenster SOLLEN ihren Ressourcenverbrauch reduzieren können.
14. Gespeicherte Fensterzustände MÜSSEN mit der aktuellen Displayumgebung reconciled werden.
15. Der Ausfall eines Fensters DARF Desktop oder Window Server nicht blockieren.
16. Anwendungen DÜRFEN ohne entsprechende Capability keine fremden Fensterinhalte lesen oder verändern.
17. Fensteroperationen MÜSSEN über Accessibility erreichbar sein.
18. Fenster DÜRFEN Capabilities und Objekte unabhängig von klassischen Anwendungen repräsentieren.

## Abhängigkeiten

- `NPSPEC-UI-WINDOWSERVER-0001`
- `NPSPEC-UI-DESKTOP-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-COMPOSITOR-0001`
- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-RESPONSIVE-0001`
- `NPSPEC-UI-COMPONENT-0001`
- `NPSPEC-UI-THEME-0001`
- `NPSPEC-UI-INPUTROUTING-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `NPSPEC-UI-CAPABILITYDRIVEN-0001`
- `NPSPEC-UI-FRAMESCHEDULING-0001`

## Ergebnis

NovaOS behandelt Fenster als verwaltete Arbeitskontexte:

```text
Object / Capability / Application
              ↓
            Window
              ↓
       Window Server
              ↓
   Workspace + Display
              ↓
         Scene Graph
              ↓
         Compositor
```

Damit bleiben Fenster leicht, responsive, sicher und unabhängig von der klassischen Annahme, dass jedes Fenster zwingend zu einer monolithischen Anwendung gehören muss.