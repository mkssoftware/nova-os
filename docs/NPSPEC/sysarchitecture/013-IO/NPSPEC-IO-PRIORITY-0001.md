# NPSPEC-IO-PRIORITY-0001 – Nova I/O Priority

## Status

Angenommen

## Kategorie

I/O / Priority / Scheduling

## Zweck

NovaOS definiert ein einheitliches Prioritätsmodell für I/O-Requests.

```text
IORequest
   ↓
I/O Priority
   ↓
I/O Scheduler
   ↓
Provider
```

Prioritäten beschreiben die relative Dringlichkeit einer I/O-Operation, ohne Sicherheits-, Ressourcen- oder Realtime-Regeln zu umgehen.

## Grundprinzip

```text
Priority ≠ Permission
Priority ≠ Deadline
Priority ≠ Guaranteed Execution
```

Eine hohe Priorität beeinflusst Scheduling-Entscheidungen, garantiert jedoch weder sofortige Ausführung noch unbegrenzte Ressourcennutzung.

## Prioritätsklassen

NovaOS soll mindestens folgende logische Klassen unterstützen:

```text
Realtime
Interactive
Normal
Background
Maintenance
```

Die Klassen bleiben unabhängig von gerätespezifischen Hardware-Prioritäten.

## Bedeutung

### Realtime

Für zeitkritische I/O mit expliziten zeitlichen Anforderungen.

### Interactive

Für I/O, deren Verzögerung unmittelbar durch den Benutzer wahrnehmbar ist.

### Normal

Standardklasse für gewöhnliche I/O-Operationen.

### Background

Für nicht zeitkritische Hintergrundarbeit.

### Maintenance

Für Wartungsoperationen wie Scrubbing, Indexierung oder Optimierung.

## Request Priority

Ein `IORequest` darf seine Priorität explizit angeben.

```text
IORequest
├── Operation
├── Target
├── Priority
└── Deadline
```

Fehlt eine explizite Priorität, muss eine definierte Standardklasse verwendet werden.

## Vererbung

Prioritäten dürfen aus dem ausführenden Kontext abgeleitet werden.

```text
Task
  ↓
Execution Context
  ↓
IORequest
```

Explizite Policies oder Execution Contracts dürfen die abgeleitete Priorität begrenzen oder überschreiben.

## Priority und Deadline

Priorität und Deadline bleiben getrennte Eigenschaften.

```text
Priority → relative Dringlichkeit
Deadline → zeitliche Grenze
```

Ein Request mit niedrigerer Priorität kann aufgrund einer nahen Deadline vor einem höher priorisierten Request ausgeführt werden.

## Fairness

Hohe Priorität darf niedrigere Klassen nicht dauerhaft verdrängen.

```text
High Priority
      +
Fairness Policy
      ↓
I/O Scheduler
```

Der Scheduler muss Starvation verhindern können, sofern keine explizite Realtime-Policy etwas anderes verlangt.

## Priority Inversion

NovaOS muss Situationen berücksichtigen, in denen hoch priorisierte I/O indirekt von niedriger priorisierter Arbeit abhängt.

```text
High Priority Request
        ↓ waits for
Low Priority Dependency
```

Der Scheduler oder zuständige Provider darf temporäre Prioritätsanpassungen verwenden, um solche Blockierungen zu begrenzen.

## Provider Mapping

Provider dürfen logische NovaOS-Prioritäten auf eigene Mechanismen abbilden.

```text
Nova Priority
     ↓
Provider Mapping
     ↓
Hardware Queue / Protocol Priority
```

Eine fehlende Hardwareunterstützung darf die logische Prioritätssemantik nicht ungültig machen.

## Ressourcenökonomie

Prioritäten dürfen Ressourcenentscheidungen beeinflussen.

Beispiele:

```text
Queue Position
Bandwidth
I/O Parallelism
Wakeup Behavior
Power State
```

Harte Ressourcenlimits dürfen dadurch nicht überschritten werden.

## Sicherheit

Ein Consumer darf seine effektive Priorität nicht beliebig erhöhen.

Prioritätserhöhungen müssen durch Policy, Capability oder Execution Contract begrenzbar sein.

```text
Requested Priority
       ↓
Policy Check
       ↓
Effective Priority
```

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
RequestID
Requested Priority
Effective Priority
Priority Class
Owner
Wait Time
Deadline
Provider Mapping
```

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Prioritätsmodell für I/O-Requests bereitstellen.
2. Priorität, Deadline und Berechtigung MÜSSEN getrennte Konzepte bleiben.
3. NovaOS SOLL die Klassen `Realtime`, `Interactive`, `Normal`, `Background` und `Maintenance` unterstützen.
4. Jeder Request MUSS eine eindeutig bestimmbare effektive Priorität besitzen.
5. Fehlende Prioritätsangaben MÜSSEN auf eine definierte Standardklasse abgebildet werden.
6. Prioritäten SOLLEN aus dem Execution Context ableitbar sein.
7. Consumer DÜRFEN ihre effektive Priorität NICHT unkontrolliert erhöhen.
8. Hohe Prioritäten DÜRFEN Security- oder Ressourcenlimits NICHT umgehen.
9. Der I/O-Scheduler SOLL Starvation niedriger Prioritäten verhindern.
10. Priority Inversion MUSS erkennbar und begrenzbar sein.
11. Provider DÜRFEN logische Prioritäten auf Hardware- oder Protokollmechanismen abbilden.
12. Angeforderte und effektive Priorität SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-SCHEDULER-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SYNC-PRIORITYINHERITANCE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-IO-0005`

## Ergebnis

```text
Requested Priority
        ↓
Policy / Execution Contract
        ↓
Effective Priority
        ↓
I/O Scheduler
        ↓
Provider
```

NovaOS erhält damit ein einheitliches I/O-Prioritätsmodell, das Dringlichkeit ausdrückt, ohne Fairness, Sicherheit, Ressourcenlimits oder explizite Deadline-Anforderungen zu verletzen.