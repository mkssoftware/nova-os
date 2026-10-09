
# NPSPEC-NOVALANG-OBJECTS-0001 – NovaLang Objects

## Status

Angenommen

## Kategorie

NovaLang / Objektorientierung

## Zweck

Definiert Klassen, Objekte, Vererbung, Kapselung und Polymorphie. Die Syntax orientiert sich möglichst exakt an VB.NET und verwendet das native Objektsystem von NovaOS.

## Klassen und Objekte

Klassen werden mit `Class` definiert und mit `New` instanziiert.

```vb
Public Class Person

    Public Property Name As String

    Public Sub New(name As String)
        Me.Name = name
    End Sub

    Public Function Begruessen() As String
        Return "Hallo " & Name
    End Function

End Class

Dim person As New Person("Max")
```

Jede Instanz besitzt eine eigene Objektidentität und einen definierten Lebenszyklus.

## Objektmodell

| Element | Bedeutung |
|---|---|
| `Class` | Referenztyp mit Objektidentität |
| `Structure` | Werttyp mit Kopiersemantik |
| `Interface` | Vertrag für Mitglieder |
| `Enum` | Benannte Konstantenwerte |
| `MustInherit` | Abstrakte Klasse |
| `NotInheritable` | Nicht vererbbare Klasse |
| `Shared` | Klassenbezogenes Mitglied |
| `Me` | Aktuelle Instanz |
| `MyBase` | Basisklassenimplementierung |
| `MyClass` | Nichtvirtueller Zugriff auf die eigene Klassenimplementierung |

## Vererbung und Interfaces

```vb
Public MustInherit Class Fahrzeug

    Public MustOverride Sub Starten()

End Class

Public Class Auto
    Inherits Fahrzeug

    Public Overrides Sub Starten()
        Console.WriteLine("Motor gestartet")
    End Sub

End Class
```

- Klassen unterstützen einfache Implementierungsvererbung.
- Mehrfachvererbung von Klassen ist ausgeschlossen.
- Mehrere Interfaces können mit `Implements` implementiert werden.
- `Overridable`, `Overrides` und `MustOverride` steuern Polymorphie.
- `Overloads` ermöglicht Methodenüberladung.

## Eigenschaften und Kapselung

```vb
Public Class Counter

    Private _value As Integer

    Public ReadOnly Property Value As Integer
        Get
            Return _value
        End Get
    End Property

    Public Sub Increment()
        _value += 1
    End Sub

End Class
```

Felder, Eigenschaften und Methoden unterliegen den definierten Sichtbarkeitsregeln.

Eigenschaften können automatische oder explizite `Get`-/`Set`-Implementierungen besitzen.

## Objektlebensdauer

- Objekte werden durch die NovaOS-Runtime verwaltet.
- Referenzen dürfen keine ungültigen Speicherzugriffe ermöglichen.
- Nicht mehr erreichbare Objekte müssen sicher freigegeben werden können.
- Externe Ressourcen benötigen eine definierte Freigabe.
- Gemeinsame Objektreferenzen unterliegen den Nebenläufigkeitsregeln.
- Objektidentität bleibt unabhängig von Speicheradresse und Speicherverwaltungsstrategie.

## Normative Anforderungen

1. NovaLang MUSS `Class`, `Structure`, `Interface` und `Enum` unterstützen.
2. Klassen MÜSSEN Konstruktoren, Felder, Eigenschaften und Methoden unterstützen.
3. Vererbung und Polymorphie MÜSSEN statisch typgeprüft werden.
4. Objektidentität und Wertgleichheit MÜSSEN unterscheidbar sein.
5. Sichtbarkeitsregeln MÜSSEN Kapselung gewährleisten.
6. Objekte MÜSSEN speichersicher verwaltet werden.
7. Objektzugriffe DÜRFEN keine Capability-Berechtigungen erzeugen oder umgehen.
8. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe grundlegende Objektsemantik verwenden.

## Ergebnis

NovaLang besitzt ein natives, VB.NET-orientiertes Objektsystem mit Klassen, Strukturen, Interfaces, Vererbung und Polymorphie, vollständig unabhängig von der .NET-Runtime.
