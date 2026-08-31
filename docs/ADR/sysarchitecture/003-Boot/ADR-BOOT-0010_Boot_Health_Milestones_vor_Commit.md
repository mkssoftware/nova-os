# ADR-BOOT-0010 – Boot Health Milestones vor Commit

## Status

Angenommen

## Kategorie

Boot / Health / A/B-Boot / Rollback / Known-Good / Recovery / Resilienz / Updates

## Kontext

Mit `ADR-BOOT-0009_A_B_Boot_und_Boot_Rollback` ist festgelegt, dass eine neue NovaOS-Systemgeneration zunächst als Candidate gestartet und erst nach erfolgreicher Health Validation zu `Known-Good` erklärt wird.

Dafür reicht ein einzelnes Signal wie:

```text
Kernel gestartet
```

nicht aus.

Ein System kann den Kernel erfolgreich laden und trotzdem wenige Sekunden später scheitern, beispielsweise durch:

- fehlgeschlagene Speicherinitialisierung,
- nicht verfügbare Systempartition,
- beschädigte Systemdienste,
- fehlerhafte Treiber,
- fehlende kritische Capabilities,
- nicht erreichbaren stabilen Betriebszustand.

Würde ein Candidate bereits nach Kernel Entry committed, könnte eine defekte Generation die vorherige Known-Good-Generation verdrängen.

NovaOS benötigt deshalb definierte **Boot Health Milestones**, die den Fortschritt eines Bootvorgangs beschreiben und bestimmen, wann eine Candidate-Generation tatsächlich als erfolgreich gestartet gilt.

---

## Entscheidung

NovaOS verwendet eine **mehrstufige Boot-Health-Bewertung vor dem Commit einer Candidate-Generation**.

Ein Boot durchläuft definierte Health Milestones.

Konzeptionell:

```text
Boot Started
     ↓
Kernel Reached
     ↓
Kernel Stable
     ↓
System Root Available
     ↓
Critical Services Ready
     ↓
System Operational
     ↓
Health Confirmed
     ↓
Commit Candidate
```

Ein Candidate darf erst zu `Known-Good` werden, wenn alle für die aktive Boot Policy erforderlichen Milestones erfolgreich erreicht wurden.

Die zentrale Regel lautet:

```text
Boot success is a proven state,
not a successful jump into the kernel.
```

---

## Boot Health Milestones

NovaOS definiert semantische Bootphasen.

Die konkrete Implementierung darf zusätzliche Milestones einführen, mindestens müssen jedoch folgende Zustände unterscheidbar sein:

```text
BootStarted
KernelEntered
KernelInitialized
SystemRootReady
CriticalServicesReady
Operational
HealthConfirmed
```

Die Milestones beschreiben erreichte Systemzustände und nicht lediglich ausgeführte Funktionen.

---

## Milestone-Modell

Konzeptionell:

```text
BootHealthState {
    generation
    bootAttempt
    currentMilestone
    reachedMilestones
    failedMilestone
    healthStatus
}
```

Mögliche Health States sind mindestens:

```text
Pending
Healthy
Failed
TimedOut
Unknown
```

---

## BootStarted

`BootStarted` wird gesetzt, sobald ein konkreter Bootversuch für eine Systemgeneration begonnen wurde.

Damit kann NovaOS unterscheiden zwischen:

```text
Candidate selected
```

und:

```text
Candidate actually attempted
```

Ab diesem Zeitpunkt zählt der Vorgang als Bootversuch.

---

## KernelEntered

`KernelEntered` bedeutet, dass:

- das Kernelimage erfolgreich geladen,
- die erforderliche Bootverifikation abgeschlossen,
- der Kernel Handoff erfolgt,
- der Kernel Entry Point erreicht

wurde.

Dieser Milestone bestätigt lediglich den erfolgreichen Übergang vom Bootloader zum Kernel.

Er reicht nicht für einen Commit.

---

## KernelInitialized

