NPSPEC-DATATYPE-DOCUMENT-0001: Dokument- und Texttypen

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:**
  - NPSPEC-DATATYPE-0001
  - NPSPEC-DATATYPE-PRIMITIVE-0001
  - NPSPEC-DATATYPE-SEMANTIC-0001
  - NPSPEC-DATATYPE-COMPOSITE-0001
  - NPSPEC-DATAFLOW-0001
  - NPSPEC-CAPABILITY-CONTRACT-0001
  - NPSPEC-CAPABILITY-DISCOVERY-0001
  - NPSPEC-CAPABILITY-REGISTRY-0001
  - ADR-DATAFLOW-0002
  - ADR-DATAFLOW-0003
  - ADR-DATAFLOW-0005
  - ADR-DATAFLOW-0006

---

# 1. Zweck

Diese Spezifikation definiert die Dokument- und Texttypen der Nova Platform.

Sie beschreibt ein universelles, typisiertes Modell für:

- einfachen Text
- formatierten Text
- strukturierte Dokumente
- Quelltexte
- Konfigurationsdateien
- Präsentationen
- Tabellenkalkulationen
- elektronische Bücher
- technische Dokumentationen
- gescannte Dokumente
- hybride Dokumente
- Dokumentfragmente
- Dokumentensammlungen

Dokumente werden innerhalb der Nova Platform nicht ausschließlich als Dateien oder undurchsichtige Bytefolgen behandelt.

Sie werden als typisierte, strukturierte, versionierte und semantisch beschriebene Datenobjekte modelliert.

---

# 2. Ziele

Das Dokument- und Texttypmodell verfolgt folgende Ziele:

- einheitliche Repräsentation aller Dokumentarten
- Trennung von Inhalt, Struktur, Darstellung und Speicherung
- verlustarme Dokumentkonvertierung
- semantische Dokumentverarbeitung
- vollständige Durchsuchbarkeit
- sichere Capability-Komposition
- reproduzierbare Dokumenttransformation
- formatunabhängige Bearbeitung
- langfristige Versionierung
- barrierefreie Dokumentverarbeitung
- lokale KI-Integration
- plattformunabhängige Serialisierung

---

# 3. Grundprinzipien

Dokument- und Texttypen basieren auf folgenden Prinzipien:

- Dokumentinhalt und Dateiformat sind getrennte Konzepte.
- Text besitzt eine definierte Zeichenkodierung.
- Dokumente besitzen eine explizite Struktur.
- Formatierungen werden nicht mit semantischer Bedeutung gleichgesetzt.
- Dokumente sind standardmäßig unveränderlich.
- Änderungen erzeugen neue Dokumentversionen.
- Dokumentkonvertierungen erfolgen über explizite Capabilities.
- Verlustbehaftete Transformationen müssen gekennzeichnet werden.
- Dokumentteile müssen adressierbar sein.
- Dokumente können eingebettete Ressourcen enthalten.
- Metadaten sind typisiert und versioniert.
- Barrierefreiheitsinformationen sind Bestandteil des Dokumentmodells.
- Sicherheits- und Datenschutzklassifikationen bleiben erhalten.
- Herkunft und Bearbeitungshistorie werden über Provenance dokumentiert.

---

# 4. Architekturübersicht

```text
Nova Document Object
├── Document Identity
├── Document Type
├── Content Model
│   ├── Text
│   ├── Structure
│   ├── Semantics
│   ├── Formatting
│   └── Embedded Objects
├── Metadata
├── Resources
├── Accessibility
├── Security
├── Provenance
├── Serialization
└── Version
```

---

# 5. Dokumentbegriff

Ein Nova-Dokument ist ein typisiertes Datenobjekt, das Informationen in einer für Menschen oder Maschinen interpretierbaren Struktur enthält.

Ein Dokument kann bestehen aus:

```text
Text

Bildern

Tabellen

Diagrammen

Formeln

Multimedia

Metadaten

Interaktiven Elementen

Eingebetteten Objekten
```

Ein Dokument ist nicht zwingend an eine einzelne Datei gebunden.

Es kann:

- vollständig im Speicher liegen,
- auf mehrere Dateien verteilt sein,
- aus Referenzen bestehen,
- aus einem Datenstrom geladen werden,
- aus mehreren Dokumentfragmenten zusammengesetzt sein.

---

# 6. Dokumentidentität

Persistente Dokumente besitzen eine stabile Dokumentidentität.

Beispiel:

```text
DocumentID
```

Eine Dokumentidentität ist unabhängig von:

- Dateiname
- Speicherort
- Dateiformat
- aktueller Version
- Darstellung
- Exportformat

Mehrere Serialisierungen können dasselbe logische Dokument repräsentieren.

---

# 7. Dokumentversion

Jede inhaltliche oder strukturelle Änderung erzeugt eine neue Dokumentversion.

Beispiel:

```text
Document
├── Version 1
├── Version 2
└── Version 3
```

Eine Version besitzt mindestens:

```text
Document ID

Version ID

Parent Version

Creation Time

Creator

Change Summary

Content Hash
```

Die vorherige Version bleibt abhängig von der Aufbewahrungsrichtlinie verfügbar.

---

# 8. Dokumenttypen

Nova definiert folgende grundlegende Dokumenttypen:

```text
TextDocument

StructuredTextDocument

RichTextDocument

PageDocument

SpreadsheetDocument

PresentationDocument

SourceCodeDocument

ConfigurationDocument

NotebookDocument

FormDocument

BookDocument

ScannedDocument

CompoundDocument

DocumentCollection
```

Diese Typen können weiter spezialisiert werden.

---

# 9. TextDocument

Ein `TextDocument` enthält primär unformatierten Text.

Beispiele:

```text
PlainTextDocument

LogDocument

ReadmeDocument

TranscriptDocument
```

Der Inhalt basiert auf:

```text
String

oder

TextBuffer
```

Ein TextDocument besitzt keine verpflichtende visuelle Formatierung.

---

# 10. PlainTextDocument

Ein `PlainTextDocument` repräsentiert reinen Text ohne eingebettete Darstellungselemente.

Eigenschaften:

- Unicode-basiert
- zeilenorientiert oder kontinuierlich
- keine implizite Schriftart
- keine implizite Seitengröße
- keine eingebetteten Bilder
- keine versteckte Formatierungslogik

Beispiel:

```text
Technical Type:

String

Semantic Type:

nova.document.plain-text
```

---

# 11. StructuredTextDocument

Ein `StructuredTextDocument` enthält Text mit expliziter logischer Struktur.

Beispiele:

```text
MarkdownDocument

HTMLDocument

XMLDocument

AsciiDocDocument

ReStructuredTextDocument
```

Die Struktur kann enthalten:

- Überschriften
- Absätze
- Listen
- Zitate
- Tabellen
- Codeblöcke
- Verweise
- Fußnoten
- Metadaten

---

# 12. RichTextDocument

Ein `RichTextDocument` enthält Text mit visueller und struktureller Formatierung.

Beispiele:

```text
RTFDocument

OpenDocumentText

WordProcessingDocument
```

Es kann enthalten:

- Schriftarten
- Schriftgrößen
- Farben
- Absatzformatierungen
- Seitenumbrüche
- Kopf- und Fußzeilen
- eingebettete Medien
- Kommentare
- Änderungsverfolgung

Rich Text darf nicht als bloße formatierte Zeichenfolge modelliert werden.

---

# 13. PageDocument

Ein `PageDocument` besitzt eine festgelegte oder reproduzierbare Seitendarstellung.

Beispiele:

```text
PDFDocument

XPSDocument

PrintDocument
```

Eigenschaften:

- Seitenstruktur
- Seitengröße
- feste Positionierung
- Druckdarstellung
- optionale eingebettete Schriftarten
- optionale interaktive Formulare
- optionale digitale Signaturen

Ein PageDocument kann gleichzeitig eine zugängliche logische Struktur enthalten.

---

# 14. SpreadsheetDocument

Ein `SpreadsheetDocument` enthält tabellarische, formelbasierte oder analytische Daten.

Es besteht aus:

```text
Workbook

Worksheets

Cells

Ranges

Tables

Formulas

Charts

Metadata
```

Beispiele:

```text
OpenDocumentSpreadsheet

ExcelWorkbook

CSVTableDocument
```

CSV wird nur dann als SpreadsheetDocument behandelt, wenn ein gültiges Tabellenmodell zugeordnet wurde.

---

# 15. PresentationDocument

