# ADR-MEMORY-0012 – ASLR, Guard Pages und Non-Executable Memory

## Status

Angenommen

## Kategorie

Kernel / Memory Management / Virtual Memory / Security / Exploit Mitigation

## Kontext

Die Isolation von Execution Domains verhindert nicht automatisch die Ausnutzung von Speicherfehlern innerhalb einer Domain.

Typische Fehlerklassen umfassen:

- Stack Overflows,
- Buffer Overflows,
- Use-after-Free,
- ungültige Pointer,
- Überschreiben von Funktionszeigern,
- Code Injection,
- Return-Oriented Programming,
- vorhersehbare Speicherlayouts.

NovaOS benötigt deshalb grundlegende Schutzmechanismen direkt im Virtual-Memory-Modell.

Drei zentrale Mechanismen sind:

```text
ASLR
Guard Pages
Non-Executable Memory
```

Diese Mechanismen verfolgen unterschiedliche Ziele:

```text
ASLR
→ erschwert Vorhersage von Adressen

Guard Pages
→ erkennen Zugriffe außerhalb gültiger Bereiche

NX Memory
→ verhindert Ausführung aus nicht autorisierten Datenbereichen
```

Keiner dieser Mechanismen ersetzt jedoch korrekte Speicherverwaltung, Capability Security oder sichere Programmiersprachen.

Sie bilden zusätzliche Verteidigungsschichten innerhalb des Defense-in-Depth-Modells von NovaOS.

---

## Entscheidung

NovaOS integriert:

- **Address Space Layout Randomization (ASLR)**,
- **Guard Pages**,
- **Non-Executable Memory (NX / Execute Disable)**

als grundlegende Sicherheitsmechanismen des Virtual Memory Managers.

Standardmodell:

```text
Virtual Address Space
        ↓
Randomized Layout
        +
Guard Regions
        +
Explicit Execute Permission
        ↓
Protected Execution Domain
```

Security Policies besitzen Vorrang vor Performance-Optimierungen oder Layout-Präferenzen.

---

## Grundprinzip

```text
Addresses should not be predictable.

Memory boundaries should fail closed.

Data is not executable by default.

Executable memory must be explicit.
```

---

## Explizite Speicherrechte

NovaOS verwendet für virtuelle Mappings explizite Zugriffsrechte.

Mindestens:

```text
Read
Write
Execute
```

Diese Rechte sind unabhängig voneinander modelliert.

Ein Mapping erhält nur die tatsächlich notwendigen Rechte.

Beispiel:

```text
Code:
Read + Execute

Data:
Read + Write

Constant Data:
Read

Guard:
No Access
```

---

## Non-Executable Memory

Speicher ist standardmäßig nicht ausführbar.

Ein Bereich darf nur dann ausführbaren Code enthalten, wenn er ausdrücklich mit Execute-Berechtigung gemappt wurde.

```text
Anonymous Memory
    ↓
RW

Executable Image
    ↓
RX
```

Das bloße Schreiben von Maschineninstruktionen in einen Datenbereich macht diesen nicht ausführbar.

---

## W^X

NovaOS folgt grundsätzlich dem Prinzip:

```text
Writable XOR Executable
```

Ein Mapping soll nicht gleichzeitig:

```text
Write + Execute
```

besitzen.

Standardfall:

```text
Code → RX
Data → RW
```

Dies erschwert direkte Code-Injection-Angriffe.

---

## JIT und dynamische Codegenerierung

Just-in-Time-Compiler und andere legitime Codegeneratoren benötigen kontrollierte Übergänge.

Beispiel:

```text
Allocate RW
    ↓
Generate Code
    ↓
Validate / Finalize
    ↓
Change Mapping
    ↓
RX
```

Ein dauerhafter RWX-Bereich ist nicht der Standardmechanismus.

Transitions zwischen Write und Execute müssen explizit autorisiert sein.

---

## Dual Mapping

Für spezialisierte Runtimes kann NovaOS alternativ unterschiedliche Mappings desselben Memory Objects erlauben.

Beispiel:

```text
Memory Object
   ├── RW Mapping
   └── RX Mapping
```

Dies ist nur zulässig, wenn Security Policy, Coherency und Authority dies explizit erlauben.

Auch hierbei darf kein unkontrollierter Schreibzugriff auf ausführbaren Code möglich werden.

---

## ASLR

Address Space Layout Randomization verändert die Platzierung relevanter Speicherbereiche innerhalb des Virtual Address Space.

