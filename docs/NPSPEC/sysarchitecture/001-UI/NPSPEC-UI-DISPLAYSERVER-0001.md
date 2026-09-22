# NPSPEC-UI-DISPLAYSERVER-0001 – Nova Display Server

## Status

Angenommen

## Kategorie

UI / Display / Graphics / Hardware Abstraction

## Zweck

Der Nova Display Server stellt die kontrollierte Verbindung zwischen dem UI-System und physischen oder virtuellen Ausgabegeräten her.

Er verwaltet insbesondere:

```text
Displays
Display Modes
Framebuffer / Scanout
Refresh
Scaling
Orientation
Display Topology
Color Properties
Virtual Displays
```

Der Display Server ist von Window Management und Composition getrennt.

## Architektur

```text
Applications
     ↓
Window Server / Compositor
     ↓
Nova Display Server
     ↓
Display Driver / GPU
     ↓
Physical Display
```

Anwendungen greifen nicht direkt auf physische Displays zu.

## Verantwortungsgrenze

Der Display Server ist für die **Ausgabe auf Displays** zuständig.

Der Window Server bleibt verantwortlich für:

```text
Windows
Surfaces
Z-Order
Focus
Input Routing
Composition Policy
```

Der Display Server kennt daher keine Anwendungsfenster als primäres Konzept.

## Display Object

Jedes erkannte Display wird als eindeutiges Systemobjekt repräsentiert.

```text
DisplayID
Connector
Supported Modes
Current Mode
Physical Size
Scaling
Orientation
Refresh Rate
Color Information
State
```

`DisplayID` bleibt von Anzeigename oder Anschlussbezeichnung getrennt.

## Display Modes

Ein Display kann mehrere Modi anbieten.

```text
1920×1080 @ 60 Hz
2560×1440 @ 144 Hz
3840×2160 @ 120 Hz
```

Ein Mode beschreibt mindestens:

```text
Width
Height
Refresh Rate
Pixel Format
Timing
```

Nur vom Display- und Treiberpfad unterstützte Modi dürfen aktiviert werden.

## Display Topology

Mehrere Displays werden als explizite Topologie verwaltet.

Beispiel:

```text
┌──────────────┐ ┌──────────────┐
│ Display A    │ │ Display B    │
│ 1920×1080    │ │ 2560×1440    │
└──────────────┘ └──────────────┘
```

Unterstützt werden mindestens:

```text
Extended
Mirrored
Single Display
Virtual Display
```

Die logische Position eines Displays ist von seiner physischen Verbindung unabhängig.

## Scaling

Scaling wird pro Display beschrieben.

```text
Display A → 100 %
Display B → 150 %
Display C → 200 %
```

Der Display Server stellt die benötigten Display-Informationen bereit.

Die eigentliche UI-Skalierung bleibt Aufgabe höherer UI-Schichten.

## Scanout

Der Compositor übergibt einen fertigen Frame oder geeignete Scanout-Ressourcen an den Display Server.

```text
Compositor
    ↓
Frame / Buffer
    ↓
Display Server
    ↓
Scanout
    ↓
Display
```

Direkter Scanout darf verwendet werden, wenn keine Composition erforderlich ist und die Sicherheitsbedingungen erfüllt sind.

## Synchronisation

Frame-Ausgabe muss mit den Eigenschaften des Displays koordiniert werden.

Der Display Server unterstützt dafür Mechanismen wie:

```text
VSync
Presentation Timing
Frame Completion
Refresh Tracking
```

Variable Refresh Rate kann verwendet werden, sofern Display und Treiber dies unterstützen.

## Hotplug

Displays können zur Laufzeit erscheinen oder verschwinden.

```text
Connect
   ↓
Detect
   ↓
Enumerate
   ↓
Configure
```

und:

```text
Disconnect
   ↓
Invalidate Display
   ↓
Reconfigure Topology
```

Ein Displayverlust darf den UI-Stack nicht zum Absturz bringen.

## Virtuelle Displays

Virtuelle Ausgabegeräte werden über dieselbe grundlegende Display-Abstraktion unterstützt.

Beispiele:

```text
Remote Desktop
Virtual Machine
Screen Streaming
Testing
Headless Session
```

Für höhere UI-Schichten soll möglichst kein grundlegend anderer Ausgabepfad notwendig sein.

## Sicherheit

Display-Zugriff wird über Capabilities kontrolliert.

Normale Anwendungen erhalten keinen direkten Zugriff auf:

```text
Display Configuration
Raw Scanout
Physical Connectors
Other Sessions
Protected Display Content
```

Privilegierte Display-Operationen benötigen explizite Autorisierung.

## Fallback

NovaOS muss einen grundlegenden Display-Pfad ohne vollständige GPU-Beschleunigung bereitstellen können.

```text
GPU Driver
   ↓ failure
Basic Display Driver
   ↓
Framebuffer / Simple Scanout
```

Damit bleiben Boot, Recovery und grundlegende Benutzerinteraktion möglich.

## Normative Anforderungen

1. Physische Displays MÜSSEN über den Display Server abstrahiert werden.
2. Anwendungen DÜRFEN physische Displays nicht direkt kontrollieren.
3. Displays MÜSSEN stabile `DisplayID`s besitzen.
4. Unterstützte Display Modes MÜSSEN explizit ermittelt werden.
5. Mehrere Displays MÜSSEN unterstützt werden.
6. Scaling MUSS pro Display beschreibbar sein.
7. Display Hotplug MUSS zur Laufzeit unterstützt werden.
8. Der Display Server MUSS physische und virtuelle Displays unterstützen können.
9. Scanout MUSS über kontrollierte Buffer-Übergaben erfolgen.
10. Display-Konfiguration MUSS capability-basiert geschützt werden.
11. Der Verlust eines Displays DARF den UI-Stack nicht zum Absturz bringen.
12. Ein grundlegender Fallback-Displaypfad MUSS verfügbar sein.

## Abhängigkeiten

- `NPSPEC-UI-WINDOWSERVER-0001`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CAPABILITY-0001`
- `ADR-DRIVER-0001`
- `ADR-VERIFY-0005_Capability_Safety`

## Ergebnis

Der Nova Display Server bildet die hardwareorientierte Ausgabeschicht des UI-Stacks:

```text
Window Server / Compositor
           ↓
     Display Server
           ↓
   Display Driver
           ↓
        Display
```

Damit bleiben Fensterverwaltung und Composition von konkreter Display-Hardware getrennt, während NovaOS physische, virtuelle und zukünftige Ausgabegeräte über eine gemeinsame Display-Abstraktion verwalten kann.