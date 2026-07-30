# NPSPEC-CONTROL-TEXTFIELD-0001: Nova Text Field Control

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** UI Controls
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova UI Framework, Nova SDK, Nova Desktop Shell und alle Nova-Anwendungen
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert den **Nova Text Field Control (NTF)**.

Das Textfeld dient der Eingabe kurzer Texte und bildet die Grundlage sämtlicher Formulare, Suchfelder und Eingabemasken innerhalb von Nova OS.

Diese Spezifikation beschreibt:

- Erscheinungsbild
- Verhalten
- Zustände
- Validierung
- Eingabemethoden
- Animationen
- Accessibility
- API

---

# 2. Zielsetzung

Der Nova Text Field Control verfolgt folgende Ziele:

- schnelle Texteingabe
- konsistentes Verhalten
- hohe Performance
- vollständige Tastaturbedienung
- Touch-Unterstützung
- GPU-beschleunigte Darstellung
- Barrierefreiheit
- Erweiterbarkeit

---

# 3. Designphilosophie

Ein Textfeld soll jederzeit eindeutig erkennen lassen:

- welche Informationen erwartet werden
- ob Eingaben möglich sind
- ob Eingaben korrekt sind
- welcher Zustand aktiv ist

Der Benutzer soll nie rätseln müssen, was eingegeben werden soll.

Ein Textfeld sollte immer mit einer sichtbaren Beschriftung kombiniert werden. Platzhalter dienen lediglich als ergänzende Hilfe und dürfen keine wesentlichen Informationen enthalten. :contentReference[oaicite:0]{index=0}

---

# 4. Architektur

```text
Nova Text Field

├── Label
├── Input Area
├── Placeholder
├── Prefix
├── Suffix
├── Icon
├── Clear Button
├── Validation
├── Helper Text
├── Focus Ring
└── Event Handler
```

---

# 5. Komponentenaufbau

```text
Name

┌──────────────────────────────┐
│ 🔍 Text                      │
└──────────────────────────────┘

Hilfetext
```

Optional:

- Label
- Prefix
- Suffix
- Icon
- Clear Button
- Passwortbutton
- Fortschrittsanzeige

---

# 6. Textfeldtypen

Nova definiert folgende Standardtypen:

- Standard
- Passwort
- Suche
- URL
- E-Mail
- Telefonnummer
- Zahl
- Datum
- Zeit
- Hex-Farbe
- Dateiname

Neue Typen benötigen eine eigene Spezifikation.

---

# 7. Größen

| Größe | Höhe |
|--------|------:|
| Small | 28 px |
| Medium | 36 px |
| Large | 44 px |
| Touch | 52 px |

---

# 8. Beschriftung

Standard:

```text
Label

↓

Textfeld

↓

Hilfetext
```

Labels sollen:

- kurz
- eindeutig
- dauerhaft sichtbar

sein.

Platzhalter ersetzen niemals Labels. :contentReference[oaicite:1]{index=1}

---

# 9. Placeholder

Placeholder dienen ausschließlich als Hinweis.

Beispiele:

```text
Suche...

name@firma.de

https://

MM.JJJJ
```

Der Placeholder verschwindet bei der Eingabe.

Notwendige Informationen dürfen nicht ausschließlich im Placeholder stehen. :contentReference[oaicite:2]{index=2}

---

# 10. Hilfetext

Hilfetexte erklären:

- Format
- Eingaberegeln
- Beispiele
- Einschränkungen

Beispiel:

```text
Mindestens 12 Zeichen
```

---

# 11. Zustände

Jedes Textfeld besitzt:

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

Success

↓

Warning

↓

Error
```

---

# 12. Fokus

Beim Fokus:

- Nova Accent Border
- Focus Ring
- Cursor sichtbar
- Caret blinkt

Nur ein Textfeld besitzt gleichzeitig den Eingabefokus.

---

# 13. Eingabe

Unterstützt:

- Tastatur
- Touch
- Spracheingabe
- Handschrift
- IME
- Zwischenablage

---

# 14. Auswahl

Unterstützt:

- Zeichen
- Wörter
- Zeilen
- Alles markieren

Standard:

```text
STRG+A

STRG+C

STRG+X

STRG+V

STRG+Z

