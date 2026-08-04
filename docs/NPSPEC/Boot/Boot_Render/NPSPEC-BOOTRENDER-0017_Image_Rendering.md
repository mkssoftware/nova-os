# NPSPEC-BOOTRENDER-0017
# Image Rendering

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0017 |
| Titel | Image Rendering |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0016 |
| Zugehörige ADRs | ADR-BOOTRESOURCE-0003, ADR-BOOTRESOURCE-0006, ADR-BOOTGFX-0002, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **Image Rendering System** der NovaOS Boot Rendering Engine.

Das System stellt das Rendern von Rastergrafiken bereit und bildet die Grundlage für sämtliche grafischen Ressourcen des Bootmanagers.

Unterstützt werden:

- Logos
- Icons
- Hintergrundbilder
- Splash Screens
- Mauszeiger
- Illustrationen
- UI-Grafiken
- Texturelemente

Alle Bilder werden unabhängig von BIOS oder UEFI identisch dargestellt.

---

# 2. Geltungsbereich

Diese Spezifikation gilt ausschließlich für Rastergrafiken.

Unterstützte Formate:

- PNG
- BMP (intern)
- RAW RGBA

Nicht Bestandteil sind:

- SVG
- Schriftarten
- Video
- Animationen

---

# 3. Architektur

```text
Asset Package
      │
      ▼
Image Loader
      │
      ▼
Decoder
      │
      ▼
Image Cache
      │
      ▼
Image Renderer
      │
      ▼
Framebuffer
```

---

# 4. Bildlebenszyklus

```text
Datei laden

↓

Validieren

↓

Dekodieren

↓

Image erzeugen

↓

Cache

↓

Rendern

↓

Freigeben
```

---

# 5. Bildbeschreibung

```c
typedef struct
{
    uint32_t width;
    uint32_t height;

    uint32_t stride;

    nova_pixel_format_t format;

    void* pixels;
} nova_image_t;
```

---

# 6. Pixelformate

Unterstützt werden:

- RGBA8888
- BGRA8888
- RGB888
- RGB565

Intern wird bevorzugt:

```text
RGBA8888
Premultiplied Alpha
```

---

# 7. PNG-Unterstützung

PNG ist das Standardformat für sämtliche UI-Grafiken.

Unterstützt werden:

- 8 Bit
- Alpha-Kanal
- True Color
- Indexed Color (nach Konvertierung)

Nicht unterstützt werden:

- APNG
- Interlaced PNG

---

# 8. Rohdaten

Bereits dekodierte Bilder dürfen direkt verwendet werden.

Dies reduziert die Startzeit des Bootmanagers.

---

# 9. Laden

Bilder werden ausschließlich über das Ressourcenmanagement geladen.

Direkte Dateizugriffe des Renderers sind nicht zulässig.

---

# 10. Validierung

Vor dem Dekodieren werden geprüft:

- Dateigröße
- Header
- Format
- Bildgröße
- Speicherbedarf
- Prüfsumme

Ungültige Bilder werden verworfen.

---

# 11. Dekodierung

Die Dekodierung erfolgt vollständig vor dem ersten Rendern.

Während des Render-Hotpaths dürfen keine Bilddaten dekodiert werden.

---

# 12. Skalierung

Folgende Skalierungsarten werden unterstützt:

- Nearest Neighbor
- Bilinear

Standard:

```text
Bilinear
```

Für Pixelgrafiken darf Nearest Neighbor verwendet werden.

---

# 13. Positionierung

Bilder können:

- frei positioniert
- zentriert
- gestreckt
- proportional skaliert

werden.

---

# 14. Transformation

Unterstützt werden:

- Translation
- Rotation
- Skalierung
- Spiegelung

Die Transformation erfolgt vor der Rasterisierung.

---

# 15. Clipping

Vor dem Zeichnen wird das Bild gegen den aktuellen Clippingbereich geprüft.

Nicht sichtbare Bereiche werden nicht verarbeitet.

---

# 16. Alpha

Alle Bilder unterstützen:

- vollständige Transparenz
- Teiltransparenz
- Premultiplied Alpha

