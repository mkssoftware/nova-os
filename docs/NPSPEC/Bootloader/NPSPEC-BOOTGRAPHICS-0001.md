# NPSPEC-BOOTGRAPHICS-0001
# Nova Boot Graphics Framework

| Feld | Wert |
|------|------|
| Dokument | NPSPEC-BOOTGRAPHICS-0001 |
| Titel | Nova Boot Graphics Framework |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | Boot Graphics |
| Bereich | Nova Boot Platform |
| Übergeordnet | NPSPEC-BOOTSTAGE2-0001 |
| Baut auf | NPSPEC-BOOTPLATFORM-0001, NPSPEC-BOOTMEMORY-0001 |
| Letzte Änderung | 2026-07-14 |

---

# 1. Zweck

Das **Nova Boot Graphics Framework (NBGF)** definiert die komplette Grafikarchitektur des Nova Bootloaders.

Es stellt sämtliche grafischen Funktionen bereit, bevor der Kernel gestartet wird.

Alle grafischen Komponenten verwenden ausschließlich dieses Framework.

---

# 2. Ziele

Das Framework soll

- BIOS VBE unterstützen
- UEFI GOP unterstützen
- Framebuffer abstrahieren
- Double Buffering bereitstellen
- PNG laden
- SVG darstellen
- TTF-Schriften rendern
- Animationen unterstützen
- Themes bereitstellen
- Fenster und Widgets darstellen

---

# 3. Designprinzipien

Das Graphics Framework basiert auf folgenden Prinzipien.

## Plattformneutralität

Der Renderer kennt niemals

- VBE
- GOP
- Firmware

Alle Grafikzugriffe erfolgen ausschließlich über die Graphics Runtime.

---

## Einheitlicher Framebuffer

Alle Plattformen liefern dieselbe Struktur.

```text
BIOS

↓

VBE

↓

Framebuffer
```

```text
UEFI

↓

GOP

↓

Framebuffer
```

↓

```text
Nova Graphics Runtime
```

---

## Software Renderer

Während des Bootvorgangs erfolgt das Rendering vollständig in Software.

Es werden keine GPU-spezifischen APIs verwendet.

---

## Determinismus

Jeder Renderdurchlauf erzeugt bei identischen Eingaben dasselbe Bild.

---

## Trennung von Darstellung und Logik

Widgets erzeugen keine Pixel.

Sie erzeugen lediglich Renderbefehle.

---

# 4. Architektur

```text
Boot Core
↓
Graphics Runtime
├── Framebuffer Manager
├── Renderer
├── Font Engine
├── Image Engine
├── Vector Engine
├── Theme Manager
├── Animation Engine
├── Widget Renderer
└── Present Engine
```

---

# 5. Grafikpipeline

```text
Widget

↓

Layout

↓

Renderer

↓

Backbuffer

↓

Present

↓

Framebuffer
```

---

# 6. Komponenten

Das Framework besteht aus

- Graphics Runtime
- Framebuffer Manager
- Renderer
- Font Engine
- PNG Decoder
- SVG Renderer
- Theme Manager
- Animation Runtime
- UI Renderer
- Presentation Engine

---

# 7. Framebuffer

Der Framebuffer stellt die einzige Ausgabefläche dar.

```c
typedef struct
{
    void* address;

    uint32_t width;

    uint32_t height;

    uint32_t pitch;

    uint32_t bpp;

    uint32_t format;

} nova_framebuffer_t;
```

---

# 8. Pixel Formate

Pflicht

- XRGB8888
- ARGB8888

Optional

- RGB565
- BGRX8888

Intern arbeitet der Renderer bevorzugt mit 32 Bit Farbtiefe.

---

# 9. Unterstützte Auflösungen

Mindestens

- 800×600
- 1024×768
- 1280×720
- 1366×768
- 1600×900
- 1920×1080

Optional

- 2560×1440
- 3840×2160

Die höchste kompatible Auflösung wird automatisch gewählt.

---

# 10. Graphics Runtime

Die Runtime verwaltet

- Grafikmodus
- Framebuffer
- Renderstatus
- Backbuffer
- Dirty Regions
- Synchronisation

Sie bildet den Einstiegspunkt aller Grafikoperationen.

---

# 11. Graphics Manager

```text
Graphics Runtime

↓

Framebuffer

↓

Renderer

↓

Present
```

Der Graphics Manager steuert den gesamten Renderablauf.

---

# 12. Lebenszyklus

```text
Platform

↓

Graphics Detect

↓

Framebuffer

↓

Renderer

↓

Theme

↓

UI

↓

Present
```

---

# 13. Grafikobjekte

```c
typedef enum
{
    NOVA_GRAPHICS_FRAMEBUFFER,

    NOVA_GRAPHICS_SURFACE,

    NOVA_GRAPHICS_TEXTURE,

    NOVA_GRAPHICS_FONT,

    NOVA_GRAPHICS_IMAGE,

    NOVA_GRAPHICS_VECTOR,

    NOVA_GRAPHICS_WIDGET

} nova_graphics_object_type_t;
```

---

# 14. Graphics Context

```c
typedef struct
{
    nova_framebuffer_t framebuffer;

    void* backbuffer;

    bool double_buffering;

    bool dirty_regions;

} nova_graphics_context_t;
```

---

# 15. Anforderungen

Das Graphics Framework MUSS

- plattformneutral sein
- VBE unterstützen
- GOP unterstützen
- einen gemeinsamen Framebuffer bereitstellen
- deterministisch arbeiten
- Double Buffering vorbereiten

---

# 16. SHOULD

Das Framework SOLLTE

- hohe Auflösungen unterstützen
- modular aufgebaut sein
- vollständig dokumentiert sein
- SIMD-Optimierungen vorbereiten

---

# 17. MAY

Das Framework DARF

- GPU-Beschleunigung vorbereiten
- mehrere Farbräume unterstützen
- HDR-Metadaten vorbereiten
- mehrere Backbuffer verwalten

sofern die öffentliche API unverändert bleibt.

---

# Architekturentscheidung (ADR)

Die Nova Boot Graphics Runtime bildet die einzige offizielle Grafikschnittstelle des Bootloaders.

Der Boot Core, die UI und sämtliche Widgets kennen weder VBE noch GOP oder andere firmwareabhängige Grafikmechanismen.

Dadurch bleibt die komplette grafische Oberfläche unabhängig von BIOS, UEFI und zukünftigen Plattformen.

---

# 18. Graphics Initialization

Nach erfolgreicher Initialisierung des Platform Layers beginnt die Initialisierung der Grafik.

```text
Platform

↓

Graphics Runtime

↓

Display Detection

↓

Framebuffer

↓

Video Mode

↓

Renderer

↓

UI
```

Die Graphics Runtime übernimmt anschließend die vollständige Kontrolle über sämtliche Grafikoperationen.

---

# 19. Display Detection

Die Plattform erkennt automatisch alle verfügbaren Displays.

Unterstützt

- BIOS VBE
- UEFI GOP
- integrierte Displays
- externe Monitore

Die Displayerkennung erfolgt vollständig automatisch.

---

# 20. Graphics Driver

Die Plattform stellt genau einen Grafiktreiber bereit.

```text
Graphics Runtime

↓

Graphics Driver

↓

VBE Driver

oder

GOP Driver
```

Der Renderer kennt niemals den verwendeten Treiber.

---

# 21. BIOS VBE Driver

Unter BIOS wird ausschließlich

```text
VESA BIOS Extensions (VBE)
```

verwendet.

Unterstützt

- VBE 2.0
- VBE 3.0
- Linear Framebuffer

---

# 22. Aufgaben des VBE Drivers

Der VBE Driver übernimmt

- Grafikkartenerkennung
- Moduserkennung
- Moduswechsel
- Framebufferadresse
- Farbtiefe
- Auflösung
- Pitch

Alle Informationen werden anschließend in Nova-Datenstrukturen übernommen.

---

# 23. UEFI GOP Driver

Unter UEFI wird ausschließlich

```text
Graphics Output Protocol (GOP)
```

verwendet.

Unterstützt

- GOP
- Framebuffer
- Pixelformate
- Video Modes

---

# 24. GOP Aufgaben

Der GOP Driver übernimmt

- verfügbare Modi
- aktuelle Auflösung
- Framebuffer
- Pixel Format
- Pitch
- Displayinformationen

Danach arbeitet ausschließlich die Graphics Runtime.

---

# 25. Driver Interface

Alle Grafiktreiber implementieren dieselbe API.

```c
typedef struct
{
    nova_result_t (*initialize)(void);

    nova_result_t (*enumerate_modes)(
        nova_video_mode_list_t* list);

    nova_result_t (*set_mode)(
        uint32_t mode);

    nova_result_t (*framebuffer)(
        nova_framebuffer_t* fb);

} nova_graphics_driver_t;
```

---

# 26. Video Mode

```c
typedef struct
{
    uint32_t width;

    uint32_t height;

    uint32_t bpp;

    uint32_t pitch;

    uint32_t refresh_rate;

    uint32_t pixel_format;

} nova_video_mode_t;
```

---

# 27. Video Mode Auswahl

Die Graphics Runtime wählt automatisch den besten Modus.

Prioritäten

```text
32 Bit

↓

höchste Auflösung

↓

Linear Framebuffer

↓

höchste Bildqualität
```

---

# 28. Fallback

Kann der gewünschte Modus nicht aktiviert werden,

wird automatisch zurückgefallen.

Reihenfolge

```text
1920×1080

↓

1600×900

↓

1366×768

↓

1280×720

↓

1024×768

↓

800×600
```

Dadurch startet Nova auf möglichst vielen Systemen.

---

# 29. Monitorinformationen

Falls verfügbar werden gelesen

- Hersteller
- Modell
- native Auflösung
- Farbtiefe
- EDID
- Seriennummer (optional)

---

# 30. EDID Runtime

Unterstützt

- EDID 1.x
- zukünftige Erweiterungen

Die EDID-Daten werden standardisiert gespeichert.

```c
typedef struct
{
    char manufacturer[4];

    char model[32];

    uint32_t native_width;

    uint32_t native_height;

} nova_display_info_t;
```

---

# 31. Pixelformate

Unterstützt

```text
ARGB8888

XRGB8888

RGB565

BGRX8888
```

Intern konvertiert der Renderer alle Formate in ein einheitliches Arbeitsformat.

---

# 32. Farbmanagement

