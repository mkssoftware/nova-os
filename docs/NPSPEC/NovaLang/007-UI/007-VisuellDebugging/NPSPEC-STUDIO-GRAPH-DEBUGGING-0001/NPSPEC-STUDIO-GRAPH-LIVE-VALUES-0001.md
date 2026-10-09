# NPSPEC-STUDIO-GRAPH-LIVE-VALUES-0001

**Status:** Angenommen

## Zweck
Anzeige von Laufzeitwerten an Pins.

## Festlegungen
- Aktuelle Werte werden nur für beobachtbare Pins eines gewählten Ausführungskontexts angezeigt und mit Typ sowie Aktualisierungszeit versehen.
- Große Daten erscheinen gekürzt und auf Abruf; geheime oder nicht freigegebene Daten werden maskiert, die Anzeige fordert keine zusätzlichen Capabilities an.
- Debug-Daten sind von der produktiven Ausführung getrennt; Capability-Grenzen und Isolation bleiben wirksam.

## Ergebnis
Laufzeitdaten sind am Entstehungsort verständlich.
