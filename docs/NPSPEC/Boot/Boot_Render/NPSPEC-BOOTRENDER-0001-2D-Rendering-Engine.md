# NPSPEC-BOOTRENDER-0001
# 2D Rendering Engine

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRENDER-0001 |
| Titel | 2D Rendering Engine |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0010 |
| ADRs | ADR-BOOTGFX-0001 bis ADR-BOOTGFX-0012, ADR-BOOTUI-0005 bis ADR-BOOTUI-0012 |

---

# 1. Ziel

Diese Spezifikation definiert die vollständige Architektur der 2D Rendering Engine des NovaOS Bootmanagers.

Die Rendering Engine ist für die Erzeugung sämtlicher sichtbarer Bildinhalte verantwortlich.

Hierzu gehören unter anderem:

- Hintergründe
- Fenster
- Dialoge
- Glass-Effekte
- Schriften
- SVG-Icons
- PNG-Grafiken
- Schatten
- Blur
- Animationen
- Compositing
- Clipping
- Layer

Die Engine arbeitet vollständig unabhängig von BIOS oder UEFI und verwendet ausschließlich die Graphics Abstraction Layer.

---

# 2. Architekturziele

Die Rendering Engine muss:

- deterministisch arbeiten
- hardwareunabhängig sein
- reproduzierbare Ergebnisse liefern
- vollständig 32-Bit RGBA verwenden
- Dirty-Region-Rendering unterstützen
- Double- und Triple-Buffering unterstützen
- Glass-Design ermöglichen
- animierte Oberflächen unterstützen
- modular aufgebaut sein
- leicht testbar sein

---

# 3. Architekturübersicht

```
                 Scene Graph
                      │
               Render Pipeline
                      │
             Render Command Queue
                      │
              Rasterizer Engine
                      │
              Compositor Engine
                      │
                 Back Buffer
                      │
                  Present()
                      │
               Graphics Backend
                      │
           BIOS (VBE) / UEFI (GOP)
```

Die Engine verwendet einen **Retained-Mode-Ansatz**: Die Szene wird im Speicher gehalten und Änderungen aktualisieren das Szenenmodell, anstatt jede Bildausgabe vollständig neu aufzubauen. Dies erleichtert Optimierungen wie Dirty-Region-Rendering und zentrale Zustandsverwaltung. :contentReference[oaicite:0]{index=0}

---

# 4. Hauptkomponenten

## Scene Graph

Verwaltet

- Controls
- Dialoge
- Layer
- Fenster
- Overlays
- Animationen

---

## Render Queue

Enthält sämtliche Renderbefehle.

Beispiel

```
DrawRectangle
↓
DrawShadow
↓
DrawBlur
↓
DrawText
↓
DrawSVG
```

---

## Rasterizer

Erzeugt

- Linien
- Rechtecke
- Kreise
- Kurven
- Polygone
- Icons
- Schrift

---

## Compositor

Mischt

- Layer
- Transparenz
- Blur
- Schatten
- Glass
- Alpha

---

## Presenter

Kopiert

```
Back Buffer

↓

Front Buffer
```

---

# 5. Rendering Pipeline

Jeder Frame besitzt dieselbe Pipeline.

```
Scene Update

↓

Layout

↓

Animation

↓

Dirty Detection

↓

Build Render Queue

↓

Rasterizer

↓

Compositor

↓

Present
```

Kein Schritt darf übersprungen werden.

---

# 6. Render Commands

Die Engine verwendet ausschließlich Renderbefehle.

Beispiele

```cpp
DrawRectangle()

DrawRoundedRectangle()

DrawEllipse()

DrawLine()

DrawPolygon()

DrawText()

DrawImage()

DrawSVG()

DrawShadow()

DrawBlur()

DrawGradient()

Clip()

PushLayer()

PopLayer()
```

---

# 7. Primitive

Unterstützt werden

## Geometrie

- Punkt
- Linie
- Rechteck
- Kreis
- Ellipse
- Polygon
- Pfad

---

## Füllungen

- Solid Color
- Linear Gradient
- Radial Gradient
- Pattern

---

## Konturen

- Solid
- Dashed
- Dotted

---

# 8. Layermodell

Jeder Frame besteht aus Layern.

```
Background

↓

Content

↓

Controls

↓

Dialogs

↓

Overlay

↓

Cursor
```

Layer besitzen

- Alpha
- Visibility
- Dirty Flag
- Clipping

---

# 9. Koordinatensystem

Verwendet werden ausschließlich

```
Logical Units
```

Die Umrechnung erfolgt erst unmittelbar vor dem Rasterizer.

---

# 10. Render States

Jeder Renderauftrag besitzt

```cpp
Transform

Opacity

Clip

Blend Mode

Shadow

Blur

Visibility

Layer
```

---

# 11. Clipping

Unterstützt

- Rechteck
- Pfad
- Maske

Verschachtelung ist erlaubt.

---

# 12. Alpha Blending

Verwendet wird

```
Premultiplied Alpha
```

Standardformel

```
Source

+

Destination
```

Die Alpha-Komposition erfolgt ausschließlich im Compositor.

---

# 13. Transformationen

Unterstützt

- Translation
- Rotation
- Skalierung
- Matrix
- Pivot

Intern

```
3×3 Matrix
```

---

# 14. Text Rendering

Unterstützt

- UTF-8
- Anti Aliasing
- Glyph Cache
- Font Fallback
- Kerning
- Ligaturen

Schrift wird vollständig über die Text Engine erzeugt.

