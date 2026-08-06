# NPSPEC-BOOTCONTROL-SWITCH-0001
# Switch

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-SWITCH-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Switch-Control** der NovaOS Boot UI.

Ein Switch dient zum sofortigen Aktivieren oder Deaktivieren einer Funktion. Im Gegensatz zur Checkbox signalisiert ein Switch unmittelbar den aktuellen Betriebszustand einer Option und wird insbesondere für Systemeinstellungen verwendet, die direkt wirksam werden.

Typische Einsatzbereiche:

- Secure Boot
- Fast Boot
- Debug-Modus
- Netzwerkfunktionen
- Diagnoseoptionen
- Entwickleroptionen
- Telemetrie
- Hardwarefunktionen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Switch-Controls.

Sie umfasst:

- Standardswitch
- Destructive Switch
- Lockable Switch
- Busy Switch

Nicht Bestandteil sind:

- Checkboxen
- Radio Buttons
- Toggle Buttons

---

# 3. Architektur

```text
Boot Control Framework

↓

Switch

↓

Switch Template

↓

Renderer
```

---

# 4. Grundprinzip

Ein Switch repräsentiert genau einen booleschen Betriebszustand.

Der aktuelle Zustand muss jederzeit eindeutig sichtbar sein.

Optional kann die Änderung unmittelbar nach der Benutzeraktion wirksam werden.

---

# 5. Aufgaben

Der Switch übernimmt:

- Zustandsumschaltung
- Darstellung des Betriebszustands
- Fokusverwaltung
- Animationen
- Accessibility
- Themeintegration
- Ereignisauslösung
- Sperrverwaltung

---

# 6. Aufbau

```text
Switch

├── Track

├── Thumb

├── Label

├── Description (optional)

└── Focus Indicator
```

---

# 7. Eigenschaften

Jeder Switch besitzt:

- ID
- Label
- Beschreibung (optional)
- Zustand
- Enabled
- Locked
- Busy
- Style
- Template

---

# 8. Betriebszustände

```text
OFF

↓

ON
```

Optional:

```text
OFF

↓

BUSY

↓

ON
```

Ein Tristate-Modus wird nicht unterstützt.

---

# 9. Control States

Zusätzlich:

```text
Normal

Hovered

Focused

Pressed

Disabled

Locked
```

---

# 10. Größen

Empfohlene Standardgrößen:

| Element | Größe |
|---------|-------|
| Switch | 52 × 28 DLU |
| Thumb | 22 × 22 DLU |
| Gesamtes Control | mindestens 140 × 32 DLU |

Alle Maße sind DPI-unabhängig.

---

# 11. Layout

```text
Label

↓

Description (optional)

↓

Switch
```

Alternativ:

```text
Label

Spacing

Switch
```

Die Position wird durch das Layout-System bestimmt.

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
- Touch Tap

---

# 13. Zustandswechsel

```text
OFF

↓

ON

↓

OFF
```

Ist der Switch gesperrt oder deaktiviert, erfolgt kein Zustandswechsel.

---

# 14. Darstellung

Der Switch verwendet:

- Glass Material
- animierten Thumb
- farbigen Track
- Focus Glow
- weiche Schatten

Der Track visualisiert den aktuellen Zustand.

---

# 15. Animationen

Unterstützt werden:

- Thumb Slide
- Track Color Transition
- Hover Fade
- Focus Glow
- Press Animation

Animationen laufen synchron mit dem Motion-System.

---

# 16. Busy-Modus

Während einer laufenden Systemoperation kann der Switch den Zustand **Busy** annehmen.

Eigenschaften:

- keine weiteren Eingaben
- optionale Busy-Animation
- Abschluss mit Erfolg oder Fehler

---

# 17. Accessibility

Jeder Switch besitzt:

- Name
- Rolle
- Beschreibung
- aktueller Zustand
- Shortcut (optional)

Screenreader geben den Zustand "Ein" oder "Aus" eindeutig aus.

---

# 18. Themeintegration

Das Theme definiert:

- Track-Farben
- Thumb-Farben
- Fokusfarbe
- Schatten
- Transparenz
- Animationen

Es existieren keine fest codierten Farben.

---

# 19. Speicherverwaltung

Switches verwenden:

- Objektpools
- gemeinsame Templates
- gemeinsame Styles

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Zustandswechsel
- O(1)-Animation
- O(1)-Fokuswechsel
- deterministische Laufzeit

Nur geänderte Switches werden neu gerendert.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Zustand
- beschädigtes Template
- fehlender Style
- Nullpointer
- ungültiger Busy-Zustand

Fehlerhafte Switches werden deaktiviert.

---

# 22. Sicherheit

Das System verhindert:

- rekursive Zustandswechsel
- Mehrfachaktivierungen
- Zustandsänderungen im Locked-Zustand
- ungültige Busy-Übergänge
- inkonsistente Zustände

Destruktive Switches müssen vor dem Umschalten eine Bestätigung anfordern.

---

# 23. API

```c
typedef struct nova_switch nova_switch_t;

typedef enum
{
    NOVA_SWITCH_OFF,
    NOVA_SWITCH_ON,
    NOVA_SWITCH_BUSY
}
nova_switch_state_t;

nova_result_t
nova_switch_create(
    nova_switch_t** sw);

nova_result_t
nova_switch_set_state(
    nova_switch_t* sw,
    nova_switch_state_t state);

nova_switch_state_t
nova_switch_get_state(
    const nova_switch_t* sw);

bool
nova_switch_is_on(
    const nova_switch_t* sw);

nova_result_t
nova_switch_set_locked(
    nova_switch_t* sw,
    bool locked);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Switch-ID
- Zustandswechsel
- Busy-Dauer
- Fokuswechsel
- Aktivierungen
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 25. Testfälle

Zu testen sind:

- OFF → ON
- ON → OFF
- Busy-Modus
- Locked-Zustand
- Tastatur
- Maus
- Touch
- Controller
- Themewechsel
- Accessibility
- Fehlerfälle

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- der aktuelle Betriebszustand jederzeit eindeutig dargestellt wird.
- Zustandswechsel deterministisch erfolgen.
- Busy- und Locked-Zustände korrekt verarbeitet werden.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTCONTROL-SWITCH-0001 – Switch-Control
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

# 28. Zusammenfassung

Das **Switch-Control** definiert das standardisierte Bedienelement zum unmittelbaren Ein- und Ausschalten von Funktionen innerhalb der NovaOS-Bootoberfläche. Es stellt den aktuellen Betriebszustand jederzeit eindeutig dar und unterstützt Busy- sowie Locked-Zustände, vollständige Theme-Integration, Animationen und Accessibility. Durch deterministische Zustandswechsel, klare Trennung zwischen Darstellung und Logik sowie statische Speicherverwaltung entsteht ein konsistentes und performantes UI-Control für BIOS- und UEFI-basierte Bootumgebungen.