# NPSPEC-NETWORK-ZEROCOPY-0001 – Nova Network Zero-Copy

## Status

Angenommen

## Kategorie

Network / Data Movement / Zero-Copy

## Zweck

NovaOS definiert Zero-Copy-Netzwerkpfade zur Vermeidung unnötiger Datenkopien zwischen Anwendungen, Netzwerk-Stack und Netzwerkhardware.

```text
Application Buffer
       ↓
Network Stack
       ↓
Driver / DMA
       ↓
NIC
```

Zero-Copy ist eine Optimierung des Datenpfades und darf Sicherheits-, Ownership- oder Lebensdauerregeln nicht umgehen.

## Grundprinzip

```text
Zero-Copy ≠ Zero Data Movement
Buffer Reference ≠ Buffer Permission
Shared Buffer ≠ Shared Authority
Zero-Copy ≠ Mandatory
```

Kann ein sicherer Zero-Copy-Pfad nicht hergestellt werden, muss ein Copy-Fallback möglich bleiben.

## Netzwerkbuffer

Zero-Copy verwendet kontrollierte Buffer aus der gemeinsamen NovaOS-Datamove-Infrastruktur.

```text
NetworkBuffer
├── BufferID
├── Owner
├── Size
├── Access Rights
└── Lifetime
```

Optional:

```text
Memory Mapping
DMA Mapping
Scatter/Gather List
Semantic Type
Security Context
```

Buffer-Identität und Zugriffsberechtigung bleiben getrennt.

## Send-Pfad

Beim Senden dürfen Anwendungsdaten ohne zusätzliche Kopie an Netzwerkkomponenten weitergereicht werden.

```text
Application
    ↓
Shared / Pinned Buffer
    ↓
TCP / UDP / QUIC
    ↓
IP
    ↓
Driver
    ↓
DMA
```

Der Buffer muss bis zum Abschluss der relevanten Netzwerkoperation gültig bleiben.

Die Anwendung darf Speicherbereiche nicht verändern, wenn dadurch eine laufende Übertragung inkonsistent würde.

## Receive-Pfad

Empfangene Netzwerkdaten dürfen direkt in kontrollierte Buffer geschrieben werden.

```text
NIC
 ↓
DMA Buffer
 ↓
Network Stack
 ↓
Application
```

Eine Anwendung darf nur Zugriff auf die ihr ausdrücklich zugeordneten Datenbereiche erhalten.

Interne Kernel- oder Fremddaten dürfen nicht durch Buffer-Sharing sichtbar werden.

## Ownership

Jeder Buffer benötigt einen eindeutig bestimmbaren Ownership-Zustand.

```text
Application
    ↓ transfer / share
Network Stack
    ↓
Driver
    ↓ completion
Application
```

Ownership darf übertragen oder kontrolliert geteilt werden.

Ein Buffer darf erst wiederverwendet oder freigegeben werden, wenn keine aktive Netzwerkoperation ihn mehr benötigt.

## DMA

Zero-Copy soll mit DMA und IOMMU zusammenarbeiten.

```text
Buffer
  ↓
DMA Mapping
  ↓
IOMMU
  ↓
NIC
```

DMA-Berechtigungen müssen auf die tatsächlich benötigten Speicherbereiche begrenzt werden.

Ein Netzwerkgerät darf durch Zero-Copy keinen allgemeinen Zugriff auf den Systemspeicher erhalten.

## Scatter/Gather

Netzwerkoperationen dürfen Scatter/Gather verwenden.

```text
Buffer A ─┐
Buffer B ─┼→ Network Packet / Stream
Buffer C ─┘
```

Dadurch können Header und Payload aus mehreren Speicherbereichen verarbeitet werden, ohne sie vorher zusammenzukopieren.

## Protokollintegration

Zero-Copy muss protokollunabhängig integrierbar sein.

```text
TCP
UDP
QUIC
 ↓
Network Buffer Model
```

