# NPSPEC-STUDIO-GRAPH-VISUAL-DEBUGGING-0001

**Status:** Angenommen

## Zweck
Visuelles Debugging des NovaLang Logic Graph.

## Festlegungen
- Debugging bildet den tatsächlichen Ausführungszustand auf den unveränderten Graph ab; die Laufzeit liefert hierzu strukturierte Debug-Events mit Graph-, Knoten- und Ausführungs-IDs.
- Start, Pause, Fortsetzen und Ende einer Sitzung sind eindeutig erkennbar; Debug-Ansichten verändern weder Graph-Semantik noch Berechtigungen.
- Debug-Daten sind von der produktiven Ausführung getrennt; Capability-Grenzen und Isolation bleiben wirksam.

## Ergebnis
Debugging ist direkt im Logic Graph nachvollziehbar.
