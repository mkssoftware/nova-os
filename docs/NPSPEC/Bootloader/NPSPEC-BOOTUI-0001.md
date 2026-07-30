# NPSPEC-BOOTUI-0001
# Nova Boot User Interface Framework

| Feld | Wert |
|------|------|
| Dokument | NPSPEC-BOOTUI-0001 |
| Titel | Nova Boot User Interface Framework |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | Boot UI |
| Bereich | Nova Boot Platform |
| Übergeordnet | NPSPEC-BOOTSTAGE2-0001 |
| Baut auf | NPSPEC-BOOTGRAPHICS-0001, NPSPEC-BOOTINPUT-0001 |
| Letzte Änderung | 2026-07-16 |

---

# 1. Zweck

Das **Nova Boot User Interface Framework (NBUI)** definiert die komplette grafische Benutzeroberfläche des Nova Bootloaders.

Es beschreibt sämtliche UI-Komponenten, deren Verhalten, Layout, Animationen und Interaktionen.

Das Framework bildet die Grundlage für

- Boot Manager
- Recovery
- Diagnose
- Installer
- Verschlüsselung
- Rettungsumgebung
- zukünftige grafische Boot-Anwendungen.

---

# 2. Ziele

Das UI Framework soll

- moderne Benutzeroberflächen ermöglichen
- vollständig widgetbasiert arbeiten
- vollständig DPI-unabhängig sein
- Animationen unterstützen
- Touch unterstützen
- Tastatur vollständig unterstützen
- Maus unterstützen
- Accessibility vorbereiten

---

# 3. Designprinzipien

Die Nova Boot UI orientiert sich an folgenden Grundprinzipien.

## Modern

Klare Flächen

große Schaltflächen

runde Ecken

Glass Design

Animationen

---

## Schnell

Die Oberfläche darf den Bootvorgang nicht verzögern.

UI-Operationen müssen deterministisch erfolgen.

---

## Konsistent

Alle Dialoge

Fenster

Listen

Buttons

verwenden dieselben Regeln.

---

## Responsive

Die Oberfläche funktioniert auf

800×600

bis

4K

ohne Anpassungen.

---

## Theme Driven

Kein Widget besitzt eigene Farben.

Alles stammt aus der Theme Engine.

---

# 4. Architektur

```text
Boot Core
↓
Boot UI Runtime
├── Window Manager
├── Widget Manager
├── Layout Engine
├── Navigation Engine
├── Dialog Manager
├── Animation Engine
├── Focus Manager
├── Accessibility
└── Theme Engine
```

---

# 5. Boot UI Runtime

Die Runtime verwaltet

- Fenster
- Dialoge
- Widgets
- Layout
- Navigation
- Animation
- Fokus
- Darstellung

---

# 6. UI Pipeline

```text
Widget Tree

↓

Layout

↓

Theme

↓

Animation

↓

Renderer

↓

Present
```

---

# 7. UI Manager

Der UI Manager koordiniert sämtliche Komponenten.

```text
Input

↓

Focus

↓

Layout

↓

Animation

↓

Rendering

↓

Present
```

---

# 8. Fensterkonzept

Der Bootloader verwendet ein vereinfachtes Fenstersystem.

Fenster können

- modal
- nicht modal
- Vollbild
- Dialog
- Overlay

sein.

---

# 9. Fensterobjekt

```c
typedef struct
{
    uint64_t id;

    char title[64];

    nova_rect_t bounds;

    bool modal;

    bool visible;

    bool enabled;

} nova_window_t;
```

---

# 10. Window Manager

Der Window Manager verwaltet

- Reihenfolge
- Sichtbarkeit
- Fokus
- Z-Order
- Lebensdauer

---

# 11. Fensterzustände

```text
Hidden

↓

Opening

↓

Visible

↓

Focused

↓

Disabled

↓

Closing
```

---

# 12. Widgetsystem

Jedes sichtbare Element ist ein Widget.

Es existieren keine Sonderfälle.

---

# 13. Standard Widgets

Version 1 definiert

- Label
- Button
- Image
- Icon
- ProgressBar
- CheckBox
- Switch
- TextBox
- PasswordBox
- ListView
- TreeView
- ScrollView
- Slider
- ComboBox
- Menu
- Dialog
- Window

---

# 14. Widget Eigenschaften

Alle Widgets besitzen

- Position
- Größe
- Padding
- Margin
- Sichtbarkeit
- Aktiviert
- Fokus
- Material
- Theme
- Animation

---

# 15. Widget Basisobjekt

```c
typedef struct
{
    uint64_t id;

    nova_widget_type_t type;

    nova_rect_t bounds;

    bool visible;

    bool enabled;

    bool focused;

    bool hovered;

} nova_widget_base_t;
```

---

# 16. Lebenszyklus

```text
Create

↓

Initialize

↓

Layout

↓

Render

↓

Update

↓

Destroy
```

---

# 17. Anforderungen

Das Boot UI Framework MUSS

- vollständig widgetbasiert sein
- Fenster verwalten
- Layout unterstützen
- Theme Driven arbeiten
- deterministisch sein

---

# 18. SHOULD

Das Framework SOLLTE

- Touch unterstützen
- Accessibility vorbereiten
- mehrere Dialoge verwalten
- zukünftige Erweiterungen erlauben

---

# 19. MAY

Das Framework DARF

- OEM Widgets
- Plugin Widgets
- experimentelle Controls
- zukünftige Eingabegeräte

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Boot UI Framework trennt **Benutzeroberfläche**, **Logik**, **Layout**, **Animation** und **Rendering** vollständig voneinander.

Widgets beschreiben ausschließlich ihren Zustand und ihre Eigenschaften.

Die eigentliche Darstellung erfolgt ausschließlich über:

- Theme Engine
- Layout Engine
- Graphics Framework
- Animation Engine

Dadurch bleibt die gesamte Oberfläche modular, konsistent und langfristig wartbar.

---

# 20. Window Manager

Der **Nova Window Manager (NWM)** verwaltet sämtliche Fenster des Bootloaders.

Er ist verantwortlich für

- Fenstererstellung
- Z-Order
- Fokus
- Lebenszyklus
- Animationen
- Sichtbarkeit
- Eingabeverteilung

Es existiert genau eine Instanz des Window Managers.

---

# 21. Ziele

Der Window Manager soll

- mehrere Fenster verwalten
- Modalität unterstützen
- Overlays unterstützen
- Animationen koordinieren
- Fokus verwalten
- Eingaben verteilen
- Speicher effizient verwalten

---

# 22. Architektur

```text
Boot UI Runtime
↓
Window Manager
├── Desktop Root
├── Window Stack
├── Dialog Manager
├── Overlay Manager
├── Focus Manager
├── Animation Controller
└── Event Router
```

---

# 23. Desktop Root

Alle Fenster besitzen denselben Wurzelknoten.

```text
Desktop Root
├── Background
├── Boot Manager
├── Recovery
├── Dialoge
├── Popups
└── Cursor
```

Der Desktop Root selbst ist kein sichtbares Fenster.

---

# 24. Window Stack

Fenster werden in einem Stack verwaltet.

```text
Top

↓

Popup

↓

Dialog

↓

Fenster

↓

Desktop
```

Das oberste sichtbare Fenster erhält standardmäßig den Fokus.

---

# 25. Z-Order

Die Zeichenreihenfolge wird ausschließlich über die Z-Order bestimmt.

```text
0   Desktop

10  Fenster

20  Dialog

30  Popup

40  Tooltip

50  Cursor
```

Eigene Z-Werte außerhalb dieses Schemas sind nicht zulässig.

---

# 26. Window Objekt

```c
typedef struct
{
    uint64_t id;

    char title[64];

    nova_rect_t bounds;

    uint32_t z_order;

    bool visible;

    bool modal;

    bool active;

    bool focused;

} nova_window_t;
```

---

# 27. Fensterzustände

```text
Created

↓

Hidden

↓

Opening

↓

Visible

↓

Focused

↓

Inactive

↓

Closing

↓

Destroyed
```

Alle Übergänge erfolgen über den Window Manager.

---

# 28. Sichtbarkeit

Ein Fenster kann

- sichtbar
- verborgen
- minimiert (vorbereitet)
- geschlossen

sein.

Version 1 unterstützt keine klassische Minimierung.

---

# 29. Aktivierung

Die Aktivierung eines Fensters erfolgt durch

- Maus
- Tastatur
- API
- Navigation

Es kann immer nur ein aktives Hauptfenster geben.

---

# 30. Modale Fenster

Ein modales Fenster blockiert sämtliche Eingaben außerhalb seines Bereichs.

Typische Beispiele:

- Passwortabfrage
- Datenträgerformatierung
- Sicherheitswarnungen
- Wiederherstellungsdialoge

---

# 31. Dialoge

Dialoge sind spezialisierte Fenster.

Eigenschaften

- modal oder nicht modal
- feste Größe
- definierte Schaltflächen
- automatische Zentrierung

---

# 32. Dialogtypen

Version 1 unterstützt

- Information
- Warnung
- Fehler
- Bestätigung
- Passwort
- Fortschritt
- Auswahl

---

# 33. Overlay

Overlays liegen oberhalb aller normalen Fenster.

Beispiele

- Ladeanimation
- Bildschirmabdunklung
- Sperrbildschirm
- Notfallhinweise

Overlays besitzen keine klassische Fensterdekoration.

---

# 34. Fensterdekoration

Version 1 definiert

- Titelbereich
- Symbol
- Schließen-Schaltfläche (optional)
- Schatten
- Glasmaterial

Minimieren und Maximieren sind im Bootloader nicht vorgesehen.

---

# 35. Fokusverwaltung

Der Focus Manager verwaltet

- Tastaturfokus
- Mausfokus
- Standardfokus
- Rückkehrfokus

Der Fokus wird niemals direkt durch Widgets geändert.

---

# 36. Fokusregeln

Es gilt:

- Genau ein Widget besitzt den Tastaturfokus.
- Genau ein Fenster ist aktiv.
- Ein modales Fenster besitzt immer den Fokus.
- Versteckte Fenster können keinen Fokus erhalten.

---

# 37. Navigation

Die Standardnavigation erfolgt über

- Tab
- Shift + Tab
- Pfeiltasten
- Enter
- Escape

Optional zusätzlich

- Maus
- Touch

---

# 38. Standardaktionen

| Taste | Funktion |
|--------|----------|
| Tab | Nächstes Widget |
| Shift+Tab | Vorheriges Widget |
| Enter | Aktivieren |
| Escape | Dialog schließen oder zurück |
| Pfeiltasten | Navigation |

---

# 39. Fensteranimationen

Beim Öffnen

- Fade In
- Scale In

Beim Schließen

- Fade Out
- Scale Out

Animationen dürfen jederzeit deaktiviert werden.

---

# 40. Fensterverwaltung

Der Window Manager stellt folgende Funktionen bereit:

