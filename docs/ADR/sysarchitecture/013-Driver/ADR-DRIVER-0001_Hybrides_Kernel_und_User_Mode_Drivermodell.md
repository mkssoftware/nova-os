# ADR-DRIVER-0001 – Hybrides Kernel- und User-Mode-Drivermodell

## Status

Angenommen

## Kategorie

Driver / Kernel Architecture / Isolation / Security / Reliability

## Kontext

Treiber benötigen je nach Hardwareklasse sehr unterschiedliche Privilegien.

Ein vollständig im Kernel ausgeführtes Treibermodell bietet direkten Hardwarezugriff und geringe Übergangskosten, erhöht jedoch die Auswirkungen von Treiberfehlern auf das Gesamtsystem.

Ein ausschließliches User-Mode-Modell verbessert Isolation und Wiederherstellbarkeit, ist jedoch für bestimmte hardwarenahe, bootkritische oder extrem latenzkritische Funktionen ungeeignet.

NovaOS benötigt deshalb ein Modell, das beide Ausführungsformen kontrolliert kombiniert.

---

## Entscheidung

NovaOS verwendet ein **hybrides Kernel- und User-Mode-Drivermodell**.

```text
Driver Architecture
 ├── Kernel-Mode Driver
 └── User-Mode Driver
```

User Mode ist für geeignete Treiber grundsätzlich zu bevorzugen.

Kernel Mode wird verwendet, wenn technische Anforderungen privilegierten oder besonders hardwarenahen Code notwendig machen.

---

## Grundprinzip

```text
User Mode where practical.

Kernel Mode where necessary.
```

---

## Kernel-Mode-Treiber

Kernel-Mode-Treiber sind für Funktionen vorgesehen, die privilegierten Hardwarezugriff oder besonders enge Kernelintegration benötigen.

Dazu können insbesondere gehören:

- Interrupt-Controller,
- Timer,
- IOMMU,
- grundlegende Bus- und Plattformtreiber,
- frühe Boot- und Storage-Pfade,
- bestimmte Realtime- oder Performance-kritische Komponenten.

Kernel Mode darf nicht allein aus historischen oder bequemeren Implementierungsgründen gewählt werden.

---

## User-Mode-Treiber

Geeignete Gerätetreiber sollen außerhalb des Kernel-Adressraums in isolierten Execution Domains ausgeführt werden.

```text
Application
     ↓
I/O Infrastructure
     ↓
User-Mode Driver
     ↓
Controlled Hardware Access
```

Ein Fehler in einem solchen Treiber soll dadurch möglichst auf dessen Execution Domain begrenzt bleiben.

---

## Kontrollierter Hardwarezugriff

User-Mode-Treiber erhalten keinen allgemeinen Zugriff auf physische Hardware oder Kernel-Speicher.

Benötigte Ressourcen werden explizit autorisiert und zugewiesen.

Dazu können gehören:

- MMIO-Bereiche,
- I/O-Ressourcen,
- Interrupts,
- DMA-Kontexte,
- IOMMU-Domänen,
- Device Queues.

```text
Driver Authority
      ↓
Assigned Resources
      ↓
Device Access
```

Die Zuweisung folgt dem Capability-, Security- und Resource-Modell von NovaOS.

---

## Gemeinsames Drivermodell

Kernel- und User-Mode-Treiber sollen dieselben grundlegenden Driver Contracts verwenden.

Gemeinsam bleiben insbesondere:

- Device Identity,
- Driver Identity,
- unterstützte Capabilities,
- I/O Request Semantik,
- Resource Contracts,
- Lifecycle,
- Trust-Anforderungen.

Die Ausführungsdomäne ist damit eine Eigenschaft der Treiberimplementierung und nicht Grundlage zweier vollständig getrennter Treiberarchitekturen.

---

## I/O-Integration

Treiber integrieren sich in das einheitliche NovaOS-I/O-Modell.

```text
I/O Request
     ↓
Driver
     ↓
Device
     ↓
Completion
```

Soweit geeignet verwenden beide Treibertypen:

- Completion-Based I/O,
- Shared Buffers,
- Scatter/Gather,
- DMA,
- I/O Resource Budgets,
- QoS und Deadlines.

---

## Fehlerisolation und Recovery

User-Mode-Treiber sollen bei Fehlern unabhängig vom Kernel beendet und neu gestartet werden können, sofern Hardwarezustand und Driver Contract dies erlauben.

```text
Driver Failure
     ↓
Isolation
     ↓
Driver Restart
     ↓
Device Recovery
```

Kernel-Mode-Treiber besitzen aufgrund ihrer Privilegien ein höheres Systemrisiko und müssen entsprechend strengeren Anforderungen unterliegen.

---

## Trust und Security

