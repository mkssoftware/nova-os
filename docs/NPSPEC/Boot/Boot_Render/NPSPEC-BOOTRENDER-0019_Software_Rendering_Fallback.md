# NPSPEC-BOOTRENDER-0019
# Software Rendering Fallback

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0019 |
| Titel | Software Rendering Fallback |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0018 |
| Zugehörige ADRs | ADR-BOOTUI-0003, ADR-BOOTGFX-0011, ADR-BOOTPERF-0004, ADR-BOOTSAFE-0001, ADR-BOOTSAFE-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Software Rendering Fallback System** der NovaOS Boot Rendering Engine.

Das System stellt sicher, dass der Bootmanager unabhängig von Hardwarebeschleunigung oder Firmwareeinschränkungen jederzeit eine vollständig funktionsfähige grafische Benutzeroberfläche bereitstellen kann.

Der Software Renderer dient als:

- Standard-Renderer für BIOS-Systeme
- Fallback für UEFI-Systeme
- Diagnosemodus
- Recovery-Modus
- Referenzimplementierung der Rendering Engine

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Rendering-Komponenten der Bootoberfläche.

Der Software Renderer unterstützt:

- 2D-Primitiven
- Bilder
- Text
- Layer
- Transparenz
- Gradienten
- Schatten
- Blur (qualitätsabhängig)
- Animationen

Nicht Bestandteil sind:

- GPU-Rendering
- Hardwarebeschleunigung
- 3D-Funktionen

---

# 3. Architektur

```text
Boot UI Runtime
        │
        ▼
Rendering Engine
        │
        ▼
Software Renderer
        │
        ▼
Framebuffer Backend
        │
        ▼
BIOS / UEFI
```

---

# 4. Grundprinzip

Alle Zeichenoperationen werden ausschließlich durch die CPU ausgeführt.

Es werden keine GPU-APIs, Firmware-Beschleunigungsfunktionen oder proprietären Erweiterungen verwendet.

Dadurch bleibt das Rendering auf allen Plattformen reproduzierbar.

---

# 5. Einsatzbedingungen

Der Software Renderer wird verwendet wenn:

- keine Hardwarebeschleunigung verfügbar ist
- GOP oder VBE fehlschlagen
- der Safe Mode aktiv ist
- der Benutzer ihn erzwingt
- Diagnosefunktionen aktiv sind

---

# 6. Rendererfähigkeiten

Der Renderer unterstützt:

- Linien
- Rechtecke
- Rounded Rectangles
- Kreise
- Bilder
- Text
- Alpha-Blending
- Clipping
- Layer
- Dirty Regions
- Compositing

Alle Funktionen entsprechen der normalen Rendering Engine.

---

# 7. Renderpipeline

```text
Render Commands

↓

Scene Graph

↓

Software Rasterizer

↓

Compositor

↓

Framebuffer
```

---

# 8. Pixelpipeline

Für jedes Pixel erfolgt:

```text
Transformation

↓

Clipping

↓

Masking

↓

Alpha

↓

Compositing

↓

Framebuffer
```

Die Reihenfolge ist verbindlich.

---

# 9. Pixelformat

Intern wird verwendet:

```text
RGBA8888
Premultiplied Alpha
```

Andere Formate werden beim Laden konvertiert.

---

# 10. Speicher

Alle Renderpuffer werden beim Start reserviert.

Während des Render-Hotpaths dürfen keine Heap-Allokationen erfolgen.

---

# 11. Offscreen Surfaces

Der Renderer unterstützt:

- Backbuffer
- Blur Surface
- Overlay Surface
- Dialog Surface
- Image Surface

Die Anzahl richtet sich nach dem Speicherbudget.

---

# 12. Optimierungen

Folgende Optimierungen sind zulässig:

- SIMD
- Scanline Rendering
- Dirty Regions
- Region Caching
- Image Cache
- Gradient Cache

Die sichtbare Ausgabe darf sich dadurch nicht verändern.

---

# 13. SIMD

Falls verfügbar, dürfen SIMD-Erweiterungen genutzt werden:

- SSE2
- SSE4
- AVX2

Die Auswahl erfolgt zur Laufzeit.

Fehlt eine Erweiterung, muss automatisch auf die generische Implementierung zurückgefallen werden.

---

# 14. Multi-Core

Der Software Renderer muss vollständig ohne Mehrkernunterstützung funktionieren.

Optional darf später paralleles Tile Rendering ergänzt werden.

