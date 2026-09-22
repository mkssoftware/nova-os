# NPSPEC-UI-INPUTROUTING-0001 – Input Routing

## Status

Angenommen

## Kategorie

UI / Input / Routing / Security

## Zweck

NovaOS verwendet einen zentral kontrollierten Input Router, der Eingaben von Eingabegeräten an den jeweils berechtigten UI-Kontext weiterleitet.

Unterstützte Eingabequellen umfassen unter anderem:

```text
Keyboard
Mouse
Touch
Pen
Touchpad
Game Controller
Accessibility Input
Virtual Input
```

Input Routing trennt physische Eingabegeräte von Anwendungen und verhindert direkten unkontrollierten Zugriff auf globale Eingaben.

## Architektur

```text
Input Device
     ↓
Input Driver
     ↓
Input Service
     ↓
Input Router
     ↓
Window / Surface / UI Element
```

Anwendungen erhalten standardmäßig keine direkten Raw-Events physischer Eingabegeräte.

## Input Event

Ein normalisiertes Input Event enthält mindestens:

```text
EventID
DeviceID
EventType
Timestamp
Input Data
Security Context
```

Gerätespezifische Rohdaten werden durch die Input-Schicht in definierte NovaOS-Events überführt.

## Routing

Der Input Router bestimmt das Ziel anhand von:

```text
Active Session
Focus
Pointer Position
Scene Graph
Input Capture
Security Context
System UI State
```

Der Scene Graph kann für Hit Testing verwendet werden.

```text
Pointer Position
      ↓
Scene Graph Hit Test
      ↓
Target Window
      ↓
Target UI Element
```

## Keyboard Focus

Tastatureingaben werden grundsätzlich an den aktiven Keyboard-Focus weitergeleitet.

```text
Keyboard
   ↓
Focused Window
   ↓
Focused UI Element
```

Focus-Wechsel müssen durch den Window Server kontrolliert werden.

Eine Anwendung darf sich nicht beliebig globalen Keyboard-Focus zuweisen.

## Pointer Routing

Pointer-Eingaben werden anhand ihrer Position und der sichtbaren Scene bestimmt.

```text
Pointer Event
     ↓
Coordinates
     ↓
Hit Test
     ↓
Topmost Eligible Target
```

Clipping, Transformationen, Sichtbarkeit und Z-Order müssen berücksichtigt werden.

## Touch und Pen

Touch- und Pen-Eingaben besitzen eine stabile Zuordnung während einer Interaktion.

```text
Touch Begin
    ↓
Target Selection
    ↓
Touch Move
    ↓
Same Interaction Target
    ↓
Touch End
```

Ein laufender Kontakt darf nicht allein durch eine Veränderung der Z-Order unkontrolliert zu einer anderen Anwendung wechseln.

## Input Capture

Ein UI-Kontext kann für definierte Interaktionen temporär Input Capture erhalten.

Beispiele:

```text
Drag & Drop
Window Resize
Menu Interaction
Pointer Drag
```

Capture muss explizit, begrenzt und widerrufbar sein.

## System Shortcuts

Bestimmte Eingabekombinationen werden vor der Weiterleitung an Anwendungen durch NovaOS verarbeitet.

```text
Input
  ↓
System Shortcut Check
  ├── System Action
  └── Application Routing
```

Sicherheitskritische Systemkombinationen dürfen nicht durch normale Anwendungen abgefangen oder blockiert werden.

## Secure Input

Security UI kann einen geschützten Input-Modus aktivieren.

Beispiele:

```text
Login
Unlock
Credential Prompt
Permission Confirmation
Secure Attention
```

Während Secure Input dürfen nicht autorisierte Anwendungen die betreffenden Eingaben weder empfangen noch beobachten.

## Raw Input

Direkterer Zugriff auf Geräteereignisse kann für spezielle Anwendungen erlaubt werden.

```text
Input Device
     ↓
Restricted Raw Input Capability
     ↓
Authorized Application
```

Raw Input benötigt eine explizite Capability und darf geschützte Systemeingaben nicht umgehen.

## Virtueller Input

Software darf virtuelle Eingabequellen bereitstellen, wenn sie entsprechend autorisiert ist.

```text
Remote Desktop
Accessibility Service
Automation
Virtual Device
```

Virtuelle Events müssen als solche identifizierbar bleiben.

Sie dürfen nicht automatisch dieselbe Vertrauensstellung wie physische oder sichere Eingaben erhalten.

## Multi-Session

Input wird einem aktiven Session-Kontext zugeordnet.

```text
Physical Input
      ↓
Active Seat / Session
      ↓
Session Input Router
      ↓
Application
```

Eine Session darf nicht ohne explizite Autorisierung Eingaben einer anderen Session beobachten.

## Event Ordering

Input Events müssen innerhalb eines relevanten Eingabestroms in definierter Reihenfolge verarbeitet werden.

Zeitstempel stammen aus einer geeigneten monotonen Zeitbasis.

```text
Event A
   ↓
Event B
   ↓
Event C
```

## Latenz

Der Input-Pfad soll möglichst kurz bleiben.

```text
Physical Input
      ↓
Routing
      ↓
Application State
      ↓
Frame Scheduling
      ↓
Presentation
```

Input Events sollen unmittelbar als Signal für das Frame Scheduling verwendet werden können.

## Fehlerbehandlung

Fällt ein Ziel während einer Interaktion aus, muss dessen Input-Zustand kontrolliert beendet werden.

```text
Target Crash
    ↓
Release Focus / Capture
    ↓
Discard or Reroute safely
```

Input Capture darf niemals dauerhaft durch einen abgestürzten Prozess blockiert bleiben.

## Normative Anforderungen

1. Eingaben MÜSSEN über einen kontrollierten Input Router geleitet werden.
2. Anwendungen DÜRFEN globale Eingaben nicht standardmäßig beobachten.
3. Keyboard Input MUSS anhand des aktiven Focus geroutet werden.
4. Pointer Routing MUSS Scene Graph, Z-Order und Transformationen berücksichtigen.
5. Laufende Touch- und Pen-Interaktionen MÜSSEN eine stabile Zielzuordnung besitzen.
6. Input Capture MUSS explizit, begrenzt und widerrufbar sein.
7. Sicherheitskritische Systemeingaben DÜRFEN nicht durch normale Anwendungen blockiert werden.
8. Secure Input MUSS gegenüber normalen Anwendungen isoliert sein.
9. Raw Input MUSS eine explizite Capability erfordern.
10. Virtuelle Eingabequellen MÜSSEN identifizierbar sein.
11. Sessions MÜSSEN hinsichtlich ihrer Input Streams isoliert sein.
12. Input Events MÜSSEN geeignete monotone Zeitstempel besitzen.
13. Input MUSS als Signal für Frame Scheduling verwendet werden können.
14. Focus und Capture MÜSSEN beim Ausfall ihres Besitzers sicher freigegeben werden.

## Abhängigkeiten

- `NPSPEC-UI-WINDOWSERVER-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-FRAMESCHEDULING-0001`
- `ADR-CAPABILITY-0001`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-VERIFY-0005_Capability_Safety`

## Ergebnis

NovaOS behandelt Eingaben als kontrollierten Systemdatenstrom:

```text
Device
   ↓
Normalize
   ↓
Authorize
   ↓
Route
   ↓
Focused / Hit-Tested Target
   ↓
UI Update
   ↓
Frame
```

Damit erhalten Anwendungen nur die für ihren UI-Kontext bestimmten Eingaben, während Focus, Secure Input, globale Systemaktionen und Session-Grenzen zentral durch NovaOS geschützt werden.