# ADR-OBJECT-0009 – Zero-Copy Object Pipelines

## Status

Angenommen

## Kategorie

Object Architecture / Pipelines / Performance

## Kontext

Unified Object Pipelines können große Datenmengen zwischen mehreren Capabilities transportieren.

Würde jeder Pipeline-Schritt den vollständigen Payload kopieren, entstehen unnötige Speicherbandbreite, Latenz und Speicherverbrauch.

```text
Object
  ↓ Copy
Capability A
  ↓ Copy
Capability B
  ↓ Copy
Capability C
```

NovaOS benötigt deshalb einen gemeinsamen Zero-Copy-Mechanismus für Object Pipelines.

## Entscheidung

NovaOS verwendet **Zero-Copy Object Pipelines**, soweit dies technisch und sicher möglich ist.

```text
Object Payload
      ↓
Shared Reference
      ↓
Capability A
      ↓
Capability B
      ↓
Capability C
```

Capabilities erhalten kontrollierten Zugriff auf bestehende Datenbereiche, statt den Payload grundsätzlich zu kopieren.

## Grundmodell

Object Identity und Payload werden getrennt behandelt.

```text
Object
 ├── Identity
 ├── Metadata
 └── Payload Reference
```

Pipeline-Schritte können Payload References weiterreichen.

## Zugriff

Zero-Copy bedeutet keinen uneingeschränkten gemeinsamen Speicherzugriff.

Jeder Zugriff muss weiterhin durch:

```text
Permissions
Capability Contract
Memory Protection
Lifetime Rules
```

kontrolliert werden.

## Immutable Objects

Immutable Payloads eignen sich bevorzugt für Zero-Copy.

```text
Immutable Payload
      ↓
Read-only Mapping
   ┌──┼──┐
   ↓  ↓  ↓
  A   B   C
```

Mehrere Consumer können denselben Datenbereich lesen.

## Mutable Objects

Bei Mutable Objects muss Schreibzugriff kontrolliert werden.

Je nach Contract können verwendet werden:

```text
Exclusive Write
Copy-on-Write
Versioned Mutation
```

Unkontrollierte parallele Änderungen sind nicht zulässig.

## Pipeline-Übergabe

Zwischen Capabilities werden bevorzugt:

```text
Object Handle
Buffer Handle
Shared Memory Reference
Mapped Region
```

übergeben.

Die konkrete technische Form bleibt implementierungsabhängig.

## Fallback

Zero-Copy ist eine Optimierung und keine Voraussetzung für die semantische Korrektheit einer Pipeline.

Falls Zero-Copy nicht sicher oder technisch möglich ist:

```text
Zero-Copy unavailable
        ↓
Controlled Copy
```

Die Pipeline muss weiterhin korrekt funktionieren.

## Lifetime

Ein referenzierter Payload darf nicht freigegeben werden, solange ein gültiger Pipeline-Consumer darauf zugreifen kann.

```text
Acquire
   ↓
Use
   ↓
Release
```

Lifetime und Ownership müssen eindeutig definiert sein.

## Normative Anforderungen

1. NovaOS SOLL Object Pipelines Zero-Copy ausführen, wenn dies sicher möglich ist.
2. Zero-Copy DARF Permissions und Isolation nicht umgehen.
3. Payload References MÜSSEN kontrollierte Lifetime-Regeln besitzen.
4. Immutable Payloads SOLLEN read-only geteilt werden können.
5. Schreibzugriffe auf Mutable Payloads MÜSSEN explizit kontrolliert werden.
6. Copy-on-Write MUSS als mögliche Strategie unterstützt werden können.
7. Pipeline-Semantik DARF nicht von Zero-Copy abhängig sein.
8. Ist Zero-Copy nicht möglich, MUSS ein kontrollierter Copy-Fallback möglich sein.
9. Object Identity MUSS unabhängig von der konkreten Speicherabbildung bleiben.

## Konsequenzen

### Positive Konsequenzen

- weniger Speicheroperationen,
- geringere Latenz,
- geringerer Speicherverbrauch,
- effizientere Verarbeitung großer Medien-, Daten- und Streamobjekte,
- bessere Nutzung gemeinsamer Buffer.

### Negative Konsequenzen

- Lifetime- und Ownership-Management werden wichtiger,
- Mutable Payloads benötigen zusätzliche Synchronisation,
- Isolation muss trotz gemeinsam genutzter Speicherbereiche erhalten bleiben.

## Abhängigkeiten

- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0009_Daten_und_Objekt_Pipelining`
- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-OBJECT-0007_Immutable_und_Mutable_Object_Semantik`
- `ADR-OBJECT-0008_Unified_Object_Pipelines`
- `ADR-SEMANTIC-0005_Typed_IPC`

## Zugehörige NPSPECs

- `NPSPEC-OBJECT-ZERO-COPY-0001`
- `NPSPEC-OBJECT-BUFFER-LIFETIME-0001`
- `NPSPEC-OBJECT-ZERO-COPY-FALLBACK-0001`

## Ergebnis

NovaOS transportiert in Object Pipelines bevorzugt Referenzen statt Payload-Kopien:

```text
Object Payload
      ↓
Protected Reference
      ↓
Capability A
      ↓
Capability B
      ↓
Capability C
```

Der zentrale Grundsatz lautet:

```text
Daten werden nicht kopiert,
nur weil sie den nächsten
Verarbeitungsschritt erreichen.

Wo sicher möglich,
wandert die Referenz statt der Daten.
```