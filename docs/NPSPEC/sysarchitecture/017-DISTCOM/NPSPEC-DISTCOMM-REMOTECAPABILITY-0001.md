# NPSPEC-DISTCOMM-REMOTECAPABILITY-0001 – Nova Remote Capability

## Status

Angenommen

## Kategorie

Distributed Communication / Capability Security / Remote Authority

## Zweck

NovaOS definiert Remote Capabilities als kontrollierte Erweiterung des lokalen Capability-Modells über System- und Netzwerkgrenzen hinweg.

```text
Local Process
     ↓
Remote Capability
     ↓
Secure Distributed Channel
     ↓
Remote Service / Resource
```

Eine Remote Capability repräsentiert delegierte Autorität auf eine entfernte Ressource, ohne deren physische Netzwerkadresse zur Berechtigung zu machen.

## Grundprinzip

```text
Remote Address ≠ Authority
Service Identity ≠ Permission
Capability Reference ≠ Capability Secret
Connectivity ≠ Authorization
Remote Capability ≠ Unlimited Delegation
```

Die Berechtigung entsteht aus der Capability und ihrem Security Context, nicht aus IP-Adresse, Port oder Erreichbarkeit.

## Capability-Modell

Eine Remote Capability besitzt mindestens:

```text
RemoteCapability
├── CapabilityID
├── TargetID
├── Rights
├── Issuer
├── Holder
└── State
```

Optional:

```text
Constraints
Expiration
Delegation Policy
Security Context
Network Intent
Sovereignty Policy
ExecutionContract
Revocation Information
```

`TargetID` bleibt unabhängig vom aktuellen Netzwerkstandort der Ressource.

## Rechte

Capabilities müssen möglichst eng begrenzte Rechte besitzen.

Beispiele:

```text
Read
Write
Execute
Invoke
Subscribe
Publish
Administer
```

Rechte dürfen zusätzlich auf bestimmte Methoden oder Operationen begrenzt werden.

```text
Capability
└── Service X
    ├── ReadStatus
    └── Restart
```

## Delegation

Eine Remote Capability darf delegierbar sein, sofern die ursprüngliche Capability dies erlaubt.

```text
Holder A
   ↓ delegate
Holder B
```

Delegation darf Rechte abschwächen:

```text
Read + Write
     ↓
Read
```

Sie darf keine zusätzlichen Rechte erzeugen.

```text
Delegated Rights ⊆ Original Rights
```

## Attenuation

Capabilities dürfen durch zusätzliche Einschränkungen abgeschwächt werden.

Beispiele:

```text
Time Limit
Method Limit
Resource Limit
Network Constraint
Sovereignty Constraint
Usage Count
```

Eine abgeschwächte Capability darf nicht wieder erweitert werden, sofern keine separate höhere Autorität dies ausdrücklich erlaubt.

## Remote Invocation

Eine Remote Capability darf für RPC oder Messaging verwendet werden.

```text
Remote Capability
       ↓
Authorization
       ↓
Distributed RPC
       ↓
Remote Service
```

Die entfernte Seite muss die Capability vor Ausführung der geschützten Operation validieren.

Der Transport einer Capability allein darf keine erfolgreiche Autorisierung garantieren.

## Übertragung

Capabilities dürfen ausschließlich über geeignete geschützte Kommunikationspfade übertragen werden.

```text
Capability
    ↓
Protected Serialization
    ↓
Authenticated Channel
    ↓
Remote Security Domain
```

Capability-Material darf nicht unbeabsichtigt in:

```text
Logs
URLs
Diagnostics
Crash Dumps
Telemetry
```

offengelegt werden.

## Location Transparency

Remote Capabilities verwenden logische Ressourcenidentitäten.

```text
Capability
    ↓
TargetID
    ↓
Service Resolution
    ↓
Current Endpoint
```

Eine Ressource darf migrieren oder repliziert werden, ohne dass dadurch automatisch neue Berechtigungen entstehen.

```text
Identity ≠ Location
Transparent Location ≠ Transparent Authority
```

