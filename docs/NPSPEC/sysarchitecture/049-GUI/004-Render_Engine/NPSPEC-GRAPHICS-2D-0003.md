# NPSPEC-GRAPHICS-2D-0003 – Nova Vector Graphics

**Status:** Angenommen  
**Bereich:** Graphics / 2D / Vector Graphics  
**System:** NovaOS

## 1. Zweck

Nova Vector Graphics definiert die Darstellung und Verarbeitung auflösungsunabhängiger Vektorgrafiken innerhalb der Nova 2D Rendering Engine.

Ziel ist eine präzise, skalierbare und ressourcenschonende Vektorgrafikverarbeitung für Nova UI, Symbole, Diagramme, technische Zeichnungen und grafische Anwendungen.

Die Verarbeitung erfolgt unabhängig vom verwendeten CPU- oder GPU-Backend.

## 2. Architektur

Das Vektorgrafiksystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Vector Renderer | Darstellung von Vektorgeometrien |
| Path Engine | Verwaltung und Verarbeitung geometrischer Pfade |
| Geometry Processor | Berechnung und Transformation von Geometrien |
| Tessellator | Zerlegung komplexer Geometrien für geeignete Rendering-Backends |
| Stroke Engine | Darstellung von Konturen |
| Fill Engine | Verarbeitung von Flächenfüllungen |
| Vector Cache | Wiederverwendung vorberechneter Geometrien |

Das System verwendet Nova Primitive Drawing und die Nova Graphics Backend Abstraction.

Die Vektorgeometrie bleibt unabhängig von ihrer konkreten Rasterisierung.

## 3. Vektormodell

Vektorgrafiken bestehen aus geometrischen Objekten mit definierten Darstellungsattributen.

Unterstützt werden:

- Linien und Polylinien.
- Rechtecke, Kreise und Ellipsen.
- Polygone.
- Quadratische und kubische Bézier-Kurven.
- Offene und geschlossene Pfade.
- Zusammengesetzte Pfade.
- Verschachtelte Geometrien.
- Transformationsgruppen.

Jedes Objekt besitzt eine eindeutige Identität innerhalb seines Vektordokuments oder Rendering-Kontexts.

Geometrie und Darstellungsattribute MÜSSEN getrennt verarbeitet werden können.

## 4. Pfadverarbeitung

Die Path Engine unterstützt folgende Operationen:

| Operation | Beschreibung |
|---|---|
| MoveTo | Startpunkt eines Teilpfades festlegen |
| LineTo | Gerades Liniensegment hinzufügen |
| QuadTo | Quadratische Bézier-Kurve hinzufügen |
| CubicTo | Kubische Bézier-Kurve hinzufügen |
| ArcTo | Bogen hinzufügen |
| ClosePath | Teilpfad schließen |

Pfade können mehrere voneinander unabhängige Teilpfade enthalten.

Zusätzlich werden unterstützt:

- Pfadtransformationen.
- Pfadbegrenzungen.
- Pfadlängenberechnung.
- Punkt-auf-Pfad-Abfragen.
- Punkt-in-Fläche-Prüfungen.
- Geometrische Schnittmengenprüfungen.

Erweiterte geometrische Operationen können durch spezialisierte Framework-Funktionen bereitgestellt werden.

## 5. Konturen und Füllungen

Vektorgeometrien unterstützen:

- Einfarbige Konturen und Füllungen.
- Variable Konturstärken.
- Linienenden und Linienverbindungen.
- Strichmuster.
- Lineare und radiale Farbverläufe.
- Texturfüllungen.
- Transparenz.
- Blending-Modi.

Für geschlossene Pfade werden die Füllregeln `Non-Zero` und `Even-Odd` unterstützt.

Konturen und Füllungen MÜSSEN unabhängig voneinander aktiviert werden können.

## 6. Skalierung und Transformation

Vektorgrafiken verwenden logische Koordinaten und sind nicht an eine feste Pixelauflösung gebunden.

Unterstützt werden:

- Translation.
- Rotation.
- Skalierung.
- Spiegelung.
- Affine Transformationen.
- Verschachtelte Transformationen.

Die ursprüngliche Vektorgeometrie bleibt bei Skalierungen erhalten.

Die Rasterisierung erfolgt entsprechend der Zielauflösung und dem aktuellen Transformationszustand.

## 7. Rasterisierung und Tessellation

Die Darstellung erfolgt über CPU- oder GPU-basierte Rendering-Backends.

Der Renderer entscheidet anhand der verfügbaren Fähigkeiten zwischen:

- Direkter Pfadrasterisierung.
- Scanline-Rasterisierung.
- Tessellation.
- Wiederverwendung zwischengespeicherter Geometrien.

Kurven werden mit einer definierten, auf die Zielauflösung abgestimmten Genauigkeit verarbeitet.

Die Tessellation DARF die ursprüngliche Vektorgeometrie nicht dauerhaft verändern.

Antialiasing MUSS unterstützt werden.

## 8. Vektorszenen

Nova Vector Graphics unterstützt hierarchisch organisierte Vektorobjekte.

