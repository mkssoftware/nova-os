# ADR-INTERRUPT-0005 – Interrupt Coalescing als optimierbare Policy

## Status

Angenommen

## Kategorie

Kernel / Interrupts / I/O / Coalescing / Performance / Latenz / Energie / Policy

## Kontext

Moderne Geräte können sehr hohe Ereignisraten erzeugen.

Beispiele:

- Netzwerkadapter,
- NVMe,
- Hochgeschwindigkeits-Storage,
- virtuelle Geräte,
- Accelerator Completion Queues.

Würde jedes einzelne Ereignis unmittelbar einen Interrupt erzeugen, entstehen hohe Kosten durch:

- Interrupt Entry/Exit,
- Scheduling,
- Deferred Work,
- Cache-Verdrängung,
- CPU-Wechsel,
- Controllerzugriffe.

Beispiel:

```text
Event
  ↓
IRQ

Event
  ↓
IRQ

Event
  ↓
IRQ

Event
  ↓
IRQ
```

Interrupt Coalescing fasst mehrere Ereignisse zusammen und erzeugt erst nach:

- einer bestimmten Ereignisanzahl,
- einem Zeitfenster,
- einer Kombination aus beiden

einen Interrupt.

Beispiel:

```text
Event
Event
Event
Event
   ↓
Coalescing
   ↓
1 Interrupt
```

Dadurch sinkt der Interruptoverhead.

Gleichzeitig steigt jedoch potentiell die Latenz einzelner Ereignisse.

NovaOS darf Interrupt Coalescing deshalb nicht als statische Hardwareeinstellung behandeln.

Es wird als optimierbare System-Policy modelliert.

---

## Entscheidung

NovaOS behandelt Interrupt Coalescing als **explizite, policygesteuerte Optimierung** innerhalb des Interrupt- und I/O-Subsystems.

Die Hardware stellt lediglich verfügbare Mechanismen bereit.

Die System-Policy entscheidet anhand von Anforderungen und Laufzeitzustand über:

- Aktivierung,
- Zeitfenster,
- Ereignisschwellen,
- maximale Verzögerung,
- Anpassung,
- Deaktivierung.

Das Grundmodell lautet:

```text
Execution Requirements
        +
Current Workload
        +
Device Capabilities
        ↓
Coalescing Policy
        ↓
Coalescing Parameters
        ↓
Device / Interrupt Mechanism
```

---

## Grundprinzip

```text
Coalesce for efficiency.

Do not hide latency.
```

Interrupt Coalescing ist eine Optimierung und darf niemals eine Korrektheitsvoraussetzung sein.

---

## Mechanism / Policy Separation

Der Gerätetreiber beziehungsweise die HAL stellt Mechanismen bereit.

Beispiel:

```text
SetCoalescing(
    eventThreshold,
    timeThreshold
)
```

Die Auswahl der Werte erfolgt nicht dauerhaft im Treiber.

Die Policy liegt oberhalb der Hardwaremechanik.

```text
Policy decides why and how much.

Driver applies what hardware supports.
```

---

## Coalescing-Modell

Konzeptionell:

```text
InterruptCoalescingPolicy {
    mode
    eventThreshold
    timeThreshold
    maximumLatency
    adaptationPolicy
}
```

Die konkrete Datenstruktur wird separat spezifiziert.

---

## Coalescing Modes

NovaOS soll mindestens folgende semantische Modi unterstützen können:

```text
Disabled
Fixed
Adaptive
LatencyOptimized
ThroughputOptimized
EnergyOptimized
```

Nicht jede Hardware muss jeden Modus direkt implementieren.

Die Policy übersetzt die gewünschte Semantik auf die verfügbaren Hardwaremechanismen.

---

## Event-basiertes Coalescing

Bei eventbasiertem Coalescing wird ein Interrupt nach einer bestimmten Anzahl von Ereignissen ausgelöst.

Beispiel:

```text
eventThreshold = 32
```

Dann kann gelten:

```text
32 completions
      ↓
1 interrupt
```

Dies eignet sich besonders bei hoher und kontinuierlicher Last.

---

## Zeitbasiertes Coalescing

Bei zeitbasiertem Coalescing wird ein Interrupt spätestens nach einem definierten Zeitfenster ausgelöst.

Beispiel:

```text
timeThreshold = 50 µs
```

Damit wird verhindert, dass einzelne Ereignisse unbegrenzt auf weitere Ereignisse warten.

---

## Kombiniertes Coalescing

Hardware kann beide Mechanismen kombinieren.

Beispiel:

```text
interrupt when:

32 events reached

OR

50 µs elapsed
```

