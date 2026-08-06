# NPSPEC-BOOTCONTROL-0002
# Control State Model

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-0002 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Framework |
| Priorität | Kritisch |
| Gültigkeitsbereich | Sämtliche UI-Controls des NovaOS Boot Managers |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTINPUT-0005, NPSPEC-BOOTMOTION-0004, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Control State Model** aller UI-Controls der NovaOS Bootoberfläche.

Das Zustandsmodell beschreibt sämtliche logischen Zustände eines Controls sowie deren erlaubte Übergänge.

Alle Controls verwenden dasselbe Zustandsmodell, unabhängig vom konkreten Control-Typ.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Button
- Label
- TextBox
- CheckBox
- RadioButton
- ProgressBar
- Image
- List
- ScrollView
- Container
- zukünftige Controls

---

# 3. Architektur

```text
Input

↓

State Manager

↓

Control State

↓

Animation

↓

Rendering
```

---

# 4. Grundprinzip

Ein Control befindet sich jederzeit in genau einem logischen Hauptzustand.

Zusätzliche Statusflags dürfen parallel aktiv sein.

Der Zustandswechsel erfolgt ausschließlich über den State Manager.

---

# 5. Aufgaben

Das State Model verwaltet:

- Lebenszyklus
- Interaktionszustände
- Sichtbarkeit
- Aktivierung
- Fokus
- Auswahl
- Animationstrigger
- Ereignisse

---

# 6. Hauptzustände

Jedes Control besitzt folgende Hauptzustände:

```text
Created

↓

Initialized

↓

Visible

↓

Active

↓

Disabled

↓

Destroyed
```

Kein Zustand darf übersprungen werden.

---

# 7. Interaktionszustände

Während **Active** können zusätzlich aktiv sein:

```text
Normal

Hovered

Focused

Pressed

Selected

Checked

Busy
```

Mehrere Statusflags dürfen gleichzeitig gesetzt sein.

---

# 8. Sichtbarkeitszustände

Unterstützt werden:

```text
Visible

Hidden

Collapsed
```

**Hidden**

- nicht sichtbar
- Layout bleibt erhalten

**Collapsed**

- vollständig aus Layout entfernt

---

# 9. Fokuszustände

```text
Focusable

↓

Focused

↓

Blurred
```

Nur fokussierbare Controls dürfen den Zustand **Focused** annehmen.

---

# 10. Aktivierungszustände

```text
Enabled

↓

Disabled

↓

Enabled
```

Disabled Controls:

- reagieren nicht auf Eingaben
- bleiben renderbar
- behalten ihren Zustand

---

# 11. Auswahlzustände

Für auswählbare Controls:

```text
Unselected

↓

Selected
```

Checkboxen:

```text
Unchecked

↓

Checked

↓

Indeterminate
```

---

# 12. Busy State

Busy signalisiert:

- laufende Verarbeitung
- temporäre Sperre
- Hintergrundaktivität

Busy blockiert keine Darstellung.

---

# 13. Fehlerzustand

Optional:

```text
Error
```

Eigenschaften:

- Fehlerdarstellung
- Fehlerstil
- Fehlerbeschreibung

Der Fehlerzustand ersetzt nicht den Lebenszyklus.

---

# 14. Zustandsübergänge

Erlaubte Übergänge:

```text
Normal

↓

Hovered

↓

Focused

↓

Pressed

↓

Normal
```

Ungültige Übergänge werden verworfen.

---

# 15. Animationen

Jeder Zustandswechsel kann Animationen auslösen:

- Fade
- Focus Glow
- Press Animation
- Scale
- Color Transition

Animationen verändern niemals den eigentlichen Zustand.

---

# 16. Ereignisse

Bei jedem Zustandswechsel entstehen:

- StateChanging
- StateChanged

Diese Ereignisse werden über das Event-System verteilt.

---

# 17. Accessibility

