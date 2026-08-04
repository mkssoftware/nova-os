# NPSPEC-BOOTRENDER-0015
# Shadows and Glows

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0015 |
| Titel | Shadows and Glows |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0014 |
| Zugehörige ADRs | ADR-BOOTGFX-0009, ADR-BOOTGFX-0011, ADR-BOOTDESIGN-0003, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **Shadows and Glows System** der NovaOS Boot Rendering Engine.

Es ermöglicht moderne Tiefen- und Hervorhebungseffekte zur Verbesserung der visuellen Hierarchie und Benutzerführung.

Unterstützt werden:

- Drop Shadows
- Inner Shadows
- Ambient Shadows
- Outer Glows
- Inner Glows
- Focus Glows
- Hover Glows
- Selection Highlights

Alle Effekte werden softwarebasiert berechnet und sind unabhängig von BIOS- oder UEFI-Grafiktreibern.

---

# 2. Geltungsbereich

Das System wird verwendet von:

- Fenstern
- Dialogen
- Buttons
- Panels
- Karten
- Menüs
- Icons
- Mauszeiger
- Fokusindikatoren

Nicht Bestandteil sind:

- Blur-Effekte
- Alpha-Compositing
- Gradient-Rendering

---

# 3. Architektur

```text
Render Object
      │
      ▼
Geometry
      │
      ▼
Shadow / Glow Generator
      │
      ▼
Blur Kernel
      │
      ▼
Alpha Compositing
      │
      ▼
Framebuffer
```

---

# 4. Grundprinzip

Schatten und Glows werden aus der Kontur eines Objekts erzeugt.

Die Berechnung erfolgt in vier Schritten:

```text
Kontur

↓

Maske

↓

Blur

↓

Farbüberlagerung
```

---

# 5. Shadow-Typen

Folgende Schattentypen werden unterstützt:

- Drop Shadow
- Inner Shadow
- Ambient Shadow
- Contact Shadow

Der Standardtyp für Dialoge ist **Drop Shadow**.

---

# 6. Glow-Typen

Unterstützt werden:

- Outer Glow
- Inner Glow
- Focus Glow
- Accent Glow

Glows dienen ausschließlich dekorativen oder interaktiven Zwecken.

---

# 7. Effektbeschreibung

```c
typedef struct
{
    nova_color_t color;

    float radius;
    float opacity;

    float offset_x;
    float offset_y;

    bool inner;
} nova_shadow_t;
```

Die gleiche Struktur wird für Glows verwendet.

---

# 8. Drop Shadow

Drop Shadows werden außerhalb der Objektkontur erzeugt.

Eigenschaften:

- weicher Verlauf
- frei definierbarer Offset
- einstellbare Deckkraft
- variabler Radius

---

# 9. Inner Shadow

Inner Shadows werden innerhalb der Objektkontur berechnet.

Sie erzeugen einen vertieften Eindruck und dürfen die Objektgrenzen nicht überschreiten.

---

# 10. Ambient Shadow

Ambient Shadows simulieren diffuses Umgebungslicht.

Eigenschaften:

- kein Offset
- großer Radius
- geringe Deckkraft

---

# 11. Outer Glow

Outer Glows erweitern die sichtbare Objektfläche.

Typische Einsatzgebiete:

- Fokus
- Hover
- Warnungen
- Akzentfarben

---

# 12. Inner Glow

Inner Glows liegen vollständig innerhalb der Objektkontur.

Sie werden häufig bei Glass-Oberflächen eingesetzt.

---

# 13. Fokusindikatoren

Der Tastaturfokus wird standardmäßig durch einen Focus Glow dargestellt.

Eigenschaften:

- hohe Sichtbarkeit
- geringer Radius
- Akzentfarbe
- animierbar

---

# 14. Hover-Effekte

Hover-Zustände dürfen Glows oder leichte Schattenverstärkungen verwenden.

Der Übergang erfolgt über das Animationssystem.

---

# 15. Radius

Der Blur-Radius bestimmt die Weichheit des Effekts.

```text
Radius = 0

↓

harte Kante

Radius > 0

↓

weicher Verlauf
```

---

# 16. Offset

Der Schattenversatz wird unabhängig in X- und Y-Richtung angegeben.

```text
Offset X

Offset Y
```

Negative Werte sind zulässig.

---

# 17. Farbe

Schatten und Glows unterstützen vollständige RGBA-Farben.

Standard:

- Schatten: Schwarz mit reduzierter Deckkraft
- Glow: Design-Akzentfarbe

---

# 18. Deckkraft