## Revocation

Remote Capabilities müssen widerrufbar sein können, wenn ihre Capability-Klasse dies erfordert.

```text
Active
  ↓
Revocation
  ↓
Revoked
```

Verteilte Systeme können Revocation nicht immer sofort global sichtbar machen.

Das Modell muss deshalb Zustände wie:

```text
Valid
Expired
Revoked
Unknown
```

darstellen können.

`Unknown` darf nicht automatisch als `Valid` behandelt werden, wenn eine aktuelle Validierung zwingend erforderlich ist.

## Ablaufzeit

Capabilities dürfen zeitlich begrenzt werden.

```text
Issued
  ↓
Valid
  ↓
Expired
```

Zeitbasierte Regeln müssen mit der NovaOS-Zeit- und Sicherheitsinfrastruktur kompatibel sein.

## Netzwerkfehler

Capability-Gültigkeit und Netzwerkverfügbarkeit bleiben getrennt.

```text
Capability Valid + Network Unavailable
Capability Revoked + Network Available
```

Ein Netzwerkfehler darf nicht als Capability-Widerruf interpretiert werden.

Ebenso darf erfolgreiche Netzwerkkommunikation keine Capability-Gültigkeit beweisen.

## Sovereignty

Remote Capabilities dürfen Sovereignty-Einschränkungen enthalten.

```text
Capability
├── Rights
└── Allowed Sovereignty Context
```

Ein gültiges Recht darf nicht über einen Netzwerkpfad ausgeübt werden, der verbindliche Sovereignty-Anforderungen verletzt.

## Ressourcensteuerung

NovaOS muss Ressourcen begrenzen können für:

```text
Remote Capabilities
Validation State
Delegation Chains
Revocation State
Pending Validation
Capability Cache
```

Capability-Validierungen dürfen keine unbegrenzten Netzwerk- oder Ressourcenzyklen verursachen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
CapabilityID
TargetID
Issuer
Holder
Rights
Constraints
State
Expiration
Delegation Depth
Revocation State
```

Geheimes Capability-Material darf nicht über normale Introspection ausgegeben werden.

## Normative Anforderungen

1. NovaOS MUSS Remote Capabilities unterstützen können.
2. Remote Authority MUSS unabhängig von Netzwerkadresse und Port sein.
3. Capabilities MÜSSEN explizite Rechte besitzen.
4. Delegation DARF keine zusätzlichen Rechte erzeugen.
5. Capability Attenuation MUSS unterstützt werden können.
6. Remote Capabilities MÜSSEN sicher übertragbar sein.
7. Capability-Material DARF NICHT ungeschützt protokolliert werden.
8. Remote Services MÜSSEN Capabilities vor geschützten Operationen validieren können.
9. Capability-Gültigkeit und Netzwerkverfügbarkeit MÜSSEN getrennt bleiben.
10. Revocation MUSS einen expliziten `Unknown`-Zustand unterstützen können.
11. Sovereignty- und Security-Regeln DÜRFEN durch Remote Capabilities NICHT umgangen werden.
12. Capability-Zustände SOLLEN ohne Offenlegung geheimer Daten introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-CAPABILITY-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-DISTCOMM-RPC-0001`
- `NPSPEC-DISTCOMM-MESSAGEBUS-0001`
- `NPSPEC-DISTCOMM-PUBSUB-0001`
- `NPSPEC-DISTCOMM-TLS-0001`
- `NPSPEC-NETWORK-INTENT-0001`
- `NPSPEC-NETWORK-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-DISTCOMM-0007`

## Ergebnis

```text
Authority
   ↓
Remote Capability
   ↓
Rights + Constraints
   ↓
Secure Distributed Communication
   ↓
Remote Validation
   ↓
Authorized Operation
```

NovaOS erhält damit ein verteiltes Capability-Modell, bei dem Autorität kontrolliert über Systemgrenzen übertragen, eingeschränkt, delegiert und widerrufen werden kann, ohne Berechtigungen an Netzwerkadressen oder physische Standorte zu binden.