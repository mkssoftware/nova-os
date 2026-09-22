# NPSPEC-UI-DESKTOP-0001 – Nova Desktop

## Status

Angenommen

## Kategorie

UI / Desktop / Workspace / System UI

## Zweck

Der Nova Desktop bildet den primären visuellen Arbeitsbereich einer interaktiven NovaOS-Sitzung.

Er verbindet:

```text
Workspace
Windows
System UI
Capabilities
Objects
Notifications
Navigation
```

Der Desktop ist dabei keine monolithische Anwendung, sondern eine System-UI-Komposition auf Basis der NovaOS-UI-Architektur.

## Grundprinzip

```text
Session
   ↓
Desktop Workspace
   ├── Windows
   ├── Objects
   ├── System UI
   ├── Ribbon / Commands
   └── Overlays
```

Der Desktop stellt Arbeitskontext bereit, besitzt aber nicht automatisch die Ressourcen oder Daten der darauf dargestellten Anwendungen.

## Architektur

```text
User Session
     ↓
Nova Desktop
     ↓
Window Server
     ↓
Scene Graph
     ↓
Compositor
     ↓
Display Server
```

Der Desktop verwendet dieselben nativen UI-, Surface- und Composition-Mechanismen wie andere NovaOS-Komponenten.

## Desktop Workspace

Ein Workspace beschreibt den aktuellen Arbeitsbereich.

Er kann enthalten:

```text
Background
Windows
Pinned Objects
Open Objects
Panels
Widgets
Workspace State
```

Workspace-Zustand soll unabhängig von konkreten Displaykoordinaten speicherbar sein.

## System UI

Zum Desktop gehören systemkontrollierte Elemente wie:

```text
Application / Capability Launcher
Task / Window Navigation
Notifications
Quick Settings
System Status
Workspace Controls
Clock
Session Controls
```

Diese Elemente besitzen geschützte System-UI-Rollen.

Normale Anwendungen dürfen System-UI nicht imitieren oder überlagern, wenn dadurch Sicherheitsgrenzen verletzt werden.

## Objektorientierter Desktop

Der Desktop darf NovaOS-Objekte direkt darstellen.

```text
Document
Image
Folder
Project
Device
Capability
```

Desktop-Objekte referenzieren stabile Objektidentitäten und nicht ausschließlich Dateipfade.

```text
Desktop Item
    ↓
ObjectID
    ↓
Current Object Location
```

Ein verschobenes Objekt kann dadurch weiterhin dasselbe Desktop-Objekt bleiben.

## Öffnen von Objekten

Das Öffnen eines Objekts erfolgt bevorzugt über dessen semantischen Typ und verfügbare Capabilities.

```text
Object
  ↓
Semantic Type
  ↓
Applicable Capability
  ↓
Provider
  ↓
UI
```

Der Desktop muss daher nicht zwingend eine feste Anwendung mit einem Dateityp verknüpfen.

## Fenster

Fenster werden durch den Window Server verwaltet.

Der Desktop darf:

```text
Open
Activate
Arrange
Minimize
Restore
Move between Workspaces
```

anfordern.

Die tatsächliche Fensterautorität verbleibt beim Window Server.

## Workspaces

NovaOS darf mehrere virtuelle Workspaces bereitstellen.

```text
Workspace 1
Workspace 2
Workspace 3
```

Fenster und Arbeitskontexte können einem oder mehreren Workspaces zugeordnet werden.

Der Wechsel eines Workspaces soll keinen Neustart oder Neuaufbau der enthaltenen Anwendungen erfordern.

## Multi-Display

Ein Desktop kann mehrere Displays umfassen.

```text
Desktop Session
 ├── Display A
 ├── Display B
 └── Display C
```

Workspaces dürfen:

```text
Displayübergreifend
Pro Display
Hybrid
```

organisiert werden.

Displayänderungen müssen dynamisch verarbeitet werden.

## Responsive Desktop

System-UI passt sich an:

```text
Display Size
Scaling
Orientation
Input Method
Available Space
Accessibility
```

an.

Die grundlegenden Systemfunktionen müssen unabhängig von der konkreten Darstellung erreichbar bleiben.

## Capability-Driven Desktop

Desktop-Funktionen werden bevorzugt über Capabilities bereitgestellt.

```text
User Intent
    ↓
Desktop Action
    ↓
Capability
    ↓
Authorized Provider
```

Neue Capability Provider können dadurch Funktionen bereitstellen, ohne den Desktop selbst zu verändern.

## Kontextaktionen

Ausgewählte Objekte können kontextbezogene Aktionen bereitstellen.

