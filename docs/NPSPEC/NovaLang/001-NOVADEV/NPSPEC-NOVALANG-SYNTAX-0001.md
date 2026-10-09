# NPSPEC-NOVALANG-SYNTAX-0001 – NovaLang Syntax Definition

## Status

Angenommen

## Kategorie

NovaLang / Sprachsyntax

## Zweck

Diese Spezifikation definiert die grundlegende Syntax von NovaLang.

NovaLang orientiert sich syntaktisch möglichst exakt an Visual Basic .NET (VB.NET), verwendet jedoch ein eigenständiges Typsystem, einen nativen Compiler und die NovaOS-Runtime.

Ziel ist eine leicht verständliche, gut lesbare und leistungsfähige Sprache für Systemkomponenten, Programme, Capabilities, Solutions, Logic Graph und deklarative Benutzeroberflächen.

Die Syntax gilt einheitlich für `.nova`, `.nlf` und `.nui`.

## Grundprinzipien

1. VB.NET bildet die syntaktische Grundlage.
2. Schlüsselwörter sind ausgeschrieben und gut lesbar.
3. Anweisungen sind grundsätzlich zeilenorientiert.
4. Blöcke besitzen explizite Abschlüsse.
5. Bezeichner unterscheiden nicht zwischen Groß- und Kleinschreibung.
6. Typdeklarationen verwenden `As`.
7. Generics verwenden `Of`.
8. Klassen, Funktionen und Kontrollstrukturen folgen dem VB.NET-Modell.
9. Die Sprache unterstützt imperative, objektorientierte, funktionale, ereignisorientierte und deklarative Programmierung.
10. NovaLang benötigt keine .NET-Runtime.

## Quelldateien

| Erweiterung | Verwendung |
|---|---|
| `.nova` | Allgemeiner NovaLang-Quelltext |
| `.nlf` | Nova Logic File für Solutions und Logic Graph |
| `.nui` | Deklarative NovaLang-Benutzeroberfläche |

Alle Dateitypen verwenden dieselben lexikalischen Regeln, Ausdrücke und Typsemantiken.

`.nlf` ist kein eingeschränkter Dialekt.

`.nui` verwendet die deklarativen Konstrukte derselben Sprache.

## Quelltextkodierung

NovaLang verwendet UTF-8.

Ein UTF-8-BOM ist nicht erforderlich.

Zeilenenden mit LF und CRLF werden unterstützt.

Die Tokenisierung muss unabhängig von Betriebssystem, Systemsprache und Regionaleinstellungen erfolgen.

## Groß- und Kleinschreibung

NovaLang ist wie VB.NET nicht case-sensitive.

```vb
Dim MeineVariable As Integer = 10

meinevariable = 20

MEINEVARIABLE = 30
```

Alle Schreibweisen bezeichnen dieselbe Variable.

Dies gilt auch für Schlüsselwörter.

```vb
If aktiviert Then
    Starten()
End If
```

Die ursprüngliche Schreibweise bleibt für Entwicklungswerkzeuge erhalten.

Die Unicode-Vergleichsregeln müssen versioniert und eindeutig definiert sein.

Systemidentitäten wie `CapabilityID` unterliegen ihren eigenen Vergleichsregeln.

## Kommentare

Einzeilige Kommentare beginnen mit `'`.

```vb
' Dies ist ein Kommentar

Dim zahl As Integer = 10 ' Kommentar hinter einer Anweisung
```

Dokumentationskommentare beginnen mit `'''`.

```vb
''' <summary>
''' Addiert zwei Zahlen.
''' </summary>
Public Function Addieren(a As Integer, b As Integer) As Integer
    Return a + b
End Function
```

C-artige Kommentare mit `//` und `/* ... */` gehören nicht zur regulären NovaLang-Syntax.

## Anweisungen und Blöcke

Eine Anweisung endet grundsätzlich mit einem logischen Zeilenumbruch.

