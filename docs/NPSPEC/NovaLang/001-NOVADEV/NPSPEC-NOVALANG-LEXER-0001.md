# NPSPEC-NOVALANG-LEXER-0001 – NovaLang Lexical Analysis

## Status

Angenommen

## Kategorie

NovaLang / Lexer

## Zweck

Diese Spezifikation definiert die lexikalische Analyse von NovaLang auf Grundlage der Sprachsyntax von Visual Basic .NET (VB.NET).

Der Lexer wandelt UTF-8-Quelltext in einen strukturierten Tokenstrom um und bildet die erste Verarbeitungsstufe für Parser, Compiler, Interpreter, NovaLang Studio und Logic Graph.

NovaLang verwendet einen gemeinsamen Lexer für `.nova`, `.nlf` und `.nui`.

Der Lexer arbeitet unabhängig von der .NET-Runtime, KI und Entwicklungswerkzeugen.

## Grundprinzipien

1. VB.NET-orientierte Tokenisierung.
2. Einheitlicher Lexer für alle NovaLang-Dateitypen.
3. Unicode-Unterstützung mit UTF-8 als Quelltextkodierung.
4. Schlüsselwörter und Bezeichner ohne Unterscheidung der Groß- und Kleinschreibung.
5. Zeilenorientierte Anweisungen mit logischen Zeilenabschlüssen.
6. Unterstützung expliziter und impliziter Zeilenfortsetzung.
7. Deterministische Erkennung von Operatoren und Literalen.
8. Vollständige Quelltextpositionen und rekonstruierbare Formatierung.
9. Inkrementelle Analyse für Entwicklungswerkzeuge.
10. Keine Ausführung oder semantische Interpretation während der Tokenisierung.

## Architektur

```text
UTF-8 Source
      ↓
Encoding Validation
      ↓
Unicode Decoder
      ↓
Lexical State Machine
      ↓
Token Recognition
      ↓
Trivia Collection
      ↓
Token Stream
      ↓
NovaLang Parser
```

Der Lexer besitzt keine Abhängigkeit von Typauflösung, Capability-Registry oder Runtime.

## Token-Modell

```text
Token
├── TokenKind
├── RawLexeme
├── CanonicalValue
├── LiteralValue
├── SourceRange
│   ├── StartByteOffset
│   ├── EndByteOffset
│   ├── StartLine
│   ├── StartColumn
│   ├── EndLine
│   └── EndColumn
├── LeadingTrivia
├── TrailingTrivia
├── Flags
└── Diagnostics[]
```

`RawLexeme` bewahrt die originale Schreibweise.

`CanonicalValue` dient beispielsweise dem Vergleich von Schlüsselwörtern und Bezeichnern.

`LiteralValue` enthält einen dekodierten Literalwert, sofern dieser ohne semantische Typauflösung bestimmt werden kann.

Byteoffsets beziehen sich auf den ursprünglichen UTF-8-Quelltext.

## Token-Kategorien

```text
Keyword
Identifier
EscapedIdentifier
IntegerLiteral
FloatingLiteral
DecimalLiteral
StringLiteral
CharacterLiteral
DateLiteral
BooleanLiteral
NothingLiteral
Operator
Punctuation
NewLine
EndOfFile
InvalidToken
```

Kommentare, Whitespace und Zeilenfortsetzungen werden als Trivia oder ergänzende lexikalische Informationen gespeichert.

## Schlüsselwörter

NovaLang erkennt die in der Sprachgrammatik definierten VB.NET-orientierten Schlüsselwörter.

Dazu gehören insbesondere:

```text
AddHandler
And
AndAlso
As
Async
Await
Boolean
ByRef
ByVal
Call
Case
Catch
Class
Const
Continue
Decimal
Delegate
Dim
Do
Double
Each
Else
ElseIf
End
Enum
Event
Exit
False
Finally
For
Friend
Function
Get
Handles
If
Implements
Imports
In
Inherits
Integer
Interface
Is
IsNot
Long
Loop
Me
Mod
Module
MustInherit
MustOverride
MyBase
MyClass
Namespace
New
Next
Not
Nothing
Of
Or
OrElse
Optional
Overrides
Overridable
Partial
Private
Property
Protected
Public
RaiseEvent
ReadOnly
RemoveHandler
Return
Select
Set
Shared
Short
Single
Static
Step
String
Structure
Sub
Then
Throw
To
True
Try
UInteger
ULong
UShort
Using
While
With
WriteOnly
Xor
```

Die vollständige Schlüsselworttabelle wird gemeinsam mit der Grammatikversion verwaltet.

## Groß- und Kleinschreibung

