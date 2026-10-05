# NPSPEC-CAPABILITY-OUTPUT-0001 – Nova Capability Output

## Status

Angenommen

## Kategorie

Capability / Output

## Zweck

NovaOS definiert das Ausgabemodell für Capability-Aufrufe.

Capability-Ausgaben werden über semantische Typen und klar definierte Verträge beschrieben, damit Ergebnisse unabhängig von Provider, Implementierung, Speicherort und physischem Datenformat weiterverarbeitet werden können.

## Grundprinzipien

```text
Output ≠ Authority
Output Type ≠ Physical Format
Output Reference ≠ Ownership
Output ≠ Persistent Object
Result ≠ Successful Commit
Capability Output ≠ Provider-specific Structure
```

## Output-Modell

Eine Capability-Operation kann Ausgaben deklarieren als:

```text
CapabilityOutput
├── Name
├── SemanticTypeID
├── Cardinality
├── Constraints
└── TransferMode
```

Optional:

```text
Mutability
Lifetime
Persistence
SizeLimit
Streaming
```

## Semantische Typen

Ausgaben sollen über stabile semantische Typen beschrieben werden:

```text
Capability
     ↓
SemanticTypeID
     ↓
Type Registry
     ↓
Consumer
```

Das physische Format darf unabhängig vom semantischen Typ sein.

## Mehrere Ausgaben

Eine Operation darf mehrere benannte Ergebnisse liefern:

```text
ImageAnalysis
├── image
├── metadata
└── confidence
```

Jede Ausgabe besitzt einen eigenen Typ und Vertrag.

## Übergabeformen

Ausgaben können bereitgestellt werden als:

```text
Value
ObjectID
Authorized Handle
Shared Buffer
Stream
```

Die konkrete Übergabeform darf die semantische Bedeutung nicht verändern.

## Zero-Copy

Große Ergebnisse sollen nach Möglichkeit ohne unnötige Kopien weitergegeben werden können:

```text
Capability Provider
       ↓
Shared / Mapped Buffer
       ↓
Consumer
```

Ist Zero-Copy nicht sicher oder möglich, muss ein kontrollierter Copy-Fallback verfügbar sein.

## Objekt-Ausgaben

Erzeugt eine Capability ein persistentes Objekt, erhält dieses eine eigene stabile `ObjectID`.

```text
Capability
    ↓
Create Object
    ↓
ObjectID
```

Temporäre Ergebnisse dürfen stattdessen an Task-, Process- oder andere definierte Lifetimes gebunden sein.

## Authority

Eine ausgegebene Referenz darf ausschließlich die vorgesehene Authority übertragen.

```text
Output Handle
    ↓
Defined Authority
```

Ein Ergebnis darf keine weitergehenden Rechte erzeugen, als durch Capability-Vertrag und Policy vorgesehen.

## Streaming

Capabilities müssen Streaming-Ausgaben unterstützen können:

```text
Capability
    ↓
Stream
    ↓
Backpressure
    ↓
Consumer
```

Streaming muss Cancellation, Deadline und Ressourcenlimits berücksichtigen können.

## Fehler und Teilergebnisse

Capability Interface muss definieren, ob eine Operation:

```text
Atomic Result
Partial Result
Streaming Result
```

liefert.

Teilergebnisse dürfen nicht automatisch als vollständig oder persistent committed gelten.

## Lifetime

Nicht persistente Ausgaben benötigen eine definierte Lebensdauer:

```text
Call
Task
Process
Stream
Explicit Lifetime
Persistent
```

Nach Ablauf müssen zugehörige Ressourcen kontrolliert freigegeben werden können.

## Normative Anforderungen

1. Capability-Ausgaben MÜSSEN durch das Capability Interface beschreibbar sein.
2. Ausgaben SOLLEN stabile semantische Typen verwenden.
3. Semantischer Typ und physisches Format MÜSSEN getrennt bleiben.
4. Operationen MÜSSEN mehrere benannte Ausgaben unterstützen können.
5. Value-, ObjectID-, Handle-, Buffer- und Stream-Ausgaben MÜSSEN unterstützt werden können.
6. Persistente neue Objekte MÜSSEN eine eigene stabile `ObjectID` erhalten.
7. Ausgabereferenzen DÜRFEN keine unbeabsichtigte Authority erzeugen.
8. Übertragene Authority MUSS auf den definierten Output-Vertrag beschränkt sein.
9. Zero-Copy SOLL verwendet werden können, wenn Sicherheit und Kompatibilität dies erlauben.
10. Ein sicherer Copy-Fallback MUSS verfügbar sein.
11. Nicht persistente Ausgaben MÜSSEN eine definierte Lebensdauer besitzen.
12. Streaming-Ausgaben MÜSSEN Backpressure und Cancellation unterstützen können.
13. Teil- und Streaming-Ergebnisse MÜSSEN von vollständig abgeschlossenen Ergebnissen unterscheidbar sein.
14. Output-Typen, Transfermodus, Lifetime und Authority MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-INPUT-0001`
- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-REGISTRY-TYPE-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein typisiertes und providerunabhängiges Ausgabemodell für Capabilities. Ergebnisse können als Werte, Objekte, autorisierte Handles, Buffer oder Streams weitergegeben werden, während semantischer Typ, physisches Format, Persistenz, Lebensdauer und Authority klar voneinander getrennt bleiben.