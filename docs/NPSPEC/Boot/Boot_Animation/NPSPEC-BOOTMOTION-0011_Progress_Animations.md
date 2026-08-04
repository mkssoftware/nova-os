# NPSPEC-BOOTMOTION-0011
# Progress Animations

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0011 |
| Titel | Progress Animations |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0010 |
| Zugehörige ADRs | ADR-BOOTMOTION-0003, ADR-BOOTMOTION-0004, ADR-BOOTMOTION-0005, ADR-BOOTPERF-0003 |

---

# 1. Ziel

Diese Spezifikation definiert sämtliche **Progress Animations** der NovaOS Boot UI.

Sie beschreibt die Darstellung und Animation aller Fortschrittsanzeigen während Boot-, Diagnose-, Recovery- und Wartungsvorgängen.

Das Motion System unterstützt:

- lineare Fortschrittsbalken
- kreisförmige Fortschrittsanzeigen
- unbestimmte Animationen
- bestimmte Fortschrittsanimationen
- Statusübergänge
- Abschlussanimationen
- Fehleranimationen

Das Ziel besteht darin, jederzeit eindeutig sichtbar zu machen:

- dass gearbeitet wird,
- wie weit ein Vorgang fortgeschritten ist,
- wann ein Vorgang abgeschlossen ist,
- wann ein Fehler aufgetreten ist.

Fortschrittsanzeigen unterscheiden grundsätzlich zwischen **determiniertem** Fortschritt (bekannter Fortschritt) und **indeterminiertem** Fortschritt (Dauer unbekannt). :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Fortschrittsanzeigen des Bootmanagers.

Sie umfasst:

- Bootvorgänge
- Recovery
- Speicherdiagnose
- Dateisystemprüfung
- Updateprozesse
- Backup
- Wiederherstellung
- Verschlüsselung

Nicht Bestandteil sind:

- Splash Screen
- Ladeanimationen ohne Statusbezug

---

# 3. Architektur

```text
Progress Source

↓

Progress Manager

↓

Animation Scheduler

↓

Progress Animator

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Progress Animationen repräsentieren ausschließlich den aktuellen Status eines laufenden Vorgangs.

Sie dürfen niemals künstlich beschleunigt oder verlangsamt werden.

Der dargestellte Fortschritt muss dem tatsächlichen Fortschritt entsprechen.

---

# 5. Aufgaben

Das Motion System übernimmt:

- Fortschrittsanimation
- Statuswechsel
- Übergänge
- Abschlussanimation
- Fehleranimation
- Synchronisation
- Qualitätsanpassung

---

# 6. Progressarten

Unterstützt werden:

- Determinate Progress
- Indeterminate Progress
- Circular Progress
- Linear Progress
- Segmentierter Progress

---

# 7. Determinate Progress

Der Fortschritt besitzt einen bekannten Wert.

```text
0 %

↓

100 %
```

Der Balken folgt exakt dem gemeldeten Fortschritt.

---

# 8. Indeterminate Progress

Ist der Fortschritt unbekannt, verwendet das System eine zyklische Animation.

Die Animation darf niemals einen konkreten Fortschrittswert suggerieren. :contentReference[oaicite:1]{index=1}

---

# 9. Lineare Progress Animation

Animierbar:

- Balkenlänge
- Farbverlauf
- Deckkraft
- Highlight

Die Animation erfolgt kontinuierlich.

---

# 10. Kreisförmige Progress Animation

Animierbar:

- Startwinkel
- Endwinkel
- Rotation
- Deckkraft

Der Kreis darf sowohl determiniert als auch indeterminiert arbeiten.

---

# 11. Statuswechsel

Folgende Zustände werden unterstützt:

```text
Idle

↓

Running

↓

Completed

↓

Failed

↓

