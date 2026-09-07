# ADR-TRANSACTION-0005 – Multi-Service Transactions

## Status

Angenommen

## Kategorie

Transaction Architecture / Services / Distributed Coordination

## Kontext

Systemoperationen in NovaOS können mehrere Services gleichzeitig betreffen.

Beispiele sind:

- koordinierte Konfigurationsänderungen,
- Provider-Wechsel,
- Service-Migrationen,
- Software-Updates,
- Änderungen gemeinsamer Daten,
- Identity- oder Policy-Änderungen,
- deklarative Desired-State-Änderungen.

Eine Änderung kann nur dann semantisch gültig sein, wenn alle beteiligten Services einen kompatiblen Zustand erreichen.

```text
Transaction
 ├── Service A
 ├── Service B
 └── Service C
```

Eine klassische globale ACID-Transaktion ist dafür nicht generell geeignet. Services können unterschiedliche Persistenzmodelle besitzen, remote ausgeführt werden, zeitweise nicht erreichbar sein oder irreversible Side Effects erzeugen.

NovaOS benötigt deshalb ein systemweites Modell für Multi-Service Transactions, das lokale Atomarität mit expliziter verteilter Operationssemantik verbindet.

## Entscheidung

NovaOS unterstützt **Multi-Service Transactions** als koordinierte System Transactions über mehrere Service-Grenzen.

```text
Multi-Service Transaction
          ↓
Transaction Plan
     ┌────┼────┐
     ↓    ↓    ↓
 Service A  B  C
     ↓    ↓    ↓
 Prepare / Execute
     └────┼────┘
          ↓
Commit / Compensate / Reconcile
          ↓
Verification
```

NovaOS garantiert dabei keine universelle globale ACID-Atomarität.

Jeder beteiligte Service muss stattdessen seine relevante Transaction-Semantik explizit deklarieren.

## Grundprinzip

```text
Cross-service consistency
is based on explicit semantics,
not assumed global atomicity.
```

Die Transaction Runtime koordiniert die beteiligten Services entsprechend ihrer tatsächlichen Fähigkeiten.

## Transaction Participants

Jeder beteiligte Service wird als Transaction Participant behandelt.

Ein Participant muss für relevante Operationen deklarieren können, welche Transaction-Eigenschaften er unterstützt.

Beispiele:

```text
Prepare
Atomic Commit
Rollback
Compensation
Idempotency
Reconciliation
Verification
```

Nicht jeder Service muss alle Eigenschaften unterstützen.

## Transaction Plan

Vor der Ausführung erstellt NovaOS einen Transaction Plan.

```text
Requested Change
      ↓
Participants
      ↓
Dependencies
      ↓
Transaction Semantics
      ↓
Transaction Plan
```

Der Plan beschreibt insbesondere:

- beteiligte Services,
- Abhängigkeiten,
- Operationsreihenfolge,
- Commit-Grenzen,
- Compensation,
- Recovery-Verhalten.

## Lokale Transaktionen

Jeder Service soll seine internen Änderungen nach Möglichkeit über eine lokale Transaction absichern.

```text
Global Coordination
       ↓
Service A → Local Transaction
Service B → Local Transaction
Service C → Local Transaction
```

Dadurch bleibt lokale Atomarität erhalten, ohne globale Atomarität vorauszusetzen.

## Prepare

Unterstützen alle erforderlichen Participants eine vorbereitbare Operation, kann NovaOS Änderungen zunächst vorbereiten.

```text
Prepare A
Prepare B
Prepare C
    ↓
All Ready?
 ├── Yes → Commit
 └── No  → Abort
```

Prepare darf jedoch nicht als universell verfügbare Eigenschaft angenommen werden.

## Commit

Der Commit einer Multi-Service Transaction beschreibt den semantischen Übergang zum neuen systemweiten Zustand.

Je nach beteiligten Services kann dieser durch unterschiedliche Mechanismen realisiert werden.

```text
Atomic Local Commit
Coordinated Commit
Ordered Commit
Compensatable Commit
```

Die konkrete Strategie muss aus den Transaction Contracts ableitbar sein.

## Compensation

Kann ein bereits ausgeführter Effekt nicht technisch zurückgerollt werden, muss eine semantische Compensation verwendet werden können.

Beispiel:

```text
Service A committed
Service B failed
      ↓
Compensate Service A
```

Compensation bedeutet nicht zwingend die exakte Wiederherstellung des vorherigen physischen Zustands, sondern die Wiederherstellung der erforderlichen Systeminvarianten.

## Reconciliation

Bei unbekanntem Zustand eines Participants muss NovaOS dessen tatsächlichen Zustand ermitteln.

```text
Commit Request
     ↓
Connection Lost
     ↓
Commit State Unknown
     ↓
Reconcile
```

