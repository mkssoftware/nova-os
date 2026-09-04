# ADR-RESILIENCE-0008 – Health State Model

## Status

Angenommen

## Kategorie

Resilience Architecture / Health Monitoring / State Model

## Kontext

Nova.Resilience benötigt eine einheitliche Darstellung des Gesundheitszustands von Systemkomponenten.

Ein einfaches binäres Modell aus `Healthy` und `Failed` reicht nicht aus. Komponenten können beispielsweise noch funktionieren, aber bereits degradiert sein, sich in Recovery befinden oder aufgrund unklarer Informationen keinen zuverlässig bestimmbaren Zustand besitzen.

Ohne ein gemeinsames Health State Model würden Dienste, Workloads, Capability Provider, Treiber und andere Komponenten unterschiedliche Gesundheitsbegriffe verwenden und Recovery-Entscheidungen erschweren.

## Entscheidung

NovaOS definiert ein systemweites **Health State Model** als gemeinsame Grundlage für Nova.Resilience.

Der grundlegende Zustandsraum lautet:

```text
Unknown
Healthy
Degraded
Unhealthy
Recovering
Failed
```

Diese Zustände beschreiben den beobachteten Betriebszustand einer Komponente und sind von Trust, Security und Availability getrennt.

## Grundprinzip

```text
Health State
≠
Trust State
≠
Security State
```

Eine gesunde Komponente ist nicht automatisch vertrauenswürdig.

Eine vertrauenswürdige Komponente ist nicht automatisch funktionsfähig.

## Zustände

### Unknown

Der Gesundheitszustand kann aktuell nicht zuverlässig bestimmt werden.

```text
Unknown ≠ Healthy
Unknown ≠ Failed
```

`Unknown` darf nicht automatisch positiv interpretiert werden.

### Healthy

Die Komponente erfüllt ihre definierten Health- und Betriebsanforderungen.

```text
Observed State
      ↓
Health Requirements satisfied
      ↓
Healthy
```

### Degraded

Die Komponente funktioniert weiterhin, erfüllt jedoch nicht mehr ihren vollständigen vorgesehenen Betriebszustand.

Beispiele:

```text
Reduced Performance
Partial Provider Loss
Limited Redundancy
Resource Pressure
Reduced Functionality
```

Ein degradiertes System kann weiterhin verwendet werden, sofern Contracts und Policies dies zulassen.

### Unhealthy

Die Komponente zeigt einen Fehlerzustand oder verletzt relevante Health-Anforderungen, kann aber möglicherweise noch lokal wiederhergestellt werden.

```text
Unhealthy
    ↓
Nova.Resilience
    ↓
Recovery
```

### Recovering

Für die Komponente läuft aktuell eine Recovery.

```text
Unhealthy
    ↓
Recovering
    ↓
Verify
```

`Recovering` bedeutet nicht, dass die Recovery erfolgreich sein wird.

### Failed

Die Komponente kann innerhalb ihres aktuellen Recovery Scope nicht in einen gültigen Betriebszustand gebracht werden.

```text
Failed
   ↓
Escalation / Replacement / Isolation
```

`Failed` kann eine Eskalation auf eine übergeordnete Failure Domain auslösen.

## Zustandsübergänge

Health States besitzen explizite Übergänge.

Beispielsweise:

```text
Unknown
   ↓
Healthy
   ↓
Degraded
   ↓
Unhealthy
   ↓
Recovering
   ├──→ Healthy
   ├──→ Degraded
   └──→ Failed
```

Direkte Übergänge bleiben zulässig, wenn die zugrunde liegende Evidence dies rechtfertigt.

Beispiel:

```text
Healthy → Failed
```

bei einem unmittelbaren vollständigen Hardwareausfall.

## Health Evidence

Health States werden aus beobachtbarer Evidence abgeleitet.

Beispiele:

```text
Health Checks
Heartbeats
Timeouts
Invariant Checks
Resource State
Error Rates
Dependency State
Integrity Signals
Hardware Events
```

Die konkrete Bewertung wird durch komponenten- beziehungsweise domänenspezifische Health Policies definiert.

## Health und Dependencies

Der Health State einer Komponente kann vom Zustand ihrer Abhängigkeiten beeinflusst werden.

```text
Service
 ├── Provider A → Healthy
 ├── Provider B → Failed
 └── Storage    → Healthy

Service → Degraded
```

Ein fehlerhafter Dependency State muss jedoch nicht automatisch denselben Zustand auf die abhängige Komponente übertragen.

Die tatsächliche Auswirkung wird anhand der Abhängigkeitssemantik bestimmt.

## Aggregation

Health States müssen hierarchisch aggregierbar sein.

```text
Subsystem
   ├── Service A → Healthy
   ├── Service B → Degraded
   └── Service C → Healthy
```

Der aggregierte Zustand darf nicht ausschließlich durch eine einfache Worst-State-Regel bestimmt werden.

Kritikalität, Redundanz und Abhängigkeiten müssen berücksichtigt werden können.

## Health und Capability Provider

Capability Provider veröffentlichen einen für die Provider-Auswahl nutzbaren Health State.

```text
Capability Resolution
        ↓
Provider Candidates
        ↓
Health State
        ↓
Eligible Providers
```

Ein `Failed` Provider darf nicht für neue Operationen ausgewählt werden.

Ein `Degraded` Provider kann verwendet werden, wenn der Execution Contract und die geltenden Policies dies erlauben.

## Health und Circuit Breaker

Health State und Circuit Breaker bleiben getrennte Konzepte.

