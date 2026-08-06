# NPSPEC-BOOTTEXT-0007
# Text Measurement

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0007 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTTEXT-0004, NPSPEC-BOOTTEXT-0005, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTRENDER-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Text Measurement System** der NovaOS-Bootoberfläche.

Das Text Measurement System berechnet sämtliche geometrischen Eigenschaften eines Textes, bevor dieser gerendert wird. Es liefert exakte Größeninformationen für das Layoutsystem und ermöglicht eine konsistente Platzierung von Texten unabhängig von Schriftart, Sprache oder Bildschirmauflösung.

Das System führt keine Darstellung durch und erzeugt ausschließlich Messdaten.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Labels
- Buttons
- Menüs
- Listen
- Dialoge
- Tooltips
- Statusanzeigen
- Recovery
- Installer
- Diagnosemodule

---

# 3. Ziele

Das Text Measurement System muss:

- deterministisch arbeiten
- Unicode vollständig unterstützen
- DPI-unabhängig sein
- Font-Metriken verwenden
- mehrzeilige Texte berechnen
- hohe Performance bieten

---

# 4. Architektur

```text
UTF-8 Text

↓

Unicode Parser

↓

Font Metrics

↓

Glyph Metrics

↓

Layout Calculator

↓

Measurement Result

↓

Layout Engine
```

---

# 5. Komponenten

Das Text Measurement System besteht aus:

- Text Metrics Engine
- Font Metrics Manager
- Glyph Metrics Resolver
- Line Measurement Engine
- Bounding Box Calculator
- Baseline Calculator

---

# 6. Grundprinzip

Der Ablauf erfolgt wie folgt:

```text
UTF-8 Text

↓

Unicode

↓

Glyph Lookup

↓

Glyph Metrics

↓

Text Measurement

↓

Layout
```

Es werden ausschließlich Messdaten erzeugt.

---

# 7. Messgrößen

Für jeden Text werden berechnet:

- Breite
- Höhe
- Baseline
- Ascender
- Descender
- Line Height
- Bounding Box

---

# 8. Fontmetriken

Verwendet werden:

- Ascender
- Descender
- Cap Height
- X Height
- Line Gap
- Advance Width

Alle Werte stammen ausschließlich aus dem Font.

---

# 9. Glyphmetriken

Für jede Glyphe werden bestimmt:

- Breite
- Höhe
- Bearing X
- Bearing Y
- Advance
- Bounding Box

Glyphmetriken werden aus dem Glyph Cache übernommen.

---

# 10. Textbreite

Die Gesamtbreite ergibt sich aus:

```text
Summe aller Glyph Advances

+

Kerning

+

Letter Spacing
```

Es erfolgt keine nachträgliche Skalierung.

---

# 11. Texthöhe

Die Höhe ergibt sich aus:

- Ascender
- Descender
- Line Gap
- Anzahl der Zeilen

Die Berechnung erfolgt vollständig vor dem Rendering.

---

# 12. Mehrzeilige Texte

Unterstützt werden:

- feste Zeilenumbrüche
- automatischer Wortumbruch
- begrenzte Zeilenanzahl
- Trunkierung

Für jede Zeile werden eigene Messdaten erzeugt.

---

# 13. Kerning

Unterstützt werden:

- Font Kerning
- benutzerdefiniertes Kerning (optional)

Kerning wird vor der Breitenberechnung angewendet.

---

# 14. Letter Spacing

Unterstützt werden:

- negativ
- neutral
- positiv

Letter Spacing wird gleichmäßig auf alle Glyphen angewendet.

---

# 15. Bounding Box

Für jeden Text wird berechnet:

```text
X

Y

Breite

Höhe
```

Die Bounding Box dient ausschließlich dem Layout.

---

# 16. DPI-Skalierung

Alle Messwerte werden intern in Device Independent Units (DIU) berechnet.

Die Umrechnung auf physische Pixel erfolgt erst während des Renderings.

---

# 17. Themeintegration

Das Theme beeinflusst:

- Schriftgröße
- Fontfamilie
- Letter Spacing
- Zeilenhöhe

Die Messengine verarbeitet ausschließlich die resultierenden Werte.

---

# 18. Accessibility

Unterstützt werden:

- große Schriftgrößen
- dynamische Skalierung
- hoher Kontrast

Vergrößerte Schriftgrößen erzeugen automatisch neue Messdaten.

---

# 19. Sicherheit

Das System verhindert:

- ungültige Fontmetriken
- negative Größen
- beschädigte Glyphdaten
- Integerüberläufe
- rekursive Layoutberechnungen

Alle Eingabedaten werden validiert.

---

# 20. Speicherverwaltung

Das Text Measurement System verwendet:

- statische Metriktabellen
- Glyph-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 21. Performance

Anforderungen:

- O(n)-Textmessung
- O(1)-Glyph Lookup
- O(1)-Font Lookup
- deterministische Laufzeit

Die Laufzeit wächst ausschließlich linear mit der Anzahl der Glyphen.

---

# 22. API

```c
typedef struct
{
    int32_t width;
    int32_t height;
    int32_t baseline;
    int32_t ascender;
    int32_t descender;
}
nova_text_metrics_t;

nova_result_t
nova_text_measure_initialize(void);

nova_result_t
nova_text_measure(
    const char* utf8,
    const nova_font_t* font,
    nova_text_metrics_t* metrics);

int32_t
nova_text_width(
    const char* utf8,
    const nova_font_t* font);

int32_t
nova_text_height(
    const char* utf8,
    const nova_font_t* font);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Textlänge
- Glyphanzahl
- Breite
- Höhe
- Baseline
- Berechnungszeit
- Font-ID
- Fehler

---

# 24. Testfälle

Zu testen sind:

- ASCII
- UTF-8
- Unicode
- Mehrzeilige Texte
- Kerning
- Letter Spacing
- verschiedene Schriftgrößen
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Textgrößen korrekt berechnet werden.
- Unicode vollständig unterstützt wird.
- Fontmetriken korrekt verwendet werden.
- Kerning und Letter Spacing berücksichtigt werden.
- mehrzeilige Texte korrekt vermessen werden.
- Bounding Boxes deterministisch berechnet werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTTEXT-0007 – Text Measurement
- ADR-BOOTTEXT-0001 – Boot Text Engine
- ADR-BOOTFONT-0001 – Font Management
- ADR-BOOTLAYOUT-0001 – Layout Engine

## NPSPECs

- NPSPEC-BOOTTEXT-0001 – Boot Text Engine
- NPSPEC-BOOTTEXT-0004 – Font Loading
- NPSPEC-BOOTTEXT-0005 – Glyph Rasterization
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTRENDER-0001 – Boot Rendering Engine

---

# 27. Zusammenfassung

Das **Text Measurement System** definiert die standardisierte Vermessung sämtlicher Texte innerhalb der NovaOS-Bootoberfläche. Es berechnet auf Basis von Unicode, Font- und Glyphmetriken alle für das Layout benötigten Größen wie Breite, Höhe, Baseline und Bounding Box, bevor eine Darstellung erfolgt. Durch deterministische Algorithmen, DPI-unabhängige Maßeinheiten, vollständige Unicode-Unterstützung und statische Speicherverwaltung stellt das System eine präzise und performante Grundlage für die Positionierung aller Textinhalte auf BIOS- und UEFI-basierten Systemen bereit.