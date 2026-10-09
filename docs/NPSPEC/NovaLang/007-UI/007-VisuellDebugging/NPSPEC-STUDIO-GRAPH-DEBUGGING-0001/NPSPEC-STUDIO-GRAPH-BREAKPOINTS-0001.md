# NPSPEC-STUDIO-GRAPH-BREAKPOINTS-0001

**Status:** Angenommen

## Zweck
Haltepunkte für Graph-Knoten.

## Festlegungen
- Haltepunkte werden an stabilen Knoten-IDs gespeichert, lassen sich aktivieren und deaktivieren und greifen vor dem nächsten ausführbaren Schritt. Bedingte Haltepunkte werten nur erlaubte, nebenwirkungsfreie Ausdrücke aus.
- Bei einem Treffer wird nur der betroffene Ausführungskontext angehalten; die Oberfläche zeigt Haltepunkt und Ursache.
- Debug-Daten sind von der produktiven Ausführung getrennt; Capability-Grenzen und Isolation bleiben wirksam.

## Ergebnis
Graph-Ausführung ist gezielt unterbrechbar.
