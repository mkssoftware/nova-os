# NPSPEC-GRAPHICS-2D-0005 – Nova Clipping and Masking

**Status:** Angenommen  
**Bereich:** Graphics / 2D / Clipping and Masking  
**System:** NovaOS

## 1. Zweck

Nova Clipping and Masking definiert die Begrenzung und Maskierung grafischer Zeichenoperationen innerhalb der Nova 2D Rendering Engine.

Ziel ist eine präzise, effiziente und backendunabhängige Kontrolle darüber, welche Bildbereiche dargestellt oder verändert werden dürfen.

Das System unterstützt geometrische Begrenzungen, komplexe Masken, Transparenz und verschachtelte Zeichenbereiche.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Clip Manager | Verwaltung aktiver Begrenzungen |
| Clip Region | Beschreibung geometrischer Zeichenbereiche |
| Mask Manager | Verwaltung von Masken |
| Mask Resource | Speicherung von Abdeckungs- und Transparenzwerten |
| Clip Processor | Berechnung sichtbarer Bereiche |
| Mask Processor | Anwendung von Masken |
| Backend Adapter | Umsetzung durch CPU- oder GPU-Rendering |

Clipping und Masking sind Bestandteil des Drawing Context und verwenden die Nova Graphics Backend Abstraction.

## 3. Clipping

Clipping begrenzt Zeichenoperationen auf definierte geometrische Bereiche.

Unterstützt werden:

- Rechteckiges Clipping.
- Abgerundete Rechtecke.
- Elliptische Begrenzungen.
- Pfadbasiertes Clipping.
- Zusammengesetzte Clip-Regionen.
- Verschachtelte Begrenzungen.

Zeichenoperationen außerhalb der aktiven Clip-Region DÜRFEN das Rendering-Ziel nicht verändern.

Teilweise sichtbare Geometrien werden entsprechend ihrer tatsächlichen Überdeckung dargestellt.

## 4. Clip-Operationen

Clip-Regionen können durch folgende Operationen kombiniert werden:

| Operation | Beschreibung |
|---|---|
| Replace | Vorhandene Begrenzung ersetzen |
| Intersect | Schnittmenge bilden |
| Union | Bereiche vereinigen |
| Difference | Bereich abziehen |
| XOR | Symmetrische Differenz bilden |

Die Operationen wirken auf den aktuellen Clipping-Zustand.

Ein leerer Clip-Bereich führt dazu, dass nachfolgende Zeichenoperationen keine sichtbaren Änderungen erzeugen.

## 5. Masking

Masking steuert die Sichtbarkeit einzelner Pixel anhand einer Maske.

Unterstützt werden:

- Binäre Masken.
- Alphamasken.
- Geometrische Masken.
- Rasterbasierte Masken.
- Verschachtelte Masken.

Eine Alphamaske verwendet Abdeckungswerte zwischen `0.0` und `1.0`.

Dabei gilt:

- `0.0`: vollständig ausgeblendet.
- `1.0`: vollständig sichtbar.
- Zwischenwerte: teilweise Abdeckung.

Masken werden mit der Deckkraft der Zeichenoperation kombiniert.

## 6. Koordinaten und Transformationen

Clip-Regionen und Masken besitzen einen definierten Koordinatenraum.

Unterstützt werden:

- Lokale Koordinaten.
- Logische Rendering-Koordinaten.
- Transformationen.
- Skalierung.
- Rotation.
- Verschachtelte Transformationszustände.

Der beim Setzen einer Begrenzung gültige Transformationszustand MUSS eindeutig berücksichtigt werden.

Nachträgliche Änderungen des Drawing Context DÜRFEN bereits festgelegte Clip-Regionen nicht unbeabsichtigt verändern.

## 7. Verschachtelung und Zustandsverwaltung

Clipping und Masking werden über den Drawing Context verwaltet.

Dieser unterstützt:

- Speichern des aktuellen Zustands.
- Hinzufügen weiterer Begrenzungen.
- Kombination mehrerer Masken.
- Wiederherstellen vorheriger Zustände.

Verschachtelte Begrenzungen MÜSSEN in definierter Reihenfolge ausgewertet werden.

Die Zustandsverwaltung MUSS verhindern, dass Änderungen eines Drawing Context unabhängige Rendering-Kontexte beeinflussen.

## 8. Rasterisierung und Antialiasing

Geometrische Begrenzungen werden entsprechend dem Rendering-Ziel rasterisiert.

Dabei gilt:

- Pixelabdeckung wird bei Antialiasing berücksichtigt.
- Teilweise bedeckte Pixel erhalten entsprechende Abdeckungswerte.
- Masken werden mit den definierten Blending-Regeln kombiniert.
- Clipping erfolgt vor dem endgültigen Schreiben in das Rendering-Ziel.

CPU- und GPU-Renderer MÜSSEN dieselbe grundlegende Clipping-Semantik verwenden.

Geringfügige numerische Unterschiede bei der Rasterisierung sind zulässig.

## 9. Leistungsoptimierung

