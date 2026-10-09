
# NPSPEC-NOVALANG-VARIABLES-0001 – NovaLang Variables and Constants

## Status

Angenommen

## Kategorie

NovaLang / Variablen und Konstanten

## Zweck

Diese Spezifikation definiert Deklaration, Initialisierung, Zuweisung, Gültigkeitsbereich, Veränderbarkeit und Lebensdauer von Variablen und Konstanten in NovaLang.

Die Syntax orientiert sich möglichst exakt an Visual Basic .NET (VB.NET), verwendet jedoch das native, statisch typisierte und speichersichere Ausführungsmodell von NovaOS.

Die Regeln gelten einheitlich für `.nova`, `.nlf` und `.nui`.

## Grundprinzipien

1. Variablen werden mit `Dim` deklariert.
2. Konstanten werden mit `Const` deklariert.
3. Explizite Typangaben verwenden `As`.
4. Lokale Typinferenz wird unterstützt.
5. Bezeichner unterscheiden nicht zwischen Groß- und Kleinschreibung.
6. Jede Variable besitzt einen definierten Typ, Gültigkeitsbereich und Lebenszyklus.
7. Nicht initialisierter Speicher darf nicht zugänglich sein.
8. Schreibgeschützte Werte dürfen nach ihrer Initialisierung nicht verändert werden.
9. Referenzen und Ressourcen unterliegen den NovaOS-Sicherheitsregeln.
10. Variablendeklarationen erzeugen keine Capability-Berechtigungen.

## Variablendeklaration

Die grundlegende Syntax lautet:

```vb
Dim name As Type = expression
```

Beispiele:

```vb
Dim name As String = "NovaOS"
Dim version As Integer = 1
Dim aktiviert As Boolean = True
Dim temperatur As Double = 21.5
```

Eine Variable besitzt nach erfolgreicher Deklaration einen festen statischen Typ.

Der Wert darf sich ändern, der deklarierte Typ nicht.

## Typinferenz

NovaLang unterstützt lokale Typinferenz nach VB.NET-Vorbild.

```vb
Dim name = "NovaOS"
Dim version = 1
Dim aktiviert = True
```

Der Compiler ermittelt:

```text
name       → String
version    → Integer
aktiviert  → Boolean
```

Typinferenz ist standardmäßig aktiviert.

```vb
Option Infer On
```

Eine Typinferenz darf nicht automatisch zu dynamischer Typisierung führen.

Ist der Typ nicht eindeutig bestimmbar, muss eine explizite Typangabe verlangt oder die Deklaration nach einer ausdrücklich definierten Sprachregel behandelt werden.

## Explizite Typangaben

```vb
Dim counter As Integer = 0
Dim text As String = ""
Dim wert As Double = 0.0
```

Explizite Typangaben haben Vorrang vor der Typinferenz.

Der Initialisierungswert muss zum deklarierten Typ kompatibel sein.

## Mehrfachdeklarationen

Mehrere Variablen können in einer Anweisung deklariert werden.

```vb
Dim a As Integer = 10, b As Integer = 20
```

Auch gemeinsame Typangaben werden nach VB.NET-Vorbild unterstützt.

```vb
Dim x, y, z As Integer
```

Alle drei Variablen besitzen den Typ `Integer`.

Jede Variable erhält eine eigenständige Identität und einen eigenen Speicherzustand.

## Konstanten

Konstanten werden mit `Const` deklariert.

```vb
Const MaxAttempts As Integer = 5
Const ApplicationName As String = "NovaOS"
Const Pi As Double = 3.141592653589793
```

Konstanten müssen einen zur Übersetzungszeit bestimmbaren Wert besitzen.

Nach ihrer Deklaration dürfen sie nicht verändert werden.

```vb
Const MaxAttempts As Integer = 5

' Nicht zulässig:
MaxAttempts = 10
```

Konstantenausdrücke dürfen keine Laufzeit-I/O, Netzwerkzugriffe oder anderen unautorisierten Nebenwirkungen ausführen.

## ReadOnly

`ReadOnly` definiert schreibgeschützte Felder.

```vb
Public Class Configuration

    Public ReadOnly ApplicationName As String = "NovaOS"

End Class
```

Im Gegensatz zu `Const` darf ein `ReadOnly`-Feld einen zur Laufzeit ermittelten Initialisierungswert besitzen.

```vb
Public Class Session

    Public ReadOnly SessionID As String

    Public Sub New(id As String)
        SessionID = id
    End Sub

End Class
```

Zuweisungen sind ausschließlich innerhalb der definierten Initialisierungsphase zulässig.

