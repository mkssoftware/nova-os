
# NPSPEC-STUDIO-GRAPH-GROUPING-0001 – NovaLang Studio Graph Grouping

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Gruppierung

## Zweck

Definiert die visuelle Gruppierung und Organisation zusammengehöriger Knoten innerhalb eines Logic Graphs.

Ziel ist, umfangreiche Graphen übersichtlich zu strukturieren, ohne deren Ausführungssemantik zu verändern.

## Architektur

Das Grouping-System besteht aus:

- **Group Manager:** Verwaltung visueller Gruppen.
- **Group Renderer:** Darstellung von Gruppenrahmen und Beschriftungen.
- **Selection Controller:** Gemeinsame Auswahl gruppierter Elemente.
- **Layout Controller:** Positionierung und Größenanpassung.
- **Group Metadata Store:** Speicherung der Gruppeneigenschaften.
- **Undo Integration:** Rücknahme von Gruppierungsänderungen.

Das System verwendet das bestehende Graphmodell und den Graph Canvas.

## Gruppierungsmodell

Eine Gruppe besitzt:

- Eindeutige Group-ID
- Namen und optionale Beschreibung
- Zugeordnete Knoten
- Position und Größe
- Optionale Farbe und Darstellung
- Sichtbarkeits- und Minimierungszustand

Gruppen dienen ausschließlich der visuellen Organisation.

Ein Subgraph ist dagegen ein eigenständiger ausführbarer Graphbereich mit definierten Schnittstellen.

## Bedienung

Unterstützt werden:

- Mehrere Knoten zu einer Gruppe zusammenfassen
- Gruppen benennen und farblich kennzeichnen
- Gruppen gemeinsam verschieben
- Gruppen minimieren und erweitern
- Knoten hinzufügen oder entfernen
- Gruppierungen auflösen
- Gruppen kopieren und einfügen
- Gruppen über Kontextmenü erstellen

Die Bedienung erfolgt direkt auf dem Graph Canvas.

## Darstellung

Gruppen werden durch dezente Rahmen oder Hintergrundflächen dargestellt.

Beschriftungen bleiben auch bei größeren Zoomstufenänderungen erkennbar.

Minimierte Gruppen dürfen die zugrunde liegenden Knoten und Verbindungen nicht aus der Graphdefinition entfernen.

Verbindungen zwischen gruppierten und externen Knoten bleiben erhalten.

## Verschachtelung

Gruppen dürfen ineinander verschachtelt werden.

Zyklische Gruppenzugehörigkeiten sind unzulässig.

Die Verschachtelungstiefe muss zur Vermeidung übermäßiger Ressourcenbelastung begrenzbar sein.

## Subgraph-Integration

Gruppierte Knoten können über eine gesonderte Aktion in einen Subgraph überführt werden.

Dabei müssen externe Verbindungen in definierte Subgraph-Ports überführt und anschließend validiert werden.

Eine reine Gruppierung darf niemals automatisch die Ausführungssemantik verändern.

## Speicherung

Gruppeneigenschaften werden als Editor-Metadaten gespeichert.

Node-IDs, Port-IDs und Verbindungen bleiben bei Gruppierungsänderungen unverändert.

Gruppierungsaktionen müssen Undo und Redo unterstützen.

## Normative Anforderungen

1. Der Graph Editor MUSS visuelle Knotengruppen unterstützen.
2. Gruppen MÜSSEN eindeutige IDs besitzen.
3. Mehrere Knoten MÜSSEN gemeinsam gruppiert und verschoben werden können.
4. Gruppen MÜSSEN benannt und aufgelöst werden können.
5. Minimieren und Erweitern MÜSSEN unterstützt werden.
6. Gruppierungen DÜRFEN die Graph-Ausführungssemantik nicht verändern.
7. Bestehende Verbindungen und Knotenidentitäten MÜSSEN erhalten bleiben.
8. Verschachtelte Gruppen DÜRFEN keine zyklischen Gruppenzugehörigkeiten erzeugen.
9. Eine Umwandlung in Subgraphs MUSS gesondert erfolgen und validiert werden.
10. Gruppeneigenschaften MÜSSEN als Editor-Metadaten gespeichert werden.
11. Undo und Redo MÜSSEN unterstützt werden.
12. Alle grundlegenden Grouping-Funktionen MÜSSEN ohne KI verfügbar sein.

## Ergebnis

NovaLang Studio erhält ein flexibles Gruppierungssystem zur übersichtlichen Organisation komplexer Logic Graphs, ohne deren Funktionalität oder Ausführungsverhalten zu beeinflussen.
