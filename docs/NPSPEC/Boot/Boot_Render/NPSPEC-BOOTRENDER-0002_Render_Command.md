# NPSPEC-BOOTRENDER-0002
# Render Command System

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRENDER-0002 |
| Titel | Render Command System |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0010 |
| ADRs | ADR-BOOTGFX-0004, ADR-BOOTUI-0006, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **Render Command System** der NovaOS Boot Rendering Engine.

Das Render Command System bildet die Schnittstelle zwischen:

- Scene Graph
- Controls
- Dialogen
- Layout Engine
- Motion Engine

und dem eigentlichen Renderer.

Keine Komponente zeichnet selbst Pixel.

Stattdessen erzeugt jede Komponente ausschließlich Render Commands.

Dadurch entsteht eine vollständig deterministische Renderpipeline.

---

# 2. Architekturziele

Das Render Command System verfolgt folgende Ziele.

- vollständige Trennung zwischen UI und Rendering
- deterministische Renderreihenfolge
- reproduzierbare Frames
- einfache Optimierung
- einfache Parallelisierung
- Debugbarkeit
- Plattformunabhängigkeit
- geringer Speicherverbrauch

---

# 3. Architektur

```
Scene Graph

↓

Controls

↓

Render Commands

↓

Render Queue

↓

Sorting

↓

Rasterizer

↓

Compositor

↓

Framebuffer
```

Die Rendering Engine arbeitet ausschließlich auf einer geordneten Liste von Renderbefehlen. Dieses Prinzip ähnelt modernen Grafikpipelines, bei denen Zeichenbefehle zunächst gesammelt und anschließend in einer definierten Reihenfolge verarbeitet werden. :contentReference[oaicite:0]{index=0}

---

# 4. Grundprinzip

Kein Control zeichnet selbst.

Beispiel

Button

↓

erzeugt

```
DrawRoundedRectangle

↓

DrawShadow

↓

DrawText
```

Der Renderer kennt keinen Button.

Er kennt ausschließlich Render Commands.

---

# 5. Eigenschaften

Jeder Render Command ist

- unveränderlich
- serialisierbar
- deterministisch
- validierbar
- cachefähig

Nach dem Erzeugen darf ein Command nicht mehr verändert werden.

---

# 6. Render Queue

Alle Commands landen zunächst in der Render Queue.

```
Render Queue

↓

Sortieren

↓

Optimieren

↓

Ausführen
```

---

# 7. Command Buffer

Jeder Frame besitzt genau einen Command Buffer.

```
Begin Frame

↓

Command Buffer

↓

Execute

↓

Clear
```

Nach jedem Frame wird der Buffer vollständig zurückgesetzt.

---

# 8. Command Struktur

Jeder Render Command besitzt

```cpp
CommandID

CommandType

Layer

ZIndex

ClipID

TransformID

Opacity

Flags
```

Optional

```cpp
ResourceHandle

Color

Shadow

Blur

Text

Geometry
```

---

# 9. Command Typen

Unterstützt werden

## Geometrie

```
DrawLine

DrawRectangle

DrawRoundedRectangle

DrawCircle

DrawEllipse

DrawPolygon

DrawPath
```

---

## Bilder

```
DrawPNG

DrawSVG

DrawImage
```

---

## Text

```
DrawGlyph

DrawText

DrawParagraph
```

---

## Effekte

```
DrawShadow

DrawBlur

DrawGlow

DrawGradient
```

---

## Layer

```
PushLayer

PopLayer

PushClip

PopClip
```

---

## Transformation

```
PushTransform

PopTransform
```

---

# 10. Command Reihenfolge

Die Reihenfolge ist fest definiert.

```
Background

↓

Images

↓

Shapes

↓

Controls

↓

Text

↓

Dialogs

↓

Overlay

↓

Cursor
```

Diese Reihenfolge darf nicht verändert werden.

---

# 11. Sortierung

Sortiert wird nach

```
Layer

↓

ZIndex

↓

Creation Order
```

Dadurch entstehen reproduzierbare Frames.

---

# 12. Render States

Commands besitzen keinen eigenen Rendercode.

Sie referenzieren Render States.

Beispiel

```
Opacity

Clip

Transform

Blend Mode

Layer
```

---

# 13. Ressourcen

Commands referenzieren ausschließlich

```
Resource Handle
```

niemals

```
Dateipfade
```

Beispiel

```
FontID

SVGID

PNGID
```

---

# 14. Transformationen

Transformationen werden nicht kopiert.

Sie werden referenziert.

```
Transform Matrix

↓

Handle

↓

Command
```

---

# 15. Clipping

