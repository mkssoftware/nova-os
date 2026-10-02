# NPSPEC-UPDATE-LIVE-0001 – Nova Live Update

## Status

Angenommen

## Kategorie

Update / Live Evolution / Runtime Replacement

## Zweck

NovaOS definiert Live Updates für Komponenten, die während des laufenden Betriebs ersetzt werden können, ohne das gesamte System neu zu starten.

```text
Running Component v1
        ↓
Prepare v2
        ↓
Validate
        ↓
Transfer State
        ↓
Atomic Switch
        ↓
Component v2
        ↓
Verify
```

Live Updates ergänzen normale, A/B- und Immutable-Updates, ersetzen diese jedoch nicht.

## Grundprinzipien

```text
Live Update ≠ In-Place Modification
Loaded ≠ Active
Active ≠ Verified
Compatible ≠ Safe
State Transfer ≠ Memory Copy
Switch ≠ Update Complete
Live Update ≠ Zero Downtime Guarantee
Rollback ≠ Restore Revoked Authority
```

## Live Update Model

```text
LiveUpdate
├── UpdateID
├── ComponentID
├── OldVersion
├── NewVersion
├── OldInstance
├── NewInstance
├── State
└── SwitchPolicy
```

Optional:

```text
StateSchema
MigrationPlan
ExecutionContract
CapabilitySet
ResourceBudget
RollbackPlan
VerificationPlan
ProvenanceID
```

## Geeignete Komponenten

Live Updates können unter anderem verwendet werden für:

```text
Drivers
Services
System Modules
Capability Providers
Runtime Components
Libraries
User-Space Infrastructure
Selected Kernel Components
```

Nicht jede Komponente muss live aktualisierbar sein.

## Update-Ablauf

```text
Acquire Update
      ↓
Validate Package
      ↓
Load New Instance
      ↓
Validate Compatibility
      ↓
Prepare State Transfer
      ↓
Quiesce / Synchronize
      ↓
Transfer State
      ↓
Atomic Switch
      ↓
Verify New Instance
      ↓
Retire Old Instance
```

## Parallel Preparation

Die neue Instanz soll soweit möglich parallel zur bestehenden Instanz vorbereitet werden.

```text
Old Instance → serving requests

New Instance
    ↓
Load
Initialize
Validate
Prepare
```

Dadurch wird die Unterbrechungszeit auf den eigentlichen Umschaltpunkt reduziert.

## State Transfer

Persistenter oder laufzeitrelevanter Zustand muss explizit übertragen werden.

```text
State v1
   ↓
Migration
   ↓
State v2
```

State Transfer darf nicht als beliebiges Kopieren interner Speicherstrukturen verstanden werden.

Übertragbare Zustände müssen über definierte Schemas oder Contracts beschrieben werden.

## Quiescence

Vor dem Switch kann ein kontrollierter Quiescence Point erforderlich sein.

```text
Accept Requests
      ↓
Stop New Critical Work
      ↓
Drain / Finish Operations
      ↓
State Stable
      ↓
Switch
```

Die notwendige Quiescence muss möglichst klein gehalten werden.

## Structured Concurrency

Aktive Tasks der alten Instanz müssen kontrolliert behandelt werden.

Mögliche Strategien:

```text
Finish
Cancel
Transfer
Restart
Drain
```

Verwaiste Tasks dürfen nicht unkontrolliert weiterlaufen.

## Capability Transfer

Capabilities werden nicht automatisch auf die neue Instanz übertragen.

```text
Old Capabilities
      ↓
Revalidate
      ↓
Required Authority
      ↓
New Capability Set
```

```text
State Transfer ≠ Authority Transfer
```

Die neue Instanz erhält nur aktuell autorisierte Capabilities.

## Atomic Switch

Nach erfolgreicher Vorbereitung erfolgt ein definierter Umschaltpunkt.

```text
Reference → Old Instance

Atomic Switch

Reference → New Instance
```

Neue Requests müssen anschließend eindeutig zur neuen Instanz geleitet werden.

Bereits laufende Operationen dürfen entsprechend ihrer Update Policy kontrolliert beendet oder migriert werden.

## Handle Stability

Wo möglich sollen stabile logische IDs und Handles verhindern, dass Nutzer einer Capability den physischen Providerwechsel kennen müssen.

```text
Stable Capability
       ↓
Provider v1
       ↓
Live Switch
       ↓
Provider v2
```

```text
Identity ≠ Implementation Instance
```

## Verification

Nach dem Switch wird die neue Instanz geprüft.

```text
New Instance Active
       ↓
Health Check
       ↓
Contract Verification
       ↓
State Verification
       ↓
Operational Verification
```

Die alte Instanz darf erst entfernt werden, wenn die neue Instanz ausreichend bestätigt wurde.

## Rollback

Schlägt die Verifikation fehl:

```text
New Instance
     ↓
Failure
     ↓
Switch Back
     ↓
Old Instance
```

Rollback ist nur möglich, wenn State und externe Effekte dies weiterhin zulassen.

Andernfalls:

```text
Compensation
Recovery
Restart
A/B Fallback
```

## Kernel Live Update

Kernel-Komponenten dürfen nur live ersetzt werden, wenn ihre Update-Grenzen explizit definiert sind.

