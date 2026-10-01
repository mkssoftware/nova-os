# NPSPEC-DISTRIBUTED-REMOTECAPABILITY-0001 – Nova Distributed Remote Capability

## Status

Angenommen

## Kategorie

Distributed / Capability / Remote Authority

## Zweck

NovaOS definiert ein sicheres Modell zur Nutzung und Delegation von Capabilities über System- und Knotengrenzen hinweg.

Eine Remote Capability repräsentiert explizite Autorität gegenüber einem entfernten Objekt, Dienst oder Provider, ohne Netzwerkadresse, Verbindung oder Identität selbst zu einer Berechtigung zu machen.

```text
Local Authority
      ↓
Delegation
      ↓
Attenuation
      ↓
Remote Capability
      ↓
Distributed IPC
      ↓
Authorized Remote Operation
```

## Grundprinzipien

```text
Remote Capability ≠ Network Connection
Remote Capability ≠ Endpoint
Remote Capability ≠ Identity
Remote Capability ≠ Authentication
Remote Capability ≠ Trust
Remote Capability ≠ ObjectID

Reachable ≠ Authorized
Authenticated ≠ Authorized
Trusted ≠ Authorized
Known ObjectID ≠ Authorized
```

## Remote Capability Model

Eine Remote Capability wird logisch beschrieben durch:

```text
RemoteCapability
├── CapabilityID
├── CapabilityTypeID
├── Target ObjectID
├── Authority
├── Issuer
└── State
```

Optional:

```text
Holder
Delegation Chain
Constraints
Expiration
ExecutionID
TaskID
Trust Requirements
Sovereignty Constraints
Revocation Information
Remote Endpoint Reference
```

Die `CapabilityID` identifiziert die konkrete Authority-Instanz.

## Authority

Die Capability beschreibt explizit erlaubte Operationen.

Beispiel:

```text
ObjectID: StorageObject-42

Authority:
├── Read
└── MetadataRead
```

Andere Rechte werden nicht implizit gewährt.

```text
Read ≠ Write
Read ≠ Share
Read ≠ Delegate
```

## Delegation

Remote Authority entsteht durch kontrollierte Delegation.

```text
Capability A
      ↓
Delegation
      ↓
Remote Capability B
```

Dabei gilt:

```text
Authority(B) ⊆ Authority(A)
```

Delegation darf niemals zusätzliche Authority erzeugen.

## Attenuation

Remote Capabilities sollen möglichst eingeschränkt werden.

Mögliche Constraints:

```text
Allowed Operations
Object Scope
Time Limit
ExecutionID
TaskID
ProviderID
Location
Data Scope
Maximum Usage
Delegation Depth
```

Beispiel:

```text
Original:
Read + Write + Share

Remote:
Read
Object = X
Execution = Y
Expires = T
```

## Task-Bound Capabilities

Für Distributed Execution sollen Remote Capabilities bevorzugt an konkrete Tasks gebunden werden.

```text
Execution
   ↓
Remote Task
   ↓
Temporary Remote Capability
```

Nach Ende des Tasks kann die Capability:

```text
Expire
Revoke
Destroy
```

werden.

## Capability Transfer

Capabilities dürfen nicht als gewöhnliche Datenobjekte übertragen werden.

```text
Sender
   ↓
Kernel / Capability System
   ↓
Validated Delegation
   ↓
Distributed IPC
   ↓
Remote Capability System
   ↓
Receiver
```

Der Transfer muss durch die beteiligten Capability-Systeme kontrolliert werden.

## Capability Representation

Die Transportdarstellung einer Remote Capability ist nicht die Capability selbst.

```text
Capability Authority
      ↓
Protected Representation
      ↓
Transport
      ↓
Validation
      ↓
Remote Capability Handle
```

Tokens müssen gegen:

```text
Forgery
Modification
Replay
Unauthorized Duplication
Disclosure
```

geschützt werden.

## Remote Handles

Ein Prozess kann eine Remote Capability über einen lokalen Handle referenzieren.

```text
Process
   ↓
Local Handle
   ↓
Remote Capability
   ↓
Remote Object
```

Der Handle besitzt nur innerhalb seines lokalen Capability-Kontextes Bedeutung.

```text
Handle ≠ Global CapabilityID
```

## Endpoint Resolution

Eine Remote Capability darf unabhängig vom aktuellen Netzwerkendpunkt bleiben.

```text
CapabilityID
      ↓
Target Identity
      ↓
Endpoint Resolution
      ↓
Current Endpoint
```

Eine Änderung von:

```text
IP
Port
Route
Node
```

muss nicht automatisch eine neue Authority erzeugen.

## Authentication

Vor Nutzung einer Remote Capability können Kommunikationspartner authentifiziert werden.

```text
Peer Authentication
      +
Valid Remote Capability
      ↓
Authorized Request
```

Authentication allein reicht nicht aus.

## Trust

Trust wird getrennt von Authority bewertet.

```text
Remote Capability
      +
Required Trust
      ↓
Eligible Remote Execution
```