Die Graphics Runtime arbeitet intern standardmäßig mit

```text
sRGB
```

Alle Farbwerte werden in diesem Farbraum verarbeitet.

---

# 33. Alpha

Unterstützt

- Alpha Blending
- Opacity
- Transparenz
- Glas-Effekte

Diese Funktionen arbeiten vollständig softwarebasiert.

---

# 34. Mehrmonitorvorbereitung

Version 1 unterstützt standardmäßig

```text
1 Display
```

Die Architektur erlaubt jedoch bereits

- mehrere Displays
- Spiegelung
- erweiterte Desktops
- unterschiedliche Auflösungen

ohne Änderungen am Renderer.

---

# 35. Displayobjekt

```c
typedef struct
{
    uint32_t id;

    nova_framebuffer_t framebuffer;

    nova_video_mode_t mode;

    nova_display_info_t info;

    bool primary;

} nova_display_t;
```

---

# 36. Graphics Context

```c
typedef struct
{
    nova_display_t display;

    nova_framebuffer_t framebuffer;

    bool initialized;

} nova_graphics_runtime_t;
```

---

# 37. Fehlerbehandlung

Mögliche Fehler

- kein Framebuffer
- ungültiger Grafikmodus
- keine GOP-Unterstützung
- keine VBE-Unterstützung
- ungültige Pixelformate
- Speicherfehler

Alle Fehler werden an das zentrale Bootlog weitergeleitet.

---

# 38. Anforderungen

Die Graphics Runtime MUSS

- VBE unterstützen
- GOP unterstützen
- Framebuffer abstrahieren
- Video Modes erkennen
- Pixelformate vereinheitlichen
- Fallbacks besitzen

---

# 39. SHOULD

Die Graphics Runtime SOLLTE

- EDID auslesen
- mehrere Monitore vorbereiten
- hohe Auflösungen unterstützen
- sRGB verwenden

---

# 40. MAY

Die Graphics Runtime DARF

- HDR vorbereiten
- DisplayPort-Metadaten speichern
- Farbraumkonvertierungen
- zukünftige Grafikstandards

unterstützen.

---

# Architekturentscheidung (ADR)

Die Graphics Runtime besitzt **genau eine interne Darstellung des Framebuffers**.

Ob die Hardware über

- BIOS VBE,
- UEFI GOP,
- ARM Framebuffer oder
- zukünftige Plattformen

angesprochen wird, ist ausschließlich Aufgabe des Graphics Drivers.

Dadurch bleibt der gesamte Renderer einschließlich Themes, Fonts, PNG, SVG und Widgets vollständig plattformunabhängig.

---

# 41. Framebuffer Manager

Der **Framebuffer Manager (FBM)** verwaltet sämtliche Framebuffer des Bootloaders.

Er stellt dem Renderer eine einheitliche Renderoberfläche bereit.

Der Renderer schreibt niemals direkt in den Hardware-Framebuffer.

---

# 42. Ziele

Der Framebuffer Manager soll

- Hardware abstrahieren
- Double Buffering bereitstellen
- mehrere Render Targets unterstützen
- Offscreen Rendering ermöglichen
- Dirty Regions verwalten
- Present steuern

---

# 43. Architektur

```text
Graphics Runtime
↓
Framebuffer Manager
├── Frontbuffer
├── Backbuffer
├── Render Targets
├── Dirty Regions
├── Clipping
└── Present Engine
```

---

# 44. Frontbuffer

Der Frontbuffer entspricht dem Hardware-Framebuffer.

```text
Renderer

↓

Backbuffer

↓

Present

↓

Frontbuffer
```

Direktes Schreiben in den Frontbuffer ist unzulässig.

---

# 45. Backbuffer

Alle Zeichenoperationen erfolgen ausschließlich im Backbuffer.

Vorteile

- kein Flackern
- keine halbfertigen Bilder
- einfache Animationen
- konsistenter Bildaufbau

---

# 46. Framebuffer Objekt

```c
typedef struct
{
    void* address;

    uint32_t width;

    uint32_t height;

    uint32_t pitch;

    uint32_t bpp;

    uint32_t pixel_format;

    uint64_t size;

} nova_framebuffer_t;
```

---

# 47. Backbuffer Objekt

```c
typedef struct
{
    void* memory;

    uint64_t size;

    bool dirty;

} nova_backbuffer_t;
```

---

# 48. Render Target

Ein Render Target beschreibt jede Fläche,

auf die gezeichnet werden kann.

```text
Framebuffer

↓

Surface

↓

Image

↓

Widget

↓

Texture
```

---

# 49. Surface

```c
typedef struct
{
    uint32_t width;

    uint32_t height;

    uint32_t pitch;

    uint32_t format;

    void* pixels;

} nova_surface_t;
```

Surfaces dienen als universelle Zeichenflächen.

---

# 50. Surface Manager

Der Surface Manager verwaltet

- Oberflächen
- Lebensdauer
- Speicher
- Größen
- Clipping

---

# 51. Render Context

```c
typedef struct
{
    nova_surface_t* target;

    nova_rect_t clip;

    uint32_t color;

    uint8_t alpha;

} nova_render_context_t;
```

---

# 52. Double Buffering

Nova verwendet grundsätzlich

```text
Backbuffer

↓

Render

↓

Present

↓

Framebuffer
```

Dadurch entstehen keine sichtbaren Zeichenartefakte.

---

# 53. Triple Buffering

Version 1 unterstützt kein Triple Buffering.

Die Architektur bleibt jedoch dafür vorbereitet.

---

# 54. Dirty Regions

Nur geänderte Bereiche werden neu gezeichnet.

```text
Invalidate

↓

Dirty Rectangle

↓

Redraw

↓

Present
```

---

# 55. Dirty Rectangle

```c
typedef struct
{
    int32_t x;

    int32_t y;

    uint32_t width;

    uint32_t height;

} nova_dirty_rect_t;
```

---

# 56. Vorteile

Dirty Regions reduzieren

- CPU-Last
- Speicherzugriffe
- Renderzeit
- Energieverbrauch

---

# 57. Clipping

Jede Zeichenoperation wird auf den gültigen Bereich begrenzt.

```text
Clip Rectangle

↓

Renderer

↓

Visible Pixels
```

---

# 58. Clip Rectangle

```c
typedef struct
{
    int32_t left;

    int32_t top;

    int32_t right;

    int32_t bottom;

} nova_clip_rect_t;
```

---

# 59. Viewport

Ein Viewport beschreibt den sichtbaren Zeichenbereich.

```text
Surface

↓

Viewport

↓

Visible Area
```

Widgets besitzen eigene Viewports.

---

# 60. Offscreen Rendering

Widgets werden zunächst außerhalb des sichtbaren Bildes gerendert.

```text
Widget

↓

Offscreen Surface

↓

Composition

↓

Backbuffer
```

---

# 61. Blitting

Das Kopieren zwischen Oberflächen erfolgt über Blit-Operationen.

Unterstützt

- Copy
- Alpha Blending
- Stretch
- Color Key
- Rotation (optional)

---

# 62. Blit API

```c
nova_result_t
nova_blit(
    nova_surface_t* source,
    nova_surface_t* destination,
    nova_rect_t* source_rect,
    nova_rect_t* destination_rect);
```

---

# 63. Present Engine

Die Present Engine kopiert den Backbuffer in den Frontbuffer.

```text
Backbuffer

↓

Present

↓

Framebuffer
```

---

# 64. Present Strategien

Unterstützt

- Vollbild
- Dirty Regions
- Teilbereiche

Die Strategie wird automatisch gewählt.

---

# 65. Synchronisation

Alle Renderdurchläufe besitzen dieselbe Reihenfolge.

```text
Layout

↓

Render

↓

Composition

↓

Present
```

Dies verhindert Race Conditions und inkonsistente Bildzustände.

---

# 66. Speicherverwaltung

Alle Backbuffer werden über das Nova Boot Memory Framework reserviert.

Eigenschaften

- ausgerichtet
- reserviert
- validiert
- diagnostizierbar

---

# 67. Statistik

```c
typedef struct
{
    uint64_t frames_rendered;

    uint64_t present_calls;

    uint64_t dirty_regions;

    uint64_t pixels_drawn;

} nova_graphics_statistics_t;
```

---

# 68. Anforderungen

Der Framebuffer Manager MUSS

- Double Buffering besitzen
- Surfaces verwalten
- Dirty Regions unterstützen
- Clipping verwenden
- Present bereitstellen

---

# 69. SHOULD

Der Framebuffer Manager SOLLTE

- Triple Buffering vorbereiten
- Offscreen Rendering unterstützen
- mehrere Render Targets besitzen
- Performance optimieren

---

# 70. MAY

Der Framebuffer Manager DARF

- GPU-Backends vorbereiten
- Render-Caching
- Tile Rendering
- partielle Hardwarebeschleunigung

unterstützen.

---

# Architekturentscheidung (ADR)

Nova verwendet konsequent einen **retained-mode-orientierten Rendering-Ansatz** mit Offscreen-Surfaces, Double Buffering und Dirty Regions.

Dadurch wird eine flüssige grafische Oberfläche ermöglicht, obwohl der Bootloader ausschließlich einen Software-Renderer verwendet.

Die Trennung zwischen Renderer, Framebuffer Manager und Present Engine erlaubt es außerdem, künftig Hardwarebeschleunigung zu integrieren, ohne die öffentliche Rendering-API oder die UI-Komponenten ändern zu müssen.

---

# 71. Software Renderer

Der **Nova Software Renderer (NSR)** ist die zentrale Renderkomponente des Bootloaders.

Er erzeugt sämtliche Pixel ausschließlich über die CPU.

Während des Bootvorgangs werden keinerlei GPU-spezifische APIs verwendet.

---

# 72. Ziele

Der Renderer soll

- deterministisch arbeiten
- plattformunabhängig sein
- hohe Bildqualität liefern
- Alpha unterstützen
- Antialiasing unterstützen
- SIMD vorbereiten
- Multithreading vorbereiten

---

# 73. Architektur

```text
Renderer
├── Primitive Engine
├── Rasterizer
├── Alpha Engine
├── Gradient Engine
├── Antialiasing
├── Blend Engine
├── Clipping
└── Composition
```

---

# 74. Rendering Pipeline

```text
Widget

↓

Render Command

↓

Primitive

↓

Rasterizer

↓

Blend

↓

Backbuffer
```

