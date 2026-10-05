# NPSPEC-CAPABILITY-REGISTRATION-0001 – Nova Capability Registration

## Status

Angenommen

## Kategorie

Capability / Registration

## Zweck

NovaOS definiert die kontrollierte Registrierung von Capabilities, Providern und Implementierungen in der Capability Registry.

Die Registrierung macht eine Capability für Discovery und Resolution bekannt, erzeugt jedoch weder Authority noch Berechtigungen und aktiviert nicht automatisch eine Implementierung.

## Grundprinzipien

```text
Registration ≠ Authority
Registration ≠ Permission
Registration ≠ Trust
Registration ≠ Activation
Registration ≠ Provider Selection
Registered ≠ Available
```

## Registrierungsmodell

Eine Registrierung verbindet:

```text
CapabilityRegistration
├── CapabilityID
├── CapabilityVersion
├── InterfaceVersion
├── ProviderID
├── ImplementationID
├── PackageID
├── Manifest
├── Compatibility
├── TrustState
└── State
```

## Ablauf

```text
Capability Package
      ↓
Manifest Validation
      ↓
Identity Validation
      ↓
Integrity / Trust Check
      ↓
Interface Validation
      ↓
Dependency Check
      ↓
Compatibility Check
      ↓
Registry Transaction
      ↓
Publish Registration
```

Erst nach erfolgreichem Commit darf der Eintrag durch Discovery sichtbar werden.

## Identitäten

Folgende Identitäten bleiben getrennt:

```text
CapabilityID
ProviderID
ImplementationID
PackageID
```

Mehrere Provider und Implementierungen dürfen dieselbe `CapabilityID` registrieren.

```text
CapabilityID
├── Provider A
│   ├── Implementation A1
│   └── Implementation A2
└── Provider B
    └── Implementation B1
```

## Validierung

Vor einer Registrierung müssen mindestens geprüft werden:

```text
Manifest Schema
CapabilityID
Provider Identity
Implementation Identity
Interface
Versions
Dependencies
Compatibility
Integrity
Trust Information
```

Ungültige oder widersprüchliche Registrierungen dürfen nicht veröffentlicht werden.

## Registrierung und Verfügbarkeit

Ein registrierter Eintrag kann unterschiedliche Zustände besitzen:

```text
Registered
Available
Unavailable
Restricted
Disabled
Incompatible
Revoked
```

Die Registry darf daher Registrierung nicht mit aktueller Ausführbarkeit gleichsetzen.

## Aktualisierung

Änderungen an Package, Provider oder Implementierung müssen über eine kontrollierte Registry-Aktualisierung erfolgen.

```text
Validate
   ↓
Stage
   ↓
Commit
   ↓
Publish
```

Registry-Änderungen müssen generationen- beziehungsweise versionskonsistent sichtbar werden.

## Entfernung

Beim Entfernen eines Capability-Pakets müssen dessen Provider- und Implementierungsregistrierungen kontrolliert entfernt oder deaktiviert werden.

Bestehende Handles oder laufende Ausführungen werden nach ihrer eigenen Lifecycle-, Revocation- und Execution-Policy behandelt.

## Sicherheit

Die Berechtigung zur Registrierung ist von der Berechtigung zur Nutzung einer Capability getrennt.

```text
Register Capability
        ≠
Use Capability
```

Registry-Einträge dürfen keine Capability-Tokens, Credentials oder andere geheime Authority enthalten.

## Normative Anforderungen

1. Capabilities MÜSSEN über die Capability Registry registrierbar sein.
2. Registrierung MUSS auf validierten Manifestdaten basieren.
3. CapabilityID, ProviderID, ImplementationID und PackageID MÜSSEN getrennt bleiben.
4. Mehrere Provider und Implementierungen derselben Capability MÜSSEN registrierbar sein.
5. Identität, Interface, Versionen, Integrität und Abhängigkeiten MÜSSEN vor Veröffentlichung validiert werden.
6. Registry-Änderungen MÜSSEN transaktional veröffentlicht werden.
7. Unvollständige Registrierungen DÜRFEN nicht sichtbar werden.
8. Registrierung und aktuelle Verfügbarkeit MÜSSEN getrennt behandelt werden.
9. Registrierung DARF keine Capability-Authority oder Permission erzeugen.
10. Registry-Einträge DÜRFEN keine Tokens, Credentials oder geheimen Handles enthalten.
11. Updates und Entfernung MÜSSEN bestehende Registrierungen kontrolliert ersetzen oder deaktivieren.
12. Discovery MUSS ausschließlich veröffentlichte Registrierungen berücksichtigen.
13. Registrierungsstatus, Herkunft, Provider, Implementierung und Version MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-PACKAGE-0001`
- `NPSPEC-CAPABILITY-MANIFEST-0001`
- `NPSPEC-CAPABILITY-INTERFACE-0001`
- `NPSPEC-CAPABILITY-IMPLEMENTATION-0001`
- `NPSPEC-CAPABILITY-DEPENDENCY-0001`
- `NPSPEC-CAPABILITY-VERSIONING-0001`
- `NPSPEC-CAPABILITY-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-REGISTRY-UPDATE-0001`

## Ergebnis

NovaOS besitzt einen kontrollierten und transaktionalen Registrierungsprozess für Capabilities. Capability-Pakete können ihre Provider und Implementierungen eindeutig in der Registry veröffentlichen, sodass sie für Discovery und spätere Resolution verfügbar werden, ohne Registrierung mit Trust, Permission, Authority oder tatsächlicher Ausführung gleichzusetzen.