Ein `PresentationDocument` beschreibt eine Folge visueller Präsentationseinheiten.

Es enthält:

```text
Slides

Layouts

Themes

Animations

Transitions

Speaker Notes

Embedded Media
```

Beispiele:

```text
OpenDocumentPresentation

PowerPointPresentation
```

Die logische Präsentationsstruktur wird unabhängig von der konkreten Dateiserialisierung modelliert.

---

# 16. SourceCodeDocument

Ein `SourceCodeDocument` repräsentiert Programmquelltext.

Es besitzt zusätzliche semantische Informationen:

```text
Programming Language

Language Version

Module

Encoding

Syntax Tree

Symbol Information

Diagnostics
```

Beispiele:

```text
NovaLangSource

RustSource

CSource

VBNetSource

PythonSource

JavaScriptSource
```

Ein SourceCodeDocument kann sowohl den Originaltext als auch eine daraus abgeleitete Syntaxstruktur enthalten.

---

# 17. ConfigurationDocument

Ein `ConfigurationDocument` enthält deklarative Konfigurationsdaten.

Beispiele:

```text
JSONConfiguration

YAMLConfiguration

TOMLConfiguration

INIConfiguration

XMLConfiguration
```

Es kann einem registrierten Schema zugeordnet werden.

Beispiel:

```text
ConfigurationDocument<NovaRuntimeConfiguration>
```

Konfigurationsdokumente dürfen nicht allein anhand ihrer Dateiendung als vertrauenswürdig gelten.

---

# 18. NotebookDocument

Ein `NotebookDocument` kombiniert ausführbare und nicht ausführbare Inhalte.

Es besteht aus geordneten Zellen.

Zelltypen können sein:

```text
TextCell

CodeCell

OutputCell

FormulaCell

VisualizationCell

InputCell
```

Ein Notebook enthält zusätzlich:

- Laufzeitinformationen
- Kernel- oder Runtime-Referenzen
- Ausführungsreihenfolge
- Zellabhängigkeiten
- Ergebnisprovenance

---

# 19. FormDocument

Ein `FormDocument` enthält interaktive oder ausfüllbare Felder.

Beispiele:

```text
ApplicationForm

SurveyForm

InspectionForm

ProtocolForm

PDFForm
```

Formularelemente können sein:

```text
TextField

NumberField

DateField

Checkbox

RadioGroup

SelectionList

SignatureField

FileField
```

Jedes Formularfeld besitzt einen definierten Datentyp und Validierungsregeln.

---

# 20. BookDocument

Ein `BookDocument` beschreibt ein umfangreiches, gegliedertes Dokument.

Es kann enthalten:

```text
Cover

Title Page

Chapters

Sections

Table of Contents

Footnotes

Endnotes

Bibliography

Index

Appendices
```

Beispiele:

```text
EPUBDocument

TechnicalManual

NovelDocument

ScientificBook
```

Ein Buch kann aus mehreren Dokumentressourcen zusammengesetzt sein.

---

# 21. ScannedDocument

Ein `ScannedDocument` basiert primär auf aufgenommenen oder gescannten Seitenbildern.

Es kann enthalten:

```text
Page Images

OCR Text

Layout Regions

Confidence Values

Language Information

Correction Layers
```

OCR-Ergebnisse werden als abgeleitete Daten behandelt.

Der erkannte Text ersetzt nicht automatisch das ursprüngliche Seitenbild.

---

# 22. CompoundDocument

Ein `CompoundDocument` kombiniert mehrere Dokument- oder Medientypen.

Beispiele:

```text
Document with Attachments

Email with Files

Technical Report with CAD Models

Project Documentation Package
```

Ein CompoundDocument besteht aus:

```text
Primary Document

Embedded Documents

Referenced Documents

Resources

Relationship Graph
```

---

# 23. DocumentCollection

Eine `DocumentCollection` enthält mehrere logisch zusammengehörige Dokumente.

Beispiele:

```text
Project Documentation

Case File

Technical Documentation Set

Contract Package

Archive Collection
```

Eine Collection kann:

- geordnet
- ungeordnet
- hierarchisch
- versioniert
- durchsuchbar

sein.

---

# 24. Textmodell

Text wird in Nova als Folge von Unicode-Codepunkten modelliert.

Das logische Textmodell ist unabhängig von der externen Zeichenkodierung.

```text
External Encoding
↓
Decoder
↓
Nova Unicode Text
↓
Encoder
↓
External Encoding
```

Unterstützte externe Kodierungen können sein:

```text
UTF-8

UTF-16

UTF-32

ISO-8859-1

Windows-1252
```

UTF-8 ist die bevorzugte Standardserialisierung.

---

# 25. Unicode-Normalisierung

Texttypen müssen ihre Unicode-Normalisierungsstrategie deklarieren.

Unterstützte Formen:

```text
NFC

NFD

NFKC

NFKD

Preserve
```

Die Runtime darf Text nicht unbemerkt normalisieren, wenn dadurch:

- digitale Signaturen
- Quelltext
- Identifikatoren
- kryptographische Werte
- historische Dokumente

verändert werden könnten.

---

# 26. TextBuffer

Für große oder häufig bearbeitete Texte kann ein `TextBuffer` verwendet werden.

Ein TextBuffer kann intern implementiert werden als:

```text
Rope

Piece Table

Gap Buffer

Chunked Buffer

Persistent Text Tree
```

Das interne Modell darf die logische Textdarstellung nicht verändern.

TextBuffer unterstützen:

- effiziente Einfügungen
- effiziente Löschungen
- Bereichsreferenzen
- Versionierung
- inkrementelle Parser
- parallele Analyse

---

# 27. Zeilenmodell

Textdokumente können in logische Zeilen unterteilt werden.

Zeilenenden werden intern normalisiert oder explizit erhalten.

Unterstützte externe Zeilenenden:

```text
LF

CRLF

CR
```

Ein Dokument kann deklarieren:

```text
normalizeLineEndings: true
```

oder:

```text
preserveLineEndings: true
```

Quelltexte und signierte Dokumente sollen ihre ursprünglichen Zeilenenden erhalten können.

---

# 28. Textpositionen

Positionen in einem Text werden nicht ausschließlich durch rohe Byteoffsets beschrieben.

Unterstützte Positionsmodelle:

```text
Byte Offset

Code Unit Offset

Code Point Offset

Grapheme Offset

Line and Column

Text Anchor
```

Öffentliche Dokument-APIs müssen angeben, welches Positionsmodell verwendet wird.

---

# 29. Grapheme Cluster

Ein sichtbares Zeichen kann aus mehreren Unicode-Codepunkten bestehen.

Benutzerorientierte Cursor- und Auswahloperationen sollen deshalb Grapheme Cluster verwenden.

Beispiel:

```text
Unicode-Codepunkte

≠

sichtbare Zeichen
```

Compiler und Parser können weiterhin Codepunkt- oder Bytepositionen verwenden.

---

# 30. TextRange

Ein `TextRange` beschreibt einen zusammenhängenden Textbereich.

Es enthält:

```text
Document Reference

Start Position

End Position

Position Model

Version
```

Ein TextRange ist immer an eine Dokumentversion gebunden.

Nach Änderungen kann eine Neuzuordnung über Text Anchors erforderlich sein.

---

# 31. TextAnchor

Ein `TextAnchor` beschreibt eine möglichst stabile Position innerhalb eines veränderlichen Dokuments.

Er kann basieren auf:

- benachbarten Textfragmenten
- strukturellen Knoten
- persistenten Marker-IDs
- Änderungsoperationen
- Versionstransformationen

Text Anchors werden verwendet für:

- Kommentare
- Diagnosen
- Lesezeichen
- Annotationen
- kollaborative Bearbeitung

---

# 32. Dokumentstruktur

Strukturierte Dokumente werden als Dokumentbaum oder Dokumentgraph modelliert.

Beispiel:

```text
Document
├── Metadata
├── Heading
├── Paragraph
│   ├── Text
│   └── Emphasis
├── List
│   ├── ListItem
│   └── ListItem
└── Table
```

Jeder Strukturknoten besitzt:

```text
Node ID

Node Type

Attributes

Children

Semantic Role
```

---

# 33. DocumentNode

Ein `DocumentNode` ist die grundlegende strukturelle Einheit eines Dokuments.

Standardknoten umfassen:

```text
DocumentRoot

Section

Heading

Paragraph

TextRun

List

ListItem

Table

TableRow

TableCell

Figure

Caption

Quote

CodeBlock

Link

Footnote

PageBreak
```

