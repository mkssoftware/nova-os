# NPSPEC-IPC-TYPED-0001 – Nova Typed IPC

## Status

Angenommen

## Kategorie

IPC / Typed Communication / Interfaces

## Zweck

NovaOS definiert typisierte IPC-Schnittstellen, damit Prozesse, Dienste und Treiber strukturierte Daten über eindeutig definierte Verträge austauschen können.

```text
Sender
  ↓
Typed Message
  ↓
IPC Interface
  ↓
Receiver
```

Typisierung soll fehleranfällige, frei interpretierbare Nachrichtenformate vermeiden und stabile, evolvierbare Systeminterfaces ermöglichen.

## Grundprinzip

```text
Message Type ≠ Implementation
Interface ≠ Endpoint
Type Compatibility ≠ Permission
Valid Data ≠ Authorized Operation
```

Typprüfung und Sicherheitsprüfung bleiben getrennte Mechanismen.

## IPC Interface

Ein typisiertes IPC-Interface beschreibt die erlaubten Operationen und Datentypen.

```text
IPCInterface
├── InterfaceID
├── Version
├── Operations
├── Request Types
└── Response Types
```

Ein Endpoint kann ein oder mehrere Interfaces bereitstellen.

## Typisierte Operationen

Operationen sollen explizite Ein- und Ausgabetypen besitzen.

```text
Operation ReadObject
    ↓
Request<ObjectID>
    ↓
Response<ObjectData>
```

Operationen dürfen zusätzlich Fehler-, Completion- und Event-Typen definieren.

## Nachrichtentypen

Typisierte Nachrichten müssen eine eindeutig interpretierbare Struktur besitzen.

```text
TypedMessage
├── TypeID
├── SchemaVersion
├── Payload
└── Metadata
```

Optionale Felder müssen von verpflichtenden Feldern unterscheidbar sein.

## Semantic Types

NovaOS soll neben strukturellen Datentypen semantische Typen unterstützen.

```text
Bytes
  ≠
ImageData
  ≠
AudioData
  ≠
EncryptedPayload
```

Gleiche Speicherrepräsentation bedeutet nicht automatisch gleiche Semantik.

Semantic Types können zur Auswahl geeigneter Capabilities, Provider und Verarbeitungspfade verwendet werden.

## Schema

Komplexe Nachrichtentypen müssen über versionierbare Schemas beschrieben werden können.

```text
Schema
├── TypeID
├── Version
├── Required Fields
├── Optional Fields
└── Constraints
```

Empfänger müssen Nachrichten vor Verarbeitung gegen das erwartete Schema validieren können.

## Versionierung

Interfaces und Nachrichtentypen müssen evolvierbar sein.

```text
Interface v1
     ↓
Compatible Evolution
     ↓
Interface v2
```

Neue optionale Felder sollen ältere Empfänger nicht automatisch inkompatibel machen.

Unbekannte verpflichtende Semantik muss dagegen erkannt werden.

## Kompatibilität

NovaOS soll mindestens folgende Zustände unterscheiden können:

```text
Compatible
Backward Compatible
Forward Compatible
Incompatible
Unknown
```

`Unknown` darf nicht automatisch als kompatibel behandelt werden.

## Serialisierung

Die logische Typdefinition darf nicht von einer einzelnen Speicher- oder Serialisierungsrepräsentation abhängen.

```text
Logical Type
    ↓
Encoding
    ↓
IPC Transport
```

Lokale Kommunikation darf optimierte Repräsentationen verwenden, sofern die definierte Semantik erhalten bleibt.

## Shared Buffer

Große Payloads dürfen über typisierte Shared-Buffer-Referenzen übertragen werden.

```text
Typed Message
     ↓
Buffer<ImageData>
     ↓
Shared Buffer
```

Typinformationen ersetzen dabei nicht Ownership-, Lifetime- oder Capability-Regeln des Buffers.

## Capability Transfer

Übertragene Capabilities sollen ebenfalls typisiert sein.

```text
Capability<DeviceControl>
Capability<SharedBufferRead>
Capability<StorageObject>
```

Der Typ beschreibt die erwartete Capability-Klasse, erzeugt jedoch keine zusätzliche Autorität.

## Fehler

Interfaces sollen definierte Fehlertypen verwenden.

```text
Result<T>
├── Success<T>
└── Error<E>
```

Unbekannte oder ungültige Nachrichtentypen müssen kontrolliert abgewiesen werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
InterfaceID
Interface Version
Operation
Request Type
Response Type
Schema Version
Compatibility State
```

Damit können Debugging, Dokumentation und dynamische Interface-Erkennung unterstützt werden.

## Normative Anforderungen

1. NovaOS MUSS typisierte IPC-Interfaces unterstützen.
2. Typisierte Operationen MÜSSEN definierte Request- und Response-Typen besitzen können.
3. Nachrichtentypen MÜSSEN eindeutig identifizierbar sein.
4. Komplexe Typen MÜSSEN versionierbare Schemas unterstützen können.
5. Empfänger MÜSSEN eingehende typisierte Nachrichten validieren können.
6. Typkompatibilität DARF NICHT als Zugriffsberechtigung interpretiert werden.
7. NovaOS SOLL Semantic Types zusätzlich zu strukturellen Typen unterstützen.
8. Unbekannte verpflichtende Semantik DARF NICHT stillschweigend ignoriert werden.
9. Interface- und Schema-Versionierung MUSS kompatible Evolution ermöglichen.
10. Typisierte Shared Buffers MÜSSEN weiterhin den normalen Ownership- und Capability-Regeln folgen.
11. Typisierte Capabilities DÜRFEN keine zusätzlichen Rechte allein durch ihren Typ erzeugen.
12. Interface-, Typ- und Kompatibilitätsinformationen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-IPC-0002`

## Ergebnis

```text
Endpoint
   ↓
Typed Interface
   ↓
Validated Operation
   ↓
Semantic Request / Response
   ↓
IPC Transport
```

NovaOS erhält damit ein typisiertes IPC-Modell, bei dem Systemkomponenten über explizite, versionierte und semantisch definierte Verträge kommunizieren können, ohne Typisierung mit Autorisierung oder Implementierungsdetails zu vermischen.