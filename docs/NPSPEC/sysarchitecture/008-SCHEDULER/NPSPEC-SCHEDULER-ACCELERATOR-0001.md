# NPSPEC-SCHEDULER-ACCELERATOR-0001 – Nova Accelerator-Aware Scheduling

## Status

Entwurf

## Kategorie

Scheduler / Accelerator / Heterogeneous Compute / Resource Economy

## Zweck

NovaOS definiert Accelerator-aware Scheduling für Tasks, deren Ausführung auf spezialisierten Recheneinheiten wie GPU, NPU oder anderen Compute Accelerators möglich oder erforderlich ist.

```text
Compute Task
     ↓
Execution Requirements
     ↓
Available Providers
     ↓
CPU / GPU / NPU / Accelerator
     ↓
Execution
```

Ziel ist die Auswahl einer geeigneten Recheneinheit anhand tatsächlicher Anforderungen statt fester Hardwarezuordnung.

## Grundprinzip

Ein Accelerator wird als ausführender Compute Provider behandelt.

```text
Operation
   ↓
Execution Contract
   ↓
Compatible Providers
   ↓
Cost Evaluation
   ↓
Selected Provider
```

Dabei gilt:

```text
Accelerator Available ≠ Accelerator Beneficial
```

Die Nutzung eines Accelerators muss gegenüber CPU-Ausführung einen ausreichenden Vorteil besitzen oder explizit erforderlich sein.

## Accelerator-Modell

NovaOS kann unterschiedliche Compute Provider verwalten.

Beispiele:

```text
CPU
GPU
NPU
DSP
FPGA
Dedicated Compute Engine
Virtual Accelerator
Remote Accelerator
```

Herstellerspezifische Eigenschaften werden über standardisierte Capability- und Provider-Informationen abstrahiert.

## Provider Capabilities

Ein Accelerator beschreibt seine Fähigkeiten strukturiert.

Beispiele:

```text
Supported Operations
Supported Data Types
Memory Capacity
Memory Model
Parallelism
Performance Characteristics
Energy Characteristics
Determinism
Security Properties
Driver State
```

Nur kompatible Provider dürfen für eine Operation berücksichtigt werden.

## Provider-Auswahl

Die Auswahl erfolgt grundsätzlich:

```text
Hard Requirements
       ↓
Compatible Providers
       ↓
Data Location
       ↓
Transfer Cost
       ↓
Performance / Latency
       ↓
Energy / Thermal / Load
       ↓
Selected Provider
```

Hard Requirements aus dem Execution Contract haben Vorrang.

## Datenlokalität

Accelerator Scheduling muss berücksichtigen, wo Eingabe- und Ausgabedaten liegen.

```text
Data @ CPU Memory
       ↓
Transfer
       ↓
Accelerator Memory
       ↓
Compute
```

Bei großen Datenmengen kann die Übertragung teurer sein als die eigentliche Berechnung.

Daher gilt:

```text
Compute Cost + Transfer Cost = Effective Cost
```

## Zero-Copy

Wenn Hardware und Sicherheitsmodell es erlauben, darf NovaOS Zero-Copy oder Shared-Memory-Mechanismen verwenden.

```text
Shared Buffer
   ↙     ↘
 CPU     GPU
```

Zero-Copy ist eine Optimierung und keine Voraussetzung für Accelerator-Ausführung.

## Queueing

Accelerators können eigene Execution Queues besitzen.

```text
Tasks
  ↓
Accelerator Queue
  ↓
Device Scheduler
  ↓
Execution
```

Der Nova Scheduler koordiniert systemweite Anforderungen mit gerätespezifischem Scheduling.

## Parallelität

Eine Operation darf auf mehrere Compute Provider verteilt werden, wenn sie semantisch teilbar ist.

```text
Compute Operation
      ↓
Partition
   ↙   ↓   ↘
 CPU  GPU  GPU
   ↘   ↓   ↙
    Result
```

