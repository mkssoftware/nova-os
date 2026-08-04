# NPSPEC-BOOTMOTION-0012
# Interruptible Animations

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0012 |
| Titel | Interruptible Animations |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0011 |
| Zugehörige ADRs | ADR-BOOTMOTION-0005, ADR-BOOTMOTION-0006, ADR-BOOTINPUT-0001, ADR-BOOTINPUT-0005, ADR-BOOTNAV-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das System für **Interruptible Animations** der NovaOS Boot UI.

Interruptible Animations ermöglichen es, laufende Animationen jederzeit sicher zu:

- unterbrechen
- pausieren
- umkehren
- ersetzen
- auf ein neues Ziel umlenken
- synchron fortsetzen

Dadurch reagiert die Bootoberfläche jederzeit unmittelbar auf neue Benutzereingaben und muss niemals auf das Ende einer Animation warten. Moderne UI-Systeme unterstützen unterbrechbare Animationen, sodass laufende Übergänge angehalten oder auf ein neues Ziel umgelenkt werden können. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Animationen der Boot UI.

Sie umfasst:

- Dialoganimationen
- Menüanimationen
- Fokusanimationen
- Materialanimationen
- Overlayanimationen
- Progress Animationen
- Navigationstransitionen
- Control Animationen

Nicht Bestandteil sind:

- Splash Screen
- Bootlogo
- Hardwareinitialisierung

---

# 3. Architektur

```text
Input Event

↓

Motion Manager

↓

Animation Scheduler

↓

Interrupt Manager

↓

Animation Instance

↓

Property Animator

↓

Compositor
```

---

# 4. Grundprinzip

Jede Animation befindet sich jederzeit in einem gültigen Zwischenzustand.

Eine Unterbrechung darf niemals:

- sichtbare Sprünge
- Flackern
- doppelte Animationen
- inkonsistente Zustände

erzeugen.

---

# 5. Aufgaben

Das System übernimmt:

- Animation unterbrechen
- Animation pausieren
- Animation fortsetzen
- Animation ersetzen
- Ziel ändern
- Richtung ändern
- Geschwindigkeit übernehmen
- Zustände synchronisieren

---

# 6. Unterbrechbare Animation

Eine Animation besitzt zusätzlich:

```c
typedef struct
{
    bool interruptible;

    bool paused;

    bool reversing;

    float current_progress;

    float current_velocity;

} nova_interruptible_animation_t;
```

---

# 7. Animationszustände

```text
Created

↓

Waiting

↓

Running

↓

Paused

↓

Interrupted

↓

Resuming

↓

Completed

↓

Cancelled
```

---

# 8. Unterbrechung

Eine Unterbrechung darf ausgelöst werden durch:

- Tastatureingabe
- Maus
- Touch
- Navigation
- Dialogwechsel
- Recovery
- API

---

# 9. Pause

Während einer Pause:

- bleiben sämtliche Propertywerte erhalten.
- bleibt die aktuelle Geschwindigkeit gespeichert.
- wird die Timeline eingefroren.

Es erfolgt keinerlei Rücksetzung.

---

# 10. Fortsetzen

Beim Fortsetzen:

- wird der aktuelle Zustand übernommen.
- läuft die Animation exakt an derselben Stelle weiter.

Ein Neustart ist unzulässig.

---

# 11. Richtungswechsel

Eine Animation darf ihre Richtung ändern.

Beispiel:

```text
0 %

↓

60 %

↓

40 %

↓

20 %

↓

0 %
```

Der Richtungswechsel erfolgt ohne sichtbaren Sprung.

---

# 12. Zieländerung

Während einer laufenden Animation darf das Ziel geändert werden.

Beispiel:

```text
Position A

↓

Position B

↓

Position C
```

Die Bewegung erfolgt kontinuierlich vom aktuellen Zustand zum neuen Ziel. Unterbrechbare Animationen sollen dabei ihre aktuelle Geschwindigkeit möglichst beibehalten, um abrupte Bewegungswechsel zu vermeiden. :contentReference[oaicite:1]{index=1}

---

# 13. Geschwindigkeitsübernahme

Beim Zielwechsel wird übernommen:

- aktuelle Geschwindigkeit
- aktuelle Beschleunigung
- aktueller Fortschritt

Dadurch entstehen keine sichtbaren Geschwindigkeitssprünge.

---

# 14. Ersetzen

Eine Animation darf vollständig ersetzt werden.

Dabei gilt:

```text
Alte Animation

↓

aktueller Zustand

↓

Neue Animation
```

Es erfolgt keine Rückkehr zum Ausgangszustand.

---

# 15. Mehrere Unterbrechungen

Mehrfache Unterbrechungen sind zulässig.

Der Scheduler verarbeitet sie in zeitlicher Reihenfolge.

---

# 16. Materialanimationen

Unterbrechbar sind:

- Blur
- Tint
- Glow
- Shadow
- Transparenz
- Border

Alle Materialparameter bleiben synchron.

---

# 17. Dialoge

Während Enter Motion:

- darf Dialog schließen.
- darf Dialog ersetzt werden.

