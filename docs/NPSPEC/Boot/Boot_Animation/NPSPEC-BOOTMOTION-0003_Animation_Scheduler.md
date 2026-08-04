# NPSPEC-BOOTMOTION-0003
# Animation Scheduler

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0003 |
| Titel | Animation Scheduler |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001, NPSPEC-BOOTMOTION-0002 |
| Zugehörige ADRs | ADR-BOOTMOTION-0001, ADR-BOOTMOTION-0002, ADR-BOOTMOTION-0003, ADR-BOOTMOTION-0005, ADR-BOOTPERF-0002 |

---

# 1. Ziel

Diese Spezifikation definiert den **Animation Scheduler** der NovaOS Boot UI.

Der Scheduler verwaltet sämtliche Animationen während ihrer gesamten Laufzeit und stellt sicher, dass sie deterministisch, synchronisiert und unabhängig von der Bildrate ausgeführt werden.

Zu seinen Aufgaben gehören:

- Starten neuer Animationen
- Aktualisieren laufender Animationen
- Beenden abgeschlossener Animationen
- Priorisierung
- Synchronisation
- Gruppenverwaltung
- Wiederholungen
- Abbruch laufender Animationen

Der Scheduler ist das zentrale Steuerelement des Motion Systems.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für alle Animationen des Bootmanagers.

Sie umfasst:

- Property Animationen
- Materialanimationen
- Dialoganimationen
- Menüanimationen
- Cursoranimationen
- Overlayanimationen
- Übergänge

Nicht Bestandteil sind:

- Rendering
- Compositing
- Audio

---

# 3. Architektur

```text
Timeline
      │
      ▼
Animation Scheduler
      │
      ├──────────────┐
      ▼              ▼
Running Queue   Waiting Queue
      │              │
      └──────┬───────┘
             ▼
Property Animator
             │
             ▼
Compositor
```

---

# 4. Grundprinzip

Der Scheduler arbeitet ausschließlich zeitbasiert.

Er berechnet niemals einzelne Animationsschritte anhand der Anzahl gerenderter Frames.

Alle Animationen beziehen ihre Zeit von der globalen Animation Timeline.

---

# 5. Aufgaben

Der Scheduler übernimmt:

- Animation registrieren
- Animation starten
- Animation pausieren
- Animation fortsetzen
- Animation stoppen
- Animation entfernen
- Gruppen verwalten
- Prioritäten verwalten
- Timeline synchronisieren

---

# 6. Schedulerstruktur

```c
typedef struct
{
    uint32_t active_count;

    uint32_t waiting_count;

    uint32_t completed_count;

    bool paused;

} nova_animation_scheduler_t;
```

---

# 7. Animationszustände

Jede Animation besitzt genau einen Zustand.

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

↓

