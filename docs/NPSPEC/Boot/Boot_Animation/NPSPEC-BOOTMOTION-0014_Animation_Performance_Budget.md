# NPSPEC-BOOTMOTION-0014
# Animation Performance Budget

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0014 |
| Titel | Animation Performance Budget |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0013, NPSPEC-BOOTPERF-0001 bis NPSPEC-BOOTPERF-0006 |
| Zugehörige ADRs | ADR-BOOTPERF-0001, ADR-BOOTPERF-0002, ADR-BOOTPERF-0003, ADR-BOOTPERF-0004, ADR-BOOTMOTION-0005 |

---

# 1. Ziel

Diese Spezifikation definiert das **Animation Performance Budget** der NovaOS Boot UI.

Das Budget legt verbindliche Grenzen für sämtliche Animationen fest, damit die Bootoberfläche jederzeit flüssig, deterministisch und reaktionsschnell bleibt.

Es definiert unter anderem:

- Framebudget
- CPU-Budget
- Speicherbudget
- Animationsbudget
- Materialbudget
- Blurbudget
- Schedulerbudget
- Qualitätsstufen

Animationen dürfen niemals die Stabilität oder Startgeschwindigkeit des Bootmanagers beeinträchtigen.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Animationen der Boot UI.

Sie umfasst:

- Dialoge
- Menüs
- Navigation
- Fokus
- Progress
- Materialanimationen
- Overlayanimationen
- Recovery UI

Nicht Bestandteil sind:

- Kernelanimationen
- Hardwareinitialisierung

---

# 3. Architektur

```text
Animation

↓

Animation Scheduler

↓

Performance Budget

↓

Motion Policy

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Jede Animation verbraucht:

- CPU-Zeit
- Speicher
- Renderzeit

Das gesamte Motion System arbeitet innerhalb eines festen Budgets.

Bei Budgetüberschreitung werden Animationen automatisch vereinfacht.

---

# 5. Performanceziele

| Parameter | Ziel |
|---|---:|
| Ziel-Framerate | 60 FPS |
| Minimal | 30 FPS |
| Framebudget | 16,67 ms |
| Motion Budget | ≤ 3 ms |
| Scheduler | ≤ 1 ms |
| Material | ≤ 1 ms |
| Reserve | ≥ 1 ms |

---

# 6. CPU Budget

Animationen dürfen maximal:

```text
20 %

der verfügbaren Renderzeit
```

beanspruchen.

Rendering besitzt stets höhere Priorität.

---

# 7. Speicherbudget

Maximal reservierter Speicher:

| Objekt | Limit |
|---|---:|
| Animationen | 512 KB |
| Timelines | 128 KB |
| Scheduler | 128 KB |
| Springdaten | 128 KB |
| Motion Cache | 256 KB |

---

# 8. Objektlimits

Maximal gleichzeitig:

| Objekt | Anzahl |
|---|---:|
| Animationen | 256 |
| Timelines | 128 |
| Spring Animationen | 64 |
| Materialanimationen | 64 |
| Progressanimationen | 32 |

---

# 9. Materialbudget

Pro Frame:

- maximal 8 Bluranimationen
- maximal 32 Shadowanimationen
- maximal 32 Glowanimationen
- maximal 16 Glassanimationen

---

# 10. Schedulerbudget

Der Animation Scheduler darf:

- höchstens einmal pro Frame laufen
- keine rekursiven Updates erzeugen
- keine Blockierungen verursachen

---

# 11. Framebudget

Die empfohlene Aufteilung:

| Bereich | Budget |
|---|---:|
| Animation Scheduler | 1 ms |
| Property Updates | 1 ms |
| Materialanimation | 1 ms |
| Rendering | 8 ms |
| Compositor | 4 ms |
| Reserve | 1,67 ms |

---

# 12. Qualitätsstufen

Unterstützt werden:

- Ultra
- High
- Standard
- Performance
- Safe

Der Wechsel erfolgt automatisch oder über Konfiguration.

---

# 13. Automatische Degradation

Bei Budgetüberschreitung erfolgt folgende Reihenfolge:

1. Glow reduzieren
2. Shadow reduzieren
3. Blur reduzieren
4. Spring ersetzen
5. Materialanimationen vereinfachen
6. Dekorative Animationen deaktivieren

Essenzielle Animationen bleiben erhalten. Richtlinien für barrierearme Animationen empfehlen ebenfalls, dekorative Bewegungen zuerst zu reduzieren und funktionale Rückmeldungen beizubehalten. :contentReference[oaicite:0]{index=0}

---

# 14. Animation Prioritäten

| Priorität | Beispiele |
|---|---|
| Kritisch | Sicherheitsdialoge |
| Hoch | Fokus |
| Normal | Navigation |
| Niedrig | Dekoration |

Niedrige Prioritäten werden zuerst reduziert.

---

# 15. Parallelität

Animationen dürfen parallel laufen.

Der Scheduler garantiert:

- deterministische Reihenfolge
- konstante Laufzeit
- identisches Ergebnis auf allen Plattformen

---

# 16. Reduced Motion

Ist Reduced Motion aktiv:

- Spring deaktivieren
- Bounce deaktivieren
- Materialanimation reduzieren
- CPU-Budget senken

Dadurch sinkt der Ressourcenverbrauch zusätzlich. :contentReference[oaicite:1]{index=1}

---

# 17. Speicherverwaltung

Alle Motion-Objekte stammen aus:

- festen Pools
- festen Listen
- statischen Ressourcen

Heap-Allokationen während Animationen sind verboten.

---

# 18. Performanceüberwachung

Das System überwacht kontinuierlich:

- Framezeit
- Schedulerzeit
- CPU-Zeit
- Materialzeit
- Speicherverbrauch
- Objektanzahl

---

# 19. Budgetverletzungen

Wird ein Budget überschritten:

```text
Warnung

