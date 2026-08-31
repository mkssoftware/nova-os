# ADR-DRIVER-0003 – Driver Isolation Domains

## Status

Angenommen

## Kategorie

Driver / Isolation / Execution Domains / Security / Reliability

## Kontext

`ADR-DRIVER-0001` definiert ein hybrides Kernel- und User-Mode-Drivermodell. `ADR-DRIVER-0002` legt Driver Sandboxing und Least-Privilege-Zugriff auf Hardware fest.

Für User-Mode-Treiber muss zusätzlich festgelegt werden, wie deren Isolation strukturiert wird.

Würden alle User-Mode-Treiber in einer gemeinsamen Ausführungsdomäne laufen, könnte ein Fehler oder kompromittierter Treiber andere Treiber beeinflussen. Eine eigene Domain für jede kleinste Treiberkomponente kann dagegen unnötige Ressourcen- und Kommunikationskosten verursachen.

NovaOS benötigt deshalb explizite **Driver Isolation Domains**.

---

## Entscheidung

User-Mode-Treiber werden in kontrollierten **Driver Isolation Domains** ausgeführt.

```text
Device
  ↓
Driver Isolation Domain
  ├── Driver Code
  ├── Driver State
  ├── Assigned Resources
  └── Driver Tasks
```

Eine Isolation Domain bildet die Sicherheits-, Fehler-, Ressourcen- und Lebensdauergrenze für die darin ausgeführten Treiberkomponenten.

Standardmäßig sollen voneinander unabhängige Treiber nicht dieselbe Isolation Domain teilen.

---

## Grundprinzip

```text
Isolate failures at meaningful driver boundaries.
```

---

## Isolation Domain

Eine Driver Isolation Domain basiert auf den normalen NovaOS Execution-Domain-Mechanismen und erhält insbesondere:

- einen isolierten Virtual Address Space,
- einen eigenen Security Context,
- Resource Budgets,
- zugewiesene Hardware-Ressourcen,
- definierte Driver Capabilities.

Es entsteht damit kein separates zweites Prozess- oder Speichermodell nur für Treiber.

---

## Isolationsgrenze

Treiber unterschiedlicher Isolation Domains dürfen nicht direkt auf den privaten Zustand der jeweils anderen Domain zugreifen.

Kommunikation erfolgt über definierte Systemmechanismen und Driver Contracts.

```text
Driver Domain A
      ↓
Defined Interface
      ↓
Driver Domain B
```

Ein Fehler in Domain A soll Domain B grundsätzlich nicht korrumpieren können.

---

## Gruppierung von Treibern

Mehrere eng zusammengehörende Treiberkomponenten dürfen innerhalb derselben Isolation Domain ausgeführt werden, wenn dies durch ihren Driver Contract gerechtfertigt ist.

Beispiele können ein gemeinsam implementierter Device Stack oder mehrere stark gekoppelte Funktionen eines Geräts sein.

Eine gemeinsame Domain bedeutet jedoch eine gemeinsame Fehler- und Vertrauensgrenze und darf deshalb nicht allein zur Reduzierung von Overhead gewählt werden.

---

## Hardware-Ressourcen

Hardware-Ressourcen werden einer Driver Isolation Domain explizit zugeordnet.

Dazu können gehören:

- Devices,
- MMIO-Bereiche,
- Interrupts,
- DMA-Kontexte,
- IOMMU-Domänen,
- Device Queues.

Eine Domain darf nur auf Ressourcen zugreifen, für die sie entsprechende Authority besitzt.

---

## Fehler und Supervision

Die Driver Isolation Domain bildet die primäre Fehlergrenze für User-Mode-Treiber.

```text
Driver Fault
     ↓
Isolation Domain Failure
     ↓
Driver Supervision
     ↓
Restart / Recovery / Disable
```

Andere unabhängige Driver Domains sollen davon unbeeinflusst bleiben.

Die konkrete Restart- und Recovery-Policy wird separat spezifiziert.

---

## Resource Economy

Ressourcenverbrauch wird der Driver Isolation Domain zugerechnet.

Dies umfasst insbesondere:

- CPU,
- Memory,
- Pinned Memory,
- I/O,
- DMA,
- Queue-Ressourcen.

Dadurch können fehlerhafte oder überlastete Treiber innerhalb definierter Ressourcenlimits gehalten werden.

---

## Trust

Treiber innerhalb einer gemeinsamen Isolation Domain müssen miteinander kompatible Trust- und Security-Anforderungen besitzen.

Treiber mit unterschiedlichen oder nicht vereinbaren Trust-Anforderungen dürfen nicht allein aus Performancegründen in derselben Domain zusammengeführt werden.

Hard Security Constraints haben Vorrang vor möglichen Effizienzgewinnen.

---

## Kernel-Mode-Treiber