Beispielsweise:

```text
Executable
Shared Libraries
Heap
Stacks
Mappings
Runtime Objects
```

können randomized platziert werden.

Dadurch soll ein Angreifer nicht zuverlässig aus einer bekannten Softwareversion auf konkrete virtuelle Adressen schließen können.

---

## ASLR pro Execution Domain

Da jede Execution Domain einen eigenen Virtual Address Space besitzt, wird ASLR pro Domain angewendet.

```text
Domain A:
Code → Address X

Domain B:
Code → Address Y
```

Identische Softwareinstanzen müssen nicht dasselbe virtuelle Layout verwenden.

---

## Randomization-Seeds

ASLR benötigt geeignete Randomness.

Seeds müssen aus einer vertrauenswürdigen Entropiequelle stammen und dürfen nicht trivial aus:

- PID,
- Startzeit,
- Domain-ID,
- fortlaufenden Countern

ableitbar sein.

Die konkrete Randomness-Infrastruktur wird außerhalb dieser ADR spezifiziert.

---

## ASLR-Granularität

ASLR soll nicht nur eine einzelne globale Basisadresse verschieben.

Je nach Plattform und ABI können unterschiedliche Regionen unabhängig randomisiert werden.

Beispiel:

```text
Image Base
Libraries
Heap Base
Stack Base
Anonymous Mappings
```

Die tatsächlich verfügbare Entropie hängt vom jeweiligen Address-Space-Format ab.

---

## Alignment

Randomization muss die technischen Alignment-Anforderungen der jeweiligen:

- Page Size,
- Huge Page,
- ABI,
- Hardwarearchitektur

einhalten.

ASLR darf keine ungültigen oder ineffizienten Mappings erzeugen.

---

## Guard Pages

Guard Pages sind absichtlich nicht zugängliche virtuelle Bereiche.

```text
Valid Memory
    ↓
Guard Page
    ↓
Invalid Access → Fault
```

Sie besitzen kein für den jeweiligen Zugriff nutzbares Mapping.

---

## Stack Guards

Stacks sollen durch Guard Regions gegen Überlauf geschützt werden.

Beispiel:

```text
Guard
-----
Stack
-----
Guard
```

Je nach Stack-Growth-Modell kann mindestens an der Wachstumsgrenze eine Guard Page erforderlich sein.

Ein Zugriff über die gültige Stack-Grenze hinaus erzeugt einen kontrollierten Fault.

---

## Heap und Allocator Guards

Memory Allocators dürfen Guard Pages gezielt zwischen oder um Speicherregionen einsetzen.

Beispielsweise für:

- große Allokationen,
- besonders sensitive Objekte,
- Debugging,
- Security-hardened Modi.

Guard Pages für jede kleine Allokation wären hingegen unverhältnismäßig teuer.

Der Einsatz bleibt deshalb teilweise policygesteuert.

---

## Guard Regions

NovaOS betrachtet nicht nur einzelne Pages, sondern allgemein **Guard Regions**.

Dadurch können auch mehrere nicht gemappte Pages verwendet werden.

```text
Object A
   ↓
Guard Region
   ↓
Object B
```

Die Größe kann abhängig von Sicherheitsniveau, Adressraumgröße und Workload variieren.

---

## Null Page

Die niedrigsten virtuellen Adressen sollen standardmäßig nicht als normaler Userspace-Speicher verfügbar sein.

Insbesondere soll die Null-Adresse nicht auf regulären nutzbaren Speicher zeigen.

Dadurch werden Nullpointer-Dereferenzierungen frühzeitig als Fault erkannt.

Kompatibilitätsausnahmen müssen explizit und stark eingeschränkt sein.

---

## Invalid Mapping Fault

Ein Zugriff auf:

```text
Guard Page
Unmapped Region
Non-canonical Address
```

muss als ungültiger Speicherzugriff erkannt werden.

Er darf nicht durch Demand Paging automatisch legitimiert werden.

Dies folgt aus `ADR-MEMORY-0006_Demand_Paging_und_Lazy_Allocation`.

---

## Execute Fault

Versucht ein Task Code aus einem nicht ausführbaren Mapping zu starten:

```text
Instruction Fetch
      ↓
NX Mapping
      ↓
Protection Fault
```

Der Fehler wird dem verursachenden Execution Context zugeordnet.

NovaOS darf das Mapping nicht automatisch ausführbar machen.