Das Blending erfolgt über das Alpha-Compositing-System.

---

# 17. Farbmanagement

Die Rendering Engine verarbeitet alle Bilder intern im selben Pixelformat.

Formatkonvertierungen erfolgen ausschließlich beim Laden.

---

# 18. Image Cache

Dekodierte Bilder dürfen zwischengespeichert werden.

Der Cache reduziert:

- Ladezeit
- Dekodierungszeit
- Speicherkopien

---

# 19. Speicherverwaltung

Bildspeicher wird durch das Ressourcenmanagement verwaltet.

Doppelte Bildkopien sind zu vermeiden.

---

# 20. Layerintegration

Bilder können auf beliebigen Rendering-Layern dargestellt werden.

Beispiele:

- Hintergrund
- UI
- Dialoge
- Overlay
- Cursor

---

# 21. Glass Design

Bilder dürfen mit folgenden Effekten kombiniert werden:

- Blur
- Schatten
- Glow
- Gradienten
- Transparenz

---

# 22. Dirty Regions

Beim Aktualisieren eines Bildes wird ausschließlich dessen Begrenzungsbereich invalidiert.

---

# 23. Performance

Optimierungen:

- Cache
- SIMD-Kopien
- Scanline Rendering
- Dirty Regions
- vorberechnete Alpha-Werte

---

# 24. Speicherbudget

Die maximale Bildgröße wird durch das Boot-UI-Speicherbudget begrenzt.

Zu große Bilder dürfen nicht geladen werden.

---

# 25. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiges Dateiformat
- beschädigte Datei
- Speicherfehler
- ungültige Bildgröße
- ungültiger Header
- Dekodierungsfehler

---

# 26. Sicherheit

Das System verhindert:

- Integer Overflow
- Out-of-Bounds-Zugriffe
- beschädigte Bilddaten
- Nullpointer
- ungültige Pixelformate

Alle Eingabedaten müssen vollständig validiert werden.

---

# 27. API

```c
nova_image_t*
nova_image_load(
    const char* resource);

void
nova_image_destroy(
    nova_image_t* image);

void
nova_image_render(
    const nova_image_t* image,
    nova_surface_t* surface,
    const nova_rect_t* destination);

nova_size_t
nova_image_get_size(
    const nova_image_t* image);
```

---

# 28. Diagnosedaten

Zu erfassen sind:

- Anzahl geladener Bilder
- Cache Hits
- Cache Misses
- Dekodierungszeit
- Speicherverbrauch
- Bildgrößen
- Renderzeit
- Fehleranzahl

---

# 29. Testfälle

Zu testen sind:

- PNG mit Alpha
- PNG ohne Alpha
- große Bilder
- kleine Icons
- Skalierung
- Rotation
- Clipping
- Cache
- beschädigte Dateien
- Speichermangel

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- PNG-Dateien korrekt geladen werden.
- Alpha-Blending fehlerfrei funktioniert.
- Bilder korrekt transformiert werden.
- Clipping vollständig berücksichtigt wird.
- der Image Cache ordnungsgemäß arbeitet.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Renderer identische Ergebnisse liefern.

---

# 31. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0003 – PNG als Rastergrafikformat
- ADR-BOOTRESOURCE-0006 – Vorverarbeitete und validierte Ressourcen
- ADR-BOOTGFX-0002 – Alpha-Compositing im Bootmanager
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTRENDER-0008 – Buffering and Frame Presentation
- NPSPEC-BOOTRENDER-0010 – Clipping and Masks
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing
- NPSPEC-BOOTRENDER-0014 – Rounded Geometry
- NPSPEC-BOOTRESOURCE-0001 – Boot Asset Package

---

# 32. Zusammenfassung

Das **Image Rendering System** stellt die Darstellung sämtlicher Rastergrafiken innerhalb des NovaOS Bootmanagers bereit. Durch validierte Ressourcen, PNG-Unterstützung, einheitliche Pixelformate, integriertes Alpha-Compositing und einen leistungsfähigen Image Cache entstehen hochwertige und performante Benutzeroberflächen mit identischem Verhalten auf BIOS- und UEFI-Systemen.