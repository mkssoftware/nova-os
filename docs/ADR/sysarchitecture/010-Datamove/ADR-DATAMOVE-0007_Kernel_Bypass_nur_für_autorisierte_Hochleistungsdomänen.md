# ADR-DATAMOVE-0007 – Kernel Bypass nur für autorisierte Hochleistungsdomänen

## Status

Angenommen

## Kategorie

Kernel / Data Movement / I/O / Security / Performance

## Kontext

Bei extrem latenz- oder durchsatzkritischen Workloads können Kernel-Übergänge, zusätzliche Scheduling-Schritte und wiederholte I/O-Abstraktionen einen relevanten Anteil der Gesamtkosten verursachen.

Techniken wie Kernel Bypass ermöglichen deshalb einen direkteren Datenpfad zwischen einer Execution Domain und Hardware.

```text
Normal:

Application
    ↓
Kernel I/O Path
    ↓
Driver
    ↓
Device


Kernel Bypass:

Authorized Domain
    ↓
Controlled Device Interface
    ↓
Device
```

Ein allgemeiner Kernel Bypass würde jedoch zentrale Schutz-, Ressourcen- und Kontrollmechanismen des Systems umgehen.

---

## Entscheidung

NovaOS unterstützt Kernel Bypass **ausschließlich als explizit autorisierten Hochleistungsmodus**.

Kernel Bypass ist kein allgemeiner Standard-I/O-Pfad.

Er darf nur verwendet werden, wenn die Execution Domain:

- die notwendige Authority besitzt,
- isolierte Hardware-Ressourcen erhalten kann,
- definierte Resource Budgets besitzt,
- alle Security- und Data-Sovereignty-Anforderungen erfüllt.

```text
Execution Domain
      ↓
Authorization
      ↓
Resource Assignment
      ↓
Isolation Setup
      ↓
Kernel-Bypass Data Path
```

Der Kernel bleibt für Einrichtung, Isolation, Ressourcenvergabe, Widerruf und Kontrolle verantwortlich.

---

## Grundprinzip

```text
Bypass the data path,
not the security model.
```

---

## Autorisierte Hochleistungsdomänen

Kernel Bypass wird nur für Execution Domains aktiviert, deren Execution Contract dies ausdrücklich erlaubt.

Typische Einsatzbereiche können sein:

- High-Performance Networking,
- Storage,
- Realtime Data Processing,
- GPU/NPU- und Accelerator-Pipelines,
- spezialisierte Compute-Workloads.

Normale Anwendungen erhalten keinen impliziten direkten Hardwarezugriff.

---

## Kontrollierter Bypass

Kernel Bypass bedeutet nicht, dass der Kernel vollständig aus der Kontrolle entfernt wird.

Der Kernel beziehungsweise die zuständigen Systemkomponenten bleiben verantwortlich für:

```text
Authorization
Resource Assignment
IOMMU Isolation
Memory Registration
Interrupt / Queue Assignment
Accounting
Revocation
```

Nur der eigentliche Hochfrequenz-Datenpfad darf nach erfolgreicher Einrichtung weitgehend ohne Kernel-Interaktion arbeiten.

---

## Memory und IOMMU

Direkter Device-Zugriff darf ausschließlich auf explizit autorisierte Speicherbereiche erfolgen.

IOMMU-Isolation soll verwendet werden, wenn die Plattform sie unterstützt.

Shared Buffers, DMA-Mappings und Pinned Memory bleiben Teil der bestehenden NovaOS-Memory- und Data-Movement-Architektur.

Kernel Bypass erzeugt kein separates Speicher- oder DMA-Modell.

---

## Ressourcenisolation

Direkt verwendete Hardware-Ressourcen müssen der jeweiligen Domain kontrolliert zugeordnet werden können.

Dazu können gehören:

- Device Queues,
- DMA Contexts,
- Interrupts,
- Buffer Pools,
- Descriptor Rings,
- Accelerator Queues.

Eine Hochleistungsdomäne darf dadurch andere Domains nicht unkontrolliert verdrängen oder deren Ressourcen verwenden.

---

## Resource Economy

Kernel Bypass darf die systemweite Resource Economy nicht umgehen.

Auch direkt verwendete Ressourcen bleiben:

- budgetiert,
- zurechenbar,
- messbar,
- begrenzbar.

Der reduzierte Kernel-Datenpfad bedeutet keine reduzierte Ressourcenverantwortung.

---

## Security und Data Sovereignty

Security-, Trust- und Data-Sovereignty-Regeln bleiben Hard Constraints.

Kernel Bypass darf insbesondere nicht:

- beliebigen physischen Speicher zugänglich machen,
- Capability-Prüfungen dauerhaft umgehen,
- fremde Device Queues übernehmen,
- Informationsflussregeln außer Kraft setzen.

Kann die erforderliche Isolation nicht hergestellt werden, wird Kernel Bypass nicht aktiviert.

---

## Realtime

Kernel Bypass kann für explizite Realtime-Domänen sinnvoll sein, wenn dadurch variable Kernelpfade reduziert werden.

Benötigte Ressourcen sollen vor Beginn der kritischen Phase reserviert und eingerichtet werden.

Kernel Bypass allein garantiert jedoch keine Realtime-Eigenschaften.

---

## Fallback