---

## Write-to-Code Fault

Ein Schreibzugriff auf RX-Code erzeugt ebenfalls einen Protection Fault.

```text
Write
 ↓
RX Mapping
 ↓
Fault
```

Codeänderungen müssen über einen expliziten autorisierten Update-Mechanismus erfolgen.

---

## Hot Replacement

Hot Replacement und Live Evolution benötigen kontrollierte Codeänderungen.

Typischer Ablauf:

```text
Prepare new code in RW memory
        ↓
Validation / Trust Check
        ↓
Finalize
        ↓
Map RX
        ↓
Atomic activation
```

Die Hot-Replacement-Architektur darf W^X nicht umgehen.

---

## Shared Libraries und Shared Code

Mehrere Domains dürfen dasselbe immutable ausführbare Memory Object verwenden.

Beispiel:

```text
Executable Object
   ├── RX Mapping → Domain A
   └── RX Mapping → Domain B
```

Die virtuellen Adressen können aufgrund von ASLR unterschiedlich sein.

```text
Same code object
Different virtual addresses
```

Dies verhindert unnötige Kopien und bleibt mit ASLR vereinbar.

---

## Copy-On-Write

COW darf Protection-Bits nicht abschwächen.

Eine RX-Codepage darf durch COW nicht automatisch zu RWX werden.

Wenn writable private data aus einem gemeinsamen Backing erzeugt wird, erhält die neue Page nur die für diesen Zustand vorgesehenen Rechte.

---

## Shared Memory

Shared Memory Objects besitzen ebenfalls explizite Execute-Rechte.

Normales IPC- oder Daten-Sharing ist standardmäßig nicht ausführbar.

Executable Shared Memory erfordert einen expliziten Security Contract.

Dies verhindert, dass beliebige Shared Buffers als Code-Injection-Kanal verwendet werden.

---

## Huge Pages

Huge Pages können unterschiedliche Schutzgranularitäten einschränken.

Wenn innerhalb einer Huge Page unterschiedliche Rechte notwendig werden:

```text
RX
RW
Guard
```

muss sie gegebenenfalls gesplittet werden.

Huge-Page-Effizienz darf Security Boundaries nicht vergröbern.

---

## Memory Compression

Executable Pages dürfen grundsätzlich komprimierbar sein, sofern der jeweilige Memory Contract dies zulässt.

Nach Dekompression müssen ihre ursprünglichen Protection-Bits exakt wiederhergestellt werden.

Kompression darf niemals dazu führen, dass ausführbarer Speicher temporär oder dauerhaft beschreibbar wird.

---

## Memory Deduplication

Executable immutable Pages sind potenziell gute Deduplication-Kandidaten, sofern die Trust-Policy dies erlaubt.

Die Deduplication darf die ursprünglichen Rechte nicht verändern.

Beispiel:

```text
RX + RX
   ↓
Deduplicated RX
```

Nicht zulässig wäre:

```text
RX + RW
   ↓
shared RWX
```

---

## Realtime

ASLR selbst verursacht primär beim Aufbau des Address Space Kosten und steht daher grundsätzlich nicht im Konflikt mit Realtime.

Guard Pages und NX-Faults stellen Fehlerpfade dar und dürfen nicht Teil eines erfolgreichen Hard-Realtime-Kernpfades sein.

Realtime-Domains können ihr Address-Space-Layout vor Beginn der kritischen Phase vollständig vorbereiten.

---

## Deterministic Mode

ASLR steht teilweise im Spannungsverhältnis zu reproduzierbaren virtuellen Adressen.

Deterministic Mode muss deshalb zwischen mindestens zwei Anforderungen unterscheiden können:

```text
Deterministic program behavior
```

und:

```text
Identical virtual address layout
```

Für viele deterministische Workloads ist ein identisches Layout nicht erforderlich.

Wenn reproduzierbare Adressen explizit benötigt werden, kann ein definierter Seed oder ein festes Layout verwendet werden, sofern die Security Policy dies zulässt.

Security besitzt bei Konflikten Vorrang, sofern kein expliziter vertrauenswürdiger Ausführungsmodus eine andere Entscheidung erlaubt.

---

## Debugging

Debugger benötigen Einblick in das tatsächliche randomisierte Address-Space-Layout.

ASLR darf deshalb Debugging nicht durch unzugängliche interne Informationen verhindern.

Entsprechende Introspection benötigt jedoch explizite Debug Authority.

