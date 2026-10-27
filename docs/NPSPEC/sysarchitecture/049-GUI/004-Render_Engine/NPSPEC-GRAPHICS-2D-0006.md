# NPSPEC-GRAPHICS-2D-0006 – Nova Transformations

**Status:** Angenommen  
**Bereich:** Graphics / 2D / Transformations  
**System:** NovaOS

## 1. Zweck

Nova Transformations definiert die einheitliche Verarbeitung geometrischer Transformationen innerhalb der Nova 2D Rendering Engine.

Ziel ist die präzise und effiziente Positionierung, Skalierung, Rotation und Verformung zweidimensionaler Grafikelemente.

Transformationen sind unabhängig vom verwendeten Rendering-Backend und stehen über das portable Nova Framework zur Verfügung.

## 2. Architektur

Das Transformationssystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Transform Manager | Verwaltung von Transformationen |
| Matrix Engine | Berechnung und Kombination von Matrizen |
| Coordinate Mapper | Umrechnung zwischen Koordinatenräumen |
| Transform Stack | Verwaltung verschachtelter Transformationen |
| Transform Processor | Anwendung auf Geometrien und Zeichenoperationen |
| Backend Adapter | Umsetzung durch CPU- oder GPU-Rendering |

Transformationen werden über den Drawing Context bereitgestellt.

Die ursprünglichen Geometriedaten bleiben grundsätzlich unverändert.

## 3. Transformationstypen

NovaOS unterstützt folgende Transformationen:

| Transformation | Beschreibung |
|---|---|
| Translation | Verschiebung |
| Scaling | Skalierung |
| Rotation | Drehung |
| Reflection | Spiegelung |
| Shearing | Scherung |
| Affine Transformation | Kombination affiner Transformationen |
| Matrix Transformation | Direkte Verwendung einer Transformationsmatrix |

Transformationen können einzeln oder kombiniert angewendet werden.

Negative Skalierungsfaktoren ermöglichen Spiegelungen.

## 4. Matrixmodell

Für affine 2D-Transformationen verwendet NovaOS homogene 3×3-Matrizen.

Die Standarddarstellung lautet:

    | m11  m12  dx |
    | m21  m22  dy |
    |  0    0    1 |

Punkte werden als Spaltenvektoren verarbeitet.

Die Transformation erfolgt durch:

    P' = M × P

Dabei gilt:

- `P`: Ursprünglicher Punkt.
- `M`: Transformationsmatrix.
- `P'`: Transformierter Punkt.

Die Identitätsmatrix stellt den unveränderten Zustand dar.

Die Reihenfolge der Matrixmultiplikation MUSS eindeutig definiert sein.

## 5. Transformationsreihenfolge

Transformationen sind grundsätzlich nicht kommutativ.

NovaOS verwendet folgende Festlegung:

- Punkte werden als Spaltenvektoren dargestellt.
- Transformationen werden durch Matrixmultiplikation kombiniert.
- Bei `M = A × B` wird zuerst `B`, anschließend `A` angewendet.
- Neue lokale Transformationen werden standardmäßig rechts an die bestehende Matrix multipliziert.

Die API DARF zusätzliche explizite Operationen für Vor- und Nachmultiplikation bereitstellen.

Die Reihenfolge MUSS unabhängig vom Rendering-Backend identisch sein.

## 6. Koordinatenräume

NovaOS unterscheidet:

| Koordinatenraum | Beschreibung |
|---|---|
| Local Space | Koordinaten eines Grafikelements |
| Parent Space | Koordinaten des übergeordneten Elements |
| World Space | Gemeinsamer logischer Szenenraum |
| View Space | Koordinaten des aktuellen Darstellungsbereichs |
| Device Space | Physische Koordinaten des Rendering-Ziels |

Transformationen ermöglichen die Umrechnung zwischen diesen Räumen.

Die Umrechnung von logischen Koordinaten in physische Pixel berücksichtigt die Skalierung des Rendering-Ziels.

Die Transformationen DÜRFEN keine feste Displayauflösung voraussetzen.

## 7. Transform Stack

Der Transform Stack ermöglicht verschachtelte Transformationen.

Unterstützte Operationen:

- `PushTransform`
- `PopTransform`
- `SetTransform`
- `ResetTransform`
- `MultiplyTransform`
- `GetTransform`

Ein gespeicherter Transformationszustand kann nach verschachtelten Zeichenoperationen wiederhergestellt werden.

Ein `PopTransform` ohne entsprechenden gespeicherten Zustand MUSS kontrolliert zurückgewiesen werden.

Unabhängige Drawing Contexts besitzen voneinander getrennte Transformationszustände.

## 8. Transformationsursprung

Rotationen und Skalierungen können um einen frei definierbaren Ursprung erfolgen.

Unterstützt werden:

- Lokaler Ursprung.
- Mittelpunkt einer Geometrie.
- Benutzerdefinierter Transformationspunkt.

Eine Transformation um einen Ursprung wird durch entsprechende Matrixkombinationen umgesetzt.

