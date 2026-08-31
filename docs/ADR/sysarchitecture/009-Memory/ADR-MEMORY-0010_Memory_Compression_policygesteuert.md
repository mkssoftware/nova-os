# ADR-MEMORY-0010 – Memory Compression policygesteuert

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Compression / Memory Pressure / Resource Economy

## Kontext

Memory Compression kann physischen Speicher einsparen, indem selten oder aktuell nicht aktiv genutzte Pages komprimiert im RAM gehalten werden.

Statt eine Page unmittelbar auf ein langsameres Backing auszulagern oder Speicher freizugeben, kann NovaOS beispielsweise:

```text
Uncompressed Page
      ↓
Compression
      ↓
Compressed Memory Store
```

verwenden.

Bei erneutem Zugriff:

```text
Compressed Page
      ↓
Decompression
      ↓
Resident Page
```

Dies kann insbesondere bei moderatem Memory Pressure sinnvoll sein, da komprimierter RAM oft deutlich geringere Zugriffszeiten als Storage-basiertes Paging besitzt.

Memory Compression ist jedoch nicht kostenlos.

Sie benötigt:

- CPU-Zeit,
- zusätzliche Speicherbandbreite,
- Metadaten,
- Kompressions- und Dekompressionslatenz,
- gegebenenfalls temporären Arbeitsspeicher.

Je nach Workload können diese Kosten den Nutzen übersteigen.

Beispielsweise sind bereits komprimierte oder stark entropische Daten häufig schlechte Kandidaten.

NovaOS behandelt Memory Compression deshalb nicht als festen Bestandteil jeder Speicherallokation, sondern als policygesteuerte Optimierung innerhalb der systemweiten Memory-Pressure- und Resource-Economy-Architektur.

---

## Entscheidung

NovaOS unterstützt **Memory Compression als optionale, policygesteuerte Speicheroptimierung**.

Memory Compression wird vor allem als Reaktion auf Memory Pressure oder als vorbereitende Optimierung für geeignete inaktive Pages eingesetzt.

Die grundlegende Entscheidungsfolge lautet:

```text
Memory State
    ↓
Reclaim Candidate
    ↓
Eligibility Check
    ↓
Cost / Benefit Evaluation
    ↓
Keep Resident
or
Compress
or
Reclaim
or
Use Backing Store
```

Compression darf keine Hard Constraints, Realtime-Garantien, Security- oder Data-Sovereignty-Regeln umgehen.

---

## Grundprinzip

```text
Compression trades CPU for memory.

Use it only when that trade is beneficial.

Compressed memory remains a managed resource.

Never make correctness depend on compression.
```

---

## Compressed Memory Store

Komprimierte Pages werden in einem explizit verwalteten **Compressed Memory Store** gehalten.

Konzeptionell:

```text
Compressed Memory Store
    ├── Compressed Page A
    ├── Compressed Page B
    ├── Metadata
    └── Ownership / Accounting
```

Der Store ist Bestandteil des Memory-Management-Subsystems und keine normale unkontrollierte Anwendungscache-Struktur.

Er muss:

- begrenzbar,
- überwacht,
- accounted,
- reclaimbar

sein.

---

## Page-Zustände

Memory Compression erweitert den möglichen Lebenszyklus einer Page.

```text
Resident
   ↓
Compressed
   ↓
Resident
```

oder:

```text
Resident
   ↓
Compressed
   ↓
Reclaimed / Backed
```

Mindestens müssen folgende Zustände unterscheidbar sein:

```text
Resident Uncompressed
Compressed
Non-resident Backed
Discarded / Reconstructable
```

Die konkrete Zustandsmaschine wird in den zugehörigen NPSPECs definiert.

---

## Kandidatenauswahl

Nicht jede Page ist für Compression geeignet.

Bevorzugte Kandidaten können beispielsweise sein:

- aktuell inaktive anonyme Pages,
- selten verwendete private Daten,
- rekonstruierbare, aber teuer neu erzeugbare Daten,
- Pages mit erwartbar gutem Kompressionsverhältnis.

Schlechte Kandidaten können sein:

- aktive Working-Set-Pages,
- bereits komprimierte Daten,
- verschlüsselte beziehungsweise hohe Entropie aufweisende Daten,
- gepinnte Pages,
- Hard-Realtime-Memory,
- Device-gebundene Pages.

Die genaue Auswahl bleibt Policy.

---

## Kostenmodell

Die Compression-Policy bewertet mindestens zwei grundlegende Größen:

```text
Memory Benefit
vs.
Compute / Latency Cost
```

Relevant können sein:

- erwartete Kompressionsrate,
- Page-Größe,
- Zugriffshäufigkeit,
- Zeit seit letztem Zugriff,
- CPU-Auslastung,
- Memory Pressure,
- Speicherbandbreite,
- Energieverbrauch,
- NUMA Locality,
- erwartete Wiederverwendung.

Eine hohe Kompressionsrate allein reicht nicht aus, wenn die benötigte CPU-Zeit für die aktuelle Workload zu teuer ist.

---

## Algorithmusauswahl

Der konkrete Kompressionsalgorithmus ist keine feste Architekturentscheidung dieser ADR.

NovaOS darf entsprechend dem allgemeinen Algorithmusmodell automatisch einen geeigneten Algorithmus auswählen.

Konzeptionell:

```text
Compress(page)
```

bedeutet:

```text
Page characteristics
+ Hardware
+ Latency target
+ CPU budget
+ Memory pressure
        ↓
Algorithm selection
```

Ein geeigneter Execution Contract darf eine Präferenz oder einen konkreten Algorithmus vorgeben, solange keine Hard Constraints verletzt werden.

---

## Algorithmusanforderungen

Kompressionsalgorithmen können unterschiedliche Ziele optimieren:

```text
Fast compression
Fast decompression
High compression ratio
Low memory overhead
Low energy cost
```

NovaOS darf deshalb unterschiedliche Algorithmen beziehungsweise Implementierungen für unterschiedliche Workloads einsetzen.

Die Semantik des Compressed Memory Store darf jedoch nicht von einem bestimmten Algorithmus abhängen.

---

## Memory Pressure

Memory Compression ist eng mit `ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal` verbunden.

Typische Policy:

```text
Normal
    ↓
Compression mostly unnecessary

Elevated
    ↓
Compress good inactive candidates

Constrained
    ↓
More aggressive compression

Critical
    ↓
Compression + reclaim + OOM recovery
```

Diese Abfolge ist keine starre globale Schwellenlogik.

Die tatsächlichen Aktionen werden durch die jeweilige Resource- und Pressure-Policy bestimmt.

---

## Compression ist kein Reclaim

Eine komprimierte Page belegt weiterhin physischen Speicher.

Daher gilt:

```text
Compressed != Reclaimed
```

Compression reduziert lediglich die Größe des belegten Backings.

Beispiel:

```text
4 KiB Page
   ↓
1.2 KiB compressed
```

belegt weiterhin etwa 1.2 KiB plus Metadaten.

Das Pressure-Modell muss diese Ressource weiterhin berücksichtigen.

---

## Resource Accounting

Der Compressed Memory Store muss vollständig in die Resource Economy integriert sein.

Mindestens müssen unterschieden werden können:

```text
Original Logical Size
Compressed Physical Size
Compression Metadata
Owner / Execution Domain
Shared Cost
```

Eine Domain darf Memory Budgets nicht umgehen, indem ihre Pages lediglich in den Compressed Store verschoben werden.

Die tatsächliche komprimierte Belegung bleibt ihrer Ressourcenverantwortung zurechenbar.

---

## Domain-Local Pressure

Compression soll bevorzugt innerhalb der verursachenden Resource beziehungsweise Execution Domain wirken.

Beispiel:

```text
Domain A exceeds soft memory budget
        ↓
Compress suitable pages of Domain A
```

statt zunächst Speicher anderer unabhängiger Domains zu komprimieren.

Systemweite Policies dürfen bei globalem Pressure zusätzliche Kandidaten auswählen, müssen jedoch Ownership und Reservations berücksichtigen.