Schlüsselwörter und Bezeichner werden ohne Unterscheidung der Groß- und Kleinschreibung erkannt.

```vb
Dim Name As String = "NovaOS"

name = "Nova"

NAME = "NovaOS"
```

Alle drei Bezeichner beziehen sich auf dieselbe Identität.

Ebenso sind folgende Schlüsselwörter lexikalisch gleichwertig:

```vb
If
IF
if
iF
```

Die ursprüngliche Schreibweise bleibt erhalten.

Die kanonische Vergleichsform muss anhand einer versionierten Unicode-Regel bestimmt werden.

Eine Änderung der Unicode-Version darf nicht unbemerkt die Identität vorhandener Bezeichner verändern.

## Unicode-Bezeichner

NovaLang unterstützt Unicode-Bezeichner.

```vb
Dim größe As Integer = 100
Dim temperatur As Double = 21.5
Dim ΔWert As Double = 0.5
```

Die zulässigen Unicode-Zeichenklassen werden durch die Sprachversion definiert.

Es gelten folgende Regeln:

- Bezeichner müssen mit einem zulässigen Startzeichen beginnen.
- Nachfolgende Zeichen müssen der definierten Identifier-Grammatik entsprechen.
- Unicode-Normalisierung darf den ursprünglichen Quelltext nicht verändern.
- Kanonisch äquivalente Schreibweisen werden nach den festgelegten Vergleichsregeln behandelt.
- Unsichtbare Steuerzeichen und gefährliche Unicode-Sequenzen müssen erkannt werden.
- Visuell verwechselbare Bezeichner müssen für Sicherheitsdiagnosen identifizierbar bleiben.

Die Bezeichnerregeln sind unabhängig von den Vergleichsregeln für CapabilityIDs, ObjectIDs und andere Systemidentitäten.

## Escaped Identifier

Reservierte Schlüsselwörter können nach VB.NET-Vorbild in eckigen Klammern als Bezeichner verwendet werden.

```vb
Dim [Class] As String = "Test"
Dim [Function] As Integer = 10
```

Der Lexer erzeugt hierfür ein `EscapedIdentifier`-Token.

Die eckigen Klammern gehören zur lexikalischen Darstellung, nicht zur eigentlichen Bezeichneridentität.

```text
[Class] → Identifier: Class
```

Die genaue Menge zulässiger Zeichen innerhalb eines Escaped Identifier wird durch die Grammatikversion bestimmt.

## Zeilenorientierte Tokenisierung

NovaLang verwendet logische Zeilenabschlüsse.

```vb
Dim a As Integer = 10
Dim b As Integer = 20

Dim ergebnis As Integer = a + b
```

Der Lexer unterscheidet:

```text
PhysicalNewLine
LogicalNewLine
LineContinuation
```

Ein physischer Zeilenumbruch erzeugt nur dann ein `NewLine`-Token, wenn die aktuelle Anweisung dadurch syntaktisch abgeschlossen werden kann.

Die Entscheidung über implizite Fortsetzungen basiert ausschließlich auf lexikalisch und grammatikalisch definierten Regeln.

## Explizite Zeilenfortsetzung

NovaLang unterstützt `_` als explizites Fortsetzungszeichen.

```vb
Dim ergebnis As Integer = 10 + _
                          20 + _
                          30
```

Das Fortsetzungszeichen muss an einer zulässigen Position stehen.

Der folgende physische Zeilenumbruch wird nicht als logischer Anweisungsabschluss ausgegeben.

Ungültige Fortsetzungen müssen diagnostiziert werden.

## Implizite Zeilenfortsetzung

NovaLang unterstützt implizite Fortsetzungen an eindeutig definierten Stellen.

Beispiel:

```vb
Dim ergebnis As Integer = 10 +
                          20 +
                          30
```

Weitere zulässige Kontexte werden durch die Grammatik definiert.

Der Lexer darf keine beliebigen Zeilenumbrüche ignorieren.

## Anweisungstrennung

Ein Doppelpunkt kann mehrere Anweisungen innerhalb einer logischen Zeile trennen.

```vb
Dim a As Integer = 10 : Dim b As Integer = 20
```

Der Lexer erzeugt ein eigenes `Colon`-Token.

Die Zulässigkeit der Trennung im jeweiligen Kontext wird vom Parser geprüft.

## Whitespace

Unterstützte Whitespace-Formen umfassen:

```text
Space
Tab
LF
CRLF
```

Weitere Unicode-Whitespace-Zeichen werden entsprechend den versionierten Lexikalregeln behandelt.

Whitespace innerhalb von Zeichenketten wird unverändert als Bestandteil des Literals verarbeitet.

