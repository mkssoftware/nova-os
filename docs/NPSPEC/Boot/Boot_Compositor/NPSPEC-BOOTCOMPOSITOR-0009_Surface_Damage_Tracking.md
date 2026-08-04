# NPSPEC-BOOTCOMPOSITOR-0009
# Surface Damage Tracking

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0009 |
| Titel | Surface Damage Tracking |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositor |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0001 bis NPSPEC-BOOTCOMPOSITOR-0008, NPSPEC-BOOTRENDER-0009 |
| Zugehörige ADRs | ADR-BOOTUI-0007, ADR-BOOTGFX-0005, ADR-BOOTPERF-0002, ADR-BOOTPERF-0006 |

---

# 1. Ziel

Diese Spezifikation definiert das **Surface Damage Tracking System** des NovaOS Boot Compositors.

Das System erkennt sämtliche Änderungen an Rendering-Surfaces und bestimmt exakt jene Bereiche, die vor dem nächsten Frame neu berechnet werden müssen.

Dadurch werden:

- unnötige Renderoperationen vermieden
- Speicherbandbreite reduziert
- Compositing beschleunigt
- Animationen flüssiger dargestellt
- Energieverbrauch minimiert

Das Damage Tracking bildet die Grundlage des Dirty-Region-Renderings. Das Prinzip, nur tatsächlich geänderte Bereiche ("Damage Regions") zu aktualisieren, wird auch in modernen Compositing-Systemen eingesetzt, um den Aufwand für das Re-Rendering zu reduzieren. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Surfaces des Bootmanagers.

Sie umfasst:

- Layer Surfaces
- Dialog Surfaces
- Overlay Surfaces
- Blur Surfaces
- Backdrop Surfaces
- Cursor Surface
- Offscreen Surfaces

Nicht Bestandteil sind:

- Framebuffer-Treiber
- Ressourcenverwaltung

---

# 3. Architektur

```text
UI Änderung
      │
      ▼
Surface Manager
      │
      ▼
Damage Tracker
      │
      ▼
Damage Merger
      │
      ▼
Dirty Region Renderer
      │
      ▼
Compositor
```

---

# 4. Grundprinzip

Jede Surface besitzt einen eigenen Damage-Zustand.

Verändert sich eine Surface, wird ausschließlich der tatsächlich betroffene Bereich markiert.

Beim nächsten Frame werden nur diese Bereiche neu gerendert.

---

# 5. Aufgaben

Das System übernimmt:

- Damage-Erkennung
- Damage-Speicherung
- Damage-Zusammenführung
- Surface-Invalidierung
- Dirty-Region-Erzeugung
- Damage-Vererbung
- Damage-Löschung

---

# 6. Damage Region

Eine Damage Region beschreibt einen geänderten rechteckigen Bereich.

```c
typedef struct
{
    int32_t x;
    int32_t y;

    uint32_t width;
    uint32_t height;

} nova_damage_rect_t;
```

---

# 7. Surface Status

Jede Surface besitzt einen Status.

```text
Clean

Dirty

Fully Dirty

Destroyed
```

Nur Dirty- und Fully-Dirty-Surfaces werden verarbeitet.

---

# 8. Damage Container

```c
typedef struct
{
    nova_damage_rect_t regions[64];

    uint32_t count;

    bool full_damage;

} nova_damage_set_t;
```

Die maximale Anzahl einzelner Regionen ist konfigurierbar.

---

# 9. Damage-Auslöser

Eine Surface wird Dirty bei:

- Positionsänderung
- Größenänderung
- Alphaänderung
- Inhaltsänderung
- Bildwechsel
- Textänderung
- Bluränderung
- Themewechsel
- Animation
- Sichtbarkeitsänderung

---

# 10. Full Damage

Eine vollständige Invalidierung erfolgt bei:

- Auflösungswechsel
- Themewechsel
- Rendererwechsel
- Recovery
- Surface-Neuerstellung
- Framebufferwechsel

---

# 11. Damage Merge

Überlappende Regionen werden automatisch vereinigt.

```text
Region A

+

Region B

↓

Region C
```

Dadurch sinkt die Anzahl der Renderoperationen.

---

# 12. Damage Propagation

Ändert sich eine Surface, wird die Änderung an abhängige Ebenen weitergegeben.

Beispiele:

- Blur Surface
- Overlay
- Dialog
- Glass Material

