
# NPSPEC-STUDIO-GRAPH-LAYOUT-0001 – NovaLang Studio Graph Layout

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Layout

## Zweck

Definiert die automatische und manuelle Anordnung von Knoten, Gruppen und Verbindungen innerhalb des Logic Graph Editors.

Ziel ist eine übersichtliche, platzsparende und gut lesbare Darstellung komplexer Graphen ohne Veränderung ihrer Ausführungssemantik.

## Architektur

Das Layout-System besteht aus:

- **Layout Manager:** Koordination der Anordnung.
- **Auto Layout Engine:** Automatische Positionierung von Knoten.
- **Connection Router:** Berechnung übersichtlicher Verbindungsverläufe.
- **Alignment Controller:** Ausrichtung und gleichmäßige Verteilung.
- **Collision Resolver:** Vermeidung visueller Überlappungen.
- **Layout Metadata Store:** Speicherung von Positionen und Darstellungsoptionen.

Das System integriert sich in Graph Canvas, Connections und Grouping.

## Layout-Modi

Unterstützt werden:

- **Manual Layout:** Freie Positionierung durch den Benutzer.
- **Hierarchical Layout:** Anordnung nach Abhängigkeiten und Datenfluss.
- **Horizontal Layout:** Ausrichtung von links nach rechts.
- **Vertical Layout:** Ausrichtung von oben nach unten.
- **Compact Layout:** Platzsparende Anordnung.
- **Auto Layout:** Automatische Auswahl einer geeigneten Anordnung.

Der Benutzer kann den Layout-Modus jederzeit wechseln.

## Positionierung

Das Layout-System unterstützt:

- Rasterausrichtung
- Automatische Abstände
- Horizontale und vertikale Ausrichtung
- Gleichmäßige Verteilung
- Mehrfachauswahl
- Fixierte Knotenpositionen
- Berücksichtigung bestehender Gruppen

Manuell fixierte Knoten dürfen durch automatische Anordnung nicht unbeabsichtigt verschoben werden.

## Verbindungsführung

Verbindungen werden möglichst übersichtlich geführt.

Dabei sollen Kreuzungen, Überlagerungen und unnötige Richtungswechsel reduziert werden.

Verbindungstypen und Portzuordnungen bleiben unverändert.

## Gruppen und Subgraphs

Gruppen können gemeinsam angeordnet werden.

Subgraphs werden als eigenständige visuelle Elemente behandelt.

Die interne Anordnung eines Subgraphs bleibt unabhängig vom übergeordneten Graphen.

## Performance

Layoutberechnungen erfolgen bei Bedarf asynchron.

Große Graphen können abschnittsweise angeordnet werden.

Während der Berechnung muss die Benutzeroberfläche bedienbar bleiben.

## Speicherung

Knotenpositionen, Abstände, Routingpunkte und Layout-Modi werden als Editor-Metadaten gespeichert.

Layoutänderungen müssen über Undo und Redo rückgängig gemacht werden können.

## Normative Anforderungen

1. Der Graph Editor MUSS manuelle und automatische Layouts unterstützen.
2. Horizontale, vertikale und hierarchische Anordnung MÜSSEN verfügbar sein.
3. Knoten MÜSSEN ausgerichtet und gleichmäßig verteilt werden können.
4. Überlappungen SOLLEN automatisch vermieden werden.
5. Fixierte Knotenpositionen MÜSSEN berücksichtigt werden.
6. Gruppen und Subgraphs MÜSSEN in Layoutberechnungen einbezogen werden.
7. Verbindungsverläufe SOLLEN automatisch optimiert werden.
8. Layoutänderungen DÜRFEN Knotenidentitäten, Portzuordnungen oder Graphlogik nicht verändern.
9. Umfangreiche Layoutberechnungen DÜRFEN die Benutzeroberfläche nicht blockieren.
10. Layoutinformationen MÜSSEN getrennt von der Ausführungssemantik gespeichert werden.
11. Undo und Redo MÜSSEN unterstützt werden.
12. Alle grundlegenden Layoutfunktionen MÜSSEN ohne KI verfügbar sein.

## Ergebnis

NovaLang Studio erhält ein flexibles und ressourcenschonendes Layout-System, das Logic Graphs automatisch oder manuell übersichtlich anordnet, ohne deren Funktionalität zu beeinflussen.