Das zuerst eintretende Ereignis löst die Zustellung aus.

Dies ist das bevorzugte Modell, sofern Hardware und Anforderungen es unterstützen.

---

## Maximum Latency

Coalescing benötigt eine explizite obere Latenzgrenze.

Konzeptionell:

```text
maximumLatency
```

Die Policy darf keinen Coalescingparameter wählen, der eine harte Latenzanforderung verletzt.

Beispiel:

```text
Execution Contract:
    maximum interrupt latency = 100 µs
```

Dann darf ein Coalescing-Zeitfenster nicht unkontrolliert darüber liegen.

---

## Throughput

Bei hohen Datenraten kann stärkeres Coalescing den Durchsatz verbessern.

Beispiele:

```text
higher event threshold
longer coalescing window
larger batches
```

Dadurch sinkt der Anteil der CPU-Zeit, der für Interruptverwaltung benötigt wird.

---

## Latenz

Für interaktive oder latenzkritische Operationen kann Coalescing reduziert oder deaktiviert werden.

Beispiele:

- Eingabegeräte,
- Echtzeit-I/O,
- einzelne synchrone Storageoperationen,
- latenzkritische Netzwerkpakete.

Die Policy darf verschiedene Workloadklassen unterschiedlich behandeln.

---

## Energieeffizienz

Coalescing kann die Anzahl von CPU Wakeups reduzieren.

Dadurch kann es im Energiesparbetrieb sinnvoll sein, Ereignisse stärker zusammenzufassen.

Beispiel:

```text
fewer interrupts
     ↓
fewer CPU wakeups
     ↓
longer idle states
```

Energieoptimierung darf jedoch keine harte Latenzanforderung verletzen.

---

## Adaptive Policy

Im Adaptive Mode dürfen Coalescingparameter dynamisch verändert werden.

Mögliche Eingangssignale:

- Interrupt Rate,
- Event Rate,
- Queue Depth,
- CPU Load,
- Deferred Work Load,
- Durchschnittslatenz,
- Tail Latency,
- Energieprofil.

Beispiel:

```text
Low load
   ↓
small coalescing window

High load
   ↓
larger coalescing window
```

---

## Hysterese

Adaptive Coalescingparameter dürfen nicht bei jeder kleinen Laständerung wechseln.

Die Policy muss Hysterese unterstützen können.

Dadurch werden:

- Parameter-Flapping,
- unnötige Controllerprogrammierung,
- instabile Latenz

vermieden.

---

## Änderungsrate

Coalescingparameter sollen nur mit kontrollierter Frequenz verändert werden.

Eine Policy darf nicht versuchen, die Hardware für jedes einzelne Ereignis neu zu programmieren.

---

## Prediction Error

Adaptive Policies dürfen Prediction Error als Lernsignal nutzen.

Beispiel:

```text
Prediction:
    64-event coalescing improves throughput
    without violating latency

Observed:
    tail latency exceeds target

Prediction Error
    ↓
reduce coalescing
```

Prediction Error darf nur Optimierungsentscheidungen verändern.

Hard Constraints bleiben unverändert.

---

## Execution Contracts

Coalescing muss mit `Nova.ExecutionContract` integrierbar sein.

Relevante Anforderungen können sein:

```text
Latency / Deadline
Determinism Requirement
Resource Budget
Energy Preference
Realtime Requirement
```

Damit kann dieselbe Hardware abhängig vom aktuellen Workload unterschiedlich konfiguriert werden.

---

## Per-Queue-Coalescing

Bei Multi-Queue-Geräten soll Coalescing möglichst pro Queue steuerbar sein.

Beispiel:

```text
NIC
 ├── Queue 0 → low latency
 ├── Queue 1 → throughput
 ├── Queue 2 → throughput
 └── Queue 3 → realtime
```

Dadurch müssen nicht alle Queues dieselbe Policy verwenden.

---

## MSI-X Integration

Per-Queue-Coalescing lässt sich besonders gut mit MSI-X kombinieren.

Konzeptionell:

```text
Queue
   ↓
Coalescing Policy
   ↓
MSI-X Interrupt
   ↓
Affinity
   ↓
Deferred Work
```

Die einzelnen Mechanismen bleiben dennoch unabhängig.

---

## Hybrides Interrupt-/Polling-Modell

Coalescing ergänzt `ADR-INTERRUPT-0004`.

Die Mechanismen sind nicht identisch:

```text
Coalescing
    ↓
reduces interrupt frequency

Polling
    ↓
temporarily avoids repeated interrupts
```

Eine typische Kombination kann sein:

```text
Hardware Coalescing
      ↓
Interrupt
      ↓
Deferred Polling
      ↓
Batch Processing
```

Die Policy darf beide Mechanismen gemeinsam optimieren.

---

## Deferred Work

Coalescing darf lange Interrupt Handler nicht rechtfertigen.

Auch ein zusammengefasster Interrupt folgt weiterhin:

```text
Interrupt
   ↓
Minimal Handler
   ↓
Deferred Work
```

entsprechend `ADR-INTERRUPT-0003`.

---

## Affinity

Coalescingparameter können abhängig von der verfügbaren CPU- und Queue-Lokalität angepasst werden.

Beispiel:

```text
local lightly loaded CPU
    → smaller coalescing

remote overloaded CPU
    → larger coalescing
```

Dies bleibt eine Optimierung.

Interrupt Affinity selbst wird durch `ADR-INTERRUPT-0002` geregelt.

---

## CPU Load

Hohe CPU-Auslastung kann ein Grund sein, stärkere Coalescingparameter zu wählen.

Dadurch können Interruptkosten reduziert werden.

Die Policy darf jedoch nicht durch stärkeres Coalescing eine bestehende Deadlineverletzung verschärfen.

---

## Resource Economy

Interrupt Coalescing beeinflusst mehrere Systemressourcen gleichzeitig:

```text
CPU time
interrupt rate
energy
latency
queue occupancy
```

Die Entscheidung wird deshalb in die systemweite Ressourcenökonomie integriert.

---

## Realtime

Hard-Realtime-Workloads dürfen:

- Coalescing vollständig deaktivieren,
- feste kleine Schwellenwerte,
- feste maximale Zeitfenster

verlangen.

Adaptive Änderungen dürfen innerhalb eines Hard-Realtime-Scopes deaktiviert werden.

---

## Deterministic Mode

Im Deterministic Mode müssen Coalescingparameter stabil und reproduzierbar sein.

Geeignete Modi sind beispielsweise:

```text
Disabled
```

oder:

```text
Fixed {
    eventThreshold = N
    timeThreshold = T
}
```

Adaptive Policyänderungen werden innerhalb des entsprechenden deterministischen Scopes deaktiviert.

---

## Graceful Degradation

Kann ein gewünschter Coalescingmodus von der Hardware nicht vollständig umgesetzt werden, darf die Policy auf eine schwächere gültige Konfiguration zurückfallen.

Beispiel:

```text
requested:
    event + time threshold

hardware supports:
    time threshold only
```

Dann kann ein zeitbasiertes Fallback verwendet werden, sofern die Anforderungen weiterhin erfüllt werden.

Hard Constraints dürfen nicht still verletzt werden.

---

## Capability Discovery

Treiber müssen die Coalescingfähigkeiten eines Geräts melden können.

Beispiel:

```text
InterruptCoalescingCapabilities {
    eventBased
    timeBased
    perQueue
    minimumTime
    maximumTime
    maximumEventThreshold
}
```

Die Policy darf keine Funktionen voraussetzen, die die Hardware nicht besitzt.

---

## Shared Interrupts

Bei Shared Legacy Interrupts kann Coalescing nur eingeschränkt kontrollierbar sein.

Die Policy muss die tatsächliche Hardwaregrenze berücksichtigen.

Gerätespezifische Coalescingmechanismen dürfen dennoch verwendet werden, sofern sie andere Geräte nicht beeinflussen.

---

## Virtuelle Geräte

Virtuelle Geräte können ebenfalls Interrupt Moderation beziehungsweise Coalescing bereitstellen.

Diese Funktionen werden über dieselbe semantische Policy integriert.

NovaOS unterscheidet auf Policyebene nicht unnötig zwischen physischer und virtueller Implementierung.

---

## Device Hotplug

Beim Hinzufügen eines Geräts werden dessen Coalescingfähigkeiten erkannt.

Beim Entfernen werden adaptive Zustände und zugehörige Policyressourcen freigegeben.

Ein nachträglicher Deferred Worker darf kein bereits entferntes Device umkonfigurieren.

---

## Fehlerbehandlung

Mögliche Fehler sind:

```text
Unsupported
InvalidThreshold
ConfigurationRejected
DeviceUnavailable
PolicyConstraintConflict
```

Eine fehlgeschlagene Coalescingoptimierung darf die grundsätzliche Interruptfunktion nicht außer Betrieb setzen.

Soweit möglich wird auf einen gültigen Interrupt-Baseline-Modus zurückgefallen.

---

## Fail-Safe Default

Kann keine sichere Coalescingkonfiguration bestimmt werden, gilt:

```text
Coalescing disabled
```

