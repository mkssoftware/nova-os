# NPSPEC-IO-QOS-0001 – Nova I/O Quality of Service

## Status

Entwurf

## Kategorie

I/O / QoS / Scheduling / Resource Management

## Zweck

NovaOS definiert ein einheitliches Quality-of-Service-Modell für I/O-Operationen.

```text
IORequest
   ↓
QoS Requirements
   ↓
I/O Scheduler
   ↓
Provider
```

QoS beschreibt Anforderungen und Grenzen für Latenz, Durchsatz, Priorität und Ressourcenverbrauch einer I/O-Operation.

## Grundprinzip

```text
QoS ≠ Priority
QoS ≠ Deadline
QoS ≠ Guaranteed Unlimited Resources
```

QoS kombiniert mehrere Anforderungen zu einem kontrollierten I/O-Serviceprofil.

## QoS-Profil

Ein I/O-Request darf ein QoS-Profil besitzen.

```text
IOQoS
├── Priority Class
├── Latency Target
├── Deadline
├── Minimum Throughput
├── Maximum Bandwidth
└── Resource Budget
```

Nicht jede Eigenschaft muss für jeden Request gesetzt sein.

## QoS-Klassen

NovaOS soll mindestens folgende logische Klassen unterstützen:

```text
Realtime
Interactive
Normal
Background
Maintenance
```

Die Klassen definieren Standardwerte, dürfen aber durch einen `ExecutionContract` konkretisiert werden.

## Harte und weiche Anforderungen

QoS-Anforderungen müssen zwischen harten und weichen Bedingungen unterscheiden können.

```text
Hard Requirement
    → MUSS eingehalten werden

Soft Preference
    → SOLL optimiert werden
```

Kann eine harte Anforderung nicht erfüllt werden, muss dies erkannt werden.

Eine weiche Präferenz darf zugunsten höher priorisierter Systemanforderungen verletzt werden.

## Latenz

Requests dürfen ein gewünschtes Latenzziel besitzen.

```text
Submit
  ↓
Queue
  ↓
Execution
  ↓
Completion
```

NovaOS soll Queue-, Provider- und Completion-Latenz getrennt beobachten können.

## Durchsatz

QoS darf Mindest- oder Zielwerte für den Durchsatz beschreiben.

Beispiele:

```text
Minimum Throughput
Target Throughput
Maximum Bandwidth
```

Eine Bandbreitenbegrenzung darf verwendet werden, um Ressourcen zwischen Consumern zu teilen.

## Ressourcenbudget

QoS darf Ressourcenlimits enthalten.

```text
Queue Depth
Bandwidth
Memory
DMA Resources
CPU Overhead
```

Ein Consumer darf durch QoS keine systemweiten Ressourcenlimits umgehen.

## Admission Control

Vor der Annahme harter QoS-Anforderungen darf NovaOS prüfen, ob diese realistisch erfüllbar sind.

```text
QoS Request
    ↓
Admission Control
    ↓
Accept / Degrade / Reject
```

Harte Anforderungen dürfen nicht stillschweigend als garantiert akzeptiert werden, wenn die benötigten Ressourcen nicht verfügbar sind.

## Scheduling

Der I/O-Scheduler muss QoS-Informationen berücksichtigen können.

```text
Requests
   ↓
Priority + Deadline + QoS
   ↓
I/O Scheduler
```

QoS darf Request-Reihenfolge, Queue-Zuteilung, Bandbreite und Parallelität beeinflussen.

## Isolation

NovaOS soll I/O-Ressourcen zwischen unterschiedlichen Workloads isolieren können.

```text
Interactive ─┐
Realtime ────┼→ QoS Isolation → Provider
Background ──┘
```

Ein Hintergrundprozess soll dadurch beispielsweise interaktive I/O nicht dauerhaft verdrängen können.

## Degradation

Kann ein gewünschtes QoS-Niveau nicht gehalten werden, muss NovaOS kontrolliert degradieren können.

```text
Requested QoS
     ↓
Resource Pressure
     ↓
Reduced QoS
```

Harte Anforderungen dürfen dabei nicht unbemerkt in weiche Anforderungen umgewandelt werden.

## Provider Mapping

Provider dürfen NovaOS-QoS auf eigene Mechanismen abbilden.

```text
Nova QoS
   ↓
Provider Mapping
   ↓
Hardware Queue / Protocol QoS
```

Fehlende Hardwareunterstützung muss erkennbar sein.

## Monitoring

NovaOS soll tatsächliche und angeforderte QoS vergleichen können.

```text
Requested
   ↕
Observed
   ↓
Deviation
```

Diese Daten dürfen für Scheduling, Diagnose und adaptive Optimierung verwendet werden.

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches QoS-Modell für I/O bereitstellen.
2. QoS, Priorität und Deadline MÜSSEN getrennte Eigenschaften bleiben.
3. QoS MUSS harte Anforderungen und weiche Präferenzen unterscheiden können.
4. Requests SOLLEN Latenz-, Durchsatz- und Ressourcenanforderungen ausdrücken können.
5. Harte QoS-Anforderungen MÜSSEN vor einer Garantie auf Erfüllbarkeit prüfbar sein.
6. Nicht erfüllbare harte Anforderungen DÜRFEN NICHT stillschweigend akzeptiert werden.
7. Der I/O-Scheduler MUSS QoS-Informationen berücksichtigen können.
8. QoS DÜRFEN Security- oder Ressourcenlimits NICHT umgehen.
9. NovaOS SOLL QoS-Isolation zwischen Workloads unterstützen.
10. Kontrollierte QoS-Degradation MUSS möglich sein.
11. Provider DÜRFEN QoS auf eigene Hardware- oder Protokollmechanismen abbilden.
12. Angeforderte und tatsächlich erreichte QoS SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-SCHEDULER-0001`
- `NPSPEC-IO-PRIORITY-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-SCHEDULER-QOS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IO-0006`

## Ergebnis

```text
I/O Requirements
      ↓
QoS Profile
      ↓
Admission + Scheduling
      ↓
Provider
      ↓
Observed QoS
```

NovaOS erhält damit ein einheitliches I/O-QoS-Modell, das Latenz, Durchsatz, Priorität und Ressourcenverbrauch kontrolliert und mit Execution Contracts sowie dem I/O-Scheduler verbindet.