# NPSPEC-BOOTDIALOG-0010
# Dialog Animation

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-BOOTDIALOG-0010 |
| Titel | Dialog Animation |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Bootmanager / Dialog System |
| Priorität | Hoch |
| Abhängigkeiten | NPSPEC-BOOTDIALOG-0001 bis NPSPEC-BOOTDIALOG-0009, NPSPEC-BOOTMOTION-0001 bis NPSPEC-BOOTMOTION-0014 |
| Zugehörige ADRs | ADR-BOOTDIALOG-0010, ADR-BOOTMOTION-0008, ADR-BOOTMOTION-0007, ADR-BOOTPERF-0003, ADR-BOOTACCESS-0002 |

---

# 1. Ziel

Diese Spezifikation definiert das **Dialog Animation System** der NovaOS Boot UI.

Das System beschreibt sämtliche Animationen beim Öffnen, Schließen und Aktualisieren von Dialogen.

Es unterstützt:

- Einblendanimationen
- Ausblendanimationen
- Fokusanimationen
- Größenänderungen
- Materialanimationen
- Backdrop-Animationen
- Zustandswechsel
- Reduced Motion

Animationen verbessern die Orientierung des Benutzers, ohne den Bootvorgang zu verzögern.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Dialoge.

Sie umfasst:

- Informationsdialoge
- Warnungen
- Fehlerdialoge
- Passwortdialoge
- Fortschrittsdialoge
- Recovery-Dialoge
- modale Dialoge

Nicht Bestandteil sind:

- Seitenübergänge
- Fensteranimationen
- Layoutberechnung

---

# 3. Architektur

```text
Dialog Manager

↓

Motion Manager

↓

Animation Timeline

↓

Compositor

↓

Rendering
```

---

# 4. Grundprinzip

Jeder Dialog besitzt einen definierten Animationslebenszyklus.

Animationen:

- visualisieren Zustandswechsel
- verändern keine Dialogdaten
- sind jederzeit unterbrechbar
- arbeiten deterministisch

---

# 5. Aufgaben

Das Animationssystem übernimmt:

- Öffnungsanimation
- Schließanimation
- Fokusanimation
- Materialanimation
- Backdropanimation
- Zustandswechsel
- Animation Sync
- Performanceanpassung

---

# 6. Animationsphasen

Jede Dialoganimation besteht aus:

```text
Prepare

↓

Start

↓

Running

↓

Finish

↓

Completed
```

Unterbrochene Animationen wechseln in den Zustand **Cancelled**.

---

# 7. Öffnungsanimation

Standard:

```text
Opacity

0 %

↓

100 %

+

Scale

96 %

↓

100 %
```

Der Dialog erscheint weich und ohne sichtbares Springen.

---

# 8. Schließanimation

Standard:

```text
Opacity

100 %

↓

0 %

+

Scale

100 %

↓

98 %
```

Nach Abschluss wird der Dialog freigegeben.

---

# 9. Backdrop Animation

Beim Öffnen:

```text
Opacity

0 %

↓

100 %
```

Beim Schließen erfolgt der Ablauf in umgekehrter Reihenfolge.

Backdrop und Dialog bleiben synchron.

---

# 10. Glass Animation

Glass-Eigenschaften werden gemeinsam animiert:

- Transparenz
- Blur
- Lichtreflexion
- Schatten

Alle Materialänderungen erfolgen synchron mit der Dialoganimation.

---

# 11. Fokusanimation

Beim Fokuswechsel:

- sanfte Hervorhebung
- kurzer Übergang
- keine Positionsänderung

Die Animation darf Eingaben nicht verzögern.

---

# 12. Größenänderungen

Ändert sich der Inhalt:

- erfolgt eine animierte Größenanpassung.

Die Animation darf Layoutsprünge verhindern.

---

# 13. Fortschrittsdialoge

Fortschrittsdialoge aktualisieren:

- Fortschrittsbalken
- Status
- Prozentanzeige

ohne vollständige Dialoganimation.

---

# 14. Unterbrechbarkeit

Alle Dialoganimationen sind:

- unterbrechbar
- pausierbar
- umkehrbar

