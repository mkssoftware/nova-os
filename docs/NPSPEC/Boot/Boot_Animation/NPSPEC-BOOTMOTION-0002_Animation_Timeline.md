# NPSPEC-BOOTMOTION-0002
# Animation Timeline

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0002 |
| Titel | Animation Timeline |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 |
| Zugehörige ADRs | ADR-BOOTMOTION-0001, ADR-BOOTMOTION-0002, ADR-BOOTMOTION-0003, ADR-BOOTMOTION-0006 |

---

# 1. Ziel

Diese Spezifikation definiert die **Animation Timeline** der NovaOS Boot UI.

Die Animation Timeline bildet die zentrale Zeitbasis sämtlicher Animationen und sorgt dafür, dass Bewegungen unabhängig von der Bildrate identisch ablaufen.

Sie verwaltet:

- Startzeiten
- Endzeiten
- Laufzeiten
- Verzögerungen
- Wiederholungen
- Keyframes
- Übergänge
- Synchronisation

Alle Animationen verwenden dieselbe globale Timeline. Moderne Animationssysteme basieren ebenfalls auf einer gemeinsamen Zeitachse, an die einzelne Animationen gekoppelt werden. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Animationen des Bootmanagers.

Sie umfasst:

- Dialoge
- Menüs
- Controls
- Panels
- Glass Material
- Acrylic Material
- Overlay Layer
- Cursoranimationen

Nicht Bestandteil sind:

- Audio
- Splashvideo
- Kernelanimationen

---

# 3. Architektur

```text
Boot Timer
      │
      ▼
Global Timeline
      │
      ▼
Animation Scheduler
      │
      ▼
Animation Instances
      │
      ▼
Property Animator
      │
      ▼
Compositor
```

---

# 4. Grundprinzip

Alle Animationen beziehen ihre Zeit ausschließlich von der globalen Timeline.

Die Timeline kennt keine Frames.

Sie arbeitet ausschließlich mit absoluten Zeitwerten.

---

# 5. Aufgaben

Die Timeline übernimmt:

- Zeitverwaltung
- Startpunkte
- Endpunkte
- Fortschrittsberechnung
- Wiederholungen
- Verzögerungen
- Synchronisation
- Ereignisse

---

# 6. Zeitbasis

Intern verwendet die Timeline:

```text
uint64_t Zeit in Millisekunden
```

Alternativ dürfen hochauflösende Mikrosekunden verwendet werden.

---

# 7. Timeline

```c
typedef struct
{
    uint64_t current_time;

    uint64_t delta_time;

    bool paused;

} nova_timeline_t;
```

---

# 8. Animationseintrag

```c
typedef struct
{
    uint64_t start_time;

    uint64_t duration;

    uint64_t delay;

    uint32_t repeat_count;

    bool auto_reverse;

} nova_timeline_animation_t;
```

---

# 9. Lebenszyklus

```text
Created

↓

Waiting

↓

Running

↓

Completed

↓

Destroyed
```

---

# 10. Fortschritt

Der Animationsfortschritt wird berechnet als:

```text
(CurrentTime - StartTime)

/

Duration
```

Das Ergebnis liegt zwischen:

```text
0.0

↓

1.0
```

---

# 11. Verzögerungen

Animationen dürfen eine Startverzögerung besitzen.

Während der Verzögerung werden keine Properties verändert.

---

# 12. Wiederholungen

Unterstützt werden:

- keine Wiederholung
- feste Anzahl
- unendlich

Unendliche Animationen sind ausschließlich für Ladeanzeigen zulässig.

---

# 13. Auto Reverse

Animationen dürfen automatisch rückwärts abgespielt werden.

Beispiel:

```text
0%

↓

100%

↓

0%
```

---

# 14. Keyframes

Eine Timeline unterstützt beliebig viele Keyframes.

Jeder Keyframe besitzt:

- Zeitpunkt
- Propertywert
- Easing

---

# 15. Timeline Events

Folgende Ereignisse werden erzeugt:

- Started
- Delayed
- Running
- Loop
- Finished
- Cancelled

---

# 16. Synchronisation