Besonders kritisch sind:

```text
Interrupt Paths
Scheduler
Memory Management
Synchronization
IPC
Security Enforcement
Capability Management
```

Für nicht sicher live ersetzbare Kerneländerungen muss NovaOS einen normalen Boot-, A/B- oder Immutable-Update-Pfad verwenden.

## Driver Live Update

Treiber können über isolierte Driver Domains aktualisiert werden.

```text
Driver v1
    ↓
Quiesce Device
    ↓
Preserve Device State
    ↓
Driver v2
    ↓
Restore / Reinitialize
    ↓
Verify Device
```

Ein fehlgeschlagener Driver Live Update darf nicht automatisch das gesamte System gefährden.

## Transaction Integration

Live Update wird als Update-Transaktion behandelt.

```text
Begin
 ↓
Prepare New Instance
 ↓
Validate
 ↓
Prepare State
 ↓
Switch
 ↓
Verify
 ↓
Commit / Rollback
```

## Resource Management

Während des Übergangs können alte und neue Instanz gleichzeitig Ressourcen benötigen.

NovaOS muss berücksichtigen:

```text
Memory
CPU
Handles
Buffers
Device Resources
Temporary State
```

Die Live-Aktualisierung darf harte Resource Guarantees anderer Workloads nicht verletzen.

## Realtime

Für Realtime-Komponenten muss der Switch innerhalb definierter zeitlicher Grenzen erfolgen.

```text
Live Update
+
Realtime Contract
↓
Bounded Transition
```

Kann die zeitliche Garantie nicht eingehalten werden, muss Live Update abgelehnt oder verschoben werden.

## Failure Handling

Fehler können auftreten während:

```text
Load
Initialization
State Migration
Quiescence
Switch
Verification
Old Instance Retirement
```

Jede Phase muss einen definierten Failure State besitzen.

`Unknown` darf nicht als erfolgreicher Live Update interpretiert werden.

## Provenance

NovaOS soll nachvollziehen können:

```text
UpdateID
ComponentID
OldVersion
NewVersion
StateMigration
CapabilityChanges
SwitchTime
VerificationResult
RollbackResult
FailureReason
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Live Update Capability
Current Version
Prepared Version
Update Phase
State Migration State
Quiescence State
Switch State
Verification State
Rollback Availability
Old Instance State
```

## Normative Anforderungen

1. NovaOS MUSS Live Updates für geeignete Komponenten unterstützen können.
2. Live Update DARF NICHT für jede Komponente vorausgesetzt werden.
3. Die neue Instanz MUSS vor dem Switch validiert werden.
4. Die aktive Instanz SOLL während der Vorbereitung weiterarbeiten können.
5. State Transfer MUSS explizit definiert sein.
6. State-Schema-Kompatibilität MUSS geprüft werden.
7. State Transfer DARF NICHT automatisch Authority übertragen.
8. Capabilities MÜSSEN für die neue Instanz revalidiert werden.
9. Laufende Tasks MÜSSEN kontrolliert behandelt werden.
10. Der Switch MUSS einen eindeutig definierten Übergang besitzen.
11. Neue Requests DÜRFEN nach dem Switch NICHT unkontrolliert zur alten Instanz gelangen.
12. Stabile logische Identitäten SOLLEN Providerwechsel abstrahieren.
13. Die neue Instanz MUSS nach Aktivierung verifiziert werden.
14. Die alte Instanz SOLL bis zur ausreichenden Verifikation verfügbar bleiben.
15. Fehlgeschlagene Live Updates MÜSSEN Rollback, Compensation oder Recovery ermöglichen.
16. Irreversible State-Änderungen MÜSSEN vor dem Switch erkannt werden.
17. Kritische Kernel-Komponenten DÜRFEN nur mit explizit sicherem Live-Update-Modell ersetzt werden.
18. Nicht live aktualisierbare Änderungen MÜSSEN auf einen alternativen Update-Pfad ausweichen.
19. Live Updates MÜSSEN mit dem transaktionalen Update-Modell integrierbar sein.
20. Temporärer Ressourcenmehrbedarf MUSS berücksichtigt werden.
21. Harte Resource Guarantees DÜRFEN durch Live Updates NICHT verletzt werden.
22. Realtime-Garantien MÜSSEN während kritischer Live Updates erhalten bleiben.
23. `Unknown` DARF NICHT als erfolgreicher Update-Zustand gelten.
24. Live-Update-Entscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
25. Live-Update-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-IMMUTABLE-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `ADR-ARCH-0176`

## Ergebnis

```text
Running v1
    │
    ├───────────────→ continues running
    │
Prepare v2
    ↓
Validate
    ↓
Transfer State
    ↓
Revalidate Capabilities
    ↓
Quiesce
    ↓
Atomic Switch
    ↓
Running v2
    ↓
Verify
   ↙   ↘
 OK    Failure
 ↓       ↓
Retire  Rollback /
v1      Recovery
```

NovaOS erhält damit einen kontrollierten Live-Update-Mechanismus, der Komponenten während des laufenden Betriebs ersetzen kann, ohne Sicherheit, State-Konsistenz, Capability-Grenzen oder Recovery-Eigenschaften zugunsten minimaler Ausfallzeit aufzugeben.