---

# 15. Qualitätsprofile

Alle Rendering Quality Profiles werden unterstützt.

Nicht verfügbare Effekte werden kontrolliert deaktiviert.

---

# 16. Fehlerbehandlung

Tritt während des Renderns ein Fehler auf:

```text
Renderfehler

↓

Renderer zurücksetzen

↓

Full Repaint

↓

Safe Renderer

↓

Textmodus
```

Der Bootprozess darf nicht abbrechen.

---

# 17. Speicherfehler

Bei Speichermangel werden:

- Blur deaktiviert
- Glows deaktiviert
- Schatten vereinfacht
- Qualitätsprofil reduziert

Die Bedienbarkeit muss erhalten bleiben.

---

# 18. Recovery

Der Renderer muss jederzeit neu initialisiert werden können.

Hierzu gehören:

- Freigabe temporärer Ressourcen
- Leeren der Caches
- erneute Initialisierung der Renderpuffer

---

# 19. Dirty Regions

Der Software Renderer unterstützt vollständiges Dirty-Region-Rendering.

Nur geänderte Bereiche werden neu berechnet.

---

# 20. Present

Der Renderer arbeitet mit:

- Double Buffering
- Partial Present
- Full Present

Die Auswahl erfolgt durch den Present Scheduler.

---

# 21. Animationen

Alle Animationen werden softwareseitig berechnet.

Die Bildqualität darf dabei nicht vom verwendeten Firmware-Backend abhängen.

---

# 22. Diagnostik

Der Renderer liefert:

- Framezeit
- Renderzeit
- Presentzeit
- Pixelanzahl
- Anzahl Zeichenoperationen
- Cache-Nutzung

Diese Daten werden an das Diagnoseframework übergeben.

---

# 23. Sicherheit

Das System verhindert:

- Nullpointerzugriffe
- Out-of-Bounds-Schreibzugriffe
- Integer Overflow
- beschädigte Renderpuffer
- doppelte Speicherfreigaben

Alle Eingabedaten müssen vor der Verarbeitung validiert werden.

---

# 24. API

```c
nova_result_t
nova_sw_renderer_initialize(void);

nova_result_t
nova_sw_renderer_shutdown(void);

nova_result_t
nova_sw_renderer_render_frame(
    const nova_scene_t* scene);

nova_result_t
nova_sw_renderer_present(void);

bool
nova_sw_renderer_is_available(void);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- aktiver Renderer
- SIMD-Version
- Framezeit
- Renderzeit
- Presentzeit
- Pixelanzahl
- Speicherverbrauch
- Cache Hits
- Cache Misses

---

# 26. Testfälle

Zu testen sind:

- vollständiges Rendering
- Dirty Regions
- Alpha-Blending
- Bilder
- Text
- Gradienten
- Blur
- Schatten
- Speichermangel
- Renderer-Neustart
- Safe Mode

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- der Bootmanager vollständig ohne GPU betrieben werden kann.
- sämtliche Renderingfunktionen verfügbar sind.
- der Renderer deterministisch arbeitet.
- Speicherfehler kontrolliert behandelt werden.
- der Bootprozess bei Renderfehlern nicht unterbrochen wird.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.
- keine Heap-Allokationen im Render-Hotpath stattfinden.

---

# 28. Referenzen

## ADRs

- ADR-BOOTUI-0003 – Software-Rendering als garantierter Fallback
- ADR-BOOTGFX-0011 – Qualitätsstufen für grafische Effekte
- ADR-BOOTPERF-0004 – Automatische Effektdegradation
- ADR-BOOTSAFE-0001 – Funktionaler Textmodus als letzter Fallback
- ADR-BOOTSAFE-0002 – UI-Fehler dürfen den Bootprozess nicht blockieren

## NPSPECs

- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0008 – Buffering and Frame Presentation
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles
- NPSPEC-BOOTUI-0008 – Boot UI Error Recovery

---

# 29. Zusammenfassung

Das **Software Rendering Fallback System** garantiert eine vollständig funktionsfähige grafische Benutzeroberfläche unabhängig von Hardwarebeschleunigung oder Firmwarefähigkeiten. Durch deterministische CPU-basierte Rasterisierung, vollständige Unterstützung der Rendering Engine, integrierte Fehlerbehandlung und adaptive Qualitätsprofile bleibt der NovaOS Bootmanager auf allen unterstützten Plattformen zuverlässig, stabil und visuell konsistent.