`ReadOnly` bedeutet nicht automatisch, dass das referenzierte Objekt unveränderlich ist.

## Veränderbarkeit

NovaLang unterscheidet:

```text
Mutable Local Variable
Mutable Field
ReadOnly Field
Constant
Immutable Value
Mutable Object
Shared Mutable State
```

Beispiel:

```vb
Dim counter As Integer = 0

counter += 1
```

Die Variable ist veränderbar.

```vb
Const MaxCount As Integer = 100
```

Die Konstante ist unveränderlich.

Veränderbarkeit einer Variable und Veränderbarkeit eines referenzierten Objekts sind getrennte Eigenschaften.

## Initialisierung

Jede Variable muss vor ihrer Verwendung einen definierten Zustand besitzen.

```vb
Dim zahl As Integer = 0
```

NovaLang unterstützt VB.NET-orientierte Standardinitialisierung.

```vb
Dim zahl As Integer
Dim aktiviert As Boolean
```

Dabei gelten die definierten Standardwerte:

| Typ | Standardwert |
|---|---|
| `Boolean` | `False` |
| Ganzzahltypen | `0` |
| Gleitkommatypen | `0` |
| `Decimal` | `0` |
| `Char` | `ChrW(0)` |
| Nullable-Werttypen | `Nothing` |
| Referenztypen | `Nothing`, sofern zulässig |

Nicht-nullbare Referenzen müssen vor einer Verwendung gültig initialisiert sein.

Ein uninitialisierter oder ungültiger Speicherzustand darf nicht als regulärer Wert verfügbar werden.

## Zuweisungen

```vb
Dim zahl As Integer = 10

zahl = 20
```

Die rechte Seite wird zuerst ausgewertet.

Anschließend wird das Ergebnis gemäß den Typregeln der Zielvariable zugewiesen.

Die Zuweisung darf keine inkompatiblen oder unsicheren Typänderungen verursachen.

## Zusammengesetzte Zuweisungen

NovaLang unterstützt die VB.NET-orientierten Zuweisungsoperatoren.

```text
=
+=
-=
*=
/=
\=
^=
&=
<<=
>>=
```

Beispiel:

```vb
Dim counter As Integer = 10

counter += 5
counter -= 2
counter *= 3
```

Zusammengesetzte Zuweisungen müssen die festgelegte Operator-, Konvertierungs- und Überlaufsemantik einhalten.

Die Zielreferenz einer zusammengesetzten Zuweisung darf nur einmal ausgewertet werden.

## Werttypen

Werttypen verwenden Wertsemantik.

```vb
Dim a As Integer = 10
Dim b As Integer = a

b = 20
```

Ergebnis:

```text
a = 10
b = 20
```

Die Änderung von `b` verändert `a` nicht.

Dies gilt entsprechend für benutzerdefinierte `Structure`-Typen, sofern ihre Mitglieder keine separat referenzierten veränderlichen Objekte enthalten.

## Referenztypen

Referenzvariablen speichern eine kontrollierte Referenz auf eine Objektinstanz.

```vb
Dim person1 As New Person("Max")
Dim person2 As Person = person1
```

Beide Variablen können auf dasselbe Objekt verweisen.

```text
person1 ─┐
         ├── Person Object
person2 ─┘
```

Eine Zuweisung kopiert die Referenz, nicht automatisch das gesamte Objekt.

Die Lebensdauer des Objekts wird durch die NovaOS-Runtime abgesichert.

## Nothing

NovaLang verwendet `Nothing` nach VB.NET-orientierter Syntax.

```vb
Dim person As Person = Nothing
Dim alter As Integer? = Nothing
```

Bei Referenztypen bezeichnet `Nothing` das Fehlen einer gültigen Referenz.

Bei `Nullable(Of T)` bezeichnet es einen Wert ohne enthaltenen Nutzwert.

Nicht-nullbare Referenztypen dürfen keinen unzulässigen `Nothing`-Zustand annehmen.

Ungesicherte Zugriffe auf fehlende Referenzen müssen statisch erkannt oder kontrolliert zur Laufzeit abgefangen werden.

## Nullable-Variablen

```vb
Dim alter As Integer? = Nothing

alter = 30
```

Die Kurzform:

```vb
Integer?
```

entspricht:

```vb
Nullable(Of Integer)
```

Der Zugriff auf den enthaltenen Wert muss dessen Vorhandensein berücksichtigen.

```vb
If alter.HasValue Then
    Console.WriteLine(alter.Value)
End If
```

## Gültigkeitsbereiche

NovaLang verwendet lexikalische Gültigkeitsbereiche.