Clipping erfolgt stapelbasiert.

```
Push Clip

↓

Draw Commands

↓

Pop Clip
```

Verschachtelung ist erlaubt.

---

# 16. Layer

Ein Layer besitzt

```
LayerID

Opacity

Visibility

Blur

Clip

Transform
```

---

# 17. Batchbildung

Vor dem Rendern werden Commands zusammengefasst.

Beispiel

```
Rectangle

Rectangle

Rectangle

↓

Batch
```

Dadurch werden unnötige Zustandswechsel reduziert.

---

# 18. Optimierungen

Vor dem Rendern erfolgen

- Batchbildung
- Command Sorting
- State Merging
- Clipping Optimierung
- Dirty Region Filter
- Resource Lookup

---

# 19. Command Validierung

Vor Ausführung wird geprüft

- gültiger Layer
- gültiger Clip
- gültige Ressourcen
- gültige Transformation
- gültige Farben
- gültige Handles

Ungültige Commands werden verworfen.

---

# 20. Speicherlayout

```
Frame Pool

↓

Command Buffer

↓

Render Queue

↓

Execute

↓

Reset
```

Alle Commands leben ausschließlich innerhalb eines Frames.

---

# 21. Threadmodell

Version 1

```
Single Thread
```

Spätere Versionen

```
Scene

↓

Command Building

↓

Parallel

↓

Merge

↓

Render
```

Das Ausführen der Commands erfolgt weiterhin in deterministischer Reihenfolge.

---

# 22. Fehlerbehandlung

Ungültiger Command

↓

Log

↓

Verwerfen

↓

Weiterarbeiten

Ungültige Ressourcen

↓

Placeholder

↓

Weiterarbeiten

---

# 23. Debug Commands

Optional

```
DrawBounds

DrawClip

DrawDirtyRegion

DrawFPS

DrawLayoutGrid
```

Diese Commands werden ausschließlich im Debugmodus erzeugt.

---

# 24. API

```cpp
BeginFrame()

EndFrame()

SubmitCommand()

ExecuteCommands()

ClearCommands()

SortCommands()

OptimizeCommands()

ValidateCommands()
```

---

# 25. Performance

Zielwerte

| Operation | Ziel |
|-----------|------|
| Command erzeugen | < 50 ns |
| Command sortieren | < 1 ms |
| Command validieren | < 500 µs |
| Queue zurücksetzen | < 100 µs |

Keine Heap-Allokationen während des Render-Hotpaths.

---

# 26. Sicherheitsanforderungen

Das Command System darf niemals

- ungültige Speicherbereiche referenzieren
- Ressourcen verändern
- Commands nachträglich ändern
- Renderreihenfolgen verletzen

Alle Commands werden vor der Ausführung validiert.

---

# 27. Testanforderungen

Zu testen sind

## Commands

- alle Commandtypen
- ungültige Commands
- leere Commands

## Queue

- Sortierung
- Reihenfolge
- Batchbildung

## Ressourcen

- fehlende PNG
- fehlende SVG
- ungültige Fonts

## Layer

- Alpha
- Clipping
- Transformation

## Performance

- Command-Aufbau
- Queue-Größe
- Speicherverbrauch

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt wenn

- alle UI-Komponenten ausschließlich Render Commands erzeugen
- der Renderer keine UI-Komponenten kennt
- sämtliche Commands deterministisch verarbeitet werden
- die Renderreihenfolge reproduzierbar ist
- keine Heap-Allokationen im Render-Hotpath auftreten
- Batchbildung korrekt funktioniert
- ungültige Commands sicher verworfen werden

---

# 29. Referenzen

## ADR

- ADR-BOOTUI-0006 – Retained-Mode-Szenengraph
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPEC

- NPSPEC-BOOTUI-0002 – Boot UI Runtime
- NPSPEC-BOOTUI-0004 – Boot UI State Model
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager

---

# 30. Zusammenfassung

Das **Render Command System** bildet die zentrale Kommunikationsschicht zwischen der Bootoberfläche und der Rendering Engine. Alle UI-Komponenten erzeugen ausschließlich deklarative Render Commands, die anschließend gesammelt, validiert, sortiert, optimiert und in einer deterministischen Reihenfolge ausgeführt werden. Durch diese Architektur bleiben Rendering, UI-Logik und Hardwarezugriffe vollständig voneinander getrennt. Gleichzeitig ermöglicht das System Optimierungen wie Batchbildung, Dirty-Region-Rendering und zukünftige Parallelisierung, ohne die Reproduzierbarkeit oder Stabilität der Bootoberfläche zu beeinträchtigen.