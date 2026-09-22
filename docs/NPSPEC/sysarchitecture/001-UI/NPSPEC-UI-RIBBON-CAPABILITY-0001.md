# NPSPEC-UI-RIBBON-CAPABILITY-0001 – Ribbon Capability Integration

## Status

Angenommen

## Kategorie

UI / Ribbon / Capabilities / Semantic Actions

## Zweck

NovaOS ermöglicht Capabilities, ihre für den aktuellen Kontext relevanten Aktionen kontrolliert in das Nova Ribbon einzubringen.

```text
Capability
    ↓
Semantic Actions
    ↓
Ribbon Contribution
    ↓
Ribbon Model
```

Das Ribbon kennt dabei nicht die interne Implementierung eines Capability Providers.

## Grundprinzip

Ein Provider liefert keine fertige Ribbon-Oberfläche, sondern beschreibt seine verfügbaren Aktionen semantisch.

```text
Nicht:

Provider → eigener Toolbar-Code

Sondern:

Provider
   ↓
Capability Actions
   ↓
Ribbon Contribution
   ↓
Nova UI Runtime
```

Darstellung, Layout und Integration bleiben unter Kontrolle von NovaOS.

## Ribbon Contribution

Eine Capability kann eine Ribbon Contribution veröffentlichen.

Diese kann enthalten:

```text
ContributionID
CapabilityID
ActionID
Semantic Role
Label
Icon Reference
Group
Priority
Context Requirements
State
```

Die Contribution beschreibt eine Funktion, nicht deren endgültige Position oder Darstellung.

## Beispiel

```text
Capability:
Image.Edit

Actions:
- Crop
- Rotate
- Resize
- Adjust
```

NovaOS kann daraus erzeugen:

```text
Image Tools
 ├── Edit
 │    ├── Crop
 │    ├── Rotate
 │    └── Resize
 └── Adjust
```

## Kontextbindung

Eine Contribution kann Bedingungen definieren.

```text
Required Object Type: Image
Required Selection: 1
Required Capability: Image.Edit
```

Nur wenn der Kontext passt, wird die Aktion berücksichtigt.

```text
Context
   ↓
Match Requirements
   ↓
Applicable Contributions
```

## Capability Discovery

Ribbon Contributions werden über das Capability-System ermittelt.

```text
Current Object
      ↓
Semantic Type
      ↓
Capability Discovery
      ↓
Applicable Providers
      ↓
Ribbon Contributions
```

Damit können Funktionen dynamisch erscheinen, wenn neue Capabilities verfügbar werden.

## Provider-Unabhängigkeit

Mehrere Provider können dieselbe Capability bereitstellen.

```text
Image.Edit
 ├── Provider A
 ├── Provider B
 └── Provider C
```

Das Ribbon soll deshalb primär die semantische Aktion darstellen:

```text
Crop
```

und nicht:

```text
Crop with Provider A
```

Die Provider-Auswahl erfolgt getrennt.

## Action Binding

Eine Ribbon-Aktion bindet an eine semantische Capability-Anforderung.

```text
Ribbon Action
      ↓
Capability Requirement
      ↓
Execution Contract
      ↓
Provider Selection
      ↓
Execution
```

Dadurch kann der Provider gewechselt werden, ohne die Ribbon-Struktur neu definieren zu müssen.

## Gruppierung

Capabilities dürfen bevorzugte Gruppen vorschlagen.

```text
Edit
Format
Insert
Analyze
Export
Developer
```

Die endgültige Gruppierung bestimmt jedoch die Nova UI Runtime.

Provider dürfen keine beliebigen globalen Ribbon-Strukturen erzwingen.

## Priorität

Contributions können eine semantische Priorität angeben.

```text
Primary
Secondary
Contextual
Rare
```

Diese Priorität dient als Hinweis für:

```text
Placement
Responsive Collapse
Overflow
Adaptive Presentation
```

Sie erzeugt keine höhere Systempriorität oder Berechtigung.

## Konflikte

Mehrere Capabilities können ähnliche Aktionen bereitstellen.

```text
Provider A → Rotate
Provider B → Rotate
```

NovaOS soll solche Beiträge anhand von:

```text
Semantic Action
Capability Contract
Context
Provider Selection
```

