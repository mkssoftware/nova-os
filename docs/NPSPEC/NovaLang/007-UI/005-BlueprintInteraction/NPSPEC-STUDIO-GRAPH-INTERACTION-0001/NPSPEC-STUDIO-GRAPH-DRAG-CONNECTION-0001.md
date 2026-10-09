# NPSPEC-STUDIO-GRAPH-DRAG-CONNECTION-0001

**Status:** Angenommen

## Zweck

Verbindungen ziehen für den NovaLang Studio Logic Graph festlegen.

## Festlegungen

- Eine Verbindung wird vom Ausgangs-Pin zum kompatiblen Eingangs-Pin gezogen; währenddessen erscheinen Vorschau und zulässige Zielpins.
- Loslassen auf unzulässigem Ziel verändert den Graph nicht und zeigt den Validierungsgrund an.
- Verbindungsänderungen sind atomar und Undo-/Redo-fähig; Capability-Grenzen dürfen nicht durch bloßes Verbinden umgangen werden.

## Ergebnis

Eine einheitliche, nachvollziehbare und für NovaLang sowie das Capability-Modell sichere verbindungen ziehen im Logic Graph.
