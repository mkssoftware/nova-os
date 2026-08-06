# NPSPEC-BOOTCONTROL-SLIDER-0001
# Slider

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-SLIDER-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Controls |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTCONTROL-0003, NPSPEC-BOOTCONTROL-0004, NPSPEC-BOOTINPUT-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Slider-Control** der NovaOS Boot UI.

Ein Slider dient zur Auswahl eines numerischen Wertes innerhalb eines definierten Wertebereichs. Er ermöglicht schnelle und präzise Änderungen kontinuierlicher oder diskreter Werte.

Typische Einsatzbereiche:

- Lautstärke
- Helligkeit
- Zeitlimits
- Speichergrenzen
- CPU-Limits
- Performanceprofile
- Skalierungsfaktoren
- Timeout-Einstellungen

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche Slider.

Sie umfasst:

- horizontalen Slider
- vertikalen Slider
- kontinuierlichen Slider
- diskreten Slider
- Bereichsslider (optional)

Nicht Bestandteil sind:

- Scrollbars
- ProgressBars
- Drehregler

---

# 3. Architektur

```text
Boot Control Framework

↓

Slider

↓

Slider Template

↓

Renderer
```

---

# 4. Grundprinzip

Ein Slider repräsentiert genau einen numerischen Wert.

Der aktuelle Wert liegt immer innerhalb eines definierten Minimums und Maximums.

Optional kann der Wert auf feste Schrittweiten gerundet werden.

---

# 5. Aufgaben

Der Slider übernimmt:

- Werteverwaltung
- Benutzereingaben
- Fokusverwaltung
- Animationen
- Accessibility
- Themeintegration
- Ereignisauslösung

---

# 6. Aufbau

```text
Slider

├── Track

├── Fill Area

├── Thumb

├── Tick Marks (optional)

├── Value Label (optional)

└── Focus Indicator
```

---

# 7. Eigenschaften

Jeder Slider besitzt:

- ID
- Minimum
- Maximum
- Aktueller Wert
- Schrittweite
- Orientierung
- Style
- Template
- Enabled

Optional:

- Tick Marks
- Wertebeschriftung

---

# 8. Wertebereich

Der Wertebereich wird definiert durch:

```text
Minimum

↓

Maximum
```

Es gilt:

```
Minimum ≤ Wert ≤ Maximum
```

---

# 9. Schrittweite

Unterstützt werden:

- kontinuierliche Werte
- feste Schrittweiten

Beispiel:

```text
0

10

20

30

40

50
```

Der aktuelle Wert wird auf die nächstgelegene Stufe gerundet.

---

# 10. Orientierung

Unterstützt werden:

```text
Horizontal
```

und

```text
Vertikal
```

Die Orientierung beeinflusst ausschließlich die Darstellung.

---

# 11. Zustände

```text
Normal

Hovered

Focused

Pressed

Disabled

Busy
```

---

# 12. Eingaben

Unterstützt:

- Maus
- Tastatur
- Touch
- Controller

Tastatur:

- ←
- →
- ↑
- ↓
- Home
- End
- Page Up
- Page Down

---

# 13. Werteänderung

Eine Änderung erfolgt:

```text
Input

↓

Thumb Position

↓

Value

↓

ValueChanged Event
```

Der Wert wird nur geändert, wenn er gültig ist.

---

# 14. Darstellung

Der Slider verwendet:

- Glass Material
- Track
- Fill Area
- animierten Thumb
- Focus Glow

Die Darstellung wird vollständig über Theme und Template definiert.

---

# 15. Animationen

Unterstützt werden:

- Thumb Slide
- Fill Transition
- Hover Fade
- Focus Glow
- Value Transition

Animationen beeinflussen ausschließlich die Darstellung.

---

# 16. Accessibility

Jeder Slider besitzt:

- Name
- Rolle
- Beschreibung
- aktueller Wert
- Minimum
- Maximum

Screenreader geben den aktuellen Wert numerisch aus.

---

# 17. Themeintegration

Das Theme definiert:

- Track
- Fill Area
- Thumb
- Fokusfarbe
- Transparenz
- Schatten
- Animationen

Es existieren keine fest codierten Farben.

---

# 18. Speicherverwaltung

Slider verwenden:

- Objektpools
- gemeinsame Templates
- gemeinsame Styles

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(1)-Werteänderung
- O(1)-Thumbbewegung
- O(1)-Fokuswechsel
- deterministische Laufzeit

Nur geänderte Bereiche werden neu gerendert.

---

# 20. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- ungültiger Wertebereich
- ungültige Schrittweite
- Wert außerhalb des Bereichs
- beschädigtes Template
- Nullpointer

Ungültige Werte werden automatisch in den zulässigen Bereich begrenzt.

---

# 21. Sicherheit

Das System verhindert:

- Werte außerhalb des Bereichs
- ungültige Schrittweiten
- rekursive ValueChanged-Ereignisse
- beschädigte Zustände
- inkonsistente Thumb-Positionen

---

# 22. API

```c
typedef struct nova_slider nova_slider_t;

nova_result_t
nova_slider_create(
    nova_slider_t** slider);

nova_result_t
nova_slider_set_range(
    nova_slider_t* slider,
    double minimum,
    double maximum);

nova_result_t
nova_slider_set_value(
    nova_slider_t* slider,
    double value);

double
nova_slider_get_value(
    const nova_slider_t* slider);

nova_result_t
nova_slider_set_step(
    nova_slider_t* slider,
    double step);
```

---

# 23. Diagnosedaten

Zu erfassen sind:

- Slider-ID
- aktueller Wert
- Werteänderungen
- Fokuswechsel
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 24. Testfälle

Zu testen sind:

- Minimum
- Maximum
- Schrittweite
- kontinuierliche Werte
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

- der aktuelle Wert jederzeit innerhalb des gültigen Bereichs liegt.
- Schrittweiten korrekt berücksichtigt werden.
- Theme und Templates vollständig verwendet werden.
- Accessibility vollständig unterstützt wird.
- Werteänderungen deterministisch erfolgen.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 26. Referenzen

## ADRs

- ADR-BOOTCONTROL-SLIDER-0001 – Slider-Control
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

Das **Slider-Control** definiert das standardisierte Bedienelement zur Auswahl numerischer Werte innerhalb der NovaOS-Bootoberfläche. Es unterstützt kontinuierliche und diskrete Wertebereiche, horizontale und vertikale Ausrichtungen, vollständige Theme-Integration, Animationen sowie Accessibility. Durch deterministische Werteänderungen, konsistente Darstellung und statische Speicherverwaltung bildet der Slider ein leistungsfähiges und flexibles UI-Control für BIOS- und UEFI-basierte Bootumgebungen.