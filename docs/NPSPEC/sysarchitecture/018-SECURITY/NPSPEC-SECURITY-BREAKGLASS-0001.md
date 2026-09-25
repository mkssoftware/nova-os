# NPSPEC-SECURITY-BREAKGLASS-0001 – Nova Break-Glass Access

## Status

Angenommen

## Kategorie

Security / Emergency Access / Break-Glass

## Zweck

NovaOS definiert Break-Glass als kontrollierten Notfallmechanismus für außergewöhnliche Situationen, in denen reguläre Autorisierungswege nicht ausreichen.

```text
Emergency
   ↓
Break-Glass Request
   ↓
Strong Verification
   ↓
Temporary Authority
```

Break-Glass ist kein universeller Administrator- oder Root-Zugang.

## Grundprinzipien

```text
Emergency Access ≠ Root
Break-Glass ≠ Security Bypass
Temporary Authority ≠ Permanent Authority
Emergency ≠ Unlimited Permission
Recovery Need ≠ Automatic Authorization
```

## Break-Glass-Modell

Ein Break-Glass-Vorgang besitzt mindestens:

```text
BreakGlassSession
├── SessionID
├── Requester
├── Reason
├── Granted Authority
├── Expiration
└── State
```

Optional:

```text
Approval
Security Context
Target Resources
IncidentID
Audit Context
```

## Aktivierung

Break-Glass muss explizit aktiviert werden.

```text
Request
   ↓
Authenticate
   ↓
Policy Check
   ↓
Optional Approval
   ↓
Temporary Capability
```

Die Aktivierung darf zusätzliche Faktoren verlangen:

```text
MFA
Recovery Key
Physical Presence
Second Person Approval
Hardware Token
```

## Begrenzung

Notfallautorität muss möglichst eng begrenzt sein:

```text
Specific Resources
Specific Operations
Short Lifetime
Defined Purpose
```

Beispiel:

```text
Repair Boot Configuration
```

darf nicht automatisch Zugriff auf sämtliche Benutzerdaten erzeugen.

## Temporäre Capabilities

Break-Glass soll bevorzugt temporäre Capabilities erzeugen.

```text
Emergency Policy
      ↓
Temporary Capability
      ↓
Required Operation
      ↓
Revoke
```

Die normale Capability-Architektur bleibt damit erhalten.

## MAC und Isolation

Break-Glass darf Sicherheitsgrenzen nur dort erweitern, wo dies explizit vorgesehen ist.

Ein Break-Glass-Vorgang darf nicht automatisch:

```text
All MAC Rules Disable
All Sandboxes Disable
All Isolation Disable
```

Stattdessen werden gezielt benötigte Ausnahmen autorisiert.

## Ablauf und Widerruf

Break-Glass-Autorität muss:

```text
Expire
Revoke
Terminate on Session End
```

unterstützen.

Nach Ende des Notfalls müssen temporäre Rechte entfernt werden.

## Recovery

Recovery und NovaDOS dürfen Break-Glass verwenden, wenn geschützte Reparaturmaßnahmen erforderlich sind.

```text
Recovery
   ↓
Break-Glass
   ↓
Authorized Repair
```

Recovery Mode allein erzeugt keine unbegrenzte Autorität.

## Audit

Jede Break-Glass-Nutzung muss besonders auditierbar sein.

Mindestens:

```text
Requester
Reason
Granted Authority
Target
Start
End
Actions
Result
```

Break-Glass-Auditdaten dürfen vom Benutzer der Notfallautorität nicht unbemerkt entfernt werden.

## Fail-Safe

Kann die erforderliche Notfallautorisierung nicht ausreichend validiert werden, muss die Policy bestimmen, ob die Operation verweigert oder ein speziell abgesicherter Offline-Recovery-Pfad verwendet wird.

```text
Unknown Authorization ≠ Automatic Access
```

## Normative Anforderungen

1. NovaOS MUSS Break-Glass als kontrollierten Notfallmechanismus unterstützen können.
2. Break-Glass DARF NICHT als universeller Root-Zugang implementiert werden.
3. Notfallautorität MUSS explizit aktiviert werden.
4. Gewährte Rechte MÜSSEN auf das notwendige Minimum begrenzbar sein.
5. Break-Glass-Rechte MÜSSEN zeitlich begrenzbar und widerrufbar sein.
6. Temporäre Capabilities SOLLEN für Notfallautorität verwendet werden.
7. MAC- und Isolationsregeln DÜRFEN NICHT pauschal deaktiviert werden.
8. Recovery Mode DARF NICHT automatisch unbegrenzte Autorität erzeugen.
9. Break-Glass-Nutzung MUSS besonders auditierbar sein.
10. Nach Ende der Session MÜSSEN temporäre Rechte entzogen werden.

## Abhängigkeiten

- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-MAC-0001`
- `NPSPEC-SECURITY-LEASTPRIVILEGE-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-AUDIT-0001`
- `NPSPEC-BOOT-RECOVERY-0001`
- `ADR-SECURITY-0017`

## Ergebnis

```text
Emergency
   ↓
Verified Break-Glass
   ↓
Minimal Temporary Authority
   ↓
Repair
   ↓
Revoke + Audit
```

NovaOS erhält damit einen kontrollierten Notfallzugang, der notwendige Reparatur- und Recovery-Maßnahmen ermöglicht, ohne einen dauerhaften universellen Superuser einzuführen.