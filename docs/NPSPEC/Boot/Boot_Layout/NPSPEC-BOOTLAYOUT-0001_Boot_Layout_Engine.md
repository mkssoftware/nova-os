# NPSPEC-BOOTLAYOUT-0001
# Boot Layout Engine

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0001 |
| Titel | Boot Layout Engine |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0010, NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0020 |
| Zugehörige ADRs | ADR-BOOTLAYOUT-0001, ADR-BOOTLAYOUT-0002, ADR-BOOTLAYOUT-0003, ADR-BOOTDESIGN-0002, ADR-BOOTPERF-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die vollständige **Boot Layout Engine** der NovaOS Boot UI.

Die Layout Engine berechnet sämtliche Positionen und Größen aller Benutzeroberflächenobjekte.

Sie bildet die zentrale Komponente zwischen:

- UI-Objektmodell
- Rendering Engine
- Compositor
- Eingabesystem

Alle sichtbaren Objekte werden ausschließlich über die Layout Engine positioniert.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche grafischen Oberflächen des Bootmanagers.

Sie umfasst:

- Bootmenüs
- Dialoge
- Listen
- Panels
- Buttons
- Textfelder
- Icons
- Statusleisten
- Recovery UI

Nicht Bestandteil sind:

- Rendering
- Compositing
- Animationen

---

# 3. Architektur

```text
UI Tree

↓

Layout Engine

↓

Layout Tree

↓

Render Tree

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Die Layout Engine erzeugt aus der logischen UI-Struktur einen vollständig berechneten Layout Tree.

Jedes Objekt besitzt danach:

- Position
- Größe
- Padding
- Margin
- Alignment
- Clippingbereich

Das Rendering arbeitet ausschließlich auf diesem berechneten Layout.

Constraint-basierte und hierarchische Layoutsysteme werden auch in modernen UI-Frameworks verwendet, um Positionen und Größen automatisch zu berechnen. :contentReference[oaicite:0]{index=0}

---

# 5. Aufgaben

Die Layout Engine übernimmt:

- Größenberechnung
- Positionsberechnung
- automatische Ausrichtung
- Containerlayout
- Padding
- Margin
- Clipping
- Skalierung
- Bildschirmanpassung
- Layoutinvalidierung

---

# 6. Layout Pipeline

```text
UI erstellen

↓

Styles anwenden

↓

Größen berechnen

↓

Positionen berechnen

↓

Clipping erzeugen

↓

Layout Tree erzeugen

↓

Render Tree erzeugen
```

---

# 7. Layoutobjekt

```c
typedef struct
{
    int32_t x;

    int32_t y;

    uint32_t width;

    uint32_t height;

    nova_margin_t margin;

    nova_padding_t padding;

} nova_layout_box_t;
```

---

# 8. Layout Tree

Jedes UI-Element besitzt genau einen Layoutknoten.

Der Layout Tree entspricht der Hierarchie des UI Trees.

---

# 9. Layout Passes

Die Engine arbeitet in drei Durchläufen:

1. Measure Pass
2. Arrange Pass
3. Finalize Pass

---

# 10. Measure Pass

Der erste Durchlauf bestimmt:

- gewünschte Größe
- Minimalgröße
- Maximalgröße
- intrinsische Größe

---

# 11. Arrange Pass

Im zweiten Durchlauf werden berechnet:

- X
- Y
- Breite
- Höhe

aller Layoutobjekte.

---

# 12. Finalize Pass

Abschließend werden erzeugt:

- Clippingbereiche
- Hit-Test-Flächen
- Renderrechtecke
- Dirty Regions

---

# 13. Koordinatensystem

Ursprung:

```text
0 / 0
```

Position:

```text
oben links
```

Alle logischen Koordinaten und Größen verwenden DLU (Device Layout Units).

Physische Pixelkoordinaten entstehen ausschließlich durch die abschließende,
deterministische Umrechnung im Scaling- und Rendering-System.

---

# 14. Größenmodell

Unterstützt werden:

- feste Größe
- automatische Größe
- Prozentwerte
- verfügbare Restfläche
- Inhaltsgröße

---

# 15. Padding

Padding beschreibt den Innenabstand eines Containers.

Padding beeinflusst:

- verfügbare Fläche
- Position der Kindobjekte

---

# 16. Margin

Margin beschreibt den Außenabstand eines Elements.

Margins benachbarter Elemente werden niemals überlagert.

---

# 17. Alignment

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

# 18. Layout Container

Unterstützt werden:

- Stack
- Horizontal
- Vertical
- Grid
- Absolute
- Overlay

---

# 19. Grid Layout

Grid unterstützt:

- Zeilen
- Spalten
- feste Größen
- Auto-Größen
- prozentuale Größen

---

# 20. Overlay Layout

Overlay Container stapeln mehrere Objekte übereinander.

Alle Objekte besitzen dieselbe Grundfläche.

---

# 21. Clipping

Jeder Container kann Clipping aktivieren.

Kindobjekte dürfen den Clippingbereich nicht überschreiten.

---

# 22. Z-Reihenfolge

Die Layout Engine berechnet zusätzlich:

- Zeichenreihenfolge
- Hit-Test-Reihenfolge
- Fokusreihenfolge

---

# 23. Layout Invalidierung

Ein neues Layout wird berechnet wenn:

- Größe geändert wurde
- Inhalt geändert wurde
- Bildschirmauflösung geändert wurde
- Skalierung geändert wurde
- Styles geändert wurden

---

# 24. Skalierung

Die Layout Engine arbeitet vollständig DPI-unabhängig.

Die endgültigen Pixelwerte entstehen erst nach Anwendung des Scaling Systems.

---

# 25. Speicherverwaltung

Alle Layoutobjekte stammen aus:

- festen Objektpools
- festen Layoutlisten

Heap-Allokationen während eines Layoutdurchlaufs sind verboten.

---

# 26. Performance

Anforderungen:

- deterministische Laufzeit
- O(n)-Traversal
- keine Rekursion mit unbeschränkter Tiefe
- keine Heap-Allokationen

---

# 27. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- zyklische Layoutstruktur
- ungültige Größen
- negative Breiten
- ungültige Container
- beschädigte Layoutdaten

Im Fehlerfall wird das betroffene Objekt übersprungen.

---

# 28. Sicherheit

Das System verhindert:

- rekursive Layoutschleifen
- Endlosschleifen
- Speicherüberläufe
- Nullpointer
- beschädigte Layout Trees

---

# 29. API

```c
nova_result_t
nova_layout_initialize(void);

