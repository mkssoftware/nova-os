# ADR-IDENTITY-0010 – Step-Up Authentication

## Status

Angenommen

## Kategorie

Identity Architecture / Authentication / Security Context

## Kontext

Eine einmal erfolgreich authentisierte Identity darf nicht automatisch für jede spätere Operation als ausreichend stark authentisiert gelten.

Besonders sensible Operationen können einen aktuelleren oder stärkeren Identitätsnachweis erfordern.

NovaOS benötigt deshalb einen Mechanismus, mit dem die erforderliche Authentication Strength gezielt erhöht werden kann.

## Entscheidung

NovaOS führt **Step-Up Authentication als natives, policygesteuertes Identity-Verfahren** ein.

```text
Authenticated Identity
        ↓
Requested Operation
        ↓
Authentication Policy
        ↓
Sufficient? ── Yes → Continue
        │
        No
        ↓
Step-Up Authentication
        ↓
Continue
```

Die bestehende Identity bleibt erhalten. Lediglich ihr für den jeweiligen Kontext bestätigtes Authentication Level wird erhöht.

## Grundprinzip

```text
Authentication is contextual.

Authenticated once
≠
Authenticated strongly enough for everything
```

## Auslöser

Step-Up Authentication kann insbesondere erforderlich werden durch:

- sensible Systemoperationen,
- erhöhte Capability-Anforderungen,
- Security Policies,
- Zugriff auf geschützte Daten,
- Credential- oder Identity-Änderungen,
- administrative Aktionen,
- veränderten Trust Context.

## Authentication Strength

NovaOS muss unterschiedliche Authentication Strength Levels ausdrücken können.

Diese können unter anderem von folgenden Eigenschaften abhängen:

```text
Credential Type
Number of Independent Factors
Hardware Protection
Credential Freshness
Trust Context
```

Die konkrete Bewertung wird in NPSPECs definiert.

## Scope und Lifetime

Eine erfolgreiche Step-Up Authentication gilt nicht automatisch unbegrenzt.

Sie kann gebunden sein an:

```text
Operation
Task
Session
Capability
Execution Context
Time Window
```

Nach Ablauf des Scopes oder Lifetimes gilt wieder das vorherige Authentication Level.

## Verhältnis zu Authority

Step-Up Authentication erzeugt keine zusätzliche Authority.

```text
Authentication Strength ≠ Authority
```

Eine Operation benötigt weiterhin die erforderlichen Capabilities und muss alle relevanten Policies erfüllen.

Step-Up Authentication bestätigt lediglich, dass die Identity mit der erforderlichen Stärke authentisiert wurde.

## Delegated Context

Delegierte Authority darf Step-Up-Anforderungen nicht umgehen.

Falls eine Operation eine stärkere Authentication der ursprünglichen Identity verlangt, muss diese Anforderung entlang des Delegated Context erhalten bleiben.

## Adaptive Systeme

Adaptive oder risikobasierte Systeme dürfen zusätzliche Step-Up Authentication verlangen.

Sie dürfen jedoch niemals eine durch harte Security Policy vorgeschriebene Authentication Strength reduzieren.

## Normative Anforderungen

1. NovaOS MUSS Step-Up Authentication als natives Identity-Verfahren unterstützen.
2. Step-Up Authentication MUSS policygesteuert ausgelöst werden können.
3. Die bestehende Identity DARF durch Step-Up Authentication nicht ersetzt werden.
4. NovaOS MUSS unterschiedliche Authentication Strength Levels ausdrücken können.
5. Authentication Strength MUSS Credential-, Factor-, Trust- und Freshness-Eigenschaften berücksichtigen können.
6. Step-Up Authentication MUSS auf einen definierten Scope und Lifetime begrenzbar sein.
7. Eine erfolgreiche Step-Up Authentication DARF keine automatische Authority erzeugen.
8. Capability- und Policy-Prüfungen MÜSSEN weiterhin unabhängig erfolgen.
9. Delegation DARF Step-Up-Anforderungen nicht umgehen.
10. Adaptive Systeme DÜRFEN stärkere Authentication verlangen, aber harte Anforderungen NICHT abschwächen.
11. Step-Up Authentication MUSS lokale und federierte Identities unterstützen können.
12. Sicherheitsrelevante Step-Up-Ereignisse MÜSSEN kontrolliert auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- normale Nutzung benötigt nicht dauerhaft maximale Authentication Strength,
- sensible Operationen können gezielt stärker geschützt werden,
- Passkeys und MFA können kontextabhängig eingesetzt werden,
- delegierte und verteilte Operationen können Authentication Requirements erhalten.

### Negative Konsequenzen

- Authentication Context muss über seinen Lifecycle verwaltet werden,
- zusätzliche Authentisierung kann sensible Workflows unterbrechen.

## Verworfene Alternativen

### Einmalige Authentication für die gesamte Session

Verworfen, da unterschiedliche Operationen unterschiedliche Sicherheitsanforderungen besitzen.

### Maximale MFA für jede Operation

Verworfen, da dies unnötige Benutzerinteraktion erzeugen würde.

### Step-Up erzeugt zusätzliche Berechtigungen

Verworfen, da Authentication und Authorization getrennt bleiben müssen.

## Abhängigkeiten

- `ADR-IDENTITY-0001_Identity_getrennt_von_Display_Name_Credential_Process_und_Address`
- `ADR-IDENTITY-0008_Delegated_Context_statt_Impersonation`
- `ADR-IDENTITY-0009_Passkeys_und_MFA`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0003_Least_Privilege_und_Privilege_Separation`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-STEP-UP-AUTHENTICATION-0001`
- `NPSPEC-IDENTITY-AUTHENTICATION-STRENGTH-0001`
- `NPSPEC-IDENTITY-AUTHENTICATION-CONTEXT-0001`
- `NPSPEC-IDENTITY-STEP-UP-POLICY-0001`
- `NPSPEC-IDENTITY-STEP-UP-TEST-0001`

## Ergebnis

NovaOS behandelt Authentication Strength als kontextabhängige Eigenschaft:

```text
Identity
   ↓
Current Authentication Context
   ↓
Operation Requirement
   ↓
Optional Step-Up
   ↓
Authorization
```

Damit kann NovaOS für sensible Operationen gezielt stärkere Authentisierung verlangen, ohne Identity, Authentication und Authority miteinander zu vermischen.