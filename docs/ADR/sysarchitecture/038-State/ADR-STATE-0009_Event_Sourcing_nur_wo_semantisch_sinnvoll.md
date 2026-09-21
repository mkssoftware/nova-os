# ADR-STATE-0009 – Event Sourcing nur wo semantisch sinnvoll

## Status

Angenommen

## Kategorie

State Management / Event Sourcing / History

## Kontext

NovaOS benötigt für bestimmte Bereiche eine nachvollziehbare Historie von Zustandsänderungen.

Event Sourcing könnte grundsätzlich jede Änderung als Event speichern und den aktuellen Zustand daraus rekonstruieren.

Ein systemweites Event-Sourcing-Modell würde jedoch unnötige Komplexität, Speicherbedarf und Replay-Kosten erzeugen.

## Entscheidung

NovaOS verwendet **Event Sourcing selektiv und nur dort, wo die Ereignishistorie selbst semantische Bedeutung besitzt**.

```text
State
├── normal versioned state
└── event-sourced state
```

Event Sourcing ist kein globales Pflichtmodell für NovaOS.

## Einsatzkriterien

Event Sourcing ist sinnvoll, wenn mindestens einer der folgenden Punkte relevant ist:

```text
History is part of the semantics
Auditability is required
State reconstruction is useful
Temporal analysis is required
Events trigger dependent behavior
```

Typische Kandidaten sind:

```text
Transactions
Security Events
Configuration Changes
Recovery Actions
Agent Actions
Critical State Transitions
```

## Normales State Management

Für gewöhnlichen Systemzustand bleibt das bestehende Modell bevorzugt:

```text
Current State
+
Version
+
Optional History
```

Ein einfacher Zustandswechsel benötigt keinen Event Store, wenn die vollständige Ereignisfolge keinen zusätzlichen semantischen Wert besitzt.

## Event-Sourced State

Bei Event Sourcing entsteht Zustand aus einer geordneten Ereignisfolge.

```text
Event 1
  ↓
Event 2
  ↓
Event 3
  ↓
Current State
```

Events sind nach ihrer verbindlichen Speicherung unveränderlich.

Korrekturen erfolgen durch neue Events.

## Snapshots

Lange Event-Historien dürfen durch Snapshots ergänzt werden.

```text
Events 1–100
     ↓
Snapshot
     ↓
Events 101–120
     ↓
Current State
```

Damit muss der Zustand nicht bei jeder Rekonstruktion vollständig vom ersten Event an aufgebaut werden.

## Events und Observability

State Events sind nicht automatisch identisch mit:

```text
Logs
Metrics
Traces
Audit Events
```

Ein Event-Sourcing-Event ist Bestandteil des fachlichen Zustandsmodells.

Observability beschreibt dagegen die Ausführung und Beobachtung des Systems.

## Normative Anforderungen

1. NovaOS DARF Event Sourcing nicht als universelles State-Modell voraussetzen.
2. Event Sourcing SOLL nur verwendet werden, wenn die Ereignishistorie semantischen Nutzen besitzt.
3. Persistierte Events MÜSSEN unveränderlich sein.
4. Korrekturen MÜSSEN durch neue Events erfolgen.
5. Event-Sourced State MUSS deterministisch aus seiner definierten Eventfolge rekonstruierbar sein.
6. Lange Event-Historien MÜSSEN durch Snapshots optimierbar sein.
7. Event-Sourcing-Events MÜSSEN von Logs, Traces und normalen Observability Events unterscheidbar sein.
8. Bereiche ohne semantischen Nutzen aus Event Sourcing SOLLEN normales versioniertes State Management verwenden.

## Konsequenzen

### Positiv

- Event Sourcing wird dort genutzt, wo seine Historie echten Mehrwert bietet,
- vollständige Änderungsverläufe können gezielt rekonstruiert werden,
- unnötige globale Event-Store-Komplexität wird vermieden,
- State Versioning und Event Sourcing können nebeneinander existieren.

### Negativ

- unterschiedliche State-Modelle müssen unterstützt werden,
- für jeden Anwendungsbereich muss entschieden werden, ob Event Sourcing sinnvoll ist.

## Abhängigkeiten

- `ADR-STATE-0001_Globaler_System_State_Graph`
- `ADR-STATE-0005_State_Snapshots`
- `ADR-STATE-0006_State_Versioning_und_History`
- `ADR-STATE-0007_Transactional_State`
- `ADR-REALTIME-0007_Record_Replay`
- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_und_Tracing`

## Ergebnis

NovaOS folgt dem Grundsatz:

```text
Event Sourcing
ist ein Werkzeug,
kein universelles State-Modell.
```

Es wird nur dort eingesetzt, wo die Ereignisfolge selbst ein relevanter Bestandteil der Systemsemantik ist.