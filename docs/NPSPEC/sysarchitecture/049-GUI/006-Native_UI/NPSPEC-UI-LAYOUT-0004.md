# NPSPEC-UI-LAYOUT-0004 – Nova Dock Layout

**Status:** Angenommen  
**Bereich:** UI / Layout / Dock  
**System:** NovaOS

## 1. Zweck

Nova Dock Layout definiert die Anordnung nativer UI-Elemente entlang der Seiten eines übergeordneten Containers.

Ziel ist ein flexibles, deterministisches und ressourcenschonendes Layoutsystem für Benutzeroberflächen mit festen Randbereichen und einem verbleibenden Inhaltsbereich.

Nova Dock Layout ist Bestandteil der Nova Layout Engine und verwendet deren Measure- und Arrange-Verfahren.

## 2. Architektur

Nova Dock Layout umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Dock Container | Verwaltung der untergeordneten UI-Elemente |
| Dock Resolver | Bestimmung der Andockposition |
| Dock Measure Handler | Berechnung des benötigten Platzes |
| Dock Arrange Handler | Positionierung der Elemente |
| Remaining Space Resolver | Verwaltung des verbleibenden Layoutbereichs |

Die Layoutberechnung erfolgt unabhängig vom verwendeten Renderbackend.

## 3. Layoutmodell

Ein Dock-Container ordnet seine untergeordneten Elemente an den Seiten des verfügbaren Bereichs an.

Unterstützte Andockpositionen:

- **Left:** Linker Rand.
- **Top:** Oberer Rand.
- **Right:** Rechter Rand.
- **Bottom:** Unterer Rand.

Jedes angeordnete Element reduziert den für nachfolgende Elemente verfügbaren Bereich.

Die Verarbeitung erfolgt standardmäßig in der Reihenfolge der untergeordneten Elemente.

Das letzte Element kann optional den gesamten verbleibenden Bereich ausfüllen.

## 4. Eigenschaften

Nova Dock Layout unterstützt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Dock | Andockposition eines Elements |
| LastChildFill | Letztes Element füllt den verbleibenden Bereich |
| Padding | Innenabstand des Containers |
| HorizontalAlignment | Horizontale Ausrichtung |
| VerticalAlignment | Vertikale Ausrichtung |
| FlowDirection | Logische Schreibrichtung |
| ClipToBounds | Begrenzung der Darstellung auf den Container |

Untergeordnete Elemente können eigene Größen, Außenabstände und Ausrichtungen besitzen.

Die Dock-Eigenschaft wird als zugeordnete Eigenschaft des jeweiligen Elements definiert.

## 5. Deklarative Definition

Dock Layouts werden über das Nova Declarative UI Model beschrieben.

Beispiel:

```xml
<DockPanel LastChildFill="True"
           Padding="8">

    <ToolBar DockPanel.Dock="Top"
             Height="40" />

    <StatusBar DockPanel.Dock="Bottom"
               Height="24" />

    <StackPanel DockPanel.Dock="Left"
                Width="200">
        <Button Content="Start" />
        <Button Content="Einstellungen" />
    </StackPanel>

    <ContentControl />
</DockPanel>
```

In diesem Beispiel werden zunächst die obere Werkzeugleiste, anschließend die Statusleiste und danach der linke Navigationsbereich angeordnet.

Das letzte Element erhält den verbleibenden Inhaltsbereich.

Die tatsächliche Größenberechnung erfolgt durch die Nova Layout Engine.

## 6. Measure-Verfahren

Während der Measure-Phase ermittelt der Dock-Container den benötigten Platz seiner untergeordneten Elemente.

Dabei gilt:

1. Der verfügbare Bereich wird anhand der Containerbedingungen bestimmt.
2. Elemente werden in ihrer definierten Reihenfolge gemessen.
3. Links und rechts angedockte Elemente beanspruchen horizontale Ausdehnung.
4. Oben und unten angedockte Elemente beanspruchen vertikale Ausdehnung.
5. Der verbleibende Bereich wird für nachfolgende Elemente berücksichtigt.
6. Mindest- und Maximalgrößen werden angewendet.

Die Berechnung MUSS reproduzierbare Ergebnisse erzeugen.

