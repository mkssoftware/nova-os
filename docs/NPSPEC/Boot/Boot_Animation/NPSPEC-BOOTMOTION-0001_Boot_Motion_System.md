# NPSPEC-BOOTMOTION-0001
# Boot Motion System

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0001 |
| Titel | Boot Motion System |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTCOMPOSITOR-0001 bis NPSPEC-BOOTCOMPOSITOR-0010, NPSPEC-BOOTRENDER-0001 bis NPSPEC-BOOTRENDER-0020 |
| Zugehörige ADRs | ADR-BOOTMOTION-0001, ADR-BOOTMOTION-0002, ADR-BOOTMOTION-0003, ADR-BOOTMOTION-0004, ADR-BOOTMOTION-0006, ADR-BOOTMOTION-0007, ADR-BOOTMOTION-0008 |

---

# 1. Ziel

Diese Spezifikation definiert das vollständige **Boot Motion System** der NovaOS Boot UI.

Das Motion System ist verantwortlich für sämtliche zeitbasierten Animationen der Bootoberfläche und sorgt für eine moderne, flüssige und konsistente Benutzererfahrung.

Es steuert unter anderem:

- Dialoganimationen
- Menüanimationen
- Fensteranimationen
- Glass-Animationen
- Hover-Effekte
- Fokusanimationen
- Ladeanimationen
- Übergänge
- Materialanimationen
- Cursoranimationen

Alle Animationen werden unabhängig vom verwendeten Grafik-Backend identisch dargestellt. Moderne UI-Systeme verwenden zeitbasierte, konsistente Animationen, um räumliche Zusammenhänge zu vermitteln und Benutzerinteraktionen verständlicher zu machen. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Animationen innerhalb der Bootoberfläche.

Sie umfasst:

- Controls
- Dialoge
- Menüs
- Panels
- Overlays
- Glass Materials
- Acrylic Materials
- Cursor
- Splash Screen
- Recovery UI

Nicht Bestandteil sind:

- Bootlogo-Animation
- Kernelanimationen
- Hardwareinitialisierung

---

# 3. Architektur

```text
UI Event
      │
      ▼
Motion Manager
      │
      ▼
Animation Scheduler
      │
      ▼
Animation Engine
      │
      ▼
Property Animator
      │
      ▼
Compositor
      │
      ▼
Framebuffer
```

---

# 4. Grundprinzip

Das Motion System arbeitet vollständig zeitbasiert.

Animationen werden niemals anhand der Anzahl gerenderter Frames berechnet.

Alle Animationen basieren ausschließlich auf:

- Startzeit
- aktuelle Zeit
- Endzeit

Dadurch entstehen identische Bewegungen unabhängig von der Bildrate.

---

# 5. Aufgaben

Das Motion System übernimmt:

- Animationserzeugung
- Zeitverwaltung
- Scheduler
- Property Animation
- Übergänge
- Materialanimationen
- Easing
- Spring Animationen
- Animation Events
- Abbruch laufender Animationen

---

# 6. Komponenten

Das Motion System besteht aus:

- Motion Manager
- Animation Scheduler
- Animation Queue
- Timeline
- Property Animator
- Transition Engine
- Motion Diagnostics

---

# 7. Animationsmodell

Jede Animation besitzt:

```c
typedef struct
{
    uint64_t start_time;

    uint64_t duration;

    uint32_t easing;

    uint32_t property;

    bool running;

} nova_animation_t;
```

---

# 8. Animierbare Eigenschaften

Das System unterstützt mindestens:

- Position
- Größe
- Rotation
- Skalierung
- Deckkraft
- Blur Radius
- Glow
- Shadow
- Tint
- Border
- Corner Radius

Neue Eigenschaften können ergänzt werden.

---

# 9. Animationsarten

Unterstützt werden:

- Property Animation
- Transition
- Timeline Animation
- Spring Animation
- Keyframe Animation
- Sequenzanimation
- Parallelanimation

---

# 10. Zeitmodell

Alle Zeitangaben erfolgen in:

```text
Millisekunden
```

Intern verwendet das System hochauflösende Boot-Timer.

---

# 11. Standarddauer

Empfohlene Werte:

| Animation | Dauer |
|---|---:|
| Hover | 80 ms |
| Fokus | 100 ms |
| Button | 120 ms |
| Menü | 180 ms |
| Dialog | 220 ms |
| Overlay | 250 ms |
| Recovery | 300 ms |

Alle Werte stammen aus den Motion Tokens.

---

# 12. Motion Tokens

Das Motion System verwendet ausschließlich zentrale Motion Tokens.

Hierzu gehören:

- Dauer
- Verzögerung
- Easing
- Springparameter
- Geschwindigkeiten

Direkte Zahlenwerte im UI-Code sind unzulässig.

---

# 13. Motion States

Animationen besitzen folgende Zustände:

```text
Created

↓

Waiting

↓

Running

↓

Paused

↓

Completed

↓

Cancelled
```

---

# 14. Scheduler

Der Animation Scheduler:

- startet Animationen
- beendet Animationen
- aktualisiert Properties
- entfernt abgeschlossene Animationen
- synchronisiert parallele Animationen

