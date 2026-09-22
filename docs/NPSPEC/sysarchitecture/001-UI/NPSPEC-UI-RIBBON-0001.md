# NPSPEC-UI-RIBBON-0001 – Nova Ribbon

## Status

Angenommen

## Kategorie

UI / Navigation / Commands / Semantic UI

## Zweck

NovaOS verwendet ein kontextabhängiges Ribbon als mögliche Darstellung umfangreicher Funktionsgruppen.

Das Ribbon ist keine fest verdrahtete Werkzeugleiste, sondern eine **semantische Projektion verfügbarer Aktionen und Capabilities**.

```text
Current Context
      ↓
Available Actions
      ↓
Capabilities
      ↓
Ribbon Model
      ↓
Responsive Representation
```

## Grundprinzip

Das Ribbon zeigt bevorzugt Funktionen, die im aktuellen Arbeitskontext sinnvoll sind.

```text
Selected Object: Image

Ribbon
 ├── Edit
 ├── Crop
 ├── Rotate
 ├── Adjust
 └── Export
```

Ändert sich der Kontext:

```text
Selected Object: Text

Ribbon
 ├── Format
 ├── Paragraph
 ├── Insert
 └── Review
```

## Architektur

```text
Semantic UI
     +
Current Context
     +
Available Capabilities
     ↓
Ribbon Model
     ↓
Declarative UI
     ↓
Responsive Layout
```

## Ribbon Model

Ein Ribbon besteht logisch aus:

```text
Ribbon
 ├── Section
 │    ├── Group
 │    │    ├── Action
 │    │    ├── Action
 │    │    └── Control
 │    └── Group
 └── Contextual Section
```

Die Struktur beschreibt Funktionen und Beziehungen, nicht deren exakte Pixelposition.

## Sections

Sections gruppieren größere Funktionsbereiche.

Beispiele:

```text
Home
Insert
Layout
Review
View
Developer
```

Anwendungen und Systemmodule dürfen eigene semantische Sections bereitstellen.

## Groups

Zusammengehörige Aktionen werden gruppiert.

Beispiel:

```text
Edit
 ├── Cut
 ├── Copy
 ├── Paste
 └── Delete
```

Gruppen sollen nach Funktion und nicht nach Implementierungsmodul gebildet werden.

## Kontextuelle Sections

Bestimmte Sections erscheinen nur bei passendem Kontext.

```text
Image Selected
      ↓
Image Tools

Table Selected
      ↓
Table Tools
```

Kontextuelle Funktionen verschwinden wieder, wenn ihr Kontext nicht mehr besteht.

## Capability Integration

Ribbon-Aktionen können direkt aus verfügbaren Capabilities entstehen.

```text
Selected Object
      ↓
Semantic Type
      ↓
Capability Discovery
      ↓
Applicable Actions
      ↓
Ribbon
```

Beispiel:

```text
Arduino Project
      ↓
Code.Edit
Build
Flash
Serial.Monitor
```

Damit können integrierte NovaOS-Capabilities Funktionen in einen gemeinsamen Arbeitskontext einbringen.

## Provider-Unabhängigkeit

Das Ribbon bindet Aktionen bevorzugt an semantische Capabilities statt an konkrete Programme.

```text
Ribbon Action
      ↓
Image.Edit
      ↓
Selected Provider
```

Ein Providerwechsel muss daher nicht automatisch die Oberfläche verändern.

## Responsive Verhalten

Das Ribbon passt sich dem verfügbaren Raum an.

```text
Expanded
↓
Icon + Label + Groups

Medium
↓
Compact Groups

Small
↓
Priority Actions + Overflow
```

Funktionen dürfen in ein Overflow-Menü verschoben werden, statt unkontrolliert abgeschnitten zu werden.

## Prioritäten

Aktionen können eine Darstellungspriorität besitzen.

```text
Primary
Secondary
Contextual
Rare
```

Bei wenig Platz bleiben wichtige Funktionen bevorzugt sichtbar.

Priorität bedeutet keine zusätzliche Systemberechtigung.