`KernelInitialized` bedeutet, dass die grundlegenden Kernelmechanismen soweit initialisiert wurden, dass der Kernel seinen vorgesehenen stabilen Initialisierungszustand erreicht hat.

Dazu können gehören:

- Memory Management,
- Scheduler,
- Interruptsystem,
- grundlegende HAL,
- erforderliche Kernel Services.

Die exakten Voraussetzungen werden durch die Kernel-/Boot-Spezifikationen definiert.

---

## SystemRootReady

`SystemRootReady` bedeutet, dass die vorgesehene Systemgeneration als System Root verfügbar und ausreichend validiert ist.

Dies umfasst insbesondere den erfolgreichen Zugriff auf die für den weiteren Start erforderlichen Systemdaten.

Ein Kernel, der läuft, aber seine Systemgeneration nicht verwenden kann, gilt nicht als erfolgreicher Boot.

---

## CriticalServicesReady

Dieser Milestone bestätigt, dass die für einen funktionsfähigen NovaOS-Betrieb als kritisch definierten Systemdienste gestartet und einsatzfähig sind.

Nicht jeder Hintergrunddienst ist dafür erforderlich.

Die Menge kritischer Dienste wird durch das jeweilige Systemprofil definiert.

Beispiele können sein:

```text
Trust Service
Capability Runtime
Storage Services
System Configuration
Core IPC Services
```

Die konkrete Liste gehört nicht in diese ADR.

---

## Operational

`Operational` bedeutet, dass NovaOS seinen vorgesehenen grundlegenden Betriebszustand erreicht hat.

Bei einem Desktop-System kann dies beispielsweise bedeuten:

```text
Kernel
+
Critical Services
+
Session Infrastructure
```

Bei einem Server oder Embedded-System kann der entsprechende Zustand anders definiert sein.

Der Milestone ist daher semantisch und nicht an eine bestimmte grafische Oberfläche gebunden.

---

## HealthConfirmed

`HealthConfirmed` ist der finale für den A/B-Commit relevante Milestone.

Er bedeutet:

```text
required milestones reached
+
no blocking health failure
+
candidate generation accepted
```

Erst danach darf:

```text
Candidate
    ↓
KnownGood
```

erfolgen.

---

## Commit-Grenze

Die Commit-Grenze liegt ausdrücklich **nach** erfolgreicher Health Confirmation.

```text
Candidate Boot
      ↓
Required Milestones
      ↓
HealthConfirmed
      ↓
COMMIT
      ↓
KnownGood
```

Vor diesem Punkt muss die vorherige Known-Good-Generation als Rollbackziel erhalten bleiben.

---

## Milestone Policies

Nicht jedes NovaOS-System muss exakt dieselben Milestones für einen erfolgreichen Boot verlangen.

Die Boot Policy definiert deshalb:

```text
Required
Optional
Diagnostic
```

Milestones.

Beispiel:

```text
Required:
    KernelInitialized
    SystemRootReady
    CriticalServicesReady
    Operational

Optional:
    NetworkReady
```

Ein fehlendes optionales Netzwerk darf somit einen ansonsten erfolgreichen Offline-Boot nicht automatisch verhindern.

---

## Profilabhängigkeit

Health Policies können sich je nach Systemprofil unterscheiden.

Beispiele:

```text
Desktop
Server
Embedded
Recovery
NovaDOS
```

Ein Desktop muss nicht dieselben Health-Kriterien wie ein Headless Server besitzen.

Die grundlegende Milestone-Semantik bleibt jedoch systemweit einheitlich.

---

## Timeouts

Für erforderliche Milestones können Zeitgrenzen definiert werden.

Beispiel:

```text
CriticalServicesReady
Deadline: 30 s
```

Wird ein erforderlicher Milestone innerhalb seiner zulässigen Zeit nicht erreicht, kann der Bootversuch als:

```text
TimedOut
```

gewertet werden.

