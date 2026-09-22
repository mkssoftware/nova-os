# NPSPEC-UI-THEME-0001 – Nova Theme System

## Status

Angenommen

## Kategorie

UI / Theme / Visual Design / System UI

## Zweck

NovaOS verwendet ein systemweites Theme-System, das die visuelle Darstellung aller nativen UI-Komponenten konsistent steuert.

```text
Semantic UI
    ↓
UI Components
    ↓
Theme
    ↓
Visual Representation
```

Das Theme verändert die Darstellung, nicht die funktionale oder semantische Bedeutung eines UI-Elements.

## Grundprinzip

UI-Komponenten definieren bevorzugt semantische Rollen statt feste Farben oder Darstellungswerte.

```text
Nicht:

Background = #202020
Text = #FFFFFF

Sondern:

Background = Surface.Primary
Text = Text.Primary
```

Das aktive Theme löst diese Rollen in konkrete visuelle Werte auf.

## Architektur

```text
System Theme
     ↓
Theme Tokens
     ↓
UI Components
     ↓
Scene Graph
     ↓
Compositor
```

Anwendungen verwenden dieselben Theme-Grundlagen wie die NovaOS-Systemoberfläche.

## Theme Tokens

Das Theme stellt zentrale Designwerte bereit.

Beispiele:

```text
Colors
Typography
Spacing
Corner Radius
Borders
Shadows
Transparency
Blur
Elevation
Animation
Icon Style
```

Diese Werte werden über definierte semantische Tokens angesprochen.

## Farben

Farben werden nach ihrer Funktion definiert.

Beispiele:

```text
Surface.Primary
Surface.Secondary
Surface.Elevated

Text.Primary
Text.Secondary
Text.Disabled

Accent.Primary
Accent.Secondary

State.Success
State.Warning
State.Error
State.Information
```

Anwendungen sollen keine Systemfarben anhand konkreter RGB-Werte voraussetzen.

## Dark und Light Mode

NovaOS unterstützt mindestens:

```text
Dark
Light
```

Das bevorzugte NovaOS-Standarddesign darf auf einer ruhigen dunklen Fluent-/Acrylic-artigen Darstellung basieren.

Ein Theme-Wechsel erfolgt dynamisch.

```text
Theme Change
     ↓
Affected Properties
     ↓
Damage
     ↓
Recomposition
```

Ein Neustart der Anwendung ist nicht erforderlich.

## Material und Transparenz

NovaOS darf Materialien wie:

```text
Solid
Transparent
Acrylic
Glass
Blurred Surface
Elevated Surface
```

bereitstellen.

Beispiel:

```text
Background
   ↓
Blur
   ↓
Tint
   ↓
Noise / Material
   ↓
Foreground Content
```

Materialien werden zentral durch das Theme-System beschrieben.

## Effekte

Visuelle Effekte können umfassen:

```text
Blur
Transparency
Shadow
Glow
Depth
Rounded Corners
Transitions
```

Effekte sind optional.

```text
Full Effects
     ↓
Reduced Effects
     ↓
Basic Composition
```

Funktionalität darf nicht von dekorativen Effekten abhängen.

## Akzentfarbe

NovaOS kann eine systemweite Akzentfarbe besitzen.

Diese kann stammen aus:

```text
User Selection
Wallpaper
Theme
System Default
```

Die automatisch bestimmte Akzentfarbe muss ausreichenden Kontrast gewährleisten.

## Typografie

Das Theme definiert semantische Textrollen.

```text
Display
Title
Heading
Body
Caption
Label
Code
```

Komponenten verwenden diese Rollen statt fest eingebauter Schriftgrößen.

## Skalierung

Theme-Werte werden in logischen UI-Einheiten beschrieben.

```text
Theme Token
     ↓
Logical Units
     ↓
Display Scaling
     ↓
Physical Output
```

Themes dürfen nicht von einer bestimmten Displayauflösung abhängig sein.

## Animation

Animationsparameter können zentral definiert werden.

```text
Duration
Easing
Transition Type
Motion Scale
```

Accessibility- oder Nutzereinstellungen können Animationen reduzieren oder vollständig deaktivieren.

## Accessibility

Accessibility-Einstellungen haben Vorrang vor dekorativen Theme-Vorgaben.