nova_result_t
nova_layout_measure(
    nova_ui_tree_t* tree);

nova_result_t
nova_layout_arrange(
    nova_ui_tree_t* tree);

nova_result_t
nova_layout_finalize(
    nova_ui_tree_t* tree);

void
nova_layout_shutdown(void);
```

---

# 30. Diagnosedaten

Zu erfassen sind:

- Layoutzeit
- Measurezeit
- Arrangezeit
- Finalizezeit
- Layoutobjekte
- Containeranzahl
- Invalidierungen
- Fehleranzahl

---

# 31. Testfälle

Zu testen sind:

- Stack Layout
- Grid Layout
- Overlay Layout
- Alignment
- Padding
- Margin
- Clipping
- Layoutinvalidierung
- Auflösungswechsel
- Recovery

---

# 32. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche UI-Objekte ausschließlich über die Layout Engine positioniert werden.
- drei deterministische Layoutdurchläufe existieren.
- alle Containerarten vollständig unterstützt werden.
- Clipping korrekt funktioniert.
- Layoutinvalidierungen zuverlässig erkannt werden.
- keine Heap-Allokationen während des Layoutdurchlaufs stattfinden.
- BIOS- und UEFI-Systeme identische Layouts erzeugen.

---

# 33. Referenzen

## ADRs

- ADR-BOOTLAYOUT-0001 – Hierarchisches Layoutmodell
- ADR-BOOTLAYOUT-0002 – Drei-Phasen-Layoutberechnung
- ADR-BOOTLAYOUT-0003 – Containerbasiertes Layoutsystem
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens
- ADR-BOOTPERF-0001 – Festes Speicherbudget

## NPSPECs

- NPSPEC-BOOTUI-0006 – Resolution and Scaling System
- NPSPEC-BOOTRENDER-0003 – Scene Graph
- NPSPEC-BOOTRENDER-0004 – Layer and Surface Management
- NPSPEC-BOOTCOMPOSITOR-0002 – Compositor Layer Model

---

# 34. Zusammenfassung

Die **Boot Layout Engine** bildet das zentrale Layoutsystem der NovaOS Bootoberfläche. Sie berechnet in einem deterministischen Drei-Phasen-Verfahren sämtliche Größen, Positionen, Clippingbereiche und Renderrechtecke aller UI-Elemente. Durch containerbasierte Layouts, feste Speicherpools und vollständige Trennung von Layout, Rendering und Compositing entsteht eine skalierbare, performante und hardwareunabhängige Grundlage für die gesamte Bootoberfläche. Moderne Layoutsysteme verwenden ebenfalls hierarchische Layoutbäume und mehrstufige Berechnungen, um Positionen und Größen konsistent aus einer deklarativen UI-Struktur abzuleiten. :contentReference[oaicite:1]{index=1}
