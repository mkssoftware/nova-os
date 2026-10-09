
# NPSPEC-NOVALANG-CONTROLFLOW-0001 – NovaLang Control Flow

## Status

Angenommen

## Kategorie

NovaLang / Kontrollfluss

## Zweck

Definiert die Steuerung des Programmablaufs durch Bedingungen, Verzweigungen, Schleifen und Sprunganweisungen. Die Syntax orientiert sich möglichst exakt an VB.NET.

## Kontrollstrukturen

| Konstruktion | Syntax |
|---|---|
| Bedingung | `If ... Then ... ElseIf ... Else ... End If` |
| Mehrfachauswahl | `Select Case ... Case ... End Select` |
| Zählschleife | `For ... To ... Step ... Next` |
| Iteration | `For Each ... In ... Next` |
| Bedingte Schleife | `While ... End While` |
| Do-Schleife | `Do While ... Loop`, `Do Until ... Loop` |
| Nachgestellte Bedingung | `Do ... Loop While`, `Do ... Loop Until` |
| Schleifenabbruch | `Exit For`, `Exit While`, `Exit Do` |
| Iteration überspringen | `Continue For`, `Continue While`, `Continue Do` |
| Funktionsrückgabe | `Return` |
| Prozedur verlassen | `Exit Sub`, `Exit Function` |

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

Bedingungen müssen einen gültigen `Boolean`-Wert ergeben. Nur der ausgewählte Zweig wird ausgeführt.

## Mehrfachauswahl

```vb
Select Case status
    Case 0
        Starten()
    Case 1, 2
        Fortsetzen()
    Case Else
        Beenden()
End Select
```

Der Selektorausdruck wird einmal ausgewertet. Nur der erste passende Zweig wird ausgeführt.

## Schleifen

```vb
For i As Integer = 1 To 10
    Console.WriteLine(i)
Next

For Each datei As FileInfo In dateien
    Verarbeiten(datei)
Next

While aktiviert
    Aktualisieren()
End While
```

`For` verwendet inklusive Endwerte. `For Each` nutzt typisierte Iteratoren.

## Ablaufsteuerung

```vb
For i As Integer = 1 To 10
    If i = 5 Then Continue For
    If i = 8 Then Exit For

    Console.WriteLine(i)
Next
```

`Return` beendet die aktuelle Funktion und liefert gegebenenfalls einen Rückgabewert.

`Exit` und `Continue` dürfen nur innerhalb zulässiger Kontrollstrukturen verwendet werden.

## Ausführungsregeln

- Kontrollstrukturen besitzen eindeutige Eintritts- und Austrittsbedingungen.
- Verschachtelte Blöcke werden nach lexikalischem Gültigkeitsbereich verarbeitet.
- Schleifenbedingungen werden an den jeweils definierten Prüfstellen ausgewertet.
- Iteratoren und Ressourcen müssen bei vorzeitigem Verlassen ordnungsgemäß freigegeben werden.
- `Finally`-Blöcke dürfen durch `Return`, `Exit` oder `Continue` nicht umgangen werden.
- Cancellation und Ressourcenlimits müssen kontrolliert behandelbar sein.
- Unstrukturierte Sprünge wie `GoTo` gehören nicht zum sicheren NovaLang-Standard.

## Normative Anforderungen

1. NovaLang MUSS die definierten VB.NET-orientierten Kontrollstrukturen unterstützen.
2. Bedingungen MÜSSEN statisch als `Boolean` prüfbar sein.
3. Schleifen MÜSSEN definierte Abbruch- und Fortsetzungsregeln besitzen.
4. `Return`, `Exit` und `Continue` MÜSSEN ihren jeweiligen Gültigkeitsbereich einhalten.
5. Ressourcenfreigabe und Fehlerbehandlung DÜRFEN durch Kontrollflusswechsel nicht umgangen werden.
6. Compiler und Interpreter MÜSSEN dieselbe beobachtbare Kontrollflusssemantik gewährleisten.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselben Kontrollflussregeln verwenden.

## Ergebnis

NovaLang besitzt einen strukturierten, VB.NET-orientierten Kontrollfluss mit eindeutig definierten Bedingungen, Schleifen, Verzweigungen und Abbruchmechanismen, integriert in das sichere Ausführungsmodell von NovaOS.
