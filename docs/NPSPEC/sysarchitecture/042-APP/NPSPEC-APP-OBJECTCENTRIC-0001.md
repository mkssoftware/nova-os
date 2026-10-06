# NPSPEC-APP-OBJECTCENTRIC-0001 – Nova Object-Centric App

## Status

Angenommen

## Kategorie

App / Object-Centric

## Zweck

NovaOS definiert ein objektzentriertes App-Modell, bei dem stabile Systemobjekte und ihre möglichen Operationen im Mittelpunkt stehen.

Der Nutzer arbeitet mit einem Objekt und seiner Bedeutung. Geeignete Funktionen, Capabilities und Oberflächen werden abhängig von Objekttyp, Kontext und Benutzerabsicht bereitgestellt.

## Grundprinzipien

```text
Object ≠ App
Object ≠ File
Object ≠ Path
ObjectID ≠ Location
Object Type ≠ Owning Program
Operation ≠ Provider
Object Access ≠ Authority
```

## Modell

```text
Object
├── ObjectID
├── SemanticType
├── Properties
├── Relations[]
├── State
└── AvailableOperations[]
```

Ein Objekt kann beispielsweise sein:

```text
Document
Image
Contact
Message
Device
Media
Dataset
Project
Calendar Entry
System Resource
NovaFile
```

Die konkrete physische Repräsentation ist nicht Teil der Objektidentität.

## Objektzentrierte Interaktion

```text
Object
   +
Semantic Type
   +
User Intent
   +
Context
      ↓
Available Operations
      ↓
Capability Resolution
      ↓
UI + Execution
```

NovaOS bestimmt damit nicht zuerst eine App, sondern zunächst die für das Objekt sinnvollen Operationen.

## Operationen

Operationen werden semantisch beschrieben.

Beispiel:

```text
Image
├── View
├── Edit
├── Crop
├── Convert
├── Share
└── Print
```

Die jeweilige Operation kann durch unterschiedliche Capability-Provider implementiert werden.

```text
Operation
   ↓
CapabilityID
   ↓
Compatible Providers
   ↓
Selected Provider
```

## Beziehungen

Objekte dürfen über persistente Beziehungen miteinander verbunden sein.

```text
Object A
   ↓ Relation
Object B
```

Diese Beziehungen basieren auf stabilen `ObjectID`s und nicht auf Dateipfaden.

Dadurch können objektzentrierte Oberflächen zusammengehörige Informationen unabhängig von ihrem Speicherort darstellen.

## Kontext

Verfügbare Operationen dürfen beeinflusst werden durch:

```text
Workspace
User Intent
Current Task
Object State
Available Capabilities
Authority
Policy
Trust
Device Resources
```

Nicht relevante Funktionen können zurücktreten, ohne die Objektidentität oder Daten zu verändern.

## UI

Eine objektzentrierte Oberfläche darf dynamisch aus den für das Objekt verfügbaren Funktionen aufgebaut werden.

```text
Object
   ↓
Semantic Model
   ↓
Relevant Capabilities
   ↓
Generated / Declarative UI
```

Die UI ist eine Darstellung des Arbeitskontexts und nicht Eigentümer des Objekts.

## Programme und Solutions

Klassische Programme und Solutions bleiben vollständig unterstützt.

Sie können:

```text
Objects anzeigen
Objects bearbeiten
Operations bereitstellen
Capabilities bereitstellen
Object Relations verwenden
```

Ein Objekt muss jedoch keinem bestimmten Programm gehören.

## Sicherheit

Das Erkennen oder Anzeigen eines Objekts erzeugt keine Authority.

```text
Object Reference
      +
Requested Operation
      +
Existing Authority
      +
Policy
      =
Authorized Operation
```

Provider erhalten nur die für die konkrete Operation erforderlichen autorisierten Handles und Capabilities.

## Normative Anforderungen

1. NovaOS MUSS Objekte unabhängig von Apps und Speicherpfaden identifizieren können.
2. Objektidentität MUSS auf stabilen `ObjectID`s basieren.
3. Objekte MÜSSEN semantische Typen besitzen können.
4. Operationen MÜSSEN unabhängig von konkreten Providern beschreibbar sein.
5. Mehrere Provider für dieselbe Operation MÜSSEN unterstützt werden können.
6. Objekte DÜRFEN keinem bestimmten Programm gehören müssen.
7. Objektbeziehungen SOLLEN über stabile ObjectIDs dargestellt werden.
8. User Intent und Kontext DÜRFEN die angebotenen Operationen beeinflussen.
9. Dynamische und generierte Oberflächen MÜSSEN möglich sein.
10. Sichtbarkeit oder Kenntnis eines Objekts DARF keine Authority erzeugen.
11. Provider DÜRFEN nur die für ihre Operation erforderliche Authority erhalten.
12. ObjectID, Semantic Type, Operation, Provider und effektive Authority MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-APP-MODEL-0001`
- `NPSPEC-APP-CAPABILITYCOMPOSED-0001`
- `NPSPEC-APP-DOCUMENTCENTRIC-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-RELATION-0001`
- `NPSPEC-CAPABILITY-ID-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-WORKSPACE-MODEL-0001`

## Ergebnis

NovaOS stellt Objekte und ihre semantisch möglichen Operationen in den Mittelpunkt. Apps, Solutions und Capability-Provider werden zu austauschbaren Werkzeugen für diese Objekte, während Identität, Beziehungen, Daten und Benutzerkontext unabhängig von einzelnen Anwendungen erhalten bleiben.