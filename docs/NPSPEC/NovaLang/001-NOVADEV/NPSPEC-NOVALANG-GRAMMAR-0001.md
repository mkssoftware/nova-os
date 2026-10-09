# NPSPEC-NOVALANG-GRAMMAR-0001 – NovaLang Formal Grammar

## Status

Angenommen

## Kategorie

NovaLang / Grammatik

## Zweck

Diese Spezifikation definiert die formale Grammatik von NovaLang auf Grundlage der Sprachsyntax von Visual Basic .NET (VB.NET).

Sie bildet die verbindliche Grundlage für Lexer, Parser, Compiler, Interpreter, NovaLang Studio und Logic Graph.

NovaLang verwendet eine gut lesbare, zeilenorientierte Syntax mit ausgeschriebenen Schlüsselwörtern und expliziten Blockabschlüssen.

Die Grammatik gilt einheitlich für `.nova`, `.nlf` und `.nui`.

NovaLang ist eine eigenständige Sprache und benötigt keine .NET-Runtime.

## Grundprinzipien

1. Die Syntax orientiert sich möglichst exakt an VB.NET.
2. Anweisungen werden grundsätzlich durch Zeilenumbrüche getrennt.
3. Mehrzeilige Blöcke besitzen explizite Abschlüsse wie `End If`, `End Sub` und `End Class`.
4. Bezeichner unterscheiden nicht zwischen Groß- und Kleinschreibung.
5. Typdeklarationen verwenden `As`.
6. Generische Parameter verwenden `Of`.
7. Kommentare beginnen mit `'`.
8. Ausdrücke, Typen und Deklarationen folgen einer gemeinsamen Grammatik.
9. Deklarative Benutzeroberflächen verwenden dieselbe NovaLang-Syntax.
10. Syntaxanalyse erfolgt unabhängig von Laufzeit, KI und Systemberechtigungen.

## Grammatiknotation

Die formale Grammatik verwendet EBNF.

```ebnf
rule = expression ;

A | B       (* Alternative *)
[ A ]       (* Optional *)
{ A }       (* Wiederholung *)
( A )       (* Gruppierung *)
"A"         (* Terminal *)
```

Zusätzliche lexikalische Symbole:

```text
IDENTIFIER      Bezeichner
INTEGER         Ganzzahlliteral
FLOAT           Gleitkommaliteral
DECIMAL         Dezimalliteral
STRING          Zeichenkettenliteral
CHARACTER       Zeichenliteral
NEWLINE         Logischer Zeilenabschluss
EOF             Dateiende
```

Die EBNF beschreibt die syntaktische Struktur. Kontextabhängige Regeln wie Typauflösung, Sichtbarkeit und Berechtigungen werden separat geprüft.

## Programmeinheit

```ebnf
compilationUnit =
    { optionStatement }
    { importsStatement }
    { declaration | statement }
    EOF ;

declaration =
    namespaceDeclaration
  | moduleDeclaration
  | classDeclaration
  | structureDeclaration
  | interfaceDeclaration
  | enumDeclaration
  | delegateDeclaration
  | functionDeclaration
  | subDeclaration
  | propertyDeclaration
  | eventDeclaration
  | variableDeclaration
  | constantDeclaration ;
```

Nicht jede Deklaration ist in jedem Gültigkeitsbereich zulässig. Die entsprechenden Kontextregeln werden semantisch geprüft.

## Zeilenstruktur

```ebnf
statementTerminator =
    NEWLINE
  | ":" ;

statementList =
    statement
    { statementTerminator statement } ;
```

Ein logischer Zeilenabschluss beendet eine Anweisung, sofern die Grammatik keine Fortsetzung erwartet.

Explizite Zeilenfortsetzung erfolgt wie in VB.NET mit `_`.

```vb
Dim ergebnis As Integer = 10 + _
                          20 + _
                          30
```

Eine implizite Fortsetzung ist nur an grammatikalisch eindeutig definierten Stellen zulässig.

Zeilenumbrüche innerhalb von Zeichenkettenliteralen unterliegen gesonderten Literalregeln.

## Kommentare

```ebnf
comment =
    "'" { commentCharacter } ;
```

Beispiel:

```vb
' Dies ist ein Kommentar

Dim zahl As Integer = 10 ' Kommentar hinter einer Anweisung
```

Kommentare beeinflussen die Programmausführung nicht.

