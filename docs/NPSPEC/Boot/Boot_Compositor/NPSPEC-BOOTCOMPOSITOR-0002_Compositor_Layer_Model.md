# NPSPEC-BOOTCOMPOSITOR-0002
# Compositor Layer Model

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0002 |
| Titel | Compositor Layer Model |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositor |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0001, NPSPEC-BOOTRENDER-0004, NPSPEC-BOOTRENDER-0012 |
| Zugehörige ADRs | ADR-BOOTGFX-0004, ADR-BOOTGFX-0005, ADR-BOOTUI-0006, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Layer Model** des NovaOS Boot Compositors.

Das Layer Model beschreibt den logischen Aufbau sämtlicher sichtbarer Ebenen der Bootoberfläche sowie deren Lebenszyklus, Eigenschaften und Beziehungen.

Es stellt sicher, dass:

- alle UI-Elemente eindeutig einer Ebene zugeordnet sind,
- Rendering und Compositing voneinander getrennt bleiben,
- Z-Reihenfolgen deterministisch sind,
- Effekte ausschließlich auf definierte Ebenen wirken,
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Ebenen innerhalb der Boot UI.

Sie umfasst:

- Hintergrund
- Wallpaper
- Panels
- Dialoge
- Menüs
- Overlays
- Mauszeiger
- Debug Layer
- Recovery Layer

Nicht Bestandteil sind:

- Scene Graph
- Layout Engine
- Animation Scheduler

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
Compositor Layer Model
      │
      ▼
Framebuffer Backend
```

---

# 4. Grundprinzip

Jede sichtbare Darstellung erfolgt innerhalb genau eines Layers.

Layer besitzen keine eigenen Zeichenfunktionen.

Sie enthalten ausschließlich gerenderte Surfaces sowie Darstellungsinformationen.

---

# 5. Layerhierarchie

```text
Root
 ├── Background
 ├── Desktop
 ├── Dialog
 ├── Popup
 ├── Overlay
 ├── Cursor
 └── Debug
```

Die Hierarchie dient ausschließlich der Organisation.

Die tatsächliche Darstellung erfolgt über den Z-Index.

---

# 6. Layertypen

Die Engine unterstützt mindestens:

- Background
- Wallpaper
- Desktop
- Panel
- Dialog
- Popup
- Notification
- Overlay
- Cursor
- Debug

Weitere Layertypen können ergänzt werden.

---

# 7. Layerbeschreibung

```c
typedef struct
{
    uint32_t id;

    nova_layer_type_t type;

    int32_t z_index;

    bool visible;

    float opacity;

    nova_surface_t* surface;

} nova_layer_t;
```

---

# 8. Eigenschaften

Jeder Layer besitzt:

- eindeutige ID
- Typ
- Sichtbarkeit
- Deckkraft
- Surface
- Dirty Status
- Transformationsmatrix
- Blendmodus
- Clippingbereich

---

# 9. Root Layer

Der Root Layer repräsentiert den vollständigen Bildschirm.

Er besitzt:

- keine Surface
- keine Transparenz
- keine Transformation

Alle übrigen Layer sind logisch diesem Layer untergeordnet.

---

# 10. Hintergrund

Der Hintergrundlayer enthält:

- Hintergrundfarbe
- Hintergrundbild
- Splashgrafik

Er befindet sich immer auf der untersten Ebene.

---

# 11. Desktop Layer

Der Desktop Layer enthält:

- Bootmenü
- Listen
- Buttons
- Icons
- Statusinformationen

Er bildet die Standardarbeitsfläche des Bootmanagers.

---

# 12. Dialog Layer

Dialoge werden grundsätzlich auf einem eigenen Layer dargestellt.

Eigenschaften:

- modal
- Blur
- Schatten
- Glass Design

---

# 13. Popup Layer

Popup Layer dienen für:

- Kontextmenüs
- Untermenüs
- Tooltips

Sie liegen oberhalb normaler Dialoge.

---

# 14. Overlay Layer

Overlay Layer enthalten:

- Ladeanzeigen
- Sperrbildschirme
- Recovery Overlay
- Sicherheitswarnungen

---

# 15. Cursor Layer

Der Mauszeiger besitzt einen eigenen Layer.

Eigenschaften:

- höchste Standardpriorität
- keine Transparenzvererbung
- Dirty Region nur für Cursorbereich

---

# 16. Debug Layer

Der Debug Layer ist ausschließlich in Debug-Builds aktiv.

Er kann enthalten:

- FPS
- Dirty Regions
- Clipping
- Layergrenzen
- Speicherinformationen

---

# 17. Z-Reihenfolge

Standard:

| Ebene | Z |
|---|---:|
| Background | 0 |
| Wallpaper | 50 |
| Desktop | 100 |
| Dialog | 300 |
| Popup | 400 |
| Overlay | 500 |
| Cursor | 900 |
| Debug | 1000 |

---

# 18. Sichtbarkeit

Ein Layer wird verarbeitet wenn:

- sichtbar
- gültige Surface vorhanden
- Deckkraft > 0
- Dirty oder Full Render aktiv

---

# 19. Deckkraft

Jeder Layer besitzt eine globale Deckkraft.

```text
0.0

