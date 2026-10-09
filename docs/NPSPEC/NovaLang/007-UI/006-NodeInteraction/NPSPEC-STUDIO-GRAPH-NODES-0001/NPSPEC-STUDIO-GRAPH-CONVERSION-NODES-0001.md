# NPSPEC-STUDIO-GRAPH-CONVERSION-NODES-0001

**Status:** Angenommen

## Zweck
Explizite Daten- und Typkonvertierungen ermöglichen.

## Festlegungen
- Verlustfreie, implizit zugelassene Konvertierungen sind von expliziten Casts zu unterscheiden.
- Potentiell verlustbehaftete Konvertierungen benötigen einen sichtbaren Knoten.
- Fehlschläge werden über Ergebnis- oder Fehlerpins signalisiert, nicht verschwiegen.
- Capability- und Handle-Typen dürfen nicht durch allgemeine Konvertierungen aufgewertet werden.

## Ergebnis
Typumwandlungen sind sichtbar und sicher.