Dokumentformate dürfen spezialisierte Knoten ergänzen.

---

# 34. Node ID

Jeder adressierbare Dokumentknoten kann eine stabile Node ID besitzen.

Node IDs ermöglichen:

- Querverweise
- Annotationen
- inkrementelle Bearbeitung
- Änderungsverfolgung
- kollaborative Bearbeitung
- Teilaktualisierungen
- Provenance auf Elementebene

Eine Node ID darf nicht ohne Migration wiederverwendet werden.

---

# 35. Block- und Inline-Elemente

Nova unterscheidet:

```text
Block Elements

Inline Elements
```

Block-Elemente:

```text
Paragraph

Heading

Table

List

CodeBlock

Figure
```

Inline-Elemente:

```text
TextRun

Emphasis

Strong

Link

InlineCode

Citation

LineBreak
```

Die zulässige Verschachtelung wird durch das jeweilige Dokumentmodell definiert.

---

# 36. Semantische Struktur

Visuelle Formatierung und semantische Struktur sind getrennt.

Beispiel:

```text
Semantik:

Heading Level 1
```

```text
Darstellung:

32 pt, fett
```

Ein großer, fett formatierter Absatz ist nicht automatisch eine Überschrift.

Diese Trennung ist erforderlich für:

- Barrierefreiheit
- automatische Inhaltsverzeichnisse
- semantische Suche
- alternative Darstellung
- Dokumentkonvertierung
- KI-Verarbeitung

---

# 37. Formatierung

Formatierungen werden als typisierte Eigenschaften modelliert.

Beispiele:

```text
Font Family

Font Size

Font Weight

Text Color

Background

Alignment

Spacing

Indentation

Border

Page Break
```

Formatierungen können angewendet werden auf:

- Dokumente
- Abschnitte
- Blöcke
- Textbereiche
- Tabellen
- Zellen
- eingebettete Elemente

---

# 38. Styles

Wiederverwendbare Formatierungen werden als Styles modelliert.

Beispiel:

```text
Style:

Heading1

Properties:

fontSize: 24pt

fontWeight: bold

spacingBefore: 12pt

spacingAfter: 6pt
```

Styles besitzen:

```text
Style ID

Name

Parent Style

Properties

Scope

Version
```

Direkte Formatierung kann Styles überschreiben, muss aber explizit gekennzeichnet sein.

---

# 39. Themes

Ein Theme beschreibt dokumentweite Designentscheidungen.

Es kann enthalten:

```text
Color Palette

Typography

Spacing

Page Styles

Table Styles

Chart Styles
```

Themes dürfen die semantische Dokumentstruktur nicht verändern.

---

# 40. Layout

Das Layout beschreibt die räumliche Anordnung von Dokumentelementen.

Nova unterscheidet:

```text
Flow Layout

Page Layout

Grid Layout

Absolute Layout

Responsive Layout
```

Ein Dokument kann mehrere Layoutdarstellungen besitzen.

Beispiel:

```text
BookDocument
├── Screen Layout
├── Print Layout
└── Accessible Linear Layout
```

---

# 41. Seitenmodell

Seitengestützte Dokumente besitzen:

```text
Page Size

Orientation

Margins

Header

Footer

Page Number

Columns

Page Background
```

Seitengrößen werden mit dem Nova-Einheitensystem beschrieben.

Beispiel:

```text
width: 210 mm

height: 297 mm
```

---

# 42. Tabellen

Dokumenttabellen werden als strukturierte Dokumentelemente modelliert.

Eine Tabelle besteht aus:

```text
Table

Rows

Columns

Cells

Headers

Spans

Styles
```

Tabellen müssen ihre semantischen Kopfbeziehungen beschreiben können.

Dies ist besonders für barrierefreie Ausgabe erforderlich.

---

# 43. Listen

Listen besitzen:

```text
Ordered

Unordered

Definition

Task
```

Eine Liste enthält typisierte List Items.

Nummerierungen und Aufzählungszeichen sind Darstellungsinformationen und nicht die alleinige semantische Definition einer Liste.

---

# 44. Verweise

Dokumente können interne und externe Verweise enthalten.

```text
Internal Reference

External URI

Document Reference

Citation Reference

Attachment Reference
```

Ein Verweis besitzt:

```text
Target

Label

Relationship Type

Access Policy

Validation State
```

Externe Verweise dürfen nicht automatisch geladen oder ausgeführt werden.

---

# 45. Querverweise

Ein Querverweis zeigt auf einen adressierbaren Dokumentknoten.

Beispiele:

```text
Kapitel

Abbildung

Tabelle

Formel

Fußnote

Literaturquelle
```

Die sichtbare Beschriftung kann automatisch aus dem Ziel erzeugt werden.

---

# 46. Fußnoten und Endnoten

Fußnoten und Endnoten werden als verknüpfte Dokumentknoten modelliert.

Sie bestehen aus:

```text
Reference Node

Note Node

Stable Note ID

Display Policy
```

Eine Konvertierung zwischen Fußnote und Endnote darf die fachliche Beziehung nicht verlieren.

---

# 47. Zitate und Literaturangaben

Wissenschaftliche Dokumente können typisierte Zitierungen enthalten.

Ein Citation-Element enthält:

```text
Citation ID

Source Reference

Locator

Citation Style

Rendered Text
```

Bibliographische Daten werden getrennt von der gerenderten Zitation gespeichert.

---

# 48. Formeln

Mathematische Formeln werden als semantische Formelausdrücke modelliert.

Unterstützte Repräsentationen können sein:

```text
MathML

LaTeX Expression

Nova Expression Tree

Rendered Image
```

Die gerenderte Darstellung soll nicht die einzige verfügbare Form sein.

---

# 49. Codeblöcke

Codeblöcke besitzen zusätzliche Eigenschaften:

```text
Programming Language

Language Version

Syntax Highlighting

Execution Policy

Line Numbers

Source Reference
```

Ein Codeblock ist standardmäßig nicht ausführbar.

Ausführung erfordert eine explizite Capability und passende Berechtigungen.

---

# 50. Eingebettete Ressourcen

Dokumente können Ressourcen einbetten oder referenzieren.

Beispiele:

```text
Images

Fonts

Audio

Video

Attachments

Stylesheets

Scripts

Data Files
```

Jede Ressource besitzt:

```text
Resource ID

Media Type

Semantic Type

Content Hash

Location

Embedding Mode

Security Classification
```

---

# 51. Eingebettete Bilder

Bilder werden als referenzierte oder eingebettete Nova-Datenobjekte behandelt.

Ein Bild im Dokument enthält zusätzlich:

```text
Alternative Text

Caption

Display Size

Crop

Position

Relationship to Text
```

Das Dokumentformat darf den ursprünglichen Bildtyp nicht unbemerkt verändern.

---

# 52. Eingebettete Schriftarten

Schriftarten können eingebettet werden, sofern:

- die Lizenz dies erlaubt,
- Sicherheitsprüfungen erfolgreich sind,
- die Ressource validiert wurde.

Das Dokument muss ein Fallback-Verhalten definieren.

Eingebettete Schriftarten gelten als potenziell aktive oder komplexe Binärressourcen und müssen isoliert verarbeitet werden.

---

# 53. Aktive Inhalte

Dokumente können aktive Inhalte enthalten.

Beispiele:

```text
Scripts

Macros

External Data Connections

Embedded Applications

Interactive Actions
```

Aktive Inhalte sind standardmäßig deaktiviert.

Sie benötigen:

- explizite Berechtigungen
- Capability-basierte Ausführung
- Sandbox-Isolation
- nachvollziehbare Herkunft
- Benutzerfreigabe oder Systemrichtlinie

---

# 54. Dokumentmetadaten

Dokumentmetadaten werden typisiert gespeichert.

Standardfelder können sein:

```text
Title

Subtitle

Description

Author

Organization

Language

Created

Modified

Keywords

Subject

Category

License

Copyright

Revision

Template
```

Freie Metadaten dürfen über namespacierte Erweiterungen ergänzt werden.

---

# 55. Autorenschaft

Autorenschaft wird getrennt von technischer Erzeugung modelliert.

Mögliche Rollen:

```text
Author

Editor

Reviewer

Translator

Illustrator

Publisher

Generator

Converter
```

Eine Capability, die ein Dokument exportiert, wird nicht automatisch zum fachlichen Autor.

---

# 56. Sprache

Dokumente und Dokumentteile können Sprachinformationen besitzen.

Beispiel:

```text
Document Language:

de-DE
```