```vb
Dim a As Integer = 10
Dim b As Integer = 20
Dim ergebnis As Integer = a + b
```

Mehrere Anweisungen können mit `:` getrennt werden.

```vb
Dim a As Integer = 10 : Dim b As Integer = 20
```

Blöcke besitzen explizite Abschlüsse.

```vb
If aktiviert Then
    Starten()
End If
```

Geschweifte Klammern sind keine allgemeinen Blockbegrenzer.

## Zeilenfortsetzung

NovaLang unterstützt explizite Zeilenfortsetzung mit `_`.

```vb
Dim ergebnis As Integer = 10 + _
                          20 + _
                          30
```

Implizite Zeilenfortsetzung ist an grammatikalisch eindeutig definierten Stellen erlaubt.

```vb
Dim ergebnis As Integer = 10 +
                          20 +
                          30
```

## Variablen

Variablen werden mit `Dim` deklariert.

```vb
Dim name As String = "NovaOS"
Dim version As Integer = 1
Dim aktiviert As Boolean = True
```

Der Typ kann bei eindeutiger Typinferenz entfallen.

```vb
Dim name = "NovaOS"
Dim version = 1
Dim aktiviert = True
```

NovaLang verwendet standardmäßig statische Typprüfung.

## Konstanten

Konstanten werden mit `Const` deklariert.

```vb
Const MaxAttempts As Integer = 5
Const ApplicationName As String = "NovaOS"
Const Pi As Double = 3.141592653589793
```

Konstante Ausdrücke müssen zur Übersetzungszeit auswertbar sein.

## Primitive Datentypen

NovaLang unterstützt die VB.NET-orientierten Typnamen:

```text
Boolean
Byte
SByte
Short
UShort
Integer
UInteger
Long
ULong
Single
Double
Decimal
Char
String
Object
```

Zusätzlich werden moderne und semantische NovaOS-Typen unterstützt.

Die genaue Speicherrepräsentation wird durch das Typsystem definiert.

## Numerische Literale

```vb
Dim ganzzahl As Integer = 42
Dim gleitkomma As Double = 3.1415
Dim dezimalzahl As Decimal = 19.95D

Dim hexWert As Integer = &HFF
Dim binaerWert As Integer = &B101010
Dim oktalWert As Integer = &O77
```

Das Dezimaltrennzeichen im Quelltext ist immer `.`.

## Zeichenketten

Zeichenketten verwenden doppelte Anführungszeichen.

```vb
Dim name As String = "NovaOS"
```

Anführungszeichen innerhalb einer Zeichenkette werden verdoppelt.

```vb
Dim text As String = "Er sagte ""Hallo""."
```

Die Verkettung erfolgt mit `&`.

```vb
Dim begruessung As String = "Hallo " & name
```

C-artige Escape-Sequenzen sind nicht Bestandteil der regulären Stringsyntax.

## Zeichenliterale

Zeichenliterale verwenden den VB.NET-orientierten Suffix `c`.

```vb
Dim buchstabe As Char = "A"c
```

`Char` verwendet die festgelegte VB.NET-kompatible UTF-16-Codeunit-Semantik.

Für Unicode-Skalarwerte und Grapheme stehen gesonderte Textoperationen und Typen zur Verfügung.

## Boolean und Nothing

```vb
Dim aktiviert As Boolean = True
Dim beendet As Boolean = False

Dim person As Person = Nothing
```

`Nothing` wird anhand des Zieltyps interpretiert.

Unsichere Nullreferenzzugriffe dürfen nicht durch die sichere Sprachsemantik entstehen.

## Operatoren

### Arithmetische Operatoren

```text
+       Addition
-       Subtraktion
*       Multiplikation
/       Division
\       Ganzzahldivision
Mod     Modulo
^       Potenzierung
```

### Vergleichsoperatoren

