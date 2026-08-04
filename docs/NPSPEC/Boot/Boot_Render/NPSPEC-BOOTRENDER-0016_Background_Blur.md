# NPSPEC-BOOTRENDER-0016
# Background Blur

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0016 |
| Titel | Background Blur |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0015 |
| Zugehörige ADRs | ADR-BOOTGFX-0010, ADR-BOOTGFX-0011, ADR-BOOTDESIGN-0003, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Background Blur System** der NovaOS Boot Rendering Engine.

Das System ermöglicht hochwertige Glass-Oberflächen durch die Weichzeichnung bereits gerenderter Hintergrundinhalte.

Der Blur wird verwendet für:

- Dialoge
- Menüs
- Overlays
- Panels
- Kontextmenüs
- Benachrichtigungen
- Glass-Container
- modale Fenster

Das System arbeitet vollständig softwarebasiert und erzeugt auf BIOS- und UEFI-Systemen identische Ergebnisse.

---

# 2. Geltungsbereich

Diese Spezifikation gilt ausschließlich für Hintergrundunschärfe.

Nicht Bestandteil sind:

- Schatten
- Glow
- Alpha-Blending
- Gradienten
- Bildfilter

---

# 3. Architektur

```text
Scene Graph
      │
      ▼
Layer Rendering
      │
      ▼
Offscreen Surface
      │
      ▼
Blur Engine
      │
      ▼
Glass Compositing
      │
      ▼
Framebuffer
```

---

# 4. Grundprinzip

Der Blur wird niemals auf dem gesamten Bildschirm berechnet.

Stattdessen erfolgt:

```text
Hintergrund

↓

Ausschneiden

↓

Blur

↓

Glass Overlay

↓

Compositing
```

Nur der tatsächlich sichtbare Bereich wird verarbeitet.

---

# 5. Blur Surface

Für jede Blur-Fläche existiert eine eigene Offscreen Surface.

Eigenschaften:

- Hintergrundkopie
- temporär
- nicht sichtbar
- wiederverwendbar

---

# 6. Blur Region

Jede Blur-Berechnung besitzt:

- Position
- Größe
- Radius
- Qualitätsstufe

```c
typedef struct
{
    nova_rect_t bounds;

    float radius;

    uint8_t quality;
} nova_blur_region_t;
```

---

# 7. Blur Radius

Der Radius bestimmt die Stärke der Unschärfe.

Empfohlene Werte:

| Verwendung | Radius |
|---|---:|
| Menü | 6 px |
| Button | 4 px |
| Dialog | 12 px |
| Glass Panel | 16 px |
| Overlay | 20 px |

Alle Werte werden über Design Tokens definiert.

---

# 8. Qualitätsstufen

Das System unterstützt:

- Low
- Standard
- High

Niedrige Qualitätsstufen verwenden kleinere Kernel und weniger Samples.

---

# 9. Blur Kernel

Die Engine unterstützt:

- Box Blur
- Gaussian Blur
- Dual-Pass Gaussian

Standard:

```text
Dual-Pass Gaussian
```

---

# 10. Blur Pipeline

```text
Offscreen Surface

↓

Horizontal Blur

↓

Vertikaler Blur

↓

Glass Overlay
```

Die Trennung reduziert den Rechenaufwand erheblich.

---

# 11. Clipping

Vor der Blur-Berechnung erfolgt:

- Bildschirm-Clipping
- Layer-Clipping
- Dialog-Clipping
- Masken-Clipping

Pixel außerhalb der Blur Region dürfen nicht verarbeitet werden.

---

# 12. Dirty Regions

Blur-Flächen werden vollständig in das Dirty-Region-System integriert.

Nur geänderte Bereiche werden neu berechnet.

---

# 13. Layerintegration

Blur verarbeitet ausschließlich bereits fertig gerenderte Hintergrund-Layer.

Eigene Layer werden niemals rekursiv geblurrt.

---

# 14. Glass Design

Glass-Flächen bestehen aus:

- Blur
- Transparenz
- Gradient
- leichter Innenbeleuchtung
- Schatten

Der Blur bildet die Grundlage des Glass-Effekts.

---

# 15. Transparenz

Nach der Blur-Berechnung wird die Glass-Fläche über Alpha-Compositing eingeblendet.

Die Deckkraft wird unabhängig vom Blur gesteuert.

---

# 16. Rounded Geometry