```c
nova_window_t*
nova_window_create(void);

void
nova_window_show(
    nova_window_t* window);

void
nova_window_hide(
    nova_window_t* window);

void
nova_window_destroy(
    nova_window_t* window);
```

---

# 41. Fensterereignisse

Ein Fenster kann folgende Ereignisse empfangen:

- OnCreate
- OnShow
- OnHide
- OnFocus
- OnBlur
- OnResize
- OnClose
- OnDestroy

Die Ereignisse werden über den Event Dispatcher ausgelöst.

---

# 42. Größenänderung

Version 1 unterstützt keine frei skalierbaren Fenster.

Fenster besitzen

- feste Größe
- automatische Skalierung
- DPI-Anpassung

Die API bleibt für spätere Größenänderungen vorbereitet.

---

# 43. Fehlerbehandlung

Mögliche Fehler:

- ungültiges Fenster
- doppelte ID
- fehlender Speicher
- ungültige Z-Order
- Fokuskonflikte

Alle Fehler werden protokolliert und an das Boot-Diagnosesystem gemeldet.

---

# 44. Anforderungen

Der Window Manager MUSS

- Fenster verwalten
- Fokus koordinieren
- Z-Order einhalten
- modale Fenster unterstützen
- Lebenszyklen verwalten

---

# 45. SHOULD

Der Window Manager SOLLTE

- flüssige Animationen verwenden
- Overlays unterstützen
- Eingaben priorisieren
- spätere Fenstergrößenänderungen vorbereiten

---

# 46. MAY

Der Window Manager DARF

- Docking vorbereiten
- Snap Layouts vorbereiten
- Mehrmonitorbetrieb vorbereiten
- Fenstergruppen unterstützen

ohne die öffentliche API zu verändern.

---

# Architekturentscheidung (ADR)

Der Nova Window Manager verwaltet **ausschließlich den logischen Zustand** aller Fenster.

Das eigentliche Zeichnen erfolgt weiterhin ausschließlich über das **Nova Boot Graphics Framework**, während Animationen durch die **Animation Engine** und Eingaben durch das **Boot Input Framework** verarbeitet werden.

Dadurch bleibt die Fensterverwaltung vollständig von Rendering, Eingabe und Layout entkoppelt und kann später sowohl im Bootloader als auch im Kernel-Desktop wiederverwendet werden.

---

# 47. Layout Engine

Die **Nova Layout Engine (NLE)** berechnet automatisch die Position und Größe sämtlicher Widgets.

Kein Widget darf seine Position selbst bestimmen.

Die Layout Engine garantiert dadurch ein konsistentes Erscheinungsbild auf allen Auflösungen.

---

# 48. Ziele

Die Layout Engine soll

- automatische Größenberechnung durchführen
- DPI-unabhängig arbeiten
- responsive Layouts unterstützen
- Container verschachteln können
- Layouts deterministisch berechnen
- zukünftige Layoutsysteme ermöglichen

---

# 49. Architektur

```text
Widget Tree
↓
Layout Engine
├── Constraint Solver
├── Flex Layout
├── Grid Layout
├── Stack Layout
├── Dock Layout
├── Responsive Engine
├── DPI Scaler
└── Layout Cache
```

---

# 50. Layout Pipeline

```text
Widget Tree

↓

Measure

↓

Constraints

↓

Layout

↓

Arrange

↓

Render Tree
```

Jeder Schritt muss vollständig abgeschlossen sein, bevor der nächste beginnt.

---

# 51. Layout Objekt

```c
typedef struct
{
    nova_rect_t bounds;

    nova_margin_t margin;

    nova_padding_t padding;

    nova_alignment_t alignment;

    bool visible;

} nova_layout_t;
```

---

# 52. Layout Container

Container dienen ausschließlich der Anordnung ihrer Kind-Widgets.

Container besitzen selbst keine Anwendungslogik.

Version 1 unterstützt:

- Panel
- StackPanel
- Grid
- FlexContainer
- ScrollContainer
- OverlayContainer

---

# 53. Margin

Der Außenabstand eines Widgets.

```text
┌───────────────────────────┐
│ Margin                    │
│ ┌───────────────────────┐ │
│ │ Widget                │ │
│ └───────────────────────┘ │
└───────────────────────────┘
```

---

# 54. Padding

Der Innenabstand eines Widgets.

```text
┌───────────────────────────┐
│ Margin                    │
│ ┌───────────────────────┐ │
│ │ Padding               │ │
│ │ Widget                │ │
│ │ ┌───────────────────┐ │ │
│ │ │ Inhalt            │ │ │ 
│ │ └───────────────────┘ │ │ 
│ └───────────────────────┘ │
└───────────────────────────┘
```

---

# 55. Alignment

Unterstützt

```text
Start

Center

End

Stretch
```

Horizontal und vertikal unabhängig.

---

# 56. Größenmodell

Widgets besitzen

```text
Minimum

↓

Preferred

↓

Maximum
```

Die endgültige Größe berechnet die Layout Engine.

---

# 57. Flex Layout

Flex Layout eignet sich für dynamische Oberflächen.

Unterstützt

- Row
- Column
- Wrap
- Gap
- Grow
- Shrink
- Alignment

---

# 58. Flex Eigenschaften

```c
typedef struct
{
    float grow;

    float shrink;

    float basis;

} nova_flex_properties_t;
```

---

# 59. Grid Layout

Widgets werden in Zeilen und Spalten angeordnet.

```text
┌────┬────┬────┐
│    │    │    │
├────┼────┼────┤
│    │    │    │
├────┼────┼────┤
│    │    │    │
└────┴────┴────┘
```

Unterstützt

- feste Größen
- automatische Größen
- prozentuale Größen

---

# 60. Stack Layout

Unterstützt

```text
Vertical

Horizontal
```

Alle Elemente werden hintereinander angeordnet.

---

# 61. Dock Layout

Widgets können angedockt werden.

```text
Top

Bottom

Left

Right

Fill
```

Ideal für Boot-Menüs und Werkzeugleisten.

---

# 62. Overlay Layout

Mehrere Widgets können dieselbe Fläche belegen.

Beispiele

- Glasüberlagerungen
- Ladeanzeigen
- Cursor
- Dialoge

---

# 63. Scroll Container

Version 1 unterstützt

- vertikales Scrollen
- horizontales Scrollen
- automatische Scrollbars

---

# 64. Responsive Layout

Die Oberfläche passt sich automatisch an

- Bildschirmauflösung
- DPI
- Seitenverhältnis
- Fenstergröße

an.

---

# 65. Breakpoints

Empfohlene Klassen

```text
Small

800 px

↓

Medium

1280 px

↓

Large

1920 px

↓

Ultra

2560 px+
```

---

# 66. DPI Scaling

Alle Maße werden zunächst in logischen Einheiten definiert.

```text
Logical Units

↓

DPI Scale

↓

Pixel
```

Dadurch bleibt die Bedienoberfläche unabhängig von der Pixeldichte.

---

# 67. Safe Area

Die Layout Engine berücksichtigt Sicherheitsabstände.

Verhindert

- abgeschnittene Inhalte
- überlappende Widgets
- Randprobleme

---

# 68. Layout Cache

Berechnete Layouts können zwischengespeichert werden.

```text
Widget Tree

↓

Hash

↓

Cache

↓

Reuse
```

Dadurch werden unnötige Neuberechnungen vermieden.

---

# 69. Invalidierung

Nur geänderte Bereiche werden neu berechnet.

```text
Widget geändert

↓

Invalidate

↓

Layout neu

↓

Render
```

---

# 70. Z-Layout

Die Layout Engine bestimmt ausschließlich Position und Größe.

Die Zeichenreihenfolge wird weiterhin ausschließlich über die Z-Order des Window Managers festgelegt.

---

# 71. Layout API

```c
void
nova_layout_measure(
    nova_widget_t* root);

void
nova_layout_arrange(
    nova_widget_t* root);

void
nova_layout_update(
    nova_widget_t* root);
```

---

# 72. Fehlerbehandlung

Mögliche Fehler

- ungültige Größen
- zyklische Container
- negative Abmessungen
- ungültige Constraints
- Layout Overflow

Fehler werden protokolliert und führen niemals zu undefiniertem Verhalten.

---

# 73. Anforderungen

Die Layout Engine MUSS

- deterministisch arbeiten
- Flex Layout unterstützen
- Grid Layout unterstützen
- DPI-Skalierung unterstützen
- Container verschachteln können

---

# 74. SHOULD

Die Layout Engine SOLLTE

- Layout Cache verwenden
- Responsive Layout unterstützen
- Invalidierung unterstützen
- hohe Performance erreichen

---

# 75. MAY

Die Layout Engine DARF

- Constraint Layout
- Masonry Layout
- Auto Layout
- virtuelle Layouts

unterstützen, sofern die öffentliche API unverändert bleibt.

---

# Architekturentscheidung (ADR)

Die Nova Layout Engine basiert auf einem **zweistufigen Measure-/Arrange-Modell**.

Widgets beschreiben ausschließlich ihre gewünschten Eigenschaften (Mindestgröße, bevorzugte Größe, maximale Größe und Layoutparameter).

Die endgültige Positionierung erfolgt zentral durch die Layout Engine.

Dadurch entstehen:

- reproduzierbare Layouts
- konsistente Darstellung
- einfache DPI-Anpassung
- hohe Wiederverwendbarkeit
- vollständige Trennung zwischen Layout und Rendering

---

# 76. Standard Widget Library

Die **Nova Standard Widget Library (NSWL)** definiert sämtliche Standard-Widgets des Bootloaders.

Alle Widgets bauen auf derselben Basisklasse auf und werden vollständig über die Theme Engine gestaltet.

Eigene Zeichenlogik innerhalb der Widgets ist nicht zulässig.

---

# 77. Ziele

Die Widget Library soll

- konsistente Widgets bereitstellen
- vollständig Theme-gesteuert arbeiten
- Animationen unterstützen
- DPI-unabhängig sein
- Touch unterstützen
- Tastatur unterstützen
- Accessibility vorbereiten

---

# 78. Widget Hierarchie

```text
Widget
├── Container
├── Label
├── Button
├── TextBox
├── PasswordBox
├── Image
├── Icon
├── CheckBox
├── Switch
├── Slider
├── ProgressBar
├── ListView
├── TreeView
├── ScrollView
├── Menu
├── Dialog
└── Window
```

---

# 79. Widget Basis

Alle Widgets erben dieselben Eigenschaften.

```c
typedef struct
{
    uint64_t id;

    nova_rect_t bounds;

    bool visible;

    bool enabled;

    bool focused;

    bool hovered;

    bool pressed;

    nova_theme_t* theme;

} nova_widget_base_t;
```

---

# 80. Widget Zustände

Jedes Widget besitzt dieselben Zustände.

```text
Normal

↓

Hover

↓

Focused

↓

Pressed

↓

Disabled
```