---

## Shared Memory

Shared Memory Objects können nur dann komprimiert werden, wenn ihre Semantik dies erlaubt.

Eine Page darf insbesondere nicht einfach aus einem gemeinsam zugänglichen Mapping entfernt und komprimiert werden, während andere Teilnehmer weiterhin direkten Zugriff erwarten.

Mögliche Strategien sind:

- Shared Object vollständig nicht komprimierbar,
- koordinierte Unmap-/Fault-Semantik,
- komprimierbares immutable Backing,
- private COW-Ableitungen komprimieren.

Die genaue Policy hängt vom Shared Object Contract ab.

---

## Copy-On-Write

COW Pages können grundsätzlich Compression-Kandidaten sein.

Gemeinsam verwendete COW-Backings sollten jedoch nicht unnötig mehrfach komprimiert werden.

Die Objekt- und Ownership-Semantik muss erhalten bleiben.

Nach einer COW-Trennung können private Pages unabhängig bewertet werden.

---

## Demand Paging

Eine komprimierte Page kann bei Zugriff einen speziellen Memory Fault auslösen.

```text
Access
  ↓
Compressed Mapping
  ↓
Fault
  ↓
Allocate destination page
  ↓
Decompress
  ↓
Install resident mapping
  ↓
Resume
```

Dieser Fault muss von klassischen Storage-backed Page Faults unterscheidbar sein.

---

## Dekompressionspfad

Der Dekompressionspfad muss robust und begrenzt sein.

Dekompression benötigt typischerweise:

- Zielseite,
- CPU-Zeit,
- Zugriff auf den Compressed Store.

Kann keine Zielseite bereitgestellt werden, muss die Situation über das normale Memory-Pressure- beziehungsweise OOM-Modell behandelt werden.

Ein komprimierter Zustand darf nicht zu einer nicht auflösbaren versteckten Ressourcenabhängigkeit führen.

---

## NUMA

Compressed Memory Stores müssen NUMA-aware verwaltet werden können.

Beispielsweise:

```text
NUMA Node 0
   └── Local Compressed Store

NUMA Node 1
   └── Local Compressed Store
```

Dies reduziert unnötige Remote-Memory-Zugriffe bei Compression und Decompression.

Eine Page kann bei Dekompression entsprechend ihrer aktuellen Placement Policy auch auf einem anderen NUMA Node materialisiert werden.

Required-NUMA-Constraints bleiben verbindlich.

---

## Realtime

Hard-Realtime-Memory darf nicht unkontrolliert komprimiert werden.

Dekompression erzeugt variable Latenz und möglicherweise zusätzliche Allokationen.

Für Hard-Realtime-Bereiche gilt deshalb grundsätzlich:

```text
Critical Working Set
      ↓
Resident / Reserved
      ↓
No policy-driven compression
```

Eine explizite Realtime-Policy kann bestimmte nichtkritische Speicherbereiche innerhalb derselben Domain dennoch als komprimierbar markieren.

---

## Deterministic Mode

Adaptive Compression kann Ausführungszeit und Page-Fault-Verhalten verändern.

Deterministic Mode muss deshalb:

- Compression deaktivieren,
- feste Kandidaten definieren,
- feste Algorithmen verwenden,
- oder reproduzierbare Compression-Policies vorgeben

können.

Deterministische Korrektheit darf nicht davon abhängen, ob eine Page komprimiert wurde.

---

## CPU- und Energieökonomie

Compression spart Speicher auf Kosten von Compute.

Daher muss die Resource Economy auch den CPU- und Energieverbrauch berücksichtigen.

Beispiel:

```text
Memory Pressure high
CPU Pressure low
    ↓
Compression attractive
```

gegenüber:

```text
Memory Pressure moderate
CPU saturated
    ↓
Compression may be harmful
```

Die Optimierung darf nicht nur auf die Maximierung freier Bytes ausgerichtet sein.

---

## Hardwarebeschleunigung

Wenn die Plattform geeignete Hardwarebeschleuniger für Compression bereitstellt, darf NovaOS diese verwenden.

