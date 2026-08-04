# NPSPEC-BOOTMOTION-0007
# Transition System

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0007 |
| Titel | Transition System |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0006, NPSPEC-BOOTCOMPOSITOR-0001 |
| Zugehörige ADRs | ADR-BOOTMOTION-0003, ADR-BOOTMOTION-0005, ADR-BOOTMOTION-0006, ADR-BOOTDIALOG-0001, ADR-BOOTNAV-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Transition System** der NovaOS Boot UI.

Das Transition System steuert sämtliche Zustandsübergänge der Benutzeroberfläche und sorgt dafür, dass Änderungen klar nachvollziehbar, konsistent und flüssig dargestellt werden.

Es verwaltet unter anderem:

- Szenenwechsel
- Dialogübergänge
- Menüwechsel
- Overlay-Einblendungen
- Seitenwechsel
- Materialübergänge
- Zustandswechsel
- Navigationstransitionen

Alle Transitionen basieren auf dem zentralen Motion System und werden vollständig vom Animation Scheduler gesteuert. Moderne UI-Systeme verwenden zentrale Transition-Frameworks, um Zustandswechsel konsistent und nachvollziehbar zu animieren. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche UI-Zustandswechsel.

Sie umfasst:

- Dialoge
- Menüs
- Panels
- Controls
- Overlays
- Recovery UI
- Einstellungen
- Bootmenüs

Nicht Bestandteil sind:

- Bootlogo
- Kernelinitialisierung
- Hardwareinitialisierung

---

# 3. Architektur

```text
UI Event

↓

Transition Manager

↓

Transition Scheduler

↓

Animation Scheduler

↓

Property Animator

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Eine Transition beschreibt niemals einzelne Animationen.

Sie beschreibt ausschließlich den Übergang zwischen zwei definierten UI-Zuständen.

Die eigentlichen Animationen werden automatisch erzeugt.

---

# 5. Aufgaben

Das Transition System übernimmt:

- Zustandswechsel
- Übergangsdefinition
- Synchronisation
- Parallelisierung
- Transitiongruppen
- Materialübergänge
- Navigation
- Ereignissteuerung

---

# 6. Transitionbeschreibung

```c
typedef struct
{
    uint32_t id;

    uint32_t type;

    uint32_t duration;

    uint32_t easing;

    bool interruptible;

} nova_transition_t;
```

---

# 7. Transitionarten

Unterstützt werden:

- Scene Transition
- Dialog Transition
- Overlay Transition
- Material Transition
- Navigation Transition
- Control Transition
- State Transition
- Visibility Transition

---

# 8. Lebenszyklus

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

# 9. Trigger

Eine Transition kann ausgelöst werden durch:

- Benutzereingabe
- Navigation
- Statusänderung
- Systemereignis
- Recovery
- Animation

---

# 10. Zustände

Jede Transition besitzt:

- Startzustand
- Endzustand

Der Startzustand bleibt unverändert bis zum eigentlichen Beginn der Transition.

---

# 11. Standardübergänge

Standardmäßig unterstützt das System:

- Fade
- Slide
- Scale
- Zoom
- Material Fade
- Cross Fade

Weitere Übergänge können ergänzt werden.

---

# 12. Dialog Transition

Dialoge verwenden standardmäßig:

- Fade In
- Scale In
- Background Blur

Beim Schließen erfolgt:

- Fade Out
- Scale Out

---

# 13. Menü Transition

Menüs verwenden:

- Slide
- Fade

Die Richtung richtet sich nach der Navigationshierarchie.

---

# 14. Overlay Transition

Overlays verwenden:

- Fade
- Material Transition

Die Hintergrundverdunklung wird synchron animiert.

---

# 15. Navigation Transition

Navigationen bestehen aus:

- alter Zustand ausblenden
- neuer Zustand einblenden

Beide Animationen laufen synchron.

---

# 16. Material Transition

Animiert werden:

- Blur
- Tint
- Shadow
- Glow
- Transparenz

Alle Materialparameter werden gleichzeitig aktualisiert.

---

# 17. Übergangsgruppen

Mehrere Transitionen dürfen gruppiert werden.

Alle Transitionen einer Gruppe:

- starten gemeinsam
- enden gemeinsam
- können gemeinsam abgebrochen werden

---

# 18. Unterbrechung

Transitionen dürfen unterbrochen werden.

Beim Neustart erfolgt keine Rücksetzung.

Die neue Transition übernimmt den aktuellen Animationszustand.

---

# 19. Synchronisation

Alle Transitionen werden vor dem Rendern synchronisiert.

Ein Frame enthält niemals teilweise aktualisierte Transitionen.

---

# 20. Prioritäten

Prioritäten:

| Priorität | Verwendung |
|---|---|
| Kritisch | Sicherheitsdialog |
| Hoch | Dialog |
| Normal | Navigation |
| Niedrig | Dekoration |

---

# 21. Qualitätsprofile

Je nach Qualitätsprofil dürfen reduziert werden:

- Blur
- Materialanimation
- Shadow
- Glow

Die eigentliche Transition bleibt erhalten.

---

# 22. Reduced Motion

Bei aktiviertem Reduced Motion:

- Bewegungen werden reduziert.
- Fade bleibt erhalten.
- Materialwechsel bleiben erhalten.

Große Bewegungen entfallen.

---

# 23. Speicherverwaltung

Alle Transitionen stammen aus einem festen Objektpool.

Heap-Allokationen während der Laufzeit sind verboten.

---

# 24. Performance

Anforderungen:

- deterministische Laufzeit
- O(n)-Scheduler
- keine rekursiven Transitionen
- keine Heap-Allokationen

---

# 25. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Startzustand
- ungültiger Endzustand
- zyklische Transition
- doppelte Transition
- ungültige Dauer

Im Fehlerfall erfolgt unmittelbar der Wechsel ohne Animation.

---

# 26. Sicherheit

Das System verhindert:

- rekursive Transitionen
- Endlosschleifen
- Nullpointer
- doppelte Zustandswechsel
- beschädigte Transitiongruppen

---

# 27. API

```c
nova_result_t
nova_transition_initialize(void);

