# ADR-DISTCOMM-0009 – Operation-Aware Retry und Circuit Breaking

## Status

Angenommen

## Kategorie

Distributed Communication / Reliability / Retry / Circuit Breaking / Nova.Call

## Kontext

Verteilte Kommunikation ist grundsätzlich mit temporären Fehlern konfrontiert.

Dazu gehören beispielsweise:

- Verbindungsabbrüche,
- Timeouts,
- überlastete Services,
- temporär nicht verfügbare Service Instances,
- fehlerhafte Netzwerkpfade,
- Provider-Ausfälle.

Ein automatischer Retry kann solche Fehler überbrücken.

Nicht jede Operation darf jedoch gefahrlos wiederholt werden. Insbesondere zustandsverändernde oder nicht idempotente Operationen können bei einem Retry doppelte Effekte erzeugen.

Gleichzeitig können wiederholte Calls an einen bereits erkennbar ausgefallenen oder überlasteten Service zusätzliche Last erzeugen und einen Fehlerzustand verstärken.

NovaOS benötigt deshalb eine Retry- und Circuit-Breaking-Architektur, die die **Semantik der jeweiligen Operation** berücksichtigt.

---

## Entscheidung

NovaOS führt **Operation-Aware Retry und Circuit Breaking als native Eigenschaften von `Nova.Call`** ein.

```text
Nova.Call
    ↓
Operation Contract
    ↓
Execution Attempt
    ↓
Result
 ├── Success
 └── Failure
       ↓
Retry permitted?
 ├── Yes → Retry Policy
 └── No  → Return Failure / UnknownOutcome
```

Retry-Entscheidungen werden nicht allein anhand technischer Fehlercodes getroffen, sondern anhand von:

- Operation Semantics,
- Idempotency,
- Deduplication,
- Deadline,
- Failure Type,
- Resource Budget,
- Circuit State.

Circuit Breaker verhindern wiederholte Calls an Ziele, deren aktueller Zustand weitere Versuche voraussichtlich nicht sinnvoll zulässt.

---

## Grundprinzip

```text
Retry only when semantically safe.
```

und:

```text
Do not amplify failure.
```

---

## Operation-Aware Retry

Der Operationscontract beschreibt, ob und unter welchen Bedingungen eine Operation wiederholt werden darf.

Konzeptionell können Operationen beispielsweise klassifiziert werden als:

```text
RetrySafe
ConditionallyRetryable
Deduplicated
NonRetryable
```

Die konkrete Klassifikation wird in NPSPECs definiert.

Die Retry Policy darf niemals stärkere Retry-Garantien annehmen, als der Operationscontract erlaubt.

---

## Idempotency und Deduplication

Operation-Aware Retry integriert sich direkt mit Idempotency und Deduplication.

```text
Operation
   ↓
Idempotent?
 ├── Yes → Retry may be safe
 └── No
      ↓
Deduplicated?
 ├── Yes → Retry may be safe
 └── No  → No automatic retry after uncertain execution
```

Ein Retry derselben logischen Operation verwendet dieselbe Operation Identity.

Ein Retry darf nicht als neue fachliche Operation erzeugt werden.

---

## Failure Classification

Nicht jeder Fehler rechtfertigt einen Retry.

Fehler müssen semantisch klassifizierbar sein.

Beispielsweise:

```text
Transient
Overloaded
Unavailable
Permanent
Unauthorized
InvalidRequest
DeadlineExceeded
UnknownOutcome
```

Ein `Unauthorized`- oder `InvalidRequest`-Fehler wird durch unveränderte Wiederholung normalerweise nicht behoben.

Temporäre Netzwerk- oder Providerfehler können dagegen retry-fähig sein.

Die konkrete Fehlerklassifikation wird in NPSPECs definiert.

---

## Unknown Outcome

Besondere Vorsicht gilt bei:

```text
UnknownOutcome
```

Wenn nicht bekannt ist, ob eine Remote Operation bereits einen Effekt erzeugt hat, darf NovaOS keinen automatischen Retry durchführen, sofern nicht:

- die Operation idempotent ist,
- Deduplication vorhanden ist,
- oder der Contract eine andere sichere Retry-Semantik definiert.

```text
UnknownOutcome
      ↓
Safe retry semantics?
 ├── Yes → Retry
 └── No  → Propagate UnknownOutcome
```

---

## Retry Budget