---

# 75. Render Commands

Der Renderer verarbeitet ausschließlich Renderbefehle.

Beispiele

- Draw Pixel
- Draw Line
- Draw Rectangle
- Draw Circle
- Draw Image
- Draw Text
- Fill Gradient
- Draw Shadow

Dadurch bleibt die Rendering Engine unabhängig von Widgets.

---

# 76. Primitive

Unterstützt werden

- Pixel
- Linie
- Rechteck
- Kreis
- Ellipse
- Polygon
- Bézier-Kurve
- Dreieck
- Bogen

---

# 77. Pixel

```c
void
nova_draw_pixel(
    int32_t x,
    int32_t y,
    nova_color_t color);
```

Dies ist die elementare Zeichenoperation.

Alle anderen Primitive bauen darauf auf.

---

# 78. Linien

Unterstützt

- horizontale Linien
- vertikale Linien
- beliebige Winkel

Algorithmus

- Bresenham
- Xiaolin Wu (optional)

---

# 79. Linien API

```c
void
nova_draw_line(
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    nova_color_t color);
```

---

# 80. Rechtecke

Unterstützt

- Outline
- Filled
- Rounded
- Rounded Filled

---

# 81. Rechteck API

```c
void
nova_draw_rect(
    nova_rect_t rect,
    nova_color_t color);
```

---

# 82. Abgerundete Rechtecke

Nova verwendet standardmäßig abgerundete Ecken.

```text
Radius

↓

4

8

12

16

24 Pixel
```

Diese bilden die Grundlage des Nova-Designsystems.

---

# 83. Kreise

Unterstützt

- Outline
- Filled

Algorithmus

Midpoint Circle Algorithm

---

# 84. Kreisbögen

Unterstützt

- Teilkreise
- Fortschrittsanzeigen
- Loader
- Animationen

---

# 85. Bézier-Kurven

Unterstützt

- Quadratisch
- Kubisch

Diese werden später für

- SVG
- Animationen
- Diagramme

verwendet.

---

# 86. Polygone

Unterstützt

- Dreieck
- Viereck
- n-Eck

Optional

- konkave Polygone

---

# 87. Farbfüllungen

Unterstützt

- Vollfarbe
- Horizontaler Verlauf
- Vertikaler Verlauf
- Radialer Verlauf
- Mehrfarbige Verläufe

---

# 88. Gradient API

```c
void
nova_fill_gradient(
    nova_rect_t rect,
    nova_gradient_t* gradient);
```

---

# 89. Alpha Blending

Alle Zeichenoperationen unterstützen Alpha.

```text
Alpha

0 %

↓

transparent

100 %

↓

deckend
```

---

# 90. Blend Modi

Version 1

- Normal

Vorbereitet

- Multiply
- Screen
- Overlay
- Soft Light
- Add
- Subtract

---

# 91. Antialiasing

Version 1 unterstützt

- Linien
- Kreise
- Bézier-Kurven

Die Aktivierung erfolgt optional.

---

# 92. Schatten

Unterstützt

- Drop Shadow
- Inner Shadow
- Ambient Shadow

Diese werden vollständig softwarebasiert erzeugt.

---

# 93. Blur

Vorbereitet

- Box Blur
- Gaussian Blur

Diese bilden später die Grundlage für den Nova-Glaseffekt.

---

# 94. Transparenz

Unterstützt

- Fenstertransparenz
- Glas
- halbtransparente Panels
- Menüs
- Dialoge

---

# 95. Clipping

Jede Primitive wird automatisch geclippt.

```text
Primitive

↓

Clip Rectangle

↓

Rasterizer
```

Dadurch können keine Pixel außerhalb der Renderfläche geschrieben werden.

---

# 96. Composition

Mehrere Renderbefehle werden in einer festen Reihenfolge kombiniert.

```text
Background

↓

Gradient

↓

Image

↓

Shadow

↓

Widget

↓

Text

↓

Cursor
```

---

# 97. SIMD

Vorbereitet

- SSE2
- SSE4
- AVX2
- AVX-512
- ARM NEON

Die SIMD-Unterstützung verändert niemals die öffentliche API.

---

# 98. Statistik

```c
typedef struct
{
    uint64_t draw_calls;

    uint64_t pixels;

    uint64_t primitives;

    uint64_t blend_calls;

    uint64_t clip_operations;

} nova_renderer_statistics_t;
```

---

# 99. Anforderungen

Der Renderer MUSS

- Primitive rendern
- Alpha unterstützen
- Gradients unterstützen
- Clipping durchführen
- deterministisch arbeiten

---

# 100. SHOULD

Der Renderer SOLLTE

- Antialiasing unterstützen
- SIMD vorbereiten
- Schatten darstellen
- Blur vorbereiten
- reproduzierbare Ergebnisse liefern

---

# 101. MAY

Der Renderer DARF

- Hardwarebeschleunigung vorbereiten
- Compute Shader Backends
- Vulkan Renderer
- OpenGL Renderer
- Direct3D Renderer

unterstützen, sofern diese dieselbe Rendering-API implementieren.

---

# Architekturentscheidung (ADR)

Der Nova Renderer basiert vollständig auf einer **Render-Command-Pipeline**.

Widgets und UI-Komponenten erzeugen keine Pixel direkt, sondern ausschließlich abstrakte Zeichenbefehle.

Der Software Renderer interpretiert diese Befehle und rasterisiert sie in den Backbuffer.

Diese Architektur ermöglicht:

- vollständige Plattformunabhängigkeit
- deterministisches Rendering
- spätere Hardwarebeschleunigung
- einfache Optimierung
- identische Darstellung auf BIOS, UEFI, ARM64 und zukünftigen Plattformen

---

# 102. Font Engine

Die **Nova Font Engine (NFE)** stellt sämtliche Schriftfunktionen des Bootloaders bereit.

Sie unterstützt sowohl Bitmap- als auch Vektorschriften und bildet die Grundlage für sämtliche Textausgaben.

Alle Texte des Bootloaders werden ausschließlich über die Font Engine gerendert.

---

# 103. Ziele

Die Font Engine soll

- TrueType unterstützen
- OpenType vorbereiten
- Bitmap Fonts unterstützen
- Unicode darstellen
- UTF-8 unterstützen
- Kerning unterstützen
- Glyph Cache besitzen
- Icon Fonts unterstützen

---

# 104. Architektur

```text
Graphics Runtime
↓
Font Engine
├── Font Loader
├── Font Manager
├── Glyph Cache
├── UTF Decoder
├── Layout Engine
├── Rasterizer
├── Hinting
└── Text Renderer
```

---

# 105. Unterstützte Schriftformate

Pflicht

- TrueType (.ttf)

Version 1 optional

- Bitmap Fonts

Vorbereitet

- OpenType (.otf)
- Variable Fonts
- WOFF
- WOFF2

---

# 106. Font Runtime

```c
typedef struct
{
    uint32_t id;

    char family[64];

    char style[32];

    uint32_t units_per_em;

    bool scalable;

} nova_font_t;
```

---

# 107. Font Manager

Der Font Manager verwaltet

- geladene Fonts
- Standardschrift
- Fallback Fonts
- Icon Fonts
- Cache
- Referenzzählung

---

# 108. Font Loader

Der Loader lädt Schriftarten aus

- Boot Image
- Recovery Image
- RAM Disk
- eingebetteten Ressourcen

Die Font Engine arbeitet niemals direkt mit Dateisystemen.

---

# 109. Glyph

```c
typedef struct
{
    uint32_t unicode;

    uint16_t width;

    uint16_t height;

    int16_t bearing_x;

    int16_t bearing_y;

    int16_t advance;

    void* bitmap;

} nova_glyph_t;
```

---

# 110. Glyph Cache

Bereits gerenderte Glyphen werden zwischengespeichert.

```text
Unicode

↓

Rasterize

↓

Cache

↓

Render
```

Dadurch muss jede Glyphe nur einmal berechnet werden.

---

# 111. Cachestrategie

Version 1 verwendet

```text
LRU Cache
```

Eigenschaften

- geringer Speicherverbrauch
- hohe Geschwindigkeit
- deterministisch

---

# 112. Unicode

Die Font Engine unterstützt vollständig

```text
Unicode
```

Version 1

- Basic Latin
- Latin-1
- Latin Extended
- Greek
- Cyrillic
- Mathematical Symbols
- Box Drawing

Vorbereitet

- vollständiges Unicode

---

# 113. UTF-8

Alle Texte werden intern als UTF-8 verarbeitet.

```text
UTF-8

↓

Unicode Codepoint

↓

Glyph

↓

Rasterizer
```

---

# 114. Text Layout

Die Layout Engine berechnet

- Zeilenumbrüche
- Textbreite
- Texthöhe
- Baseline
- Absatzabstände
- Ausrichtung

---

# 115. Text Alignment

Unterstützt

- links
- rechts
- zentriert
- Blocksatz (vorbereitet)

---

# 116. Kerning

Unterstützt

- horizontales Kerning

Vorbereitet

- vertikales Kerning

---

# 117. Hinting

Version 1

- Auto Hinting

Vorbereitet

- Native Hinting
- Subpixel Hinting

---

# 118. Text Renderer

```c
nova_result_t
nova_draw_text(
    nova_surface_t* target,
    const char* utf8,
    nova_font_t* font,
    nova_color_t color);
```

Alle Texte werden über diese API gerendert.

---

# 119. Text Styles

Unterstützt

- Normal
- Fett
- Kursiv
- Unterstrichen
- Durchgestrichen

Vorbereitet

- Variable Font Axes

---

# 120. Fallback Fonts

Kann eine Glyphe nicht dargestellt werden,

wird automatisch eine Fallback-Schrift verwendet.

```text
Primary Font

↓

Glyph fehlt

↓

Fallback Font
```

---

# 121. Icon Fonts

Version 1 unterstützt Icon Fonts.

Verwendungszwecke

- Buttons
- Navigation
- Einstellungen
- Warnungen
- Bootstatus
- Recovery

---

# 122. Emoji

Nicht Bestandteil von Version 1.

Die Architektur bleibt jedoch kompatibel.

---

# 123. Textobjekt

```c
typedef struct
{
    nova_font_t* font;

    uint32_t size;

    nova_color_t color;

    uint32_t alignment;

    bool antialiasing;

} nova_text_style_t;
```

---

# 124. Antialiasing

Version 1 unterstützt

