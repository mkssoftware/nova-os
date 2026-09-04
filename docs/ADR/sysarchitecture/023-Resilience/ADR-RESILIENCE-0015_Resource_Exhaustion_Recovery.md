# ADR-RESILIENCE-0015 – Resource Exhaustion Recovery

## Status

Angenommen

## Kategorie

Resilience Architecture / Resource Exhaustion / Recovery

## Kontext

Ressourcenerschöpfung ist eine besondere Fehlerklasse, da zusätzliche Recovery-Aktivitäten selbst weitere Ressourcen benötigen und den ursprünglichen Fehler verstärken können.

Betroffen sein können beispielsweise:

- Arbeitsspeicher,
- CPU-Zeit,
- Threads und Tasks,
- Handles,
- Storage,
- I/O-Kapazität,
- Netzwerkbandbreite,
- GPU-/NPU-Ressourcen,
- Energie,
- interne Queues und Buffer.

Ein einfacher Restart ist keine ausreichende Strategie, wenn die eigentliche Ursache weiterhin besteht.

Nova.Resilience benötigt deshalb einen definierten Recovery-Pfad für Resource Exhaustion.

## Entscheidung

NovaOS behandelt **Resource Exhaustion als eigene Resilience-Fehlerklasse** und integriert deren Recovery mit der systemweiten Ressourcenökonomie.

```text
Resource Pressure
       ↓
Detect
       ↓
Classify
       ↓
Contain
       ↓
Reclaim / Throttle / Degrade
       ↓
Verify
       ↓
Restore
```

Ziel ist zunächst die Stabilisierung des Systems und anschließend die Wiederherstellung ausreichender Ressourcenreserven.

## Grundprinzip

```text
Resource Exhaustion
≠
Component Crash
```

und:

```text
Recovery must not
amplify the exhausted resource.
```

## Resource Pressure

NovaOS soll Ressourcenerschöpfung möglichst erkennen, bevor eine Ressource vollständig aufgebraucht ist.

Dafür können mehrere Pressure-Zustände verwendet werden:

```text
Normal
Pressure
Critical
Exhausted
```

Die konkreten Schwellenwerte sind ressourcen- und systemspezifisch.

## Resource Scope

Nova.Resilience muss bestimmen können, auf welcher Ebene die Ressourcenknappheit entsteht.

```text
Task
 ↓
Workload
 ↓
Service
 ↓
Failure Domain
 ↓
System
```

Zusätzlich muss zwischen lokalen und gemeinsam genutzten Ressourcen unterschieden werden.

Ein lokales Memory Budget einer Anwendung ist beispielsweise nicht mit systemweiter Speichererschöpfung gleichzusetzen.

## Containment

Bei kritischer Ressourcenknappheit muss zunächst verhindert werden, dass der Verbrauch unkontrolliert weiter wächst.

Mögliche Maßnahmen sind:

```text
Throttle
Backpressure
Admission Control
Suspend
Rate Limit
Reject New Work
Circuit Break
```

Nicht kritische neue Arbeit darf zugunsten der Systemstabilität zurückgewiesen oder verzögert werden.

## Resource Reclamation

NovaOS kann kontrolliert Ressourcen zurückgewinnen.

Beispiele:

```text
Release Caches
Discard Reconstructable State
Compact Memory
Close Idle Resources
Evict Optional Data
Terminate Disposable Work
Reclaim Expired Leases
```

Die Reihenfolge richtet sich nach Resource Policy, Kritikalität und Wiederherstellungskosten.

## Reclaimability

Ressourcen und Zustände sollen nach Möglichkeit ihre Reclaimability beschreiben können.

Beispielsweise:

```text
Pinned
Critical
Reconstructable
Cached
Disposable
```

Dadurch kann NovaOS bevorzugt Ressourcen freigeben, deren Verlust keine oder geringe semantische Auswirkungen besitzt.

## Priorisierung

Bei knappen Ressourcen muss NovaOS kritische Funktionen gegenüber weniger wichtigen Workloads priorisieren können.

```text
Available Resources
        ↓
Critical System Functions
        ↓
Foreground / Required Work
        ↓
Background / Optional Work
```

Die Priorisierung muss durch Policies und Resource Contracts bestimmt werden und darf nicht ausschließlich aus aktueller Aktivität abgeleitet werden.

## Reserved Recovery Capacity

NovaOS soll für kritische Recovery-Pfade eine begrenzte **Recovery Reserve** vorsehen können.

```text
Normal Resource Pool
        +
Recovery Reserve
```

