# ADR-RESILIENCE-0007 – Systemweite Circuit Breaker

## Status

Angenommen

## Kategorie

Resilience Architecture / Circuit Breaker / Failure Containment

## Kontext

Wenn ein Dienst, Capability Provider, Gerät oder Remote-System wiederholt fehlschlägt, können weitere Aufrufe den Fehler verstärken.

Insbesondere in Verbindung mit automatischen Retries können dadurch:

- Ressourcen unnötig verbraucht,
- Warteschlangen aufgebaut,
- Deadlines überschritten,
- abhängige Komponenten blockiert,
- Retry Storms ausgelöst,
- Cascading Failures verursacht

werden.

Lokale Circuit Breaker einzelner Anwendungen reichen für NovaOS nicht aus, da mehrere unabhängige Workloads denselben fehlerhaften Provider oder dieselbe Ressource verwenden können.

## Entscheidung

Nova.Resilience stellt **systemweit koordinierbare Circuit Breaker** bereit.

```text
Operations
    ↓
Failure Signals
    ↓
Circuit Breaker
    ↓
┌────────┬───────────┬──────────┐
│ Closed │ Half-Open │   Open   │
└────────┴───────────┴──────────┘
```

Circuit Breaker werden mit Failure Domains, Capability Resolution, Retry Policies und Recovery Planning integriert.

## Grundprinzip

```text
Known unhealthy dependency
        ↓
Stop repeated pressure
        ↓
Allow controlled recovery probe
```

Ein bereits als nicht verfügbar erkannter Provider soll nicht von jeder abhängigen Workload erneut belastet werden.

## Zustände

Ein Circuit Breaker besitzt mindestens die Zustände:

```text
Closed
Half-Open
Open
```

### Closed

Operationen werden normal zugelassen.

Fehlersignale werden beobachtet.

### Open

Weitere passende Operationen werden vorübergehend abgewiesen oder auf alternative Provider umgeleitet.

```text
Request
   ↓
Circuit Open
   ├── Alternative Provider
   └── Fail Fast
```

### Half-Open

Eine begrenzte Anzahl kontrollierter Probe-Operationen wird zugelassen.

```text
Open
 ↓
Recovery Condition
 ↓
Half-Open
 ├── Success → Closed
 └── Failure → Open
```

## Systemweite Koordination

Circuit-Breaker-Zustände können systemweit geteilt werden, wenn mehrere Workloads dieselbe Failure Domain oder denselben Provider verwenden.

```text
Workload A ─┐
Workload B ─┼→ Provider X
Workload C ─┘
                 ↓
             Failure
                 ↓
       Shared Circuit State
```

Dadurch müssen nicht alle Workloads denselben Fehler unabhängig erneut entdecken.

## Scope

Circuit Breaker müssen einen expliziten Scope besitzen.

Beispiele:

```text
Operation
Capability Provider
Service
Device
Remote Endpoint
Failure Domain
```

Ein Fehler in einer einzelnen Operation darf nicht automatisch alle Operationen eines Providers sperren, wenn diese unabhängig funktionieren können.

## Failure Classification

Nicht jeder Fehler darf einen Circuit Breaker beeinflussen.

Geeignete Signale können beispielsweise sein:

```text
Repeated Timeout
Provider Unavailable
Connection Failure
Resource Exhaustion
Health Check Failure
Repeated Internal Failure
```

Fehler wie:

```text
Invalid Input
Permission Denied
Capability Missing
Policy Violation
```

dürfen nicht automatisch als Provider-Ausfall interpretiert werden.

## Integration mit Retry

Circuit Breaker und Retry werden gemeinsam ausgewertet.

```text
Operation Failure
       ↓
Retry Semantics
       ↓
Circuit State
       ↓
Retry Budget
       ↓
Retry / Fail Fast / Alternative Provider
```

Ein offener Circuit verhindert, dass Retry Policies weiterhin unkontrolliert Last auf eine bekannte fehlerhafte Abhängigkeit erzeugen.

## Capability Provider Replacement

Ist ein Circuit für einen Capability Provider geöffnet, kann die Capability Registry nach einem kompatiblen alternativen Provider suchen.

```text
Provider A
   ↓
Circuit Open
   ↓
Capability Resolution
   ↓
Provider B
```

Der alternative Provider muss weiterhin alle Anforderungen des ursprünglichen Capability- und Execution Contracts erfüllen.

## Half-Open Probes

Probe-Operationen im Half-Open-Zustand müssen begrenzt sein.

Sie dürfen nicht dazu führen, dass alle wartenden Workloads gleichzeitig erneut auf den Provider zugreifen.

Die Probe muss eine geeignete Operation verwenden, deren Ausführung selbst sicher ist.

## Recovery und Health

Ein Circuit darf nicht allein aufgrund abgelaufener Zeit automatisch als gesund betrachtet werden.

Zeit kann den Übergang zu `Half-Open` erlauben, die Rückkehr zu `Closed` benötigt jedoch ausreichende positive Evidence.

```text
Open
 ↓
Wait / Recovery Signal
 ↓
Half-Open
 ↓
Verified Success
 ↓
Closed
```

## Hierarchische Circuit Breaker

Circuit Breaker können entlang hierarchischer Failure Domains organisiert werden.

```text
Subsystem
   ↓
Service
   ↓
Provider
   ↓
Operation
```

