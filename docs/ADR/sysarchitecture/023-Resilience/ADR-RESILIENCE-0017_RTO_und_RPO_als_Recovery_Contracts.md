# ADR-RESILIENCE-0017 – RTO und RPO als Recovery Contracts

## Status

Angenommen

## Kategorie

Resilience Architecture / Recovery Objectives / Contracts

## Kontext

Nicht jede NovaOS-Komponente besitzt dieselben Anforderungen an Wiederherstellungszeit und zulässigen Zustandsverlust.

Ein UI-Dienst kann möglicherweise mehrere Sekunden Recovery tolerieren, während ein sicherheitskritischer Steuerungsdienst deutlich strengere Anforderungen besitzt. Ebenso kann ein Cache problemlos vollständig rekonstruiert werden, während persistente Nutzerdaten nahezu keinen Verlust tolerieren.

Ohne explizite Recovery-Ziele kann Nova.Resilience nicht zuverlässig entscheiden:

- wie schnell Recovery erfolgen muss,
- wie viel State Loss zulässig ist,
- welche Checkpoints benötigt werden,
- wie viel Redundanz sinnvoll ist,
- wann Degraded Mode akzeptabel ist,
- welche Recovery-Strategie zulässig ist.

## Entscheidung

NovaOS behandelt **Recovery Time Objective (RTO)** und **Recovery Point Objective (RPO)** als explizite Recovery Contracts.

```text
Component / Service / Data
          ↓
Recovery Contract
     ├── RTO
     └── RPO
          ↓
Nova.Resilience Planning
```

RTO und RPO werden nicht nur dokumentiert, sondern können von Nova.Resilience aktiv für Planung, Recovery und Verification verwendet werden.

## Grundprinzip

```text
RTO = How quickly must recovery complete?

RPO = How much state loss is acceptable?
```

Beide Werte beschreiben unterschiedliche Anforderungen und dürfen nicht miteinander vermischt werden.

## Recovery Time Objective

RTO definiert die maximal akzeptierte Zeit bis zur Wiederherstellung eines definierten Betriebszustands.

```text
Failure
   ↓
Recovery
   ↓
Restore
   ↓
Elapsed Time ≤ RTO
```

Der Zielzustand kann abhängig vom Contract entweder:

```text
Normal Operation
```

oder:

```text
Verified Degraded Operation
```

sein.

## Recovery Point Objective

RPO definiert den maximal zulässigen Verlust von relevantem Zustand.

Beispiel:

```text
RPO = 0
→ kein bestätigter State Loss zulässig

RPO = 30 s
→ maximal 30 Sekunden bestätigter Zustand dürfen verloren gehen
```

RPO muss zur Semantik der jeweiligen Daten oder Komponente passen.

## Contract Scope

Recovery Contracts müssen an geeignete Scopes gebunden werden können.

Beispiele:

```text
Task
Workload
Service
Application
Storage Object
Dataset
Capability Provider
Subsystem
```

Unterschiedliche Scopes können unterschiedliche RTO- und RPO-Werte besitzen.

## Hierarchische Verträge

Recovery Contracts können hierarchisch wirken.

```text
Subsystem
   ↓
Service
   ↓
Workload
```

Ein untergeordneter Contract darf die harten Recovery-Ziele eines übergeordneten kritischen Systems nicht unkontrolliert verletzen.

Die effektive Recovery-Anforderung ergibt sich aus dem jeweiligen Scope und seinen Abhängigkeiten.

## RTO und Recovery Scope

RTO beeinflusst die Auswahl des Recovery Scope.

```text
Failure
   ↓
Candidate Recovery A → 100 ms
Candidate Recovery B → 2 s
Candidate Recovery C → 15 s
```

Nova.Resilience darf nur Strategien auswählen, die den geltenden RTO-Contract erfüllen können, sofern dieser als Hard Constraint definiert ist.

## RPO und Checkpointing

RPO beeinflusst Semantic Checkpointing und Persistenzstrategien.

```text
Required RPO
    ↓
Checkpoint / Replication Strategy
    ↓
Recoverable State
```

Ein strenges RPO kann beispielsweise häufigere Checkpoints, Replikation oder transaktionale Persistenz erforderlich machen.

Die konkrete technische Umsetzung bleibt getrennt vom Recovery Contract.