Die Nutzung muss über die normale Hardware-/Accelerator-Abstraktion erfolgen.

Der Memory Manager darf nicht von einer spezifischen Hardwarekompressionsfunktion abhängig werden.

---

## Security

Komprimierter Speicher enthält weiterhin Daten der jeweiligen Execution Domain.

Der Compressed Memory Store muss deshalb denselben Security- und Isolationseigenschaften unterliegen wie anderes physisches Backing.

Insbesondere dürfen:

- Daten verschiedener Domains nicht unkontrolliert offengelegt werden,
- freigegebene komprimierte Blöcke keine Restdaten sichtbar machen,
- Metadaten keine unautorisierten Informationsflüsse erzeugen.

---

## Data Sovereignty

Compression darf den zulässigen Speicherort von Daten nicht verändern.

Wenn Daten beispielsweise nur innerhalb einer bestimmten Memory Domain gespeichert werden dürfen, gilt dies ebenfalls für ihre komprimierte Darstellung.

```text
Data Sovereignty Constraint
          ↓
Compressed representation
must remain compliant
```

Compression darf nicht als Umweg genutzt werden, um Placement-Regeln zu umgehen.

---

## Verschlüsselte Daten

Bereits verschlüsselte Daten besitzen typischerweise eine geringe Komprimierbarkeit.

NovaOS darf solche Pages entsprechend schlechter priorisieren oder ganz von Compression ausschließen.

Security-Architektur und Verschlüsselung dürfen jedoch nicht abgeschwächt werden, nur um bessere Kompressionsraten zu erreichen.

---

## Zero-Copy

Aktiv verwendete Zero-Copy-Buffers sind normalerweise schlechte Compression-Kandidaten, insbesondere wenn:

- Devices direkt auf sie zugreifen,
- Pages gepinnt sind,
- DMA-Mappings bestehen.

Vor einer möglichen Compression müssen entsprechende Zugriffe beendet und die erforderlichen Ownership- und Mapping-Übergänge abgeschlossen sein.

---

## Huge Pages

Huge Pages können für Compression problematisch sein.

Eine Policy darf entscheiden:

```text
Compress whole huge page
```

oder:

```text
Split huge page
      ↓
Compress selected base pages
```

Ein Split ist sinnvoll, wenn nur ein kleiner Teil der Huge Page inaktiv ist.

Die Entscheidung muss Kosten für:

- Split,
- TLB,
- Fragmentierung,
- Compression

gegeneinander abwägen.

---

## Hotplug

Compressed Memory darf Memory Hotplug nicht verhindern, ohne dass dies sichtbar wird.

Beim Draining einer Memory Domain müssen komprimierte Blöcke:

- dekomprimiert und migriert,
- direkt in einen anderen Compressed Store verschoben,
- oder anderweitig kontrolliert evakuiert

werden können.

Nicht migrierbare Daten müssen den Hotplug kontrolliert blockieren.

---

## Adaptive Policy

NovaOS darf Compression anhand realer Ergebnisse adaptiv steuern.

Beispiel:

```text
Prediction:
Page likely compressible and inactive

        ↓

Actual:
Low ratio + immediate reuse

        ↓

Prediction Error

        ↓

Reduce future compression priority
```

Mögliche Lernsignale sind:

- tatsächliche Kompressionsrate,
- Zeit bis zur Dekompression,
- CPU-Kosten,
- vermiedene Storage-I/O,
- zusätzlicher Memory Pressure.

Diese Anpassungen bleiben Soft Policy.

---

## Failure Handling

Schlägt Compression fehl, bleibt die ursprüngliche Page gültig, solange sie nicht bereits sicher ersetzt wurde.

Konzeptionell:

```text
Resident Page
    ↓
Attempt Compression
    ↓
Failure
    ↓
Keep Resident
```

Eine Page darf erst dann aus ihrem bisherigen gültigen Zustand entfernt werden, wenn das komprimierte Backing vollständig erstellt und veröffentlicht wurde.

Dekompressionsfehler eines gültigen Compressed Objects sind als schwerwiegende Integritätsfehler zu behandeln.

