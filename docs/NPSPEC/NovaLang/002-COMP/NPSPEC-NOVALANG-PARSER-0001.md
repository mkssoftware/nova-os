
# NPSPEC-NOVALANG-PARSER-0001 – NovaLang Parser

## Status

Angenommen

## Kategorie

NovaLang / Parser

## Zweck

Definiert die syntaktische Analyse von NovaLang-Quellcode und die Erstellung eines Abstract Syntax Tree (AST). Der Parser verwendet die formale NovaLang-Grammatik mit möglichst VB.NET-kompatibler Syntax.

## Eingabe und Ausgabe

| Element | Beschreibung |
|---|---|
| Eingabe | Tokenstrom des NovaLang-Lexers |
| Ausgabe | Abstract Syntax Tree (AST) |
| Diagnostik | Syntaxfehler, Positionen, Korrekturhinweise |
| Quellformate | `.nova`, `.nlf`, `.nui` |

Alle Quellformate verwenden denselben Sprachparser. Formatabhängige Einschränkungen werden durch den jeweiligen Kontext geprüft.

## Parserarchitektur

Der Parser muss folgende Konstruktionen erkennen:

- Deklarationen, Namensräume und Module
- Klassen, Strukturen, Interfaces und Generics
- Variablen, Funktionen und Eigenschaften
- Ausdrücke, Operatoren und Zuweisungen
- Kontrollstrukturen und Pattern Matching
- Async/Await, Events und Exception-Behandlung
- Attribute und deklarative UI-Konstruktionen

Die Implementierung darf Recursive Descent, Pratt Parsing oder gleichwertige Verfahren kombinieren.

## Abstract Syntax Tree

Der AST bildet die syntaktische Struktur des Quellcodes ab.

Jeder Knoten besitzt mindestens:

- Knotentyp
- Quelltextposition und Textbereich
- Zugehörige Unterknoten
- Erforderliche syntaktische Informationen

Semantische Typinformationen werden in der nachfolgenden Analyse ergänzt oder separat zugeordnet.

## Fehlerbehandlung

- Syntaxfehler müssen mit genauer Quelltextposition gemeldet werden.
- Der Parser muss nach Fehlern möglichst weiterarbeiten können.
- Unvollständiger Quellcode darf für NovaLang Studio analysierbar bleiben.
- Fehlerhafte Konstruktionen dürfen nicht als gültiger AST ohne Diagnose ausgegeben werden.
- Fehlerkorrekturen dürfen die ursprüngliche Quelltextbedeutung nicht stillschweigend verändern.

## Inkrementelles Parsing

Der Parser soll bei Quelltextänderungen nur betroffene Bereiche erneut analysieren.

Unveränderte Syntaxstrukturen dürfen wiederverwendet werden.

Dies ermöglicht schnelle Diagnostik, Syntaxhervorhebung und Codevervollständigung in NovaLang Studio.

## Normative Anforderungen

1. Der Parser MUSS die versionierte NovaLang-Grammatik implementieren.
2. Der Parser MUSS einen strukturierten AST erzeugen.
3. Alle Sprachkonstruktionen MÜSSEN eindeutig syntaktisch analysierbar sein.
4. Syntaxfehler MÜSSEN strukturiert und positionsgenau diagnostiziert werden.
5. Fehlerbehandlung MUSS eine kontrollierte Fortsetzung der Analyse ermöglichen.
6. Der Parser MUSS unvollständigen Quellcode für Entwicklungswerkzeuge unterstützen.
7. Inkrementelles Parsing SOLL unterstützt werden.
8. `.nova`, `.nlf` und `.nui` MÜSSEN denselben Sprachparser verwenden.

## Ergebnis

NovaLang erhält einen modularen, fehlertoleranten und für inkrementelle Verarbeitung geeigneten Parser, der eine einheitliche syntaktische Grundlage für Compiler und NovaLang Studio bereitstellt.
