# ADR-BOOTCONTROL-0001 – Gemeinsames Basismodell für Bedienelemente

## Status

Angenommen

## Kategorie

Boot UI / Controls / Interaction / Accessibility / Rendering / Input

## Kontext

Die grafische Bootoberfläche von NovaOS benötigt wiederverwendbare Bedienelemente für:

- Bootmenüs,
- Recovery,
- NovaDOS-Auswahl,
- Dialoge,
- Einstellungen,
- Listen,
- Bestätigungen,
- Fortschrittsanzeigen.

Ohne gemeinsames Basismodell würden einzelne Controls jeweils eigene Regeln für:

- Zustand,
- Eingabe,
- Fokus,
- Darstellung,
- Aktivierung,
- Deaktivierung,
- Barrierefreiheit

implementieren.

Dies würde Inkonsistenzen erzeugen und die Boot-UI unnötig vergrößern.

Die Bootumgebung soll deshalb ein kleines, gemeinsames Control-Modell besitzen, auf dem konkrete Bedienelemente aufbauen.

---

## Entscheidung

Alle interaktiven Bedienelemente der NovaOS-Bootoberfläche basieren auf einem gemeinsamen logischen Control-Modell.

Konzeptionell:

```text
BootControl
    │
    ├── Button
    ├── ListItem
    ├── CheckBox
    ├── RadioButton
    ├── Slider
    ├── TextInput
    └── weitere Controls
```

Das Basismodell definiert ausschließlich gemeinsame Eigenschaften und Verhaltensregeln.

Spezifische Control-Semantik bleibt Bestandteil des jeweiligen konkreten Controls.

---

## Grundprinzip

```text
One interaction model.

Many control types.
```

Das gemeinsame Modell soll insbesondere verhindern, dass jedes Control eigene inkompatible Implementierungen für Fokus, Status und Eingabeverarbeitung erhält.

---

## Logisches Basismodell

Konzeptionell:

```text
BootControl {
    id

    bounds

    visible
    enabled

    focusable
    focused

    hovered
    pressed
    selected

    state

    style
    content

    accessibility

    parent
}
```

Die konkrete Speicherstruktur ist nicht Bestandteil dieser ADR.

---

## Trennung von Control, Darstellung und Eingabe

Ein Control ist nicht identisch mit seinen gezeichneten Pixeln.

Die Architektur trennt:

```text
Control State
     ↓
Visual State
     ↓
Renderer
```

sowie:

```text
Input Event
     ↓
Hit Test / Focus
     ↓
Control Interaction
     ↓
State Change
```

Dadurch kann dieselbe logische Control-Semantik mit unterschiedlichen Renderern verwendet werden.

---

## Gemeinsame Zustände

Bedienelemente müssen mindestens folgende Grundzustände unterstützen können:

```text
Normal
Hovered
Pressed
Focused
Disabled
```

Controls dürfen zusätzliche eigene Zustände besitzen.

Beispiele:

```text
Selected
Checked
Indeterminate
Expanded
Error
```

Nicht jeder Grundzustand muss für jedes Control visuell unterschiedlich dargestellt werden.

---

## Zustandstrennung

Die folgenden Zustände dürfen nicht miteinander gleichgesetzt werden:

```text
Focused
Selected
Hovered
Pressed
```

Beispiel:

Ein Listenelement kann ausgewählt sein, ohne aktuell den Tastaturfokus zu besitzen.

---

## Enabled

Ein deaktiviertes Control darf keine normale Aktivierungsaktion ausführen.

```text
enabled = false
```

bedeutet jedoch nicht zwingend:

```text
visible = false
```

Ein deaktiviertes Control kann sichtbar bleiben und entsprechend dargestellt werden.

---

## Visibility

Unsichtbare Controls:

- werden nicht gerendert,
- nehmen nicht an normalem Hit Testing teil,
- dürfen keinen aktiven Fokus behalten.

Layoutverhalten bei unsichtbaren Controls wird durch das Boot-Layout-System definiert.

