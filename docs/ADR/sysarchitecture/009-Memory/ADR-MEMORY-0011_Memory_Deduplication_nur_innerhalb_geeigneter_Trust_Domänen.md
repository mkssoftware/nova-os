# ADR-MEMORY-0011 – Memory Deduplication nur innerhalb geeigneter Trust-Domänen

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Deduplication / Security / Trust / Resource Economy

## Kontext

Memory Deduplication kann identische Speicherinhalte erkennen und mehrere physische Kopien durch ein gemeinsames Backing ersetzen.

Beispiel:

```text
Before:

Domain A → Page X
Domain B → Page Y

X == Y
```

Nach erfolgreicher Deduplication:

```text
Domain A ──┐
           ├── Shared Physical Page
Domain B ──┘
```

Schreibzugriffe können anschließend über Copy-On-Write wieder getrennt werden.

Dies kann bei vielen identischen Daten erhebliche Mengen physischen Speichers einsparen, beispielsweise bei:

- identischen Runtime-Daten,
- mehrfach geladenen unveränderten Ressourcen,
- gleichartigen Execution Domains,
- virtuellen beziehungsweise isolierten Workloads,
- großen read-mostly Datensätzen.

Systemweite inhaltsbasierte Deduplication besitzt jedoch erhebliche Sicherheitsrisiken.

Wenn Speicher verschiedener nicht vertrauensgleicher Domains allein aufgrund identischer Inhalte zusammengeführt wird, können beobachtbare Unterschiede bei:

- Page Faults,
- Copy-On-Write,
- Timing,
- Memory Pressure,
- Resource Accounting

zu Seitenkanälen führen.

Ein Angreifer könnte dadurch unter bestimmten Bedingungen ableiten, ob eine andere Domain bestimmte Daten besitzt.

NovaOS darf deshalb Speicherersparnis nicht über Isolation, Security oder Data Sovereignty stellen.

---

## Entscheidung

NovaOS unterstützt **Memory Deduplication ausschließlich als optionale, policygesteuerte Optimierung innerhalb dafür geeigneter Trust-Domänen**.

Es gibt keine standardmäßige globale inhaltsbasierte Deduplication über beliebige Execution Domains hinweg.

Grundmodell:

```text
Memory Pages
    ↓
Trust Boundary Check
    ↓
Deduplication Eligibility
    ↓
Content Comparison
    ↓
Optional Merge
    ↓
Shared Read-only Backing
    ↓
Write → Copy-On-Write
```

Vor jeder Deduplication muss feststehen, dass die beteiligten Speicherobjekte innerhalb einer zulässigen gemeinsamen Deduplication- beziehungsweise Trust-Domäne liegen.

---

## Grundprinzip

```text
Deduplicate only where trust permits.

Never cross security boundaries
for memory efficiency.

Content equality does not imply
sharing authority.
```

---

## Trust-Domäne

Eine Trust-Domäne beschreibt eine Menge von Execution Domains beziehungsweise Memory Objects, zwischen denen Deduplication gemäß Security-, Trust- und Data-Sovereignty-Policy zulässig ist.

Beispiele können sein:

```text
Same Execution Domain
Same Application Instance
Same trusted service group
Explicitly configured sandbox group
Trusted immutable system resources
```

Die konkrete Trust-Domain-Zuordnung wird durch die Nova.Trust- und Security-Policy bestimmt.

---

## Keine globale Deduplication

NovaOS führt keine automatische systemweite Deduplication zwischen beliebigen Domains durch.

Insbesondere gilt:

```text
Same content
    !=
permission to share physical backing
```

Zwei Pages dürfen nur dann zusammengeführt werden, wenn sowohl:

- inhaltliche Gleichheit,
- als auch Sharing-Berechtigung

gegeben sind.

---

## Geeignete Kandidaten

Bevorzugte Kandidaten können sein:

- read-mostly Pages,
- immutable Memory Objects,
- identische Runtime-Ressourcen,
- private Pages innerhalb derselben Trust-Domäne,
- lange unveränderte Pages,
- mehrfach vorhandene unveränderliche Daten.

Schlechte Kandidaten sind insbesondere:

- häufig beschriebene Pages,
- Hard-Realtime-Working-Sets,
- DMA-/Device-gebundene Pages,
- sicherheitskritische Secrets,
- Pages aus nicht kompatiblen Trust-Domänen.

Die Auswahl bleibt Policy.

---

## Deduplication-Verfahren

