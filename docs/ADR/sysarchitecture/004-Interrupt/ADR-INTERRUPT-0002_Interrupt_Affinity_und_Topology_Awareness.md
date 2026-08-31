# ADR-INTERRUPT-0002 – Interrupt Affinity und Topology Awareness

## Status

Angenommen

## Kategorie

Kernel / Interrupts / SMP / NUMA / Topologie / Scheduling / Lokalität / Performance

## Kontext

Auf Mehrkern- und NUMA-Systemen ist es nicht optimal, Interrupts beliebig auf irgendeiner CPU auszuführen.

Die Wahl der Ziel-CPU beeinflusst unter anderem:

- Cache-Lokalität,
- NUMA-Zugriffe,
- Queue-Lokalität,
- Scheduler-Interaktion,
- Latenz,
- Energieverbrauch,
- Echtzeitverhalten,
- Lastverteilung.

Beispiel:

```text
NVMe Device
    ↓
PCIe Root Complex
    ↓
NUMA Node 1
```

Wird der zugehörige Interrupt überwiegend auf einer CPU von NUMA Node 0 verarbeitet, entstehen zusätzliche Datenbewegungen und Remote-Zugriffe.

Gleichzeitig darf Interrupt Routing nicht ausschließlich nach Lokalität erfolgen.

Weitere Anforderungen können wichtiger sein:

- explizite CPU-Bindung,
- Echtzeitisolierung,
- reservierte CPUs,
- CPU Hotplug,
- Energiepolitik,
- Interruptlast,
- Sicherheits- oder Isolationseigenschaften.

NovaOS benötigt deshalb eine zentrale Interrupt-Affinity-Architektur, die die vorhandene Hardwaretopologie berücksichtigt, ohne Routingentscheidungen in die einzelnen Interrupt-Controller-Backends zu verlagern.

---

## Entscheidung

NovaOS behandelt **Interrupt Affinity als explizite, topology-aware Systementscheidung**.

Der Interrupt Controller stellt den Routingmechanismus bereit.

Eine darüberliegende Interrupt-Affinity-Policy bestimmt das gewünschte Ziel anhand von:

- Hardwaretopologie,
- Device-Lokalität,
- CPU-Verfügbarkeit,
- NUMA,
- Schedulerzustand,
- Resource Policy,
- Realtime- und Determinism-Anforderungen,
- expliziten Affinity Constraints.

Das Grundmodell lautet:

```text
Interrupt Source
      ↓
Affinity Constraints
      ↓
Hardware System Graph
      ↓
Interrupt Affinity Policy
      ↓
Target CPU / CPU Set
      ↓
Interrupt Controller
```

---

## Grundprinzip

```text
Interrupt routing is a policy decision.

Interrupt delivery is a hardware mechanism.
```

und:

```text
Prefer locality.

Respect stronger constraints first.
```

---

## Interrupt Affinity

Interrupt Affinity beschreibt, auf welchen CPUs eine Interruptquelle verarbeitet werden darf beziehungsweise bevorzugt verarbeitet werden soll.

Konzeptionell:

```text
InterruptAffinity {
    allowedCpuSet
    preferredCpuSet
    excludedCpuSet
    fixedTarget
}
```

Nicht jede Interruptquelle benötigt alle Felder.

---

## Hard und Soft Affinity

NovaOS unterscheidet zwischen:

```text
Required Affinity
```

und:

```text
Preferred Affinity
```

Beispiele:

```text
Required:
    CPU must be in realtime partition

Preferred:
    CPU should be on device-local NUMA node
```

Preferred Affinity darf verletzt werden, wenn stärkere Anforderungen dies notwendig machen.

Required Affinity darf nicht stillschweigend ignoriert werden.

---

## Priorität der Entscheidungen

Die grundlegende Reihenfolge folgt den allgemeinen NovaOS-Architekturprinzipien.

Konzeptionell:

```text
Safety
Security / Isolation
Hard Realtime
Required Affinity
Determinism
CPU Availability
Topology / Locality
Load Distribution
Energy / Performance Preference
```

Nicht jede Plattform oder Policy benötigt alle Ebenen.

---

## Topology Awareness

Die Affinity-Entscheidung verwendet den Hardware System Graph aus `ADR-HAL-0004`.

Relevante Beziehungen können sein:

