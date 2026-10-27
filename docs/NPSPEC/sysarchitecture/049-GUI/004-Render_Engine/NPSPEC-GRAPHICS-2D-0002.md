# NPSPEC-GRAPHICS-2D-0002 – Nova Primitive Drawing

**Status:** Angenommen  
**Bereich:** Graphics / 2D / Primitive Drawing  
**System:** NovaOS

## 1. Zweck

Nova Primitive Drawing definiert die grundlegenden Zeichenoperationen der Nova 2D Rendering Engine.

Ziel ist eine einheitliche, schnelle und präzise Darstellung geometrischer Grundelemente, unabhängig vom verwendeten CPU- oder GPU-Backend.

Die Zeichenfunktionen bilden die Grundlage für Nova UI, Vektorgrafiken, Diagramme, technische Zeichnungen und weitere grafische Anwendungen.

## 2. Architektur

Primitive Drawing ist Bestandteil der Nova 2D Rendering Engine.

Die Verarbeitung erfolgt über folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| Primitive Renderer | Verarbeitung grundlegender Zeichenoperationen |
| Geometry Processor | Berechnung geometrischer Eigenschaften |
| Stroke Processor | Verarbeitung von Linien und Konturen |
| Fill Processor | Verarbeitung von Flächenfüllungen |
| Rasterizer | Umwandlung geometrischer Elemente in Pixel |
| Drawing Context | Verwaltung von Zeichenparametern und Transformationen |

Die Ausführung erfolgt über die Nova Graphics Backend Abstraction.

## 3. Unterstützte Primitive

Die Engine unterstützt folgende Grundelemente:

| Primitive | Beschreibung |
|---|---|
| Point | Einzelner Punkt |
| Line | Gerade Linie zwischen zwei Punkten |
| Polyline | Verbundene Liniensegmente |
| Rectangle | Rechteck |
| Rounded Rectangle | Rechteck mit abgerundeten Ecken |
| Circle | Kreis |
| Ellipse | Ellipse |
| Arc | Kreisbogen oder Ellipsenbogen |
| Triangle | Dreieck |
| Polygon | Geschlossenes Vieleck |
| Bézier Curve | Quadratische und kubische Bézier-Kurven |
| Path | Zusammengesetzter geometrischer Pfad |

Primitive können mit Konturen, Füllungen oder beiden Darstellungsarten gezeichnet werden.

Komplexe Geometrien werden durch Kombination grundlegender Primitive und Pfadoperationen aufgebaut.

## 4. Koordinatensystem

Alle Primitive verwenden das einheitliche logische Koordinatensystem der Nova 2D Rendering Engine.

Festlegungen:

- Koordinaten verwenden Fließkommawerte.
- Der Koordinatenursprung liegt standardmäßig links oben.
- Die X-Achse verläuft nach rechts.
- Die Y-Achse verläuft nach unten.
- Transformationen können das lokale Koordinatensystem verändern.
- Die Umrechnung in physische Pixel erfolgt beim Rendering.

Geometrische Berechnungen DÜRFEN nicht von einer festen Bildschirmauflösung abhängen.

## 5. Linien und Konturen

Der Stroke Processor unterstützt:

- Variable Linienstärken.
- Durchgezogene Linien.
- Gestrichelte und gepunktete Linien.
- Benutzerdefinierte Strichmuster.
- Linienenden: Butt, Square und Round.
- Linienverbindungen: Miter, Bevel und Round.
- Miter-Limits.
- Geschlossene und offene Konturen.

Linienstärken werden in logischen Koordinateneinheiten angegeben.

Die Darstellung MUSS Transformationen und Skalierungen berücksichtigen.

## 6. Flächenfüllungen

Der Fill Processor unterstützt:

- Einfarbige Füllungen.
- Lineare Farbverläufe.
- Radiale Farbverläufe.
- Bild- und Texturfüllungen.
- Transparente Füllungen.

Für komplexe Pfade werden folgende Füllregeln unterstützt:

- Non-Zero Winding Rule.
- Even-Odd Rule.

Die Füllregel MUSS für jeden entsprechenden Zeichenaufruf eindeutig bestimmbar sein.

## 7. Geometrische Transformationen

Primitive unterstützen:

- Translation.
- Rotation.
- Skalierung.
- Spiegelung.
- Affine Transformationen.

Transformationen werden über den Drawing Context bereitgestellt.

Mehrere Transformationen können kombiniert werden.

Die Reihenfolge der Transformationen MUSS eindeutig definiert sein.

## 8. Clipping und Antialiasing

Primitive werden ausschließlich innerhalb des gültigen Clipping-Bereichs dargestellt.

Unterstützt werden:

- Rechteckiges Clipping.
- Pfadbasiertes Clipping.
- Verschachtelte Clipping-Bereiche.
- Antialiasing für Linien und Kanten.