---

## Fokus

Die Boot-UI besitzt ein explizites Fokusmodell.

Der Fokus bestimmt, welches Control gerichtete Tastatur- oder vergleichbare Eingabe erhält.

Grundmodell:

```text
Input
  ↓
Focus Manager
  ↓
Focused Control
```

Nur fokussierbare und aktive Controls dürfen normalen Fokus erhalten.

---

## Fokusnavigation

Die Bootoberfläche muss vollständig ohne Zeigegerät bedienbar sein.

Daher müssen Controls eine deterministische Fokusnavigation unterstützen.

Abhängig vom jeweiligen UI-Kontext können verwendet werden:

```text
Tab
Shift+Tab
Arrow Keys
Enter
Space
Escape
```

Die konkrete Zuordnung wird in den Input- und Control-Spezifikationen definiert.

---

## Fokusdarstellung

Ein fokussiertes Control muss visuell eindeutig erkennbar sein.

Der Fokusindikator darf nicht ausschließlich durch Hover dargestellt werden.

Dies ist insbesondere relevant für:

- Tastaturbedienung,
- Recovery,
- eingeschränkte Eingabegeräte,
- Barrierefreiheit.

---

## Pointer-Interaktion

Bei vorhandenem Pointer erfolgt die Verarbeitung grundsätzlich nach:

```text
Pointer Position
      ↓
Hit Test
      ↓
Target Control
      ↓
Interaction State
```

Das Basismodell stellt dafür gemeinsame Zustände wie:

```text
Hovered
Pressed
```

bereit.

---

## Aktivierung

Controls mit Aktivierungssemantik verwenden eine einheitliche logische Aktivierungsaktion.

Beispiel:

```text
Activate()
```

Die konkrete Auswirkung wird vom jeweiligen Control beziehungsweise dessen gebundener Aktion definiert.

Damit können:

- Maus,
- Touch,
- Tastatur

dieselbe semantische Aktion auslösen.

---

## Commands / Actions

Ein Control soll eine Aktion auslösen können, ohne deren Implementierungslogik selbst enthalten zu müssen.

Konzeptionell:

```text
Control
   ↓
Action
   ↓
Boot Operation
```

Beispiele:

```text
BootNormal
BootRecovery
LaunchNovaDOS
OpenDiagnostics
ConfirmDialog
CancelDialog
```

Damit bleiben Darstellung und Bootlogik getrennt.

---

## Sicherheitskritische Aktionen

Die gemeinsame Control-Infrastruktur darf keine sicherheitskritische Autorität allein aus UI-Ereignissen ableiten.

Beispiel:

```text
Button clicked
```

bedeutet nur:

```text
Action requested
```

Die eigentliche Operation muss weiterhin durch:

- Policy,
- Berechtigungen,
- Trust,
- Recovery-Regeln

validiert werden.

---

## Layout

Ein Control besitzt eine logische Geometrie.

Mindestens:

```text
x
y
width
height
```

beziehungsweise eine äquivalente Bounds-Repräsentation.

Das Layoutsystem entscheidet über die tatsächliche Positionierung.

Controls dürfen nicht voraussetzen, dass ihre Bildschirmposition fest einkompiliert ist.

---

## Coordinate System

Hit Testing und Rendering müssen dasselbe definierte Koordinatenmodell verwenden.

Transformationen dürfen die logische Interaktionsfläche nicht inkonsistent zur sichtbaren Darstellung machen.

---

## Inhalt

Das Basismodell soll unterschiedliche Control-Inhalte zulassen.

Beispiele:

```text
Text
Icon
Image
Composite Content
```

Ein Control-Typ darf die zulässigen Inhalte weiter einschränken.

---

## Styling

Darstellung wird über definierte Style-Eigenschaften beziehungsweise Theme-Ressourcen bestimmt.

Ein Control darf nicht für jeden Zustand vollständig eigene Renderinglogik duplizieren.

Beispiel:

```text
Button
 ├── Normal Style
 ├── Hover Style
 ├── Pressed Style
 ├── Focus Style
 └── Disabled Style
```

Die konkrete Style-Architektur wird separat spezifiziert.

---

## Boot-UI-Theme

Controls müssen sich in ein gemeinsames Boot-UI-Theme integrieren lassen.

Dadurch können zentrale Eigenschaften wie:

- Abstände,
- Schrift,
- Rahmen,
- Ecken,
- Transparenz,
- Fokusindikatoren

konsistent verwaltet werden.

---

## Rendering

Das Control-Modell ist rendererunabhängig.

Mögliche Backends können sein:

```text
Framebuffer
Software Renderer
future accelerated renderer
```

Ein Control soll seine Semantik nicht aufgrund des verwendeten Renderbackends ändern.

---

## Invalidierung

Ändert sich ein visuell relevanter Control-State, muss eine Neudarstellung angefordert werden können.

Beispiel:

```text
Normal
  ↓
Hovered
  ↓
Invalidate
```

Das Control entscheidet dabei nicht zwingend selbst über den tatsächlichen Renderzeitpunkt.

---

## Ereignismodell

Controls erhalten strukturierte Eingabe- und Lifecycle-Ereignisse.

Beispiele:

```text
FocusGained
FocusLost
PointerEnter
PointerLeave
PointerDown
PointerUp
KeyDown
KeyUp
Activate
```

Nicht unterstützte Ereignisse dürfen ignoriert werden.

---

## Event Routing

Die Boot-UI darf ein einfaches hierarchisches Event-Routing zwischen:

```text
Parent
  ↓
Child
```

unterstützen.

Das Modell soll jedoch bewusst klein bleiben und keine vollständige Desktop-UI-Event-Infrastruktur voraussetzen.

---

## Parent-Child-Beziehungen

Controls dürfen Teil hierarchischer UI-Strukturen sein.

Beispiel:

```text
Dialog
  ├── Text
  ├── List
  │    ├── ListItem
  │    └── ListItem
  └── Button
```

Parent-Child-Beziehungen dienen insbesondere:

- Layout,
- Sichtbarkeit,
- Fokus,
- Ereignisrouting,
- Lifecycle.

---

## Lifecycle

Ein Control durchläuft einen einfachen Lifecycle.

Konzeptionell:

```text
Created
   ↓
Attached
   ↓
Active
   ↓
Detached
   ↓
Destroyed
```

Temporäre UI-Elemente wie Dialoge müssen Ressourcen beim Entfernen sauber freigeben.

---

## Ressourcen

Controls dürfen Ressourcen wie:

- Textlayouts,
- Icons,
- Bilder,
- Rendering-Caches

referenzieren.

Die eigentlichen Ressourcen werden über den Boot Resource Manager verwaltet.

Controls sollen große Ressourcen nicht unnötig duplizieren.

---

## Text

Textdarstellung erfolgt über die gemeinsame Boot-Text-Infrastruktur.

Controls implementieren keine eigenen separaten Font- oder Unicode-Renderer.

---

## Accessibility

Das Control-Basismodell unterstützt mindestens semantische Informationen wie:

```text
role
name
state
value
```

soweit für das jeweilige Control relevant.

Beispiele:

```text
Role: Button
Name: Recovery starten

Role: ListItem
Name: NovaDOS
Selected: true
```

Dadurch bleibt die UI-Semantik von der rein visuellen Darstellung getrennt.

---

## Reduced Motion

Controls dürfen Animationen besitzen.

Sie müssen jedoch mit der Boot-UI-Einstellung für reduzierte Bewegung kompatibel sein.

Die Funktion eines Controls darf nicht von einer Animation abhängen.

---

## Fehlerzustände

Ein Control darf nicht durch ungültige Eingabe oder ungültigen Zustand die gesamte Bootoberfläche destabilisieren.

Unbekannte beziehungsweise ungültige visuelle Zustände sollen auf einen sicheren Baseline-State zurückfallen.

---

## Performance

