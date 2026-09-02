# ADR-DISTCOMM-0011 – Sovereignty Preflight vor Remote Execution

## Status

Angenommen

## Kategorie

Distributed Communication / Data Sovereignty / Remote Execution / Information Flow / Nova.Call

## Kontext

NovaOS unterstützt mit `Nova.Call` location-transparente Aufrufe über lokale und entfernte Execution Domains hinweg.

Ein logischer Service kann dadurch abhängig von:

- Verfügbarkeit,
- Last,
- Hardware,
- Accelerator-Verfügbarkeit,
- Latenz,
- Energiebedarf,
- Ressourcenbudgets

lokal oder auf einem entfernten System ausgeführt werden.

Location Transparency darf jedoch nicht dazu führen, dass Daten oder Verarbeitung unbemerkt eine unzulässige Sovereignty-, Trust- oder administrative Grenze überschreiten.

Ein Remote Call kann bereits vor seiner eigentlichen Ausführung relevante Datenbewegungen verursachen:

```text
Caller
  ↓
Serialization
  ↓
Network Transfer
  ↓
Remote System
  ↓
Authorization
```

Würde Data Sovereignty erst auf dem Remote System geprüft, wären möglicherweise bereits Daten an einen Ort oder Provider übertragen worden, den der Contract verbietet.

NovaOS benötigt deshalb eine verbindliche Prüfung **vor Beginn einer Remote Execution und vor der dafür erforderlichen Datenübertragung**.

---

## Entscheidung

NovaOS führt einen **Sovereignty Preflight** für Remote Execution ein.

Vor einer Remote-Ausführung muss geprüft werden, ob der geplante Ausführungs- und Kommunikationspfad die geltenden:

- Data-Sovereignty Policies,
- Information-Flow Policies,
- Trust Requirements,
- Security Constraints,
- Location Constraints

erfüllt.

```text
Nova.Call
    ↓
Remote Execution Candidate
    ↓
Sovereignty Preflight
    ↓
Allowed?
 ├── No  → Reject / Replan
 └── Yes → Data Transfer + Remote Execution
```

Der Preflight erfolgt vor der Freigabe geschützter Daten an die Remote Execution Domain.

---

## Grundprinzip

```text
Check before transfer.
Check before execution.
```

und:

```text
Reachable
    ≠
Permitted
```

---

## Preflight als Hard Constraint

Data Sovereignty ist keine nachträgliche Optimierung.

Sie wird vor Performance-, Kosten-, Energie- oder Adaptionszielen geprüft.

Konzeptionell gilt:

```text
Remote Candidate
      ↓
Security
      ↓
Data Sovereignty
      ↓
Information Flow
      ↓
Trust
      ↓
Eligible?
      ↓
Performance Optimization
```

Ein schnellerer oder günstigerer Remote Provider darf nicht gewählt werden, wenn er Hard Constraints verletzt.

---

## Preflight Scope

Der Sovereignty Preflight bewertet nicht nur den endgültigen Remote Service.

Er muss alle für die geplante Ausführung relevanten bekannten Komponenten berücksichtigen können.

Dazu gehören insbesondere:

- Remote Execution Domain,
- Service Instance,
- Provider,
- administrative Domain,
- geografische Location,
- Netzwerkpfad,
- Transit Domains,
- Overlay/VPN,
- benötigte Storage Services,
- nachgelagerte Remote Dependencies.

```text
Caller
  ↓
Network Path
  ↓
Provider
  ↓
Remote Execution
  ↓
Remote Dependency
```

Eine zulässige Zielmaschine allein reicht nicht aus, wenn der erforderliche Datenpfad oder eine notwendige Abhängigkeit unzulässig ist.

---

## Datenklassifikation

Der Preflight muss die Sovereignty- und Information-Flow-Eigenschaften der tatsächlich betroffenen Daten berücksichtigen.

```text
Input Data
 ├── Semantic Type
 ├── Classification
 ├── Sovereignty Policy
 ├── Information-Flow Policy
 └── Provenance
```

