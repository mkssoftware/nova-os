# NPSPEC-STUDIO-GRAPH-LOGIC-NODES-0001

**Status:** Angenommen

## Zweck
Boolesche Logik und logische Verknüpfungen modellieren.

## Festlegungen
- AND, OR, NOT und XOR arbeiten mit expliziten Booleschen Pins.
- Kurzschlussauswertung ist nur zulässig, wenn die Auswertungssemantik eindeutig definiert ist.
- Unbekannte oder nullable Zustände werden nicht implizit als False ausgelegt.
- Logikknoten führen keine versteckten Capability-Aktionen aus.

## Ergebnis
Logische Ausdrücke haben eindeutige Semantik.
