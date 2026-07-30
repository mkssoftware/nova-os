# NPSPEC-CONTROL-LABEL-0001: Nova Label Control

- **Status:** Angenommen
- **Version:** 1.0
- **Kategorie:** UI Controls
- **Erstellt:** 2026-07-23
- **Gültig für:** Nova UI Framework, Nova SDK, Nova Desktop Shell und alle Nova-Anwendungen
- **Ersetzt:** Keine

---

# 1. Einleitung

Diese Spezifikation definiert den **Nova Label Control (NLB)**.

Ein Label dient zur Beschriftung anderer Benutzeroberflächen-Elemente oder zur Anzeige kurzer statischer Informationen.

Labels gehören zu den grundlegendsten UI-Komponenten und müssen in allen Nova-Anwendungen einheitlich verwendet werden.

---

# 2. Zielsetzung

Der Nova Label Control verfolgt folgende Ziele:

- eindeutige Beschriftung
- hohe Lesbarkeit
- konsistentes Erscheinungsbild
- minimale visuelle Ablenkung
- vollständige Accessibility
- Themefähigkeit
- GPU-beschleunigte Darstellung

---

# 3. Designphilosophie

Ein Label beantwortet immer die Frage:

> **"Was ist dieses Element?"**

Ein Label darf niemals dekorativ eingesetzt werden.

Labels dienen ausschließlich der Beschreibung oder Kennzeichnung von Informationen oder Steuerelementen.

Labels sollen kurz und präzise formuliert werden. Platzhalter dürfen niemals Labels ersetzen. :contentReference[oaicite:0]{index=0}

---

# 4. Architektur

```text
Nova Label

├── Text
├── Icon (optional)
├── Required Indicator
├── Info Indicator
├── State
├── Theme
├── Typography
└── Accessibility
```

---

# 5. Komponentenaufbau

```text
🛈 Benutzername
```

Optional:

- Symbol
- Pflichtfeld-Markierung
- Info-Button
- Statussymbol

---

# 6. Labeltypen

Nova definiert folgende Standardtypen:

- Standard Label
- Field Label
- Section Label
- Group Label
- Caption
- Status Label
- Badge Label
- Title Label

Neue Typen benötigen eine eigene Spezifikation.

---

# 7. Standard Label

Verwendung:

- allgemeine Beschriftungen
- kurze Hinweise
- Informationen

Eigenschaften:

- normale Schrift
- neutraler Farbton
- keine Interaktion

---

# 8. Field Label

Field Labels beschriften Eingabefelder.

Beispiel:

```text
Vorname

┌────────────────────┐
│                    │
└────────────────────┘
```

Field Labels stehen standardmäßig oberhalb des Eingabefeldes. :contentReference[oaicite:1]{index=1}

---

# 9. Section Label

Section Labels strukturieren Inhalte.

Beispiele:

```text
Netzwerk

Benutzer

Darstellung

Datenschutz
```

Eigenschaften:

- größere Schrift
- semibold
- zusätzlicher Abstand

---

# 10. Group Label

Beschreibt eine Gruppe von Elementen.

Beispiel:

```text
Persönliche Daten

Vorname

Nachname

Geburtsdatum
```

---

# 11. Caption

Captions beschreiben:

- Bilder
- Diagramme
- Tabellen
- Medien

Eigenschaften:

- kleinere Schrift
- dezente Farbe

---

# 12. Status Label

Status Labels zeigen Zustände.

Beispiele:

```text
Online

Offline

Synchronisiert

Fehler
```

Status Labels dürfen zusätzlich Farbakzente besitzen.

---

# 13. Badge Label

Badge Labels zeigen:

- Neu
- Beta
- Vorschau
- Administrator
- Lokal
- Cloud

Sie dürfen maximal zwei Wörter enthalten. :contentReference[oaicite:2]{index=2}

---

# 14. Größen

| Größe | Schrift |
|---------|---------:|
| XS | 10 px |
| Small | 12 px |
| Medium | 14 px |
| Large | 16 px |
| XL | 20 px |
| XXL | 24 px |
| Display | 32 px |

Die Schriftgrößen basieren auf den Nova Design Tokens.

---

# 15. Typografie

Standard:

- Nova Sans
- Sentence Case
- linksbündig
- optimale Zeilenhöhe

Großbuchstaben werden nur für spezielle Oberflächen verwendet.

Labels sollen in Sentence Case geschrieben werden und keine unnötige Interpunktion enthalten. :contentReference[oaicite:3]{index=3}

---

# 16. Farben

Standardfarben:

```text
Primary Text

Secondary Text

Disabled

Error

Success

Warning

Accent
```

Alle Farben stammen aus den Nova Design Tokens.

---

# 17. Ausrichtung

Unterstützt:

- links
- zentriert
- rechts
- Blocksatz (nur Dokumente)

Standard:

```text
Linksbündig
```

