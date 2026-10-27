# NPSPEC-GRAPHICS-TEXT-0004 – Nova Text Shaping and Layout

**Status:** Angenommen  
**Bereich:** Graphics / Text / Shaping and Layout  
**System:** NovaOS

## 1. Zweck

Nova Text Shaping and Layout definiert die sprachlich und typografisch korrekte Verarbeitung sowie die räumliche Anordnung von Unicode-Text.

Ziel ist eine einheitliche, präzise und performante Textverarbeitung für Nova UI, NovaStudio, Texteditoren und grafische Anwendungen.

Das Subsystem ist Bestandteil des portablen Nova Framework und arbeitet unabhängig vom Rendering-Backend.

## 2. Architektur

Das System besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Text Analyzer | Analyse von Unicode, Sprache und Schreibrichtung |
| Text Shaper | Umwandlung von Text in Glyphenfolgen |
| Script Processor | Verarbeitung schriftspezifischer Regeln |
| Font Resolver | Auswahl geeigneter Schriftressourcen |
| Line Breaker | Bestimmung zulässiger Zeilenumbrüche |
| Text Layout Engine | Räumliche Anordnung von Text |
| Text Metrics | Berechnung von Textabmessungen |
| Text Hit Tester | Zuordnung zwischen Textpositionen und Koordinaten |

Die Verarbeitung erfolgt in zwei getrennten Phasen:

1. **Text Shaping:** Ermittlung der darzustellenden Glyphen und ihrer relativen Positionen.
2. **Text Layout:** Anordnung der geformten Glyphen innerhalb eines definierten Textbereichs.

Die grafische Darstellung übernimmt die Nova Text Rendering Engine.

## 3. Unicode-Verarbeitung

Das System unterstützt:

- UTF-8 und UTF-16.
- Unicode-Codepoints.
- Graphemcluster.
- Kombinierende Zeichen.
- Variation Selectors.
- Unicode-Normalisierung bei Bedarf.
- Bidirektionalen Text.
- Unterschiedliche Schriftsysteme.

Codepoints, Graphemcluster und Glyphen werden als unterschiedliche Einheiten behandelt.

Die Zuordnung zwischen ursprünglichem Text und erzeugten Glyphen MUSS erhalten bleiben.

Die Engine DARF den ursprünglichen Text nicht implizit verändern.

## 4. Textanalyse

Vor dem Shaping wird der Text in geeignete Abschnitte unterteilt.

Dabei werden berücksichtigt:

- Schreibrichtung.
- Schriftsystem.
- Sprache.
- Schriftart.
- Typografische Eigenschaften.
- Unicode-Grenzen.

Abschnitte mit unterschiedlichen Eigenschaften werden getrennt verarbeitet.

Die Textanalyse MUSS auch Texte unterstützen, die mehrere Schriftsysteme und Schreibrichtungen enthalten.

## 5. Text Shaping

Der Text Shaper wandelt Unicode-Text in positionierte Glyphenfolgen um.

Unterstützt werden:

- Kontextabhängige Glyphenformen.
- Ligaturen.
- Kerning.
- Glyphensubstitution.
- Glyphenpositionierung.
- Kombinierende Zeichen.
- Schriftabhängige typografische Regeln.
- OpenType-Layoutfunktionen.

Das Shaping-Ergebnis enthält mindestens:

- Glyphenkennungen.
- Zugehörige FontFaces.
- Glyphenpositionen und Advances.
- Glyphen-Offsets.
- Zuordnung zu Textclustern.
- Schreibrichtung.

Shaping MUSS unabhängig von der späteren Rasterisierung erfolgen.

## 6. Schreibrichtungen

Das System unterstützt:

- Links-nach-rechts (LTR).
- Rechts-nach-links (RTL).
- Gemischten bidirektionalen Text.
- Vertikale Schreibrichtungen, sofern erforderlich.

Die Verarbeitung bidirektionaler Texte erfolgt entsprechend dem Unicode Bidirectional Algorithm.

Logische Textreihenfolge und visuelle Darstellungsreihenfolge bleiben unterscheidbar.

Cursorbewegungen und Textauswahl MÜSSEN beide Ordnungen berücksichtigen können.

## 7. Font Fallback

Die Schriftzuordnung erfolgt über Nova Font Management.

Kann eine Schrift benötigte Glyphen nicht bereitstellen, wird eine geeignete Ersatzschrift ausgewählt.

Dabei gilt:

- Zusammengehörige Graphemcluster SOLLEN nicht auf mehrere Schriftarten aufgeteilt werden.
- Schriftwechsel MÜSSEN bei Bedarf erneutes Shaping auslösen.
- Die Fallback-Auswahl MUSS reproduzierbar sein.
- Fehlende Glyphen werden kontrolliert behandelt.

Die Text Layout Engine verwendet die Metriken der tatsächlich ausgewählten Schriftressourcen.

## 8. Zeilenumbruch

Die Line Breaker-Komponente unterstützt:

- Automatischen Zeilenumbruch.
- Explizite Zeilenumbrüche.
- Wortgrenzen.
- Unicode-Zeilenumbruchregeln.
- Unterschiedliche Umbruchstrategien.
- Begrenzte Textbereiche.
- Optionale Silbentrennung.

Die Zeilenumbruchregeln orientieren sich am Unicode Line Breaking Algorithm.

Sprachabhängige Silbentrennung kann über optionale Wörterbuch- oder Regelmodule ergänzt werden.

Eine Wissensdatenbank oder Netzwerkverbindung ist dafür nicht erforderlich.

## 9. Text Layout

Die Layout Engine unterstützt:

- Einzeiligen und mehrzeiligen Text.
- Links-, Rechts- und Zentrierausrichtung.
- Blocksatz.
- Zeilenhöhe und Zeilenabstand.
- Absatzabstände.
- Einzüge.
- Tabulatoren.
- Textbegrenzungen.
- Vertikale Ausrichtung.

Textbereiche werden in logischen Koordinaten beschrieben.

Die Layoutberechnung MUSS unabhängig von der tatsächlichen Displayauflösung erfolgen.

## 10. Textmetriken und Hit Testing

Die Engine stellt folgende Informationen bereit:

- Textbreite und Texthöhe.
- Zeilenpositionen.
- Glyphenpositionen.
- Textbegrenzungsrechtecke.
- Cursorpositionen.
- Auswahlbereiche.
- Zuordnung von Koordinaten zu Textpositionen.

Hit Testing MUSS Graphemcluster, Ligaturen und bidirektionale Textabschnitte berücksichtigen.

Cursorpositionen DÜRFEN nicht unbeabsichtigt innerhalb unteilbarer Graphemcluster liegen.

## 11. Caching und inkrementelles Layout

Zur Leistungsoptimierung unterstützt das System:

- Caching geformter Textabschnitte.
- Wiederverwendung unveränderter Layoutbereiche.
- Inkrementelle Neuberechnung.
- Parallele Verarbeitung unabhängiger Textabschnitte.
- Bedarfsgerechte Verarbeitung großer Dokumente.

Änderungen an Text, Schriftparametern oder Layoutbedingungen MÜSSEN betroffene Cache-Einträge invalidieren.

Unveränderte Textbereiche SOLLEN nicht unnötig erneut verarbeitet werden.

Die Verarbeitung MUSS bei gleichen Eingaben und Konfigurationen reproduzierbare Ergebnisse liefern.

## 12. Integration

- **Nova Font Management:** Schriftauflösung, Fallback und Font-Metriken.
- **Nova Font Rasterization:** Rasterisierung der positionierten Glyphen.
- **Nova Text Rendering Engine:** Grafische Darstellung des Layout-Ergebnisses.
- **Nova UI:** Textdarstellung, Eingabefelder und Textauswahl.
- **NovaStudio:** Quellcodeeditor und Textbearbeitung.
- **Nova Framework:** Portable Unicode-, Shaping- und Layoutschnittstellen.

Das Subsystem besitzt keine direkte Abhängigkeit von GPU, Window Manager oder Compositor.

## 13. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS Unicode-basiertes Text Shaping und Layout bereitstellen.
- **GRAPHICS-002:** UTF-8 und UTF-16 MÜSSEN unterstützt werden.
- **GRAPHICS-003:** Codepoints, Graphemcluster und Glyphen MÜSSEN unterschieden werden.
- **GRAPHICS-004:** Text Shaping und Text Layout MÜSSEN getrennte Komponenten sein.
- **GRAPHICS-005:** Kontextabhängige Glyphenformen, Ligaturen und Kerning MÜSSEN unterstützt werden.
- **GRAPHICS-006:** OpenType-Glyphensubstitution und -Positionierung MÜSSEN unterstützt werden.
- **GRAPHICS-007:** Bidirektionaler Text MUSS korrekt verarbeitet werden.
- **GRAPHICS-008:** Die Zuordnung zwischen Textpositionen und Glyphen MUSS erhalten bleiben.
- **GRAPHICS-009:** Font Fallback MUSS unterstützt werden.
- **GRAPHICS-010:** Automatischer und expliziter Zeilenumbruch MÜSSEN verfügbar sein.
- **GRAPHICS-011:** Mehrzeiliges Layout und Textausrichtung MÜSSEN unterstützt werden.
- **GRAPHICS-012:** Textmetriken und Cursorpositionen MÜSSEN abfragbar sein.
- **GRAPHICS-013:** Hit Testing MUSS Graphemcluster und bidirektionalen Text berücksichtigen.
- **GRAPHICS-014:** Inkrementelle Layoutberechnung SOLL unterstützt werden.
- **GRAPHICS-015:** Shaping und Layout MÜSSEN ohne GPU-Beschleunigung funktionieren.
- **GRAPHICS-016:** Die Schnittstellen MÜSSEN Bestandteil des portablen Nova Framework sein.
- **GRAPHICS-017:** Die Verarbeitung MUSS deterministisch und ohne Netzwerk- oder KI-Abhängigkeit funktionieren.

## 14. Fehlerbehandlung

- Ungültige Unicode-Sequenzen werden gemäß einer definierten Fehlerstrategie behandelt.
- Nicht unterstützte Schriftfunktionen werden kontrolliert behandelt.
- Fehlende Glyphen lösen die definierte Font-Fallback-Strategie aus.
- Ungültige Layoutabmessungen werden zurückgewiesen.
- Numerische Überläufe und ungültige Speicherbereiche MÜSSEN erkannt werden.
- Fehlerhafte Textabschnitte DÜRFEN unabhängige Layoutbereiche nicht beschädigen.

## 15. Ergebnis

NovaOS erhält eine einheitliche und portable Textverarbeitung für unterschiedliche Sprachen, Schriftsysteme und Layoutanforderungen.

Texte können typografisch korrekt geformt, umgebrochen, positioniert und für interaktive Bearbeitung ausgewertet werden.

Die Trennung zwischen Textanalyse, Shaping, Layout und Rendering ermöglicht eine leistungsfähige und langfristig erweiterbare Textarchitektur.

**Grundsatz:** NovaOS verarbeitet Text entsprechend seiner sprachlichen und typografischen Struktur. Erst danach wird das Ergebnis grafisch dargestellt.