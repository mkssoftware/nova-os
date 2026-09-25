# NPSPEC-CAPABILITY-REGISTRY-0001 – Nova Capability Registry

## Status

Angenommen

## Kategorie

Capability / Registry / Discovery / Security

## Zweck

NovaOS definiert eine systemweite Capability Registry zur Registrierung und Auffindbarkeit verfügbarer Capability-Typen, Interfaces und Provider.

```text
Provider
   ↓
Capability Registration
   ↓
Capability Registry
   ↓
Discovery
   ↓
Authorized Acquisition
```

Die Registry beschreibt verfügbare Fähigkeiten, erzeugt jedoch selbst keine Autorität.

## Grundprinzipien

```text
Registered ≠ Authorized
Discoverable ≠ Accessible
Registry Entry ≠ Capability
Registry ≠ Capability Store
Registry ≠ Trust Authority
Provider Registration ≠ Provider Trust
Capability Metadata ≠ Capability Token
```

## Registry-Modell

Ein Registry-Eintrag beschreibt mindestens:

```text
CapabilityRegistryEntry
├── CapabilityTypeID
├── Semantic Interface
├── Provider Identity
├── Interface Version
└── State
```

Optional:

```text
Provider Instance
Trust Requirements
Security Requirements
Location Class
Sovereignty Properties
Resource Properties
Supported Constraints
Acquisition Method
Schema Reference
```

Die Registry speichert keine verwendbaren Capability Tokens.

## Capability Type Identity

Capability-Typen benötigen stabile Identitäten.

```text
CapabilityTypeID
      ↓
Semantic Capability
```

Beispiele:

```text
Storage.Read
Document.Render
Network.Connect
Audio.Output
Compute.GPU
Device.Camera
```

Name, Provider oder Implementierung dürfen nicht die grundlegende semantische Identität ersetzen.

## Provider Registration

Provider registrieren angebotene Capabilities über einen kontrollierten Mechanismus.

```text
Provider
   ↓
Identity Validation
   ↓
Registration Policy
   ↓
Registry Entry
```

Eine erfolgreiche Registrierung bedeutet nicht automatisch, dass der Provider vertrauenswürdig oder für jeden Requester nutzbar ist.

## Discovery

Capability Discovery verwendet die Registry als Informationsquelle.

```text
Capability Requirement
       ↓
Registry Query
       ↓
Candidate Providers
       ↓
Policy Filtering
       ↓
Negotiation / Acquisition
```

Die Registry darf nur Informationen zurückgeben, die der anfragende Security Context sehen darf.

## Registry und Authority

Die Registry darf keine Autorität direkt vergeben.

```text
Registry Entry
     ↓
Discovery
     ↓
Candidate
     ↓
Authorization / Negotiation
     ↓
Capability
```

Die tatsächliche Capability wird erst nach erfolgreicher Security-, Trust- und Policy-Prüfung erzeugt oder delegiert.

## Versionierung

Mehrere Interface-Versionen können parallel registriert sein.

```text
Document.Render
├── v1
├── v2
└── v3
```

Negotiation bestimmt eine kompatible Version.

Unsichere oder verbotene Versionen können durch Policy ausgeschlossen werden.

## Dynamische Registrierung

Die Registry muss dynamische Provider unterstützen.

```text
Register
Update
Suspend
Unregister
Replace
```

Dies gilt insbesondere für:

```text
Services
Drivers
Applications
Remote Providers
Hotplug Devices
Live-Replaced Components
```

## Provider-Ausfall

Fällt ein Provider aus:

```text
Provider Failure
      ↓
Registry State Update
      ↓
Provider Unavailable
```

Bestehende Capabilities werden separat über deren eigenen Lebenszyklus und Revocation-Regeln behandelt.

## Location Transparency

Die Registry kann lokale und entfernte Provider enthalten.

```text
CapabilityTypeID
├── Local Provider
├── Remote Provider
└── Migrated Provider
```

Der physische Standort darf die semantische Capability Identity nicht verändern.

Sovereignty- und Trust-Regeln bleiben dennoch verbindlich.

## Sicherheit

Registry-Manipulation ist sicherheitskritisch.

Nur autorisierte Komponenten dürfen:

```text
Register
Modify
Suspend
Remove
```

Registry-Einträge anderer Security Domains dürfen nicht unautorisiert verändert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
CapabilityTypeID
Provider Identity
Interface Version
Availability
Trust State
Location Class
Supported Constraints
Registry State
```

Sensible Provider- oder Infrastrukturdetails können gefiltert werden.

## Normative Anforderungen

1. NovaOS MUSS Capability-Typen und Provider zentral oder logisch einheitlich registrieren können.
2. Registry-Einträge DÜRFEN keine verwendbaren Capability Tokens enthalten.
3. Registrierung DARF keine Autorität für Requester erzeugen.
4. Capability-Typen MÜSSEN unabhängig von konkreten Providern identifizierbar sein.
5. Provider Registration MUSS autorisiert und validiert werden.
6. Discovery-Ergebnisse MÜSSEN abhängig vom Security Context filterbar sein.
7. Mehrere Provider und Interface-Versionen MÜSSEN parallel unterstützt werden können.
8. Dynamische Registrierung und Entfernung MÜSSEN unterstützt werden.
9. Lokale und entfernte Provider SOLLEN über dasselbe semantische Registry-Modell darstellbar sein.
10. Registry-Zustände MÜSSEN introspektierbar und auditierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-NEGOTIATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-CAPABILITY-APPLICATION-0001`
- `NPSPEC-CAPABILITY-DRIVER-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `NPSPEC-TRUST-POLICY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-CAPABILITY-0015`

## Ergebnis

```text
Capability Providers
        ↓
Controlled Registration
        ↓
Capability Registry
        ↓
Semantic Discovery
        ↓
Policy + Negotiation
        ↓
Explicit Capability Acquisition
```

NovaOS erhält damit eine einheitliche Registry für verfügbare Capability-Typen und Provider, ohne Registrierung, Auffindbarkeit oder Sichtbarkeit mit tatsächlicher Autorität gleichzusetzen.