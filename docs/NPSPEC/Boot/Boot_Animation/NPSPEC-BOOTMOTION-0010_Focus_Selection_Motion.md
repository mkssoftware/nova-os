# NPSPEC-BOOTMOTION-0010
# Focus and Selection Motion

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTMOTION-0010 |
| Titel | Focus and Selection Motion |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Motion |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0009, NPSPEC-BOOTINPUT-0005 |
| Zugehörige ADRs | ADR-BOOTMOTION-0003, ADR-BOOTMOTION-0005, ADR-BOOTINPUT-0005, ADR-BOOTCONTROL-0003, ADR-BOOTACCESS-0004 |

---

# 1. Ziel

Diese Spezifikation definiert sämtliche **Focus- und Selection-Animationen** der NovaOS Boot UI.

Sie beschreibt sämtliche visuellen Bewegungen beim:

- Erhalten des Fokus
- Verlust des Fokus
- Wechsel des Fokus
- Auswählen eines Elements
- Aktivieren eines Controls
- Navigieren per Tastatur
- Navigieren per Maus
- Navigieren per Touch

Das Ziel besteht darin, jederzeit eindeutig sichtbar zu machen:

- welches Element den Fokus besitzt,
- welches Element ausgewählt ist,
- welche Aktion gerade ausgeführt wird.

Ein klar sichtbarer Fokus verbessert die Orientierung und sollte ausschließlich als Reaktion auf Benutzerinteraktionen wechseln. :contentReference[oaicite:0]{index=0}

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche fokussierbaren UI-Elemente.

Sie umfasst:

- Buttons
- Listen
- Menüs
- Checkboxen
- Radiobuttons
- Textfelder
- Dialoge
- Panels
- Listenzeilen

Nicht Bestandteil sind:

- Mauszeiger
- Fensteranimationen
- Navigationstransitionen

---

# 3. Architektur

```text
Input Event

↓

Focus Manager

↓

Selection Manager

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

Fokus und Auswahl sind zwei unterschiedliche Zustände.

Ein Element kann:

- Fokus besitzen
- ausgewählt sein
- beides gleichzeitig sein

Die Animationen dieser Zustände dürfen unabhängig voneinander ausgeführt werden.

---

# 5. Aufgaben

Das Motion System übernimmt:

- Fokusanimation
- Auswahlanimation
- Fokuswechsel
- Fokusverlust
- Aktivierungsanimation
- Tastaturfeedback
- Mausfeedback
- Touchfeedback

---

# 6. Fokuszustände

Jedes Element besitzt folgende Zustände:

```text
Normal

↓

Focused

↓

Pressed

↓

Selected

↓

Disabled
```

---

# 7. Fokuswechsel

Beim Wechsel des Fokus erfolgt:

```text
Alter Fokus

↓

Focus Exit Motion

↓

Neuer Fokus

↓

