# NPSPEC-BOOTRENDER-0013 – Vector Geometry

| Feld | Wert |
|---|---|
| Status | Draft |
| Geltung | Bootmanager, BIOS und UEFI |

## Verbindlicher Kern

Koordinaten sind 16.16-Fixed-Point-DLU. Der Rasterizer verwendet den
Pixelmittelpunkt und die Non-Zero-Winding-Regel. Primitive sind `path`, `rect`,
`circle`, `ellipse`, `line`, `polyline` und `polygon`; Ausgabe ist stets eine
premultiplizierte RGBA8-Surface gemäß BOOTRENDER-0012.

`path d` unterstützt ausschließlich absolute/relative `M L H V C Q Z`.
Kurven werden deterministisch mit maximal 0,25 DLU Abweichung abgeflacht;
`A`, `S`, `T` und unbekannte Kommandos sind Validierungsfehler. Fills erlauben
`nonzero` und `evenodd`; Strokes erlauben Butt/Round/Square sowie Miter/Round/
Bevel, Miterlimit 1..32. Alle Größen, Offsets und Segmentzahlen werden vor der
Rasterisierung auf Überlauf und ein Budget von 4096 Segmenten geprüft.

Akzeptanz: identische Pixel auf BIOS/UEFI für ein Referenzset mit Rechteck,
Kreis, Bézier, Überlappung und allen Fill-/Stroke-Varianten; ungültige Pfade,
NaN/Infinity und Budgetüberschreitung werden kontrolliert abgewiesen.
