# NPSPEC-BOOTCOMPOSITOR-0005
# Backdrop Processing

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0005 |
| Titel | Backdrop Processing |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositor |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0001 bis NPSPEC-BOOTCOMPOSITOR-0004, NPSPEC-BOOTRENDER-0016 |
| Zugehörige ADRs | ADR-BOOTGFX-0010, ADR-BOOTGFX-0004, ADR-BOOTDIALOG-0003, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Backdrop Processing** des NovaOS Boot Compositors.

Backdrop Processing beschreibt sämtliche Verarbeitungsschritte, die auf dem Bildinhalt **hinter** einem UI-Element durchgeführt werden, bevor dieses dargestellt wird.

Das System bildet die Grundlage für:

- Glass Design
- Hintergrundunschärfe
- Hintergrundabdunklung
- Farbfilter
- Hintergrundtönung
- Kontrastanpassung
- Frosted-Glass-Effekte

Alle Backdrop-Effekte werden ausschließlich auf bereits fertig gerenderte Hintergrundebenen angewendet. Dieses Prinzip entspricht modernen Compositing-Systemen, bei denen visuelle Ebenen nach dem Rendern zusammengesetzt und mit Effekten versehen werden. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche UI-Elemente mit Hintergrundeffekten.

Sie umfasst:

- Dialoge
- Panels
- Menüs
- Kontextmenüs
- Overlays
- Popups
- Glass Container
- Benachrichtigungen

Nicht Bestandteil sind:

- Schatten
- Glow
- Textdarstellung
- Bildrendering

---

# 3. Architektur

```text
Background Layer
        │
        ▼
Backdrop Capture
        │
        ▼
Backdrop Processing
        │
        ▼
Glass Overlay
        │
        ▼
Compositor
        │
        ▼
Framebuffer
```

---

# 4. Grundprinzip

Backdrop Processing verarbeitet niemals den vollständigen Bildschirm.

Es verarbeitet ausschließlich den Bereich hinter dem darzustellenden Objekt.

```text
Hintergrund

↓

Capture

↓

Effekte

↓

Overlay

↓

Compositing
```

---

# 5. Aufgaben

Das System übernimmt:

- Backdrop Capture
- Blur
- Farbfilter
- Tönung
- Kontrastanpassung
- Sättigungsänderung
- Hintergrundabdunklung
- Backdrop Cache

---

# 6. Backdrop Region

Jede Verarbeitung erfolgt innerhalb einer Backdrop Region.

```c
typedef struct
{
    nova_rect_t bounds;

    bool dirty;

    uint32_t effect_mask;

} nova_backdrop_region_t;
```

---

# 7. Backdrop Capture

Vor jeder Verarbeitung wird der Hintergrund kopiert.

Es werden ausschließlich Layer unterhalb des aktuellen Objekts berücksichtigt.

Der aktuelle Layer darf niemals Bestandteil seines eigenen Backdrops sein.

---

# 8. Capture-Reihenfolge

```text
Background

↓

Desktop

↓

Dialoge

↓

Capture

↓

Aktuelles Overlay
```

Alle höheren Layer bleiben unberücksichtigt.

---

# 9. Blur

Nach dem Capture wird optional ein Blur angewendet.

Der Blur wird durch NPSPEC-BOOTRENDER-0016 definiert.

---

# 10. Farbfilter

Unterstützte Farbfilter:

- keine Änderung
- leichte Aufhellung
- leichte Abdunklung
- Graustufen
- benutzerdefinierter Farbfilter

Standardmäßig erfolgt keine Farbveränderung.

---

# 11. Hintergrundtönung

Nach dem Blur darf eine halbtransparente Farbschicht aufgetragen werden.

Typische Farben:

- Weiß
- Schwarz
- Design-Akzentfarbe

Die Deckkraft ist frei konfigurierbar.

---

# 12. Kontrast

Optional kann der Hintergrundkontrast verändert werden.

Typische Werte:

- 90 %
- 100 %
- 110 %

Standard:

```text
100 %
```

---

# 13. Sättigung

Optional darf die Farbsättigung verändert werden.

Beispiele:

```text
80 %

100 %

120 %
```

---

# 14. Reihenfolge der Verarbeitung

```text
Capture

↓

Blur

↓

Kontrast

↓

Sättigung

↓

Farbfilter

↓

Tint

↓

Alpha
```

Diese Reihenfolge ist verbindlich.

---

# 15. Glass Design

Glass-Oberflächen verwenden standardmäßig:

- Blur
- weiße Tönung
- reduzierte Deckkraft
- Schatten
- Border

