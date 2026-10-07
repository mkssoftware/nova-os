# NPSPEC-POWER-SUSPEND-0001 – Nova System Suspend

## Status

Angenommen

## Kategorie

Power / Suspend

## Zweck

NovaOS definiert Suspend als kontrollierten Systemzustand, in dem die normale Ausführung weitgehend angehalten wird, während ausreichend Systemzustand erhalten bleibt, um eine schnelle Wiederaufnahme zu ermöglichen.

Suspend bleibt von Hibernate, Shutdown, Display-Off und Runtime Power Management getrennt.

## Grundprinzipien

```text
Suspend ≠ Hibernate
Suspend ≠ Shutdown
Suspend ≠ Display Off
Suspend ≠ Runtime Device Suspend
Suspend ≠ Session Lock
Wake ≠ Successful Resume
Requested Suspend ≠ Reached Suspend
```

## Modell

```text
SuspendContext
├── SuspendID
├── SuspendMode
├── WakeSources[]
├── DeviceStates[]
├── PlatformState
├── Constraints
└── State
```

## Suspend-Modi

NovaOS verwendet abstrakte Suspend-Modi:

```text
Light Suspend
System Suspend
Deep Suspend
```

Die konkrete Umsetzung wird durch den Platform Provider bestimmt.

Beispiele können sein:

```text
ACPI Sleep States
Modern Standby
SoC Low-Power States
Architecture-Specific Suspend
```

Höhere Systemschichten dürfen nicht von einem bestimmten Firmwaremodell abhängig sein.

## Suspend-Ablauf

```text
Suspend Request
      ↓
Validate
      ↓
Prepare
      ↓
Freeze Tasks
      ↓
Quiesce I/O
      ↓
Suspend Devices
      ↓
Configure Wake Sources
      ↓
Platform Suspend
```

Der Übergang muss kontrolliert erfolgen.

## Vorbereitung

Vor Suspend müssen relevante Systemaktivitäten einen konsistenten Zustand erreichen.

Dazu gehören insbesondere:

```text
Filesystem Transactions
Pending Storage Writes
Device I/O
Network State
Timers
Running Tasks
Critical Services
```

Nicht suspendierbare kritische Operationen dürfen den Übergang blockieren oder verzögern.

## Geräte

Geräte werden entsprechend ihrer Abhängigkeiten suspendiert:

```text
Applications / Services
        ↓
Devices
        ↓
Controllers
        ↓
Buses
        ↓
Power Domains
        ↓
Platform
```

Beim Resume erfolgt die Wiederherstellung in einer geeigneten abhängigen Reihenfolge.

## Wake Sources

NovaOS muss Wake Sources explizit verwalten:

```text
Power Button
Keyboard
Pointer
Lid
RTC
Network
USB
Device Event
Platform Event
```

Wake-Fähigkeit und aktivierte Wake Source bleiben getrennt.

## Resume

```text
Wake Event
    ↓
Platform Resume
    ↓
Restore Power Domains
    ↓
Resume Devices
    ↓
Restore Services
    ↓
Resume Tasks
    ↓
Verify
    ↓
Active
```

Ein Wake Event allein bedeutet nicht, dass Resume erfolgreich abgeschlossen wurde.

## Fehlerbehandlung

Schlägt Suspend während der Vorbereitung fehl:

```text
Suspend Failure
      ↓
Abort Transition
      ↓
Restore Previous State
      ↓
Verify
```

Schlägt Resume teilweise fehl, muss NovaOS betroffene Komponenten isolieren, wiederherstellen oder kontrolliert degradieren können.

Kritische Fehler dürfen Recovery oder einen sicheren Neustart auslösen.

## Sicherheit

Suspend darf Sicherheitszustände nicht implizit abschwächen.

Insbesondere bleiben getrennt:

```text
Suspend State
Session State
Authentication State
Encryption State
Capability State
```

Policy darf beispielsweise verlangen, dass nach Resume eine erneute Authentifizierung erforderlich ist.

## Normative Anforderungen

1. NovaOS MUSS Suspend als kontrollierte Systemtransition behandeln.
2. Suspend, Hibernate und Shutdown MÜSSEN getrennte Zustände bleiben.
3. Suspend DARF nicht an ACPI oder eine bestimmte Plattformtechnologie gebunden sein.
4. Vor Suspend MÜSSEN kritische I/O- und Transaktionszustände berücksichtigt werden.
5. Geräte MÜSSEN abhängigkeitsgerecht suspendiert und wiederhergestellt werden.
6. Wake Sources MÜSSEN explizit konfigurierbar sein.
7. Wake-Fähigkeit und aktivierte Wake Source MÜSSEN getrennt bleiben.
8. Fehlgeschlagene Suspend-Transitionen MÜSSEN kontrolliert abbrechbar sein.
9. Resume MUSS den wiederhergestellten Systemzustand verifizieren.
10. Suspend DARF Sicherheits- oder Capability-Grenzen nicht abschwächen.
11. Plattformabhängige Suspend-Modi MÜSSEN hinter der Platform-Power-Abstraktion bleiben.
12. Suspend-Modus, Transition, Wake Source, Fehler und Resume-Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-POWER-ARCH-0001`
- `NPSPEC-POWER-PLATFORM-0001`
- `NPSPEC-POWER-ACPI-0001`
- `NPSPEC-POWER-DEVICE-0001`
- `NPSPEC-POWER-RUNTIME-0001`
- `NPSPEC-POWER-STORAGE-0001`
- `NPSPEC-POWER-NETWORK-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann das gesamte System kontrolliert in einen energiesparenden Suspend-Zustand überführen und anschließend zuverlässig wieder aufnehmen. Plattformzustände, Geräte, I/O, Wake Sources und Sicherheitskontext werden koordiniert behandelt, während Suspend von Hibernate, Shutdown und Runtime Power Management klar getrennt bleibt.