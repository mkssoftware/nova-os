# NPSPEC-GRAPHICS-2D-0001 – Nova 2D Rendering Engine

**Status:** Angenommen  
**Bereich:** Graphics / 2D Rendering  
**System:** NovaOS

## 1. Zweck

Nova 2D Rendering Engine definiert die zweidimensionale Grafikverarbeitung von NovaOS.

Ziel ist eine schnelle, präzise und ressourcenschonende Rendering-Engine für Benutzeroberflächen, Anwendungen, Grafiken und Systemdarstellungen.

Die Engine unterstützt CPU- und GPU-Rendering über eine einheitliche Schnittstelle und ist unabhängig vom Window Manager, Compositor und Nova UI nutzbar.

## 2. Architektur

Die 2D Rendering Engine ist Bestandteil des portablen Nova Framework und verwendet die Nova Graphics API.

| Komponente | Aufgabe |
|---|---|
| 2D Renderer | Verarbeitung zweidimensionaler Grafikoperationen |
| Drawing Context | Verwaltung des aktuellen Zeichenzustands |
| Geometry Engine | Verarbeitung geometrischer Formen |
| Rasterizer | Umwandlung von Geometrie in Pixel |
| Paint Engine | Farben, Farbverläufe und Füllmuster |
| Image Renderer | Darstellung und Transformation von Bildern |
| Text Renderer Interface | Anbindung der Text- und Schriftverarbeitung |
| 2D Command Processor | Aufzeichnung und Ausführung von Zeichenbefehlen |

Die Engine verwendet die Graphics Backend Abstraction zur Auswahl geeigneter CPU- oder GPU-Implementierungen.

## 3. Rendering-Modell

Die Engine unterstützt zwei Betriebsarten:

- **Immediate Rendering:** Zeichenoperationen werden unmittelbar zur Ausführung übergeben.
- **Retained Rendering:** Zeichenoperationen werden als wiederverwendbare Darstellungsdaten gespeichert.

Beide Betriebsarten verwenden dieselben grundlegenden Grafikoperationen.

Retained Rendering ermöglicht die gezielte Aktualisierung veränderter Inhalte.

Die Engine DARF keine dauerhafte Speicherung vollständiger Szenen voraussetzen.

## 4. Zeichenoperationen

Die Engine unterstützt folgende Grundfunktionen:

- Punkte und Linien.
- Rechtecke und abgerundete Rechtecke.
- Kreise und Ellipsen.
- Polygone.
- Bézier-Kurven.
- Vektorpfade.
- Konturen und Flächenfüllungen.
- Bilder und Texturen.
- Farbverläufe.
- Transparenz und Alpha-Blending.
- Clipping und Maskierung.
- Geometrische Transformationen.

Komplexe Formen werden durch kombinierbare Pfadoperationen dargestellt.

Die Ausgabe MUSS unabhängig vom verwendeten Rendering-Backend eine definierte Semantik besitzen.

## 5. Drawing Context

Jeder Drawing Context verwaltet den aktuellen Zeichenzustand.

Dazu gehören:

- Rendering-Ziel.
- Transformationsmatrix.
- Clipping-Bereich.
- Farbe und Füllstil.
- Konturstil und Linienstärke.
- Transparenz.
- Blending-Modus.
- Antialiasing-Einstellungen.

Zeichenzustände können gespeichert und wiederhergestellt werden.

Änderungen eines Drawing Context DÜRFEN andere unabhängige Drawing Contexts nicht beeinflussen.

## 6. Koordinaten und Transformationen

Die Engine verwendet ein einheitliches zweidimensionales Koordinatensystem.

Unterstützt werden:

- Translation.
- Skalierung.
- Rotation.
- Spiegelung.
- Affine Transformationen.
- Verschachtelte Transformationen.
- Umrechnung zwischen logischen und physischen Koordinaten.

Logische Koordinaten werden unabhängig von der tatsächlichen Displayauflösung verarbeitet.

Die Umrechnung in physische Pixel erfolgt unter Berücksichtigung des jeweiligen Rendering-Ziels.

## 7. Rasterisierung

Der Rasterizer wandelt geometrische Objekte in darstellbare Pixel um.

Unterstützt werden:

- Füllung geometrischer Flächen.
- Rasterisierung von Linien und Kurven.
- Antialiasing.
- Pixelgenaues Clipping.
- Alpha-Blending.
- Unterschiedliche Pixelformate.

Die Rasterisierung MUSS definierte Regeln für Pixelabdeckung und Transparenz verwenden.

CPU- und GPU-Implementierungen SOLLEN visuell konsistente Ergebnisse erzeugen.

Hardwarebedingte Rundungsabweichungen sind zulässig.

## 8. Farben und Transparenz

Die Engine verwendet das zentrale Nova Graphics Color System.

Unterstützt werden:

- RGB- und RGBA-Farben.
- Transparenz.
- Lineare und nichtlineare Farbverarbeitung.
- Farbverläufe.
- Unterschiedliche Blending-Modi.
- Farbkonvertierungen.

Farboperationen MÜSSEN den definierten Farbraum berücksichtigen.

Die Engine DARF keine feste Displayfarbtiefe voraussetzen.

HDR-Funktionen werden verwendet, sofern das Rendering-Ziel und das Backend diese unterstützen.

## 9. Bildverarbeitung

Der Image Renderer unterstützt:

- Darstellung von Bitmapgrafiken.
- Skalierung.
- Zuschneiden.
- Rotation.
- Spiegelung.
- Transparenz.
- Bildfilterung.
- Teilbereichsaktualisierungen.

