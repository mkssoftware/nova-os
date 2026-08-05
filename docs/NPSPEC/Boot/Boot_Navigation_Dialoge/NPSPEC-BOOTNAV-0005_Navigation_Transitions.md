# NPSPEC-BOOTNAV-0005
# Navigation Transitions

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTNAV-0005 |
| Titel | Navigation Transitions |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Navigation |
| Priorität | Kritisch |
| Abhängigkeiten | NPSPEC-BOOTNAV-0001 bis NPSPEC-BOOTNAV-0004, NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0014 |
| Zugehörige ADRs | ADR-BOOTNAV-0005, ADR-BOOTMOTION-0006, ADR-BOOTMOTION-0007, ADR-BOOTPERF-0003, ADR-BOOTACCESS-0002 |

---

# 1. Ziel

Diese Spezifikation definiert sämtliche **Navigation Transitions** der NovaOS Boot UI.

Navigation Transitions beschreiben die visuellen Übergänge zwischen zwei Ansichten.

Sie dienen dazu:

- Navigationsrichtung zu verdeutlichen
- Zustandswechsel sichtbar zu machen
- Orientierung zu verbessern
- Fokuswechsel nachvollziehbar darzustellen
- Dialoge konsistent einzublenden
- den Benutzerfluss zu unterstützen

Transitions sind rein visuell und verändern niemals die eigentliche Navigation.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Navigationen innerhalb der Bootoberfläche.

Sie umfasst:

- Seitenwechsel
- Untermenüs
- Dialoge
- Recovery
- Einstellungen
- Diagnose
- Passwortdialoge
- Fehlerdialoge

Nicht Bestandteil sind:

- Rendering
- Layout
- Navigation Stack

---

# 3. Architektur

```text
Navigation Event

↓

Navigation Controller

↓

Transition Manager

↓

Motion System

↓

Compositor

↓

Framebuffer
```

---

# 4. Grundprinzip

Jeder Navigationswechsel besitzt exakt eine Transition.

Eine Transition:

- visualisiert ausschließlich den Zustandswechsel
- verändert niemals Navigationsdaten
- darf jederzeit unterbrochen werden
- ist vollständig deterministisch

---

# 5. Aufgaben

Das Transition-System übernimmt:

- Enter Motion
- Exit Motion
- Dialog Transition
- Back Transition
- Replace Transition
- Recovery Transition
- Focus Synchronisation
- Animation Coordination

---

# 6. Standardablauf

```text
Navigation

↓

Leave Animation

↓

Navigation Stack Update

↓

Enter Animation

↓

Focus Restore
```

Die Navigation erfolgt zwischen Exit- und Enter-Animation atomar.

---

# 7. Vorwärtsnavigation

Standardübergang:

```text
Current Page

↓

Slide + Fade

↓

Next Page
```

Die neue Ansicht erscheint aus der Navigationsrichtung.

---

# 8. Rücknavigation

Standardübergang:

```text
Current Page

↓

Reverse Slide

↓

Previous Page
```

Die Animation erfolgt exakt spiegelbildlich zur Vorwärtsnavigation.

---

# 9. Replace Navigation

Bei Replace:

```text
Current Page

↓

Cross Fade

↓

New Page
```

Da kein zusätzlicher Stackeintrag entsteht, wird keine Rückwärtsrichtung visualisiert.

---

# 10. Dialog Transition

Dialoge verwenden:

```text
Fade

+

Scale

+

Backdrop Fade
```

Der Hintergrund bleibt sichtbar.

---

# 11. Modale Dialoge

Modale Dialoge:

- blockieren Eingaben
- erhalten höchste Animationspriorität
- besitzen eigene Transitionen

---

# 12. Recovery Transition

Beim Wechsel in den Recovery-Bereich:

```text
Current Page

↓

Fade

↓

Recovery
```

Die Animation soll bewusst ruhiger wirken als normale Navigationen.

---

# 13. Fehlerdialoge

Fehlerdialoge erscheinen unmittelbar.

Sie verwenden:

- Fade
- leichte Skalierung
- Fokusanimation

Auf aufwendige Bewegungen wird verzichtet.

---

# 14. Transition States

Jede Transition besitzt folgende Zustände:

```text
Waiting

↓

Running

↓

Completed

↓

Cancelled
```

Unterbrochene Transitionen wechseln in den Zustand **Cancelled**.

---

# 15. Unterbrechbarkeit

Alle Navigationstransitionen sind:

- unterbrechbar
- pausierbar
- umkehrbar

Das Verhalten richtet sich nach NPSPEC-BOOTMOTION-0012.

---

# 16. Fokus