Die Engine MUSS den Transformationsursprung unabhängig von der ursprünglichen Geometrie berücksichtigen können.

## 9. Inverse Transformationen

Das System unterstützt die Berechnung inverser Transformationsmatrizen.

Anwendungsfälle:

- Koordinatenumrechnung.
- Hit Testing.
- Maus- und Touchpositionen.
- Geometrische Abfragen.
- Rücktransformation von Punkten.

Nicht invertierbare Matrizen MÜSSEN erkannt werden.

Die Engine DARF bei fehlender Invertierbarkeit keine ungültigen Koordinaten zurückgeben, ohne den Fehler kenntlich zu machen.

## 10. Rendering-Verhalten

Transformationen wirken auf:

- Geometrische Primitive.
- Vektorpfade.
- Bilder.
- Textdarstellungen.
- Clipping-Geometrien.
- Masken.
- Verschachtelte Grafikobjekte.

Die Anwendung erfolgt vor der endgültigen Rasterisierung.

Bereits definierte Clip-Regionen behalten ihre festgelegte Bedeutung im jeweiligen Koordinatenraum.

Transformationen MÜSSEN mit den Regeln von Nova Clipping and Masking übereinstimmen.

## 11. Leistungsoptimierung

Das System unterstützt:

- Caching kombinierter Transformationsmatrizen.
- SIMD-optimierte Matrixoperationen.
- GPU-basierte Transformationen.
- Vermeidung wiederholter Matrixberechnungen.
- Wiederverwendung unveränderter Geometrien.
- Frühzeitige Sichtbarkeitsprüfung transformierter Objekte.

Transformationen SOLLEN möglichst ohne Veränderung der ursprünglichen Geometriedaten ausgeführt werden.

Caches MÜSSEN bei Änderungen relevanter Transformationsparameter invalidiert werden.

## 12. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS ein einheitliches 2D-Transformationssystem bereitstellen.
- GRAPHICS-002: Translation, Skalierung, Rotation, Spiegelung und Scherung MÜSSEN unterstützt werden.
- GRAPHICS-003: Affine Transformationen MÜSSEN über 3×3-Matrizen darstellbar sein.
- GRAPHICS-004: Die Matrixkonvention und Multiplikationsreihenfolge MÜSSEN eindeutig definiert sein.
- GRAPHICS-005: Transformationen MÜSSEN kombinierbar sein.
- GRAPHICS-006: Verschachtelte Transformationen MÜSSEN unterstützt werden.
- GRAPHICS-007: Transformationszustände MÜSSEN gespeichert und wiederhergestellt werden können.
- GRAPHICS-008: Die Umrechnung zwischen Koordinatenräumen MUSS unterstützt werden.
- GRAPHICS-009: Benutzerdefinierte Transformationsursprünge MÜSSEN möglich sein.
- GRAPHICS-010: Inverse Transformationen MÜSSEN berechnet werden können.
- GRAPHICS-011: Nicht invertierbare Matrizen MÜSSEN erkannt werden.
- GRAPHICS-012: Transformationen MÜSSEN unabhängig vom Rendering-Backend dieselbe Semantik besitzen.
- GRAPHICS-013: Sämtliche grundlegenden Transformationen MÜSSEN ohne GPU-Beschleunigung funktionieren.
- GRAPHICS-014: GPU-Beschleunigung SOLL bei geeigneter Hardware verwendet werden.
- GRAPHICS-015: Die ursprünglichen Geometriedaten SOLLEN bei Transformationen unverändert bleiben.
- GRAPHICS-016: Ungültige Matrixwerte DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- GRAPHICS-017: Die Transformationsfunktionen MÜSSEN im portablen Nova Framework verfügbar sein.
- GRAPHICS-018: Die Verarbeitung MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

- Nicht endliche Matrixwerte werden zurückgewiesen.
- Numerische Überläufe werden erkannt oder kontrolliert behandelt.
- Nicht invertierbare Matrizen erzeugen einen definierten Fehlerstatus.
- Ungültige Transformationszustände DÜRFEN unabhängige Drawing Contexts nicht beeinflussen.
- Fehlgeschlagene Matrixoperationen DÜRFEN den bisherigen gültigen Transformationszustand nicht unkontrolliert verändern.
- Bei GPU-Ausfall SOLL die Verarbeitung über den CPU Renderer fortgesetzt werden.

## 14. Ergebnis

NovaOS erhält ein einheitliches und portables Transformationssystem für zweidimensionale Grafiken.

Geometrien, Bilder und Oberflächenelemente können unabhängig von Auflösung und Rendering-Backend positioniert, skaliert, gedreht und kombiniert werden.

Die Architektur ermöglicht verschachtelte Koordinatensysteme, präzise geometrische Berechnungen und effiziente Rendering-Optimierungen.

**Grundsatz:** Transformationen verändern die Darstellung und Position geometrischer Inhalte, ohne deren ursprüngliche Daten verändern zu müssen.