Beispiele:

```text
High Contrast
Large Text
Reduced Transparency
Reduced Motion
Enhanced Focus Indicators
```

```text
Theme
  +
Accessibility Constraints
       ↓
Effective Theme
```

## System-UI

Sicherheitskritische System-UI verwendet kontrollierte Theme-Rollen.

Anwendungen dürfen geschützte Systemdialoge nicht allein durch Theme-Nachbildung als authentische NovaOS-Systemoberfläche ausgeben.

Visuelle Ähnlichkeit erzeugt keine Systemautorität.

## Anwendungen

Native Anwendungen sollen standardmäßig dem System-Theme folgen.

Sie dürfen eigene visuelle Identität besitzen, müssen aber grundlegende Anforderungen an:

```text
Readability
Accessibility
Semantic States
Focus Visibility
System Dialog Separation
```

einhalten.

## Theme-Wechsel

Themes müssen zur Laufzeit wechselbar sein.

```text
Old Theme
    ↓
Theme Transaction
    ↓
New Theme
    ↓
Incremental UI Update
```

Der Wechsel soll atomar aus Sicht der sichtbaren UI erfolgen, damit keine Mischung inkonsistenter Theme-Zustände sichtbar wird.

## Benutzerdefinierte Themes

NovaOS darf zusätzliche Themes unterstützen.

```text
Built-in Theme
User Theme
Third-party Theme
```

Themes dürfen ausschließlich definierte Darstellungsparameter verändern.

Sie dürfen keine:

```text
Capabilities
Permissions
Executable Logic
Security Policies
```

erhalten oder verändern.

## Ressourcenanpassung

Das Theme-System kann mit der Ressourcenökonomie zusammenarbeiten.

```text
Normal
→ Blur + Transparency + Animation

Resource Pressure
→ Reduced Effects

Fallback
→ Solid Surfaces
```

Dabei bleibt die semantische UI unverändert.

## Normative Anforderungen

1. NovaOS MUSS ein systemweites Theme-System bereitstellen.
2. Native Komponenten SOLLEN semantische Theme Tokens statt fester Darstellungswerte verwenden.
3. Theme und funktionale UI-Semantik MÜSSEN getrennt bleiben.
4. Dark und Light Mode MÜSSEN unterstützt werden.
5. Themes MÜSSEN zur Laufzeit wechselbar sein.
6. Theme-Wechsel SOLLEN ohne Neustart von Anwendungen erfolgen.
7. Farben, Typografie, Abstände, Materialien und Effekte SOLLEN zentral definierbar sein.
8. Theme-Werte SOLLEN auf logischen UI-Einheiten basieren.
9. Accessibility-Einstellungen MÜSSEN gegenüber dekorativen Theme-Vorgaben Vorrang besitzen.
10. Visuelle Effekte MÜSSEN kontrolliert reduzierbar oder deaktivierbar sein.
11. Funktionalität DARF nicht von Blur, Transparenz oder anderen dekorativen Effekten abhängen.
12. Benutzerdefinierte Themes DÜRFEN keine zusätzlichen Capabilities oder Berechtigungen erhalten.
13. Anwendungen DÜRFEN durch Theme-Nachbildung keine Systemautorität erhalten.
14. Theme-Änderungen SOLLEN inkrementelle UI- und Damage-Updates auslösen.
15. Bei fehlender GPU-Unterstützung MUSS eine funktionale vereinfachte Darstellung möglich bleiben.

## Abhängigkeiten

- `NPSPEC-UI-COMPONENT-0001`
- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-RESPONSIVE-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `NPSPEC-UI-GPUCOMPOSITION-0001`
- `NPSPEC-UI-DAMAGETRACKING-0001`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

## Ergebnis

NovaOS trennt konsequent zwischen Bedeutung und Darstellung:

```text
Semantic UI
     ↓
Component
     ↓
Theme Tokens
     ↓
Effective Theme
     ↓
Scene Graph
     ↓
Rendering
```

Dadurch kann NovaOS sein ruhiges, modernes und futuristisches Erscheinungsbild systemweit konsistent darstellen, während Dark/Light Mode, Accessibility, individuelle Themes und reduzierte Effekte dieselben UI-Komponenten verwenden.