Einzelne Bereiche können abweichen:

```text
Paragraph Language:

en-US
```

Sprachinformationen beeinflussen:

- Rechtschreibprüfung
- Silbentrennung
- Text-to-Speech
- Übersetzung
- Suchindexierung
- Barrierefreiheit

---

# 57. Schreibrichtung

Text kann folgende Schreibrichtungen besitzen:

```text
Left to Right

Right to Left

Top to Bottom

Auto
```

Bidirektionaler Text muss Unicode-konform verarbeitet werden.

Die Schreibrichtung darf nicht ausschließlich aus der Sprache abgeleitet werden.

---

# 58. Lokalisierung

Ein Dokument kann lokalisierbare Inhalte enthalten.

Lokalisierbare Einheiten besitzen:

```text
Translation Unit ID

Source Text

Source Language

Target Language

Context

Status

Placeholders
```

Übersetzungen erzeugen neue Dokumentvarianten oder verknüpfte Sprachversionen.

---

# 59. Annotationen

Annotationen ergänzen ein Dokument, ohne dessen Hauptinhalt unmittelbar zu verändern.

Beispiele:

```text
Comment

Highlight

Bookmark

Correction

Review Note

Semantic Tag

AI Suggestion
```

Eine Annotation besitzt:

```text
Annotation ID

Target

Author

Created

Type

Content

Status
```

---

# 60. Kommentare

Kommentare sind spezialisierte Annotationen.

Sie können:

- beantwortet
- aufgelöst
- wieder geöffnet
- versioniert
- einem Benutzer zugeordnet

werden.

Kommentare gehören nicht automatisch zum exportierten Hauptinhalt.

---

# 61. Änderungsverfolgung

Dokumentänderungen können als Operationen gespeichert werden.

Beispiele:

```text
Insert Text

Delete Text

Replace Text

Move Node

Change Style

Add Comment

Accept Change

Reject Change
```

Jede Änderung enthält:

```text
Change ID

Author

Timestamp

Base Version

Operation

Target

Status
```

---

# 62. Kollaborative Bearbeitung

Für parallele Bearbeitung können Mechanismen wie folgende verwendet werden:

```text
Operational Transformation

CRDT

Version Branching

Merge Operations
```

Die konkrete Methode ist nicht Bestandteil des logischen Dokumenttyps.

Das Ergebnis muss jedoch:

- deterministisch zusammenführbar
- nachvollziehbar
- versioniert
- konfliktanzeigefähig

sein.

---

# 63. Dokumentfragmente

Ein `DocumentFragment` enthält einen adressierbaren Teil eines Dokuments.

Beispiele:

```text
Section Fragment

Paragraph Fragment

Table Fragment

Text Selection

Slide Fragment

Worksheet Range
```

Fragmente besitzen:

```text
Source Document

Source Version

Root Nodes

Dependencies

Resource References
```

---

# 64. Kopieren und Einfügen

Zwischen Dokumenten übertragene Inhalte werden als DocumentFragment verarbeitet.

Ein Fragment kann mehrere Repräsentationen anbieten:

```text
Native Structured Fragment

Plain Text

HTML

Image Preview
```

Die Zielanwendung wählt die geeignetste kompatible Repräsentation.

Aktive Inhalte dürfen nicht ungeprüft übernommen werden.

---

# 65. Dokumentvorlagen

Ein `DocumentTemplate` beschreibt eine wiederverwendbare Dokumentstruktur.

Es kann enthalten:

```text
Fixed Content

Placeholders

Styles

Themes

Rules

Default Metadata

Generation Instructions
```

Platzhalter besitzen definierte Datentypen.

Beispiel:

```text
customerName: PersonName

invoiceDate: Date

totalAmount: CurrencyAmount
```

---

# 66. Platzhalter

Ein Dokumentplatzhalter ist ein typisierter Einfügepunkt.

Er besitzt:

```text
Placeholder ID

Expected Type

Required

Default Value

Formatting

Validation
```

Ein Platzhalter darf nur mit kompatiblen Daten befüllt werden.

---

# 67. Dokumentgenerierung

Dokumente können aus:

- Vorlagen
- Datenobjekten
- Datenbanken
- Capability-Ergebnissen
- KI-Ausgaben
- Benutzerinteraktionen

erzeugt werden.

Die Generierung muss dokumentieren:

```text
Template Version

Input Objects

Generator Capability

Generator Version

Parameters

Timestamp
```

---

# 68. Dokumentanalyse

Dokumentanalyse-Capabilities können erzeugen:

```text
Document Structure

Extracted Text

Keywords

Summary

Entities

Language

Classification

Embeddings

Accessibility Report
```

Analyseergebnisse sind abgeleitete Datenobjekte und dürfen das Originaldokument nicht stillschweigend verändern.

---

# 69. Textextraktion

Textextraktion muss ihre Quelle und Qualität dokumentieren.

Mögliche Quellen:

```text
Native Text Layer

OCR

Speech Recognition

Metadata

Embedded Alternative Text
```

Ein Extraktionsergebnis enthält:

```text
Source Region

Extraction Method

Confidence

Language

Ordering
```

---

# 70. OCR

OCR wird als explizite Konverter- oder Analyse-Capability behandelt.

Beispiel:

```text
ScannedDocument

↓

OCR Capability

↓

StructuredTextDocument
```

OCR-Ausgaben müssen:

- Vertrauenswerte unterstützen,
- Seiten- und Bereichsbezüge erhalten,
- unsichere Zeichen kennzeichnen,
- das Originalbild referenzieren,
- Korrekturen versionieren.

---

# 71. Semantische Analyse

Semantische Dokumentanalyse kann erkennen:

```text
Persons

Organizations

Locations

Dates

Tasks

Decisions

Requirements

Risks

References
```

Erkannte Informationen werden als Annotationen oder separate Datenobjekte gespeichert.

Sie gelten nicht automatisch als zweifelsfrei korrekt.

---

# 72. Dokumentindexierung

Dokumente können für Suchsysteme indexiert werden.

Ein Index kann enthalten:

```text
Full Text

Structure

Metadata

Semantic Types

Entities

Embeddings

Access Rights

Version
```

Der Index muss die Zugriffsrechte des Quelldokuments respektieren.

Ein Benutzer darf über Suchergebnisse keine geschützten Inhalte ableiten können.

---

# 73. Volltextsuche

Volltextsuche unterstützt:

```text
Exact Search

Token Search

Phrase Search

Fuzzy Search

Regular Expressions

Language-Aware Search
```

Suchergebnisse referenzieren:

```text
Document ID

Version

Text Range

Matching Fragment

Score
```

---

# 74. Semantische Suche

Semantische Suche verwendet Bedeutungsinformationen, Metadaten und Embeddings.

Sie muss:

- Zugriffsrechte beachten,
- Modellversionen dokumentieren,
- Ergebnisse nachvollziehbar referenzieren,
- Indexversionen berücksichtigen,
- lokale Verarbeitung unterstützen.

Embeddings ersetzen nicht die ursprünglichen Dokumentinhalte.

---

# 75. Dokumentkonvertierung

Jede Dokumentkonvertierung erfolgt über eine registrierte Converter Capability.

Beispiele:

```text
MarkdownDocument
↓
HTMLDocument
```

```text
WordProcessingDocument
↓
PDFDocument
```

```text
ScannedDocument
↓
StructuredTextDocument
```

Implizite Formatänderungen sind unzulässig.

---

# 76. Konvertierungsqualität

Konverter deklarieren eine Qualitätsklasse.

```text
Lossless

Semantically Lossless

Visually Lossless

Potentially Lossy

Lossy

Irreversible
```

Bedeutung:

```text
Lossless

Sämtliche Informationen bleiben erhalten.
```

```text
Semantically Lossless

Die fachliche Struktur bleibt erhalten,
die Darstellung kann sich ändern.
```

```text
Visually Lossless

Die Darstellung bleibt erhalten,
die editierbare Struktur kann verloren gehen.
```

```text
Potentially Lossy

Informationsverlust ist formatabhängig möglich.
```

```text
Lossy

Bekannte Informationen gehen verloren.
```

```text
Irreversible

Eine vollständige Rückkonvertierung ist nicht möglich.
```

---

# 77. Konvertierungsbericht

Jede nicht vollständig verlustfreie Konvertierung kann einen Bericht erzeugen.

Dieser enthält:

```text
Source Type

Target Type

Converter

Preserved Features

Changed Features

Removed Features

Warnings

Quality Classification
```

