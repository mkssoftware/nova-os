# NPSPEC-BOOTCONTROL-CHECKBOX-0001
# Checkbox

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-CHECKBOX-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Checkbox-Control** der NovaOS Boot UI.

Eine Checkbox ermöglicht dem Benutzer das Aktivieren oder Deaktivieren einer unabhängigen Option. Mehrere Checkboxen können gleichzeitig aktiviert sein und beeinflussen sich gegenseitig nicht.

Typische Einsatzbereiche:

- Bootoptionen
- Erweiterte Einstellungen
- Diagnoseoptionen
- Recovery-Funktionen
- Sicherheitsoptionen
- Bestätigungen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Checkboxen innerhalb der Bootoberfläche.

Sie umfasst:

- einfache Checkboxen
- Checkboxen mit Beschreibung
- tristate Checkboxen
- gruppierte Checkboxen

Nicht Bestandteil sind:

- Radio Buttons
- Toggle Switches
- Listen

---

# 3. Architektur

```text
Boot Control Framework

↓

Checkbox

↓

Template

↓

Renderer
```

---

# 4. Grundprinzip

Eine Checkbox repräsentiert genau einen booleschen Zustand.

Eine Checkbox darf niemals automatisch den Zustand anderer Checkboxen verändern.

---

# 5. Aufgaben

Die Checkbox übernimmt:

- Zustandsverwaltung
- Benutzerinteraktion
- Fokusdarstellung
- Themeintegration
- Animationen
- Accessibility
- Ereignisauslösung

---

# 6. Aufbau

```text
Checkbox

├── Box

├── Check Indicator

├── Label

├── Description (optional)

└── Focus Indicator
```

---

# 7. Eigenschaften

Jede Checkbox besitzt:

- ID
- Text
- Beschreibung (optional)
- Zustand
- Sichtbarkeit
- Style
- Template
- Enabled

---

# 8. Zustände

Unterstützt werden:

```text
Unchecked

↓

Checked
```

Optional:

```text
Unchecked

↓

Indeterminate

↓

Checked
```

---

# 9. Control States

Zusätzlich:

```text
Normal

Hovered

Focused

Pressed

Disabled

Busy
```

---

# 10. Größen

Empfohlene Mindestgröße:

| Element | Größe |
|---------|-------|
| Box | 20 × 20 DLU |
| Gesamtes Control | mindestens 120 × 28 DLU |

Die Größe skaliert DPI-unabhängig.

---

# 11. Layout

```text
☐

Spacing

Label

Description (optional)
```

Label und Beschreibung werden linksbündig ausgerichtet.

---

# 12. Eingaben

Unterstützt:

- Maus
- Tastatur
- Touch
- Controller

Aktivierung:

- Linksklick
- Enter
- Space
- Controller A

---

# 13. Zustandswechsel

```text
Unchecked

↓

Checked

↓

Unchecked
```

Bei Tristate:

```text
Unchecked

↓

Indeterminate

↓

Checked

↓

Unchecked
```

---

# 14. Darstellung

Die Checkbox verwendet:

- Glass Material
- Focus Glow
- animierten Haken
- Themefarben
- abgerundete Ecken

Der Haken wird ausschließlich über das Theme dargestellt.

---

# 15. Animationen

Unterstützt werden:

- Check Fade
- Check Scale
- Hover Fade
- Focus Glow
- Border Transition

Animationen beeinflussen ausschließlich die Darstellung.

---

# 16. Accessibility

Jede Checkbox besitzt:

- Name
- Rolle
- Beschreibung
- Checked Status
- Shortcut (optional)

Screenreader geben den aktuellen Zustand eindeutig aus.

---

# 17. Themeintegration

Das Theme definiert:

- Hintergrund
- Rahmen
- Haken
- Fokusfarbe
- Schrift
- Schatten
- Transparenz

Es existieren keine fest codierten Farben.

---

# 18. Speicherverwaltung

Checkboxen verwenden:

- Objektpools
- gemeinsame Templates
- gemeinsame Styles

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Zustandswechsel
- O(1)-Fokuswechsel
- O(1)-Aktualisierung
- deterministische Laufzeit

Nur geänderte Checkboxen werden neu gerendert.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Zustand
- beschädigtes Template
- fehlender Style
- Nullpointer

Fehlerhafte Checkboxen werden deaktiviert.

---

# 21. Sicherheit

Das System verhindert:

- ungültige Zustandskombinationen
- rekursive Ereignisse
- doppelte Statusänderungen
- inkonsistente Tristate-Zustände

Eine Zustandsänderung darf genau ein Ereignis auslösen.

---

# 22. API

```c
typedef struct nova_checkbox nova_checkbox_t;

typedef enum
{
    NOVA_CHECKBOX_UNCHECKED,
    NOVA_CHECKBOX_CHECKED,
    NOVA_CHECKBOX_INDETERMINATE
}
nova_checkbox_state_t;

nova_result_t
nova_checkbox_create(
    nova_checkbox_t** checkbox);

nova_result_t
nova_checkbox_set_state(
    nova_checkbox_t* checkbox,
    nova_checkbox_state_t state);

nova_checkbox_state_t
nova_checkbox_get_state(
    const nova_checkbox_t* checkbox);

bool
nova_checkbox_checked(
    const nova_checkbox_t* checkbox);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Checkbox-ID
- Zustandswechsel
- Fokuswechsel
- Aktivierungen
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Checked
- Unchecked
- Indeterminate
- Tastatur
- Maus
- Touch
- Controller
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 25. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- Checkboxen unabhängig voneinander arbeiten.
- Zustandswechsel deterministisch erfolgen.
- Tristate optional unterstützt wird.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig integriert ist.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTCONTROL-CHECKBOX-0001 – Checkbox
- ADR-BOOTTHEME-0001 – Theme-System
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTINPUT-0001 – Eingabearchitektur
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 27. Zusammenfassung

Die **Checkbox** definiert das standardisierte Steuerelement zur Auswahl unabhängiger Optionen innerhalb der NovaOS-Bootoberfläche. Sie unterstützt Zwei- und optional Drei-Zustands-Logik, vollständige Theme- und Template-Integration, Animationen sowie Accessibility. Durch deterministische Zustandswechsel, klare Trennung zwischen Logik und Darstellung sowie statische Speicherverwaltung entsteht ein konsistentes und performantes UI-Control für BIOS- und UEFI-basierte Bootumgebungen.