Ein neuer Dialog darf eine laufende Animation kontrolliert ersetzen.

---

# 15. Reduced Motion

Ist Reduced Motion aktiv:

Standard:

```text
Fade

+

Scale
```

Reduced Motion:

```text
Fade
```

Zoom-, Blur- und Materialanimationen werden reduziert oder deaktiviert.

---

# 16. Synchronisation

Folgende Elemente werden synchron animiert:

- Dialog
- Backdrop
- Glass Material
- Fokus
- Schatten

Asynchrone Übergänge sind unzulässig.

---

# 17. Accessibility

Animationen berücksichtigen:

- Reduced Motion
- High Contrast
- Screenreader
- Tastaturbedienung

Screenreader-Ausgaben dürfen nicht durch Animationen verzögert werden.

---

# 18. Speicherverwaltung

Dialoganimationen verwenden:

- statische Timelines
- feste Keyframes
- feste Animationsobjekte

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Animationsstart
- deterministische Laufzeit
- keine Heap-Allokationen

Animationen müssen innerhalb des definierten Performance Budgets ausgeführt werden.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültige Timeline
- doppelte Animation
- fehlende Keyframes
- beschädigte Materialdaten
- Nullpointer

Im Fehlerfall wird der Dialog sofort ohne Animation dargestellt.

---

# 21. Sicherheit

Das System verhindert:

- mehrere aktive Dialoganimationen für denselben Dialog
- inkonsistente Zustandswechsel
- rekursive Animationen
- Fokusverlust
- beschädigte Animationszustände

---

# 22. API

```c
typedef enum
{
    NOVA_DIALOG_ANIMATION_OPEN,
    NOVA_DIALOG_ANIMATION_CLOSE,
    NOVA_DIALOG_ANIMATION_RESIZE,
    NOVA_DIALOG_ANIMATION_FOCUS
}
nova_dialog_animation_t;

nova_result_t
nova_dialog_animation_start(
    nova_dialog_t* dialog,
    nova_dialog_animation_t animation);

nova_result_t
nova_dialog_animation_cancel(
    nova_dialog_t* dialog);

bool
nova_dialog_animation_running(
    const nova_dialog_t* dialog);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Animationstyp
- Laufzeit
- Abbrüche
- Reduced Motion Status
- Materialanimation
- Fokusanimation
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Öffnen
- Schließen
- Größenänderung
- Fokusanimation
- Backdrop
- Glass Material
- Reduced Motion
- Accessibility
- Unterbrechung
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Dialoge definierte Ein- und Ausblendanimationen besitzen.
- Backdrop und Dialog synchron animiert werden.
- Größenänderungen ohne Layoutsprünge erfolgen.
- Animationen jederzeit unterbrochen werden können.
- Reduced Motion vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Animationsverhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTDIALOG-0010 – Dialoganimationen
- ADR-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- ADR-BOOTMOTION-0007 – Transition System
- ADR-BOOTPERF-0003 – Animation Performance Budget
- ADR-BOOTACCESS-0002 – Reduced Motion

## NPSPECs

- NPSPEC-BOOTDIALOG-0001 – Boot Dialog Architecture
- NPSPEC-BOOTDIALOG-0003 – Glass Dialog
- NPSPEC-BOOTMOTION-0001 – Boot Motion System
- NPSPEC-BOOTMOTION-0008 – Dialog Enter and Exit Motion
- NPSPEC-BOOTMOTION-0012 – Interruptible Animations
- NPSPEC-BOOTMOTION-0013 – Reduced Motion Mode

---

# 27. Zusammenfassung

Das **Dialog Animation System** definiert sämtliche Animationen der NovaOS Bootdialoge. Es umfasst Ein- und Ausblendanimationen, Material- und Backdrop-Animationen, Fokusübergänge sowie animierte Größenänderungen. Durch deterministische Abläufe, vollständige Synchronisation aller visuellen Komponenten, Unterstützung von Reduced Motion und statische Speicherverwaltung entsteht ein flüssiges, konsistentes und performantes Dialogerlebnis auf BIOS- und UEFI-Systemen.