Bei unbegrenztem verfügbarem Platz MÜSSEN die Größenanforderungen der untergeordneten Elemente nach definierten Regeln ausgewertet werden.

## 7. Arrange-Verfahren

Während der Arrange-Phase werden die endgültigen Positionen und Größen berechnet.

Dabei gilt:

- Links angedockte Elemente werden am linken Rand des verbleibenden Bereichs angeordnet.
- Rechts angedockte Elemente werden am rechten Rand angeordnet.
- Oben angedockte Elemente werden am oberen Rand angeordnet.
- Unten angedockte Elemente werden am unteren Rand angeordnet.
- Nach jeder Anordnung wird der verbleibende Bereich entsprechend reduziert.
- Das letzte Element kann den verbleibenden Bereich vollständig ausfüllen.

Die Berechnung MUSS unabhängig von der grafischen Zeichenreihenfolge erfolgen.

## 8. LastChildFill

Die Eigenschaft `LastChildFill` bestimmt das Verhalten des letzten untergeordneten Elements.

### Aktiviert

Das letzte Element erhält den verbleibenden Layoutbereich.

Seine Dock-Eigenschaft wird für die Platzzuweisung ignoriert.

### Deaktiviert

Das letzte Element wird entsprechend seiner Dock-Eigenschaft angeordnet.

Ein nicht belegter Restbereich bleibt bestehen.

Der Standardwert von `LastChildFill` ist `True`.

Die Eigenschaft MUSS zur Laufzeit änderbar sein.

## 9. Größenverhalten

Nova Dock Layout unterstützt:

- Feste Elementgrößen.
- Automatische Elementgrößen.
- Mindest- und Maximalgrößen.
- Größenanpassung an Inhalte.
- Ausrichtung innerhalb zugewiesener Bereiche.

Die Größe eines angedockten Elements bestimmt, wie viel Platz es entlang der jeweiligen Andockachse beansprucht.

Der verbleibende Bereich DARF nicht negativ werden.

Bei Platzmangel MÜSSEN Größenbeschränkungen und Überlaufbedingungen kontrolliert behandelt werden.

## 10. Reihenfolge und Überlagerung

Die Reihenfolge der Elemente beeinflusst die Aufteilung des verfügbaren Platzes.

Beispielsweise kann ein zuerst angeordneter oberer Bereich die gesamte Containerbreite beanspruchen, während ein später angeordneter linker Bereich nur die verbleibende Höhe erhält.

Dabei gilt:

- Die Layoutreihenfolge MUSS eindeutig definiert sein.
- Die Zeichenreihenfolge wird unabhängig davon durch das UI-System bestimmt.
- Mehrere Elemente DÜRFEN dieselbe Andockposition verwenden.
- Überlagerungen DÜRFEN durch ausdrücklich definierte Layout- und Darstellungseigenschaften entstehen.

Dock Layout führt keine automatische Fensterverwaltung oder interaktive Fensterandockung durch.

Diese Funktionen gehören zum Nova Window Manager beziehungsweise zu darauf aufbauenden UI-Komponenten.

## 11. Dynamische Änderungen

Nova Dock Layout unterstützt Änderungen zur Laufzeit.

Dazu gehören:

- Hinzufügen und Entfernen von Elementen.
- Änderungen der Dock-Eigenschaft.
- Änderungen der Elementreihenfolge.
- Änderungen von LastChildFill.
- Größenänderungen.
- Änderungen von Padding und Margin.
- Änderungen der Sichtbarkeit.

Änderungen MÜSSEN gezielte Layoutinvalidierungen auslösen.

Unveränderte Berechnungsergebnisse SOLLEN wiederverwendet werden.

## 12. Performance und Caching

Nova Dock Layout verwendet die Caching- und Invalidierungsmechanismen der Nova Layout Engine.

Dabei gilt:

- Unveränderte Elementgrößen SOLLEN zwischengespeichert werden.
- Änderungen einzelner Elemente SOLLEN nur notwendige Neuberechnungen auslösen.
- Abhängige nachfolgende Elemente MÜSSEN bei Änderungen des verbleibenden Bereichs berücksichtigt werden.
- Mehrere Änderungen DÜRFEN zusammengefasst werden.
- Unveränderte UI-Bereiche DÜRFEN keine unnötigen Renderoperationen verursachen.

