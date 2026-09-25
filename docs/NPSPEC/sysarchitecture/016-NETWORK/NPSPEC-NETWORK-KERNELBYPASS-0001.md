# NPSPEC-NETWORK-KERNELBYPASS-0001 – Nova Network Kernel Bypass

## Status

Angenommen

## Kategorie

Network / Performance / Kernel Bypass

## Zweck

NovaOS definiert kontrollierte Kernel-Bypass-Netzwerkpfade für Anwendungen und Systemdienste mit sehr hohen Anforderungen an Latenz, Durchsatz oder Paketverarbeitung.

```text
Application
    ↓
Controlled Network Channel
    ↓
Shared Rings / Buffers
    ↓
Driver / NIC
```

Kernel Bypass reduziert Eingriffe des Kernels im Datenpfad, entfernt jedoch nicht dessen Kontrolle über Sicherheit, Ressourcen und Lifecycle.

## Grundprinzip

```text
Kernel Bypass ≠ Security Bypass
Direct Hardware Access ≠ Hardware Ownership
Fast Path ≠ Uncontrolled Path
Mapped Queue ≠ Network Authority
```

Der Kernel bleibt Kontrollinstanz für Einrichtung, Berechtigungen, Ressourcenvergabe und Entzug des Bypass-Pfades.

## Modell

Ein Kernel-Bypass-Kanal besitzt mindestens:

```text
NetworkBypassChannel
├── ChannelID
├── Owner
├── InterfaceID
├── Queues
├── Buffers
├── Security Context
└── State
```

Optional:

```text
FlowID
NamespaceID
QoS
ExecutionContract
Resource Budget
DMA Context
```

Die `ChannelID` ist unabhängig von Hardwareadresse, Queue-Nummer oder Prozessadresse.

## Control Plane und Data Plane

NovaOS trennt Kontroll- und Datenpfad.

```text
Control Plane
Kernel
├── Authorization
├── Configuration
├── Resource Allocation
└── Revocation

Data Plane
Application ↔ Shared Queue ↔ NIC
```

Der schnelle Datenpfad darf ohne Kernelwechsel pro Paket arbeiten.

Sicherheitsrelevante Änderungen bleiben kontrollierte Kerneloperationen.

## Queue-Zuordnung

Hardwarequeues dürfen kontrolliert Prozessen oder Diensten zugeordnet werden.

```text
NIC
├── Queue 0 → Kernel
├── Queue 1 → Service A
└── Queue 2 → Service B
```

Eine Queue-Zuordnung darf keinen Zugriff auf fremde Queues oder Netzwerkbuffer ermöglichen.

## Shared Rings

Kernel Bypass soll gemeinsame Ringbuffer verwenden können.

```text
Application
    ↕
Descriptor Ring
    ↕
NIC
```

Ringbuffer müssen:

- begrenzte Größe besitzen,
- definierte Ownership-Regeln verwenden,
- Producer und Consumer eindeutig bestimmen,
- Backpressure ermöglichen.

## Speicher und DMA

Kernel Bypass muss die gemeinsame DMA- und IOMMU-Infrastruktur verwenden.

```text
Application Buffer
      ↓
Controlled Mapping
      ↓
IOMMU
      ↓
NIC DMA
```

DMA-Zugriffe müssen auf explizit autorisierte Speicherbereiche begrenzt werden.

Direkter Zugriff eines Geräts auf den gesamten Prozess- oder Systemspeicher ist nicht zulässig.

## Zero-Copy

Kernel Bypass darf Zero-Copy verwenden.

```text
Application Buffer
        ↓
DMA
        ↓
NIC
```

Dabei gelten weiterhin:

```text
Ownership
Lifetime
Memory Protection
DMA Isolation
Resource Limits
```

Kann ein sicherer direkter Pfad nicht bereitgestellt werden, muss ein kontrollierter alternativer Netzwerkpfad möglich sein.

## Network Stack Integration

Kernel Bypass darf den normalen Netzwerk-Stack teilweise umgehen, muss aber in das NovaOS-Netzwerkmodell eingebunden bleiben.

```text
Network Flow
     ↓
Policy Decision
   ↙          ↘
Normal Stack  Bypass Path
```

Flow-, Namespace-, Firewall-, QoS- und Security-Kontext müssen vor Einrichtung des Bypass-Pfades berücksichtigt werden.

