# NPSPEC-STUDIO-GRAPH-CAPABILITY-NODES-0001

**Status:** Angenommen

## Zweck
Autorisierte Systemfähigkeiten explizit in den Graphen einbinden.

## Festlegungen
- Jeder Capability-Knoten referenziert eine registrierte Capability-ID und deklarierte Operation.
- Eine Capability wird ausschließlich nach Berechtigungsprüfung gegen die Solution-Identität aktiviert.
- Ein-/Ausgabepins sind strikt typisiert; Rückgaben können Daten oder begrenzte Handles enthalten.
- Capability-Knoten sind die einzigen regulären Eintrittspunkte für privilegierte Systemoperationen im Logic Graph.

## Ergebnis
Berechtigte Systemoperationen sind sichtbar, kontrollierbar und auditierbar.
