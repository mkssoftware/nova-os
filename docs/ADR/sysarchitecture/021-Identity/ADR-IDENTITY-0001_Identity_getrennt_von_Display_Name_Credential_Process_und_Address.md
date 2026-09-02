# ADR-IDENTITY-0001 – Identity getrennt von Display Name, Credential, Process und Address

## Status

Angenommen

## Kategorie

Identity Architecture / System Identity

## Kontext

Traditionelle Systeme vermischen Identität häufig mit technischen oder sichtbaren Repräsentationen wie Benutzername, Prozess-ID, Netzwerkadresse oder Credential.

Diese Eigenschaften können sich jedoch ändern, ohne dass sich die zugrunde liegende Identität ändert.

Beispiele:

```text
Display Name → kann geändert werden
Credential   → kann rotiert werden
Process      → kann neu gestartet werden
Address      → kann wechseln
Identity     → bleibt logisch dieselbe
```

NovaOS benötigt deshalb ein stabiles Identitätsmodell, das nicht von solchen veränderlichen Eigenschaften abhängt.

## Entscheidung

NovaOS trennt **Identity strikt von Display Name, Credential, Process und Address**.

```text
Identity
 ├── Display Name
 ├── Credentials
 ├── Processes / Execution Domains
 └── Addresses / Endpoints
```

Die Identity ist die stabile logische Referenz auf ein Subjekt oder Objekt.

Alle anderen Eigenschaften werden dieser Identity zugeordnet, definieren sie jedoch nicht.

## Grundprinzip

```text
Identity = Who or what is this?

Display Name = How is it shown?

Credential = How is identity proven?

Process = Where is code currently executing?

Address = Where is something currently reachable?
```

Diese Konzepte dürfen nicht gegenseitig als Ersatz verwendet werden.

## Stabile Identity

Eine Identity muss unabhängig von kurzlebigen Systemzuständen bestehen können.

```text
Process 4711
     ↓ restart
Process 9328

Identity remains unchanged
```

Dasselbe gilt für wechselnde Geräte, Netzwerkadressen, Credentials oder Anzeigenamen.

## Display Name

Display Names sind menschenlesbare Bezeichnungen.

Mehrere Identitäten können denselben Display Name besitzen und eine Identity kann ihren Display Name ändern.

```text
Display Name ≠ Identity
```

Sicherheitsentscheidungen dürfen deshalb nicht allein auf Display Names basieren.

## Credentials

Credentials dienen dem Nachweis oder der Authentisierung einer Identity.

Beispiele:

```text
Password
Cryptographic Key
Certificate
Hardware Credential
Passkey
Token
```

Eine Identity kann mehrere Credentials besitzen.

Credentials können hinzugefügt, rotiert oder widerrufen werden, ohne eine neue Identity erzeugen zu müssen.

```text
Credential proves Identity.

Credential is not Identity.
```

## Process und Execution Domain

Prozesse und Execution Domains können im Namen einer Identity handeln, sind aber nicht selbst automatisch diese Identity.

Die Zuordnung muss explizit und systemverifizierbar sein.

Ein Neustart oder eine Migration darf die logische Service- oder Application-Identity nicht unnötig verändern.

## Address und Endpoint

Netzwerkadressen und Endpoints beschreiben Erreichbarkeit.

```text
Identity
   ↓
Service Resolution
   ↓
Address / Endpoint
```

Eine Identity kann gleichzeitig mehrere Adressen besitzen oder ihre Adresse wechseln.

```text
Address ≠ Identity
```

Dies entspricht der NovaOS-Netzwerkarchitektur:

```text
Identity → Address → Route
```

## Identity und Authority

Identity erzeugt keine automatische Authority.

```text
Identity
   ≠
Capability
```

Eine erfolgreich authentisierte Identity benötigt weiterhin die erforderlichen Capabilities und Policy-Freigaben für eine Operation.

Ebenso bedeutet Trust in eine Identity nicht automatisch Authority.

## Identity Binding