beziehungsweise eine bekannte konservative Hardwarekonfiguration.

Korrekte Interruptzustellung hat Vorrang vor Optimierung.

---

## Introspection

Aktuelle Coalescingentscheidungen müssen introspektierbar sein.

Beispiel:

```text
Device:
    nic0

Queue:
    4

Mode:
    Adaptive

Event Threshold:
    32

Time Threshold:
    40 µs

Observed IRQ Rate:
    52000/s

Reason:
    High packet rate
    Latency target still satisfied
```

---

## Decision Record

Adaptive Änderungen sollen einen strukturierten Entscheidungsgrund besitzen.

Konzeptionell:

```text
CoalescingDecision {
    source
    oldConfiguration
    newConfiguration
    reason
    constraints
    observedMetrics
}
```

Dies unterstützt Architecture Introspection und Diagnose.

---

## Telemetrie

Für Optimierung und Diagnose sollen mindestens messbar sein können:

- Event Rate,
- Interrupt Rate,
- Events pro Interrupt,
- Coalescing Window,
- Queue Depth,
- Processing Latency,
- Tail Latency,
- CPU Cost,
- Anzahl der Policyänderungen.

Nicht jede Messgröße muss permanent mit maximaler Auflösung erfasst werden.

---

## Sicherheit

Nicht privilegierte Anwendungen dürfen Interrupt-Coalescing-Hardware nicht direkt programmieren.

Treiber dürfen unterstützte Mechanismen bereitstellen und Workloadinformationen melden.

Die finale Policyentscheidung bleibt unter Systemkontrolle.

---

## KI-Unabhängigkeit

Coalescing muss vollständig mit statischen und deterministischen Regeln funktionieren.

KI darf optional bei der Optimierung helfen.

Sie darf keine:

- Latenzgrenze,
- Realtime-Anforderung,
- Sicherheitsregel,
- Determinismusvorgabe

überschreiben.

---

## Normative Anforderungen

1. NovaOS MUSS Interrupt Coalescing als optionale Policy und nicht als Korrektheitsvoraussetzung behandeln.
2. Coalescing-Mechanismus und Coalescing-Policy MÜSSEN getrennt sein.
3. Treiber MÜSSEN unterstützte Coalescingfähigkeiten explizit melden können.
4. Event- und zeitbasierte Schwellen MÜSSEN getrennt modellierbar sein.
5. Kombiniertes Event-/Zeit-Coalescing MUSS unterstützt werden können, sofern die Hardware dies ermöglicht.
6. Coalescing MUSS eine maximale zulässige Latenz berücksichtigen können.
7. Hard-Latency- und Realtime-Constraints DÜRFEN NICHT durch Coalescing verletzt werden.
8. Coalescing MUSS vollständig deaktivierbar sein.
9. Feste Coalescingparameter MÜSSEN unterstützt werden können.
10. Adaptive Coalescingparameter SOLLEN unterstützt werden können.
11. Adaptive Änderungen MÜSSEN Hysterese verwenden können.
12. Adaptive Änderungen SOLLEN in ihrer Änderungsfrequenz begrenzt werden.
13. Event Rate, Interrupt Rate und Queue Depth DÜRFEN als Policy-Signale verwendet werden.
14. CPU-Last, Energie und Latenz DÜRFEN als Optimierungsziele verwendet werden.
15. Multi-Queue-Geräte SOLLEN Coalescing pro Queue unterstützen können.
16. Coalescing MUSS mit MSI/MSI-X kombinierbar sein.
17. Coalescing MUSS mit Deferred Work kompatibel bleiben.
18. Coalescing MUSS mit dem hybriden Interrupt-/Polling-Modell kombinierbar sein.
19. Coalescing DARF NICHT als Ersatz für Backpressure verwendet werden.
20. Resource Economy MUSS Coalescingkosten und -vorteile berücksichtigen können.
21. Realtime-Policies MÜSSEN Coalescing begrenzen oder deaktivieren können.
22. Deterministic Mode MUSS feste reproduzierbare Coalescingparameter ermöglichen.
23. Hardware-Limitierungen MÜSSEN explizit behandelt werden.
24. Ungültige oder nicht erfüllbare Konfigurationen MÜSSEN typisierte Fehler liefern.
25. Eine fehlgeschlagene Optimierung DARF die grundsätzliche Interruptfunktion NICHT unnötig deaktivieren.
26. Ein konservativer Fail-Safe-Modus MUSS verfügbar sein.
27. Aktuelle Coalescingparameter MÜSSEN introspektierbar sein.
28. Adaptive Policyänderungen SOLLEN begründbar und auditierbar sein.
29. Nicht privilegierte Anwendungen DÜRFEN Coalescinghardware NICHT direkt programmieren.
30. Korrekte Coalescingsteuerung MUSS vollständig ohne KI möglich sein.

