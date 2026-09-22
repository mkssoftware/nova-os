# NPSPEC-UI-CAPABILITYDRIVEN-0001 – Capability-Driven UI

## Status

Angenommen


## Kategorie

UI / Capabilities / Declarative / Security

## Zweck

NovaOS erzeugt und aktiviert UI-Funktionen anhand tatsächlich verfügbarer und autorisierter **Capabilities**.

Die Oberfläche soll nicht statisch voraussetzen, dass eine bestimmte Anwendung oder ein bestimmter Dienst vorhanden ist.

```text
User Intent
    ↓
Required Capability
    ↓
Capability Discovery
    ↓
Authorized Provider
    ↓
UI Function
```

## Grundprinzip

Die UI fragt nach einer Funktion, nicht nach einem bestimmten Programm.

```text
Nicht:

Open PhotoEditor.exe

Sondern:

Capability:
Image.Edit
```

NovaOS ermittelt geeignete Capability Provider.

## Architektur

```text
Declarative UI
      ↓
Capability Requirement
      ↓
Capability Registry
      ↓
Policy / Permission Check
      ↓
Provider Selection
      ↓
Capability Binding
```

Die UI bleibt dadurch von konkreten Implementierungen entkoppelt.

## Capability Requirement

Ein UI-Element kann eine benötigte Capability deklarieren.

Beispiele:

```text
Document.Print
Image.Edit
Media.Play
File.Share
Text.Translate
Object.Encrypt
```

Zusätzliche Anforderungen können über einen Execution Contract beschrieben werden.

```text
Capability
+ Semantic Types
+ Trust Requirements
+ Resource Constraints
+ Data Sovereignty
```

## Capability Discovery

Die UI Runtime darf verfügbare Provider über das Capability Registry ermitteln.

```text
Required Capability
       ↓
Registry
       ↓
Provider A
Provider B
Provider C
```

Die Existenz eines Providers bedeutet noch nicht, dass dessen Nutzung autorisiert ist.

## UI-Aktivierung

UI-Funktionen können abhängig von verfügbaren Capabilities dargestellt werden.

```text
Capability Available + Authorized
            ↓
        UI Enabled
```

Fehlt eine Capability:

```text
Capability Missing
      ↓
Disable / Hide / Alternative
```

Das Verhalten muss für die jeweilige UI-Funktion definiert sein.

## Dynamische Provider

Capabilities dürfen zur Laufzeit verfügbar oder nicht verfügbar werden.

Beispiele:

```text
Device connected
Service started
Remote provider available
Permission revoked
Provider crashed
```

Die UI muss darauf reagieren können.

```text
Capability State Change
        ↓
Declarative UI State
        ↓
Reconciliation
```

Ein vollständiger Neustart der Anwendung soll dafür nicht erforderlich sein.

## Provider-Unabhängigkeit

UI-Komponenten dürfen nicht unnötig an einen konkreten Provider gekoppelt werden.

```text
UI
 ↓
Capability Contract
 ├── Provider A
 ├── Provider B
 └── Provider C
```

Provider können ausgetauscht werden, solange sie den erforderlichen Vertrag erfüllen.

## Semantische Typen

Capabilities verwenden semantisch definierte Ein- und Ausgaben.

```text
Image Object
    ↓
Image.Edit
    ↓
Modified Image Object
```

Die UI muss dadurch keine providerspezifischen Datenformate kennen.

## Berechtigungen

Eine sichtbare UI-Funktion erzeugt keine Berechtigung.

```text
UI Action
    ↓
Capability Request
    ↓
Authorization
    ↓
Execution
```

Fehlt die erforderliche Autorisierung, darf die Capability nicht ausgeführt werden.

## Capability Delegation

Eine Anwendung darf nur Capabilities an UI-Komponenten delegieren, die sie selbst kontrolliert delegieren darf.

```text
Application Capability
        ↓
Attenuation
        ↓
UI Component
```

Delegierte Rechte sollen auf den tatsächlich benötigten Umfang begrenzt werden.

## Remote Capabilities

Capability Provider dürfen lokal oder entfernt ausgeführt werden.

```text
UI
 ↓
Capability
 ├── Local Provider
 └── Remote Provider
```

Location Transparency darf Sicherheits-, Trust- oder Data-Sovereignty-Regeln nicht umgehen.

## Fehlerbehandlung

Fällt ein Provider aus:

```text
Provider Failure
      ↓
Capability Invalid
      ↓
UI Reconciliation
      ↓
Alternative / Disabled State
```

Die UI darf nicht dauerhaft von einem nicht mehr verfügbaren Provider abhängig bleiben.

Falls möglich, kann ein kompatibler Provider neu gebunden werden.

## Adaptive UI

Adaptive UI darf verfügbare Capabilities berücksichtigen.

```text
Context
   +
Available Capabilities
   ↓
Adaptive UI
```

Adaptive Entscheidungen dürfen jedoch weder Berechtigungen erzeugen noch Sicherheitsregeln umgehen.

## Legacy Integration

Auch Legacy-Anwendungen können über Adapter als Capability Provider eingebunden werden.