Optional

```text
Selected

Checked

Loading

Invalid
```

---

# 81. Label

Das Label stellt ausschließlich Text dar.

Unterstützt

- UTF-8
- Unicode
- Mehrzeilig
- Zeilenumbruch
- Ellipsis
- Alignment

Labels besitzen keine Benutzereingaben.

---

# 82. Label API

```c
nova_label_t*
nova_label_create(void);

void
nova_label_set_text(
    nova_label_t* label,
    const char* text);
```

---

# 83. Button

Buttons lösen Aktionen aus.

Bestandteile

- Icon
- Text
- Hintergrund
- Material
- Schatten
- Fokusrahmen

---

# 84. Button Animationen

Unterstützt

- Hover Fade

- Press Animation

- Focus Glow

- Ripple (optional)

---

# 85. Button API

```c
nova_button_t*
nova_button_create(void);

void
nova_button_set_text(
    nova_button_t* button,
    const char* text);

void
nova_button_set_icon(
    nova_button_t* button,
    nova_icon_t* icon);
```

---

# 86. TextBox

Die TextBox dient zur Eingabe von Text.

Unterstützt

- Cursor
- Auswahl
- Zwischenablage (vorbereitet)
- UTF-8
- Undo (vorbereitet)

---

# 87. PasswordBox

Die PasswordBox basiert auf der TextBox.

Zusätzlich

- Zeichenmaskierung
- Passwortstärke (optional)
- Ein-/Ausblenden

Geeignet für

- BitLocker
- LUKS
- Boot-Passwort
- Recovery-Schlüssel

---

# 88. CheckBox

Unterstützt

```text
Unchecked

↓

Checked

↓

Indeterminate
```

---

# 89. Switch

Der Switch dient zum Ein-/Ausschalten von Optionen.

Animation

```text
OFF

↓

Slide

↓

ON
```

---

# 90. ProgressBar

Version 1

- Linear

- Circular

Unterstützt

- determiniert
- indeterminiert

---

# 91. Slider

Eigenschaften

- Minimum
- Maximum
- Schrittweite
- aktueller Wert

Anwendungsbeispiele

- Lautstärke
- Helligkeit
- Timeout
- Lüftertest

---

# 92. Image Widget

Unterstützt

- PNG
- SVG
- BMP

Eigenschaften

- Skalierung
- Zentrierung
- Rotation
- Alpha

---

# 93. Icon Widget

Icons basieren ausschließlich auf SVG.

Vorteile

- beliebige Skalierung
- kleine Dateigröße
- Theme-Farben
- Retina geeignet

---

# 94. ScrollView

Unterstützt

- vertikal
- horizontal
- automatische Scrollbars
- Touch Scrolling (vorbereitet)

---

# 95. ListView

Die ListView stellt Listen dar.

Eigenschaften

- Auswahl
- Fokus
- Icons
- Mehrspaltig (vorbereitet)

Beispiele

- Bootgeräte
- Partitionen
- Logs
- Diagnosen

---

# 96. TreeView

Hierarchische Darstellung.

Verwendung

- EFI Partitionen
- Recovery Daten
- Dateisystem
- Bootmodule

---

# 97. Menu

Version 1 unterstützt

- Menüleiste
- Kontextmenü
- Popup-Menü

---

# 98. Dialog Buttons

Standardisierte Schaltflächen

```text
OK

Abbrechen

Ja

Nein

Weiter

Zurück

Fertig

Wiederholen
```

Alle besitzen dieselben Größen.

---

# 99. Widget Ereignisse

Alle Widgets unterstützen

```text
OnCreate

OnFocus

OnBlur

OnHover

OnPressed

OnReleased

OnClick

OnDestroy
```

---

# 100. Widget Styling

Widgets besitzen keine eigenen Farben.

Alle Eigenschaften stammen aus

- Theme
- Material
- Typography
- Animation Tokens

---

# 101. Accessibility

Vorbereitet

- Screen Reader
- große Schrift
- hoher Kontrast
- Tastaturnavigation
- Fokusrahmen

---

# 102. Widget API

```c
nova_widget_t*
nova_widget_create(
    nova_widget_type_t type);

void
nova_widget_destroy(
    nova_widget_t* widget);

void
nova_widget_set_visible(
    nova_widget_t* widget,
    bool visible);

void
nova_widget_set_enabled(
    nova_widget_t* widget,
    bool enabled);
```

---

# 103. Anforderungen

Die Widget Library MUSS

- Theme Driven arbeiten
- konsistente Zustände besitzen
- Animationen unterstützen
- DPI-unabhängig sein
- standardisierte APIs besitzen

---

# 104. SHOULD

Die Widget Library SOLLTE

- Accessibility vorbereiten
- Touch unterstützen
- SVG-Icons verwenden
- moderne Animationen besitzen

---

# 105. MAY

Die Widget Library DARF

- OEM Widgets
- Plugin Widgets
- experimentelle Controls
- zukünftige UI-Komponenten

unterstützen.

---

# Architekturentscheidung (ADR)

Die Nova Widget Library definiert **ausschließlich das Verhalten und die Eigenschaften** der Widgets.

Die Darstellung erfolgt vollständig über:

- Theme Engine
- Graphics Framework
- Layout Engine
- Animation Engine

Dadurch besitzen alle Widgets ein identisches Verhalten, eine konsistente Optik und können ohne Änderungen am Rendering erweitert oder ersetzt werden.

---

# 106. Erweiterte Widget Library

Die **Erweiterte Widget Library (Advanced Widget Library)** ergänzt die Standard-Widgets um komplexe UI-Komponenten.

Sie bildet die Grundlage für

- Boot Manager
- Recovery
- Dateimanager
- Installer
- Diagnosewerkzeuge
- Partitionierungswerkzeuge

Alle erweiterten Widgets bauen ausschließlich auf Standard-Widgets auf.

---

# 107. Ziele

Die Advanced Widgets sollen

- komplexe Daten darstellen
- große Datenmengen verwalten
- Navigation vereinfachen
- konsistente Bedienung ermöglichen
- Theme Driven arbeiten
- Animationen unterstützen

---

# 108. Architektur

```text
Advanced Widgets
├── Card
├── Table
├── Tree
├── NavigationView
├── Tabs
├── Breadcrumb
├── Accordion
├── File Browser
├── Dashboard
└── Wizard
```

---

# 109. Card Widget

Die **Card** bildet den grundlegenden Informationscontainer.

Sie besteht aus

```text
Titel

↓

Untertitel

↓

Inhalt

↓

Aktionen
```

---

# 110. Eigenschaften

Cards besitzen

- Glass Material
- Shadow
- Radius
- Padding
- Header
- Footer

Cards dürfen beliebige Widgets enthalten.

---

# 111. Verwendung

Cards werden eingesetzt für

- Bootgeräte
- Partitionen
- Betriebssysteme
- Wiederherstellungspunkte
- Diagnosen
- Systeminformationen

---

# 112. Card API

```c
nova_card_t*
nova_card_create(void);

void
nova_card_set_title(
    nova_card_t* card,
    const char* title);

void
nova_card_set_subtitle(
    nova_card_t* card,
    const char* subtitle);
```

---

# 113. Table Widget

Die Tabelle dient zur Darstellung strukturierter Daten.

Unterstützt

- Kopfzeilen
- Zeilen
- Spalten
- Sortierung (vorbereitet)
- Auswahl
- Icons

---

# 114. Tabellenstruktur

```text
┌────────┬─────────┬─────────┐
│ Name   │ Größe   │ Status  │
├────────┼─────────┼─────────┤
│ EFI    │ 512 MB  │ OK      │
└────────┴─────────┴─────────┘
```

---

# 115. Tree Widget

Der Tree stellt hierarchische Daten dar.

Beispiele

```text
Disk
├── EFI
├── Recovery
└── Windows
```

---

# 116. Tree Eigenschaften

Unterstützt

- Aufklappen
- Zuklappen
- Icons
- Auswahl
- Fokus

---

# 117. Navigation View

Die Navigation View bildet das Hauptmenü.

```text
│ Boot
│ Recovery
│ Diagnose
│ Sicherheit
│ Einstellungen
│ Info
```

Version 1 unterstützt

- vertikale Navigation

Vorbereitet

- horizontale Navigation

---

# 118. Navigation API

```c
nova_navigation_view_t*
nova_navigation_create(void);
```

---

# 119. Tabs

Tabs dienen zum Umschalten zwischen Bereichen.

```text
┌──────────────────────────────┐
│ Boot │ Diagnose │ Recovery   │
└──────────────────────────────┘
```

---

# 120. Tab Eigenschaften

Unterstützt

- Icon
- Text
- Animation
- Fokus

Vorbereitet

- Schließbare Tabs

---

# 121. Breadcrumb

Breadcrumbs zeigen den aktuellen Navigationspfad.

```text
Recovery

>

Festplatte

>

EFI

>

Logs
```

---

# 122. Accordion

Accordion-Elemente gruppieren Inhalte.

```text
▼ Speicher

▼ Netzwerk

▶ Sicherheit
```

Nur geöffnete Bereiche werden dargestellt.

---

# 123. Dashboard Widget

Das Dashboard besteht aus mehreren Cards.

Beispiele

```text
CPU

RAM

Festplatte

Bootstatus

Temperatur

Diagnose
```

---

# 124. Status Widget

Ein Status Widget visualisiert

- Erfolg
- Warnung
- Fehler
- Aktivität
- Offline

Es verwendet ausschließlich Theme-Farben.

---

# 125. Boot Device Widget

Spezialwidget zur Darstellung bootfähiger Systeme.

Beispiel

```text
🖥 Nova OS

SSD

UEFI

Priorität 1
```

---

# 126. File Browser

Der File Browser dient ausschließlich zur Dateiauswahl.

Unterstützt

- Verzeichnisse
- Laufwerke
- Icons
- Größen
- Änderungsdatum

Nicht unterstützt

- Dateibearbeitung

---

# 127. File Picker

Der File Picker dient zur Auswahl

- Kernel
- Initramfs
- Bootlogos
- Konfigurationsdateien
- Recovery Images

---

# 128. Wizard

Ein Wizard führt Benutzer Schritt für Schritt.

Beispiel

```text
Willkommen

↓

Datenträger

↓

Partition

↓

Bestätigung

↓

Fertig
```

---

# 129. Stepper

Der Stepper visualisiert Fortschritte.

```text
①

↓

②

↓

③

↓

④
```

---

# 130. Search Box

Vorbereitet

- lokale Suche
- Partitionen
- Bootgeräte
- Logs

Version 1 besitzt keine Volltextsuche.

---

# 131. Notification Panel

Version 1 unterstützt einfache Benachrichtigungen.

Beispiele

- Boot erfolgreich
- Recovery abgeschlossen
- Datenträger erkannt
- Passwort falsch

---

# 132. Toast

