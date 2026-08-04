# NPSPEC-BOOTUI-0005
# BIOS/UEFI Graphics Abstraction

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTUI-0005 |
| Titel | BIOS/UEFI Graphics Abstraction |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTUI-0001 bis NPSPEC-BOOTUI-0004 |
| ADRs | ADR-BOOTUI-0002, ADR-BOOTGFX-0001 bis ADR-BOOTGFX-0012 |

---

# 1. Ziel

Diese Spezifikation definiert die **einheitliche Grafik-Abstraktionsschicht (Graphics Abstraction Layer, GAL)** des NovaOS-Bootmanagers.

Sie kapselt sämtliche Unterschiede zwischen:

- BIOS + VBE (VESA BIOS Extensions)
- UEFI + GOP (Graphics Output Protocol)

sodass alle darüberliegenden Komponenten ausschließlich gegen eine gemeinsame API arbeiten.

Kein Bestandteil der Rendering-Engine, Layout-Engine oder Controls darf direkt auf BIOS- oder UEFI-spezifische Funktionen zugreifen.

Die Firmware-spezifischen Unterschiede werden vollständig innerhalb der Graphics Abstraction Layer verborgen. UEFI stellt dafür mit dem **Graphics Output Protocol (GOP)** einen standardisierten Zugriff auf Framebuffer, Videomodi und grundlegende Grafikfunktionen bereit; unter Legacy-BIOS übernimmt VBE diese Rolle. :contentReference[oaicite:0]{index=0}

---

# 2. Ziele der Abstraktionsschicht

Die Graphics Abstraction Layer (GAL) verfolgt folgende Ziele:

- vollständige Trennung zwischen Firmware und Rendering
- identische Rendering-Ergebnisse unter BIOS und UEFI
- identisches Verhalten aller Controls
- identische Animationen
- identische Layoutberechnung
- identische Eingabekoordinaten
- vollständige Austauschbarkeit des Grafik-Backends
- einfache Testbarkeit

---

# 3. Architektur

```
                  Boot UI
                     │
              Rendering Engine
                     │
          Graphics Abstraction Layer
        ┌────────────┴─────────────┐
BIOS Graphics Backend     UEFI Graphics Backend
        │                          │
       VBE                     GOP (Graphics Output Protocol)
        │                          │
     Framebuffer             Framebuffer
```

---

# 4. Verantwortlichkeiten

Die Graphics Abstraction Layer übernimmt ausschließlich:

- Initialisierung des Grafiksystems
- Auswahl eines Grafikmodus
- Bereitstellung des Framebuffers
- Pixelformat-Konvertierung
- Auflösungsinformationen
- DPI-Berechnung
- Present()
- Double Buffer
- Triple Buffer
- Hardwareinformationen
- Synchronisation

Nicht Bestandteil:

- Rendering
- Animation
- Layout
- Controls
- Navigation

---

# 5. Unterstützte Plattformen

## BIOS

Verwendet

```
VESA BIOS Extensions (VBE)
```

Eigenschaften

- Real Mode Initialisierung
- Protected Mode Rendering
- Linear Framebuffer
- VBE 2.0+
- VBE 3.0 bevorzugt

---

## UEFI

Verwendet

```
Graphics Output Protocol (GOP)
```

Eigenschaften

- Native UEFI
- Linear Framebuffer
- 32 Bit Farbtiefe
- Mehrere Grafikkarten möglich
- Native Auflösungen

GOP stellt dem Bootloader einen Framebuffer sowie Informationen zu Auflösung, Pixelformat und Zeilenabstand bereit und ist als Abstraktion für verschiedenste Grafik-Hardware konzipiert. :contentReference[oaicite:1]{index=1}

---

# 6. Architekturprinzip

Sämtliche Firmwareunterschiede enden innerhalb der Graphics Abstraction.

Die Rendering Engine kennt ausschließlich:

```cpp
GraphicsContext
```

und niemals

```cpp
EFI_GRAPHICS_OUTPUT_PROTOCOL
```

oder

```cpp
VBE_INFO_BLOCK
```

---

# 7. Graphics Context

Jede Plattform erzeugt denselben Kontext.

```cpp
struct GraphicsContext
{
    Width
    Height

    Pitch

    BitsPerPixel

    PixelFormat

    Framebuffer

    Backbuffer

    Present()

    Capabilities
}
```

---

# 8. Capabilities

Die Graphics Abstraction beschreibt sämtliche Fähigkeiten.

Beispiele

```cpp
SupportsAlpha

SupportsBlending

SupportsDoubleBuffer

SupportsTripleBuffer

SupportsHardwareBlit

SupportsLinearFramebuffer

SupportsPointer

SupportsNativeResolution
```

---

# 9. Pixel Formate

Intern arbeitet NovaOS ausschließlich mit

```
RGBA8888
```

Unterstützte Firmwareformate

```
RGB888

BGR888

ARGB8888

BGRA8888

RGB565
```

Nicht unterstützte Formate werden automatisch konvertiert.

---

# 10. Grafikmodus

Beim Start erfolgt:

```
Grafikmodus suchen
↓
Bewerten
↓
Besten Modus auswählen
↓
Framebuffer initialisieren
↓
Backbuffer erzeugen
↓
Context erstellen
```

---

# 11. Modusauswahl

```
Prioritäten

1. Native Auflösung
       ↓
2. 32 Bit
       ↓
3. Linear Framebuffer
       ↓
4. Größte verfügbare Auflösung
       ↓
5. 16 Bit als Fallback

```

---

# 12. Framebuffer

Der Graphics Context enthält

```
Front Buffer
↓
Back Buffer
↓
Optional
Triple Buffer
```

Present erfolgt ausschließlich über

```
GraphicsContext::Present()
```

---

# 13. Speicherlayout

```
Framebuffer
──────────────────────
Pixel
Pixel
Pixel
Pixel
──────────────
Pitch
──────────────
Pixel
Pixel
Pixel
```

Pitch kann größer sein als

```
Width * BytesPerPixel
```

Die Rendering Engine darf niemals eigene Annahmen treffen.

---

# 14. Auflösungsmodell

Unterstützt werden

```
640x480

800x600

1024x768

1280x720

1366x768

1600x900

1920x1080

2560x1440

3840x2160
```

Die Runtime muss dynamisch mit beliebigen weiteren Auflösungen umgehen können.

---

# 15. Koordinatensystem

Links oben

```
0,0
```

Rechts unten

```
Width-1

Height-1
```

Y wächst nach unten.

X wächst nach rechts.

Unter UEFI beschreibt GOP dasselbe Koordinatensystem (0,0 links oben; Y nach unten), wodurch BIOS- und UEFI-Backends identisch abstrahiert werden können. :contentReference[oaicite:2]{index=2}

---

# 16. DPI

```
Falls Firmware
DPI liefert
↓
verwenden
ansonsten
↓
Heuristik
↓
100 %
↓
125 %
↓
150 %
↓
175 %
↓
200 %
```
---

# 17. Backend API

Jedes Backend implementiert

```cpp
Initialize()

Shutdown()

QueryModes()

SetMode()

GetFramebuffer()

Present()

GetCapabilities()

GetResolution()
```

---

# 18. Fehlerbehandlung

Mögliche Fehler

```
Kein GOP

Kein VBE

Framebuffer ungültig

Pixelformat unbekannt

Speicherfehler

Mode nicht verfügbar
```

Recovery

```
anderen Modus wählen

↓

Fallback

↓

Textmodus
```

---

# 19. Synchronisation

Present

darf ausschließlich

am Ende eines Frames

erfolgen.

Kein direktes Schreiben

in den sichtbaren Framebuffer.

---

# 20. Mehrere Monitore

Falls Firmware mehrere Displays bereitstellt

↓

wird

ein

Primary Display

gewählt.

Sekundäre Displays

dürfen später aktiviert werden.

---

# 21. Hardwareinformationen

Backend liefert

```cpp
Vendor

Device

Framebuffer Address

Pitch

Pixel Format

Memory Size

Firmware Type
```

---

# 22. Sicherheitsanforderungen

Die Graphics Abstraction darf niemals

- ungültige Speicherbereiche verwenden
- außerhalb des Framebuffers schreiben
- Firmwaredaten verändern
- nicht validierte Modi übernehmen

Alle Zeiger müssen vor der Verwendung geprüft werden.

---

# 23. Performance

Initialisierung

< 50 ms

Moduswechsel

< 100 ms

Present

< 2 ms

Keine Heap-Allokationen während

```
Present()
```

---

# 24. Testanforderungen

Es müssen getestet werden

## BIOS

- VBE 2.0
- VBE 3.0

## UEFI

- GOP
- verschiedene Hersteller
- virtuelle Maschinen

## Pixelformate

- RGB
- BGR
- Alpha

## Auflösungen

- VGA
- HD
- Full HD
- QHD
- 4K

## Fehler

- ungültige Modi
- beschädigte Firmwaredaten
- fehlender Framebuffer

---

# 25. Referenzen

## ADR

- ADR-BOOTUI-0002 – Einheitliche UI-Architektur für BIOS und UEFI
- ADR-BOOTGFX-0001 – Lineares 32-Bit-RGBA-Farbformat
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTGFX-0005 – Offscreen-Surfaces

## NPSPEC

- NPSPEC-BOOTUI-0001 – Bootmanager UI Architecture
- NPSPEC-BOOTUI-0002 – Boot UI Runtime
- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor

---

# 26. Zusammenfassung

Die **BIOS/UEFI Graphics Abstraction Layer (GAL)** bildet die verbindliche Hardware-Abstraktionsschicht zwischen Firmware und Boot-UI. Sie vereinheitlicht BIOS/VBE und UEFI/GOP zu einer gemeinsamen API und stellt sicher, dass alle darüberliegenden Komponenten – Rendering, Layout, Controls, Animationen und Navigation – unabhängig von der Firmware identisch arbeiten. Durch die Trennung der Hardwarezugriffe von der Rendering-Logik bleibt die Bootoberfläche portabel, deterministisch, testbar und für zukünftige Grafik-Backends erweiterbar.