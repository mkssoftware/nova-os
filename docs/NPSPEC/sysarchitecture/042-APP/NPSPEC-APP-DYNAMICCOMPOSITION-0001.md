# NPSPEC-APP-DYNAMICCOMPOSITION-0001 – Nova Dynamic App Composition

## Status

Angenommen

## Kategorie

App / Dynamic Composition

## Zweck

NovaOS definiert die dynamische Zusammensetzung von Apps zur Laufzeit.

Eine App darf benötigte Components, Services, Capabilities und UI-Bestandteile abhängig von Aufgabe, Intent, Objekten, Kontext und verfügbaren Systemressourcen zusammensetzen, ohne dass ihre gesamte Funktionalität statisch im App-Paket enthalten sein muss.

## Grundprinzipien

```text
App ≠ Static Component Set
Composition ≠ Installation
Composition ≠ Authority
CapabilityID ≠ Provider
Dynamic Selection ≠ Uncontrolled Execution
UI Composition ≠ Security Boundary
Recomposition ≠ New App Identity
```

## Modell

```text
DynamicComposition
├── AppID
├── CompositionID
├── Requirements[]
├── Components[]
├── Services[]
├── Capabilities[]
├── UIElements[]
├── Bindings[]
└── ExecutionContract
```

Die Zusammensetzung beschreibt eine konkrete Ausprägung der App für den aktuellen Kontext.

## Komposition

```text
Intent / Task / Object
        +
App Requirements
        +
Context
        ↓
Discovery
        ↓
Candidate Components
Capabilities
Services
UI Elements
        ↓
Validation
        ↓
Composition
        ↓
Execution
```

Die App-Identität bleibt unabhängig von der aktuell verwendeten Komposition erhalten.

## Dynamische Auswahl

NovaOS darf Bestandteile anhand von:

```text
Semantic Compatibility
User Intent
Object Type
Workspace Context
Trust
Policy
Available Authority
Resource Budget
Device Capabilities
Execution Location
User Preferences
```

auswählen.

Explizite Benutzerentscheidungen haben Vorrang vor adaptiver Optimierung, sofern keine höheren Sicherheits- oder Systemgrenzen entgegenstehen.

## Rekonfiguration

Eine bestehende Komposition darf zur Laufzeit angepasst werden.

```text
Current Composition
        ↓
Context Change
        ↓
Re-evaluate
        ↓
Validate
        ↓
Recompose
        ↓
Continue
```

Dabei dürfen Components oder Provider hinzugefügt, entfernt oder ersetzt werden.

Sicherheitsrelevante Änderungen müssen vor Aktivierung erneut validiert werden.

## UI-Komposition

Die sichtbare Oberfläche darf aus den aktuell relevanten Funktionen entstehen.

```text
Available Operations
        +
Task Context
        ↓
UI Composition
```

Position und Darstellung dürfen angepasst werden, ohne die zugrunde liegenden Capabilities oder deren Authority zu verändern.

## Authority

Dynamische Komposition erzeugt keine Berechtigungen.

```text
Composition Requirements
        ∩
Existing Authority
        ∩
Policy
        ∩
Execution Contract
        =
Effective Authority
```

Neu eingebundene Bestandteile erhalten ausschließlich die für ihre konkrete Funktion autorisierte Authority.

## Ausführung

Bestandteile einer Komposition dürfen unterschiedlich ausgeführt werden:

```text
In-Process
Out-of-Process
Sandboxed
System Service
Accelerated
Remote
```

Der Ausführungsort darf geändert werden, sofern Vertrag, Sicherheit und Semantik erhalten bleiben.

## Fehler und Fallback

Bei Ausfall oder Nichtverfügbarkeit eines Bestandteils darf NovaOS:

```text
Alternative Provider
Recomposition
Graceful Degradation
Retry
User Selection
Failure
```

verwenden.

Ein Fallback darf keine Sicherheits- oder Hard Constraints abschwächen.

## Normative Anforderungen

1. NovaOS MUSS Apps zur Laufzeit aus mehreren funktionalen Bestandteilen zusammensetzen können.
2. AppID und konkrete Komposition MÜSSEN getrennt bleiben.
3. Dynamische Komposition DARF keine Authority erzeugen.
4. Components, Services und Capabilities MÜSSEN vor Verwendung validiert werden.
5. Provider MÜSSEN austauschbar sein können, sofern ihre Verträge kompatibel sind.
6. Rekonfiguration MUSS während der Laufzeit möglich sein können.
7. Sicherheitsrelevante Änderungen MÜSSEN vor Aktivierung erneut geprüft werden.
8. User Intent und Kontext DÜRFEN die Komposition beeinflussen.
9. Explizite Benutzerentscheidungen MÜSSEN gegenüber adaptiver Optimierung berücksichtigt werden.
10. Alternative Provider DÜRFEN nur unter Einhaltung aller Hard Constraints verwendet werden.
11. Rekonfiguration DARF die stabile App-Identität nicht verändern.
12. Aktuelle Komposition, Provider, Ausführungsorte und effektive Authority MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-APP-COMPONENT-0001`
- `NPSPEC-APP-SERVICE-0001`
- `NPSPEC-APP-CAPABILITYCOMPOSED-0001`
- `NPSPEC-APP-TASKCENTRIC-0001`
- `NPSPEC-APP-INTENTCENTRIC-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann Apps dynamisch aus passenden Components, Services, Capabilities und UI-Bestandteilen zusammensetzen und bei verändertem Kontext neu konfigurieren. Die App-Identität bleibt stabil, während Implementierung, Provider und Ausführungsort flexibel bleiben und weiterhin den bestehenden Sicherheits-, Authority- und Execution-Contract-Grenzen unterliegen.