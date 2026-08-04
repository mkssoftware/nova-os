# NPSPEC-BOOTLAYOUT-0010
# Text-Aware Layout

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTLAYOUT-0010 |
| Titel | Text-Aware Layout |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Layout |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTLAYOUT-0001 bis NPSPEC-BOOTLAYOUT-0009, NPSPEC-BOOTFONT-0001 |
| Zugehörige ADRs | ADR-BOOTLAYOUT-0012, ADR-BOOTFONT-0002, ADR-BOOTACCESS-0003, ADR-BOOTDESIGN-0002, ADR-BOOTI18N-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Text-Aware Layout System** der NovaOS Boot UI.

Das System stellt sicher, dass sämtliche Layoutentscheidungen unter Berücksichtigung des tatsächlich darzustellenden Textes erfolgen.

Es unterstützt:

- intrinsische Textgrößen
- automatische Größenanpassung
- Zeilenumbrüche
- Mehrzeilentext
- Textkürzung
- Baseline-Ausrichtung
- internationale Zeichensätze
- unterschiedliche Schriftgrößen

Dadurch bleibt die Bootoberfläche unabhängig von Sprache, Schriftart und Auflösung jederzeit lesbar und konsistent. Moderne Layoutsysteme berücksichtigen die intrinsische Größe von Text bei der Layoutberechnung, um Bedienelemente automatisch an ihren Inhalt anzupassen. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche textbasierten UI-Elemente.

Sie umfasst:

- Labels
- Buttons
- Listen
- Menüs
- Dialoge
- Statusanzeigen
- Warnmeldungen
- Eingabefelder

Nicht Bestandteil sind:

- Font Rendering
- Glyph Rasterization
- Text Rendering Pipeline

---

# 3. Architektur

```text
Localized Text

↓

Font Engine

↓

Text Measurement

↓

Text Layout

↓

Layout Engine

↓

Rendering
```

---

# 4. Grundprinzip

Kein Layout darf von festen Textlängen ausgehen.

Alle Größen werden anhand des tatsächlich gerenderten Textes berechnet.

---

# 5. Aufgaben

Das Text-Aware Layout übernimmt:

- Textmessung
- intrinsische Größenberechnung
- Zeilenumbruch
- Baseline-Berechnung
- Mindestgrößen
- Textausrichtung
- Kürzung
- Layoutinvalidierung

---

# 6. Intrinsische Textgröße

Jeder Text besitzt:

- Mindestbreite
- Maximalbreite
- Mindesthöhe
- Maximalhöhe

Diese Werte werden vor dem Layoutdurchlauf bestimmt. Das intrinsische Maß beschreibt die natürliche Größe eines Elements ausschließlich anhand seines Inhalts. :contentReference[oaicite:1]{index=1}

---

# 7. Textmessung

Gemessen werden:

- Zeichenbreite
- Zeilenhöhe
- Ascender
- Descender
- Baseline
- Wortabstände

Die Messung erfolgt vor dem Measure Pass.

---

# 8. Zeilenumbruch

Unterstützt werden:

- Word Wrap
- Character Wrap
- No Wrap

Der Modus wird pro Control festgelegt.

---

# 9. Mehrzeilentext

Mehrzeilige Controls passen ihre Höhe automatisch an.

Die Breite bleibt durch das Layout bestimmt.

---

# 10. Textkürzung

Unterstützte Modi:

- None
- End Ellipsis
- Middle Ellipsis
- Start Ellipsis

Standard:

```text
End Ellipsis
```

---

# 11. Mindestgrößen

Jedes textbasierte Control besitzt eine Mindestgröße.

Diese ergibt sich aus:

- Schriftgröße
- Padding
- intrinsischer Textgröße

---

# 12. Baseline

Alle textbasierten Controls können entlang ihrer Baseline ausgerichtet werden.

Dies gewährleistet eine konsistente Typografie.

---

# 13. Lokalisierung

Das Layout muss ohne Änderungen funktionieren für:

- Deutsch
- Englisch
- Französisch
- Spanisch
- Italienisch
- weitere Sprachen

Längere Übersetzungen dürfen keine Überlappungen erzeugen.

---

# 14. Unicode

Unterstützt werden:

- Unicode
- UTF-8
- Mehrbytezeichen
- Emoji (optional)
- RTL-Vorbereitung

Die Layout Engine arbeitet unabhängig vom Zeichensatz.

---

# 15. Schriftgrößen

Textgrößen werden vollständig durch das DPI- und Skalierungssystem angepasst.

Alle Layoutberechnungen erfolgen nach der Schriftmessung.

---

# 16. Icons und Text