Beispiel:

```text
Markdown → PlainText

Removed:

- heading hierarchy
- links
- emphasis
- tables
```

---

# 78. Export

Export erzeugt eine externe Dokumentrepräsentation.

Ein Export verändert nicht automatisch den Typ des Quelldokuments.

Beispiel:

```text
Nova RichTextDocument
↓
PDF Export Capability
↓
PDFDocument
```

Das Ergebnis ist ein neues Datenobjekt mit eigener Provenance.

---

# 79. Import

Import wandelt eine externe Repräsentation in ein Nova-Dokumentmodell um.

Der Importprozess umfasst:

```text
Format Detection

Security Validation

Decoding

Parsing

Schema Validation

Resource Resolution

Type Assignment

Provenance Creation
```

Dateiendungen dürfen nur als Hinweis dienen.

---

# 80. Formaterkennung

Formaterkennung verwendet mehrere Signale:

```text
Magic Bytes

Container Structure

Declared Media Type

File Extension

Internal Signatures

Parser Validation
```

Das Ergebnis kann folgende Zustände besitzen:

```text
Confirmed

Probable

Ambiguous

Unknown

Invalid
```

Ein Format darf nicht ausschließlich aufgrund des Dateinamens als bestätigt gelten.

---

# 81. Dateiformat und Dokumenttyp

Dateiformat und Dokumenttyp sind getrennt.

Beispiel:

```text
Dateiformat:

ZIP Container
```

```text
Dokumenttyp:

OpenDocumentText
```

Ein Dateiformat kann mehrere Dokumenttypen transportieren.

Ein Dokumenttyp kann mehrere Serialisierungsformate besitzen.

---

# 82. Media Type

Dokumentserialisierungen können einen registrierten Media Type besitzen.

Beispiele:

```text
text/plain

text/markdown

text/html

application/pdf

application/json
```

Media Types ergänzen die Nova-Type-ID, ersetzen sie jedoch nicht.

---

# 83. MIME- und Typsicherheit

Ein deklarierter Media Type muss durch Inhaltsprüfung bestätigt werden können.

Bei Abweichungen zwischen:

- deklarierter Type ID
- Media Type
- Dateiendung
- erkanntem Format

muss die Runtime eine Warnung oder einen Fehler erzeugen.

---

# 84. Serialisierung

Dokumenttypen können serialisiert werden als:

```text
Nova Document Binary

Nova Document JSON

Native External Format

Archive Package

Content-Addressed Object Graph
```

Die Serialisierung muss:

- Typinformationen
- Dokumentversion
- Struktur
- Ressourcenbeziehungen
- Metadaten
- Sicherheitsklassifikation

erhalten oder eindeutig referenzieren.

---

# 85. Nova Document Binary

Das native Nova-Dokumentformat kann bestehen aus:

```text
Document Header

Type Information

Structure Graph

Text Chunks

Style Definitions

Resource Table

Metadata

Provenance References

Integrity Information
```

Es soll unterstützen:

- Lazy Loading
- partielle Aktualisierung
- Chunking
- Kompression
- Content Addressing
- deduplizierte Ressourcen
- schnelle Indexierung

---

# 86. Paketbasierte Dokumente

Komplexe Dokumente können als Paket serialisiert werden.

Beispiel:

```text
Package
├── manifest.yaml
├── document.bin
├── resources/
├── metadata/
└── signatures/
```

Das Manifest beschreibt alle enthaltenen Ressourcen und Beziehungen.

Pfadmanipulationen und unzulässige externe Referenzen müssen verhindert werden.

---

# 87. Externe Ressourcen

Dokumente können externe Ressourcen referenzieren.

Eine externe Referenz enthält:

```text
URI

Expected Type

Integrity Hash

Access Policy

Cache Policy

Fallback
```

Externe Ressourcen werden nicht ohne Berechtigungsprüfung geladen.

Für reproduzierbare Dokumente soll ein Integritätshash vorhanden sein.

---

# 88. Offlinefähigkeit

Dokumente können ihre Offlineanforderungen deklarieren.

```text
SelfContained

OfflineCapable

OnlineDependent
```

Ein selbstenthaltenes Dokument darf keine zwingenden externen Ressourcen benötigen.

---

# 89. Integrität

Dokumente können Integritätsinformationen besitzen.

Beispiele:

```text
Document Hash

Resource Hashes

Merkle Tree

Digital Signature

Timestamp Signature
```

Integritätsprüfungen beziehen sich auf eine eindeutig definierte kanonische Repräsentation.

---

# 90. Digitale Signaturen

Ein Dokument kann eine oder mehrere digitale Signaturen besitzen.

Eine Signatur enthält:

```text
Signer

Signature Algorithm

Signed Scope

Document Version

Timestamp

Certificate Information

Validation State
```

Signaturen können gelten für:

- gesamtes Dokument
- einzelne Abschnitte
- bestimmte Ressourcen
- Metadaten
- Formularfelder

---

# 91. Signaturgültigkeit

Jede Veränderung an einem signierten Bereich muss die Signaturprüfung beeinflussen.

Mögliche Zustände:

```text
Valid

Invalid

Unknown

Expired

Revoked

Partially Valid

Content Modified
```

Die Runtime darf eine ungültige Signatur nicht als gültig darstellen.

---

# 92. Verschlüsselung

Dokumente können vollständig oder teilweise verschlüsselt werden.

Mögliche Bereiche:

```text
Entire Document

Selected Sections

Attachments

Metadata

Form Fields

Comments
```

Verschlüsselung wird über registrierte kryptographische Capabilities ausgeführt.

---

# 93. Zugriffsrechte

Dokumentrechte können umfassen:

```text
Read

Edit

Comment

Annotate

Print

Copy

Export

Sign

Share

Execute Active Content
```

Rechte werden durch das Nova-Capability- und Sicherheitsmodell durchgesetzt.

Dokumentinterne Beschränkungen allein gelten nicht als vollständiger Schutz.

---

# 94. Datenschutzklassifikation

Dokumente und einzelne Bestandteile können klassifiziert werden.

Beispiele:

```text
Public

Internal

Confidential

PersonalData

SensitivePersonalData

Secret
```

Die Klassifikation beeinflusst:

- Speicherung
- Synchronisation
- Indexierung
- Protokollierung
- KI-Verarbeitung
- Export
- Löschung

---

# 95. Vertrauliche Felder

Ein Dokument kann vertrauliche Teilbereiche besitzen.

Beispiel:

```text
FormDocument
├── Name: PersonalData
├── Address: PersonalData
└── Password: Secret
```

Vertrauliche Felder dürfen:

- nicht unmaskiert protokolliert,
- nicht in allgemeine Vorschauen aufgenommen,
- nicht ohne Berechtigung indexiert

werden.

---

# 96. Redaction

Redaction entfernt oder verdeckt vertrauliche Inhalte kontrolliert.

Nova unterscheidet:

```text
Visual Redaction

Logical Redaction

Cryptographic Redaction

Permanent Redaction
```

Eine bloße schwarze Rechteckfläche über sichtbarem Text gilt nicht als sichere permanente Redaction.

---

# 97. Dokumentbereinigung

Eine Sanitization Capability kann entfernen:

```text
Hidden Metadata

Comments

Tracked Changes

Scripts

Macros

External Links

Embedded Files

Personal Information

Unused Resources
```

Die Bereinigung erzeugt ein neues Dokumentobjekt und einen Bericht.

---

# 98. Sichere Vorschau

Nicht vertrauenswürdige Dokumente werden in einer isolierten Vorschau dargestellt.

Die Vorschau darf standardmäßig nicht:

- aktive Inhalte ausführen,
- externe Verbindungen aufbauen,
- lokale Dateien lesen,
- eingebettete Programme starten,
- Systemschriftarten verändern.

---

# 99. Barrierefreiheit

Dokumenttypen müssen Barrierefreiheitsinformationen unterstützen.

Dazu gehören:

```text
Logical Reading Order

Heading Structure

Alternative Text

Table Headers

Language

Form Labels

Captions

Descriptions

Navigation Landmarks
```

Barrierefreiheit ist Bestandteil des Dokumentmodells und keine ausschließlich visuelle Zusatzfunktion.

---

# 100. Lesereihenfolge

Die logische Lesereihenfolge muss unabhängig vom visuellen Layout definierbar sein.

Dies ist besonders relevant für:

- mehrspaltige Dokumente
- Präsentationen
- PDF
- komplexe Formulare
- frei positionierte Elemente