```text
Device
  ↓
Bus / Root Complex
  ↓
NUMA Node
  ↓
CPU Package
  ↓
Core
  ↓
Hardware Thread
```

Auch Cache-Sharing und weitere Locality-Daten können berücksichtigt werden.

---

## Device Locality

Für ein Gerät kann eine bevorzugte CPU-Menge aus seiner Hardwaretopologie abgeleitet werden.

Beispiel:

```text
Device
  ↓
PCIe Root Complex
  ↓
NUMA Node 1
  ↓
CPUs {8..15}
```

Damit ergibt sich:

```text
preferredCpuSet = CPUs {8..15}
```

Diese Information ist eine Präferenz und keine automatische harte Bindung.

---

## NUMA

NUMA-Lokalität ist ein First-Class-Kriterium.

Wenn:

- Gerät,
- DMA-Puffer,
- Verarbeitungsthread

auf demselben NUMA Node liegen, sollen die zugehörigen Interrupts bevorzugt ebenfalls dort verarbeitet werden.

Zielmodell:

```text
Device
   ↓
DMA Memory
   ↓
Interrupt CPU
   ↓
Processing Thread
```

möglichst innerhalb derselben Locality Domain.

---

## Cache-Lokalität

Wenn sinnvoll, kann die Affinity Policy auch Cache-Topologie berücksichtigen.

Beispiel:

```text
Interrupt CPU
      ↓
Shared LLC
      ↓
Worker CPU
```

Dies kann bei hochfrequenten Netzwerk- oder Storagequeues relevant sein.

Cache-Lokalität bleibt jedoch ein Optimierungsziel und keine universelle harte Regel.

---

## Queue Awareness

Moderne Geräte besitzen häufig mehrere Hardwarequeues.

Beispiel:

```text
NIC
 ├── Queue 0 → IRQ 0
 ├── Queue 1 → IRQ 1
 ├── Queue 2 → IRQ 2
 └── Queue 3 → IRQ 3
```

NovaOS soll Queue und Interrupt gemeinsam platzieren können.

Konzeptionell:

```text
Queue N
   ↓
Interrupt N
   ↓
CPU N
   ↓
Worker N
```

Dadurch können unnötige Cross-CPU-Übergaben reduziert werden.

---

## MSI-X

MSI-X eignet sich besonders für topology-aware Routing, da einzelne Queues beziehungsweise Funktionen eigene Interruptvektoren besitzen können.

NovaOS soll diese Möglichkeit nutzen können, ohne Treiber an konkrete APIC- oder Controllerdetails zu binden.

---

## Scheduler-Integration

Interrupt Affinity und Thread Scheduling dürfen nicht vollständig unabhängig voneinander optimiert werden.

Beispiel:

```text
IRQ → CPU 6
Worker → CPU 27
```

kann unnötige:

- Cache Transfers,
- Queue Handoffs,
- NUMA Traffic

erzeugen.

Der Scheduler soll deshalb die Affinity von zugehörigen Interruptquellen berücksichtigen können.

Umgekehrt darf das Interrupt Subsystem relevante Worker-Affinity berücksichtigen.

---

## Keine harte Kopplung

Interrupt Handler und Verarbeitungsthread müssen nicht zwingend auf derselben CPU laufen.

Mögliche Architektur:

```text
Interrupt
   ↓
Minimal IRQ Handler
   ↓
Queue Work
   ↓
Worker Thread
```

Affinity optimiert den Datenfluss, erzwingt aber keine unnötige Ausführung kompletter Treiberlogik im Interruptkontext.

---

## CPU Sets

Affinity wird bevorzugt über CPU Sets statt über einzelne numerische CPU-IDs modelliert.

Beispiele:

```text
Allowed:
    {4,5,6,7}

Preferred:
    {6,7}
```

Damit können:

- SMT,
- Core-Gruppen,
- NUMA Nodes,
- Realtime Partitions

flexibler beschrieben werden.

---

## Stable CPU Identity

Die Policy darf nicht davon ausgehen, dass eine temporäre CPU-Indexnummer eine dauerhaft stabile physische Identität darstellt.

Die HAL-/Topologieebene stellt die Beziehung zwischen logischer CPU und Hardwaretopologie bereit.

---

## CPU Hotplug

Wird eine CPU offline genommen, müssen auf sie geroutete Interrupts neu bewertet werden.

Konzeptionell:

```text
CPU Offline
    ↓
Find affected IRQs
    ↓
Recalculate Affinity
    ↓
Route to valid target
```

Ein Interrupt darf nicht dauerhaft auf eine nicht verfügbare CPU gebunden bleiben.

---

## CPU Online

Beim Hinzufügen beziehungsweise Reaktivieren einer CPU darf die Affinity Policy Routingentscheidungen neu optimieren.

Dies ist jedoch nicht zwingend sofort erforderlich.

Unnötiges Routing-Churn soll vermieden werden.

---

## Affinity Rebalancing

NovaOS darf Interrupt Affinity dynamisch anpassen.

Mögliche Gründe:

- CPU Hotplug,
- starke IRQ-Last,
- NUMA-Verlagerung,
- Worker Migration,
- Energiepolitik,
- Gerätequeueänderungen.

Rebalancing ist eine Policyentscheidung und keine Pflicht jedes Controllers.

---

## Hysterese

Interrupts dürfen nicht bei kleinen Laständerungen ständig zwischen CPUs verschoben werden.

Die Policy soll deshalb Hysterese beziehungsweise Mindestvorteile für Rebalancing verwenden können.

Beispiel:

```text
small predicted gain
    ↓
keep current route
```

statt permanenter Migration.

---

## Routing Cost

Die Auswahl einer Ziel-CPU kann über ein Kostenmodell erfolgen.

Konzeptionell:

```text
Cost =
    NUMA Distance
  + Queue Distance
  + CPU Load
  + Cache Penalty
  + Migration Cost
  + Policy Penalty
```

Die konkrete Gewichtung ist Policy und nicht Bestandteil dieser ADR.

---

## Locality als Optimierungsziel

Diese ADR folgt `ADR-ARCH-0018`.

Lokalität gilt als Optimierungsziel, sofern keine stärkeren Anforderungen entgegenstehen.

Es gilt nicht:

```text
nearest CPU always wins
```

sondern:

```text
best valid CPU under current constraints
```

---

## Resource Economy

Interruptverarbeitung verbraucht CPU-Ressourcen.

Sehr interruptlastige Geräte dürfen deshalb in die systemweite Ressourcenökonomie einbezogen werden.

Dies kann beispielsweise verhindern, dass:

```text
one CPU
```

durch zahlreiche hochfrequente Interruptquellen überlastet wird.

---

## Load Awareness

Die aktuelle Interrupt- und CPU-Last darf als Optimierungskriterium verwendet werden.

Beispiel:

```text
CPU 4:
    local
    overloaded

CPU 5:
    local
    lightly loaded
```

Dann kann CPU 5 bevorzugt werden.

Die Lastentscheidung darf Required Constraints nicht verletzen.

---

## Realtime

Echtzeit-Workloads können explizite Interrupt-Affinity verlangen.

Beispiel:

```text
Realtime Device
      ↓
IRQ
      ↓
Reserved Realtime CPU
```

oder umgekehrt:

```text
General Device IRQs
      ↓
Excluded from realtime CPUs
```

Diese Constraints sind stärker als reine Last- oder Locality-Optimierung.

---

## CPU Isolation

NovaOS muss CPUs beziehungsweise CPU Sets für bestimmte Aufgaben reservieren können.

Interrupt Affinity muss solche Isolation respektieren.

Beispiel:

```text
CPU Set A:
    realtime isolated

CPU Set B:
    general interrupt handling
```

Nicht autorisierte Interruptquellen dürfen nicht automatisch in isolierte CPU Sets migriert werden.

---

## Deterministic Mode

Im Deterministic Mode kann Interrupt Routing fixiert werden.

Beispiel:

```text
IRQ 42
    ↓
CPU 3
```

Während des deterministischen Scopes darf adaptives Rebalancing deaktiviert sein.

Änderungen sind nur zulässig, wenn eine harte Voraussetzung dies erzwingt, beispielsweise CPU-Ausfall.

---

## Forced Affinity

Systemkomponenten beziehungsweise autorisierte Administratoren können eine konkrete Affinity erzwingen.

Beispiel:

```text
Required CPU Set = {4,5}
```

Kann diese Constraint nicht erfüllt werden, muss dies als Fehler behandelt werden.

NovaOS darf dann nicht still auf beliebige CPUs ausweichen.

---

## Preferred Affinity

Eine bevorzugte Affinity ist ein weiches Ziel.

