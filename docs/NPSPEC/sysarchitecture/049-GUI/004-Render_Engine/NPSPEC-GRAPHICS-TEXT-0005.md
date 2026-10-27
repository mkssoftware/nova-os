# NPSPEC-GRAPHICS-TEXT-0005 – Nova Unicode and International Text

**Status:** Angenommen  
**Bereich:** Graphics / Text / Unicode and Internationalization  
**System:** NovaOS

## 1. Zweck

Nova Unicode and International Text definiert die einheitliche Verarbeitung internationaler Texte innerhalb von NovaOS und des portablen Nova Framework.

Ziel ist die korrekte Verarbeitung, Speicherung und Darstellung von Unicode-Text unabhängig von Sprache, Schriftsystem und Schreibrichtung.

Das Subsystem stellt die gemeinsame Unicode-Grundlage für Nova Text Shaping and Layout, Nova UI, NovaStudio und weitere Systemkomponenten bereit.

## 2. Architektur

Das Subsystem besteht aus folgenden Komponenten:

| Komponente | Aufgabe |
|---|---|
| Unicode Core | Unicode-Datenmodell und grundlegende Operationen |
| Encoding Engine | Kodierung und Dekodierung |
| Text Normalizer | Unicode-Normalisierung |
| Text Segmenter | Erkennung von Textgrenzen |
| Script Analyzer | Erkennung von Schriftsystemen |
| Direction Analyzer | Analyse bidirektionaler Texte |
| Locale Manager | Sprach- und Gebietsschemainformationen |
| Unicode Data Provider | Bereitstellung versionierter Unicode-Daten |

Die Komponenten arbeiten unabhängig von Font Management, Text Layout und Rendering.

Unicode-Funktionen werden über das Nova Framework bereitgestellt und können auch außerhalb grafischer Anwendungen verwendet werden.

## 3. Unicode-Datenmodell

NovaOS unterscheidet zwischen:

- Code Units.
- Unicode-Codepoints.
- Graphemclustern.
- Wörtern.
- Sätzen.
- Textabschnitten.
- Glyphen.

Die interne Standardkodierung für Text ist UTF-8.

UTF-16 wird für Interoperabilität und entsprechende Schnittstellen unterstützt.

UTF-32 kann für codepointorientierte Verarbeitung verwendet werden.

Textpositionen MÜSSEN ihre Bezugseinheit eindeutig definieren.

Ein Unicode-Codepoint DARF nicht grundsätzlich als vollständiges sichtbares Zeichen betrachtet werden.

## 4. Kodierung und Dekodierung

Die Encoding Engine unterstützt:

- UTF-8.
- UTF-16 Little Endian.
- UTF-16 Big Endian.
- UTF-32 Little Endian.
- UTF-32 Big Endian.

Die Verarbeitung umfasst:

- Validierung von Eingabesequenzen.
- Konvertierung zwischen Kodierungen.
- Erkennung ungültiger Sequenzen.
- Behandlung von Byte Order Marks.
- Streaming-basierte Dekodierung.
- Kontrollierte Fehlerbehandlung.

Ungültige Eingaben können abhängig von der gewählten Strategie zurückgewiesen oder durch `U+FFFD` ersetzt werden.

Stillschweigender Datenverlust MUSS vermieden werden.

## 5. Unicode-Normalisierung

Das System unterstützt die Unicode-Normalisierungsformen:

- NFC.
- NFD.
- NFKC.
- NFKD.

Normalisierung erfolgt ausschließlich auf ausdrückliche Anforderung oder gemäß einem dokumentierten Schnittstellenvertrag.

Die ursprüngliche Zeichenfolge DARF nicht unbeabsichtigt verändert werden.

Kompatibilitätsnormalisierung MUSS von kanonischer Normalisierung unterscheidbar bleiben.

Normalisierung ist keine allgemeine Sicherheitsprüfung und ersetzt keine anwendungsspezifische Validierung.

## 6. Textsegmentierung

Der Text Segmenter bestimmt sprachlich relevante Textgrenzen.

Unterstützt werden:

- Graphemclustergrenzen.
- Wortgrenzen.
- Satzgrenzen.
- Zeilenumbruchmöglichkeiten.

Die Segmentierung orientiert sich an den Unicode-Standards UAX #29 und UAX #14.

Sprachabhängige Erweiterungen können über optionale Regelmodule bereitgestellt werden.

Graphemcluster bilden die Grundlage für benutzerorientierte Cursorbewegungen und Löschoperationen.

## 7. Internationale Schriftsysteme

NovaOS unterstützt Unicode-basierte Schriftsysteme einschließlich:

- Lateinischer, griechischer und kyrillischer Schrift.
- Arabischer und hebräischer Schrift.
- Indischer Schriftsysteme.
- Chinesischer, japanischer und koreanischer Schrift.
- Kombinierender Zeichen.
- Emoji-Sequenzen.
- Historischer und weiterer Unicode-Schriften.

Die Unicode-Verarbeitung darf keine feste Zeichenbreite oder eine bestimmte Leserichtung voraussetzen.

Die tatsächliche Glyphendarstellung erfolgt über Nova Text Shaping and Layout sowie Nova Font Management.

## 8. Bidirektionaler Text

Die Direction Analyzer-Komponente implementiert den Unicode Bidirectional Algorithm gemäß UAX #9.

Unterstützt werden:

- Links-nach-rechts-Text.
- Rechts-nach-links-Text.
- Gemischte Schreibrichtungen.
- Bidirektionale Isolate.
- Explizite Richtungssteuerzeichen.

Logische Textreihenfolge und visuelle Darstellungsreihenfolge MÜSSEN getrennt bleiben.

Die Unicode-Komponente liefert die erforderlichen Richtungsinformationen.

Die endgültige Glyphenpositionierung erfolgt durch Nova Text Shaping and Layout.

## 9. Sprache und Locale

Der Locale Manager stellt sprach- und regionsabhängige Informationen bereit.

Dazu gehören:

- Sprachkennungen.
- Regionskennungen.
- Schreibrichtung.
- Dezimal- und Gruppierungszeichen.
- Datums- und Zeitformate.
- Sprachabhängige Groß- und Kleinschreibung.
- Sortier- und Vergleichsregeln.

Locale-Einstellungen MÜSSEN unabhängig von der verwendeten Textkodierung sein.

Die Sprache einer Anwendung DARF unabhängig von der Systemsprache gewählt werden.

Locale-Daten können durch versionierte Datenpakete erweitert werden.

## 10. Unicode-Daten und Versionierung

Unicode-Eigenschaften werden über einen zentralen Unicode Data Provider bereitgestellt.

Dieser verwaltet:

- Unicode-Version.
- Zeichenkategorien.
- Schriftsystemzuordnungen.
- Normalisierungsdaten.
- Segmentierungsregeln.
- Bidirektionale Eigenschaften.
- Zeichenumwandlungen.

Die verwendete Unicode-Version MUSS abfragbar sein.

Zusammengehörige Unicode-Datentabellen MÜSSEN versionskonsistent sein.

Unicode-Daten werden lokal bereitgestellt. Netzwerkzugriffe sind nicht erforderlich.

## 11. Sicherheit und Determinismus

Unicode-Eingaben gelten als potenziell nicht vertrauenswürdig.

Das System MUSS:

- Ungültige Kodierungen erkennen.
- Speicher- und Größenüberläufe verhindern.
- Steuerzeichen korrekt behandeln.
- Bidirektionale Steuerzeichen erhalten und erkennbar machen können.
- Unicode-Verwechslungen nicht als Identitätsgleichheit interpretieren.
- Reproduzierbare Ergebnisse bei identischer Unicode-Version und Konfiguration liefern.

Sicherheitskritische Vergleiche, beispielsweise von Ressourcenkennungen, DÜRFEN nicht von benutzerabhängigen Locale-Einstellungen abhängen.

Die Erkennung visuell verwechselbarer Zeichen kann über optionale Sicherheitsfunktionen ergänzt werden.

## 12. Integration