Eine fehlende oder widersprüchliche Lesereihenfolge muss diagnostizierbar sein.

---

# 101. Alternativtexte

Nicht textuelle Inhalte können Alternativtexte besitzen.

Beispiele:

```text
Image Alternative Text

Chart Description

Formula Description

Audio Transcript

Video Caption
```

Automatisch erzeugte Alternativtexte müssen als solche gekennzeichnet werden.

---

# 102. Accessibility Report

Eine Prüf-Capability kann einen Barrierefreiheitsbericht erzeugen.

Dieser enthält:

```text
Document Version

Standard Profile

Errors

Warnings

Affected Nodes

Recommendations

Automated Checks

Manual Checks Required
```

Der Bericht ist ein separates Datenobjekt.

---

# 103. Dokumentvorschau

Eine Vorschau ist eine abgeleitete Darstellung eines Dokuments.

Beispiele:

```text
Thumbnail

Page Image

Text Preview

Slide Preview

First Page Preview
```

Vorschauen:

- besitzen eigene Typen,
- referenzieren die Quelldokumentversion,
- dürfen vertrauliche Inhalte nicht unkontrolliert offenlegen,
- können veraltet sein.

---

# 104. Rendering

Rendering überführt ein Dokumentmodell in eine visuelle, auditive oder druckbare Darstellung.

Beispiele:

```text
Document → Screen Surface

Document → Page Images

Document → Printer Commands

Document → Speech
```

Rendering erzeugt keine inhaltliche Dokumentänderung.

---

# 105. Renderingprofile

Ein Renderingprofil kann definieren:

```text
Target Device

Page Size

Resolution

Color Space

Font Policy

Accessibility Mode

Animation Policy

Resource Limits
```

Das Profil wird in der Provenance des Rendering-Ergebnisses dokumentiert.

---

# 106. Drucken

Drucken ist eine spezialisierte Rendering- und Ausgabeoperation.

Ein PrintJob enthält:

```text
Document Reference

Document Version

Page Selection

Copies

Duplex

Color Mode

Paper Size

Scaling

Printer Capability
```

Die Druckausgabe muss die ausgewählte Dokumentversion verwenden.

---

# 107. Text-to-Speech

Textdokumente können durch eine Speech Capability vorgelesen werden.

Die Verarbeitung berücksichtigt:

```text
Language

Reading Order

Headings

Alternative Text

Pronunciation

Hidden Content Policy
```

Das erzeugte Audio ist ein separates Datenobjekt.

---

# 108. Übersetzung

Dokumentübersetzung wird als explizite Transformations-Capability ausgeführt.

Beispiel:

```text
GermanDocument
↓
Translation Capability
↓
EnglishDocument
```

Die Übersetzung muss:

- Ausgangs- und Zielsprache dokumentieren,
- Struktur möglichst erhalten,
- unsichere Passagen kennzeichnen,
- Terminologieprofile unterstützen,
- die Quelldokumentversion referenzieren.

---

# 109. Zusammenfassung

Eine Zusammenfassungs-Capability erzeugt ein neues Text- oder Dokumentobjekt.

Das Ergebnis enthält:

```text
Source Document

Source Version

Summary Type

Model or Algorithm

Parameters

Generated Time
```

Eine Zusammenfassung ersetzt nicht das Quelldokument.

---

# 110. KI-generierte Dokumentinhalte

KI-generierte Inhalte müssen als solche kennzeichenbar sein.

Mögliche Metadaten:

```text
GeneratedByAI

Model ID

Model Version

Prompt Reference

Generation Parameters

Human Reviewed

Review Status
```

Die konkrete Benutzeroberfläche kann diese Informationen abhängig von Richtlinien anzeigen.

---

# 111. Dokument-Embeddings

Embeddings können für Dokumente oder Dokumentteile erzeugt werden.

Ein Embedding referenziert:

```text
Source Document

Source Version

Source Range or Node

Model ID

Model Version

Dimension

Normalization

Created
```

Embeddings gelten als abgeleitete Daten und können eigenen Datenschutzregeln unterliegen.

---

# 112. Provenance

Die Provenance eines Dokuments dokumentiert:

```text
Origin

Importer

Parser

Generator

Editors

Converters

Transformations

Resources

Source Documents

Model Usage

Signatures

Migrations
```

Jede Dokumentversion besitzt eine nachvollziehbare Beziehung zu ihren Vorgängern und Quellen.

---

# 113. Dokumentbeziehungen

Zwischen Dokumenten können typisierte Beziehungen bestehen.

Beispiele:

```text
DerivedFrom

TranslationOf

RevisionOf

AttachmentOf

PartOf

References

Replaces

Supersedes

SummaryOf

TemplateOf
```

Diese Beziehungen werden als Dokumentgraph gespeichert.

---

# 114. Lebenszyklus

Ein Dokument kann folgende Zustände durchlaufen:

```text
Draft

InReview

Approved

Published

Superseded

Archived

Deleted
```

Zusätzliche domänenspezifische Zustände sind zulässig.

Der Lebenszykluszustand ist getrennt von Dateiformat und Dokumentversion.

---

# 115. Aufbewahrung

Dokumente können Aufbewahrungsrichtlinien besitzen.

Beispiele:

```text
Temporary

KeepUntilDate

KeepForDuration

Permanent

LegalHold

UserManaged
```

Aufbewahrungsrichtlinien können auf Dokumente, Versionen und Ressourcen angewendet werden.

---

# 116. Archivierung

Archivierte Dokumente müssen:

- lesbar bleiben,
- ihre Typinformationen behalten,
- Integritätsprüfungen ermöglichen,
- notwendige Ressourcen enthalten oder referenzieren,
- Formatmigrationen dokumentieren.

Für Langzeitarchivierung sollen offene und dokumentierte Formate bevorzugt werden.

---

# 117. Löschung

Dokumentlöschung kann folgende Formen besitzen:

```text
Logical Delete

Trash

Permanent Delete

Cryptographic Erasure

Retention-Protected
```

Bei referenzierten Ressourcen muss geprüft werden, ob diese noch von anderen Dokumenten benötigt werden.

---

# 118. Deduplizierung

Identische Dokumentressourcen können anhand ihrer Inhalts-Hashes dedupliziert werden.

Deduplizierung darf nicht:

- Dokumentidentitäten vermischen,
- Zugriffsrechte zusammenführen,
- Provenance verlieren,
- verschlüsselte Sicherheitsbereiche umgehen.

---

# 119. Versionsvergleich

Dokumentversionen können strukturell verglichen werden.

Mögliche Änderungen:

```text
Text Inserted

Text Deleted

Text Replaced

Node Added

Node Removed

Node Moved

Style Changed

Metadata Changed

Resource Changed
```

Der Vergleich soll semantische Struktur berücksichtigen und nicht ausschließlich Byteunterschiede anzeigen.

---

# 120. Zusammenführung

Mehrere Dokumentversionen können zusammengeführt werden.

Ein Merge enthält:

```text
Base Version

Left Version

Right Version

Resolved Changes

Conflicts

Merge Capability

Result Version
```

Nicht automatisch lösbare Konflikte müssen explizit erhalten bleiben.

---

# 121. Konflikttypen

Dokumentkonflikte können sein:

```text
Text Conflict

Structural Conflict

Style Conflict

Metadata Conflict

Resource Conflict

Deletion Conflict

Permission Conflict
```

Konflikte dürfen nicht stillschweigend durch zufällige Reihenfolge aufgelöst werden.

---

# 122. Capability Contracts

Dokumentverarbeitende Capabilities müssen deklarieren:

```text
Accepted Document Types

Accepted Versions

Required Structure

Required Features

Unsupported Features

Output Document Type

Conversion Quality

Resource Policy

Security Requirements
```

Beispiel:

```text
Input:

MarkdownDocument

Output:

HTMLDocument

Quality:

Semantically Lossless
```

---

# 123. Featuredeklaration

Dokumenttypen können unterstützte Features deklarieren.

Beispiele:

```text
Tables

Footnotes

Comments

TrackChanges

EmbeddedFonts

Macros

DigitalSignatures

Forms

Animations
```

Konverter vergleichen Quell- und Zielfeatures vor der Ausführung.

---

# 124. Featureverlust

Wenn das Zielformat ein Quellfeature nicht unterstützt, muss der Konverter:

- die Konvertierung ablehnen,
- eine Ersatzdarstellung erzeugen,
- das Feature entfernen,
- oder eine Warnung erzeugen.

Das Verhalten muss vorab deklarierbar sein.