Diese Reserve darf nicht für normalen Workload-Verbrauch verwendet werden, wenn dadurch die Fähigkeit zur Recovery gefährdet würde.

Sie soll ausschließlich für klar definierte kritische Recovery- und Stabilisierungsvorgänge verfügbar sein.

## Memory Exhaustion

Bei Speicherknappheit soll NovaOS abgestuft reagieren.

Beispielsweise:

```text
Memory Pressure
      ↓
Cache Reclamation
      ↓
Memory Compaction
      ↓
Throttle / Backpressure
      ↓
Optional Workload Reduction
      ↓
Targeted Termination
```

Ungezieltes Beenden beliebiger Prozesse ist keine bevorzugte Recovery-Strategie.

## Storage Exhaustion

Bei erschöpftem persistentem Speicher muss NovaOS zwischen notwendigen und entfernbaren Daten unterscheiden können.

Beispiele:

```text
Expired Temporary Data
Reconstructable Cache
Old Recovery Artifacts
Logs within Retention Policy
User Data
Critical System State
```

Data Retention, Privacy und Recovery-Anforderungen müssen weiterhin eingehalten werden.

Nova.Resilience darf keine geschützten Benutzerdaten allein zur Freigabe von Speicher löschen.

## Compute Exhaustion

Bei CPU-, GPU- oder NPU-Überlastung können beispielsweise folgende Maßnahmen verwendet werden:

```text
Scheduling Priority Adjustment
Concurrency Reduction
Admission Control
Work Deferral
Provider Migration
Degraded Algorithm
```

Ein alternativer Algorithmus darf verwendet werden, wenn der Execution Contract dies zulässt.

## Queue Exhaustion

Unbegrenzte Warteschlangen sind nicht als normale Recovery-Strategie zulässig.

```text
Incoming Work
      ↓
Bounded Queue
      ↓
Backpressure / Reject / Defer
```

Queue Pressure muss auf den Produzenten zurückwirken können.

## Distributed Resources

Location Transparency darf Ressourcenknappheit entfernter Provider nicht verbergen.

```text
Local Workload
      ↓
Remote Provider
      ↓
Resource Exhaustion
```

NovaOS kann alternative Provider auswählen, wenn deren Verwendung mit Capability-, Trust-, Sovereignty- und Execution-Contract-Anforderungen vereinbar ist.

## Recovery Storm Protection

Bei Resource Exhaustion müssen Recovery-Aktivitäten besonders streng begrenzt werden.

```text
Resource Exhaustion
       +
Aggressive Recovery
       ↓
Worse Exhaustion
```

Recovery Budgets und Recovery Storm Protection gelten deshalb auch für Ressourcen-Recovery.

## Verification

Nach einer Recovery muss geprüft werden, ob ausreichend stabile Ressourcenreserven wiederhergestellt wurden.

```text
Reclaim / Throttle / Degrade
          ↓
Resource Verification
          ↓
Stable?
   ├── Yes → Controlled Restore
   └── No  → Further Recovery / Escalation
```

Ein kurzfristiger Rückgang des Verbrauchs reicht nicht aus, wenn unmittelbar erneut Exhaustion droht.

## Degraded Mode

Kann der normale Ressourcenbedarf nicht wiederhergestellt werden, darf NovaOS kontrolliert in einen degradierten Betriebszustand wechseln.

```text
Insufficient Capacity
        ↓
Preserve Critical Functions
        ↓
Degraded Mode
```

Nicht kritische Funktionen können dabei temporär eingeschränkt oder deaktiviert werden.

## Normative Anforderungen