↓

unsichtbar

1.0

↓

voll sichtbar
```

Die Deckkraft wirkt zusätzlich zum Alpha einzelner Pixel.

---

# 20. Transformation

Layer unterstützen:

- Translation
- Skalierung
- Rotation

Transformationen wirken auf den gesamten Layer.

---

# 21. Clipping

Jeder Layer besitzt einen eigenen Clippingbereich.

Alle Renderoperationen werden auf diesen Bereich begrenzt.

---

# 22. Blendmodus

Pflicht:

- Normal

Optional:

- Add
- Multiply
- Screen
- Overlay

Nicht unterstützte Modi werden auf **Normal** reduziert.

---

# 23. Dirty Status

Jeder Layer besitzt einen Dirty Status.

```text
Clean

Dirty

Full Dirty
```

Der Compositor verwendet diese Information zur Auswahl des Renderbereichs.

---

# 24. Layerlebenszyklus

```text
Erzeugen

↓

Initialisieren

↓

Aktiv

↓

Dirty

↓

Compositing

↓

Inaktiv

↓

Freigeben
```

---

# 25. Layerverwaltung

Layer werden ausschließlich über den Layer Manager erzeugt.

Direkte Layererzeugung durch Controls ist nicht zulässig.

---

# 26. Speicherverwaltung

Layer besitzen keine eigenen Speicherallokationen.

Surfaces werden zentral verwaltet.

Layer enthalten ausschließlich Referenzen.

---

# 27. Performance

Optimierungen:

- Dirty Regions
- Layer Caching
- stabile Sortierung
- frühes Verwerfen unsichtbarer Layer

Heap-Allokationen während des Compositings sind verboten.

---

# 28. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- doppelte Layer-ID
- ungültiger Z-Index
- fehlende Surface
- rekursive Layer
- ungültige Referenzen

Fehler dürfen den Bootprozess nicht blockieren.

---

# 29. Sicherheit

Das System verhindert:

- Layerzyklen
- doppelte Parent-Beziehungen
- Nullpointer
- ungültige Blendmodi
- Out-of-Bounds-Zugriffe

---

# 30. API

```c
nova_result_t
nova_layer_create(
    nova_layer_t* layer);

nova_result_t
nova_layer_destroy(
    nova_layer_t* layer);

nova_result_t
nova_layer_set_visible(
    nova_layer_t* layer,
    bool visible);

nova_result_t
nova_layer_set_opacity(
    nova_layer_t* layer,
    float opacity);

nova_result_t
nova_layer_set_zindex(
    nova_layer_t* layer,
    int32_t z_index);
```

---

# 31. Diagnosedaten

Zu erfassen sind:

- Layeranzahl
- sichtbare Layer
- Dirty Layer
- Blendmodi
- Speicherverbrauch
- Z-Änderungen
- Layerwechsel
- Compositingzeit

---

# 32. Testfälle

Zu testen sind:

- Layererzeugung
- Layerlöschung
- Z-Reihenfolge
- Transparenz
- Clipping
- Blendmodi
- Dirty Layer
- Cursor Layer
- Dialog Layer
- Recovery Layer

---

# 33. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jeder sichtbare Inhalt genau einem Layer zugeordnet ist.
- Layer deterministisch sortiert werden.
- Transparenz korrekt verarbeitet wird.
- Dirty Layer vollständig unterstützt werden.
- keine Heap-Allokationen im Compositing-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 34. Referenzen

## ADRs

- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTGFX-0005 – Offscreen-Surfaces
- ADR-BOOTUI-0006 – Resolution and Scaling System
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTRENDER-0004 – Layer and Surface Management
- NPSPEC-BOOTRENDER-0012 – Alpha Compositing
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur

---

# 35. Zusammenfassung

Das **Compositor Layer Model** definiert die vollständige Organisation aller grafischen Ebenen des NovaOS Bootmanagers. Durch eine klar strukturierte Layerhierarchie, deterministische Z-Reihenfolgen, integrierte Transparenz-, Clipping- und Dirty-Region-Unterstützung sowie eine strikte Trennung zwischen Rendering und Compositing entsteht eine robuste Grundlage für moderne Glass-Oberflächen mit konsistentem Verhalten auf BIOS- und UEFI-Systemen.