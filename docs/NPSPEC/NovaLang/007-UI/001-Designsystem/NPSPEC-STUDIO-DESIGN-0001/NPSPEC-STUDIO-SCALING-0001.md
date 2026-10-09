# NPSPEC-STUDIO-SCALING-0001

**Status:** Angenommen

## Zweck

Robustes Layout auf unterschiedlichen Displaygrößen und bei UI-Skalierung.

## Festlegungen

- Layoutgrößen beruhen auf logischen Einheiten statt fest codierten Pixelmaßen.
- Ribbon und Werkzeugbereiche reagieren adaptiv auf verfügbare Breite, ohne Kernfunktionen zu verstecken.
- Editor und Logic Graph besitzen voneinander unabhängige Zoomstufen.
- Bei Skalierungswechsel werden Anordnung, Hit-Testing und Textumbruch konsistent neu berechnet.

## Ergebnis

Lesbare und bedienbare Oberfläche auf unterschiedlichen Displays.