```text
=       Gleich
<>      Ungleich
<       Kleiner
<=      Kleiner oder gleich
>       Größer
>=      Größer oder gleich
Is      Referenzidentität
IsNot   Negierte Referenzidentität
Like    Mustervergleich
```

### Logische Operatoren

```text
Not
And
AndAlso
Or
OrElse
Xor
```

`AndAlso` und `OrElse` verwenden Kurzschlussauswertung.

### Zuweisungsoperatoren

```text
=
+=
-=
*=
/=
\=
^=
&=
```

Die Operatorpräzedenz und Assoziativität werden durch die formale Grammatik verbindlich definiert.

## Bedingungen

```vb
If temperatur > 30 Then

    Console.WriteLine("Warm")

ElseIf temperatur > 20 Then

    Console.WriteLine("Angenehm")

Else

    Console.WriteLine("Kühl")

End If
```

Einzeilige Bedingungen werden ebenfalls unterstützt.

```vb
If aktiviert Then Starten()
```

## Select Case

```vb
Select Case zustand

    Case SystemState.Starting
        Console.WriteLine("Startet")

    Case SystemState.Running
        Console.WriteLine("Läuft")

    Case SystemState.Stopped
        Console.WriteLine("Beendet")

    Case Else
        Console.WriteLine("Unbekannt")

End Select
```

NovaLang unterstützt typgeprüfte `Case`-Ausdrücke.

Erweiterte Pattern-Matching-Konstrukte müssen die bestehende Syntax eindeutig ergänzen.

## For-Schleifen

```vb
For i As Integer = 1 To 10

    Console.WriteLine(i)

Next
```

Schrittweiten werden mit `Step` definiert.

```vb
For i As Integer = 10 To 0 Step -1

    Console.WriteLine(i)

Next
```

`To` verwendet die VB.NET-orientierte inklusive Endwertsemantik.

## For Each

```vb
For Each datei As FileInfo In dateien

    Console.WriteLine(datei.Name)

Next
```

Die Iteration erfolgt über typisierte Aufzählungsschnittstellen.

## While-Schleifen

```vb
While aktiviert

    Verarbeitung.Starten()

End While
```

## Do-Schleifen

```vb
Do While aktiviert

    Verarbeitung.Starten()

Loop
```

Auch nachgestellte Bedingungen werden unterstützt.

```vb
Do

    Verarbeitung.Starten()

Loop Until beendet
```

## Funktionen

Funktionen werden mit `Function` deklariert.

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer

    Return a + b

End Function
```

Parameter verwenden die Schreibweise:

```text
name As Type
```

Rückgabetypen werden mit `As` angegeben.

NovaLang unterstützt Funktionsüberladung, Rekursion und generische Funktionen.

## Prozeduren

Prozeduren ohne Rückgabewert verwenden `Sub`.

```vb
Public Sub Begruessen(name As String)

    Console.WriteLine("Hallo " & name)

End Sub
```

## Parameter

NovaLang unterstützt die VB.NET-orientierten Parametermodifikatoren.

```text
ByVal
ByRef
Optional
ParamArray
```

Beispiel:

```vb
Public Function Begruessung(Optional name As String = "Gast") As String

    Return "Hallo " & name

End Function
```

`ByRef` muss die Speicher- und Lebensdauersicherheitsregeln von NovaOS einhalten.

## Klassen

```vb
Public Class Person

    Private _name As String

    Public Sub New(name As String)
        _name = name
    End Sub

    Public Function GetName() As String
        Return _name
    End Function

End Class
```

Unterstützte Konzepte:

- Kapselung
- Vererbung
- Polymorphie
- Konstruktoren
- Methoden
- Eigenschaften
- Interfaces
- Generics
- Sichtbarkeitsmodifikatoren

## Vererbung

```vb
Public Class Fahrzeug

    Public Overridable Sub Starten()
    End Sub

End Class

