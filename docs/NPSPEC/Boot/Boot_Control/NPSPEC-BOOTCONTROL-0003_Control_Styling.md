# NPSPEC-BOOTCONTROL-0003
# Control Styling

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-0003 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Framework |
| Priorität | Hoch |
| Gültigkeitsbereich | Sämtliche UI-Controls des NovaOS Boot Managers |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTCONTROL-0001, NPSPEC-BOOTCONTROL-0002, NPSPEC-BOOTTHEME-0001, NPSPEC-BOOTRENDER-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Control Styling System** der NovaOS Boot UI.

Das Styling-System beschreibt die visuelle Darstellung aller Controls unabhängig von deren Funktionalität. Sämtliche Farben, Abstände, Rundungen, Materialien und Animationen werden zentral über Styles definiert.

Controls enthalten keine fest codierten Darstellungsparameter.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für sämtliche UI-Controls.

Sie umfasst:

- Buttons
- Labels
- Textfelder
- Checkboxen
- Radiobuttons
- Listen
- Bilder
- ProgressBars
- ScrollViews
- Container
- zukünftige Controls

---

# 3. Architektur

```text
Theme

↓

Style Manager

↓

Style Object

↓

Control

↓

Renderer
```

---

# 4. Grundprinzip

Ein Style beschreibt ausschließlich das Erscheinungsbild eines Controls.

Ein Style enthält keine Geschäftslogik, keine Eingabeverarbeitung und keine Layoutregeln.

Mehrere Controls dürfen denselben Style gemeinsam verwenden.

---

# 5. Aufgaben

Das Styling-System übernimmt:

- Farben
- Schriftarten
- Größen
- Abstände
- Rundungen
- Transparenz
- Materialien
- Schatten
- Animationen
- Zustandsdarstellung

---

# 6. Style-Hierarchie

```text
Theme

├── Button Style

├── Label Style

├── TextBox Style

├── CheckBox Style

├── ProgressBar Style

└── Container Style
```

Gemeinsame Eigenschaften werden vom Theme geerbt.

---

# 7. Style-Komponenten

Ein Style besteht aus:

```text
Colors

Typography

Geometry

Materials

Effects

Animations

Accessibility
```

---

# 8. Farbdefinitionen

Jeder Style definiert mindestens:

- Hintergrundfarbe
- Vordergrundfarbe
- Textfarbe
- Rahmenfarbe
- Fokusfarbe
- Akzentfarbe
- Fehlerfarbe
- Warnfarbe

Alle Farben stammen aus dem aktiven Theme.

---

# 9. Typografie

Ein Style definiert:

- Schriftfamilie
- Schriftgröße
- Schriftschnitt
- Zeilenhöhe
- Zeichenabstand
- Textausrichtung

Alle Schriften werden zentral verwaltet.

---

# 10. Geometrie

Ein Style definiert:

- Innenabstand (Padding)
- Außenabstand (Margin)
- Rahmenstärke
- Eckradius
- Mindestgröße
- Maximalgröße

Die Geometrie beeinflusst nicht das Layoutverhalten.

---

# 11. Materialien

Unterstützt werden:

- Opaque
- Glass
- Acrylic
- Transparent

Das Material bestimmt:

- Transparenz
- Hintergrundunschärfe
- Lichtreflexion
- Tiefenwirkung

---

# 12. Visuelle Effekte

Ein Style kann enthalten:

- Schatten
- Glow
- Border Highlight
- Inner Shadow
- Farbverläufe

Effekte werden ausschließlich durch den Renderer erzeugt.

---

# 13. Zustandsabhängige Styles

Für jeden Zustand können eigene Darstellungen definiert werden:

```text
Normal

Hovered

Focused

Pressed

Disabled

Selected

Busy

Error
```

Nicht definierte Zustände erben den Standardstil.

---

# 14. Style-Vererbung

Styles dürfen Eigenschaften erben.

Beispiel:

```text
Base Button

↓

Primary Button

↓

Destructive Button
```

Nur geänderte Eigenschaften werden überschrieben.

---

# 15. Themewechsel

