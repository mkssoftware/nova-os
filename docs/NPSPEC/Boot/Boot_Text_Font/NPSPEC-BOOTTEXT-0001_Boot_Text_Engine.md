# NPSPEC-BOOTTEXT-0001
## Boot Text Engine

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTFONT-0001 |

---

# 1. Ziel

Die Boot Text Engine stellt das vollständige Textsystem der NovaOS-Bootoberfläche bereit.

Sie bildet die Grundlage für sämtliche textbasierten Darstellungen innerhalb des Boot Managers und stellt sicher, dass Text unabhängig von Auflösung, Firmware (BIOS/UEFI), Sprache oder Schriftart konsistent dargestellt wird.

Die Engine ist vollständig hardwareunabhängig und arbeitet ausschließlich mit der NovaOS-Rendering-API.

---

# 2. Geltungsbereich

Die Text Engine wird verwendet für:

- Labels
- Buttons
- Listen
- Dialoge
- Menüs
- Statusanzeigen
- Fehlermeldungen
- Recovery
- Installer
- Diagnosesystem
- Bootlogo-Untertitel

---

# 3. Ziele

Die Engine muss:

- Unicode unterstützen
- DPI-unabhängig arbeiten
- deterministisch rendern
- mehrere Schriftarten unterstützen
- mehrere Größen unterstützen
- Themeintegration besitzen
- hohe Renderperformance bieten

---

# 4. Architektur

```text
UI Control

↓

Text Layout

↓

Text Engine

├── Font Manager
├── Glyph Cache
├── Text Shaper
├── Line Break Engine
├── Renderer
└── Metrics Engine

↓

Boot Render Engine

↓

Framebuffer
```

---

# 5. Komponenten

Die Text Engine besteht aus:

- Font Manager
- Glyph Manager
- Glyph Cache
- Text Shaper
- Text Layout Engine
- Text Metrics Engine
- Renderer
- Localization Layer

---

# 6. Grundprinzip

Text wird niemals direkt gerendert.

Der Ablauf lautet:

```text
UTF-8 Text

↓

Unicode Parser

↓

Glyph Lookup

↓

Text Layout

↓

Glyph Positioning

↓

Renderer

↓

Framebuffer
```

---

# 7. Unterstützte Zeichen

Unterstützt werden:

- ASCII
- UTF-8
- Unicode BMP
- Unicode Supplementary Planes (optional)

Beispiele:

- Deutsch
- Englisch
- Französisch
- Japanisch
- Chinesisch
- Arabisch (zukünftig)

---

# 8. Schriftarten

Die Engine unterstützt:

- NovaFont
- Bitmap Fonts
- Vektorfonts (optional)
- Signierte Fontpakete

Alle Schriftarten werden zentral verwaltet.

---

# 9. Schriftgrößen

Unterstützt werden:

- Small
- Normal
- Large
- Title
- Display

Intern erfolgt die Berechnung in Device Independent Units (DLU).

---

# 10. Text Layout

Die Layout Engine unterstützt:

- Zeilenumbruch
- Wortumbruch
- Trunkierung
- Mehrzeilige Texte
- Zentrierung
- Blocksatz (optional)

---

# 11. Textausrichtung

Unterstützt werden:

```text
Links

Zentriert

Rechts
```

Vertikal:

```text
Oben

Mitte

Unten
```

---

# 12. Glyph Cache

Alle Glyphen werden zwischengespeichert.

Eigenschaften:

- LRU Cache
- statischer Speicher
- deterministische Laufzeit

Doppelte Glyphen werden niemals mehrfach gespeichert.

---

# 13. Textmetriken

Berechnet werden:

- Ascender
- Descender
- Baseline
- Line Height
- Glyph Width
- Bounding Box

Alle Controls verwenden dieselben Metriken.

---

# 14. Rendering

Unterstützt werden:

- Alpha Blending
- Antialiasing (optional)
- Subpixel Rendering (optional)
- Monochrom Rendering
- Outline Rendering (optional)

---

# 15. Themeintegration

Das Theme definiert:

- Schriftarten
- Schriftgrößen
- Farben
- Transparenz
- Schatten
- Hervorhebungen

Keine Eigenschaften sind fest codiert.

---

# 16. Internationalisierung

Unterstützt werden:

- Unicode
- UTF-8
- Sprachpakete
- Lokalisierung

Die Text Engine besitzt keinerlei sprachabhängige Logik.

---

# 17. Accessibility

Unterstützt werden:

- große Schrift
- hoher Kontrast
- skalierbare Schriftgrößen
- Screen Reader IDs

Alle Texte besitzen eindeutige Accessibility-IDs.

---

# 18. Sicherheit

Das System verhindert:

- ungültige Unicode-Sequenzen
- beschädigte Glyphen
- fehlerhafte Fonts
- Speicherüberläufe
- rekursive Renderaufrufe

Alle Eingaben werden validiert.

---

# 19. Speicherverwaltung

Die Text Engine verwendet:

- statische Glyph-Caches
- Font-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Glyph Lookup
- O(1)-Glyph Cache
- O(n)-Textlayout
- deterministische Laufzeit

Die Renderzeit muss linear zur Anzahl der Glyphen sein.

---

# 21. API

```c
typedef struct nova_text_layout nova_text_layout_t;

nova_result_t
nova_text_initialize(void);

nova_result_t
nova_text_layout(
    const char* utf8,
    const nova_font_t* font,
    nova_text_layout_t* layout);

nova_result_t
nova_text_render(
    const nova_text_layout_t* layout,
    nova_canvas_t* canvas);

nova_text_metrics_t
nova_text_measure(
    const char* utf8,
    const nova_font_t* font);
```

---

# 22. Diagnosedaten

Zu erfassen sind:

- Font
- Glyph Cache Hit Rate
- Glyph Cache Miss Rate
- Renderzeit
- Layoutzeit
- Unicode-Fehler
- Speicherverbrauch

---

# 23. Testfälle

Zu testen sind:

- ASCII
- UTF-8
- Unicode
- Zeilenumbruch
- Mehrzeilige Texte
- Themewechsel
- Schriftgrößen
- Internationalisierung
- Accessibility
- Fehlerfälle

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Unicode vollständig unterstützt wird.
- alle Schriftarten korrekt dargestellt werden.
- Glyph Caching funktioniert.
- Textlayout deterministisch arbeitet.
- Themeintegration vollständig vorhanden ist.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 25. Referenzen

## ADRs

- ADR-BOOTTEXT-0001 – Boot Text Engine
- ADR-BOOTFONT-0001 – Font Management
- ADR-BOOTRENDER-0001 – Rendering Engine
- ADR-BOOTTHEME-0001 – Theme System

## NPSPECs

- NPSPEC-BOOTRENDER-0001 – Boot Rendering Engine
- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTFONT-0001 – Font Management

---

# 26. Zusammenfassung

Die **Boot Text Engine** stellt das zentrale Textsystem der NovaOS-Bootoberfläche bereit. Sie abstrahiert Schriftarten, Glyphen, Textlayout und Rendering zu einer einheitlichen Engine mit vollständiger Unicode-Unterstützung. Durch deterministische Layout- und Renderalgorithmen, statische Glyph-Caches, zentrale Themeintegration sowie Unterstützung für Internationalisierung und Accessibility bildet sie die Grundlage für sämtliche Textdarstellungen innerhalb des NovaOS Boot Managers auf BIOS- und UEFI-Systemen.