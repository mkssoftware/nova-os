# NPSPEC-ARCH-ZEROCOPY-0001 – Nova Zero-Copy

## Status

Angenommen

## Kategorie

Architecture / Data Movement / Memory / Performance

## Zweck

NovaOS vermeidet unnötige Datenkopien zwischen Komponenten, Prozessen und Hardware.

```text
Data
 ↓
Shared / Mapped Buffer
 ↓
Consumer
```

Zero-Copy ist eine Optimierung und darf Sicherheit oder Korrektheit nicht beeinträchtigen.

## Grundprinzip

Statt:

```text
Producer
 ↓ Copy
Kernel
 ↓ Copy
Service
 ↓ Copy
Consumer
```

soll, wo möglich:

```text
Producer
      ↓
Shared Buffer
      ↓
Consumer
```

verwendet werden.

## Buffer-Modell

Daten können über kontrollierte Buffer referenziert werden.

```text
BufferID
Owner
Size
Type
Access Rights
Lifetime
State
```

Der Buffer kann dabei zwischen mehreren autorisierten Komponenten geteilt werden.

## Zugriff

Zugriffsrechte werden explizit vergeben.

```text
Read
Write
ReadWrite
Transfer
```

Eine Buffer-Referenz erzeugt keine zusätzlichen Berechtigungen auf die enthaltenen Daten.

## Ownership

Ownership und Zugriff müssen eindeutig definiert sein.

Beispiele:

```text
Exclusive Ownership
Shared Read
Controlled Transfer
```

Ownership-Wechsel müssen synchronisiert erfolgen.

## Hardware

Zero-Copy soll auch Hardwarepfade unterstützen können.

Beispiele:

```text
Storage → Memory
Network → Memory
Memory → GPU
GPU → Display
Camera → GPU
```

DMA und Shared Memory können hierfür verwendet werden.

## Synchronisation

Gemeinsam verwendete Buffer benötigen explizite Synchronisation.

```text
Producer Ready
      ↓
Buffer Available
      ↓
Consumer
      ↓
Release
```

Unkontrollierte Race Conditions dürfen nicht durch Zero-Copy entstehen.

## Fallback

Ist Zero-Copy nicht sicher oder technisch möglich:

```text
Zero-Copy unavailable
        ↓
Controlled Copy
```

Korrektheit und Isolation haben Vorrang vor Performance.

## Normative Anforderungen

1. NovaOS SOLL unnötige Datenkopien systemweit vermeiden.
2. Zero-Copy MUSS über kontrollierte Buffer- oder Objekt-Referenzen erfolgen.
3. Buffer MÜSSEN eindeutige Ownership- und Zugriffsregeln besitzen.
4. Geteilte Buffer MÜSSEN capability- und security-basiert geschützt werden.
5. Synchronisation zwischen Producer und Consumer MUSS explizit definiert sein.
6. Zero-Copy SOLL für CPU-, GPU-, Storage-, Network- und Display-Pfade nutzbar sein.
7. Zero-Copy DARF Speicherisolation und Security nicht umgehen.
8. Bei ungeeigneten Bedingungen MUSS ein sicherer Copy-Fallback möglich sein.
9. Zero-Copy DARF die semantische Bedeutung der übertragenen Daten nicht verändern.
10. Ressourcen MÜSSEN nach Ende ihrer Nutzung kontrolliert freigegeben werden.

## Abhängigkeiten

- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0009_Daten_und_Objekt_Pipelining`

## Ergebnis

```text
Producer
    ↓
Controlled Shared Data
    ↓
Consumer
```

NovaOS kann Daten damit effizient zwischen Komponenten und Hardware bewegen, ohne unnötige Kopien zu erzeugen oder Sicherheitsgrenzen aufzugeben.