Retries sind keine kostenlosen Operationen.

Jeder Retry verbraucht zusätzliche:

- Zeit,
- CPU,
- Netzwerkbandbreite,
- Queue Capacity,
- Remote Compute,
- Energie.

NovaOS verwendet deshalb begrenzte Retry Budgets.

```text
Original Call
    ↓
Retry Budget
 ├── Attempt 1
 ├── Attempt 2
 └── Attempt N
```

Unbegrenzte automatische Retries sind nicht zulässig.

---

## Deadlines

Retries müssen innerhalb der ursprünglichen Deadline beziehungsweise des Call Contracts bleiben.

```text
Call Deadline
     ↓
Attempt
     ↓
Remaining Time
     ↓
Retry useful?
```

Ein Retry darf nicht gestartet werden, wenn die verbleibende Zeit offensichtlich nicht mehr ausreicht, sofern der Contract keine andere Semantik definiert.

Retries verlängern eine End-to-End-Deadline nicht automatisch.

---

## Backoff

Retry Policies sollen eine kontrollierte Verzögerung zwischen Wiederholungsversuchen unterstützen.

Dadurch wird verhindert, dass ein temporär ausgefallener Service sofort mit weiteren Calls überlastet wird.

```text
Failure
  ↓
Backoff
  ↓
Retry
```

Die konkrete Backoff-Strategie wird in NPSPECs definiert.

Sie kann unter anderem von:

- Failure Type,
- Retry Count,
- Service Health,
- Deadline,
- Load

abhängen.

---

## Retry Storm Prevention

NovaOS muss verhindern, dass viele Caller gleichzeitig einen ausgefallenen Service durch synchronisierte Retries zusätzlich belasten.

```text
Service Failure
      ↓
1000 Callers
      ↓
Immediate Retry
      ↓
Retry Storm
```

Retry Policies müssen deshalb Mechanismen zur Lastverteilung und Begrenzung unterstützen.

Die konkrete Umsetzung gehört in die NPSPECs.

---

## Circuit Breaker

Ein Circuit Breaker verhindert wiederholte Calls an ein Ziel, dessen aktueller Zustand weitere Versuche voraussichtlich nicht sinnvoll zulässt.

Konzeptionell:

```text
Closed
  ↓
Failures
  ↓
Open
  ↓
Recovery Probe
  ↓
Half-Open
  ↓
Success / Failure
```

Die konkrete State Machine, Schwellenwerte und Zeitsteuerung werden in NPSPECs definiert.

---

## Circuit Scope

Circuit Breaker dürfen nicht ausschließlich global pro Host betrachtet werden.

Ein Circuit kann abhängig vom Contract beispielsweise gelten für:

- Service Identity,
- Service Instance,
- Operation,
- Provider,
- Route,
- Remote Dependency.

Damit kann NovaOS zwischen einem vollständig ausgefallenen Service und einer einzelnen fehlerhaften Operation unterscheiden.

---

## Service Discovery

Circuit Breaking wird mit Service Discovery integriert.

Ist eine Service Instance als problematisch klassifiziert, kann NovaOS eine andere zulässige Instance auswählen.

```text
Service
 ├── Instance A → Circuit Open
 ├── Instance B → Healthy
 └── Instance C → Healthy
```

Der Wechsel der Instance muss weiterhin:

- Authority,
- Trust,
- Data Sovereignty,
- Operation Identity,
- Deduplication

respektieren.

---

## Provider Health

Circuit Breaking und Provider Health sind verwandte, aber getrennte Konzepte.

```text
Provider Health
      ↓
Observed condition

Circuit Breaker
      ↓
Call admission decision
```

Health liefert Informationen über den Zustand eines Providers oder Services.

Der Circuit Breaker entscheidet, ob neue Calls beziehungsweise Versuche aktuell zugelassen werden.

---

## Backpressure

Backpressure darf nicht automatisch durch aggressive Retries umgangen werden.

```text
Backpressure
     ↓
Retry immediately
     ↓
More overload
```

Wird ein Call aufgrund von Überlastung zurückgewiesen oder verzögert, muss die Retry Policy diese Information berücksichtigen.

Circuit Breaking kann bei anhaltender Überlastung zusätzliche Calls temporär unterbinden.

---

## Multipath und Connection Migration

Ein Fehler eines Netzwerkpfades muss nicht automatisch bedeuten, dass die logische Operation erneut ausgeführt werden muss.

