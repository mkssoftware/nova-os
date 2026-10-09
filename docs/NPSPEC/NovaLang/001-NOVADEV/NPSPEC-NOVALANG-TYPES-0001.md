
# NPSPEC-NOVALANG-TYPES-0001 – NovaLang Type System

## Status

Angenommen

## Kategorie

NovaLang / Typsystem

## Zweck

Diese Spezifikation definiert das native Typsystem von NovaLang.

NovaLang übernimmt die vertrauten Typdeklarationen und Typnamen von Visual Basic .NET (VB.NET), verwendet jedoch eine eigenständige, statisch geprüfte und speichersichere Typimplementierung für NovaOS.

Das Typsystem bildet die gemeinsame Grundlage für:

- Systemkomponenten und Capabilities
- Klassische Programme
- Solutions und Nova Logic Files
- Logic Graph
- Deklarative Benutzeroberflächen
- Asynchrone und nebenläufige Verarbeitung
- Semantische NovaOS-Objekte

NovaLang benötigt weder die .NET-Runtime noch die .NET Base Class Library.

## Grundprinzipien

1. **VB.NET-Kompatibilität:** Typnamen, Deklarationen und Generics orientieren sich möglichst exakt an VB.NET.
2. **Statische Typprüfung:** Typfehler werden grundsätzlich vor der Ausführung erkannt.
3. **Kontrollierte Typinferenz:** Eindeutige Typen können automatisch ermittelt werden.
4. **Speichersicherheit:** Sichere Sprachkonstrukte verhindern ungültige Speicherzugriffe.
5. **Explizite Konvertierungen:** Potenziell verlustbehaftete Konvertierungen werden kontrolliert.
6. **Semantische Typidentität:** Fachlich unterschiedliche Typen bleiben unterscheidbar.
7. **Capability-Sicherheit:** Typreferenzen erzeugen keine Systemberechtigungen.
8. **Generische Programmierung:** Typisierte, wiederverwendbare Komponenten werden unterstützt.
9. **Einheitlichkeit:** `.nova`, `.nlf` und `.nui` verwenden dasselbe Typsystem.
10. **Plattformunabhängigkeit:** Typsemantik und Wertebereiche sind unabhängig von CPU und Betriebssystem.

## Architektur

```text
NovaLang Source
       ↓
Parser / AST
       ↓
Name Resolution
       ↓
Type Resolution
       ↓
Type Inference
       ↓
Type Checking
       ↓
Semantic Validation
       ↓
Typed Nova IR
       ↓
Compiler / Runtime
```

Die Typprüfung ist unabhängig vom gewählten Ausführungsbackend.

## Typmodell

```text
TypeDescriptor
├── TypeID
├── Name
├── Namespace
├── TypeKind
├── BaseType
├── Interfaces[]
├── GenericParameters[]
├── Constraints[]
├── Members[]
├── Nullability
├── Mutability
├── SemanticIdentity
├── Visibility
└── Version
```

`TypeID` bezeichnet die eindeutige Typidentität.

Der Anzeigename eines Typs ist nicht allein für seine Identität maßgeblich.

## Primitive Datentypen

NovaLang verwendet folgende VB.NET-orientierte Typnamen:

| Typ | Bedeutung | Größe |
|---|---|---|
| `Boolean` | Wahrheitswert | Logisch True/False |
| `Byte` | Vorzeichenlose Ganzzahl | 8 Bit |
| `SByte` | Ganzzahl mit Vorzeichen | 8 Bit |
| `Short` | Ganzzahl mit Vorzeichen | 16 Bit |
| `UShort` | Vorzeichenlose Ganzzahl | 16 Bit |
| `Integer` | Ganzzahl mit Vorzeichen | 32 Bit |
| `UInteger` | Vorzeichenlose Ganzzahl | 32 Bit |
| `Long` | Ganzzahl mit Vorzeichen | 64 Bit |
| `ULong` | Vorzeichenlose Ganzzahl | 64 Bit |
| `Single` | Gleitkommazahl | 32 Bit |
| `Double` | Gleitkommazahl | 64 Bit |
| `Decimal` | Dezimalzahl | 128-Bit-Modell |
| `Char` | UTF-16-Codeunit | 16 Bit |
| `String` | Unicode-Zeichenkette | Variabel |
| `Object` | Allgemeiner Objekttyp | Laufzeitabhängig |

