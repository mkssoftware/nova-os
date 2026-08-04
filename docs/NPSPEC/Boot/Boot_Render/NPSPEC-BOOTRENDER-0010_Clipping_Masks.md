# NPSPEC-BOOTRENDER-0010
# Clipping and Masks

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0010 |
| Titel | Clipping and Masks |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0009 |
| Zugehörige ADRs | ADR-BOOTGFX-0006, ADR-BOOTGFX-0004, ADR-BOOTUI-0006, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Clipping- und Maskensystem** der NovaOS Boot Rendering Engine.

Das System stellt sicher, dass ausschließlich sichtbare Pixel verarbeitet werden und grafische Elemente kontrolliert auf definierte Bereiche begrenzt werden.

Das Clipping- und Maskensystem dient außerdem als Grundlage für:

- Scrollbereiche
- Dialogfenster
- Glass-Effekte
- Blur
- Schatten
- Icons
- Vektorobjekte
- Overlays
- Animationen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Zeichenoperationen innerhalb der Bootoberfläche.

Dazu gehören:

- Primitive
- Text
- Bilder
- SVG
- Controls
- Layer
- Dialoge
- Blurflächen
- Effekte

Nicht Bestandteil sind:

- Alpha-Compositing
- Rendering Pipeline
- Layerverwaltung

---

# 3. Architektur

```text
UI Element
      │
      ▼
Transform
      │
      ▼
Clipping
      │
      ▼
Masking
      │
      ▼
Rendering
      │
      ▼
Framebuffer
```

---

# 4. Grundprinzip

Jeder Renderbefehl wird vor dem Zeichnen geprüft.

Reihenfolge:

```text
Transformation
      │
      ▼
Bounding Box
      │
      ▼
Clipping
      │
      ▼
Masken
      │
      ▼
Rasterisierung
```

Ein Pixel außerhalb des Clippingbereichs darf niemals geschrieben werden.

---

# 5. Clipping Region

Eine Clipping Region beschreibt den maximal sichtbaren Bereich.

```c
typedef struct
{
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} nova_clip_rect_t;
```

Alle Koordinaten sind logisch.

---

# 6. Eigenschaften

Eine Clipping Region besitzt:

- Position
- Größe
- Aktivstatus
- Elternregion
- Typ

---

# 7. Clipping Stack

Die Runtime verwaltet einen Clipping Stack.

```text
Push Root

↓

Push Dialog

↓

Push ScrollView

↓

Push Button

↓

Pop

↓

Pop
```

Der oberste Eintrag bestimmt den aktuell gültigen Clippingbereich.

---

# 8. Root Clip

Zu Beginn jedes Frames existiert genau eine Root Region.

```text
0
0
Width
Height
```

Sie entspricht der Bildschirmgröße.

---

# 9. Push Clip

```c
void
nova_clip_push(
    const nova_clip_rect_t* rect);
```

Die neue Region wird automatisch mit der aktuellen Region geschnitten.

---

# 10. Pop Clip

```c
void
nova_clip_pop(void);
```

Nach dem Entfernen wird die vorherige Region wieder aktiv.

---

# 11. Schnittmenge

Neue Regionen werden über die Schnittmenge berechnet.

```text
Current Clip

∩

New Clip

=

Effective Clip
```

Leere Schnittmengen führen zum Überspringen des Renderbefehls.

---

# 12. Clipping Test

Vor jedem Renderbefehl erfolgt:

```text
Bounding Box

gegen

Current Clip
```

Ergebnisse:

- vollständig sichtbar
- teilweise sichtbar
- vollständig außerhalb

---

# 13. Vollständig außerhalb

Liegt ein Objekt vollständig außerhalb der Region, darf keine Rasterisierung erfolgen.

Dadurch werden unnötige Berechnungen vermieden.

---

# 14. Teilweise sichtbar

Nur der sichtbare Teil wird gerastert.

Alle übrigen Pixel werden verworfen.

---

# 15. Vollständig sichtbar

Ist das Objekt vollständig innerhalb der Region, entfällt weiteres Clipping.

---

# 16. Clipping Ebenen

Jede Ebene besitzt ihren eigenen Clip.

Beispiele:

- Desktop
- Hintergrund
- Menü
- Dialog
- Tooltip
- Mauszeiger

---

# 17. Dialoge

Dialoge erzeugen automatisch eine neue Clipping Region.

Kindobjekte dürfen den Dialogbereich nicht verlassen.

---

# 18. Scrollbereiche

Scrollbare Controls erzeugen:

```text
Viewport

↓

Clip

↓

Scroll Offset

↓

Render
```

Inhalte außerhalb des Viewports werden nicht dargestellt.

---

# 19. Bilder

Rasterbilder werden vor dem Zeichnen geclippt.

Teilbereiche dürfen direkt verwendet werden.

---

# 20. SVG

Vektorobjekte werden zunächst gegen den Clip getestet.

Erst anschließend erfolgt die Rasterisierung.

---

# 21. Text

Text wird zeilenweise geclippt.

Glyphen außerhalb des Clippingbereichs werden nicht gezeichnet.