XML-Dokumentationskommentare mit `'''` können als gesonderte Erweiterung definiert werden.

## Bezeichner

```ebnf
identifier =
    IDENTIFIER ;

qualifiedName =
    identifier { "." identifier } ;
```

Bezeichner unterstützen Unicode.

Groß- und Kleinschreibung beeinflussen ihre Identität nicht.

```vb
Dim MeineVariable As Integer = 10

meinevariable = 20
MEINEVARIABLE = 30
```

Alle drei Schreibweisen beziehen sich auf dieselbe Variable.

Die ursprüngliche Schreibweise bleibt für Quelltextdarstellung und Diagnosen erhalten.

Reservierte Schlüsselwörter können entsprechend den definierten VB.NET-Regeln durch eckige Klammern als Bezeichner verwendet werden.

```vb
Dim [Class] As String = "Beispiel"
```

## Schlüsselwörter

Der Sprachkern unterstützt insbesondere:

```text
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
And
AndAlso
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

Schlüsselwörter werden ohne Berücksichtigung der Groß- und Kleinschreibung erkannt.

## Imports

```ebnf
importsStatement =
    "Imports" qualifiedName statementTerminator ;
```

Beispiel:

```vb
Imports Nova.System
Imports Nova.Text
Imports Nova.Network
```

`Imports` macht Namen verfügbar, erteilt aber keine Capability-Berechtigungen.

## Namespaces

```ebnf
namespaceDeclaration =
    "Namespace" qualifiedName statementTerminator
        { declaration }
    "End" "Namespace" statementTerminator ;
```

Beispiel:

```vb
Namespace Nova.Example

    Public Class Application

    End Class

End Namespace
```

## Module

```ebnf
moduleDeclaration =
    [ accessModifier ]
    "Module" identifier statementTerminator
        { declaration }
    "End" "Module" statementTerminator ;
```

Beispiel:

```vb
Public Module MainModule

    Public Sub Main()
        Console.WriteLine("NovaOS")
    End Sub

End Module
```

## Variablen

```ebnf
variableDeclaration =
    [ modifierList ]
    "Dim" variableDeclaratorList statementTerminator ;

variableDeclaratorList =
    variableDeclarator
    { "," variableDeclarator } ;

variableDeclarator =
    identifier
    [ "As" type ]
    [ "=" expression ] ;
```

Beispiele:

```vb
Dim name As String = "NovaOS"

Dim counter As Integer = 0

Dim enabled = True
```

Die Typinferenz wird durch das NovaLang-Typsystem geregelt.

## Konstanten

```ebnf
constantDeclaration =
    [ accessModifier ]
    "Const" identifier
    [ "As" type ]
    "=" constantExpression
    statementTerminator ;
```

Beispiel:

```vb
Const MaxAttempts As Integer = 5
```

Konstante Ausdrücke müssen zur Übersetzungszeit auswertbar sein.

## Typgrammatik

```ebnf
type =
    namedType
    [ nullableSuffix ]
    { arraySuffix } ;

namedType =
    qualifiedName
    [ genericArguments ] ;

genericArguments =
    "(" "Of" typeList ")" ;

typeList =
    type { "," type } ;

nullableSuffix =
    "?" ;

arraySuffix =
    "(" { "," } ")" ;
```

Beispiele:

```vb
Dim name As String

Dim numbers As Integer()

Dim values As List(Of Integer)

Dim optionalNumber As Integer?
```

Typnamen können Aliase für definierte primitive oder semantische Typen sein.

## Funktionen

```ebnf
functionDeclaration =
    [ modifierList ]
    [ "Async" ]
    "Function" identifier
    [ genericParameters ]
    "(" [ parameterList ] ")"
    "As" type
    statementTerminator
        { statement }
    "End" "Function"
    statementTerminator ;

genericParameters =
    "(" "Of" identifier { "," identifier } ")" ;

parameterList =
    parameter { "," parameter } ;

parameter =
    [ parameterModifier ]
    identifier "As" type
    [ "=" expression ] ;

parameterModifier =
    "ByVal"
  | "ByRef"
  | "Optional" ;
```

Beispiel:

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer

    Return a + b

End Function
```

Die vollständigen Regeln für optionale Parameter, Parameterarrays und generische Constraints werden in ergänzenden Sprachspezifikationen definiert.

