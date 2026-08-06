# NPSPEC-BOOTCONTROL-SCROLLVIEW-0001
# Scroll View

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-SCROLLVIEW-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Scroll View Control** der NovaOS Boot UI.

Eine Scroll View ermöglicht die Darstellung von Inhalten, die größer als der verfügbare Anzeigebereich sind. Sie stellt einen Viewport mit horizontaler und/oder vertikaler Navigation bereit und dient als Container für andere Controls.

Typische Einsatzbereiche:

- Listen
- Protokolle
- Einstellungen
- Lizenztexte
- Diagnosedaten
- Recovery-Berichte
- Dateiansichten

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Scroll Views.

Sie umfasst:

- vertikale Scroll View
- horizontale Scroll View
- bidirektionale Scroll View

Nicht Bestandteil sind:

- Scrollbars
- Listen
- Tabellen

---

# 3. Architektur

```text
Boot Control Framework

↓

Scroll View

├── Viewport

├── Content Container

├── Scrollbars (optional)

└── Renderer
```

---

# 4. Grundprinzip

Eine Scroll View besitzt genau einen sichtbaren Viewport.

Der gesamte Inhalt bleibt unverändert und wird lediglich relativ zum Viewport verschoben.

Die Scroll View verändert niemals den Inhalt selbst.

---

# 5. Aufgaben

Die Scroll View übernimmt:

- Clipping
- Scrollverwaltung
- Viewportverwaltung
- Fokusweiterleitung
- Themeintegration
- Accessibility
- Ereignisverarbeitung

---

# 6. Aufbau

```text
Scroll View

├── Background

├── Viewport

├── Content

├── Vertical Scrollbar (optional)

├── Horizontal Scrollbar (optional)

└── Focus Indicator
```

---

# 7. Eigenschaften

Jede Scroll View besitzt:

- ID
- Inhalt
- Scrollposition X
- Scrollposition Y
- Sichtbarkeit
- Style
- Template

Optional:

- Scrollbars
- Overscroll
- Padding

---

# 8. Scrollrichtungen

Unterstützt werden:

```text
Vertikal
```

```text
Horizontal
```

```text
Beide Richtungen
```

Die Richtung wird unabhängig für jede Achse konfiguriert.

---

# 9. Zustände

```text
Normal

Focused

Disabled

Busy
```

Optional:

```text
Scrolling
```

---

# 10. Viewport

Der Viewport definiert den sichtbaren Bereich.

Eigenschaften:

- feste Größe
- Clipping
- DPI-unabhängig
- Layoutgesteuert

Inhalte außerhalb des Viewports werden nicht dargestellt.

---

# 11. Eingaben

Unterstützt:

- Mausrad
- Touch
- Tastatur
- Controller

Tastatur:

- ↑
- ↓
- ←
- →
- Page Up
- Page Down
- Home
- End

---

# 12. Scrollverhalten

Unterstützt werden:

- Zeilenweise
- Seitenweise
- Pixelgenau
- Smooth Scrolling (optional)

Das Verhalten wird zentral konfiguriert.

---

# 13. Fokus

Der Fokus bleibt auf dem aktuell ausgewählten Kind-Control.

Die Scroll View sorgt automatisch dafür, dass das fokussierte Element vollständig sichtbar ist.

---

# 14. Darstellung

Die Scroll View verwendet:

- Glass Material
- Clipping
- Themefarben
- optionale Scrollbars

Die Darstellung erfolgt ausschließlich über Theme und Template.

---

# 15. Animationen

Optional unterstützt:

- Smooth Scroll
- Fade Scrollbar
- Overscroll Animation
- Focus Scroll

Animationen werden über das Motion-System ausgeführt.

---

# 16. Accessibility

Die Scroll View besitzt:

- Name
- Rolle
- Beschreibung
- Scrollposition

Screenreader geben zusätzlich an, wenn weiterer Inhalt außerhalb des sichtbaren Bereichs vorhanden ist.

---

# 17. Themeintegration

Das Theme definiert:

- Hintergrund
- Transparenz
- Schatten
- Scrollbarfarben
- Animationen

Alle visuellen Eigenschaften stammen ausschließlich aus dem Theme.

---

# 18. Speicherverwaltung

Scroll Views verwenden:

- gemeinsame Templates
- gemeinsame Styles
- zentrale Viewportobjekte

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Scrollposition
- O(1)-Viewportberechnung
- O(1)-Clipping
- deterministische Laufzeit

Es werden ausschließlich sichtbare Bereiche gerendert.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Scrollposition
- beschädigter Content
- fehlendes Template
- ungültiger Viewport
- Nullpointer

Ungültige Scrollpositionen werden automatisch begrenzt.

---

# 21. Sicherheit

Das System verhindert:

- Scrollpositionen außerhalb des Inhalts
- ungültige Viewports
- rekursive Layoutberechnungen
- beschädigte Child-Referenzen
- inkonsistente Fokuszustände

Das Clipping darf niemals Speichergrenzen überschreiten.

---

# 22. API

```c
typedef struct nova_scroll_view nova_scroll_view_t;

nova_result_t
nova_scroll_view_create(
    nova_scroll_view_t** view);

nova_result_t
nova_scroll_view_set_content(
    nova_scroll_view_t* view,
    nova_control_t* content);

nova_result_t
nova_scroll_view_scroll_to(
    nova_scroll_view_t* view,
    int32_t x,
    int32_t y);

nova_result_t
nova_scroll_view_scroll_into_view(
    nova_scroll_view_t* view,
    nova_control_t* child);

nova_point_t
nova_scroll_view_position(
    const nova_scroll_view_t* view);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- View-ID
- Scrollposition
- Viewportgröße
- Inhaltgröße
- Scrollereignisse
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Vertikales Scrollen
- Horizontales Scrollen
- Clipping
- Focus Scroll
- Touch
- Tastatur
- Controller
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Inhalte korrekt innerhalb des Viewports dargestellt werden.
- Scrollpositionen niemals den gültigen Bereich verlassen.
- fokussierte Elemente automatisch sichtbar werden.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- ausschließlich sichtbare Bereiche gerendert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTCONTROL-SCROLLVIEW-0001 – Scroll View
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Die **Scroll View** definiert den standardisierten Container zur Darstellung von Inhalten, die größer als der verfügbare Anzeigebereich sind. Sie verwaltet Viewport, Clipping und Scrollposition unabhängig vom eigentlichen Inhalt und unterstützt verschiedene Scrollrichtungen, Theme-Integration, Animationen sowie vollständige Accessibility. Durch deterministische Scrolllogik, effizientes Rendering sichtbarer Bereiche und statische Speicherverwaltung bildet sie die Grundlage für komplexe Ansichten innerhalb der NovaOS-Bootoberfläche auf BIOS- und UEFI-Systemen.