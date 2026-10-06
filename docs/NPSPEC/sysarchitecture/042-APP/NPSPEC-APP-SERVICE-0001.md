# NPSPEC-APP-SERVICE-0001 – Nova App Service

## Status

Angenommen

## Kategorie

App / Service

## Zweck

NovaOS definiert App Services als von einer App bereitgestellte, klar definierte Dienste.

Ein App Service stellt Funktionalität über ein explizites Interface bereit und kann von Components, anderen Apps, Solutions oder autorisierten Systemkomponenten genutzt werden.

## Grundprinzipien

```text
Service ≠ App
Service ≠ Component
Service ≠ Process
Service ≠ Capability
Service Identity ≠ Provider Location
Service Discovery ≠ Authority
```

## Modell

```text
AppService
├── ServiceID
├── ProviderAppID
├── Interface
├── Version
├── ExecutionModel
├── Requirements
├── State
└── TrustState
```

`ServiceID` identifiziert den angebotenen Dienst unabhängig von Prozess, Instanz oder physischer Ausführungsposition.

## Bereitstellung

Eine App kann einen oder mehrere Services bereitstellen:

```text
App
├── Component
├── Service A
└── Service B
```

Ein Service darf intern durch einen oder mehrere Components implementiert werden.

Die interne Implementierung bleibt vom öffentlichen Service-Vertrag getrennt.

## Auflösung

Service-Nutzung erfolgt über registrierte Identitäten und Verträge:

```text
Service Requirement
        ↓
Service Discovery
        ↓
Provider Resolution
        ↓
Contract Validation
        ↓
Authorized Connection
```

Der Aufrufer darf nicht von einem festen Installationspfad oder Prozess abhängig sein.

## Interface

Ein Service muss ein definiertes Interface besitzen.

Dieses beschreibt mindestens:

```text
Operations
Input Types
Output Types
Errors
Version
Compatibility
```

Semantische Typen sollen für Ein- und Ausgaben verwendet werden, sofern verfügbar.

## Ausführung

Ein App Service kann abhängig von seinem Execution Contract ausgeführt werden als:

```text
In-Process
Out-of-Process
On-Demand
Persistent
Isolated
Remote
```

Die Ausführungsposition ist nicht Teil der Service-Identität.

## Lifecycle

```text
Registered
    ↓
Available
    ↓
Activated
    ↓
Running
    ↓
Idle / Suspended
    ↓
Deactivated
```

Services dürfen bei Bedarf aktiviert und bei Nichtbenutzung wieder freigegeben werden.

## Sicherheit

Service Discovery erzeugt keine Berechtigung zur Nutzung.

```text
Caller Authority
      ∩
Service Requirements
      ∩
Provider Policy
      ∩
System Policy
      =
Effective Service Authority
```

Capability- und Handle-Übertragungen müssen explizit erfolgen.

Ein Service darf die Authority seiner Clients nicht implizit übernehmen.

## Fehler und Verfügbarkeit

Service-Ausfälle müssen vom Provider-Ausfall unterscheidbar sein.

Mögliche Zustände:

```text
Available
Degraded
Unavailable
Blocked
Failed
```

NovaOS darf bei kompatiblen Services alternative Provider auswählen.

## Versionierung

Service-Interface und Implementierung müssen getrennt versionierbar sein.

Mehrere kompatible Implementierungen oder Versionen dürfen parallel verfügbar sein.

## Normative Anforderungen

1. Jeder registrierte App Service MUSS eine stabile `ServiceID` besitzen.
2. Service-Identität MUSS von Prozess und Ausführungsort unabhängig sein.
3. Jeder Service MUSS ein explizites Interface besitzen.
4. Service Discovery DARF keine Authority erzeugen.
5. Service-Nutzung MUSS autorisiert werden.
6. Services DÜRFEN intern aus mehreren Components bestehen.
7. Interne Implementierung und öffentlicher Service-Vertrag MÜSSEN getrennt bleiben.
8. Services DÜRFEN bedarfsgesteuert aktiviert werden.
9. Mehrere kompatible Provider MÜSSEN unterstützt werden können.
10. Service- und Implementierungsversion MÜSSEN getrennt behandelbar sein.
11. Ein Service DARF Client-Authority nicht implizit übernehmen.
12. ServiceID, Provider, Version, Zustand, Ausführungsort und effektive Authority MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-APP-COMPONENT-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-REGISTRY-SERVICE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein einheitliches Modell für von Apps bereitgestellte Services. Dienste werden über stabile Identitäten und explizite Verträge angesprochen, können unabhängig von ihrer Implementierung aufgelöst und ausgeführt werden und bleiben vollständig in das Capability-, Trust- und Introspection-Modell von NovaOS integriert.