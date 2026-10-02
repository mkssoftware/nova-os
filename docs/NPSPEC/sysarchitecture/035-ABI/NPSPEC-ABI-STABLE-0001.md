# NPSPEC-ABI-STABLE-0001 – Nova Stable ABI

## Status

Angenommen

## Kategorie

ABI / Compatibility / Binary Stability

## Zweck

NovaOS definiert eine langfristig stabile ABI, damit bereits kompilierte Programme, Services, Treiber und Systemkomponenten auch nach internen Änderungen des Betriebssystems weiter funktionieren können.

```text
Binary
  ↓
Stable ABI Contract
  ↓
NovaOS Version N
NovaOS Version N+1
NovaOS Version N+2
```

Die interne Implementierung darf sich weiterentwickeln, ohne unnötig Binärkompatibilität zu brechen.

## Grundprinzipien

```text
Stable ABI ≠ Frozen ABI
Stable ABI ≠ Frozen Kernel
Stable ABI ≠ Stable Implementation
Binary Compatibility ≠ Semantic Compatibility
Deprecated ≠ Removed
Internal Structure ≠ Public ABI
```

Stabilität wird durch klar definierte öffentliche Verträge erreicht, nicht durch das Einfrieren interner Strukturen.

## Stability Model

```text
StableABI
├── ABI_ID
├── MajorVersion
├── MinorVersion
├── Architecture
├── FeatureSet
├── CompatibilityLevel
└── LifecycleState
```

## Compatibility Levels

NovaOS unterscheidet:

```text
Stable
Extended
Experimental
Internal
Deprecated
```

### Stable

Langfristig zugesicherte Binärkompatibilität.

### Extended

Optionale ABI-Erweiterung mit definiertem Compatibility Contract.

### Experimental

Darf sich zwischen Versionen ändern.

### Internal

Nur für interne NovaOS-Komponenten ohne externes Stabilitätsversprechen.

### Deprecated

Noch unterstützt, soll aber nicht mehr für neue Software verwendet werden.

## Versioning

```text
Major Version
→ inkompatible Änderungen möglich

Minor Version
→ kompatible Erweiterungen
```

Beispiel:

```text
Nova ABI 1.0
Nova ABI 1.1
Nova ABI 1.2
```

Ein Programm für ABI `1.0` soll grundsätzlich unter einer kompatiblen `1.x`-Implementierung funktionieren.

## Additive Evolution

Neue Funktionen sollen bevorzugt additiv eingeführt werden.

```text
ABI 1.0
├── Feature A
└── Feature B

ABI 1.1
├── Feature A
├── Feature B
└── Feature C
```

Bestehende Semantik darf dabei nicht stillschweigend verändert werden.

## Stable Identifiers

Öffentliche IDs müssen innerhalb ihres Compatibility Contracts stabil bleiben.

Beispiele:

```text
Syscall IDs
Error Codes
Feature IDs
Structure Versions
Protocol IDs
ABI Flags
```

Entfernte IDs dürfen nicht mit inkompatibler Bedeutung wiederverwendet werden.

## Stable Structures

Öffentliche Strukturen sollen evolutionstauglich aufgebaut sein.

```text
struct NovaObjectInfo {
    uint32_t size;
    uint32_t version;

    ...
};
```

Neue Felder sollen nach Möglichkeit angehängt werden.

Consumer dürfen nur Felder verwenden, die durch Größe und Version garantiert vorhanden sind.

## Reserved Fields

Strukturen können reservierte Bereiche besitzen:

```text
reserved[4]
```

Reservierte Felder müssen initial definierten Regeln folgen und dürfen von Anwendungen nicht für eigene Daten verwendet werden.

## Flags

Neue Flags dürfen ergänzt werden.

Unbekannte Flags müssen entsprechend der jeweiligen ABI-Regel:

```text
Reject
oder
Ignore Safely
```

behandelt werden.

Kritische unbekannte Flags dürfen nicht stillschweigend ignoriert werden.

## Data Layout

Stabile ABI-Typen müssen eindeutig definieren:

```text
Size
Alignment
Signedness
Padding
Byte Order
Representation
```

Compilerabhängige Layout-Annahmen sollen an öffentlichen ABI-Grenzen vermieden werden.

## Handles

Handles müssen von internen Kernelobjekten entkoppelt bleiben.

```text
Stable Handle
     ↓
Internal Resolution
     ↓
Current Kernel Object
```

Dadurch können interne Kernelstrukturen geändert werden, ohne die ABI zu verändern.

## Syscall Stability

Stabile Syscalls müssen garantieren:

```text
Stable Syscall ID
Stable Argument Semantics
Stable Result Semantics
Stable Error Semantics
```

Die interne Implementierung darf vollständig ersetzt werden.

```text
Stable Interface
      ≠
Stable Implementation
```

## Feature Discovery

Neue Funktionen sollen über Feature Discovery erkannt werden.

```text
Query Feature
     ↓
Supported?
├── Yes → Use
└── No  → Fallback
```

Versionsnummern allein sollen nicht für jede Capability-Entscheidung verwendet werden.

## Semantic Stability

Binäre Kompatibilität allein reicht nicht aus.

Beispiel:

```text
Same Function Signature
        +
Different Meaning
        =
Semantic ABI Break
```

Verhalten, Fehlersemantik und relevante Garantien gehören deshalb zum ABI Contract.

## Deprecation

ABI-Komponenten dürfen kontrolliert deprecated werden.

```text
Stable
  ↓
Deprecated
  ↓
Compatibility Period
  ↓
Optional Removal in Future Major ABI
```

Deprecation darf bestehende Programme nicht unmittelbar unbrauchbar machen.

## Compatibility Layer

