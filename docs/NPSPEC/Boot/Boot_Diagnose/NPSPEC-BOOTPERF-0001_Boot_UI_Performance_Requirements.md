# NPSPEC-BOOTPERF-0001
# Boot UI Performance Requirements

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTPERF-0001 |
| Titel | Boot UI Performance Requirements |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Performance |
| Priorität | Kritisch |
| Gültigkeitsbereich | Gesamte NovaOS Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTCOMPOSITOR-0001, NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTDESIGN-0001 |

---

# 1. Ziel

Diese Spezifikation definiert sämtliche Performance-Anforderungen für die grafische Bootoberfläche von NovaOS.

Die Boot UI muss bereits unmittelbar nach der Initialisierung der Grafikhardware flüssig, deterministisch und ohne wahrnehmbare Verzögerungen reagieren. Performance ist Bestandteil der Systemarchitektur und darf nicht von der verfügbaren Hardware abhängig sein.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Boot Dialoge
- Recovery
- Installer
- Self-Healing
- Diagnoseoberflächen
- Netzwerk-Boot
- Snapshot-Oberflächen
- sämtliche Boot Controls

---

# 3. Ziele

Die Bootoberfläche muss:

- deterministisch rendern
- flüssige Animationen ermöglichen
- minimale Speicherzugriffe verursachen
- unnötige CPU-Last vermeiden
- skalierbar sein
- reproduzierbare Laufzeiten besitzen

---

# 4. Performance-Grundsätze

Die Boot UI folgt folgenden Prinzipien:

- kein unnötiges Redrawing
- Dirty Region Rendering
- Double Buffering
- Ressourcen-Caching
- Objektpools
- Vorinitialisierung aller Ressourcen

---

# 5. Zielplattformen

Die Performance muss auf folgenden Plattformen gewährleistet sein:

- BIOS
- UEFI
- virtuelle Maschinen
- Embedded-Systeme
- x86
- x64
- ARM64

---

# 6. Startzeiten

Maximale Initialisierungszeiten:

| Komponente | Maximal |
|------------|----------|
| UI Engine | 30 ms |
| Theme laden | 10 ms |
| Ressourcen initialisieren | 20 ms |
| Erstes Fenster | 50 ms |
| Erster vollständiger Frame | 100 ms |

---

# 7. Rendering

Anforderungen:

- kontinuierliches Rendering
- deterministische Framezeiten
- keine Frame-Aussetzer
- keine sichtbaren Artefakte
- kein Flackern

---

# 8. Bildwiederholrate

Sollwerte:

| Betriebsart | Ziel |
|-------------|------|
| Animationen | ≥ 60 FPS |
| Idle | adaptiv |
| Fortschrittsanzeigen | ≥ 30 FPS |
| Software Rendering | ≥ 30 FPS |

---

# 9. Eingabelatenz

Maximale Eingabeverzögerung:

| Eingabe | Maximum |
|----------|----------|
| Tastatur | 10 ms |
| Maus | 10 ms |
| Touch | 16 ms |

---

# 10. Animationen

Animationen müssen:

- ruckelfrei sein
- keine Speicherallokationen verursachen
- Motion Tokens verwenden
- zeitlich deterministisch sein

---

# 11. Speicheranforderungen

Die UI verwendet:

- Objektpools
- Resource Cache
- Theme Cache
- Glyph Cache
- Icon Cache

Während der Darstellung sind dynamische Heap-Allokationen unzulässig.

---

# 12. CPU-Auslastung

Im Leerlauf:

- möglichst minimale CPU-Auslastung
- ereignisgesteuertes Rendering
- keine Busy-Wait-Schleifen

---

# 13. GPU-Unterstützung

Falls vorhanden:

- Hardwarebeschleunigung
- Alpha-Blending
- Skalierung
- Farbkonvertierung

Falls nicht verfügbar:

- vollständiger Software-Fallback

---

# 14. Ressourcenverwaltung

Alle Ressourcen werden:

- referenzgezählt
- wiederverwendet
- zentral verwaltet
- bei Nichtverwendung freigegeben

---

# 15. Layout Performance

Layoutberechnung:

- O(n)

Hit Testing:

- O(log n) oder besser

Control Lookup:

- O(1)

Theme Lookup:

- O(1)

---

# 16. Rendering Pipeline

Jeder Frame besteht aus:

1. Eingaben
2. Layout
3. Animation
4. Dirty Region Berechnung
5. Rendering
6. Präsentation

Die Reihenfolge darf nicht verändert werden.

---

# 17. Dirty Regions

Es werden ausschließlich geänderte Bereiche neu gezeichnet.

Vollständige Bildschirmaktualisierungen erfolgen nur:

- beim Themewechsel
- nach Auflösungsänderung
- nach explizitem Redraw

---

# 18. Ressourcenkompression

Unterstützt werden:

- PNG
- SVG
- komprimierte Assetpakete

Dekomprimierung erfolgt ausschließlich beim Laden.

---

# 19. Parallelisierung

Falls verfügbar dürfen parallel erfolgen:

- Ressourcenladen
- SVG-Rasterisierung
- Font-Initialisierung
- Animation-Vorbereitung

Die Darstellung eines Frames bleibt deterministisch.

---

# 20. Energieverbrauch

Die Bootoberfläche soll:

- CPU-Leerlauf unterstützen
- unnötige Aktualisierungen vermeiden
- Bildschirme bei Inaktivität dimmen können
- Animationen reduzieren können

---

# 21. Sicherheit

Performanceoptimierungen dürfen niemals:

- Integritätsprüfungen umgehen
- Sicherheitsprüfungen verzögern
- Renderingfehler verursachen
- Datenverlust ermöglichen

---

# 22. API

```c
typedef struct
{
    uint32_t fps;
    uint32_t frame_time_us;
    uint32_t draw_calls;
    uint32_t dirty_regions;
    uint64_t memory_usage;
}
nova_boot_perf_metrics_t;

nova_result_t
nova_boot_perf_initialize(void);

const nova_boot_perf_metrics_t*
nova_boot_perf_metrics(void);

nova_result_t
nova_boot_perf_reset(void);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- FPS
- Framezeit
- CPU-Auslastung
- Speicherverbrauch
- Anzahl Draw Calls
- Dirty Regions
- Renderzeit
- Layoutzeit

---

# 24. Testfälle

Zu testen sind:

- Kaltstart
- Warmstart
- Themewechsel
- Animationen
- große Auflösungen
- Software Rendering
- Hardwarebeschleunigung
- geringe Speicherausstattung
- hohe Eingabelast
- Recovery-Modus

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- die erste vollständige Benutzeroberfläche innerhalb von 100 ms nach der UI-Initialisierung dargestellt wird.
- Animationen mit mindestens 60 FPS (bzw. 30 FPS im Software-Rendering) dargestellt werden.
- sämtliche Eingaben innerhalb der definierten Latenz verarbeitet werden.
- während der Laufzeit keine dynamischen Heap-Allokationen erfolgen.
- ausschließlich Dirty Regions neu gerendert werden.
- sämtliche Ressourcen zentral gecacht werden.
- Hardware- und Software-Rendering identische Ergebnisse liefern.
- alle Performance-Metriken über die Diagnose-API verfügbar sind.

---

# 26. Referenzen

## ADRs

- ADR-BOOTPERF-0001 – Boot UI Performance Requirements
- ADR-BOOTRENDER-0001 – Rendering Engine
- ADR-BOOTCOMPOSITOR-0001 – Compositor
- ADR-BOOTRESOURCE-0001 – Resource Manager

## NPSPECs

- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTMOTION-0001 – Motion System
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language

---

# 27. Zusammenfassung

Die **Boot UI Performance Requirements** definieren sämtliche Leistungsanforderungen der grafischen NovaOS-Bootoberfläche. Sie legen deterministische Framezeiten, geringe Eingabelatenzen, konsequentes Ressourcen-Caching, Dirty-Region-Rendering, Double Buffering und den Verzicht auf Heap-Allokationen während der Laufzeit fest. Dadurch bleibt die Bootoberfläche auf BIOS-, UEFI-, virtuellen und eingebetteten Systemen gleichermaßen flüssig, vorhersagbar und ressourcenschonend.