- Graustufen-Antialiasing

Vorbereitet

- Subpixel Rendering

---

# 125. Font Statistics

```c
typedef struct
{
    uint64_t glyphs_loaded;

    uint64_t glyph_cache_hits;

    uint64_t glyph_cache_misses;

    uint64_t rendered_strings;

} nova_font_statistics_t;
```

---

# 126. Speicherverwaltung

Alle Fonts und Glyphen werden über das

```text
Nova Boot Memory Framework
```

verwaltet.

Die Font Engine reserviert niemals Speicher direkt.

---

# 127. Anforderungen

Die Font Engine MUSS

- TrueType unterstützen
- UTF-8 unterstützen
- Unicode verarbeiten
- Glyph Cache besitzen
- Fallback Fonts unterstützen

---

# 128. SHOULD

Die Font Engine SOLLTE

- Kerning unterstützen
- Hinting unterstützen
- Antialiasing verwenden
- Icon Fonts unterstützen

---

# 129. MAY

Die Font Engine DARF

- OpenType
- Variable Fonts
- Color Fonts
- Emoji
- SVG Fonts

unterstützen.

---

# 130. Architekturentscheidung (ADR)

Die Nova Font Engine rendert sämtliche Texte **softwarebasiert** und vollständig unabhängig von der zugrunde liegenden Plattform.

Alle Texte werden zunächst von UTF-8 in Unicode-Codepoints dekodiert, anschließend über den Glyph Cache aufgelöst und schließlich als Renderbefehle an den Software Renderer übergeben.

Dadurch entstehen:

- konsistente Textdarstellung
- vollständige Plattformunabhängigkeit
- reproduzierbare Ergebnisse
- einfache Erweiterbarkeit
- identische Darstellung auf BIOS, UEFI, ARM64 und zukünftigen Plattformen

---

# 131. Image Engine

Die **Nova Image Engine (NIE)** stellt sämtliche Bildfunktionen des Bootloaders bereit.

Sie übernimmt

- Laden
- Dekodieren
- Skalieren
- Zwischenspeichern
- Rendern

aller Bildressourcen.

---

# 132. Ziele

Die Image Engine soll

- PNG unterstützen
- BMP unterstützen
- SVG unterstützen
- JPEG vorbereiten
- Transparenz unterstützen
- Bildcache besitzen
- Softwarebasiert arbeiten

---

# 133. Architektur

```text
Graphics Runtime
↓
Image Engine
├── Image Loader
├── PNG Decoder
├── BMP Decoder
├── JPEG Decoder
├── SVG Renderer
├── Texture Manager
├── Image Cache
└── Composition Engine
```

---

# 134. Unterstützte Bildformate

Pflicht

- PNG
- BMP
- SVG

Version 1 optional

- JPEG

Vorbereitet

- WebP
- AVIF
- ICO
- TIFF

---

# 135. PNG Decoder

PNG ist das primäre Bildformat des Nova Bootloaders.

Unterstützt

- RGBA
- RGB
- Graustufen
- Alpha
- Interlaced PNG

---

# 136. PNG Eigenschaften

Unterstützt

- 8 Bit
- 16 Bit
- Transparenz
- Farbpaletten
- Alpha

Nicht unterstützt

- Animation (APNG)

---

# 137. BMP Decoder

Unterstützt

- 24 Bit
- 32 Bit
- Alpha (wenn vorhanden)

BMP dient hauptsächlich

- Diagnose
- Recovery
- Kompatibilität

---

# 138. JPEG

Version 1

- Baseline JPEG

Vorbereitet

- Progressive JPEG

JPEG eignet sich insbesondere für

- Hintergrundbilder
- Fotos
- Recovery-Medien

---

# 139. SVG Renderer

SVG wird vollständig softwarebasiert gerendert.

Unterstützt

- Paths
- Linien
- Rechtecke
- Kreise
- Ellipsen
- Bézier-Kurven
- Gruppen
- Transformationen

---

# 140. SVG Version

Version 1

SVG Tiny 1.2

Vorbereitet

SVG 2.0

---

# 141. SVG Objekte

```text
SVG

↓

Parser

↓

Scene Graph

↓

Renderer

↓

Backbuffer
```

---

# 142. Unterstützte SVG Elemente

- path
- rect
- circle
- ellipse
- polygon
- polyline
- line
- g
- defs
- use

---

# 143. SVG Transformationen

Unterstützt

- translate
- rotate
- scale
- matrix
- skewX
- skewY

---

# 144. SVG Farben

Unterstützt

- RGB
- RGBA
- Hex
- Named Colors
- Gradients

---

# 145. SVG Gradients

Unterstützt

- Linear Gradient

- Radial Gradient

Vorbereitet

- Mesh Gradient

---

# 146. Image Runtime

```c
typedef struct
{
    uint32_t width;

    uint32_t height;

    uint32_t format;

    uint32_t pitch;

    void* pixels;

} nova_image_t;
```

---

# 147. Image Loader

```c
nova_result_t
nova_image_load(
    const char* path,
    nova_image_t* image);
```

Der Loader erkennt das Bildformat automatisch.

---

# 148. Texture Manager

Alle Bilder werden intern als Texturen behandelt.

```text
PNG

↓

Texture

↓

Renderer
```

Dadurch arbeiten alle Renderkomponenten identisch.

---

# 149. Texture Objekt

```c
typedef struct
{
    uint32_t width;

    uint32_t height;

    uint32_t format;

    void* pixels;

} nova_texture_t;
```

---

# 150. Image Cache

Bereits dekodierte Bilder werden zwischengespeichert.

```text
Image

↓

Decode

↓

Cache

↓

Render
```

Dadurch müssen PNG-Dateien nur einmal dekodiert werden.

---

# 151. Cache Strategie

Version 1

```text
LRU Cache
```

Eigenschaften

- deterministisch
- geringer Speicherverbrauch
- hohe Performance

---

# 152. Skalierung

Unterstützt

- Originalgröße
- Verkleinern
- Vergrößern

Version 1

Interpolation

- Nearest Neighbor
- Bilinear

Vorbereitet

- Bicubic
- Lanczos

---

# 153. Rotation

Unterstützt

- 90°
- 180°
- 270°

Vorbereitet

- beliebige Winkel

---

# 154. Spiegelung

Unterstützt

- Horizontal

- Vertikal

---

# 155. Alpha

Alle Bilder unterstützen

- Transparenz
- Alpha Blending
- Opacity

---

# 156. Bildkomposition

Mehrere Bilder können kombiniert werden.

```text
Background

↓

Image

↓

Overlay

↓

Text

↓

Cursor
```

---

# 157. Farbraum

Intern verwendet die Image Engine

```text
sRGB
```

Alle Bildformate werden darauf konvertiert.

---

# 158. Speicherverwaltung

Alle Bilder werden ausschließlich über

```text
Nova Boot Memory Framework
```

verwaltet.

Es erfolgen keine direkten Speicherallokationen.

---

# 159. Statistik

```c
typedef struct
{
    uint64_t images_loaded;

    uint64_t cache_hits;

    uint64_t cache_misses;

    uint64_t decoded_png;

    uint64_t decoded_svg;

} nova_image_statistics_t;
```

---

# 160. Anforderungen

Die Image Engine MUSS

- PNG unterstützen
- SVG unterstützen
- BMP unterstützen
- Bildcache besitzen
- Alpha unterstützen

---

# 161. SHOULD

Die Image Engine SOLLTE

- JPEG unterstützen
- Bilineare Skalierung besitzen
- Bildrotation unterstützen
- Farbraumkonvertierung durchführen

---

# 162. MAY

Die Image Engine DARF

- AVIF
- WebP
- TIFF
- ICO
- HDR-Bilder

unterstützen.

---

# Architekturentscheidung (ADR)

Die Nova Image Engine trennt **Dekodierung**, **Zwischenspeicherung** und **Rendering** vollständig voneinander.

Alle Bildformate werden zunächst in das interne Objekt `nova_image_t` konvertiert und anschließend als `nova_texture_t` an den Software Renderer übergeben.

Dadurch muss der Renderer keine Kenntnisse über PNG-, BMP-, JPEG- oder SVG-Dateien besitzen und bleibt vollständig unabhängig von Dateiformaten.

---

# 163. Theme Engine

Die **Nova Theme Engine (NTE)** definiert das vollständige Erscheinungsbild des Bootloaders.

Sie stellt Farben, Materialien, Abstände, Eckenradien, Animationen und sämtliche Designparameter bereit.

Alle grafischen Komponenten verwenden ausschließlich die Theme Engine.

---

# 164. Ziele

Die Theme Engine soll

- einheitliche Designs bereitstellen
- Materialsystem verwalten
- Glas-Effekte unterstützen
- Farben zentral verwalten
- Animationen definieren
- High-DPI unterstützen
- Themes austauschbar machen

---

# 165. Architektur

```text
Graphics Runtime
↓
Theme Engine
├── Color Manager
├── Material Manager
├── Typography
├── Icons
├── Elevation
├── Blur Engine
├── Shadow Engine
├── Animation Tokens
└── Theme Loader
```

---

# 166. Designphilosophie

Nova orientiert sich an folgenden Prinzipien:

- Klarheit
- Eleganz
- Konsistenz
- Geschwindigkeit
- Minimalismus
- Räumliche Tiefe
- Moderne Glasoptik

Die Oberfläche soll hochwertig wirken, ohne den Bootvorgang zu verlangsamen.

---

# 167. Standard Theme

Version 1 enthält das Theme

```text
Nova Dark Glass
```

Eigenschaften

- dunkle Oberfläche
- transparente Materialien
- weiche Schatten
- dezente Farbverläufe
- Akzentfarben
- hoher Kontrast

---

# 168. Theme Struktur

```text
Theme
├── Colors
├── Typography
├── Materials
├── Shadows
├── Borders
├── Icons
├── Animations
└── Widgets
```

---

# 169. Theme Objekt

```c
typedef struct
{
    char name[64];

    uint32_t version;

    nova_color_palette_t palette;

    nova_material_theme_t materials;

} nova_theme_t;
```

---

# 170. Farbpalette

Die Theme Engine definiert ausschließlich semantische Farben.

Nicht

```text
Rot

Grün

Blau
```

sondern

```text
Primary

Secondary

Accent

Background

Surface

Error

Success

Warning

Info
```

---

# 171. Farbobjekt

