# NPSPEC-UI-COMPOSITOR-0001 – Nova Compositor

## Status

Angenommen

## Kategorie

UI / Compositor / Rendering / Graphics

## Zweck

Der Nova Compositor setzt die sichtbaren Surfaces des Window Servers zu finalen Frames zusammen.

Er übernimmt insbesondere:

```text
Surface Composition
Z-Order
Clipping
Transforms
Transparency
Effects
Damage Processing
Frame Scheduling
Direct Scanout
```

Der Compositor verwaltet keine Anwendungslogik und keine Display-Hardware direkt.

## Architektur

```text
Applications
     ↓
Surfaces
     ↓
Window Server
     ↓
Nova Compositor
     ↓
Display Server
     ↓
Display
```

Window Server, Compositor und Display Server bleiben logisch getrennte Komponenten.

## Composition

Der Compositor erhält eine geordnete Menge sichtbarer Surfaces.

```text
Background
    ↓
Application Surfaces
    ↓
System UI
    ↓
Overlay
    ↓
Cursor
    ↓
Final Frame
```

Nur tatsächlich sichtbare Inhalte müssen in den finalen Frame einbezogen werden.

## Composition Tree

Die darzustellende Szene wird als Composition Tree beschrieben.

```text
Display
 ├── Desktop
 ├── Window A
 │    └── Surface A
 ├── Window B
 │    └── Surface B
 ├── System UI
 └── Cursor
```

Knoten können Transformationen, Sichtbarkeit, Clipping und weitere Darstellungsinformationen besitzen.

## Surface Buffer

Anwendungsinhalte werden über kontrollierte Buffer bereitgestellt.

```text
Application
    ↓
Render Buffer
    ↓
Surface
    ↓
Compositor
```

Der Compositor erhält nur die für Composition notwendigen Zugriffsrechte.

Eine Anwendung erhält keinen Zugriff auf die Buffer anderer Anwendungen.

## Damage Tracking

Nur geänderte Bereiche sollen neu verarbeitet werden.

```text
Surface Damage
      ↓
Visible Damage
      ↓
Composition Damage
      ↓
Updated Frame
```

Verdeckte oder unveränderte Bereiche können übersprungen werden.

## Frame Scheduling

Der Compositor koordiniert die Frame-Erzeugung mit dem Display Server.

```text
Surface Updates
      ↓
Frame Scheduling
      ↓
Composition
      ↓
Presentation
```

Unnötige Frames sollen vermieden werden.

Die Darstellung soll sich an tatsächlichen Änderungen und Presentation Deadlines orientieren.

## GPU Composition

GPU-beschleunigte Composition wird bevorzugt.

Typische Operationen sind:

```text
Scaling
Rotation
Alpha Blending
Clipping
Transforms
Blur
Shadows
Color Conversion
```

Der Compositor darf geeignete GPU- oder Accelerator-Provider verwenden.

## Software Fallback

Grundlegende Composition muss auch ohne funktionierende GPU-Beschleunigung möglich bleiben.

```text
GPU Composition
      ↓ failure
Software Composition
      ↓
Display Server
```

Komplexe visuelle Effekte dürfen dabei reduziert oder deaktiviert werden.

## Direct Scanout

Kann eine Surface direkt auf einem Display dargestellt werden, darf Composition übersprungen werden.

```text
Surface
   ↓
Direct Scanout
   ↓
Display Server
```

Dies ist beispielsweise bei exklusivem Fullscreen möglich.

Direct Scanout ist eine Optimierung und darf Sicherheits-, Farb- oder Displayanforderungen nicht umgehen.

## Zero-Copy

Buffer sollen möglichst ohne unnötige Kopien zwischen Anwendung, Compositor und Display Server übertragen werden.

```text
Application Buffer
        ↓
Shared / GPU Buffer
        ↓
Compositor
        ↓
Scanout
```

Zero-Copy darf nur verwendet werden, wenn Isolation und Speicherzugriffe korrekt durchgesetzt werden können.

## Effekte

Visuelle Effekte werden als Composition-Eigenschaften behandelt.

Beispiele:

```text
Transparency
Blur
Shadow
Animation Transform
Rounded Clipping
Opacity
```

Effekte dürfen abhängig von Performance-, Energie- oder Hardwarebedingungen reduziert werden, sofern keine harte UI-Anforderung verletzt wird.

## Geschützte Inhalte

Bestimmte Surfaces können als geschützt markiert werden.

Der Compositor muss deren Security Policy bei:

```text
Composition
Screenshots
Screen Recording
Remote Display
Mirroring
```

berücksichtigen.

Eine sichtbare Surface bedeutet nicht automatisch, dass ihr Inhalt ausgelesen oder aufgezeichnet werden darf.

## Fehlerbehandlung

Fehlerhafte Surfaces dürfen den Compositor nicht destabilisieren.

Ungültige Buffer oder Composition-Daten werden verworfen oder durch einen definierten Fehlerzustand ersetzt.

```text
Invalid Surface
      ↓
Reject / Isolate
      ↓
Composition continues
```

## Normative Anforderungen

1. Der Compositor MUSS Surfaces zu finalen Frames zusammensetzen können.
2. Composition MUSS anhand einer definierten Z-Order erfolgen.
3. Anwendungen DÜRFEN keine fremden Surface Buffer lesen.
4. Damage Tracking SOLL unnötige Composition vermeiden.
5. Frame Scheduling SOLL mit Presentation Timing koordiniert werden.
6. GPU-Beschleunigung SOLL verwendet werden, wenn verfügbar und geeignet.
7. Grundlegende Composition MUSS einen Software-Fallback besitzen.
8. Direct Scanout SOLL möglich sein, wenn keine Composition erforderlich ist.
9. Zero-Copy SOLL verwendet werden, wenn Isolation und Hardware dies erlauben.
10. Visuelle Effekte DÜRFEN unter Ressourcenknappheit kontrolliert reduziert werden.
11. Geschützte Surfaces MÜSSEN ihre Security Policy während der gesamten Composition behalten.
12. Fehler einzelner Surfaces DÜRFEN den Compositor nicht zum Absturz bringen.

## Abhängigkeiten

- `NPSPEC-UI-WINDOWSERVER-0001`
- `NPSPEC-UI-DISPLAYSERVER-0001`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CAPABILITY-0001`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

Der Nova Compositor bildet die Rendering-Brücke zwischen Window Server und Display Server:

```text
Surfaces
   ↓
Composition Tree
   ↓
Nova Compositor
   ↓
Final Frame / Direct Scanout
   ↓
Display Server
```

Damit bleiben Anwendungsrendering, Fensterverwaltung, Composition und physische Display-Ausgabe voneinander getrennt und können unabhängig optimiert oder ersetzt werden.