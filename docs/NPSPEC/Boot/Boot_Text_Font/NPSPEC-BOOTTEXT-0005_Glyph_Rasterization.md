# NPSPEC-BOOTTEXT-0005
# Glyph Rasterization

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0005 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTTEXT-0004, NPSPEC-BOOTFONT-0001, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTRENDER-0016 |

---

# 1. Ziel

Diese Spezifikation definiert das **Glyph Rasterization System** der NovaOS-Bootoberfläche.

Das Glyph Rasterization System wandelt skalierbare oder bitmapbasierte Glyphen in Pixelinformationen um, die anschließend von der Rendering Engine auf den Framebuffer gezeichnet werden.

Die Rasterisierung erfolgt deterministisch und unabhängig von BIOS-, UEFI- oder Grafikhardware.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- sämtliche UI-Texte
- Labels
- Buttons
- Listen
- Dialoge
- Menüs
- Statusanzeigen
- Recovery
- Installer
- Diagnosemodule

---

# 3. Ziele

Das Rasterisierungssystem muss:

- deterministisch arbeiten
- hohe Darstellungsqualität liefern
- verschiedene Fonttypen unterstützen
- Glyph-Caching ermöglichen
- DPI-unabhängig arbeiten
- reproduzierbare Ergebnisse erzeugen

---

# 4. Architektur

```text
Font

↓

Glyph Resolver

↓

Glyph Rasterizer

↓

Bitmap Cache

↓

Renderer

↓

Framebuffer
```

---

# 5. Komponenten

Das Rasterisierungssystem besteht aus:

- Glyph Loader
- Outline Rasterizer
- Bitmap Rasterizer
- Hinting Engine
- Bitmap Cache
- Alpha Generator

---

# 6. Grundprinzip

Der Ablauf erfolgt in mehreren Schritten:

```text
Unicode

↓

Glyph Lookup

↓

Glyph Outline

↓

Rasterisierung

↓

Alpha Bitmap

↓

Glyph Cache

↓

Rendering
```

Jede Glyphe wird nur einmal rasterisiert und anschließend zwischengespeichert.

---

# 7. Unterstützte Fonttypen

Unterstützt werden:

- Nova Font Package
- Bitmap Fonts

Optional:

- TrueType
- OpenType

Die Rasterisierung erfolgt unabhängig vom ursprünglichen Fontformat.

---

# 8. Rasterisierung

Die Engine unterstützt:

- Graustufenrasterisierung
- Monochromrasterisierung

Optional:

- Subpixelrasterisierung
- Signed Distance Fields (SDF)

Die gewählte Methode wird systemweit konfiguriert.

---

# 9. Hinting

Optional unterstützt:

- Font Hinting
- Auto Hinting

Ist kein Hinting verfügbar, erfolgt eine geometrische Rasterisierung.

---

# 10. Alpha-Erzeugung

Die Rasterisierung erzeugt Alpha-Masken.

Unterstützt werden:

```text
1 Bit

8 Bit Alpha
```

8-Bit-Alpha ist der Standard.

---

# 11. Glyph Cache

Rasterisierte Glyphen werden im Cache gespeichert.

Eigenschaften:

- gemeinsame Nutzung
- statischer Speicher
- LRU-Verwaltung
- deterministische Zugriffe

Eine Glyphe darf pro Größe nur einmal im Cache existieren.

---

# 12. Skalierung

Glyphen werden ausschließlich vor der Rasterisierung skaliert.

Nachträgliches Bitmap-Stretching ist nicht zulässig.

---

# 13. Qualitätsstufen

Unterstützt werden:

- Low
- Standard
- High

Die Qualitätsstufe beeinflusst ausschließlich den Rasterisierungsalgorithmus.

---

# 14. Themeintegration

Das Theme definiert:

- Schriftgröße
- Kantenglättung
- Renderqualität

Das Theme beeinflusst nicht die Fontdaten.

---

# 15. Fehlerbehandlung

Kann eine Glyphe nicht rasterisiert werden:

```text
Missing Glyph

↓

Fallback Font

↓

Replacement Glyph
```

Die Textdarstellung wird niemals vollständig abgebrochen.

---

# 16. Sicherheit

Das System verhindert:

- ungültige Glyphdaten
- beschädigte Fonttabellen
- Speicherüberläufe
- rekursive Rasterisierung
- doppelte Cacheeinträge

Alle Glyphen werden vor der Rasterisierung validiert.

---

# 17. Speicherverwaltung

Das Rasterisierungssystem verwendet:

- statische Glyph-Caches
- Bitmap-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Glyph Lookup
- O(1)-Cache Lookup
- O(n)-Rasterisierung
- deterministische Laufzeit

Bereits rasterisierte Glyphen dürfen niemals erneut berechnet werden.

---

# 19. API

```c
typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t* alpha_bitmap;
}
nova_glyph_bitmap_t;

nova_result_t
nova_glyph_rasterizer_initialize(void);

nova_result_t
nova_glyph_rasterize(
    const nova_font_t* font,
    uint32_t codepoint,
    uint32_t size,
    nova_glyph_bitmap_t* bitmap);

const nova_glyph_bitmap_t*
nova_glyph_cache_lookup(
    const nova_font_t* font,
    uint32_t codepoint,
    uint32_t size);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Font-ID
- Glyph-ID
- Rasterisierungszeit
- Cache Hits
- Cache Misses
- Speicherverbrauch
- Fehleranzahl

---

# 21. Testfälle

Zu testen sind:

- Bitmap Fonts
- Nova Fonts
- unterschiedliche Schriftgrößen
- Graustufenrasterisierung
- Monochromrasterisierung
- Glyph Cache
- Fallback Fonts
- Themewechsel
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Glyphen korrekt rasterisiert werden.
- Bitmap- und Outline-Fonts unterstützt werden.
- Glyph Caching zuverlässig funktioniert.
- Rasterisierung deterministisch erfolgt.
- Fallback-Glyphen korrekt verwendet werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTTEXT-0005 – Glyph Rasterization
- ADR-BOOTFONT-0001 – Font Management
- ADR-BOOTRENDER-0001 – Rendering Engine
- ADR-BOOTPERF-0001 – Performance Budget

## NPSPECs

- NPSPEC-BOOTTEXT-0001 – Boot Text Engine
- NPSPEC-BOOTTEXT-0004 – Font Loading
- NPSPEC-BOOTFONT-0001 – Font Management
- NPSPEC-BOOTRENDER-0001 – Boot Rendering Engine
- NPSPEC-BOOTRENDER-0016 – Image Rendering

---

# 24. Zusammenfassung

Das **Glyph Rasterization System** definiert die standardisierte Umwandlung von Glyphen in Pixelinformationen innerhalb der NovaOS-Bootoberfläche. Es unterstützt Bitmap- und skalierbare Schriftarten, erzeugt hochwertige Alpha-Bitmaps, integriert Glyph-Caching sowie Font-Fallbacks und arbeitet vollständig unabhängig von Firmware und Grafikhardware. Durch deterministische Rasterisierung, statische Speicherverwaltung und zentrale Qualitätssteuerung bildet das System die Grundlage für eine performante und konsistente Textdarstellung auf BIOS- und UEFI-basierten Systemen.