```c
typedef struct
{
    nova_color_t primary;

    nova_color_t secondary;

    nova_color_t accent;

    nova_color_t background;

    nova_color_t surface;

    nova_color_t text;

} nova_color_palette_t;
```

---

# 172. Materialsystem

Nova verwendet ein Materialsystem.

Materialien

- Solid
- Transparent
- Glass
- Acrylic
- Frosted
- Overlay
- Elevated

Widgets definieren lediglich das gewünschte Material.

---

# 173. Material Objekt

```c
typedef struct
{
    float opacity;

    float blur;

    float saturation;

    float brightness;

    bool shadow;

} nova_material_t;
```

---

# 174. Glass Rendering

Das Glass-System simuliert moderne Glasoberflächen.

Eigenschaften

- Transparenz
- Hintergrundunschärfe
- Lichtreflexion
- Farbüberlagerung
- weiche Kanten

Die Berechnung erfolgt vollständig softwarebasiert.

---

# 175. Blur Engine

Version 1 unterstützt

- Box Blur

Vorbereitet

- Gaussian Blur
- Dual Kawase Blur
- Variable Blur

---

# 176. Schatten

Unterstützt

- Ambient Shadow
- Key Shadow
- Contact Shadow
- Inner Shadow

Die Schattenparameter werden zentral im Theme definiert.

---

# 177. Elevation

Widgets besitzen Höhenstufen.

```text
Level 0

Hintergrund

↓

Level 1

Panels

↓

Level 2

Fenster

↓

Level 3

Dialoge

↓

Level 4

Popup

↓

Level 5

Tooltip
```

Jede Stufe definiert automatisch Schatten und Material.

---

# 178. Corner Radius

Nova verwendet konsistente Eckradien.

Standardwerte

```text
2 px

4 px

8 px

12 px

16 px

24 px
```

Eigene Werte sollen vermieden werden.

---

# 179. Abstände

Ein gemeinsames Spacing-System sorgt für ein einheitliches Layout.

```text
4 px

8 px

12 px

16 px

24 px

32 px

48 px
```

Alle Widgets orientieren sich an diesen Einheiten.

---

# 180. Typografie

Die Theme Engine definiert

- Überschrift
- Titel
- Untertitel
- Fließtext
- Hinweistext
- Monospace

Die konkreten Schriftarten stammen aus der Font Engine.

---

# 181. Icon System

Icons werden ausschließlich als SVG verwendet.

Vorteile

- verlustfreie Skalierung
- kleine Dateigröße
- einheitlicher Stil
- beliebige Auflösungen

---

# 182. Animation Tokens

Animationen werden zentral definiert.

```text
Fast

100 ms

↓

Normal

200 ms

↓

Slow

350 ms
```

Widgets verwenden ausschließlich diese Werte.

---

# 183. Easing

Standard

```text
Ease Out Cubic
```

Vorbereitet

- Ease In
- Ease In Out
- Elastic
- Bounce
- Spring

---

# 184. Responsive Design

Das Theme passt sich automatisch an

- Bildschirmgröße
- Auflösung
- DPI
- Seitenverhältnis

an.

---

# 185. High-DPI

Alle Größen werden logisch definiert.

```text
Logical Units

↓

Scale

↓

Pixel
```

Dadurch bleibt die Oberfläche auf allen Auflösungen konsistent.

---

# 186. Theme Loader

```c
nova_result_t
nova_theme_load(
    const char* name);
```

Der Loader lädt sämtliche Theme-Ressourcen.

---

# 187. Theme API

```c
const nova_theme_t*
nova_theme_current(void);
```

Alle Komponenten beziehen ihre Darstellung über diese API.

---

# 188. Theme Statistics

```c
typedef struct
{
    uint64_t materials;

    uint64_t colors;

    uint64_t icons;

    uint64_t animations;

} nova_theme_statistics_t;
```

---

# 189. Anforderungen

Die Theme Engine MUSS

- Farben zentral verwalten
- Materialsystem besitzen
- Glasoberflächen unterstützen
- Animation Tokens definieren
- High-DPI unterstützen

---

# 190. SHOULD

Die Theme Engine SOLLTE

- Blur unterstützen
- SVG-Icons verwenden
- Responsive Layouts ermöglichen
- zukünftige Themes unterstützen

---

# 191. MAY

Die Theme Engine DARF

- Light Theme
- High Contrast Theme
- Accessibility Themes
- OEM Themes
- Benutzerdefinierte Themes

unterstützen.

---

# Architekturentscheidung (ADR)

Die Theme Engine trennt **Darstellung** vollständig von der **Anwendungslogik**.

Widgets definieren ausschließlich semantische Eigenschaften wie Material, Farbe, Größe und Elevation.

Die konkrete Darstellung – einschließlich Glas, Schatten, Farben, Animationen und Typografie – wird ausschließlich durch das aktive Theme bestimmt.

Dadurch können zukünftige Designs eingeführt werden, ohne Änderungen an Widgets oder Rendering-Code vorzunehmen.

---

# 192. Widget Rendering Engine

Die **Nova Widget Rendering Engine (NWRE)** stellt sämtliche grafischen UI-Komponenten des Bootloaders bereit.

Sie bildet die oberste Ebene des Nova Graphics Frameworks.

Alle sichtbaren Elemente werden als Widgets dargestellt.

---

# 193. Ziele

Die Widget Engine soll

- einheitliche Widgets bereitstellen
- Layout berechnen
- Rendering koordinieren
- Animationen unterstützen
- Fokus verwalten
- Eingaben verarbeiten
- Theme Engine verwenden

---

# 194. Architektur

```text
Widget Engine
├── Widget Tree
├── Layout Engine
├── Render Tree
├── Focus Manager
├── Animation Manager
├── Event Dispatcher
├── Hit Testing
└── Composition
```

---

# 195. Widget Tree

Alle Widgets bilden einen hierarchischen Baum.

```text
Desktop
├── Background
├── Window
│   ├── Titlebar
│   ├── Button
│   └── Label
└── Cursor
```

Jedes Widget besitzt genau einen Elternknoten.

---

# 196. Widget Objekt

```c
typedef struct nova_widget
{
    uint64_t id;

    nova_widget_type_t type;

    nova_rect_t bounds;

    bool visible;

    bool enabled;

    struct nova_widget* parent;

    struct nova_widget** children;

} nova_widget_t;
```

---

# 197. Widget Typen

```c
typedef enum
{
    NOVA_WIDGET_PANEL,

    NOVA_WIDGET_LABEL,

    NOVA_WIDGET_BUTTON,

    NOVA_WIDGET_IMAGE,

    NOVA_WIDGET_PROGRESSBAR,

    NOVA_WIDGET_SLIDER,

    NOVA_WIDGET_LIST,

    NOVA_WIDGET_ICON,

    NOVA_WIDGET_WINDOW,

    NOVA_WIDGET_CANVAS,

    NOVA_WIDGET_CUSTOM

} nova_widget_type_t;
```

---

# 198. Widget Lifecycle

```text
Create

↓

Initialize

↓

Layout

↓

Render

↓

Update

↓

Destroy
```

---

# 199. Layout Engine

Die Layout Engine berechnet automatisch

- Position
- Größe
- Abstände
- Ausrichtung
- Skalierung

Widgets positionieren sich niemals selbst.

---

# 200. Layoutmodelle

Version 1 unterstützt

- Absolute Layout
- Flex Layout
- Vertical Layout
- Horizontal Layout

Vorbereitet

- Grid Layout
- Constraint Layout
- Dock Layout

---

# 201. Flex Layout

Flex Layout orientiert sich an modernen UI-Systemen.

Unterstützt

- Row
- Column
- Grow
- Shrink
- Alignment
- Spacing

---

# 202. Absolute Layout

Widgets besitzen feste Koordinaten.

```text
x

y

width

height
```

Dieses Layout eignet sich besonders für

- Splash Screens
- Bootlogos
- Recovery

---

# 203. Responsive Layout

Alle Layouts unterstützen

- DPI Scaling
- Bildschirmgrößen
- Seitenverhältnisse
- automatische Zentrierung

---

# 204. Z-Order

Widgets besitzen Ebenen.

```text
Desktop

↓

Panel

↓

Window

↓

Dialog

↓

Popup

↓

Tooltip

↓

Cursor
```

---

# 205. Render Tree

Nach Abschluss des Layouts entsteht der Render Tree.

```text
Widget Tree

↓

Layout

↓

Render Tree

↓

Renderer
```

Der Renderer verarbeitet ausschließlich den Render Tree.

---

# 206. Hit Testing

Das Hit Testing bestimmt,

welches Widget Eingaben erhält.

```text
Cursor

↓

Hit Test

↓

Widget
```

---

# 207. Focus Manager

Der Focus Manager verwaltet

- Mausfokus
- Tastaturfokus
- Hover
- Aktiv
- Disabled

---

# 208. Fokuszustände

```c
typedef enum
{
    NOVA_FOCUS_NONE,

    NOVA_FOCUS_HOVER,

    NOVA_FOCUS_ACTIVE,

    NOVA_FOCUS_KEYBOARD,

    NOVA_FOCUS_DISABLED

} nova_focus_state_t;
```

---

# 209. Event Dispatcher

Alle Eingaben laufen über den Event Dispatcher.

```text
Mouse

↓

Keyboard

↓

Touch

↓

Dispatcher

↓

Widget
```

---

# 210. Widget Rendering

Die Renderreihenfolge

```text
Background

↓

Shadow

↓

Material

↓

Border

↓

Image

↓

Content

↓

Overlay

↓

Focus

↓

Animation
```

Dadurch entsteht ein konsistenter Bildaufbau.

---

# 211. Panel Widget

Panels dienen als Container.

Eigenschaften

- Material
- Padding
- Radius
- Shadow
- Border

Panels besitzen keine eigene Logik.

---

# 212. Label Widget

Labels stellen ausschließlich Text dar.

Unterstützt

- Mehrzeilig
- Alignment
- Ellipsis
- UTF-8
- Icon Fonts

---

# 213. Button Widget

Ein Button besitzt

- Hintergrund
- Icon
- Text
- Hover
- Pressed
- Disabled
- Focus

Alle Zustände stammen aus der Theme Engine.

---

# 214. Progress Widget

Unterstützt

- Linear
- Circular

Verwendung

- Bootfortschritt
- Recovery
- Downloads
- Speicherprüfung

---

