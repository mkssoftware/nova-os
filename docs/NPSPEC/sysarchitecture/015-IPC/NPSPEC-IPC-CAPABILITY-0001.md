# NPSPEC-IPC-CAPABILITY-0001 – Nova IPC Capability Model

## Status

Entwurf

## Kategorie

IPC / Capability / Security

## Zweck

NovaOS definiert ein Capability-basiertes Berechtigungsmodell für IPC. Kommunikation mit einem Endpoint ist nur erlaubt, wenn der aufrufende Kontext eine passende Capability besitzt.

```text
Process A
   ↓
IPC Capability
   ↓
Endpoint
   ↓
Process B
```

Die Kenntnis eines Endpoints oder einer Interface-ID erzeugt keine Zugriffsberechtigung.

## Grundprinzip

```text
Endpoint Identity ≠ Permission
Interface Knowledge ≠ Authority
Capability Possession ≠ Unlimited Access
Capability Transfer ≠ Authority Creation
```

IPC-Autorität muss explizit vergeben, begrenzt und widerrufbar sein.

## IPC Capability

Eine IPC-Capability beschreibt die erlaubte Kommunikation mit einem bestimmten Ziel.

```text
IPCCapability
├── CapabilityID
├── Endpoint
├── Interface
├── Operations
└── Constraints
```

Optionale Einschränkungen können enthalten:

```text
Message Types
Resource Limits
Deadline Limits
Delegation Rights
Lifetime
Security Context
```

## Operationsrechte

Capabilities sollen auf konkrete Operationen begrenzbar sein.

```text
StorageService
├── Read
├── Write
├── Delete
└── Admin
```

Eine `Read`-Capability darf beispielsweise keine `Write`- oder `Admin`-Operation autorisieren.

## Endpoint Binding

Capabilities müssen an einen logischen Endpoint oder eine definierte Service-Identität gebunden werden können.

```text
Capability
    ↓
EndpointID
    ↓
Resolved Location
```

Eine Änderung der physischen Location darf die Capability nicht automatisch verändern.

Location Transparency darf keine zusätzlichen Rechte erzeugen.

## Capability Transfer

Capabilities dürfen über IPC kontrolliert übertragen werden.

```text
Process A
   ↓
Delegate
   ↓
Process B
```

Der Sender muss zur Delegation berechtigt sein.

Eine delegierte Capability darf:

```text
gleich stark
oder
stärker eingeschränkt
```

sein, aber niemals zusätzliche Autorität erzeugen.

## Capability Attenuation

Bei der Delegation sollen Rechte reduziert werden können.

```text
Read + Write + Delete
        ↓
Attenuation
        ↓
Read
```

Einschränkungen dürfen insbesondere betreffen:

```text
Operations
Endpoint
Interface
Lifetime
Resource Budget
Delegation
```

## Capability Transfer in Typed IPC

Typed IPC darf Capability-Typen explizit beschreiben.

```text
Request<
    Capability<StorageRead>
>
```

Der Typ einer Capability beschreibt ihre erwartete Semantik, ersetzt jedoch keine Autorisierungsprüfung.

## Widerruf

IPC-Capabilities müssen widerrufbar sein.

```text
Granted
   ↓
Revoke
   ↓
Invalid
```

Widerruf kann beispielsweise erfolgen bei:

```text
Process Termination
Service Shutdown
Security Policy Change
Sandbox Violation
Capability Expiration
Endpoint Removal
```

Nach Widerruf dürfen keine neuen Operationen über die Capability gestartet werden.

## Lebenszyklus

```text
Create
  ↓
Grant
  ↓
Use
  ↓
Delegate
  ↓
Revoke / Expire
```

Capabilities müssen einen eindeutig bestimmbaren Zustand besitzen.

## Endpoint-Neustart

Ein Neustart eines Services darf nicht automatisch alte Capability-Garantien verletzen.

NovaOS muss unterscheiden können zwischen:

```text
Logical Service Identity
Service Instance
Endpoint Instance
```

Capabilities können abhängig von ihrer Definition an eine logische Identität oder konkrete Instanz gebunden sein.

## Resource Control

Eine Capability darf zusätzlich Ressourcenlimits enthalten.

```text
Capability
├── Max Queue Usage
├── Max Message Size
├── Rate Limit
└── Resource Budget
```

Capability-Besitz bedeutet damit nicht automatisch unbegrenzte Nutzung eines Services.

## Sicherheit

IPC muss vor Ausführung mindestens prüfen können:

```text
Capability Valid?
Endpoint Allowed?
Interface Allowed?
Operation Allowed?
Constraints Satisfied?
Security Context Valid?
```

Eine fehlgeschlagene Prüfung muss die Operation vor ihrer Ausführung verhindern.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Capability Type
Endpoint
Interface
Operations
Constraints
Delegation State
Expiration
Revocation State
```

Geheime Capability-Werte oder nicht autorisierte Sicherheitsinformationen dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS IPC-Zugriffe durch explizite Capabilities kontrollieren können.
2. Die Kenntnis einer Endpoint-ID DARF NICHT automatisch Zugriff gewähren.
3. IPC-Capabilities MÜSSEN auf Endpoints, Interfaces und Operationen begrenzbar sein.
4. Capability-Besitz DARF NICHT automatisch unbegrenzte Ressourcennutzung erlauben.
5. Capability-Delegation MUSS explizit autorisiert sein.
6. Delegierte Capabilities DÜRFEN NICHT mehr Autorität als ihre Quelle besitzen.
7. NovaOS SOLL Capability Attenuation unterstützen.
8. Typed IPC DARF Capability-Typen verwenden, MUSS aber weiterhin die tatsächliche Autorisierung prüfen.
9. IPC-Capabilities MÜSSEN widerrufbar oder zeitlich begrenzbar sein können.
10. Widerrufene Capabilities DÜRFEN NICHT für neue Operationen verwendet werden.
11. Location Transparency DARF Capability-Grenzen NICHT verändern oder umgehen.
12. Capability-Zustände und Berechtigungsumfang SOLLEN sicher introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IPC-0001`
- `NPSPEC-IPC-TYPED-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-DRIVER-CAPABILITY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IPC-0003`

## Ergebnis

```text
Caller
  ↓
IPC Capability
  ↓
Endpoint + Interface + Operation
  ↓
Authorization
  ↓
IPC Execution
```

NovaOS erhält damit ein explizites Capability-Modell für IPC, bei dem Kommunikationsrechte gezielt vergeben, eingeschränkt, delegiert und widerrufen werden können, ohne Endpoint-Kenntnis mit Autorität gleichzusetzen.