Die Layoutberechnung MUSS ohne permanente Aktualisierungsschleife funktionieren.

## 13. Skalierung und Schreibrichtung

Nova Dock Layout verwendet logische UI-Koordinaten.

Dabei gilt:

- Unterschiedliche DPI-Skalierungen werden durch die Nova Layout Engine berücksichtigt.
- Die Größenberechnung bleibt unabhängig von der physischen Bildschirmauflösung.
- Rechts-nach-links-Oberflächen MÜSSEN unterstützt werden können.
- Physische Andockpositionen und logische Schreibrichtung MÜSSEN eindeutig voneinander unterscheidbar sein.

Die Änderung der Schreibrichtung DARF keine unkontrollierten Layoutänderungen verursachen.

## 14. Fehlerbehandlung

Nova Dock Layout MUSS ungültige Layoutbedingungen kontrolliert behandeln.

Dazu gehören:

- Ungültige Dock-Werte.
- Negative oder nicht berechenbare Größen.
- Widersprüchliche Größenbeschränkungen.
- Ungültige Abstände.
- Ressourcenüberschreitungen.

Fehlerhafte Dock-Definitionen DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

## 15. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein natives Dock Layout bereitstellen.
- **UI-002:** Dock Layout MUSS die Positionen Left, Top, Right und Bottom unterstützen.
- **UI-003:** Die Anordnung MUSS anhand einer definierten Elementreihenfolge erfolgen.
- **UI-004:** Dock Layout MUSS das Measure- und Arrange-Verfahren verwenden.
- **UI-005:** Jedes angeordnete Element MUSS den verbleibenden Layoutbereich entsprechend seiner Platzzuweisung reduzieren.
- **UI-006:** LastChildFill MUSS unterstützt werden.
- **UI-007:** Feste und automatische Elementgrößen MÜSSEN unterstützt werden.
- **UI-008:** Mindest- und Maximalgrößen MÜSSEN berücksichtigt werden.
- **UI-009:** Padding, Margin und Ausrichtung MÜSSEN unterstützt werden.
- **UI-010:** Mehrere Elemente MÜSSEN dieselbe Andockposition verwenden können.
- **UI-011:** Dynamische Änderungen MÜSSEN unterstützt werden.
- **UI-012:** Änderungen MÜSSEN gezielte Layoutinvalidierungen auslösen.
- **UI-013:** Unveränderte Berechnungsergebnisse SOLLEN wiederverwendet werden.
- **UI-014:** Dock Layout MUSS unabhängig vom Renderbackend funktionieren.
- **UI-015:** Layoutberechnungen MÜSSEN deterministisch erfolgen.
- **UI-016:** Ungültige Layoutbedingungen MÜSSEN kontrolliert behandelt werden.
- **UI-017:** Nova Dock Layout MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 16. Integration

- **Nova Layout Engine:** Measure, Arrange und Layoutinvalidierung.
- **Nova Declarative UI Model:** Deklarative Dock-Definitionen.
- **Nova UI Runtime:** Verwaltung und Aktualisierung.
- **Nova UI Property System:** Dock-Eigenschaften.
- **Nova UI Control Framework:** Dock-Container und untergeordnete Controls.
- **Nova UI Component Lifecycle:** Dynamische Elemente.
- **Nova UI Renderer Abstraction:** Darstellung der angeordneten Elemente.
- **Nova UI Threading Model:** Konsistente Layoutberechnung.
- **Nova Window Manager:** Fensterverwaltung und darauf aufbauende Andockfunktionen.

## 17. Ergebnis

NovaOS erhält ein natives und performantes Dock Layout zur Anordnung von UI-Elementen entlang der Containerseiten.

Werkzeugleisten, Navigationsbereiche, Statusleisten und zentrale Inhaltsbereiche können flexibel miteinander kombiniert werden.

Durch gezielte Invalidierungen und die Wiederverwendung gültiger Berechnungsergebnisse werden unnötige Layoutoperationen vermieden.

**Grundsatz:** Nova Dock Layout verteilt den verfügbaren Platz schrittweise entlang definierter Containerseiten und überlässt den verbleibenden Bereich den nachfolgenden Elementen.