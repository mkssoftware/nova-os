# NPSPEC-STUDIO-GRAPH-FLOW-NODES-0001

**Status:** Angenommen

## Zweck
Den Kontrollfluss über Verzweigungen, Schleifen und Sequenzen steuern.

## Festlegungen
- Branch, Sequence, Loop und Merge sind explizite Kontrollflussoperatoren.
- Schleifen besitzen Abbruchbedingungen und begrenzbare Ausführungsbudgets; rekursive oder endlose Ausführung wird begrenzt.
- Datenpins bestimmen keinen impliziten Kontrollfluss.
- Graphvalidierung erkennt unerreichbare Pfade und unzulässige Zyklen.

## Ergebnis
Kontrollfluss bleibt deterministisch und prüfbar.
