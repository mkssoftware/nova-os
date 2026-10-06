# NPSPEC-COMPAT-ABI-0001 – Nova Compatibility ABI

## Status

Angenommen

## Kategorie

Compatibility / ABI

## Zweck

NovaOS definiert eine kontrollierte ABI-Kompatibilitätsschicht für binäre Software, die nicht gegen die native NovaOS-ABI erstellt wurde.

Die Compatibility ABI übersetzt fremde binäre Aufrufkonventionen und Laufzeitannahmen auf native NovaOS-Mechanismen, ohne die fremde ABI zum Bestandteil des nativen NovaOS-Kerns zu machen.

## Grundprinzipien

```text
Compatibility ABI ≠ Native Nova ABI
ABI Compatibility ≠ API Compatibility
ABI Compatibility ≠ Runtime Compatibility
Compatibility ≠ Authority
Binary Compatibility ≠ Trust
Legacy ABI ≠ Kernel ABI
```

## Modell

```text
Foreign Binary
      ↓
Compatibility Detection
      ↓
ABI Profile
      ↓
Compatibility Provider
      ↓
Nova ABI / System Interfaces
      ↓
NovaOS
```

## ABI-Profil

Eine unterstützte ABI wird durch ein versioniertes Profil beschrieben:

```text
CompatibilityABI
├── ABI-ID
├── Architecture
├── CallingConvention
├── DataLayout
├── Alignment
├── SymbolModel
├── BinaryFormat
├── ThreadModel
├── ErrorModel
└── Version
```

Optional können weitere ABI-spezifische Eigenschaften definiert werden.

## Binärformate

Die Compatibility-Schicht darf unterschiedliche Binärformate unterstützen, beispielsweise:

```text
PE
ELF
WASM
Other Registered Formats
```

Das Binärformat allein bestimmt nicht die vollständige ABI.

## Aufrufkonvention

Der Compatibility Provider muss ABI-spezifische Unterschiede kontrolliert behandeln können:

```text
Registers
Stack Layout
Parameter Passing
Return Values
Alignment
Structure Layout
Symbol Resolution
Exception / Error Boundaries
```

## Architektur

ABI-Kompatibilität ist architekturabhängig.

```text
x86
x86-64
ARM
Other Architectures
```

Ist zusätzlich eine andere CPU-Architektur erforderlich, kann eine separate Emulations- oder Übersetzungsschicht notwendig sein.

## Systemaufrufe

Fremde Systemaufrufe dürfen nicht ungeprüft direkt an den NovaOS-Kernel weitergereicht werden.

```text
Foreign System Call
       ↓
Compatibility Provider
       ↓
Validation
       ↓
Nova System Interface
```

Die native Kernel-ABI bleibt dadurch unabhängig von fremden ABI-Modellen.

## Capability-Sicherheit

Fremde ABI-Aufrufe dürfen das NovaOS-Capability-Modell nicht umgehen.

```text
Foreign API / ABI Request
        ↓
Compatibility Translation
        ↓
Capability / Policy Check
        ↓
NovaOS Operation
```

Ein fremdes Konzept wie Administrator-, Root- oder Prozessprivileg darf nicht automatisch in uneingeschränkte NovaOS-Authority übersetzt werden.

## Isolation

Compatibility Provider sollen außerhalb des kritischen Kernel-Kerns betrieben werden, soweit dies technisch möglich ist.

Fehler in einer fremden ABI oder deren Übersetzung dürfen den Kernel nicht unnötig gefährden.

## Versionierung

Mehrere ABI-Versionen dürfen parallel unterstützt werden:

```text
ABI-ID
├── Version 1
├── Version 2
└── Version 3
```

Programme werden gegen ein geeignetes ABI-Profil aufgelöst.

## Fallback

Kann eine ABI nicht nativ übersetzt werden, darf NovaOS weitere Compatibility Provider verwenden:

```text
Native Compatibility
        ↓ unavailable
Binary Translation
        ↓ unavailable
Emulation
        ↓
Result
```

Sicherheits- und Hard Requirements dürfen dabei nicht umgangen werden.

## Normative Anforderungen

1. Fremde ABIs MÜSSEN von der nativen NovaOS-ABI getrennt bleiben.
2. Jede unterstützte fremde ABI MUSS eindeutig identifizierbar und versionierbar sein.
3. Calling Convention, Data Layout und Alignment MÜSSEN ABI-spezifisch beschreibbar sein.
4. Mehrere ABI-Versionen MÜSSEN parallel unterstützt werden können.
5. Fremde Systemaufrufe DÜRFEN nicht ungeprüft direkt auf Kernelmechanismen abgebildet werden.
6. Compatibility Provider MÜSSEN NovaOS-Sicherheitsgrenzen einhalten.
7. ABI-Kompatibilität DARF keine Authority oder Permission erzeugen.
8. Fremde Privilegmodelle DÜRFEN das Capability-Modell nicht umgehen.
9. Nicht unterstützte ABI-Funktionen MÜSSEN kontrolliert fehlschlagen.
10. Binary Translation und Emulation DÜRFEN als Fallback verwendet werden.
11. Compatibility Provider SOLLEN soweit möglich vom kritischen Kernel isoliert sein.
12. ABI-Profil, Provider, Version und Kompatibilitätszustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-PROGRAM-LEGACY-0001`
- `NPSPEC-PROGRAM-COMPATIBILITY-0001`
- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-FOUNDATION-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-CAPABILITY-COMPATIBILITY-0001`

## Ergebnis

NovaOS kann Software mit fremden binären Schnittstellen über versionierte Compatibility Provider ausführen, ohne diese ABIs in die native Systemarchitektur zu übernehmen. Fremde Calling Conventions, Binärformate und Systemaufrufe werden kontrolliert auf NovaOS-Mechanismen abgebildet, während Capability-Sicherheit, Isolation und die native NovaOS-ABI erhalten bleiben.