Ein Protokoll darf Zero-Copy nur verwenden, wenn seine eigene Semantik für Retransmission, Verschlüsselung, Fragmentierung und Lifetime erhalten bleibt.

Insbesondere Retransmission kann eine längere Buffer-Lifetime erfordern.

## Asynchrones I/O

Zero-Copy muss mit dem asynchronen NovaOS-I/O-Modell zusammenarbeiten.

```text
IORequest
   ↓
Zero-Copy Buffer
   ↓
Network Operation
   ↓
Completion
   ↓
Buffer Release
```

Completion muss eindeutig bestimmen können, wann ein Buffer wiederverwendet werden darf.

## Sicherheit

Zero-Copy darf keine Speicherisolation umgehen.

Vor Mapping oder Übergabe müssen mindestens geprüft werden:

```text
Buffer Ownership
Access Rights
Memory Range
Security Context
DMA Permission
Lifetime
```

Netzwerkdaten bleiben auch bei direkter Buffer-Übergabe nicht vertrauenswürdig.

## Copy-Fallback

Ein Copy-Pfad muss verfügbar bleiben.

```text
Zero-Copy Possible?
    ├── Yes → Direct Buffer Path
    └── No  → Safe Copy Path
```

Copy-Fallback darf insbesondere verwendet werden bei:

```text
Security Boundary
Incompatible Memory
Encryption Requirements
Alignment Constraints
Unsupported Hardware
Small Payloads
```

NovaOS darf anhand der Kosten entscheiden, dass Kopieren effizienter als Zero-Copy ist.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Pinned Memory
DMA Mappings
Shared Buffers
Outstanding Buffers
Scatter/Gather Entries
Pending Network Operations
```

Zero-Copy darf nicht zu unbegrenzt gepinntem oder dauerhaft gebundenem Speicher führen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
BufferID
Owner
Size
Mapping State
DMA State
Network Flow
Outstanding Operations
Lifetime
Zero-Copy / Copy Path
```

Speicherinhalte dürfen ohne entsprechende Berechtigung nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS SOLL Zero-Copy-Netzwerkpfade unterstützen.
2. Zero-Copy DARF NICHT verpflichtend für Netzwerkoperationen sein.
3. Netzwerkbuffer MÜSSEN eindeutige Ownership- und Lifetime-Regeln besitzen.
4. Buffer-Referenzen DÜRFEN NICHT als Zugriffsberechtigung interpretiert werden.
5. Zero-Copy DARF Prozess- oder Speicherisolation NICHT umgehen.
6. DMA-Zugriffe MÜSSEN auf autorisierte Speicherbereiche begrenzt werden.
7. Scatter/Gather SOLL für Netzwerkoperationen unterstützt werden können.
8. TCP, UDP und QUIC MÜSSEN Zero-Copy kontrolliert integrieren können.
9. Buffer-Lifetime MUSS bis zum tatsächlichen Abschluss der Operation erhalten bleiben.
10. Ein sicherer Copy-Fallback MUSS verfügbar sein.
11. Gepinnter Speicher und DMA-Mappings MÜSSEN ressourcenbegrenzt sein.
12. Zero-Copy-Zustand und Buffer-Nutzung SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-NETWORK-STACK-0001`
- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-TCP-0001`
- `NPSPEC-NETWORK-UDP-0001`
- `NPSPEC-NETWORK-QUIC-0001`
- `NPSPEC-IO-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-SCATTERGATHER-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-SHAREDBUFFER-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-NETWORK-0021`

## Ergebnis

```text
Application Buffer
        ↓
Controlled Zero-Copy
        ↓
Network Protocols
        ↓
Driver + DMA / IOMMU
        ↓
Network Hardware
```

NovaOS erhält damit einen kontrollierten Zero-Copy-Netzwerkpfad, der unnötige Speicheroperationen reduziert, ohne Ownership, Speicherisolation, Sicherheit oder Ressourcenbegrenzung zu opfern.