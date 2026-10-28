# NPSPEC-UI-LAYOUT-0003 – Nova Stack Layout

**Status:** Angenommen  
**Bereich:** UI / Layout / Stack  
**System:** NovaOS

## 1. Zweck

Nova Stack Layout definiert die lineare Anordnung nativer UI-Elemente innerhalb von NovaOS.

Ziel ist ein einfaches, flexibles und ressourcenschonendes Layoutsystem, das Elemente horizontal oder vertikal anordnet.

Nova Stack Layout ist Bestandteil der Nova Layout Engine und verwendet deren Measure- und Arrange-Verfahren.

## 2. Architektur

Nova Stack Layout umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Stack Container | Verwaltung der untergeordneten UI-Elemente |
| Orientation Resolver | Festlegung der Anordnungsrichtung |
| Stack Measure Handler | Berechnung des benötigten Platzes |
| Stack Arrange Handler | Positionierung der Elemente |
| Spacing Resolver | Verwaltung der Elementabstände |

Die Layoutberechnung erfolgt unabhängig vom Renderbackend.

## 3. Layoutmodell

Ein Stack ordnet seine untergeordneten Elemente entlang einer Hauptachse an.

Unterstützte Ausrichtungen:

- **Vertical:** Elemente werden untereinander angeordnet.
- **Horizontal:** Elemente werden nebeneinander angeordnet.

Die jeweils andere Achse bildet die Querachse.

Die Anordnung erfolgt standardmäßig in der Reihenfolge der untergeordneten Elemente.

Ein Stack führt keinen automatischen Zeilen- oder Spaltenumbruch durch.

## 4. Eigenschaften

Nova Stack Layout unterstützt mindestens:

| Eigenschaft | Beschreibung |
|---|---|
| Orientation | Horizontale oder vertikale Anordnung |
| Spacing | Abstand zwischen benachbarten Elementen |
| Padding | Innenabstand des Containers |
| HorizontalAlignment | Horizontale Ausrichtung |
| VerticalAlignment | Vertikale Ausrichtung |
| FlowDirection | Logische Anordnungsrichtung |
| ClipToBounds | Begrenzung der Darstellung auf den Container |

Untergeordnete Elemente können eigene Größen, Außenabstände und Ausrichtungen besitzen.

Negative Abstände DÜRFEN nur verwendet werden, wenn das Property System sie ausdrücklich unterstützt.

## 5. Deklarative Definition

Stack Layouts werden über das Nova Declarative UI Model beschrieben.

Beispiel einer vertikalen Anordnung:

```xml
<StackPanel Orientation="Vertical"
            Spacing="12"
            Padding="16">

    <TextBlock Text="NovaOS Einstellungen" />

    <Button Content="System" />

    <Button Content="Darstellung" />

    <Button Content="Netzwerk" />

</StackPanel>
```

Beispiel einer horizontalen Anordnung:

```xml
<StackPanel Orientation="Horizontal"
            Spacing="8">

    <Button Content="Zurück" />

    <Button Content="Weiter" />

    <Button Content="Abbrechen" />

</StackPanel>
```

Die Definition beschreibt ausschließlich die Anordnung.

Die tatsächliche Berechnung erfolgt durch die Nova Layout Engine.

## 6. Measure-Verfahren

Während der Measure-Phase wird der benötigte Platz aller untergeordneten Elemente ermittelt.

Dabei gilt:

- Entlang der Hauptachse werden die benötigten Größen addiert.
- Zwischenräume werden anhand der Spacing-Eigenschaft berücksichtigt.
- Entlang der Querachse wird die größte benötigte Ausdehnung ermittelt.
- Innen- und Außenabstände werden berücksichtigt.
- Mindest- und Maximalgrößen werden angewendet.

Die Measure-Phase MUSS reproduzierbare Ergebnisse erzeugen.

Bei unbegrenztem Platz entlang der Hauptachse MÜSSEN untergeordnete Elemente entsprechend ihren Größenanforderungen gemessen werden.

## 7. Arrange-Verfahren

Während der Arrange-Phase werden die endgültigen Positionen und Größen berechnet.

Dabei gilt:

1. Die Anordnung beginnt am definierten Ursprung des Containers.
2. Jedes Element erhält seinen zugewiesenen Bereich.
3. Die Position entlang der Hauptachse wird fortlaufend aktualisiert.
4. Zwischen benachbarten Elementen wird Spacing berücksichtigt.
5. Die Ausrichtung entlang der Querachse wird angewendet.

Die Berechnung MUSS unabhängig vom Renderbackend erfolgen.

## 8. Größenverhalten

Nova Stack Layout unterstützt:

- Feste Elementgrößen.
- Automatische Elementgrößen.
- Mindest- und Maximalgrößen.
- Ausrichtung innerhalb der Querachse.
- Größenanpassung an Inhalte.

Ein Stack verteilt verbleibenden Platz entlang seiner Hauptachse standardmäßig nicht proportional.

Für proportionale Größenverteilung SOLL Nova Grid Layout oder ein entsprechender flexibler Layoutcontainer verwendet werden.

Bei Platzmangel DÜRFEN Elemente über den verfügbaren Bereich hinausragen.

Die tatsächliche Sichtbarkeit richtet sich nach Clipping und übergeordneten Containern.

## 9. Abstände und Ausrichtung