```text
Namespace Scope
    ↓
Type Scope
    ↓
Member Scope
    ↓
Block Scope
    ↓
Local Variable
```

Beispiel:

```vb
Public Sub Berechnen()

    Dim ergebnis As Integer = 10

    If ergebnis > 5 Then
        Dim meldung As String = "Größer als fünf"
        Console.WriteLine(meldung)
    End If

End Sub
```

`meldung` ist außerhalb ihres gültigen Blocks nicht verfügbar.

Die Regeln zur Namensüberschattung orientieren sich an VB.NET.

## Groß- und Kleinschreibung

NovaLang unterscheidet bei Bezeichnern nicht zwischen Groß- und Kleinschreibung.

```vb
Dim MeineVariable As Integer = 10

meinevariable = 20
MEINEVARIABLE = 30
```

Alle Schreibweisen beziehen sich auf dieselbe Variable.

Eine erneute Deklaration, die sich lediglich durch Groß- und Kleinschreibung unterscheidet, ist im selben Gültigkeitsbereich nicht zulässig.

## Lokale Variablen

Lokale Variablen werden innerhalb von Funktionen, Prozeduren und anderen zulässigen Blöcken deklariert.

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer

    Dim ergebnis As Integer = a + b

    Return ergebnis

End Function
```

Ihre Lebensdauer wird durch den jeweiligen Ausführungskontext bestimmt.

Eine Referenz auf lokale Daten darf nicht über deren gültige Lebensdauer hinaus unsicher verwendet werden.

## Felder

Felder gehören zu Klassen, Strukturen oder Modulen.

```vb
Public Class Counter

    Private _value As Integer = 0

    Public Sub Increment()

        _value += 1

    End Sub

End Class
```

Felder besitzen die Lebensdauer ihres zugehörigen Objekts beziehungsweise ihres definierten Modulkontexts.

## Shared-Variablen

`Shared` definiert Mitglieder, die einer Klasse statt einer einzelnen Instanz zugeordnet sind.

```vb
Public Class ApplicationState

    Public Shared InstanceCount As Integer = 0

End Class
```

Gemeinsam veränderlicher Zustand muss bei nebenläufigem Zugriff synchronisiert werden.

`Shared` erzeugt keine automatische Thread-Sicherheit.

## Static-Variablen

NovaLang unterstützt VB.NET-orientierte statische lokale Variablen.

```vb
Public Sub Zaehlen()

    Static aufrufe As Integer = 0

    aufrufe += 1

End Sub
```

Eine statische lokale Variable behält ihren Wert zwischen Funktionsaufrufen innerhalb ihres definierten Laufzeitkontexts.

Initialisierung, Lebensdauer und Nebenläufigkeit müssen eindeutig festgelegt sein.

Statische Variablen dürfen keine Sicherheits- oder Isolationsgrenzen zwischen Solutions oder Prozessen umgehen.

## Parameter

Parameter werden wie lokale, typisierte Bindungen behandelt.

```vb
Public Function Multiplizieren(a As Integer, b As Integer) As Integer

    Return a * b

End Function
```

NovaLang unterstützt:

```text
ByVal
ByRef
Optional
ParamArray
```

Die Übergabesemantik wird durch den jeweiligen Parametermodifikator bestimmt.

## ByVal

`ByVal` übergibt einen eigenen Parameterwert.

```vb
Public Sub Aendern(ByVal zahl As Integer)

    zahl = 100

End Sub
```

Die Zuweisung verändert nicht die Variable des Aufrufers.

Bei Referenztypen wird die Referenz als Wert übergeben. Änderungen am referenzierten Objekt können daher sichtbar bleiben.

## ByRef

`ByRef` ermöglicht kontrollierte Änderungen an der Variable des Aufrufers.

```vb
Public Sub Erhoehen(ByRef wert As Integer)

    wert += 1

End Sub
```

Beispiel:

```vb
Dim zahl As Integer = 10

Erhoehen(zahl)
```

Danach enthält `zahl` den Wert `11`.

`ByRef` muss Referenzlebensdauer, Aliasregeln und nebenläufige Zugriffe absichern.

## Variablen in Lambdas

NovaLang unterstützt das Erfassen lokaler Variablen durch Closures.

```vb
Dim faktor As Integer = 2

Dim berechnen = Function(x As Integer) x * faktor
```

Eingefangene Variablen müssen eine gültige Lebensdauer besitzen.

Bei veränderlichen Variablen gelten zusätzliche Regeln für gemeinsame Zugriffe.

Closures dürfen keine ungültigen Referenzen auf bereits freigegebene lokale Speicherbereiche enthalten.

## Variablen in Async-Funktionen

```vb
Public Async Function LadenAsync() As Task(Of String)

    Dim daten As String = Await Netzwerk.LesenAsync()

    Return daten