NovaOS kann ältere ABI-Versionen durch Compatibility Layer unterstützen.

```text
Old Binary
    ↓
Legacy ABI
    ↓
Compatibility Layer
    ↓
Current NovaOS
```

Der Compatibility Layer übersetzt alte ABI-Semantik auf aktuelle Mechanismen.

## Security

ABI-Stabilität darf niemals verhindern, dass kritische Sicherheitsprobleme behoben werden.

```text
Compatibility
     ↓
Security Constraint
```

Bei Konflikten gilt:

```text
Safety
↓
Security
↓
Trust
↓
ABI Compatibility
```

Unsichere historische Semantik darf eingeschränkt, emuliert oder in Ausnahmefällen deaktiviert werden.

## Capability Evolution

Neue Capability-Regeln dürfen ältere Programme nicht automatisch mit zusätzlicher Authority ausstatten.

```text
Old Binary
    ↓
Compatibility Layer
    ↓
Current Capability Policy
```

```text
Compatibility ≠ Legacy Authority Preservation
```

Aktuelle Security Policy bleibt maßgeblich.

## Architecture Independence

ABI-Stabilität gilt innerhalb eines definierten Architekturprofils.

```text
Nova ABI
├── x86-32 Stable Profile
├── x86-64 Stable Profile
└── ARM64 Stable Profile
```

Binärkompatibilität zwischen unterschiedlichen CPU-Architekturen wird nicht vorausgesetzt.

## Language Independence

Die stabile ABI darf nicht von einer bestimmten Programmiersprache abhängig sein.

```text
C
C++
Rust
NovaLang
Other
  ↓
Stable Nova ABI
```

Sprachspezifische Laufzeitdetails bleiben außerhalb der grundlegenden ABI.

## Testing

Stable ABI muss automatisiert überprüfbar sein.

Tests sollen unter anderem erkennen:

```text
Changed Structure Layout
Changed Syscall ID
Changed Constant
Changed Calling Convention
Changed Error Semantics
Removed Feature
Changed Alignment
```

ABI-Kompatibilität soll Bestandteil der Build- und Release-Validierung sein.

## ABI Manifest

NovaOS soll maschinenlesbare ABI-Metadaten bereitstellen können.

```text
ABI Manifest
├── Version
├── Architecture
├── Syscalls
├── Structures
├── Constants
├── Features
└── Deprecated Interfaces
```

Dadurch können Toolchains und Compatibility Tests ABI-Unterschiede automatisch erkennen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ABI Version
Architecture Profile
Compatibility Level
Supported Features
Deprecated Features
Compatibility Layers
```

## Normative Anforderungen

1. NovaOS MUSS eine langfristig stabile öffentliche ABI definieren.
2. Öffentliche und interne ABI MÜSSEN getrennt bleiben.
3. Stable ABI DARF interne Kernelimplementierungen NICHT unnötig festschreiben.
4. Inkompatible Änderungen MÜSSEN eine neue Major ABI erfordern.
5. Kompatible Erweiterungen SOLLEN additiv erfolgen.
6. Bestehende öffentliche IDs DÜRFEN NICHT inkompatibel wiederverwendet werden.
7. Öffentliche Strukturen SOLLEN versionierbar und erweiterbar sein.
8. Datenlayout und Alignment MÜSSEN eindeutig spezifiziert werden.
9. Stable Handles MÜSSEN von internen Kernel-Pointern getrennt bleiben.
10. Syscall IDs und deren Semantik MÜSSEN innerhalb einer kompatiblen ABI stabil bleiben.
11. Binary Compatibility DARF NICHT mit Semantic Compatibility gleichgesetzt werden.
12. Feature Discovery SOLL für optionale Erweiterungen verwendet werden.
13. Deprecated Interfaces MÜSSEN eindeutig erkennbar sein.
14. Entfernung stabiler Interfaces SOLL grundsätzlich nur über eine neue Major ABI erfolgen.
15. Compatibility Layer DÜRFEN aktuelle Security-Regeln NICHT umgehen.
16. ABI Compatibility DARF notwendige Sicherheitskorrekturen NICHT verhindern.
17. Alte Programme DÜRFEN durch Compatibility NICHT zusätzliche Authority erhalten.
18. ABI-Stabilität MUSS pro Architekturprofil definiert werden.
19. Die öffentliche ABI SOLL sprachneutral bleiben.
20. ABI-Kompatibilität MUSS automatisiert testbar sein.
21. ABI-Breaks MÜSSEN während Build- oder Release-Validierung erkennbar sein.
22. NovaOS SOLL ein maschinenlesbares ABI Manifest bereitstellen.
23. ABI-Versionen und Features MÜSSEN introspektierbar sein.
24. Stable ABI MUSS langfristige Evolution ohne unnötige Neukompilierung bestehender Software ermöglichen.

## Abhängigkeiten

- `NPSPEC-ABI-NOVA-0001`
- `NPSPEC-ABI-SYSCALL-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-VERSIONING-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-TRUST-SOFTWARE-0001`
- `NPSPEC-OBSERVABILITY-INTROSPECTION-0001`
- `ADR-ARCH-0143`

## Ergebnis

```text
Stable ABI Contract
        ↓
Stable IDs
Stable Types
Stable Semantics
Feature Discovery
Versioning
        ↓
Internal NovaOS Evolution
        ↓
Kernel Changes
Service Changes
Implementation Changes
Architecture Improvements
        ↓
Existing Binary Continues Running
```

NovaOS erhält damit eine langfristig stabile und gleichzeitig evolvierbare ABI, die bestehende Binärsoftware von internen Systemänderungen entkoppelt, ohne Kernelentwicklung, Sicherheitsverbesserungen oder zukünftige Erweiterungen einzuschränken.