Die angegebenen Größen beziehen sich auf die logische Typrepräsentation.

Speicherlayout, Alignment und ABI werden separat definiert.

`Decimal` besitzt ein festgelegtes Dezimalmodell; seine numerische Präzision und sein Wertebereich werden in einer ergänzenden Spezifikation normiert.

## Typdeklarationen

NovaLang verwendet `As` zur Angabe des Datentyps.

```vb
Dim name As String = "NovaOS"
Dim version As Integer = 1
Dim temperatur As Double = 21.5
Dim aktiviert As Boolean = True
```

Der Datentyp einer Variablen bleibt nach der Deklaration grundsätzlich erhalten.

```vb
Dim zahl As Integer = 10

zahl = 20
```

Eine inkompatible Zuweisung wird vom Compiler abgelehnt.

## Typinferenz

NovaLang unterstützt lokale Typinferenz nach VB.NET-Vorbild.

```vb
Dim name = "NovaOS"
Dim version = 1
Dim aktiviert = True
```

Der Compiler bestimmt die Typen anhand der Initialisierung.

```text
name       → String
version    → Integer
aktiviert  → Boolean
```

Typinferenz darf nicht zu unkontrollierter dynamischer Typisierung führen.

Öffentliche Schnittstellen sollen ihre Parametertypen und Rückgabetypen explizit deklarieren.

## Option Strict

NovaLang unterstützt eine VB.NET-orientierte `Option Strict`-Direktive.

```vb
Option Strict On
```

Für NovaLang gilt standardmäßig:

```text
Option Strict On
```

Damit werden insbesondere verhindert:

- Implizite verlustbehaftete Konvertierungen
- Unkontrollierte Late-Binding-Aufrufe
- Unsichere Typannahmen

Eine Kompatibilitätsoption `Option Strict Off` darf ausschließlich in ausdrücklich zugelassenen Ausführungskontexten existieren.

Sie darf keine Speicher- oder Capability-Sicherheitsgarantien außer Kraft setzen.

## Option Infer

```vb
Option Infer On
```

`Option Infer On` erlaubt kontrollierte lokale Typinferenz.

```vb
Dim ergebnis = 10 + 20
```

Mit deaktivierter Typinferenz müssen nicht explizit typisierte Deklarationen nach den festgelegten Sprachregeln behandelt werden.

Ein stillschweigender Rückfall auf unsichere dynamische Typisierung ist nicht zulässig.

## Werttypen

Werttypen besitzen definierte Wertsemantik.

Dazu gehören:

```text
Boolean
Integer
Long
Double
Decimal
Char
Structure
Enum
```

Beispiel:

```vb
Dim a As Integer = 10
Dim b As Integer = a

b = 20
```

Die Änderung von `b` verändert `a` nicht.

## Referenztypen

Referenztypen besitzen eine definierte Objektidentität.

Dazu gehören insbesondere:

```text
Class
String
Array
Delegate
Interface References
```

Beispiel:

```vb
Dim person1 As New Person("Max")
Dim person2 As Person = person1
```

Beide Variablen können auf dieselbe Objektinstanz verweisen.

Referenzidentität und Wertgleichheit bleiben unterschiedliche Konzepte.

## Structures

NovaLang verwendet `Structure` für benutzerdefinierte Werttypen.

```vb
Public Structure Position

    Public X As Double
    Public Y As Double

End Structure
```

Beispiel:

```vb
Dim position1 As New Position With {
    .X = 10,
    .Y = 20
}

Dim position2 As Position = position1
```

Strukturen müssen eine eindeutig definierte Kopier- und Lebensdauerseman­tik besitzen.

## Classes

Klassen verwenden Referenzsemantik.

