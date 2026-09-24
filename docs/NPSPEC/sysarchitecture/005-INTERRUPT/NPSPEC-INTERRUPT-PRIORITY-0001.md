# NPSPEC-INTERRUPT-PRIORITY-0001 – Nova Interrupt Priority

## Status

Entwurf

## Kategorie

Interrupts / Priority / Latency / Kernel

## Zweck

NovaOS definiert ein einheitliches Prioritätsmodell für Interrupts, damit zeitkritische Ereignisse bevorzugt verarbeitet und weniger wichtige Interrupts kontrolliert verzögert werden können.

```text
Interrupts
    ↓
Priority Evaluation
    ↓
CPU / Interrupt Controller
    ↓
Handler
```

## Grundprinzip

Interrupt-Priorität beschreibt die Dringlichkeit der unmittelbaren Interrupt-Verarbeitung.

```text
Higher Priority
      ↓
Earlier Handling
      ↓
Lower Priority
```

Interrupt-Priorität ist von Prozess- oder Thread-Priorität getrennt.

```text
Interrupt Priority ≠ Scheduling Priority
```

## Prioritätsklassen

NovaOS verwendet logische Prioritätsklassen, die durch die HAL auf die jeweilige Hardware abgebildet werden.

Beispiel:

```text
Critical
High
Normal
Low
```

Die konkrete Anzahl hardwareseitiger Prioritätsstufen darf plattformabhängig sein.

## Prioritätszuweisung

Die Priorität kann abhängig sein von:

```text
Interrupt Type
Device Class
Realtime Requirement
Platform Constraint
Kernel Policy
```

Treiber dürfen nicht beliebig maximale Prioritäten beanspruchen.

## Preemption

Höher priorisierte Interrupts dürfen niedrigere Interrupt-Verarbeitung unterbrechen, sofern Plattform und aktuelle Ausführung dies zulassen.

```text
Low Priority Handler
        ↓
High Priority Interrupt
        ↓
High Priority Handler
        ↓
Resume Low Priority Handler
```

Verschachtelung muss begrenzt und kontrollierbar bleiben.

## Maskierung

NovaOS muss Interrupts anhand ihrer Priorität temporär blockieren können.

```text
Current Priority Level
        ↓
Allow Higher Priority
Mask Lower Priority
```

Globale Interrupt-Deaktivierung soll nur für möglichst kurze kritische Abschnitte verwendet werden.

## Realtime

Realtime-Interrupts können erhöhte Prioritäten erhalten, wenn dies durch definierte Realtime-Anforderungen gerechtfertigt ist.

```text
Realtime Requirement
        ↓
Interrupt Priority
        ↓
Bounded Response
```

Eine hohe Priorität allein stellt jedoch keine Deadline-Garantie dar.

## Starvation

Dauerhaft hoch priorisierte Interrupts dürfen niedrigere Prioritäten nicht unkontrolliert verdrängen.

Interrupt Storms und fehlerhafte Geräte müssen erkannt und begrenzt werden können.

## Multiprocessing

Interrupt-Priorität und CPU-Routing können gemeinsam verwendet werden.

```text
Interrupt
   ↓
Priority + Affinity
   ↓
Target CPU
```

Die Auswahl der Ziel-CPU bleibt eine Policy-Entscheidung außerhalb der HAL.

## Normative Anforderungen

1. NovaOS MUSS Interrupt-Prioritäten unterstützen.
2. Logische Prioritäten MÜSSEN auf plattformspezifische Hardwareprioritäten abbildbar sein.
3. Interrupt-Priorität MUSS von Scheduling-Priorität getrennt bleiben.
4. Höhere Prioritäten DÜRFEN niedrigere Interrupt-Verarbeitung kontrolliert unterbrechen.
5. Interrupt-Verschachtelung MUSS begrenzbar sein.
6. Prioritätsbasierte Maskierung MUSS unterstützt werden, sofern die Hardware dies ermöglicht.
7. Treiber DÜRFEN maximale Interrupt-Prioritäten nicht unkontrolliert beanspruchen.
8. Realtime-Anforderungen SOLLEN bei der Prioritätszuweisung berücksichtigt werden.
9. Hohe Interrupt-Priorität DARF NICHT als alleinige Deadline-Garantie gelten.
10. Interrupt Storms SOLLEN erkannt und begrenzt werden.
11. Starvation niedriger Prioritäten MUSS berücksichtigt werden.
12. Hardwareabhängige Prioritätsmechanismen MÜSSEN durch die HAL abstrahiert werden.

## Abhängigkeiten

- `NPSPEC-INTERRUPT-0001`
- `NPSPEC-HAL-INTERRUPT-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `ADR-INTERRUPT-0002`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`

## Ergebnis

```text
Interrupt
   ↓
Logical Priority
   ↓
HAL Mapping
   ↓
Hardware Priority
   ↓
Controlled Processing
```

NovaOS erhält damit ein plattformunabhängiges Interrupt-Prioritätsmodell, das niedrige Latenzen ermöglicht, ohne Scheduling und Interrupt-Verarbeitung miteinander zu vermischen.