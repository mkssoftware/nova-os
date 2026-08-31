# ADR-DRIVER-0009 – Device Model als deklarativer Systemgraph

## Status

Angenommen

## Kategorie

Driver / Device Model / Declarative System Model / Hardware Topology / Lifecycle

## Kontext

NovaOS modelliert die physische Hardwaretopologie bereits über den Hardware System Graph der HAL.

Für die Treiberarchitektur reicht eine reine Auflistung erkannter Geräte jedoch nicht aus. Das System muss zusätzlich Beziehungen zwischen Devices, Treibern, Ressourcen, Capabilities und logischen Gerätefunktionen darstellen können.

Ein Gerät kann aus mehreren Funktionen bestehen, von anderen Geräten abhängen oder über Busse, Bridges und Controller verbunden sein.

Das Device Model soll deshalb kein separates imperatives Geräteverzeichnis bilden, sondern Teil des deklarativen Systemmodells von NovaOS sein.

---

## Entscheidung

NovaOS modelliert Devices als **deklarativen, versionierten Device System Graph**.

```text
Hardware System Graph
        ↓
Device System Graph
        ↓
Driver Binding
        ↓
Device Capabilities
        ↓
I/O Services
```

Devices werden als stabile Knoten mit Eigenschaften und expliziten Beziehungen dargestellt.

Der Graph beschreibt den bekannten aktuellen Gerätezustand und bildet die Grundlage für Driver Binding, Ressourcenverwaltung, Hotplug und Architecture Introspection.

---

## Grundprinzip

```text
Devices are not a list.

Devices are a graph of identities,
relationships and capabilities.
```

---

## Device Nodes

Ein Device Node repräsentiert eine identifizierbare physische oder logische Gerätefunktion.

Ein Node kann insbesondere besitzen:

- Device Identity,
- Device Type,
- Parent-/Child-Beziehungen,
- Hardware Properties,
- zugewiesene Ressourcen,
- bereitgestellte Capabilities,
- Lifecycle State,
- gebundenen Driver beziehungsweise Provider.

Device Identity muss unabhängig von einem temporären Pfad oder der Erkennungsreihenfolge sein können.

---

## Beziehungen

Der Device Graph bildet Beziehungen explizit ab.

Beispiele:

```text
PCIe Root
   ↓
Controller
   ├── Device A
   └── Device B
```

oder:

```text
Physical Device
   ├── Function A
   ├── Function B
   └── Logical Device
```

Damit können Abhängigkeiten, zusammengesetzte Geräte und Multifunction Devices ohne starre Baumstruktur modelliert werden.

---

## Verbindung zum Hardware System Graph

Der Device System Graph ergänzt den Hardware System Graph aus `ADR-HAL-0004`.

```text
Hardware System Graph
        ↕
Device System Graph
```

Der Hardware System Graph beschreibt die physische Plattform- und Topologiesicht.

Der Device System Graph beschreibt die für Driver Binding, Capabilities, Lifecycle und I/O relevante Gerätesicht.

Beide Graphen dürfen gemeinsame Identitäten und Beziehungen referenzieren, ohne dieselben Informationen unnötig zu duplizieren.

---

## Driver Binding

Driver Binding erfolgt anhand deklarierter Device- und Driver-Eigenschaften.

```text
Device Node
     +
Driver Contract
     ↓
Binding Policy
     ↓
Driver Instance
```

Die physische Position eines Geräts allein bestimmt nicht seine Treiberidentität.

Binding berücksichtigt zusätzlich Security-, Trust-, Capability- und Kompatibilitätsanforderungen.

---

## Device Capabilities

Ein Device Node kann deklarieren, welche technischen Fähigkeiten er bereitstellt.

Diese Fähigkeiten bilden die Grundlage für die Auswahl geeigneter Driver und System Provider.

Device Capabilities sind von den Security Capabilities aus `ADR-DRIVER-0004` zu unterscheiden:

```text
Device Capability
→ Was kann das Gerät?

Driver Capability
→ Was darf der Treiber?
```

---

## Lifecycle

Der Device Graph integriert die deklarativen Device Lifecycle States aus `ADR-HAL-0007`.

Änderungen durch:

- Discovery,
- Hotplug,
- Driver Binding,
- Driver Replacement,
- Device Failure,
- Resource Reassignment

werden als kontrollierte Änderungen des Systemgraphen behandelt.

Graphänderungen müssen konsistent veröffentlicht werden.

---

## Location und Topologie

Device Nodes behalten ihre Beziehungen zur Hardwaretopologie.

Dadurch können Scheduler, Memory Manager und I/O-System beispielsweise erkennen:

- NUMA-Nähe,
- Busstruktur,
- Accelerator-Nähe,
- DMA-/IOMMU-Zuordnung,
- gemeinsame Controller,
- relevante Locality-Beziehungen.

Der Device Graph wird damit zu einer wichtigen Informationsquelle für topology-aware Systementscheidungen.

---

## Architecture Introspection

Der Device System Graph ist Bestandteil der systemweiten Architecture Introspection.

Autorisierte Komponenten können damit nachvollziehen:

```text
Device
  ↓
Topology
  ↓
Resources
  ↓
Driver
  ↓
Capabilities
  ↓
Lifecycle
```

Nicht autorisierte Introspection darf keine sicherheitskritischen Hardwareinformationen offenlegen.

---

## Normative Anforderungen

1. NovaOS MUSS Devices als deklarativen Systemgraph modellieren.
2. Device Nodes MÜSSEN stabile Device Identities besitzen können.
3. Device Identity DARF NICHT ausschließlich von Discovery-Reihenfolge oder temporären Pfaden abhängen.
4. Beziehungen zwischen Devices MÜSSEN explizit modellierbar sein.
5. Der Device Graph MUSS physische und logische Device-Funktionen darstellen können.
6. Der Device Graph MUSS mit dem Hardware System Graph verknüpft sein.
7. Informationen SOLLEN zwischen Hardware- und Device-Graph nicht unnötig dupliziert werden.
8. Driver Binding MUSS auf deklarativen Device- und Driver-Contracts basieren können.
9. Device Capabilities und Driver Authorities MÜSSEN getrennte Konzepte bleiben.
10. Device Lifecycle States MÜSSEN im Graph repräsentierbar sein.
11. Hotplug und Driver Replacement MÜSSEN als konsistente Graphänderungen darstellbar sein.
12. Topologie- und Locality-Beziehungen MÜSSEN für andere Systemkomponenten nutzbar sein.
13. Graphänderungen MÜSSEN versioniert beziehungsweise konsistent beobachtbar sein.
14. Der Device Graph MUSS kontrolliert introspektierbar sein.
15. Security- und Trust-Policies MÜSSEN auch für Graphzugriff und Driver Binding gelten.

---

## Konsequenzen

### Positive Konsequenzen

- einheitliches systemweites Device Model,
- natürliche Darstellung komplexer und zusammengesetzter Hardware,
- bessere Grundlage für Hotplug und Driver Binding,
- topology-aware Scheduling und Data Placement,
- konsistente Architecture Introspection,
- direkte Integration in das Declarative System Model.

### Negative Konsequenzen

- Device- und Hardware-Graph müssen konsistent miteinander verbunden werden,
- Graphänderungen benötigen definierte Publikations- und Versionssemantik,
- Device Identity muss für unterschiedliche Hardwareklassen zuverlässig bestimmt werden.

---

## Verworfene Alternativen

### Flache globale Geräteliste

Verworfen.

Sie bildet Abhängigkeiten, Topologie und zusammengesetzte Geräte nur unzureichend ab.

### Rein hierarchischer Device Tree

Verworfen.

Nicht alle relevanten Gerätebeziehungen lassen sich sinnvoll als einzelner Baum darstellen.

### Separates Device Model pro Treibersubsystem

Verworfen.

Dies würde systemweite Topologie-, Lifecycle- und Introspection-Funktionen fragmentieren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0003_Declarative_System_Model`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-DRIVER-0003_Driver_Isolation_Domains`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-DRIVER-0005_Driver_Hot_Reload`
- `ADR-DRIVER-0006_Driver_Live_Replacement`

---

## Zugehörige NPSPECs

- `NPSPEC-DRIVER-DEVICE-GRAPH-0001`
- `NPSPEC-DRIVER-DEVICE-IDENTITY-0001`
- `NPSPEC-DRIVER-DEVICE-RELATION-0001`
- `NPSPEC-DRIVER-DEVICE-CAPABILITY-0001`
- `NPSPEC-DRIVER-DEVICE-BINDING-0001`
- `NPSPEC-DRIVER-DEVICE-GRAPH-TEST-0001`

---

## Ergebnis

NovaOS behandelt die Gerätearchitektur als Teil seines deklarativen Systemmodells:

```text
Hardware Topology
       ↕
Device System Graph
       ↓
Driver Binding
       ↓
Capabilities
       ↓
I/O
```

Dadurch werden Geräte, ihre Beziehungen, Treiber, Ressourcen und Fähigkeiten als zusammenhängendes, introspektierbares Systemmodell statt als isolierte Geräteliste verwaltet.