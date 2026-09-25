# NPSPEC-MEMORY-PROTECTION-0001 – Nova Memory Protection

## Status

Angenommen

## Kategorie

Memory / Protection / Isolation / Virtual Memory / Security

## Zweck

NovaOS definiert Memory Protection als systemweiten Mechanismus zum Schutz von Speicher vor nicht autorisierten Lese-, Schreib- und Ausführungszugriffen.

```text
Memory Access
     ↓
Mapping + Permissions
     ↓
Hardware Protection
   ┌──────┴──────┐
Allowed        Denied
   ↓              ↓
Access       Protection Fault
```

Memory Protection bildet eine zentrale Sicherheitsgrenze zwischen Kernel, Prozessen, Diensten, Geräten und gemeinsam genutztem Speicher.

## Grundprinzip

Der Besitz oder die Kenntnis einer Adresse erzeugt keine Zugriffsberechtigung.

```text
Known Address ≠ Authorized Access
Mapped Memory ≠ Unrestricted Memory
```

Jeder Speicherzugriff muss den effektiven Schutzregeln des aktuellen Ausführungskontexts entsprechen.

## Schutzebenen

Memory Protection entsteht aus mehreren Ebenen:

```text
Security Policy
      ↓
Capability / Object Rights
      ↓
VAS Mapping Permissions
      ↓
Page Table Permissions
      ↓
MMU Enforcement
```

Eine untere Ebene darf keine Rechte gewähren, die eine höhere Ebene verbietet.

## Zugriffsrechte

Mappings müssen mindestens folgende Rechte unterscheiden können:

```text
Read
Write
Execute
```

Zusätzlich müssen Schutzdomänen unterscheidbar sein:

```text
User
Kernel
Device
Shared
```

Die konkrete Hardwaredarstellung bleibt architekturabhängig.

## Effective Permissions

Die tatsächlich zulässigen Rechte ergeben sich aus allen relevanten Einschränkungen.

```text
Effective Permissions
=
Object Rights
∩
Capability Rights
∩
Mapping Rights
∩
Security Policy
∩
Hardware Constraints
```

Rechte dürfen durch Mapping-Operationen eingeschränkt, aber nicht ohne Autorisierung erweitert werden.

## User-/Kernel-Isolation

User Mode darf nicht direkt auf geschützten Kernel-Speicher zugreifen.

```text
User Mode
   ↓
Kernel Address
   ↓
Denied
```

Kernel-Speicher muss durch verfügbare Hardwaremechanismen geschützt werden.

Ein Wechsel in einen privilegierten Ausführungskontext darf nur über definierte Kernel-Einstiegspunkte erfolgen.

## Prozessisolation

Private Speicherbereiche eines Prozesses dürfen für andere Prozesse nicht automatisch zugänglich sein.

```text
Process A Private Memory
           X
Process B
```

Interprozesszugriffe benötigen einen expliziten Mechanismus wie:

```text
Shared Memory
IPC
Capability-authorized Mapping
Controlled Debug Access
```

## Read / Write / Execute

NovaOS muss unterschiedliche Zugriffsarten getrennt kontrollieren.

```text
Code        → Read + Execute
Data        → Read + Write
Read-only   → Read
```

Schreibzugriffe auf schreibgeschützte Pages und Ausführungszugriffe auf nicht ausführbare Pages müssen Protection Faults erzeugen.

## Write XOR Execute

NovaOS soll grundsätzlich:

```text
Writable XOR Executable
```

verwenden.

Normale Speicherbereiche sollen nicht gleichzeitig schreibbar und ausführbar sein.

```text
Write + Execute
```

darf nur über einen explizit autorisierten Mechanismus erlaubt werden, beispielsweise für kontrollierte JIT-Runtimes.

## Guard Pages

Nicht gemappte oder explizit geschützte Guard Pages sollen Speicherbereiche voneinander trennen können.

```text
Stack
  ↓
Guard Page
  ↓
Other Memory
```

Typische Anwendungen:

```text
Stack Overflow Detection
Heap Protection
Critical Structure Isolation
Kernel Stack Protection
```

Ein Zugriff muss einen kontrollierten Fault erzeugen.

## Protection Changes

Zugriffsrechte bestehender Mappings dürfen geändert werden.

```text
Read/Write
    ↓
Protect
    ↓
Read-Only
```

Eine Rechteerweiterung muss autorisiert werden.

Eine Einschränkung muss nach Abschluss auf allen relevanten CPUs wirksam sein.

## TLB-Kohärenz

Änderungen an Memory Permissions können gecachte TLB-Translationen ungültig machen.

```text
Permission Change
       ↓
Page Table Update
       ↓
TLB Invalidation
       ↓
Protection Effective
```

Eine entfernte Berechtigung darf nach Abschluss der Operation nicht über einen veralteten TLB-Eintrag weiter nutzbar sein.

## Copy-on-Write

