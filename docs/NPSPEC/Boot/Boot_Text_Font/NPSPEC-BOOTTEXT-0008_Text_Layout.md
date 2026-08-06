# NPSPEC-BOOTTEXT-0008
# Text Layout

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0008 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTTEXT-0002, NPSPEC-BOOTTEXT-0003, NPSPEC-BOOTTEXT-0004, NPSPEC-BOOTTEXT-0005, NPSPEC-BOOTTEXT-0006, NPSPEC-BOOTTEXT-0007, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTRENDER-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Text Layout System** der NovaOS-Bootoberfläche.

Das Text Layout System ist verantwortlich für die Anordnung sämtlicher Glyphen innerhalb eines Textblocks. Es berechnet Zeilenumbrüche, Ausrichtung, Kerning, Glyph-Positionen und Bounding Boxes und erzeugt daraus ein vollständiges Layout, das anschließend unverändert vom Renderer dargestellt wird.

Das Layoutsystem arbeitet vollständig unabhängig vom eigentlichen Rendering.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Labels
- Buttons
- Listen
- Menüs
- Dialoge
- Tooltips
- Statusanzeigen
- Recovery
- Installer
- Diagnosemodule

---

# 3. Ziele

Das Layoutsystem muss:

- Unicode vollständig unterstützen
- deterministisch arbeiten
- DPI-unabhängig sein
- mehrere Zeilen unterstützen
- unterschiedliche Ausrichtungen unterstützen
- unabhängig vom Rendering arbeiten

---

# 4. Architektur

```text
UTF-8 Text

↓

Unicode Parser

↓

Text Measurement

↓

Layout Engine

↓

Glyph Positioning

↓

Layout Result

↓

Renderer
```

---

# 5. Komponenten

Das Text Layout System besteht aus:

- Layout Engine
- Line Break Engine
- Word Wrap Engine
- Alignment Engine
- Glyph Positioner
- Bounding Box Generator

---

# 6. Grundprinzip

Der Layoutprozess erfolgt in mehreren Schritten:

```text
UTF-8

↓

Unicode

↓

Glyph Lookup

↓

Measurement

↓

Zeilenbildung

↓

Glyph Positionierung

↓

Layout Ergebnis
```

Der Renderer verändert das Layout niemals.

---

# 7. Layoutobjekt

Ein Layout besteht aus:

- Text
- Glyphen
- Positionen
- Zeilen
- Bounding Box
- Baseline
- Fontinformationen

Das Layoutobjekt ist unveränderlich.

---

# 8. Zeilenumbruch

Unterstützt werden:

- explizite Zeilenumbrüche
- automatischer Wortumbruch
- Zeichenumbruch (optional)

Zeilenumbrüche erfolgen ausschließlich vor dem Rendering.

---

# 9. Ausrichtung

Horizontal:

- Links
- Zentriert
- Rechts

Optional:

- Blocksatz

Vertikal:

- Oben
- Mitte
- Unten

---

# 10. Glyph-Positionierung

Für jede Glyphe werden berechnet:

- X-Position
- Y-Position
- Advance
- Bounding Box

Glyphenpositionen bleiben nach der Berechnung unverändert.

---

# 11. Kerning

Unterstützt werden:

- Font-Kerning
- benutzerdefiniertes Kerning (optional)

Kerning wird vor der endgültigen Positionierung angewendet.

---

# 12. Letter Spacing

Unterstützt werden:

- negativ
- neutral
- positiv

Letter Spacing wird gleichmäßig auf alle Glyphen einer Zeile angewendet.

---

# 13. Mehrzeilige Texte

Jede Zeile besitzt:

- eigene Baseline
- eigene Breite
- eigene Glyphenliste
- eigene Bounding Box

Alle Zeilen bilden gemeinsam den Textblock.

---

# 14. Textbegrenzung

Unterstützt werden:

- maximale Breite
- maximale Höhe
- maximale Zeilenanzahl

Bei Überschreitung:

- Trunkierung
- Ellipsis (`…`)
- Abschneiden (optional)

Das Verhalten wird pro Control festgelegt.

---

# 15. RTL-Unterstützung

Optional unterstützt:

- Right-to-Left
- Bidirectional Algorithm

Standardmäßig erfolgt die Darstellung links nach rechts.