STRG+Y
```

---

# 15. Clear Button

Optional.

Eigenschaften:

- erscheint bei Inhalt
- verschwindet bei leerem Feld
- löscht gesamten Inhalt

Ein Löschsymbol im Eingabefeld verbessert die Bedienbarkeit bei Such- und Eingabefeldern. :contentReference[oaicite:3]{index=3}

---

# 16. Passwortfelder

Unterstützt:

- Passwort anzeigen
- Passwort verbergen
- Passwortgenerator
- Passwortstärke
- Copy deaktivieren (optional)

---

# 17. Validierung

Unterstützt:

- Live
- Bei Fokusverlust
- Beim Absenden

Je nach Eingabetyp kann eine sofortige oder verzögerte Validierung sinnvoll sein. Beispielsweise sollten Passwörter direkt geprüft werden, während E-Mail-Adressen oft erst nach Abschluss der Eingabe validiert werden. :contentReference[oaicite:4]{index=4}

---

# 18. Fehlermeldungen

Bei Fehlern:

- rote Umrandung
- Fehlersymbol
- Fehlermeldung
- Screenreader-Hinweis

Beispiel:

```text
Ungültige E-Mail-Adresse
```

---

# 19. Erfolg

Bei Erfolg:

- grüner Rahmen
- Häkchen
- optionale Erfolgsmeldung

---

# 20. Animationen

Das Textfeld verwendet:

- Fade
- Glow
- Border Animation
- Ripple
- Focus Animation

Alle Animationen folgen dem Nova Motion System.

---

# 21. Prefix

Optional.

Beispiele:

```text
€

$

https://

@
```

---

# 22. Suffix

Optional.

Beispiele:

```text
kg

MB

%

ms
```

---

# 23. Suchfeld

Ein Suchfeld erweitert das Standard-Textfeld um:

- Suchsymbol
- Clear Button
- Live-Suche
- Suchverlauf
- Suchvorschläge

---

# 24. Mehrzeilige Eingabe

Lange Texte werden **nicht** vom Text Field verarbeitet.

Hierfür existiert:

```text
Nova TextArea Control
```

---

# 25. Touchmodus

Touchmodus verwendet:

- größere Trefferfläche
- größere Schrift
- größere Icons
- größere Innenabstände

Mindesthöhe:

```text
52 px
```

---

# 26. Accessibility

Unterstützt:

- Screenreader
- Tastatursteuerung
- Sprachsteuerung
- hohe Kontraste
- große Schrift
- reduzierte Animationen

Alle Eingabefelder benötigen eine programmatisch zugeordnete Beschriftung und dürfen Platzhalter nicht als Ersatz verwenden. :contentReference[oaicite:5]{index=5}

---

# 27. Performance

Das Textfeld soll:

- GPU-beschleunigt rendern
- Eingaben ohne Verzögerung verarbeiten
- minimale Speicherbelegung besitzen
- flüssig bis 240 Hz arbeiten

---

# 28. Entwickler-API

Das Nova SDK stellt bereit:

```text
TextField

↓

SearchField

↓

PasswordField

↓

EmailField

↓

UrlField

↓

NumberField

↓

DateField
```

---

# 29. Erweiterbarkeit

Neue Textfeldtypen müssen:

- Design Tokens verwenden
- Motion Tokens verwenden
- dieselben Zustände unterstützen
- Accessibility vollständig unterstützen

---

# 30. Positive Konsequenzen

- konsistente Formulare
- einfache Bedienung
- hohe Wiedererkennbarkeit
- moderne Optik
- geringe Fehlerquote

---

# 31. Negative Konsequenzen

- höherer Entwicklungsaufwand
- umfangreiche Validierung erforderlich
- mehr UI-Tests notwendig

---

# 32. Auswirkungen auf andere Module

Diese Spezifikation betrifft:

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-DIALOG-0001 – Nova Dialog System
- Theme Manager
- Accessibility Manager
- Input Manager
- Form Framework

Alle Textfelder innerhalb von Nova OS MÜSSEN dieser Spezifikation entsprechen.

---

# 33. Umsetzungsregeln

- Jedes Textfeld MUSS eine eindeutige Beschriftung besitzen.
- Platzhalter DÜRFEN Labels nicht ersetzen.
- Validierungsfehler MÜSSEN klar dargestellt werden.
- Alle Textfelder MÜSSEN vollständig per Tastatur bedienbar sein.
- Alle Animationen MÜSSEN dem Nova Motion System folgen.
- Alle Farben MÜSSEN aus den Nova Design Tokens stammen.
- Textfelder MÜSSEN GPU-beschleunigt gerendert werden.
- Accessibility MUSS vollständig unterstützt werden.

---

# 34. Referenzen

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-INTERACTION-0001 – Nova Interaction Model
- NPSPEC-DIALOG-0001 – Nova Dialog System
- NPSPEC-CONTROL-TEXTAREA-0001 *(zukünftig)*
- Fluent 2 – Input Usage Guide :contentReference[oaicite:6]{index=6}
- Fluent 2 – Field Usage Guide :contentReference[oaicite:7]{index=7}
- Fluent 2 – Text Field Guidance (iOS) :contentReference[oaicite:8]{index=8}

---

# 35. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Text Field Control als verbindlicher Standard für alle einzeiligen Texteingabefelder in Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.