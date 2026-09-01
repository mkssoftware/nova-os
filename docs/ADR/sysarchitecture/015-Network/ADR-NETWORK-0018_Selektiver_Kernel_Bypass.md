# ADR-NETWORK-0018 – Selektiver Kernel Bypass

## Status

Angenommen

## Kategorie

Network / Kernel Bypass / High Performance / Zero-Copy / Security

## Kontext

Der normale NovaOS-Netzwerkpfad bietet zentrale Funktionen wie Security Enforcement, Resource Accounting, Routing Policy, QoS und Treiberabstraktion.

Für bestimmte Hochleistungs-Workloads kann die wiederholte Verarbeitung durch allgemeine Kernelpfade jedoch unnötige Latenz, Context Switches und Queueing verursachen.

NovaOS besitzt bereits eine allgemeine Architektur für autorisierten Kernel Bypass. Diese soll auch für Netzwerkkommunikation nutzbar sein, ohne einen zweiten, unkontrollierten Netzwerkstack neben der nativen Architektur zu schaffen.

---

## Entscheidung

NovaOS unterstützt **selektiven Kernel Bypass für autorisierte Hochleistungs-NetworkFlows**.

```text
Normal Path

NetworkFlow
    ↓
Kernel Network Stack
    ↓
Driver
    ↓
Device


Authorized Bypass

NetworkFlow
    ↓
Authorized Fast Path
    ↓
Device Queue / DMA
    ↓
Device
```

Kernel Bypass ist eine optionale Ausführungsform eines `NetworkFlow` und keine separate Netzwerkarchitektur.

Der Kernel behält die Kontrolle über Autorisierung, Isolation, Ressourcenvergabe, Einrichtung und Widerruf.

---

## Grundprinzip

```text
Bypass the data path,
not the security model.
```

---

## Selektive Verwendung

Kernel Bypass wird nicht zum Standardpfad für allgemeine Netzwerkkommunikation.

Er ist für Workloads vorgesehen, bei denen der zusätzliche Aufwand des normalen Kernelpfads nachweislich relevant ist, beispielsweise bei:

- sehr hoher Paket- oder Datenrate,
- extrem niedrigen Latenzanforderungen,
- spezialisierten Netzwerkdiensten,
- geeigneten Realtime-Workloads.

Die konkrete Entscheidung erfolgt anhand von Contracts, Authority und verfügbaren Ressourcen.

---

## Autorisierung

Eine Execution Domain erhält Kernel Bypass nur mit expliziter Authority.

```text
NetworkFlow
     ↓
Bypass Request
     ↓
Authority + Trust
     ↓
Resource / Isolation Check
     ↓
Bypass Granted / Rejected
```

Die Fähigkeit zur normalen Netzwerkkommunikation beinhaltet keine automatische Berechtigung für Kernel Bypass.

Fail-Safe Default ist die Verwendung des normalen Netzwerkpfads beziehungsweise die Ablehnung eines zwingend geforderten Bypass-Modus.

---

## NetworkFlow bleibt erhalten

Auch ein Bypass-Datenpfad bleibt einem `NetworkFlow` zugeordnet.

Dadurch bleiben insbesondere:

- Flow Identity,
- Flow Requirements,
- Security Context,
- Resource Context,
- QoS,
- Causation und Tracing

Teil des systemweiten Netzwerkmodells.

Kernel Bypass darf keine inkompatible parallele Netzwerksemantik erzeugen.

---

## Ressourcen und Isolation

Direkter Zugriff auf Netzwerkressourcen erfolgt ausschließlich über kontrolliert zugewiesene Ressourcen.

Dazu können gehören:

- Device Queues,
- DMA Contexts,
- Shared Buffers,
- Interrupt-/Completion-Ressourcen,
- IOMMU Mappings.

```text
Execution Domain
      ↓
Assigned Network Resources
      ↓
IOMMU / Device Isolation
      ↓
Network Device
```

Eine Bypass-Domain erhält keinen allgemeinen Zugriff auf beliebige physische Speicher- oder Geräteressourcen.

---

## Zero-Copy und DMA

Kernel Bypass integriert sich in die bestehende Shared-Buffer-, Zero-Copy-, Pinned-Memory- und DMA-Architektur.

Es entsteht kein separates Bypass-Speichermodell.

```text
Shared Buffer
     ↓
Authorized DMA Mapping
     ↓
Assigned Device Queue
     ↓
Network Device
```

Pinning und DMA-Ressourcen bleiben budgetiert und kontrolliert.

---

## Security und Firewall

Kernel Bypass darf die Identity- und Capability-Aware Firewall sowie andere verbindliche Security Policies nicht grundsätzlich umgehen.

Sicherheitsentscheidungen können bereits bei Einrichtung und Autorisierung eines Bypass-Pfads durchgesetzt und durch geeignete Hardware- oder Providermechanismen abgesichert werden.

Kann eine erforderliche Security Policy im Bypass-Modus nicht zuverlässig durchgesetzt werden, ist dieser Modus für den betreffenden Flow nicht zulässig.

---

## Resource Economy und QoS

Bypass-Ressourcen werden der verursachenden Resource Domain zugerechnet.

Insbesondere müssen:

- Device Queues,
- Bandbreite,
- Shared Buffers,
- Pinned Memory,
- DMA Mappings

budgetiert werden.

Kernel Bypass darf weder Traffic Shaping noch Resource Budgets implizit aufheben.

---

## Lifecycle und Revocation

Kernel Bypass ist widerrufbar.

Ein Bypass-Pfad kann insbesondere beendet werden bei:

- Ende der Execution Domain,
- Capability Revocation,
- Device Removal,
- Security- oder Policy-Änderung,
- Ressourcenverletzung.

