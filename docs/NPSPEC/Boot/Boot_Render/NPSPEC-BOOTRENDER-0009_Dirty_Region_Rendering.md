# NPSPEC-BOOTRENDER-0009
# Dirty Region Rendering

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0009 |
| Titel | Dirty Region Rendering |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0008 |
| Zugehörige ADRs | ADR-BOOTUI-0007, ADR-BOOTGFX-0004, ADR-BOOTPERF-0002, ADR-BOOTPERF-0003 |

---

# 1. Ziel

Diese Spezifikation definiert das **Dirty Region Rendering** der NovaOS Boot Rendering Engine.

Anstatt bei jeder Änderung den gesamten Bildschirm neu zu rendern, werden ausschließlich die Bereiche aktualisiert, deren sichtbarer Inhalt verändert wurde.

Das Verfahren reduziert:

- CPU-Auslastung
- Speicherbandbreite
- Present-Zeit
- Energieverbrauch
- Anzahl der Pixeloperationen

ohne die Konsistenz der Benutzeroberfläche zu beeinträchtigen.

---

# 2. Geltungsbereich

Das Dirty-Region-System wird von sämtlichen grafischen Komponenten verwendet.

Dazu gehören:

- Fenster
- Dialoge
- Buttons
- Listen
- Animationen
- Mauszeiger
- Blur-Effekte
- Overlays
- Statusanzeigen

Nicht Bestandteil sind:

- vollständiges Rendering
- Layerverwaltung
- Framebuffer Backend
- Scene Graph

---

# 3. Architektur

```text
UI Änderung
      │
      ▼
Element markiert Dirty
      │
      ▼
Dirty Manager
      │
      ▼
Regionen zusammenführen
      │
      ▼
Renderer
      │
      ▼
Present
```

---

# 4. Grundprinzip

Eine Region gilt als **Dirty**, wenn sich ihr sichtbarer Inhalt geändert hat.

Gründe können sein:

- Positionsänderung
- Größenänderung
- Farbänderung
- Textänderung
- Sichtbarkeitsänderung
- Animation
- Transparenzänderung
- Blur-Änderung
- Mausbewegung
- Fokuswechsel

---

# 5. Region

Eine Dirty Region wird durch ein Rechteck beschrieben.

```c
typedef struct
{
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} nova_rect_t;
```

Alle Koordinaten beziehen sich auf den logischen Bildschirm.

---

# 6. Damage Set

Mehrere Dirty Regions werden in einem Damage Set gesammelt.

```c
typedef struct
{
    nova_rect_t regions[64];
    uint32_t count;
    bool full_damage;
} nova_damage_set_t;
```

`full_damage` erzwingt ein vollständiges Rendering.

---

# 7. Regionserzeugung

Eine Region wird erzeugt, wenn:

- ein Control invalidiert wird
- ein Layer verändert wird
- eine Animation läuft
- ein Dialog erscheint
- ein Dialog verschwindet
- ein Tooltip angezeigt wird
- ein Effekt geändert wird

---

# 8. Invalidation

Jedes UI-Objekt besitzt eine Invalidate-Funktion.

```c
void
nova_ui_invalidate(
    nova_rect_t bounds
);
```

Sie fügt die Region dem Dirty Manager hinzu.

---

# 9. Zusammenführen

Überlappende Regionen sollen zusammengeführt werden.

Beispiel:

```text
┌─────┐
│ A   │
└──┐  │
   │B │
   └──┘
```

↓

```text
┌──────────┐
│          │
└──────────┘
```

Dadurch sinkt die Anzahl der Renderoperationen.

---

# 10. Teilung

Sehr große Regionen dürfen aufgeteilt werden.

Gründe:

- Cachefreundlichkeit
- SIMD-Optimierung
- Backendgrenzen

---

# 11. Begrenzung

Alle Regionen müssen auf die Bildschirmgröße begrenzt werden.

```text
0 ≤ Left < Width
0 ≤ Top < Height
0 ≤ Right ≤ Width
0 ≤ Bottom ≤ Height
```

Ungültige Regionen werden verworfen.

---

# 12. Leere Regionen

Regionen mit:

```text
Width <= 0

oder

Height <= 0
```

werden ignoriert.

---

# 13. Duplikate

Identische Regionen dürfen nur einmal gespeichert werden.

---

# 14. Maximale Anzahl

Die Runtime unterstützt standardmäßig:

```text
64 Dirty Regions
```

Der Wert muss konfigurierbar sein.

---

# 15. Full Damage

Ein vollständiges Rendering wird ausgelöst bei:

- Auflösungswechsel
- Themewechsel
- Recovery
- Initialisierung
- Speicherfehler
- zu vielen Regionen

---

# 16. Schwellenwert

