# NPSPEC-DISTCOMM-SERIALIZATION-0001 – Nova Distributed Serialization

## Status

Angenommen

## Kategorie

Distributed Communication / Serialization / Data Encoding

## Zweck

NovaOS definiert eine gemeinsame Serialisierungsinfrastruktur für den Austausch typisierter Daten über Prozess-, System-, Speicher- und Netzwerkgrenzen hinweg.

```text
Typed Object
     ↓
Schema
     ↓
Serialization
     ↓
Byte / Structured Representation
     ↓
Transport
```

Serialisierung beschreibt ausschließlich die Darstellung von Daten für Speicherung oder Übertragung und bleibt von ihrer fachlichen Semantik getrennt.

## Grundprinzip

```text
Serialization ≠ Schema
Encoding ≠ Semantics
Valid Encoding ≠ Valid Data
Deserialization ≠ Trust
Wire Format ≠ Type Identity
```

Ein erfolgreich dekodierter Datenstrom gilt nicht automatisch als semantisch gültig oder vertrauenswürdig.

## Serialisierungsmodell

Eine Serialisierungsoperation besitzt mindestens:

```text
SerializationContext
├── SchemaID
├── TypeID
├── Schema Version
├── Encoding
└── State
```

Optional:

```text
Security Context
Resource Budget
ExecutionContract
Compatibility Policy
Compression
Canonicalization
```

Das verwendete Encoding darf die logische Identität eines Datentyps nicht bestimmen.

## Encodings

NovaOS darf unterschiedliche Serialisierungsformate unterstützen.

Beispiele:

```text
Binary
Structured Text
Compact Binary
Canonical Binary
Future Encodings
```

Konkrete Formate werden als austauschbare Provider behandelt.

```text
Nova Serialization
       ↓
Encoding Provider
       ↓
Wire Representation
```

Das öffentliche Datenmodell soll nicht von einem einzelnen Format abhängig sein.

## Serialisierung

Der grundlegende Ablauf lautet:

```text
Typed Data
    ↓
Schema Validation
    ↓
Encoder
    ↓
Serialized Data
```

Der Encoder muss die für das ausgewählte Encoding geltenden Größen-, Typ- und Strukturregeln einhalten.

## Deserialisierung

Eingehende Daten werden als nicht vertrauenswürdig behandelt.

```text
Serialized Data
      ↓
Decoder
      ↓
Structural Validation
      ↓
Schema Validation
      ↓
Typed Data
```

Objekte dürfen erst nach ausreichender Validierung für sicherheitskritische Operationen verwendet werden.

## Schema-Integration

Serialization verwendet `NPSPEC-DISTCOMM-SCHEMA-0001`.

```text
Schema
  ↓
Serialization
  ↓
Transport
```

Das Schema definiert Struktur und Semantik.

Serialization definiert deren konkrete Darstellung.

Dadurch darf dasselbe Schema über unterschiedliche Encodings übertragen werden.

## Versionierung

Die Serialisierung muss Schema-Versionen transportieren oder eindeutig aus dem Kommunikationskontext ableiten können.

```text
SchemaID
Version
Encoding
Payload
```

Der Decoder muss erkennen können, ob eine unterstützte Schema-Version vorliegt.

Kompatibilitätsentscheidungen gehören zum Schema-Modell und dürfen nicht allein vom Decoder bestimmt werden.

## Unbekannte Felder

Wenn das Schema dies erlaubt, müssen unbekannte optionale Felder:

```text
Preserve
oder
Skip
```

können.

Unbekannte zwingend benötigte Semantik darf nicht stillschweigend entfernt werden.

## Deterministische Serialisierung

NovaOS soll kanonische bzw. deterministische Serialisierung unterstützen können.

```text
Same Logical Data
       ↓
Canonical Serialization
       ↓
Same Representation
```

Dies ist insbesondere relevant für:

```text
Hashing
Signing
Content Addressing
Caching
Reproducibility
Deterministic Execution
```

