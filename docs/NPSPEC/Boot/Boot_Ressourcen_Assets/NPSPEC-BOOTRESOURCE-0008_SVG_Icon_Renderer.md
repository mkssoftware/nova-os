# NPSPEC-BOOTRESOURCE-0008
# SVG Icon Renderer

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0008 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0004, NPSPEC-BOOTRESOURCE-0006, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTRENDER-0012, NPSPEC-BOOTRENDER-0013, NPSPEC-BOOTRENDER-0016 |

---

# 1. Ziel

Diese Spezifikation definiert den **SVG Icon Renderer** der NovaOS-Bootoberfläche.

Der SVG Icon Renderer verarbeitet skalierbare Vektorgrafiken (SVG) und erzeugt daraus gerenderte Bitmaps für die Darstellung innerhalb der Bootoberfläche. Der Fokus liegt auf kleinen, monochromen oder mehrfarbigen UI-Icons mit hoher Darstellungsqualität und konsistenter Skalierung.

Der Renderer implementiert bewusst nur einen definierten Teil des SVG-Standards, um Speicherbedarf, Komplexität und Bootzeit gering zu halten.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Toolbar-Icons
- Menü-Icons
- Buttons
- Statussymbole
- Warnsymbole
- Theme-Icons
- Bootlogo-Symbole
- Accessibility-Symbole

---

# 3. Ziele

Der SVG Renderer muss:

- SVG-Dateien deterministisch rendern
- verlustfreie Skalierung ermöglichen
- Themefarben unterstützen
- hohe Renderqualität liefern
- statisch arbeiten
- zukünftige Erweiterungen ermöglichen

---

# 4. Architektur

```text
SVG-Datei

↓

Resource Loader

↓

SVG Parser

↓

Scene Builder

↓

Vector Renderer

↓

Rasterizer

↓

RGBA Bitmap

↓

Renderer
```

---

# 5. Komponenten

Das System besteht aus:

- SVG Parser
- XML Reader
- Scene Graph Builder
- Geometry Engine
- Path Renderer
- Rasterizer
- Bitmap Generator

---

# 6. Grundprinzip

Der Ablauf erfolgt wie folgt:

```text
SVG

↓

XML

↓

Objektmodell

↓

Vektorpfade

↓

Transformationen

↓

Rasterisierung

↓

Bitmap
```

Der Renderer arbeitet vollständig unabhängig vom Dateiformat des Zielsystems.

---

# 7. Unterstützte SVG-Version

Unterstützt wird:

```text
SVG 1.1 Tiny Profile
```

Optional können ausgewählte Funktionen aus SVG 2 ergänzt werden.

Nicht unterstützte Elemente werden ignoriert oder führen – abhängig von ihrer Relevanz – zu einem Validierungsfehler.

---

# 8. Unterstützte Elemente

Pflicht:

- svg
- g
- path
- rect
- circle
- ellipse
- line
- polyline
- polygon

Optional:

- defs
- use
- clipPath

Nicht unterstützt:

- script
- foreignObject
- animation
- filter
- video
- audio

---

# 9. Unterstützte Attribute

Unterstützt werden:

- fill
- stroke
- stroke-width
- opacity
- transform
- viewBox
- width
- height

Weitere Attribute können später ergänzt werden.

---

# 10. Transformationen

Unterstützt werden:

- translate
- scale
- rotate
- matrix
- skewX
- skewY

Transformationen werden vor der Rasterisierung angewendet.

---

# 11. Farben

Unterstützt werden:

- RGB
- RGBA
- Hex
- Themefarben

Optional:

- aktuelle Textfarbe (`currentColor`)

Themefarben können beim Rendern ersetzt werden.

---

# 12. Skalierung

SVG-Icons werden grundsätzlich verlustfrei skaliert.

Empfohlene Zielgrößen:

- 16 × 16
- 20 × 20
- 24 × 24
- 32 × 32
- 48 × 48
- 64 × 64

