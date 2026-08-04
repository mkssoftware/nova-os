# NPSPEC-BOOTLAYOUT-0005
# Overlay Layout

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0005 |
| Titel | Overlay Layout |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0004, NPSPEC-BOOTCOMPOSITOR-0003 bis NPSPEC-BOOTCOMPOSITOR-0008 |
| Zugehörige ADRs | ADR-BOOTLAYOUT-0006, ADR-BOOTCOMPOSITOR-0002, ADR-BOOTCOMPOSITOR-0003, ADR-BOOTDIALOG-0001, ADR-BOOTDESIGN-0003 |

---

# 1. Ziel

Diese Spezifikation definiert das **Overlay Layout** der NovaOS Boot UI.

Das Overlay Layout dient zur Darstellung mehrerer UI-Elemente innerhalb derselben Grundfläche.

Es wird verwendet für:

- Dialoge
- Overlays
- Glass Layer
- Acrylic Layer
- Backdrop
- Popups
- Kontextfenster
- Ladeanzeigen
- Recovery-Warnungen

Alle Elemente werden übereinander angeordnet und unterscheiden sich ausschließlich durch ihre Z-Reihenfolge.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Overlay-Container.

Sie umfasst:

- Modale Dialoge
- Nichtmodale Overlays
- Benachrichtigungen
- Glass-Ebenen
- Schattenebenen
- Ladebildschirme
- Recovery-Dialoge

Nicht Bestandteil sind:

- Stack Layout
- Grid Layout
- Absolute Layout

---

# 3. Architektur

```text
Overlay Container

↓

Layout Engine

↓

Overlay Layer

↓

Z-Order

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Alle Kindobjekte teilen sich dieselbe Containerfläche.

Die Darstellung erfolgt ausschließlich über:

- Position
- Z-Index
- Sichtbarkeit
- Transparenz

Die Reihenfolge der Darstellung wird nicht durch die Einfügereihenfolge, sondern durch die Layer-Priorität bestimmt.

---

# 5. Aufgaben

Das Overlay Layout übernimmt:

- Überlagerung mehrerer Elemente
- Layerverwaltung
- Z-Reihenfolge
- Alignment
- Clipping
- Sichtbarkeit
- Fokusreihenfolge

---

# 6. Overlayfläche

Alle Elemente erhalten standardmäßig die vollständige Containergröße.

Die tatsächliche Größe kann anschließend durch Alignment oder feste Größen angepasst werden.

---

# 7. Z-Reihenfolge

Jedes Element besitzt:

- Z-Index
- Layer-Priorität

Die Darstellung erfolgt aufsteigend nach Layer-Priorität und anschließend nach Z-Index.

---

# 8. Layerarten

Unterstützt werden:

- Background
- Content
- Overlay
- Dialog
- Popup
- Tooltip
- Debug

Die Layerdefinition erfolgt zentral über den Compositor.

---

# 9. Positionierung

Ein Overlay kann:

- absolut positioniert
- zentriert
- an Kanten ausgerichtet
- gestreckt

werden.

---

# 10. Alignment

Unterstützt werden:

Horizontal:

- Left
- Center
- Right
- Stretch

Vertikal:

- Top
- Center
- Bottom
- Stretch

---

# 11. Padding

Der Overlay-Container besitzt optional Padding.

Dieses beeinflusst ausschließlich den nutzbaren Innenbereich.

---

# 12. Margin

Overlay-Elemente dürfen Margin verwenden.

Margins beeinflussen nicht die Größe des Containers.

---

# 13. Sichtbarkeit

Unterstützte Zustände:

- Visible
- Hidden
- Collapsed

**Hidden**

- bleibt Bestandteil des Layouts.

**Collapsed**

- wird vollständig aus der Layoutberechnung ausgeschlossen.

---

# 14. Modale Overlays

Modale Overlays:

- blockieren Eingaben
- besitzen höchste Fokuspriorität
- verdecken darunterliegende Bedienelemente

Der Hintergrund bleibt sichtbar, jedoch nicht interaktiv.

---

# 15. Nichtmodale Overlays

Nichtmodale Overlays:

- blockieren keine Eingaben
- besitzen geringere Priorität
- dürfen parallel existieren

---

# 16. Clipping

Jeder Overlay-Container kann Clipping aktivieren.

Kindobjekte dürfen den Clippingbereich nicht überschreiten.

---

# 17. Glass und Acrylic

Overlay Layout unterstützt nativ:

- Glass Layer
- Acrylic Layer
- Blurflächen
- Schattenflächen

Die Materialberechnung erfolgt anschließend im Compositor.

---

# 18. Layout Invalidierung

Eine Neuberechnung erfolgt bei:

- Größenänderung
- Layeränderung
- Sichtbarkeitsänderung
- Z-Index-Änderung
- Skalierungsänderung

---

# 19. Accessibility

Accessibility beeinflusst:

- Mindestabstände
- Fokusgrößen
- Touchflächen
- Dialogpositionierung

Die Layerreihenfolge bleibt unverändert.

---

# 20. Skalierung

Alle Positionen werden abschließend durch das Resolution and Scaling System angepasst.

Overlay-Layouts bleiben dadurch auf allen Bildschirmauflösungen konsistent.

---

# 21. Speicherverwaltung

Jeder Overlay-Container besitzt:

- feste Layerliste
- feste Objektliste
- Layoutinformationen

Heap-Allokationen während des Layoutdurchlaufs sind verboten.

---

# 22. Performance

Anforderungen:

- deterministische Berechnung
- O(n)-Traversal
- keine rekursiven Layerberechnungen
- keine Heap-Allokationen

---

# 23. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- doppelte Layerzuweisung
- ungültiger Z-Index
- beschädigte Layerliste
- ungültige Overlayfläche
- fehlerhafte Sichtbarkeitszustände

Im Fehlerfall wird das betroffene Overlay nicht dargestellt.

---

# 24. Sicherheit

Das System verhindert:

- mehrere aktive modale Ebenen mit gleicher Priorität
- rekursive Overlaystrukturen
- Nullpointer
- Speicherüberläufe
- ungültige Layerhierarchien

---

# 25. API

```c
typedef enum
{
    NOVA_OVERLAY_BACKGROUND,
    NOVA_OVERLAY_CONTENT,
    NOVA_OVERLAY_DIALOG,
    NOVA_OVERLAY_POPUP,
    NOVA_OVERLAY_TOOLTIP,
    NOVA_OVERLAY_DEBUG
}
nova_overlay_layer_t;

