# NPSPEC-BOOTUI-0003
# Boot UI Lifecycle

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0003 |
| Titel | Boot UI Lifecycle |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Abhängigkeiten | NPSPEC-BOOTUI-0001, NPSPEC-BOOTUI-0002 |
| Nachfolger | NPSPEC-BOOTUI-0004, NPSPEC-BOOTRENDER-0001 |
| ADRs | ADR-BOOTUI-0001 bis ADR-BOOTUI-0012 |

---

# 1. Ziel

Diese Spezifikation beschreibt den vollständigen Lebenszyklus (**Lifecycle**) der grafischen Benutzeroberfläche des NovaOS-Bootmanagers.

Der Lifecycle definiert eindeutig:

- wann Komponenten erzeugt werden
- wann Ressourcen geladen werden
- wann Eingaben erlaubt sind
- wann Animationen starten
- wann gerendert wird
- wann Komponenten zerstört werden
- wann Speicher freigegeben wird
- wie Fehler behandelt werden
- wie Fallbacks aktiviert werden

Der Lifecycle stellt sicher, dass **jede Boot-Sitzung vollständig deterministisch** abläuft. Ein klar definierter Lifecycle reduziert Fehler und macht das Verhalten reproduzierbar – ein zentrales Architekturziel für den frühen Bootprozess. :contentReference[oaicite:0]{index=0}

---

# 2. Architekturziele

Der Lifecycle muss gewährleisten:

- deterministische Initialisierung
- reproduzierbare Zustände
- definierte Übergänge
- keine Race Conditions
- keine unkontrollierten Ressourcen
- vollständige Wiederherstellbarkeit
- vollständige Diagnosefähigkeit
- kontrolliertes Shutdown-Verhalten

---

# 3. Gesamtübersicht

```
Power On

↓

Firmware

↓

Bootloader

↓

Bootmanager

↓

Boot UI Runtime

↓

Boot UI Lifecycle

↓

Kernel Start
```

Innerhalb des Boot UI Lifecycles existieren mehrere klar definbare Phasen.

---

# 4. Lifecycle-Phasen

```
Created

↓

Initializing

↓

Loading Resources

↓

Building Scene

↓

Layout

↓

Ready

↓

Running

↓

Suspended

↓

Recovery

↓

Shutdown

↓

Destroyed
```

Ein Zustand darf ausschließlich über definierte Übergänge verlassen werden.

---

# 5. Phase: Created

## Zweck

Die Runtime wird erzeugt.

Noch keine Ressourcen.

Noch kein Rendering.

## Initialisiert werden

- Runtime Context
- Global State
- Memory Pools
- Configuration Container

## Nicht erlaubt

- Rendering
- Animation
- Eingaben

---

# 6. Phase: Initializing

Initialisierung sämtlicher Kernsysteme.

## Reihenfolge

```
Memory

↓

Platform

↓

Graphics

↓

Input

↓

Diagnostics

↓

Resource Manager

↓

Scene Manager

↓

Motion

↓

Renderer
```

Falls eine Initialisierung fehlschlägt

↓

Recovery

---

# 7. Phase: Loading Resources

Alle statischen Ressourcen werden geladen.

## Ressourcen

- Fonts
- Icons
- PNG
- SVG
- Theme
- Animationen
- Konfiguration
- Sprachdateien

Alle Ressourcen werden

- validiert
- versioniert
- gehasht
- zwischengespeichert

---

# 8. Phase: Building Scene

Erzeugung des Scene Graphs.

```
Root

↓

Desktop

↓

Panels

↓

Controls

↓

Dialogs

↓

Overlays
```

Alle Controls werden erzeugt.

Noch keine Animation.

---

# 9. Phase: Layout

Berechnung aller Layouts.

## Aufgaben

- DPI
- Skalierung
- Position
- Größen
- Grid
- Stack
- Constraints

Nach Abschluss besitzt jedes Objekt

- Position
- Größe
- Clipping
- Sichtbarkeit

---

# 10. Phase: Ready

Die Oberfläche ist vollständig aufgebaut.

Alle Ressourcen vorhanden.

Alle Controls vorhanden.

Alle Layouts berechnet.

Alle Animationen vorbereitet.

Noch keine Benutzereingaben.

---

# 11. Phase: Running

Normale Laufzeit.

Die Runtime verarbeitet permanent

```
Input

↓

Update

↓

Animation

↓

Layout

↓

Dirty Detection

↓

Rendering

↓

Present
```

Diese Schleife endet erst durch

- Kernelstart
- Recovery
- Shutdown

---

# 12. Update-Zyklus

Jeder Frame besitzt exakt dieselben Schritte.

```
Input

↓

Event Queue

↓

Control Update

↓

Application Logic

↓

Animation

↓

Layout

↓

Dirty Detection

↓

Render Queue

↓

Compositor

↓

Present

↓

Diagnostics
```

Kein Schritt darf übersprungen werden.

---

# 13. Input Lifecycle

```
Hardware

↓

Platform

↓

Input Driver

↓

Input Manager

↓

Focus Manager

↓

Scene

↓

Control

↓

Application
```

Nach erfolgreicher Verarbeitung

↓

Event beendet

---

# 14. Animation Lifecycle

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

Animationen besitzen keine Endlosschleifen.

---

# 15. Dialog Lifecycle

