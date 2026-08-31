# ADR-INTERRUPT-0001 – Einheitliche Interrupt-Controller-Abstraktion

## Status

Angenommen

## Kategorie

Kernel / HAL / Interrupts / Interrupt Controller / Hardwareabstraktion / Plattformportabilität

## Kontext

NovaOS soll unterschiedliche Prozessorarchitekturen und Plattformen unterstützen.

Die zugrunde liegende Interrupt-Hardware unterscheidet sich dabei erheblich.

Beispiele sind:

- x86 PIC,
- x86 Local APIC,
- x86 I/O APIC,
- x86 MSI/MSI-X,
- ARM GIC,
- RISC-V PLIC beziehungsweise zukünftige AIA-Komponenten,
- virtuelle Interrupt Controller von Hypervisoren.

Der generische Kernel darf deshalb nicht direkt von einem bestimmten Interrupt Controller wie APIC oder GIC abhängen.

Ohne gemeinsame Abstraktion würden Komponenten wie:

- Scheduler,
- Timer,
- Gerätetreiber,
- SMP,
- Power Management,
- IRQ-Routing

plattformspezifische Interruptlogik enthalten.

Dies widerspricht der in `ADR-HAL-0001` und `ADR-HAL-0002` festgelegten Trennung zwischen generischem Kernel, Architektur und Plattform.

---

## Entscheidung

NovaOS führt eine **einheitliche Interrupt-Controller-Abstraktion** innerhalb der HAL ein.

Das Grundmodell lautet:

```text
Kernel / Driver / Scheduler
            ↓
     Interrupt Subsystem
            ↓
Interrupt Controller Contract
            ↓
      Platform Provider
            ↓
┌───────────┼────────────┬────────────┐
▼           ▼            ▼            ▼
PIC      APIC/IOAPIC    ARM GIC    RISC-V
                                      PLIC/AIA
```

Generische Kernelkomponenten arbeiten ausschließlich mit semantischen Interruptoperationen.

Register, Controller-IDs und plattformspezifische Routingdetails bleiben innerhalb der zuständigen HAL-/Platform-Implementierung.

---

## Grundprinzip

```text
The kernel handles interrupts.

The platform handles interrupt controllers.
```

Der generische Kernel kennt die Bedeutung eines Interrupts, aber nicht die konkrete Hardwareprogrammierung des Controllers.

---

## Interrupt Controller Contract

Konzeptionell:

```text
InterruptController {
    Initialize()

    Enable(source)
    Disable(source)

    Mask(source)
    Unmask(source)

    Configure(source, configuration)
    Route(source, target)

    Acknowledge()
    EndOfInterrupt(interrupt)

    GetCapabilities()
}
```

Nicht jeder Hardwarecontroller muss jede Operation identisch implementieren.

Nicht unterstützte Funktionen müssen explizit über Capabilities beziehungsweise typisierte Fehler erkennbar sein.

---

## Interrupt Source

Interruptquellen werden über eine gemeinsame logische Identität beschrieben.

Konzeptionell:

```text
InterruptSource {
    id
    type
    controller
    hardwareSource
}
```

Der generische Kernel darf nicht voraussetzen, dass eine logische Interrupt-ID direkt einer:

- PIC IRQ Line,
- APIC Vector Number,
- GIC Interrupt ID,
- PLIC Source ID

entspricht.

---

## Interrupt Identity und Vector

NovaOS trennt:

```text
Interrupt Source
```

von:

```text
CPU Interrupt Vector
```

Eine Hardwarequelle kann durch den Controller auf einen CPU-seitigen Interruptvektor beziehungsweise eine entsprechende Architekturabstraktion abgebildet werden.

Dadurch wird beispielsweise auf x86 vermieden, dass:

```text
IRQ == IDT Vector
```

als systemweite Annahme entsteht.

---

## Interrupt-Typen

Die Abstraktion muss mindestens folgende grundlegende Klassen unterscheiden können:

```text
External
Timer
InterProcessor
MessageSignaled
Platform
Spurious
```

