
# NPSPEC-STUDIO-SYNTAX-HIGHLIGHTING-0001 – NovaLang Studio Syntax Highlighting

## Status

Angenommen

## Kategorie

NovaLang Studio / Code Editor / Syntax Highlighting

## Zweck

Definiert die Syntaxhervorhebung von NovaLang Studio für Quellcode, deklarative Benutzeroberflächen und Logic-Graph-Skripte.

Ziel ist eine schnelle, präzise und übersichtliche Darstellung von Code, die dessen Struktur unmittelbar erkennbar macht, ohne die Lesbarkeit oder Performance zu beeinträchtigen.

## Architektur

| Komponente | Aufgabe |
|---|---|
| Syntax Tokenizer | Lexikalische Klassifizierung |
| Semantic Highlighter | Semantische Symbolklassifizierung |
| Theme Resolver | Zuordnung von Farben und Textstilen |
| Highlight Renderer | Darstellung im Editor |
| Incremental Highlighter | Aktualisierung geänderter Bereiche |

Die Syntaxhervorhebung verwendet die gemeinsame NovaLang-Sprachdefinition und die Analyseinformationen des Language Service.

## Unterstützte Dateitypen

| Dateityp | Hervorhebung |
|---|---|
| `.nova` | Vollständige NovaLang-Syntax |
| `.nlf` | NovaLang-Syntax für Logic-Graph-Skripte |
| `.nui` | Deklarative NovaLang-Syntax und UI-Bindungen |
| `.xml` | XML-Elemente, Attribute und Werte |
| `.md` | Markdown-Strukturen und eingebetteter Code |

`.nova`, `.nlf` und `.nui` verwenden dieselben grundlegenden Token- und Typklassifizierungen.

## Token-Kategorien

Mindestens folgende Kategorien werden unterschieden:

- Schlüsselwörter
- Datentypen
- Klassen und Strukturen
- Interfaces und Module
- Funktionen und Methoden
- Eigenschaften und Felder
- Variablen und Parameter
- Konstanten
- Zeichenketten und Zeichenliterale
- Zahlen und boolesche Literale
- Operatoren
- Kommentare und Dokumentationskommentare
- Attribute und Annotationen
- Namespaces
- Fehlerhafte Syntax

Die Kategorien müssen unabhängig vom verwendeten Farbschema definiert sein.

## Lexikalische Hervorhebung

Der Syntax Tokenizer erkennt Sprachbestandteile ohne vollständige semantische Analyse.

Beispiel:

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer
    Dim ergebnis As Integer = a + b
    Return ergebnis
End Function
```

Schlüsselwörter wie `Public`, `Function`, `As`, `Dim`, `Return` und `End` werden unmittelbar erkannt.

Die Groß- und Kleinschreibung darf die Klassifizierung von NovaLang-Schlüsselwörtern nicht beeinflussen.

## Semantische Hervorhebung

Der Language Service ergänzt lexikalische Informationen durch aufgelöste Symboltypen.

Dadurch können beispielsweise unterschieden werden:

- Lokale Variable und Klassenfeld
- Typname und Namespace
- Methode und Eigenschaft
- Parameter und Konstante
- Deklaration und Referenz

Semantische Informationen haben bei gültiger und aktueller Analyse Vorrang vor rein lexikalischen Klassifizierungen.

## Inkrementelle Verarbeitung

Die Hervorhebung wird bei Änderungen nur für betroffene Bereiche neu berechnet.

- Unveränderte Tokeninformationen werden wiederverwendet.
- Abhängige Syntaxbereiche werden bei Bedarf aktualisiert.
- Veraltete semantische Ergebnisse dürfen nicht als aktuelle Analyse dargestellt werden.
- Die lexikalische Hervorhebung bleibt während laufender Hintergrundanalysen verfügbar.

## Fehlerdarstellung

Syntaxfehler werden unabhängig von der normalen Tokenfarbe gekennzeichnet.

Die Darstellung kann enthalten:

- Dezente Unterstreichungen
- Fehlermarkierungen
- Diagnosehinweise
- Fehlercodes und Beschreibungen

Fehlerkennzeichnungen dürfen den eigentlichen Quelltext nicht verdecken.

## Themes

Die Darstellung verwendet das NovaOS-Themensystem.

Unterstützt werden:

- Dark Mode
- Light Mode
- Benutzerdefinierte Farbschemata
- Kontrastreiche Darstellung
- Anpassbare Schriftstile

Syntaxfarben müssen ausreichend unterscheidbar sein und dürfen nicht ausschließlich über Farbunterschiede wichtige Fehlerzustände vermitteln.

## Logic-Graph-Integration

Custom Scripts innerhalb eines Logic Graph verwenden dieselbe NovaLang-Hervorhebung wie reguläre `.nova`-Dateien.

Capability-Referenzen und bereitgestellte Ein- und Ausgänge können zusätzlich semantisch gekennzeichnet werden.

Die Hervorhebung darf keine Capability-Berechtigungen erzeugen oder verändern.

## UI-Designer-Integration

Bei `.nui`-Dateien werden zusätzlich erkannt:

- UI-Typen und Komponenten
- Eigenschaften
- Datenbindungen
- Ereignisbindungen
- Deklarative Ausdrücke

Die Klassifizierung muss mit den tatsächlichen NovaLang-Typinformationen und UI-Verträgen übereinstimmen.

## Performance

- Die Hervorhebung MUSS inkrementell arbeiten.
- Sichtbare Editorbereiche werden bevorzugt verarbeitet.
- Semantische Analysen dürfen die Texteingabe nicht blockieren.
- Token- und Symbolinformationen sollen zwischengespeichert werden.
- Speicherverbrauch und Hintergrundaufgaben müssen begrenzbar sein.
- Große Dateien müssen ohne vollständige Neuanalyse bei jeder Eingabe bearbeitbar bleiben.

## Normative Anforderungen

1. NovaLang Studio MUSS lexikalische und semantische Syntaxhervorhebung unterstützen.
2. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben grundlegenden Sprachklassifizierungen verwenden.
3. Die Hervorhebung MUSS auf der gemeinsamen NovaLang-Sprachdefinition basieren.
4. Schlüsselwörter, Typen, Symbole, Literale und Kommentare MÜSSEN unterscheidbar sein.
5. Änderungen MÜSSEN inkrementell verarbeitet werden.
6. Semantische Analysen DÜRFEN die Texteingabe nicht blockieren.
7. Syntaxfehler MÜSSEN unabhängig vom normalen Farbschema erkennbar sein.
8. Dark Mode, Light Mode und benutzerdefinierte Themes MÜSSEN unterstützt werden.
9. Die Darstellung MUSS barrierearme und kontrastreiche Konfigurationen ermöglichen.
10. Logic Graph und UI Designer MÜSSEN dieselbe Klassifizierungsinfrastruktur verwenden können.
11. Die Hervorhebung DARF keine Capability- oder Sicherheitsgrenzen umgehen.
12. Die gesamte Syntaxhervorhebung MUSS ohne KI funktionieren.

## Ergebnis

NovaLang Studio erhält eine schnelle, inkrementelle und semantisch präzise Syntaxhervorhebung für NovaLang, Logic Graph und deklarative Benutzeroberflächen.

Die Darstellung bleibt einheitlich, anpassbar und ressourcenschonend.
