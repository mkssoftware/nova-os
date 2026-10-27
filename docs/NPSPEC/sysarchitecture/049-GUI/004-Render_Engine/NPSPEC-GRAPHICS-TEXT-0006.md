# NPSPEC-GRAPHICS-TEXT-0006 – Nova Font Fallback

**Status:** Angenommen  
**Bereich:** Graphics / Text / Font Fallback  
**System:** NovaOS

## 1. Zweck

Nova Font Fallback definiert die automatische Auswahl geeigneter Ersatzschriftarten, wenn eine angeforderte Schrift benötigte Zeichen oder Glyphen nicht bereitstellen kann.

Ziel ist eine vollständige, sprachübergreifende und typografisch konsistente Textdarstellung, ohne dass Anwendungen Ersatzschriften selbst verwalten müssen.

Das Subsystem ist Bestandteil von Nova Font Management und wird über das portable Nova Framework bereitgestellt.

## 2. Architektur

Das Font-Fallback-System besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Fallback Resolver | Auswahl geeigneter Ersatzschriften |
| Font Coverage Index | Ermittlung unterstützter Unicode-Zeichen |
| Script Matcher | Zuordnung geeigneter Schriften zu Schriftsystemen |
| Style Matcher | Vergleich typografischer Eigenschaften |
| Fallback Registry | Verwaltung definierter Ersatzschriftketten |
| Fallback Cache | Zwischenspeicherung aufgelöster Schriftzuordnungen |

Die Font Registry und der Font Resolver aus Nova Font Management bilden die Grundlage.

Die tatsächliche Glyphenbildung erfolgt weiterhin durch Nova Text Shaping and Layout.

## 3. Fallback-Modell

Die Schriftauswahl erfolgt über eine priorisierte Fallback-Kette.

Standardreihenfolge:

1. Angeforderte Schriftfamilie.
2. Anwendungsspezifische Ersatzschriften.
3. Benutzerdefinierte Ersatzschriften.
4. Systemweite sprach- und schriftspezifische Ersatzschriften.
5. Allgemeine System-Fallback-Schrift.
6. Definierte Missing-Glyph-Darstellung.

Die Reihenfolge MUSS konfigurierbar und deterministisch sein.

Anwendungen DÜRFEN eigene Fallback-Ketten definieren, ohne die systemweite Konfiguration zu verändern.

## 4. Schriftauswahl

Der Fallback Resolver berücksichtigt:

- Unicode-Abdeckung.
- Schriftsystem und Sprache.
- Schriftfamilie.
- Schriftstärke.
- Schriftstil und Schriftbreite.
- Verfügbare Variationsachsen.
- Unterstützung benötigter Shaping-Funktionen.
- Emoji- und Farbglyphenunterstützung.

Eine passende Unicode-Zeichenabdeckung allein garantiert keine korrekte Darstellung.

Die endgültige Eignung einer Schrift MUSS bei Bedarf durch Text Shaping überprüft werden.

## 5. Graphemcluster und Shaping

Fallback erfolgt auf Grundlage zusammengehöriger Texteinheiten.

Festlegungen:

- Graphemcluster SOLLEN möglichst vollständig durch dieselbe Schrift dargestellt werden.
- Kombinierende Zeichen MÜSSEN berücksichtigt werden.
- Emoji-Sequenzen und Variation Selectors MÜSSEN gemeinsam ausgewertet werden.
- Kontextabhängige Schriften MÜSSEN nach einem Schriftwechsel erneut geformt werden.
- Shaping-relevante Zusammenhänge über Graphemclustergrenzen hinweg MÜSSEN erhalten bleiben.

Ein Schriftwechsel DARF nicht zu einer unbeabsichtigten Trennung zusammengehöriger Zeichen führen.

## 6. Internationale Schriftunterstützung

Das System unterstützt Fallback für unterschiedliche Schriftsysteme, insbesondere:

- Lateinisch, Griechisch und Kyrillisch.
- Arabisch und Hebräisch.
- Chinesisch, Japanisch und Koreanisch.
- Indische Schriftsysteme.
- Weitere Unicode-Schriften.
- Mathematische und technische Symbole.
- Emojis.

Sprachspezifische Glyphenvarianten MÜSSEN bei der Auswahl berücksichtigt werden können.

Fehlende Schriftressourcen werden durch die nächstgeeignete verfügbare Schrift ersetzt.

## 7. Emoji-Fallback

Für Emoji-Darstellung gelten zusätzliche Regeln:

- Emoji- und Textpräsentation werden unterschieden.
- Variation Selectors werden berücksichtigt.
- Zusammengesetzte Emoji-Sequenzen werden als zusammengehörige Einheiten behandelt.
- Farbglyphen werden bevorzugt, wenn eine entsprechende Darstellung angefordert wird.
- Monochrome Ersatzdarstellungen sind zulässig.