Timeouts sind Policy und dürfen nicht als willkürliche feste Konstanten über die Bootarchitektur verteilt werden.

---

## Kein reines Zeitkriterium

Ein System darf nicht allein deshalb als gesund gelten, weil es eine bestimmte Zeit lang nicht abgestürzt ist.

Nicht zulässig als alleinige Definition:

```text
System survived 60 seconds
    =
Healthy
```

Entscheidend sind explizite Zustandsnachweise.

Eine zusätzliche Stabilitätsperiode darf jedoch Teil einer Health Policy sein.

---

## Failure

Ein erforderlicher Milestone kann explizit fehlschlagen.

Beispiel:

```text
SystemRootReady
    ↓
Failed
```

In diesem Fall darf der Candidate nicht committed werden.

Abhängig von der Boot Policy kann:

- der aktuelle Versuch abgebrochen,
- ein weiterer Versuch erlaubt,
- Rollback aktiviert,
- Recovery gestartet

werden.

---

## Unknown State

Kann NovaOS nicht feststellen, ob ein erforderlicher Health Milestone erfolgreich erreicht wurde, gilt:

```text
Unknown
≠
Healthy
```

Ein unbekannter Required Health State darf keinen Candidate-Commit auslösen.

Dies folgt den Fail-Safe Defaults.

---

## Boot Attempt Tracking

Jeder Candidate-Boot besitzt eine eindeutige Attempt Identity.

Beispiel:

```text
Generation: 42
Attempt: 2
```

Milestones müssen dem konkreten Bootversuch zugeordnet werden.

Ein Milestone eines früheren Bootversuchs darf nicht als Nachweis für einen neuen Versuch wiederverwendet werden.

---

## Persistenz

Bestimmte Boot-Health-Informationen müssen crash-konsistent gespeichert werden können.

Dadurch kann der nächste Boot erkennen:

```text
Generation 42
Attempt 1
Last Milestone: KernelInitialized
No Health Confirmation
```

Dies ermöglicht eine präzisere Fehlerdiagnose und Rollbackentscheidung.

---

## Schreibhäufigkeit

Nicht jeder interne Initialisierungsschritt muss persistent gespeichert werden.

Die Milestone-Struktur soll bewusst grob genug bleiben, um:

- unnötige Storage Writes,
- Bootzeitkosten,
- Schreibverschleiß

zu vermeiden.

Feingranulare Diagnose gehört in Boot- und Systemlogs.

---

## Boot Failure Classification

Der zuletzt erreichte Milestone hilft bei der Einordnung eines Bootfehlers.

Beispiel:

```text
BootStarted
KernelEntered
KernelInitialized
SystemRootReady
CriticalServicesReady
    ↓
Failure before Operational
```

Dadurch kann Recovery erkennen, in welcher Phase der Start gescheitert ist.

---

## A/B-Rollback

Die Milestones liefern die Health-Grundlage für `ADR-BOOT-0009`.

Beispiel:

```text
Slot B
Generation 42
Candidate

KernelEntered        ✓
KernelInitialized    ✓
SystemRootReady      ✓
CriticalServicesReady✗

Result:
    Candidate not committed
```

Nach entsprechender Rollback Policy kann anschließend Slot A gestartet werden.

---

## Known-Good

`Known-Good` ist das Ergebnis eines erfolgreich abgeschlossenen Candidate-Lebenszyklus.

Es gilt:

```text
Verified
+
Booted
+
HealthConfirmed
=
eligible for Known-Good
```

Dabei bleibt Verified Boot weiterhin unabhängig von Health.

Ein funktionierendes, aber nicht vertrauenswürdiges System darf nicht Known-Good werden.

---

## Trust und Health

NovaOS trennt:

```text
Trust
```

von:

```text
Health
```

Ein Artefakt kann:

```text
Trusted but broken
```

oder:

```text
Functional but untrusted
```

sein.

Für einen normalen Candidate-Commit müssen beide relevanten Bedingungen erfüllt sein.

