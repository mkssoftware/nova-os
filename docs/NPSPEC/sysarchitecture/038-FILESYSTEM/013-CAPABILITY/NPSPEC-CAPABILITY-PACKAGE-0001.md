# NPSPEC-CAPABILITY-PACKAGE-0001 – Nova Capability Package

## Status

Angenommen

## Kategorie

Capability / Package

## Zweck

NovaOS definiert ein Capability Package als installierbare und versionierte Einheit zur Bereitstellung einer oder mehrerer Capability-Implementierungen.

Ein Package enthält Implementierung, Manifest, Abhängigkeiten und erforderliche Ressourcen, ohne selbst Authority zu repräsentieren.

## Grundprinzipien

```text
Package ≠ Capability
Package ≠ CapabilityID
Package ≠ Provider
Package ≠ Authority
Installed ≠ Authorized
Package Signature ≠ Trust
```

## Package-Modell

```text
CapabilityPackage
├── PackageID
├── Version
├── ProviderID
├── Manifest
├── Implementations/
├── Resources/
└── Dependencies/
```

Ein Package darf mehrere Capabilities bereitstellen:

```text
PackageID
├── Capability A
├── Capability B
└── Capability C
```

Jede Capability behält ihre eigene stabile `CapabilityID`.

## Manifest

Das Package-Manifest beschreibt mindestens:

```text
PackageID
Version
ProviderID
Provided Capabilities
Capability Versions
Interfaces
Dependencies
Compatibility
Resource Requirements
Trust Information
```

Das Manifest beschreibt Eigenschaften und Anforderungen, erzeugt jedoch keine Berechtigungen.

## Capability-Bereitstellung

Nach Installation können die enthaltenen Implementierungen in der Capability Registry registriert werden:

```text
Capability Package
       ↓
Validate
       ↓
Install
       ↓
Register Provider
       ↓
Capability Registry
```

Die Registry verweist auf die bereitgestellten Capabilities und ihren Provider.

## Mehrere Provider

Unterschiedliche Packages dürfen dieselbe Capability implementieren:

```text
de.nova.image.decode
├── Package A / Provider A
├── Package B / Provider B
└── Package C / Provider C
```

Die Auswahl einer Implementierung erfolgt unabhängig von der Package-Identität über Registry, Policy und Execution Contract.

## Abhängigkeiten

Capability Packages dürfen Abhängigkeiten besitzen auf:

```text
System Libraries
Runtime
Framework
Other Capability Packages
System Interfaces
Hardware Features
```

Abhängigkeiten müssen versioniert und deterministisch auflösbar sein.

## Versionierung

Package-Version und Capability-Version bleiben getrennt:

```text
PackageID + PackageVersion

CapabilityID + CapabilityVersion
```

Eine neue Package-Version muss nicht automatisch eine neue Capability-Version erzeugen.

## Installation und Update

Package-Änderungen müssen kontrolliert erfolgen:

```text
Validate
  ↓
Trust Check
  ↓
Resolve Dependencies
  ↓
Install / Update
  ↓
Register
  ↓
Verify
```

Aktive Implementierungen dürfen nur ersetzt werden, wenn der Wechsel sicher durchgeführt werden kann.

## Sicherheit

Vor der Aktivierung müssen mindestens relevante Eigenschaften prüfbar sein:

```text
Identity
Integrity
Signature
Provenance
Trust
Compatibility
```

Ein installiertes Capability Package erhält dadurch keine Runtime-Authority.

Benötigte Systemressourcen müssen separat capability-basiert autorisiert werden.

## Entfernung

Bei der Deinstallation müssen die zugehörigen Provider-Registrierungen entfernt oder deaktiviert werden.

Bestehende Capability-Tokens dürfen nicht allein aufgrund der Package-Entfernung auf eine andere Implementierung umgebunden werden.

## Normative Anforderungen

1. Capability Packages MÜSSEN eine stabile `PackageID` besitzen.
2. Package- und Capability-Identität MÜSSEN getrennt bleiben.
3. Ein Package DARF mehrere Capabilities bereitstellen.
4. Mehrere Packages DÜRFEN dieselbe Capability implementieren.
5. Packages MÜSSEN versionierbar sein.
6. Package-Version und Capability-Version MÜSSEN getrennt behandelt werden.
7. Bereitgestellte Capabilities MÜSSEN über ihre stabile `CapabilityID` registriert werden.
8. Package-Abhängigkeiten MÜSSEN deterministisch auflösbar sein.
9. Installation DARF keine Capability-Authority erzeugen.
10. Packages MÜSSEN vor Aktivierung auf Integrität und Trust prüfbar sein.
11. Package-Updates DÜRFEN bestehende Authority nicht implizit erweitern.
12. Deinstallation DARF bestehende Handles nicht auf andere Provider umleiten.
13. Package, Provider, Capabilities und Versionen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-NAMING-0001`
- `NPSPEC-CAPABILITY-NAMESPACE-0001`
- `NPSPEC-REGISTRY-CAPABILITY-0001`
- `NPSPEC-TRUST-CAPABILITY-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-POLICY-INSTALL-0001`
- `NPSPEC-SYSTEM-UPDATES-0001`

## Ergebnis

NovaOS kann Capability-Implementierungen als eigenständige, versionierte Packages installieren, aktualisieren und entfernen. Package, Capability, Provider, Version und Authority bleiben dabei getrennte Konzepte, sodass mehrere austauschbare Implementierungen derselben Capability sicher koexistieren können.