---

## Architecture Introspection

Memory Compression muss kontrolliert introspektierbar sein.

Relevant sind insbesondere:

```text
Compressed logical bytes
Compressed physical bytes
Compression ratio
Compressed page count
Compression rate
Decompression rate
Compression CPU cost
Decompression latency
Store utilization
Compression failures
Algorithm usage
NUMA distribution
Owner / Domain
```

Damit kann die Policy beurteilen, ob Compression tatsächlich einen positiven Gesamteffekt besitzt.

---

## Normative Anforderungen

1. NovaOS MUSS Memory Compression als optionalen Memory-Management-Mechanismus unterstützen können.
2. Memory Compression MUSS policygesteuert und nicht universell erzwungen werden.
3. Komprimierte Pages MÜSSEN in einem explizit verwalteten Compressed Memory Store gehalten werden.
4. Compressed Memory MUSS weiterhin als physisch belegte Ressource accounted werden.
5. Compression DARF Memory Budgets und Domain Limits NICHT umgehen.
6. Kandidatenauswahl MUSS aktive, gepinnte und andere ungeeignete Speicherbereiche ausschließen können.
7. Compression MUSS mit dem systemweiten Memory-Pressure-Modell integriert sein.
8. Compression MUSS von vollständigem Reclaim semantisch unterschieden werden.
9. Der konkrete Kompressionsalgorithmus DARF policy- beziehungsweise laufzeitabhängig gewählt werden.
10. Die Memory-Semantik DARF NICHT von einem bestimmten Kompressionsalgorithmus abhängen.
11. Komprimierte Pages MÜSSEN ihrer verantwortlichen Resource Domain zurechenbar bleiben.
12. Domain-lokale Pressure-Recovery SOLL bevorzugt Speicher der verursachenden Domain berücksichtigen.
13. Shared Memory DARF nur komprimiert werden, wenn dessen expliziter Contract dies erlaubt.
14. Compression MUSS mit Copy-On-Write und Demand Paging integrierbar sein.
15. Zugriffe auf komprimierte Pages MÜSSEN einen kontrollierten Dekompressionspfad besitzen.
16. Dekompressionsfehler aufgrund fehlender Memory-Ressourcen MÜSSEN in Pressure-/OOM-Handling integrierbar sein.
17. Compressed Memory Stores MÜSSEN NUMA-aware implementierbar sein.
18. Required-NUMA- und Data-Sovereignty-Constraints MÜSSEN auch für komprimiertes Backing gelten.
19. Hard-Realtime-Memory DARF NICHT unkontrolliert policygesteuert komprimiert werden.
20. Deterministic Mode MUSS adaptive Compression kontrollieren oder deaktivieren können.
21. CPU-, Latenz-, Energie- und Memory-Kosten MÜSSEN in Compression-Policies berücksichtigt werden können.
22. Hardwarebeschleunigte Compression DARF verwendet werden, ohne eine architekturelle Abhängigkeit zu erzeugen.
23. Compressed Memory MUSS denselben Isolation- und Security-Anforderungen wie unkomprimiertes Backing unterliegen.
24. Aktive gepinnte Zero-Copy-/DMA-Buffers DÜRFEN NICHT ohne vorherigen kontrollierten Übergang komprimiert werden.
25. Huge Pages MÜSSEN vor oder während Compression policygesteuert gesplittet werden können.
26. Memory Hotplug MUSS komprimierten Speicher kontrolliert migrieren oder den Vorgang blockieren können.
27. Ein fehlgeschlagener Compression-Versuch DARF die gültige Ursprungsseite NICHT verlieren.
28. Compression Ratio, Kosten, Nutzung und Fehler MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- physischer RAM kann effektiver genutzt werden,
- moderater Memory Pressure kann ohne unmittelbares Storage-Paging abgefangen werden,
- inaktive Working Sets können mit niedrigerer physischer Belegung erhalten bleiben,
- NovaOS kann CPU-, Memory- und Energiezustand gemeinsam optimieren,
- unterschiedliche Algorithmen können passend zur Workload gewählt werden,
- adaptive Policies können schlechte Compression-Kandidaten mit der Zeit vermeiden.