Mehrere Animationen dürfen dieselbe Timeline verwenden.

Dadurch entstehen synchronisierte Bewegungen.

---

# 17. Timeline Gruppen

Animationen können zu Gruppen zusammengefasst werden.

Alle Animationen einer Gruppe:

- starten gemeinsam
- pausieren gemeinsam
- stoppen gemeinsam

---

# 18. Pause

Die Timeline unterstützt:

- globale Pause
- Gruppenpause
- Einzelpause

Während einer Pause bleibt der Fortschritt erhalten.

---

# 19. Zeitkorrektur

Nach längeren Unterbrechungen wird die Timeline automatisch synchronisiert.

Sprünge dürfen keine sichtbaren Artefakte erzeugen.

---

# 20. Reduced Motion

Bei aktiviertem Reduced Motion:

- verkürzt sich die Timeline,
- Fade-Animationen bleiben erhalten,
- Bewegungsanimationen werden reduziert.

---

# 21. Qualitätsprofile

Die Rendering Quality Profiles dürfen beeinflussen:

- Animationsdauer
- Wiederholungsrate
- Materialanimationen

Die Timeline selbst bleibt unverändert.

---

# 22. Speicherverwaltung

Alle Timeline-Einträge stammen aus einem festen Speicherpool.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 23. Performance

Anforderungen:

- O(n)-Update
- deterministische Laufzeit
- keine rekursiven Zeitberechnungen
- keine dynamischen Speicherallokationen

---

# 24. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- Dauer = 0
- ungültiger Startzeitpunkt
- negativer Zeitbereich
- beschädigte Timeline
- doppelte Animation

Fehler dürfen den Bootprozess nicht unterbrechen.

---

# 25. Sicherheit

Das System verhindert:

- Zeitüberläufe
- rekursive Timelines
- doppelte Scheduler-Einträge
- ungültige Animationseinträge
- Nullpointer

---

# 26. API

```c
nova_result_t
nova_timeline_initialize(void);

void
nova_timeline_update(
    uint64_t current_time);

uint64_t
nova_timeline_get_time(void);

void
nova_timeline_pause(void);

void
nova_timeline_resume(void);
```

---

# 27. Diagnosedaten

Zu erfassen sind:

- aktuelle Timeline-Zeit
- aktive Animationen
- pausierte Animationen
- Wiederholungen
- Scheduler-Auslastung
- Updatezeit
- Zeitkorrekturen
- Frameabweichung

---

# 28. Testfälle

Zu testen sind:

- normale Animation
- Startverzögerung
- Wiederholung
- Auto Reverse
- Timeline Pause
- Timeline Resume
- Gruppenanimation
- Reduced Motion
- Qualitätswechsel
- Recovery

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Animationen dieselbe globale Timeline verwenden.
- Animationen unabhängig von der Bildrate identisch verlaufen.
- Wiederholungen und Auto Reverse korrekt funktionieren.
- Gruppenanimationen synchron laufen.
- Zeitkorrekturen keine sichtbaren Sprünge verursachen.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 30. Referenzen

## ADRs

- ADR-BOOTMOTION-0001 – Zeitbasierte statt framebasierter Animationen
- ADR-BOOTMOTION-0002 – Zentraler Animation-Scheduler
- ADR-BOOTMOTION-0003 – Property-basierte Animationen
- ADR-BOOTMOTION-0006 – Animierte Szenenübergänge

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles
- NPSPEC-BOOTRENDER-0020 – Rendering Diagnostics

---

# 31. Zusammenfassung

Die **Animation Timeline** stellt die zentrale Zeitbasis des NovaOS Boot Motion Systems dar. Sie synchronisiert sämtliche Animationen über eine gemeinsame, zeitbasierte Timeline und ermöglicht deterministische Bewegungen unabhängig von der Bildrate. Durch Unterstützung von Verzögerungen, Wiederholungen, Auto-Reverse, Gruppenanimationen und einer festen Speicherverwaltung bildet sie die Grundlage für flüssige und konsistente Animationen der gesamten Bootoberfläche. :contentReference[oaicite:1]{index=1}