Die Abstände ergeben sich aus:

- Padding des Stack-Containers.
- Margin der untergeordneten Elemente.
- Spacing zwischen benachbarten Elementen.

Spacing wird ausschließlich zwischen angeordneten Elementen angewendet.

Vor dem ersten und nach dem letzten Element wird kein zusätzliches Spacing eingefügt.

Die Ausrichtung entlang der Querachse erfolgt anhand der jeweiligen Elementeigenschaften.

## 10. Anordnungsrichtung

Nova Stack Layout unterstützt unterschiedliche logische Anordnungsrichtungen.

Dabei gilt:

- Die Orientierung bestimmt die Hauptachse.
- FlowDirection berücksichtigt die Schreibrichtung der Benutzeroberfläche.
- Rechts-nach-links-Oberflächen MÜSSEN unterstützt werden können.
- Die logische Reihenfolge der Elemente bleibt unabhängig von ihrer visuellen Position erhalten.

Die tatsächliche Anordnung MUSS eindeutig definiert sein.

## 11. Dynamische Änderungen

Nova Stack Layout unterstützt Änderungen zur Laufzeit.

Dazu gehören:

- Hinzufügen und Entfernen von Elementen.
- Änderungen der Elementreihenfolge.
- Wechsel der Orientierung.
- Änderungen von Spacing und Padding.
- Änderungen von Elementgrößen.
- Änderungen der Sichtbarkeit.

Änderungen MÜSSEN gezielte Layoutinvalidierungen auslösen.

Unveränderte Berechnungsergebnisse SOLLEN wiederverwendet werden.

## 12. Performance und Caching

Nova Stack Layout verwendet die Caching- und Invalidierungsmechanismen der Nova Layout Engine.

Dabei gilt:

- Unveränderte Elementgrößen SOLLEN zwischengespeichert werden.
- Änderungen einzelner Elemente SOLLEN nur notwendige Neuberechnungen auslösen.
- Mehrere Änderungen DÜRFEN zusammengefasst werden.
- Nicht sichtbare Elemente SOLLEN keine unnötigen Renderoperationen verursachen.

Bei sehr großen Elementmengen SOLLEN virtualisierte Container verwendet werden können.

## 13. Fehlerbehandlung

Nova Stack Layout MUSS ungültige Layoutbedingungen kontrolliert behandeln.

Dazu gehören:

- Ungültige Orientierungswerte.
- Nicht berechenbare Elementgrößen.
- Widersprüchliche Größenbeschränkungen.
- Ungültige Abstände.
- Ressourcenüberschreitungen.

Fehlerhafte Stack-Definitionen DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

## 14. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein natives Stack Layout bereitstellen.
- **UI-002:** Stack Layout MUSS horizontale und vertikale Anordnung unterstützen.
- **UI-003:** Die Anordnung MUSS anhand einer definierten Elementreihenfolge erfolgen.
- **UI-004:** Stack Layout MUSS das Measure- und Arrange-Verfahren verwenden.
- **UI-005:** Feste und automatische Elementgrößen MÜSSEN unterstützt werden.
- **UI-006:** Mindest- und Maximalgrößen MÜSSEN berücksichtigt werden.
- **UI-007:** Padding, Margin und Spacing MÜSSEN unterstützt werden.
- **UI-008:** Elemente MÜSSEN entlang der Querachse ausgerichtet werden können.
- **UI-009:** Unterschiedliche logische Schreibrichtungen MÜSSEN unterstützt werden können.
- **UI-010:** Dynamische Änderungen MÜSSEN unterstützt werden.
- **UI-011:** Layoutänderungen MÜSSEN gezielte Invalidierungen auslösen.
- **UI-012:** Unveränderte Berechnungsergebnisse SOLLEN wiederverwendet werden.
- **UI-013:** Stack Layout MUSS unabhängig vom Renderbackend funktionieren.
- **UI-014:** Layoutberechnungen MÜSSEN deterministisch erfolgen.
- **UI-015:** Ungültige Layoutbedingungen MÜSSEN kontrolliert behandelt werden.
- **UI-016:** Nova Stack Layout MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 15. Integration

- **Nova Layout Engine:** Measure, Arrange und Layoutinvalidierung.
- **Nova Declarative UI Model:** Deklarative Stack-Definitionen.
- **Nova UI Runtime:** Verwaltung und Aktualisierung.
- **Nova UI Property System:** Layoutrelevante Eigenschaften.
- **Nova UI Control Framework:** Stack-Container und untergeordnete Controls.
- **Nova UI Component Lifecycle:** Dynamische Elemente.
- **Nova UI Renderer Abstraction:** Darstellung der angeordneten Elemente.
- **Nova UI Threading Model:** Konsistente Layoutberechnung.

## 16. Ergebnis

NovaOS erhält ein natives, einfaches und performantes Stack Layout zur linearen Anordnung von UI-Elementen.

Horizontale und vertikale Layouts können flexibel kombiniert und dynamisch verändert werden.

Durch gezielte Invalidierungen und die Wiederverwendung gültiger Berechnungsergebnisse werden unnötige Layoutoperationen vermieden.

**Grundsatz:** Nova Stack Layout ordnet Elemente entlang einer definierten Achse an und berechnet nur das neu, was sich durch veränderte Layoutbedingungen tatsächlich ändern muss.