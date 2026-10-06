# NPSPEC-COMPAT-CPUEMULATION-0001 – Nova CPU Emulation

## Status

Angenommen

## Kategorie

Compatibility / CPU Emulation

## Zweck

NovaOS definiert CPU-Emulation für Binärprogramme, deren Prozessorarchitektur oder benötigte CPU-Funktionen auf dem Host nicht direkt verfügbar sind.

Die Emulation bildet den beobachtbaren Zustand einer fremden CPU kontrolliert auf NovaOS ab und dient insbesondere als Fallback, wenn native Ausführung oder Binary Translation nicht möglich oder nicht ausreichend ist.

## Grundprinzipien

```text
CPU Emulation ≠ Binary Translation
CPU Emulation ≠ ABI Translation
Guest CPU ≠ Host CPU
Guest Privilege ≠ Host Privilege
Emulated Address ≠ Native Address
Emulation ≠ Authority
```

## Modell

```text
Foreign Binary
      ↓
Guest CPU
      ↓
Instruction Decode
      ↓
CPU Emulator
      ↓
Emulated CPU State
      ↓
Compatibility Environment
      ↓
NovaOS
```

## CPU-Modell

Eine emulierte CPU wird mindestens beschrieben durch:

```text
CPUEmulation
├── GuestArchitecture
├── GuestISA
├── CPUProfile
├── RegisterState
├── ExecutionMode
├── MemoryModel
├── ExceptionModel
└── FeatureSet
```

Ein CPU-Profil darf konkrete Prozessorfunktionen oder ISA-Erweiterungen ein- beziehungsweise ausschalten.

## CPU-Zustand

Der Emulator verwaltet den für das Guest-System sichtbaren Prozessorzustand:

```text
General Registers
Instruction Pointer
Flags
Control State
Floating Point State
Vector State
Execution Mode
```

Nur der für das jeweilige CPU-Profil erforderliche Zustand muss implementiert werden.

## Instruktionsausführung

Guest-Instruktionen werden dekodiert und entsprechend ihrer definierten Semantik ausgeführt:

```text
Fetch
  ↓
Decode
  ↓
Validate
  ↓
Execute
  ↓
Update Guest State
```

Nicht unterstützte oder ungültige Instruktionen müssen einen definierten Emulationsfehler auslösen.

## Speicher

Guest-Adressen dürfen nicht direkt als Host-Adressen interpretiert werden.

```text
Guest Virtual Address
        ↓
Guest Memory Model
        ↓
Validated Mapping
        ↓
Nova Memory
```

Speicherzugriffe bleiben durch NovaOS-Speicherschutz und den Security Context der Compatibility-Umgebung begrenzt.

## Exceptions und Interrupts

CPU-spezifische Ereignisse dürfen emuliert werden:

```text
Exceptions
Faults
Traps
Interrupts
```

Sie bleiben Teil des Guest-CPU-Modells und dürfen nicht ungeprüft auf native Host-Interrupts oder Kernelmechanismen abgebildet werden.

## Privilegierte Instruktionen

Guest-Privilegstufen besitzen keine entsprechende native NovaOS-Authority.

```text
Guest Kernel Mode
       ≠
Nova Kernel Mode
```

Privilegierte Instruktionen müssen emuliert, abgefangen oder kontrolliert abgewiesen werden.

## Zusammenspiel mit Binary Translation

NovaOS darf CPU-Emulation und Binary Translation kombinieren:

```text
Guest Code
   ↓
Binary Translation
   ↓
Unsupported / Special Operation
   ↓
CPU Emulation
```

Alternativ darf vollständige Interpretation verwendet werden.

## Systemaufrufe

Bei der Ausführung normaler fremder Programme werden erkannte Systemgrenzen an die Compatibility-Schicht übergeben:

```text
Guest Program
      ↓
Foreign Syscall
      ↓
Compatibility Syscall Translation
      ↓
NovaOS
```

CPU-Emulation erzeugt dadurch keine eigene System-Authority.

## Determinismus

Ein Emulator darf einen deterministischen Ausführungsmodus bereitstellen.

Dieser kann insbesondere für:

```text
Debugging
Testing
Replay
Recovery
Verification
```

verwendet werden.

## Normative Anforderungen

1. Guest- und Host-CPU MÜSSEN logisch getrennt bleiben.
2. Guest-CPU-Zustand MUSS explizit verwaltet werden.
3. Guest-Adressen DÜRFEN nicht ungeprüft als Host-Adressen verwendet werden.
4. Guest-Privilegstufen DÜRFEN keine NovaOS-Authority erzeugen.
5. Privilegierte Instruktionen MÜSSEN emuliert, abgefangen oder kontrolliert abgewiesen werden.
6. Nicht unterstützte Instruktionen MÜSSEN kontrolliert fehlschlagen.
7. CPU-Exceptions und Interrupts DÜRFEN native Kernelgrenzen nicht umgehen.
8. CPU-Emulation DARF keine Capabilities oder Permissions erzeugen.
9. Systemoperationen MÜSSEN weiterhin über die zuständige Compatibility-Schicht ausgeführt werden.
10. CPU-Emulation und Binary Translation MÜSSEN kombinierbar sein.
11. Native Ausführung SOLL bevorzugt werden, wenn sie kompatibel und sicher möglich ist.
12. Guest-Architektur, CPU-Profil, FeatureSet und Emulationszustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-ABITRANSLATION-0001`
- `NPSPEC-COMPAT-BINARYTRANSLATION-0001`
- `NPSPEC-COMPAT-SYSCALL-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`

## Ergebnis

NovaOS kann fremde CPU-Architekturen und nicht vorhandene Prozessorfunktionen kontrolliert emulieren. Guest-Register, Instruktionen, Speicher, Exceptions und Privilegstufen bleiben innerhalb des emulierten CPU-Modells, während NovaOS-Speicherschutz, Capabilities und Kernelgrenzen uneingeschränkt maßgeblich bleiben.