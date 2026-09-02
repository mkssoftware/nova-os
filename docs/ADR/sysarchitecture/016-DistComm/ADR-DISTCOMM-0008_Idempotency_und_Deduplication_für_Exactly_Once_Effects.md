# ADR-DISTCOMM-0008 – Idempotency und Deduplication für Exactly-Once Effects

## Status

Angenommen

## Kategorie

Distributed Communication / Reliability / Idempotency / Deduplication / Nova.Call

## Kontext

Verteilte Kommunikation kann nicht zuverlässig garantieren, dass ein Request genau einmal übertragen und genau einmal verarbeitet wird.

Netzwerkfehler können beispielsweise folgenden Zustand erzeugen:

```text
Caller
  ↓
Request
  ↓
Remote Execution
  ↓
Response lost
  ↓
Caller uncertain
```

Der Caller weiß in diesem Fall nicht, ob die Operation ausgeführt wurde.

Ein Retry kann notwendig sein, könnte aber dieselbe Operation erneut ausführen.

Bei Operationen wie:

- Zahlungen,
- Storage-Änderungen,
- Ressourcenallokationen,
- Capability-Delegationen,
- Systemtransaktionen

können doppelte Effekte nicht akzeptabel sein.

NovaOS benötigt deshalb eine Architektur, die nicht versucht, eine unrealistische universelle Exactly-Once-Delivery-Garantie vorzutäuschen, sondern **Exactly-Once Effects** durch explizite Idempotency- und Deduplication-Mechanismen ermöglicht.

---

## Entscheidung

NovaOS unterscheidet ausdrücklich zwischen:

```text
Exactly-Once Delivery
```

und:

```text
Exactly-Once Effect
```

Eine allgemeine Exactly-Once-Delivery-Garantie über verteilte Systemgrenzen wird nicht vorausgesetzt.

Stattdessen unterstützt `Nova.Call` für geeignete Operationen:

- stabile Operation Identities,
- Idempotency,
- Deduplication,
- gespeicherte beziehungsweise rekonstruierbare Operation Outcomes.

```text
Caller
  ↓
Operation ID
  ↓
Nova.Call
  ↓
Deduplication
  ↓
Operation
  ↓
Effect
```

Wird dieselbe logische Operation erneut übertragen, darf sie nicht automatisch einen zweiten semantischen Effekt erzeugen.

---

## Grundprinzip

```text
Messages may repeat.
Effects must not.
```

und:

```text
Exactly-once effect
is a semantic contract,
not a transport illusion.
```

---

## Operation Identity

Eine deduplizierbare Operation benötigt eine stabile logische Operation Identity.

```text
Operation
 ├── Operation ID
 ├── Caller Context
 ├── Target
 └── Contract
```

Retries derselben logischen Operation verwenden dieselbe Operation Identity.

Eine neue fachliche Operation erhält eine neue Identity.

Die konkrete Struktur und Erzeugung von Operation IDs wird in NPSPECs definiert.

---

## Idempotency

Eine Operation ist idempotent, wenn ihre wiederholte Ausführung denselben relevanten semantischen Effekt besitzt wie ihre einmalige Ausführung.

```text
Apply(X)
Apply(X)
Apply(X)

        ↓

Same Semantic Effect
```

NovaOS muss zwischen:

- intrinsisch idempotenten Operationen,
- durch Idempotency Keys geschützten Operationen,
- nicht idempotenten Operationen

unterscheiden können.

Eine Operation darf nicht ohne entsprechenden Contract als idempotent angenommen werden.

---

## Deduplication

Bei deduplizierbaren Calls prüft der Empfänger die Operation Identity.

```text
Request
   ↓
Operation ID
   ↓
Seen before?
 ├── No  → Execute
 └── Yes → Do not repeat effect
```

Für bereits abgeschlossene Operationen kann das gespeicherte beziehungsweise rekonstruierbare Ergebnis zurückgegeben werden.

Die konkrete Deduplication-Storage- und Lifetime-Strategie wird in NPSPECs definiert.

---

## Exactly-Once Effect

Exactly-Once Effect bedeutet:

> Für eine eindeutig identifizierte logische Operation wird der relevante semantische Effekt höchstens einmal erzeugt und ein erfolgreicher Abschluss kann als solcher erkannt werden.

