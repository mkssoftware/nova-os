# NPSPEC-CONTROL-BUTTON-0001: Nova Button Control

- **Status:** Angenommen    
- **Version:** 1.0
- **Kategorie:** UI Controls
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova UI Framework, Nova SDK, Nova Desktop Shell und alle Nova-Anwendungen
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert den **Nova Button Control (NBC)**.

Buttons sind die wichtigste interaktive Komponente innerhalb von Nova OS.

Sie lösen unmittelbar Aktionen aus und bilden die Grundlage nahezu aller Benutzerinteraktionen.

Diese Spezifikation beschreibt:

- Erscheinungsbild
- Verhalten
- Zustände
- Animationen
- Accessibility
- API
- Designregeln

---

# 2. Zielsetzung

Der Nova Button verfolgt folgende Ziele:

- eindeutige Benutzeraktionen
- konsistentes Erscheinungsbild
- schnelle Reaktion
- vollständige Tastaturbedienung
- Touch-Unterstützung
- GPU-beschleunigte Darstellung
- Barrierefreiheit
- Erweiterbarkeit

---

# 3. Designphilosophie

Ein Button soll eindeutig vermitteln:

- Dies ist anklickbar.
- Diese Aktion wird sofort ausgeführt.
- Der aktuelle Zustand ist jederzeit sichtbar.

Buttons dürfen niemals dekorativ eingesetzt werden.

Buttons dienen zum Auslösen einer einzelnen Aktion. Für Navigation oder das Öffnen einer anderen Ansicht sollten stattdessen Links oder Navigationselemente verwendet werden. :contentReference[oaicite:0]{index=0}

---

# 4. Architektur

```text
Nova Button

├── Background
├── Border
├── Icon
├── Label
├── Badge
├── Progress
├── Focus Ring
├── Ripple
├── Animation
└── Event Handler
```

---

# 5. Komponentenaufbau

```text
┌────────────────────────────┐
│ Icon   Text        Badge   │
└────────────────────────────┘
```

Optional:

- Icon
- Badge
- Progress
- Dropdown-Pfeil

---

# 6. Buttontypen

Nova definiert folgende Standardtypen:

- Primary Button
- Secondary Button
- Outline Button
- Text Button
- Icon Button
- Toggle Button
- Split Button
- Menu Button
- Compound Button
- Destructive Button

Neue Buttontypen benötigen eine eigene NPSPEC.

---

# 7. Primary Button

Eigenschaften:

- Nova Cyan Hintergrund
- weiße Schrift
- hervorgehobener Schatten

Verwendung:

- wichtigste Aktion eines Dialogs
- Speichern
- Installieren
- Weiter
- Anmelden

Es soll pro Ansicht möglichst nur **einen** Primary Button geben. :contentReference[oaicite:1]{index=1}

---

# 8. Secondary Button

Eigenschaften:

- neutraler Hintergrund
- dünner Rahmen
- dunkle Schrift

Verwendung:

- normale Aktionen
- Werkzeuge
- Navigation innerhalb einer Ansicht

---

# 9. Outline Button

Eigenschaften:

- transparenter Hintergrund
- sichtbarer Rahmen

Verwendung:

- sekundäre Aktionen
- Werkzeuge
- Dialoge

---

# 10. Text Button

Eigenschaften:

- kein Rahmen
- kein Hintergrund
- nur Text

Verwendung:

- Listen
- Tabellen
- Inline-Aktionen

---

# 11. Icon Button

Besteht ausschließlich aus:

- Fluent SVG Icon

Optional:

- Tooltip
- Badge

Verwendung:

- Symbolleisten
- Fensteraktionen
- Mediensteuerung

---

# 12. Toggle Button

Zustände:

```text
Aus

↓

Ein
```

Eigenschaften:

- dauerhafter Zustand
- visuelle Kennzeichnung

Toggle Buttons werden für Werkzeugleisten verwendet. Für dauerhafte Systemeinstellungen ist ein Switch vorzuziehen. :contentReference[oaicite:2]{index=2}

---

# 13. Split Button

Aufbau:

```text
┌────────────┬───┐
│ Aktion     │ ▼ │
└────────────┴───┘
```

Unterstützt:

- Primäraktion
- Zusatzmenü

---

# 14. Compound Button

Besteht aus:

```text
Titel

Beschreibung
```

Verwendung:

- Installationsdialoge
- Assistenten
- Startseiten

---

# 15. Größen

Standardgrößen:

| Größe | Höhe |
|--------|------|
| Small | 28 px |
| Medium | 36 px |
| Large | 44 px |
| Touch | 52 px |

---

# 16. Innenabstände

Standard:

```text
Links/Rechts

16 px

Oben/Unten

8 px
```

Das Layout basiert auf dem Nova-8-Pixel-Raster.

---

# 17. Radien

Standard:

```text
12 px
```

Touchmodus:

```text
16 px
```

---

# 18. Zustände

Alle Buttons besitzen dieselben Zustände.