Public Class Auto
    Inherits Fahrzeug

    Public Overrides Sub Starten()
        Console.WriteLine("Motor startet")
    End Sub

End Class
```

Vererbung unterliegt statischer Typprüfung.

## Strukturen

```vb
Public Structure Position

    Public X As Double
    Public Y As Double

End Structure
```

Strukturen besitzen Wertsemantik gemäß dem NovaLang-Typsystem.

## Enumerationen

```vb
Public Enum SystemState

    Starting
    Running
    Suspended
    Stopped

End Enum
```

## Interfaces

```vb
Public Interface IDrawable

    Sub Draw()

End Interface
```

Implementierung:

```vb
Public Class Window
    Implements IDrawable

    Public Sub Draw() Implements IDrawable.Draw

        Console.WriteLine("Fenster zeichnen")

    End Sub

End Class
```

Interfaces definieren überprüfbare Verträge.

## Eigenschaften

NovaLang unterstützt automatische Eigenschaften.

```vb
Public Property Name As String
Public Property Alter As Integer
```

Ausführliche Eigenschaften verwenden `Get` und `Set`.

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

## Generics

Generische Typen verwenden `Of`.

```vb
Public Class Container(Of T)

    Public Property Value As T

End Class
```

Generische Funktionen:

```vb
Public Function Identitaet(Of T)(value As T) As T

    Return value

End Function
```

Generische Constraints werden statisch geprüft.

## Namespaces

```vb
Namespace Nova.Example

    Public Class Application

    End Class

End Namespace
```

## Imports

```vb
Imports Nova.System
Imports Nova.Text
Imports Nova.Math
```

`Imports` macht Typen und Namensräume verfügbar.

Es erteilt keine Systemberechtigungen.

## Module

```vb
Public Module MainModule

    Public Sub Main()

        Console.WriteLine("NovaOS gestartet")

    End Sub

End Module
```

Module können Funktionen, Konstanten und andere zulässige Deklarationen enthalten.

## Fehlerbehandlung

NovaLang unterstützt strukturierte Fehlerbehandlung.

```vb
Try

    DatenLaden()

Catch ex As Exception

    Console.WriteLine(ex.Message)

Finally

    RessourcenFreigeben()

End Try
```

Zusätzlich werden typisierte Ergebnisse unterstützt.

```vb
Public Function Laden() As Result(Of String, Error)

    Return Result.Ok("Daten")

End Function
```

Exceptions, Result-Fehler, Abbruch und Vertragsverletzungen bleiben unterscheidbare Zustände.

## Asynchrone Funktionen

```vb
Public Async Function DatenLadenAsync() As Task(Of String)

    Dim daten As String = Await Netzwerk.LesenAsync()

    Return daten

End Function
```

`Async` und `Await` verwenden das NovaOS-Taskmodell.

Die Implementierung benötigt keine .NET-Runtime.

Asynchrone Operationen unterliegen Structured Concurrency und den jeweiligen Execution Contracts.

## Ereignisse

```vb
Public Event DataChanged(value As String)
```

Auslösen eines Ereignisses:

```vb
RaiseEvent DataChanged("Aktualisiert")
```

Ereignisse können über `AddHandler`, `RemoveHandler` und `Handles` mit Handlern verbunden werden.

Die Ereignisbindung muss typ- und lebensdauersicher erfolgen.

## Lambdas

NovaLang unterstützt VB.NET-orientierte Lambda-Ausdrücke.

```vb
Dim verdoppeln = Function(x As Integer) x * 2
```

Mehrzeilige Lambdas:

```vb
Dim berechnen = Function(x As Integer) As Integer

                    Return x * 2

                End Function
```

## Nullable-Typen

Optionale Werttypen verwenden die VB.NET-orientierte Nullable-Syntax.

```vb
Dim alter As Integer? = Nothing
```

Die Kurzform entspricht:

```vb
Dim alter As Nullable(Of Integer) = Nothing
```

Die Verwendung von `Nothing` und die Nullbarkeit von Referenztypen werden durch das NovaLang-Typsystem definiert.

## Arrays und Collections

```vb
Dim zahlen As Integer() = {1, 2, 3, 4, 5}