## Prozeduren

```ebnf
subDeclaration =
    [ modifierList ]
    "Sub" identifier
    "(" [ parameterList ] ")"
    statementTerminator
        { statement }
    "End" "Sub"
    statementTerminator ;
```

Beispiel:

```vb
Public Sub Begruessen(name As String)

    Console.WriteLine("Hallo " & name)

End Sub
```

## Klassen

```ebnf
classDeclaration =
    [ modifierList ]
    "Class" identifier
    [ genericParameters ]
    statementTerminator
        [ inheritsClause ]
        { implementsClause }
        { classMember }
    "End" "Class"
    statementTerminator ;

inheritsClause =
    "Inherits" type statementTerminator ;

implementsClause =
    "Implements" typeList statementTerminator ;

classMember =
    declaration
  | constructorDeclaration ;
```

Beispiel:

```vb
Public Class Person

    Public Property Name As String

    Public Sub New(name As String)
        Me.Name = name
    End Sub

End Class
```

## Konstruktoren

```ebnf
constructorDeclaration =
    [ accessModifier ]
    "Sub" "New"
    "(" [ parameterList ] ")"
    statementTerminator
        { statement }
    "End" "Sub"
    statementTerminator ;
```

## Strukturen

```ebnf
structureDeclaration =
    [ modifierList ]
    "Structure" identifier
    [ genericParameters ]
    statementTerminator
        { classMember }
    "End" "Structure"
    statementTerminator ;
```

Beispiel:

```vb
Public Structure Position

    Public X As Double
    Public Y As Double

End Structure
```

## Interfaces

```ebnf
interfaceDeclaration =
    [ modifierList ]
    "Interface" identifier
    [ genericParameters ]
    statementTerminator
        { interfaceMember }
    "End" "Interface"
    statementTerminator ;

interfaceMember =
    functionSignature
  | subSignature
  | propertySignature
  | eventSignature ;
```

Beispiel:

```vb
Public Interface IDrawable

    Sub Draw()

End Interface
```

Interface-Implementierungen verwenden die VB.NET-nahe `Implements`-Syntax.

## Enumerationen

```ebnf
enumDeclaration =
    [ accessModifier ]
    "Enum" identifier
    [ "As" type ]
    statementTerminator
        { enumMember }
    "End" "Enum"
    statementTerminator ;

enumMember =
    identifier
    [ "=" constantExpression ]
    statementTerminator ;
```

Beispiel:

```vb
Public Enum SystemState

    Starting
    Running
    Suspended
    Stopped

End Enum
```

## Eigenschaften

```ebnf
propertyDeclaration =
    [ modifierList ]
    "Property" identifier
    [ "(" [ parameterList ] ")" ]
    "As" type
    [ "=" expression ]
    statementTerminator ;
```

Beispiel einer automatisch implementierten Eigenschaft:

```vb
Public Property Name As String
```

Ausführliche Eigenschaften verwenden `Get`, `Set` und `End Property`.

```vb
Private _name As String

Public Property Name As String

    Get
        Return _name
    End Get

    Set(value As String)
        _name = value
    End Set

End Property
```

## Bedingungen

```ebnf
ifStatement =
    "If" expression "Then" statementTerminator
        { statement }
    { "ElseIf" expression "Then" statementTerminator
        { statement } }
    [ "Else" statementTerminator
        { statement } ]
    "End" "If"
    statementTerminator ;
```

Beispiel:

```vb
If temperatur > 30 Then

    Console.WriteLine("Warm")

ElseIf temperatur > 20 Then

    Console.WriteLine("Angenehm")

Else

    Console.WriteLine("Kühl")

End If
```

Einzeilige `If`-Anweisungen werden als separate, eindeutig definierte Grammatikform unterstützt.

## Select Case

```ebnf
selectStatement =
    "Select" "Case" expression statementTerminator
        { caseBlock }
    "End" "Select"
    statementTerminator ;

caseBlock =
    "Case" casePatternList statementTerminator
        { statement } ;

casePatternList =
    casePattern { "," casePattern } ;

casePattern =
    expression
  | "Else" ;
```

Beispiel:

```vb
Select Case state

    Case SystemState.Starting
        Console.WriteLine("Startet")

    Case SystemState.Running
        Console.WriteLine("Läuft")

    Case Else
        Console.WriteLine("Unbekannt")

End Select
```

