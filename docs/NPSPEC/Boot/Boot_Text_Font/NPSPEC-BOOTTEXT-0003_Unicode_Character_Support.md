# NPSPEC-BOOTTEXT-0003
# Unicode Character Support

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0003 |
| Titel | Unicode Character Support |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Text System |
| Priorität | Hoch |
| Gültigkeitsbereich | Gesamte NovaOS Bootplattform |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTTEXT-0002, NPSPEC-BOOTRESOURCE-0009, NPSPEC-BOOTRENDER-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die Unicode-Unterstützung der NovaOS-Bootplattform.

Das Textsystem muss internationale Zeichen konsistent darstellen können und dabei eine deterministische Verarbeitung aller unterstützten Unicode-Zeichen gewährleisten. Ziel ist eine vollständige Internationalisierung der Bootoberfläche ohne sprachabhängige Sonderimplementierungen.

---

# 2. Geltungsbereich

Die Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Diagnoseoberflächen
- Passwortdialoge
- Fehlermeldungen
- Einstellungen
- Hilfe
- sämtliche zukünftigen Bootoberflächen

---

# 3. Ziele

Das Textsystem muss:

- Unicode nativ unterstützen
- UTF-8 als Standard verwenden
- UTF-16 optional unterstützen
- deterministisch arbeiten
- sprachunabhängig sein
- zukünftige Unicode-Versionen unterstützen

---

# 4. Architektur

Das Unicode-System besteht aus:

- UTF Decoder
- Unicode Parser
- Glyph Resolver
- Font Manager
- Fallback Engine
- Text Renderer

---

# 5. Zeichenkodierung

Primär unterstützt:

- UTF-8

Optional:

- UTF-16

Nicht unterstützt:

- UTF-7
- ANSI
- ASCII-spezifische Sonderpfade

ASCII ist vollständig als Teilmenge von UTF-8 enthalten.

---

# 6. Unterstützte Unicode-Bereiche

Mindestens folgende Unicode-Blöcke müssen unterstützt werden:

- Basic Latin
- Latin-1 Supplement
- Latin Extended
- Greek
- Cyrillic
- Hebrew
- Arabic
- Thai
- Georgian
- Armenian
- Hiragana
- Katakana
- Hangul
- CJK Unified Ideographs
- General Punctuation
- Mathematical Symbols
- Currency Symbols
- Arrows
- Box Drawing
- Block Elements
- Geometric Shapes
- Dingbats

Weitere Blöcke können ergänzt werden.

---

# 7. Zeichenverarbeitung

Der Parser verarbeitet:

- Unicode Code Points
- Mehrbytezeichen
- Surrogate (UTF-16)
- Combining Characters
- Variation Selectors

Ungültige Sequenzen werden erkannt.

---

# 8. Glyphauflösung

Für jedes Zeichen erfolgt:

```text
Unicode

↓

Glyph Lookup

↓

Font

↓

Glyph Atlas

↓

Renderer
```

Fehlende Glyphen lösen den Font-Fallback aus.

---

# 9. Kombinationen

Unterstützt werden:

- Combining Marks
- Akzentzeichen
- Ligaturen (optional)
- Diakritische Zeichen

Die visuelle Reihenfolge entspricht den Unicode-Regeln.

---

# 10. Schreibrichtungen

Unterstützt werden:

- Left-to-Right (LTR)
- Right-to-Left (RTL)

Komplexe bidirektionale Textlayouts können in einer späteren Version ergänzt werden.

---

# 11. Zeilenumbruch

Unterstützt werden:

- Wortumbruch
- Zeichenumbruch
- Ellipsis
- automatische Zeilenhöhe

Zeilenumbrüche folgen den definierten Layoutregeln der Bootoberfläche.

---

# 12. Ersatzzeichen

Ungültige Zeichen werden dargestellt durch:

```text
U+FFFD
```

Das Ersatzzeichen muss in allen Standardfonts vorhanden sein.

---

# 13. Font-Fallback

