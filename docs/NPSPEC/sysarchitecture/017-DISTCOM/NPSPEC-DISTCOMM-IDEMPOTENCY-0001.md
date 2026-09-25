# NPSPEC-DISTCOMM-IDEMPOTENCY-0001 – Nova Distributed Idempotency

## Status

Angenommen

## Kategorie

Distributed Communication / Reliability / Idempotency

## Zweck

NovaOS definiert ein gemeinsames Idempotenz-Modell für verteilte Operationen, die aufgrund von Retries, Netzwerkfehlern oder unbekannten Ausführungszuständen mehrfach übertragen werden können.

```text
Logical Operation
      ↓
Attempt 1
Attempt 2
Attempt 3
      ↓
One Intended Effect
```

Idempotenz soll verhindern, dass wiederholte Übertragungen unbeabsichtigt mehrfach denselben fachlichen Effekt erzeugen.

## Grundprinzip

```text
Retry ≠ New Operation
Duplicate Request ≠ Duplicate Effect
Same Payload ≠ Same Operation
Idempotency ≠ Exactly-Once Delivery
Transport Deduplication ≠ Business Idempotency
```

Die Identität einer logischen Operation muss unabhängig von ihren einzelnen Übertragungsversuchen bleiben.

## Idempotenz-Modell

Eine idempotente Operation besitzt mindestens:

```text
IdempotencyContext
├── OperationID
├── IdempotencyKey
├── Operation Type
├── Scope
├── State
└── Lifetime
```

Optional:

```text
Result Reference
Security Context
ExecutionContract
Resource Budget
TransactionID
ProducerID
ServiceID
```

`OperationID` bezeichnet die logische Operation.

Ein einzelner Retry erhält keine neue logische Operationsidentität.

## Idempotency Key

Ein stabiler `IdempotencyKey` darf verwendet werden, um mehrere Versuche derselben Operation zu erkennen.

```text
Operation
   ↓
IdempotencyKey = X
   ├── Attempt 1
   ├── Attempt 2
   └── Attempt 3
```

Der Schlüssel muss innerhalb seines definierten Scopes eindeutig sein.

Mögliche Scopes:

```text
Service
Method
Client
Security Domain
Queue
Transaction
```

## Verarbeitung

Eine empfangende Seite kann eine Operation anhand ihres Idempotency Keys prüfen.

```text
Request
   ↓
Idempotency Lookup
   ├── Unknown   → Execute
   ├── Running   → Wait / Report
   ├── Completed → Return Stored Result
   └── Failed    → Apply Policy
```

Die Prüfung und Registrierung des Schlüssels muss so erfolgen, dass parallele Duplikate nicht unabhängig denselben Effekt auslösen.

## Zustandsmodell

Ein Idempotency Record kann mindestens besitzen:

```text
Unknown
   ↓
Executing
   ↓
Completed
```

Zusätzlich:

```text
Failed
Expired
Invalid
```

`Executing` ist notwendig, damit gleichzeitig eintreffende Duplikate erkannt werden können.

## Ergebniswiederverwendung

Bei bereits erfolgreich abgeschlossenen Operationen darf das gespeicherte Ergebnis erneut zurückgegeben werden.

```text
Duplicate Request
       ↓
Completed Record
       ↓
Previous Result
```

Dadurch muss die fachliche Operation nicht erneut ausgeführt werden.

Nicht jedes Ergebnis muss vollständig gespeichert werden; alternativ darf eine Referenz auf das Ergebnis verwendet werden.

## Semantische Idempotenz

Einige Operationen sind von Natur aus idempotent.

Beispiel:

```text
SetVolume(50)
SetVolume(50)
SetVolume(50)
```

Der resultierende Zustand bleibt gleich.

Andere Operationen sind nicht automatisch idempotent:

```text
IncrementCounter()
CreatePayment()
AppendRecord()
```

Diese benötigen zusätzliche Idempotenzmechanismen, wenn sichere Wiederholung erforderlich ist.

## Payload-Prüfung

Ein bereits verwendeter Idempotency Key darf nicht unbemerkt für eine semantisch andere Operation verwendet werden.

```text
Key X + Operation A
Key X + Operation B
        ↓
Conflict
```

NovaOS soll dafür Operationstyp, relevante Parameter oder einen stabilen Request-Fingerprint berücksichtigen können.

## Retry-Integration

Distributed Retry verwendet Idempotenz zur Entscheidung über sichere Wiederholung.