Die Aufteilung darf nur erfolgen, wenn Synchronisations- und Transferkosten den erwarteten Vorteil nicht aufheben.

## Ressourcenbudgets

Accelerator-Ausführung kann Ressourcen reservieren:

```text
Compute Time
Device Memory
Memory Bandwidth
Transfer Bandwidth
Power Budget
Queue Capacity
```

Diese Ressourcen werden in die systemweite Resource Economy integriert.

## Realtime

Accelerators dürfen für Realtime-Ausführung verwendet werden, wenn ihre zeitlichen Eigenschaften ausreichend bekannt und kontrollierbar sind.

```text
Realtime Contract
       ↓
Bounded Accelerator Behaviour?
       ↓
Yes → Eligible
No  → Reject for Hard Realtime
```

Unvorhersehbare Queue- oder Transferlatenzen dürfen nicht als garantierte Realtime-Kapazität behandelt werden.

## Energie und Thermal

Accelerator-Auswahl darf Energieverbrauch und thermische Belastung berücksichtigen.

```text
Performance
+
Energy Cost
+
Thermal Cost
=
Effective Provider Suitability
```

Ein schnellerer Accelerator ist nicht automatisch die beste Wahl.

## Fallback

Ist ein bevorzugter Accelerator nicht verfügbar, darf NovaOS einen kompatiblen alternativen Provider verwenden.

```text
Preferred GPU
     ↓ unavailable
Compatible CPU
     ↓
Fallback Execution
```

Ein `Forced Provider` darf jedoch nicht automatisch ersetzt werden.

## Failure

Accelerator-Ausfälle müssen kontrolliert behandelt werden.

```text
Device Failure
     ↓
Task State Evaluation
     ↓
Retry / Fallback / Fail / Recover
```

Eine unbekannte Ausführung darf nicht automatisch als erfolgreich wiederholt werden, wenn dadurch Seiteneffekte dupliziert werden könnten.

## Normative Anforderungen

1. NovaOS MUSS unterschiedliche Compute Provider modellieren können.
2. Accelerator Capabilities MÜSSEN strukturiert beschreibbar sein.
3. Hard Requirements MÜSSEN vor Provider-Optimierungen geprüft werden.
4. Nicht kompatible Provider DÜRFEN NICHT ausgewählt werden.
5. Datenübertragungskosten MÜSSEN bei der Provider-Auswahl berücksichtigt werden können.
6. Datenlokalität SOLL bei Accelerator Scheduling berücksichtigt werden.
7. Zero-Copy SOLL verwendet werden können, wenn Hardware und Sicherheitsmodell dies erlauben.
8. Accelerator-Ressourcen MÜSSEN in die Resource Economy integrierbar sein.
9. Realtime-Garantien DÜRFEN nur auf ausreichend begrenztem Accelerator-Verhalten beruhen.
10. Energie-, Thermal- und Lastzustände SOLLEN bei der Provider-Auswahl berücksichtigt werden.
11. Ein explizit erzwungener Provider DARF NICHT stillschweigend ersetzt werden.
12. Provider-Auswahl, Queueing und Fallback-Entscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-HETEROGENEOUS-0001`
- `NPSPEC-SCHEDULER-ENERGY-0001`
- `NPSPEC-SCHEDULER-THERMAL-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-ZEROCOPY-0001`
- `ADR-SCHED-0012`

## Ergebnis

```text
Compute Operation
       ↓
Execution Contract
       ↓
Provider Capability Filter
       ↓
Data + Transfer + Resource Cost
       ↓
Performance + Energy + Thermal
       ↓
CPU / GPU / NPU / Accelerator
       ↓
Execution
```

NovaOS erhält damit ein Accelerator-aware Scheduling-Modell, das spezialisierte Recheneinheiten als systemweite Compute Provider behandelt und ihre Auswahl anhand von Capabilities, Datenlokalität, Ressourcenbedarf und Execution Contracts steuert.