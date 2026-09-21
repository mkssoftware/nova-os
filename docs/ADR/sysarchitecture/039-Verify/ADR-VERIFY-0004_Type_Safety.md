# ADR-VERIFY-0004 – Type Safety

## Status

Angenommen

## Kategorie

Verification / Type Safety / Interfaces / Correctness

## Kontext

NovaOS verbindet Kernel, Treiber, Services, Capabilities und Anwendungen über klar definierte Schnittstellen.

Viele Fehler entstehen, wenn semantisch unterschiedliche Werte trotz gleicher technischer Darstellung verwechselt werden.

Beispiele:

```text
Object ID ≠ Process ID
Bytes ≠ Pages
Milliseconds ≠ Ticks
User Handle ≠ Kernel Handle
Physical Address ≠ Virtual Address
```

Primitive Datentypen allein können solche Fehler nicht zuverlässig verhindern.

## Entscheidung

NovaOS behandelt **Type Safety als grundlegendes Architekturprinzip**.

Semantisch unterschiedliche Werte sollen durch unterschiedliche Typen repräsentiert werden.

```text
Raw Primitive
     ↓
Semantic Type
     ↓
Validated Interface
```

Typfehler sollen möglichst bereits zur Compile-Zeit oder spätestens an einer definierten Systemgrenze erkannt werden.

## Semantic Types

NovaOS verwendet explizite Typen für systemrelevante Konzepte.

Beispiele:

```text
ObjectID
ProcessID
CapabilityID
VirtualAddress
PhysicalAddress
PageCount
ByteCount
Duration
Timestamp
ResourceBudget
```

Gleiche interne Repräsentation bedeutet nicht automatisch Typkompatibilität.

## Systemgrenzen

Besonders streng typisiert werden Schnittstellen zwischen:

```text
Kernel ↔ Driver
Kernel ↔ Userspace
Service ↔ Service
Capability ↔ Provider
Local ↔ Remote
Trusted ↔ Untrusted
```

Externe oder untypisierte Daten müssen vor der internen Verwendung validiert und in definierte Typen überführt werden.

## Typkonvertierung

Konvertierungen zwischen semantisch unterschiedlichen Typen müssen explizit erfolgen.

```text
Input Type
    ↓
Explicit Conversion
    ↓
Validation
    ↓
Target Type
```

Implizite Konvertierungen dürfen keine relevanten Informationen, Einheiten oder Sicherheitsgrenzen verschleiern.

## Type Safety und Validation

Type Safety und Validation sind getrennte Mechanismen.

```text
Correct Type
≠
Valid Value
```

Ein `ProcessID` kann beispielsweise korrekt typisiert, aber nicht existent oder nicht autorisiert sein.

Deshalb bleiben Range-, State-, Permission- und Contract-Prüfungen erforderlich.

## Unsafe Boundaries

Bereiche, in denen das Typsystem bewusst umgangen wird, müssen explizit begrenzt sein.

Dies betrifft insbesondere:

```text
Hardware Interfaces
Foreign Interfaces
Legacy Code
Raw Memory
Binary Protocol Parsing
```

Nach Verlassen dieser Grenze sollen Daten wieder in sichere NovaOS-Typen überführt werden.

## Normative Anforderungen

1. NovaOS MUSS Type Safety als grundlegendes Korrektheitsprinzip behandeln.
2. Semantisch unterschiedliche Systemwerte SOLLEN unterschiedliche Typen besitzen.
3. Kritische Systemgrenzen MÜSSEN typisierte Schnittstellen verwenden.
4. Externe Daten MÜSSEN vor der internen Verwendung validiert werden.
5. Relevante Typkonvertierungen MÜSSEN explizit erfolgen.
6. Typkompatibilität DARF nicht allein aus gleicher Speicherrepräsentation abgeleitet werden.
7. Type Safety DARF Runtime Validation und Authorization nicht ersetzen.
8. Unsafe Type Conversions MÜSSEN klar begrenzt und überprüfbar sein.
9. Öffentliche ABI- und API-Typen MÜSSEN versionierbar und eindeutig definiert sein.

## Konsequenzen

### Positiv

- verhindert ganze Klassen semantischer Programmierfehler,
- verbessert API- und ABI-Klarheit,
- stärkt Compiler- und Static-Analysis-Prüfungen,
- unterstützt Semantic Types und explizite Contracts.

### Negativ

- zusätzliche Typdefinitionen erhöhen den Entwicklungsaufwand,
- hardwarenahe und Legacy-Schnittstellen benötigen Konvertierungsschichten.

## Abhängigkeiten

- `ADR-VERIFY-0003_Memory_Safety`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-SEMANTIC-0001`
- `ADR-SEMANTIC-0008_Explizite_Konvertierungen`

## Ergebnis

NovaOS folgt dem Prinzip:

```text
If two values mean different things,
they should not accidentally be interchangeable.
```

Type Safety wird genutzt, um semantische Fehler möglichst früh zu verhindern und Systemgrenzen eindeutig und überprüfbar zu machen.