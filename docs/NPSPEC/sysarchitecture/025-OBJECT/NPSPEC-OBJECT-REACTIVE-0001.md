# NPSPEC-OBJECT-REACTIVE-0001 – Nova Reactive Object Model

## Status

Angenommen

## Kategorie

Object / Reactive / Event-Driven Architecture

## Zweck

NovaOS definiert ein reaktives Objektmodell, bei dem Änderungen an Objekten kontrolliert beobachtet und abhängige Verarbeitung automatisch ausgelöst werden kann.

```text
Object Change
     ↓
State Transition
     ↓
Reactive Event
     ↓
Dependencies
     ↓
Affected Operations
```

Dadurch können Systemkomponenten auf Zustandsänderungen reagieren, ohne Objekte permanent abzufragen.

## Grundprinzipien

```text
Object Change ≠ Automatic Authority
Event ≠ Capability
Subscription ≠ Object Access
Observation ≠ Modification
Dependency ≠ Ownership
Notification ≠ Guaranteed Processing
Reactive Execution ≠ Uncontrolled Execution
State Change ≠ Immediate Global Propagation
```

## Reactive Object

Ein reaktiv beobachtbares Objekt besitzt:

```text
ReactiveObject
├── ObjectID
├── VersionID
├── Observable State
└── Change Generation
```

Optional:

```text
SemanticTypeID
Reactive Properties
Dependencies
Subscribers
Event Policy
Consistency Policy
```

## Reactive Events

Objektänderungen können strukturierte Ereignisse erzeugen.

```text
ObjectEvent
├── EventID
├── ObjectID
├── EventType
├── VersionID
└── Timestamp
```

Optionale Informationen:

```text
Previous VersionID
Changed Properties
TransactionID
ExecutionID
ProvenanceID
Cause EventID
```

## Ereignistypen

Beispiele:

```text
Created
Modified
VersionChanged
MetadataChanged
RelationshipChanged
StateChanged
Moved
Retired
Available
Unavailable
```

Objekttypen dürfen zusätzliche Ereignisse definieren.

## Beobachtung

Komponenten können relevante Änderungen abonnieren.

```text
Subscriber
    ↓
Subscription
    ↓
Object / Type / Condition
```

Subscriptions können beispielsweise filtern nach:

```text
ObjectID
ObjectTypeID
SemanticTypeID
Relationship
EventType
Metadata Condition
```

## Capability Security

Eine Subscription benötigt explizite Autorität.

```text
Observe Capability
        ↓
Subscription
        ↓
Authorized Events
```

Dabei gilt:

```text
Observe(Object) ≠ Read(Object)
Observe(Object) ≠ Modify(Object)
```

Ein Event darf keine geschützten Objektinformationen offenlegen, für die der Empfänger keine Berechtigung besitzt.

## Abhängigkeiten

Objekte können reaktive Abhängigkeiten besitzen.

```text
Object A
   ↓ changes
Object B
   ↓ invalidated
Operation
   ↓ recompute
Object C
```

Abhängigkeiten sollen über stabile `ObjectID`s und Semantic Relationships beschrieben werden.

## Invalidierung

Nicht jede Änderung muss sofort eine Neuberechnung auslösen.

NovaOS kann zunächst abhängige Zustände markieren:

```text
Valid
  ↓
Dependency Changed
  ↓
Stale
  ↓
Recompute
  ↓
Valid
```

Dadurch können unnötige Berechnungen vermieden werden.

## Reactive Execution

Ein Event kann eine semantische Operation auslösen.

```text
Object Event
     ↓
Reactive Rule
     ↓
Semantic Execution
     ↓
ExecutionContract
     ↓
Result
```

Die ausgelöste Operation benötigt weiterhin ihre eigenen Capabilities.

```text
Event ≠ Authority Transfer
```

## Event-Ketten

Reaktive Verarbeitung kann weitere Änderungen erzeugen.

```text
Object A
   ↓
Event A
   ↓
Operation
   ↓
Object B
   ↓
Event B
```

NovaOS muss Ursache und Folge nachvollziehen können.

Dafür können verwendet werden:

```text
Cause EventID
ExecutionID
TransactionID
TraceID
```

## Schleifenschutz

Reaktive Abhängigkeiten können Zyklen erzeugen.

```text
A → B → C → A
```

NovaOS muss Mechanismen unterstützen für:

```text
Cycle Detection
Generation Limits
Duplicate Suppression
Idempotency
Convergence Detection
```

Unkontrollierte Event-Schleifen dürfen nicht unbegrenzt Ressourcen verbrauchen.

## Transaktionen

Änderungen innerhalb einer Transaktion sollen nicht zwingend als einzelne externe Events sichtbar werden.

```text
Begin
 ↓
Change A
Change B
Change C
 ↓
Commit
 ↓
Committed Events
```

Beobachter sollen grundsätzlich nur konsistente veröffentlichte Zustände erhalten.

## Backpressure

Schnelle Objektänderungen dürfen Subscriber nicht unbegrenzt überlasten.

Mögliche Strategien:

```text
Bounded Queue
Coalescing
Latest-State
Batching
Priority
Drop Policy
```

Die Strategie muss zur Semantik des Event-Typs passen.

## Versionierung

Reactive Events sollen konkrete Objektversionen referenzieren können.

```text
Object:v7
   ↓ Change
Object:v8
   ↓
VersionChanged(v7 → v8)
```

Dadurch können Subscriber erkennen, ob Zwischenversionen fehlen.

## Verteilte Objekte

Reactive Events können lokale und entfernte Komponenten erreichen.

Dabei müssen berücksichtigt werden:

```text
Ordering
Duplicates
Delay
Disconnect
Stale State
Reconnect
```

Eine Event-Zustellung darf nicht automatisch als global synchroner Zustand interpretiert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
Reactive State
Change Generation
Subscriptions
Dependencies
Pending Events
Last Event
Invalidation State
Reactive Executions
```

## Normative Anforderungen

1. NovaOS MUSS Objektänderungen als strukturierte Reactive Events darstellen können.
2. Reactive Events SOLLEN stabile `ObjectID`s und `VersionID`s verwenden.
3. Observation MUSS von Read-, Write- und Execution-Authority getrennt bleiben.
4. Events DÜRFEN keine implizite Capability übertragen.
5. Reaktive Abhängigkeiten MÜSSEN kontrolliert invalidierbar sein.
6. Reactive Execution MUSS die normale Capability- und ExecutionContract-Architektur verwenden.
7. Event-Ketten MÜSSEN nachvollziehbar sein können.
8. NovaOS MUSS Schutz gegen unkontrollierte reaktive Zyklen unterstützen.
9. Transaktionale Änderungen SOLLEN erst als konsistenter veröffentlichter Zustand sichtbar werden.
10. Reactive Processing MUSS Backpressure und autorisierte Introspection unterstützen können.

## Abhängigkeiten

- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-REFERENCE-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-RELATIONSHIP-0001`
- `NPSPEC-OBJECT-PIPELINE-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-IPC-EVENTBUS-0001`
- `NPSPEC-IPC-PUBSUB-0001`
- `NPSPEC-IPC-BACKPRESSURE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `ADR-ARCH-0030`

## Ergebnis

```text
Object State Change
        ↓
Reactive Event
        ↓
Authorized Subscription
        ↓
Dependency Invalidation
        ↓
Semantic Execution
        ↓
Updated Objects
```

NovaOS erhält damit ein reaktives Objektmodell, bei dem Änderungen automatisch und kontrolliert durch das System propagiert werden können, ohne Polling, Objektidentität, Event-Verteilung und Autorität miteinander zu vermischen.