# NPSPEC-ACCESSIBILITY-0001: Nova Accessibility Framework

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** Core Framework
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova OS, Nova SDK, Nova Desktop Shell, Nova Design System und sämtliche Anwendungen
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert das **Nova Accessibility Framework (NAF)**.

Accessibility (Barrierefreiheit) ist keine optionale Erweiterung, sondern ein grundlegender Bestandteil sämtlicher Komponenten innerhalb von Nova OS.

Alle Oberflächen müssen für möglichst viele Menschen unabhängig von körperlichen, sensorischen oder kognitiven Einschränkungen vollständig nutzbar sein.

Accessibility wird bereits während der Entwicklung berücksichtigt und nicht nachträglich ergänzt. Moderne Designsysteme empfehlen Accessibility als Grundprinzip aller Komponenten und orientieren sich mindestens an WCAG-AA. :contentReference[oaicite:0]{index=0}

---

# 2. Zielsetzung

Das Nova Accessibility Framework verfolgt folgende Ziele:

- vollständige Bedienbarkeit
- gleiche Benutzererfahrung für alle
- konsistente Accessibility APIs
- vollständige Tastaturbedienung
- Unterstützung sämtlicher Eingabegeräte
- Screenreader-Unterstützung
- Sprachsteuerung
- hohe Konfigurierbarkeit

---

# 3. Grundprinzipien

Nova basiert auf den vier WCAG-Prinzipien.

```text
Wahrnehmbar

↓

Bedienbar

↓

Verständlich

↓

Robust
```

Diese Prinzipien bilden die Grundlage sämtlicher UI-Komponenten. :contentReference[oaicite:1]{index=1}

---

# 4. Architektur

```text
Nova Accessibility Framework

├── Accessibility Manager
├── Accessibility Tree
├── Screen Reader API
├── Keyboard Manager
├── Focus Manager
├── High Contrast Engine
├── Magnifier
├── Speech Services
├── Input Adaptation
├── Cognitive Support
├── Automation API
└── Accessibility Inspector
```

---

# 5. Unterstützte Benutzergruppen

Nova unterstützt insbesondere:

- blinde Benutzer
- sehbehinderte Benutzer
- farbfehlsichtige Benutzer
- gehörlose Benutzer
- schwerhörige Benutzer
- motorisch eingeschränkte Benutzer
- Nutzer mit Tremor
- Nutzer mit kognitiven Einschränkungen
- ältere Menschen

Accessibility richtet sich grundsätzlich an alle Benutzer.

---

# 6. Accessibility Tree

Jede Oberfläche erzeugt automatisch einen Accessibility Tree.

```text
Desktop

↓

Fenster

↓

Container

↓

Controls

↓

Text

↓

Icons
```

Jedes Element besitzt:

- Name
- Rolle
- Beschreibung
- Zustand
- Aktionen
- Position

---

# 7. Accessibility Rollen

Standardrollen:

```text
Window

Dialog

Button

Label

TextField

List

Menu

MenuItem

Image

Tree

Table

Toolbar

Tab

Slider

Checkbox

RadioButton

ProgressBar
```

Neue Controls müssen eigene Rollen definieren.

---

# 8. Tastaturbedienung

Grundsatz:

> Jede Funktion muss vollständig ohne Maus erreichbar sein.

Unterstützt:

- TAB
- SHIFT+TAB
- ENTER
- SPACE
- ESC
- Pfeiltasten
- Funktionstasten
- Tastenkombinationen

Es darf niemals eine Tastaturfalle entstehen. Sämtliche Funktionen müssen über die Tastatur erreichbar sein. :contentReference[oaicite:2]{index=2}

---

# 9. Fokus

Der Fokus besitzt folgende Eigenschaften:

- jederzeit sichtbar
- ausreichender Kontrast
- Themefähig
- Animation optional

Standard:

```text
2 px Accent Ring
```

---

# 10. Fokusreihenfolge

Die Fokusreihenfolge folgt:

```text
oben

↓

unten

↓

links

↓

rechts
```

Sie muss logisch und vorhersehbar sein.

---

# 11. Screenreader

Nova unterstützt:

- UI Automation
- AT-SPI
- zukünftige Accessibility APIs

Jedes UI-Element liefert:

- Name
- Rolle
- Status
- Beschreibung
- Aktionen

---

# 12. Sprachsteuerung

Unterstützt:

- Navigation
- Texteingabe
- Diktat
- Programmsteuerung
- Fenstersteuerung

Beispiele:

```text
Öffne Explorer

Schließe Fenster

Scrolle nach unten

Klicke Speichern
```

---

# 13. Vergrößerung

Nova unterstützt:

```text
100 %

125 %

150 %

175 %

200 %

250 %

300 %

400 %
```

Alle Komponenten skalieren verlustfrei.

---

# 14. DPI-Skalierung

Unterstützt:

- mehrere Monitore
- unterschiedliche DPI
- dynamisches Wechseln
- HDR

Alle Controls sind vollständig DPI-unabhängig.

---

# 15. Hoher Kontrast

Nova besitzt integrierte Kontrastmodi.

Beispiele:

```text
Hell

Dunkel

High Contrast Dark

High Contrast Light

Benutzerdefiniert
```

---

# 16. Farben

