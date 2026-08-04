# NPSPEC-BOOTCOMPOSITOR-0001
# Boot Compositor

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0001 |
| Titel | Boot Compositor |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositing |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0020, NPSPEC-BOOTUI-0004 |
| Zugehörige ADRs | ADR-BOOTGFX-0002, ADR-BOOTGFX-0003, ADR-BOOTGFX-0004, ADR-BOOTUI-0006, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert den **Boot Compositor** der NovaOS Boot UI Engine.

Der Boot Compositor ist für das Zusammenführen sämtlicher grafischer Ebenen zu einem fertigen Bild verantwortlich. Er bildet die letzte Stufe der Rendering-Pipeline unmittelbar vor der Präsentation auf dem Bildschirm.

Zu seinen Aufgaben gehören:

- Layer-Zusammenführung
- Alpha-Compositing
- Z-Reihenfolge
- Transparenz
- Blur-Einbindung
- Schatten
- Glow-Effekte
- Dirty-Region-Compositing
- Offscreen-Surfaces
- Frame-Erzeugung

Der Compositor arbeitet vollständig unabhängig vom verwendeten Grafik-Backend.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche grafischen Inhalte des Bootmanagers.

Sie umfasst:

- Desktop
- Hintergrund
- Panels
- Dialoge
- Menüs
- Mauszeiger
- Benachrichtigungen
- Overlays
- Debug-Oberflächen

Nicht Bestandteil sind:

- Rasterisierung
- Layout
- Animationen
- Ressourcenverwaltung

---

# 3. Architektur

```text
Scene Graph
       │
       ▼
Render Engine
       │
       ▼
Layer Manager
       │
       ▼
Boot Compositor
       │
       ▼
Framebuffer Backend
       │
       ▼
Display
```

---

# 4. Grundprinzip

Der Compositor verarbeitet ausschließlich bereits gerenderte Layer.

Er erzeugt keine grafischen Inhalte selbst.

Alle Zeichenoperationen erfolgen vor dem Compositing.

---

# 5. Aufgaben

Der Compositor übernimmt:

- Layer sortieren
- Sichtbarkeit bestimmen
- Alpha-Blending
- Blur anwenden
- Schatten integrieren
- Glows integrieren
- Dirty Regions auswerten
- fertigen Frame erzeugen

---

# 6. Layermodell

Jeder Layer besitzt:

- ID
- Typ
- Sichtbarkeit
- Z-Index
- Surface
- Deckkraft
- Blendmodus
- Transformationsmatrix

---

# 7. Standard-Layer

Empfohlene Layerreihenfolge:

| Z | Ebene |
|---:|---|
| 0 | Hintergrund |
| 100 | Dekoration |
| 200 | Desktop |
| 300 | Dialoge |
| 400 | Menüs |
| 500 | Tooltips |
| 600 | Overlay |
| 700 | Mauszeiger |
| 800 | Debug |

---

# 8. Layerstruktur

```c
typedef struct
{
    uint32_t id;

    int32_t z_index;

    bool visible;

    float opacity;

    nova_surface_t* surface;

} nova_layer_t;
```

---

# 9. Layerreihenfolge

Vor jedem Frame werden alle sichtbaren Layer nach ihrem Z-Index sortiert.

Die Sortierung muss stabil sein.

Layer mit gleichem Z-Index behalten ihre Einfügereihenfolge.

---

# 10. Sichtbarkeit

Ein Layer wird verarbeitet wenn:

- sichtbar
- gültige Surface
- Deckkraft > 0
- innerhalb des Clippingbereichs

Unsichtbare Layer werden übersprungen.

---

# 11. Alpha-Compositing

Das Compositing erfolgt ausschließlich mit Premultiplied Alpha.

Jeder Layer wird auf den bisherigen Bildinhalt aufgetragen.

---

# 12. Blendmodus

Verpflichtend:

- Normal

Optional:

- Add
- Multiply
- Screen
- Overlay

Nicht unterstützte Modi müssen automatisch auf **Normal** zurückfallen.

---

# 13. Offscreen-Surfaces

Folgende Layer dürfen Offscreen gerendert werden:

- Blur
- Dialog
- Overlay
- Animation
- Glass

Die Offscreen-Surface muss vor dem Compositing vollständig fertiggestellt sein.

---

# 14. Blur

Blur wird grundsätzlich vor dem Alpha-Compositing berechnet.

Der Blur liest ausschließlich darunterliegende Layer.

Ein Layer darf niemals sich selbst verwischen.

---

# 15. Schatten

Schatten werden unmittelbar vor dem eigentlichen Layer zusammengesetzt.

Die Reihenfolge lautet:

```text
Shadow

↓

Layer

↓

Border

↓

Glow
```

---

# 16. Glow

