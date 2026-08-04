# NPSPEC-BOOTRENDER-0008
# Buffering and Frame Presentation

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTRENDER-0008 |
| Titel | Buffering and Frame Presentation |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Rendering |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0007 |
| Zugehörige ADRs | ADR-BOOTUI-0005, ADR-BOOTGFX-0004, ADR-BOOTPERF-0002, ADR-BOOTPERF-0003 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige Buffering- und Präsentationssystem der NovaOS Boot Rendering Engine.

Sie beschreibt:

- Bufferverwaltung
- Frame-Lebenszyklus
- Double- und Triple-Buffering
- Present-Strategien
- Synchronisation
- Frame-Scheduling
- Dirty-Region-Übertragung
- Page-Flipping
- Frame-Timing
- Recovery bei Present-Fehlern

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche grafischen Ausgaben des Bootmanagers.

Sie umfasst:

- BIOS VBE
- UEFI GOP
- Software Renderer
- Safe Mode
- Test Renderer

Nicht Bestandteil sind:

- Rendering
- Compositing
- Animationen
- Scene Graph
- UI Layout

---

# 3. Architektur

```text
Scene Graph
      │
      ▼
Render Commands
      │
      ▼
Rendering Engine
      │
      ▼
Backbuffer
      │
      ▼
Present Scheduler
      │
      ▼
Framebuffer Backend
      │
      ▼
Display
```

---

# 4. Ziele

Das System muss:

- flimmerfreie Darstellung gewährleisten
- Tearing vermeiden
- deterministisch arbeiten
- geringe Speicherbelegung besitzen
- Frameverluste erkennen
- Present-Zeiten messen
- auf BIOS und UEFI identisch funktionieren

---

# 5. Bufferarten

Die Runtime kennt folgende Buffer:

- Frontbuffer
- Backbuffer
- Renderbuffer
- Offscreen Surface
- Overlay Surface
- Blur Surface
- Dialog Surface

Jeder Buffer besitzt einen definierten Eigentümer.

---

# 6. Frontbuffer

Der Frontbuffer entspricht dem sichtbaren Framebuffer.

Eigenschaften:

- ausschließlich Ausgabe
- niemals direkt rendern
- nur über Present verändern
- firmwareabhängig

---

# 7. Backbuffer

Der Backbuffer enthält den fertig gerenderten Frame.

Eigenschaften:

- vollständige Szene
- nicht sichtbar
- Renderziel
- atomare Übergabe an Present

---

# 8. Triple Buffer

Optional darf ein dritter Buffer verwendet werden.

```text
Render Buffer
      │
      ▼
Pending Buffer
      │
      ▼
Frontbuffer
```

Triple Buffering darf nur aktiviert werden, wenn ausreichend Speicher vorhanden ist.

---

# 9. Bufferzustände

```c
typedef enum
{
    NOVA_BUFFER_FREE,
    NOVA_BUFFER_RENDERING,
    NOVA_BUFFER_READY,
    NOVA_BUFFER_PRESENTING,
    NOVA_BUFFER_DISPLAYED
} nova_buffer_state_t;
```

Ungültige Zustandswechsel sind zu verhindern.

---

# 10. Frame Lifecycle

```text
Frame erzeugen
      │
      ▼
Rendern
      │
      ▼
Compositing
      │
      ▼
Present vorbereiten
      │
      ▼
Framebuffer aktualisieren
      │
      ▼
Frame abgeschlossen
```

---

# 11. Double Buffering

Standardverfahren:

```text
Backbuffer rendern
      │
      ▼
Present
      │
      ▼
Frontbuffer sichtbar
```

Während des Renderns bleibt der Frontbuffer unverändert.

---

# 12. Triple Buffering

Optional:

```text
Buffer A
Buffer B
Buffer C
```

Der Present Scheduler verwaltet die Übergänge.

---

# 13. Present Scheduler

Der Scheduler entscheidet:

- wann präsentiert wird
- welcher Buffer sichtbar wird
- ob Partial Present möglich ist
- ob Full Present erforderlich ist

---

# 14. Presentarten

Unterstützt werden:

- Full Present
- Partial Present
- Forced Present

---

# 15. Full Present

Der gesamte Backbuffer wird übertragen.

Verwendung:

- erste Darstellung
- Größenänderung
- Themewechsel
- Recovery
- vollständige Invalidierung

---

# 16. Partial Present

Nur Dirty Regions werden kopiert.

Voraussetzungen:

- gültige Regionen
- identisches Pixelformat
- kompatibler Backendpfad