```text
Normal

↓

Hover

↓

Pressed

↓

Focused

↓

Disabled

↓

Loading

↓

Success

↓

Error
```

---

# 19. Hover

Beim Hover:

- leichte Aufhellung
- weicher Schatten
- Glow
- Mauszeiger = Hand

---

# 20. Pressed

Beim Drücken:

```text
100 %

↓

97 %

↓

100 %
```

Zusätzlich:

- kurzer Schattenwechsel
- Ripple
- Klicksound (optional)

---

# 21. Fokus

Der Tastaturfokus besitzt:

- Nova Cyan Focus Ring
- Mindestbreite 2 px
- vollständig sichtbar

---

# 22. Disabled

Eigenschaften:

- reduzierte Deckkraft
- keine Animation
- keine Eingabe
- keine Schatten

Der Zustand muss eindeutig erkennbar sein.

---

# 23. Loading

Während einer Aktion:

```text
Spinner

↓

Button deaktiviert

↓

Text bleibt sichtbar
```

Der Benutzer erkennt, dass die Aktion verarbeitet wird.

---

# 24. Farben

Primary

```text
Nova Cyan
```

Secondary

```text
Neutral
```

Destructive

```text
Nova Red
```

Success

```text
Nova Green
```

Warning

```text
Nova Orange
```

Alle Farben stammen ausschließlich aus den Design Tokens.

---

# 25. Icons

Es werden ausschließlich verwendet:

- Fluent System Icons
- SVG
- Themefähig

Icons besitzen standardmäßig:

```text
20 × 20 px
```

---

# 26. Animationen

Nova Buttons verwenden:

- Fade
- Scale
- Spring
- Ripple
- Glow

Alle Animationen folgen dem Nova Motion System.

---

# 27. Tastatur

Standard:

```text
TAB

↓

Fokus

ENTER

↓

Ausführen

SPACE

↓

Ausführen
```

---

# 28. Touch

Touchmodus:

- größere Trefferfläche
- mindestens 44 × 44 px
- längere Ripple-Animation

---

# 29. Accessibility

Unterstützt:

- Screenreader
- Tastatursteuerung
- Sprachsteuerung
- hohe Kontraste
- große Schriftarten
- reduzierte Animationen

Schaltflächen müssen ausreichende Kontraste und aussagekräftige Beschriftungen besitzen sowie in allen Zuständen barrierefrei bedienbar sein. :contentReference[oaicite:3]{index=3}

---

# 30. Performance

Buttons sollen:

- GPU-beschleunigt rendern
- unter 10 ms reagieren
- keine Layoutsprünge erzeugen
- minimale Speicherbelegung besitzen

---

# 31. Entwickler-API

Das Nova SDK stellt bereit:

```text
Button

↓

PrimaryButton

↓

SecondaryButton

↓

IconButton

↓

ToggleButton

↓

SplitButton

↓

MenuButton

↓

CompoundButton
```

---

# 32. Erweiterbarkeit

Neue Buttontypen müssen:

- Design Tokens verwenden
- Motion Tokens verwenden
- Accessibility unterstützen
- dieselben Zustände besitzen

---

# 33. Positive Konsequenzen

- konsistente Benutzerführung
- hohe Wiedererkennbarkeit
- geringe Lernkurve
- moderne Optik
- einfache Erweiterbarkeit

---

# 34. Negative Konsequenzen

- größerer Implementierungsaufwand
- umfangreiche UI-Tests
- konsequente Einhaltung der Designregeln erforderlich

---

# 35. Auswirkungen auf andere Module

Diese Spezifikation betrifft:

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-DIALOG-0001 – Nova Dialog System
- NPSPEC-WINDOW-0001 – Nova Window System
- Theme Manager
- Accessibility Manager
- Nova SDK

Alle Schaltflächen innerhalb von Nova OS MÜSSEN dieser Spezifikation entsprechen.

---

# 36. Umsetzungsregeln

- Jeder Button MUSS genau eine Primärfunktion besitzen.
- Pro Ansicht SOLL höchstens ein Primary Button verwendet werden.
- Alle Buttons MÜSSEN vollständig per Tastatur bedienbar sein.
- Alle Buttons MÜSSEN GPU-beschleunigt gerendert werden.
- Alle Buttons MÜSSEN Design Tokens verwenden.
- Alle Animationen MÜSSEN dem Nova Motion System folgen.
- Fluent SVG Icons MÜSSEN für Symbolschaltflächen verwendet werden.
- Accessibility MUSS vollständig unterstützt werden.

---

# 37. Referenzen

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-DIALOG-0001 – Nova Dialog System
- NPSPEC-WINDOW-0001 – Nova Window System
- Fluent 2 – Button Usage Guide :contentReference[oaicite:4]{index=4}
- Microsoft Windows Controls and Patterns :contentReference[oaicite:5]{index=5}

---

# 38. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Button Control als verbindlicher Standard für alle Schaltflächen in Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.