Die konkrete Erkennung identischer Pages ist kein fester Bestandteil dieser ADR.

Eine Implementierung kann beispielsweise mehrstufig arbeiten:

```text
Candidate Selection
      ↓
Fast Fingerprint
      ↓
Hash Comparison
      ↓
Full Content Verification
      ↓
Merge
```

Ein Hash allein darf nicht als Beweis vollständiger Gleichheit verwendet werden, wenn daraus eine gemeinsame physische Page entsteht.

Vor dem Merge muss die Gleichheit zuverlässig verifiziert werden.

---

## Merge-Semantik

Nach erfolgreicher Deduplication verweisen mehrere Mappings auf dasselbe schreibgeschützte Backing.

```text
Domain A ──┐
           ├── Shared Page
Domain B ──┘
```

Bei einem Schreibzugriff:

```text
Write
 ↓
COW Fault
 ↓
Private Physical Page
 ↓
Writable Mapping
```

Damit bleiben die ursprünglichen privaten Speichersemmantiken erhalten.

---

## Deduplication ist nicht Shared Memory

Deduplication erzeugt keine explizite Shared-Memory-Semantik.

Bei Shared Memory gilt:

```text
Write by A
    ↓
Visible to B
```

Bei deduplizierten privaten Pages gilt:

```text
Write by A
    ↓
COW
    ↓
Not visible to B
```

Deduplication ist daher lediglich eine physische Speicheroptimierung.

Sie darf keine neue logische Kommunikationsbeziehung zwischen Domains erzeugen.

---

## Copy-On-Write

Deduplication basiert für private veränderliche Speicherbereiche auf Copy-On-Write.

`ADR-MEMORY-0007_Copy_On_Write_als_gezielte_Optimierung` bleibt daher verbindlich.

Ein Merge darf nur erfolgen, wenn bei späterer Divergenz eine sichere Trennung möglich ist.

---

## Trust und Security

Deduplication wird durch Nova.Trust kontrolliert.

Relevante Faktoren können sein:

```text
Identity
Signature
Provenance
Permission
Capability
Security Context
Data Sovereignty
```

Trust-Gleichheit darf nicht allein aus gemeinsamer Softwareidentität abgeleitet werden.

Beispielsweise können zwei Instanzen derselben Anwendung unterschiedliche Benutzer- oder Datenkontexte besitzen.

Die konkrete Policy entscheidet, ob physisches Sharing zulässig ist.

---

## Sensitive Memory

Speicher, der als besonders sensitiv klassifiziert ist, soll standardmäßig nicht dedupliziert werden.

Dazu können gehören:

- Schlüsselmaterial,
- Credentials,
- Security Tokens,
- geheime Modellzustände,
- geschützte personenbezogene Daten,
- andere explizit als confidential markierte Memory Objects.

Auch innerhalb derselben Trust-Domäne kann Deduplication für solche Daten deaktiviert werden.

---

## Data Sovereignty

Deduplication darf keine Data-Sovereignty-Grenzen überschreiten.

Wenn zwei identische Pages unterschiedlichen:

- Data Owners,
- Sovereignty Policies,
- Memory Domains,
- Security Policies

unterliegen, dürfen sie nicht allein aufgrund identischer Inhalte zusammengeführt werden.

Ein gemeinsames physisches Backing ist selbst eine relevante Ressourcen- und Informationsflussbeziehung.

---

## Side-Channel-Schutz

Deduplication kann beobachtbare Unterschiede erzeugen.

Beispielsweise:

```text
Write
 ↓
COW occurs?
 ↓
Timing difference
```

NovaOS muss deshalb Deduplication über nicht vertrauensgleiche Sicherheitsgrenzen standardmäßig verhindern.

Innerhalb zulässiger Trust-Domänen muss weiterhin bewertet werden, ob Timing- oder Resource-Accounting-Seitenkanäle akzeptabel sind.

Security besitzt Vorrang vor Memory Savings.

---

## Resource Accounting

Deduplicated Memory muss explizit accounted werden.

Mindestens müssen unterscheidbar sein:

```text
Logical Memory Usage
Unique Physical Backing
Shared Deduplicated Backing
Per-domain References
COW Expansion
```

Physischer Speicher darf nicht mehrfach als exklusiv belegt gezählt werden.

Gleichzeitig darf Deduplication nicht zur Umgehung von Budgets führen.

---

## Einsparungszuordnung

Wenn mehrere Domains eine deduplizierte Page gemeinsam verwenden, muss die Resource Economy eine definierte Kostenverteilung besitzen.