End Function
```

Lokale Variablen, deren Werte über einen `Await`-Punkt hinaus benötigt werden, müssen durch die Runtime sicher erhalten bleiben.

Ihre Lebensdauer ist an den zugehörigen Task-Kontext gebunden.

## Nebenläufigkeit

Gemeinsam veränderliche Variablen benötigen kontrollierte Synchronisation.

```text
Shared Mutable Variable
          ↓
Synchronization
          ↓
Authorized Access
```

Die Runtime muss Speicherordnung und Sichtbarkeit von Änderungen eindeutig definieren.

Sichere NovaLang-Konstrukte dürfen keine undefinierten Datenrennen ermöglichen.

## Atomare Variablen

Für geeignete primitive Typen können atomare Operationen bereitgestellt werden.

```vb
Dim counter As New Atomic(Of Integer)(0)

counter.Increment()
```

`Atomic(Of T)` ist ein beispielhafter Standardbibliothekstyp.

Die unterstützten Typen, Operationen und Speicherordnungen werden gesondert spezifiziert.

Atomare Variablen ersetzen nicht die Synchronisation komplexer gemeinsamer Zustände.

## Ressourcenvariablen

Variablen können Referenzen auf Systemressourcen enthalten.

```vb
Using stream As IStream = Datei.Oeffnen()

    stream.Write("NovaOS")

End Using
```

Die Ressource muss entsprechend ihrem Lebenszyklusvertrag freigegeben werden.

Eine Variablenreferenz allein begründet keine zusätzliche Systemberechtigung.

## Capability-Handles

Capability-Handles werden ausschließlich durch autorisierte Systemmechanismen bereitgestellt.

```vb
Public Function DatenLesen(reader As INetworkReader) As Result(Of String, Error)

    Return reader.Read()

End Function
```

Es gelten folgende Regeln:

- Eine Variable vom Typ `CapabilityID` besitzt nicht automatisch Berechtigungen.
- Ein Interface-Typ verleiht keine Berechtigung.
- `Nothing` darf nicht als gültiges Capability-Handle behandelt werden.
- Handles dürfen nicht durch Casts oder normale Objekterzeugung gefälscht werden.
- Kopieren und Weitergeben eines Handles unterliegen den Capability-Delegationsregeln.
- Das Ende einer Variablenlebensdauer darf keine fremden Capability-Handles ungültig machen.

## Logic-Graph-Variablen

NovaLang-Skripte innerhalb eines Logic Graph verwenden normale Variablendeklarationen.

```vb
Public Function Verarbeiten(eingabe As String) As String

    Dim ergebnis As String = eingabe & " verarbeitet"

    Return ergebnis

End Function
```

Eingaben werden durch typisierte Graph-Verbindungen bereitgestellt.

Custom Scripts dürfen keine eigenen Capability-Berechtigungen anfordern.

Variablen dürfen ausschließlich die bereitgestellten Daten und autorisierten Referenzen enthalten.

## Deklarative UI-Variablen

`.nui` verwendet dieselben Variablen- und Typregeln wie `.nova` und `.nlf`.

```vb
Dim titel As String = "NovaOS"

