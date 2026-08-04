# NPSPEC-BOOTUI-0002
# Boot UI Runtime

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0002 |
| Titel | Boot UI Runtime |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Abhängigkeiten | NPSPEC-BOOTUI-0001, ADR-BOOTUI-0001 bis ADR-BOOTUI-0012 |
| Nachfolger | NPSPEC-BOOTUI-0003, NPSPEC-BOOTRENDER-0001 |

---

# 1. Ziel

Diese Spezifikation beschreibt die vollständige Runtime (Laufzeitumgebung) der grafischen Oberfläche des NovaOS Bootmanagers.

Die Runtime ist das Herzstück der Boot-UI und verwaltet den gesamten Lebenszyklus aller grafischen Komponenten.

Sie übernimmt unter anderem:

- Initialisierung
- Ressourcenverwaltung
- Eingabeverarbeitung
- Szenenverwaltung
- Layout
- Rendering
- Animationen
- Fehlerbehandlung
- Speicherverwaltung
- Performanceüberwachung

Die Runtime bildet damit den vollständigen "Mini-Desktop" des Bootmanagers.

---

# 2. Aufgaben der Runtime

Die Runtime ist für sämtliche Laufzeitprozesse verantwortlich.

## Kernaufgaben

- Initialisierung aller Subsysteme
- Verwaltung der UI
- Aktualisierung aller Zustände
- Steuerung sämtlicher Animationen
- Verarbeitung aller Eingaben
- Rendering aller Frames
- Laden sämtlicher Ressourcen
- Speicherüberwachung
- Fehlererkennung
- Diagnose

---

# 3. Architektur

```
                   Boot Manager
                         │
                   Boot UI Runtime
                         │
 ┌───────────────────────┼────────────────────┐
 │                       │                    │
Initialization     Resource System     Diagnostics
 │                       │                    │
 ├──────────────┐        │                    │
 │              │        │                    │
Input      Scene Graph   │                    │
 │              │        │                    │
 │         Layout Engine |                    │
 │              │        |                    │
 │         Motion Engine |                    │
 │              │        |                    │
 └────────► Rendering Engine ◄────────────────┘
                      │
                Framebuffer
```

---

# 4. Runtime-Komponenten

## 4.1 Runtime Core

Verantwortlich für:

- Initialisierung
- Shutdown
- Hauptschleife
- Statusverwaltung
- Scheduler
- Frameverwaltung

---

## 4.2 Resource Manager

Verwaltet

- Fonts
- Icons
- PNG
- SVG
- Themes
- Animationen
- Konfiguration

Eigenschaften

- Referenzzählung
- Cache
- Integritätsprüfung
- Versionsprüfung

---

## 4.3 Scene Manager

Verwaltet

- sämtliche Fenster
- Dialoge
- Overlays
- Controls
- Popups
- Animationen

---

## 4.4 Input Manager

Verarbeitet

- Tastatur
- Maus
- Touch
- Controller
- Firmware Events

---

## 4.5 Motion Manager

Verantwortlich für

- Fade
- Blur
- Scale
- Rotation
- Opacity
- Position
- Spring Animationen

---

## 4.6 Layout Manager

Berechnet

- Größen
- DPI
- Skalierung
- Positionen
- Auto Layout
- Responsive Layout

---

## 4.7 Render Manager

Erstellt

- Render Queue
- Dirty Regions
- Compositor
- Framebuffer
- Present

---

## 4.8 Diagnostics

Sammelt

- FPS
- Renderzeit
- Speicher
- Fehler
- Ressourcen
- Performance

---

# 5. Initialisierung

Boot UI Runtime startet in folgender Reihenfolge.

```
Framebuffer

↓

Memory Pools

↓

Diagnostics

↓

Configuration

↓

Theme

↓

Fonts

↓

Icons

↓

Resources

↓

Scene Graph

↓

Layout Engine

↓

Motion Engine

↓

Input Manager

↓

Rendering

↓

Boot Screen
```

Ein Schritt darf erst beginnen wenn der vorherige erfolgreich abgeschlossen wurde.

---

# 6. Hauptschleife

Die Runtime arbeitet vollständig eventbasiert.

```
while(Running)

↓

Timer

↓

Input

↓

Update Scene

↓

Animation

↓

Layout

↓

Dirty Detection

↓

Render Queue

↓

Rendering

↓

Present

↓

Diagnostics
```

Die Schleife läuft solange bis

- Kernel gestartet wird
- Neustart
- Ausschalten
- Fehlerzustand

---

# 7. Runtime Status

Die Runtime besitzt folgende Zustände

```
Created

↓

Initializing

↓

Loading

↓

Running

↓

Suspended

↓

Recovery

↓

Shutdown
```

Ungültige Zustandswechsel sind nicht erlaubt.

---

# 8. Scheduler

Der Runtime Scheduler verwaltet

- Animationen
- Timer
- Wiederholungen
- Idle Tasks
- Diagnose

Prioritäten

```
Critical

↓

Input

↓

Rendering

↓

Animations

↓

Layout

↓

Background

↓

Idle
```

---

# 9. Ereignismodell

Alle Ereignisse werden zentral verarbeitet.

Beispiele

```
MouseMove

↓

Button

↓

Scene

↓

Control

↓

Application
```

