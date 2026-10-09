# NPSPEC-STUDIO-GRAPH-DATA-NODES-0001

**Status:** Angenommen

## Zweck
Typisierte Werte erzeugen, transformieren und weiterreichen.

## Festlegungen
- Konstanten, Strukturzugriffe, Collections und Datenprojektionen sind als Datenknoten verfügbar.
- Datenknoten sind ohne explizite Effektdeklaration seiteneffektfrei.
- Nullbarkeit, Besitz und Lebensdauer von Daten folgen dem NovaLang-Typsystem.
- Große Datenobjekte dürfen über referenzierte, berechtigungskontrollierte Handles übertragen werden.

## Ergebnis
Datentransformation ist typisiert und vom Systemzugriff getrennt.