Mögliche Modelle sind beispielsweise:

```text
Owner pays
Equal split
Reference-weighted accounting
System shared pool
```

Diese ADR legt kein konkretes Abrechnungsmodell fest.

Die Policy muss jedoch deterministisch und introspektierbar sein.

---

## Memory Pressure

Deduplication kann unter Memory Pressure verstärkt als Optimierung eingesetzt werden.

Beispiel:

```text
Elevated Pressure
      ↓
Scan eligible cold pages
      ↓
Merge duplicates
```

Dies bleibt jedoch eine Soft Policy.

Memory Pressure darf niemals dazu führen, dass Trust- oder Security-Grenzen abgeschwächt werden.

---

## Deduplication-Kosten

Deduplication verbraucht selbst Ressourcen:

- CPU-Zeit,
- Speicherbandbreite,
- Hash-/Fingerprint-Speicher,
- Page-Scanning,
- Synchronisation,
- COW-Verwaltung.

Daher muss die Policy Nutzen und Kosten gegeneinander abwägen.

```text
Expected memory saved
        vs.
Scanning + hashing + COW cost
```

---

## Adaptive Policy

NovaOS darf aus tatsächlichen Ergebnissen lernen.

Beispiel:

```text
Prediction:
Large duplicate ratio expected

        ↓

Actual:
Few duplicates found

        ↓

Prediction Error

        ↓

Reduce future scan priority
```

Ebenso können hohe COW-Raten darauf hinweisen, dass eine Page-Klasse kein geeigneter Deduplication-Kandidat ist.

Adaptive Entscheidungen bleiben Soft Policy.

---

## NUMA

Deduplication darf NUMA Locality nicht ignorieren.

Wenn identische Pages auf verschiedenen NUMA Nodes liegen, kann ein Merge zwar Speicher sparen, gleichzeitig aber Remote-Memory-Zugriffe erzeugen.

Die Policy muss deshalb abwägen:

```text
Memory Saving
vs.
Locality Cost
```

Ein Merge über NUMA Nodes ist nur zulässig, wenn Hard Placement Constraints dies erlauben.

---

## Realtime

Hard-Realtime-Memory soll standardmäßig nicht durch Hintergrund-Deduplication verändert werden.

Scanning, Merge, TLB-Änderungen und spätere COW Faults können nicht vorhersehbare Latenzen erzeugen.

Realtime-Speicher kann daher explizit als:

```text
Non-deduplicable
```

markiert werden.

Deduplication darf nicht im kritischen Hard-Realtime-Pfad erforderlich sein.

---

## Deterministic Mode

Deterministic Mode muss adaptive oder zeitabhängige Deduplication deaktivieren oder reproduzierbar konfigurieren können.

Mögliche Varianten sind:

```text
Deduplication disabled
```

oder:

```text
Fixed eligible regions
Fixed scan points
Fixed policy
```

Korrektheit darf niemals davon abhängen, ob ein Merge stattgefunden hat.

---

## Huge Pages

Huge Pages können dedupliziert werden, sofern ihre gesamte relevante Einheit identisch und geeignet ist.

Alternativ kann eine Huge Page gesplittet werden, um kleinere identische Bereiche zusammenzuführen.

Die Policy muss zwischen:

```text
Huge Page retention
vs.
Deduplication benefit
```

abwägen.

Ein Split nur für geringe Speicherersparnis kann mehr TLB-Kosten als Nutzen erzeugen.

---

## Memory Compression

Deduplication und Memory Compression sind unterschiedliche Mechanismen.

```text
Deduplication:
N identical pages → 1 backing

Compression:
1 page → smaller representation
```

Die Policy kann beide kombinieren.

Typischerweise sollte jedoch vermieden werden, identische Pages mehrfach unabhängig zu komprimieren, wenn ein zulässiges gemeinsames Backing effizienter wäre.

Trust-Grenzen bleiben dabei verbindlich.

---

## Shared Memory Objects

Explizite Shared Memory Objects müssen nicht durch Deduplication erkannt werden, da Sharing bereits Teil ihrer Semantik ist.

Deduplication kann jedoch für interne immutable Backings oder private Ableitungen verwendet werden.

Sie darf die expliziten Ownership- und Mapping-Beziehungen aus `ADR-MEMORY-0008` nicht ersetzen.

---

## Device- und DMA-Memory

Aktiv für DMA oder Device-Zugriffe gepinnte Pages sind standardmäßig keine Deduplication-Kandidaten.

