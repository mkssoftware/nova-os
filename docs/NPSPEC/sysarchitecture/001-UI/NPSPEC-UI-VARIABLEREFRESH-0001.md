# NPSPEC-UI-VARIABLEREFRESH-0001 – Variable Refresh Rate

## Status

Angenommen

## Kategorie

UI / Display / Presentation / Frame Timing

## Zweck

NovaOS unterstützt **Variable Refresh Rate (VRR)**, damit ein Display seine Bildwiederholrate an die tatsächlich bereitgestellten Frames anpassen kann.

```text
Frame Ready
    ↓
Presentation
    ↓
Display Refresh
```

Dadurch werden unnötige Refresh-Zyklen reduziert und Darstellung, Latenz und Energieverbrauch verbessert.

## Architektur

```text
Frame Scheduler
      ↓
Compositor
      ↓
Display Server
      ↓
VRR-capable Display
```

VRR wird durch den Display Server bereitgestellt und vom Frame Scheduler bei der Presentation-Planung berücksichtigt.

## Capability Discovery

Der Display Server ermittelt für jedes Display:

```text
VRR Supported
Minimum Refresh Rate
Maximum Refresh Rate
Current VRR State
Driver Support
```

VRR darf nur aktiviert werden, wenn der vollständige Display-Pfad die Funktion unterstützt.

## Refresh Range

Ein VRR-Display besitzt einen unterstützten Frequenzbereich.

Beispiel:

```text
48 Hz ─────────────── 144 Hz
  ↑                      ↑
Minimum                Maximum
```

NovaOS muss Frames innerhalb dieses gültigen Bereichs präsentieren.

## Frame-gesteuerte Ausgabe

Bei aktivem VRR bestimmt nicht mehr ausschließlich ein festes Refresh-Raster den Presentation-Zeitpunkt.

```text
Frame Complete
     ↓
Presentation Request
     ↓
Display Refresh
```

Dadurch kann ein fertiger Frame früher dargestellt werden, sofern die Display-Grenzen dies erlauben.

## Statische Inhalte

Bei unverändertem Bild soll die Refresh Rate reduziert werden können.

```text
No Damage
    ↓
Low Refresh Activity
```

Der Display Server darf erforderliche Refresh-Zyklen weiterhin selbst auslösen, wenn dies für das Display notwendig ist.

## Interaktive Inhalte

Bei Eingaben oder Animationen kann die Refresh Rate erhöht werden.

```text
User Input
    ↓
UI Update
    ↓
Frame Ready
    ↓
Higher Refresh Activity
```

Ziel ist eine geringe Input-to-Presentation-Latenz ohne dauerhaft maximale Refresh Rate erzwingen zu müssen.

## Minimum Refresh

Wird die minimale VRR-Frequenz unterschritten, kann NovaOS abhängig von Display und Treiber:

```text
Repeat Frame
Trigger Refresh
Use Frame Duplication
Temporarily Leave VRR
```

Die konkrete Strategie bleibt Aufgabe des Display-/Presentation-Pfades.

## Multi-Display

VRR wird pro Display verwaltet.

```text
Display A → 60 Hz fixed
Display B → 48–144 Hz VRR
Display C → 40–120 Hz VRR
```

Ein VRR-Display darf nicht unnötig an das Refresh-Raster eines anderen Displays gekoppelt werden.

## Fullscreen und Windowed

VRR soll sowohl für:

```text
Exclusive Fullscreen
Borderless Fullscreen
Windowed Composition
```

verwendbar sein, sofern der jeweilige Display- und Composition-Pfad dies unterstützt.

VRR ist damit keine ausschließlich für Spiele vorgesehene Funktion.

## Direct Scanout

VRR kann mit Direct Scanout kombiniert werden.

```text
Application Surface
        ↓
Direct Scanout
        ↓
VRR Presentation
```

Dadurch können Composition-Overhead und Presentation-Latenz weiter reduziert werden.

## Energieoptimierung

VRR kann zur Energieeinsparung verwendet werden.

```text
Static UI
   ↓
Lower Refresh Activity
   ↓
Lower Display / GPU Work
```

Energieoptimierung darf jedoch keine harten Latenz- oder Realtime-Anforderungen verletzen.

## Fallback

Ist VRR nicht verfügbar oder wird es instabil, fällt NovaOS auf einen festen Display Mode zurück.

```text
VRR
 ↓ failure / unsupported
Fixed Refresh Rate
```

Die grundlegende Darstellung darf nicht von VRR abhängen.

## Observability

NovaOS soll mindestens folgende Informationen bereitstellen:

```text
VRR State
Supported Range
Current Presentation Rate
VRR Transitions
Fallback Events
```

Damit können Display- und Performanceprobleme diagnostiziert werden.

## Normative Anforderungen

1. NovaOS SOLL VRR unterstützen, wenn Display und Treiber dies bereitstellen.
2. VRR-Unterstützung MUSS pro Display erkannt werden.
3. Minimum und Maximum Refresh Rate MÜSSEN bekannt sein.
4. Der Frame Scheduler MUSS VRR bei der Presentation-Planung berücksichtigen können.
5. VRR MUSS pro Display unabhängig steuerbar sein.
6. Statische Inhalte SOLLEN reduzierte Refresh-Aktivität ermöglichen.
7. Interaktive Inhalte SOLLEN mit möglichst geringer Presentation-Latenz dargestellt werden.
8. Verhalten unterhalb der minimalen VRR-Frequenz MUSS definiert sein.
9. VRR SOLL mit Direct Scanout kombinierbar sein.
10. VRR SOLL auch bei Windowed Composition unterstützt werden können.
11. Energieoptimierung DARF harte Timing-Anforderungen nicht verletzen.
12. Bei fehlender oder fehlerhafter VRR-Unterstützung MUSS Fixed Refresh als Fallback verfügbar sein.
13. VRR-Zustand und unterstützter Frequenzbereich MÜSSEN beobachtbar sein.

## Abhängigkeiten

- `NPSPEC-UI-DISPLAYSERVER-0001`
- `NPSPEC-UI-COMPOSITOR-0001`
- `NPSPEC-UI-FRAMESCHEDULING-0001`
- `NPSPEC-UI-GPUCOMPOSITION-0001`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0009_Realtime_Constraints_vor_Energieoptimierung`

## Ergebnis

NovaOS behandelt die Display Refresh Rate als dynamische Eigenschaft des Presentation-Pfades:

```text
Actual UI Activity
       ↓
Frame Scheduler
       ↓
Presentation Timing
       ↓
Variable Refresh Rate
```

Damit kann sich die Bildwiederholrate an den tatsächlichen UI-Inhalt anpassen, anstatt die gesamte Oberfläche permanent an ein starres Refresh-Raster zu binden.