---

# 18. Umbruch

Regeln:

- Labels dürfen umbrechen
- keine Abschneidung
- Ellipsis nur wenn ausdrücklich erlaubt

Mehrzeilige Labels sind zulässig. Sie dürfen nicht abgeschnitten werden, wenn genügend Platz vorhanden ist. :contentReference[oaicite:4]{index=4}

---

# 19. Pflichtfelder

Pflichtfelder besitzen:

```text
Vorname *
```

Alternativ:

```text
Vorname

(required)
```

Die Pflichtmarkierung muss auch von Screenreadern erkannt werden. :contentReference[oaicite:5]{index=5}

---

# 20. Info Labels

Optional kann ein Info-Button ergänzt werden.

```text
Benutzername  (i)
```

Der Info-Button zeigt:

- Hilfetexte
- Erläuterungen
- Dokumentation

Wichtige Informationen dürfen niemals ausschließlich im Info-Popover stehen. :contentReference[oaicite:6]{index=6}

---

# 21. Zustände

Labels unterstützen:

```text
Normal

↓

Disabled

↓

Error

↓

Warning

↓

Success
```

Normale Labels besitzen keinen Hover-Effekt.

---

# 22. Icons

Optional.

Verwendet werden ausschließlich:

- Fluent SVG Icons

Standardgröße:

```text
16 × 16 px
```

---

# 23. Animationen

Labels besitzen standardmäßig:

- keine Animation

Optional:

- Fade
- Color Transition

Animationen müssen dem Nova Motion System entsprechen.

---

# 24. Accessibility

Unterstützt:

- Screenreader
- hohe Kontraste
- große Schrift
- Zoom
- Sprachsteuerung

Jedes Eingabefeld MUSS programmatisch mit seinem Label verknüpft sein. Platzhalter ersetzen niemals die Beschriftung. :contentReference[oaicite:7]{index=7}

---

# 25. Performance

Labels sollen:

- GPU-beschleunigt gerendert werden
- minimale Speicherbelegung besitzen
- keine unnötigen Layout-Neuberechnungen erzeugen

---

# 26. Entwickler-API

Das Nova SDK stellt bereit:

```text
Label

↓

FieldLabel

↓

SectionLabel

↓

GroupLabel

↓

Caption

↓

StatusLabel

↓

BadgeLabel

↓

TitleLabel
```

---

# 27. Erweiterbarkeit

Neue Labeltypen müssen:

- Nova Typography verwenden
- Design Tokens verwenden
- Themefähig sein
- Accessibility unterstützen

---

# 28. Positive Konsequenzen

- konsistente Benutzeroberflächen
- hohe Lesbarkeit
- einfache Wartbarkeit
- klare Benutzerführung
- vollständige Theme-Unterstützung

---

# 29. Negative Konsequenzen

- höhere Standardisierung
- zusätzlicher Pflegeaufwand für Typografie
- umfangreiche UI-Tests erforderlich

---

# 30. Auswirkungen auf andere Module

Diese Spezifikation betrifft:

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-TYPOGRAPHY-0001 *(zukünftig)*
- NPSPEC-CONTROL-TEXTFIELD-0001
- NPSPEC-CONTROL-BUTTON-0001
- NPSPEC-DIALOG-0001
- Theme Manager
- Accessibility Manager

Alle Beschriftungen innerhalb von Nova OS MÜSSEN dieser Spezifikation entsprechen.

---

# 31. Umsetzungsregeln

- Jedes Eingabefeld MUSS ein sichtbares Label besitzen.
- Platzhalter DÜRFEN Labels niemals ersetzen.
- Labels SOLLEN kurz und eindeutig formuliert sein.
- Labels MÜSSEN Themefähig sein.
- Alle Farben MÜSSEN aus den Nova Design Tokens stammen.
- Fluent SVG Icons MÜSSEN verwendet werden.
- Accessibility MUSS vollständig unterstützt werden.

---

# 32. Referenzen

- NPSPEC-DESIGN-0001 – Nova Design Language
- NPSPEC-MOTION-0001 – Nova Motion System
- NPSPEC-CONTROL-TEXTFIELD-0001 – Nova Text Field Control
- NPSPEC-CONTROL-BUTTON-0001 – Nova Button Control
- Fluent 2 – Label Usage Guidance :contentReference[oaicite:8]{index=8}
- Fluent 2 – Field Usage Guidance :contentReference[oaicite:9]{index=9}
- Fluent 2 – Info Label Guidance :contentReference[oaicite:10]{index=10}

---

# 33. Statushistorie

- **2026-07-23:** Version 1.0 erstellt.
- **2026-07-23:** Nova Label Control als verbindlicher Standard für alle Beschriftungen in Nova OS definiert.
- **Zukünftige Änderungen:** Nur durch eine neue Version dieser Spezifikation oder eine ersetzende NPSPEC zulässig.