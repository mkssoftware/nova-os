# NPSPEC-IDENTITY-FEDERATION-0001 – Nova Identity Federation

## Status

Angenommen

## Kategorie

Identity / Federation / Trust

## Zweck

NovaOS definiert Identity Federation als kontrollierte Nutzung von Identitäten externer oder anderer vertrauenswürdiger Identity Domains, ohne diese Identitäten lokal neu verwalten zu müssen.

```text
External Identity
       ↓
Identity Provider
       ↓
Federation Trust
       ↓
Nova Identity Mapping
       ↓
Local Security Context
```

Federation überträgt Identitätsaussagen, jedoch nicht automatisch Autorität.

## Grundprinzipien

```text
Federated Identity ≠ Local Identity
Federation ≠ Automatic Trust
Authentication ≠ Authorization
External Role ≠ Local Capability
Identity Provider ≠ Security Authority
Valid Assertion ≠ Allowed Access
```

## Federation-Modell

Eine Federation-Beziehung besitzt mindestens:

```text
FederationRelationship
├── FederationID
├── Identity Provider
├── Trust Policy
├── Identity Mapping
└── State
```

Optional:

```text
Allowed Identity Types
Allowed Claims
Security Domain
Authentication Requirements
Attestation Requirements
Sovereignty Policy
Expiration
Revocation Policy
```

## Identity Provider

NovaOS muss unterschiedliche Identity Provider integrieren können.

```text
Organization
Remote NovaOS System
Enterprise Identity Provider
Cloud Identity Provider
Federated Service
```

Provider werden nicht allein aufgrund technischer Erreichbarkeit vertraut.

## Trust Establishment

Vor Annahme einer externen Identität muss eine explizite Trust-Beziehung bestehen.

```text
Provider Identity
      +
Cryptographic Verification
      +
Trust Policy
      ↓
Accepted Federation
```

Trust kann auf bestimmte Identitätstypen, Claims, Dienste oder Security Domains begrenzt werden.

## Identity Mapping

Externe Identitäten müssen kontrolliert auf NovaOS-Identitäten abgebildet werden.

```text
External Subject
      ↓
Federation Mapping
      ↓
Nova IdentityID
```

Die externe Kennung darf nicht unkontrolliert zur internen stabilen Identität werden.

Mappings müssen eindeutig und nachvollziehbar sein.

## Claims

Federierte Provider können Claims liefern:

```text
Identity
Organization
Group
Role
Authentication Strength
Device Information
Attributes
```

Claims sind Eingaben für Policies.

```text
Claim ≠ Capability
```

Ein externer Administrator- oder Rollen-Claim darf daher nicht automatisch entsprechende NovaOS-Autorität erzeugen.

## Autorisierung

Nach erfolgreicher Federation erfolgt eine eigenständige lokale Autorisierungsentscheidung.

```text
Federated Identity
       ↓
Verified Claims
       ↓
Nova Security Policy
       ↓
Capabilities
```

NovaOS behält damit die Kontrolle über tatsächlich gewährte Rechte.

## Föderationskette

Mehrstufige Federation muss kontrollierbar sein.

```text
Provider A
   ↓
Provider B
   ↓
NovaOS
```

NovaOS muss entscheiden können, ob indirekte Trust-Beziehungen akzeptiert werden.

```text
Trust(A → B) + Trust(B → Nova)
          ≠
Automatic Trust(A → Nova)
```

## Lebensdauer und Revocation

Federierte Identitätsinformationen müssen zeitlich begrenzbar sein.

```text
Issued
 ↓
Valid
 ↓
Expired / Revoked
```

NovaOS muss Provider-Revocation und lokale Revocation getrennt behandeln können.

Eine lokal gesperrte federierte Identität darf nicht allein durch erneute externe Authentifizierung wieder aktiviert werden.

## Offline-Verhalten

Ausfall eines externen Identity Providers muss explizit behandelt werden.

Mögliche Policies:

```text
Deny
Use Valid Cached Assertion
Restricted Offline Access
Require Local Identity
```

```text
Provider Unreachable ≠ Identity Valid
```

## Privacy

Federation soll nur notwendige Identitätsattribute übertragen.

```text
Requested Claims
       ↓
Data Minimization
       ↓
Required Claims Only
```

Selective Disclosure und pseudonyme Identitäten sollen unterstützt werden, sofern Provider und Protokoll dies ermöglichen.

## Sovereignty

Federierte Identitätsdaten müssen Privacy- und Data-Sovereignty-Regeln berücksichtigen.

Identity Federation darf keine unzulässige Übertragung personenbezogener Daten erzwingen.

## Introspection

NovaOS soll mindestens darstellen können:

```text
FederationID
Provider
External Identity Reference
Nova IdentityID
Trust State
Accepted Claims
Expiration
State
```

Credentials, Tokens und geheimes Schlüsselmaterial dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS föderierte Identitäten unterstützen können.
2. Federation MUSS auf expliziten Trust Policies basieren.
3. Externe Identität DARF NICHT automatisch lokale Autorität erzeugen.
4. Externe Claims DÜRFEN NICHT automatisch als Capabilities behandelt werden.
5. Identity Mapping MUSS eindeutig und nachvollziehbar sein.
6. Föderationsketten MÜSSEN explizit kontrollierbar sein.
7. Provider- und lokale Revocation MÜSSEN berücksichtigt werden.
8. Federation MUSS Data Minimization und Privacy Policies unterstützen.
9. Provider-Ausfälle MÜSSEN über definierte Offline-Policies behandelt werden.
10. Federation-Zustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-IDENTITY-DEVICE-0001`
- `NPSPEC-IDENTITY-SERVICE-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-ABAC-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-DATASOVEREIGNTY-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-PRIVACY-SELECTIVEDISCLOSURE-0001`
- `NPSPEC-PRIVACY-SOVEREIGNTY-0001`
- `ADR-IDENTITY-0007`

## Ergebnis

```text
External Identity Provider
          ↓
Authentication + Trust
          ↓
Controlled Identity Mapping
          ↓
Nova Identity
          ↓
Local Policy + Capabilities
          ↓
Authorized Operation
```

NovaOS erhält damit eine kontrollierte Identity-Federation-Architektur, die externe Identitäten integrieren kann, ohne externe Identitätsprovider automatisch zu Autorisierungsinstanzen für NovaOS zu machen.