```text
Legacy Application
       ↓
Compatibility Adapter
       ↓
Nova Capability
       ↓
Native UI
```

Damit können Legacy-Funktionen in moderne NovaOS-Workflows integriert werden, ohne deren UI vollständig übernehmen zu müssen.

## Normative Anforderungen

1. NovaOS-UI MUSS Funktionen anhand semantischer Capabilities beschreiben können.
2. UI-Komponenten SOLLEN nicht unnötig an konkrete Provider gekoppelt sein.
3. Capability Discovery MUSS über definierte Systemmechanismen erfolgen.
4. Sichtbarkeit einer UI-Funktion DARF keine Autorisierung implizieren.
5. Capability-Nutzung MUSS vor der Ausführung autorisiert werden.
6. Capability Provider MÜSSEN austauschbar sein können, wenn ihre Verträge kompatibel sind.
7. Änderungen der Capability-Verfügbarkeit MÜSSEN dynamisch in den UI-Zustand überführbar sein.
8. Fehlende Capabilities MÜSSEN kontrolliert dargestellt oder durch Alternativen ersetzt werden können.
9. Delegierte Capabilities MÜSSEN auf den erforderlichen Umfang begrenzt werden können.
10. Semantische Typen SOLLEN die Schnittstelle zwischen UI und Capability Provider bestimmen.
11. Remote Capabilities DÜRFEN Trust-, Security- oder Data-Sovereignty-Regeln nicht umgehen.
12. Adaptive UI DARF aus Capability-Verfügbarkeit keine zusätzlichen Berechtigungen ableiten.
13. Provider-Ausfälle MÜSSEN ohne Ausfall der gesamten UI behandelt werden können.
14. Legacy-Funktionen DÜRFEN über kontrollierte Adapter als Capability Provider integriert werden.

## Abhängigkeiten

- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-ADAPTIVE-0001`
- `ADR-CAPABILITY-0001`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-COMPAT-0015_Legacy_Application_als_Capability_Provider`

## Ergebnis

NovaOS verbindet Benutzeroberflächen mit Funktionen über Capabilities statt über fest gekoppelte Anwendungen:

```text
User Intent
    ↓
UI Action
    ↓
Capability Requirement
    ↓
Discover + Authorize
    ↓
Select Provider
    ↓
Execute
    ↓
Update UI State
```

Damit kann dieselbe NovaOS-Oberfläche unterschiedliche lokale, entfernte oder kompatibilitätsbasierte Provider verwenden, ohne ihre grundlegende UI-Architektur ändern zu müssen.

## Kategorie

UI / Capabilities / Declarative / Security

## Zweck

NovaOS erzeugt und aktiviert UI-Funktionen anhand tatsächlich verfügbarer und autorisierter **Capabilities**.

Die Oberfläche soll nicht statisch voraussetzen, dass eine bestimmte Anwendung oder ein bestimmter Dienst vorhanden ist.

```text
User Intent
    ↓
Required Capability
    ↓
Capability Discovery
    ↓
Authorized Provider
    ↓
UI Function
```

## Grundprinzip

Die UI fragt nach einer Funktion, nicht nach einem bestimmten Programm.

```text
Nicht:

Open PhotoEditor.exe

Sondern:

Capability:
Image.Edit
```

NovaOS ermittelt geeignete Capability Provider.

## Architektur

```text
Declarative UI
      ↓
Capability Requirement
      ↓
Capability Registry
      ↓
Policy / Permission Check
      ↓
Provider Selection
      ↓
Capability Binding
```

Die UI bleibt dadurch von konkreten Implementierungen entkoppelt.

## Capability Requirement

Ein UI-Element kann eine benötigte Capability deklarieren.

Beispiele:

```text
Document.Print
Image.Edit
Media.Play
File.Share
Text.Translate
Object.Encrypt
```

Zusätzliche Anforderungen können über einen Execution Contract beschrieben werden.

```text
Capability
+ Semantic Types
+ Trust Requirements
+ Resource Constraints
+ Data Sovereignty
```

## Capability Discovery

Die UI Runtime darf verfügbare Provider über das Capability Registry ermitteln.

```text
Required Capability
       ↓
Registry
       ↓
Provider A
Provider B
Provider C
```

Die Existenz eines Providers bedeutet noch nicht, dass dessen Nutzung autorisiert ist.

## UI-Aktivierung

UI-Funktionen können abhängig von verfügbaren Capabilities dargestellt werden.

```text
Capability Available + Authorized
            ↓
        UI Enabled
```

Fehlt eine Capability:

```text
Capability Missing
      ↓
Disable / Hide / Alternative
```

Das Verhalten muss für die jeweilige UI-Funktion definiert sein.

## Dynamische Provider

Capabilities dürfen zur Laufzeit verfügbar oder nicht verfügbar werden.

Beispiele:

```text
Device connected
Service started
Remote provider available
Permission revoked
Provider crashed
```

Die UI muss darauf reagieren können.

```text
Capability State Change
        ↓
Declarative UI State
        ↓
Reconciliation
```