NovaOS soll zunächst unterscheiden zwischen:

```text
Path Failure
Transport Failure
Service Failure
Operation Failure
```

Bei geeigneten Transporten können Multipath oder Connection Migration die bestehende Kommunikation fortsetzen.

Ein neuer logischer Retry soll erst erfolgen, wenn dies tatsächlich erforderlich und semantisch zulässig ist.

---

## Security

Retry und Circuit Breaking dürfen Security-Fehler nicht durch alternative Pfade oder schwächere Policies umgehen.

Beispielsweise darf:

```text
Unauthorized
```

nicht zu:

```text
Retry without required capability
```

führen.

Ebenso darf ein Trust- oder Sovereignty-Fehler nicht durch Auswahl eines nicht zulässigen alternativen Services umgangen werden.

---

## Graceful Degradation

Wenn ein Service nicht verfügbar ist und der Contract eine alternative Funktion erlaubt, kann Circuit Breaking mit Graceful Degradation kombiniert werden.

```text
Primary Service
      ↓
Circuit Open
      ↓
Permitted Alternative?
 ├── Yes → Degraded Service
 └── No  → Explicit Failure
```

Die Alternative muss weiterhin alle Hard Constraints erfüllen.

---

## Adaptive Retry Policy

NovaOS darf Retry- und Circuit-Breaker-Policies innerhalb zulässiger Grenzen adaptiv optimieren.

Beobachtungen können beispielsweise umfassen:

- Erfolgswahrscheinlichkeit,
- Latenz,
- Service Health,
- Failure Rate,
- Recovery Time,
- Prediction Error.

Adaptive Verfahren dürfen jedoch:

- keine nicht retry-fähige Operation retry-fähig machen,
- keine Hard Constraints verändern,
- keine Retry Budgets unbegrenzt erweitern.

---

## Deterministic Mode

Im Deterministic Mode müssen Retry- und Circuit-Breaker-Entscheidungen kontrollierbar und reproduzierbar sein.

Dynamische beziehungsweise adaptive Entscheidungen können durch festgelegte:

- Retry Counts,
- Backoff-Regeln,
- Circuit Thresholds,
- Failover-Reihenfolgen

ersetzt werden.

---

## Introspection und Causation

NovaOS muss Retry- und Circuit-Breaker-Entscheidungen kontrolliert nachvollziehbar machen.

Beispielsweise:

```text
Operation ID
 ├── Attempt 1
 │    └── Transport Failure
 ├── Attempt 2
 │    └── Timeout
 ├── Circuit State
 └── Final Outcome
```

Causation Tracking muss unterscheiden können zwischen ursprünglichem Call und daraus entstandenen Retry Attempts.

---

## Normative Anforderungen

1. NovaOS MUSS Operation-Aware Retry für Distributed Calls unterstützen.
2. Retry-Entscheidungen MÜSSEN die Semantik der jeweiligen Operation berücksichtigen.
3. Nicht retry-fähige Operationen DÜRFEN NICHT automatisch wiederholt werden.
4. Retries derselben logischen Operation MÜSSEN dieselbe Operation Identity verwenden.
5. Idempotency und Deduplication MÜSSEN in Retry-Entscheidungen integrierbar sein.
6. Fehler MÜSSEN hinsichtlich ihrer Retry-Relevanz klassifizierbar sein.
7. `UnknownOutcome` DARF NICHT ohne sichere Retry-Semantik automatisch wiederholt werden.
8. Automatische Retries MÜSSEN durch ein Retry Budget begrenzt sein.
9. Retries DÜRFEN eine End-to-End-Deadline NICHT implizit verlängern.
10. Retry Policies MÜSSEN kontrolliertes Backoff unterstützen können.
11. NovaOS MUSS Mechanismen gegen Retry Storms unterstützen.
12. Distributed Communication MUSS Circuit Breaking unterstützen können.
13. Circuit Breaker MÜSSEN auf geeignete Service-, Instance-, Operation- oder Provider-Scopes anwendbar sein.
14. Circuit Breaking MUSS mit Service Discovery und Provider Health integrierbar sein.
15. Backpressure DARF NICHT durch aggressive automatische Retries umgangen werden.
16. Path- oder Transportfehler SOLLEN von tatsächlichen Operation Failures unterschieden werden.
17. Multipath und Connection Migration SOLLEN vor unnötigen neuen Operation Attempts berücksichtigt werden.
18. Retry und Failover DÜRFEN Security-, Capability-, Trust- oder Data-Sovereignty-Constraints NICHT umgehen.
19. Adaptive Retry Policies DÜRFEN die Retry-Semantik des Operationscontracts NICHT erweitern.
20. Deterministic Mode MUSS Retry- und Circuit-Breaker-Entscheidungen kontrollieren können.
21. Retry Attempts, Circuit States und finale Outcomes MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- temporäre Distributed Failures können automatisch abgefangen werden,
- nicht idempotente Operationen werden vor unsicheren Retries geschützt,
- Retry Storms und kaskadierende Überlastungen werden reduziert,
- ausgefallene Services werden nicht dauerhaft mit sinnlosen Calls belastet,
- Service Discovery, Provider Health und Connection Migration können gemeinsam reagieren,
- Retry-Verhalten bleibt mit Deadlines und Resource Economy vereinbar.