Destroyed
```

---

# 8. Warteschlangen

Der Scheduler verwaltet mindestens folgende Warteschlangen:

- Waiting Queue
- Running Queue
- Finished Queue

Die Warteschlangen arbeiten deterministisch.

---

# 9. Start

Eine Animation startet wenn:

- ihre Startzeit erreicht wurde,
- sämtliche Abhängigkeiten erfüllt sind,
- ausreichende Ressourcen vorhanden sind.

---

# 10. Aktualisierung

Vor jedem Frame:

1. Timeline aktualisieren
2. Running Queue durchlaufen
3. Propertywerte berechnen
4. Animationen abschließen
5. neue Animationen starten

Diese Reihenfolge ist verbindlich.

---

# 11. Beenden

Eine Animation endet wenn:

- ihre Dauer erreicht wurde,
- sie abgebrochen wird,
- ein Fehler auftritt,
- die Anwendung sie beendet.

Nach dem Beenden wird sie aus der Running Queue entfernt.

---

# 12. Prioritäten

Animationen besitzen Prioritäten.

| Priorität | Verwendung |
|---|---|
| Kritisch | Sicherheitsdialoge |
| Hoch | Dialoge |
| Normal | Controls |
| Niedrig | Dekoration |

Höhere Prioritäten werden zuerst verarbeitet.

---

# 13. Gruppen

Animationen können Gruppen bilden.

Eigenschaften:

- gemeinsamer Start
- gemeinsames Ende
- gemeinsame Pause
- gemeinsame Wiederaufnahme

---

# 14. Synchronisation

Mehrere Animationen dürfen synchron laufen.

Die Aktualisierung erfolgt innerhalb desselben Scheduler-Durchlaufs.

---

# 15. Wiederholungen

Der Scheduler unterstützt:

- einmalig
- feste Anzahl
- unendlich

Unendliche Animationen sind ausschließlich für Ladeanimationen zulässig.

---

# 16. Auto Reverse

Animationen dürfen automatisch rückwärts abgespielt werden.

Der Scheduler verwaltet Richtungswechsel selbstständig.

---

# 17. Unterbrechung

Laufende Animationen dürfen:

- ersetzt
- beendet
- neu gestartet
- rückwärts abgespielt

werden.

Der Scheduler bereinigt alle Zustände automatisch.

---

# 18. Abhängigkeiten

Animationen dürfen von anderen Animationen abhängig sein.

Beispiele:

- Dialog erscheint nach Hintergrund-Fade.
- Menü öffnet nach Dialoganimation.
- Button erscheint nach Panelanimation.

Zyklische Abhängigkeiten sind verboten.

---

# 19. Qualitätsprofile

Je nach aktivem Qualitätsprofil darf der Scheduler:

- Animationen verkürzen
- Materialanimationen deaktivieren
- Parallelität reduzieren

Die zeitliche Reihenfolge bleibt unverändert.

---

# 20. Reduced Motion

Bei aktiviertem Reduced Motion:

- Bewegungsanimationen werden reduziert,
- Fade-Animationen bleiben erhalten,
- Schedulerlogik bleibt unverändert.

---

# 21. Speicherverwaltung

Alle Scheduler-Einträge werden aus einem festen Pool reserviert.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 22. Performance

Anforderungen:

- O(n)-Durchlauf
- deterministische Laufzeit
- keine rekursiven Scheduler-Aufrufe
- keine dynamischen Speicherallokationen

---

# 23. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- doppelte Registrierung
- ungültige Timeline
- beschädigte Warteschlange
- zyklische Abhängigkeiten
- ungültige Priorität

Fehler dürfen den Bootprozess nicht unterbrechen.

---

# 24. Sicherheit

Das System verhindert:

- doppelte Scheduler-Einträge
- Endlosschleifen
- Nullpointer
- beschädigte Listen
- Zeitüberläufe

Alle Schedulerdaten werden vor jeder Aktualisierung validiert.

---

# 25. API

```c
nova_result_t
nova_scheduler_initialize(void);

nova_result_t
nova_scheduler_register(
    nova_animation_t* animation);

void
nova_scheduler_update(
    uint64_t current_time);

void
nova_scheduler_cancel(
    nova_animation_t* animation);

void
nova_scheduler_shutdown(void);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- aktive Animationen
- wartende Animationen
- abgeschlossene Animationen
- Schedulerlaufzeit
- Gruppenanzahl
- Prioritätsverteilung
- Wiederholungen
- Fehleranzahl

---

# 27. Testfälle

Zu testen sind:

- einzelne Animation
- mehrere Animationen
- Gruppenanimation
- Prioritäten
- Wiederholungen
- Auto Reverse
- Abbruch
- Pause
- Reduced Motion
- Recovery

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Animationen deterministisch verwaltet werden.
- Prioritäten korrekt berücksichtigt werden.
- Gruppenanimationen synchron laufen.
- zyklische Abhängigkeiten erkannt werden.
- Reduced Motion vollständig unterstützt wird.
- keine Heap-Allokationen im Scheduler-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 29. Referenzen

## ADRs

- ADR-BOOTMOTION-0001 – Zeitbasierte statt framebasierter Animationen
- ADR-BOOTMOTION-0002 – Zentraler Animation-Scheduler
- ADR-BOOTMOTION-0003 – Property-basierte Animationen
- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0002 – Animation Timeline
- NPSPEC-BOOTRENDER-0018 – Rendering Quality Profiles
- NPSPEC-BOOTRENDER-0020 – Rendering Diagnostics

---

# 30. Zusammenfassung

Der **Animation Scheduler** ist die zentrale Steuerkomponente des NovaOS Boot Motion Systems. Er verwaltet sämtliche Animationen über ihren gesamten Lebenszyklus hinweg, synchronisiert sie mit der globalen Timeline und sorgt für eine deterministische Ausführung unabhängig von der Bildrate. Durch Prioritäten, Gruppenverwaltung, Abhängigkeitsauflösung und feste Speicherpools gewährleistet der Scheduler eine performante und konsistente Animationsausführung auf BIOS- und UEFI-Systemen.