Beispiel:

```text
Preferred NUMA Node = 1
```

Ist dort keine geeignete CPU verfügbar, kann ein alternativer gültiger Zielprozessor gewählt werden.

Diese Abweichung sollte introspektierbar sein.

---

## Affinity Inheritance

Mehrere Interruptquellen desselben Geräts dürfen gemeinsame Affinity-Präferenzen erben.

Einzelne Queues können diese anschließend weiter spezialisieren.

Beispiel:

```text
NIC
Preferred Node 1
    │
    ├── Queue 0 → CPU 8
    ├── Queue 1 → CPU 9
    ├── Queue 2 → CPU 10
    └── Queue 3 → CPU 11
```

---

## Shared Interrupts

Legacy Shared Interrupts erschweren eindeutige Geräte-Affinity.

Wenn mehrere Geräte dieselbe Hardwareinterruptquelle teilen, muss eine gemeinsame gültige Zielmenge verwendet werden.

Die Policy darf nicht so tun, als ließen sich solche Geräte unabhängig routen.

MSI/MSI-X soll bevorzugt werden, wenn dadurch eine bessere Trennung möglich ist und die Plattform dies unterstützt.

---

## SMT Awareness

Hardware Threads desselben physischen Cores sind nicht gleichbedeutend mit unabhängigen Cores.

Die Policy kann deshalb zwischen:

```text
same hardware thread
same core
same cache domain
same NUMA node
different NUMA node
```

unterscheiden.

Dadurch lässt sich beispielsweise vermeiden, mehrere sehr interruptlastige Queues unnötig auf SMT-Geschwister zu konzentrieren.

---

## Power Management

Energiepolitik darf Interrupt Affinity beeinflussen.

Beispielsweise kann ein energiesparender Modus Interrupts stärker auf bereits aktive CPUs konzentrieren.

Ein Performance-Modus kann sie stärker verteilen.

Auch hier gilt:

```text
energy policy
<
hard realtime / required affinity / safety
```

---

## Device Hotplug

Beim Hinzufügen eines Geräts wird dessen initiale Interrupt Affinity anhand der aktuellen Topologie bestimmt.

Beim Entfernen werden die zugehörigen Affinity- und Routingressourcen freigegeben.

Die Entscheidung arbeitet mit der aktuellen Hardware-Graph-Generation.

---

## Topology Changes

Ändert sich die Systemtopologie, können bestehende Affinityentscheidungen ungültig oder suboptimal werden.

Beispiele:

- CPU Hotplug,
- Memory Hotplug,
- PCIe Hotplug,
- virtuelle CPU-Neukonfiguration.

NovaOS muss solche Änderungen erkennen können.

Nicht jede Topologieänderung erzwingt sofortiges Rebalancing.

---

## Hardware Graph Generation

Affinityentscheidungen sollen die verwendete Hardware-Graph-Generation referenzieren können.

Beispiel:

```text
Decision:
    IRQ 52 → CPU 8

TopologyGeneration:
    103
```

Damit kann erkannt werden, ob eine Entscheidung auf veralteter Topologie basiert.

---

## Interrupt Affinity Decision Record

Routingentscheidungen sollen strukturiert introspektierbar sein.

Konzeptionell:

```text
InterruptAffinityDecision {
    source
    selectedTarget
    allowedTargets
    preferredTargets
    topologyGeneration
    reason
}
```

Beispiel:

```text
Selected CPU:
    9

Reason:
    Device-local NUMA node
    Lower IRQ load

Rejected CPU 8:
    Higher current load
```

Dies entspricht der Architecture-Introspection-Philosophie von NovaOS.

---

## Prediction Error

Adaptive Affinity Policies dürfen ihre Kostenmodelle anhand beobachteter Ergebnisse verbessern.

Beispiel:

```text
Predicted:
    CPU 8 lower latency

Observed:
    CPU 8 higher latency

Prediction Error
    ↓
Cost model adjustment
```

Prediction Error darf lediglich Optimierungsmodelle beeinflussen.

Hard Constraints bleiben unverändert.

---

## KI-Unabhängigkeit

Interrupt Affinity benötigt keine KI.

Die Baseline muss über deterministische:

- Topologieregeln,
- Constraints,
- Kostenmodelle,
- Schedulerinformationen

funktionieren.

