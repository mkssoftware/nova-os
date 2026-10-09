# NPSPEC-STUDIO-GRAPH-INCREMENTAL-RENDERING-0001

**Status:** Angenommen

## Zweck
Schrittweise Graph-Aktualisierung.

## Festlegungen
- Änderungen an Knoten, Pins und Leitungen dürfen nur abhängige Zeichenbereiche invalidieren.
- Datenänderungen und Renderaufträge müssen zusammengefasst und versionsgebunden werden.
- Unveränderte Beschriftungen, Pfade und Geometrien sollen im Cache verbleiben.
- Zwischenzustände dürfen nicht als gültiges finales Graphresultat erscheinen.

## Ergebnis
Graphänderungen werden effizient und konsistent dargestellt.
