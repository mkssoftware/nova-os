# ADR-STATE-0006 – State Versioning und History

## Status

Angenommen

## Kategorie

State Management / Versioning / History

## Kontext

NovaOS-Zustände verändern sich kontinuierlich. Für Diagnose, Recovery, Reconciliation und Record/Replay muss nachvollziehbar sein, welcher Zustand zu welchem Zeitpunkt gültig war.

Ein einzelner aktueller State reicht dafür nicht aus.

## Entscheidung

NovaOS versieht relevante Systemzustände mit **monoton fortschreitenden Versionen bzw. Generationsnummern** und kann eine begrenzte State History führen.

```text
State v1
   ↓
State v2
   ↓
State v3
   ↓
State v4
```

Der aktuelle Zustand bleibt die aktive Sicht; ältere Versionen dienen der Historie und Analyse.

## State Version

Eine Zustandsänderung erzeugt eine neue Version.

```text
Node ID
State Version
State
Timestamp / Ordering
```

Die stabile Identität eines Nodes bleibt dabei erhalten.

```text
Node ID = konstant
State Version = verändert sich
```

## History

Für relevante Nodes können frühere Zustände gespeichert werden.

Beispiel:

```text
Service A

v41 → Stopped
v42 → Starting
v43 → Running
v44 → Failed
v45 → Recovering
v46 → Running
```

Nicht jede kurzfristige interne Änderung muss dauerhaft historisiert werden.

Retention richtet sich nach Bedeutung, Policy und verfügbarem Speicher.

## State Changes

History soll Zustandsänderungen nachvollziehbar machen.

Mindestens relevant sind:

```text
Previous Version
New Version
Transition
Result
```

Damit können Änderungen mit Logs, Traces, Executions und Recovery-Aktionen korreliert werden.

## Optimistic Concurrency

Versionen können zur Erkennung konkurrierender Änderungen verwendet werden.

```text
Read State v10
      ↓
Modify
      ↓
Commit if Current == v10
```

Ist inzwischen `v11` aktiv, muss die Operation neu bewertet oder abgelehnt werden.

Dadurch werden verlorene Updates vermieden.

## Snapshots

State Snapshots referenzieren definierte State-Versionen bzw. Generationen.

```text
State History
      ↓
Generation 105
      ↓
Snapshot
```

Snapshots und History ergänzen sich, sind aber nicht identisch.

## Recovery

Historische Zustände können Recovery-Entscheidungen unterstützen.

Ein früherer State darf jedoch nicht einfach zurückgeschrieben werden.

```text
Historical State
      ↓
Recovery Plan
      ↓
Valid State Transition
```

Recovery muss weiterhin State Machines, Authorization und Hard Constraints respektieren.

## Distributed State

Bei verteiltem State muss keine universelle globale Versionsnummer vorausgesetzt werden.

NovaOS kann je nach Subsystem verwenden:

```text
Local Generation
Object Version
Causal Ordering
Distributed Version Metadata
```

Die verwendete Ordnung muss für den jeweiligen Anwendungsfall eindeutig definiert sein.

## Normative Anforderungen

1. Relevante Systemzustände MÜSSEN versionierbar sein.
2. Zustandsänderungen MÜSSEN neue Versionen bzw. Generationen erzeugen können.
3. Stable Node Identity und State Version MÜSSEN getrennt behandelt werden.
4. NovaOS MUSS eine begrenzte State History unterstützen können.
5. History MUSS einer Retention Policy unterliegen.
6. State Versioning MUSS zur Erkennung konkurrierender Änderungen nutzbar sein.
7. Snapshots MÜSSEN definierte State-Versionen referenzieren können.
8. Historische Zustände DÜRFEN Hard Constraints und State Machines nicht umgehen.
9. Verteiltes State Versioning DARF keine universelle globale Versionsnummer voraussetzen.

## Konsequenzen

### Positiv

- Zustandsänderungen werden nachvollziehbar,
- konkurrierende Änderungen können erkannt werden,
- Diagnose und Self-Healing erhalten historischen Kontext,
- Snapshots und Record/Replay können genaue State-Versionen referenzieren.

### Negativ

- History benötigt zusätzlichen Speicher,
- Retention muss verwaltet werden,
- verteilte Versionsordnung ist komplexer als lokale Versionierung.

## Abhängigkeiten

- `ADR-STATE-0001_Globaler_System_State_Graph`
- `ADR-STATE-0002_Desired_und_Actual_State`
- `ADR-STATE-0003_State_Machines`
- `ADR-STATE-0004_State_Reconciliation`
- `ADR-STATE-0005_State_Snapshots`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-REALTIME-0007_Record_Replay`
- `ADR-OBSERVABILITY-0005_System_State_Graph`

## Ergebnis

NovaOS betrachtet Systemzustand nicht nur als aktuellen Wert, sondern als versionierbare Entwicklung:

```text
Identity
   +
Versioned State
   +
Bounded History
```

Dadurch kann NovaOS nachvollziehen, **was sich geändert hat, in welcher Reihenfolge es geschah und auf welchem Zustand eine Entscheidung basierte**.