Bereichsvergleiche und weitere VB.NET-Case-Formen werden durch ergänzende Grammatikregeln definiert.

## For-Schleifen

```ebnf
forStatement =
    "For" identifier
    [ "As" type ]
    "=" expression
    "To" expression
    [ "Step" expression ]
    statementTerminator
        { statement }
    "Next" [ identifier ]
    statementTerminator ;
```

Beispiel:

```vb
For i As Integer = 1 To 10

    Console.WriteLine(i)

Next
```

`To` schließt den Endwert entsprechend den VB.NET-Regeln ein, sofern er durch die Schrittweite erreicht wird.

## For Each

```ebnf
forEachStatement =
    "For" "Each" identifier
    [ "As" type ]
    "In" expression
    statementTerminator
        { statement }
    "Next" [ identifier ]
    statementTerminator ;
```

Beispiel:

```vb
For Each name As String In names

    Console.WriteLine(name)

Next
```

## While-Schleifen

```ebnf
whileStatement =
    "While" expression statementTerminator
        { statement }
    "End" "While"
    statementTerminator ;
```

Beispiel:

```vb
While running

    ProcessEvents()

End While
```

## Do-Schleifen

```ebnf
doStatement =
    "Do"
    [ "While" expression | "Until" expression ]
    statementTerminator
        { statement }
    "Loop"
    [ "While" expression | "Until" expression ]
    statementTerminator ;
```

Die Kombination aus Vor- und Nachbedingung innerhalb derselben Schleife ist nicht zulässig.

## Return

```ebnf
returnStatement =
    "Return"
    [ expression ]
    statementTerminator ;
```

Beispiel:

```vb
Return ergebnis
```

## Fehlerbehandlung

```ebnf
tryStatement =
    "Try" statementTerminator
        { statement }
    { catchBlock }
    [ finallyBlock ]
    "End" "Try"
    statementTerminator ;

catchBlock =
    "Catch"
    [ identifier "As" type ]
    [ "When" expression ]
    statementTerminator
        { statement } ;

finallyBlock =
    "Finally" statementTerminator
        { statement } ;
```

Beispiel:

```vb
Try

    DatenLaden()

Catch ex As Exception

    Console.WriteLine(ex.Message)

Finally

    RessourcenFreigeben()

End Try
```

## Ausdrucksgrammatik

NovaLang übernimmt grundsätzlich die Operatorhierarchie von VB.NET.

```ebnf
expression =
    assignmentExpression ;

assignmentExpression =
    logicalExpression
    [ assignmentOperator expression ] ;

logicalExpression =
    comparisonExpression
    { logicalOperator comparisonExpression } ;

comparisonExpression =
    concatenationExpression
    { comparisonOperator concatenationExpression } ;

concatenationExpression =
    additiveExpression
    { "&" additiveExpression } ;

additiveExpression =
    multiplicativeExpression
    { ( "+" | "-" ) multiplicativeExpression } ;

multiplicativeExpression =
    powerExpression
    { ( "*" | "/" | "\" | "Mod" ) powerExpression } ;

powerExpression =
    unaryExpression
    [ "^" powerExpression ] ;

unaryExpression =
    ( "+" | "-" | "Not" ) unaryExpression
  | awaitExpression ;

awaitExpression =
    [ "Await" ] postfixExpression ;
```

Diese EBNF ist eine strukturelle Übersicht. Die normative Operatorpräzedenz wird als vollständige Tabelle festgelegt, insbesondere für `Not`, `And`, `AndAlso`, `Or`, `OrElse`, `Xor`, `Is`, `IsNot`, `Like` und Potenzierung.

Zuweisungen sind nur in grammatikalisch zulässigen Anweisungskontexten erlaubt.

## Operatoren

NovaLang unterstützt mindestens:

```text
Arithmetik:
+  -  *  /  \  Mod  ^

Vergleich:
=  <>  <  <=  >  >=  Is  IsNot  Like

Logik:
Not  And  AndAlso  Or  OrElse  Xor

Verkettung:
&

Zuweisung:
=  +=  -=  *=  /=  \=  ^=  &=

Memberzugriff:
.

Nullable:
?
```

`=` wird abhängig vom syntaktischen Kontext als Vergleich oder Zuweisung interpretiert.

`&` dient der Zeichenkettenverkettung.

## Primärausdrücke