```
Create

↓

Fade In

↓

Visible

↓

Interaction

↓

Fade Out

↓

Destroy
```

Während "Visible"

- Fokus exklusiv
- Hintergrund gesperrt
- Navigation blockiert

---

# 16. Resource Lifecycle

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

Referenzzählung verhindert doppelte Freigaben.

---

# 17. Control Lifecycle

```
Created

↓

Initialized

↓

Visible

↓

Focused

↓

Pressed

↓

Disabled

↓

Hidden

↓

Destroyed
```

Nicht jeder Zustand muss durchlaufen werden.

---

# 18. Scene Lifecycle

```
Create Scene

↓

Populate

↓

Layout

↓

Visible

↓

Running

↓

Destroy
```

Vor Destroy

- Animation stoppen
- Ressourcen freigeben
- Referenzen entfernen

---

# 19. Theme Lifecycle

```
Load

↓

Validate

↓

Apply

↓

Notify Controls

↓

Render

↓

Cache
```

Themewechsel erfolgt atomar.

Währenddessen

keine Benutzereingaben.

---

# 20. Error Lifecycle

```
Fehler erkannt

↓

Diagnose

↓

Recovery

↓

Fallback

↓

Weiterarbeiten
```

Beispiele

```
SVG Fehler

↓

PNG

↓

Placeholder
```

oder

```
Font Fehler

↓

Fallback Font
```

Die UI bleibt bedienbar.

---

# 21. Recovery Lifecycle

```
Running

↓

Error

↓

Safe State

↓

Fallback aktivieren

↓

Layout neu

↓

Rendering

↓

Running
```

Falls nicht möglich

↓

Shutdown

---

# 22. Shutdown Lifecycle

```
Animation stoppen

↓

Input deaktivieren

↓

Dialogs schließen

↓

Renderer stoppen

↓

Caches freigeben

↓

Scene löschen

↓

Runtime beenden
```

Danach

Kernelstart

oder

Firmware

---

# 23. Speicher-Lifecycle

```
Allocate

↓

Initialize

↓

Use

↓

Cache

↓

Release

↓

Reuse
```

Keine Speicherlecks erlaubt.

Keine Fragmentierung.

---

# 24. Ereignisreihenfolge

Alle Ereignisse besitzen

```
Created

↓

Queued

↓

Processing

↓

Completed

↓

Disposed
```

Ein Event darf niemals mehrfach verarbeitet werden.

---

# 25. Zustandsautomat

```
Created

↓

Initializing

↓

Ready

↓

Running

↓

Suspended

↓

Recovery

↓

Shutdown

↓

Destroyed
```

Ungültige Übergänge

Beispiele

```
Destroyed

↓

Running
```

sind verboten.

---

# 26. Watchdog

Der Lifecycle wird permanent überwacht.

Kontrolliert werden

- Renderzeit
- Animationen
- Speicher
- Event Queue
- Ressourcen
- Deadlocks

Bei Timeout

↓

Recovery

---

# 27. Synchronisationsregeln

Während eines Frames

dürfen

- Layout
- Rendering
- Animation
- Ressourcen

nicht gleichzeitig dieselben Objekte verändern.

Änderungen erfolgen ausschließlich innerhalb definierter Updatephasen.

---

# 28. Performancevorgaben

| Bereich | Ziel |
|----------|------|
| Runtime Initialisierung | < 200 ms |
| Ressourcen laden | < 100 ms |
| Layout | < 2 ms |
| Animation | < 2 ms |
| Rendering | < 8 ms |
| Gesamtframe | < 16 ms |
| Zielbildrate | 60 FPS |
| Minimal | 30 FPS |

---

# 29. Testanforderungen

Es müssen getestet werden

## Lifecycle

- Initialisierung
- Recovery
- Shutdown

## Ressourcen

- fehlende Dateien
- beschädigte Assets
- ungültige Versionen

## Controls

- Erzeugen
- Anzeigen
- Entfernen

## Dialoge

- Öffnen
- Schließen
- Fokus

## Animationen

- Start
- Pause
- Ende
- Abbruch

## Performance

- Speicher
- FPS
- Renderzeit
- Startzeit

---

# 30. Referenzen

## ADR

- ADR-BOOTUI-0001 – Eigene Boot-UI-Engine
- ADR-BOOTUI-0006 – Retained-Mode-Szenengraph
- ADR-BOOTUI-0007 – Dirty-Region-Rendering
- ADR-BOOTMOTION-0001 – Zeitbasierte Animationen
- ADR-BOOTSAFE-0003 – Watchdog

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTUI-0002 – Boot UI Runtime
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager

---

# 31. Zusammenfassung

Der **Boot UI Lifecycle** definiert den vollständigen Lebenszyklus der Benutzeroberfläche – von der Erzeugung über Initialisierung, Ressourcenverwaltung, Szenenaufbau, Layout, Laufzeit, Fehlerbehandlung und Recovery bis hin zum kontrollierten Shutdown. Durch klar definierte Zustandsübergänge, eine feste Frame-Verarbeitungspipeline und deterministische Abläufe wird sichergestellt, dass die Bootoberfläche auf allen unterstützten Plattformen reproduzierbar, stabil und performant arbeitet. Dieses Lifecycle-Modell bildet die Grundlage für sämtliche UI-Komponenten und Runtime-Prozesse des NovaOS-Bootmanagers.