Nicht jeder Call benötigt dieselben Einschränkungen.

Ein Call ohne sensible Daten kann andere zulässige Execution Locations besitzen als ein Call mit stark eingeschränkten Daten.

---

## Semantic Types

Semantic Types können Sovereignty-relevante Eigenschaften tragen oder mit entsprechenden Policies verknüpft sein.

Beispielsweise können semantisch unterschiedliche Daten trotz identischer technischer Repräsentation unterschiedliche Regeln besitzen.

```text
PublicImage
PatientRecord
AuthenticationSecret
TelemetryData
FinancialRecord
```

Der Sovereignty Preflight bewertet daher nicht ausschließlich rohe Byte Payloads, sondern deren bekannte semantische Bedeutung.

---

## Execution Contract

Sovereignty Requirements werden in den Execution- beziehungsweise Call Contract integriert.

Konzeptionell:

```text
Nova.ExecutionContract
 ├── Semantic Types
 ├── Data-Sovereignty Policy
 ├── Trust Requirement
 ├── Location Constraints
 ├── Resource Budget
 ├── Deadline
 └── Determinism Requirement
```

Damit wird Sovereignty Teil der normalen Ausführungsplanung und kein separater nachträglicher Sondermechanismus.

---

## Remote Execution Planning

Der Sovereignty Preflight ist Teil der Planung einer Remote Execution.

```text
Execution Intent
      ↓
Candidate Discovery
      ↓
Sovereignty Preflight
      ↓
Eligible Candidates
      ↓
Soft Optimization
      ↓
Selected Execution Target
```

Nicht zulässige Kandidaten werden vor der Optimierungsphase entfernt.

---

## Unknown ist nicht Allowed

Für Hard Sovereignty Constraints gilt:

```text
Known Allowed     → Eligible
Known Forbidden   → Reject
Unknown           → Reject / Require Resolution
```

NovaOS darf fehlende Location-, Provider-, Transit- oder Trust-Informationen nicht automatisch als zulässig behandeln.

Die Policy kann explizit definieren, welche Unsicherheiten akzeptabel sind.

Ohne eine solche Erlaubnis gilt Fail-Safe Default.

---

## Netzwerkpfad

Der Sovereignty Preflight integriert sich mit Sovereignty-Aware Routing.

Ein Remote Target ist nur dann verwendbar, wenn auch ein zulässiger Kommunikationspfad existiert.

```text
Remote Target Allowed
        +
Route Allowed
        =
Remote Execution Possible
```

Ein zulässiges Ziel über einen unzulässigen Transitpfad ist kein zulässiger Execution Plan.

---

## Overlay und VPN

Ein VPN oder verschlüsseltes Overlay verändert nicht automatisch die Sovereignty-Eigenschaften des Underlay-Pfades.

```text
Encrypted
    ≠
Sovereignty Compliant
```

Wenn eine Policy bestimmte geografische, administrative oder Provider-Grenzen verbietet, darf Verschlüsselung diese Einschränkung nicht automatisch aufheben.

Overlay und Underlay müssen entsprechend der Policy bewertet werden.

---

## Service Discovery

Service Discovery kann mehrere Instanzen desselben logischen Services liefern.

```text
Service Identity
 ├── Instance A – Region A
 ├── Instance B – Region B
 └── Instance C – Region C
```

Der Sovereignty Preflight filtert nicht zulässige Instanzen vor der endgültigen Auswahl.

Die logische Service Identity allein garantiert keine zulässige Execution Location.

---

## Capability und Authority

Eine gültige Remote Capability bedeutet nicht automatisch, dass Daten an das entsprechende Ziel übertragen werden dürfen.

```text
Capability
    =
Authority to perform operation

Sovereignty Policy
    =
Authority/constraint on information movement
```

Beide Bedingungen müssen erfüllt sein.

```text
Authorized
     +
Sovereignty Compliant
     =
Potentially Executable
```

---

## Trust

Auch ein technisch zulässiger geografischer Standort kann aufgrund seines Trust Contexts ungeeignet sein.