Controls mit Icon und Text berechnen automatisch:

- Icongröße
- Textgröße
- Zwischenabstand
- Gesamtgröße

---

# 17. Responsive Layout

Text beeinflusst:

- Dialogbreite
- Listenbreite
- Buttongröße
- Panelgröße

Responsive Regeln werden nach der Textmessung angewendet.

---

# 18. Accessibility

Accessibility kann verändern:

- Schriftgröße
- Zeilenhöhe
- Zeichenabstand
- Mindestgrößen

Das Layout wird anschließend vollständig neu berechnet.

---

# 19. Layout Invalidierung

Eine Neuberechnung erfolgt bei:

- Textänderung
- Sprachwechsel
- Schriftwechsel
- DPI-Wechsel
- Accessibility-Änderung

---

# 20. Speicherverwaltung

Textmetriken werden zwischengespeichert.

Der Cache verwendet feste Speicherpools.

Heap-Allokationen während des Layoutdurchlaufs sind verboten.

---

# 21. Performance

Anforderungen:

- O(n)-Traversal
- einmalige Textmessung pro Layoutdurchlauf
- deterministische Ergebnisse
- keine Heap-Allokationen

Mehrfachmessungen desselben Textes innerhalb eines Layoutdurchlaufs sind zu vermeiden. :contentReference[oaicite:2]{index=2}

---

# 22. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Schrift
- fehlende Glyphen
- beschädigte Unicode-Daten
- ungültige Textmetriken
- Überlauf der Textfläche

Im Fehlerfall wird ein Ersatzfont verwendet.

---

# 23. Sicherheit

Das System verhindert:

- abgeschnittenen Text
- überlappende Texte
- negative Größen
- Nullpointer
- ungültige Baselines

---

# 24. API

```c
typedef struct
{
    uint32_t width;

    uint32_t height;

    int32_t baseline;

} nova_text_metrics_t;

nova_result_t
nova_text_measure(
    const char* text,
    const nova_font_t* font,
    nova_text_metrics_t* metrics);

nova_result_t
nova_layout_apply_text(
    nova_layout_node_t* node);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Textbreiten
- Texthöhen
- Baselines
- Zeilenumbrüche
- Kürzungen
- Layoutinvalidierungen
- Cache-Treffer
- Fehleranzahl

---

# 26. Testfälle

Zu testen sind:

- kurze Texte
- lange Texte
- Mehrzeilentext
- Unicode
- Lokalisierung
- Ellipsis
- Baseline Alignment
- Accessibility
- DPI-Wechsel
- Recovery Mode

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Layoutentscheidungen auf gemessenen Textmetriken basieren.
- lange Texte keine Überlappungen erzeugen.
- Mehrzeilentext automatisch unterstützt wird.
- Unicode und Lokalisierung vollständig funktionieren.
- Accessibility-Anpassungen korrekt berücksichtigt werden.
- keine Heap-Allokationen während der Layoutberechnung stattfinden.
- BIOS- und UEFI-Systeme identische Layoutergebnisse erzeugen.

---

# 28. Referenzen

## ADRs

- ADR-BOOTLAYOUT-0012 – Textabhängige Layoutberechnung
- ADR-BOOTFONT-0002 – Einheitliche Fontmetriken
- ADR-BOOTACCESS-0003 – Skalierbare Schrift- und Bedienelementgrößen
- ADR-BOOTDESIGN-0002 – Gemeinsame Design-Tokens
- ADR-BOOTI18N-0001 – Unicode und Internationalisierung

## NPSPECs

- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTLAYOUT-0006 – Constraint-Based Sizing
- NPSPEC-BOOTLAYOUT-0009 – DPI and Resolution Scaling
- NPSPEC-BOOTFONT-0001 – Boot Font System
- NPSPEC-ACCESSIBILITY-0001 – Accessibility Framework

---

# 29. Zusammenfassung

Das **Text-Aware Layout** sorgt dafür, dass sämtliche Layoutentscheidungen der NovaOS Bootoberfläche auf den tatsächlichen Eigenschaften des darzustellenden Textes basieren. Intrinsische Textgrößen, automatische Zeilenumbrüche, Baseline-Ausrichtung, Unicode-Unterstützung und DPI-Skalierung werden vollständig in die Layoutberechnung integriert. Dadurch entstehen sprachunabhängige, robuste und konsistente Benutzeroberflächen, die auf BIOS- und UEFI-Systemen identisch funktionieren. Die Verwendung intrinsischer Textmetriken und inhaltsabhängiger Größen entspricht den Prinzipien moderner Layoutsysteme. :contentReference[oaicite:3]{index=3}