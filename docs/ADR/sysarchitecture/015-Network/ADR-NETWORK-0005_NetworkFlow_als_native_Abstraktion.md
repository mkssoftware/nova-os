# ADR-NETWORK-0005 – NetworkFlow als native Abstraktion

## Status

Angenommen

## Kategorie

Network / NetworkFlow / Transport / Data Movement / Execution Contracts

## Kontext

`ADR-NETWORK-0003` definiert semantische Flow Requirements und `ADR-NETWORK-0004` behandelt TCP, UDP und QUIC als Transportprovider.

Dafür benötigt NovaOS eine konkrete systemweite Abstraktion, welche einen logischen Netzwerkdatenfluss repräsentiert, ohne ihn dauerhaft an Socket, Transportprotokoll, IP-Version oder Netzwerkinterface zu koppeln.

Klassische Socket-APIs bleiben für Kompatibilität und Low-Level-Zugriffe wichtig, sollen jedoch nicht das fundamentale native Netzwerkobjekt von NovaOS darstellen.

---

## Entscheidung

NovaOS führt **`NetworkFlow` als native Abstraktion für Netzwerkkommunikation** ein.

```text
Application / Capability
        ↓
    NetworkFlow
        ↓
Flow Requirements
        ↓
Transport + Path Resolution
        ↓
Network Provider
```

Ein `NetworkFlow` beschreibt eine logische Kommunikationsbeziehung einschließlich ihrer semantischen Anforderungen und ihres Execution Context.

Transport, IP-Version, Interface und konkreter Netzwerkpfad sind Eigenschaften der gewählten Ausführung und nicht die Identität des Flows.

---

## Grundprinzip

```text
NetworkFlow describes communication.

Providers implement its transport.
```

---

## NetworkFlow Identity

Ein `NetworkFlow` besitzt eine logische Identität, die nicht ausschließlich von einem klassischen Netzwerk-Tupel wie:

```text
Source IP
Source Port
Destination IP
Destination Port
Protocol
```

abhängt.

Diese Werte können Eigenschaften einer konkreten Flow-Ausführung sein, definieren jedoch nicht zwingend die übergeordnete Flow Identity.

Damit kann ein Flow grundsätzlich seinen physischen Netzwerkpfad ändern, ohne seine logische Bedeutung zu verlieren.

---

## Flow Contract

Ein `NetworkFlow` verbindet Kommunikationsziel und semantische Flow Requirements.

Konzeptionell:

```text
NetworkFlow
 ├── Endpoint / Service
 ├── Semantic Types
 ├── Flow Requirements
 ├── Security Context
 ├── Resource Context
 └── Execution Context
```

Die konkrete Struktur wird in den zugehörigen NPSPECs festgelegt.

---

## Transport Resolution

Ein `NetworkFlow` ist nicht grundsätzlich an TCP, UDP oder QUIC gebunden.

```text
NetworkFlow
     ↓
Requirements
     ↓
Eligible Providers
 ┌────┼─────┐
TCP  UDP   QUIC
```

Die Transportauswahl folgt den Regeln aus `ADR-NETWORK-0003`.

Eine explizite Transportbindung bleibt möglich.

---

## Network Path

Transport Provider und Netzwerkpfad bleiben getrennte Entscheidungen.

Ein Flow kann abhängig von Hardware, Routing und Policies beispielsweise über unterschiedliche:

- Interfaces,
- IP-Versionen,
- Netzwerkprovider,
- physische Verbindungen

geführt werden.

```text
NetworkFlow
     ↓
Transport
     ↓
Network Path
     ↓
Interface
```

Location-, Security- und Data-Sovereignty-Constraints begrenzen dabei die zulässigen Pfade.

---

## Lifecycle

`NetworkFlow` besitzt einen kontrollierten Lifecycle und integriert sich in Structured Concurrency.

Ein Flow gehört zu einem definierten Execution Context und darf nicht unbeabsichtigt als verwaiste Netzwerkaktivität weiterlaufen.

Cancellation, Deadlines und Scope-Ende können auf den Flow propagiert werden.

Transportinterne Zustände bleiben Aufgabe des jeweiligen Providers.

---

## Data Movement

`NetworkFlow` integriert sich in die systemweite Data-Movement-Architektur.

Daten können insbesondere über:

- Typed Data,
- Shared Buffers,
- Scatter/Gather,
- Zero-Copy,
- DMA

an den Netzwerkstack übergeben werden.

```text
Data / Object
     ↓
Shared Buffer
     ↓
NetworkFlow
     ↓
Transport Provider
     ↓
Network Device
```

Unnötige Kopien sollen entsprechend den allgemeinen Copy-Avoidance-Regeln vermieden werden.

---

## Resource Economy

Jeder `NetworkFlow` ist einem Resource Context zurechenbar.

Dadurch können unter anderem:

- Bandbreite,
- Buffer,
- Queue Capacity,
- Connections,
- CPU-Aufwand,
- Netzwerkgeräte-Ressourcen

kontrolliert und bilanziert werden.

Flow Priority oder QoS hebt Resource Budgets nicht automatisch auf.

---

## Causation und Tracing

Ein `NetworkFlow` kann Causation- und Trace-Kontext übernehmen.

Dadurch können Netzwerkoperationen dem ursprünglichen Task, IPC Call oder Service Request zugeordnet werden.

```text
Task
 ↓
IPC Call
 ↓
NetworkFlow
 ↓
Remote Service
```

