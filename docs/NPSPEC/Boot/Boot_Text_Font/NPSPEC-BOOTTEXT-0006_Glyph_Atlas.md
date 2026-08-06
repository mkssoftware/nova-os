# NPSPEC-BOOTTEXT-0006
# Glyph Atlas

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0006 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTTEXT-0004, NPSPEC-BOOTTEXT-0005, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTMEMORY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Glyph Atlas System** der NovaOS-Bootoberfläche.

Der Glyph Atlas dient als zentrale, gemeinsam genutzte Speicherstruktur für bereits rasterisierte Glyphen. Ziel ist es, dieselbe Glyphe unabhängig von ihrer späteren Verwendung nur einmal im Speicher abzulegen und anschließend mehrfach zu rendern.

Der Glyph Atlas reduziert Speicherverbrauch, minimiert Rasterisierungsvorgänge und verbessert die Renderleistung des gesamten Boot Managers.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Diagnosemodule
- sämtliche Text-Controls
- Theme-System

---

# 3. Ziele

Das Glyph-Atlas-System muss:

- Glyphen zentral verwalten
- Mehrfachverwendung ermöglichen
- Rasterisierung minimieren
- deterministisch arbeiten
- Cache-Effizienz maximieren
- statischen Speicher verwenden

---

# 4. Architektur

```text
UTF-8

↓

Unicode

↓

Glyph Lookup

↓

Glyph Rasterizer

↓

Glyph Atlas

↓

Renderer

↓

Framebuffer
```

---

# 5. Komponenten

Das Glyph-Atlas-System besteht aus:

- Atlas Manager
- Atlas Allocator
- Glyph Registry
- Packing Engine
- Lookup Table
- Eviction Manager

---

# 6. Grundprinzip

Jede Glyphe wird:

```text
Rasterisiert

↓

Atlas gespeichert

↓

Referenziert

↓

Mehrfach verwendet
```

Ein erneutes Rasterisieren derselben Glyphe ist zu vermeiden.

---

# 7. Atlasstruktur

Ein Atlas besteht aus:

```text
Header

Glyph Pages

Lookup Table

Free Space Table

Statistik
```

Alle Bereiche besitzen feste Speichergrenzen.

---

# 8. Atlas-Seiten

Der Atlas kann mehrere Seiten besitzen.

Jede Seite enthält:

- Alpha-Bitmaps
- Metadaten
- freie Bereiche

Alle Seiten besitzen identische Abmessungen.

---

# 9. Glyph-Eintrag

Jeder Atlas-Eintrag enthält:

- Font-ID
- Unicode-Codepunkt
- Schriftgröße
- Stil
- Atlas-Seite
- Position
- Breite
- Höhe
- Referenzzähler

---

# 10. Atlas Packing

Unterstützt werden:

- Shelf Packing
- Skyline Packing

Standard:

```text
Skyline Packing
```

Das Packing muss deterministisch erfolgen.

---

# 11. Lookup

Eine Glyphe wird identifiziert durch:

```text
Font-ID

+

Codepunkt

+

Schriftgröße

+

Stil
```

Der Lookup erfolgt in konstanter Zeit.

---

# 12. Referenzverwaltung

Jede Glyphe besitzt einen Referenzzähler.

Beim Entfernen eines Controls wird der Zähler reduziert.

Nicht referenzierte Glyphen können entfernt werden.

---

# 13. Atlas-Erweiterung

Ist eine Seite vollständig belegt:

```text
Neue Seite

↓

Registrierung

↓

Weiteres Packing
```

Die maximale Seitenanzahl wird systemweit konfiguriert.

---

# 14. Eviction

Optional unterstützt:

- LRU
- LFU

Standard:

```text
LRU
```

Kritische UI-Glyphen dürfen niemals entfernt werden.

---

# 15. Themeintegration

Ein Themewechsel erzeugt keinen neuen Atlas.

Nur geänderte Fontvarianten werden ergänzt.

Bereits vorhandene Glyphen bleiben erhalten.

---

# 16. Sicherheit

Das System verhindert:

- doppelte Glypheneinträge
- überlappende Atlasbereiche
- beschädigte Lookup-Tabellen
- Speicherüberläufe
- ungültige Referenzzähler

Alle Atlasoperationen werden validiert.

---

# 17. Speicherverwaltung

Das Glyph-Atlas-System verwendet:

- statische Atlasseiten
- feste Lookup-Tabellen
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Lookup
- O(1)-Referenzverwaltung
- O(log n) oder besser für Packing-Freiplatzsuche
- deterministische Laufzeit

Bereits gespeicherte Glyphen dürfen nicht erneut rasterisiert werden.

---

# 19. API

```c
typedef struct
{
    uint32_t page;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
}
nova_glyph_atlas_entry_t;

nova_result_t
nova_glyph_atlas_initialize(void);

const nova_glyph_atlas_entry_t*
nova_glyph_atlas_lookup(
    const nova_font_t* font,
    uint32_t codepoint,
    uint32_t size);

nova_result_t
nova_glyph_atlas_insert(
    const nova_font_t* font,
    uint32_t codepoint,
    uint32_t size,
    const nova_glyph_bitmap_t* bitmap);

nova_result_t
nova_glyph_atlas_release(
    const nova_font_t* font,
    uint32_t codepoint,
    uint32_t size);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Atlasseiten
- Glyphanzahl
- Cache Hits
- Cache Misses
- Referenzzähler
- Auslastung
- Evictions
- Speicherverbrauch

---

# 21. Testfälle

Zu testen sind:

- Glyph-Einfügen
- Lookup
- Mehrfachverwendung
- Atlas-Erweiterung
- Skyline Packing
- LRU
- Themewechsel
- Fallback Fonts
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Glyphen eindeutig identifiziert werden.
- bereits vorhandene Glyphen mehrfach verwendet werden.
- Atlasseiten deterministisch verwaltet werden.
- Packing ohne Überlappungen erfolgt.
- Referenzverwaltung korrekt arbeitet.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTTEXT-0006 – Glyph Atlas
- ADR-BOOTFONT-0001 – Font Management
- ADR-BOOTMEMORY-0001 – Speicherverwaltung
- ADR-BOOTRENDER-0001 – Rendering Engine

## NPSPECs

- NPSPEC-BOOTTEXT-0001 – Boot Text Engine
- NPSPEC-BOOTTEXT-0004 – Font Loading
- NPSPEC-BOOTTEXT-0005 – Glyph Rasterization
- NPSPEC-BOOTRENDER-0001 – Boot Rendering Engine
- NPSPEC-BOOTMEMORY-0001 – Boot Memory Management

---

# 24. Zusammenfassung

Das **Glyph Atlas System** definiert die zentrale Verwaltung rasterisierter Glyphen innerhalb der NovaOS-Bootoberfläche. Es speichert jede Glyphenkombination aus Schriftart, Stil und Größe genau einmal, ermöglicht deren wiederholte Nutzung und reduziert dadurch Rasterisierungsvorgänge sowie Speicherverbrauch erheblich. Durch deterministisches Packing, statische Atlasseiten, effiziente Lookup-Tabellen und Referenzverwaltung bildet der Glyph Atlas eine leistungsfähige Grundlage für die Textdarstellung auf BIOS- und UEFI-basierten Systemen.