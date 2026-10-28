# NPSPEC-UI-ANIMATION-0001 – Nova Animation Engine

**Status:** Angenommen  
**Bereich:** UI / Animation  
**System:** NovaOS

## 1. Zweck

Nova Animation Engine definiert das native Animationssystem der NovaOS-Benutzeroberfläche.

Ziel ist die flüssige, deterministische und ressourcenschonende Animation von UI-Eigenschaften, Zustandsübergängen und visuellen Effekten.

Animationen werden unabhängig von der Anwendungslogik beschrieben und nur dann berechnet, wenn tatsächlich eine Animation aktiv ist.

Die Animation Engine arbeitet unabhängig vom verwendeten Renderbackend.

## 2. Architektur

Das Subsystem umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Animation Manager | Verwaltung aktiver Animationen |
| Animation Timeline | Zeitlicher Ablauf einer Animation |
| Animation Scheduler | Planung erforderlicher Aktualisierungen |
| Animation Evaluator | Berechnung animierter Eigenschaftswerte |
| Interpolation Engine | Berechnung von Zwischenwerten |
| Animation Composition Manager | Kombination mehrerer Animationen |
| Animation State Manager | Verwaltung des Animationszustands |
| Animation Clock | Bereitstellung einer monotonen Zeitbasis |

Die Animation Engine verwendet das Nova UI Property System und die Nova UI Renderer Abstraction.

## 3. Animationsmodell

Eine Animation beschreibt die zeitliche Veränderung einer Eigenschaft.

Eine Animationsdefinition kann enthalten:

- Zielelement.
- Zieleigenschaft.
- Startwert.
- Endwert.
- Dauer.
- Verzögerung.
- Interpolationsverfahren.
- Wiederholungsverhalten.
- Abspielrichtung.
- Abschlussverhalten.

Animationen MÜSSEN unabhängig von ihrer konkreten Darstellung definiert werden können.

## 4. Unterstützte Animationstypen

NovaOS unterstützt folgende Animationstypen:

| Typ | Beschreibung |
|---|---|
| Property Animation | Veränderung einer UI-Eigenschaft |
| Keyframe Animation | Animation über definierte Zwischenwerte |
| Transform Animation | Position, Skalierung und Rotation |
| Color Animation | Übergänge zwischen Farben |
| Opacity Animation | Veränderung der Transparenz |
| Effect Animation | Animation grafischer Effekte |
| State Transition | Übergang zwischen visuellen Zuständen |
| Storyboard | Koordination mehrerer Animationen |

Weitere Animationstypen können über definierte Erweiterungsschnittstellen ergänzt werden.

## 5. Deklarative Animationen

Animationen können im Nova Declarative UI Model definiert werden.

Konzeptionelles Beispiel:

```xml
<Animation
    Target="Panel"
    Property="Opacity"
    From="0"
    To="1"
    Duration="300ms"
    Easing="EaseOut" />
```

Animationen können auch innerhalb von Styles, Control Templates und Visual States definiert werden.

Die konkrete Syntax wird durch das deklarative UI-Schema festgelegt.

## 6. Keyframe Animation

Keyframe Animationen definieren mehrere Werte entlang einer Timeline.

Beispiel:

```xml
<KeyframeAnimation
    Target="Panel"
    Property="Opacity"
    Duration="500ms">

    <Keyframe Time="0ms" Value="0" />
    <Keyframe Time="200ms" Value="0.5" />
    <Keyframe Time="500ms" Value="1" />

</KeyframeAnimation>
```

Dabei gilt:

- Keyframes MÜSSEN zeitlich eindeutig eingeordnet werden.
- Zwischenwerte werden durch definierte Interpolationsverfahren berechnet.
- Unterschiedliche Interpolationen pro Abschnitt DÜRFEN unterstützt werden.
- Ungültige Keyframe-Definitionen MÜSSEN erkannt werden.

## 7. Interpolation und Easing

Nova Animation Engine unterstützt unterschiedliche Interpolationsverfahren.

Dazu gehören:

- Linear.
- EaseIn.
- EaseOut.
- EaseInOut.
- Step.
- Cubic Bezier.
- Spring.

Dabei gilt:

- Interpolationsverfahren MÜSSEN deterministisch ausgewertet werden.
- Nicht interpolierbare Eigenschaften MÜSSEN kontrolliert behandelt werden.
- Benutzerdefinierte Interpolationen DÜRFEN registriert werden.
- Physikalisch inspirierte Animationen MÜSSEN definierte Abbruch- und Stabilitätsbedingungen besitzen.

## 8. Animierbare Eigenschaften

Grundsätzlich können geeignete Eigenschaften des Nova UI Property System animiert werden.

Beispiele:

- Opacity.
- Position.
- Scale.
- Rotation.
- Width und Height.
- Farben.
- Schatten.
- Blur.
- Transformationsmatrizen.