## RPO und irreversible Side Effects

Nicht jeder Zustand kann durch einen Snapshot wiederhergestellt werden.

Externe oder irreversible Side Effects müssen bei der Bewertung des tatsächlichen RPO berücksichtigt werden.

```text
Local State Restored
      ≠
Complete Semantic State Restored
```

RPO bezieht sich deshalb auf den semantisch relevanten Zustand und nicht ausschließlich auf gespeicherte Bytes.

## Hard und Soft Objectives

RTO und RPO können als unterschiedliche Contract-Stärken definiert werden.

Beispielsweise:

```text
Hard RTO
Preferred RTO

Hard RPO
Preferred RPO
```

Ein Hard Objective darf nicht durch Optimierung oder adaptive Verfahren abgeschwächt werden.

Preferred Objectives können zur Auswahl zwischen mehreren zulässigen Recovery-Strategien dienen.

## Recovery Planning

Nova.Resilience berücksichtigt RTO und RPO bereits bei der Planung.

```text
Failure Classification
        +
Recovery Scope
        +
RTO
        +
RPO
        +
Available Recovery Actions
        ↓
Recovery Plan
```

Eine Recovery-Strategie, die zwar technisch möglich ist, aber die Contract-Ziele offensichtlich nicht erfüllen kann, soll nicht als reguläre Lösung ausgewählt werden.

## Verification

Nach Recovery muss NovaOS prüfen können, ob die Recovery Contracts eingehalten wurden.

```text
Recovery Completed
       ↓
Actual Recovery Time
       +
Actual State Loss
       ↓
Contract Verification
```

Ein technisch erfolgreicher Restore kann trotzdem als Contract Violation gelten.

## Contract Violation

Kann RTO oder RPO nicht eingehalten werden, muss dies explizit behandelt werden.

Mögliche Reaktionen sind:

```text
Escalation
Degraded Mode
Safe State
Alternative Provider
Failover
Administrative Notification
```

Eine Verletzung darf nicht stillschweigend als erfolgreiche Recovery dargestellt werden.

## Resource Economy

Strenge RTO- und RPO-Anforderungen können zusätzliche Ressourcen benötigen.

Beispiele:

```text
Reserved Capacity
Replication
Frequent Checkpoints
Hot Standby
Additional Memory
Additional Storage
```

Recovery Contracts müssen deshalb mit der systemweiten Ressourcenökonomie abgestimmt werden.

Ein sehr strenges RTO oder RPO ist keine kostenlose Eigenschaft.

## Capability Provider

Capability Provider können eigene Recovery-Eigenschaften deklarieren.

```text
Provider A
RTO = 50 ms
RPO = 0

Provider B
RTO = 2 s
RPO = 5 s
```

Capability Resolution kann diese Eigenschaften berücksichtigen, wenn der Consumer entsprechende Recovery Requirements verlangt.

## Distributed Systems

Bei verteilten Diensten müssen RTO und RPO systemübergreifend betrachtet werden.

```text
Service
 ├── Local Component
 ├── Remote Provider
 └── Storage
```

Die langsamste oder verlustreichste kritische Abhängigkeit kann das effektiv erreichbare Recovery Objective bestimmen.

Location Transparency darf diese Grenzen nicht verbergen.

## Adaptive Systeme

Adaptive Systeme dürfen Recovery-Strategien optimieren, um RTO und RPO besser einzuhalten.

Sie dürfen jedoch Hard Recovery Objectives nicht selbstständig verändern.

Prediction Error kann genutzt werden, um zukünftige Recovery-Zeit- oder State-Loss-Schätzungen zu verbessern.

## Normative Anforderungen

