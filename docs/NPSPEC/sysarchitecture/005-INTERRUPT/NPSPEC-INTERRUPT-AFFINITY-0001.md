# NPSPEC-INTERRUPT-AFFINITY-0001 – Nova Interrupt Affinity

## Status

Entwurf

## Kategorie

Interrupts / Affinity / SMP / Locality

## Zweck

NovaOS definiert Interrupt Affinity, um Interrupts gezielt bestimmten CPUs oder CPU-Gruppen zuzuordnen.

```text
Interrupt
   ↓
Affinity Policy
   ↓
CPU Set
   ↓
Target CPU
```

Dadurch können Lastverteilung, Cache-Lokalität, NUMA-Nähe und Realtime-Anforderungen berücksichtigt werden.

## Grundprinzip

Interrupt-Quelle und ausführende CPU sind logisch getrennt.

```text
Device
   ↓
Interrupt
   ↓
Allowed CPU Set
   ↓
Selected CPU
```

Die HAL stellt Routing-Mechanismen bereit. Die Auswahl der geeigneten CPU erfolgt durch höhere Systempolitik.

## Affinity-Modell

Eine Interrupt-Affinität beschreibt mindestens:

```text
InterruptID
AllowedCPUs
PreferredCPUs
CurrentTarget
Constraints
```

Dabei gilt:

```text
AllowedCPUs = Hard Constraint
PreferredCPUs = Soft Preference
```

## CPU-Gruppen

Affinity kann einzelne CPUs oder Gruppen adressieren.

```text
CPU 0
CPU 1
CPU Set
NUMA Node
Core Group
```

Stabile logische CPU-IDs sollen gegenüber hardwarespezifischen Controller-IDs bevorzugt werden.

## Lokalität

NovaOS soll Interrupts möglichst dort verarbeiten können, wo auch die zugehörigen Daten oder Tasks verarbeitet werden.

```text
Device
  ↓
NUMA Node
  ↓
Interrupt CPU
  ↓
Consumer Task
```

Lokalität ist eine Optimierung und darf harte Anforderungen nicht überschreiben.

## Lastverteilung

Interrupts können zwischen geeigneten CPUs verteilt werden.

```text
CPU Load
+
Interrupt Load
+
Affinity Constraints
      ↓
Target Selection
```

Häufige Migration ohne messbaren Nutzen soll vermieden werden.

## Realtime

Realtime-Anforderungen können feste oder eingeschränkte Affinitäten verlangen.

```text
Realtime Interrupt
       ↓
Defined CPU Set
       ↓
Controlled Execution
```

Solche Hard Constraints dürfen nicht durch adaptive Lastverteilung überschrieben werden.

## CPU Hotplug

Wird eine Ziel-CPU entfernt oder deaktiviert:

```text
CPU Offline
    ↓
Reevaluate Affinity
    ↓
Select Valid CPU
```

Existiert kein zulässiges Ziel, muss der Interrupt in einen definierten Fehler- oder Degradationszustand wechseln.

## Normative Anforderungen

1. NovaOS MUSS Interrupt Affinity auf Multiprozessorsystemen unterstützen.
2. Interrupts MÜSSEN einem definierten CPU-Set zugeordnet werden können.
3. Harte und bevorzugte Affinitäten MÜSSEN unterscheidbar sein.
4. Die HAL MUSS das hardwareabhängige Interrupt-Routing abstrahieren.
5. NUMA- und Cache-Lokalität SOLLEN bei der Zielauswahl berücksichtigt werden.
6. Interrupt-Last SOLL zwischen geeigneten CPUs verteilt werden können.
7. Hard Affinity Constraints DÜRFEN NICHT durch adaptive Optimierung verletzt werden.
8. CPU-Hotplug MUSS eine Neubewertung betroffener Affinitäten auslösen.
9. Eine nicht verfügbare Ziel-CPU DARF NICHT weiterhin als gültiges Routing-Ziel behandelt werden.
10. Affinity-Entscheidungen SOLLEN diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-INTERRUPT-0001`
- `NPSPEC-INTERRUPT-PRIORITY-0001`
- `NPSPEC-HAL-INTERRUPT-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `ADR-INTERRUPT-0003`

## Ergebnis

```text
Interrupt
   ↓
Constraints + Topology + Load
   ↓
Affinity Selection
   ↓
Target CPU
```

NovaOS kann Interrupts damit gezielt auf geeignete CPUs verteilen und dabei Lokalität, Last und harte Ausführungsanforderungen berücksichtigen.