# NPSPEC-STUDIO-GRAPH-WATCH-0001

**Status:** Angenommen

## Zweck
Beobachtung ausgewählter Werte.

## Festlegungen
- Pins, Variablen und nebenwirkungsfreie Ausdrücke können als Watch mit stabiler Referenz gespeichert werden.
- Die Watch-Liste zeigt Typ, Wert, Ausführungskontext und Gültigkeit; ungültige Referenzen werden markiert statt stillschweigend umgebogen. Zugriff bleibt durch die Debug-Berechtigung begrenzt.
- Debug-Daten sind von der produktiven Ausführung getrennt; Capability-Grenzen und Isolation bleiben wirksam.

## Ergebnis
Wichtige Werte lassen sich über mehrere Schritte verfolgen.