Glow wird nach dem eigentlichen Layer dargestellt.

Er darf benachbarte Layer optisch überlagern, jedoch keine höhere Z-Ebene überschreiten.

---

# 17. Dirty Regions

Der Compositor verarbeitet standardmäßig ausschließlich Dirty Regions.

Full Compositing erfolgt bei:

- Erstinitialisierung
- Auflösungswechsel
- Themewechsel
- Recovery
- vollständiger Invalidierung

---

# 18. Clipping

Vor jeder Layeroperation erfolgt:

- Bildschirm-Clipping
- Layer-Clipping
- Dirty-Region-Clipping

Dadurch werden unnötige Pixeloperationen vermieden.

---

# 19. Transparenz

Layer besitzen eine globale Deckkraft.

Zusätzlich bleibt die Transparenz einzelner Pixel erhalten.

Beide Werte werden multipliziert.

---

# 20. Glass-Oberflächen

Glass-Flächen bestehen aus:

- Blur
- Gradient
- Transparenz
- Shadow
- Border

Diese Komponenten werden vollständig durch den Compositor zusammengeführt.

---

# 21. Mauszeiger

Der Mauszeiger bildet immer die oberste normale Ebene.

Er wird grundsätzlich zuletzt dargestellt.

---

# 22. Present

Nach Abschluss des Compositings wird der fertige Frame an den Present Scheduler übergeben.

Der Compositor besitzt keinen direkten Zugriff auf den Framebuffer.

---

# 23. Speicherverwaltung

Der Compositor verwaltet:

- Layerlisten
- Surface-Referenzen
- temporäre Effektflächen

Heap-Allokationen während des Compositings sind nicht zulässig.

---

# 24. Performance

Anforderungen:

- deterministische Laufzeit
- Dirty-Region-Unterstützung
- SIMD zulässig
- keine doppelten Layerdurchläufe
- minimale Speicherzugriffe

---

# 25. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Layer
- fehlende Surface
- ungültige Z-Reihenfolge
- Blendfehler
- beschädigte Effektfläche

Fehler dürfen den Bootvorgang nicht unterbrechen.

---

# 26. Sicherheit

Das System verhindert:

- rekursive Layer
- doppelte Layerreferenzen
- Out-of-Bounds-Zugriffe
- Nullpointer
- ungültige Blendmodi

Alle Eingabedaten werden vor der Verarbeitung validiert.

---

# 27. API

```c
nova_result_t
nova_compositor_initialize(void);

nova_result_t
nova_compositor_shutdown(void);

nova_result_t
nova_compositor_begin_frame(void);

nova_result_t
nova_compositor_submit_layer(
    const nova_layer_t* layer);

nova_result_t
nova_compositor_compose(
    const nova_damage_set_t* damage);

nova_result_t
nova_compositor_end_frame(void);
```

---

# 28. Diagnosedaten

Zu erfassen sind:

- Anzahl Layer
- sichtbare Layer
- Blendoperationen
- Blur-Operationen
- Shadow-Operationen
- Glow-Operationen
- Dirty Regions
- Compositingzeit
- Speicherverbrauch

---

# 29. Testfälle

Zu testen sind:

- ein Layer
- mehrere Layer
- gleiche Z-Reihenfolge
- Transparenz
- Alpha-Blending
- Blur
- Schatten
- Glow
- Dirty Regions
- vollständiges Re-Compositing
- Recovery

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Layer korrekt zusammengesetzt werden.
- Alpha-Compositing deterministisch arbeitet.
- Blur ausschließlich Hintergrundinformationen verwendet.
- Schatten und Glows korrekt integriert werden.
- Dirty Regions vollständig unterstützt werden.
- keine Heap-Allokationen im Compositing-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.

---

# 31. Referenzen

## ADRs

- ADR-BOOTGFX-0002 – Alpha-Compositing im Bootmanager
- ADR-BOOTGFX-0003 – Premultiplied Alpha
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTUI-0006 – Resolution and Scaling System
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTRENDER-0004 – Layer and Surface Management
- NPSPEC-BOOTRENDER-0008 – Buffering and Frame Presentation
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTRENDER-0019 – Software Rendering Fallback

---

# 32. Zusammenfassung

Der **Boot Compositor** bildet die zentrale Komponente der NovaOS Boot UI Rendering-Pipeline. Er führt sämtliche gerenderten Layer unter Berücksichtigung von Z-Reihenfolge, Alpha-Compositing, Transparenz, Blur, Schatten und Glow zu einem finalen Frame zusammen. Durch deterministische Verarbeitung, vollständige Dirty-Region-Unterstützung und eine klare Trennung zwischen Rendering und Präsentation gewährleistet er eine hochwertige, performante und konsistente Darstellung der Bootoberfläche auf BIOS- und UEFI-Systemen.