---

## Recovery

Recovery verwendet Boot Health Milestones zur Diagnose und Entscheidungsfindung.

Beispiel:

```text
Repeated failure at:
CriticalServicesReady
```

kann gezieltere Recoverymaßnahmen ermöglichen als ein generisches:

```text
Boot failed
```

Recovery darf eigene Milestone Policies besitzen.

---

## Self-Healing

Self-Healing kann Health-Milestones verwenden, um Fehlerbereiche einzugrenzen.

Beispielsweise:

```text
SystemRootReady reached
CriticalServicesReady failed
```

deutet auf eine andere Fehlerklasse hin als:

```text
KernelEntered not reached
```

Self-Healing darf aus einem Milestone allein jedoch keine irreversible Reparaturentscheidung ohne ausreichenden Nachweis ableiten.

---

## Boot Health Provider

Subsysteme dürfen ihren Health State über definierte Contracts melden.

Beispiel:

```text
HealthProvider {
    Check()
    GetState()
}
```

Die zentrale Boot Health Authority entscheidet anhand der Policy, wann ein übergeordneter Milestone erfüllt ist.

Ein einzelner Dienst darf nicht eigenständig die gesamte Generation als Known-Good markieren.

---

## Health Aggregation

Ein Milestone kann von mehreren Bedingungen abhängen.

Beispiel:

```text
CriticalServicesReady
        │
        ├── Trust Service Ready
        ├── Storage Service Ready
        └── Capability Runtime Ready
```

Erst wenn alle erforderlichen Bedingungen erfüllt sind, gilt der Milestone als erreicht.

---

## Graceful Degradation

Nicht jede eingeschränkte Funktion verhindert einen erfolgreichen Boot.

Beispiel:

```text
GPU unavailable
```

kann bei vorhandenem Software-Fallback einen degradierten, aber funktionsfähigen Zustand erlauben.

Die Health Policy muss deshalb zwischen:

```text
Blocking Failure
Degraded
Optional Failure
```

unterscheiden können.

---

## Security

Health Reports sind sicherheitsrelevant, da sie einen Candidate-Commit beeinflussen.

Normale Anwendungen dürfen deshalb nicht:

```text
HealthConfirmed
```

setzen oder Boot-Milestones manipulieren.

Nur autorisierte Systemkomponenten erhalten entsprechende Capabilities.

---

## Kein gefälschtes Success Signal

Ein kompromittierter oder fehlerhafter normaler Prozess darf nicht durch ein einfaches:

```text
boot successful
```

den Candidate committen können.

Health Confirmation muss aus autorisierten und validierten Systemzuständen abgeleitet werden.

---

## Architecture Introspection

Der aktuelle Boot Health State soll introspektierbar sein.

Beispiel:

```text
Generation:
    42

Boot Attempt:
    1

Current Milestone:
    CriticalServicesReady

Health:
    Pending
```

Nach abgeschlossenem Boot:

```text
Health:
    Confirmed

Generation:
    KnownGood
```

---

## Audit

Mindestens folgende Ereignisse sollen strukturiert protokollierbar sein:

```text
boot attempt started
milestone reached
milestone failed
milestone timeout
health confirmed
candidate committed
candidate rejected
```

---

## Determinismus

Bei identischem beobachteten Systemzustand und identischer Health Policy muss die Entscheidung:

```text
Healthy / Failed
```

deterministisch sein.

Adaptive Systeme dürfen Performance- oder Diagnoseparameter optimieren, aber nicht die erforderlichen Health-Kriterien probabilistisch ersetzen.

---

## KI-Unabhängigkeit

KI darf optional:

- Health Logs analysieren,
- wahrscheinliche Fehlerursachen bestimmen,
- Recoverymaßnahmen vorschlagen.

KI darf nicht:

- Required Milestones überspringen,
- `HealthConfirmed` ohne deterministischen Nachweis setzen,
- einen fehlgeschlagenen Candidate zu Known-Good erklären.