Eine gültige Capability kann durch einen Execution Contract dennoch ausgeschlossen werden, wenn der Remote Provider die Trust Requirements nicht erfüllt.

## Sovereignty

Remote Authority darf Sovereignty Constraints nicht umgehen.

```text
Valid Capability
      +
Forbidden Sovereignty Domain
      ↓
Execution Rejected
```

Capability und Sovereignty müssen beide erfüllt sein.

## Revocation

Remote Capabilities müssen widerrufbar sein können.

Zustände:

```text
Valid
Revoked
Expired
Unknown
```

Dabei gilt:

```text
Unknown ≠ Valid
```

Für sicherheitskritische Operationen kann eine aktuelle Revocation-Prüfung erforderlich sein.

## Expiration

Remote Capabilities sollen zeitlich begrenzt werden können.

```text
Issued
  ↓
Valid
  ↓
Expires
  ↓
Invalid
```

Expiration reduziert langfristig bestehende Remote Authority.

## Capability Chains

Delegationen können Ketten bilden.

```text
Capability A
   ↓
Capability B
   ↓
Capability C
```

Für jede Stufe gilt:

```text
Authority(C) ⊆ Authority(B) ⊆ Authority(A)
```

NovaOS muss Delegation Depth begrenzen können.

## Reconnection

Eine neue Netzwerkverbindung macht eine alte Remote Capability nicht automatisch gültig.

Nach Reconnection können erforderlich sein:

```text
Peer Reauthentication
Capability Revalidation
Expiration Check
Revocation Check
Trust Revalidation
Sovereignty Revalidation
```

## Failure

Bei Kommunikationsverlust muss zwischen:

```text
Capability Invalid
```

und:

```text
Capability State Unknown
```

unterschieden werden.

Ein Netzwerkfehler widerruft nicht automatisch die Capability.

## Distributed Execution

Der Execution Planner kann minimale Remote Capabilities für einen Task ableiten.

```text
ExecutionContract
      ↓
Required Operations
      ↓
Required Authority
      ↓
Minimal Remote Capabilities
      ↓
Remote Task
```

Nach Abschluss werden temporäre Capabilities freigegeben oder widerrufen.

## Logging

Capability Secrets oder übertragbare Tokens dürfen nicht in:

```text
Logs
Crash Dumps
Telemetry
Trace Metadata
Error Messages
```

geschrieben werden.

Es dürfen ausschließlich sichere Referenzen oder CapabilityIDs verwendet werden, sofern die Policy dies erlaubt.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
CapabilityID
CapabilityTypeID
Target ObjectID
Authority Scope
Holder
Issuer
Delegation Chain
Expiration
Revocation State
Execution Binding
Task Binding
Trust Constraints
Sovereignty Constraints
```

Die introspektive Darstellung darf keine übertragbaren Capability Secrets offenlegen.

## Normative Anforderungen

1. NovaOS MUSS Remote Authority über explizite Capabilities modellieren.
2. Netzwerkverbindungen, Endpoints und Identitäten DÜRFEN keine implizite Authority erzeugen.
3. Remote Capability Delegation MUSS monoton attenuierend sein.
4. Delegierte Authority DARF die Authority der Quell-Capability NICHT überschreiten.
5. Capability Transfer MUSS durch das Capability-System kontrolliert werden.
6. Remote Capabilities SOLLEN an Execution, Task, Zeit oder Objektbereich bindbar sein.
7. Transportrepräsentationen MÜSSEN gegen Fälschung, Manipulation und Replay geschützt werden.
8. Remote Handles DÜRFEN NICHT als globale Capability-Identitäten behandelt werden.
9. Endpoint Migration DARF nicht automatisch neue Authority erzeugen.
10. Trust und Sovereignty MÜSSEN unabhängig von Capability Authority geprüft werden.
11. Remote Capabilities MÜSSEN Revocation und Expiration unterstützen können.
12. `Unknown` Revocation State DARF bei erforderlicher Validierung NICHT als `Valid` behandelt werden.
13. Reconnection MUSS Capability-Revalidation ermöglichen.
14. Capability Secrets DÜRFEN NICHT in Logs, Traces oder Crash Dumps erscheinen.
15. Distributed Execution SOLL nur die minimal erforderlichen Remote Capabilities delegieren.
16. Remote Capability State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-SCHEDULER-0001`
- `NPSPEC-DISTRIBUTED-IPC-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-IPC-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0063`

## Ergebnis

```text
Local Capability
      ↓
Explicit Delegation
      ↓
Attenuation
      ↓
Protected Remote Capability
      ↓
Distributed IPC
      ↓
Remote Validation
      ↓
Authorized Remote Operation
      ↓
Expiration / Revocation
```

NovaOS erhält damit ein verteiltes Capability-Modell, bei dem Authority kontrolliert über Systemgrenzen hinweg delegiert werden kann, ohne Netzwerkverbindungen, Identitäten, Endpoints oder Trust selbst zu Berechtigungen zu machen.