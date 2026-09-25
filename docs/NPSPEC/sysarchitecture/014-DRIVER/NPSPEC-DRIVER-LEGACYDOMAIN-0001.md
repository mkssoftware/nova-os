# NPSPEC-DRIVER-LEGACYDOMAIN-0001 – Nova Legacy Driver Domain

## Status

Angenommen

## Kategorie

Driver / Legacy Compatibility / Isolation

## Zweck

NovaOS definiert eine isolierte Legacy Driver Domain für ältere oder fremde Treiber, die nicht dem nativen NovaOS-Treibermodell entsprechen.

```text
Legacy Driver
     ↓
Legacy Domain
     ↓
Compatibility Layer
     ↓
Nova Driver Framework
     ↓
Hardware
```

Legacy-Treiber sollen verwendet werden können, ohne ihre ursprünglichen Privilegien oder Annahmen ungeprüft auf NovaOS zu übertragen.

## Grundprinzip

```text
Legacy Compatibility ≠ Native Trust
Legacy Privilege ≠ Nova Privilege
Compatibility ≠ Security Bypass
```

Legacy-Treiber werden grundsätzlich als weniger vertrauenswürdig als native, vollständig validierte NovaOS-Treiber behandelt.

## Legacy Domain

Legacy-Treiber müssen innerhalb einer kontrollierten Domain ausgeführt werden.

```text
Legacy Domain
├── Compatibility Runtime
├── Restricted Memory
├── Virtualized Resources
├── Controlled I/O
└── Resource Limits
```

Direkter Zugriff auf interne NovaOS-Kernelstrukturen ist nicht zulässig.

## Compatibility Layer

Die Compatibility Layer übersetzt fremde Treiberschnittstellen auf native NovaOS-Mechanismen.

```text
Legacy API
    ↓
Compatibility Layer
    ↓
Nova API
```

Übersetzt werden dürfen beispielsweise:

```text
I/O Requests
Memory Operations
Interrupt Registration
DMA Requests
Device Enumeration
Synchronization
```

Nicht unterstützte Semantik muss explizit fehlschlagen und darf nicht stillschweigend verändert werden.

## Hardwarezugriff

Legacy-Treiber dürfen Hardware nur über kontrolliert bereitgestellte Ressourcen verwenden.

```text
Legacy Driver
      ↓
Compatibility Layer
      ↓
Capability Check
      ↓
MMIO / Ports / Interrupt / DMA
```

Die vom ursprünglichen Treibermodell erwarteten Privilegien dürfen virtualisiert oder eingeschränkt werden.

## Speicher

Legacy-Treiber sollen vom nativen Kernel und anderen Treibern isoliert bleiben.

```text
Legacy Memory
      ≠
Nova Kernel Memory
      ≠
Native Driver Memory
```

Benötigte Speicherbereiche müssen explizit gemappt werden.

## DMA

Legacy-DMA muss über die NovaOS-DMA- und IOMMU-Infrastruktur vermittelt werden.

```text
Legacy DMA Request
       ↓
Translation
       ↓
IOMMU Mapping
       ↓
Device
```

Ein Legacy-Treiber darf keine uneingeschränkten physischen Speicherzugriffe erhalten.

## Interrupts

Legacy-Interrupt-Mechanismen müssen auf das NovaOS-Interruptmodell abgebildet werden.

```text
Hardware IRQ
    ↓
Nova Interrupt Layer
    ↓
Legacy Compatibility
    ↓
Legacy Driver
```

Unbegrenztes Warten oder blockierende Verarbeitung im Interrupt-Kontext darf nicht übernommen werden.

## I/O

Legacy-I/O soll auf das native Request-/Completion-Modell abgebildet werden.

```text
Nova IORequest
      ↓
Legacy Translation
      ↓
Legacy Driver
      ↓
Completion Translation
      ↓
Nova Completion
```

Damit bleiben Scheduling, Cancellation, QoS und Ressourcensteuerung unter Kontrolle von NovaOS.

## Fehlerisolation

Ein Fehler innerhalb der Legacy Domain soll nicht unmittelbar zum Kernel-Fehler führen.

```text
Legacy Driver Failure
        ↓
Contain
        ↓
Terminate Domain
        ↓
Reset Device
        ↓
Restart / Disable
```

Hängende Treiber müssen durch Timeouts und Watchdogs erkennbar sein.

## Ressourcenlimits

Die Legacy Domain muss Ressourcen begrenzen können.

```text
Memory
CPU Time
DMA Memory
Queue Depth
Interrupt Rate
I/O Bandwidth
```

Legacy-Kompatibilität darf keine unbegrenzte Ressourcennutzung ermöglichen.

## Native Migration

Legacy-Treiber gelten als Kompatibilitätslösung und nicht als bevorzugtes langfristiges Treibermodell.

```text
Legacy Driver
     ↓
Compatibility
     ↓
Native Driver Available
     ↓
Live Replacement
```

Ein kompatibler nativer NovaOS-Treiber soll den Legacy-Treiber kontrolliert ersetzen können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Legacy Driver
Legacy Domain
Compatibility Type
Bound Device
Granted Resources
Pending I/O
Resource Usage
Violations
Error State
```

## Normative Anforderungen

1. NovaOS SOLL Legacy-Treiber innerhalb isolierter Legacy Domains ausführen können.
2. Legacy-Treiber DÜRFEN NICHT automatisch native Kernelprivilegien erhalten.
3. Fremde Treiberschnittstellen MÜSSEN über eine kontrollierte Compatibility Layer vermittelt werden.
4. Nicht unterstützte Legacy-Semantik MUSS explizit erkennbar sein.
5. Hardwarezugriffe MÜSSEN auf explizit freigegebene Ressourcen begrenzt werden.
6. Legacy-Treiber DÜRFEN NICHT direkt auf interne NovaOS-Kernelstrukturen zugreifen.
7. DMA MUSS über die NovaOS-DMA-/IOMMU-Infrastruktur kontrolliert werden.
8. Legacy-I/O SOLL auf das native Request-/Completion-Modell abgebildet werden.
9. Hängende Legacy-Treiber MÜSSEN durch begrenzte Wartezeiten oder Watchdogs erkennbar sein.
10. Ressourcenverbrauch der Legacy Domain MUSS begrenzbar sein.
11. Fehler SOLLEN auf die jeweilige Legacy Domain begrenzt bleiben.
12. Native Treiber SOLLEN Legacy-Treiber kontrolliert ersetzen können.

## Abhängigkeiten

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-FRAMEWORK-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-DRIVER-SANDBOX-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-DRIVER-LIVEREPLACE-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `ADR-DRIVER-0010`

## Ergebnis

```text
Legacy Driver
      ↓
Isolated Legacy Domain
      ↓
Compatibility + Capability Control
      ↓
Native NovaOS Interfaces
      ↓
Hardware
```

NovaOS erhält damit eine begrenzte Kompatibilitätsumgebung für Legacy-Treiber, ohne fremde Treibermodelle, Privilegien oder Sicherheitsannahmen direkt in den nativen Kernel übernehmen zu müssen.