Veränderliche Eigenschaften werden explizit an eine Identity gebunden.

Konzeptionell:

```text
Identity
 ├── Display Name Binding
 ├── Credential Binding
 ├── Execution Binding
 └── Address Binding
```

Die konkreten Binding-, Validierungs- und Lifecycle-Verfahren werden in NPSPECs definiert.

## Normative Anforderungen

1. NovaOS MUSS Identity als eigenständiges stabiles Systemkonzept definieren.
2. Identity MUSS von Display Name, Credential, Process und Address getrennt sein.
3. Display Names DÜRFEN NICHT als eindeutige Sicherheitsidentität vorausgesetzt werden.
4. Eine Identity MUSS mehrere Credentials besitzen können.
5. Credential-Rotation DARF die zugrunde liegende Identity nicht automatisch ändern.
6. Prozesse und Execution Domains MÜSSEN explizit an relevante Identities bindbar sein.
7. Prozessneustarts DÜRFEN stabile Service- oder Application-Identities nicht unnötig verändern.
8. Netzwerkadressen und Endpoints DÜRFEN NICHT als vertrauenswürdige Identity behandelt werden.
9. Eine Identity MUSS mehreren Adressen oder Endpoints zugeordnet werden können.
10. Identity DARF keine automatische Capability oder Authority erzeugen.
11. Identity Bindings MÜSSEN systemverifizierbar und lifecyclefähig sein.
12. Relevante Identity-Beziehungen MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- stabile Identitäten trotz Prozess-, Credential- oder Adresswechsel,
- saubere Trennung von Authentication und Authorization,
- bessere Unterstützung für Service Migration und verteilte Systeme,
- Credential-Rotation ohne Identitätswechsel,
- Display Names können frei geändert werden.

### Negative Konsequenzen

- zusätzliche Identity-Bindings müssen verwaltet werden,
- Legacy-Systeme benötigen Mapping auf das native Identity-Modell.

## Verworfene Alternativen

### Benutzername als Identity

Verworfen, da Anzeigenamen veränderlich und nicht zwingend eindeutig sind.

### Credential als Identity

Verworfen, da Credentials rotierbar und widerrufbar sein müssen.

### Process ID als Identity

Verworfen, da Prozesse kurzlebige Ausführungsinstanzen sind.

### IP-Adresse als Identity

Verworfen, da Adressen Erreichbarkeit und nicht Identität beschreiben.

## Abhängigkeiten

- `ADR-NETWORK-0006_Identity_Address_und_Route_strikt_trennen`
- `ADR-SECURITY-0001_Capability_Based_Security_als_zentrales_Autorisierungsmodell`
- `ADR-SECURITY-0006_ABAC_als_zentrale_Policyentscheidung`
- `ADR-PROCESS-0001_Natives_Execution_Domain_Modell`
- `ADR-DISTCOMM-0001_Nova_Call_als_einheitliche_Distributed_Call_Abstraktion`

## Zugehörige NPSPECs

- `NPSPEC-IDENTITY-MODEL-0001`
- `NPSPEC-IDENTITY-BINDING-0001`
- `NPSPEC-IDENTITY-CREDENTIAL-BINDING-0001`
- `NPSPEC-IDENTITY-EXECUTION-BINDING-0001`
- `NPSPEC-IDENTITY-ADDRESS-BINDING-0001`
- `NPSPEC-IDENTITY-TEST-0001`

## Ergebnis

NovaOS verwendet eine stabile Identity als eigenständige logische Referenz:

```text
                 Identity
                    │
        ┌───────────┼───────────┐
        ↓           ↓           ↓
   Credentials   Execution   Addresses
        ↓                       ↓
Authentication              Reachability

Display Name → Presentation
Capability   → Authority
```

Damit bleibt eindeutig getrennt, **wer oder was etwas ist, wie es dargestellt wird, wie seine Identität nachgewiesen wird, wo es ausgeführt wird, wo es erreichbar ist und was es tun darf**.