Ein Timeout oder Kommunikationsfehler darf nicht automatisch als fehlgeschlagener Commit interpretiert werden.

## Idempotenz

Wiederholbare Multi-Service-Operationen sollen idempotent ausführbar sein.

Transaction Identity und Operation Identity müssen dafür zur Deduplication verwendet werden können.

```text
Request
Transaction = T42
Operation   = O7
```

Ein Retry darf denselben semantischen Effekt nicht unbeabsichtigt mehrfach erzeugen.

## Abhängigkeiten

Participants können voneinander abhängig sein.

```text
Service A
   ↓
Service B
   ↓
Service C
```

Die Transaction Runtime muss diese Abhängigkeiten bei Prepare, Commit, Compensation und Recovery berücksichtigen.

Eine rein parallele Ausführung ist nur zulässig, wenn keine relevante Reihenfolge erforderlich ist.

## Failure Isolation

Der Ausfall eines Participants soll nicht automatisch einen größeren Recovery Scope als notwendig erzeugen.

NovaOS versucht zunächst, den kleinsten konsistenten Scope wiederherzustellen.

```text
Service B Failure
      ↓
Local Recovery possible?
 ├── Yes → Continue Transaction
 └── No  → Transaction Recovery
```

## Timeouts

Timeouts müssen Bestandteil der Operationssemantik sein.

Ein Timeout bedeutet grundsätzlich:

```text
Result Unknown
```

wenn nicht garantiert werden kann, dass die Operation nicht ausgeführt wurde.

NovaOS muss deshalb zwischen:

```text
Failed
Succeeded
Unknown
```

unterscheiden können.

## RTO und RPO

Multi-Service Transactions müssen bestehende Recovery Contracts berücksichtigen.

Die Recovery-Strategie darf nicht unabhängig von RTO und RPO gewählt werden.

Bei kritischen Transactions kann ein Participant ausgeschlossen werden, wenn dessen Recovery-Eigenschaften die erforderlichen Contracts nicht erfüllen.

## Desired State

Multi-Service Transactions können die Ausführung atomarer Desired-State-Änderungen koordinieren.

```text
Desired State Change
       ↓
Multi-Service Transaction
       ↓
Service Coordination
       ↓
Reconciliation
       ↓
Verified Desired State
```

Der Desired-State-Commit und die physische Multi-Service-Konvergenz bleiben dabei getrennte Konzepte.

## Authority

Eine Multi-Service Transaction erzeugt keine gemeinsame Super-Authority.

Jeder Participant muss die für seine Operation erforderliche Authority prüfen.

```text
Transaction Authority
        ↓
Attenuated Context
        ↓
Service Participant
```

Delegierte Authority soll auf den tatsächlich notwendigen Scope begrenzt werden.

## Security und Trust

Jeder Participant bleibt an seine Security-, Trust- und Data-Sovereignty-Anforderungen gebunden.

Eine Multi-Service Transaction darf diese Grenzen nicht umgehen, nur um einen gemeinsamen Commit zu erreichen.

## Recovery

Unterbrochene Multi-Service Transactions müssen persistent rekonstruierbar sein können.

Mindestens relevant sind:

```text
Transaction Identity
Participants
Participant States
Executed Operations
Commit State
Compensation State
Recovery State
```

Nova.Resilience kann daraus die notwendige Recovery ableiten.

## Offline Recovery

Betrifft eine unterbrochene Multi-Service Transaction kritische Systemdienste, kann NovaDOS den persistenten Transaction State analysieren.

NovaDOS muss dabei dieselbe Operationssemantik beachten und darf unbekannte Remote- oder Service-Zustände nicht blind zurücksetzen.

## Verification

Eine Multi-Service Transaction gilt erst als erfolgreich, wenn die relevanten systemweiten Invarianten verifiziert wurden.

```text
Participants Completed
        ↓
Cross-Service Verification
        ↓
Valid?
 ├── Yes → Complete
 └── No  → Recovery
```

Der Erfolg jedes einzelnen Participants reicht nicht zwingend aus, wenn der resultierende Gesamtzustand inkonsistent ist.

## Normative Anforderungen