```ebnf
postfixExpression =
    primaryExpression
    { memberAccess | invocation | indexAccess } ;

primaryExpression =
    literal
  | identifier
  | "Me"
  | "MyBase"
  | "Nothing"
  | parenthesizedExpression
  | objectCreationExpression
  | lambdaExpression ;

parenthesizedExpression =
    "(" expression ")" ;

memberAccess =
    "." identifier ;

invocation =
    "(" [ argumentList ] ")" ;

indexAccess =
    "(" argumentList ")" ;

argumentList =
    argument { "," argument } ;

argument =
    [ identifier ":=" ] expression ;
```

Der Parser erzeugt zunächst eine syntaktische Repräsentation. Die semantische Analyse unterscheidet Methodenaufrufe, Indexzugriffe und andere zulässige Verwendungen.

## Objekterzeugung

```ebnf
objectCreationExpression =
    "New" type
    "(" [ argumentList ] ")" ;
```

Beispiel:

```vb
Dim person As New Person("Max")
```

Objektinitialisierer verwenden die definierte VB.NET-nahe `With`-Syntax.

```vb
Dim person As New Person With {
    .Name = "Max",
    .Alter = 30
}
```

Geschweifte Klammern sind hier Bestandteil der Initialisierersyntax und keine allgemeinen Blockbegrenzer.

## Generics

```ebnf
genericParameters =
    "(" "Of" genericParameterList ")" ;

genericParameterList =
    genericParameter { "," genericParameter } ;

genericParameter =
    identifier [ genericConstraint ] ;
```

Beispiel:

```vb
Public Class Container(Of T)

    Public Property Value As T

End Class
```

Generische Constraints werden durch ergänzende Typregeln definiert.

## Lambdas

NovaLang unterstützt VB.NET-nahe Lambda-Ausdrücke.

```vb
Dim verdoppeln = Function(x As Integer) x * 2
```

Mehrzeilige Lambdas besitzen explizite Blockabschlüsse.

```vb
Dim berechnen = Function(x As Integer) As Integer

                    Return x * 2

                End Function
```

## Asynchrone Funktionen

```ebnf
asyncFunctionDeclaration =
    [ modifierList ]
    "Async" "Function" identifier
    "(" [ parameterList ] ")"
    "As" type
    statementTerminator
        { statement }
    "End" "Function"
    statementTerminator ;
```

Beispiel:

```vb
Public Async Function LadenAsync() As Task(Of String)

    Dim daten = Await Netzwerk.LesenAsync()

    Return daten

End Function
```

`Task` ist ein NovaLang-/NovaOS-Typ und setzt keine .NET-Runtime voraus.

## Ereignisse

```ebnf
eventDeclaration =
    [ accessModifier ]
    "Event" identifier
    "(" [ parameterList ] ")"
    statementTerminator ;
```

Beispiel:

```vb
Public Event DataChanged(value As String)
```

Ereignisse können durch `RaiseEvent` ausgelöst werden.

```vb
RaiseEvent DataChanged("Aktualisiert")
```

## Deklarative Benutzeroberflächen

`.nui` verwendet die gemeinsame NovaLang-Grammatik.

Die deklarative UI-Syntax orientiert sich an VB.NET-Ausdrücken, Eigenschaften und Objektinitialisierungen.

Ein möglicher deklarativer Aufbau ist:

```vb
Window With {
    .Title = "NovaOS",
    .Width = 800,
    .Height = 500
}
```

Die endgültige deklarative Struktur, insbesondere verschachtelte Elemente, Bindungen und Ereigniszuordnungen, wird in einer eigenen NovaLang-UI-Spezifikation normativ festgelegt.

`.nui` darf keine von NovaLang abweichenden Ausdrucks-, Typ- oder Bezeichnerregeln einführen.

## Logic-Graph-Skripte

`.nlf` verwendet die vollständige NovaLang-Grammatik.

Beispiel:

```vb
Public Function Transformieren(eingabe As String) As String

    Return eingabe & " verarbeitet"

End Function
```

Logic-Graph-Skripte können typisierte Parameter und Rückgabewerte besitzen.

Die Grammatik erzeugt keine Capability-Berechtigungen.

## Sichtbarkeitsmodifikatoren

