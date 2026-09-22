# NPSPEC-UI-GPUCOMPOSITION-0001 – GPU Composition

## Status

Angenommen

## Kategorie

UI / Compositor / GPU / Rendering

## Zweck

NovaOS nutzt die GPU bevorzugt für die Composition der Benutzeroberfläche.

Die GPU übernimmt geeignete Operationen wie:

```text
Surface Composition
Scaling
Transforms
Alpha Blending
Clipping
Blur
Shadows
Color Conversion
Animation
```

GPU Composition ist eine Optimierung des Compositors und keine Voraussetzung für die grundsätzliche Funktionsfähigkeit der UI.

## Architektur

```text
Scene Graph
     ↓
Compositor
     ↓
GPU Composition Backend
     ↓
GPU Driver
     ↓
Display Server
```

Der Compositor bleibt von einer konkreten GPU oder Grafik-API unabhängig.

## GPU Backend

Hardwareabhängige Funktionen werden hinter einem GPU Composition Backend gekapselt.

```text
Compositor
    ↓
GPU Backend
    ├── GPU A
    ├── GPU B
    └── Software Backend
```

Der Austausch einer GPU darf keine Änderung am Scene Graph oder Window Server erfordern.

## Composition Pipeline

Ein Frame wird aus der aktuellen Scene Generation erzeugt.

```text
Scene Generation
      ↓
Visible Nodes
      ↓
Damage Regions
      ↓
Composition Plan
      ↓
GPU Commands
      ↓
Final Frame
```

Nur für den Frame relevante Ressourcen sollen verarbeitet werden.

## GPU Resources

Surfaces können über GPU-kompatible Buffer bereitgestellt werden.

```text
Application
     ↓
GPU / Shared Buffer
     ↓
Compositor
     ↓
GPU
```

Buffer müssen explizite Eigentums-, Zugriffs- und Synchronisationsregeln besitzen.

## Zero-Copy

Wenn technisch möglich, sollen Surface Buffer ohne zusätzliche Kopie direkt für Composition oder Scanout verwendet werden.

```text
Application Buffer
        ↓
GPU Composition
        ↓
Display Scanout
```

Zero-Copy darf Isolation und Speicherzugriffsschutz nicht abschwächen.

## Synchronisation

Der Zugriff auf gemeinsam verwendete GPU-Ressourcen muss explizit synchronisiert werden.

Unterstützt werden müssen geeignete Mechanismen für:

```text
Render Complete
Buffer Ready
Composition Complete
Presentation Complete
```

CPU-seitiges Busy Waiting soll vermieden werden.

## Damage-Based Composition

GPU Composition soll mit Damage Tracking kombiniert werden.

```text
Scene Damage
     ↓
Affected Surfaces
     ↓
Required Composition
```

Eine vollständige Neuberechnung des Frames soll vermieden werden, wenn nur Teilbereiche verändert wurden.

## Direct Scanout

Kann eine Surface ohne Composition direkt dargestellt werden, darf das GPU-Composition-Stadium übersprungen werden.

```text
Surface
   ↓
Direct Scanout
   ↓
Display Server
```

Direct Scanout wird gegenüber unnötiger Composition bevorzugt, sofern alle Anforderungen erfüllt sind.

## Effekte

Visuelle Effekte werden durch den Compositor angefordert und vom Backend umgesetzt.

Beispiele:

```text
Acrylic / Blur
Transparency
Shadow
Rounded Clipping
Transforms
Transitions
Animations
```

Das GPU Backend entscheidet nicht selbst über UI-Design oder Window-Policy.

## Ressourcensteuerung

GPU Composition unterliegt der systemweiten Ressourcensteuerung.

Relevant sind insbesondere:

```text
GPU Time
VRAM
Memory Bandwidth
Power
Thermal Budget
Frame Deadline
```

UI-Reaktionsfähigkeit und Frame Deadlines sollen gegenüber rein dekorativen Effekten priorisiert werden.

## Degradation

Bei Ressourcenknappheit dürfen optionale Effekte reduziert werden.

Beispiel:

```text
Full Effects
     ↓
Reduced Blur
     ↓
Reduced Animation
     ↓
Basic Composition
```

Die funktionale Bedienbarkeit der Oberfläche muss erhalten bleiben.

## Software Fallback

Fällt GPU Composition aus, muss auf Software Composition gewechselt werden können.

```text
GPU Failure
    ↓
Software Compositor
    ↓
Display Server
```

Der Wechsel darf keinen vollständigen Ausfall der Benutzeroberfläche erzwingen.

## Multi-GPU

Bei mehreren GPUs kann NovaOS einen geeigneten Composition Provider auswählen.

```text
Integrated GPU
Discrete GPU
Virtual GPU
Software Provider
```

Die Auswahl kann Performance, Energiebedarf, Display-Zuordnung und verfügbare Fähigkeiten berücksichtigen.

## Sicherheit

GPU-Ressourcen unterschiedlicher Anwendungen müssen voneinander isoliert bleiben.

Der Compositor darf nur auf Buffer zugreifen, die ihm explizit für Composition bereitgestellt wurden.

```text
Application Memory
       ≠
Compositor Authority
```

GPU Composition darf keine Umgehung von Capability-, Memory- oder Surface-Isolation ermöglichen.

## Normative Anforderungen

1. NovaOS SOLL GPU Composition bevorzugen, wenn geeignete Hardware verfügbar ist.
2. Der Compositor MUSS von konkreter GPU-Hardware unabhängig bleiben.
3. GPU-Funktionen MÜSSEN über austauschbare Backends bereitgestellt werden.
4. Surface Buffer MÜSSEN explizite Zugriffs- und Synchronisationsregeln besitzen.
5. Zero-Copy SOLL verwendet werden, wenn Sicherheit und Hardware dies erlauben.
6. Damage Tracking SOLL unnötige GPU-Arbeit vermeiden.
7. Direct Scanout SOLL unnötiger Composition vorgezogen werden.
8. GPU Composition MUSS Ressourcenbudgets berücksichtigen.
9. Optionale Effekte MÜSSEN bei Ressourcenknappheit reduzierbar sein.
10. UI-Funktionalität MUSS gegenüber dekorativen Effekten priorisiert werden.
11. Ein Software-Fallback MUSS verfügbar sein.
12. GPU-Speicher unterschiedlicher Security Contexts MUSS isoliert bleiben.
13. Multi-GPU-Systeme SOLLEN unterschiedliche Composition Provider unterstützen können.

## Abhängigkeiten

- `NPSPEC-UI-DISPLAYSERVER-0001`
- `NPSPEC-UI-COMPOSITOR-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-RETAINEDMODE-0001`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0006_Zero_Copy`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-CAPABILITY-0001`

## Ergebnis

NovaOS verwendet GPU Composition nach dem Prinzip:

```text
Scene Graph
    ↓
Damage + Composition Plan
    ↓
GPU when useful
    ↓
Direct Scanout when possible
    ↓
Software fallback when necessary
```

Damit kann NovaOS eine moderne, flüssige und visuell anspruchsvolle Oberfläche bereitstellen, ohne die grundlegende UI-Funktionalität von einer bestimmten GPU oder Grafikarchitektur abhängig zu machen.