### Negative Konsequenzen

- Retry Policies benötigen semantische Informationen über Operationen,
- Circuit Breaker erzeugen zusätzlichen verteilten Zustand,
- falsch konfigurierte Thresholds können verfügbare Services vorübergehend ausschließen,
- verteilte Fehlerbehandlung wird komplexer als einfache automatische Wiederholung.

---

## Verworfene Alternativen

### Automatischer Retry jedes Fehlers

Verworfen.

Nicht idempotente Operationen könnten mehrfach ausgeführt werden und permanente Fehler würden unnötige Last erzeugen.

### Keine automatischen Retries

Verworfen.

Viele kurzzeitige Netzwerk- und Providerfehler können sicher und effizient automatisch behandelt werden.

### Circuit Breaker ausschließlich pro Host

Verworfen.

Ein Host kann mehrere unabhängige Services und Operationen bereitstellen, deren Gesundheitszustände unterschiedlich sein können.

### Unbegrenzte Retries bis zum Erfolg

Verworfen.

Dies verletzt Resource Budgets und Deadlines und kann Ausfälle durch Retry Storms verstärken.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0010_Structured_Concurrency_systemweit`
- `ADR-ARCH-0011_Deterministic_Mode`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0002_Location_Transparency_ohne_Kosten_und_Fehler_zu_verbergen`
- `ADR-DISTCOMM-0007_Bounded_Queues_und_Backpressure`
- `ADR-DISTCOMM-0008_Idempotency_und_Deduplication_für_Exactly_Once_Effects`
- `ADR-NETWORK-0007_Multipath_Networking`
- `ADR-NETWORK-0012_Service_Discovery`
- `ADR-NETWORK-0019_Connection_Migration`
- `ADR-NETWORK-0022_Health_Aware_Network_Provider_Selection`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-RETRY-0001`
- `NPSPEC-DISTCOMM-RETRY-POLICY-0001`
- `NPSPEC-DISTCOMM-RETRY-BUDGET-0001`
- `NPSPEC-DISTCOMM-RETRY-BACKOFF-0001`
- `NPSPEC-DISTCOMM-FAILURE-CLASSIFICATION-0001`
- `NPSPEC-DISTCOMM-CIRCUIT-BREAKER-0001`
- `NPSPEC-DISTCOMM-CIRCUIT-BREAKER-STATE-0001`
- `NPSPEC-DISTCOMM-RETRY-INTROSPECTION-0001`
- `NPSPEC-DISTCOMM-RETRY-TEST-0001`

---

## Ergebnis

NovaOS behandelt Retries nicht als einfache Transportwiederholung, sondern als semantische Entscheidung über eine logische Operation:

```text
                 Nova.Call
                     ↓
              Operation Contract
                     ↓
                Call Attempt
                     ↓
                  Failure
                     ↓
       ┌─────────────┴─────────────┐
       ↓                           ↓
 Circuit Evaluation          Retry Safety
       ↓                           ↓
 Open?                    Idempotent /
       ↓                   Deduplicated?
   Yes / No                       ↓
       └─────────────┬─────────────┘
                     ↓
             Retry Budget +
                 Deadline
                     ↓
            Retry / Failover /
             Explicit Failure
```

Damit kann NovaOS temporäre Fehler automatisch überbrücken, ohne nicht idempotente Operationen unsicher zu wiederholen oder bereits gestörte Dienste durch unbegrenzte Retry-Kaskaden zusätzlich zu belasten.