CPU Exceptions werden zwar über die gemeinsame Interrupt-/Exception-Infrastruktur verarbeitet, sind jedoch keine externen Interrupt-Controller-Quellen.

Exceptions und Hardwareinterrupts bleiben deshalb semantisch unterscheidbar.

---

## Controller-Hierarchie

Eine Plattform kann mehrere Interrupt Controller gleichzeitig besitzen.

Beispiel x86:

```text
CPU
 │
Local APIC
 │
 ├── IPI
 ├── Local Timer
 │
 └── External Interrupt
          ↑
       I/O APIC
          ↑
        Device
```

Die Abstraktion darf daher nicht von genau einem globalen Controller ausgehen.

Controller können hierarchisch beziehungsweise kooperativ arbeiten.

---

## Legacy PIC

Der klassische x86 PIC wird als Legacy Interrupt Controller unterstützt.

Auf modernen x86-Systemen soll jedoch APIC-basierter Betrieb bevorzugt werden.

Konzeptionell:

```text
Legacy PC
    ↓
PIC

Modern PC
    ↓
Local APIC + I/O APIC
```

Der PIC darf die generische Kernelarchitektur nicht bestimmen.

---

## Local APIC und I/O APIC

Auf geeigneten x86-Systemen werden:

- Local APIC für lokale CPU-Interruptfunktionen,
- I/O APIC für externe Interruptquellen

über den x86 Platform Provider integriert.

APIC-spezifische Konzepte wie:

- APIC ID,
- Redirection Table,
- Delivery Mode,
- Destination Mode

bleiben außerhalb des generischen Kernelcodes.

---

## ARM GIC

ARM-Plattformen können GIC-Versionen über einen entsprechenden Platform Provider integrieren.

GIC-spezifische Konzepte werden auf die gemeinsamen NovaOS-Semantiken abgebildet.

Der generische Kernel darf keine GIC-spezifischen Register oder Interruptnummern voraussetzen.

---

## RISC-V

RISC-V-Interruptarchitekturen werden über dieselbe semantische Abstraktion integriert.

Dabei kann die konkrete Plattform beispielsweise:

```text
PLIC
```

oder zukünftige:

```text
AIA / IMSIC
```

Mechanismen verwenden.

Die NovaOS-Kernelarchitektur darf nicht dauerhaft an PLIC gebunden werden.

---

## Interrupt Routing

Interrupt Routing wird als explizite Operation behandelt.

Konzeptionell:

```text
Interrupt Source
       ↓
Routing Policy
       ↓
Interrupt Controller
       ↓
Target CPU / CPU Set
```

Die Hardwareabstraktion stellt den Mechanismus bereit.

Die Entscheidung, auf welche CPU ein Interrupt bevorzugt geroutet wird, gehört zur System- beziehungsweise Scheduling-Policy.

Dies folgt der Mechanism-/Policy-Trennung von NovaOS.

---

## CPU Affinity

Interrupts müssen, sofern die Hardware dies unterstützt, an:

- einzelne CPUs,
- CPU-Gruppen,
- geeignete Zielmengen

gebunden werden können.

Dies ist insbesondere relevant für:

- SMP,
- NUMA,
- Netzwerk,
- Storage,
- Echtzeit,
- Cache-Lokalität.

Die tatsächliche Zielauswahl erfolgt oberhalb des Controllers.

---

## NUMA und Lokalität

Interrupt Routing soll die Hardwaretopologie aus dem Hardware System Graph berücksichtigen können.

Beispiel:

```text
NVMe Device
    ↓
PCIe Root Complex
    ↓
NUMA Node 1
    ↓
CPU on NUMA Node 1
```

Der Interrupt Controller stellt dafür die Routingmechanismen bereit.

Die Locality Policy entscheidet über die bevorzugte Platzierung.

---

## Masking

NovaOS unterscheidet zwischen:

```text
Disable
```

und:

```text
Mask
```

soweit die Hardware diese Semantik unterstützt.