Der Preflight muss deshalb Data Sovereignty und Trust gemeinsam berücksichtigen können.

Beispielsweise kann eine Policy Verarbeitung nur auf:

- bestimmten vertrauenswürdigen Geräten,
- bestimmten administrativen Domains,
- attestierten Execution Environments

erlauben.

---

## Delegierte Capabilities

Capability Delegation darf Sovereignty Constraints nicht abschwächen.

Wird eine Remote Capability an einen weiteren Service delegiert, muss dessen Verwendung weiterhin mit den geltenden Sovereignty Policies kompatibel sein.

```text
Authority Delegation
       ↓
Sovereignty unchanged or stricter
```

Delegationsauthority erzeugt keine zusätzliche Data-Movement-Authority.

---

## Zero-Serialization Local Fast Path

Der Sovereignty Preflight ist insbesondere relevant für die Entscheidung zwischen lokaler und entfernter Ausführung.

```text
Execution Candidates
 ├── Local
 └── Remote
       ↓
   Preflight required
```

Ist Remote Execution nicht zulässig, kann NovaOS eine lokale Execution Domain verwenden, sofern diese den Contract erfüllt.

Der lokale Fast Path darf dadurch als zulässige Alternative gewählt werden.

---

## Replanning

Scheitert ein Sovereignty Preflight, darf NovaOS einen anderen zulässigen Execution Plan suchen.

```text
Candidate A
    ↓
Sovereignty Failure
    ↓
Candidate B
    ↓
Preflight
    ↓
Allowed
```

Ein solcher Replan darf Hard Constraints nicht abschwächen.

Existiert kein zulässiger Kandidat, muss die Operation:

- warten,
- fehlschlagen,
- lokal ausgeführt,
- oder explizit degradiert

werden, sofern der Contract dies erlaubt.

---

## Änderungen nach dem Preflight

Ein erfolgreicher Preflight ist keine unbegrenzte dauerhafte Freigabe.

Ändern sich relevante Bedingungen vor oder während der Ausführung, kann eine erneute Prüfung erforderlich sein.

Beispiele:

- Connection Migration,
- Providerwechsel,
- Service Migration,
- Failover,
- neue Remote Dependency,
- Änderung des Network Path,
- Änderung relevanter Trust Information.

```text
Plan Changed
     ↓
Sovereignty-Relevant?
 ├── No  → Continue
 └── Yes → Revalidate
```

---

## Connection Migration

Eine bestehende Remote Operation darf bei Connection Migration nicht automatisch auf einen Sovereignty-inkompatiblen Pfad wechseln.

Vor einem relevanten Pfadwechsel muss die neue Route erneut gegen die geltenden Hard Constraints geprüft werden.

---

## Remote Service Migration

Wird ein logischer Service während seiner Lebensdauer auf eine andere Execution Location verschoben, darf die bestehende Service Identity nicht als automatische Sovereignty-Freigabe gelten.

```text
Same Service Identity
        ≠
Same Sovereignty Context
```

Die neue Execution Location muss erneut zulässig sein.

---

## Nested Remote Calls

Ein Remote Service kann selbst weitere Remote Calls auslösen.

```text
Caller
  ↓
Service A
  ↓
Service B
  ↓
Service C
```

Sovereignty Constraints müssen entlang dieser Causation Chain propagiert werden.

Ein zulässiger Call zu Service A darf nicht dazu führen, dass A die Daten ohne entsprechende Authority an einen unzulässigen Service B weitergibt.

---

## End-to-End Deadline

Sovereignty Preflight verbraucht einen Teil des End-to-End Deadline Budgets.

Eine knappe Deadline darf jedoch nicht dazu führen, dass der Preflight übersprungen wird.

```text
Deadline Pressure
      ≠
Permission to bypass sovereignty
```

Wenn kein rechtzeitig validierbarer zulässiger Plan existiert, muss die Operation entsprechend ihrem Contract fehlschlagen oder degradieren.

---

## Caching