Das System unterstützt:

- Scissor Rectangles für rechteckiges Clipping.
- Stencil Buffers bei geeigneter GPU-Hardware.
- Alphamasken für komplexe Begrenzungen.
- Caching vorberechneter Clip-Regionen.
- Tile-basiertes Clipping.
- Frühzeitige Sichtbarkeitsprüfung.
- Dirty-Region-Optimierung.

Einfache Begrenzungen SOLLEN ohne zusätzliche Maskenbuffer verarbeitet werden.

Komplexe Masken SOLLEN nur für tatsächlich betroffene Bildbereiche berechnet werden.

Optimierungen DÜRFEN die definierte Clipping- und Masking-Semantik nicht verändern.

## 10. Ressourcenverwaltung

Masken und zwischengespeicherte Clip-Daten unterliegen dem Nova Graphics Resource Lifecycle.

Dabei gilt:

- Maskenressourcen besitzen gültige Handles.
- Speicherzugriffe werden begrenzt.
- Gemeinsam verwendete Masken werden synchronisiert.
- Änderungen invalidieren abhängige Caches.
- Nicht mehr benötigte Ressourcen werden kontrolliert freigegeben.

Die Erstellung einer Maske DARF keine unkontrollierte Speicherbelegung verursachen.

## 11. Integration in NovaOS

### Nova 2D Rendering Engine

Verwendet Clipping und Masking für sämtliche unterstützten Zeichenoperationen.

### Nova Primitive Drawing

Begrenzt geometrische Formen, Linien und Pfade.

### Nova Vector Graphics

Verwendet pfadbasierte Begrenzungen und geometrische Masken.

### Nova Image Rendering

Unterstützt Bildmasken, Transparenz und zugeschnittene Bildbereiche.

### Nova UI

Verwendet Clipping für Controls, Scrollbereiche, abgerundete Oberflächen und verschachtelte Layouts.

### Nova Compositor

Kann Masken für Fensterformen, Transparenz und grafische Effekte verwenden.

## 12. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS rechteckiges und pfadbasiertes Clipping unterstützen.
- GRAPHICS-002: Clip-Regionen MÜSSEN kombinierbar sein.
- GRAPHICS-003: Verschachtelte Clip-Regionen MÜSSEN unterstützt werden.
- GRAPHICS-004: Zeichenoperationen DÜRFEN außerhalb der gültigen Clip-Region keine Pixel verändern.
- GRAPHICS-005: Binäre Masken und Alphamasken MÜSSEN unterstützt werden.
- GRAPHICS-006: Masken MÜSSEN mit Transparenz und Blending kombinierbar sein.
- GRAPHICS-007: Transformationen MÜSSEN bei Clipping und Masking berücksichtigt werden.
- GRAPHICS-008: Clipping-Zustände MÜSSEN gespeichert und wiederhergestellt werden können.
- GRAPHICS-009: Antialiasing MUSS für geometrische Begrenzungen unterstützt werden.
- GRAPHICS-010: Die grundlegende Semantik MUSS unabhängig vom Rendering-Backend bleiben.
- GRAPHICS-011: Clipping und Masking MÜSSEN ohne GPU-Beschleunigung funktionieren.
- GRAPHICS-012: Hardwarebeschleunigung SOLL bei geeigneter GPU verwendet werden.
- GRAPHICS-013: Maskenressourcen MÜSSEN kontrolliert verwaltet werden.
- GRAPHICS-014: Änderungen an Masken MÜSSEN abhängige Caches invalidieren.
- GRAPHICS-015: Ungültige Begrenzungen DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- GRAPHICS-016: Clipping und Masking MÜSSEN im portablen Nova Framework verfügbar sein können.
- GRAPHICS-017: Die Verarbeitung MUSS deterministisch und ohne KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

- Ungültige Clip-Geometrien werden kontrolliert zurückgewiesen.
- Nicht endliche Koordinatenwerte werden erkannt.
- Fehlerhafte Maskenabmessungen oder Speicherlayouts werden zurückgewiesen.
- Fehlgeschlagene Maskenoperationen DÜRFEN den bestehenden gültigen Clipping-Zustand nicht unkontrolliert verändern.
- Nicht unterstützte Hardwarefunktionen werden durch geeignete Softwareverfahren ersetzt oder eindeutig gemeldet.
- Bei GPU-Ausfall SOLL die Verarbeitung über den CPU Renderer fortgesetzt werden.

## 14. Ergebnis

NovaOS erhält ein einheitliches System zur geometrischen Begrenzung und pixelgenauen Maskierung grafischer Inhalte.

Clipping und Masking ermöglichen komplexe Oberflächen, transparente Elemente, verschachtelte Zeichenbereiche und effiziente Bildaktualisierungen.

Die Architektur bleibt unabhängig von der verwendeten Hardware und unterstützt sowohl Software- als auch GPU-Rendering.

**Grundsatz:** Jede Zeichenoperation verändert ausschließlich die durch Clipping, Masking und Berechtigungen zugelassenen Bildbereiche.