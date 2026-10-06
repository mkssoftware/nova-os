# NPSPEC-COMPAT-POSIX-0001 – Nova POSIX Compatibility

## Status

Angenommen

## Kategorie

Compatibility / POSIX

## Zweck

NovaOS definiert eine POSIX-Kompatibilitätsschicht für Software, die POSIX-Schnittstellen und POSIX-typisches Systemverhalten erwartet.

POSIX-Kompatibilität wird über die Compatibility-Architektur bereitgestellt und verändert weder die native NovaOS-Architektur noch deren Capability-, Namespace- oder Sicherheitsmodell.

## Grundprinzipien

```text
POSIX Compatibility ≠ Native NovaOS API
POSIX Process Model ≠ Nova Process Model
POSIX Path ≠ Native Object Identity
POSIX Permission ≠ Nova Authority
POSIX UID/GID ≠ Capability
POSIX root ≠ Universal Nova Authority
POSIX Compatibility ≠ Full Unix Emulation
```

## Modell

```text
POSIX Program
     ↓
POSIX Personality
     ↓
POSIX API / ABI
     ↓
Compatibility Translation
     ↓
Nova System Interfaces
     ↓
NovaOS
```

## Unterstützte Bereiche

Die POSIX-Schicht kann insbesondere bereitstellen:

```text
File Operations
Directories
File Descriptors
Processes
Threads
Signals
Pipes
Environment
Time
Sockets
Synchronization
Standard I/O
Process Exit / Wait
```

Der konkrete unterstützte POSIX-Umfang wird versioniert und introspektierbar beschrieben.

## Dateisystem

POSIX-Pfade werden innerhalb der POSIX-Personality auf den NovaOS-Namespace projiziert:

```text
POSIX Path
    ↓
POSIX Namespace View
    ↓
Nova Namespace Resolution
    ↓
ObjectID
```

Pfad und Objektidentität bleiben getrennt.

POSIX-Symlinks und andere POSIX-spezifische Namespace-Konzepte werden durch die Compatibility-Schicht kontrolliert abgebildet.

## File Descriptors

POSIX File Descriptors werden auf kontrollierte NovaOS-Handles abgebildet:

```text
POSIX fd
   ↓
Compatibility Handle Table
   ↓
Authorized Nova Handle
```

Ein File Descriptor darf keine größere Authority besitzen als das zugrunde liegende NovaOS-Handle.

## Prozesse

POSIX-Prozessoperationen werden auf das NovaOS-Prozessmodell übersetzt:

```text
fork
exec
wait
exit
signals
```

Falls eine POSIX-Semantik nicht direkt durch NovaOS unterstützt wird, darf die Personality diese emulieren.

Die Emulation darf native Sicherheits- oder Ressourcenregeln nicht umgehen.

## Benutzer und Rechte

POSIX-Konzepte wie:

```text
UID
GID
Mode Bits
Owner
root
```

werden als Compatibility-Semantik behandelt.

Sie ersetzen nicht das native Capability-Modell.

```text
POSIX Permission
      ↓
Compatibility Mapping
      ↓
Nova Capability / Policy
      ↓
Effective Authority
```

Insbesondere darf `UID 0` keine universelle NovaOS-Authority erzeugen.

## Signale

POSIX-Signale werden auf geeignete NovaOS-Prozess-, Event- und Cancellation-Mechanismen abgebildet.

Die Compatibility-Schicht muss Unterschiede zwischen POSIX-Signalverhalten und nativen NovaOS-Mechanismen kapseln.

## IPC und Pipes

POSIX:

```text
pipes
local sockets
shared memory
semaphores
```

dürfen auf native NovaOS-IPC-Mechanismen abgebildet werden.

Native Capability-Grenzen bleiben dabei erhalten.

## Netzwerk

POSIX-Socket-Aufrufe werden über die NovaOS-Netzwerk- und Capability-Schicht ausgeführt:

```text
POSIX Socket API
       ↓
Compatibility Provider
       ↓
Network Capability
       ↓
Nova Network Stack
```

POSIX-Socket-Zugriff erzeugt keine implizite Netzwerkberechtigung.

## Fehler

NovaOS-Fehler werden kontrolliert auf POSIX-kompatible Fehlerzustände wie `errno` abgebildet.

Informationsverlust durch diese Abbildung darf intern diagnostizierbar bleiben.

## Kompatibilitätszustand

Funktionen können gekennzeichnet werden als:

```text
NativeMapping
Emulated
PartiallySupported
Unsupported
Unavailable
```

## Normative Anforderungen

1. POSIX MUSS als Compatibility-Schicht und nicht als natives NovaOS-Systemmodell behandelt werden.
2. POSIX-Pfade MÜSSEN über eine kontrollierte Namespace-Projektion aufgelöst werden.
3. POSIX File Descriptors MÜSSEN auf autorisierte NovaOS-Handles abgebildet werden.
4. POSIX-Prozesssemantik MUSS auf native Mechanismen abgebildet oder kontrolliert emuliert werden können.
5. POSIX UID/GID und Mode Bits DÜRFEN das Capability-Modell nicht ersetzen.
6. POSIX `root` DARF keine universelle NovaOS-Authority erzeugen.
7. POSIX-Netzwerkzugriffe MÜSSEN weiterhin NovaOS-Capability- und Policy-Prüfungen unterliegen.
8. POSIX-IPC DARF native Sicherheitsgrenzen nicht umgehen.
9. POSIX-Fehler MÜSSEN kontrolliert aus nativen Fehlern ableitbar sein.
10. Nicht unterstützte POSIX-Funktionen MÜSSEN kontrolliert fehlschlagen.
11. Unterschiedliche POSIX-Kompatibilitätsversionen MÜSSEN unterstützt werden können.
12. Unterstützungsgrad, Emulationen und Abweichungen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-PROGRAM-LEGACY-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-NAMESPACE-RESOLUTION-0001`
- `NPSPEC-SYSTEM-KERNEL-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS kann POSIX-orientierte Software über eine isolierte Compatibility Personality ausführen. POSIX-Pfade, File Descriptors, Prozesse, Signale, IPC, Sockets und klassische Unix-Berechtigungskonzepte werden auf native NovaOS-Mechanismen abgebildet, während ObjectIDs, Handles, Capabilities und NovaOS-Sicherheitsregeln maßgeblich bleiben.