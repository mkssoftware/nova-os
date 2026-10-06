# NPSPEC-COMPAT-SYSCALL-0001 – Nova Compatibility Syscall Translation

## Status

Angenommen

## Kategorie

Compatibility / Syscall

## Zweck

NovaOS definiert eine kontrollierte Übersetzungsschicht für Systemaufrufe fremder Betriebssysteme und ABIs.

Fremde Syscalls werden nicht Bestandteil der nativen NovaOS-Syscall-ABI, sondern durch den jeweiligen Compatibility Provider validiert, übersetzt und auf native NovaOS-Systemmechanismen abgebildet.

## Grundprinzipien

```text
Foreign Syscall ≠ Nova Syscall
Syscall Number ≠ Operation Identity
Syscall Compatibility ≠ Native ABI
Translation ≠ Authority
Foreign Privilege ≠ Nova Authority
Unsupported ≠ Silently Ignored
```

## Modell

```text
Foreign Process
      ↓
Foreign Syscall
      ↓
ABI / Personality Detection
      ↓
Syscall Translation
      ↓
Argument Validation
      ↓
Capability / Policy Check
      ↓
Nova System Interface
      ↓
Result Translation
      ↓
Foreign Process
```

## Syscall-Profil

Jede unterstützte Syscall-Schnittstelle wird durch ein versioniertes Profil beschrieben:

```text
SyscallProfile
├── PersonalityID
├── ABI-ID
├── Architecture
├── Version
├── CallingConvention
├── SyscallTable
└── ErrorModel
```

Die Bedeutung eines Syscalls wird durch das Profil bestimmt und nicht allein durch seine numerische Nummer.

## Übersetzung

Ein Mapping beschreibt die Abbildung eines fremden Syscalls:

```text
Foreign Syscall
├── Number
├── Operation
├── Arguments
├── Structures
├── Flags
└── Expected Result
        ↓
Translation
        ↓
Nova Operation
```

Dabei dürfen Argumente, Datenstrukturen, Flags und Rückgabewerte konvertiert werden.

## Direkte Abbildung

Wenn NovaOS eine semantisch passende Operation besitzt:

```text
Foreign Syscall
      ↓
Validate
      ↓
Native Nova Operation
```

Die Übersetzung soll möglichst direkt erfolgen.

## Emulation

Existiert keine direkte Entsprechung, darf die Compatibility-Schicht einen Syscall aus mehreren nativen Operationen emulieren:

```text
Foreign Syscall
      ↓
Compatibility Provider
      ├── Nova Operation A
      ├── Nova Operation B
      └── Nova Operation C
```

Die beobachtbare Semantik des fremden Systems soll soweit möglich erhalten bleiben.

## Handles und Ressourcen

Fremde Deskriptoren oder Handles werden niemals ungeprüft als native Handles verwendet.

```text
Foreign Handle / FD
        ↓
Compatibility Handle Table
        ↓
Authorized Nova Handle
        ↓
Resource
```

Die Übersetzung darf keine zusätzliche Authority erzeugen.

## Speicherzugriffe

Pointer und Speicherbereiche aus fremden Syscalls müssen vor ihrer Verwendung validiert werden.

Die Compatibility-Schicht muss insbesondere prüfen:

```text
Address Range
Access Rights
Size
Alignment
Lifetime
Structure Layout
```

Ungültige Speicherreferenzen müssen kontrolliert abgewiesen werden.

## Capability-Integration

Ein fremder Syscall darf geschützte NovaOS-Ressourcen nur verwenden, wenn die notwendige Authority vorhanden ist.

```text
Foreign Syscall
      ↓
Requested Operation
      ↓
Existing Authority ∩ Policy
      ↓
Authorized Nova Operation
```

Syscall-Kompatibilität erzeugt selbst keine Capability.

## Blocking und Async

Blockierende fremde Syscalls dürfen auf native asynchrone NovaOS-Mechanismen abgebildet werden.

Die Compatibility-Schicht stellt dabei die vom fremden ABI erwartete beobachtbare Semantik bereit.

Cancellation, Deadline und Ressourcenlimits bleiben durch NovaOS kontrollierbar.

## Fehlerübersetzung

Native Fehler werden in das Fehlerformat der jeweiligen Personality übersetzt:

```text
Nova Error
    ↓
Compatibility Mapping
    ↓
errno / Status Code / Foreign Error
```

Der ursprüngliche NovaOS-Fehler darf für Diagnose und Introspection erhalten bleiben.

## Nicht unterstützte Syscalls

Ein nicht unterstützter Syscall muss einen definierten Compatibility-Fehler erzeugen.

Er darf weder stillschweigend erfolgreich sein noch Sicherheitsprüfungen umgehen.

## Performance

Häufig verwendete und sicher direkt abbildbare Syscalls dürfen optimierte Translation Paths verwenden.

Optimierungen dürfen Validierung, Capability-Prüfung, Isolation oder Policy Enforcement nicht umgehen.

## Normative Anforderungen

1. Fremde Syscalls DÜRFEN nicht Bestandteil der nativen NovaOS-Syscall-ABI werden.
2. Syscall-Schnittstellen MÜSSEN ABI-, Architektur- und versionsabhängig beschreibbar sein.
3. Syscall-Nummern DÜRFEN nicht ohne zugehöriges Syscall-Profil interpretiert werden.
4. Argumente und fremde Speicherreferenzen MÜSSEN vor Verwendung validiert werden.
5. Fremde Handles und File Descriptors MÜSSEN auf autorisierte NovaOS-Handles abgebildet werden.
6. Syscall-Übersetzung DARF keine zusätzliche Authority erzeugen.
7. Geschützte Operationen MÜSSEN Capability- und Policy-Prüfungen unterliegen.
8. Nicht direkt abbildbare Syscalls DÜRFEN kontrolliert emuliert werden.
9. Nicht unterstützte Syscalls MÜSSEN deterministisch und kontrolliert fehlschlagen.
10. Native Fehler MÜSSEN auf das jeweilige Compatibility-Fehlermodell abbildbar sein.
11. Optimierte Translation Paths DÜRFEN Sicherheitsprüfungen nicht umgehen.
12. Syscall-Profil, Mapping, Emulationsstatus und Fehler MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-POSIX-0001`
- `NPSPEC-COMPAT-LINUX-0001`
- `NPSPEC-COMPAT-WIN32-0001`
- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS kann fremde Systemaufrufe über versionierte Syscall-Profile kontrolliert auf native Systemmechanismen abbilden. Fremde Syscall-Nummern, Argumente, Handles, Speicherstrukturen und Fehlermodelle bleiben Bestandteil der jeweiligen Compatibility Personality, während die native NovaOS-Syscall-ABI, Capability-Sicherheit und Systemarchitektur unabhängig bleiben.