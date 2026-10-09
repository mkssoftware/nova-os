# NPSPEC-STUDIO-GRAPH-CAPABILITY-PINS-0001

**Status:** Angenommen

## Zweck
Capability-bezogene Eingänge und Ausgänge eindeutig typisieren.

## Festlegungen
- Pins unterscheiden Capability-Anfragen, Operationsergebnisse, Daten und Capability-Handles.
- Handle-Pins tragen Gültigkeitsbereich, Operationstyp und Lebensdauer im Typvertrag.
- Pins dürfen weder in beliebige Datentypen konvertiert noch durch reine Datenleitungen autorisiert werden.
- Visuelle Kennzeichnung ergänzt, ersetzt aber niemals die semantische Validierung.

## Ergebnis
Capability-Pins vermitteln nur explizit freigegebene Befugnisse.
