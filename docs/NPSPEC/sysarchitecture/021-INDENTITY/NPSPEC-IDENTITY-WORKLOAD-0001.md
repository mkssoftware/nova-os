# NPSPEC-IDENTITY-WORKLOAD-0001 – Nova Workload Identity

## Status

Angenommen

## Kategorie

Identity / Workload Identity / Security

## Zweck

NovaOS definiert Workload Identities für konkrete ausführende Instanzen wie Prozesse, Tasks, Container, isolierte Dienste oder verteilte Compute-Workloads.

```text
Workload
   ↓
Workload Identity
   ↓
Authentication + Context
   ↓
Capabilities + Policies
```

Eine Workload Identity beschreibt eine konkrete Ausführung und ist von Application Identity und Service Identity getrennt.

## Grundprinzipien

```text
Workload Identity ≠ Application Identity
Workload Identity ≠ Service Identity
Workload Identity ≠ Process ID
Workload Identity ≠ Authority
Workload Identity ≠ Location
Workload Restart ≠ Same Instance
```

## Identitätsmodell

Eine Workload Identity besitzt mindestens:

```text
WorkloadIdentity
├── IdentityID
├── WorkloadID
├── Parent Identity
└── State
```

Optional:

```text
Application Identity
Service Identity
Instance ID
Security Domain
Execution Context
Trust State
Attestation Reference
Node Identity
Creation Time
Expiration
```

`IdentityID` bezeichnet die sicherheitsrelevante Identität der Workload.

## Identitätshierarchie

Workloads müssen einer übergeordneten Identität zugeordnet werden können.

```text
Application Identity
        ↓
Workload Identity
        ↓
Process / Tasks
```

oder:

```text
Service Identity
      ↓
Workload Identity
      ↓
Service Instance
```

Die Workload darf dadurch keine Rechte erhalten, die ihre übergeordnete Autorität überschreiten.

## Instanzidentität

Jede sicherheitsrelevante Ausführungsinstanz soll eine eigene Workload Identity erhalten können.

```text
Service
├── Workload A
├── Workload B
└── Workload C
```

Ein Neustart kann eine neue Workload Identity erzeugen, während die übergeordnete Service- oder Application Identity erhalten bleibt.

## Kurzlebige Identitäten

Workload Identities sollen bevorzugt kurzlebig sein.

```text
Create
 ↓
Active
 ↓
Expire / Terminate
 ↓
Invalid
```

Langfristige statische Credentials sollen für dynamische Workloads vermieden werden.

## Authentifizierung

Workloads müssen ihre Identität gegenüber anderen Komponenten nachweisen können.

Geeignete Mechanismen können umfassen:

```text
Ephemeral Credentials
Capability-bound Identity
Cryptographic Credentials
Attestation
Authenticated IPC
```

Credentials sollen an die konkrete Workload und ihren Security Context gebunden sein.

## Autorität

Eine Workload Identity erzeugt keine automatische Autorität.

```text
Workload Identity
       ↓
Authentication
       ↓
Security Policy
       ↓
Capabilities
       ↓
Authorized Operation
```

Capabilities sollen nach dem Least-Privilege-Prinzip auf die konkrete Workload begrenzt werden.

## Delegation

Übergeordnete Anwendungen oder Services dürfen begrenzte Autorität an Workloads delegieren.

```text
Parent Authority
      ↓ attenuation
Workload Authority
```

Es muss gelten:

```text
Authority(Workload) ⊆ Authority(Parent)
```

## Isolation

Workload Identity muss mit Isolation und Sandboxing integrierbar sein.

```text
Workload Identity
       ↓
Security Domain
       ↓
Isolation
       ↓
Resource Access
```

Zwei Workloads derselben Anwendung dürfen unterschiedliche Sicherheitskontexte besitzen.

## Migration

Eine laufende Workload kann zwischen Compute-Knoten migriert werden.

```text
Node A
  ↓
Workload Identity
  ↓ migration
Node B
```

Die Identität darf nicht allein aufgrund des neuen Standorts geändert werden.

Der Zielknoten muss Identität, Trust State und Autorität erneut validieren.

## Attestation

Workload Identity muss mit Attestation kombinierbar sein.

```text
Workload Identity
        +
Code Identity
        +
Execution Environment
        ↓
Attestation
```

Dadurch kann überprüft werden, ob die erwartete Workload im erwarteten Sicherheitskontext ausgeführt wird.

## Revocation und Beendigung

Beim Beenden einer Workload müssen zugehörige kurzlebige Autoritäten widerrufen oder ungültig werden können.

Dies betrifft insbesondere:

```text
Capabilities
Sessions
Tokens
Credentials
Delegations
Shared Resources
```

## Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
WorkloadID
Parent Identity
State
Security Domain
Trust State
Execution Location
Expiration
```

Geheime Credentials dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Workload Identities für konkrete Ausführungsinstanzen unterstützen.
2. Workload Identity MUSS von Application-, Service- und Process-Identity getrennt sein.
3. Workload Identity DARF NICHT von Prozess-ID oder Standort abhängen.
4. Workloads SOLLEN kurzlebige Credentials verwenden.
5. Workload Identity DARF NICHT automatisch Autorität erzeugen.
6. Delegierte Workload-Autorität DARF die Parent Authority NICHT überschreiten.
7. Workload Identity MUSS mit Isolation und Sandboxing integrierbar sein.
8. Migration MUSS eine erneute Validierung von Trust und Autorität ermöglichen.
9. Beendete Workloads DÜRFEN keine weiterhin gültige instanzgebundene Autorität behalten.
10. Workload-Zustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-APPLICATION-0001`
- `NPSPEC-IDENTITY-SERVICE-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-ISOLATION-0001`
- `NPSPEC-SECURITY-SANDBOX-0001`
- `NPSPEC-PROCESS-MIGRATION-0001`
- `ADR-IDENTITY-0006`

## Ergebnis

```text
Application / Service Identity
            ↓
      Workload Identity
            ↓
Authentication + Attestation
            ↓
Restricted Capabilities
            ↓
     Isolated Execution
```

NovaOS erhält damit kurzlebige, instanzbezogene Workload Identities, die konkrete Ausführungen eindeutig identifizieren und gleichzeitig von Anwendung, Service, Prozess-ID, Standort und tatsächlicher Autorität getrennt bleiben.