# ADR-DRIVER-0007 – Legacy Driver nicht direkt in den Nova Kernel laden

## Status

Angenommen

## Kategorie

Driver / Compatibility / Kernel Security / Isolation / Legacy Support

## Kontext

NovaOS soll vorhandene Hardware möglichst breit unterstützen. Dafür kann die Nutzung bestehender Treiber aus anderen Betriebssystem- oder Treiberökosystemen sinnvoll sein.

Legacy-Treiber wurden jedoch für andere Kernelarchitekturen, ABIs, Sicherheitsmodelle, Speicherverwaltungen und I/O-Modelle entwickelt.

Das direkte Laden solcher Binärtreiber in den Nova Kernel würde fremde Annahmen und privilegierten Code unmittelbar in die vertrauenswürdigste Ausführungsdomäne von NovaOS übernehmen.

Dies widerspricht der NovaOS-Architektur aus expliziten Contracts, Driver Sandboxing, Capability-basierter Authority und starker Isolation.

---

## Entscheidung

NovaOS lädt **Legacy-Treiber grundsätzlich nicht direkt in den nativen Nova Kernel**.

```text
Legacy Driver
     ↓
Compatibility Environment
     ↓
Nova Driver / I/O Contracts
     ↓
HAL
     ↓
Hardware
```

Legacy-Treiber müssen über eine kontrollierte Compatibility- oder Isolation-Schicht ausgeführt werden.

Native Nova-Treiber bleiben der bevorzugte Treiberpfad.

---

## Grundprinzip

```text
Legacy compatibility must not become
native kernel authority.
```

---

## Keine fremden Kernel-ABIs im Nova Kernel

NovaOS implementiert keine fremde Kernel-ABI mit dem Ziel, beliebige Legacy-Kernel-Treiber direkt im Nova Kernel auszuführen.

Insbesondere dürfen fremde Erwartungen an:

- Kernelstrukturen,
- Speicherverwaltung,
- Interruptmodell,
- Synchronisation,
- DMA,
- I/O-Lifecycle,
- Security-Modell

nicht zu verbindlichen internen Eigenschaften des Nova Kernels werden.

Die native Nova-Kernelarchitektur bleibt unabhängig von Legacy-Kompatibilität.

---

## Compatibility Environment

Legacy-Treiber können durch spezialisierte Compatibility Provider unterstützt werden.

```text
Legacy Driver API
       ↓
Compatibility Provider
       ↓
NovaOS Contracts
       ↓
Native Mechanisms
```

Der Provider übersetzt unterstützte Legacy-Semantik auf kontrollierte NovaOS-Mechanismen.

Die konkrete Architektur solcher Provider wird separat spezifiziert.

---

## Isolation

Legacy-Treiber sollen soweit technisch möglich außerhalb des nativen Kernel-Adressraums ausgeführt werden.

Sie erhalten eine begrenzte Isolation Domain und ausschließlich die für das zugewiesene Gerät erforderlichen Ressourcen.

Ein Legacy-Treiber darf nicht aufgrund seines ursprünglichen Privilegmodells automatisch entsprechende NovaOS-Privilegien erhalten.

---

## Hardwarezugriff

Hardwarezugriffe von Legacy-Treibern müssen über kontrollierte NovaOS-Ressourcenzuweisungen erfolgen.

Dies betrifft insbesondere:

- MMIO,
- Interrupts,
- DMA,
- IOMMU,
- Device Queues,
- Pinned Memory.

Die Compatibility-Schicht darf Driver Sandboxing und das Driver Capability Model nicht umgehen.

---

## Native Alternative

Wenn ein geeigneter nativer Nova-Treiber vorhanden ist, soll dieser gegenüber einem Legacy-Treiber bevorzugt werden.

```text
Native Nova Driver
        ↓ preferred

Legacy Compatibility Driver
        ↓ fallback
```

Legacy-Unterstützung dient primär Migration, Hardwarekompatibilität und Übergangsphasen und soll nicht die langfristige native Driver Architecture bestimmen.

---

## Nicht unterstützbare Legacy-Treiber

Nicht jeder Legacy-Treiber kann sicher oder sinnvoll abstrahiert werden.

Benötigt ein Treiber unkontrollierten Zugriff auf fremde Kernelstrukturen oder andere nicht sicher abbildbare Mechanismen, darf NovaOS seine Ausführung ablehnen.

```text
Compatibility possible?
   ├── Yes → Isolated Compatibility
   └── No  → Reject
```

Kompatibilität darf nicht durch Aufweichung fundamentaler Sicherheitsgrenzen erzwungen werden.

