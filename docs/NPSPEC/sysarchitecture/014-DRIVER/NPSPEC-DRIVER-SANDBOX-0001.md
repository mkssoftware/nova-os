# NPSPEC-DRIVER-SANDBOX-0001 – Nova Driver Sandbox

## Status

Angenommen

## Kategorie

Driver / Sandbox / Security / Isolation

## Zweck

NovaOS definiert eine Driver Sandbox zur kontrollierten Ausführung von Treibern mit minimal notwendigen Berechtigungen.

```text
Driver
   ↓
Sandbox
   ├── Capabilities
   ├── Resource Limits
   ├── Memory Isolation
   └── Device Access
        ↓
      Hardware
```

Die Sandbox ergänzt die allgemeine Treiberisolation um explizite Zugriffs-, Ressourcen- und Kommunikationsgrenzen.

## Grundprinzip

```text
Sandbox ≠ Trust
Driver Loaded ≠ Fully Authorized
Device Ownership ≠ System Authority
```

Ein Treiber erhält ausschließlich die Ressourcen und Schnittstellen, die für seine definierte Aufgabe benötigt werden.

## Sandbox-Profil

Jede Sandbox besitzt ein definiertes Profil.

```text
DriverSandbox
├── DriverID
├── Security Context
├── Capabilities
├── Device Resources
├── Memory Limits
├── I/O Limits
└── IPC Rules
```

Das Profil muss vor Aktivierung des Treibers festgelegt oder aus einer vertrauenswürdigen Policy erzeugt werden.

## Capability-Modell

Zugriffe werden über explizite Capabilities autorisiert.

Beispiele:

```text
DeviceAccess
MMIOAccess
DMAAccess
InterruptAccess
SharedBufferAccess
IPCServiceAccess
```

Die Existenz eines Gerätes oder einer Ressource darf keine automatische Zugriffsberechtigung erzeugen.

## Hardwarezugriff

Hardware-Ressourcen müssen auf die konkrete Treiberinstanz begrenzt werden.

```text
Sandbox
   ├── MMIO Range A
   ├── Interrupt 7
   └── DMA Context B
```

Nicht freigegebene MMIO-, Port-, Interrupt- oder DMA-Ressourcen dürfen nicht verwendet werden.

## Speicher

Sandbox-Treiber sollen einen isolierten Speicherbereich verwenden.

```text
Driver Memory
      ≠
Kernel Memory
      ≠
Other Driver Memory
```

Gemeinsamer Speicher darf nur über explizit freigegebene Shared Buffers oder Mappings entstehen.

## DMA

DMA muss auch innerhalb einer Sandbox kontrolliert bleiben.

```text
Driver
   ↓
DMA Capability
   ↓
IOMMU Mapping
   ↓
Authorized Buffer
```

Ein Gerät darf keinen DMA-Zugriff auf Speicher außerhalb der freigegebenen Bereiche erhalten.

## Kommunikation

Kommunikation außerhalb der Sandbox muss über definierte Schnittstellen erfolgen.

```text
Driver
   ↓
IPC / IORequest / Shared Buffer
   ↓
Authorized Endpoint
```

Direkter Zugriff auf interne Datenstrukturen anderer Treiber oder Subsysteme ist nicht zulässig.

## Ressourcenlimits

Eine Sandbox muss Ressourcen begrenzen können.

```text
Memory
CPU Time
DMA Memory
Queue Depth
IPC Capacity
Interrupt Rate
```

Überschreitungen müssen kontrolliert behandelt werden können.

## I/O

I/O-Requests innerhalb der Sandbox bleiben an ihren ursprünglichen Security Context gebunden.

```text
IORequest
   ↓
Sandbox Policy
   ↓
Driver
   ↓
Device
```

Die Sandbox darf keine Möglichkeit bieten, I/O-Security- oder Capability-Prüfungen zu umgehen.

## Fehlerbehandlung

Sandbox-Verletzungen müssen erkannt werden können.

Beispiele:

```text
Unauthorized Memory Access
Invalid MMIO Access
Unauthorized DMA
Capability Violation
Resource Limit Exceeded
Invalid IPC
```

Abhängig von Schwere und Policy darf NovaOS:

```text
Deny
Throttle
Terminate
Restart
Quarantine
Disable
```

verwenden.

## Recovery

Nach einem Sandbox- oder Treiberfehler soll NovaOS die betroffene Domäne kontrolliert zurücksetzen können.

```text
Violation
   ↓
Contain
   ↓
Terminate / Reset
   ↓
Verify
   ↓
Restart / Disable
```

Ein Neustart darf erst erfolgen, wenn Ressourcen und Geräte in einen definierten Zustand zurückgeführt wurden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
DriverID
SandboxID
Capabilities
Granted Resources
Resource Usage
DMA Mappings
IPC Endpoints
Violations
Sandbox State
```

Nicht autorisierte Sicherheitsinformationen dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Treiber in kontrollierten Sandbox-Domänen ausführen können.
2. Sandbox-Berechtigungen MÜSSEN nach Least Privilege vergeben werden.
3. Ein geladener Treiber DARF NICHT automatisch vollständige Hardware- oder Kernelrechte erhalten.
4. Hardwarezugriffe MÜSSEN auf explizit freigegebene Ressourcen begrenzt sein.
5. Gemeinsamer Speicher MUSS explizit autorisiert werden.
6. DMA MUSS auf freigegebene Speicherbereiche beschränkt werden.
7. Kommunikation außerhalb der Sandbox MUSS über definierte Schnittstellen erfolgen.
8. Sandbox-Ressourcen MÜSSEN begrenzbar sein.
9. I/O-Requests DÜRFEN ihre ursprünglichen Security-Regeln NICHT umgehen.
10. Sandbox-Verletzungen MÜSSEN erkennbar und kontrolliert behandelbar sein.
11. Fehlerhafte Sandboxes SOLLEN unabhängig vom Gesamtsystem zurückgesetzt oder deaktiviert werden können.
12. Capabilities, Ressourcenverbrauch und Verletzungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-DRIVER-USERMODE-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-HAL-DMA-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `ADR-DRIVER-0006`

## Ergebnis

```text
Driver
   ↓
Restricted Sandbox
   ├── Capabilities
   ├── Memory
   ├── DMA
   ├── I/O
   └── Resource Limits
          ↓
     Controlled Hardware
```

NovaOS erhält damit eine kontrollierte Treiber-Sandbox, die Hardwarezugriffe, Speicher, Kommunikation und Ressourcen auf die tatsächlich benötigten Berechtigungen begrenzt und Fehler auf eine möglichst kleine Domäne beschränkt.