```ebnf
accessModifier =
    "Public"
  | "Private"
  | "Protected"
  | "Friend" ;

modifierList =
    modifier { modifier } ;

modifier =
    accessModifier
  | "Shared"
  | "ReadOnly"
  | "WriteOnly"
  | "Overridable"
  | "Overrides"
  | "MustOverride"
  | "MustInherit"
  | "Partial"
  | "Static" ;
```

Nicht jede Modifikatorkombination ist zulässig.

Die semantische Analyse prüft Gültigkeitsbereich und Kombinationen.

## Parser-Kontext

Der Parser muss folgende Konstrukte unterscheiden:

```text
Declaration
Statement
Expression
Type
Member Access
Invocation
Object Initializer
Lambda
Declarative UI
```

Kontextabhängige Entscheidungen müssen deterministisch erfolgen.

Der Parser darf keine Laufzeitwerte benötigen.

## Fehlerbehandlung

Syntaxfehler müssen mindestens folgende Informationen enthalten:

```text
ErrorCode
Message
SourceFile
StartOffset
EndOffset
Line
Column
ExpectedTokens
ActualToken
```

Beispiel:

```text
NOVA-SYNTAX-0012

Fehlender Blockabschluss: End If

Datei: Main.nova
Zeile: 24
Spalte: 1
```

Der Parser soll nach einem Fehler kontrolliert fortfahren können, damit weitere Diagnosen möglich bleiben.

## Grammatikversionierung

Die Grammatik wird unabhängig von Compiler und Runtime versioniert.

```text
GrammarVersion
LexerVersion
SyntaxVersion
LanguageVersion
```

Neue Sprachkonstrukte dürfen bestehende Programme nicht unbemerkt semantisch verändern.

Inkompatible Änderungen benötigen eine explizite Sprachversionsänderung.

## Normative Anforderungen

1. NovaLang MUSS eine VB.NET-orientierte formale Grammatik besitzen.
2. Die Grammatik MUSS zeilenorientierte Anweisungen unterstützen.
3. Mehrzeilige Blöcke MÜSSEN explizite Abschlüsse besitzen.
4. Bezeichner MÜSSEN ohne Unterscheidung der Groß- und Kleinschreibung aufgelöst werden.
5. Typdeklarationen MÜSSEN `As` verwenden.
6. Generische Parameter MÜSSEN die `Of`-Syntax unterstützen.
7. Funktionen und Prozeduren MÜSSEN `Function` und `Sub` verwenden.
8. Klassen, Strukturen, Interfaces und Namespaces MÜSSEN VB.NET-nahe Deklarationen besitzen.
9. Kontrollstrukturen MÜSSEN sich grundsätzlich an VB.NET orientieren.
10. Operatoren MÜSSEN eine eindeutig definierte Präzedenz und Assoziativität besitzen.
11. `.nova`, `.nlf` und `.nui` MÜSSEN denselben Sprachkern verwenden.
12. Der Parser DARF keine Systemberechtigungen erzeugen oder anfordern.
13. Syntaxanalyse MUSS unabhängig von der .NET-Runtime erfolgen.
14. Fehler MÜSSEN präzise diagnostizierbar sein.
15. Grammatikänderungen MÜSSEN versioniert werden.
16. Compiler und Interpreter MÜSSEN dieselbe Grammatikinterpretation verwenden.
17. Eine vollständige maschinenlesbare Referenzgrammatik MUSS bereitgestellt werden.
18. Lexer, Parser und semantische Analyse MÜSSEN getrennte Verarbeitungsschritte bleiben.

## Abhängigkeiten

- `NPSPEC-NOVALANG-CORE-0001`
- `NPSPEC-NOVALANG-SYNTAX-0001`
- `NPSPEC-NOVALANG-LEXER-0001`
- `NPSPEC-NOVALANG-TYPES-0001`
- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-UTF8-0001`
- `NPSPEC-TEXT-SECURITY-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`

## Ergebnis

NovaLang besitzt eine einheitliche, VB.NET-orientierte Grammatik mit verständlichen Schlüsselwörtern, zeilenorientierten Anweisungen und expliziten Blockabschlüssen.

Die Grammatik bildet die Grundlage für die native NovaLang-Toolchain und unterstützt klassische Programme, Solutions, Logic Graph und deklarative Benutzeroberflächen.

NovaLang bleibt syntaktisch vertraut, technisch eigenständig und vollständig unabhängig von der .NET-Runtime.