Das globale Deaktivieren von ASLR ist nicht der bevorzugte Debugging-Mechanismus.

---

## Crash Dumps

Crash Dumps und Diagnostik müssen die tatsächlich verwendeten virtuellen Layouts dokumentieren können.

Dazu gehören insbesondere:

```text
Mapping Base
Mapping Size
Protection
Backing Object
ASLR Metadata where authorized
Fault Address
Fault Type
```

Dadurch bleiben Fehler trotz Randomisierung analysierbar.

---

## Security Domains

ASLR- und Memory-Protection-Policies können abhängig vom Security Context unterschiedlich streng sein.

Beispielsweise können besonders exponierte Dienste zusätzliche:

- Guard Regions,
- Randomization,
- Mapping Restrictions

verwenden.

Die Basisregeln NX und explizite Execute Authority bleiben jedoch systemweit gültig.

---

## Fail-Safe Verhalten

Kann eine gewünschte Security-Eigenschaft nicht hergestellt werden, darf NovaOS sie nicht still abschwächen.

Beispiele:

```text
Executable mapping cannot enforce NX separation
        ↓
Reject

JIT cannot obtain authorized W→X transition
        ↓
Reject

Guard region cannot be established where required
        ↓
Fail allocation / mapping
```

Security-Constraints sind Hard Constraints.

---

## Architecture Introspection

Virtual-Memory-Sicherheitszustände müssen kontrolliert introspektierbar sein.

Relevant sind insbesondere:

```text
Mapping Range
Read / Write / Execute
Guard Regions
ASLR enabled
Randomized regions
W^X violations prevented
Protection Faults
Execute Faults
Guard Faults
JIT transitions
```

Sensitive ASLR-Informationen dürfen nur autorisierten Komponenten zugänglich sein.

Eine uneingeschränkte Offenlegung randomisierter Adressen würde den Schutzwert von ASLR reduzieren.

---

## Normative Anforderungen

1. NovaOS MUSS ASLR, Guard Pages und Non-Executable Memory als native Virtual-Memory-Sicherheitsmechanismen unterstützen.
2. Speicher MUSS standardmäßig nicht ausführbar sein, sofern Execute nicht explizit gewährt wurde.
3. Read-, Write- und Execute-Rechte MÜSSEN explizit und unabhängig modellierbar sein.
4. NovaOS SOLL W^X als Standardregel für ausführbaren Speicher verwenden.
5. Dauerhafte RWX-Mappings DÜRFEN NICHT der normale Mechanismus für dynamische Codegenerierung sein.
6. JIT-Runtimes MÜSSEN kontrollierte Write-to-Execute-Übergänge verwenden.
7. Execute Authority MUSS explizit kontrollierbar sein.
8. ASLR MUSS pro Virtual Address Space beziehungsweise Execution Domain anwendbar sein.
9. ASLR MUSS geeignete, nicht trivial vorhersehbare Randomness verwenden.
10. Randomization MUSS Architektur-, ABI- und Alignment-Anforderungen einhalten.
11. Guard Pages beziehungsweise Guard Regions MÜSSEN als nicht zugängliche virtuelle Bereiche unterstützt werden.
12. Stacks MÜSSEN gegen unkontrolliertes Überschreiten ihrer gültigen Grenzen durch geeignete Guard-Semantik schützbar sein.
13. Die Null-Adresse SOLL standardmäßig nicht als regulärer Userspace-Speicher mapbar sein.
14. Guard- und andere Invalid-Mapping-Faults DÜRFEN NICHT durch Demand Paging automatisch legitimiert werden.
15. Ein Instruction Fetch aus NX Memory MUSS einen Protection Fault erzeugen.
16. Ein Schreibzugriff auf nicht beschreibbaren ausführbaren Code MUSS einen Protection Fault erzeugen.
17. Hot Replacement MUSS W^X- und Execute-Authority-Regeln erhalten.
18. Shared Code DARF in mehreren Domains an unterschiedlichen ASLR-Adressen gemappt werden.
19. Shared Memory MUSS standardmäßig nicht ausführbar sein.
20. Huge Pages MÜSSEN gesplittet werden können, wenn feinere Security- oder Guard-Grenzen erforderlich sind.
21. Compression und Deduplication DÜRFEN bestehende Protection-Rechte NICHT abschwächen.
22. Deterministic Mode MUSS ASLR kontrolliert und policykonform konfigurieren können.
23. Debugging DARF ASLR nur über explizite Debug Authority introspektieren oder verändern.
24. Security-Anforderungen an NX, W^X und Guard Regions MÜSSEN als Hard Constraints behandelt werden.
25. Kann eine erforderliche Protection nicht hergestellt werden, MUSS die Mapping- oder Ausführungsoperation fail-safe scheitern.
26. Protection Faults, Guard Faults und relevante Mapping-Sicherheitszustände MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Code-Injection-Angriffe werden durch NX und W^X erschwert,
- vorhersehbare Speicherlayouts werden durch ASLR reduziert,
- Stack- und andere Bereichsüberschreitungen können durch Guard Pages früher erkannt werden,
- virtuelle Speicherrechte werden Teil des expliziten Security-Modells,
- JIT, Shared Code und Hot Replacement bleiben kontrolliert möglich,
- die Mechanismen ergänzen Capability- und Trust-Security um hardwaregestützte Schutzschichten.

