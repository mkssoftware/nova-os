# NPSPEC-CAPABILITY-TOKEN-0001 – Nova Capability Token

## Status

Angenommen

## Kategorie

Capability / Token / Security

## Zweck

NovaOS definiert Capability Tokens als geschützte, übertragbare Repräsentation einer konkreten Capability.

```text
Capability
     ↓
Capability Token
     ↓
Validation
     ↓
Rights + Constraints
     ↓
Authorized Operation
```

Ein Token transportiert Autorität. Seine bloße Identifikation oder Kenntnis darf jedoch nicht automatisch zur Nutzung ausreichen.

## Grundprinzipien

```text
Token ≠ Identity
Token ≠ CapabilityID
Token ≠ Principal Identity
Token Possession = Potential Authority
Token Validation ≠ Trust
Expired Token ≠ Valid Authority
Copied Token ≠ New Capability
Token Disclosure = Security Event
```

## Token-Modell

Ein Capability Token repräsentiert mindestens:

```text
CapabilityToken
├── CapabilityID
├── Target ObjectID
├── Rights
├── Constraints
├── State
└── Integrity Protection
```

Optional:

```text
Issuer Identity
Holder Binding
ParentCapabilityID
Security Domain
Issued Time
Expiration
Nonce
Delegation Depth
Trust Requirements
Location Constraints
Purpose
```

## Token-Typen

NovaOS kann unterschiedliche Repräsentationen unterstützen:

```text
Kernel Capability Handle
Protected Capability Object
Sealed Token
Signed Token
Remote Capability Token
Short-lived Delegation Token
```

Die konkrete Darstellung darf die semantische Capability nicht verändern.

## Unforgeability

Capability Tokens müssen gegen Fälschung und unautorisierte Veränderung geschützt sein.

Geeignete Mechanismen umfassen:

```text
Kernel-controlled Handles
Unpredictable References
MAC
Digital Signature
Sealing
Protected Object Tables
```

Anwendungen dürfen keine zusätzliche Autorität durch Manipulation eines Tokens erzeugen können.

## Holder Binding

Tokens können an einen bestimmten Security Context gebunden sein.

```text
Token
├── Workload Identity
├── Process Security Domain
├── Device Identity
└── Session Context
```

Ein gestohlener gebundener Token darf außerhalb seines gültigen Kontexts nicht automatisch verwendbar sein.

## Validierung

Vor Nutzung muss die Capability validiert werden.

```text
Token
  ↓
Integrity
  ↓
State
  ↓
Expiration
  ↓
Holder Binding
  ↓
Rights + Constraints
  ↓
Policy
```

Erst danach darf die angeforderte Operation ausgeführt werden.

## Attenuation

Bei Delegation dürfen eingeschränkte Tokens erzeugt werden.

```text
Token A
Rights: Read + Write
     ↓
Attenuation
     ↓
Token B
Rights: Read
```

Es gilt:

```text
Authority(Token B) ⊆ Authority(Token A)
```

Ein abgeleiteter Token erhält eine eigene `CapabilityID`, wenn er eine eigenständige Capability-Instanz repräsentiert.

## Übertragung

Token-Übertragung muss explizit erfolgen.

```text
Holder A
   ↓
Authorized Transfer
   ↓
Holder B
```

IPC-, RPC- oder Remote-Übertragung darf Tokens nicht versehentlich über Serialisierung, Logging oder normale Datenkopien offenlegen.

## Remote Tokens

Remote Capability Tokens benötigen zusätzliche Schutzmechanismen.

```text
Local Capability
      ↓
Controlled Export
      ↓
Remote Token
      ↓
Authenticated Channel
      ↓
Remote Validation
```

Remote Token ≠ Remote Trust.

Die empfangende Seite muss weiterhin lokale Security-, Trust- und Sovereignty-Policies anwenden.

## Ablauf

Tokens können zeitlich begrenzt werden.

```text
Issued
  ↓
Active
  ↓
Expired
```

Kurzlebige Tokens sind für temporäre Delegationen und Remote-Autorität zu bevorzugen.

## Revocation

Tokens müssen widerrufbar sein, sofern ihre Capability revokabel ist.

```text
CapabilityID
     ↓
Revoked
     ↓
All Representations Invalid
```

Mehrere Tokens derselben Capability dürfen nach deren Revocation keine gültige Autorität mehr darstellen.

## Speicherung

Capability Tokens müssen wie sicherheitskritische Autoritätsobjekte behandelt werden.

Sie dürfen insbesondere nicht ungeschützt erscheinen in:

```text
Logs
Crash Dumps
Telemetry
Clipboard
Normal Configuration
Command History
Debug Output
```

## Introspection

Autorisierte Introspection darf anzeigen:

```text
CapabilityID
Target
Rights
Constraints
Issuer
Holder Binding
Expiration
State
```

Der verwendbare Token selbst darf dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS Capability Tokens gegen Fälschung und Manipulation schützen.
2. Tokens MÜSSEN eindeutig einer Capability zugeordnet werden können.
3. Token-Validierung MUSS Rechte, Constraints und aktuellen State berücksichtigen.
4. Tokens MÜSSEN optional an einen Holder oder Security Context bindbar sein.
5. Delegierte Tokens DÜRFEN keine größere Autorität als ihre Quelle besitzen.
6. Token-Übertragung MUSS explizit und kontrolliert erfolgen.
7. Remote Tokens MÜSSEN über geschützte Kommunikationswege übertragen werden.
8. Revocation einer Capability MUSS ihre gültigen Token-Repräsentationen invalidieren können.
9. Capability Tokens DÜRFEN NICHT in Logs, Telemetrie oder ungeschützte Diagnosedaten gelangen.
10. Introspection DARF verwendbare Capability Tokens NICHT offenlegen.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-SECURITY-DELEGATION-0001`
- `NPSPEC-SECURITY-REVOCATION-0001`
- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-DISTCOMM-REMOTECAPABILITY-0001`
- `NPSPEC-CRYPTO-ARCH-0001`
- `NPSPEC-IDENTITY-WORKLOAD-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `ADR-CAPABILITY-0002`

## Ergebnis

```text
Capability Identity
       ↓
Protected Token
       ↓
Explicit Transfer
       ↓
Validation
       ↓
Rights + Constraints
       ↓
Authorized Operation
```

NovaOS erhält damit eine sichere Token-Repräsentation für Capabilities, mit der Autorität lokal und remote kontrolliert übertragen, eingeschränkt, zeitlich begrenzt und widerrufen werden kann.