Das Control-Modell muss für eine Bootumgebung ressourcenschonend bleiben.

Insbesondere sollen vermieden werden:

- unnötige Heap-Allokationen pro Frame,
- permanente vollständige Neuberechnung,
- große Objektgraphen,
- unnötige Kopien von Ressourcen.

---

## Determinismus

Bei gleichem:

- Control-State,
- Input,
- Theme,
- Layout

muss das sichtbare und semantische Verhalten deterministisch sein.

Adaptive oder KI-basierte Entscheidungen sind für grundlegende Bedienelemente nicht erforderlich.

---

## Normative Anforderungen

1. Alle interaktiven Boot-UI-Controls MÜSSEN auf einem gemeinsamen Basismodell aufbauen.
2. Das Basismodell MUSS mindestens Sichtbarkeit, Aktivierung, Fokus und grundlegenden Interaktionszustand abbilden.
3. `Focused`, `Selected`, `Hovered` und `Pressed` MÜSSEN logisch unterscheidbar sein.
4. Deaktivierte Controls DÜRFEN keine normale Aktivierungsaktion ausführen.
5. Unsichtbare Controls DÜRFEN NICHT am normalen Hit Testing teilnehmen.
6. Unsichtbare oder deaktivierte Controls DÜRFEN keinen ungültigen aktiven Fokus behalten.
7. Die Boot-UI MUSS ohne Pointer vollständig bedienbar sein.
8. Fokusnavigation MUSS deterministisch sein.
9. Fokus MUSS visuell erkennbar sein.
10. Maus-, Touch- und Tastaturaktivierung SOLLEN dieselbe semantische Control-Aktion auslösen können.
11. Control-Aktivierung MUSS von der eigentlichen Bootoperation getrennt sein.
12. UI-Aktivierung DARF keine Sicherheits- oder Trustprüfung ersetzen.
13. Rendering und Hit Testing MÜSSEN ein konsistentes Koordinatenmodell verwenden.
14. Controls MÜSSEN unterschiedliche visuelle Zustände darstellen können.
15. Styling SOLL zentral über gemeinsame Style- und Theme-Ressourcen erfolgen.
16. Die Control-Semantik DARF NICHT vom verwendeten Renderbackend abhängen.
17. Visuelle Zustandsänderungen MÜSSEN Rendering-Invalidierung auslösen können.
18. Controls MÜSSEN strukturierte Eingabeereignisse verarbeiten können.
19. Parent-Child-Beziehungen MÜSSEN unterstützt werden.
20. Entfernte Controls MÜSSEN ihre kontrollierten Ressourcen freigeben beziehungsweise Referenzen lösen.
21. Textdarstellung MUSS die gemeinsame Boot-Text-Infrastruktur verwenden.
22. Ressourcen SOLLEN über den Boot Resource Manager geteilt werden.
23. Controls SOLLEN semantische Accessibility-Informationen bereitstellen.
24. Die Funktion eines Controls DARF NICHT von Animationen abhängen.
25. Reduced-Motion-Einstellungen MÜSSEN unterstützt werden.
26. Ungültige Control-Zustände DÜRFEN die Bootoberfläche NICHT unkontrolliert destabilisieren.
27. Das Control-Modell SOLL unnötige Allokationen und Ressourcenverdopplung vermeiden.
28. Grundlegendes Control-Verhalten MUSS ohne KI funktionieren.
29. Bei identischem Zustand und identischer Eingabe MUSS das Control-Verhalten deterministisch sein.
30. Spezifische Control-Typen DÜRFEN das Basismodell erweitern, aber dessen gemeinsame Semantik NICHT widersprüchlich neu definieren.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliches Verhalten aller Boot-UI-Bedienelemente,
- weniger duplizierte Eingabe- und Fokuslogik,
- einfachere Implementierung neuer Controls,
- gemeinsame Accessibility- und Theme-Regeln,
- konsistente Tastatur-, Maus- und Touchbedienung,
- einfachere Tests,
- geringere Kopplung zwischen UI und Bootlogik.