---

## Normative Anforderungen

1. NovaOS MUSS Boot Health über explizite Milestones modellieren.
2. Kernel Entry DARF NICHT allein als erfolgreicher Candidate-Boot gelten.
3. Ein Candidate DARF erst nach `HealthConfirmed` zu Known-Good werden.
4. Erforderliche Milestones MÜSSEN durch die aktive Boot Health Policy definiert werden.
5. Required und Optional Milestones MÜSSEN unterscheidbar sein.
6. Health Policies MÜSSEN unterschiedliche Systemprofile unterstützen können.
7. Jeder Candidate-Bootversuch MUSS eindeutig identifizierbar sein.
8. Milestone-Nachweise DÜRFEN NICHT zwischen unterschiedlichen Bootversuchen wiederverwendet werden.
9. Erforderliche Milestones DÜRFEN definierte Deadlines besitzen.
10. Ein Timeout eines Required Milestones MUSS als nicht erfolgreicher Health State behandelt werden.
11. Ein unbekannter Required Health State DARF NICHT zu einem Commit führen.
12. Boot Health DARF NICHT ausschließlich über eine verstrichene Zeitspanne definiert werden.
13. Kritische Milestones MÜSSEN crash-konsistent nachvollziehbar gemacht werden können.
14. Persistente Milestones SOLLEN grob genug sein, um unnötige Storage Writes zu vermeiden.
15. Boot Health MUSS mit A/B-Boot und Rollback integriert sein.
16. Boot Health MUSS von Verified Boot getrennt bleiben.
17. Ein Candidate MUSS sowohl erforderliche Trust- als auch Health-Bedingungen erfüllen.
18. Recovery MUSS den zuletzt erreichten beziehungsweise fehlgeschlagenen Milestone verwenden können.
19. Self-Healing DARF Health-Milestones zur Fehlerklassifikation verwenden.
20. Health-Milestones MÜSSEN mehrere abhängige Systemzustände aggregieren können.
21. Degraded States MÜSSEN von Blocking Failures unterscheidbar sein.
22. Die Autorität zum Bestätigen kritischer Health States MUSS capabilitybasiert geschützt sein.
23. Normale Anwendungen DÜRFEN eine Generation NICHT direkt zu Known-Good erklären.
24. Health Confirmation MUSS aus autorisierten Systemzuständen abgeleitet werden.
25. Milestone-Übergänge und Health Confirmation MÜSSEN auditierbar sein.
26. Der aktuelle Boot Health State MUSS introspektierbar sein.
27. Die Health-Entscheidung MUSS bei identischem Zustand und identischer Policy deterministisch sein.
28. Boot Health MUSS vollständig ohne KI funktionieren.
29. KI DARF Required Milestones NICHT umgehen oder ersetzen.
30. Die vorherige Known-Good-Generation MUSS bis zum erfolgreichen Candidate-Commit als Rollbackziel erhalten bleiben.

---

## Konsequenzen

### Positive Konsequenzen

- Candidate-Systeme werden erst nach tatsächlicher Betriebsfähigkeit committed.
- Kernelstart und vollständiger Booterfolg werden sauber getrennt.
- Rollbackentscheidungen erhalten eine präzisere Grundlage.
- Bootfehler können nach ihrer Startphase klassifiziert werden.
- Recovery und Self-Healing erhalten strukturierte Health-Informationen.
- Desktop-, Server-, Embedded- und Recovery-Systeme können unterschiedliche Health Policies verwenden.
- fehlerhafte Updates können die vorherige Known-Good-Generation nicht allein durch einen erfolgreichen Kernelstart verdrängen.

### Negative Konsequenzen

- Kernel und kritische Systemdienste müssen Health-Zustände zuverlässig melden.
- der Boot-Success-Punkt muss pro Systemprofil eindeutig definiert werden.
- persistente Milestone-Verwaltung erzeugt zusätzlichen Boot State.
- fehlerhafte Health Contracts könnten einen eigentlich funktionierenden Candidate unnötig zurückrollen.

