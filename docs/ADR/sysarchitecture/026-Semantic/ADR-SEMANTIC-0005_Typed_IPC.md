# ADR-SEMANTIC-0005 – Typed IPC

## Status

Angenommen

## Kategorie

Semantic Architecture / IPC / Type Safety

## Kontext

Klassische IPC-Mechanismen übertragen Bytes, Nachrichten oder primitive Datenstrukturen, ohne deren fachliche Bedeutung zwingend zu kennen.

Dadurch können Sender und Empfänger technisch kompatible, aber semantisch unterschiedliche Daten austauschen.

NovaOS benötigt deshalb semantische Typinformation auch an Prozess- und Servicegrenzen.

## Entscheidung

NovaOS verwendet **Typed IPC**.

IPC-Schnittstellen können Ein- und Ausgaben durch Semantic Types definieren.

```text
Sender
  ↓
Typed Message
  ↓
IPC
  ↓
Contract Check
  ↓
Receiver
```

Die IPC-Schicht transportiert weiterhin effizient die zugrunde liegenden Daten, erhält jedoch deren semantischen Contract.

## IPC Contracts

IPC-Endpunkte deklarieren die erwarteten Typen.

```text
Request:
    Image.RGBA

Response:
    Image.RGBA
```

Sender und Empfänger müssen kompatible Semantic Types verwenden.

## Typprüfung

Vor der Übergabe wird geprüft:

```text
Expected Type
      ↓
Provided Type
      ↓
Compatible?
```

Semantisch inkompatible Nachrichten werden zurückgewiesen.

## Konvertierung

Explizit definierte Konvertierungen können zwischen kompatiblen Semantic Types eingesetzt werden.

```text
Audio.PCM.S16
      ↓
Conversion
      ↓
Audio.PCM.F32
```

Eine Konvertierung darf nicht allein aufgrund gleicher primitiver Repräsentation erfolgen.

## Zero-Copy

Typed IPC darf Zero-Copy-Datenübertragung nicht verhindern.

```text
Typed Resource
     ↓
Shared Buffer / Handle
     ↓
Receiver
```

Semantic Type und Zugriffskontrolle können getrennt von den eigentlichen Payload-Daten übertragen werden.

## Versionierung

IPC Contracts müssen versionierbar sein.

Neue Versionen dürfen bestehende Kommunikationspartner nicht stillschweigend semantisch inkompatibel machen.

## Security

Semantic Types ersetzen keine Berechtigungsprüfung.

```text
Type Compatible
      ≠
Authorized
```

IPC muss weiterhin Capability-, Permission- und Isolation-Regeln beachten.

## Normative Anforderungen

1. NovaOS MUSS Semantic Types in IPC Contracts unterstützen.
2. Sender und Empfänger MÜSSEN semantisch kompatible Datentypen verwenden.
3. Inkompatible Typed Messages MÜSSEN zurückgewiesen werden.
4. Konvertierungen MÜSSEN explizit definiert sein.
5. Typed IPC MUSS mit Zero-Copy-Mechanismen kompatibel sein.
6. IPC Contracts MÜSSEN versionierbar sein.
7. Semantic Type Safety DARF Authorization und Permission Checks nicht ersetzen.

## Konsequenzen

### Positive Konsequenzen

- semantisch falsche IPC-Verbindungen werden verhindert,
- Service- und Capability-Schnittstellen werden eindeutig beschreibbar,
- automatische Komposition wird sicherer,
- Zero-Copy bleibt möglich.

### Negative Konsequenzen

- IPC Contracts benötigen zusätzliche Typinformationen,
- Typ- und Versionskompatibilität muss geprüft werden.

## Abhängigkeiten

- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0002_Strong_Semantic_Typing`
- `ADR-SEMANTIC-0003_Typed_Resources`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-IPC-0001_IPC_Grundmodell`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-TYPED-IPC-0001`
- `NPSPEC-SEMANTIC-IPC-CONTRACT-0001`
- `NPSPEC-SEMANTIC-IPC-COMPATIBILITY-0001`

## Ergebnis

NovaOS transportiert über IPC nicht nur Daten, sondern erhält deren semantische Bedeutung:

```text
Typed Data
    ↓
IPC Contract
    ↓
Type Check
    ↓
Transfer
```

Der zentrale Grundsatz lautet:

```text
IPC verbindet nicht nur Speicher.

IPC verbindet
semantisch kompatible Daten.
```