# NPSPEC-REGISTRY-CODEC-0001 – Nova Codec Registry

## Status

Angenommen

## Kategorie

Registry / Codec

## Zweck

NovaOS definiert die Codec Registry als systemweites Verzeichnis verfügbarer Encoder, Decoder und Transcoder.

Sie ermöglicht die Auswahl geeigneter Codec-Implementierungen anhand von Datentyp, Format, Hardware, Ressourcen und Execution Contract.

## Grundprinzipien

```text
CodecID ≠ Implementation
Codec ≠ Format
Codec ≠ Provider
Discovery ≠ Authority
Codec Support ≠ Hardware Support
```

## Registry-Modell

Ein Codec-Eintrag kann enthalten:

```text
CodecRegistryEntry
├── CodecID
├── Version
├── Operations
├── InputTypes
├── OutputTypes
├── Formats
├── Implementations
└── State
```

Optional:

```text
HardwareAcceleration
PerformanceProfile
ResourceRequirements
QualityProfile
Determinism
TrustRequirements
Compatibility
```

## Operationen

Ein Codec kann eine oder mehrere Operationen bereitstellen:

```text
Decode
Encode
Transcode
Parse
Mux
Demux
```

## Implementierungen

Ein Codec darf mehrere Implementierungen besitzen:

```text
CodecID
├── Software Implementation
├── SIMD Implementation
├── GPU Implementation
└── Hardware Codec
```

Die konkrete Implementierung bleibt von der logischen Codec-Identität getrennt.

## Discovery

```text
Input Type / Format
        ↓
Codec Registry
        ↓
Compatible Codecs
        ↓
Compatible Implementations
        ↓
Policy / Execution Contract
        ↓
Selected Implementation
```

Die Registry darf hierzu Informationen aus Type-, Device- und Algorithm Registry verwenden.

## Auswahl

Die Auswahl kann berücksichtigen:

```text
Input / Output Type
Format
Quality
Latency
Deadline
Resource Budget
Energy
Hardware Availability
Determinism
Trust
User Preference
```

Hardwarebeschleunigung darf bevorzugt werden, wenn sie verfügbar, autorisiert und mit den Anforderungen kompatibel ist.

## Format und semantischer Typ

Physisches Format und semantischer Typ bleiben getrennt.

```text
Semantic Type
     ↓
Physical Format
     ↓
Codec
```

Beispielsweise kann derselbe semantische Medientyp durch unterschiedliche Formate und Codecs repräsentiert werden.

## Sicherheit

Codec Discovery erzeugt keine Authority.

Benötigt eine Implementierung Zugriff auf GPU, Accelerator, geschützten Speicher oder andere Geräte, müssen die erforderlichen Capabilities separat autorisiert werden.

Nicht vertrauenswürdige Codec-Implementierungen können isoliert ausgeführt werden.

## Normative Anforderungen

1. NovaOS MUSS eine Codec Registry bereitstellen.
2. Codecs MÜSSEN stabile `CodecID`s besitzen können.
3. Codec, Format und Implementierung MÜSSEN getrennt behandelt werden.
4. Ein Codec DARF mehrere Implementierungen besitzen.
5. Encoder, Decoder und Transcoder MÜSSEN beschreibbar sein.
6. Input- und Output-Typen MÜSSEN deklarierbar sein.
7. Hardwarebeschleunigte Implementierungen MÜSSEN registrierbar sein.
8. Die Auswahl MUSS Execution Contracts berücksichtigen können.
9. Discovery DARF keine Authority erzeugen.
10. Gerätezugriffe MÜSSEN separat capability-basiert autorisiert werden.
11. Codec-Versionen und Kompatibilität MÜSSEN unterscheidbar sein.
12. Codec, Implementierung, unterstützte Formate und Auswahlgrund MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-REGISTRY-0001`
- `NPSPEC-REGISTRY-TYPE-0001`
- `NPSPEC-REGISTRY-ALGORITHM-0001`
- `NPSPEC-REGISTRY-DEVICE-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Codec-Funktionen unabhängig von konkreten Implementierungen registrieren und dynamisch auswählen. Software-, GPU- und Hardware-Codecs können dadurch über ein gemeinsames Modell genutzt werden, während semantischer Typ, Format, Codec, Implementierung und Authority voneinander getrennt bleiben.