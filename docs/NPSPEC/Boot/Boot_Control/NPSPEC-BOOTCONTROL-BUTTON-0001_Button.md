# NPSPEC-BOOTCONTROL-BUTTON-0001
# Button

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-BUTTON-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTINPUT-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Button-Control** der NovaOS Boot UI.

Der Button dient als universelles Steuerelement zum Auslösen von Aktionen innerhalb des Boot Managers.

Alle Schaltflächen der Bootoberfläche basieren auf dieser Spezifikation.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Buttons.

Sie umfasst:

- Standard Button
- Primary Button
- Secondary Button
- Destructive Button
- Icon Button
- Text Button
- Toggle Button
- Split Button (optional)

Nicht Bestandteil sind:

- Menüeinträge
- Links
- Listen

---

# 3. Architektur

```text
Boot Control Framework

↓

Button

↓

Template

↓

Renderer
```

---

# 4. Grundprinzip

Ein Button stellt genau **eine Benutzeraktion** dar.

Ein Button besitzt:

- eindeutige Aktion
- eindeutigen Zustand
- optional ein Symbol
- optional einen Text

Ein Button darf niemals mehrere Aktionen gleichzeitig auslösen.

---

# 5. Aufgaben

Der Button übernimmt:

- Benutzerinteraktion
- Fokusdarstellung
- Klickverarbeitung
- Zustandswechsel
- Themeintegration
- Accessibility
- Animation
- Ereignisauslösung

---

# 6. Aufbau

Ein Button besteht aus:

```text
Button

├── Background

├── Border

├── Icon (optional)

├── Text

├── Focus Indicator

└── Ripple Layer (optional)
```

---

# 7. Eigenschaften

Jeder Button besitzt:

- ID
- Text
- Icon
- ToolTip
- Aktion
- Zustand
- Style
- Template
- Größe
- Sichtbarkeit

---

# 8. Button-Typen

Unterstützt werden:

### Standard

Normale Aktion.

### Primary

Hauptaktion eines Dialogs.

### Secondary

Alternative Aktion.

### Destructive

Irreversible Aktionen.

### Icon

Nur Symbol.

### Text

Nur Text.

### Toggle

Bleibt gedrückt.

---

# 9. Zustände

```text
Normal

↓

Hovered

↓

Focused

↓

Pressed

↓

Released

↓

Disabled
```

Zusätzlich:

```text
Selected

Busy
```

---

# 10. Größen

Empfohlene Mindestgrößen:

| Typ | Größe |
|------|-------|
| Standard | 120 × 40 DLU |
| Icon | 40 × 40 DLU |
| Small | 90 × 32 DLU |
| Large | 180 × 48 DLU |

Alle Größen sind DPI-unabhängig.

---

# 11. Layout

Der Inhalt wird:

```text
Horizontal

Icon

Spacing

Text
```

oder

```text
Text
```

zentriert dargestellt.

---

# 12. Eingaben

Unterstützt:

- Maus
- Tastatur
- Touch
- Controller

Aktivierung erfolgt über:

- Linksklick
- Enter
- Space
- Controller A

---

# 13. Klickverarbeitung

Ablauf:

```text
Focus

↓

Pressed

↓

Released

↓

Click

↓

Action
```

Die Aktion wird erst nach vollständigem Klick ausgelöst.

---

# 14. Toggle Button

Toggle Buttons wechseln zwischen:

```text
Off

↓

On

↓

Off
```

Der Zustand bleibt gespeichert.

---

# 15. Darstellung

Ein Button verwendet:

- Glass Material
- abgerundete Ecken
- weichen Schatten
- Focus Glow
- Themefarben

Alle Darstellungsparameter stammen aus dem Theme.

---

# 16. Animationen

Unterstützt werden:

- Hover Fade
- Focus Glow
- Press Scale
- Color Transition
- Shadow Transition

Animationen folgen dem Motion Framework.

---

# 17. Accessibility

Der Button besitzt:

- Name
- Rolle
- Beschreibung
- Shortcut
- Status

Screenreader geben Text und Zustand aus.

---

# 18. Themeintegration

Das Theme definiert:

- Hintergrund
- Rahmen
- Schrift
- Rundungen
- Transparenz
- Schatten
- Fokusfarbe

Der Button enthält keine fest codierten Farben.

---

# 19. Speicherverwaltung

Buttons verwenden:

- Objektpools
- statische Templates
- gemeinsame Styles

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Klick
- O(1)-Zustandswechsel
- O(1)-Fokuswechsel
- deterministische Laufzeit

Nur geänderte Bereiche werden neu gerendert.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlende Aktion
- fehlendes Template
- ungültiger Zustand
- beschädigtes Theme
- Nullpointer

Im Fehlerfall wird der Button deaktiviert.

---

# 22. Sicherheit

Das System verhindert:

- Mehrfachklick während Busy
- doppelte Ereignisse
- ungültige Zustände
- fehlende Fokusdarstellung
- rekursive Klickereignisse

Destructive Buttons dürfen niemals automatisch ausgelöst werden.

---

# 23. API

```c
typedef enum
{
    NOVA_BUTTON_STANDARD,
    NOVA_BUTTON_PRIMARY,
    NOVA_BUTTON_SECONDARY,
    NOVA_BUTTON_DESTRUCTIVE,
    NOVA_BUTTON_ICON,
    NOVA_BUTTON_TEXT,
    NOVA_BUTTON_TOGGLE
}
nova_button_type_t;

nova_result_t
nova_button_create(
    nova_button_type_t type,
    nova_button_t** button);

nova_result_t
nova_button_set_text(
    nova_button_t* button,
    const char* text);

nova_result_t
nova_button_set_icon(
    nova_button_t* button,
    nova_icon_t icon);

nova_result_t
nova_button_set_action(
    nova_button_t* button,
    nova_action_t action);

bool
nova_button_pressed(
    const nova_button_t* button);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Button-ID
- Button-Typ
- Klickanzahl
- Fokuswechsel
- Zustandswechsel
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 25. Testfälle

Zu testen sind:

- Standard Button
- Primary Button
- Secondary Button
- Destructive Button
- Toggle Button
- Tastatur
- Maus
- Touch
- Controller
- Accessibility
- Themewechsel
- Fehlerfälle

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Buttons auf dem Boot Control Framework basieren.
- alle Button-Typen unterstützt werden.
- Zustandswechsel deterministisch erfolgen.
- Aktionen genau einmal ausgelöst werden.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTCONTROL-BUTTON-0001 – Einheitliches Button-Control
- ADR-BOOTINPUT-0001 – Eingabearchitektur
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTMOTION-0004 – Animierbare Eigenschaften
- ADR-BOOTTHEME-0001 – Theme-System

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTCONTROL-0003 – Control Styling
- NPSPEC-BOOTCONTROL-0004 – Control Templates
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 28. Zusammenfassung

Das **Button-Control** definiert die standardisierte Schaltfläche der NovaOS Bootoberfläche. Es unterstützt verschiedene Button-Typen, konsistente Zustandsübergänge, vollständige Theme-Integration, Animationen sowie Accessibility. Durch eine klare Trennung von Funktion, Darstellung und Template, deterministische Verarbeitung und statische Speicherverwaltung bildet der Button die Grundlage sämtlicher Benutzerinteraktionen innerhalb des NovaOS Boot Managers auf BIOS- und UEFI-Systemen.