Ein übergeordneter Circuit kann untergeordnete Operationen blockieren, wenn die gesamte Failure Domain betroffen ist.

Lokale Fehler sollen dagegen möglichst nur den tatsächlich betroffenen Scope öffnen.

## Distributed Systems

Für entfernte Dienste können Circuit-Breaker-Zustände lokal oder kontrolliert geteilt werden.

Ein entfernter Dienst darf den lokalen Circuit-Breaker-Zustand jedoch nicht eigenmächtig bestimmen.

Lokale Nova.Resilience Policies bleiben maßgeblich.

## Deterministic Mode

Im Deterministic Mode müssen relevante Zustandsübergänge und Probe-Entscheidungen reproduzierbar beziehungsweise nachvollziehbar sein.

Zeitbasierte und adaptive Mechanismen dürfen die geforderten Determinismusgarantien nicht verletzen.

## Normative Anforderungen

1. Nova.Resilience MUSS systemweit koordinierbare Circuit Breaker unterstützen.
2. Circuit Breaker MÜSSEN mindestens `Closed`, `Open` und `Half-Open` unterstützen.
3. Circuit Breaker MÜSSEN einen expliziten Scope besitzen.
4. Failure Classification MUSS bestimmen können, welche Fehler den Circuit beeinflussen.
5. Client- oder Policy-Fehler DÜRFEN nicht automatisch als Provider-Ausfall gewertet werden.
6. Ein offener Circuit MUSS weitere passende automatische Retries begrenzen oder verhindern.
7. Ein offener Circuit MUSS alternative Recovery oder Provider Resolution ermöglichen können.
8. Alternative Provider MÜSSEN weiterhin den ursprünglichen Contracts und Hard Constraints entsprechen.
9. Half-Open Probes MÜSSEN in Anzahl und Ressourcenverbrauch begrenzt sein.
10. Die Rückkehr zu `Closed` MUSS ausreichende positive Evidence erfordern.
11. Ein reiner Zeitablauf DARF nicht automatisch einen gesunden Zustand beweisen.
12. Circuit Breaker MÜSSEN mit hierarchischen Failure Domains integrierbar sein.
13. Systemweit geteilter Circuit State MUSS den tatsächlichen Failure Scope berücksichtigen.
14. Circuit Breaker DÜRFEN Security-, Trust-, Capability- oder Sovereignty-Entscheidungen nicht umgehen.
15. Deterministic Mode MUSS kontrollierbare Circuit-State-Transitions ermöglichen.
16. Circuit-Zustände, Zustandsübergänge und relevante Ursachen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- bekannte fehlerhafte Provider werden nicht weiter unnötig belastet,
- Retry Storms und Cascading Failures werden reduziert,
- mehrere Workloads können gemeinsame Failure Evidence nutzen,
- alternative Capability Provider können frühzeitig verwendet werden,
- Recovery eines Providers wird kontrolliert geprüft,
- Circuit Breaker integrieren sich in Failure Domains und Nova.Resilience.

### Negative Konsequenzen

- systemweit geteilte Circuit-Zustände benötigen Koordination,
- falsch gewählte Scopes können gesunde Operationen unnötig blockieren,
- Half-Open-Probes benötigen sorgfältige Operationssemantik.

## Verworfene Alternativen

### Circuit Breaker ausschließlich pro Anwendung

Verworfen, da mehrere Anwendungen denselben fehlerhaften System- oder Capability Provider unabhängig belasten könnten.

### Nach Timeout immer Circuit öffnen

Verworfen, da einzelne transiente Fehler nicht automatisch einen Provider-Ausfall bedeuten.

### Circuit nach festem Timeout automatisch schließen

Verworfen, da vergangene Zeit keine Evidence für die Wiederherstellung des Providers darstellt.

### Circuit Breaker statt Retry Budgets

Verworfen, da beide unterschiedliche Aufgaben erfüllen und sich ergänzen.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0005_Retry_nur_nach_expliziter_Operationssemantik`
- `ADR-RESILIENCE-0006_Retry_Budgets_Backoff_und_Jitter`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-ARCH-0010_Deterministic_Mode`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-CIRCUIT-BREAKER-0001`
- `NPSPEC-RESILIENCE-CIRCUIT-STATE-0001`
- `NPSPEC-RESILIENCE-CIRCUIT-SCOPE-0001`
- `NPSPEC-RESILIENCE-CIRCUIT-PROBE-0001`
- `NPSPEC-RESILIENCE-CIRCUIT-COORDINATION-0001`
- `NPSPEC-RESILIENCE-CIRCUIT-TEST-0001`

## Ergebnis

Nova.Resilience verwendet Circuit Breaker als systemweiten Schutz gegen wiederholte Zugriffe auf bekannte fehlerhafte Abhängigkeiten:

```text
Requests
   ↓
Failure Evidence
   ↓
Circuit Breaker
   ├── Closed    → Normal Operation
   ├── Open      → Fail Fast / Alternative Provider
   └── Half-Open → Controlled Probe
                         ↓
                      Verify
                    ┌────┴────┐
                 Healthy    Failed
                    ↓          ↓
                  Closed      Open
```

Damit werden Circuit Breaker nicht als isolierte Anwendungsmechanismen behandelt, sondern als Bestandteil der systemweiten Failure-Containment-, Retry- und Recovery-Architektur von NovaOS.