Focus Enter Motion
```

Beide Animationen laufen synchron.

---

# 8. Focus Enter

Standardanimation:

| Property | Start | Ende |
|---|---:|---:|
| Deckkraft | 60 % | 100 % |
| Skalierung | 98 % | 100 % |
| Glow | 0 % | 100 % |

---

# 9. Focus Exit

Standardanimation:

| Property | Start | Ende |
|---|---:|---:|
| Glow | 100 % | 0 % |
| Deckkraft Fokusring | 100 % | 0 % |

---

# 10. Selection Motion

Beim Auswählen eines Elements dürfen animiert werden:

- Hintergrundfarbe
- Border
- Glow
- Shadow
- Tint
- Deckkraft

Die Auswahlanimation beginnt unmittelbar nach erfolgreicher Aktivierung.

---

# 11. Press Motion

Beim Drücken eines Controls:

| Property | Start | Ende |
|---|---:|---:|
| Skalierung | 100 % | 97 % |
| Shadow | Normal | Reduziert |

Beim Loslassen erfolgt die Rückanimation.

---

# 12. Aktivierung

Nach erfolgreicher Aktivierung:

- kurzer Highlight-Effekt
- anschließend Selection Motion

Die Aktivierungsanimation darf höchstens 120 ms dauern.

---

# 13. Fokusindikator

Der Fokusindikator besteht aus:

- Focus Ring
- Glow
- Highlight
- Border

Alle Komponenten werden gemeinsam animiert.

---

# 14. Listen

Beim Wechsel zwischen Listeneinträgen:

- Highlight verschieben
- alter Eintrag ausblenden
- neuer Eintrag hervorheben

Der Fokusindikator springt niemals abrupt.

---

# 15. Menüs

Beim Navigieren durch Menüs:

- Fokus gleitet zum nächsten Eintrag
- Highlight wird animiert
- Hintergrund bleibt stabil

Dadurch bleibt die Orientierung erhalten.

---

# 16. Dialoge

Beim Öffnen eines Dialogs:

- Fokus erscheint auf dem Standard-Element
- Focus Enter Motion startet automatisch

Beim Schließen wird der Fokus auf das zuvor aktive Element zurückgesetzt.

---

# 17. Maus

Beim Überfahren eines Elements:

- Hover Motion

Beim Anklicken:

- Press Motion
- anschließend Selection Motion

Hover ersetzt niemals den Tastaturfokus.

---

# 18. Tastatur

Beim Navigieren per Tastatur:

- jeder Fokuswechsel wird animiert
- Fokusring bleibt jederzeit sichtbar

Der Fokus darf niemals ohne Benutzereingabe auf ein anderes Element springen. :contentReference[oaicite:1]{index=1}

---

# 19. Touch

Bei Touch-Eingaben:

- kurzer Press-Effekt
- anschließend Selection Motion

Ein dauerhafter Fokusring wird nicht dargestellt.

---

# 20. Animationen

Standardwerte:

| Animation | Dauer |
|---|---:|
| Focus Enter | 120 ms |
| Focus Exit | 100 ms |
| Selection | 140 ms |
| Press | 80 ms |

Alle Werte stammen aus den Motion Tokens.

---

# 21. Easing

Standard:

- Focus → Ease Out Cubic
- Selection → Ease Out Cubic
- Press → Spring (Critical)

---

# 22. Reduced Motion

Bei aktiviertem Reduced Motion:

- Skalierung deaktivieren
- Glow reduzieren
- Fade beibehalten
- Fokusring sofort darstellen

Die Sichtbarkeit des Fokus darf niemals eingeschränkt werden. :contentReference[oaicite:2]{index=2}

---

# 23. Qualitätsprofile

Je nach Qualitätsprofil dürfen reduziert werden:

- Glow
- Shadow
- Materialanimation
- Skalierung

Der Fokusring bleibt immer sichtbar.

---

# 24. Speicherverwaltung

Alle Fokusanimationen verwenden:

- festen Animationspool
- feste Materialobjekte

Heap-Allokationen während der Laufzeit sind verboten.

---

# 25. Performance

Anforderungen:

- deterministische Laufzeit
- O(n)-Scheduler
- synchronisierte Fokusanimationen
- keine Heap-Allokationen

---

# 26. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiges Fokusziel
- fehlendes UI-Element
- doppelte Fokuszuweisung
- beschädigte Animation
- ungültiger Fokuszustand

Im Fehlerfall wird der Fokus ohne Animation gesetzt.

---

# 27. Sicherheit

Das System verhindert:

- mehrere aktive Fokusobjekte
- rekursive Fokuswechsel
- Endlosschleifen
- Nullpointer
- ungültige Selektion

Zu jedem Zeitpunkt existiert höchstens ein aktives Fokusobjekt.

---

# 28. API

```c
nova_result_t
nova_focus_enter(
    nova_control_t* control);

nova_result_t
nova_focus_exit(
    nova_control_t* control);

nova_result_t
nova_selection_play(
    nova_control_t* control);

bool
nova_focus_is_animating(
    const nova_control_t* control);
```

---

# 29. Diagnosedaten

Zu erfassen sind:

- Fokuswechsel
- Selektionen
- Press-Animationen
- Hover-Animationen
- Schedulerlaufzeit
- aktive Fokusobjekte
- Fehleranzahl
- Reduced Motion Status

---

# 30. Testfälle

Zu testen sind:

- Fokus per Tastatur
- Fokus per Maus
- Fokus per Touch
- Listen
- Menüs
- Dialoge
- Press Motion
- Selection Motion
- Reduced Motion
- Recovery Mode

---

# 31. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Fokuswechsel animiert werden.
- Fokus und Auswahl unabhängig voneinander funktionieren.
- Hover den Tastaturfokus nicht ersetzt.
- Press- und Selection-Animationen korrekt ausgeführt werden.
- Reduced Motion vollständig unterstützt wird.
- der Fokus jederzeit eindeutig sichtbar bleibt.
- keine Heap-Allokationen im Motion-Hotpath stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten zeigen.

---

# 32. Referenzen

## ADRs

- ADR-BOOTMOTION-0003 – Property-basierte Animationen
- ADR-BOOTMOTION-0005 – Unterbrechbare und reversible Animationen
- ADR-BOOTINPUT-0005 – Eingabefokus und Fokusnavigation
- ADR-BOOTCONTROL-0003 – Einheitliches Fokusmodell
- ADR-BOOTACCESS-0004 – Sichtbare Fokusindikatoren

## NPSPECs

- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0003 – Animation Scheduler
- NPSPEC-BOOTMOTION-0005 – Easing Functions
- NPSPEC-BOOTMOTION-0007 – Transition System
- NPSPEC-BOOTINPUT-0005 – Input Focus Management
- NPSPEC-BOOTCONTROL-0003 – Control Focus Model

---

# 33. Zusammenfassung

Das **Focus and Selection Motion System** definiert sämtliche Animationen für Fokus- und Auswahlzustände innerhalb der NovaOS Bootoberfläche. Durch konsistente Focus-, Press- und Selection-Animationen sowie eine klare Trennung zwischen Fokus und Auswahl entsteht eine jederzeit nachvollziehbare Benutzerführung. Die deterministische Ausführung, adaptive Qualitätsprofile und vollständige Unterstützung von Reduced Motion gewährleisten eine barrierefreie, performante und konsistente Bedienung auf BIOS- und UEFI-Systemen. :contentReference[oaicite:3]{index=3}