zusammenführen, statt unnötige doppelte Buttons zu erzeugen.

## Dynamische Änderungen

Capability Contributions können sich zur Laufzeit ändern.

```text
Provider Added
Provider Removed
Capability Revoked
Object Changed
Selection Changed
```

Der Ablauf lautet:

```text
Capability Change
      ↓
Reevaluate Contributions
      ↓
Update Ribbon Model
      ↓
Declarative Reconciliation
```

Ein Neustart der Anwendung ist nicht erforderlich.

## Capability State

Der Zustand einer Capability kann direkt in die Ribbon-Aktion einfließen.

```text
Available
Unavailable
Busy
Pending
Restricted
```

Beispiel:

```text
Image.Export
     ↓
Provider Busy
     ↓
Export Action → Pending
```

## Berechtigungen

Eine Ribbon Contribution ist keine Berechtigung.

```text
Contribution Visible
       ≠
Capability Authorized
```

Bei Ausführung muss die tatsächliche Autorisierung geprüft werden.

```text
Ribbon Action
      ↓
Authorization
      ↓
Capability Execution
```

## Legacy Provider

Legacy-Anwendungen können über Compatibility Adapter Ribbon Contributions bereitstellen.

```text
Legacy Application
       ↓
Capability Adapter
       ↓
Semantic Actions
       ↓
Ribbon Contribution
```

Legacy-Code erhält dadurch keinen direkten Zugriff auf das native Ribbon.

## Remote Provider

Auch Remote Capabilities können Contributions bereitstellen.

```text
Remote Provider
      ↓
Capability
      ↓
Ribbon Contribution
```

Die Darstellung einer Remote Capability darf Trust-, Security- oder Data-Sovereignty-Anforderungen nicht umgehen.

## Normative Anforderungen

1. Capability Provider SOLLEN Ribbon-Funktionen als semantische Contributions beschreiben.
2. Provider DÜRFEN keine eigene unkontrollierte Ribbon-Oberfläche injizieren.
3. Ribbon Contributions MÜSSEN einer Capability oder semantischen Action zugeordnet sein.
4. Contributions DÜRFEN Kontextbedingungen definieren.
5. Die Nova UI Runtime MUSS die endgültige Darstellung und Gruppierung kontrollieren.
6. Ribbon-Aktionen SOLLEN von konkreten Providern entkoppelt sein.
7. Mehrere äquivalente Provider SOLLEN nicht automatisch doppelte UI-Aktionen erzeugen.
8. Capability-Änderungen MÜSSEN dynamisch in das Ribbon übernommen werden können.
9. Contributions DÜRFEN Darstellungsprioritäten vorschlagen.
10. Darstellungspriorität DARF keine zusätzliche Autorität erzeugen.
11. Die Sichtbarkeit einer Contribution DARF keine Capability-Autorisierung implizieren.
12. Capability-Autorisierung MUSS spätestens vor der Ausführung geprüft werden.
13. Legacy Provider DÜRFEN nur über kontrollierte Adapter integrieren.
14. Remote Contributions MÜSSEN denselben Security-, Trust- und Sovereignty-Regeln wie ihre Capability unterliegen.

## Abhängigkeiten

- `NPSPEC-UI-RIBBON-0001`
- `NPSPEC-UI-CAPABILITYDRIVEN-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-RESPONSIVE-0001`
- `ADR-CAPABILITY-0001`
- `ADR-CAPABILITY-0004_Capability_Registry`
- `ADR-CAPABILITY-0005_Capability_Discovery`
- `ADR-EXECUTION-0001_Nova.ExecutionContract`
- `ADR-COMPAT-0015_Legacy_Application_als_Capability_Provider`

## Ergebnis

Capabilities integrieren ihre Funktionen nicht durch eigene UI-Fragmente, sondern über semantische Contributions:

```text
Capability Provider
        ↓
Semantic Actions
        ↓
Context Filtering
        ↓
Ribbon Contribution
        ↓
Nova Ribbon Model
        ↓
Responsive Presentation
```

Damit bleibt das Nova Ribbon systemweit konsistent, während neue lokale, entfernte oder Legacy-basierte Capabilities ihre Funktionen dynamisch in die Oberfläche einbringen können.