```vb
Public Class Person

    Public Property Name As String

    Public Sub New(name As String)
        Me.Name = name
    End Sub

End Class
```

NovaLang unterstützt:

- Kapselung
- Vererbung
- Polymorphie
- Konstruktoren
- Eigenschaften
- Methoden
- Interfaces
- Generics

Die Objektlebensdauer wird durch die NovaOS-Runtime kontrolliert.

## Enumerationen

```vb
Public Enum SystemState

    Starting
    Running
    Suspended
    Stopped

End Enum
```

Enumerationen besitzen eine eigenständige Typidentität.

Ein Enum darf nicht ohne definierte Konvertierung mit einem beliebigen Ganzzahltyp gleichgesetzt werden.

## Interfaces

Interfaces definieren überprüfbare Typverträge.

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

Die Implementierung muss sämtliche erforderlichen Schnittstellenverträge erfüllen.

## Generics

NovaLang verwendet die VB.NET-Syntax `Of`.

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

Generics werden statisch typgeprüft.

## Generische Constraints

NovaLang unterstützt VB.NET-orientierte Constraints.

```vb
Public Class Repository(Of T As Class)

End Class
```

Unterstützte Constraint-Kategorien umfassen:

```text
Class
Structure
New
Interface
Base Class
Semantic Type Constraint
```

Zusätzliche NovaOS-Constraints dürfen die Typsicherheit erweitern, aber nicht umgehen.

## Arrays

Arrays verwenden die VB.NET-orientierte Syntax.

```vb
Dim zahlen As Integer() = {1, 2, 3, 4, 5}
```

Mehrdimensionale Arrays werden unterstützt.

```vb
Dim matrix(2, 2) As Integer
```

Arraygrenzen und Indexzugriffe müssen kontrolliert geprüft werden.

Speicherunsichere Indexzugriffe sind im sicheren Sprachkern nicht zulässig.

## Collections

NovaLang unterstützt generische Collections.

```vb
Dim namen As New List(Of String)

namen.Add("Anna")
namen.Add("Max")
```

Grundlegende Collection-Typen:

```text
List(Of T)
Dictionary(Of TKey, TValue)
HashSet(Of T)
Queue(Of T)
Stack(Of T)
```

Die konkreten Implementierungen werden durch die NovaLang-Standardbibliothek bereitgestellt.

## Tupel

NovaLang unterstützt typisierte Tupel.

```vb
Dim position As (X As Integer, Y As Integer) = (10, 20)
```

Tupel besitzen eine definierte Elementreihenfolge und Typstruktur.

## Nullable-Typen

NovaLang unterstützt `Nullable(Of T)` und die Kurzform `T?`.

```vb
Dim alter As Integer? = Nothing
```

Entspricht:

```vb
Dim alter As Nullable(Of Integer) = Nothing
```

Ein Nullable-Wert besitzt einen expliziten Zustand:

```text
HasValue
Value
```

Der Zugriff auf einen nicht vorhandenen Wert muss kontrolliert behandelt werden.

## Nullbarkeit von Referenztypen

NovaLang unterscheidet zwischen gültigen Referenzen und potenziell fehlenden Referenzen.

```vb
Dim person As Person = New Person("Max")
```

Die sichere Sprachsemantik darf nicht voraussetzen, dass eine Referenz immer gültig ist, wenn dies nicht statisch oder zur Laufzeit abgesichert wurde.

Nullbarkeitsanalyse muss insbesondere erkennen:

- Mögliche Nothing-Zuweisungen
- Ungesicherte Memberzugriffe
- Nicht initialisierte Referenzen
- Ungültige Rückgabewerte

Die konkrete Annotation nicht-nullbarer und nullable Referenztypen wird durch die ergänzende Nullability-Spezifikation definiert.

## Nothing

NovaLang verwendet `Nothing` entsprechend der VB.NET-orientierten Syntax.

```vb
Dim person As Person = Nothing
```

`Nothing` wird anhand des Zieltyps interpretiert.

Es darf keine ungeprüften Nullreferenzzugriffe ermöglichen.

