
# NPSPEC-NOVALANG-FFI-0001 – NovaLang Foreign Function Interface

## Status

Angenommen

## Kategorie

NovaLang / Runtime / Interoperabilität

## Zweck

Definiert das Foreign Function Interface (FFI) zur kontrollierten Einbindung nativer Bibliotheken und Funktionen anderer Programmiersprachen.

Ziel ist die Zusammenarbeit mit C, C++ und bestehenden Systembibliotheken ohne .NET-Abhängigkeit.

## Architektur

Das FFI verbindet NovaLang mit nativen Funktionen über definierte Application Binary Interfaces (ABI).

| Komponente | Aufgabe |
|---|---|
| FFI Binder | Auflösung externer Funktionen |
| ABI Adapter | Übergabe von Parametern und Rückgabewerten |
| Type Marshaller | Konvertierung zwischen Datentypen |
| Library Loader | Laden nativer Bibliotheken |
| Capability Bridge | Autorisierung geschützter Zugriffe |

## Funktionsdeklaration

NovaLang verwendet eine VB.NET-orientierte Syntax:

```vb
Declare Function NativeAdd Lib "mathlib" Alias "add" (
    ByVal a As Integer,
    ByVal b As Integer
) As Integer
```

Externe Funktionen müssen ihre ABI und Aufrufkonvention eindeutig festlegen können.

Die Deklaration allein lädt keine Bibliothek und erteilt keine Berechtigung.

## Datentypen

Das FFI unterstützt definierte ABI-kompatible Typen:

- Ganzzahlen und Gleitkommazahlen
- Boolean mit expliziter ABI-Repräsentation
- Zeiger und native Handles
- Strings mit festgelegter Kodierung
- Arrays und Speicherpuffer
- Strukturen mit definiertem Speicherlayout

Objektreferenzen und verwaltete Collections dürfen nicht ohne explizites Marshalling übergeben werden.

## Speicher und Lebenszeiten

- Besitzverhältnisse nativer Speicherbereiche müssen eindeutig sein.
- Verwaltete Objekte benötigen bei direktem nativen Zugriff kontrolliertes Pinning.
- Native Ressourcen müssen über definierte Freigabefunktionen oder `Dispose` freigegeben werden.
- Rückgegebene Zeiger dürfen nicht ungeprüft dereferenziert werden.
- Callbacks müssen ihre Lebenszeit und Aufrufkonvention einhalten.

## Sicherheit

FFI-Aufrufe gelten als sicherheitskritische Übergänge.

- Native Bibliotheken benötigen eine gültige Lade- und Ausführungsberechtigung.
- FFI darf die Capability Bridge nicht umgehen.
- Nicht vertrauenswürdige Bibliotheken müssen in geeigneten NovaOS-Schutzdomänen ausgeführt werden.
- Direkte native Zeigeroperationen sind auf autorisierte Unsafe-Kontexte beschränkt.
- FFI ist innerhalb von Logic-Graph-Custom-Scripts nicht direkt verfügbar; native Funktionen müssen dort über ausdrücklich bereitgestellte Capabilities eingebunden werden.

Ein direkter nativer Aufruf innerhalb desselben Prozesses stellt keine eigenständige Sicherheitsgrenze dar.

## Fehlerbehandlung

Native Fehlercodes werden über definierte Wrapper in NovaLang-Fehlerwerte oder Exceptions übersetzt.

Exceptions dürfen ABI-Grenzen nur überschreiten, wenn beide Seiten ausdrücklich ein kompatibles Exception-Modell vereinbaren.

## Normative Anforderungen

1. NovaLang MUSS ein natives FFI für ABI-kompatible Bibliotheken unterstützen.
2. Aufrufkonventionen, Datentypen und Speicherlayouts MÜSSEN eindeutig definiert sein.
3. Marshalling und Speicherlebenszeiten MÜSSEN kontrolliert werden.
4. Native Bibliotheken MÜSSEN vor dem Laden autorisiert werden.
5. FFI-Aufrufe DÜRFEN Capability- und Sandbox-Regeln nicht umgehen.
6. Unsichere Zeigeroperationen MÜSSEN auf autorisierte Kontexte beschränkt bleiben.
7. Logic-Graph-Custom-Scripts DÜRFEN keine direkten FFI-Aufrufe durchführen.
8. FFI MUSS für AOT, JIT und Interpreter nutzbar sein.
9. Das FFI DARF keine .NET-Runtime voraussetzen.

## Ergebnis

NovaLang erhält ein natives, ABI-basiertes Foreign Function Interface zur Einbindung bestehender Bibliotheken mit kontrolliertem Marshalling, definierten Speicherverträgen und konsequenter NovaOS-Capability-Isolation.