---

# 125. Streaming

Große Dokumente können gestreamt verarbeitet werden.

Unterstützte Modelle:

```text
Byte Stream

Text Stream

Node Stream

Page Stream

Row Stream

Slide Stream
```

Streaming darf die Validierung von Dokumentgrenzen und Strukturen nicht umgehen.

---

# 126. Partielles Laden

Dokumente können teilweise geladen werden.

Beispiele:

```text
Specific Pages

Specific Chapters

Selected Worksheet

Metadata Only

Text Only

Resource On Demand
```

Ein partiell geladenes Dokument muss diesen Zustand explizit deklarieren.

---

# 127. Lazy Loading

Ressourcen und Dokumentteile können verzögert geladen werden.

Eine Lazy Reference enthält:

```text
Target

Expected Type

Integrity Information

Loading Policy

Failure Policy
```

Ein fehlgeschlagener Ladevorgang darf nicht als leeres Dokumentelement interpretiert werden.

---

# 128. Speicherrepräsentation

Dokumente können intern unterschiedliche Repräsentationen verwenden:

```text
Flat Text Buffer

Syntax Tree

Document Object Model

Persistent Tree

Page Object Graph

Chunked Object Store
```

Die konkrete In-Memory-Repräsentation ist von der logischen Dokumentsemantik getrennt.

---

# 129. Zero-Copy

Große Text- und Binärressourcen sollen Zero-Copy-Mechanismen unterstützen.

Beispiele:

```text
Memory-Mapped Document

Shared Text Buffer

Shared Image Resource

Immutable Resource Chunk
```

Bearbeitung erfolgt über:

- Copy-on-Write
- persistente Datenstrukturen
- neue Objektversionen

---

# 130. Caching

Die Runtime kann cachen:

```text
Parsed Structure

Rendered Pages

Thumbnails

Search Index

Embeddings

Font Layout

Converted Resources
```

Jeder Cacheeintrag ist an:

```text
Document ID

Version

Capability Version

Parameters
```

gebunden.

---

# 131. Fehlerbehandlung

Standardisierte Dokumentfehler umfassen:

```text
DOCUMENT_TYPE_UNKNOWN

DOCUMENT_FORMAT_UNKNOWN

DOCUMENT_FORMAT_MISMATCH

DOCUMENT_CORRUPTED

DOCUMENT_VERSION_UNSUPPORTED

DOCUMENT_STRUCTURE_INVALID

DOCUMENT_ENCODING_INVALID

DOCUMENT_RESOURCE_MISSING

DOCUMENT_REFERENCE_INVALID

DOCUMENT_SIGNATURE_INVALID

DOCUMENT_PERMISSION_DENIED

DOCUMENT_ACTIVE_CONTENT_BLOCKED

DOCUMENT_CONVERSION_LOSS

DOCUMENT_SCHEMA_VIOLATION

DOCUMENT_LIMIT_EXCEEDED
```

Fehler müssen den betroffenen Dokumentbereich möglichst genau referenzieren.

---

# 132. Ressourcenlimits

Die Runtime muss Schutzgrenzen unterstützen für:

```text
Maximum Document Size

Maximum Text Length

Maximum Node Count

Maximum Nesting Depth

Maximum Page Count

Maximum Embedded Resource Count

Maximum Decompressed Size

Maximum Formula Complexity

Maximum Macro Runtime
```

Limits verhindern:

- Speichererschöpfung
- Zip Bombs
- Parserangriffe
- Endlosschleifen
- übermäßige CPU-Nutzung

---

# 133. Parseranforderungen

Dokumentparser müssen:

- Eingaben als nicht vertrauenswürdig behandeln,
- Größenlimits erzwingen,
- Tiefenlimits erzwingen,
- ungültige Strukturen ablehnen,
- externe Ressourcen kontrollieren,
- aktive Inhalte isolieren,
- präzise Fehler melden,
- partielle Wiederherstellung kennzeichnen.

Ein Parser darf beschädigte Inhalte nicht stillschweigend als gültig deklarieren.

---

# 134. Wiederherstellung beschädigter Dokumente

Eine Recovery Capability kann versuchen, beschädigte Dokumente teilweise wiederherzustellen.

Das Ergebnis muss unterscheiden:

```text
Recovered Content

Missing Content

Modified Content

Unverified Content

Discarded Content
```

Das reparierte Dokument erhält eine neue Identität oder Version und vollständige Provenance.

---

# 135. Schema

Jeder veröffentlichte Dokumenttyp besitzt ein maschinenlesbares Schema.

Das Schema beschreibt:

```text
Type ID

Version

Base Document Type

Allowed Nodes

Required Nodes

Attributes

Resources

Features

Constraints

Serialization Formats
```

Das Schema wird in der Nova Type Registry registriert.

---

# 136. Reflection

Dokumenttypen unterstützen Reflection.

Abfragbar sind:

- Type ID
- Dokumentkategorie
- Version
- Strukturmodell
- unterstützte Knoten
- Features
- Metadaten
- Ressourcen
- Sicherheitsklassifikation
- Barrierefreiheitsinformationen
- Serialisierungen
- verfügbare Konverter

---

# 137. Type Registry

Die Nova Type Registry speichert für Dokumenttypen:

```text
Document Type ID

Name

Namespace

Version

Parent Type

Schema

Features

Media Types

Extensions

Converters

Validators

Renderers

Editors

Migration Paths
```

Dateiendungen dienen nur der Discovery und nicht als verbindliche Typidentität.

---

# 138. Discovery

Capability Discovery kann Dokumentverarbeitung anhand folgender Kriterien durchführen:

```text
Document Type

Required Feature

Output Type

Language

Security Policy

Conversion Quality

Accessibility Requirement

Performance Profile
```

Beispiel:

```text
Finde eine Capability, die MarkdownDocument
verlustarm in PDFDocument konvertiert.
```

---

# 139. YAML-Beispiel einer Dokumenttypdefinition

```yaml
documentType:
  id: nova.document.markdown
  name: MarkdownDocument
  namespace: nova.document
  version: 1.0.0
  baseType: nova.document.structured-text
  technicalRepresentation:
    - String
    - TextBuffer
  encoding:
    default: utf-8
  structure:
    root: DocumentRoot
    allowedNodes:
      - Heading
      - Paragraph
      - TextRun
      - Emphasis
      - Strong
      - Link
      - List
      - ListItem
      - Table
      - CodeBlock
      - Quote
  features:
    - headings
    - lists
    - tables
    - links
    - code-blocks
  serialization:
    mediaTypes:
      - text/markdown
    extensions:
      - md
      - markdown
  security:
    activeContent: false
```

---

# 140. YAML-Beispiel eines Dokumentobjekts

```yaml
document:
  documentId: 01K0NOVA7R4Y9C2D6M8P3F5H1J
  versionId: 01K0NOVA8A6T2G4Q7W9E1B3K5M
  type:
    technical: Object
    semantic: nova.document.markdown
    version: 1.0.0
  metadata:
    title: Nova Typed Dataflow Architecture
    language: de-DE
    created: 2026-07-28T13:00:00+02:00
  content:
    rootNode: node-root
  resources: []
  security:
    classification: internal
  provenance:
    enabled: true
```

---

# 141. YAML-Beispiel einer Dokumentstruktur

```yaml
nodes:
  - id: node-root
    type: DocumentRoot
    children:
      - node-heading
      - node-paragraph
  - id: node-heading
    type: Heading
    attributes:
      level: 1
    children:
      - node-heading-text
  - id: node-heading-text
    type: TextRun
    text: Einführung
  - id: node-paragraph
    type: Paragraph
    children:
      - node-paragraph-text
  - id: node-paragraph-text
    type: TextRun
    text: Dieses Dokument beschreibt die Nova-Architektur.
```

---

# 142. YAML-Beispiel einer Konvertierungs-Capability

```yaml
capability:
  id: nova.converter.markdown-to-html
  version: 1.0.0
  input:
    type: nova.document.markdown
  output:
    type: nova.document.html
  conversion:
    quality: semantically-lossless
    preserves:
      - headings
      - paragraphs
      - lists
      - tables
      - links
      - code-blocks
    mayChange:
      - visual-style
  security:
    scriptsGenerated: false
    externalResources: blocked
```

---

# 143. YAML-Beispiel eines Textbereichs

```yaml
textRange:
  documentId: 01K0NOVA7R4Y9C2D6M8P3F5H1J
  versionId: 01K0NOVA8A6T2G4Q7W9E1B3K5M
  positionModel: grapheme
  start:
    line: 10
    column: 4
  end:
    line: 10
    column: 19
```