---

## Konsequenzen

### Positive Konsequenzen

- geringerer Interruptoverhead,
- höherer Durchsatz bei hoher Last,
- bessere Batch-Verarbeitung,
- weniger CPU-Wakeups,
- bessere Energieeffizienz,
- an Workload und Execution Contracts anpassbar,
- gute Integration mit Polling und Deferred Work,
- per-Queue-Optimierung moderner Geräte.

### Negative Konsequenzen

- zu aggressives Coalescing erhöht Latenz,
- adaptive Steuerung benötigt Telemetrie,
- Hardwarefähigkeiten unterscheiden sich stark,
- Wechselwirkungen mit Polling, Affinity und Queueing erhöhen die Policy-Komplexität.

---

## Verworfene Alternativen

### Immer maximale Coalescingwerte

Verworfen.

Dies maximiert zwar potentiell den Durchsatz, kann aber Latenzanforderungen erheblich verletzen.

### Coalescing immer deaktivieren

Verworfen.

Bei hohen Ereignisraten würden unnötige Interruptkosten entstehen.

### Feste globale Systemwerte

Verworfen.

Unterschiedliche Geräte, Queues und Workloads besitzen unterschiedliche Anforderungen.

### Treiber entscheidet vollständig selbst

Verworfen.

Der Treiber besitzt nicht den vollständigen Kontext zu:

- Systemlast,
- Execution Contracts,
- Realtime,
- Energiepolitik,
- Resource Economy.

### Adaptive KI als notwendige Steuerung

Verworfen.

Interruptkorrektheit und grundlegende Optimierung müssen deterministisch ohne KI funktionieren.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-INTERRUPT-0001_Einheitliche_Interrupt_Controller_Abstraktion`
- `ADR-INTERRUPT-0002_Interrupt_Affinity_und_Topology_Awareness`
- `ADR-INTERRUPT-0003_Deferred_Work_statt_langer_Interrupt_Handler`
- `ADR-INTERRUPT-0004_Hybrides_Interrupt_und_Polling_Modell`
- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-INTERRUPT-COALESCING-0001`
- `NPSPEC-INTERRUPT-COALESCING-POLICY-0001`
- `NPSPEC-INTERRUPT-COALESCING-CAPABILITY-0001`
- `NPSPEC-INTERRUPT-COALESCING-THRESHOLD-0001`
- `NPSPEC-INTERRUPT-COALESCING-LATENCY-0001`
- `NPSPEC-INTERRUPT-COALESCING-ADAPTIVE-0001`
- `NPSPEC-INTERRUPT-COALESCING-QUEUE-0001`
- `NPSPEC-INTERRUPT-COALESCING-REALTIME-0001`
- `NPSPEC-INTERRUPT-COALESCING-INTROSPECTION-0001`
- `NPSPEC-INTERRUPT-COALESCING-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Coalescing deaktiviert,
- feste Event-Schwelle,
- feste Zeit-Schwelle,
- kombinierte Event-/Zeit-Schwelle,
- niedrige Ereignisrate,
- hohe Ereignisrate,
- Latenzgrenze,
- Hard-Realtime mit deaktiviertem Coalescing,
- adaptive Erhöhung,
- adaptive Reduzierung,
- Hysterese,
- Multi-Queue-NIC,
- Multi-Queue-NVMe,
- MSI-X,
- Coalescing mit Deferred Work,
- Coalescing mit Polling,
- CPU-Überlast,
- Energieprofil,
- Deterministic Mode,
- nicht unterstützte Hardwarefunktion,
- ungültige Schwellenwerte,
- Device Hot Removal,
- Fail-Safe-Rückkehr zur Baseline,
- Introspection und Telemetrie.

---

## Ergebnis

NovaOS behandelt Interrupt Coalescing nicht als statische Geräteeinstellung, sondern als kontrollierte System-Policy.

Das Architekturmodell lautet:

```text
Workload
   +
Execution Contract
   +
Device Capabilities
        ↓
Coalescing Policy
        ↓
Thresholds
        ↓
Interrupt Generation
        ↓
Deferred Work / Polling
```

Damit kann NovaOS Interruptkosten reduzieren, ohne Latenz-, Realtime- oder Determinismusanforderungen aus den Augen zu verlieren.

Die zentrale Architekturregel lautet:

```text
Reduce unnecessary interrupts.

Keep latency explicit.

Let policy choose the tradeoff.

Never trade correctness
for fewer interrupts.
```