```text
Failure
   ↓
Idempotency State
   ↓
Retry Decision
```

Eine als idempotent markierte Operation darf dennoch weiteren Einschränkungen wie Deadline, Backpressure oder Retry Budget unterliegen.

## Messaging und Queues

Bei Message Bus, Pub/Sub und Queues dürfen `MessageID` oder separate Idempotency Keys zur Duplikaterkennung verwendet werden.

```text
Message
   ↓
Delivery 1
Delivery 2
   ↓
Idempotent Consumer
   ↓
One Intended Effect
```

At-Least-Once Delivery soll dadurch sicherer verarbeitet werden können.

## Transaktionen

Idempotenz soll mit NovaOS-Transaktionen kombinierbar sein.

```text
Begin
  ↓
Check Idempotency
  ↓
Execute Effect
  ↓
Store Result
  ↓
Commit
```

Idempotency Record und fachlicher Effekt sollen, wenn erforderlich, atomar gekoppelt werden können.

Dadurch wird vermieden:

```text
Effect committed
Idempotency record missing
```

## Verteilung

Idempotency Records dürfen verteilt, repliziert oder partitioniert gespeichert werden.

Dabei muss die zugesicherte Konsistenz explizit definiert sein.

NovaOS darf keine globale Exactly-Once-Semantik vortäuschen, wenn die zugrunde liegende Infrastruktur diese nicht garantiert.

## Ablaufzeit

Idempotency Records dürfen zeitlich begrenzt sein.

```text
Active
  ↓
Retention Period
  ↓
Expired
```

Nach Ablauf kann derselbe Schlüssel abhängig von Policy nicht mehr als bekannt gelten.

Die Retention-Zeit muss deshalb zur möglichen Retry-Dauer passen.

## Sicherheit

Idempotency Keys sind keine Berechtigungen.

```text
IdempotencyKey ≠ Capability
```

Jeder erneute Aufruf muss weiterhin die erforderlichen Sicherheitsregeln erfüllen.

Ein Angreifer darf durch Kenntnis eines Idempotency Keys keine fremden Ergebnisse abrufen oder Operationen autorisieren können.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Idempotency Records
Retention Time
Stored Results
Lookup Memory
Persistent Storage
Concurrent Lookups
```

Alte Records müssen kontrolliert entfernt werden können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
OperationID
IdempotencyKey
Scope
State
Attempt Count
Created
Expiration
Result State
Conflict State
```

Sensible Payloads oder Ergebnisse dürfen nicht ungefiltert ausgegeben werden.

## Normative Anforderungen

1. NovaOS MUSS ein gemeinsames Distributed-Idempotency-Modell bereitstellen können.
2. Eine logische Operation MUSS über Retries dieselbe Operationsidentität behalten können.
3. Idempotency Keys MÜSSEN einen definierten Scope besitzen.
4. Parallele Duplikate MÜSSEN kontrolliert erkannt werden können.
5. Ein bereits verwendeter Key DARF NICHT unbemerkt für eine andere Operation wiederverwendet werden.
6. Abgeschlossene Operationen DÜRFEN gespeicherte Ergebnisse erneut liefern.
7. Idempotenz DARF NICHT mit Exactly-Once Delivery gleichgesetzt werden.
8. Messaging und Queues SOLLEN idempotente Consumer unterstützen können.
9. Idempotenz SOLL mit Transaktionen atomar kombinierbar sein.
10. Idempotency Keys DÜRFEN NICHT als Autorisierung behandelt werden.
11. Idempotency Records MÜSSEN ressourcen- und zeitbegrenzt sein.
12. Idempotenzzustände und Konflikte SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-PUBSUB-0001`
- `NPSPEC-DISTCOMM-QUEUE-0001`
- `NPSPEC-DISTCOMM-RETRY-0001`
- `NPSPEC-DISTCOMM-SCHEMA-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-DISTCOMM-0013`

## Ergebnis

```text
Logical Operation
       ↓
OperationID + IdempotencyKey
       ↓
Duplicate Detection
       ↓
Execute Once / Reuse Result
       ↓
Controlled Effect
```

NovaOS erhält damit ein gemeinsames Idempotenz-Modell, das Retries und At-Least-Once-Kommunikation sicherer macht, indem mehrere Übertragungsversuche derselben logischen Operation erkannt und unbeabsichtigte Mehrfachwirkungen kontrolliert verhindert werden.