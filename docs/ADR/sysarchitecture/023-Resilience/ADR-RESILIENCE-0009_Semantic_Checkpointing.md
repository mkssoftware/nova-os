# ADR-RESILIENCE-0009 – Semantic Checkpointing

## Status

Angenommen

## Kategorie

Resilience Architecture / State Recovery / Checkpointing

## Kontext

Klassisches Checkpointing speichert häufig einen technischen Snapshot von Speicher, Prozesszustand oder Dateisystem.

Für NovaOS reicht dies nicht immer aus. Ein technisch vollständiger Snapshot kann semantisch inkonsistent sein, beispielsweise wenn:

- eine Transaktion noch nicht abgeschlossen ist,
- externe Side Effects bereits ausgeführt wurden,
- abhängige Ressourcen einen anderen Zustand besitzen,
- Capability Handles nicht mehr gültig sind,
- verteilte Operationen nur teilweise abgeschlossen wurden.

Nova.Resilience benötigt deshalb Checkpoints, die nicht nur technischen Zustand, sondern einen **semantisch gültigen Wiederherstellungspunkt** repräsentieren.

## Entscheidung

NovaOS führt **Semantic Checkpointing** als systemweiten Recovery-Mechanismus ein.

```text
Runtime State
      ↓
Semantic Boundary
      ↓
Consistency Validation
      ↓
Checkpoint
      ↓
Recovery
```

Ein Semantic Checkpoint beschreibt einen Zustand, von dem eine Komponente unter definierten Bedingungen korrekt weiterarbeiten kann.

## Grundprinzip

```text
Restorable bytes
≠
Restorable system state
```

Ein Checkpoint gilt nur dann als Recovery-Punkt, wenn seine relevanten semantischen Bedingungen bekannt und erfüllbar sind.

## Semantic Boundary

Komponenten können sichere Checkpoint-Grenzen definieren.

Beispiele:

```text
Transaction committed
Document state consistent
Request completed
Pipeline stage completed
External effect reconciled
Application state stable
```

Nova.Resilience soll Checkpoints bevorzugt an solchen Grenzen erzeugen.

## Checkpoint-Inhalt

Ein Semantic Checkpoint kann neben eigentlichen Zustandsdaten zusätzliche Informationen enthalten:

```text
State
Semantic Version
Invariant Set
Dependency References
Transaction State
External Effect State
Capability Requirements
Recovery Metadata
```

Nicht jede Komponente benötigt alle Informationen.

Die konkrete Darstellung wird in NPSPECs definiert.

## Invariants

Ein Checkpoint kann Bedingungen definieren, die für seine Wiederherstellung erfüllt sein müssen.

```text
Checkpoint
    +
Required Invariants
    ↓
Restore Validation
```

Kann eine notwendige Invariante nicht hergestellt oder bestätigt werden, darf der Checkpoint nicht als vollständig gültiger Recovery-Zustand behandelt werden.

## Abhängigkeiten

Checkpoints können von externen oder gemeinsam genutzten Zuständen abhängig sein.

```text
Application Checkpoint
        │
        ├── Storage State
        ├── Service State
        └── Capability Dependencies
```

Nova.Resilience muss solche Abhängigkeiten bei der Wiederherstellung berücksichtigen können.

Ein lokaler Checkpoint darf nicht automatisch als global konsistent gelten.

## Transaktionen

Semantic Checkpointing integriert sich mit transaktionalen Systemoperationen.

Bevorzugte Checkpoints liegen an eindeutig definierten Transaktionsgrenzen:

```text
Transaction
    ↓
Commit
    ↓
Semantic Checkpoint
```

Ein Checkpoint innerhalb einer laufenden Transaktion muss deren Zustand explizit berücksichtigen oder die Transaktion bei Recovery sicher zurücksetzen können.

## Externe Side Effects

Nicht alle Zustände können durch einen lokalen Snapshot zurückgesetzt werden.

Beispiele:

```text
Network Message
Remote Mutation
Device Command
Payment
Physical Actuation
```

Semantic Checkpoints müssen deshalb bekannte externe Side Effects berücksichtigen können.

```text
Local State
    +
External Effect State
    ↓
Recovery Semantics
```

Eine Recovery darf externe Effekte nicht unbeabsichtigt erneut ausführen.

## Capability Handles

Kurzlebige Capability Handles oder Leases dürfen nicht blind aus einem Checkpoint wiederhergestellt werden.

```text
Checkpoint
    ↓
Restore
    ↓
Capability Revalidation
```

NovaOS muss relevante Authority, Leases, Epochs und Revocation-Zustände erneut prüfen können.

Ein alter Checkpoint darf keine widerrufene Authority wiederherstellen.

## Checkpoint-Versionierung

Semantic Checkpoints müssen versionierbar sein.

Dies ermöglicht Recovery über kompatible Software- oder Komponentenupdates hinweg.

```text
Checkpoint Version
        ↓
Current Component Version
        ↓
Compatible?
   ├── Yes → Restore
   └── No  → Migration / Alternative Recovery
```

Hot Replacement und Live Evolution können dadurch mit State Recovery kombiniert werden.

## Checkpoint Scope

Checkpoints können unterschiedliche Scopes besitzen:

```text
Task
Workload
Service
Application
Subsystem
```

Der Checkpoint Scope soll mit den hierarchischen Failure Domains und dem kleinsten sicheren Recovery Scope abgestimmt werden.

## Verification

Nach Wiederherstellung eines Checkpoints muss der resultierende Zustand verifiziert werden.

```text
Checkpoint
    ↓
Restore
    ↓
Invariant Verification
    ↓
Health Verification
    ↓
Resume
```

Das erfolgreiche Laden eines Checkpoints allein gilt nicht als erfolgreiche Recovery.