Die VB.NET-Kompatibilität wird dort eingeschränkt, wo andernfalls die Speichersicherheit verletzt würde.

## Strings

`String` repräsentiert Unicode-Text.

```vb
Dim text As String = "Hallo NovaOS"
```

Die Sprachsyntax orientiert sich an VB.NET.

```vb
Dim nachricht As String = "Hallo " & text
```

Die interne Speicherrepräsentation darf von .NET abweichen.

NovaLang muss Unicode-Skalarwerte, Grapheme, Textsegmentierung und Normalisierung über definierte Textschnittstellen unterstützen.

## Char

`Char` besitzt eine VB.NET-kompatible UTF-16-Codeunit-Semantik.

```vb
Dim zeichen As Char = "A"c
```

Ein `Char` ist nicht zwangsläufig ein vollständiges Unicode-Graphem.

Für vollständige Unicode-Skalarwerte und Grapheme müssen geeignete Texttypen beziehungsweise Textoperationen bereitgestellt werden.

## Typkonvertierungen

NovaLang unterscheidet:

```text
Implicit Widening Conversion
Explicit Narrowing Conversion
Checked Conversion
Forbidden Conversion
```

Beispiel:

```vb
Dim zahl As Integer = 42

Dim gross As Long = CLng(zahl)
```

Explizite Konvertierungen orientieren sich an VB.NET.

```text
CBool
CByte
CChar
CDate
CDbl
CDec
CInt
CLng
CObj
CSByte
CShort
CSng
CStr
CUInt
CULng
CUShort
CType
DirectCast
TryCast
```

Die Funktionen müssen durch die NovaLang-Runtime beziehungsweise Standardbibliothek bereitgestellt werden.

Ungültige Konvertierungen müssen kontrolliert fehlschlagen.

## Numerische Sicherheit

NovaLang muss Ganzzahlüberläufe, Division durch null und ungültige numerische Konvertierungen kontrolliert behandeln.

Für Gleitkommazahlen müssen insbesondere folgende Zustände eindeutig definiert sein:

```text
NaN
PositiveInfinity
NegativeInfinity
PositiveZero
NegativeZero
```

Numerische Operationen müssen eine plattformunabhängig spezifizierte Semantik besitzen.

## Result-Typen

NovaLang erweitert das VB.NET-Typsystem um typisierte Fehlerergebnisse.

```vb
Public Function Laden() As Result(Of String, Error)

    Return Result.Ok("Daten")

End Function
```

Ein Result besitzt zwei mögliche Zustände:

```text
Result(Of T, E)
├── Ok(T)
└── Err(E)
```

Die konkrete Result-API wird durch die NovaLang-Standardbibliothek definiert.

Result-Typen ersetzen nicht automatisch sämtliche Exceptions.

## Delegates

NovaLang unterstützt typisierte Delegates.

```vb
Public Delegate Function Berechnung(a As Integer, b As Integer) As Integer
```

Delegates dürfen auf kompatible Funktionen und Methoden verweisen.

Die Lebensdauer referenzierter Objekte muss abgesichert sein.

## Lambdas

```vb
Dim verdoppeln As Func(Of Integer, Integer) =
    Function(x As Integer) x * 2
```

Lambda-Ausdrücke werden anhand ihrer Parameter, Rückgabewerte und ihres Kontexts typgeprüft.

Closures müssen eine sichere Lebensdauer ihrer eingefangenen Variablen gewährleisten.

## Async-Typen

NovaLang unterstützt asynchrone Ergebnistypen.

```text
Task
Task(Of T)
```

Beispiel:

```vb
Public Async Function DatenLadenAsync() As Task(Of String)

    Dim daten As String = Await Netzwerk.LesenAsync()

    Return daten

End Function
```

Diese Typen gehören zur NovaOS-Runtime und benötigen keine .NET-Runtime.

Asynchrone Operationen unterliegen Structured Concurrency, Cancellation und Execution Contracts.

## Semantische Typen