Kurzzeitige Hinweise.

```text
✓ Einstellungen gespeichert
```

Automatisches Ausblenden.

---

# 133. Command Palette

Vorbereitet

```text
Ctrl+P
```

Direkter Zugriff auf Bootfunktionen.

Nicht Bestandteil von Version 1.

---

# 134. Widget Factory

Alle Widgets werden über eine Factory erzeugt.

```c
nova_widget_t*
nova_widget_factory_create(
    nova_widget_type_t type);
```

---

# 135. Anforderungen

Die erweiterten Widgets MUSSEN

- Theme Driven arbeiten
- Standard-Widgets verwenden
- Animationen unterstützen
- Layout Engine verwenden
- Accessibility vorbereiten

---

# 136. SHOULD

Die erweiterten Widgets SOLLTEN

- große Datenmengen darstellen
- Icons unterstützen
- Tastaturbedienung besitzen
- Touch vorbereiten

---

# 137. MAY

Die erweiterten Widgets DÜRFEN

- Diagramme
- Kalender
- Zeitachsen
- OEM Widgets
- Plugin Widgets

unterstützen.

---

# Architekturentscheidung (ADR)

Erweiterte Widgets bestehen ausschließlich aus einer Kombination vorhandener Standard-Widgets.

Es werden **keine separaten Rendering-Komponenten** eingeführt.

Dadurch bleiben sämtliche UI-Komponenten:

- konsistent
- leicht wartbar
- vollständig Theme Driven
- über die Layout Engine steuerbar
- kompatibel mit zukünftigen Erweiterungen

---

# 138. UI Event Framework

Das **Nova UI Event Framework (NUEF)** verarbeitet sämtliche Benutzereingaben innerhalb des Bootloaders.

Es stellt eine einheitliche Ereignisschnittstelle für

- Maus
- Tastatur
- Touch
- Controller (zukünftig)
- Accessibility-Geräte (zukünftig)

bereit.

Kein Widget greift direkt auf Hardware-Eingaben zu.

---

# 139. Ziele

Das Event Framework soll

- Eingaben vereinheitlichen
- Ereignisse priorisieren
- Fokus berücksichtigen
- deterministisch arbeiten
- Plattformunabhängig sein
- zukünftige Eingabegeräte unterstützen

---

# 140. Architektur

```text
Input Runtime
↓
Input Events
↓
Event Dispatcher
├── Focus Manager
├── Hit Test
├── Command Engine
├── Action Engine
├── Widget Events
└── Navigation Engine
```

---

# 141. Event Pipeline

```text
Hardware

↓

Input Runtime

↓

Raw Event

↓

Event Dispatcher

↓

Widget Event

↓

Action

↓

Render
```

---

# 142. Event Objekt

```c
typedef struct
{
    uint64_t timestamp;

    nova_event_type_t type;

    uint64_t target;

    uint64_t source;

    uint32_t flags;

} nova_event_t;
```

---

# 143. Event Typen

```c
typedef enum
{
    NOVA_EVENT_NONE,

    NOVA_EVENT_MOUSE,

    NOVA_EVENT_KEYBOARD,

    NOVA_EVENT_TOUCH,

    NOVA_EVENT_WINDOW,

    NOVA_EVENT_WIDGET,

    NOVA_EVENT_COMMAND,

    NOVA_EVENT_SYSTEM

} nova_event_type_t;
```

---

# 144. Mausereignisse

Unterstützt

- Move
- Enter
- Leave
- Button Down
- Button Up
- Double Click
- Wheel

---

# 145. Tastaturereignisse

Unterstützt

- Key Down
- Key Up
- Character Input
- Modifier
- Auto Repeat

Alle Zeichen werden intern als Unicode verarbeitet.

---

# 146. Touch

Version 1 vorbereitet

Unterstützt zukünftig

- Tap
- Double Tap
- Long Press
- Swipe
- Drag
- Pinch
- Zoom

---

# 147. Fensterereignisse

Fenster erhalten

```text
Create

↓

Show

↓

Hide

↓

Focus

↓

Blur

↓

Resize

↓

Close

↓

Destroy
```

---

# 148. Widget Ereignisse

Alle Widgets unterstützen

```text
OnCreate

OnHover

OnEnter

OnLeave

OnPressed

OnReleased

OnClick

OnFocus

OnBlur

OnDestroy
```

---

# 149. Ereignispriorität

```text
Critical

↓

High

↓

Normal

↓

Background
```

Bootkritische Ereignisse besitzen höchste Priorität.

---

# 150. Event Queue

Alle Ereignisse werden zunächst in einer Queue gespeichert.

```text
Input

↓

Queue

↓

Dispatcher

↓

Widget
```

Die Queue arbeitet nach dem FIFO-Prinzip.

---

# 151. Event Routing

Der Dispatcher bestimmt automatisch den Empfänger.

```text
Event

↓

Hit Test

↓

Focus

↓

Target Widget
```

---

# 152. Hit Testing

Vor jedem Ereignis wird geprüft

- sichtbares Widget
- aktiviertes Widget
- Fokus
- Z-Order
- Mausposition

Nur das gültige Ziel erhält das Ereignis.

---

# 153. Event Bubbling

Version 1 unterstützt Event Bubbling.

```text
Button

↓

Panel

↓

Window

↓

Desktop
```

Nicht behandelte Ereignisse steigen im Widgetbaum nach oben.

---

# 154. Event Capturing

Vorbereitet

```text
Desktop

↓

Window

↓

Panel

↓

Widget
```

Kann später für globale Eingaben verwendet werden.

---

# 155. Fokuswechsel

Der Fokus kann wechseln durch

- Maus
- Tab
- API
- Navigation

Der Focus Manager verhindert Mehrfachfokus.

---

# 156. Commands

Widgets erzeugen keine Aktionen direkt.

Sie erzeugen ausschließlich Commands.

```text
Button

↓

Command

↓

Application Logic
```

---

# 157. Command Objekt

```c
typedef struct
{
    uint64_t id;

    char name[64];

    void* parameter;

} nova_command_t;
```

---

# 158. Action Engine

Commands werden in Aktionen umgesetzt.

Beispiele

- Boot starten
- Recovery öffnen
- Datenträger scannen
- Passwort prüfen
- Einstellungen speichern

---

# 159. Shortcut Manager

Version 1 unterstützt

| Tastenkombination | Funktion |
|-------------------|----------|
| Enter | Aktivieren |
| Escape | Zurück |
| Tab | Nächstes Widget |
| Shift+Tab | Vorheriges Widget |
| Pfeiltasten | Navigation |
| F1 | Hilfe (optional) |
| F5 | Aktualisieren |
| F10 | Boot-Menü (optional) |

---

# 160. Navigation Engine

Die Navigation Engine verarbeitet

- Tastatur
- Controller (vorbereitet)
- Touch (vorbereitet)

Sie kennt ausschließlich Widgets.

---

# 161. Event Filter

Vor dem Dispatch können Filter eingreifen.

Beispiele

- Eingaben sperren
- Modalität
- Sicherheitsabfragen
- Recovery-Modus

---

# 162. Ereignisprotokoll

Alle kritischen Ereignisse werden protokolliert.

Beispiele

- Boot gestartet
- Passwort eingegeben
- Recovery geöffnet
- Fehlerdialog bestätigt

---

# 163. Event API

```c
void
nova_event_dispatch(
    nova_event_t* event);

void
nova_event_post(
    nova_event_t* event);

void
nova_event_process(void);
```

---

# 164. Fehlerbehandlung

Mögliche Fehler

- ungültiges Widget
- fehlender Fokus
- ungültiges Ereignis
- Queue Overflow
- Dispatcher-Fehler

Diese Fehler werden an das Diagnosesystem gemeldet.

---

# 165. Anforderungen

Das Event Framework MUSS

- Ereignisse vereinheitlichen
- Event Queue besitzen
- Routing durchführen
- Fokus berücksichtigen
- deterministisch arbeiten

---

# 166. SHOULD

Das Event Framework SOLLTE

- Event Bubbling unterstützen
- Touch vorbereiten
- Commands verwenden
- Accessibility vorbereiten

---

# 167. MAY

Das Event Framework DARF

- Gamepads
- Sprachsteuerung
- Gestenerkennung
- Makros
- Benutzerdefinierte Events

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova UI Event Framework trennt **Hardware-Eingaben**, **UI-Ereignisse** und **Anwendungslogik** vollständig voneinander.

Widgets reagieren ausschließlich auf standardisierte UI-Ereignisse und erzeugen lediglich Commands.

Die eigentliche Ausführung erfolgt durch die Action Engine.

Dadurch bleibt das gesamte UI-System unabhängig von Eingabegeräten und kann sowohl im Bootloader als auch später im Nova-Desktop unverändert verwendet werden.

---

# 138. UI Event Framework

Das **Nova UI Event Framework (NUEF)** verarbeitet sämtliche Benutzereingaben innerhalb des Bootloaders.

Es stellt eine einheitliche Ereignisschnittstelle für

- Maus
- Tastatur
- Touch
- Controller (zukünftig)
- Accessibility-Geräte (zukünftig)

bereit.

Kein Widget greift direkt auf Hardware-Eingaben zu.

---

# 139. Ziele

Das Event Framework soll

- Eingaben vereinheitlichen
- Ereignisse priorisieren
- Fokus berücksichtigen
- deterministisch arbeiten
- Plattformunabhängig sein
- zukünftige Eingabegeräte unterstützen

---

# 140. Architektur

```text
Input Runtime
↓
Input Events
↓
Event Dispatcher
├── Focus Manager
├── Hit Test
├── Command Engine
├── Action Engine
├── Widget Events
└── Navigation Engine
```

---

# 141. Event Pipeline

```text
Hardware

↓

Input Runtime

↓

Raw Event

↓

Event Dispatcher

↓

Widget Event

↓

Action

↓

Render
```

---

# 142. Event Objekt

```c
typedef struct
{
    uint64_t timestamp;

    nova_event_type_t type;

    uint64_t target;

    uint64_t source;

    uint32_t flags;

} nova_event_t;
```

---

# 143. Event Typen

```c
typedef enum
{
    NOVA_EVENT_NONE,

    NOVA_EVENT_MOUSE,

    NOVA_EVENT_KEYBOARD,

    NOVA_EVENT_TOUCH,

    NOVA_EVENT_WINDOW,

    NOVA_EVENT_WIDGET,

    NOVA_EVENT_COMMAND,

    NOVA_EVENT_SYSTEM

} nova_event_type_t;
```

---

# 144. Mausereignisse

Unterstützt

- Move
- Enter
- Leave
- Button Down
- Button Up
- Double Click
- Wheel

---

# 145. Tastaturereignisse

Unterstützt

- Key Down
- Key Up
- Character Input
- Modifier
- Auto Repeat

Alle Zeichen werden intern als Unicode verarbeitet.

---

# 146. Touch

