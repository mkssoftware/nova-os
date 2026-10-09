
# NPSPEC-STUDIO-GRAPH-NAVIGATION-0001 – NovaLang Studio Graph Navigation

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Navigation

## Zweck

Definiert die Navigation innerhalb umfangreicher Logic Graphs und zwischen verbundenen Subgraphs.

Ziel ist, Knoten, Verbindungen und Ausführungsbereiche schnell und intuitiv zu erreichen, ohne den Arbeitskontext zu verlieren.

## Architektur

Das Navigation-System besteht aus:

- **Navigation Controller:** Steuerung sämtlicher Navigationsaktionen.
- **Viewport Navigator:** Verwaltung von Zoom und Arbeitsbereich.
- **Graph Explorer:** Strukturierte Übersicht von Knoten und Subgraphs.
- **Minimap:** Kompakte Gesamtansicht des Graphen.
- **Navigation History:** Verwaltung besuchter Positionen.
- **Search Navigator:** Direkte Navigation zu gesuchten Elementen.

Das System verwendet Graph Canvas und die vorhandenen Graph-Identitäten.

## Navigationsfunktionen

Unterstützt werden:

- Zoomen um die Mausposition
- Verschieben der Arbeitsfläche
- Gesamten Graphen einpassen
- Ausgewählte Elemente zentrieren
- Direkter Sprung zu Knoten und Ports
- Navigation entlang von Verbindungen
- Wechsel zwischen Graphen und Subgraphs
- Zurück- und Vorwärtsnavigation
- Tastatur- und Maussteuerung

## Graph Explorer

Der Graph Explorer stellt die Struktur des aktuellen Graphen dar.

Knoten, Gruppen und Subgraphs können direkt ausgewählt und auf dem Canvas fokussiert werden.

Die Darstellung muss mit Änderungen am Graphmodell synchron bleiben.

## Minimap

Die Minimap zeigt:

- Gesamtausdehnung des Graphen
- Aktuellen sichtbaren Arbeitsbereich
- Knoten und Gruppen
- Optional ausgewählte Elemente

Durch Klicken oder Ziehen kann der sichtbare Arbeitsbereich verändert werden.

Die Minimap muss deaktivierbar sein.

## Suche

Die Navigation unterstützt die Suche nach:

- Knotennamen und Node-IDs
- Knotentypen
- Capability-IDs
- Subgraphs
- Benutzerdefinierten Bezeichnungen

Suchergebnisse können direkt auf dem Canvas fokussiert werden.

## Subgraph-Navigation

Subgraphs können direkt geöffnet werden.

Eine Navigationshistorie ermöglicht die Rückkehr zum übergeordneten Graphen und zur vorherigen Ansicht.

Viewport-Position, Zoomstufe und Auswahlzustand sollen beim Wechsel erhalten bleiben.

## Performance

Die Navigation muss unabhängig von der Anzahl nicht sichtbarer Knoten reaktionsschnell bleiben.

Suchindizes und Navigationsstrukturen werden bei Graphänderungen inkrementell aktualisiert.

Animationen dürfen die Bedienung nicht verzögern und müssen deaktivierbar sein.

## Normative Anforderungen

1. Der Graph Editor MUSS Zoom und Verschieben unterstützen.
2. Graphen und ausgewählte Elemente MÜSSEN automatisch zentriert werden können.
3. Eine strukturierte Graphübersicht MUSS verfügbar sein.
4. Eine optionale Minimap MUSS unterstützt werden.
5. Knoten und Subgraphs MÜSSEN direkt über die Suche erreichbar sein.
6. Navigation entlang bestehender Verbindungen MUSS möglich sein.
7. Subgraph-Wechsel MÜSSEN den Navigationskontext erhalten.
8. Zurück- und Vorwärtsnavigation MÜSSEN unterstützt werden.
9. Navigation MUSS per Maus und Tastatur möglich sein.
10. Navigationsaktionen DÜRFEN die Graph-Ausführungssemantik nicht verändern.
11. Umfangreiche Graphen MÜSSEN ohne blockierende Navigation bedienbar bleiben.
12. Alle Navigationsfunktionen MÜSSEN ohne KI verfügbar sein.

## Ergebnis

NovaLang Studio erhält ein schnelles, kontextbewahrendes Navigationssystem, mit dem auch komplexe Logic Graphs und verschachtelte Subgraphs übersichtlich und unmittelbar erreichbar bleiben.
