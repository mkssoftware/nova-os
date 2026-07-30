# NPSPEC-INPUT-0001: Nova Input Framework

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Framework
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova OS, Nova Kernel, Nova Desktop Shell, Nova SDK und sämtliche Anwendungen
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert das **Nova Input Framework (NIF)**.

Das Nova Input Framework bildet die zentrale Eingabeschicht des Betriebssystems und verarbeitet sämtliche Benutzereingaben unabhängig vom Eingabegerät.

Alle Eingaben werden zunächst vereinheitlicht und anschließend an das jeweils zuständige UI-Element weitergeleitet.

Das Framework arbeitet vollständig ereignisbasiert.

---

# 2. Zielsetzung

Das Nova Input Framework verfolgt folgende Ziele:

- einheitliche Eingabeverarbeitung
- Geräteunabhängigkeit
- geringe Latenz
- hohe Präzision
- vollständige Accessibility
- Erweiterbarkeit
- Sicherheit
- Multimodale Eingabe

---

# 3. Designphilosophie

Jede Eingabe besitzt denselben Lebenszyklus.

Nicht das Gerät ist entscheidend, sondern die vom Benutzer beabsichtigte Aktion.

Beispiele:

- Klick
- Tippen
- Sprache
- Touch
- Tastatur
- Controller

werden intern in dieselben Nova-Eingabeereignisse umgewandelt.

Mehrere Eingabemethoden sollen gleichzeitig unterstützt werden, sodass Benutzer jederzeit zwischen Maus, Tastatur, Touch oder Sprache wechseln können. :contentReference[oaicite:0]{index=0}

---

# 4. Architektur

```text
Hardware

↓

Treiber

↓

Nova Input Manager

↓

Input Pipeline

↓

Gesture Engine

↓

Accessibility Layer

↓

Window Manager

↓

Focused Control

↓

Application
```

---

# 5. Komponenten

Das Framework besteht aus:

```text
Nova Input Framework

├── Keyboard Manager
├── Mouse Manager
├── Touch Manager
├── Pen Manager
├── Gesture Manager
├── Controller Manager
├── Voice Input
├── IME Manager
├── Clipboard Manager
├── Shortcut Manager
├── Accessibility Input
└── Input Dispatcher
```

---

# 6. Unterstützte Eingabegeräte

Nova unterstützt:

- Tastatur
- Maus
- Touchscreen
- Touchpad
- Stylus
- Grafiktablett
- Game Controller
- SpaceMouse
- Sprachsteuerung
- Eye Tracking
- Alternative Eingabegeräte

Neue Geräte werden über Treiber integriert.

---

# 7. Eingabeereignisse

Alle Geräte erzeugen standardisierte Events.

```text
Input Event

↓

Normalize

↓

Dispatch

↓

Handle

↓

Feedback
```

---

# 8. Eventtypen

Nova definiert:

```text
KeyDown

KeyUp

MouseMove

MouseDown

MouseUp

Wheel

TouchDown

TouchMove

TouchUp

Gesture

Pen

Voice

Controller

IME
```

---

# 9. Eventstruktur

Jedes Event besitzt:

```text
Event ID

Zeitstempel

Gerät

Position

Modifier

State

Target

Flags
```

Optional:

- Druck
- Neigung
- Geschwindigkeit
- Rotation

---

# 10. Tastatureingabe

Unterstützt:

- Unicode
- Dead Keys
- Modifier
- Scancodes
- Keycodes
- Auto Repeat
- Mehrere Tastaturen

---

# 11. Maus

Unterstützt:

- beliebig viele Tasten
- Scrollrad
- horizontales Scrollen
- High Resolution Wheel
- Gaming-Mäuse

---

# 12. Touch

Touch unterstützt:

- Single Touch
- Multi Touch
- Palm Rejection
- Gestenerkennung

Mindestanzahl:

```text
10 Touchpunkte
```

---

# 13. Stift

Unterstützt:

- Druckstufen
- Neigung
- Rotation
- Radierer
- Hover

---

# 14. Gesten

Standardgesten:

```text
Tap

Double Tap

Long Press

Swipe

Pinch

Rotate

Drag

Flick
```

Komplexe Gesten müssen stets eine einfachere Alternative besitzen. :contentReference[oaicite:1]{index=1}

---

# 15. Spracheingabe

Unterstützt:

- Diktat
- Navigation
- Befehle
- KI-Steuerung

Beispiele:

```text
Öffne Explorer

Suche Datei

Schließe Fenster

Erstelle Dokument
```

---

# 16. Controller

Unterstützt:

- Xbox
- PlayStation
- Nintendo
- Generic HID

Controller können das gesamte System bedienen.

---

# 17. IME

Das Input Method Framework unterstützt:

- Chinesisch
- Japanisch
- Koreanisch
- Arabisch
- Hindi
- Benutzerdefinierte IMEs

Die IME-Verarbeitung erfolgt vor der Weitergabe an das fokussierte Steuerelement. :contentReference[oaicite:2]{index=2}

---

# 18. Fokus

Nur ein UI-Control besitzt gleichzeitig den Eingabefokus.

```text
Desktop

↓

Fenster

↓

Dialog

↓

Control
```

---

# 19. Event Routing

