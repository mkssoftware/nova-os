# NPSPEC-UI-COLOR-0001 – Nova Color System

## Status

Angenommen

## Kategorie

UI / Color / Theme / Accessibility / Rendering

## Zweck

NovaOS verwendet ein systemweites semantisches Farbsystem.

UI-Komponenten verwenden keine fest definierten Farben, sondern **Color Tokens**, deren konkrete Farbwerte durch Theme, Kontext und Accessibility bestimmt werden.

```text
Semantic Color Role
        ↓
Theme
        ↓
Accessibility Constraints
        ↓
Effective Color
        ↓
Rendering
```

## Grundprinzip

Statt:

```text
Background = #202020
Text       = #FFFFFF
```

verwendet NovaOS:

```text
Background = Surface.Primary
Text       = Text.Primary
```

Die konkrete Farbe wird erst durch das aktive Theme bestimmt.

## Farbrollen

Das Nova Color System definiert semantische Farbgruppen.

```text
Surface.*
Text.*
Accent.*
Border.*
State.*
Selection.*
Focus.*
Overlay.*
```

## Surface Colors

Oberflächen verwenden abgestufte semantische Rollen.

```text
Surface.Background
Surface.Primary
Surface.Secondary
Surface.Elevated
Surface.Overlay
Surface.Disabled
```

Diese Rollen können abhängig vom Theme unterschiedliche Farben und Transparenzwerte besitzen.

## Text Colors

Textfarben werden nach ihrer Bedeutung definiert.

```text
Text.Primary
Text.Secondary
Text.Tertiary
Text.Disabled
Text.OnAccent
Text.Link
```

Textfarben müssen ausreichenden Kontrast zum jeweiligen Hintergrund besitzen.

## Accent Colors

Akzentfarben dienen zur Hervorhebung wichtiger Interaktionen.

```text
Accent.Primary
Accent.Secondary
Accent.Hover
Accent.Pressed
Accent.Subtle
```

Die Akzentfarbe kann stammen aus:

```text
System Default
Theme
User Selection
Wallpaper
```

Automatisch erzeugte Varianten müssen weiterhin Accessibility-Anforderungen erfüllen.

## Statusfarben

Systemzustände erhalten semantische Rollen.

```text
State.Success
State.Information
State.Warning
State.Error
State.Critical
```

Farbe darf jedoch niemals die einzige Informationsquelle sein.

Beispiel:

```text
Error

Color
+
Icon
+
Semantic State
+
Optional Text
```

## Interaktionszustände

UI-Komponenten können Farbvarianten für Zustände besitzen.

```text
Normal
Hover
Pressed
Selected
Focused
Disabled
```

Diese werden aus semantischen Tokens erzeugt.

```text
Button.Background
Button.Background.Hover
Button.Background.Pressed
```

Komponenten sollen nicht selbst beliebige Zustandsfarben berechnen.

## Auswahl und Fokus

Auswahl und Keyboard-Fokus müssen klar unterscheidbar sein.

```text
Selection.Background
Selection.Foreground

Focus.Border
Focus.Highlight
```

Ein Focus Indicator muss auch bei benutzerdefinierten Akzentfarben ausreichend sichtbar bleiben.

## Dark Mode

Dark Mode verwendet dunkle Oberflächen mit abgestuften Helligkeitswerten.

```text
Background
   ↓
Surface
   ↓
Elevated Surface
   ↓
Foreground
```

Nicht jede Oberfläche soll vollständig schwarz sein.

Abstufungen unterstützen visuelle Hierarchie und den von NovaOS vorgesehenen ruhigen Fluent-/Glass-Look.

## Light Mode

Light Mode verwendet dieselben semantischen Tokens.

```text
Surface.Primary
Text.Primary
Accent.Primary
```

Nur deren konkrete Werte ändern sich.

Anwendungen benötigen dadurch keine separate Farbarchitektur für Dark und Light Mode.

## Transparenz und Glass

Farben können mit Materialeigenschaften kombiniert werden.

```text
Base Color
    +
Opacity
    +
Backdrop
    +
Blur
    ↓
Effective Surface
```

Beispiele:

```text
Glass.Primary
Glass.Secondary
Glass.Overlay
```

Lesbarkeit muss unabhängig vom Hintergrund gewährleistet bleiben.

Falls dies nicht garantiert werden kann, muss eine stärker deckende Darstellung verwendet werden.

## Kontrast

NovaOS muss Kontrastanforderungen zentral prüfen können.

```text
Foreground
    +
Background
    ↓
Contrast Evaluation
    ↓
Allowed / Adjusted
```

Themes und automatische Farbgenerierung dürfen keine unlesbaren Kombinationen erzeugen.

## High Contrast

NovaOS unterstützt einen High-Contrast-Modus.

```text
Normal Theme
      ↓
Accessibility Override
      ↓
High Contrast Palette
```