---

# 15. Bildformate

Unterstützt

- PNG
- SVG

Intern

```
RGBA8888
```

---

# 16. Glass Rendering

Glass besteht aus

- Blur
- Transparenz
- Noise
- Lichtkante
- Schatten

Reihenfolge

```
Capture

↓

Blur

↓

Tint

↓

Noise

↓

Border

↓

Shadow
```

---

# 17. Schatten

Unterstützt

- Outer Shadow
- Inner Shadow
- Glow

Parameter

```cpp
Radius

Opacity

Offset

Color
```

---

# 18. Blur

Blur wird ausschließlich

auf Offscreen-Surfaces

berechnet.

Blur besitzt

Qualitätsstufen

```
Low

Medium

High
```

---

# 19. Dirty Region Rendering

Nur geänderte Bereiche werden neu gerendert.

```
Scene

↓

Dirty Detection

↓

Merge

↓

Render
```

Dies reduziert den Rechenaufwand erheblich und ist ein typischer Vorteil eines Retained-Mode-Renderers. :contentReference[oaicite:1]{index=1}

---

# 20. Double Buffer

Standard

```
Back Buffer

↓

Present

↓

Front Buffer
```

Optional

Triple Buffer.

---

# 21. Speicherlayout

```
Framebuffer

↓

Backbuffer

↓

Layer Buffer

↓

Offscreen Buffer
```

Alle Speicherbereiche werden beim Start reserviert.

---

# 22. Fehlerbehandlung

Fehler

↓

Placeholder

↓

Fallback

↓

Log

↓

Weiterarbeiten

Renderer darf niemals abstürzen.

---

# 23. Performance

Zielwerte

| Operation | Ziel |
|-----------|------|
| Rechteck | < 50 ns |
| Text | < 200 ns pro Glyph |
| PNG | < 2 ms |
| SVG | < 5 ms |
| Blur | < 4 ms |
| Present | < 2 ms |

Gesamter Frame

```
≤ 16 ms
```

---

# 24. API

```cpp
BeginFrame()

EndFrame()

Present()

DrawRectangle()

DrawRoundedRectangle()

DrawText()

DrawImage()

DrawSVG()

PushLayer()

PopLayer()

SetClip()

SetOpacity()

SetTransform()

DrawShadow()

DrawBlur()
```

---

# 25. Threadmodell

Version 1

Single Thread

```
Scene

↓

Render

↓

Present
```

Spätere Versionen dürfen

- SVG Parsing
- PNG Decoding
- Blur

parallelisieren.

---

# 26. Debug Rendering

Optional

- Dirty Regions
- FPS
- Layer
- Renderzeiten
- Draw Calls
- Speicher

---

# 27. Sicherheitsanforderungen

Renderer darf niemals

- außerhalb des Framebuffers schreiben
- ungültige Pointer verwenden
- nicht validierte Ressourcen rendern
- Speicher überschreiben

Alle Render Commands werden validiert.

---

# 28. Testanforderungen

Zu testen

## Primitive

- Rechtecke
- Linien
- Kreise
- Polygone

## Text

- UTF-8
- Font Fallback
- Kerning

## Bilder

- PNG
- SVG

## Effekte

- Blur
- Schatten
- Glass
- Transparenz

## Layer

- Alpha
- Clipping
- Dirty Regions

## Performance

- FPS
- Speicher
- Draw Calls

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt wenn

- sämtliche Primitive korrekt gerendert werden
- alle Layer korrekt komponiert werden
- Glass Design identisch dargestellt wird
- Dirty Regions korrekt funktionieren
- Double Buffering fehlerfrei arbeitet
- keine Renderfehler auftreten
- BIOS und UEFI identische Ergebnisse liefern
- sämtliche Performanceziele erreicht werden

---

# 30. Referenzen

## ADR

- ADR-BOOTGFX-0001 – Lineares 32-Bit-RGBA-Farbformat
- ADR-BOOTGFX-0002 – Alpha-Compositing
- ADR-BOOTGFX-0003 – Premultiplied Alpha
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTGFX-0005 – Offscreen-Surfaces
- ADR-BOOTGFX-0006 – Clipping- und Maskensystem
- ADR-BOOTGFX-0007 – Transformationen über 2D-Matrizen
- ADR-BOOTGFX-0008 – Gradient-Rendering
- ADR-BOOTGFX-0009 – Schatten- und Glow-Rendering
- ADR-BOOTGFX-0010 – Hintergrundunschärfe für Glass-Dialoge

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTUI-0005 – BIOS/UEFI Graphics Abstraction
- NPSPEC-BOOTRESOURCE-0001 – Resource Manager
- NPSPEC-BOOTTEXT-0001 – Text Rendering Engine
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor

---

# 31. Zusammenfassung

Die **2D Rendering Engine** bildet den grafischen Kern des NovaOS-Bootmanagers. Sie verwendet eine moderne, retained-mode-basierte Renderarchitektur mit Scene Graph, Render Queue, Rasterizer und Compositor. Durch die konsequente Trennung zwischen Szenenverwaltung, Rendering und Hardwareabstraktion können BIOS- und UEFI-Systeme identische Ergebnisse erzeugen. Unterstützt werden hochwertige grafische Effekte wie Glass-Design, Transparenz, Schatten, Blur, SVG-Grafiken und Animationen, während Dirty-Region-Rendering, Double Buffering und feste Speicherbudgets eine hohe Performance und deterministisches Verhalten sicherstellen. :contentReference[oaicite:2]{index=2}