Bilddecodierung und Dateiformatverarbeitung sind nicht Bestandteil der eigentlichen Rendering-Engine.

Sie werden über geeignete Funktionen des Nova Framework bereitgestellt.

## 10. Textdarstellung

Die Engine stellt eine Schnittstelle für die Text- und Schriftverarbeitung bereit.

Dazu gehören:

- Darstellung vorbereiteter Glyphen.
- Positionierung von Glyphen.
- Textfarben und Transparenz.
- Clipping.
- Transformationen.
- Antialiasing.

Schriftanalyse, Textlayout, Unicode-Verarbeitung und Font-Shaping erfolgen durch das zuständige Text-Subsystem.

Die 2D Engine übernimmt die grafische Darstellung der vorbereiteten Textdaten.

## 11. Rendering-Optimierung

Die Engine unterstützt folgende Optimierungen:

- Dirty-Region-Rendering.
- Tile-basiertes Rendering.
- Wiederverwendung vorberechneter Geometrien.
- Caching von Rasterisierungsdaten.
- Zusammenfassung kompatibler Zeichenoperationen.
- SIMD-Beschleunigung.
- Parallele Verarbeitung unabhängiger Bildbereiche.
- GPU-Batching.
- Vermeidung unnötiger Speicherübertragungen.

Optimierungen DÜRFEN die definierte Zeichenreihenfolge und das sichtbare Ergebnis nicht unzulässig verändern.

Nicht benötigte Ressourcen SOLLEN automatisch freigegeben werden.

## 12. Integration in NovaOS

### Nova UI

Verwendet die 2D Rendering Engine zur Darstellung von Controls, Oberflächen, Symbolen und grafischen Elementen.

### Nova Compositor

Verwendet die Engine für geeignete zweidimensionale Kompositionsoperationen und Effekte.

### Nova Graphics Core

Verwaltet Rendering-Ressourcen, Befehle und Synchronisation.

### Nova Graphics Backend

Stellt CPU- oder GPU-basierte Rendering-Implementierungen bereit.

### Nova Framework

Stellt die 2D Rendering Engine auch für portable Anwendungen auf unterstützten Fremdsystemen bereit.

## 13. Normative Anforderungen

- GRAPHICS-001: NovaOS MUSS eine einheitliche 2D Rendering Engine bereitstellen.
- GRAPHICS-002: Die Engine MUSS unabhängig vom Window Manager und Compositor verwendbar sein.
- GRAPHICS-003: Grundlegende 2D-Operationen MÜSSEN ohne GPU-Beschleunigung funktionieren.
- GRAPHICS-004: Hardwarebeschleunigung MUSS über die Graphics Backend Abstraction eingebunden werden können.
- GRAPHICS-005: Die Engine MUSS geometrische Formen, Pfade und Bilder darstellen können.
- GRAPHICS-006: Transformationen und Clipping MÜSSEN unterstützt werden.
- GRAPHICS-007: Alpha-Blending und Antialiasing MÜSSEN unterstützt werden.
- GRAPHICS-008: Drawing Contexts MÜSSEN unabhängig voneinander verwaltet werden können.
- GRAPHICS-009: Die Rendering-Semantik MUSS backendunabhängig definiert sein.
- GRAPHICS-010: Logische Koordinaten MÜSSEN unabhängig von der physischen Displayauflösung verwendbar sein.
- GRAPHICS-011: Immediate Rendering MUSS unterstützt werden.
- GRAPHICS-012: Retained Rendering MUSS architektonisch unterstützt werden.
- GRAPHICS-013: Partielle Bildaktualisierungen SOLLEN unterstützt werden.
- GRAPHICS-014: Die Engine MUSS die zentrale Farbverwaltung berücksichtigen.
- GRAPHICS-015: Grafikressourcen MÜSSEN gemäß Nova Graphics Resource Lifecycle verwaltet werden.
- GRAPHICS-016: Fehlerhafte Zeichenoperationen DÜRFEN keine fremden Speicherbereiche beschädigen.
- GRAPHICS-017: Die Engine MUSS Bestandteil des portablen Nova Framework sein können.
- GRAPHICS-018: Die Rendering-Ausführung MUSS ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 14. Fehlerbehandlung

- Ungültige Geometrien werden kontrolliert zurückgewiesen.
- Nicht unterstützte Zeichenoperationen werden eindeutig gemeldet.
- Ungültige Ressourcenhandles dürfen nicht verwendet werden.
- Fehlerhafte Transformationen dürfen keine unkontrollierten Speicherzugriffe verursachen.
- Bei GPU-Ausfall SOLL auf den CPU Renderer gewechselt werden.
- Fehlgeschlagene Rendering-Aufträge werden an den Graphics Core gemeldet.
- Unabhängige Drawing Contexts SOLLEN durch lokale Rendering-Fehler nicht beeinträchtigt werden.

## 15. Ergebnis

NovaOS erhält eine einheitliche, portable und leistungsfähige 2D Rendering Engine.

Sie bildet die grafische Grundlage für Nova UI, Systemprogramme und weitere zweidimensionale Darstellungen.

Durch CPU- und GPU-Unterstützung, moderne Vektorgrafikfunktionen und gezielte Rendering-Optimierungen ermöglicht sie eine flüssige und ressourcenschonende Darstellung.

**Grundsatz:** Die Nova 2D Rendering Engine stellt grafische Inhalte unabhängig von Fensterverwaltung, Displayhardware und konkretem Rendering-Backend dar.