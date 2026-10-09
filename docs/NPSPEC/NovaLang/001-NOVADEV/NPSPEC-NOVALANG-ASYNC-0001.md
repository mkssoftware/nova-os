
# NPSPEC-NOVALANG-ASYNC-0001 – NovaLang Asynchronous Programming

## Status

Angenommen

## Kategorie

NovaLang / Asynchrone Programmierung

## Zweck

Definiert asynchrone Funktionen, Aufgaben und nicht blockierende Ausführung. Die Syntax orientiert sich an VB.NET und verwendet die native NovaOS-Task-Runtime.

## Async und Await

NovaLang unterstützt `Async` und `Await`.

```vb
Public Async Function LadenAsync() As Task(Of String)
    Dim daten As String = Await Netzwerk.LesenAsync()
    Return daten
End Function
```

- `Async` kennzeichnet asynchrone Funktionen.
- `Await` wartet auf eine asynchrone Operation, ohne zwingend einen Systemthread zu blockieren.
- `Task(Of T)` repräsentiert eine Aufgabe mit Rückgabewert.
- `Task` repräsentiert eine Aufgabe ohne Rückgabewert.
- `Await` ist nur in zulässigen asynchronen Kontexten erlaubt.

## Task-Lebenszyklus

Ein Task besitzt folgende Zustände:

| Zustand | Bedeutung |
|---|---|
| Created | Erstellt |
| Running | In Ausführung |
| Suspended | Wartet auf Fortsetzung |
| Completed | Erfolgreich abgeschlossen |
| Faulted | Mit Fehler beendet |
| Cancelled | Abgebrochen |

Zustandsübergänge werden durch die NovaOS-Runtime kontrolliert.

## Structured Concurrency

Asynchrone Aufgaben gehören grundsätzlich zu einem übergeordneten Ausführungskontext.

- Untergeordnete Tasks besitzen definierte Lebensdauern.
- Elternkontexte kontrollieren Abschluss und Abbruch ihrer Aufgaben.
- Unbeaufsichtigte Hintergrundaufgaben benötigen einen eigenen autorisierten Lebenszyklus.
- Ressourcenlimits werden an untergeordnete Aufgaben weitergegeben.
- Aufgaben dürfen keine Capability-Berechtigungen eigenständig erweitern.

## Cancellation

Tasks unterstützen kooperativen Abbruch über einen Cancellation-Kontext.

```vb
Public Async Function VerarbeitenAsync(
    cancellation As CancellationToken
) As Task

    cancellation.ThrowIfCancellationRequested()

    Await Verarbeitung.StartAsync(cancellation)

End Function
```

Cancellation muss kontrolliert weitergegeben und von gewöhnlichen Fehlern unterscheidbar sein.

## Fehlerbehandlung

```vb
Try
    Dim daten As String = Await LadenAsync()
Catch ex As IOException
    Console.WriteLine(ex.Message)
End Try
```

Exceptions werden beim `Await` gemäß der definierten Fehlersemantik weitergegeben.

Nicht behandelte Task-Fehler müssen durch den übergeordneten Ausführungskontext erfasst werden.

## Ausführungsregeln

- Asynchrone Funktionen werden als sichere Zustandsmaschinen oder semantisch gleichwertige Konstruktionen ausgeführt.
- Lokale Variablen bleiben über Suspendierungspunkte hinweg gültig.
- `Await` garantiert keinen Wechsel des ausführenden Threads.
- Gemeinsamer veränderlicher Zustand benötigt Synchronisation.
- Scheduling und Prioritäten werden durch die NovaOS-Runtime gesteuert.
- Deterministische Ausführung muss innerhalb definierter Kontexte möglich sein.

## Normative Anforderungen

1. NovaLang MUSS `Async`, `Await`, `Task` und `Task(Of T)` unterstützen.
2. Asynchrone Funktionen MÜSSEN statisch typgeprüft werden.
3. Tasks MÜSSEN definierte Zustände und Lebensdauern besitzen.
4. Structured Concurrency MUSS unterstützt werden.
5. Cancellation und Fehlerweitergabe MÜSSEN kontrolliert erfolgen.
6. `Await` DARF keine ungültigen Variablen- oder Ressourcenreferenzen verursachen.
7. Tasks DÜRFEN keine Capability- oder Isolationsgrenzen umgehen.
8. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe grundlegende Async-Semantik verwenden.

## Ergebnis

NovaLang unterstützt native, VB.NET-orientierte asynchrone Programmierung mit `Async`/`Await`, typsicheren Tasks, Structured Concurrency und kontrollierter Ausführung durch NovaOS – unabhängig von der .NET-Runtime.
