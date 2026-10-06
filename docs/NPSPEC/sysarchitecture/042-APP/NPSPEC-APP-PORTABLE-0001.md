# NPSPEC-APP-PORTABLE-0001 – Nova Portable App

## Status

Angenommen

## Kategorie

App / Portability

## Zweck

NovaOS definiert ein Portabilitätsmodell für Apps, damit kompatible Apps unabhängig von einem bestimmten Gerät, Installationspfad oder konkreten Capability-Provider ausgeführt werden können.

Portabilität beschreibt die Übertragbarkeit einer App und darf weder Sicherheit noch Kompatibilitätsprüfung umgehen.

## Grundprinzipien

```text
Portability ≠ Compatibility
Portability ≠ Installation
Portability ≠ Trust
Portability ≠ Authority
App Identity ≠ Device
App Identity ≠ Path
Capability Requirement ≠ Provider
```

## Modell

```text
PortableApp
├── AppID
├── Package
├── Requirements[]
├── CapabilityRequirements[]
├── Compatibility
├── Resources
└── PortableState
```

Die stabile `AppID` bleibt bei einer Übertragung auf ein anderes Gerät erhalten.

## Portabilität

Eine portable App darf übertragen werden als:

```text
App Package
    +
Resources
    +
Portable Configuration
    +
Optional Portable State
```

Gerätespezifische Daten, temporäre Daten, Caches, Credentials und lokale Sicherheitszustände gehören nicht automatisch zum portablen Zustand.

## Zielsystem

Vor der Ausführung prüft NovaOS:

```text
Portable App
     ↓
Compatibility Validation
     ↓
Capability Resolution
     ↓
Trust Validation
     ↓
Permission Evaluation
     ↓
Execution
```

Fehlende Anforderungen müssen als nicht verfügbar oder inkompatibel behandelt werden.

## Capability-Abhängigkeiten

Portable Apps sollen Funktionen über stabile `CapabilityID`s anfordern.

```text
Capability Requirement
        ↓
Target System
        ↓
Compatible Provider
```

Die App darf dadurch auf unterschiedlichen Geräten verschiedene Implementierungen derselben Capability verwenden.

Der konkrete Provider ist nicht Bestandteil der portablen App-Identität.

## Pfadunabhängigkeit

Eine portable App darf nicht von einem festen Installationspfad abhängig sein.

Interne Ressourcen müssen über package-relative oder registrierte Ressourcenreferenzen erreichbar sein.

Externe Objekte sollen über stabile ObjectIDs, semantische Referenzen oder neu autorisierte Ressourcenbindungen angesprochen werden.

## Berechtigungen

Das Übertragen einer App überträgt nicht automatisch deren Berechtigungen.

```text
Portable App
    +
Target Device Policy
    +
Validated Identity
    +
User Decisions
      ↓
Effective Permissions
```

Persistente Berechtigungen dürfen nur übernommen werden, wenn NovaOS die App-Identität, Integrität, Sicherheitsanforderungen und geltende Policy ausreichend validieren kann.

## Zustand

Portable State muss von gerätegebundenem Runtime State getrennt bleiben.

```text
Portable
├── User Configuration
├── App State
└── Portable Metadata

Device Local
├── Cache
├── Temporary Data
├── Hardware Bindings
├── Runtime Handles
└── Security Tokens
```

Runtime Handles und Capability Tokens dürfen nicht als portable Referenzen behandelt werden.

## Offline-Ausführung

Eine portable App darf ohne Netzwerk ausgeführt werden, sofern alle notwendigen lokalen Ressourcen und Capabilities verfügbar sind.

Remote-Abhängigkeiten müssen explizit als solche erkennbar sein.

## Programme und Solutions

Das Portabilitätsmodell gilt unabhängig vom App-Typ.

```text
Portable App
├── Program
├── Solution
├── Web App Package
└── Registered App Type
```

Die konkrete Portabilität wird durch die Anforderungen des jeweiligen App-Typs bestimmt.

## Normative Anforderungen

1. Portable Apps MÜSSEN ihre stabile `AppID` bei Gerätewechsel behalten können.
2. App-Identität MUSS von Gerät und Installationspfad unabhängig bleiben.
3. Zielsysteme MÜSSEN Anforderungen vor der Ausführung validieren.
4. Capability-Abhängigkeiten SOLLEN über stabile `CapabilityID`s beschrieben werden.
5. Konkrete Capability-Provider DÜRFEN zwischen Geräten variieren.
6. Portabilität DARF keine Authority erzeugen oder erweitern.
7. Berechtigungen DÜRFEN nicht ungeprüft auf andere Geräte übertragen werden.
8. Portable und gerätegebundene Zustände MÜSSEN unterscheidbar sein.
9. Runtime Handles, Capability Tokens und vergleichbare Sicherheitsobjekte DÜRFEN nicht direkt portiert werden.
10. Apps DÜRFEN nicht von festen absoluten Installationspfaden abhängig sein müssen.
11. Offline-Ausführung MUSS möglich sein, sofern alle erforderlichen lokalen Abhängigkeiten verfügbar sind.
12. Portabilitätsstatus, Anforderungen, Kompatibilität und fehlende Abhängigkeiten MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-COMPATIBILITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-SOLUTION-PORTABILITY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Apps unabhängig von Gerät, Pfad und konkreten Capability-Providern übertragen und ausführen. Portabilität bleibt dabei von Kompatibilität, Trust und Authority getrennt, sodass Apps flexibel zwischen NovaOS-Systemen wechseln können, ohne Sicherheitsgrenzen oder gerätespezifische Zustände unkontrolliert mitzunehmen.