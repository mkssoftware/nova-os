
# NPSPEC-NOVALANG-PATTERNMATCHING-0001 – NovaLang Pattern Matching

## Status

Angenommen

## Kategorie

NovaLang / Mustererkennung

## Zweck

Definiert die typsichere Prüfung und Zerlegung von Werten anhand von Mustern. NovaLang erweitert die VB.NET-orientierte Syntax um moderne Pattern-Matching-Funktionen.

## Unterstützte Muster

| Muster | Beispiel |
|---|---|
| Konstanten | `Case 10` |
| Mehrfachwerte | `Case 1, 2, 3` |
| Wertebereiche | `Case 1 To 10` |
| Vergleich | `Case Is > 100` |
| Typprüfung | `TypeOf obj Is Person` |
| Nothing-Prüfung | `obj Is Nothing` |
| Zeichenketten | `text Like "Nova*"` |
| Typmuster | `Case Type Person As p` |
| Eigenschaftsmuster | `Case {.Alter = 18 To 65}` |
| Wildcard | `Case Else` |

Typ- und Eigenschaftsmuster sind NovaLang-Erweiterungen gegenüber VB.NET.

## Select Case

```vb
Select Case wert
    Case 0
        Console.WriteLine("Null")
    Case 1 To 10
        Console.WriteLine("Klein")
    Case Is > 10
        Console.WriteLine("Groß")
    Case Else
        Console.WriteLine("Unbekannt")
End Select
```

Der Selektorausdruck wird einmal ausgewertet. Der erste passende Zweig wird ausgeführt.

## Typmuster

```vb
Select Case objekt
    Case Type Person As person
        Console.WriteLine(person.Name)
    Case Type Fahrzeug As fahrzeug
        fahrzeug.Starten()
    Case Else
        Console.WriteLine("Unbekannter Typ")
End Select
```

Die gebundene Variable besitzt den geprüften Typ und ist nur im zugehörigen Zweig sichtbar.

## Eigenschaftsmuster

```vb
Select Case person
    Case {.Alter = 18 To 65}
        Console.WriteLine("Erwachsen")
    Case {.Alter = 0 To 17}
        Console.WriteLine("Minderjährig")
End Select
```

Eigenschaftsmuster prüfen mehrere Bedingungen auf einem Objekt, ohne dessen Typ- oder Speichersicherheit zu umgehen.

## Auswertungsregeln

- Muster werden in Quelltextreihenfolge geprüft.
- Muster müssen mit dem geprüften Typ kompatibel sein.
- `Nothing` wird vor einem erforderlichen Memberzugriff sicher behandelt.
- Musterbindungen gelten ausschließlich im erfolgreichen Zweig.
- Mehrfachauswertungen von Ausdrücken mit Nebenwirkungen müssen vermieden werden.
- Der Compiler soll unerreichbare Muster und unvollständige Fallunterscheidungen diagnostizieren.
- Pattern Matching darf keine impliziten Typkonvertierungen außerhalb der NovaLang-Typregeln durchführen.

## Normative Anforderungen

1. NovaLang MUSS Konstanten-, Bereichs-, Vergleichs- und Typmuster unterstützen.
2. NovaLang MUSS Eigenschaftsmuster und typisierte Musterbindungen unterstützen.
3. Muster MÜSSEN statisch typgeprüft werden.
4. Die Auswertung MUSS eine definierte Reihenfolge besitzen.
5. Musterbindungen MÜSSEN auf ihren gültigen Zweig begrenzt sein.
6. Pattern Matching DARF keine Capability-Berechtigungen erzeugen oder Sicherheitsgrenzen umgehen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe Pattern-Matching-Semantik verwenden.

## Ergebnis

NovaLang erweitert die vertraute VB.NET-Syntax um typsicheres Pattern Matching mit Typ-, Wertebereichs- und Eigenschaftsmustern für eine kompakte und sichere Fallunterscheidung.