### Negative Konsequenzen

- das gemeinsame Basismodell muss langfristig stabil gehalten werden,
- spezielle Controls müssen sich an gemeinsame Lifecycle- und Fokusregeln halten,
- zu viele Eigenschaften im Basismodell könnten unnötige Komplexität erzeugen.

Daher bleibt das Basismodell bewusst klein.

---

## Verworfene Alternativen

### Jedes Control vollständig unabhängig implementieren

Verworfen.

Dies würde Fokus-, Eingabe-, State- und Renderinglogik mehrfach duplizieren.

### Vollständiges Desktop-UI-Framework im Bootloader

Verworfen.

Die Bootumgebung benötigt ein deutlich kleineres und kontrollierbareres Modell.

### Darstellung und Bootaktion direkt koppeln

Verworfen.

Ein UI-Ereignis darf keine sicherheitskritische Bootoperation ungeprüft ausführen.

### Nur Mausbedienung

Verworfen.

Recovery und Bootmenüs müssen auch ohne funktionierenden Pointer nutzbar sein.

### Fokus und Auswahl als derselbe Zustand

Verworfen.

Beide Konzepte besitzen unterschiedliche Semantik.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0008_Recovery_als_First_Class_Bootmodus`
- `ADR-BOOT-0011_NovaDOS_als_Offline_Recovery_Umgebung`
- Boot-Rendering-Architektur
- Boot-Layout-Architektur
- Boot-Input-Architektur
- Boot-Text-Architektur
- Boot-Resource-Manager
- Boot-Accessibility-Regeln

---

## Zugehörige NPSPECs

- `NPSPEC-BOOTCONTROL-BASE-0001`
- `NPSPEC-BOOTCONTROL-STATE-0001`
- `NPSPEC-BOOTCONTROL-FOCUS-0001`
- `NPSPEC-BOOTCONTROL-INPUT-0001`
- `NPSPEC-BOOTCONTROL-EVENT-0001`
- `NPSPEC-BOOTCONTROL-STYLE-0001`
- `NPSPEC-BOOTCONTROL-BUTTON-0001`
- `NPSPEC-BOOTCONTROL-LISTITEM-0001_List_Item`
- `NPSPEC-BOOTCONTROL-CHECKBOX-0001`
- `NPSPEC-BOOTCONTROL-RADIOBUTTON-0001`
- `NPSPEC-BOOTCONTROL-TEXTINPUT-0001`
- `NPSPEC-BOOTCONTROL-ACCESSIBILITY-0001`
- `NPSPEC-BOOTCONTROL-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Fokusgewinn und Fokusverlust,
- deterministische Tastaturnavigation,
- Maus-Hover,
- Pointer-Press und Release,
- Aktivierung über Enter,
- Aktivierung über Space,
- Aktivierung über Pointer,
- deaktiviertes Control,
- unsichtbares Control,
- ausgewähltes Control ohne Fokus,
- fokussiertes Control ohne Auswahl,
- Parent-Child-Struktur,
- Control-Entfernung während UI-Laufzeit,
- Themewechsel,
- Reduced Motion,
- Renderer-Invalidierung,
- gemeinsame Textressourcen,
- Accessibility-Rolle und -Name,
- verhindert ausgeführte Aktion ohne erforderliche Bootautorisierung.

---

## Ergebnis

Die NovaOS-Bootoberfläche verwendet ein gemeinsames, kleines Control-Modell als Grundlage aller interaktiven Bedienelemente.

Das Architekturmodell lautet:

```text
Input
  ↓
Control
  ↓
State
  ↓
Semantic Action
  ↓
Policy / Authorization
  ↓
Boot Operation

Control State
  ↓
Visual State
  ↓
Renderer
```

Damit werden Eingabe, Zustand, Darstellung und eigentliche Bootlogik sauber voneinander getrennt.

Die zentrale Architekturregel lautet:

```text
Share control semantics.

Separate state from rendering.

Separate interaction from authority.

Keep the boot UI small and deterministic.
```