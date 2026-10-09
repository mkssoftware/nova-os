# NPSPEC-STUDIO-GRAPH-INTERACTION-LATENCY-0001

**Status:** Angenommen

## Zweck
Geringe Verzögerung bei Graph-Interaktionen.

## Festlegungen
- Ziehen von Knoten, Verknüpfen von Pins, Panning und Zoom müssen Vorrang vor Hintergrundvalidierung erhalten.
- Als Darstellungsziel gelten 60 Bilder/s auf geeigneter Hardware; degradierte Modi müssen Eingaben weiterführen.
- Vorschauverbindungen müssen unmittelbar lokal erscheinen; Typ- und Capability-Prüfungen dürfen nachgelagert bestätigen.
- Verzögerungen sind getrennt nach Eingabe, Hit-Test, Layout und Zeichnung zu erfassen.

## Ergebnis
Direkte, nachvollziehbare Bedienung auch in großen Graphs.