```text
Selected Object
      ↓
Semantic Type
      ↓
Available Capabilities
      ↓
Context Actions
```

Diese können beispielsweise über:

```text
Ribbon
Context Menu
Command Palette
Keyboard
Touch
```

dargestellt werden.

## Persistenz

NovaOS darf Desktop-Zustand speichern.

Dazu gehören beispielsweise:

```text
Workspace Configuration
Window Placement
Pinned Objects
Panel Configuration
Display Assignment
User Preferences
```

Persistierter Zustand muss gegen aktuell verfügbare Displays, Capabilities und Objekte reconciled werden.

## Wiederherstellung

Nach Neustart oder UI-Ausfall kann NovaOS den letzten gültigen Desktop-Zustand rekonstruieren.

```text
Stored Desired State
       ↓
Current Environment
       ↓
Reconciliation
       ↓
Restored Desktop
```

Nicht mehr verfügbare Ressourcen dürfen die Wiederherstellung nicht blockieren.

## Sicherheit

Der Desktop besitzt keinen pauschalen Zugriff auf Inhalte aller dargestellten Anwendungen.

```text
Desktop
   ≠
Global Application Authority
```

Insbesondere darf er nicht allein aufgrund seiner System-UI-Rolle:

```text
Application Memory lesen
Protected Surfaces lesen
Credentials lesen
Capabilities übernehmen
Security Contexts umgehen
```

## Accessibility

Alle zentralen Desktopfunktionen müssen über den Accessibility Tree erreichbar sein.

Desktopnavigation darf nicht ausschließlich von:

```text
Pointer Position
Visual Effects
Drag & Drop
```

abhängen.

## Fehlerisolation

Der Ausfall einer Anwendung darf den Desktop nicht beenden.

Ebenso sollen einzelne Desktop-Komponenten isoliert wiederherstellbar sein.

```text
Component Failure
      ↓
Isolate
      ↓
Restore Component
      ↓
Continue Session
```

## Normative Anforderungen

1. Der Nova Desktop MUSS als System-UI auf der nativen NovaOS-UI-Architektur aufbauen.
2. Der Desktop DARF nicht als monolithische Anwendung vorausgesetzt werden.
3. Desktop-Objekte SOLLEN stabile NovaOS-Objektidentitäten verwenden.
4. Objektaktionen SOLLEN über semantische Capabilities ermittelt werden.
5. Fensterverwaltung MUSS über den Window Server erfolgen.
6. Mehrere Workspaces SOLLEN unterstützt werden.
7. Multi-Display MUSS dynamisch unterstützt werden.
8. System-UI MUSS gegenüber normalen Anwendungen geschützt sein.
9. Desktop-Zustand SOLL deklarativ persistierbar sein.
10. Persistierter Zustand MUSS mit der aktuellen Umgebung reconciled werden können.
11. Der Desktop DARF aus seiner Systemrolle keine uneingeschränkte Autorität über Anwendungen ableiten.
12. Zentrale Desktopfunktionen MÜSSEN über Accessibility erreichbar sein.
13. Der Ausfall einzelner Anwendungen DARF den Desktop nicht blockieren.
14. Desktop-Komponenten SOLLEN unabhängig wiederherstellbar sein.
15. Responsive und adaptive Darstellung DARF die semantische Erreichbarkeit grundlegender Systemfunktionen nicht verändern.

## Abhängigkeiten

- `NPSPEC-UI-WINDOWSERVER-0001`
- `NPSPEC-UI-DISPLAYSERVER-0001`
- `NPSPEC-UI-COMPOSITOR-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-DECLARATIVE-0001`
- `NPSPEC-UI-ADAPTIVE-0001`
- `NPSPEC-UI-CAPABILITYDRIVEN-0001`
- `NPSPEC-UI-SEMANTIC-0001`
- `NPSPEC-UI-RESPONSIVE-0001`
- `NPSPEC-UI-ACCESSIBILITYTREE-0001`
- `ADR-OBJECT-0001`
- `ADR-CAPABILITY-0001`
- `ADR-STATE-0002_Desired_und_Actual_State`
- `ADR-STATE-0004_State_Reconciliation`

## Ergebnis

Der Nova Desktop bildet eine deklarative, objekt- und capability-orientierte Arbeitsumgebung:

```text
User Session
      ↓
Workspace
      ↓
Objects + Windows + Context
      ↓
Semantic Actions
      ↓
Capabilities
      ↓
NovaOS Services
```

Damit bleibt der Desktop eine leichte Systemoberfläche, während die eigentliche Funktionalität aus unabhängigen NovaOS-Objekten, Capabilities und Systemdiensten zusammengesetzt wird.