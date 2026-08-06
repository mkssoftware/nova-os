# NPSPEC-BOOTCONTROL-0001
## Boot Control Framework

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTCONTROL-0001 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI Framework |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTUI-0001, BOOTLAYOUT-0001, BOOTRENDER-0001, BOOTINPUT-0001, BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Das Boot Control Framework definiert die gemeinsame Infrastruktur sämtlicher grafischer Steuerelemente (Controls) der NovaOS-Bootoberfläche.

Alle Controls besitzen dieselbe technische Basis und dieselben Lebenszyklen. Dadurch verhalten sich sämtliche UI-Komponenten identisch hinsichtlich Rendering, Layout, Eingaben, Fokusverwaltung, Animationen und Accessibility.

Das Framework dient ausschließlich als Basisschicht und enthält keine spezifische Logik einzelner Controls.

---

# 2. Ziele des Frameworks

Das Framework soll:

- einheitliche API bereitstellen
- deterministische Laufzeiten besitzen
- keinerlei Heap-Allokationen während der Laufzeit durchführen
- BIOS und UEFI identisch unterstützen
- vollständig themenfähig sein
- vollständig DPI-unabhängig arbeiten
- Accessibility unterstützen
- leicht erweiterbar sein

---

# 3. Architektur

```text
Application

↓

Dialog / Page

↓

Layout Engine

↓

Control Framework

↓

Individual Controls

↓

Renderer

↓

Framebuffer
```

---

# 4. Aufgaben

Das Framework übernimmt:

- Control-Erzeugung
- Lebenszyklus
- Zustandsverwaltung
- Eingaben
- Fokus
- Rendering
- Dirty-State
- Invalidierung
- Parent/Child-Verwaltung
- Ereignisse
- Accessibility
- Theme-Anbindung
- Animationen

---

# 5. Grundprinzip

Jedes UI-Element ist ein Control.

Beispiele:

- Button
- Label
- TextBox
- Image
- ProgressBar
- CheckBox
- RadioButton
- List
- Container
- ScrollView

Alle besitzen dieselbe Basisklasse.

---

# 6. Objektmodell

```text
Control
├── Visual
├── Layout
├── Input
├── Focus
├── State
├── Theme
├── Accessibility
└── Animation
```

---

# 7. Lebenszyklus

```text
Create

↓

Initialize

↓

Measure

↓

Arrange

↓

Visible

↓

Active

↓

Disabled

↓

Destroy
```

Kein Schritt darf übersprungen werden.

---

# 8. Control States

Jedes Control besitzt mindestens folgende Zustände:

```text
Normal

Hovered

Focused

Pressed

Disabled

Hidden
```

Optionale Zustände:

```text
Checked

Selected

Expanded

Busy

Error
```

---

# 9. Eigenschaften

Jedes Control besitzt:

```text
ID

Name

Bounds

Visibility

Opacity

Enabled

Focusable

Parent

Children

Theme

State
```

---

# 10. Parent/Child Modell

Controls bilden einen Baum.

```text
Dialog
├── Header
├── Content
│   ├── Label
│   ├── Button
│   └── ProgressBar
└── Footer
```

Jedes Control besitzt höchstens einen Parent.

---

# 11. Ereignissystem

Unterstützte Ereignisse:

- Create
- Destroy
- Focus
- Blur
- MouseEnter
- MouseLeave
- MouseMove
- MouseDown
- MouseUp
- Click
- DoubleClick
- KeyDown
- KeyUp
- CharacterInput
- ValueChanged
- StateChanged
- LayoutChanged
- ThemeChanged

---

# 12. Ereignisfluss

```text
Input

↓

Control

↓

Parent

↓

Dialog

↓

Application
```

Nicht behandelte Ereignisse steigen im Baum nach oben.

---

# 13. Rendering

Jedes Control besitzt:

- RenderNode
- DirtyFlag
- ClipRegion
- Material
- Z-Index

Rendering erfolgt ausschließlich über den Renderer.

---

# 14. Dirty State

Änderungen setzen:

```text
Dirty

↓

Layout

↓

Render

↓

Present
```

Nur betroffene Bereiche werden neu gezeichnet.

---

# 15. Layout

Das Framework stellt bereit:

- Preferred Size
- Minimum Size
- Maximum Size
- Margin
- Padding
- Alignment
- Stretch

Die Layout Engine bestimmt die endgültige Position.

---

# 16. Fokus

Fokussierbare Controls besitzen:

```text
Focusable

↓

Focused

↓

Blur
```

Der Fokus wird ausschließlich zentral verwaltet.

---

# 17. Eingaben

Unterstützt:

- Tastatur
- Maus
- Touch
- Controller

Alle Geräte verwenden dieselben Ereignisse.

---

# 18. Animationen

Das Framework integriert:

- Fade
- Scale
- Opacity
- Position
- Focus Animation
- State Animation

Animationen verändern niemals Layoutdaten.

---

# 19. Theme Integration

Das Theme liefert:

- Farben
- Schriftarten
- Rundungen
- Schatten
- Transparenz
- Größen
- Animationen

Controls enthalten keinerlei fest codierte Farben.

---

# 20. Accessibility

Jedes Control besitzt:

- Name
- Rolle
- Beschreibung
- Status
- Fokusreihenfolge
- Shortcut

Screenreader greifen ausschließlich auf diese Daten zu.

---

# 21. Ressourcenverwaltung

Alle Controls verwenden:

- Objektpools
- Handle-System
- Referenzzählung
- statische Tabellen

Heap-Allokationen zur Laufzeit sind verboten.

---

# 22. Speicherlayout

```text
Control Pool

↓

Control

↓

Layout Data

↓

State Data

↓

Theme Data

↓

Render Data
```

Alle Daten liegen zusammenhängend im Speicher.

---

# 23. Performance

Anforderungen:

- O(1) Zustandswechsel
- O(1) Fokuswechsel
- O(1) Invalidierung
- O(n) Rendering
- O(n) Layout

Keine rekursiven Traversierungen außerhalb der UI-Hierarchie.

---

# 24. Threading

Das Boot UI arbeitet Single-Threaded.

Alle Control-Operationen erfolgen im UI-Thread.

Nebenläufige Änderungen sind unzulässig.

---

# 25. Sicherheit

Das Framework verhindert:

- doppelte IDs
- ungültige Eltern
- rekursive Hierarchien
- Fokusverlust
- ungültige Zustände
- zerstörte Referenzen

Alle Änderungen erfolgen atomar.

---

# 26. Fehlerbehandlung

Erkannt werden:

- Nullpointer
- ungültiger Parent
- ungültiger Zustand
- beschädigte Hierarchie
- doppelte IDs
- Layoutfehler
- Renderfehler

Fehlerhafte Controls werden deaktiviert.

---

# 27. API

```c
typedef struct nova_control nova_control_t;

typedef enum
{
    NOVA_CONTROL_VISIBLE,
    NOVA_CONTROL_ENABLED,
    NOVA_CONTROL_FOCUSED,
    NOVA_CONTROL_DIRTY
}
nova_control_flag_t;

nova_result_t
nova_control_create(
    nova_control_type_t type,
    nova_control_t **control);

nova_result_t
nova_control_destroy(
    nova_control_t *control);

nova_result_t
nova_control_set_parent(
    nova_control_t *child,
    nova_control_t *parent);

nova_result_t
nova_control_set_bounds(
    nova_control_t *control,
    nova_rect_t bounds);

nova_result_t
nova_control_invalidate(
    nova_control_t *control);

nova_result_t
nova_control_focus(
    nova_control_t *control);
```

---

# 28. Diagnosedaten

Zu erfassen sind:

- Anzahl Controls
- aktive Controls
- Fokuswechsel
- Dirty Regions
- Renderdauer
- Layoutdauer
- Speicherverbrauch
- Fehler
- Ereignisanzahl

---

# 29. Testfälle

Zu testen sind:

- Control-Erzeugung
- Parent/Child-Verwaltung
- Fokuswechsel
- Zustandswechsel
- Ereignisweiterleitung
- Layout
- Rendering
- Dirty Regions
- Accessibility
- Themewechsel
- Speicherverwaltung
- Fehlerfälle

---

# 30. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche Controls ausschließlich auf dem Boot Control Framework basieren.
- alle Controls denselben Lebenszyklus besitzen.
- Rendering, Layout und Eingaben vollständig integriert sind.
- Parent/Child-Beziehungen konsistent bleiben.
- Accessibility vollständig unterstützt wird.
- keine Heap-Allokationen während der Laufzeit erfolgen.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.
- neue Controls ausschließlich durch Ableitung vom Base Control erstellt werden können.

---

# 31. Referenzen

## ADRs

- ADR-BOOTCONTROL-0001 – Einheitliches Control Framework
- ADR-BOOTUI-0002 – Deklaratives UI-Modell
- ADR-BOOTINPUT-0005 – Fokusverwaltung
- ADR-BOOTACCESS-0001 – Accessibility
- ADR-BOOTPERF-0002 – Deterministische UI-Verarbeitung

## NPSPECs

- NPSPEC-BOOTUI-0001 – Boot UI Architecture
- NPSPEC-BOOTLAYOUT-0001 – Boot Layout Engine
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTINPUT-0001 – Input System
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 32. Zusammenfassung

Das **Boot Control Framework** bildet die technische Grundlage sämtlicher grafischer Steuerelemente der NovaOS-Bootoberfläche. Es definiert einen einheitlichen Lebenszyklus, ein gemeinsames Zustandsmodell, standardisierte Ereignisverarbeitung sowie die vollständige Integration in Layout, Rendering, Fokusverwaltung, Animationen und Accessibility. Durch statische Speicherverwaltung, deterministische Laufzeiten und eine konsequente Trennung zwischen Framework und konkreten Controls entsteht eine robuste, erweiterbare und hardwareunabhängige Basis für alle UI-Komponenten des NovaOS Boot Managers.