Beim Themewechsel:

```text
Theme

↓

Styles

↓

Controls

↓

Renderer
```

Alle Controls werden automatisch aktualisiert.

---

# 16. Animationen

Styles definieren Animationen für:

- Farbwechsel
- Transparenz
- Fokus
- Schatten
- Materialwechsel

Die Ausführung erfolgt durch das Motion-System.

---

# 17. Accessibility

Jeder Style muss unterstützen:

- High Contrast
- Reduced Motion
- hohe DPI
- Screenreader-Kompatibilität
- ausreichende Farbkontraste

Accessibility-Styles überschreiben normale Styles.

---

# 18. Ressourcenverwaltung

Styles werden:

- einmal geladen
- zentral gespeichert
- zwischen Controls geteilt

Doppelte Style-Objekte sind zu vermeiden.

---

# 19. Speicherverwaltung

Styles verwenden:

- statische Tabellen
- konstante Farbpaletten
- gemeinsame Materialdefinitionen

Heap-Allokationen während der Laufzeit sind verboten.

---

# 20. Performance

Anforderungen:

- O(1)-Stylezugriff
- O(1)-Themewechsel pro Control
- deterministische Laufzeit
- keine Heap-Allokationen

Styles dürfen nicht mehrfach berechnet werden.

---

# 21. Fehlerbehandlung

Folgende Fehler müssen erkannt werden:

- fehlender Style
- ungültige Farbe
- ungültiges Material
- beschädigtes Theme
- Nullpointer

Kann ein Style nicht geladen werden, wird der Standardstil verwendet.

---

# 22. Sicherheit

Das System verhindert:

- ungültige Style-Referenzen
- rekursive Style-Vererbung
- beschädigte Theme-Daten
- fehlende Accessibility-Styles
- inkonsistente Darstellungen

---

# 23. API

```c
typedef struct nova_style nova_style_t;

nova_result_t
nova_style_apply(
    nova_control_t* control,
    const nova_style_t* style);

const nova_style_t*
nova_style_get(
    nova_style_id_t id);

nova_result_t
nova_style_reload(void);

nova_result_t
nova_style_theme_changed(void);
```

---

# 24. Diagnosedaten

Zu erfassen sind:

- Style-ID
- Theme-ID
- Materialtyp
- Farbpalette
- Themewechsel
- Renderzeit
- Speicherverbrauch
- Fehleranzahl

---

# 25. Testfälle

Zu testen sind:

- Themewechsel
- Style-Vererbung
- Glass Material
- Acrylic Material
- Fokusstil
- Disabled Style
- Error Style
- High Contrast
- Reduced Motion
- DPI-Skalierung

---

# 26. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Darstellungsparameter ausschließlich über Styles definiert werden.
- Controls keine fest codierten Farben oder Größen enthalten.
- Style-Vererbung korrekt funktioniert.
- Themewechsel automatisch alle Controls aktualisiert.
- Accessibility-Stile vollständig unterstützt werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identische Styles erzeugen.

---

# 27. Referenzen

## ADRs

- ADR-BOOTCONTROL-0003 – Zentrales Styling-System
- ADR-BOOTTHEME-0001 – Theme-Architektur
- ADR-BOOTRENDER-0015 – Shadows and Glows
- ADR-BOOTCOMPOSITOR-0006 – Glass Material
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTCONTROL-0001 – Boot Control Framework
- NPSPEC-BOOTCONTROL-0002 – Control State Model
- NPSPEC-BOOTTHEME-0001 – Theme System
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 28. Zusammenfassung

Das **Control Styling System** definiert die vollständige visuelle Darstellung sämtlicher UI-Controls der NovaOS Bootoberfläche. Farben, Typografie, Materialien, Geometrie, Effekte und Animationen werden zentral über Styles verwaltet und konsequent von der Steuerlogik getrennt. Durch Style-Vererbung, Theme-Integration, Accessibility-Unterstützung und statische Ressourcenverwaltung entsteht ein konsistentes, erweiterbares und performantes Erscheinungsbild für alle Controls auf BIOS- und UEFI-Systemen.