Cancelled
```

Zwischen den Zuständen erfolgen animierte Übergänge.

---

# 12. Abschlussanimation

Nach erfolgreichem Abschluss:

- Balken vollständig füllen
- kurzer Highlight-Effekt
- sanft ausblenden

Die Animation dauert maximal 250 ms.

---

# 13. Fehleranimation

Bei Fehlern:

- Fortschrittsanimation stoppen
- Farbe auf Fehlerfarbe wechseln
- kurzer Puls

Eine endlose Fehleranimation ist unzulässig.

---

# 14. Aktualisierung

Fortschrittswerte werden ausschließlich durch den Progress Manager geliefert.

Animationen interpolieren nur zwischen zwei gültigen Werten.

---

# 15. Sprungkorrektur

Große Fortschrittssprünge werden weich interpoliert.

Die tatsächliche Reihenfolge der Fortschrittswerte darf dabei nicht verändert werden.

---

# 16. Parallelität

Mehrere Fortschrittsanzeigen dürfen gleichzeitig dargestellt werden.

Jede Anzeige besitzt:

- eigene Timeline
- eigenen Status
- eigene Animation

---

# 17. Materialanimation

Animiert werden dürfen:

- Tint
- Glow
- Shadow
- Border
- Transparenz

Blur wird standardmäßig nicht verwendet.

---

# 18. Qualitätsprofile

Je nach Qualitätsprofil dürfen reduziert werden:

- Glow
- Highlight
- Gradient
- Schatten

Die Fortschrittsinformation bleibt unverändert sichtbar.

---

# 19. Reduced Motion

Bei aktiviertem Reduced Motion:

- zyklische Bewegungen werden verlangsamt,
- Highlight-Effekte reduziert,
- Abschlussanimation vereinfacht.

Der Fortschritt bleibt jederzeit eindeutig erkennbar.

---

# 20. Synchronisation

Alle Progress Animationen werden unmittelbar vor dem Rendern aktualisiert.

Ein Fortschrittswert wird niemals innerhalb eines Frames mehrfach verändert.

---

# 21. Speicherverwaltung

Alle Progress Animationen verwenden:

- festen Objektpool
- feste Timelineobjekte

Heap-Allokationen während der Laufzeit sind verboten.

---

# 22. Performance

Anforderungen:

- deterministische Laufzeit
- O(n)-Scheduler
- keine rekursiven Animationen
- keine Heap-Allokationen

---

# 23. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Fortschrittswert
- Wert < 0 %
- Wert > 100 %
- fehlende Datenquelle
- beschädigte Progress Animation

Bei Fehlern wird automatisch auf einen indeterminierten Fortschrittsindikator umgeschaltet.

---

# 24. Sicherheit

Das System verhindert:

- rückwärts laufende Fortschritte
- doppelte Abschlussanimationen
- ungültige Prozentwerte
- Nullpointer
- beschädigte Timelines

---

# 25. API

```c
nova_result_t
nova_progress_initialize(void);

nova_result_t
nova_progress_set_value(
    nova_progress_t* progress,
    float value);

nova_result_t
nova_progress_set_state(
    nova_progress_t* progress,
    nova_progress_state_t state);

void
nova_progress_update(
    uint64_t current_time);

bool
nova_progress_is_running(
    const nova_progress_t* progress);
```

---

# 26. Diagnosedaten

Zu erfassen sind:

- Fortschrittswert
- Animationsstatus
- Abschlussanimationen
- Fehleranimationen
- Schedulerlaufzeit
- Aktualisierungsrate
- aktive Progressanzeigen
- Fehleranzahl

---

# 27. Testfälle

Zu testen sind:

- Determinate Progress
- Indeterminate Progress
- Linear Progress
- Circular Progress
- Abschlussanimation
- Fehleranimation
- Qualitätswechsel
- Reduced Motion
- Recovery
- Parallelbetrieb

---

# 28. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- determinate und indeterminate Fortschrittsanzeigen vollständig unterstützt werden.
- Fortschrittswerte korrekt interpoliert werden.
- Abschluss- und Fehleranimationen eindeutig dargestellt werden.
- Reduced Motion vollständig unterstützt wird.
- mehrere Progressanzeigen parallel funktionieren.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identische Progress Animationen erzeugen.

---

# 29. Referenzen

## ADRs

- ADR-BOOTMOTION-0003 – Property-basierte Animationen
- ADR-BOOTMOTION-0004 – Easing- und Spring-Funktionen
- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen
- ADR-BOOTPERF-0003 – Adaptive Bildwiederholrate

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTMOTION-0005 – Easing Functions
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTMOTION-0010 – Focus and Selection Motion

---

# 30. Zusammenfassung

Die **Progress Animations** definieren sämtliche Fortschrittsdarstellungen der NovaOS Bootoberfläche. Das System unterstützt determinierte und indeterminierte Fortschrittsanzeigen, lineare und kreisförmige Darstellungen sowie animierte Statuswechsel für Abschluss- und Fehlerzustände. Durch deterministische Aktualisierung, adaptive Qualitätsprofile und feste Speicherverwaltung entsteht eine konsistente und performante Benutzerführung auf BIOS- und UEFI-Systemen. :contentReference[oaicite:2]{index=2}