COW verwendet Memory Protection gezielt zur Erkennung von Schreibzugriffen.

```text
Logical Writable Page
        ↓
Physical Read-Only COW Mapping
        ↓
Write Fault
        ↓
COW Resolution
```

Ein COW-Fault muss von einer echten Zugriffsverletzung unterscheidbar sein.

## Shared Memory

Shared Memory darf für unterschiedliche Teilnehmer unterschiedliche Rechte besitzen.

```text
Shared Object
├── Process A → Read/Write
├── Process B → Read
└── Process C → No Access
```

Gemeinsames physisches Backing bedeutet keine gemeinsamen Berechtigungen.

## Device Memory

MMIO- und Device-Memory-Bereiche benötigen besondere Schutzregeln.

```text
Device Memory
      ↓
Authorized Mapping
      ↓
Driver / Service
```

Direkte Device Mappings müssen explizit autorisiert sein und geeignete Cache- sowie Zugriffseigenschaften verwenden.

## DMA-Schutz

CPU Memory Protection allein schützt nicht vor DMA-fähigen Geräten.

```text
Device
  ↓
DMA
  ↓
Physical Memory
```

Sofern verfügbar, muss die IOMMU zur Isolation von Device-Zugriffen eingesetzt werden.

```text
Device
  ↓
IOMMU
  ↓
Authorized Frames only
```

## Sensitive Memory

Sicherheitskritischer Speicher darf zusätzliche Eigenschaften erhalten.

Beispiele:

```text
Cryptographic Keys
Credentials
Security State
Kernel Secrets
```

Mögliche Policies:

```text
Non-Swappable
Non-Deduplicatable
Non-Executable
Restricted Mapping
Secure Erasure
```

## Protection Fault

Ein nicht erlaubter Zugriff muss kontrolliert klassifiziert werden.

Mindestens:

```text
Read Violation
Write Violation
Execute Violation
User / Kernel Violation
Guard Fault
COW Fault
Invalid Mapping
```

Ein Protection Fault darf nicht automatisch als Kernel Panic behandelt werden.

Die Reaktion hängt vom betroffenen Security Context und der Fault-Ursache ab.

## Fail-Safe

Kann NovaOS die erforderliche Speicherisolation nicht sicher herstellen, gilt:

```text
Unknown Protection State
        ≠
Allowed Access
```

Bei sicherheitskritischen Unsicherheiten muss der Zugriff verweigert oder der betroffene Ausführungskontext kontrolliert beendet werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Memory Regions
Effective Permissions
Protection Faults
Permission Changes
Guard Regions
Executable Mappings
Writable+Executable Exceptions
Device Mappings
```

Sensible Adressen und fremde Mapping-Informationen müssen entsprechend dem Security Context gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS Memory Protection hardwaregestützt durchsetzen, sofern die Plattform entsprechende Mechanismen bereitstellt.
2. User- und Kernel-Speicher MÜSSEN voneinander isoliert werden.
3. Private Prozessspeicherbereiche DÜRFEN ohne explizite Autorisierung NICHT durch andere Prozesse zugänglich sein.
4. Read-, Write- und Execute-Rechte MÜSSEN getrennt kontrollierbar sein.
5. NovaOS SOLL Write-XOR-Execute standardmäßig durchsetzen.
6. Schreibbar-ausführbarer Speicher MUSS explizit autorisiert werden.
7. Rechteerweiterungen MÜSSEN durch die zuständige Security Policy autorisiert sein.
8. Entfernte Rechte DÜRFEN nach Abschluss einer Protection-Änderung nicht über veraltete TLB-Einträge weiter nutzbar sein.
9. Shared Memory DARF die individuellen Permissions seiner Teilnehmer NICHT vereinheitlichen.
10. Device Memory MUSS über explizit autorisierte Mappings geschützt werden.
11. DMA-fähige Geräte SOLLEN durch IOMMU-Isolation auf autorisierte Speicherbereiche begrenzt werden.
12. Protection Faults und Permission-Änderungen SOLLEN sicherheitsgefiltert introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-PAGETABLE-0001`
- `NPSPEC-MEMORY-TLB-0001`
- `NPSPEC-MEMORY-COW-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `ADR-MEMORY-0021`
- `ADR-VERIFY-0003_Memory_Safety`
- `ADR-VERIFY-0007_Information_Flow_Verification`

## Ergebnis

```text
Access Request
      ↓
Security + Capability Rights
      ↓
Mapping Permissions
      ↓
Page Table Protection
      ↓
MMU / IOMMU
   ┌──────┴──────┐
Allowed        Denied
   ↓              ↓
Memory Access  Protection Fault
```

NovaOS erhält damit ein mehrschichtiges Memory-Protection-Modell, das Prozess-, Kernel-, Shared- und Device-Speicher konsequent isoliert und Hardware-Schutzmechanismen mit der übergeordneten Capability- und Security-Architektur verbindet.