Dabei dürfen dekorative Farbnuancen zugunsten klarer Trennung reduziert werden.

## Farbfehlsichtigkeit

Wichtige Zustände dürfen nicht ausschließlich über Unterschiede wie:

```text
Red vs Green
Blue vs Purple
```

kommuniziert werden.

Zusätzliche semantische Signale können sein:

```text
Icon
Shape
Pattern
Label
Position
State Description
```

## Farbräume

Die interne Farbdarstellung soll unterschiedliche Display-Farbräume unterstützen können.

Beispiele:

```text
sRGB
Wide Gamut
HDR-capable Color Spaces
```

UI-Farben werden semantisch definiert und durch den Rendering-/Display-Pfad in den geeigneten Ausgabefarbraum überführt.

## Farbmanagement

```text
Semantic Color
      ↓
Theme Color
      ↓
Working Color Space
      ↓
Display Color Management
      ↓
Physical Display
```

Das UI-System soll nicht davon ausgehen, dass jedes Display dieselbe Farbwiedergabe besitzt.

## Multi-Display

Bei mehreren Displays können unterschiedliche Farbprofile aktiv sein.

```text
Same Semantic Color
       ↓
Display A Profile
       ↓
Output A

Same Semantic Color
       ↓
Display B Profile
       ↓
Output B
```

Die semantische Farbe bleibt identisch, während die physische Ausgabe angepasst werden darf.

## Dynamische Farben

Farben dürfen dynamisch aus Kontextinformationen erzeugt werden.

Beispiel:

```text
Wallpaper
    ↓
Accent Extraction
    ↓
Contrast Validation
    ↓
Accent.Primary
```

Automatisch erzeugte Farben müssen vor ihrer Verwendung validiert werden.

## Anwendungen

Native Anwendungen sollen das Nova Color System verwenden.

Eigene Farben sind zulässig für:

```text
Branding
Content
Documents
Graphics
Visualization
Media
```

Systeminteraktive Elemente sollen bevorzugt semantische Systemfarben verwenden.

## Sicherheit

Farben besitzen keine Sicherheitsbedeutung allein aufgrund ihrer Darstellung.

Eine Anwendung darf beispielsweise eine rote Warnfarbe darstellen, erhält dadurch jedoch keine System-UI-Autorität.

Geschützte Sicherheitsdialoge müssen zusätzlich durch die System-UI-Architektur abgesichert werden.

## Performance

Aufgelöste Color Tokens dürfen gecacht werden.

```text
Semantic Token
      ↓
Theme Resolution
      ↓
Cached Effective Color
```

Bei Theme- oder Accessibility-Änderungen werden nur betroffene Farbwerte invalidiert.

## Normative Anforderungen

1. NovaOS MUSS ein semantisches Color-Token-System bereitstellen.
2. Native UI-Komponenten SOLLEN keine fest eingebauten Systemfarben verwenden.
3. Farbrollen MÜSSEN unabhängig von konkreten RGB-Werten definierbar sein.
4. Dark und Light Mode MÜSSEN dieselben semantischen Farbrollen verwenden können.
5. Text- und Interaktionsfarben MÜSSEN ausreichenden Kontrast gewährleisten.
6. Farbe DARF bei wichtigen Zuständen nicht die einzige Informationsquelle sein.
7. Focus und Selection MÜSSEN visuell unterscheidbar sein.
8. Accessibility-Einstellungen MÜSSEN Theme-Farben überschreiben können.
9. High-Contrast-Darstellung MUSS unterstützt werden.
10. Automatisch erzeugte Akzentfarben MÜSSEN vor Verwendung validiert werden.
11. Transparente Oberflächen MÜSSEN bei unzureichender Lesbarkeit auf eine geeignete Darstellung zurückfallen.
12. Unterschiedliche Display-Farbräume SOLLEN unterstützt werden.
13. Farbmanagement SOLL pro Display möglich sein.
14. Theme- und Farbänderungen SOLLEN inkrementell verarbeitet werden.
15. Visuelle Farbe DARF keine Security- oder Capability-Autorität erzeugen.

## Abhängigkeiten

- `NPSPEC-UI-THEME-0001`
- `NPSPEC-UI-COMPONENT-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `NPSPEC-UI-DISPLAYSERVER-0001`
- `NPSPEC-UI-GPUCOMPOSITION-0001`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

## Ergebnis

NovaOS trennt die Bedeutung einer Farbe von ihrem konkreten Farbwert:

```text
Semantic Role
     ↓
Theme
     ↓
Accessibility
     ↓
Color Management
     ↓
Display Output
```

Damit können Dark Mode, Light Mode, Glass-Effekte, Akzentfarben, High Contrast und unterschiedliche Displays auf derselben semantischen Farbbasis arbeiten, ohne dass Anwendungen ihre UI-Farben für jede Darstellung selbst verwalten müssen.