---

## Verworfene Alternativen

### Commit direkt nach Kernel Entry

Verworfen.

Der Kernel kann erfolgreich starten, obwohl das restliche System nicht betriebsfähig ist.

### Commit nach festem Zeitintervall

Verworfen.

Zeitablauf allein beweist keinen gesunden Systemzustand.

### Jeder Dienst darf Boot Success melden

Verworfen.

Damit wäre der sicherheitskritische Commit-State nicht ausreichend geschützt.

### Nur ein globaler `BootSuccessful`-Boolean

Als internes Gesamtmodell verworfen.

Er enthält keine Information darüber, welche Bootphase erreicht oder wo ein Fehler aufgetreten ist.

### Alle Dienste müssen für Commit erfolgreich sein

Verworfen.

Nicht kritische oder optional degradierbare Komponenten dürfen einen ansonsten funktionsfähigen Boot nicht grundsätzlich verhindern.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-BOOT-0005_Vertrauenswürdige_Bootkette`
- `ADR-BOOT-0008_Recovery_als_First_Class_Bootmodus`
- `ADR-BOOT-0009_A_B_Boot_und_Boot_Rollback`
- `ADR-ARCH-0003_Deklaratives_Systemmodell_als_Architekturgrundlage`
- `ADR-ARCH-0004_Transaktionale_Systemoperationen`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-ARCH-0020_KI_nicht_als_Korrektheitsvoraussetzung_des_Kernels`

---

## Zugehörige NPSPECs

- `NPSPEC-BOOT-HEALTH-0001`
- `NPSPEC-BOOT-HEALTH-MILESTONE-0001`
- `NPSPEC-BOOT-HEALTH-POLICY-0001`
- `NPSPEC-BOOT-HEALTH-PROVIDER-0001`
- `NPSPEC-BOOT-HEALTH-AGGREGATION-0001`
- `NPSPEC-BOOT-BOOT-SUCCESS-0001`
- `NPSPEC-BOOT-ATTEMPT-0001`
- `NPSPEC-BOOT-KNOWNGOOD-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-HEALTH-TEST-0001`

---

## Verifikation

Die Umsetzung muss mindestens prüfen:

- Candidate erreicht alle Required Milestones,
- Candidate erreicht nur `KernelEntered`,
- Fehler während Kernelinitialisierung,
- System Root nicht verfügbar,
- kritischer Dienst startet nicht,
- optionaler Dienst startet nicht,
- degradierter aber zulässiger Systemzustand,
- Required Milestone Timeout,
- unbekannter Health State,
- erfolgreicher `HealthConfirmed`-Übergang,
- Commit zu Known-Good,
- Absturz unmittelbar vor Health Confirmation,
- Neustart nach unbestätigtem Candidate,
- Rollback auf vorherige Known-Good-Generation,
- unterschiedliche Health Policies für verschiedene Systemprofile,
- manipuliertes beziehungsweise nicht autorisiertes Health-Signal,
- Recovery anhand des zuletzt erreichten Milestones.

---

## Ergebnis

NovaOS bewertet einen erfolgreichen Boot nicht anhand eines einzelnen technischen Ereignisses, sondern anhand definierter semantischer Health Milestones.

Das Modell lautet:

```text
Candidate
    ↓
BootStarted
    ↓
KernelEntered
    ↓
KernelInitialized
    ↓
SystemRootReady
    ↓
CriticalServicesReady
    ↓
Operational
    ↓
HealthConfirmed
    ↓
Commit
    ↓
KnownGood
```

Damit bleibt die vorherige Known-Good-Generation erhalten, bis die neue Generation ihre tatsächliche Betriebsfähigkeit nachgewiesen hat.

Die zentrale Architekturregel lautet:

```text
Loading is not booting.

Booting is not health.

Health must be confirmed
before commit.
```