# NPSPEC-STUDIO-DPI-0001

**Status:** Angenommen

## Zweck

Korrekte Darstellung auf HiDPI- und Multi-Monitor-Systemen.

## Festlegungen

- Rendering trennt logische UI-Koordinaten von physikalischen Gerätepixeln.
- Icons und sonstige Skalierungsgrafiken sind vorzugsweise vektorbasiert.
- Beim Verschieben zwischen Monitoren mit unterschiedlicher Pixeldichte werden UI-Metriken und Rendering aktualisiert.
- Text, Rahmen und Graph-Verbindungen werden pixelgenau ausgerichtet; Pointer-Koordinaten bleiben korrekt.

## Ergebnis

Scharfe Darstellung und präzise Eingabe bei variabler DPI.