Dim namen As New List(Of String)
```

Arrays und Collections besitzen typisierte Elemente.

Indexzugriffe und Bereichsprüfungen müssen speichersicher erfolgen.

## Objektinitialisierung

```vb
Dim person As New Person With {
    .Name = "Max",
    .Alter = 30
}
```

Geschweifte Klammern sind bei Objekt- und Collection-Initialisierern zulässig.

Sie ersetzen keine VB.NET-Blockabschlüsse.

## Semantische Typen

NovaLang unterstützt native NovaOS-Typen.

```text
ObjectID
CapabilityID
SolutionID
AppID
SemanticTypeID
ResourceHandle
```

Beispiel:

```vb
Dim objekt As ObjectID
Dim faehigkeit As CapabilityID
```

Semantisch unterschiedliche Typen dürfen nicht allein aufgrund identischer Speicherrepräsentation austauschbar sein.

## Capability-Schnittstellen

Capabilities werden über autorisierte, typisierte Schnittstellen verwendet.

```vb
Public Function DatenLesen(reader As INetworkReader) As Result(Of String, Error)

    Return reader.Read()

End Function
```

Dabei gilt:

- `Imports` erteilt keine Berechtigungen.
- Ein Capability-Typ erteilt keine Berechtigungen.
- Eine CapabilityID ist kein autorisiertes Handle.
- Berechtigungen entstehen ausschließlich durch autorisierte Systemmechanismen.
- Capability-Aufrufe unterliegen Execution Contracts und Systemrichtlinien.

## Nova Logic Files

`.nlf` verwendet exakt dieselbe NovaLang-Syntax.

```vb
Public Function Transformieren(eingabe As String) As String

    Return eingabe & " verarbeitet"

End Function
```

Es existiert kein separater NovaLang-Skriptdialekt.

## Logic-Graph-Integration

Custom Scripts verwenden normale NovaLang-Funktionen.

```text
Network Capability
        ↓
Typed Data
        ↓
NovaLang Custom Script
        ↓
Typed Result
        ↓
Storage Capability
```

Ein Custom Script darf nicht selbst zusätzliche Systemberechtigungen anfordern.

Es verarbeitet ausschließlich die vom Logic Graph bereitgestellten Daten und autorisierten Schnittstellen.

## Deklarative Benutzeroberflächen

`.nui` verwendet dieselbe NovaLang-Syntax und dasselbe Typsystem.

Deklarative UI-Konstrukte orientieren sich an VB.NET-Objektinitialisierung, Eigenschaften, Bindungen und Ereignissen.

Beispiel für die syntaktische Grundrichtung:

```vb
Dim fenster As New Window With {
    .Title = "NovaOS",
    .Width = 800,
    .Height = 500
}
```

Verschachtelte UI-Elemente, reaktive Bindungen und deklarative Ereigniszuordnungen werden durch eine gesonderte UI-Syntax-Spezifikation definiert.

Dabei dürfen keine abweichenden Ausdrucks- oder Typregeln entstehen.

Die UI muss auch ohne KI deterministisch erzeugbar und rekonstruierbar sein.

## Sichtbarkeitsmodifikatoren

NovaLang unterstützt:

```text
Public
Private
Protected
Friend
Protected Friend
Private Protected
```

Weitere Modifikatoren umfassen:

```text
Shared
ReadOnly
WriteOnly
Overridable
Overrides
MustOverride
MustInherit
NotInheritable
Partial
Static
```

Die zulässigen Kombinationen werden semantisch geprüft.

## Syntaxdiagnosen

Der Compiler muss präzise Fehlermeldungen erzeugen.

Beispiel:

```text
NOVA-SYNTAX-0012