Die Animation beginnt jeweils am aktuellen Zustand.

---

# 18. Navigation

Navigationstransitionen dürfen:

- umgekehrt
- ersetzt
- neu gestartet

werden.

Die Navigationshierarchie bleibt unverändert.

---

# 19. Fokusanimationen

Wechselt der Fokus während einer laufenden Animation:

- endet die alte Animation am aktuellen Zustand.
- startet unmittelbar die neue Focus Motion.

Es existiert niemals mehr als ein aktives Fokusziel.

---

# 20. Progress Animationen

Progress Animationen dürfen jederzeit:

- neuen Fortschritt übernehmen
- Richtung ändern
- abgeschlossen werden

Der Fortschritt darf niemals rückwärts laufen, außer wenn dies ausdrücklich vom Progress Manager angefordert wird.

---

# 21. Qualitätsprofile

Je nach Qualitätsprofil dürfen reduziert werden:

- Spring
- Bounce
- Materialanimationen

Die Unterbrechbarkeit bleibt vollständig erhalten.

---

# 22. Reduced Motion

Bei aktiviertem Reduced Motion:

- bleiben Animationen unterbrechbar.
- Bewegungen werden reduziert.
- Fade bleibt erhalten.

Die Reaktionsfähigkeit der Oberfläche darf nicht eingeschränkt werden.

---

# 23. Speicherverwaltung

Alle unterbrechbaren Animationen verwenden:

- festen Objektpool
- feste Schedulerlisten

Heap-Allokationen während der Laufzeit sind verboten.

---

# 24. Performance

Anforderungen:

- O(n)-Scheduler
- deterministische Laufzeit
- keine rekursiven Unterbrechungen
- keine Heap-Allokationen

Eine Unterbrechung muss innerhalb desselben Frames verarbeitet werden.

---

# 25. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Animation
- doppelte Unterbrechung
- beschädigte Timeline
- ungültiges Ziel
- Nullpointer

Im Fehlerfall wird die Animation kontrolliert beendet.

---

# 26. Sicherheit

Das System verhindert:

- doppelte Animationen
- Endlosschleifen
- widersprüchliche Zielzustände
- Nullpointer
- rekursive Unterbrechungen

Alle Zustandswechsel werden vor ihrer Ausführung validiert.

---

# 27. API

```c
nova_result_t
nova_animation_interrupt(
    nova_animation_t* animation);

nova_result_t
nova_animation_resume(
    nova_animation_t* animation);

nova_result_t
nova_animation_reverse(
    nova_animation_t* animation);

nova_result_t
nova_animation_redirect(
    nova_animation_t* animation,
    const nova_animation_target_t* target);

bool
nova_animation_is_interruptible(
    const nova_animation_t* animation);
```

---

# 28. Diagnosedaten

Zu erfassen sind:

- Unterbrechungen
- Richtungswechsel
- Zieländerungen
- Fortsetzungen
- Pausen
- Schedulerlaufzeit
- aktive Animationen
- Fehleranzahl

---

# 29. Testfälle

Zu testen sind:

- Animation unterbrechen
- Animation pausieren
- Animation fortsetzen
- Richtungswechsel
- Zieländerung
- Dialog während Enter Motion schließen
- Fokuswechsel während Animation
- Navigation unterbrechen
- Reduced Motion
- Recovery Mode

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Animationen jederzeit unterbrochen werden können.
- Zieländerungen ohne sichtbare Sprünge erfolgen.
- aktuelle Geschwindigkeit übernommen wird.
- Dialog- und Navigationstransitionen korrekt unterbrochen werden.
- Reduced Motion vollständig unterstützt wird.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 31. Referenzen

## ADRs

- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen
- ADR-BOOTMOTION-0006 – Animierte Szenenübergänge
- ADR-BOOTINPUT-0001 – Vereinheitlichte Eingabeereignisse
- ADR-BOOTINPUT-0005 – Eingabefokus und Fokusnavigation
- ADR-BOOTNAV-0001 – Zustandsbasierte Navigation

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0002 – Animation Timeline
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTMOTION-0005 – Easing Functions
- NPSPEC-BOOTMOTION-0006 – Spring Animations
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- NPSPEC-BOOTMOTION-0009 – Menu Navigation Transitions
- NPSPEC-BOOTMOTION-0010 – Focus and Selection Motion
- NPSPEC-BOOTMOTION-0011 – Progress Animations

---

# 32. Zusammenfassung

Die **Interruptible Animations** bilden die Grundlage für eine jederzeit reaktionsfähige NovaOS Bootoberfläche. Laufende Animationen können ohne sichtbare Sprünge pausiert, umgekehrt, ersetzt oder auf neue Zielzustände umgelenkt werden. Durch die Übernahme von Fortschritt und Geschwindigkeit, deterministische Scheduler-Abläufe sowie feste Speicherverwaltung entsteht ein modernes Motion-System, das auch bei schnellen Benutzereingaben flüssig, konsistent und identisch auf BIOS- und UEFI-Systemen arbeitet. :contentReference[oaicite:2]{index=2}