# NPSPEC-BOOTUI-0004
# Boot UI State Model

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0004 |
| Titel | Boot UI State Model |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001, NPSPEC-BOOTUI-0002, NPSPEC-BOOTUI-0003 |
| ADRs | ADR-BOOTCONTROL-0002, ADR-BOOTCONTROL-0003, ADR-BOOTNAV-0001, ADR-BOOTNAV-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige Zustandsmodell (State Model) der Boot-UI.

Sie beschreibt:

- sämtliche Zustände
- erlaubte Zustandsübergänge
- Zustandsautomaten
- Objektzustände
- Szenenzustände
- Dialogzustände
- Navigationszustände
- Eingabezustände
- Renderzustände
- Fehlerzustände

Das State Model stellt sicher, dass sich jedes Objekt der Boot-UI jederzeit in einem **eindeutig definierten Zustand** befindet. Dadurch werden inkonsistente Zustände, unzulässige Übergänge und nicht reproduzierbares Verhalten verhindert. Deterministische Zustandsübergänge gelten als Grundprinzip der Runtime. :contentReference[oaicite:0]{index=0}

---

# 2. Architekturprinzipien

Das Zustandsmodell basiert auf folgenden Regeln:

- Jeder Zustand besitzt genau einen Besitzer.
- Jeder Zustandswechsel erfolgt atomar.
- Ein Objekt besitzt immer genau einen Hauptzustand.
- Zustandswechsel dürfen ausschließlich über definierte Transitionen erfolgen.
- Jeder Zustandswechsel erzeugt Ereignisse.
- Jeder Zustandswechsel kann protokolliert werden.
- Fehlerzustände besitzen definierte Recovery-Wege.
- Ungültige Übergänge sind verboten.

---

# 3. Globales Runtime-Zustandsmodell

```
Created
↓
Initializing
↓
Loading
↓
Ready
↓
Running
↓
Recovery
↓
Shutdown
↓
Destroyed
```

---

# 4. Zustandsbeschreibung

## Created

Runtime wurde erzeugt.

Vorhanden:

- Speicherpools
- Runtime Context

Nicht vorhanden:

- Ressourcen
- Renderer
- Controls
- Eingaben

---

## Initializing

Subsysteme werden initialisiert.

Aktiv:

- Speicher
- Grafik
- Plattform
- Timer

Noch deaktiviert:

- Rendering
- Animation
- Navigation

---

## Loading

Lädt:

- Fonts
- Icons
- PNG
- SVG
- Theme
- Sprache
- Konfiguration

Alle Ressourcen müssen validiert werden.

---

## Ready

Alle Komponenten vorhanden.

Alle Layouts berechnet.

Keine Benutzereingaben.

---

## Running

Normale Laufzeit.

Alle Subsysteme aktiv.

---

## Recovery

Recovery-Modus.

Nur kritische Komponenten aktiv.

Animationen können deaktiviert werden.

---

## Shutdown

Boot UI beendet.

Keine neuen Eingaben.

Keine Animationen.

---

## Destroyed

Alle Ressourcen freigegeben.

Alle Speicherbereiche zurückgegeben.

Runtime beendet.

---

# 5. Runtime-Zustandsautomat

```
Created
↓
Initializing
↓
Loading
↓
Ready
↓
Running
↓
Shutdown
↓
Destroyed
```

Fehler:

```
Running
↓
Recovery
↓
Running
```

oder

```
Recovery
↓
Shutdown
```

---

# 6. Scene-Zustände

Jede Scene besitzt:

```
Created
↓
Building
↓
Layout
↓
Visible
↓
Running
↓
Hidden
↓
Destroyed
```

---

# 7. Dialog-Zustände

```
Created
↓
Fade In
↓
Visible
↓
Focused
↓
Closing
↓
Fade Out
↓
Destroyed
```

Während **Focused**

- Navigation blockiert
- Hintergrund deaktiviert
- Fokus exklusiv

---

# 8. Overlay-Zustände

```
Hidden
↓
Opening
↓
Visible
↓
Closing
↓
Hidden
```

---

# 9. Control-Zustände

Jedes Bedienelement besitzt folgenden Zustandsautomaten.

```
Created
↓
Initialized
↓
Visible
↓
Enabled
↓
Focused
↓
Pressed
↓
Released
↓
Focused
↓
Disabled
↓
Hidden
↓
Destroyed
```

---

# 10. Zustandsattribute

Jedes Control besitzt mindestens:

```cpp
StateID

Enabled

Visible

Focused

Pressed

Hovered

Selected

Checked

Animated

Dirty

Invalid
```

---

# 11. Fokusmodell

Der Fokus besitzt eigene Zustände.

```
No Focus
↓
Focused
↓
Captured
↓
Released
↓
No Focus
```

Nur ein Objekt darf gleichzeitig den Eingabefokus besitzen.

---

# 12. Mauszustände

```
Outside
↓
Hover
↓
Pressed
↓
Dragging
↓
Released
↓
Hover
↓
Outside
```

---

# 13. Tastaturzustände

```
Released
↓
Pressed
↓
Repeat
↓
Released
```

---

# 14. Touchzustände

