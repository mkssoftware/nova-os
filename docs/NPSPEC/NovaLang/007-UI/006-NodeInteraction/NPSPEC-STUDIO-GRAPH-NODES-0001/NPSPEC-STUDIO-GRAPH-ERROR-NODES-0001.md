# NPSPEC-STUDIO-GRAPH-ERROR-NODES-0001

**Status:** Angenommen

## Zweck
Fehler gezielt behandeln und weitergeben.

## Festlegungen
- Try, Catch, Recover und Propagate besitzen definierte Fehler- und Kontrollflusspins.
- Fehler enthalten typisierte Kennungen und nicht automatisch sensible Diagnosedaten.
- Nicht behandelte Fehler propagieren bis zur verantwortlichen Graph-Grenze.
- Eine Fehlerbehandlung darf verweigerte Berechtigungen nicht in einen erfolgreichen Zugriff umdeuten.

## Ergebnis
Fehlerbehandlung ist sichtbar und sicher.
