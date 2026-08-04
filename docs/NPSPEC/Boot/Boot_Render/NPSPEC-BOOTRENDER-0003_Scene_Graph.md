# NPSPEC-BOOTRENDER-0003
# Scene Graph

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRENDER-0003 |
| Titel | Scene Graph |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0010, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTRENDER-0002 |
| ADRs | ADR-BOOTUI-0006, ADR-BOOTUI-0007, ADR-BOOTCONTROL-0001 bis ADR-BOOTCONTROL-0007 |

---

# 1. Ziel

Diese Spezifikation definiert den vollständigen **Scene Graph** der NovaOS Boot Rendering Engine.

Der Scene Graph bildet die zentrale Datenstruktur der gesamten Bootoberfläche.

Er beschreibt:

- sämtliche UI-Objekte
- ihre Hierarchie
- ihre Zustände
- ihre Transformationen
- ihre Sichtbarkeit
- ihre Beziehungen
- ihre Ressourcen
- ihre Animationen

Der Scene Graph enthält **keine Rendering-Logik**.

Er beschreibt ausschließlich den aktuellen Zustand der Benutzeroberfläche.

---

# 2. Architekturziele

Der Scene Graph verfolgt folgende Ziele.

- vollständige Trennung zwischen UI und Rendering
- deterministische Darstellung
- hierarchische Objektverwaltung
- automatische Vererbung
- einfache Animation
- effiziente Dirty-Erkennung
- geringe Speicherkosten
- schnelle Traversierung
- einfache Erweiterbarkeit

---

# 3. Architektur

```
               Scene Graph
                   │
               Root Node
         ┌─────────┴─────────┐
     Desktop Root      Overlay Root
         │                  │
     Dialog Root       Cursor Root
         │                  │
      Controls        Notifications
         │
     Leaf Nodes
```

Der Scene Graph folgt einem **Retained-Mode-Modell**. Änderungen werden zunächst am Szenenmodell vorgenommen; das Rendering verarbeitet anschließend dieses Modell. Dadurch können Optimierungen wie Dirty Regions und Zustandsvererbung zentral durchgeführt werden. :contentReference[oaicite:0]{index=0}

---

# 4. Grundprinzip

Der Scene Graph speichert sämtliche Objekte dauerhaft.

```
Button

↓

Node

↓

Scene Graph

↓

Render Commands

↓

Renderer
```

Ein Control besitzt keine Zeichenlogik.

Es beschreibt ausschließlich seinen Zustand.

---

# 5. Root Node

Jeder Scene Graph besitzt genau einen Root.

```
Root

↓

Children

↓

Subtree
```

Der Root besitzt

- keine Eltern
- keine Transformation
- keine Darstellung

Er dient ausschließlich als Einstiegspunkt.

---

# 6. Node-Arten

Unterstützt werden

## Container

- Scene
- Panel
- Stack
- Grid

---

## Controls

- Button
- Label
- TextField
- List
- Tree
- Checkbox
- Switch
- Progress

---

## Dialoge

- Modal
- Overlay
- Popup

---

## Grafik

- SVG
- PNG
- Image
- Gradient
- Blur Layer

---

## Text

- Paragraph
- Text
- Glyph Run

---

## Spezial

- Cursor
- Tooltip
- Focus Indicator
- Debug Overlay

---

# 7. Node-Hierarchie

```
Root

↓

Panel

↓

Dialog

↓

Button

↓

Icon

↓

Text
```

Jeder Node besitzt genau einen Elternknoten.

Mehrfache Eltern sind verboten.

---

# 8. Node-Eigenschaften

Jeder Node besitzt mindestens

```cpp
NodeID

Parent

Children

Type

State

Bounds

Transform

Opacity

Visibility

Layer

Flags
```

---

# 9. Sichtbarkeit

Jeder Node besitzt

```
Visible

Hidden

Collapsed
```

### Hidden

Node wird nicht dargestellt.

Layout bleibt erhalten.

---

### Collapsed

Node existiert nicht mehr im Layout.

---

# 10. Aktivierung

Jeder Node besitzt

```
Enabled

Disabled
```

Disabled Nodes

- erhalten keinen Fokus
- reagieren nicht auf Eingaben
- werden weiterhin gerendert

---

# 11. Zustände

Alle Controls besitzen

```
Normal

Hover

Pressed

Focused

Disabled

Selected
```

Die Zustände stammen vollständig aus dem Boot UI State Model.

---

# 12. Kindobjekte

Kinder erben automatisch

- Sichtbarkeit
- Opacity
- Transformation
- Clipping

Nicht vererbt werden

- Fokus
- Animation
- Eingabezustand

---

# 13. Transformationen

Jeder Node besitzt

```
Translation

Rotation

Scale

Pivot
```

Intern

```
3x3 Matrix
```

Die Transformation eines Elternknotens wird rekursiv auf alle Kindknoten angewendet.

---

# 14. Koordinatensystem

Alle Koordinaten sind relativ zum Elternknoten.

```
Parent

↓

Child

↓

Grand Child
```

Absolute Bildschirmkoordinaten werden erst während der Traversierung berechnet.

---

# 15. Layer

Jeder Node gehört genau einem Layer.

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

---

# 16. Dirty Flags

Jeder Node besitzt

```
Layout Dirty

Render Dirty

State Dirty

Animation Dirty
```

Nur Dirty Nodes erzeugen neue Render Commands.

---

# 17. Ressourcen

Nodes referenzieren ausschließlich Handles.

Beispiel