Dies bedeutet nicht, dass:

- nur ein Netzwerkpaket übertragen wurde,
- nur ein Request gesendet wurde,
- keine Retries stattgefunden haben,
- nur eine interne Verarbeitung versucht wurde.

```text
Multiple Delivery Attempts
          ↓
Single Logical Operation
          ↓
Single Committed Effect
```

---

## Integration mit transaktionalen Operationen

Für zustandsverändernde Operationen soll Deduplication mit dem eigentlichen Commit gekoppelt werden können.

```text
Operation ID
     ↓
Begin Transaction
     ↓
Check Deduplication State
     ↓
Apply Effect
     ↓
Record Outcome
     ↓
Commit
```

Dadurch wird verhindert, dass Effect und Deduplication Record unabhängig voneinander inkonsistent werden.

Die konkrete atomare Umsetzung ist abhängig vom verwendeten Storage- oder Service-Modell.

---

## Retry-Semantik

`Nova.Call` darf Retries nur entsprechend der Semantik des Call Contracts durchführen.

```text
Call Failure
    ↓
Retry Safe?
 ├── Yes → Retry
 └── No  → Return uncertain outcome
```

Automatische Retries sind insbesondere zulässig, wenn:

- die Operation intrinsisch idempotent ist,
- eine geeignete Deduplication-Garantie besteht,
- eindeutig feststeht, dass die Operation noch nicht ausgeführt wurde.

Bei nicht idempotenten Operationen ohne Deduplication darf NovaOS keinen möglicherweise doppelten Effekt durch einen transparenten Retry erzeugen.

---

## Unknown Outcome

Trotz Idempotency und Deduplication können Situationen entstehen, in denen der lokale Teilnehmer den endgültigen Zustand nicht bestimmen kann.

```text
Request
   ↓
Remote Node
   ↓
Partial Failure
   ↓
Outcome Unknown
```

In solchen Fällen bleibt `UnknownOutcome` eine gültige explizite Distributed-Call-Semantik.

NovaOS darf einen unbekannten Zustand nicht als sicheren Fehlschlag behandeln und dadurch unkontrolliert eine neue fachliche Operation erzeugen.

---

## Result Replay

Für bereits deduplizierte Operationen kann der Empfänger das ursprüngliche Ergebnis erneut bereitstellen.

```text
Operation ID already completed
          ↓
Stored Outcome
          ↓
Result Replay
```

Dabei muss zwischen dem semantischen Operation Outcome und temporären Transportmetadaten unterschieden werden.

Nicht jedes Ergebnis muss unbegrenzt gespeichert werden.

Retention und Rekonstruktion werden durch Contract und Resource Policy bestimmt.

---

## Deduplication Scope

Operation Identities müssen innerhalb eines eindeutig definierten Scopes interpretiert werden.

Ein Scope kann beispielsweise an folgende Eigenschaften gebunden sein:

- Service Identity,
- Capability Target,
- Caller Identity,
- Session,
- Transaction Domain.

Dadurch werden zufällige oder absichtliche Operation-ID-Kollisionen zwischen unabhängigen Kontexten verhindert.

---

## Deduplication Lifetime

Deduplication State kann nicht grundsätzlich unbegrenzt gespeichert werden.

Der Contract muss deshalb eine geeignete Gültigkeits- beziehungsweise Retention-Semantik ermöglichen.

```text
Operation ID
    ↓
Deduplication Window
    ↓
Expiration
```

Nach Ablauf eines Deduplication Windows darf ein alter Request nicht automatisch als sicher deduplizierbar gelten.

Operationen, die langfristige Exactly-Once-Effects benötigen, benötigen entsprechend dauerhaftere Identitäts- oder Transaktionsmechanismen.

---

## Security

Operation IDs sind keine Capabilities.

```text
Operation ID
    ≠
Authority
```

Das Wissen um eine gültige Operation Identity darf keine Berechtigung zur:

- Ausführung,
- Wiederholung,
- Abfrage des Ergebnisses

erzeugen.

Capability-, Identity-, Trust- und Security-Prüfungen bleiben unabhängig von Deduplication bestehen.

---

## Replay Protection

Deduplication kann Bestandteil des Replay-Schutzes sein, ersetzt jedoch nicht automatisch kryptografische Replay Protection.

