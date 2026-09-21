# ADR-COMPAT-0005 – Linux ABI über Syscall Translation

## Status

Angenommen

## Kategorie

Compatibility / Linux / ABI / Syscalls

## Kontext

Viele Linux-Anwendungen erwarten nicht nur POSIX-APIs, sondern konkrete Linux-ABI-Semantik und Linux-Systemaufrufe.

Dazu gehören beispielsweise:

```text
Linux Syscalls
File Descriptors
Signals
Futex
epoll
mmap
ioctl
/proc
/sys
```

Eine direkte Übernahme der Linux-Syscall-Architektur in den NovaOS-Kernel würde jedoch die native NovaABI an Linux binden.

## Entscheidung

NovaOS unterstützt die Linux ABI über eine **Linux Compatibility Personality mit Syscall Translation**.

```text
Linux Binary
     ↓
Linux Syscall
     ↓
Linux Personality
     ↓
Syscall Translation
     ↓
Nova API / NovaABI
     ↓
NovaOS
```

Linux-Systemaufrufe werden übersetzt, emuliert oder kontrolliert abgelehnt.

Der NovaOS-Kernel implementiert keine Linux-Syscall-ABI als native Kernel-ABI.

## Syscall Translation

Die Translation Layer ordnet Linux-Systemaufrufe geeigneten NovaOS-Mechanismen zu.

Beispiel:

```text
Linux open()
     ↓
Linux FD Semantics
     ↓
Nova Object / Capability
```

Die Anwendung sieht weiterhin Linux-Semantik, während NovaOS intern seine eigenen Abstraktionen verwendet.

## Linux ABI

Die Personality kann Linux-spezifische ABI-Eigenschaften bereitstellen:

```text
Syscall Numbers
Calling Conventions
Structures
Error Codes
Flags
Signal Semantics
```

Diese Definitionen bleiben Bestandteil der Linux Compatibility Layer.

## Kernelobjekte

Linux-Konzepte werden nicht automatisch zu nativen NovaOS-Kernelobjekten.

```text
Linux PID
Linux FD
Linux Socket
Linux Signal
```

werden auf geeignete NovaOS-Objekte und Ausführungskontexte abgebildet.

## Nicht direkt übersetzbare Syscalls

Nicht jeder Linux-Syscall besitzt ein direktes NovaOS-Äquivalent.

Die Personality darf deshalb:

```text
Translate
Emulate
Compose
Virtualize
Reject
```

Ein nicht unterstützter Syscall muss einen definierten Linux-kompatiblen Fehler liefern.

## ioctl

`ioctl` benötigt besondere Behandlung, da darüber zahlreiche Linux-spezifische Geräte- und Treiberschnittstellen verfügbar sind.

```text
Linux ioctl
     ↓
Validated Compatibility Handler
     ↓
Nova Driver / Capability
```

Linux-Anwendungen erhalten dadurch keinen direkten unkontrollierten Zugriff auf NovaOS-Treiber.

## Sicherheit

Syscall Translation ist eine Security Boundary.

Jeder übersetzte Aufruf unterliegt weiterhin:

```text
Capabilities
Permissions
Sandbox
Resource Budgets
Trust
Data Sovereignty
```

Linux-Semantik darf keine native NovaOS-Sicherheitsregel umgehen.

## Versionierung

Die unterstützte Linux ABI wird unabhängig von der NovaABI versioniert.

```text
Linux ABI Version
        ≠
NovaABI Version
```

Dadurch kann Linux-Kompatibilität erweitert oder angepasst werden, ohne die native Kernel-ABI entsprechend zu verändern.

## Normative Anforderungen

1. Linux-ABI-Kompatibilität MUSS außerhalb der nativen NovaABI implementiert werden.
2. Linux-Syscalls MÜSSEN über eine definierte Translation Boundary verarbeitet werden.
3. Linux-Syscall-Nummern DÜRFEN keine native NovaABI-Struktur bestimmen.
4. Linux-Kernelobjekte MÜSSEN auf native NovaOS-Mechanismen abgebildet oder emuliert werden.
5. Nicht unterstützte Syscalls MÜSSEN definiert fehlschlagen.
6. `ioctl` DARF keinen unkontrollierten direkten Treiberzugriff ermöglichen.
7. Syscall Translation MUSS NovaOS-Security- und Capability-Regeln respektieren.
8. Die unterstützte Linux ABI MUSS unabhängig versionierbar sein.
9. Neue Linux-ABI-Funktionen DÜRFEN keine Erweiterung der NovaABI erzwingen, sofern sie außerhalb des Kernels abbildbar sind.

## Konsequenzen

### Positiv

- Linux-Binaries können ohne native Linux-Kernelarchitektur unterstützt werden,
- NovaABI bleibt klein und unabhängig,
- Linux-Kompatibilität kann separat weiterentwickelt werden,
- NovaOS-Sicherheitsmechanismen bleiben maßgeblich.

### Negativ

- Syscall Translation erzeugt zusätzlichen Implementierungsaufwand,
- komplexe Linux-spezifische Syscalls können umfangreiche Emulation benötigen,
- nicht jede Linux-Kernel-Schnittstelle lässt sich vollständig abbilden.

## Abhängigkeiten

- `ADR-COMPAT-0001_Native_Nova_Architektur_nicht_durch_POSIX_Linux_oder_Win32_bestimmen`
- `ADR-COMPAT-0002_Kleine_versionierte_NovaABI`
- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0004_POSIX_als_Kompatibilitätsschicht`
- `ADR-CAPABILITY-0001`
- `ADR-OBJECT-0001`

## Ergebnis

Linux-Kompatibilität folgt dem Modell:

```text
Linux ABI
    ↓
Syscall Translation
    ↓
Native NovaOS Mechanisms
```

NovaOS kann damit Linux-Anwendungen unterstützen, ohne die Linux-Syscall-Architektur zur eigenen Kernelarchitektur zu machen.