- **Nova Text Shaping and Layout:** Unicode-Analyse, Segmentierung und Schreibrichtung.
- **Nova Font Management:** Unicode-Abdeckung und Schriftauswahl.
- **Nova Text Rendering Engine:** Darstellung internationaler Texte.
- **Nova UI:** Mehrsprachige Oberflächen und Texteingabe.
- **NovaStudio:** Unicode-fähige Quellcodebearbeitung.
- **Nova Framework:** Portable Unicode- und Locale-Funktionen.
- **Nova.Trust:** Sichere Behandlung nicht vertrauenswürdiger Texte.

Das Subsystem besitzt keine Abhängigkeit von GPU, Compositor oder Window Manager.

## 13. Normative Anforderungen

- **GRAPHICS-001:** NovaOS MUSS Unicode als Grundlage der Textverarbeitung verwenden.
- **GRAPHICS-002:** UTF-8 MUSS die interne Standardkodierung sein.
- **GRAPHICS-003:** UTF-16 MUSS unterstützt werden.
- **GRAPHICS-004:** Unicode-Codepoints und Graphemcluster MÜSSEN unterscheidbar sein.
- **GRAPHICS-005:** Ungültige Kodierungssequenzen MÜSSEN erkannt werden.
- **GRAPHICS-006:** NFC, NFD, NFKC und NFKD MÜSSEN unterstützt werden.
- **GRAPHICS-007:** Normalisierung DARF nicht ohne definierte Anforderung erfolgen.
- **GRAPHICS-008:** Graphem-, Wort- und Satzsegmentierung MÜSSEN verfügbar sein.
- **GRAPHICS-009:** Unicode-Zeilenumbruchregeln MÜSSEN unterstützt werden.
- **GRAPHICS-010:** Der Unicode Bidirectional Algorithm MUSS unterstützt werden.
- **GRAPHICS-011:** Logische und visuelle Textreihenfolge MÜSSEN unterscheidbar bleiben.
- **GRAPHICS-012:** Sprach- und Locale-Einstellungen MÜSSEN unabhängig von der Textkodierung sein.
- **GRAPHICS-013:** Die verwendete Unicode-Version MUSS abfragbar sein.
- **GRAPHICS-014:** Unicode-Datentabellen MÜSSEN versionskonsistent sein.
- **GRAPHICS-015:** Sicherheitskritische Textvergleiche DÜRFEN nicht implizit localeabhängig sein.
- **GRAPHICS-016:** Unicode-Funktionen MÜSSEN unabhängig vom Rendering-Backend funktionieren.
- **GRAPHICS-017:** Die Schnittstellen MÜSSEN Bestandteil des portablen Nova Framework sein.
- **GRAPHICS-018:** Die Verarbeitung MUSS offline und ohne KI-Abhängigkeit funktionieren.

## 14. Fehlerbehandlung

- Ungültige Unicode-Sequenzen werden gemäß der konfigurierten Fehlerstrategie behandelt.
- Nicht unterstützte Kodierungen werden eindeutig gemeldet.
- Unvollständige Streaming-Sequenzen werden bis zur Vervollständigung zwischengespeichert oder kontrolliert zurückgewiesen.
- Ungültige Locale-Kennungen lösen einen definierten Fallback aus.
- Fehlende Unicode-Daten werden erkannt und gemeldet.
- Fehlerhafte Eingaben DÜRFEN keine Speicherverletzungen oder unkontrollierten Zustandsänderungen verursachen.

## 15. Ergebnis

NovaOS erhält eine zentrale, portable und versionskontrollierte Unicode-Infrastruktur.

Internationale Texte können unabhängig von Sprache, Schriftsystem und Rendering-Backend korrekt verarbeitet werden.

Die Architektur bildet die gemeinsame Grundlage für mehrsprachige Benutzeroberflächen, Texteditoren und sämtliche textverarbeitenden Komponenten des Nova Framework.

**Grundsatz:** Text wird in NovaOS als strukturierte Unicode-Information verarbeitet. Sprache, Schreibrichtung und Darstellung sind Eigenschaften des Textes und keine Einschränkungen des Systems.