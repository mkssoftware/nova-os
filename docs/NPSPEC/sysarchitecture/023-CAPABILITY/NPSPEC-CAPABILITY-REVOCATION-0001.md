# NPSPEC-CAPABILITY-REVOCATION-0001 – Nova Capability Revocation

## Status

Angenommen

## Kategorie

Capability / Revocation / Security

## Zweck

NovaOS definiert Capability Revocation als kontrollierten Entzug bereits erteilter Autorität.

```text
Capability
    ↓
Revocation
    ↓
Authority Invalid
```

Revocation muss auch bestehende Handles, Tokens, Delegationen und abgeleitete Capabilities berücksichtigen.

## Grundprinzipien

```text
Revocation ≠ Deletion
Revocation ≠ Expiration
Revocation ≠ Attenuation
Revocation ≠ Identity Revocation
Revoked Capability ≠ Unknown Capability
Revoked Authority ≠ Recoverable by Copy
Unknown Revocation State ≠ Valid
```

## Zustände

Capabilities unterstützen mindestens:

```text
Valid
Revoked
Expired
Unknown
```

Dabei gilt:

```text
Valid   → Autorität darf geprüft werden
Revoked → Autorität verweigern
Expired → Autorität verweigern
Unknown → nicht als gültig behandeln
```

## Revocation-Modell

Eine Revocation referenziert mindestens:

```text
CapabilityRevocation
├── CapabilityID
├── RevocationID
├── State
├── Timestamp
└── Reason
```

Optional:

```text
Issuer
Revoker Identity
Delegation Scope
Affected Descendants
Policy Reference
TransactionID
Audit Reference
```

Die Capability Identity bleibt für Audit und Provenance erhalten.

## Direkte Revocation

Eine einzelne Capability kann gezielt widerrufen werden.

```text
Capability A
    ↓
Revoked
```

Alle Handles und Tokens, die diese Capability repräsentieren, müssen dadurch unwirksam werden.

## Delegationsbaum

Capabilities können Ableitungen besitzen:

```text
A
├── B
│   ├── D
│   └── E
└── C
```

Revocation muss unterschiedliche Scopes unterstützen:

```text
Single Capability
Delegation Branch
All Descendants
Entire Capability Family
```

Die konkrete Wirkung wird durch die Revocation Policy bestimmt.

## Abgeleitete Capabilities

Wird eine Quelle widerrufen, müssen abhängige Capabilities neu bewertet werden.

```text
Source Revoked
      ↓
Dependency Resolution
      ↓
Derived Capabilities
      ↓
Re-Evaluation
```

Eine Ableitung darf Revocation ihrer erforderlichen Autoritätsquelle nicht umgehen.

## Tokens und Handles

```text
CapabilityID
├── Handle A
├── Handle B
├── Token A
└── Token B
```

Revocation gilt für die Autorität hinter den Repräsentationen.

```text
Revoked Capability
      ↓
Handle Resolution → Deny
Token Validation  → Deny
```

Eine Kopie eines Tokens darf Revocation nicht umgehen.

## Composite Capabilities

Bei zusammengesetzten Capabilities:

```text
Capability A ─┐
Capability B ─┼→ Composite X
Capability C ─┘
```

führt die Revocation einer erforderlichen Komponente zur Neubewertung von `Composite X`.

Das Ergebnis kann sein:

```text
Valid
Restricted
Revoked
```

abhängig davon, ob die betreffende Komponente zwingend benötigt wird.

## Laufende Operationen

Revocation muss definierte Auswirkungen auf laufende Operationen besitzen.

```text
Revocation
    ↓
Active Operation
    ↓
Policy
├── Continue
├── Cancel
├── Restrict
└── Rollback
```

Sicherheitskritische Operationen sollen bei Autoritätsverlust abbrechbar sein.

Transaktionale Operationen sollen einen sicheren Rollback oder definierten Fehlerzustand verwenden.

## Verteilte Revocation

Remote Capabilities benötigen propagierbare Revocation.

```text
Authority Source
      ↓
Revocation
      ↓
Remote Holders
      ↓
Revalidation
```

Caches müssen zeitlich oder ereignisbasiert invalidierbar sein.

Bei nicht überprüfbarem Revocation-Status gilt:

```text
Unknown ≠ Valid
```

Die konkrete Behandlung erfolgt gemäß Security- und Availability-Policy.

## Revocation und Recovery

Eine widerrufene Capability darf nicht einfach wieder aktiviert werden.

Erneute Autorität erfordert:

```text
New Authorization
      ↓
New Capability
      ↓
New CapabilityID
```

Die alte Capability bleibt historisch `Revoked`.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
CapabilityID
Current State
RevocationID
Reason
Timestamp
Affected Descendants
Dependency State
```

## Audit

Revocation-Ereignisse sollen mindestens nachvollziehbar machen:

```text
Who
revoked which Capability
when
why
with which Scope
```

Geheime Capability Tokens dürfen nicht protokolliert werden.

## Normative Anforderungen

1. NovaOS MUSS Capability Revocation unterstützen.
2. Revocation MUSS bestehende Token- und Handle-Repräsentationen unwirksam machen können.
3. Revocation DARF NICHT durch Kopieren einer Capability-Repräsentation umgangen werden.
4. Delegationsbäume MÜSSEN gezielt oder rekursiv widerrufbar sein.
5. Abhängige Capabilities MÜSSEN nach Revocation neu bewertet werden.
6. Composite Capabilities MÜSSEN den Verlust erforderlicher Komponenten berücksichtigen.
7. Laufende Operationen MÜSSEN auf Autoritätsverlust reagieren können.
8. Remote Revocation MUSS propagierbar und revalidierbar sein.
9. `Unknown` DARF NICHT als `Valid` interpretiert werden.
10. Wiederhergestellte Autorität MUSS als neue Capability autorisiert werden.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-TOKEN-0001`
- `NPSPEC-CAPABILITY-HANDLE-0001`
- `NPSPEC-CAPABILITY-COMPOSITION-0001`
- `NPSPEC-CAPABILITY-DELEGATION-0001`
- `NPSPEC-CAPABILITY-ATTENUATION-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-CAPABILITY-0009`

## Ergebnis

```text
Capability Authority
       ↓
Revocation
       ↓
Handles + Tokens + Delegations
       ↓
Dependency Re-Evaluation
       ↓
Authority Denied / Restricted
```

NovaOS erhält damit einen systemweiten Revocation-Mechanismus, durch den bereits erteilte Capability-Autorität zuverlässig entzogen und über lokale, abgeleitete und entfernte Repräsentationen hinweg neu bewertet werden kann.