Dabei gilt:

- Animierbare Eigenschaften MÜSSEN eindeutig gekennzeichnet sein.
- Animationen DÜRFEN keine ungültigen Eigenschaftswerte erzeugen.
- Die Animation MUSS die Typregeln der Zieleigenschaft berücksichtigen.
- Nicht animierbare Eigenschaften MÜSSEN kontrolliert zurückgewiesen werden.

## 9. Animationspriorität

Animationen beeinflussen den effektiven Wert einer UI-Eigenschaft.

Dabei gilt:

- Der ursprüngliche Eigenschaftswert bleibt erhalten.
- Animierte Werte werden über das Nova UI Property System bereitgestellt.
- Die Priorität gegenüber Styles, Bindings und lokalen Werten MUSS eindeutig definiert sein.
- Nach Abschluss der Animation wird der effektive Wert gemäß dem definierten Abschlussverhalten bestimmt.

Animationen DÜRFEN Datenbindungen nicht unbeabsichtigt überschreiben.

## 10. Gleichzeitige Animationen

Mehrere Animationen können dieselbe Eigenschaft beeinflussen.

Dabei gilt:

- Konflikte MÜSSEN deterministisch aufgelöst werden.
- Animationen können ersetzt, kombiniert oder überlagert werden.
- Die jeweilige Kompositionsregel MUSS ausdrücklich definiert sein.
- Nicht kompatible Kombinationen MÜSSEN kontrolliert behandelt werden.

Die Animation Engine DARF keine unkontrollierten Eigenschaftskonflikte erzeugen.

## 11. Storyboards

Storyboards koordinieren mehrere Animationen.

Unterstützt werden:

- Parallele Animationen.
- Sequenzielle Animationen.
- Verzögerte Starts.
- Wiederholungen.
- Verschachtelte Animationsgruppen.

Beispiel:

```xml
<Storyboard>

    <Animation
        Target="Panel"
        Property="Opacity"
        From="0"
        To="1"
        Duration="300ms" />

    <Animation
        Target="Panel"
        Property="Scale"
        From="0.95"
        To="1"
        Duration="300ms" />

</Storyboard>
```

Storyboards MÜSSEN als zusammengehörige Animationseinheiten gestartet und beendet werden können.

## 12. Animationssteuerung

Animationen unterstützen folgende Operationen:

- Start.
- Pause.
- Resume.
- Stop.
- Cancel.
- Restart.
- Seek.

Eine Animation kann folgende Zustände besitzen:

| Zustand | Bedeutung |
|---|---|
| Created | Definiert, noch nicht gestartet |
| Running | Wird ausgeführt |
| Paused | Vorübergehend angehalten |
| Completed | Regulär abgeschlossen |
| Cancelled | Kontrolliert abgebrochen |

Zustandsänderungen können über das Nova UI Event System bereitgestellt werden.

## 13. Zeitmodell und Determinismus

Nova Animation Engine verwendet eine monotone Zeitbasis.

Dabei gilt:

- Animationswerte werden anhand der Timeline-Zeit berechnet.
- Die Darstellung darf von der tatsächlichen Bildwiederholrate unabhängig sein.
- Verzögerte Frames DÜRFEN nicht zu einer unkontrollierten Beschleunigung führen.
- Im Deterministic Mode MUSS eine reproduzierbare Zeitsteuerung möglich sein.
- Animationen MÜSSEN auch mit einer simulierten Zeitquelle ausführbar sein.

Die Berechnung eines Animationswerts SOLL bei identischer Definition und identischem Zeitpunkt reproduzierbar sein.

## 14. Frame Scheduling

Nova Animation Engine arbeitet bedarfsgesteuert.

Dabei gilt:

- Ohne aktive Animationen werden keine Animationsframes angefordert.
- Während aktiver Animationen werden nur erforderliche Aktualisierungen geplant.
- Die Frameplanung SOLL mit der Bildwiederholrate des Ausgabegeräts synchronisiert werden.
- Nicht sichtbare Animationen SOLLEN pausiert oder reduziert werden können.
- Die Animation Engine DARF keine permanente Neuzeichnung der gesamten Benutzeroberfläche erzwingen.

Die Frameplanung erfolgt in Zusammenarbeit mit dem Compositor und dem Render-System.

## 15. Damage Tracking

Animationen MÜSSEN mit dem Damage Tracking von NovaOS zusammenarbeiten.

Dabei gilt:

- Nur tatsächlich veränderte Bereiche werden invalidiert.
- Unveränderte UI-Elemente behalten ihre gültigen Renderinformationen.
- Transformationsänderungen berücksichtigen die vorherige und neue sichtbare Fläche.
- Effekte berücksichtigen ihren tatsächlichen Einflussbereich.
- Mehrere Invalidierungen SOLLEN zusammengefasst werden.