# 215. Image Widget

Das Image Widget unterstützt

- PNG
- SVG
- BMP

Eigenschaften

- Skalierung
- Zentrierung
- Alpha
- Rotation

---

# 216. Window Widget

Fenster bestehen aus

```text
Titlebar

↓

Content

↓

Status

↓

Shadow
```

Sie bilden die Grundlage des Nova Boot Managers.

---

# 217. Canvas Widget

Das Canvas erlaubt freies Zeichnen.

Verwendung

- Diagramme
- Debug
- Animationen
- Spezialwidgets

---

# 218. Widget API

```c
nova_widget_t*
nova_widget_create(
    nova_widget_type_t type);
```

---

# 219. Layout API

```c
void
nova_layout(
    nova_widget_t* root);
```

---

# 220. Render API

```c
void
nova_render_widget(
    nova_widget_t* widget);
```

---

# 221. Anforderungen

Die Widget Engine MUSS

- Widget Tree besitzen
- Layout berechnen
- Fokus verwalten
- Hit Testing durchführen
- Theme Engine verwenden

---

# 222. SHOULD

Die Widget Engine SOLLTE

- Flex Layout unterstützen
- Responsive Layout besitzen
- Animationen vorbereiten
- Offscreen Rendering verwenden

---

# 223. MAY

Die Widget Engine DARF

- Constraint Layout
- Virtualisierte Listen
- GPU Widgets
- Accessibility Layer

unterstützen.

---

# Architekturentscheidung (ADR)

Die Nova Widget Engine verwendet einen **retained-mode Widget Tree**.

Widgets erzeugen weder Pixel noch führen sie Layoutberechnungen selbst durch.

Sie beschreiben ausschließlich ihre Eigenschaften.

Layout Engine, Renderer und Theme Engine erzeugen daraus die endgültige Darstellung.

Dadurch entstehen:

- deterministische Darstellung
- geringe Kopplung
- einfache Erweiterbarkeit
- identisches Verhalten auf allen Plattformen

---

# 224. Animation Engine

Die **Nova Animation Engine (NAE)** steuert sämtliche Animationen des Bootloaders.

Sie sorgt für flüssige Übergänge und moderne Benutzerinteraktionen, ohne den Bootvorgang zu verzögern.

Animationen sind vollständig optional und dürfen niemals die Funktionalität beeinträchtigen.

---

# 225. Ziele

Die Animation Engine soll

- flüssige Übergänge erzeugen
- zeitgesteuert arbeiten
- deterministisch sein
- geringe CPU-Last besitzen
- Animationen kombinieren
- Animationen abbrechen können

---

# 226. Architektur

```text
Animation Engine
├── Timeline
├── Animator
├── Transition Engine
├── Physics Engine
├── Scheduler
├── Keyframe Engine
├── State Machine
└── Present Synchronizer
```

---

# 227. Ablauf

```text
Event

↓

Animation

↓

Timeline

↓

Interpolation

↓

Renderer

↓

Present
```

---

# 228. Animation Objekt

```c
typedef struct
{
    uint64_t id;

    uint64_t duration;

    uint64_t elapsed;

    bool running;

    bool finished;

} nova_animation_t;
```

---

# 229. Timeline

Alle Animationen besitzen eine Timeline.

```text
Start

↓

Running

↓

Finish
```

Die Timeline verwendet monotone Bootzeit.

---

# 230. Zeitbasis

Alle Zeiten werden in

```text
Nanosekunden
```

gespeichert.

Die Animation Engine verwendet ausschließlich den Nova Timer Service.

---

# 231. Keyframes

Unterstützt

- Start
- Zwischenpunkte
- Ende

```text
0 %

↓

25 %

↓

50 %

↓

75 %

↓

100 %
```

---

# 232. Keyframe Objekt

```c
typedef struct
{
    float position;

    float value;

} nova_keyframe_t;
```

---

# 233. Interpolation

Version 1 unterstützt

- Linear
- Ease In
- Ease Out
- Ease In Out

Vorbereitet

- Cubic
- Elastic
- Bounce
- Spring

---

# 234. Standard Easing

Nova verwendet standardmäßig

```text
Ease Out Cubic
```

Dies sorgt für ruhige Bewegungen.

---

# 235. Animationstypen

Unterstützt

- Position
- Größe
- Transparenz
- Rotation
- Skalierung
- Farbe

---

# 236. Transition Engine

Transitions verbinden zwei Widgetzustände.

```text
State A

↓

Transition

↓

State B
```

---

# 237. State Animation

Widgets können Zustände animieren.

Beispiele

- Hover
- Pressed
- Disabled
- Selected
- Focus

---

# 238. Boot Animation

Die Bootanimation besteht aus mehreren Sequenzen.

```text
Logo

↓

Glow

↓

Progress

↓

Boot Manager
```

Jede Sequenz ist unabhängig konfigurierbar.

---

# 239. Splash Animation

Unterstützt

- Fade In
- Fade Out
- Zoom
- Rotation
- Blur
- Glow

---

# 240. Progress Animation

Der Fortschrittsbalken unterstützt

- kontinuierliche Bewegung
- weiche Übergänge
- Statuswechsel

---

# 241. Cursor Animation

Der Cursor kann animiert werden.

Unterstützt

- Fade
- Scale
- Hover
- Klickeffekte

---

# 242. Physics Animation

Vorbereitet

- Federbewegung
- Trägheit
- Dämpfung
- Impuls

Version 1 verwendet standardmäßig keine physikalischen Animationen.

---

# 243. Animation Scheduler

Der Scheduler startet Animationen.

```text
Queue

↓

Timeline

↓

Update

↓

Render
```

---

# 244. Gleichzeitige Animationen

Mehrere Animationen dürfen parallel laufen.

Beispiele

- Fenster öffnet
- Hintergrund blendet
- Fortschrittsbalken bewegt sich

---

# 245. Prioritäten

```text
Critical

↓

High

↓

Normal

↓

Background
```

Bootkritische Animationen besitzen höchste Priorität.

---

# 246. Animation API

```c
nova_result_t
nova_animation_start(
    nova_animation_t* animation);
```

---

# 247. Stop API

```c
nova_result_t
nova_animation_stop(
    uint64_t id);
```

---

# 248. Update API

```c
void
nova_animation_update(
    uint64_t delta_time);
```

Diese Funktion wird einmal pro Renderzyklus aufgerufen.

---

# 249. Synchronisation

Animationen werden ausschließlich zwischen

```text
Layout

↓

Render

↓

Present
```

aktualisiert.

Dadurch entstehen keine inkonsistenten Widgetzustände.

---

# 250. Performance

Version 1 soll

- mindestens 60 FPS erreichen

Zukünftig

- 120 FPS
- Adaptive Refresh

---

# 251. Animation Statistics

```c
typedef struct
{
    uint64_t animations_started;

    uint64_t animations_finished;

    uint64_t dropped_frames;

    uint64_t average_frame_time;

} nova_animation_statistics_t;
```

---

# 252. Energieverwaltung

Während kritischer Bootphasen können Animationen automatisch reduziert werden.

Strategien

- kürzere Dauer
- geringere Bildrate
- vollständiges Deaktivieren

---

# 253. Safe Mode

Im Safe Mode sind Animationen standardmäßig deaktiviert.

Dadurch wird maximale Stabilität gewährleistet.

---

# 254. Anforderungen

Die Animation Engine MUSS

- deterministisch arbeiten
- Timelines besitzen
- mehrere Animationen verwalten
- Widgets animieren
- Performance überwachen

---

# 255. SHOULD

Die Animation Engine SOLLTE

- Physics vorbereiten
- adaptive Bildraten unterstützen
- Animationen priorisieren
- reproduzierbare Ergebnisse liefern

---

# 256. MAY

Die Animation Engine DARF

- GPU-Interpolation
- Motion Blur
- Morphing
- Shadereffekte
- Partikelsysteme

unterstützen.

---

# Architekturentscheidung (ADR)

Die Nova Animation Engine arbeitet vollständig **datengetrieben**.

Animationen beschreiben ausschließlich den zeitlichen Verlauf von Eigenschaften.

Die eigentliche Darstellung erfolgt weiterhin ausschließlich über den Software Renderer.

Dadurch bleiben Animationen unabhängig von Widgets, Themes und zukünftigen Rendering-Backends.

---

# 257. Graphics Diagnostics Framework

Das **Nova Graphics Diagnostics Framework (NGDF)** überwacht sämtliche Grafikoperationen des Bootloaders.

Es dient

- Diagnose
- Performanceanalyse
- Debugging
- Qualitätskontrolle
- Recovery
- Kernel-Handoff

Alle Diagnosedaten werden lokal erzeugt.

---

# 258. Ziele

Das Framework soll

- Rendering überwachen
- Framezeiten messen
- FPS berechnen
- Speicherverbrauch analysieren
- Renderfehler erkennen
- Diagnoseberichte erzeugen

---

# 259. Architektur

```text
Graphics Runtime
↓
Diagnostics
├── Render Logger
├── Profiler
├── FPS Counter
├── Statistics
├── Debug Overlay
├── Screenshot Engine
├── Validator
└── Export
```

---

# 260. Render Logger

Jede Zeichenoperation kann protokolliert werden.

Beispiele

- Draw Rectangle
- Draw Image
- Draw SVG
- Draw Text
- Present
- Animation

Der Umfang ist konfigurierbar.

---

# 261. Render Log Entry

```c
typedef struct
{
    uint64_t timestamp;

    uint32_t component;

    uint32_t operation;

    uint64_t duration;

    uint32_t pixels;

} nova_render_log_entry_t;
```

---

# 262. Render Profiler

Der Profiler misst

- Layoutzeit
- Renderzeit
- Blendzeit
- Presentzeit
- Gesamtzeit

Alle Werte werden in Nanosekunden gespeichert.

---

# 263. Performance Report

```c
typedef struct
{
    uint64_t layout_time;

    uint64_t render_time;

    uint64_t blend_time;

    uint64_t present_time;

    uint64_t total_time;

} nova_graphics_performance_report_t;
```

---

# 264. FPS Counter

Die Runtime berechnet

- aktuelle FPS
- durchschnittliche FPS
- minimale FPS
- maximale FPS

Die Werte dienen ausschließlich der Diagnose.

---

# 265. Render Statistics

