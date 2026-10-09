# NPSPEC-STUDIO-UI-RENDERING-0001

**Status:** Angenommen

## Zweck
Effiziente Darstellung der Studio-Oberfläche.

## Festlegungen
- Die Rendering-Pipeline muss invalidierte Bereiche statt ganzer Fenster neu zeichnen.
- Layout, Textmessung, Compositing und Darstellung sollen getrennt aktualisiert werden.
- Fluent-/Acrylic-Effekte dürfen keine dauerhafte Vollbild-Neuzeichnung erzwingen.
- Fehlende GPU-Beschleunigung muss über einen funktionsfähigen, reduzierten Darstellungsmodus abgefangen werden.

## Ergebnis
Eine stabile Darstellung ohne unnötige Rechenlast.
