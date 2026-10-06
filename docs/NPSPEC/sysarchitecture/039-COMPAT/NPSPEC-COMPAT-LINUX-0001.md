# NPSPEC-COMPAT-LINUX-0001 – Nova Linux Compatibility

## Status

Angenommen

## Kategorie

Compatibility / Linux

## Zweck

NovaOS definiert eine Linux-Kompatibilitätsschicht für die Ausführung von Linux-Binärprogrammen.

Linux-Kompatibilität baut auf der POSIX- und Compatibility-Architektur auf und bildet Linux-spezifische ABIs, Systemaufrufe, Dateisystemkonventionen und Laufzeiterwartungen auf native NovaOS-Mechanismen ab.

## Grundprinzipien

```text
Linux Compatibility ≠ Linux Kernel
Linux ABI ≠ Nova ABI
Linux Syscall ≠ Nova Syscall
Linux Namespace ≠ Nova Namespace
Linux Permission ≠ Nova Authority
Linux root ≠ Universal Nova Authority
Linux Device Model ≠ Native Nova Device Model
```

## Modell

```text
Linux Binary
     ↓
Linux Personality
     ↓
Linux ABI / Runtime Environment
     ↓
Linux Syscall Translation
     ↓
Nova System Interfaces
     ↓
Capabilities / Policy
     ↓
NovaOS
```

## Binärprogramme

Linux-Programme können insbesondere über ELF erkannt und geladen werden.

```text
ELF Binary
   ↓
Architecture / ABI Detection
   ↓
Linux Compatibility Provider
   ↓
Loader
   ↓
Process
```

ELF selbst bestimmt dabei nicht vollständig die benötigte Linux-Kompatibilität.

## Systemaufrufe

Linux-Systemaufrufe werden nicht direkt zum Bestandteil der nativen NovaOS-Kernel-ABI.

```text
Linux Syscall
     ↓
Syscall Compatibility Layer
     ↓
Validation
     ↓
Nova System Interface
```

Nicht direkt abbildbare Semantik darf kontrolliert emuliert werden.

## Dateisystem

Linux-typische Pfade können innerhalb der Personality projiziert werden:

```text
/
├── bin
├── etc
├── home
├── lib
├── proc
├── sys
├── tmp
└── dev
```

Diese Struktur ist eine Compatibility-Ansicht.

Sie verändert nicht den nativen NovaOS-Namespace.

```text
Linux Path
    ↓
Linux Namespace Projection
    ↓
Nova Namespace Resolution
    ↓
ObjectID / Virtual Resource
```

## Virtuelle Systembereiche

Linux-spezifische virtuelle Bereiche wie:

```text
/proc
/sys
/dev
```

dürfen durch virtuelle Namespace-Provider erzeugt werden.

Sie stellen kompatible Projektionen auf NovaOS-Systeminformationen und Geräte dar und sind keine nativen Linux-Kerneldateisysteme.

## Libraries und Runtime

Linux-Programme dürfen kompatible Laufzeitkomponenten verwenden:

```text
Dynamic Loader
libc
Runtime Libraries
Application Libraries
```

Diese bleiben innerhalb der Compatibility-Umgebung beziehungsweise des Programms isoliert und ersetzen keine nativen NovaOS-Systembibliotheken.

## Prozesse und Threads

Linux-spezifische Prozesssemantik wird auf NovaOS-Prozesse und Tasks abgebildet.

Dazu können gehören:

```text
fork
clone
exec
wait
signals
futex
process groups
sessions
```

Nicht direkt vorhandene Semantik darf durch den Linux Compatibility Provider emuliert werden.

## IPC

Linux-Mechanismen wie:

```text
pipes
Unix sockets
shared memory
futex
eventfd
epoll
```

dürfen auf native NovaOS-IPC-, Event- und Synchronisationsmechanismen abgebildet werden.

## Netzwerk

Linux-Socketoperationen werden über native Netzwerk-Capabilities ausgeführt:

```text
Linux Socket API
       ↓
Linux Compatibility
       ↓
Network Capability
       ↓
Nova Network Stack
```

Die Linux-Kompatibilität erzeugt keine implizite Netzwerk-Authority.

## Geräte

Linux-Geräteknoten werden als Compatibility-Projektionen behandelt:

```text
/dev/*
   ↓
Virtual Device Entry
   ↓
Nova DeviceID
   ↓
Authorized Device Handle
```

Ein sichtbarer `/dev`-Eintrag erzeugt keine Geräteberechtigung.

## Benutzer und root

Linux-Konzepte wie:

```text
UID
GID
Capabilities
Namespaces
root
```

werden innerhalb der Linux-Personality emuliert beziehungsweise übersetzt.

Sie ersetzen nicht das native NovaOS-Capability-System.

```text
Linux root
    ≠
NovaOS unrestricted authority
```

## Container

Linux-Containerkonzepte dürfen innerhalb der Compatibility-Schicht unterstützt werden.

Linux Namespaces, cgroups und ähnliche Mechanismen werden dabei auf geeignete NovaOS-Isolations-, Namespace- und Ressourcenmechanismen abgebildet.

## Kompatibilitätszustand

Linux-Funktionen können gekennzeichnet werden als:

```text
NativeMapping
Translated
Emulated
PartiallySupported
Unsupported
Unavailable
```

## Normative Anforderungen

1. Linux MUSS als Compatibility Personality und nicht als nativer NovaOS-Kernelmodus behandelt werden.
2. Linux-Binärprogramme MÜSSEN anhand ihrer ABI- und Laufzeitanforderungen identifizierbar sein.
3. Linux-Systemaufrufe MÜSSEN über eine kontrollierte Übersetzungsschicht verarbeitet werden.
4. Linux-Systemaufrufe DÜRFEN die native NovaOS-Kernel-ABI nicht erweitern.
5. Linux-Pfade MÜSSEN über eine isolierte Namespace-Projektion bereitgestellt werden können.
6. `/proc`, `/sys` und `/dev` MÜSSEN als virtuelle Compatibility-Ressourcen realisierbar sein.
7. Linux-Libraries DÜRFEN native NovaOS-Systembibliotheken nicht unkontrolliert ersetzen.
8. Linux-Prozess-, Thread-, Signal- und IPC-Semantik MUSS übersetzbar oder kontrolliert emulierbar sein.
9. Linux-Netzwerkzugriffe MÜSSEN NovaOS-Capability- und Policy-Prüfungen unterliegen.
10. Linux-Geräteknoten DÜRFEN keine Geräte-Authority erzeugen.
11. Linux `root` DARF keine universelle NovaOS-Authority erzeugen.
12. Linux-Containermechanismen DÜRFEN native Sicherheits- und Ressourcenlimits nicht umgehen.
13. Nicht unterstützte Linux-Funktionen MÜSSEN kontrolliert fehlschlagen.
14. Unterstützungsgrad, Übersetzungen und Emulationen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-POSIX-0001`
- `NPSPEC-PROGRAM-LEGACY-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-NAMESPACE-VIRTUAL-0001`
- `NPSPEC-NAMESPACE-RESOLUTION-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS kann Linux-Software über eine isolierte Linux Compatibility Personality ausführen. Linux-ABIs, Systemaufrufe, Dateisystemkonventionen, Prozesse, IPC, Geräte und Laufzeitumgebungen werden auf native NovaOS-Mechanismen abgebildet, ohne Linux-Kernelstrukturen oder das Linux-Privilegmodell zum Bestandteil der nativen NovaOS-Architektur zu machen.