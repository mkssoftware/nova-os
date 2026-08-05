# NPSPEC-BOOTRENDER-0014
# Rounded Geometry

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0014 |
| Titel | Rounded Geometry |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0013 |
| Zugehörige ADRs | ADR-BOOTGFX-0006, ADR-BOOTDESIGN-0003, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **Rounded Geometry System** der NovaOS Boot Rendering Engine.

Das System stellt die Grundlage für sämtliche geometrischen Objekte mit abgerundeten Kanten dar und wird für das gesamte NovaOS Design verwendet.

Es unterstützt unter anderem:

- Rounded Rectangles
- Rounded Panels
- Buttons
- Dialoge
- Karten
- Eingabefelder
- Glass-Oberflächen
- Masken
- Schatten
- Hover-Effekte

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche grafischen Objekte mit gekrümmten Begrenzungen.

Nicht Bestandteil sind:

- SVG-Pfade
- Bézier-Objekte
- Kreise
- Ellipsen
- Polygonrenderer

---

# 3. Architektur

```text
Render Object
      │
      ▼
Rounded Geometry
      │
      ▼
Clipping
      │
      ▼
Gradient
      │
      ▼
Shadow
      │
      ▼
Framebuffer
```

---

# 4. Grundprinzip

Rounded Geometry beschreibt geometrische Formen, deren Ecken durch definierte Radien ersetzt werden.

Die Engine arbeitet intern ausschließlich mit mathematischen Parametern.

Bitmapmasken sind hierfür nicht erforderlich.

---

# 5. Primitive

Die Engine unterstützt folgende Primitive:

- Rectangle
- Rounded Rectangle
- Capsule
- Circle
- Ellipse

Rounded Rectangle ist die Standardform der NovaOS-Oberfläche.

---

# 6. Rounded Rectangle

Eine Rounded Rectangle besteht aus:

- Position
- Größe
- Eckradius
- Randstärke
- Füllung

```c
typedef struct
{
    float x;
    float y;

    float width;
    float height;

    float radius;
} nova_round_rect_t;
```

---

# 7. Individuelle Radien

Optional besitzt jede Ecke einen eigenen Radius.

```c
typedef struct
{
    float top_left;
    float top_right;
    float bottom_right;
    float bottom_left;
} nova_corner_radius_t;
```

---

# 8. Radiusbegrenzung

Der Radius darf niemals größer sein als:

```text
min(Breite, Höhe) / 2
```

Zu große Werte werden automatisch begrenzt.

---

# 9. Standardradien

Empfohlene Werte:

| Element | Radius |
|---|---:|
| Button | 8 DLU |
| Dialog | 18 DLU |
| Panel | 12 DLU |
| Card | 14 DLU |
| Textfeld | 8 DLU |
| Menü | 10 DLU |
| Overlay | 20 DLU |

Alle Werte werden über Design Tokens bereitgestellt.

---

# 10. Koordinatensystem

Rounded Geometry verwendet ausschließlich logische Koordinaten.

Die Umrechnung in Pixel erfolgt erst während der Rasterisierung.

---

# 11. Skalierung

Alle Radien skalieren proportional zur UI-Skalierung.

```text
Radius

×

ScaleFactor

=

Neuer Radius
```

---

# 12. Transformation

Rounded Geometry unterstützt:

- Translation
- Rotation
- Skalierung
- Spiegelung

Die Rundung bleibt dabei erhalten.

---

# 13. Rasterisierung

Die Rasterisierung erfolgt analytisch.

Eigenschaften:

- Anti-Aliasing
- deterministisch
- keine Bitmapmasken
- keine Polygonapproximation notwendig

---

# 14. Anti-Aliasing

Alle Rundungen unterstützen Kantenglättung.

Das Qualitätsprofil bestimmt:

- deaktiviert
- Standard
- Hoch

---

# 15. Clipping

Rounded Geometry wird vollständig in das Clipping-System integriert.

Pixel außerhalb des Clippingbereichs werden verworfen.

---

# 16. Masken

Rounded Rectangles können direkt als Maske verwendet werden.

Dadurch entstehen:

- Dialogmasken
- Glassmasken
- Blurmasken
- Bildmasken

---

# 17. Gradienten

Gradienten orientieren sich an der Geometrie.

Die Rundung beeinflusst ausschließlich die sichtbare Fläche.

