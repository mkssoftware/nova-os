# ADR-COMPAT-0011 – Binary Translation für Cross-ISA

## Status

Angenommen

## Kategorie

Compatibility / Binary Translation / ISA / Execution

## Kontext

NovaOS soll Software ausführen können, die für eine andere CPU-Architektur kompiliert wurde als die vorhandene Hardware.

Beispiele:

```text
x86-64 Binary → ARM64 System
ARM64 Binary  → x86-64 System
x86 Binary    → ARM64 System
```

API- oder ABI-Kompatibilität allein reicht hierfür nicht aus, da die Maschinenbefehle der fremden Instruction Set Architecture (ISA) nicht direkt ausgeführt werden können.

## Entscheidung

NovaOS unterstützt **Binary Translation als Cross-ISA-Ausführungsmechanismus** innerhalb der Execution Compatibility Ladder.

```text
Foreign Binary
      ↓
ISA Detection
      ↓
Binary Translation
      ↓
Native Host Instructions
      ↓
NovaOS
```

Binary Translation bleibt eine Compatibility-Funktion und beeinflusst nicht die native NovaOS-Architektur.

## Translation-Modi

NovaOS darf unterschiedliche Verfahren verwenden:

```text
Interpretation
Static Binary Translation
Dynamic Binary Translation
JIT Translation
```

Die konkrete Methode kann abhängig von Anwendung, Architektur und Execution Contract gewählt werden.

## Translation Cache

Bereits übersetzte Codeblöcke dürfen zwischengespeichert werden.

```text
Guest Code
    ↓
Translate
    ↓
Native Code
    ↓
Translation Cache
```

Cache-Einträge müssen eindeutig an Binary-Version, Guest ISA, Host ISA und relevante Translation-Konfiguration gebunden sein.

## ABI und Systemaufrufe

ISA-Translation und Betriebssystem-Kompatibilität bleiben getrennte Aufgaben.

```text
Guest Instructions
      ↓
ISA Translation
      ↓
Guest API / ABI
      ↓
Compatibility Personality
      ↓
NovaOS
```

Ein übersetztes Linux- oder Win32-Binary benötigt weiterhin die entsprechende Compatibility Personality.

## Speicher und Ausführung

Binary Translation muss Unterschiede zwischen Architekturen berücksichtigen, insbesondere:

```text
Register Model
Calling Convention
Memory Ordering
Atomic Operations
Alignment
Endianness
Instruction Semantics
```

Semantische Unterschiede dürfen nicht stillschweigend ignoriert werden.

## Sicherheit

Generierter nativer Code unterliegt denselben Sicherheitsregeln wie andere ausführbare Inhalte.

Insbesondere müssen gelten:

```text
W^X
Executable Memory Policy
Capability Boundaries
Sandboxing
Code Integrity
Resource Budgets
```

Binary Translation darf keine zusätzlichen Systemrechte erzeugen.

## Performance

Binary Translation darf optimiert werden, solange die erforderliche Semantik erhalten bleibt.

NovaOS kann häufig ausgeführte Codepfade erkennen und stärker optimieren.

```text
Execution
   ↓
Profiling
   ↓
Hot Code
   ↓
Optimized Translation
```

Adaptive Optimierung darf Hard Constraints nicht verletzen.

## Fallback

Kann eine Anwendung nicht zuverlässig übersetzt werden, kann NovaOS innerhalb der Compatibility Ladder auf stärkere Isolation oder vollständige Emulation zurückfallen.

```text
Binary Translation
        ↓ failure
Emulation / VM
```

Ein Fallback muss explizit erkennbar sein.

## Normative Anforderungen

1. NovaOS MUSS Cross-ISA-Ausführung über Binary Translation unterstützen können.
2. Guest ISA und Host ISA MÜSSEN eindeutig erkannt werden.
3. ISA-Translation und Betriebssystem-Kompatibilität MÜSSEN getrennte Schichten bleiben.
4. Architekturunterschiede bei Speicher-, Atomics- und Instruktionssemantik MÜSSEN berücksichtigt werden.
5. Generierter Code MUSS den normalen NovaOS-Sicherheitsregeln unterliegen.
6. Binary Translation DARF keine zusätzliche Autorität erzeugen.
7. Translation Caches MÜSSEN an eindeutige Binary- und Translation-Versionen gebunden sein.
8. Optimierungen DÜRFEN die geforderte Ausführungssemantik nicht verändern.
9. Nicht zuverlässig übersetzbare Software MUSS definiert fehlschlagen oder auf eine andere Compatibility-Stufe wechseln können.

## Konsequenzen

### Positiv

- Software anderer CPU-Architekturen kann ausgeführt werden,
- Migration zwischen Hardwarearchitekturen wird erleichtert,
- bestehende Softwarebestände bleiben länger nutzbar,
- Translation kann durch Caching und adaptive Optimierung beschleunigt werden.

### Negativ

- Binary Translation erzeugt zusätzlichen Laufzeitaufwand,
- unterschiedliche Speicher- und Instruktionsmodelle erhöhen die Komplexität,
- nicht jede Software lässt sich effizient oder vollständig übersetzen.

## Abhängigkeiten

- `ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen`
- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0007_Execution_Compatibility_Ladder`
- `ADR-COMPAT-0008_Legacy_Software_in_Compatibility_Domains`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0010_Automatic_Execution_Planning`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

NovaOS trennt CPU-Architektur und Betriebssystem-Kompatibilität:

```text
Foreign ISA
    ↓
Binary Translation
    ↓
Compatibility Personality
    ↓
Native NovaOS
```

Damit kann Software für fremde Prozessorarchitekturen ausgeführt werden, ohne die native NovaOS-ISA oder Systemarchitektur danach auszurichten.