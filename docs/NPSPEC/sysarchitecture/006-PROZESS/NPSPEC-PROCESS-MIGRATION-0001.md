# NPSPEC-PROCESS-MIGRATION-0001 – Nova Process Migration

## Status

Entwurf

## Kategorie

Process / Migration / Distributed Execution / Location Transparency

## Zweck

NovaOS definiert die kontrollierte Migration eines laufenden oder pausierten Prozesses zwischen Ausführungsorten.

```text
Process
   ↓
Migration
   ↓
New Execution Location
```

Dabei sollen Prozesszustand, Sicherheit und semantische Identität erhalten bleiben.

## Grundprinzip

Die logische Identität eines Prozesses ist von seinem physischen Ausführungsort getrennt.

```text
Process Identity ≠ Execution Location
```

Eine Migration darf daher den Ausführungsort ändern, ohne den Prozess für seine Umgebung unnötig als neues logisches Objekt erscheinen zu lassen.

## Migrationsziele

Migration kann erfolgen zwischen:

```text
CPU Cores
NUMA Nodes
Local Compute Providers
Virtual Machines
Physical Systems
Cluster Nodes
```

Nicht jede Prozessklasse muss jede Form der Migration unterstützen.

## Migrationsablauf

```text
Migration Request
       ↓
Validate Target
       ↓
Validate Constraints
       ↓
Prepare Target
       ↓
Quiesce Process
       ↓
Checkpoint
       ↓
Transfer State
       ↓
Restore
       ↓
Verify
       ↓
Switch Execution
       ↓
Release Source
```

Die Quelle darf erst endgültig aufgegeben werden, wenn das Ziel erfolgreich verifiziert wurde.

## Prozesszustand

Für die Migration müssen relevante Zustände übertragen oder rekonstruiert werden.

Dazu können gehören:

```text
Memory
Threads
CPU Context
Capabilities
IPC State
Resource References
Execution Metadata
```

Der Checkpoint-Mechanismus bildet die Grundlage für die Zustandssicherung.

## Ressourcen

Ressourcen werden abhängig von ihrer Semantik behandelt.

```text
Transfer
Reconnect
Recreate
Remap
Keep Remote
Reject Migration
```

Ein lokal gebundenes Gerät kann beispielsweise eine Migration verhindern oder einen alternativen Capability Provider erfordern.

## Capabilities und Sicherheit

Capabilities müssen am Ziel erneut validiert werden.

```text
Source Authority
      ↓
Migration
      ↓
Target Validation
      ↓
Target Authority
```

Migration darf keine Berechtigungen erweitern.

Widerrufene oder am Ziel nicht zulässige Capabilities dürfen nicht wiederhergestellt werden.

## Execution Contract

Vor der Migration müssen Hard Requirements des Prozesses am Ziel erfüllt sein.

Beispiele:

```text
Architecture
Memory
Latency
Determinism
Trust
Data Sovereignty
Required Devices
Required Capabilities
```

Ein Ziel, das Hard Requirements nicht erfüllt, darf nicht verwendet werden.

## Location Transparency

Andere Komponenten sollen möglichst weiterhin über stabile Identitäten mit dem Prozess interagieren.

```text
ProcessID
   ↓
Location Resolution
   ↓
Current Execution Location
```

Standortabhängige Referenzen müssen nach der Migration aktualisiert oder weitergeleitet werden.

## Live Migration

NovaOS darf Live Migration unterstützen.

Dabei soll die Unterbrechungszeit durch Verfahren wie schrittweise Speicherübertragung reduziert werden.

```text
Pre-Copy State
     ↓
Short Quiesce
     ↓
Final State Transfer
     ↓
Resume Target
```

Live Migration ist eine Optimierung und keine Voraussetzung für jede Migration.

## Fehlerbehandlung

Schlägt die Migration vor dem endgültigen Umschalten fehl:

```text
Migration Failure
       ↓
Abort
       ↓
Resume Source
```

Nach einem unklaren Kommunikationsabbruch muss verhindert werden, dass Quelle und Ziel gleichzeitig unabhängig weiterlaufen.

```text
Unknown ≠ Migration Successful
```

## Normative Anforderungen

1. NovaOS SOLL Prozessmigration unterstützen.
2. Prozessidentität MUSS vom Ausführungsort getrennt bleiben.
3. Migration MUSS über einen konsistenten Prozesszustand erfolgen.
4. Das Ziel MUSS vor der Migration validiert werden.
5. Hard Requirements des Execution Contracts MÜSSEN am Ziel erfüllt sein.
6. Capabilities MÜSSEN am Ziel erneut validiert werden.
7. Migration DARF keine Rechteerweiterung verursachen.
8. Ressourcen MÜSSEN explizite Migrationssemantik besitzen.
9. Nicht migrierbare Ressourcen MÜSSEN erkannt werden.
10. Die Quelle DARF erst nach erfolgreicher Zielverifikation endgültig freigegeben werden.
11. Quelle und Ziel DÜRFEN nach einer Migration nicht unkontrolliert gleichzeitig dieselbe Prozessinstanz ausführen.
12. Fehlgeschlagene oder unklare Migrationen MÜSSEN diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-PROCESS-MODEL-0001`
- `NPSPEC-PROCESS-CHECKPOINT-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-DISTRIBUTED-0001`
- `ADR-DISTRIBUTED-0009`

## Ergebnis

```text
Process @ Source
       ↓
Checkpoint + Validate
       ↓
Transfer
       ↓
Restore + Verify
       ↓
Process @ Target
```

NovaOS kann Prozesse damit kontrolliert zwischen geeigneten Ausführungsorten verschieben, während Identität, Sicherheitsgrenzen, Ressourcenanforderungen und Prozesszustand erhalten bleiben.