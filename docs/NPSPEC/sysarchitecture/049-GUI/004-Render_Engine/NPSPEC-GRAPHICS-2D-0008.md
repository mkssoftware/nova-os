# NPSPEC-GRAPHICS-2D-0008 – Nova Gradients and Brushes

**Status:** Angenommen  
**Bereich:** Graphics / 2D / Gradients and Brushes  
**System:** NovaOS

## 1. Zweck

Nova Gradients and Brushes definiert ein einheitliches System zur Gestaltung von Flächen, Konturen und geometrischen Objekten innerhalb der Nova 2D Rendering Engine.

Ziel ist die flexible, hochwertige und ressourcenschonende Darstellung von Farben, Farbverläufen, Mustern und Texturen.

Brushes beschreiben das Erscheinungsbild einer Zeichenoperation unabhängig von der zugrunde liegenden Geometrie und dem verwendeten Rendering-Backend.

## 2. Architektur

Das System besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Brush Manager | Verwaltung und Bereitstellung von Brushes |
| Brush Resource | Beschreibung einer Füll- oder Konturquelle |
| Gradient Processor | Berechnung von Farbverläufen |
| Pattern Processor | Verarbeitung wiederholbarer Muster |
| Texture Sampler | Abtastung bildbasierter Brushes |
| Brush Transformer | Transformation von Brush-Koordinaten |
| Backend Adapter | CPU- und GPU-spezifische Umsetzung |

Brushes werden durch Nova Primitive Drawing, Nova Vector Graphics und die Nova 2D Rendering Engine verwendet.

Die eigentliche Rasterisierung erfolgt über die Nova Graphics Backend Abstraction.

## 3. Brush-Modell

Ein Brush beschreibt, wie die geometrische Abdeckung eines Zeichenobjekts farblich gestaltet wird.

Grundlegende Eigenschaften:

- Brush-Typ.
- Farb- oder Bildquelle.
- Deckkraft.
- Koordinatenraum.
- Transformationsmatrix.
- Sampling-Einstellungen.
- Erweiterungsverhalten.

Brushes MÜSSEN unabhängig von geometrischen Objekten definiert und wiederverwendet werden können.

Eine Änderung der Geometrie DARF nicht automatisch die ursprüngliche Brush-Definition verändern.

## 4. Unterstützte Brush-Typen

| Brush | Beschreibung |
|---|---|
| Solid Brush | Einheitliche Farbe |
| Linear Gradient Brush | Linearer Farbverlauf |
| Radial Gradient Brush | Radialer Farbverlauf |
| Conic Gradient Brush | Winkelabhängiger Farbverlauf |
| Image Brush | Bildbasierte Füllung |
| Pattern Brush | Wiederholbares geometrisches oder rasterbasiertes Muster |

Weitere Brush-Typen können über das Nova Framework ergänzt werden.

Alle grundlegenden Brush-Typen MÜSSEN auch ohne GPU-Beschleunigung darstellbar sein.

## 5. Farbverläufe

Ein Farbverlauf besteht aus einer geometrischen Verlaufsdefinition und einer geordneten Menge von Farbstützpunkten.

Ein Farbstützpunkt enthält:

- Position im Verlauf.
- Farbwert.
- Alphawert.

Die Position wird standardmäßig im Bereich `0.0` bis `1.0` angegeben.

Unterstützt werden:

- Beliebig viele Farbstützpunkte innerhalb definierter Ressourcenlimits.
- Unterschiedliche Abstände zwischen Farbstützpunkten.
- Transparente Farbverläufe.
- Mehrfarbige Verläufe.
- Umkehrung der Verlaufsrichtung.

Die Reihenfolge der Farbstützpunkte MUSS eindeutig bestimmt werden.

Gleiche Positionen DÜRFEN zur Definition harter Farbübergänge verwendet werden.

## 6. Verlaufsgeometrien

### Linear Gradient