```text
Health State
    ↓
Failure Evidence
    ↓
Circuit Breaker Policy
```

Ein `Unhealthy` Provider kann zur Öffnung eines Circuit Breakers beitragen, erzeugt aber nicht zwingend automatisch einen offenen Circuit.

## Health und Recovery

Nova.Resilience verwendet Health States zur Steuerung und Verifikation von Recovery.

```text
Unhealthy
    ↓
Recovering
    ↓
Recovery Action
    ↓
Verify
    ↓
Healthy / Degraded / Failed
```

Eine Recovery darf nicht allein aufgrund ihrer erfolgreichen Ausführung den Zustand `Healthy` setzen.

Die Health-Anforderungen müssen erneut verifiziert werden.

## Staleness

Health Evidence besitzt eine zeitliche Gültigkeit.

Veraltete Informationen dürfen nicht unbegrenzt als aktueller Health State verwendet werden.

```text
Health Evidence
      ↓
Freshness
      ↓
Current Health State
```

Kann ein Zustand aufgrund veralteter Evidence nicht mehr zuverlässig bestimmt werden, kann der Health State zu `Unknown` wechseln.

## Normative Anforderungen

1. NovaOS MUSS ein systemweit einheitliches Health State Model bereitstellen.
2. Das Modell MUSS mindestens `Unknown`, `Healthy`, `Degraded`, `Unhealthy`, `Recovering` und `Failed` unterscheiden.
3. Health State MUSS von Trust-, Security- und Authority-Zuständen getrennt bleiben.
4. `Unknown` DARF nicht automatisch als `Healthy` interpretiert werden.
5. `Degraded` MUSS einen weiterhin nutzbaren, aber eingeschränkten Betriebszustand darstellen können.
6. `Recovering` DARF nicht als erfolgreiche Recovery interpretiert werden.
7. Health States MÜSSEN aus nachvollziehbarer Health Evidence ableitbar sein.
8. Dependency Health MUSS bei der Bewertung abhängiger Komponenten berücksichtigt werden können.
9. Dependency Health DARF nicht ohne Berücksichtigung der Abhängigkeitssemantik direkt übertragen werden.
10. Health States MÜSSEN hierarchisch aggregierbar sein.
11. Health Aggregation MUSS Kritikalität und Redundanz berücksichtigen können.
12. Capability Resolution MUSS Provider Health berücksichtigen können.
13. `Failed` Provider DÜRFEN nicht für neue normale Operationen ausgewählt werden.
14. Health State und Circuit-Breaker-State MÜSSEN getrennte Konzepte bleiben.
15. Eine Recovery DARF erst nach Verification einen neuen bestätigten Health State erzeugen.
16. Health Evidence MUSS Freshness beziehungsweise Staleness berücksichtigen können.
17. Health-State-Änderungen MÜSSEN relevante Recovery- oder Reevaluation-Prozesse auslösen können.
18. Health States und relevante Zustandsübergänge MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- einheitliche Health-Semantik für NovaOS,
- degradierte Zustände können ohne sofortigen Totalausfall dargestellt werden,
- Recovery erhält klar definierte Ausgangs- und Zielzustände,
- Capability Resolution kann Provider Health berücksichtigen,
- Health-Informationen können über Failure Domains aggregiert werden.

### Negative Konsequenzen

- Komponenten müssen geeignete Health Evidence bereitstellen,
- Health Aggregation benötigt Wissen über Abhängigkeiten und Kritikalität,
- verteilte Komponenten benötigen Mechanismen zur Behandlung veralteter Health-Daten.

## Verworfene Alternativen

### Nur Healthy und Failed

Verworfen, da degradierte, unbekannte und laufende Recovery-Zustände nicht ausreichend dargestellt werden könnten.

### Numerischer globaler Health Score

Verworfen, da unterschiedliche Fehlerarten und Betriebszustände nicht sinnvoll auf eine universelle Zahl reduziert werden können.

### Health direkt aus Trust ableiten

Verworfen, da Vertrauenswürdigkeit und technische Funktionsfähigkeit unterschiedliche Eigenschaften sind.

### Worst-State-Aggregation für alle Hierarchien

Verworfen, da redundante oder nicht kritische Komponenten ansonsten unnötig den gesamten übergeordneten Zustand verschlechtern würden.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0007_Systemweite_Circuit_Breaker`
- `ADR-CAPABILITY-0014_Capability_Provider_Austauschbarkeit`
- `ADR-ARCH-0013_Architecture_Introspection`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-HEALTH-STATE-0001`
- `NPSPEC-RESILIENCE-HEALTH-EVIDENCE-0001`
- `NPSPEC-RESILIENCE-HEALTH-TRANSITION-0001`
- `NPSPEC-RESILIENCE-HEALTH-AGGREGATION-0001`
- `NPSPEC-RESILIENCE-HEALTH-FRESHNESS-0001`
- `NPSPEC-RESILIENCE-HEALTH-PROVIDER-0001`
- `NPSPEC-RESILIENCE-HEALTH-TEST-0001`

## Ergebnis

Nova.Resilience verwendet ein gemeinsames Health State Model:

```text
Unknown
   ↓
Healthy
   ↓
Degraded
   ↓
Unhealthy
   ↓
Recovering
   ├──→ Healthy
   ├──→ Degraded
   └──→ Failed
```

Damit besitzt NovaOS eine systemweit einheitliche Sprache für den Betriebszustand von Komponenten und kann Detection, Capability Resolution, Circuit Breaker und Recovery auf denselben Health-Zuständen aufbauen.