Version 1 vorbereitet

Unterstützt zukünftig

- Tap
- Double Tap
- Long Press
- Swipe
- Drag
- Pinch
- Zoom

---

# 147. Fensterereignisse

Fenster erhalten

```text
Create

↓

Show

↓

Hide

↓

Focus

↓

Blur

↓

Resize

↓

Close

↓

Destroy
```

---

# 148. Widget Ereignisse

Alle Widgets unterstützen

```text
OnCreate

OnHover

OnEnter

OnLeave

OnPressed

OnReleased

OnClick

OnFocus

OnBlur

OnDestroy
```

---

# 149. Ereignispriorität

```text
Critical

↓

High

↓

Normal

↓

Background
```

Bootkritische Ereignisse besitzen höchste Priorität.

---

# 150. Event Queue

Alle Ereignisse werden zunächst in einer Queue gespeichert.

```text
Input

↓

Queue

↓

Dispatcher

↓

Widget
```

Die Queue arbeitet nach dem FIFO-Prinzip.

---

# 151. Event Routing

Der Dispatcher bestimmt automatisch den Empfänger.

```text
Event

↓

Hit Test

↓

Focus

↓

Target Widget
```

---

# 152. Hit Testing

Vor jedem Ereignis wird geprüft

- sichtbares Widget
- aktiviertes Widget
- Fokus
- Z-Order
- Mausposition

Nur das gültige Ziel erhält das Ereignis.

---

# 153. Event Bubbling

Version 1 unterstützt Event Bubbling.

```text
Button

↓

Panel

↓

Window

↓

Desktop
```

Nicht behandelte Ereignisse steigen im Widgetbaum nach oben.

---

# 154. Event Capturing

Vorbereitet

```text
Desktop

↓

Window

↓

Panel

↓

Widget
```

Kann später für globale Eingaben verwendet werden.

---

# 155. Fokuswechsel

Der Fokus kann wechseln durch

- Maus
- Tab
- API
- Navigation

Der Focus Manager verhindert Mehrfachfokus.

---

# 156. Commands

Widgets erzeugen keine Aktionen direkt.

Sie erzeugen ausschließlich Commands.

```text
Button

↓

Command

↓

Application Logic
```

---

# 157. Command Objekt

```c
typedef struct
{
    uint64_t id;

    char name[64];

    void* parameter;

} nova_command_t;
```

---

# 158. Action Engine

Commands werden in Aktionen umgesetzt.

Beispiele

- Boot starten
- Recovery öffnen
- Datenträger scannen
- Passwort prüfen
- Einstellungen speichern

---

# 159. Shortcut Manager

Version 1 unterstützt

| Tastenkombination | Funktion |
|-------------------|----------|
| Enter | Aktivieren |
| Escape | Zurück |
| Tab | Nächstes Widget |
| Shift+Tab | Vorheriges Widget |
| Pfeiltasten | Navigation |
| F1 | Hilfe (optional) |
| F5 | Aktualisieren |
| F10 | Boot-Menü (optional) |

---

# 160. Navigation Engine

Die Navigation Engine verarbeitet

- Tastatur
- Controller (vorbereitet)
- Touch (vorbereitet)

Sie kennt ausschließlich Widgets.

---

# 161. Event Filter

Vor dem Dispatch können Filter eingreifen.

Beispiele

- Eingaben sperren
- Modalität
- Sicherheitsabfragen
- Recovery-Modus

---

# 162. Ereignisprotokoll

Alle kritischen Ereignisse werden protokolliert.

Beispiele

- Boot gestartet
- Passwort eingegeben
- Recovery geöffnet
- Fehlerdialog bestätigt

---

# 163. Event API

```c
void
nova_event_dispatch(
    nova_event_t* event);

void
nova_event_post(
    nova_event_t* event);

void
nova_event_process(void);
```

---

# 164. Fehlerbehandlung

Mögliche Fehler

- ungültiges Widget
- fehlender Fokus
- ungültiges Ereignis
- Queue Overflow
- Dispatcher-Fehler

Diese Fehler werden an das Diagnosesystem gemeldet.

---

# 165. Anforderungen

Das Event Framework MUSS

- Ereignisse vereinheitlichen
- Event Queue besitzen
- Routing durchführen
- Fokus berücksichtigen
- deterministisch arbeiten

---

# 166. SHOULD

Das Event Framework SOLLTE

- Event Bubbling unterstützen
- Touch vorbereiten
- Commands verwenden
- Accessibility vorbereiten

---

# 167. MAY

Das Event Framework DARF

- Gamepads
- Sprachsteuerung
- Gestenerkennung
- Makros
- Benutzerdefinierte Events

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova UI Event Framework trennt **Hardware-Eingaben**, **UI-Ereignisse** und **Anwendungslogik** vollständig voneinander.

Widgets reagieren ausschließlich auf standardisierte UI-Ereignisse und erzeugen lediglich Commands.

Die eigentliche Ausführung erfolgt durch die Action Engine.

Dadurch bleibt das gesamte UI-System unabhängig von Eingabegeräten und kann sowohl im Bootloader als auch später im Nova-Desktop unverändert verwendet werden.

---

# 197. Dialog Framework

Das **Nova Dialog Framework (NDF)** definiert sämtliche Dialoge des Bootloaders.

Dialoge dienen der Benutzerinteraktion bei

- Bestätigungen
- Warnungen
- Fehlermeldungen
- Kennworteingaben
- Fortschrittsanzeigen
- Dateiauswahl
- Sicherheitsabfragen

Alle Dialoge basieren auf dem Window Manager und der Widget Library.

---

# 198. Ziele

Das Dialog Framework soll

- konsistente Dialoge bereitstellen
- modale Abläufe unterstützen
- Animationen verwenden
- Accessibility vorbereiten
- Theme Driven arbeiten
- deterministisch sein

---

# 199. Architektur

```text
Boot UI Runtime
↓
Dialog Framework
├── Dialog Manager
├── MessageBox
├── Password Dialog
├── Progress Dialog
├── File Dialog
├── Notification Manager
├── Toast Manager
└── Overlay Manager
```

---

# 200. Dialogarten

Version 1 definiert

- Information
- Warnung
- Fehler
- Bestätigung
- Passwort
- Fortschritt
- Dateiauswahl
- Eingabedialog

Alle Dialogtypen besitzen dieselbe Grundstruktur.

---

# 201. Dialog Aufbau

```text
┌──────────────────────────────┐
│ Titel                        │
├──────────────────────────────┤
│ Symbol                       │
│                              │
│ Nachricht                    │
│                              │
├──────────────────────────────┤
│ Schaltflächen                │
└──────────────────────────────┘
```

---

# 202. Dialog Objekt

```c
typedef struct
{
    uint64_t id;

    char title[64];

    char message[512];

    nova_dialog_type_t type;

    bool modal;

    bool visible;

} nova_dialog_t;
```

---

# 203. MessageBox

Die MessageBox dient zur Anzeige einfacher Informationen.

Unterstützt

- Information
- Warnung
- Fehler
- Frage

---

# 204. Standardsymbole

Version 1 definiert

- Information
- Warnung
- Fehler
- Erfolg
- Frage
- Sicherheit

Alle Symbole stammen aus der SVG Icon Library.

---

# 205. Standard Buttons

Unterstützt

```text
OK

Abbrechen

Ja

Nein

Weiter

Zurück

Fertig

Wiederholen

Ignorieren
```

Die Reihenfolge ist systemweit einheitlich.

---

# 206. Passwortdialog

Der Passwortdialog dient zur Eingabe von

- Bootpasswörtern
- BitLocker-Schlüsseln
- LUKS-Passwörtern
- Wiederherstellungsschlüsseln
- Administratorpasswörtern

---

# 207. Passwortdialog Eigenschaften

Unterstützt

- Zeichenmaskierung
- Passwort anzeigen
- Caps-Lock-Hinweis
- Tastaturfokus
- Zwischenablage (optional deaktivierbar)

---

# 208. Fortschrittsdialog

Der Fortschrittsdialog visualisiert laufende Prozesse.

Beispiele

- Speicherprüfung
- Dateikopie
- Backup
- Wiederherstellung
- Datenträgerprüfung

---

# 209. Fortschrittstypen

Unterstützt

```text
Determiniert

↓

Indeterminiert
```

Optional

```text
Mehrstufig
```

---

# 210. Dateiauswahldialog

Der File Dialog unterstützt

- Laufwerke
- Verzeichnisse
- Dateien
- Filter
- Icons

Nicht Bestandteil

- Dateibearbeitung

---

# 211. Sicherheitsdialog

Sicherheitskritische Aktionen erfordern einen speziellen Dialog.

Beispiele

- Datenträger löschen
- Partition formatieren
- Bootloader überschreiben
- Schlüssel löschen

Diese Dialoge besitzen zusätzliche Bestätigungsschritte.

---

# 212. Benachrichtigungen

Das Notification Framework verwaltet

- Informationen
- Warnungen
- Erfolge
- Fehler
- laufende Aufgaben

Benachrichtigungen erscheinen standardmäßig oben rechts.

---

# 213. Notification Objekt

```c
typedef struct
{
    uint64_t id;

    nova_notification_type_t type;

    char title[64];

    char message[256];

    uint64_t timeout;

} nova_notification_t;
```

---

# 214. Toast

Toasts sind kurzlebige Hinweise.

Beispiele

```text
✓ Änderungen gespeichert

✓ Datenträger erkannt

✓ Reparatur abgeschlossen
```

Sie verschwinden automatisch.

---

# 215. Overlay

Overlays blockieren den Hintergrund.

Verwendung

- Passwortabfrage
- Wiederherstellung
- Sperrbildschirm
- kritische Warnungen

Overlays besitzen keine eigene Fensterdekoration.

---

# 216. Dialoganimationen

Beim Öffnen

- Fade In
- Scale In

Beim Schließen

- Fade Out
- Scale Out

Alle Animationen stammen aus der Animation Engine.

---

# 217. Dialog Queue

Mehrere Dialoge werden über eine Queue verwaltet.

```text
Dialog 1

↓

Dialog 2

↓

Dialog 3
```

Es ist standardmäßig nur ein modaler Dialog gleichzeitig sichtbar.

---

# 218. Dialog Priorität

```text
Critical

↓

Security

↓

Warning

↓

Information
```

Kritische Dialoge unterbrechen alle anderen UI-Aktionen.

---

# 219. Dialog API

```c
nova_dialog_t*
nova_dialog_create(
    nova_dialog_type_t type);

void
nova_dialog_show(
    nova_dialog_t* dialog);

void
nova_dialog_close(
    nova_dialog_t* dialog);
```

---

# 220. MessageBox API

```c
void
nova_message_box(
    const char* title,
    const char* message,
    nova_dialog_type_t type);
```

---

# 221. Notification API

```c
void
nova_notification_show(
    nova_notification_t* notification);
```

---

