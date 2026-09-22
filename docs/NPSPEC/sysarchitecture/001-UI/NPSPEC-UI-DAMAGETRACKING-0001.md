# NPSPEC-UI-DAMAGETRACKING-0001 – Damage Tracking

## Status

Angenommen

## Kategorie

UI / Rendering / Compositor / Performance

## Zweck

NovaOS verwendet Damage Tracking, um nur die Bereiche einer Oberfläche neu zu verarbeiten, deren sichtbarer Inhalt sich tatsächlich geändert hat.

```text
UI Change
   ↓
Damage Region
   ↓
Scene Graph
   ↓
Compositor
   ↓
Partial Update
```

Dadurch werden unnötiges Rendering, Composition, Speicherbandbreite und Energieverbrauch reduziert.

## Damage Region

Eine Damage Region beschreibt einen oder mehrere Bereiche einer Surface oder Scene, deren vorheriger Bildinhalt nicht mehr gültig ist.

```text
Surface
┌─────────────────────────┐
│                         │
│      ┌──────────┐       │
│      │ Damage   │       │
│      └──────────┘       │
│                         │
└─────────────────────────┘
```

Damage wird unabhängig vom eigentlichen Pixelinhalt beschrieben.

## Damage Sources

Damage kann unter anderem entstehen durch:

```text
Content Change
Property Change
Window Movement
Resize
Animation
Visibility Change
Z-Order Change
Transform Change
Effect Change
Surface Replacement
```

Nicht jede Zustandsänderung muss automatisch den gesamten Frame invalidieren.

## Surface Damage

Anwendungen oder UI-Komponenten melden Änderungen ihrer eigenen Surface.

```text
Application Render
       ↓
Surface Damage
       ↓
Window Server
```

Eine Anwendung darf nur Damage für Ressourcen melden, die zu ihrem eigenen Kontext gehören.

## Scene Damage

Der Scene Graph überführt lokale Änderungen in sichtbare Scene Damage Regions.

```text
Surface Damage
      +
Transform
      +
Clipping
      +
Visibility
      ↓
Scene Damage
```

Dabei müssen Position und Transformation des betroffenen Nodes berücksichtigt werden.

## Damage Propagation

Änderungen an übergeordneten Nodes können Damage auf untergeordnete Bereiche übertragen.

```text
Parent Transform Change
          ↓
Affected Subtree
          ↓
Scene Damage
```

Die Propagation soll auf tatsächlich betroffene Bereiche begrenzt bleiben.

## Occlusion

Vollständig verdeckte Damage Regions müssen nicht unmittelbar komponiert werden.

```text
Damaged Surface
       ↓
Occlusion Test
       ↓
Not Visible
       ↓
Skip Composition
```

Wird der Bereich später sichtbar, muss sein aktueller Inhalt korrekt verfügbar sein.

## Damage Aggregation

Mehrere Damage Regions dürfen für einen Frame zusammengeführt werden.

```text
Damage A
Damage B
Damage C
   ↓
Aggregate
   ↓
Frame Damage
```

Das System darf nahe oder überlappende Bereiche zusammenfassen, wenn dies effizienter ist.

## Full Damage

Ist partielle Aktualisierung nicht sinnvoll oder nicht zuverlässig möglich, darf der gesamte relevante Bereich als beschädigt markiert werden.

```text
Partial Damage
      ↓
Too Complex / Unknown
      ↓
Full Damage
```

Korrekte Darstellung hat Vorrang vor der Optimierung.

## Frame Scheduling

Damage dient als Eingang für das Frame Scheduling.

```text
No Damage
   ↓
No New Frame Required

Damage
   ↓
Schedule Frame
```

Andere Gründe wie Display Refresh, Video, Animation oder externe Presentation-Anforderungen können trotzdem einen Frame auslösen.

## GPU Composition

Damage Regions werden an das GPU- oder Software-Composition-Backend weitergegeben.

```text
Frame Damage
     ↓
Composition Plan
     ↓
GPU / Software Backend
```

Das Backend darf entscheiden, ob partielle oder vollständige Composition effizienter ist.

## Buffer Age

Wenn mehrere Buffer verwendet werden, muss berücksichtigt werden, welchen vorherigen Inhalt ein Buffer enthält.

```text
Buffer Age
    +
Damage History
    ↓
Required Redraw Region
```

NovaOS darf frühere Damage-Informationen verwenden, um den minimal erforderlichen Aktualisierungsbereich zu bestimmen.

## Sicherheit

Damage-Metadaten dürfen keine Möglichkeit bieten, Inhalte fremder Surfaces auszulesen.

```text
Damage Information
       ≠
Surface Content Access
```

Security- und Isolation-Grenzen bleiben unabhängig vom Damage Tracking bestehen.

## Fehlerbehandlung

Fehlende, ungültige oder widersprüchliche Damage-Information darf keine fehlerhafte Darstellung verursachen.

Im Zweifel gilt:

```text
Unknown Damage
     ↓
Expand Damage
     ↓
Full Redraw if necessary
```

Damage Tracking ist eine Optimierung und keine Voraussetzung für korrekte Darstellung.

## Normative Anforderungen

1. NovaOS MUSS Damage Regions für Surfaces und Scenes darstellen können.
2. Anwendungen SOLLEN Änderungen ihrer Surfaces explizit melden.
3. Scene-Transformationen MÜSSEN bei der Damage-Berechnung berücksichtigt werden.
4. Damage SOLL auf tatsächlich betroffene Bereiche begrenzt werden.
5. Überlappende Damage Regions SOLLEN zusammengeführt werden können.
6. Verdeckte Bereiche SOLLEN unnötige Composition vermeiden.
7. Damage MUSS in das Frame Scheduling einfließen können.
8. GPU- und Software-Backends MÜSSEN Damage-Informationen verwenden können.
9. Buffer Age und Damage History SOLLEN unterstützt werden.
10. Damage Tracking DARF keine Surface-Isolation umgehen.
11. Unbekannter Damage-Zustand MUSS konservativ behandelt werden.
12. Vollständiges Redraw MUSS jederzeit als korrekter Fallback möglich sein.

## Abhängigkeiten

- `NPSPEC-UI-WINDOWSERVER-0001`
- `NPSPEC-UI-COMPOSITOR-0001`
- `NPSPEC-UI-SCENEGRAPH-0001`
- `NPSPEC-UI-RETAINEDMODE-0001`
- `NPSPEC-UI-GPUCOMPOSITION-0001`
- `ADR-ARCH-0004_Systemweite_Ressourcenökonomie`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

## Ergebnis

NovaOS aktualisiert die Oberfläche nach dem Prinzip:

```text
Change
   ↓
Damage
   ↓
Visible Damage
   ↓
Minimal Composition
   ↓
Presentation
```

Damage Tracking reduziert Rendering- und Composition-Arbeit auf die tatsächlich veränderten Bereiche, während bei Unsicherheit jederzeit auf eine vollständige Aktualisierung zurückgefallen werden kann.