1. NovaOS MUSS Multi-Service Transactions als Spezialisierung der System Transactions unterstützen.
2. Beteiligte Services MÜSSEN als explizite Transaction Participants modellierbar sein.
3. Participants MÜSSEN ihre relevante Transaction-Semantik deklarieren können.
4. NovaOS DARF keine universelle globale ACID-Atomarität über Service-Grenzen voraussetzen.
5. Lokale Änderungen SOLLEN durch lokale Transactions abgesichert werden.
6. Multi-Service Transactions MÜSSEN einen expliziten Transaction Plan besitzen.
7. Participant-Abhängigkeiten MÜSSEN bei Ausführung und Recovery berücksichtigt werden.
8. Prepare DARF nur verwendet werden, wenn der jeweilige Participant diese Semantik unterstützt.
9. Nicht rollbackfähige Operationen MÜSSEN Compensation oder Reconciliation unterstützen können.
10. Unbekannte Commit-Zustände DÜRFEN nicht automatisch als fehlgeschlagen interpretiert werden.
11. Multi-Service-Operationen SOLLEN soweit semantisch möglich idempotent ausführbar sein.
12. Transaction Identity MUSS zur Deduplication und Recovery verwendbar sein.
13. Timeouts MÜSSEN von eindeutigem Operation Failure unterscheidbar sein.
14. Recovery MUSS den kleinsten konsistenten Recovery Scope anstreben.
15. RTO und RPO MÜSSEN bei Multi-Service-Recovery berücksichtigt werden.
16. Desired-State-Änderungen MÜSSEN Multi-Service Transactions verwenden können.
17. Multi-Service Transactions DÜRFEN keine zusätzliche Authority erzeugen.
18. Delegierte Authority MUSS auf den erforderlichen Participant- und Operations-Scope begrenzbar sein.
19. Security-, Trust-, Integrity- und Data-Sovereignty-Hard-Constraints MÜSSEN für jeden Participant erhalten bleiben.
20. Unterbrochene Multi-Service Transactions MÜSSEN persistent rekonstruierbar und recoverbar sein.
21. Cross-Service-Invarianten MÜSSEN nach relevanten Transactions verifizierbar sein.
22. Transaction Plan, Participant States, Commit, Compensation und Recovery MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- zusammengehörige Änderungen über Service-Grenzen können koordiniert werden,
- lokale Atomarität bleibt nutzbar,
- NovaOS setzt keine unrealistische globale ACID-Garantie voraus,
- irreversible und externe Effekte können über Compensation behandelt werden,
- unbekannte Zustände werden über Reconciliation statt blindem Retry behandelt,
- Multi-Service-Änderungen integrieren sich in Desired State und Nova.Resilience.

### Negative Konsequenzen

- Services müssen ihre Transaction-Semantik explizit beschreiben,
- Compensation kann komplexer als klassischer Rollback sein,
- unbekannte Remote-Zustände können zusätzliche Reconciliation benötigen,
- systemweite Verification kann aufwendiger als lokale Transaction Verification sein.

## Verworfene Alternativen

### Globale ACID-Transaktionen für alle Services

Verworfen, da nicht alle Services, Geräte oder externen Systeme eine gemeinsame atomare Commit-Semantik unterstützen.

### Best-Effort-Aufrufe ohne Transaction Coordination

Verworfen, da Teilfehler zu inkonsistenten systemweiten Zuständen führen können.

### Jeder Service führt unabhängige Recovery durch

Verworfen, da lokale Recovery allein systemweite Invarianten nicht garantieren kann.

### Timeout immer als Failure behandeln

Verworfen, da eine Operation trotz verlorener Antwort erfolgreich ausgeführt worden sein kann.

## Abhängigkeiten

- `ADR-TRANSACTION-0001_System_Transactions`
- `ADR-TRANSACTION-0002_Atomic_Desired_State_Changes`
- `ADR-TRANSACTION-0003_Configuration_Transactions`
- `ADR-TRANSACTION-0004_Resource_Transactions`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0005_Retry_nur_nach_expliziter_Operationssemantik`
- `ADR-RESILIENCE-0010_Rollback_über_Systemgrenzen`
- `ADR-RESILIENCE-0012_Recovery_Verification_vor_Restore`
- `ADR-RESILIENCE-0017_RTO_und_RPO_als_Recovery_Contracts`
- `ADR-CAPABILITY-0007_Capability_Delegation`
- `ADR-CAPABILITY-0008_Capability_Attenuation`

## Zugehörige NPSPECs

- `NPSPEC-TRANSACTION-MULTISERVICE-0001`
- `NPSPEC-TRANSACTION-PARTICIPANT-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-PLAN-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-COMMIT-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-COMPENSATION-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-RECONCILIATION-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-RECOVERY-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-TEST-0001`

## Ergebnis

NovaOS koordiniert zusammengehörige Änderungen über Service-Grenzen durch explizite Multi-Service Transactions:

```text
System Transaction
       ↓
Transaction Plan
       ↓
Participants
 ┌─────┼─────┐
 ↓     ↓     ↓
 A     B     C
 ↓     ↓     ↓
Local Transaction Semantics
 └─────┼─────┘
       ↓
Commit / Compensate / Reconcile
       ↓
Cross-Service Verification
       ↓
Consistent System State
```

Damit erhält NovaOS ein realistisches Transaktionsmodell für verteilte Systemoperationen, ohne eine globale Atomarität zu versprechen, die über Service-, Geräte- oder Netzwerkgrenzen technisch nicht zuverlässig garantiert werden kann.