Jede Capability, die Kernel Bypass als Optimierung verwendet, soll soweit architektonisch sinnvoll auch einen kontrollierten normalen Systempfad besitzen.

```text
Kernel Bypass available
        ↓
High-Performance Path

Kernel Bypass unavailable
        ↓
Normal Kernel Path
```

Eine fehlende Bypass-Fähigkeit darf keine allgemeine Systemfunktion voraussetzen, sofern sie nicht explizit Bestandteil des Execution Contracts ist.

---

## Widerruf

NovaOS muss Kernel-Bypass-Zugriffe kontrolliert entziehen können.

Dies kann insbesondere bei:

- Domain-Beendigung,
- Security-Verletzungen,
- Device Removal,
- Ressourcenverletzungen,
- System-Rekonfiguration

erforderlich sein.

Nach einem Widerruf darf die Domain keinen weiteren direkten Zugriff auf die entzogenen Ressourcen besitzen.

---

## Architecture Introspection

Kernel-Bypass-Nutzung muss kontrolliert introspektierbar sein.

Relevant sind insbesondere:

- autorisierte Domain,
- verwendetes Device,
- zugewiesene Queues und Ressourcen,
- DMA-/IOMMU-Kontext,
- Pinned Memory,
- Resource Budgets,
- aktiver Bypass-Modus.

---

## Normative Anforderungen

1. NovaOS DARF Kernel Bypass für autorisierte Hochleistungsdomänen unterstützen.
2. Kernel Bypass DARF NICHT der allgemeine Standard-I/O-Pfad sein.
3. Kernel Bypass MUSS explizite Authority erfordern.
4. Der Kernel MUSS Kontrolle über Einrichtung, Isolation, Ressourcenvergabe und Widerruf behalten.
5. Direkter Device-Zugriff MUSS auf autorisierte Ressourcen und Speicherbereiche begrenzt sein.
6. IOMMU-Isolation SOLL verwendet werden, wenn geeignete Hardwareunterstützung verfügbar ist.
7. Kernel Bypass MUSS die bestehenden Shared-Buffer-, DMA- und Pinned-Memory-Abstraktionen verwenden.
8. Kernel Bypass DARF Resource Budgets und Accounting NICHT umgehen.
9. Security-, Trust- und Data-Sovereignty-Constraints MÜSSEN vollständig erhalten bleiben.
10. Fehlende ausreichende Isolation MUSS zur Ablehnung des Kernel Bypass führen.
11. Bypass-Ressourcen MÜSSEN kontrolliert widerrufbar sein.
12. Kernel-Bypass-Nutzung und Ressourcenbindungen MÜSSEN introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- sehr niedrige I/O-Latenzen für spezialisierte Workloads,
- weniger Kernel-Übergänge im Hot Path,
- höhere Durchsatzraten,
- bessere Nutzung moderner Queue-basierter Hardware,
- Integration in die bestehende NovaOS-Sicherheits- und Ressourcenarchitektur.

### Negative Konsequenzen

- komplexere Ressourcen- und Device-Isolation,
- nicht jede Hardware eignet sich für sicheren Kernel Bypass,
- zusätzliche Anforderungen an IOMMU und Device Queues,
- Hochleistungsdomänen benötigen strengere Lifecycle-Kontrolle.

---

## Verworfene Alternativen

### Kernel Bypass für jede Anwendung

Verworfen.

Dies würde Isolation, Ressourcensteuerung und Systemkontrolle unnötig schwächen.

### Kein Kernel Bypass

Verworfen.

Für spezialisierte Hochleistungs- und Realtime-Workloads können Kernelpfade vermeidbare Kosten verursachen.

### Vollständige Übergabe der Hardwarekontrolle

Verworfen.

Der Datenpfad darf den Kernel umgehen; Security-, Ressourcen- und Lifecycle-Kontrolle dürfen es nicht.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DATAMOVE-0001_Zero_Copy_Datenpfade`
- `ADR-DATAMOVE-0002_Shared_Buffers_als_systemweite_Abstraktion`
- `ADR-DATAMOVE-0004_DMA_Based_Transfer`
- `ADR-DATAMOVE-0005_Copy_Avoidance_vor_blinder_Zero_Copy_Optimierung`
- `ADR-MEMORY-0014_Pinned_Memory_als_budgetierte_Ressource`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-DATAMOVE-KERNEL-BYPASS-0001`
- `NPSPEC-DATAMOVE-KERNEL-BYPASS-AUTH-0001`
- `NPSPEC-DATAMOVE-KERNEL-BYPASS-RESOURCE-0001`
- `NPSPEC-DATAMOVE-KERNEL-BYPASS-IOMMU-0001`
- `NPSPEC-DATAMOVE-KERNEL-BYPASS-REVOCATION-0001`
- `NPSPEC-DATAMOVE-KERNEL-BYPASS-TEST-0001`

---

## Ergebnis

NovaOS erlaubt Kernel Bypass dort, wo die zusätzlichen Performancevorteile gerechtfertigt sind, ohne die übergeordnete Systemkontrolle aufzugeben.

```text
Kernel controls:
Security + Resources + Isolation + Lifecycle

Domain controls:
Authorized high-performance data path
```

Die zentrale Architekturregel lautet:

```text
Bypass overhead.

Never bypass authority,
isolation or accountability.
```