Falls eine Glyphe fehlt:

```text
Primärer Font

↓

Fallback Font

↓

Recovery Font

↓

Missing Glyph Symbol
```

Der Fallback erfolgt automatisch.

---

# 14. Speicherverwaltung

Das Unicode-System verwendet:

- Glyph Cache
- Unicode Lookup Table
- UTF Decoder Buffer
- Font Cache

Mehrfache Dekodierungen identischer Zeichenfolgen sollen vermieden werden.

---

# 15. Performance

Das System muss:

- UTF-8 effizient dekodieren
- Glyphen cachen
- redundante Lookups vermeiden
- Lazy Loading von Glyphen unterstützen

---

# 16. Fehlerbehandlung

Zu erkennen sind:

- ungültige UTF-8-Sequenzen
- ungültige UTF-16-Sequenzen
- fehlende Glyphen
- beschädigte Fonts
- ungültige Code Points

---

# 17. Sicherheit

Das Unicode-System muss:

- Puffergrenzen prüfen
- ungültige Sequenzen sicher behandeln
- Integerüberläufe verhindern
- keine unkontrollierten Speicherzugriffe verursachen

---

# 18. Diagnostik

Bereitgestellt werden:

- dekodierte Zeichen
- unbekannte Zeichen
- Fallback-Nutzung
- Fontwechsel
- Dekodierungsfehler
- Cache-Statistiken

---

# 19. API

```c
typedef struct
{
    uint32_t codepoint;
    uint32_t glyph_id;
    bool fallback_used;
}
nova_unicode_glyph_t;

nova_result_t
nova_unicode_initialize(void);

nova_result_t
nova_unicode_decode_utf8(
    const char* utf8,
    uint32_t* codepoint);

nova_result_t
nova_unicode_lookup(
    uint32_t codepoint,
    nova_unicode_glyph_t* glyph);

bool
nova_unicode_is_supported(
    uint32_t codepoint);
```

---

# 20. Diagnosedaten

Zu protokollieren sind:

- Unicode-Codepoint
- Glyph-ID
- Font-ID
- Fallbackstatus
- Dekodierungsfehler
- Cache-Hits
- Cache-Misses
- Renderdauer

---

# 21. Testfälle

Zu testen sind:

- ASCII
- UTF-8
- UTF-16
- Mehrbytezeichen
- CJK
- Arabisch
- Kyrillisch
- Griechisch
- Emoji-Platzhalter
- ungültige UTF-Sequenzen

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- UTF-8 vollständig unterstützt wird.
- alle definierten Unicode-Blöcke korrekt verarbeitet werden.
- fehlende Glyphen automatisch über Font-Fallback behandelt werden.
- ungültige Sequenzen sicher erkannt werden.
- Ersatzzeichen korrekt dargestellt werden.
- sämtliche Diagnosedaten über die API verfügbar sind.
- Unicode-Dekodierung deterministisch erfolgt.
- zukünftige Unicode-Versionen ohne Architekturänderungen integrierbar sind.

---

# 23. Referenzen

## ADRs

- ADR-BOOTTEXT-0003 – Unicode Character Support

## NPSPECs

- NPSPEC-BOOTTEXT-0001 – Text Rendering Engine
- NPSPEC-BOOTTEXT-0002 – Font Rendering
- NPSPEC-BOOTRESOURCE-0009 – Font Resources
- NPSPEC-BOOTRENDER-0001 – Rendering Engine

---

# 24. Zusammenfassung

Die **Unicode Character Support**-Spezifikation definiert die vollständige Unterstützung internationaler Zeichen innerhalb der NovaOS-Bootplattform. Sie legt UTF-8 als primäre Zeichenkodierung fest, beschreibt die Verarbeitung von Unicode-Codepoints, Glyphenauflösung, Font-Fallback und Fehlerbehandlung und schafft damit die Grundlage für eine sprachunabhängige, deterministische und weltweit einsetzbare Bootoberfläche.