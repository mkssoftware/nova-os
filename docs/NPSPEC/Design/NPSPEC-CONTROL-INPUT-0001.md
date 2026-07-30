# NPSPEC-CONTROL-INPUT-0001: Nova Input Control

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** UI Controls
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova UI Framework, Nova SDK, Nova Desktop Shell und alle Nova-Anwendungen
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert den **Nova Input Control (NIC)**.

Der Nova Input Control ist die abstrakte Basiskomponente sämtlicher Eingabesteuerelemente innerhalb von Nova OS.

Alle Controls, welche Benutzereingaben entgegennehmen, leiten sich von dieser Spezifikation ab.

Hierzu gehören unter anderem:

- TextField
- TextArea
- ComboBox
- NumberField
- PasswordField
- DatePicker
- ColorPicker
- Slider
- Switch
- Checkbox
- RadioButton

Der Nova Input Control stellt das gemeinsame Verhalten, die Zustände, Ereignisse und Schnittstellen aller Eingabekomponenten bereit.

---

# 2. Zielsetzung

Der Nova Input Control verfolgt folgende Ziele:

- konsistente Eingabeverarbeitung
- einheitliche API
- hohe Performance
- vollständige Accessibility
- Themefähigkeit
- Erweiterbarkeit
- GPU-beschleunigte Darstellung
- Geräteunabhängigkeit

---

# 3. Designphilosophie

Alle Eingabekomponenten sollen sich identisch verhalten.

Unabhängig vom konkreten Control gelten dieselben Regeln für:

- Fokus
- Hover
- Validierung
- Tastaturbedienung
- Animationen
- Zustände

Ein konsistentes Verhalten reduziert die Lernkurve und verbessert die Benutzerfreundlichkeit. Formularelemente sollten gemeinsame Interaktionsmuster besitzen. :contentReference[oaicite:0]{index=0}

---

# 4. Architektur

```text
Nova Input Control

├── Label
├── Input Surface
├── Validation
├── Placeholder
├── Helper Text
├── Prefix
├── Suffix
├── Focus Ring
├── State Manager
├── Event Handler
└── Accessibility
```

---

# 5. Vererbungsstruktur

```text
InputControl

├── TextField
├── TextArea
├── NumberField
├── PasswordField
├── SearchField
├── DatePicker
├── TimePicker
├── ComboBox
├── ColorPicker
├── Slider
├── Checkbox
├── Switch
└── RadioButton
```

Alle Eingabekomponenten müssen von `InputControl` ableiten.

---

# 6. Gemeinsame Eigenschaften

Jeder Input Control besitzt:

- ID
- Name
- Label
- Wert
- Standardwert
- Placeholder
- Helper Text
- Tooltip
- Enabled
- Visible
- ReadOnly
- Required

Optional:

- Prefix
- Suffix
- Icon
- Badge

---

# 7. Zustände

Alle Input Controls besitzen dieselben Zustände.

```text
Normal

↓

Hover

↓

Focused

↓

Filled

↓

Disabled

↓

Readonly

↓

Warning

↓

Error

↓

Success
```

---

# 8. Fokus

Beim Fokus:

- Nova Accent Border
- Focus Ring
- Themeabhängige Hervorhebung
- Eingabecursor (falls zutreffend)

Es darf immer nur ein Input Control gleichzeitig den Eingabefokus besitzen.

---

# 9. Validierung

Der Nova Input Control unterstützt:

- Live-Validierung
- Validierung beim Fokusverlust
- Validierung beim Absenden

Validierungsfehler besitzen:

- Symbol
- Farbe
- Meldung
- Accessibility-Hinweis

Inline-Validierung soll dem Benutzer helfen, Fehler frühzeitig zu erkennen, ohne unnötig zu stören. :contentReference[oaicite:1]{index=1}

---

# 10. Pflichtfelder

Pflichtfelder werden dargestellt durch:

```text
Benutzername *
```

Alternativ:

```text
Required
```

Screenreader müssen Pflichtfelder eindeutig erkennen.

---

# 11. Label

Jedes Input Control besitzt standardmäßig:

```text
Label

↓

Input

↓

Helper Text
```

Ein sichtbares Label ist Bestandteil jeder Eingabekomponente. Placeholder dürfen niemals die Beschriftung ersetzen. :contentReference[oaicite:2]{index=2}

---

# 12. Placeholder

Placeholder dienen ausschließlich als kurze Hilfe.

Beispiele:

```text
Suche...

MM.JJJJ

name@example.com
```

Der Placeholder verschwindet während der Eingabe.

---

# 13. Helper Text

Hilfetexte beschreiben:

- Format
- Einschränkungen
- Beispiele
- Empfehlungen

Beispiel:

```text
Mindestens 8 Zeichen
```

---

# 14. Prefix und Suffix

Unterstützt:

Prefix

```text
€

https://

@
```

Suffix

```text
kg

%

MB

ms
```

---

# 15. Eingabemethoden

Unterstützt:

- Tastatur
- Maus
- Touch
- Stylus
- Spracheingabe
- IME
- Zwischenablage

Alle Eingabegeräte werden durch das Nova Input Framework vereinheitlicht.

---

# 16. Ereignisse

Jeder Input Control erzeugt:

```text
Focus

Blur

Input

Change

Validation

Selection

Copy

Paste

Cut

Undo

Redo
```

Weitere Ereignisse können ergänzt werden.

---

# 17. Tastatur

Standardbelegung:

```text
TAB

SHIFT+TAB

ENTER

ESC

CTRL+A

CTRL+C

CTRL+V

CTRL+X

CTRL+Z

CTRL+Y
```

Alle Eingabekomponenten müssen vollständig per Tastatur bedienbar sein.

---

# 18. Zwischenablage

Unterstützt:

- Kopieren
- Ausschneiden
- Einfügen
- Drag & Drop

Anwendungen können einzelne Funktionen einschränken.

---

# 19. Animationen

Der Nova Input Control verwendet:

- Fade
- Border Transition
- Glow
- Focus Animation
- Ripple (optional)

Alle Animationen folgen dem Nova Motion System.

---

# 20. Theme-Unterstützung

Unterstützt:

- Light
- Dark
- High Contrast
- Benutzerdefinierte Themes

Alle Farben stammen aus den Nova Design Tokens.

---

# 21. Fehlerzustand

Der Fehlerzustand zeigt:

- roten Rahmen
- Fehlersymbol
- Validierungstext
- Screenreader-Hinweis

Fehlermeldungen sollen erklären, wie das Problem behoben werden kann, anstatt nur den Fehler zu benennen. :contentReference[oaicite:3]{index=3}

---

# 22. Erfolg

Der Erfolgszustand zeigt:

- grünen Rahmen
- Häkchen
- optionale Erfolgsmeldung

---

# 23. Accessibility

Unterstützt:

- Screenreader
- Tastatursteuerung
- Sprachsteuerung
- hohe Kontraste
- große Schrift
- reduzierte Animationen

Alle Input Controls müssen einen Accessibility-Namen, eine Rolle und ihren aktuellen Zustand bereitstellen. :contentReference[oaicite:4]{index=4}

---

# 24. Performance

Der Nova Input Control soll:

- GPU-beschleunigt rendern
- Eingaben ohne wahrnehmbare Verzögerung verarbeiten
- Layout-Neuberechnungen minimieren
- flüssig bis 240 Hz arbeiten

---

# 25. Entwickler-API

Das Nova SDK stellt bereit:

```text
InputControl

↓

Value

↓

Validation

↓

State

↓

Events

↓

Accessibility
```

Jede Eingabekomponente verwendet diese gemeinsame API.

---

# 26. Erweiterbarkeit

Neue Eingabekomponenten müssen:

- von InputControl ableiten
- dieselben Zustände verwenden
- Accessibility unterstützen
- Nova Design Tokens verwenden
- Nova Motion Tokens verwenden

---

# 27. Positive Konsequenzen

- konsistente Eingabekomponenten
- einfache Entwicklung
- geringe Lernkurve
- hohe Wiederverwendbarkeit
- einheitliche APIs

---

# 28. Negative Konsequenzen

- höherer Implementierungsaufwand
- strenge Standardisierung
- umfangreiche Tests erforderlich

---

# 29. Auswirkungen auf andere Module

Diese Spezifikation betrifft:

- NPSPEC-INPUT-0001 – Nova Input Framework
- NPSPEC-ACCESSIBILITY-0001 – Nova Accessibility Framework
- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- sämtliche `NPSPEC-CONTROL-*`
- Theme Manager
- Input Manager
- Accessibility Manager

Alle Eingabekomponenten innerhalb von Nova OS MÜSSEN auf dieser Basisspezifikation aufbauen.

---

# 30. Umsetzungsregeln

- Alle Eingabekomponenten MÜSSEN von `InputControl` ableiten.
- Jedes Input Control MUSS ein sichtbares Label unterstützen.
- Placeholder DÜRFEN Labels niemals ersetzen.
- Alle Zustände MÜSSEN konsistent implementiert werden.
- Alle Animationen MÜSSEN dem Nova Motion System folgen.
- Alle Farben MÜSSEN aus den Nova Design Tokens stammen.
- Accessibility MUSS vollständig unterstützt werden.
- Die API MUSS für alle Eingabekomponenten identisch aufgebaut sein.

---

# 31. Referenzen

- NPSPEC-INPUT-0001 – Nova Input Framework
- NPSPEC-ACCESSIBILITY-0001 – Nova Accessibility Framework
- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-CONTROL-TEXTFIELD-0001 – Nova Text Field Control
- Fluent 2 – Input Usage Guidance :contentReference[oaicite:5]{index=5}
- Fluent 2 – Field Usage Guidance :contentReference[oaicite:6]{index=6}

---

# 32. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Input Control als verbindliche Basisklasse für sämtliche Eingabekomponenten in Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.