---

# 144. Validierungsregeln

Ein Dokumenttyp gilt als gültig, wenn:

- eine eindeutige Type ID existiert,
- eine gültige Dokumentkategorie angegeben ist,
- ein Strukturmodell definiert ist,
- alle verwendeten Knotentypen registriert sind,
- Zeichenkodierungsregeln eindeutig sind,
- Dokumentfeatures maschinenlesbar beschrieben werden,
- Ressourcenbeziehungen typisiert sind,
- Sicherheitsregeln definiert sind,
- Versionierung unterstützt wird,
- mindestens eine Serialisierung oder interne Repräsentation definiert ist.

---

# 145. Dokumentinstanzvalidierung

Eine Dokumentinstanz gilt als gültig, wenn:

- der deklarierte Dokumenttyp registriert ist,
- die Dokumentstruktur dem Schema entspricht,
- alle Pflichtmetadaten vorhanden sind,
- alle Knoten gültige Typen besitzen,
- alle Node IDs eindeutig sind,
- alle internen Referenzen auflösbar sind,
- eingebettete Ressourcen gültig sind,
- Zeichenkodierung und Textinhalt gültig sind,
- Sicherheitsrichtlinien eingehalten werden,
- Größenlimits nicht überschritten sind.

---

# 146. Laufzeitanforderungen

Die Nova Runtime muss:

- Dokumenttypen dynamisch auflösen,
- strukturierte Dokumente validieren,
- Dokumentteile adressieren,
- Ressourcenreferenzen sicher verwalten,
- Versionsbeziehungen erhalten,
- partielle Dokumente kennzeichnen,
- Textpositionen eindeutig behandeln,
- Konvertierungen über Capabilities ausführen,
- Provenance automatisch fortschreiben,
- Sicherheitsklassifikationen erhalten,
- aktive Inhalte isolieren,
- Größen- und Ressourcenlimits erzwingen.

---

# 147. Compileranforderungen

Der Nova-Compiler muss für statisch bekannte Dokumenttypen:

- Typkompatibilität prüfen,
- Dokumentknoten validieren,
- Platzhaltertypen prüfen,
- ungültige Feld- und Knotenzugriffe erkennen,
- Capability Contracts prüfen,
- Konvertierungsanforderungen erkennen,
- verlustbehaftete Konvertierungen melden,
- dokumentbezogene Reflection unterstützen.

---

# 148. Editoranforderungen

Nova-Dokumenteditoren müssen:

- Dokumentversionen berücksichtigen,
- semantische Struktur erhalten,
- Änderungen nachvollziehbar speichern,
- Undo und Redo unterstützen,
- Textpositionen stabil behandeln,
- Sicherheitsklassifikationen respektieren,
- aktive Inhalte nicht unkontrolliert ausführen,
- barrierefreie Bearbeitung ermöglichen.

---

# 149. Testanforderungen

Jeder Dokumenttyp benötigt Tests für:

- Typidentität
- Schema
- Parsing
- Serialisierung
- Deserialisierung
- Zeichenkodierung
- Unicode
- Strukturvalidierung
- Ressourcenauflösung
- Versionskompatibilität
- Dokumentkonvertierung
- Sicherheitsgrenzen
- Barrierefreiheit
- beschädigte Eingaben
- Größenlimits
- partielle Dokumente
- Provenance

---

# 150. Konvertertests

Jede Dokumentkonvertierungs-Capability benötigt Tests für:

- vollständig unterstützte Features
- nicht unterstützte Features
- Verlustklassifikation
- Formatierungsänderungen
- Ressourcenübernahme
- Metadatenübernahme
- Barrierefreiheitsinformationen
- Sicherheitsmerkmale
- beschädigte Quelldokumente
- deterministische Ausgabe
- Round-Trip-Verhalten

---

# 151. Round-Trip-Tests

Sofern eine Rückkonvertierung unterstützt wird, muss geprüft werden:

```text
A → B → A
```

Der Testbericht unterscheidet:

```text
Identical

Semantically Equivalent

Visually Equivalent

Partially Equivalent

Not Equivalent
```

Eine erfolgreiche technische Rückkonvertierung bedeutet nicht automatisch vollständige Gleichheit.

---

# 152. Sicherheitsanforderungen

Dokumentverarbeitung muss:

- nicht vertrauenswürdige Eingaben isolieren,
- aktive Inhalte standardmäßig blockieren,
- externe Ressourcen kontrollieren,
- Parserlimits erzwingen,
- Dateityp-Spoofing erkennen,
- Pfadmanipulationen verhindern,
- Signaturen korrekt prüfen,
- vertrauliche Inhalte schützen,
- Metadatenlecks verhindern,
- Berechtigungen durchsetzen.

---

# 153. Datenschutzanforderungen

Dokumentverarbeitung muss:

- personenbezogene Inhalte klassifizieren können,
- Indexierung kontrollieren,
- KI-Verarbeitung lokal oder richtlinienkonform durchführen,
- Vorschauen schützen,
- sensible Metadaten berücksichtigen,
- Aufbewahrungs- und Löschregeln anwenden,
- Redaction nachvollziehbar ausführen,
- Provenance datensparsam gestalten.

---

# 154. Leistungsanforderungen

Implementierungen sollen:

- inkrementelles Parsing unterstützen,
- Lazy Loading verwenden,
- partielle Serialisierung ermöglichen,
- Text- und Ressourcen-Chunks teilen,
- Zero-Copy-Verarbeitung nutzen,
- Render- und Parse-Ergebnisse cachen,
- parallele Seitenverarbeitung ermöglichen,
- große Dokumente streamen,
- Suchindizes inkrementell aktualisieren.

---

# 155. Qualitätsanforderungen

Das Dokument- und Texttypmodell muss:

- stark typisiert
- Unicode-fähig
- semantisch strukturiert
- vollständig reflektierbar
- versionierbar
- konvertierbar
- sicher
- datenschutzfähig
- barrierefrei
- streamfähig
- Zero-Copy-fähig
- plattformunabhängig
- langfristig erweiterbar

sein.

---

# 156. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation, wenn:

- Text- und Dokumenttypen getrennt von Dateiformaten modelliert werden,
- Unicode als internes Textmodell unterstützt wird,
- strukturierte Dokumentknoten verfügbar sind,
- Dokumente stabile Identitäten und Versionen besitzen,
- Textbereiche und Dokumentfragmente adressierbar sind,
- Dokumentmetadaten typisiert werden,
- eingebettete Ressourcen sicher verwaltet werden,
- Konvertierungen ausschließlich über Capabilities erfolgen,
- Verlustklassifikationen unterstützt werden,
- Barrierefreiheitsinformationen erhalten bleiben,
- aktive Inhalte kontrolliert werden,
- Provenance und Sicherheitsklassifikationen unterstützt werden,
- Type Registry und Reflection verfügbar sind.

---

# 157. Zusammenfassung

Die **Dokument- und Texttypen** definieren das universelle Dokumentmodell der Nova Platform.

Dokumente werden nicht lediglich als Dateien, Zeichenketten oder undurchsichtige Bytefolgen behandelt. Sie werden als strukturierte, typisierte, versionierte und semantisch beschriebene Datenobjekte modelliert.

Das Modell umfasst einfachen Text, strukturierte Textdokumente, Rich Text, Seitendokumente, Tabellenkalkulationen, Präsentationen, Quelltexte, Konfigurationen, Notebooks, Formulare, Bücher, Scans und zusammengesetzte Dokumentpakete.

Durch die Trennung von Inhalt, Struktur, Semantik, Formatierung, Layout und Serialisierung können Dokumente formatunabhängig verarbeitet werden. Stabile Dokument- und Knotenidentitäten ermöglichen Versionierung, Annotationen, Änderungsverfolgung, kollaborative Bearbeitung und präzise Referenzen.

Explizite Konverter-Capabilities, Qualitätsklassifikationen und Konvertierungsberichte verhindern unbemerkten Informationsverlust. Sicherheits-, Datenschutz- und Barrierefreiheitsinformationen bleiben während des gesamten Dokumentlebenszyklus erhalten.

Damit bilden die Dokument- und Texttypen eine zentrale Grundlage für Editoren, Office-Anwendungen, Dokumentenverwaltung, semantische Suche, lokale KI, technische Dokumentation, digitale Archive und automatisierte Informationsverarbeitung innerhalb der Nova Platform.
```
````
