# NPSPEC-BOOTPERF-0002
# Frame-Time Budget

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTPERF-0002 |
| Titel | Frame-Time Budget |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Performance |
| Priorität | Kritisch |
| Gültigkeitsbereich | Gesamte NovaOS Boot UI Rendering Pipeline |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTPERF-0001, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTCOMPOSITOR-0001, NPSPEC-BOOTMOTION-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Frame-Time Budget** der NovaOS Bootoberfläche.

Das Ziel besteht darin, jedem Frame ein festes Zeitbudget zuzuweisen, sodass sämtliche Rendering-, Layout-, Animations- und Compositing-Prozesse innerhalb eines deterministischen Zeitfensters abgeschlossen werden.

Ein Überschreiten des Budgets ist als Performancefehler zu behandeln.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Rendering Engine
- Compositor
- Layout Engine
- Animation Engine
- UI Controls
- Text Engine
- SVG Renderer
- Dialogsystem
- Boot Manager
- Recovery
- Installer

---

# 3. Ziele

Das Frame-Time Budget muss:

- konstante Frametimes ermöglichen
- Jitter minimieren
- Eingabelatenzen reduzieren
- reproduzierbare Performance garantieren
- priorisierte Renderaufgaben unterstützen
- Diagnosen ermöglichen

---

# 4. Frame-Zeit

Sollwerte:

| Ziel | Zeit |
|------|------:|
| 60 FPS | 16,67 ms |
| 30 FPS | 33,33 ms |
| 20 FPS | 50,00 ms |

Die Bootoberfläche arbeitet standardmäßig mit einem Ziel von **60 FPS**, sofern die Hardware dies zulässt.

---

# 5. Budgetaufteilung

Maximales Budget pro Frame:

| Pipeline-Schritt | Budget |
|------------------|--------:|
| Eingabeverarbeitung | 0,5 ms |
| Ereignisverarbeitung | 0,5 ms |
| Layout | 2,0 ms |
| Animation | 2,0 ms |
| Textlayout | 1,5 ms |
| Rendering | 6,0 ms |
| Compositing | 2,0 ms |
| Präsentation | 1,5 ms |
| Reserve | 0,67 ms |

Gesamt:

```text
16,67 ms
```

---

# 6. Priorisierung

Renderaufgaben besitzen Prioritätsstufen:

- Kritisch
- Hoch
- Normal
- Niedrig

Kritische Aufgaben:

- Fokus
- Cursor
- Dialoge
- Fortschrittsanzeigen
- Fehlermeldungen

---

# 7. Überschreitung

Wird das Budget überschritten:

1. Diagnoseeintrag erzeugen
2. Ursache protokollieren
3. optionale Qualitätsreduzierung
4. Animationen anpassen
5. Dirty Regions optimieren

---

# 8. Soft Budget

Kurzzeitige Überschreitungen sind zulässig:

```text
≤ 20 ms
```

Sie dürfen jedoch nicht dauerhaft auftreten.

---

# 9. Hard Budget

Folgende Grenze darf niemals überschritten werden:

```text
33 ms
```

Ein Überschreiten entspricht einem sichtbaren Frameverlust.

---

# 10. Idle Frames

Im Leerlauf gilt:

- keine vollständigen Redraws
- keine Layoutberechnung
- keine Animationen
- ausschließlich Ereignisverarbeitung

---

# 11. Animation Budget

Animationen dürfen maximal beanspruchen:

```text
2 ms
```

Komplexere Animationen müssen:

- vereinfacht werden
- interpoliert werden
- adaptiv reduziert werden

---

# 12. Layout Budget

Layoutberechnung:

```text
≤ 2 ms
```

Das Layoutsystem darf ausschließlich geänderte Bereiche neu berechnen.

---

# 13. Rendering Budget

Rendering:

```text
≤ 6 ms
```

Das Budget umfasst:

- Geometrie
- Bilder
- Icons
- Texte
- Transparenz
- Schatten

---

# 14. Text Rendering Budget

Texte:

```text
≤ 1,5 ms
```

Glyph Atlas und Glyph Cache müssen verwendet werden.

---

# 15. Compositor Budget

Compositing:

```text
≤ 2 ms
```

Enthält:

- Ebenen
- Alpha-Blending
- Blur
- Glasmaterial
- Clipping

---

# 16. GPU Budget

Hardwarebeschleunigung soll bevorzugt werden.

Bei GPU-Auslastung dürfen:

- CPU-Layout
- Ressourcenvorbereitung
- Animationen

parallel vorbereitet werden.

---

# 17. Software Rendering

Bei Software Rendering gelten:

- mindestens 30 FPS
- deterministische Framezeiten
- identische Darstellung

Qualität darf reduziert werden, Funktionalität nicht.

---

# 18. Monitoring

Folgende Werte werden permanent überwacht:

- Framezeit
- Layoutzeit
- Renderzeit
- Compositingzeit
- CPU-Zeit
- GPU-Zeit
- Idle-Zeit

---

# 19. Performanceklassen

Es werden folgende Klassen definiert:

| Klasse | Bedeutung |
|---------|-----------|
| A | ≤16,67 ms |
| B | ≤20 ms |
| C | ≤25 ms |
| D | ≤33 ms |
| E | >33 ms |

Klasse A ist das Ziel.

---

# 20. Optimierungsmaßnahmen

Automatische Optimierungen:

- Dirty Region Reduction
- Animation Simplification
- Shadow Reduction
- Blur Reduction
- SVG Cache
- Glyph Cache
- Layer Caching

---

# 21. Speicherverwaltung

Während eines Frames sind verboten:

- Heap Allocation
- Heap Free
- Ressourcen-Neuladung
- Shader-Neuerstellung
- Font-Neuladen

---

# 22. API

```c
typedef struct
{
    uint32_t frame_time_us;
    uint32_t layout_time_us;
    uint32_t animation_time_us;
    uint32_t render_time_us;
    uint32_t compositor_time_us;
    uint32_t presentation_time_us;
}
nova_frame_budget_t;

nova_result_t
nova_frame_budget_initialize(void);

const nova_frame_budget_t*
nova_frame_budget_current(void);

bool
nova_frame_budget_exceeded(void);

void
nova_frame_budget_reset(void);
```

---

# 23. Diagnosedaten

Zu protokollieren sind:

- durchschnittliche Framezeit
- maximale Framezeit
- minimale Framezeit
- Layoutzeit
- Renderzeit
- Compositingzeit
- Budgetverletzungen
- Optimierungsmaßnahmen

---

# 24. Testfälle

Zu testen sind:

- 60 FPS
- 30 FPS
- Software Rendering
- GPU Rendering
- Themewechsel
- Dialoganimationen
- Glasmaterial
- SVG Rendering
- Text Rendering
- hohe Systemlast

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- die Ziel-Framezeit von **16,67 ms** auf unterstützter Hardware eingehalten wird.
- keine dauerhaften Budgetüberschreitungen auftreten.
- Layout, Rendering und Compositing innerhalb ihrer Teilbudgets abgeschlossen werden.
- Software Rendering mindestens **30 FPS** erreicht.
- alle Budgetüberschreitungen protokolliert werden.
- während eines Frames keine Heap-Allokationen erfolgen.
- automatische Optimierungsmaßnahmen bei Überlastung greifen.
- sämtliche Frame-Metriken über die Diagnose-API verfügbar sind.

---

# 26. Referenzen

## ADRs

- ADR-BOOTPERF-0002 – Frame-Time Budget
- ADR-BOOTRENDER-0001 – Rendering Engine
- ADR-BOOTCOMPOSITOR-0001 – Boot Compositor

## NPSPECs

- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTMOTION-0001 – Motion System

---

# 27. Zusammenfassung

Das **Frame-Time Budget** definiert die maximal zulässige Verarbeitungszeit eines einzelnen Frames innerhalb der NovaOS-Bootoberfläche. Durch feste Zeitbudgets für Eingabeverarbeitung, Layout, Animation, Rendering, Compositing und Präsentation wird eine deterministische und flüssige Darstellung mit einer Zielbildrate von 60 FPS erreicht. Budgetüberschreitungen werden automatisch erkannt, protokolliert und durch adaptive Optimierungsmechanismen kompensiert, ohne die funktionale Integrität der Benutzeroberfläche zu beeinträchtigen.