Die Weitergabe erfolgt rekursiv entlang der Compositor-Hierarchie. Änderungen an einer Oberfläche oder ihren Eigenschaften müssen sich auf alle betroffenen zusammengesetzten Ebenen auswirken. :contentReference[oaicite:1]{index=1}

---

# 13. Layer Damage

Jeder Layer besitzt einen eigenen Damage-Satz.

Der Compositor bildet daraus den endgültigen Damage-Bereich des Frames.

---

# 14. Backdrop Damage

Ändert sich der Hintergrund eines Glass-Elements, wird automatisch dessen Backdrop Surface invalidiert.

---

# 15. Cursor Damage

Beim Verschieben des Mauszeigers werden zwei Regionen markiert:

- alte Position
- neue Position

Dadurch bleibt der Renderaufwand minimal.

---

# 16. Animationen

Animierte Objekte erzeugen pro Frame neue Damage Regions.

Die Regionsgröße entspricht dem Vereinigungsbereich aus alter und neuer Objektposition.

---

# 17. Clipping

Damage Regions werden vor dem Rendern gegen den Clippingbereich geprüft.

Bereiche außerhalb des sichtbaren Bereichs werden verworfen.

---

# 18. Surface Cache

Nach erfolgreichem Rendern wird die Surface wieder als **Clean** markiert.

---

# 19. Lebenszyklus

```text
Clean

↓

Dirty

↓

Merge

↓

Rendering

↓

Compositing

↓

Clean
```

---

# 20. Speicherverwaltung

Alle Damage-Strukturen werden beim Start reserviert.

Heap-Allokationen während des Render-Hotpaths sind unzulässig.

---

# 21. Performance

Optimierungen:

- Region Merge
- frühes Clipping
- Surface Cache
- SIMD
- Bounding Box Tests

Die Verwaltung der Damage Regions muss in deterministischer Zeit erfolgen.

---

# 22. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Region
- negative Größen
- beschädigte Surface
- beschädigte Regionliste
- Überlauf der Regionanzahl

Kann eine Damage Region nicht korrekt verarbeitet werden, wird automatisch **Full Damage** ausgelöst.

---

# 23. Sicherheit

Das System verhindert:

- Out-of-Bounds-Zugriffe
- Nullpointer
- rekursive Damage-Schleifen
- doppelte Regionseinträge
- ungültige Rechtecke

Alle Eingaben werden vor der Verarbeitung validiert.

---

# 24. API

```c
nova_result_t
nova_damage_initialize(void);

void
nova_damage_add(
    nova_surface_t* surface,
    const nova_damage_rect_t* rect);

void
nova_damage_merge(
    nova_damage_set_t* damage);

void
nova_damage_clear(
    nova_surface_t* surface);

bool
nova_damage_is_dirty(
    const nova_surface_t* surface);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Anzahl Damage Regions
- Full Damage Frames
- Merge-Operationen
- Dirty Surfaces
- Clean Surfaces
- Renderfläche
- Compositingfläche
- Framezeit

---

# 26. Testfälle

Zu testen sind:

- einzelne Damage Region
- mehrere Regionen
- Region Merge
- Full Damage
- Cursorbewegung
- Animation
- Dialogöffnung
- Bluränderung
- Themewechsel
- Recovery

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- ausschließlich geänderte Surface-Bereiche neu gerendert werden.
- Damage Regions korrekt zusammengeführt werden.
- Full Damage zuverlässig funktioniert.
- Cursorbewegungen nur minimale Renderflächen erzeugen.
- Damage Propagation vollständig funktioniert.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.

---

# 28. Referenzen

## ADRs

- ADR-BOOTUI-0007 – Dirty-Region-Rendering
- ADR-BOOTGFX-0005 – Offscreen-Surfaces
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath
- ADR-BOOTPERF-0006 – Frame-Time-Messung und Diagnose

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTRENDER-0004 – Layer and Surface Management
- NPSPEC-BOOTRENDER-0009 – Dirty Region Rendering
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTRENDER-0020 – Rendering Diagnostics

---

# 29. Zusammenfassung

Das **Surface Damage Tracking System** bildet die Grundlage für effizientes inkrementelles Rendering im NovaOS Bootmanager. Durch die präzise Erkennung, Verwaltung und Zusammenführung geänderter Oberflächenbereiche werden ausschließlich tatsächlich betroffene Regionen neu gerendert. Die enge Integration mit Layern, Blur-, Glass- und Overlay-Systemen ermöglicht eine performante, deterministische und speichereffiziente Darstellung mit identischem Verhalten auf BIOS- und UEFI-Systemen.