1. Nova.Resilience MUSS Resource Exhaustion als eigene Fehlerklasse behandeln.
2. NovaOS MUSS Resource Pressure nach Möglichkeit vor vollständiger Exhaustion erkennen können.
3. Resource Exhaustion MUSS einem konkreten Resource Scope zugeordnet werden können.
4. Lokale und systemweite Ressourcenerschöpfung MÜSSEN unterscheidbar sein.
5. Recovery MUSS verhindern, dass zusätzliche Recovery-Aktivität die erschöpfte Ressource unkontrolliert weiter belastet.
6. NovaOS MUSS Backpressure, Throttling oder Admission Control zur Stabilisierung unterstützen können.
7. Reclaimable Ressourcen MÜSSEN kontrolliert freigegeben werden können.
8. Reclaimability SOLL semantisch beschreibbar sein.
9. Kritische Systemfunktionen MÜSSEN bei Ressourcenknappheit priorisierbar sein.
10. NovaOS SOLL begrenzte Recovery Reserves für kritische Recovery-Pfade unterstützen können.
11. Recovery Reserves DÜRFEN nicht als reguläres zusätzliches Workload-Budget behandelt werden.
12. Speicher-Recovery SOLL gezielte Reclamation gegenüber willkürlicher Prozessbeendigung bevorzugen.
13. Storage Recovery DARF Privacy-, Retention- oder Data-Sovereignty-Regeln nicht umgehen.
14. Geschützte Benutzerdaten DÜRFEN nicht allein zur Ressourcenfreigabe gelöscht werden.
15. Compute Exhaustion MUSS Concurrency Reduction und Work Deferral unterstützen können.
16. Queues MÜSSEN begrenzbar sein und Backpressure unterstützen können.
17. Alternative Provider DÜRFEN nur unter Einhaltung der bestehenden Contracts und Hard Constraints verwendet werden.
18. Ressourcen-Recovery MUSS Recovery Budgets und Recovery Storm Protection berücksichtigen.
19. Nach Recovery MUSS die Stabilität der relevanten Ressourcenlage verifiziert werden.
20. Ein kontrollierter Degraded Mode MUSS möglich sein, wenn ausreichende normale Kapazität nicht wiederhergestellt werden kann.
21. Resource-Pressure-, Recovery- und Degradation-Entscheidungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Ressourcenknappheit kann behandelt werden, bevor das gesamte System instabil wird,
- Recovery verstärkt die Ressourcenerschöpfung nicht unkontrolliert,
- kritische Funktionen können bevorzugt erhalten bleiben,
- Reclamation erfolgt semantisch statt willkürlich,
- Backpressure verhindert unbegrenztes Wachstum von Arbeit,
- NovaOS kann kontrolliert in einen degradierten Zustand wechseln.

### Negative Konsequenzen

- Ressourcen benötigen zusätzliche Pressure- und Reclaimability-Metadaten,
- Recovery Reserves reduzieren geringfügig die für normale Workloads verfügbare Kapazität,
- systemweite Priorisierung benötigt koordinierte Resource Policies.

## Verworfene Alternativen

### Bei Resource Exhaustion betroffene Komponente neu starten

Verworfen, da ein Restart die zugrunde liegende Ressourcenknappheit nicht zwangsläufig beseitigt und zusätzliche Ressourcen benötigen kann.

### Beliebige Prozesse beenden, bis genügend Ressourcen verfügbar sind

Verworfen, da dies Kritikalität, State Ownership und semantische Auswirkungen ignoriert.

### Ressourcen vollständig auslasten und erst dann reagieren

Verworfen, da nach vollständiger Exhaustion möglicherweise nicht mehr genügend Ressourcen für eine kontrollierte Recovery verfügbar sind.

### Unbegrenzte Queues als Lastpuffer

Verworfen, da dadurch Ressourcenverbrauch lediglich verschoben und letztlich verstärkt wird.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0006_Retry_Budgets_Backoff_und_Jitter`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0013_Recovery_Storm_Protection`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-RESOURCE-EXHAUSTION-0001`
- `NPSPEC-RESILIENCE-RESOURCE-PRESSURE-0001`
- `NPSPEC-RESILIENCE-RESOURCE-RECLAIM-0001`
- `NPSPEC-RESILIENCE-RECOVERY-RESERVE-0001`
- `NPSPEC-RESILIENCE-RESOURCE-BACKPRESSURE-0001`
- `NPSPEC-RESILIENCE-DEGRADED-RESOURCE-MODE-0001`
- `NPSPEC-RESILIENCE-RESOURCE-EXHAUSTION-TEST-0001`

## Ergebnis

Nova.Resilience behandelt Ressourcenerschöpfung nicht als gewöhnlichen Crash, sondern als kontrollierbaren systemweiten Ressourcenzustand:

```text
Resource Pressure
       ↓
Detect + Classify
       ↓
Contain Growth
       ↓
Reclaim + Throttle + Prioritize
       ↓
Verify Resource Stability
       ├── Stable → Controlled Restore
       └── Insufficient
                ↓
          Degraded Mode / Escalation
```

Damit kann NovaOS bei Ressourcenknappheit zunächst seine Stabilität sichern, gezielt Kapazität zurückgewinnen und kritische Funktionen erhalten, anstatt durch aggressive Recovery die ursprüngliche Ressourcenerschöpfung weiter zu verstärken.