`Mask` unterdrückt die Zustellung einer Interruptquelle temporär.

`Disable` kann einen stärkeren Lifecycle-Zustand darstellen.

Controller-Backends müssen ihre konkrete Hardwaresemantik auf die gemeinsamen Operationen abbilden.

---

## Trigger Mode

Externe Interruptquellen können unterschiedliche Triggersemantik besitzen.

Mindestens relevant sind:

```text
Edge
Level
```

Zusätzlich kann Polarität relevant sein:

```text
ActiveHigh
ActiveLow
```

Diese Eigenschaften werden als Konfiguration der Interruptquelle behandelt und nicht im generischen Treibercode durch Controllerregister manipuliert.

---

## Acknowledge und End-of-Interrupt

Controllerabhängige Operationen wie:

```text
Acknowledge
EndOfInterrupt
```

werden über das Interrupt Subsystem abstrahiert.

Ein generischer Interrupt Handler darf keine APIC-, GIC- oder PLIC-spezifischen EOI-Register direkt ansprechen.

---

## Spurious Interrupts

Die Abstraktion muss Controller-spezifische Spurious Interrupts kontrolliert behandeln können.

Ein Spurious Interrupt darf:

- erkannt,
- gezählt,
- diagnostiziert

werden.

Er darf nicht automatisch als regulärer Geräteinterrupt interpretiert werden.

---

## MSI und MSI-X

Message Signaled Interrupts werden als reguläre Interruptquellen in das gemeinsame Modell integriert.

Treiber sollen nicht direkt von einer bestimmten APIC-Implementierung abhängig sein.

Konzeptionell:

```text
Device
   ↓
MSI / MSI-X
   ↓
Interrupt Allocation
   ↓
Interrupt Source
   ↓
Handler
```

Die konkrete Nachrichtencodierung und Controllerprogrammierung übernimmt die Plattform-/PCI-Infrastruktur.

---

## Interrupt Allocation

Interruptvektoren und controllerabhängige Ressourcen werden zentral verwaltet.

Treiber fordern semantisch:

```text
Interrupt Resource
```

an.

Sie reservieren nicht eigenmächtig globale Vektornummern.

Dies verhindert Konflikte zwischen:

- Geräten,
- Timern,
- IPIs,
- Plattforminterrupts,
- Kernelreservierungen.

---

## Inter-Processor Interrupts

SMP-Systeme benötigen Inter-Processor Interrupts.

Konzeptionell:

```text
SendIPI(target, reason)
```

Die tatsächliche Hardwareimplementierung kann beispielsweise:

- Local APIC,
- GIC,
- IMSIC

verwenden.

IPI-Gründe werden semantisch vom Hardwaremechanismus getrennt.

Beispiele:

```text
Reschedule
TLBShootdown
CPUStop
CrossCpuCall
```

---

## Interrupt Handler

Interrupt Controller und Interrupt Handler bleiben getrennte Konzepte.

```text
Interrupt Controller
        ↓
Interrupt Dispatch
        ↓
Registered Handler
```

Der Controller bestimmt die Hardwarezustellung.

Das Kernel Interrupt Subsystem verwaltet die Zuordnung zu Handlern.

---

## Handler Lifecycle

Ein Interrupt Handler muss kontrolliert:

```text
Register
Enable
Handle
Disable
Synchronize
Unregister
```

werden können.

Beim Entfernen eines Treibers muss sichergestellt werden, dass kein bereits laufender Handler anschließend auf freigegebene Ressourcen zugreift.

---

## Hotplug

Die Interrupt-Abstraktion muss mit dem deklarativen Device Lifecycle kompatibel sein.

Beim Entfernen eines Geräts gilt konzeptionell:

```text
Quiesce Device
      ↓
Mask Interrupts
      ↓
Drain / Synchronize Handlers
      ↓
Release Interrupt Resources
      ↓
Remove Device
```

Surprise Removal muss ebenfalls kontrolliert behandelt werden.

---

## Power Management