Blur-Flächen übernehmen automatisch die Geometrie des zugehörigen Objekts.

Rounded Rectangles erzeugen entsprechend gerundete Blur-Masken.

---

# 17. Masken

Blur unterstützt:

- Rechteckmasken
- Rounded Masks
- Alpha Masks

Die Maskierung erfolgt nach der Blur-Berechnung.

---

# 18. Animationen

Folgende Eigenschaften dürfen animiert werden:

- Radius
- Deckkraft
- Größe
- Position

Der Blur selbst wird nicht interpoliert, sondern pro Frame neu berechnet.

---

# 19. Caching

Statische Blur-Flächen dürfen zwischengespeichert werden.

Der Cache wird ungültig bei:

- Hintergrundänderung
- Größenänderung
- Positionsänderung
- Qualitätswechsel

---

# 20. Aktualisierung

Eine Blur-Fläche wird neu berechnet bei:

- Dirty Region
- Layeränderung
- Dialogbewegung
- Animation
- Themewechsel

---

# 21. Speicher

Blur verwendet ausschließlich vorreservierte Offscreen Surfaces.

Heap-Allokationen während des Renderings sind nicht zulässig.

---

# 22. Performance

Optimierungen:

- Dual-Pass Blur
- SIMD
- Dirty Regions
- Blur Cache
- Tile Rendering

---

# 23. Speicherbudget

Das Blur-System darf das konfigurierte Speicherbudget der Boot UI nicht überschreiten.

Bei Speichermangel wird automatisch eine niedrigere Qualitätsstufe gewählt.

---

# 24. Adaptive Degradation

Bei geringer Leistung darf:

- Radius reduziert werden
- Kernel vereinfacht werden
- Cache aggressiver verwendet werden

Die Benutzeroberfläche muss dabei konsistent bleiben.

---

# 25. Sicherheit

Das System verhindert:

- negative Radien
- ungültige Blur Regions
- Out-of-Bounds-Zugriffe
- rekursive Blur-Berechnungen
- doppelte Offscreen-Surfaces

---

# 26. API

```c
void
nova_blur_initialize(void);

void
nova_blur_shutdown(void);

void
nova_blur_render(
    const nova_blur_region_t* region,
    nova_surface_t* destination);

void
nova_blur_invalidate(
    const nova_rect_t* area);

void
nova_blur_clear_cache(void);
```

---

# 27. Diagnosedaten

Zu erfassen sind:

- Anzahl Blur-Flächen
- Blur-Radius
- Kerneltyp
- Cache Hits
- Cache Misses
- Renderzeit
- Speicherverbrauch
- Qualitätsstufe

---

# 28. Testfälle

Zu testen sind:

- kleine Blur-Flächen
- große Blur-Flächen
- Rounded Blur
- Dialog Blur
- mehrere Blur-Flächen
- Dirty Regions
- Cache
- Qualitätswechsel
- Animation
- Speichermangel

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Blur ausschließlich Hintergrundinformationen verarbeitet.
- Rounded Geometry vollständig unterstützt wird.
- Dirty Regions korrekt berücksichtigt werden.
- Glass-Oberflächen konsistent dargestellt werden.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- adaptive Qualitätsstufen funktionieren.
- BIOS- und UEFI-Renderer identische Ergebnisse liefern.

---

# 30. Referenzen

## ADRs

- ADR-BOOTGFX-0010 – Hintergrundunschärfe für Glass-Dialoge
- ADR-BOOTGFX-0011 – Qualitätsstufen für grafische Effekte
- ADR-BOOTDESIGN-0003 – Adaptives Glass-Design
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTRENDER-0004 – Layer and Surface Management
- NPSPEC-BOOTRENDER-0008 – Buffering and Frame Presentation
- NPSPEC-BOOTRENDER-0010 – Clipping and Masks
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing
- NPSPEC-BOOTRENDER-0014 – Rounded Geometry
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows

---

# 31. Zusammenfassung

Das **Background Blur System** stellt die Grundlage für hochwertige Glass-Oberflächen im NovaOS Bootmanager bereit. Durch Offscreen-Surfaces, Dual-Pass-Gaussian-Blur, Dirty-Region-Unterstützung, adaptives Qualitätsmanagement und vollständige Integration in das Compositing-System entsteht eine moderne, performante und deterministische Darstellung, die unabhängig vom Firmware-Backend identisch funktioniert.