Dies ermöglicht End-to-End-Observability über lokale und entfernte Systemgrenzen.

---

## Security und Authority

Das Erzeugen oder Verwenden eines `NetworkFlow` benötigt die entsprechende Netzwerk-Authority.

Die Kenntnis eines Endpoints oder einer Adresse erzeugt keine Berechtigung zur Kommunikation.

Security, Nova.Trust und Data Sovereignty können sowohl:

- Ziel,
- Transport,
- Provider,
- Netzwerkpfad

einschränken.

---

## Sockets als Kompatibilitätsprojektion

Klassische Socket-Schnittstellen bleiben verfügbar.

Sie werden jedoch als Low-Level- beziehungsweise Compatibility API über den nativen Netzwerkmechanismen betrachtet.

```text
Native API
   → NetworkFlow

Compatibility API
   → Socket
       ↓
   NetworkFlow / Transport Provider
```

NovaOS-interne Komponenten sollen bevorzugt `NetworkFlow` und die semantischen Netzwerkcontracts verwenden.

---

## Normative Anforderungen

1. NovaOS MUSS `NetworkFlow` als native Abstraktion für Netzwerkkommunikation bereitstellen.
2. `NetworkFlow` DARF NICHT grundsätzlich an ein bestimmtes Transportprotokoll gebunden sein.
3. `NetworkFlow` MUSS semantische Flow Requirements aufnehmen können.
4. Die logische Flow Identity DARF NICHT ausschließlich von IP-Adressen, Ports oder Transportprotokoll abhängen.
5. Transport Provider und Netzwerkpfad MÜSSEN getrennt auswählbar sein können.
6. `NetworkFlow` MUSS IPv6 und IPv4 unterstützen können.
7. `NetworkFlow` MUSS mit TCP-, UDP-, QUIC- und zukünftigen Transportprovidern kombinierbar sein.
8. Cancellation und Deadlines MÜSSEN auf einen Flow propagierbar sein.
9. `NetworkFlow` MUSS in Structured Concurrency integrierbar sein.
10. Netzwerkressourcen MÜSSEN einem Resource Context zurechenbar sein.
11. `NetworkFlow` MUSS mit Shared Buffers und der Data-Movement-Architektur integrierbar sein.
12. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN Transport- und Pfadauswahl begrenzen können.
13. `NetworkFlow` MUSS Causation und Tracing unterstützen können.
14. Klassische Socket-APIs MÜSSEN als Low-Level- oder Compatibility-Schnittstelle möglich bleiben.
15. Flow, Transportprovider und tatsächlicher Netzwerkpfad MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- native Netzwerkkommunikation ist nicht an Socket-Semantik gebunden,
- Transportprovider können dynamisch gewählt werden,
- Netzwerkpfad und Transport bleiben getrennte Entscheidungen,
- Execution Contracts, Resource Economy und Structured Concurrency werden direkt integriert,
- zukünftige Transport- und Netzwerktechnologien können leichter eingebunden werden,
- Socket-Kompatibilität bleibt erhalten.

### Negative Konsequenzen

- eine zusätzliche native Abstraktion neben klassischen Sockets entsteht,
- Compatibility APIs müssen auf das native Modell abgebildet werden,
- Flow-, Transport- und Pfadresolution benötigen klar definierte Contracts.

---

## Verworfene Alternativen

### Socket als fundamentale NovaOS-Netzwerkabstraktion

Verworfen.

Sockets koppeln Anwendungen stark an klassische Address-Family-, Transport- und Endpoint-Semantik und bilden die weitergehenden NovaOS-Contracts nur eingeschränkt ab.

### NetworkFlow fest an Transportprotokoll binden

Verworfen.

Dies würde die semantische Transportauswahl aus `ADR-NETWORK-0003` unterlaufen.

### Separate Flow-Abstraktion pro Transport

Verworfen.

TCP, UDP, QUIC und zukünftige Transporte sollen Provider einer gemeinsamen Netzwerkarchitektur bleiben.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0001_IPv6_als_First_Class_Protokoll`
- `ADR-NETWORK-0002_IPv4_als_vollwertige_Kompatibilität`
- `ADR-NETWORK-0003_Transportabstraktion_über_semantische_Flow_Requirements`
- `ADR-NETWORK-0004_TCP_UDP_QUIC_als_Transportprovider`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-IPC-0007_Causation_und_Tracing_über_IPC_Grenzen`
- `ADR-DATAMOVE-0005_Copy_Avoidance_vor_blinder_Zero_Copy_Optimierung`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-FLOW-0001`
- `NPSPEC-NETWORK-FLOW-IDENTITY-0001`
- `NPSPEC-NETWORK-FLOW-LIFECYCLE-0001`
- `NPSPEC-NETWORK-FLOW-RESOURCE-0001`
- `NPSPEC-NETWORK-FLOW-SECURITY-0001`
- `NPSPEC-NETWORK-SOCKET-COMPAT-0001`
- `NPSPEC-NETWORK-FLOW-TEST-0001`

---

## Ergebnis

`NetworkFlow` wird zur nativen logischen Netzwerkabstraktion von NovaOS:

```text
Application / Capability
          ↓
      NetworkFlow
          ↓
 Semantic Requirements
          ↓
 Transport Resolution
          ↓
   Network Path
          ↓
       Hardware
```

Damit wird die Bedeutung einer Netzwerkkommunikation von ihrer konkreten Transport-, Adressierungs- und Hardwareimplementierung getrennt, ohne deren Kosten und Eigenschaften zu verbergen.