```cpp
FontID

SVGID

PNGID

ThemeID
```

Dateipfade dürfen niemals gespeichert werden.

---

# 18. Animation

Animationen verändern ausschließlich Eigenschaften.

Beispiel

```
Opacity

Scale

Translation

Rotation

Blur
```

Die Node-Struktur bleibt unverändert.

---

# 19. Layout

Layoutinformationen werden im Node gespeichert.

```cpp
Width

Height

MinWidth

MinHeight

Margin

Padding

Alignment
```

---

# 20. Traversierung

Der Scene Graph wird rekursiv durchlaufen.

```
Root

↓

Parent

↓

Child

↓

Child

↓

Sibling
```

Während der Traversierung werden

- Transformationen
- Sichtbarkeit
- Clipping
- Layer
- Opacity

akkumuliert.

---

# 21. Rendererzeugung

Traversal

↓

Node

↓

Render Commands

↓

Render Queue

↓

Renderer

Der Renderer verarbeitet niemals direkt den Scene Graph.

---

# 22. Ereignisse

Jeder Node kann Ereignisse empfangen.

Beispiele

```
MouseEnter

MouseLeave

Click

Focus

Blur

KeyDown

KeyUp
```

Die Ereignisverarbeitung erfolgt unabhängig vom Rendering.

---

# 23. Speicherverwaltung

Alle Nodes werden im Scene Pool gespeichert.

Lebenszyklus

```
Create

↓

Attach

↓

Visible

↓

Detach

↓

Destroy
```

Die Speicherverwaltung erfolgt vollständig über das Boot UI Memory Model.

---

# 24. API

```cpp
CreateNode()

DestroyNode()

AttachNode()

DetachNode()

FindNode()

Traverse()

Invalidate()

MarkDirty()

SetVisible()

SetTransform()

SetOpacity()
```

---

# 25. Fehlerbehandlung

Ungültiger Parent

↓

Node verwerfen

Ungültige Transformation

↓

Identity Matrix

Ungültige Ressourcen

↓

Placeholder

↓

Weiterarbeiten

Der Scene Graph darf niemals inkonsistent werden.

---

# 26. Performance

Zielwerte

| Operation | Ziel |
|-----------|------|
| Node erzeugen | < 200 ns |
| Node anhängen | < 100 ns |
| Traversierung | < 1 ms |
| Dirty Detection | < 500 µs |
| Render Command-Erzeugung | < 2 ms |

Die Traversierung muss linear zur Anzahl der sichtbaren Nodes erfolgen.

---

# 27. Debugmodus

Optional darstellbar

- Node IDs
- Elternbeziehungen
- Dirty Nodes
- Layer
- Transformationen
- Clipping
- Fokus
- Layoutgrenzen

---

# 28. Sicherheitsanforderungen

Der Scene Graph darf niemals

- Zyklen enthalten
- mehrere Eltern besitzen
- ungültige Handles referenzieren
- zerstörte Nodes referenzieren

Alle Änderungen werden vor dem Einfügen validiert.

---

# 29. Testanforderungen

Zu testen sind

## Hierarchie

- Root Node
- Eltern-Kind-Beziehungen
- Verschachtelungen

## Transformationen

- Translation
- Rotation
- Skalierung
- Vererbung

## Zustände

- Sichtbarkeit
- Fokus
- Aktivierung

## Dirty System

- Layout Dirty
- Render Dirty
- State Dirty

## Performance

- Traversierung
- Speicherverbrauch
- große Szenen

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt wenn

- alle UI-Objekte vollständig im Scene Graph verwaltet werden
- jeder Node genau einen Elternknoten besitzt
- die Traversierung deterministisch erfolgt
- Dirty Regions korrekt erkannt werden
- Render Commands ausschließlich aus dem Scene Graph erzeugt werden
- keine Zyklen entstehen
- große Szenen performant verarbeitet werden

---

# 31. Referenzen

## ADR

- ADR-BOOTUI-0006 – Retained-Mode-Szenengraph
- ADR-BOOTUI-0007 – Dirty-Region-Rendering
- ADR-BOOTCONTROL-0001 – Gemeinsames Basismodell für Bedienelemente
- ADR-BOOTCONTROL-0002 – Zustandsautomat für Controls
- ADR-BOOTCONTROL-0005 – Wiederverwendbare Komponenten statt fest gezeichneter Oberflächen

## NPSPEC

- NPSPEC-BOOTUI-0002 – Boot UI Runtime
- NPSPEC-BOOTUI-0004 – Boot UI State Model
- NPSPEC-BOOTUI-0007 – Boot UI Memory Model
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0002 – Render Command System
- NPSPEC-BOOTLAYOUT-0001 – Layout Engine
- NPSPEC-BOOTCONTROL-0001 – Control Framework

---

# 32. Zusammenfassung

Der **Scene Graph** bildet das zentrale Objektmodell der NovaOS-Bootoberfläche. Sämtliche visuellen Elemente – von Panels über Dialoge bis hin zu Buttons, Icons und Texten – werden als hierarchische Nodes verwaltet. Die Rendering Engine greift niemals direkt auf Controls zu, sondern erzeugt Render Commands ausschließlich durch die Traversierung des Scene Graphs. Durch die Kombination aus Retained-Mode-Architektur, Dirty-Region-Erkennung, Zustandsvererbung und deterministischer Traversierung entsteht eine hochperformante, leicht erweiterbare und vollständig reproduzierbare Grundlage für die gesamte grafische Bootoberfläche von NovaOS. :contentReference[oaicite:1]{index=1}