Einrückungen besitzen keine eigene Blocksemantik.

## Kommentare

NovaLang verwendet den VB.NET-Kommentarstil.

```vb
' Dies ist ein Kommentar

Dim zahl As Integer = 10 ' Kommentar hinter einer Anweisung
```

Der Kommentar beginnt mit `'` außerhalb von Zeichenketten und endet am physischen Zeilenende.

Kommentare werden als Trivia gespeichert.

C-artige Kommentare mit `//` oder `/* ... */` gehören nicht zur regulären NovaLang-Syntax.

## Dokumentationskommentare

NovaLang unterstützt XML-Dokumentationskommentare nach VB.NET-Vorbild.

```vb
''' <summary>
''' Addiert zwei Zahlen.
''' </summary>
''' <param name="a">Erste Zahl</param>
''' <param name="b">Zweite Zahl</param>
''' <returns>Die Summe.</returns>
Public Function Addieren(a As Integer, b As Integer) As Integer
    Return a + b
End Function
```

Der Lexer erkennt `'''` als Dokumentationskommentar.

Die XML-Struktur wird durch eine nachgelagerte Dokumentationsanalyse verarbeitet.

## Ganzzahlliterale

NovaLang unterstützt dezimale Ganzzahlliterale.

```vb
Dim a As Integer = 42
Dim b As Long = 1000000
```

Zusätzlich werden VB.NET-orientierte Zahlensystempräfixe unterstützt.

```vb
Dim hexWert As Integer = &HFF
Dim binaer As Integer = &B101010
Dim oktal As Integer = &O77
```

Der Lexer erkennt die Zahlensystembasis und die lexikalische Darstellung.

Wertebereich und Zieltyp werden anschließend geprüft.

## Gleitkommaliterale

```vb
Dim temperatur As Double = 21.5
Dim faktor As Double = 1.25E3
Dim wert As Single = 0.75F
```

Der Lexer erkennt:

```text
Decimal Digits
Fractional Part
Exponent
Type Suffix
```

Das Dezimaltrennzeichen im Quelltext ist unabhängig von der Systemsprache immer `.`.

## Dezimalliterale

```vb
Dim preis As Decimal = 19.95D
```

Dezimal- und Gleitkommaliterale müssen eindeutig unterschieden werden.

Die exakte numerische Repräsentation wird durch das NovaLang-Typsystem definiert.

## Numerische Suffixe

NovaLang unterstützt versionierte VB.NET-orientierte Typsuffixe.

Dazu gehören insbesondere:

```text
S
US
I
UI
L
UL
F
R
D
```

Der Lexer muss zwischen Literalsuffixen und nachfolgenden Bezeichnern unterscheiden.

Ungültige Kombinationen müssen diagnostiziert werden.

## Negative Zahlen

Das Minuszeichen ist ein eigenständiger Operator.

```vb
Dim zahl As Integer = -42
```

Tokenisierung:

```text
Keyword(Dim)
Identifier(zahl)
Keyword(As)
Keyword(Integer)
Operator(=)
Operator(-)
IntegerLiteral(42)
```

Die Negation wird vom Parser verarbeitet.

## Zeichenkettenliterale

NovaLang verwendet doppelte Anführungszeichen.

```vb
Dim name As String = "NovaOS"
Dim text As String = "Hallo Welt"
```

Ein Anführungszeichen innerhalb einer Zeichenkette wird durch Verdopplung dargestellt.

```vb
Dim text As String = "Er sagte ""Hallo""."
```

C-artige Escape-Sequenzen wie `\n` und `\t` sind nicht Bestandteil der regulären VB.NET-orientierten Stringsyntax.

Zeilenumbrüche werden beispielsweise über definierte Konstanten oder Stringfunktionen erzeugt.

```vb
Dim text As String = "Zeile 1" & vbCrLf & "Zeile 2"
```

`vbCrLf` kann durch eine kompatible NovaLang-Standardbibliothek bereitgestellt werden.

## Zeichenliterale

NovaLang verwendet den VB.NET-orientierten Zeichensuffix `c`.

```vb
Dim buchstabe As Char = "A"c
```

Ein `Char` repräsentiert einen UTF-16-Codeunit entsprechend dem VB.NET-kompatiblen Sprachmodell.

Unicode-Skalarwerte und Grapheme werden durch gesonderte Unicode-Typen beziehungsweise Stringoperationen verarbeitet.

Der Lexer erkennt das Zeichenliteral, während die semantische Analyse die zulässige Länge und Repräsentation prüft.

## Boolean-Literale

```vb
Dim aktiviert As Boolean = True
Dim beendet As Boolean = False
```