# 222. Toast API

```c
void
nova_toast_show(
    const char* message);
```

---

# 223. Fehlerbehandlung

Mögliche Fehler

- ungültiger Dialog
- fehlender Speicher
- doppelte Dialog-ID
- Dialog Queue Overflow
- ungültiger Dialogtyp

Alle Fehler werden im Diagnosesystem protokolliert.

---

# 224. Anforderungen

Das Dialog Framework MUSS

- modale Dialoge unterstützen
- Benachrichtigungen verwalten
- Fortschrittsdialoge bereitstellen
- Passwortdialoge unterstützen
- Theme Driven arbeiten

---

# 225. SHOULD

Das Dialog Framework SOLLTE

- Animationen verwenden
- Dialog Queue besitzen
- Sicherheitsdialoge unterstützen
- Accessibility vorbereiten

---

# 226. MAY

Das Dialog Framework DARF

- Assistenten
- mehrseitige Dialoge
- Benachrichtigungsverlauf
- OEM-Dialoge
- Plugin-Dialoge

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Dialog Framework behandelt Dialoge als **spezialisierte modale Fenster**.

Alle Dialogtypen verwenden dieselbe technische Infrastruktur und unterscheiden sich ausschließlich durch ihren Inhalt und ihr Verhalten.

Dadurch entstehen:

- konsistente Bedienung
- einheitliche Animationen
- gemeinsame Theme-Unterstützung
- einfache Erweiterbarkeit
- zentrale Verwaltung durch den Dialog Manager

---

# 227. UI Animation Framework

Das **Nova UI Animation Framework (NUAF)** definiert sämtliche Animationen innerhalb der Boot-Benutzeroberfläche.

Animationen dienen ausschließlich der Verbesserung der Benutzerführung.

Sie dürfen niemals die Funktionalität oder Stabilität des Bootloaders beeinträchtigen.

---

# 228. Ziele

Das Animation Framework soll

- moderne Übergänge bereitstellen
- Widgets animieren
- Fenster animieren
- Navigation unterstützen
- GPU-unabhängig arbeiten
- deterministisch sein
- geringe CPU-Last erzeugen

---

# 229. Architektur

```text
Boot UI Runtime
↓
Animation Framework
├── Animation Manager
├── Timeline
├── Transition Engine
├── Keyframe Engine
├── Scheduler
├── Physics Engine
├── Interpolation Engine
└── Animation Cache
```

---

# 230. Animation Pipeline

```text
UI Event

↓

Animation

↓

Timeline

↓

Interpolation

↓

Widget Update

↓

Renderer

↓

Present
```

Alle Animationen werden ausschließlich zwischen Layout und Rendering aktualisiert.

---

# 231. Animation Objekt

```c
typedef struct
{
    uint64_t id;

    uint64_t duration;

    uint64_t elapsed;

    bool running;

    bool finished;

    nova_animation_type_t type;

} nova_ui_animation_t;
```

---

# 232. Animation Typen

Version 1 unterstützt

- Fade
- Scale
- Slide
- Opacity
- Rotation
- Color
- Shadow
- Blur

Vorbereitet

- Morphing
- Physics
- Partikel

---

# 233. Widget Animationen

Jedes Widget kann folgende Animationen besitzen

- OnCreate
- OnShow
- OnHide
- OnHover
- OnFocus
- OnPress
- OnRelease
- OnDestroy

---

# 234. Fensteranimationen

Beim Öffnen

```text
Opacity 0 %

↓

Opacity 100 %

+

Scale 95 %

↓

100 %
```

---

# 235. Schließanimation

Standard

```text
Opacity 100 %

↓

0 %

+

Scale 100 %

↓

95 %
```

---

# 236. Seitenwechsel

Unterstützt

- Fade
- Slide Left
- Slide Right
- Slide Up
- Slide Down
- Scale

Alle Übergänge besitzen dieselbe Standarddauer.

---

# 237. Navigation Animationen

Beim Seitenwechsel

```text
Aktuelle Seite

↓

Fade Out

↓

Neue Seite

↓

Fade In
```

Alternativ

```text
Slide
```

---

# 238. Dialoganimationen

Dialoge verwenden

- Fade
- Scale

Sicherheitsdialoge dürfen keine langen Animationen verwenden.

---

# 239. Tooltip Animation

Unterstützt

```text
Fade

+

Scale
```

Dauer

```text
100 ms
```

---

# 240. Toast Animation

Beim Einblenden

```text
Slide Up

+

Fade
```

Beim Ausblenden

```text
Fade
```

---

# 241. Focus Animation

Der Fokus wird durch

- Glow
- Outline
- Opacity

visualisiert.

Keine springenden Animationen.

---

# 242. Hover Animation

Hover verwendet

- leichte Farbänderung
- Materialänderung
- Shadow Animation

Dauer

```text
100 ms
```

---

# 243. Press Animation

Beim Klicken

```text
Scale

100 %

↓

97 %

↓

100 %
```

Dadurch entsteht ein haptischer Eindruck.

---

# 244. Progress Animation

ProgressBars unterstützen

- kontinuierliche Bewegung
- weiche Übergänge
- Statuswechsel
- Farbwechsel

---

# 245. Splash Screen

Der Splash Screen besteht aus

```text
Nova Logo

↓

Glow

↓

Fade

↓

Boot Progress
```

Die gesamte Animation darf maximal 3 Sekunden dauern.

---

# 246. Boot Animation

Der Standardablauf

```text
Firmware

↓

Nova Logo

↓

Systemprüfung

↓

Boot Manager

↓

Kernel
```

Animationen dürfen übersprungen werden, wenn der Schnellstart aktiviert ist.

---

# 247. Microinteractions

Version 1 definiert

- Hover
- Focus
- Press
- Release
- Success
- Warning

Diese Animationen dauern maximal 150 ms.

---

# 248. Timeline

Alle Animationen besitzen

```text
Start

↓

Running

↓

Finish
```

Mehrere Animationen dürfen parallel ausgeführt werden.

---

# 249. Interpolation

Unterstützt

- Linear
- Ease In
- Ease Out
- Ease In Out
- Ease Out Cubic

Standard

```text
Ease Out Cubic
```

---

# 250. Animation Tokens

Alle Zeiten werden zentral definiert.

| Token | Dauer |
|--------|-------|
| Instant | 0 ms |
| Fast | 100 ms |
| Normal | 200 ms |
| Slow | 350 ms |
| Extra Slow | 500 ms |

Widgets dürfen keine eigenen Zeiten definieren.

---

# 251. Animation Scheduler

Der Scheduler priorisiert Animationen.

```text
Critical

↓

Window

↓

Dialog

↓

Widget

↓

Background
```

Bootkritische Animationen besitzen höchste Priorität.

---

# 252. Animation Cache

Vorbereitete Animationen können zwischengespeichert werden.

Dadurch entfallen wiederholte Berechnungen identischer Abläufe.

---

# 253. Performance

Version 1 soll

- mindestens 60 FPS
- Framezeit unter 16 ms
- keine sichtbaren Ruckler

erreichen.

---

# 254. Safe Mode

Im Safe Mode

- werden alle Animationen deaktiviert
- werden Übergänge sofort ausgeführt
- wird maximale Stabilität bevorzugt

---

# 255. Animation API

```c
nova_ui_animation_t*
nova_animation_create(
    nova_animation_type_t type);

void
nova_animation_start(
    nova_ui_animation_t* animation);

void
nova_animation_stop(
    nova_ui_animation_t* animation);

void
nova_animation_update(
    uint64_t delta_time);
```

---

# 256. Fehlerbehandlung

Mögliche Fehler

- ungültige Animation
- doppelte Animation
- Timeline beschädigt
- ungültige Dauer
- Scheduler-Fehler

Diese Fehler werden protokolliert und führen nicht zum Absturz des UI-Systems.

---

# 257. Anforderungen

Das Animation Framework MUSS

- deterministisch arbeiten
- Fenster animieren
- Widgets animieren
- Seitenübergänge unterstützen
- zentrale Animation Tokens verwenden

---

# 258. SHOULD

Das Animation Framework SOLLTE

- Microinteractions besitzen
- Animation Cache verwenden
- hohe Bildrate erreichen
- Animationen priorisieren

---

# 259. MAY

Das Animation Framework DARF

- Physics Animationen
- GPU-Interpolation
- Motion Blur
- Partikelsysteme
- OEM-Animationen

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova UI Animation Framework behandelt Animationen als **zeitgesteuerte Zustandsänderungen**.

Animationen verändern ausschließlich Eigenschaften von Widgets, Fenstern und Seiten.

Das eigentliche Rendering bleibt vollständig Aufgabe des **Nova Boot Graphics Frameworks**.

Dadurch entstehen:

- reproduzierbare Animationen
- vollständige Trennung von Rendering und Logik
- einfache Erweiterbarkeit
- identisches Verhalten auf BIOS-, UEFI- und zukünftigen Plattformen

---

# 260. Accessibility Framework

Das **Nova Accessibility Framework (NAF)** stellt sicher, dass die Benutzeroberfläche auch unter erschwerten Bedingungen zuverlässig bedienbar bleibt.

Obwohl der Bootloader nur kurze Zeit aktiv ist, müssen alle sicherheitsrelevanten Funktionen ohne Maus und möglichst barrierearm nutzbar sein.

---

# 261. Ziele

Das Framework soll

- vollständige Tastaturbedienung ermöglichen
- hohe Kontraste unterstützen
- große Schriftgrößen unterstützen
- Screen Reader vorbereiten
- Internationalisierung ermöglichen
- Lokalisierung unterstützen
- zukünftige Accessibility-Technologien vorbereiten

---

# 262. Architektur

```text
Boot UI Runtime
↓
Accessibility Framework
├── Keyboard Navigation
├── Focus Visualization
├── Contrast Manager
├── Scaling Engine
├── Localization
├── Resource Manager
├── Accessibility Tree
└── UI Semantics
```

---

# 263. Accessibility Tree

Parallel zum Widget Tree wird ein semantischer Baum aufgebaut.

```text
Widget Tree

↓

Accessibility Tree

↓

Semantik

↓

Assistive Technologien
```

Version 1 nutzt diesen Baum hauptsächlich intern.

---

# 264. Accessibility Objekt

```c
typedef struct
{
    uint64_t id;

    char role[32];

    char label[128];

    char description[256];

    bool focusable;

} nova_accessibility_node_t;
```

---

# 265. Widget Rollen

Alle Widgets besitzen eine semantische Rolle.

Beispiele

```text
Window

Button

Label

Image

CheckBox

Switch

ProgressBar

Slider

Menu

Dialog
```

---

# 266. Tastaturbedienung

Der gesamte Bootloader muss ohne Maus bedienbar sein.

Unterstützt

- Tab
- Shift+Tab
- Pfeiltasten
- Enter
- Escape
- Leertaste

---

# 267. Fokusanzeige