Ein vollständiger Neustart der Anwendung soll dafür nicht erforderlich sein.

## Provider-Unabhängigkeit

UI-Komponenten dürfen nicht unnötig an einen konkreten Provider gekoppelt werden.

```text
UI
 ↓
Capability Contract
 ├── Provider A
 ├── Provider B
 └── Provider C
```

Provider können ausgetauscht werden, solange sie den erforderlichen Vertrag erfüllen.

## Semantische Typen

Capabilities verwenden semantisch definierte Ein- und Ausgaben.

```text
Image Object
    ↓
Image.Edit
    ↓
Modified Image Object
```

Die UI muss dadurch keine providerspezifischen Datenformate kennen.

## Berechtigungen

Eine sichtbare UI-Funktion erzeugt keine Berechtigung.

```text
UI Action
    ↓
Capability Request
    ↓
Authorization
    ↓
Execution
```

Fehlt die erforderliche Autorisierung, darf die Capability nicht ausgeführt werden.

## Capability Delegation

Eine Anwendung darf nur Capabilities an UI-Komponenten delegieren, die sie selbst kontrolliert delegieren darf.

```text
Application Capability
        ↓
Attenuation
        ↓
UI Component
```

Delegierte Rechte sollen auf den tatsächlich benötigten Umfang begrenzt werden.

## Remote Capabilities

Capability Provider dürfen lokal oder entfernt ausgeführt werden.

```text
UI
 ↓
Capability
 ├── Local Provider
 └── Remote Provider
```

Location Transparency darf Sicherheits-, Trust- oder Data-Sovereignty-Regeln nicht umgehen.

## Fehlerbehandlung

Fällt ein Provider aus:

```text
Provider Failure
      ↓
Capability Invalid
      ↓
UI Reconciliation
      ↓
Alternative / Disabled State
```

Die UI darf nicht dauerhaft von einem nicht mehr verfügbaren Provider abhängig bleiben.

Falls möglich, kann ein kompatibler Provider neu gebunden werden.

## Adaptive UI

Adaptive UI darf verfügbare Capabilities berücksichtigen.

```text
Context
   +
Available Capabilities
   ↓
Adaptive UI
```

Adaptive Entscheidungen dürfen jedoch weder Berechtigungen erzeugen noch Sicherheitsregeln umgehen.

## Legacy Integration

Auch Legacy-Anwendungen können über Adapter als Capability Provider eingebunden werden.

```text
Legacy Application
       ↓
Compatibility Adapter
       ↓
Nova Capability
       ↓
Native UI
```

Damit können Legacy-Funktionen in moderne NovaOS-Workflows integriert werden, ohne deren UI vollständig übernehmen zu müssen.

## Normative Anforderungen

1. NovaOS-UI MUSS Funktionen anhand semantischer Capabilities beschreiben können.
2. UI-Komponenten SOLLEN nicht unnötig an konkrete Provider gekoppelt sein.
3. Capability Discovery MUSS über definierte Systemmechanismen erfolgen.
4. Sichtbarkeit einer UI-Funktion DARF keine Autorisierung implizieren.
5. Capability-Nutzung MUSS vor der Ausführung autorisiert werden.
6. Capability Provider MÜSSEN austauschbar sein können, wenn ihre Verträge kompatibel sind.
7. Änderungen der Capability-Verfügbarkeit MÜSSEN dynamisch in den UI-Zustand überführbar sein.
8. Fehlende Capabilities MÜSSEN kontrolliert dargestellt oder durch Alternativen ersetzt werden können.
9. Delegierte Capabilities MÜSSEN auf den erforderlichen Umfang begrenzt werden können.
10. Semantische Typen SOLLEN die Schnittstelle zwischen UI und Capability Provider bestimmen.
11. Remote Capabilities DÜRFEN Trust-, Security- oder Data-Sovereignty-Regeln nicht umgehen.
12. Adaptive UI DARF aus Capability-Verfügbarkeit keine zusätzlichen Berechtigungen ableiten.
13. Provider-Ausfälle MÜSSEN ohne Ausfall der gesamten UI behandelt werden können.
14. Legacy-Funktionen DÜRFEN über kontrollierte Adapter als Capability Provider integriert werden.

## Abhängigkeiten

- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-ADAPTIVE-0001`
- `ADR-CAPABILITY-0001`
- `ADR-ARCH-0007_Location_Transparency`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-SECURITY-0016_Data_Sovereignty_als_Hard_Constraint`
- `ADR-COMPAT-0015_Legacy_Application_als_Capability_Provider`

## Ergebnis

NovaOS verbindet Benutzeroberflächen mit Funktionen über Capabilities statt über fest gekoppelte Anwendungen:

```text
User Intent
    ↓
UI Action
    ↓
Capability Requirement
    ↓
Discover + Authorize
    ↓
Select Provider
    ↓
Execute
    ↓
Update UI State
```

Damit kann dieselbe NovaOS-Oberfläche unterschiedliche lokale, entfernte oder kompatibilitätsbasierte Provider verwenden, ohne ihre grundlegende UI-Architektur ändern zu müssen.