Fehlender Blockabschluss: End If

Datei: Main.nova
Zeile: 24
Spalte: 1
```

Diagnosen enthalten mindestens:

```text
ErrorCode
Severity
Message
SourceFile
SourceRange
ExpectedSyntax
ActualSyntax
```

NovaLang Studio kann zusätzlich Korrekturvorschläge anbieten.

## Syntaxversionierung

Die NovaLang-Syntax wird unabhängig von Compiler und Runtime versioniert.

Neue Sprachkonstrukte dürfen bestehende Programme nicht unbemerkt verändern.

Die Sprachversion bestimmt:

- Verfügbare Schlüsselwörter.
- Gültige Deklarationen.
- Operatoren und deren Präzedenz.
- Literalformate.
- Zeilenfortsetzungsregeln.
- Erweiterte Sprachkonstrukte.

## Normative Anforderungen

1. NovaLang MUSS sich syntaktisch möglichst exakt an VB.NET orientieren.
2. NovaLang MUSS zeilenorientierte Anweisungen und explizite Blockabschlüsse verwenden.
3. Bezeichner und Schlüsselwörter MÜSSEN ohne Unterscheidung der Groß- und Kleinschreibung verarbeitet werden.
4. Typdeklarationen MÜSSEN die `As`-Syntax unterstützen.
5. Generics MÜSSEN die `Of`-Syntax verwenden.
6. Funktionen und Prozeduren MÜSSEN mit `Function` und `Sub` definiert werden.
7. Klassen, Strukturen, Interfaces, Enumerationen und Namespaces MÜSSEN VB.NET-orientierte Deklarationen verwenden.
8. Kontrollstrukturen MÜSSEN grundsätzlich der VB.NET-Syntax entsprechen.
9. Zeichenketten und Kommentare MÜSSEN die VB.NET-orientierten Regeln verwenden.
10. Operatorpräzedenz und Assoziativität MÜSSEN eindeutig definiert sein.
11. Statische Typprüfung und kontrollierte Typinferenz MÜSSEN unterstützt werden.
12. `Async` und `Await` MÜSSEN mit dem NovaOS-Taskmodell kompatibel sein.
13. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben grundlegenden Syntax- und Typregeln verwenden.
14. `.nlf` DARF keinen separaten Sprachdialekt definieren.
15. Deklarative UI-Konstrukte MÜSSEN Bestandteil derselben Sprachdefinition sein.
16. Imports und Typdeklarationen DÜRFEN keine Capability-Berechtigungen erzeugen.
17. Custom Scripts DÜRFEN keine eigenen Systemberechtigungen anfordern.
18. NovaLang MUSS unabhängig von der .NET-Runtime ausführbar sein.
19. Syntaxfehler MÜSSEN präzise diagnostizierbar sein.
20. Syntaxänderungen MÜSSEN versioniert werden.

## Abhängigkeiten

- `NPSPEC-NOVALANG-CORE-0001`
- `NPSPEC-NOVALANG-GRAMMAR-0001`
- `NPSPEC-NOVALANG-LEXER-0001`
- `NPSPEC-NOVALANG-TYPES-0001`
- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-UTF8-0001`
- `NPSPEC-TEXT-SECURITY-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`

## Ergebnis

NovaLang besitzt eine einheitliche, gut lesbare und möglichst VB.NET-kompatible Sprachsyntax.

Die Sprache verwendet vertraute Deklarationen, Kontrollstrukturen, Klassen, Interfaces, Generics und Ereignisse, kombiniert diese jedoch mit dem nativen Typ-, Sicherheits- und Ausführungsmodell von NovaOS.

Programme, Capabilities, Solutions, Logic Graph und deklarative Benutzeroberflächen verwenden dieselbe Sprachgrundlage.

NovaLang bleibt technisch eigenständig und vollständig unabhängig von der .NET-Runtime.