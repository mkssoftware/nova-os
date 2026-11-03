# NPSPEC-RENDER-SCISSOR-0001 – Nova Scissor Rendering

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Engine

## 1. Zweck

Nova Scissor Rendering definiert die hardware- und softwaregestützte Begrenzung von Rendering-Operationen auf rechteckige Bildbereiche.

Scissor Rendering verhindert Schreiboperationen außerhalb definierter Bereiche eines Render Targets.

Ziel ist die effiziente Umsetzung von Partial Rendering, Region-Based Rendering und grafischem Clipping bei möglichst geringem Verarbeitungsaufwand.

## 2. Architekturprinzipien

- **Region-Limited:** Schreiboperationen werden räumlich begrenzt.
- **Damage-Aware:** Scissor-Bereiche können aus Damage Regions abgeleitet werden.
- **Hardware-Accelerated:** GPU-Scissor-Funktionen werden bevorzugt verwendet.
- **Backend-Independent:** Die Schnittstelle ist unabhängig von konkreter Grafikhardware.
- **Composable:** Mehrere räumliche Begrenzungen können kombiniert werden.
- **State-Aware:** Scissor-Zustände werden explizit verwaltet.
- **Fallback-Safe:** Softwarebasiertes Clipping bleibt möglich.

## 3. Scissor-Modell

Ein Scissor Rectangle beschreibt einen rechteckigen Bereich innerhalb eines Render Targets.

Es besitzt folgende Eigenschaften:

- **X:** Horizontale Startposition.
- **Y:** Vertikale Startposition.
- **Width:** Breite des Bereichs.
- **Height:** Höhe des Bereichs.
- **Target:** Zugehöriges Render Target.

Die Koordinaten werden in ganzzahligen Pixelkoordinaten des jeweiligen Rendering-Ziels angegeben.

Die Render Engine MUSS Scissor Rectangles auf gültige Render-Target-Grenzen begrenzen.

Leere Scissor Rectangles DÜRFEN keine Pixel verändern.

## 4. Scissor State

NovaOS verwaltet den aktiven Scissor-Zustand als Bestandteil des Rendering-Kontexts.

Dieser umfasst:

- Aktivierungszustand
- Aktuelles Scissor Rectangle
- Zugehöriges Render Target
- Gültigen Koordinatenraum

Scissor-Zustandsänderungen MÜSSEN vor der Ausführung betroffener Rendering-Befehle wirksam werden.

Der Zustand DARF innerhalb von Command Buffers gespeichert werden.

## 5. Scissor Rendering

Die Render Engine begrenzt Rasterisierungs- und Schreiboperationen auf den aktiven Scissor-Bereich.

Dabei gilt:

- Pixel außerhalb des Scissor Rectangle DÜRFEN nicht verändert werden.
- Rendering-Befehle DÜRFEN geometrisch über den Scissor-Bereich hinausreichen.
- Fragmente außerhalb des Bereichs werden verworfen.
- Der Scissor-Zustand gilt für die jeweils unterstützten Rendering-Operationen.

Scissor Rendering MUSS von geometrischem Clipping und der Ermittlung sichtbarer Szenenelemente unterschieden werden.

## 6. Damage Integration

Nova Damage Tracking stellt die betroffenen Bildbereiche bereit.

Die Render Engine DARF daraus Scissor Rectangles erzeugen.

Dabei MUSS:

- Die vollständige Damage Region abgedeckt werden.
- Die Koordinatentransformation korrekt erfolgen.
- Die Begrenzung des Render Targets berücksichtigt werden.
- Eine konservative Erweiterung möglich sein.

Mehrere Damage Regions DÜRFEN durch mehrere Scissor-Bereiche oder einen gemeinsamen umschließenden Bereich verarbeitet werden.

## 7. Mehrere Scissor Rectangles

NovaOS MUSS mehrere getrennte Damage Regions unterstützen können.

Da einzelne Rendering-Backends möglicherweise nur einen aktiven Scissor-Bereich pro Draw Call unterstützen, DARF die Render Engine:

- Rendering-Befehle mit unterschiedlichen Scissor Rectangles wiederholen.
- Mehrere Regionen zusammenfassen.
- Regionen über zusätzliche Clip-Masken verarbeiten.
- Auf ein größeres Scissor Rectangle zurückfallen.

Die Strategie SOLL anhand des erwarteten Rendering-Aufwands gewählt werden.

## 8. Kombination mit Clipping

Scissor Rendering kann mit weiteren Begrenzungsverfahren kombiniert werden.

Dazu gehören:

- Geometrisches Clipping
- Hierarchisches Clipping
- Stencil Clipping
- Clip Masks
- Region-Based Rendering

