# NPSPEC-STUDIO-GRAPH-ERROR-HIGHLIGHT-0001

**Status:** Angenommen

## Zweck
Fehlerlokalisierung im Graph.

## Festlegungen
- Laufzeit- und Validierungsfehler markieren den verursachenden Knoten oder Pin und verlinken zur Diagnose mit Fehlercode und Kontext.
- Fehlerfarben sind nicht das einzige Signal; Folgefehler werden vom Ursprungsfehler unterschieden, und Markierungen bleiben bis zur Bestätigung oder neuen Prüfung erhalten.
- Debug-Daten sind von der produktiven Ausführung getrennt; Capability-Grenzen und Isolation bleiben wirksam.

## Ergebnis
Fehlerursachen sind schnell auffindbar.
