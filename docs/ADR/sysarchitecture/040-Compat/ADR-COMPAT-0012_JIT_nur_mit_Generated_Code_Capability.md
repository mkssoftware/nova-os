# ADR-COMPAT-0012 – JIT nur mit Generated Code Capability

## Status

Angenommen

## Kategorie

Compatibility / JIT / Security / Capabilities

## Kontext

JIT-Systeme erzeugen zur Laufzeit neuen ausführbaren Maschinencode.

Dies wird unter anderem benötigt für:

```text
Binary Translation
Managed Runtimes
WebAssembly
Scripting Runtimes
Dynamic Optimization
```

Die freie Erzeugung ausführbaren Speichers würde jedoch zentrale NovaOS-Sicherheitsmechanismen wie W^X und Code Integrity schwächen.

## Entscheidung

NovaOS erlaubt JIT-generierten Code nur über eine explizite **Generated Code Capability**.

```text
Runtime
   ↓
Generated Code Capability
   ↓
Writable Code Buffer
   ↓
Finalize / Validate
   ↓
Executable Code
```

Ohne diese Capability darf ein Prozess keinen dynamisch erzeugten Speicher in ausführbaren Code überführen.

## Generated Code Capability

Die Capability autorisiert ausschließlich definierte Operationen zur dynamischen Codeerzeugung.

Sie kann begrenzen:

```text
Memory Size
Execution Context
Lifetime
Runtime Identity
Allowed Operations
Resource Budget
```

Der Besitz allgemeiner Speicherrechte reicht nicht zur Erzeugung ausführbaren Codes aus.

## W^X

NovaOS erzwingt grundsätzlich:

```text
Writable XOR Executable
```

Speicher darf nicht gleichzeitig beschreibbar und ausführbar sein.

Der typische Übergang lautet:

```text
Allocate Writable
      ↓
Generate Code
      ↓
Finalize
      ↓
Writable = false
Executable = true
```

Erneute Änderungen benötigen einen kontrollierten Übergang zurück in einen nicht ausführbaren Zustand.

## Finalisierung

Vor der Ausführung wird der erzeugte Code finalisiert.

Dabei können geprüft werden:

```text
Capability Validity
Memory Bounds
Execution Permissions
Integrity Metadata
Runtime Policy
Architecture Requirements
```

Die Finalisierung bildet die Sicherheitsgrenze zwischen Datenerzeugung und Codeausführung.

## Provenance

Generierter Code muss seiner Herkunft zuordenbar sein.

```text
Generated Code
     ↓
Runtime / Translator
Source Object
Generation Context
Code Version
```

Damit kann NovaOS nachvollziehen, welche Runtime den Code erzeugt hat.

## Isolation

Generierter Code übernimmt keine zusätzlichen Rechte.

```text
Generated Code Authority
        ⊆
Runtime Authority
```

Die Codeerzeugung darf insbesondere keine Capability-Eskalation ermöglichen.

## Revocation

Die Generated Code Capability muss entziehbar sein.

Nach Revocation darf kein neuer ausführbarer Code erzeugt werden.

Bereits erzeugter Code kann abhängig von Policy ebenfalls invalidiert oder aus der Ausführung entfernt werden.

## Compatibility

Binary Translator und Compatibility Runtimes können die Capability gezielt erhalten.

```text
Cross-ISA Binary
       ↓
Translator
       ↓
Generated Code Capability
       ↓
Native Code
```

Normale Legacy-Anwendungen benötigen diese Capability nicht automatisch.

## Normative Anforderungen

1. Dynamische Codeerzeugung MUSS eine Generated Code Capability erfordern.
2. Allgemeine Speicherrechte DÜRFEN keine implizite JIT-Autorität verleihen.
3. NovaOS MUSS W^X für generierten Code erzwingen.
4. Schreibbarer Code DARF nicht gleichzeitig ausführbar sein.
5. Der Übergang zu ausführbarem Code MUSS kontrolliert erfolgen.
6. Generierter Code DARF keine zusätzliche Autorität erhalten.
7. Generated Code Capabilities MÜSSEN begrenzbar und entziehbar sein.
8. Generierter Code MUSS seiner erzeugenden Runtime zuordenbar sein.
9. Compatibility Runtimes DÜRFEN JIT-Rechte nur explizit erhalten.
10. JIT-Optimierungen DÜRFEN Hard Security Constraints nicht umgehen.

## Konsequenzen

### Positiv

- JIT bleibt möglich, ohne W^X grundsätzlich aufzugeben,
- dynamische Codeerzeugung wird explizit autorisiert,
- Binary Translation und moderne Runtimes bleiben unterstützt,
- JIT-Missbrauch kann stärker isoliert werden.

### Negativ

- JIT-Runtimes benötigen zusätzliche Capability-Integration,
- Code-Finalisierung erzeugt zusätzlichen Verwaltungsaufwand.

## Abhängigkeiten

- `ADR-COMPAT-0011_Binary_Translation_für_Cross_ISA`
- `ADR-CAPABILITY-0001`
- `ADR-CAPABILITY-0010_Attenuation`
- `ADR-CAPABILITY-0011_Revocation`
- `ADR-VERIFY-0003_Memory_Safety`
- `ADR-VERIFY-0005_Capability_Safety`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

## Ergebnis

NovaOS erlaubt dynamische Codeerzeugung nach dem Prinzip:

```text
No Generated Code
Without Explicit Capability

Write
  ↓
Finalize
  ↓
Execute
```

Damit bleiben JIT, Binary Translation und dynamische Runtimes möglich, ohne allgemeine Schreibrechte automatisch in Code-Ausführungsrechte umzuwandeln.