Die effektive Schreibbegrenzung MUSS sämtliche aktiven Clip-Bedingungen berücksichtigen.

Scissor Rendering ersetzt keine komplexen nichtrechteckigen Clip-Verfahren.

## 9. Koordinaten und Transformationen

Scissor Rectangles werden im Koordinatenraum des jeweiligen Render Targets ausgewertet.

NovaOS MUSS bei der Umrechnung berücksichtigen:

- Viewport
- Skalierung
- Transformationen
- Render-Target-Auflösung
- Koordinatenursprung
- Pixelrundung

Die Umrechnung MUSS konservativ erfolgen, sodass erforderliche Pixel nicht versehentlich ausgeschlossen werden.

## 10. Render Graph Integration

Der Nova Render Graph DARF Scissor-Informationen an Render Passes übergeben.

Dabei können:

- Render Passes räumlich begrenzt werden.
- Unnötige Draw Calls vermieden werden.
- Kompatible Scissor-Bereiche zusammengefasst werden.
- Rendering-Aufträge nach Regionen organisiert werden.

Die Render Engine ist für die konkrete Umsetzung der Scissor-Zustände verantwortlich.

## 11. Material- und Effektverarbeitung

Scissor Rendering MUSS grafische Effektabhängigkeiten berücksichtigen.

Dies betrifft insbesondere:

- Blur
- Schatten
- Refraction
- Reflection
- Liquid Glass
- Backdrop Sampling

Dabei MUSS zwischen Eingabe- und Ausgabebereichen unterschieden werden.

Ein Scissor Rectangle DARF die erforderlichen Eingabedaten eines Effekts nicht unzulässig begrenzen.

Die Ausgabe eines Effekts DARF hingegen auf die tatsächlich betroffene Region beschränkt werden.

## 12. Hardwarebeschleunigung

NovaOS SOLL vorhandene GPU-Scissor-Funktionen verwenden.

Die Hardwareimplementierung MUSS über das Rendering-Backend abstrahiert werden.

Bei fehlender Hardwareunterstützung MUSS eine softwarebasierte Begrenzung möglich sein.

Scissor Rendering DARF nicht voraussetzen, dass sämtliche Rendering-Befehle durch die Hardware vollständig übersprungen werden.

Die primäre Garantie besteht in der Begrenzung der betroffenen Ausgabepixel.

## 13. Performance

NovaOS SOLL Scissor Rendering zur Reduzierung unnötiger Pixelverarbeitung einsetzen.

Dabei sind insbesondere zu berücksichtigen:

- Größe des Scissor-Bereichs
- Anzahl der Scissor-Zustandswechsel
- Anzahl erforderlicher Draw Calls
- GPU-Rasterisierungskosten
- Speicherbandbreite
- Render-Target-Eigenschaften

Eine zu starke Aufteilung in kleine Scissor-Bereiche DARF durch Zusammenfassung optimiert werden.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Scissor Rendering unterstützen.
2. Scissor Rectangles MÜSSEN im Koordinatenraum des Render Targets definiert sein.
3. Scissor Rectangles MÜSSEN auf gültige Render-Target-Grenzen begrenzt werden.
4. Pixel außerhalb des aktiven Scissor-Bereichs DÜRFEN durch die betroffenen Rendering-Operationen nicht verändert werden.
5. Leere Scissor Rectangles DÜRFEN keine Pixel verändern.
6. Scissor-Zustände MÜSSEN explizit verwaltbar sein.
7. Damage Regions MÜSSEN in Scissor Rectangles überführbar sein.
8. Mehrere Damage Regions MÜSSEN unterstützt werden können.
9. Koordinatentransformationen MÜSSEN korrekt berücksichtigt werden.
10. Scissor Rendering MUSS mit weiteren Clipping-Verfahren kombinierbar sein.
11. Effektabhängigkeiten DÜRFEN nicht durch unzulässige Begrenzungen verletzt werden.
12. Scissor Rendering MUSS in den Nova Render Graph integrierbar sein.
13. GPU-Scissor-Funktionen SOLLEN bevorzugt genutzt werden.
14. Ein Software-Fallback MUSS verfügbar sein.
15. Die visuelle Korrektheit MUSS unabhängig von der gewählten Scissor-Strategie erhalten bleiben.

## 15. Ergebnis

Nova Scissor Rendering ermöglicht die präzise rechteckige Begrenzung grafischer Schreiboperationen.

Durch die Integration mit Damage Tracking, Partial Rendering und Region-Based Rendering kann NovaOS unnötige Pixelverarbeitung reduzieren.

Scissor Rendering bildet damit einen grundlegenden Mechanismus für die effiziente, inkrementelle Darstellung grafischer Inhalte innerhalb der Nova Render Engine.