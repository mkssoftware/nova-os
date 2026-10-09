
# NPSPEC-NOVALANG-ERRORHANDLING-0001 – NovaLang Error Handling

## Status

Angenommen

## Kategorie

NovaLang / Fehlerbehandlung

## Zweck

Definiert die Behandlung von Laufzeitfehlern, Exceptions und typisierten Fehlerergebnissen. Die Syntax orientiert sich an VB.NET und integriert das sichere Ausführungsmodell von NovaOS.

## Exception-Behandlung

NovaLang unterstützt `Try`, `Catch`, `Finally` und `Throw`.

```vb
Try
    DatenLaden()
Catch ex As IOException
    Console.WriteLine(ex.Message)
Catch ex As Exception
    Console.WriteLine("Fehler: " & ex.Message)
Finally
    RessourcenFreigeben()
End Try
```

- `Catch` behandelt passende Exception-Typen.
- Mehrere `Catch`-Blöcke sind zulässig.
- `When` ermöglicht zusätzliche Filterbedingungen.
- `Throw` löst eine Exception aus.
- `Throw` ohne Argument wirft innerhalb eines `Catch` die aktuelle Exception erneut.
- `Finally` wird beim regulären Verlassen und bei behandelbaren Exceptions ausgeführt.

## Typisierte Fehlerergebnisse

Für erwartbare Fehler unterstützt NovaLang `Result(Of T, E)`.

```vb
Public Function Laden() As Result(Of String, Error)
    If Not DateiVorhanden() Then
        Return Result.Fail("Datei fehlt")
    End If

    Return Result.Ok("Daten")
End Function
```

`Result` unterscheidet Erfolg und Fehler ohne Exception. Beide Zustände müssen eindeutig und typsicher verarbeitet werden.

## Fehlerklassen

| Fehlerart | Behandlung |
|---|---|
| Syntax- und Typfehler | Compilerdiagnose |
| Erwartbarer Operationsfehler | `Result(Of T, E)` |
| Behandelbarer Laufzeitfehler | `Try/Catch` |
| Cancellation | Kontrollierter Aufgabenabbruch |
| Capability-Verweigerung | Autorisierungsfehler |
| Vertragsverletzung | Definierter Contract-Fehler |
| Kritischer Runtime-Fehler | Isolierung oder kontrollierter Abbruch |

## Fehlerweitergabe

- Unbehandelte Exceptions werden entlang des Aufrufkontexts weitergegeben.
- Asynchrone Fehler werden über den zugehörigen Task propagiert.
- Fehler in untergeordneten Tasks folgen den Structured-Concurrency-Regeln.
- Ressourcen müssen bei behandelbaren Fehlern ordnungsgemäß freigegeben werden.
- Ein Prozessabsturz garantiert keine Ausführung von `Finally`.
- Sicherheitsverletzungen dürfen nicht durch Exception-Behandlung autorisiert werden.

## Diagnostik

Fehlerobjekte unterstützen mindestens:

- Fehlertyp und Fehlercode
- Fehlermeldung
- Ursache beziehungsweise innere Exception
- Stacktrace und Quelltextposition, soweit verfügbar

Sensible Informationen dürfen nur entsprechend den Berechtigungen offengelegt werden.

## Normative Anforderungen

1. NovaLang MUSS `Try`, `Catch`, `Finally`, `Throw` und `Catch When` unterstützen.
2. Exceptions MÜSSEN typisiert und kontrolliert weitergegeben werden.
3. `Result(Of T, E)` MUSS für erwartbare Fehler verfügbar sein.
4. Asynchrone Fehler und Cancellation MÜSSEN definiert behandelt werden.
5. Fehlerbehandlung DARF keine Capability- oder Speicherisolierung umgehen.
6. Compiler und Runtime MÜSSEN nachvollziehbare Fehlerdiagnosen bereitstellen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe grundlegende Fehlersemantik verwenden.

## Ergebnis

NovaLang kombiniert VB.NET-orientierte Exception-Behandlung mit typisierten Fehlerergebnissen, sicherer Fehlerweitergabe und kontrollierter Ressourcenfreigabe.