Farben dürfen niemals allein Informationen transportieren.

Zusätzlich werden verwendet:

- Icons
- Muster
- Text
- Badges

Farben allein dürfen keine Bedeutung vermitteln. Zusätzliche visuelle Merkmale sind erforderlich. :contentReference[oaicite:3]{index=3}

---

# 17. Kontrast

Alle Standardoberflächen erfüllen mindestens:

```text
WCAG AA
```

Textkontrast:

```text
≥ 4.5 : 1
```

Großer Text:

```text
≥ 3 : 1
```

Nova empfiehlt intern höhere Kontrastwerte für kritische Oberflächen. :contentReference[oaicite:4]{index=4}

---

# 18. Animationen

Animationen können reduziert werden.

Unterstützt:

```text
Normal

↓

Reduziert

↓

Deaktiviert
```

---

# 19. Zeitabhängige Inhalte

Benutzer dürfen:

- Zeit verlängern
- Animation pausieren
- automatische Wiedergabe stoppen

---

# 20. Audio

Audio unterstützt:

- Lautstärkeregelung
- Untertitel
- Transkripte
- visuelle Hinweise

---

# 21. Sprache

Nova unterstützt:

- einfache Sprache
- Lokalisierung
- Screenreader-Aussprache
- Sprachumschaltung

---

# 22. Fehlerbehandlung

Fehler müssen:

- verständlich
- eindeutig
- lösungsorientiert

sein.

Beispiel:

```text
Passwort zu kurz.

Mindestens 12 Zeichen erforderlich.
```

Fehlermeldungen sollen Benutzern helfen, Eingaben zu korrigieren und verständlich formuliert sein. :contentReference[oaicite:5]{index=5}

---

# 23. Kognitive Unterstützung

Nova bietet:

- reduzierte Ablenkung
- klare Navigation
- konsistente Bedienung
- einfache Sprache
- Fokusmodus

---

# 24. Touch Accessibility

Unterstützt:

- große Trefferflächen
- Gestenalternativen
- längere Berührungszeiten

Mindestgröße:

```text
44 × 44 px
```

---

# 25. Maus

Unterstützt:

- vergrößerten Cursor
- farbigen Cursor
- Cursor-Schatten
- Mausspuren

---

# 26. Entwickler-API

Das Nova SDK stellt bereit:

```text
AccessibilityObject

↓

Role

↓

State

↓

Action

↓

Relation

↓

LiveRegion

↓

AutomationPeer
```

---

# 27. Accessibility Events

Standardereignisse:

```text
FocusChanged

↓

ValueChanged

↓

SelectionChanged

↓

Expanded

↓

Collapsed

↓

Notification
```

---

# 28. Accessibility Inspector

Nova enthält einen integrierten Inspector.

Er zeigt:

- Rollen
- Namen
- Fokus
- Kontrast
- Reihenfolge
- Tree
- Fehler

---

# 29. Performance

Accessibility darf:

- keine sichtbaren Verzögerungen verursachen
- GPU-Rendering nicht beeinträchtigen
- Speicher effizient nutzen

---

# 30. Erweiterbarkeit

Neue Controls müssen:

- Accessibility Tree unterstützen
- Rollen definieren
- Tastaturbedienung besitzen
- Screenreader unterstützen
- Fokus unterstützen

---

# 31. Positive Konsequenzen

- vollständige Barrierefreiheit
- gesetzeskonforme Anwendungen
- höhere Benutzerzufriedenheit
- bessere Softwarequalität
- konsistente Entwicklung

---

# 32. Negative Konsequenzen

- höherer Entwicklungsaufwand
- umfangreichere Tests
- größere API

---

# 33. Auswirkungen auf andere Module

Diese Spezifikation betrifft:

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-WINDOW-0001 – Nova Window System
- NPSPEC-DIALOG-0001 – Nova Dialog System
- sämtliche `NPSPEC-CONTROL-*`
- Theme Manager
- Window Manager
- Input Manager
- Notification System

Alle Komponenten innerhalb von Nova OS MÜSSEN dieser Spezifikation entsprechen.

---

# 34. Umsetzungsregeln

- Alle UI-Komponenten MÜSSEN vollständig per Tastatur bedienbar sein.
- Jeder Control MUSS einen Accessibility-Namen besitzen.
- Jeder Control MUSS eine Accessibility-Rolle besitzen.
- Farben DÜRFEN niemals die einzige Informationsquelle sein.
- Alle Standardoberflächen MÜSSEN mindestens WCAG-AA erfüllen.
- Focus MÜSSEN jederzeit sichtbar sein.
- Screenreader MÜSSEN vollständig unterstützt werden.
- Accessibility MUSS Bestandteil jeder neuen UI-Komponente sein.

---

# 35. Referenzen

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-WINDOW-0001 – Nova Window System
- NPSPEC-DIALOG-0001 – Nova Dialog System
- W3C – Web Content Accessibility Guidelines (WCAG) 2.1 :contentReference[oaicite:6]{index=6}
- W3C – Accessibility Principles :contentReference[oaicite:7]{index=7}
- Fluent 2 – Accessibility Guidelines :contentReference[oaicite:8]{index=8}

---

# 36. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Accessibility Framework als verbindlicher Accessibility-Standard für Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.