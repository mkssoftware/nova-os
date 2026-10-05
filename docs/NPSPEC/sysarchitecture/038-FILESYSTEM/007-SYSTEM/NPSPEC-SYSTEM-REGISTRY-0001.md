# NPSPEC-SYSTEM-REGISTRY-0001 – Nova System Registry

## Status

Angenommen

## Kategorie

System / Registry

## Zweck

NovaOS definiert die System Registry als strukturierte Registrierung systemweiter Komponenten und ihrer stabilen Identitäten.

Die Registry ermöglicht Discovery und Auflösung, ist jedoch weder universelle Konfigurationsdatenbank noch Speicherort für Berechtigungen oder laufende Zustände.

## Grundprinzipien

```text
Registry ≠ Filesystem
Registry ≠ Settings
Registry ≠ Permission Store
Registry ≠ Runtime State
Registration ≠ Authority
Registry Entry ≠ Component
```

## Registry-Modell

Ein Eintrag kann enthalten:

```text
RegistryEntry
├── RegistryID
├── ComponentID
├── ComponentType
├── Version
├── Interfaces
├── State
└── Reference
```

Optional:

```text
Provider
Dependencies
Compatibility
Trust State
Metadata
```

`Reference` verweist auf die tatsächliche Systemressource und ersetzt deren eigene Identität nicht.

## Registrierbare Komponenten

Die Registry kann insbesondere erfassen:

```text
System Services
System Modules
Libraries
Runtimes
Drivers
Framework Provider
Capability Provider
System Components
```

Spezialisierte Subsysteme dürfen eigene Registries besitzen.

## Registrierung

```text
Component
    ↓
Validate Identity
    ↓
Validate Metadata
    ↓
Register
    ↓
Publish
```

Nur erfolgreich validierte Einträge dürfen als aktiv veröffentlicht werden.

## Discovery

Andere Systemkomponenten können die Registry zur Suche verwenden:

```text
Requirement
    ↓
Registry Query
    ↓
Candidate Components
    ↓
Compatibility / Policy Check
    ↓
Resolved Component
```

Discovery erzeugt keine Authority.

## Identität

Die Registry verwendet stabile Komponentenidentitäten.

```text
ComponentID ≠ Path
ComponentID ≠ Filename
ComponentID ≠ ProcessID
```

Eine interne Verschiebung oder Änderung des physischen Speicherorts darf die Komponentenidentität nicht automatisch verändern.

## Aktualisierung

Registry-Änderungen sollen transaktional erfolgen.

```text
Prepare
  ↓
Validate
  ↓
Commit
  ↓
Publish
```

Updates und Live Replacement müssen bestehende und neue Versionen eindeutig unterscheiden können.

## Konsistenz

Die Registry darf keine nicht mehr vorhandenen Komponenten dauerhaft als verfügbar melden.

Inkonsistente Einträge müssen erkannt, entfernt oder neu aufgebaut werden können.

Die Registry darf aus den maßgeblichen Komponenteninformationen rekonstruierbar sein.

## Sicherheit

Registrierung, Änderung und Entfernung systemweiter Einträge erfordern entsprechende Authority.

Ein Registry-Eintrag selbst enthält keine Capability und gewährt keinen Zugriff auf die registrierte Ressource.

## Normative Anforderungen

1. NovaOS MUSS eine strukturierte System Registry bereitstellen.
2. Registrierte Komponenten MÜSSEN über stabile Identitäten referenzierbar sein.
3. Registry-Einträge DÜRFEN nicht als Komponentenidentität selbst behandelt werden.
4. Registrierung MUSS validierbar sein.
5. Discovery DARF keine Authority erzeugen.
6. Registry-Einträge DÜRFEN keine aktiven Capability-Tokens enthalten.
7. System Settings SOLLEN nicht als allgemeine Registry-Daten gespeichert werden.
8. Registry-Änderungen SOLLEN transaktional erfolgen.
9. Nicht mehr gültige Einträge MÜSSEN erkennbar sein.
10. Die Registry MUSS rekonstruierbar sein.
11. Registry-Änderungen MÜSSEN autorisiert sein.
12. Identität, Version, Typ, Provider und Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SYSTEM-LAYOUT-0001`
- `NPSPEC-SYSTEM-DRIVERS-0001`
- `NPSPEC-SYSTEM-FRAMEWORK-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-MODULES-0001`
- `NPSPEC-SYSTEM-SERVICES-0001`
- `NPSPEC-SYSTEM-LIBRARIES-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt eine einheitliche Registry für systemweite Komponenten und Provider. Sie ermöglicht stabile Registrierung, Discovery und Auflösung, ohne Konfiguration, Berechtigungen, Objektidentitäten oder Laufzeitzustände in einer zentralen monolithischen Datenbank zu vermischen.