Kernel-Mode-Treiber laufen nicht innerhalb normaler User-Mode Driver Isolation Domains.

Sie unterliegen aufgrund ihrer Kernel-Ausführung anderen Isolationsmechanismen und strengeren Trust-Anforderungen.

Driver Isolation Domains ersetzen daher nicht die Entscheidung zwischen Kernel- und User-Mode-Treiber.

---

## Architecture Introspection

NovaOS muss Driver Isolation Domains kontrolliert introspektierbar machen.

Erkennbar sein sollen insbesondere:

- Domain Identity,
- enthaltene Treiber,
- zugewiesene Geräte,
- Resource Budgets,
- Trust Context,
- aktueller Lifecycle-Zustand.

---

## Normative Anforderungen

1. NovaOS MUSS Driver Isolation Domains für User-Mode-Treiber unterstützen.
2. Driver Isolation Domains MÜSSEN auf dem allgemeinen Execution-Domain-Modell aufbauen.
3. Eine Driver Isolation Domain MUSS eine eigene Security- und Fehlergrenze bilden.
4. Unabhängige Treiber SOLLEN standardmäßig getrennte Isolation Domains verwenden.
5. Mehrere Treiberkomponenten DÜRFEN bei explizit begründeter enger Kopplung eine Domain teilen.
6. Eine gemeinsame Domain DARF inkompatible Trust- oder Security-Grenzen NICHT zusammenführen.
7. Hardware-Ressourcen MÜSSEN einer Driver Isolation Domain explizit zugeordnet werden.
8. Eine Domain DARF ausschließlich autorisierte Hardware-Ressourcen verwenden.
9. Kommunikation zwischen getrennten Driver Domains MUSS über definierte Interfaces erfolgen.
10. Fehler einer User-Mode Driver Domain SOLLEN auf diese Domain begrenzt bleiben.
11. Driver Domains MÜSSEN in die systemweite Supervision integrierbar sein.
12. Ressourcenverbrauch MUSS der jeweiligen Driver Isolation Domain zurechenbar sein.
13. Kernel-Mode-Treiber DÜRFEN NICHT allein durch Driver Isolation Domains als gleichwertig isoliert betrachtet werden.
14. Driver-Domain-Zuordnung und Ressourcen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Treiberfehler bleiben stärker lokalisiert,
- kompromittierte Treiber besitzen einen begrenzten Wirkungsbereich,
- Driver Restart und Recovery werden erleichtert,
- Ressourcen können pro Driver Domain kontrolliert werden,
- eng gekoppelte Treiber können bei Bedarf gemeinsam ausgeführt werden.

### Negative Konsequenzen

- zusätzliche Execution Domains benötigen Ressourcen,
- Kommunikation zwischen getrennten Treibern verursacht Übergangskosten,
- die Gruppierung von Treiberkomponenten benötigt klare Regeln.

---

## Verworfene Alternativen

### Eine gemeinsame Domain für alle User-Mode-Treiber

Verworfen.

Ein einzelner Fehler könnte zahlreiche unabhängige Treiber beeinflussen.

### Zwingend eine Domain pro Treiberkomponente

Verworfen.

Eng gekoppelte Komponenten können dadurch unnötige Kommunikations- und Ressourcenaufwände erzeugen.

### Eigenes Isolation-Modell ausschließlich für Treiber

Verworfen.

Driver Domains sollen die allgemeinen NovaOS Execution-Domain-, Security- und Resource-Mechanismen wiederverwenden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell`
- `ADR-DRIVER-0002_Driver_Sandboxing`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0002_Prozesse_als_Isolations_und_Ressourcendomänen`
- `ADR-PROCESS-0005_Systemweite_Process_Supervision`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-IO-0006_IO_Resource_Budgets`
- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starker_Isolation`

---

## Zugehörige NPSPECs

- `NPSPEC-DRIVER-DOMAIN-0001`
- `NPSPEC-DRIVER-DOMAIN-ISOLATION-0001`
- `NPSPEC-DRIVER-DOMAIN-GROUPING-0001`
- `NPSPEC-DRIVER-DOMAIN-RESOURCE-0001`
- `NPSPEC-DRIVER-DOMAIN-LIFECYCLE-0001`
- `NPSPEC-DRIVER-DOMAIN-TEST-0001`

---

## Ergebnis

User-Mode-Treiber werden innerhalb expliziter Fehler-, Sicherheits- und Ressourcengrenzen ausgeführt:

```text
Device A → Driver Domain A
Device B → Driver Domain B
Device C → Driver Domain C
               ↓
        NovaOS Supervision
```

Mehrere eng gekoppelte Komponenten können kontrolliert eine Domain teilen, ohne die Isolation unabhängiger Treiber aufzugeben.

Die zentrale Architekturregel lautet:

```text
Share a driver domain only
when sharing the failure boundary is intentional.
```