Treiber sind sicherheitskritische Systemkomponenten und werden in Nova.Trust integriert.

Die Entscheidung, einen Treiber zu laden, kann insbesondere berücksichtigen:

```text
Identity
   ↓
Signature
   ↓
Provenance
   ↓
Permission
   ↓
Device Capability
   ↓
Audit
```

Kernel-Mode-Ausführung erfordert eine höhere Vertrauensstufe als gewöhnliche User-Mode-Ausführung.

---

## Auswahl des Ausführungsmodus

Die Entscheidung zwischen Kernel und User Mode erfolgt anhand expliziter Driver Contracts und Systemanforderungen.

Relevante Kriterien können sein:

- benötigte Privilegien,
- Boot-Abhängigkeit,
- Interrupt-Anforderungen,
- Realtime-Anforderungen,
- Performance,
- Isolation,
- Hardwareeigenschaften.

Die konkrete Klassifikation einzelner Treiberklassen wird in NPSPECs definiert.

---

## Normative Anforderungen

1. NovaOS MUSS Kernel- und User-Mode-Treiber unterstützen.
2. User Mode SOLL für geeignete Treiber bevorzugt werden.
3. Kernel Mode DARF nur verwendet werden, wenn technische Anforderungen ihn rechtfertigen.
4. Kernel- und User-Mode-Treiber SOLLEN gemeinsame Driver Contracts verwenden.
5. User-Mode-Treiber MÜSSEN in isolierten Execution Domains ausgeführt werden können.
6. User-Mode-Treiber DÜRFEN keinen uneingeschränkten physischen Speicher- oder Hardwarezugriff besitzen.
7. Hardware-Ressourcen MÜSSEN explizit autorisiert und dem Treiber zugewiesen werden.
8. DMA von User-Mode-Treibern MUSS in die DMA-/IOMMU-Architektur integriert werden.
9. Treiber SOLLEN das einheitliche I/O Request- und Completion-Modell verwenden.
10. Treiberressourcen MÜSSEN der Resource Economy zurechenbar sein.
11. User-Mode-Treiber SOLLEN unabhängig restartbar sein, sofern der Device Contract dies erlaubt.
12. Kernel-Mode-Treiber MÜSSEN strengeren Trust- und Security-Anforderungen unterliegen können.
13. Die Wahl des Ausführungsmodus MUSS durch explizite technische Anforderungen begründbar sein.
14. Driver Identity, Ausführungsmodus und zugewiesene Hardware-Ressourcen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- bessere Fehlerisolation für viele Treiber,
- geringere Kernel-Angriffsfläche,
- restartbare User-Mode-Treiber,
- Kernel Mode bleibt für hardwarenahe Anforderungen verfügbar,
- gemeinsames I/O- und Ressourcenmodell für beide Varianten.

### Negative Konsequenzen

- User-Mode-Treiber benötigen kontrollierte Kernel-/Hardware-Schnittstellen,
- Übergänge zwischen Execution Domains können zusätzliche Kosten verursachen,
- Treiber müssen nach Eignung für Kernel oder User Mode klassifiziert werden.

---

## Verworfene Alternativen

### Alle Treiber im Kernel

Verworfen.

Treiberfehler würden unnötig große Teile des Systems gefährden.

### Alle Treiber im User Mode

Verworfen.

Bestimmte bootkritische, privilegierte und hardwarenahe Funktionen benötigen eine engere Kernelintegration.

### Zwei vollständig getrennte Treiberarchitekturen

Verworfen.

Kernel- und User-Mode-Treiber sollen dieselben grundlegenden Device-, I/O- und Resource-Contracts verwenden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starker_Isolation`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`
- `ADR-IO-0001_Completion_Based_Asynchronous_IO`
- `ADR-IO-0002_Einheitliches_IO_Request_Modell`
- `ADR-IO-0006_IO_Resource_Budgets`

---

## Zugehörige NPSPECs

- `NPSPEC-DRIVER-MODEL-0001`
- `NPSPEC-DRIVER-KERNELMODE-0001`
- `NPSPEC-DRIVER-USERMODE-0001`
- `NPSPEC-DRIVER-HARDWARE-ACCESS-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `NPSPEC-DRIVER-TRUST-0001`
- `NPSPEC-DRIVER-TEST-0001`

---

## Ergebnis

NovaOS verbindet die Performance und Hardware-Nähe von Kernel-Treibern mit der Isolation von User-Mode-Treibern:

```text
               Driver Model
                /        \
       User Mode        Kernel Mode
           ↓                 ↓
      Isolation        Privileged Access
            \               /
             Device / HAL
```

Die zentrale Architekturregel lautet:

```text
Keep drivers isolated when possible.

Grant kernel privilege only when necessary.
```