Eine Vektorszene kann enthalten:

- Geometrieobjekte.
- Transformationsgruppen.
- Clipping-Bereiche.
- Darstellungsattribute.
- Referenzen auf wiederverwendbare Geometrien.

Die Zeichenreihenfolge wird durch die Szenenstruktur bestimmt.

Änderungen einzelner Objekte SOLLEN ohne vollständige Neuberechnung der gesamten Szene verarbeitet werden können.

Die Vektorszene ist unabhängig vom UI-Layoutsystem und kann auch außerhalb von Nova UI verwendet werden.

## 9. Caching und Optimierung

Zur Leistungsoptimierung unterstützt das System:

- Caching vorberechneter Pfade.
- Wiederverwendung tessellierter Geometrien.
- Caching rasterisierter Vektorobjekte.
- Geometrisches Clipping.
- Dirty-Region-Updates.
- Zusammenfassung kompatibler Zeichenoperationen.
- CPU- und GPU-Beschleunigung.

Caches MÜSSEN bei Änderungen relevanter Geometrie-, Transformations- oder Darstellungsparameter invalidiert werden.

Zwischengespeicherte Rastergrafiken DÜRFEN nicht als Ersatz für die ursprünglichen Vektordaten verwendet werden.

## 10. Integration in NovaOS

### Nova 2D Rendering Engine

Übernimmt die eigentliche Darstellung der Vektorgeometrien.

### Nova Primitive Drawing

Stellt grundlegende geometrische Zeichenoperationen bereit.

### Nova UI

Verwendet Vektorgrafiken für skalierbare Symbole, Controls und Oberflächenelemente.

### Nova Framework

Stellt die Vektorgrafikfunktionen plattformübergreifend bereit.

### Nova Graphics Backend

Ermöglicht CPU- und GPU-basierte Rasterisierung.

Vektordateiformate und deren Import oder Export werden durch separate Framework-Komponenten verarbeitet.

## 11. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS eine auflösungsunabhängige Vektorgrafikverarbeitung bereitstellen.
- GRAPHICS-002: Vektorgeometrien MÜSSEN unabhängig von ihrer Rasterisierung gespeichert werden können.
- GRAPHICS-003: Die Path Engine MUSS offene und geschlossene Pfade unterstützen.
- GRAPHICS-004: Quadratische und kubische Bézier-Kurven MÜSSEN unterstützt werden.
- GRAPHICS-005: Pfade MÜSSEN mehrere Teilpfade enthalten können.
- GRAPHICS-006: Konturen und Füllungen MÜSSEN unabhängig konfigurierbar sein.
- GRAPHICS-007: Die Füllregeln Non-Zero und Even-Odd MÜSSEN unterstützt werden.
- GRAPHICS-008: Geometrische Transformationen MÜSSEN unterstützt werden.
- GRAPHICS-009: Vektorgrafiken MÜSSEN ohne GPU-Beschleunigung darstellbar sein.
- GRAPHICS-010: GPU-Beschleunigung SOLL bei geeigneter Hardware verwendet werden.
- GRAPHICS-011: Antialiasing MUSS unterstützt werden.
- GRAPHICS-012: Die Rasterisierungsgenauigkeit MUSS definierten Toleranzen entsprechen.
- GRAPHICS-013: Wiederverwendbare Geometrien MÜSSEN zwischengespeichert werden können.
- GRAPHICS-014: Änderungen an Geometrien MÜSSEN abhängige Caches invalidieren.
- GRAPHICS-015: Die Zeichenreihenfolge MUSS erhalten bleiben.
- GRAPHICS-016: Ungültige Geometrien DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- GRAPHICS-017: Die Vektorgrafikfunktionen MÜSSEN Bestandteil des portablen Nova Framework sein können.
- GRAPHICS-018: Die Verarbeitung MUSS ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 12. Fehlerbehandlung

- Ungültige Pfadoperationen werden zurückgewiesen.
- Nicht endliche Koordinatenwerte werden erkannt.
- Numerische Überläufe MÜSSEN verhindert oder kontrolliert behandelt werden.
- Fehlerhafte Geometrien DÜRFEN unabhängige Vektorobjekte nicht beschädigen.
- Nicht unterstützte Rendering-Funktionen werden eindeutig gemeldet.
- Bei GPU-Ausfall SOLL die Darstellung über den CPU Renderer fortgesetzt werden.

## 13. Ergebnis

NovaOS erhält eine skalierbare und hardwareunabhängige Vektorgrafikverarbeitung.

Geometrien, Pfade und komplexe Vektorszenen können präzise dargestellt, transformiert und wiederverwendet werden.

Die Architektur bildet die Grundlage für auflösungsunabhängige Benutzeroberflächen, Symbole, Diagramme und technische Grafikanwendungen.

**Grundsatz:** Vektorgrafiken beschreiben geometrische Inhalte unabhängig von Pixeln. Die konkrete Darstellung passt sich dem Rendering-Ziel an, ohne die ursprüngliche Geometrie zu verändern.