---

# 15. Parallelität

Mehrere Animationen dürfen gleichzeitig laufen.

Die Aktualisierung erfolgt deterministisch.

Die Reihenfolge wird durch den Scheduler festgelegt.

---

# 16. Unterbrechung

Laufende Animationen dürfen jederzeit:

- gestoppt
- ersetzt
- umgeleitet
- rückwärts abgespielt

werden.

---

# 17. Übergänge

Unterstützt werden:

- Fade
- Slide
- Scale
- Zoom
- Material Transition
- Glass Transition

---

# 18. Materialanimationen

Animiert werden dürfen:

- Blur
- Transparenz
- Tint
- Glow
- Shadow
- Border
- Highlight

---

# 19. Qualitätsprofile

Die Rendering Quality Profiles beeinflussen:

- Animationsdauer
- Materialeffekte
- Bluranimationen
- Shadowanimationen

Im Safe-Profil werden Animationen vollständig deaktiviert.

---

# 20. Reduced Motion

Ist "Reduced Motion" aktiv:

- werden Bewegungen reduziert,
- Fade-Animationen bleiben erhalten,
- Layoutänderungen erfolgen weiterhin animiert.

Die Bedienbarkeit darf dadurch nicht beeinträchtigt werden. :contentReference[oaicite:1]{index=1}

---

# 21. Synchronisation

Animationen werden unmittelbar vor dem Rendern aktualisiert.

Alle Änderungen eines Frames werden gemeinsam dargestellt.

---

# 22. Speicherverwaltung

Animationen werden aus einem festen Pool verwaltet.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 23. Performance

Anforderungen:

- deterministische Laufzeit
- O(n)-Scheduler
- keine rekursiven Animationen
- keine Speicherallokationen im Hotpath

---

# 24. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Dauer
- doppelte Animation
- ungültige Property
- beschädigte Timeline
- ungültige Easing-Funktion

Fehler dürfen den Bootprozess nicht unterbrechen.

---

# 25. Sicherheit

Das System verhindert:

- Endlosschleifen
- doppelte Scheduler-Einträge
- Nullpointer
- beschädigte Animationen
- ungültige Zeitstempel

Alle Eingabedaten werden validiert.

---

# 26. API

```c
nova_result_t
nova_motion_initialize(void);

nova_result_t
nova_motion_shutdown(void);

nova_animation_t*
nova_motion_create(
    const nova_animation_desc_t* desc);

void
nova_motion_cancel(
    nova_animation_t* animation);

void
nova_motion_update(
    uint64_t current_time);

bool
nova_motion_is_running(
    const nova_animation_t* animation);
```

---

# 27. Diagnosedaten

Zu erfassen sind:

- aktive Animationen
- Scheduler-Auslastung
- Framezeit
- Animationsdauer
- abgebrochene Animationen
- Parallelanimationen
- Motion Pool
- Qualitätsprofil

---

# 28. Testfälle

Zu testen sind:

- einzelne Animation
- parallele Animationen
- Animation abbrechen
- Animation ersetzen
- Dialoganimation
- Materialanimation
- Qualitätswechsel
- Reduced Motion
- Safe Mode
- Recovery

---

# 29. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Animationen zeitbasiert arbeiten.
- Animationen unabhängig von der Bildrate identisch verlaufen.
- parallele Animationen deterministisch aktualisiert werden.
- Motion Tokens vollständig verwendet werden.
- Reduced Motion korrekt unterstützt wird.
- keine Heap-Allokationen im Animations-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Animationen darstellen.

---

# 30. Referenzen

## ADRs

- ADR-BOOTMOTION-0001 – Zeitbasierte statt framebasierter Animationen
- ADR-BOOTMOTION-0002 – Zentraler Animation-Scheduler
- ADR-BOOTMOTION-0003 – Property-basierte Animationen
- ADR-BOOTMOTION-0004 – Easing- und Spring-Funktionen
- ADR-BOOTMOTION-0006 – Animierte Szenenübergänge
- ADR-BOOTMOTION-0007 – Reduzierte Bewegung bei eingeschränkter Leistung
- ADR-BOOTMOTION-0008 – Unterstützung von „Reduced Motion“

## NPSPECs

- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor
- NPSPEC-BOOTRENDER-0015 – Shadows and Glows
- NPSPEC-BOOTRENDER-0016 – Background Blur
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles
- NPSPEC-BOOTCOMPOSITOR-0006 – Glass Material
- NPSPEC-BOOTCOMPOSITOR-0007 – Acrylic Material

---

# 31. Zusammenfassung

Das **Boot Motion System** bildet die zentrale Animationsplattform der NovaOS Bootoberfläche. Es steuert sämtliche zeitbasierten Übergänge, Materialeffekte und Benutzerinteraktionen über einen deterministischen Scheduler und ein einheitliches Property-Animationsmodell. Durch Motion Tokens, zentrale Zeitverwaltung, adaptive Qualitätsprofile und Unterstützung für „Reduced Motion“ entsteht eine hochwertige, konsistente und performante Benutzeroberfläche mit identischem Verhalten auf BIOS- und UEFI-Systemen. :contentReference[oaicite:2]{index=2}