nova_result_t
nova_overlay_measure(
    nova_overlay_t* overlay);

nova_result_t
nova_overlay_arrange(
    nova_overlay_t* overlay);

nova_result_t
nova_overlay_update(
    nova_overlay_t* overlay);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- Anzahl Overlayobjekte
- Layeranzahl
- Z-Index-Verteilung
- Layoutzeit
- Clippingbereiche
- Sichtbarkeitswechsel
- Invalidierungen
- Fehleranzahl

---

# 27. Testfälle

Zu testen sind:

- modale Dialoge
- nichtmodale Overlays
- Glass Layer
- Acrylic Layer
- Z-Index
- Alignment
- Clipping
- Sichtbarkeitswechsel
- Accessibility
- Auflösungswechsel

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- mehrere Overlay-Ebenen gleichzeitig unterstützt werden.
- Z-Index und Layer-Priorität korrekt ausgewertet werden.
- modale Overlays Eingaben zuverlässig blockieren.
- Hidden- und Collapsed-Zustände korrekt verarbeitet werden.
- Glass- und Acrylic-Layer vollständig unterstützt werden.
- keine Heap-Allokationen während des Layoutdurchlaufs stattfinden.
- BIOS- und UEFI-Systeme identische Overlay-Layouts erzeugen.

---

# 29. Referenzen

## ADRs

- ADR-BOOTLAYOUT-0006 – Overlay-basierte Oberflächenarchitektur
- ADR-BOOTCOMPOSITOR-0002 – Einheitliches Layer-Modell
- ADR-BOOTCOMPOSITOR-0003 – Overlay Layer
- ADR-BOOTDIALOG-0001 – Modale Overlay-Dialoge
- ADR-BOOTDESIGN-0003 – Adaptives Glass-Design

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTCOMPOSITOR-0002 – Compositor Layer Model
- NPSPEC-BOOTCOMPOSITOR-0003 – Overlay Layer
- NPSPEC-BOOTCOMPOSITOR-0004 – Modal Layer
- NPSPEC-BOOTCOMPOSITOR-0005 – Backdrop Processing
- NPSPEC-BOOTCOMPOSITOR-0006 – Glass Material
- NPSPEC-BOOTUI-0006 – Resolution and Scaling System

---

# 30. Zusammenfassung

Das **Overlay Layout** stellt die Grundlage für sämtliche übereinanderliegende Benutzeroberflächen der NovaOS Boot UI dar. Es ermöglicht die strukturierte Verwaltung von Dialogen, Overlays, Glass- und Acrylic-Flächen sowie modalen und nichtmodalen Ebenen innerhalb einer gemeinsamen Containerfläche. Durch deterministische Layerverwaltung, feste Speicherstrukturen und vollständige Integration mit dem Compositor entsteht ein konsistentes und performantes Overlay-System für BIOS- und UEFI-basierte Bootoberflächen.