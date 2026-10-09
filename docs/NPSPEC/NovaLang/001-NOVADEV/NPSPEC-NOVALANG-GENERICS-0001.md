
# NPSPEC-NOVALANG-GENERICS-0001 – NovaLang Generics

## Status

Angenommen

## Kategorie

NovaLang / Generische Typen

## Zweck

Definiert generische Klassen, Strukturen, Interfaces und Funktionen zur typsicheren Wiederverwendung von Code. Die Syntax orientiert sich möglichst exakt an VB.NET.

## Generische Typen

Generische Typparameter werden mit `Of` deklariert.

```vb
Public Class Container(Of T)

    Public Property Value As T

    Public Sub New(value As T)
        Me.Value = value
    End Sub

End Class

Dim box As New Container(Of Integer)(42)
```

Generische Typen werden zur Übersetzungszeit geprüft und benötigen keine dynamische Typisierung.

## Generische Funktionen

```vb
Public Function Identitaet(Of T)(wert As T) As T
    Return wert
End Function

Dim zahl As Integer = Identitaet(42)
```

Typargumente können explizit angegeben oder aus den Funktionsargumenten abgeleitet werden.

## Constraints

| Constraint | Bedeutung |
|---|---|
| `As Class` | Referenztyp |
| `As Structure` | Werttyp |
| `As New` | Öffentlicher parameterloser Konstruktor |
| `As InterfaceName` | Implementiert das Interface |
| `As BaseClass` | Erbt von der Basisklasse |
| `As { ... }` | Kombination zulässiger Constraints |

```vb
Public Class Repository(Of T As {Class, New})

    Public Function Create() As T
        Return New T()
    End Function

End Class
```

Constraints werden statisch geprüft.

## Generische Interfaces und Vererbung

```vb
Public Interface IStorage(Of T)
    Function Load() As T
    Sub Save(value As T)
End Interface
```

Generische Typen dürfen andere generische Typen implementieren oder erweitern, sofern sämtliche Constraints erfüllt sind.

## Typidentität und Laufzeit

- `Container(Of Integer)` und `Container(Of String)` sind unterschiedliche konstruierte Typen.
- Generische Typparameter besitzen eine eindeutige Typidentität.
- Generics unterstützen Wert- und Referenztypen.
- Die Runtime muss erforderliche Typinformationen erhalten.
- Die Implementierung darf Spezialisierung oder gemeinsamen Maschinencode verwenden.
- Unterschiedliche Implementierungsstrategien müssen semantisch äquivalent bleiben.

## Normative Anforderungen

1. NovaLang MUSS Generics für Klassen, Strukturen, Interfaces, Delegates und Funktionen unterstützen.
2. Generische Parameter MÜSSEN mit `Of` deklariert werden.
3. Constraints MÜSSEN statisch geprüft werden.
4. Typinferenz MUSS bei generischen Funktionsaufrufen unterstützt werden.
5. Generische Typen MÜSSEN eindeutige Typidentitäten besitzen.
6. Generics DÜRFEN keine unsicheren Typumwandlungen oder Capability-Rechteausweitungen ermöglichen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben Generics-Regeln verwenden.

## Ergebnis

NovaLang unterstützt VB.NET-orientierte Generics mit statischer Typprüfung, Constraints und Typinferenz. Die Implementierung erfolgt nativ und unabhängig von der .NET-Runtime.