```
Down
↓
Move
↓
Hold
↓
Release
```

---

# 15. Navigationszustände

```
Idle
↓
Navigating
↓
Transition
↓
Completed
↓
Idle
```

Während **Transition**

- keine neue Navigation
- keine Dialogöffnung
- keine Seitenänderung

---

# 16. Seitenzustände

```
Created
↓
Loading
↓
Active
↓
Inactive
↓
Destroyed
```

---

# 17. Animationszustände

```
Created
↓
Waiting
↓
Running
↓
Paused
↓
Finished
↓
Destroyed
```

Animationen dürfen nicht endlos laufen.

---

# 18. Renderzustände

```
Idle
↓
Dirty
↓
Rendering
↓
Present
↓
Idle
```

Nur **Dirty** Objekte werden neu gerendert.

---

# 19. Ressourcenzustände

```
Registered
↓
Loading
↓
Loaded
↓
Cached
↓
Referenced
↓
Released
↓
Destroyed
```

---

# 20. Themezustände

```
Default
↓
Loading
↓
Applying
↓
Active
↓
Updating
↓
Active
```

---

# 21. Fehlerzustände

```
Healthy
↓
Warning
↓
Recoverable
↓
Recovery
↓
Healthy
```

oder

```
Recoverable
↓
Fatal
↓
Shutdown
```

---

# 22. Zustandsübergänge

Jeder Übergang besitzt

```cpp
OldState
↓
Transition
↓
Validation
↓
Callbacks
↓
NewState
```

Ungültige Übergänge werden verworfen.

---

# 23. Transition-Events

Vor jedem Wechsel

```
OnExit()
```

Nach jedem Wechsel

```
OnEnter()
```

Während des Wechsels

```
OnTransition()
```

Optional

```
OnAnimation()

OnLayout()

OnRender()
```

---

# 24. Zustandshierarchie

```
Runtime
└── Scene
     └── Dialog
          └── Controls
                └── Animation
```

Kindobjekte dürfen niemals einen Zustand besitzen, der mit dem Zustand ihres Elternobjekts unvereinbar ist.

---

# 25. Synchronisation

Während eines Frames

dürfen

- State Update
- Layout
- Rendering
- Navigation

nicht gleichzeitig dieselben Objekte verändern.

Reihenfolge

```
Input
↓
State Update
↓
Animation
↓
Layout
↓
Render
```

---

# 26. Zustandsvalidierung

Vor jedem Wechsel werden geprüft

- Elternzustand
- Kindzustand
- Ressourcen
- Fokus
- Sichtbarkeit
- Animation
- Speicher
- Navigation

---

# 27. Recovery

Bei inkonsistentem Zustand

```
Rollback
↓
Restore
↓
Layout
↓
Render
↓
Continue
```

Falls nicht möglich
↓
Shutdown

---

# 28. Debuginformationen

Jedes Objekt besitzt

```cpp
Current State

Previous State

Time Stamp

Owner

Transition Counter

Error Counter
```

---

# 29. Logging

Alle State Changes können protokolliert werden.

Beispiel

```
[Scene]

Created
↓
Visible
↓
Running
↓
Hidden
↓
Destroyed
```

---

# 30. Performance

Maximale Zustandswechsel

```
100.000

Transitions

pro Sekunde
```

Keine Heap-Allokationen während State Updates.

---

# 31. Testanforderungen

Zu testen sind

## Runtime

- Initialisierung
- Recovery
- Shutdown

## Controls

- Fokus
- Press
- Disable
- Destroy

## Navigation

- Seitenwechsel
- Back
- Dialoge

## Animation

- Start
- Ende
- Pause
- Abbruch

## Fehler

- ungültige Zustände
- doppelte Übergänge
- Recovery

---

# 32. API

```cpp
State Create()

State Destroy()

State Transition()

State Validate()

State Rollback()

State Reset()

State Serialize()

State Deserialize()
```

---

# 33. Referenzen

## ADR

- ADR-BOOTCONTROL-0002 – Zustandsautomat für Controls
- ADR-BOOTCONTROL-0003 – Einheitliches Fokusmodell
- ADR-BOOTNAV-0001 – Zustandsbasierte Navigation
- ADR-BOOTNAV-0002 – Hierarchischer Navigations-Stack

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTUI-0002 – Boot UI Runtime
- NPSPEC-BOOTUI-0003 – Boot UI Lifecycle
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTNAV-0001 – Boot Navigation Model

---

# 34. Zusammenfassung

Das **Boot UI State Model** definiert sämtliche Zustände und Zustandsübergänge der Bootoberfläche. Alle Komponenten – Runtime, Szenen, Dialoge, Controls, Navigation, Eingabe, Animationen und Rendering – folgen einem einheitlichen, hierarchischen Zustandsmodell. Dadurch wird sichergestellt, dass jeder Zustandswechsel nachvollziehbar, atomar und deterministisch erfolgt. Das Modell bildet die Grundlage für eine robuste, reproduzierbare und wartbare Boot-UI-Architektur und ermöglicht konsistente Fehlerbehandlung sowie gezielte Wiederherstellungsmechanismen.