# NPSPEC-GRAPHICS-TEXT-0003 – Nova Font Rasterization

**Status:** Angenommen  
**Bereich:** Graphics / Text / Font Rasterization  
**System:** NovaOS

## 1. Zweck

Nova Font Rasterization definiert die Umwandlung von Schriftglyphen in darstellbare Pixel- und Abdeckungsdaten.

Ziel ist eine präzise, performante und hardwareunabhängige Schriftendarstellung für Nova UI, NovaStudio und grafische Anwendungen.

Die Rasterisierung ist Bestandteil der Nova Text Rendering Engine und wird über das portable Nova Framework bereitgestellt.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Glyph Rasterizer | Umwandlung von Glyphen in Rasterdaten |
| Outline Processor | Verarbeitung von Glyphenkonturen |
| Hinting Engine | Anpassung an das Pixelraster |
| Coverage Processor | Berechnung der Pixelabdeckung |
| Glyph Bitmap Generator | Erzeugung rasterisierter Glyphen |
| Glyph Cache | Wiederverwendung rasterisierter Glyphen |
| Backend Adapter | CPU- und GPU-spezifische Umsetzung |

Die Rasterisierung verwendet Schriftressourcen aus Nova Font Management.

Text Shaping und Text Layout erfolgen vor der Rasterisierung und bleiben eigenständige Komponenten.

## 3. Glyphenmodell

Die Rasterisierung verarbeitet positionierte Glyphen anhand folgender Informationen:

- FontFace und FontInstance.
- Glyphenkennung.
- Schriftgröße.
- Glyphenkonturen oder Bitmap-Daten.
- Transformationsparameter.
- Rasterisierungsmodus.
- Hinting-Einstellungen.
- Subpixel-Position.

Die ursprünglichen Glyphendaten bleiben unverändert.

Eine Glyphe kann für unterschiedliche Größen und Darstellungsmodi mehrfach rasterisiert werden.

## 4. Rasterisierungsverfahren

NovaOS unterstützt folgende Verfahren:

| Verfahren | Beschreibung |
|---|---|
| Monochrome | Binäre Pixelabdeckung |
| Grayscale | Kantenglättung über Graustufenabdeckung |
| Subpixel Positioning | Positionierung mit Bruchteilen eines Pixels |
| Color Glyph | Darstellung mehrfarbiger Glyphen |
| Bitmap Glyph | Verwendung vorhandener Glyphen-Bitmaps |

Graustufen-Rasterisierung ist das Standardverfahren.

Subpixel-Antialiasing auf Basis der physischen Farbkanalanordnung ist optional und wird nur bei geeigneten Ausgabebedingungen verwendet.

Die Rasterisierung MUSS vollständig über die CPU möglich sein.

## 5. Outline-Verarbeitung

Der Outline Processor verarbeitet skalierbare Glyphenkonturen.

Unterstützt werden:

- Gerade Liniensegmente.
- Quadratische Bézier-Kurven.
- Kubische Bézier-Kurven.
- Geschlossene Konturen.
- Zusammengesetzte Glyphen.
- Mehrere Konturen innerhalb einer Glyphe.

Die Verarbeitung berücksichtigt die Füllregeln des jeweiligen Schriftformats.

Kurven werden entsprechend der gewünschten Darstellungsqualität rasterisiert.

## 6. Hinting

Hinting verbessert die Lesbarkeit insbesondere bei kleinen Schriftgrößen.

Unterstützt werden:

- Deaktiviertes Hinting.
- Leichtes Hinting.
- Vollständiges Hinting, sofern vom Schriftformat unterstützt.

Hinting DARF die ursprünglichen Schriftkonturen nicht dauerhaft verändern.

Nicht unterstützte oder fehlerhafte Hinting-Anweisungen MÜSSEN kontrolliert behandelt werden.

Die Anwendung MUSS Hinting deaktivieren können.

## 7. Pixelabdeckung und Antialiasing

Die Rasterisierung berechnet die Abdeckung eines Pixels durch die jeweilige Glyphe.

Abdeckungswerte liegen zwischen `0.0` und `1.0`.

Die Glyphenabdeckung wird bei der späteren Darstellung mit Textfarbe, Transparenz und Blending kombiniert.

Festlegungen:

- Die Rasterisierung erzeugt Abdeckungsdaten.
- Die Text Rendering Engine übernimmt deren Farbgebung und Komposition.
- Antialiasing berücksichtigt die tatsächliche Glyphengeometrie.
- Mehrfach angewendete Kantenglättung DARF keine unbeabsichtigte zusätzliche Transparenz erzeugen.

Die Verarbeitung richtet sich nach Nova Anti-Aliasing.

## 8. Transformationen

Glyphen können unter Berücksichtigung geometrischer Transformationen rasterisiert werden.

Unterstützt werden:

- Skalierung.
- Rotation.
- Translation.
- Affine Transformationen.

Die Rasterisierung MUSS den Transformationszustand berücksichtigen, sofern dieser die Glyphenform oder Pixelabdeckung beeinflusst.

Reine Positionsänderungen SOLLEN ohne erneute Rasterisierung verarbeitet werden, sofern das gewählte Verfahren dies erlaubt.

