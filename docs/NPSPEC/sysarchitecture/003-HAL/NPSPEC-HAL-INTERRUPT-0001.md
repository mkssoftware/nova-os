# NPSPEC-HAL-INTERRUPT-0001 – Nova Interrupt Abstraction

## Status

Angenommen

## Kategorie

HAL / Interrupts / CPU / Platform

## Zweck

NovaOS abstrahiert hardwareabhängige Interrupt-Mechanismen hinter einer einheitlichen HAL-Schnittstelle.

```text
Hardware Event
      ↓
Interrupt Controller
      ↓
HAL Interrupt Layer
      ↓
Kernel / Driver
```

## Grundprinzip

Interrupt-Quellen und Interrupt-Controller werden logisch von ihrer konkreten Hardwareimplementierung getrennt.

```text
Interrupt Source
       ↓
Logical Interrupt
       ↓
HAL
       ↓
APIC / IOAPIC / MSI / MSI-X / andere
```

## Interrupt-Modell

Ein Interrupt beschreibt mindestens:

```text
InterruptID
Source
Vector
Target
Priority
Trigger Mode
State
```

Typische Zustände:

```text
Disabled
Enabled
Pending
Active
Masked
```

## Interrupt-Controller

Die HAL kapselt unterschiedliche Controller hinter gemeinsamen Mechanismen.

Beispiele:

```text
Local APIC
IOAPIC
Legacy PIC
Virtual Interrupt Controller
Platform-specific Controller
```

Höhere Kernel-Komponenten sollen nicht direkt von einem bestimmten Controller abhängen.

## Routing

Interrupts müssen kontrolliert zu CPUs geroutet werden können.

```text
Device
  ↓
Interrupt
  ↓
Interrupt Controller
  ↓
Target CPU
```

Die HAL stellt den Routing-Mechanismus bereit.

Die Entscheidung über CPU-Affinität oder Lastverteilung liegt außerhalb der HAL.

## MSI / MSI-X

Message Signaled Interrupts sollen über dieselbe logische Interrupt-Abstraktion nutzbar sein.

```text
Device
   ↓
MSI / MSI-X
   ↓
Logical Interrupt
   ↓
Handler
```

## Interrupt Handler

Interrupt-Handler müssen registriert und entfernt werden können.

```text
Register
   ↓
Enable
   ↓
Handle
   ↓
Acknowledge
   ↓
Disable / Remove
```

Handler sollen nur unmittelbar notwendige Arbeit im Interrupt-Kontext ausführen.

Aufwendige Verarbeitung soll in geeignete nachgelagerte Ausführungskontexte verschoben werden.

## Maskierung

Interrupts müssen kontrolliert:

```text
Enable
Disable
Mask
Unmask
Acknowledge
```

werden können.

Der tatsächliche Mechanismus bleibt plattformspezifisch.

## Multiprocessing

Interrupts müssen auf Mehrprozessorsystemen gezielt CPUs zugeordnet werden können.

Die HAL unterstützt außerdem Inter-Processor Interrupts:

```text
CPU A
  ↓ IPI
CPU B
```

## Fehlerverhalten

Unbekannte oder unerwartete Interrupts dürfen das System nicht unkontrolliert destabilisieren.

NovaOS muss solche Ereignisse diagnostizierbar machen und sicher behandeln.

## Normative Anforderungen

1. NovaOS MUSS Hardware-Interrupts über eine gemeinsame HAL-Schnittstelle abstrahieren.
2. Interrupt-Controller MÜSSEN hinter plattformspezifischen Implementierungen gekapselt werden.
3. Interrupts MÜSSEN eindeutig identifizierbar sein.
4. Handler MÜSSEN kontrolliert registriert und entfernt werden können.
5. Interrupts MÜSSEN maskiert und demaskiert werden können.
6. Interrupt-Routing zu CPUs MUSS unterstützt werden.
7. MSI und MSI-X SOLLEN über das allgemeine Interrupt-Modell integrierbar sein.
8. Inter-Processor Interrupts MÜSSEN auf Multiprozessorsystemen unterstützt werden.
9. Die HAL DARF keine Scheduling- oder Lastverteilungspolitik festlegen.
10. Interrupt-Handler SOLLEN ihre Arbeit im Interrupt-Kontext minimieren.
11. Unerwartete Interrupts MÜSSEN kontrolliert behandelt und diagnostizierbar sein.
12. Nicht unterstützte Interrupt-Funktionen MÜSSEN eindeutig als nicht verfügbar gemeldet werden.

## Abhängigkeiten

- `NPSPEC-HAL-0001`
- `NPSPEC-HAL-PLATFORM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `ADR-ARCH-0002_Mechanism_und_Policy`
- `ADR-INTERRUPT-0001`
- `ADR-INTERRUPT-0002`
- `ADR-INTERRUPT-0003`
- `ADR-INTERRUPT-0004`
- `ADR-INTERRUPT-0005`

## Ergebnis

```text
Hardware Interrupt
       ↓
Platform Controller
       ↓
Nova HAL Interrupt
       ↓
Kernel / Driver Handler
```

NovaOS erhält damit eine einheitliche Interrupt-Architektur, die unabhängig vom konkreten Interrupt-Controller und der zugrunde liegenden Plattform verwendet werden kann.