## Adaptive Integration

Adaptive UI darf die Präsentation häufig verwendeter Aktionen optimieren.

Sie darf jedoch keine Funktionen unvorhersehbar entfernen oder deren Bedeutung verändern.

```text
Usage Pattern
     ↓
Adaptive Presentation
     ↓
Stable Semantic Action
```

Explizite Nutzeranpassungen haben Vorrang.

## Nutzeranpassung

Der Nutzer darf unterstützte Bereiche des Ribbons anpassen.

Beispiele:

```text
Pin Action
Unpin Action
Reorder Group
Collapse Section
Show Preferred Actions
```

Systemkritische Regeln und geschützte UI-Bereiche bleiben davon unberührt.

## Zustände

Ribbon-Aktionen können Zustände besitzen:

```text
Enabled
Disabled
Selected
Active
Pending
Unavailable
```

Der Zustand muss aus dem tatsächlichen System- oder Anwendungskontext abgeleitet werden.

## Eingabemethoden

Ribbon-Aktionen müssen mit unterschiedlichen Eingabemethoden verwendbar sein.

```text
Mouse
Keyboard
Touch
Pen
Accessibility
```

Die Darstellung darf sich entsprechend anpassen.

## Accessibility

Ribbon-Struktur und Aktionen müssen semantisch im Accessibility Tree erscheinen.

```text
Ribbon
 ├── Section
 ├── Group
 └── Action
```

Navigation darf nicht ausschließlich von visueller Position abhängig sein.

## Sicherheit

Die Anzeige einer Ribbon-Aktion erzeugt keine Berechtigung.

```text
Ribbon Action
      ↓
Capability Request
      ↓
Authorization
      ↓
Execution
```

Wird eine Capability widerrufen, muss der entsprechende UI-Zustand aktualisiert werden.

## Normative Anforderungen

1. Das Nova Ribbon MUSS semantisch und deklarativ beschreibbar sein.
2. Ribbon-Aktionen SOLLEN auf semantischen Actions oder Capabilities basieren.
3. Sections und Groups SOLLEN Funktionen logisch gruppieren.
4. Kontextuelle Sections MÜSSEN dynamisch ein- und ausgeblendet werden können.
5. Das Ribbon MUSS auf Änderungen verfügbarer Capabilities reagieren können.
6. Ribbon-Aktionen SOLLEN von konkreten Capability Providern entkoppelt sein.
7. Das Ribbon MUSS responsive Darstellungen unterstützen.
8. Bei Platzmangel SOLLEN niedrig priorisierte Aktionen kontrolliert komprimiert oder in Overflow-Strukturen verschoben werden.
9. Adaptive Anpassungen DÜRFEN die semantische Bedeutung einer Aktion nicht verändern.
10. Explizite Nutzeranpassungen MÜSSEN gegenüber adaptiven Anpassungen Vorrang besitzen.
11. Ribbon-Aktionen MÜSSEN im Accessibility Tree repräsentierbar sein.
12. Die Anzeige einer Aktion DARF keine Autorisierung implizieren.
13. Capability-Änderungen MÜSSEN in den Ribbon-Zustand übernommen werden.
14. Das Ribbon DARF nicht Voraussetzung für die Ausführung einer Capability sein.

## Abhängigkeiten

- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-ADAPTIVE-0001`
- `NPSPEC-UI-CAPABILITYDRIVEN-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-RESPONSIVE-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `ADR-CAPABILITY-0001`
- `ADR-ARCH-0008_Semantic_Types`

## Ergebnis

Das Nova Ribbon ist keine statische Sammlung von Buttons, sondern eine kontextabhängige Darstellung verfügbarer Systemfunktionen:

```text
Context
   ↓
Semantic Actions
   ↓
Available Capabilities
   ↓
Prioritize + Group
   ↓
Responsive Ribbon
```

Damit kann NovaOS komplexe Funktionen übersichtlich bereitstellen, ohne Anwendungen an starre Menüs, konkrete Programme oder feste Bildschirmgrößen zu binden.