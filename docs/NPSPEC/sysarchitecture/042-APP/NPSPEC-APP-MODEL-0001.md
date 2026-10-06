# NPSPEC-APP-MODEL-0001 – Nova App Model

## Status

Angenommen

## Kategorie

App / Model

## Zweck

NovaOS definiert ein einheitliches App-Modell für ausführbare Nutzerkomponenten.

Eine App ist dabei die vom Nutzer wahrgenommene ausführbare Einheit. Ihre technische Umsetzung kann ein klassisches monolithisches Programm, eine Solution oder eine andere registrierte NovaOS-Ausführungsform sein.

## Grundprinzipien

```text
App ≠ Program
App ≠ Solution
App ≠ Process
App ≠ Package
App ≠ Capability
App Identity ≠ Path
App Visibility ≠ Authority
```

## Modell

```text
App
├── AppID
├── Type
├── Name
├── Version
├── EntryPoint
├── ExecutionModel
├── Capabilities
├── Resources
├── State
└── TrustState
```

`AppID` identifiziert eine App stabil und unabhängig von Installationspfad, Anzeigename oder laufenden Prozessen.

## App-Typen

NovaOS kann unterschiedliche technische App-Modelle unter einer gemeinsamen Benutzerperspektive darstellen:

```text
App
├── Program
├── Solution
├── System App
├── Web App
├── Compatibility App
└── Registered App Provider
```

Der konkrete Typ bestimmt Packaging, Ausführung und Lifecycle.

## Programme

Ein Program ist eine klassische weitgehend eigenständige Anwendung:

```text
App
 ↓
Program Package
 ↓
Executable / Runtime
 ↓
Processes
```

Programme können eigene Bibliotheken, Ressourcen und private `SYS`-Abhängigkeiten besitzen.

## Solutions

Eine Solution ist eine deklarative Zusammenstellung aus UI, Logic Graph, Capabilities und optionaler NovaLang-Logik:

```text
App
 ↓
Solution
├── UI
├── Logic Graph
├── Capabilities
└── Resources
```

Eine Solution ist kein monolithisches Programmpaket.

## App-Instanz

Der Start einer App erzeugt einen Ausführungskontext:

```text
App Identity
     ↓
Launch
     ↓
App Instance
├── Security Context
├── Namespace Context
├── Processes / Tasks
├── Resource Budget
└── Runtime State
```

Mehrere Instanzen derselben App dürfen gleichzeitig existieren.

## Capabilities

Apps erhalten keinen impliziten Vollzugriff auf Benutzer- oder Systemressourcen.

```text
App Requirements
      ∩
Granted Capabilities
      ∩
Policy
      =
Effective Authority
```

Die technische Herkunft der App darf das Capability-Modell nicht umgehen.

## Daten und Zustand

App-Identität, ausführbarer Inhalt und Nutzerdaten bleiben getrennt.

```text
App
├── Package
├── Settings
├── State
├── Cache
└── User Data References
```

Nutzerdaten sollen über ObjectIDs, autorisierte Handles oder andere NovaOS-Ressourcenreferenzen eingebunden werden.

## Lifecycle

Apps besitzen einen kontrollierten Lifecycle:

```text
Discover
 ↓
Register
 ↓
Launch
 ↓
Running
 ↓
Suspend / Resume
 ↓
Terminate
```

Installation, Update und Entfernung hängen vom jeweiligen App-Typ ab und sind nicht zwingend Voraussetzung für die Ausführung jeder App.

## Integration

Apps können NovaOS-Systemfunktionen über registrierte Schnittstellen nutzen:

```text
Capabilities
Services
Semantic Types
File Formats
Protocols
Notifications
Sharing
Search
Generated UI
```

Die Integration basiert auf stabilen IDs und Verträgen, nicht auf fest verdrahteten Anwendungspfaden.

## Normative Anforderungen

1. Jede registrierte App MUSS eine stabile `AppID` besitzen.
2. `AppID` MUSS unabhängig von Name und Pfad sein.
3. App, Program, Solution, Package und Process MÜSSEN getrennte Konzepte bleiben.
4. NovaOS MUSS unterschiedliche App-Typen unter einem gemeinsamen App-Modell darstellen können.
5. Der Start einer App MUSS einen expliziten Ausführungskontext erzeugen.
6. Mehrere Instanzen derselben App MÜSSEN unterstützt werden können.
7. Apps DÜRFEN keine implizite Authority aus ihrer Installation oder Sichtbarkeit erhalten.
8. Systemzugriffe MÜSSEN über autorisierte NovaOS-Mechanismen erfolgen.
9. App-Daten und App-Identität MÜSSEN getrennt bleiben.
10. Programme und Solutions DÜRFEN ihre jeweiligen nativen Ausführungsmodelle behalten.
11. Compatibility Apps DÜRFEN das native Sicherheitsmodell nicht umgehen.
12. AppID, Typ, Zustand, Ausführungskontext und Trust State MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-PROGRAM-PACKAGE-0001`
- `NPSPEC-SOLUTION-ID-0001`
- `NPSPEC-SOLUTION-PACKAGE-0001`
- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS besitzt ein übergeordnetes App-Modell, das klassische Programme, Solutions und weitere ausführbare Modelle unter einer gemeinsamen Nutzerperspektive zusammenführt. Die jeweilige technische Architektur bleibt erhalten, während Identität, Lifecycle, Sicherheit, Ressourcensteuerung und Systemintegration einheitlich behandelt werden.