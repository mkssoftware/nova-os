# NPSPEC-APP-SANDBOX-0001 – Nova App Sandbox

## Status

Angenommen

## Kategorie

App / Security / Sandbox

## Zweck

NovaOS definiert eine isolierte Sandbox für Apps und deren Ausführung.

Die Sandbox begrenzt, welche Ressourcen eine App sehen, verwenden und beeinflussen kann. Sie ergänzt das Capability-Modell um technische Isolation und verhindert, dass Fehler oder kompromittierte Apps außerhalb ihres autorisierten Ausführungskontexts wirken.

## Grundprinzipien

```text
Sandbox ≠ Permission
Sandbox ≠ Capability
Sandbox ≠ Trust
Sandbox ≠ Process
Isolation ≠ Authority
Visibility ≠ Authority
App Identity ≠ Sandbox Identity
```

## Modell

```text
AppSandbox
├── SandboxID
├── AppID
├── SecurityContext
├── NamespaceContext
├── CapabilitySet
├── ResourceBudget
├── IsolationProfile
└── State
```

Eine App-Instanz erhält beim Start einen definierten Sandbox-Kontext.

## Isolation

Die Sandbox kann insbesondere isolieren:

```text
Memory
Processes
Threads
Filesystem
Namespace
IPC
Devices
Network
Temporary Data
Runtime State
System Interfaces
```

Die konkrete Stärke der Isolation richtet sich nach App-Typ, Trust State, Policy und Execution Contract.

## Namespace

Eine App sieht nur den für sie erzeugten Namespace:

```text
Global Namespace
      ↓
Policy
      ↓
App Namespace
├── Authorized Objects
├── App Resources
├── Private SYS Overlay
└── Explicit Projections
```

Ein sichtbares Objekt erzeugt dabei keine zusätzliche Authority.

## Authority

Die effektive Authority einer App ergibt sich aus:

```text
Granted Capabilities
        ∩
Policy
        ∩
Sandbox Restrictions
        ∩
Execution Contract
        =
Effective Authority
```

Die Sandbox darf Authority weiter einschränken, aber nicht eigenständig erweitern.

## Ressourcen

Die Sandbox darf Ressourcenlimits durchsetzen für:

```text
CPU
Memory
Storage
I/O
Network
GPU
Processes
Threads
Handles
Temporary Data
```

Diese Limits werden mit der Nova Resource Economy und dem Execution Contract koordiniert.

## Kommunikation

Kommunikation über Sandbox-Grenzen muss kontrolliert erfolgen.

```text
App Sandbox
     ↓
Authorized IPC / Service / Capability
     ↓
External Resource
```

Handles und Capabilities dürfen nur über explizit autorisierte Übergaben übertragen werden.

## App Components

Components einer App dürfen innerhalb derselben Sandbox oder in zusätzlichen isolierten Sub-Kontexten ausgeführt werden.

```text
App Sandbox
├── Component A
├── Component B
└── Isolated Component C
```

Ein isolierter Component darf gegenüber der App weiter reduzierte Authority besitzen.

## Fehlerisolation

Fehler innerhalb einer Sandbox sollen außerhalb liegende Apps und Systemkomponenten nicht beeinträchtigen.

NovaOS darf eine fehlerhafte Sandbox:

```text
Suspend
Restrict
Restart
Terminate
Quarantine
```

ohne Beendigung anderer unabhängiger Apps behandeln.

## Trust

Trust darf die Wahl des Isolation Profiles beeinflussen.

```text
Higher Trust
    ≠
More Authority
```

Auch vertrauenswürdige Apps erhalten keine Berechtigungen allein aufgrund ihres Trust States.

Unbekannte oder nicht vertrauenswürdige Apps dürfen mit strengeren Isolationseinstellungen ausgeführt werden.

## Normative Anforderungen

1. Jede isolierte App-Instanz MUSS einem definierten Sandbox-Kontext zugeordnet sein.
2. `SandboxID` und `AppID` MÜSSEN getrennte Identitäten bleiben.
3. Sandbox und Capability-Authority MÜSSEN getrennte Sicherheitsmechanismen bleiben.
4. Eine Sandbox DARF Authority nur einschränken, nicht eigenständig erweitern.
5. Speicher-, Namespace- und Prozessisolation MÜSSEN unterstützt werden.
6. Ressourcenlimits MÜSSEN pro Sandbox durchsetzbar sein.
7. Kommunikation über Sandbox-Grenzen MUSS kontrolliert erfolgen.
8. Capability- und Handle-Übertragungen MÜSSEN explizit autorisiert sein.
9. Components DÜRFEN zusätzliche Isolation innerhalb einer App erhalten.
10. Sandbox-Fehler SOLLEN auf den betroffenen Kontext begrenzt bleiben.
11. Trust DARF Isolation beeinflussen, aber keine zusätzliche Authority erzeugen.
12. SandboxID, Isolation Profile, Ressourcenverbrauch, Zustand und effektive Authority MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-APP-COMPONENT-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-NAMESPACE-APPLICATION-0001`
- `NPSPEC-NAMESPACE-PERMISSION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS isoliert Apps in kontrollierten Sandbox-Kontexten, deren Namespace, Ressourcen und Kommunikationswege explizit begrenzt sind. Die Sandbox ergänzt die Capability-basierte Sicherheit, ohne selbst Authority zu erzeugen, und begrenzt Fehler sowie kompromittierte Apps auf ihren jeweiligen Ausführungskontext.