Preflight-Ergebnisse dürfen kontrolliert gecacht werden, wenn ihre Voraussetzungen weiterhin gültig sind.

Ein Cache-Eintrag muss an relevante Eigenschaften gebunden sein, beispielsweise:

- Policy Version,
- Data Classification,
- Target Identity,
- Execution Location,
- Provider,
- Route Context,
- Trust State.

Ändert sich eine relevante Voraussetzung, darf ein veraltetes Preflight-Ergebnis nicht weiter als Autorisierung verwendet werden.

Die konkrete Cache-Semantik wird in NPSPECs definiert.

---

## TOCTOU

Zwischen Preflight und tatsächlicher Ausführung können sich Bedingungen ändern.

NovaOS muss deshalb vermeiden, dass ein erfolgreich geprüfter Plan anschließend unter wesentlich anderen Bedingungen ausgeführt wird.

```text
Preflight
   ↓
Approved Plan
   ↓
Relevant State Changed
   ↓
Revalidation
```

Die konkrete Bindung zwischen geprüftem Plan und tatsächlicher Ausführung wird in NPSPECs definiert.

---

## Graceful Degradation

Wenn Remote Execution aufgrund von Sovereignty Constraints nicht möglich ist, kann der Contract eine zulässige Alternative definieren.

Beispiele:

```text
Remote Accelerator
        ↓
Not Sovereignty Compliant
        ↓
Local CPU Execution
```

oder:

```text
High-Quality Remote Model
        ↓
Not Permitted
        ↓
Local Smaller Model
```

Eine solche Degradation muss explizit erlaubt sein.

---

## Adaptive Systeme

NovaOS darf adaptive Systeme verwenden, um wahrscheinlich zulässige und geeignete Execution Candidates schneller zu priorisieren.

Prediction Error kann dabei zur Verbesserung der Kandidatenauswahl verwendet werden.

Adaptive Modelle dürfen jedoch niemals die verbindliche Sovereignty-Prüfung ersetzen.

```text
Prediction
    ↓
Candidate Ordering

Policy Validation
    ↓
Actual Permission
```

---

## Deterministic Mode

Im Deterministic Mode muss die Auswahl zulässiger Execution Locations und relevanter Preflight-Entscheidungen reproduzierbar kontrollierbar sein.

Die zugrunde liegenden Hard Sovereignty Constraints gelten unabhängig vom Deterministic Mode unverändert.

---

## Resource Economy

Sovereignty Preflight selbst verbraucht Ressourcen.

Dazu können gehören:

- Policy Evaluation,
- Route Evaluation,
- Trust Validation,
- Location Resolution,
- Attestation,
- Dependency Analysis.

Diese Kosten werden in die systemweite Resource Economy integriert.

Resource Pressure darf jedoch nicht dazu führen, dass verpflichtende Hard-Constraint-Prüfungen übersprungen werden.

---

## Introspection und Audit

NovaOS muss Remote-Execution-Entscheidungen hinsichtlich ihrer Sovereignty-Bewertung kontrolliert nachvollziehbar machen.

Beispielsweise:

```text
Remote Execution Plan
 ├── Target Identity
 ├── Execution Location
 ├── Provider
 ├── Route
 ├── Data Classification
 ├── Sovereignty Policy
 ├── Trust State
 ├── Preflight Result
 └── Decision Reason
```

Bei Ablehnung soll nachvollziehbar sein, welcher Hard Constraint den Kandidaten ausgeschlossen hat.

Sensitive Policy- oder Dateninformationen dürfen dabei nicht unnötig offengelegt werden.

---

## Normative Anforderungen