KI darf niemals erforderlich sein, um einen gültigen Interruptpfad herzustellen.

---

## Fehlerfälle

Typisierte Affinityfehler können sein:

```text
NoValidTarget
RequiredCpuUnavailable
UnsupportedRouting
TopologyUnavailable
AffinityConflict
```

Bei Preferred-Affinity-Problemen darf ein gültiger Fallback verwendet werden.

Bei Required-Affinity-Konflikten muss die Policy den Fehler explizit behandeln.

---

## Fallback bei unbekannter Topologie

Wenn keine zuverlässige Topologieinformation verfügbar ist, muss NovaOS weiterhin Interrupts routen können.

Dann kann beispielsweise eine neutrale Policy verwendet werden:

```text
available CPU
+
load balancing
```

Unbekannte Topologie darf nicht zur Annahme falscher Lokalität führen.

---

## Security

Nicht privilegierte Anwendungen dürfen Interrupt Affinity nicht beliebig verändern.

Treiber dürfen Präferenzen für ihre eigenen Interruptressourcen anmelden, jedoch nur innerhalb der ihnen zugewiesenen Autorität.

Die endgültige Entscheidung verbleibt beim System.

---

## Normative Anforderungen

1. NovaOS MUSS Interrupt Affinity als explizite Systementscheidung modellieren.
2. Interrupt Routing Policy und Interrupt-Controller-Mechanismus MÜSSEN getrennt bleiben.
3. Affinity MUSS CPU Sets unterstützen können.
4. Required und Preferred Affinity MÜSSEN unterscheidbar sein.
5. Required Affinity DARF NICHT stillschweigend verletzt werden.
6. Preferred Affinity DARF bei Bedarf auf ein anderes gültiges Ziel zurückfallen.
7. Die Affinity Policy MUSS den Hardware System Graph berücksichtigen können.
8. NUMA-Lokalität MUSS als Routingkriterium verfügbar sein.
9. Device-, Queue- und Worker-Lokalität SOLLEN gemeinsam optimierbar sein.
10. Queue-basierte Geräte MÜSSEN individuelle Interrupt-Affinity unterstützen können.
11. MSI/MSI-X MÜSSEN topology-aware geroutet werden können.
12. Scheduler und Interrupt Subsystem MÜSSEN relevante Affinityinformationen austauschen können.
13. CPU Hotplug MUSS eine Neubewertung ungültiger Interruptziele auslösen.
14. Interrupts DÜRFEN NICHT auf offline CPUs geroutet bleiben.
15. Dynamisches Affinity Rebalancing MUSS möglich sein.
16. Rebalancing SOLL unnötiges Routing-Churn vermeiden.
17. Echtzeit- und CPU-Isolation-Constraints MÜSSEN gegenüber reinen Performancepräferenzen Vorrang besitzen.
18. Deterministic Mode MUSS festes Interrupt Routing unterstützen können.
19. Erzwungene Affinity MUSS explizit als Hard Constraint behandelt werden.
20. Shared Interrupts MÜSSEN ihre tatsächlichen gemeinsamen Routinggrenzen respektieren.
21. SMT- und Cachetopologie SOLLEN als zusätzliche Locality-Kriterien verwendbar sein.
22. Energie- und Lastpolitik DÜRFEN Affinity beeinflussen, solange stärkere Constraints eingehalten werden.
23. Device Hotplug MUSS Affinityressourcen korrekt erzeugen und freigeben.
24. Affinityentscheidungen SOLLEN die verwendete Topologie-Generation referenzieren können.
25. Routingentscheidungen und deren Gründe MÜSSEN introspektierbar sein.
26. Affinityfehler MÜSSEN typisiert behandelt werden können.
27. Unbekannte Topologie DARF NICHT als falsche bekannte Lokalität interpretiert werden.
28. Die Interruptversorgung MUSS auch bei fehlender detaillierter Topologie funktionieren.
29. Nicht privilegierte Anwendungen DÜRFEN globale Interrupt Affinity NICHT beliebig verändern.
30. Die grundlegende Affinity-Entscheidung MUSS vollständig ohne KI funktionieren.

---

## Konsequenzen

### Positive Konsequenzen

