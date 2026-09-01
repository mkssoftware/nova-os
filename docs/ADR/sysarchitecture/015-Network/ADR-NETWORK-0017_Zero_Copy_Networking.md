# ADR-NETWORK-0017 – Zero-Copy Networking

## Status

Angenommen

## Kategorie

Network / Zero-Copy / Data Movement / Shared Buffers / NetworkFlow

## Kontext

NovaOS definiert Zero-Copy als systemweites Architekturprinzip und verwendet Shared Buffers, Memory Objects, Scatter/Gather und DMA für effiziente Datenbewegung.

Netzwerkkommunikation ist dabei ein besonders relevanter Datenpfad. Klassische Netzwerkstacks können Daten mehrfach zwischen Anwendung, Kernel, Protokollschichten, Treiber und Netzwerkgerät kopieren.

NovaOS soll solche Kopien vermeiden, wenn Daten sicher über gemeinsame Buffer, Ownership Transfer, Mapping oder DMA weitergereicht werden können.

Zero-Copy bleibt jedoch eine Optimierung. Security, Isolation, Data Sovereignty, Locality oder Hardwareanforderungen dürfen eine kontrollierte Kopie erzwingen.

---

## Entscheidung

NovaOS gestaltet seine Netzwerkarchitektur grundsätzlich für **Zero-Copy beziehungsweise Copy-Avoidance-Datenpfade**.

```text
Application
    ↓
Shared Buffer
    ↓
NetworkFlow
    ↓
Transport Provider
    ↓
Network Provider
    ↓
DMA / Network Device
```

Daten sollen zwischen den beteiligten Komponenten bevorzugt durch Referenz-, Mapping- oder Ownership-Operationen weitergegeben werden, anstatt ihren Inhalt zwischen separaten Buffern zu kopieren.

---

## Grundprinzip

```text
Move ownership and references
before moving bytes.
```

Zero-Copy ist dabei kein absolutes Ziel:

```text
Copy Avoidance > Blind Zero-Copy
```

---

## Shared Buffers

Shared Buffers bilden die bevorzugte Datenabstraktion für größere Netzwerktransfers.

Ein Buffer kann kontrolliert zwischen:

- Anwendung,
- Transportprovider,
- Netzwerkstack,
- Treiber,
- Netzwerkgerät

weitergereicht werden.

Die logische Buffer Identity bleibt dabei unabhängig von virtuellen oder physischen Adressen.

---

## Ownership und Lifetime

Zero-Copy Networking benötigt explizite Ownership- und Lifetime-Regeln.

```text
Producer
   ↓
Buffer
   ↓
Network Stack
   ↓
Device
   ↓
Completion
   ↓
Release / Reuse
```

Ein Buffer darf nicht wiederverwendet oder verändert werden, solange eine Netzwerkoperation noch darauf angewiesen ist.

Ownership Transfer ist zu bevorzugen, wenn der vorherige Besitzer den Buffer nicht weiter benötigt.

---

## Scatter/Gather und DMA

Zero-Copy Networking integriert sich direkt mit Scatter/Gather I/O und DMA.

```text
Shared Buffers
      ↓
Scatter/Gather
      ↓
DMA Mapping
      ↓
Network Device
```

Physische Kontiguität der Nutzdaten darf nicht grundsätzlich vorausgesetzt werden.

Gerätezugriff erfolgt über die kontrollierte DMA-/IOMMU-Architektur von NovaOS.

---

## Receive Path

Auch eingehende Netzwerkdaten sollen unnötige Kopien vermeiden.

```text
Network Device
      ↓
DMA
      ↓
Shared Buffer
      ↓
Network Stack
      ↓
NetworkFlow Consumer
```

Soweit Transportsemantik, Hardware und Security dies erlauben, sollen empfangene Daten ohne zusätzliche Payload-Kopien bis zum Consumer weitergegeben werden können.

---

## Kontrollierter Copy-Fallback

NovaOS garantiert nicht, dass jeder Netzwerktransfer vollständig ohne Kopie erfolgt.

Eine Kopie kann erforderlich oder sinnvoll sein bei:

- inkompatiblen Buffer-Eigenschaften,
- Security- oder Isolation-Grenzen,
- Data-Sovereignty-Anforderungen,
- notwendiger Datentransformation,
- ungeeigneter Hardware,
- ungünstiger NUMA- oder Device-Locality,
- unverhältnismäßigen Mapping- oder Pinning-Kosten.

```text
Transfer
   ↓
Zero-Copy feasible?
   ├── Yes → Shared / Map / DMA
   └── No  → Controlled Copy
```

Die korrekte Netzwerksemantik darf niemals von erfolgreichem Zero-Copy abhängen.

---

## Locality

Zero-Copy und Locality werden gemeinsam betrachtet.

Ein entfernter NUMA-Buffer oder ungünstig platzierter Gerätespeicher kann höhere Gesamtkosten verursachen als eine kontrollierte lokale Kopie.

Die Auswahl folgt daher dem allgemeinen Copy-Avoidance-Prinzip:

```text
Eligible Strategies
      ↓
Locality + Transfer Cost
      ↓
Zero-Copy / Mapping / DMA / Copy
```

---

## Resource Economy

Zero-Copy-Netzwerkpfade bleiben vollständig budgetiert.

Insbesondere müssen folgende Ressourcen zurechenbar sein:

- Shared Buffers,
- residenter Speicher,
- Pinned Memory,
- DMA Mappings,
- IOMMU-Ressourcen,
- Queue Capacity,
- Buffer Lifetime.