Backdrop Processing liefert den vorbereiteten Hintergrund.

---

# 16. Hintergrundabdunklung

Modale Dialoge dürfen zusätzlich eine Hintergrundabdunklung anwenden.

Diese erfolgt nach dem Blur.

---

# 17. Clipping

Backdrop Processing arbeitet ausschließlich innerhalb des Clippingbereichs.

Pixel außerhalb der Region werden nicht verarbeitet.

---

# 18. Rounded Geometry

Die Backdrop Region übernimmt automatisch die Geometrie des darzustellenden Objekts.

Gerundete Dialoge erzeugen entsprechend gerundete Backdrop-Masken.

---

# 19. Dirty Regions

Backdrop Processing unterstützt vollständiges Dirty-Region-Rendering.

Nur geänderte Bereiche werden neu berechnet.

---

# 20. Backdrop Cache

Statische Backdrops dürfen zwischengespeichert werden.

Der Cache wird ungültig bei:

- Hintergrundänderung
- Positionsänderung
- Größenänderung
- Bluränderung
- Themewechsel

---

# 21. Qualitätsprofile

Backdrop Processing verwendet die Rendering Quality Profiles.

Je nach Profil können reduziert werden:

- Blur Radius
- Kontrast
- Sättigung
- Tint
- Samplingqualität

---

# 22. Speicherverwaltung

Für Backdrop Processing werden Offscreen Surfaces verwendet.

Diese werden beim Start reserviert.

Heap-Allokationen während des Renderings sind verboten.

---

# 23. Performance

Optimierungen:

- Dirty Regions
- Tile Rendering
- SIMD
- Backdrop Cache
- frühes Clipping

Die Verarbeitung darf ausschließlich den benötigten Bereich betreffen.

---

# 24. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Region
- fehlende Surface
- beschädigter Cache
- ungültiger Blur
- Speicherfehler

Bei einem Fehler wird unmittelbar der unveränderte Hintergrund verwendet.

---

# 25. Sicherheit

Das System verhindert:

- rekursive Backdrop-Berechnungen
- Selbstreferenzen
- Nullpointer
- Out-of-Bounds-Zugriffe
- ungültige Effektparameter

Alle Parameter müssen vor der Verarbeitung validiert werden.

---

# 26. API

```c
nova_result_t
nova_backdrop_initialize(void);

nova_result_t
nova_backdrop_capture(
    const nova_rect_t* bounds);

nova_result_t
nova_backdrop_process(
    const nova_backdrop_region_t* region);

nova_result_t
nova_backdrop_render(
    nova_surface_t* destination);

void
nova_backdrop_invalidate(
    const nova_rect_t* area);
```

---

# 27. Diagnosedaten

Zu erfassen sind:

- Anzahl Backdrop Regions
- Blur-Radius
- Cache Hits
- Cache Misses
- Capturezeit
- Processingzeit
- Speicherverbrauch
- Dirty Regions

---

# 28. Testfälle

Zu testen sind:

- kleiner Dialog
- großer Dialog
- Glass Panel
- modaler Dialog
- Hintergrundänderung
- Dirty Regions
- Backdrop Cache
- Blur deaktiviert
- Qualitätswechsel
- Recovery

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- ausschließlich Hintergrundlayer verarbeitet werden.
- der aktuelle Layer niemals Bestandteil seines eigenen Backdrops ist.
- Blur, Tint und Kontrast korrekt angewendet werden.
- Dirty Regions vollständig unterstützt werden.
- der Backdrop Cache korrekt arbeitet.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.

---

# 30. Referenzen

## ADRs

- ADR-BOOTGFX-0010 – Hintergrundunschärfe für Glass-Dialoge
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTDIALOG-0003 – Glass-Design mit adaptivem Fallback
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTCOMPOSITOR-0003 – Overlay Layer
- NPSPEC-BOOTCOMPOSITOR-0004 – Modal Layer
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles

---

# 31. Zusammenfassung

Das **Backdrop Processing** bildet die Grundlage sämtlicher modernen Glass- und Transparenzeffekte des NovaOS Bootmanagers. Durch das gezielte Erfassen der Hintergrundlayer, die anschließende Verarbeitung mit Blur-, Farb- und Kontrastfiltern sowie die Integration in den Boot Compositor entstehen hochwertige Oberflächen mit konsistentem Erscheinungsbild. Dank Backdrop Cache, Dirty-Region-Unterstützung und deterministischer Verarbeitung arbeitet das System performant und liefert identische Ergebnisse auf BIOS- und UEFI-Systemen.