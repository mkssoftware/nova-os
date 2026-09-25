# NPSPEC-PRIVACY-EXPIRATION-0001 – Nova Data Expiration

## Status

Angenommen

## Kategorie

Privacy / Data Protection / Expiration

## Zweck

NovaOS definiert Data Expiration als systemweiten Mechanismus, durch den Daten nach einem festgelegten Zeitpunkt oder Ereignis automatisch ihren gültigen Nutzungszustand verlieren.

```text
Data
 ↓
Expiration Policy
 ↓
Valid
 ↓
Expired
 ↓
Restricted / Delete / Anonymize / Archive
```

Expiration bestimmt, **wann Daten nicht mehr regulär verwendet werden dürfen**. Die anschließende Behandlung wird durch Retention- und Löschrichtlinien bestimmt.

## Grundprinzipien

```text
Expired ≠ Deleted
Expired ≠ Physically Erased
Retention ≠ Expiration
Expired Data ≠ Normally Usable Data
Clock Change ≠ Extended Authorization
```

## Expiration-Modell

Ein Ablaufzustand besitzt mindestens:

```text
Expiration
├── TargetID
├── Expiration Condition
├── Expiration Action
└── State
```

Optional:

```text
Expiration Time
Purpose
Trigger Event
Grace Period
Security Domain
Retention Policy
Legal Hold
```

Mögliche Zustände:

```text
Active
Expiring
Expired
Held
Unknown
```

## Ablaufbedingungen

Expiration muss unterschiedliche Bedingungen unterstützen können:

```text
Absolute Time
Relative Lifetime
Purpose Completion
Session End
Account Removal
Transaction Completion
Security Event
Explicit Revocation
```

Beispiel:

```text
Created
  ↓
Valid for 24 Hours
  ↓
Expired
```

## Zugriff nach Ablauf

Abgelaufene Daten dürfen nicht automatisch weiterhin normal verwendet werden.

```text
Access Request
      ↓
Expiration Check
      ↓
Active  → Continue
Expired → Policy Action
```

Mögliche Aktionen:

```text
Deny Access
Read-Only
Delete
Secure Erase
Anonymize
Archive
Review
```

## Retention-Abgrenzung

Expiration und Retention sind getrennte Konzepte.

```text
Expiration → Darf das Datum noch regulär genutzt werden?
Retention  → Wie lange darf das Datum noch existieren?
```

Ein Objekt kann daher bereits abgelaufen sein, aber aufgrund einer definierten Retention Policy noch gespeichert bleiben.

## Grace Period

Policies dürfen eine kontrollierte Übergangszeit definieren.

```text
Active
 ↓
Expired
 ↓
Grace Period
 ↓
Final Action
```

Eine Grace Period darf nicht implizit entstehen.

## Legal Hold

Ein Legal Hold kann eine Löschung verhindern, muss aber nicht automatisch die reguläre Nutzung abgelaufener Daten wieder erlauben.

```text
Expired + Legal Hold
        ↓
Preserved
        ↓
Normal Use Still Restricted
```

## Verteilte Systeme

Expiration muss über Replikate und entfernte Systeme propagierbar sein.

```text
Expiration Event
      ↓
Replica A → Expired
Replica B → Expired
Replica C → Pending
```

`Unknown` darf nicht als `Active` interpretiert werden, wenn eine aktuelle Ablaufprüfung erforderlich ist.

## Zeitbasis

Sicherheitsrelevante Ablaufentscheidungen dürfen nicht allein von leicht manipulierbaren Anwendungszeitwerten abhängen.

NovaOS soll geeignete vertrauenswürdige Zeitquellen und monotone Zeitmessung verwenden können.

## Integration

Expiration muss integrierbar sein mit:

```text
Capabilities
Sessions
Temporary Permissions
Keys
Personal Data
Caches
Tokens
Remote Data
```

Expiration kann dabei Revocation oder Retention-Aktionen auslösen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
TargetID
Expiration Condition
Expiration Time
State
Grace Period
Expiration Action
Retention State
```

## Normative Anforderungen

1. NovaOS MUSS Data Expiration systemweit unterstützen.
2. Expiration MUSS von Retention und Löschung getrennt behandelt werden.
3. Ablaufbedingungen MÜSSEN zeit- oder ereignisbasiert definierbar sein.
4. Abgelaufene Daten DÜRFEN NICHT automatisch normal weiterverwendet werden.
5. Grace Periods MÜSSEN explizit definiert werden.
6. Legal Hold DARF Expiration NICHT automatisch aufheben.
7. Expiration MUSS auf verteilte Kopien propagierbar sein.
8. Sicherheitskritische Expiration MUSS geeignete vertrauenswürdige Zeitmechanismen verwenden können.
9. `Unknown` DARF bei erforderlicher aktueller Prüfung NICHT automatisch als `Active` gelten.
10. Expiration-Zustände SOLLEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-RETENTION-0001`
- `NPSPEC-CRYPTO-SECUREERASE-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `ADR-PRIVACY-0003`

## Ergebnis

```text
Active Data
    ↓
Expiration Condition
    ↓
Expired
    ↓
Usage Restricted
    ↓
Retention / Deletion Policy
```

NovaOS erhält damit einen einheitlichen Ablaufmechanismus, der klar zwischen dem Ende der zulässigen Nutzung und der tatsächlichen Aufbewahrung oder Löschung von Daten unterscheidet.