```text
Input

↓

Capture Phase

↓

Target Phase

↓

Bubble Phase
```

Ein Event kann:

- verarbeitet
- verändert
- abgebrochen

werden.

---

# 20. Eingabepipeline

```text
Hardware

↓

Treiber

↓

Raw Input

↓

Normalization

↓

Gesture Engine

↓

Accessibility

↓

Dispatch

↓

Control
```

Die Trennung zwischen nativer Eingabe, Normalisierung und Weiterleitung vereinfacht die Unterstützung verschiedener Plattformen und Eingabegeräte. :contentReference[oaicite:3]{index=3}

---

# 21. Shortcuts

Standard:

```text
CTRL+C

CTRL+V

CTRL+X

CTRL+Z

CTRL+Y

ALT+TAB

WIN

F1

F2

F5
```

Alle Shortcuts können vom Benutzer angepasst werden.

---

# 22. Clipboard

Unterstützt:

- Text
- Bilder
- Dateien
- HTML
- SVG
- Mehrfachformate

Clipboard History ist Bestandteil des Frameworks.

---

# 23. Drag & Drop

Unterstützt:

- intern
- zwischen Anwendungen
- zwischen Monitoren
- zwischen Prozessen

---

# 24. Eingabelatenz

Zielwerte:

| Ereignis | Ziel |
|----------|------:|
| Tastatur | < 5 ms |
| Maus | < 2 ms |
| Touch | < 8 ms |
| Pen | < 8 ms |

---

# 25. Accessibility

Unterstützt:

- Screenreader
- Sprachsteuerung
- Sticky Keys
- Filter Keys
- Mouse Keys
- Eye Tracking
- Alternative Geräte

Accessibility kann Eingaben bei Bedarf abfangen oder umwandeln, bevor sie an Anwendungen weitergegeben werden. :contentReference[oaicite:4]{index=4}

---

# 26. Sicherheit

Eingaben dürfen:

- nicht gefälscht werden
- nicht von fremden Prozessen mitgelesen werden
- nicht ohne Berechtigung umgeleitet werden

Geschützte Eingabefelder:

- Passwort
- PIN
- Kryptographie

---

# 27. Performance

Das Framework soll:

- unter 1 ms Dispatch-Zeit erreichen
- Multi-Core unterstützen
- Lock-Free Queues verwenden
- GPU-Ereignisse parallel verarbeiten

---

# 28. Entwickler-API

Das Nova SDK stellt bereit:

```text
InputDevice

↓

InputEvent

↓

Keyboard

↓

Mouse

↓

Touch

↓

Pen

↓

Gesture

↓

Voice

↓

Controller
```

---

# 29. Erweiterbarkeit

Neue Eingabegeräte müssen:

- Input Events erzeugen
- Accessibility unterstützen
- Sicherheitsrichtlinien einhalten
- dieselbe Event Pipeline verwenden

---

# 30. Positive Konsequenzen

- einheitliche Eingabeverarbeitung
- geringe Latenz
- hohe Erweiterbarkeit
- moderne Geräteunterstützung
- konsistente APIs

---

# 31. Negative Konsequenzen

- komplexe Event Pipeline
- höherer Entwicklungsaufwand
- umfangreiche Hardwaretests erforderlich

---

# 32. Auswirkungen auf andere Module

Diese Spezifikation betrifft:

- NPSPEC-WINDOW-0001 – Nova Window System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-ACCESSIBILITY-0001 – Nova Accessibility Framework
- NPSPEC-DIALOG-0001 – Nova Dialog System
- sämtliche `NPSPEC-CONTROL-*`
- Window Manager
- Gesture Engine
- Accessibility Manager
- Clipboard Manager

Alle Benutzereingaben innerhalb von Nova OS MÜSSEN über das Nova Input Framework verarbeitet werden.

---

# 33. Umsetzungsregeln

- Sämtliche Eingabegeräte MÜSSEN dieselbe Event Pipeline verwenden.
- Eingaben MÜSSEN geräteunabhängig verarbeitet werden.
- Accessibility MUSS Eingaben bei Bedarf verändern oder übernehmen können.
- Das Event Routing MUSS Capture-, Target- und Bubble-Phase unterstützen.
- Alle Eingaben MÜSSEN Zeitstempel besitzen.
- Die Eingabelatenz SOLL so gering wie möglich sein.
- Neue Eingabegeräte MÜSSEN ohne Änderungen bestehender Anwendungen integrierbar sein.
- Sicherheitskritische Eingaben MÜSSEN besonders geschützt werden.

---

# 34. Referenzen

- NPSPEC-WINDOW-0001 – Nova Window System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-ACCESSIBILITY-0001 – Nova Accessibility Framework
- NPSPEC-DIALOG-0001 – Nova Dialog System
- Chromium – The Life of an Input Event in Desktop Chrome UI :contentReference[oaicite:5]{index=5}
- W3C – WCAG 2.2 Input Modalities :contentReference[oaicite:6]{index=6}
- Microsoft Learn – Interaction Guidelines :contentReference[oaicite:7]{index=7}
- Fuchsia RFC-0096 – User Input Architecture :contentReference[oaicite:8]{index=8}

---

# 35. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Input Framework als verbindliche Eingabearchitektur für Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.