Falls mehr als 50 % der Bildschirmfläche Dirty sind, darf automatisch auf Full Damage umgeschaltet werden.

Der Grenzwert ist konfigurierbar.

---

# 17. Regionenberechnung

Die Fläche berechnet sich als:

```text
Area =
Width × Height
```

Dabei sind Integer-Overflows zu verhindern.

---

# 18. Clipping

Vor dem Rendering wird jede Region geclippt.

```text
Dirty Region
        │
        ▼
Clip gegen Bildschirm
        │
        ▼
Clip gegen Layer
        │
        ▼
Clip gegen Surface
```

---

# 19. Layerabhängigkeit

Eine Dirty Region betrifft nur Layer, die sie tatsächlich schneiden.

Nicht betroffene Layer werden übersprungen.

---

# 20. Animationen

Animationen markieren nur ihre tatsächlich veränderten Bereiche.

Eine vollständige Bildschirmaktualisierung ist unzulässig.

---

# 21. Mauszeiger

Der Mauszeiger erzeugt zwei Dirty Regions:

- alte Position
- neue Position

Dadurch werden lediglich diese Bereiche neu gerendert.

---

# 22. Blur

Ändert sich ein Blur-Hintergrund, muss zusätzlich der Blur-Radius berücksichtigt werden.

Beispiel:

```text
Dirty +
Blur Radius
=
Render Region
```

---

# 23. Schatten

Schatten vergrößern die Dirty Region.

Die Erweiterung erfolgt entsprechend den Shadow-Metriken.

---

# 24. Transparenz

Alpha-Blending kann benachbarte Pixel beeinflussen.

Die Dirty Region ist entsprechend zu erweitern.

---

# 25. Present

Nach dem Rendering wird ausschließlich das Damage Set präsentiert.

Nicht betroffene Bildschirmbereiche bleiben unverändert.

---

# 26. Scheduler

Der Present Scheduler entscheidet:

- Full Present
- Partial Present

auf Basis des Damage Sets.

---

# 27. Speicher

Die Regionen werden in einem vorreservierten Speicher verwaltet.

Heap-Allokationen während des Renderings sind verboten.

---

# 28. Performance

Anforderungen:

- Merge < 100 µs
- Clip < 50 µs
- Region hinzufügen < 20 µs
- Keine linearen Vollbildoperationen bei kleinen Änderungen

---

# 29. Sicherheit

Das System verhindert:

- ungültige Rechtecke
- Speicherüberläufe
- Integer Overflow
- doppelte Regionen
- Out-of-Bounds-Koordinaten

---

# 30. API

```c
void
nova_dirty_initialize(void);

void
nova_dirty_clear(void);

void
nova_dirty_add(
    const nova_rect_t* rect);

bool
nova_dirty_is_full(void);

const nova_damage_set_t*
nova_dirty_get(void);
```

---

# 31. Diagnosedaten

Zu erfassen sind:

- Anzahl Dirty Regions
- Merge-Operationen
- Clip-Operationen
- Full Damage
- Partial Damage
- durchschnittliche Regionengröße
- maximale Regionengröße

---

# 32. Testfälle

Zu testen sind:

- einzelne Region
- mehrere Regionen
- überlappende Regionen
- identische Regionen
- Clipping
- Full Damage
- Mausbewegung
- Animation
- Blur
- Schatten

---

# 33. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- ausschließlich geänderte Bereiche neu gerendert werden.
- Regionen korrekt zusammengeführt werden.
- Full Damage automatisch aktiviert werden kann.
- Clipping fehlerfrei arbeitet.
- keine Out-of-Bounds-Zugriffe entstehen.
- Heap-Allokationen im Render-Hotpath vermieden werden.
- BIOS und UEFI identisches Verhalten zeigen.

---

# 34. Referenzen

## ADRs

- ADR-BOOTUI-0007 – Dirty-Region-Rendering
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath
- ADR-BOOTPERF-0003 – Adaptive Bildwiederholrate

## NPSPECs

- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0004 – Layer and Surface Management
- NPSPEC-BOOTRENDER-0005 – Framebuffer Backend
- NPSPEC-BOOTRENDER-0008 – Buffering and Frame Presentation

---

# 35. Zusammenfassung

Das **Dirty Region Rendering** reduziert die Anzahl der zu rendernden Pixel erheblich, indem ausschließlich geänderte Bildschirmbereiche aktualisiert werden. Durch Regionenzusammenführung, Clipping und die enge Integration mit dem Present Scheduler wird eine flüssige und effiziente Darstellung der NovaOS Bootoberfläche erreicht. Das Verfahren ist vollständig plattformunabhängig und funktioniert identisch auf BIOS- und UEFI-Systemen.