Antialiasing SOLL standardmäßig aktiviert sein, sofern keine andere Darstellung angefordert wird.

Für pixelgenaue Darstellungen MUSS Antialiasing deaktivierbar sein.

## 9. Rendering-Verhalten

Zeichenoperationen werden grundsätzlich in ihrer definierten Reihenfolge verarbeitet.

Dabei gilt:

- Spätere Operationen können frühere Inhalte überzeichnen.
- Transparenz wird entsprechend dem gewählten Blending-Modus verarbeitet.
- Geometrien außerhalb des Clipping-Bereichs werden nicht dargestellt.
- Ungültige oder nicht darstellbare Geometrien werden kontrolliert behandelt.
- Rendering-Ergebnisse bleiben innerhalb der definierten numerischen Toleranzen backendübergreifend konsistent.

Die Engine DARF kompatible Zeichenoperationen zur Leistungsoptimierung zusammenfassen.

Die sichtbare Zeichenreihenfolge MUSS dabei erhalten bleiben.

## 10. Leistungsoptimierung

Primitive Drawing unterstützt:

- Batching gleichartiger Zeichenoperationen.
- Caching wiederverwendbarer Geometrien.
- SIMD-optimierte Rasterisierung.
- GPU-beschleunigte Primitive.
- Tile-basiertes Rendering.
- Clipping vor aufwendiger Rasterisierung.
- Vermeidung unnötiger Speicherzugriffe.

Kleine Zeichenoperationen DÜRFEN direkt durch die CPU verarbeitet werden, wenn dadurch geringere Ausführungskosten entstehen.

Die Wahl des Rendering-Backends erfolgt gemäß Nova GPU and CPU Rendering.

## 11. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS grundlegende geometrische Primitive bereitstellen.
- GRAPHICS-002: Primitive MÜSSEN über die Nova 2D Rendering Engine verwendbar sein.
- GRAPHICS-003: Alle Primitive MÜSSEN ein einheitliches Koordinatenmodell verwenden.
- GRAPHICS-004: Linien und geschlossene Geometrien MÜSSEN mit Konturen dargestellt werden können.
- GRAPHICS-005: Geschlossene Geometrien MÜSSEN Flächenfüllungen unterstützen.
- GRAPHICS-006: Linienstärken, Linienenden und Linienverbindungen MÜSSEN konfigurierbar sein.
- GRAPHICS-007: Quadratische und kubische Bézier-Kurven MÜSSEN unterstützt werden.
- GRAPHICS-008: Zusammengesetzte Pfade MÜSSEN unterstützt werden.
- GRAPHICS-009: Geometrische Transformationen MÜSSEN unterstützt werden.
- GRAPHICS-010: Clipping MUSS für sämtliche Primitive berücksichtigt werden.
- GRAPHICS-011: Antialiasing MUSS aktivierbar und deaktivierbar sein.
- GRAPHICS-012: Transparenz und Blending MÜSSEN unterstützt werden.
- GRAPHICS-013: Die Zeichenreihenfolge MUSS erhalten bleiben.
- GRAPHICS-014: Primitive MÜSSEN über CPU-Rendering darstellbar sein.
- GRAPHICS-015: GPU-Beschleunigung SOLL bei geeigneter Hardware verwendet werden.
- GRAPHICS-016: Ungültige Geometrien DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- GRAPHICS-017: Die Schnittstellen MÜSSEN Bestandteil des portablen Nova Framework sein können.
- GRAPHICS-018: Primitive Drawing MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 12. Fehlerbehandlung

- Ungültige Koordinaten werden erkannt und kontrolliert behandelt.
- Nicht endliche Werte wie NaN oder Infinity werden zurückgewiesen.
- Geometrien mit ungültigen Abmessungen werden nicht unkontrolliert gerendert.
- Numerische Überläufe MÜSSEN verhindert oder erkannt werden.
- Nicht unterstützte Zeichenfunktionen werden eindeutig gemeldet.
- Bei GPU-Ausfall SOLL die Darstellung über den CPU Renderer fortgesetzt werden.
- Fehlerhafte Primitive DÜRFEN unabhängige Zeichenoperationen nicht unkontrolliert beeinträchtigen.

## 13. Ergebnis

NovaOS erhält eine einheitliche Grundlage zur Darstellung geometrischer Primitive.

Punkte, Linien, Formen, Kurven und Pfade können unabhängig von der verwendeten Hardware gezeichnet, transformiert und kombiniert werden.

Die Architektur unterstützt sowohl einfache Benutzeroberflächen als auch komplexe Vektorgrafiken und technische Darstellungen.

**Grundsatz:** Jede geometrische Grundform wird durch eine einheitliche, präzise und backendunabhängige Zeichenoperation beschrieben.