Der Kernel behält die notwendige Control Plane, um zugewiesene Ressourcen sicher zurückzunehmen.

---

## Fallback

Wenn Kernel Bypass nicht verfügbar oder nicht zulässig ist, kann ein `NetworkFlow` den normalen Kernelpfad verwenden, sofern sein Contract dies erlaubt.

```text
Bypass Preferred
      ↓
Unavailable
      ↓
Normal Kernel Path
```

Ist Kernel Bypass dagegen ausdrücklich als Hard Requirement definiert, muss die Nichterfüllbarkeit als expliziter Fehler behandelt werden.

---

## Introspection

Kernel-Bypass-Nutzung muss kontrolliert introspektierbar sein.

NovaOS soll insbesondere nachvollziehen können:

```text
NetworkFlow
     ↓
Normal / Bypass Path
     ↓
Assigned Resources
     ↓
Device / Queue
```

Damit bleiben Performance, Ressourcenverbrauch und Sicherheitszustand diagnostizierbar.

---

## Normative Anforderungen

1. NovaOS MUSS selektiven Kernel Bypass für geeignete Netzwerkworkloads unterstützen können.
2. Kernel Bypass DARF NICHT der verpflichtende Standardpfad allgemeiner Netzwerkkommunikation sein.
3. Kernel Bypass MUSS explizite Authority erfordern.
4. Normale Network Authority DARF NICHT automatisch Kernel-Bypass-Authority einschließen.
5. Ein Bypass-Pfad MUSS weiterhin einem `NetworkFlow` zugeordnet bleiben.
6. Kernel Bypass DARF keine separate unkontrollierte Netzwerkarchitektur erzeugen.
7. Der Kernel MUSS Kontrolle über Einrichtung, Ressourcenvergabe und Widerruf behalten.
8. Direkter Gerätezugriff MUSS auf explizit zugewiesene Ressourcen begrenzt sein.
9. Kernel Bypass MUSS mit IOMMU-, DMA-, Shared-Buffer- und Pinned-Memory-Mechanismen integrierbar sein.
10. Kernel Bypass DARF Security-, Capability- oder Data-Sovereignty-Constraints NICHT umgehen.
11. Nicht zuverlässig durchsetzbare Hard Constraints MÜSSEN Kernel Bypass für den betreffenden Flow ausschließen.
12. Bypass-Ressourcen MÜSSEN der systemweiten Resource Economy zugerechnet werden.
13. Kernel Bypass DARF Resource Budgets und verbindliche QoS-Grenzen NICHT umgehen.
14. Bypass-Ressourcen MÜSSEN kontrolliert widerrufbar sein.
15. Bypass-Nutzung und zugewiesene Ressourcen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- sehr niedrige Netzwerk-Latenzen werden für geeignete Workloads möglich,
- Kernel-Overhead kann bei hohen Datenraten reduziert werden,
- Zero-Copy und direkter DMA-Zugriff können effizienter genutzt werden,
- Security und Resource Economy bleiben trotz Fast Path erhalten,
- Anwendungen benötigen kein separates Netzwerkmodell.

### Negative Konsequenzen

- Ressourcen- und Geräteisolation werden komplexer,
- Bypass-Pfade benötigen geeignete Hardware- und Treiberunterstützung,
- bestimmte Kernel-Netzwerkfunktionen müssen bereits bei Einrichtung oder über Hardwaremechanismen durchgesetzt werden.

---

## Verworfene Alternativen

### Genereller Kernel Bypass für alle Anwendungen

Verworfen.

Dies würde Sicherheitskontrolle, Ressourcenverwaltung und die Komplexität allgemeiner Netzwerkkommunikation unnötig verschlechtern.

### Vollständig unabhängiger Userspace-Netzwerkstack

Verworfen.

Kernel Bypass soll den Datenpfad optimieren und nicht die native NovaOS-Netzwerkarchitektur duplizieren.

### Unbeschränkter direkter Gerätezugriff

Verworfen.

Dies würde Isolation, IOMMU-Kontrolle, Resource Economy und das Capability-Modell verletzen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-NETWORK-0005_NetworkFlow_als_native_Abstraktion`
- `ADR-NETWORK-0009_Identity_und_Capability_Aware_Firewall`
- `ADR-NETWORK-0015_QoS_und_Traffic_Shaping`
- `ADR-NETWORK-0017_Zero_Copy_Networking`
- `ADR-DATAMOVE-0007_Kernel_Bypass_nur_für_autorisierte_Hochleistungsdomänen`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-NETWORK-KERNEL-BYPASS-0001`
- `NPSPEC-NETWORK-KERNEL-BYPASS-AUTH-0001`
- `NPSPEC-NETWORK-KERNEL-BYPASS-RESOURCE-0001`
- `NPSPEC-NETWORK-KERNEL-BYPASS-IOMMU-0001`
- `NPSPEC-NETWORK-KERNEL-BYPASS-REVOCATION-0001`
- `NPSPEC-NETWORK-KERNEL-BYPASS-TEST-0001`

---

## Ergebnis

NovaOS erlaubt autorisierten Hochleistungsdomänen einen direkten Netzwerk-Datenpfad, ohne die systemweite Kontrolle aufzugeben:

```text
NetworkFlow
     ↓
Authority + Policy
     ↓
Normal Path / Kernel Bypass
                 ↓
        Shared Buffer + DMA
                 ↓
          Assigned Queue
                 ↓
          Network Device
```

Damit kann der Kernel für geeignete Datenpfade umgangen werden, während Security, Isolation, Resource Economy und die native `NetworkFlow`-Semantik weiterhin verbindlich bleiben.