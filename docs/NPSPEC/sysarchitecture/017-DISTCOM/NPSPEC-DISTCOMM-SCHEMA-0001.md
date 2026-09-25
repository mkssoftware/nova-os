# NPSPEC-DISTCOMM-SCHEMA-0001 – Nova Distributed Schema

## Status

Angenommen

## Kategorie

Distributed Communication / Schema / Data Contracts

## Zweck

NovaOS definiert ein gemeinsames Schema-Modell für typisierte Daten, Nachrichten und Schnittstellen über Prozess-, System-, Versions- und Netzwerkgrenzen hinweg.

```text
Producer
   ↓
Typed Schema
   ↓
Serialization / Transport
   ↓
Typed Schema
   ↓
Consumer
```

Schemas beschreiben die Struktur und Semantik verteilter Daten unabhängig vom konkreten Transportformat.

## Grundprinzip

```text
Schema ≠ Serialization Format
Type Name ≠ Semantic Identity
Compatible Structure ≠ Compatible Meaning
Unknown Field ≠ Invalid Message
Schema Version ≠ Application Version
```

Strukturelle und semantische Kompatibilität müssen getrennt betrachtet werden.

## Schema-Modell

Ein Schema besitzt mindestens:

```text
DistributedSchema
├── SchemaID
├── TypeID
├── Version
├── Fields
└── Compatibility Rules
```

Optional:

```text
Semantic Type
Constraints
Defaults
Required Fields
Extensions
Security Classification
Provenance
```

`SchemaID` und `TypeID` müssen unabhängig von Netzwerkstandort, Dienstinstanz und Serialisierungsformat sein.

## Feldmodell

Ein Feld kann mindestens besitzen:

```text
Field
├── FieldID
├── Name
├── Type
├── Required
└── Constraints
```

Optional:

```text
Default Value
Semantic Meaning
Security Classification
Deprecated
Introduced Version
```

`FieldID` soll bei Umbenennung stabil bleiben können.

## Semantic Types

Schemas sollen NovaOS Semantic Types unterstützen.

```text
Integer
```

ist beispielsweise nicht automatisch gleichbedeutend mit:

```text
Temperature
Distance
Timestamp
UserID
```

Semantische Typen ermöglichen zusätzliche Validierung und verhindern strukturell gültige, aber semantisch falsche Datenübergaben.

## Versionierung

Schemas müssen unabhängig von Anwendungen versionierbar sein.

```text
Schema v1
   ↓
Schema v2
   ↓
Schema v3
```

Änderungen müssen hinsichtlich ihrer Kompatibilität klassifizierbar sein.

Beispiele:

```text
Compatible
Conditionally Compatible
Breaking
```

## Evolution

Schema-Evolution soll additive Änderungen bevorzugen.

Beispiel:

```text
v1
├── Name
└── ID

v2
├── Name
├── ID
└── Description (optional)
```

Neue optionale Felder sollen von älteren Implementierungen ignoriert oder erhalten werden können.

Entfernung oder Bedeutungsänderung bestehender Felder muss als potenziell inkompatibel behandelt werden.

## Unbekannte Felder

Unbekannte optionale Felder sollen kontrolliert verarbeitet werden können.

```text
Known Field   → Process
Unknown Optional Field → Preserve / Skip
Unknown Required Semantics → Reject / Limited Mode
```

Unbekannte Daten dürfen nicht automatisch als Fehler gelten.

Unbekannte zwingend benötigte Semantik darf jedoch nicht stillschweigend ignoriert werden.

## Required Fields

Pflichtfelder müssen sparsam verwendet werden.

Neue Pflichtfelder können ältere Implementierungen inkompatibel machen.

Daher sollen neue Felder standardmäßig optional sein, sofern die Semantik dies erlaubt.

## Serialisierung

Schemas dürfen mit unterschiedlichen Serialisierungsformaten verwendet werden.

```text
Schema
 ├── Binary Encoding
 ├── Structured Text
 └── Future Encoding
```

Das logische Schema darf nicht unnötig an ein bestimmtes Wire Format gebunden sein.

## Schema Negotiation

Kommunikationspartner dürfen unterstützte Schema-Versionen aushandeln.

```text
Endpoint A
   ↓ supported versions
Negotiation
   ↑ supported versions
Endpoint B
```

Das Ergebnis muss eine von beiden Seiten unterstützte und policy-konforme Version sein.

Fehlt eine kompatible Version, muss die Kommunikation kontrolliert fehlschlagen.

## RPC

Distributed RPC verwendet Schemas für:

```text
Method Input
Method Output
Structured Errors
Streaming Messages
```

Eine RPC-Methode darf ihre unterstützten Schema-Versionen explizit definieren.

## Messaging

Message Bus, Pub/Sub und Queues verwenden Schemas für typisierte Nachrichten.

```text
Topic / Queue
      ↓
SchemaID
      ↓
Typed Message
```

Ein Topic oder eine Queue darf mehrere kompatible Schema-Versionen unterstützen.

## Validierung

Eingehende Daten müssen vor sicherheitskritischer oder semantischer Verarbeitung validierbar sein.

Geprüft werden können:

```text
Type
Size
Required Fields
Ranges
Encoding
Semantic Constraints
Schema Version
```

Externe Daten gelten grundsätzlich als nicht vertrauenswürdig.

## Sicherheit

Schemas dürfen Sicherheitsklassifikationen enthalten.

Beispiele:

```text
Public
Internal
Sensitive
Secret
```

Schema-Metadaten allein ersetzen jedoch keine Zugriffskontrolle.

```text
Schema Classification ≠ Authorization
```

## Ressourcensteuerung

Schema-Verarbeitung muss Grenzen unterstützen für:

```text
Message Size
Field Count
Nesting Depth
Collection Size
String Length
Schema Cache
Validation Cost
```

Komplexe oder manipulierte Schemas dürfen keinen unbegrenzten Ressourcenverbrauch verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
SchemaID
TypeID
Version
Fields
Compatibility
Semantic Types
Supported Encodings
Producer Version
Consumer Version
Validation State
```

Sicherheitskritische Metadaten dürfen nur entsprechend ihrer Berechtigung sichtbar sein.

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames Schema-Modell für verteilte Kommunikation bereitstellen können.
2. Schema-Identität MUSS unabhängig vom Transport sein.
3. Schema und Serialisierungsformat MÜSSEN getrennt modellierbar sein.
4. Schemas MÜSSEN versionierbar sein.
5. Schema-Kompatibilität MUSS explizit bestimmbar sein.
6. Additive Evolution SOLL bevorzugt werden.
7. Unbekannte optionale Felder SOLLEN kontrolliert ignoriert oder erhalten werden können.
8. Unbekannte zwingende Semantik DARF NICHT stillschweigend ignoriert werden.
9. Semantic Types SOLLEN unterstützt werden.
10. Eingehende Daten MÜSSEN gegen ihr Schema validierbar sein.
11. Schema-Verarbeitung MUSS ressourcenbegrenzt sein.
12. Schema- und Kompatibilitätszustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-PUBSUB-0001`
- `NPSPEC-DISTCOMM-QUEUE-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-DISTCOMM-0008`

## Ergebnis

```text
Semantic Type
      ↓
Distributed Schema
      ↓
Version + Compatibility
      ↓
Serialization
      ↓
Distributed Communication
      ↓
Validation
      ↓
Typed Consumer
```

NovaOS erhält damit ein gemeinsames, evolvierbares Schema-System für verteilte Kommunikation, das Datentypen und ihre Semantik unabhängig von Transport, Serialisierung, Dienststandort und Anwendungsversion beschreibt.