```c
typedef struct
{
    uint64_t frames;

    uint64_t draw_calls;

    uint64_t pixels;

    uint64_t images;

    uint64_t glyphs;

    uint64_t widgets;

} nova_render_statistics_t;
```

---

# 266. Speicherüberwachung

Erfasst werden

- Backbuffer
- Surfaces
- Texturen
- Fonts
- SVG
- PNG
- Widget Cache

---

# 267. Graphics Memory Report

```c
typedef struct
{
    uint64_t framebuffer;

    uint64_t backbuffer;

    uint64_t textures;

    uint64_t glyphs;

    uint64_t widgets;

} nova_graphics_memory_report_t;
```

---

# 268. Debug Overlay

Im Entwicklermodus kann ein Overlay eingeblendet werden.

Angezeigt werden

- FPS
- Framezeit
- Speicherverbrauch
- Draw Calls
- Dirty Regions
- Mausposition

Das Overlay wird niemals in Release-Builds aktiviert.

---

# 269. Dirty Region Visualizer

Optional können Dirty Regions dargestellt werden.

```text
Widget

↓

Dirty Rectangle

↓

Rot markiert
```

Dies erleichtert die Optimierung.

---

# 270. Render Tree Visualizer

Optional kann der komplette Render Tree dargestellt werden.

Beispiel

```text
Desktop

↓

Panel

↓

Window

↓

Button

↓

Icon
```

---

# 271. Layout Visualizer

Die Layout Engine kann

- Abstände
- Padding
- Margin
- Viewports
- Clipping

grafisch darstellen.

---

# 272. Screenshot Engine

Der Bootloader kann den aktuellen Frame speichern.

Unterstützt

- PNG
- BMP

Verwendung

- Fehlerberichte
- Recovery
- Entwicklung

---

# 273. Screenshot API

```c
nova_result_t
nova_graphics_screenshot(
    const char* filename);
```

---

# 274. Recording

Vorbereitet

- Frame Recording
- Boot Recording
- Render Replay

Version 1 unterstützt ausschließlich Einzelbilder.

---

# 275. Graphics Validation

Vor jedem Present wird geprüft

✔ gültiger Framebuffer

✔ gültiger Backbuffer

✔ gültige Render Targets

✔ keine ungültigen Widgets

✔ keine beschädigten Texturen

---

# 276. Validation Report

```c
typedef struct
{
    bool framebuffer_valid;

    bool renderer_valid;

    bool theme_valid;

    bool widgets_valid;

    bool images_valid;

} nova_graphics_validation_report_t;
```

---

# 277. Fehlerklassen

```text
INFO

↓

WARNING

↓

RECOVERABLE

↓

FATAL
```

Nur FATAL verhindert den weiteren Renderprozess.

---

# 278. Render Recovery

Bei kleineren Fehlern versucht das Framework

- Surface neu erzeugen
- Cache leeren
- Widget neu rendern
- Dirty Region vergrößern

Erst danach erfolgt ein Fehlerabbruch.

---

# 279. Diagnosebericht

```c
typedef struct
{
    nova_graphics_statistics_t statistics;

    nova_graphics_memory_report_t memory;

    nova_graphics_performance_report_t performance;

    nova_graphics_validation_report_t validation;

} nova_graphics_report_t;
```

---

# 280. Export

Der Bericht wird Bestandteil von

```text
nova_boot_info_t
```

Der Kernel kann die Informationen für spätere Analysen übernehmen.

---

# 281. Diagnose API

```c
nova_result_t
nova_graphics_report(
    nova_graphics_report_t* report);
```

---

# 282. Logging API

```c
nova_result_t
nova_graphics_log(
    nova_render_log_entry_t* entries,
    uint32_t* count);
```

---

# 283. Anforderungen

Das Diagnostics Framework MUSS

- Rendering überwachen
- Performance messen
- Diagnoseberichte erzeugen
- Speicher analysieren
- Validierung durchführen

---

# 284. SHOULD

Das Framework SOLLTE

- Debug Overlays besitzen
- Screenshots unterstützen
- Dirty Regions visualisieren
- Render Trees darstellen

---

# 285. MAY

Das Framework DARF

- Videoaufzeichnung
- Heatmaps
- GPU-Diagnose
- automatische Performanceanalyse
- Remote-Debugging

unterstützen.

---

# 286. Architekturentscheidung (ADR)

Die Grafikdiagnose ist ein fester Bestandteil des Nova Graphics Frameworks.

Alle Renderdaten werden über standardisierte Diagnoseobjekte erfasst.

Dadurch können Fehler bereits während des Bootvorgangs analysiert werden, ohne Änderungen am Renderer oder an Widgets vorzunehmen.

Die Diagnosekomponenten sind vollständig vom eigentlichen Rendering getrennt und können in Release-Builds deaktiviert werden.

---

# 287. Öffentliche Graphics API

Das Nova Boot Graphics Framework stellt eine stabile öffentliche API bereit.

Diese API ist die einzige zulässige Schnittstelle für sämtliche Grafikoperationen innerhalb des Bootloaders.

```text
Boot Core

↓

Graphics API

↓

Graphics Runtime

↓

Platform Driver
```

Direkte Zugriffe auf Framebuffer, VBE oder GOP sind außerhalb der Graphics Runtime nicht zulässig.

---

# 288. API Designprinzipien

Die Graphics API folgt den Grundsätzen

- deterministisch
- modular
- ABI-stabil
- plattformneutral
- erweiterbar
- dokumentiert

Alle Änderungen erfolgen ausschließlich über neue API-Versionen.

---

# 289. Graphics Initialisierung

```c
nova_result_t
nova_graphics_initialize(void);
```

Initialisiert

- Display
- Framebuffer
- Renderer
- Font Engine
- Image Engine
- Theme Engine
- Widget Engine
- Animation Engine

---

# 290. Shutdown

```c
nova_result_t
nova_graphics_shutdown(void);
```

Beendet sämtliche Grafikkomponenten kontrolliert.

Der Aufruf erfolgt unmittelbar vor dem Kernel-Handoff.

---

# 291. Present

```c
nova_result_t
nova_graphics_present(void);
```

Überträgt den aktuellen Backbuffer in den Frontbuffer.

---

# 292. Clear

```c
void
nova_graphics_clear(
    nova_color_t color);
```

Löscht den Backbuffer vollständig.

---

# 293. Surface API

```c
nova_surface_t*
nova_surface_create(
    uint32_t width,
    uint32_t height);
```

---

# 294. Image API

```c
nova_result_t
nova_image_load(
    const char* filename,
    nova_image_t* image);
```

---

# 295. Font API

```c
nova_result_t
nova_font_load(
    const char* filename,
    nova_font_t* font);
```

---

# 296. Widget API

```c
nova_widget_t*
nova_widget_create(
    nova_widget_type_t type);
```

---

# 297. Theme API

```c
nova_result_t
nova_theme_load(
    const char* name);
```

---

# 298. Animation API

```c
nova_result_t
nova_animation_start(
    nova_animation_t* animation);
```

---

# 299. Screenshot API

```c
nova_result_t
nova_graphics_screenshot(
    const char* filename);
```

---

# 300. Graphics Context

```c
const nova_graphics_runtime_t*
nova_graphics_context(void);
```

Liefert den aktuellen Graphics Context.

---

# 301. Graphics Statistics

```c
const nova_graphics_statistics_t*
nova_graphics_statistics(void);
```

---

# 302. Graphics Diagnostics

```c
nova_result_t
nova_graphics_report(
    nova_graphics_report_t* report);
```

---

# 303. Graphics Export

```c
nova_result_t
nova_graphics_export(
    nova_boot_graphics_info_t* info);
```

Exportiert sämtliche Grafikdaten für den Kernel.

---

# 304. Fehlercodes

```c
typedef enum
{
    NOVA_GRAPHICS_SUCCESS = 0,

    NOVA_GRAPHICS_NO_DISPLAY,

    NOVA_GRAPHICS_NO_FRAMEBUFFER,

    NOVA_GRAPHICS_INVALID_SURFACE,

    NOVA_GRAPHICS_INVALID_FONT,

    NOVA_GRAPHICS_INVALID_IMAGE,

    NOVA_GRAPHICS_OUT_OF_MEMORY,

    NOVA_GRAPHICS_UNSUPPORTED,

    NOVA_GRAPHICS_FATAL

} nova_graphics_result_t;
```

---

# 305. ABI

Die Graphics Runtime besitzt eine eigene ABI.

Alle exportierten Strukturen beginnen mit

```c
typedef struct
{
    uint64_t magic;

    uint16_t major;

    uint16_t minor;

    uint32_t size;

} nova_graphics_abi_header_t;
```

---

# 306. Magic

```text
"NGBI"
```

Nova Graphics Boot Information

---

# 307. Versionsverwaltung

Semantic Versioning

```text
Major

↓

ABI Änderungen

Minor

↓

Neue Funktionen

Patch

↓

Fehlerkorrekturen
```

---

# 308. Referenzimplementierung

```text
graphics/
├── api/
├── framebuffer/
├── renderer/
├── surfaces/
├── images/
├── fonts/
├── vectors/
├── themes/
├── widgets/
├── animation/
├── diagnostics/
├── export/
├── tests/
└── docs/
```

---

# 309. Unit Tests

Jede Komponente besitzt eigene Unit Tests.

Mindestens

- Renderer
- Framebuffer
- Font Engine
- Image Engine
- SVG
- Theme
- Widgets
- Animation

---

# 310. Integrationstests

Pflichttests

- BIOS VBE

- UEFI GOP

- QEMU

- Bochs

- VMware

- Hyper-V

- VirtualBox

- reale Hardware

---

# 311. Regressionstests

Vor jedem Release werden geprüft

✔ Renderer

✔ Framebuffer

✔ Themes

✔ Fonts

✔ SVG

✔ PNG

✔ Widgets

✔ Animationen

✔ ABI

---

# 312. Belastungstests

Empfohlene Tests

- 800×600

- 1024×768

- 1280×720

- 1920×1080

- 2560×1440

- 3840×2160

Zusätzlich

- 16-Bit

- 24-Bit

- 32-Bit

Pixelformate

---

# 313. Performanceziele

Empfohlene Zielwerte

| Komponente | Ziel |
|------------|------|
| Present | < 2 ms |
| Rechteck | < 100 ns |
| PNG Laden | < 20 ms |
| SVG Render | < 10 ms |
| Text Rendering | < 2 ms |
| Layout | < 1 ms |
| Widget Render | < 5 ms |