oder

```
Timer

↓

Animation

↓

Scene

↓

Renderer
```

Alle Events besitzen

- ID
- Quelle
- Ziel
- Zeitstempel
- Priorität
- Status

---

# 10. Speicherverwaltung

Die Runtime besitzt keine unkontrollierten Heap-Allokationen.

Verwendet werden

## Permanent Pool

- Fonts
- Themes
- SVG
- Icons

---

## Scene Pool

- Controls
- Dialoge
- Layouts

---

## Animation Pool

- Keyframes
- Curves
- States

---

## Frame Pool

Temporäre Daten

Nach jedem Frame freigegeben.

---

## Cache

- Glyphen
- Blur
- Images
- SVG
- Layout

---

# 11. Ressourcenverwaltung

Alle Ressourcen besitzen

```
ID

Version

Hash

Größe

Typ

Status

Referenzen
```

Zustände

```
NotLoaded

↓

Loading

↓

Loaded

↓

Cached

↓

Released
```

---

# 12. Fehlerbehandlung

Jede Runtime-Komponente besitzt

- Fehlercode
- Recovery Funktion
- Logging
- Neustartmöglichkeit

Beispiel

```
Font beschädigt

↓

Fallback Font

↓

Weiterarbeiten
```

oder

```
PNG beschädigt

↓

Placeholder

↓

Weiterarbeiten
```

Die Runtime darf niemals abstürzen.

---

# 13. Watchdog

Jede Hauptkomponente besitzt einen Watchdog.

Überwacht werden

- Renderzeit
- Speicher
- Deadlocks
- Event Queue
- Animationen

Bei Fehler

```
Komponente stoppen

↓

Fallback aktivieren

↓

Log erzeugen

↓

Boot fortsetzen
```

---

# 14. Threadmodell

Die Baseline arbeitet Single Threaded.

```
Runtime Thread

↓

Input

↓

Update

↓

Animation

↓

Layout

↓

Rendering

↓

Present
```

Spätere Versionen dürfen

- Asset Loader
- Bilddecoder
- SVG Parser

parallel ausführen.

Rendering bleibt deterministisch.

---

# 15. Performancebudgets

## Runtime Initialisierung

≤ 200 ms

---

## Input

≤ 1 ms

---

## Layout

≤ 2 ms

---

## Animation

≤ 2 ms

---

## Rendering

≤ 8 ms

---

## Present

≤ 2 ms

---

## Gesamter Frame

≤ 16 ms

Ziel

60 FPS

---

# 16. Logging

Runtime protokolliert

- Start
- Shutdown
- Ressourcen
- Warnungen
- Fehler
- Performance
- Speicher
- FPS

Schweregrade

```
Trace

Debug

Info

Warning

Error

Fatal
```

---

# 17. Sicherheitsanforderungen

Die Runtime darf niemals

- Speicher außerhalb reservierter Bereiche schreiben
- unvalidierte Ressourcen laden
- unendliche Schleifen erzeugen
- ungültige Pointer verwenden
- Bootvorgang blockieren

Alle Ressourcen werden vor Nutzung geprüft.

---

# 18. Erweiterbarkeit

Neue Runtime Module dürfen hinzugefügt werden.

Beispiele

- Netzwerk
- Telemetrie
- Theme Loader
- Plugins
- Diagnosetools

Die Runtime verwendet dafür ein registrierungsbasiertes Modulmodell.

---

# 19. Testanforderungen

Zu testen sind

## Initialisierung

- BIOS
- UEFI
- Recovery

---

## Speicher

- Pool Overflow
- Fragmentierung
- Cache

---

## Rendering

- Dirty Regions
- Vollbild
- Blur
- Transparenz

---

## Eingabe

- Tastatur
- Maus
- Touch

---

## Fehler

- fehlende Fonts
- beschädigte PNG
- defekte SVG
- ungültige Themes
- Ressourcenfehler

---

## Performance

- FPS
- Speicher
- CPU
- Startzeit

---

# 20. Referenzen

## ADR

- ADR-BOOTUI-0001 – Eigene Boot-UI-Engine
- ADR-BOOTUI-0005 – Double- und Triple-Buffering
- ADR-BOOTUI-0006 – Retained-Mode-Szenengraph
- ADR-BOOTUI-0007 – Dirty-Region-Rendering
- ADR-BOOTPERF-0001 – Festes Speicherbudget
- ADR-BOOTSAFE-0003 – Watchdog für Rendering und Eingabe

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTINPUT-0001 – Boot Input Architecture
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager

---

# 21. Zusammenfassung

Die **Boot UI Runtime** ist die zentrale Laufzeitumgebung des NovaOS-Bootmanagers. Sie koordiniert sämtliche Subsysteme – von Initialisierung, Ressourcenverwaltung und Eingabe über Animationen und Layout bis hin zu Rendering und Diagnose. Durch deterministische Abläufe, feste Speicherbudgets, definierte Zustandsmodelle und umfassende Fehlerbehandlung stellt sie sicher, dass die Bootoberfläche auf BIOS- und UEFI-Systemen stabil, performant und visuell konsistent arbeitet und jederzeit ohne Beeinträchtigung des eigentlichen Bootvorgangs auf sichere Fallbacks wechseln kann.