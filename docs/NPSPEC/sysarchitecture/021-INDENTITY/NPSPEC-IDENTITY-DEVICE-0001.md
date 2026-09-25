# NPSPEC-IDENTITY-DEVICE-0001 – Nova Device Identity

## Status

Angenommen

## Kategorie

Identity / Device Identity / Trust

## Zweck

NovaOS definiert ein einheitliches Identitätsmodell für physische und virtuelle Geräte.

```text
Device
  ↓
Device Identity
  ↓
Authentication + Attestation
  ↓
Trust Evaluation
  ↓
Security Policy
```

Geräteidentität beschreibt, welches Gerät beteiligt ist, erzeugt jedoch allein keine Autorität oder Vertrauenswürdigkeit.

## Grundprinzipien

```text
Device Identity ≠ Device Trust
Device Identity ≠ Authority
Hardware ID ≠ Stable Identity
Network Address ≠ Device Identity
Authenticated Device ≠ Trusted Device
```

## Identitätsmodell

Eine Geräteidentität besitzt mindestens:

```text
DeviceIdentity
├── IdentityID
├── Device Type
├── Provider
└── State
```

Optional:

```text
Hardware Identity
Device Model
Manufacturer
Security Domain
Trust State
Attestation Reference
Owner Reference
Credentials
Platform Information
```

Die `IdentityID` muss unabhängig von Netzwerkadresse, Gerätename oder physischem Anschluss stabil sein können.

## Gerätetypen

NovaOS muss unterschiedliche Geräteklassen unterstützen können:

```text
Computer
Mobile Device
Peripheral
Storage Device
Network Device
Embedded Device
Virtual Device
Security Hardware
```

Weitere Typen müssen erweiterbar sein.

## Hardwareidentität

Vorhandene Hardwaremerkmale dürfen zur Identifikation beitragen:

```text
TPM Identity
Secure Hardware Identity
Firmware Identity
Platform Identifier
Device Certificate
```

Ein einzelner Hardware-Identifier darf nicht ungeprüft als vollständiger Vertrauensnachweis gelten.

## Geräteauthentifizierung

Geräte müssen ihre Identität kryptografisch nachweisen können.

```text
Device Identity
      ↓
Credential / Key
      ↓
Challenge
      ↓
Cryptographic Proof
```

Private Geräteschlüssel sollen bevorzugt durch TPM, HSM oder vergleichbare Hardware geschützt werden.

## Attestation

Device Identity muss mit Attestation kombinierbar sein.

```text
Identity
   +
Platform State
   ↓
Attestation
   ↓
Trust Evaluation
```

Dadurch kann NovaOS zwischen der Identität eines Geräts und dessen aktuellem Sicherheitszustand unterscheiden.

## Trust

Vertrauen wird separat bewertet.

```text
Known Device ≠ Trusted Device
```

Trust Policies können berücksichtigen:

```text
Identity
Attestation
Code Integrity
Boot State
Security Domain
Provider
Device State
```

## Autorität

Ein authentifiziertes Gerät erhält nicht automatisch Zugriff.

```text
Device Identity
      ↓
Authentication
      ↓
Trust Evaluation
      ↓
Capabilities + Policies
```

Die tatsächliche Autorität wird durch die NovaOS-Sicherheitsarchitektur bestimmt.

## Gerätewechsel und Migration

Logische Identität und konkrete Hardware müssen getrennt behandelbar sein.

Bei Hardwareaustausch oder Migration muss NovaOS entscheiden können, ob:

```text
Identity Preserved
Identity Rebound
New Identity Created
```

wird.

Eine Hardwareänderung darf nicht unkontrolliert bestehende Geräteautorität übernehmen.

## Lebenszyklus

Geräteidentitäten unterstützen mindestens:

```text
Enroll
 ↓
Active
 ↓
Suspended
 ↓
Revoked
 ↓
Retired
```

Verlorene oder kompromittierte Geräte müssen widerrufbar sein.

## Privacy

Geräteidentitäten dürfen nicht unnötig als globale Tracking-Identifier offengelegt werden.

Wo möglich sollen verwendet werden:

```text
Scoped Device Identity
Pseudonymous Identifier
Service-specific Identity
```

## Introspection

NovaOS soll mindestens darstellen können:

```text
IdentityID
Device Type
Provider
State
Trust State
Attestation State
Security Domain
```

Geheime Geräteschlüssel dürfen nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Device-Identity-Modell bereitstellen.
2. Geräteidentität MUSS von Netzwerkadresse und Gerätename getrennt sein.
3. Geräteidentität DARF NICHT automatisch Vertrauen oder Autorität erzeugen.
4. Kryptografische Geräteauthentifizierung MUSS unterstützt werden können.
5. Hardwaregeschützte Geräteidentitäten SOLLEN unterstützt werden.
6. Device Identity MUSS mit Attestation integrierbar sein.
7. Hardwareänderungen DÜRFEN bestehende Autorität NICHT automatisch übernehmen.
8. Geräteidentitäten MÜSSEN widerrufbar sein.
9. Globale Tracking-Identifier SOLLEN vermieden werden.
10. Geräte- und Trust-Zustände SOLLEN sicher introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-IDENTITY-0001`
- `NPSPEC-IDENTITY-LOCAL-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `NPSPEC-CRYPTO-KEYSTORE-0001`
- `NPSPEC-BOOT-TPM-0001`
- `NPSPEC-BOOT-MEASURED-0001`
- `ADR-IDENTITY-0003`

## Ergebnis

```text
Device
  ↓
Stable Device Identity
  ↓
Authentication + Attestation
  ↓
Trust Evaluation
  ↓
Capabilities + Security Policies
```

NovaOS erhält damit ein einheitliches Geräteidentitätsmodell, das Geräteidentität, aktuellen Vertrauenszustand und tatsächliche Autorität konsequent voneinander trennt.