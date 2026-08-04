# NPSPEC-BOOTCOMPOSITOR-0010
# Compositor Performance Limits

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTCOMPOSITOR-0010 |
| Titel | Compositor Performance Limits |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Compositor |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0001 bis NPSPEC-BOOTCOMPOSITOR-0009, NPSPEC-BOOTRENDER-0018 bis NPSPEC-BOOTRENDER-0020 |
| Zugehörige ADRs | ADR-BOOTPERF-0001, ADR-BOOTPERF-0002, ADR-BOOTPERF-0003, ADR-BOOTPERF-0004, ADR-BOOTPERF-0006 |

---

# 1. Ziel

Diese Spezifikation definiert sämtliche **Performancegrenzen (Performance Limits)** des NovaOS Boot Compositors.

Der Bootmanager muss auf unterschiedlichster Hardware eine flüssige und deterministische Darstellung gewährleisten.

Hierzu definiert diese Spezifikation:

- maximale Renderzeiten
- Speichergrenzen
- Surface-Limits
- Layer-Limits
- Blur-Limits
- Framebudgets
- Compositingbudgets
- automatische Qualitätsanpassungen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Compositor
- Rendering Engine
- Layer Manager
- Blur Engine
- Backdrop Processing
- Surface Manager
- Present Scheduler

Nicht Bestandteil sind:

- Kernel Scheduler
- GPU-Treiber
- Dateisystem

---

# 3. Architektur

```text
Frame

↓

Render Budget

↓

Compositor Budget

↓

Present Budget

↓

Display
```

---

# 4. Grundprinzip

Jeder Frame besitzt ein festes Zeitbudget.

Wird dieses Budget überschritten, muss der Compositor automatisch Maßnahmen zur Leistungsreduzierung einleiten.

Die Benutzeroberfläche muss dabei jederzeit vollständig bedienbar bleiben.

---

# 5. Performanceziele

| Parameter | Ziel |
|---|---:|
| Ziel-Framerate | 60 FPS |
| Framebudget | 16,67 ms |
| Renderbudget | ≤ 8 ms |
| Compositingbudget | ≤ 4 ms |
| Present | ≤ 2 ms |
| Reserve | ≥ 2 ms |

---

# 6. Minimalanforderungen

Auch auf sehr langsamer Hardware muss erreicht werden:

| Parameter | Mindestwert |
|---|---:|
| Framerate | 30 FPS |
| Framebudget | 33,3 ms |

---

# 7. Speicherbudget

Standard:

| Ressource | Limit |
|---|---:|
| Layer Surfaces | 32 MB |
| Blur Surfaces | 16 MB |
| Backdrop Surfaces | 16 MB |
| Image Cache | 16 MB |
| Render Buffer | 16 MB |

Die Grenzwerte können plattformabhängig angepasst werden.

---

# 8. Layer Limits

Standard:

| Objekt | Maximum |
|---|---:|
| Layer | 128 |
| Dialog Layer | 16 |
| Overlay Layer | 16 |
| Modal Layer | 8 |
| Debug Layer | 1 |

---

# 9. Surface Limits

Maximal zulässig:

- 256 aktive Surfaces
- 64 Offscreen-Surfaces
- 32 Blur-Surfaces
- 32 Backdrop-Surfaces

---

# 10. Damage Limits

Pro Frame:

- maximal 64 Damage Regions
- maximal 512 Merge-Operationen

Bei Überschreitung erfolgt automatisch Full Damage.

---

# 11. Blur Limits

Maximal gleichzeitig:

- 16 Blur-Operationen
- 32 px Blur-Radius
- 8 aktive Glass-Flächen

---

# 12. Shadow Limits

Maximal:

- 64 Schatten
- 16 Glows
- 2 Schattenebenen pro Objekt

---

# 13. Animation Limits

Maximal:

- 128 parallele Animationen
- 32 Materialanimationen
- 16 Bluranimationen

---

# 14. Render Commands

Empfohlen:

- maximal 10.000 Render Commands pro Frame

Bei Überschreitung dürfen Commands zusammengefasst werden.

---

# 15. Dirty Regions

Empfohlen:

- maximal 25 % der Bildschirmfläche

Wird dieser Wert überschritten, erfolgt Full Repaint.

---

# 16. Present

Der Present Scheduler darf maximal einen fertigen Frame puffern.

Frame-Staus sind unzulässig.

---

# 17. Qualitätsanpassung

Bei dauerhaft überschrittenem Framebudget werden schrittweise reduziert:

1. Glow
2. Shadowqualität
3. Blur Radius
4. Materialeffekte
5. Transparenz
6. Animationen

---

# 18. Speicherknappheit

Bei Speichermangel:

- Cache leeren
- Blur reduzieren
- Offscreen-Surfaces freigeben
- Qualitätsprofil reduzieren

Der Bootmanager muss weiterhin vollständig funktionieren.

---

# 19. Recovery

Kann das Zeitbudget dauerhaft nicht eingehalten werden:

```text
High

↓

Standard

↓

Performance

↓

Safe
```

Der Wechsel erfolgt automatisch.

---

# 20. Parallelisierung

Der Compositor muss vollständig auf einem CPU-Kern funktionieren.

Optionale Mehrkernoptimierungen dürfen das Ergebnis nicht verändern.

---

# 21. Determinismus

Die Laufzeit einzelner Algorithmen muss vorhersehbar sein.

Nicht zulässig sind:

- unbeschränkte Schleifen
- rekursive Renderdurchläufe
- unkontrollierte Speicherallokationen

---

# 22. Diagnostik

Das System überwacht kontinuierlich:

- Framezeit
- Renderzeit
- Compositingzeit
- Presentzeit
- Speicherverbrauch
- Surfaceanzahl
- Layeranzahl
- Cache-Nutzung

---

# 23. Fehlerbehandlung

Bei Grenzwertüberschreitungen werden erzeugt:

- Warnung
- Diagnoseeintrag
- Qualitätsanpassung

Kritische Überschreitungen dürfen niemals den Bootprozess abbrechen.

---

# 24. Sicherheit

Das System verhindert:

- Speicherüberläufe
- Surface-Leaks
- unendliche Renderzyklen
- doppelte Frame-Präsentationen
- beschädigte Caches

---

# 25. API

```c
void
nova_compositor_limits_initialize(void);

const nova_compositor_limits_t*
nova_compositor_limits_get(void);

void
nova_compositor_limits_update_statistics(
    const nova_frame_statistics_t* statistics);

bool
nova_compositor_limits_exceeded(void);

void
nova_compositor_limits_apply_fallback(void);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- durchschnittliche Framezeit
- maximale Framezeit
- Renderzeit
- Compositingzeit
- Presentzeit
- Layeranzahl
- Surfaceanzahl
- Bluranzahl
- Cache Hits
- Cache Misses
- aktive Qualitätsstufe

---

# 27. Testfälle

Zu testen sind:

- maximale Layeranzahl
- maximale Surfaceanzahl
- maximale Bluranzahl
- Speichergrenzen
- Renderbudget
- Framebudget
- Dirty-Region-Limit
- Qualitätswechsel
- Speichermangel
- Recovery

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- das Framebudget zuverlässig überwacht wird.
- automatische Qualitätsanpassungen funktionieren.
- sämtliche Ressourcenlimits eingehalten werden.
- Speichergrenzen korrekt behandelt werden.
- Diagnosewerte vollständig verfügbar sind.
- keine Heap-Allokationen im Render-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Ergebnisse liefern.

---

# 29. Referenzen

## ADRs

- ADR-BOOTPERF-0001 – Festes Speicherbudget für die Boot-UI
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath
- ADR-BOOTPERF-0003 – Adaptive Bildwiederholrate
- ADR-BOOTPERF-0004 – Automatische Effektdegradation
- ADR-BOOTPERF-0006 – Frame-Time-Messung und Diagnose

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTCOMPOSITOR-0009 – Surface Damage Tracking
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles
- NPSPEC-BOOTRENDER-0019 – Software Rendering Fallback
- NPSPEC-BOOTRENDER-0020 – Rendering Diagnostics
- NPSPEC-BOOTUI-0007 – Boot UI Memory Model

---

# 30. Zusammenfassung

Die **Compositor Performance Limits** definieren die verbindlichen Leistungsgrenzen des NovaOS Boot Compositors. Durch klar definierte Zeit-, Speicher- und Ressourcenbudgets, kontinuierliche Laufzeitüberwachung sowie automatische Qualitätsanpassungen gewährleistet das System eine flüssige, deterministische und stabile Darstellung der Bootoberfläche auf unterschiedlich leistungsfähiger Hardware. Moderne Grafiksysteme verfolgen ähnliche Strategien, indem sie Aktualisierungsbereiche begrenzen und Ressourcenbudgets überwachen, um eine gleichmäßige Bildausgabe sicherzustellen. :contentReference[oaicite:0]{index=0}