Eine Animation DARF keine vollständige Neuzeichnung erzwingen, sofern diese technisch nicht erforderlich ist.

## 16. Compositor-Animationen

Geeignete Animationen SOLLEN direkt durch den Compositor ausgeführt werden können.

Dazu gehören insbesondere:

- Translation.
- Skalierung.
- Rotation.
- Transparenz.
- Geeignete grafische Effekte.

Dabei gilt:

- Compositor-Animationen SOLLEN ohne erneute Berechnung des UI-Layouts ausführbar sein.
- Animationsdefinitionen MÜSSEN unabhängig vom Renderbackend bleiben.
- Nicht unterstützte Animationen werden über die reguläre UI-Animation verarbeitet.
- Der Wechsel zwischen Ausführungswegen DARF keine sichtbaren Zustandsfehler verursachen.

## 17. Layoutabhängige Animationen

Animationen von Layout-Eigenschaften können zusätzliche Berechnungen verursachen.

Dabei gilt:

- Änderungen an Width, Height oder anderen Layout-Eigenschaften invalidieren das erforderliche Layout.
- Reine Transformationen SOLLEN ohne vollständige Layoutneuberechnung erfolgen.
- Layoutabhängige Animationen MÜSSEN mit Nova Layout Measurement and Arrangement zusammenarbeiten.
- Unveränderte Layoutbereiche SOLLEN ihre berechneten Werte behalten.

Transformationsanimationen sind gegenüber unnötigen Layoutanimationen zu bevorzugen.

## 18. Visual States und Themewechsel

Nova Animation Engine unterstützt Animationen für:

- Hover.
- Pressed.
- Focused.
- Selected.
- Disabled.
- Loading.
- Themewechsel.
- Fensterübergänge.

Dabei gilt:

- Funktionale Zustandsänderungen erfolgen unabhängig von Animationen.
- Animationen DÜRFEN Zustandsänderungen nicht blockieren.
- Laufende Animationen MÜSSEN bei neuen Zuständen unterbrechbar sein.
- Themewechsel MÜSSEN auch ohne Animation funktionieren.

## 19. Barrierefreiheit

Nova Animation Engine berücksichtigt die Einstellungen von Nova UI Accessibility.

Dabei gilt:

- Reduzierte Animationen MÜSSEN unterstützt werden.
- Animationen MÜSSEN global und anwendungsbezogen deaktivierbar sein können.
- Notwendige Zustandsinformationen MÜSSEN auch ohne Animation erkennbar bleiben.
- Blinkende und stark bewegte Effekte SOLLEN vermieden werden.
- Benutzerpräferenzen besitzen Vorrang vor rein dekorativen Animationen.

## 20. Ressourcenökonomie

Nova Animation Engine MUSS ressourcenschonend arbeiten.

Dabei gilt:

- Animationen werden nur bei tatsächlichem Bedarf berechnet.
- Nicht sichtbare Animationen SOLLEN reduziert werden.
- Unveränderte Animationswerte lösen keine Renderinvalidierung aus.
- GPU-beschleunigte Animationen SOLLEN verwendet werden, wenn dies effizienter ist.
- Bei eingeschränkten Ressourcen können Animationen vereinfacht oder deaktiviert werden.
- Nicht mehr benötigte Animationsressourcen werden freigegeben.

Die Bedienbarkeit besitzt Vorrang vor dekorativen Effekten.

## 21. NovaLang-Integration

Animationen können programmatisch erstellt und gesteuert werden.

Konzeptionelles Beispiel:

```vb
Dim fade As New Animation()

fade.Target = Panel
fade.Property = "Opacity"
fade.From = 0
fade.To = 1
fade.Duration = 300

fade.Start()
```

NovaLang MUSS dieselben Animationsregeln wie das deklarative UI-Modell verwenden.

Die konkrete API wird durch das Nova Framework festgelegt.

## 22. NovaStudio und Logic Graph

NovaStudio unterstützt:

- Visuelle Erstellung von Animationen.
- Timeline-Editor.
- Keyframe-Bearbeitung.
- Auswahl von Easing-Funktionen.
- Storyboard-Editor.
- Vorschau von Animationen.
- Debugging aktiver Animationen.
- Darstellung von Animationen im Logic Graph.

Logic Graph kann Animationen starten, pausieren, abbrechen und auf ihren Abschluss reagieren.

## 23. Sicherheit

Nova Animation Engine unterliegt Nova.Trust.

Dabei gilt:

- Animationen DÜRFEN keine zusätzlichen Capabilities verleihen.
- Animationsdefinitionen MÜSSEN validiert werden.
- Benutzerdefinierte Interpolationen MÜSSEN innerhalb ihres autorisierten Kontexts ausgeführt werden.
- Animationen DÜRFEN keine Sicherheitsgrenzen umgehen.
- Fehlerhafte Animationen DÜRFEN andere UI-Kontexte nicht beeinträchtigen.
- Sicherheitsrelevante UI-Elemente DÜRFEN durch Animationen nicht unzulässig verborgen oder manipuliert werden.

## 24. Normative Anforderungen

- **UI-001:** NovaOS MUSS eine native Animation Engine bereitstellen.
- **UI-002:** Animationen MÜSSEN deklarativ und programmatisch definiert werden können.
- **UI-003:** Property Animationen MÜSSEN unterstützt werden.
- **UI-004:** Keyframe Animationen MÜSSEN unterstützt werden.
- **UI-005:** Unterschiedliche Interpolationsverfahren MÜSSEN verfügbar sein.
- **UI-006:** Animationswerte MÜSSEN typsicher verarbeitet werden.
- **UI-007:** Gleichzeitige Animationen MÜSSEN deterministisch kombiniert werden.
- **UI-008:** Storyboards MÜSSEN unterstützt werden.
- **UI-009:** Animationen MÜSSEN gestartet, pausiert und abgebrochen werden können.
- **UI-010:** Animationen MÜSSEN eine monotone Zeitbasis verwenden.
- **UI-011:** Der Deterministic Mode MUSS reproduzierbare Animationen ermöglichen.
- **UI-012:** Die Frameplanung MUSS bedarfsgesteuert erfolgen.
- **UI-013:** Ohne aktive Animationen DÜRFEN keine unnötigen Animationsframes erzeugt werden.
- **UI-014:** Animationen MÜSSEN gezielte Renderinvalidierungen verwenden.
- **UI-015:** Geeignete Animationen SOLLEN compositorseitig ausführbar sein.
- **UI-016:** Layoutabhängige Animationen MÜSSEN gezielte Layoutinvalidierungen verwenden.
- **UI-017:** Animationen MÜSSEN mit Nova Visual State Styling zusammenarbeiten.
- **UI-018:** Dynamische Themewechsel MÜSSEN animiert werden können.
- **UI-019:** Reduzierte Animationen MÜSSEN unterstützt werden.
- **UI-020:** Animationen MÜSSEN unabhängig vom Renderbackend definiert werden können.
- **UI-021:** NovaStudio MUSS Animationen visuell bearbeiten können.
- **UI-022:** Logic Graph MUSS Animationen steuern können.
- **UI-023:** Animationen MÜSSEN die Sicherheitsregeln von Nova.Trust einhalten.
- **UI-024:** Nova Animation Engine MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 25. Integration

- **Nova UI Runtime:** Verwaltung aktiver Animationen.
- **Nova UI Property System:** Animierbare Eigenschaften.
- **Nova Declarative UI Model:** Deklarative Animationsdefinitionen.
- **Nova UI Threading Model:** Zeitsteuerung und Synchronisation.
- **Nova UI Component Lifecycle:** Verwaltung von Animationsressourcen.
- **Nova Control Framework:** Animation nativer Controls.
- **Nova Control States:** Funktionale Zustandsänderungen.
- **Nova Visual State Styling:** Animierte Zustandsübergänge.
- **Nova Dynamic Theme Switching:** Animierte Themewechsel.
- **Nova Layout Engine:** Layoutabhängige Animationen.
- **Nova UI Renderer Abstraction:** Darstellung animierter Eigenschaften.
- **Nova Render Engine:** Grafische Berechnung.
- **Nova Compositor:** Compositor-Animationen und Damage Tracking.
- **Nova Display Server:** Synchronisation mit Ausgabegeräten.
- **Nova UI Accessibility:** Reduzierte Animationen.
- **Nova UI Event System:** Animationsereignisse.
- **Nova Commands and Actions:** Steuerung von Animationen.
- **NovaLang:** Programmatische Animationen.
- **NovaStudio:** Timeline- und Animationseditor.
- **Nova Logic Graph:** Visuelle Animationssteuerung.
- **Nova.Trust:** Sicherheit und Isolation.

## 26. Ergebnis

NovaOS erhält eine native, deterministische und ressourcenschonende Animation Engine.

UI-Eigenschaften, visuelle Zustände und Übergänge können flüssig animiert werden, ohne die funktionale Logik oder die gesamte Benutzeroberfläche permanent neu berechnen zu müssen.

Durch bedarfsgesteuerte Frameplanung, Damage Tracking und compositorseitige Ausführung geeigneter Animationen bleibt das System auch bei komplexen Oberflächen effizient.

**Grundsatz:** NovaOS animiert nur, was sich tatsächlich bewegt oder verändert – und berechnet nur das, was für den nächsten sichtbaren Zustand erforderlich ist.