Ein Merge würde sonst:

- IOMMU-Mappings,
- Device Ownership,
- Coherency,
- Pinning-Semantik

komplizieren.

Deduplication darf erst nach einem kontrollierten Übergang aus solchen Zuständen erfolgen.

---

## Hotplug und Migration

Deduplicated Backing kann von mehreren Domains referenziert werden.

Bei Memory Hotplug oder Migration muss deshalb berücksichtigt werden, dass ein einzelnes physisches Backing mehrere logische Nutzer besitzt.

Das Backing kann:

- gemeinsam migriert,
- aufgespalten,
- oder bei nicht erfüllbaren Constraints dedupliziert zurückgebaut

werden.

Die Sharing-Semantik der privaten Pages muss dabei erhalten bleiben.

---

## Undeduplication

NovaOS muss deduplizierten Speicher wieder trennen können.

Gründe umfassen:

- Schreibzugriff,
- Änderung der Trust Policy,
- Data-Sovereignty-Änderung,
- Migration,
- Realtime-Anforderung,
- Debugging,
- Security Policy.

Die Trennung erfolgt typischerweise über COW beziehungsweise explizite Materialisierung privater Pages.

---

## Trust-Policy-Änderungen

Ändert sich die Trust-Beziehung zwischen zwei Domains, darf bestehende Deduplication nicht unbegrenzt unverändert fortbestehen.

Beispiel:

```text
Domains A + B
trusted for deduplication
        ↓
Policy changes
        ↓
No longer compatible
        ↓
Undeduplicate / isolate
```

Der Übergang muss kontrolliert erfolgen.

Fail-safe bedeutet im Zweifel Isolation.

---

## Failure Handling

Schlägt ein Deduplication-Versuch fehl, bleiben die ursprünglichen Pages unverändert.

```text
Page A + Page B
      ↓
Attempt merge
      ↓
Failure
      ↓
Keep both pages
```

Ein Merge darf erst veröffentlicht werden, wenn:

- Gleichheit bestätigt,
- Trust geprüft,
- Mapping-Updates vorbereitet,
- Lifetime und Accounting konsistent

sind.

---

## Architecture Introspection

Deduplication muss kontrolliert introspektierbar sein.

Relevant sind insbesondere:

```text
Eligible Pages
Scanned Pages
Merged Pages
Saved Physical Bytes
Deduplication Groups
Trust Domain
COW Faults after merge
Undeduplication events
NUMA locality impact
CPU scanning cost
Rejected cross-domain merges
```

Sensitive Inhalte, Hashes oder Fingerprints dürfen nicht unautorisiert offengelegt werden.

---

## Normative Anforderungen

1. NovaOS DARF Memory Deduplication als optionale Speicheroptimierung unterstützen.
2. Deduplication MUSS policygesteuert sein.
3. NovaOS DARF standardmäßig KEINE globale inhaltsbasierte Deduplication über beliebige Trust-Domänen durchführen.
4. Ein Deduplication-Merge MUSS sowohl inhaltliche Gleichheit als auch zulässige Trust-Beziehungen voraussetzen.
5. Inhaltliche Gleichheit DARF NICHT als Sharing-Berechtigung interpretiert werden.
6. Deduplication MUSS innerhalb explizit geeigneter Trust-Domänen begrenzbar sein.
7. Sensitive Memory MUSS von Deduplication ausgeschlossen werden können.
8. Data-Sovereignty- und Security-Constraints MÜSSEN vor jedem Merge geprüft werden.
9. Deduplication DARF keine neue logische Shared-Memory-Kommunikationssemantik erzeugen.
10. Deduplizierte private Pages MÜSSEN bei Schreibzugriff sicher über Copy-On-Write getrennt werden können.
11. Gleichheit MUSS vor einem Merge zuverlässig verifiziert werden; ein Hash allein DARF bei Kollisionsmöglichkeit nicht genügen.
12. Ein fehlgeschlagener Deduplication-Versuch DARF die ursprünglichen Pages NICHT verändern.
13. Deduplicated Backing MUSS mit Ownership-, Lifetime- und Reference-Management integriert sein.
14. Deduplicated Memory MUSS vollständig in das Resource Accounting integriert sein.
15. Deduplication DARF Memory Budgets NICHT umgehen.
16. Memory-Pressure-Policies DÜRFEN Deduplication aggressiver einsetzen, aber keine Trust-Grenzen abschwächen.
17. Deduplication-Policies MÜSSEN CPU-, Bandbreiten- und Scan-Kosten berücksichtigen können.
18. NUMA Locality MUSS bei Cross-NUMA-Deduplication berücksichtigt werden.
19. Required-NUMA-Constraints DÜRFEN durch Deduplication NICHT verletzt werden.
20. Hard-Realtime-Memory MUSS von automatischer Deduplication ausgeschlossen werden können.
21. Deterministic Mode MUSS adaptive Deduplication kontrollieren oder deaktivieren können.
22. Deduplication MUSS mit Huge Pages und Memory Compression koordinierbar sein.
23. Aktiv gepinnte DMA-/Device-Pages DÜRFEN standardmäßig NICHT dedupliziert werden.
24. NovaOS MUSS bestehende Deduplication bei geänderten Trust- oder Security-Anforderungen wieder auflösen können.
25. Fail-safe MUSS bei unklarer Trust-Situation Isolation statt Deduplication wählen.
26. Deduplication-Aktivität, Einsparungen, Kosten, Trust-Domänen und COW-Folgekosten MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- identische Speicherinhalte können physischen RAM einsparen,
- gleichartige vertrauenswürdige Workloads können effizienter betrieben werden,
- Memory Pressure kann ohne unmittelbaren Reclaim reduziert werden,
- COW erhält trotz gemeinsamem Backing private Speichersemmantik,
- Trust- und Data-Sovereignty-Grenzen bleiben Bestandteil der Entscheidung,
- adaptive Policies können schlechte Deduplication-Kandidaten erkennen.