Interrupt Controller können unterschiedliche Power States besitzen.

Beim Suspend/Resume müssen notwendige Routing- und Controllerzustände:

- erhalten,
- rekonstruiert,
- neu validiert

werden können.

Der generische Kernel darf nicht voraussetzen, dass sämtliche Controllerregister einen Suspend unverändert überstehen.

---

## Virtualisierung

Virtuelle Plattformen können eigene Interrupt Controller bereitstellen.

Diese werden über dieselbe Abstraktion integriert.

Beispiele:

```text
Virtual APIC
Virtual GIC
Virtual PLIC
```

Der Kernel soll dadurch möglichst keine separate virtuelle Interruptarchitektur benötigen.

---

## Prioritäten

Wenn die Hardware Interruptprioritäten unterstützt, müssen diese über semantische Prioritätsklassen abstrahiert werden können.

Der Kernel darf nicht davon ausgehen, dass unterschiedliche Controller dieselbe Anzahl oder Codierung von Prioritätsstufen besitzen.

---

## Echtzeit

Für Echtzeitbetrieb müssen Interrupt Routing und Priorität kontrollierbar sein.

Insbesondere sollen vermeidbare:

- Migrationen,
- Prioritätsänderungen,
- dynamische Neuverteilungen

in entsprechend deterministischen beziehungsweise Echtzeit-Szenarien eingeschränkt werden können.

---

## Deterministic Mode

Im Deterministic Mode müssen relevante Interruptentscheidungen stabilisiert werden können.

Beispielsweise:

```text
fixed routing
fixed vector allocation
fixed priority mapping
```

soweit die Plattform dies unterstützt.

Die Hardwareabstraktion darf keine unnötige adaptive Neuverteilung erzwingen.

---

## Resource Economy

Interruptressourcen sind begrenzt.

Dazu können gehören:

- Vektoren,
- MSI/MSI-X-Einträge,
- Controller Routing Entries,
- CPU-Zielressourcen.

Sie werden deshalb als verwaltete Systemressourcen behandelt.

---

## Sicherheit

Treiber dürfen Interrupt Controller nicht beliebig programmieren.

Ein Treiber erhält nur Autorität über die ihm zugewiesenen Interruptressourcen.

Direkte Manipulation fremder Routing Entries oder Interruptquellen ist nicht zulässig.

Dies gilt besonders für isolierte beziehungsweise User-Mode-Treiber.

---

## Fehlerbehandlung

Controlleroperationen müssen typisierte Fehler liefern können.

Beispiele:

```text
Unsupported
InvalidSource
InvalidTarget
ResourceExhausted
ControllerUnavailable
ConfigurationRejected
RoutingFailed
```

Ein Fehler darf nicht durch willkürliche Controllerannahmen verborgen werden.

---

## Architecture Introspection

NovaOS soll Interrupttopologie und Routing strukturiert darstellen können.

Beispiel:

```text
Interrupt Source:
    NVMe Queue 0

Controller:
    IOAPIC / MSI

Target:
    CPU 4

NUMA Node:
    1

State:
    Enabled
```

Sicherheitskritische Details können abhängig von Berechtigungen gefiltert werden.

---

## Hardware System Graph

Interrupt Controller und relevante Beziehungen werden in den Hardware System Graph integriert.

Beispiel:

```text
Device
  │
  └── InterruptSource
          │
          ▼
    InterruptController
          │
          ▼
         CPU
```

Damit können:

- Topologie,
- Routing,
- NUMA,
- Hotplug,
- Diagnose

gemeinsam betrachtet werden.

---

## Normative Anforderungen