Der Fokus wird erst nach Abschluss der Enter-Transition aktiviert.

Während einer laufenden Transition dürfen keine Fokuswechsel stattfinden.

---

# 17. Reduced Motion

Ist Reduced Motion aktiv:

Standard:

```text
Slide + Fade
```

Reduced Motion:

```text
Fade
```

Zoom-, Spring- und Parallaxeffekte werden deaktiviert.

---

# 18. Performance Budget

Transitions unterliegen dem Animation Performance Budget.

Werden Ressourcen knapp, erfolgt folgende Reduktion:

1. Blur deaktivieren
2. Shadow reduzieren
3. Scale entfernen
4. Fade beibehalten

Die Orientierung des Benutzers muss jederzeit erhalten bleiben.

---

# 19. Layoutintegration

Nach Abschluss der Transition erfolgen:

- Focus Restore
- Dirty Region Update
- Render Flush

Layoutänderungen während einer laufenden Transition sind unzulässig.

---

# 20. Speicherverwaltung

Transitionen verwenden:

- feste Objektpools
- statische Timelines
- feste Animationstabellen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 21. Performance

Anforderungen:

- O(1)-Start
- deterministische Laufzeit
- keine rekursiven Transitionen
- keine Heap-Allokationen

Alle Transitionen müssen innerhalb des definierten Motion-Budgets ausgeführt werden.

---

# 22. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Transition
- doppelte Transition
- beschädigte Timeline
- fehlende Zielseite
- Nullpointer

Im Fehlerfall erfolgt der Seitenwechsel ohne Animation.

---

# 23. Sicherheit

Das System verhindert:

- mehrere aktive Navigationstransitionen
- widersprüchliche Richtungen
- rekursive Transitionen
- inkonsistente Fokuszustände
- beschädigte Animationszustände

---

# 24. API

```c
typedef enum
{
    NOVA_TRANSITION_PUSH,
    NOVA_TRANSITION_POP,
    NOVA_TRANSITION_REPLACE,
    NOVA_TRANSITION_DIALOG,
    NOVA_TRANSITION_RECOVERY
}
nova_transition_type_t;

nova_result_t
nova_transition_begin(
    nova_transition_type_t type);

nova_result_t
nova_transition_cancel(void);

bool
nova_transition_running(void);
```

---

# 25. Diagnosedaten

Zu erfassen sind:

- Transitiontyp
- Laufzeit
- Abbrüche
- Reduced Motion Status
- Performance Budget
- Fokusübernahmen
- Animationsdauer
- Fehleranzahl

---

# 26. Testfälle

Zu testen sind:

- Push Transition
- Pop Transition
- Replace Transition
- Dialog Transition
- Recovery Transition
- Fehlerdialog
- Reduced Motion
- Unterbrechung
- Performance Budget
- Recovery Mode

---

# 27. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede Navigation exakt eine definierte Transition besitzt.
- Vorwärts- und Rücknavigation spiegelbildliche Animationen verwenden.
- Replace Navigation als Cross Fade dargestellt wird.
- Reduced Motion vollständig unterstützt wird.
- Transitionen jederzeit unterbrochen werden können.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identische Übergänge erzeugen.

---

# 28. Referenzen

## ADRs

- ADR-BOOTNAV-0005 – Einheitliche Navigationstransitionen
- ADR-BOOTMOTION-0006 – Animierte Szenenübergänge
- ADR-BOOTMOTION-0007 – Zustandsbasierte Übergänge
- ADR-BOOTPERF-0003 – Adaptive Bildwiederholrate
- ADR-BOOTACCESS-0002 – Unterstützung von Reduced Motion

## NPSPECs

- NPSPEC-BOOTNAV-0001 – Boot Navigation Model
- NPSPEC-BOOTNAV-0004 – Back Navigation
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTMOTION-0012 – Interruptible Animations
- NPSPEC-BOOTMOTION-0013 – Reduced Motion Mode
- NPSPEC-BOOTMOTION-0014 – Animation Performance Budget

---

# 29. Zusammenfassung

Die **Navigation Transitions** definieren sämtliche visuellen Übergänge zwischen Ansichten der NovaOS Bootoberfläche. Sie visualisieren Navigationsrichtungen, koordinieren Fokuswechsel und sorgen für einen konsistenten Benutzerfluss, ohne den eigentlichen Navigationszustand zu verändern. Durch deterministische Abläufe, Unterstützung von Reduced Motion, vollständige Unterbrechbarkeit und feste Speicherverwaltung entstehen flüssige und reproduzierbare Seitenübergänge auf BIOS- und UEFI-Systemen.