Ein linearer Farbverlauf wird durch Start- und Endpunkt definiert.

Die Farbposition ergibt sich aus der Projektion des Abtastpunkts auf die Verlaufsachse.

### Radial Gradient

Ein radialer Farbverlauf wird durch Mittelpunkt, Radien und optionalen Fokuspunkt definiert.

Elliptische Farbverläufe MÜSSEN unterstützt werden.

### Conic Gradient

Ein konischer Farbverlauf wird durch Mittelpunkt, Startwinkel und Winkelbereich definiert.

Die Farbe wird anhand des Winkels zum Mittelpunkt bestimmt.

Alle Verlaufsgeometrien unterstützen Transformationen.

## 7. Interpolation und Farbräume

Farbverläufe unterstützen mindestens:

- Lineare Interpolation zwischen Farbstützpunkten.
- Interpolation der Transparenz.
- Farbraumbewusste Farbverarbeitung.

Die Interpolation erfolgt standardmäßig im linearen RGB-Farbraum.

Weitere Interpolationsräume können über das Nova Graphics Color System bereitgestellt werden.

Für transparente Farbverläufe MUSS eine definierte Alpha-Interpolation verwendet werden, die unerwünschte Farbsäume vermeidet.

Die verwendete Interpolationsmethode MUSS eindeutig festgelegt sein.

## 8. Erweiterungsverhalten

Für Positionen außerhalb des definierten Verlaufsbereichs werden folgende Modi unterstützt:

| Modus | Verhalten |
|---|---|
| Clamp | Randfarbe fortsetzen |
| Repeat | Farbverlauf wiederholen |
| Mirror | Farbverlauf abwechselnd spiegeln |
| Transparent | Außerhalb des Bereichs transparent darstellen |

Der Erweiterungsmodus wird pro Brush festgelegt.

Das Verhalten MUSS unabhängig vom Rendering-Backend dieselbe Semantik besitzen.

## 9. Image und Pattern Brushes

Image Brushes verwenden Bildressourcen aus Nova Image Rendering.

Unterstützt werden:

- Skalierung.
- Rotation.
- Positionierung.
- Wiederholung.
- Spiegelnde Wiederholung.
- Zuschneiden.
- Konfigurierbare Bildfilterung.

Pattern Brushes ermöglichen die wiederholte Darstellung geometrischer oder rasterbasierter Muster.

Die Wiederholung MUSS unabhängig von der Größe der zu füllenden Geometrie funktionieren.

Bildressourcen werden über den Nova Graphics Resource Lifecycle verwaltet.

## 10. Koordinaten und Transformationen

Brushes unterstützen zwei grundlegende Koordinatenmodelle:

| Modell | Beschreibung |
|---|---|
| Object Space | Brush-Koordinaten relativ zur Geometrie |
| User Space | Brush-Koordinaten im logischen Zeichenraum |

Zusätzlich kann jeder Brush eine eigene Transformationsmatrix besitzen.

Damit sind beispielsweise Farbverläufe möglich, die unabhängig von der Rotation eines Objekts ausgerichtet bleiben.

Die Reihenfolge von Geometrie- und Brush-Transformationen MUSS eindeutig definiert sein.

Nicht invertierbare Brush-Transformationen werden kontrolliert behandelt.

## 11. Transparenz und Clipping

Brushes unterstützen globale Deckkraft und individuelle Alphawerte.

Die resultierende Darstellung berücksichtigt:

- Geometrische Pixelabdeckung.
- Brush-Farbe und Brush-Alpha.
- Globale Deckkraft.
- Aktive Masken.
- Clipping.
- Blending-Modus.

Die Verarbeitung MUSS mit Nova Clipping and Masking sowie Nova Anti-Aliasing kompatibel sein.

Brushes DÜRFEN keine Pixel außerhalb der gültigen geometrischen Abdeckung verändern.