## 9. Farbige Glyphen

Die Engine unterstützt farbige Schriftglyphen, beispielsweise Emojis.

Mögliche Repräsentationen:

- Mehrschichtige Vektorglyphen.
- Eingebettete Bitmap-Glyphen.
- Farbige Glyphenkonturen.

Die Unterstützung konkreter Font-Tabellen erfolgt über Nova Font Management und die zugehörigen Font-Parser.

Farbige Glyphen werden gemäß Nova Graphics Color System verarbeitet.

Nicht unterstützte Farbformate MÜSSEN kontrolliert behandelt werden.

## 10. Glyph Cache

Rasterisierte Glyphen werden zur Wiederverwendung zwischengespeichert.

Ein Cache-Eintrag berücksichtigt mindestens:

- FontFace und FontInstance.
- Glyphenkennung.
- Schriftgröße.
- Rasterisierungsmodus.
- Hinting-Modus.
- Relevante Transformationsparameter.
- Subpixel-Position, sofern erforderlich.

Unterstützt werden:

- CPU-Glyphen-Caches.
- GPU-Glyph-Atlases.
- Bedarfsgerechte Rasterisierung.
- Wiederverwendung vorhandener Glyphen.
- Kontrollierte Cache-Freigabe.

Cache-Einträge MÜSSEN bei Änderungen relevanter Parameter invalidiert werden.

## 11. Leistungsoptimierung

Die Engine unterstützt:

- SIMD-optimierte Rasterisierung.
- Parallele Verarbeitung unabhängiger Glyphen.
- Wiederverwendung vorberechneter Konturen.
- Glyph-Atlas-Batching.
- Vermeidung unnötiger Neurasterisierung.
- Bedarfsgerechte Speicherallokation.

Die Rasterisierung SOLL nur für tatsächlich benötigte Glyphen erfolgen.

GPU-Beschleunigung DARF verwendet werden, sofern das Backend geeignete Funktionen bereitstellt.

## 12. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS eine eigenständige Glyphenrasterisierung bereitstellen.
- **GRAPHICS-002:** Skalierbare Glyphenkonturen MÜSSEN unterstützt werden.
- **GRAPHICS-003:** Monochrome und Graustufen-Rasterisierung MÜSSEN verfügbar sein.
- **GRAPHICS-004:** Graustufen-Antialiasing MUSS als Standardverfahren verfügbar sein.
- **GRAPHICS-005:** Hinting MUSS konfigurierbar und deaktivierbar sein.
- **GRAPHICS-006:** Subpixel-Positionierung MUSS unterstützt werden.
- **GRAPHICS-007:** Farbige Glyphen MÜSSEN bei geeigneten Font-Daten darstellbar sein.
- **GRAPHICS-008:** Glyphen MÜSSEN unter Berücksichtigung geometrischer Transformationen rasterisiert werden können.
- **GRAPHICS-009:** Die ursprünglichen Glyphendaten DÜRFEN nicht verändert werden.
- **GRAPHICS-010:** Rasterisierte Glyphen MÜSSEN zwischengespeichert werden können.
- **GRAPHICS-011:** Cache-Schlüssel MÜSSEN sämtliche rasterisierungsrelevanten Parameter berücksichtigen.
- **GRAPHICS-012:** Die grundlegende Rasterisierung MUSS ohne GPU-Beschleunigung funktionieren.
- **GRAPHICS-013:** GPU-Beschleunigung SOLL bei geeigneter Hardware verwendet werden.
- **GRAPHICS-014:** Speicherzugriffe und Glyphenabmessungen MÜSSEN validiert werden.
- **GRAPHICS-015:** Rasterisierung und Text Layout MÜSSEN getrennte Komponenten bleiben.
- **GRAPHICS-016:** Die Rasterisierungsfunktionen MÜSSEN über das portable Nova Framework verfügbar sein.
- **GRAPHICS-017:** Die Verarbeitung MUSS deterministisch und ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

- Ungültige Glyphenkennungen werden zurückgewiesen.
- Fehlerhafte Glyphenkonturen werden kontrolliert behandelt.
- Nicht unterstützte Hinting-Anweisungen DÜRFEN keine unkontrollierte Ausführung verursachen.
- Ungültige Glyphenabmessungen und Speicherbereiche werden erkannt.
- Fehlgeschlagene Rasterisierungen DÜRFEN bestehende Cache-Einträge nicht beschädigen.
- Bei GPU-Ausfall SOLL die Rasterisierung über die CPU fortgesetzt werden.

## 14. Ergebnis

NovaOS erhält eine präzise und hardwareunabhängige Rasterisierung für skalierbare und bitmapbasierte Schriftglyphen.

Schriftzeichen können in unterschiedlichen Größen, Qualitätsstufen und Darstellungsmodi effizient erzeugt und wiederverwendet werden.

Die Trennung zwischen Font Management, Text Shaping, Text Layout und Glyph Rasterization ermöglicht eine flexible und langfristig erweiterbare Textarchitektur.

**Grundsatz:** Die Rasterisierung wandelt typografisch vorbereitete Glyphen in präzise Pixelabdeckung um, ohne deren ursprüngliche Schriftinformationen zu verändern.