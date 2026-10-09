
# NPSPEC-NOVALANG-FUNCTIONS-0001 – NovaLang Functions and Procedures

## Status

Angenommen

## Kategorie

NovaLang / Funktionen und Prozeduren

## Zweck

Definiert Deklaration, Parameterübergabe, Rückgabewerte und Aufrufverhalten von Funktionen und Prozeduren. Die Syntax orientiert sich möglichst exakt an VB.NET.

## Deklaration

NovaLang unterscheidet Funktionen mit Rückgabewert (`Function`) und Prozeduren ohne Rückgabewert (`Sub`).

```vb
Public Function Addieren(a As Integer, b As Integer) As Integer
    Return a + b
End Function

Public Sub Anzeigen(text As String)
    Console.WriteLine(text)
End Sub
```

Funktionen und Prozeduren können innerhalb von Modulen, Klassen und Strukturen definiert werden.

## Parameter

| Modifikator | Bedeutung |
|---|---|
| `ByVal` | Übergabe eines Parameterwerts (Standard) |
| `ByRef` | Veränderbarer Zugriff auf die Variable des Aufrufers |
| `Optional` | Optionaler Parameter mit Standardwert |
| `ParamArray` | Variable Anzahl typisierter Argumente |

```vb
Public Function Multiplizieren(
    a As Integer,
    Optional faktor As Integer = 2
) As Integer
    Return a * faktor
End Function
```

Argumente können positionsbasiert oder benannt übergeben werden.

## Rückgabewerte

- `Return` beendet die Funktion und liefert den Rückgabewert.
- Rückgabewerte müssen dem deklarierten Typ entsprechen.
- `Sub` besitzt keinen Rückgabewert.
- Alle regulär erreichbaren Funktionsausgänge müssen einen definierten Rückgabewert liefern.
- Fehler können über Exceptions oder `Result(Of T, E)` behandelt werden.

## Überladung und Generics

Mehrere Funktionen dürfen denselben Namen besitzen, wenn ihre Signaturen eindeutig unterscheidbar sind.

```vb
Public Function Maximum(Of T As IComparable(Of T))(
    a As T,
    b As T
) As T
    Return If(a.CompareTo(b) >= 0, a, b)
End Function
```

Generische Typparameter und Constraints werden statisch geprüft. Eine Überladung allein anhand des Rückgabetyps ist nicht zulässig.

## Lambdas und Delegates

```vb
Dim verdoppeln As Func(Of Integer, Integer) =
    Function(x As Integer) x * 2
```

Funktionen können als typisierte Delegates übergeben werden. Closures dürfen Variablen ihres umgebenden Gültigkeitsbereichs sicher erfassen.

## Asynchrone Funktionen

```vb
Public Async Function LadenAsync() As Task(Of String)
    Return Await Datenquelle.LesenAsync()
End Function
```

`Async` und `Await` verwenden die NovaOS-Runtime und Structured Concurrency. Asynchrone Funktionen müssen Cancellation und Ressourcenlebensdauern berücksichtigen.

## Aufrufsemantik

- Argumente werden grundsätzlich von links nach rechts ausgewertet.
- Parameter und Rückgabewerte werden statisch typgeprüft.
- Rekursive Aufrufe sind zulässig und unterliegen Ressourcenlimits.
- Sichtbarkeit wird durch `Public`, `Private`, `Protected` und `Friend` gesteuert.
- `Shared` kennzeichnet Funktionen ohne erforderliche Objektinstanz.
- Funktionen erhalten ausschließlich die explizit oder durch ihren autorisierten Ausführungskontext bereitgestellten Capabilities.
- Custom Scripts im Logic Graph dürfen keine zusätzlichen Systemberechtigungen anfordern.

## Normative Anforderungen

1. NovaLang MUSS `Function` und `Sub` unterstützen.
2. Parameter MÜSSEN typisiert sein und `ByVal`, `ByRef`, `Optional` und `ParamArray` unterstützen.
3. Funktionen MÜSSEN gültige Rückgabewerte liefern.
4. Überladungen und generische Funktionen MÜSSEN statisch auflösbar sein.
5. Lambdas, Delegates und Rekursion MÜSSEN unterstützt werden.
6. Asynchrone Funktionen MÜSSEN das NovaOS-Ausführungsmodell verwenden.
7. Funktionsaufrufe DÜRFEN keine impliziten Capability-Berechtigungen erzeugen.
8. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe Funktionssemantik verwenden.

## Ergebnis

NovaLang bietet VB.NET-orientierte Funktionen und Prozeduren mit statischer Typprüfung, Generics, Überladung, Lambdas und asynchroner Ausführung, vollständig unabhängig von der .NET-Runtime.
