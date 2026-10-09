
# NPSPEC-NOVALANG-EVENTS-0001 – NovaLang Events

## Status

Angenommen

## Kategorie

NovaLang / Ereignisse

## Zweck

Definiert die Deklaration, Registrierung und Auslösung typisierter Ereignisse. Die Syntax orientiert sich an VB.NET und verwendet das native Ereignismodell von NovaOS.

## Ereignisdeklaration

Ereignisse werden mit `Event` deklariert und mit `RaiseEvent` ausgelöst.

```vb
Public Class Sensor

    Public Event ValueChanged(value As Integer)

    Public Sub Update(value As Integer)
        RaiseEvent ValueChanged(value)
    End Sub

End Class
```

Ereignisparameter werden statisch typgeprüft.

## Ereignisbehandlung

NovaLang unterstützt `AddHandler`, `RemoveHandler`, `Handles` und `WithEvents`.

```vb
Dim sensor As New Sensor()

AddHandler sensor.ValueChanged, AddressOf OnValueChanged

Private Sub OnValueChanged(value As Integer)
    Console.WriteLine(value)
End Sub
```

Handler müssen zur Ereignissignatur kompatibel sein.

`RemoveHandler` entfernt eine bestehende Registrierung. `Handles` ermöglicht deklarative Ereignisbindungen.

## Ereignissemantik

- Ereignisse können mehrere Handler besitzen.
- Synchrone Handler werden in Registrierungsreihenfolge ausgeführt.
- `RaiseEvent` ruft die registrierten Handler synchron auf.
- Exceptions werden nach den NovaLang-Fehlerregeln weitergegeben.
- Registrierungen besitzen eine definierte Lebensdauer.
- Doppelte Registrierungen sind zulässig und werden einzeln behandelt.
- Änderungen der Handlerliste während einer Auslösung beeinflussen erst nachfolgende Auslösungen.

## Asynchrone Ereignisse

Asynchrone Handler werden unterstützt, müssen jedoch über einen definierten Task-Kontext ausgeführt werden.

Für Ereignisse mit garantiertem Abschluss oder Fehlerweitergabe sind explizit awaitbare Ereignisoperationen vorzusehen.

Unkontrollierte Fire-and-Forget-Ausführung ist nicht zulässig.

## NovaOS-Integration

Ereignisse können durch Capabilities, Solutions, Logic Graph und UI-Komponenten bereitgestellt werden.

- Ereignisdaten werden typisiert übertragen.
- Ereignisquellen unterliegen Capability-Berechtigungen.
- Handler erhalten keine zusätzlichen Berechtigungen durch ihre Registrierung.
- Prozessübergreifende Ereignisse verwenden autorisierte IPC-Mechanismen.
- Ereignisregistrierungen müssen sicher freigegeben werden können.

## Normative Anforderungen

1. NovaLang MUSS `Event`, `RaiseEvent`, `AddHandler`, `RemoveHandler`, `Handles` und `WithEvents` unterstützen.
2. Ereignisse und Handler MÜSSEN statisch typgeprüft werden.
3. Synchrone Ereignisse MÜSSEN eine definierte Aufrufreihenfolge besitzen.
4. Registrierung und Deregistrierung MÜSSEN kontrolliert erfolgen.
5. Asynchrone Ereignisverarbeitung MUSS Structured Concurrency berücksichtigen.
6. Ereignisse DÜRFEN keine Capability- oder Isolationsgrenzen umgehen.
7. `.nova`, `.nlf` und `.nui` MÜSSEN dieselbe grundlegende Ereignissemantik verwenden.

## Ergebnis

NovaLang besitzt ein natives, VB.NET-orientiertes Ereignismodell mit typisierten Handlern, kontrollierter Registrierung und sicherer Integration in die ereignisgesteuerte NovaOS-Architektur.