`True` und `False` werden unabhängig von der Groß- und Kleinschreibung erkannt.

## Nothing-Literal

NovaLang verwendet `Nothing` als VB.NET-orientiertes Literal.

```vb
Dim person As Person = Nothing
```

`Nothing` ist kein universeller untypisierter Speicherzeiger.

Die Bedeutung hängt vom Zieltyp und den Nullable-Regeln ab.

Speicher- und Typsicherheit müssen erhalten bleiben.

## Datumsliterale

NovaLang kann VB.NET-orientierte Datumsliterale unterstützen.

```vb
Dim datum As Date = #10/9/2026#
```

Die lexikalische Erkennung erfolgt unabhängig von den regionalen Einstellungen des Systems.

Die zulässigen Formate und ihre semantische Interpretation müssen separat eindeutig spezifiziert werden.

Mehrdeutige Datumsangaben dürfen nicht von der aktuellen Systemsprache abhängen.

## Operatoren

Der Lexer erkennt mindestens folgende Operatoren:

```text
+
-
*
/
\
^
&
=
<>
<
<=
>
>=
+=
-=
*=
/=
\=
^=
&=
<<
>>
<<=
>>=
```

Zusätzlich werden Schlüsselwortoperatoren erkannt:

```text
And
AndAlso
Or
OrElse
Not
Xor
Mod
Is
IsNot
Like
```

Die Operatorpräzedenz wird vom Parser beziehungsweise der Grammatik definiert.

## Longest-Match-Prinzip

Bei mehreren möglichen Operatoren wird grundsätzlich die längste gültige Tokenfolge erkannt.

```text
>=  vor >
<=  vor <
<>  vor <
+=  vor +
<<= vor <<
```

Das Verfahren muss deterministisch sein.

## Satzzeichen

```text
(
)
[
]
{
}
,
.
:
;
?
```

Die Bedeutung eines Satzzeichens ergibt sich aus dem syntaktischen Kontext.

Geschweifte Klammern dürfen beispielsweise für Initialisierer verwendet werden, jedoch nicht als allgemeine Blockbegrenzer.

## Lexer-Zustände

```text
Normal
StringLiteral
CharacterLiteral
Comment
DocumentationComment
EscapedIdentifier
NumericLiteral
ExplicitContinuation
ErrorRecovery
```

Jeder Zustand besitzt definierte Eintritts- und Austrittsbedingungen.

Der Lexer muss bei fehlerhaftem Quelltext kontrolliert in einen gültigen Analysezustand zurückkehren können.

## Tokenisierungsbeispiel

Quelltext:

```vb
Dim ergebnis As Integer = 10 + 20
```

Tokenstrom:

```text
Keyword(Dim)
Identifier(ergebnis)
Keyword(As)
Keyword(Integer)
Operator(=)
IntegerLiteral(10)
Operator(+)
IntegerLiteral(20)
NewLine
EndOfFile
```

Whitespace wird über Trivia und SourceRanges erhalten.

## Blockerkennung

Der Lexer erkennt Schlüsselwörter wie:

```text
If
Then
Else
End
Function
Sub
Class
Structure
Interface
Namespace
```

Er entscheidet jedoch nicht, ob ein Block korrekt geschlossen wurde.

Beispiel:

```vb
If aktiviert Then
    Starten()
End If
```

Die Zuordnung von `If` zu `End If` erfolgt durch den Parser.

## Inkrementelle Tokenisierung

NovaLang Studio muss Quelltextänderungen effizient verarbeiten können.

```text
Source Edit
     ↓
Affected Source Range
     ↓
Lexer State Recovery
     ↓
Partial Re-Lexing
     ↓
Token Stream Update
     ↓
Incremental Parsing
```

Unveränderte Quelltextbereiche sollen nicht unnötig erneut analysiert werden.

Eine inkrementelle Analyse muss denselben Tokenstrom erzeugen wie eine vollständige Analyse desselben Quelltexts.

## Fehlerdiagnosen

Der Lexer muss mindestens folgende Fehler erkennen:

- Ungültige UTF-8-Sequenzen.
- Unzulässige Unicode-Zeichen.
- Ungültige Bezeichner.
- Nicht geschlossene Zeichenketten.
- Nicht geschlossene Escaped Identifier.
- Ungültige Zahlenliterale.
- Ungültige numerische Suffixe.
- Fehlerhafte explizite Zeilenfortsetzungen.
- Unbekannte Operatorzeichen.
- Überschrittene Ressourcenlimits.

Jede Diagnose enthält:

```text
Diagnostic
├── ErrorCode
├── Severity
├── Message
├── SourceRange
├── LexerState
└── RecoveryInformation
```

