# NPSPEC-CAPABILITY-IDENTITY-0001 – Nova Capability Identity

## Status

Angenommen

## Kategorie

Capability / Identity / Security

## Zweck

NovaOS definiert die eindeutige Identität von Capabilities unabhängig von Besitzer, Speicherort, Handle oder Darstellung.

```text
Capability
├── CapabilityID
├── Target
├── Rights
├── Constraints
└── State
```

Eine Capability ist ein expliziter Autoritätsnachweis. Ihre Identität ermöglicht Revocation, Delegation, Audit und Introspection, ohne Capability Identity mit Principal Identity gleichzusetzen.

## Grundprinzipien

```text
Capability Identity ≠ Principal Identity
CapabilityID ≠ Authority
Handle ≠ Capability Identity
ObjectID ≠ CapabilityID
Copy of Handle ≠ New Authority
Capability Identity ≠ Location
Identity ≠ Ownership
```

## CapabilityID

Jede sicherheitsrelevante Capability muss eine stabile `CapabilityID` besitzen.

```text
CapabilityID
     ↓
Capability Object
├── Target ObjectID
├── Rights
├── Constraints
├── Issuer
└── State
```

Die `CapabilityID` identifiziert die konkrete Autoritätsinstanz.

Sie darf nicht allein zur Ausübung der Autorität ausreichen.

```text
Knowledge(CapabilityID) ≠ Possession(Capability)
```

## Capability und Principal

Capabilities können Principals zugeordnet oder an sie gebunden sein:

```text
User Identity
Service Identity
Application Identity
Workload Identity
Agent Identity
Device Identity
```

Die Identity beschreibt, wer handelt.

Die Capability beschreibt, was dieser Principal tun darf.

```text
Principal Identity
       +
Capability
       ↓
Authorized Operation
```

## Target Identity

Capabilities referenzieren Ziele über stabile Objektidentitäten.

```text
CapabilityID
     ↓
Target ObjectID
```

Das Ziel kann beispielsweise sein:

```text
File
NovaFile
Device
Service
Endpoint
Memory Object
Network Resource
System Capability
```

Pfad, Adresse oder Speicherort sind nicht Teil der grundlegenden Capability Identity.

## Instanzidentität

Werden Capabilities attenuiert oder neu delegiert, entstehen eigenständig identifizierbare Capability-Instanzen.

```text
Capability A
Rights: Read + Write
      ↓ Delegate
Capability B
Rights: Read
```

Dabei gilt:

```text
CapabilityID(A) ≠ CapabilityID(B)

Authority(B) ⊆ Authority(A)
```

Dadurch bleiben Delegationsketten nachvollziehbar.

## Ableitungsbeziehungen

Capabilities können Herkunftsbeziehungen besitzen:

```text
Capability A
├── Capability B
│   └── Capability D
└── Capability C
```

Optional:

```text
ParentCapabilityID
IssuerIdentity
IssuedToIdentity
DelegationID
CreationTime
Expiration
```

Diese Metadaten dürfen keine zusätzliche Autorität erzeugen.

## Lebenszyklus

Capability Identity bleibt während des Lebenszyklus stabil.

```text
Issued
  ↓
Active
  ↓
Restricted / Suspended
  ↓
Expired / Revoked
  ↓
Retired
```

Eine widerrufene Capability behält ihre `CapabilityID`, damit Audit und Provenance weiterhin möglich bleiben.

## Revocation

Revocation wird auf konkrete Capability-Identitäten angewendet.

```text
CapabilityID
     ↓
Revocation
     ↓
State = Revoked
```

Abgeleitete Capabilities müssen abhängig von Delegations- und Revocation-Policy ebenfalls invalidiert werden können.

## Location Transparency

Capability Identity ist unabhängig vom physischen Ort.

```text
CapabilityID
     ↓
Target ObjectID
     ↓
Local / Remote / Migrated
```

Migration eines Targets darf die Capability Identity nicht automatisch verändern.

Remote Capability-Nutzung erfordert weiterhin sichere Authority-Übertragung und lokale Policy-Prüfung.

## Introspection

Autorisierte Komponenten sollen mindestens abfragen können:

```text
CapabilityID
Target ObjectID
Rights
Constraints
State
Issuer
Holder / Binding
ParentCapabilityID
Expiration
```

Das Anzeigen einer Capability Identity darf nicht deren Nutzung ermöglichen.

## Audit

Sicherheitsrelevante Aktionen sollen auf die verwendete Capability referenzieren können.

```text
Audit Event
├── Principal Identity
├── CapabilityID
├── Target ObjectID
├── Operation
└── Result
```

Damit bleibt nachvollziehbar:

```text
Who
used which Authority
on which Object
for which Operation
```

## Normative Anforderungen

1. NovaOS MUSS sicherheitsrelevante Capability-Instanzen eindeutig identifizieren können.
2. `CapabilityID` MUSS von Principal Identity und Target ObjectID getrennt sein.
3. Kenntnis einer `CapabilityID` DARF NICHT zur Nutzung der Capability ausreichen.
4. Abgeleitete Capabilities MÜSSEN eigenständig identifizierbar sein.
5. Delegationsbeziehungen MÜSSEN nachvollziehbar sein können.
6. Capability Identity MUSS unabhängig von Speicherort und Handle-Darstellung sein.
7. Revocation MUSS konkrete Capability-Identitäten adressieren können.
8. Eine Revocation DARF die historische Capability Identity NICHT löschen.
9. Capability Identity MUSS in Audit und Introspection integrierbar sein.
10. Capability-Metadaten DÜRFEN keine zusätzliche Autorität erzeugen.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-TRUST-IDENTITY-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-CAPABILITY-0001`

## Ergebnis

```text
Principal Identity
       ↓
CapabilityID → Authority Instance
       ↓
Target ObjectID
       ↓
Rights + Constraints
       ↓
Authorized Operation
```

NovaOS erhält damit eine stabile Identitätsschicht für Capabilities, durch die Autoritätsinstanzen unabhängig von Principals, Objekten und Speicherorten eindeutig verfolgt, delegiert, widerrufen und auditiert werden können.