### Negative Konsequenzen

- ASLR erschwert teilweise Debugging und reproduzierbare Adressanalysen,
- Guard Regions verbrauchen virtuellen Adressraum und Page-Table-Ressourcen,
- JIT-Runtimes benötigen zusätzliche Mapping-Transitions,
- Huge Pages können aufgrund feiner Protection-Grenzen häufiger gesplittet werden,
- zusätzliche Protection- und TLB-Updates können Performance-Kosten verursachen.

---

## Verworfene Alternativen

### Feste Address-Space-Layouts

Verworfen als Standard.

Sie erleichtern Exploits, die bekannte virtuelle Adressen voraussetzen.

### Alle Memory-Mappings ausführbar machen

Verworfen.

Dies würde Datenbereiche unmittelbar als Code-Injection-Ziel verfügbar machen.

### RWX als Standard für JIT-Code

Verworfen.

Dauerhaft gleichzeitig beschreibbarer und ausführbarer Speicher erhöht die Angriffsfläche unnötig.

### Guard Pages nur im Debug-Build

Verworfen.

Stack- und kritische Guard Regions besitzen auch im Produktivsystem einen erheblichen Sicherheitswert.

### ASLR statt anderer Schutzmechanismen

Verworfen.

ASLR ist keine Isolation und ersetzt weder NX, W^X, Capabilities noch sichere Speicherverwaltung.

### Security-Grenzen zugunsten von Huge Pages aufweichen

Verworfen.

Performance-Optimierung besitzt keine Priorität gegenüber Speicherprotektion.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-MEMORY-0005_Virtual_Address_Spaces_pro_Execution_Domain`
- `ADR-MEMORY-0006_Demand_Paging_und_Lazy_Allocation`
- `ADR-MEMORY-0007_Copy_On_Write_als_gezielte_Optimierung`
- `ADR-MEMORY-0008_Shared_Memory_über_explizite_Shared_Objects`
- `ADR-MEMORY-0009_Huge_Pages_policygesteuert`
- `ADR-MEMORY-0010_Memory_Compression_policygesteuert`
- `ADR-MEMORY-0011_Memory_Deduplication_nur_innerhalb_geeigneter_Trust_Domänen`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0012_Hot_Replacement_und_Live_Evolution`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SCHED-0012_Hard_Constraints_vor_Optimierungszielen`

---

## Zugehörige NPSPECs

- `NPSPEC-MEMORY-ASLR-0001`
- `NPSPEC-MEMORY-GUARD-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `NPSPEC-MEMORY-NX-0001`
- `NPSPEC-MEMORY-WX-0001`
- `NPSPEC-MEMORY-JIT-PROTECTION-0001`
- `NPSPEC-MEMORY-PROTECTION-FAULT-0001`
- `NPSPEC-MEMORY-SECURITY-TEST-0001`

---

## Ergebnis

NovaOS integriert grundlegende Exploit-Mitigation direkt in sein Virtual-Memory-Modell:

```text
Execution Domain
      ↓
Randomized Address Space
      +
Guard Regions
      +
Explicit R/W/X Permissions
      ↓
Hardware-enforced Memory Protection
```

Dabei gilt:

```text
Code → RX
Data → RW
Guard → No Access
```

und dynamische Codegenerierung erfolgt nur über kontrollierte Zustandsübergänge.

Die zentrale Architekturregel lautet:

```text
Randomize where possible.

Guard critical boundaries.

Data is non-executable by default.

Never trade memory protection
for convenience or performance.
```