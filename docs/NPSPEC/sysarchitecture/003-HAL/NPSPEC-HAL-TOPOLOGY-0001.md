# NPSPEC-HAL-TOPOLOGY-0001 – Nova Hardware Topology

## Status

Entwurf

## Kategorie

HAL / Hardware / Topology / Discovery

## Zweck

NovaOS stellt die physische und logische Hardwaretopologie eines Systems als einheitliches, strukturiertes Modell bereit.

```text
Firmware / Hardware
        ↓
Topology Discovery
        ↓
Normalized Topology
        ↓
Kernel / Scheduler / Drivers
```

## Grundprinzip

Hardware wird nicht nur als Liste einzelner Komponenten betrachtet, sondern als Beziehungen zwischen Ressourcen.

```text
System
 ├── NUMA Node
 │    ├── CPU
 │    │    ├── Core
 │    │    └── Thread
 │    └── Memory
 ├── Interrupt Controller
 └── I/O Bus
      └── Device
```

## Topologieobjekte

Ein Topologieobjekt beschreibt mindestens:

```text
TopologyID
Type
Parent
Children
Properties
State
```

Typische Objekttypen sind:

```text
Package
Core
Thread
NUMA Node
Memory Region
Cache
Interrupt Controller
Bus
Device
```

## Erkennung

Topologieinformationen können aus mehreren Quellen stammen:

```text
Boot Information
Firmware Tables
ACPI
Device Tree
CPU Discovery
Bus Enumeration
```

Die Platform Abstraction normalisiert diese Informationen vor ihrer Verwendung durch höhere Systemschichten.

## CPU-Topologie

NovaOS muss Beziehungen zwischen:

```text
Package
Core
Hardware Thread
Cache
NUMA Node
```

abbilden können.

Diese Informationen können beispielsweise vom Scheduler für Affinität und Lokalitätsentscheidungen verwendet werden.

## Speicher-Topologie

Speicherbereiche können mit ihrer physischen Zuordnung beschrieben werden.

```text
Memory Region
     ↓
NUMA Node
     ↓
CPU Group
```

Damit können Speicher- und Ausführungsort gemeinsam optimiert werden.

## I/O-Topologie

Geräte und Busse sollen als Beziehungen modelliert werden.

```text
Root
 └── Bus
      └── Device
           └── Interrupt / DMA Resources
```

Die Topologie ersetzt dabei nicht die eigentlichen Gerätetreiber.

## Dynamische Änderungen

Hardwaretopologie kann sich während des Betriebs ändern.

Beispiele:

```text
CPU Hotplug
Memory Hotplug
Device Hotplug
Virtual Hardware Changes
```

Änderungen müssen kontrolliert in das Topologiemodell übernommen werden.

## Normative Anforderungen

1. NovaOS MUSS eine strukturierte Hardwaretopologie bereitstellen.
2. Topologieelemente MÜSSEN eindeutig identifizierbar sein.
3. Beziehungen zwischen CPU, Speicher, Interrupts, Bussen und Geräten MÜSSEN darstellbar sein.
4. Firmware- und Hardwareinformationen MÜSSEN vor Übernahme validiert werden.
5. CPU-Package-, Core- und Thread-Strukturen MÜSSEN abbildbar sein.
6. NUMA- und Cache-Beziehungen SOLLEN unterstützt werden.
7. I/O- und Bus-Topologien SOLLEN strukturiert darstellbar sein.
8. Höhere Systemschichten SOLLEN die normalisierte Topologie statt plattformspezifischer Tabellen verwenden.
9. Dynamische Topologieänderungen MÜSSEN kontrolliert aktualisierbar sein.
10. Topologieinformationen DÜRFEN keine Scheduling- oder Ressourcenpolitik selbst festlegen.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-ARCH-0002_Mechanism_und_Policy`
- `ADR-HAL-0004`

## Ergebnis

```text
Physical Hardware
       ↓
Topology Discovery
       ↓
Normalized Hardware Graph
       ↓
Scheduler / Memory / Drivers / Resources
```

NovaOS erhält damit eine einheitliche Beschreibung der räumlichen und logischen Beziehungen seiner Hardware, auf deren Grundlage höhere Systemschichten optimale Entscheidungen treffen können.