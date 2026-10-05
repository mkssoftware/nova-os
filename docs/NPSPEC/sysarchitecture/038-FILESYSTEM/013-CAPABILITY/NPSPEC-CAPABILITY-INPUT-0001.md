# NPSPEC-CAPABILITY-INPUT-0001 – Nova Capability Input

## Status

Angenommen

## Kategorie

Capability / Input

## Zweck

NovaOS definiert das Eingabemodell für Capability-Aufrufe.

Capability-Eingaben werden über klar definierte semantische Typen und Verträge beschrieben, damit Provider austauschbar bleiben und Daten unabhängig von konkreten Speicherformaten verarbeitet werden können.

## Grundprinzipien

```text
Input ≠ Authority
Input Type ≠ Physical Format
Input Reference ≠ Ownership
Input Access ≠ Write Access
Valid Type ≠ Valid Content
Capability Input ≠ Provider-specific Structure
```

## Input-Modell

Eine Capability-Operation kann ihre Eingaben deklarieren als:

```text
CapabilityInput
├── Name
├── SemanticTypeID
├── Cardinality
├── Required
├── Constraints
└── TransferMode
```

Optional:

```text
DefaultValue
ValidationRules
SizeLimit
Mutability
Lifetime
```

## Semantische Typen

Eingaben sollen über die Type Registry beschrieben werden:

```text
Input
  ↓
SemanticTypeID
  ↓
Type Registry
  ↓
Capability Interface
```

Beispiel:

```text
Image
  ↓
de.nova.image.filter.gaussian
  ↓
Image
```

Das konkrete Format des Bildes kann davon unabhängig sein.

## Mehrere Eingaben

Eine Operation darf mehrere benannte Eingaben besitzen:

```text
GaussianFilter
├── image
├── radius
└── strength
```

Jede Eingabe besitzt ihren eigenen Typ und eigene Constraints.

## Referenzen

Große oder bereits vorhandene Daten müssen nicht kopiert werden.

Eingaben können bereitgestellt werden als:

```text
Value
ObjectID
Authorized Handle
Shared Buffer
Stream
```

Der verwendete Übergabemechanismus darf die semantische Bedeutung nicht verändern.

## Zero-Copy

Wenn möglich, darf NovaOS Eingaben über kontrollierte gemeinsame Buffer oder Mappings bereitstellen:

```text
Producer
   ↓
Shared / Mapped Buffer
   ↓
Capability Provider
```

Ist Zero-Copy nicht sicher oder möglich, muss ein kontrollierter Copy-Fallback verwendet werden können.

## Validierung

Vor der Verarbeitung muss eine Capability Eingaben gegen ihren Interface-Vertrag validieren können:

```text
Input
 ↓
Type Check
 ↓
Constraint Check
 ↓
Authority Check
 ↓
Execution
```

Ungültige Eingaben müssen kontrolliert abgelehnt werden.

## Authority

Eine Eingabereferenz gewährt nur die für den Aufruf vorgesehene Authority.

```text
Input Handle
   ↓
Read-only Authority
```

darf beispielsweise nicht automatisch Schreib- oder Delegationsrechte erzeugen.

Die Capability darf ihre Input-Authority nicht über den übergebenen Scope hinaus erweitern.

## Lifetime

Referenzierte Eingaben benötigen eine definierte Lebensdauer.

```text
Call
Task
Stream
Explicit Lifetime
```

Ein Provider darf nach Ablauf der Lebensdauer nicht weiter auf die Eingabe zugreifen.

## Streaming

Capabilities müssen Streaming-Eingaben unterstützen können:

```text
Producer
   ↓
Stream
   ↓
Backpressure
   ↓
Capability
```

Streaming muss Cancellation, Deadline und Ressourcenlimits berücksichtigen können.

## Normative Anforderungen

1. Capability-Eingaben MÜSSEN durch das Capability Interface beschreibbar sein.
2. Eingaben SOLLEN stabile semantische Typen verwenden.
3. Semantischer Typ und physisches Datenformat MÜSSEN getrennt bleiben.
4. Operationen MÜSSEN mehrere benannte Eingaben unterstützen können.
5. Eingaben MÜSSEN vor Verarbeitung validierbar sein.
6. Eingabereferenzen DÜRFEN keine zusätzliche Authority erzeugen.
7. Übergebene Handles MÜSSEN auf die erforderliche Authority beschränkbar sein.
8. Value-, Handle-, Buffer- und Stream-basierte Übergabe MÜSSEN unterstützt werden können.
9. Zero-Copy SOLL verwendet werden können, wenn Sicherheit und Kompatibilität dies erlauben.
10. Ein sicherer Copy-Fallback MUSS verfügbar sein.
11. Referenzierte Eingaben MÜSSEN eine definierte Lebensdauer besitzen.
12. Streaming-Eingaben MÜSSEN Backpressure und Cancellation unterstützen können.
13. Input-Typen, Constraints und Transfermodus MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-REGISTRY-TYPE-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein typisiertes und providerunabhängiges Eingabemodell für Capabilities. Daten können als Werte, Objektreferenzen, autorisierte Handles, Buffer oder Streams übergeben werden, während semantischer Typ, physisches Format, Lebensdauer und Authority klar voneinander getrennt bleiben.