# NPSPEC-UI-LAYOUT-0002 – Nova Grid Layout

**Status:** Angenommen  
**Bereich:** UI / Layout / Grid  
**System:** NovaOS

## 1. Zweck

Nova Grid Layout definiert die zeilen- und spaltenbasierte Anordnung nativer UI-Elemente innerhalb von NovaOS.

Ziel ist ein flexibles, deterministisches und ressourcenschonendes Rastersystem, das feste, automatische und proportionale Größen unterstützt.

Nova Grid Layout ist Bestandteil der Nova Layout Engine und verwendet deren Measure- und Arrange-Verfahren.

## 2. Architektur

Nova Grid Layout umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Grid Container | Verwaltung des Rasterlayouts |
| Row Definition | Definition einer Zeile |
| Column Definition | Definition einer Spalte |
| Grid Cell Resolver | Zuordnung von Elementen zu Rasterzellen |
| Grid Size Resolver | Berechnung der Zeilen- und Spaltengrößen |
| Grid Span Resolver | Verarbeitung zellenübergreifender Elemente |

Die Berechnung erfolgt unabhängig vom verwendeten Renderbackend.

## 3. Rastermodell

Ein Grid besteht aus Zeilen und Spalten.

Jedes untergeordnete UI-Element kann folgende Eigenschaften besitzen:

- Row.
- Column.
- RowSpan.
- ColumnSpan.
- HorizontalAlignment.
- VerticalAlignment.
- Margin.

Zeilen und Spalten werden über eigene Definitionen beschrieben.

Nicht ausdrücklich positionierte Elemente verwenden standardmäßig Zeile 0 und Spalte 0.

Mehrere Elemente DÜRFEN dieselbe Rasterzelle belegen.

Ihre Darstellung erfolgt entsprechend der definierten Zeichenreihenfolge.

## 4. Größenarten

Nova Grid Layout unterstützt drei grundlegende Größenarten:

| Größenart | Beschreibung |
|---|---|
| Fixed | Feste Größe in logischen UI-Einheiten |
| Auto | Größe anhand des benötigten Inhalts |
| Star | Proportionaler Anteil des verfügbaren Platzes |

Beispiele:

- `100` – feste Größe.
- `Auto` – automatische Größe.
- `*` – ein proportionaler Anteil.
- `2*` – zwei proportionale Anteile.

Die Größenberechnung MUSS Mindest- und Maximalgrößen berücksichtigen.

## 5. Deklarative Definition

Grid Layouts werden über das Nova Declarative UI Model beschrieben.

Beispiel:

```xml
<Grid>
    <Grid.RowDefinitions>
        <RowDefinition Height="Auto" />
        <RowDefinition Height="*" />
        <RowDefinition Height="40" />
    </Grid.RowDefinitions>

    <Grid.ColumnDefinitions>
        <ColumnDefinition Width="200" />
        <ColumnDefinition Width="*" />
    </Grid.ColumnDefinitions>

    <TextBlock Grid.Row="0"
               Grid.Column="0"
               Grid.ColumnSpan="2"
               Text="NovaOS" />

    <Button Grid.Row="1"
            Grid.Column="0"
            Content="Menü" />

    <TextBlock Grid.Row="1"
               Grid.Column="1"
               Text="Inhalt" />
</Grid>
```

Die Definition beschreibt ausschließlich die Anordnung.

Die tatsächliche Größenberechnung erfolgt durch die Nova Layout Engine.

## 6. Measure-Verfahren

Während der Measure-Phase ermittelt das Grid den benötigten Platz seiner untergeordneten Elemente.

Dabei gilt:

1. Feste Zeilen- und Spaltengrößen werden berücksichtigt.
2. Automatische Größen werden anhand der Inhalte ermittelt.
3. Proportionale Größen werden anhand des verfügbaren Platzes berechnet.
4. Zellenübergreifende Elemente werden berücksichtigt.
5. Mindest- und Maximalgrößen werden angewendet.

Die Berechnung MUSS eine eindeutig definierte Reihenfolge besitzen.

Bei unbegrenztem verfügbarem Platz MÜSSEN proportionale Größen nach einer definierten Regel behandelt werden.

## 7. Arrange-Verfahren

Während der Arrange-Phase werden die endgültigen Zellpositionen und Zellgrößen berechnet.

Dabei gilt:

- Zeilen und Spalten erhalten ihre berechneten Größen.
- Untergeordnete Elemente werden ihren Zellen zugeordnet.
- RowSpan und ColumnSpan werden berücksichtigt.
- Ausrichtungen und Außenabstände werden angewendet.
- Elemente werden innerhalb ihrer zugewiesenen Bereiche angeordnet.

Die Berechnung MUSS unabhängig von der Zeichenreihenfolge erfolgen.

## 8. Proportionale Größenverteilung

Star-Größen verteilen den verbleibenden verfügbaren Platz anhand ihrer Gewichtung.

Beispiel:

```xml
<Grid.ColumnDefinitions>
    <ColumnDefinition Width="*" />
    <ColumnDefinition Width="2*" />
    <ColumnDefinition Width="*" />
</Grid.ColumnDefinitions>
```

Die Spalten erhalten bei ausreichendem verfügbarem Platz das Verhältnis 1:2:1.

Dabei gilt:

- Feste und automatische Größen werden zuerst berücksichtigt.
- Der verbleibende Platz wird proportional verteilt.
- Mindest- und Maximalgrößen besitzen Vorrang.
- Begrenzte Spalten werden bei Bedarf aus der weiteren Verteilung ausgeschlossen.

Negative Größen DÜRFEN nicht entstehen.

## 9. RowSpan und ColumnSpan

UI-Elemente können mehrere Zeilen oder Spalten belegen.

Dabei gilt:

- RowSpan definiert die Anzahl belegter Zeilen.
- ColumnSpan definiert die Anzahl belegter Spalten.
- Der Platzbedarf wird bei der Größenberechnung berücksichtigt.
- Die endgültige Größe ergibt sich aus den beteiligten Zeilen beziehungsweise Spalten.

Zellenübergreifende Elemente DÜRFEN keine unkontrollierten Berechnungszyklen verursachen.

Ungültige Span-Werte MÜSSEN kontrolliert behandelt werden.

## 10. Dynamische Änderungen

Nova Grid Layout unterstützt Änderungen zur Laufzeit.

Dazu gehören:

- Hinzufügen und Entfernen von Zeilen.
- Hinzufügen und Entfernen von Spalten.
- Änderungen der Größenarten.
- Änderungen der Zellzuordnung.
- Änderungen von RowSpan und ColumnSpan.
- Hinzufügen und Entfernen von UI-Elementen.

Änderungen MÜSSEN gezielte Layoutinvalidierungen auslösen.

Unveränderte Rasterbereiche SOLLEN ihre gültigen Berechnungsergebnisse behalten.

## 11. Performance und Caching

Nova Grid Layout verwendet die Caching- und Invalidierungsmechanismen der Nova Layout Engine.

Dabei gilt:

- Unveränderte Zeilen- und Spaltendefinitionen SOLLEN wiederverwendet werden.
- Größenberechnungen SOLLEN nur bei relevanten Änderungen erneut erfolgen.
- Mehrere Änderungen DÜRFEN zusammengefasst werden.
- Wiederholte vollständige Rasterberechnungen SOLLEN vermieden werden.

Die Berechnung MUSS auch bei großen Rastern kontrollierbar bleiben.

Für sehr große Datenmengen SOLLEN virtualisierte Steuerelemente verwendet werden können.

## 12. Fehlerbehandlung

Nova Grid Layout MUSS ungültige Rasterdefinitionen kontrolliert behandeln.

Dazu gehören:

- Negative Größen.
- Ungültige Star-Gewichtungen.
- Ungültige Zeilen- oder Spaltenindizes.
- Ungültige Span-Werte.
- Widersprüchliche Größenbeschränkungen.
- Ressourcenüberschreitungen.

Fehlerhafte Grid-Definitionen DÜRFEN andere UI-Instanzen nicht beeinträchtigen.

## 13. Normative Anforderungen

- **UI-001:** NovaOS MUSS ein natives Grid Layout bereitstellen.
- **UI-002:** Grid Layout MUSS Zeilen und Spalten unterstützen.
- **UI-003:** Feste, automatische und proportionale Größen MÜSSEN unterstützt werden.
- **UI-004:** Mindest- und Maximalgrößen MÜSSEN berücksichtigt werden.
- **UI-005:** UI-Elemente MÜSSEN einzelnen Rasterzellen zugeordnet werden können.
- **UI-006:** RowSpan und ColumnSpan MÜSSEN unterstützt werden.
- **UI-007:** Mehrere Elemente DÜRFEN dieselbe Rasterzelle belegen.
- **UI-008:** Grid Layout MUSS das Measure- und Arrange-Verfahren verwenden.
- **UI-009:** Größenberechnungen MÜSSEN deterministisch erfolgen.
- **UI-010:** Dynamische Rasteränderungen MÜSSEN unterstützt werden.
- **UI-011:** Änderungen MÜSSEN gezielte Layoutinvalidierungen auslösen.
- **UI-012:** Unveränderte Berechnungsergebnisse SOLLEN wiederverwendet werden.
- **UI-013:** Grid Layout MUSS unabhängig vom Renderbackend funktionieren.
- **UI-014:** Ungültige Rasterdefinitionen MÜSSEN kontrolliert behandelt werden.
- **UI-015:** Nova Grid Layout MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 14. Integration

- **Nova Layout Engine:** Measure, Arrange und Layoutinvalidierung.
- **Nova Declarative UI Model:** Deklarative Grid-Definitionen.
- **Nova UI Runtime:** Verwaltung und Aktualisierung.
- **Nova UI Property System:** Grid-Eigenschaften.
- **Nova UI Control Framework:** Grid-Container und untergeordnete Controls.
- **Nova UI Component Lifecycle:** Dynamische Elemente.
- **Nova UI Renderer Abstraction:** Darstellung der angeordneten Elemente.
- **Nova UI Threading Model:** Konsistente Layoutberechnung.

## 15. Ergebnis

NovaOS erhält ein natives, flexibles und performantes Grid Layout zur zeilen- und spaltenbasierten Anordnung von Benutzeroberflächen.

Feste, automatische und proportionale Größen ermöglichen die Erstellung komplexer und anpassungsfähiger Layouts.

Durch gezielte Invalidierungen und Caching werden unnötige Neuberechnungen vermieden.

**Grundsatz:** Nova Grid Layout verteilt den verfügbaren Platz anhand klar definierter Regeln und berechnet nur die Rasterbereiche neu, deren Layoutbedingungen sich tatsächlich verändert haben.