Ein Angreifer darf einen aufgezeichneten Request nicht allein deshalb erneut autorisiert ausführen können, weil eine Operation ID vorhanden ist.

Security Context, Capability Lifetime und Authentizität müssen weiterhin geprüft werden.

---

## Capability Delegation

Operationen zur Capability Delegation können Exactly-Once-Effect-Semantik benötigen.

Eine wiederholte Übertragung derselben Delegationsoperation darf beispielsweise nicht unbeabsichtigt mehrere unabhängige Delegationen erzeugen, wenn der Contract nur eine logische Delegation beschreibt.

Deduplication ersetzt dabei nicht die Validierung der Delegationsauthority.

---

## Bounded State

Deduplication State ist eine Systemressource und muss begrenzt werden.

```text
Deduplication Records
        ↓
Resource Budget
        ↓
Retention Policy
```

Ein Remote Peer darf nicht unbegrenzt neue Operation IDs erzeugen und dadurch unbeschränkt lokalen Speicher belegen.

Deduplication muss deshalb mit:

- Resource Budgets,
- Quotas,
- Expiration,
- Garbage Collection

integrierbar sein.

---

## Distributed Deduplication

Ist ein logischer Service über mehrere Instanzen verteilt, muss der Deduplication Scope mit der tatsächlichen Servicearchitektur übereinstimmen.

```text
             Service Identity
              /     |     \
             ↓      ↓      ↓
         Instance A B      C
              \     |     /
               ↓    ↓    ↓
            Deduplication
```

Eine erneute Anfrage darf nicht allein deshalb einen zweiten Effekt erzeugen, weil sie durch Service Discovery oder Failover eine andere Instanz erreicht.

Welche Konsistenz dafür erforderlich ist, hängt vom jeweiligen Exactly-Once-Contract ab und wird in NPSPECs definiert.

---

## Connection Migration und Failover

Connection Migration, Multipath oder Providerwechsel dürfen die logische Operation Identity nicht verändern.

```text
Operation ID
     ↓
Path A fails
     ↓
Path B
     ↓
Same Operation ID
```

Dadurch bleibt Deduplication unabhängig vom konkreten Netzwerkpfad möglich.

---

## Introspection und Audit

NovaOS muss Exactly-Once-relevante Operationen kontrolliert nachvollziehbar machen können.

Beispielsweise:

```text
Operation ID
 ├── Target
 ├── Idempotency Contract
 ├── First Attempt
 ├── Retry Attempts
 ├── Deduplication Decision
 └── Final Known Outcome
```

Sensitive Daten und Capability-Material dürfen dabei nicht unnötig offengelegt werden.

---

## Normative Anforderungen

1. NovaOS DARF keine universelle Exactly-Once-Delivery-Garantie für Distributed Communication voraussetzen.
2. NovaOS MUSS Exactly-Once Effects für geeignete Operationen unterstützen können.
3. Deduplication MUSS auf einer stabilen logischen Operation Identity basieren können.
4. Retries derselben logischen Operation MÜSSEN dieselbe Operation Identity verwenden können.
5. Neue fachliche Operationen MÜSSEN von Retries bestehender Operationen unterscheidbar sein.
6. Idempotency MUSS explizit Bestandteil des Operationscontracts sein können.
7. Nicht idempotente Operationen DÜRFEN NICHT ohne geeignete Absicherung automatisch als retry-safe behandelt werden.
8. Deduplication MUSS wiederholte semantische Effekte verhindern können.
9. Zustandsverändernde Exactly-Once-Operationen SOLLEN Deduplication State und Effect atomar beziehungsweise transaktional koppeln können.
10. Bereits abgeschlossene Operation Outcomes SOLLEN für Retries wiederverwendbar oder rekonstruierbar sein.
11. `UnknownOutcome` MUSS auch bei Distributed Operations explizit darstellbar bleiben.
12. Operation IDs MÜSSEN innerhalb eines definierten Scopes interpretiert werden.
13. Deduplication State MUSS eine definierte Lifetime beziehungsweise Retention Policy besitzen.
14. Operation IDs DÜRFEN NICHT als Authority behandelt werden.
15. Capability-, Identity-, Trust- und Security-Prüfungen MÜSSEN unabhängig von Deduplication erhalten bleiben.
16. Deduplication State MUSS in die systemweite Resource Economy integrierbar sein.
17. Verteilte Serviceinstanzen MÜSSEN Exactly-Once-Contracts über Instanzwechsel hinweg erfüllen können, sofern dies vom Contract verlangt wird.
18. Connection Migration, Multipath und Providerwechsel DÜRFEN die logische Operation Identity NICHT verändern.
19. Retry-, Deduplication- und Outcome-Entscheidungen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- sichere Retries für geeignete zustandsverändernde Operationen,
- keine falsche universelle Exactly-Once-Delivery-Annahme,
- robuste Behandlung verlorener Antworten und Connection Failures,
- Exactly-Once Effects können mit transaktionalen Systemoperationen kombiniert werden,
- Failover und Service-Instance-Wechsel bleiben möglich,
- Operation Identity schafft eine gemeinsame Grundlage für Deduplication, Audit und Causation.

