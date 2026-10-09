# NPSPEC-STUDIO-GRAPH-CUSTOMSCRIPT-NODES-0001

**Status:** Angenommen

## Zweck
Eigene NovaLang-Logik ohne eigenständige Rechteausweitung ausführen.

## Festlegungen
- Custom-Skripte verwenden ausschließlich die NovaLang-Syntax; .nlf ist kein eigener Dialekt.
- Skripte erhalten nur explizit verbundene, typisierte Eingaben und liefern deklarierte Ausgaben.
- Direkte Systemzugriffe oder eigenständige Capability-Anforderungen aus dem Skript sind untersagt.
- Erforderliche Systemdaten werden vorgelagert über Capability-Knoten geliefert.

## Ergebnis
Eigene Logik bleibt leistungsfähig, ohne das Berechtigungsmodell zu umgehen.
