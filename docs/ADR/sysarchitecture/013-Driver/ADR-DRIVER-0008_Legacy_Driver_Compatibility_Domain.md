# ADR-DRIVER-0008 – Legacy Driver Compatibility Domain

## Status

Angenommen

## Kategorie

Driver / Compatibility / Isolation / Legacy Support / Security

## Kontext

`ADR-DRIVER-0007` legt fest, dass Legacy-Kernel-Treiber nicht direkt in den nativen Nova Kernel geladen werden.

Für die kontrollierte Ausführung solcher Treiber benötigt NovaOS eine dedizierte Umgebung, welche die erwartete Legacy-Treibersemantik bereitstellt und gleichzeitig die nativen Sicherheits- und Ressourcenmodelle von NovaOS durchsetzt.

Diese Umgebung darf kein zweiter Kernel und keine allgemeine Emulation eines fremden Betriebssystems werden.

---

## Entscheidung

NovaOS führt geeignete Legacy-Treiber innerhalb einer isolierten **Legacy Driver Compatibility Domain** aus.

```text
Legacy Driver
      ↓
Legacy Driver Compatibility Domain
      ↓
Compatibility Provider
      ↓
NovaOS Driver / I/O / HAL Contracts
      ↓
Hardware
```

Die Compatibility Domain übersetzt unterstützte Legacy-Treiberoperationen auf native NovaOS-Mechanismen und bildet gleichzeitig eine Sicherheits-, Ressourcen- und Fehlergrenze.

---

## Grundprinzip

```text
Emulate the required driver contract,
not the foreign kernel.
```

---

## Compatibility Domain

Eine Legacy Driver Compatibility Domain baut auf dem allgemeinen NovaOS Execution-Domain- und Driver-Isolation-Modell auf.

Sie kapselt insbesondere:

- Legacy Driver Code,
- Compatibility Provider,
- Legacy Runtime State,
- zugewiesene Device-Ressourcen,
- Resource Budgets,
- Security- und Trust-Kontext.

Legacy-Code erhält dadurch keine direkte Nova-Kernel-Ausführung.

---

## Compatibility Provider

Der Compatibility Provider stellt die unterstützten Teile einer Legacy-Treiber-API bereit und übersetzt sie auf native NovaOS-Mechanismen.

```text
Legacy API
    ↓
Compatibility Provider
    ↓
NovaOS Contracts
```

Nur explizit unterstützte Legacy-Semantik wird bereitgestellt.

Nicht unterstützte oder nicht sicher abbildbare Operationen müssen abgelehnt werden.

---

## Hardwarezugriff

Die Compatibility Domain darf Hardware ausschließlich über explizit zugewiesene Ressourcen verwenden.

Dies betrifft insbesondere:

- MMIO,
- Interrupts,
- DMA,
- IOMMU-Kontexte,
- Device Queues,
- Pinned Memory.

Das ursprüngliche Privilegmodell des Legacy-Treibers erzeugt keine entsprechende NovaOS-Authority.

---

## Isolation

Legacy Driver Compatibility Domains sind von:

- Nova Kernel,
- Anwendungen,
- nativen Driver Domains,
- anderen Compatibility Domains

zu isolieren.

Ein Fehler oder Absturz eines Legacy-Treibers soll grundsätzlich auf seine Compatibility Domain begrenzt bleiben.

Mehrere Legacy-Treiber dürfen nur dann eine Domain teilen, wenn eine gemeinsame Trust- und Fehlergrenze ausdrücklich zulässig ist.

---

## Native I/O-Integration

Nach außen soll die Compatibility Domain native NovaOS-Driver- und I/O-Contracts bereitstellen.

```text
NovaOS I/O Request
        ↓
Compatibility Domain
        ↓
Legacy Driver
        ↓
Device
        ↓
NovaOS Completion
```

Damit müssen Anwendungen und andere Systemkomponenten keine Legacy-Treibersemantik kennen.

---

## Resource Economy

Alle durch Legacy-Treiber verursachten Ressourcen müssen der Compatibility Domain zurechenbar bleiben.

Dazu gehören insbesondere:

- CPU,
- Memory,
- I/O,
- DMA,
- Pinned Memory,
- Queue- und Device-Ressourcen.

Legacy-Kompatibilität darf die systemweite Resource Economy nicht umgehen.

---

## Trust und Security

Die Ausführung eines Legacy-Treibers benötigt eine explizite Trust- und Policy-Entscheidung.

Die Compatibility Domain begrenzt dessen technische Authority, ersetzt aber keine Vertrauensprüfung.

```text
Driver Identity
      ↓
Trust Evaluation
      ↓
Compatibility Policy
      ↓
Granted Capabilities
      ↓
Execution
```

Unbekannte oder nicht sicher abbildbare Anforderungen werden nach dem Fail-Safe-Prinzip abgelehnt.

---

## Lifecycle und Recovery

Die Compatibility Domain wird als überwachte Execution Domain behandelt.

Bei einem Fehler kann NovaOS abhängig vom Device Contract:

- die Domain beenden,
- den Legacy-Treiber neu starten,
- das Gerät zurücksetzen,
- einen anderen Treiber aktivieren,
- das Gerät deaktivieren.