---

# 18. Schatten

Schatten werden entlang der Außenkontur berechnet.

Der Schatten folgt exakt dem Eckradius.

---

# 19. Blur

Blur wird ausschließlich innerhalb der gerundeten Form berechnet.

Außerhalb erfolgt keine Verarbeitung.

---

# 20. Rahmen

Rounded Geometry unterstützt:

- Innenrahmen
- Außenrahmen
- zentrierte Rahmen

Die Rundung bleibt dabei unverändert.

---

# 21. Glass Design

Glass Panels verwenden standardmäßig:

- Rounded Rectangle
- Gradient
- Blur
- Shadow

Rounded Geometry bildet somit die Grundlage der NovaOS Design Language.

---

# 22. Animationen

Folgende Eigenschaften dürfen animiert werden:

- Radius
- Größe
- Position
- Rahmenstärke

Dadurch entstehen weiche Übergänge zwischen verschiedenen Zuständen.

---

# 23. Hit Testing

Trefferprüfungen berücksichtigen die tatsächliche gerundete Form.

Nicht sichtbare Eckbereiche dürfen keine Eingaben empfangen.

---

# 24. Layout

Das Layout berechnet ausschließlich das äußere Begrenzungsrechteck.

Die Rundung beeinflusst die Position anderer Elemente nicht.

---

# 25. Speicher

Rounded Geometry speichert ausschließlich mathematische Parameter.

Es werden keine vorberechneten Bitmaps benötigt.

---

# 26. Performance

Anforderungen:

- keine Heap-Allokationen
- SIMD zulässig
- deterministische Laufzeit
- analytische Berechnung

---

# 27. Optimierungen

Zulässige Optimierungen:

- Radius Cache
- Scanline Rendering
- vorberechnete Kreisbögen
- SIMD
- Bounding Box Test

Die sichtbare Darstellung darf sich dadurch nicht ändern.

---

# 28. Sicherheit

Das System verhindert:

- negative Radien
- NaN-Werte
- unendliche Werte
- ungültige Größen
- Überläufe
- Out-of-Bounds-Zugriffe

---

# 29. API

```c
void
nova_round_rect_create(
    nova_round_rect_t* rect,
    float x,
    float y,
    float width,
    float height,
    float radius);

void
nova_round_rect_set_radius(
    nova_round_rect_t* rect,
    float radius);

void
nova_round_rect_render(
    const nova_round_rect_t* rect,
    nova_surface_t* surface);

bool
nova_round_rect_contains(
    const nova_round_rect_t* rect,
    float x,
    float y);
```

---

# 30. Diagnosedaten

Zu erfassen sind:

- Anzahl Rounded Rectangles
- durchschnittlicher Radius
- maximale Rundung
- Renderzeit
- Cache Hits
- Cache Misses
- Anti-Aliasing aktiv
- Schatten aktiv

---

# 31. Testfälle

Zu testen sind:

- Radius 0
- maximaler Radius
- unterschiedliche Eckradien
- Skalierung
- Rotation
- Gradient
- Blur
- Schatten
- Clipping
- Hit Testing
- Animation

---

# 32. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Rounded Rectangles korrekt dargestellt werden.
- individuelle Eckradien unterstützt werden.
- Anti-Aliasing korrekt funktioniert.
- Clipping und Masken vollständig integriert sind.
- Schatten und Blur der Geometrie folgen.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Renderer identische Ergebnisse liefern.

---

# 33. Referenzen

## ADRs

- ADR-BOOTGFX-0006 – Clipping- und Maskensystem
- ADR-BOOTDESIGN-0003 – Adaptives Glass-Design
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0010 – Clipping and Masks
- NPSPEC-BOOTRENDER-0011 – 2D Transformations
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing
- NPSPEC-BOOTRENDER-0013 – Gradients

---

# 34. Zusammenfassung

Das **Rounded Geometry System** bildet die geometrische Grundlage der NovaOS Design Language. Durch analytisch berechnete Rundungen, individuelle Eckradien, Anti-Aliasing sowie die vollständige Integration in Clipping, Masking, Gradient-, Blur- und Schatten-Rendering entstehen moderne Oberflächen mit hoher Darstellungsqualität. Das System arbeitet deterministisch, plattformunabhängig und identisch auf BIOS- und UEFI-Systemen.
