# NPSPEC-STUDIO-GRAPH-CAPABILITY-FLOW-0001

**Status:** Angenommen

## Zweck
Den sicheren Verlauf privilegierter Operationen durch den Graphen definieren.

## Festlegungen
- Eine Capability-Prüfung liegt vor jeder privilegierten Operation; die Solution-GUID allein ist keine Autorisierung.
- Capability-Knoten geben nur freigegebene Ergebnisse oder eng begrenzte Handles an Folgeknoten weiter.
- Custom-Skripte verarbeiten erhaltene Daten, sie führen keinen direkten, verborgenen Systemzugriff aus.
- Entzug, Ablauf oder Änderung sicherheitsrelevanter Solution-Inhalte invalidiert betroffene Berechtigungen.
- Graph-Validierung und Laufzeit prüfen den Fluss unabhängig von dessen visueller Darstellung.

## Ergebnis
Capability-Flüsse bleiben nachvollziehbar und können Berechtigungen nicht umgehen.
