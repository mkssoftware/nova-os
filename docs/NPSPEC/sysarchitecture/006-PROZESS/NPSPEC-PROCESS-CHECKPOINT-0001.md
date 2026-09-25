# NPSPEC-PROCESS-CHECKPOINT-0001 – Nova Process Checkpoint

## Status

Angenommen

## Kategorie

Process / Checkpoint / Recovery / State

## Zweck

NovaOS definiert einen kontrollierten Mechanismus zum Sichern des relevanten Zustands eines Prozesses, damit dieser später wiederhergestellt, migriert oder für Recovery-Zwecke verwendet werden kann.

```text
Running Process
      ↓
Checkpoint
      ↓
Saved Process State
      ↓
Restore
      ↓
Process Execution
```

## Grundprinzip

Ein Checkpoint ist ein konsistenter Zustandspunkt eines Prozesses.

```text
Checkpoint ≠ vollständiger Systemsnapshot
```

Gesichert werden nur die für eine spätere Wiederherstellung notwendigen Prozesszustände und explizit einbezogenen Ressourcen.

## Checkpoint-Inhalt

Ein Checkpoint kann mindestens enthalten:

```text
Process Identity
Process State
Thread State
CPU Context
Virtual Memory State
Resource References
Capability References
IPC State
Execution Metadata
Checkpoint Version
```

Nicht serialisierbare oder externe Ressourcen müssen explizit behandelt werden.

## Erstellung

Die grundlegende Erstellung erfolgt:

```text
Checkpoint Request
       ↓
Validate
       ↓
Quiesce Process
       ↓
Capture State
       ↓
Validate Snapshot
       ↓
Persist
       ↓
Resume
```

Der Prozess soll nur so lange wie notwendig angehalten werden.

## Konsistenz

Der gespeicherte Zustand muss intern konsistent sein.

Zusammengehörige Zustände dürfen nicht aus unterschiedlichen logischen Zeitpunkten stammen, wenn dadurch ein ungültiger Prozesszustand entstehen würde.

```text
Memory
+
Threads
+
Resources
=
Consistent Checkpoint
```

## Ressourcen

Externe Ressourcen benötigen definierte Checkpoint-Semantik.

Beispiele:

```text
Files
IPC Connections
Shared Memory
Devices
Network Connections
GPU Resources
```

Eine Ressource kann:

```text
Preserved
Reconnected
Recreated
Transferred
Excluded
Unsupported
```

sein.

## Capabilities

Capabilities dürfen nicht allein durch ihre Speicherung dauerhaft gültig bleiben.

Beim Restore müssen sicherheitsrelevante Rechte erneut gegen den aktuellen Systemzustand geprüft werden.

```text
Stored Capability
       ↓
Revalidate
       ↓
Restored Authority
```

Ein Checkpoint darf keine widerrufenen Rechte wiederherstellen.

## Restore

Die Wiederherstellung erfolgt kontrolliert:

```text
Load Checkpoint
      ↓
Validate Version
      ↓
Validate Security
      ↓
Restore Memory
      ↓
Restore Resources
      ↓
Restore Threads
      ↓
Verify
      ↓
Resume
```

Der Prozess darf erst weiterlaufen, wenn die erforderlichen Zustände erfolgreich wiederhergestellt wurden.

## Versionierung

Checkpoint-Formate müssen versioniert sein.

```text
CheckpointVersion
ProcessVersion
RuntimeVersion
```

Inkompatible Zustände dürfen nicht stillschweigend geladen werden.

Migration zwischen kompatiblen Versionen darf unterstützt werden.

## Migration

Checkpoints können als Grundlage für Prozessmigration verwendet werden.

```text
Process
   ↓
Checkpoint
   ↓
Transfer
   ↓
Restore
   ↓
New Execution Location
```

Dabei müssen Location-, Trust-, Security- und Data-Sovereignty-Anforderungen eingehalten werden.

## Fehlerbehandlung

Schlägt ein Checkpoint fehl, darf der laufende Prozess nicht in einem teilweise eingefrorenen Zustand verbleiben.

Schlägt ein Restore fehl:

```text
Restore Failure
      ↓
Abort Restore
      ↓
Cleanup
      ↓
Report Failure
```

Ein teilweise wiederhergestellter Prozess darf nicht als erfolgreich laufend gelten.

## Normative Anforderungen

1. NovaOS SOLL Prozess-Checkpoints unterstützen.
2. Ein Checkpoint MUSS einen konsistenten Prozesszustand repräsentieren.
3. Checkpoint-Formate MÜSSEN versioniert sein.
4. Nicht checkpointfähige Ressourcen MÜSSEN explizit erkannt werden.
5. Capabilities MÜSSEN beim Restore erneut validiert werden.
6. Widerrufene Berechtigungen DÜRFEN durch Restore NICHT wiederhergestellt werden.
7. Ein Restore MUSS vor Wiederaufnahme der Ausführung verifiziert werden.
8. Fehlgeschlagene Checkpoints DÜRFEN den ursprünglichen Prozess nicht inkonsistent zurücklassen.
9. Fehlgeschlagene Restores MÜSSEN teilweise erzeugte Ressourcen bereinigen.
10. Prozessmigration MUSS Security-, Trust- und Sovereignty-Constraints einhalten.
11. Inkompatible Checkpoints DÜRFEN NICHT stillschweigend geladen werden.
12. Checkpoint- und Restore-Vorgänge SOLLEN diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-PROCESS-MODEL-0001`
- `NPSPEC-PROCESS-LIFECYCLE-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-PROCESS-SUPERVISION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-PROCESS-0007`
- `ADR-STATE-0005_State_Snapshots`
- `ADR-STATE-0006_State_Versioning_und_History`

## Ergebnis

```text
Running Process
      ↓
Consistent Checkpoint
      ↓
Persist / Transfer
      ↓
Validate
      ↓
Restore
      ↓
Resume
```

NovaOS erhält damit einen kontrollierten Mechanismus für Prozesszustandssicherung, Recovery und Migration, ohne Sicherheitsrechte oder Zustandskonsistenz beim Restore zu umgehen.