1. NovaOS MUSS vor relevanter Remote Execution einen Sovereignty Preflight durchführen können.
2. Der Preflight MUSS vor der Freigabe geschützter Daten an die Remote Execution Domain erfolgen.
3. Data Sovereignty MUSS als Hard Constraint vor Soft Optimization behandelt werden.
4. Der Preflight MUSS die geplante Execution Location berücksichtigen.
5. Der Preflight MUSS relevante Netzwerk-, Provider-, Transit- und Overlay-Kontexte berücksichtigen können.
6. Semantic Types und Data Classification MÜSSEN in Sovereignty-Entscheidungen einbezogen werden können.
7. Sovereignty Requirements MÜSSEN Bestandteil von `Nova.ExecutionContract` beziehungsweise `Nova.Call` Contracts sein können.
8. Nicht zulässige Remote Candidates MÜSSEN vor der Soft-Optimization-Phase ausgeschlossen werden.
9. Unbekannte Sovereignty-relevante Informationen DÜRFEN ohne explizite Policy NICHT als zulässig behandelt werden.
10. Ein zulässiges Remote Target MUSS über einen ebenfalls zulässigen Kommunikationspfad erreichbar sein.
11. Verschlüsselung oder VPN DÜRFEN Sovereignty Constraints NICHT automatisch außer Kraft setzen.
12. Service Discovery MUSS Sovereignty-basierte Candidate-Filterung unterstützen können.
13. Eine gültige Capability DARF NICHT als automatische Data-Sovereignty-Freigabe behandelt werden.
14. Capability Delegation DARF bestehende Sovereignty Constraints NICHT abschwächen.
15. Trust Requirements MÜSSEN gemeinsam mit Sovereignty Constraints berücksichtigt werden können.
16. Bei einem fehlgeschlagenen Preflight DARF NovaOS einen alternativen zulässigen Execution Plan suchen.
17. Ein Replan DARF Hard Constraints NICHT abschwächen.
18. Sovereignty-relevante Änderungen des Execution Plans MÜSSEN eine erneute Validierung auslösen können.
19. Connection Migration und Service Migration DÜRFEN bestehende Sovereignty Constraints NICHT umgehen.
20. Sovereignty Constraints MÜSSEN entlang abhängiger Remote Calls propagierbar sein.
21. Deadline Pressure DARF verpflichtende Sovereignty-Prüfungen NICHT umgehen.
22. Gecachte Preflight-Ergebnisse DÜRFEN nur verwendet werden, solange ihre relevanten Voraussetzungen gültig sind.
23. NovaOS MUSS TOCTOU-relevante Änderungen zwischen Preflight und tatsächlicher Ausführung erkennen beziehungsweise kontrollieren können.
24. Graceful Degradation aufgrund fehlender Remote-Zulässigkeit DARF nur innerhalb explizit erlaubter Alternativen erfolgen.
25. Adaptive Systeme DÜRFEN Sovereignty Validation NICHT ersetzen.
26. Resource Pressure DARF verpflichtende Sovereignty-Prüfungen NICHT deaktivieren.
27. Preflight-Ergebnisse und Ablehnungsgründe MÜSSEN kontrolliert introspektierbar und auditierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- geschützte Daten verlassen eine zulässige Sovereignty Domain nicht bereits vor der Policy-Prüfung,
- Remote Execution wird direkt mit Data Sovereignty und Information Flow verbunden,
- Service Discovery, Routing und Execution Planning verwenden dieselben Hard Constraints,
- Cloud-, Edge-, Cluster- und Accelerator-Ausführung können kontrolliert eingesetzt werden,
- Location Transparency bleibt erhalten, ohne geografische oder administrative Grenzen zu verstecken,
- Failover und Migration können Sovereignty sicher berücksichtigen,
- lokale Ausführung kann automatisch als zulässige Alternative verwendet werden.

### Negative Konsequenzen

- Remote Execution benötigt zusätzliche Planungs- und Policy-Prüfungen,
- Informationen über Location, Provider und Transit Domains müssen verfügbar und vertrauenswürdig genug sein,
- dynamische Migration kann erneute Preflight-Prüfungen benötigen,
- strikte Policies können die Menge verfügbarer Remote-Ressourcen erheblich reduzieren.

---

## Verworfene Alternativen

### Sovereignty erst auf dem Remote System prüfen

Verworfen.

Zu diesem Zeitpunkt können geschützte Daten bereits eine unzulässige System-, Provider- oder geografische Grenze überschritten haben.