↓

Qualität reduzieren

↓

Motion Budget anpassen

↓

Safe Mode
```

Der Bootprozess darf niemals blockiert werden.

---

# 20. Cache

Animationen dürfen verwenden:

- Easing Cache
- Property Cache
- Matrix Cache

Alle Caches besitzen feste Größen.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- Budgetüberschreitung
- Objektpool erschöpft
- Schedulerüberlastung
- Speicherlimit überschritten
- Materiallimit überschritten

Im Fehlerfall erfolgt automatische Qualitätsreduktion.

---

# 22. Sicherheit

Das System verhindert:

- unendliche Animationen
- Schedulerblockierungen
- Heapfragmentierung
- Speicherüberläufe
- rekursive Animationen

---

# 23. API

```c
nova_result_t
nova_motion_budget_initialize(void);

const nova_motion_budget_t*
nova_motion_budget_get(void);

bool
nova_motion_budget_can_allocate(
    nova_animation_type_t type);

void
nova_motion_budget_update(
    const nova_frame_statistics_t* statistics);

void
nova_motion_budget_apply_fallback(void);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Framezeit
- Schedulerzeit
- CPU-Auslastung
- Speicherverbrauch
- aktive Animationen
- Materialanimationen
- Qualitätsprofil
- Budgetverletzungen

---

# 25. Testfälle

Zu testen sind:

- maximale Animationsanzahl
- maximale Materialanimationen
- Schedulergrenzen
- Speichergrenzen
- Qualitätswechsel
- Reduced Motion
- Safe Mode
- Recovery
- Dauerlast
- Budgetverletzungen

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Animationen innerhalb des definierten Budgets ausgeführt werden.
- Budgetverletzungen automatisch erkannt werden.
- Qualitätsprofile korrekt umgeschaltet werden.
- dekorative Animationen zuerst reduziert werden.
- essenzielle Animationen erhalten bleiben.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTPERF-0001 – Festes Speicherbudget für die Boot-UI
- ADR-BOOTPERF-0002 – Keine Speicherallokation im Render-Hotpath
- ADR-BOOTPERF-0003 – Adaptive Bildwiederholrate
- ADR-BOOTPERF-0004 – Automatische Effektdegradation
- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTMOTION-0012 – Interruptible Animations
- NPSPEC-BOOTMOTION-0013 – Reduced Motion Mode
- NPSPEC-BOOTPERF-0001 – Fixed Memory Budget
- NPSPEC-BOOTPERF-0006 – Frame-Time Diagnostics

---

# 28. Zusammenfassung

Das **Animation Performance Budget** definiert verbindliche Leistungsgrenzen für sämtliche Animationen der NovaOS Bootoberfläche. Durch feste Zeit-, Speicher- und Ressourcenbudgets, kontinuierliche Laufzeitüberwachung sowie automatische Qualitätsanpassungen wird sichergestellt, dass Animationen die Stabilität und Reaktionsfähigkeit des Bootmanagers niemals beeinträchtigen. Essenzielle Bewegungen bleiben erhalten, während dekorative Effekte bei Bedarf reduziert werden, um auf BIOS- und UEFI-Systemen jederzeit eine flüssige und deterministische Benutzeroberfläche bereitzustellen. Empfehlungen aktueller Accessibility-Richtlinien unterstützen ebenfalls das Reduzieren nicht essenzieller Animationen bei eingeschränkten Ressourcen oder aktivierter Bewegungsreduktion. :contentReference[oaicite:2]{index=2}