NovaLang integriert die semantischen Typen von NovaOS.

Dazu gehören:

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
Dim capability As CapabilityID
```

Semantisch unterschiedliche Typen dürfen nicht automatisch austauschbar sein.

```text
ObjectID ≠ CapabilityID
```

Auch dann nicht, wenn ihre interne Repräsentation identisch ist.

## Capability-Typen

Capability-Schnittstellen werden als typisierte Interfaces verwendet.

```vb
Public Function DatenLesen(reader As INetworkReader) As Result(Of String, Error)

    Return reader.Read()

End Function
```

Ein Interface beschreibt nur den Vertrag.

Die tatsächliche Berechtigung entsteht durch ein autorisiertes Capability-Handle.

```text
Capability Interface ≠ Capability Authority
```

Capability-Handles dürfen nicht durch Casts, Reflection oder Objekterzeugung gefälscht werden.

## Logic-Graph-Typisierung

Logic Graph verwendet exakt dasselbe Typsystem wie NovaLang.

```text
Capability Output(Of T)
          ↓
Typed Connection(Of T)
          ↓
NovaLang Input(Of T)
          ↓
Typed Result(Of R)
```

Nicht kompatible Verbindungen müssen vor der Ausführung erkannt werden.

Beispiel:

```vb
Public Function Verarbeiten(eingabe As String) As String

    Return eingabe & " verarbeitet"

End Function
```

Custom Scripts dürfen keine eigenen Capability-Berechtigungen anfordern.

Die Typisierung beschreibt Daten und Schnittstellen, nicht automatisch die Berechtigung zu deren Verwendung.

## Deklarative UI-Typisierung

`.nui` verwendet dieselben Typen wie `.nova` und `.nlf`.

Beispiel:

```vb
Dim fenster As New Window With {
    .Title = "NovaOS",
    .Width = 800,
    .Height = 500
}
```

Eigenschaften, Bindungen und Ereignisse müssen statisch überprüfbar sein.

Ein separater UI-Typdialekt ist nicht zulässig.

## Mutabilität

NovaLang unterscheidet:

```text
Mutable Variable
ReadOnly Field
Constant
Mutable Object
Immutable Object
Shared State
```

Beispiel:

```vb
Dim counter As Integer = 0

Const MaxCount As Integer = 100
```

`ReadOnly` und `Const` besitzen unterschiedliche Semantik.

Die Veränderbarkeit einer Referenz ist von der Veränderbarkeit ihres Zielobjekts zu unterscheiden.

## ByVal und ByRef

NovaLang unterstützt die VB.NET-orientierten Parametermodifikatoren.

```vb
Public Sub Erhoehen(ByRef wert As Integer)

    wert += 1