## Firewall und Security

Kernel Bypass darf Firewall- oder Capability-Regeln nicht umgehen.

Policy kann beispielsweise:

```text
Before Channel Creation
During Flow Binding
At Hardware Filter
At Virtual Interface
```

durchgesetzt werden.

Wenn notwendige Sicherheitsregeln nicht zuverlässig im Bypass-Pfad durchsetzbar sind, darf dieser Pfad nicht verwendet werden.

## QoS und Congestion Control

Kernel-Bypass-Verkehr bleibt Bestandteil der gemeinsamen Ressourcenökonomie.

```text
Bypass Flow
   ↓
QoS / Resource Policy
   ↓
NIC Queue
```

Bypass-Anwendungen dürfen Bandbreitenlimits oder Congestion-Control-Anforderungen nicht allein aufgrund ihres direkten Datenpfades umgehen.

## Lifecycle

Ein Bypass-Kanal besitzt einen kontrollierten Lebenszyklus.

```text
Created
   ↓
Authorized
   ↓
Mapped
   ↓
Active
   ↓
Revoking
   ↓
Closed
```

Der Kernel muss einen Bypass-Kanal wieder entziehen können.

Beim Prozessabsturz müssen Queues, Buffer, DMA-Mappings und Hardwarezustände kontrolliert bereinigt werden.

## Fallback

Kernel Bypass ist eine Optimierung und keine Voraussetzung für Netzwerkfunktionalität.

```text
Bypass Available?
 ├── Yes → Fast Path
 └── No  → Normal Network Stack
```

NovaOS darf den normalen Netzwerkpfad verwenden bei:

```text
Unsupported Hardware
Security Conflict
Resource Pressure
Incompatible Protocol
Debugging
Recovery
```

## Ressourcensteuerung

NovaOS muss mindestens begrenzen können:

```text
Hardware Queues
Pinned Memory
DMA Mappings
Shared Rings
Outstanding Buffers
Bandwidth
CPU Time
```

Eine Anwendung darf durch Kernel Bypass keine exklusiven Ressourcen unbegrenzt blockieren.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ChannelID
Owner
InterfaceID
FlowID
Queue Assignment
Buffer Usage
DMA State
Bandwidth
Security State
Bypass State
```

Sensible Speicher- oder Hardwareinformationen dürfen nur autorisiert sichtbar sein.

## Normative Anforderungen

1. NovaOS SOLL kontrollierten Network Kernel Bypass unterstützen können.
2. Kernel Bypass DARF NICHT als Security Bypass wirken.
3. Einrichtung und Entzug eines Bypass-Kanals MÜSSEN durch den Kernel kontrolliert werden.
4. Hardwarequeues DÜRFEN nur autorisierten Sicherheitsdomänen zugeordnet werden.
5. DMA MUSS durch geeignete Speicher- und IOMMU-Isolation begrenzt werden.
6. Kernel Bypass SOLL Zero-Copy und Shared Rings verwenden können.
7. Bypass-Pfade DÜRFEN Firewall-, Namespace- oder Capability-Regeln NICHT umgehen.
8. QoS- und Ressourcenlimits MÜSSEN weiterhin durchsetzbar bleiben.
9. Congestion-Control-Anforderungen DÜRFEN NICHT durch Kernel Bypass umgangen werden.
10. Der Kernel MUSS einen aktiven Bypass-Kanal widerrufen können.
11. Ein normaler Netzwerkpfad MUSS als sicherer Fallback verfügbar bleiben.
12. Bypass-Kanäle und deren Ressourcenverbrauch SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-NAMESPACE-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `NPSPEC-NETWORK-CONGESTION-0001`
- `NPSPEC-NETWORK-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-KERNELBYPASS-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-RINGBUFFER-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0022`

## Ergebnis

```text
Application
     ↓
Authorized Fast Path
     ↓
Shared Rings + Zero-Copy
     ↓
DMA / IOMMU
     ↓
NIC
```

NovaOS erhält damit einen kontrollierten Hochleistungs-Netzwerkpfad, der Kernelinteraktionen im Data Plane minimiert, während Sicherheit, Isolation, Ressourcensteuerung und Widerruf weiterhin unter Kontrolle des Systems bleiben.