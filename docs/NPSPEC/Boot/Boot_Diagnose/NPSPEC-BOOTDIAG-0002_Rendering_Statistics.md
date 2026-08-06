# NPSPEC-BOOTDIAG-0002
# Rendering Statistics

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTDIAG-0002 |
| Titel | Rendering Statistics |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Diagnostics |
| Priorität | Hoch |
| Gültigkeitsbereich | Rendering Engine, Compositor, Motion Engine, Text Engine, Resource Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTDIAG-0001, NPSPEC-BOOTPERF-0001, NPSPEC-BOOTPERF-0002, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTCOMPOSITOR-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die Erfassung und Bereitstellung sämtlicher **Rendering Statistics** der NovaOS Bootoberfläche.

Das Statistiksystem sammelt kontinuierlich Leistungs-, Speicher- und Renderingmetriken der gesamten grafischen Pipeline. Die Daten dienen ausschließlich der Diagnose, Performanceanalyse und Optimierung der Bootoberfläche.

Die Erfassung darf den eigentlichen Bootvorgang nicht messbar beeinträchtigen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Rendering Engine
- Software Renderer
- GPU Renderer
- Compositor
- Motion Engine
- Text Engine
- Resource Manager
- Theme Engine
- Boot Manager
- Recovery
- Installer

---

# 3. Ziele

Das Statistiksystem muss:

- sämtliche Renderingmetriken erfassen
- reproduzierbare Messungen liefern
- geringe Laufzeitkosten besitzen
- deterministisch arbeiten
- modular erweiterbar sein
- exportierbar sein

---

# 4. Architektur

Das Statistiksystem besteht aus:

- Statistics Collector
- Frame Monitor
- Performance Counter
- Resource Counter
- Diagnostics Interface
- Export Service

Alle Komponenten arbeiten unabhängig voneinander.

---

# 5. Erfasste Kennzahlen

Mindestens folgende Werte werden kontinuierlich erfasst:

- FPS
- durchschnittliche Framezeit
- minimale Framezeit
- maximale Framezeit
- Renderzeit
- Layoutzeit
- Compositingzeit
- Präsentationszeit
- Idle-Zeit

---

# 6. Frame-Statistiken

Für jeden Frame werden gespeichert:

- Frame-ID
- Startzeit
- Endzeit
- Gesamtdauer
- Dirty Regions
- Draw Calls
- Layeranzahl

Optional können Einzelmessungen verworfen werden, um Speicher zu sparen.

---

# 7. Renderingmetriken

Zu erfassen sind:

- gezeichnete Rechtecke
- Linien
- Kreise
- Pfade
- Bilder
- SVGs
- Glyphen
- Farbverläufe
- Schatten
- Blur-Effekte

---

# 8. Draw Calls

Folgende Werte werden gespeichert:

- Gesamtanzahl
- durchschnittliche Anzahl
- maximale Anzahl
- Draw Calls pro Layer
- Draw Calls pro Fenster

---

# 9. Dirty Regions

Erfasst werden:

- Anzahl
- Gesamtfläche
- durchschnittliche Fläche
- maximale Fläche
- Verhältnis zur Bildschirmfläche

---

# 10. Layerstatistiken

Für jede Ebene:

- Layer-ID
- Sichtbarkeit
- Renderzeit
- Transparenz
- Speicherverbrauch

---

# 11. Textstatistiken

Zu erfassen sind:

- gerenderte Glyphen
- Cache-Hits
- Cache-Misses
- Rasterisierungen
- Fontwechsel
- Textlayoutdauer

---

# 12. SVG-Statistiken

Erfasst werden:

- gerenderte SVGs
- Cache-Hits
- Cache-Misses
- Rasterisierungszeit
- Transformationszeit

---

# 13. Ressourcenstatistiken

Folgende Werte werden überwacht:

- Resource Cache
- Theme Cache
- Glyph Cache
- SVG Cache
- Animation Cache

Für jeden Cache:

- Größe
- Auslastung
- Trefferquote
- Freigaben

---

# 14. GPU-Statistiken

Falls Hardwarebeschleunigung verfügbar ist:

- GPU-Auslastung
- GPU-Zeit
- Transferzeit
- GPU-Speicher
- GPU-Cache

Bei Software Rendering werden diese Werte als "nicht verfügbar" gekennzeichnet.

---

# 15. Speicherstatistiken

Zu erfassen sind:

- Gesamtspeicher
- belegter Speicher
- Cache-Speicher
- Objektpools
- temporärer Speicher
- Spitzenverbrauch

