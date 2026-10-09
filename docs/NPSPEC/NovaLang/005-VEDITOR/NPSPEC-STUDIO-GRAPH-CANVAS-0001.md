
# NPSPEC-STUDIO-GRAPH-CANVAS-0001 – NovaLang Studio Graph Canvas

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Canvas

## Zweck

Definiert die interaktive Arbeitsfläche zur visuellen Darstellung und Bearbeitung von Logic Graphs.

Ziel ist eine schnelle, übersichtliche und intuitive Bedienung auch bei umfangreichen Graphen mit zahlreichen Knoten und Verbindungen.

## Architektur

Das Graph Canvas besteht aus:

- **Canvas Renderer:** Darstellung von Knoten, Ports und Verbindungen.
- **Viewport Controller:** Steuerung von Zoom und sichtbarem Arbeitsbereich.
- **Interaction Manager:** Verarbeitung von Maus-, Tastatur- und Touch-Eingaben.
- **Selection Manager:** Verwaltung ausgewählter Graph-Elemente.
- **Layout Engine:** Positionierung und automatische Anordnung.
- **Connection Layer:** Darstellung und Aktualisierung von Verbindungen.
- **Overlay Manager:** Anzeige von Auswahlrahmen, Fehlern und Debug-Informationen.

Das Canvas verwendet die Graphdefinition des Logic Graph Editors und verändert nicht deren Ausführungssemantik.

## Darstellung

Das Canvas unterstützt:

- Praktisch unbegrenzte, virtuell verwaltete Arbeitsfläche
- Frei positionierbare Knoten
- Typisierte Eingangs- und Ausgangsports
- Visuelle Verbindungen zwischen Ports
- Dezentes Raster mit optionalem Snap-to-Grid
- Zoom und Verschieben der Arbeitsfläche
- Gruppierungen und Subgraph-Darstellungen
- Minimierung und Erweiterung von Knoten

Die Gestaltung orientiert sich am NovaOS Fluent-Design mit ruhigen Farben, dezenten Transparenzen und klar erkennbaren Interaktionselementen.

## Interaktion

Unterstützt werden:

- Knoten durch Ziehen verschieben
- Mehrere Elemente gleichzeitig auswählen
- Auswahlrahmen aufziehen
- Verbindungen durch Ziehen zwischen Ports erstellen
- Knoten und Verbindungen löschen
- Kopieren, Ausschneiden und Einfügen
- Undo und Redo
- Kontextmenüs
- Tastaturkürzel
- Drag-and-drop aus der Node Palette

Alle Änderungen werden an das Graphmodell übergeben und anschließend validiert.

## Navigation

Das Canvas ermöglicht:

- Stufenloses Zoomen innerhalb definierter Grenzen
- Zoomen um die Mausposition
- Verschieben per Maus oder Touch
- Gesamten Graphen einpassen
- Auswahl zentrieren
- Schnelle Navigation zu Knoten
- Optionale Minimap

Viewport-Position und Zoomstufe werden als Editor-Metadaten gespeichert.

## Layout und Verbindungen

Knoten können manuell oder automatisch angeordnet werden.

Verbindungen werden bei Positionsänderungen unmittelbar aktualisiert.

Porttypen und Verbindungszustände müssen visuell unterscheidbar sein.

Ungültige Verbindungen werden erkennbar markiert und dürfen nicht als gültige Graphverbindungen übernommen werden.

## Performance

Das Canvas verwendet:

- Viewport Culling für nicht sichtbare Elemente
- Inkrementelle Aktualisierung veränderter Bereiche
- Wiederverwendung von Renderressourcen
- Hardwarebeschleunigung, sofern verfügbar
- Reduzierte Animationen bei begrenzten Ressourcen

Auch große Graphen sollen ohne unnötige vollständige Neuberechnung dargestellt werden.

## Debugging

Während einer Debug-Sitzung können aktive Knoten, Datenflüsse, Haltepunkte und Fehler hervorgehoben werden.

Debug-Overlays dürfen die gespeicherte Graphdefinition nicht verändern.

## Normative Anforderungen

1. Das Graph Canvas MUSS Knoten, Ports und Verbindungen interaktiv darstellen.
2. Knoten MÜSSEN frei positionierbar sein.
3. Zoom und Verschieben MÜSSEN unterstützt werden.
4. Mehrfachauswahl und Auswahlrahmen MÜSSEN verfügbar sein.
5. Verbindungen MÜSSEN direkt zwischen Ports erstellt werden können.
6. Undo und Redo MÜSSEN unterstützt werden.
7. Automatische Knotenanordnung SOLL verfügbar sein.
8. Ungültige Verbindungen MÜSSEN visuell erkennbar sein.
9. Änderungen MÜSSEN mit dem Graphmodell synchronisiert werden.
10. Viewport Culling und inkrementelles Rendering MÜSSEN unterstützt werden.
11. Editor-Metadaten DÜRFEN die Ausführungssemantik nicht beeinflussen.
12. Debug-Informationen MÜSSEN als getrennte Overlays darstellbar sein.
13. Das Canvas MUSS ohne Hardwarebeschleunigung funktionsfähig bleiben.
14. Alle grundlegenden Canvas-Funktionen MÜSSEN ohne KI verfügbar sein.

## Ergebnis

NovaLang Studio erhält eine performante, frei navigierbare und intuitiv bedienbare Graph-Arbeitsfläche zur visuellen Entwicklung komplexer NovaOS-Solutions.