---

# 22. Primitive

Folgende Primitive unterstützen Clipping:

- Linie
- Rechteck
- Kreis
- Ellipse
- Polygon
- Bézierkurve

---

# 23. Masken

Masken definieren zusätzliche Sichtbarkeitsbereiche.

Maskentypen:

- Rechteck
- Bitmap
- Alpha
- Vektor
- Rounded Rectangle

---

# 24. Rechteckmasken

Die einfachste Maskenform.

```text
██████████

sichtbar
```

---

# 25. Alpha-Masken

Jeder Pixel besitzt einen Alpha-Wert.

```text
0 = unsichtbar

255 = sichtbar
```

Zwischenwerte sind zulässig. :contentReference[oaicite:0]{index=0}

---

# 26. Bitmapmasken

Eine Bitmap kann als Sichtbarkeitsmaske dienen.

Eigenschaften:

- Graustufen
- Alpha
- feste Größe

---

# 27. Vektormasken

Vektormasken bestehen aus Pfaden.

Vorteile:

- beliebige Skalierung
- geringe Speichergröße
- hohe Qualität

---

# 28. Rounded Rectangle

Rounded Rectangles besitzen integrierte Masken.

Die Rundung wird beim Clipping berücksichtigt.

---

# 29. Mask Stack

Masken können verschachtelt werden.

```text
Root

↓

Dialog

↓

Image

↓

Blur
```

Die effektive Maske ergibt sich aus allen aktiven Ebenen.

---

# 30. Kombination

Mehrere Masken werden in Reihenfolge angewendet.

```text
Clip

↓

Mask A

↓

Mask B

↓

Mask C
```

---

# 31. Blur

Blur darf ausschließlich innerhalb der aktiven Maske berechnet werden.

Außerhalb der Maske erfolgt keine Berechnung.

---

# 32. Schatten

Schatten werden zunächst vollständig berechnet.

Anschließend erfolgt das Clipping.

---

# 33. Transparenz

Masken beeinflussen ausschließlich die Sichtbarkeit.

Alpha-Blending erfolgt erst nach dem Masking. :contentReference[oaicite:1]{index=1}

---

# 34. Performance

Optimierungen:

- Bounding Box Test
- frühes Verwerfen
- SIMD
- Scanline Clipping
- Mask Cache

---

# 35. Speicher

Masken werden vorreserviert.

Heap-Allokationen während des Renderings sind unzulässig.

---

# 36. Mask Cache

Statische Masken dürfen zwischengespeichert werden.

Beispiele:

- Dialoge
- Icons
- Buttons

---

# 37. Sicherheit

Das System verhindert:

- Stack Overflow
- ungültige Masken
- ungültige Clips
- Out-of-Bounds-Zugriffe
- rekursive Masken

---

# 38. API

```c
void
nova_clip_push(
    const nova_clip_rect_t* rect);

void
nova_clip_pop(void);

const nova_clip_rect_t*
nova_clip_current(void);

bool
nova_clip_visible(
    const nova_rect_t* rect);

void
nova_mask_push(
    nova_mask_t* mask);

void
nova_mask_pop(void);
```

---

# 39. Diagnosedaten

Zu erfassen sind:

- Anzahl Clips
- Push Operationen
- Pop Operationen
- Masken
- Cache Treffer
- Cache Misses
- verworfene Objekte
- Clip Tests

---

# 40. Testfälle

Zu testen sind:

- Root Clip
- Dialog Clip
- Scroll Clip
- verschachtelte Clips
- Rechteckmasken
- Bitmapmasken
- Alpha-Masken
- Vektormasken
- Blur
- Schatten
- vollständiges Verwerfen
- Teilclipping

---

# 41. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- kein Pixel außerhalb des Clippingbereichs geschrieben wird.
- verschachtelte Clippingbereiche korrekt funktionieren.
- Masken beliebig kombiniert werden können.
- Scrollbereiche korrekt begrenzt sind.
- Dialoge ihre Inhalte vollständig beschneiden.
- Clipping deterministisch arbeitet.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS und UEFI identisches Verhalten zeigen.

---

# 42. Referenzen

## ADRs

- ADR-BOOTGFX-0006 – Clipping- und Maskensystem
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTUI-0006 – Resolution and Scaling
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0004 – Layer and Surface Management
- NPSPEC-BOOTRENDER-0008 – Buffering and Frame Presentation
- NPSPEC-BOOTRENDER-0009 – Dirty Region Rendering
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing

---

# 43. Zusammenfassung

Das **Clipping and Masks System** begrenzt sämtliche Renderoperationen auf die tatsächlich sichtbaren Bereiche. Durch Clipping-Stacks, verschachtelte Masken und frühzeitiges Verwerfen unsichtbarer Objekte reduziert das System die Renderlast erheblich und bildet gleichzeitig die Grundlage für Dialoge, Scrollbereiche, Blur-Effekte und moderne Glass-Oberflächen. Es arbeitet vollständig plattformunabhängig und liefert identisches Verhalten auf BIOS- und UEFI-Systemen.