---

# 314. Speicherziele

Das Framework soll möglichst wenig Speicher verwenden.

Empfohlen

| Komponente | Speicher |
|------------|----------:|
| Renderer | < 2 MB |
| Theme | < 512 KB |
| Fonts | < 4 MB |
| Bilder | variabel |
| Widget Tree | < 1 MB |

---

# 315. Dokumentationsstandard

Jede öffentliche Funktion wird dokumentiert.

```c
/*
Beschreibung

Parameter

Rückgabewert

Fehler

Seiteneffekte

Komplexität

Thread Safety
*/
```

---

# 316. Anforderungen

Die Graphics API MUSS

- vollständig dokumentiert sein
- ABI-stabil sein
- versioniert sein
- plattformneutral bleiben
- testbar sein

---

# 317. SHOULD

Die Graphics API SOLLTE

- leicht portierbar sein
- zukünftige Plattformen unterstützen
- Hardwarebeschleunigung vorbereiten
- reproduzierbare Ergebnisse liefern

---

# 318. MAY

Die Graphics API DARF

- experimentelle Renderer
- GPU-Backends
- Debugschnittstellen
- OEM-Erweiterungen

unterstützen,

sofern die öffentliche API unverändert bleibt.

---

# Architekturentscheidung (ADR)

Die öffentliche Graphics API ist die einzige zulässige Schnittstelle zwischen Boot Core und Grafiksystem.

Alle Implementierungsdetails – einschließlich VBE, GOP, Software Renderer, Theme Engine, Font Engine und Animation Engine – bleiben vollständig gekapselt.

Dadurch kann die interne Implementierung jederzeit modernisiert oder ersetzt werden, ohne dass bestehende Boot-Komponenten angepasst werden müssen.

---

# 319. Referenzarchitektur

Das Nova Boot Graphics Framework bildet die vollständige Grafikschicht des Bootloaders.

Es verbindet Hardware, Rendering und Benutzeroberfläche zu einer gemeinsamen Plattform.

```text
                    Boot Core
                        │
                        ▼
              Graphics Runtime API
                        │
 ┌──────────────────────┼──────────────────────┐
 │                      │                      │
 ▼                      ▼                      ▼
Renderer          Theme Engine         Widget Engine
 │                      │                      │
 ▼                      ▼                      ▼
Image Engine      Font Engine        Animation Engine
 │                      │                      │
 └─────────────── Composition ────────────────┘
                        │
                        ▼
               Framebuffer Manager
                        │
              ┌─────────┴─────────┐
              ▼                   ▼
        BIOS VBE Driver      UEFI GOP Driver
              │                   │
              └─────────┬─────────┘
                        ▼
                  Bildschirm
```

Diese Architektur bleibt unabhängig von Firmware, Hardware und zukünftigen Grafik-Backends.

---

# 320. Initialisierungsreihenfolge

Die Komponenten werden immer in derselben Reihenfolge gestartet.

```text
Platform Runtime

↓

Graphics Driver

↓

Framebuffer

↓

Renderer

↓

Font Engine

↓

Image Engine

↓

Theme Engine

↓

Widget Engine

↓

Animation Engine

↓

Diagnostics

↓

UI

↓

Present
```

Diese Reihenfolge ist verbindlich.

---

# 321. Beziehungen zu anderen NPSPECs

Das Boot Graphics Framework baut auf folgenden Spezifikationen auf:

- NPSPEC-BOOTSTAGE2-0001
- NPSPEC-BOOTPLATFORM-0001
- NPSPEC-BOOTMEMORY-0001
- NPSPEC-BOOTINPUT-0001
- NPSPEC-BOOTFS-0001
- NPSPEC-BOOTUI-0001
- NPSPEC-BOOTRECOVERY-0001

Spätere Kernel-Komponenten übernehmen große Teile dieser Architektur.

---

# 322. Datenfluss

Der gesamte Renderprozess folgt einer festen Pipeline.

```text
Widget

↓

Layout

↓

Theme

↓

Animation

↓

Render Commands

↓

Software Renderer

↓

Backbuffer

↓

Present

↓

Framebuffer
```

Es existieren keine alternativen Renderpfade.

---

# 323. Ressourcenverwaltung

Alle Ressourcen werden zentral verwaltet.

```text
Memory Runtime

↓

Graphics Runtime

↓

Fonts

↓

Images

↓

SVG

↓

Widgets

↓

Animationen
```

Direkte Speicherverwaltung innerhalb einzelner Grafikmodule ist nicht zulässig.

---

# 324. Implementierungsphasen

## Phase 1

- VBE
- GOP
- Framebuffer
- Software Renderer

---

## Phase 2

- Primitive
- Alpha Blending
- Double Buffering
- Dirty Regions

---

## Phase 3

- Font Engine
- UTF-8
- TrueType
- Glyph Cache

---

## Phase 4

- PNG
- SVG
- Texture Manager
- Image Cache

---

## Phase 5

- Theme Engine
- Glass Rendering
- Materialsystem
- Schatten

---

## Phase 6

- Widget Engine
- Layout Engine
- Render Tree
- Focus Management

---

## Phase 7

- Animation Engine
- Splash Animation
- Boot Animation
- Transition Engine

---

## Phase 8

- Diagnostics
- Performance Analyzer
- Screenshot Engine
- Validation

---

# 325. Akzeptanzkriterien

Das Framework gilt als vollständig implementiert, wenn

1. BIOS VBE unterstützt wird.
2. UEFI GOP unterstützt wird.
3. Framebuffer korrekt abstrahiert wird.
4. Double Buffering funktioniert.
5. Dirty Regions verwendet werden.
6. Der Software Renderer sämtliche Primitive darstellen kann.
7. TrueType-Schriften korrekt gerendert werden.
8. PNG- und SVG-Dateien dargestellt werden.
9. Das Theme-System vollständig funktioniert.
10. Widgets korrekt gerendert werden.
11. Animationen flüssig dargestellt werden.
12. Diagnoseinformationen exportiert werden.

---

# 326. Qualitätsziele

Das Nova Graphics Framework verfolgt folgende Ziele:

- deterministische Darstellung
- konsistente Bildqualität
- minimale Latenz
- hohe Performance
- vollständige Plattformunabhängigkeit
- reproduzierbare Renderausgabe
- geringer Speicherverbrauch

---

# 327. Performanceziele

Empfohlene Zielwerte

| Bereich | Ziel |
|---------|------:|
| Graphics Initialisierung | < 25 ms |
| Present | < 2 ms |
| Vollständiger Frame | < 16 ms |
| Text Rendering | < 2 ms |
| PNG Dekodierung | < 20 ms |
| SVG Rendering | < 10 ms |
| Widget Layout | < 1 ms |
| Widget Rendering | < 5 ms |

Auf geeigneter Hardware soll eine Bildwiederholrate von mindestens **60 FPS** erreicht werden.

---

# 328. Architekturprinzipien

Das Nova Boot Graphics Framework basiert dauerhaft auf folgenden Grundprinzipien.

## Graphics First

Alle grafischen Komponenten arbeiten ausschließlich über die Graphics Runtime.

---

## Single Renderer

Es existiert genau ein Renderer.

Alle Zeichenoperationen laufen über dieselbe Rendering Pipeline.

---

## Software First

Der Software Renderer bildet die Referenzimplementierung.

Hardwarebeschleunigung ist optional und darf die API nicht verändern.

---

## Theme Driven

Die Darstellung wird vollständig vom Theme bestimmt.

Widgets definieren ausschließlich semantische Eigenschaften.

---

## Retained Mode

Die Benutzeroberfläche wird durch einen Widget Tree beschrieben.

Direktes Immediate Rendering ist nicht Bestandteil der Architektur.

---

## Deterministic Rendering

Identische Eingaben erzeugen immer identische Pixel.

---

## Platform Independence

Renderer, Widgets, Fonts, Bilder und Themes besitzen keinerlei Firmwareabhängigkeiten.

---

## API Stability

Die öffentliche Graphics API bleibt über die gesamte Hauptversion stabil.

---

# 329. Erweiterungsmöglichkeiten

Das Framework ist bereits für zukünftige Funktionen vorbereitet.

Unter anderem

- GPU Renderer
- Vulkan Backend
- OpenGL Backend
- Direct3D Backend
- Metal Backend
- HDR Rendering
- Color Management
- Multi Monitor
- HiDPI
- Variable Refresh Rate
- Hardware Cursor
- Hardware Composition
- GPU Blur
- GPU Shadow Rendering
- Partikelsysteme
- Video Playback
- 3D Boot Environment

Diese Erweiterungen dürfen ausschließlich neue Implementierungen bereitstellen und niemals die öffentliche API verändern.

---

# 330. Langfristige Vision

Das Nova Boot Graphics Framework soll langfristig nicht nur den Bootloader versorgen, sondern die Grundlage der gesamten Nova-Grafikarchitektur bilden.

Dies umfasst:

```text
Bootloader

↓

Recovery

↓

Kernel Console

↓

Window Manager

↓

Desktop Environment

↓

Applications

↓

Future 3D UI
```

Dadurch entsteht eine durchgängige Grafikplattform mit einheitlicher API, identischen Rendermechanismen und konsistenter Darstellung über alle Systemebenen hinweg.

---

# 331. Zusammenfassung

Das **Nova Boot Graphics Framework** definiert die komplette grafische Infrastruktur des Nova-Bootloaders.

Es umfasst:

- Plattformabstraktion (VBE/GOP)
- Framebuffer Management
- Software Renderer
- Primitive Rendering
- Double Buffering
- Font Engine
- Image Engine
- SVG Rendering
- Theme Engine
- Widget Engine
- Animation Engine
- Diagnostics Framework
- Öffentliche Graphics API

Alle Komponenten arbeiten zusammen, ohne voneinander abhängig zu sein. Dadurch entsteht eine moderne, modulare und langfristig wartbare Grafikarchitektur.

---

# 332. Abschluss

Mit der Umsetzung dieser Spezifikation verfügt der Nova-Bootloader über eine vollständige, erweiterbare und plattformunabhängige Grafikplattform.

Sie bildet die Grundlage für:

- den grafischen Bootmanager,
- die Recovery-Oberfläche,
- Diagnosewerkzeuge,
- den späteren Kernel-Graphics-Layer,
- sowie das zukünftige Nova-Desktop-System.