## Datenschutz und Secrets

Checkpoints können sensible Informationen enthalten.

Sie unterliegen deshalb denselben Security-, Privacy-, Encryption- und Data-Sovereignty-Regeln wie der ursprüngliche Zustand.

Secrets sollen nach Möglichkeit nicht als exportierbares Rohmaterial im Checkpoint gespeichert werden.

Stattdessen sollen sichere Referenzen oder erneut validierbare Handles verwendet werden.

## Checkpoint-Auswahl

Existieren mehrere gültige Checkpoints, kann Nova.Resilience den geeignetsten auswählen.

Dabei können berücksichtigt werden:

```text
Age
Semantic Validity
Recovery Cost
State Loss
Compatibility
Dependency Availability
Trust State
```

Der neueste Checkpoint ist nicht zwingend der beste Recovery-Punkt.

## Normative Anforderungen

1. NovaOS MUSS Semantic Checkpointing als Recovery-Mechanismus unterstützen können.
2. Ein Semantic Checkpoint MUSS einen semantisch definierten Wiederherstellungspunkt repräsentieren.
3. Technische Snapshot-Fähigkeit allein DARF nicht als semantische Wiederherstellbarkeit gelten.
4. Komponenten MÜSSEN sichere Checkpoint-Grenzen definieren können.
5. Checkpoints MÜSSEN relevante Invariants und Recovery-Metadaten referenzieren können.
6. Abhängigkeiten und Shared State MÜSSEN bei der Wiederherstellung berücksichtigt werden können.
7. Transaktionszustände MÜSSEN bei Checkpoint und Recovery berücksichtigt werden.
8. Externe Side Effects DÜRFEN durch Recovery nicht unbeabsichtigt wiederholt werden.
9. Capability Handles, Leases und Authority MÜSSEN nach Recovery erneut validierbar sein.
10. Ein Checkpoint DARF widerrufene Authority nicht wiederherstellen.
11. Semantic Checkpoints MÜSSEN versionierbar sein.
12. Checkpoint-Kompatibilität MUSS vor einer Wiederherstellung prüfbar sein.
13. Checkpoint Scope MUSS mit Failure Domain und Recovery Scope verknüpfbar sein.
14. Eine Checkpoint-Recovery MUSS anschließend verifiziert werden.
15. Checkpoints MÜSSEN Security-, Privacy-, Trust- und Data-Sovereignty-Regeln einhalten.
16. Secrets SOLLEN nach Möglichkeit nicht als exportierbares Rohmaterial in Checkpoints gespeichert werden.
17. Nova.Resilience MUSS zwischen mehreren Checkpoints anhand ihrer Recovery-Eignung auswählen können.
18. Checkpoint-Erzeugung, Auswahl und Wiederherstellung MÜSSEN kontrolliert introspektierbar und auditierbar sein.

## Konsequenzen

### Positive Konsequenzen

- Recovery stellt semantisch gültigen statt nur technischen Zustand wieder her,
- Transaktionen und externe Side Effects können korrekt berücksichtigt werden,
- widerrufene Capabilities werden durch alte Checkpoints nicht reaktiviert,
- Checkpoints können mit Hot Replacement und Live Evolution kombiniert werden,
- Recovery kann gezielt auf kleinen Failure Domains erfolgen.

### Negative Konsequenzen

- Komponenten müssen ihre semantischen Recovery-Grenzen beschreiben,
- externe Side Effects erschweren konsistente Checkpoints,
- versionierte Zustände benötigen Kompatibilitäts- oder Migrationslogik.

## Verworfene Alternativen

### Reine Memory Snapshots

Verworfen, da ein technisch wiederherstellbarer Speicherzustand nicht automatisch semantisch konsistent ist.

### Vollständige System-Snapshots für jede Recovery

Verworfen, da dies den Recovery Scope unnötig vergrößert und lokale Fehler nicht effizient behandelt.

### Immer den neuesten Checkpoint verwenden

Verworfen, da ein älterer Checkpoint semantisch geeigneter oder mit dem aktuellen Systemzustand besser kompatibel sein kann.

### Capability Handles unverändert wiederherstellen

Verworfen, da Revocation, Leases und Authority seit der Checkpoint-Erstellung verändert worden sein können.

## Abhängigkeiten

- `ADR-RESILIENCE-0001_Nova_Resilience_als_systemweite_Self_Healing_Architektur`
- `ADR-RESILIENCE-0002_Detect_Classify_Contain_Preserve_Plan_Recover_Verify_Restore_Learn`
- `ADR-RESILIENCE-0003_Hierarchische_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-ARCH-0003_Transaktionale_Systemoperationen`
- `ADR-ARCH-0012_Hot_Replacement_Live_Evolution`
- `ADR-CAPABILITY-0009_Capability_Revocation`
- `ADR-SECURITY-0011_Capability_Leases_und_Epochs`

## Zugehörige NPSPECs

- `NPSPEC-RESILIENCE-SEMANTIC-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-INVARIANT-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-DEPENDENCY-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-VERSIONING-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-RESTORE-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-TEST-0001`

## Ergebnis

Nova.Resilience verwendet Checkpoints nicht lediglich als technische Snapshots:

```text
Consistent Runtime State
        ↓
Semantic Boundary
        ↓
Semantic Checkpoint
        ↓
Failure
        ↓
Compatibility + Authority Validation
        ↓
Restore
        ↓
Invariant Verification
        ↓
Resume
```

Damit wird ein Checkpoint in NovaOS zu einem definierten, überprüfbaren **semantischen Wiederherstellungspunkt**, der Zustand, Abhängigkeiten, Transaktionen und Authority korrekt berücksichtigt.