## 12. Ressourcen und Optimierung

Das System unterstützt:

- Wiederverwendbare Brush-Ressourcen.
- Caching vorberechneter Farbverläufe.
- GPU-basierte Verlaufsberechnung.
- SIMD-optimierte CPU-Verarbeitung.
- Wiederverwendung von Pattern- und Texturressourcen.
- Vermeidung unnötiger Speicherallokationen.

Unveränderliche Brushes SOLLEN zwischen mehreren Zeichenoperationen gemeinsam genutzt werden.

Änderungen an Brush-Parametern MÜSSEN abhängige Caches invalidieren.

Die Darstellung einfacher Solid Brushes SOLL ohne zusätzliche Texturressourcen erfolgen.

## 13. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS ein einheitliches Brush-System bereitstellen.
- **GRAPHICS-002:** Brushes MÜSSEN unabhängig von Geometrien definiert werden können.
- **GRAPHICS-003:** Solid, Linear Gradient, Radial Gradient, Conic Gradient, Image und Pattern Brushes MÜSSEN unterstützt werden.
- **GRAPHICS-004:** Farbverläufe MÜSSEN mehrere Farbstützpunkte unterstützen.
- **GRAPHICS-005:** Farbinterpolation MUSS farbraumbewusst erfolgen.
- **GRAPHICS-006:** Transparente Farbverläufe MÜSSEN unterstützt werden.
- **GRAPHICS-007:** Clamp, Repeat, Mirror und Transparent MÜSSEN als Erweiterungsmodi verfügbar sein.
- **GRAPHICS-008:** Brushes MÜSSEN eigene Transformationen unterstützen.
- **GRAPHICS-009:** Object Space und User Space MÜSSEN als Koordinatenmodelle verfügbar sein.
- **GRAPHICS-010:** Brushes MÜSSEN mit Clipping, Masking und Anti-Aliasing kombinierbar sein.
- **GRAPHICS-011:** Sämtliche grundlegenden Brush-Typen MÜSSEN über CPU-Rendering darstellbar sein.
- **GRAPHICS-012:** GPU-Beschleunigung SOLL bei geeigneter Hardware verwendet werden.
- **GRAPHICS-013:** Brush-Ressourcen MÜSSEN wiederverwendbar sein.
- **GRAPHICS-014:** Ressourcen und Caches MÜSSEN kontrolliert verwaltet werden.
- **GRAPHICS-015:** Ungültige Brush-Parameter DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- **GRAPHICS-016:** Die Brush-Funktionen MÜSSEN über das portable Nova Framework verfügbar sein.
- **GRAPHICS-017:** Die Verarbeitung MUSS deterministisch und ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 14. Fehlerbehandlung

- Ungültige Farbwerte und nicht endliche Parameter werden zurückgewiesen.
- Farbverläufe ohne gültige Farbstützpunkte werden kontrolliert behandelt.
- Degenerierte Verlaufsgeometrien erhalten ein definiertes Fehler- oder Ersatzverhalten.
- Nicht invertierbare Transformationen werden erkannt.
- Fehlende Bildressourcen werden eindeutig gemeldet.
- Fehlgeschlagene Cache- oder GPU-Allokationen DÜRFEN andere Grafikressourcen nicht beschädigen.
- Bei GPU-Ausfall SOLL die Darstellung über den CPU Renderer fortgesetzt werden.

## 15. Ergebnis

NovaOS erhält ein einheitliches System zur Gestaltung geometrischer Flächen und Konturen.

Farben, Farbverläufe, Texturen und Muster können unabhängig von der Geometrie definiert, transformiert und wiederverwendet werden.

Die Architektur ermöglicht hochwertige grafische Oberflächen bei effizienter CPU- und GPU-Nutzung.

**Grundsatz:** Die Geometrie bestimmt, wo gezeichnet wird. Der Brush bestimmt, wie dieser Bereich dargestellt wird.