nova_result_t
nova_transition_begin(
    const nova_transition_t* transition);

void
nova_transition_update(
    uint64_t current_time);

void
nova_transition_cancel(
    uint32_t transition_id);

bool
nova_transition_running(
    uint32_t transition_id);
```

---

# 28. Diagnosedaten

Zu erfassen sind:

- aktive Transitionen
- abgeschlossene Transitionen
- Transitiondauer
- Abbrüche
- Gruppen
- Materialtransitionen
- Schedulerzeit
- Fehleranzahl

---

# 29. Testfälle

Zu testen sind:

- Dialog Transition
- Menü Transition
- Navigation Transition
- Overlay Transition
- Material Transition
- Gruppen
- Unterbrechung
- Reduced Motion
- Qualitätswechsel
- Recovery

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Zustandswechsel über das Transition System erfolgen.
- mehrere Transitionen synchron ausgeführt werden.
- Materialtransitionen korrekt unterstützt werden.
- Unterbrechungen ohne sichtbare Sprünge funktionieren.
- Reduced Motion vollständig unterstützt wird.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 31. Referenzen

## ADRs

- ADR-BOOTMOTION-0003 – Property-basierte Animationen
- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen
- ADR-BOOTMOTION-0006 – Animierte Szenenübergänge
- ADR-BOOTDIALOG-0001 – Modale Overlay-Dialoge für Untermenüs
- ADR-BOOTNAV-0002 – Hierarchischer Navigations-Stack

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0002 – Animation Timeline
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTMOTION-0004 – Animatable Properties
- NPSPEC-BOOTMOTION-0005 – Easing Functions
- NPSPEC-BOOTMOTION-0006 – Spring Animations
- NPSPEC-BOOTCOMPOSITOR-0001 – Boot Compositor

---

# 32. Zusammenfassung

Das **Transition System** bildet die zentrale Infrastruktur für sämtliche Zustandswechsel der NovaOS Bootoberfläche. Es definiert den Übergang zwischen zwei UI-Zuständen, synchronisiert alle beteiligten Animationen und integriert Material-, Dialog- und Navigationstransitionen in einen einheitlichen Ablauf. Durch deterministische Ausführung, feste Ressourcenverwaltung und adaptive Qualitätsprofile gewährleistet das System flüssige, konsistente und hardwareunabhängige Übergänge auf BIOS- und UEFI-Systemen. :contentReference[oaicite:1]{index=1}