1. NovaOS MUSS Interrupt Controller über eine gemeinsame HAL-Abstraktion ansprechen.
2. Generischer Kernelcode DARF keine APIC-, GIC-, PLIC- oder PIC-spezifischen Register direkt verwenden.
3. Die Abstraktion MUSS mehrere gleichzeitig vorhandene Interrupt Controller unterstützen.
4. Interrupt Source und CPU Interrupt Vector MÜSSEN logisch getrennt sein.
5. CPU Exceptions und externe Hardwareinterrupts MÜSSEN semantisch unterscheidbar bleiben.
6. Controller-Backends MÜSSEN ihre unterstützten Fähigkeiten explizit melden können.
7. Interrupt Routing MUSS über eine gemeinsame semantische Operation möglich sein.
8. Routing-Mechanismus und Routing-Policy MÜSSEN getrennt sein.
9. CPU Affinity MUSS unterstützt werden, sofern die Hardware dies ermöglicht.
10. Edge-/Level-Trigger und relevante Polarität MÜSSEN abstrahiert konfigurierbar sein.
11. Controllerabhängiges Acknowledge und End-of-Interrupt MÜSSEN gekapselt werden.
12. Spurious Interrupts MÜSSEN kontrolliert behandelt werden.
13. MSI und MSI-X MÜSSEN in das gemeinsame Interruptmodell integrierbar sein.
14. Interruptressourcen MÜSSEN zentral verwaltet werden.
15. Treiber DÜRFEN globale Interruptvektoren NICHT eigenmächtig reservieren.
16. Inter-Processor Interrupts MÜSSEN über eine architekturunabhängige Semantik auslösbar sein.
17. Interrupt Handler und Interrupt Controller MÜSSEN getrennte Subsystemverantwortlichkeiten bleiben.
18. Handler MÜSSEN vor ihrer Freigabe mit laufenden Interruptausführungen synchronisiert werden können.
19. Die Interruptarchitektur MUSS Device Hotplug und Surprise Removal unterstützen.
20. Suspend/Resume MUSS Controllerzustände kontrolliert wiederherstellen können.
21. Virtuelle Interrupt Controller MÜSSEN über dieselbe Abstraktion integrierbar sein.
22. Hardwareprioritäten MÜSSEN auf semantische Prioritätsklassen abbildbar sein.
23. Echtzeit- und Deterministic-Mode-Policies MÜSSEN stabiles Interrupt Routing anfordern können.
24. Interruptressourcen MÜSSEN in die systemweite Ressourcenverwaltung integrierbar sein.
25. Treiber DÜRFEN nur ihre autorisierten Interruptressourcen kontrollieren.
26. Controllerfehler MÜSSEN typisiert an höhere Schichten weitergegeben werden können.
27. Interrupt Controller und Routing MÜSSEN in den Hardware System Graph integrierbar sein.
28. Interruptzustände und Routing MÜSSEN kontrolliert introspektierbar sein.
29. Die grundlegende Interruptverarbeitung MUSS vollständig ohne KI funktionieren.
30. Neue Interrupt-Controller-Technologien MÜSSEN durch neue Backends integrierbar sein, ohne die generische Kernelarchitektur zu verändern.

---

## Konsequenzen

### Positive Konsequenzen

- APIC, GIC, PLIC/AIA und zukünftige Controller können hinter derselben Kernelarchitektur verwendet werden.
- Treiber bleiben weitgehend plattformunabhängig.
- SMP-, NUMA- und Echtzeit-Routing können zentral geplant werden.
- MSI/MSI-X und klassische IRQs lassen sich in einem gemeinsamen Modell behandeln.
- virtuelle Plattformen benötigen keine separate Kernelarchitektur.
- Interruptressourcen werden kontrollierbar und introspektierbar.
- zukünftige Hardware kann durch zusätzliche Controller-Backends ergänzt werden.

### Negative Konsequenzen

- plattformspezifische Controllerfunktionen müssen auf gemeinsame Semantik abgebildet werden,
- nicht jede Hardwarefähigkeit besitzt auf allen Plattformen eine direkte Entsprechung,
- Interrupt Routing und Ressourcenverwaltung benötigen zusätzliche zentrale Infrastruktur.

---

## Verworfene Alternativen

### APIC als systemweite Interruptarchitektur

Verworfen.

Dies würde den Kernel unnötig an x86 binden.

### Separate Interruptarchitektur für jede CPU-Architektur