Nicht-kanonische Encodings dürfen nicht für Operationen verwendet werden, die eine stabile Byte-Repräsentation voraussetzen.

## Streaming

Große Datenstrukturen sollen ohne vollständige Materialisierung serialisiert und deserialisiert werden können.

```text
Object Stream
     ↓
Encoder
     ↓
Chunks
     ↓
Transport
```

Streaming muss Backpressure unterstützen können.

## Zero-Copy

Wenn technisch und sicher möglich, darf Serialization vorhandene Buffer direkt verwenden.

```text
Typed Buffer
     ↓
Serialization View
     ↓
Transport Buffer
```

Zero-Copy ist keine zwingende Garantie.

NovaOS muss auf sichere Copy-basierte Verarbeitung zurückfallen können.

## Sicherheit

Deserialisierung ist eine Sicherheitsgrenze.

Decoder müssen insbesondere gegen folgende Klassen begrenzt werden:

```text
Oversized Payload
Excessive Nesting
Huge Collections
Invalid Length Fields
Malformed Encoding
Unexpected Types
Resource Exhaustion
```

Serialisierte Daten dürfen keine implizite Autorität übertragen.

Remote Capabilities benötigen dafür ihre eigene definierte geschützte Repräsentation.

## Ressourcensteuerung

NovaOS muss Grenzen unterstützen für:

```text
Payload Size
Object Count
Field Count
Nesting Depth
String Length
Collection Size
Buffer Memory
Decode Time
```

Resource Budgets und Execution Contracts dürfen diese Grenzen weiter einschränken.

## Fehler

Fehler müssen strukturiert unterscheidbar sein.

```text
UnsupportedEncoding
UnsupportedSchema
UnsupportedVersion
MalformedData
SchemaViolation
ResourceLimitExceeded
SecurityViolation
```

Teilweise dekodierte Daten dürfen bei einem Fehler nicht automatisch als gültiges Objekt behandelt werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
SchemaID
TypeID
Schema Version
Encoding
Serialized Size
Validation State
Canonical State
Resource Usage
Failure Reason
```

Sensible Payload-Inhalte dürfen nicht automatisch über Introspection sichtbar werden.

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame Serialisierungsinfrastruktur bereitstellen können.
2. Serialization und Schema MÜSSEN getrennte Abstraktionen bleiben.
3. Das logische Datenmodell DARF NICHT an ein einzelnes Encoding gebunden sein.
4. Eingehende serialisierte Daten MÜSSEN als nicht vertrauenswürdig behandelt werden.
5. Deserialisierte Daten MÜSSEN gegen ihre strukturellen und semantischen Anforderungen validierbar sein.
6. Schema-Versionen MÜSSEN eindeutig bestimmbar sein.
7. Unbekannte optionale Felder SOLLEN erhalten oder übersprungen werden können.
8. Deterministische Serialisierung SOLL für Hashing, Signaturen und reproduzierbare Verarbeitung verfügbar sein.
9. Streaming SOLL unterstützt werden.
10. Zero-Copy DARF verwendet werden, MUSS aber einen sicheren Copy-Fallback besitzen.
11. Serialisierung und Deserialisierung MÜSSEN ressourcenbegrenzt sein.
12. Serialisierungszustände und Fehler SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTCOMM-SCHEMA-0001`
- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-PUBSUB-0001`
- `NPSPEC-DISTCOMM-QUEUE-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-IPC-ZEROCOPY-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DISTCOMM-0009`

## Ergebnis

```text
Typed Data
    ↓
Schema
    ↓
Serialization Provider
    ↓
Canonical / Streaming Representation
    ↓
Transport
    ↓
Validation + Deserialization
    ↓
Typed Data
```

NovaOS erhält damit eine transport- und formatunabhängige Serialisierungsinfrastruktur, die typisierte Daten effizient, sicher, versionierbar und bei Bedarf deterministisch über System- und Netzwerkgrenzen übertragen kann.