---

## Trust und Security

Legacy-Treiber unterliegen weiterhin Nova.Trust und den allgemeinen Security Policies.

Eine gültige Legacy-Signatur oder Herkunft allein berechtigt nicht zur Ausführung mit Nova-Kernel-Privilegien.

Legacy-Kompatibilität darf Hard Constraints wie Security, Data Sovereignty oder Resource Limits nicht überschreiben.

---

## Normative Anforderungen

1. NovaOS DARF Legacy-Kernel-Treiber grundsätzlich NICHT direkt in den nativen Nova Kernel laden.
2. Legacy-Treiber MÜSSEN über kontrollierte Compatibility- oder Isolation-Mechanismen integriert werden.
3. NovaOS DARF seine interne Kernelarchitektur NICHT von einer fremden Legacy-Kernel-ABI abhängig machen.
4. Legacy-Treiber SOLLEN außerhalb des nativen Kernel-Adressraums ausgeführt werden.
5. Legacy-Treiber MÜSSEN dem Driver Capability Model unterliegen.
6. Hardware-Ressourcen MÜSSEN explizit zugewiesen werden.
7. DMA und IOMMU MÜSSEN über die nativen NovaOS-Sicherheitsmechanismen kontrolliert werden.
8. Legacy-Kompatibilität DARF Driver Sandboxing NICHT umgehen.
9. Native Nova-Treiber SOLLEN gegenüber Legacy-Treibern bevorzugt werden.
10. Nicht sicher abbildbare Legacy-Semantik MUSS abgelehnt werden können.
11. Legacy-Signaturen DÜRFEN NICHT automatisch Nova-Kernel-Authority verleihen.
12. Security-, Trust-, Data-Sovereignty- und Resource-Constraints MÜSSEN auch für Legacy-Treiber gelten.
13. Compatibility Provider MÜSSEN ihre unterstützte Legacy-Semantik explizit deklarieren.
14. Die Verwendung eines Legacy-Treibers und seines Compatibility Providers MUSS introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Legacy-Unterstützung gefährdet nicht die native Kernelarchitektur,
- fremder Kernelcode erhält keine automatische Nova-Kernel-Authority,
- bestehende Hardware kann trotzdem über Compatibility Provider unterstützt werden,
- NovaOS bleibt langfristig unabhängig von fremden Treiber-ABIs,
- native Driver Contracts bleiben die maßgebliche Architektur.

### Negative Konsequenzen

- nicht alle Legacy-Treiber werden kompatibel sein,
- Compatibility Provider verursachen zusätzlichen Entwicklungsaufwand,
- bestimmte Legacy-Treiber können zusätzliche Laufzeitkosten verursachen.

---

## Verworfene Alternativen

### Fremde Kernel-Treiber direkt in Nova Kernel laden

Verworfen.

Dies würde fremden privilegierten Code und fremde Kernelannahmen unmittelbar in die NovaOS Trusted Computing Base aufnehmen.

### Vollständige fremde Kernel-ABI als Nova-Kernel-Schnittstelle

Verworfen.

Dadurch würde die langfristige NovaOS-Architektur von Legacy-Designentscheidungen abhängig.

### Legacy-Treiber vollständig verbieten

Verworfen.

Kontrollierte Legacy-Kompatibilität kann insbesondere während der Einführung von NovaOS die Hardwareunterstützung erheblich verbessern.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starker_Isolation`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell`
- `ADR-DRIVER-0002_Driver_Sandboxing`
- `ADR-DRIVER-0003_Driver_Isolation_Domains`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`

---

## Zugehörige NPSPECs

- `NPSPEC-DRIVER-LEGACY-0001`
- `NPSPEC-DRIVER-LEGACY-COMPAT-0001`
- `NPSPEC-DRIVER-LEGACY-ISOLATION-0001`
- `NPSPEC-DRIVER-LEGACY-RESOURCE-0001`
- `NPSPEC-DRIVER-LEGACY-SECURITY-0001`
- `NPSPEC-DRIVER-LEGACY-TEST-0001`

---

## Ergebnis

Legacy-Treiber werden als Kompatibilitätskomponenten und nicht als native Bestandteile des Nova Kernels behandelt:

```text
Legacy Driver
      ↓
Isolated Compatibility
      ↓
NovaOS Driver Contracts
      ↓
Native HAL / I/O
      ↓
Hardware
```

Damit kann NovaOS bestehende Treiberökosysteme nutzen, ohne deren Kernelarchitektur und Sicherheitsmodell in den eigenen Kernel zu übernehmen.