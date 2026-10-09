# NPSPEC-STUDIO-GRAPH-DATA-INSPECTOR-0001

**Status:** Angenommen

## Zweck
Strukturierte Laufzeitdaten prüfen.

## Festlegungen
- Ein Inspector zeigt typisierte Werte, Felder, Listen und verschachtelte Objekte aus einem angehaltenen oder explizit beobachteten Kontext.
- Zyklen und große Datenmengen werden begrenzt bzw. lazy geladen; Referenzen, Nullwerte und nicht verfügbare Daten sind unterscheidbar. Inspektion führt keine nebenwirkungsreichen Getter aus.
- Debug-Daten sind von der produktiven Ausführung getrennt; Capability-Grenzen und Isolation bleiben wirksam.

## Ergebnis
Komplexe Daten sind kontrolliert einsehbar.