### Capability als ausreichende Remote-Freigabe

Verworfen.

Authority zur Ausführung einer Operation und Authority beziehungsweise Policy zur Bewegung von Informationen sind unterschiedliche Konzepte.

### VPN automatisch als Sovereignty-konform behandeln

Verworfen.

Verschlüsselung schützt Inhalte, ändert aber nicht automatisch geografische, administrative oder Provider-Eigenschaften des Underlays.

### Remote Execution zuerst starten und bei Policy-Verletzung abbrechen

Verworfen.

Eine bereits erfolgte Datenübertragung kann nicht durch einen späteren Abbruch rückgängig gemacht werden.

### Soft Penalty für nicht bevorzugte Sovereignty Domains

Verworfen.

Verbindliche Data-Sovereignty-Regeln sind Hard Constraints und dürfen nicht durch Performance- oder Kostenoptimierung überstimmt werden.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0002_Mechanism_Policy_Separation`
- `ADR-ARCH-0003_Declarative_System_Model`
- `ADR-ARCH-0005_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0014_Explicit_Contracts`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`
- `ADR-DISTCOMM-0002_Location_Transparency_ohne_Kosten_und_Fehler_zu_verbergen`
- `ADR-DISTCOMM-0003_Remote_Capabilities`
- `ADR-DISTCOMM-0004_Capability_Delegation_über_Netzwerkgrenzen`
- `ADR-DISTCOMM-0005_Typed_Schemas_und_versionierte_Serialisierung`
- `ADR-DISTCOMM-0010_End_To_End_Deadline_Propagation`
- `ADR-NETWORK-0008_Policy_Based_Routing`
- `ADR-NETWORK-0012_Service_Discovery`
- `ADR-NETWORK-0014_Native_Overlay_VPN_Architektur`
- `ADR-NETWORK-0019_Connection_Migration`
- `ADR-NETWORK-0020_Intent_Based_Networking`
- `ADR-NETWORK-0021_Data_Sovereignty_Aware_Routing`

---

## Zugehörige NPSPECs

- `NPSPEC-DISTCOMM-SOVEREIGNTY-PREFLIGHT-0001`
- `NPSPEC-DISTCOMM-REMOTE-EXECUTION-POLICY-0001`
- `NPSPEC-DISTCOMM-EXECUTION-LOCATION-0001`
- `NPSPEC-DISTCOMM-SOVEREIGNTY-CONTEXT-0001`
- `NPSPEC-DISTCOMM-SOVEREIGNTY-PROPAGATION-0001`
- `NPSPEC-DISTCOMM-SOVEREIGNTY-REVALIDATION-0001`
- `NPSPEC-DISTCOMM-SOVEREIGNTY-PREFLIGHT-CACHE-0001`
- `NPSPEC-DISTCOMM-SOVEREIGNTY-PREFLIGHT-INTROSPECTION-0001`
- `NPSPEC-DISTCOMM-SOVEREIGNTY-PREFLIGHT-TEST-0001`

---

## Ergebnis

NovaOS prüft die Zulässigkeit einer Remote Execution **bevor** geschützte Daten übertragen oder Remote-Ressourcen verwendet werden:

```text
                  Execution Intent
                         ↓
                 Candidate Discovery
                         ↓
               Remote Candidate Plan
                         ↓
                Sovereignty Preflight
                         ↓
          ┌──────────────┴──────────────┐
          ↓                             ↓
      Forbidden                      Allowed
          ↓                             ↓
   Reject / Replan              Soft Optimization
                                        ↓
                               Selected Remote Plan
                                        ↓
                                  Data Transfer
                                        ↓
                                Remote Execution
```

Damit wird Data Sovereignty zu einem integralen Bestandteil der Distributed-Execution-Architektur.

NovaOS entscheidet nicht lediglich, **wo eine Operation technisch ausgeführt werden kann**, sondern prüft vor jeder relevanten Remote-Ausführung, **wo ihre Daten und Verarbeitung tatsächlich stattfinden dürfen**.