Der aktuelle Fokus muss jederzeit eindeutig sichtbar sein.

Unterstützt

- Outline
- Glow
- Farbwechsel
- Kontrastmodus

Blinkende Effekte sind nicht zulässig.

---

# 268. Hoher Kontrast

Version 1 unterstützt einen High-Contrast-Modus.

Eigenschaften

- schwarzer Hintergrund
- weiße Schrift
- hohe Kontrastwerte
- deutliche Fokusrahmen

---

# 269. Skalierung

Alle UI-Elemente können logisch skaliert werden.

Standardstufen

```text
100 %

125 %

150 %

175 %

200 %
```

Die Skalierung erfolgt ohne Layoutänderungen.

---

# 270. Schriftgrößen

Version 1 definiert standardisierte Textgrößen.

```text
XS

S

M

L

XL

XXL
```

Alle Widgets verwenden ausschließlich diese Größen.

---

# 271. Screen Reader

Version 1 enthält keine vollständige Screen-Reader-Unterstützung.

Die Architektur stellt jedoch alle notwendigen semantischen Informationen bereit.

Dadurch kann später eine Assistenzsoftware integriert werden, ohne die Widgets ändern zu müssen.

---

# 272. Internationalisierung (i18n)

Die Benutzeroberfläche verwendet ausschließlich Ressourcenkennungen.

Beispiel

```text
IDS_BOOT_START

IDS_RECOVERY

IDS_SETTINGS

IDS_PASSWORD
```

Widgets enthalten niemals fest codierte Texte.

---

# 273. Lokalisierung (l10n)

Alle Texte werden aus Sprachressourcen geladen.

Unterstützt

- Deutsch
- Englisch

Vorbereitet

- beliebige weitere Sprachen

---

# 274. Sprachpakete

Ein Sprachpaket besteht aus

```text
Strings

↓

Icons (optional)

↓

Datumsformate

↓

Zahlenformate

↓

Richtung
```

---

# 275. RTL-Unterstützung

Die Architektur unterstützt zukünftige Sprachen mit Rechts-nach-Links-Schrift.

Vorbereitet

- Arabisch
- Hebräisch
- Persisch

Version 1 aktiviert RTL standardmäßig nicht.

---

# 276. Ressourcenverwaltung

Alle UI-Ressourcen werden zentral verwaltet.

Unterstützt

- Texte
- Icons
- Farben
- Animationen
- Themes

---

# 277. Resource Objekt

```c
typedef struct
{
    uint32_t id;

    char language[8];

    const char* value;

} nova_resource_entry_t;
```

---

# 278. Ressourcen API

```c
const char*
nova_resource_get_string(
    uint32_t id);
```

---

# 279. Datums- und Zahlenformate

Vorbereitet

- Datumsformat
- Uhrzeitformat
- Dezimaltrennzeichen
- Tausendertrennzeichen

Version 1 verwendet standardmäßig ISO-8601 für Datumsangaben.

---

# 280. Farbenblindheit

Vorbereitet

- Deuteranopie
- Protanopie
- Tritanopie

Statusinformationen sollen nicht ausschließlich über Farben vermittelt werden.

---

# 281. Symbolik

Kritische Informationen werden immer durch

- Farbe
- Symbol
- Text

gleichzeitig dargestellt.

Beispiel

```text
⚠ Warnung

Nicht nur gelb
```

---

# 282. Accessibility API

```c
void
nova_accessibility_update(
    nova_widget_t* widget);

const nova_accessibility_node_t*
nova_accessibility_node(
    uint64_t id);
```

---

# 283. Fehlerbehandlung

Mögliche Fehler

- fehlende Ressourcen
- ungültige Sprach-ID
- fehlende Übersetzung
- doppelte Ressourcen-ID
- inkonsistente Accessibility-Daten

Fehlende Übersetzungen fallen automatisch auf die Standardsprache zurück.

---

# 284. Anforderungen

Das Accessibility Framework MUSS

- vollständige Tastaturbedienung ermöglichen
- Internationalisierung unterstützen
- Lokalisierung unterstützen
- semantische Rollen definieren
- Ressourcen zentral verwalten

---

# 285. SHOULD

Das Accessibility Framework SOLLTE

- High-Contrast unterstützen
- Skalierung ermöglichen
- Screen Reader vorbereiten
- Farbenblindheit berücksichtigen

---

# 286. MAY

Das Accessibility Framework DARF

- Braille-Ausgabe vorbereiten
- Sprachsteuerung integrieren
- Audiounterstützung
- alternative Eingabegeräte

unterstützen.

---

# Architekturentscheidung (ADR)

Das Nova Accessibility Framework trennt **Darstellung**, **Semantik** und **Lokalisierung** vollständig voneinander.

Widgets enthalten ausschließlich Ressourcenkennungen und semantische Informationen.

Die tatsächliche Darstellung, Sprache und Zugänglichkeit werden erst zur Laufzeit durch Theme-, Ressourcen- und Accessibility-Komponenten bestimmt.

Dadurch bleibt das gesamte UI-System unabhängig von Sprache, Schriftgröße und zukünftigen Assistenztechnologien.

---

# 287. Öffentliche UI API

Das Nova Boot UI Framework stellt eine stabile öffentliche API bereit.

Diese API ist die einzige zulässige Schnittstelle zur Erstellung und Verwaltung der Benutzeroberfläche.

```text
Boot Core

↓

Boot UI API

↓

UI Runtime

↓

Window Manager

↓

Graphics Runtime
```

Direkte Zugriffe auf Renderer, Fensterlisten oder Widgets sind außerhalb der UI Runtime nicht zulässig.

---

# 288. API Designprinzipien

Die öffentliche API folgt den Grundsätzen

- ABI-stabil
- modular
- objektorientiert
- deterministisch
- dokumentiert
- plattformneutral
- versionsfähig

---

# 289. UI Initialisierung

```c
nova_result_t
nova_ui_initialize(void);
```

Initialisiert

- Window Manager
- Widget Manager
- Layout Engine
- Theme Engine
- Dialog Framework
- Navigation Framework
- Animation Framework
- Accessibility Framework

---

# 290. UI Shutdown

```c
nova_result_t
nova_ui_shutdown(void);
```

Beendet sämtliche UI-Komponenten kontrolliert.

---

# 291. UI Update

```c
void
nova_ui_update(
    uint64_t delta_time);
```

Aktualisiert

- Layout
- Animationen
- Navigation
- Dialoge
- Widgets

---

# 292. UI Render

```c
void
nova_ui_render(void);
```

Erzeugt den vollständigen Render Tree und übergibt ihn an das Graphics Framework.

---

# 293. Window API

```c
nova_window_t*
nova_window_create(void);

void
nova_window_destroy(
    nova_window_t* window);

void
nova_window_show(
    nova_window_t* window);

void
nova_window_hide(
    nova_window_t* window);
```

---

# 294. Widget API

```c
nova_widget_t*
nova_widget_create(
    nova_widget_type_t type);

void
nova_widget_destroy(
    nova_widget_t* widget);

void
nova_widget_add_child(
    nova_widget_t* parent,
    nova_widget_t* child);

void
nova_widget_remove_child(
    nova_widget_t* child);
```

---

# 295. Layout API

```c
void
nova_layout_measure(
    nova_widget_t* root);

void
nova_layout_arrange(
    nova_widget_t* root);

void
nova_layout_invalidate(
    nova_widget_t* widget);
```

---

# 296. Navigation API

```c
void
nova_navigation_push(
    nova_page_t* page);

void
nova_navigation_pop(void);

void
nova_navigation_replace(
    nova_page_t* page);

void
nova_navigation_home(void);
```

---

# 297. Dialog API

```c
nova_dialog_t*
nova_dialog_create(
    nova_dialog_type_t type);

void
nova_dialog_show(
    nova_dialog_t* dialog);

void
nova_dialog_close(
    nova_dialog_t* dialog);
```

---

# 298. Notification API

```c
void
nova_notification_show(
    nova_notification_t* notification);

void
nova_notification_hide(
    uint64_t id);
```

---

# 299. Animation API

```c
void
nova_animation_start(
    nova_ui_animation_t* animation);

void
nova_animation_stop(
    nova_ui_animation_t* animation);

void
nova_animation_update(
    uint64_t delta_time);
```

---

# 300. Theme API

```c
nova_result_t
nova_theme_load(
    const char* theme);

const nova_theme_t*
nova_theme_current(void);
```

---

# 301. Ressourcen API

```c
const char*
nova_resource_get_string(
    uint32_t id);

nova_icon_t*
nova_resource_get_icon(
    uint32_t id);

nova_image_t*
nova_resource_get_image(
    uint32_t id);
```

---

# 302. Event API

```c
void
nova_event_post(
    nova_event_t* event);

void
nova_event_dispatch(
    nova_event_t* event);
```

---

# 303. Öffentliche UI Runtime

```c
typedef struct
{
    nova_window_manager_t* windows;

    nova_navigation_manager_t* navigation;

    nova_dialog_manager_t* dialogs;

    nova_animation_manager_t* animations;

    nova_theme_t* theme;

} nova_ui_runtime_t;
```

---

# 304. ABI Header

Alle exportierten Datenstrukturen beginnen mit

```c
typedef struct
{
    uint64_t magic;

    uint16_t major;

    uint16_t minor;

    uint32_t size;

} nova_ui_abi_header_t;
```

---

# 305. Magic

```text
"NUBI"
```

Nova User Interface Binary Interface

---

# 306. Versionierung

Semantic Versioning

```text
Major

↓

ABI Änderungen

Minor

↓

Neue Funktionen

Patch

↓

Fehlerkorrekturen
```

---

# 307. Fehlercodes

```c
typedef enum
{
    NOVA_UI_SUCCESS = 0,

    NOVA_UI_INVALID_WINDOW,

    NOVA_UI_INVALID_WIDGET,

    NOVA_UI_INVALID_DIALOG,

    NOVA_UI_INVALID_PAGE,

    NOVA_UI_OUT_OF_MEMORY,

    NOVA_UI_INVALID_STATE,

    NOVA_UI_UNSUPPORTED,

    NOVA_UI_FATAL

} nova_ui_result_t;
```

---

# 308. Referenzimplementierung

```text
ui/
├── runtime/
├── windows/
├── widgets/
├── layouts/
├── navigation/
├── dialogs/
├── themes/
├── animation/
├── accessibility/
├── resources/
├── api/
├── tests/
└── docs/
```

---

# 309. Unit Tests

Jede UI-Komponente besitzt eigene Unit Tests.

Mindestens

- Window Manager
- Layout Engine
- Widget Library
- Navigation
- Dialoge
- Theme Engine
- Animationen
- Accessibility

---

# 310. Integrationstests

Pflichttests

- BIOS
- UEFI
- Maus
- Tastatur
- Touch (Simulation)
- verschiedene Auflösungen
- verschiedene DPI-Stufen

