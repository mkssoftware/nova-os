# NPSPEC-STUDIO-GRAPH-VIEWPORT-CULLING-0001

**Status:** Angenommen

## Zweck
Sichtbarkeitsfilter des Graph-Canvas.

## Festlegungen
- Nur Elemente innerhalb des Viewports einschließlich eines kleinen Vorladebereichs sollen vollständig gerendert werden.
- Für ausgeblendete Knoten und Leitungen bleiben Geometrie, Trefferprüfung und Datenmodell erhalten.
- Verschieben und Zoomen muss den sichtbaren Bestand inkrementell aktualisieren.
- Ausgewählte oder fokussierte Elemente dürfen durch Culling nicht logisch verloren gehen.

## Ergebnis
Das Canvas verarbeitet vorzugsweise sichtbare Elemente.