---

# 16. Animationsstatistiken

Erfasst werden:

- aktive Animationen
- abgeschlossene Animationen
- Animationsdauer
- verworfene Animationen
- Motion-Tokens

---

# 17. Zeitfenster

Statistiken werden bereitgestellt für:

- aktueller Frame
- letzte Sekunde
- letzte 10 Sekunden
- gesamte Bootsitzung

---

# 18. Performanceklassen

Die Statistik klassifiziert automatisch:

| Klasse | Framezeit |
|---------|-----------|
| A | ≤16,67 ms |
| B | ≤20 ms |
| C | ≤25 ms |
| D | ≤33 ms |
| E | >33 ms |

---

# 19. Export

Exportformate:

- Nova Diagnostic Format
- JSON
- CSV
- Binärformat

Der Export erfolgt ausschließlich auf Benutzeranforderung.

---

# 20. Sicherheit

Die Rendering Statistics enthalten keine:

- Benutzerdaten
- Passwörter
- kryptographischen Schlüssel
- TPM-Daten

Diagnoseinformationen sind ausschließlich lesend.

UEFI empfiehlt, zeitaufwendige Diagnosen nicht im normalen Bootpfad auszuführen, sondern über dedizierte Diagnosemechanismen bereitzustellen, um kurze Bootzeiten zu erhalten. :contentReference[oaicite:0]{index=0}

---

# 21. Speicherverwaltung

Das Statistiksystem verwendet:

- Ring Buffer
- Counter Pool
- Statistics Cache
- Export Buffer

Alle Speicherbereiche werden vorallokiert.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 22. API

```c
typedef struct
{
    uint32_t fps;
    uint32_t frame_count;
    uint32_t draw_calls;
    uint32_t dirty_regions;
    uint32_t rendered_glyphs;
    uint32_t rendered_images;
    uint32_t rendered_svgs;
    uint64_t frame_time_us;
    uint64_t render_time_us;
    uint64_t layout_time_us;
    uint64_t compositor_time_us;
}
nova_render_statistics_t;

nova_result_t
nova_render_statistics_initialize(void);

const nova_render_statistics_t*
nova_render_statistics_current(void);

const nova_render_statistics_t*
nova_render_statistics_average(void);

nova_result_t
nova_render_statistics_export(void);

void
nova_render_statistics_reset(void);
```

---

# 23. Diagnosedaten

Zu protokollieren sind:

- FPS
- Framezeiten
- Draw Calls
- Dirty Regions
- Layeranzahl
- Speicherverbrauch
- Cache-Hits
- Cache-Misses
- GPU-Auslastung
- CPU-Auslastung
- Renderzeit
- Layoutzeit

---

# 24. Testfälle

Zu testen sind:

- 60 FPS Rendering
- 30 FPS Rendering
- Software Renderer
- GPU Renderer
- Themewechsel
- SVG Rendering
- große Glyph-Atlanten
- Cache-Evictions
- Exportfunktion
- Langzeiterfassung

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Renderingmetriken kontinuierlich erfasst werden.
- die Erfassung den Bootvorgang nicht messbar verlangsamt.
- alle Messwerte über die Diagnostics API verfügbar sind.
- Export in alle unterstützten Formate möglich ist.
- sämtliche Caches und Draw Calls überwacht werden.
- GPU- und Software-Rendering gleichermaßen unterstützt werden.
- keine Heap-Allokationen während der Erfassung stattfinden.
- sämtliche Statistiken deterministisch reproduzierbar sind.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIAG-0002 – Rendering Statistics
- ADR-BOOTPERF-0001 – Boot Performance Requirements
- ADR-BOOTRENDER-0001 – Rendering Engine

## NPSPECs

- NPSPEC-BOOTDIAG-0001 – Boot UI Diagnostics
- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements
- NPSPEC-BOOTPERF-0002 – Frame-Time Budget
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor

---

# 27. Zusammenfassung

Die **Rendering Statistics** bilden das zentrale Telemetrie- und Statistiksystem der NovaOS-Renderingpipeline. Sie erfassen kontinuierlich Framezeiten, FPS, Draw Calls, Dirty Regions, Cache-Auslastung, Speicherverbrauch sowie GPU- und CPU-Metriken und stellen diese über eine standardisierte Diagnoseschnittstelle bereit. Durch vorallokierte Speicherstrukturen, deterministische Messverfahren und eine modulare Architektur liefert das System reproduzierbare Leistungsdaten, ohne die Performance oder Stabilität der Bootoberfläche zu beeinträchtigen.