---

# 311. Regressionstests

Vor jedem Release werden geprüft

✔ Fensterverwaltung

✔ Layout

✔ Widgets

✔ Navigation

✔ Dialoge

✔ Animationen

✔ Accessibility

✔ ABI

---

# 312. Performanceziele

| Komponente | Ziel |
|------------|------:|
| UI Initialisierung | < 10 ms |
| Layout | < 1 ms |
| Widget Update | < 2 ms |
| Navigation | < 1 ms |
| Dialog öffnen | < 5 ms |
| Seitenwechsel | < 10 ms |
| UI Render | < 5 ms |

---

# 313. Speicherziele

| Bereich | Ziel |
|---------|------:|
| Window Manager | < 256 KB |
| Widget Tree | < 1 MB |
| Theme | < 512 KB |
| Navigation | < 256 KB |
| Animationen | < 512 KB |

---

# 314. Dokumentationsstandard

Jede öffentliche Funktion wird dokumentiert.

```c
/*
Beschreibung

Parameter

Rückgabewert

Fehler

Komplexität

Seiteneffekte

Thread Safety
*/
```

---

# 315. Anforderungen

Die öffentliche UI API MUSS

- ABI-stabil sein
- dokumentiert sein
- versionsfähig sein
- deterministisch arbeiten
- plattformneutral sein

---

# 316. SHOULD

Die API SOLLTE

- leicht portierbar sein
- spätere Desktop-Erweiterungen unterstützen
- Mehrmonitorbetrieb vorbereiten
- GPU-Backends vorbereiten

---

# 317. MAY

Die API DARF

- experimentelle Widgets
- OEM-Erweiterungen
- Plugin-Systeme
- zukünftige Eingabegeräte

unterstützen, sofern die öffentliche API unverändert bleibt.

---

# 318. Architekturentscheidung (ADR)

Die öffentliche UI API bildet die **einzige offizielle Schnittstelle** zwischen Boot Core und Benutzeroberfläche.

Alle internen Komponenten – Window Manager, Widget Library, Layout Engine, Navigation, Dialoge und Animationen – bleiben vollständig gekapselt.

Dadurch können interne Implementierungen jederzeit modernisiert werden, ohne bestehende Boot-Komponenten oder Anwendungen anzupassen.

---

# 319. Referenzarchitektur

Das Nova Boot UI Framework bildet die vollständige grafische Benutzeroberfläche des Bootloaders.

Alle UI-Komponenten arbeiten ausschließlich über standardisierte Frameworks.

```text
                    Boot Core
                         │
                         ▼
                  Boot UI Runtime
                         │
 ┌───────────────────────┼────────────────────────┐
 │                       │                        │
 ▼                       ▼                        ▼
Window Manager     Navigation Engine      Dialog Framework
 │                       │                        │
 ▼                       ▼                        ▼
Layout Engine      Widget Library        Animation Framework
 │                       │                        │
 └─────────────── UI Composition ─────────────────┘
                         │
                         ▼
                Graphics Framework API
                         │
                         ▼
                Nova Boot Graphics Runtime
                         │
                         ▼
                     Bildschirm
```

Alle Komponenten kommunizieren ausschließlich über öffentliche APIs.

---

# 320. Beziehungen zu anderen NPSPECs

Das Boot UI Framework baut auf folgenden Spezifikationen auf:

- NPSPEC-BOOTGRAPHICS-0001
- NPSPEC-BOOTINPUT-0001
- NPSPEC-BOOTMEMORY-0001
- NPSPEC-BOOTPLATFORM-0001
- NPSPEC-THEME-0001 (zukünftig)
- NPSPEC-ACCESSIBILITY-0001 (zukünftig)

Spätere Desktop-Komponenten übernehmen große Teile dieser Architektur.

---

# 321. Gesamter UI-Datenfluss

```text
Input

↓

Input Runtime

↓

Event Dispatcher

↓

Navigation

↓

Widget Tree

↓

Layout Engine

↓

Animation Engine

↓

Render Tree

↓

Graphics Framework

↓

Framebuffer

↓

Monitor
```

Dieser Ablauf ist verbindlich.

---

# 322. Ressourcenfluss

```text
Theme

↓

Strings

↓

Icons

↓

Fonts

↓

Images

↓

Widgets

↓

Renderer
```

Alle Ressourcen werden zentral verwaltet.

---

# 323. Objektmodell

Das UI basiert vollständig auf einer hierarchischen Objektstruktur.

```text
Application

↓

Window

↓

Page

↓

Container

↓

Widget

↓

Content
```

Direkte Beziehungen zwischen nicht verwandten Widgets sind nicht zulässig.

---

# 324. Zustandsmodell

Alle UI-Komponenten besitzen definierte Zustände.

```text
Created

↓

Initialized

↓

Visible

↓

Focused

↓

Disabled

↓

Hidden

↓

Destroyed
```

Ungültige Zustandsübergänge müssen erkannt und verhindert werden.

---

# 325. Implementierungsphasen

## Phase 1

- Window Manager
- Widget Basis
- Layout Engine

---

## Phase 2

- Standard Widgets
- Dialoge
- Navigation

---

## Phase 3

- Animation Framework
- Theme Integration
- Ressourcenverwaltung

---

## Phase 4

- Erweiterte Widgets
- Dashboard
- Dateiauswahl

---

## Phase 5

- Accessibility
- Internationalisierung
- High Contrast

---

## Phase 6

- Optimierungen
- Caching
- Diagnosen
- Performance

---

# 326. Akzeptanzkriterien

Das Framework gilt als vollständig implementiert, wenn

✓ mehrere Fenster dargestellt werden können

✓ Layout automatisch berechnet wird

✓ Widgets vollständig funktionieren

✓ Navigation korrekt arbeitet

✓ Dialoge dargestellt werden

✓ Animationen flüssig laufen

✓ Accessibility vorbereitet ist

✓ sämtliche Ressourcen zentral geladen werden

✓ das Theme vollständig angewendet wird

✓ das Graphics Framework ausschließlich über die öffentliche API verwendet wird

---

# 327. Qualitätsziele

Das UI Framework verfolgt dauerhaft folgende Ziele

- Konsistenz
- Determinismus
- Wartbarkeit
- Erweiterbarkeit
- Wiederverwendbarkeit
- Plattformunabhängigkeit
- geringe Speicherbelegung
- geringe CPU-Last

---

# 328. Performanceziele

Empfohlene Zielwerte

| Bereich | Ziel |
|----------|------:|
| UI Initialisierung | < 10 ms |
| Layout | < 1 ms |
| Widget Update | < 2 ms |
| Seitenwechsel | < 10 ms |
| Dialog öffnen | < 5 ms |
| Fenster öffnen | < 8 ms |
| Vollständiger UI-Render | < 5 ms |

Version 1 soll auf geeigneter Hardware dauerhaft mindestens **60 FPS** erreichen.

---

# 329. Architekturprinzipien

Das Nova UI Framework basiert dauerhaft auf folgenden Grundprinzipien.

## Widget First

Jedes sichtbare Element ist ein Widget.

Es existieren keine Sonderfälle.

---

## Theme Driven

Alle visuellen Eigenschaften stammen aus der Theme Engine.

Widgets besitzen keine fest definierten Farben oder Schriftarten.

---

## Layout Driven

Die Positionierung erfolgt ausschließlich über die Layout Engine.

Widgets bestimmen ihre Position niemals selbst.

---

## Retained Mode

Die Oberfläche wird vollständig über einen Widget Tree beschrieben.

Immediate Mode Rendering ist nicht Bestandteil der Architektur.

---

## Event Driven

Alle Benutzerinteraktionen erfolgen ausschließlich über das Event Framework.

Direkte Hardwarezugriffe sind verboten.

---

## Separation of Concerns

Jede Komponente besitzt genau eine Verantwortlichkeit.

- Window Manager → Fenster
- Navigation → Seiten
- Layout → Positionierung
- Graphics → Rendering
- Theme → Darstellung
- Input → Eingaben

---

## API Stability

Alle öffentlichen Schnittstellen bleiben innerhalb einer Hauptversion stabil.

---

## Platform Independence

Die Benutzeroberfläche besitzt keinerlei Abhängigkeiten zu BIOS, UEFI oder zukünftigen Plattformen.

---

# 330. Erweiterungsmöglichkeiten

Die Architektur ist bereits vorbereitet für

- Multi Window Desktop
- Mehrmonitorbetrieb
- GPU Rendering
- Hardwarebeschleunigte Animationen
- Plugin Widgets
- OEM Designs
- Window Docking
- Snap Layouts
- Touch Optimierung
- Stifteingabe
- Gamepad Navigation
- Sprachsteuerung
- 3D Benutzeroberflächen
- KI-gestützte Navigation

Diese Erweiterungen dürfen die öffentliche API nicht verändern.

---

# 331. Langfristige Vision

Das Nova Boot UI Framework bildet langfristig die gemeinsame Grundlage für sämtliche Benutzeroberflächen innerhalb von Nova.

```text
Bootloader

↓

Recovery

↓

Kernel UI

↓

Login Manager

↓

Desktop

↓

Systemprogramme

↓

Anwendungen
```

Dadurch entsteht ein durchgängiges UI-Framework mit identischen Widgets, identischem Layoutsystem und einheitlicher Benutzerführung über das gesamte Betriebssystem hinweg.

---

# 332. Roadmap

## Version 1.x

- Vollständiger Bootloader
- Recovery
- Diagnosetools
- Installer

---

## Version 2.x

- Kernel Window Manager
- Desktop Shell
- Mehrfenstersystem
- Desktop Widgets

---

## Version 3.x

- Hardwarebeschleunigung
- GPU Composition
- HDR
- Multi Monitor
- Accessibility vollständig

---

## Version 4.x

- Adaptive Benutzeroberfläche
- KI-gestützte Navigation
- Sprachsteuerung
- Erweiterte Animationen
- Cloud-Synchronisation (optional)

---

# 333. Zusammenfassung

Das **Nova Boot UI Framework** definiert die vollständige Benutzeroberfläche des Nova-Bootloaders.

Es umfasst:

- Window Manager
- Widget Library
- Layout Engine
- Navigation Framework
- Dialog Framework
- Event Framework
- Animation Framework
- Accessibility Framework
- Ressourcenverwaltung
- Öffentliche UI API

Alle Komponenten arbeiten modular zusammen und kommunizieren ausschließlich über definierte Schnittstellen.

---

# 334. Abschluss

Mit der Umsetzung dieser Spezifikation verfügt Nova über ein modernes, konsistentes und vollständig erweiterbares UI-System, das bereits im Bootloader beginnt und ohne Architekturbruch bis zum Desktop und zu den Anwendungen weitergeführt werden kann.

Das Framework bildet damit die Referenzimplementierung für sämtliche grafischen Benutzeroberflächen innerhalb des Nova-Ökosystems.