- bessere NUMA- und Cache-Lokalität,
- geringere unnötige Cross-CPU-Datenbewegung,
- effizientere Nutzung von Multi-Queue-Geräten,
- bessere Integration von Interrupts und Scheduler,
- explizite Unterstützung für Echtzeit und CPU Isolation,
- nachvollziehbare Routingentscheidungen,
- dynamische Anpassung an CPU- und Device-Hotplug,
- bessere Skalierbarkeit auf großen SMP-Systemen.

### Negative Konsequenzen

- Affinity Policy benötigt aktuelle Topologie- und Lastinformationen,
- dynamisches Rebalancing erhöht die Komplexität,
- falsch gewichtete Optimierungsmodelle können Performance verschlechtern,
- Shared Legacy Interrupts begrenzen die erreichbare Granularität.

---

## Verworfene Alternativen

### Alle Interrupts auf CPU 0

Verworfen.

Dies skaliert schlecht und erzeugt unnötige Last- und Locality-Probleme.

### Round-Robin ohne Topologie

Verworfen.

Eine gleichmäßige Verteilung berücksichtigt weder NUMA noch Device- oder Worker-Lokalität.

### Interrupt Controller entscheidet selbst über Ziel-CPU

Verworfen.

Der Controller besitzt nicht den vollständigen Systemkontext für Scheduler-, NUMA-, Realtime- und Resource-Policy.

### Immer nächstgelegene CPU verwenden

Verworfen.

Lokalität ist wichtig, aber nicht stärker als alle anderen Systemconstraints.

### Permanente aggressive Neuverteilung

Verworfen.

Routingänderungen verursachen selbst Kosten und können Cache-Lokalität destabilisieren.

### Treiber kontrollieren Affinity vollständig selbst

Verworfen.

Globale Ressourcen-, Isolation-, Realtime- und Topologieziele benötigen zentrale Koordination.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-INTERRUPT-0001_Einheitliche_Interrupt_Controller_Abstraktion`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-INTERRUPT-AFFINITY-0001`
- `NPSPEC-INTERRUPT-TOPOLOGY-0001`
- `NPSPEC-INTERRUPT-ROUTING-POLICY-0001`
- `NPSPEC-INTERRUPT-CPUSET-0001`
- `NPSPEC-INTERRUPT-NUMA-0001`
- `NPSPEC-INTERRUPT-QUEUE-AFFINITY-0001`
- `NPSPEC-INTERRUPT-LOAD-BALANCE-0001`
- `NPSPEC-INTERRUPT-REALTIME-AFFINITY-0001`
- `NPSPEC-INTERRUPT-DETERMINISTIC-AFFINITY-0001`
- `NPSPEC-INTERRUPT-HOTPLUG-AFFINITY-0001`
- `NPSPEC-INTERRUPT-AFFINITY-INTROSPECTION-0001`
- `NPSPEC-INTERRUPT-AFFINITY-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Device und CPU auf demselben NUMA Node,
- Device und CPU auf unterschiedlichen NUMA Nodes,
- Required CPU Set,
- Preferred CPU Set,
- nicht verfügbare Required CPU,
- Fallback bei Preferred Affinity,
- Multi-Queue-NIC,
- Multi-Queue-NVMe,
- MSI-X mit unterschiedlichen Ziel-CPUs,
- Worker- und IRQ-Lokalität,
- CPU Hotplug Offline,
- CPU Hotplug Online,
- dynamisches Rebalancing,
- Rebalancing-Hysterese,
- CPU Isolation,
- Realtime-Affinity,
- Deterministic Mode mit festem Routing,
- Shared Legacy Interrupt,
- SMT-aware Verteilung,
- unbekannte Topologie,
- veraltete Hardware-Graph-Generation,
- typisierter Affinitykonflikt,
- Architecture-Introspection der Routingentscheidung.

---

## Ergebnis

NovaOS behandelt Interrupt Affinity als topology-aware Policy oberhalb der eigentlichen Interrupt-Controller-Hardware.

Das Architekturmodell lautet:

```text
Interrupt Source
      ↓
Constraints
      ↓
Topology + NUMA + Load
      ↓
Affinity Policy
      ↓
Target CPU Set
      ↓
Interrupt Controller
```

Damit wird die Ziel-CPU nicht durch starre Controllerregeln bestimmt, sondern anhand des aktuellen Systemkontexts ausgewählt.

Die zentrale Architekturregel lautet:

```text
Route interrupts where they make sense.

Prefer locality.

Respect hard constraints.

Keep topology in the policy,
not in the controller.
```