### Negative Konsequenzen

- Deduplication benötigt zusätzlichen persistenten oder temporären Zustand,
- verteilte Deduplication kann Konsistenzkosten verursachen,
- Retention und Garbage Collection müssen sorgfältig definiert werden,
- Exactly-Once Effects sind teurer als einfache At-Most-Once- oder At-Least-Once-Semantik.

---

## Verworfene Alternativen

### Universelles Exactly-Once Delivery

Verworfen.

Netzwerkfehler und partielle Ausfälle verhindern eine allgemeine Garantie, dass eine Nachricht physisch exakt einmal übertragen und verarbeitet wird.

### Automatische Retries aller fehlgeschlagenen Calls

Verworfen.

Bei nicht idempotenten Operationen können dadurch doppelte semantische Effekte entstehen.

### Deduplication ausschließlich anhand des Payloads

Verworfen.

Zwei fachlich unabhängige Operationen können identische Daten besitzen und müssen dennoch getrennt ausgeführt werden.

### Unbegrenzte Speicherung aller Operation IDs

Verworfen.

Dies würde unbegrenzten Ressourcenverbrauch erzeugen und widerspricht der systemweiten Resource Economy.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0004_Transactional_System_Operations`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0002_Location_Transparency_ohne_Kosten_und_Fehler_zu_verbergen`
- `ADR-DISTCOMM-0003_Remote_Capabilities`
- `ADR-DISTCOMM-0004_Capability_Delegation_über_Netzwerkgrenzen`
- `ADR-DISTCOMM-0005_Typed_Schemas_und_versionierte_Serialisierung`
- `ADR-DISTCOMM-0007_Bounded_Queues_und_Backpressure`
- `ADR-NETWORK-0012_Service_Discovery`
- `ADR-NETWORK-0019_Connection_Migration`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0013_Transaktionale_Dateioperationen`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-IDEMPOTENCY-0001`
- `NPSPEC-DISTCOMM-OPERATION-IDENTITY-0001`
- `NPSPEC-DISTCOMM-DEDUPLICATION-0001`
- `NPSPEC-DISTCOMM-DEDUPLICATION-STORE-0001`
- `NPSPEC-DISTCOMM-RETRY-SEMANTICS-0001`
- `NPSPEC-DISTCOMM-EXACTLY-ONCE-EFFECT-0001`
- `NPSPEC-DISTCOMM-RESULT-REPLAY-0001`
- `NPSPEC-DISTCOMM-DEDUPLICATION-TEST-0001`

---

## Ergebnis

NovaOS unterscheidet bewusst zwischen physischer Nachrichtenzustellung und semantischen Effekten:

```text
                Logical Operation
                       ↓
                  Operation ID
                       ↓
                    Nova.Call
                       ↓
              Delivery / Retry
                       ↓
                 Deduplication
                       ↓
              Transaction / Effect
                       ↓
                 Stored Outcome
                       ↓
                     Result
```

Damit können Requests bei Netzwerkfehlern kontrolliert wiederholt werden, ohne bei geeigneten Contracts denselben fachlichen Effekt mehrfach zu erzeugen.

NovaOS verspricht daher keine unrealistische universelle Exactly-Once Delivery, sondern stellt die Mechanismen für explizit definierte und überprüfbare **Exactly-Once Effects** bereit.