Ein Legacy-Treiberfehler soll keinen Systemneustart erzwingen, sofern die Hardware einen kontrollierten Recovery-Pfad erlaubt.

---

## Native Treiber bleiben bevorzugt

Die Legacy Driver Compatibility Domain ist eine Kompatibilitätslösung und kein gleichwertiger Ersatz für native Nova-Treiber.

```text
Native Nova Driver
       ↓ preferred

Legacy Compatibility Domain
       ↓ fallback
```

Sobald ein geeigneter nativer Treiber verfügbar ist, soll dieser bevorzugt werden.

---

## Normative Anforderungen

1. NovaOS MUSS Legacy-Treiber über isolierte Legacy Driver Compatibility Domains integrieren können.
2. Compatibility Domains MÜSSEN auf dem allgemeinen Execution-Domain- und Driver-Isolation-Modell aufbauen.
3. Legacy-Treiber DÜRFEN NICHT direkt im nativen Nova Kernel ausgeführt werden.
4. Compatibility Provider MÜSSEN Legacy-Semantik auf native NovaOS-Contracts abbilden.
5. Nicht sicher abbildbare Legacy-Operationen MÜSSEN abgelehnt werden.
6. Legacy-Treiber DÜRFEN ausschließlich explizit zugewiesene Hardware-Ressourcen verwenden.
7. DMA und IOMMU MÜSSEN durch die nativen NovaOS-Mechanismen kontrolliert werden.
8. Legacy-Privilegien DÜRFEN NICHT automatisch in NovaOS-Authority übersetzt werden.
9. Compatibility Domains MÜSSEN voneinander und vom Nova Kernel isoliert sein.
10. Native I/O-Nutzer SOLLEN keine Legacy-Treibersemantik kennen müssen.
11. Ressourcenverbrauch MUSS der jeweiligen Compatibility Domain zurechenbar sein.
12. Legacy-Treiber MÜSSEN weiterhin Nova.Trust und den NovaOS-Security-Policies unterliegen.
13. Compatibility Domains MÜSSEN in die systemweite Supervision und Recovery integrierbar sein.
14. Native Nova-Treiber SOLLEN gegenüber Legacy-Compatibility-Treibern bevorzugt werden.
15. Compatibility Provider, Legacy-Treiber, Ressourcen und Domain-Zustand MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Legacy-Hardwareunterstützung ohne fremden Code im Nova Kernel,
- Fehler und Sicherheitsrisiken werden stärker isoliert,
- native NovaOS-I/O- und Resource-Modelle bleiben erhalten,
- Legacy-Treiber können überwacht und neu gestartet werden,
- fremde Kernelarchitekturen bestimmen nicht die interne NovaOS-Architektur.

### Negative Konsequenzen

- Compatibility Provider müssen entwickelt und gepflegt werden,
- nicht jede Legacy-Treibersemantik kann sicher unterstützt werden,
- zusätzliche Domain- und Übersetzungskosten entstehen.

---

## Verworfene Alternativen

### Legacy-Treiber direkt im Nova Kernel

Verworfen.

Dies widerspricht der nativen Isolation und würde fremden Kernelcode in die NovaOS Trusted Computing Base aufnehmen.

### Vollständigen fremden Kernel pro Treiber bereitstellen

Verworfen.

NovaOS benötigt nur die erforderliche Treiberkompatibilität und keine vollständige fremde Betriebssystemumgebung.

### Eine globale Compatibility Domain für alle Legacy-Treiber

Verworfen.

Ein einzelner fehlerhafter Treiber könnte dadurch unnötig andere Legacy-Treiber beeinflussen.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-DRIVER-0001_Hybrides_Kernel_und_User_Mode_Drivermodell`
- `ADR-DRIVER-0002_Driver_Sandboxing`
- `ADR-DRIVER-0003_Driver_Isolation_Domains`
- `ADR-DRIVER-0004_Driver_Capability_Model`
- `ADR-DRIVER-0007_Legacy_Driver_nicht_direkt_in_den_Nova_Kernel_laden`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-PROCESS-0005_Systemweite_Process_Supervision`
- `ADR-HAL-0006_DMA_und_IOMMU_als_kontrollierte_Systemressourcen`
- `ADR-IO-0002_Einheitliches_IO_Request_Modell`

---

## Zugehörige NPSPECs

- `NPSPEC-DRIVER-LEGACY-DOMAIN-0001`
- `NPSPEC-DRIVER-LEGACY-PROVIDER-0001`
- `NPSPEC-DRIVER-LEGACY-API-0001`
- `NPSPEC-DRIVER-LEGACY-ISOLATION-0001`
- `NPSPEC-DRIVER-LEGACY-LIFECYCLE-0001`
- `NPSPEC-DRIVER-LEGACY-TEST-0001`

---

## Ergebnis

Legacy-Treiber erhalten in NovaOS eine kontrollierte Kompatibilitätsumgebung:

```text
Legacy Driver
      ↓
Compatibility Domain
      ↓
Native NovaOS Contracts
      ↓
HAL / I/O
      ↓
Hardware
```

Die Compatibility Domain stellt nur die benötigte Legacy-Treibersemantik bereit und verhindert, dass daraus eine fremde Kernelarchitektur innerhalb des Nova Kernels entsteht.