Jeder Zustand besitzt ein Accessibility-Pendant.

Beispiele:

```text
Focused

↓

Accessibility Focus

Disabled

↓

Unavailable

Selected

↓

Selected State
```

---

# 18. Theme Integration

Jeder Zustand besitzt eigene Themeparameter:

- Hintergrundfarbe
- Rahmenfarbe
- Textfarbe
- Schatten
- Transparenz
- Animation

Themes definieren ausschließlich die Darstellung.

---

# 19. Speicherverwaltung

Der Zustand wird gespeichert als:

- Bitflags
- Statusstruktur
- feste Enumerationen

Keine dynamischen Zustandsobjekte.

Heap-Allokationen sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Zustandswechsel
- O(1)-Statusprüfung
- deterministische Laufzeit
- keine Heap-Allokationen

Nur betroffene Controls werden aktualisiert.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Zustand
- mehrfacher Hauptzustand
- ungültiger Übergang
- beschädigte Statusflags
- Nullpointer

Ungültige Übergänge werden ignoriert und protokolliert.

---

# 22. Sicherheit

Das System verhindert:

- mehrere Hauptzustände gleichzeitig
- ungültige Übergänge
- beschädigte Statusdaten
- rekursive Zustandswechsel
- inkonsistente Fokuszustände

---

# 23. API

```c
typedef enum
{
    NOVA_CONTROL_STATE_CREATED,
    NOVA_CONTROL_STATE_INITIALIZED,
    NOVA_CONTROL_STATE_VISIBLE,
    NOVA_CONTROL_STATE_ACTIVE,
    NOVA_CONTROL_STATE_DISABLED,
    NOVA_CONTROL_STATE_DESTROYED
}
nova_control_state_t;

typedef uint32_t nova_control_flags_t;

nova_result_t
nova_control_set_state(
    nova_control_t* control,
    nova_control_state_t state);

nova_result_t
nova_control_set_flags(
    nova_control_t* control,
    nova_control_flags_t flags);

nova_control_state_t
nova_control_get_state(
    const nova_control_t* control);

nova_control_flags_t
nova_control_get_flags(
    const nova_control_t* control);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- aktueller Zustand
- Statusflags
- Zustandswechsel
- Fokuswechsel
- Fehlerzustände
- Busy-Zeiten
- Speicherverbrauch
- Fehleranzahl

---

# 25. Testfälle

Zu testen sind:

- Lebenszyklus
- Fokuswechsel
- Hover
- Pressed
- Disabled
- Hidden
- Collapsed
- Busy
- Error
- Accessibility
- Themewechsel

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jedes Control genau einen Hauptzustand besitzt.
- Statusflags korrekt kombiniert werden können.
- ungültige Zustandswechsel verhindert werden.
- Animationen ausschließlich auf Zustandswechsel reagieren.
- Accessibility vollständig synchronisiert wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Zustandsverhalten erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTCONTROL-0002 – Einheitliches Zustandsmodell
- ADR-BOOTINPUT-0005 – Fokusverwaltung
- ADR-BOOTMOTION-0004 – Animierbare Eigenschaften
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische Zustandsverwaltung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTINPUT-0005 – Focus Management
- NPSPEC-BOOTMOTION-0004 – Animatable Properties
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework
- NPSPEC-BOOTRENDER-0001 – Rendering Engine

---

# 28. Zusammenfassung

Das **Control State Model** definiert das einheitliche Zustandsmodell sämtlicher UI-Controls der NovaOS Bootoberfläche. Es trennt Lebenszyklus, Interaktionszustände und Statusflags klar voneinander und gewährleistet deterministische Zustandsübergänge, vollständige Integration in Rendering, Animationen und Accessibility sowie eine statische Speicherverwaltung ohne Heap-Allokationen. Dadurch verhalten sich alle Controls konsistent und vorhersehbar auf BIOS- und UEFI-Systemen.