Dim fenster As New Window With {
    .Title = titel,
    .Width = 800,
    .Height = 500
}
```

Reaktive Zustandsvariablen und UI-Bindungen werden durch ergänzende Spezifikationen definiert.

Eine gewöhnliche Variable ist nicht automatisch reaktiv.

Die UI-Runtime muss zwischen einmaliger Wertübernahme und dauerhafter Datenbindung unterscheiden.

## Variablenidentität

Jede deklarierte Variable besitzt eine eindeutige Identität innerhalb ihres Gültigkeitsbereichs.

```text
VariableIdentity
├── ModuleIdentity
├── ScopeIdentity
├── DeclarationIdentity
├── TypeIdentity
└── LifetimeIdentity
```

Die Identität einer Variablen ist nicht mit der Identität ihres aktuellen Wertes gleichzusetzen.

Die interne Identitätsdarstellung muss nicht Bestandteil der öffentlichen Sprachsyntax sein.

## Speicherverwaltung

Variablen und referenzierte Objekte unterliegen dem sicheren Speichermodell von NovaOS.

Der Compiler und die Runtime müssen insbesondere verhindern:

- Zugriff auf ungültige Speicherbereiche
- Use-after-free
- Ungültige Referenzen
- Nicht initialisierte Speicherzugriffe
- Unzulässige Typumwandlungen
- Ungesicherte gemeinsame Speicherzugriffe
- Unkontrollierte Ressourcenlebensdauern

Die konkrete Speicherverwaltungsstrategie bleibt vom Ausführungsbackend abhängig.

## Ressourcenlimits

Variablenspeicher unterliegt dem Execution Contract.

```text
Execution Contract
├── Stack Budget
├── Heap Budget
├── Object Budget
├── Resource Budget
└── Lifetime Constraints
```

Speicherüberschreitungen müssen kontrolliert behandelt werden.

## Introspection

NovaLang muss autorisiert folgende Variableninformationen bereitstellen können:

```text
VariableName
VariableType
VariableScope
VariableLifetime
Mutability
InitializationState
SourceLocation
```

Der aktuelle Wert darf nur offengelegt werden, wenn der aufrufende Kontext dazu berechtigt ist.

Capability-Handles, Zugangsdaten und andere geschützte Inhalte müssen besonders behandelt werden.

## Normative Anforderungen

1. NovaLang MUSS Variablen mit `Dim` deklarieren.
2. NovaLang MUSS Konstanten mit `Const` deklarieren.
3. Explizite Typangaben MÜSSEN `As` verwenden.
4. Lokale Typinferenz MUSS unterstützt werden.
5. Bezeichner MÜSSEN ohne Unterscheidung der Groß- und Kleinschreibung aufgelöst werden.
6. Jede Variable MUSS einen definierten statischen Typ besitzen.
7. Jede Variable MUSS einen definierten Gültigkeitsbereich besitzen.
8. Variablen MÜSSEN vor ihrer Verwendung einen gültigen Initialisierungszustand besitzen.
9. Nicht initialisierte Speicherinhalte DÜRFEN nicht zugänglich sein.
10. Zuweisungen MÜSSEN typkompatibel sein.
11. `Const`-Werte DÜRFEN nach ihrer Deklaration nicht verändert werden.
12. `ReadOnly`-Felder DÜRFEN nur innerhalb ihrer definierten Initialisierungsphase zugewiesen werden.
13. Werttypen und Referenztypen MÜSSEN ihre jeweilige Kopiersemantik einhalten.
14. `Nothing` MUSS gemäß den Nullability-Regeln behandelt werden.
15. `ByVal` und `ByRef` MÜSSEN sichere und eindeutig definierte Übergabesemantiken besitzen.
16. `Shared`- und `Static`-Variablen MÜSSEN kontrollierte Lebensdauerregeln besitzen.
17. Eingefangene Variablen in Closures MÜSSEN lebensdauersicher sein.
18. Lokale Variablen in Async-Funktionen MÜSSEN über erforderliche Suspendierungspunkte erhalten bleiben.
19. Gemeinsam veränderliche Variablen MÜSSEN sicher synchronisierbar sein.
20. Variablendeklarationen DÜRFEN keine Capability-Berechtigungen erzeugen.
21. Capability-Handles DÜRFEN nicht durch Variablenzuweisungen oder Typkonvertierungen gefälscht werden.
22. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben grundlegenden Variablenregeln verwenden.
23. Variablen und referenzierte Objekte MÜSSEN den NovaOS-Speichersicherheitsregeln entsprechen.
24. Variablenspeicher MUSS durch Execution Contracts begrenzbar sein.
25. Variableninformationen MÜSSEN kontrolliert introspektierbar sein.
26. Die Implementierung MUSS unabhängig von der .NET-Runtime funktionieren.

## Abhängigkeiten

- `NPSPEC-NOVALANG-CORE-0001`
- `NPSPEC-NOVALANG-SYNTAX-0001`
- `NPSPEC-NOVALANG-GRAMMAR-0001`
- `NPSPEC-NOVALANG-LEXER-0001`
- `NPSPEC-NOVALANG-TYPES-0001`
- `NPSPEC-NOVALANG-SEMANTICS-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`

## Ergebnis

NovaLang besitzt ein einheitliches, VB.NET-orientiertes Variablenmodell mit `Dim`, `Const`, `ReadOnly`, `Shared`, `Static`, `ByVal` und `ByRef`.

Variablen werden statisch typisiert, sicher initialisiert und innerhalb eindeutig definierter Gültigkeitsbereiche und Lebensdauern verwaltet.

Das Modell unterstützt klassische Programme, Capabilities, Solutions, Logic Graph und deklarative Benutzeroberflächen, ohne die Speicher- und Berechtigungssicherheit von NovaOS zu beeinträchtigen.

Die Implementierung bleibt vollständig unabhängig von der .NET-Runtime.