### Negative Konsequenzen

- Page-Scanning und Content-Vergleich benötigen CPU und Speicherbandbreite,
- Deduplication erhöht COW- und Mapping-Komplexität,
- falsche Kandidatenauswahl kann mehr Kosten als Nutzen erzeugen,
- NUMA Locality kann durch physisches Sharing verschlechtert werden,
- Trust-Policy-Änderungen können spätere Undeduplication erforderlich machen,
- Resource Accounting gemeinsamer physischer Pages wird komplexer.

---

## Verworfene Alternativen

### Globale Deduplication über alle Domains

Verworfen.

Dies würde potenzielle Side Channels und unzulässige Sharing-Beziehungen über Security-Grenzen schaffen.

### Deduplication allein anhand identischer Hashes

Verworfen.

Hash-Kollisionen dürfen keine physische Speicherzusammenführung unterschiedlicher Inhalte verursachen.

### Identische Pages automatisch zu Shared Memory machen

Verworfen.

Physische Deduplication darf die logische private Memory-Semantik nicht verändern.

### Deduplication für Secrets und sensitive Daten

Verworfen als Standard.

Die mögliche Speicherersparnis rechtfertigt keine zusätzlichen Side-Channel- und Isolationrisiken.

### Deduplication ohne Resource Accounting

Verworfen.

Shared physisches Backing darf Domain-Budgets und Ressourcenverantwortung nicht unklar machen.

### Memory Pressure darf Trust-Grenzen überschreiben

Verworfen.

Security und Data Sovereignty sind Hard Constraints und besitzen Vorrang vor Speicheroptimierungen.

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
- `ADR-MEMORY-0010_Memory_Compression_policygesteuert`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-DEDUP-0001`
- `NPSPEC-MEMORY-DEDUP-TRUST-0001`
- `NPSPEC-MEMORY-DEDUP-SCANNER-0001`
- `NPSPEC-MEMORY-DEDUP-MERGE-0001`
- `NPSPEC-MEMORY-DEDUP-COW-0001`
- `NPSPEC-MEMORY-DEDUP-ACCOUNTING-0001`
- `NPSPEC-MEMORY-DEDUP-SECURITY-0001`
- `NPSPEC-MEMORY-DEDUP-TEST-0001`

---

## Ergebnis

NovaOS verwendet Memory Deduplication nur dort, wo physisches Sharing sicher und ausdrücklich zulässig ist:

```text
Candidate Pages
      ↓
Content Equality
      +
Trust Compatibility
      +
Security / Sovereignty
      ↓
Optional Deduplication
      ↓
Shared Read-only Backing
      ↓
Write
      ↓
Copy-On-Write
```

Damit bleibt Deduplication eine kontrollierte Optimierung und wird nicht zu einer versteckten systemweiten Sharing-Beziehung.

Die zentrale Architekturregel lautet:

```text
Equal data is not equal authority.

Deduplicate only inside
compatible trust boundaries.

Security before memory savings.

When in doubt, keep pages isolated.
```