### Negative Konsequenzen

- Compression verbraucht CPU-Zeit und Speicherbandbreite,
- Dekompression erhöht die Zugriffslatenz,
- Resource Accounting und Page-State-Management werden komplexer,
- schlechte Kandidaten können mehr Kosten als Nutzen verursachen,
- Shared Memory, Huge Pages, Realtime und DMA benötigen zusätzliche Einschränkungen,
- ein großer Compressed Store kann selbst zum Memory-Pressure-Faktor werden.

---

## Verworfene Alternativen

### Alle inaktiven Pages automatisch komprimieren

Verworfen.

Viele Pages sind schlecht komprimierbar oder werden kurzfristig wieder benötigt.

### Memory Compression grundsätzlich deaktivieren

Verworfen.

Dies würde eine wertvolle Zwischenstufe zwischen Resident Memory und teurerem Reclaim beziehungsweise Storage-basiertem Paging ausschließen.

### Compression als kostenloses Reclaim behandeln

Verworfen.

Komprimierte Daten belegen weiterhin physischen Speicher und benötigen zusätzliche CPU-Ressourcen.

### Einen festen Kompressionsalgorithmus systemweit verwenden

Verworfen.

Unterschiedliche Algorithmen besitzen unterschiedliche Kostenprofile und eignen sich für unterschiedliche Workloads und Hardware.

### Hard-Realtime-Working-Sets transparent komprimieren

Verworfen.

Unvorhersehbare Dekompressions- und Allokationslatenz widerspricht harten zeitlichen Garantien.

### Memory Budgets nach Compression nicht mehr berücksichtigen

Verworfen.

Dies würde Resource Accounting und Domain-Isolation umgehen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0001_Hierarchische_Physical_Memory_Architecture`
- `ADR-MEMORY-0002_NUMA_Aware_Physical_Memory_Allocation`
- `ADR-MEMORY-0003_Memory_Pressure_als_systemweites_Signal`
- `ADR-MEMORY-0004_Domain_Local_Out_Of_Memory_Handling`
- `ADR-MEMORY-0005_Virtual_Address_Spaces_pro_Execution_Domain`
- `ADR-MEMORY-0006_Demand_Paging_und_Lazy_Allocation`
- `ADR-MEMORY-0007_Copy_On_Write_als_gezielte_Optimierung`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`
- `ADR-MEMORY-0009_Huge_Pages_policygesteuert`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0018_Locality_als_Optimierungsziel`
- `ADR-CONCURRENCY-0007_Explizite_Realtime_Concurrency_Domänen`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-COMPRESSION-0001`
- `NPSPEC-MEMORY-COMPRESSED-STORE-0001`
- `NPSPEC-MEMORY-COMPRESSION-POLICY-0001`
- `NPSPEC-MEMORY-COMPRESSION-ALGORITHM-0001`
- `NPSPEC-MEMORY-COMPRESSION-PRESSURE-0001`
- `NPSPEC-MEMORY-COMPRESSION-NUMA-0001`
- `NPSPEC-MEMORY-COMPRESSION-REALTIME-0001`
- `NPSPEC-MEMORY-COMPRESSION-TEST-0001`

---

## Ergebnis

NovaOS integriert Memory Compression als optionale Zwischenstufe innerhalb seines Memory-Pressure-Modells:

```text
Resident Memory
      ↓
Pressure / Policy
      ↓
Candidate Evaluation
      ↓
 ┌────┼─────────────┐
 ↓    ↓             ↓
Keep Compress     Reclaim
      ↓
Compressed Store
      ↓
Later Access
      ↓
Decompress
      ↓
Resident
```

Dabei werden nicht nur eingesparte Bytes, sondern auch CPU-, Latenz-, NUMA-, Energie- und Resource-Economy-Kosten berücksichtigt.

Die zentrale Architekturregel lautet:

```text
Compression is a trade,
not free memory.

Compress when memory saved
is worth the compute cost.

Keep policy above mechanism.

Preserve all hard constraints.
```