---

# 17. Dirty Regions

```c
typedef struct
{
    nova_rect_t regions[64];
    uint32_t count;
} nova_damage_set_t;
```

Überlappende Regionen sollen zusammengeführt werden.

---

# 18. Present Pipeline

```text
Backbuffer
      │
      ▼
Damage Analyse
      │
      ▼
Backend
      │
      ▼
Framebuffer
```

---

# 19. Synchronisation

Während eines Presents darf:

- kein Rendern beginnen
- kein Buffer freigegeben werden
- kein Moduswechsel erfolgen

---

# 20. Present Lock

Das System besitzt einen globalen Present Lock.

Ziel:

- atomare Frameübergabe
- konsistente Darstellung

---

# 21. Page Flipping

Falls unterstützt:

```text
Buffer A sichtbar

↓

Buffer B sichtbar
```

Falls nicht verfügbar, erfolgt eine Speicherkopie.

---

# 22. Frame Timing

Jeder Frame speichert:

- Startzeit
- Endzeit
- Renderdauer
- Presentdauer

---

# 23. Zielwerte

| Operation | Ziel |
|---|---:|
| Present | < 2 ms |
| Bufferwechsel | < 50 µs |
| Dirty Analyse | < 100 µs |

---

# 24. VSync

Das System unterstützt:

- echtes VSync
- emuliertes VSync
- deaktiviertes VSync

Falls keine Synchronisation möglich ist, wird direkt präsentiert.

---

# 25. Adaptive Strategie

Der Scheduler darf automatisch wählen:

- Full Present
- Partial Present
- Triple Buffer
- Double Buffer

Abhängig von:

- Dirty Area
- Speicher
- Backend
- Performance

---

# 26. Speicher

Buffer werden beim Start reserviert.

Während des Render-Hotpaths sind keine Heap-Allokationen zulässig.

---

# 27. Recovery

Bei Presentfehler:

```text
Present Fehler
      │
      ▼
Full Present
      │
      ▼
Software Renderer
      │
      ▼
Safe Mode
```

---

# 28. Sicherheit

Das System verhindert:

- gleichzeitiges Schreiben
- Buffer Overrun
- Use-after-Free
- ungültige Bufferzustände
- ungültige Pointer

---

# 29. API

```c
nova_result_t
nova_present_initialize(void);

nova_result_t
nova_present_shutdown(void);

nova_result_t
nova_present_frame(
    nova_damage_set_t* damage);

nova_result_t
nova_present_full(void);

nova_result_t
nova_present_partial(
    nova_damage_set_t* damage);

bool
nova_present_is_busy(void);
```

---

# 30. Testfälle

Zu testen sind:

- Double Buffer
- Triple Buffer
- Full Present
- Partial Present
- Dirty Regions
- Bufferwechsel
- Recovery
- Safe Mode
- BIOS
- UEFI

---

# 31. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- kein sichtbares Flimmern auftritt.
- kein Tearing entsteht.
- Double Buffering standardmäßig aktiv ist.
- Triple Buffering optional funktioniert.
- Partial Present korrekt arbeitet.
- Full Present jederzeit möglich ist.
- Bufferzustände konsistent bleiben.
- Fehler den Bootprozess nicht blockieren.
- BIOS und UEFI identisches Verhalten zeigen.

---

# 32. Referenzen

## ADRs

- ADR-BOOTUI-0005 – Double- und Triple-Buffering
- ADR-BOOTGFX-0004 – Ebenenbasiertes Compositing
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath
- ADR-BOOTPERF-0003 – Adaptive Bildwiederholrate

## NPSPECs

- NPSPEC-BOOTRENDER-0001 – 2D Rendering Engine
- NPSPEC-BOOTRENDER-0004 – Layer and Surface Management
- NPSPEC-BOOTRENDER-0005 – Framebuffer Backend
- NPSPEC-BOOTRENDER-0006 – UEFI GOP Backend
- NPSPEC-BOOTRENDER-0007 – BIOS VBE Backend

---

# 33. Zusammenfassung

Das **Buffering and Frame Presentation System** stellt sicher, dass sämtliche grafischen Inhalte des NovaOS Bootmanagers flimmerfrei, deterministisch und plattformunabhängig dargestellt werden. Durch Double-Buffering als Standard, optionales Triple-Buffering, Dirty-Region-Rendering und einen zentralen Present Scheduler wird eine hochwertige Darstellung erreicht, ohne den Bootprozess zu verzögern.