Die Rasterisierung erfolgt immer in der Zielauflösung.

---

# 13. Rasterisierung

Die Ausgabe erfolgt als:

```text
RGBA 32 Bit
```

Optional:

- Alpha Antialiasing
- Kantenglättung

Bitmap-Stretching ist nicht zulässig.

---

# 14. Themeintegration

Das Theme kann definieren:

- Primärfarbe
- Sekundärfarbe
- Akzentfarbe
- Fehlerfarbe
- Warnfarbe

SVG-Dateien dürfen diese Farben referenzieren.

---

# 15. Cacheintegration

Nach erfolgreichem Rendering wird das Ergebnis im Resource Cache gespeichert.

Mehrfach benötigte Icons werden nicht erneut gerendert.

---

# 16. Sicherheit

Das System verhindert:

- ungültige XML-Strukturen
- rekursive Referenzen
- beschädigte Pfaddaten
- Buffer Overflows
- Integerüberläufe
- unzulässige SVG-Elemente

Alle SVG-Dateien werden vollständig validiert.

---

# 17. Speicherverwaltung

Das SVG-System verwendet:

- statische XML-Puffer
- Path-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(n)-XML-Parsing
- O(n)-Pfadverarbeitung
- O(n)-Rasterisierung
- O(1)-Cache-Lookup

Bereits gerenderte Icons werden ausschließlich aus dem Cache geladen.

---

# 19. API

```c
typedef struct
{
    uint32_t width;
    uint32_t height;
    nova_bitmap_t bitmap;
}
nova_svg_icon_t;

nova_result_t
nova_svg_initialize(void);

nova_result_t
nova_svg_render(
    const void* source,
    size_t source_size,
    uint32_t target_size,
    nova_svg_icon_t* icon);

bool
nova_svg_validate(
    const void* source,
    size_t source_size);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- SVG-Version
- Anzahl Pfade
- Anzahl Transformationen
- Zielgröße
- Renderzeit
- Cache-Hits
- Validierungsfehler
- Speicherverbrauch

---

# 21. Testfälle

Zu testen sind:

- path
- circle
- rect
- polygon
- Transformationen
- Themefarben
- Skalierung
- Alpha
- beschädigte XML-Dateien
- ungültige SVG-Elemente
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- alle unterstützten SVG-Elemente korrekt verarbeitet werden.
- Icons verlustfrei in unterschiedliche Größen rasterisiert werden.
- Themefarben korrekt ersetzt werden.
- Transformationen deterministisch angewendet werden.
- gerenderte Icons im Cache wiederverwendet werden.
- unzulässige SVG-Dateien sicher abgelehnt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0008 – SVG Icon Renderer
- ADR-BOOTRESOURCE-0004 – Resource Loading
- ADR-BOOTRENDER-0012 – Transformations
- ADR-BOOTRENDER-0013 – Vector Geometry

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading
- NPSPEC-BOOTRESOURCE-0006 – Resource Integrity Verification
- NPSPEC-BOOTRENDER-0001 – Boot Rendering Engine
- NPSPEC-BOOTRENDER-0012 – Transformations
- NPSPEC-BOOTRENDER-0013 – Vector Geometry
- NPSPEC-BOOTRENDER-0016 – Image Rendering

---

# 24. Zusammenfassung

Der **SVG Icon Renderer** definiert die standardisierte Verarbeitung und Rasterisierung von SVG-basierten Symbolen innerhalb der NovaOS-Bootoberfläche. Er unterstützt einen bewusst eingeschränkten, für Benutzeroberflächen optimierten Teil des SVG-Standards und erzeugt daraus hochwertige RGBA-Bitmaps in beliebigen Zielgrößen. Durch deterministische Verarbeitung, Themeintegration, Resource-Caching und statische Speicherverwaltung ermöglicht das System eine konsistente, performante und skalierbare Icondarstellung auf BIOS- und UEFI-basierten Systemen.