---

# 16. DPI-Skalierung

Das Layout wird ausschließlich in Device Independent Units (DIU) berechnet.

Eine Umrechnung auf physische Pixel erfolgt erst in der Rendering Engine.

---

# 17. Themeintegration

Das Theme definiert:

- Schriftgröße
- Font
- Zeilenhöhe
- Letter Spacing
- Kerning

Das Layoutsystem verarbeitet ausschließlich die resultierenden Werte.

---

# 18. Accessibility

Unterstützt werden:

- große Schriftgrößen
- dynamische Skalierung
- Screen Reader IDs
- Fokus-Markierungen

Accessibility erzeugt bei geänderten Schriftgrößen automatisch ein neues Layout.

---

# 19. Sicherheit

Das System verhindert:

- ungültige Glyphpositionen
- negative Layoutgrößen
- rekursive Layoutberechnungen
- beschädigte Layoutobjekte
- Integerüberläufe

Alle Layoutdaten werden validiert.

---

# 20. Speicherverwaltung

Das Layoutsystem verwendet:

- statische Layoutobjekte
- Glyph-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 21. Performance

Anforderungen:

- O(n)-Layoutberechnung
- O(1)-Glyph Lookup
- O(1)-Font Lookup
- deterministische Laufzeit

Die Berechnungszeit wächst ausschließlich linear mit der Anzahl der Glyphen.

---

# 22. API

```c
typedef struct
{
    nova_rect_t bounds;
    uint32_t glyph_count;
    uint32_t line_count;
}
nova_text_layout_t;

nova_result_t
nova_text_layout_initialize(void);

nova_result_t
nova_text_layout_build(
    const char* utf8,
    const nova_font_t* font,
    const nova_text_constraints_t* constraints,
    nova_text_layout_t* layout);

const nova_glyph_position_t*
nova_text_layout_glyphs(
    const nova_text_layout_t* layout);

nova_rect_t
nova_text_layout_bounds(
    const nova_text_layout_t* layout);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Glyphanzahl
- Zeilenanzahl
- Layoutgröße
- Berechnungszeit
- Font-ID
- Trunkierungen
- Fehler

---

# 24. Testfälle

Zu testen sind:

- ASCII
- UTF-8
- Unicode
- Wortumbruch
- Mehrzeilige Texte
- Ausrichtungen
- Kerning
- Letter Spacing
- RTL (optional)
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Unicode vollständig unterstützt wird.
- Zeilenumbrüche korrekt berechnet werden.
- Glyphen deterministisch positioniert werden.
- alle Ausrichtungen korrekt funktionieren.
- mehrzeilige Texte korrekt aufgebaut werden.
- Trunkierung und Ellipsis korrekt arbeiten.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTTEXT-0008 – Text Layout
- ADR-BOOTTEXT-0001 – Boot Text Engine
- ADR-BOOTLAYOUT-0001 – Layout Engine
- ADR-BOOTRENDER-0001 – Rendering Engine

## NPSPECs

- NPSPEC-BOOTTEXT-0001 – Boot Text Engine
- NPSPEC-BOOTTEXT-0002 – UTF-8 Text Processing
- NPSPEC-BOOTTEXT-0003 – Unicode Character Support
- NPSPEC-BOOTTEXT-0004 – Font Loading
- NPSPEC-BOOTTEXT-0005 – Glyph Rasterization
- NPSPEC-BOOTTEXT-0006 – Glyph Atlas
- NPSPEC-BOOTTEXT-0007 – Text Measurement
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTRENDER-0001 – Boot Rendering Engine

---

# 27. Zusammenfassung

Das **Text Layout System** definiert die standardisierte Anordnung von Glyphen innerhalb der NovaOS-Bootoberfläche. Es verarbeitet Unicode-Texte, berechnet Zeilenumbrüche, Glyphenpositionen, Ausrichtungen und Begrenzungen und erzeugt daraus ein unveränderliches Layoutobjekt für die Rendering Engine. Durch deterministische Algorithmen, vollständige Trennung von Layout und Rendering, DPI-unabhängige Berechnung sowie statische Speicherverwaltung stellt das System eine konsistente und performante Textdarstellung auf BIOS- und UEFI-basierten Systemen sicher.