Verworfen.

Zu viele Kernelkomponenten müssten dadurch Architekturunterschiede kennen.

### Direkte Controllerprogrammierung durch Treiber

Verworfen.

Dies verhindert zentrale Ressourcenverwaltung, Isolation und sichere Routing-Policy.

### Globale feste IRQ-Nummern

Verworfen.

Moderne Interruptarchitekturen, MSI/MSI-X und unterschiedliche Plattformen lassen sich damit nicht sauber modellieren.

### Genau ein Interrupt Controller pro System

Verworfen.

Moderne Plattformen verwenden häufig mehrere zusammenwirkende Controller.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0001_Modularer_monolithischer_Kernel_mit_starker_Isolation`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0018_Lokalität_als_Optimierungsziel`
- `ADR-HAL-0001_Einheitliche_Hardware_Abstraction_Layer`
- `ADR-HAL-0002_Plattformabstraktion_für_x86_ARM_und_zukünftige_Architekturen`
- `ADR-HAL-0003_Firmware_über_Platform_Provider_abstrahieren`
- `ADR-HAL-0004_Hardwaretopologie_als_Systemgraph`
- `ADR-HAL-0005_NUMA_als_First_Class_Topologie`
- `ADR-HAL-0007_Hotplug_über_deklarative_Device_Lifecycle_States`

---

## Zugehörige NPSPECs

- `NPSPEC-INTERRUPT-CONTROLLER-0001`
- `NPSPEC-INTERRUPT-SOURCE-0001`
- `NPSPEC-INTERRUPT-ROUTING-0001`
- `NPSPEC-INTERRUPT-VECTOR-0001`
- `NPSPEC-INTERRUPT-HANDLER-0001`
- `NPSPEC-INTERRUPT-IPI-0001`
- `NPSPEC-INTERRUPT-MSI-0001`
- `NPSPEC-INTERRUPT-AFFINITY-0001`
- `NPSPEC-INTERRUPT-PIC-0001`
- `NPSPEC-INTERRUPT-APIC-0001`
- `NPSPEC-INTERRUPT-IOAPIC-0001`
- `NPSPEC-INTERRUPT-GIC-0001`
- `NPSPEC-INTERRUPT-RISCV-0001`
- `NPSPEC-INTERRUPT-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Legacy-PIC-Betrieb,
- Local-APIC-/I/O-APIC-Betrieb,
- ARM-GIC-Backend,
- RISC-V-Interrupt-Backend,
- mehrere Controller innerhalb einer Plattform,
- Edge-triggered Interrupt,
- Level-triggered Interrupt,
- Mask/Unmask,
- Enable/Disable,
- CPU-Affinity,
- Routing auf unterschiedliche CPUs,
- MSI,
- MSI-X,
- Inter-Processor Interrupts,
- Spurious Interrupts,
- Handler-Registrierung und Entfernung,
- Interrupt während Device Hot Removal,
- Suspend/Resume,
- Ressourcenerschöpfung,
- ungültige Interruptquelle,
- Deterministic Mode mit festem Routing,
- virtuelle Interrupt Controller,
- Hardware-System-Graph-Introspection.

---

## Ergebnis

NovaOS besitzt eine gemeinsame Interrupt-Controller-Abstraktion, über die unterschiedliche Hardwarearchitekturen in dasselbe Interrupt Subsystem integriert werden.

Das Modell lautet:

```text
Interrupt Source
       ↓
Interrupt Subsystem
       ↓
Routing / Allocation
       ↓
Interrupt Controller Contract
       ↓
Platform Backend
       ↓
Hardware
```

Damit bleiben Controllerdetails innerhalb der Hardware- und Plattformabstraktion, während Kernel, Scheduler und Treiber mit gemeinsamen semantischen Interruptoperationen arbeiten.

Die zentrale Architekturregel lautet:

```text
Abstract the controller.

Preserve interrupt semantics.

Keep routing policy above hardware.

Never make one platform's interrupt model
the kernel's interrupt model.
```