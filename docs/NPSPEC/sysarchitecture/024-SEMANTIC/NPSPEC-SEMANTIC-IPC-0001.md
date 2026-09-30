# NPSPEC-SEMANTIC-IPC-0001 – Nova Semantic IPC

## Status

Angenommen

## Kategorie

Semantic / IPC / Communication

## Zweck

NovaOS definiert semantische IPC, bei der Komponenten nicht nur rohe Bytes oder implementierungsspezifische Nachrichten austauschen, sondern typisierte Daten, Operationen und Ressourcen mit systemweit definierter Bedeutung.

```text
Sender
  ↓
Semantic Message
  ↓
Typed IPC
  ↓
Receiver
```

Dadurch werden IPC-Schnittstellen unabhängig von konkreter Programmiersprache, Prozessimplementierung und physischer Repräsentation.

## Grundprinzipien

```text
Semantic Message ≠ Raw Bytes
Semantic Type ≠ Serialization Format
Operation Identity ≠ Function Address
Object Identity ≠ Memory Address
Message Compatibility ≠ Authority
IPC Access ≠ Resource Access
Serialization ≠ Capability Transfer
```

## Semantic Message

Eine semantische IPC-Nachricht besitzt mindestens:

```text
SemanticMessage
├── MessageTypeID
├── OperationID
├── Input Semantic Types
└── Schema Version
```

Optional:

```text
ObjectID
ResourceID
ExecutionContractID
TransactionID
TraceID
Deadline
Capability References
Constraints
Metadata
```

## Semantische Operationen

IPC-Aufrufe beschreiben die gewünschte Operation unabhängig von der konkreten Implementierung.

```text
Operation:
Document.Render

Input:
Nova.Document.Text

Output:
Nova.Image.Raster
```

Der Empfänger implementiert die semantische Operation über ein kompatibles Capability Interface.

## Typisierte Daten

IPC-Felder verwenden definierte Semantic Types.

```text
Request
├── Source: Nova.Document.Text
├── TargetFormat: Nova.Image.Raster
└── Quality: Nova.Quality.Level
```

Der Semantic Type beschreibt die Bedeutung.

Die physische Darstellung wird separat behandelt.

## Serialisierung

Semantic IPC ist unabhängig vom verwendeten Serialisierungsformat.

```text
Semantic Message
      ↓
Serialization
      ↓
IPC Transport
```

Mögliche Darstellungen:

```text
Binary Schema
Shared Memory
Zero-Copy Buffer
Structured Message
Remote Serialization
```

Es gilt:

```text
Semantic Type ≠ Serialization
```

## Object References

Systemobjekte sollen über stabile Identitäten referenziert werden.

```text
ObjectID
ResourceID
CapabilityID
```

Speicheradressen oder lokale Handles dürfen nicht als globale semantische Identitäten interpretiert werden.

## Capability Integration

Benötigt eine Nachricht Autorität, muss diese explizit übertragen oder delegiert werden.

```text
Semantic Request
       +
Capability Transfer
       ↓
Authorized Operation
```

Dabei gilt:

```text
Semantic Reference ≠ Capability
ObjectID ≠ Permission
ResourceID ≠ Permission
```

Capability Transfer folgt `NPSPEC-CAPABILITY-IPC-0001`.

## Zero-Copy

Semantic IPC muss Zero-Copy unterstützen können.

```text
Semantic Metadata
       +
Shared Buffer Reference
       ↓
Receiver
```

Der gemeinsame Buffer verändert nicht die semantische Bedeutung der Daten und erzeugt keine zusätzliche Autorität.

## ExecutionContract

Komplexe IPC-Aufrufe können einen ExecutionContract referenzieren.

```text
Semantic Operation
       ↓
ExecutionContract
       ↓
Capabilities
       ↓
Resources
       ↓
Provider Execution
```

Damit können Deadline, Ressourcenbudget, Determinismus, Trust und Sovereignty über IPC hinweg erhalten bleiben.

## Lokale und entfernte Kommunikation

Das semantische Modell soll unabhängig vom Transport bleiben.

```text
Semantic Request
├── Local IPC
├── Shared Memory
├── Message Bus
└── Remote RPC
```

Ein Wechsel des Transports darf die Bedeutung der Operation nicht still verändern.

## Versionierung

Semantic IPC muss explizite Versionierung unterstützen.

```text
MessageTypeID
OperationID
SemanticType Version
Schema Version
Interface Version
```

Diese Versionen bleiben getrennte Konzepte.

Inkompatible Änderungen dürfen nicht still als kompatibel interpretiert werden.

## Fehler

Fehler sollen ebenfalls semantisch beschrieben werden.

```text
OperationResult
├── Success
├── InvalidInput
├── Unauthorized
├── CapabilityRevoked
├── ResourceUnavailable
├── DeadlineExceeded
└── UnsupportedSemanticType
```

Transportfehler und Operationsfehler bleiben unterscheidbar.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
MessageTypeID
OperationID
Semantic Types
Schema Version
Interface Version
Sender Identity
Receiver Identity
ExecutionContract
Transferred Capabilities
TransactionID
TraceID
```

Payload und Capability Tokens dürfen dadurch nicht automatisch offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS semantisch typisierte IPC-Nachrichten unterstützen können.
2. Semantic Types MÜSSEN von Serialisierungsformaten getrennt bleiben.
3. Operationen SOLLEN über stabile semantische IDs beschrieben werden.
4. Objekt- und Ressourcenidentitäten DÜRFEN NICHT als Autorität interpretiert werden.
5. Capability Transfer MUSS explizit erfolgen.
6. Semantic IPC MUSS Zero-Copy unterstützen können.
7. ExecutionContracts SOLLEN über IPC propagierbar sein.
8. Lokale und entfernte Kommunikation SOLLEN dasselbe semantische Modell verwenden können.
9. Interface-, Schema- und Semantic-Type-Versionen MÜSSEN getrennt behandelbar sein.
10. Semantic IPC MUSS introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-FILE-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-DISTCOMM-SCHEMA-0001`
- `NPSPEC-DISTCOMM-SERIALIZATION-0001`
- `ADR-ARCH-0011`

## Ergebnis

```text
Semantic Operation
       ↓
Typed Semantic Message
       ↓
Explicit Capabilities
       ↓
IPC / Zero-Copy / Remote Transport
       ↓
Compatible Provider
       ↓
Semantic Result
```

NovaOS erhält damit eine IPC-Ebene, auf der Komponenten über die Bedeutung von Daten und Operationen kommunizieren können, ohne dauerhaft an konkrete Speicherformate, Programmiersprachen, Prozesse oder Transportmechanismen gekoppelt zu sein.