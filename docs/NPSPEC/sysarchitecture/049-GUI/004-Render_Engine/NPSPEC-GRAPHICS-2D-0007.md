# NPSPEC-GRAPHICS-2D-0007 – Nova Anti-Aliasing

**Status:** Angenommen  
**Bereich:** Graphics / 2D / Anti-Aliasing  
**System:** NovaOS

## 1. Zweck

Nova Anti-Aliasing definiert die Kantenglättung innerhalb der Nova 2D Rendering Engine.

Ziel ist die hochwertige Darstellung von Linien, Kurven, geometrischen Formen und Vektorgrafiken ohne störende Treppeneffekte.

Die Kantenglättung MUSS unabhängig von der verwendeten Hardware funktionieren und sich hinsichtlich Qualität und Ressourcenverbrauch anpassen lassen.

## 2. Architektur

Das Anti-Aliasing-System ist Bestandteil der Rasterisierung und verwendet die Nova Graphics Backend Abstraction.

Es besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| AA Controller | Auswahl und Konfiguration der Glättungsverfahren |
| Coverage Processor | Berechnung der Pixelabdeckung |
| Sample Processor | Verwaltung zusätzlicher Abtastpunkte |
| Edge Processor | Analyse geometrischer Kanten |
| Resolve Processor | Zusammenführung von Abtastwerten |
| Backend Adapter | CPU- und GPU-spezifische Umsetzung |

Anti-Aliasing verändert nicht die ursprüngliche Geometrie, sondern deren Rasterisierung.

## 3. Unterstützte Verfahren

NovaOS unterstützt folgende Verfahren:

| Verfahren | Beschreibung |
|---|---|
| None | Keine Kantenglättung |
| Coverage AA | Kantenglättung anhand der geometrischen Pixelabdeckung |
| Supersampling (SSAA) | Berechnung mit erhöhter Abtastdichte |
| Multisampling (MSAA) | Mehrere Abtastpunkte pro Pixel |
| Analytic AA | Analytische Berechnung der Kantenabdeckung |

Festlegungen:

- Coverage AA bildet das grundlegende Verfahren für CPU-Rendering.
- MSAA kann durch geeignete GPU-Backends bereitgestellt werden.
- SSAA dient als optionales Qualitätsverfahren.
- Analytic AA SOLL für geeignete Vektorgeometrien verwendet werden.
- Nicht verfügbare Verfahren MÜSSEN durch eine unterstützte Alternative ersetzt oder eindeutig gemeldet werden.

## 4. Qualitätsstufen

Die Engine stellt folgende Qualitätsstufen bereit:

| Stufe | Verhalten |
|---|---|
| Off | Keine Kantenglättung |
| Low | Ressourcenschonende Kantenglättung |
| Medium | Ausgewogenes Verhältnis zwischen Qualität und Leistung |
| High | Hochwertige Kantenglättung |
| Ultra | Maximale unterstützte Qualität |

Die Qualitätsstufen beschreiben das gewünschte Ergebnis und sind nicht fest an ein bestimmtes Verfahren gebunden.

Die konkrete Umsetzung erfolgt anhand der Backend-Fähigkeiten.

Die Anwendung DARF ein bestimmtes Verfahren ausdrücklich anfordern.

## 5. Pixelabdeckung

Die Kantenglättung basiert auf der geometrischen Abdeckung eines Pixels.

Der Abdeckungswert liegt zwischen `0.0` und `1.0`.

- `0.0`: Keine geometrische Abdeckung.
- `1.0`: Vollständige geometrische Abdeckung.
- Zwischenwerte: Teilweise Abdeckung.

Die Abdeckung wird mit der Deckkraft des gezeichneten Elements kombiniert.

Die Verarbeitung MUSS die definierten Alpha- und Blending-Regeln berücksichtigen.

Geometrische Abdeckung und Materialtransparenz bleiben logisch getrennte Eigenschaften.

## 6. Anwendungsbereiche

Anti-Aliasing unterstützt:

- Linien und Konturen.
- Kreise und Ellipsen.
- Bézier-Kurven.
- Polygone.
- Vektorpfade.
- Transformierte Geometrien.
- Geometrische Clip-Regionen.
- Vektorbasierte Masken.

Für Textdarstellung gelten zusätzlich die Festlegungen des Nova Text Rendering Systems.

Bildskalierung und Texturfilterung werden durch Nova Image Rendering definiert und sind nicht Bestandteil der geometrischen Kantenglättung.

## 7. Transformationen und Clipping

Anti-Aliasing MUSS die aktuellen geometrischen Transformationen berücksichtigen.

Dabei gilt:

- Die Abdeckung wird im Koordinatenraum des Rendering-Ziels bestimmt.
- Skalierungen und Rotationen beeinflussen die resultierende Kantenabdeckung.
- Clipping-Grenzen können geglättet werden.
- Maskenabdeckung und geometrische Abdeckung werden entsprechend den definierten Regeln kombiniert.

Mehrfach angewendete Kantenglättung DARF keine unbeabsichtigte zusätzliche Transparenz erzeugen.

## 8. CPU- und GPU-Rendering

### CPU Rendering

Der CPU Renderer MUSS grundlegende Kantenglättung durch Coverage AA unterstützen.

Optimierungen durch SIMD, Scanline- und Tile-Verarbeitung sind zulässig.

### GPU Rendering

Der GPU Renderer DARF hardwarebeschleunigte Verfahren wie MSAA verwenden.

Die Anzahl unterstützter Samples wird über die Backend-Capabilities ermittelt.

CPU- und GPU-Rendering MÜSSEN dieselbe grundlegende Abdeckungs- und Blending-Semantik verwenden.

Geringfügige Unterschiede innerhalb definierter Qualitätstoleranzen sind zulässig.

## 9. Ressourcen und Leistung

Das System unterstützt:

- Anpassbare Qualitätsstufen.
- Begrenzung zusätzlicher Sample-Buffer.
- Tile-basierte Kantenglättung.
- Wiederverwendung vorberechneter Abdeckungsdaten.
- Vermeidung unnötiger Mehrfachabtastungen.
- Hardwarebeschleunigung bei geeigneten Backends.

Die Engine SOLL für einfache Geometrien möglichst kostengünstige Verfahren verwenden.

Eine automatische Qualitätsreduzierung DARF nur erfolgen, wenn die Anwendung oder eine geltende Systemrichtlinie dies erlaubt.

Die gewählte Qualitätsstufe MUSS für die Anwendung nachvollziehbar bleiben.

## 10. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS geometrische Kantenglättung bereitstellen.
- **GRAPHICS-002:** Anti-Aliasing MUSS vollständig deaktivierbar sein.
- **GRAPHICS-003:** Coverage AA MUSS durch den CPU Renderer unterstützt werden.
- **GRAPHICS-004:** Die Qualitätsstufe MUSS konfigurierbar sein.
- **GRAPHICS-005:** Die verfügbare Hardwarebeschleunigung MUSS über Backend-Capabilities ermittelt werden.
- **GRAPHICS-006:** Die Kantenglättung MUSS geometrische Transformationen berücksichtigen.
- **GRAPHICS-007:** Clipping und Masking MÜSSEN mit Anti-Aliasing kombinierbar sein.
- **GRAPHICS-008:** Geometrische Abdeckung und Materialtransparenz MÜSSEN korrekt kombiniert werden.
- **GRAPHICS-009:** Die ursprünglichen Geometriedaten DÜRFEN durch Anti-Aliasing nicht verändert werden.
- **GRAPHICS-010:** Die grundlegende Rendering-Semantik MUSS backendunabhängig sein.
- **GRAPHICS-011:** Nicht unterstützte Verfahren MÜSSEN kontrolliert behandelt werden.
- **GRAPHICS-012:** Zusätzliche Speicheranforderungen MÜSSEN den Ressourcenrichtlinien entsprechen.
- **GRAPHICS-013:** Eine automatische Qualitätsreduzierung MUSS durch Richtlinien kontrollierbar sein.
- **GRAPHICS-014:** Die Funktionen MÜSSEN über das portable Nova Framework verfügbar sein.
- **GRAPHICS-015:** Die Verarbeitung MUSS ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 11. Fehlerbehandlung

- Ungültige Qualitätsstufen werden zurückgewiesen.
- Nicht unterstützte Sample-Anzahlen werden erkannt.
- Fehlgeschlagene Sample-Buffer-Allokationen werden kontrolliert behandelt.
- Bei fehlender Hardwareunterstützung SOLL ein geeignetes Softwareverfahren verwendet werden.
- Fehler bei der Kantenglättung DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- Ein Fallback DARF die definierten Clipping- und Blending-Regeln nicht verletzen.

## 12. Ergebnis

NovaOS erhält eine einheitliche, hardwareunabhängige Kantenglättung für zweidimensionale Grafiken.

Linien, Kurven, Formen und Vektorgrafiken können mit anpassbarer Qualität dargestellt werden.

Die Engine berücksichtigt dabei sowohl die gewünschte Darstellungsqualität als auch den verfügbaren Ressourcenrahmen.

**Grundsatz:** NovaOS stellt geometrische Kanten so präzise und glatt wie erforderlich dar, ohne unnötig Rechenleistung oder Speicher zu verbrauchen.