Beispiel:

```text
NOVA-LEXER-0011

Nicht geschlossenes Zeichenkettenliteral.

Datei: Main.nova
Zeile: 14
Spalte: 23
```

## Sicherheit

Der Lexer darf keine Quelltextanweisungen ausführen.

Er muss gegen fehlerhafte und absichtlich manipulierte Eingaben abgesichert sein.

Ressourcenlimits umfassen:

```text
MaximumSourceSize
MaximumTokenLength
MaximumTokenCount
MaximumIdentifierLength
MaximumDiagnosticCount
MaximumProcessingBudget
MaximumMemoryBudget
```

Unicode-Bidi-Steuerzeichen, Confusables und andere potenziell irreführende Zeichen müssen für Sicherheitsprüfungen erkennbar bleiben.

## Determinismus

Bei identischem Quelltext und identischer Sprachversion muss der Lexer denselben Tokenstrom erzeugen.

Die Tokenisierung darf nicht abhängen von:

- Systemsprache.
- Regionaleinstellungen.
- Zeitzone.
- Benutzeroberfläche.
- KI-Modellen.
- Verfügbarem Netzwerk.
- Zufälligen Laufzeitentscheidungen.

## Introspection

Der Lexer stellt mindestens folgende Informationen bereit:

```text
LexerVersion
LanguageVersion
UnicodeVersion
TokenCount
DiagnosticCount
ProcessingDuration
IncrementalAnalysisState
ResourceUsage
```

Diagnose- und Analyseinformationen müssen über autorisierte Entwicklungswerkzeuge abrufbar sein.

## Normative Anforderungen

1. NovaLang MUSS einen gemeinsamen Lexer für `.nova`, `.nlf` und `.nui` verwenden.
2. Der Lexer MUSS UTF-8 als Quelltextkodierung unterstützen.
3. Schlüsselwörter und Bezeichner MÜSSEN ohne Unterscheidung der Groß- und Kleinschreibung erkannt werden.
4. Unicode-Bezeichner MÜSSEN unterstützt werden.
5. Die ursprüngliche Schreibweise aller Token MUSS erhalten bleiben.
6. Logische Zeilenabschlüsse MÜSSEN erkannt werden.
7. Explizite und definierte implizite Zeilenfortsetzungen MÜSSEN unterstützt werden.
8. VB.NET-orientierte Kommentare mit `'` MÜSSEN unterstützt werden.
9. XML-Dokumentationskommentare MÜSSEN erkennbar sein.
10. Numerische Literale MÜSSEN unabhängig von Regionaleinstellungen tokenisiert werden.
11. Zeichenketten MÜSSEN die VB.NET-orientierte Anführungszeichen-Escaping-Syntax verwenden.
12. Operatoren MÜSSEN deterministisch erkannt werden.
13. Das Longest-Match-Prinzip MUSS angewendet werden.
14. Token MÜSSEN präzise Quelltextpositionen besitzen.
15. Kommentare und Whitespace MÜSSEN rekonstruierbar bleiben.
16. Der Lexer DARF keine semantische Typprüfung durchführen.
17. Der Lexer DARF keine Capability-Berechtigungen anfordern.
18. Der Lexer DARF keinen Quellcode ausführen.
19. Fehlerhafte Eingaben MÜSSEN kontrolliert behandelbar sein.
20. Inkrementelle und vollständige Tokenisierung MÜSSEN äquivalente Ergebnisse liefern.
21. Die Tokenisierung MUSS unabhängig von der .NET-Runtime erfolgen.
22. Lexer-Version, Zustände und Diagnosen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NOVALANG-CORE-0001`
- `NPSPEC-NOVALANG-SYNTAX-0001`
- `NPSPEC-NOVALANG-GRAMMAR-0001`
- `NPSPEC-NOVALANG-TYPES-0001`
- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-UTF8-0001`
- `NPSPEC-TEXT-NORMALIZATION-0001`
- `NPSPEC-TEXT-SECURITY-0001`

## Ergebnis

NovaLang besitzt einen einheitlichen, Unicode-fähigen und deterministischen Lexer, dessen Tokenisierung sich möglichst eng an VB.NET orientiert.

Der Lexer unterstützt zeilenorientierte Syntax, VB.NET-Schlüsselwörter, Kommentare, Literale, Operatoren und die gemeinsame Sprachgrundlage für Programme, Solutions, Logic Graph und deklarative Benutzeroberflächen.

Die lexikalische Analyse bleibt unabhängig von .NET, KI und der NovaOS-Runtime und bildet die verbindliche Grundlage für alle NovaLang-Parserimplementierungen.