Die Deckkraft wird als Gleitkommazahl angegeben.

```text
0.0 = unsichtbar

1.0 = vollständig sichtbar
```

---

# 19. Mehrere Effekte

Ein Objekt darf mehrere Schatten und Glows gleichzeitig besitzen.

Beispiel:

- Ambient Shadow
- Drop Shadow
- Focus Glow

Die Reihenfolge wird durch die Effektliste bestimmt.

---

# 20. Reihenfolge

Effekte werden in folgender Reihenfolge gerendert:

```text
Shadow

↓

Fill

↓

Border

↓

Glow
```

---

# 21. Rounded Geometry

Schatten folgen exakt der gerundeten Geometrie.

Die Kontur wird aus dem Rounded-Geometry-System übernommen.

---

# 22. Clipping

Shadow- und Glow-Bereiche werden gegen den aktuellen Clippingbereich geprüft.

Unsichtbare Bereiche werden verworfen.

---

# 23. Masken

Masken beschneiden sowohl Schatten als auch Glows.

Die Effektberechnung erfolgt vor dem Masking.

---

# 24. Animationen

Folgende Eigenschaften dürfen animiert werden:

- Radius
- Farbe
- Deckkraft
- Offset

Die Animation erfolgt über das Motion-System.

---

# 25. Glass Design

Glass-Dialoge verwenden standardmäßig:

- Ambient Shadow
- Drop Shadow
- leichter Inner Glow

Diese Kombination erzeugt räumliche Tiefe.

---

# 26. Performance

Optimierungen:

- vorberechnete Blur-Kernel
- Effekt-Caching
- Dirty-Region-Unterstützung
- SIMD-Optimierung

---

# 27. Speicher

Effektdefinitionen werden zusammen mit dem Renderobjekt gespeichert.

Temporäre Puffer werden nur bei Bedarf verwendet.

---

# 28. Qualitätsstufen

Das System unterstützt:

- Niedrig
- Standard
- Hoch

Niedrige Qualitätsstufen dürfen kleinere Blur-Radien oder vereinfachte Kernel verwenden.

---

# 29. Sicherheit

Das System verhindert:

- negative Radien
- ungültige Offsets
- NaN-Werte
- Überläufe
- ungültige Farbdaten

---

# 30. API

```c
void
nova_shadow_set(
    nova_render_object_t* object,
    const nova_shadow_t* shadow);

void
nova_glow_set(
    nova_render_object_t* object,
    const nova_shadow_t* glow);

void
nova_effect_clear(
    nova_render_object_t* object);

void
nova_effect_render(
    const nova_render_object_t* object,
    nova_surface_t* surface);
```

---

# 31. Diagnosedaten

Zu erfassen sind:

- Anzahl Schatten
- Anzahl Glows
- Blur-Radius
- Renderzeit
- Cache Hits
- Cache Misses
- Qualitätsstufe
- Effektanzahl pro Frame

---

# 32. Testfälle

Zu testen sind:

- Drop Shadow
- Inner Shadow
- Ambient Shadow
- Outer Glow
- Inner Glow
- Focus Glow
- mehrere Effekte
- Clipping
- Masking
- Animation
- Qualitätswechsel

---

# 33. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Schattentypen korrekt dargestellt werden.
- Glows unabhängig von Schatten funktionieren.
- Effekte korrekt mit Rounded Geometry zusammenarbeiten.
- Clipping und Masken berücksichtigt werden.
- Animationen flüssig ablaufen.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Renderer identische Ergebnisse liefern.

---

# 34. Referenzen

## ADRs

- ADR-BOOTGFX-0009 – Schatten- und Glow-Rendering
- ADR-BOOTGFX-0011 – Qualitätsstufen für grafische Effekte
- ADR-BOOTDESIGN-0003 – Adaptives Glass-Design
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTRENDER-0010 – Clipping and Masks
- NPSPEC-BOOTRENDER-0011 – 2D Transformations
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing
- NPSPEC-BOOTRENDER-0013 – Gradients
- NPSPEC-BOOTRENDER-0014 – Rounded Geometry

---

# 35. Zusammenfassung

Das **Shadows and Glows System** erweitert die NovaOS Boot Rendering Engine um hochwertige Tiefen- und Hervorhebungseffekte. Durch softwarebasierte Berechnung, vollständige Integration in Clipping, Rounded Geometry und Alpha-Compositing sowie konfigurierbare Qualitätsstufen entstehen moderne Benutzeroberflächen mit räumlicher Tiefe und klarer visueller Hierarchie – konsistent auf BIOS- und UEFI-Systemen.