Zero-Copy darf nicht verwendet werden, um Memory- oder Network-Budgets zu umgehen.

---

## Security und Isolation

Gemeinsame Buffer dürfen Isolation und Capability Authority nicht abschwächen.

Ein `NetworkFlow` erhält nur Zugriff auf Buffer und Speicherbereiche, für die entsprechende Authority besteht.

DMA-Zugriff eines Netzwerkgeräts muss auf die für die jeweilige Operation autorisierten Speicherbereiche begrenzt bleiben.

---

## Transportprovider

TCP-, UDP-, QUIC- und zukünftige Transportprovider sollen Zero-Copy-fähige Datenpfade unterstützen können.

Die konkrete Transportsemantik kann jedoch zusätzliche Verarbeitung oder Kopien erforderlich machen.

Provider müssen deshalb keine Zero-Copy-Garantie vortäuschen, wenn diese technisch nicht eingehalten werden kann.

---

## Normative Anforderungen

1. NovaOS MUSS Netzwerkdatenpfade grundsätzlich für Copy Avoidance auslegen.
2. Shared Buffers SOLLEN die bevorzugte Buffer-Abstraktion für größere Netzwerktransfers sein.
3. Buffer Identity DARF NICHT von einer bestimmten virtuellen oder physischen Adresse abhängen.
4. Ownership und Lifetime von Netzwerkbuffern MÜSSEN explizit kontrolliert werden.
5. Zero-Copy Networking MUSS mit Scatter/Gather I/O und DMA integrierbar sein.
6. Netzwerkgeräte DÜRFEN nur auf autorisierte Speicherbereiche zugreifen.
7. Eingehende und ausgehende Datenpfade SOLLEN unnötige Payload-Kopien vermeiden.
8. Zero-Copy DARF NICHT Voraussetzung für die Korrektheit eines `NetworkFlow` sein.
9. NovaOS MUSS einen kontrollierten Copy-Fallback unterstützen.
10. Security-, Isolation- und Data-Sovereignty-Constraints MÜSSEN Vorrang vor Zero-Copy haben.
11. Locality und Gesamtkosten MÜSSEN bei der Wahl zwischen Zero-Copy und Kopieren berücksichtigt werden können.
12. Pinned Memory, DMA Mappings und Shared Buffers MÜSSEN der Resource Economy zugerechnet werden.
13. Zero-Copy DARF Resource Budgets NICHT umgehen.
14. Transportprovider MÜSSEN unterschiedliche Zero-Copy-Fähigkeiten deklarieren können.
15. Tatsächliche Kopien und Zero-Copy-Pfade MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- weniger Speicherbandbreite und CPU-Aufwand,
- geringere Latenz bei geeigneten Netzwerktransfers,
- direkte Integration mit DMA und Shared Buffers,
- effizientere große Datenströme,
- einheitliche Data-Movement-Architektur über IPC, I/O und Netzwerk hinweg.

### Negative Konsequenzen

- Buffer-Lifetime und Ownership werden wichtiger,
- DMA und Pinning erzeugen zusätzliche Ressourcenbindungen,
- nicht jeder Netzwerkpfad kann vollständig Zero-Copy arbeiten,
- Locality kann eine Kopie gelegentlich wirtschaftlicher machen.

---

## Verworfene Alternativen

### Zero-Copy für jeden Netzwerktransfer erzwingen

Verworfen.

Mapping-, Pinning-, Synchronisations- und Locality-Kosten können eine Kopie günstiger oder notwendig machen.

### Separates Netzwerk-Buffer-Modell

Verworfen.

Netzwerkkommunikation soll die systemweite Shared-Buffer- und Data-Movement-Architektur verwenden.

### Anwendungen direkt mit DMA-Adressen arbeiten lassen

Verworfen.

Dies würde Isolation, Portabilität, IOMMU-Kontrolle und das allgemeine Memory-Object-Modell umgehen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0015_QoS_und_Traffic_Shaping`
- `ADR-NETWORK-0016_Adaptive_Congestion_Control`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0009_Unified_Data_Object_Pipelining`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`
- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0003_Scatter_Gather_IO`
- `ADR-DATAMOVE-0004_DMA_Based_Transfer`
- `ADR-DATAMOVE-0005_Copy_Avoidance_vor_blinder_Zero_Copy_Optimierung`
- `ADR-DATAMOVE-0006_Data_und_Cache_Locality`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-ZEROCOPY-0001`
- `NPSPEC-NETWORK-BUFFER-0001`
- `NPSPEC-NETWORK-ZEROCOPY-TX-0001`
- `NPSPEC-NETWORK-ZEROCOPY-RX-0001`
- `NPSPEC-NETWORK-DMA-0001`
- `NPSPEC-NETWORK-ZEROCOPY-FALLBACK-0001`
- `NPSPEC-NETWORK-ZEROCOPY-TEST-0001`

---

## Ergebnis

NovaOS verbindet den Netzwerkstack direkt mit seiner systemweiten Zero-Copy- und Data-Movement-Architektur:

```text
Application
     ↓
Shared Buffer
     ↓
NetworkFlow
     ↓
Transport
     ↓
DMA / Device
```

Daten werden bevorzugt durch Ownership, Referenzen und kontrollierte Mappings bewegt. Tatsächliche Kopien bleiben dort zulässig, wo sie aus Sicherheits-, Kompatibilitäts-, Locality- oder Kostengründen die bessere Lösung darstellen.