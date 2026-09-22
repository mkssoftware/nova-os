# NPSPEC-UI-ACCESSIBILITYTREE-0001 – Accessibility Tree

## Status

Angenommen

## Kategorie

UI / Accessibility / Semantics / Assistive Technology

## Zweck

NovaOS stellt die semantische Struktur der Benutzeroberfläche über einen **Accessibility Tree** bereit.

Assistive Technologien sollen nicht aus Pixeln oder Renderdaten ableiten müssen, welche Bedienelemente vorhanden sind.

```text
UI Tree
   ↓
Accessibility Semantics
   ↓
Accessibility Tree
   ↓
Assistive Technology
```

Der Accessibility Tree beschreibt Bedeutung, Zustand und mögliche Aktionen von UI-Elementen unabhängig von ihrer visuellen Darstellung.

## Architektur

```text
Application UI
      ↓
UI Tree
      ↓
Accessibility Tree
      ↓
Accessibility Service
      ↓
Screen Reader / Assistive Technology
```

Scene Graph und Accessibility Tree bleiben getrennt.

```text
Scene Graph         → visuelle Darstellung
Accessibility Tree  → semantische Bedienstruktur
```

## Accessibility Node

Ein zugängliches UI-Element wird als Accessibility Node repräsentiert.

Ein Node kann mindestens enthalten:

```text
AccessibilityID
Role
Name
Description
Value
State
Actions
Parent
Children
Bounds
```

Nicht jedes visuelle Scene-Element benötigt einen eigenen Accessibility Node.

## Rollen

UI-Elemente besitzen semantische Rollen.

Beispiele:

```text
Window
Button
Text
TextField
Checkbox
RadioButton
Slider
Menu
MenuItem
List
ListItem
Table
Image
Link
Document
```

Eigene Controls müssen ihre Semantik auf geeignete Accessibility-Rollen abbilden können.

## Zustände

Accessibility Nodes können relevante Zustände veröffentlichen.

Beispiele:

```text
Focused
Selected
Checked
Expanded
Collapsed
Disabled
ReadOnly
Required
Hidden
```

Zustandsänderungen werden als semantische Änderungen gemeldet.

## Aktionen

Assistive Technologien können unterstützte Aktionen ermitteln.

Beispiele:

```text
Activate
Focus
Select
Expand
Collapse
Increment
Decrement
SetValue
Scroll
```

Eine Aktion wird nicht durch simulierte Mausbewegungen ausgeführt, wenn eine direkte semantische Operation verfügbar ist.

## Beziehungen

Der Accessibility Tree bildet logische Beziehungen ab.

```text
Window
 ├── Toolbar
 │    ├── Back Button
 │    └── Search Field
 └── Document
      ├── Heading
      ├── Paragraph
      └── Link
```

Diese Struktur darf von der visuellen Scene-Graph-Hierarchie abweichen.

## Fokus

Accessibility Focus und normaler Input Focus werden unterschieden.

```text
Input Focus
Accessibility Focus
```

Beide können miteinander koordiniert werden, müssen aber nicht identisch sein.

## Dynamische Änderungen

Änderungen an der UI werden inkrementell in den Accessibility Tree übernommen.

```text
UI Change
    ↓
Semantic Change
    ↓
Accessibility Event
```

Typische Events sind:

```text
Node Added
Node Removed
Focus Changed
Value Changed
State Changed
Selection Changed
Structure Changed
```

Ein vollständiger Neuaufbau des Trees soll nicht für jede Änderung erforderlich sein.

## Virtuelle Inhalte

Große oder virtualisierte UI-Strukturen müssen nicht vollständig gleichzeitig materialisiert werden.

Beispiele:

```text
Large Lists
Tables
File Views
Documents
Virtualized Controls
```

Nicht materialisierte Bereiche können über semantische Navigation und Lazy Materialization zugänglich gemacht werden.

## Custom Controls

Eigene UI-Komponenten müssen ihre Semantik explizit beschreiben können.

```text
Custom Visual Control
        ↓
Accessibility Adapter
        ↓
Semantic Node
```

Ein Custom Control ohne Standarddarstellung darf dadurch trotzdem vollständig zugänglich sein.

## Sicherheit

Der Accessibility Tree kann sensible Informationen enthalten.

Zugriff erfolgt deshalb über kontrollierte Capabilities.

```text
Assistive Technology
        ↓
Accessibility Capability
        ↓
Accessibility Service
```

Eine Accessibility-Berechtigung darf nicht automatisch uneingeschränkten Zugriff auf alle anderen Systemressourcen erzeugen.

Secure UI kann zusätzliche Einschränkungen definieren.

## Remote und Automation

Accessibility-Semantik darf auch für autorisierte:

```text
Assistive Technology
UI Automation
Testing
Remote Interaction
```

verwendet werden.

Accessibility und allgemeine Automation bleiben jedoch getrennte Autoritätsbereiche.

## Performance

Accessibility darf den normalen Rendering-Pfad nicht blockieren.

```text
UI Update
 ├── Rendering Path
 └── Accessibility Update
```

Semantische Aktualisierungen sollen inkrementell und ereignisbasiert erfolgen.

## Normative Anforderungen

1. Native NovaOS-UI MUSS semantische Accessibility-Informationen bereitstellen können.
2. Accessibility Tree und Scene Graph MÜSSEN logisch getrennt bleiben.
3. Accessibility Nodes MÜSSEN Rollen, Zustände und Aktionen beschreiben können.
4. UI-Elemente SOLLEN stabile Accessibility IDs besitzen.
5. Custom Controls MÜSSEN eigene Accessibility-Semantik bereitstellen können.
6. Änderungen SOLLEN inkrementell als Accessibility Events veröffentlicht werden.
7. Assistive Technologien SOLLEN direkte semantische Aktionen verwenden können.
8. Große virtualisierte Inhalte MÜSSEN ohne vollständige Materialisierung zugänglich gemacht werden können.
9. Accessibility Focus MUSS unabhängig vom normalen Input Focus darstellbar sein.
10. Zugriff auf Accessibility-Daten MUSS capability-basiert kontrolliert werden.
11. Secure UI MUSS sensible Accessibility-Informationen schützen können.
12. Accessibility-Verarbeitung DARF den Rendering-Pfad nicht unnötig blockieren.
13. Accessibility-Semantik SOLL für autorisierte Tests und Automation wiederverwendbar sein.

## Abhängigkeiten

- `NPSPEC-UI-WINDOWSERVER-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-RETAINEDMODE-0001`
- `NPSPEC-UI-INPUTROUTING-0001`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-CAPABILITY-0001`
- `ADR-VERIFY-0005_Capability_Safety`

## Ergebnis

NovaOS behandelt Accessibility als semantische Eigenschaft der UI und nicht als nachträgliche Analyse der Darstellung:

```text
UI Element
    ↓
Semantic Meaning
    ↓
Accessibility Tree
    ↓
Assistive Technology
```

Damit können Screenreader und andere assistive Technologien direkt mit der Bedeutung und den Aktionen der Benutzeroberfläche arbeiten, unabhängig davon, wie diese visuell gerendert wird.