Ist keine geeignete Emoji-Schrift verfügbar, erfolgt eine definierte Ersatzdarstellung.

## 8. Typografische Konsistenz

Ersatzschriften können unterschiedliche Metriken besitzen.

Das System berücksichtigt:

- Schriftgröße.
- Ascender und Descender.
- Glyphenbreiten.
- Schriftstärke.
- Baseline.
- Zeilenhöhe.

Fallback-Schriften SOLLEN dem Erscheinungsbild der ursprünglich angeforderten Schrift möglichst nahekommen.

Die Text Layout Engine MUSS die tatsächlichen Metriken der ausgewählten Ersatzschrift verwenden.

Eine künstliche Anpassung von Schriftmetriken DARF die Lesbarkeit nicht beeinträchtigen.

## 9. Caching und Ressourcen

Der Fallback Cache speichert aufgelöste Schriftzuordnungen.

Cache-Schlüssel berücksichtigen mindestens:

- Angeforderte Schrift.
- Schriftsystem und Sprache.
- Benötigte Zeichen beziehungsweise Textsequenz.
- Typografische Eigenschaften.
- Fallback-Konfiguration.
- Version der Font Registry.

Änderungen an installierten Schriften oder Fallback-Regeln MÜSSEN betroffene Cache-Einträge invalidieren.

Bereits geladene FontFaces werden über Nova Font Management wiederverwendet.

## 10. Fehlerbehandlung

- Nicht verfügbare Schriftfamilien lösen die definierte Fallback-Kette aus.
- Fehlende Glyphen führen zur Prüfung weiterer geeigneter Schriften.
- Fehlerhafte Schriftdateien werden übersprungen und gemeldet.
- Zyklische Fallback-Referenzen MÜSSEN erkannt und verhindert werden.
- Die Suche MUSS durch definierte Ressourcen- und Iterationsgrenzen beschränkt sein.
- Ist keine geeignete Schrift verfügbar, wird eine definierte Missing-Glyph-Darstellung verwendet.

Fallback DARF keine automatische Installation oder einen Netzwerkdownload von Schriftarten auslösen.

## 11. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS einen zentralen Font-Fallback-Mechanismus bereitstellen.
- **GRAPHICS-002:** Fallback-Ketten MÜSSEN priorisiert und deterministisch ausgewertet werden.
- **GRAPHICS-003:** Anwendungen MÜSSEN eigene Fallback-Regeln definieren können.
- **GRAPHICS-004:** Unicode-Abdeckung, Sprache und Schriftsystem MÜSSEN bei der Auswahl berücksichtigt werden.
- **GRAPHICS-005:** Graphemcluster und zusammengehörige Shaping-Einheiten MÜSSEN korrekt behandelt werden.
- **GRAPHICS-006:** Kontextabhängige Glyphenformen MÜSSEN nach erforderlichen Schriftwechseln erneut berechnet werden.
- **GRAPHICS-007:** Emoji-Sequenzen und Variation Selectors MÜSSEN berücksichtigt werden.
- **GRAPHICS-008:** Typografische Eigenschaften SOLLEN bei der Ersatzschriftauswahl erhalten bleiben.
- **GRAPHICS-009:** Die tatsächlichen Font-Metriken MÜSSEN an die Text Layout Engine übergeben werden.
- **GRAPHICS-010:** Fallback-Ergebnisse MÜSSEN zwischengespeichert werden können.
- **GRAPHICS-011:** Änderungen an Schriftressourcen und Fallback-Regeln MÜSSEN Cache-Invalidierungen auslösen.
- **GRAPHICS-012:** Zyklische Fallback-Ketten und unbegrenzte Suchvorgänge MÜSSEN verhindert werden.
- **GRAPHICS-013:** Eine definierte Missing-Glyph-Darstellung MUSS verfügbar sein.
- **GRAPHICS-014:** Font Fallback MUSS unabhängig vom Rendering-Backend funktionieren.
- **GRAPHICS-015:** Die Funktionen MÜSSEN über das portable Nova Framework verfügbar sein.
- **GRAPHICS-016:** Font Fallback MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 12. Ergebnis

NovaOS erhält einen zentralen und sprachübergreifenden Font-Fallback-Mechanismus.

Fehlende Schriftzeichen werden automatisch durch geeignete verfügbare Schriftarten ersetzt, während sprachliche Zusammenhänge und typografische Eigenschaften möglichst erhalten bleiben.

Die Integration in Nova Font Management und Nova Text Shaping and Layout ermöglicht eine konsistente Textdarstellung in sämtlichen Anwendungen.

**Grundsatz:** Der Benutzer soll Texte unabhängig von den ursprünglich gewählten Schriftarten lesen können, ohne sich um fehlende Glyphen oder Ersatzschriften kümmern zu müssen.