End Sub
```

`ByRef` darf keine ungültigen Referenzen erzeugen.

Die Runtime muss Referenzlebensdauer, Schreibzugriff und Nebenläufigkeit kontrollieren.

## Speicherverwaltung

Das Typsystem unterstützt eine sichere Speicherverwaltung.

Es muss verhindern beziehungsweise kontrolliert absichern:

- Use-after-free
- Double-free
- Ungültige Referenzen
- Unkontrollierte Pointerarithmetik
- Ungültige Typumwandlungen
- Unzulässige gemeinsame Speicherzugriffe
- Datenrennen in sicheren Sprachkonstrukten

Die konkrete Speicherverwaltung darf je nach Ausführungsumgebung variieren.

## Typidentität

Ein Typ besitzt eine stabile Identität.

```text
TypeIdentity
├── Namespace
├── TypeName
├── ModuleIdentity
├── GenericArguments
├── SemanticIdentity
└── ContractVersion
```

Typidentität darf nicht allein aus einem Anzeigenamen abgeleitet werden.

## Typkompatibilität

NovaLang unterscheidet:

```text
Exact Type Match
Assignable Type
Interface Compatibility
Generic Compatibility
Semantic Compatibility
Explicit Conversion
Incompatible Type
```

Kompatibilität wird durch definierte Typregeln bestimmt.

## Typversionierung

Öffentliche Typen und Interfaces müssen versionierbar sein.

Änderungen an folgenden Eigenschaften können die Kompatibilität beeinflussen:

```text
Field Layout
Method Signature
Property Type
Interface Contract
Generic Constraint
Semantic Identity
Nullability Contract
```

Binäre ABI-Kompatibilität und semantische Typkompatibilität werden getrennt bewertet.

## Introspection

Das Typsystem muss folgende Informationen bereitstellen können:

```text
TypeID
TypeName
TypeKind
Namespace
BaseType
Interfaces
Members
GenericArguments
Constraints
Nullability
SemanticIdentity
ContractVersion
```

Reflection und Introspection dürfen keine Capability-Berechtigungen umgehen.

## Normative Anforderungen

1. NovaLang MUSS VB.NET-orientierte Typnamen und Typdeklarationen verwenden.
2. Typdeklarationen MÜSSEN die `As`-Syntax unterstützen.
3. Generics MÜSSEN die `Of`-Syntax verwenden.
4. NovaLang MUSS statische Typprüfung unterstützen.
5. Kontrollierte lokale Typinferenz MUSS unterstützt werden.
6. `Option Strict On` MUSS der sichere Standard sein.
7. Primitive Typen MÜSSEN plattformunabhängig definierte Wertebereiche besitzen.
8. Werttypen und Referenztypen MÜSSEN eindeutig unterscheidbar sein.
9. Klassen, Strukturen, Interfaces, Enumerationen und Delegates MÜSSEN unterstützt werden.
10. Arrays, Collections, Tupel und Generics MÜSSEN typisiert sein.
11. Nullable-Typen MÜSSEN unterstützt werden.
12. Ungültige Nullreferenzzugriffe MÜSSEN kontrolliert verhindert werden.
13. Implizite verlustbehaftete Konvertierungen DÜRFEN im sicheren Standardmodus nicht erfolgen.
14. Numerische Überläufe und ungültige Konvertierungen MÜSSEN kontrolliert behandelbar sein.
15. Semantische Typidentitäten MÜSSEN erhalten bleiben.
16. Capability-Typen DÜRFEN keine Berechtigungen erzeugen.
17. Capability-Handles DÜRFEN nicht durch Typkonvertierungen gefälscht werden.
18. Logic Graph und NovaLang MÜSSEN dasselbe Typsystem verwenden.
19. `.nova`, `.nlf` und `.nui` DÜRFEN keine unterschiedlichen Typsemantiken besitzen.
20. `Async` und `Await` MÜSSEN mit den nativen NovaOS-Tasktypen kompatibel sein.
21. Speicher- und Referenzsicherheit MÜSSEN standardmäßig gewährleistet werden.
22. Öffentliche Typverträge MÜSSEN versionierbar sein.
23. Typinformationen MÜSSEN kontrolliert introspektierbar sein.
24. Das Typsystem MUSS unabhängig von der .NET-Runtime implementierbar sein.

## Abhängigkeiten

- `NPSPEC-NOVALANG-CORE-0001`
- `NPSPEC-NOVALANG-SYNTAX-0001`
- `NPSPEC-NOVALANG-GRAMMAR-0001`
- `NPSPEC-NOVALANG-LEXER-0001`
- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-TEXT-UNICODE-0001`
- `NPSPEC-TEXT-UTF8-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`

## Ergebnis

NovaLang besitzt ein statisches, speichersicheres und erweiterbares Typsystem mit einer möglichst VB.NET-kompatiblen Syntax.

Die Sprache verwendet vertraute Typnamen, `As`-Deklarationen, `Of`-Generics, Klassen, Strukturen, Interfaces und Delegates.

Gleichzeitig integriert sie moderne NovaOS-Konzepte wie semantische Typidentitäten, typisierte Capability-Schnittstellen, Structured Concurrency und sichere Logic-Graph-Verbindungen.

Das Typsystem bleibt unabhängig von .NET und bildet die gemeinsame Grundlage für sämtliche NovaLang-Anwendungsbereiche.
