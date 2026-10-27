# NPSPEC-GRAPHICS-TEXT-0001 – Nova Text Rendering Engine

**Status:** Angenommen  
**Bereich:** Graphics / Text / Rendering  
**System:** NovaOS

## 1. Zweck

Nova Text Rendering Engine definiert die Darstellung von Text innerhalb der Nova Graphics Architecture.

Ziel ist eine präzise, performante und hardwareunabhängige Textdarstellung für Nova UI, NovaStudio, Systemprogramme und grafische Anwendungen.

Die Engine unterstützt Unicode, unterschiedliche Schriftarten, skalierbare Schriftgrößen und hochwertige Kantenglättung.

Sie ist Bestandteil des portablen Nova Framework.

## 2. Architektur

Das Textsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Text Renderer | Darstellung positionierter Glyphen |
| Font Manager | Verwaltung und Auswahl von Schriftarten |
| Text Layout Engine | Positionierung und Anordnung von Text |
| Text Shaping Engine | Umwandlung von Unicode-Text in Glyphenfolgen |
| Glyph Rasterizer | Rasterisierung von Schriftzeichen |
| Glyph Cache | Zwischenspeicherung gerasterter Glyphen |
| Text Drawing Context | Verwaltung der Darstellungsparameter |

Die Komponenten besitzen getrennte Schnittstellen.

Text Shaping und Text Layout liefern die positionierten Glyphen. Der Text Renderer übernimmt deren Darstellung über die Nova 2D Rendering Engine.

## 3. Textmodell

Die Engine verarbeitet Unicode-Text und unterstützt:

- UTF-8 und UTF-16.
- Unterschiedliche Schriftarten und Schriftgrößen.
- Schriftstile und Schriftstärken.
- Mehrsprachigen Text.
- Kombinierende Zeichen.
- Ligaturen.
- Unterschiedliche Schreibrichtungen.
- Emojis und farbige Glyphen.

Unicode-Zeichen, Graphemcluster und Glyphen MÜSSEN als unterschiedliche Einheiten behandelt werden.

Die Engine DARF nicht voraussetzen, dass ein Unicode-Zeichen genau einer Glyphe entspricht.

## 4. Font Management

Der Font Manager stellt Schriftressourcen bereit.

Er unterstützt:

- Laden und Freigeben von Schriftarten.
- Schriftfamilien und Schriftschnitte.
- Font Fallback.
- Variable Fonts.
- Skalierbare Schriftgrößen.
- Font-Metriken.
- Wiederverwendung geladener Schriftressourcen.

Schriftdateien werden durch eine separate Font-Parser-Komponente verarbeitet.

Fehlende Glyphen MÜSSEN über eine definierte Fallback-Strategie behandelt werden.

## 5. Text Shaping

Die Text Shaping Engine wandelt Unicode-Text in darstellbare Glyphenfolgen um.

Dabei berücksichtigt sie:

- Schriftabhängige Glyphenzuordnung.
- Ligaturen.
- Kerning.
- Kombinierende Zeichen.
- Kontextabhängige Glyphenformen.
- Schreibrichtung.
- Sprach- und Schriftregeln.

Die Ausgabe enthält Glyphenkennungen, Positionierungsinformationen und die Zuordnung zu den ursprünglichen Textbereichen.

Text Shaping MUSS unabhängig vom Rendering-Backend erfolgen.

## 6. Text Layout

Die Text Layout Engine übernimmt:

- Zeilenumbruch.
- Textausrichtung.
- Zeilenabstände.
- Absatzabstände.
- Textbegrenzungen.
- Bidirektionalen Text.
- Positionierung von Glyphen.
- Berechnung von Textabmessungen.

Die Engine MUSS Textpositionen und Abmessungen für Hit Testing, Cursorpositionierung und Textauswahl bereitstellen können.

Die eigentliche Darstellung bleibt Aufgabe des Text Renderers.

## 7. Glyph Rendering

Der Glyph Rasterizer unterstützt:

- Skalierbare Glyphenkonturen.
- Graustufen-Antialiasing.
- Monochrome Darstellung.
- Farbige Glyphen.
- Bitmap-Glyphen.
- Subpixel-Positionierung.

Subpixel-Antialiasing ist optional und DARF nur verwendet werden, wenn die Eigenschaften des Ausgabegeräts bekannt und geeignet sind.

Graustufen-Antialiasing bildet das Standardverfahren.

Die Rasterisierung MUSS ohne GPU-Beschleunigung möglich sein.

## 8. Transformationen und Farben

Textdarstellungen unterstützen:

- Translation.
- Skalierung.
- Rotation.
- Transparenz.
- Clipping.
- Farbige Füllungen.
- Farbverläufe, sofern vom Glyphenformat unterstützt.

Die Transformationen erfolgen gemäß Nova Transformations.

Farbverarbeitung und Blending richten sich nach dem Nova Graphics Color System.

## 9. Glyph Caching

Zur Leistungsoptimierung unterstützt die Engine:

- Glyphen-Caching.
- Glyph Atlases.
- Wiederverwendung vorberechneter Glyphen.
- CPU- und GPU-basierte Glyphenspeicherung.
- Bedarfsgerechte Cache-Freigabe.

Cache-Einträge berücksichtigen mindestens Schriftidentität, Glyphenkennung, Größe, Rasterisierungsmodus und relevante Darstellungsparameter.

Caches MÜSSEN bei Änderungen dieser Parameter korrekt invalidiert werden.

Die ursprünglichen Font-Daten bleiben unabhängig von gerasterten Glyphen erhalten.

## 10. CPU- und GPU-Rendering

### CPU Rendering

Der CPU Renderer stellt die grundlegende Textdarstellung einschließlich Glyphenrasterisierung, Antialiasing und Alpha-Blending bereit.

### GPU Rendering

Der GPU Renderer kann Glyph Atlases, Texturen und hardwarebeschleunigte Zeichenoperationen verwenden.

Die Auswahl erfolgt über die Nova Graphics Backend Abstraction.

Beide Rendering-Pfade verwenden dieselben Ergebnisse aus Text Shaping und Text Layout.

## 11. Sicherheit und Ressourcen

Schriftdateien und Textinhalte gelten grundsätzlich als potenziell nicht vertrauenswürdige Eingaben.

Deshalb gilt:

- Font-Daten MÜSSEN vor ihrer Verwendung validiert werden.
- Speichergrenzen MÜSSEN überprüft werden.
- Glyphen- und Cachegrößen unterliegen Ressourcenlimits.
- Fehlerhafte Fonts DÜRFEN keine unkontrollierten Speicherzugriffe verursachen.
- Textverarbeitung DARF keine direkten Kernelzugriffe ermöglichen.

Die Engine MUSS auch ohne Netzwerkverbindung und KI-Komponenten vollständig funktionieren.

## 12. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS eine einheitliche Text Rendering Engine bereitstellen.
- **GRAPHICS-002:** Die Engine MUSS Unicode-Text unterstützen.
- **GRAPHICS-003:** UTF-8 und UTF-16 MÜSSEN verarbeitet werden können.
- **GRAPHICS-004:** Text Shaping, Text Layout und Glyph Rendering MÜSSEN getrennte Komponenten sein.
- **GRAPHICS-005:** Mehrsprachiger und bidirektionaler Text MUSS unterstützt werden.
- **GRAPHICS-006:** Ligaturen, Kerning und kombinierende Zeichen MÜSSEN korrekt verarbeitet werden können.
- **GRAPHICS-007:** Font Fallback MUSS unterstützt werden.
- **GRAPHICS-008:** Skalierbare Schriftarten MÜSSEN unterstützt werden.
- **GRAPHICS-009:** Graustufen-Antialiasing MUSS verfügbar sein.
- **GRAPHICS-010:** Farbige Glyphen und Emojis MÜSSEN darstellbar sein, sofern geeignete Font-Daten vorhanden sind.
- **GRAPHICS-011:** Textabmessungen und Glyphenpositionen MÜSSEN abfragbar sein.
- **GRAPHICS-012:** Text MUSS geometrische Transformationen und Clipping unterstützen.
- **GRAPHICS-013:** Glyphen MÜSSEN zwischengespeichert werden können.
- **GRAPHICS-014:** Die grundlegende Textdarstellung MUSS ohne GPU-Beschleunigung funktionieren.
- **GRAPHICS-015:** GPU-Beschleunigung SOLL bei geeigneter Hardware verwendet werden.
- **GRAPHICS-016:** Font-Ressourcen MÜSSEN kontrolliert verwaltet werden.
- **GRAPHICS-017:** Die Textfunktionen MÜSSEN Bestandteil des portablen Nova Framework sein.
- **GRAPHICS-018:** Die Verarbeitung MUSS deterministisch und ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 13. Fehlerbehandlung

- Ungültige Unicode-Sequenzen werden gemäß einer definierten Fehlerstrategie behandelt.
- Fehlende Glyphen werden durch geeignete Fallback-Glyphen ersetzt.
- Fehlerhafte Font-Dateien werden zurückgewiesen.
- Nicht unterstützte Font-Funktionen werden eindeutig gemeldet.
- Fehlgeschlagene Glyphenrasterisierungen DÜRFEN andere Textressourcen nicht beschädigen.
- Bei GPU-Ausfall SOLL die Darstellung über den CPU Renderer fortgesetzt werden.

## 14. Ergebnis

NovaOS erhält eine portable und leistungsfähige Textdarstellung für Benutzeroberflächen und grafische Anwendungen.

Unicode-Text kann unabhängig vom verwendeten Rendering-Backend geformt, angeordnet und dargestellt werden.

Die Trennung von Font Management, Text Shaping, Text Layout und Glyph Rendering ermöglicht eine langfristig erweiterbare Architektur.

**Grundsatz:** Text wird zuerst sprachlich und typografisch korrekt verarbeitet und anschließend unabhängig von der verwendeten Hardware dargestellt.