1. NovaOS MUSS RTO und RPO als explizite Recovery Contracts unterstützen.
2. RTO und RPO MÜSSEN semantisch getrennte Eigenschaften bleiben.
3. Recovery Contracts MÜSSEN an unterschiedliche System- und Daten-Scopes bindbar sein.
4. RTO MUSS die maximal zulässige Wiederherstellungszeit ausdrücken können.
5. RPO MUSS den maximal zulässigen semantisch relevanten State Loss ausdrücken können.
6. RTO und RPO MÜSSEN als Hard oder Preferred Objectives definierbar sein können.
7. Hard RTO und Hard RPO DÜRFEN durch Optimierung oder adaptive Verfahren nicht abgeschwächt werden.
8. Nova.Resilience MUSS RTO und RPO bei der Recovery-Planung berücksichtigen.
9. Semantic Checkpointing MUSS RPO-Anforderungen berücksichtigen können.
10. Recovery Scope MUSS RTO-Anforderungen berücksichtigen können.
11. Irreversible und externe Side Effects MÜSSEN bei der Bewertung des tatsächlichen RPO berücksichtigt werden.
12. Recovery Verification MUSS tatsächliche Recovery Time und relevanten State Loss gegen den Contract prüfen können.
13. Eine RTO- oder RPO-Verletzung DARF nicht stillschweigend als vollständige erfolgreiche Recovery gelten.
14. Contract Violations MÜSSEN Eskalation, Degraded Mode oder andere definierte Reaktionen auslösen können.
15. Recovery Contracts MÜSSEN mit Resource Economy abgestimmt werden können.
16. Capability Provider MÜSSEN RTO- und RPO-relevante Eigenschaften deklarieren können.
17. Capability Resolution MUSS Recovery Requirements berücksichtigen können.
18. Verteilte Recovery MUSS die Recovery-Eigenschaften kritischer Remote Dependencies berücksichtigen.
19. Adaptive Systeme DÜRFEN Recovery-Ziele optimieren, aber Hard Objectives nicht verändern.
20. RTO-, RPO- und Contract-Violation-Ereignisse MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Recovery-Ziele werden explizit und maschinenlesbar,
- Nova.Resilience kann Recovery-Strategien zielgerichteter auswählen,
- Checkpointing und Replikation können aus tatsächlichen RPO-Anforderungen abgeleitet werden,
- Provider lassen sich nach Recovery-Eigenschaften auswählen,
- Contract-Verletzungen werden sichtbar statt als erfolgreiche Recovery kaschiert.

### Negative Konsequenzen

- Komponenten müssen realistische Recovery Objectives definieren,
- strenge RTO-/RPO-Ziele können zusätzliche Ressourcen erfordern,
- verteilte Systeme benötigen End-to-End-Betrachtung der Recovery-Eigenschaften.

## Verworfene Alternativen

### Einheitliches RTO und RPO für das gesamte System

Verworfen, da unterschiedliche Komponenten stark unterschiedliche Recovery-Anforderungen besitzen.

### RTO und RPO nur als Dokumentation

Verworfen, da Nova.Resilience diese Ziele aktiv für Planung und Verification verwenden soll.

### RPO ausschließlich als Backup-Intervall

Verworfen, da semantischer State Loss auch Transaktionen, externe Side Effects und verteilte Zustände umfasst.

### RTO als reine Performance-Metrik

Verworfen, da RTO eine verbindliche Recovery-Anforderung und nicht nur ein Optimierungswert sein kann.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0009_Semantic_Checkpointing`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0016_Safe_State_für_Safety_und_Realtime`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-RECOVERY-CONTRACT-0001`
- `NPSPEC-RESILIENCE-RTO-0001`
- `NPSPEC-RESILIENCE-RPO-0001`
- `NPSPEC-RESILIENCE-RECOVERY-OBJECTIVE-HIERARCHY-0001`
- `NPSPEC-RESILIENCE-RECOVERY-CONTRACT-VERIFICATION-0001`
- `NPSPEC-RESILIENCE-RECOVERY-CONTRACT-VIOLATION-0001`
- `NPSPEC-RESILIENCE-RECOVERY-CONTRACT-TEST-0001`

## Ergebnis

Nova.Resilience behandelt Recovery-Ziele als explizite Verträge:

```text
Recovery Contract
   ├── RTO
   │    → maximale Wiederherstellungszeit
   │
   └── RPO
        → maximal zulässiger State Loss
              ↓
       Recovery Planning
              ↓
           Recovery
              ↓
         Verification
              ↓
     Contract satisfied?
       ├── Yes → Restore
       └── No  → Escalate / Degrade / Safe State
```

Damit wird Recovery in NovaOS nicht nur danach bewertet, **ob** eine Komponente wieder funktioniert, sondern auch danach, **wie schnell** sie wiederhergestellt wurde und **wie viel semantisch relevanter Zustand dabei verloren gegangen ist**.